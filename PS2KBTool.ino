/*
 * PS2KBTool
 * 
 * Uses an Arduino Nano to provide PS2/AT keyboard scan code
 * modifications to an XT keyboard interface.
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

//*************************************************************************

#include "globals.h"

#include "commands.h"
#include "eeprom_utils.h"
#include "keyboard.h"
#include "serial_utils.h"

/*************************************************************************
 * Variables
 *************************************************************************/
bool program_mode       = false;

bool at_clk_busy        = false;
bool at_data_bit        = false;
bool at_data_ready      = false;
bool at_data_printed    = false;
bool break_key_pressed  = false;
bool ext_101_enabled    = false;
bool ext_pressed        = false;
bool ext_nav_pressed    = false;
bool ext_strip_pressed  = false;
bool isr_disabled       = false;
bool key_release        = false;
bool serial_enabled     = false;
bool sysreq_key_pressed = false;

bool caps_lock          = false;
bool num_lock           = false;
bool scroll_lock        = false;

char rx_byte            = 0;
char tx_byte            = 0;

String host_command     = "";
String last_command     = "";

byte at_clk_count       = 0;
byte at_clk_prev        = 0;
byte at_data_byte       = 0;
byte at_data_prev       = 0;
byte at_data_temp       = 0;
byte xt_data_byte       = 0;

byte kb_leds            = 0;
byte kb_leds_prev       = 0;

unsigned int at_timeout = 0;
unsigned int board_type = 0;
unsigned int count      = 0;
unsigned int temp       = 0;

struct kb_timings
{
  byte at_bit_delay;
  byte at_next_delay;
  byte at_start_delay;
  byte xt_bit_delay;
  byte xt_next_delay;
  byte xt_start_delay;
} kbt;

/*************************************************************************
 * Macro's
 *************************************************************************/
#define LOG(x) if (serial_enabled){sHostPrint((x));}
#define LOG_HEX(x) if (serial_enabled){sHostPrint(String((x), HEX));}

/*************************************************************************
 * Setup
 *************************************************************************/
