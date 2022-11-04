pkill python
pkill chromium

cd ~/r_POD83/m_terminal
source ./venv/bin/activate

FLASK_APP=server.py FLASK_DEBUG=true flask run --host 0.0.0.0 --port 5500 &
FLASK_APP=ter1.py FLASK_DEBUG=true flask run --host 0.0.0.0 --port 5551 &
FLASK_APP=ter2.py FLASK_DEBUG=true flask run --host 0.0.0.0 --port 5552 &
FLASK_APP=kiosk.py FLASK_DEBUG=true flask run --host 0.0.0.0 --port 5501 &
DISPLAY=:0 /usr/bin/chromium-browser --noerrdialogs --disable-infobars --kiosk 192.168.87.168:5501 &
