/********************************************************************
* EEPROM.h
* DESCRIPTION: Contains the drivers and functions to read and write
* to an SPI EEProm. This was originally created for use with the
* 25LC320A.
* Notes: This software can be used in conjunction with the Nokia5110
* LCD software. In that case the Nokia 5110 and EEProm share the same
* CLK lines and Data out lines
* Author: Jonathan L Clark
* Date: 8/27/2014
* Update: 6/15/2016, Moved all P2OUT, P2IN type
* statements to the header so they can be changed
* globally. Modified the driver to switch between bit banging
* and USCI. (Note: The USCI code in place now is for the MSP430g2553
* and is not functional)
********************************************************************/
#ifndef EEPROM_H_
#define EEPROM_H_

#define USE_USCI 0
#define EEPROM_CS 0x40
#define EEPROM_SO 0x80
#define EEPROM_SCLK 0x10
#define EEPROM_SI 0x08
#define WRITE_DELAY 2000
#define EEPROM_MAX_ADDR 4095

//CS Pin defines
#define CS_HIGH  (P2OUT |= EEPROM_CS)
#define CS_LOW   (P2OUT &= ~EEPROM_CS)
#define CS_SETUP (P2DIR |= EEPROM_CS)

//Big bang pin defines
#define SI_SETUP (P2DIR |= EEPROM_SI)
#define SI_HIGH  (P2OUT |= EEPROM_SI)
#define SI_LOW   (P2OUT &= ~EEPROM_SI)

#define SO_PORT (P2IN)

#define SCLK_SETUP (P2DIR |= EEPROM_SCLK)
#define SCLK_HIGH  (P2OUT |= EEPROM_SCLK)
#define SCLK_LOW   (P2OUT &= ~EEPROM_SCLK)

void WriteEEProm_16Bit(unsigned int data);
void EEPromInit(void);
void WriteEEProm_8Bit(char data);
char ReadEEProm_8Bit(void);
char EEProm_Read(unsigned int address);
void EEProm_Write(unsigned int address, char data);
void EEProm_Write_16(unsigned int address, unsigned int data);
unsigned int EEProm_Read_16(unsigned int address);
void EEPromClear(void);

#endif /* EEPROM_H_ */
