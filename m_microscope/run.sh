pkill python
pkill flask
pkill chromium

# shellcheck disable=SC2164
source venv/bin/activate

sleep 5 &

# comment or uncomment to run microscope
FLASK_APP=microscope.py FLASK_DEBUG=true flask run --host 0.0.0.0 --port 5555 &
sleep 5 &

# modify the IP address you want to display
# comment or uncomment to run the browser on the webpage you choose
DISPLAY=:0 /usr/bin/chromium-browser --noerrdialogs --disable-infobars --kiosk localhost:5555 &
