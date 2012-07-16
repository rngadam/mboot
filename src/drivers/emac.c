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
#include "emac.h"

extern U8 id_buf[8];

void Init_EMAC(void)
{
	PMC_PeriEn(25);
	rEMAC_NCR = 0;
	rEMAC_IDR = 0xFFFFFFFF;
	rEMAC_SA1B = (id_buf[2]<<24) + (id_buf[3]<<16) + (id_buf[6]<<8) + id_buf[7];
	rEMAC_SA1T = (id_buf[0]<<8) + id_buf[1];
	TRACE_MSG("EMAC Initialized with System ID.");
}
