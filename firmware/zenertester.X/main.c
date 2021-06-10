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

#include <stdio.h>

#include "main.h"
#include "lcd.h"

void main(void) 
{
    uint8_t lastButtonState;
    
    initSystem();
    __delay_ms(5);
    
    // Initialize LCD display.
    initLCD();
    clearLCD();
    
    // Allow time to stable the output of MC34063.
    printString("Initializing...", 15);
    __delay_ms(1500);
    
    // Load custom characters into LCD CGRAM.
    setGraphicRAM(LCD_CCHAR_POINT, customCharPoint);
    
    setGraphicRAM(LCD_CCHAR_ARROW_FWD, customCharFwdArrow);
    setGraphicRAM(LCD_CCHAR_ZENER_FWD, customCharFwdZener);
    setGraphicRAM(LCD_CCHAR_DIODE_FWD, customCharFwdDiode);
    
    setGraphicRAM(LCD_CCHAR_ARROW_REV, customCharRevArrow);
    setGraphicRAM(LCD_CCHAR_ZENER_REV, customCharRevZener);
    setGraphicRAM(LCD_CCHAR_DIODE_REV, customCharRevDiode);
    
    setGraphicRAM(LCD_CCHAR_WIRE, customCharWire);
           
    lastButtonState = PORTA;
    scanAndPrintZener();
    
    // Start service loop.
    while(1)
    {
        if(((PORTA & 0x04) == 0x04) && ((lastButtonState & 0x04) == 0x00))
        {
            scanAndPrintZener();
        }
        
        lastButtonState = PORTA; 
        __delay_ms(100);
    }
        
    return;
}

void initSystem()
{    
    // Set internal oscillator to 4 MHz.
    OSCCON = 0x64;
    
    // Disable unused peripherals.
    CCP1CON = 0x00;
    SSPCON  = 0x00;
    
    // Set PORTB as output port and reserved for LCD controller.
    PORTB = 0x00;
    TRISB = 0x00;
    
    // Set PORTA[0..1] for analog inputs. Remaining pins are configured as digital I/O.
    PORTA = 0x00;
    TRISA = 0x07;
    
    // Configure ADC to use internal references and set up other conversion settings.
    ANSEL = 0x03;
    ADCON0 = 0x01;
    ADCON1 = 0xC0;
}

uint16_t getADCValue(uint8_t channel)
{
    ADCON0bits.CHS = channel;  
    ADCON0bits.ADON = 1;
    
    // Wait until conversion completes.
    __delay_ms(2);
    ADCON0bits.GO_DONE = 1;
    while(ADCON0bits.GO_DONE == 1);
    
    return (ADRESH << 8) + ADRESL;
}

void getCurrentZenerValues(uint16_t *zenerTerminalVal)
{
    zenerTerminalVal[0] = getADCValue(0);
    __delay_us(50);
    zenerTerminalVal[1] = getADCValue(1);
    __delay_us(50);
    
    uint8_t scanPass = 0;
    
    while(scanPass < 50)
    {
        zenerTerminalVal[0] = (getADCValue(0) + zenerTerminalVal[0]) / 2;
        __delay_us(50);
        zenerTerminalVal[1] = (getADCValue(1) + zenerTerminalVal[1]) / 2;
        __delay_us(50);
        
        scanPass++;
    }
}

void scanForZener(uint16_t *readData)
{
    uint16_t tempVal[2];
    
    readData[0] = 0;
    readData[1] = 0;
    
    // Scan input terminals in normal direction and get ADC values.
    PORTA = 0x40;
    __delay_ms(120);            
    getCurrentZenerValues(tempVal);
    PORTA = 0x00;
    
    if(tempVal[1] < tempVal[0])
    {
        readData[0] = tempVal[0] - tempVal[1];
    }
    else
    {
        readData[0] = tempVal[1] - tempVal[0];
    }
    
    // Scan input terminals in reverse direction and get ADC values.
    PORTA = 0x80;
    __delay_ms(120);
    getCurrentZenerValues(tempVal);
    PORTA = 0x00;
    
    if(tempVal[1] < tempVal[0])
    {        
        readData[1] = tempVal[0] - tempVal[1];
    }
    else
    {
        readData[1] = tempVal[1] - tempVal[0];
    }
}

uint32_t adcToVolts(uint16_t adcVal)
{
    uint32_t adcVolt = (30000000 / 1023)  * adcVal;
    return adcVolt;
}

