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
********************************************************************/
#ifndef EEPROM_H_
#define EEPROM_H_

#define EEPROM_CS 0x40
#define WRITE_DELAY 2000
#define EEPROM_MAX_ADDR 4095


void EEPromInit(void);
uint8_t EEProm_Read(uint16_t address);
void EEProm_Write(uint16_t address, uint8_t data);
void EEProm_Write_16(uint16_t address, uint16_t data);
uint16_t EEProm_Read_16(uint16_t address);
void EEPromClear(void);
void WriteEEProm_Array(uint16_t address, uint8_t* data, uint8_t size);
void ReadEEProm_Array(uint16_t address, uint8_t* outputData, uint8_t size);

#endif /* EEPROM_H_ */
