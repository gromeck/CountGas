/*
**	temp functions
*/
#if TMP
#include <OneWire.h>
#include <DallasTemperature.h>
#include "temp.h"

/*
**	the one wire lib
*/
static OneWire _oneWire(TEMP_PIN);

/*
**	the DS1820 sensors
*/
static DallasTemperature _sensors(&_oneWire);

/*
**  Create and initialize the list of DS18S20 sensors
*/
static const unsigned int DS1820_TEMPERATURE_PRECISION = 9;

/*
**  initialize temperatur sensors
*/
void TempInit(void)
{
    _sensors.begin();
    delay(1000);
    LogMsg("TEMP: found %d sensor(s)",_sensors.getDeviceCount());
    LogMsg("TEMP: parasite power is %s",(_sensors.isParasitePowerMode()) ? "ON" : "OFF");
    for (int n = 0;n < _sensors.getDeviceCount();n++) {
        DeviceAddress addr;

        _sensors.getAddress(addr,n);
        _sensors.setResolution(addr,DS1820_TEMPERATURE_PRECISION);
        LogMsg("TEMP: #%d: addr=%s",n,AddressToString(addr,8,0));
    }
}

void TempUpdate(void)
{
    /*
    **  Read temperature values and start next conversion.
    */
    _sensors.requestTemperatures();
}

/*
**  dump the sensors to an ethernet client
*/
void TempDump(EthernetClient client)
{
    for (int n = 0;n < _sensors.getDeviceCount();n++) {
        char buffer[8];
        DeviceAddress addr;

        _sensors.getAddress(addr,n);
        client.print("TEMP");
        client.print(n,DEC);
        client.print(" ");
        client.print(AddressToString(addr,8,0));
        client.print(" ");
        client.println(dtostrf(_sensors.getTempCByIndex(n),4,1,buffer));
    }
}

/*
**  print temperature of all sensors.
 */
void TempLogMsg(void)
{
    for (int n = 0;n < _sensors.getDeviceCount();n++) {
        char buffer[8];

        LogMsg("TEMP: #%d: %sC",n,
            dtostrf(_sensors.getTempCByIndex(n),4,1,buffer));
    }
}

#endif

/**/
