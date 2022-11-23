pkill python
pkill flask
pkill chromium

# shellcheck disable=SC2164
cd ~/r_POD83/m_terminal
source ./venv/bin/activate

sleep 5 &
FLASK_APP=server.py FLASK_DEBUG=true flask run --host 0.0.0.0 --port 5500 &
sleep 5 &
FLASK_APP=ter1.py FLASK_DEBUG=true flask run --host 0.0.0.0 --port 5551 &
sleep 5 &
FLASK_APP=ter2.py FLASK_DEBUG=true flask run --host 0.0.0.0 --port 5552 &
sleep 5 &
FLASK_APP=kiosk.py FLASK_DEBUG=true flask run --host 0.0.0.0 --port 5501 &
sleep 5 &
DISPLAY=:0 /usr/bin/chromium-browser --noerrdialogs --disable-infobars --kiosk 192.168.87.168:5501 &
