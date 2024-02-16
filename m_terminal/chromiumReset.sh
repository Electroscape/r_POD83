#!/bin/bash
pkill chromium


# set environment variable to enable sound
XDG_RUNTIME_DIR=/run/user/$(id -u)
export XDG_RUNTIME_DIR

# modify the IP address you want to display
# comment or uncomment to run the browser on the webpage you choose
DISPLAY=:0 /usr/bin/chromium-browser --noerrdialogs --disable-infobars --autoplay-policy=no-user-gesture-required --disable-features=AudioServiceSandbox --kiosk 0.0.0.0:5551 &
