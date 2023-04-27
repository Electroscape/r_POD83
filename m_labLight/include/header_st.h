#pragma once

#define HHVersion   1


#define relayAmount 4

#define open        0
#define closed      1

enum relays {
    labEntry,
    labExit,
    cleanRoomDoor, 
    cleanRoomDecon,
};

enum relayInits {
    labEntry_init = closed,
    labExit_init = closed,
    cleanRoomDoor_init = closed,
    cleanRoomDecon_init = closed,
};

int relayPinArray[relayAmount] = {
    labEntry,
    labExit,
    cleanRoomDoor,
    cleanRoomDecon
};

int relayInitArray[relayAmount] = {
    labEntry_init,
    labExit_init,
    cleanRoomDoor_init,
    cleanRoomDecon_init
};

enum brains {
    ledCeilBrain = 3
};


// --- Inputs IGNORE THE NAMING SHEME, this is already transmitted as binary
enum IOPins {
    lightOff_pin,             // red
    lightNormal_pin,      // black  
    lightNormalBright_pin,      // black  
    lightRed_pin,          // Green
    lightRedBright_pin,          // Green
    lightBlue_pin,          // Green
    door_pin,          // Green
    inverted_door_pin,
};


enum IO {
    lightOff = 1,
    lightNormal,
    lightRed,
    lightBlue,
    lightRachelAnnouncement,
    lightRachelEnd,
    lightDavidAnnouncement,
    lightDavidEnd,
    labDoorLock,
    labDoorUnlock,
    deconTrigger,
    cleanRoomDoorTrigger, // 12
};

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

