import os
import sys

file_dir = os.path.dirname(__file__)
sys.path.append(file_dir)

from flask import Flask, render_template, send_from_directory
from flask_socketio import SocketIO
import json
import logging
from datetime import datetime as dt

# Next two lines are for the issue: https://github.com/miguelgrinberg/python-engineio/issues/142
from engineio.payload import Payload

Payload.max_decode_packets = 200

from ring_list import RingList

chat_history = RingList(100)
chat_history.append('Welcome to the server window')

now = dt.now()
log_name = now.strftime("server %m_%d_%Y  %H_%M_%S.log")
logging.basicConfig(filename=log_name, level=logging.DEBUG,
                    format=f'%(asctime)s %(levelname)s %(name)s %(threadName)s : %(message)s')



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
login_users = {
    "tr1": "empty",
    "tr2": "empty"
}
loading_percent = 0

app = Flask(__name__)
app.config['SECRET_KEY'] = 'EscapeTerminal#'

ip_conf = read_json(f"ip_config.json", from_static=False)

ip_conf = [f"http://{ip}" for ip in ip_conf.values() if isinstance(ip, str)]
all_cors = ip_conf + ['*']

#  engineio_logger=True, for really detailed logs
sio = SocketIO(app, cors_allowed_origins=all_cors,
               ping_timeout=120, ping_interval=20)


@app.route("/", methods=["GET", "POST"])
def index():
    config = {
        "title": "Server Terminal",
        "id": "server",
        "lang": "en"
    }
    # ip_address = request.remote_addr
    # logging.info("Requester IP: " + ip_address)
    return render_template("server_home.html", g_config=config, chat_msg=chat_history.get())


@app.route("/get_globals", methods=["GET", "POST"])
def get_globals():
    return read_json("json/server_config.json")


@app.route("/get_progress", methods=["GET", "POST"])
def get_progress():
    return {"percent": loading_percent}


@app.route('/get_samples', methods=['GET', 'POST'])
def get_samples() -> dict:
    return samples


@app.route('/get_auth_users', methods=['GET', 'POST'])
def get_auth_users() -> dict:
    return login_users


@sio.on("connect")
def on_connect():
    logging.info("New socket connected")


@sio.on("disconnect")
def vid_on_disconnect():
    sid = "request.sid"
    display_name = "display name here"

    logging.debug("Member left: {}<{}>".format(display_name, sid))

    sio.emit('response_to_fe', {
        "update": f"user {sid} left"
    })


def frontend_server_messages(json_msg):
    chat_history.append(json_msg)
    sio.emit('response_to_fe', json_msg)


def all_samples_solved():
    answer = True
    for sample in samples:
        answer = answer and sample.get("status") == "released"
    return answer


@sio.on('msg_to_server')
def handle_received_messages(json_msg):
    logging.info('server received message: ' + str(json_msg))
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
                "message": "dishout"
            }
            sio.emit("trigger", trigger_msg)
            # should I add ~5 sec delay here?
            # after trigger msg and before sending the samples updates msg
            # for the physical world to take place.
            if all_samples_solved():
                sio.emit("samples", {"flag": "done"})

        sio.emit("samples", samples)
    elif json_msg.get("levels") and "correct" in str(json_msg):
        sio.emit("to_clients", {
            "username": "tr2",
            "cmd": "elancell",
            "message": "solved"
        })
    elif "/lab_control" in str(json_msg):
        logging.info("access to laser-lock requested")
        # access the airlock lab
        sio.emit("trigger", {"username": "arb", "cmd": "airlock", "message": "access"})
    elif json_msg.get("cmd") == "cleanroom":
        logging.info(f"cleanroom status: {json_msg.get('message')}")
        # access the cleanroom
        sio.emit("trigger", {"username": "arb", "cmd": "cleanroom", "message": json_msg.get("message")})
        # also update TR2
        sio.emit("to_clients", {"username": "tr2", "cmd": "cleanroom", "message": json_msg.get("message")})
    elif json_msg.get("cmd") == "upload":
        msg = json_msg.get("message")
        if msg in ["elancell", "rachel"]:
            sio.emit("trigger", {"username": "arb", "cmd": "upload", "message": msg})
    else:
        # broadcast chat message
        sio.emit('response_to_terminals', json_msg)
    # send it to frontend
    frontend_server_messages(json_msg)


@sio.on('triggers')
def override_triggers(msg):
    # Display message on frontend chatbox
    frontend_server_messages(msg)
    # print in console for debugging
    logging.info(f"msg to arb pi: sio.on('trigger', {str(msg)})")
    # emit message on "trigger" channel for the arb RPi
    # Therefore listener on the arb Pi is @sio.on("trigger")
    sio.emit("trigger", msg)