void setup()
{
  // Initialise EEPROM
  eInit();

  // Stop KB from sending data
  pinMode(AT_CLK, OUTPUT);
  digitalWrite(AT_CLK, LOW);

  // Initialise input pins
  pinMode(AT_DATA, INPUT_PULLUP);
  pinMode(XT_CLK, INPUT_PULLUP);
  pinMode(XT_DATA, INPUT_PULLUP);
  pinMode(CONFIG_1, INPUT_PULLUP);

  // Initialise unused digital pins
  pinMode(D2, INPUT_PULLUP);
  pinMode(D14, INPUT_PULLUP);
  pinMode(D15, INPUT_PULLUP);

  // Initialise output pins
  pinMode(LED_NANO, OUTPUT);

  // Get the board type from EEPROM
  board_type = kGetBoardType();

  // Perform board specific initialisation
  switch(board_type)
  {
    case B_DEV:
      // Initialise output pins for the LED's
      pinMode(LED_AT_CLK, OUTPUT);
      pinMode(LED_AT_DATA, OUTPUT);
      pinMode(LED_XT_CLK, OUTPUT);
      pinMode(LED_XT_DATA, OUTPUT);
      pinMode(LED_DEV, OUTPUT);

      // Flash the LED's
      digitalWrite(LED_AT_CLK, HIGH);
      digitalWrite(LED_AT_DATA, HIGH);
      digitalWrite(LED_XT_CLK, HIGH);
      digitalWrite(LED_XT_DATA, HIGH);
      digitalWrite(LED_DEV, HIGH);

      delay (500);

      digitalWrite(LED_AT_CLK, LOW);
      digitalWrite(LED_AT_DATA, LOW);
      digitalWrite(LED_XT_CLK, LOW);
      digitalWrite(LED_XT_DATA, LOW);
      digitalWrite(LED_DEV, LOW);
      break;

    case B_STD:
      // Initialise the LED outputs as unused pins
      pinMode(LED_AT_CLK, INPUT_PULLUP);
      pinMode(LED_AT_DATA, INPUT_PULLUP);
      pinMode(LED_XT_CLK, INPUT_PULLUP);
      pinMode(LED_XT_DATA, INPUT_PULLUP);
      pinMode(LED_DEV, INPUT_PULLUP);
      break;

    case B_NUXT:
      // Initialise the LED outputs as unused pins
      pinMode(LED_AT_CLK, INPUT_PULLUP);
      pinMode(LED_AT_DATA, INPUT_PULLUP);
      pinMode(LED_XT_CLK, INPUT_PULLUP);
      pinMode(LED_XT_DATA, INPUT_PULLUP);
      pinMode(LED_DEV, INPUT_PULLUP);
      break;

    default:
      break;
  }

  // Get keyboard delay timings from EEPROM
  kbt.at_bit_delay = kGetDelayTimings(1);
  kbt.at_next_delay = kGetDelayTimings(2);
  kbt.at_start_delay = kGetDelayTimings(3);
  kbt.xt_bit_delay = kGetDelayTimings(4);
  kbt.xt_next_delay = kGetDelayTimings(5);
  kbt.xt_start_delay = kGetDelayTimings(6);

  // Read config DIP switch 1
  temp = digitalRead(CONFIG_1);
  if (temp == LOW) // Program mode selected
  {
    program_mode = true;

    digitalWrite(LED_NANO, HIGH);

    // Enable serial mode in case it has been disabled in the EEPROM
    serial_enabled = true;

    S_HOST.begin(sHostGetBaudRate());
    sHostPrintln(T_PROG_MODE);
  }
  else
  {
    program_mode = false;

    digitalWrite(LED_NANO, LOW);

    serial_enabled = sHostGetEnabled();

    if(board_type == B_DEV)
    {
      checkDevOptions();
    }

    if (serial_enabled)
    {
      // Initialise host serial port
      S_HOST.begin(sHostGetBaudRate());
    }

    // Enable the KB
    pinMode(AT_CLK, INPUT_PULLUP);
    delay(10);

    // Reset keyboard
    sendAtCode(0xFF);
    delay(500);

    // Cycle keyboard LED's
    sendAtCode(0xED);
    delay(kbt.at_next_delay);
    sendAtCode(0x02);
    delay(250);
    sendAtCode(0xED);
    delay(kbt.at_next_delay);
    sendAtCode(0x04);
    delay(250);
    sendAtCode(0xED);
    delay(kbt.at_next_delay);
    sendAtCode(0x01);
    delay(250);
    sendAtCode(0xED);
    delay(kbt.at_next_delay);
    sendAtCode(0x00);
    LOG ("\n");

    // Set up the interrupt for the AT_CLK line
    attachInterrupt(digitalPinToInterrupt(AT_CLK), INT1_ISR, FALLING);
    // Reset flags and counters
    at_data_ready = false;
    at_clk_count = 0;

    if(board_type == B_DEV)
    {
      // Turn off AT_CLK LED on DEV board
      digitalWrite(LED_AT_CLK, LOW);
    }
  }

  // Get extended 101 key enabled state
  ext_101_enabled = kGet101Enabled();
}

/*************************************************************************
 * The loop function runs over and over again forever
 *************************************************************************/
void loop()
{
  if (program_mode)
  {
    processCommands();
  }
  else
  {
    processKeyPress();
  }
}

/*************************************************************************
 * Sample function for use with the dev board
 *************************************************************************/
void checkDevOptions(void)
{
  if(board_type == B_DEV)
  {
    if (analogRead(A0) < 128)
    {
      devOptions1();
    }
    
    if (analogRead(A1) < 128)
    {
      devOptions2();
    }
    
    if (analogRead(A2) < 128)
    {
      devOptions3();
    }
    
    if (analogRead(A3) < 128)
    {
      devOptions4();
    }
  }
}

