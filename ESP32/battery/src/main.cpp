#include <Arduino.h>

const int pinADC = 34;  // Chân ADC của ESP32
const float R1 = 2; // kΩ
const float R2 = 1;  // kΩ
const float Vref = 3.3; // Điện áp tham chiếu ESP32

float readBatteryVoltage() {
    int raw = analogRead(pinADC);
    float voltage = (raw / 4095.0) * Vref; // Chuyển đổi ADC -> điện áp
    return voltage * (R1 + R2) / R2; // Bù lại do mạch phân áp
}

float voltageToPercentage(float voltage) {
   return ((voltage-3)/(4.2-3))*100;
}

void setup() {
    Serial.begin(9600);
}

void loop() {
    float voltage = readBatteryVoltage();
    float percentage = voltageToPercentage(voltage);
    float raw = analogRead(pinADC);
    Serial.print("Điện áp pin: ");
    Serial.print(voltage);
    Serial.print("V - Dung lượng: ");
    Serial.print(percentage);
    Serial.println("%");
    Serial.print(raw);

    delay(2000);
}