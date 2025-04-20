import os
import requests
from urllib.parse import urljoin
import re

# Define the static folder for Flask
download_dir = "static/local_lib"
os.makedirs(download_dir, exist_ok=True)

# Define the font storage directory
font_dir = "static/webfonts"
os.makedirs(font_dir, exist_ok=True)

# Font URLs and their corresponding local filenames
# Font Awesome font files and their URLs
font_mapping = {
    "https://fonts.gstatic.com/s/roboto/v47/KFOMCnqEu92Fr1ME7kSn66aGLdTylUAMQXC89YmC2DPNWuaabVmUiA8.ttf": "roboto-300.ttf",
    "https://fonts.gstatic.com/s/roboto/v47/KFOMCnqEu92Fr1ME7kSn66aGLdTylUAMQXC89YmC2DPNWubEbVmUiA8.ttf": "roboto-400.ttf",
    "https://fonts.gstatic.com/s/roboto/v47/KFOMCnqEu92Fr1ME7kSn66aGLdTylUAMQXC89YmC2DPNWub2bVmUiA8.ttf": "roboto-500.ttf",
    "https://fonts.gstatic.com/s/roboto/v47/KFOMCnqEu92Fr1ME7kSn66aGLdTylUAMQXC89YmC2DPNWuYjalmUiA8.ttf": "roboto-700.ttf",
    "https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/webfonts/fa-solid-900.woff2": "fa-solid-900.woff2",
    "https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/webfonts/fa-solid-900.ttf": "fa-solid-900.ttf",
    "https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/webfonts/fa-regular-400.woff2": "fa-regular-400.woff2",
    "https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/webfonts/fa-regular-400.ttf": "fa-regular-400.ttf",
    "https://manzdev.github.io/twitch-manzdev-bios/assets/perfect-dos-vga437.woff2": "perfect-dos-vga437.woff2"
}


# Dictionary mapping original URLs to new local paths
# URLs and filenames for the required .map files
url_mapping = {
    "https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css": "all.min.css",
    "https://fonts.googleapis.com/css?family=Roboto:300,400,500,700&display=swap": "roboto.css",
    "https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/5.0.0/mdb.min.css": "mdb.min.css",
    "https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/5.0.0/mdb.min.js": "mdb.min.js",
    "https://code.jquery.com/jquery-3.6.1.js": "jquery-3.6.1.js",
    "https://cdnjs.cloudflare.com/ajax/libs/jquery.mask/1.14.16/jquery.mask.min.js": "jquery.mask.min.js",
    "https://ajax.googleapis.com/ajax/libs/jquery/3.6.1/jquery.min.js": "jquery.min.js",
    "https://cdn.socket.io/4.5.0/socket.io.min.js": "socket.io.min.js",
    "https://cdnjs.cloudflare.com/ajax/libs/toastr.js/latest/toastr.min.js": "toastr.min.js",
    "https://cdnjs.cloudflare.com/ajax/libs/toastr.js/latest/toastr.min.css": "toastr.min.css",
    "https://cdnjs.cloudflare.com/ajax/libs/sweetalert/2.1.2/sweetalert.min.js": "sweetalert.min.js",
    "https://cdn.socket.io/4.5.0/socket.io.min.js.map": "socket.io.min.js.map",
    "https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/5.0.0/mdb.min.js.map": "mdb.min.js.map",
    "https://code.jquery.com/jquery-3.6.1.min.map": "jquery-3.6.1.min.map",
    "https://cdnjs.cloudflare.com/ajax/libs/toastr.js/latest/toastr.js.map": "toastr.js.map",
    "https://code.iconify.design/iconify-icon/1.0.5/iconify-icon.min.js": "iconify-icon.min.js",
    "https://stackpath.bootstrapcdn.com/bootstrap/4.1.0/js/bootstrap.min.js": "bootstrap.min.js",
    "https://stackpath.bootstrapcdn.com/bootstrap/4.1.0/js/bootstrap.min.js.map": "bootstrap.min.js.map",
    "https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.0/umd/popper.min.js": "popper.min.js",
    "https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.0/umd/popper.min.js.map": "popper.min.js.map",
    "https://code.jquery.com/ui/1.12.1/jquery-ui.js": "jquery-ui.js",
    "https://cdn.jsdelivr.net/css-toggle-switch/latest/toggle-switch.css": "toggle-switch.css",
    "https://cdn.jsdelivr.net/css-toggle-switch/latest/toggle-switch.css.map": "toggle-switch.css.map",
    "https://cdnjs.cloudflare.com/ajax/libs/jqueryui/1.13.2/themes/dark-hive/jquery-ui.min.css": "jquery-ui.min.css",
    "https://cdnjs.cloudflare.com/ajax/libs/jqueryui/1.13.2/themes/dark-hive/theme.min.css": "theme.min.css",
    "https://cdnjs.cloudflare.com/ajax/libs/three.js/0.150.1/three.min.js": "three.min.js"
}

# Download each file and save it locally
for url, local_path in url_mapping.items():
    try:
        response = requests.get(url, stream=True)
        response.raise_for_status()
        
        # Save the file to static folder
        filename = os.path.join(download_dir, os.path.basename(local_path))
        with open(filename, "wb") as file:
            for chunk in response.iter_content(chunk_size=8192):
                file.write(chunk)
        
        print(f"Downloaded: {filename}")
    except Exception as e:
        print(f"Failed to download {url}: {e}")


# Download font files
for url, filename in font_mapping.items():
    try:
        response = requests.get(url, stream=True)
        response.raise_for_status()

        file_path = os.path.join(font_dir, filename)
        with open(file_path, "wb") as file:
            for chunk in response.iter_content(chunk_size=8192):
                file.write(chunk)
        
        print(f"Downloaded: {file_path}")
    except Exception as e:
        print(f"Failed to download {url}: {e}")


def download_png_images(css_url, save_folder="images"):
    # Ensure the save directory exists
    os.makedirs(save_folder, exist_ok=True)

    # Fetch the CSS content
    response = requests.get(css_url)
    if response.status_code != 200:
        print("Failed to fetch CSS file")
        return

    css_content = response.text

    # Regex pattern to find URLs in CSS
    pattern = re.compile(r'url\(["\']?(images/[^)]+\.png)["\']?\)')

    # Find all matches
    matches = pattern.findall(css_content)

    # Download each image
    for img_path in set(matches):  # Using set to avoid duplicates
        img_url = urljoin(css_url, img_path)
        img_name = os.path.basename(img_path)
        save_path = os.path.join(save_folder, img_name)

        img_response = requests.get(img_url, stream=True)
        if img_response.status_code == 200:
            with open(save_path, 'wb') as img_file:
                for chunk in img_response.iter_content(1024):
                    img_file.write(chunk)
            print(f"Downloaded: {img_name}")
        else:
            print(f"Failed to download: {img_name}")

download_png_images("https://cdnjs.cloudflare.com/ajax/libs/jqueryui/1.13.2/themes/dark-hive/jquery-ui.min.css", download_dir + "/images")