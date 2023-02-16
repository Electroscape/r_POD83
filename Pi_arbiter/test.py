from communication.TESocketServer import TESocketServer
from time import sleep


nw_sock = TESocketServer(12345)


def main():
    while True:
        sleep(10)
        nw_sock.transmit("usb_boot")
        exit()


if __name__ == '__main__':
    main()
