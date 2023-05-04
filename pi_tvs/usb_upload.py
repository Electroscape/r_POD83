from pathlib import Path
import socketio
from time import sleep

sio = socketio.Client()
rachel_usb_path = Path("/media/2cp/rachel")
elancell_usb_path = Path("/media/2cp/elancell")


@sio.event
def connect():
    print("Connected to Server!")


def main():
    upload_elancell = False
    upload_rachel = False
    while True:
        if rachel_usb_path.exists() and not upload_rachel:
            upload_rachel = True
            sio.emit("events", {"username": "tr2", "cmd": "breach", "message": "breach"})
            print("usb_rachel_enable")
        if upload_rachel and not rachel_usb_path.exists():
            upload_rachel = False
            print("usb_rachel_disable")
            sio.emit("events", {"username": "tr2", "cmd": "breach", "message": "secure"})
        if elancell_usb_path.exists() and not upload_elancell:
            upload_elancell = True
            print("usb_elancell_enable")
            sio.emit("events", {"username": "tr2", "cmd": "elancell", "message": "enable"})
        if upload_elancell and not elancell_usb_path.exists():
            upload_elancell = False
            print("usb_elancell_disable")
            sio.emit("events", {"username": "tr2", "cmd": "elancell", "message": "disable"})


def connect():
    while True:
        try:
            sio.connect("http://192.168.87.180:5500")
            return True
        except socketio.exceptions.ConnectionError as exc:
            print(f'Caught exception socket.error : {exc}')
            sleep(1)


if __name__ == '__main__':
    connected = connect()

    try:
        main()
    except KeyboardInterrupt:
        pass
    finally:
        sio.disconnect()

