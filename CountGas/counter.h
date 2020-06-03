/*
**	counter prototypes
*/
#ifndef __COUNTER_H__
#define __COUNTER_H__ 1

/*
**	initialize the counter
*/
void CounterInit(int counter_pin_in,int counter_pin_out,int eeprom_addr);

/*
**	overwrite the counter
*/
void CounterSetValue(double counter);

/*
**	read the counter
*/
double CounterGetValue(void);

/*
**  overwrite the counter increment
*/
void CounterSetIncrement(double inc);

/*
**  read the counter increment
*/
double CounterGetIncrement(void);

/*
**	update the counter handling
*/
void CounterUpdate(void);

#endif

/**/
