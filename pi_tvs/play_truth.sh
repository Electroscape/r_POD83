export DISPLAY=:0.0
pkill vlc

cvlc --fullscreen --no-video-title --video-on-top --play-and-exit --quiet The_Truth.mp4 &
cvlc --no-embedded-video --fullscreen --no-video-title --video-wallpaper --loop --quiet black_screen.jpg &


