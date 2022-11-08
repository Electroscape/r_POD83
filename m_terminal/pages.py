from flask import Blueprint, render_template, Response
from fns import get_samples_status, listdir_no_hidden
from cam_stream import gen_frames

app_pages = Blueprint('app_pages', __name__, template_folder='templates')


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
        "title": "Samples Status",
        "samples": get_samples_status()
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
        "title": "Upload Results"
    }
    print("open Elancell page")
    return render_template("p_elancell_upload.html", g_config=config)


@app_pages.route('/media_control', methods=['GET', 'POST'])
def media_control():
    media_files = listdir_no_hidden('static/media')

    config = {
        "title": "Media Gallery",
        "files": media_files
    }
    print("open media page")
    return render_template("p_media.html", g_config=config)


@app_pages.route('/video_feed')
def video_feed():
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')


@app_pages.errorhandler(404)
def page_not_found():
    return render_template('page_not_found.html'), 404


@app_pages.after_request
def adding_header_content(head):
    head.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    head.headers["Pragma"] = "no-cache"
    head.headers["Expires"] = "0"
    head.headers['Cache-Control'] = 'public, max-age=0'
    return head


if __name__ == "__main__":
    print("inside pages")
