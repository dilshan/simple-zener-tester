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

#ifndef MAIN_H
#define	MAIN_H

#include <stdint.h>

#include "sysconfig.h"

#define OPEN_VOLTAGE            280     // 28.0V
#define SHORT_CIRCUIT_VOLTAGE   20      // 2.0V

#define DIRECTION_FWD           1
#define DIRECTION_REV           0

void initSystem(void);
uint16_t getADCValue(uint8_t channel);
void scanForZener(uint16_t *readData);
uint32_t adcToVolts(uint16_t adcVal);
void printFloat(uint32_t *val);
void getCurrentZenerValues(uint16_t *zenerTerminalVal);
void scanAndPrintZener(void);

#endif	/* MAIN_H */

