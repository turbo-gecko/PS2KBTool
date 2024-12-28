#ifndef _NANO_DEF_H_
#define _NANO_DEF_H_
/*
 * nano_def.h
 *
 * Definitions file for the Arduino Nano
 * 
 * This software is copyright 2021 by Gary Hammond (ZL3GH). It is
 * free to use for non-commercial use.
 * 
 * WARNING: Use of this software could result in a universe ending
 * paradox so use at your own risk. No warranties expressed or 
 * implied.
 */

// Arduino Nano Pins
#define D0          0
#define D1          1
#define D2          2
#define D3          3
#define D4          4
#define D5          5
#define D6          6
#define D7          7
#define D8          8

#define CTS         D2
#define LED_0       D8
#define LED_25      D7
#define LED_50      D6
#define LED_75      D5
#define LED_100     D4

#define BUFFER_SIZE 1024  // The general upper limit is ~1536 bytes
//#define BUFFER_SIZE 512  // The general upper limit is ~1536 bytes
#define LED_DELAY   100   // delay time in mS for LED cycling
#define TX_DELAY    20    // Time delay between serial data in uS

#endif // _NANO_DEF_H_
