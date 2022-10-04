/*
* radio.h
* 
* Part of the interrupteur433 project.
* Radio encoding and decoding function headers
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

#ifndef _RADIO_H
#define _RADIO_H

/*
 * Radio frame format : 0xAA ... (12 times) ... 0xFF data 0xFF (data repeated 'count' times) 0xFF
 *                      |          preamble         |                    data                   |
 *                      
 * To avoid long low bit sequences, an extra high bit is added after 6 consecutive low bits.
 * Data is little endian encoded.
 */

#define RADIO_PREAMBLE_BYTE 0xAA
#define RADIO_PREAMBLE_LEN 12
#define RADIO_STOP_BYTE 0xFF
#define RADIO_COUNT 2
#define RADIO_PERIOD 800 // in µs

void radio_init_tx(void);
void radio_init_rx(void);
void radio_send_data(uint8_t *buffer, const uint8_t len);
void radio_send_data(uint32_t d);
uint32_t radio_wait_for_data(void);
void radio_wait_for_data(uint8_t *buffer, uint8_t *len);

#endif
