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
 * 
 *  Q: 
 * 
 */


#include <stb_mother.h>
#include <stb_mother_ledCmds.h>
#include <stb_mother_IO.h>
#include <stb_mother_servoCmds.h>


#include "header_st.h"

STB_MOTHER Mother;
STB_MOTHER_IO MotherIO;

int stage = setupStage;
// since stages are single binary bits and we still need to d some indexing
int stageIndex = 0;
// doing this so the first time it updates the brains oled without an exta setup line
int lastStage = -1;
int lastInput = -1;
int DishCount = 0; // Counter for Dishes

unsigned long beltEndTime = millis();
int beltActive = 0;     // also saves the dishNo to avoid having multiple dishes on the belt colliding


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

void func_move_servo(int Brain, int PWM_No) {
    // on startup the servo is on position 0 since the excecution ends there
    SERVO_CMDS::moveServo(Mother, Brain, PWM_No, 180);
    delay(600);
    SERVO_CMDS::moveServo(Mother, Brain, PWM_No, 0);
    delay(600);
    Serial.println("end moveservo");
}

/**
 * @brief  starts the belt and sets variable for timing it, ignored for hamburg flag
 * 
*/
void beginBelt(int beltNo) {
    #ifndef Hamburg
        Mother.motherRelay.digitalWrite(BeltOn, open);  //Start Belt normalDirection
        beltEndTime = millis() + beltOperationTime;
        beltActive = beltNo;
    #endif
}

void stopBelt() {
    #ifndef Hamburg
        Mother.motherRelay.digitalWrite(BeltOn, closed);  //Start Belt 
        delay(50);   
        Mother.motherRelay.digitalWrite(beltReverse_1, closed);  // Change Belt Direction
        Mother.motherRelay.digitalWrite(beltReverse_2, closed);  // change Belt Direction

        Mother.motherRelay.digitalWrite(pump1, PumpOn);
        Mother.motherRelay.digitalWrite(pump2, PumpOn);
        Mother.motherRelay.digitalWrite(pump3, PumpOn);
        Mother.motherRelay.digitalWrite(pump4, PumpOn);
        Mother.motherRelay.digitalWrite(pump5, PumpOn);

        beltActive = 0;
    #endif
}


