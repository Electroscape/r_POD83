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


class Event:
    def __init__(self, callback):
        self.callback = callback


class SoundEvent:
    def __int__(self, effect_no, is_fx=True):
        self.id = effect_no
        self.is_fx = is_fx

    # @TODO: can we pass the settings somehow?
    def activate_sound(self):
        payload = {
            "ip": "192.168.178.172",
        }
        if self.is_fx:
            payload["fx_id"] = self.id
        else:
            payload["group_id"] = self.id
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
    "airlock_wrong": SoundEvent(1),
    "airlock_sterilisation": SoundEvent(8),
    "airlock_decon": SoundEvent(2),
    "usb_boot": SoundEvent(2, False)
}






