/*********
Moe Azizi, March 2021

This main.cpp will work in coordination with the GUI folder to interact with the circuit board
implemented under skate boot to measure acceleration, but the main idea is to monitor the HW and see if 
updating browser and saving data on an SD card will carry on without any glich
Data recorded on SD card will be analyzed to improve athlete performance

NodeMCU-32S a collection of advanced project packaged in one project
include Async Web Server, SD Card functions 
drived from main origin.cpp
fix needed for 
1: FILENAME ->name() to work

*********/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Arduino_JSON.h>

#include "SPIFFS.h"
#include "SD.h"
#include "FS.h"
#include "SPI.h"

bool PRINTLOG = false;              //true for print logs

// Replace with your network credentials
//const char *ssid = "Mehrdad";
//const char *password = "abcdefg";
// mobile hot spot
const char *ssid = "MoesS9";
const char *password = "4168294000";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

// Json Variable to Hold Sensor Readings
JSONVar readings;

// Timer variables
unsigned long lastTime = 0;
unsigned long lastTimeTemperature = 0;
unsigned long lastTimeAcc = 0;
unsigned long gyroDelay = 10;
unsigned long temperatureDelay = 1000;
unsigned long accelerometerDelay = 200;

// Create a sensor object
Adafruit_MPU6050 mpu;

sensors_event_t a, g, temp;

float gyroX, gyroY, gyroZ;
float accX, accY, accZ;
float temperature;

//Gyroscope sensor deviation
float gyroXerror = 0.07;
float gyroYerror = 0.03;
float gyroZerror = 0.01;

void SerialLog(const char *msg)
{
 if(PRINTLOG)
  Serial.print(msg);
}
void SerialLogln(const char *msg)
{
 if(PRINTLOG)
  Serial.println(msg);
}

// Init MPU6050
void initMPU()
{
  if (!mpu.begin())
  {
    SerialLogln("Failed to find MPU6050 chip");
    while (1)
    {
      delay(10);
    }
  }
  SerialLogln("MPU6050 Found!");

}

void initSPIFFS()
{
  if (!SPIFFS.begin())
  {
    SerialLogln("An error has occurred while mounting SPIFFS");
  }
  SerialLogln("SPIFFS mounted successfully");
}

