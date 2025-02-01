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
bool ext_101_enabled    = false;
bool ext_pressed        = false;
bool ext_nav_pressed    = false;
bool ext_strip_pressed  = false;
bool isr_disabled       = false;
bool key_release        = false;
bool serial_enabled     = false;

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
unsigned int b_type     = 0;
unsigned int count      = 0;

struct kb_timings
{
  byte at_bit_delay;
  byte at_next_delay;
  byte at_start_delay;
  byte xt_bit_delay;
  byte xt_next_delay;
  byte xt_start_delay;
};

struct kb_timings kbt;

/*************************************************************************
 * Macro's
 *************************************************************************/
#define LOG(x) if (serial_enabled){S_HOST.print((x));}
#define LOG_HEX(x) if (serial_enabled){S_HOST.print((x), HEX);}

/*************************************************************************
 * Setup
 *************************************************************************/
void setup()
{
  // Stop KB from sending data
  pinMode(AT_CLK, OUTPUT);
  digitalWrite(AT_CLK, LOW);

  // Initialise input pins
  pinMode(D2, INPUT_PULLUP);
  pinMode(AT_DATA, INPUT_PULLUP);
  pinMode(XT_CLK, INPUT_PULLUP);
  pinMode(XT_DATA, INPUT_PULLUP);
  pinMode(CONFIG_1, INPUT_PULLUP);
  pinMode(CONFIG_2, INPUT_PULLUP);

  // Initialise unused digital pins
  pinMode(D2, INPUT_PULLUP);
  pinMode(D14, INPUT_PULLUP);
  pinMode(D15, INPUT_PULLUP);

  switch(kGetBoardType())
  {
    case B_DEV:
      // Initialise output pins for the LED's
      pinMode(LED_AT_CLK, OUTPUT);
      pinMode(LED_AT_DATA, OUTPUT);
      pinMode(LED_XT_CLK, OUTPUT);
      pinMode(LED_XT_DATA, OUTPUT);
      pinMode(LED_PROG, OUTPUT);

      // Flash the LED's
      digitalWrite(LED_AT_CLK, HIGH);
      digitalWrite(LED_AT_DATA, HIGH);
      digitalWrite(LED_XT_CLK, HIGH);
      digitalWrite(LED_XT_DATA, HIGH);
      digitalWrite(LED_PROG, HIGH);

      delay (500);

      digitalWrite(LED_AT_CLK, LOW);
      digitalWrite(LED_AT_DATA, LOW);
      digitalWrite(LED_XT_CLK, LOW);
      digitalWrite(LED_XT_DATA, LOW);
      digitalWrite(LED_PROG, LOW);
      break;

    case B_STD:
      // Initialise the LED outputs as unused pins
      pinMode(LED_AT_CLK, INPUT_PULLUP);
      pinMode(LED_AT_DATA, INPUT_PULLUP);
      pinMode(LED_XT_CLK, INPUT_PULLUP);
      pinMode(LED_XT_DATA, INPUT_PULLUP);
      pinMode(LED_PROG, INPUT_PULLUP);
      break;

    default:
      break;
  }

  // Initialise EEPROM
  eInit();

  // Get keyboard delay timings from EEPROM
  kbt.at_bit_delay = kGetDelayTimings(1);
  kbt.at_next_delay = kGetDelayTimings(2);
  kbt.at_start_delay = kGetDelayTimings(3);
  kbt.xt_bit_delay = kGetDelayTimings(4);
  kbt.xt_next_delay = kGetDelayTimings(5);
  kbt.xt_start_delay = kGetDelayTimings(6);

  int temp1 = digitalRead(CONFIG_1);
  if (temp1 == LOW)
  {
    program_mode = true;

    if (kGetBoardType() == B_DEV)
    {
      digitalWrite(LED_PROG, HIGH);
    }

    serial_enabled = true;
    S_HOST.begin(sHostGetBaudRate());
    S_HOST.println("Programming mode...");
  }
  else
  {
    program_mode = false;

    serial_enabled = sHostGetEnabled();
    if (serial_enabled)
    {
      // Initialise host serial port
      S_HOST.begin(sHostGetBaudRate());
    }

    // Enable the KB
    pinMode(AT_CLK, INPUT_PULLUP);
    delay(10);

    sendAtCode(0xFF);
    delay(500);
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
    at_data_ready = false;
    at_clk_count = 0;
    if (kGetBoardType() == B_DEV)
    {
      digitalWrite(LED_AT_CLK, LOW); // Turn off AT_CLK LED
    }
  }

  // Get extended 101 key enabled state
  ext_101_enabled = kGet101Enabled();
}

// The loop function runs over and over again forever
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

