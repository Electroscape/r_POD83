#pragma once

#define relayAmount 0




enum brains {
    ledCeilBrain = 4
};


// --- Inputs
enum IO {
    lightOff_pin,             // red
    lightNormal_pin,      // black  
    lightNormalBright_pin,      // black  
    lightRed_pin,          // Green
    lightRedBright_pin,          // Green
    lightBlue_pin,          // Green
    door_pin,          // Green
    inverted_door_pin,
};

const int lightOff = 1 << lightOff_pin;
const int lightNormal = 1 << lightNormal_pin;
const int lightNormalBright = 1 << lightNormalBright_pin;
const int lightRed = 1 << lightRed_pin; 
const int lightRedBright = 1 << lightRedBright_pin; 
const int lightBlue = 1 << lightBlue_pin; 
const int door = 1 << door_pin;
const int inverted_door = 1 << inverted_door_pin;

#define outputCnt 1
#define inputCnt 7

static constexpr int clrLight[3] = {255,200,120};


int intputArray[inputCnt] = {
    lightOff_pin,             // red
    lightNormal_pin,      // black  
    lightNormalBright_pin,      // black  
    lightRed_pin,          // Green
    lightRedBright_pin,          // Green  
    lightBlue_pin,          // Green
    door_pin,
};

int outputArray[outputCnt] = {
    inverted_door_pin
};


// -- relays
enum relays {
};

enum relayInits {
};

int relayPinArray[relayAmount] = {
};

int relayInitArray[relayAmount] = {
};

