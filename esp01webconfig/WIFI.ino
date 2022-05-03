#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void wifiSetup()
{
    WiFi.setAutoConnect(true);
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.mode(WIFI_AP_STA);
    delay(100);
    WiFi.softAP(WiFi.hostname());
    if (config.wifi.ssid)
    {
        DEBUG_MSG("连接网络:%s\n", config.wifi.ssid);
        WiFi.begin(config.wifi.ssid, config.wifi.psw);
    }
    else
        WiFi.begin();
    DEBUG_MSG("WiFi 连接...");
    switch (WiFi.waitForConnectResult(5000))
    {
    case -1:
        DEBUG_MSG("超时");
        break;
    case WL_CONNECTED:
        DEBUG_MSG("成功");
        break;
    case WL_CONNECT_FAILED:
        DEBUG_MSG("失败");
        break;
    case WL_CONNECTION_LOST:
        DEBUG_MSG("丢失");
        break;
    case WL_WRONG_PASSWORD:
        DEBUG_MSG("密码错误");
        break;
    case WL_DISCONNECTED:
        DEBUG_MSG("断开");
        break;
    default:
        DEBUG_MSG("%d",WiFi.status());
        break;
    }
    DEBUG_MSG("\n");
}