void initSDCard()
{
  if (!SD.begin())
  {
    SerialLogln("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    SerialLogln("No SD card attached");
    return;
  }

  SerialLog("SD Card Type: ");
  if (cardType == CARD_MMC)
  {
    SerialLogln("MMC");
  }
  else if (cardType == CARD_SD)
  {
    SerialLogln("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    SerialLogln("SDHC");
  }
  else
  {
    SerialLogln("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);  
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

// Initialize WiFi STA
void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  SerialLogln("");
  SerialLog("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    SerialLog(".");
    delay(1000);
  }
  SerialLogln("");
  Serial.println(WiFi.localIP());
}
String getGyro()
{
  mpu.getEvent(&a, &g, &temp);

  float gyroX_temp = g.gyro.x;
  if (abs(gyroX_temp) > gyroXerror)
  {
    gyroX += gyroX_temp / 50.00;
  }

  float gyroY_temp = g.gyro.y;
  if (abs(gyroY_temp) > gyroYerror)
  {
    gyroY += gyroY_temp / 70.00;
  }

  float gyroZ_temp = g.gyro.z;
  if (abs(gyroZ_temp) > gyroZerror)
  {
    gyroZ += gyroZ_temp / 90.00;
  }
  return String(gyroX) + String(",") + String(gyroY) + String(",") + String(gyroZ);
}
/*
// Initialize WiFi AP
void initWiFi_AP()
{
  WiFi.softAP(ssid_ap, password_ap);
 
  SerialLogln();
  SerialLog("IP address: ");
  SerialLogln(WiFi.softAPIP());
}
*/
void PrintAccelRange()
{
   switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    SerialLogln("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    SerialLogln("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    SerialLogln("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    SerialLogln("+-16G");
    break;
  }
}
void setAccelRange(int Sa)
{
  switch(Sa){
    case 2:  mpu.setAccelerometerRange(MPU6050_RANGE_2_G); break;
    case 4:  mpu.setAccelerometerRange(MPU6050_RANGE_4_G); break;
    case 8:  mpu.setAccelerometerRange(MPU6050_RANGE_8_G); break;
    case 16: mpu.setAccelerometerRange(MPU6050_RANGE_16_G); break;
    default: SerialLogln("not recognized");
  }
  delay(300);
  PrintAccelRange();
}

void PrintGyroRange()
{
switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    SerialLogln("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    SerialLogln("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    SerialLogln("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    SerialLogln("+- 2000 deg/s");
    break;
  }
}
void setGyroRange(int Sg)
{
  switch(Sg){
    case 250:   mpu.setGyroRange(MPU6050_RANGE_250_DEG); break;
    case 500:   mpu.setGyroRange(MPU6050_RANGE_500_DEG); break;
    case 1000:  mpu.setGyroRange(MPU6050_RANGE_1000_DEG); break;
    case 2000:  mpu.setGyroRange(MPU6050_RANGE_2000_DEG); break;
  }
  delay(300);
  PrintGyroRange();
}

void PrintFilterRange()
{
 switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    SerialLogln("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    SerialLogln("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    SerialLogln("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    SerialLogln("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    SerialLogln("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    SerialLogln("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    SerialLogln("5 Hz");
    break;
  }
}
void setFilterRange(int Sf)
{
  switch(Sf){
    case 260:   mpu.setFilterBandwidth(MPU6050_BAND_260_HZ); break;
    case 184:   mpu.setFilterBandwidth(MPU6050_BAND_184_HZ); break;
    case 94:    mpu.setFilterBandwidth(MPU6050_BAND_94_HZ); break;
    case 44:    mpu.setFilterBandwidth(MPU6050_BAND_44_HZ); break;
    case 21:    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); break;
    case 10:    mpu.setFilterBandwidth(MPU6050_BAND_10_HZ); break; 
    case 5:     mpu.setFilterBandwidth(MPU6050_BAND_5_HZ); break;
  }
  delay(300);
  PrintFilterRange();
}

String getGyroReadings()
{
  mpu.getEvent(&a, &g, &temp);

  float gyroX_temp = g.gyro.x;
  if (abs(gyroX_temp) > gyroXerror)
  {
    gyroX += gyroX_temp / 50.00;
  }

  float gyroY_temp = g.gyro.y;
  if (abs(gyroY_temp) > gyroYerror)
  {
    gyroY += gyroY_temp / 70.00;
  }

  float gyroZ_temp = g.gyro.z;
  if (abs(gyroZ_temp) > gyroZerror)
  {
    gyroZ += gyroZ_temp / 90.00;
  }

  readings["gyroX"] = String(gyroX);
  readings["gyroY"] = String(gyroY);
  readings["gyroZ"] = String(gyroZ);

  String jsonString = JSON.stringify(readings);
  return jsonString;
}

String getAccel()
{
  mpu.getEvent(&a, &g, &temp);
  // Get current acceleration values
  accX = a.acceleration.x;
  accY = a.acceleration.y;
  accZ = a.acceleration.z;
  return String(accX) + String(",") + String(accY) + String(",") + String(accZ);
}

String getAccReadings()
{
  mpu.getEvent(&a, &g, &temp);
  // Get current acceleration values
  accX = a.acceleration.x;
  accY = a.acceleration.y;
  accZ = a.acceleration.z;
  readings["accX"] = String(accX);
  readings["accY"] = String(accY);
  readings["accZ"] = String(accZ);
  String accString = JSON.stringify(readings);
  return accString;
}

String getTemperature()
{
  mpu.getEvent(&a, &g, &temp);
  temperature = temp.temperature;
  return String(temperature);
}

String getBatteryVoltage()
{
  return String("4.22");
}
//--------------------------------------File functions
String RECPATH;
String READPATH;
bool NEWFILE = false;

File file;
String fileMode = "IDLE";

void fileRec(fs::FS &fs, const char *path, const char *message)
{
  if (RECPATH)
  {
    file = fs.open(path, FILE_WRITE);
    if (!file)
    {
      SerialLogln("Failed to open file for writing");
      return;
    }
    if (file.print(message))
    {
      SerialLogln("File initiated for writing");
      fileMode = "REC";
      NEWFILE = true;
    }
    else
    {
      SerialLogln("Write failed");
    }
    file.close();
    file = fs.open(path, FILE_APPEND);
    if (file)
      SerialLogln("File ready for appending logs");
  }
}
void fileAppend(const char *message)
{
  file.print(message);
}

void fileEnd(fs::FS &fs, const char *path)
{
  if (file)
  {
    file.close();
    SerialLogln("Proccessing file ended!");
    fileMode = "IDLE";
  }
}
void fileOpen(fs::FS &fs, const char *path)
{
  if (READPATH)
  {
    file = fs.open(path);
    if (!file)
    {
      SerialLogln("Failed to open file for reading");
      return;
    }
    if (file.available())
    {
      SerialLogln("File initiated for reading");
      fileMode = "READ";
    }
    else
    {
      SerialLogln("Read ended");
      fileEnd(SD, READPATH.c_str());
    }
  }
}
void fileRead(fs::FS &fs, const char *path)
{
}

//----------------------------------------------------
void setup()
{
  Serial.begin(115200);
  initWiFi();
  //initWiFi_AP();
  initSDCard();
  //initSPIFFS();
  initMPU();

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    //request->send(SPIFFS, "/index.html", "text/html");
    request->send(SD, "/index.html", "text/html");
  });

  //server.serveStatic("/", SPIFFS, "/");
  server.serveStatic("/", SD, "/");

  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
    gyroX = 0;
    gyroY = 0;
    gyroZ = 0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/resetX", HTTP_GET, [](AsyncWebServerRequest *request) {
    gyroX = 0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/resetY", HTTP_GET, [](AsyncWebServerRequest *request) {
    gyroY = 0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/resetZ", HTTP_GET, [](AsyncWebServerRequest *request) {
    gyroZ = 0;
    request->send(200, "text/plain", "OK");
  });
  //////////////////////////////////////////////////////////////////////////////

  server.on("/fRec", HTTP_GET, [](AsyncWebServerRequest *request) {
    SerialLogln("fRec was pressed");
    if (fileMode == "IDLE")
    {
      RECPATH = String("/logs/log_") + String(random(1, 9999), DEC) + String(".txt");
      SerialLogln(RECPATH.c_str());
      fileRec(SD, RECPATH.c_str(), "Millis,GyroX,GyroY,GyroZ,AccelX,AccelY,AccelZ,Temp\r\n");
    }
    request->send(200, "text/plain", "OK");
  });

  server.on("/fEnd", HTTP_GET, [](AsyncWebServerRequest *request) {
    SerialLogln("fEnd was pressed");
    if (fileMode == "REC")
    {
      fileEnd(SD, RECPATH.c_str());
      RECPATH = "";
    }
    else if (fileMode == "READ")
    {
      fileEnd(SD, READPATH.c_str());
      READPATH = "";
    }

    request->send(200, "text/plain", "OK");
  });

  server.on("/fRead", HTTP_GET, [](AsyncWebServerRequest *request) {
    // fileOpen(SD,READPATH.c_str());
    request->send(200, "text/plain", "OK");
  });

  server.on("/filelist", HTTP_GET, [](AsyncWebServerRequest *request) {
      if (request->hasParam("FILENAME")){
      SerialLog("filelist has FILENAME=");
      String message = request->getParam("FILENAME")->name();
      SerialLogln(message.c_str());
    }
    else { SerialLogln("filelist has no FILENAME"); }
    request->send(200, "text/plain", "OK");
  });
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  server.on("/aRange", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("PARAM1")){
      SerialLog("aRange has PARAM1=");
      String message = request->getParam("PARAM1")->value();
      SerialLogln(message.c_str());
      setAccelRange(message.toInt());
    }
    else { SerialLogln("aRange has no PARAM1"); }
    request->send(200, "text/plain", "OK");
  });
  server.on("/gRange", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("PARAM1")){
      SerialLog("gRange has PARAM1=");
      String message = request->getParam("PARAM1")->value();
      SerialLogln(message.c_str());
      setGyroRange(message.toInt());
    }
    else { SerialLogln("gRange has no PARAM1"); }
    request->send(200, "text/plain", "OK");
  });
  server.on("/fRange", HTTP_GET, [](AsyncWebServerRequest *request) {
      if (request->hasParam("PARAM1")){
      SerialLog("fRange has PARAM1=");
      String message = request->getParam("PARAM1")->value();
      SerialLogln(message.c_str());
      setFilterRange(message.toInt());
    }
    else { SerialLogln("fRange has no PARAM1"); }
    request->send(200, "text/plain", "OK");
  });

  /////////////////////////////////////////////////////////////////////////////
  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client) {
    if (client->lastId())
    {
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);

  server.begin();

  PrintAccelRange();
  PrintGyroRange();
  PrintFilterRange();

} // end of setup

