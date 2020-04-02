/*
**	temp prototypes
*/

#if TMP
#include <OneWire.h>
#include <DallasTemperature.h>
#include "util.h"

/*
**	init the sensors
*/
void TempInit(void);

/*
**  cyclic update of the sensors
*/
void TempUpdate(void);

/*
**  print temperature of all sensors.
*/
void TempLogMsg(void);

#endif

/**/

