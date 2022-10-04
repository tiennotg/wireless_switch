/*
* const.h
* 
* Part of the interrupteur433 project.
* Constants.
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

#ifndef _CONST_H
#define _CONST_H

/* 
 *  Index storage in EEPROM is in TLV-format (TLV: Tag length value) 
 *  Index tag: 0xA6
 *  
 *  For example: 0xA6 0x04 index(32bits integer)
 *                ^    ^      ^
 *               tag length  value (little endian)
 */
#define EE_START_ADDRESS  0x10
#define EE_INDEX_TAG      0x15

#define BUFFER_SIZE       255       // Size of general purpose buffers
#define SERIAL_RATE       115200    // Baud rate of the Serial port (used for debugging)
#define DEAD_TIME         100       // Minimal interval between two switch pulses (in ms)

#define RADIO_PIN         3         // Pin for data from radio module (TX or RX)
#define INTERRUPT_PIN     12        // Pin for the switch (TX side)
#define OUTPUT_PIN        12        // Pin for the load (RX side)

#endif
