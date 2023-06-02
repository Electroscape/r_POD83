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
int beltActive = 0;     // also saves the dishNo


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
    // on startup the servo is on position 0
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
        beltEndTime = millis() + 14000;
        beltActive = beltNo;
    #endif
}

void stopBelt() {
    Mother.motherRelay.digitalWrite(BeltOn, closed);  //Start Belt 
    delay(50);   
    Mother.motherRelay.digitalWrite(Belt1, closed);  // Change Belt Direction
    Mother.motherRelay.digitalWrite(Belt2, closed);  // change Belt Direction

    Mother.motherRelay.digitalWrite(Pump1, PumpOn);
    Mother.motherRelay.digitalWrite(Pump2, PumpOn);
    Mother.motherRelay.digitalWrite(Pump3, PumpOn);
    Mother.motherRelay.digitalWrite(Pump4, PumpOn);
    Mother.motherRelay.digitalWrite(Pump5, PumpOn);

    beltActive = 0;
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
            DishCount = 0;
            // Set Color
            #ifndef Hamburg
                #ifndef IgnoreLeds
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrWhite, 20);
                #endif
            #endif 
        break;
        
        case waitRequest: break;

        case Dish1: // Start Belt normalDirection -> Empty Dish -> Start Light -> Start Pump -> Stop Belt normalDirection
            
            #ifndef Hamburg
                beginBelt(Dish1);
                delay(500);
                Mother.motherRelay.digitalWrite(Pump5, PumpOff);
                delay(500);
                #ifndef IgnoreLeds
                LED_CMDS::setLEDToClr(Mother, LED_Brain, LED_CMDS::clrYellow, 100, PWM::set1, 4);
                #endif
                delay(800);
            #endif
            func_move_servo(Servo_Brain2,0);   
        break;

        case Dish2: // Start Belt normalDirection -> Empty Dish -> Start Light -> Start Pump -> Stop Belt normalDirection
            #ifndef Hamburg // No Belt no Light no Pump in Hamburg
                beginBelt(Dish2);
                delay(500);
                Mother.motherRelay.digitalWrite(Pump4, PumpOff);  //Start Belt normalDirection
                delay(500);
                #ifndef IgnoreLeds
                LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrGreen, 100, PWM::set1, 3);
                #endif
                delay(800); 
            #endif
            func_move_servo(Servo_Brain1,3);
        break;

        case Dish3:// Start Belt normalDirection -> Empty Dish -> Start Light -> Start Pump -> Stop Belt normalDirection           
            #ifndef Hamburg // No Belt no Light no Pump in Hamburg
                beginBelt(Dish3);
                delay(500);
                Mother.motherRelay.digitalWrite(Pump3, PumpOff); 
                delay(500);
                #ifndef IgnoreLeds
                LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrPurple, 100, PWM::set1, 2);
                #endif
                delay(800);
            #endif
            func_move_servo( Servo_Brain1,2);
            wdt_enable(WDTO_8S);
        break;

        case Dish4: // Start Belt normalDirection -> Empty Dish -> Start Light -> Start Pump -> Stop Belt normalDirection
            #ifndef Hamburg // No Belt no Light no Pump in Hamburg
                beginBelt(Dish4);
                delay(500);
                Mother.motherRelay.digitalWrite(Pump2, PumpOff);
                delay(500);  
                #ifndef IgnoreLeds          
                LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrBlue, 100, PWM::set1, 1);
                #endif
                delay(800);
            #endif
            func_move_servo(Servo_Brain1, 1);
        break;

        case Dish5: // Start Belt normalDirection -> Empty Dish -> Start Light -> Start Pump -> Stop Belt normalDirection
            #ifndef Hamburg // No Belt no Light no Pump in Hamburg
                Mother.motherRelay.digitalWrite(Belt1, open);  //Change Belt Direction
                Mother.motherRelay.digitalWrite(Belt2, open);  //change Belt Direction
                delay(100);
                beginBelt(Dish5);
                delay(500);
                Mother.motherRelay.digitalWrite(Pump1, PumpOff); 
                delay(500);
                #ifndef IgnoreLeds
                LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrRed, 100, PWM::set1, 0);
                #endif
                delay(800);
            #endif
            func_move_servo( Servo_Brain1,0);
        break;

        case WorldsEnd: 
            stopBelt();
            #ifndef Hamburg // No Belt no Light no Pump in Hamburg
                Mother.motherRelay.digitalWrite(Belt1, open);  //Change Belt Direction
                Mother.motherRelay.digitalWrite(Belt2, open);  //change Belt Direction
                delay(100);
                Mother.motherRelay.digitalWrite(BeltOn, open);  //Start Belt 
                #ifndef IgnoreLeds
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrRed, 100); 
                delay(300);
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrBlack, 100); 
                delay(100);
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrRed, 100); 
                delay(300);
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrBlack, 100); 
                delay(100);
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrRed, 100); 
                delay(300);
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrBlack, 100); 
                delay(100);
                wdt_reset();
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrRed, 100); 
                delay(300);
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrBlack, 100); 
                delay(100);
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrRed, 100); 
                delay(300);
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrBlack, 100); 
                delay(100);
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrRed, 100); 
                delay(300);
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrBlack, 100); 
                delay(100);
                wdt_reset();
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrRed, 100); 
                //LED_CMDS::blinking(Mother, LED_Brain , LED_CMDS::clrBlack, LED_CMDS::clrRed, 100, 400, 100, 100,PWM::set1);
                delay(300);
                wdt_disable();
                delay(10000);
                wdt_enable(WDTO_8S);
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrRed);
                #endif
                wdt_disable();
                delay(5000);
                wdt_enable(WDTO_8S);
                Mother.motherRelay.digitalWrite(BeltOn, closed);  //Stop Belt   
                #ifndef IgnoreLeds
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrBlack); 
                #endif
                Mother.motherRelay.digitalWrite(Pump1, open); 
                Mother.motherRelay.digitalWrite(Pump2, open);  
                Mother.motherRelay.digitalWrite(Pump3, open);  
                Mother.motherRelay.digitalWrite(Pump4, open); 
                Mother.motherRelay.digitalWrite(Pump5, open); 
            #endif
        break;

        case DavidEnd:
            stopBelt();
            #ifndef Hamburg // No Belt no Light no Pump in Hamburg
                #ifndef IgnoreLeds
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrGreen); 
                #endif
                Mother.motherRelay.digitalWrite(Pump1, open); 
                Mother.motherRelay.digitalWrite(Pump2, open);  
                Mother.motherRelay.digitalWrite(Pump3, open);  
                Mother.motherRelay.digitalWrite(Pump4, open); 
                Mother.motherRelay.digitalWrite(Pump5, open); 
            #endif      
        break;

    }
    wdt_reset();
    Serial.println("ending stage");
    // delay(3500);
    stage = waitRequest;
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
        wdt_reset();
        DishCount = DishCount + 1;
        switch (DishCount) {
            case 1: stage = Dish1; break;
            case 2: stage = Dish2; break;
            case 3: stage = Dish3; break;
            case 4: stage = Dish4; break;
            case 5: stage = Dish5; break;
            case 7: stage = setupStage; break; //reset
        }  
        break;
        case (IOValues::dish1): 
            wdt_reset();
            stage = Dish1;
            DishCount = 1;
        break;
        case (IOValues::dish2): 
            wdt_reset();
            stage = Dish2;
            DishCount = 2;
        break;
        case (IOValues::dish3):
            wdt_reset();
            stage = Dish3;
            DishCount = 3;
        break;
        case (IOValues::dish4):
            wdt_reset();
            stage = Dish4;
            DishCount = 4;
        break;
        case (IOValues::dish5):
            wdt_reset();
            stage = Dish5;
            DishCount = 5;
        break;
        case(IOValues::rachelEnd):
            wdt_reset();
            stage = WorldsEnd;
            DishCount = 6;
        break;
        case(IOValues::elancellEnd):
            wdt_reset();
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




