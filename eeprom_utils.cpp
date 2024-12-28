/*
 * eeprom_utils.cpp
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

#include <Arduino.h>
#include <EEPROM.h>

#include "globals.h"

#include "eeprom_utils.h"
#include "serial_utils.h"

//*************************************************************************
unsigned long eCrc(void) {
  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  unsigned long crc = ~0L;

  for (int index = E_SIGNATURE ; index < E_END_ADDRESS  ; ++index) {
    crc = crc_table[(crc ^ EEPROM[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (EEPROM[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  return crc;
}

//*************************************************************************
void eInit() {
  if (EEPROM.read(E_SIGNATURE) == 0x55 && EEPROM.read(E_SIGNATURE + 1) == 0xAA) {
    unsigned long crc_calc = eCrc();
    unsigned long crcSaved = 0;

    EEPROM.get(E_CHECKSUM, crcSaved);
  }
  else {
    eResetDefaultValues();
  }
}

//*************************************************************************
void ePrintValues() {
  for (int i = 0; i < E_END_ADDRESS; i++) {
    if (EEPROM.read(i) < 16) {
      S_HOST.print("0");
    }
    S_HOST.print(String(EEPROM.read(i), HEX) + " ");
  }
  S_HOST.println("");
}

//*************************************************************************
void eResetDefaultValues() {
  unsigned long crc_calc = 0;

  ePrintValues();

  EEPROM.put(E_SIGNATURE, (unsigned int) 0xAA55);
  EEPROM.put(E_VERSION, (unsigned int) 0x0001);
  EEPROM.put(E_SIZE, (unsigned int) (E_END_ADDRESS - 4));
  EEPROM.put(E_HOST_BAUD, (unsigned int) S_DEF_HOST_BAUD);
  EEPROM.put(E_CHAR_DELAY, (unsigned int) S_DEF_CHAR_DELAY);
  EEPROM.put(E_LINE_DELAY, (unsigned int) S_DEF_LINE_DELAY);
  EEPROM.put(E_XON_XOFF, (byte) S_DEF_XON_XOFF);
  eUpdateCrc();

  ePrintValues();
}

//*************************************************************************
void eUpdateCrc() {
  unsigned long crc_calc = 0;

  crc_calc = eCrc();
  EEPROM.put(E_CHECKSUM, (unsigned long) crc_calc);
}
