/*
* radio.cpp
* 
* Part of the interrupteur433 project.
* Radio encoding and decoding functions.
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

#include <stdint.h>
#include <Arduino.h>
#include "radio.h"
#include "const.h"
#include "debug.h"

int low_bit_counter = 0;

void radio_init_tx(void)
{
  pinMode(RADIO_PIN, OUTPUT);
}

void radio_init_rx(void)
{
  pinMode(RADIO_PIN, INPUT);
}

void send_byte(const uint8_t B)
{
  // if 6 consecutive bits are low, add an extra high bit
  uint8_t mask = 0x80;
  uint8_t i=0;
  uint32_t last_bit_time = 0;
  
  while (i<=8)
  {
    if (micros()-last_bit_time >= RADIO_PERIOD)
    {
      if (i>=8 && low_bit_counter < 6) break;
      last_bit_time = micros();
      if (low_bit_counter == 6)
      {
        digitalWrite(RADIO_PIN, HIGH);
        low_bit_counter = 0;
      }
      else if (B & mask)
      {
        digitalWrite(RADIO_PIN, HIGH);
        low_bit_counter = 0;
        i++;
        mask = mask >> 1;
      }
      else
      {
        digitalWrite(RADIO_PIN, LOW);
        low_bit_counter++;
        i++;
        mask = mask >> 1;
      }
    }
  }
}

void radio_send_data(uint8_t *buffer, const uint8_t len)
{
  _DBG_PRINT("Send data over the air.")
  low_bit_counter = 0;
  for (int i=0; i<RADIO_PREAMBLE_LEN; i++)
    send_byte(RADIO_PREAMBLE_BYTE);
  send_byte(RADIO_STOP_BYTE);

  for (int i=0; i<RADIO_COUNT; i++)
  {
    for (uint8_t j=0; j<len; j++)
      send_byte(buffer[j]);
    send_byte(RADIO_STOP_BYTE);
  }
  digitalWrite(RADIO_PIN, LOW);
}

void radio_send_data(uint32_t d)
{
  const uint8_t data_len = 4;
  uint8_t buffer[data_len];
  
  for (uint8_t i=0; i<data_len; i++)
  {
    buffer[i] = d & 0xFF;
    d = d >> 8;
  }
  
  radio_send_data(buffer, data_len);
}

void radio_wait_for_data(uint8_t *buffer, uint8_t *len)
{
  bool msg_started = false;
  bool msg_ended = false;
  uint32_t last_read = 0;
  int zero_counter = 0;
  uint8_t bit_array[BUFFER_SIZE];
  uint8_t bit_array_len=0;
  uint8_t max_len = *len;
  
  
  while (!msg_ended)
  {
    if (!msg_started)
    {
      if (digitalRead(RADIO_PIN))
      {
        msg_started = true;
        bit_array[bit_array_len++] = 0x01; // Store first bit (always high)
        delayMicroseconds(RADIO_PERIOD/2); // Ensure to probe signal at center of a period,
                                           // to avoid side effects due to edges of the square signal
        last_read = micros();
      }
    }
    else
    {
      if (micros()-last_read >= RADIO_PERIOD) // A period is over, probe bit value
      {
        last_read = micros();
        if (digitalRead(RADIO_PIN)) // High bit
        {
          if (zero_counter < 6) // Ignore extra '1' bits added to avoid long '0' sequences.
            bit_array[bit_array_len++] = 1;
          zero_counter = 0;
        }
        else // low bit
        {
          bit_array[bit_array_len++] = 0;
          zero_counter++;
        }
        if (zero_counter >= 10 || bit_array_len >= BUFFER_SIZE) // More than ten low bits, or buffer overflow? Message ended.
          msg_ended = true;
      }
    }
  }

  if (bit_array_len > 20) // Ignore too short messages
  {
    _DBG_PRINT("Unpack bit sequence into bytes.")
    
    memset(buffer,0,*len);
    *len = bit_array_len/8;
    for (int i=0; i<*len; i++)
    {
      for (int j=0; j<8; j++)
      {
        if (bit_array[i*8+j] == 1)
          bitSet(buffer[i], 8-j-1);
      }
    }
    _DBG_PRINT_BUFFER(buffer,*len)
  }
  else
    *len = 0;
}

uint32_t radio_wait_for_data(void)
{
  uint8_t buffer[BUFFER_SIZE];
  uint8_t len=BUFFER_SIZE;
  radio_wait_for_data(buffer,&len);

  if (len < RADIO_PREAMBLE_LEN + 6) // +6, for 4 bytes of data, and two stop bytes
    return 0;

  // Count number of valid bytes in preamble
  // If it is more than 60% correct, then look for data
  int preamble_byte_count = 0;
  for (int i=0; i<RADIO_PREAMBLE_LEN; i++)
    if (buffer[i] == RADIO_PREAMBLE_BYTE) preamble_byte_count++;
  if (preamble_byte_count > RADIO_PREAMBLE_LEN * 0.6)
  {
    _DBG_PRINT("Valid preamble! Continue.")
    uint8_t *p = (uint8_t*) memchr(buffer,RADIO_STOP_BYTE, len) + 1;
    
    if (buffer+len-p > 4) // (Codes are 4-bytes long. Including stop byte, data lenght should be more than 4)
    {
      uint32_t r=0;
      for (int j=3; j>=0; j--)
      {
        r = r<<8;
        r += p[j];
      }
      return r;
    }
    else
    {
      _DBG_PRINT("Data corrupted.")
      return 0;
    }
  }
  return 0;
}
