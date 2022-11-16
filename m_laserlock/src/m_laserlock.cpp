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
 *  6. Wrong code entered on access module -> "Access denied" currently not implemented
 * add numbering of brains to header to make changes easiers
 *  Q: 
 * 
 */


#include <stb_mother.h>
#include <stb_keypadCmds.h>
#include <stb_oledCmds.h>
#include <stb_mother_ledCmds.h>


#include "header_st.h"

// using the reset PCF for this
PCF8574 inputPCF;
STB_MOTHER Mother;
int stage = setupStage;
// since stages are single binary bits and we still need to d some indexing
int stageIndex = 0;
// doing this so the first time it updates the brains oled without an exta setup line
int lastStage = -1;
int inputTicks = 0;

// general timestamp going to use this to timeout the card repsentation in unlocked and RFIDoutput
unsigned long timestamp = millis();


void enableWdt() {
    wdt_enable(WDTO_8S);
}


/**
 * @brief Set the Stage Index object
 * @todo safety considerations
*/
void setStageIndex() {
    for (int i=0; i<StageCount; i++) {
        if (stage <= 1 << i) {
            stageIndex = i;
            Serial.print("stageIndex:");
            Serial.println(stageIndex);
            delay(1000);
            return;
        }
    }
    Serial.println(F("STAGEINDEX ERRROR!"));
    wdt_reset();
    delay(16000);
}


/**
 * @brief  consider just using softwareReset
*/
void gameReset() {
    stage = setupStage;
    for (int relayNo=0; relayNo < relayAmount; relayNo++) {
        Mother.motherRelay.digitalWrite(relayNo, relayInitArray[relayNo]);
    }
}


/**
 * @brief handles timeouts rfidTX and timeout of the RFID presentation
*/
void timedTrigger() {
    if (timestamp > millis()) { return; }
    switch (stage) {
        case unlock: 
            stage = failedUnlock;
        break;
    }
}


// sets txRelays to 0 to indicate empty RFID presence, may be called after a timeout from the mainloop to keep timing simpler
void outputRFIDReset() {
    for (int pinNo=0; pinNo<txRelayAmount; pinNo++) {
        Mother.motherRelay.digitalWrite(rfidTxPins[pinNo], closed);
    }
}


/**
 * @brief converts the passwordindex to a signal on the relays, also leftshifted the passed index once
 * 0 for no RFID
 * 1 for David
 * 2 for Rachel
 * @param index 
*/
void outputRFID(int index) {
    index = index << 1;
    for (int txPin=0; txPin<txRelayAmount; txPin++) {
        Mother.motherRelay.digitalWrite(rfidTxPins[txPin], ((index & (1 << txPin)) > 0) );
    }
    timestamp = millis() + rfidTxDuration;
}


