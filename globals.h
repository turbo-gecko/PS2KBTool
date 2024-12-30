#ifndef _GLOBALS_H_
#define _GLOBALS_H_
/*
 * globals.h
 *
 * Global definitions file for the Arduino Nano
 * 
 * This software is copyright 2024-2025 by Gary Hammond (ZL3GH) along
 * with all the software bugs herein. It is free to use for
 * non-commercial purposes.
 * 
 * WARNING: DO NOT USE this software in any medical device or for any 
 * other mission critical purpose.
 * 
 * Use of this software could result in a universe ending paradox so 
 * use entirely at your own risk. No warranties or guarantees are 
 * expressed or implied.
 */

#include "nano_def.h"

/*************************************************************************
 * Global Constants
 *************************************************************************/

// Software info
const String  VERSION               = "00.02.00";

// ASCII key definitions
#define       CTRL_C                3       // Ascii code for control-c
#define       BACK_SPACE            8       // Ascii code for control-h or backspace
#define       XON                   17      // Ascii code for control-q or XON
#define       XOFF                  19      // Ascii code for control-s or XOFF
#define       UP_ARROW              30      // Ascii code for the up arrow

// Serial port definitions
#define       S_HOST                Serial
#define       S_DEF_HOST_BAUD       115200  // Default host baud rate
#define       S_DEF_CHAR_DELAY      0       // Default host inter character delay im mSec
#define       S_DEF_LINE_DELAY      0       // Default host inter line delay in mSec
#define       S_DEF_XON_XOFF        0       // Default value of 1 means enabled
#define       S_DEF_SERIAL_ENABLED  0       // Default value of 1 means enabled

// EEPROM address definitions
#define       E_CHECKSUM            0       // 4 bytes CRC32 checksum of E_SIZE bytes
#define       E_SIGNATURE           4       // 2 bytes containing the value 55 AA
#define       E_VERSION             6       // 2 byte string containing EEPROM version number
#define       E_SIZE                8       // 2 bytes (word) for the size of non-volatile data
#define       E_HOST_BAUD           10      // 4 bytes (long) for the host serial baud rate
#define       E_CHAR_DELAY          14      // 2 bytes (int) for host serial inter-character delay
#define       E_LINE_DELAY          16      // 2 bytes (int) for host serial inter-line delay
#define       E_XON_XOFF            18      // 1 byte (byte) for XON/XOFF enabled flag
#define       E_SERIAL_ENABLED      19      // 1 byte (byte) for serial enabled flag
#define       E_END_ADDRESS         20      // End of EEPROM values

#endif // _GLOBALS_H_
