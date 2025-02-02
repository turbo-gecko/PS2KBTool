#ifndef _ENGLISH_H_
#define _ENGLISH_H_

/*
 * english.h
 * 
 * English language text definitions.
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

#define T_HELP              "help"
#define T_IS_INVALID        " is invalid"
#define T_OFF               "off"
#define T_ON                "on"

#define T_ON_OR_OFF         "Please Use either 'on' or 'off'"
#define T_PROG_MODE         "Programming mode..."

#define T_HELP_01           "The following commands are available:"
#define T_HELP_02           "--Keyboard--"
#define T_HELP_03           "kbt <1|2|3>           - set converter board type"
#define T_HELP_04           "k101 <on|off>         - set enhanced 101 keys"
#define T_HELP_05           "kabd <uSec>           - set AT bit delay"
#define T_HELP_06           "kand <uSec>           - set AT next byte delay"
#define T_HELP_07           "kasd <uSec>           - set AT start bit delay"
#define T_HELP_08           "kxbd <uSec>           - set XT bit delay"
#define T_HELP_09           "kxnd <uSec>           - set XT next byte delay"
#define T_HELP_10           "kxsd <uSec>           - set XT start bit delay"
#define T_HELP_11           "--Serial--"
#define T_HELP_12           "sbr <baud>            - set host baud rate"
#define T_HELP_13           "scd <mSec>            - set inter character delay"
#define T_HELP_14           "sld <mSec>            - set inter line delay"
#define T_HELP_15           "sfc <on|off>          - set XON/XOFF flow control"
#define T_HELP_16           "sen <on|off>          - turn ON/OFF output to the serial port"
#define T_HELP_17           "--Debug--"
#define T_HELP_18           "reset                 - reset device"
#define T_HELP_19           "ccrc                  - calculate EEPROM CRC"
#define T_HELP_20           "scrc                  - display saved EEPROM CRC"
#define T_HELP_21           "ep                    - print all EEPROM values"
#define T_HELP_22           "er <address>          - read value from EEPROM address"
#define T_HELP_23           "ew <address> <value>  - write value to EEPROM address"

#define T_HELP_40           "Keyboard: kbt,k101,kabd,kand,kasd,kxbd,kxnd,kxsd"
#define T_HELP_41           "Serial  : sbr,scd,sld,sfc,sen"
#define T_HELP_42           "Debug   : reset,ccrc,scrc,ep,er,ew"
#define T_HELP_43           "type 'help' for more detailed help"

#define T_MSG_01            "Calculated CRC = "
#define T_MSG_02            "Saved CRC = "
#define T_MSG_03            "command '" + command + "' not found"
#define T_MSG_04            "Type 'help' for a list of commands"
#define T_MSG_05            "Invalid board type"
#define T_MSG_06            "Board type = "
//#define T_MSG_07            ""
#define T_MSG_08            "101 keys is enabled"
#define T_MSG_09            "101 keys is disabled"
#define T_MSG_10            "AT Bit Delay = "
#define T_MSG_11            "AT Next Byte Delay = "
#define T_MSG_12            "AT Start Bit Delay = "
#define T_MSG_13            "XT Bit Delay = "
#define T_MSG_14            "XT Next Byte Delay = "
#define T_MSG_15            "XT Start Bit Delay = "
#define T_MSG_16            "Host baud rate not valid"
#define T_MSG_17            "Baud rate = "
#define T_MSG_18            "Inter character delay too small"
#define T_MSG_19            "Inter character delay = "
#define T_MSG_20            "Inter line delay too small"
#define T_MSG_21            "Inter line delay = "
#define T_MSG_22            "Xon/Xoff flow control is enabled"
#define T_MSG_23            "Xon/Xoff flow control is disabled"
#define T_MSG_24            "Serial output is enabled"
#define T_MSG_25            "Serial output is disabled"
#define T_MSG_26            "EEPROM Address "
#define T_MSG_27            "EEPROM address not specified"
#define T_MSG_28            "Writing to EEPROM Address "
#define T_MSG_29            "EEPROM value not specified"

#endif // _ENGLISH_H_
