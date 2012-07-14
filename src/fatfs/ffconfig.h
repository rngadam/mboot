/*
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */

#ifndef __FFCONFIG_H__
#define __FFCONFIG_H__

#include "integer.h"

#define DRV_MMC          0
#define DRV_NAND         1
#define DRV_SDRAM        2
#define DRV_ATA          3
#define DRV_USB          4

#define SECTOR_SIZE_DEFAULT		512
#define SECTOR_SIZE_SDRAM			512
#define SECTOR_SIZE_SDCARD		512

#endif /* __FFCONFIG_H__ */