bool passwordInterpreter(char* password) {
    Mother.STB_.defaultOled.clear();
    for (int passNo=0; passNo < PasswordAmount; passNo++) {
        if (passwordMap[passNo] & stage) {
            if ( strlen(passwords[passNo]) == strlen(password) &&
                strncmp(passwords[passNo], password, strlen(passwords[passNo]) ) == 0
            ) {
                delay(500);
                outputRFID(passNo);
                if (stage == unlock) { stage = unlocked; }
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief handles evalauation of codes and sends the result to the access module
 * @param cmdPtr 
*/
void handleResult(char *cmdPtr) {
    cmdPtr = strtok(NULL, KeywordsList::delimiter.c_str());

    // prepare return msg with correct or incorrect
    char msg[10] = "";
    char noString[3] = "";
    strcpy(msg, keypadCmd.c_str());
    strcat(msg, KeywordsList::delimiter.c_str());
    if (passwordInterpreter(cmdPtr) && (cmdPtr != NULL)) {
        sprintf(noString, "%d", KeypadCmds::correct);
        strcat(msg, noString);
    } else {
        sprintf(noString, "%d", KeypadCmds::wrong);
        strcat(msg, noString);
    }
  
    Mother.sendCmdToSlave(msg);
}

// again good candidate for a mother specific lib
bool checkForRfid() {
    if (strncmp(KeywordsList::rfidKeyword.c_str(), Mother.STB_.rcvdPtr, KeywordsList::rfidKeyword.length() ) != 0) {
        return false;
    } 
    char *cmdPtr = strtok(Mother.STB_.rcvdPtr, KeywordsList::delimiter.c_str());
    handleResult(cmdPtr);
    wdt_reset();
    return true;
}


void interpreter() {
    while (Mother.nextRcvdLn()) {
        checkForRfid();
    }
}

void oledUpdate() {
    char msg[32] = "";
    strcpy(msg, oledHeaderCmd.c_str());
    strcat(msg, KeywordsList::delimiter.c_str());
    strcat(msg, stageTexts[stageIndex]); 
    Mother.sendCmdToSlave(msg);
}


void oledFailed() {
    timestamp = millis() + displayFailedUnlock;
    char timeoutMsg[32] = "";
    strcpy(timeoutMsg, oledHeaderCmd.c_str());
    strcat(timeoutMsg, KeywordsList::delimiter.c_str());
    strcat(timeoutMsg, "Timeout"); 
    char cleanMsg[32] = "";
    strcpy(cleanMsg, oledHeaderCmd.c_str());
    strcat(cleanMsg, KeywordsList::delimiter.c_str());
    strcat(cleanMsg, stageTexts[stageIndex]); 
    while (timestamp > millis()) {
        wdt_reset();
        Mother.sendCmdToSlave(timeoutMsg);
        delay(1500);
        Mother.sendCmdToSlave(cleanMsg);
        delay(1500);
    }  
}


void stageActions() {
    wdt_reset();
    switch (stage) {
        case setupStage: 
            LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrBlack, 100, 0);
            stage = idle;
        break;
        case failedBoot:
            wdt_disable();
            LED_CMDS::fade2color(Mother, ledBrain, LED_CMDS::clrBlack, 100, LED_CMDS::clrRed, 50, 10000, PWM::set1);
            delay(10000);

            // total duration 5s
            LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrBlack, 100);
            delay(25);
            LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrRed, 50);
            delay(50);
            LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrBlack, 100);
            delay(25);
            LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrRed, 40);
            delay(60);
            LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrBlack, 100);
            delay(25);
            LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrRed, 35);
            delay(25);
            LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrBlack, 100);
            delay(25);
            LED_CMDS::fade2color(Mother, ledBrain, LED_CMDS::clrRed, 30, LED_CMDS::clrRed, 0, 4775, PWM::set1);
            delay(4775);
            enableWdt();
            stage = idle;
        break;
        case operational:
            LED_CMDS::fade2color(Mother, ledBrain, LED_CMDS::clrRed, 0, LED_CMDS::clrRed, 80, 10000, PWM::set1);
            wdt_disable();
            delay(10000);
            enableWdt();
            stage = idle;
        break;
        case decon:
            // @todo: check timing here
            int runTime;
            for (int brightness = 10; brightness <= 100; brightness += 10) {
                runTime = (100 - brightness) * 5;  // loop should be a total of 8100ms
                if (runTime < 20) { runTime = 50; }
                LED_CMDS::running(Mother, ledBrain, LED_CMDS::clrBlue, brightness, runTime, ledCnt, PWM::set1, 1000);
                delay(runTime);
                wdt_reset();
            }
            LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrBlack, 100);
            delay(100);      // 8130
            // LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrWhite, 100, 0);
            // doesnt specify fade but may aswell see how it works
            LED_CMDS::fade2color(Mother, ledBrain, LED_CMDS::clrWhite, 100, LED_CMDS::clrBlue, 50, 6830, PWM::set1);
            timestamp = millis() + presentationTime;
            stage = unlock;
        break;
        case unlock:
        break;
        case failedUnlock:
            LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrBlack, 100);
            delay(50);
            LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrRed, 100);
            delay(200);
            LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrBlack, 100);
            delay(200);
            LED_CMDS::fade2color(Mother, ledBrain, LED_CMDS::clrRed, 100, LED_CMDS::clrRed, 80, displayFailedUnlock,  PWM::set1);
            oledFailed();
            stage = idle;
        break;
        case unlocked:
            LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrGreen, 50);
            Mother.motherRelay.digitalWrite(door, doorOpen);
            delay(5000);
            outputRFIDReset();
            wdt_reset();
            LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrWhite, 20);
            wdt_disable();
            delay(15000);
            enableWdt();
            stage = idle;
        break; 
    }
}


