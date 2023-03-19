from flask import Flask, render_template, send_from_directory
from flask_socketio import SocketIO
import socketio

from rfid import RFID

app = Flask(__name__)
app.config['SECRET_KEY'] = 'EscapeTerminal#'

# standard Python
sio = socketio.Client()
server_ip = "http://raspi-4-pod-t1:5500"  # <= change server ip here
self_sio = SocketIO(app, cors_allowed_origins="*")

cards = {
    "1": "Triangle.mp4",
    "2": "Circle.mp4",
    "3": "Hexagon.mp4",
    "4": "Dot.mp4",
    "5": "Killswitch.mp4",
    "0": "0.mp4"
}

valid_cards = list(cards.keys())
for c in valid_cards:
    cards[c] = f"static/media/microscope/PD_{cards[c]}"


@app.route("/", methods=["GET", "POST"])
def index():
    return render_template("m_microscope.html", cards=cards, microscope=get_status())


@app.route("/get_status", methods=["GET", "POST"])
def get_status():
    res_dict = nfc_reader.get_data()
    return {
        "show": cards[res_dict["data"]],
        "status": res_dict["status"]
    }


@app.route('/favicon.ico')
def favicon():
    return send_from_directory("static", 'favicon.ico',
                               mimetype='image/vnd.microsoft.icon')


@sio.event
def connect():
    print("Connected to Server!")


@sio.event
def disconnect():
    global connected
    print("microscope is disconnected from server")
    connected = False


@sio.on("rfid_event")
def rfid_updates(data):
    print(f"rfid message: {data}")
    if data.get("cmd") == "status":
        data["status"] = data["message"]
        nfc_reader.set_rfid_status(data["message"])
    elif data.get("cmd") == "sample":
        data["data"] = data["message"]
        nfc_reader.set_rfid_data(data["message"])

    self_sio.emit("microscope_fe", nfc_reader.get_data())


@self_sio.event
def connect():
    print("Self is connected!")


@self_sio.on("msg_to_backend")
def on_msg(data):
    print(f"from frontend: {data} -> forward to server")
    # sio.emit("msg_to_server", data)


print("creating RFID instance")
nfc_reader = RFID(server_ip=server_ip, cards=valid_cards)


def check_for_updates():
    prev_data = nfc_reader.get_data().copy()
    while True:
        self_sio.sleep(2)
        while not nfc_reader.connected:
            print(f"in polling mode {prev_data}")
            if prev_data == nfc_reader.get_data():
                self_sio.sleep(1)
            else:
                prev_data = nfc_reader.get_data().copy()
                print("updates to frontend from polling")
                self_sio.emit("microscope_fe", prev_data)
                print(f"sent: {prev_data}")


def keep_reconnecting():
    global connected

    while True:
        if not connected:
            try:
                # connecting to sio
                sio.connect(server_ip)
                connected = True
            except Exception:
                self_sio.sleep(2)
                print(f"re-try connect to server: {server_ip}")


# polling updates if server is offline
self_sio.start_background_task(check_for_updates)

connected = False
self_sio.start_background_task(keep_reconnecting)

if __name__ == "__main__":
    app.run(debug=True, host='0.0.0.0', port=5555)
