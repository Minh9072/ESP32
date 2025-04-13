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
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int led1 = 26;
const int led2 = 25;
const int alarmPin = 23;
 
const char* ssid = "Bao Tu Trong Trung_2.4G";
const char* password = "@2022.com";
//Tạo một web server tại cổng 80 - cổng mặc định cho esp32 webserver
WiFiServer webServer(80);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 25200, 60000);

char Time[] = "00:00:00";
char Date[] = "00/00/2000";
byte lastSecond_, second_, minute_, hour_, day_, month_;
int year_;
 
String led1Status = "OFF";
String led2Status = "OFF";
 
String header;
 
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
 
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

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(alarmPin, OUTPUT);
 
  Serial.print("Connecting to wifi");
  Serial.println(ssid);

  display.clearDisplay();
  display.setCursor(1, 2);
  display.print("Connecting to wifi");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  delay(2000);
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(1, 1);
  display.println("WiFi connected.");
  display.println(" ");
  display.print("IP: ");
  display.print(WiFi.localIP());
  display.display();
   
  //khởi tạo webserver
  webServer.begin();
}
 
void loop() {
  //Hiển thị thời gian
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
    display.setCursor(1, 2);
    display.println("WiFi connected.");
    display.println(" ");
    display.print("IP: ");
    display.print(WiFi.localIP());
  
    display.setCursor(0, 35);
    display.setTextSize(1);
    display.print("Time: ");
    display.println(Time);
    display.println(" ");
    display.print("Date: ");
    display.println(Date);
    display.display();
  }

  WiFiClient webClient = webServer.available(); // nếu có client kết nối với esp32 webserver
  if(webClient)
  {
    //khoi tao gia tri ban dau cho time
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New web Client");
    //biến lưu giá trị response
    String currentLine = "";
    //nếu có client connect và không quá thời gian time out
    while(webClient.connected() && currentTime - previousTime <= timeoutTime)
    {
      //đọc giá trị timer tại thời điểm hiện tại
      currentTime = millis();
      //nếu client còn kết nối
      if(webClient.available())
      {
        //đọc giá trị truyền từ client theo từng byte kiểu char
        char c = webClient.read();
        Serial.write(c);
        header += c; // lưu giá trị vào Header
        if(c == '\n') //Nếu đọc được kí tự xuống dòng (hết chuỗi truyền tới)
        {
          if (currentLine.length() == 0) 
          {
            //esp32 webserver response
            // HTTP headers luôn luôn bắt đầu với code HTTP (ví d HTTP/1.1 200 OK)
            webClient.println("HTTP/1.1 200 OK");
            webClient.println("Content-type:text/html"); // sau đó là kiểu nội dụng mà client gửi tới, ví dụ này là html
            webClient.println("Connection: close"); // kiểu kết nối ở đây là close. Nghĩa là không giữ kết nối sau khi nhận bản tin
            webClient.println();
 
            // nếu trong file header có giá trị
            if (header.indexOf("GET /led1/on") >= 0) 
            {
              Serial.println("Led1 on");
              led1Status = "on";
              digitalWrite(led1, HIGH);
              digitalWrite(alarmPin, HIGH);
            } 
            else if (header.indexOf("GET /led1/off") >= 0) 
            {
              Serial.println("Led1 off");
              led1Status = "off";
              digitalWrite(led1, LOW);
              digitalWrite(alarmPin, LOW);
            } 
            else if (header.indexOf("GET /led2/on") >= 0) 
            {
              Serial.println("Led2 on");
              led2Status = "on";
              digitalWrite(led2, HIGH);
              digitalWrite(alarmPin, HIGH);
            } 
            else if (header.indexOf("GET /led2/off") >= 0) 
            {
              Serial.println("Led2 off");
              led2Status = "off";
              digitalWrite(led2, LOW);
              digitalWrite(alarmPin, LOW);
            }
            // Response trang HTML của esp32 webserver
            webClient.println("<!DOCTYPE html><html>");
            webClient.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            //thêm font-awesome 
            webClient.println("<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css\">");
            // code CSS cho web
            //css cho toan bo trang 
              webClient.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
              //css cho nut nhan
              webClient.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
              webClient.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
              webClient.println(".button2 {background-color: #5e0101;}</style></head>");
             
            // Web Page Heading H1 with CSS
            webClient.println("<body><h1 style=\"color:Tomato;\">ESP32 Station Web Server</h1>");
 
            // Web Page Heading H2
            webClient.println("<h2 style=\"color:#077a39;\"><a href=\"https://www.facebook.com/minh.truong.83731\">FB: Minh Truong</a></h2>");
            webClient.println("<i class=\"fa fa-home\" aria-hidden=\"true\"></i>");
 
            // Display current state, and ON/OFF buttons for Led1
            webClient.println("<p>Led1 - State " + led1Status + "</p>");
            // If the Led1Status is off, it displays the ON button       
            if (led1Status=="off") 
            {
              //khởi tạo một nút nhấn có đường dẫn đích là /led1/on
              webClient.println("<p><a href=\"/led1/on\"><button class=\"button\">ON</button></a></p>");
            } 
            else
            {
              //khởi tạo một nút nhấn có đường dẫn đích là /led1/off
              webClient.println("<p><a href=\"/led1/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
                
            // Display current state, and ON/OFF buttons for Led2
            webClient.println("<p>Led2 - State " + led2Status + "</p>");
            // If the led2 is off, it displays the ON button       
            if (led2Status=="off") 
            {
              //khởi tạo một nút nhấn có đường dẫn đích là /led2/on
              webClient.println("<p><a href=\"/led2/on\"><button class=\"button\">ON</button></a></p>");
            } 
            else
            {
              //khởi tạo một nút nhấn có đường dẫn đích là /led2/on
              webClient.println("<p><a href=\"/led2/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            webClient.println("</body></html>");
             
            // The HTTP response ends with another blank line
            webClient.println();
            // Break out of the while loop
            break;
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')   //nếu giá trị gửi tới khác xuống duòng
        {
          currentLine += c;     //lưu giá trị vào biến
        }
      }
    }
    // Xoá header để sử dụng cho lần tới
    header = "";
    // ngắt kết nối với client trên esp32 webserver
    webClient.stop();
    Serial.println("Client disconnected.");
    Serial.println(""); 
  }
}