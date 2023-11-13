#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>  //i2c communication SDA SCL
#include <Adafruit_BMP280.h>
#include <AHT10.h>
//#include <ESP32Servo.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <WebServer.h>
#include "mylib.h"
#include <SPI.h>
#include <Ethernet.h>
#include "mylib2.h"

// wifi setup
const char *ssid = "Coffee";
const char *password = "12345678";
IPAddress local_ip(192, 168, 1, 2);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);


// วัดระยะทาง
const int trigPin = 5;
const int echoPin = 18;
//HCSR04 hc(trigPin, echoPin);
// servo
const int servoPin = 23;

const float SOUND_SPEED = 0.034;
const float CM_TO_INCH = 0.393701;

long duration;
float distanceCm;
float distanceInch;

float temperature;  // อุณหภูมิ
float humidity;     // ความชื้น
float pressure;     // ความกดอากาศ

int address = 0;
int count;

AHT10 myAHT20;  // Create an instance of the AHT10 sensor
Adafruit_BMP280 bmp;
HCSR04 hc(trigPin, echoPin);

Servo myservo;

unsigned long text1s = 0;
unsigned long openTime = 0;
const unsigned long finishTime = 3000;  // 3 seconds

unsigned long cTime = 0;
unsigned long pTime = 0;
bool checkText = false;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  wifi_setup();
  //distance_setup();
  AHT20_BMP280_setup();
  myservo.myAttach(servoPin);
  // start with 0 degree
  myservo.angle(0);
}

void loop() {
  server.handleClient();
  distance_output();
  AHT20_BMP280_output();
  if (millis() - text1s >= 1000) {
    Serial.printf("Distance (cm) :  %.2f ,Temperature : %.2f ,humidity : %.2f \n", distanceCm, temperature, humidity);
    //Serial.println(count + "," + distanceCm + "," + temperature + "," + humidity);
    //Serial.printf("%d,%.2f,");

    text1s = millis();
  }
  // มีแก้ววางอยู่ //temperature
  if (distanceCm <= 10 && (temperature >= 37 || humidity >= 88)) {
    myservo.angle(90);
    count = EEPROM.read(address);
    //Serial.println(count);
    // เริ่มจับเวลาทำงาน
    if (openTime == 0) {
      // If the cup is detected within 10 cm, start the timer
      count++;
      EEPROM.put(address, count);
      EEPROM.commit();
      Serial.println("เปิดฝา");
      openTime = millis();
    }

    // ทำงานครบ 3 วิ
    if (millis() - openTime >= finishTime) {

      Serial.println("จับเวลาครบ 3 วินาที");
      Serial.println("ปิดฝา");
      myservo.angle(0);
      openTime = 0;  // Reset the timer
      delay(5000);
    }

  }
  // กรณีไม่มีแก้ววาง
  else {
    // กรณีวางไม่ครบ 3 วิ
    if (openTime != 0) {
      checkText = true;
      // reset
      openTime = 0;
    }
    // ปิด
    if (openTime == 0 && checkText == true) {
      Serial.println("ปิดฝา");
      myservo.angle(0);
      checkText = false;
      delay(2500);
    }
  }

  delay(100);
}

//--------------------------------functions---------------------------



// setup และ data ของ อุณหภูมิ ความชื้น ความกดอากาศ
void AHT20_BMP280_setup() {

  Serial.println(F("AHT20+BMP280 test"));

  while (myAHT20.begin() != true) {
    Serial.println(F("AHT20 not connected or fail to load calibration coefficient"));  //(F()) save string to flash & keeps dynamic memory free
    delay(5000);
  }
  Serial.println(F("AHT20 OK"));

  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1)
      ;
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void AHT20_BMP280_output() {
  // – SDA (Serial Data) คือ สายสัญญาณสำหรับรับ-ส่งข้อมูล
  // – SCL (Serial Clock) คือ สายสัญญาณนาฬิกา ใช้เป็นสำหรับควบคุมการรับ-ส่งข้อมูล
  temperature = myAHT20.readTemperature();
  //Serial.printf("อุณหภูมิ %.2f *C \n", temperature);

  humidity = myAHT20.readHumidity();
  //Serial.printf("ความชื้น %.2f RH \n", humidity);

  pressure = bmp.readPressure();
  //Serial.printf("ความกดอากาศ %.2f hPa \n\n", pressure);
}

// set up และ data ตัววัดระยะทาง
void distance_setup() {
  HCSR04 hc(trigPin, echoPin);
}
void distance_output() {
  distanceCm = hc.dist();
}

// wifi setup
void wifi_setup() {
  // connect to WiFi
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  server.on("/", handle_OnConnect);

  //server.onNotFound(handle_NotFound);
  server.begin();
}
void handle_OnConnect() {
  server.send(200, "text/html", SendHTML());
}
String SendHTML() {
  count = EEPROM.read(address);
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Coffee</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #3498db;}\n";
  ptr += ".button-on:active {background-color: #2980b9;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  // body
  ptr += "<h1>Coffe count</h1>\n";
  ptr += "<h1>" + String(count) + "</h1>\n";

  // end body
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
