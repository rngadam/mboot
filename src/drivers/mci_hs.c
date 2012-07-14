/* ----------------------------------------------------------------------------
 *		 
 *        mboot for Lophilo embedded system
 *		
 * ----------------------------------------------------------------------------
 */

#include "types.h"
#include "trace.h"
#include "math.h"

#include "board.h"
#include "sfr.h"
#include "system.h"
#include "dma.h"

#include "mci_hs.h"

#define MCI_FIFO_SIZE		(0x4000-0x400)     // 15.5K
#define DMA_XFR_SIZE		(0xF00)    // SAM3: 0xFFF, 9M10: 0xFFFF
#define NUM_LLI					(5)
#define DMA_TOTAL				(DMA_XFR_SIZE*NUM_LLI)
#define LAST_ROW				0x100

// Bit mask for status register errors.
#define STATUS_ERRORS					((U32)(AT91C_MCI_UNRE | AT91C_MCI_OVRE | AT91C_MCI_BLKOVRE | AT91C_MCI_CSTOE | AT91C_MCI_DTOE | AT91C_MCI_DCRCE | AT91C_MCI_RTOE | AT91C_MCI_RENDE | AT91C_MCI_RCRCE | AT91C_MCI_RDIRE | AT91C_MCI_RINDE))
// MCI data timeout configuration with 1048576 MCK cycles between 2 data transfers.
#define DTOR_1MEGA_CYCLES			(AT91C_MCI_DTOCYC | AT91C_MCI_DTOMUL)

Mci mciDrv; /// MCI driver instance.
MciCmd sdCmd;

//------------------------------------------------------------------------------
//         Local variables
//------------------------------------------------------------------------------

static U16 xfredBlocks = 0xFFFF;
static U8 dmaLastLliNdx = 0;
static DmaLinkList  LLI_MCI [NUM_LLI];   // Max: > 64K Bytes

//------------------------------------------------------------------------------
//         Internal Functions
//------------------------------------------------------------------------------

void AT91F_Prepare_Multiple_Transfer(U32 Channel, U32 LLI_rownumber, U32 LLI_Last_Row, U32 From_add, U32 To_add, U32 Ctrla, U32 Ctrlb)
{
	LLI_MCI[LLI_rownumber].sourceAddress = From_add;
	LLI_MCI[LLI_rownumber].destAddress = To_add;
	LLI_MCI[LLI_rownumber].controlA = Ctrla;
	LLI_MCI[LLI_rownumber].controlB = Ctrlb;
	if(LLI_Last_Row != LAST_ROW) LLI_MCI[LLI_rownumber].descriptor = (U32)&LLI_MCI[LLI_rownumber + 1] + 0;
	else
	{
		dmaLastLliNdx = LLI_rownumber;
		LLI_MCI[LLI_rownumber].descriptor = 0;
	}
}

