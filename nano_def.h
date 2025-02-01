#ifndef _NANO_DEF_H_
#define _NANO_DEF_H_
/*
 * nano_def.h
 *
 * Definitions file for the Arduino Nano. These are pin specific definitions
 * for the Arduino Nano. To use another Arduino, duplicate and rename this
 * file to the new platform, change the definitions and update PS2KBTool to
 * point to the new file.
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

// PS2KBTool pin definitions
#define AT_CLK        D3
#define AT_DATA       D9

#define XT_CLK        D11
#define XT_DATA       D10

#define CONFIG_1      D12
#define CONFIG_2      D13

#define LED_AT_CLK    D8
#define LED_AT_DATA   D7
#define LED_XT_CLK    D6
#define LED_XT_DATA   D5
#define LED_PROG      D4

#endif // _NANO_DEF_H_
