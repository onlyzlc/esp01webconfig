#include <EEPROM.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void wifiSetup()
{
    // WiFi.setAutoConnect(true);
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP("ESP");
    DEBUG_MSG("Setting AP...\n");

    WiFi.mode(WIFI_AP_STA);

    DEBUG_MSG("Soft-AP IP address = %s", WiFi.softAPIP());

    // 连接上一次的WiFi
    WiFi.begin();
    wifiStatus = WiFi.waitForConnectResult(5000);
}