// trans_size: number of transfers in SRC side
// forceByte: 1 - byte count, 0 - word count // src_addr = HSMCI_FIFO(ch)
U32 DMACH_MCI_P2M(U32 channel_index, U8* dest_addr, U32 trans_size, U8 addrIncMode, U8 forceByte)
{
	U32 srcAddress;
	U32 destAddress;
	U32 buffSize;
	U32 LLI_rownumber = 0;
	U32 srcAddressMode = addrIncMode ? (AT91C_HDMA_SRC_ADDRESS_MODE_INCR) : (AT91C_HDMA_SRC_ADDRESS_MODE_FIXED);
	U32 scSize, dcSize, mWidth, perWidth, addrInc;

	DMA_DisableChannel(channel_index); // Disable dma channel
	
	// DMA channel configuration
	srcAddress = (U32)(HSMCI_FIFO(0)); // Set the data start address
	destAddress = (U32)dest_addr; //(U32)SSC_THR_ADD; 
	buffSize = trans_size;

	mWidth = ((destAddress & 0x3) == 0) ? AT91C_HDMA_DST_WIDTH_WORD : AT91C_HDMA_DST_WIDTH_BYTE; // Memory width can be WORD if address is aligned
	perWidth = forceByte ? AT91C_HDMA_SRC_WIDTH_BYTE : AT91C_HDMA_SRC_WIDTH_WORD; // Peripheral width is byte if FBYTE mode
	addrInc  = forceByte ? 1 : 4;

	if(buffSize >= DMA_TOTAL)
	{
		DEBUG_MSG("SD DMA, size too big %d", buffSize);
		buffSize = DMA_TOTAL;
	}

	DMA_SetSourceAddr(channel_index, srcAddress); // Set DMA channel source address
	DMA_SetDestinationAddr(channel_index,destAddress); // Set DMA channel destination address
	DMA_SetDescriptorAddr(channel_index, (U32)&LLI_MCI[0]); // Set DMA channel DSCR
	DMA_SetSourceBufferSize(channel_index, buffSize, (perWidth >> 24), (mWidth >> 28), 0);
	DMA_SetSourceBufferMode(channel_index, DMA_TRANSFER_LLI, srcAddressMode >> 24); 
	DMA_SetDestBufferMode(channel_index, DMA_TRANSFER_LLI, (AT91C_HDMA_DST_ADDRESS_MODE_INCR >> 28));
	DMA_SetConfig(channel_index, BOARD_SD_DMA_HW_SRC_REQ_ID | BOARD_SD_DMA_HW_DEST_REQ_ID | AT91C_HDMA_SRC_H2SEL_HW | AT91C_HDMA_DST_H2SEL_HW | AT91C_HDMA_SOD_DISABLE | AT91C_HDMA_FIFOCFG_ENOUGHSPACE); // Set DMA channel config
	scSize = AT91C_HDMA_SCSIZE_1;
	dcSize = AT91C_HDMA_DCSIZE_4;

	// Set link list
	buffSize *= addrInc; // convert size to byte count

	while(destAddress < ((U32)(dest_addr + buffSize)))
	{
		if(((U32)(dest_addr + buffSize)) - destAddress <= (DMA_XFR_SIZE*addrInc))
		{
			AT91F_Prepare_Multiple_Transfer(channel_index, LLI_rownumber, LAST_ROW, srcAddress, destAddress, (((((U32)(dest_addr + buffSize)) - destAddress)/addrInc) | perWidth | mWidth | scSize | dcSize), (AT91C_HDMA_DST_DSCR_FETCH_FROM_MEM | AT91C_HDMA_DST_ADDRESS_MODE_INCR | AT91C_HDMA_SRC_DSCR_FETCH_DISABLE | srcAddressMode | AT91C_HDMA_FC_PER2MEM));
		}
		else
		{
			AT91F_Prepare_Multiple_Transfer(channel_index, LLI_rownumber, 0, srcAddress, destAddress, ((DMA_XFR_SIZE) | perWidth | mWidth | scSize | dcSize), (AT91C_HDMA_DST_DSCR_FETCH_FROM_MEM | AT91C_HDMA_DST_ADDRESS_MODE_INCR | AT91C_HDMA_SRC_DSCR_FETCH_DISABLE | srcAddressMode | AT91C_HDMA_FC_PER2MEM));
		}
		destAddress += DMA_XFR_SIZE*addrInc;
		LLI_rownumber++;
	}
	return 0;
}

