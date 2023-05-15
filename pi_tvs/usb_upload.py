from pathlib import Path
import socketio

import logging

logging.basicConfig(filename='tvs.log', level=logging.DEBUG,
                    format=f'%(asctime)s %(levelname)s %(name)s %(threadName)s : %(message)s')


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
        if not sio.connected:
            continue
        try:
            if rachel_usb_path.exists():
                if not upload_rachel:
                    upload_rachel = True
                    sio.emit("events", {"username": "tr2", "cmd": "breach", "message": "breach"})
                    # print("usb_rachel_enable")
            elif upload_rachel:
                upload_rachel = False
                # print("usb_rachel_disable")
                sio.emit("events", {"username": "tr2", "cmd": "breach", "message": "secure"})
            if elancell_usb_path.exists():
                if not upload_elancell:
                    upload_elancell = True
                    # print("usb_elancell_enable")
                    sio.emit("events", {"username": "tr2", "cmd": "elancell", "message": "enable"})
            elif upload_elancell:
                upload_elancell = False
                # print("usb_elancell_disable")
                sio.emit("events", {"username": "tr2", "cmd": "elancell", "message": "disable"})
        except PermissionError as err:
            logging.debug(err)



if __name__ == '__main__':
    try:
        sio.connect("http://192.168.178.179:5500")
    except socketio.exceptions.ConnectionError as exc:
        logging.debug(f'Caught exception socket.error : {exc}')

    try:
        main()
    except KeyboardInterrupt:
        pass
    except Exception as exp:
        logging.error(exp)
    finally:
        sio.disconnect()

