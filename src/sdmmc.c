/* ----------------------------------------------------------------------------
 *		 
 *        mboot for Lophilo embedded system
 *		
 * ----------------------------------------------------------------------------
 */

#include <string.h>

#include "types.h"
#include "trace.h"
#include "mboot.h"

#include "system.h"
#include "mci_hs.h"
#include "sdmmc.h"

//------------------------------------------------------------------------------
//         Global variables
//------------------------------------------------------------------------------

//U8 gSdmmcAutoHsEnable = 1;

//------------------------------------------------------------------------------
//         Local constants
//------------------------------------------------------------------------------

// Timeout wait loop
#define TO_LOOP						0x10000

// SD card operation states
#define SD_STATE_IDLE			0
#define SD_STATE_INIT			1
#define SD_STATE_READY		2
#define SD_STATE_READ			0x10
#define SD_STATE_RD_RDY		0x11
#define SD_STATE_RD_BSY		0x12
#define SD_STATE_WRITE		0x20
#define SD_STATE_WR_RDY		0x21
#define SD_STATE_WR_BSY		0x22
#define SD_STATE_BOOT			0x30

#define SD_ADDRESS(address) 	((sdDrv.totalSize == 0xFFFFFFFF) ? (address):((address) << SD_BLOCK_SIZE_BIT))

//-----------------------------------------------------------------------------
/// MMC/SD in SPI mode reports R1 status always, and R2 for SEND_STATUS
/// R1 is the low order byte; R2 is the next highest byte, when present.
//-----------------------------------------------------------------------------
#define R1_SPI_IDLE             (1 << 0)
#define R1_SPI_ERASE_RESET      (1 << 1)
#define R1_SPI_ILLEGAL_COMMAND  (1 << 2)
#define R1_SPI_COM_CRC          (1 << 3)
#define R1_SPI_ERASE_SEQ        (1 << 4)
#define R1_SPI_ADDRESS          (1 << 5)
#define R1_SPI_PARAMETER        (1 << 6)
// R1 bit 7 is always zero
#define R2_SPI_CARD_LOCKED      (1 << 0)
#define R2_SPI_WP_ERASE_SKIP    (1 << 1)
#define R2_SPI_LOCK_UNLOCK_FAIL R2_SPI_WP_ERASE_SKIP
#define R2_SPI_ERROR            (1 << 2)
#define R2_SPI_CC_ERROR         (1 << 3)
#define R2_SPI_CARD_ECC_ERROR   (1 << 4)
#define R2_SPI_WP_VIOLATION     (1 << 5)
#define R2_SPI_ERASE_PARAM      (1 << 6)
#define R2_SPI_OUT_OF_RANGE     (1 << 7)
#define R2_SPI_CSD_OVERWRITE    R2_SPI_OUT_OF_RANGE

// Status register constants
#define STATUS_APP_CMD          (1UL << 5)
#define STATUS_SWITCH_ERROR     (1UL << 7)
#define STATUS_READY_FOR_DATA   (1UL << 8)
#define STATUS_IDLE             (0UL << 9)
#define STATUS_READY            (1UL << 9)
#define STATUS_IDENT            (2UL << 9)
#define STATUS_STBY             (3UL << 9)
#define STATUS_TRAN             (4UL << 9)
#define STATUS_DATA             (5UL << 9)
#define STATUS_RCV              (6UL << 9)
#define STATUS_PRG              (7UL << 9)
#define STATUS_DIS              (8UL << 9)
#define STATUS_STATE						(0xFUL << 9)
#define STATUS_ERASE_RESET       (1UL << 13)
#define STATUS_WP_ERASE_SKIP     (1UL << 15)
#define STATUS_CIDCSD_OVERWRITE  (1UL << 16)
#define STATUS_OVERRUN           (1UL << 17)
#define STATUS_UNERRUN           (1UL << 18)
#define STATUS_ERROR             (1UL << 19)
#define STATUS_CC_ERROR          (1UL << 20)
#define STATUS_CARD_ECC_FAILED   (1UL << 21)
#define STATUS_ILLEGAL_COMMAND   (1UL << 22)
#define STATUS_COM_CRC_ERROR     (1UL << 23)
#define STATUS_UN_LOCK_FAILED    (1UL << 24)
#define STATUS_CARD_IS_LOCKED    (1UL << 25)
#define STATUS_WP_VIOLATION      (1UL << 26)
#define STATUS_ERASE_PARAM       (1UL << 27)
#define STATUS_ERASE_SEQ_ERROR   (1UL << 28)
#define STATUS_BLOCK_LEN_ERROR   (1UL << 29)
#define STATUS_ADDRESS_MISALIGN  (1UL << 30)
#define STATUS_ADDR_OUT_OR_RANGE (1UL << 31)

#define STATUS_STOP							(STATUS_CARD_IS_LOCKED | STATUS_COM_CRC_ERROR | STATUS_ILLEGAL_COMMAND | STATUS_CC_ERROR | STATUS_ERROR | STATUS_STATE | STATUS_READY_FOR_DATA)
#define STATUS_WRITE						(STATUS_ADDR_OUT_OR_RANGE | STATUS_ADDRESS_MISALIGN | STATUS_BLOCK_LEN_ERROR | STATUS_WP_VIOLATION | STATUS_CARD_IS_LOCKED | STATUS_COM_CRC_ERROR | STATUS_ILLEGAL_COMMAND | STATUS_CC_ERROR | STATUS_ERROR | STATUS_ERASE_RESET | STATUS_STATE | STATUS_READY_FOR_DATA)
#define STATUS_READ							(STATUS_ADDR_OUT_OR_RANGE | STATUS_ADDRESS_MISALIGN | STATUS_BLOCK_LEN_ERROR | STATUS_CARD_IS_LOCKED | STATUS_COM_CRC_ERROR | STATUS_ILLEGAL_COMMAND | STATUS_CARD_ECC_FAILED | STATUS_CC_ERROR | STATUS_ERROR | STATUS_ERASE_RESET | STATUS_STATE | STATUS_READY_FOR_DATA)
#define STATUS_SD_SWITCH				(STATUS_ADDR_OUT_OR_RANGE | STATUS_CARD_IS_LOCKED | STATUS_COM_CRC_ERROR | STATUS_ILLEGAL_COMMAND | STATUS_CARD_ECC_FAILED | STATUS_CC_ERROR | STATUS_ERROR | STATUS_UNERRUN | STATUS_OVERRUN | STATUS_STATE)

//-----------------------------------------------------------------------------
/// OCR Register
//-----------------------------------------------------------------------------
#define AT91C_VDD_16_17									(1UL << 4)
#define AT91C_VDD_17_18									(1UL << 5)
#define AT91C_VDD_18_19									(1UL << 6)
#define AT91C_VDD_19_20									(1UL << 7)
#define AT91C_VDD_20_21									(1UL << 8)
#define AT91C_VDD_21_22									(1UL << 9)
#define AT91C_VDD_22_23									(1UL << 10)
#define AT91C_VDD_23_24									(1UL << 11)
#define AT91C_VDD_24_25									(1UL << 12)
#define AT91C_VDD_25_26									(1UL << 13)
#define AT91C_VDD_26_27									(1UL << 14)
#define AT91C_VDD_27_28									(1UL << 15)
#define AT91C_VDD_28_29									(1UL << 16)
#define AT91C_VDD_29_30									(1UL << 17)
#define AT91C_VDD_30_31									(1UL << 18)
#define AT91C_VDD_31_32									(1UL << 19)
#define AT91C_VDD_32_33									(1UL << 20)
#define AT91C_VDD_33_34									(1UL << 21)
#define AT91C_VDD_34_35									(1UL << 22)
#define AT91C_VDD_35_36									(1UL << 23)
#define AT91C_SDIO_MP										(1UL << 27)
#define AT91C_SDIO_NF										(7UL << 28)
#define AT91C_MMC_OCR_BIT2930						(3UL << 29)
#define AT91C_CARD_POWER_UP_BUSY				(1UL << 31)

#define AT91C_MMC_HOST_VOLTAGE_RANGE		(AT91C_VDD_27_28 + AT91C_VDD_28_29 + AT91C_VDD_29_30 + AT91C_VDD_30_31 + AT91C_VDD_31_32 + AT91C_VDD_32_33)

#define AT91C_CCS    (1 << 30)

// MCI_CMD Register Value
#define AT91C_POWER_ON_INIT      (0 | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_INIT | AT91C_MCI_OPDCMD)