// trans_size: number of transfers in SRC side
// forceByte: 1 - byte count, 0 - word count dest_addr = HSMCI_FIFO(ch)
U32 DMACH_MCI_M2P(U32 channel_index, U8* src_addr, U32 trans_size, U8 addrIncMode, U8  forceByte)
{
	U32 srcAddress;
	U32 destAddress;
	U32 buffSize;
	U32 LLI_rownumber = 0;
	U32 dstAddressMode = addrIncMode ? (AT91C_HDMA_DST_ADDRESS_MODE_INCR) : (AT91C_HDMA_DST_ADDRESS_MODE_FIXED);
	U32 dcSize, scSize, mWidth, perWidth, addrInc;

	// Disable dma channel
	DMA_DisableChannel(channel_index);

	buffSize = trans_size;
	if(buffSize >= DMA_TOTAL)
	{
		DEBUG_MSG("SD DMA, size too big %d", buffSize);
		buffSize = DMA_TOTAL;
	}

	// DMA channel configuration
	srcAddress = (U32)src_addr; // Set the data start address
	//destAddress = (U32)dest_addr;
	destAddress = (U32)(HSMCI_FIFO(0));
	mWidth = ((srcAddress & 0x3) == 0) ? AT91C_HDMA_SRC_WIDTH_WORD : AT91C_HDMA_SRC_WIDTH_BYTE; // Memory width
	perWidth = forceByte ? AT91C_HDMA_DST_WIDTH_BYTE : AT91C_HDMA_DST_WIDTH_WORD; // One Transfer size (1 or 4)

	if (mWidth == AT91C_HDMA_SRC_WIDTH_BYTE) //addrInc = forceByte ? 1 : 4;
	{
		addrInc = 1;
		if(!forceByte) buffSize *= 4;
	}
	else addrInc = 4;

	DMA_SetSourceAddr(channel_index, srcAddress); // Set DMA channel source address
	DMA_SetDestinationAddr(channel_index,destAddress); // Set DMA channel destination address
	DMA_SetDescriptorAddr(channel_index, (U32)&LLI_MCI[0]); // Set DMA channel DSCR
	DMA_SetSourceBufferSize(channel_index, buffSize, (mWidth >> 24), (perWidth >> 28), 0); // Set DMA channel control A 
	DMA_SetSourceBufferMode(channel_index, DMA_TRANSFER_LLI, (AT91C_HDMA_SRC_ADDRESS_MODE_INCR >> 24)); //Set DMA channel control B
	DMA_SetDestBufferMode(channel_index, DMA_TRANSFER_LLI, dstAddressMode >> 28);
	DMA_SetConfig(channel_index, BOARD_SD_DMA_HW_SRC_REQ_ID | BOARD_SD_DMA_HW_DEST_REQ_ID | AT91C_HDMA_SRC_H2SEL_SW | AT91C_HDMA_DST_H2SEL_HW | AT91C_HDMA_SOD_DISABLE | AT91C_HDMA_FIFOCFG_LARGESTBURST); // Set DMA channel config

	dcSize = AT91C_HDMA_DCSIZE_1;
	scSize = AT91C_HDMA_SCSIZE_4;

	// Set link list
	buffSize *= addrInc; // convert to byte address
	while(srcAddress < ((U32)(src_addr + buffSize)))
	{
		if(((U32)(src_addr + buffSize)) - srcAddress <= (DMA_XFR_SIZE*addrInc))
		{
			AT91F_Prepare_Multiple_Transfer(channel_index, LLI_rownumber, LAST_ROW, srcAddress, destAddress, (((((U32)(src_addr + buffSize)) - srcAddress)/addrInc) | mWidth | perWidth | scSize | dcSize ), (AT91C_HDMA_DST_DSCR_FETCH_DISABLE | dstAddressMode | AT91C_HDMA_SRC_DSCR_FETCH_FROM_MEM | AT91C_HDMA_SRC_ADDRESS_MODE_INCR | AT91C_HDMA_FC_MEM2PER));
		}
		else
		{
			AT91F_Prepare_Multiple_Transfer(channel_index, LLI_rownumber, 0, srcAddress, destAddress, ((DMA_XFR_SIZE) | mWidth | perWidth | scSize | dcSize), (AT91C_HDMA_DST_DSCR_FETCH_DISABLE | dstAddressMode | AT91C_HDMA_SRC_DSCR_FETCH_FROM_MEM | AT91C_HDMA_SRC_ADDRESS_MODE_INCR | AT91C_HDMA_FC_MEM2PER));
		}
		srcAddress += DMA_XFR_SIZE*addrInc;
		LLI_rownumber++;
	}
	return 0;
}

void DMACH_EnableIt(U32 ch)
{
    U32 intFlag;

    intFlag = DMA_GetInterruptMask();
    intFlag |= (0x1 << ch);
    DMA_EnableIt(intFlag);
}


