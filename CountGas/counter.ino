/*
**  counter functions
**
**  each pulse is 0,001cbm of gas
*/
#include "counter.h"
#include "eeprom.h"

#define COUNTER_PRECISION 1000000

/*
**  counter value
*/
static long _counter_val = 0;

/*
**  counter fraction / 1000000
*/
static long _counter_frac = 0;

/*
**  counter incremenent
*/
static double _counter_inc = 1000;

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
static void CounterTrigger()
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
**  initialize the counter
*/
void CounterInit(int counter_pin_in,int counter_pin_out,int counter_val_eeprom_addr,int counter_inc_eeprom_addr)
{
    time_t t = now();
    char buffer[20];
    double counter,increment;

    /*
    **  read the value stored in the EEPROM
    */
    _counter_val_eeprom_addr = counter_val_eeprom_addr;
    EepromReadByType(_counter_val_eeprom_addr,&counter);
    _counter_val = counter;
    _counter_frac = (counter - _counter_val) * COUNTER_PRECISION;
    LogMsg("COUNTER: counter=%s",dtostrf(counter,1,6,buffer));
    
    /*
    **  read the increment stored in the EEPROM
    */
    _counter_inc_eeprom_addr = counter_inc_eeprom_addr;
    EepromReadByType(_counter_inc_eeprom_addr,&increment);
    _counter_inc = increment * COUNTER_PRECISION;
    LogMsg("COUNTER: increment=%s",dtostrf(increment,1,6,buffer));
    
    /*
    **    init some book keeping
    */
    _counter_val_written = counter;
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
**  overwrite the counter
*/
void CounterSetValue(double counter)
{
    char buffer[20];
    
    if (_counter_val_written != counter) {
        LogMsg("COUNTER: writing counter %s to EEPROM",dtostrf(counter,1,6,buffer));
        EepromWriteByType(_counter_val_eeprom_addr,counter);
        _counter_val_written = counter;
        _counter_val_written_time = now();
    }
    _counter_val = counter;
    _counter_frac = (counter - _counter_val) * COUNTER_PRECISION;
}

/*
**  read the counter
*/
double CounterGetValue(void)
{
    return (double) _counter_val + (double) _counter_frac / COUNTER_PRECISION;
}

/*
**  overwrite the counter increment
*/
void CounterSetIncrement(double inc)
{
    char buffer[20];

    if (_counter_inc != (long) inc * COUNTER_PRECISION) {
        LogMsg("COUNTER: writing increment %s to EEPROM",dtostrf(inc,1,6,buffer));
        EepromWriteByType(_counter_inc_eeprom_addr,inc);
    }
    _counter_inc = inc * COUNTER_PRECISION;
}

/*
**  read the counter increment
*/
double CounterGetIncrement(void)
{
  return (double) _counter_inc / COUNTER_PRECISION;
}

/*
**  update the counter handling
*/
void CounterUpdate(void)
{
    time_t t = now();
    char buffer[20];

    /*
    **  dispatch on events
    */
    if (_counter_triggered) {
        if (t >= _counter_triggered_time + COUNTER_MIN_TRIGGER_CYCLE) {
            /*
            **  counter pulse detected
            */
            _counter_frac += _counter_inc;
            while (_counter_frac > COUNTER_PRECISION) {
              _counter_val++;
              _counter_frac -= COUNTER_PRECISION;
            }
            _counter_triggered_time = t;
            digitalWrite(_counter_pin_out,HIGH);
            LogMsg("COUNTER: value=%s",dtostrf(CounterGetValue(),1,6,buffer));
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
      CounterSetValue(COUNTER2DOUBLE);
}/**/
