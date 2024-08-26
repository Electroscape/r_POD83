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
 * - double post of clean airlock & decontamination
 * - enumerating brain types
 * 🔲 add feedback to close the door when tryint to lock
 *  6. Wrong code entered on access module -> "Access denied" currently not implemented
 * ✅ add numbering of brains to header to make changes easiers
 * in between stage or text for unlocking with rfid
 *  Q:
 * 🔲 Timeout needs to send wrongres
 * 🔲 booting text on startup
 * 🔲 consider using laststage for the switch cases?
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

int lastState = -1;

unsigned long last_flutter_time = millis(); 
unsigned long repeat_time = random(180000, 240000); //time between flutter sequences
unsigned long next_flutter_step = 0; //time, in flutter sequence, between diffrent flutter elements
unsigned long last_flutter_step = 0; //last time a flutter sequence ended
int flutter_color_possible = 0; // 0=nothing, 1=red, 2=blue
bool flutter_possible_general = false; //flutter set possible by arbiter and triggered by time (75 min)
int flutter_activity_count = 1; //counting flutter elements in flutter sequence
int flutter_activity_end = random(10, 15); //number of flutter elements in one flutter sequence


void enableWdt() {
    wdt_enable(WDTO_8S);
}


void flutter() {

    unsigned long now_time = millis();

    //flutter blue
    if (now_time - last_flutter_time > repeat_time && now_time - last_flutter_step >= next_flutter_step && flutter_color_possible > 0 && flutter_possible_general){

        if(flutter_activity_count % 2 == 0){
            if(flutter_color_possible == 1){
                LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrRed, random(70, 100));
            }
            if(flutter_color_possible == 2){
                LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlue, random(70, 100));
            }
            next_flutter_step = random(20, 150);
        }
        else{
            if(flutter_color_possible == 1){
                LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrRed, random(0, 30));
            }
            if(flutter_color_possible == 2){
                LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlue, random(0, 30));
            }
            next_flutter_step = random(20, 150);
        }
        last_flutter_step = millis();
        flutter_activity_count = flutter_activity_count + 1;
        
        if(flutter_activity_count == flutter_activity_end){
            if(flutter_color_possible == 1){
                LED_CMDS::fade2color(Mother, ledCeilBrain, LED_CMDS::clrGreen, 0, LED_CMDS::clrRed, 40, 2000, 1);
                LED_CMDS::fade2color(Mother, ledCeilBrain, LED_CMDS::clrGreen, 0, LED_CMDS::clrRed, 40, 2000, 2);
                LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrGreen, 100);
            }
            if(flutter_color_possible == 2){
                LED_CMDS::fade2color(Mother, ledCeilBrain, LED_CMDS::clrBlue, 0, LED_CMDS::clrBlue, 55, 2000, 1);
                LED_CMDS::fade2color(Mother, ledCeilBrain, LED_CMDS::clrBlue, 0, LED_CMDS::clrBlue, 55, 2000, 2);
                LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlue, 100);
            }
            flutter_activity_count = 1;
            repeat_time = random(180000, 240000);
            next_flutter_step = 0;
            last_flutter_time = now_time;
            flutter_activity_end = random(10, 15);
        }
    }
}


void handleInputs() {

    int result = MotherIO.getInputs();

    if (lastState == result || result == 0) {
        return;
    }
    lastState = result;

    unsigned long startTime = millis();
    Serial.println(result);

    switch (result) {
        case lightOff: 
            Mother.motherRelay.digitalWrite(labEntry, closed);
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlack, 100);
        break;
        case lightNormal: 
            Mother.motherRelay.digitalWrite(labEntry, open);
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, clrLight, 60);
            flutter_color_possible = 0;
        break;
        case lightRed:
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrRed, 40);
            flutter_color_possible = 1;
        break;
        case lightBlue:
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrBlue, 55);
            flutter_color_possible = 2;
        break;
        case flutteractivate:
            flutter_possible_general = true;
        break;
        case lightRachelAnnouncement:
            flutter_color_possible = 0;
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrRed, 30);
            Mother.motherRelay.digitalWrite(labEntry, open);
            #ifndef Hamburg
            wdt_disable();
            while ((millis() - startTime) < (unsigned long) 42500) {}
            startTime = millis();
            while ((millis() - startTime) < (unsigned long) 50000) {
                LED_CMDS::fade2color(Mother, ledCeilBrain, LED_CMDS::clrRed, 30, LED_CMDS::clrBlack, 30, 600, 1);
                delay(600);
                LED_CMDS::fade2color(Mother, ledCeilBrain, LED_CMDS::clrBlack, 30, LED_CMDS::clrRed, 30, 600, 1);
                delay(600);
            }
            LED_CMDS::fade2color(Mother, ledCeilBrain, LED_CMDS::clrRed, 30, LED_CMDS::clrBlack, 30, 600, 1);
            startTime = millis();
            while ((millis() - startTime) < (unsigned long) 60000) {}
            LED_CMDS::fade2color(Mother, ledCeilBrain, LED_CMDS::clrBlack, 30, LED_CMDS::clrWhite, 60, 600, 1); 
            wdt_enable(WDTO_8S);
            #endif
        break;
        case lightRachelEnd:
            flutter_color_possible = 0;
            Mother.motherRelay.digitalWrite(labEntry, open);
            wdt_disable();
            #ifdef Hamburg
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
            #else
                while ((millis() - startTime) < (unsigned long) 10000) {
                    LED_CMDS::fade2color(Mother, ledCeilBrain, LED_CMDS::clrRed, 30, LED_CMDS::clrBlack, 30, 600, 1);
                    delay(600);
                    LED_CMDS::fade2color(Mother, ledCeilBrain, LED_CMDS::clrBlack, 30, LED_CMDS::clrRed, 30, 600, 1);
                    delay(600);
                }
                LED_CMDS::fade2color(Mother, ledCeilBrain, LED_CMDS::clrRed, 30, LED_CMDS::clrBlack, 30, 600, 1);  
                while ((millis() - startTime) < (unsigned long) 59400) {}
                LED_CMDS::fade2color(Mother, ledCeilBrain, LED_CMDS::clrBlack, 30, LED_CMDS::clrWhite, 60, 600, 1);  

            #endif              
            wdt_enable(WDTO_8S);
        break;

        case lightDavidAnnouncement:
            flutter_color_possible = 0;
            Mother.motherRelay.digitalWrite(labEntry, open);
            LED_CMDS::setAllStripsToClr(Mother, ledCeilBrain, LED_CMDS::clrGreen, 40);
        break;
        case IO::deconTrigger:
            Mother.motherRelay.digitalWrite(decon, open);
            delay(5000);
            Mother.motherRelay.digitalWrite(decon, closed);
        break;
        default: break;
    }

}


void setup() {

    Mother.begin();
    // starts serial and default oled
    Mother.relayInit(relayPinArray, relayInitArray, relayAmount);
    MotherIO.ioInit(intputArray, sizeof(intputArray), outputArray, sizeof(outputArray));

    Serial.println("WDT endabled");
    enableWdt();

    // technicall 3 but for the first segments till unlocked there is no need
    Mother.rs485SetSlaveCount(0);



    wdt_reset();

}

void loop() {
    handleInputs();   
    wdt_reset();
    flutter();
}




