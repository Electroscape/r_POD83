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

void func_move_servo(STB_MOTHER Mother,int Brain, int PWM_No) {
    // Secured Dishout more  Servo Movements to get sure
    SERVO_CMDS::moveServo(Mother, Brain, PWM_No, 0);
    delay(500);
    SERVO_CMDS::moveServo(Mother, Brain, PWM_No, 180);
    delay(500);
    /*
    SERVO_CMDS::moveServo(Mother, Brain, PWM_No, 0);
    delay(500);
    SERVO_CMDS::moveServo(Mother, Brain, PWM_No, 180);
    delay(500);
    SERVO_CMDS::moveServo(Mother, Brain, PWM_No, 180);
    delay(500);
    */
}


void stageActions() {
    wdt_reset();

    switch (stage) {
        case setupStage: 
            // Set Color
            #ifndef Hamburg
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrWhite, 20);
            #endif 
            delay(500);
        break;
        
        case waitRequest:break;

        case Dish1: // Start Belt normalDirection -> Empty Dish -> Start Light -> Start Pump -> Stop Belt normalDirection
            
            #ifndef Hamburg
                Mother.motherRelay.digitalWrite(BeltOn, open);  //Start Belt normalDirection
                delay(500);
                Mother.motherRelay.digitalWrite(Pump5, closed);  //Stop Pump
                delay(500);
                LED_CMDS::setLEDToClr(Mother, LED_Brain, LED_CMDS::clrYellow, 100, PWM::set1, 4);
                delay(800);
            #endif
            func_move_servo(Mother, Servo_Brain2,0);
            wdt_disable();           
            #ifndef Hamburg
                delay(13000); // long time for Belt
                Mother.motherRelay.digitalWrite(BeltOn, closed);  //Stop Belt normalDirection   
            #endif
            wdt_enable(WDTO_8S);    
        break;

        case Dish2: // Start Belt normalDirection -> Empty Dish -> Start Light -> Start Pump -> Stop Belt normalDirection
            #ifndef Hamburg // No Belt no Light no Pump in Hamburg
                Mother.motherRelay.digitalWrite(BeltOn, open);  //Start Belt normalDirection
                delay(500);
                Mother.motherRelay.digitalWrite(Pump4, closed);  //Start Belt normalDirection
                delay(500);
                LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrGreen, 100, PWM::set1, 3);
                delay(800); 
            #endif
            func_move_servo(Mother, Servo_Brain1,3);
            wdt_disable();          
            #ifndef Hamburg
                delay(11000);
                Mother.motherRelay.digitalWrite(BeltOn, closed);  //Stop Belt normalDirection   
            #endif
            wdt_enable(WDTO_8S);
        break;

        case Dish3:// Start Belt normalDirection -> Empty Dish -> Start Light -> Start Pump -> Stop Belt normalDirection           
            #ifndef Hamburg // No Belt no Light no Pump in Hamburg
                Mother.motherRelay.digitalWrite(BeltOn, open);  //Start Belt normalDirection
                delay(500);
                Mother.motherRelay.digitalWrite(Pump3, closed);  //Start Belt normalDirection
                delay(500);
                LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrPurple, 100, PWM::set1, 2);
                delay(800);
            #endif
            func_move_servo(Mother, Servo_Brain1,2);
            wdt_disable();       
            #ifndef Hamburg // No Belt no Light no Pump in Hamburg
                delay(11000);
                Mother.motherRelay.digitalWrite(BeltOn, closed);  //Stop Belt normalDirection   
            #endif
            wdt_enable(WDTO_8S);
        break;

        case Dish4: // Start Belt normalDirection -> Empty Dish -> Start Light -> Start Pump -> Stop Belt normalDirection
            #ifndef Hamburg // No Belt no Light no Pump in Hamburg
                Mother.motherRelay.digitalWrite(BeltOn, open);  //Start Belt normalDirection
                delay(500);
                Mother.motherRelay.digitalWrite(Pump2, closed);  //Start Belt normalDirection
                delay(500);            
                LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrBlue, 100, PWM::set1, 1);
                delay(800);
            #endif
            func_move_servo(Mother, Servo_Brain1,1);
            wdt_disable();
            #ifndef Hamburg // No Belt no Light no Pump in Hamburg
                delay(11000);
                Mother.motherRelay.digitalWrite(BeltOn, closed);  //Stop Belt normalDirection   
            #endif
            wdt_enable(WDTO_8S); 
        break;

        case Dish5: // Start Belt normalDirection -> Empty Dish -> Start Light -> Start Pump -> Stop Belt normalDirection
            #ifndef Hamburg // No Belt no Light no Pump in Hamburg
                Mother.motherRelay.digitalWrite(Belt1, open);  //Change Belt Direction
                Mother.motherRelay.digitalWrite(Belt2, open);  //change Belt Direction
                delay(100);
                Mother.motherRelay.digitalWrite(BeltOn, open);  //Start Belt 
                delay(500);
                Mother.motherRelay.digitalWrite(Pump1, closed);  //Start Belt normalDirection
                delay(500);
                LED_CMDS::setLEDToClr(Mother, LED_Brain , LED_CMDS::clrRed, 100, PWM::set1, 0);
                delay(800);
            #endif
            func_move_servo(Mother, Servo_Brain1,0);
            wdt_disable();
            
            #ifndef Hamburg // No Belt no Light no Pump in Hamburg
                delay(12000);
                Mother.motherRelay.digitalWrite(BeltOn, closed);  //Stop Belt normalDirection
                delay(200);   
                Mother.motherRelay.digitalWrite(Belt1, closed);  //Change Belt Direction
            Mother.motherRelay.digitalWrite(Belt2, closed);  //change Belt Direction
            #endif
            wdt_enable(WDTO_8S);
        break;

        case WorldsEnd: 
            #ifndef Hamburg // No Belt no Light no Pump in Hamburg
                Mother.motherRelay.digitalWrite(Belt1, open);  //Change Belt Direction
                Mother.motherRelay.digitalWrite(Belt2, open);  //change Belt Direction
                delay(100);
                Mother.motherRelay.digitalWrite(BeltOn, open);  //Start Belt 
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
                wdt_disable();
                delay(5000);
                wdt_enable(WDTO_8S);
                Mother.motherRelay.digitalWrite(BeltOn, closed);  //Stop Belt   
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrBlack); 
                Mother.motherRelay.digitalWrite(Pump1, open); 
                Mother.motherRelay.digitalWrite(Pump2, open);  
                Mother.motherRelay.digitalWrite(Pump3, open);  
                Mother.motherRelay.digitalWrite(Pump4, open); 
                Mother.motherRelay.digitalWrite(Pump5, open); 
            #endif
        break;

        case DavidEnd:
            #ifndef Hamburg // No Belt no Light no Pump in Hamburg
                LED_CMDS::setAllStripsToClr(Mother, LED_Brain , LED_CMDS::clrGreen); 
                Mother.motherRelay.digitalWrite(Pump1, open); 
                Mother.motherRelay.digitalWrite(Pump2, open);  
                Mother.motherRelay.digitalWrite(Pump3, open);  
                Mother.motherRelay.digitalWrite(Pump4, open); 
                Mother.motherRelay.digitalWrite(Pump5, open); 
            #endif      
        break;

    }
    stage = waitRequest;
    wdt_reset();
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
    lastStage = waitRequest;
    int result = MotherIO.getInputs();
    if (result > 0) {
        Serial.println(F("Input from Arbiter!"));
        Serial.println(result);
    }
    switch (result) {
        case (1 << 0): //dispenserAction
        wdt_reset();
        DishCount = DishCount + 1;
        switch (DishCount) {
            case 1: stage = Dish1; break;
            case 2: stage = Dish2; break;
            case 3: stage = Dish3; break;
            case 4: stage = Dish4; break;
            case 5: stage = Dish5; break;
            case 6: stage = WorldsEnd; break;
            case 7: stage = setupStage; DishCount = 0; break; //reset
        }  
        break;
        case (2): //Dish1
            wdt_reset();
            stage = Dish1;
            DishCount = 1;
        break;
        case (3): //Dish2
            wdt_reset();
            stage = Dish2;
            DishCount = 2;
        break;
        case (4): //Dish3
            wdt_reset();
            stage = Dish3;
            DishCount = 3;
        break;
        case (5): //Dish4
            wdt_reset();
            stage = Dish4;
            DishCount = 4;
        break;
        case (6): //Dish5
            wdt_reset();
            stage = Dish5;
            DishCount = 5;
        break;
        case(7): // Rachel End
            wdt_reset();
            stage = WorldsEnd;
            DishCount = 6;
        break;
        case(8): // Elancell End
            wdt_reset();
            stage= DavidEnd;
            DishCount = 6;
        break;
    }
    /* if (result == dispenserAction) {
        wdt_reset();
        DishCount = DishCount + 1;
        switch (DishCount) {
            case 1: stage = Dish1; break;
            case 2: stage = Dish2; break;
            case 3: stage = Dish3; break;
            case 4: stage = Dish4; break;
            case 5: stage = Dish5; break;
            case 6: stage = WorldsEnd; break;
            case 7: stage = setupStage; DishCount = 0; break; //reset
        }        

    } */
}

void setup() {

    // starts serial and default oled
    Mother.begin();
    
    //delay(20000); // EnoughTime for Arbiter and Terminal to start up
    Mother.relayInit(relayPinArray, relayInitArray, relayAmount);
    MotherIO.ioInit(intputArray, sizeof(intputArray), outputArray, sizeof(outputArray));

    Serial.println("WDT endabled");
    wdt_enable(WDTO_8S);

    Mother.rs485SetSlaveCount(3);
    
    setStageIndex();
    wdt_reset();
}


void loop() {

    Mother.rs485PerformPoll(); // change to input from Arbiter Pi
    handleInputs();    
    stageUpdate();

    wdt_reset();
}




