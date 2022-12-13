# This is a sample Python script.


from flask import Flask, request
import json
import socketio

# standard Python would be python-socketIo

# GPIO.add_event_detect(data.gpio, GPIO.RISING, callback=callback, bouncetime=20)
#


try:
    import RPi.GPIO as GPIO

    print("Running on RPi")
except (RuntimeError, ModuleNotFoundError):
    print("Running without GPIOs on non Pi ENV / non RPi.GPIO installed machine")
    # self.settings.is_rpi_env = False
    # GPIO.VERBOSE = False
    from GPIOEmulator.EmulatorGUI import GPIO

server_ip = "http://192.168.87.168:5500"

app = Flask('Pi-Arbiter')
app.config['SECRET_KEY'] = 'gjr39dkjn344_!67#'
sio = socketio.Client()


# asyncio
# sio = socketio.AsyncClient()
gpio_thread = None


def load_settings(self):
    try:
        with open('config.json') as json_file:
            cfg = json.loads(json_file.read())
    except ValueError as e:
        print('failure to read config.json')
        print(e)
        exit()


@sio.event
def connect():
    print("Connected to Server!")


def usb_boot():
    sio.emit("usb_boot", "boot")


def main():
    while True:
        usb_boot()


if __name__ == '__main__':
    sio.connect(server_ip)
    # socketio.run(app, host='0.0.0.0', port=80, debug=False, allow_unsafe_werkzeug=True)
    main()
