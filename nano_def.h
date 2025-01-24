#ifndef _NANO_DEF_H_
#define _NANO_DEF_H_
/*
 * nano_def.h
 *
 * Definitions file for the Arduino Nano when used with the
 * Garmic-001 carrier board.
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

// Arduino Nano Pins
#define D0            0
#define D1            1
#define D2            2
#define D3            3
#define D4            4
#define D5            5
#define D6            6
#define D7            7
#define D8            8
#define D9            9
#define D10           10
#define D11           11
#define D12           12
#define D13           13
#define D14           14
#define D15           15

#define CTS           D2

#define AT_CLK        D3
#define AT_DATA       D9

#define XT_CLK        D11
#define XT_DATA       D10

#ifdef DEV_BOARD
#define CONFIG_1      D12
#define CONFIG_2      D13
#else
// Fix for getting the dip switch the wrong way around on the board!
#define CONFIG_1      D13
#define CONFIG_2      D12
#endif

#ifdef DEV_BOARD
#define LED_AT_CLK    D8
#define LED_AT_DATA   D7
#define LED_NUM       D6
#define LED_CAPS      D5
#define LED_SCROLL    D4
#endif

#define BUFFER_SIZE   1024  // The general upper limit is ~1536 bytes
//#define BUFFER_SIZE   512  // The general upper limit is ~1536 bytes
#define LED_DELAY     100   // delay time in mS for LED cycling
#define TX_DELAY      20    // Time delay between serial data in uS

#endif // _NANO_DEF_H_
