export DISPLAY=:0.0
pkill vlc

cvlc --fullscreen --no-video-title --video-on-top --no-audio --quiet rtsp://USER:SEKRITPW@192.168.178.184:88/videoMain &