//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Reset MCI interface and disable it.
/// \param keepSettings Keep old register settings, including _MR, _SDCR, _DTOR
//------------------------------------------------------------------------------
void MCI_Reset(U32 keepSet)
{
	U32 mciMr, mciSdcr, mciDtor, mciCstor;
	U32 mciDma, mciCfg;

	if(keepSet)
	{
		mciMr				= rHSMCI_MR(0);
		mciSdcr			= rHSMCI_SDCR(0);
		mciDtor			= rHSMCI_DTOR(0);
		mciCstor		= rHSMCI_CSTOR(0);
		mciDma			= rHSMCI_DMA(0);
		mciCfg			= rHSMCI_CFG(0);

		rHSMCI_CR(0)	= AT91C_MCI_SWRST;
		rHSMCI_CR(0)	= AT91C_MCI_MCIDIS | AT91C_MCI_PWSDIS;

		rHSMCI_MR(0)	= mciMr;
		rHSMCI_SDCR(0)	= mciSdcr;
		rHSMCI_DTOR(0)	= mciDtor;
		rHSMCI_CSTOR(0)	= mciCstor;
		rHSMCI_DMA(0)	= mciDma;
		rHSMCI_CFG(0)	= mciCfg;
	}
	else
	{
		rHSMCI_CR(0)	= AT91C_MCI_SWRST;
		rHSMCI_CR(0)	= AT91C_MCI_MCIDIS | AT91C_MCI_PWSDIS;
	}
}

//------------------------------------------------------------------------------
/// 
/// 
//------------------------------------------------------------------------------
void MCI_Init(void)
{
	U16 clkDiv;
	U32 mciCfg = 0;

	// Initialize the MCI driver structure
	mciDrv.mciMode = (AT91C_MCI_SCDSEL_SLOTA | AT91C_MCI_SCDBUS_4BITS);
	mciDrv.bPolling = MCI_POLLING_MODE;
	mciDrv.semaphore = 1;

	rPIOA_PDR = 0x3F;
	rPIOA_PUDR = 0x1;
	rPIOA_PUER = 0x1E;
	rPIOA_ASR	= 0x3F;
	
	PMC_PeriEn(11); // Enable the MCI clock
	MCI_Reset(0); // Reset the MCI

	rHSMCI_IDR(0) = 0xFFFFFFFF; // Disable all the interrupts
	rHSMCI_DTOR(0) = DTOR_1MEGA_CYCLES; // Set the Data Timeout Register

	clkDiv = (BOARD_MCK / (MCI_INITIAL_SPEED * 2)) - 1; // Set the Mode Register: 400KHz for MCK = 48MHz (CLKDIV = 58)
	rHSMCI_MR(0) = (clkDiv | (AT91C_MCI_PWSDIV & (0x7 << 8)));
	rHSMCI_SDCR(0) = (AT91C_MCI_SCDSEL_SLOTA | AT91C_MCI_SCDBUS_4BITS); // Set the SDCard Register
	rHSMCI_CR(0) = AT91C_MCI_MCIEN; // Enable the MCI and the Power Saving
	rHSMCI_DMA(0) = AT91C_MCI_DMAEN_DISABLE; // Disable the DMA interface
	mciCfg = AT91C_MCI_FIFOMODE_AMOUNTDATA | AT91C_MCI_FERRCTRL_RWCMD; // Configure MCI
	rHSMCI_CFG(0) = mciCfg;
	PMC_PeriDis(11); // Disable the MCI peripheral clock.
}

//------------------------------------------------------------------------------
/// Enable/disable a MCI driver instance.
/// \param pMci  Pointer to a MCI driver instance.
/// \param enb  0 for disable MCI and 1 for enable MCI.
//------------------------------------------------------------------------------
void MCI_Enable(U8 enb)
{
	if(enb == DISABLE) // Set the Control Register: Enable/Disable MCI interface clock
	{
		rHSMCI_CR(0) = AT91C_MCI_MCIDIS;
	}
	else
	{
		rHSMCI_CR(0) = AT91C_MCI_MCIEN;
	}
}

//------------------------------------------------------------------------------
/// Close a MCI driver instance and the underlying peripheral.
/// \param pMci  Pointer to a MCI driver instance.
/// \param pMciHw  Pointer to a MCI peripheral.
//------------------------------------------------------------------------------
void MCI_Close(void)
{
	rHSMCI_IDR(0) = 0xFFFFFFFF; // Disable all the interrupts
	rHSMCI_CR(0) = AT91C_MCI_MCIDIS; // Disable the MCI
	PMC_PeriDis(11); // Disable the MCI peripheral clock.
	mciDrv.semaphore = 1; // Initialize the MCI driver structure
}

