/* ----------------------------------------------------------------------------
 *		 
 *        mboot for Lophilo embedded system
 *		
 * ----------------------------------------------------------------------------
 */

#include "types.h"
#include "trace.h"
#include "option.h"
#include "math.h"

#include "sfr.h"
#include "system.h"
#include "twi.h"

void Init_TWI(U32 twck)
{
	U32 ckdiv;
	U32 cldiv;
	U8 ok;
	
	PMC_PeriEn(12);
	Delay_10us(1);
	rTWI_CR = (1 << 7); // Reset TWI.
	
	TWI_Reset();
	
	DEBUG_MSG("TWI_ConfigureMaster()");

	rTWI_CR = (1 << 4); // SVEN: TWI Slave Mode Enabled
	rTWI_CR = (1 << 7); // Reset the TWI
	cldiv = rTWI_RHR;
	rTWI_CR = (1 << 5); // TWI Slave Mode Disabled, TWI Master Mode Disabled
	ckdiv = 0;
	rTWI_CR = (1 << 3);
	ok = 0;
	rTWI_CR = (1 << 2); // Set master mode

	while (!ok) // Configure clock
	{
		cldiv = (((BOARD_MCK) / (2 * twck)) - 3) / power(2, ckdiv);
		if (cldiv <= 255)
		{
			ok = 1;
		}
		else
		{
			ckdiv++;
		}
	}
	
	DEBUG_MSG("Using CKDIV = %u and CLDIV/CHDIV = %u", ckdiv, cldiv);
	rTWI_CWGR = 0;
	rTWI_CWGR = (ckdiv << 16) | (cldiv << 8) | cldiv;
}

U8 I2C_Read(U8 address, U32 iaddress, U8 isize, U8 *pData, U32 num)
{
	U32 timeout;
	if(num == 1) TWI_Stop(); // Set STOP signal if only one byte is sent
	TWI_StartRead(address, iaddress, isize); // Start read
	while(num > 0) // Read all bytes, setting STOP before the last byte
	{
		if(num == 1) TWI_Stop(); // Last byte
		timeout = 0; // Wait for byte then read and store it
		while(!TWI_ByteReceived() && (++timeout<TWITIMEOUTMAX));
		if(timeout == TWITIMEOUTMAX)
		{
			TRACE_ERR("TWID Timeout BR");
			return 1;
		}
		*pData++ = TWI_ReadByte();
		num--;
	}
	timeout = 0; // Wait for transfer to be complete
	while(!TWI_TransferComplete() && (++timeout<TWITIMEOUTMAX));
	if(timeout == TWITIMEOUTMAX)
	{
		TRACE_ERR("TWID Timeout TC");
		return 1;
	}
	return 0;
}

U8 I2C_Write(U8 address, U32 iaddress, U8 isize, U8 *pData, U32 num)
{
	U32 timeout;
	TWI_StartWrite(address, iaddress, isize, *pData++); // Start write
	num--;
	while(num > 0) // Send all bytes
	{
		timeout = 0; // Wait before sending the next byte
		while(!TWI_ByteSent() && (++timeout<TWITIMEOUTMAX));
		if(timeout == TWITIMEOUTMAX)
		{
			TRACE_ERR("TWID Timeout BS.");
			return 1;
		}
		TWI_WriteByte(*pData++);
		num--;
	}
	timeout = 0; // Wait for actual end of transfer
	TWI_SendSTOPCondition(); // Send a STOP condition
	while(!TWI_TransferComplete() && (++timeout<TWITIMEOUTMAX));
	if (timeout == TWITIMEOUTMAX)
	{
		TRACE_ERR("TWID Timeout TC2.");
		return 1;
	}
	return 0;
}

void TWI_Stop(void)
{
	rTWI_CR = (1 << 1);
}

void TWI_SendSTOPCondition(void)
{
	rTWI_CR |= (1 << 1);
}

void TWI_StartRead(U8 address, U32 iaddress, U8 isize)
{
	rTWI_MMR = 0; // Set slave address and number of internal address bytes
	rTWI_MMR = (isize << 8) | (1 << 12) | (address << 16);
	rTWI_IADR = 0; // Set internal address bytes
	rTWI_IADR = iaddress;
	rTWI_CR = (1 << 0); // Send START condition
}

U8 TWI_ReadByte(void)
{
	return rTWI_RHR;
}

void TWI_WriteByte(U8 byte)
{
	rTWI_THR = byte;
}

void TWI_StartWrite(U8 address, U32 iaddress, U8 isize, U8 data)
{
	rTWI_MMR = 0; // Set slave address and number of internal address bytes
	rTWI_MMR = (isize << 8) | (address << 16);
	rTWI_IADR = 0; // Set internal address bytes
	rTWI_IADR = iaddress;
	TWI_WriteByte(data); // Write first byte to send
}

U8 TWI_ByteReceived(void)
{
	return ((rTWI_SR & (1 << 1)) == (1 << 1));
}

U8 TWI_ByteSent(void)
{
	return ((rTWI_SR & (1 << 2)) == (1 << 2));
}

U8 TWI_TransferComplete(void)
{
	return ((rTWI_SR & (1 << 0)) == (1 << 0));
}

void TWI_Reset(void)
{
	rPIOA_PER = 0x300000; // Configure TWI pins as GPIO to be able to generate
	rPIOA_PUER = 0x300000;
	rPIOA_OER = 0x300000;
	rPIOA_SODR = 0x300000;
	Delay_10us(10);
	rPIOA_CODR = 0x300000; // a Wake-up pulse.
	Delay_10us(100);
	rPIOA_SODR = 0x300000;
	Delay_ms(30); // Delay before starting any communication.
	
	rPIOA_ODR = 0x300000; // Reconfigure pins as TWI.
	rPIOA_PDR = 0x300000;
	rPIOA_ASR = 0x300000;
	Delay_ms(10);
}