/*************************************************************************
 * Check for scan codes that are not key presses i.e., BAT, Acks etc.
 *************************************************************************/
bool checkSpecialCase(void)
{
  static bool ret_val = false;
  // Process data byte
  // Handle special cases that will not be passed through first
  switch(at_data_byte)
  {
    case 0xAA:
      // BAT from KB
      LOG ("\n");
      LOG_HEX(at_data_byte);
      LOG (" <BAT>\n\n");
      ret_val = true;
      break;

    case 0xF0:
      // Key release
      if (ext_101_enabled)
      {
        if (at_data_prev == 0xE0)
        {
          sendXtCode(at_data_prev);
          delayMicroseconds(kbt.xt_next_delay);
        }
      }
      LOG_HEX (at_data_byte);
      LOG ("\t");
      key_release = true;
      ret_val = true;
      break;

    case 0xFA:
      // Ack from KB
      LOG ("\n");
      LOG_HEX(at_data_byte);
      LOG (" <ACK>\n\n");
      ret_val = true;
      break;

    default:
      ret_val = false;
      break;
  }
  return ret_val;
}

/*************************************************************************
 * Sample function for use with the dev board
 *************************************************************************/
void devOptions1(void)
{
  return;
}

/*************************************************************************
 * Sample function for use with the dev board
 *************************************************************************/
void devOptions2(void)
{
  return;
}

/*************************************************************************
 * Sample function for use with the dev board
 *************************************************************************/
void devOptions3(void)
{
  return;
}

/*************************************************************************
 * Sample function for use with the dev board
 *************************************************************************/
void devOptions4(void)
{
  serial_enabled = true;
  return;
}

/*************************************************************************
 * Process the program mode commands
 *************************************************************************/
void processCommands(void)
{
  // Sample function call checking the additinal DIP switches on the DEV board
  if(board_type == B_DEV)
  {
    checkDevOptions();
  }

  // Process serial input from the host port
  if (S_HOST.available() > 0)
  {
    rx_byte = sHostRead();
    sHostPrint((const String) rx_byte);
    if (rx_byte == '\n' || rx_byte == '\r')
    {
      sHostPrintln("");
      if (host_command.length() > 0)
      {
        processCommand(host_command);
        last_command = host_command;
        host_command = "";
      } 
      sHostPrompt();
    }
    else
    {
      if (rx_byte == BACK_SPACE)
      {
        if (host_command.length() > 0)
        {
          sHostPrint(" ");
          S_HOST.write(BACK_SPACE);
          host_command.remove(host_command.length() - 1, 1);
        }
        else
        {
          sHostPrint(">");
        }
      }
      else
      {
        if (rx_byte == UP_ARROW)
        {
          host_command = host_command + last_command;
          sHostPrint(host_command);
        }
        else
        {
          host_command = host_command + (char) rx_byte;
        }
      }
    }
  }
}

/*************************************************************************
 * Process extended key sequence
 *************************************************************************/
void processExtKey(void)
{
  // Not the E0 12 ext sequence
  if (at_data_byte != 0x12)
  {
    xt_data_byte = AT2XTExt(at_data_byte);
    // Found a corresponding ext code
    if (xt_data_byte != 0x00)
    {
      ext_pressed = true;
      // Send the E0
      sendXtCode(at_data_prev);
      delayMicroseconds(kbt.xt_next_delay);
    }
    else
    {
      xt_data_byte = AT2XTExtNav(at_data_byte);
      // Found a corresponding ext nav code
      if (xt_data_byte != 0x00)
      {
        ext_nav_pressed = true;
        // Allow the ext keys?
        if (ext_101_enabled)
        {
          // Send the E0
          sendXtCode(at_data_prev);
          delayMicroseconds(kbt.xt_next_delay);
        }
        else
        {
          // Strip the E0
          ext_strip_pressed = true;
        }
      }
      else
      // See if the byte is one of the codes to have the E0 stripped
      {
        xt_data_byte = AT2XTExtStrip(at_data_byte);
        // Found a corresponding ext code
        if (xt_data_byte != 0x00)
        {
          // Strip the E0
          ext_strip_pressed = true;
        }
      }
    }
  }
  else
  {
    // PrintScreen/SysReq pressed
    sysreq_key_pressed = true;

    // Eat this key press as it is not a valid XT key
    xt_data_byte = 0;
  }

  if (at_data_byte == 0x7C && sysreq_key_pressed)
  {
    // Eat this key press as it is not a valid XT key
    xt_data_byte = 0;
  }

  LOG_HEX (xt_data_byte);
  // Found a valid scan code?
  if (xt_data_byte != 0x00)
  {
    // Send it!
    sendXtCode(xt_data_byte);
  }
}

