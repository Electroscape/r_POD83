from flask import Flask, render_template, send_from_directory

app = Flask(__name__)


@app.route("/", methods=["GET", "POST"])
def index():
    ips = {
        "server": "http://192.168.87.168:5500",
        "ter1": "http://192.168.87.168:5551",
        "ter2": "http://192.168.87.168:5552"
    }
    return render_template("kiosk_tabs.html", ips=ips)


@app.route('/favicon.ico')
def favicon():
    return send_from_directory("static", 'server_favicon.ico',
                               mimetype='image/vnd.microsoft.icon')


if __name__ == "__main__":
    app.run(debug=True, host='0.0.0.0', port=5501)
