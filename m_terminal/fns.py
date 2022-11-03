import os

from flask import json
import cv2


def gen_frames():
    '''
    for ip camera use - rtsp://username:password@ip_address:88/videoMain' 
    for local webcam use cv2.VideoCapture(0)
    '''
    # camera = cv2.VideoCapture("rtsp://TeamEscape:*********@192.168.88.21:88/videoMain")
    camera = cv2.VideoCapture("rtsp://rtsp.stream/pattern")
    # camera = cv2.VideoCapture(0)

    while True:
        success, frame = camera.read()  # read the camera frame
        if not success:
            break
        else:
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')  # concat frame one by one and show result


def js_r(filename: str, auth="") -> dict:
    """
    json read function is used to get the json data from a file and load it to a dict

    :param auth: authenticated user
    :param filename: json file name inside static folder
    :return: dict or None
    """
    try:
        with open(f"static/{filename}", "r") as f_in:
            json_data = json.load(f_in)
            json_data["btns"] = configure_btns(json_data.get("btns"), auth)
        return json_data

    except IOError:
        return {}


def configure_btns(data: list, auth=""):
    with open("templates/ControlPanel.html", "r") as f_in:
        options_html = f_in.read()

    for d in data:
        # get default html
        txt = options_html
        # create ids
        d.update({"id": d.get("title").replace(" ", "")})
        # change authenticated user
        if d["auth"] and auth == d["auth"].lower():
            d["auth"] = False

        keys = {
            "control.id": d.get("id"),
            "control.image": d.get("image"),
            "control.details": d.get("details"),
            "control.title": d.get("title"),
            "control.link": d.get("link")
        }
        for k, v in keys.items():
            txt = txt.replace(k, v)
        d.update({"html": txt})

    return data


def listdir_no_hidden(path):
    return [f for f in os.listdir(path) if not f.startswith('.')]


def get_samples_status() -> list:
    # read some GPIOs or get status from hardware
    samples = [
        {"name": "virus one",
         "status": "released",
         "icon": "fa-solid fa-check",
         "info": "toxic"},

        {"name": "virus two",
         "status": "locked",
         "icon": "fas fa-lock",
         "info": "non-toxic"},

        {"name": "virus three",
         "status": "locked",
         "icon": "fas fa-lock",
         "info": "non-active"},

        {"name": "virus four",
         "status": "released",
         "icon": "fas fa-lock-open",
         "info": "toxic"},

        {"name": "virus five",
         "status": "locked",
         "icon": "fas fa-lock",
         "info": "nobel"},

        {"name": "virus six",
         "status": "released",
         "icon": "fas fa-lock-open",
         "info": "aggressive"}
    ]
    return samples
