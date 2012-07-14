/* ----------------------------------------------------------------------------
 *		 
 *        mboot for Lophilo embedded system
 *		
 * ----------------------------------------------------------------------------
 */

#include "types.h"
#include "trace.h"

#include "sfr.h"
#include "dma.h"
#include "board.h"
#include "system.h"

/// Global DMA transfer instance.
static Dmad dmad;

//------------------------------------------------------------------------------
/// Delay some loop
//------------------------------------------------------------------------------
// void Delay_m(volatile U32 loop)
// {
// 	for(;loop > 0; loop --);
// }

void Delay_10us(U8 delay)
{
	U16 clock_count = (U16)(BOARD_MCK / 100000 / 8);

	rTC0_CMR(0) = 0x1; // Set clock frequency to BOARD_MCK / 8.
	rTC0_CCR(0) = (0x1 << 0); // Enable TC clock.
	rTC0_CCR(0) = (0x1 << 2); // Reset clock counter and start TC clock.
	while(rTC0_CV(0) < clock_count * (U16)delay);
	rTC0_CCR(0) = (0x1 << 1); // Disable TC clock.
}

void Delay_ms(U16 delay)
{
	rTC0_CMR(0) = 0x3;// Set clock frequency to BOARD_MCK / 128.
	rTC0_CCR(0) = (0x1 << 0); // Enable TC clock.
	do
	{
		rTC0_CCR(0) = (0x1 << 2); // Reset clock counter and start TC clock.
		while (rTC0_CV(0) >= BOARD_MCK / 1000 / 128); // Wait until the counter has been reset.
		while (rTC0_CV(0) < BOARD_MCK / 1000 / 128); // Wait until the counter has reached a value equivalent to 1 ms delay.
	} 
	while (--delay);
	rTC0_CCR(0) = (0x1 << 1); //  Disable TC clock.
}


//------------------------------------------------------------------------------
/// Enables the clock of a peripheral. The peripheral ID (AT91C_ID_xxx) is used
/// to identify which peripheral is targetted.
/// Note that the ID must NOT be shifted (i.e. 1 << AT91C_ID_xxx).
/// \param id  Peripheral ID (AT91C_ID_xxx).
//------------------------------------------------------------------------------
void PMC_PeriEn(U32 id)
{
	if(id > 31) return;

	if(!((rPMC_PCSR >> id) & 0x1))
	{
		rPMC_PCER = (0x1 << id);
	}
}

//------------------------------------------------------------------------------
/// Disables the clock of a peripheral. The peripheral ID (AT91C_ID_xxx) is used
/// to identify which peripheral is targetted.
/// Note that the ID must NOT be shifted (i.e. 1 << AT91C_ID_xxx).
/// \param id  Peripheral ID (AT91C_ID_xxx).
//------------------------------------------------------------------------------
void PMC_PeriDis(U32 id)
{
	if(id > 31) return;
	
	if((rPMC_PCSR >> id) & 0x1)
	{
		rPMC_PCDR = (0x1 << id);
	}
}

//------------------------------------------------------------------------------
/// Configures an interrupt in the AIC. The interrupt is identified by its
/// source (AT91C_ID_xxx) and is configured to use the specified mode and
/// interrupt handler function. Mode is the value that will be put in AIC_SMRx
/// and the function address will be set in AIC_SVRx.
/// The interrupt is disabled before configuration, so it is useless
/// to do it before calling this function. When AIC_ConfigureIT returns, the
/// interrupt will always be disabled and cleared; it must be enabled by a
/// call to AIC_EnableIT().
/// \param source  Interrupt source to configure.
/// \param mode  Triggering mode and priority of the interrupt.
/// \param handler  Interrupt handler function.
//------------------------------------------------------------------------------
void IRQ_PeriConf(U32 id, U32 mode, void(*handler)( void ))
{
	rAIC_IDCR = (0x1 << id);
	rAIC_SMR(id) = mode;
	rAIC_SVR(id) = (U32) handler;
	rAIC_ICCR = (0x1 << id);
}

//------------------------------------------------------------------------------
/// Enables interrupts coming from the given (unique) source (AT91C_ID_xxx).
/// \param source  Interrupt source to enable.
//------------------------------------------------------------------------------
void IRQ_PeriEn(U32 id)
{
	rAIC_IECR = (0x1 << id);
}

//------------------------------------------------------------------------------
/// Disables interrupts coming from the given (unique) source (AT91C_ID_xxx).
/// \param source  Interrupt source to enable.
//------------------------------------------------------------------------------
void IRQ_PeriDis(U32 id)
{
	rAIC_IDCR = (0x1 << id);
}

