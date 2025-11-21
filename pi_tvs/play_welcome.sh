export DISPLAY=:0.0
pkill vlc

cvlc --fullscreen --no-video-title --video-on-top --play-and-exit --gain=1.0 --quiet welcome.mp4 &
cvlc --fullscreen --no-video-title --video-on-top --play-and-exit --gain=1.0 --quiet --loop biovita_adv.mp4 &


