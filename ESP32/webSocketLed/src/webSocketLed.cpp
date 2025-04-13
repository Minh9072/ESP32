#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "Bao Tu Trong Trung_2.4G";
const char* password = "@2022.com";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const int ledPins[8] = {12, 14, 27, 26, 25, 33, 32, 5};
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Điều khiển 8 LED</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            text-align: center;
            margin: 0;
            padding: 20px;
        }
        h2 {
            color: #fff;
            background: linear-gradient(45deg, #007bff, #00c6ff);
            padding: 15px;
            border-radius: 10px;
            display: inline-block;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
        }
        .led-container {
            display: flex;
            flex-wrap: wrap;
            justify-content: center;
            gap: 20px;
            margin-top: 20px;
        }
        .led-wrapper {
            display: flex;
            flex-direction: column;
            align-items: center;
        }
        .led {
            width: 60px;
            height: 60px;
            border-radius: 50%;
            background-color: gray;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.3);
            transition: background-color 0.3s, box-shadow 0.3s;
        }
        .led.active {
            background-color: red;
            box-shadow: 0 0 20px red;
        }
        .btn {
            margin-top: 10px;
            padding: 8px 15px;
            cursor: pointer;
            border: none;
            border-radius: 5px;
            background-color: #007bff;
            color: white;
            font-size: 14px;
            transition: background-color 0.3s;
        }
        .btn:hover {
            background-color: #0056b3;
        }
        .btn.active {
            background-color: green;
        }
    </style>
</head>
<body>
    <h2>Điều khiển 8 LED</h2>
    <div class="led-container" id="ledContainer">
        <!-- LED và nút điều khiển -->
    </div>
    <button class="btn" onclick="lightUpSequentially()">Sáng Dần</button>
    <button class="btn" onclick="turnOffSequentially()">Tắt Dần</button>
    <button class="btn" onclick="lightUpAll()">Sáng Hết</button>
    <button class="btn" onclick="turnOffAll()">Tắt Hết</button>
    
    <script>
        const ledContainer = document.getElementById('ledContainer');
        for (let i = 0; i < 8; i++) {
            const ledWrapper = document.createElement('div');
            ledWrapper.classList.add('led-wrapper');

            const led = document.createElement('div');
            led.classList.add('led');
            led.id = `led${i}`;
            
            const button = document.createElement('button');
            button.innerText = `LED ${i + 1}`;
            button.classList.add('btn');
            button.id = `btn${i}`;
            button.onclick = () => toggleLed(i);
            
            ledWrapper.appendChild(led);
            ledWrapper.appendChild(button);
            ledContainer.appendChild(ledWrapper);
        }

        var gateway = `ws://${window.location.hostname}/ws`;
        let ws;

        function initWebSocket() {
            ws = new WebSocket(gateway);
            ws.onopen = () => console.log("WebSocket kết nối!");
            ws.onmessage = (event) => console.log("ESP32 gửi:", event.data);
            ws.onclose = () => setTimeout(initWebSocket, 1000);
        }

        function sendCommand(command) {
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(command);
            }
        }

        // Gửi lệnh khi nhấn các nút
        function toggleLed(index) {
            const led = document.getElementById(`led${index}`);
            const button = document.getElementById(`btn${index}`);
            if (led.classList.contains('active')) {
                led.classList.remove('active');
                button.classList.remove('active');
                sendCommand("LED" + index + "_OFF" );
            } else {
                led.classList.add('active');
                button.classList.add('active');
                sendCommand("LED" + index + "_ON ");
            }            
        }

        function lightUpSequentially() {
            for (let i = 0; i < 8; i++) {
                setTimeout(() => {
                    document.getElementById(`led${i}`).classList.add('active');
                    document.getElementById(`btn${i}`).classList.add('active');
                    sendCommand("LED" + i + "_TOG");
                }, i * 300);
            }
        }

        function turnOffSequentially() {
            for (let i = 0; i < 8; i++) {
                setTimeout(() => {
                    document.getElementById(`led${i}`).classList.remove('active');
                    document.getElementById(`btn${i}`).classList.remove('active');
                    sendCommand("LED" + i + "_TOG");
                }, i * 300);  
            }
        }

        function lightUpAll() {
            for (let i = 0; i < 8; i++) {
                document.getElementById(`led${i}`).classList.add('active');
                document.getElementById(`btn${i}`).classList.add('active');        
            }
            sendCommand("ON_ALL  "); 
        }

        function turnOffAll() {
            for (let i = 0; i < 8; i++) {
                document.getElementById(`led${i}`).classList.remove('active');
                document.getElementById(`btn${i}`).classList.remove('active');       
            }
            sendCommand("OFF_ALL ");
        }

        // Khởi tạo WebSocket khi trang tải xong
        window.onload = initWebSocket;
    </script>
</body>
</html>
)rawliteral";

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        String message = String((char*)data);
        Serial.println("Received: " + message.substring(0, 8));
        display.clearDisplay();
        display.setCursor(1, 2);
        display.println("Received:" + message.substring(0, 8));
        display.display();
        
        if (message.substring(5).startsWith("TOG")) {
            int ledIndex = message.substring(3,4).toInt();
            if (ledIndex >= 0 && ledIndex < 8) {
                digitalWrite(ledPins[ledIndex], !digitalRead(ledPins[ledIndex]));
            }
        }
        else if (message.substring(5).startsWith("ON")) {
            int ledIndex = message.substring(3,4).toInt();
            if (ledIndex >= 0 && ledIndex < 8) {
                digitalWrite(ledPins[ledIndex], HIGH);
            }
        }
        else if (message.substring(5).startsWith("OFF")) {
            int ledIndex = message.substring(3,4).toInt();
            if (ledIndex >= 0 && ledIndex < 8) {
                digitalWrite(ledPins[ledIndex], LOW);
            }
        } 
        else if (message.startsWith("ON_ALL")) {
            for (int i = 0; i < 8; i++) {
                digitalWrite(ledPins[i], HIGH);
            }
        } 
        else if (message.startsWith("OFF_ALL")) {
            for (int i = 0; i < 8; i++) {
                digitalWrite(ledPins[i], LOW);
            }
        } 
        else if (message == "SEQUENTIAL_ON") {
            for (int i = 0; i < 8; i++) {
                digitalWrite(ledPins[i], HIGH);
                delay(300);
            }
        } 
        else if (message == "SEQUENTIAL_OFF") {
            for (int i = 0; i < 8; i++) {
                digitalWrite(ledPins[i], LOW);
                delay(300);
            }
        }
    }
}

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_DATA) {
        handleWebSocketMessage(arg, data, len);
    }
}

void setup() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(1, 2);
    display.println("Hello.....");
    display.display();
    delay(2000);

    Serial.begin(9600);
    Serial.print("Connecting to wifi");
    Serial.println(ssid);

    display.clearDisplay();
    display.setCursor(1, 2);
    display.print("Connecting to wifi");
    display.display();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");

        display.print(".");
        display.display();
    }
    Serial.println("\nWiFi connected");
    Serial.println(WiFi.localIP());

    display.clearDisplay();
    display.setCursor(1, 1);
    display.println("WiFi connected.");
    display.println(" ");
    display.print("IP: ");
    display.print(WiFi.localIP());
    display.display();

    ws.onEvent(onWebSocketEvent);
    server.addHandler(&ws);
    server.begin();

    for (int i = 0; i < 8; i++) {
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);
    }
    
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", index_html);
    });
}

void loop() {
    ws.cleanupClients();
}