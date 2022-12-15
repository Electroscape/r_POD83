from datetime import datetime as dt
from time import sleep
from GPIOEmulator.EmulatorGUI import GPIO




some_bundle = GPIOBundle([4,5,6,14])

def main():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(4, GPIO.OUT, initial=GPIO.HIGH)
    GPIO.setup(5, GPIO.OUT, initial=GPIO.HIGH)
    GPIO.setup(6, GPIO.OUT, initial=GPIO.HIGH)
    GPIO.setup(14, GPIO.OUT, initial=GPIO.HIGH)
    while True:
        some_bundle.set_value(0)
        sleep(5)
        some_bundle.set_value(1)
        sleep(5)
        some_bundle.set_value(2)
        sleep(5)
        some_bundle.set_value(4)
        sleep(5)
        some_bundle.set_value(8)
        sleep(5)
        some_bundle.set_value(15)
        sleep(5)


main()