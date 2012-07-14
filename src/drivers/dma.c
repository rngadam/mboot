/* ----------------------------------------------------------------------------
 *		 
 *        mboot for Lophilo embedded system
 *		
 * ----------------------------------------------------------------------------
 */

#include "types.h"

#include "sfr.h"
#include "dma.h"

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Configure a DMAC peripheral
//------------------------------------------------------------------------------
void DMA_Config(U32 flag)
{
	rDMAC_GCFG = flag;
}

//------------------------------------------------------------------------------
/// Enables a DMAC peripheral
//------------------------------------------------------------------------------
void DMA_Enable(void)
{
	rDMAC_EN = 0x1;
}

//------------------------------------------------------------------------------
/// Disables DMAC peripheral
//------------------------------------------------------------------------------
void DMA_Disable(void)
{
	rDMAC_EN = 0x0;
}

//-----------------------------------------------------------------------------
/// Enable DMA interrupt
/// \param flag IT to be enabled
//-----------------------------------------------------------------------------
void DMA_EnableIt (U32 flag)
{
	rDMAC_EBCIER = flag;
}

//-----------------------------------------------------------------------------
/// Disable DMA interrupt
/// \param flag IT to be enabled
//-----------------------------------------------------------------------------
void DMA_DisableIt (U32 flag)
{
	rDMAC_EBCIDR = flag;
}

//-----------------------------------------------------------------------------
/// Return DMA Interrupt Status
//-----------------------------------------------------------------------------
U32 DMA_GetStatus(void)
{
	return rDMAC_EBCISR;
}

//-----------------------------------------------------------------------------
/// Return DMA Interrupt Mask Status
//-----------------------------------------------------------------------------
U32 DMA_GetInterruptMask(void)
{
	return rDMAC_EBCIMR;
}

//-----------------------------------------------------------------------------
/// Returns the current status register of the given DMA peripheral, but
/// masking interrupt sources which are not currently enabled.
//-----------------------------------------------------------------------------
U32 DMA_GetMaskedStatus(void)
{
	U32 status;
	status = rDMAC_EBCISR;
	status &= rDMAC_EBCIMR;
	return status;
}

//------------------------------------------------------------------------------
/// Enables DMAC channel 
/// \param channel Particular channel number.
//------------------------------------------------------------------------------
void DMA_EnableChannel(U32 ch)
{
    rDMAC_CHER |= 0x1 << ch;
}

void DMA_EnableChannels(U32 bmCh)
{
    rDMAC_CHER = bmCh;
}

//------------------------------------------------------------------------------
/// Disables a DMAC channel 
/// \param channel Particular channel number.
//------------------------------------------------------------------------------
void DMA_DisableChannel(U32 ch)
{
    rDMAC_CHDR |= 0x1 << ch;
}

void DMA_DisableChannels(U32 bmCh)
{
    rDMAC_CHDR = bmCh;
}

//------------------------------------------------------------------------------
/// Resume DMAC channel from an stall state.
/// \param channel Particular channel number.
//------------------------------------------------------------------------------
void DMA_KeeponChannel(U32 ch)
{
    rDMAC_CHER |= (1 << 24) << ch;
}

//------------------------------------------------------------------------------
/// Clear automatic mode for multi-buffer transfer.
/// \param channel Particular channel number.
//------------------------------------------------------------------------------
void DMA_ClearAutoMode(U32 ch)
{
    rDMAC_CTRLB(ch) &= 0x7FFFFFFF;
}

//------------------------------------------------------------------------------
/// Return DMAC channel status 
//------------------------------------------------------------------------------
U32 DMA_GetChannelStatus(void)
{
	return rDMAC_CHSR;
}

//-----------------------------------------------------------------------------
/// Set DMA source address used by a HDMA channel.
/// \param channel Particular channel number.
/// \param sources sources address.
//-----------------------------------------------------------------------------
void DMA_SetSourceAddr(U8 ch, U32 addr)
{
	rDMAC_SADDR(ch) = addr;
}

//-----------------------------------------------------------------------------
/// Set DMA destination address used by a HDMA channel.
/// \param channel Particular channel number.
/// \param sources destination address.
//-----------------------------------------------------------------------------
void DMA_SetDestinationAddr(U8 ch, U32 addr)
{
	rDMAC_DADDR(ch) = addr;
}

//-----------------------------------------------------------------------------
/// Set DMA descriptor address used by a HDMA channel.
/// \param channel Particular channel number.
/// \param sources destination address.
//-----------------------------------------------------------------------------
void DMA_SetDescriptorAddr(U8 ch, U32 addr)
{
	rDMAC_DSCR(ch) = addr;
}

//-----------------------------------------------------------------------------
/// Set DMA control A register used by a HDMA channel.
/// \param channel Particular channel number.
/// \param size Dma transfer size in byte.
/// \param sourceWidth Single transfer width for source.
/// \param destWidth Single transfer width for destination.
/// \param done Transfer done field.
//-----------------------------------------------------------------------------
void DMA_SetSourceBufferSize(U8 channel, U32 size, U8 sourceWidth, U8 destWidth, U8 done)
{
	rDMAC_CTRLA(channel) = (size | (sourceWidth << 24) | (destWidth << 28) | (done << 31));
}
                                
