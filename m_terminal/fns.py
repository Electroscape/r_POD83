import os

from flask import json


def js_r(filename: str, auth="", from_static=True) -> dict:
    """
    json read function is used to get the json data from a file and load it to a dict

    :param auth: authenticated user
    :param filename: json file name inside static folder
    :return: dict or None
    """

    if from_static:
        filename = f"static/{filename}"

    try:
        with open(filename, "r") as f_in:
            json_data = json.load(f_in)
            if not filename.startswith("ip_"):
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
        d.update({"id": d.get("link").replace("_", "-")})
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
        {"name": "sample 1",
         "passcode": "1111",
         "elements": [{"iconName": "airbnb", "brandName": "Airbnb", "color": "#fd5c63"},
                      {"iconName": "itunes", "brandName": "iTunes", "color": "#80D8FF"},
                      {"iconName": "soundcloud", "brandName": "SoundCloud", "color": "#ff8800"},
                      {"iconName": "napster", "brandName": "Napster", "color": "#111111"},
                      {"iconName": "imdb", "brandName": "IMDb", "color": "#dba506"}],
         "status": "released",
         "icon": "fa-solid fa-check",
         "info": "toxic"},

        {"name": "sample 2",
         "passcode": "1112",
         "elements": [{"iconName": "stripe", "brandName": "Stripe", "color": "#00afe1"},
                      {"iconName": "node", "brandName": "Node.js", "color": "#68a063"},
                      {"iconName": "sass", "brandName": "Sass", "color": "#cd6799"},
                      {"iconName": "twitter", "brandName": "Twitter", "color": "#1da1f2"},
                      {"iconName": "html5", "brandName": "HTML5", "color": "#e34f26"}],
         "status": "locked",
         "icon": "fas fa-lock",
         "info": "non-toxic"},

        {"name": "sample 3",
         "passcode": "1113",
         "elements": [{"iconName": "meetup", "brandName": "Meetup", "color": "#e0393e"},
                      {"iconName": "digital-ocean", "brandName": "Digital Ocean", "color": "#008bcf"},
                      {"iconName": "java", "brandName": "Java", "color": "#5382a1"},
                      {"iconName": "grunt", "brandName": "Grunt", "color": "#fba919"},
                      {"iconName": "docker", "brandName": "Docker", "color": "#0db7ed"}],
         "status": "locked",
         "icon": "fas fa-lock",
         "info": "non-active"},

        {"name": "sample 4",
         "passcode": "1114",
         "elements": [{"iconName": "bluetooth", "brandName": "Bluetooth", "color": "#3b5998"},
                      {"iconName": "patreon", "brandName": "Patreon", "color": "#f96854"},
                      {"iconName": "firefox", "brandName": "Firefox", "color": "#e66000"},
                      {"iconName": "vuejs", "brandName": "Vue.js", "color": "#42b883"},
                      {"iconName": "laravel", "brandName": "Laravel", "color": "#f55247"}],
         "status": "released",
         "icon": "fas fa-lock-open",
         "info": "toxic"}
    ]
    return samples
