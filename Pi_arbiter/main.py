# This is a sample Python script.


from flask import Flask, request
from flask_socketio import SocketIO, emit
import json


try:
    import RPi.GPIO as GPIO

    print("Running on RPi")
except (RuntimeError, ModuleNotFoundError):
    print("Running without GPIOs on non Pi ENV / non RPi.GPIO installed machine")
    # self.settings.is_rpi_env = False
    # GPIO.VERBOSE = False
    from GPIOEmulator.EmulatorGUI import GPIO


app = Flask('Pi-Arbiter')

async_mode = None
socketio = SocketIO(app, async_mode=async_mode)
gpio_thread = None


def load_settings(self):
    try:
        with open('config.json') as json_file:
            cfg = json.loads(json_file.read())
    except ValueError as e:
        print('failure to read config.json')
        print(e)
        exit()


if __name__ == '__main__':
    socketio.emit('riddles_updated', {
        'relays': 'Stuff'}, namespace='/test', broadcast=True)

