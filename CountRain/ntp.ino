/*
**	NTP functions
*/
#if NTP
#include "util.h"
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
**  NTP time stamp is in the first 48 bytes of the message
*/
#define NTP_PACKET_SIZE 48

/*
**  NTP packet buffer
*/
static byte _packetBuffer[NTP_PACKET_SIZE];

/*
 * some NTP configuration
 */
static String _ntp_server = "";
static IPAddress _ntp_ip(0,0,0,0);
static int _ntp_sync_cycle = 0;

/*
**  UDP instance to let us send and receive packets
*/
static WiFiUDP _Udp;

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

    _Udp.beginPacket(_ntp_ip,NTP_UDP_PORT);
    _Udp.write(_packetBuffer,NTP_PACKET_SIZE);
    _Udp.endPacket();
//    Serial.println("NTP: requesing time");
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

    /*
     * NTP time is in seconds since Jan 1 1900
     */
    unsigned long highWord = word(_packetBuffer[40], _packetBuffer[41]);
    unsigned long lowWord = word(_packetBuffer[42], _packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long ntpTime = highWord << 16 | lowWord;
    // now convert NTP time into UNIX time (seconds since Jan 1 1970)
    ntpTime -= 2208988800UL;
//    Serial.print("NTP: time received: "); Serial.println(ntpTime);
    return ntpTime;
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
     * get the NTP from the configuration
     */
    _ntp_server = EepromReadString(EEPROM_ADDR_NTP_SERVER,EEPROM_SIZE_NTP_SERVER);
    if (_ntp_server == "")
      _ntp_server = NTP_SERVER_DEFAULT;

    LogMsg("NTP: server=%s",_ntp_server.c_str());

    /*
     * look up the ntpservers ip
     */
    if (WiFi.hostByName(_ntp_server.c_str(),_ntp_ip)) {
      /*
       * we could resolve the ntp name, so store it in the flash
       */
      LogMsg("NTP: lookup of %s successful: %s",_ntp_server.c_str(),IPAddressToString(_ntp_ip));
    }

    LogMsg("NTP: ip=%s",IPAddressToString(_ntp_ip));

    /*
    **    init the UDP socket
    */
    if (_ntp_ip[0]) {
        _Udp.begin(NTP_UDP_LOCAL_PORT);
        setSyncInterval((unsigned int) NTP_SYNC_INTERVAL);
        setSyncProvider(NtpSync);
    }
}

/*
**  update the NTP time
*/
void NtpUpdate(void)
{
    /*
     * if time got not yet synced, retry the init
     */
    if (++_ntp_sync_cycle >= NTPSYNC_CYCLES && timeStatus() != timeSet) {
        _ntp_sync_cycle = 0;
        NtpInit();
    }
}

#endif

/**/
