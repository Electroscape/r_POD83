/**
 * @file m_analyzer.cpp
 * @author Christian Walter (pauseandplay@cybwalt.de) & Martin Pek (martin.pek@web.de)
 * @brief controls lights
 * inputs via RFID 
 * @version 0.1
 * @date 2023-02-03
 * 
 * @copyright Copyright (c) 2023
 * 
 *  TODO: 
 *  🔲 Consider cutting down on amount of stages, this might just be too annoying to trace and easier to read with less potential corners
 *  🔲 Split passwords into two arrays
 *  🔲 is waitfordish5 necessary? maybe it could be be simpler to remove it
 *  🔲 make the checksum bitshifted, much shorter 
 *  Q: 
 * 
 */


#include <stb_mother.h>
#include <stb_mother_ledCmds.h>
#include <stb_mother_IO.h>
#include <string.h>
#include "header_st.h"

// using the reset PCF for this
PCF8574 inputPCF;
STB_MOTHER Mother;
STB_MOTHER_IO MotherIO;

int stage = stages::start;
// since stages are single binary bits and we still need to d some indexing
int stageIndex = 0;
// doing this so the first time it updates the brains oled without an exta setup line
int lastStage = -1;
// Indicates the status of Light during runMode

int light_setting = 0;
int color_hint_active = 0; 
int setStrips = 0;

struct RFID_Check {
    char* str;
    int status = 0; // 0 = no Card; 1 = Card; 2 = right Card
    int status_old = 0; 
    bool ks_present = false;
    bool updated = true;    // reset by functions like evalauation and effects that run once
};

RFID_Check RFID[4];
unsigned long lastRfidCheck = millis();
unsigned long nextLightTimer = millis(); // timer for giving non blocking LightHint


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


void passwordInterpreter() {

    for (int i_RFID = 0; i_RFID < 4; i_RFID++) {
        char* password = RFID[i_RFID].str;
        RFID[i_RFID].status = 0;       
        RFID->ks_present = false;

        for (int passNo=0; passNo < sampleCount; passNo++) {
            if ( ( strlen(passwords[passNo]) == strlen(password) ) && strncmp(passwords[passNo], password, strlen(passwords[passNo]) ) == 0) {       
                if (passNo == i_RFID) { 
                    RFID[i_RFID].status = 2; // Dish presented and at right position
                } else {
                    RFID[i_RFID].status = 1; // Dish presented but at wrong position
                }
            } else if ( (strlen(passwords[5]) == strlen(password) ) && strncmp(passwords[5], password, strlen(passwords[5]) ) == 0)  {
                RFID->ks_present = true;
                if (passNo == 0) {
                    RFID[i_RFID].status = 2; // Dish presented and at right position
                } else {
                    RFID[i_RFID].status = 1; // Dish presented and at right position
                }
            }

            if (RFID[i_RFID].status != RFID[i_RFID].status_old) {
                RFID->updated = true;
            }
        }   
    }
}


/**
 * @brief Gives the Color Hint, runs indefinetely unless cancelled
 * @param  
*/
void color_hint() {
    unsigned long actTime = millis();

    if ((nextLightTimer > actTime ) || (color_hint_active == 0)) { return; } // only new light setting if timer is reached

    switch (light_setting) {
        case (red_blinking):
            LED_CMDS::blinking(Mother, 0, LED_CMDS::clrBlack, LED_CMDS::clrRed, 50, 100, 100, 100, PWM::set1_2_3_4); 
            nextLightTimer += 1400;
            light_setting += 1;
        break;

        case (turn_off):
            LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100); 
            nextLightTimer += 300;
            light_setting += 1;
        break;
        
        case (hint_place1):
            LED_CMDS::setStripToClr(Mother, 0, LED_CMDS::clrRed, 100, PWM::set1);  
            nextLightTimer += 600;
            light_setting += 1;
        break;
        
        case (hint_place2):
            LED_CMDS::setStripToClr(Mother, 0, LED_CMDS::clrBlue, 100, PWM::set2);  
            nextLightTimer += 600;
            light_setting += 1;
        break;
        
        case (hint_place3):
            LED_CMDS::setStripToClr(Mother, 0, LED_CMDS::clrYellow, 100, PWM::set3);  
            nextLightTimer += 600;
            light_setting += 1;
        break;
        
        case (hint_place4):
            LED_CMDS::setStripToClr(Mother, 0, LED_CMDS::clrGreen, 100, PWM::set4);  
            nextLightTimer += 1600;
            light_setting = red_blinking;
        break;
    }
        
}


