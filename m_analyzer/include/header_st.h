#pragma once

#define StageCount 8
#define PasswordAmount 11
#define MaxPassLen 5
#define closed 1
#define open   0

const unsigned long rfidCheckInterval = 400;

#define outputCnt 2
#define inputCnt 0

enum IO {
    IO0,            
    IO1
};

int intputArray[inputCnt] = {
};
int outputArray[outputCnt] = {
    IO0,             
    IO1,             
};
enum IOEvents{
    firstSolutionEvent = 1,
    secondSolutionEvent
};

enum light{
    red_blinking,
    turn_off,
    hint_place1,
    hint_place2,
    hint_place3,
    hint_place4
};

enum stages {
    setupStage = 1,
    runMode1 = 2,
    analyze = 4,
    runMode2 = 8,
    firstSolution  = 16,
    waitfordish5 = 32,
    runMode3 = 64,
    secondSolution =128
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
    ledFlag+rfidFlag,
    ledFlag+rfidFlag
};

char passwords[PasswordAmount][MaxPassLen] = {
    "P1 ", // runmode1
    "P4 ", // runmode1
    "P2 ", // runmode1
    "P3 ", // runmode1
    "P5 ", // WaitforDish5
    "P5 ", // runmode3
    "P4 ", // runmode3
    "P2 ", // runmode3
    "P3 ", // runmode3
};


