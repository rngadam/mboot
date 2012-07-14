/*
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */

#ifndef __SDMMC_H__
#define __SDMMC_H__

#include "types.h"
#include "mci_hs.h"

//------------------------------------------------------------------------------
//         Constants
//------------------------------------------------------------------------------

#define SD_ERROR_DRIVER						1		/// There was an error with the SD driver.
#define SD_ERROR_NORESPONSE				2		/// The SD card did not answer the command.
#define SD_ERROR_NOT_INITIALIZED	3		/// The SD card did not answer the command.
#define SD_ERROR_BUSY							4		/// The SD card is busy
#define SD_ERROR_NOT_SUPPORT			5		/// The operation is not supported

/// Card types
#define CARD_TYPE_bmHC						(1 << 0)		/// Bit for High-Capacity(Density)
#define CARD_TYPE_bmSDMMC					(0x3 << 1)	/// Bit mask for SD/MMC
#define CARD_TYPE_bmSD						(0x1 << 1)	/// Bit for SD
#define CARD_TYPE_bmMMC						(0x2 << 1)	/// Bit for MMC
#define CARD_TYPE_bmSDIO					(1 << 3)		/// Bit for SDIO

#define CARD_UNKNOWN		(0)																/// Card can not identified
#define CARD_SD         (CARD_TYPE_bmSD)									/// SD Card (0x2)
#define CARD_SDHC       (CARD_TYPE_bmSD|CARD_TYPE_bmHC)		/// SD High Capacity Card (0x3)
#define CARD_MMC        (CARD_TYPE_bmMMC)									/// MMC Card (0x4)
#define CARD_MMCHD      (CARD_TYPE_bmMMC|CARD_TYPE_bmHC)	/// MMC High-Density Card (0x5)
#define CARD_SDIO       (CARD_TYPE_bmSDIO)								/// SDIO only card (0x8)
#define CARD_SDCOMBO    (CARD_TYPE_bmSDIO|CARD_SD)				/// SDIO Combo, with SD embedded (0xA)
#define CARD_SDHCCOMBO  (CARD_TYPE_bmSDIO|CARD_SDHC)			/// SDIO Combo, with SDHC embedded (0xB)

#define SD_BLOCK_SIZE					512		/// SD card block size in bytes.
#define SD_BLOCK_SIZE_BIT			9			/// SD card block size binary shift value

//------------------------------------------------------------------------------
//         Macros
//------------------------------------------------------------------------------

// CID register access macros (128 bits, 4 * 32 bits).
#define SD_CID(bitfield, bits)		((sdDrv.cid[3-(bitfield)/32] >> ((bitfield)%32)) & ((1 << (bits)) - 1))
#define SD_CID_MID()								SD_CID(120, 8)      ///< Manufacturer ID  
#define SD_CID_BGA()								SD_CID(112, 2)      ///< Card/BGA(eMMC)

// CSD register access macros (128 bits, 4 * 32 bits).
#define SD_CSD(bitfield, bits)		(((sdDrv.csd)[3-(bitfield)/32] >> ((bitfield)%32)) & ((1 << (bits)) - 1))
#define SD_CSD_STRUCTURE()					SD_CSD(126, 2) ///< CSD structure 00b  Version 1.0 01b version 2.0 High Cap
#define SD_CSD_TRAN_SPEED()				SD_CSD(96, 8) ///< Max. data transfer rate
#define SD_CSD_READ_BL_LEN()				SD_CSD(80, 4) ///< Max. read data block length
#define SD_CSD_C_SIZE()						((SD_CSD(72, 2) << 10) + (SD_CSD(64, 8) << 2)  + SD_CSD(62, 2)) ///< Device size
#define SD_CSD_C_SIZE_HC()					((SD_CSD(64,  6) << 16) + (SD_CSD(56,  8) << 8)  + SD_CSD(48,  8)) ///< Device size v2.0 High Capacity
#define SD_CSD_C_SIZE_MULT()				SD_CSD(47,  3) ///< Device size multiplier
#define SD_CSD_MULT()							(1 << (SD_CSD_C_SIZE_MULT() + 2))
#define SD_CSD_BLOCKNR()						((SD_CSD_C_SIZE() + 1) * SD_CSD_MULT())
#define SD_CSD_BLOCKNR_HC()				((SD_CSD_C_SIZE_HC() + 1) * 1024)
#define SD_CSD_BLOCK_LEN()					(1 << SD_CSD_READ_BL_LEN())
#define SD_CSD_TOTAL_SIZE()				(SD_CSD_BLOCKNR() * SD_CSD_BLOCK_LEN())
#define SD_CSD_TOTAL_SIZE_HC()			((SD_CSD_C_SIZE_HC() + 1) * 512* 1024)

