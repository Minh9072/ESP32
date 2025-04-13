#include <Arduino.h>

const int dataPin = 4;  // Chân DS của 74HC595
const int clockPin = 2; // Chân SH_CP của 74HC595
const int latchPin = 5; // Chân ST_CP của 74HC595

// Hàm gửi dữ liệu đến 74HC595
void sendData(byte data) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, data);
  digitalWrite(latchPin, HIGH);
}

void sangDan(){
  uint8_t y = 0;
  for(int i = 0; i < 8; i++){
    y=(y>>1)+0x80;
    sendData(y);
    delay(400);
  }
  for(int i = 0; i < 8; i++){
    y=y>>1;
    sendData(y);
    delay(400);
  }
}

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
}

void loop() {
  sangDan();

}