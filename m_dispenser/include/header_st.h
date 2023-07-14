#pragma once

// #define Hamburg 1
// purely for testing and easier debugging
// #define IgnoreLeds 1

#define StageCount 9
#define PasswordAmount 1 //only for debugg
#define MaxPassLen 5
#define closed 1
#define open   0

#define PumpOn      1
#define PumpOff     0

const unsigned long beltOperationTime = 17000;


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

int dispenserAction = (1 << dispenserAction_pin);

int intputArray[inputCnt] = {
    dispenserAction_pin,
    dispenserIO2,
    dispenserIO3,
    dispenserIO4
};

int outputArray[outputCnt] = {};

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
    pump1,
    pump2,
    pump3,
    pump4,
    pump5,
    beltReverse_1,      // both belt reverse need to be triggered to reverse
    beltReverse_2,      // both belt reverse need to be triggered to reverse
    BeltOn,             // VCC to trough beltReverse relays 
    relayAmount
};

enum relayInits {
    pump1_init = PumpOn,
    pump2_init = PumpOn,
    pump3_init = PumpOn,
    pump4_init = PumpOn, 
    pump5_init = PumpOn, 
    Relay6_init = closed,  
    Relay7_init = closed,  
    Relay8_init = closed, 
};

int relayPinArray[relayAmount] = {
    pump1,
    pump2,
    pump3,
    pump4,
    pump5,
    beltReverse_1,
    beltReverse_2,
    BeltOn
};

int relayInitArray[relayAmount] = {
    pump1_init,
    pump2_init,
    pump3_init,
    pump4_init, 
    pump5_init,
    Relay6_init,
    Relay7_init,
    Relay8_init, 
};

// Stages
enum stages {
    setupStage = 1,     // sets the colour of the leds
    waitRequest = 2,
    Dish1 = 4,
    Dish2 = 8,
    Dish3 = 16,
    Dish4 = 32,
    Dish5 = 64,
    WorldsEnd =128,
    DavidEnd = 256,
};

// the sum of all stages sprinkled with a bit of black magic
int stageSum = ~( ~0 << StageCount );
int dishStageSum = Dish1 | Dish2 | Dish3 | Dish4 | Dish5;

