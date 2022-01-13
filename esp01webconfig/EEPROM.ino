#include <EEPROM.h>
#define SSID_ADR 0
#define PSW_ADR 32
void clearEEPROM(int start, int end)
{
  for (int i = start; i < end; i++)
  {
    EEPROM.write(i, 0x00);
  }
}
void storeWifi(String ssid, String password)
{
  clearEEPROM(SSID_ADR, PSW_ADR + 64);
  for (int j = 0; j < ssid.length(); j++)
  {
    EEPROM.write(SSID_ADR + j, ssid[j]);
  }
  for (int j = 0; j < password.length(); j++)
  {
    EEPROM.write(PSW_ADR + j, password[j]);
  }
  EEPROM.commit();
  DEBUG_MSG("已存储当前Wifi配置\n");
}
String getStoreSSID()
{
  String ssid;
  for (int i = SSID_ADR; i < SSID_ADR + 32; i++)
  {
    ssid += (char)EEPROM.read(i);
  }
  return ssid;
}
String getStorePSW()
{
  String psw;
  for (int i = PSW_ADR; i < PSW_ADR + 64; i++)
  {
    psw += (char)EEPROM.read(i);
  }
  return psw;
}