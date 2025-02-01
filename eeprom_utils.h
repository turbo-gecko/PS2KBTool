#ifndef _EEPROM_UTILS_H_
#define _EEPROM_UTILS_H_

/*
 * eeprom_utils.h
 * 
 * EEPROM helper functions.
 * 
 * This software is copyright 2024-2025 by Gary Hammond (ZL3GH). It is free
 * to use for non-commercial purposes.
 * 
 * WARNING: DO NOT USE this software in any medical device or for any 
 * other mission critical purpose.
 * 
 * Use of this software could result in a universe ending paradox so 
 * use entirely at your own risk. No warranties or guarantees are 
 * expressed or implied.
 */

unsigned long eCrc(void);
void eInit();
void ePrintValues();
void eResetDefaultValues();
void eUpdateCrc();

#endif // _EEPROM_UTILS_H_
