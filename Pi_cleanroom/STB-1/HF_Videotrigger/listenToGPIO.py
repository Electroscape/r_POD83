from time import sleep
import RPi.GPIO as GPIO
import run_video_once
import sshTrigger
import subprocess
import os

GPIO.setmode(GPIO.BOARD)
GPIO.setup(7, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

print("listening to gpios to trigger HF video")

def main():

    while True:
        hysteresis = 0
        while GPIO.input(7):
            hysteresis += 1
            if hysteresis > 5:
                print("successfully detected")
                subprocess.Popen(['sh', './play_decon.sh'], cwd=os.getcwd())
                sleep(5)

        sleep(0.1)

main()