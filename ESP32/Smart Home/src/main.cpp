#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include "HC74595.h"
#include "StepperULN2003.h"
#include "L298MotorDriver.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FirebaseESP32.h>

// --------- Constants and Configuration ---------
#define DHT_PIN 33
#define DHT_TYPE DHT11
#define BUTTON_PIN 4
#define LED_PIN 25
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define FIREBASE_HOST "https://baocao-21d86-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "*************************************"
const char ssid[] = "Bao Tu Trong Trung_2.4G";
const char pass[] = "@2022.com";

// --------- Hardware Objects ---------
HC74595 shiftRegister(5, 19, 18); // dataPin, clockPin, latchPin
StepperULN2003 stepper(12, 14, 27, 26); // IN1-IN4 (GPIO pins)
L298MotorDriver motor(2, 15, 4); // IN1, IN2, ENA
DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --------- Firebase ---------
WiFiClient wifiClient;
FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;

// --------- Timing ---------
unsigned long previousMillis = 0;
unsigned long lastFirebaseSync = 0;

// ----------Global variable----------
String mainLightState, tvState, acState, fanState;
int lightBrightness, tvVolume, acTemperature, fanSpeed;
float temp, humd;
SemaphoreHandle_t displayMutex;

// ========== Function Prototypes ========== //
void displayMessage(const char* message);
void setupWiFi();
void setupFirebase();
void setupDisplay();
void syncFirebase();
void setupHardware();
void taskLightControl(void *pvParameters);
void taskACControl(void *pvParameters);
void taskFanControl(void *pvParameters);
void updateSensorReadings();
void taskDisplayUpdate(void *pvParameters);

// --------- Task Handles ---------
TaskHandle_t wifiTaskHandle = NULL;
TaskHandle_t firebaseTaskHandle = NULL;
TaskHandle_t lightTaskHandle = NULL;
TaskHandle_t acTaskHandle = NULL;
TaskHandle_t fanTaskHandle = NULL;
TaskHandle_t sensorTaskHandle = NULL;
TaskHandle_t displayTaskHandle = NULL;

// ------------------------------------------------
// Initialization
// ------------------------------------------------
void setup() {
    Serial.begin(9600);

    setupDisplay();
    setupWiFi();
    setupFirebase();
    setupHardware();

    displayMessage("System Ready");

    xTaskCreate(taskLightControl, "LightControlTask", 2048, NULL, 1, &lightTaskHandle);
    xTaskCreate(taskACControl, "ACControlTask", 2048, NULL, 1, &acTaskHandle);
    xTaskCreate(taskFanControl, "FanControlTask", 2048, NULL, 1, &fanTaskHandle);
    xTaskCreate(taskDisplayUpdate, "DisplayUpdateTask", 10000, NULL, 1, &displayTaskHandle);
}

// ------------------------------------------------
// Main Loop
// ------------------------------------------------
void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastFirebaseSync > 100) {
        syncFirebase();
        lastFirebaseSync = currentMillis;
    }

    updateSensorReadings();
}

// ------------------------------------------------
// WiFi Connection
// ------------------------------------------------
void setupWiFi() {
    WiFi.begin(ssid, pass, 6);
    displayMessage("Connecting to WiFi...");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        display.print(".");
        display.display();
    }

    displayMessage("WiFi Connected");
}

// ------------------------------------------------
// Firebase Setup
// ------------------------------------------------
void setupFirebase() {
    firebaseConfig.host = FIREBASE_HOST;
    firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&firebaseConfig, &firebaseAuth);
    Firebase.reconnectWiFi(true);
}

// ------------------------------------------------
// Display Setup
// ------------------------------------------------
void setupDisplay() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 connection failed");
        return;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Booting...");
    display.display();
    delay(2000);
}

// Helper to show message on screen
void displayMessage(const char* message) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(message);
    display.display();
}

// ------------------------------------------------
// Hardware Init
// ------------------------------------------------
void setupHardware() {
    shiftRegister.begin();
    shiftRegister.clear(); // Turn off all outputs

    stepper.begin();
    stepper.setRPM(20); // 20 RPM

    motor.begin();

    pinMode(LED_PIN, OUTPUT);
    dht.begin();
}

