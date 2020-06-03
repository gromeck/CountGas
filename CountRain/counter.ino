/*
**	counter functions
**
**  the tipping bucket rain sensor from MISOL is used here
**
**  each tip is ...
**    0.2794mm/m²  https://pi.gate.ac.uk/posts/2014/01/25/raingauge/
**    0.3537mm/m²  https://www.robotics.org.za/WH-SP-RG
**    0.01inch/m²  https://www.amazon.de/MISOL-weather-Ersatzteil-Wetterstation-Manometer/dp/B00QDMBXUA (aus Frage)
**     = 0.254mm/m²
**    0,303mm/m²   https://www.amazon.de/MISOL-weather-Ersatzteil-Wetterstation-Manometer/dp/B00QDMBXUA (aus Rezension)
*/
#include "counter.h"
#include "eeprom.h"

/*
**  counter value
*/
static double _counter_val = 0.0;

/*
**  counter incremenent
*/
static double _counter_inc = 0.3;

/*
**  minimum seconds between two triggers
*/
#define COUNTER_MIN_TRIGGER_CYCLE 1

/*
**  minimum milli seconds the digital input has to be at HIGH
**  before a valid trigger is detected
*/
#define COUNTER_MIN_TRIGGER_LENGTH 50

/* 
**  flag for the counter trigger
*/
static volatile boolean _counter_triggered = false;

/*
**  timestamp when the counter was triggered last time
*/
static time_t _counter_triggered_time = 0;

/*
**  the address to read/write the counter into the EEPROM
*/
static int _counter_val_eeprom_addr = -1;
static int _counter_inc_eeprom_addr = -1;

/*
**  how often should the counter value be written to the EEPROM
**
**  NOTE: one per hours seems to be a tradeoff between the EEPROMs
**        life cycle (~100.000 write/erase ops) and not loosing
**        the counter
*/
#define COUNTER_EEPROM_WRITE_CYCLE (60 * 60)

/*
**  timestamp when the counter was written last time to the EEPROM
*/
static time_t _counter_val_written_time = 0;

/*
**  last counter value which was written to the EEPROM
*/
static double _counter_val_written = 0;

/*
**  the pins to use (will be overwritten during init)
*/
static int _counter_pin_in;
static int _counter_pin_out;

/*
**  interrupt handler
**
**  the interrupt handler is disabled
**  right after the trigger was received
*/
static void ICACHE_RAM_ATTR CounterTrigger()
{
    static volatile unsigned long _last_low = 0;
    static volatile boolean _last_state = HIGH;

    int state = digitalRead(_counter_pin_in) ? HIGH : LOW;

    if (state == _last_state)
        return;

    if ((_last_state = state) == HIGH) {
        /*
        **    HIGH
        */
        if (millis() - _last_low > COUNTER_MIN_TRIGGER_LENGTH)
            _counter_triggered = true;
    }
    else {
        /*
        **    LOW
        */
        _last_low = millis();
    }
}

/*
**	initialize the counter
*/
void CounterInit(int counter_pin_in,int counter_pin_out,int counter_val_eeprom_addr,int counter_inc_eeprom_addr)
{
    time_t t = now();

    /*
    **  read the value stored in the EEPROM
    */
    _counter_val_eeprom_addr = counter_val_eeprom_addr;
    EepromReadByType(_counter_val_eeprom_addr,&_counter_val);
    LogMsg("COUNTER: value=%.6lf",_counter_val);
    
    /*
    **  read the increment stored in the EEPROM
    */
    _counter_inc_eeprom_addr = counter_inc_eeprom_addr;
    EepromReadByType(_counter_inc_eeprom_addr,&_counter_inc);
    LogMsg("COUNTER: increment=%.6lf",_counter_inc);
    
    /*
    **    init some book keeping
    */
    _counter_val_written = _counter_val;
    _counter_val_written_time = t;
    _counter_triggered_time = t;

    /*
    **  configure GPIO
    **   - enable the pullup
    **   - use interrupt (= 0)
    */
    _counter_pin_in = counter_pin_in;
    LogMsg("COUNTER: input pin=%d",_counter_pin_in);
    pinMode(_counter_pin_in,INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(_counter_pin_in),CounterTrigger,CHANGE);

    /*
    **  configure GPIO as monitor (LED)
    */
    _counter_pin_out = counter_pin_out;
    LogMsg("COUNTER: output pin=%d",_counter_pin_out);
    pinMode(counter_pin_out,OUTPUT);
}

/*
**	overwrite the counter
*/
void CounterSetValue(double value)
{
    _counter_val = value;
    if (_counter_val_written != _counter_val) {
        LogMsg("COUNTER: writing value %.6lf to EEPROM",_counter_val);
        EepromWriteByType(_counter_val_eeprom_addr,_counter_val);
        _counter_val_written = _counter_val;
        _counter_val_written_time = now();
    }
}

/*
**	read the counter
*/
double CounterGetValue(void)
{
    return _counter_val;
}

/*
**  overwrite the counter increment
*/
void CounterSetIncrement(double inc)
{
    if (_counter_inc != inc) {
        _counter_inc = inc;
        EepromWriteByType(_counter_inc_eeprom_addr,_counter_inc);
    }
}

/*
**  read the counter increment
*/
double CounterGetIncrement(void)
{
  return _counter_inc;
}

/*
**	update the counter handling
*/
void CounterUpdate(void)
{
    time_t t = now();

    /*
    **  dispatch on events
    */
    if (_counter_triggered) {
        if (t >= _counter_triggered_time + COUNTER_MIN_TRIGGER_CYCLE) {
            /*
            **  counter pulse detected
            */
            _counter_val += _counter_inc;
            _counter_triggered_time = t;
            digitalWrite(_counter_pin_out,HIGH);
            LogMsg("COUNTER: %.6lf",_counter_val);
        }
        _counter_triggered = false;
    }
    else {
        digitalWrite(_counter_pin_out,LOW);
    }

    /*
     * maybe it's time to write the counter to the EEPROM
     */
    if (t >= _counter_val_written_time + COUNTER_EEPROM_WRITE_CYCLE)
      CounterSetValue(_counter_val);
}/**/
