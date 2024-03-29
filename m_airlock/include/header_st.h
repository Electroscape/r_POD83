#pragma once

#define StageCount 15
#define PasswordAmount 6
#define MaxPassLen 10
// may aswell move this into the Oled lib?
#define headLineMaxSize 17

#define open        0
#define closed      1
#define gateUp      0
#define gateDown    1

unsigned long introDuration = 5000;
// duration of the operation of the gate
unsigned long gateDuration = 30000;
unsigned long runningLightDuration = 10000;
// provide blinking and warning before the game moves on startup
unsigned long gateWarningDelay = 4500;

enum relays {
    beamerIntro,
    beamerDecon,
    gate_pwr,
    gate_direction,
    alarm,
    uvLight,
    // entry door
    door,
    relayAmount
};

enum relayInits {
    beamer_init = open,
    beamerDecon_init = closed,
    gate_pwr_init = closed,
    gate_direction_init = gateUp,
    alarm_init = closed,
    uvLight_init = closed,
    doorInit = open
};

int relayPinArray[relayAmount] = {
    beamerIntro,
    beamerDecon,
    gate_pwr,
    gate_direction,
    alarm,
    uvLight,
    door
};

int relayInitArray[relayAmount] = {
    beamer_init,
    beamerDecon_init,
    gate_pwr_init,
    gate_direction_init,
    alarm_init,
    uvLight_init,
    doorInit
};


#define outputCnt 4
// technically we already got one but ... changing the code is no prio here
#define inputCnt 4

enum IO {
    IO0,             // red, MSB
    IO1,             // blk
    IO2,             // green
    IO3,             // white
    door_reed,
    IO4,
    IO5,
    IO6
};

int david_end = (1 << 7);
int rachel_announce = (1 << 6); 
int rachel_end = (1 << 5);

int intputArray[inputCnt] = {
    door_reed,
    IO4,
    IO5,
    IO6
};

int outputArray[outputCnt] = {
    IO0,             // red, MSB
    IO1,             // blk
    IO2,             // green
    IO3,             // white
};

enum IOEvents{
    doorClosed = 1,
    welcomeVideo,
    wrongCode,
    uvEvent,
    sterilisationEvent,
    airlockOpeningEvent,
    fumigationEvent, // ab hier neu!!
    BioScanIntro,
    BioScanEvent,
    BioScanDenied,
};

enum stages {
    setupStage = 1,
    preStage = 2,
    // ready to scan the RFID card, red light
    startStage = 4,
    intro = 8,
    decontamination = 16,
    // entering the password after presenting hte RFID
    airlockRequest = 32, 
    airlockOpening = 64,
    // brief stage to use the running lights and keep the alarm on
    airlockOpen = 128,
    idle = 256, 
    airlockFailed = 512,
    fumigation = 1024,
    sterilization = 2048,
    BiometricScan = 4096,
    david_end_Stage = 1 << 13,
    rachel_end_stage = 1 << 14
};
// the sum of all stages sprinkled with a bit of black magic
int stageSum = ~( ~0 << StageCount );


// could have multiple brains listed here making up a matrix
// for now its only an Access module mapped here
int flagMapping[StageCount] {
    0,
    0,
    rfidFlag,
    0,
    0,
    keypadFlag,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

char passwords[PasswordAmount][MaxPassLen] = {
    "GF",
    "1984",
};

// defines what password/RFIDCode is used at what stage, if none is used its -1
int passwordMap[PasswordAmount] = {
    startStage,
    airlockRequest
};
// make a mapping of what password goes to what stage


char stageTexts[StageCount][headLineMaxSize] = {
    "Welcome",
    "Welcome",
    "Present Card",
    "Thank you",
    "Decontamination",
    "Enter Code",
    "Wait to enter",
    "Caution",
    "",
    "Access denied",
    "Fumigation",
    "Sterilisation",
    "Biometric Scan",
    "Thank You",
    "Self Destruction",
};
