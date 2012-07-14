/* ----------------------------------------------------------------------------
 *		 
 *        mboot for Lophilo embedded system
 *		
 * ----------------------------------------------------------------------------
 */

#include "types.h"
#include "trace.h"
#include "option.h"

#include "mboot.h"

#include "sfr.h"
#include "fpga.h"

extern U32 gridFileSize;

static U8 SFR_RD8(U32 ioaddr)
{
	return *((volatile U8 *)(ioaddr));
}

void Init_FPGA(void)
{
	rPIOA_PER = (1 << 27);
	rPIOA_OER = (1 << 27);
	GRID_RESET();

	rPIOB_PER = (1 << 18) + (1 << 17) + (1 << 16) + (1 << 15) + (1 << 14);
	rPIOB_OER = (1 << 17) + (1 << 16) + (1 << 15);
	rPIOB_ODR = (1 << 18) + (1 << 14);
	
	FPGA_CONF_N();
}

void FPGA_Config(void)
{
	U32 i;
	U8 buf, cnt;

	FPGA_CONF_P();

	while(!FPGA_STAT);
	
	TRACE_MSG_NONR("Start config FPGA [");
	
	for(i = 0; i <= gridFileSize; i++)
	{
		buf = SFR_RD8(GRIDADDR + i);
		
		for(cnt = 0; cnt < 8; cnt++)
		{
			if(((buf>>(cnt))&(0x1))==0x1)
			{
				FPGA_DATA_P();
			}
			else
			{
				FPGA_DATA_N();
			}	
			FPGA_DCLK_P();
			FPGA_DCLK_N();
		}
		
		if(FPGA_DONE)
		{
			PRINT_F("]\n\r");
			break;
		}
		
		if(i % 12000 == 0) PRINT_F(".");
	}
	
	if(!FPGA_DONE) TRACE_ERR("FPGA configuration failed.");
}