//-----------------------------------------------------------------------------
// Command Classes
//-----------------------------------------------------------------------------
//
// Class 0, 2, 4, 5, 7 and 8 are mandatory and shall be supported by
// all SD Memory Cards.
//
// Basic Commands (class 0)
//
// Cmd0 MCI + SPI
#define   AT91C_GO_IDLE_STATE_CMD				(0 | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_NONE)
// Cmd1 SPI
#define   AT91C_MMC_SEND_OP_COND_CMD		(1 | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_OPDCMD)
// Cmd2 MCI
#define   AT91C_ALL_SEND_CID_CMD				(2 | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_NONE | AT91C_MCI_OPDCMD | AT91C_MCI_RSPTYP_136 )
// Cmd3 MCI
#define   AT91C_SET_RELATIVE_ADDR_CMD		(3 | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_OPDCMD | AT91C_MCI_MAXLAT )
// Cmd4 MCI
#define AT91C_SET_DSR_CMD								(4 | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_NO | AT91C_MCI_MAXLAT)
// Cmd5 MCI
#define AT91C_IO_SEND_OP_COND_CMD				(5 | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_OPDCMD)
// Cmd6 SD/MMC
#define AT91C_MMC_SWITCH_CMD						(6 | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_R1B | AT91C_MCI_MAXLAT)

#define AT91C_SD_SWITCH_CMD							(6 | AT91C_MCI_TRCMD_START | AT91C_MCI_TRTYP_BLOCK | AT91C_MCI_TRDIR_READ | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_MAXLAT)
// cmd7 MCI
#define   AT91C_SEL_DESEL_CARD_CMD			(7 | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_MAXLAT)
#define   AT91C_SEL_CARD_CMD						(7 | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_MAXLAT)
#define   AT91C_DESEL_CARD_CMD					(7 | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_NO | AT91C_MCI_MAXLAT)
// Cmd8 MCI + SPI
#define   AT91C_SEND_IF_COND						(8  | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_MAXLAT)
// Cmd9 MCI + SPI
#define   AT91C_SEND_CSD_CMD						(9  | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_136 | AT91C_MCI_MAXLAT)
// Cmd10 MCI + SPI
#define   AT91C_SEND_CID_CMD						(10 | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_136 | AT91C_MCI_MAXLAT)
// Cmd12 MCI + SPI
#define   AT91C_STOP_TRANSMISSION_CMD		(12 | AT91C_MCI_TRCMD_STOP | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_R1B | AT91C_MCI_MAXLAT)
// Cmd13 MCI + SPI
#define   AT91C_SEND_STATUS_CMD					(13 | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_MAXLAT)
// Cmd15 MCI
#define AT91C_GO_INACTIVE_STATE_CMD			(15 | AT91C_MCI_RSPTYP_NO)
// Cmd58 SPI
#define   AT91C_READ_OCR_CMD						(58 | AT91C_MCI_RSPTYP_48 | AT91C_MCI_SPCMD_NONE | AT91C_MCI_MAXLAT)
// Cmd59 SPI
#define   AT91C_CRC_ON_OFF_CMD					(59 | AT91C_MCI_RSPTYP_48 | AT91C_MCI_SPCMD_NONE | AT91C_MCI_MAXLAT)

//*------------------------------------------------
//* Class 2 commands: Block oriented Read commands
//*------------------------------------------------

// Cmd8 for MMC
#define AT91C_SEND_EXT_CSD_CMD					(8 | AT91C_MCI_SPCMD_NONE | AT91C_MCI_OPDCMD_PUSHPULL | AT91C_MCI_RSPTYP_48 | AT91C_MCI_TRCMD_START | AT91C_MCI_TRTYP_BLOCK | AT91C_MCI_TRDIR | AT91C_MCI_MAXLAT)
// Cmd16
#define AT91C_SET_BLOCKLEN_CMD					(16 | AT91C_MCI_TRCMD_NO | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_MAXLAT)
// Cmd17
#define AT91C_READ_SINGLE_BLOCK_CMD     (17 | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_TRCMD_START | AT91C_MCI_TRTYP_BLOCK | AT91C_MCI_TRDIR | AT91C_MCI_MAXLAT)
// Cmd18
#define AT91C_READ_MULTIPLE_BLOCK_CMD   (18 | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_TRCMD_START | AT91C_MCI_TRTYP_MULTIPLE | AT91C_MCI_TRDIR | AT91C_MCI_MAXLAT)

//*------------------------------------------------
//* Class 4 commands: Block oriented write commands
//*------------------------------------------------
// Cmd24
#define AT91C_WRITE_BLOCK_CMD						(24 | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_TRCMD_START | (AT91C_MCI_TRTYP_BLOCK & ~(AT91C_MCI_TRDIR)) | AT91C_MCI_MAXLAT)
// Cmd25
#define AT91C_WRITE_MULTIPLE_BLOCK_CMD	(25 | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_TRCMD_START | (AT91C_MCI_TRTYP_MULTIPLE & ~(AT91C_MCI_TRDIR)) | AT91C_MCI_MAXLAT)
// Cmd27
#define AT91C_PROGRAM_CSD_CMD						(27 | AT91C_MCI_RSPTYP_48 )

//*-----------------------------------------------
// Class 8 commands: Application specific commands
//*-----------------------------------------------
// Cmd55
#define AT91C_APP_CMD										(55 | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_TRCMD_NO | AT91C_MCI_MAXLAT)
// ACMD6
#define AT91C_SD_SET_BUS_WIDTH_CMD			(6  | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_TRCMD_NO | AT91C_MCI_MAXLAT)
// ACMD13
#define AT91C_SD_STATUS_CMD							(13 | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_TRCMD_START | AT91C_MCI_TRTYP_BLOCK | AT91C_MCI_TRDIR_READ | AT91C_MCI_MAXLAT)
// ACMD41
#define AT91C_SD_APP_OP_COND_CMD				(41 | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_TRCMD_NO)
// ACMD51
#define AT91C_SD_SEND_SCR_CMD						(51 | AT91C_MCI_SPCMD_NONE | AT91C_MCI_RSPTYP_48 | AT91C_MCI_TRCMD_START | AT91C_MCI_TRDIR_READ | AT91C_MCI_TRTYP_BLOCK | AT91C_MCI_MAXLAT)

// Optional commands
#define SD_ACMD6_SUPPORT        (1UL << 0)
#define SD_ACMD13_SUPPORT       (1UL << 1)
#define SD_ACMD41_SUPPORT       (1UL << 2)
#define SD_ACMD51_SUPPORT       (1UL << 3)

#define SD_CMD16_SUPPORT        (1UL << 8)

//extern Mci mciDrv; /// MCI driver instance.
extern MciCmd sdCmd;
SdCard sdDrv; /// SDCard driver instance

//------------------------------------------------------------------------------
//         Local functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Get Trans Speed Value (Kbit/s)
/// param: tranSpeed The TRAN_SPEED value from SD(IO)/MMC enum information.
/// param: unitList  Transfer rate units (Kbit/s), 4 units listed.
/// param: nbUnits   Transfer rate units list size.
/// param: codeList  Time value codes list, 16 codes listed.
//------------------------------------------------------------------------------
static U32 MmcGetTranSpeed(U32 tranSpeed, const U32* unitList, U32 nbUnits, const U32* codeList)
{
	U32 unit, value;
	
	unit = tranSpeed & 0x7;
	if (unit < nbUnits) unit = unitList[unit];
	else return 0;
	value = (tranSpeed >> 3) & 0xF;
	if (value < 16) value = codeList[value];
	else return 0;
	return (unit * value);
}

//------------------------------------------------------------------------------
/// Get Trans Speed Value
/// param: pSd    
//------------------------------------------------------------------------------
void GetTransSpeedValue(void)
{
	// CSD register, TRANS_SPEED bit
	const U32 units[4] = {10, 100, 1000, 10000 };
	const U32 values_emmc[16] = {0, 10, 12, 13, 15, 20, 26, 30, 35, 40, 45, 52, 55, 60, 70, 80};
	const U32 values_sdmmc[16] = {0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80};

	sdDrv.transSpeed = MmcGetTranSpeed((sdDrv.csd[0] & 0xFF), units, 4, (sdDrv.cardType >= CARD_MMC && ((sdDrv.cid[0] >> 16 & (0x3)) == 1) ? values_emmc : values_sdmmc));

	if ((sdDrv.cardType >= CARD_MMC) && (((U8*)(sdDrv.extData))[185]))
	{
		sdDrv.transSpeed *= 2;
	}
	
	DEBUG_MSG("SD/MMC TRANS SPEED %d KBit/s", sdDrv.transSpeed);
	sdDrv.transSpeed *= 1000; 
}

//------------------------------------------------------------------------------
/// Reset the SdCmd
//------------------------------------------------------------------------------
static void ResetCommand(void)
{
	sdCmd.cmd       = 0;
	sdCmd.arg       = 0;
	sdCmd.pData     = 0;
	sdCmd.blockSize = 0;
	sdCmd.nbBlock   = 0;
	sdCmd.pResp     = 0;
	sdCmd.callback  = 0;
	sdCmd.pArg      = 0;
	sdCmd.resType   = 0;
	sdCmd.dataTran  = 0;
	sdCmd.tranType  = 0;
	sdCmd.isRead    = 0;
	sdCmd.status    = 0;
}

