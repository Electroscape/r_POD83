export DISPLAY=:0.0
pkill vlc

cvlc --no-embedded-video --fullscreen --no-video-title --video-wallpaper --quiet --loop black_screen.jpg &
echo "video played, back to blank screen, bye"