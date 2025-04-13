#include <Arduino.h>

const int stbPin = 15; 
const int clockPin = 2; 
const int dataPin = 4; 
                     /*0*/ /*1*/ /*2*/ /*3*/ /*4*/ /*5*/ /*6*/ /*7*/ /*8*/ /*9*/ 
uint8_t segCode[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f }; 
uint8_t dataDisplay[] = { 0x00, 0x00, 0x40, 0x00, 0x00, 0x40, 0x00, 0x00 };
uint8_t gio=0, phut=0, giay=0 ; 
 
void sendCommand(uint8_t value) 
{ 
  digitalWrite(stbPin, LOW); 
  shiftOut(dataPin, clockPin, LSBFIRST, value); 
  digitalWrite(stbPin, HIGH); 
} 
 
void TM1638init() 
{ 
  sendCommand(0x40); // set auto increment mode 
  digitalWrite(stbPin, LOW); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0xc0);   // set starting address to 0 
  for(uint8_t i = 0; i < 16; i++) 
  { 
    shiftOut(dataPin, clockPin, LSBFIRST, 0x00); 
  } 
  digitalWrite(stbPin, HIGH); 
} 

void segDisplay(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6, uint8_t data7, uint8_t data8) 
{ 
  sendCommand(0x40); 
  digitalWrite(stbPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0xc0);  
  shiftOut(dataPin, clockPin, LSBFIRST, data1); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0x00);
  shiftOut(dataPin, clockPin, LSBFIRST, data2); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0x00);
  shiftOut(dataPin, clockPin, LSBFIRST, data3); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0x00);
  shiftOut(dataPin, clockPin, LSBFIRST, data4); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0x00);
  shiftOut(dataPin, clockPin, LSBFIRST, data5); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0x00); 
  shiftOut(dataPin, clockPin, LSBFIRST, data6); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0x00); 
  shiftOut(dataPin, clockPin, LSBFIRST, data7); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0x00); 
  shiftOut(dataPin, clockPin, LSBFIRST, data8); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0x00); 
  digitalWrite(stbPin, HIGH); 
} 

void setLed(uint8_t value) 
{ 
  pinMode(dataPin, OUTPUT); 
 
  sendCommand(0x44); 
  digitalWrite(stbPin, LOW); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0xC1 + (7 << 1)); 
  shiftOut(dataPin, clockPin, LSBFIRST, value); 
  digitalWrite(stbPin, HIGH); 

  digitalWrite(stbPin, LOW); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0xC1 + (6 << 1)); 
  shiftOut(dataPin, clockPin, LSBFIRST, value >> 1); 
  digitalWrite(stbPin, HIGH); 

  digitalWrite(stbPin, LOW); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0xC1 + (5 << 1)); 
  shiftOut(dataPin, clockPin, LSBFIRST, value >> 2); 
  digitalWrite(stbPin, HIGH); 

  digitalWrite(stbPin, LOW); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0xC1 + (4 << 1)); 
  shiftOut(dataPin, clockPin, LSBFIRST, value >> 3); 
  digitalWrite(stbPin, HIGH); 

  digitalWrite(stbPin, LOW); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0xC1 + (3 << 1)); 
  shiftOut(dataPin, clockPin, LSBFIRST, value >> 4); 
  digitalWrite(stbPin, HIGH); 

  digitalWrite(stbPin, LOW); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0xC1 + (2 << 1)); 
  shiftOut(dataPin, clockPin, LSBFIRST, value >> 5); 
  digitalWrite(stbPin, HIGH); 

  digitalWrite(stbPin, LOW); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0xC1 + (1 << 1)); 
  shiftOut(dataPin, clockPin, LSBFIRST, value >> 6); 
  digitalWrite(stbPin, HIGH); 

  digitalWrite(stbPin, LOW); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0xC1 + (0 << 1)); 
  shiftOut(dataPin, clockPin, LSBFIRST, value >> 7); 
  digitalWrite(stbPin, HIGH); 
}
 
void setup() 
{ 
  pinMode(stbPin, OUTPUT); 
  pinMode(clockPin, OUTPUT); 
  pinMode(dataPin, OUTPUT); 
 
  sendCommand(0x88);  // activate 
  TM1638init(); 
} 

void loop() 
{ 
  giay++;
  if(giay>59)
  {
    giay = 0; phut++;
    if(phut>59)
    {
      phut = 0; gio++;
    }
  }
  segDisplay(segCode[gio/10], segCode[gio%10], 0x40, segCode[phut/10], segCode[phut%10], 0x40, segCode[giay/10], segCode[giay%10]);
  delay(100);

  uint8_t y = 0;
  for(int i = 0; i < 8; i++){
    y=(y>>1)+0x80;
    setLed(y);
    delay(400);
  }
  for(int i = 0; i < 8; i++){
    y=y>>1;
    setLed(y);
    delay(400);
  }
}