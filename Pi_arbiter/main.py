# This is a sample Python script.


import json

import requests
import socketio
from time import sleep

import os
# standard Python would be python-socketIo

# GPIO.add_event_detect(data.gpio, GPIO.RISING, callback=callback, bouncetime=20)
#

'''
@TODO: expection handling

'''


try:
    import RPi.GPIO as GPIO
    print("Running on RPi")
except (RuntimeError, ModuleNotFoundError):
    print("Running without GPIOs on non Pi ENV / non RPi.GPIO installed machine")
    # self.settings.is_rpi_env = False
    # GPIO.VERBOSE = False
    from GPIOEmulator.EmulatorGUI import GPIO


# standard Python
sio = socketio.Client()


# asyncio
# sio = socketio.AsyncClient()
gpio_thread = None

'''
class event:
    def __init__(self, name, gpio_trigger, gpio_out, sock):
        self.name = name
'''

usb_pin = 5


class Settings:
    def __init__(self, event_mapping, server_add, sound_server_add):
        # self.pin_mapping = _pin_mapping
        self.event_mapping = event_mapping
        self.server_add = server_add
        self.output_GPIOs = [usb_pin]
        self.input_GPIOs = [7]
        self.sound_server_add = sound_server_add


def load_settings():
    try:
        with open('config.json') as json_file:
            cfg = json.loads(json_file.read())
            event_mapping = cfg["event_mapping"]
            terminal_server_add = cfg["terminal_server_add"]
            sound_server_add = cfg["sound_server_add"]
    except ValueError as e:
        print('failure to read config.json')
        print(e)
        exit()
    return Settings(event_mapping, terminal_server_add, sound_server_add)


@sio.event
def connect():
    print("Connected to Server!")


def usb_boot():
    # sio.emit("usb_boot", "boot")
    GPIO.output(usb_pin, GPIO.LOW)
    payload = {
        "ip": "192.168.178.172",
        "group_id": 2
    }
    ret = requests.post(settings.sound_server_add, payload)
    print(ret)


def setup_gpios():
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)

    for pin in settings.input_GPIOs:
        GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

    for pin in settings.output_GPIOs:
        GPIO.setup(pin, GPIO.OUT, initial=GPIO.HIGH)


def scan_for_usb():
    return True


def main():
    while True:
        if scan_for_usb():
            usb_boot()
            sleep(20)


if __name__ == '__main__':
    settings = load_settings()
    setup_gpios()
    # usb_boot()
    # exit()
    connected = False
    while not connected:
        try:
            sio.connect(settings.server_add)
            connected = True
        except socketio.exceptions.ConnectionError as exc:
            print(f'Caught exception socket.error : {exc}')
            sleep(1)

    main()