//------------------------------------------------------------------------------
/// Get the  MCI CLKDIV in the MCI_MR register. The max. for MCI clock is
/// MCK/2 and corresponds to CLKDIV = 0
/// \param pMci  Pointer to the low level MCI driver.
/// \param mciSpeed  MCI clock speed in Hz.
//------------------------------------------------------------------------------
U32 MCI_GetSpeed(U32 *mciDiv)
{
	U32 mciMr;
	mciMr = rHSMCI_MR(0); // Get the Mode Register
	mciMr &= AT91C_MCI_CLKDIV;
	if(mciDiv) *mciDiv = mciMr;
	return (BOARD_MCK / 2 / (mciMr + 1));
}

//------------------------------------------------------------------------------
/// Configure the  MCI CLKDIV in the MCI_MR register. The max. for MCI clock is
/// MCK/2 and corresponds to CLKDIV = 0
/// \param pMci  Pointer to the low level MCI driver.
/// \param mciSpeed  MCI clock speed in Hz.
/// \param mciLimit  MCI clock limit in Hz, if not limit, set mciLimit to zero.
/// \param mck       MCK to generate MCI Clock, in Hz
/// \return The actual speed used, 0 for fail.
//------------------------------------------------------------------------------
U32 MCI_SetSpeed(U32 mciSpeed, U32 mciLimit, U32 mck)
{
	U32 mciMr;
	U32 clkdiv;
	U32 divLimit = 0;

	mciMr = rHSMCI_MR(0) & (~(U32)AT91C_MCI_CLKDIV);

	// Multimedia Card Interface clock (MCCK or MCI_CK) is Master Clock (MCK)
	// divided by (2*(CLKDIV+1))
	// mciSpeed = MCK / (2*(CLKDIV+1))
	if(mciLimit)
	{
		divLimit = (mck / 2 / mciLimit);
		if((mck / 2) % mciLimit) divLimit ++;
	}
	if(mciSpeed > 0)
	{
		clkdiv = (mck / 2 / mciSpeed);
		if (mciLimit && clkdiv < divLimit) clkdiv = divLimit;
		if (clkdiv > 0) clkdiv -= 1;
	}
	else clkdiv = 0;
	
	mciSpeed = mck / 2 / (clkdiv + 1); // Actual MCI speed

	// Set the Data Timeout Register & Completion Timeout
	// Data timeout is 500ms, completion timeout 1s.
	// MCI_SetTimeout(pMciHw, mciSpeed / 2, mciSpeed);

	rHSMCI_MR(0) = (mciMr | clkdiv);

	return (mciSpeed);
}

//------------------------------------------------------------------------------
/// Configure the MCI_CFG to enable the HS mode
/// \param pMci     Pointer to the low level MCI driver.
/// \param hsEnable 1 to enable, 0 to disable HS mode.
//------------------------------------------------------------------------------
void MCI_EnableHsMode(U8 hsEnable)
{
	U32 value;
	value = rHSMCI_CFG(0);
	if (hsEnable) value |=  (0x1 << 8);
	else value &= ~(U32)(0x1 << 8);
	rHSMCI_CFG(0) = value;
}

//------------------------------------------------------------------------------
/// Configure the  MCI SDCBUS in the MCI_SDCR register. Only two modes available
///
/// \param pMci  Pointer to the low level MCI driver.
/// \param busWidth  MCI bus width mode.
//------------------------------------------------------------------------------
void MCI_SetBusWidth(U8 busWidth)
{
	U32 value;
	value = (rHSMCI_SDCR(0) & ~((U32)(0x3 << 6)));
	rHSMCI_SDCR(0) = (value | busWidth);
}

