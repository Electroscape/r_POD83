import board
import busio
from time import sleep
import socketio

from adafruit_pn532.adafruit_pn532 import MIFARE_CMD_AUTH_A
from adafruit_pn532.i2c import PN532_I2C

# standard Python
sio = socketio.Client()

# I2C connection:
i2c = busio.I2C(board.SCL, board.SDA)
pn532 = PN532_I2C(i2c, debug=False)
ic, ver, rev, support = pn532.firmware_version
print("Found PN532 with firmware version: {0}.{1}".format(ver, rev))
sleep(0.5)
pn532.SAM_configuration()  # Configure PN532 to communicate with MiFare cards

classic_read_block = 1
ntag_read_block = 4


def rfid_present() -> bytearray:
    """
    checks if the card is present inside the box
    @return: (bytearray) with uid or empty value.
    """
    try:
        uid = pn532.read_passive_target(timeout=0.5)  # read the card
    except RuntimeError:
        uid = b''

    return uid


def authenticate(uid) -> bool:
    key = [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF]
    try:
        rc = pn532.mifare_classic_authenticate_block(
            uid, classic_read_block, MIFARE_CMD_AUTH_A, key)
        print("classic card authenticate successfully")
    except Exception as e:
        print(e)
        rc = False
        print("authentication failed! It might be ntag")

    return rc


def rfid_read(uid):
    """
    Reads data written on the card
    """
    read_data = ""

    auth = authenticate(uid)

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


class RFID(socketio.AsyncClientNamespace):
    # Check door status
    def __init__(self, cards, server_ip):
        super().__init__()
        self.cards = cards
        self.ip = server_ip
        self.data = {
            "message": cards["0"],
            "status": "off"
        }
        self.task = sio.start_background_task(self.check_loop)

    def on_connect(self):
        pass

    def on_disconnect(self):
        # attempt to reconnect, otherwise sleep for 2 seconds
        try:
            self.connect((host, port))
            connected = True
            print("re-connection successful")
        except socket.error:
            sleep(2)

    def get_data(self):
        return self.data

    async def on_set_status(self, status):
        self.data["status"] = status

    async def check_loop(self):
        while True:
            card_uid = rfid_present()
            if card_uid:
                print(f"Card found uid: {card_uid}")

                card_read = rfid_read(card_uid)

                print(f"Data on card: {card_read}")
                if card_read in self.cards.keys():
                    self.data["show"] = card_read
                    print(self.cards[card_read])
                    await self.emit('rfid_update', self.data)
                else:
                    print('Wrong Card')
                    await self.emit('rfid_update', self.data)

                # wait here until card is removed
                # if wrong card should it stuck?!
                while rfid_present() and card_read:
                    continue

                await self.emit('rfid_update', "0")
                print("card removed")
