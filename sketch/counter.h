/*
**	counter prototypes
*/

/*
**	initialize the counter
*/
void CounterInit(int counter_pin_in,int counter_pin_out,int eeprom_addr);

/*
**	overwrite the counter
*/
void CounterSet(unsigned long counter);

/*
**	read the counter
*/
unsigned long CounterGetValue(void);

/*
**	overwrite the counter increment
*/
void CounterSetIncrement(unsigned long increment);

/*
**	read the increment
*/
unsigned long CounterGetIncrement(void);

/*
**	update the counter handling
*/
void CounterUpdate(void);

/**/

