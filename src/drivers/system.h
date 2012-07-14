/*
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "types.h"

#define DMAD_ERROR_BUSY              1		/// DMA driver is currently busy.
#define DMAD_USE_DEFAULT_IT          1		/// Using the default interrupt handler of the DMAD
#define DMAD_NO_DEFAULT_IT           0

typedef void (*DmaCallback)(); /// DMA driver callback function.

typedef struct _DmaLinkList
{
	U32 sourceAddress; /// Source address.
	U32 destAddress; /// Destination address.
	U32 controlA; /// Control A value.
	U32 controlB; /// Control B value.
	U32 descriptor; /// Descriptor Address.
} DmaLinkList;

typedef struct _PictureInPicture
{
	U16 pipSourceHoleSize; /// Size in byte add to the source address in PIP.
	U16 pipSourceBoundarySize; /// Number of transfer in byte to perform before the source address increase.
	U16 pipDestHoleSize; /// Size in byte add to the destination address in PIP.
	U16 pipDestBoundarySize; /// Number of transfer in byte to perform before the destination address increase.
} PictureInPicture;

typedef struct _DmaTransfer
{
	volatile U8 status; /// Buffer transfer status.
	U32 bufSize; /// Transfer buffer size in byte.
	volatile U32 transferSize; /// Total transfer size to byte.
	DmaCallback callback; /// Optional callback function.
} DmaTransfer;

typedef struct _Dmad
{
	DmaTransfer transfers[8]; /// List of transfers occuring on each channel.
} Dmad;


//void Delay_m(volatile U32 loop);
void Delay_10us(U8 delay);
void Delay_ms(U16 delay);

void PMC_PeriEn(U32 id);
void PMC_PeriDis(U32 id);
void IRQ_PeriConf(U32 id, U32 mode, void( *handler )( void ));
void IRQ_PeriEn(U32 id);
void IRQ_PeriDis(U32 id);

void DMAD_Initialize(U8 channel, U8 defaultHandler);
U8 DMAD_Configure_Buffer(U8 channel, U8 sourceTransferMode, U8 destTransferMode, DmaLinkList *lli, PictureInPicture *pip);
U8 DMAD_Configure_TransferController(U8 channel, U32 bufSize, U8 sourceWidth, U8 destWidth, U32 sourceAddress, U32 destAddress);
U8 DMAD_BufferTransfer(U8 channel, U32 size, DmaCallback callback, U8 polling);
U8 DMAD_IsFinished(U8 channel);

void Init_TIMER(void);

#endif /* __SYSTEM_H__ */
