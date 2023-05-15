import requests

audio_path = "http://POD-ITX/AudioInterface.php"

payload = {
    "ip": "192.168.178.172",
    "group_id": -1
}


def main():
    resp = requests.post(audio_path, payload)
    print(resp)


main()
