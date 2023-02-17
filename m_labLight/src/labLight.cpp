/**
 * @file m_laserlock.cpp
 * @author Martin Pek (martin.pek@web.de)
 * @brief controls a 
 * inputs via RFID and Keypad
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022
 * 
 *  TODO: 
 * - double post of clean airlock & decontamination
 * - enumerating brain types
 * 🔲 add feedback to close the door when tryint to lock
 *  6. Wrong code entered on access module -> "Access denied" currently not implemented
 * ✅ add numbering of brains to header to make changes easiers
 * in between stage or text for unlocking with rfid
 *  Q:
 * 🔲 Timeout needs to send wrongres
 * 🔲 booting text on startup
 * 🔲 consider using laststage for the switch cases?
 * 
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



void enableWdt() {
    wdt_enable(WDTO_8S);
}



void handleInputs() {


    int result = MotherIO.getInputs();
    Serial.println(result);

    switch (result) {
        case lightOff: 
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 100);
        break;
        case lightNormal: 
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, clrLight, 40);
        break;
        case lightNormalBright:
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, clrLight, 60);
        break;
        case lightRed:
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrRed, 40);
        break;
        case lightRedBright:
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrRed, 60);
        break;
        default: break;
    }
    delay(5000);

}


void setup() {

    Mother.begin();
    // starts serial and default oled
    Mother.relayInit(relayPinArray, relayInitArray, relayAmount);
    MotherIO.ioInit(intputArray, sizeof(intputArray), outputArray, sizeof(outputArray));

    Serial.println("WDT endabled");
    enableWdt();

    // technicall 3 but for the first segments till unlocked there is no need
    Mother.rs485SetSlaveCount(0);
  
    wdt_reset();
}


void loop() {
    handleInputs();    
    wdt_reset();
}