//------------------------------------------------------------------------------
/// Sends the current SD card driver command to the card.
/// Returns 0 if successful; Otherwise, returns the transfer status code or
/// SD_ERROR_DRIVER if there was a problem with the SD transfer.
/// param: pSd  Pointer to a SdCard driver instance.
//------------------------------------------------------------------------------
static U8 SendCommand(void)
{
	U8 sd_err;

	// Send command
	sd_err = MCI_SendCommand();
	if(sd_err)
	{
		TRACE_ERR("MCI SendCommand: Failed to send command (%d)", sd_err);
		return SD_ERROR_DRIVER;
	}
	// Wait for command to complete (if no callback defined)
	if(sdCmd.callback == 0)
	{
		while(!MCI_IsTxComplete());
	}
	return sdCmd.status;
}

//------------------------------------------------------------------------------
/// Initialization delay: The maximum of 1 msec, 74 clock cycles and supply ramp
/// up time.
/// Returns the command transfer result (see SendCommand).
/// param: pSd  Pointer to a SdCard driver instance.
//------------------------------------------------------------------------------
static U8 Pon(void)
{
	U32 response;
	U8 sd_err;
	
	
	//response = 0;
	ResetCommand();
	// Fill command information
	sdCmd.cmd = AT91C_POWER_ON_INIT;
	sdCmd.pResp = &response;
	
	// Send command
	sd_err = SendCommand();
	
	DEBUG_MSG("Pon(): 0x%x", response);
	return sd_err;
}

//------------------------------------------------------------------------------
/// Resets all cards to idle state
/// param: pSd  Pointer to a SdCard driver instance.
/// param: arg  Argument used.
/// return the command transfer result (see SendCommand).
//------------------------------------------------------------------------------
static U8 Cmd0(U32 arg)
{
	U32 response;
	U8 sd_err;

	// DEBUG_MSG("Cmd0()");
	ResetCommand();
	// Fill command information
	sdCmd.cmd = AT91C_GO_IDLE_STATE_CMD;
	sdCmd.arg = arg;
	sdCmd.pResp = &response;
	sd_err =  SendCommand(); // send command
	return sd_err;
}

//------------------------------------------------------------------------------
/// Asks any card to send the CID numbers
/// on the CMD line (any card that is
/// connected to the host will respond)
/// Returns the command transfer result (see SendCommand).
/// param: pSd  Pointer to a SD card driver instance.
/// param: pCid  Buffer for storing the CID numbers.
//------------------------------------------------------------------------------
static U8 Cmd2(void)
{
	// DEBUG_MSG("Cmd2()");
	ResetCommand();
	// Fill the command information
	sdCmd.cmd = AT91C_ALL_SEND_CID_CMD;
	sdCmd.resType = 2;
	sdCmd.pResp = sdDrv.cid;
	return SendCommand(); // Send the command
}

//------------------------------------------------------------------------------
/// Ask the card to publish a new relative address (RCA)
/// Returns the command transfer result (see SendCommand).
/// param: pSd  Pointer to a SD card driver instance.
//------------------------------------------------------------------------------
static U8 Cmd3(void)
{
	U32 cardAddress;
	U8 sd_err;

	// DEBUG_MSG("Cmd3()");
	ResetCommand();
	// Fill command information
	sdCmd.cmd = AT91C_SET_RELATIVE_ADDR_CMD;

	// Assign relative address to MMC card
	if((sdDrv.cardType == CARD_MMC) || (sdDrv.cardType == CARD_MMCHD))
	{
		sdCmd.arg = (0x1 << 16);
	}
	sdCmd.resType = 1;
	sdCmd.pResp = &cardAddress;
	sd_err = SendCommand(); // Send command
	if (sd_err)
	{
		return sd_err;
	}
	// Save card address in driver
	if((sdDrv.cardType == CARD_SD) || (sdDrv.cardType == CARD_SDHC))
	{
		sdDrv.cardAddress = (cardAddress >> 16) & 0xFFFF;
	}
	else if(sdDrv.cardType >= CARD_SDIO)
	{
		sdDrv.cardAddress = (cardAddress >> 16) & 0xFFFF;
	}
	else
	{
		// Default MMC RCA is 0x0001
		sdDrv.cardAddress = 1;
	}
	return 0;
}

//------------------------------------------------------------------------------
/// Command toggles a card between the
/// stand-by and transfer states or between
/// the programming and disconnect states.
/// Returns the command transfer result (see SendCommand).
/// param: pSd  Pointer to a SD card driver instance.
/// param: address  Relative Card Address (0 deselects all).
//------------------------------------------------------------------------------
static U8 Cmd7(U32 address)
{
	// DEBUG_MSG("Cmd7()");
	ResetCommand();
	// Fill command information
	sdCmd.cmd = AT91C_SEL_DESEL_CARD_CMD;
	sdCmd.arg = address << 16;
	sdCmd.resType = 1;
	return SendCommand(); // Send command
}

//------------------------------------------------------------------------------
/// Switches the mode of operation of the selected card (SD/MMC) or
/// modifies the EXT_CSD registers (for MMC only).
/// CMD6 is valid under the "trans" state.
/// return The command transfer result (see SendCommand).
/// param:  pSd         Pointer to a SD/MMC card driver instance.
/// param:  pSwitchArg  Pointer to a MmcCmd6Arg instance.
/// param:  pStatus     Pointer to where the 512bit status is returned.
/// param:  pResp       Pointer to where the response is returned.
//------------------------------------------------------------------------------
static U8 Cmd6(const void *pSwitchArg, U32 *pStatus, U32 *pResp)
{
	U32 response;
	U8 sd_err;
	SdCmd6Arg *pSdSwitch;

	// DEBUG_MSG("CMD6()");
	ResetCommand();
	if(sdDrv.cardType >= CARD_SD)
	{
		pSdSwitch = (SdCmd6Arg*)pSwitchArg;
		// R1 response & 512 bits of status on DAT
		sdCmd.cmd = AT91C_SD_SWITCH_CMD;
		sdCmd.resType = 1;
		sdCmd.arg = (pSdSwitch->mode << 31) | (pSdSwitch->reserved << 30) | (pSdSwitch->reserveFG6 << 20) | (pSdSwitch->reserveFG5 << 16) | (pSdSwitch->reserveFG4 << 12) | (pSdSwitch->reserveFG3 <<  8) | (pSdSwitch->command << 4) | (pSdSwitch->accessMode << 0);
		if(pStatus)
		{
			sdCmd.blockSize = 512 / 8;
			sdCmd.nbBlock = 1;
			sdCmd.pData = (U8*)pStatus;
			sdCmd.dataTran = 1;
			sdCmd.isRead = 1;
			sdCmd.tranType = MCI_NEW_TRANSFER;
		}
	}
	sdCmd.pResp = &response;
	DEBUG_MSG("CMD6(%d) arg 0x%X", sdDrv.cardType, sdCmd.arg);
	sd_err = SendCommand();
	if (sd_err) return sd_err;
	else if (pResp) *pResp = response;
	return 0;
}

//------------------------------------------------------------------------------
/// SD:  Sends SD Memory Card interface condition, which includes host supply
///      voltage information and asks the card whether card supports voltage.
///      Should be performed at initialization time to detect the card type.
/// MMC: SEND_EXT_CSD, to get EXT_CSD register as a block of data.
///      Valid under "trans" state.
/// param: pSd   Pointer to a SD card driver instance.
/// param: sdCmd For SD Memory Card interface condition 
/// param: arg   Expected supply voltage(SD) or 512 byte buffer pointer (MMC).
/// return 0 if successful;
///         otherwise returns SD_ERROR_NORESPONSE if the card did not answer
///         the command, or SD_ERROR_DRIVER.
//------------------------------------------------------------------------------
static U8 Cmd8(U8 sdCommand, void* arg)
{
	U32 response;
	U8 sd_err;
	U8 supplyVoltage = (U8)((U32)arg);

	// DEBUG_MSG("Cmd8()");
	ResetCommand();

	if(sdCommand)
	{
		// Fill command information
		sdCmd.cmd = AT91C_SEND_IF_COND;
		sdCmd.arg = (supplyVoltage << 8) | (0xAA);
		sdCmd.resType = 7;
		DEBUG_MSG("supplyVoltage: 0x%x", supplyVoltage);
	}
	else
	{
		sdCmd.cmd = AT91C_SEND_EXT_CSD_CMD;
		sdCmd.resType = 1;
		sdCmd.blockSize = SD_BLOCK_SIZE;
		sdCmd.nbBlock = 512 / SD_BLOCK_SIZE;
		sdCmd.pData = arg;
		sdCmd.dataTran = 1;
		sdCmd.isRead = 1;
		sdCmd.tranType = MCI_NEW_TRANSFER;
	}
	sdCmd.pResp = &response;
	// Send command
	sd_err = SendCommand();
	
	if(sdCommand)
	{
		// Check result
		if(sd_err == MCI_STATUS_NORESPONSE)
		{
			return SD_ERROR_NORESPONSE;
		}
		// SD_R7
		// Bit 0 - 7: check pattern (echo-back)
		// Bit 8 -11: voltage accepted
		else if(!sd_err && ((response & 0x00000FFF) == ((supplyVoltage << 8) | 0xAA)))
		{
			return 0;
		}
		else
		{
			return SD_ERROR_DRIVER;
		}
	}
	return sd_err;
}

