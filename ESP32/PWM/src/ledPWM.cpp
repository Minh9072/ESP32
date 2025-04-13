#include <Arduino.h>

const int ledPin =  13;

const int freq = 5000;  // tần số xung
const int ledChannel = 0; // kênh PWM
const int resolution = 8; // độ phân giải 8bit

void setup() {
  // setup kênh pwm
  ledcSetup(ledChannel, freq, resolution); 
  // xuất pwm ra chân 16
  ledcAttachPin(ledPin, ledChannel);
}

void loop() {
  // Sáng dần
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    // changing the LED brightness with PWM
    ledcWrite(ledChannel, dutyCycle);
    delay(5);
  }

  // tắt dần
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
    // changing the LED brightness with PWM
    ledcWrite(ledChannel, dutyCycle);   
    delay(5);
  }
}