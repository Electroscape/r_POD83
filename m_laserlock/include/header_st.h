#pragma once

#define StageCount 11
#define PasswordAmount 2
#define MaxPassLen 10
// may aswell move this into the Oled lib?
#define headLineMaxSize 16

#define relayAmount 2
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


// 3 IO therefore 1+2+4 = 7 values 
// 4 IO = 15 Values
enum IOpins {
    IO_1,
    IO_2,
    IO_3,
    reedDoor, 
    IO_5,
    IO_6,
    IO_7,                       
    IO_8,                                            
};

// 15 values 4 IOs
enum inputValues {  
    roomBoot = 1,            
    elancellEnd,            
    rachelEnd,            
    lightOff,            
    cleanupLight,            
    failedBootTrigger,        
    bootupTrigger,        
};

// 7 Values 3 IOs
enum outputValues {
    david = 1,
    rachel,
    seperationEnd,
    davidSeperated,     // status for T1
    rachelSeperated,     // status for T1
    timeout,            // when card was not presented in time
};



#define outputCnt 4
#define inputCnt 4


int intputArray[inputCnt] = {
    IO_1,
    IO_2,
    IO_3,
    reedDoor
};

int outputArray[outputCnt] = {
    IO_5,
    IO_6,
    IO_7,                       
    IO_8,                       
};


// -- relays
enum relays {
    door,
    outerDoor
};

enum relayInits {
    doorInit = doorClosed,
    outerDoorInit = doorClosed,
};

int relayPinArray[relayAmount] = {
    door, 
    outerDoor,
};

int relayInitArray[relayAmount] = {
    doorInit,
    outerDoorInit
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