void loop()
{
  if ((millis() - lastTime) > gyroDelay)
  {
    if (fileMode == "REC")
    {
      String Str;
      Str += String(millis());
      Str += ",";
      Str += getGyro();
      Str += ",";
      Str += getAccel();
      Str += ",";
      Str += getTemperature();
      Str += "\r\n";
      fileAppend(Str.c_str());
      events.send(getGyroReadings().c_str(), "gyro_readings", millis());
    }
    else if (fileMode == "READ")
    {
      // fileRead(SD,READPATH.c_str());
      // events.send(getGyroReadings().c_str(), "gyro_readings", millis());
    }
    else // fileMode == "IDLE"
    {
      // Send Events to the Web Server with the Sensor Readings
      events.send(getGyroReadings().c_str(), "gyro_readings", millis());
    }
    lastTime = millis();
  }
  if ((millis() - lastTimeAcc) > accelerometerDelay)
  {
    // Send Events to the Web Server with the Sensor Readings
    events.send(getAccReadings().c_str(), "accelerometer_readings", millis());
    lastTimeAcc = millis();
  }
  if ((millis() - lastTimeTemperature) > temperatureDelay)
  {
    // Send Events to the Web Server with the Sensor Readings
    events.send(getTemperature().c_str(), "temperature_reading", millis());
    lastTimeTemperature = millis();
  }
  if (NEWFILE)
  {
    events.send(RECPATH.c_str(), "filelist_reading", millis());
    NEWFILE = false;
  }
}
