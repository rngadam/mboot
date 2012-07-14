/* ----------------------------------------------------------------------------
 *		 
 *        mboot for Lophilo embedded system
 *		
 * ----------------------------------------------------------------------------
 */

#include "types.h"
#include "trace.h"
#include "option.h"

#include "sfr.h"
#include "system.h"
#include "leds.h"

void Init_LEDS(void)
{
	PMC_PeriEn(19);
	
	rPIOD_PDR = 0x7000001;
	rPIOD_BSR = 0x7000001;
	
	rPWM_MR		= (0x0 << 28) + (0xA << 24) + (0x82 << 16) + (0x0 << 12) + (0x1 << 8) + (0x1 << 0);
	rPWM_ENA	= 0xF; // Enable all PWM channels
	rPWM_IDR	= 0xF; // Disable PWM as interrupt source
	
	rPWM_CMR(0)		= 0xC;
	rPWM_CDTY(0)	= 0x200;
	rPWM_CPRD(0)	= 0x400;
	rPWM_CUPD(0)	= 0x0;

	rPWM_CMR(1)		= 0xC;
	rPWM_CDTY(1)	= 0x300;
	rPWM_CPRD(1)	= 0x400;
	rPWM_CUPD(1)	= 0x0;
	
	rPWM_CMR(2)		= 0xC;
	rPWM_CDTY(2)	= 0x200;
	rPWM_CPRD(2)	= 0x400;
	rPWM_CUPD(2)	= 0x0;
	
	rPWM_CMR(3)		= 0xC;
	rPWM_CDTY(3)	= 0x300;
	rPWM_CPRD(3)	= 0x400;
	rPWM_CUPD(3)	= 0x0;	
}
