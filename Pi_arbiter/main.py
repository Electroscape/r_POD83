# This is a sample Python script.


import json
import socketio
from time import sleep, time
from event_mapping import *
import os
import logging
# standard Python would be python-socketIo

# GPIO.add_event_detect(data.gpio, GPIO.RISING, callback=callback, bouncetime=20)
#

'''
@TODO: 
    * expection handling
    * cooldowns, need to consider what time library
    * resettimes for gpio output
    * handling of multiple IO reading the same pulldown sensor via level shifter 

'''

# make this a function?
try:
    import RPi.GPIO as GPIO
    print("Running on RPi")
except (RuntimeError, ModuleNotFoundError):
    print("Running without GPIOs on non Pi ENV / non RPi.GPIO installed machine")
    # self.settings.is_rpi_env = False
    # GPIO.VERBOSE = False
    from GPIOEmulator.EmulatorGUI import GPIO

# GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

# standard Python
sio = socketio.Client()
bundle_input_groups = []

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


class Settings:
    def __init__(self, event_mapping, server_add, sound_server_add):
        # self.pin_mapping = _pin_mapping
        self.event_mapping = event_mapping
        self.server_add = server_add
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


@sio.event
def my_message(data):
    print('message received with ', data)


@sio.event
def disconnect(sid):
    global connected
    connected = False
    connect()


def setup_gpio_callbacks():
    # GPIO.add_event_callback(channel, my_callback, bouncetime=200)
    print()


def usb_boot():
    handle_event(event_map, 'usb_boot')
    sio.emit('usbBoot', {'user_name': 'tr1', 'cmd': 'usbBoot', 'message': 'boot'})
    global usb_booted
    usb_booted = True


# may be moved to a util library so these can be declared in a
# config file
class GPIOBundle:
    def __init__(self, pins, callback_dictionary=None):
        self.__pins = pins
        self.__cooldown = 0
        self.__callback_dictionary = callback_dictionary
        for pin in self.__pins:
            if callback_dictionary is None:
                GPIO.setup(pin, GPIO.OUT, initial=GPIO.HIGH)
            else:
                GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
                bundle_input_groups.append(self)

    def get_value(self):
        sleep(0.01)
        res = 0
        for i, pin in enumerate(self.__pins):
            if GPIO.input(pin) == GPIO.LOW:
                res |= 1 << i
        return res

    def set_value(self, value):
        for i, pin in enumerate(self.__pins):
            if (1 << i) & value > 0:
                GPIO.output(pin, GPIO.LOW)
            else:
                GPIO.output(pin, GPIO.HIGH)

    def has_event(self):
        # doing a really long cooldown here may need to be made a passed parameter or config value
        if time() - self.__cooldown < 10:
            return False
        for pin in self.__pins:
            if GPIO.input(pin) == GPIO.LOW:
                self.__cooldown = time()
                return True
        return False

    def handle(self):
        if not self.has_event():
            return False
        print("event detected")
        value = self.get_value()
        try:
            self.__callback_dictionary[value]()
        except ValueError:
            pass
        except Exception as err:
            print(f"failing to handle GPIObundle due to:\n{err}")

        print(" WIP here")


# laserlock_in_bundle = GPIOBundle([27, 26, 25], GPIO.IN)
def check_gpios():
    for bundle in bundle_input_groups:
        bundle.handle


@sio.on('*')
def catch_all(event, data):
    print("\n")
    print(event)
    print(data)
    print("\n")
    pass


@sio.event
def my_event(sid, data):
    # handle the message
    print("received an event")
    print(sid)
    print(data)


@sio.on('airlock_fe')
def airlock_boot_request(msg):
    print(f"message from frontend {msg}")
    try:
        fixed_connection_pin = event_map["laserlock_bootdecon"]["gpio_in"]
        if GPIO.input(fixed_connection_pin) == GPIO.HIGH:
            handle_event(event_map, "laserlock_fail")
        else:
            handle_event(event_map, "laserlock_bootdecon")
    except KeyError:
        print("airlock_boot_request failed due to keyerror")


# need to specify further since this is not the only gpio setup
def setup_gpios():

    for value in event_map.values():
        try:
            for pin in value['gpio_out']:
                GPIO.setup(pin, GPIO.OUT, initial=GPIO.HIGH)
        except KeyError:
            pass
        except Exception as exp:
            print(f"issue setting up GPIO {pin} "
                  f"resulting in {exp} ")
            exit()

        try:
            for pin in value['gpio_in']:
                GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
                # callback stuff here
        except KeyError:
            pass
        except Exception as exp:
            print(f"issue setting up GPIO {pin} "
                  f"resulting in {exp} ")
            exit()


def scan_for_usb():
    return os.path.exists("/dev/sda") and not usb_booted


def main():
    while True:
        if scan_for_usb():
            usb_boot()
            sleep(20)


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
    GPIO.cleanup()
    try:
        main()
    finally:
        GPIO.cleanup()
