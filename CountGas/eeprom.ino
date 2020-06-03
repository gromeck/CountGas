/*
**	eeprom functions & definitions
*/
#include <EEPROM.h>

static int _eeprom_size;

/*
**  init the EEPROM handling
*/
void EepromInit(const int size)
{
    _eeprom_size = size;
    EEPROM.begin(_eeprom_size);
}

/*
**  clear the eeprom
*/
void EepromDump()
{
    byte *buffer;

    if (buffer = (byte *) malloc(_eeprom_size)) {
      EepromRead(0,_eeprom_size,buffer);
      dump("EEPROM",buffer,_eeprom_size);
      free(buffer);
    }
}

/*
**  clear the eeprom
*/
void EepromClear()
{
    for (int n = 0;n < _eeprom_size;n++)
        EEPROM.write(n,0xff);
    EEPROM.commit();
}

/*
**    read from the EEPROM
**
**    if all bytes were FF, zero is returned
**    and the buffer is not written
*/
int EepromRead(const int addr,const int len,byte *buffer)
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
        //LogMsg("EEPROM.read: addr[%d] = %u",addr + n,buffer[n]);
    }
    return 1;
}

/*
**	write to the EEPROM
*/
void EepromWrite(const int addr,const int len,const byte *buffer)
{
    for (int n = 0;n < len;n++) {
        EEPROM.write(addr + n,buffer[n]);
        //LogMsg("EEPROM.write: addr[%d] = %u",addr + n,buffer[n]);
    }
    EEPROM.commit();
}

/*
 * read a String from the EEPROM
 */
String EepromReadString(const int addr,const int maxlen)
{
    char *buffer = NULL;
    String s = "";

    if (buffer = (char *) malloc(maxlen)) {
      if (EepromRead(addr,maxlen,(byte *) buffer)) {
        buffer[maxlen - 1] = '\0';
        s = buffer;
      }
      free(buffer);
    }
    return s;
}

/*
 * read a String from the EEPROM
 */
void EepromWriteString(const int addr,const int maxlen,String s)
{
    char *buffer = NULL;
    
    if (s.length() < maxlen && (buffer = (char *) malloc(maxlen))) {
      memset(buffer,'\0',maxlen);
      strcpy(buffer,s.c_str());
      EepromWrite(addr,maxlen,(byte *) buffer);
      free(buffer);
    }
}

/**/
