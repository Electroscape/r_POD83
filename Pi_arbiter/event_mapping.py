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

laserlock_io_isSeperation = 16
laserlock_io_david = 32
laserlock_io_rachel = 64
laserlock_io_seperationEnd = 128
laserlock_input = 4

airlock_input = 5   # Begin, Video, Fumigation, SterilizationIntro, Sterilization, BioScanIntro, BioScan, BioScanDenied, Wrong, Opening
dispenser_output = 1    # put out current dish
analyzer_input = 2  # first four placed right; placed right with dish no5

delay_event = "IPreferWeDidNot"

binary_pcfs = [airlock_input, laserlock_input]


def play_elancell_intro():
    # subprocess.call(['pkill -f', "vlc"])
    subprocess.Popen(['cvlc', "media/Welcome to Elancell_w_Audio.mp4",
                      "--no-embedded-video", "--fullscreen", '--no-video-title', '--video-on-top'])

# these are the pcf addresses, first 3 are input last 3 are outputs
# [0x38, 0x39, 0x3A, 0x3C, 0x3D, 0x3E]


event_map = {
    "airlock_begin": {
        trigger_cmd: "airlock",
        trigger_msg: "begin",
        pcf_in_add: airlock_input,
        pcf_in: 1,
        sound: {
            is_fx: True,
            sound_id: 0
        }
    },
    "airlock_begin_atmo": {
        trigger_cmd: "airlock",
        trigger_msg: "begin",
        pcf_in_add: airlock_input,
        pcf_in: 1,
        sound: {
            is_fx: False,
            sound_id: 0
        }
    },
    "airlock_intro": { # Sound stopps to early?
        trigger_cmd: "airlock",
        trigger_msg: "intro",
        pcf_in_add: airlock_input,
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
        pcf_in_add: airlock_input,
        pcf_in: 7, 
        sound: {
            sound_id: 26
        }
    },
    "airlock_sterilizationIntro": {
        trigger_cmd: "airlock",
        trigger_msg: "sterilizationIntro",
        pcf_in_add: airlock_input,
        pcf_in: 5, 
        sound: {
            sound_id: 23
        }
    },
    "airlock_UV": {
        trigger_cmd: "airlock",
        trigger_msg: "UV",
        pcf_in_add: airlock_input,
        pcf_in: 4,
        sound: {
            sound_id: 2
        }
    },
    "airlock_BioScanIntro": {
        trigger_cmd: "airlock",
        trigger_msg: "BioScanIntro",
        pcf_in_add: airlock_input,
        pcf_in: 8,
        sound: {
            sound_id: 22
        }
    },
    "airlock_BioScan": {
        trigger_cmd: "airlock",
        trigger_msg: "BioScan",
        pcf_in_add: airlock_input,
        pcf_in: 9,
        sound: {
            sound_id: 25
        }
    },
    "airlock_BioScanDenied": {
        trigger_cmd: "airlock",
        trigger_msg: "BioScanDenied",
        pcf_in_add: airlock_input,
        pcf_in: 10,
        sound: {
            sound_id: 21
        }
    },
    "airlock_wrong": {
        trigger_cmd: "airlock",
        trigger_msg: "wrong",
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
    # Airlock 2 (Laserlock Functions)
    "usb_boot": {
        trigger_cmd: "usb",
        trigger_msg: "boot",
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
        delay_event: 1.5,
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
            fe_cb_msg: "rachel"
        }
    },
    "laserlock_auth_tr1_david": {
        trigger_cmd: "ter1",
        trigger_msg: "david",
        pcf_in_add: laserlock_input,
        pcf_in: laserlock_io_isSeperation + laserlock_io_david,
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr1",
            fe_cb_msg: "david"
        }
    },
    "laserlock_auth_tr2_rachel": {
        trigger_cmd: "ter2",
        trigger_msg: "rachel",
        pcf_in_add: laserlock_input,
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
        pcf_in_add: laserlock_input,
        pcf_in: laserlock_io_isSeperation + laserlock_io_rachel,
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr1",
            fe_cb_msg: "rachel"
        }
    },
    "laserlock_auth_tr2_david": {
        trigger_cmd: "ter2",
        trigger_msg: "david",
        pcf_in_add: laserlock_input,
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
        pcf_in_add: laserlock_input,
        pcf_in: laserlock_io_seperationEnd,
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr1",
            fe_cb_msg: "empty"
        }
    },
    "laserlock_lockout_tr2": {
        trigger_cmd: "ter2",
        trigger_msg: "empty",
        pcf_in_add: laserlock_input,
        pcf_in: laserlock_io_seperationEnd,
        fe_cb: {
            fe_cb_cmd: "auth",
            fe_cb_tgt: "tr2",
            fe_cb_msg: "empty"
        }
    },
    "dispenser_dishout": {
        trigger_cmd: "dispenser",
        trigger_msg: "dishout",
        pcf_out_add: 0,
        pcf_out: 1 << 3
    },
    "analyzer_run1": {
        trigger_cmd: "analyzer",
        trigger_msg: "run1Right",
        pcf_in_add: analyzer_input,
        pcf_in: 1 << 0
    },
    "analyzer_run2": {
        trigger_cmd: "analyzer",
        trigger_msg: "run2Right",
        pcf_in_add: analyzer_input,
        pcf_in: 1 << 1
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






