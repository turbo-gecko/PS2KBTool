/*
 * commands.cpp
 * 
 * Command line commands.
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

#include <Arduino.h>
#include <EEPROM.h>

#include "globals.h"

#include "commands.h"
#include "eeprom_utils.h"
#include "keyboard.h"
#include "serial_utils.h"

/*************************************************************************
 * Displays help text to the host serial port
 *************************************************************************/
void displayHelp()
{
  S_HOST.println("");
  S_HOST.print(F("PS2KB Tool - v"));
  S_HOST.println(VERSION);
  S_HOST.println(F(T_HELP_01));
  S_HOST.println(F(T_HELP_02));
  S_HOST.println(F(T_HELP_03));
  S_HOST.println(F(T_HELP_04));
  S_HOST.println(F(T_HELP_05));
  S_HOST.println(F(T_HELP_06));
  S_HOST.println(F(T_HELP_07));
  S_HOST.println(F(T_HELP_08));
  S_HOST.println(F(T_HELP_09));
  S_HOST.println(F(T_HELP_10));
  S_HOST.println(F(T_HELP_11));
  S_HOST.println(F(T_HELP_12));
  S_HOST.println(F(T_HELP_13));
  S_HOST.println(F(T_HELP_14));
  S_HOST.println(F(T_HELP_15));
  S_HOST.println(F(T_HELP_16));
  S_HOST.println(F(T_HELP_17));
  S_HOST.println(F(T_HELP_18));
  S_HOST.println(F(T_HELP_19));
  S_HOST.println(F(T_HELP_20));
  S_HOST.println(F(T_HELP_21));
  S_HOST.println(F(T_HELP_22));
  S_HOST.println(F(T_HELP_23));
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
  
  if (command.equals(T_HELP))
  {
    displayHelp();
    return true;
  }
  else if (command.equals("?"))
  {
    S_HOST.println(F(T_HELP_40));
    S_HOST.println(F(T_HELP_41));
    S_HOST.println(F(T_HELP_42));
    S_HOST.println(F(T_HELP_43));
    return true;
  }
  // ************************* Keyboard Commands *********************************
  else if (command.equals("kbt"))
  {
    return cKbBoardType(param);
  }
  else if (command.equals("k101"))
  {
    return cKb101(param);
  }
  else if (command.equals("kabd"))
  {
    return cKbTimings(param, 1);
  }
  else if (command.equals("kand"))
  {
    return cKbTimings(param, 2);
  }
  else if (command.equals("kasd"))
  {
    return cKbTimings(param, 3);
  }
  else if (command.equals("kxbd"))
  {
    return cKbTimings(param, 4);
  }
  else if (command.equals("kxnd"))
  {
    return cKbTimings(param, 5);
  }
  else if (command.equals("kxsd"))
  {
    return cKbTimings(param, 6);
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
    unsigned long crc_calc = eCrc();
    S_HOST.println(T_MSG_01 + String(crc_calc, HEX));
    return true;
  }
  else if (command.equals("scrc"))
  {
    unsigned long crc_saved = 0;
    EEPROM.get(E_CHECKSUM, crc_saved);
    S_HOST.println(T_MSG_02 + String(crc_saved, HEX));
    return true;
  }
  else if (command.equals("ep"))
  {
    for (int i = 0; i < E_END_ADDRESS; i++)
    {
      if (EEPROM.read(i) < 16)
      {
        S_HOST.print("0");
      }
      S_HOST.print(String(EEPROM.read(i), HEX) + " ");
    }
    S_HOST.println("");
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

  S_HOST.println("");
  S_HOST.println(T_MSG_03);
  S_HOST.println(F(T_MSG_04));
  return false;
}

/*************************************************************************
 * Prints the current dirctory and prompt.
 *************************************************************************/
void sHostPrompt()
{
  S_HOST.print(">");
}

/*************************************************************************
 * Commands.
 *************************************************************************/
//*************************************************************************
//*************************************************************************
bool cKbBoardType(const String param)
{
  if (param.length() > 0)
  {
    if ((param.toInt() < 1) | (param.toInt() >= B_LAST))
    {
      S_HOST.println(F(T_MSG_05));
      return false;
    }
    else
    {
      kBoardType(param.toInt());
      return true;
    }
  }
  else
  {
    S_HOST.println(T_MSG_06 + String(kGetBoardType(), DEC));
    return true;
  }
}

bool cKb101(const String param)
{
  if (param.length() > 0)
  {
    if (param.equals(T_ON))
    {
      k101Enabled(true);
    }
    else
    {
      if (param.equals(T_OFF))
      {
        k101Enabled(false);
      }
      else
      {
        S_HOST.println(param + T_IS_INVALID);
        S_HOST.println(F(T_ON_OR_OFF));
        return false;
      }
    }
  }
  else
  {
    if (kGet101Enabled())
    {
      S_HOST.println(F(T_MSG_08));
    }
    else
    {
      S_HOST.println(F(T_MSG_09));
    }
    return true;
  }
  return false; // We should never get here.
}

bool cKbTimings(const String param, byte item)
{
  if (param.length() > 0)
  {
    kDelayTimings(param.toInt(), item);
  }
  else
  {
    switch(item)
    {
      case 1:
        S_HOST.println(T_MSG_10 + String(kGetDelayTimings(item), DEC));
        break;
      case 2:
        S_HOST.println(T_MSG_11 + String(kGetDelayTimings(item), DEC));
        break;
      case 3:
        S_HOST.println(T_MSG_12 + String(kGetDelayTimings(item), DEC));
        break;
      case 4:
        S_HOST.println(T_MSG_13 + String(kGetDelayTimings(item), DEC));
        break;
      case 5:
        S_HOST.println(T_MSG_14 + String(kGetDelayTimings(item), DEC));
        break;
      case 6:
        S_HOST.println(T_MSG_15 + String(kGetDelayTimings(item), DEC));
        break;
      default:
        return false; // We should never get here.
        break;
    }
  }
  return true;
}

//*************************************************************************
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
      S_HOST.println(F(T_MSG_16));
      return false;
    }
  }
  else
  {
    S_HOST.println(T_MSG_17 + String(sHostGetBaudRate(), DEC) + " bps");
    return true;
  }
}

