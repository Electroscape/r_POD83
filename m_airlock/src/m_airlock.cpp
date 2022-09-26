/**
 * @file m_airlock.cpp
 * @author Martin Pek (martin.pek@web.de)
 * @brief controls a gate, lights and beamer.
 * inputs via RFID and Keypad
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022
 * 
 *  TODO: 
 *  6. Wrong code entered on access module -> "Access denied" currently not implemented
 * 
 *  Q: 
 * 
 */


#include <stb_mother.h>
#include <stb_keypadCmds.h>
#include <stb_oledCmds.h>
#include <stb_mother_ledCmds.h>


#include "header_st.h"



STB_MOTHER Mother;
int stage = preStage;
// since stages are single binary bits and we still need to d some indexing
int stageIndex=0;
// doing this so the first time it updates the brains oled without an exta setup line
int lastStage = -1;
bool repeatDecontamination = false;



// since stages are binary bit being shifted we cannot use them to index
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
    delay(16000);
}



/**
 * @brief  consider just using softwareReset
*/
void gameReset() {
    stage = startStage;
    for (int relayNo=0; relayNo < relayAmount; relayNo++) {
        Mother.motherRelay.digitalWrite(relayNo, relayInitArray[relayNo]);
    }
    repeatDecontamination = false;
}


bool passwordInterpreter(char* password) {
    Mother.STB_.defaultOled.clear();
    for (int passNo=0; passNo < PasswordAmount; passNo++) {
        if (passwordMap[passNo] & stage) {
            if (strncmp(passwords[passNo], password, strlen(passwords[passNo]) ) == 0) {
                delay(500);
                stage = stage << 1;
                return true;
            }
        }
    }
    // specifics to a failed input of the password
    if ( stage == airlockRequest ) {
        stage = decontamination;
    }
    return false;
}



// candidate to be moved to a mother specific part of the keypad lib
bool checkForKeypad() {

    /*
    Mother.STB_.dbgln("checkforKeypad");
    Mother.STB_.dbgln(Mother.STB_.rcvdPtr);
    */

    if (strncmp(keypadCmd.c_str(), Mother.STB_.rcvdPtr, keypadCmd.length()) != 0) {
        return false;
    }
    Mother.sendAck();

    char *cmdPtr = strtok(Mother.STB_.rcvdPtr, KeywordsList::delimiter.c_str());
    cmdPtr = strtok(NULL, KeywordsList::delimiter.c_str());
    int cmdNo;
    sscanf(cmdPtr, "%d", &cmdNo);

    /* no evaluation requested, may just be an update for oled display on mother
    * or being used for things like an interface
    */
    if (cmdNo != KeypadCmds::evaluate) { return true; }

    cmdPtr = strtok(NULL, KeywordsList::delimiter.c_str());
    /*
    Serial.println("password is: ");
    Serial.println(cmdPtr);
    delay(500);
    */

    // TODO: error handling here in case the rest of the msg is lost?
    if (!(cmdPtr != NULL)) {
        // send NACK? this isnt in the control flow yet or simply eof?
        return false;
    }

    // prepare return msg with correct or incorrect
    char msg[10] = "";
    char noString[3];
    strcpy(msg, keypadCmd.c_str());
    strcat(msg, KeywordsList::delimiter.c_str());
    if (passwordInterpreter(cmdPtr)) {
        sprintf(noString, "%d", KeypadCmds::correct);
        strcat(msg, noString);
    } else {
        sprintf(noString, "%d", KeypadCmds::wrong);
        strcat(msg, noString);
    }
    // idk why but we had a termination poblem, maybe sprintf doesnt terminate?
    msg[strlen(msg) - 1] = '\0';

    strcat(msg, noString);
    Mother.sendCmdToSlave(msg);

    return true;
}

