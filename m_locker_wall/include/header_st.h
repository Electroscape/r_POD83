#pragma once

#define StageCount 2
#define PasswordAmount 6
#define MaxPassLen 10
#define lockerCnt 4

// may aswell move this into the Oled lib?
#define headLineMaxSize 16

#define open   0
#define closed 1

enum relays {
    locker_1,
    locker_2,
    locker_3,
    locker_4,
    service, 
    relayAmount, 
};

enum relayInits {
    locker_1_init = closed,
    locker_2_init = closed,
    locker_3_init = closed,
    locker_4_init = closed,
    service_init = closed
};

int relayPinArray[relayAmount] = {
    locker_1,
    locker_2,
    locker_3,
    locker_4,
    service
};

int relayInitArray[relayAmount] = {
    locker_1_init,
    locker_2_init,
    locker_3_init,
    locker_4_init,
    service_init
};


enum stages{
    gameLive = 1,
    serviceMode = 2
};

// the sum of all stages sprinkled with a bit of black magic
int stageSum = ~( ~0 << StageCount );


// could have multiple brains listed here making up a matrix
int flagMapping[StageCount]{
    keypadFlag + oledFlag,
    keypadFlag + oledFlag
};
// save what already is turned on on the brain so we do not need to send it again
int devicesOn = 0;

char passwords[PasswordAmount][MaxPassLen] = {
    "0001",
    "0002",
    "0003",
    "0004",
    "1111",     // service code
    "0000",     // reset code, does this also work within th service mode?
};

const int resetIndex = service + 1;


// defines what password/RFIDCode is used at what stage, if none is used its -1
int passwordMap[PasswordAmount] = {
    gameLive,
    gameLive,
    gameLive,
    gameLive,
    stageSum, // service code, valid in all stages
    stageSum  // reset codevalid in all stages
};
// make a mapping of what password goes to what stage


char stageTexts[StageCount][headLineMaxSize] = {
    "Access Code",
    "Service Mode"
};