/*
** util prototypes
*/

#if DBG
/*
**  debug message
*/
#define DbgMsg  LogMsg
#else
#define DbgMsg(...)  
#endif

/*
**  return a given IP or HW address as a string
*/
char *AddressToString(byte *addr,int addrlen,int dec,char sep);

/*
**  convert the string into an IP or HW address
*/
IPAddress StringToIpAddress(char *buffer);

/*
**  log a smessage to serial
*/
void LogMsg(const char *fmt,...);

/**/

