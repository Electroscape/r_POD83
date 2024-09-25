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
unsigned long lastChangeTime = 0; // Store the last time the result changed
const unsigned long TIME_THRESHOLD = 5000; 
unsigned long startTime = 0;
bool roomArmed = false;

void enableWdt() {
    wdt_enable(WDTO_8S);
}

void openCleanroom() {
    wdt_reset();
    delay(1000);
    Mother.motherRelay.digitalWrite(PNDOOR_PIN, closed);
    Mother.motherRelay.digitalWrite(KEYPAD_PIN, closed);
    
    LED_CMDS::running(Mother, ledBrain, LED_CMDS::clrRed, 100, 300, ZYL_LED_CNT, zyl_leds, 200);
    startTime = millis();
    while ((millis() - startTime) < (DOOR_OPENTIME * 1000)) {
        wdt_reset();
    }

    // LED_CMDS::setStripToClr(Mother, ledBrain, LED_CMDS::clrBlack, 100, led_strips::zyl_leds);
    LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrBlack, 100);
}

void runDecontamination() {
    wdt_reset();
    delay(3000);
    Mother.motherRelay.digitalWrite(RPI_VIDEO_PIN, !RPI_VIDEO_INIT);
    delay(1000);
    Mother.motherRelay.digitalWrite(RPI_VIDEO_PIN, RPI_VIDEO_INIT);
    
    LED_CMDS::setStripToClr(Mother, ledBrain, LED_CMDS::clrGreen, 100, led_strips::zyl_leds);

    Mother.motherRelay.digitalWrite(FR_LIGHTS_PIN, open); Serial.println("Light: off");
    wdt_reset();
    delay(2000);
    wdt_reset();
    Mother.motherRelay.digitalWrite(FANS_SMALL_PIN, !FANS_SMALL_INIT); Serial.println("Fan small: on");
    delay(3000);

    static constexpr int clr1[3] = {170, 95, 255};
    static constexpr int clr2[3] = {0, 255, 255};
    LED_CMDS::blinking(Mother, ledBrain, clr1, clr2, 1000, 1000, 100, 100, led_strips::fr_leds);


    for (int z=0; z<14; z++) {

        Serial.print("z: "); Serial.println(z);
        if (z==0) {Mother.motherRelay.digitalWrite(FOG_PIN, !FOG_INIT); Serial.println("Fog: on");}
        else if (z==5) {Mother.motherRelay.digitalWrite(FOG_PIN, FOG_INIT); Serial.println("Fog: off");}
        else if (z==6) {Mother.motherRelay.digitalWrite(FAN_OUT_BIG_PIN, !FAN_OUT_BIG_INIT); Serial.println("Fan OUT big: on");}
        else {}//Serial.print("ez: "); Serial.println(z);}

        Mother.motherRelay.digitalWrite(FR_LIGHTS_PIN, closed);
        wdt_reset();
        delay(1000);

        Mother.motherRelay.digitalWrite(FR_LIGHTS_PIN, FR_LIGHTS_INIT);
        wdt_reset();
        delay(1000);
    }

    Mother.motherRelay.digitalWrite(FR_LIGHTS_PIN, closed);
    // Green light -----------------------------------------------
    LED_CMDS::setStripToClr(Mother, ledBrain, LED_CMDS::clrGreen, 100, led_strips::fr_leds);
    Serial.println("LED: green");
    Mother.motherRelay.digitalWrite(FANS_SMALL_PIN, FANS_SMALL_INIT); Serial.println("Fan small: off");
    wdt_reset();
    delay(1000);
    Mother.motherRelay.digitalWrite(ROOM_LIGHT_PIN, open);
    Mother.motherRelay.digitalWrite(FR_LIGHTS_PIN, open); Serial.println("Light: on");


    // White light -----------------------------------------------
    LED_CMDS::setStripToClr(Mother, ledBrain, clr1, 100, led_strips::fr_leds);
    Serial.println("LED: white");

    // moved it forward 4s bec it really seems abrupt shutdown
    delay(1000);
    Mother.motherRelay.digitalWrite(FAN_OUT_BIG_PIN, FAN_OUT_BIG_INIT);

    wdt_reset();
    // was 4s but i moved delay of 1s to the main fan
    delay(3000);

    Mother.motherRelay.digitalWrite(KEYPAD_PIN, open);
    LED_CMDS::setStripToClr(Mother, ledBrain, LED_CMDS::clrGreen, 100, led_strips::zyl_leds);

    openCleanroom();
};


void triggerTimeoutAction() {
    Serial.println("Timeout triggered without input change!");
    // Add more actions you want to happen after the timeout
}

void handleInputs() {

    int result = MotherIO.getInputs();

    if (lastState == result) {
       if (result != 0) {
            // Check how much time has passed since last result change
            if (millis() - lastChangeTime > TIME_THRESHOLD) {
                // Trigger action after the time threshold has been reached
                roomArmed = true;
                Mother.motherRelay.digitalWrite(KEYPAD_PIN, closed);
                lastChangeTime = millis(); // Reset the timer after triggering
            }
        } else if (!roomArmed) {
            Mother.motherRelay.digitalWrite(PNDOOR_PIN, closed);
        }
        return;
    }
    lastState = result;
    lastChangeTime = millis(); // Reset time when result changes

    Serial.println(result);

    switch (result) {
        case IO::deconTrigger:
        if (roomArmed) {
            roomArmed = false;
            runDecontamination();
        }
        break;
        case IO::arming:
            Mother.motherRelay.digitalWrite(PNDOOR_PIN, open);
        default: break;
    }

}

void initRoom() {
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

    initRoom();
}

void loop() {
    handleInputs();   
    wdt_reset();
}




