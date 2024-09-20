/**
 * @file m_cleanroom.cpp
 * @author Martin Pek (martin.pek@web.de)
 * @brief controls a 
 * inputs via RFID and Keypad
 * @version 0.1
 * @date 2024-09-19
 * 
 * @copyright Copyright (c) 2022
 * 
 *  TODO:  
 */


#include <stb_mother.h>
#include <stb_mother_IO.h>
#include <stb_keypadCmds.h>
#include <stb_oledCmds.h>
#include <stb_mother_ledCmds.h>


#include "header_st.h"

// using the reset PCF for this
PCF8574 inputPCF;
STB_MOTHER Mother;
STB_MOTHER_IO MotherIO;

int lastState = -1;
int armingTicks = 0;


void enableWdt() {
    wdt_enable(WDTO_8S);
}

void open_cleanroom() {
    
}

void runDecontamination() {
    delay(3000);
    Mother.motherRelay.digitalWrite(RPI_VIDEO_PIN, !RPI_VIDEO_INIT);
    delay(1000);
    Mother.motherRelay.digitalWrite(RPI_VIDEO_PIN, RPI_VIDEO_INIT);
};

void handleInputs() {

    int result = MotherIO.getInputs();

    if (lastState == result || result == 0) {
        return;
    }
    lastState = result;

    unsigned long startTime = millis();
    Serial.println(result);

    switch (result) {
        case IO::arming:
            armingTicks++;
        break;
        case IO::deconTrigger:
            runDecontamination();
        break;
        default: break;
    }

}

void init_room() {
    LED_CMDS::setStripToClr(Mother, ledBrain, LED_CMDS::clrRed, 100, zyl_leds);
    LED_CMDS::setStripToClr(Mother, ledBrain, LED_CMDS::clrBlack, 100, fr_leds);
    
    // reset relays cmd?? may aswell add one ot the library if it doesnt exist
    Mother.relayInit(relayPinArray, relayInitArray, relayAmount);
    /*
    Mother.motherRelay.digitalWrite(ROOM_LIGHT_PIN, closed);
    Mother.motherRelay.digitalWrite(FR_LIGHTS_PIN, open);
    Mother.motherRelay.digitalWrite(RPI_VIDEO_PIN, RPI_VIDEO_INIT);
    */
}


void setup() {

    Mother.begin();
    // Mother.relayInit(relayPinArray, relayInitArray, relayAmount);
    MotherIO.ioInit(intputArray, sizeof(intputArray), outputArray, sizeof(outputArray));

    Serial.println("WDT endabled");
    enableWdt();

    // technicall 1 but for the first segments till unlocked there is no need
    Mother.rs485SetSlaveCount(0);

    init_room();
    wdt_reset();

}

void loop() {
    handleInputs();   
    wdt_reset();
}




