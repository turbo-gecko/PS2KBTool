#ifndef _SERIAL_UTILS_H_
#define _SERIAL_UTILS_H_

/*
 * serial_utils.h
 * 
 * Serial helper functions.
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

/*************************************************************************
 * sHostBaudRate
 * 
 * Updates the host serial port baud rate.
 * The baud rates are restricted to 115200, 57600, 38400, 19200, 9600, 
 * 4800, 2400, 1200, 600 and 300 bps.
 *
 * Returns true if successfully set the baud rate to a valid value,
 * otherwise returns false.
 *************************************************************************/
bool sHostBaudRate(const unsigned long value);

/*************************************************************************
 * sHostGetCharDelay
 * 
 * Returns the host serial port baud rate..
 *************************************************************************/
unsigned long sHostGetBaudRate();

/*************************************************************************
 * sHostCharDelay
 * 
 * Updates the time delay in mSecs between each character sent via the
 * sHostPrint() and sHostPrintln() functions.
 *
 * On a model 100 using the TELCOM software, a minimum of 15 mSec delay is
 * required to receive all characters sent. Any shorter time period can
 * result in dropped characters.
 *************************************************************************/
void sHostCharDelay(const unsigned int value);

/*************************************************************************
 * sHostGetCharDelay
 * 
 * Returns the time delay in mSecs between each character sent via the
 * sHostPrint() and sHostPrintln() functions.
 *************************************************************************/
unsigned int sHostGetCharDelay();

/*************************************************************************
 * sHostLineDelay
 * 
 * Updates the time delay in mSecs at the end of a line of text sent via
 * the sHostPrintln() function.
 *
 * On a model 100 using the TELCOM software, a minimum of 150 mSec delay is
 * required to receive all characters sent. Any shorter time period can
 * result in the final carriage return not being received.
 *************************************************************************/
void sHostLineDelay(const unsigned int value);

/*************************************************************************
 * sHostGetLineDelay
 * 
 * Returns the time delay in mSecs between each line of text sent via
 * the sHostPrintln() function.
 *************************************************************************/
unsigned int sHostGetLineDelay();

/*************************************************************************
 * sHostPrint
 * 
 * Sends the text specified by 'message' to the host serial port.
 * Toggles the TXD activity LED for the duration of sending of the message.
 * Returns false of a <ctrl>-c is received during sending, otherwise
 * returns true.
 *************************************************************************/
bool sHostPrint(const String &message);

/*************************************************************************
 * sHostPrintln
 * 
 * Sends the text specified by 'message' to the host serial port with a
 * trailing carriage return.
 * Toggles the TXD activity LED for the duration of sending of the message.
 * Returns false of a <ctrl>-c is received during sending, otherwise
 * returns true.
 *************************************************************************/
bool sHostPrintln(const String &message);

/*************************************************************************
 * sHostRead
 * 
 * Reads a single character from the host serial port and returns the
 * character to the caller.
 * Toggles the RXD activity LED for the duration of receiving of the
 * character.
 *************************************************************************/
char sHostRead();

/*************************************************************************
 * sHostXonXoff
 * 
 * Turns on or off the Xon Xoff flow control. Pass in 'true' to turn on
 * or 'false' to turn off.
 *************************************************************************/
void sHostXonXoff(const bool value);

/*************************************************************************
 * sHostGetXonXoff
 * 
 * Returns the state of the Xon Xoff flow control.
 *************************************************************************/
bool sHostGetXonXoff();

/*************************************************************************
 * sHostEnabled
 * 
 * Turns on or off serial communications. Pass in 'true' to turn on
 * or 'false' to turn off.
 *************************************************************************/
void sHostEnabled(const bool value);

/*************************************************************************
 * sHostEnabled
 * 
 * Returns the state of the serial communications.
 *************************************************************************/
bool sHostGetEnabled();

#endif // _SERIAL_UTILS_H_
