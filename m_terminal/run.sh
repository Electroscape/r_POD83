pkill python
pkill flask
pkill chromium

# cd to this script dir
# fixes activate venv from crontab 
cd "${0%/*}"
# shellcheck disable=SC2164
source venv/bin/activate

sleep 5 &

# comment or uncomment to run server 
FLASK_APP=server.py flask run --host 0.0.0.0 --port 5500 --no-debugger --no-reload &
sleep 5 &

# comment or uncomment to run terminal 1
FLASK_APP=ter1.py flask run --host 0.0.0.0 --port 5551 --no-debugger --no-reload &
sleep 5 &

# comment or uncomment to run terminal 2
FLASK_APP=ter2.py flask run --host 0.0.0.0 --port 5552 --no-debugger --no-reload &
sleep 5 &

# comment or uncomment to run kiosk tabs
FLASK_APP=kiosk.py flask run --host 0.0.0.0 --port 5501 --no-debugger --no-reload &
sleep 5 &

# modify the IP address you want to display
# comment or uncomment to run the browser on the webpage you choose
DISPLAY=:0 /usr/bin/chromium-browser --noerrdialogs --disable-infobars --kiosk 0.0.0.0:5501 &
