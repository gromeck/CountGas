/*
**	eeprom functions & definitions
*/
#include <EEPROM.h>

/*
**    read from the EEPROM
**
**    if all bytes were FF, zero is returned
**    and the buffer is not written
*/
int EepromRead(int addr,int len,byte *buffer)
{
    int n;
       
    for (n = 0;n < len;n++) {
        if (EEPROM.read(addr + n) != 0xff)
            break;
    }
    if (n >= len)
        return 0;

    for (n = 0;n < len;n++) {
        buffer[n] = EEPROM.read(addr + n);
    }
    return 1;
}

/*
**	write to the EEPROM
*/
void EepromWrite(int addr,int len,byte *buffer)
{
    for (int n = 0;n < len;n++)
        EEPROM.write(addr + n,buffer[n]);
}

/**/

