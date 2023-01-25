# This is a sample Python script.


import json
import socketio
from time import sleep
# not a fan of * impports ... amybe we can make classes
from event_mapping import *
import os
import logging
from gpio_fncs import *
# standard Python would be python-socketIo

# GPIO.add_event_detect(data.gpio, GPIO.RISING, callback=callback, bouncetime=20)
#

'''
@TODO: 
    * expection handling
    * cooldowns, need to consider what time library
    * resettimes for gpio output
    * handling of multiple IO reading the same pulldown sensor via level shifter 
    * gpio callback from fe event
    * gpio cooldowns
    * map fe events to events
    * Caught exception socket.error : Already connected
'''

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
    def __init__(self, server_add, sound_server_add):
        self.server_add = server_add
        self.sound_server_add = sound_server_add


def load_settings():
    try:
        with open('config.json') as json_file:
            cfg = json.loads(json_file.read())
            # event_mapping = cfg["event_mapping"]
            terminal_server_add = cfg["terminal_server_add"]
            sound_server_add = cfg["sound_server_add"]
    except ValueError as e:
        print('failure to read config.json')
        print(e)
        exit()
    return Settings(terminal_server_add, sound_server_add)


@sio.event
def connect():
    print("Connected to Server!")


@sio.event
def disconnect():
    global connected
    connected = False
    sio.disconnect()
    connect()


def usb_boot():
    sio.emit("events", {"username": "tr1", "cmd": "usbBoot", "message": "boot"})
    global usb_booted
    usb_booted = True


'''
@sio.on('*')
def catch_all(event, data):
    print("\n")
    print(event)
    print(data)
    print("\n")
    pass
'''


def handle_event(event_key, event_value=None):
    if event_key is None:
        return
    if event_value is None:
        try:
            event_value = event_map[event_key]
        except KeyError as err:
            print(f"handle_event received invalid key: {event_key}")

    print(f"handling event {event_key}")
    try:
        event_entry = event_value[sound]
        print(f"activating sound: {event_entry}")
        activate_sound(event_entry)
    except KeyError:
        pass

    try:
        pin = event_value[gpio_out]
        print(f"setting output: {pin}")
        GPIO.output(pin, GPIO.LOW)
        # @TODO: add timestamp for reset here or a thread to reset
    except KeyError:
        pass


@sio.on("trigger")
def handle_fe(data):
    print(data)
    event_name = False
    for event in event_map.values():
        try:
            event_name = event[fe_event]
            if event_name == data.get('update'):
                break
        except KeyError:
            pass

    if not event_name:
        return

    activate_sound(event)
    gpio_cb(event)
    activate_sound(event)


# this needs to move to gpio_fncs
def setup_gpios():

    for event_value in event_map.values():
        try:
            pin = event_value[gpio_out]
            GPIO.setup(pin, GPIO.OUT, initial=GPIO.HIGH)
        except KeyError:
            pass
        except Exception as exp:
            print(f"issue setting up GPIO {pin} "
                  f"resulting in {exp} ")
            exit()

        try:
            pin = event_value[gpio_in]
            GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
            # @TODO: callback stuff here?
        except KeyError:
            pass
        except Exception as exp:
            print(f"issue setting up GPIO {pin} "
                  f"resulting in {exp} ")
            exit()


def scan_for_usb():
    return os.path.exists("/dev/sda") and not usb_booted


def scan_gpio_events():
    for event_key in event_map.keys():
        event_value = event_map[event_key]
        try:
            input_pin = event_value[gpio_in]
            # @TODO: cooldown here
            if GPIO.input(input_pin) == GPIO.LOW:
                return event_key
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


def main():
    while True:
        if scan_for_usb():
            usb_boot()
            sleep(8)
        handle_event(scan_gpio_events())


if __name__ == '__main__':
    settings = load_settings()
    setup_gpios()
    connected = connect()
    try:
        main()
    finally:
        GPIO.cleanup()
