from flask import Flask, render_template, request, redirect, url_for, send_from_directory
from flask_socketio import SocketIO
import json

# Next two lines are for the issue: https://github.com/miguelgrinberg/python-engineio/issues/142
from engineio.payload import Payload
from ring_list import RingList

chat_history = RingList(100)
chat_history.append('Welcome to the server window')


def read_json(filename: str) -> dict:
    """
    json read function is used to get the json data from a file and load it to a dict

    :param filename: json file name inside static folder
    :return: dict or None
    """
    try:
        with open(f"static/{filename}", "r") as f_in:
            json_data = json.load(f_in)
        return json_data

    except IOError:
        return {}


Payload.max_decode_packets = 200

app = Flask(__name__)
app.config['SECRET_KEY'] = 'EscapeTerminal#'

config = read_json(f"json/server_config.json")
app.jinja_env.globals['G_CONFIG'] = config

all_cors = [f"http://{ip}:{port}" for ip in config["ip"].values() for port in config["port"].values()]
all_cors.append('*')
sio = SocketIO(app, cors_allowed_origins=all_cors)


@app.route("/", methods=["GET", "POST"])
def index():
    # ip_address = request.remote_addr
    # print("Requester IP: " + ip_address)
    return render_template("server_home.html", g_config=config, chat_msg=chat_history.get())


@app.route("/get_globals", methods=["GET", "POST"])
def get_globals():
    return read_json(f"json/server_config.json")


@sio.on("connect")
def on_connect():
    print("New socket connected")


@sio.on("disconnect")
def vid_on_disconnect():
    sid = "request.sid"
    display_name = "display name here"

    print("Member left: {}<{}>".format(display_name, sid))

    sio.emit('response_to_fe', {
        "update": f"user {sid} left"
    })


def frontend_server_messages(json_msg):
    chat_history.append(json_msg)
    sio.emit('response_to_fe', json_msg)


@sio.on('msg_to_server')
def handle_received_messages(json_msg):
    print('server received message: ' + str(json_msg))

    # if message is command
    if json_msg.get("cmd") == "auth":
        authenticate_usr(json_msg)

    # send back whatever received
    sio.emit('response_to_terminals', json_msg)
    # send it back to frontend
    frontend_server_messages(json_msg)


@sio.on('auth_usr')
def authenticate_usr(msg):
    sio.emit("login_msg", msg)
    frontend_server_messages(msg)


@sio.on('airlock')
def airlock_updates(msg):
    sio.emit("airlock_updates", msg["message"])
    frontend_server_messages(msg)


@sio.on('usbBoot')
def usb_boot(msg):
    sio.emit("usb_boot", msg["message"])
    frontend_server_messages(msg)


@app.route('/favicon.ico')
def favicon():
    return send_from_directory("static", 'server_favicon.ico',
                               mimetype='image/vnd.microsoft.icon')


if __name__ == "__main__":
    sio.run(app, debug=False, host='0.0.0.0', port=5500)
    # app.run(debug=True, host='0.0.0.0', port=5500)
