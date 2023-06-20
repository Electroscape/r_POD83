export DISPLAY=:0.0
pkill vlc

echo "video played, back to stream, bye"
cvlc --fullscreen --no-video-title --video-on-top --play-and-exit --gain=1.0 --quiet biovita_adv.mp4  &&
./play_camera_feed.sh



