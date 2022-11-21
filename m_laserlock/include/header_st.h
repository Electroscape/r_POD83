#pragma once

#define StageCount 9
#define PasswordAmount 2
#define MaxPassLen 10
// may aswell move this into the Oled lib?
#define headLineMaxSize 16

#define relayAmount 8
#define open        0
#define closed      1

#define doorOpen    0
#define doorClosed  1

#define txRelayAmount 2
#define ledCnt 13

// how long the system remains in unlock and accepts RFID cards 
unsigned long deconRFIDTimeout = 10000;
// how long the signal of RFID identity remains active 
unsigned long rfidTxDuration = 5000;
unsigned long displayFailedUnlock = 8000;
unsigned long rfidTimeout = 3000;

#define inputCnt 4

enum brains {
    airlockAccess,      // access module on the outside
    labAccess,          // access module on the inside of the lab
    ledBrain
};


enum inputs {
    failedBootTrigger,        // Red
    deconTrigger,       // Black
    bootupTrigger,    // Green
    reedDoor
};

uint8_t inputTypes[inputCnt] = {
    INPUT_PULLUP,
    INPUT_PULLUP,
    INPUT_PULLUP,
    INPUT_PULLUP
};


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
    doorInit = doorClosed,
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
    failedBoot = 2,     // trigger boot
    operational = 4,    // trigger boot & connectionfixed
    decon = 8,          // trigger decon
    unlock = 16,
    failedUnlock = 32,
    unlocked = 64,
    idle = 128, 
    locked = 256
};

// the sum of all stages sprinkled with a bit of black magic
int stageSum = ~( ~0 << StageCount );


// could have multiple brains listed here making up a matrix
// for now its only an Access module mapped here
int flagMapping[StageCount] {
    0,
    0,          // failedBoot
    0,          // operational
    0,          // decon
    rfidFlag,   // unlock
    0,          // failedUnlock
    rfidFlag,   // unlocked ... there is a cooldown on the access module so it should be fine to reactivate
    0,          // idle
    rfidFlag    // locked
};

char passwords[PasswordAmount][MaxPassLen] = {
    "SD",   // David
    "AH"    // Rachel
};

// defines what password/RFIDCode is used at what stage, if none is used its -1
int passwordMap[PasswordAmount] = {
    unlock + unlocked + locked,
    unlock + unlocked + locked
};
// make a mapping of what password goes to what stage


char stageTexts[StageCount][headLineMaxSize] = {
    "",
    "Booting",              // failedBoot
    "",                     // operational
    "",                     // decon
    "Scan Arm",             // unlock
    "Timeout",              // failedUnlock
    "Access Granted",       // unlocked
    "",
    ""
};