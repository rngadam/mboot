/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>

#include "types.h"
#include "trace.h"
#include "media.h"
#include "diskio.h"
#include "integer.h"
#include "ffconfig.h"

extern Media sdMed;

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE drv)
{
	DSTATUS stat = STA_NOINIT;
	switch (drv)
	{
		case DRV_SDRAM :
			stat = 0;
		break;
		
		case DRV_MMC :
			stat = 0;
		break;

		case DRV_NAND:
			stat = 0;
		break;
	}
	return stat;
}

/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE drv)
{
	DSTATUS stat = STA_NOINIT;
	switch(drv)
	{
		case DRV_MMC :
		stat = 0;
		break;  
	}
	return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE drv, BYTE *buff, DWORD sector, BYTE count)
{
	U8 result;
	DRESULT res = RES_ERROR;
	U32 addr, len;
	
	if(sdMed.blockSize < SECTOR_SIZE_DEFAULT)
	{
		addr = sector * (SECTOR_SIZE_DEFAULT / sdMed.blockSize);
		len  = count * (SECTOR_SIZE_DEFAULT / sdMed.blockSize);
	}
	else
	{
		addr = sector;
		len  = count;
	}
	result = MEDIA_Read(addr, (void*)buff, len, NULL, NULL);
	if( result == MED_STATUS_SUCCESS )
	{
		res = RES_OK;
	}
	else
	{
		TRACE_ERR("MED_Read pb: 0x%X", result);
		res = RES_ERROR;
	}
	return res;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
DRESULT disk_write(BYTE drv, const BYTE *buff, DWORD sector, BYTE count)
{
	DRESULT res=RES_PARERR;
	U32 result;
	void * tmp;
	tmp = (void *) buff;
	U32 addr, len;
	
	if(sdMed.blockSize < SECTOR_SIZE_DEFAULT)
	{
		addr = sector * (SECTOR_SIZE_DEFAULT / sdMed.blockSize);
		len  = count * (SECTOR_SIZE_DEFAULT / sdMed.blockSize);
	}
	else
	{
		addr = sector;
		len  = count;
	}

	result = MEDIA_Write(addr, (void*)tmp, len, NULL, NULL);

	if( result == MED_STATUS_SUCCESS )
	{
		res = RES_OK;
	}
	else
	{
		TRACE_ERR("MED_Write pb: 0x%X", result);
		res = RES_ERROR;
	}

	return res;
}
#endif /* _READONLY */

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
// Command    Description 
//
//CTRL_SYNC    Make sure that the disk drive has finished pending write process.
// When the disk I/O module has a write back cache, flush the dirty sector immediately.
// In read-only configuration, this command is not needed.
//
//GET_SECTOR_COUNT    Returns total sectors on the drive into the DWORD variable pointed by Buffer.
// This command is used in only f_mkfs function.
//
//GET_BLOCK_SIZE    Returns erase block size of the memory array in unit
// of sector into the DWORD variable pointed by Buffer.
// When the erase block size is unknown or magnetic disk device, return 1.
// This command is used in only f_mkfs function.
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(BYTE drv, BYTE ctrl, void *buff)
{ 
	DRESULT res=RES_PARERR;
	switch (drv)
	{
		case DRV_MMC:
			switch (ctrl)
			{ 
				case GET_BLOCK_SIZE:
					*(WORD*)buff = 1; 
					res = RES_OK; 
				break; 

				case GET_SECTOR_COUNT:   /* Get number of sectors on the disk (DWORD) */ 
					if (sdMed.blockSize < SECTOR_SIZE_DEFAULT)
						*(DWORD*)buff = (DWORD)(sdMed.size / (SECTOR_SIZE_DEFAULT / sdMed.blockSize));
					else
						*(DWORD*)buff = (DWORD)(sdMed.size);
					res = RES_OK;
				break; 

				case GET_SECTOR_SIZE:   /* Get sectors on the disk (WORD) */ 
					if (sdMed.blockSize < SECTOR_SIZE_DEFAULT)
						*(WORD*)buff = SECTOR_SIZE_DEFAULT;
					else
						*(WORD*)buff = sdMed.blockSize;
					res = RES_OK;
				break;

				case CTRL_SYNC:   /* Make sure that data has been written */ 
					res = RES_OK; 
				break; 

				default: 
					res = RES_PARERR; 
			}
		break;
	} 
	return res; 
}

//------------------------------------------------------------------------------
/// Currnet time is returned with packed into a DWORD value.
/// The bit field is as follows:
///   bit31:25  Year from 1980 (0..127)
///   bit24:21  Month (1..12)
///   bit20:16  Day in month(1..31)
///   bit15:11  Hour (0..23)
///   bit10:5   Minute (0..59)
///   bit4:0    Second / 2 (0..29)
//------------------------------------------------------------------------------
DWORD get_fattime(void)
{
	U32 time;

	time = ((2012-1980)<<25) | (8<<21) | (9<<16) | (20<<11) | (30<<5) | ((59/2)<<0);
	return time;
}
