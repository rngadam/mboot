/*
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */

#ifndef __DMA_H__
#define __DMA_H__

#include "types.h"

#define DMA_TRANSFER_SINGLE      0
#define DMA_TRANSFER_LLI         1  
#define DMA_TRANSFER_RELOAD      2
#define DMA_TRANSFER_CONTIGUOUS  3

void DMA_Config(U32 flag);
void DMA_Enable(void);
void DMA_Disable(void);
void DMA_EnableIt (U32 flag);
void DMA_DisableIt (U32 flag);
U32 DMA_GetStatus(void);
U32 DMA_GetInterruptMask(void);
U32 DMA_GetMaskedStatus(void);
void DMA_EnableChannel(U32 ch);
void DMA_EnableChannels(U32 bmCh);
void DMA_DisableChannel(U32 ch);
void DMA_DisableChannels(U32 bmCh);
void DMA_KeeponChannel(U32 ch);
void DMA_ClearAutoMode(U32 ch);
U32 DMA_GetChannelStatus(void);
void DMA_SetSourceAddr(U8 ch, U32 addr);
void DMA_SetDestinationAddr(U8 ch, U32 addr);
void DMA_SetDescriptorAddr(U8 ch, U32 addr);
void DMA_SetSourceBufferSize(U8 channel, U32 size, U8 sourceWidth, U8 destWidth, U8 done);
void DMA_SetSourceBufferMode(U8 channel, U8 transferMode, U8 addressingType);
void DMA_SetDestBufferMode(U8 channel, U8 transferMode, U8 addressingType);
void DMA_SetConfig(U8 channel, U32 value);
void DMA_SPIPconfig(U8 channel, U32 pipHole, U32 pipBoundary);
void DMA_DPIPconfig(U8 channel, U32 pipHole, U32 pipBoundary);
void DMA_SetFlowControl(U8 channel, U32 flow);

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR HDMA Channel structure
// *****************************************************************************
#ifndef __ASSEMBLY__
typedef struct _AT91S_HDMA_CH {
	U32	 HDMA_SADDR; 	// HDMA Channel Source Address Register
	U32	 HDMA_DADDR; 	// HDMA Channel Destination Address Register
	U32	 HDMA_DSCR; 	// HDMA Channel Descriptor Address Register
	U32	 HDMA_CTRLA; 	// HDMA Channel Control A Register
	U32	 HDMA_CTRLB; 	// HDMA Channel Control B Register
	U32	 HDMA_CFG; 	// HDMA Channel Configuration Register
	U32	 HDMA_SPIP; 	// HDMA Channel Source Picture in Picture Configuration Register
	U32	 HDMA_DPIP; 	// HDMA Channel Destination Picture in Picture Configuration Register
	U32	 HDMA_BDSCR; 	// HDMA Reserved
	U32	 HDMA_CADDR; 	// HDMA Reserved
} AT91S_HDMA_CH, *AT91PS_HDMA_CH;
#else
#define HDMA_SADDR      (AT91_CAST(U32 *) 	0x00000000) // (HDMA_SADDR) HDMA Channel Source Address Register
#define HDMA_DADDR      (AT91_CAST(U32 *) 	0x00000004) // (HDMA_DADDR) HDMA Channel Destination Address Register
#define HDMA_DSCR       (AT91_CAST(U32 *) 	0x00000008) // (HDMA_DSCR) HDMA Channel Descriptor Address Register
#define HDMA_CTRLA      (AT91_CAST(U32 *) 	0x0000000C) // (HDMA_CTRLA) HDMA Channel Control A Register
#define HDMA_CTRLB      (AT91_CAST(U32 *) 	0x00000010) // (HDMA_CTRLB) HDMA Channel Control B Register
#define HDMA_CFG        (AT91_CAST(U32 *) 	0x00000014) // (HDMA_CFG) HDMA Channel Configuration Register
#define HDMA_SPIP       (AT91_CAST(U32 *) 	0x00000018) // (HDMA_SPIP) HDMA Channel Source Picture in Picture Configuration Register
#define HDMA_DPIP       (AT91_CAST(U32 *) 	0x0000001C) // (HDMA_DPIP) HDMA Channel Destination Picture in Picture Configuration Register
#define HDMA_BDSCR      (AT91_CAST(U32 *) 	0x00000020) // (HDMA_BDSCR) HDMA Reserved
#define HDMA_CADDR      (AT91_CAST(U32 *) 	0x00000024) // (HDMA_CADDR) HDMA Reserved

