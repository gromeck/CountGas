/*
**	util functions
*/
#include "util.h"

/*
 * convert an IP address to a field of bytes
 */
const byte *IPAddressToBytes(IPAddress &addr)
{
    static uint8_t bytes[4];

    for (int n;n < 4;n++)
      bytes[n] = addr[n];

    return bytes;
}

/*
 * convert a field of bytes to an IP address
 */
IPAddress BytesToIPAddress(uint8_t *bytes)
{
    IPAddress addr(0,0,0,0);

    for (int n;n < 4;n++)
      addr[n] = bytes[n];

    return addr;
}

/*
**    generic conversion
*/
const char *AddressToString(byte *addr,int addrlen,int dec)
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

const byte *StringToAddress(const char *str,int addrlen,int dec)
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
 * dump data from an address
 */
void dump(String title,const byte *addr,const int len)
{
#define BYTES_PER_ROW 16

    for (int row = 0;row * BYTES_PER_ROW < len;row++) {
      String dump_offset = "";
      String dump_hex = "";
      String dump_ascii = "";
      char tmp[10];
      
      sprintf(tmp,"%04x:",row * BYTES_PER_ROW);
      dump_offset = tmp;
      for (int col = 0;col < BYTES_PER_ROW && row * BYTES_PER_ROW + col < len;col++) {
        if (col > 0 && col % (BYTES_PER_ROW / 2) == 0) {
          dump_hex += " ";
          dump_ascii += " ";
        }
        byte x = addr[row * BYTES_PER_ROW + col];
        sprintf(tmp," %02x",x);
        dump_hex += tmp;
        sprintf(tmp,"%c",(isprint(x)) ? x : '.');
        dump_ascii += tmp;
      }

      while (dump_hex.length() < BYTES_PER_ROW * 3 + 1)
        dump_hex += " ";
      String dump = "DUMP: " + title + ": " + dump_offset + dump_hex + "  " + dump_ascii;
      LogMsg(dump.c_str());
    }
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