//-----------------------------------------------------------------------------
/// Set DMA transfer mode for source used by a HDMA channel.
/// \param channel Particular channel number.
/// \param transferMode Transfer buffer mode (single, LLI, reload or contiguous)
/// \param addressingType Type of addrassing mode
///                       0 : incrementing, 1: decrementing, 2: fixed.
//-----------------------------------------------------------------------------
void DMA_SetSourceBufferMode(U8 channel, U8 transferMode, U8 addressingType)
{
	U32 value;

	value = rDMAC_CTRLB(channel);
	value &= ~ ((0x1 << 16) | (0x3 << 24) | (0x80000000));//1<<31);
	switch(transferMode)
	{
		case DMA_TRANSFER_SINGLE:
			value |= (0x1 << 16) | (addressingType << 24);
			break;
		case DMA_TRANSFER_LLI:
			value |= (addressingType << 24);
			break;
		case DMA_TRANSFER_RELOAD:
		case DMA_TRANSFER_CONTIGUOUS:
			value |= (0x1 << 16) | (addressingType << 24) | (0x80000000); //1<<31;
			break;
	}             
	rDMAC_CTRLB(channel) = value;

	if(transferMode == DMA_TRANSFER_RELOAD || transferMode == DMA_TRANSFER_CONTIGUOUS)
	{
		value = rDMAC_CFG(channel);
		value &= ~(U32)(0x1 << 8);
		if(transferMode == DMA_TRANSFER_RELOAD) // When automatic mode is activated, the source address and the control register are reloaded from previous transfer.
		{
			value |= (0x1 << 8);
		}
		rDMAC_CFG(channel) = value;
	}
	else
	{
		rDMAC_CFG(channel) = 0;
	}
}

//-----------------------------------------------------------------------------
/// Set DMA transfer mode for destination used by a HDMA channel.
/// \param channel Particular channel number.
/// \param transferMode Transfer buffer mode (single, LLI, reload or contiguous)
/// \param addressingType Type of addrassing mode
///                       0 : incrementing, 1: decrementing, 2: fixed.
//-----------------------------------------------------------------------------
void DMA_SetDestBufferMode(U8 channel, U8 transferMode, U8 addressingType)
{
	U32 value;

	value = rDMAC_CTRLB(channel);
	value &= ~(U32)((0x1 << 20)| (0x3 << 28));

	switch(transferMode)
	{
		case DMA_TRANSFER_SINGLE:
		case DMA_TRANSFER_RELOAD:
		case DMA_TRANSFER_CONTIGUOUS:
			value |= (0x1 << 20) | (addressingType << 28);
			break;
		case DMA_TRANSFER_LLI:
			value |= addressingType << 28;
			break;
	}             
	rDMAC_CTRLB(channel) = value;
	if(transferMode == DMA_TRANSFER_RELOAD || transferMode == DMA_TRANSFER_CONTIGUOUS)
	{
		value = rDMAC_CFG(channel);
		value &= ~(U32)(0x1 << 12);
		if(transferMode == DMA_TRANSFER_RELOAD) // When automatic mode is activated, the source address and the control register are reloaded from previous transfer.
		{
			value |= (0x1 << 12);
		}
		rDMAC_CFG(channel) = value;
	}
	else
	{
		rDMAC_CFG(channel) = 0;
	}
}

//------------------------------------------------------------------------------
/// Set DMA configuration registe used by a HDMA channel.
/// \param channel Particular channel number.
/// \param value configuration value.
//------------------------------------------------------------------------------
void DMA_SetConfig(U8 channel, U32 value)
{
    rDMAC_CFG(channel) = value;
}

//------------------------------------------------------------------------------
/// Set DMA source PIP configuration used by a HDMA channel.
/// \param channel Particular channel number.
/// \param pipHole stop on done mode.
/// \param pipBoundary lock mode.
//------------------------------------------------------------------------------
void DMA_SPIPconfig(U8 channel, U32 pipHole, U32 pipBoundary)
{
    U32 value;
	
    value = rDMAC_CTRLB(channel);
    value &= ~(U32)(0x1 << 8);
    value |= (0x1 << 8);
    rDMAC_CTRLB(channel) = value;
    rDMAC_SPIP(channel) = (pipHole + 1) | pipBoundary <<16;
}

//------------------------------------------------------------------------------
/// Set DMA destination PIP configuration used by a HDMA channel.
/// \param channel Particular channel number.
/// \param pipHole stop on done mode.
/// \param pipBoundary lock mode.
//------------------------------------------------------------------------------
void DMA_DPIPconfig(U8 channel, U32 pipHole, U32 pipBoundary)
{
    U32 value;
	
    value = rDMAC_CTRLB(channel);
    value &= ~(U32)(0x1 << 12);
    value |= (0x1 << 12);
    rDMAC_CTRLB(channel) = value;
    rDMAC_DPIP(channel) = (pipHole + 1) | pipBoundary <<16;
}

//-----------------------------------------------------------------------------
/// Set DMA control B register Flow control bit field.
/// \param channel Particular channel number.
/// \param size Dma transfer size in byte.
/// \param sourceWidth Single transfer width for source.
/// \param destWidth Single transfer width for destination.
/// \param done Transfer done field.
//-----------------------------------------------------------------------------
void DMA_SetFlowControl(U8 channel, U32 flow)
{
    U32 value;
	
    value = rDMAC_CTRLB(channel);
    value &= ~(U32)(0x7 << 21);
    value |= flow << 21;       
    rDMAC_CTRLB(channel) = value;
}

