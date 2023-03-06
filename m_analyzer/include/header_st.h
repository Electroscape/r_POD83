#pragma once

#define StageCount 7
#define PasswordAmount 10
#define MaxPassLen 5
#define closed 1
#define open   0

const unsigned long rfidCheckInterval = 250;

enum stages {
    setupStage = 1,
    runMode1 = 2,
    analyze = 4,
    runMode1_fast = 8,
    firstSolution  = 16,
    waitfordish5 = 32,
    runMode2 = 64

};

// the sum of all stages sprinkled with a bit of black magic
int stageSum = ~( ~0 << StageCount );


// could have multiple brains listed here making up a matrix
// for now its only an Access module mapped here
int flagMapping[StageCount] {
    0,
    ledFlag+rfidFlag,
    ledFlag,
    ledFlag+rfidFlag,
    ledFlag,
    rfidFlag,
    ledFlag+rfidFlag
};

char passwords[PasswordAmount][MaxPassLen] = {
    "P1 ",
    "P2 ",
    "P3 ",
    "P4 ",
    "ZERO",  
    "P5 "
};

/* // defines what password/RFIDCode is used at what stage, if none is used its -1
int passwordMap[PasswordAmount] = {
    runMode1,
    runMode1,
    runMode1,
    runMode1,
    runMode1,
}; */

