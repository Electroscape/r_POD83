import requests
import subprocess
from enum import Enum

# http://www.compciv.org/guides/python/fundamentals/dictionaries-overview/
# defaults?

# these are the pcf addresses, first 3 are Arbiter -> Brain as outputs
# last 3 are Brain -> Arbiter inputs
# [0x38, 0x39, 0x3A, 0x3C, 0x3D, 0x3E]

#
laserlock_out_pcf = 0
# also used for the cleanroom rigger
airlock_out_pcf = 1
lab_light_out_pcf = 2
# inputs
laserlock_in_pcf = 4
# If we need more inputs this is the prime candidate to consolidate with the above
laserlock_in_2_pcf = 3
airlock_in_pcf = 5
analyzer_in_pcf = laserlock_in_2_pcf

sound = "sound"
is_fx = "is_fx"
sound_id = "id"

pcf_in = "pcf_in"
# if a specific pin must be set to high in order to trigger, only works on non-binary
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

event_script = "script"
event_condition = "condition"
event_delay = "delay"


class AirlockOut(Enum):
    rachel_announce, rachel_end, david_end = range(1, 4)


laserlock_io_isSeperation = 16
laserlock_io_david = 32
laserlock_io_rachel = 64
laserlock_io_seperationEnd = 128

labLight_trigger = "labLight"
lab_light_off = 1
lab_light_white = 2
lab_light_standby = 3
lab_light_on = 4
lab_rachel_end_announce = 5
lab_rachel_end = 6
lab_david_end_announce = 7

# Begin, Video, Fumigation, SterilizationIntro, Sterilization, BioScanIntro, BioScan, BioScanDenied, Wrong, Opening


binary_pcfs = [airlock_in_pcf, laserlock_in_pcf]

blank_screen_pid = subprocess.Popen(["cvlc", "media/black_screen.jpg", "--no-embedded-video", "--fullscreen",
                                     "--no-video-title", "--video-wallpaper", "--quiet", "--loop"])


class States:
    def __init__(self):
        self.laserlock_door_armed = False
        self.laserlock_door_opened = False
        self.laserlock_fixed = False


states = States()


def play_elancell_intro():
    blank_screen_pid.kill()
    print("playing elancell intro")
    subprocess.Popen(['cvlc', "media/Welcome to Elancell_w_Audio.mp4",
                      "--no-embedded-video", "--fullscreen", '--no-video-title', '--video-on-top', '--quiet'])


def laserlock_arm_door():
    states.laserlock_door_armed = True


def laserlock_set_door_opened_state():
    states.laserlock_door_opened = True


def laserlock_door_open_condition():
    return states.laserlock_door_armed and not states.laserlock_door_opened


def laserlock_fixed_condition():
    # print(states.laserlock_fixed)
    return not states.laserlock_fixed


def laserlock_set_fixed():
    states.laserlock_fixed = True


