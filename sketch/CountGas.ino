/*
**  Sketch to count pulses from a gas counter
**  and make them accessible via HTTP.
**  Optionally 
**
**  Needed Hardware:
**
**    Arduino Uno
**    Ethernet Shield
**
**  Connectors:
**    Counter trigger:
**      PIN2   <= closing switch on trigger
**
**    Counter Feedback:
**      PIN4  => LED
**
**  Used EEPROM layout:
**    Address
**     0 ..  5 => MAC address to use
**     6 ..  9 => IP address of the NTP server
**    10 .. 13 => counter value
**    14 .. 17 => counter increment per pulse
**
**  Runtime enviroment:
**      DHCP   => device will ask for an IP address
**      NTP    => device will connect an NTP server
**      HTTP   <= device will reply to HTTP requests with its current time, counter value
**
**  HTTP GET parameters to configure the device:
**     counterval  => set a new counter value (stored in EEPROM)
**     counterinc  => set a new counter increment (stored in EEPROM)
**     macaddr     => give the device a mac addr (stored in EEPROM; see below for default)
**     ntpip       => ip address of the NTP server (stored in EEPROM; default: 0.0.0.0)
*/
#define DBG  0
#define NTP  1

#include <stdarg.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Time.h>
#include <EEPROM.h>
#include "util.h"
#include "ntp.h"
#include "http.h"
#include "counter.h"
#include "eeprom.h"

/*
**  address in the EEPROM to store the counter
*/
#define EEPROM_ADDR_MACADDR       0
#define EEPROM_ADDR_NTPIP         6
#define EEPROM_ADDR_COUNTER_VAL   10
#define EEPROM_ADDR_COUNTER_INC   14

/*
**  set the own MAC address here
*/
byte _mac[] = {  0xDE, 0xAD, 0xBE, 0x00, 0x00, 0x01 };

#define COUNTER_PIN_IN 2
#define COUNTER_PIN_OUT 4
#define ACTIVITY_PIN_OUT 5

#define ACTIVITY_CYCLES 10

/*
**  activity/power indicator state
*/
int _activity_state = LOW;
int _activity_cycle = 0;

void setup()
{
#if 0
    for (int n = 0;n < 20;n++)
        EEPROM.write(n,0xff);
#endif

    /*
    **    configure the activity LED
    */
    pinMode(ACTIVITY_PIN_OUT,OUTPUT);

    /*
    **  initialize serial communications
    */
    Serial.begin(9600);

    /*
    **  initialize DHCP
    */
    EepromRead(EEPROM_ADDR_MACADDR,6,(byte *) &_mac);
    LogMsg("LAN: MAC=%s",AddressToString(_mac,6,0));
    //LogMsg("LAN: init DHCP");
    if (Ethernet.begin(_mac) == 0) {
        LogMsg("LAN: no DHCP");
        for(;;)
            ;
    }
    uint32_t ip = Ethernet.localIP();
    LogMsg("LAN: IP=%s",AddressToString((byte *) &ip,4,1));

#if NTP
    /*
    **  initialize NTP
    */
    //LogMsg("LAN: init NTP");
    NtpInit();
#endif

    /*
    **  init counter
    */
    //LogMsg("COUNTER: init counter");
    CounterInit(COUNTER_PIN_IN,COUNTER_PIN_OUT,EEPROM_ADDR_COUNTER_VAL,EEPROM_ADDR_COUNTER_INC);

    /*
    **  init done
    */
    LogMsg("READY");
}

void loop()
{
    /*
    **    flash the activity LED
    */
    if (++_activity_cycle >= ACTIVITY_CYCLES) {
      digitalWrite(ACTIVITY_PIN_OUT,_activity_state = ((_activity_state == LOW) ? HIGH : LOW));
      _activity_cycle = 0;
    }

    /*
    **  Update the counter
    */
    CounterUpdate();

    /*
    **  listen & handle incoming http requests
    */
    HttpHandleRequest();

    delay(100);
}/**/
