pkill vlc
# sudo pkill fbi
pkill python

# cd to this script dir
# fixes activate venv from crontab 
cd "${0%/*}"

source venv/bin/activate
python3 usb_upload.py

# sudo fbi -T 1 -noverbose -a black_screen.jpg
export DISPLAY=:0.0
python3 videoTrigger.py
