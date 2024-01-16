export DISPLAY=:0.0
pkill vlc

cvlc --fullscreen --loop --no-video-title --video-on-top --play-and-exit --gain=0.0 --quiet biovita_adv.mp4  &