#pragma once

#define StageCount 11
#define PasswordAmount 2
#define MaxPassLen 10
// may aswell move this into the Oled lib?
#define headLineMaxSize 16

#define relayAmount 1
#define open        0
#define closed      1

#define doorOpen    0
#define doorClosed  1

#define ledCnt 13

// how long the system remains in unlock and accepts RFID cards 
unsigned long deconRFIDTimeout = 12000;
// how long the signal of RFID identity remains active 
unsigned long rfidTxDuration = 5000;
unsigned long displayFailedUnlock = 8000;
unsigned long rfidTimeout = 3000;
unsigned long lightStartDuration = 20000;

// FL lamps shall be some industrial reddish light 
//static constexpr int clrLight[3] = {255,200,120};
static constexpr int clrLight[3] = {255,200,30};

enum brains {
    airlockAccess,      // access module on the outside
    labAccess,          // access module on the inside of the lab
    ledLaserBrain,
    ledCeilBrain
};


// --- Inputs
enum IO {
    room1Light,             // red
    failedBootTrigger,      // black  
    bootupTrigger,          // Green
    reedDoor,               // white    
    isSeperation_pin,
    david_pin,
    rachel_pin,
    seperationEnd_pin
};

int isSeperation = 1 << isSeperation_pin;
int david = 1 << david_pin;
int rachel = 1 << rachel_pin;
int seperationEnd = 1 << seperationEnd_pin; 

#define outputCnt 4
#define inputCnt 4

uint8_t inputTypes[inputCnt] = {
    INPUT_PULLUP,
    INPUT_PULLUP,
    INPUT_PULLUP,
    INPUT_PULLUP
};

int intputArray[inputCnt] = {
    room1Light,             // red
    failedBootTrigger,      // black  
    bootupTrigger,          // Green
    reedDoor,               // white    
};

int outputArray[outputCnt] = {
    isSeperation_pin,
    david_pin,
    rachel_pin,
    seperationEnd_pin
};


// -- relays
enum relays {
    door,
};

enum relayInits {
    doorInit = doorClosed,
};

int relayPinArray[relayAmount] = {
    door, 
};

int relayInitArray[relayAmount] = {
    doorInit,
};


enum stages {
    setupStage = 1, 
    idle = 2, 
    failedBoot = 4,     // trigger boot
    successfulBoot = 8,    // trigger boot & connectionfixed
    decon = 16,          // trigger decon
    unlock = 32,
    failedUnlock = 64,
    unlocked = 128,
    seperationUnlocked = 256,       // waiting for Auth on both sides to lock
    seperationLocked = 512,         // waiting for Auth on one side to unlock
    lightStart = 1024,
};

// the sum of all stages sprinkled with a bit of black magic
int stageSum = ~( ~0 << StageCount );


// could have multiple brains listed here making up a matrix
// for now its only an Access module mapped here
int flagMapping[StageCount] {
    0,          // setupStage
    0,          // idle
    0,          // failedBoot
    0,          // successfulBoot
    0,          // decon
    rfidFlag,   // unlock
    0,          // failedUnlock
    0,          // unlocked ... there is a cooldown on the access module so it should be fine to reactivate
    rfidFlag,   // seperationUnlocked
    rfidFlag,   // seperationLocked
    0           // lightStart from "USB-Boot"
};

char passwords[PasswordAmount][MaxPassLen] = {
    "SD",   // David
    "AH"    // Rachel
};

// defines what password/RFIDCode is used at what stage, if none is used its -1
int passwordMap[PasswordAmount] = {
    unlock + seperationLocked + seperationUnlocked,
    unlock + seperationLocked + seperationUnlocked
};
// make a mapping of what password goes to what stage


char stageTexts[StageCount][headLineMaxSize] = {
    "",                     // setupStage
    "",                     // idle 
    "Booting",              // failedBoot
    "Booting",                     // successfulBoot
    "",                     // decon
    "Scan Arm",             // unlock
    "Timeout",              // failedUnlock
    "Access Granted",       // unlocked
    "Present To Lock",      // seperationUnlocked
    "Present to Open"       // seperationLocked
    ""                      // lightStart
};