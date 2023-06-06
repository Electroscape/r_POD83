# Author Martin Pek
# 2CP - TeamEscape - Engineering

import socket
from time import sleep
from threading import Thread


class SocketClient:
    # trials to create a client (0 or less will try forever,
    #                            1 will not connect at all 
    #                            2 or more will try trials -1 times)
    def __init__(self, ip, port, timeout=None):
        self.ip = ip
        self.port = port
        self.timeout = timeout
        self.buffer = []
        thread = Thread(target=self.__run_socket_client)
        thread.start()

    def __connect(self):
        try:
            self.s.connect((self.ip, self.port))
            print("client has connected to {}:{}".format(self.ip, self.port))
        except socket.error as msg:
            self.s.close()
            print(msg)
            return False
        return True

    def __received(self):
        try:
            lines = self.s.recv(1024)
            if type(lines) is not str:
                for line in lines.splitlines():
                    line = line.decode()
                    # Todo: buffer overflow? mby have a ringbuffer? limited size?
                    self.buffer.append(line)
                    print(line)
            return True
        except socket.timeout:
            return False

    def __run_socket_client(self):
        while True:
            self.successful = False
            s = socket.socket()
            s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            s.settimeout(self.timeout)
            self.s = s
            print('socket client looking for connection')
            if self.__connect():
                while self.__received():
                    try:
                        self.s.send(bytes("Ping", "UTF-8"))
                    except socket.error:
                        break
            else:
                sleep(1)

    def read_buffer(self):
        ret = self.buffer
        self.buffer = []
        return ret


if __name__ == "__main__":
    print("not imported")
    SocketClient('127.0.0.1', 12345)


'''
from serial_brain.socket_client import SocketClient
test = SocketClient('127.0.0.1', 12345)

test.read_buffer()
'''