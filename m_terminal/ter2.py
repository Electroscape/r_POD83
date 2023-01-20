from flask import request, Flask, render_template, send_from_directory
from flask_socketio import SocketIO
from ring_list import RingList
from fns import js_r
from pages import app_pages
import socketio

app = Flask(__name__)
app.config['SECRET_KEY'] = 'EscapeTerminal#'

# standard Python
sio = socketio.Client()
self_sio = SocketIO(app, cors_allowed_origins="*")

ip_conf = js_r("ip_config.json", from_static=False)

# Configuration Constants
server_ip = "http://" + ip_conf["server"]
g_lang = "en"  # first run starts in English
terminal_name = "TR2"  # which config file to load


@app.route('/', methods=['GET', 'POST'])
def entry_point():  # begin of the code
    return render_template("index.html", g_config=get_globals())


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


@app.route('/cleanroom', methods=['GET', 'POST'])
def cleanroom():
    config = {
        "title": "Cleanroom Access",
        "passcode": "4321"
    }
    print("open browser page")
    return render_template("TR2/p_cleanroom.html", g_config=config)


@app.route('/get_chat', methods=['GET', 'POST'])
def get_chat():
    return chat_msgs.get()


@app.route('/get_elancell', methods=['GET', 'POST'])
def get_elancell():
    return elancell_upload


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

        msg = data.get("message")

    # Commands
    if data.get("cmd") == "auth":
        login_user = msg
        print(f"login msg: {msg}")
        print(f'{terminal_name} authenticated user is: {login_user}')
        self_sio.emit('usr_auth', {'usr': login_user, 'data': get_globals()})


@sio.on('response_to_terminals')
def on_message(data):
    chat_msgs.append(data)
    self_sio.emit("response_to_frontend", data)


@self_sio.on("msg_to_backend")
def on_msg(data):
    print(f"from frontend: {data} -> forward to server")
    sio.emit("msg_to_server", data)


print("connecting to server")
sio.connect(server_ip)

print("Init global variables")
login_user = ""  # either David, Rachel or empty string
chat_msgs = RingList(100)  # stores the whole conversation
elancell_upload = "disable"

app.register_blueprint(app_pages)

if __name__ == "__main__":
    app.run(debug=True, host='0.0.0.0', port=5552)
