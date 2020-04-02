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
          time_t t = now();
          char timestamp[20];
          
          LogMsg("HTTP: sending reply");
          sprintf(timestamp,"%02d.%02d.%04d %02d:%02d:%02d",
            day(t),month(t),year(t),hour(t),minute(t),second(t));

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          client.println("TIME " + (String) timestamp);
          client.print("COUNTER ");
	  client.println(CounterGetValue(),DEC);
          client.print("INCREMENT ");
	  client.println(CounterGetIncrement(),DEC);
#if TMP
          TempDump(client);
#endif
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

            if (val = HttpGetParam(parameters,HTTP_GET_VAR_COUNTERVAL)) {
                CounterSetValue(atol(val));
                LogMsg("HTTP: COUNTER VALUE=%lu",CounterGetValue());
            }
            if (val = HttpGetParam(parameters,HTTP_GET_VAR_COUNTERINC)) {
                CounterSetIncrement(atol(val));
                LogMsg("HTTP: COUNTER INCREMENT=%lu",CounterGetIncrement());
            }
            if (val = HttpGetParam(parameters,HTTP_GET_VAR_MACADDR)) {
                byte *macaddr = StringToAddress(val,6,0);

                EepromWrite(EEPROM_ADDR_MACADDR,6,macaddr);
                LogMsg("HTTP: MACADDR=%s",AddressToString(macaddr,6,0));
            }
            if (val = HttpGetParam(parameters,HTTP_GET_VAR_NTPIP)) {
                uint32_t ntpip = *((uint32_t *) StringToAddress(val,4,1));

                EepromWrite(EEPROM_ADDR_NTPIP,4,(byte *) &ntpip);
                LogMsg("HTTP: NTPIP=%s",AddressToString((byte *) ((uint32_t *) &ntpip),4,1));
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

