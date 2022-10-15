import os
from flask import Blueprint, render_template, send_from_directory, Response
from fns import gen_frames

app_pages = Blueprint('app_pages', __name__, template_folder='templates')


@app_pages.route('/browser', methods=['GET', 'POST'])
def browser():
    config = {
        "title": "Wiki Page"
    }
    print("open wiki page")
    return render_template("p_browser.html", g_config=config)


@app_pages.route('/lab_control', methods=['GET', 'POST'])
def lab_control():
    config = {
        "title": "Lab Control",
        "password": "777"
    }
    print("open lab page")
    return render_template("p_lab.html", g_config=config)


@app_pages.route('/gas_control', methods=['GET', 'POST'])
def gas_control():
    config = {
        "title": "Gas Control"
    }
    print("open gas page")
    return render_template("p_gas_control.html", g_config=config)


@app_pages.route('/foscam_control', methods=['GET', 'POST'])
def foscam_control():
    config = {
        "title": "CCTV Cameras"
    }
    print("open CCTV page")
    return render_template("p_cctv.html", g_config=config)


@app_pages.route('/sample_release', methods=['GET', 'POST'])
def sample_release():
    config = {
        "title": "Samples Management"
    }
    print("open sample release page")
    return render_template("p_sample_release.html", g_config=config)


@app_pages.route('/microscope', methods=['GET', 'POST'])
def microscope():
    config = {
        "title": "Microscope"
    }
    print("open microscope page")
    return render_template("p_microscope.html", g_config=config)


@app_pages.route('/gas_analysis', methods=['GET', 'POST'])
def gas_analysis():
    config = {
        "title": "Gas Analysis"
    }
    print("open gas analysis page")
    return render_template("p_gas_analysis.html", g_config=config)


@app_pages.route('/cleanroom', methods=['GET', 'POST'])
def cleanroom():
    config = {
        "title": "Cleanroom Access"
    }
    print("open browser page")
    return render_template("p_cleanroom.html", g_config=config)


@app_pages.route('/elancell_upload', methods=['GET', 'POST'])
def elancell_upload():
    config = {
        "title": "Upload to Elancell"
    }
    print("open Elancell page")
    return render_template("p_elancell_upload.html", g_config=config)


@app_pages.route('/media_control', methods=['GET', 'POST'])
def media_control():
    media_files = os.listdir('static/media')

    config = {
        "title": "Media Gallery",
        "files": media_files
    }
    print("open media page")
    return render_template("p_media.html", g_config=config)


@app_pages.route('/video_feed')
def video_feed():
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')


@app_pages.route('/favicon.ico')
def favicon():
    return send_from_directory("static", 'favicon.ico',
                               mimetype='image/vnd.microsoft.icon')


@app_pages.errorhandler(404)
def page_not_found():
    return render_template('page_not_found.html'), 404


if __name__ == "__main__":
    print("inside pages")
