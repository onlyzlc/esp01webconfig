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

#define KEY D8

struct Config
{
  struct Wifi
  {
    char ssid[32];
    char psw[64];
  }wifi;
}config;

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

void setup()
{
  // 启动指示
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }
  Serial.begin(115200);

  EEPROM.begin(512);

  DEBUG_MSG("Booting...\n");

  EEPROM.get(0, config);

  DEBUG_MSG("ssid:%s\n", config.wifi.ssid);
  DEBUG_MSG("psw:%s\n", config.wifi.psw);

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

  DEBUG_MSG("Ready\n");
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
  ArduinoOTA.handle();
  server.handleClient();
  // MDNS.update();
}