event_map = {
    "airlock_begin": {
        trigger_cmd: "airlock",
        trigger_msg: "begin",
        pcf_in_add: airlock_in_pcf,
        pcf_in: 1,
        sound: {
            is_fx: True,
            sound_id: 0
        }
    },
    "airlock_begin_atmo": {
        trigger_cmd: "airlock",
        trigger_msg: "begin",
        pcf_in_add: airlock_in_pcf,
        pcf_in: 1,
        sound: {
            is_fx: False,
            sound_id: 0
        }
    },
    # Sound stops to early?
    "airlock_intro": {
        trigger_cmd: "airlock",
        trigger_msg: "intro",
        pcf_in_add: airlock_in_pcf,
        pcf_in: 2,
        event_script: play_elancell_intro,
        # this is the sound to go along with teh video
        sound: {
            is_fx: True,
            sound_id: 24
        }
    },
    "airlock_fumigation": {
        trigger_cmd: "airlock",
        trigger_msg: "fumigation",
        pcf_in_add: airlock_in_pcf,
        pcf_in: 7, 
        sound: {
            sound_id: 26
        }
    },
    "airlock_sterilizationIntro": {
        trigger_cmd: "airlock",
        trigger_msg: "sterilizationIntro",
        pcf_in_add: airlock_in_pcf,
        pcf_in: 5, 
        sound: {
            sound_id: 23
        }
    },
    "airlock_UV": {
        trigger_cmd: "airlock",
        trigger_msg: "UV",
        pcf_in_add: airlock_in_pcf,
        pcf_in: 4,
        sound: {
            sound_id: 2
        }
    },
    "airlock_BioScanIntro": {
        trigger_cmd: "airlock",
        trigger_msg: "BioScanIntro",
        pcf_in_add: airlock_in_pcf,
        pcf_in: 8,
        sound: {
            sound_id: 22
        }
    },
    "airlock_BioScan": {
        trigger_cmd: "airlock",
        trigger_msg: "BioScan",
        pcf_in_add: airlock_in_pcf,
        pcf_in: 9,
        sound: {
            sound_id: 25
        }
    },
    "airlock_BioScanDenied": {
        trigger_cmd: "airlock",
        trigger_msg: "BioScanDenied",
        pcf_in_add: airlock_in_pcf,
        pcf_in: 10,
        sound: {
            sound_id: 21
        }
    },
    "airlock_wrong": {
        trigger_cmd: "airlock",
        trigger_msg: "wrong",
        pcf_in_add: airlock_in_pcf,
        pcf_in: 3,
        sound: {
            is_fx: True,
            sound_id: 1
        }
    },
    "airlock_opening": {
        pcf_in_add: airlock_in_pcf,
        pcf_in: 6,
        sound: {
            is_fx: False,
            sound_id: 1
        }
    },
    # Airlock 2 (Laserlock Function)
    "usb_boot": {
        trigger_cmd: "usb",
        trigger_msg: "boot",
        pcf_out_add: [laserlock_out_pcf],
        pcf_out: [1 << 0],
        sound: {
            is_fx: False,
            sound_id: 2
        },
    },
    "laserlock_fail": {
        trigger_cmd: "airlock",
        trigger_msg: "access",
        pcf_out_add: [laserlock_out_pcf],
        pcf_out: [1 << 1],
        sound: {
            sound_id: 3
        }
    },
    "laserlock_cable_fixed": {
        pcf_in_add: laserlock_in_2_pcf,
        pcf_in: 1 << 0,
        event_condition: laserlock_fixed_condition,
        event_script: laserlock_set_fixed,
        fe_cb: {
            fe_cb_cmd: "airlock",
            fe_cb_tgt: "tr1",
            fe_cb_msg: "fixed"
        }
    },
    "laserlock_door_opened": {
        pcf_in_add: laserlock_in_2_pcf,
        pcf_in: 1 << 1,
        event_condition: laserlock_door_open_condition,
        event_script: laserlock_set_door_opened_state,
        sound: {
            sound_id: 3,
            is_fx: False
        }
    },
    "laserlock_bootdecon": {
        pcf_out_add: [laserlock_out_pcf],
        pcf_out: [1 << 2],
        event_script: laserlock_arm_door,
        # event_delay: 0,
        sound: {
            sound_id: 4,
        }
    },
    "laserlock_welcome_david": {
        pcf_in_add: laserlock_in_pcf,
        pcf_in: laserlock_io_david,
        pcf_out_add: [lab_light_out_pcf],
        pcf_out: [lab_light_standby],
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
        pcf_in_add: laserlock_in_pcf,
        pcf_in: laserlock_io_rachel,
        pcf_out_add: [lab_light_out_pcf],
        pcf_out: [lab_light_standby],
        sound: {
            sound_id: 16
        },
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr1",
            fe_cb_msg: "rachel"
        }
    },
    "laserlock_auth_tr1_david": {
        trigger_cmd: "ter1",
        trigger_msg: "david",
        pcf_in_add: laserlock_in_pcf,
        pcf_in: laserlock_io_isSeperation + laserlock_io_david,
        pcf_out_add: [lab_light_out_pcf],
        pcf_out: [lab_light_on],
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr1",
            fe_cb_msg: "david"
        },
        sound: {
            sound_id: 4,
            is_fx: False
        }
    },
    "laserlock_auth_tr2_rachel": {
        trigger_cmd: "ter2",
        trigger_msg: "rachel",
        pcf_in_add: laserlock_in_pcf,
        pcf_in:  laserlock_io_isSeperation + laserlock_io_david,
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr2",
            fe_cb_msg: "rachel"
        }
    },
    "laserlock_auth_tr1_rachel": {
        trigger_cmd: "ter1",
        trigger_msg: "rachel",
        pcf_in_add: laserlock_in_pcf,
        pcf_in: laserlock_io_isSeperation + laserlock_io_rachel,
        pcf_out_add: [lab_light_out_pcf],
        pcf_out: [lab_light_on],
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr1",
            fe_cb_msg: "rachel"
        },
        sound: {
            sound_id: 4,
            is_fx: False
        }
    },
    "laserlock_auth_tr2_david": {
        trigger_cmd: "ter2",
        trigger_msg: "david",
        pcf_in_add: laserlock_in_pcf,
        pcf_in: laserlock_io_isSeperation + laserlock_io_rachel,
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr2",
            fe_cb_msg: "david"
        }
    },
    "laserlock_lockout_tr1": {
        trigger_cmd: "ter1",
        trigger_msg: "empty",
        pcf_in_add: laserlock_in_pcf,
        pcf_in: laserlock_io_seperationEnd,
        pcf_out_add: [lab_light_out_pcf],
        pcf_out: [lab_light_standby],
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr1",
            fe_cb_msg: "empty"
        },
        sound: {
            is_fx: False,
            sound_id: 2
        },
    },
    "laserlock_lockout_tr2": {
        trigger_cmd: "ter2",
        trigger_msg: "empty",
        pcf_in_add: laserlock_in_pcf,
        pcf_in: laserlock_io_seperationEnd,
        pcf_out_add: [lab_light_out_pcf],
        pcf_out: [lab_light_standby],
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr2",
            fe_cb_msg: "empty"
        }
    },
    "dispenser_dishout": {
        trigger_cmd: "dispenser",
        trigger_msg: "dishout",
        pcf_out_add: [lab_light_out_pcf],
        pcf_out: [1 << 4]
    },
    "analyzer_run1": {
        trigger_cmd: "analyzer",
        trigger_msg: "run1Right",
        pcf_in_add: analyzer_in_pcf,
        pcf_in: 1 << 2,
        pcf_out_add: [lab_light_out_pcf],
        pcf_out: [1 << 2]
    },
    "analyzer_run2": {
        trigger_cmd: "analyzer",
        trigger_msg: "run2Right",
        pcf_in_add: analyzer_in_pcf,
        pcf_in: 1 << 3,
        fe_cb: {
            fe_cb_cmd: "elancell",
            fe_cb_msg: "enable",
            fe_cb_tgt: "tr2"
        }
    },
    "reset_atmo": {
        sound: {
            is_fx: False,
            sound_id: -1
        },
    },
    "cleanroom": {
        trigger_cmd: "cleanroom",
        trigger_msg: "unlock",
        pcf_out_add: [1],
        pcf_out: [1 << 3],
    },
    "lab_light_off": {
        trigger_cmd: labLight_trigger,
        trigger_msg: "off",
        pcf_out_add: [lab_light_out_pcf],
        pcf_out: [lab_light_off]
    },
    "end_rachel_announce": {
        trigger_cmd: labLight_trigger,
        trigger_msg: "rachel",
        pcf_out_add: [lab_light_out_pcf, airlock_out_pcf],
        pcf_out: [lab_rachel_end_announce, AirlockOut.rachel_announce]
    },
    "end_rachel": {
        trigger_cmd: labLight_trigger,
        trigger_msg: "rachelEnd",
        pcf_out_add: [lab_light_out_pcf, airlock_out_pcf],
        pcf_out: [lab_rachel_end, AirlockOut.rachel_end]
    },
    "end_david_announce": {
        trigger_cmd: labLight_trigger,
        trigger_msg: "davidEnd",
        pcf_out_add: [lab_light_out_pcf, airlock_out_pcf],
        pcf_out: [lab_david_end_announce, AirlockOut.david_end]
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






