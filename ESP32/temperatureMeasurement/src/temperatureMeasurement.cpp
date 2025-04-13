#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#include <LM75.h>
LM75 Temp0; //(0x48);

const int buttonPin = 26;
const int alarmPin = 13;
uint8_t maxTemp = 32;

const int dataPin = 4;  // Chân DS của 74HC595, Chân DIO của TM1638
const int clockPin = 2; // Chân SH_CP của 74HC595, Chân CLK của TM1638
const int latchPin = 5; // Chân ST_CP của 74HC595
const int stbPin = 15;  // Chân STB của TM1638

                     /*0*/ /*1*/ /*2*/ /*3*/ /*4*/ /*5*/ /*6*/ /*7*/ /*8*/ /*9*/ 
uint8_t segCode[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f}; 
                  /*H*/ /*E*/ /*L*/ /*L*/ /*O*/ /*.*/ /*.*/ /*.*/ 
uint8_t text[] = {0x76, 0x79, 0x38, 0x38, 0x3f, 0x80, 0x80, 0x80};

void sendCommand(uint8_t value) { 
  digitalWrite(stbPin, LOW); 
  shiftOut(dataPin, clockPin, LSBFIRST, value); 
  digitalWrite(stbPin, HIGH); 
} 
 
void TM1638init() { 
  sendCommand(0x40); // set auto increment mode 
  digitalWrite(stbPin, LOW); 
  shiftOut(dataPin, clockPin, LSBFIRST, 0xc0);   // set starting address to 0 
  for(uint8_t i = 0; i < 16; i++) 
  { 
    shiftOut(dataPin, clockPin, LSBFIRST, 0x00); 
  } 
  digitalWrite(stbPin, HIGH); 
} 

void segDisplay(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6, uint8_t data7, uint8_t data8) { 
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

void setLed(uint8_t value) { 
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

// Hàm gửi dữ liệu đến 74HC595
void sendData(byte data) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, data);
  digitalWrite(latchPin, HIGH);
}

void button(){
  if(digitalRead(buttonPin)==0){
    delay(10);
    if(digitalRead(buttonPin)==0){
      while(digitalRead(buttonPin)==0);
      maxTemp = maxTemp + 2;
      if(maxTemp > 50) maxTemp = 20;
    }
  }
}

void alarm(){
  digitalWrite(alarmPin, HIGH);
  sendData(0x01);
  setLed(0xff);
  button();
  delay(200); 

  digitalWrite(alarmPin, LOW);
  sendData(0x00);
  setLed(0x00);
  button();
}

void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(alarmPin, OUTPUT);
  pinMode(stbPin, OUTPUT);
  sendCommand(0x88); 
  TM1638init();

  Temp0.begin();
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Hello.....");
  display.display();

  segDisplay(text[0], text[1], text[2], text[3], text[4], text[5], text[6], text[7]);
  delay(2000);
}

void loop() {
  button();
  float temp = Temp0.getTemperature();
  int tempI = Temp0.getTemperature();
  if(temp > maxTemp) {
    display.invertDisplay(true);
    alarm();
  }
  else display.invertDisplay(false);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(1, 1);
  display.println("Nhiet Do:");
  display.print(temp);
  display.print((char)248);
  display.println("C");
  display.println(" ");

  display.print("Max:");
  display.print(maxTemp);
  display.print((char)248);
  display.println("C");
  display.display();

  segDisplay(segCode[tempI/10], segCode[tempI%10], 0x63, 0x39, segCode[maxTemp/10], segCode[maxTemp%10], 0x63, 0x39);
  delay(100);
  
}