from communication.TESocketServer import TESocketServer
from time import sleep
from event_mapping import *

nw_sock = TESocketServer(12345)


def main():
    event_value = event_map[ "laserlock_lockout_tr2"]
    cb_dict = event_value.get(fe_cb, False)

    if type(cb_dict) != list:
        cb_dict = list(cb_dict)

    print(type(cb_dict))
    print(cb_dict)


if __name__ == '__main__':
    main()


'''
@sio.event
def disconnect():
    if not connected:
        return False
    global connected
    connected = False
    sio.disconnect()
    connect()

@sio.on('*')
def catch_all(event, data):
    print("\n")
    print(event)
    print(data)
    print("\n")
    pass
'''