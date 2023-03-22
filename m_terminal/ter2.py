import os
import sys

file_dir = os.path.dirname(__file__)
sys.path.append(file_dir)

from flask import request, Flask, render_template, send_from_directory
from flask_socketio import SocketIO
from ring_list import RingList
from fns import js_r
from pages import app_pages, get_login_user
import socketio

app = Flask(__name__)
app.config['SECRET_KEY'] = 'EscapeTerminal#'

# standard Python
sio = socketio.Client()
self_sio = SocketIO(app, cors_allowed_origins="*")

ip_conf = js_r("ip_config.json", from_static=False, add_buttons=False)

# Configuration Constants
server_ip = "http://" + ip_conf["server"]
g_lang = "en"  # first run starts in English
terminal_name = "TR2"  # which config file to load


@app.route('/', methods=['GET', 'POST'])
def entry_point():  # begin of the code
    flags = {
        "elancell": elancell_upload,
        "cleanroom": cleanroom
    }
    return render_template("index.html", g_config=get_globals(), flags=flags)


@app.route('/chat_control', methods=['GET', 'POST'])
def chat_control():
    config = {
        "title": "Chat Room",
        "chat_msg": chat_msgs.get()
    }
    print("open chat page")
    return render_template("p_chat.html", g_config=config)


@app.route('/get_globals', methods=['GET', 'POST'])
def get_globals():
    global login_user

    login_user = get_login_user(terminal_name)
    g_config = js_r(f"json/{terminal_name}_config_{g_lang}.json", auth=login_user)
    g_config["lang"] = g_lang
    return g_config


@app.route('/switch_lang', methods=['GET', 'POST'])
def switch_language():
    global g_lang

    if request.method == 'POST':
        req_lang = request.form.get("lang")

        if req_lang or req_lang != g_lang:
            g_lang = req_lang.strip()
            print(f"Switch language to {g_lang}")
            return get_globals()


@app.route('/get_chat', methods=['GET', 'POST'])
def get_chat():
    return chat_msgs.get()


@app.route('/get_elancell', methods=['GET', 'POST'])
def get_elancell():
    return elancell_upload


@app.route('/get_cleanroom', methods=['GET', 'POST'])
def get_cleanroom():
    return cleanroom


@app.route('/microscope', methods=['GET', 'POST'])
def microscope():
    config = {
        "title": "Microscope",
        "src": "http://" + ip_conf["mcrp"]
    }
    print("open microscope page")
    return render_template("TR2/p_microscope.html", g_config=config)


@app.route('/favicon.ico')
def favicon():
    return send_from_directory("static", f'favicon_{terminal_name}.ico',
                               mimetype='image/vnd.microsoft.icon')


@sio.event
def connect():
    print("Connected to Server!")


@self_sio.event
def connect():
    print("Self is connected!")


@sio.event
def connect_error():
    print("The connection to server failed!")


@sio.on('to_clients')
def events_handler(data):
    # filter the message to get the user here!
    if data.get("username") != terminal_name.lower():
        print(f"irrelevant msg: {data}")
        return 0
    else:
        global login_user
        global elancell_upload
        global cleanroom

        msg = data.get("message")

    # Commands
    if data.get("cmd") == "auth":
        login_user = msg
        print(f"login msg: {msg}")
        print(f'{terminal_name} authenticated user is: {login_user}')
        self_sio.emit('usr_auth', {'usr': login_user, 'data': get_globals()})
    elif data.get("cmd") == "elancell":
        elancell_upload = msg
        print(f"elancell msg: {msg}")
        self_sio.emit('elancell_fe', {'data': elancell_upload})
    elif data.get("cmd") == "cleanroom":
        cleanroom = msg
        print(f"cleanroom msg: {msg}")


@sio.on('samples')
def samples_handler(samples):
    self_sio.emit("samples_updates", samples)


@sio.on('response_to_terminals')
def on_message(data):
    chat_msgs.append(data)
    self_sio.emit("response_to_frontend", data)


@self_sio.on("msg_to_backend")
def on_msg(data):
    global elancell_upload

    print(f"from frontend: {data} -> forward to server")
    if data.get("cmd") == "upload":
        elancell_upload = f"done_{data.get('message')}"

    if connected:
        sio.emit("msg_to_server", data)
    else:
        print(f"server is offline! lost data: {data}")


print("connecting to server")
connected = False


@sio.event
def disconnect():
    global connected
    print("Disconnected from server")
    connected = False


def keep_reconnecting():
    global connected

    while True:
        if not connected:
            try:
                # connecting to sio
                sio.connect(server_ip)
                connected = True
            except Exception as e:
                self_sio.sleep(2)
                print(f"re-try connect to server: {server_ip}")


self_sio.start_background_task(keep_reconnecting)

print("Init global variables")
login_user = get_login_user(terminal_name)  # either David, Rachel or empty string
chat_msgs = RingList(100)  # stores chat history max 100 msgs
elancell_upload = "disable"
cleanroom = "lock"

app.register_blueprint(app_pages)

if __name__ == "__main__":
    app.run(debug=True, host='0.0.0.0', port=5552)