//------------------------------------------------------------------------------
/// Starts a MCI  transfer. This is a non blocking function. It will return
/// as soon as the transfer is started.
/// Return 0 if successful; otherwise returns MCI_ERROR_LOCK if the driver is
/// already in use.
/// \param pMci  Pointer to an MCI driver instance.
//------------------------------------------------------------------------------
U8 MCI_SendCommand(void)
{
	U32 mciIer, mciMr;
	U32 transSize;
	U32 mciBlkr;
	U32 mciDma;
	
	// Try to acquire the MCI semaphore
	if (mciDrv.semaphore == 0)
	{
			return MCI_ERROR_LOCK;
	}
	mciDrv.semaphore--;

	// Command is now being executed
	sdCmd.status = MCI_STATUS_PENDING;

	// Enable the MCI peripheral clock
	PMC_PeriEn(11);

	// Set Default Mode register value
	mciMr = rHSMCI_MR(0) & (~(AT91C_MCI_WRPROOF | AT91C_MCI_RDPROOF | AT91C_MCI_BLKLEN));
										  
	// Command with DATA stage
	if(sdCmd.blockSize && sdCmd.nbBlock)
	{
		mciDma = rHSMCI_DMA(0) | AT91C_MCI_DMAEN_ENABLE;
		rHSMCI_DMA(0) = mciDma;
		DEBUG_MSG("MCI_SendCommand() data mciDma: 0x%08X", mciDma);
		
		transSize = (sdCmd.nbBlock * sdCmd.blockSize);

		if(sdCmd.isRead)
		{
			if (transSize > MCI_FIFO_SIZE)
			{
				xfredBlocks = MCI_FIFO_SIZE/sdCmd.blockSize;
				transSize = MCI_FIFO_SIZE;
			}
			else
			{
				xfredBlocks = sdCmd.nbBlock;
			}
		}
		else
		{
			xfredBlocks = sdCmd.nbBlock;
		}

		if((sdCmd.blockSize & 0x3) != 0) // Force byte, DataReg & DMA should be BYTE based
		{
			mciMr |= AT91C_MCI_PDCFBYTE;
		}
		else transSize = toWCOUNT(transSize);

		// New transfer
		if(sdCmd.tranType == MCI_NEW_TRANSFER)
		{
			// Set block size
			rHSMCI_MR(0) = (mciMr | AT91C_MCI_RDPROOF | AT91C_MCI_WRPROOF | (sdCmd.blockSize << 16));
			mciBlkr = (rHSMCI_BLKR(0) & (~(unsigned int)AT91C_MCI_BCNT));
			rHSMCI_BLKR(0) = (mciBlkr | (transSize/(sdCmd.blockSize)));
		}

		// DATA transfer from card to host
		if(sdCmd.isRead)
		{
			DMACH_MCI_P2M(BOARD_MCI_DMA_CHANNEL, (U8*)sdCmd.pData, transSize, 0, (mciMr & AT91C_MCI_PDCFBYTE) > 0);
			DMACH_EnableIt(BOARD_MCI_DMA_CHANNEL);
			DMA_EnableChannel(BOARD_MCI_DMA_CHANNEL);
			mciIer = AT91C_MCI_DMADONE | STATUS_ERRORS;
		}
		else // DATA transfer from host to card
		{
			if((mciMr & AT91C_MCI_PDCFBYTE) > 0)
			{
				DMACH_MCI_M2P(BOARD_MCI_DMA_CHANNEL, (U8*)sdCmd.pData, toWCOUNT(transSize), 0, 0); // Still using WORD mode to write FIFO
			}
			else
			{
				DMACH_MCI_M2P(BOARD_MCI_DMA_CHANNEL, (U8*)sdCmd.pData, transSize, 0, 0);
			}
			DMACH_EnableIt(BOARD_MCI_DMA_CHANNEL);
			DMA_EnableChannel(BOARD_MCI_DMA_CHANNEL);
			mciIer = AT91C_MCI_DMADONE | STATUS_ERRORS;
		}
	}
	else if(sdCmd.dataTran) // Start an infinite block transfer (but no data in current command)
	{
		rHSMCI_MR(0) = mciMr | AT91C_MCI_RDPROOF | AT91C_MCI_WRPROOF |(sdCmd.blockSize << 16); // Set block size
		mciBlkr = rHSMCI_BLKR(0) & (~(unsigned int)AT91C_MCI_BCNT); // Set data length: 0
		rHSMCI_BLKR(0) = mciBlkr;
		mciIer = AT91C_MCI_CMDRDY | STATUS_ERRORS;
	}
	else // No data transfer: stop at the end of the command
	{
		rHSMCI_MR(0) = mciMr;
		mciIer = AT91C_MCI_CMDRDY | STATUS_ERRORS;
	}
    
	rHSMCI_CR(0) = AT91C_MCI_MCIEN;

	if((sdCmd.tranType != MCI_CONTINUE_TRANSFER) || (sdCmd.blockSize == 0)) // Send the command
	{
		rHSMCI_ARGR(0) = sdCmd.arg;
		rHSMCI_CMDR(0) = sdCmd.cmd;
	}

	if (sdCmd.resType == 3 || sdCmd.resType == 4) // Ignore CRC error for R3 & R4
	{
		mciIer &= ~((unsigned int)AT91C_MCI_RCRCE);
	}
    
	if (sdCmd.tranType == MCI_STOP_TRANSFER) // Ignore errors for stop command :)
	{
		mciIer &= ~((U32)(AT91C_MCI_DCRCE |AT91C_MCI_BLKOVRE |AT91C_MCI_DTOE |AT91C_MCI_CSTOE));
	}
	
	mciIer &= ~(0 | AT91C_MCI_UNRE | AT91C_MCI_OVRE | AT91C_MCI_DTOE | AT91C_MCI_DCRCE | AT91C_MCI_BLKOVRE | AT91C_MCI_CSTOE ); // Ignore data error
	rHSMCI_IER(0) = mciIer; // Interrupt enable shall be done after PDC TXTEN and RXTEN

	return 0;
}