/**
 * @brief  
 * @todo consider making this non blocking
*/
void analyzeFx() {
    if (!RFID->updated) {
        return;
    }
    wdt_reset();
    LED_CMDS::blinking(Mother, 0, LED_CMDS::clrBlack, LED_CMDS::clrRed, 50, 100, 100, 100, PWM::set1_2_3_4); 
    delay(300);
    LED_CMDS::blinking(Mother, 0, LED_CMDS::clrBlack, LED_CMDS::clrBlue, 50, 100, 100, 100, PWM::set1_2_3_4); 
    delay(300);
    LED_CMDS::blinking(Mother, 0, LED_CMDS::clrBlack, LED_CMDS::clrPurple, 50, 100, 100, 100, PWM::set1_2_3_4); 
    delay(300);
    LED_CMDS::blinking(Mother, 0, LED_CMDS::clrBlack, LED_CMDS::clrYellow, 50, 100, 100, 100, PWM::set1_2_3_4); 
    delay(300);
    LED_CMDS::blinking(Mother, 0, LED_CMDS::clrBlack, LED_CMDS::clrGreen, 50, 100, 100, 100, PWM::set1_2_3_4); 
    delay(300);
    LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100);
    wdt_reset();
}


/**
 * @brief handles evalauation of codes and sends the result to the access module
 * @param cmdPtr 
*/
void handleResult(char *cmdPtr) {

    cmdPtr = strtok(NULL, KeywordsList::delimiter.c_str());
    RFID[0].str = cmdPtr;
    cmdPtr = strtok(NULL, KeywordsList::delimiter.c_str());
    RFID[1].str = cmdPtr;
    cmdPtr = strtok(NULL, KeywordsList::delimiter.c_str());
    RFID[2].str = cmdPtr;
    cmdPtr = strtok(NULL, KeywordsList::delimiter.c_str());
    RFID[3].str = cmdPtr;
    

    if (cmdPtr == NULL) { return; }

    passwordInterpreter();

    int checkSum = 0;

    checkSum = RFID[0].status + RFID[1].status + RFID[2].status + RFID[3].status;

    bool allSlotsPresent = true;

    for (int i_RFID = 0; i_RFID < 4; i_RFID++) {   
        if (RFID[i_RFID].status == 0) {
            allSlotsPresent = false;
            // Serial.println(RFID[i_RFID].status);
            break;
        }
    }    

    if (stage > firstSolution && !RFID->ks_present) { return; }

    if (!allSlotsPresent) {
        color_hint_active = 0;

        for (int i_RFID = 0; i_RFID < 4; i_RFID++) {          
            if (RFID[i_RFID].status != RFID[i_RFID].status_old) { return; }

            RFID[i_RFID].status_old = RFID[i_RFID].status; 
            
            if (RFID[i_RFID].status > 0) {  
                LED_CMDS::setStripToClr(Mother, 0, LED_CMDS::clrWhite, 100, 1 << i_RFID);   // use Bitshift to turn on LED 
            } else { 
                LED_CMDS::setStripToClr(Mother, 0, LED_CMDS::clrBlack, 100, 1 << i_RFID);   // use Bitshift to turn off LED
            }  
        }
        return;     // leaving the function since there is no need to evaluate further
    }

    analyzeFx();
    wdt_disable();        
    
    // not every piece correctly placed
    if (checkSum < 8) { 
        LED_CMDS::blinking(Mother, 0, LED_CMDS::clrBlack, LED_CMDS::clrRed, 50, 100, 100, 100, PWM::set1_2_3_4); 
        delay(500); 
    } else {
        stage = stage << 1;
    }


    /*
    switch (stage) {

        case runMode1: // Stage runMode 1 if a Dish is placed LEDs should get white, blink red and stay at this position on white until all 4 dishes are placed
            for (int i_RFID = 0; i_RFID < 4; i_RFID++) { // Check all RFIDs if a new Dish is presented set  to white
                if (RFID[i_RFID].status == RFID[i_RFID].status_old) { continue; }//only if status of RFID has changed, no return because all RFID neede to be checked!
                
                if (RFID[i_RFID].status > 0) { //blinking if dish is new 
                    LED_CMDS::blinking(Mother, 0, LED_CMDS::clrBlack, LED_CMDS::clrRed, 50, 100, 100, 100, PWM::set1_2_3_4); 
                    delay(1300);   
                }                        
            }
        break;

        case runMode2: // without the light up and analyze, it should blink red for short if it is the wrong combination and gives color hint
            if (checkSum == 8) { // Right Solution found -> change state
                color_hint_active = 0;
                stage = firstSolution;
                return;
            }
            if (RFID[0].status > 0 && RFID[1].status > 0 && RFID[2].status > 0 && RFID[3].status > 0) { // only if 4 dishes are presented is always wrong solution, because if CheckSum Condition before
                    if (color_hint_active == 0) {
                        color_hint_active = 1;
                        light_setting = 0; // always start with red blinking
                        nextLightTimer = millis();
                    }
                return;     
            } else {
                LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100); // black if one dish is removed
                color_hint_active = 0; // and stopping colorhint
            }
        break;

        // waitfordish5
        
        for (int i_RFID = 0; i_RFID < 4; i_RFID++) { //check all RFIDs for Dish 5
            if (RFID[i_RFID].status > 0) { // if anny RFID has Dish5 change the stage
                stage = runMode3;
                Serial.println("Dish5 found");                      
                LED_CMDS::blinking(Mother, 0, LED_CMDS::clrBlack, LED_CMDS::clrWhite, 125, 125, 100, 100, PWM::set1_2_3_4); 
                delay(1000);
                return;
            }
        }
        


        case runMode3: // without the light up and analyze, it should blink red for short if it is the wrong combination and gives color hint, at the moment technical the same as runmode2
            if (checkSum == 8) { // Right Solution found -> change state
                color_hint_active = 0;
                stage = secondSolution;
                return;
            }
            if (RFID[0].status > 0 && RFID[1].status > 0 && RFID[2].status > 0 && RFID[3].status > 0) { // only if 4 dishes are presented is always wrong solution, because if CheckSum Condition before
                    if (color_hint_active == 0) { // start color_hint
                        color_hint_active = 1;
                        light_setting = 0; // always start with red blinking
                        nextLightTimer = millis();
                    }
                return;     
            } else {
                LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100); // black if one dish is removed
                color_hint_active = 0;
            }
        break;                
    }                 
      
    */
    wdt_enable(WDTO_8S);
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
    while (Mother.nextRcvdLn()) {
        checkForRfid();
    }
}

