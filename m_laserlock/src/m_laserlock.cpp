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
 *  6. Wrong code entered on access module -> "Access denied" currently not implemented
 * in between stage or text for unlocking with rfid
 *  Q:
 * 🔲 booting text on startup
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
int stage = setupStage;
// since stages are single binary bits and we still need to d some indexing
int stageIndex = 0;
// doing this so the first time it updates the brains oled without an exta setup line
int lastStage = -1;
int inputTicks = 0;

// general timestamp going to use this to timeout the card repsentation in unlocked and RFIDoutput
unsigned long timestamp = millis();
// timedTrigger shall reset this value 
// cardsPreset adding bits of present cards with | 
int cardsPresent = 0;
int brainsPresent = 0;
bool lightOn = false;

int validBrainResult = 0;


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
    // stage = setupStage;
    for (int relayNo=0; relayNo < relayAmount; relayNo++) {
        Mother.motherRelay.digitalWrite(relayNo, relayInitArray[relayNo]);
    }
    lightOn = false;
}


/**
 * @brief  
 * @param result 
 * @param brainNo can be set to > labAccess so it sends to both brains
*/
void sendResult(bool result, int brainNo=Mother.getPolledSlave()) {
    // prepare return msg with correct or incorrect
    char msg[10] = "";
    char noString[3] = "";
    strcpy(msg, keypadCmd.c_str());
    strcat(msg, KeywordsList::delimiter.c_str());

    // can i make an integer oneliner?
    if (result) {
        sprintf(noString, "%d", KeypadCmds::correct);
    } else {
        sprintf(noString, "%d", KeypadCmds::wrong);
    }
    strcat(msg, noString);

    Mother.sendCmdToSlave(msg, brainNo);
}


void resetDualRfid() {
    cardsPresent = brainsPresent = 0;
}


/**
 * @brief handles timeouts rfidTX and timeout of the RFID presentation
*/
void timedTrigger() {
    if (timestamp > millis() || lastStage != stage) { return; }
    switch (stage) {
        case unlock: 
            stage = failedUnlock;
        break;
        // alternatively use cardspresent to send evaluation
        default: 
            if ((stage & (seperationLocked + seperationUnlocked)) > 0) {
                if (cardsPresent == 0) {return;}
                resetDualRfid();
                sendResult(false, 0);
                sendResult(false, 1);
            }
        break;
    }
}


/**
 * @brief  handles RFId scanning required on both sides
 * @param passNo 
*/
void checkDualRfid(int passNo) {
     
    timestamp = millis() + rfidTimeout;
    if (stage == seperationUnlocked && inputPCF.digitalRead(reedDoor)) {
        resetDualRfid();
        Serial.println("door is not closed");
        return;
    }
    cardsPresent |= passNo + 1;
    brainsPresent |= Mother.getPolledSlave() + 1;

    // not presented on both cards, hence we exit here
    if (cardsPresent <= PasswordAmount) { return; }
    // not both rfid readers used so its invalid
    if (brainsPresent <= int(labAccess) + 1) {
        Serial.println("not enoough brains present");
        return;
    }

    cardsPresent = brainsPresent = 0;

    switch (stage) {
        case seperationUnlocked: 
            Mother.motherRelay.digitalWrite(door, doorClosed); 
            delay(100);
            if (Mother.getPolledSlave() == 0) {
                switch (passNo) {
                    // either this case?
                    case 0: MotherIO.setOuput(davidSeperated, true); break;
                    case 1: MotherIO.setOuput(rachelSeperated, true); break;
                }
            } else {
                switch (passNo) {
                    case 0: MotherIO.setOuput(rachelSeperated, true); break;
                    case 1: MotherIO.setOuput(davidSeperated, true); break;
                }
            }
            stage = seperationLocked; 
        break;
        case seperationLocked:
            Mother.motherRelay.digitalWrite(door, doorOpen); // first thing we do since we dont wanna
            delay(100);
            MotherIO.setOuput(seperationEnd, true);
            stage = seperationUnlocked;
        break;
    }

    sendResult(true, 0);
    sendResult(true, 1);

    wdt_reset();
    delay(500);
    MotherIO.outputReset();
    delay(4500);
    wdt_reset();
}


