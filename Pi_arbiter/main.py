# @author Martin Pek (martin.pek@web.de)

import time

import json
import socketio
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
    * 🔲 expection handling
    * ✅ cooldowns, need to consider what time library
    * 🔲 handling of multiple IO reading the same pulldown sensor via level shifter 
    * ✅ gpio callback from fe event
    * ✅ gpio output cooldowns
    * ✅ gpio input cooldowns
    * ✅ map fe events to events
    * 🔲 Caught exception socket.error : Already connected? -> Try: finally: disco?
    * 🔲 logging
    * ✅ rachel + david
    * 🔲 cable broken needs to be a delta check and broadcast... or a specific fncs
    * 🔲 resettime as param
    * init for GPIO_in_high
'''

# standard Python
sio = socketio.Client()
gpio_input_cooldowns = set()

# asyncio
# sio = socketio.AsyncClient()
gpio_thread = None
# used to prevent multiple boots
usb_booted = False
connected = False

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


'''
@sio.event
def disconnect():
    if not connected:
        return False
    global connected
    connected = False
    sio.disconnect()
    connect()

'''


def usb_boot():
    sio.emit("events", {"username": "tr1", "cmd": "usbBoot", "message": "boot"})
    handle_event("usb_boot")
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
        except KeyError:
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
        # reset_timer([pin])
        sleep(3)
        GPIO.output(pin, GPIO.HIGH)
    except KeyError:
        pass

    print("FE cbs")

    cb_dict = event_value.get(fe_cb, False)
    if not cb_dict:
        return
    cb_tgt = cb_dict.get(fe_cb_tgt, False)
    cb_cmb = cb_dict.get(fe_cb_cmd, False)
    if not cb_cmb or not cb_tgt:
        return
    cb_msg = cb_dict.get(fe_cb_msg, "")
    print(f"sio emitting: {cb_tgt} {cb_cmb} {cb_msg}" )
    sio.emit("events", {"username": cb_tgt, "cmd": cb_cmb, "message": cb_msg})




@sio.on("trigger")
def handle_fe(data):
    print(data)

    # Checking first if the arbiter is teh tgt
    try:
        if not data.get('username') == 'arb':
            return False
    except KeyError:
        return False

    for key, event in event_map.items():
        try:
            # event_name = event[fe_event]
            cmd = event.get(trigger_cmd, False)
            if not cmd or cmd != data.get('cmd'):
                # print(f"wrong command {cmd}")
                continue
            msg = event.get(trigger_msg, False)
            if msg and msg != data.get("message"):
                # print(f"wrong msg {msg}")
                continue
            if key == "laserlock_fail":
                pin = event_map["laserlock_cable_fixed"][gpio_in]
                if GPIO.input(pin) == GPIO.LOW:
                    handle_event("laserlock_bootdecon")
            handle_event(key)
        except KeyError:
            pass


# this needs to move to gpio_fncs
def setup_gpios():

    for event_value in event_map.values():
        try:
            pin = event_value[gpio_out]
            GPIO.setup(pin, GPIO.OUT, initial=GPIO.HIGH)
            GPIO.output(pin, GPIO.HIGH)
        except KeyError:
            pass
        except Exception as exp:
            print(f"issue setting up GPIO {pin} "
                  f"resulting in {exp} ")
            exit()

        try:
            pin = event_value[gpio_in]
            GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
            # @TODO: callback stuff here?
        except KeyError:
            pass
        except Exception as exp:
            print(f"issue setting up GPIO {pin} "
                  f"resulting in {exp} ")
            exit()


def scan_for_usb():
    return os.path.exists("/dev/sda") and not usb_booted


# @Todo: needs to be fixed, thread.time doesnt work on rpi?
def is_gpio_on_cooldown(pin):
    # print(gpio_input_cooldowns)
    for cooldown in gpio_input_cooldowns:
        if pin == cooldown[0]:
            return True
            if cooldown[1] < time.thread_time():
                print()
                # gpio_input_cooldowns.remove(cooldown)
            else:
                print("GPIO is on cooldown")
                return True
    return False



def handle_gpio_events():
    event_pins_cd = []
    for event_key in event_map.keys():
        event_value = event_map[event_key]
        is_low_trigger = True
        try:
            input_pin = event_value[gpio_in]
            # print(f"chacking input: {input_pin}")
        except KeyError:
            try:
                input_pin = event_value[gpio_in_high]
                is_low_trigger = False
            except KeyError:
                continue

        is_low_state = GPIO.input(input_pin) == GPIO.LOW
        if is_low_state and is_low_trigger:
            # print(f"valid trigger at pin {input_pin}")
            if not is_gpio_on_cooldown(input_pin):
                # or not (is_low_trigger and is_low_state):
                handle_event(event_key)
                event_pins_cd.append(input_pin)
    for input_pin in event_pins_cd:
        # time.thread_time() doesnt work on rpi
        gpio_input_cooldowns.add((input_pin, 0 + 5))


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
            # handle_event("laserlock_cable_fixed")
            # sleep(8)
        handle_gpio_events()
        # handle_event("laserlock_bootdecon") # Schwarz
        # handle_event("laserlock_fail") # Grün
        # exit()
        '''
        GPIO.output(4, GPIO.LOW)
        sleep(4)
        GPIO.output(5, GPIO.LOW)
        sleep(4)
        GPIO.output(6, GPIO.LOW)
        sleep(4)

        GPIO.output(6, GPIO.HIGH)
        sleep(4)
        
        '''


        # exit()
        '''

        sleep(3)
        handle_event("laserlock_fail")
        GPIO.output(5, GPIO.HIGH)
        sleep(25)
        handle_event("laserlock_bootdecon")
        sleep(25)
        '''



if __name__ == '__main__':
    settings = load_settings()
    setup_gpios()
    connected = connect()
    # otherwise calling an already running atmo does not work
    handle_event("reset_atmo")

    try:
        main()
    except KeyboardInterrupt:
        pass
    finally:
        GPIO.cleanup()
        sio.disconnect()