/**
 * @brief  triggers effects specific to the given stage, 
 * room specific excecutions can happen here
 * TODO: avoid reposts of setflags, but this is an optimisation
*/
void stageUpdate() {
    if (lastStage == stage) { return; }
    Serial.print("Stage is:");
    Serial.println(stage);
    setStageIndex();
        
    // check || stageIndex >= int(sizeof(stages))
    if (stageIndex < 0) {
        Serial.println(F("Stages out of index!"));
        delay(15000);
        
    }
    // important to do this before stageActions! otherwise we skip stages
    lastStage = stage;

    Mother.setFlags(0, flagMapping[stageIndex]);
    oledUpdate();
    stageActions();
}


void inputInit() {
    for (int pin=0; pin<inputCnt; pin++) {
        inputPCF.begin(RESET_I2C_ADD);
        inputPCF.pinMode((uint8_t) pin, INPUT_PULLUP);
    }
}


/**
 * @brief  handles inputs passed from the RPi and trigger stages
 * @todo make this shorter and easier to read and understand
*/
int inputDetector() {

    int ticks;
    for (int pin=0; pin<inputCnt; pin++) {
        if (pin == connectionFixed) { continue; }
        ticks = 0;
        while(!inputPCF.digitalRead(pin)) {
            if (ticks > 5) {
                return pin;
            }
            ticks++;
        }
    }

    return -1;
}

void handleRpiInputs() {

    if (stage != idle) { return; }
    lastStage = idle;

    switch (inputDetector()) {
        case bootTrigger: 
            if (!inputPCF.digitalRead(connectionFixed)) {
                Serial.println("setting operational");
                delay(5000);
                stage = operational;
            } else {
                Serial.println("setting failed");
                delay(5000);
                stage = failedBoot;
            }
        break;
        case deconTrigger: 
            Serial.println("setting decon");
            delay(5000);
            stage = decon;
        break;
        case resetTrigger:
            Mother.motherRelay.digitalWrite(door, doorClosed);
            LED_CMDS::setAllStripsToClr(Mother, ledBrain, LED_CMDS::clrRed, 80);
            // not necessary but makes sense for safety
            outputRFIDReset();
        break;
    }

}


void setup() {

    Mother.begin();
    // starts serial and default oled
    Mother.relayInit(relayPinArray, relayInitArray, relayAmount);

    Serial.println("WDT endabled");
    enableWdt();

    // technicall 2 but no need to poll the 2nd 
    Mother.rs485SetSlaveCount(1);

    setStageIndex();
    inputInit();

    /*
    Mother.setFlags(0, flagMapping[stageIndex]);
    Mother.setupComplete(0);
    */
    /*
    int argsCnt = 2;
    int ledCount[argsCnt] = {0, 3};
    Mother.sendSetting(1, settingCmds::ledCount, ledCount, argsCnt);
    Mother.setupComplete(1);
    */

    wdt_reset();
    delay(1000);
    gameReset();
}


void loop() {
    Mother.rs485PerformPoll();
    interpreter();
    timedTrigger();
    handleRpiInputs();    
    stageUpdate();
    wdt_reset();
}




