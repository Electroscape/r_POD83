# This is a sample Python script.


import json
import socketio
from time import sleep
from event_mapping import *
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
# used to prevent multiple boots
usb_booted = False

'''
class event:
    def __init__(self, name, gpio_trigger, gpio_out, sock):
        self.name = name
'''

usb_in = 5
usb_out = 7


class Settings:
    def __init__(self, event_mapping, server_add, sound_server_add):
        # self.pin_mapping = _pin_mapping
        self.event_mapping = event_mapping
        self.server_add = server_add
        self.output_GPIOs = [usb_out]
        self.input_GPIOs = [usb_in]
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


def setup_gpio_callbacks():
    # GPIO.add_event_callback(channel, my_callback, bouncetime=200)
    print()


def usb_boot():
    # sio.emit('tr1', 'boot')
    handle_event(event_map, 'usb_boot')
    sio.emit('usbBoot', {'user_name': 'tr1', 'cmd': 'usbBoot', 'message': 'boot'})
    # sio.emit('usbBoot', {'user_name': 'tr1', 'cmd': 'usbBoot', 'message': 'boot'})
    # GPIO.output(usb_out, GPIO.LOW)
    # global usb_booted
    # usb_booted = True


class InputGroup:
    def __init__(self, pins):
        self.pins = pins

    def read_binary_inputs(self):
        result = 0
        for i, pin in enumerate(self.pins):
            if GPIO.input(pin):
                result += 1 << i
        return result


@sio.on('usb_boot')
def airlock_intro(msg):
    print(msg)
    # sound_events["airlock_video"]


def setup_gpios():
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)

    for value in event_map.values():
        try:
            for pin in value['gpio_out']:
                GPIO.setup(pin, GPIO.OUT, initial=GPIO.HIGH)
        except KeyError:
            pass

        try:
            for pin in value['gpio_in']:
                GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
                # callback stuff here
        except KeyError:
            pass


def scan_for_usb():
    return os.path.exists("/dev/sda")


def main():
    while True:
        if scan_for_usb():
            usb_boot()
            sleep(20)
        # GPIO scanning
        # socketio


def handle_event(event_dict, event_name):
    print(f"handling event {event_name}")
    try:
        event = event_dict[event_name]
    except KeyError:
        return False

    try:
        sound_cb = event["sound_cb"]
        print(f"activating sound: {sound_cb}")
        activate_sound(sound_cb)
    except KeyError:
        pass

    try:
        pins = event["gpio_out"]
        for pin in pins:
            print(f"setting output: {pin}")
            GPIO.output(pin, GPIO.LOW)
    except KeyError:
        pass


def connect():
    while True:
        try:
            sio.connect(settings.server_add)
            return True
        except socketio.exceptions.ConnectionError as exc:
            print(f'Caught exception socket.error : {exc}')
            sleep(1)


if __name__ == '__main__':
    settings = load_settings()
    setup_gpios()

    connected = connect()

    sleep(5)
    usb_boot()
    exit()
    main()
