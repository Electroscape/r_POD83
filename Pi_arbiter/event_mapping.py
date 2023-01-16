import requests




'''
double post of teh same atmo is not possible

we could connect the cables to both the RPi and Mother

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
  
  
  # @sio.on('usbBoot','boot')
  
  # receive an event would be 
    @sio.on('channel_name')
    def authenticate_usr(msg):
 
'''

# http://www.compciv.org/guides/python/fundamentals/dictionaries-overview/
# defaults?


def cb_test():
    print("callback stuff")


test_name = "airlock_intro"

event_map = {
    "airlock_intro": {
        # if several pins are used we need a state aswell
        "gpio_in": [],
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
        "gpio_out": [4],
        "sound_cb": {
            "is_fx": False,
            "id": 2
        },
    },
    "laserlock_fail": {
        "gpio_out": [5],
        "sound_cb": {
            "id": 3
        }
    },
    "laserlock_bootdecon": {
        "gpio_in": [27],
        "gpio_out": [6],
        "sound_cb": {
            "id": 4
        }
    },
    "laserlock_welcome_david": {
        "gpio_in_bundle": [1],
        "sound_cb": {
            "id": 15
        }
    },
    "laserlock_welcome_rachel": {
        "sound_cb": {
            "id": 16
        }
    },
}


def activate_sound(sound_cb):
    print(sound_cb)
    payload = dict(ip="192.168.178.172")

    try:
        sound_id = sound_cb["id"]
        if sound_cb["is_fx"]:
            payload["fx_id"] = sound_id
        else:
            payload["group_id"] = sound_id
    except KeyError:
        pass

    try:
        ret = requests.post("http://POD-ITX/AudioInterface.php", payload)
        print(ret)
    except OSError as OSE:
        print(f"failed to request sound due to {OSE}")
        exit()


def bundle_laserlock_cb():
    # login users
    return






