from time import time, sleep
# not a fan of * impports ... amybe we can make classes
from event_mapping import *
from threading import Thread

# GPIO.add_event_detect(data.gpio, GPIO.RISING, callback=callback, bouncetime=20)

# make this a function?
try:
    import RPi.GPIO as GPIO
    print("Running on RPi")
except (RuntimeError, ModuleNotFoundError):
    print("Running without GPIOs on non Pi ENV / non RPi.GPIO installed machine")
    # self.settings.is_rpi_env = False
    # GPIO.VERBOSE = False
    from GPIOEmulator.EmulatorGUI import GPIO


from pcf8574 import PCF8574
# GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)


# may be moved to a util library so these can be declared in a
# config file
class GPIOBundle:
    def __init__(self, pins, callback_dictionary=None):
        self.__pins = pins
        self.__cooldown = 0
        self.__callback_dictionary = callback_dictionary
        for pin in self.__pins:
            if callback_dictionary is None:
                GPIO.setup(pin, GPIO.OUT, initial=GPIO.HIGH)
            else:
                GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
                # bundle_input_groups.append(self)

    def get_value(self):
        sleep(0.01)
        res = 0
        for i, pin in enumerate(self.__pins):
            if GPIO.input(pin) == GPIO.LOW:
                res |= 1 << i
        return res

    def set_value(self, value):
        for i, pin in enumerate(self.__pins):
            if (1 << i) & value > 0:
                GPIO.output(pin, GPIO.LOW)
            else:
                GPIO.output(pin, GPIO.HIGH)

    def has_event(self):
        # doing a really long cooldown here may need to be made a passed parameter or config value
        if time() - self.__cooldown < 10:
            return False
        for pin in self.__pins:
            if GPIO.input(pin) == GPIO.LOW:
                self.__cooldown = time()
                return True
        return False

    def handle(self):
        if not self.has_event():
            return False
        print("event detected")
        value = self.get_value()
        try:
            self.__callback_dictionary[value]()
        except ValueError:
            pass
        except Exception as err:
            print(f"failing to handle GPIObundle due to:\n{err}")

        print(" WIP here")


# laserlock_in_bundle = GPIOBundle([27, 26, 25], GPIO.IN)
def check_gpios(bundle_input_groups):
    for bundle in bundle_input_groups:
        bundle.handle()


def gpio_cb(event_dict):
    try:
        GPIO.output(event_dict["gpio_out"], GPIO.LOW)
    except KeyError:
        pass


def reset_timer_target(pins, gpio_ref):
    sleep(3)
    print(f"setting pins back to low {pins}")
    for pin in pins:
        gpio_ref.output(pin, gpio_ref.HIGH)


def reset_gpios(pins):
    gpio_reset_thread = Thread(target=reset_timer_target, args=(pins, GPIO, ))
    gpio_reset_thread.start()


def cooldown_check(pin, cooldown_list):
    for cooldown in cooldown_list:
        print("WIP")
