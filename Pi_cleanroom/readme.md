## Add the required Files to event_files Folder

as usb.mp4 <br>
and decon.mp4

## Setup crontab with

`XDG_RUNTIME_DIR=/run/user/1000
@reboot sleep 20 && bash /home/pi/playVideo.sh >> out.txt  2>&1`
