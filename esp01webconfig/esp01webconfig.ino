// MDNS相关代码全部被注释了,因为测试没有起效.

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
// #include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <Arduino_JSON.h>
#include <ESP8266HTTPUpdateServer.h>
#include <EEPROM.h>

#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
#define DEBUG_MSG(...)
#endif

#define KEY 0

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

uint32_t keyDuration = 0;
unsigned long keyDownTime;
IRAM_ATTR void keyDown()
{
  DEBUG_MSG("Key down\n");
  detachInterrupt(digitalPinToInterrupt(KEY));
  keyDownTime = millis();
  attachInterrupt(digitalPinToInterrupt(KEY), keyUp, RISING);
}
IRAM_ATTR void keyUp()
{
  DEBUG_MSG("Key up\n");
  keyDuration = millis() - keyDownTime;
  attachInterrupt(digitalPinToInterrupt(KEY), keyDown, FALLING);
}

void setup()
{
  pinMode(KEY, INPUT_PULLUP);

  Serial.begin(115200);
  Serial1.begin(115200);

  EEPROM.begin(512);

  DEBUG_MSG("Booting...\n");

  DEBUG_MSG("Mounting the filesystem...\n");
  if (!LittleFS.begin())
  {
    DEBUG_MSG("Could not mount the filesystem...\n");
    delay(2000);
    ESP.restart();
  }

  // if (MDNS.begin("esp8266"))
  // {
  //   DEBUG_MSG("MDNS responder started\n");
  //   MDNS.addService("http", "tcp", 80);
  // }

  httpUpdater.setup(&server);
  httpServerSetup();

  wifiSetup();
  OTAInit();

  attachInterrupt(digitalPinToInterrupt(KEY), keyDown, FALLING);

  DEBUG_MSG("Ready\n");
}

void loop()
{
  ArduinoOTA.handle();
  server.handleClient();
  // MDNS.update();

  if (keyDuration > 3000)
  {
    keyDuration = 0;
    // 重置操作
  }
}
