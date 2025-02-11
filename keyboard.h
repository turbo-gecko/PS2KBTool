#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

/*
 * keyboard.h
 * 
 * Keyboard scan code helper functions.
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

byte AT2XT(byte);
byte AT2XTExt(byte);
byte AT2XTExtNav(byte);
byte AT2XTExtStrip(byte);

void kBoardType(const unsigned int value);
unsigned int kGetBoardType();
void kDelayTimings(byte value, byte item);
byte kGetDelayTimings(byte item);

void k101Enabled(const bool value);
bool kGet101Enabled();

#endif // _KEYBOARD_H_
