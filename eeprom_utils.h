#ifndef _EEPROM_UTILS.H_
#define _EEPROM_UTILS.H_

/*
 * eeprom_utils.h
 * 
 * EEPROM helper functions.
 * 
 * This software is copyright 2021 by Gary Hammond (ZL3GH). It is
 * free to use for non-commercial use.
 * 
 * WARNING: Use of this software could result in a universe ending
 * paradox so use at your own risk. No warranties expressed or 
 * implied.
 */

unsigned long eCrc(void);
void eInit();
void ePrintValues();
void eResetDefaultValues();
void eUpdateCrc();

#endif // _EEPROM_UTILS.H_
