#ifndef _GLOBALS_H_
#define _GLOBALS_H_
/*
 * globals.h
 *
 * Global definitions file for the Arduino Nano
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

#include "nano_def.h"

/*************************************************************************
 * Global Constants
 *************************************************************************/

// Software info
const String  VERSION           = "01.00.00";

// Host board type
#define B_STD                   1       // Standard board with no LED's or dev options switches
#define B_DEV                   2       // Development board
#define B_NUXT                  3       // Board specific for the NuXT that is internally mounted
#define B_LAST                  4       // Used to check valid board types. Always keep at the end

// ASCII key constants
#define CTRL_C                  3       // Ascii code for control-c
#define BACK_SPACE              8       // Ascii code for control-h or backspace
#define XON                     17      // Ascii code for control-q or XON
#define XOFF                    19      // Ascii code for control-s or XOFF
#define UP_ARROW                30      // Ascii code for the up arrow

// Serial constants
#define TX_DELAY                20    // Time delay between serial data in uS

// Default serial definitions
#define S_HOST                  Serial
#define S_DEF_HOST_BAUD         115200  // Default host baud rate
#define S_DEF_CHAR_DELAY        0       // Default host inter character delay im mSec
#define S_DEF_LINE_DELAY        0       // Default host inter line delay in mSec
#define S_DEF_XON_XOFF          0       // Default value of 1 means enabled
#define S_DEF_SERIAL_ENABLED    0       // Default value of 1 means enabled

// Default keyboard definitions
#define K_DEF_EXT_KEYS_ENABLED  0       // Default value of 1 means enabled
#define K_DEF_BOARD_TYPE        1       // Default value of 1 for the standard board

#define K_DEF_AT_BIT_DELAY      30      // AT_CLK bit time delay (per clock state)
#define K_DEF_AT_NEXT_DELAY     3       // Wait period after sending byte to the AT keyboard
#define K_DEF_AT_START_DELAY    5       // Settling period after changing AT_CLK before changing AT_DATA

#define K_DEF_XT_BIT_DELAY      30      // XT_CLK bit time delay (per clock state)
#define K_DEF_XT_NEXT_DELAY     100     // Wait period after sending byte to the XT keyboard
#define K_DEF_XT_START_DELAY    5       // Settling period after changing XT_CLK before changing XT_DATA

// EEPROM address definitions
#define E_CHECKSUM              0       // 4 bytes CRC32 checksum of E_SIZE bytes
#define E_SIGNATURE             4       // 2 bytes containing the value 55 AA
#define E_VERSION               6       // 2 bytes (int) containing EEPROM version number
#define E_SIZE                  8       // 2 bytes (word) for the size of non-volatile data
#define E_HOST_BAUD             10      // 4 bytes (long) for the host serial baud rate
#define E_CHAR_DELAY            14      // 2 bytes (int) for host serial inter-character delay
#define E_LINE_DELAY            16      // 2 bytes (int) for host serial inter-line delay
#define E_XON_XOFF              18      // 1 byte (byte) for XON/XOFF enabled flag
#define E_SERIAL_ENABLED        19      // 1 byte (byte) for serial enabled flag
#define E_EXT_KEYS_ENABLED      20      // 1 byte (byte) for 101 extended keys flag
#define E_BOARD_TYPE            21      // 2 bytes (int) for the board type
#define E_AT_BIT_DELAY          23      // 1 byte (byte) for AT bit delay
#define E_AT_NEXT_DELAY         24      // 1 byte (byte) for AT next byte delay
#define E_AT_START_DELAY        25      // 1 byte (byte) for AT start bit delay
#define E_XT_BIT_DELAY          26      // 1 byte (byte) for XT bit delay
#define E_XT_NEXT_DELAY         27      // 1 byte (byte) for XT next byte delay
#define E_XT_START_DELAY        28      // 1 byte (byte) for XT start bit delay
#define E_END_ADDRESS           29      // End of EEPROM values

#endif // _GLOBALS_H_