/**
 * @brief handles room specific actions
 * @param passNo 
*/
void handleCorrectPassword(int passNo) {

    switch (stage) {
        case unlock: 
            // delay(500);
            stage = unlocked; 
            if (passNo == 0) {
                MotherIO.setOuput(david, true);
            } else {
                MotherIO.setOuput(rachel, true);
            }
            // start polling the 2nd Access since there is no need before
            Mother.rs485SetSlaveCount(2);
        break;
        // be careful here if we change stages
        default:
            checkDualRfid(passNo);
        break;
    }
}


bool passwordInterpreter(char* password) {
    // Mother.STB_.defaultOled.clear();
    Serial.print("Handling pw");
    Serial.println(password);
    for (int passNo=0; passNo < PasswordAmount; passNo++) {
        if (passwordMap[passNo] & stage) {
            // strlen(passwords[passNo]) == strlen(password) &&
            if ( strncmp(passwords[passNo], password, strlen(passwords[passNo]) ) == 0) {
                handleCorrectPassword(passNo);
                Serial.println("Correct PW");
                return true;
            }
        }
    }
    return false;
}


/**
 * @brief handles evalauation of codes and sends the result to the access module
 * @param cmdPtr 
 * @todo handle the unlock/locking here with 2 access modules
*/
void handleResult() {
    Mother.STB_.rcvdPtr = strtok(Mother.STB_.rcvdPtr, KeywordsList::delimiter.c_str());
    if ((Mother.STB_.rcvdPtr != NULL) && passwordInterpreter(Mother.STB_.rcvdPtr)) {
        // excluding the cases where both cards need to be present
        // here may be the ussie... keep the sendresult stuff in one place
        if ((stage & (seperationUnlocked + seperationLocked)) == 0) {
            sendResult(true);
        }
    } else {
        sendResult(false);
    }
}


// again good candidate for a mother specific lib
bool checkForRfid() {
    if (strncmp(KeywordsList::rfidKeyword.c_str(), Mother.STB_.rcvdPtr, KeywordsList::rfidKeyword.length() ) != 0) {
        return false;
    } 
    Mother.STB_.rcvdPtr += KeywordsList::rfidKeyword.length();
    handleResult();
    wdt_reset();
    return true;
}


void interpreter() {
    while (Mother.nextRcvdLn()) {
        checkForRfid();
    }
}


void oledUpdate(int brainIndex = 0) {
    char msg[32] = "";
    strcpy(msg, oledHeaderCmd.c_str());
    strcat(msg, KeywordsList::delimiter.c_str());
    strcat(msg, stageTexts[stageIndex]); 
    Mother.sendCmdToSlave(msg, brainIndex);
}


