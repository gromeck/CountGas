/*
**  http functions
*/
#include "http.h"

/*
**  Initialize the Ethernet server library
**  with the IP address and port you want to use 
*/
EthernetServer _WebServer(80);

/*
**    search for the param var and return the value
**
**    the params string is expected to start and end with a &
*/
static char *HttpGetParam(String params,const char *var)
{
    static char buffer[20];
    int start,end;

#if DBG
    Serial.println(params);
#endif
    if ((start = params.indexOf((String) "&" + var + "=")) >= 0) {
        DbgMsg("HTTP: found %s @ %d",var,start);
        start += strlen(var) + 2;
        int end = params.indexOf("&",start);
        params.substring(start,end).toCharArray(buffer,sizeof(buffer) - 1);
        DbgMsg("HTTP: %s=%s",var,buffer);
        return buffer;
    }
    return NULL;
}

/*
**  handle an incoming HTTP request
*/
static void HttpReceiveRequest(EthernetClient client)
{
  char buffer[20];
  bool allparams = false;

  LogMsg("HTTP: incoming request");
  String currentLine = "";

  while (client.connected()) {
    if (client.available()) {
      char c = client.read();

#if DBG
      Serial.write(c);
#endif
      if (c == '\n') {
        /*
        **  a new lines starts
        */
        if (currentLine.length() == 0) {
          /*
          **  line was empty, so reply to the request
          */
          uint32_t ip = 0;
          String var;
          char buffer[20];
          
          LogMsg("HTTP: sending reply");

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/plain");
          client.println("Connection: close");
          client.println();

          client.println("TIME " + (String) now());

          var = HTTP_GET_VAR_COUNTER;
          var.toUpperCase();
          dtostrf(CounterGetValue(),1,6,buffer);
          client.println(var + (String) " " + (String) buffer);

          if (allparams) {
            /*
             * alle parameters should be returned
             */

            var = HTTP_GET_VAR_INCREMENT;
            var.toUpperCase();
            dtostrf(CounterGetIncrement(),1,6,buffer);
            client.println(var + (String) " " + (String) buffer);

            byte bytes[6];
            IPAddress ntpip;

            var = HTTP_GET_VAR_NTPIP;
            var.toUpperCase();
            EepromRead(EEPROM_ADDR_NTP_IP,EEPROM_SIZE_NTP_IP,bytes);
            ntpip = BytesToIPAddress(bytes);
            client.println(var + " " + (String) IPAddressToString(ntpip));

            var = HTTP_GET_VAR_MACADDR;
            var.toUpperCase();
            EepromRead(EEPROM_ADDR_MACADDR,EEPROM_SIZE_MACADDR,bytes);
            client.println(var + " " + (String) AddressToString(bytes,6,0));
          }

          break;
        }

        /*
        **  check this input line
        */
        if (currentLine.startsWith("GET /")) {
          int params = currentLine.indexOf("?");
          
          if (params >= 0) {
            /*
            **    at least one GET parameter
            */
            int end = currentLine.indexOf("#",params + 1);

            if (end < 0)
                end = currentLine.indexOf(" ",params + 1);
            if (end < 0)
                end = currentLine.length() - params;
            String parameters = "&" + currentLine.substring(params + 1,end) + "&";
            char *val;

            if (val = HttpGetParam(parameters,HTTP_GET_VAR_ALLPARAMS)) {
                allparams = (atoi(val)) ? 1 : 0;
            }
            if (val = HttpGetParam(parameters,HTTP_GET_VAR_MACADDR)) {
                const byte *macaddr = StringToAddress(val,6,0);

                EepromWrite(EEPROM_ADDR_MACADDR,6,macaddr);
                LogMsg("HTTP: set MACADDR=%s",AddressToString(macaddr,6,0));
            }
            if (val = HttpGetParam(parameters,HTTP_GET_VAR_NTPIP)) {
                uint32_t ntpip = *((uint32_t *) StringToAddress(val,4,1));

                EepromWrite(EEPROM_ADDR_NTP_IP,4,(byte *) &ntpip);
                LogMsg("HTTP: set NTPIP=%s",AddressToString((byte *) ((uint32_t *) &ntpip),4,1));
            }
            if (val = HttpGetParam(parameters,HTTP_GET_VAR_COUNTER)) {
                CounterSetValue(atof(val));
                LogMsg("HTTP: set COUNTER: value=%s",dtostrf(CounterGetValue(),1,6,buffer));
            }
            if (val = HttpGetParam(parameters,HTTP_GET_VAR_INCREMENT)) {
                CounterSetIncrement(atof(val));
                LogMsg("HTTP: set INCREMENT: value=%s",dtostrf(CounterGetIncrement(),1,6,buffer));
            }
          }
        }

        /*
        **  reset the line
        */
        currentLine = "";
      } 
      else if (c != '\r') {
        currentLine += c;
      }
    }
  }
  // give the web browser time to receive the data
  delay(1);
  client.stop();
}

/*
**	handle incoming HTTP requests
*/
void HttpHandleRequest(void)
{
    EthernetClient WebClient = _WebServer.available();
    if (WebClient)
        HttpReceiveRequest(WebClient);
}/**/
