// **********************************************************************************
// MIT License
//
// Copyright (c) 2020 Dilshan R Jayakody
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// **********************************************************************************

#ifndef LCD_H
#define	LCD_H

#include <stdint.h>

#define LCD_FUNCTION_SET        0x28
#define LCD_ENTRY_MODE          0x06
#define LCD_DISPLAY_SETUP       0x0C

#define LCD_CLEARDISPLAY        0x01
#define LCD_HOME                0x02
#define LCD_SET_CGRAM           0x40
#define LCD_SET_ROW1            0x80
#define LCD_SET_ROW2            0xC0

#define LCD_CCHAR_POINT              0

#define LCD_CCHAR_ARROW_FWD          1
#define LCD_CCHAR_ZENER_FWD          2
#define LCD_CCHAR_DIODE_FWD          3

#define LCD_CCHAR_ARROW_REV          4
#define LCD_CCHAR_ZENER_REV          5
#define LCD_CCHAR_DIODE_REV          6

#define LCD_CCHAR_WIRE               7

const uint8_t customCharPoint[8] = { 0x00, 0x1F, 0x11, 0x15, 0x15, 0x11, 0x1F, 0x00 };

const uint8_t customCharFwdArrow[8] = { 0x08, 0x0C, 0x0E, 0x1F, 0x1F, 0x0E, 0x0C, 0x08 };
const uint8_t customCharFwdZener[8] = { 0x0C, 0x08, 0x08, 0x0B, 0x0B, 0x08, 0x08, 0x18 };
const uint8_t customCharFwdDiode[8] = { 0x08, 0x08, 0x08, 0x0B, 0x0B, 0x08, 0x08, 0x08 };

const uint8_t customCharRevArrow[8] = { 0x02, 0x06, 0x0E, 0x1F, 0x1F, 0x0E, 0x06, 0x02 };
const uint8_t customCharRevZener[8] = { 0x03, 0x02, 0x02, 0x1A, 0x1A, 0x02, 0x02, 0x06 };
const uint8_t customCharRevDiode[8] = { 0x02, 0x02, 0x02, 0x1A, 0x1A, 0x02, 0x02, 0x02 };

const uint8_t customCharWire[8] = { 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00 };

void sendNibble(uint8_t inChar, uint8_t isData);
void sendData(uint8_t val, uint8_t mode);

void initLCD(void);
void clearLCD(void);
void setLocation(uint8_t col, uint8_t row);
void setGraphicRAM(uint8_t location, const uint8_t *buffer);

void printString(char* buffer, int8_t size);

#define sendCmd(cmd) sendData(cmd, 0)
#define sendChr(data) sendData(data, 1)

#define returnHome() sendCmd(LCD_HOME)

#endif	/* LCD_H */

