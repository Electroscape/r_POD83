# Guidelines of installation
## pre-installation
1. connect rfid on i2c pin on rpi
2. from `sudo raspi-config` Enable i2c

## installation
1. run `bash install.sh`
2. at end of  `sudo nano /etc/nfc/libnfc.conf` add 
    ``` 
    device.name = "PN532 over I2C"
    device.connstring = "pn532_i2c:/dev/i2c-1"
    ```
3. run `i2cdetect -y 1` to assure i2c sensor is detected corretly. Default address is 0x24

## Configuration
1. Microscope can work in offline mode without connection to a server, however, to be connected to a server change the line in `microscope.py` to the correct address e.g. `server_ip = "http://raspi-4-pod-t1:5500"`
2. In `run.sh` change the ip of the kiosk display.
3. Update crontab to execute `run.sh` at reboot.
