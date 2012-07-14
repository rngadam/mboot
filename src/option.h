/*
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */

#ifndef __OPTION_H__
#define __OPTION_H__

#include "types.h"

#define MBOOT_VER 			"0.3-tabby"

#define DEBUG_ENABLE		0

#define BOARD_MAINOSC		12000000
#define BOARD_MCK				133333333
#define BOARD_VER				0xEA680002

#define ZRELADDR				0x70008000
#define ZPARAMADDR			0x70000100

#define LINUX_MACH_ID		1830

#define GRIDADDR				0x71000000
#define GRIDMAXSIZE			0x1000000

#endif /* __OPTION_H__ */