//------------------------------------------------------------------------------
/// This handler function must be called by the DMAC interrupt service routine.
/// Identifies which event was activated and calls the associated function.
//------------------------------------------------------------------------------ 
void DMAD_Handler()
{
	U32 status;
	U8 channel;
	DmaTransfer *pTransfer;

	status = DMA_GetStatus();
	if(status & (0xFF)) // Check if the buffer transfer completed is set.
	{
		for(channel = 0; channel < 8; channel++) // Scan each channel status.
		{
			if(!(status & (0x1 << channel)))
			{
				continue;
			}

			dmad.transfers[channel].transferSize -= dmad.transfers[channel].bufSize;
			if(dmad.transfers[channel].transferSize <= dmad.transfers[channel].bufSize) // if next buffer is to be the last buffer in the transfer, then clear the automatic mode bit.
			{
				DMA_ClearAutoMode(channel);
			}

			if(dmad.transfers[channel].transferSize == 0) // Transfer finished
			{
				pTransfer = &(dmad.transfers[channel]);
				pTransfer->callback();
				DMA_DisableIt(0x1 << channel); 
				DMA_DisableChannel(channel);
			}
			else
			{
				DMA_KeeponChannel(channel); // Write the KEEPON field to clear the STALL states.
			}
		}
	}
}

//------------------------------------------------------------------------------
/// Initializes the DMA controller.
/// \param channel Particular channel number
/// \param defaultHandler Using the default dmad interrupt handler.
//------------------------------------------------------------------------------
void DMAD_Initialize(U8 channel, U8 defaultHandler)
{
//	U32 status;
	U32 flag;
	
//	status = DMA_GetChannelStatus(); // Read the channel handler status to ensure the channel is a free channel.
//	DEBUG_MSG("DMAD_Initialize channel: %x, Status: %x", channel, status);
	DMA_GetStatus(); // Clear any pending interrupts on the channel.
	DMA_DisableChannel(channel); // Disble the channel.
	flag = 0xffffff;
	DMA_DisableIt(flag); // Disable the interrupt
	DMA_Enable(); // Enable DMA.
	if(defaultHandler) 
	{
		IRQ_PeriConf(21, 0, DMAD_Handler);
		IRQ_PeriEn(21);
	}
	dmad.transfers[channel].transferSize = 0; // Initialize transfer instance.
}
 
//------------------------------------------------------------------------------
/// Configure the DMA transfer buffer by giving transfer mode, it could be single 
/// buffer or multi-buffer(LLI/auto-reload/contiguous buffers) with or without 
/// Picture-In-Picture mode.
/// \param channel Particular channel number.
/// \param sourceTransferMode Source buffer transfer mode.
/// \param destTransferMode Destination buffer transfer mode.
/// \param lli Pointer to a DmaLinkList structure instance.
/// \param pip Pointer to a PictureInPicture structure.
//------------------------------------------------------------------------------
U8 DMAD_Configure_Buffer(U8 channel, U8 sourceTransferMode, U8 destTransferMode, DmaLinkList *lli, PictureInPicture *pip)
{
	DmaTransfer *pTransfer = &(dmad.transfers[channel]);

	if(pTransfer-> transferSize > 0) // Check that no transfer is pending on the channel
	{
		DEBUG_MSG("DAM transfer is already pending");
		return DMAD_ERROR_BUSY;
	}
	
	DMA_SetSourceBufferMode(channel, sourceTransferMode, 0); // Configure source transfer mode.
	DMA_SetDestBufferMode(channel, destTransferMode, 0); // Configure destination transfer mode.

	if(lli)
	{
		DMA_SetDescriptorAddr(channel, (U32)(&lli[0]));
	}
	else
	{
		DMA_SetDescriptorAddr(channel, 0);
	}
	
	if(pip)
	{
		if(pip->pipSourceBoundarySize)// If source picture-in-picture mode is enabled, program the DMAC_SPIP.
		{
			DMA_SPIPconfig(channel, pip->pipSourceHoleSize, pip->pipSourceBoundarySize); // If destination picture-in-picture mode is enabled, program the DMAC_DPIP.
		}
		if(pip->pipDestBoundarySize)
		{
			DMA_DPIPconfig(channel, pip->pipDestHoleSize, pip->pipDestBoundarySize);
		}
	}
	return 0;
}

