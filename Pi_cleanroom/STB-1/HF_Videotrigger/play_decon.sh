export DISPLAY=:0.0
pkill vlc
pkill omxplayer

cvlc --fullscreen --no-video-title --video-on-top --play-and-exit --quiet --gain=1.0 event_files/decon.mp4 &&
cvlc --fullscreen --no-video-title --video-on-top --loop --quiet event_files/usb.mp4 &