/**
 * @brief  Stage Setup and Color Test for all 4 LED Strips
*/
void setupRoom() {
    wdt_disable();   
    Serial.println("ColorTest");
    LED_CMDS::setStripToClr(Mother, 0, LED_CMDS::clrBlue, 100, set1);
    delay(100);
    LED_CMDS::setStripToClr(Mother, 0, LED_CMDS::clrGreen, 100, set2);
    delay(100);
    LED_CMDS::setStripToClr(Mother, 0, LED_CMDS::clrRed, 100, set3);
    delay(100);
    LED_CMDS::setStripToClr(Mother, 0, LED_CMDS::clrYellow, 100, set4);
    delay(500);
    LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100); // turn off all LEDs
   wdt_enable(WDTO_8S);
}


/**
 * @brief  Light actions after stage has changed
*/
void stageActions() {

    wdt_disable(); 
    wdt_reset();

    if (stage & (firstSolution + secondSolution)) {
        LED_CMDS::blinking(Mother, 0, LED_CMDS::clrBlack, LED_CMDS::clrGreen, 50, 100, 100, 100, PWM::set1_2_3_4); 
        delay(200);
    }

    switch (stage) {
        case firstSolution:
            MotherIO.setOuput(firstSolutionEvent); // Release the Killswitch (Dish5)
        break;

        case secondSolution: // After the first 4 dishes are placed at the right position, sends second Signal to arbiter
            MotherIO.setOuput(secondSolutionEvent); // Enables the upload 
            stage = firstSolution;
        break;
    }

    delay(100);
    MotherIO.outputReset();
    delay(6000);
    LED_CMDS::setAllStripsToClr(Mother, 0, LED_CMDS::clrBlack, 100); // torun off until dish 5

    wdt_enable(WDTO_8S);
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

    if (stageIndex < 0) {
        Serial.println(F("Stages out of index!"));
        delay(5000);
        wdt_reset();
    }
    // important to do this before stageActions! otherwise we skip stages
    lastStage = stage;
    stageActions();
}


void setup() {
    
    // starts serial and default oled
    Mother.begin();
    MotherIO.ioInit(intputArray, sizeof(intputArray), outputArray, sizeof(outputArray));
    Serial.println("WDT endabled");
    wdt_enable(WDTO_8S);

    Mother.rs485SetSlaveCount(1);
    setupRoom();
    setStageIndex();
    wdt_reset();
}


void loop() {
    color_hint();
    /*
    the check interval already exists on the brain, so i don't see a reason not to poll
    if (millis() - lastRfidCheck < rfidCheckInterval) {
        return;
    } 
    */
    lastRfidCheck = millis();
    Mother.rs485PerformPoll();
    interpreter();
    stageUpdate();
    wdt_reset();
    delay(50);
}




