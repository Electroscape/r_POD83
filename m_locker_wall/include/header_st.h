#pragma once

#define StageCount 8
#define PasswordAmount 4
#define MaxPassLen 10

// may aswell move this into the Oled lib?
#define headLineMaxSize 14


enum stages{
    stage1 = 1,
    stage2 = 2,
    stage3 = 4,
    stage4 = 8,
    stage5 = 16,
    stage6 = 32,
    stage7 = 64,
    stage8 = 128
};

// the sum of all stages sprinkled with a bit of black magic
int stageSum = ~(~0<<sizeof(stages));


// could have multiple brains listed here making up a matrix
int flagMapping[StageCount]{
    keypadFlag + oledFlag,
    rfidFlag + oledFlag,
    keypadFlag + rfidFlag + oledFlag,
    keypadFlag + rfidFlag + oledFlag,
    keypadFlag + rfidFlag + oledFlag,
    keypadFlag + rfidFlag + oledFlag,
    keypadFlag + rfidFlag + oledFlag,
    keypadFlag + rfidFlag + oledFlag
};
// save what already is turned on on the brain so we do not need to send it again
int devicesOn = 0;

char passwords[PasswordAmount][MaxPassLen] = {
    "0000",
    "GF",
    "1337",
    "4321"
};


// defines what password/RFIDCode is used at what stage, if none is used its -1
int passwordMap[PasswordAmount] = {
    stageSum, // valid in all stages
    stage1,
    stage2, 
    stage3 + stage4
};
// make a mapping of what password goes to what stage


char stageTexts[StageCount][headLineMaxSize] = {
    "Stage 1",
    "Stage 2",
    "Stage 3",
    "Stage 4",
    "Stage 5",
    "Stage 6",
    "Stage 7",
    "Stage 8"
};

