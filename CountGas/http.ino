/*
**  http functions
*/
#include "http.h"
#include "wifi.h"

/*
**  Initialize the Ethernet server library
**  with the IP address and port you want to use 
*/
static ESP8266WebServer _WebServer(80);

/*
 * setup the webserver
 * 
 */
void HttpSetup(void)
{
  _WebServer.on("/", []() {
    if (_WebServer.hasArg(HTTP_GET_VAR_WIFI_SSID)) {
      /*
       * set new Wifi SSID
       */
       WifiSetSSID(_WebServer.arg(HTTP_GET_VAR_WIFI_SSID));
    }
    if (_WebServer.hasArg(HTTP_GET_VAR_WIFI_PSK)) {
      /*
       * set new Wifi SSID
       */
       WifiSetPSK(_WebServer.arg(HTTP_GET_VAR_WIFI_PSK));
    }
    if (_WebServer.hasArg(HTTP_GET_VAR_NTP_SERVER)) {
      /*
       * set new Wifi SSID
       */
       NtpSetServer(_WebServer.arg(HTTP_GET_VAR_NTP_SERVER));
    }
    if (_WebServer.hasArg(HTTP_GET_VAR_COUNTERVAL)) {
      /*
       * set new counter val
       */
       CounterSetValue(_WebServer.arg(HTTP_GET_VAR_COUNTERVAL).toFloat());
    }
    if (_WebServer.hasArg(HTTP_GET_VAR_COUNTERINC)) {
      /*
       * set new counter increment
       */
       CounterSetIncrement(_WebServer.arg(HTTP_GET_VAR_COUNTERINC).toFloat());
    }

    /*
     * reply with the current status
     */
    String message = "";
    char buffer[20];
    
    message += "TIME " + (String) now() + "\n";
    
    dtostrf(CounterGetValue(),1,6,buffer);
    message += "COUNTER " + (String) buffer + "\n";

    dtostrf(CounterGetIncrement(),1,6,buffer);
    message += "INCREMENT " + (String) buffer + "\n";

    _WebServer.send(200,"text/plain",message);
  });

  _WebServer.begin();
  LogMsg("HTTP: server started");
}

/*
**	handle incoming HTTP requests
*/
void HttpHandleRequest(void)
{
  _WebServer.handleClient();
}/**/
