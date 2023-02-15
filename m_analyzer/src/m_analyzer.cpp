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
int stage = setupStage;
// since stages are single binary bits and we still need to d some indexing
int stageIndex=0;
// doing this so the first time it updates the brains oled without an exta setup line
int lastStage = -1;

struct RFID_Check 
{
    char* str;
    int status = 0; // 0 = no Card; 1 = Card; 2 = right Card
    int status_old = 0;
};
RFID_Check RFID[4];
unsigned long lastRfidCheck = millis();
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
}

void passwordActions(int passNo) { // also used for RFID Card Reader
  switch (stage) {
        case setupStage:
            switch (passNo) {
                default: 
                    //PetriDishStatus[passNo] = true;
                break;
            }
        break;
        case runMode1:
        break;
    }


}
bool passwordInterpreter() {
    bool found  = false;
    //Serial.println("passwordInterpreter:");
    for (int i_RFID=0; i_RFID < 4; i_RFID++){
        char* password = RFID[i_RFID].str;
        //Serial.println(password);
        for (int passNo=0; passNo < PasswordAmount; passNo++) {
            if (passwordMap[passNo] & stage) {
                if ( ( strlen(passwords[passNo]) == strlen(password) ) &&
                    strncmp(passwords[passNo], password, strlen(passwords[passNo]) ) == 0
                ) {       
                found = true;
                if (passNo == i_RFID)
                {
                    RFID[i_RFID].status = 2; // Card at right position
                }
                else if (passNo == 4 )
                {
                    RFID[i_RFID].status = 0; // No Card
                }
                else 
                {
                    RFID[i_RFID].status = 1; // Card at wrong position
                }
                }
            }
        }        
    }
    return found;
}

/**
 * @brief handles evalauation of codes and sends the result to the access module
 * @param cmdPtr 
*/
void handleResult(char *cmdPtr) {
    //Serial.println(cmdPtr);
    cmdPtr = strtok(NULL, KeywordsList::delimiter.c_str());
    RFID[0].str = cmdPtr;
    cmdPtr = strtok(NULL, KeywordsList::delimiter.c_str());
    RFID[1].str = cmdPtr;
    cmdPtr = strtok(NULL, KeywordsList::delimiter.c_str());
    RFID[2].str = cmdPtr;
    cmdPtr = strtok(NULL, KeywordsList::delimiter.c_str());
    RFID[3].str = cmdPtr;
    /* Serial.println( RFID[0].str);
    Serial.println( RFID[1].str);
    Serial.println( RFID[2].str);
    Serial.println( RFID[3].str); */
    // Update Status with interpreter

    if (passwordInterpreter() && (cmdPtr != NULL)) {
        // Do as planned
        int stripNo;
        int checkSum;
        Serial.print(RFID[0].status);
        Serial.print(RFID[1].status);
        Serial.print(RFID[2].status);
        Serial.println(RFID[3].status);
        checkSum = RFID[0].status+RFID[1].status+RFID[2].status+RFID[3].status;
        // Stage runMode 1
        if (RFID[0].status > 0){
            Mother.motherRelay.digitalWrite(Relay1, closed);
        }
        else{
            Mother.motherRelay.digitalWrite(Relay1, open);
        }
        if (RFID[1].status > 0){
            Mother.motherRelay.digitalWrite(Relay2, closed);
        }
        else{
            Mother.motherRelay.digitalWrite(Relay2, open);
        }
        if (RFID[2].status > 0){
            Mother.motherRelay.digitalWrite(Relay3, closed);
        }
        else{
            Mother.motherRelay.digitalWrite(Relay3, open);
        }

        if (stage == runMode1){
            for (int i_RFID=0; i_RFID < 4; i_RFID++){
                if (RFID[i_RFID].status != RFID[i_RFID].status_old){
                    if (RFID[i_RFID].status > 0){
                        switch (i_RFID)
                        {
                        case 0:stripNo = set1; break;
                        case 1:stripNo = set2; break;
                        case 2:stripNo = set3; break;
                        case 3:stripNo = set4; break;                    
                        default:
                            break;
                        }
                        LED_CMDS::fade2color(Mother,0,LED_CMDS::clrWhite,10,LED_CMDS::clrWhite,50,1000,stripNo);
                        delay(1100);
                        if (RFID[0].status > 0 && RFID[1].status > 0 && RFID[2].status > 0 && RFID[3].status > 0){
                            stage = analyze;
                            return;
                        }
                        LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrBlack,100,stripNo);
                        delay(200);  
                        LED_CMDS::setAllStripsToClr(Mother,0,LED_CMDS::clrRed,100);
                        delay(100);  
                        LED_CMDS::setAllStripsToClr(Mother,0,LED_CMDS::clrBlack,100);
                        delay(100);  
                        LED_CMDS::setAllStripsToClr(Mother,0,LED_CMDS::clrRed,100);
                        delay(100);  
                        LED_CMDS::setAllStripsToClr(Mother,0,LED_CMDS::clrBlack,100);
                        delay(100); 
                        LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrWhite,40,stripNo);
                        for (int i_RFID=0; i_RFID < 4; i_RFID++){
                            if (RFID[i_RFID].status > 0){
                            switch (i_RFID)
                            {
                            case 0:stripNo = set1; break;
                            case 1:stripNo = set2; break;
                            case 2:stripNo = set3; break;
                            case 3:stripNo = set4; break;                    
                            default:
                                break;
                            }
                            LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrWhite,40,stripNo);
                            }
                        }
                        }
                    else if(RFID[i_RFID].status == 0){LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrBlack,100,stripNo);}   
                    RFID[i_RFID].status_old =RFID[i_RFID].status;
                }        
            }
        }
        else if (stage == runMode1_fast)
        {          
            for (int i_RFID=0; i_RFID < 4; i_RFID++){
                if (RFID[i_RFID].status != RFID[i_RFID].status_old){
                    
                    if (checkSum == 8){ // Right Solution found -> change state
                        stage = firstSolution;
                        return;
                    }
                    if (checkSum > 3){
                        wdt_disable();   
                        LED_CMDS::setAllStripsToClr(Mother,0,LED_CMDS::clrBlack,100);
                        delay(100);  
                        LED_CMDS::setAllStripsToClr(Mother,0,LED_CMDS::clrRed,100);
                        delay(100);  
                        LED_CMDS::setAllStripsToClr(Mother,0,LED_CMDS::clrBlack,100);
                        delay(100);  
                        LED_CMDS::setAllStripsToClr(Mother,0,LED_CMDS::clrRed,100);
                        delay(100);  
                        LED_CMDS::setAllStripsToClr(Mother,0,LED_CMDS::clrBlack,100);
                        delay(200);  
                        LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrBlue,100,set1);
                        delay(100);
                        LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrGreen,100,set2);
                        delay(100);
                        LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrRed,100,set3);
                        delay(100);
                        LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrYellow,100,set4);                  
                        wdt_enable(WDTO_8S);
                    }
                    else{
                        for (int i_RFID=0; i_RFID < 4; i_RFID++){
                            if (RFID[i_RFID].status > 0){
                                switch (i_RFID){
                                    case 0:stripNo = set1; break;
                                    case 1:stripNo = set2; break;
                                    case 2:stripNo = set3; break;
                                    case 3:stripNo = set4; break;                    
                                    default: stripNo = -1;
                                }
                                if (stripNo >=0 ){
                                    LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrWhite,40,stripNo);
                                }
                            }

                        }
                    }                    
                }
            }
        } 
  
    }
}