// ------------------------------------------------
// Firebase Access
// ------------------------------------------------
void syncFirebase() {
    if (Firebase.getString(firebaseData, "/Livingroom/main-light/state")) {
        mainLightState = firebaseData.stringData();
        Serial.print("Main Light State: ");
        Serial.println(mainLightState);
    }
    if (Firebase.getString(firebaseData, "/Livingroom/main-light/brightness")) {
        lightBrightness = firebaseData.stringData().toInt();
        Serial.print("Light Brightness: ");
        Serial.println(lightBrightness);
    }

    if (Firebase.getString(firebaseData, "/Livingroom/smartTV/state")) {
        tvState = firebaseData.stringData();
        Serial.print("Smart TV State: ");
        Serial.println(tvState);
    }
    if (Firebase.getString(firebaseData, "/Livingroom/smartTV/volume")) {
        tvVolume = firebaseData.stringData().toInt();
        Serial.print("TV Volume: ");
        Serial.println(tvVolume);
    }

    if (Firebase.getString(firebaseData, "/Livingroom/air-conditioner/state")) {
        acState = firebaseData.stringData();
        Serial.print("Air-conditioner State: ");
        Serial.println(acState);
    }
    if (Firebase.getString(firebaseData, "/Livingroom/air-conditioner/temp")) {
        acTemperature = firebaseData.stringData().toInt();
        Serial.print("AC Temperature: ");
        Serial.println(acTemperature);
    }

    if (Firebase.getString(firebaseData, "/Livingroom/fan/state")) {
        fanState = firebaseData.stringData();
        Serial.print("Fan State: ");
        Serial.println(fanState);
    }
    if (Firebase.getString(firebaseData, "/Livingroom/fan/speed")) {
        fanSpeed = firebaseData.stringData().toInt();
        Serial.print("Fan Speed: ");
        Serial.println(fanSpeed);
    }
}

// ------------------------------------------------
// Light Control Task
// ------------------------------------------------
void taskLightControl(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        if (mainLightState == "ON") {
            if (lightBrightness > 87.5) {
                shiftRegister.writeByte(0xFF, LSBFIRST);
            } else if (lightBrightness > 75) {
                shiftRegister.writeByte(0xFE, LSBFIRST);
            } else if (lightBrightness > 62.5) {
                shiftRegister.writeByte(0xFC, LSBFIRST);
            } else if (lightBrightness > 50) {
                shiftRegister.writeByte(0xF8, LSBFIRST);
            } else if (lightBrightness > 37.5) {
                shiftRegister.writeByte(0xF0, LSBFIRST);
            } else if (lightBrightness > 25) {
                shiftRegister.writeByte(0xE0, LSBFIRST);
            } else if (lightBrightness > 12.5) {
                shiftRegister.writeByte(0xC0, LSBFIRST);
            } else if (lightBrightness > 0) {
                shiftRegister.writeByte(0x80, LSBFIRST);
            } else {
                shiftRegister.writeByte(0x00, LSBFIRST);
            }
        } else {
            shiftRegister.writeByte(0x00, LSBFIRST);
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Update every 100ms
    }
    vTaskDelete(NULL);
}

// ------------------------------------------------
// AC Control Task
// ------------------------------------------------
void taskACControl(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        if (acState == "ON" && !stepper.isBusy()) {
            stepper.stepForward(2048);
        } else if (acState == "OFF") {
            stepper.stop();
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Update every 100ms
    }
    vTaskDelete(NULL);
}

// ------------------------------------------------
// Fan Control Task
// ------------------------------------------------
void taskFanControl(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        if (fanState == "ON") {
            motor.forward(fanSpeed * 2.55);
        } else {
            motor.stop();
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Update every 100ms
    }
    vTaskDelete(NULL);
}

// ------------------------------------------------
// Sensor Readings Task
// ------------------------------------------------
void updateSensorReadings() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 2000) {
        previousMillis = currentMillis;

        temp = dht.readTemperature();
        humd = dht.readHumidity();

        if (isnan(temp) || isnan(humd)) {
            Serial.println("Failed to read from DHT sensor!");
            return;
        }

        Firebase.setFloat(firebaseData, "/Livingroom/temperature", temp);
        Firebase.setFloat(firebaseData, "/Livingroom/humidity", humd);

        Serial.printf("Temp: %.2f °C, Humidity: %.2f %%\n", temp, humd);
    }
}

// ------------------------------------------------
// Display Update Task
// ------------------------------------------------
void taskDisplayUpdate(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        if (tvState == "ON") {
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(WHITE);
            display.setCursor(0, 0);

            // Display sensor data
            if (!isnan(temp) && !isnan(humd)) {
                display.printf("Temp: %.1f C\n", temp);
                display.printf("Hum:  %.1f %%\n", humd);
            } else {
                display.println("Temp: -- C");
                display.println("Hum:  -- %");
            }

            // Display device states
            display.println("------------");
            display.printf("Light: %s (%d%%)\n", mainLightState.c_str(), lightBrightness);
            display.printf("TV:    %s (Vol:%d)\n", tvState.c_str(), tvVolume);
            display.printf("AC:    %s (%dC)\n", acState.c_str(), acTemperature);
            display.printf("Fan:   %s (%d%%)\n", fanState.c_str(), fanSpeed);
            display.display();
        } else if (tvState == "OFF") {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.println("-----------------");
            display.println("------TV OFF-----");
            display.println("-----------------");
            display.display();
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Update display every 1 second
    }
    vTaskDelete(NULL);
}
