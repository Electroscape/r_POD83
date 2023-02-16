from socket_client import SocketClient
from subprocess import Popen


sock = SocketClient('raspi-4-pod-arbiter', 12345)
video_dict = {
    "usb_boot": "welcome.mp4"
}

def main():
    video_proc = None

    background = Popen(["cvlc", "black_screen.jpg", "--no-embedded-video", "--fullscreen", "--no-video-title", "--video-wallpaper"])

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
            print(f"playing video {video_name}")
            video_proc = Popen(['cvlc', video_name,
               "--no-embedded-video", "--fullscreen", '--no-video-title', '--video-on-top'])
        except KeyError:
            print(f"unknown key received {msg}")



main()
