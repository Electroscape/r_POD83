import board
import busio
import json
from time import sleep

from adafruit_pn532.adafruit_pn532 import MIFARE_CMD_AUTH_A
from adafruit_pn532.i2c import PN532_I2C


# I2C connection:
i2c = busio.I2C(board.SCL, board.SDA)
pn532 = PN532_I2C(i2c, debug=False)
ic, ver, rev, support = pn532.firmware_version
print("Found PN532 with firmware version: {0}.{1}".format(ver, rev))
sleep(0.5)
pn532.SAM_configuration() # Configure PN532 to communicate with MiFare cards

classic_read_block = 1
ntag_read_block = 4

data = {
    "1": "1.png",
    "2": "2.png",
    "3": "3.png",
    "4": "4.png",
    "5": "5.png",
    "0": "0.mp4"
}

valid_cards = data.keys()

def rfid_read(uid):
    read_data = ""

    '''
    Checks if the card is read or not 
    '''
    try:
        # if classic tag
        auth = authenticate(uid)
    except Exception:
        # if ntag
        auth = False

    try:
        # Switch between ntag and classic
        if auth:  # True for classic and False for ntags
            data = pn532.mifare_classic_read_block(classic_read_block)
        else:
            data = pn532.ntag2xx_read_block(ntag_read_block)

        if data:
            read_data = data.decode('utf-8')[:2]
        else:
            read_data = ""
            print("None block")

    except Exception as e:
        print(e)

    return read_data

def rfid_present():
    '''
    checks if the card is present inside the box
    '''
    try:
        uid = pn532.read_passive_target(timeout=0.5) #read the card
    except RuntimeError:
        uid = None

    return uid


def authenticate(uid):
   
    rc = 0
    key = [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF]
    rc = pn532.mifare_classic_authenticate_block(
        uid, classic_read_block, MIFARE_CMD_AUTH_A, key)
    print(rc)
    return rc


def main():
   
    while True:
        card_uid = rfid_present()
        if card_uid:
            print(f"Card found uid: {card_uid}")

            card_read = rfid_read(card_uid)
            
            print(f"Data on card: {card_read}")
            if card_read in valid_cards:
                print(data[card_read])
            else:
                print('Wrong Card')
            
            # wait here until card is removed
            # if wrong card should it stuck?!
            while rfid_present() and card_read:
                continue

            print("card removed")
                      

if __name__ == "__main__":
    main()