/*************************************************************************
 * Main program logic for AT to XT scan code conversion
 *************************************************************************/
void processKeyPress(void)
{
  // Is there any data to process?
  if (at_data_ready)
  {
    // Check to see if the keyboard is finished sending
    isr_disabled = true;
    while(!digitalRead(AT_CLK));

    // Stop keyboard from sending more data
    pinMode(AT_CLK, OUTPUT);
    digitalWrite(AT_CLK, 0);
    
    // Handle special cases
    if (!checkSpecialCase())
    {
      // Not a special case
      LOG_HEX (at_data_byte);
      
      // Current scan code is not the ext code
      if (at_data_byte != 0xE0)
      {
        LOG("/");
        // Is the previous scan code the ext code?
        if (at_data_byte == 0xE1)
        {
          break_key_pressed = true;
        }

        if (at_data_prev == 0xE0)
        {
          processExtKey();
        }
        else
        {
          if (break_key_pressed)
          {
            xt_data_byte = 0;
          }
          else
          {
            xt_data_byte = AT2XT(at_data_byte);
            if(xt_data_byte == 0)
            {
              xt_data_byte = AT2XTExt(at_data_byte);
            }
          }

          processKeyRelease();

          LOG_HEX (xt_data_byte);
          // Do we have a valid scan code
          if (xt_data_byte != 0)
          {
            sendXtCode(xt_data_byte);
          }

          updateLedStatus();

          if (at_data_prev == 0xF0 && at_data_byte == 0x77 && break_key_pressed)
          {
              // Clear break key flag
              break_key_pressed = false;
          }
        }
      }
      LOG ("\t");
      if (key_release)
      {
        LOG ("\n");
        // Reset the flag
        key_release = false;
      }
    }

    // Update AT data states/values
    at_data_ready = false;
    at_data_printed = true;
    at_data_prev = at_data_byte;

    // Re-enable the keyboard to send data
    pinMode(AT_CLK, INPUT_PULLUP);
    isr_disabled = false;
  }
}

/*************************************************************************
 * Process key release actions
 *************************************************************************/
void processKeyRelease(void)
{
  // Key released
  if (at_data_prev == 0xF0 && !break_key_pressed)
  {
    // Is it an ext key?
    if (ext_pressed)
    {
      // Reset the flag
      ext_pressed = false;
      // If it should not be stripped
      if(!ext_strip_pressed)
      {
        // Send the E0
        sendXtCode(0xE0);
        delayMicroseconds(kbt.xt_next_delay);
      }
    }
    // Is it one of the ext navigation keys?
    if (ext_nav_pressed)
    {
      // Reset the flag
      ext_nav_pressed = false;
      // Are we allowed to send the extended key press?
      if (ext_101_enabled)
      {
        // Send the E0
        sendXtCode(0xE0);
        delayMicroseconds(kbt.xt_next_delay);
      }
    }

    // Check for (PrintScreen/SysReq)
    if (sysreq_key_pressed)
    {
      if (at_data_byte == 0x12)
      {
        sysreq_key_pressed = false;
      }
      // Eat this key press
      xt_data_byte = 0;
    }
    else
    {
      // Set up for key release XT scan code
      xt_data_byte = xt_data_byte + 0x80;
    }
  }
}