#endif
// -------- HDMA_SADDR : (HDMA_CH Offset: 0x0)  -------- 
#define AT91C_SADDR           (0x0 <<  0) // (HDMA_CH) 
// -------- HDMA_DADDR : (HDMA_CH Offset: 0x4)  -------- 
#define AT91C_DADDR           (0x0 <<  0) // (HDMA_CH) 
// -------- HDMA_DSCR : (HDMA_CH Offset: 0x8)  -------- 
#define AT91C_HDMA_DSCR_IF    (0x3 <<  0) // (HDMA_CH) Select AHB-Lite Interface for current channel
#define 	AT91C_HDMA_DSCR_IF_0                    (0x0) // (HDMA_CH) The Buffer Transfer descriptor is fetched via AHB-Lite Interface 0.
#define 	AT91C_HDMA_DSCR_IF_1                    (0x1) // (HDMA_CH) The Buffer Transfer descriptor is fetched via AHB-Lite Interface 1.
#define 	AT91C_HDMA_DSCR_IF_2                    (0x2) // (HDMA_CH) The Buffer Transfer descriptor is fetched via AHB-Lite Interface 2.
#define 	AT91C_HDMA_DSCR_IF_3                    (0x3) // (HDMA_CH) The Buffer Transfer descriptor is fetched via AHB-Lite Interface 3.
#define AT91C_HDMA_DSCR       (0x3FFFFFFF <<  2) // (HDMA_CH) Buffer Transfer descriptor address. This address is word aligned.
// -------- HDMA_CTRLA : (HDMA_CH Offset: 0xc)  -------- 
#define AT91C_HDMA_BTSIZE     (0xFFFF <<  0) // (HDMA_CH) Buffer Transfer Size.
#define AT91C_HDMA_SCSIZE     (0x7 << 16) // (HDMA_CH) Source Chunk Transfer Size.
#define 	AT91C_HDMA_SCSIZE_1                    (0x0 << 16) // (HDMA_CH) 1.
#define 	AT91C_HDMA_SCSIZE_4                    (0x1 << 16) // (HDMA_CH) 4.
#define 	AT91C_HDMA_SCSIZE_8                    (0x2 << 16) // (HDMA_CH) 8.
#define 	AT91C_HDMA_SCSIZE_16                   (0x3 << 16) // (HDMA_CH) 16.
#define 	AT91C_HDMA_SCSIZE_32                   (0x4 << 16) // (HDMA_CH) 32.
#define 	AT91C_HDMA_SCSIZE_64                   (0x5 << 16) // (HDMA_CH) 64.
#define 	AT91C_HDMA_SCSIZE_128                  (0x6 << 16) // (HDMA_CH) 128.
#define 	AT91C_HDMA_SCSIZE_256                  (0x7 << 16) // (HDMA_CH) 256.
#define AT91C_HDMA_DCSIZE     (0x7 << 20) // (HDMA_CH) Destination Chunk Transfer Size
#define 	AT91C_HDMA_DCSIZE_1                    (0x0 << 20) // (HDMA_CH) 1.
#define 	AT91C_HDMA_DCSIZE_4                    (0x1 << 20) // (HDMA_CH) 4.
#define 	AT91C_HDMA_DCSIZE_8                    (0x2 << 20) // (HDMA_CH) 8.
#define 	AT91C_HDMA_DCSIZE_16                   (0x3 << 20) // (HDMA_CH) 16.
#define 	AT91C_HDMA_DCSIZE_32                   (0x4 << 20) // (HDMA_CH) 32.
#define 	AT91C_HDMA_DCSIZE_64                   (0x5 << 20) // (HDMA_CH) 64.
#define 	AT91C_HDMA_DCSIZE_128                  (0x6 << 20) // (HDMA_CH) 128.
#define 	AT91C_HDMA_DCSIZE_256                  (0x7 << 20) // (HDMA_CH) 256.
#define AT91C_HDMA_SRC_WIDTH  (0x3 << 24) // (HDMA_CH) Source Single Transfer Size
#define 	AT91C_HDMA_SRC_WIDTH_BYTE                 (0x0 << 24) // (HDMA_CH) BYTE.
#define 	AT91C_HDMA_SRC_WIDTH_HALFWORD             (0x1 << 24) // (HDMA_CH) HALF-WORD.
#define 	AT91C_HDMA_SRC_WIDTH_WORD                 (0x2 << 24) // (HDMA_CH) WORD.
#define AT91C_HDMA_DST_WIDTH  (0x3 << 28) // (HDMA_CH) Destination Single Transfer Size
#define 	AT91C_HDMA_DST_WIDTH_BYTE                 (0x0 << 28) // (HDMA_CH) BYTE.
#define 	AT91C_HDMA_DST_WIDTH_HALFWORD             (0x1 << 28) // (HDMA_CH) HALF-WORD.
#define 	AT91C_HDMA_DST_WIDTH_WORD                 (0x2 << 28) // (HDMA_CH) WORD.
#define AT91C_HDMA_DONE       (0x80000000) // (0x1 << 31) // (HDMA_CH) 
// -------- HDMA_CTRLB : (HDMA_CH Offset: 0x10)  -------- 
#define AT91C_HDMA_SIF        (0x3 <<  0) // (HDMA_CH) Source Interface Selection Field.
#define 	AT91C_HDMA_SIF_0                    (0x0) // (HDMA_CH) The Source Transfer is done via AHB-Lite Interface 0.
#define 	AT91C_HDMA_SIF_1                    (0x1) // (HDMA_CH) The Source Transfer is done via AHB-Lite Interface 1.
#define 	AT91C_HDMA_SIF_2                    (0x2) // (HDMA_CH) The Source Transfer is done via AHB-Lite Interface 2.
#define 	AT91C_HDMA_SIF_3                    (0x3) // (HDMA_CH) The Source Transfer is done via AHB-Lite Interface 3.
#define AT91C_HDMA_DIF        (0x3 <<  4) // (HDMA_CH) Destination Interface Selection Field.
#define 	AT91C_HDMA_DIF_0                    (0x0 <<  4) // (HDMA_CH) The Destination Transfer is done via AHB-Lite Interface 0.
#define 	AT91C_HDMA_DIF_1                    (0x1 <<  4) // (HDMA_CH) The Destination Transfer is done via AHB-Lite Interface 1.
#define 	AT91C_HDMA_DIF_2                    (0x2 <<  4) // (HDMA_CH) The Destination Transfer is done via AHB-Lite Interface 2.
#define 	AT91C_HDMA_DIF_3                    (0x3 <<  4) // (HDMA_CH) The Destination Transfer is done via AHB-Lite Interface 3.
#define AT91C_HDMA_SRC_PIP    (0x1 <<  8) // (HDMA_CH) Source Picture-in-Picture Mode
#define 	AT91C_HDMA_SRC_PIP_DISABLE              (0x0 <<  8) // (HDMA_CH) Source Picture-in-Picture mode is disabled.
#define 	AT91C_HDMA_SRC_PIP_ENABLE               (0x1 <<  8) // (HDMA_CH) Source Picture-in-Picture mode is enabled.
#define AT91C_HDMA_DST_PIP    (0x1 << 12) // (HDMA_CH) Destination Picture-in-Picture Mode
#define 	AT91C_HDMA_DST_PIP_DISABLE              (0x0 << 12) // (HDMA_CH) Destination Picture-in-Picture mode is disabled.
#define 	AT91C_HDMA_DST_PIP_ENABLE               (0x1 << 12) // (HDMA_CH) Destination Picture-in-Picture mode is enabled.
#define AT91C_HDMA_SRC_DSCR   (0x1 << 16) // (HDMA_CH) Source Buffer Descriptor Fetch operation
#define 	AT91C_HDMA_SRC_DSCR_FETCH_FROM_MEM       (0x0 << 16) // (HDMA_CH) Source address is updated when the descriptor is fetched from the memory.
#define 	AT91C_HDMA_SRC_DSCR_FETCH_DISABLE        (0x1 << 16) // (HDMA_CH) Buffer Descriptor Fetch operation is disabled for the Source.
#define AT91C_HDMA_DST_DSCR   (0x1 << 20) // (HDMA_CH) Destination Buffer Descriptor operation
#define 	AT91C_HDMA_DST_DSCR_FETCH_FROM_MEM       (0x0 << 20) // (HDMA_CH) Destination address is updated when the descriptor is fetched from the memory.
#define 	AT91C_HDMA_DST_DSCR_FETCH_DISABLE        (0x1 << 20) // (HDMA_CH) Buffer Descriptor Fetch operation is disabled for the destination.
#define AT91C_HDMA_FC         (0x7 << 21) // (HDMA_CH) This field defines which devices controls the size of the buffer transfer, also referred as to the Flow Controller.
#define 	AT91C_HDMA_FC_MEM2MEM              (0x0 << 21) // (HDMA_CH) Memory-to-Memory (DMA Controller).
#define 	AT91C_HDMA_FC_MEM2PER              (0x1 << 21) // (HDMA_CH) Memory-to-Peripheral (DMA Controller).
#define 	AT91C_HDMA_FC_PER2MEM              (0x2 << 21) // (HDMA_CH) Peripheral-to-Memory (DMA Controller).
#define 	AT91C_HDMA_FC_PER2PER              (0x3 << 21) // (HDMA_CH) Peripheral-to-Peripheral (DMA Controller).
#define 	AT91C_HDMA_FC_PER2MEM_PER          (0x4 << 21) // (HDMA_CH) Peripheral-to-Memory (Peripheral).
#define 	AT91C_HDMA_FC_MEM2PER_PER          (0x5 << 21) // (HDMA_CH) Memory-to-Peripheral (Peripheral).
#define 	AT91C_HDMA_FC_PER2PER_PER          (0x6 << 21) // (HDMA_CH) Peripheral-to-Peripheral (Source Peripheral).
#define AT91C_HDMA_SRC_ADDRESS_MODE (0x3 << 24) // (HDMA_CH) Type of addressing mode
#define 	AT91C_HDMA_SRC_ADDRESS_MODE_INCR                 (0x0 << 24) // (HDMA_CH) Incrementing Mode.
#define 	AT91C_HDMA_SRC_ADDRESS_MODE_DECR                 (0x1 << 24) // (HDMA_CH) Decrementing Mode.
#define 	AT91C_HDMA_SRC_ADDRESS_MODE_FIXED                (0x2 << 24) // (HDMA_CH) Fixed Mode.
#define AT91C_HDMA_DST_ADDRESS_MODE (0x3 << 28) // (HDMA_CH) Type of addressing mode
#define 	AT91C_HDMA_DST_ADDRESS_MODE_INCR                 (0x0 << 28) // (HDMA_CH) Incrementing Mode.
#define 	AT91C_HDMA_DST_ADDRESS_MODE_DECR                 (0x1 << 28) // (HDMA_CH) Decrementing Mode.
#define 	AT91C_HDMA_DST_ADDRESS_MODE_FIXED                (0x2 << 28) // (HDMA_CH) Fixed Mode.
#define AT91C_HDMA_AUTO       (0x1 << 31) // (HDMA_CH) Automatic multiple buffer transfer enable
#define 	AT91C_HDMA_AUTO_DISABLE              (0x0 << 31) // (HDMA_CH) Automatic multiple buffer transfer is disabled.
#define 	AT91C_HDMA_AUTO_ENABLE               (0x1 << 31) // (HDMA_CH) Automatic multiple buffer transfer is enabled. This enables replay mode or contiguous mode when several buffers are transferred.
// -------- HDMA_CFG : (HDMA_CH Offset: 0x14)  -------- 
#define AT91C_HDMA_SRC_PER    (0xF <<  0) // (HDMA_CH) Channel Source Request is associated with peripheral identifier coded SRC_PER handshaking interface.
#define 	AT91C_HDMA_SRC_PER_0                    (0x0) // (HDMA_CH) HW Handshaking Interface number 0.
#define 	AT91C_HDMA_SRC_PER_1                    (0x1) // (HDMA_CH) HW Handshaking Interface number 1.
#define 	AT91C_HDMA_SRC_PER_2                    (0x2) // (HDMA_CH) HW Handshaking Interface number 2.
#define 	AT91C_HDMA_SRC_PER_3                    (0x3) // (HDMA_CH) HW Handshaking Interface number 3.
#define 	AT91C_HDMA_SRC_PER_4                    (0x4) // (HDMA_CH) HW Handshaking Interface number 4.
#define 	AT91C_HDMA_SRC_PER_5                    (0x5) // (HDMA_CH) HW Handshaking Interface number 5.
#define 	AT91C_HDMA_SRC_PER_6                    (0x6) // (HDMA_CH) HW Handshaking Interface number 6.
#define 	AT91C_HDMA_SRC_PER_7                    (0x7) // (HDMA_CH) HW Handshaking Interface number 7.
#define 	AT91C_HDMA_SRC_PER_8                    (0x8) // (HDMA_CH) HW Handshaking Interface number 8.
#define 	AT91C_HDMA_SRC_PER_9                    (0x9) // (HDMA_CH) HW Handshaking Interface number 9.
#define 	AT91C_HDMA_SRC_PER_10                   (0xA) // (HDMA_CH) HW Handshaking Interface number 10.
#define 	AT91C_HDMA_SRC_PER_11                   (0xB) // (HDMA_CH) HW Handshaking Interface number 11.
#define 	AT91C_HDMA_SRC_PER_12                   (0xC) // (HDMA_CH) HW Handshaking Interface number 12.
#define 	AT91C_HDMA_SRC_PER_13                   (0xD) // (HDMA_CH) HW Handshaking Interface number 13.
#define 	AT91C_HDMA_SRC_PER_14                   (0xE) // (HDMA_CH) HW Handshaking Interface number 14.
#define 	AT91C_HDMA_SRC_PER_15                   (0xF) // (HDMA_CH) HW Handshaking Interface number 15.
#define AT91C_HDMA_DST_PER    (0xF <<  4) // (HDMA_CH) Channel Destination Request is associated with peripheral identifier coded DST_PER handshaking interface.
#define 	AT91C_HDMA_DST_PER_0                    (0x0 <<  4) // (HDMA_CH) HW Handshaking Interface number 0.
#define 	AT91C_HDMA_DST_PER_1                    (0x1 <<  4) // (HDMA_CH) HW Handshaking Interface number 1.
#define 	AT91C_HDMA_DST_PER_2                    (0x2 <<  4) // (HDMA_CH) HW Handshaking Interface number 2.
#define 	AT91C_HDMA_DST_PER_3                    (0x3 <<  4) // (HDMA_CH) HW Handshaking Interface number 3.
#define 	AT91C_HDMA_DST_PER_4                    (0x4 <<  4) // (HDMA_CH) HW Handshaking Interface number 4.
#define 	AT91C_HDMA_DST_PER_5                    (0x5 <<  4) // (HDMA_CH) HW Handshaking Interface number 5.
#define 	AT91C_HDMA_DST_PER_6                    (0x6 <<  4) // (HDMA_CH) HW Handshaking Interface number 6.
#define 	AT91C_HDMA_DST_PER_7                    (0x7 <<  4) // (HDMA_CH) HW Handshaking Interface number 7.
#define 	AT91C_HDMA_DST_PER_8                    (0x8 <<  4) // (HDMA_CH) HW Handshaking Interface number 8.
#define 	AT91C_HDMA_DST_PER_9                    (0x9 <<  4) // (HDMA_CH) HW Handshaking Interface number 9.
#define 	AT91C_HDMA_DST_PER_10                   (0xA <<  4) // (HDMA_CH) HW Handshaking Interface number 10.
#define 	AT91C_HDMA_DST_PER_11                   (0xB <<  4) // (HDMA_CH) HW Handshaking Interface number 11.
#define 	AT91C_HDMA_DST_PER_12                   (0xC <<  4) // (HDMA_CH) HW Handshaking Interface number 12.
#define 	AT91C_HDMA_DST_PER_13                   (0xD <<  4) // (HDMA_CH) HW Handshaking Interface number 13.
#define 	AT91C_HDMA_DST_PER_14                   (0xE <<  4) // (HDMA_CH) HW Handshaking Interface number 14.
#define 	AT91C_HDMA_DST_PER_15                   (0xF <<  4) // (HDMA_CH) HW Handshaking Interface number 15.
#define AT91C_HDMA_SRC_REP    (0x1 <<  8) // (HDMA_CH) Source Replay Mode
#define 	AT91C_HDMA_SRC_REP_CONTIGUOUS_ADDR      (0x0 <<  8) // (HDMA_CH) When automatic mode is activated, source address is contiguous between two buffers.
#define 	AT91C_HDMA_SRC_REP_RELOAD_ADDR          (0x1 <<  8) // (HDMA_CH) When automatic mode is activated, the source address and the control register are reloaded from previous transfer..
#define AT91C_HDMA_SRC_H2SEL  (0x1 <<  9) // (HDMA_CH) Source Handshaking Mode
#define 	AT91C_HDMA_SRC_H2SEL_SW                   (0x0 <<  9) // (HDMA_CH) Software handshaking interface is used to trigger a transfer request.
#define 	AT91C_HDMA_SRC_H2SEL_HW                   (0x1 <<  9) // (HDMA_CH) Hardware handshaking interface is used to trigger a transfer request.
#define AT91C_HDMA_DST_REP    (0x1 << 12) // (HDMA_CH) Destination Replay Mode
#define 	AT91C_HDMA_DST_REP_CONTIGUOUS_ADDR      (0x0 << 12) // (HDMA_CH) When automatic mode is activated, destination address is contiguous between two buffers.
#define 	AT91C_HDMA_DST_REP_RELOAD_ADDR          (0x1 << 12) // (HDMA_CH) When automatic mode is activated, the destination address and the control register are reloaded from previous transfer..
#define AT91C_HDMA_DST_H2SEL  (0x1 << 13) // (HDMA_CH) Destination Handshaking Mode
#define 	AT91C_HDMA_DST_H2SEL_SW                   (0x0 << 13) // (HDMA_CH) Software handshaking interface is used to trigger a transfer request.
#define 	AT91C_HDMA_DST_H2SEL_HW                   (0x1 << 13) // (HDMA_CH) Hardware handshaking interface is used to trigger a transfer request.
#define AT91C_HDMA_SOD        (0x1 << 16) // (HDMA_CH) STOP ON DONE
#define 	AT91C_HDMA_SOD_DISABLE              (0x0 << 16) // (HDMA_CH) STOP ON DONE disabled, the descriptor fetch operation ignores DONE Field of CTRLA register.
#define 	AT91C_HDMA_SOD_ENABLE               (0x1 << 16) // (HDMA_CH) STOP ON DONE activated, the DMAC module is automatically disabled if DONE FIELD is set to 1.
#define AT91C_HDMA_LOCK_IF    (0x1 << 20) // (HDMA_CH) Interface Lock
#define 	AT91C_HDMA_LOCK_IF_DISABLE              (0x0 << 20) // (HDMA_CH) Interface Lock capability is disabled.
#define 	AT91C_HDMA_LOCK_IF_ENABLE               (0x1 << 20) // (HDMA_CH) Interface Lock capability is enabled.
#define AT91C_HDMA_LOCK_B     (0x1 << 21) // (HDMA_CH) AHB Bus Lock
#define 	AT91C_HDMA_LOCK_B_DISABLE              (0x0 << 21) // (HDMA_CH) AHB Bus Locking capability is disabled.
#define 	AT91C_HDMA_LOCK_B_ENABLE               (0x1 << 21) // (HDMA_CH) AHB Bus Locking capability is enabled.
#define AT91C_HDMA_LOCK_IF_L  (0x1 << 22) // (HDMA_CH) Master Interface Arbiter Lock
#define 	AT91C_HDMA_LOCK_IF_L_CHUNK                (0x0 << 22) // (HDMA_CH) The Master Interface Arbiter is locked by the channel x for a chunk transfer.
#define 	AT91C_HDMA_LOCK_IF_L_BUFFER               (0x1 << 22) // (HDMA_CH) The Master Interface Arbiter is locked by the channel x for a buffer transfer.
#define AT91C_HDMA_AHB_PROT   (0x7 << 24) // (HDMA_CH) AHB Prot
#define AT91C_HDMA_FIFOCFG    (0x3 << 28) // (HDMA_CH) FIFO Request Configuration
#define 	AT91C_HDMA_FIFOCFG_LARGESTBURST         (0x0 << 28) // (HDMA_CH) The largest defined length AHB burst is performed on the destination AHB interface.
#define 	AT91C_HDMA_FIFOCFG_HALFFIFO             (0x1 << 28) // (HDMA_CH) When half fifo size is available/filled a source/destination request is serviced.
#define 	AT91C_HDMA_FIFOCFG_ENOUGHSPACE          (0x2 << 28) // (HDMA_CH) When there is enough space/data available to perfom a single AHB access then the request is serviced.
// -------- HDMA_SPIP : (HDMA_CH Offset: 0x18)  -------- 
#define AT91C_SPIP_HOLE       (0xFFFF <<  0) // (HDMA_CH) This field indicates the value to add to the address when the programmable boundary has been reached.
#define AT91C_SPIP_BOUNDARY   (0x3FF << 16) // (HDMA_CH) This field indicates the number of source transfers to perform before the automatic address increment operation.
// -------- HDMA_DPIP : (HDMA_CH Offset: 0x1c)  -------- 
#define AT91C_DPIP_HOLE       (0xFFFF <<  0) // (HDMA_CH) This field indicates the value to add to the address when the programmable boundary has been reached.
#define AT91C_DPIP_BOUNDARY   (0x3FF << 16) // (HDMA_CH) This field indicates the number of source transfers to perform before the automatic address increment operation.
// -------- HDMA_BDSCR : (HDMA_CH Offset: 0x20)  -------- 
// -------- HDMA_CADDR : (HDMA_CH Offset: 0x24)  -------- 

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR HDMA controller
// *****************************************************************************
#ifndef __ASSEMBLY__
typedef struct _AT91S_HDMA {
	U32	 HDMA_GCFG; 	// HDMA Global Configuration Register
	U32	 HDMA_EN; 	// HDMA Controller Enable Register
	U32	 HDMA_SREQ; 	// HDMA Software Single Request Register
	U32	 HDMA_CREQ; 	// HDMA Software Chunk Transfer Request Register
	U32	 HDMA_LAST; 	// HDMA Software Last Transfer Flag Register
	U32	 HDMA_SYNC; 	// HDMA Request Synchronization Register
	U32	 HDMA_EBCIER; 	// HDMA Error, Chained Buffer transfer completed and Buffer transfer completed Interrupt Enable register
	U32	 HDMA_EBCIDR; 	// HDMA Error, Chained Buffer transfer completed and Buffer transfer completed Interrupt Disable register
	U32	 HDMA_EBCIMR; 	// HDMA Error, Chained Buffer transfer completed and Buffer transfer completed Mask Register
	U32	 HDMA_EBCISR; 	// HDMA Error, Chained Buffer transfer completed and Buffer transfer completed Status Register
	U32	 HDMA_CHER; 	// HDMA Channel Handler Enable Register
	U32	 HDMA_CHDR; 	// HDMA Channel Handler Disable Register
	U32	 HDMA_CHSR; 	// HDMA Channel Handler Status Register
	U32	 HDMA_RSVD0; 	// HDMA Reserved
	U32	 HDMA_RSVD1; 	// HDMA Reserved
	AT91S_HDMA_CH	 HDMA_CH[8]; 	// HDMA Channel structure
} AT91S_HDMA, *AT91PS_HDMA;
#else
#define HDMA_GCFG       (AT91_CAST(U32 *) 	0x00000000) // (HDMA_GCFG) HDMA Global Configuration Register
#define HDMA_EN         (AT91_CAST(U32 *) 	0x00000004) // (HDMA_EN) HDMA Controller Enable Register
#define HDMA_SREQ       (AT91_CAST(U32 *) 	0x00000008) // (HDMA_SREQ) HDMA Software Single Request Register
#define HDMA_CREQ       (AT91_CAST(U32 *) 	0x0000000C) // (HDMA_CREQ) HDMA Software Chunk Transfer Request Register
#define HDMA_LAST       (AT91_CAST(U32 *) 	0x00000010) // (HDMA_LAST) HDMA Software Last Transfer Flag Register
#define HDMA_SYNC       (AT91_CAST(U32 *) 	0x00000014) // (HDMA_SYNC) HDMA Request Synchronization Register
#define HDMA_EBCIER     (AT91_CAST(U32 *) 	0x00000018) // (HDMA_EBCIER) HDMA Error, Chained Buffer transfer completed and Buffer transfer completed Interrupt Enable register
#define HDMA_EBCIDR     (AT91_CAST(U32 *) 	0x0000001C) // (HDMA_EBCIDR) HDMA Error, Chained Buffer transfer completed and Buffer transfer completed Interrupt Disable register
#define HDMA_EBCIMR     (AT91_CAST(U32 *) 	0x00000020) // (HDMA_EBCIMR) HDMA Error, Chained Buffer transfer completed and Buffer transfer completed Mask Register
#define HDMA_EBCISR     (AT91_CAST(U32 *) 	0x00000024) // (HDMA_EBCISR) HDMA Error, Chained Buffer transfer completed and Buffer transfer completed Status Register
#define HDMA_CHER       (AT91_CAST(U32 *) 	0x00000028) // (HDMA_CHER) HDMA Channel Handler Enable Register
#define HDMA_CHDR       (AT91_CAST(U32 *) 	0x0000002C) // (HDMA_CHDR) HDMA Channel Handler Disable Register
#define HDMA_CHSR       (AT91_CAST(U32 *) 	0x00000030) // (HDMA_CHSR) HDMA Channel Handler Status Register
#define HDMA_RSVD0      (AT91_CAST(U32 *) 	0x00000034) // (HDMA_RSVD0) HDMA Reserved
#define HDMA_RSVD1      (AT91_CAST(U32 *) 	0x00000038) // (HDMA_RSVD1) HDMA Reserved