//------------------------------------------------------------------------------
/// Addressed card sends its card-specific
/// data (CSD) on the CMD line.
/// Returns the command transfer result (see SendCommand).
/// param: pSd  Pointer to a SD card driver instance.
//------------------------------------------------------------------------------
static U8 Cmd9(void)
{
	U8 sd_err;

	// DEBUG_MSG("Cmd9()");
	ResetCommand();
	// Fill command information
	sdCmd.cmd = AT91C_SEND_CSD_CMD;
	sdCmd.arg = sdDrv.cardAddress << 16;
	sdCmd.resType = 2;
	sdCmd.pResp = sdDrv.csd;
	sd_err = SendCommand(); // Send command
	return sd_err;
}

//------------------------------------------------------------------------------
/// Forces the card to stop transmission
/// param: pSd      Pointer to a SD card driver instance.
/// param: stopRead Stop reading stream/writing stream.
/// param: pStatus  Pointer to a status variable.
//------------------------------------------------------------------------------
static U8 Cmd12(U8 stopRead, U32 *pStatus)
{
	U8 sd_err;
	U32 response;

	// DEBUG_MSG("Cmd12()");
	ResetCommand();
	// Fill command information
	sdCmd.cmd = AT91C_STOP_TRANSMISSION_CMD;
	sdCmd.isRead = stopRead;
	sdCmd.tranType = MCI_STOP_TRANSFER;
	sdCmd.resType = 1;
	sdCmd.pResp = &response;
	sd_err = SendCommand(); // Send command
	if(pStatus) *pStatus = response;
	return sd_err;
}

//------------------------------------------------------------------------------
/// Addressed card sends its status register.
/// Returns the command transfer result (see SendCommand).
/// param: pSd  Pointer to a SD card driver instance.
/// param: pStatus  Pointer to a status variable.
//------------------------------------------------------------------------------
static U8 Cmd13(U32 *pStatus)
{
	U8 sd_err;

	// DEBUG_MSG("Cmd13()");
	ResetCommand();
	// Fill command information
	sdCmd.cmd = AT91C_SEND_STATUS_CMD;
	sdCmd.arg = sdDrv.cardAddress << 16;
	sdCmd.resType = 1;
	sdCmd.pResp = pStatus;
	sd_err = SendCommand(); // Send command

	return sd_err;
}

//------------------------------------------------------------------------------
/// In the case of a Standard Capacity SD Memory Card, this command sets the
/// block length (in bytes) for all following block commands
/// (read, write, lock).
/// Default block length is fixed to 512 Bytes.
/// Set length is valid for memory access commands only if partial block read
/// operation are allowed in CSD.
/// In the case of a High Capacity SD Memory Card, block length set by CMD16
/// command does not affect the memory read and write commands. Always 512
/// Bytes fixed block length is used. This command is effective for LOCK_UNLOCK
/// command. In both cases, if block length is set larger than 512Bytes, the
/// card sets the BLOCK_LEN_ERROR bit.
/// param: pSd  Pointer to a SD card driver instance.
/// param: blockLength  Block length in bytes.
//------------------------------------------------------------------------------
static U8 Cmd16(U16 blockLength)
{
	U8 sd_err;
	U32 response;

	// DEBUG_MSG("Cmd16()");
	ResetCommand();
	// Fill command information
	sdCmd.cmd = AT91C_SET_BLOCKLEN_CMD;
	sdCmd.arg = blockLength;
	sdCmd.resType = 1;
	sdCmd.pResp = &response;
	sd_err = SendCommand(); // Send command

	return sd_err;
}

//------------------------------------------------------------------------------
/// Continously transfers datablocks from card to host until interrupted by a
/// STOP_TRANSMISSION command.
/// param: pSd        Pointer to a SD card driver instance.
/// param: blockSize  Block size (shall be set to 512 in case of high capacity).
/// param: pData      Pointer to the DW aligned buffer to be filled.
/// param: address    SD card address.
//------------------------------------------------------------------------------
static U8 Cmd18(U16 nbBlock, U8 *pData, U32 address, U32 *pStatus)
{
	U8 sd_err;
	U32 response;

	//DEBUG_MSG("Cmd18()");
	ResetCommand();
	// Fill command information
	sdCmd.cmd = AT91C_READ_MULTIPLE_BLOCK_CMD;
	sdCmd.arg = address;
	sdCmd.resType = 1;
	sdCmd.pResp = &response;
	sdCmd.blockSize = SD_BLOCK_SIZE;
	sdCmd.nbBlock = nbBlock;
	sdCmd.pData = pData;
	sdCmd.dataTran = 1;
	sdCmd.isRead = 1;
	sdCmd.tranType = MCI_NEW_TRANSFER;
	sd_err = SendCommand(); // Send command
	if (pStatus) *pStatus = response;

	return sd_err;
}

//------------------------------------------------------------------------------
/// Write block command
/// param: pSd  Pointer to a SD card driver instance.
/// param: blockSize  Block size (shall be set to 512 in case of high capacity).
/// param: pData  Pointer to the DW aligned buffer to be filled.
/// param: address  SD card address.
//------------------------------------------------------------------------------
static U8 Cmd25(U16 nbBlock, U8 *pData, U32 address, U32 *pStatus)
{
	U8 sd_err;
	U32 response;

	//DEBUG_MSG("Cmd25()");
	ResetCommand();
	// Fill command information
	sdCmd.cmd = AT91C_WRITE_MULTIPLE_BLOCK_CMD;
	sdCmd.arg = address;
	sdCmd.resType = 1;
	sdCmd.pResp = &response;
	sdCmd.blockSize = SD_BLOCK_SIZE;
	sdCmd.nbBlock = nbBlock;
	sdCmd.pData = pData;
	sdCmd.dataTran = 1;
	sdCmd.tranType = MCI_NEW_TRANSFER;

	// Send command
	sd_err = SendCommand();
	if(pStatus) *pStatus = response;

	return sd_err;
}



//------------------------------------------------------------------------------
/// Initialization delay: The maximum of 1 msec, 74 clock cycles and supply
/// ramp up time.
/// Returns the command transfer result (see SendCommand).
/// param: pSd  Pointer to a SD card driver instance.
//------------------------------------------------------------------------------
static U8 Cmd55(void)
{
	U8 sd_err;
	U32 response;

	// DEBUG_MSG("Cmd55()");
	ResetCommand();
	// Fill command information
	sdCmd.cmd = AT91C_APP_CMD;
	sdCmd.arg = (sdDrv.cardAddress << 16);
	sdCmd.resType = 1;
	sdCmd.pResp = &response;
	// Send command
	sd_err = SendCommand();
	return sd_err;
}

//------------------------------------------------------------------------------
/// Defines the data bus width (00=1bit or 10=4 bits bus) to be used for data
/// transfer.
/// The allowed data bus widths are given in SCR register.
/// param: pSd  Pointer to a SD card driver instance.
/// param: busWidth  Bus width in bits.
/// return the command transfer result (see SendCommand).
//------------------------------------------------------------------------------
static U8 Acmd6(U8 busWidth)
{
	U8 sd_err;
	U32 response;

	//DEBUG_MSG("Acmd6()");
	sd_err = Cmd55();
	if (sd_err)
	{
		TRACE_ERR("Acmd6.Cmd55: %d", sd_err);
		return sd_err;
	}
	ResetCommand();
	// Fill command information
	sdCmd.cmd = AT91C_SD_SET_BUS_WIDTH_CMD;
	sdCmd.arg = (busWidth == 4) ? SD_STAT_DATA_BUS_WIDTH_4BIT : SD_STAT_DATA_BUS_WIDTH_1BIT;
	sdCmd.resType = 1;
	sdCmd.pResp = &response;
	// Send command
	return SendCommand();
}

//------------------------------------------------------------------------------
/// The SD Status contains status bits that are related to the SD memory Card
/// proprietary features and may be used for future application-specific usage.
/// Can be sent to a card only in 'tran_state'.
//------------------------------------------------------------------------------
static U8 Acmd13(U32 * pSdSTAT)
{
	U8 sd_err;
	U32 response[1];

	//DEBUG_MSG("Acmd13()");
	sd_err = Cmd55();
	if(sd_err)
	{
		TRACE_ERR("Acmd13.Cmd55: %d", sd_err);
		return sd_err;
	}
	ResetCommand();
	// Fill command information
	sdCmd.cmd = AT91C_SD_STATUS_CMD;
	sdCmd.resType = 1;
	sdCmd.pResp = response;
	sdCmd.blockSize = 512 / 8;
	sdCmd.nbBlock = 1;
	sdCmd.pData = (U8*)pSdSTAT;
	sdCmd.dataTran = 1;
	sdCmd.isRead = 1;
	sdCmd.tranType = MCI_NEW_TRANSFER;
	// Send command
	sd_err = SendCommand();

	return sd_err;
}

