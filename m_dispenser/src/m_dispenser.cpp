/**
 * @file m_analyzer.cpp
 * @author Christian Walter (pauseandplay@cybwalt.de)
 * @brief controls lights
 * inputs via RFID 
 * @version 0.1
 * @date 2023-02-03
 * 
 * @copyright Copyright (c) 2023
 * 
 *  TODO: 
 * 
 *  Q: 
 * 
 */


#include <stb_mother.h>
#include <stb_mother_ledCmds.h>


#include "header_st.h"

// using the reset PCF for this
PCF8574 inputPCF;
STB_MOTHER Mother;






void setup() {

    // starts serial and default oled
    Mother.begin();
    Mother.relayInit(relayPinArray, relayInitArray, relayAmount);

    Serial.println("WDT endabled");
    //wdt_enable(WDTO_8S);

    Mother.rs485SetSlaveCount(1);
    //wdt_reset();
}


void loop() {
    
    Mother.motherRelay.digitalWrite(Relay8, closed);
    delay(100);
    Mother.motherRelay.digitalWrite(Relay8, open);

    delay(8000);
    Mother.motherRelay.digitalWrite(Relay8, closed);
    delay(100);
    Mother.motherRelay.digitalWrite(Relay7, open);
    Mother.motherRelay.digitalWrite(Relay6, open);
    delay(100);
    Mother.motherRelay.digitalWrite(Relay8, open);

    delay(5000);


    //wdt_reset();
}




