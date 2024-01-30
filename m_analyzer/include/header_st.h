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

int intputArray[inputCnt] = {};
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
    start = 1,              // nothing been solved
    firstSolution  = 2,     // first stage has been solved, evaluation only continues with the KS
    secondSolution = 4      // solved the final stage, emit event but goes back to firstSolution to enable a repeated trigger 
};


// the sum of all stages sprinkled with a bit of black magic
int stageSum = ~( ~0 << StageCount );
#define sampleCount 4

char passwords[sampleCount + 1][MaxPassLen] = {
    "P1 ", // runmode1
    "P4 ", // runmode1
    "P2 ", // runmode1
    "P3 ", // runmode1
    "P5 "
};