void stageActions() {
    wdt_reset();

    // only allow to run the same dish again before the belt completes to avoid physical issues
    if (beltActive && beltActive != stage && stage & dishStageSum) {
        stage = waitRequest;
        Serial.println(beltActive);
        Serial.println(stage);
        return;
    }


    switch (stage) {
        case setupStage: 
            DishCount = 4;
            #ifndef Hamburg
                #ifndef IgnoreLeds
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrWhite, 20);
                #endif
            #endif 
        break;
        
        case waitRequest: break;

        case Dish1: // Start Belt normalDirection -> Empty Dish -> Start Light -> Start pump -> Stop Belt normalDirection
            
            #ifndef Hamburg
                beginBelt(Dish1);
                delay(500);
                Mother.motherRelay.digitalWrite(pump5, PumpOff);
                delay(500);
                #ifndef IgnoreLeds
                LED_CMDS::setLEDToClr(Mother, LED_Brain, LED_CMDS::clrRed, 100, PWM::set1, 4);
                #endif
                delay(800);
            #endif
            #ifdef Hamburg
                LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrRed, 100, PWM::set1);
                LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrBlack, 100, PWM::set2);
                delay(500);
            #endif
            func_move_servo(Servo_Brain2,0);   
        break;

        case Dish2: // Start Belt normalDirection -> Empty Dish -> Start Light -> Start pump -> Stop Belt normalDirection
            #ifndef Hamburg // No Belt no Light no pump in Hamburg
                beginBelt(Dish2);
                delay(500);
                Mother.motherRelay.digitalWrite(pump4, PumpOff);  //Start Belt normalDirection
                delay(500);
                #ifndef IgnoreLeds
                LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrYellow, 100, PWM::set1, 3);
                #endif
                delay(800); 
            #endif
            #ifdef Hamburg
                LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrYellow, 100, PWM::set1);
                LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrBlack, 100, PWM::set2);
                delay(500);
            #endif
            func_move_servo(Servo_Brain1, 3);
        break;

        case Dish3:// Start Belt normalDirection -> Empty Dish -> Start Light -> Start pump -> Stop Belt normalDirection           
            #ifndef Hamburg // No Belt no Light no pump in Hamburg
                beginBelt(Dish3);
                delay(500);
                Mother.motherRelay.digitalWrite(pump3, PumpOff); 
                delay(500);
                #ifndef IgnoreLeds
                LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrGreen, 100, PWM::set1, 2);
                #endif
                delay(800);
            #endif
            #ifdef Hamburg
                LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrGreen, 100, PWM::set2);
                delay(500);
            #endif
            func_move_servo(Servo_Brain1, 2);
        break;

        case Dish4: // Start Belt normalDirection -> Empty Dish -> Start Light -> Start pump -> Stop Belt normalDirection
            #ifndef Hamburg // No Belt no Light no pump in Hamburg
                beginBelt(Dish4);
                delay(500);
                Mother.motherRelay.digitalWrite(pump2, PumpOff);
                delay(500);  
                #ifndef IgnoreLeds          
                LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrBlue, 100, PWM::set1, 1);
                #endif
                delay(800);
            #endif
            #ifdef Hamburg
                LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrBlue, 100, PWM::set2);
                delay(500);
            #endif
            func_move_servo(Servo_Brain1, 1);
        break;

        case Dish5: // Start Belt normalDirection -> Empty Dish -> Start Light -> Start pump -> Stop Belt normalDirection
            wdt_disable();
            #ifndef Hamburg // No Belt no Light no pump in Hamburg
                Mother.motherRelay.digitalWrite(beltReverse_1, open);
                Mother.motherRelay.digitalWrite(beltReverse_2, open);
                delay(100);
                beginBelt(Dish5);
                delay(500);
                Mother.motherRelay.digitalWrite(pump1, PumpOff); 
                delay(500);
                #ifndef IgnoreLeds
                LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrWhite, 100, PWM::set1, 0);
                #endif
                delay(800);
            #endif

            #ifdef Hamburg
                for (int i=1; i <= 5; i++) {
                    
                    if (i % 2 == 0) {
                        Mother.motherRelay.digitalWrite(pump1, PumpOff);
                        Mother.motherRelay.digitalWrite(pump2, PumpOff);
                    } else {
                        Mother.motherRelay.digitalWrite(pump1, PumpOn);
                        Mother.motherRelay.digitalWrite(pump2, PumpOn);
                    }
                  
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrRed, 100, PWM::set1);
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrRed, 100, PWM::set2);
                    delay(20);
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrBlack, 100, PWM::set1);
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrBlack, 100, PWM::set2);
                    delay(500/i);
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrYellow, 100, PWM::set1);
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrYellow, 100, PWM::set2);
                    delay(20);
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrBlack, 100, PWM::set1);
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrBlack, 100, PWM::set2);
                    delay(500/i);
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrGreen, 100, PWM::set1);
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrGreen, 100, PWM::set2);
                    delay(20);
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrBlack, 100, PWM::set1);
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrBlack, 100, PWM::set2);
                    delay(500/i);
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrBlue, 100, PWM::set1);
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrBlue, 100, PWM::set2);
                    delay(20);
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrBlack, 100, PWM::set1);
                    LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrBlack, 100, PWM::set2);
                    delay(500/i);
                }

                LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrWhite, 100, PWM::set1);
                LED_CMDS::setStripToClr(Mother, LED_Brain, LED_CMDS::clrWhite, 100, PWM::set2);

                func_move_servo(Servo_Brain1, 0);
                Mother.motherRelay.digitalWrite(pump1, PumpOn);
                Mother.motherRelay.digitalWrite(pump2, PumpOn);
                delay(500);
                Mother.motherRelay.digitalWrite(pump1, PumpOff);
                Mother.motherRelay.digitalWrite(pump2, PumpOff);
                delay(500);
                Mother.motherRelay.digitalWrite(pump1, PumpOn);
                Mother.motherRelay.digitalWrite(pump2, PumpOn);
                delay(500);
                Mother.motherRelay.digitalWrite(pump1, PumpOff);
                Mother.motherRelay.digitalWrite(pump2, PumpOff);
                Mother.motherRelay.digitalWrite(pump1, PumpOn);
                Mother.motherRelay.digitalWrite(pump2, PumpOn);
                delay(500);
                delay(8000);
            #endif
            func_move_servo(Servo_Brain1, 0);
        break;

        case WorldsEnd: 
            stopBelt();
            #ifndef Hamburg // No Belt no Light no pump in Hamburg
                Mother.motherRelay.digitalWrite(beltReverse_1, open);
                Mother.motherRelay.digitalWrite(beltReverse_2, open);
                delay(100);
                Mother.motherRelay.digitalWrite(BeltOn, open);  //Start Belt 

                #ifndef IgnoreLeds
                // for loops .... do you speak it?
                for (int i=0; i<6; i++) {
                    LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrRed, 100); 
                    delay(300);
                    LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrBlack, 100); 
                    delay(100);
                    wdt_reset();
                }
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrRed, 100); 
                #endif
                wdt_disable();
                delay(15000);
                wdt_enable(WDTO_8S);
                Mother.motherRelay.digitalWrite(BeltOn, closed);  //Stop Belt   
                #ifndef IgnoreLeds
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrBlack); 
                #endif
                Mother.motherRelay.digitalWrite(pump1, open); 
                Mother.motherRelay.digitalWrite(pump2, open);  
                Mother.motherRelay.digitalWrite(pump3, open);  
                Mother.motherRelay.digitalWrite(pump4, open); 
                Mother.motherRelay.digitalWrite(pump5, open); 
            #endif
        break;

        case DavidEnd:
            stopBelt();
            #ifndef Hamburg // No Belt no Light no pump in Hamburg
                #ifndef IgnoreLeds
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrGreen); 
                #endif
                Mother.motherRelay.digitalWrite(pump1, open); 
                Mother.motherRelay.digitalWrite(pump2, open);  
                Mother.motherRelay.digitalWrite(pump3, open);  
                Mother.motherRelay.digitalWrite(pump4, open); 
                Mother.motherRelay.digitalWrite(pump5, open); 
            #endif      
        break;

    }
    wdt_enable(WDTO_8S);
    wdt_reset();
    Serial.println("ending stage");
    stage = waitRequest;
}


