#!/bin/bash
pkill python
pkill flask
pkill chromium

# cd to this script dir
# fixes activate venv from crontab
# shellcheck disable=SC2164
cd "${0%/*}"
source venv/bin/activate

# comment or uncomment to run server
FLASK_APP=server.py flask run --host 0.0.0.0 --port 5500 --no-debugger --no-reload &
sleep 10 &&

# comment or uncomment to run terminal 1
FLASK_APP=ter1.py flask run --host 0.0.0.0 --port 5551 --no-debugger --no-reload &

# comment or uncomment to run terminal 2
FLASK_APP=ter2.py flask run --host 0.0.0.0 --port 5552 --no-debugger --no-reload &

# comment or uncomment to run terminal 2
FLASK_APP=ter3.py flask run --host 0.0.0.0 --port 5553 --no-debugger --no-reload &

# comment or uncomment to run kiosk tabs
FLASK_APP=kiosk.py flask run --host 0.0.0.0 --port 5501 --no-debugger --no-reload &

sleep 10 &&

# set environment variable to enable sound
XDG_RUNTIME_DIR=/run/user/$(id -u)
export XDG_RUNTIME_DIR

# modify the IP address you want to display
# comment or uncomment to run the browser on the webpage you choose
DISPLAY=:0 /usr/bin/chromium-browser --noerrdialogs --disable-infobars --autoplay-policy=no-user-gesture-required --disable-features=AudioServiceSandbox --kiosk 0.0.0.0:5501 &
