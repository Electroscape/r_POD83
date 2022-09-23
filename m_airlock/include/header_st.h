#pragma once

#define StageCount 8
#define PasswordAmount 6
#define MaxPassLen 10
#define lockerCnt 4

// may aswell move this into the Oled lib?
#define headLineMaxSize 16

#define open   0
#define closed 1

enum relays {
    beamer,
    gate,
    alarm,
    uvLight,
    relayAmount
};

enum relayInits {
    beamer_init = closed,
    gate_init = closed,
    alarm_init = closed,
    uvLight_init = closed
};

int relayPinArray[relayAmount] = {
    beamer,
    gate,
    alarm,
    uvLight,
};

int relayInitArray[relayAmount] = {
    beamer_init,
    gate_init,
    alarm_init,
    uvLight_init
};


enum stages{
    preStage = 1,
    serviceMode = 2,
    intro = 4
};

// the sum of all stages sprinkled with a bit of black magic
int stageSum = ~( ~0 << StageCount );


// could have multiple brains listed here making up a matrix
// for now its only an Access module mapped here
int flagMapping[StageCount]{
    0,
    keypadFlag
};
// save what already is turned on on the brain so we do not need to send it again
int devicesOn = 0;

char passwords[PasswordAmount][MaxPassLen] = {
    "0001",
    "0002",
    "0003",
    "0004",
    "1111",     // service code
    "0000"     // reset code, does this also work within th service mode?
};

// defines what password/RFIDCode is used at what stage, if none is used its -1
int passwordMap[PasswordAmount] = {
    stageSum  // reset codevalid in all stages
};
// make a mapping of what password goes to what stage


char stageTexts[StageCount][headLineMaxSize] = {
    "Welcome",
    "Present Card",
    "Thank you",
    "Decontamination",
    "Enter Code",
    "Wait to enter",
    "Caution",
    ""
};