// again good candidate for a mother specific lib
bool checkForRfid() {

    if (strncmp(KeywordsList::rfidKeyword.c_str(), Mother.STB_.rcvdPtr, KeywordsList::rfidKeyword.length() ) != 0) {
        return false;
    } 
    Serial.println( Mother.STB_.rcvdPtr);
    char *cmdPtr = strtok(Mother.STB_.rcvdPtr, KeywordsList::delimiter.c_str());
    handleResult(cmdPtr);
    wdt_reset();
    return true;
}


void interpreter() {  
    //Serial.println(Mother.nextRcvdLn());
    while (Mother.nextRcvdLn()) {
        checkForRfid();
    }
}
 

/**
 * @brief  its a big stage so its a seperate function, split could be when
*/
void setupRoom() {
    wdt_disable();   
     //LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrRed, 100);
    Serial.println("ColorTest");
    LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrBlue,100,set1);
    delay(100);
    LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrGreen,100,set2);
    delay(100);
    LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrRed,100,set3);
    delay(100);
    LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrYellow,100,set4);
    delay(6000);

   wdt_enable(WDTO_8S);
}



void stageActions() {
    wdt_reset();
    switch (stage) {
        case setupStage:
            setupRoom();
            stage = runMode1;
        break;
        case runMode1: // set the 4 dishs until right setting
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100);
        break;
        case analyze: // After 4 dishs are placed run Analyze LightShow
            wdt_disable();   
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100);
            delay(100);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrYellow, 100);
            delay(200);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100);
            delay(100);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrYellow, 100);
            delay(200);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100);
            delay(100);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlue, 100);
            delay(200);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100);
            delay(100);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlue, 100);
            delay(200);
            wdt_enable(WDTO_8S);
            stage = runMode1_fast;       
        break;
        case runMode1_fast: // set the 4 dishs until right setting
            LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrBlue,100,set1);
            delay(20);
            LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrGreen,100,set2);
            delay(20);
            LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrRed,100,set3);
            delay(20);
            LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrYellow,100,set4); 
        break;
        case firstSolution: // After 4 dishs are placed right possible to set the last plate
            
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
        delay(5000);
        wdt_reset();
    }
    // important to do this before stageActions! otherwise we skip stages
    lastStage = stage;

    Mother.setFlags(0, flagMapping[stageIndex]);
    stageActions();
}



void setup() {

    // starts serial and default oled
    Mother.begin();
    Mother.relayInit(relayPinArray, relayInitArray, relayAmount);

    Serial.println("WDT endabled");
    wdt_enable(WDTO_8S);

    Mother.rs485SetSlaveCount(1);
    setupRoom();
    setStageIndex();
    wdt_reset();
    delay(1000);
    gameReset();
}


void loop() {
    
    Mother.motherRelay.digitalWrite(1, open);
    Mother.motherRelay.digitalWrite(Relay3, open);

    delay(1000);
    Mother.motherRelay.digitalWrite(1, closed);
    
    Mother.motherRelay.digitalWrite(Relay3, closed);

    delay(1000);



    if (millis() - lastRfidCheck < rfidCheckInterval) {
        return;
    }

    Mother.rs485PerformPoll();
    interpreter();
    stageUpdate();
    wdt_reset();
}




