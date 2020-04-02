/*
**	util functions
*/
#include "util.h"

/*
**    generic conversion
*/
char *AddressToString(byte *addr,int addrlen,int dec)
{
    static char str[25];
    int len = 0;

    for (int n = 0;n < addrlen;n++) {
        len += sprintf(&str[len],(dec) ? "%d" : "%02x",addr[n]);
        str[len++] = (dec) ? '.' : ':';
    }
    str[len - 1] = '\0';

    return str;
}

byte *StringToAddress(char *str,int addrlen,int dec)
{
    static byte addr[10];
    char *end;
    int len = 0;

    for (int n = 0;n < addrlen;n++) {
        addr[n] = strtoul(str,&end,(dec) ? 10 : 16);
        if (end)
            str = end + 1;
    }
    return addr;
}

/*
**  log a smessage to serial
*/
void LogMsg(const char *fmt,...)
{
    va_list args;
    time_t t = now();
    char timestamp[20];
    char msg[128];
 
    sprintf(timestamp,"%02d.%02d.%04d %02d:%02d:%02d",
        day(t),month(t),year(t),hour(t),minute(t),second(t));
  
    va_start(args,fmt);
    vsnprintf(msg,128,fmt,args);
    va_end(args);
  
    if (Serial) {
        Serial.print(timestamp);
        Serial.print(": ");
        Serial.println(msg);
        Serial.flush();
    }
}/**/

