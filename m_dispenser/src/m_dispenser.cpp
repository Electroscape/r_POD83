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
//#include <stb_mother_IO.h>


#include "header_st.h"

// using the reset PCF for this
PCF8574 inputPCF;
STB_MOTHER Mother;

//STB_MOTHER_IO MotherIO;





void setup() {

    // starts serial and default oled
    Mother.begin();
    Mother.relayInit(relayPinArray, relayInitArray, relayAmount);
   // MotherIO.ioInit(relayPinArray, relayInitArray, relayAmount);

    Serial.println("WDT endabled");
    //wdt_enable(WDTO_8S);

    Mother.rs485SetSlaveCount(1);
    //wdt_reset();
}


void loop() {

    // Für Zwei Minuten Blubbern ohne Förderband
    Mother.motherRelay.digitalWrite(Relay1, closed);
    Mother.motherRelay.digitalWrite(Relay2, closed);
    Mother.motherRelay.digitalWrite(Relay3, closed);
    Mother.motherRelay.digitalWrite(Relay4, closed);
    Mother.motherRelay.digitalWrite(Relay5, closed);
    Mother.motherRelay.digitalWrite(Relay8, open);
    delay(120000); // 2min Blubbern

    // Säule Nummer 2 aus und dann Förderband in eine Richtung
    Mother.motherRelay.digitalWrite(Relay2, open);
    Mother.motherRelay.digitalWrite(Relay7, closed);
    Mother.motherRelay.digitalWrite(Relay6, closed);
    delay(100);
    Mother.motherRelay.digitalWrite(Relay8, closed);
    delay(45000); // 45s Förderband
    

    // Für Zwei Minuten Blubbern ohne Förderband
    Mother.motherRelay.digitalWrite(Relay8, open);
    Mother.motherRelay.digitalWrite(Relay2, open);
    delay(120000); // 2min Blubbern 

    // Säule Nummer 2 aus und dann Förderband in die andere Richtung
    Mother.motherRelay.digitalWrite(Relay2, open);
    Mother.motherRelay.digitalWrite(Relay7, open);
    Mother.motherRelay.digitalWrite(Relay6, open);
    delay(100);
    Mother.motherRelay.digitalWrite(Relay8, closed);
    delay(45000); // 45s Förderband

    


    //wdt_reset();
}




