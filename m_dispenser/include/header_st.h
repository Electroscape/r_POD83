#pragma once

#define StageCount 9
#define PasswordAmount 1 //only for debugg
#define MaxPassLen 5
#define closed 1
#define open   0

#define PumpOn      1
#define PumpOff     0

// #define Hamburg 1
// purely for testing and debugging
// #define IgnoreLeds 1


enum brains {
    LED_Brain,     
    Servo_Brain1,         
    Servo_Brain2
};

// --- Inputs
enum IO {
    dispenserAction_pin,
    dispenserIO2,
    dispenserIO3,
    dispenserIO4
};

#define inputCnt 4
#define outputCnt 0

/* uint8_t inputTypes[inputCnt] = {
    INPUT_PULLUP
}; */
int dispenserAction = (1 << dispenserAction_pin);

int intputArray[inputCnt] = {
    dispenserAction_pin,
    dispenserIO2,
    dispenserIO3,
    dispenserIO4
};

int outputArray[outputCnt] = {
};

enum IOValues {
    dishout=1,
    dish1,
    dish2,
    dish3,
    dish4,
    dish5,
    rachelEnd,
    elancellEnd
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
    Pump1_init = PumpOn,
    Pump2_init = PumpOn,
    Pump3_init = PumpOn,
    Pump4_init = PumpOn, 
    Pump5_init = PumpOn, 
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
    Pump1_init,
    Pump2_init,
    Pump3_init,
    Pump4_init, 
    Pump5_init,
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
    DavidEnd = 256,
};

// the sum of all stages sprinkled with a bit of black magic
int stageSum = ~( ~0 << StageCount );
int dishStageSum = Dish1 | Dish2 | Dish3 | Dish4 | Dish5;