//*************************************************************************
void processCommands()
{
  if (kGetBoardType() == B_DEV)
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
          S_HOST.write(">");
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

//*************************************************************************
void processKeyPress()
{
  if (at_data_ready)
  {
    // check to see if the keyboard is finished sending
    isr_disabled = true;
    while(!digitalRead(AT_CLK));
    // stop keyboard from sending more data
    pinMode(AT_CLK, OUTPUT);
    digitalWrite(AT_CLK, 0);
    
    // process data byte
    switch(at_data_byte)
    {
      case 0xAA: // BAT from KB
        LOG ("\n");
        LOG_HEX(at_data_byte);
        LOG (" <BAT>\n\n");
        break;

      case 0xF0: // Key release
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
        break;

      case 0xFA: // Ack from KB
        LOG ("\n");
        LOG_HEX(at_data_byte);
        LOG (" <ACK>\n\n");
        break;

      default:
        LOG_HEX (at_data_byte);
        if (at_data_byte != 0xE0) // Current scan code is not the ext code
        {
          LOG ('/');
          if (at_data_prev == 0xE0) // Is the previous scan code the ext code?
          {
            if (at_data_byte != 0x12) // Not the E0 12 ext sequence
            {
              xt_data_byte = AT2XTExt(at_data_byte);
              if (xt_data_byte != 0x00) // Found a corresponding ext code
              {
                ext_pressed = true;
                sendXtCode(at_data_prev);  // so send the E0
                delayMicroseconds(kbt.xt_next_delay);
              }
              else
              {
                xt_data_byte = AT2XTExtNav(at_data_byte);
                if (xt_data_byte != 0x00) // Found a corresponding ext nav code
                {
                  ext_nav_pressed = true;
                  if (ext_101_enabled) // Allow the ext keys
                  {
                    sendXtCode(at_data_prev); // so send the E0
                    delayMicroseconds(kbt.xt_next_delay);
                  }
                  else
                  {
                    ext_strip_pressed = true;
                  }
                }
                else // See if the byte is one of the codes to have the E0 stripped
                {
                  xt_data_byte = AT2XTExtStrip(at_data_byte);
                  if (xt_data_byte != 0x00) // Found a corresponding ext code
                  {
                    ext_strip_pressed = true;
                  }
                }
              }
            }
            LOG_HEX (xt_data_byte);
            if (xt_data_byte != 0x00)
            {
              sendXtCode(xt_data_byte);
            }
          }
          else
          {
            xt_data_byte = AT2XT(at_data_byte);
            if(xt_data_byte == 0)
            {
              xt_data_byte = AT2XTExt(at_data_byte);
            }
            if (at_data_prev == 0xF0) // Key released
            {
              if (ext_pressed)
              {
                ext_pressed = false;
                if(!ext_strip_pressed)
                {
                  sendXtCode(0xE0); // so send the E0
                  delayMicroseconds(kbt.xt_next_delay);
                }
              }
              if (ext_nav_pressed)
              {
                ext_nav_pressed = false;
                if (ext_101_enabled)
                {
                  sendXtCode(0xE0); // so send the E0
                  delayMicroseconds(kbt.xt_next_delay);
                }
              }
              xt_data_byte = xt_data_byte + 0x80;
            }
            LOG_HEX (xt_data_byte);
            if (xt_data_byte != 0)
            {
              sendXtCode(xt_data_byte);
            }

            switch(at_data_byte)
            {
              case 0x58: // Caps lock
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

              case 0x77: // Num lock
                if (at_data_prev == 0xF0)
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

              case 0x7E: // Scroll lock
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
            if(kb_leds != kb_leds_prev)
            {
              updateKbLeds();
              kb_leds_prev = kb_leds;
            }
          }
        }
        LOG ("\t");
        if (key_release)
        {
          LOG ("\n");
          key_release = false;
        }
        break;
    }

    at_data_ready = false;
    at_data_printed = true;
    at_data_prev = at_data_byte;

    // re-enable the keyboard to send data
    pinMode(AT_CLK, INPUT_PULLUP);
    isr_disabled = false;
  }
}

//*************************************************************************
void sendAtCode(byte sac_code)
{
  static byte parity;
  parity = 0;
  
  // Check to see if we are processing incoming data from the AT port
  // and wait until it has completed.
  while (at_clk_busy); //incoming data so wait

  // Check to see if the AT clock is in use and if not, enable it.
  pinMode(AT_CLK, OUTPUT);
  digitalWrite(AT_CLK, HIGH);
  pinMode(AT_DATA, OUTPUT);
  digitalWrite(AT_DATA, HIGH);

  if (kGetBoardType() == B_DEV)
  {
    digitalWrite(LED_AT_DATA, HIGH);
  }

  // Send start bit.
  digitalWrite(AT_CLK, LOW);
  delayMicroseconds(kbt.at_bit_delay);
  digitalWrite(AT_DATA, LOW);
  delayMicroseconds(kbt.at_bit_delay);
  // Release the AT clock and data.
  pinMode(AT_CLK, INPUT_PULLUP);
  delayMicroseconds(kbt.at_bit_delay);

  // Send data.
  for(int count = 0; count <8; count++)
  {
    while(digitalRead(AT_CLK));
    if (bitRead(sac_code, count))
    {
      digitalWrite(AT_DATA, HIGH);
      parity++;
    }
    else
    {
      digitalWrite(AT_DATA, LOW);
    }
    delayMicroseconds(kbt.at_bit_delay);
    //while(!digitalRead(AT_CLK));
    delayMicroseconds(kbt.at_bit_delay);
  }

  // Send parity bit.
  while(digitalRead(AT_CLK));
  parity = parity & 0x01;
  //digitalWrite(AT_DATA, parity);
  if(parity == 0)
  {
    digitalWrite(AT_DATA, HIGH);
  }
  else
  {
    digitalWrite(AT_DATA, LOW);
  }
  delayMicroseconds(kbt.at_bit_delay);
  delayMicroseconds(kbt.at_bit_delay);

  if (kGetBoardType() == B_DEV)
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

//*************************************************************************
void sendXtCode(byte sxc_code)
{
  // Check to see if we are processing incoming data from the AT port
  // and wait until it has completed.
  while (at_clk_busy); //incoming data so wait

  // Check to see if the XT clock is in use and if not, enable it.
  pinMode(XT_CLK, OUTPUT);
  digitalWrite(XT_CLK, HIGH);
  pinMode(XT_DATA, OUTPUT);
  digitalWrite(XT_CLK, HIGH);

  if (kGetBoardType() == B_DEV)
  {
    digitalWrite(LED_XT_CLK, HIGH);
    digitalWrite(LED_XT_DATA, HIGH);
  }

  // Send start bit.
  digitalWrite(XT_DATA, HIGH);
  delayMicroseconds(kbt.xt_start_delay);
  digitalWrite(XT_CLK, LOW);
  delayMicroseconds(kbt.xt_bit_delay);
  digitalWrite(XT_CLK, HIGH);
  delayMicroseconds(kbt.xt_bit_delay);

  // Send data.
  for(int count = 0; count <9; count++)
  {
    digitalWrite(XT_DATA, bitRead(sxc_code, count));
    delayMicroseconds(kbt.xt_start_delay);
    digitalWrite(XT_CLK, LOW);
    delayMicroseconds(kbt.xt_bit_delay);
    digitalWrite(XT_CLK, HIGH);
    delayMicroseconds(kbt.xt_bit_delay);
  }

  digitalWrite(XT_DATA, HIGH);
  delayMicroseconds(kbt.xt_bit_delay);

  if (kGetBoardType() == B_DEV)
  {
    digitalWrite(LED_XT_DATA, LOW);
    digitalWrite(LED_XT_CLK, LOW);
  }

  // Release the XT clock and data.
  pinMode(XT_CLK, INPUT_PULLUP);
  pinMode(XT_DATA, INPUT_PULLUP);

  LOG ("[X:");
  LOG_HEX (sxc_code);
  LOG ("]");
}

//*************************************************************************
void updateKbLeds(void)
{
  sendAtCode(0xED);
  delay(kbt.at_next_delay);
  sendAtCode(kb_leds);
  delay(kbt.at_next_delay);
}

/*************************************************************************
 * Interrupt Service Routine
 *************************************************************************/
void INT1_ISR(void)
{
  if(isr_disabled)
  {
    return;
  }
  at_clk_busy = true;
  at_clk_count++;

  if (at_clk_count == 1) // Start bit
  {
    if (kGetBoardType() == B_DEV)
    {
      digitalWrite(LED_AT_CLK, HIGH); // Turn on AT_CLK LED
    }
    at_data_byte = 0;
    at_data_temp = 0;
  }
  else // Data bits
  {
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

  if (at_clk_count >= 11) // Stop bit
  {
    at_clk_count = 0;
    at_data_ready = true;
    at_clk_busy = false;

    if (kGetBoardType() == B_DEV)
    {
      digitalWrite(LED_AT_CLK, LOW); // Turn off AT_CLK LED
    }
  }
}

//*************************************************************************
void checkDevOptions(void)
{
  if (kGetBoardType() == B_DEV)
  {
    if (analogRead(A0) < 128)
    {
      digitalWrite(LED_AT_CLK, HIGH);
    }
    else
    {
      digitalWrite(LED_AT_CLK, LOW);
    }
    if (analogRead(A1) < 128)
    {
      digitalWrite(LED_AT_DATA, HIGH);
    }
    else
    {
      digitalWrite(LED_AT_DATA, LOW);
    }
    if (analogRead(A2) < 128)
    {
      digitalWrite(LED_XT_CLK, HIGH);
    }
    else
    {
      digitalWrite(LED_XT_CLK, LOW);
    }
    if (analogRead(A3) < 128)
    {
      digitalWrite(LED_XT_DATA, HIGH);
    }
    else
    {
      digitalWrite(LED_XT_DATA, LOW);
    }
  }
}
