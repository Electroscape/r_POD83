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
};

const int lightOff = 1 << lightOff_pin;
const int lightNormal = 1 << lightNormal_pin;
const int lightNormalBright = 1 << lightNormalBright_pin;
const int lightRed = 1 << lightRed_pin; 
const int lightRedBright = 1 << lightRedBright_pin; 

#define outputCnt 0
#define inputCnt 5

static constexpr int clrLight[3] = {255,200,120};


int intputArray[inputCnt] = {
    lightOff_pin,             // red
    lightNormal_pin,      // black  
    lightNormalBright_pin,      // black  
    lightRed_pin,          // Green
    lightRedBright_pin,          // Green  
};

int outputArray[outputCnt] = {
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