#endif
// -------- HDMA_GCFG : (HDMA Offset: 0x0)  -------- 
#define AT91C_HDMA_IF0_BIGEND (0x1 <<  0) // (HDMA) AHB-Lite Interface 0 endian mode.
#define 	AT91C_HDMA_IF0_BIGEND_IS_LITTLE_ENDIAN     (0x0) // (HDMA) AHB-Lite Interface 0 is little endian.
#define 	AT91C_HDMA_IF0_BIGEND_IS_BIG_ENDIAN        (0x1) // (HDMA) AHB-Lite Interface 0 is big endian.
#define AT91C_HDMA_IF1_BIGEND (0x1 <<  1) // (HDMA) AHB-Lite Interface 1 endian mode.
#define 	AT91C_HDMA_IF1_BIGEND_IS_LITTLE_ENDIAN     (0x0 <<  1) // (HDMA) AHB-Lite Interface 1 is little endian.
#define 	AT91C_HDMA_IF1_BIGEND_IS_BIG_ENDIAN        (0x1 <<  1) // (HDMA) AHB-Lite Interface 1 is big endian.
#define AT91C_HDMA_IF2_BIGEND (0x1 <<  2) // (HDMA) AHB-Lite Interface 2 endian mode.
#define 	AT91C_HDMA_IF2_BIGEND_IS_LITTLE_ENDIAN     (0x0 <<  2) // (HDMA) AHB-Lite Interface 2 is little endian.
#define 	AT91C_HDMA_IF2_BIGEND_IS_BIG_ENDIAN        (0x1 <<  2) // (HDMA) AHB-Lite Interface 2 is big endian.
#define AT91C_HDMA_IF3_BIGEND (0x1 <<  3) // (HDMA) AHB-Lite Interface 3 endian mode.
#define 	AT91C_HDMA_IF3_BIGEND_IS_LITTLE_ENDIAN     (0x0 <<  3) // (HDMA) AHB-Lite Interface 3 is little endian.
#define 	AT91C_HDMA_IF3_BIGEND_IS_BIG_ENDIAN        (0x1 <<  3) // (HDMA) AHB-Lite Interface 3 is big endian.
#define AT91C_HDMA_ARB_CFG    (0x1 <<  4) // (HDMA) Arbiter mode.
#define 	AT91C_HDMA_ARB_CFG_FIXED                (0x0 <<  4) // (HDMA) Fixed priority arbiter.
#define 	AT91C_HDMA_ARB_CFG_ROUND_ROBIN          (0x1 <<  4) // (HDMA) Modified round robin arbiter.
// -------- HDMA_EN : (HDMA Offset: 0x4)  -------- 
#define AT91C_HDMA_ENABLE     (0x1 <<  0) // (HDMA) 
#define 	AT91C_HDMA_ENABLE_DISABLE              (0x0) // (HDMA) Disables HDMA.
#define 	AT91C_HDMA_ENABLE_ENABLE               (0x1) // (HDMA) Enables HDMA.
// -------- HDMA_SREQ : (HDMA Offset: 0x8)  -------- 
#define AT91C_HDMA_SSREQ0     (0x1 <<  0) // (HDMA) Request a source single transfer on channel 0
#define 	AT91C_HDMA_SSREQ0_0                    (0x0) // (HDMA) No effect.
#define 	AT91C_HDMA_SSREQ0_1                    (0x1) // (HDMA) Request a source single transfer on channel 0.
#define AT91C_HDMA_DSREQ0     (0x1 <<  1) // (HDMA) Request a destination single transfer on channel 0
#define 	AT91C_HDMA_DSREQ0_0                    (0x0 <<  1) // (HDMA) No effect.
#define 	AT91C_HDMA_DSREQ0_1                    (0x1 <<  1) // (HDMA) Request a destination single transfer on channel 0.
#define AT91C_HDMA_SSREQ1     (0x1 <<  2) // (HDMA) Request a source single transfer on channel 1
#define 	AT91C_HDMA_SSREQ1_0                    (0x0 <<  2) // (HDMA) No effect.
#define 	AT91C_HDMA_SSREQ1_1                    (0x1 <<  2) // (HDMA) Request a source single transfer on channel 1.
#define AT91C_HDMA_DSREQ1     (0x1 <<  3) // (HDMA) Request a destination single transfer on channel 1
#define 	AT91C_HDMA_DSREQ1_0                    (0x0 <<  3) // (HDMA) No effect.
#define 	AT91C_HDMA_DSREQ1_1                    (0x1 <<  3) // (HDMA) Request a destination single transfer on channel 1.
#define AT91C_HDMA_SSREQ2     (0x1 <<  4) // (HDMA) Request a source single transfer on channel 2
#define 	AT91C_HDMA_SSREQ2_0                    (0x0 <<  4) // (HDMA) No effect.
#define 	AT91C_HDMA_SSREQ2_1                    (0x1 <<  4) // (HDMA) Request a source single transfer on channel 2.
#define AT91C_HDMA_DSREQ2     (0x1 <<  5) // (HDMA) Request a destination single transfer on channel 2
#define 	AT91C_HDMA_DSREQ2_0                    (0x0 <<  5) // (HDMA) No effect.
#define 	AT91C_HDMA_DSREQ2_1                    (0x1 <<  5) // (HDMA) Request a destination single transfer on channel 2.
#define AT91C_HDMA_SSREQ3     (0x1 <<  6) // (HDMA) Request a source single transfer on channel 3
#define 	AT91C_HDMA_SSREQ3_0                    (0x0 <<  6) // (HDMA) No effect.
#define 	AT91C_HDMA_SSREQ3_1                    (0x1 <<  6) // (HDMA) Request a source single transfer on channel 3.
#define AT91C_HDMA_DSREQ3     (0x1 <<  7) // (HDMA) Request a destination single transfer on channel 3
#define 	AT91C_HDMA_DSREQ3_0                    (0x0 <<  7) // (HDMA) No effect.
#define 	AT91C_HDMA_DSREQ3_1                    (0x1 <<  7) // (HDMA) Request a destination single transfer on channel 3.
#define AT91C_HDMA_SSREQ4     (0x1 <<  8) // (HDMA) Request a source single transfer on channel 4
#define 	AT91C_HDMA_SSREQ4_0                    (0x0 <<  8) // (HDMA) No effect.
#define 	AT91C_HDMA_SSREQ4_1                    (0x1 <<  8) // (HDMA) Request a source single transfer on channel 4.
#define AT91C_HDMA_DSREQ4     (0x1 <<  9) // (HDMA) Request a destination single transfer on channel 4
#define 	AT91C_HDMA_DSREQ4_0                    (0x0 <<  9) // (HDMA) No effect.
#define 	AT91C_HDMA_DSREQ4_1                    (0x1 <<  9) // (HDMA) Request a destination single transfer on channel 4.
#define AT91C_HDMA_SSREQ5     (0x1 << 10) // (HDMA) Request a source single transfer on channel 5
#define 	AT91C_HDMA_SSREQ5_0                    (0x0 << 10) // (HDMA) No effect.
#define 	AT91C_HDMA_SSREQ5_1                    (0x1 << 10) // (HDMA) Request a source single transfer on channel 5.
#define AT91C_HDMA_DSREQ6     (0x1 << 11) // (HDMA) Request a destination single transfer on channel 5
#define 	AT91C_HDMA_DSREQ6_0                    (0x0 << 11) // (HDMA) No effect.
#define 	AT91C_HDMA_DSREQ6_1                    (0x1 << 11) // (HDMA) Request a destination single transfer on channel 5.
#define AT91C_HDMA_SSREQ6     (0x1 << 12) // (HDMA) Request a source single transfer on channel 6
#define 	AT91C_HDMA_SSREQ6_0                    (0x0 << 12) // (HDMA) No effect.
#define 	AT91C_HDMA_SSREQ6_1                    (0x1 << 12) // (HDMA) Request a source single transfer on channel 6.
#define AT91C_HDMA_SSREQ7     (0x1 << 14) // (HDMA) Request a source single transfer on channel 7
#define 	AT91C_HDMA_SSREQ7_0                    (0x0 << 14) // (HDMA) No effect.
#define 	AT91C_HDMA_SSREQ7_1                    (0x1 << 14) // (HDMA) Request a source single transfer on channel 7.
#define AT91C_HDMA_DSREQ7     (0x1 << 15) // (HDMA) Request a destination single transfer on channel 7
#define 	AT91C_HDMA_DSREQ7_0                    (0x0 << 15) // (HDMA) No effect.
#define 	AT91C_HDMA_DSREQ7_1                    (0x1 << 15) // (HDMA) Request a destination single transfer on channel 7.
// -------- HDMA_CREQ : (HDMA Offset: 0xc)  -------- 
#define AT91C_HDMA_SCREQ0     (0x1 <<  0) // (HDMA) Request a source chunk transfer on channel 0
#define 	AT91C_HDMA_SCREQ0_0                    (0x0) // (HDMA) No effect.
#define 	AT91C_HDMA_SCREQ0_1                    (0x1) // (HDMA) Request a source chunk transfer on channel 0.
#define AT91C_HDMA_DCREQ0     (0x1 <<  1) // (HDMA) Request a destination chunk transfer on channel 0
#define 	AT91C_HDMA_DCREQ0_0                    (0x0 <<  1) // (HDMA) No effect.
#define 	AT91C_HDMA_DCREQ0_1                    (0x1 <<  1) // (HDMA) Request a destination chunk transfer on channel 0.
#define AT91C_HDMA_SCREQ1     (0x1 <<  2) // (HDMA) Request a source chunk transfer on channel 1
#define 	AT91C_HDMA_SCREQ1_0                    (0x0 <<  2) // (HDMA) No effect.
#define 	AT91C_HDMA_SCREQ1_1                    (0x1 <<  2) // (HDMA) Request a source chunk transfer on channel 1.
#define AT91C_HDMA_DCREQ1     (0x1 <<  3) // (HDMA) Request a destination chunk transfer on channel 1
#define 	AT91C_HDMA_DCREQ1_0                    (0x0 <<  3) // (HDMA) No effect.
#define 	AT91C_HDMA_DCREQ1_1                    (0x1 <<  3) // (HDMA) Request a destination chunk transfer on channel 1.
#define AT91C_HDMA_SCREQ2     (0x1 <<  4) // (HDMA) Request a source chunk transfer on channel 2
#define 	AT91C_HDMA_SCREQ2_0                    (0x0 <<  4) // (HDMA) No effect.
#define 	AT91C_HDMA_SCREQ2_1                    (0x1 <<  4) // (HDMA) Request a source chunk transfer on channel 2.
#define AT91C_HDMA_DCREQ2     (0x1 <<  5) // (HDMA) Request a destination chunk transfer on channel 2
#define 	AT91C_HDMA_DCREQ2_0                    (0x0 <<  5) // (HDMA) No effect.
#define 	AT91C_HDMA_DCREQ2_1                    (0x1 <<  5) // (HDMA) Request a destination chunk transfer on channel 2.
#define AT91C_HDMA_SCREQ3     (0x1 <<  6) // (HDMA) Request a source chunk transfer on channel 3
#define 	AT91C_HDMA_SCREQ3_0                    (0x0 <<  6) // (HDMA) No effect.
#define 	AT91C_HDMA_SCREQ3_1                    (0x1 <<  6) // (HDMA) Request a source chunk transfer on channel 3.
#define AT91C_HDMA_DCREQ3     (0x1 <<  7) // (HDMA) Request a destination chunk transfer on channel 3
#define 	AT91C_HDMA_DCREQ3_0                    (0x0 <<  7) // (HDMA) No effect.
#define 	AT91C_HDMA_DCREQ3_1                    (0x1 <<  7) // (HDMA) Request a destination chunk transfer on channel 3.
#define AT91C_HDMA_SCREQ4     (0x1 <<  8) // (HDMA) Request a source chunk transfer on channel 4
#define 	AT91C_HDMA_SCREQ4_0                    (0x0 <<  8) // (HDMA) No effect.
#define 	AT91C_HDMA_SCREQ4_1                    (0x1 <<  8) // (HDMA) Request a source chunk transfer on channel 4.
#define AT91C_HDMA_DCREQ4     (0x1 <<  9) // (HDMA) Request a destination chunk transfer on channel 4
#define 	AT91C_HDMA_DCREQ4_0                    (0x0 <<  9) // (HDMA) No effect.
#define 	AT91C_HDMA_DCREQ4_1                    (0x1 <<  9) // (HDMA) Request a destination chunk transfer on channel 4.
#define AT91C_HDMA_SCREQ5     (0x1 << 10) // (HDMA) Request a source chunk transfer on channel 5
#define 	AT91C_HDMA_SCREQ5_0                    (0x0 << 10) // (HDMA) No effect.
#define 	AT91C_HDMA_SCREQ5_1                    (0x1 << 10) // (HDMA) Request a source chunk transfer on channel 5.
#define AT91C_HDMA_DCREQ6     (0x1 << 11) // (HDMA) Request a destination chunk transfer on channel 5
#define 	AT91C_HDMA_DCREQ6_0                    (0x0 << 11) // (HDMA) No effect.
#define 	AT91C_HDMA_DCREQ6_1                    (0x1 << 11) // (HDMA) Request a destination chunk transfer on channel 5.
#define AT91C_HDMA_SCREQ6     (0x1 << 12) // (HDMA) Request a source chunk transfer on channel 6
#define 	AT91C_HDMA_SCREQ6_0                    (0x0 << 12) // (HDMA) No effect.
#define 	AT91C_HDMA_SCREQ6_1                    (0x1 << 12) // (HDMA) Request a source chunk transfer on channel 6.
#define AT91C_HDMA_SCREQ7     (0x1 << 14) // (HDMA) Request a source chunk transfer on channel 7
#define 	AT91C_HDMA_SCREQ7_0                    (0x0 << 14) // (HDMA) No effect.
#define 	AT91C_HDMA_SCREQ7_1                    (0x1 << 14) // (HDMA) Request a source chunk transfer on channel 7.
#define AT91C_HDMA_DCREQ7     (0x1 << 15) // (HDMA) Request a destination chunk transfer on channel 7
#define 	AT91C_HDMA_DCREQ7_0                    (0x0 << 15) // (HDMA) No effect.
#define 	AT91C_HDMA_DCREQ7_1                    (0x1 << 15) // (HDMA) Request a destination chunk transfer on channel 7.
// -------- HDMA_LAST : (HDMA Offset: 0x10)  -------- 
#define AT91C_HDMA_SLAST0     (0x1 <<  0) // (HDMA) Indicates that this source request is the last transfer of the buffer on channel 0
#define 	AT91C_HDMA_SLAST0_0                    (0x0) // (HDMA) No effect.
#define 	AT91C_HDMA_SLAST0_1                    (0x1) // (HDMA) Writing one to SLASTx prior to writing one to SSREQx or SCREQx indicates that this source request is the last transfer of the buffer on channel 0.
#define AT91C_HDMA_DLAST0     (0x1 <<  1) // (HDMA) Indicates that this destination request is the last transfer of the buffer on channel 0
#define 	AT91C_HDMA_DLAST0_0                    (0x0 <<  1) // (HDMA) No effect.
#define 	AT91C_HDMA_DLAST0_1                    (0x1 <<  1) // (HDMA) Writing one to DLASTx prior to writing one to DSREQx or DCREQx indicates that this destination request is the last transfer of the buffer on channel 0.
#define AT91C_HDMA_SLAST1     (0x1 <<  2) // (HDMA) Indicates that this source request is the last transfer of the buffer on channel 1
#define 	AT91C_HDMA_SLAST1_0                    (0x0 <<  2) // (HDMA) No effect.
#define 	AT91C_HDMA_SLAST1_1                    (0x1 <<  2) // (HDMA) Writing one to SLASTx prior to writing one to SSREQx or SCREQx indicates that this source request is the last transfer of the buffer on channel 1.
#define AT91C_HDMA_DLAST1     (0x1 <<  3) // (HDMA) Indicates that this destination request is the last transfer of the buffer on channel 1
#define 	AT91C_HDMA_DLAST1_0                    (0x0 <<  3) // (HDMA) No effect.
#define 	AT91C_HDMA_DLAST1_1                    (0x1 <<  3) // (HDMA) Writing one to DLASTx prior to writing one to DSREQx or DCREQx indicates that this destination request is the last transfer of the buffer on channel 1.
#define AT91C_HDMA_SLAST2     (0x1 <<  4) // (HDMA) Indicates that this source request is the last transfer of the buffer on channel 2
#define 	AT91C_HDMA_SLAST2_0                    (0x0 <<  4) // (HDMA) No effect.
#define 	AT91C_HDMA_SLAST2_1                    (0x1 <<  4) // (HDMA) Writing one to SLASTx prior to writing one to SSREQx or SCREQx indicates that this source request is the last transfer of the buffer on channel 2.
#define AT91C_HDMA_DLAST2     (0x1 <<  5) // (HDMA) Indicates that this destination request is the last transfer of the buffer on channel 2
#define 	AT91C_HDMA_DLAST2_0                    (0x0 <<  5) // (HDMA) No effect.
#define 	AT91C_HDMA_DLAST2_1                    (0x1 <<  5) // (HDMA) Writing one to DLASTx prior to writing one to DSREQx or DCREQx indicates that this destination request is the last transfer of the buffer on channel 2.
#define AT91C_HDMA_SLAST3     (0x1 <<  6) // (HDMA) Indicates that this source request is the last transfer of the buffer on channel 3
#define 	AT91C_HDMA_SLAST3_0                    (0x0 <<  6) // (HDMA) No effect.
#define 	AT91C_HDMA_SLAST3_1                    (0x1 <<  6) // (HDMA) Writing one to SLASTx prior to writing one to SSREQx or SCREQx indicates that this source request is the last transfer of the buffer on channel 3.
#define AT91C_HDMA_DLAST3     (0x1 <<  7) // (HDMA) Indicates that this destination request is the last transfer of the buffer on channel 3
#define 	AT91C_HDMA_DLAST3_0                    (0x0 <<  7) // (HDMA) No effect.
#define 	AT91C_HDMA_DLAST3_1                    (0x1 <<  7) // (HDMA) Writing one to DLASTx prior to writing one to DSREQx or DCREQx indicates that this destination request is the last transfer of the buffer on channel 3.
#define AT91C_HDMA_SLAST4     (0x1 <<  8) // (HDMA) Indicates that this source request is the last transfer of the buffer on channel 4
#define 	AT91C_HDMA_SLAST4_0                    (0x0 <<  8) // (HDMA) No effect.
#define 	AT91C_HDMA_SLAST4_1                    (0x1 <<  8) // (HDMA) Writing one to SLASTx prior to writing one to SSREQx or SCREQx indicates that this source request is the last transfer of the buffer on channel 4.
#define AT91C_HDMA_DLAST4     (0x1 <<  9) // (HDMA) Indicates that this destination request is the last transfer of the buffer on channel 4
#define 	AT91C_HDMA_DLAST4_0                    (0x0 <<  9) // (HDMA) No effect.
#define 	AT91C_HDMA_DLAST4_1                    (0x1 <<  9) // (HDMA) Writing one to DLASTx prior to writing one to DSREQx or DCREQx indicates that this destination request is the last transfer of the buffer on channel 4.
#define AT91C_HDMA_SLAST5     (0x1 << 10) // (HDMA) Indicates that this source request is the last transfer of the buffer on channel 5
#define 	AT91C_HDMA_SLAST5_0                    (0x0 << 10) // (HDMA) No effect.
#define 	AT91C_HDMA_SLAST5_1                    (0x1 << 10) // (HDMA) Writing one to SLASTx prior to writing one to SSREQx or SCREQx indicates that this source request is the last transfer of the buffer on channel 5.
#define AT91C_HDMA_DLAST6     (0x1 << 11) // (HDMA) Indicates that this destination request is the last transfer of the buffer on channel 5
#define 	AT91C_HDMA_DLAST6_0                    (0x0 << 11) // (HDMA) No effect.
#define 	AT91C_HDMA_DLAST6_1                    (0x1 << 11) // (HDMA) Writing one to DLASTx prior to writing one to DSREQx or DCREQx indicates that this destination request is the last transfer of the buffer on channel 5.
#define AT91C_HDMA_SLAST6     (0x1 << 12) // (HDMA) Indicates that this source request is the last transfer of the buffer on channel 6
#define 	AT91C_HDMA_SLAST6_0                    (0x0 << 12) // (HDMA) No effect.
#define 	AT91C_HDMA_SLAST6_1                    (0x1 << 12) // (HDMA) Writing one to SLASTx prior to writing one to SSREQx or SCREQx indicates that this source request is the last transfer of the buffer on channel 6.
#define AT91C_HDMA_SLAST7     (0x1 << 14) // (HDMA) Indicates that this source request is the last transfer of the buffer on channel 7
#define 	AT91C_HDMA_SLAST7_0                    (0x0 << 14) // (HDMA) No effect.
#define 	AT91C_HDMA_SLAST7_1                    (0x1 << 14) // (HDMA) Writing one to SLASTx prior to writing one to SSREQx or SCREQx indicates that this source request is the last transfer of the buffer on channel 7.
#define AT91C_HDMA_DLAST7     (0x1 << 15) // (HDMA) Indicates that this destination request is the last transfer of the buffer on channel 7
#define 	AT91C_HDMA_DLAST7_0                    (0x0 << 15) // (HDMA) No effect.
#define 	AT91C_HDMA_DLAST7_1                    (0x1 << 15) // (HDMA) Writing one to DLASTx prior to writing one to DSREQx or DCREQx indicates that this destination request is the last transfer of the buffer on channel 7.
// -------- HDMA_SYNC : (HDMA Offset: 0x14)  -------- 
#define AT91C_SYNC_REQ        (0xFFFF <<  0) // (HDMA) 
// -------- HDMA_EBCIER : (HDMA Offset: 0x18) Buffer Transfer Completed/Chained Buffer Transfer Completed/Access Error Interrupt Enable Register -------- 
#define AT91C_HDMA_BTC0       (0x1 <<  0) // (HDMA) Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_BTC1       (0x1 <<  1) // (HDMA) Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_BTC2       (0x1 <<  2) // (HDMA) Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_BTC3       (0x1 <<  3) // (HDMA) Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_BTC4       (0x1 <<  4) // (HDMA) Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_BTC5       (0x1 <<  5) // (HDMA) Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_BTC6       (0x1 <<  6) // (HDMA) Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_BTC7       (0x1 <<  7) // (HDMA) Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_CBTC0      (0x1 <<  8) // (HDMA) Chained Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_CBTC1      (0x1 <<  9) // (HDMA) Chained Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_CBTC2      (0x1 << 10) // (HDMA) Chained Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_CBTC3      (0x1 << 11) // (HDMA) Chained Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_CBTC4      (0x1 << 12) // (HDMA) Chained Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_CBTC5      (0x1 << 13) // (HDMA) Chained Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_CBTC6      (0x1 << 14) // (HDMA) Chained Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_CBTC7      (0x1 << 15) // (HDMA) Chained Buffer Transfer Completed Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_ERR0       (0x1 << 16) // (HDMA) Access HDMA_Error Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_ERR1       (0x1 << 17) // (HDMA) Access HDMA_Error Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_ERR2       (0x1 << 18) // (HDMA) Access HDMA_Error Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_ERR3       (0x1 << 19) // (HDMA) Access HDMA_Error Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_ERR4       (0x1 << 20) // (HDMA) Access HDMA_Error Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_ERR5       (0x1 << 21) // (HDMA) Access HDMA_Error Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_ERR6       (0x1 << 22) // (HDMA) Access HDMA_Error Interrupt Enable/Disable/Status Register
#define AT91C_HDMA_ERR7       (0x1 << 23) // (HDMA) Access HDMA_Error Interrupt Enable/Disable/Status Register
// -------- HDMA_EBCIDR : (HDMA Offset: 0x1c)  -------- 
// -------- HDMA_EBCIMR : (HDMA Offset: 0x20)  -------- 
// -------- HDMA_EBCISR : (HDMA Offset: 0x24)  -------- 
// -------- HDMA_CHER : (HDMA Offset: 0x28)  -------- 
#define AT91C_HDMA_ENA0       (0x1 <<  0) // (HDMA) When set, channel 0 enabled.
#define 	AT91C_HDMA_ENA0_0                    (0x0) // (HDMA) No effect.
#define 	AT91C_HDMA_ENA0_1                    (0x1) // (HDMA) Channel 0 enabled.
#define AT91C_HDMA_ENA1       (0x1 <<  1) // (HDMA) When set, channel 1 enabled.
#define 	AT91C_HDMA_ENA1_0                    (0x0 <<  1) // (HDMA) No effect.
#define 	AT91C_HDMA_ENA1_1                    (0x1 <<  1) // (HDMA) Channel 1 enabled.
#define AT91C_HDMA_ENA2       (0x1 <<  2) // (HDMA) When set, channel 2 enabled.
#define 	AT91C_HDMA_ENA2_0                    (0x0 <<  2) // (HDMA) No effect.
#define 	AT91C_HDMA_ENA2_1                    (0x1 <<  2) // (HDMA) Channel 2 enabled.
#define AT91C_HDMA_ENA3       (0x1 <<  3) // (HDMA) When set, channel 3 enabled.
#define 	AT91C_HDMA_ENA3_0                    (0x0 <<  3) // (HDMA) No effect.
#define 	AT91C_HDMA_ENA3_1                    (0x1 <<  3) // (HDMA) Channel 3 enabled.
#define AT91C_HDMA_ENA4       (0x1 <<  4) // (HDMA) When set, channel 4 enabled.
#define 	AT91C_HDMA_ENA4_0                    (0x0 <<  4) // (HDMA) No effect.
#define 	AT91C_HDMA_ENA4_1                    (0x1 <<  4) // (HDMA) Channel 4 enabled.
#define AT91C_HDMA_ENA5       (0x1 <<  5) // (HDMA) When set, channel 5 enabled.
#define 	AT91C_HDMA_ENA5_0                    (0x0 <<  5) // (HDMA) No effect.
#define 	AT91C_HDMA_ENA5_1                    (0x1 <<  5) // (HDMA) Channel 5 enabled.
#define AT91C_HDMA_ENA6       (0x1 <<  6) // (HDMA) When set, channel 6 enabled.
#define 	AT91C_HDMA_ENA6_0                    (0x0 <<  6) // (HDMA) No effect.
#define 	AT91C_HDMA_ENA6_1                    (0x1 <<  6) // (HDMA) Channel 6 enabled.
#define AT91C_HDMA_ENA7       (0x1 <<  7) // (HDMA) When set, channel 7 enabled.
#define 	AT91C_HDMA_ENA7_0                    (0x0 <<  7) // (HDMA) No effect.
#define 	AT91C_HDMA_ENA7_1                    (0x1 <<  7) // (HDMA) Channel 7 enabled.
#define AT91C_HDMA_SUSP0      (0x1 <<  8) // (HDMA) When set, channel 0 freezed and its current context.
#define 	AT91C_HDMA_SUSP0_0                    (0x0 <<  8) // (HDMA) No effect.
#define 	AT91C_HDMA_SUSP0_1                    (0x1 <<  8) // (HDMA) Channel 0 freezed.
#define AT91C_HDMA_SUSP1      (0x1 <<  9) // (HDMA) When set, channel 1 freezed and its current context.
#define 	AT91C_HDMA_SUSP1_0                    (0x0 <<  9) // (HDMA) No effect.
#define 	AT91C_HDMA_SUSP1_1                    (0x1 <<  9) // (HDMA) Channel 1 freezed.
#define AT91C_HDMA_SUSP2      (0x1 << 10) // (HDMA) When set, channel 2 freezed and its current context.
#define 	AT91C_HDMA_SUSP2_0                    (0x0 << 10) // (HDMA) No effect.
#define 	AT91C_HDMA_SUSP2_1                    (0x1 << 10) // (HDMA) Channel 2 freezed.
#define AT91C_HDMA_SUSP3      (0x1 << 11) // (HDMA) When set, channel 3 freezed and its current context.
#define 	AT91C_HDMA_SUSP3_0                    (0x0 << 11) // (HDMA) No effect.
#define 	AT91C_HDMA_SUSP3_1                    (0x1 << 11) // (HDMA) Channel 3 freezed.
#define AT91C_HDMA_SUSP4      (0x1 << 12) // (HDMA) When set, channel 4 freezed and its current context.
#define 	AT91C_HDMA_SUSP4_0                    (0x0 << 12) // (HDMA) No effect.
#define 	AT91C_HDMA_SUSP4_1                    (0x1 << 12) // (HDMA) Channel 4 freezed.
#define AT91C_HDMA_SUSP5      (0x1 << 13) // (HDMA) When set, channel 5 freezed and its current context.
#define 	AT91C_HDMA_SUSP5_0                    (0x0 << 13) // (HDMA) No effect.
#define 	AT91C_HDMA_SUSP5_1                    (0x1 << 13) // (HDMA) Channel 5 freezed.
#define AT91C_HDMA_SUSP6      (0x1 << 14) // (HDMA) When set, channel 6 freezed and its current context.
#define 	AT91C_HDMA_SUSP6_0                    (0x0 << 14) // (HDMA) No effect.
#define 	AT91C_HDMA_SUSP6_1                    (0x1 << 14) // (HDMA) Channel 6 freezed.
#define AT91C_HDMA_SUSP7      (0x1 << 15) // (HDMA) When set, channel 7 freezed and its current context.
#define 	AT91C_HDMA_SUSP7_0                    (0x0 << 15) // (HDMA) No effect.
#define 	AT91C_HDMA_SUSP7_1                    (0x1 << 15) // (HDMA) Channel 7 freezed.
#define AT91C_HDMA_KEEP0      (0x1 << 24) // (HDMA) When set, it resumes the channel 0 from an automatic stall state.
#define 	AT91C_HDMA_KEEP0_0                    (0x0 << 24) // (HDMA) No effect.
#define 	AT91C_HDMA_KEEP0_1                    (0x1 << 24) // (HDMA) Resumes the channel 0.
#define AT91C_HDMA_KEEP1      (0x1 << 25) // (HDMA) When set, it resumes the channel 1 from an automatic stall state.
#define 	AT91C_HDMA_KEEP1_0                    (0x0 << 25) // (HDMA) No effect.
#define 	AT91C_HDMA_KEEP1_1                    (0x1 << 25) // (HDMA) Resumes the channel 1.
#define AT91C_HDMA_KEEP2      (0x1 << 26) // (HDMA) When set, it resumes the channel 2 from an automatic stall state.
#define 	AT91C_HDMA_KEEP2_0                    (0x0 << 26) // (HDMA) No effect.
#define 	AT91C_HDMA_KEEP2_1                    (0x1 << 26) // (HDMA) Resumes the channel 2.
#define AT91C_HDMA_KEEP3      (0x1 << 27) // (HDMA) When set, it resumes the channel 3 from an automatic stall state.
#define 	AT91C_HDMA_KEEP3_0                    (0x0 << 27) // (HDMA) No effect.
#define 	AT91C_HDMA_KEEP3_1                    (0x1 << 27) // (HDMA) Resumes the channel 3.
#define AT91C_HDMA_KEEP4      (0x1 << 28) // (HDMA) When set, it resumes the channel 4 from an automatic stall state.
#define 	AT91C_HDMA_KEEP4_0                    (0x0 << 28) // (HDMA) No effect.
#define 	AT91C_HDMA_KEEP4_1                    (0x1 << 28) // (HDMA) Resumes the channel 4.
#define AT91C_HDMA_KEEP5      (0x1 << 29) // (HDMA) When set, it resumes the channel 5 from an automatic stall state.
#define 	AT91C_HDMA_KEEP5_0                    (0x0 << 29) // (HDMA) No effect.
#define 	AT91C_HDMA_KEEP5_1                    (0x1 << 29) // (HDMA) Resumes the channel 5.
#define AT91C_HDMA_KEEP6      (0x1 << 30) // (HDMA) When set, it resumes the channel 6 from an automatic stall state.
#define 	AT91C_HDMA_KEEP6_0                    (0x0 << 30) // (HDMA) No effect.
#define 	AT91C_HDMA_KEEP6_1                    (0x1 << 30) // (HDMA) Resumes the channel 6.
#define AT91C_HDMA_KEEP7      (0x1 << 31) // (HDMA) When set, it resumes the channel 7 from an automatic stall state.
#define 	AT91C_HDMA_KEEP7_0                    (0x0 << 31) // (HDMA) No effect.
#define 	AT91C_HDMA_KEEP7_1                    (0x1 << 31) // (HDMA) Resumes the channel 7.
// -------- HDMA_CHDR : (HDMA Offset: 0x2c)  -------- 
#define AT91C_HDMA_DIS0       (0x1 <<  0) // (HDMA) Write one to this field to disable the channel 0.
#define 	AT91C_HDMA_DIS0_0                    (0x0) // (HDMA) No effect.
#define 	AT91C_HDMA_DIS0_1                    (0x1) // (HDMA) Disables the channel 0.
#define AT91C_HDMA_DIS1       (0x1 <<  1) // (HDMA) Write one to this field to disable the channel 1.
#define 	AT91C_HDMA_DIS1_0                    (0x0 <<  1) // (HDMA) No effect.
#define 	AT91C_HDMA_DIS1_1                    (0x1 <<  1) // (HDMA) Disables the channel 1.
#define AT91C_HDMA_DIS2       (0x1 <<  2) // (HDMA) Write one to this field to disable the channel 2.
#define 	AT91C_HDMA_DIS2_0                    (0x0 <<  2) // (HDMA) No effect.
#define 	AT91C_HDMA_DIS2_1                    (0x1 <<  2) // (HDMA) Disables the channel 2.
#define AT91C_HDMA_DIS3       (0x1 <<  3) // (HDMA) Write one to this field to disable the channel 3.
#define 	AT91C_HDMA_DIS3_0                    (0x0 <<  3) // (HDMA) No effect.
#define 	AT91C_HDMA_DIS3_1                    (0x1 <<  3) // (HDMA) Disables the channel 3.
#define AT91C_HDMA_DIS4       (0x1 <<  4) // (HDMA) Write one to this field to disable the channel 4.
#define 	AT91C_HDMA_DIS4_0                    (0x0 <<  4) // (HDMA) No effect.
#define 	AT91C_HDMA_DIS4_1                    (0x1 <<  4) // (HDMA) Disables the channel 4.
#define AT91C_HDMA_DIS5       (0x1 <<  5) // (HDMA) Write one to this field to disable the channel 5.
#define 	AT91C_HDMA_DIS5_0                    (0x0 <<  5) // (HDMA) No effect.
#define 	AT91C_HDMA_DIS5_1                    (0x1 <<  5) // (HDMA) Disables the channel 5.
#define AT91C_HDMA_DIS6       (0x1 <<  6) // (HDMA) Write one to this field to disable the channel 6.
#define 	AT91C_HDMA_DIS6_0                    (0x0 <<  6) // (HDMA) No effect.
#define 	AT91C_HDMA_DIS6_1                    (0x1 <<  6) // (HDMA) Disables the channel 6.
#define AT91C_HDMA_DIS7       (0x1 <<  7) // (HDMA) Write one to this field to disable the channel 7.
#define 	AT91C_HDMA_DIS7_0                    (0x0 <<  7) // (HDMA) No effect.
#define 	AT91C_HDMA_DIS7_1                    (0x1 <<  7) // (HDMA) Disables the channel 7.
#define AT91C_HDMA_RES0       (0x1 <<  8) // (HDMA) Write one to this field to resume the channel 0 transfer restoring its context.
#define 	AT91C_HDMA_RES0_0                    (0x0 <<  8) // (HDMA) No effect.
#define 	AT91C_HDMA_RES0_1                    (0x1 <<  8) // (HDMA) Resumes the channel 0.
#define AT91C_HDMA_RES1       (0x1 <<  9) // (HDMA) Write one to this field to resume the channel 1 transfer restoring its context.
#define 	AT91C_HDMA_RES1_0                    (0x0 <<  9) // (HDMA) No effect.
#define 	AT91C_HDMA_RES1_1                    (0x1 <<  9) // (HDMA) Resumes the channel 1.
#define AT91C_HDMA_RES2       (0x1 << 10) // (HDMA) Write one to this field to resume the channel 2 transfer restoring its context.
#define 	AT91C_HDMA_RES2_0                    (0x0 << 10) // (HDMA) No effect.
#define 	AT91C_HDMA_RES2_1                    (0x1 << 10) // (HDMA) Resumes the channel 2.
#define AT91C_HDMA_RES3       (0x1 << 11) // (HDMA) Write one to this field to resume the channel 3 transfer restoring its context.
#define 	AT91C_HDMA_RES3_0                    (0x0 << 11) // (HDMA) No effect.
#define 	AT91C_HDMA_RES3_1                    (0x1 << 11) // (HDMA) Resumes the channel 3.
#define AT91C_HDMA_RES4       (0x1 << 12) // (HDMA) Write one to this field to resume the channel 4 transfer restoring its context.
#define 	AT91C_HDMA_RES4_0                    (0x0 << 12) // (HDMA) No effect.
#define 	AT91C_HDMA_RES4_1                    (0x1 << 12) // (HDMA) Resumes the channel 4.
#define AT91C_HDMA_RES5       (0x1 << 13) // (HDMA) Write one to this field to resume the channel 5 transfer restoring its context.
#define 	AT91C_HDMA_RES5_0                    (0x0 << 13) // (HDMA) No effect.
#define 	AT91C_HDMA_RES5_1                    (0x1 << 13) // (HDMA) Resumes the channel 5.
#define AT91C_HDMA_RES6       (0x1 << 14) // (HDMA) Write one to this field to resume the channel 6 transfer restoring its context.
#define 	AT91C_HDMA_RES6_0                    (0x0 << 14) // (HDMA) No effect.
#define 	AT91C_HDMA_RES6_1                    (0x1 << 14) // (HDMA) Resumes the channel 6.
#define AT91C_HDMA_RES7       (0x1 << 15) // (HDMA) Write one to this field to resume the channel 7 transfer restoring its context.
#define 	AT91C_HDMA_RES7_0                    (0x0 << 15) // (HDMA) No effect.
#define 	AT91C_HDMA_RES7_1                    (0x1 << 15) // (HDMA) Resumes the channel 7.
// -------- HDMA_CHSR : (HDMA Offset: 0x30)  -------- 
#define AT91C_HDMA_EMPT0      (0x1 << 16) // (HDMA) When set, channel 0 is empty.
#define 	AT91C_HDMA_EMPT0_0                    (0x0 << 16) // (HDMA) No effect.
#define 	AT91C_HDMA_EMPT0_1                    (0x1 << 16) // (HDMA) Channel 0 empty.
#define AT91C_HDMA_EMPT1      (0x1 << 17) // (HDMA) When set, channel 1 is empty.
#define 	AT91C_HDMA_EMPT1_0                    (0x0 << 17) // (HDMA) No effect.
#define 	AT91C_HDMA_EMPT1_1                    (0x1 << 17) // (HDMA) Channel 1 empty.
#define AT91C_HDMA_EMPT2      (0x1 << 18) // (HDMA) When set, channel 2 is empty.
#define 	AT91C_HDMA_EMPT2_0                    (0x0 << 18) // (HDMA) No effect.
#define 	AT91C_HDMA_EMPT2_1                    (0x1 << 18) // (HDMA) Channel 2 empty.
#define AT91C_HDMA_EMPT3      (0x1 << 19) // (HDMA) When set, channel 3 is empty.
#define 	AT91C_HDMA_EMPT3_0                    (0x0 << 19) // (HDMA) No effect.
#define 	AT91C_HDMA_EMPT3_1                    (0x1 << 19) // (HDMA) Channel 3 empty.
#define AT91C_HDMA_EMPT4      (0x1 << 20) // (HDMA) When set, channel 4 is empty.
#define 	AT91C_HDMA_EMPT4_0                    (0x0 << 20) // (HDMA) No effect.
#define 	AT91C_HDMA_EMPT4_1                    (0x1 << 20) // (HDMA) Channel 4 empty.
#define AT91C_HDMA_EMPT5      (0x1 << 21) // (HDMA) When set, channel 5 is empty.
#define 	AT91C_HDMA_EMPT5_0                    (0x0 << 21) // (HDMA) No effect.
#define 	AT91C_HDMA_EMPT5_1                    (0x1 << 21) // (HDMA) Channel 5 empty.
#define AT91C_HDMA_EMPT6      (0x1 << 22) // (HDMA) When set, channel 6 is empty.
#define 	AT91C_HDMA_EMPT6_0                    (0x0 << 22) // (HDMA) No effect.
#define 	AT91C_HDMA_EMPT6_1                    (0x1 << 22) // (HDMA) Channel 6 empty.
#define AT91C_HDMA_EMPT7      (0x1 << 23) // (HDMA) When set, channel 7 is empty.
#define 	AT91C_HDMA_EMPT7_0                    (0x0 << 23) // (HDMA) No effect.
#define 	AT91C_HDMA_EMPT7_1                    (0x1 << 23) // (HDMA) Channel 7 empty.
#define AT91C_HDMA_STAL0      (0x1 << 24) // (HDMA) When set, channel 0 is stalled.
#define 	AT91C_HDMA_STAL0_0                    (0x0 << 24) // (HDMA) No effect.
#define 	AT91C_HDMA_STAL0_1                    (0x1 << 24) // (HDMA) Channel 0 stalled.
#define AT91C_HDMA_STAL1      (0x1 << 25) // (HDMA) When set, channel 1 is stalled.
#define 	AT91C_HDMA_STAL1_0                    (0x0 << 25) // (HDMA) No effect.
#define 	AT91C_HDMA_STAL1_1                    (0x1 << 25) // (HDMA) Channel 1 stalled.
#define AT91C_HDMA_STAL2      (0x1 << 26) // (HDMA) When set, channel 2 is stalled.
#define 	AT91C_HDMA_STAL2_0                    (0x0 << 26) // (HDMA) No effect.
#define 	AT91C_HDMA_STAL2_1                    (0x1 << 26) // (HDMA) Channel 2 stalled.
#define AT91C_HDMA_STAL3      (0x1 << 27) // (HDMA) When set, channel 3 is stalled.
#define 	AT91C_HDMA_STAL3_0                    (0x0 << 27) // (HDMA) No effect.
#define 	AT91C_HDMA_STAL3_1                    (0x1 << 27) // (HDMA) Channel 3 stalled.
#define AT91C_HDMA_STAL4      (0x1 << 28) // (HDMA) When set, channel 4 is stalled.
#define 	AT91C_HDMA_STAL4_0                    (0x0 << 28) // (HDMA) No effect.
#define 	AT91C_HDMA_STAL4_1                    (0x1 << 28) // (HDMA) Channel 4 stalled.
#define AT91C_HDMA_STAL5      (0x1 << 29) // (HDMA) When set, channel 5 is stalled.
#define 	AT91C_HDMA_STAL5_0                    (0x0 << 29) // (HDMA) No effect.
#define 	AT91C_HDMA_STAL5_1                    (0x1 << 29) // (HDMA) Channel 5 stalled.
#define AT91C_HDMA_STAL6      (0x1 << 30) // (HDMA) When set, channel 6 is stalled.
#define 	AT91C_HDMA_STAL6_0                    (0x0 << 30) // (HDMA) No effect.
#define 	AT91C_HDMA_STAL6_1                    (0x1 << 30) // (HDMA) Channel 6 stalled.
#define AT91C_HDMA_STAL7      (0x1 << 31) // (HDMA) When set, channel 7 is stalled.
#define 	AT91C_HDMA_STAL7_0                    (0x0 << 31) // (HDMA) No effect.
#define 	AT91C_HDMA_STAL7_1                    (0x1 << 31) // (HDMA) Channel 7 stalled.
// -------- HDMA_RSVD : (HDMA Offset: 0x34)  -------- 
// -------- HDMA_RSVD : (HDMA Offset: 0x38)  -------- 

#endif /* __DMA_H__ */
