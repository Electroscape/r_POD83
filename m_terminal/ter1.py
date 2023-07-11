import os
import sys

import requests

file_dir = os.path.dirname(__file__)
sys.path.append(file_dir)

from flask import request, Flask, render_template, send_from_directory, url_for, redirect
from flask_flatpages import FlatPages
from flask_socketio import SocketIO

from ring_list import RingList
from fns import js_r, get_progressbar_status, listdir_no_hidden
from pages import app_pages, get_login_user, get_version
import socketio
import logging
from datetime import datetime as dt

now = dt.now()
log_name = now.strftime("T1 %m_%d_%Y  %H_%M_%S.log")
logging.basicConfig(filename=log_name, level=logging.DEBUG,
                    format=f'%(asctime)s %(levelname)s %(name)s %(threadName)s : %(message)s')

app = Flask(__name__)
app.config['SECRET_KEY'] = 'EscapeTerminal#'

# standard Python
# reconnection=False
sio = socketio.Client()
self_sio = SocketIO(app, cors_allowed_origins="*", ping_timeout=60, ping_interval=5)

ip_conf = js_r("ip_config.json", from_static=False, add_buttons=False)

# Configuration Constants
server_ip = "http://" + ip_conf["server"]
g_lang = "en"  # first run starts in English
terminal_name = "TR1"  # which config file to load

FLATPAGES_EXTENSION = '.md'
FLATPAGES_ROOT = 'content'


@app.route("/browser/", methods=['GET', 'POST'])
def browser():
    config = {
        "title": "Wiki Page",
        "lang": g_lang
    }

    # print("open wiki page")
    html_path = f'{terminal_name}/p_browser.html'
    return render_template(html_path, g_config=config, g_lang=g_lang, folders=get_posts())


@app.route('/browser/<post_path>', methods=['GET', 'POST'])
def get_post(post_path):
    config = {
        "title": post_path,
        "lang": g_lang
    }
    locker_version = ""
    logging.info(f"post path: {post_path}")

    # version control of locker images prefix
    if post_path == "locker":
        ver_prefix = get_version(terminal_name).get("media").get("prefix")
        locker_version = sorted(
            [img for img in listdir_no_hidden('static/imgs/locker') if (img.startswith(ver_prefix))])

    post = flatpages.get_or_404(post_path)
    html_path = f'{terminal_name}/post.html'
    return render_template(html_path, g_config=config, post=post, locker_version=locker_version)


@app.route('/lab_control', methods=['GET', 'POST'])
def lab_control():
    global airlock_boot
    global airlock_auth

    if request.method == "POST":
        if request.form.get("status"):
            airlock_boot = request.form.get("status")
            logging.info(f"airlock boot: {airlock_boot}")
            return airlock_boot
        elif request.form.get("auth"):
            airlock_auth = request.form.get("auth")
            logging.info(f"airlock auth: {airlock_auth}")
            return airlock_auth

    config = {
        "title": "Lab Control",
        "boot": airlock_boot,
        "auth": airlock_auth,
        "version": get_version(terminal_name).get("laserlock")
    }
    logging.info("open lab page")
    html_path = f'{terminal_name}/p_lab.html'
    return render_template(html_path, g_config=config)


@app.route('/', methods=['GET', 'POST'])
def entry_point():  # begin of the code
    if usb_boot != "boot":
        return redirect("/boot")

    conf = get_globals()
    airlock_id = "lab-control"
    return render_template("index.html", g_config=conf, airlock=airlock_boot, airlock_id=airlock_id,
                           airlock_auth=airlock_auth, samples_flag=samples_flag, progress=get_progressbar_status())


@app.route('/boot', methods=['GET', 'POST'])
def pre_entry_point():
    if usb_boot == "boot":
        return redirect("/")

    sound_effects = get_version(terminal_name).get("boot_sound")
    return render_template("TR1/boot_up.html", play_boot_sound=sound_effects)


@app.route('/chat_control', methods=['GET', 'POST'])
def chat_control():
    config = {
        "title": "Chat Room",
        "chat_msg": chat_msgs.get()
    }
    logging.info("open chat page")
    return render_template("p_chat.html", g_config=config)


@app.route('/get_globals', methods=['GET', 'POST'])
def get_globals():
    global login_user

    login_user = get_login_user(terminal_name)
    g_config = js_r(f"json/{terminal_name}_config_{g_lang}.json", auth=login_user)
    g_config["lang"] = g_lang
    return g_config


@app.route('/switch_lang', methods=['GET', 'POST'])
def switch_language():
    global g_lang

    if request.method == 'POST':
        req_lang = request.form.get("lang")
        url = request.referrer

        if req_lang or req_lang != g_lang:
            g_lang = req_lang.strip()
            logging.info(f"Switch language to {g_lang}")
            # Switch blog language
            if "browser" in url:
                logging.info("switch lang in wiki")
                return get_posts()

            return get_globals()


