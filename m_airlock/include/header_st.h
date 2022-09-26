#pragma once

#define StageCount 9
#define PasswordAmount 6
#define MaxPassLen 10
#define lockerCnt 4

// may aswell move this into the Oled lib?
#define headLineMaxSize 16

#define open   0
#define closed 1

unsigned long introDuration = 5000;
// duration of the operation of the gate
unsigned long airlockDuration = 5000;

enum relays {
    beamerIntro,
    beamerDecon,
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
    beamerIntro,
    beamerDecon,
    gate,
    alarm,
    uvLight,
};

int relayInitArray[relayAmount] = {
    beamer_init,
    beamer_init,
    gate_init,
    alarm_init,
    uvLight_init
};


enum stages{
    preStage = 1,
    // ready to scan the RFID card, red light
    startStage = 2,
    intro = 4,
    decontamination = 8,
    cleanAirlock = 16,
    // entering the password after presenting hte RFID
    airlockRequest = 32, 
    airlockOpening = 64,
    airlockOpen = 128,
    idle = 256
};

// the sum of all stages sprinkled with a bit of black magic
int stageSum = ~( ~0 << StageCount );


// could have multiple brains listed here making up a matrix
// for now its only an Access module mapped here
int flagMapping[StageCount]{
    0,
    rfidFlag,
    0,
    0,
    0,
    keypadFlag,
    0,
    0,
    0
};
// save what already is turned on on the brain so we do not need to send it again
int devicesOn = 0;

char passwords[PasswordAmount][MaxPassLen] = {
    "GF",
    "0002",
};

// defines what password/RFIDCode is used at what stage, if none is used its -1
int passwordMap[PasswordAmount] = {
    startStage + decontamination,
    airlockRequest
};
// make a mapping of what password goes to what stage


char stageTexts[StageCount][headLineMaxSize] = {
    "Welcome",
    "Present Card",
    "Thank you",
    "Decontamination",
    "Access denied",
    "Enter Code",
    "Wait to enter",
    "Caution",
    ""
};
