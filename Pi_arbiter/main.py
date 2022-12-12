# This is a sample Python script.


from flask import Flask, request
from flask_socketio import SocketIO, emit
import json
import logging


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
sio = SocketIO.Client()
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
    print("I'm connected!")


def usb_boot():
    sio.emit("usb_boot", "boot")


def main():
    usb_boot()


if __name__ == '__main__':
    main()