// again good candidate for a mother specific lib
bool checkForRfid() {

    if (strncmp(KeywordsList::rfidKeyword.c_str(), Mother.STB_.rcvdPtr, KeywordsList::rfidKeyword.length() ) != 0) {
        return false;
    } 
    char *cmdPtr = strtok(Mother.STB_.rcvdPtr, KeywordsList::delimiter.c_str());
    cmdPtr = strtok(NULL, KeywordsList::delimiter.c_str());
    
    // return msg with correct or incorrect
    // could be simply universal CodeCorrect
    char msg[10] = "";
    strcpy(msg, keypadCmd.c_str());
    strcat(msg, KeywordsList::delimiter.c_str());
    char noString[3];

    if (passwordInterpreter(cmdPtr)) {
        sprintf(noString, "%d", KeypadCmds::correct);
        strcat(msg, noString);
    } else {
        sprintf(noString, "%d", KeypadCmds::wrong);
        strcat(msg, noString);
    }
    Mother.sendCmdToSlave(msg);
    // blocking
    delay(5000);
    return true;
}


void interpreter() {
    while (Mother.nextRcvdLn()) {
        if (checkForKeypad()) { continue;}
        checkForRfid();
    }
}


void uvSequence () {
    wdt_disable();
    Mother.motherRelay.digitalWrite(uvLight, open);
    int repCnt = 4;
    if (repeatDecontamination) { repCnt = 2; }
    // are UV light supposed to be flashing or only LEDs? Assumed the latter 
    for (int rep=0; rep<repCnt; rep++) {
        for (int i=0; i<3; i++) {
            LED_CMDS::setToClr(Mother, 1, LED_CMDS::clrBlue, 100);
            LED_CMDS::setToClr(Mother, 1, LED_CMDS::clrBlack, 50);
        }
    if (rep != repCnt -1) { delay(3000); }
    }
    wdt_enable(WDTO_8S);
    Mother.motherRelay.digitalWrite(uvLight, closed);
}


void oledUpdate() {
    Mother.setFlags(0, flagMapping[stageIndex]);
    char msg[32];
    strcpy(msg, oledHeaderCmd.c_str());
    strcat(msg, KeywordsList::delimiter.c_str());
    strcat(msg, stageTexts[stageIndex]); 
    Mother.sendCmdToSlave(msg);
    lastStage = stage;
}


void stageActions() {
    oledUpdate();
    switch (stage) {
        case preStage:        
            LED_CMDS::setToClr(Mother, 1, LED_CMDS::clrWhite, 100);
            wdt_reset();
            delay(5000);
            stage = startStage;
        break;
        case startStage:    
            LED_CMDS::setToClr(Mother, 1, LED_CMDS::clrRed, 50);
        break;
        case intro: 
            Mother.motherRelay.digitalWrite(beamerIntro, open);
            LED_CMDS::setToClr(Mother, 1, LED_CMDS::clrBlack, 50);
            wdt_disable();
            delay(introDuration);
            wdt_enable(WDTO_8S);
            Mother.motherRelay.digitalWrite(beamerIntro, closed);
            stage = decontamination;
        break;
        // have to check if need some sort of synchronisation ... or have a bit of padding in the decontimnation video
        case decontamination:
            uvSequence();
            repeatDecontamination = true;
            stage = airlockRequest;
        break;
        case airlockRequest:
            LED_CMDS::setToClr(Mother, 1, LED_CMDS::clrGreen, 40);
        break;
        case airlockOpening:

        break;
        case endStage:
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
    setStageIndex();
        
    // check || stageIndex >= int(sizeof(stages))
    if (stageIndex < 0) {
        Serial.println(F("Stages out of index!"));
        delay(5000);
        wdt_reset();
    }
    stageActions();
}


void setup() {

    Mother.begin();
    // starts serial and default oled
    Mother.relayInit(relayPinArray, relayInitArray, relayAmount);

    Serial.println("WDT endabled");
    wdt_enable(WDTO_8S);

    // technicall 2 but no need to poll the 2nd 
    Mother.rs485SetSlaveCount(1);

    setStageIndex();
    Mother.setFlags(0, flagMapping[stageIndex]);
    Mother.setupComplete(0);
    int argsCnt = 2;
    int ledCount[argsCnt] = {0, 3};
    Mother.sendSetting(1, settingCmds::ledCount, ledCount, argsCnt);
    Mother.setupComplete(1);

    // smalle dealay to not load up the fuse by switching on too many devices at once
    wdt_reset();
    delay(1000);
    gameReset();
}


void loop() {
    Mother.rs485PerformPoll();
    interpreter();
    stageUpdate();
    wdt_reset();
}