/*************************************************************************
 * Send AT code to keyboard
 *************************************************************************/
void sendAtCode(byte sac_code)
{
  static byte parity;
  parity = 0;
  
  // Check to see if we are processing incoming data from the AT port
  // and wait until it has completed.
  while (at_clk_busy);

  // Check to see if the AT clock is in use and if not, enable it.
  pinMode(AT_CLK, OUTPUT);
  digitalWrite(AT_CLK, HIGH);
  pinMode(AT_DATA, OUTPUT);
  digitalWrite(AT_DATA, HIGH);

  if(board_type == B_DEV)
  {
    digitalWrite(LED_AT_DATA, HIGH);
  }

  // Pull clock line low to take control of bus
  digitalWrite(AT_CLK, LOW);
  // Wait before pulling data line low
  delayMicroseconds(kbt.at_bit_delay);
  // Send start bit
  digitalWrite(AT_DATA, LOW);
  delayMicroseconds(kbt.at_bit_delay);
  // Release the AT clock and data.
  pinMode(AT_CLK, INPUT_PULLUP);
  // Finish sending start bit
  delayMicroseconds(kbt.at_bit_delay);

  // Send data.
  for(int count = 0; count <8; count++)
  {
    // Wait for the AT_CLK to go low
    while(digitalRead(AT_CLK));
    // Send the corresponding data bit
    if (bitRead(sac_code, count))
    {
      digitalWrite(AT_DATA, HIGH);
      // Update the parity counter
      parity++;
    }
    else
    {
      digitalWrite(AT_DATA, LOW);
    }
    // Allow time for keyboard to read the bit
    delayMicroseconds(kbt.at_bit_delay);
    delayMicroseconds(kbt.at_bit_delay);
  }

  // Wait for the AT_CLK line to go low
  while(digitalRead(AT_CLK));
  // Send the parity bit.
  parity = parity & 0x01;
  if(parity == 0)
  {
    digitalWrite(AT_DATA, HIGH);
  }
  else
  {
    digitalWrite(AT_DATA, LOW);
  }
  // Allow time for the keyboard to read the bit
  delayMicroseconds(kbt.at_bit_delay);
  delayMicroseconds(kbt.at_bit_delay);

  if(board_type == B_DEV)
  {
    digitalWrite(LED_AT_DATA, LOW);
  }

  // Release the AT clock and data.
  pinMode(AT_CLK, INPUT_PULLUP);
  pinMode(AT_DATA, INPUT_PULLUP);

  LOG ("[A:");
  LOG_HEX (sac_code);
  LOG ("]");
}

/*************************************************************************
 * Send XT code to computer
 *************************************************************************/
