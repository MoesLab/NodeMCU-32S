/*********
Moe Azizi, March 2021
NodeMCU-32S a collection of advanced project packaged in one project
include Async Web Server, SD Card functions 
drived from main origin.cpp
button REC, END,    ,and writing to SD was added
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

// Replace with your network credentials

const char *ssid = "My SSID";
const char *password = "My Password";

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

// Init MPU6050
void initMPU()
{
  if (!mpu.begin())
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
}

void initSPIFFS()
{
  if (!SPIFFS.begin())
  {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

void initSDCard()
{
  if (!SD.begin())
  {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC)
  {
    Serial.println("MMC");
  }
  else if (cardType == CARD_SD)
  {
    Serial.println("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    Serial.println("SDHC");
  }
  else
  {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

// Initialize WiFi
void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
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

File file;
String fileMode = "IDLE";

void fileRec(fs::FS &fs, const char *path, const char *message)
{
  if (RECPATH)
  {
    file = fs.open(path, FILE_WRITE);
    if (!file)
    {
      Serial.println("Failed to open file for writing");
      return;
    }
    if (file.print(message))
    {
      Serial.println("File initiated for writing");
      fileMode = "REC";
    }
    else
    {
      Serial.println("Write failed");
    }
    file.close();
    file = fs.open(path, FILE_APPEND);
    if (file)
      Serial.println("File ready for appending logs");
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
    Serial.println("Proccessing file ended!");
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
      Serial.println("Failed to open file for reading");
      return;
    }
    if (file.available())
    {
      Serial.println("File initiated for reading");
      fileMode = "READ";
    }
    else
    {
      Serial.println("Read ended");
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
    Serial.println("fRec was pressed");
    if (fileMode == "IDLE")
    {
      RECPATH = String("/logs/log_") + String(random(1, 999), DEC) + String(".txt");
      Serial.println(RECPATH);
      fileRec(SD, RECPATH.c_str(), "Millis,GyroX,GyroY,GyroZ,AccelX,AccelY,AccelZ,Temp\r\n");
    }
    request->send(200, "text/plain", "OK");
  });

  server.on("/fEnd", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("fEnd was pressed");
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
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  server.on("/aRange", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Accel Range selected");
    request->send(200, "text/plain", "OK");
  });
  server.on("/gRange", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Gyro Range selected");
    request->send(200, "text/plain", "OK");
  });
  server.on("/fRange", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Filter Range selected");
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
}

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
}
