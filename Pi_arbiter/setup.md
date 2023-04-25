### RPi Setup
 * enable remote GPIO
 * sudo apt-get install rpi.gpio
 * disable pop up message on media insertion by going to `file manger -> prefrences -> Volume Management -> uncheck "show available removeable media"` 


### ubuntu testing
 * gpioEmulator: may complain about tkinter `sudo apt-get install python3-tk`

### crontab 

`@reboot sleep 30 && /home/2cp/Pi_arbiter/run.sh >> out.txt  2>&1`

