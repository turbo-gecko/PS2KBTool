/*
 * PS2KBTool
 * 
 * Uses an Arduino Nano to provide PS2/AT keyboard scan code
 * modifications to an XT keyboard interface.
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
 * Build options
 *************************************************************************/
// Uncomment if using the developer edition to enable the extra LED's and
// and switches.
//#define DEV_BOARD

//*************************************************************************

#include "globals.h"

#include "commands.h"
#include "eeprom_utils.h"
#include "keyboard.h"
#include "serial_utils.h"

/*************************************************************************
 * Variables
 *************************************************************************/
bool buffer_overflow    = false;
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

char buffer[BUFFER_SIZE];
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

unsigned int at_timeout = 0;

unsigned int count      = 0;
unsigned int head       = 0;
unsigned int peak_count = 0;
unsigned int tail       = 0;

/*************************************************************************
 * Setup
 *************************************************************************/
void setup()
{
  // Initialise input pins
  pinMode(CTS, INPUT_PULLUP);

  pinMode(AT_CLK, OUTPUT);
  digitalWrite(AT_CLK, LOW);
  pinMode(AT_DATA, INPUT_PULLUP);
  pinMode(XT_CLK, INPUT_PULLUP);
  pinMode(XT_DATA, INPUT_PULLUP);
  pinMode(CONFIG_1, INPUT_PULLUP);
  pinMode(CONFIG_2, INPUT_PULLUP);

#ifdef DEV_BOARD
  // Initialise output pins for the LED's
  pinMode(LED_AT_CLK, OUTPUT);
  pinMode(LED_AT_DATA, OUTPUT);
  pinMode(LED_NUM, OUTPUT);
  pinMode(LED_CAPS, OUTPUT);
  pinMode(LED_SCROLL, OUTPUT);

  // Flash the LED's
  digitalWrite(LED_AT_CLK, HIGH);
  digitalWrite(LED_AT_DATA, HIGH);
  digitalWrite(LED_NUM, HIGH);
  digitalWrite(LED_CAPS, HIGH);
  digitalWrite(LED_SCROLL, HIGH);

  delay (500);

  digitalWrite(LED_AT_CLK, LOW);
  digitalWrite(LED_AT_DATA, LOW);
  digitalWrite(LED_NUM, LOW);
  digitalWrite(LED_CAPS, LOW);
  digitalWrite(LED_SCROLL, LOW);
#endif

  pinMode(AT_CLK, INPUT_PULLUP);
  // Set up the interrupt for the AT_CLK line
  attachInterrupt(digitalPinToInterrupt(AT_CLK), INT1_ISR, FALLING);

  // Initialise EEPROM
  eInit();

  int temp1 = digitalRead(CONFIG_1);
  if (temp1 == LOW)
  {
    program_mode = true;

#ifdef DEV_BOARD
    digitalWrite(LED_SCROLL, HIGH);
#endif

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
#ifdef DEV_BOARD
  checkDevOptions();
#endif
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
    if (sHostGetEnabled())
    {
      // Key release?
      if (at_data_byte == 0xF0)
      {
        if (ext_101_enabled)
        {
          if (at_data_prev == 0xE0)
          {
            sendXtCode(at_data_prev);
            delayMicroseconds(10);
          }
        }
        if (serial_enabled)
        {
          S_HOST.print(at_data_byte, HEX);
          S_HOST.print("\t");
        }
        key_release = true;
      }
      else
      {
        if (serial_enabled)
        {
          S_HOST.print(at_data_byte, HEX);
        }
        if (at_data_byte != 0xE0)
        {
          if (serial_enabled)
          {
            S_HOST.print('/');
          }
          if (at_data_prev == 0xE0) // Is the previous byte the ext code?
          {
            if (at_data_byte != 0x12) // Not the E0 12 ext sequence
            {
              xt_data_byte = AT2XTExt(at_data_byte);
              if (xt_data_byte != 0x00) // Found a corresponding ext code
              {
                ext_pressed = true;
                sendXtCode(at_data_prev);  // so send the E0
                delayMicroseconds(K_TX_DELAY);
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
                    delayMicroseconds(K_TX_DELAY);
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
            if (serial_enabled)
            {
              S_HOST.print(xt_data_byte, HEX);
            }
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
                  delayMicroseconds(K_TX_DELAY);
                }
              }
              if (ext_nav_pressed)
              {
                ext_nav_pressed = false;
                if (ext_101_enabled)
                {
                  sendXtCode(0xE0); // so send the E0
                  delayMicroseconds(K_TX_DELAY);
                }
              }
              xt_data_byte = xt_data_byte + 0x80;
            }
            if (serial_enabled)
            {
              S_HOST.print(xt_data_byte, HEX);
            }
            sendXtCode(xt_data_byte);
          }
        }
        if (serial_enabled)
        {
          S_HOST.print("\t");
        }
        if (key_release)
        {
          if (serial_enabled)
          {
            S_HOST.println("");
          }
          key_release = false;
        }
      }
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

#ifdef DEV_BOARD
  digitalWrite(LED_AT_DATA, HIGH);
#endif

  // Send start bit.
  digitalWrite(XT_DATA, HIGH);
  delayMicroseconds(5);
  digitalWrite(XT_CLK, LOW);
  delayMicroseconds(30);
  digitalWrite(XT_CLK, HIGH);
  delayMicroseconds(30);

  // Send data.
  for(int count = 0; count <9; count++)
  {
    digitalWrite(XT_DATA, bitRead(sxc_code, count));
    delayMicroseconds(5);
    digitalWrite(XT_CLK, LOW);
    delayMicroseconds(30);
    digitalWrite(XT_CLK, HIGH);
    delayMicroseconds(30);
  }

  digitalWrite(XT_DATA, HIGH);

#ifdef DEV_BOARD
  digitalWrite(LED_AT_DATA, LOW);
#endif

  // Disable the XT clock and data.
  pinMode(XT_CLK, INPUT_PULLUP);
  pinMode(XT_DATA, INPUT_PULLUP);

  if (serial_enabled)
  {
    S_HOST.print("[");
    S_HOST.print(sxc_code, HEX);
    S_HOST.print("]");
  }
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
#ifdef DEV_BOARD
    digitalWrite(LED_AT_CLK, HIGH); // Turn on data RX LED
#endif
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
#ifdef DEV_BOARD
    digitalWrite(LED_AT_CLK, LOW); // Turn off data RX LED
#endif
  }
}

//*************************************************************************
#ifdef DEV_BOARD
void checkDevOptions(void)
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
    digitalWrite(LED_NUM, HIGH);
  }
  else
  {
    digitalWrite(LED_NUM, LOW);
  }
  if (analogRead(A3) < 128)
  {
    digitalWrite(LED_CAPS, HIGH);
  }
  else
  {
    digitalWrite(LED_CAPS, LOW);
  }
}
#endif
