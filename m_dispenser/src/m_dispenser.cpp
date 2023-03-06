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
#include <stb_mother_IO.h>
#include <stb_mother_servoCmds.h>


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
int DishCount = 0; // Counter for Dishes

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


void stageActions() {
    wdt_reset();
    int DishPos;
    switch (stage) {
        case setupStage: 
            // Set Color
            LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrBlue, 50);
            delay(500);
            LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrBlue, 50, 0, 0);
            delay(500);
            LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrRed, 50, 0, 1);
            delay(500);
            LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrGreen, 50, 0, 2);
            delay(500);
            LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrWhite, 50, 0, 3);
            delay(500);
            LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrYellow, 50, 0, 4);
            delay(500);
            // Set all Servos to 0
            SERVO_CMDS::moveServo(Mother, Servo_Brain1, 0, 0);
            delay(100);
            SERVO_CMDS::moveServo(Mother, Servo_Brain1, 1, 0);
            delay(100);
            SERVO_CMDS::moveServo(Mother, Servo_Brain1, 2, 0);
            delay(100);
            SERVO_CMDS::moveServo(Mother, Servo_Brain1, 3, 0);
            delay(100);
            SERVO_CMDS::moveServo(Mother, Servo_Brain2, 0, 0);
            delay(100);
            
            stage = waitRequest;
        break;
        
        case waitRequest:break;

        case Dish1: // Start Belt normalDirection -> Empty Dish1 -> Stop Light1 -> Stop Pump1 -> Stop Belt normalDirection
            Mother.motherRelay.digitalWrite(BeltOn, open);  //Start Belt normalDirection
            DishPos = 5;
            SERVO_CMDS::moveServo(Mother, Servo_Brain2, 0, 180); // Second Servo Brain!
            LED_CMDS::setLEDToClr(Mother, LED_Brain, LED_CMDS::clrBlack, 100, 0, DishPos);
            Mother.motherRelay.digitalWrite(Pump5, closed);  //Stop Pump
            delay(3000);
            Mother.motherRelay.digitalWrite(BeltOn, closed);  //Stop Belt normalDirection   
            stage = waitRequest;
        break;

        case Dish2: // Start Belt normalDirection -> Empty Dish1 -> Stop Light1 -> Stop Pump1 -> Stop Belt normalDirection
            Mother.motherRelay.digitalWrite(BeltOn, open);  //Start Belt normalDirection
            DishPos = 3;
            SERVO_CMDS::moveServo(Mother,Servo_Brain1,DishPos, 180);
            LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrBlack, 100, 0, DishPos);
            Mother.motherRelay.digitalWrite(Pump3, closed);  //Start Belt normalDirection
            delay(3000);
            Mother.motherRelay.digitalWrite(BeltOn, closed);  //Stop Belt normalDirection   
            stage = waitRequest;
        break;

        case Dish3: // Start Belt normalDirection -> Empty Dish1 -> Stop Light1 -> Stop Pump1 -> Stop Belt normalDirection
            Mother.motherRelay.digitalWrite(BeltOn, open);  //Start Belt normalDirection
            DishPos = 4;
            SERVO_CMDS::moveServo(Mother,Servo_Brain1,DishPos, 180);
            LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrBlack, 100, 0, DishPos);
            Mother.motherRelay.digitalWrite(Pump4, closed);  //Start Belt normalDirection
            delay(3000);
            Mother.motherRelay.digitalWrite(BeltOn, closed);  //Stop Belt normalDirection   
            stage = waitRequest;
        break;

        case Dish4: // Start Belt normalDirection -> Empty Dish1 -> Stop Light1 -> Stop Pump1 -> Stop Belt normalDirection
            Mother.motherRelay.digitalWrite(BeltOn, open);  //Start Belt normalDirection
            DishPos = 1;
            SERVO_CMDS::moveServo(Mother,Servo_Brain1,DishPos, 180);
            LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrBlack, 100, 0, DishPos);
            Mother.motherRelay.digitalWrite(Pump1, closed);  //Start Belt normalDirection
            delay(2000);
            Mother.motherRelay.digitalWrite(BeltOn, closed);  //Stop Belt normalDirection   
            stage = waitRequest;
        break;

        case Dish5: // Start Belt normalDirection -> Empty Dish1 -> Stop Light1 -> Stop Pump1 -> Stop Belt normalDirection
            Mother.motherRelay.digitalWrite(Belt1, open);  //Change Belt Direction
            Mother.motherRelay.digitalWrite(Belt2, open);  //change Belt Direction
            delay(100);
            Mother.motherRelay.digitalWrite(BeltOn, open);  //Start Belt 
            DishPos = 2;
            SERVO_CMDS::moveServo(Mother,Servo_Brain1,DishPos, 180);
            LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrBlack, 100, 0, DishPos);
            Mother.motherRelay.digitalWrite(Pump2, closed);  //Start Belt normalDirection
            delay(3000);
            Mother.motherRelay.digitalWrite(BeltOn, closed);  //Stop Belt normalDirection
            delay(100);   
            Mother.motherRelay.digitalWrite(Belt1, closed);  //Change Belt Direction
            Mother.motherRelay.digitalWrite(Belt2, closed);  //change Belt Direction
            stage = waitRequest;
        break;

        case WorldsEnd: // Start Belt normalDirection -> Empty Dish1 -> Stop Light1 -> Stop Pump1 -> Stop Belt normalDirection
            Mother.motherRelay.digitalWrite(BeltOn, open);  //Start Belt 
            LED_CMDS::blinking(Mother, LED_Brain , LED_CMDS::clrBlack, LED_CMDS::clrRed, 50, 200, 100, 100,0);
            delay(300);
            SERVO_CMDS::moveServo(Mother,Servo_Brain1,0, 0);
            delay(300);
            SERVO_CMDS::moveServo(Mother,Servo_Brain1,1, 0);
            delay(300);
            SERVO_CMDS::moveServo(Mother,Servo_Brain1,2, 0);
            delay(300);
            SERVO_CMDS::moveServo(Mother,Servo_Brain1,3, 0);
            delay(300);
            SERVO_CMDS::moveServo(Mother,Servo_Brain2,0, 0);
            delay(3000);
            Mother.motherRelay.digitalWrite(BeltOn, closed);  //Stop Belt 
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

    if (stage != waitRequest) { return; }
    lastStage = waitRequest;
    int result = MotherIO.getInputs();
    if (result == DispenserAction){
        DishCount = DishCount + 1;
        switch (DishCount) {
        case 1: stage = Dish1; break;
        case 2: stage = Dish2; break;
        case 3: stage = Dish3; break;
        case 4: stage = Dish4; break;
        case 5: stage = Dish5; break;
        case 6: stage = WorldsEnd; break;
        }
    }
    

}

void setup() {

    // starts serial and default oled
    Mother.begin();
    Mother.relayInit(relayPinArray, relayInitArray, relayAmount);
    MotherIO.ioInit(intputArray, sizeof(intputArray), outputArray, sizeof(outputArray));

    Serial.println("WDT endabled");
    wdt_enable(WDTO_8S);

    Mother.rs485SetSlaveCount(3);

    setStageIndex();
    inputInit();
    wdt_reset();
}


void loop() {

    Mother.rs485PerformPoll(); // change to input from Arbiter Pi
    handleInputs();    
    stageUpdate();
    wdt_reset();
}




