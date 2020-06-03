/*
**	NTP prototypes
*/
#if NTP

#ifndef __NTP_H__
#define __NTP_H__ 1

#include "util.h"

#define NTP_SERVER_DEFAULT "ntp.site"

/*
**  ntp sync cycle
*/
#define NTPSYNC_CYCLES 100

/*
 * store the NTP server
 */
#define NtpSetServer(ntp_server)   EepromWriteString(EEPROM_ADDR_NTP_SERVER,EEPROM_SIZE_NTP_SERVER,(ntp_server))

/*
**  init the NTP functions
*/
void NtpInit(void);

/*
**  update the NTP time
*/
void NtpUpdate(void);


#endif

#endif

/**/
