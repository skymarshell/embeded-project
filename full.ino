// จารบอกว่าให้เขียน libary ของ distance sensor
// แต่ distance sensor ไม่ได้ใช้ libary (ใช้ pulseIn function พื้นฐานของ arduino จากบรรทัด 140)
// https://forum.arduino.cc/t/pulsein-function-source-code/102995 --> code pulseIn ลองไปดุู
// https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/wiring_pulse.c
// https://forum.arduino.cc/t/pulsein-function-source-code/102995
// เขียน comment ด้วยตอนทำ
//
//
//

#include <Wire.h>  //i2c communication SDA SCL
#include <Adafruit_BMP280.h>
#include <AHT10.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <WebServer.h>

//wifi setup
const char* ssid = "Coffee";
const char* password = "12345678";
IPAddress local_ip(192, 168, 1, 2);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);
//วัดระยะทาง
const int trigPin = 5;
const int echoPin = 18;
//servo
const int servoPin = 23;

const float SOUND_SPEED = 0.034;
const float CM_TO_INCH = 0.393701;

long duration;
float distanceCm;
float distanceInch;

float temperature;  //อุณหภูมิ
float humidity;     //ความชื้น
float pressure;     //ความกดอากาศ


int address = 0;
int count;
 
AHT10 myAHT20;  // Create an instance of the AHT10 sensor
Adafruit_BMP280 bmp;

Servo myservo;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  wifi_setup();
  distance_setup();
  AHT20_BMP280_setup();
  myservo.attach(servoPin);
  //start with 0 degree
  myservo.write(0);
}

void loop() {
  server.handleClient();
  distance_output();
  AHT20_BMP280_output();

  //เอาไว้ทดสอบเฉยๆ เวลาใช้จริงต้องเช็ตระยะและอุณหภูมิด้วย
  if (distanceCm <= 10) {
    Serial.println("Go 180");
    myservo.write(180);
    count = EEPROM.read(address);
    count++;
    EEPROM.put(address, count);
    EEPROM.commit();
    delay(3000);
  } else {
    myservo.write(0);
  }


  delay(1000);
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
  Serial.printf("อุณหภูมิ %.2f *C \n", temperature);

  humidity = myAHT20.readHumidity();
  Serial.printf("ความชื้น %.2f RH \n", humidity);

  pressure = bmp.readPressure();
  Serial.printf("ความกดอากาศ %.2f hPa \n\n", pressure);
}

//set up และ data ตัววัดระยะทาง
void distance_setup() {
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);   // Sets the echoPin as an Input
}

void distance_output() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distanceCm = duration * SOUND_SPEED / 2;

  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;

  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);
}

//wifi setup
void wifi_setup() {
  //connect to WiFi
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  server.on("/", handle_OnConnect);

  server.begin();
  //server.on("/", handle_OnConnect);
  //server.onNotFound(handle_NotFound);
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
  //body
  ptr += "<h1>Coffe count</h1>\n";
  ptr += "<h1>" + String(count) + "</h1>\n";


  //end body
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
