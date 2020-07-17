/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.0
        Device            :  PIC16F15245
        Driver Version    :  2.00
        MPLABX IDE        :  5.40
        XC8 Compiler      :  2.20
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"

#define PH_ADDRESS  0x30
#define THERM_ADDRESS   0x32
#define PH_READS    0x3
#define THERM_READS 0x5
uint24_t newpH = 0;
uint24_t newTherm = 0;
uint8_t pHLowByte = 0;
uint16_t pHHighByte = 0;
uint24_t pHUpperByte = 0;
float pHValue = 0.00;
float thermValue = 0.00;
uint16_t lvlSensor = 0;

void main(void)
{
    SYSTEM_Initialize();

    while (1)
    {
        I2C_Read(PH_ADDRESS, 0x00, PH_READS);                                   // Read pH sensor
        (uint24_t)newpH = ((uint24_t)I2CData[2] << 16);                         // Shift upper byte into MSB of 24-bit value
        (uint24_t)newpH = (uint24_t)newpH + ((uint16_t)I2CData[1] << 8);        // Shift high byte into middle of 24-bit value
        (uint24_t)newpH = (uint24_t)newpH + I2CData[0];                         // Add the low byte into the 24-bit value
        (float)pHValue = (uint24_t)newpH;                                       // Change from int to float
        pHValue /= 100.00;                                                      // Slave value was multiplied by 100, so divide back
        printf("pH Value = %1.1f \r\n", pHValue);                               // Print value over EUSART
        
        I2C_Read(THERM_ADDRESS, 0x00, THERM_READS);                             // Read temp sensor
        (uint24_t)newTherm = ((uint24_t)I2CData[2] << 16);                      // Shift upper byte into MSB of 24-bit value
        (uint24_t)newTherm = (uint24_t)newTherm + ((uint16_t)I2CData[1] << 8);  // Shift high byte into middle of 24-bit value
        (uint24_t)newTherm = (uint24_t)newTherm + I2CData[0];                   // Add the low byte into the 24-bit value
        (float)thermValue = (uint24_t)newTherm;                                 // Change from int to float
        thermValue /= 100.00;                                                   // Slave value was multiplied by 100, so divide back
        printf("Temperature = %1.1fC \r\n", thermValue);                         // Print value over EUSART
        
        (uint16_t)lvlSensor = ((uint16_t)I2CData[4] << 8);                      // Combine bytes to form16-bit value
        (uint16_t)lvlSensor = (uint16_t)lvlSensor + I2CData[3];
        
        if(lvlSensor > 1010)                                                    // If level sensor completes circuit, low water
        {
            printf("WARNING: WATER LEVEL LOW!! ADD WATER! \r\n");
        }
        else                                                                    // Sensor input is floating, water level ok
        {
            printf("Water level OK! \r\n");
        }
        __delay_ms(3500);
        
    }
}
/**
 End of File
*/