import requests
import subprocess

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

event_script = "script"

laserlock_io_isSeperation = 1 << 7
laserlock_io_david = 1 << 6
laserlock_io_rachel = 1 << 5
laserlock_io_seperationEnd = 1 << 4
laserlock_input = 4

airlock_input = 5

binary_pcfs = [airlock_input]


def play_elancell_intro():
    subprocess.Popen(['cvlc', "media/Welcome to Elancell_w_Audio.mp4",
                      "--no-embedded-video", "--fullscreen", '--no-video-title'])

# [0x38, 0x39, 0x3A, 0x3C, 0x3D, 0x3E]


event_map = {
    "airlock_begin": {
        pcf_in_add: airlock_input,
        pcf_in: 1,
        sound: {
            is_fx: True,
            sound_id: 0
        }
    },
    "airlock_begin_atmo": {
        pcf_in_add: airlock_input,
        pcf_in: 1,
        sound: {
            is_fx: False,
            sound_id: 0
        }
    },
    "airlock_intro": {
        pcf_in_add: airlock_input,
        pcf_in: 2,
        event_script: play_elancell_intro,
        # this is the sound to go along with teh video
        sound: {
            is_fx: True,
            sound_id: 7
        }
    },
    "airlock_sterilisation": {
        pcf_in_add: airlock_input,
        # pcf_in: 5,
        sound: {
            is_fx: True,
            sound_id: 8
        }
    },
    "airlock_UV": {
        pcf_in_add: airlock_input,
        pcf_in: 4,
        sound: {
            sound_id: 2
        }
    },
    "airlock_wrong": {
        pcf_in_add: airlock_input,
        pcf_in: 3,
        sound: {
            is_fx: True,
            sound_id: 1
        }
    },
    "airlock_opening": {
        pcf_in_add: airlock_input,
        pcf_in: 6,
        sound: {
            is_fx: False,
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
        pcf_in_add: 3,
        pcf_in: 1 << 0,
        fe_cb: {
            fe_cb_cmd: "airlock",
            fe_cb_tgt: "tr1",
            fe_cb_msg: "fixed"
        }
    },
    "laserlock_bootdecon": {
        pcf_out_add: 0,
        pcf_out: 1 << 2,
        sound: {
            sound_id: 4,
        }
    },
    "laserlock_welcome_david": {
        pcf_in_add: laserlock_input,
        pcf_in: laserlock_io_david,
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
        pcf_in_add: laserlock_input,
        pcf_in: laserlock_io_rachel,
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
        pcf_in_add: laserlock_input,
        pcf_in: laserlock_io_isSeperation + laserlock_io_david,
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr2",
            fe_cb_msg: "david"
        }
    },
    "laserlock_auth_tr2_rachel": {
        pcf_in_add: laserlock_input,
        pcf_in: laserlock_io_isSeperation + laserlock_io_david,
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr2",
            fe_cb_msg: "rachel"
        }
    },
    "laserlock_auth_tr1_rachel": {
        pcf_in_add: laserlock_input,
        pcf_in: laserlock_io_isSeperation + laserlock_io_rachel,
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr1",
            fe_cb_msg: "rachel"
        }
    },
    "laserlock_auth_tr2_david": {
        pcf_in_add: laserlock_input,
        pcf_in: laserlock_io_isSeperation + laserlock_io_rachel,
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr2",
            fe_cb_msg: "david"
        }
    },
    "laserlock_lockout": {
        pcf_in_add: laserlock_input,
        pcf_in: laserlock_io_seperationEnd
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






