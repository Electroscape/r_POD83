export DISPLAY=:0.0
pkill vlc

cvlc --fullscreen --no-video-title --video-on-top --play-and-exit --gain=1.0 --quiet Rachel_Elancell_Ending.mp4 &&
cvlc --fullscreen --no-video-title --video-on-top --play-and-exit --gain=1.0 --quiet elancell_win.mp4 &
cvlc --no-embedded-video --fullscreen --no-video-title --video-wallpaper --quiet --loop black_screen.jpg &
echo "video played, back to blank screen, bye"


