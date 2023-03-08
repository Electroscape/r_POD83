# general RPI setup

Screen blanking needs to be disabled: \
`sudo raspi-config` -> display options -> screen blanking

Install a new RPi image (Desktop with recommended pre-installed packages) using RPi imager. It should contain python3.9
which is the minimum version to run the app. Make sure to:

- Change the hostname to an informative one
- Give it a static IP

# setting up crontab
there will be audio problems with hdmi bec RPi4s
https://stackoverflow.com/questions/42497130/audio-doesnt-play-with-crontab-on-raspberry-pi

get username via `id [user_name]`

<code>
XDG_RUNTIME_DIR=/run/user/1000
@reboot sleep 15 && cd pod && ./run.sh
</code>

