#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "Bao Tu Trong Trung_2.4G";
const char* password = "@2022.com";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 25200, 60000);

char Time[] = "00:00:00";
char Date[] = "00/00/2000";
byte lastSecond_, second_, minute_, hour_, day_, month_;
int year_;

const int dataPin = 4;  // Chân DS của 74HC595, Chân DIO của TM1638
const int clockPin = 2; // Chân SH_CP của 74HC595, Chân CLK của TM1638
const int latchPin = 5; // Chân ST_CP của 74HC595

void sendData(byte data) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, data);
  digitalWrite(latchPin, HIGH);
}

void setup() {
    sendData(0x00);

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println("Hello.....");
    display.display();

    Serial.begin(9600);
    Serial.print("Connecting to wifi");
    Serial.println(ssid);
  
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    timeClient.begin();
}

void loop() {
    timeClient.update();
    unsigned long unix_epoch = timeClient.getEpochTime();
    second_ = second(unix_epoch);
    if(lastSecond_ != second_){
        minute_ = minute(unix_epoch);
        hour_ = hour(unix_epoch);
        day_ = day(unix_epoch);
        month_ = month(unix_epoch);
        year_ = year(unix_epoch);

        Time[7] = second_ % 10 + 48;
        Time[6] = second_ / 10 + 48;
        Time[4]  = minute_ % 10 + 48;
        Time[3]  = minute_ / 10 + 48;
        Time[1]  = hour_ % 10 + 48;
        Time[0]  = hour_ / 10 + 48;

        Date[0]  = day_ / 10 + 48;
        Date[1]  = day_ % 10 + 48;
        Date[3]  = month_ / 10 + 48;
        Date[4]  = month_ % 10 + 48;
        Date[8] = (year_ / 10) % 10 + 48;
        Date[9] = year_ % 10 % 10 + 48;

        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextSize(2);
        display.println("Time:");
        display.println(Time);
        display.println("Date:");
        display.println(Date);
        display.display();
    }
    delay(500);
}
