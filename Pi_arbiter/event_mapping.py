import requests
from gpio_fncs import GPIOBundle



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
sound = "sound"
is_fx = "is_fx"
sound_id = "id"

pcf_in = "pcf_in"
pcf_in_add = "pcf_in_add"
pcf_out = "pcf_out"
pcf_out_add = "pcf_out_add"

# event triggered from FE
trigger_cmd = "trigger_cmd"
# may not always be required
trigger_msg = "trigger_msg"
# event triggering FE

fe_cb = "fe_cb"
fe_cb_tgt = "tgt"
fe_cb_cmd = "cmd"
fe_cb_msg = "msg"


test_name = "airlock_intro"


event_map = {
    "airlock_intro": {
        # if several pins are used we need a state aswell
        sound: {
            is_fx: True,
            sound_id: 7
        }
    },
    "airlock_sterilisation": {
        sound: {
            is_fx: True,
            sound_id: 8
        }
    },
    "airlock_decon": {
        sound: {
            sound_id: 2
        }
    },
    "airlock_wrong": {
        sound: {
            is_fx: True,
            sound_id: 1
        }
    },
    "usb_boot": {
        pcf_out_add: 0,
        pcf_out: 1 << 0,
        sound: {
            is_fx: False,
            sound_id: 2
        },
    },
    "laserlock_fail": {
        trigger_cmd: "airlock",
        trigger_msg: "access",
        pcf_out_add: 0,
        pcf_out: 1 << 1,
        sound: {
            sound_id: 3
        }
    },
    "laserlock_cable_fixed": {
        pcf_in_add: 1,
        pcf_in: 1 << 0,
        fe_cb: {
            fe_cb_cmd: "airlock",
            fe_cb_tgt: "tr1",
            fe_cb_msg: "fixed"
        }
    },
    '''
    unused for now
    "laserlock_cable_broken": {
        gpio_in_high: 12,
        fe_cb: {
            fe_cb_cmd: "airlock",
            fe_cb_tgt: "tr1",
            fe_cb_msg: "broken"
        }
    },
    '''
    "laserlock_bootdecon": {
        pcf_out_add: 0,
        pcf_out: 1 << 2,
        sound: {
            sound_id: 4,
        }
    },
    "laserlock_welcome_david": {
        pcf_in_add: 1,
        pcf_in: 17,
        sound: {
            sound_id: 15
        },
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr1",
            fe_cb_msg: "david"
        }
    },
    "laserlock_welcome_rachel": {
        pcf_in_add: 1,
        pcf_in: 18,
        sound: {
            sound_id: 16
        },
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr1",
            fe_cb_msg: "david"
        }
    },
    "laserlock_auth_tr1_david": {
        pcf_in_add: 1,
        pcf_in: 13,
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr2",
            fe_cb_msg: "david"
        }
    },
    "laserlock_auth_tr2_rachel": {
        pcf_in_add: 1,
        pcf_in: 13,
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr2",
            fe_cb_msg: "rachel"
        }
    },
    "laserlock_auth_tr1_rachel": {
        pcf_in_add: 1,
        pcf_in: 16,
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr1",
            fe_cb_msg: "rachel"
        }
    },
    "laserlock_auth_tr2_david": {
        pcf_in_add: 1,
        pcf_in: 16,
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr2",
            fe_cb_msg: "david"
        }
    },
    "laserlock_lockout": {
        pcf_in_add: 1,
        pcf_in: 19
    },
    "reset_atmo": {
        sound: {
            is_fx: False,
            sound_id: -1
        },
    }
}


def activate_sound(event_entry):
    print(event_entry)
    payload = dict(ip="192.168.178.172")

    try:
        sound_id_value = event_entry[sound_id]
        if event_entry.get(is_fx, True):
            payload["fx_id"] = sound_id_value
        else:
            payload["group_id"] = sound_id_value
    except KeyError:
        pass

    try:
        ret = requests.post("http://POD-ITX/AudioInterface.php", payload)
        print(f"send sound payload: {payload}")
        print(ret)
    except OSError as OSE:
        print(f"failed to request sound due to {OSE}")
        exit()






