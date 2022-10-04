/*
* debug.h
* 
* Part of the interrupteur433 project.
* Some preprocessor functions used for debuging.
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

#ifndef _DEBUG_H
#define _DEBUG_H

#define _DEBUG 0
#define _DBG_PRINT_BUFFER(buf,len) \
  if (_DEBUG) for (int w=0; w<len; w++) {\
    if (buf[w]<0x10) Serial.print("0");\
    Serial.print(buf[w],HEX); \
  }\
  Serial.println();
#define _DBG_PRINT(msg) \
  if (_DEBUG) Serial.println(msg);
#define _DBG_START \
  if (_DEBUG) Serial.begin(SERIAL_RATE);

#endif
