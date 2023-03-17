export DISPLAY=:0.0
pkill vlc

cvlc --fullscreen --no-video-title --video-on-top --play-and-exit --quiet welcome.mp4 &
cvlc --no-embedded-video --fullscreen --no-video-title --video-wallpaper --quiet --loop black_screen.jpg &