void oledFailed() {
    timestamp = millis() + displayFailedUnlock;
    char timeoutMsg[32] = "";
    strcpy(timeoutMsg, oledHeaderCmd.c_str());
    strcat(timeoutMsg, KeywordsList::delimiter.c_str());
    strcat(timeoutMsg, stageTexts[stageIndex]);
    char cleanMsg[32] = "";
    strcpy(cleanMsg, oledHeaderCmd.c_str());
    strcat(cleanMsg, KeywordsList::delimiter.c_str());
    strcat(cleanMsg,  "Clean Airlock"); 
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
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrBlack, 100, 0);
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 100, 0);
            stage = idle;
        break;
        case failedBoot:
            wdt_disable();
            LED_CMDS::fade2color(Mother, ledLaserBrain, LED_CMDS::clrBlack, 100, LED_CMDS::clrRed, 50, 10000, PWM::set1);
            delay(10000);

            // total duration 5s
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrBlack, 100);
            delay(25);
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrRed, 50);
            delay(50);
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrBlack, 100);
            delay(25);
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrRed, 40);
            delay(60);
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrBlack, 100);
            delay(25);
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrRed, 35);
            delay(25);
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrBlack, 100);
            delay(25);
            LED_CMDS::fade2color(Mother, ledLaserBrain, LED_CMDS::clrRed, 30, LED_CMDS::clrRed, 0, 4775, PWM::set1);
            delay(4775);
            enableWdt();
            stage = idle;
        break;
        case successfulBoot:
            LED_CMDS::fade2color(Mother, ledLaserBrain, LED_CMDS::clrRed, 0, LED_CMDS::clrRed, 80, 10000, PWM::set1);
            wdt_disable();
            delay(10000);
            
            enableWdt();
            stage = decon;
        break;
        case decon:
            // @todo: check timing here
            int effektTime;
            for (int brightness = 10; brightness <= 100; brightness += 10) {
                effektTime = (100 - brightness) * 20;
                if (effektTime < 20) { effektTime = 50; }
                //LED_CMDS::running(Mother, ledLaserBrain, LED_CMDS::clrBlue, brightness, runTime, ledCnt, PWM::set1, 1000);
                LED_CMDS::running(Mother, ledLaserBrain, LED_CMDS::clrBlue, brightness, effektTime, ledCnt, PWM::set1, 0);
                delay(900); // loop should be a total of 8100ms -> 9 loops
                wdt_reset();
            }
            Mother.motherRelay.digitalWrite(outerDoor, open);
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrBlack, 100);
            delay(100);      // 8130
            // LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrWhite, 100, 0);
            // doesnt specify fade but may aswell see how it works
            LED_CMDS::fade2color(Mother, ledLaserBrain, LED_CMDS::clrWhite, 100, LED_CMDS::clrBlue, 50, 6830, PWM::set1);
            timestamp = millis() + deconRFIDTimeout;
            stage = unlock;
        break;
        case unlock:
        break;
        case failedUnlock:
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrBlack, 100);
            delay(50);
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrRed, 100);
            delay(200);
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrBlack, 100);
            delay(200);
            LED_CMDS::fade2color(Mother, ledLaserBrain, LED_CMDS::clrRed, 100, LED_CMDS::clrRed, 80, displayFailedUnlock,  PWM::set1);
            oledFailed();
            stage = idle;
        break;
        case unlocked:
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrGreen, 50);
            Mother.motherRelay.digitalWrite(door, doorOpen);
            wdt_disable();
            delay(rfidTxDuration);
            enableWdt();
            MotherIO.outputReset();
            wdt_reset();
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrWhite, 20);
            stage = seperationUnlocked;
        break; 
        case seperationUnlocked:
            resetDualRfid();
            Mother.rs485SetSlaveCount(2);
            Mother.motherRelay.digitalWrite(door, doorOpen);
            // wdt_disable();
            // enableWdt();
            // delay(rfidTxDuration);
            // MotherIO.outputReset();
        break;
        case seperationLocked:
            resetDualRfid();
            Mother.motherRelay.digitalWrite(door, doorClosed);
            // MotherIO.setOuput(seperationEnd);
        break;
        case lightStart:
            LED_CMDS::fade2color(Mother, ledCeilBrain, clrLight, 0, clrLight, 20, lightStartDuration,  PWM::set1 + PWM::set2);
            stage = idle;
            delay(8000);
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

    // for now no need to make it work properly scaling, need to build afnc repertoir anyways
    for (int brainNo=0; brainNo < Mother.getSlaveCnt(); brainNo++) {
        Mother.setFlags(brainNo, flagMapping[stageIndex]);
        oledUpdate(brainNo);
        delay(5);
    }
    MotherIO.outputReset();
    stageActions();
}


void inputInit() {
    for (int pin=0; pin<inputCnt; pin++) {
        inputPCF.begin(RESET_I2C_ADD);
        inputPCF.pinMode((uint8_t) pin, INPUT);
        inputPCF.digitalWrite((uint8_t) pin, HIGH);
    }
}


