/*
 * serial_utils.cpp
 * 
 * Serial helper functions.
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

#include "eeprom_utils.h"
#include "globals.h"
#include "serial_utils.h"

bool control_c             = false;

byte in_byte;
byte flow_control         = S_DEF_XON_XOFF;

unsigned int char_delay   = S_DEF_CHAR_DELAY;
unsigned int line_delay   = S_DEF_LINE_DELAY;
unsigned long baud_rate   = S_DEF_HOST_BAUD;

//*************************************************************************
bool sHostBaudRate(const unsigned long value) {
  EEPROM.get(E_HOST_BAUD, baud_rate);
  if (baud_rate != value)
  {
    if (value == 115200 ||
        value == 57600 ||
        value == 38400 ||
        value == 19200 ||
        value == 9600 ||
        value == 4800 ||
        value == 2400 ||
        value == 1200 ||
        value == 600 ||
        value == 300) {
      baud_rate = value;
      EEPROM.put(E_HOST_BAUD, value);
      eUpdateCrc();
      return true;
    }
    else {
      return false;
    }
  }
  return true;
}

//*************************************************************************
unsigned long sHostGetBaudRate() {
  EEPROM.get(E_HOST_BAUD, baud_rate);
  return baud_rate;
}

//*************************************************************************
void sHostCharDelay(const unsigned int value) {
  EEPROM.get(E_CHAR_DELAY, char_delay);
  if (char_delay != value) {
    char_delay = value;
    EEPROM.put(E_CHAR_DELAY, value);
    eUpdateCrc();
  }
}

//*************************************************************************
unsigned int sHostGetCharDelay() {
  EEPROM.get(E_CHAR_DELAY, char_delay);
  return char_delay;
}

//*************************************************************************
void sHostLineDelay(const unsigned int value) {
  EEPROM.get(E_LINE_DELAY, line_delay);
  if (line_delay != value) {
    line_delay = value;
    EEPROM.put(E_LINE_DELAY, value);
    eUpdateCrc();
  }
}

//*************************************************************************
unsigned int sHostGetLineDelay() {
  EEPROM.get(E_LINE_DELAY, line_delay);
  return line_delay;
}

//*************************************************************************
bool sHostPrint(const String &message) {
  control_c = false;
  for (int i = 0; i < message.length(); i++) {
    if (S_HOST.available() > 0) {
      in_byte = sHostRead();
      if (flow_control > 0 && in_byte == XOFF) {
        while (in_byte != XON) {
          if (S_HOST.available() > 0) {
            in_byte = sHostRead();
            if (in_byte == CTRL_C) {
              control_c = true;
            }
          }
        }
      }
      else if (in_byte == CTRL_C) {
        control_c = true;
      }
    }
    if (control_c) {
      return false;
    }
    S_HOST.print(message.charAt(i));
    delay(char_delay);
  }
  return true;
}

//*************************************************************************
bool sHostPrintln(const String &message) {
  control_c = false;
  for (int i = 0; i < message.length(); i++) {
    if (S_HOST.available() > 0) {
      in_byte = sHostRead();
      if (flow_control > 0 && in_byte == XOFF) {
        while (in_byte != XON) {
          if (S_HOST.available() > 0) {
            in_byte = sHostRead();
            if (in_byte == CTRL_C) {
              control_c = true;
            }
          }
        }
      }
      else if (in_byte == CTRL_C) {
        control_c = true;
      }
    }
    if (control_c) {
      return false;
    }
    S_HOST.print(message.charAt(i));
    delay(char_delay);
  }
  S_HOST.println(""); 
  delay(line_delay);
  return true;
}

//*************************************************************************
char sHostRead() {
  static char byte_read;
  byte_read = S_HOST.read();
  return byte_read;
}

//*************************************************************************
void sHostXonXoff(const bool value) {
  if (value) {
    flow_control = 1;
  }
  else {
    flow_control = 0;
  }
  EEPROM.put(E_XON_XOFF, flow_control);
  eUpdateCrc();
}

//*************************************************************************
bool sHostGetXonXoff() {
  EEPROM.get(E_XON_XOFF, flow_control);
  if (flow_control == 0) {
    return false;
  }
  else {
    return true;
  }
}
