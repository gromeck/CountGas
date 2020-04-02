/*
**	eeprom prototypes
*/

/*
**	read from the EEPROM
**
**	if all bytes were FF, zero is returned
*/
int EepromRead(int addr,int len,byte *buffer);

/*
**	write to the EEPROM
*/
void EepromWrite(int addr,int len,byte *buffer);

/**/