void handleInputs() {
    
    if ( (stage & (idle | seperationUnlocked | seperationLocked)) == 0) { return; }

    int result = MotherIO.getInputs();
    result -= result & (1 << reedDoor);
    if (result == 0) { return; }
    // Serial.println("result");
    // Serial.println(result);
    // delay(5000);
    wdt_reset();
    unsigned long startTime = millis();
    switch (result) {
        case skipToSeperation:
            stage = seperationUnlocked;
        break;

        case failedBootTrigger: 
            stage = failedBoot;
        break;

        case bootupTrigger: 
            stage = successfulBoot;
        break;

        case roomBoot:
            if (lightOn) { return; }
            lightOn = true;
            stage = lightStart;
        break;

        case elancellEnd:
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrGreen, 60);
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrGreen, 60);
            delay(3000);
        break;

        case cleanupLight:
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrBlack, 100);
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrWhite, 75);
            delay(3000);
        break;

        case lightOff: 
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrBlack, 100);
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 100);
        break;

        case rachelAnnouncement:
            wdt_disable();
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrRed, 60);
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrRed, 60);   
            wdt_reset();
            while ((millis() - startTime) < (unsigned long) 42500) {}
            LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrBlack, 60);
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 60);
            wdt_enable(WDTO_8S);
        break;
        
        case rachelEnd:
            wdt_disable();
            #ifdef Hamburg
                LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrBlack, 60);
                LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrRed, 60);
                /* delay(41000); // Video Rachel
                delay(34000); // Video Proceed to airlock start at second "remain calm" */
                LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 50);
                delay(200);
                
                LED_CMDS::blinking(Mother, ledCeilBrain,LED_CMDS::clrBlack,LED_CMDS::clrYellow,950,50,100,100,PWM::set1);
                
                delay(10000); // Delay Countdown

                LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 50);
                delay(1000);
                for (int i=0; i<4; i++) {
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrRed, 100);
                    delay(200);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 100);
                    delay(300);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrYellow, 100);
                    delay(100);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlue, 100);
                    delay(100);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 100);
                    delay(300);
                }
                delay(500);
                LED_CMDS::blinking(Mother, ledCeilBrain,LED_CMDS::clrBlack,LED_CMDS::clrRed,10,10,100,10,PWM::set1);
                delay(1000);
                for (int i=0; i<3; i++) {
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrRed, 60);
                    delay(400);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 100);
                    delay(300);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrYellow, 60);
                    delay(200);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlue, 100);
                    delay(200);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 100);
                    delay(300);
                }
                delay(500);
                LED_CMDS::blinking(Mother, ledCeilBrain,LED_CMDS::clrBlack,LED_CMDS::clrRed,10,10,100,10,PWM::set1);
                delay(1000);
                for (int i=0; i<3; i++) {
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrRed, 30);
                    delay(650);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 100);
                    delay(300);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrYellow, 30);
                    delay(325);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlue, 100);
                    delay(325);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 100);
                    delay(300);
                }
                delay(500);
                LED_CMDS::blinking(Mother, ledCeilBrain,LED_CMDS::clrBlack,LED_CMDS::clrRed,10,10,100,10,PWM::set1);
                delay(1000);
                for (int i=0; i<3; i++) {
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrRed, 30);
                    delay(900);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 100);
                    delay(300);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrYellow, 30);
                    delay(450);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlue, 100);
                    delay(450);
                    LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 100);
                    delay(300);
                }
                LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 1000);
                delay(7000);
                LED_CMDS::fade2color(Mother, ledLaserBrain,LED_CMDS::clrBlack,100,LED_CMDS::clrWhite,100,12000,PWM::set1);
                LED_CMDS::fade2color(Mother, ledCeilBrain,LED_CMDS::clrBlack,100,LED_CMDS::clrWhite,100,12000,PWM::set1);
                delay(12000);
                LED_CMDS::setStripToClr(Mother, ledLaserBrain, LED_CMDS::clrWhite, 100,0); // set strip to clr not yet with PWMset 24.03.23
                LED_CMDS::setStripToClr(Mother, ledCeilBrain, LED_CMDS::clrWhite, 100,0); // set strip to clr not yet with PWMset 24.03.23
            #else
                LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrBlack, 60);
                LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 60);
                delay(200);
                // LED_CMDS::blinking(Mother, ledLaserBrain,LED_CMDS::clrBlack,LED_CMDS::clrYellow,950,50,100,10,PWM::set1);
                while ((millis() - startTime) < (unsigned long) 10000) {}
                LED_CMDS::setAllStripsToClr(Mother, ledLaserBrain, LED_CMDS::clrBlack, 60);
                while ((millis() - startTime) < (unsigned long) 60000) {}
                LED_CMDS::fade2color(Mother, ledCeilBrain,LED_CMDS::clrBlack,100,LED_CMDS::clrWhite,30,15000,PWM::set1);
            #endif
            wdt_enable(WDTO_8S);
        break;

        default: break;
    }
}


void setup() {

    Mother.begin();
    Mother.relayInit(relayPinArray, relayInitArray, relayAmount);
    MotherIO.ioInit(intputArray, sizeof(intputArray), outputArray, sizeof(outputArray));

    Serial.println("WDT endabled");
    enableWdt();

    // technically 3 but for the first segments till unlocked there is no need
    Mother.rs485SetSlaveCount(1);

    setStageIndex();
    inputInit();

    wdt_reset();
    delay(1000);
    gameReset();
}


void loop() {
    validBrainResult = Mother.rs485PerformPoll();
    timedTrigger();
    if (stage == seperationUnlocked && inputPCF.digitalRead(reedDoor)) {
        resetDualRfid();
    }
    if (validBrainResult) {interpreter();}
    handleInputs();    
    stageUpdate(); 
    delay(5);
    wdt_reset();
}




