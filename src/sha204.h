/*
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */

#ifndef __SHA204_H__
#define __SHA204_H__

#define SHA204_SYS_NORMAL		0x3
#define SHA204_SYS_IDLE		0x2
#define SHA204_SYS_SLEELP		0x1
#define SHA204_SYS_RESET		0x0

#define SHA204_DATA_FINISH		0x01
#define SHA204_DATA_WAKEUP	0x05
#define SHA204_DATA_BUSY		0xA0
#define SHA204_DATA_TIMEOUT	0xA1
#define SHA204_DATA_NODATA	0xB0
#define SHA204_DATA_WRONG	0xB0

#define SHA204_CMD_FAIL		0
#define SHA204_CMD_FINISH		1

U8 SHA204_RawDataRX(void);
U8 SHA204_RawDataTX(void);
U8 SHA204_Cmd_Read(U8 zone, U16 address);
void Init_SHA204(void);
void Debug_SHA204(void);

#endif /* __SHA204_H__ */
