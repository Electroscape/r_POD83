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
            //if (passwordMap[passNo] & stage) { // dont use always the same Passwords
            if ( ( strlen(passwords[passNo]) == strlen(password) ) &&
               strncmp(passwords[passNo], password, strlen(passwords[passNo]) ) == 0
               ) {       
                found = true;
                if(stage == runMode2){
                    if (passNo == i_RFID){ // better with switch, bnut how more cases at cpp?
                    RFID[i_RFID].status = 2; // Card at right position
                    }
                    else if (passNo == 4 ){
                        RFID[i_RFID].status = 0; // No Card
                    }
                    else{
                        RFID[i_RFID].status = 1; // Card at wrong position
                    }
                    return found; // return for setting runmode2 Configuration
                }
                if (passNo == i_RFID){ // better with switch, bnut how more cases at cpp?
                    RFID[i_RFID].status = 2; // Card at right position
                }
                else if (passNo == 4 ){
                    RFID[i_RFID].status = 0; // No Card
                }
                else if (passNo == 5 ){ // P5!!
                    RFID[i_RFID].status = 3; // Dish No5!
                }
                else{
                    RFID[i_RFID].status = 1; // Card at wrong position
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
        int checkSum = 0;
        int setwhite = 0;
        int presented = 0;
        int RFID_changed = 0;
        Serial.print(RFID[0].status);
        Serial.print(RFID[1].status);
        Serial.print(RFID[2].status);
        Serial.println(RFID[3].status);
        checkSum = RFID[0].status+RFID[1].status+RFID[2].status+RFID[3].status;
        wdt_disable();  
        // Stage runMode 1 if a Dish is placed LEDs should get white, blink red and stay at this position on white until all 4 dishes are placed
        switch(stage){
            case runMode1:
                if (RFID[0].status > 0 && RFID[1].status > 0 && RFID[2].status > 0 && RFID[3].status > 0){ //change stage if all dishes are placed
                    stage = analyze;
                    return;
                }
                for (int i_RFID=0; i_RFID < 4; i_RFID++){ // Check all RFIDs if a new Dish is presented at light up to white
                    if (RFID[i_RFID].status == RFID[i_RFID].status_old){ continue;}//only if status of RFID has changed, no return because all RFID neede to be checked!
                    //if (RFID[i_RFID].status != RFID[i_RFID].status_old){ 
                    RFID[i_RFID].status_old = RFID[i_RFID].status;
                    if (RFID[i_RFID].status > 0){
                        presented = 1; // a new Dish is laying on the analyzer
                        switch (i_RFID){
                            case 0:stripNo = set1; break;
                            case 1:stripNo = set2; break;
                            case 2:stripNo = set3; break;
                            case 3:stripNo = set4; break;
                            }
                        LED_CMDS::fade2color(Mother,0,LED_CMDS::clrBlack,100,LED_CMDS::clrWhite,50,500,stripNo); //light up
                    }
                }/* 
                for(int i_RFID = 0; i_RFID < 4; i_RFID++){ //check where a Dish is laying
                    if (RFID[i_RFID].status > 0){
                        setwhite += int(1 << i_RFID); //Bitshifting magic is int() necassary, in Python it is???
                    }
                } */
                if (presented > 0){ // only if any new Dish is presented but not in first loop because it is only once needed
                    presented = 0;
                    delay(500); // wait for white light up
                    LED_CMDS::blinking(Mother,0,LED_CMDS::clrBlack,LED_CMDS::clrRed,50,100,100,100,PWM::set1_2_3_4); // let all LEDs blink red!
                    delay(1000); // red blinking delay
                    LED_CMDS::setAllStripsToClr(Mother,0,LED_CMDS::clrBlack,100); 
                }
                
                for(int i_RFID = 0; i_RFID < 4; i_RFID++){ //check where a Dish is laying
                    if (RFID[i_RFID].status > 0){
                        LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrWhite,40,i_RFID);
                    }
                }
                //LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrWhite,40,setwhite); //Set all LEDs together can also be done to reset if nothing is presented                 
            break;
            case(runMode1_fast): // without the light up and analyze, it should blink red for short if it is the wrong combination and gives color hint
                if (checkSum == 8){ // Right Solution found -> change state
                        stage = firstSolution;
                        return;
                }
                for(int i_RFID = 0; i_RFID < 4; i_RFID++){ // check if there is any change
                    if (RFID[i_RFID].status == RFID[i_RFID].status_old){ continue;}
                    RFID_changed = 1;
                }
                if (RFID_changed == 0){return;}
                if (RFID[0].status > 0 && RFID[1].status > 0 && RFID[2].status > 0 && RFID[3].status > 0){ // only if 4 dishes are presented is always wrong solution!
                    LED_CMDS::blinking(Mother,0,LED_CMDS::clrBlack,LED_CMDS::clrRed,50,100,100,100,PWM::set1_2_3_4); // let all LEDs blink red!
                    delay(800); // short delay blinking red  
                    LED_CMDS::setAllStripsToClr(Mother,0,LED_CMDS::clrBlack,100); 
                    delay(100);
                    LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrBlue,100,0); //build Brain has no PWM::set for setStripToClr
                    delay(500);
                    LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrGreen,100,1);
                    delay(500);
                    LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrRed,100,2);
                    delay(500);
                    LED_CMDS::setStripToClr(Mother,0,LED_CMDS::clrYellow,100,3); 
                    delay(500);
                    return;     
                }
                LED_CMDS::setAllStripsToClr(Mother,0,LED_CMDS::clrBlack,100); // black if one dish is removed
            break;
            case(waitfordish5):
                for(int i_RFID = 0; i_RFID < 4; i_RFID++){ //check where a Dish is laying
                    if (RFID[i_RFID].status == 3){
                        stage = runMode2;
                    return;
                    }
                }
        }                 
        wdt_enable(WDTO_8S);  
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
    delay(1000);

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
            delay(500);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrYellow, 100);
            delay(500);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100);
            delay(500);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrYellow, 100);
            delay(500);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100);
            delay(100);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlue, 100);
            delay(2000);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100);
            delay(200);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlue, 100);
            delay(2000);
            wdt_enable(WDTO_8S);
            stage = runMode1_fast;       
        break;
        case runMode1_fast: // color definition in passwort interpreter        
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100);
        break;
        case firstSolution: // After 4 dishs are placed right Light Show and Signal
            wdt_disable();
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrWhite, 100);
            delay(500);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrWhite, 100);
            //Ouput to Arbiter!!!
            wdt_enable(WDTO_8S);
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100);
            stage = waitfordish5;   
        break;
        case waitfordish5: // do nothing just check if P5 is placed
            // stage change is defined at passwort_interpreter
        break;

        case runMode2: //similar to runmod1_fast but with Layer Hint instead of Color hint        
            LED_CMDS::blinking(Mother,0,LED_CMDS::clrBlack,LED_CMDS::clrRed,50,100,100,100,PWM::set1_2_3_4); // let all LEDs blink red!
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

    Serial.println("WDT endabled");
    wdt_enable(WDTO_8S);

    Mother.rs485SetSlaveCount(1);
    setupRoom();
    setStageIndex();
    wdt_reset();
    gameReset();
}


void loop() {

    if (millis() - lastRfidCheck < rfidCheckInterval) {
        return;
    }

    Mother.rs485PerformPoll();
    interpreter();
    stageUpdate();
    wdt_reset();
}




