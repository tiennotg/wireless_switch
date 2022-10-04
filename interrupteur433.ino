/*
* interrupteur433.ino
* 
* A 433MHz wireless-controlled outlet with Arduino boards.
* Main file.
* 
* Copyright (C) 2022  Guilhem Tiennot
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


// simpleHOTP library
// https://github.com/jlusPrivat/SimpleHOTP
#include <SimpleHOTP.h>
#include <EEPROM.h>
#include "debug.h"
#include "const.h"
#include "radio.h"

#define TX_BOARD

uint8_t key[] = "CHANGE ME!"; // Key, to be changed
uint64_t hotp_index = 0;

Key *skey;
SimpleHOTP *hotp;

#ifdef TX_BOARD
bool last_state = true; // State of the INTERRUPT_PIN since last reading,
                        // to compare to the next, and detect falling.
#else
int output_state = LOW;
#endif

void setup()
{
  _DBG_START
  
  if (ee_read_uint64(hotp_index))
  {
    _DBG_PRINT("Index retrieved : "+String((unsigned long) hotp_index))
  }
  else
  {
    // Index not found, initializing EEPROM 
    ee_write_uint64(hotp_index);
    _DBG_PRINT("EEPROM initialized.");
  }

  skey = new Key(key, sizeof(key));
  hotp = new SimpleHOTP(*skey, hotp_index);
  hotp->setResync(10); // Look ahead 10 OTPs, to avoid desync if we missed one
  hotp->setThrottle(0); // Never lock the validator if a code failed
  
  #ifdef TX_BOARD
  radio_init_tx();
  pinMode(INTERRUPT_PIN,INPUT_PULLUP);
  #else
  radio_init_rx();
  pinMode(OUTPUT_PIN,OUTPUT);
  digitalWrite(OUTPUT_PIN,output_state);
  #endif
}

void loop()
{
  #ifdef TX_BOARD
  // The default interrupt pin (n°12) is not suitable for interruptions
  // Polling falling edges manually.
  bool next_state = digitalRead(INTERRUPT_PIN);
  if (!next_state && (next_state != last_state))
  {
    generate_and_send_otp();
    delay(DEAD_TIME);
  }
  last_state = next_state;
  #else
  uint32_t code;
  uint64_t new_hotp_index=0;
  if ((code = radio_wait_for_data()) != 0)
  {
      _DBG_PRINT("Received OTP: "+String(code))
    if ((new_hotp_index = hotp->validate(code)) > 0)
    {
      _DBG_PRINT("Correct OTP!")
      output_state = !output_state;
      digitalWrite(OUTPUT_PIN, output_state);
      ee_write_uint64(new_hotp_index);
    }
    else
      _DBG_PRINT("Bad OTP :(")
  }
  #endif
}

#ifdef TX_BOARD
void generate_and_send_otp(void)
{
  _DBG_PRINT("Interrupt triggered!")
  uint32_t current_hotp = hotp->generateHOTP();
  _DBG_PRINT("HOTP Generated: "+String(current_hotp))
  hotp->validate(current_hotp); // The library doesn't provide a function to increment the internal counter.
                                // Doing this by validating the generated hotp.
  radio_send_data(current_hotp);
  ee_write_uint64(++hotp_index);
}
#endif

bool ee_read_uint64(uint64_t &value)
{
  uint32_t ee_index = EE_START_ADDRESS;
  uint8_t readed_tag = EEPROM.read(ee_index++);
  
  if (readed_tag == EE_INDEX_TAG)
  {
    _DBG_PRINT("Index found in EEPROM.")
    EEPROM.get(ee_index,value);
    return true;
  }
  else
  {
    _DBG_PRINT("Index not found in EEPROM.")
    return false;
  }
}

void ee_write_uint64(const uint64_t value)
{
  uint32_t ee_index = EE_START_ADDRESS;
  EEPROM.update(ee_index++, EE_INDEX_TAG);
  EEPROM.put(ee_index, value);
}