//*************************************************************************
bool cSerialCharDelay(const String param)
{
  if (param.length() > 0)
  {
    if (param.toInt() < 0)
    {
      S_HOST.println(F(T_MSG_18));
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
    S_HOST.println(T_MSG_19 + String(sHostGetCharDelay(), DEC) + " mSec");
    return true;
  }
}

//*************************************************************************
bool cSerialLineDelay(const String param)
{
  if (param.length() > 0)
  {
    if (param.toInt() < 0)
    {
      S_HOST.println(F(T_MSG_20));
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
    S_HOST.println(T_MSG_21 + String(sHostGetLineDelay(), DEC) + " mSec");
    return true;
  }
}

//*************************************************************************
bool cSerialFlowControl(const String param)
{
  if (param.length() > 0)
  {
    if (param.equals(T_ON))
    {
      sHostXonXoff(true);
    }
    else
    {
      if (param.equals(T_OFF))
      {
        sHostXonXoff(false);
      }
      else
      {
        S_HOST.println(param + T_IS_INVALID);
        S_HOST.println(F(T_ON_OR_OFF));
        return false;
      }
    }
  }
  else
  {
    if (sHostGetXonXoff())
    {
      S_HOST.println(F(T_MSG_22));
    }
    else
    {
      S_HOST.println(F(T_MSG_23));
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
    if (param.equals(T_ON))
    {
      sHostEnabled(true);
    }
    else
    {
      if (param.equals(T_OFF))
      {
        sHostEnabled(false);
      }
      else
      {
        S_HOST.println(param + T_IS_INVALID);
        S_HOST.println(F(T_ON_OR_OFF));
        return false;
      }
    }
  }
  else
  {
    if (sHostGetEnabled())
    {
      S_HOST.println(F(T_MSG_24));
    }
    else
    {
      S_HOST.println(F(T_MSG_25));
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
    S_HOST.println(T_MSG_26 + param + " = " + String(EEPROM.read(param.toInt()), HEX));
    return true;
  }
  else
  {
    S_HOST.println(F(T_MSG_27));
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
      S_HOST.println(T_MSG_28 + address + " = " + value);
      EEPROM.write(address.toInt(), value.toInt());
      return true;
    }
    else
    {
      S_HOST.println(F(T_MSG_29));
      return false;
    }
  }
  else
  {
    S_HOST.println(F(T_MSG_27));
    return false;
  }
}
