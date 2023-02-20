from flask import Flask, render_template, send_from_directory, url_for
from flask_socketio import SocketIO
import socketio

from rfid import RFID

app = Flask(__name__)
app.config['SECRET_KEY'] = 'EscapeTerminal#'

# standard Python
sio = socketio.Client()
server_ip = "http://" + "192.168.178.35:5500"
self_sio = SocketIO(app, cors_allowed_origins="*")

cards = {
    "1": "1.png",
    "2": "2.png",
    "3": "3.png",
    "4": "4.png",
    "5": "5.png",
    "0": "0.mp4"
}

valid_cards = cards.keys()


@app.route("/", methods=["GET", "POST"])
def index():
    return render_template("m_microscope.html", microscope=get_status())


@app.route("/get_status", methods=["GET", "POST"])
def get_status():
    return microscope


@app.route('/favicon.ico')
def favicon():
    return send_from_directory("static", 'server_favicon.ico',
                               mimetype='image/vnd.microsoft.icon')


@sio.event
def connect():
    print("Connected to Server!")


@self_sio.event
def connect():
    print("Self is connected!")


microscope = {
    "show": url_for("static", filename="media/microscope/0.mp4"),
    "status": "off"
}

if __name__ == "__main__":
    print("connecting to server")
    sio.connect(server_ip)

    nfc_reader = RFID(valid_cards)

    app.run(debug=True, host='0.0.0.0', port=5555)
