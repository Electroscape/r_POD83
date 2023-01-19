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
    * map fe events to events

Caught exception socket.error : Already connected
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
    connect()


def usb_boot():
    handle_gpio_event(event_map, 'usb_boot')
    sio.emit('usbBoot', {'user_name': 'tr1', 'cmd': 'usbBoot', 'message': 'boot'})
    global usb_booted
    usb_booted = True


@sio.on('*')
def catch_all(event, data):

    print("\n")
    print(event)
    print(data)
    print("\n")
    pass


def laserlock_decon():
    print()


frontend_cb_map = {
    "/lab_control keypad 0 correct": laserlock_decon()
}


@sio.on('response_to_fe')
def handle_fe(data):
    print(f"message from frontend {data}")
    update = data.get('update')
    print(f"update is: {update}")
    if "/lab_control keypad 0 correct" == update:
        event = event_map["laserlock_bootdecon"]
        activate_sound(event)
        gpio_cb(event)
        activate_sound(event)


    '''
    try:
    fixed_connection_pin = event_map["laserlock_bootdecon"]["gpio_in"]
    if GPIO.input(fixed_connection_pin) == GPIO.HIGH:
    handle_gpio_event(event_map, "laserlock_fail")
    else:
    handle_gpio_event(event_map, "laserlock_bootdecon")
    except KeyError:
    print("airlock_boot_request failed due to keyerror")
    
    '''


# this needs to move to gpio_fncs
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


def handle_gpio_event(event_dict, event_name):
    print(f"handling event {event_name}")
    try:
        event = event_dict[event_name]
    except KeyError:
        return False

    try:
        event_entry = event[sound]
        print(f"activating sound: {event_entry}")
        activate_sound(event_entry)
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
    try:
        main()
    finally:
        GPIO.cleanup()
