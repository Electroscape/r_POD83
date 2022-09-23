/**
 * @file m_access_mother_skeleton.cpp
 * @author Martin Pek (martin.pek@web.de)
 * @brief 
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022
 * 
 *  TODO: 
 *  - check passwords and passwordsmap usages 
 *  - consider a generic code/combination evaluation
 *  - map actions to passwords, change stage in these actions
 *  - consider non blocking 
 *  - consider stage to stageIndex conversion?
 */


#include <stb_mother.h>
#include <stb_keypadCmds.h>
#include <stb_oledCmds.h>
#include <stb_mother_ledCmds.h>


#include "header_st.h"



STB_MOTHER Mother;
int stage = gameLive;
// since stages are single binary bits and we still need to d some indexing
int stageIndex=0;
// doing this so the first time it updates the brains oled without an exta setup line
int lastStage = -1;
bool lockerStatuses[lockerCnt] = {false};


// since stages are binary bit being shifted we cannot use them to index
void setStageIndex() {
    stageIndex = (stage & stageSum) -1;
}


void lockAllDoors() {
    for (int relayNo=0; relayNo<relayAmount; relayNo++) {
        Mother.motherRelay.digitalWrite(relayNo, closed);
        LED_CMDS::setToClr(Mother, 1, LED_CMDS::clrRed, 50);
    }
    
}


/**
 * @brief  TODO: implement
 * 
*/
void gameReset() {
    for (int no=0; no<lockerCnt; no++) {
        lockerStatuses[no] = false;
    }
    stage = gameLive;
}


/**
 * @brief  room specific section
 * @param passNo 
*/
void passwordActions(int passNo) {
    switch (stage) {
        case gameLive:
            switch (passNo) {
                case service: 
                    stage = serviceMode; 
                    lockAllDoors();
                    Mother.motherRelay.digitalWrite(service, open);
                break;
                case resetIndex: gameReset(); break;
                default: 
                    lockerStatuses[passNo] = true;
                    Mother.motherRelay.digitalWrite(passNo, open);
                    LED_CMDS::setPixelToClr(Mother, passNo, LED_CMDS::clrGreen, 50, 1);
                break;
            }
        break;
        case serviceMode:
            stage = gameLive;
            switch (passNo) {
                case service: Mother.motherRelay.digitalWrite(service, closed); break;
                default: gameReset();
            }
        break;
    }
}


bool passwordInterpreter(char* password) {
    Mother.STB_.defaultOled.clear();
    for (int passNo=0; passNo < PasswordAmount; passNo++) {
        if (passwordMap[passNo] & stage) {
            if (strncmp(passwords[passNo], password, strlen(passwords[passNo]) ) == 0) {
                passwordActions(passNo);
                delay(500);
                return true;
            }
        }
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


void interpreter() {
    while (Mother.nextRcvdLn()) {
        checkForKeypad();
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
    lockAllDoors();
    Mother.setFlags(0, flagMapping[stageIndex]);

    char msg[32];
    strcpy(msg, oledHeaderCmd.c_str());
    strcat(msg, KeywordsList::delimiter.c_str());
    strcat(msg, stageTexts[stageIndex]); 
    Mother.sendCmdToSlave(msg);
    lastStage = stage;
}


void setup() {

    Mother.begin();
    // starts serial and default oled
    Mother.relayInit(relayPinArray, relayInitArray, relayAmount);

    Serial.println("WDT endabled");
    wdt_enable(WDTO_8S);

    // technicall 2 but no need to poll the 2nd 
    Mother.rs485SetSlaveCount(1);

    gameReset();
}


void loop() {
    Mother.rs485PerformPoll();
    interpreter();
    stageUpdate();
    wdt_reset();
}




