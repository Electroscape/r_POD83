# @author Martin Pek (martin.pek@web.de)

import json
import socketio
# not a fan of * impports ... amybe we can make classes
from event_mapping import *
import logging
from ArbiterIO import ArbiterIO, CooldownHandler
# standard Python would be python-socketIo
from time import sleep, thread_time
from communication.TESocketServer import TESocketServer
from pathlib import Path
from datetime import datetime as dt, timedelta

time_start = None

IO = ArbiterIO()
usb_live = False

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

sio = socketio.Client()
cooldowns = CooldownHandler()
nw_sock = None

gpio_thread = None
# used to prevent multiple boots
usb_booted = False
connected = False

boot_usb_path = Path("/media/2cp/usb_boot")

reset_gpios_dicts = {}
reset_delta = timedelta(seconds=3)
# used for delayed events
event_shedule = {}

now = dt.now()
log_name = now.strftime("eventlogs/Arbiter events %m_%d_%Y  %H_%M_%S.log")
logging.basicConfig(filename=log_name, level=logging.INFO,
                    format=f'%(asctime)s %(levelname)s : %(message)s')


already_started_events = []


class Settings:
    def __init__(self, server_add, sound_server_add):
        self.server_add = server_add
        self.sound_server_add = sound_server_add


def load_settings():
    try:
        with open('config.json') as json_file:
            cfg = json.loads(json_file.read())
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


def get_event_value(event_key, event_value):
    if event_value is None:
        try:
            event_value = event_map[event_key]
        except KeyError:
            print(f"handle_event received invalid key: {event_key}")
    return event_value


def trigger_event(event_key, event_value=None):

    event_value = get_event_value(event_key, event_value)
    if event_value is None:
        return


    if event_key in timed_events:
        if event_key in already_started_events:
            return
        else:
            already_started_events.append(event_key)


    if event_key == "airlock_intro":
        global time_start
        time_start = dt.now()
    elapsed_time_str = ""

    if time_start is not None:
        event_dt = dt.now() - time_start
        elapsed_time_str = str(event_dt)
    log_msg = f"{elapsed_time_str} {event_key}"
    logging.info(log_msg)

    # Sound, may be moved to a fnc
    print(event_value)
    try:
        event_entry = event_value[sound]
        print(f"activating sound: {event_entry}")
        activate_sound(event_entry)
    except KeyError:
        pass

    # IO pins
    try:
        # @todo: type casting here?
        pcf_no = event_value[pcf_out_add]
        values = event_value[pcf_out]
        print(f"setting outputs: PCF={pcf_no} Value={values}")
        for index in range(min(len(values), len(pcf_no))):
            IO.write_pcf(pcf_no[index], values[index])
            reset_gpios_dicts.update({pcf_no[index]: dt.now() + reset_delta})
    except KeyError as err:
        print(err)
        pass
    try:
        handle_event_fe(event_value, event_key)
    except socketio.exceptions as exp:
        logging.debug(f"error with sending message to FE: {exp}")
    queued_event = event_value.get(event_next_qeued, False)
    if queued_event:
        handle_event(queued_event)


def handle_event(event_key, event_value=None):

    event_value = get_event_value(event_key, event_value)
    if event_value is None:
        return

    try:
        if not event_value.get(event_condition, lambda: True)():
            # print(f"conditions not fullfilled {event_key}")
            return

        # Start Video before Sound
        event_value.get(event_script, lambda *args: 'Invalid')(event_key, nw_sock)
    except TypeError as err:
        print(f"Error with event fnc/condition {err}")
    print(f"handling event {event_key}")

    if event_value.get(event_delay, 0):
        event_shedule.update({event_key: dt.now() + timedelta(seconds=event_value.get(event_delay, 0))})
        return
    trigger_event(event_key, event_value)


def handle_event_fe(event_value, event_key):
    # Otherwise we crash
    if not sio.connected:
        return
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
    

'''
@sio.on('*')
def catch_all(event, sid, *args):
    print(f'catch_all(event={event}, sid={sid}, args={args})')
'''


@sio.on("trigger")
def handle_fe(data):
    try:
        if not data.get('username') == 'arb':
            return False
    except KeyError:
        return False

    print("\n")

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
            handle_event(key)
        except KeyError:
            pass


def handle_usb_events():
    global usb_live
    # one needs to exclude the other, removing it shall also disable said usb func
    if boot_usb_path.exists():
        if usb_live:
            handle_event("usb_boot")
    else:
        usb_live = True


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

    if rejected and input_pcf == laserlock_in_pcf:
        print(f"\n\nInvalid PCF input\n PCFNo {input_pcf} value {value}\n\n")
    cooldowns.cooldowns.update(temporary_cooldowns)


def handle_pcf_inputs(active_inputs):
    for active in active_inputs:
        handle_pcf_input(*active)


# only applies to non binary pin based values
def handle_inverted_events(active_inputs):
    for event_key, event_value in inverted_events.items():
        # print(f"\n chcecking {event_key}:")
        if inverted_triggered(event_value, active_inputs):
            # print(f"attempting event {event_key}")
            handle_event(event_key, event_value)
        # else:
            # print(f"{event_key} not triggered")


def inverted_triggered(event_value, active_inputs):
    pcf_addr = event_value.get(pcf_in_add, -1)
    pcf_value = event_value.get(pcf_in, -1)
    for pcf_input in active_inputs:
        # print(pcf_input)
        if pcf_input[0] == pcf_addr:
            # print(f"{pcf_value} compared {pcf_input[1]}")
            # print(pcf_value & pcf_input[1])
            if pcf_value & pcf_input[1] > 0:
                return False
    return True


def handle_timed_events():
    for event_key, event_value in timed_events.items():
        if timed_trigger(event_value):
            trigger_event(event_key, event_value)


def timed_trigger(event_value):
    event_time = event_value.get(trigger_time)
    if time_start is None:
        return False
    if dt.now() - time_start >= timedelta(minutes=event_time):
        return True
    else:
        return False


def reset_gpios():
    # print(reset_gpios_dicts)
    remove_keys = []
    for pcf_no, reset_time in reset_gpios_dicts.items():
        if reset_time < dt.now():
            # print(f"resetting gpio {pcf_no} {reset_time}")
            IO.write_pcf(pcf_no, 0)
            remove_keys.append(pcf_no)
    for key in remove_keys:
        reset_gpios_dicts.pop(key)


def handle_event_shedule():
    remove_keys = []
    for event_name, event_time in event_shedule.items():
        if event_time < dt.now():
            remove_keys.append(event_name)
            trigger_event(event_name)
    for key in remove_keys:
        event_shedule.pop(key)


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
        handle_usb_events()
        active_inputs = IO.get_inputs()
        handle_pcf_inputs(active_inputs=active_inputs)
        handle_inverted_events(active_inputs)
        reset_gpios()
        handle_event_shedule()

        handle_timed_events()


if __name__ == '__main__':
    settings = load_settings()
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
        sio.disconnect()
