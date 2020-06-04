/*
** util prototypes
*/

#ifndef __UTIL_H__
#define __UTIL_H__ 1

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
#define IPAddressToString(addr)   AddressToString(IPAddressToBytes(addr),4,1)

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
const char *AddressToString(const byte *addr,int addrlen,int dec);


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
