#pragma once

#define relayAmount 8
#define open        0
#define closed      1
// #define Hamburg     1

#define DOOR_OPENTIME       18
#define ARMTIME             18

enum relays {
    FR_LIGHTS_PIN,
    FAN_OUT_BIG_PIN,
    FANS_SMALL_PIN,
    RPI_VIDEO_PIN,
    FOG_PIN,
    ROOM_LIGHT_PIN, // not used anymore but kept to keep pin numbering consistent
    PNDOOR_PIN,
    KEYPAD_PIN
};

enum relayInits {
    FR_LIGHTS_INIT = open,
    FAN_OUT_BIG_INIT = closed,
    FANS_SMALL_INIT = closed,
    RPI_VIDEO_INIT = closed,
    FOG_INIT = closed,
    ROOM_LIGHT_INIT = closed, // not used anymore but kept to keep pin numbering consistent
    PNDOOR_INIT = closed,
    KEYPAD_INIT = closed
};

int relayPinArray[relayAmount] = {
    FR_LIGHTS_PIN,
    FAN_OUT_BIG_PIN,
    FANS_SMALL_PIN,
    RPI_VIDEO_PIN,
    FOG_PIN,
    ROOM_LIGHT_PIN,     // not used anymore but kept to keep pin numbering consistent
    PNDOOR_PIN,
    KEYPAD_PIN
};

int relayInitArray[relayAmount] = {
    FR_LIGHTS_INIT,
    FAN_OUT_BIG_INIT,
    FANS_SMALL_INIT,
    RPI_VIDEO_INIT,
    FOG_INIT,
    ROOM_LIGHT_INIT,
    PNDOOR_INIT,
    KEYPAD_INIT
};

enum brains {
    ledBrain = 1
};

enum led_strips {
    fr_leds = PWM::set1,
    zyl_leds = PWM::set2
};


// --- Inputs IGNORE THE NAMING SCHEME, this is already transmitted as binary
enum IOPins {
    IO1,    // armingpin
    IO2,    // deconTrigger
};


enum IO {
    arming = 1 << 0,
    deconTrigger = 1 << 1,
};

#define outputCnt 0
#define inputCnt 2


int intputArray[inputCnt] = {
    IO1,
    IO2,
};

int outputArray[outputCnt] = {};

