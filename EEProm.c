/*********************************************
* EEProm.c
*
* Author: Jonathan L Clark
* Date: Apr 21, 2015
* Update: 6/15/2016, Moved all P2OUT, P2IN type
* statements to the header so they can be changed
* globally. Modified the driver to switch between bit banging
* and USCI. (Note: The USCI code in place now is for the MSP430g2553
* and is not functional)
*************************************************/

#include <msp430.h>
#include "EEProm.h"

int EEProm_i = 0;
char EEProm_In = 0;

/***********************************************
* WRITE EEProm 16 bit
* DESCRIPTION: Write 16 bits of data to the
* EEProm.
***********************************************/
void WriteEEProm_16Bit(unsigned int data)
{
#if USE_USCI==1
	unsigned char hi = data >> 8;
	unsigned char lo = data;

	while (!(IFG2 & UCB0TXIFG));               // USCI_A0 TX buffer ready?
	         UCB0TXBUF = hi;                     // Transmit data

	while (!(IFG2 & UCB0TXIFG));               // USCI_A0 TX buffer ready?
	         UCB0TXBUF = lo;                     // Transmit data

#else
   for (EEProm_i = 0; EEProm_i < 16; EEProm_i++)
   {
      if ((data & 0x80) == 0x80)
	  {
    	  SI_HIGH;
	  }
	  else
	  {
		  SI_LOW;
	  }
      SCLK_HIGH;
	  data = data << 1;
	  SCLK_LOW;
   }
#endif

}

/***********************************************
* WRITE EE PROM DATA
* DESCRIPTION: Write data to the EEProm.
* (Assuming that CS is set LOW)
************************************************/
void WriteEEProm_8Bit(char data)
{
#if USE_USCI==1
	while (!(IFG2 & UCB0TXIFG));            // USCI_A0 TX buffer ready?
	UCB0TXBUF = data;                     // Transmit data

#else
   for (EEProm_i = 0; EEProm_i < 8; EEProm_i++)
   {
      if ((data & 0x80) == 0x80)
      {
    	  SI_HIGH;
      }
      else
      {
    	  SI_LOW;
      }
      SCLK_HIGH;
      data = data << 1;
      SCLK_LOW;
   }
#endif

}

/***********************************************
* READ EEProm 8 bit
* DESCRIPTION: Read 8 bits of EE prom data
***********************************************/
char ReadEEProm_8Bit(void)
{
#if USE_USCI==1
	while (!(IFG2 & UCB0TXIFG));               // USCI_A0 TX buffer ready?
	         UCB0TXBUF = 0xFF;                     // Dummy write

	//May need delay here
	while  (!(IFG2 & UCB0RXIFG));

	return UCB0RXBUF;

#else
   char output = 0x00;
   for (EEProm_i = 0; EEProm_i < 8; EEProm_i++)
   {
	  SCLK_HIGH;
	  output = output << 1;
	  EEProm_In = SO_PORT;
      if ((EEProm_In & EEPROM_SO) == EEPROM_SO)
      {
         output = output | 0x01;
      }
      SCLK_LOW;
   }

   return output;
#endif

}

/**********************************************
* EEProm READ
* DESCRIPTION: Read the EEProm data from the
* given address.
**********************************************/
char EEProm_Read(unsigned int address)
{
   CS_LOW;
   WriteEEProm_8Bit(0x03);      //Write command
   WriteEEProm_16Bit(address);  //Set address to write
   char output = ReadEEProm_8Bit();
   CS_HIGH;

   return output;

}

/***********************************************
* EEProm WRITE
* DESCRIPTION: Write data to the EEProm at the
* given addrress.
************************************************/
void EEProm_Write(unsigned int address, char data)
{
   CS_LOW;
   WriteEEProm_8Bit(0x06);     //Write enable command
   CS_HIGH;
   CS_LOW;
   WriteEEProm_8Bit(0x02);     //Write command
   WriteEEProm_16Bit(address); //Set address to write
   WriteEEProm_8Bit(data);     //Write EEProm data
   CS_HIGH;
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
void EEProm_Write_16(unsigned int address, unsigned int data)
{
   char top = data >> 8;
   char bottom = data & 0x00ff;
   EEProm_Write(address, top);
   address++;
   EEProm_Write(address, bottom);
}

/**********************************************
* EEProm READ 16
* DESCRIPTION: Read 16 bits of data from the
* EEProm at the given address.
**********************************************/
unsigned int EEProm_Read_16(unsigned int address)
{
	unsigned int output = EEProm_Read(address);
	address++;
	unsigned int value2 = EEProm_Read(address);
	output = output << 8;
	output = output | value2;

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
   SCLK_SETUP;
   SI_SETUP;
   CS_SETUP;
   CS_HIGH;
   SCLK_LOW;
   CS_LOW;
   WriteEEProm_8Bit(0x06); //Send WREN instruction to EEProm
   CS_HIGH;

}
