#ifndef _COMMANDS.H_
#define _COMMANDS.H_

/*
 * commands.h
 * 
 * Header file for the program mode commands
 * 
 * This software is copyright 2021 by Gary Hammond (ZL3GH). It is
 * free to use for non-commercial use.
 * 
 * WARNING: Use of this software could result in a universe ending
 * paradox so use at your own risk. No warranties expressed or 
 * implied.
 */

void displayHelp();
bool processCommand(const String cmdLine);
void sHostPrompt();

bool cSerialBaudRate(const String param);
bool cSerialCharDelay(const String param);
bool cSerialLineDelay(const String param);
bool cSerialFlowControl(const String param);

bool cEepromRead(const String param);
bool cEepromWrite(const String param);

#endif // _COMMANDS.H_
