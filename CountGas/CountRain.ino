/*
**  Sketch to count pulses from a gas counter
**  and make them accessible via HTTP.
**  Optionally 
**
**  Needed Hardware:
**
**    ESP8266 with Wifi
**
**  Connectors:
**    GPIO #2 (D4)  OUTPUT  activity LED -- PIN is also connected to the onboard LED
**    GPIO #4 (D2)  OUTPUT  LED flashing upon trigger
**    GPIO #5 (D1)  INPUT   closing switch on trigger
**
**  Used EEPROM layout:
**    Address
**     0 ..  3 => IP address of the NTP server
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
#include <Time.h>
#include "util.h"
#include "wifi.h"
#include "ntp.h"
#include "http.h"
#include "counter.h"
#include "eeprom.h"

/*
 * GPIO pins to use
 */
#define ACTIVITY_PIN_OUT 2
#define COUNTER_PIN_OUT 4
#define COUNTER_PIN_IN 5

/*
**  activity/power indicator state
*/
#define ACTIVITY_CYCLES 10
static int _activity_state = LOW;
static int _activity_cycle = 0;


void setup()
{
    /*
    **  initialize serial communications
    */
    Serial.begin(115200);
    Serial.println();
    LogMsg("*** %s *** (%s)",__FILE__,__DATE__);
    
    /*
     * init the EEPROM
     */
    EepromInit(EEPROM_SIZE);
#if 0
    EepromClear();
#endif
    EepromDump();
    
        
    /*
    **    configure the activity LED
    */
    pinMode(ACTIVITY_PIN_OUT,OUTPUT);
    pinMode(COUNTER_PIN_OUT,OUTPUT);

    /*
    **    switch on the LEDs to signal initialization
    */
    digitalWrite(ACTIVITY_PIN_OUT,HIGH);
    digitalWrite(COUNTER_PIN_OUT,HIGH);

    /*
     * init wifi
     */
    WifiSetup();
    

#if NTP
    NtpInit();
#endif

    /*
     * init the web server
     */
    HttpSetup();

    /*
    **  init counter
    */
    //LogMsg("COUNTER: init counter");
    CounterInit(COUNTER_PIN_IN,COUNTER_PIN_OUT,EEPROM_ADDR_COUNTER_VAL,EEPROM_ADDR_COUNTER_INC);

    /*
    **  init done
    */
    LogMsg("READY");

    /*
    **    switch off the LEDs to signal readiness
    */
    digitalWrite(ACTIVITY_PIN_OUT,LOW);
    digitalWrite(COUNTER_PIN_OUT,LOW);
}

void loop()
{
    int ready = (timeStatus() == timeSet);
    
    /*
    **   flash the activity LED
    **   if not in ready state, the LED is flashing fast
    */
    if (++_activity_cycle >= ((ready) ? ACTIVITY_CYCLES : (ACTIVITY_CYCLES / 10))) {
      digitalWrite(ACTIVITY_PIN_OUT,_activity_state = ((_activity_state == LOW) ? HIGH : LOW));
      _activity_cycle = 0;
      //LogMsg("ALIVE");
    }

    WifiUpdate();
#if NTP
    NtpUpdate();
#endif
    CounterUpdate();
    HttpHandleRequest();
    
    delay(100);
}/**/
