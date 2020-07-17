/* 
 * File:   i2c.c
 * Author: C13066
 *
 * Created on September 4, 2015, 1:50 PM
 */
/*
Copyright (c) 2013 - 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*/

#include "mcc.h"

// MSSP defines and macros
#define Idle        !(SSP1STATbits.R_nW | (0x1F & SSP1CON2))                    // I2C Idle
#define I2C_Start   (SSP1CON2bits.SEN)                                          // I2C Start 
#define I2C_Restart (SSP1CON2bits.RSEN)                                         // I2C Restart
#define I2C_Stop    (SSP1CON2bits.PEN)                                          // I2C Stop

void I2C_Init(void) 
{   
    SSP1STAT = 0x80;                                                            // Sample end of data output time 
    SSP1CON1 = 0x28;                                                            // SSPEN enabled; I2C master clk =FOSC/4(SSPxADD+1)  
    SSP1CON3 = 0x00; 
    SSP1ADD = 0x09;                                                             // 0x4F = 100 kHz Clock @ 32 MHz
    
    PIR1bits.SSP1IF = 0;                                                        // clear the master interrupt flag
    PIE1bits.SSP1IE = 0;                                                        // Disable the master interrupt
}

void I2C_Read(uint8_t deviceADDR, uint8_t registerADDR, uint8_t numReads)       // Read multiple bytes from specific location
{
    while (!Idle);                                                              // Wait until MSSP module is idle

    I2C_Start = 1;                                                              // Set START bit
    while (I2C_Start);                                                          // Wait until hardware clears the SEN bit 

    SSP1BUF = deviceADDR;                                                          // Load device address (write)
    while (!Idle);                                                              // Wait until MSSP module is idle

    SSP1BUF = registerADDR;                                                          // Send register address (write)
    while (!Idle);                                                              // Wait until MSSP module is idle

    I2C_Restart = 1;                                                            // Restart sequence
    while (I2C_Restart);

    SSP1BUF = (deviceADDR | 0x01);                                                 // Load device address (read)
    while (!Idle);                                                              // Wait until MSSP module is idle


    for (uint8_t I2CReads = 0; I2CReads < numReads; I2CReads++)                 // Read x number of bytes
    {

        SSP1CON2bits.RCEN = 1;                                                  // Set Receive Enable bit (Master as a receiver)
        while (!Idle);                                                          // Wait until MSSP module is idle

        I2CData[I2CReads] = SSP1BUF;                                            // Put each byte into array

        if (I2CReads == numReads - 1) 
        {
            SSP1CON2bits.ACKDT = 1;                                             // Acknowledge bit, 1 = Not acknowledge
            SSP1CON2bits.ACKEN = 1;                                             // Start Acknowledge sequence 
            while (SSP1CON2bits.ACKEN);                                         // Wait for end of acknowledge sequence 
        } 
        else 
        {
            SSP1CON2bits.ACKDT = 0;
            SSP1CON2bits.ACKEN = 1;                                             // Start Acknowledge sequence 
            while (!Idle);                                                      // Wait until MSSP module is idle
        }
    }
    I2C_Stop = 1;                                                               // Set STOP bit 
    while (!Idle);                                                              // Wait until MSSP module is idle
    NOP();
}
