import board
import busio
from time import sleep
import socketio

from adafruit_pn532.adafruit_pn532 import MIFARE_CMD_AUTH_A
from adafruit_pn532.i2c import PN532_I2C

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


def rfid_read(uid) -> str:
    """
    Reads data written on the card
    """
    read_data = "x"

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
            read_data = "x"
            print("None block")

    except Exception as e:
        print(e)

    return read_data


class RFID:
    # Check door status
    def __init__(self, server_ip, cards=None, **config):
        if cards is None:
            cards = [0]
        self.name = config.get("name", "rfid")
        self.cards = cards
        self.ip = server_ip
        self.sio = socketio.Client()
        self.setup_sio()

        self.data = {
            "data": str(cards[-1]),
            "status": config.get("init", "on")
        }
        self.connected = False
        self.reconnect_task = self.sio.start_background_task(self.keep_reconnecting)
        self.rfid_task = self.sio.start_background_task(self.check_loop)

    def setup_sio(self):

        @self.sio.on('connect')
        def on_connect():
            self.connected = True
            print(f"{self.name} is connected to server")

        @self.sio.on('disconnect')
        def on_disconnect():
            self.connected = False
            print(f"{self.name} is disconnected!")

        @self.sio.on('set_status')
        def on_set_status(status):
            print(f"change status to: {status}")
            self.data["status"] = status

    def set_rfid_status(self, status):
        print(f"change status to: {status}")
        self.data["status"] = status

    def set_rfid_data(self, msg):
        print(f"override data to: {msg}")
        self.data["data"] = msg

    def keep_reconnecting(self):
        while True:
            # attempt to reconnect, otherwise sleep for 2 seconds
            if not self.connected:
                try:
                    self.sio.connect(self.ip)
                    self.connected = True
                    print(f"{self.name} is connected to sio")
                except Exception as e:
                    print("trying to reconnect")
                    sleep(2)
            else:
                sleep(2)

    def get_data(self):
        return self.data

    def emit(self, channel, message):
        if self.connected:
            self.sio.emit(channel, message)
        else:
            print("not connected to server!")

    def check_loop(self):
        while True:
            card_uid = rfid_present()
            if card_uid:
                print(f"Card found uid: {card_uid}")

                card_all = rfid_read(card_uid)
                card_read = card_all[0]

                print(f"Data on card: {card_read}")
                if card_read in self.cards:
                    self.data["data"] = card_read
                    print(f"chosen card: {card_read}")
                    self.emit(f'{self.name}_update', self.data)
                else:
                    print(f'Wrong data: {card_all}')
                    self.emit(f'{self.name}_extra', card_all)

                # wait here until card is removed
                # if wrong card should it stuck?!
                while rfid_present() and card_read:
                    continue

                self.data["data"] = str(self.cards[-1])
                self.emit(f'{self.name}_update', self.data)
                print("card removed")
                print(f"sio.emit('{self.name}_update', '{self.data}')")
