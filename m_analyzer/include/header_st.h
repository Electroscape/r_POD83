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
    runMode1 = 1,
    analyze = 2,
    runMode2 = 4,
    firstSolution  = 8,
    runMode3 = 16,
    secondSolution = 32
};


// the sum of all stages sprinkled with a bit of black magic
int stageSum = ~( ~0 << StageCount );


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

char initialCombination[4][MaxPassLen] = {
    "P1 ", // runmode1
    "P4 ", // runmode1
    "P2 ", // runmode1
    "P3 ", // runmode1
};

char finalCombination[4][MaxPassLen] = {
    "P5 ", // runmode3
    "P4 ", // runmode3
    "P2 ", // runmode3
    "P3 ", // runmode3
};


