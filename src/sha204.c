/* ----------------------------------------------------------------------------
 *		 
 *        mboot for Lophilo embedded system
 *		
 * ----------------------------------------------------------------------------
 */

#include "types.h"
#include "trace.h"
#include "option.h"

#include "system.h"
#include "sfr.h"
#include "twi.h"
#include "sha204.h"

#define SHA204_ADDRESS			(0xC9 >> 1)
#define SHA204_CRC_POLY		(0x8005)

#define SHA204_TIMEOUTMAX		50000

U8 SHA204_TX_data[64];
U8 SHA204_TX_crc[2];
U8 SHA204_TX_len;

U8 SHA204_RX_buff[64];
U8 SHA204_RX_data[64];
U8 SHA204_RX_crc[2];
U8 SHA204_RX_len;
U8 SHA204_crc[2];

U8 SHA204_SetStatus(U8 wdaddr)
{
	if(SHA204_TX_len == 0) return SHA204_DATA_NODATA;
	
	rTWI_MMR = 0;
	rTWI_MMR = (0 << 8) | (SHA204_ADDRESS << 16);
	rTWI_IADR = 0;
	rTWI_IADR = 0;

	TWI_Stop();
	TWI_WriteByte(wdaddr);
	while(!TWI_ByteSent());
	while (!TWI_TransferComplete());
	return SHA204_DATA_FINISH;
}

U8 SHA204_Reset(void)
{
	TWI_Stop();
	TWI_StartWrite(0, (0 << 8), 0, 0);
	while (!TWI_ByteSent());
	return SHA204_SetStatus(SHA204_SYS_RESET); 
}

void SHA204_CRC_Generate(U16 poly, U8 len, U8 *data)
{
	U8 cnt, reg_shift, bit_d, bit_crc;
	U16 reg_crc;

	reg_crc = 0;
	for(cnt = 0; cnt < len; cnt++) {
		for(reg_shift = 1; reg_shift > 0; reg_shift <<= 1) {
			bit_d = (data[cnt] & reg_shift) ? 1 : 0;
			bit_crc = reg_crc >>15;
			reg_crc<<= 1;
			if ((bit_d ^ bit_crc) != 0)
			reg_crc ^= poly;
		}
	}
	SHA204_crc[0] = (U8) (reg_crc & 0x00FF);
	SHA204_crc[1] = (U8) (reg_crc>> 8);
}

U8 SHA204_OneByteRX(void)
{
	volatile U32 val;
	U32 timeout;

	timeout = 0;
	
	rTWI_CR = (1 << 0) | (1 << 1);
	do{
		val = rTWI_SR;
	} while(!(val & (1 << 0)) && !(val & (1 << 8)));
	if(val & (1 << 8)) return SHA204_DATA_BUSY; // SHA204 is busy executing a command.

	while(!TWI_ByteReceived() && (++timeout < SHA204_TIMEOUTMAX));
	if(timeout == SHA204_TIMEOUTMAX){
		return SHA204_DATA_TIMEOUT;
	}
	else {
		return SHA204_DATA_FINISH;
	}
}

U8 SHA204_RawDataRX(void)
{
	U32 i;
	
	rTWI_MMR = 0;
	rTWI_MMR = (1 << 12) | (SHA204_ADDRESS << 16);
	rTWI_IADR = 0;

	if(SHA204_OneByteRX()==SHA204_DATA_FINISH){
		SHA204_RX_buff[0] = TWI_ReadByte();
		SHA204_RX_len = SHA204_RX_buff[0] - 3;
	}
	else{
		return SHA204_DATA_TIMEOUT;
	}
	
	if((SHA204_RX_len > 36)||(SHA204_RX_len < 1)) return SHA204_DATA_WRONG;

	for(i=0; i<SHA204_RX_len; i++)
	{
		if(SHA204_OneByteRX()==SHA204_DATA_FINISH){
			SHA204_RX_buff[i+1] = TWI_ReadByte();
			SHA204_RX_data[i] = SHA204_RX_buff[i+1];
		}
		else{
			return SHA204_DATA_TIMEOUT;
		}
	}
	
	if(SHA204_OneByteRX()==SHA204_DATA_FINISH){
		SHA204_RX_crc[0] = TWI_ReadByte();
	}
	else{
		return SHA204_DATA_TIMEOUT;
	}

	if(SHA204_OneByteRX()==SHA204_DATA_FINISH){
		SHA204_RX_crc[1] = TWI_ReadByte();
	}
	else{
		return SHA204_DATA_TIMEOUT;
	}

	SHA204_CRC_Generate(SHA204_CRC_POLY, SHA204_RX_len + 1, SHA204_RX_buff);

	if((SHA204_RX_crc[0] != SHA204_crc[0])||(SHA204_RX_crc[1] != SHA204_crc[1])){
		TRACE_WRN("SHA204 RX Data CRC error");
		return SHA204_DATA_WRONG;
	}
	else if((SHA204_RX_len==1)&&(SHA204_RX_data[0]==0xFF)){
		TRACE_WRN("SHA204 TX Data CRC error");
		return SHA204_DATA_WRONG;
	}
	else if((SHA204_RX_len==1)&&(SHA204_RX_data[0]==0x11)){
		//TRACE_WRN("SHA204 WAKEUP");	
		return SHA204_DATA_WAKEUP;
	}
	else{
		return SHA204_DATA_FINISH;
	}
}



