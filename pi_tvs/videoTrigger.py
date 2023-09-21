#!/bin/bash 

from socket_client import SocketClient
from subprocess import Popen
import json

try:
    with open('ip_config.json') as json_file:
        cfg = json.loads(json_file.read())
        arbiter_ip = cfg["arbiter"]
except ValueError as e:
    print('failure to read serial_config.json')
    print(e)
    exit()


sock = SocketClient(arbiter_ip, 12345)
video_dict = {
    "usb_boot": "./play_welcome.sh",
    "end_rachel_announce": "./play_rachel.sh",
    "end_david_announce": "./play_elancell.sh",
    "analyzer_run1": "./play_killswitch.sh",
    "play_truth": "./play_truth.sh",
    "self_check": "./play_welcome.sh",
    "display_security": "./play_camera_feed.sh",
    "display_securityAutomatic": "./play_camera_feed.sh",
    "play_biovita": "./play_biovita.sh",
    "time_announcement": "./time_announcement.sh"
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
            video_proc = Popen(['sh', video_name])
        except KeyError:
            print(f"unknown key received {msg}")


main()
