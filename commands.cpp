/*
 * commands.cpp
 * 
 * Command line commands.
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

#include <Arduino.h>
#include <EEPROM.h>

#include "globals.h"

#include "commands.h"
#include "eeprom_utils.h"
#include "serial_utils.h"

/*************************************************************************
 * Displays help text to the host serial port
 *************************************************************************/
void displayHelp()
{
  sHostPrintln("");
  S_HOST.println(F("PS2KB Tool Help"));
  S_HOST.println(F("The following commands are available:"));
  S_HOST.println(F("--Serial--"));
  S_HOST.println(F("sbr <baud>            - set host baud rate"));
  S_HOST.println(F("scd <mSec>            - set inter character delay"));
  S_HOST.println(F("sld <mSec>            - set inter line delay"));
  S_HOST.println(F("sfc <on|off>          - set XON/XOFF flow control"));
  S_HOST.println(F("sen <on|off>          - turn ON/OFF output to the serial port"));
  S_HOST.println(F("--Debug--"));
  S_HOST.println(F("reset                 - reset device"));
  S_HOST.println(F("ccrc                  - calculate EEPROM CRC"));
  S_HOST.println(F("scrc                  - display saved EEPROM CRC"));
  S_HOST.println(F("er <address>          - read value from EEPROM - address"));
  S_HOST.println(F("ew <address> <value>  - write value to EEPROM address"));
}

/*************************************************************************
 * Process the command line and calls the appropriate command function if
 * a valid command.
 * Returns true if command successful, otherwise false.
 *************************************************************************/
bool processCommand(const String cmdLine)
{
  String command = "";
  String param = "";

  // Split the command line up into the command and parameters
  int index = cmdLine.indexOf(" ");
  if (index > 1)
  {
    command = cmdLine.substring(0, index);
    param = cmdLine.substring(index + 1, cmdLine.length());
  }
  else
  {
    command = cmdLine;
  }
  
  if (command.equals("help"))
  {
    displayHelp();
    return true;
  }
  else if (command.equals("?"))
  {
    S_HOST.println(F("Serial: sbr,scd,sld,sfc,sen"));
    S_HOST.println(F("Debug : reset,ccrc,scrc,er,ew"));
    S_HOST.println(F("type 'help' for more detailed help"));
    return true;
  }
  // ************************* Serial Commands *********************************
  else if (command.equals("sbr"))
  {
    return cSerialBaudRate(param);
  }
  else if (command.equals("scd"))
  {
    return cSerialCharDelay(param);
  }
  else if (command.equals("sld"))
  {
    return cSerialLineDelay(param);
  }
  else if (command.equals("sfc"))
  {
    return cSerialFlowControl(param);
  }
  else if (command.equals("sen"))
  {
    return cSerialEnabled(param);
  }
  // ************************* Debug Commands **********************************
  else if (command.equals("ccrc"))
  {
    unsigned long crcCalc = eCrc();
    sHostPrintln("Calculated CRC = " + String(crcCalc, HEX));
    return true;
  }
  else if (command.equals("scrc"))
  {
    unsigned long crcSaved = 0;
    EEPROM.get(E_CHECKSUM, crcSaved);
    sHostPrintln("Saved CRC = " + String(crcSaved, HEX));
    return true;
  }
  else if (command.equals("er"))
  {
    return cEepromRead(param);
  }
  else if (command.equals("ew"))
  {
    return cEepromWrite(param);
  }
  else if (command.equals("reset"))
  {
    asm volatile ("  jmp 0"); 
    return true;
  }

  sHostPrintln("");
  sHostPrintln("command '" + command + "' not found");
  S_HOST.println(F("Type 'help' for a list of commands"));
  return false;
}

/*************************************************************************
 * Prints the current dirctory and prompt.
 *************************************************************************/
void sHostPrompt()
{
  sHostPrint(">");
}

/*************************************************************************
 * Commands.
 *************************************************************************/
bool cSerialBaudRate(const String param)
{
  if (param.length() > 0)
  {
    if (sHostBaudRate(param.toInt()))
    {
      S_HOST.end();
      S_HOST.begin(sHostGetBaudRate());
      return true;
    }
    else
    {
      S_HOST.println(F("Host baud rate not valid"));
      return false;
    }
  }
  else
  {
    sHostPrintln("Baud rate = " + String(sHostGetBaudRate(), DEC) + " bps");
    return true;
  }
}

//*************************************************************************
bool cSerialCharDelay(const String param)
{
  if (param.length() > 0)
  {
    if (param.toInt() == 0)
    {
      S_HOST.println(F("Inter character delay too small"));
      return false;
    }
    else
    {
      sHostCharDelay(param.toInt());
      return true;
    }
  }
  else
  {
    sHostPrintln("Character delay = " + String(sHostGetCharDelay(), DEC) + " mSec");
    return true;
  }
}

//*************************************************************************
bool cSerialLineDelay(const String param)
{
  if (param.length() > 0)
  {
    if (param.toInt() == 0)
    {
      S_HOST.println(F("Inter line delay too small"));
      return false;
    }
    else
    {
      sHostLineDelay(param.toInt());
      return true;
    }
  }
  else
  {
    sHostPrintln("Line delay = " + String(sHostGetLineDelay(), DEC) + " mSec");
    return true;
  }
}

//*************************************************************************
bool cSerialFlowControl(const String param)
{
  if (param.length() > 0)
  {
    if (param.equals("on"))
    {
      sHostXonXoff(true);
    }
    else
    {
      if (param.equals("off"))
      {
        sHostXonXoff(false);
      }
      else
      {
        sHostPrintln(param + " is invalid");
        S_HOST.println(F("Please Use either 'on' or 'off'"));
        return false;
      }
    }
  }
  else
  {
    if (sHostGetXonXoff())
    {
      S_HOST.println(F("Xon/Xoff flow control is enabled"));
    }
    else
    {
      S_HOST.println(F("Xon/Xoff flow control is disabled"));
    }
    return true;
  }
  return false; // We should never get here.
}

//*************************************************************************
bool cSerialEnabled(const String param)
{
  if (param.length() > 0)
  {
    if (param.equals("on"))
    {
      sHostEnabled(true);
    }
    else
    {
      if (param.equals("off"))
      {
        sHostEnabled(false);
      }
      else
      {
        sHostPrintln(param + " is invalid");
        S_HOST.println(F("Please Use either 'on' or 'off'"));
        return false;
      }
    }
  }
  else
  {
    if (sHostGetEnabled())
    {
      S_HOST.println(F("Serial output is enabled"));
    }
    else
    {
      S_HOST.println(F("Serial output is disabled"));
    }
    return true;
  }
  return false; // We should never get here.
}

//*************************************************************************
bool cEepromRead(const String param)
{
  if (param.length() > 0)
  {
    sHostPrintln("EEPROM Address " + param + " = " + String(EEPROM.read(param.toInt()), HEX));
    return true;
  }
  else
  {
    S_HOST.println(F("EEPROM address not specified"));
    return false;
  }
}

//*************************************************************************
bool cEepromWrite(const String param)
{
  if (param.length() > 0)
  {
    int index = param.indexOf(" ");
    if (index > 0)
    {
      String address = param.substring(0, index);
      String value = param.substring(index + 1, param.length());
      sHostPrintln("Writing to EEPROM Address " + address + " = " + value);
      EEPROM.write(address.toInt(), value.toInt());
      return true;
    }
    else
    {
      S_HOST.println(F("EEPROM value not specified"));
      return false;
    }
  }
  else
  {
    S_HOST.println(F("EEPROM address not specified"));
    return false;
  }
}
