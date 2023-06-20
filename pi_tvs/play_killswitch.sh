export DISPLAY=:0.0
pkill vlc

echo "video played, back to stream, bye"
cvlc --fullscreen --no-video-title --video-on-top --play-and-exit --gain=0.9 --quiet killswitch.mp4 &&
./play_camera_feed.sh



