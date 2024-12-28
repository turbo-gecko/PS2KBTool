/*
 * Serial_Buffer_Nano
 * 
 * Uses an Arduino Nano mounted on a Garmic-001 base board to provide
 * buffered serial data in one direction and unbuffered in the other
 * direction.
 * 
 * Serial port A supports hardware handshaking whilst serial port B
 * has no handshaking. The program is designed to pass through data 
 * from serial port B and when serial port A is not ready to receive 
 * data, the Nano will start buffering the data until serial port A
 * is ready to receive it again. The default buffer size is 1024 bytes
 * which is adequate for X-MODEM 128B and 1K protocols.
 * 
 * This software is copyright 2021 by Gary Hammond (ZL3GH) along with
 * all the software bugs herein. It is free to use for non-commercial
 * purposes.
 * 
 * WARNING: DO NOT USE this software in any medical device or for any 
 * other mission critical purpose.
 * 
 * Use of this software could result in a universe ending paradox so 
 * use entirely at your own risk. No warranties or guarantees are 
 * expressed or implied.
 */

#include "globals.h"

#include "commands.h"
#include "eeprom_utils.h"
#include "serial_utils.h"

/*************************************************************************
 * Variables
 *************************************************************************/
bool buffer_overflow    = false;
bool program_mode       = true;

char buffer[BUFFER_SIZE];
char rx_byte            = 0;
char tx_byte            = 0;

String host_command     = "";
String last_command     = "";

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
  pinMode(CTS, INPUT);

  // Initialise output pins.
  pinMode(LED_0, OUTPUT);
  pinMode(LED_25, OUTPUT);
  pinMode(LED_50, OUTPUT);
  pinMode(LED_75, OUTPUT);
  pinMode(LED_100, OUTPUT);

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
  if (S_HOST.available()) {
    rx_byte = S_HOST.read();

    if (rx_byte == '^') {
      program_mode = true;
    }
  }

  if (program_mode) {
    digitalWrite(LED_75, HIGH);
    digitalWrite(LED_100, HIGH);
  }
  else {
    cycleLEDs();
  }

  // Debug
  program_mode = true;
//  sHostBaudRate(115200);
//  sHostXonXoff(0);
}

// the loop function runs over and over again forever
void loop()
{
  //Debug
//  S_HOST.println(F("Serial Buffer Help"));
//  delay(2000);

//  cycleLEDs();
  
  
  if (program_mode) {
    processCommands();
  }
  else {
    if (S_HOST.available()) {
      rx_byte = S_HOST.read();
  
      if (count == BUFFER_SIZE) {
        digitalWrite(LED_100, HIGH);
        buffer_overflow = true;
      }
      else {
        buffer[head] = rx_byte;
    
        head++;
        if (head == BUFFER_SIZE) {
          head = 0;
        }
   
        count++;
        if (count > peak_count) {
          peak_count = count;
        }
      }
    }
  
    updateLEDs(count);
        
    if (digitalRead(CTS) == 0) {
      if (count > 0) {
        tx_byte = buffer[tail];
        count--;
        tail++;
        if (tail == BUFFER_SIZE) {
          tail = 0;
        }
        S_HOST.write(tx_byte);
        delayMicroseconds(TX_DELAY);
      }
    }
  
    if (buffer_overflow == true) {
      if ((millis() % 500) > 250) {
        digitalWrite(LED_100, HIGH);
      }
      else {
        digitalWrite(LED_100, LOW);
      }
    }
  }
}

void cycleLEDs()
{
  digitalWrite(LED_0, HIGH);
  delay(LED_DELAY);
  digitalWrite(LED_25, HIGH);
  delay(LED_DELAY);
  digitalWrite(LED_50, HIGH);
  delay(LED_DELAY);
  digitalWrite(LED_75, HIGH);
  delay(LED_DELAY);
  digitalWrite(LED_100, HIGH);
  delay(LED_DELAY);
  digitalWrite(LED_0, LOW);
  delay(LED_DELAY);
  digitalWrite(LED_25, LOW);
  delay(LED_DELAY);
  digitalWrite(LED_50, LOW);
  delay(LED_DELAY);
  digitalWrite(LED_75, LOW);
  delay(LED_DELAY);
  digitalWrite(LED_100, LOW);
  delay(LED_DELAY);
}

void updateLEDs(int count)
{
  if (count > 0) {
    digitalWrite(LED_0, HIGH);
  }
  else {
    digitalWrite(LED_0, LOW);
    digitalWrite(LED_25, LOW);
    digitalWrite(LED_50, LOW);
    digitalWrite(LED_75, LOW);
    return;
  }

  if (count > BUFFER_SIZE / 4) {
    digitalWrite(LED_25, HIGH);
  }
  else {
    digitalWrite(LED_25, LOW);
    digitalWrite(LED_50, LOW);
    digitalWrite(LED_75, LOW);
    return;
  }

  if (count > BUFFER_SIZE / 2) {
    digitalWrite(LED_50, HIGH);
  }
  else {
    digitalWrite(LED_50, LOW);
    digitalWrite(LED_75, LOW);
    return;
  }

  if (count > (BUFFER_SIZE / 2) + (BUFFER_SIZE / 4)) {
    digitalWrite(LED_75, HIGH);
  }
  else {
    digitalWrite(LED_75, LOW);
  }
}

void processCommands()
{
  // Process serial input from the host port
  if (S_HOST.available() > 0) {
    rx_byte = sHostRead();
    sHostPrint((const String) rx_byte);
    if (rx_byte == '\n' || rx_byte == '\r') {
      sHostPrintln("");
      if (host_command.length() > 0) {
        processCommand(host_command);
        last_command = host_command;
        host_command = "";
      } 
      sHostPrompt();
    }
    else if (rx_byte == BACK_SPACE) {
      if (host_command.length() > 0) {
        sHostPrint(" ");
        S_HOST.write(BACK_SPACE);
        host_command.remove(host_command.length() - 1, 1);
      }
      else {
        S_HOST.write(">");
      }
    }
    else if (rx_byte == UP_ARROW) {
      host_command = host_command + last_command;
      sHostPrint(host_command);
    }
    else {
      host_command = host_command + (char) rx_byte;
    }
  }
}