def get_posts() -> dict:
    # filter posts by language
    posts = [p for p in flatpages if p.meta["lang"] == "EN/DE" or p.meta["lang"].lower() == g_lang]
    # create list of jsons for posts
    posts_json = []
    for p in posts:
        posts_json.append(
            {
                "html": p.html,
                "title": p.meta["title"],
                "lang": p.meta["lang"],
                "folder": p.meta.get("group_" + g_lang, "browser"),
                "url": url_for('get_post', post_path=p.path),
                "exert": p.body[:100] + "..."
            }
        )

    # group posts by folder name
    folders = {}
    for p in posts_json:
        # first time create key with folder name
        if not folders.get(p["folder"]):
            folders[p["folder"]] = []
        # append post to folder name list
        folders[p["folder"]].append(p)

    sorted_folders = dict(sorted(folders.items()))

    return sorted_folders


@app.route('/foscam_control', methods=['GET', 'POST'])
def foscam_control():
    config = {
        "title": "CCTV Cameras"
    }

    if request.method == 'POST':
        url = request.form.get("data")
        try:
            requests.get(url)
        except TypeError:
            logging.info("cgi ptz command")
        return "success"

    version_config = get_version("TR1").get("foscam")
    config.update(version_config)
    logging.info("open CCTV page")
    return render_template("TR1/p_cctv.html", g_config=config, cams=ip_conf["cams"])


@app.route('/get_chat', methods=['GET', 'POST'])
def get_chat():
    return chat_msgs.get()


@app.route('/favicon.ico')
def favicon():
    return send_from_directory("static", f'favicon_{terminal_name}.ico',
                               mimetype='image/vnd.microsoft.icon')


@sio.event
def connect():
    logging.info("Connected to Server!")


@self_sio.event
def connect():
    logging.info("Self is connected!")


@sio.event
def connect_error(message=None):
    logging.debug(f"The connection to server failed! \n{message}")


@sio.on('to_clients')
def events_handler(data):
    # filter the message to get the user here!
    if data.get("username") != terminal_name.lower():
        logging.info(f"irrelevant msg: {data}")
        return 0
    else:
        global login_user
        global usb_boot
        global airlock_boot
        global airlock_auth

        msg = data.get("message")

    # Commands
    if data.get("cmd") == "auth":
        login_user = msg
        logging.info(f"login msg: {msg}")
        logging.info(f'{terminal_name} authenticated user is: {login_user}')
        self_sio.emit('usr_auth', {'usr': login_user, 'data': get_globals()})
    elif data.get("cmd") == "usbBoot":
        usb_boot = msg
        logging.info(f"boot msg: {msg}")
        self_sio.emit('boot_fe', {'status': usb_boot, 'data': get_globals()})
    # TODO: refactor airlock to laserlock
    elif data.get("cmd") == "airlock":
        airlock_boot = msg
        logging.info(f"laserlock msg: {msg}")
        # only notify if not solved
        if airlock_auth != "success":
            self_sio.emit('airlock_fe', {'status': airlock_boot, 'data': get_globals()})
    elif data.get("cmd") == "airlock_auth":
        airlock_auth = msg
        logging.info(f"airlock auth msg: {msg}")
    elif data.get("cmd") == "loadingbar":
        logging.info(f"set loading bar: {msg}")
        self_sio.emit('loadingbar_fe', msg)


@sio.on('samples')
def samples_handler(samples):
    if isinstance(samples, list):
        self_sio.emit("samples_updates", samples)
    elif isinstance(samples, dict):
        global samples_flag
        samples_flag = samples.get("flag")


@sio.on('response_to_terminals')
def on_message(data):
    # chat messages are unique with the key 'user_name'
    if data.get("user_name"):
        chat_msgs.append(data)
    self_sio.emit("response_to_frontend", data)


@self_sio.on("msg_to_backend")
def on_msg(data):
    logging.info(f"from frontend: {data} -> forward to server")
    if sio.connected:
        sio.emit("msg_to_server", data)
    else:
        logging.debug(f"server is offline! lost data: {data}")


@self_sio.event
def disconnect():
    logging.debug("self is disconnected!")
    # it disconnects when navigate pages
    # raise "SelfDisconnected"


@sio.event
def disconnect():
    logging.debug("Disconnected from server")


while not sio.connected:
    try:
        # connecting to sio
        sio.connect(server_ip)
    except Exception as e:
        logging.debug(f"re-try connect to server: {server_ip}")
        sio.sleep(2)

chat_msgs = RingList(200)  # stores chat history max 200 msgs, declare before starting sockets

login_user = get_login_user(terminal_name)  # either David, Rachel or empty string
airlock_boot = "normal"
airlock_auth = "normal"
usb_boot = "shutdown"
samples_flag = "unsolved"

app.register_blueprint(app_pages)
flatpages = FlatPages(app)
app.config.from_object(__name__)

if __name__ == "__main__":
    # sio is socketio.Client(), it has no run method
    app.run(debug=True, host='0.0.0.0', port=5551)
