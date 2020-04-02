/*
**	counter functions
*/
#include "counter.h"
#include "eeprom.h"

/*
**  counter value
*/
static unsigned long _counter_val = 0;

/*
**  counter increase per pulse
*/
static unsigned long _counter_inc = 1;

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
static unsigned long _counter_val_written = 0;

/*
**  the pins to use (will be overwritten during init)
*/
static int _counter_pin_in = 2;
static int _counter_pin_out = 4;

/*
**  counter to EEPROM
**
**  return 0 if the counter was never written
*/
#if 0
static int CounterReadFromEeprom(unsigned int addr,unsigned long *x)
{
    return EepromRead(addr,sizeof(*x),(byte *) x);
}
#else
#define CounterReadFromEeprom(addr,x)    EepromRead(addr,sizeof((*x)),(byte *) (x))
#endif

/*
**  counter to EEPROM
*/
#if 0
static void CounterWriteToEeprom(unsigned int addr,unsigned long x)
{
    EepromWrite(addr,sizeof(x),(byte *) &x);
}
#else
#define CounterWriteToEeprom(addr,x)   EepromWrite(addr,sizeof(x),(byte *) &(x))
#endif

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
**	initialize the counter
*/
void CounterInit(int counter_pin_in,int counter_pin_out,int value_eeprom_addr,int increment_eeprom_addr)
{
    time_t t = now();

    /*
    **    check if the input pin is valid
    */
#if 1
    if (counter_pin_in != 2 && counter_pin_in != 3) {
        LogMsg("COUNTER: no intr on input pin %d",counter_pin_in);
        for (;;)
            ;
    }
#endif
    
    /*
    **  read the value stored in the EEPROM
    */
    _counter_val_eeprom_addr = value_eeprom_addr;
    CounterReadFromEeprom(_counter_val_eeprom_addr,&_counter_val);
    LogMsg("COUNTER: value=%lu",_counter_val);
    
    /*
    **  read the increment stored in the EEPROM
    */
    _counter_inc_eeprom_addr = increment_eeprom_addr;
    CounterReadFromEeprom(_counter_inc_eeprom_addr,&_counter_inc);
    LogMsg("COUNTER: increment=%lu",_counter_inc);
    
    /*
    **    init some book keeping
    */
    _counter_val_written = _counter_val;
    _counter_val_written_time = t;
    _counter_triggered_time = t;

    /*
    **  configure PIN2
    **   - enable the pullup
    **   - use interrupt (= 0)
    */
    _counter_pin_in = counter_pin_in;
    LogMsg("COUNTER: input pin=%d",_counter_pin_in);
    pinMode(_counter_pin_in,INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(_counter_pin_in),CounterTrigger,CHANGE);

    /*
    **  configure PIN13 (LED)
    */
    _counter_pin_out = counter_pin_out;
    LogMsg("COUNTER: output pin=%d",_counter_pin_out);
    pinMode(counter_pin_out,OUTPUT);
}

/*
**	overwrite the counter
*/
void CounterSetValue(unsigned long value)
{
    _counter_val = value;
    if (_counter_val_written != _counter_val) {
        CounterWriteToEeprom(_counter_val_eeprom_addr,_counter_val);
        _counter_val_written = _counter_val;
    }
    _counter_val_written_time = now();
}

/*
**	read the counter
*/
unsigned long CounterGetValue(void)
{
    return _counter_val;
}

/*
**	overwrite the counter increment
*/
void CounterSetIncrement(unsigned long increment)
{
    _counter_inc = increment;
    CounterWriteToEeprom(_counter_inc_eeprom_addr,_counter_inc);
}

/*
**	overwrite the counter increment
*/
unsigned long CounterGetIncrement(void)
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
            LogMsg("COUNTER: %ld",_counter_val);

            if (t >= _counter_val_written_time + COUNTER_EEPROM_WRITE_CYCLE)
	        CounterSetValue(_counter_val);
        }
        _counter_triggered = false;
    }
    else {
        digitalWrite(_counter_pin_out,LOW);
    }
}/**/