U8 SHA204_RawDataTX(void)
{
	U32 i;
	U8 TX_CRC_buff[64];
	
	if(SHA204_TX_len == 0) return SHA204_DATA_NODATA;
	
	rTWI_MMR = 0;
	rTWI_MMR = (0 << 8) | (SHA204_ADDRESS << 16);
	rTWI_IADR = 0;
	rTWI_IADR = 0;

	TX_CRC_buff[0] = SHA204_TX_len + 3;

	for(i=0;i<SHA204_TX_len;i++)
	{
		TX_CRC_buff[i+1] = SHA204_TX_data[i];
	}

	SHA204_CRC_Generate(SHA204_CRC_POLY, SHA204_TX_len + 1, TX_CRC_buff);

	TWI_WriteByte(0x3);
	while(!TWI_ByteSent());

	TWI_WriteByte(SHA204_TX_len + 3);
	while(!TWI_ByteSent());

	i=0;
	while(i<SHA204_TX_len){
		TWI_WriteByte(SHA204_TX_data[i]);
		while(!TWI_ByteSent());
		i++;
	}

	TWI_WriteByte(SHA204_crc[0]);
	while(!TWI_ByteSent());
	TWI_Stop();
	TWI_WriteByte(SHA204_crc[1]);
	while (!TWI_TransferComplete());
	
	return SHA204_DATA_FINISH;
}

#define SHA204_RETRYMAX  		5


U8 SHA204_Cmd_Read(U8 zone, U16 address)
{
	U8 status = 0;
	U8 retry = 0;

	TWI_Reset();

	while((status!=SHA204_DATA_FINISH)&&(++retry < SHA204_RETRYMAX)){
		SHA204_TX_len = 4;
		SHA204_TX_data[0] = 0x2;
		SHA204_TX_data[1] = zone;
		SHA204_TX_data[2] = (U8) (address & (0x7F));
		SHA204_TX_data[3] = 0x0;
		SHA204_RawDataTX();
		Delay_ms(5);

		status = SHA204_RawDataRX();
		if(status!=SHA204_DATA_FINISH) 
		{
			//TRACE_WRN("SHA204_Cmd_Read() RETRY!");			
			//SHA204_Reset();
			Delay_ms(160);
		}
	}

	if(retry == SHA204_RETRYMAX){
		return SHA204_CMD_FAIL;
	}
	else {
		return SHA204_CMD_FINISH;
	}
}

void Init_SHA204(void)
{
	U8 status, retry;
	retry = 0;
	status = 0xFF;
	
	TWI_Reset();
	
	while((status != SHA204_DATA_WAKEUP)&&(++retry < 10))
	{
		status = SHA204_RawDataRX();
		if(status == SHA204_DATA_WAKEUP){
			TWI_Reset();
			Delay_ms(5);
			if(SHA204_Cmd_Read(0, 1)==SHA204_CMD_FINISH){
				TRACE_MSG("SHA204 security device detected ver%d.%d.%d.%d", SHA204_RX_data[3], SHA204_RX_data[2], SHA204_RX_data[1], SHA204_RX_data[0]);
			}
			else TRACE_ERR("SHA204 security device data error!");
		}
	}
}

void Debug_SHA204(void)
{
	U32 i, j;
	
	i = 0; j = 0;
	while(1)
	{
		if(SHA204_Cmd_Read(0, i)==SHA204_CMD_FINISH) 
		{
			PRINT_F("Chunk %d READOUT: ", i);
			
			for(j=0; j<SHA204_RX_len; j++)
			{
				PRINT_F(" %02x", SHA204_RX_data[j])
				
			}
			 SHELL_EMPTY();
			 
			if(i==8){
				i=0;
				SHELL_BNNER();
			}
			else{
				i++;
			}
		}
		else{
			TRACE_ERR("SHA204 DATA FINALLY ERROR!");
			PRINT_F("Error Chunk %d READOUT: ", i);
			
			for(j=0; j<SHA204_RX_len; j++)
			{
				PRINT_F(" %02x", SHA204_RX_data[j])
				
			}
			 SHELL_EMPTY();
		}
		
		Delay_ms(100);	
	}
}

