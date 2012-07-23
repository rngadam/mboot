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
#include "emac.h"
#include "fpga.h"

#include "ff.h"
#include "integer.h"
#include "ffconf.h"

#include "init.h"
#include "mboot.h"

#define STR_ROOT_DIRECTORY		"0:"

const S8* gridName = STR_ROOT_DIRECTORY "grid.rbf";
const S8* kcmdName = STR_ROOT_DIRECTORY "kcmd.txt";
const S8* zImageName = STR_ROOT_DIRECTORY "zImage";

U32 gridFileSize, kcmdFileSize;

extern U32 board_id_l, board_id_h;

extern void BootmSet_ParamAddr(U32 pm_addr);
extern void BootmSet_MachineID(U32 mach_id);

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
			BTNDIS_N();
			Boot_Linux();
		}
	}
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
	
// 	*((volatile U32 *)(ZPARAMADDR + 4*9)) = (4 + kcmdFileSize + 5) >> 2;
// 	*((volatile U32 *)(ZPARAMADDR + 4*10)) = ATAG_CMDLINE;
	
	*((volatile U32 *)(ZPARAMADDR + 4*9)) = 2 + (kcmdFileSize >> 2) + 1;
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
//	int i;
	char *fn;

	res = f_opendir(&dir, path);
	if (res == FR_OK)
	{
//		i = strlen(path);
		for (;;)
		{
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) break;
			fn = fno.fname;
			if (*fn == '.') continue;
// 			if (fno.fattrib & AM_DIR)
// 			{
// 				TRACE_MSG(&path[i], "/%s", fn);
// 				res = scan_files(path);
// 				if (res != FR_OK) break;
// 				path[i] = 0;
// 			}
// 			else
// 			{
// 				DEBUG_MSG("%s/%s", path, fn);
// 			}
		}
	}

	return res;
}