//------------------------------------------------------------------------------
/// Asks to all cards to send their operations conditions.
/// Returns the command transfer result (see SendCommand).
/// param: pSd  Pointer to a SD card driver instance.
/// param: hcs  Shall be true if Host support High capacity.
/// param: pCCS  Set the pointed flag to 1 if hcs != 0 and SD OCR CCS flag is set.
//------------------------------------------------------------------------------
static U8 Acmd41(U8 hcs, U8 *pCCS)
{
	U8 sd_err;
	U32  response;

	do
	{
		sd_err = Cmd55();
		if (sd_err)
		{
			return sd_err;
		}

		ResetCommand();
		// Fill command information
		sdCmd.cmd = AT91C_SD_APP_OP_COND_CMD;
		sdCmd.arg = AT91C_MMC_HOST_VOLTAGE_RANGE;
		if (hcs)
		{
			sdCmd.arg |= AT91C_CCS;
		}
		sdCmd.resType = 3;
		sdCmd.pResp = &response;
		// Send command

		sd_err = SendCommand();
		if (sd_err)
		{
			TRACE_ERR("Acmd41() ERROR: %d", sd_err);
			return sd_err;
		}
		*pCCS  = ((response & AT91C_CCS) != 0);
	}
	while((response & AT91C_CARD_POWER_UP_BUSY) != AT91C_CARD_POWER_UP_BUSY);

	return 0;
}

//------------------------------------------------------------------------------
/// SD Card Configuration Register (SCR) provides information on the SD Memory
/// Card's special features that were configured into the given card. The size
/// of SCR register is 64 bits.
//------------------------------------------------------------------------------
static U8 Acmd51(U32 * pSCR)
{
	U8 sd_err;
	U32 response[1];

	//DEBUG_MSG("Acmd51()");

	sd_err = Cmd55();
	if(sd_err)
	{
		TRACE_ERR("Acmd51.Cmd55() ERROR: %d", sd_err);
		return sd_err;
	}

	ResetCommand();
	// Fill command information
	sdCmd.cmd = AT91C_SD_SEND_SCR_CMD;
	sdCmd.resType = 1;
	sdCmd.pResp = response;

	sdCmd.blockSize = 64 / 8;
	sdCmd.nbBlock = 1;
	sdCmd.pData = (U8*)pSCR;

	sdCmd.dataTran = 1;
	sdCmd.isRead = 1;
	sdCmd.tranType = MCI_NEW_TRANSFER;

	// Send command
	sd_err = SendCommand();
	return sd_err;
}

//------------------------------------------------------------------------------
/// Continue to transfer datablocks from card to host until interrupted by a
/// STOP_TRANSMISSION command.
/// param: pSd  Pointer to a SD card driver instance.
/// param: blockSize  Block size (shall be set to 512 in case of high capacity).
/// param: pData  Pointer to the application buffer to be filled.
/// param: address  SD card address.
//------------------------------------------------------------------------------
static U8 ContinuousRead(U16 nbBlock, U8 *pData, SdCallback pCb, void *pArg)
{
	DEBUG_MSG("ContinuousRead()");
	ResetCommand();
	// Fill command information
	sdCmd.blockSize = SD_BLOCK_SIZE;
	sdCmd.nbBlock   = nbBlock;
	sdCmd.pData     = pData;

	sdCmd.dataTran = 1;
	sdCmd.tranType = MCI_CONTINUE_TRANSFER;
	sdCmd.isRead = 1;

	sdCmd.callback = pCb;
	sdCmd.pArg     = pArg;

	// Send command
	return SendCommand();
}

//------------------------------------------------------------------------------
/// Continue to transfer datablocks from host to card until interrupted by a
/// STOP_TRANSMISSION command.
/// param: pSd  Pointer to a SD card driver instance.
/// param: blockSize  Block size (shall be set to 512 in case of high capacity).
/// param: pData  Pointer to the application buffer to be filled.
//------------------------------------------------------------------------------
static U8 ContinuousWrite(U16 nbBlock, const U8 *pData, SdCallback pCb, void *pArg)
{
	DEBUG_MSG("ContinuousWrite()");
	ResetCommand();
	// Fill command information
	sdCmd.blockSize = SD_BLOCK_SIZE;
	sdCmd.nbBlock   = nbBlock;
	sdCmd.pData     = (U8*)pData;

	sdCmd.dataTran = 1;
	sdCmd.tranType = MCI_CONTINUE_TRANSFER;

	sdCmd.callback = pCb;
	sdCmd.pArg     = pArg;

	// Send command
	return SendCommand();
}

//------------------------------------------------------------------------------
/// Try SW Reset several times (CMD0 with ARG 0)
/// param: pSd      Pointer to a SD card driver instance.
/// param: retry    Retry times.
/// return 0 or MCI sd_err code.
//------------------------------------------------------------------------------
static U8 SwReset(U32 retry)
{
	U32 i;
	U8 sd_err = 0;

	for (i = 0; i < retry; i ++)
	{
		sd_err = Cmd0(0);
		if(sd_err != MCI_STATUS_NORESPONSE) break;
	}
	return sd_err;
}

//------------------------------------------------------------------------------
/// Move SD card to transfer state.
//------------------------------------------------------------------------------
static U8 MoveToTranState(void)
{
	U8 sd_err;
	U32  status;

	// Quit transfer state
	if((sdDrv.state == SD_STATE_READ) || (sdDrv.state == SD_STATE_WRITE))
	{
		sd_err = Cmd12((sdDrv.state == SD_STATE_READ), &status);
		if(sd_err)
		{
			TRACE_ERR("MvToTran.Cmd12: %d", sd_err);
			return sd_err;
		}
	}
	// Put device into tran state
	sd_err = Cmd13(&status);
	if(sd_err)
	{
		TRACE_ERR("MvToTran.Cmd13: %d", sd_err);
		return sd_err;
	}
	if((status & STATUS_STATE) == STATUS_STBY)
	{
		sd_err = Cmd7(sdDrv.cardAddress);
		if(sd_err)
		{
			TRACE_ERR("MvToTran.Cmd7: %d", sd_err);
			return sd_err;
		}
	}
	return 0;
}

//------------------------------------------------------------------------------
/// Move SD card to transfer state. The buffer size must be at
/// least 512 byte long. This function checks the SD card status register and
/// address the card if required before sending the transfer command.
/// Returns 0 if successful; otherwise returns an code describing the sd_err.
/// param: pSd      Pointer to a SD card driver instance.
/// param: address  Address of the block to transfer.
/// param: nbBlocks Number of blocks to be transfer, 0 for infinite transfer.
/// param: pData    Data buffer whose size is at least the block size.
/// param: isRead   1 for read data and 0 for write data.
//------------------------------------------------------------------------------
static U8 MoveToTransferState(U32 address, U16 nbBlocks, U8 *pData, U8 isRead)
{
	U32 status;
	U8 sd_err;

	if((sdDrv.state == SD_STATE_READ) || (sdDrv.state == SD_STATE_WRITE))
	{
		if (sdDrv.state == SD_STATE_WRITE)
		{
// 			DBGU_PutByte(0);
// 			DBGU_PutByte(0);
// 			DBGU_PutByte(0);
// 			DBGU_PutByte(0);
// 			DEBUG_WP(".");
// 			DEBUG_WP(".");
// 			DEBUG_WP(".");
// 			DEBUG_WP(".");
		}

		// RW MULTI with length
		sd_err = Cmd12((sdDrv.state == SD_STATE_READ), &status);
		if(sd_err)
		{
			TRACE_ERR("MTTranState.Cmd12: st%x, er%d", sdDrv.state, sd_err);
			return sd_err;
		}
	}

	if(isRead)
	{
		// Wait for card to be ready for data transfers
		do
		{
			sd_err = Cmd13(&status);
			if(sd_err)
			{
				TRACE_ERR("MTTranState.RD.Cmd13: %d", sd_err);
				return sd_err;
			}
			if(((status & STATUS_STATE) == STATUS_IDLE)||((status & STATUS_STATE) == STATUS_READY)||((status & STATUS_STATE) == STATUS_IDENT))
			{
				DEBUG_MSG("Pb Card Identification mode");
				return SD_ERROR_NOT_INITIALIZED;
			}
			// If the card is in sending data state or in receivce data state
			if(((status & STATUS_STATE) == STATUS_RCV)||((status & STATUS_STATE) == STATUS_DATA))
			{
				DEBUG_MSG("state = 0x%X", (status & STATUS_STATE) >> 9);
			}
		}
		while (((status & STATUS_READY_FOR_DATA) == 0)||((status & STATUS_STATE) != STATUS_TRAN));
		
		// Read data
		// Move to Sending data state
		sd_err = Cmd18(nbBlocks, pData, SD_ADDRESS(address), &status);
		if(sd_err)
		{
			TRACE_ERR("MTTranState.Cmd18: %d", sd_err);
			return sd_err;
		}
		if(status & ~(STATUS_READY_FOR_DATA | STATUS_STATE))
		{
			TRACE_ERR("CMD18.stat: %x", status & ~(STATUS_READY_FOR_DATA | STATUS_STATE));
			return SD_ERROR_DRIVER;
		}
	}
	else
	{
		// Wait for card to be ready for data transfers
		do
		{
			sd_err = Cmd13(&status);
			if(sd_err)
			{
				TRACE_ERR("MoveToTransferState.WR.Cmd13: %d", sd_err);
				return sd_err;
			}
		}
		while((status & STATUS_READY_FOR_DATA) == 0);
		// Move to Sending data state
		sd_err = Cmd25(nbBlocks, pData, SD_ADDRESS(address), &status);
		if(sd_err)
		{
			TRACE_ERR("MoveToTransferState.Cmd25: %d", sd_err);
			return sd_err;
		}
		if(status & (STATUS_WRITE & ~(STATUS_READY_FOR_DATA | STATUS_STATE)))
		{
			DEBUG_MSG("CMD25(0x%x, %d).stat: %x", SD_ADDRESS(address), nbBlocks, status & (STATUS_WRITE & ~(STATUS_READY_FOR_DATA | STATUS_STATE)));
			return SD_ERROR_DRIVER;
		}
	}

	if (!sd_err) sdDrv.preBlock = address + (nbBlocks-1);
	return sd_err;
}

