#!/bin/bash 

from socket_client import SocketClient
from subprocess import Popen
from time import sleep


sock = SocketClient('raspi-4-pod-arbiter', 12345)
video_dict = {
    "usb_boot": "welcome.mp4",
    "end_rachel_announce": "Rachel_Ending.mp4",
    "end_david_announce": "Rachel_Elancell_Ending.mp4",
    "analyzer_run1": "killswitch.mp4",
}


def start_backgroound():
    return Popen(["cvlc", "black_screen.jpg", "--no-embedded-video", "--fullscreen", "--no-video-title",
                  "--video-wallpaper", "--loop"])


def main():
    video_proc = None

    background = start_backgroound()

    while True:
        msg = sock.read_buffer()
        if not msg:
            continue
        msg = msg[0]
        print(f"received message on socket {msg}")
        if video_proc is not None:
            video_proc.kill()
        try:
            video_name = video_dict[msg]
            background.kill()
            print(f"playing video {video_name}")
            video_proc = Popen(['cvlc', video_name, "--no-embedded-video", "--fullscreen",
                                '--no-video-title', '--video-on-top'])
            if msg == list(video_dict.keys())[0]:
                sleep(67)
                background = start_backgroound()
        except KeyError:
            print(f"unknown key received {msg}")


main()
