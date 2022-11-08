#pragma once

#define StageCount 10
#define PasswordAmount 2
#define MaxPassLen 10
// may aswell move this into the Oled lib?
#define headLineMaxSize 16

#define relayAmount 2
#define open        0
#define closed      1

#define ledBrain 1


enum relays {
    door,
    uv
};

enum relayInits {
    doorInit = open,
    uvInit
};

int relayPinArray[relayAmount] = {
    door, 
    uv
};

int relayInitArray[relayAmount] = {
    doorInit,
    uvInit
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
    "SD",
    "AH"
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
