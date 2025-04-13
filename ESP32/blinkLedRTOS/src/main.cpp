#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const int ledPins[4] = {2, 16, 5, 19};

void chopTatLed12(void *pvParameters){
    while(1){
        for(int i=0; i<2; i++){
            digitalWrite(ledPins[i], HIGH);           
        }
        vTaskDelay(300/portTICK_PERIOD_MS);
        for(int i=0; i<2; i++){
            digitalWrite(ledPins[i], LOW);           
        }
        vTaskDelay(300/portTICK_PERIOD_MS);
    }
}

void chopTatLed34(void *pvParameters){
    while(1){
        for(int i=2; i<4; i++){
            digitalWrite(ledPins[i], HIGH);   
        }
        vTaskDelay(600/portTICK_PERIOD_MS);
        for(int i=2; i<4; i++){
            digitalWrite(ledPins[i], LOW);   
        }
        vTaskDelay(600/portTICK_PERIOD_MS);
    }
}

void setup(){
    for (int i = 0; i < 4; i++) {
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);
    }

    xTaskCreate(chopTatLed12, "chopTatLed12", 1000, NULL, 1, NULL);
    xTaskCreate(chopTatLed34, "chopTatLed34", 1000, NULL, 1, NULL);
}

void loop(){}