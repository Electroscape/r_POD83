# This is a sample Python script.


import json
import socketio
from time import sleep
from event_mapping import sound_events
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
    # sio.emit("usb_boot", "boot")
    GPIO.output(usb_out, GPIO.LOW)
    global usb_booted
    usb_booted = True


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
def airlock_intro():
    sound_events["airlock_video"]


def setup_gpios():
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)

    for pin in settings.input_GPIOs:
        GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

    for pin in settings.output_GPIOs:
        GPIO.setup(pin, GPIO.OUT, initial=GPIO.HIGH)

    setup_gpio_callbacks()


def scan_for_usb():
    return os.path.exists("/dev/sda")


def main():
    while True:
        if scan_for_usb():
            usb_boot()
            sleep(20)
        # GPIO scanning
        # socketio


if __name__ == '__main__':
    settings = load_settings()
    setup_gpios()
    usb_boot()
    exit()
    connected = False
    while not connected:
        try:
            sio.connect(settings.server_add)
            connected = True
        except socketio.exceptions.ConnectionError as exc:
            print(f'Caught exception socket.error : {exc}')
            sleep(1)

    main()
