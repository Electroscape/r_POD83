# Starte Videodatei
# omxplayer /home/pi/Strukturformel_169.mp4 --loop --no-osd --nodeinterlace --fps 60
# in case of of some format OMX wont do, so for VLC
pkill vlc
pkill python

# Starte Stream
# sudo omxplayer rtsp://TeamEscape:EscapeS2016@192.168.178.27:88/videoMain -b -o hdmi -r &
# vlc /home/pi/Strukturformel_1610.mp4 --loop --fullscreen -r -o hdmi &
cd /home/pi/STB-1/HF_Videotrigger
export DISPLAY=:0.0
cvlc --fullscreen --no-video-title --video-on-top --loop --quiet event_files/usb.mp4 &

# omxplayer /home/pi/Strukturformel_1610.mp4 -b --loop --no-osd --nodeinterlace --fps 60 -o hdmi -r &
# python3 /home/pi/STB-1/HF_Videotrigger/run_video_startup.py &
# python3 /home/pi/STB-1/HF_Videotrigger/sshTrigger.py &
python3 listenToGPIO.py &

# Display Riddle Photo
#sudo fbi --autozoom --noverbose --vt 1 riddle_new.jpg
