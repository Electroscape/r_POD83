from flask import Flask, render_template, send_from_directory
from flask_socketio import SocketIO
import json

# Next two lines are for the issue: https://github.com/miguelgrinberg/python-engineio/issues/142
from engineio.payload import Payload
from ring_list import RingList

chat_history = RingList(100)
chat_history.append('Welcome to the server window')


def read_json(filename: str, from_static=True) -> dict:
    """
    json read function is used to get the json data from a file and load it to a dict

    :param from_static: (bool) fetch the file from static folder
    :param filename: json file name inside static folder
    :return: dict
    """
    if from_static:
        filename = f"static/{filename}"

    try:
        with open(filename, "r") as f_in:
            json_data = json.load(f_in)
        return json_data

    except IOError:
        return {}


samples = read_json("json/samples.json")
sample_icons = {
    "locked": "fas fa-lock",
    "unlocked": "fas fa-lock-open",
    "released": "fa-solid fa-check"
}

Payload.max_decode_packets = 200

app = Flask(__name__)
app.config['SECRET_KEY'] = 'EscapeTerminal#'

config = read_json(f"json/server_config.json")
ip_conf = read_json(f"ip_config.json", from_static=False)
app.jinja_env.globals['G_CONFIG'] = config

all_cors = [f"http://{ip}:{port}" for ip in config["ip"].values() for port in config["port"].values()]
ip_conf = [f"http://{ip}" for ip in ip_conf.values()]
all_cors.extend(ip_conf)
all_cors.append('*')
sio = SocketIO(app, cors_allowed_origins=all_cors)


@app.route("/", methods=["GET", "POST"])
def index():
    # ip_address = request.remote_addr
    # print("Requester IP: " + ip_address)
    return render_template("server_home.html", g_config=config, chat_msg=chat_history.get())


@app.route("/get_globals", methods=["GET", "POST"])
def get_globals():
    return read_json("json/server_config.json")


@app.route('/get_samples', methods=['GET', 'POST'])
def get_samples_status() -> dict:
    return samples


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
    if json_msg.get("keypad_update"):
        global samples
        # "gas_control keypad 0 wrong"
        cmd = json_msg.get("keypad_update").split()
        if cmd[0].startswith("/gas_control") and cmd[-1] == "correct":
            samples[int(cmd[-2])]["status"] = "unlocked"
            samples[int(cmd[-2])]["icon"] = sample_icons["unlocked"]

        elif cmd[0].startswith("/gas_analysis"):
            if cmd[-1] == "correct":
                samples[int(cmd[-2])]["status"] = "released"
                samples[int(cmd[-2])]["icon"] = sample_icons["released"]
            trigger_msg = {
                "username": "arb",
                "cmd": "dispenser",
                "message": f"{cmd[-2]} {cmd[-1]}"
            }
            sio.emit("trigger", trigger_msg)
            # should I add ~5 sec delay here?
            # after trigger msg and before sending the samples updates msg
            # for the physical world to take place.

        sio.emit("samples", samples)
    elif "/lab_control" in json_msg:
        print("access to laser-lock requested")
        # access the airlock lab
        sio.emit("trigger", {"username": "arb", "cmd": "airlock", "message": "access"})
    else:
        # broadcast chat message
        sio.emit('response_to_terminals', json_msg)
    # send it to frontend
    frontend_server_messages(json_msg)


@sio.on('events')
def events_handler(msg):
    if msg.get("username") == "server":
        if msg.get("cmd") == "reset":
            global samples
            samples = read_json("json/samples.json")
            sio.emit("samples", samples)
    else:
        sio.emit("to_clients", msg)
    frontend_server_messages(msg)


@app.route('/favicon.ico')
def favicon():
    return send_from_directory("static", 'server_favicon.ico',
                               mimetype='image/vnd.microsoft.icon')


if __name__ == "__main__":
    sio.run(app, debug=False, host='0.0.0.0', port=5500)
    # app.run(debug=True, host='0.0.0.0', port=5500)
