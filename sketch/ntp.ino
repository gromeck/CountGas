/*
**	NTP functions
*/
#if NTP
#include "util.h"
#include <Ethernet.h>
#include <EthernetClient.h>
#include <Time.h>
#include "ntp.h"

/*
**  local port to listen for UDP packets
*/
#define NTP_UDP_LOCAL_PORT 8888

/*
**  global port to receive the UDP packets from
*/
#define NTP_UDP_PORT 123

/*
**  NTP time sync interval
*/
#define NTP_SYNC_INTERVAL  (60 * 5)

/*
**  IP address of our NTP server (trillian)
*/
static IPAddress _NtpServer(0,0,0,0);

/*
**  NTP time stamp is in the first 48 bytes of the message
*/
#define NTP_PACKET_SIZE 48

/*
**  NTP packet buffer
*/
static byte _packetBuffer[NTP_PACKET_SIZE];

/*
**  UDP instance to let us send and receive packets
*/
static EthernetUDP _Udp;

/*
**  send an NTP request to the time server at the given address
**
**  NOTE: function is called asynchronous, so don't use LogMsg or Serial!
*/
static void NtpSendRequest(void)
{
    memset(_packetBuffer,0,NTP_PACKET_SIZE);
    
    _packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    _packetBuffer[1] = 0;     // Stratum, or type of clock
    _packetBuffer[2] = 6;     // Polling Interval
    _packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    _packetBuffer[12]  = 49;
    _packetBuffer[13]  = 0x4E;
    _packetBuffer[14]  = 49;
    _packetBuffer[15]  = 52;

    _Udp.beginPacket(_NtpServer,NTP_UDP_PORT);
    _Udp.write(_packetBuffer,NTP_PACKET_SIZE);
    _Udp.endPacket();
}

/*
**  handle a received NTP reply
**
**  NOTE: function is called asynchronous, so don't use LogMsg or Serial!
*/
static time_t NtpReceiveReply(void)
{
    /*
    **    read the packet
    */
    _Udp.read(_packetBuffer,NTP_PACKET_SIZE);

    unsigned long highWord = word(_packetBuffer[40], _packetBuffer[41]);
    unsigned long lowWord = word(_packetBuffer[42], _packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long ntpTime = highWord << 16 | lowWord;
    // now convert NTP time into UNIX time (seconds since Jan 1 1970)
    return ntpTime - 2208988800UL;
}

/*
**  sent an NTP request and wait for the answer
**
**  NOTE: function is called asynchronous, so don't use LogMsg or Serial!
**
**  NOTE: we will wait for a maximum of 200 * 10ms
*/
static time_t NtpSync(void)
{
    NtpSendRequest();
    
    for (int retry = 0;retry < 200;retry++) {
        if (_Udp.parsePacket()) {
            return NtpReceiveReply();
        }
        delay(10);
    }
    return 0;
}

/*
**	init the ntp functionality
*/
void NtpInit(void)
{
    /*
    **    read the NTP servers IP address from the EEPROM
    */
    uint32_t ip;
    if (EepromRead(EEPROM_ADDR_NTPIP,4,(byte *) &ip))
        _NtpServer = ip;
    LogMsg("NTP: server=%s",AddressToString((byte *) &ip,4,1));
    
    /*
    **    init the UDP socket
    */
    if (_NtpServer[0]) {
        _Udp.begin(NTP_UDP_LOCAL_PORT);
        setSyncInterval((unsigned int) NTP_SYNC_INTERVAL);
        setSyncProvider(NtpSync);
    }
}

#endif

/**/