// SCR register access macros (64 bits, 2 * 32 bits, 8 * 8 bits).
#define SD_EXT_OFFSET_SD_SCR					0   // DW
// #define SD_SCR(bitfield, bits)				(((char*)sdDrv.extData)[7-((bitfield)/8)] >> ((bitfield)%8) & ((1 << (bits)) - 1))
// #define SD_SCR_SD_SPEC()							SD_SCR(56, 4)
// #define SD_SCR_SD_SPEC_1_0_01					0
// #define SD_SCR_SD_SPEC_1_10						1
// #define SD_SCR_SD_SPEC_2_00						2

// SD Status access macros (512 bits, 16 * 32 bits, 64 * 8 bits).
#define SD_EXT_OFFSET_SD_STAT					2   // DW

// Bus width, 00: default, 10:4-bit
#define SD_STAT_DATA_BUS_WIDTH_1BIT		0x0
#define SD_STAT_DATA_BUS_WIDTH_4BIT		0x2

// // SD Switch Status access macros (512 bits, 16 * 32 bits, 64 * 8 bits).
#define SD_SW_STAT(p, bitfield, bits)					(((char*)(p))[63 - ((bitfield)/8)] >> ((bitfield)%8) & ((1 << (bits)) - 1))
#define SD_SW_STAT_FUN_GRP1_RC(p)							SD_SW_STAT(p, 376, 4)
#define SD_SW_STAT_FUN_GRP_RC_ERROR						0xF
#define SD_SW_STAT_FUN_GRP1_BUSY(p)						( SD_SW_STAT(p, 272, 8) + (SD_SW_STAT(p, 280, 8) << 8) )
#define SD_SW_STAT_FUN_GRP_FUN_BUSY(funNdx)		(1 << (funNdx))

// EXT_CSD register definition.
// #define MMC_EXTCSD(pSd)								((U8*)((pSd)->extData))
// #define SD_EXTCSD_HS_TIMING(pSd)			(MMC_EXTCSD(pSd)[185]) // High-speed interface timing




//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

typedef MciCallback SdCallback;

//------------------------------------------------------------------------------
/// Sdcard driver structure. It holds the current command being processed and
/// the SD card address.
//------------------------------------------------------------------------------
typedef struct _SdCard {
	U32 cid[4];     			/// Card IDentification (CID register)  
	U32 csd[4]; 					/// Card-specific data (CSD register)
	U32 extData[128];			/// SD SCR(64 bit), Status(512 bit) or MMC EXT_CSD(512 bytes) register
	U32 preBlock;					/// Previous access block number.
	U32 totalSize;				/// Card total size
	U32 blockNr;					/// Card block number
	U32 optCmdBitMap;			/// Card option command support list
	U32 transSpeed;				/// Card CSD TRANS_SPEED
	U16 cardAddress;			/// SD card current address.
	U8 cardType;					/// Card type
	U8 mode;							/// Card access mode
	U8 state;							/// State after sd command complete
} SdCard;

typedef struct _SdCmd6Arg
{
	U32 accessMode:4,  /// [ 3: 0] function group 1, access mode
	command:4,     /// [ 7: 4] function group 2, command system
	reserveFG3:4,  /// [11: 8] function group 3, 0xF or 0x0
	reserveFG4:4,  /// [15:12] function group 4, 0xF or 0x0
	reserveFG5:4,  /// [19:16] function group 5, 0xF or 0x0
	reserveFG6:4,  /// [23:20] function group 6, 0xF or 0x0
	reserved:7,    /// [30:24] reserved 0
	mode:1;        /// [31   ] Mode, 0: Check, 1: Switch
} SdCmd6Arg;

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

extern U8 SD_Init(void);
extern U8 SD_Read(U32 address, void *pData, U16 length, SdCallback pCallback, void *pArgs);
extern U8 SD_Write(U32 address, void *pData, U16 length, SdCallback pCallback, void *pArgs);
extern U8 SD_ReadBlock(U32 address, U16 nbBlocks, U8 *pData);
extern U8 SD_WriteBlock(U32 address, U16 nbBlocks, const U8 *pData);
extern U8 SD_Stop(void);
extern U8 SD_HighSpeedMode(U8 cardHsMode);
extern U32 MMC_GetTotalSizeKB(void);

#endif /* __SDMMC_H__ */
