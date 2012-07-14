/* ----------------------------------------------------------------------------
 *		 
 *        mboot for Lophilo embedded system
 *		
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "types.h"
#include "option.h"
#include "shell.h"
#include "trace.h"
#include "cp15.h"

#include "sfr.h"
#include "sha204.h"
#include "fpga.h"

#include "ff.h"
#include "integer.h"
#include "ffconf.h"

#include "mboot.h"

#define STR_ROOT_DIRECTORY		"0:"
#define KCMD_ETH0_MAC_TAG			"hwaddr="

const S8* gridName = STR_ROOT_DIRECTORY "grid.rbf";
const S8* kcmdName = STR_ROOT_DIRECTORY "kcmd.txt";
const S8* zImageName = STR_ROOT_DIRECTORY "zImage";

U32 gridFileSize, kcmdFileSize;
S8 kcmd_eth_mac[128];
U32 board_id_l, board_id_h;
U8 id_buf[8];

extern U8 SHA204_TX_data[64];
extern U8 SHA204_TX_len;
extern U8 SHA204_RX_data[64];
extern U8 SHA204_RX_len;

extern void BootmSet_ParamAddr(U32 pm_addr);
extern void BootmSet_MachineID(U32 mach_id);

static void Get_SysID(void);
static void Build_Kcmd_Mac(void);
static void Boot_Linux(void);
static FRESULT scan_files(char* path);

static void (*run)(void);

void mboot(void)
{
	U32 i;
	U32 ByteToRead;
	U32 ByteRead;

	FRESULT res;
	DIR dirs;
	FATFS fs;
	FIL FileObject;
	
	Get_SysID();
	
	memset(&fs, 0, sizeof(FATFS));
	for(i=ZPARAMADDR; i < ZRELADDR; i++) *((volatile S8 *)(i)) = 0;
	
	res = f_mount(0, &fs);
	if( res != FR_OK )
	{
		TRACE_ERR("Mount OS SD card failed: 0x%X", res);
		return;
	}
	res = f_opendir(&dirs, STR_ROOT_DIRECTORY);
	if(res == FR_OK )
	{
		scan_files(STR_ROOT_DIRECTORY);
		
		TRACE_MSG("Load FPGA code (grid.bin) to buffer.");
		res = f_open(&FileObject, gridName, FA_OPEN_EXISTING|FA_READ);
		if(res != FR_OK)
		{
			TRACE_ERR("Open grid.rbf file failed: 0x%X", res);
			return;
		}		
		ByteToRead = FileObject.fsize;
		gridFileSize = FileObject.fsize;
		res = f_read(&FileObject, (void*)GRIDADDR, ByteToRead, &ByteRead);
		if(res != FR_OK)
		{
			TRACE_ERR("Load grid.rbf file failed: 0x%X", res);
			return;
		}
		else
		{
			TRACE_FIN("FPGA code file load OK.");
			FPGA_Config();
		}

		TRACE_MSG("Load kernel command line.");
		res = f_open(&FileObject, kcmdName, FA_OPEN_EXISTING|FA_READ);
		if(res != FR_OK)
		{
			TRACE_ERR("Open kernel command line file failed: 0x%X", res);
			return;
		}		
		ByteToRead = FileObject.fsize;
		kcmdFileSize = FileObject.fsize;
		res = f_read(&FileObject, (void*)(ZPARAMADDR + 4*11), ByteToRead, &ByteRead);
		
		Build_Kcmd_Mac();
		
		if(res != FR_OK)
		{
			TRACE_ERR("Load kernel command line file failed: 0x%X", res);
			return;
		}
		
		TRACE_MSG("Load zImage to ZRELADDR: 0x%X.", ZRELADDR);
		res = f_open(&FileObject, zImageName, FA_OPEN_EXISTING|FA_READ);
		if(res != FR_OK)
		{
			TRACE_ERR("Open zImage file failed: 0x%X", res);
			return;
		}
		ByteToRead = FileObject.fsize;
		res = f_read(&FileObject, (void*)(ZRELADDR), ByteToRead, &ByteRead);
		if(res != FR_OK)
		{
			TRACE_ERR("Load zImage file failed: 0x%X", res);
			return;
		}
		else 
		{
			TRACE_FIN("zImage file Load OK, now let's just enjoy Linux :)\n\r");
			Boot_Linux();
		}
	}
}

static void Build_Kcmd_Mac(void)
{
	U32 i, j;
	
	String_Build(kcmd_eth_mac, " %s%02X:%02X:%02X:%02X:%02X:%02X\0", KCMD_ETH0_MAC_TAG, id_buf[7], id_buf[6], id_buf[3], id_buf[2], id_buf[1], id_buf[0]);
	j = strlen(kcmd_eth_mac);
	
	for(i=0; i<j; i++){
		*((volatile U8 *)(ZPARAMADDR + 4*11 + kcmdFileSize + i)) = kcmd_eth_mac[i];
	}
	
	kcmdFileSize = kcmdFileSize + j;
	
// 	U8 tmp[300];
// 	memset(tmp, 0, sizeof(tmp));
// 	for(i=0; i<kcmdFileSize; i++){
// 		tmp[i] = *((volatile U8 *)(ZPARAMADDR + 4*11 + i));
// 	}
// 	TRACE_MSG("Kernel command line: %s", tmp);
}

static void Get_SysID(void)
{

	U8 status;
	
	board_id_l = 0;
	board_id_h = 0;
	
	status = SHA204_Cmd_Read(0, 0);
	id_buf[0] = SHA204_RX_data[2];
	id_buf[1] = SHA204_RX_data[3];

	status = SHA204_Cmd_Read(0, 2);
	id_buf[2] = SHA204_RX_data[0];
	id_buf[3] = SHA204_RX_data[1];
	id_buf[4] = SHA204_RX_data[2];
	id_buf[5] = SHA204_RX_data[3];	
	id_buf[6] = 0x68;
	id_buf[7] = 0xEA;

	if(status==SHA204_CMD_FINISH){
		TRACE_MSG("ATAG_SERIAL/Hardware UUID: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", id_buf[7], id_buf[6], id_buf[5], id_buf[4], id_buf[3], id_buf[2], id_buf[1], id_buf[0]);
		board_id_l = ((id_buf[3]<<24) + (id_buf[2]<<16) + (id_buf[1]<<8) + id_buf[0]);
		board_id_h = ((id_buf[7]<<24) + (id_buf[6]<<16) + (id_buf[5]<<8) + id_buf[4]);
	}
	else TRACE_ERR("Load ID from SHA204 data error!");
}

static void Boot_Linux(void)
{
	U32 i,j;
	
	*((volatile U32 *)(ZPARAMADDR + 4*0)) = 2;
	*((volatile U32 *)(ZPARAMADDR + 4*1)) = ATAG_CORE;
	
	*((volatile U32 *)(ZPARAMADDR + 4*2)) = 4;
	*((volatile U32 *)(ZPARAMADDR + 4*3)) = ATAG_SERIAL;	
	*((volatile U32 *)(ZPARAMADDR + 4*4)) = board_id_l;
	*((volatile U32 *)(ZPARAMADDR + 4*5)) = board_id_h;	
	
	*((volatile U32 *)(ZPARAMADDR + 4*6)) = 3;
	*((volatile U32 *)(ZPARAMADDR + 4*7)) = ATAG_REVISION;	
	*((volatile U32 *)(ZPARAMADDR + 4*8)) = BOARD_VER;	
	
	*((volatile U32 *)(ZPARAMADDR + 4*9)) = (4 + kcmdFileSize + 5) >> 2;
	*((volatile U32 *)(ZPARAMADDR + 4*10)) = ATAG_CMDLINE;

	for(i=0;i<64;i++) for(j=0;j<8;j++) CP15_CleanInvalidateDcacheIndex((i<<26)|(j<<5));
	CP15_DisableDcache();
	CP15_DisableIcache();
	CP15_InvalidateIcache();
	CP15_DisableMMU();
	CP15_InvalidateTLB(); 
	run = (void (*)(void))ZRELADDR;
	BootmSet_ParamAddr(ZPARAMADDR);
	BootmSet_MachineID(LINUX_MACH_ID);
	run();
}

static FRESULT scan_files(char* path)
{
	FRESULT res;
	FILINFO fno;
	DIR dir;
	int i;
	char *fn;

	res = f_opendir(&dir, path);
	if (res == FR_OK)
	{
		i = strlen(path);
		for (;;)
		{
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) break;
			fn = fno.fname;
			if (*fn == '.') continue;
			if (fno.fattrib & AM_DIR)
			{
				TRACE_MSG(&path[i], "/%s", fn);
				res = scan_files(path);
				if (res != FR_OK) break;
				path[i] = 0;
			}
			else
			{
				DEBUG_MSG("%s/%s", path, fn);
			}
		}
	}

	return res;
}
