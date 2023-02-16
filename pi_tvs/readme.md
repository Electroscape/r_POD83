# setting up crontab
there will be audio problems with hdmi bec RPi4s
https://stackoverflow.com/questions/42497130/audio-doesnt-play-with-crontab-on-raspberry-pi

get username via `id [user_name]`

<code>
XDG_RUNTIME_DIR=/run/user/1000
@reboot sleep 15 && cd pod && ./run.sh
</code>

