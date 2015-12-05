/*******************************************************************
* EEProm.c
* Author: Jonathan L Clark
* Description: This is an EEProm driver for the MSP432
*
* Date: 4/21/2015
* Update: 12/4/2015, Added the array-write/read functions, removed exposed
* internal functions from EEProm.h.
********************************************************************/

#include "msp.h"
#include "EEProm.h"
#include "../../System/System.h"

/***********************************************
* WRITE EEProm 16 bit
* DESCRIPTION: Write 16 bits of data to the
* EEProm.
***********************************************/
void EEProm_SPI_WriteNibble(uint16_t data)
{
   uint8_t hi = data >> 8;
   uint8_t lo = data;


   while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
            UCB0TXBUF = hi;                     // Transmit data

   while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
            UCB0TXBUF = lo;                     // Transmit data


}

/***********************************************
* WRITE EE PROM DATA
* DESCRIPTION: Write data to the EEProm.
* (Assuming that CS is set LOW)
************************************************/
void EEProm_SPI_WriteByte(uint8_t data)
{
   while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
            UCB0TXBUF = data;                     // Transmit data
}

/***********************************************
* READ EEProm 8 bit
* DESCRIPTION: Read 8 bits of EE prom data
***********************************************/
uint8_t EEProm_SPI_ReadByte(void)
{
   while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
	         UCB0TXBUF = 0xFF;                     // Dummy write

   //May need delay here
   while  (!(UCB0IFG&UCRXIFG));

	return UCB0RXBUF;

}

/************************************************
* EEPROM WRITE ARRAY
* DESCRIPTION: Writes an array of values to the EEProm memory
* starting at the input address
*************************************************/
void WriteEEProm_Array(uint16_t address, uint8_t* data, uint8_t size)
{
	SetClockSpeed(0); //Clock Speed must be < 10Mhz to support EEProm
	uint8_t i;
    P2OUT &= ~EEPROM_CS;        //EEProm CS is pulled low for write sequence
    EEProm_SPI_WriteByte(0x06);     //Write enable command
    P2OUT |= EEPROM_CS;         //EEProm CS is pulled high to lock in data
    P2OUT &= ~EEPROM_CS;        //EEProm CS is pulled low for write sequence
    EEProm_SPI_WriteByte(0x02);     //Write command
    EEProm_SPI_WriteNibble(address); //Set address to write
    for (i = 0; i < size; i++)
    {
       while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
    	        UCB0TXBUF = data[i];             // Transmit data
    }
    //EEProm_SPI_WriteByte(data);     //Write EEProm data
    P2OUT |= EEPROM_CS;         //EEProm CS is pulled high to block further writes
    int index = 0;
	while (index < WRITE_DELAY)
	{
	   index++;
	}
	SetClockSpeed(5); //Increase Clock speed back to full
}

/************************************************
* EEPROM READ ARRAY
* DESCRIPTION: Reads an array of values to the EEProm memory
* starting at the input address
*************************************************/
void ReadEEProm_Array(uint16_t address, uint8_t* outputData, uint8_t size)
{
   SetClockSpeed(0); //Clock Speed must be < 10Mhz to support EEProm
   uint8_t i;
   P2OUT &= ~EEPROM_CS; //EEProm CS is pulled low for read sequence
   EEProm_SPI_WriteByte(0x03); //Write command
   EEProm_SPI_WriteNibble(address); //Set address to write
   for (i = 0; i < size; i++)
   {
       while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
		       UCB0TXBUF = 0xFF;                     // Dummy write

	   //May need delay here
	   while  (!(UCB0IFG&UCRXIFG));
	   outputData[i] = UCB0RXBUF;

   }
   P2OUT |= EEPROM_CS; //EEProm CS is pulled high to block further writes
   SetClockSpeed(5); //Increase Clock speed back to full
}

/**********************************************
* EEProm READ
* DESCRIPTION: Read the EEProm data from the
* given address.
**********************************************/
uint8_t EEProm_Read(uint16_t address)
{
   P2OUT &= ~EEPROM_CS; //EEProm CS is pulled low for read sequence
   EEProm_SPI_WriteByte(0x03); //Write command
   EEProm_SPI_WriteNibble(address); //Set address to write
   uint8_t output = EEProm_SPI_ReadByte();
   P2OUT |= EEPROM_CS; //EEProm CS is pulled high to block further writes

   return output;

}

/***********************************************
* EEProm WRITE
* DESCRIPTION: Write data to the EEProm at the
* given addrress.
************************************************/
void EEProm_Write(uint16_t address, uint8_t data)
{
   P2OUT &= ~EEPROM_CS;        //EEProm CS is pulled low for write sequence
   EEProm_SPI_WriteByte(0x06);     //Write enable command
   P2OUT |= EEPROM_CS;         //EEProm CS is pulled high to lock in data
   P2OUT &= ~EEPROM_CS;        //EEProm CS is pulled low for write sequence
   EEProm_SPI_WriteByte(0x02);     //Write command
   EEProm_SPI_WriteNibble(address); //Set address to write
   EEProm_SPI_WriteByte(data);     //Write EEProm data
   P2OUT |= EEPROM_CS;         //EEProm CS is pulled high to block further writes
   int index = 0;
   while (index < WRITE_DELAY)
   {
      index++;
   }
}

/**********************************************
* EEProm WRITE 16
* DESCRIPTION: Writes 16 bits of data to the
* EEProm at the given address.
***********************************************/
void EEProm_Write_16(uint16_t address, uint16_t data)
{
	SetClockSpeed(0); //Clock Speed must be < 10Mhz to support EEProm
   uint8_t top = data >> 8;
   uint8_t bottom = data & 0x00ff;
   EEProm_Write(address, top);
   address++;
   EEProm_Write(address, bottom);
   SetClockSpeed(5); //Increase Clock speed back to full
}

/**********************************************
* EEProm READ 16
* DESCRIPTION: Read 16 bits of data from the
* EEProm at the given address.
**********************************************/
uint16_t EEProm_Read_16(uint16_t address)
{
	 SetClockSpeed(0); //Clock Speed must be < 10Mhz to support EEProm
    uint16_t output = EEProm_Read(address);
	address++;
	uint16_t value2 = EEProm_Read(address);
	output = output << 8;
	output = output | value2;
	SetClockSpeed(5); //Increase Clock speed back to full

	return output;

}

/***********************************************
* EEProm Clear
* DESCRIPTION: Sets all the data in the EEProm to
* 0x00.
************************************************/
void EEPromClear(void)
{
	unsigned int index = 0;
	for (index = 0; index < EEPROM_MAX_ADDR; index++)
	{
		EEProm_Write(index, 0x00);
	}
}

/***********************************************
* EEPROM INIT
* DESCRIPTION: Setup the EEProm for usse.
***********************************************/
void EEPromInit(void)
{
	//SetClockSpeed(0);
   //__disable_interrupts();
   P2DIR |= EEPROM_CS;
   P2OUT |= EEPROM_CS; //EEProm CS is set high to prevent reading

   P2OUT &= ~EEPROM_CS; //EEProm CS is set high to prevent reading
   EEProm_SPI_WriteByte(0x06); //Send WREN instruction to EEProm
   P2OUT |= EEPROM_CS; //EEProm CS is set high to prevent reading
   //__enable_interrupts();
   //SetClockSpeed(5);

}

