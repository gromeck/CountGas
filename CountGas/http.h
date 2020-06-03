/*
**  http defines & prototypes
*/
#ifndef __HTTP_H__
#define __HTTP_H__ 1

#include "util.h"
#include <ESP8266WebServer.h>

#define HTTP_GET_VAR_WIFI_SSID    "wifi.ssid"
#define HTTP_GET_VAR_WIFI_PSK     "wifi.psk"
#define HTTP_GET_VAR_NTP_SERVER   "ntp.server"
#define HTTP_GET_VAR_COUNTERVAL   "counter.val"
#define HTTP_GET_VAR_COUNTERINC   "counter.inc"

/*
**  setup the HTTP web server:w
*/
void HttpSetup(void);

/*
**	handle incoming HTTP requests
*/
void HttpHandleRequest(void);

#endif

/**/
