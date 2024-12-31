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

//#include "Arduino.h"

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
bool isr_disabled       = false;
bool key_release        = false;

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
    digitalWrite(LED_0, HIGH); // Turn on data RX LED
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
    digitalWrite(LED_0, LOW); // Turn off data RX LED
  }
}

/*************************************************************************
 * Setup
 *************************************************************************/
void setup()
{
  // Initialise input pins
  pinMode(CTS, INPUT_PULLUP);
  pinMode(AT_CLK, INPUT_PULLUP);
  pinMode(AT_DATA, INPUT_PULLUP);
  pinMode(XT_CLK, INPUT_PULLUP);
  pinMode(XT_DATA, INPUT_PULLUP);

   // Initialise output pins.
  pinMode(LED_0, OUTPUT);
  pinMode(LED_25, OUTPUT);
  pinMode(LED_50, OUTPUT);
  pinMode(LED_75, OUTPUT);
  pinMode(LED_100, OUTPUT);
 
  // Set up the interrupt for the AT_CLK line
  attachInterrupt(digitalPinToInterrupt(AT_CLK), INT1_ISR, FALLING);

  // Initialise EEPROM
  eInit();

  // Initialise host serial port
  S_HOST.begin(sHostGetBaudRate());

  digitalWrite(LED_0, HIGH);
  digitalWrite(LED_25, HIGH);
  digitalWrite(LED_50, HIGH);
  digitalWrite(LED_75, HIGH);
  digitalWrite(LED_100, HIGH);

  // Wait for 2 seconds to see if a character has been received
  delay(2000);

  digitalWrite(LED_0, LOW);
  digitalWrite(LED_25, LOW);
  digitalWrite(LED_50, LOW);
  digitalWrite(LED_75, LOW);
  digitalWrite(LED_100, LOW);

  // Check to see if the character is a caret ^ indicating program mode
  if (S_HOST.available())
  {
    rx_byte = S_HOST.read();

    if (rx_byte == '^')
    {
      program_mode = true;
    }
  }

  if (program_mode)
  {
    digitalWrite(LED_75, HIGH);
    digitalWrite(LED_100, HIGH);
  }

  // Debug
  if (sHostGetEnabled())
  {
    S_HOST.println("Ready...");
  }
  //Debug
}

// the loop function runs over and over again forever
void loop()
{
  //Debug

  if (program_mode)
  {
    processCommands();
  }
  else
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
        if (at_data_byte == 0xF0) // Key release?
        {
          S_HOST.print(at_data_byte, HEX);
          S_HOST.print("\t");
          key_release = true;
        }
        else
        {
          S_HOST.print(at_data_byte, HEX);
          if (at_data_byte != 0xE0)
          {
            S_HOST.print('/');
            if (at_data_prev == 0xE0)
            {
              if (at_data_byte != 0x12)
              sendXtCode(at_data_prev);
              delayMicroseconds(500);
              xt_data_byte = AT2XTExt(at_data_byte);
            }
            else
            {
              xt_data_byte = AT2XT(at_data_byte);
              if(xt_data_byte == 0)
              {
                xt_data_byte = AT2XTExt(at_data_byte);
              }
              if (at_data_prev == 0xF0)
              {
                xt_data_byte = xt_data_byte + 0x80;
              }
              sendXtCode(xt_data_byte);
            }
            S_HOST.print(xt_data_byte, HEX);
          }
          S_HOST.print("\t");
          if (key_release)
          {
            S_HOST.println("");
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
}

void sendXtCode(byte sxc_code)
{
  // Check to see if we are processing incoming data from the AT port
  // and wait until it has completed.
  while (at_clk_busy); //incoming data so wait

  // Disable the AT clock and stop keyboard from sending more data.
  isr_disabled = true;
  pinMode(AT_CLK, OUTPUT);
  digitalWrite(AT_CLK, 0);

  // Check to see if the XT clock is in use and if not, enable it.
  pinMode(XT_CLK, OUTPUT);
  digitalWrite(XT_CLK, HIGH);
  pinMode(XT_DATA, OUTPUT);
  digitalWrite(XT_CLK, HIGH);

  digitalWrite(LED_100, HIGH);

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

  digitalWrite(LED_100, LOW);

  // Disable the XT clock and data.
  pinMode(XT_CLK, INPUT_PULLUP);
  pinMode(XT_DATA, INPUT_PULLUP);

  // Re-enable the AT clock.
  pinMode(AT_CLK, INPUT_PULLUP);
  isr_disabled = false;
}

void processCommands()
{
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
