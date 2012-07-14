/*
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */

#ifndef __FPGA_H__
#define __FPGA_H__

#define FPGA_CONF_N()		{rPIOB_CODR = (1 << 16);}
#define FPGA_CONF_P()		{rPIOB_SODR = (1 << 16);}

#define FPGA_DCLK_N()		{rPIOB_CODR = (1 << 17);}
#define FPGA_DCLK_P()		{rPIOB_SODR = (1 << 17);}

#define FPGA_DATA_N()		{rPIOB_CODR = (1 << 15);}
#define FPGA_DATA_P()		{rPIOB_SODR = (1 << 15);}

#define FPGA_DONE				((rPIOB_PDSR >> 14)&(0x1))
#define FPGA_STAT				((rPIOB_PDSR >> 18)&(0x1))

#define GRID_RESET()		{rPIOA_CODR = (1 << 27);}
#define GRID_UNRESET()	{rPIOA_SODR = (1 << 27);}

void Init_FPGA(void);
void FPGA_Config(void);

#endif /* __FPGA_H__ */
