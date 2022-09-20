#pragma once

#define StageCount 2
#define PasswordAmount 6
#define MaxPassLen 10

// may aswell move this into the Oled lib?
// was 14
#define headLineMaxSize 16


enum stages{
    gameLive = 1,
    serviceMode = 2,
};

// the sum of all stages sprinkled with a bit of black magic
int stageSum = ~(~0<<sizeof(stages));


// could have multiple brains listed here making up a matrix
int flagMapping[StageCount]{
    keypadFlag + oledFlag,
    keypadFlag + oledFlag
};
// save what already is turned on on the brain so we do not need to send it again
int devicesOn = 0;

char passwords[PasswordAmount][MaxPassLen] = {
    "1111",     // service code
    "0000",     // reset code, does this also work within th service mode?
    "0001",
    "0002",
    "0003",
    "0004"
};


// defines what password/RFIDCode is used at what stage, if none is used its -1
int passwordMap[PasswordAmount] = {
    stageSum, // service code, valid in all stages
    stageSum, // reset codevalid in all stages
    gameLive,
    gameLive,
    gameLive,
    gameLive
};
// make a mapping of what password goes to what stage


char stageTexts[StageCount][headLineMaxSize] = {
    "Access Code",
    "Service Mode"
};