//------------------------------------------------------------------------------
/// Switch the HS mode of card
/// param: pSd      Pointer to SdCard instance.
/// param: hsEnable 1 to enable, 0 to disable.
//------------------------------------------------------------------------------
static U8 SdMmcSwitchHsMode(U8 hsEnable)
{
	U32 status;
	U8 sd_err = SD_ERROR_DRIVER;
	SdCmd6Arg cmd6Arg;
	U32 switchStatus[512/32];
	
	if (sdDrv.mode == hsEnable) return 0;
	
	if (sdDrv.cardType >= CARD_SD)
	{
		cmd6Arg.mode = 1;
		cmd6Arg.reserved = 0;
		cmd6Arg.reserveFG6 = 0xF;
		cmd6Arg.reserveFG5 = 0xF;
		cmd6Arg.reserveFG4 = 0xF;
		cmd6Arg.reserveFG3 = 0xF;
		cmd6Arg.command = 0;
		cmd6Arg.accessMode = 1;
		sd_err = Cmd6(&cmd6Arg, switchStatus, &status);

		if (sd_err)
		{
			TRACE_ERR("SD SwitchHs.Cmd6: %d", sd_err);
		}
		else if(status & STATUS_SWITCH_ERROR)
		{
			TRACE_ERR("SD HS SW Fail");
			sd_err = SD_ERROR_DRIVER;
		}
		else if(SD_SW_STAT_FUN_GRP1_RC(switchStatus) == SD_SW_STAT_FUN_GRP_RC_ERROR)
		{
			TRACE_ERR("SD HS Not Supported");
			sd_err = SD_ERROR_DRIVER;
		}
		else if(SD_SW_STAT_FUN_GRP1_BUSY(switchStatus))
		{
			TRACE_ERR("SD HS Busy");
			sd_err = SD_ERROR_DRIVER;
		}
		else
		{
			DEBUG_MSG("SD HS %d", hsEnable);
			sdDrv.mode = hsEnable;
		}
	}
	return sd_err;
}

//------------------------------------------------------------------------------
/// Switch card state between STBY and TRAN
/// param: pSd      Pointer to a SD card driver instance.
/// param: address  Card address to TRAN, 0 to STBY
/// param: check    Whether to check the state
//------------------------------------------------------------------------------
static U8 MmcSelectCard(U16 address, U8 check)
{
	U8 sd_err;
	U32 status;
	U32 targetState = address ? STATUS_TRAN : STATUS_STBY;
	U32 srcState = address ? STATUS_STBY : STATUS_TRAN;
	if (sdDrv.cardType == CARD_SDIO) check = 0;

	// At this stage the Initialization and identification process is achieved
	// The SD card is supposed to be in Stand-by State
	while(check)
	{
		sd_err = Cmd13(&status);
		if (sd_err)
		{
			TRACE_ERR("MmcSelectCard.Cmd13 (%d)", sd_err);
			return sd_err;
		}
		if ((status & STATUS_READY_FOR_DATA))
		{
			U32 currState = status & STATUS_STATE;
			if (currState == targetState) return 0;
			if (currState != srcState)
			{
				TRACE_ERR("MmcSelectCard, wrong state %x", currState);
				return SD_ERROR_DRIVER;
			}
			break;
		}
	}

	// witch to TRAN mode to Select the current SD/MMC
	// so that SD ACMD6 can process or EXT_CSD can read.
	sd_err = Cmd7(address);
	if (sd_err == SD_ERROR_NOT_INITIALIZED && address == 0) {}
	else if (sd_err) {
	TRACE_ERR("MmcSelectCard.Cmd7 (%d)", sd_err);
	}

	return sd_err;
}

//------------------------------------------------------------------------------
/// Get SCR and SD Status information
/// param: pSd      Pointer to a SD card driver instance.
//------------------------------------------------------------------------------
static U8 SdGetExtInformation(void)
{
	U8 sd_err;

	// Reset data (64 + 512 bits, 8 + 64 bytes)
	// memset(sdDrv.extData, 0x00, 512);

	// SD Status
	if (sdDrv.optCmdBitMap & SD_ACMD13_SUPPORT)
	{
		sd_err = Acmd13(&sdDrv.extData[SD_EXT_OFFSET_SD_STAT]);
		if (sd_err)
		{
			TRACE_ERR("SdGetExt.Acmd13: %d", sd_err);
			sdDrv.optCmdBitMap &= ~SD_ACMD13_SUPPORT;
		}
	}
	// SD SCR
	sd_err = Acmd51(&sdDrv.extData[SD_EXT_OFFSET_SD_SCR]);
	if (sd_err)
	{
		TRACE_ERR("SdGetExt.Acmd51: %d", sd_err);
	}

	return 0;
}

//------------------------------------------------------------------------------
/// Update SD/MMC information.
/// Update CSD for card speed switch.
/// Update ExtDATA for any card function switch.
/// param: pSd      Pointer to a SD card driver instance.
/// return sd_err code when update CSD sd_err.
//------------------------------------------------------------------------------
static U8 SdMmcUpdateInformation(U8 csd, U8 extData)
{
	U8 sd_err;

	if(csd) // Update CSD for new TRAN_SPEED value
	{
		MmcSelectCard(0, 1);
		Delay_ms(10);
		sd_err = Cmd9();
		if (sd_err )
		{
			TRACE_ERR("SdMmcUpdateInfo.Cmd9 (%d)", sd_err);
			return sd_err;
		}
		sd_err = MmcSelectCard(sdDrv.cardAddress, 1);
	}
	if (sdDrv.cardType >= CARD_SD) SdGetExtInformation();
	GetTransSpeedValue();

	return 0;
}

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Read Block of data in a buffer pointed by pData. The buffer size must be at
/// least 512 byte long. This function checks the SD card status register and
/// address the card if required before sending the read command.
/// Returns 0 if successful; otherwise returns an code describing the sd_err.
/// param: pSd      Pointer to a SD card driver instance.
/// param: address  Address of the block to read.
/// param: pData    Data buffer whose size is at least the block size, it can
///            be 1,2 or 4-bytes aligned when used with DMA.
/// param: length   Number of blocks to be read.
/// param: pCallback Pointer to callback function that invoked when read done.
///                  0 to start a blocked read.
/// param: pArgs     Pointer to callback function arguments.
//------------------------------------------------------------------------------
U8 SD_Read(U32 address, void *pData, U16 length, SdCallback pCallback, void *pArgs)
{
	U8 sd_err;
	
	if(pCallback) // If callback is defined, performe none blocked reading
	{
		if(MCI_IsTxComplete() == 0)
		{
			return SD_ERROR_BUSY;
		}
	}

	if(sdDrv.state != SD_STATE_READ || sdDrv.preBlock + 1 != address)
	{
		sd_err = MoveToTransferState(address, 0, 0, 1); // Start infinite block reading
	}
	else sd_err = 0;
	if(!sd_err)
	{
		sdDrv.state = SD_STATE_READ;
		sdDrv.preBlock = address + (length - 1);
		sd_err = ContinuousRead(length, pData, pCallback, pArgs);
	}
	DEBUG_MSG("SDrd(%u,%u):%u", address, length, sd_err);
	return 0;    
}

