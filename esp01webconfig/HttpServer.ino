#include <ESP8266WebServer.h>
#include <LittleFS.h>

#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf(__VA_ARGS__)
#else
#define DEBUG_MSG(...)
#endif

void httpServerSetup()
{
    // Web服务器初始化
    server.on("/", HTTP_GET, handleRoot);
    server.on("/wifi", HTTP_GET, handleWifi);
    server.on("/wificonfig", HTTP_POST, handleSubmitWifiConfig);
    server.on("/wifiInfo", HTTP_GET, getWifiInfo);
    server.on("/wifiList", HTTP_GET, getWifiList);
    server.on("/test", []()
              { server.send(200, "text/plain", "this works as well"); });
    server.onNotFound(handleRoot);
    server.enableCORS(true);
    // server.enableETag(true);
    server.serveStatic("/", LittleFS, "/");
    server.begin();
    DEBUG_MSG("HTTP server started\n");
}

void handleRoot()
{
    DEBUG_MSG("Redirect...");
    String url = "/index.html";
    server.sendHeader("Location", url, true);
    server.send(302);
}
void handleNotFound()
{
    // digitalWrite(led, 1);
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }

    server.send(404, "text/plain", message);
    // digitalWrite(led, 0);
}
void handleWifi()
{
    DEBUG_MSG("To page:wifi");
    String url = "/wifi.html";
    server.sendHeader("Location", url, true);
    server.send(302);
}
void getWifiInfo()
{
    JSONVar res;
    res["hostName"] = WiFi.hostname();
    res["status"] = WiFi.status();
    if (WiFi.status() == WL_CONNECTED)
    {
        res["ssid"] = WiFi.SSID();
        res["ip"] = WiFi.localIP().toString();
    }
    server.send(200, "text/plain", JSON.stringify(res));
}
void handleSubmitWifiConfig()
{
    if (server.method() == HTTP_POST)
    {
        String ssid = server.arg(String("ssid")), password = server.arg(String("password"));
        DEBUG_MSG("Recived Wifi Config: %s,%s\n", ssid.c_str(), password.c_str());

        // 校验
        // 不测试已连接的WIfi
        if (WiFi.status() == WL_CONNECTED && ssid == WiFi.SSID())
        {
            DEBUG_MSG("已连接此Wifi \n");
            server.send(403, "text/plain", "Connected");
            return;
        }
        // 长度
        if (ssid.length() > 32)
        {
            // fail SSID too long or missing!
            DEBUG_MSG("SSID太长 \n");
            server.send(403, "text/plain", "SSID too long or missing!");
            return;
        }
        if (password.length() > 64)
        {
            // fail passphrase too long!
            DEBUG_MSG("密码太长 \n");
            server.send(403, "text/plain", "passphrase too long!");
            return;
        }
        // wifi重连时, Server不可用, 故需先回复客户端;
        server.send(200, "text/plain", "testing");
        delay(100);

        // 等待Wifi连接, 5秒超时或连接失败后, 后重新连接原存储的wifi;
        // 若成功连接新 WiFi , 则更新存储, 并保持新WiFi连接;
        DEBUG_MSG("连接到新Wifi\n");
        WiFi.begin(ssid, password);
        if (WiFi.waitForConnectResult(10000) == WL_CONNECTED)
        {
            DEBUG_MSG("状态码: %d\n", WiFi.status());
            // saveWifi(ssid, password);
            strcpy(config.wifi.ssid, ssid.c_str());
            strcpy(config.wifi.psw, password.c_str());
            
            EEPROM.put(0, config.wifi);
            EEPROM.commit();
        }
        else
        {
            if (config.wifi.ssid)
            {
                DEBUG_MSG("重连到:%s\n", config.wifi.ssid);
                WiFi.begin(config.wifi.ssid, config.wifi.psw);
            }            
        }
    }
}

void getWifiList()
{
    JSONVar res;
    int number = WiFi.scanComplete();
    DEBUG_MSG("Scan result:%d\n", number);
    if (number == -2)
    {
        // 启动扫描
        // WiFi.disconnect();
        // delay(100);
        WiFi.scanNetworks(true);
        server.send(200, "text/plain", String(number));
    }
    else if (number > 0)
    {
        // 发现的网络列表数组
        res["networks"][number];
        for (uint8_t i = 0; i < number - 1; i++)
        {
            res["networks"][i]["ssid"] = WiFi.SSID(i);
            res["networks"][i]["rssi"] = WiFi.RSSI(i);
            res["networks"][i]["open"] = WiFi.encryptionType(i) == ENC_TYPE_NONE;
            // Serial.printf("%d: %s, (%ddBm)\n", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
        }
        server.send(200, "text/plain", JSON.stringify(res));
        delay(200);
        WiFi.scanDelete();
    }
    else
    {
        server.send(200, "text/plain", String(number));
    }
}