void printFloat(uint32_t *val)
{
    char lineBuffer[18];
    
    // Split exponent and fraction part from the returned value.
    int16_t exp = *val / 1000000;
    int16_t frac = ((int32_t)(*val - (exp * 1000000))) / 1000;
    
    // Format output and print on current location of LCD.
    sprintf(lineBuffer, "%d.%d V", exp, frac);
    printString(lineBuffer, 18);
}

void scanAndPrintZener()
{
    uint16_t scanVal[2] = {0, 0};
    uint16_t maxVal;
    uint32_t voltFwd, voltRev;
    uint32_t maxVoltage;
    uint8_t temp, direction;
    
    // Clean LCD.
    clearLCD();
    returnHome();
    
    // Scan terminals and get voltage readings in forward and reverse directions.
    scanForZener(scanVal);
    voltFwd = adcToVolts(scanVal[0]);
    voltRev = adcToVolts(scanVal[1]);
    
    // Convert voltages to 100mV scale for comparison.
    scanVal[0] = voltFwd / 100000;
    scanVal[1] = voltRev / 100000;
              
    // Check for open terminals.
    if((scanVal[0] >= OPEN_VOLTAGE) && (scanVal[1] >= OPEN_VOLTAGE))
    {
        printString(" Connect Zener", 14);        
        return;
    }
    
    // Check for short-circuit.
    if((scanVal[0] <= SHORT_CIRCUIT_VOLTAGE) && (scanVal[1] <= SHORT_CIRCUIT_VOLTAGE))
    {
        printString(" Short Circuit", 14);
        
        setLocation(0, 2);
        printString("R ",2);
        sendChr(LCD_CCHAR_POINT);
        
        for(temp = 0; temp < 10; temp++)
        {
            sendChr(LCD_CCHAR_WIRE);
        }
        sendChr(LCD_CCHAR_POINT);
        printString(" B",2);
        return;
    }
    
    // Identify direction of the diode / zener.
    if(voltFwd > voltRev)
    {
        maxVal = scanVal[0];
        maxVoltage = voltFwd;
        direction = DIRECTION_FWD;        
    }
    else
    {
        maxVal = scanVal[1];
        maxVoltage = voltRev;
        direction = DIRECTION_REV;
    }
    
    // Maximum value is too close to upper voltage limit.
    if(maxVal >= OPEN_VOLTAGE)
    {
        // Detect as diode and show details.
        printString("Diode ", 6);
        printFloat(&maxVoltage);
        
        // Draw diode direction.
        setLocation(0, 2);
        printString("R ",2);
        sendChr(LCD_CCHAR_POINT);
                
        for(temp = 0; temp < 4; temp++)
        {
            sendChr(LCD_CCHAR_WIRE);
        }
        
        if(direction == DIRECTION_FWD)
        {
            // Diode is connected in forward direction.
            sendChr(LCD_CCHAR_ARROW_FWD);
            sendChr(LCD_CCHAR_DIODE_FWD);
        }
        else
        {
            // Diode is connected in reverse direction.
            sendChr(LCD_CCHAR_DIODE_REV);
            sendChr(LCD_CCHAR_ARROW_REV);
        }
        
        for(temp = 0; temp < 4; temp++)
        {
            sendChr(LCD_CCHAR_WIRE);
        }
        
        sendChr(LCD_CCHAR_POINT);
        printString(" B",2);
        return;
    }
    
    // Detect zener diode and show information.
    printString("Zener ", 6);
    printFloat(&maxVoltage);
    
    // Draw diode direction.
    setLocation(0, 2);
    printString("R ",2);
    sendChr(LCD_CCHAR_POINT);
        
    for(temp = 0; temp < 4; temp++)
    {
        sendChr(LCD_CCHAR_WIRE);
    }

    if(direction == DIRECTION_FWD)
    {
        // Diode is connected in forward direction.
        sendChr(LCD_CCHAR_ARROW_FWD);
        sendChr(LCD_CCHAR_ZENER_FWD);
    }
    else
    {
        // Diode is connected in reverse direction.
        sendChr(LCD_CCHAR_ZENER_REV);
        sendChr(LCD_CCHAR_ARROW_REV);
    }

    for(temp = 0; temp < 4; temp++)
    {
        sendChr(LCD_CCHAR_WIRE);
    }
    
    sendChr(LCD_CCHAR_POINT); 
    printString(" B",2);
}