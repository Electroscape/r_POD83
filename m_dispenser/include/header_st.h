#pragma once

#define StageCount 8
#define PasswordAmount 1 //only for debugg
#define MaxPassLen 5
#define closed 1
#define open   0


enum brains {
    LED_Brain,     
    Servo_Brain1,         
    Servo_Brain2
};

// --- Inputs
enum IO {
    dispenserAction_pin
};
#define inputCnt 1
#define outputCnt 0

/* uint8_t inputTypes[inputCnt] = {
    INPUT_PULLUP
}; */
int dispenserAction = 1 << dispenserAction_pin;

int intputArray[inputCnt] = {
    dispenserAction_pin
};

int outputArray[outputCnt] = {
};


// -- relays
enum relays {
    Pump1,
    Pump2,
    Pump3,
    Pump4,
    Pump5,
    Belt1,
    Belt2,
    BeltOn,
    relayAmount
};

enum relayInits {
    Relay1_init = open,
    Relay2_init = open,
    Relay3_init = open,
    Relay4_init = open, 
    Relay5_init = open, 
    Relay6_init = closed,  
    Relay7_init = closed,  
    Relay8_init = closed, 
};

int relayPinArray[relayAmount] = {
    Pump1,
    Pump2,
    Pump3,
    Pump4,
    Pump5,
    Belt1,
    Belt2,
    BeltOn
};

int relayInitArray[relayAmount] = {
    Relay1_init,
    Relay2_init,
    Relay3_init,
    Relay4_init, 
    Relay5_init,
    Relay6_init,
    Relay7_init,
    Relay8_init, 
};

// Stages
enum stages {
    setupStage = 1,
    waitRequest = 2,
    Dish1 = 4,
    Dish2 = 8,
    Dish3  = 16,
    Dish4 = 32,
    Dish5 = 64,
    WorldsEnd =128,
};

// the sum of all stages sprinkled with a bit of black magic
int stageSum = ~( ~0 << StageCount );


// could have multiple brains listed here making up a matrix
// for now its only an Access module mapped here
int flagMapping[StageCount] {
    ledFlag,
    ledFlag+rfidFlag,
    ledFlag,
    ledFlag,
    ledFlag,
    ledFlag,
    ledFlag,
    ledFlag
};
