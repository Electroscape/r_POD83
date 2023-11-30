import os
import sys

import requests

file_dir = os.path.dirname(__file__)
sys.path.append(file_dir)

from flask import request, Flask, render_template, send_from_directory, url_for, redirect
from flask_flatpages import FlatPages
from flask_socketio import SocketIO

from ring_list import RingList
from fns import js_r, get_progressbar_status, listdir_no_hidden, configure_btn
from pages import app_pages, get_login_user, get_version
import socketio
import logging
from datetime import datetime as dt

now = dt.now()
log_name = now.strftime("logs/T1 %m_%d_%Y  %H_%M_%S.log")
logging.basicConfig(filename=log_name, level=logging.ERROR,
                    format=f'%(asctime)s %(levelname)s %(name)s %(threadName)s : %(message)s')

app = Flask(__name__)
app.config['SECRET_KEY'] = 'EscapeTerminal#'

# standard Python
# reconnection=False
# sio = socketio.Client()
# self_sio = SocketIO(app, cors_allowed_origins="*", ping_timeout=60, ping_interval=5)

ip_conf = js_r("ip_config.json", from_static=False, add_buttons=False)

# Configuration Constants
server_ip = "http://" + ip_conf["server"]
g_lang = "de"  # first run starts in English
terminal_name = "TR1"  # which config file to load

FLATPAGES_EXTENSION = '.md'
FLATPAGES_ROOT = 'content'


class StatusVars:
    def __init__(self):
        self.laserlock_completed = False


game_status = StatusVars()


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
    if game_status.laserlock_completed and request.method == "GET":
        pass
        # return redirect('/')
    global laserlock_boot
    global laserlock_auth

    if request.method == "POST":
        if request.form.get("status"):
            laserlock_boot = request.form.get("status")
            logging.info(f"laserlock boot: {laserlock_boot}")
            return laserlock_boot
        elif request.form.get("auth"):
            laserlock_auth = request.form.get("auth")
            logging.info(f"laserlock auth: {laserlock_auth}")
            if laserlock_auth == "success":
                game_status.laserlock_completed = True
            print(laserlock_auth)
            return laserlock_auth

    config = {
        "title": "Lab Control",
        "boot": laserlock_boot,
        "auth": laserlock_auth,
        "version": get_version(terminal_name).get("laserlock")
    }
    logging.info("open lab page")
    html_path = f'{terminal_name}/p_lab.html'
    return render_template(html_path, g_config=config)


@app.route('/', methods=['GET', 'POST'])
def entry_point():  # begin of the code
    config = {
        "title": "DPL - A",
    }
    return render_template("TR1/p_cctv.html", g_config=config, cams=ip_conf["cams"])


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

    if show_personal_r == "show" and login_user == "rachel":
        tmp = {
            "title": "Personal R.",
            "details": "personal files of rachel",
            "image": "static/imgs/home/information.png",
            "link": "personal_rachel",
            "auth": False
        }
        if g_lang.lower() == "de":
            tmp["title"] = "PRIVAT R."

        g_config["btns"].append(configure_btn(tmp))
        logging.debug(g_config)

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



login_user = get_login_user(terminal_name)  # either David, Rachel or empty string
laserlock_boot = "normal"
laserlock_auth = "normal"
usb_boot = "shutdown"
samples_flag = "unsolved"
show_personal_r = "hide"

app.register_blueprint(app_pages)
flatpages = FlatPages(app)
app.config.from_object(__name__)

if __name__ == "__main__":
    # sio is socketio.Client(), it has no run method
    app.run(debug=True, host='0.0.0.0', port=5551)
