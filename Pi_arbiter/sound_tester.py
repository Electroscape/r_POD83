import requests


def activate_sound():
    payload = dict(ip="192.168.178.172")

    try:
        if True:
            payload["fx_id"] = 1
        else:
            payload["group_id"] = sound_id_value
    except KeyError:
        pass

    try:
        ret = requests.post("http://POD-ITX/AudioInterface.php", payload)
        print(ret)
    except OSError as OSE:
        print(f"failed to request sound due to {OSE}")
        exit()


def main():
    activate_sound()


if __name__ == '__main__':
    main()