export DISPLAY=:0.0
pkill vlc

cvlc --fullscreen --no-video-title --video-on-top --play-and-exit --gain=1.0 --quiet 15_minutes_remaining_DE_EN.mp4 &&

./play_camera_feed.sh