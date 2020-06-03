/*
** util prototypes
*/
#ifndef __UTIL_H__
#define __UTIL_H__ 1

#include "wifi.h"

#if DBG
/*
**  debug message
*/
#define DbgMsg  LogMsg
#else
#define DbgMsg(...)  
#endif

/*
 * convert an IP address to a C string
 */
#define IPAddressToString(addr)   ((addr).toString().c_str())

/*
 * convert an IP address to a field of bytes
 */
const byte *IPAddressToBytes(IPAddress &addr);

/*
 * convert a field of bytes to an IP address
 */
IPAddress BytesToIPAddress(byte *bytes);

/*
**  return a given IP or HW address as a string
*/
const char *AddressToString(byte *addr,int addrlen,int dec,char sep);


/*
**  convert the string into an IP or HW address
*/
const byte *StringToAddress(const char *str,int addrlen,int dec);

/*
 * dump data from an address
 */
void dump(String title,const byte *addr,const int len);

/*
**  log a smessage to serial
*/
void LogMsg(const char *fmt,...);

#endif

/**/
