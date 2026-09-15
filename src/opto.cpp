#include <Arduino.h>
#include "config.h"
#include "opto.h"

static const int pins[] = {PIN_VOL_PLUS, PIN_VOL_MINUS};
static const int NB_PINS = sizeof(pins) / sizeof(pins[0]);

void optoInit(){
    for(int i = 0; i < NB_PINS; i++){
        pinMode(pins[i], OUTPUT);
    }
    optoReleaseAll();
}

void optoReleaseAll(){
    for(int i = 0; i < NB_PINS; i++){
        digitalWrite(pins[i], 0);
    }
}

void optoPress(bool state, int pin){
    optoReleaseAll();
    digitalWrite(pin, state ? 1 : 0);
}
