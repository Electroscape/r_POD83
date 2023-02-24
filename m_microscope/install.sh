sudo apt update -y
sudo apt install i2c-tools libnfc6 libnfc-bin libnfc-examples chromium-browser -y

# shellcheck disable=SC2164
python3 -m venv venv
source venv/bin/activate

pip3 install -r requirements.txt