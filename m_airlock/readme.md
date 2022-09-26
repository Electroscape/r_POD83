# airlock
controls a gate, lights and beamer.
inputs via RFID and Keypad

### connections
reset PCF with pins 0 and vcc connects to a reed sensor
brain 0 is an access module
brain 1 is an rfid led

relay 0 is a RPi GPIO triggering the Intro video
relay 1 is a RPi GPIO triggering the decontamination video
relay 2 is the gate
relay 3 is an alarm light
relay 4 are UV lights
relay 5 is the entry/exit door