/**
 * @brief  triggers effects specific to the given stage, 
 * room specific excecutions can happen here
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

void handleInputs() {

    // @CW you may want to just use enums to fill the cases here then you dont need to comment each random number ;-)
    if (stage != waitRequest) { return; }
    lastStage = waitRequest;    // not sure this is needed
    int result = MotherIO.getInputs();

    if (lastInput == result) {
        return;
    }
    lastInput = result;

    if (result > 0) {
        Serial.println(F("Input from Arbiter!"));
        Serial.println(result);
    }

    switch (result) {
        case (IOValues::dishout):
        DishCount = DishCount + 1;
        switch (DishCount) {
            case 1: stage = Dish1; break;
            case 2: stage = Dish2; break;
            case 3: stage = Dish3; break;
            case 4: stage = Dish4; break;
            case 5: stage = Dish5; break;
        }  
        break;
        case (IOValues::dish1): 
            stage = Dish1;
            DishCount = 1;
        break;
        case (IOValues::dish2): 
            stage = Dish2;
            DishCount = 2;
        break;
        case (IOValues::dish3):
            stage = Dish3;
            DishCount = 3;
        break;
        case (IOValues::dish4):
            stage = Dish4;
            DishCount = 4;
        break;
        case (IOValues::dish5):
            stage = Dish5;
            DishCount = 5;
        break;
        case(IOValues::rachelEnd):
            stage = WorldsEnd;
            DishCount = 6;
        break;
        case(IOValues::elancellEnd):
            stage = DavidEnd;
            DishCount = 6;
        break;
    }
}


void checkBelt() {
    if (beltActive > 0 && beltEndTime < millis()){
        stopBelt();
    }
}


void setup() {
    // starts serial and default oled
    Mother.begin();
    
    // delay(20000); // EnoughTime for Arbiter and Terminal to start up
    Mother.relayInit(relayPinArray, relayInitArray, relayAmount);
    MotherIO.ioInit(intputArray, sizeof(intputArray), outputArray, sizeof(outputArray));

    Serial.println("WDT endabled");
    wdt_enable(WDTO_8S);

    Mother.rs485SetSlaveCount(3);
    
    setStageIndex();
    wdt_reset();
}


void loop() {

    // Mother.rs485PerformPoll();   // no slave has inputs to read from
    handleInputs();    
    stageUpdate();
    checkBelt();
    wdt_reset();
}




