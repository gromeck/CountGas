/*
**  Wifi prototypes
*/
#ifndef __WIFI_H__
#define __WIFI_H__  1

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include "eeprom.h"

/*
 * default WLAN SSID and PSK
 */
#define WIFI_DEFAULT_SSID "CasaDiLorenzi"
#define WIFI_DEFAULT_PSK  "EasyMoltJukeHire"

/*
 * macros to store the configuration
 */
#define WifiSetSSID(ssid)   EepromWriteString(EEPROM_ADDR_WIFI_SSID,EEPROM_SIZE_WIFI_SSID,(ssid))
#define WifiSetPSK(psk)     EepromWriteString(EEPROM_ADDR_WIFI_PSK,EEPROM_SIZE_WIFI_PSK,(psk))

/*
 * setup wifi
 */
void WifiSetup(void);

/*
 * do Wifi updates
 */
void WifiUpdate(void);

#endif

/**/