//------------------------------------------------------------------------------
/// Configure the DMA transfer control infomation.
/// \param channel Particular channel number.
/// \param bufSize Buffer transfer size in byte.
/// \param sourceWidth Source transfer width.
/// \param destWidth Destination transfer width.
/// \param sourceAddress Destination transfer width.
/// \param destAddress Destination transfer width.
//------------------------------------------------------------------------------
U8 DMAD_Configure_TransferController(U8 channel, U32 bufSize, U8 sourceWidth, U8 destWidth, U32 sourceAddress, U32 destAddress)
{
	DmaTransfer *pTransfer = &(dmad.transfers[channel]);
	
	if(pTransfer-> transferSize > 0) // Check that no transfer is pending on the channel
	{
		DEBUG_MSG("DAM transfer is already pending");
		return DMAD_ERROR_BUSY;
	}
	pTransfer->bufSize = bufSize; 
	DMA_SetSourceBufferSize(channel, bufSize, sourceWidth, destWidth, 0); // Set up the transfer width and transfer size.
	if(sourceAddress)
	{
		DMA_SetSourceAddr(channel, sourceAddress); // Write the starting source address.
	}
	if(destAddress)
	{
		DMA_SetDestinationAddr(channel, destAddress); // Write the starting destination address.
	}
	return 0;
}                                 
                           
//------------------------------------------------------------------------------
/// Starts buffer transfer on the given channel
/// \param channel Particular channel number.
/// \param size Total transfer size in byte.
/// \param callback Optional callback function.
/// \param polling Polling channel status enable.
//------------------------------------------------------------------------------
U8 DMAD_BufferTransfer(U8 channel, U32 size, DmaCallback callback, U8 polling)
{
	DmaTransfer *pTransfer = &(dmad.transfers[channel]);
	
	if (pTransfer-> transferSize > 0 ) // Check that no transfer is pending on the channel
	{
		DEBUG_MSG("DAM transfer is already pending");
		return DMAD_ERROR_BUSY;
	}
	pTransfer->status = DMAD_ERROR_BUSY; 
	pTransfer->transferSize = size;
	pTransfer->callback = callback; 

	if(!polling)
	{
		DMA_EnableIt(0x1 << channel);
	}
	DMA_EnableChannel(channel); // Enable the channel.
	if(polling)
	{
		while ((DMA_GetChannelStatus() & (0x1 << channel)) == (0x1 << channel));
		if (pTransfer->callback)
		{
			pTransfer->callback();
		}
		pTransfer->transferSize = 0;
		DMA_DisableChannel(channel);
	}
	return 0;
}

//------------------------------------------------------------------------------
/// Returns 1 if no transfer is currently pending on the given channel;
/// otherwise, returns 0.
/// \param channel  Channel number.
//------------------------------------------------------------------------------
U8 DMAD_IsFinished(U8 channel)
{
	if(dmad.transfers[channel].transferSize > 0)
	{
		return 0;
	}
	else
	{
		DMA_DisableChannel(channel);
		return 1;
	}
}

// //------------------------------------------------------------------------------
// ///
// ///
// ///
// //------------------------------------------------------------------------------
void Init_TIMER(void)
{
	volatile U32 dummy;
	PMC_PeriEn(18); // Configure the PMC to enable the Timer Counter clock for TC.
	rTC0_CCR(0) = (0x1 << 1); // (TC) Counter Clock Disable Command
	rTC0_CCR(1) = (0x1 << 1); // (TC) Counter Clock Disable Command
	rTC0_CCR(2) = (0x1 << 1); // (TC) Counter Clock Disable Command
	rTC0_IDR(0) = 0xFFFFFFFF; // Disable interrupts.
	rTC0_IDR(1) = 0xFFFFFFFF; // Disable interrupts.
	rTC0_IDR(2) = 0xFFFFFFFF; // Disable interrupts.
	dummy = rTC0_SR(0); // Clear status register.
	dummy = rTC0_SR(1); // Clear status register.
	dummy = rTC0_SR(2); // Clear status register.
}



// void SFR_WR32(U32 ioaddr, U32 data)
// {
// 	*((volatile U32 *)(ioaddr)) = data;
// }

// //------------------------------------------------------------------------------
// ///
// ///
// ///
// //------------------------------------------------------------------------------
// U32 SFR_RD32(U32 ioaddr)
// {
// 	return *((volatile U32 *)(ioaddr));
// }

// //------------------------------------------------------------------------------
// ///
// ///
// ///
// //------------------------------------------------------------------------------
// void SFR_WR16(U32 ioaddr, U16 data)
// {
// 	*((volatile U16 *)(ioaddr)) = data;
// }

// //------------------------------------------------------------------------------
// ///
// ///
// ///
// //------------------------------------------------------------------------------
// U16 SFR_RD16(U32 ioaddr)
// {
// 	return *((volatile U16 *)(ioaddr));
// }

// //------------------------------------------------------------------------------
// ///
// ///
// ///
// //------------------------------------------------------------------------------
// void SFR_WR8(U32 ioaddr, U8 data)
// {
// 	*((volatile U8 *)(ioaddr)) = data;
// }

// //------------------------------------------------------------------------------
// ///
// ///
// ///
// //------------------------------------------------------------------------------
// U8 SFR_RD8(U32 ioaddr)
// {
// 	return *((volatile U8 *)(ioaddr));
// }