//------------------------------------------------------------------------------
/// Write Block of data in a buffer pointed by pData. The buffer size must be at
/// least 512 byte long. This function checks the SD card status register and
/// address the card if required before sending the read command.
/// Returns 0 if successful; otherwise returns an code describing the sd_err.
/// param: pSd      Pointer to a SD card driver instance.
/// param: address  Address of the block to read.
/// param: pData    Data buffer whose size is at least the block size, it can
///            be 1,2 or 4-bytes aligned when used with DMA.
/// param: length   Number of blocks to be read.
/// param: pCallback Pointer to callback function that invoked when read done.
///                  0 to start a blocked read.
/// param: pArgs     Pointer to callback function arguments.
//------------------------------------------------------------------------------
U8 SD_Write(U32 address, void *pData, U16 length, SdCallback pCallback, void *pArgs)
{
	U8 sd_err;

	if(pCallback) // If callback is defined, performe none blocked writing
	{
		if(MCI_IsTxComplete() == 0)
		{
			return SD_ERROR_BUSY;
		}
	}
	if(sdDrv.state != SD_STATE_WRITE || sdDrv.preBlock + 1 != address)
	{
		sd_err = MoveToTransferState(address, 0, 0, 0); // Start infinite block writing
	}
	else sd_err = 0;
	if(!sd_err)
	{
		sdDrv.state = SD_STATE_WRITE;
		sd_err = ContinuousWrite(length, pData, pCallback, pArgs);
		sdDrv.preBlock = address + (length - 1);
	}
	DEBUG_MSG("SDwr(%u,%u):%u", address, length, sd_err);
	return 0;
}

//------------------------------------------------------------------------------
/// Read Block of data in a buffer pointed by pData. The buffer size must be at
/// least 512 byte long. This function checks the SD card status register and
/// address the card if required before sending the read command.
/// Returns 0 if successful; otherwise returns an code describing the sd_err.
/// param: pSd  Pointer to a SD card driver instance.
/// param: address  Address of the block to read.
/// param: nbBlocks Number of blocks to be read.
/// param: pData    Data buffer whose size is at least the block size, it can
///            be 1,2 or 4-bytes aligned when used with DMA.
//------------------------------------------------------------------------------
U8 SD_ReadBlock(U32 address, U16 nbBlocks, U8 *pData)
{
	U8 sd_err = 0;

	DEBUG_MSG("ReadBlk(%d,%d)", address, nbBlocks);
	if(sdDrv.state != SD_STATE_READ || sdDrv.preBlock + 1 != address)
	{
		// Start infinite block reading
		sd_err = MoveToTransferState(address, 0, 0, 1);
	}
	if(!sd_err)
	{
		sdDrv.state = SD_STATE_READ;
		sd_err = ContinuousRead(nbBlocks, pData, 0, 0);
		if (!sd_err) sdDrv.preBlock = address + (nbBlocks - 1);
	}
	return sd_err;
}

//------------------------------------------------------------------------------
/// Write Block of data pointed by pData. The buffer size must be at
/// least 512 byte long. This function checks the SD card status register and
/// address the card if required before sending the read command.
/// Returns 0 if successful; otherwise returns an SD_ERROR code.
/// param: pSd  Pointer to a SD card driver instance.
/// param: address  Address of block to write.
/// param: nbBlocks Number of blocks to be read
/// param: pData    Data buffer whose size is at least the block size, it can
///            be 1,2 or 4-bytes aligned when used with DMA.
//------------------------------------------------------------------------------
U8 SD_WriteBlock(U32 address, U16 nbBlocks, const U8 *pData)
{
	U8 sd_err = 0;

	DEBUG_MSG("WriteBlk(%d,%d)", address, nbBlocks);
	if(sdDrv.state != SD_STATE_WRITE || sdDrv.preBlock + 1 != address)
	{
		sd_err = MoveToTransferState(address, 0, 0, 0); // Start infinite block writing
	}
	if(!sd_err)
	{
		sdDrv.state = SD_STATE_WRITE;
		sd_err = ContinuousWrite(nbBlocks, pData, 0, 0);
		if(!sd_err) sdDrv.preBlock = address + (nbBlocks - 1);
	}
	return sd_err;
}

//------------------------------------------------------------------------------
/// Run the SDcard SD/MMC/SDIO Mode initialization sequence.
/// This function resets both IO and memory controller, runs the initialisation
/// procedure and the identification process. Then it leaves the card in ready
/// state. The following command must check the card type and continue to put
/// the card into tran(for memory card) or cmd(for io card) state for data
/// exchange.
/// Returns 0 if successful; otherwise returns an SD_ERROR code.
/// param: pSd  Pointer to a SD card driver instance.
//------------------------------------------------------------------------------
static U8 SdMmcIdentify(void)
{
	U8 f8 = 0, mp = 1, ccs = 0;
	U8 sd_err = 0;

	// Reset HC to default HS and BusMode
	MCI_EnableHsMode(0);
	MCI_SetBusWidth(MCI_SDCBUS_1BIT);

	// Reset MEM
	sd_err = SwReset(1);
	if(sd_err)
	{
		TRACE_ERR("SdMmcIdentify.SwReset: %d", sd_err)
	}

	// CMD8 is newly added in the Physical Layer Specification Version 2.00 to
	// support multiple voltage ranges and used to check whether the card
	// supports supplied voltage. The version 2.00 host shall issue CMD8 and
	// verify voltage before card initialization.
	// The host that does not support CMD8 shall supply high voltage range...
	sd_err = Cmd8(1, (void*)1);
	if(sd_err == 0)
	{
		f8 = 1;
	}
	else if(sd_err != SD_ERROR_NORESPONSE)
	{
		TRACE_ERR("SdMmcIdentify.Cmd8: %d", sd_err);
		return SD_ERROR_DRIVER;
	}
	else
	{
		// Delay after "no response"
		Delay_ms(20);
	}

	// SD or MMC or COMBO: mp is 1
	if(mp)
	{
		// Try SD memory initialize
		sd_err = Acmd41(f8, &ccs);
		if(sd_err)
		{
			sdDrv.cardType = CARD_UNKNOWN;
			TRACE_ERR("MMC card is not supported by bootloarder: %d", sd_err);
			return 0; // MMC Identified OK
		}
		else if(ccs)
		{
			sdDrv.cardType = CARD_SDHC;
			DEBUG_MSG("SDHC MEM");
		}
		else
		{
			sdDrv.cardType = CARD_SD;
			DEBUG_MSG("SD MEM");
		}
	}
	else sdDrv.cardType = CARD_UNKNOWN;
	
	return 0;
}

