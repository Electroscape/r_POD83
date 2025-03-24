import os
import requests

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
}


# Dictionary mapping original URLs to new local paths
# URLs and filenames for the required .map files
url_mapping = {
    "https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css": "static/local_lib/all.min.css",
    "https://fonts.googleapis.com/css?family=Roboto:300,400,500,700&display=swap": "static/local_lib/roboto.css",
    "https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/5.0.0/mdb.min.css": "static/local_lib/mdb.min.css",
    "https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/5.0.0/mdb.min.js": "static/local_lib/mdb.min.js",
    "https://code.jquery.com/jquery-3.6.1.js": "static/local_lib/jquery-3.6.1.js",
    "https://ajax.googleapis.com/ajax/libs/jquery/3.6.1/jquery.min.js": "static/local_lib/jquery.min.js",
    "https://cdn.socket.io/4.5.0/socket.io.min.js": "static/local_lib/socket.io.min.js",
    "https://cdnjs.cloudflare.com/ajax/libs/toastr.js/latest/toastr.min.js": "static/local_lib/toastr.min.js",
    "https://cdnjs.cloudflare.com/ajax/libs/toastr.js/latest/toastr.min.css": "static/local_lib/toastr.min.css",
    "https://cdnjs.cloudflare.com/ajax/libs/sweetalert/2.1.2/sweetalert.min.js": "static/local_lib/sweetalert.min.js",
    "https://cdn.socket.io/4.5.0/socket.io.min.js.map": "socket.io.min.js.map",
    "https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/5.0.0/mdb.min.js.map": "mdb.min.js.map",
    "https://code.jquery.com/jquery-3.6.1.min.map": "jquery-3.6.1.min.map",
    "https://cdnjs.cloudflare.com/ajax/libs/toastr.js/latest/toastr.js.map": "toastr.js.map",
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