@sio.on('rfid_update')
def rfid_updates(msg):
    # Display message on frontend chatbox
    frontend_server_messages(msg)
    # print in console for debugging
    logging.info(f"from microscope: {str(msg)})")
    # emit to microscope flask
    sio.emit("rfid_event", msg)


@sio.on('rfid_extra')
def rfid_extras(msg):
    # Display message on frontend chatbox
    frontend_server_messages(msg)
    # print in console for debugging
    logging.info(f"extra from rfid: {str(msg)})")
    # emit extras
    msg_split = str(msg).split("_")
    if len(msg_split) == 2:
        global login_users

        sio.emit("to_clients", {
            "username": msg_split[0],
            "cmd": "auth",
            "message": msg_split[1]
        })
        # update backend
        login_users[msg_split[0]] = msg_split[1]


@sio.on('events')
def events_handler(msg):
    global login_users
    # Filter messages to server
    if msg.get("username") == "server":
        global samples
        global loading_percent

        if msg.get("cmd") == "loadingbar":
            loading_percent = msg.get("message")
            sio.emit("to_clients", {"username": "tr1", "cmd": "loadingbar", "message": loading_percent})
            sio.emit("to_clients", {"username": "tr2", "cmd": "loadingbar", "message": loading_percent})
        elif msg.get("cmd") == "reset":
            samples = read_json("json/samples.json")
            sio.emit("samples", samples)
            sio.emit("samples", {"flag": "unsolved"})  # to reset the flag
            if msg.get("message") == "resetAll":
                logging.info("Server: Reset all")
                # reset global variables
                login_users = {
                    "tr1": "empty",
                    "tr2": "empty"
                }
                # emit default state messages to terminals
                sio.emit("to_clients", {"username": "tr1", "cmd": "auth", "message": "empty"})
                sio.emit("to_clients", {"username": "tr2", "cmd": "auth", "message": "empty"})
                sio.emit("to_clients", {"username": "tr1", "cmd": "usbBoot", "message": "disconnect"})
                sio.emit("to_clients", {"username": "tr1", "cmd": "airlock", "message": "broken"})
                sio.emit("to_clients", {"username": "tr1", "cmd": "airlock_auth", "message": "normal"})
                sio.emit("to_clients", {"username": "tr2", "cmd": "mSwitch", "message": "off"})
                sio.emit("to_clients", {"username": "tr2", "cmd": "elancell", "message": "disable"})
                sio.emit("to_clients", {"username": "tr2", "cmd": "microscope", "message": "0"})
                sio.emit("to_clients", {"username": "tr2", "cmd": "cleanroom", "message": "lock"})
                sio.emit("to_clients", {"username": "tr2", "cmd": "breach", "message": "secure"})

                # set microscope off
    elif msg.get("username") == "mcrp":
        logging.info(f"to microscope: {str(msg)}")
        sio.emit("rfid_event", msg)
    else:
        # Filters commands
        if msg.get("cmd") == "auth":
            login_users[msg.get("username")] = msg.get("message")

        if msg.get("cmd") == "usbBoot":
            loading_percent = 90
            # reset airlock status on boot event
            sio.emit("to_clients", {"username": "tr1", "cmd": "airlock_auth", "message": "normal"})

        sio.emit("to_clients", msg)
    frontend_server_messages(msg)


def loadingbar_timer():
    global loading_percent

    while True:
        # only starts after booting
        if loading_percent > 0:
            sio.emit("to_clients", {"username": "tr1", "cmd": "loadingbar", "message": loading_percent})
            sio.emit("to_clients", {"username": "tr2", "cmd": "loadingbar", "message": loading_percent})
            frontend_server_messages({"username": "tr1/tr2", "cmd": "loadingbar", "message": loading_percent})
            # 90 min / 20 loading bars = 4.5 min = 270 seconds
            # remove 1 progress bar every 270 seconds
            sio.sleep(int(60 * 4.5))
            loading_percent -= 5


progressbar_task = sio.start_background_task(loadingbar_timer)


@app.route('/favicon.ico')
def favicon():
    return send_from_directory("static", 'server_favicon.ico',
                               mimetype='image/vnd.microsoft.icon')


if __name__ == "__main__":
    sio.run(app, debug=True, host='0.0.0.0', port=5500, engineio_logger=True)
    # app.run(debug=True, host='0.0.0.0', port=5500)