void sendXtCode(byte sxc_code)
{
  // Check to see if we are processing incoming data from the AT port
  // and wait until it has completed.
  while (at_clk_busy); //incoming data so wait

  if(board_type == B_DEV)
  {
    digitalWrite(LED_XT_CLK, HIGH);
    digitalWrite(LED_XT_DATA, HIGH);
  }

  // Check to see if the XT clock is in use and if not, enable it.
  pinMode(XT_CLK, OUTPUT);
  digitalWrite(XT_CLK, HIGH);
  pinMode(XT_DATA, OUTPUT);
  digitalWrite(XT_DATA, HIGH);
  // Wait before clocking out the data
  delayMicroseconds(kbt.xt_start_delay);

  // Clock out the start bit
  digitalWrite(XT_CLK, LOW);
  delayMicroseconds(kbt.xt_bit_delay);
  digitalWrite(XT_CLK, HIGH);
  delayMicroseconds(kbt.xt_bit_delay);

  // Send the data bits
  for(int count = 0; count < 8; count++)
  {
    digitalWrite(XT_DATA, bitRead(sxc_code, count));
    delayMicroseconds(kbt.xt_start_delay);
    digitalWrite(XT_CLK, LOW);
    delayMicroseconds(kbt.xt_bit_delay);
    digitalWrite(XT_CLK, HIGH);
    delayMicroseconds(kbt.xt_bit_delay);
  }

  // Send the stop bit
  digitalWrite(XT_DATA, LOW);
  delayMicroseconds(kbt.xt_start_delay);
  digitalWrite(XT_CLK, LOW);
  delayMicroseconds(kbt.xt_bit_delay);
  digitalWrite(XT_CLK, HIGH);
  delayMicroseconds(kbt.xt_bit_delay);
  
  // Release the XT clock and data.
  pinMode(XT_CLK, INPUT_PULLUP);
  pinMode(XT_DATA, INPUT_PULLUP);

  if(board_type == B_DEV)
  {
    digitalWrite(LED_XT_DATA, LOW);
    digitalWrite(LED_XT_CLK, LOW);
  }

  LOG ("[X:");
  LOG_HEX (sxc_code);
  LOG ("]");
}

/*************************************************************************
 * Update the keyboard status LED's
 *************************************************************************/
void updateKbLeds(void)
{
  sendAtCode(0xED);
  delay(kbt.at_next_delay);
  sendAtCode(kb_leds);
  delay(kbt.at_next_delay);
}

/*************************************************************************
 * Update the keyboard LED status byte and LED's if changed
 *************************************************************************/
void updateLedStatus(void)
{
  // Handle the toggle keys that have LED's
  switch(at_data_byte)
  {
    case 0x58:
      // Caps lock
      if (at_data_prev == 0xF0)
      {
        if (bitRead(kb_leds, 2))
        {
          bitClear(kb_leds, 2);
        }
        else
        {
          bitSet(kb_leds, 2);
        }
      }
      break;

    case 0x77:
      // Num lock
      if ((at_data_prev == 0xF0) && (!break_key_pressed))
      {
        if (bitRead(kb_leds, 1))
        {
          bitClear(kb_leds, 1);
        }
        else
        {
          bitSet(kb_leds, 1);
        }
      }
      break;

    case 0x7E:
      // Scroll lock
      if (at_data_prev == 0xF0)
      {
        if (bitRead(kb_leds, 0))
        {
          bitClear(kb_leds, 0);
        }
        else
        {
          bitSet(kb_leds, 0);
        }
      }
      break;

    default:
      break;
  }

  // Has the keyboard LED states changed?
  if(kb_leds != kb_leds_prev)
  {
    updateKbLeds();
    kb_leds_prev = kb_leds;
  }
}

/*************************************************************************
 * Interrupt Service Routine
 *************************************************************************/
void INT1_ISR(void)
{
  // Are we still processing the previous key press?
  if(isr_disabled)
  {
    return;
  }

  at_clk_busy = true;
  at_clk_count++;

  // Start bit
  if (at_clk_count == 1)
  {
    if(board_type == B_DEV)
    {
      // Turn on AT_CLK LED
      digitalWrite(LED_AT_CLK, HIGH);
    }
    at_data_byte = 0;
    at_data_temp = 0;
  }
  else // Data bits
  {
    // Still processing data bits?
    if (at_clk_count < 10)
    {
      at_data_bit = digitalRead(AT_DATA);
      if (at_data_bit)
      {
        at_data_temp = bitSet(at_data_byte, (at_clk_count - 2));
        at_data_byte = at_data_temp;
      }
    }
  }

  // Stop bit
  if (at_clk_count >= 11)
  {
    // Reset flags and counters
    at_clk_count = 0;
    at_data_ready = true;
    at_clk_busy = false;

    if(board_type == B_DEV)
    {
      // Turn off AT_CLK LED
      digitalWrite(LED_AT_CLK, LOW);
    }
  }
}
