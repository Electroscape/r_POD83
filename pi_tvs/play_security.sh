export DISPLAY=:0.0
pkill vlc

cvlc --fullscreen --no-video-title --video-on-top --gain=1.0 --play-and-exit --quiet security_breach.mp4 &
cvlc --no-embedded-video --fullscreen --no-video-title --video-wallpaper --quiet --loop black_screen.jpg &
echo "video played, back to blank screen, bye"



