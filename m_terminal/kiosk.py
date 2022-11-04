from flask import Flask, render_template, send_from_directory

app = Flask(__name__)


@app.route("/", methods=["GET", "POST"])
def index():
    return render_template("kiosk_tabs.html")


@app.route('/favicon.ico')
def favicon():
    return send_from_directory("static", 'server_favicon.ico',
                               mimetype='image/vnd.microsoft.icon')


if __name__ == "__main__":
    app.run(debug=True, host='0.0.0.0', port=5501)
