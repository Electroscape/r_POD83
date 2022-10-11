from flask import request, Flask, render_template
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

# Configuration Constants
server_ip = "http://192.168.87.152:5500"
g_lang = "en"  # first run starts in English
terminal_name = "TR2"  # which config file to load


@app.route('/', methods=['GET', 'POST'])
def entry_point():  # begin of the code
    global g_lang

    if request.method == 'POST':
        req_lang = request.form.get("lang")
        if req_lang or req_lang != g_lang:
            g_lang = req_lang.strip()
            print(f"Switch language to {g_lang}")
            conf = get_globals()
            return conf

    return render_template("index.html", g_config=g_config)


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
    global g_config
    g_config = js_r(f"json/{terminal_name}_config_{g_lang}.json", auth=login_user)
    return g_config


@app.route('/get_chat', methods=['GET', 'POST'])
def get_chat():
    return chat_msgs.get()


@sio.event
def connect():
    print("Connected to Server!")


@self_sio.event
def connect():
    print("Self is connected!")


@sio.event
def connect_error():
    print("The connection to server failed!")


@sio.on('login_msg')
def usr_auth(data):
    global login_user
    print(f"Login msg: {data}")

    # filter the message to get the user here!
    if data.get("user_name") == terminal_name.lower():
        login_user = data.get("message")
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
g_config = get_globals()
chat_msgs = RingList(100)  # stores the whole conversation

app.register_blueprint(app_pages)
# app.run(debug=True, host='0.0.0.0', port=5552)
