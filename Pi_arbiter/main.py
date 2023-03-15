# @author Martin Pek (martin.pek@web.de)

import time

import json
import socketio
# not a fan of * impports ... amybe we can make classes
from event_mapping import *
import os
import logging
from ArbiterIO import ArbiterIO, CooldownHandler
# standard Python would be python-socketIo
from time import sleep, thread_time
from subprocess import Popen
from communication.TESocketServer import TESocketServer

IO = ArbiterIO()

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
    * 🔲 \/ make one event capable to trigger multiple outputs via PCF and sound 
    * 🔲 FE multiple CBs 
    * 🔲 mutiple pcf_outs on events
'''

sio = socketio.Client()
cooldowns = CooldownHandler()

gpio_thread = None
# used to prevent multiple boots
usb_booted = False
connected = False


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
    # maybe move this to the dict and handleevent
    sleep(20)
    print("sending usb_boot video trigger")
    nw_sock.transmit("usb_boot")
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
    if event_value is None:
        try:
            event_value = event_map[event_key]
        except KeyError:
            print(f"handle_event received invalid key: {event_key}")
            return

    if not event_value.get(event_condition, lambda: True)():
        # print(f"conditions not fullfilled {event_key}")
        return

    # Start Video before Sound
    event_value.get(event_script, lambda: 'Invalid')()
    sleep(event_value.get(event_delay, 0))

    # Sound, may be moved to a fnc
    print(f"handling event {event_key}")
    print(event_value)
    try:
        event_entry = event_value[sound]
        print(f"activating sound: {event_entry}")
        activate_sound(event_entry)
    except KeyError:
        pass

    # IO pins
    try:
        pcf_no = event_value[pcf_out_add]
        value = event_value[pcf_out]
        print(f"setting output: PCF={pcf_no} Value={value}")
        IO.write_pcf(pcf_no, value)
        print("sleeping... ")
        sleep(3)
        IO.write_pcf(pcf_no, 0)
    except KeyError as err:
        print(err)
        pass

    # Frontend
    cb_dict = event_value.get(fe_cb, False)
    if not cb_dict:
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
                pcf_value = event_map["laserlock_cable_fixed"][pcf_in]
                pcf_add = event_map["laserlock_cable_fixed"][pcf_in_add]
                if IO.read_pcf(pcf_add) & pcf_value == pcf_value:
                    handle_event("laserlock_bootdecon")
                else:
                    handle_event("laserlock_fail")
                return

            handle_event(key)
        except KeyError:
            pass


def scan_for_usb():
    return os.path.exists("/dev/sda") and not usb_booted


def handle_pcf_input(input_pcf, value):

    # local version so we can have one PCF input potentially trigger multiple events
    temporary_cooldowns = set()

    rejected = True

    for event_key, event_dict in event_map.items():

        try:
            if event_dict.get(pcf_in_add) != input_pcf:
                continue
            event_pcf_value = event_dict.get(pcf_in, -1)

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

        except KeyError:
            continue

    if rejected and input_pcf == 4:
        print(f"\n\nInvalid PCF input\n PCFNo {input_pcf} value {value}\n\n")
    cooldowns.cooldowns.update(temporary_cooldowns)


def handle_pcf_inputs():
    active_inputs = IO.get_inputs()
    for active in active_inputs:
        handle_pcf_input(*active)


def connect():
    while True:
        try:
            sio.connect(settings.server_add)
            return True
        except socketio.exceptions.ConnectionError as exc:
            print(f'Caught exception socket.error : {exc}')
            sleep(1)


def main():
    # handle_event("laserlock_bootdecon")
    while True:
        # blank_screen_pid.kill()
        # handle_event("airlock_intro")
        # handle_event("laserlock_bootdecon")
        # handle_event("laserlock_fail")
        # continue
        # exit()
        if scan_for_usb():
            usb_boot()
        handle_pcf_inputs()


if __name__ == '__main__':
    settings = load_settings()
    connected = connect()
    # otherwise calling an already running atmo does not work
    handle_event("reset_atmo")
    # used to trigger rpis via regular network for videos
    nw_sock = TESocketServer(12345)

    print("\n\n=== Arbiter setup complete! ===\n\n")

    try:
        main()
    except KeyboardInterrupt:
        pass
    finally:
        sio.disconnect()
