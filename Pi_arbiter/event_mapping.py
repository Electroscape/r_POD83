import requests

'''

GPIO addevent?
sio event?

and then how to handle cooldowns?

RPi 3.3V -> 5V Pulldown on arduino

4 5 6 13 GPIO out
### airlock
 * rel 1 beamerIntro
 * PCF? x Steril 
 * rel 2 beamerDecon
 * PCF? x gateOperation

### laserlock
 * usb-boot -> arbiter -> PCF 0 & sound
 ### Possible to use the same GND here and save on the signal
 * failed boot -> arbiter -> PCF 1 & sound 
 * boot & decon -> arbiter > 
 - rel TX 1-2 -> arbiter -> login & logout
 
 ### terminal 
  * boot cmd -> pcf arduino -> 
 
'''

# http://www.compciv.org/guides/python/fundamentals/dictionaries-overview/
# defaults?


def cb_test():
    print("callback stuff")

test_name = "airlock_intro"

event_map = {
    "airlock_intro": {
        # if several pins are used we need a state aswell
        "gpio_pins": [],
        "cb_fn": cb_test(),
        "sound_cb": {
            "is_fx": True,
            "id": 7
        }
    },
    "airlock_sterilisation": {
        "sound_cb": {
            "gpio_pins": [],
            "is_fx": True,
            "id": 8
        }
    },
    "airlock_decon": {
        "gpio_pins": [],
        "sound_cb": {
            "id": 2
        }
    },
    "airlock_wrong": {
        "gpio_pins": [],
        "sound_cb": {
            "is_fx": True,
            "id": 1
        }
    },
    "usb_boot": {
        "sound_cb": {
            "is_fx": False,
            "id": 2
        },
        "socket_cb": {

        }
    }
}


def handle_event(event_dict):
    try:
        sound_cb = event_dict["sound_cb"]
        activate_sound(sound_cb)
    except KeyError:
        pass
    try:
        socket_cb = event_dict["socket_cb"]
    except KeyError:
        pass


def activate_sound(sound_cb):
    payload = dict(ip="192.168.178.172")

    try:
        sound_id = sound_cb["id"]
        if sound_cb["is_fx"].is_fx:
            payload["fx_id"] = sound_id
        else:
            payload["group_id"] = sound_id
    except KeyError:
        pass

    ret = requests.post("http://POD-ITX/AudioInterface.php", payload)
    print(ret)




'''
class Events:
    def __init__(self):
        self.airlock_video = "airlock_video"
        self.airlock_wrong = "airlock_wrong"
        self.airlock_sterilisation = "airlock_sterilisation"
        self.airlock_decon = ""
'''

# @TODO: pack eventnames into class/dict?
sound_events = {
    "airlock_video": SoundEvent(7),
    "airlock_sterilisation": SoundEvent(8),
    "airlock_decon": SoundEvent(2),
    "airlock_wrong": SoundEvent(1),

    "usb_boot": SoundEvent(2, False)
}