//------------------------------------------------------------------------------
/// Check NOTBUSY and DTIP bits of status register on the given MCI driver.
/// Return value, 0 for bus ready, 1 for bus busy
/// \param pMci  Pointer to a MCI driver instance.
//------------------------------------------------------------------------------
U8 MCI_CheckBusy(void)
{
	volatile U32 status;

	PMC_PeriEn(11); // Enable MCI clock
	rHSMCI_CR(0) = AT91C_MCI_MCIEN;

	status = rHSMCI_SR(0);

	if(((status & AT91C_MCI_NOTBUSY)!=0) && ((status & AT91C_MCI_DTIP)==0))
	{
		rHSMCI_CR(0) = AT91C_MCI_MCIDIS;
		PMC_PeriDis(11); // Disable MCI clock
		return 0;
	}
	else
	{
		return 1;
	}
}

//------------------------------------------------------------------------------
/// Processes pending events on the given MCI driver.
/// \param pMci  Pointer to a MCI driver instance.
//------------------------------------------------------------------------------
void MCI_Handler(void)
{
	volatile U32 status;
	U32 status0, mask;
	U8 i;
	
	U32 intFlag;
	U32 transSize;
	U8* p;
	U8 resSize;

	status0 = rHSMCI_SR(0); // Read the status register
	mask = rHSMCI_IMR(0);
	status = status0 & mask;

	if((status & STATUS_ERRORS) != 0) // Check if an error has occured
	{
		if((status & STATUS_ERRORS) == AT91C_MCI_RTOE) // Check error code
		{
			sdCmd.status = MCI_STATUS_NORESPONSE;
		}
		else
		{
			sdCmd.status = MCI_STATUS_ERROR;
		}
		DEBUG_MSG("MCI_Handler Err:0x%08X", (status & STATUS_ERRORS));
	}
	mask &= ~STATUS_ERRORS;

	if(status & AT91C_MCI_CMDRDY) // Check if a command has been completed
	{
		rHSMCI_IDR(0) = AT91C_MCI_CMDRDY;
		if(sdCmd.isRead == 0 && sdCmd.tranType == MCI_STOP_TRANSFER)
		{
			if(status0 & AT91C_MCI_XFRDONE)
			{
				rHSMCI_CR(0) = AT91C_MCI_MCIDIS;
			}
			else
			{
				rHSMCI_IER(0) = AT91C_MCI_XFRDONE;
			}
		}
		else
		{
			mask &= ~(U32)AT91C_MCI_CMDRDY;
			if(sdCmd.dataTran == 0)
			{
				rHSMCI_CR(0) = AT91C_MCI_MCIDIS;
			}
		}
	}

	if(status & AT91C_MCI_XFRDONE) // Check if transfer stopped
	{
		mask &= ~(U32)AT91C_MCI_XFRDONE;
		rHSMCI_CR(0) = AT91C_MCI_MCIDIS;
	}

	if(status & AT91C_MCI_FIFOEMPTY) // Check FIFOEMPTY
	{
		rHSMCI_IDR(0) = AT91C_MCI_FIFOEMPTY;
		if(sdCmd.isRead == 0 && (status0 & AT91C_MCI_BLKE) == 0)
		{
			rHSMCI_IER(0) = AT91C_MCI_BLKE;
		}
		else
		{
			mask &= ~(U32)AT91C_MCI_FIFOEMPTY;
			rHSMCI_CR(0) = AT91C_MCI_MCIDIS;
		}
	}
	if(status & AT91C_MCI_BLKE)
	{
		mask &= ~(U32)AT91C_MCI_BLKE;
		rHSMCI_CR(0) = AT91C_MCI_MCIDIS;
	}

	if((status & AT91C_MCI_DMADONE) && (LLI_MCI[dmaLastLliNdx].controlA & AT91C_HDMA_DONE)) // Check if a DMA transfer has been completed
	{
		intFlag = DMA_GetInterruptMask();
		intFlag = ~intFlag;
		intFlag |= (AT91C_HDMA_BTC0 << BOARD_MCI_DMA_CHANNEL);
		DMA_DisableIt(intFlag);

		if(xfredBlocks >= sdCmd.nbBlock) // All data transferred
		{
			rHSMCI_IDR(0) = AT91C_MCI_DMADONE;
			if(sdCmd.isRead == 0 && (status0 & AT91C_MCI_FIFOEMPTY) == 0)
			{
				rHSMCI_IER(0) = AT91C_MCI_FIFOEMPTY;
			}
			else
			{
				rHSMCI_CR(0) = AT91C_MCI_MCIDIS;
				mask &= ~(U32)AT91C_MCI_DMADONE;
			}
		}
		else // Start later part of DMA
		{
			p = &sdCmd.pData[xfredBlocks*sdCmd.blockSize];
			transSize = ((sdCmd.nbBlock - xfredBlocks) * sdCmd.blockSize);
			if(transSize > MCI_FIFO_SIZE)
			{
				transSize = MCI_FIFO_SIZE;
				xfredBlocks += MCI_FIFO_SIZE/sdCmd.blockSize;
			}
			else
			{
				xfredBlocks  = sdCmd.nbBlock;
			}

			if((sdCmd.blockSize & 0x3) == 0)
			{
				transSize = toWCOUNT(transSize);
			}
			DMACH_MCI_P2M(BOARD_MCI_DMA_CHANNEL, (U8*) p, transSize, 0, (sdCmd.blockSize & 0x3) > 0);
			DMA_EnableChannel(BOARD_MCI_DMA_CHANNEL);
		}
	}

	if(!mask || sdCmd.status != MCI_STATUS_PENDING) // All non-error mask done, complete the command
	{
		if (sdCmd.pResp) // Store the card response in the provided buffer
		{
			switch (sdCmd.resType)
			{
				case 1:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7: resSize = 1; break;
				case 2: resSize = 4; break;
				default: resSize = 0; break;
			}
			for (i=0; i < resSize; i++)
			{
				sdCmd.pResp[i] = rHSMCI_RSPR0(0);
			}
		}
		
		if(sdCmd.status == MCI_STATUS_PENDING) // If no error occured, the transfer is successful
		{
			sdCmd.status = 0;
		}
		else // Any error, reset registers
		{
			MCI_Reset(1);
		}

		rHSMCI_IDR(0) = rHSMCI_IMR(0);
		DMA_DisableChannel(BOARD_MCI_DMA_CHANNEL);
		PMC_PeriDis(11); // Disable peripheral
		mciDrv.semaphore++; // Release the semaphore
		if(sdCmd.callback) // Invoke the callback associated with the current command (if any)
		{
			(sdCmd.callback)(sdCmd.status, (void*)&sdCmd);
		}
	}
}

//------------------------------------------------------------------------------
/// Returns 1 if the given MCI transfer is complete; otherwise returns 0.
/// param: pCommand  Pointer to a MciCmd instance.
//------------------------------------------------------------------------------
U8 MCI_IsTxComplete(void)
{
//	MciCmd *pCommand = mciDrv.pCommand;		

	if(mciDrv.bPolling == MCI_POLLING_MODE)
	{
		MCI_Handler();
	}
	
	if(sdCmd.status != MCI_STATUS_PENDING)
	{
		if(sdCmd.status != 0)
		{
			DEBUG_MSG("MCI_IsTxComplete: 0x%X", sdCmd.status);
		}
		return 1;
	}
	else
	{
		return 0;
	}
}
