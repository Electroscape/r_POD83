from flask import Blueprint, render_template, redirect, url_for, request
from fns import get_samples_status, listdir_no_hidden, is_unique_users

app_pages = Blueprint('app_pages', __name__, template_folder='templates')


@app_pages.route('/gas_control', methods=['GET', 'POST'])
def gas_control():
    if not is_unique_users():
        return redirect(url_for("app_pages.double_auth_block", src_url=request.path))

    config = {
        "title": "Gas Control",
        "samples": get_samples_status()
    }
    print("open gas page")
    return render_template("TR1/p_gas_control.html", g_config=config)


@app_pages.route('/foscam_control', methods=['GET', 'POST'])
def foscam_control():
    config = {
        "title": "CCTV Cameras"
    }
    print("open CCTV page")
    return render_template("TR1/p_cctv.html", g_config=config)


@app_pages.route('/sample_release', methods=['GET', 'POST'])
def sample_release():
    config = {
        "title": "Samples Status",
        "samples": get_samples_status()
    }
    print("open sample release page")
    return render_template("TR2/p_sample_release.html", g_config=config)


@app_pages.route('/microscope', methods=['GET', 'POST'])
def microscope():
    config = {
        "title": "Microscope"
    }
    print("open microscope page")
    return render_template("TR2/p_microscope.html", g_config=config)


@app_pages.route('/gas_analysis', methods=['GET', 'POST'])
def gas_analysis():
    if not is_unique_users():
        return redirect(url_for("app_pages.double_auth_block", src_url=request.path))

    config = {
        "title": "Gas Analysis",
        "samples": get_samples_status()
    }
    print("open gas analysis page")
    return render_template("TR2/p_gas_analysis.html", g_config=config)


@app_pages.route('/elancell_upload', methods=['GET', 'POST'])
def elancell_upload():
    if not is_unique_users():
        return redirect(url_for("app_pages.double_auth_block", src_url=request.path))

    config = {
        "title": "Upload Results"
    }
    print("open Elancell page")
    return render_template("TR2/p_elancell_upload.html", g_config=config)


@app_pages.route('/media_control', methods=['GET', 'POST'])
def media_control():
    media_files = listdir_no_hidden('static/media')

    config = {
        "title": "Media Gallery",
        "files": media_files
    }
    print("open media page")
    return render_template("TR1/p_media.html", g_config=config)


@app_pages.route('/double_auth', methods=['GET', 'POST'])
def double_auth_block():
    src_url = request.args.get('src_url', "/")
    print(f"src url is: {src_url}")
    config = {
        "title": "Incomplete Auth!"
    }
    if is_unique_users():
        return redirect(src_url)

    return render_template("noAuthFn.html", g_config=config, from_url=src_url)


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
