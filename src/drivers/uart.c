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
#include "uart.h"

void Init_UART(void)
{
	rPIOB_PDR = 0x3000;
	rPIOB_ASR = 0x3000;
	//  Initialize the DBGU
	rDBGU_CR = (0x1 << 7) + (0x1 << 5) + (0x1 << 3) + (0x1 << 2);
	rDBGU_IDR = 0xFFFFFFFF;
	rDBGU_BRGR = 0x48; // (5 << 16) + 144; (0x48)
	rDBGU_MR = (0x0 << 14) + (0x4 << 9);
	rDBGU_PTCR = (0x1 << 1) | (0x1 << 9);
	rDBGU_CR = (0x1 << 6) + (0x1 << 4);
}

void DBGU_PutByte(S8 c)
{
    // Wait for the transmitter to be ready  
    while ((rDBGU_SR & DBGU_TXEMPTY) == 0);
    // Send character    
    rDBGU_THR = c;
    // Wait for the transfer to complete    
    while ((rDBGU_SR & DBGU_TXEMPTY) == 0);  
}

S8 DBGU_GetByte(void)
{
    while (!(rDBGU_SR & DBGU_RXRDY));
    return rDBGU_RHR;
}

S8 DBGU_GetKey(void)
{
	if(rDBGU_SR & DBGU_RXRDY)
		return rDBGU_RHR;
	else
		return 0;
}
