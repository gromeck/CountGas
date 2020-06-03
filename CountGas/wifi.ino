/*
**  Wifi
*/
#include "wifi.h"

static String _wifi_ssid;
static String _wifi_psk;

/*
 * setup wifi
 */
void WifiSetup(void)
{
    /*
     * get the wifi SSID and PSK from the configuration
     */
    _wifi_ssid = EepromReadString(EEPROM_ADDR_WIFI_SSID,EEPROM_SIZE_WIFI_SSID);
    if (_wifi_ssid == "")
      _wifi_ssid = WIFI_DEFAULT_SSID;
      
    _wifi_psk = EepromReadString(EEPROM_ADDR_WIFI_PSK,EEPROM_SIZE_WIFI_PSK);
    if (_wifi_psk == "")
      _wifi_psk = WIFI_DEFAULT_PSK;

dump("WIFI SSID",(byte *) _wifi_ssid.c_str(),_wifi_ssid.length());

    /*
     * init the WIFI
     */
    WiFi.mode(WIFI_STA);
    WiFi.begin(_wifi_ssid,_wifi_psk);
    LogMsg("WLAN: waiting to connect to %s ...",_wifi_ssid.c_str());

    /*
     * wait to connect
     */
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }

    /*
     * up an running
     */
    IPAddress ip = WiFi.localIP();
    LogMsg("WLAN: connected to %s with local IP address %s",_wifi_ssid.c_str(),IPAddressToString(ip));

}

/*
 * do Wifi updates
 */
void WifiUpdate(void)
{
  MDNS.update();
}

/**/
