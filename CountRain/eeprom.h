/*
**	eeprom prototypes
*/
#ifndef __EPROM_H__
#define __EPROM_H__ 1

#include "util.h"
#include <EEPROM.h>

/*
**  configuration layout of the EEPROM
*/
#define EEPROM_ADDR_WIFI_SSID     0
#define EEPROM_SIZE_WIFI_SSID     64
#define EEPROM_ADDR_WIFI_PSK      (EEPROM_ADDR_WIFI_SSID + EEPROM_SIZE_WIFI_SSID)
#define EEPROM_SIZE_WIFI_PSK      64
#define EEPROM_ADDR_NTP_SERVER    (EEPROM_ADDR_WIFI_PSK + EEPROM_SIZE_WIFI_PSK)
#define EEPROM_SIZE_NTP_SERVER    64
#define EEPROM_ADDR_COUNTER_VAL   (EEPROM_ADDR_NTP_SERVER + EEPROM_SIZE_NTP_SERVER)
#define EEPROM_SIZE_COUNTER_VAL   sizeof(double)
#define EEPROM_ADDR_COUNTER_INC   (EEPROM_ADDR_COUNTER_VAL + EEPROM_SIZE_COUNTER_VAL)
#define EEPROM_SIZE_COUNTER_INC   sizeof(double)
#define EEPROM_SIZE               (EEPROM_ADDR_COUNTER_INC + EEPROM_SIZE_COUNTER_INC)

/*
**  init the EEPROM handling
*/
void EepromInit(const int size);

/*
**  dump the EEPROM
*/
void EepromDump(void);

/*
**  clear the EEPROM
*/
void EepromClear(void);

/*
**	read from the EEPROM
**
**	if all bytes were FF, zero is returned
*/
int EepromRead(int addr,int len,byte *buffer);

/*
**	write to the EEPROM
*/
void EepromWrite(int addr,int len,const byte *buffer);

/*
**  helper to read or write a datatype
*/
#define EepromReadByType(addr,x)    EepromRead(addr,sizeof((*x)),(byte *) (x))

/*
**  counter to EEPROM
*/
#define EepromWriteByType(addr,x)   EepromWrite(addr,sizeof(x),(byte *) &(x))

/*
 * read a String from the EEPROM
 */
String EepromReadString(int addr,int maxlen);

/*
 * read a String from the EEPROM
 */
void EepromWriteString(int addr,int maxlen,String s);

#endif

/**/
