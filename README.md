# PS2KBTool
The PS2KBTool is an AT to XT keyboard protocol converter that comes in 2 versions being the developer edition and the mini.

![alt text](https://github.com/turbo-gecko/PS2KBTool/blob/main/Photos/v1-pcbs.png "Boards")

## Why?
I wanted a protocol converter for my NuXT that was easy to modify without having to remove a chip, reprogram it and re-insert the chip risking damage.

## Features
- Easy to modify using inexpensive Arduino Nano and associated development tools.
- Serial debugging interface that is able to be turned on and off without reprogramming.
- EEPROM style settings to enable changes to the program's settings without having to reprogram the Nano. Settings include:
  - Run time configuration of board type via EEPROM value.
  - Serial communications parameters.
  - Keyboard interface timing delays for keyboards and computers that have special timing requirements.
  - EEPROM reading and writing.
- Developer addition includes:
  - Wide variety of connector options.
  - 4 additional config switches.
  - Test points for the interface signals.
  - Serial header.
  - 3.5" floppy drive power header.
  - 5 LED's for status of the interface lines.

## Breadboard version
The converter can be wired up on a bread board as follows:

![alt text](https://github.com/turbo-gecko/PS2KBTool/blob/main/Fritzing/PS2KB%20Tool_bb.png "Bread Board")

The LED's are optional and can be omitted for normal usage. Set the board type to 1 for no LED's and 2 with LED's via programming mode.

Schematic is at https://github.com/turbo-gecko/PS2KBTool/blob/main/Fritzing/PS2KB%20Tool_schem.png

## Getting Started
** Warning ** - When connecting the converter board to a second PC for reprogramming the Nano or changing system settings, there is a risk that the 2 computers may be at different ground potentials and damage to either or both could occur when connected this way. Whilst unlikely, it has been known to happen. I use and recommend to others to use a USB isolator between the Nano and the computer connected to its USB port.

Copy all of the .c, .h files and the PS2KBTool.ino file to a folder for use with the Arduino development tools. I am expecting that people already know how to setup and use the arduino development tools, and if not, there are a multitude of resources on the Internet explaining how to do this.

Setup Arduino for the Nano you are using and program the Nano with this software. The first time this is done, the EEPROM contents are set to a default set of values which are known to work.
Optional changes to make:
- Set the board type. It defaults to the standard mini which has no LED's. Set it to 2 if your board has LED's.
- Change the serial baud rate for programming mode and debugging.
- Turn off the serial debugging. Note that the serial debug does not appear to impact the timing of the signals on the keyboard interface lines.

To check how well the keyboard is working, I have written a keyboard diagnostics tool for XT class computers that can be downloaded from https://github.com/turbo-gecko/Keyboard-Tester
## Programming Mode
Open a serial terminal and set the baud rate to 115,200 with no flow control.
To enable programming mode via the Nano's USB port:
- On the mini or dev board, turn on DIP switch 1.
- On the bread board version, press and hold the prog button whilst also resetting the Nano, until you see 'Programming mode...' in the serial terminal. This may take up to 5 seconds.

When the 'Programming mode prompt appears, type 'help' and press enter for a list of available commands.
```
PS2KB Tool - v01.00.00
The following commands are available:
--Keyboard--
kbt <1|2|3>           - set converter board type
k101 <on|off>         - set enhanced 101 keys
kabd <uSec>           - set AT bit delay
kand <uSec>           - set AT next byte delay
kasd <uSec>           - set AT start bit delay
kxbd <uSec>           - set XT bit delay
kxnd <uSec>           - set XT next byte delay
kxsd <uSec>           - set XT start bit delay
--Serial--
sbr <baud>            - set host baud rate
scd <mSec>            - set inter character delay
sld <mSec>            - set inter line delay
sfc <on|off>          - set XON/XOFF flow control
sen <on|off>          - turn ON/OFF output to the serial port
--Debug--
reset                 - reset device
ccrc                  - calculate EEPROM CRC
scrc                  - display saved EEPROM CRC
ep                    - print all EEPROM values
er <address>          - read value from EEPROM address
ew <address> <value>  - write value to EEPROM address
```

## Serial Debug
Here is an example debug session...
```
[A:ff][A:ed][A:2][A:ed][A:4][A:ed][A:1][A:ed][A:0]

fa <ACK>

e0      72/50[X:50]     e0      f0      72/d0[X:d0]
e0      72/50[X:50]     e0      f0      72/d0[X:d0]
e0      72/50[X:50]     e0      f0      72/d0[X:d0]
e0      72/50[X:50]     e0      f0      72/d0[X:d0]
5a/1c[X:1c]     f0      5a/9c[X:9c]
25/5[X:5]       f0      25/85[X:85]
5/3b[X:3b]      f0      5/bb[X:bb]
58/3a[X:3a]     f0      58/ba[X:ba][A:ed][A:4]
58/3a[X:3a]     f0      58/ba[X:ba][A:ed][A:0]
```
### Legend:
- nn Untranslated scan code.
- xx/yy AT scan code and its translated XT equivalent.
- [X:nn] Scan code sent via the XT interface.
- [A:nn] Byte sent via the AT interface.
- <...> Decoded non-scan code from the keyboard.
- Each line that begins with an AT scan code is a key press sequence.
- Tabs indicate seperate scan codes.

In the example above:
- The first line is the sequence sent to the keyboard to briefly flash each of the indicator LED's. This is part of the programs boot sequence and is a visual indicator that it has rebooted.
- The fa <ACK> is an acknowledgement back from the keyboard.
- The next 4 lines beginning with e0 are 4 presses of the extended (inverted T) down arrow key.
- 5a sequence is pressing the <Enter> key followed by releasing the <Enter> key. 
- 25 sequence is pressing the number 4 key followed by releasing the key.
- 5 if the pressing and releasing of function key F1
- The first 58 sequence pressing/releasing the caps lock key with the trailing [A:ed][A:4] being the command to the keyboard to turn on the caps lock LED.
- The second 58 sequence pressing/releasing the caps lock key with the trailing [A:ed][A:0] being the command to the keyboard to turn off the caps lock LED.

## Known issues
- The SysReq and Break key scan codes have been disabled and will not be passed through to the computer.

## To Do
- Write up a LOT more documentation.
  - How the EEPROM code works and what needs to be changed when adding new values.
  - How the translation tables work.
  - How the code main logic works, although the code does have some level of commenting.
- Finish version 2 of the developer edition and mini boards.
 - Upload the KiCad files when done.
- Work with someone on seeing if changes are needed for foreign language keyboards.
- Add a macro feature for the AT scan codes that the XT doesn't recognise.

