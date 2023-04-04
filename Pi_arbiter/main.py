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
from time import sleep, thread_time
from subprocess import Popen
from communication.TESocketServer import TESocketServer
from pathlib import Path

# GPIO.add_event_detect(data.gpio, GPIO.RISING, callback=callback, bouncetime=20)
#

'''
@TODO: 
    * 🔲 expection handling
    * ✅ cooldowns, need to consider what time library
    * ✅ handling of multiple IO reading the same 
    * ✅ gpio callback from fe event
    * ✅ gpio output cooldowns
    * ✅ gpio input cooldowns
    * ✅ map fe events to events
    * 🔲 Caught exception socket.error : Already connected? -> Try: finally: disco?
    * 🔲 logging
    * ✅ rachel + david
    * 🔲 cable broken needs to be a delta check and broadcast... or a specific fncs
    * 🔲 resettime as param
    * ✅ designated PCFS as input or output
    * ✅ make a fnc for checking if pin/binary value present in pcf value
    * ✅ \/ make one event capable to trigger multiple outputs via PCF
    * 🔲 FE multiple CBs 
    * 🔲 mutiple pcf_outs on events
'''

# standard Python
sio = socketio.Client()
cooldowns = CooldownHandler()
nw_sock = None

# asyncio
# sio = socketio.AsyncClient()
gpio_thread = None
# used to prevent multiple boots
usb_booted = False
connected = False

boot_usb_path = Path("/media/2cp/elancell")


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


def handle_event(event_key, event_value=None):
    '''
    event first handles CB script then does the delay
    '''
    if event_value is None:
        try:
            event_value = event_map[event_key]
        except KeyError:
            print(f"handle_event received invalid key: {event_key}")

    try:
        if not event_value.get(event_condition, lambda: True)():
            # print(f"conditions not fullfilled {event_key}")
            return

        # Start Video before Sound
        event_value.get(event_script, lambda *args: 'Invalid')(event_key, nw_sock)
    except TypeError as err:
        print(f"Error with event fnc/condition {err}")
    print(f"handling event {event_key}")
    sleep(event_value.get(event_delay, 0))

    # Sound, may be moved to a fnc
    print(event_value)
    try:
        event_entry = event_value[sound]
        print(f"activating sound: {event_entry}")
        activate_sound(event_entry)
    except KeyError:
        pass

    try:
        # @todo: type casting here?
        pcf_no = event_value[pcf_out_add]
        values = event_value[pcf_out]
        print(f"setting outputs: PCF={pcf_no} Value={values}")
        for index in range(min(len(values), len(pcf_no))):
            IO.write_pcf(pcf_no[index], values[index])
        # @todo: this needs to be threaded
        print("sleeping... ")
        sleep(3)
        for index in range(len(pcf_no)):
            IO.write_pcf(pcf_no[index], 0)
    except KeyError as err:
        print(err)
        pass

    handle_event_fe(event_value, event_key)
    queued_event = event_value.get(event_next_qeued, False)
    if queued_event:
        handle_event(queued_event)


def handle_event_fe(event_value, event_key):
    # Frontend
    cb_dict = event_value.get(fe_cb, False)
    if not cb_dict:
        # This way any event can be monitored on the server
        sio.emit("events", {"username": "server", "message": event_key})
        return
    cb_tgt = cb_dict.get(fe_cb_tgt, False)
    cb_cmb = cb_dict.get(fe_cb_cmd, False)
    if not cb_cmb or not cb_tgt:
        return
    cb_msg = cb_dict.get(fe_cb_msg, "")
    print(f"sio emitting: {cb_tgt} {cb_cmb} {cb_msg}\n\n")
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
            # @todo: removed once differentiation is possible
            if key == "laserlock_fail" or key == "laserlock_bootdecon":
                pin = event_map["laserlock_cable_fixed"][gpio_in]
                if False and GPIO.input(pin) == GPIO.LOW:
                    handle_event("laserlock_bootdecon")
                else:
                    handle_event("laserlock_fail")

            handle_event(key)
        except KeyError:
            pass


def handle_usb_events():
    # one needs to exclude the other, removing it shall also disable said usb func
    if not states.usb_booted and boot_usb_path.exists():
        handle_event("usb_boot")

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


    rejected = True

    for event_key, event_dict in event_map.items():


            # checks if all pins to form the value of that event are present on the inputs
            # this way its possible mix and match multiple inputs as single pin inputs and binary
            if input_pcf in binary_pcfs:
                if event_pcf_value == value:
                    # @TODO: consider simply using the eventkeys
                    if not cooldowns.is_input_on_cooldown(input_pcf, event_pcf_value):
                        temporary_cooldowns.add((input_pcf, event_pcf_value, thread_time() + 3))
                        handle_event(event_key)
                        rejected = False
            else:
                if event_pcf_value & value == event_pcf_value:
                    # @TODO: consider simply using the eventkeys
                    if not cooldowns.is_input_on_cooldown(input_pcf, event_pcf_value):
                        temporary_cooldowns.add((input_pcf, event_pcf_value, thread_time() + 3))
                        handle_event(event_key)
                        rejected = False


    if rejected and input_pcf == 4:
        print(f"\n\nInvalid PCF input\n PCFNo {input_pcf} value {value}\n\n")
    cooldowns.cooldowns.update(temporary_cooldowns)

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
        # blank_screen_pid.kill()
        # handle_event("airlock_intro")
        # handle_event("laserlock_bootdecon")
        # handle_event("laserlock_fail")
        # continue
        # exit()
        IO.fix_cleanroom()
        handle_usb_events()
        handle_pcf_inputs()

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
    # used to trigger rpis via regular network for videos
    nw_sock = TESocketServer(12345)
    handle_event("reset_atmo")

    print("\n\n=== Arbiter setup complete! ===\n\n")

    try:
        main()
    except KeyboardInterrupt:
        pass
    finally:
        GPIO.cleanup()
        sio.disconnect()
