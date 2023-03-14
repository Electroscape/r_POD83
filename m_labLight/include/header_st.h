#pragma once

#define relayAmount 2
#define open        0
#define closed      1

enum relays {
    labEntry,
    labExit,
};

enum relayInits {
    labEntry_init = closed,
    labExit_init = closed,
};

int relayPinArray[relayAmount] = {
    labEntry,
    labExit,
};

int relayInitArray[relayAmount] = {
    labEntry_init,
    labExit_init,
};

enum brains {
    ledCeilBrain = 4
};


// --- Inputs
enum IOPins {
    lightOff_pin,             // red
    lightNormal_pin,      // black  
    lightNormalBright_pin,      // black  
    lightRed_pin,          // Green
    lightRedBright_pin,          // Green
    lightBlue_pin,          // Green
    door_pin,          // Green
    inverted_door_pin,
};


enum IO {
    lightOff = 1,
    lightNormal,
    lightRed,
    lightBlue,
    lightRachelAnnouncement,
    lightRachelEnd,
    lightDavidAnnouncement,
    lightDavidEnd,
    door = 1 << door_pin,
    inverted_door = 1 << inverted_door_pin
};

#define outputCnt 1
#define inputCnt 7

static constexpr int clrLight[3] = {255,200,120};


int intputArray[inputCnt] = {
    lightOff_pin,             // red
    lightNormal_pin,      // black  
    lightNormalBright_pin,      // black  
    lightRed_pin,          // Green
    lightRedBright_pin,          // Green  
    lightBlue_pin,          // Green
    door_pin,
};

int outputArray[outputCnt] = {
    inverted_door_pin
};

