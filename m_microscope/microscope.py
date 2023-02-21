from flask import Flask, render_template, send_from_directory, url_for
from flask_socketio import SocketIO
import socketio

from rfid import RFID

app = Flask(__name__)
app.config['SECRET_KEY'] = 'EscapeTerminal#'

# standard Python
sio = socketio.Client()
server_ip = "http://" + "192.168.178.20:5500"
self_sio = SocketIO(app, cors_allowed_origins="*")

cards = {
    "1": "1.png",
    "2": "2.png",
    "3": "3.png",
    "4": "4.png",
    "5": "5.png",
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
    card_url = cards[nfc_reader.get_data()]
    return {
        "show": card_url,
        "status": "on"
    }


@app.route('/favicon.ico')
def favicon():
    return send_from_directory("static", 'favicon.ico',
                               mimetype='image/vnd.microsoft.icon')


@sio.event
def connect():
    print("Connected to Server!")


@sio.on("rfid_event")
def rfid_updates(data):
    print(f"rfid message: {data}")
    self_sio.emit("microscope_fe", data["data"])


@self_sio.event
def connect():
    print("Self is connected!")


@self_sio.on("msg_to_backend")
def on_msg(data):
    print(f"from frontend: {data} -> forward to server")
    # sio.emit("msg_to_server", data)


# connecting to sio
sio.connect(server_ip)

print("creating RFID instance")
nfc_reader = RFID(server_ip=server_ip, cards=valid_cards)

if __name__ == "__main__":
    app.run(debug=True, host='0.0.0.0', port=5555)