//------------------------------------------------------------------------------
/// Run the SDcard SD Mode enumeration sequence. This function runs after the
/// initialisation procedure and the identification process. It sets the
/// SD card in transfer state to set the block length and the bus width.
/// Returns 0 if successful; otherwise returns an SD_ERROR code.
/// param: pSd  Pointer to a SD card driver instance.
//------------------------------------------------------------------------------
static U8 SdMmcEnum(void)
{
	U8 mem = 0, io = 0;
	U32 status;
	U16 sd_err;
	U8 isHsSupport = 0;
	U8 updateInformation = 0;

	if (sdDrv.cardType & CARD_TYPE_bmSDMMC) mem = 1;
	if (sdDrv.cardType & CARD_TYPE_bmSDIO)  io = 1;

	// For MEM
	// The host then issues the command ALL_SEND_CID (CMD2) to the card to get
	// its unique card identification (CID) number.
	// Card that is unidentified (i.e. which is in Ready State) sends its CID
	// number as the response (on the CMD line).
	if (mem)
	{
		sd_err = Cmd2();
		if (sd_err)
		{
			TRACE_ERR("SdMmcEnum.Cmd2: %d", sd_err);
			return SD_ERROR_DRIVER;
		}
	}

	// For SDIO & MEM
	// Thereafter, the host issues CMD3 (SEND_RELATIVE_ADDR) asks the
	// card to publish a new relative card address (RCA), which is shorter than
	// CID and which is used to address the card in the future data transfer
	// mode. Once the RCA is received the card state changes to the Stand-by
	// State. At this point, if the host wants to assign another RCA number, it
	// can ask the card to publish a new number by sending another CMD3 command
	// to the card. The last published RCA is the actual RCA number of the card.
	sd_err = Cmd3();
	if (sd_err)
	{
		TRACE_ERR("SdMmcInit.Cmd3 %d", sd_err);
		return SD_ERROR_DRIVER;
	}

	// For MEM
	// SEND_CSD (CMD9) to obtain the Card Specific Data (CSD register),
	// e.g. block length, card storage capacity, etc...
	if(mem)
	{
		sd_err = Cmd9();
		if(sd_err)
		{
			TRACE_ERR("SdMmcInit.Cmd9 %d", sd_err);
			return SD_ERROR_DRIVER;
		}
	}

	// For SDIO & MEM
	// Now select the card, to TRAN state
	sd_err = MmcSelectCard(sdDrv.cardAddress, 0);
	if(sd_err)
	{
		TRACE_ERR("SdMmcInit.Sel %d", sd_err);
		return SD_ERROR_DRIVER;
	}
	// SDIO only card, enumeration done
	if(!mem && io)
	{
		// Default tranSpeed: 25MHz
		sdDrv.transSpeed = 25000000;
		return 0;
	}

	// For MEM cards or combo
	// If the card support EXT_CSD, read it!
	DEBUG_MSG("Card Type %d, CSD_STRUCTURE %d", sdDrv.cardType, (((sdDrv.csd)[0] >> 30) & 3));

	// Get extended information of the card
	SdMmcUpdateInformation(0, 0);

	if(sdDrv.cardType >= CARD_SD)
	{
		// Switch to 4-bits bus width
		// (All SD Card shall support 1-bit or 4 bits width)
		sd_err = Acmd6(4);
		DEBUG_MSG("SD 4-BITS BUS");
		if (sd_err)
		{
			TRACE_ERR("SdMmcInit.12 (%d)", sd_err);
			return sd_err;
		}
		MCI_SetBusWidth(MCI_SDCBUS_4BIT);
		
		// SD Spec V1.10 or higher, switch to high-speed mode
		if (sdDrv.extData[0] >= 1)
		{
			SdCmd6Arg cmd6Arg;
			U32 switchStatus[512/32];
			cmd6Arg.mode = 1;
			cmd6Arg.reserved = 0;
			cmd6Arg.reserveFG6 = 0xF;
			cmd6Arg.reserveFG5 = 0xF;
			cmd6Arg.reserveFG4 = 0xF;
			cmd6Arg.reserveFG3 = 0xF;
			cmd6Arg.command = 0;
			cmd6Arg.accessMode = 1;
			sd_err = Cmd6(&cmd6Arg, switchStatus, &status);

			if(sd_err || (status & STATUS_SWITCH_ERROR))
			{
				TRACE_WRN("OS Boot SD Card does not support high speed mode.");
			}
			else if(SD_SW_STAT_FUN_GRP1_RC(switchStatus) == SD_SW_STAT_FUN_GRP_RC_ERROR)
			{
				TRACE_WRN("OS Boot SD Card does not support high speed mode.");
			}
			else if(SD_SW_STAT_FUN_GRP1_BUSY(switchStatus))
			{
				TRACE_ERR("SD HS Busy");
			}
			else
			{
				MCI_EnableHsMode(1);
				DEBUG_MSG("SD HS Enabled");
				isHsSupport = 1;
			}
		}
		updateInformation = 1; // Update
	}

	if (updateInformation)
	{
		SdMmcUpdateInformation(isHsSupport, 1);
	}
	return 0;
}

//------------------------------------------------------------------------------
/// Run the SDcard initialization sequence. This function runs the
/// initialisation procedure and the identification process, then it sets the
/// SD card in transfer state to set the block length and the bus width.
/// Returns 0 if successful; otherwise returns an SD_ERROR code.
/// param: pSd  Pointer to a SD card driver instance.
/// param: pSdDriver  Pointer to SD driver already initialized.
//------------------------------------------------------------------------------
U8 SD_Init(void)
{
	U8 sd_err;

	// Initialize SdCard structure
	sdDrv.cardAddress = 0;
	sdDrv.preBlock = 0xffffffff;
	sdDrv.state = SD_STATE_INIT;
	sdDrv.cardType = CARD_UNKNOWN;
	sdDrv.optCmdBitMap = 0xFFFFFFFF;
	sdDrv.mode = 0;
	ResetCommand();

	// Initialization delay: The maximum of 1 msec, 74 clock cycles and supply
	// ramp up time. Supply ramp up time provides the time that the power is
	// built up to the operating level (the bus master supply voltage) and the
	// time to wait until the SD card can accept the first command.

	// Power On Init Special Command

	sd_err = Pon();
	if(sd_err)
	{
		TRACE_ERR("SD_Init.1 (%d)", sd_err);
		return sd_err;
	}

	// After power-on or CMD0, all cards?CMD lines are in input mode, waiting
	// for start bit of the next command.
	// The cards are initialized with a default relative card address
	// (RCA=0x0000) and with a default driver stage register setting
	// (lowest speed, highest driving current capability).
	
	sd_err = SdMmcIdentify();
	if(sd_err)
	{
		TRACE_ERR("SD_Init.Identify");
		return sd_err;
	}

	sd_err = SdMmcEnum();
	if(sd_err)
	{
		TRACE_ERR("SD_Init.Enum");
		return sd_err;
	}

	// In the case of a Standard Capacity SD Memory Card, this command sets the
	// block length (in bytes) for all following block commands
	// (read, write, lock).
	// Default block length is fixed to 512 Bytes.
	// Set length is valid for memory access commands only if partial block read
	// operation are allowed in CSD.
	// In the case of a High Capacity SD Memory Card, block length set by CMD16
	// command does not affect the memory read and write commands. Always 512
	// Bytes fixed block length is used. This command is effective for
	// LOCK_UNLOCK command.
	// In both cases, if block length is set larger than 512Bytes, the card sets
	// the BLOCK_LEN_ERROR bit.
	
	if(sdDrv.cardType == CARD_SD)
	{
		sd_err = Cmd16(SD_BLOCK_SIZE);
		if (sd_err)
		{
			sdDrv.optCmdBitMap &= ~SD_CMD16_SUPPORT;
			TRACE_ERR("SD_Init.Cmd16 (%d)", sd_err);
			TRACE_ERR("Fail to set BLK_LEN, default is 512");
		}
	}
	sdDrv.state = SD_STATE_READY; // Reset status for R/W


	if(sdDrv.cardType >= CARD_SD && sdDrv.cardType < CARD_MMC && (((sdDrv.csd)[0] >> 30) & 3) >= 1)
	{
		sdDrv.blockNr   = SD_CSD_BLOCKNR_HC();
		sdDrv.totalSize = 0xFFFFFFFF;
	}
	else // Normal card
	{
		sdDrv.totalSize = SD_CSD_TOTAL_SIZE();
		sdDrv.blockNr = SD_CSD_BLOCKNR();
	}

	while (!MCI_IsTxComplete());
	if (sdDrv.cardType == CARD_UNKNOWN)
	{
		return SD_ERROR_NOT_INITIALIZED;
	}
	else
	{
		return 0;
	}	
}

//------------------------------------------------------------------------------
/// Stop the SDcard. This function stops all SD operations.
/// Returns 0 if successful; otherwise returns an SD_ERROR code.
/// param: pSd  Pointer to a SD card driver instance.
/// param: pSdDriver  Pointer to MCI driver already initialized.
//------------------------------------------------------------------------------
U8 SD_Stop(void)
{
	U8 sd_err;

	//if(sdDrv == 0) return 0;

	if(sdCmd.tranType == MCI_CONTINUE_TRANSFER)
	{
		DEBUG_MSG("SD_StopTransmission()");
		sd_err = Cmd12((sdDrv.state != SD_STATE_WRITE), 0);
		if(sd_err)
		{
			return sd_err;
		}
	}
	MCI_Close();
	return 0;
}

//------------------------------------------------------------------------------
/// Switch the SD/MMC card to High-Speed mode.
/// sdDrv.transSpeed will change to new speed limit.
/// Invoke MCI_SetSpeed() and MCI_EnableHsMode() to change MCI timing after.
/// For SD/MMC card, the speed mode will not change back until another init.
/// param: pSd      Pointer to a SD card driver instance.
/// param: hsMode   1 to enable HS mode, 0 to disable
///                 0xFF to return current mode.
/// return current mode is hsMode is 0xFF;
///         sd_err code if hsMode is 0 or 1.
//------------------------------------------------------------------------------
U8 SD_HighSpeedMode(U8 hsMode)
{
	U8 sd_err = 0;

	if(hsMode == 0xFF) return sdDrv.mode;
	if(hsMode == 0)
	{
		TRACE_ERR("Can not switch, do re-init to disable HS mode");
		return SD_ERROR_DRIVER;
	}

	// Quit transfer state
	sd_err = MoveToTranState();
	if(sd_err)
	{
		TRACE_ERR("SD_HighSpeedMode.Tran: %d", sd_err);
		return sd_err;
	}

	if(sdDrv.mode != hsMode)
	{
		sd_err = SdMmcSwitchHsMode(hsMode);
		if(sd_err == 0) sd_err = SdMmcUpdateInformation(1, 1);
	}
	// Reset state for data R/W
	sdDrv.state = SD_STATE_READY;
	return sd_err;
}

//------------------------------------------------------------------------------
/// return size of the card in KB
//------------------------------------------------------------------------------
U32 MMC_GetTotalSizeKB(void)
{
	if (sdDrv.totalSize == 0xFFFFFFFF)
	{
		return sdDrv.blockNr / 2;
	}

	return sdDrv.totalSize / 1024;
}
