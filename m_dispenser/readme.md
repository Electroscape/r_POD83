# Analyzer
controls the petri dish analyzer
inputs via four RFID and output to LEDs

### connections
reset PCF with pins 0 and vcc connects to a reed sensor
brain 1 is an rfid led

relay 0 is a RPi GPIO triggering the Intro video
relay 1 is a RPi GPIO triggering the decontamination video
relay 2 is the gate_pwr -> relay 3 com
relay 3 is the gate_direction
relay 3 is an alarm light
relay 4 are UV lights
relay 5 is the entry/exit door