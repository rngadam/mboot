/*
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */

#ifndef __TWI_H__
#define __TWI_H__

#define TWITIMEOUTMAX 50000

void Init_TWI(U32 twck);

U8 I2C_Read(U8 address, U32 iaddress, U8 isize, U8 *pData, U32 num);
U8 I2C_Write(U8 address, U32 iaddress, U8 isize, U8 *pData, U32 num);
void TWI_Stop(void);
void TWI_SendSTOPCondition(void);
void TWI_StartRead(U8 address, U32 iaddress, U8 isize);
U8 TWI_ReadByte(void);
void TWI_WriteByte(U8 byte);
void TWI_StartWrite(U8 address, U32 iaddress, U8 isize, U8 data);
U8 TWI_ByteReceived(void);
U8 TWI_ByteSent(void);
U8 TWI_TransferComplete(void);
void TWI_Reset(void);

#endif /* __TWI_H__ */
