#pragma once

#define StageCount 10
#define PasswordAmount 2
#define MaxPassLen 10
// may aswell move this into the Oled lib?
#define headLineMaxSize 16

#define relayAmount 8
#define open        0
#define closed      1

#define ledBrain 1
#define txRelayAmount 2


enum relays {
    door,
    uv,
    rel_3,
    rel_4,
    rel_5,
    rel_6,
    RFID_TX_1,
    RFID_TX_2
};


enum relayInits {
    doorInit = closed,
    uvInit = closed,
    rel_3_init = closed,
    rel_4_init = closed,
    rel_5_init = closed,
    rel_6_init = closed,
    RFID_TX_1_init = closed,
    RFID_TX_2_init = closed,
};


int relayPinArray[relayAmount] = {
    door, 
    uv,
    rel_3,
    rel_4,
    rel_5,
    rel_6,
    RFID_TX_1,
    RFID_TX_2
};


// used to send binary signal to RPi to show identity presented
int rfidTxPins[txRelayAmount] = {
    RFID_TX_1,
    RFID_TX_2
};


int relayInitArray[relayAmount] = {
    doorInit,
    uvInit,
    rel_3_init,
    rel_4_init,
    rel_5_init,
    rel_6_init,
    RFID_TX_1_init,
    RFID_TX_2_init
};


enum stages {
    setupStage = 1, 
    failedBoot = 2,     // trigger 1
    operational = 4,    
    decon = 8,          // trigger 2
    unlock = 16,
    failedUnlock = 32,
    unlocked = 64
};

// the sum of all stages sprinkled with a bit of black magic
int stageSum = ~( ~0 << StageCount );


// could have multiple brains listed here making up a matrix
// for now its only an Access module mapped here
int flagMapping[StageCount] {
    0,
    0,
    0, // oeprational
    0,
    rfidFlag, // unlock
    0,  // failedUnlock
    0   // unlocked
};

char passwords[PasswordAmount][MaxPassLen] = {
    "SD",   // David
    "AH"    // Rachel
};

// defines what password/RFIDCode is used at what stage, if none is used its -1
int passwordMap[PasswordAmount] = {
    unlock,
    unlock
};
// make a mapping of what password goes to what stage


char stageTexts[StageCount][headLineMaxSize] = {
    "",
    "Booting",
    "",
    "",
    "Scan arm",
    "",
    "",
    "",
    "",
    ""
};
