#include <ArduinoOTA.h>

void OTAInit() {
  // OTA 相关程序
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("123");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    // TRACE("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    DEBUG_MSG("End\n");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    DEBUG_MSG("Progress: %d%% \n",progress / (total / 100));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    DEBUG_MSG("Error: %d",error);
    // DEBUG_MSG(error);
    if (error == OTA_AUTH_ERROR) {
      DEBUG_MSG("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      DEBUG_MSG("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      DEBUG_MSG("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      DEBUG_MSG("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      DEBUG_MSG("End Failed");
    }
  });
  ArduinoOTA.begin();
}
