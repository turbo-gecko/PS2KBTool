#ifndef _COMMANDS_H_
#define _COMMANDS_H_

/*
 * commands.h
 * 
 * Header file for the program mode commands
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

void displayHelp();
bool processCommand(const String cmdLine);
void sHostPrompt();

bool cKb101(const String param);
bool cSerialBaudRate(const String param);
bool cSerialCharDelay(const String param);
bool cSerialLineDelay(const String param);
bool cSerialFlowControl(const String param);
bool cSerialEnabled(const String param);

bool cEepromRead(const String param);
bool cEepromWrite(const String param);

#endif // _COMMANDS_H_
