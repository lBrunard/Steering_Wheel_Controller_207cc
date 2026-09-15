#include <Arduino.h>
#include "can_bus.h"
#include "opto.h"

void setup(){
    Serial.begin(115200);
    delay(1000);

    optoInit();

    if (!start_twai()){
        delay(5000);
        ESP.restart();
    }
}

void loop(){

}
