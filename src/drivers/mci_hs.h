/*
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */

#ifndef __MCI_HS_H__
#define __MCI_HS_H__

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "types.h"
#include "board.h"

//------------------------------------------------------------------------------
//         Constants
//------------------------------------------------------------------------------

#define MCI_STATUS_PENDING      1		/// Transfer is pending.
#define MCI_STATUS_ERROR        2		/// Transfer has been aborted because an error occured.
#define MCI_STATUS_NORESPONSE   3		/// Card did not answer command.

#define MCI_ERROR_LOCK					1		/// MCI driver is currently in use.
#define MCI_NEW_TRANSFER				0		/// Start new data transfer
#define MCI_CONTINUE_TRANSFER		1		/// Continue data transfer
#define MCI_STOP_TRANSFER				2		/// Stop data transfer

/// MCI configuration with 4-bit data bus on slot A (for SD cards).
#define MCI_SD_SLOTA            (AT91C_MCI_SCDSEL_SLOTA | AT91C_MCI_SCDBUS_4BITS)

/// MCI SD Bus Width 1-bit
#define MCI_SDCBUS_1BIT (0 << 7)
/// MCI SD Bus Width 4-bit
#define MCI_SDCBUS_4BIT (1 << 7)
/// MCI SD Bus Width 8-bit
#define MCI_SDCBUS_8BIT (3 << 6)

/// The MCI Clock Speed after initialize (400K)
#define MCI_INITIAL_SPEED       400000
#define MCI_INTERRUPT_MODE 	0
#define MCI_POLLING_MODE		1

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

typedef void (*MciCallback)(U8 status, void *pCommand);		/// MCI end-of-transfer callback function.

//------------------------------------------------------------------------------
/// MCI Transfer Request prepared by the application upper layer. This structure
/// is sent to the MCI_SendCommand function to start the transfer. At the end of
/// the transfer, the callback is invoked by the interrupt handler.
//------------------------------------------------------------------------------
typedef struct _MciCmd {
	U32						cmd;				/// Command code.
	U32						arg;				/// Command argument.
	U8						*pData;			/// Data buffer, the buffer can be 1, 2 or 4 bytes aligned.
	U16						blockSize;	/// Size of data block in bytes.
	U16						nbBlock;		/// Number of blocks to be transfered
	U32						*pResp;			/// Response buffer.
	MciCallback		callback;		/// Optional user-provided callback function.
	void					*pArg;			/// Optional argument to the callback function.
	U8						resType;		/// SD card response type.
	U8						dataTran;		/// Indicate if there is data transfer
	U8						tranType;		/// Indicate if continue to transfer data
	U8						isRead;			/// Indicates if the command is a read operation
	volatile S32	status;			/// Command status.
} MciCmd;

//------------------------------------------------------------------------------
/// MCI driver structure. Holds the internal state of the MCI driver and
/// prevents parallel access to a MCI peripheral.
//------------------------------------------------------------------------------
typedef struct {
//	MciCmd				*pCommand;	/// Pointer to currently executing command.
	U8						mciMode;		/// MCI HW mode
	volatile S8		semaphore;	/// Mutex.
	U32						bPolling;		/// interrupt or polling mode
} Mci;

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

void MCI_Init(void);
void MCI_Close(void);
U32 MCI_GetSpeed(U32 *mciDiv);
U32 MCI_SetSpeed(U32 mciSpeed, U32 mciLimit, U32 mck);
void MCI_EnableHsMode(U8 hsEnable);
void MCI_SetBusWidth(U8 busWidth);
U8 MCI_SendCommand(void);
U8 MCI_CheckBusy(void);
void MCI_Handler(void);
U8 MCI_IsTxComplete(void);

// -------- MCI_CR : (MCI Offset: 0x0) MCI Control Register -------- 
#define AT91C_MCI_MCIEN				(0x1 <<  0) // (MCI) Multimedia Interface Enable
#define AT91C_MCI_MCIEN_0			(0x0) // (MCI) No effect
#define AT91C_MCI_MCIEN_1			(0x1) // (MCI) Enable the MultiMedia Interface if MCIDIS is 0
#define AT91C_MCI_MCIDIS			(0x1 <<  1) // (MCI) Multimedia Interface Disable
#define AT91C_MCI_MCIDIS_0		(0x0 <<  1) // (MCI) No effect
#define AT91C_MCI_MCIDIS_1		(0x1 <<  1) // (MCI) Disable the MultiMedia Interface
#define AT91C_MCI_PWSEN				(0x1 <<  2) // (MCI) Power Save Mode Enable
#define AT91C_MCI_PWSEN_0			(0x0 <<  2) // (MCI) No effect
#define AT91C_MCI_PWSEN_1			(0x1 <<  2) // (MCI) Enable the Power-saving mode if PWSDIS is 0.
#define AT91C_MCI_PWSDIS			(0x1 <<  3) // (MCI) Power Save Mode Disable
#define AT91C_MCI_PWSDIS_0		(0x0 <<  3) // (MCI) No effect
#define AT91C_MCI_PWSDIS_1		(0x1 <<  3) // (MCI) Disable the Power-saving mode.
#define AT91C_MCI_IOWAITEN		(0x1 <<  4) // (MCI) SDIO Read Wait Enable
#define AT91C_MCI_IOWAITEN_0	(0x0 <<  4) // (MCI) No effect
#define AT91C_MCI_IOWAITEN_1	(0x1 <<  4) // (MCI) Enables the SDIO Read Wait Operation.
#define AT91C_MCI_IOWAITDIS		(0x1 <<  5) // (MCI) SDIO Read Wait Disable
#define AT91C_MCI_IOWAITDIS_0	(0x0 <<  5) // (MCI) No effect
#define AT91C_MCI_IOWAITDIS_1	(0x1 <<  5) // (MCI) Disables the SDIO Read Wait Operation.
#define AT91C_MCI_SWRST				(0x1 <<  7) // (MCI) MCI Software reset
#define AT91C_MCI_SWRST_0			(0x0 <<  7) // (MCI) No effect
#define AT91C_MCI_SWRST_1			(0x1 <<  7) // (MCI) Resets the MCI
// -------- MCI_MR : (MCI Offset: 0x4) MCI Mode Register -------- 
#define AT91C_MCI_CLKDIV						(0xFF << 0) // (MCI) Clock Divider
#define AT91C_MCI_PWSDIV						(0x7 <<  8) // (MCI) Power Saving Divider
#define AT91C_MCI_RDPROOF						(0x1 << 11) // (MCI) Read Proof Enable
#define AT91C_MCI_RDPROOF_DISABLE 	(0x0 << 11) // (MCI) Disables Read Proof
#define AT91C_MCI_RDPROOF_ENABLE		(0x1 << 11) // (MCI) Enables Read Proof
#define AT91C_MCI_WRPROOF						(0x1 << 12) // (MCI) Write Proof Enable
#define AT91C_MCI_WRPROOF_DISABLE		(0x0 << 12) // (MCI) Disables Write Proof
#define AT91C_MCI_WRPROOF_ENABLE		(0x1 << 12) // (MCI) Enables Write Proof
#define AT91C_MCI_PDCFBYTE					(0x1 << 13) // (MCI) PDC Force Byte Transfer
#define AT91C_MCI_PDCFBYTE_DISABLE	(0x0 << 13) // (MCI) Disables PDC Force Byte Transfer
#define AT91C_MCI_PDCFBYTE_ENABLE		(0x1 << 13) // (MCI) Enables PDC Force Byte Transfer
#define AT91C_MCI_PDCPADV						(0x1 << 14) // (MCI) PDC Padding Value
#define AT91C_MCI_PDCMODE						(0x1 << 15) // (MCI) PDC Oriented Mode
#define AT91C_MCI_PDCMODE_DISABLE		(0x0 << 15) // (MCI) Disables PDC Transfer
#define AT91C_MCI_PDCMODE_ENABLE		(0x1 << 15) // (MCI) Enables PDC Transfer
#define AT91C_MCI_BLKLEN						(0xFFFF0000) //(0xFFFF << 16) // (MCI) Data Block Length
// -------- MCI_DTOR : (MCI Offset: 0x8) MCI Data Timeout Register -------- 
#define AT91C_MCI_DTOCYC      			(0xF <<  0) // (MCI) Data Timeout Cycle Number
#define AT91C_MCI_DTOMUL      			(0x7 <<  4) // (MCI) Data Timeout Multiplier
#define AT91C_MCI_DTOMUL_1    			(0x0 <<  4) // (MCI) DTOCYC x 1
#define AT91C_MCI_DTOMUL_16    			(0x1 <<  4) // (MCI) DTOCYC x 16
#define AT91C_MCI_DTOMUL_128				(0x2 <<  4) // (MCI) DTOCYC x 128
#define AT91C_MCI_DTOMUL_256				(0x3 <<  4) // (MCI) DTOCYC x 256
#define AT91C_MCI_DTOMUL_1024				(0x4 <<  4) // (MCI) DTOCYC x 1024
#define AT91C_MCI_DTOMUL_4096				(0x5 <<  4) // (MCI) DTOCYC x 4096
#define AT91C_MCI_DTOMUL_65536			(0x6 <<  4) // (MCI) DTOCYC x 65536
#define AT91C_MCI_DTOMUL_1048576		(0x7 <<  4) // (MCI) DTOCYC x 1048576
// -------- MCI_SDCR : (MCI Offset: 0xc) MCI SD Card Register -------- 
#define AT91C_MCI_SCDSEL      		(0x3 <<  0) // (MCI) SD Card/SDIO Selector
#define AT91C_MCI_SCDSEL_SLOTA		(0x0) // (MCI) Slot A selected
#define AT91C_MCI_SCDSEL_SLOTB		(0x1) // (MCI) Slot B selected
#define AT91C_MCI_SCDSEL_SLOTC		(0x2) // (MCI) Slot C selected
#define AT91C_MCI_SCDSEL_SLOTD		(0x3) // (MCI) Slot D selected
#define AT91C_MCI_SCDBUS					(0x3 <<  6) // (MCI) SDCard/SDIO Bus Width
#define AT91C_MCI_SCDBUS_1BIT			(0x0 <<  6) // (MCI) 1-bit data bus
#define AT91C_MCI_SCDBUS_4BITS		(0x2 <<  6) // (MCI) 4-bits data bus
// -------- MCI_CMDR : (MCI Offset: 0x14) MCI Command Register -------- 
#define AT91C_MCI_CMDNB       (0x3F <<  0) // (MCI) Command Number
#define AT91C_MCI_RSPTYP      (0x3 <<  6) // (MCI) Response Type
#define AT91C_MCI_RSPTYP_NO                   (0x0 <<  6) // (MCI) No response
#define AT91C_MCI_RSPTYP_48                   (0x1 <<  6) // (MCI) 48-bit response
#define AT91C_MCI_RSPTYP_136                  (0x2 <<  6) // (MCI) 136-bit response
#define AT91C_MCI_RSPTYP_R1B                  (0x3 <<  6) // (MCI) R1b response
#define AT91C_MCI_SPCMD       (0x7 <<  8) // (MCI) Special CMD
#define AT91C_MCI_SPCMD_NONE                 (0x0 <<  8) // (MCI) Not a special CMD
#define AT91C_MCI_SPCMD_INIT                 (0x1 <<  8) // (MCI) Initialization CMD
#define AT91C_MCI_SPCMD_SYNC                 (0x2 <<  8) // (MCI) Synchronized CMD
#define AT91C_MCI_SPCMD_CE_ATA               (0x3 <<  8) // (MCI) CE-ATA Completion Signal disable CMD
#define AT91C_MCI_SPCMD_IT_CMD               (0x4 <<  8) // (MCI) Interrupt command
#define AT91C_MCI_SPCMD_IT_REP               (0x5 <<  8) // (MCI) Interrupt response
#define AT91C_MCI_OPDCMD      (0x1 << 11) // (MCI) Open Drain Command
#define AT91C_MCI_OPDCMD_PUSHPULL             (0x0 << 11) // (MCI) Push/pull command
#define AT91C_MCI_OPDCMD_OPENDRAIN            (0x1 << 11) // (MCI) Open drain command
#define AT91C_MCI_MAXLAT      (0x1 << 12) // (MCI) Maximum Latency for Command to respond
#define AT91C_MCI_MAXLAT_5                    (0x0 << 12) // (MCI) 5 cycles maximum latency
#define AT91C_MCI_MAXLAT_64                   (0x1 << 12) // (MCI) 64 cycles maximum latency
#define AT91C_MCI_TRCMD       (0x3 << 16) // (MCI) Transfer CMD
#define AT91C_MCI_TRCMD_NO                   (0x0 << 16) // (MCI) No transfer
#define AT91C_MCI_TRCMD_START                (0x1 << 16) // (MCI) Start transfer
#define AT91C_MCI_TRCMD_STOP                 (0x2 << 16) // (MCI) Stop transfer
#define AT91C_MCI_TRDIR       (0x1 << 18) // (MCI) Transfer Direction
#define AT91C_MCI_TRDIR_WRITE                (0x0 << 18) // (MCI) Write
#define AT91C_MCI_TRDIR_READ                 (0x1 << 18) // (MCI) Read
#define AT91C_MCI_TRTYP       (0x7 << 19) // (MCI) Transfer Type
#define AT91C_MCI_TRTYP_BLOCK                (0x0 << 19) // (MCI) MMC/SDCard Single Block Transfer type
#define AT91C_MCI_TRTYP_MULTIPLE             (0x1 << 19) // (MCI) MMC/SDCard Multiple Block transfer type
#define AT91C_MCI_TRTYP_STREAM               (0x2 << 19) // (MCI) MMC Stream transfer type
#define AT91C_MCI_TRTYP_SDIO_BYTE            (0x4 << 19) // (MCI) SDIO Byte transfer type
#define AT91C_MCI_TRTYP_SDIO_BLOCK           (0x5 << 19) // (MCI) SDIO Block transfer type
#define AT91C_MCI_IOSPCMD     (0x3 << 24) // (MCI) SDIO Special Command
#define AT91C_MCI_IOSPCMD_NONE                 (0x0 << 24) // (MCI) NOT a special command
#define AT91C_MCI_IOSPCMD_SUSPEND              (0x1 << 24) // (MCI) SDIO Suspend Command
#define AT91C_MCI_IOSPCMD_RESUME               (0x2 << 24) // (MCI) SDIO Resume Command
#define AT91C_MCI_ATACS       (0x1 << 26) // (MCI) ATA with command completion signal
#define AT91C_MCI_ATACS_NORMAL               (0x0 << 26) // (MCI) normal operation mode
#define AT91C_MCI_ATACS_COMPLETION           (0x1 << 26) // (MCI) completion signal is expected within MCI_CSTOR
// -------- MCI_BLKR : (MCI Offset: 0x18) MCI Block Register -------- 
#define AT91C_MCI_BCNT        (0xFFFF <<  0) // (MCI) MMC/SDIO Block Count / SDIO Byte Count
// -------- MCI_CSTOR : (MCI Offset: 0x1c) MCI Completion Signal Timeout Register -------- 
#define AT91C_MCI_CSTOCYC     (0xF <<  0) // (MCI) Completion Signal Timeout Cycle Number
#define AT91C_MCI_CSTOMUL     (0x7 <<  4) // (MCI) Completion Signal Timeout Multiplier
#define AT91C_MCI_CSTOMUL_1                    (0x0 <<  4) // (MCI) CSTOCYC x 1
#define AT91C_MCI_CSTOMUL_16                   (0x1 <<  4) // (MCI) CSTOCYC x  16
#define AT91C_MCI_CSTOMUL_128                  (0x2 <<  4) // (MCI) CSTOCYC x  128
#define AT91C_MCI_CSTOMUL_256                  (0x3 <<  4) // (MCI) CSTOCYC x  256
#define AT91C_MCI_CSTOMUL_1024                 (0x4 <<  4) // (MCI) CSTOCYC x  1024
#define AT91C_MCI_CSTOMUL_4096                 (0x5 <<  4) // (MCI) CSTOCYC x  4096
#define AT91C_MCI_CSTOMUL_65536                (0x6 <<  4) // (MCI) CSTOCYC x  65536
#define AT91C_MCI_CSTOMUL_1048576              (0x7 <<  4) // (MCI) CSTOCYC x  1048576
// -------- MCI_SR : (MCI Offset: 0x40) MCI Status Register -------- 
#define AT91C_MCI_CMDRDY      (0x1 <<  0) // (MCI) Command Ready flag
#define AT91C_MCI_RXRDY       (0x1 <<  1) // (MCI) RX Ready flag
#define AT91C_MCI_TXRDY       (0x1 <<  2) // (MCI) TX Ready flag
#define AT91C_MCI_BLKE        (0x1 <<  3) // (MCI) Data Block Transfer Ended flag
#define AT91C_MCI_DTIP        (0x1 <<  4) // (MCI) Data Transfer in Progress flag
#define AT91C_MCI_NOTBUSY     (0x1 <<  5) // (MCI) Data Line Not Busy flag
#define AT91C_MCI_ENDRX       (0x1 <<  6) // (MCI) End of RX Buffer flag
#define AT91C_MCI_ENDTX       (0x1 <<  7) // (MCI) End of TX Buffer flag
#define AT91C_MCI_SDIOIRQA    (0x1 <<  8) // (MCI) SDIO Interrupt for Slot A
#define AT91C_MCI_SDIOIRQB    (0x1 <<  9) // (MCI) SDIO Interrupt for Slot B
#define AT91C_MCI_SDIOIRQC    (0x1 << 10) // (MCI) SDIO Interrupt for Slot C
#define AT91C_MCI_SDIOIRQD    (0x1 << 11) // (MCI) SDIO Interrupt for Slot D
#define AT91C_MCI_SDIOWAIT    (0x1 << 12) // (MCI) SDIO Read Wait operation flag
#define AT91C_MCI_CSRCV       (0x1 << 13) // (MCI) CE-ATA Completion Signal flag
#define AT91C_MCI_RXBUFF      (0x1 << 14) // (MCI) RX Buffer Full flag
#define AT91C_MCI_TXBUFE      (0x1 << 15) // (MCI) TX Buffer Empty flag
#define AT91C_MCI_RINDE       (0x1 << 16) // (MCI) Response Index Error flag
#define AT91C_MCI_RDIRE       (0x1 << 17) // (MCI) Response Direction Error flag
#define AT91C_MCI_RCRCE       (0x1 << 18) // (MCI) Response CRC Error flag
#define AT91C_MCI_RENDE       (0x1 << 19) // (MCI) Response End Bit Error flag
#define AT91C_MCI_RTOE        (0x1 << 20) // (MCI) Response Time-out Error flag
#define AT91C_MCI_DCRCE       (0x1 << 21) // (MCI) data CRC Error flag
#define AT91C_MCI_DTOE        (0x1 << 22) // (MCI) Data timeout Error flag
#define AT91C_MCI_CSTOE       (0x1 << 23) // (MCI) Completion Signal timeout Error flag
#define AT91C_MCI_BLKOVRE     (0x1 << 24) // (MCI) DMA Block Overrun Error flag
#define AT91C_MCI_DMADONE     (0x1 << 25) // (MCI) DMA Transfer Done flag
#define AT91C_MCI_FIFOEMPTY   (0x1 << 26) // (MCI) FIFO Empty flag
#define AT91C_MCI_XFRDONE     (0x1 << 27) // (MCI) Transfer Done flag
#define AT91C_MCI_OVRE        (0x1 << 30) // (MCI) Overrun flag
#define AT91C_MCI_UNRE        (0x80000000) // (0x1 << 31) // (MCI) Underrun flag
// -------- MCI_IER : (MCI Offset: 0x44) MCI Interrupt Enable Register -------- 
// -------- MCI_IDR : (MCI Offset: 0x48) MCI Interrupt Disable Register -------- 
// -------- MCI_IMR : (MCI Offset: 0x4c) MCI Interrupt Mask Register -------- 
// -------- MCI_DMA : (MCI Offset: 0x50) MCI DMA Configuration Register -------- 
#define AT91C_MCI_OFFSET      (0x3 <<  0) // (MCI) DMA Write Buffer Offset
#define AT91C_MCI_CHKSIZE     (0x7 <<  4) // (MCI) DMA Channel Read/Write Chunk Size
#define AT91C_MCI_CHKSIZE_1                    (0x0 <<  4) // (MCI) Number of data transferred is 1
#define AT91C_MCI_CHKSIZE_4                    (0x1 <<  4) // (MCI) Number of data transferred is 4
#define AT91C_MCI_CHKSIZE_8                    (0x2 <<  4) // (MCI) Number of data transferred is 8
#define AT91C_MCI_CHKSIZE_16                   (0x3 <<  4) // (MCI) Number of data transferred is 16
#define AT91C_MCI_CHKSIZE_32                   (0x4 <<  4) // (MCI) Number of data transferred is 32
#define AT91C_MCI_DMAEN       (0x1 <<  8) // (MCI) DMA Hardware Handshaking Enable
#define AT91C_MCI_DMAEN_DISABLE              (0x0 <<  8) // (MCI) DMA interface is disabled
#define AT91C_MCI_DMAEN_ENABLE               (0x1 <<  8) // (MCI) DMA interface is enabled
// -------- MCI_CFG : (MCI Offset: 0x54) MCI Configuration Register -------- 
#define AT91C_MCI_FIFOMODE    (0x1 <<  0) // (MCI) MCI Internal FIFO Control Mode
#define AT91C_MCI_FIFOMODE_AMOUNTDATA           (0x0) // (MCI) A write transfer starts when a sufficient amount of datas is written into the FIFO
#define AT91C_MCI_FIFOMODE_ONEDATA              (0x1) // (MCI) A write transfer starts as soon as one data is written into the FIFO
#define AT91C_MCI_FERRCTRL    (0x1 <<  4) // (MCI) Flow Error Flag Reset Control Mode
#define AT91C_MCI_FERRCTRL_RWCMD                (0x0 <<  4) // (MCI) When an underflow/overflow condition flag is set, a new Write/Read command is needed to reset the flag
#define AT91C_MCI_FERRCTRL_READSR               (0x1 <<  4) // (MCI) When an underflow/overflow condition flag is set, a read status resets the flag
#define AT91C_MCI_HSMODE      (0x1 <<  8) // (MCI) High Speed Mode
#define AT91C_MCI_HSMODE_DISABLE              (0x0 <<  8) // (MCI) Default Bus Timing Mode
#define AT91C_MCI_HSMODE_ENABLE               (0x1 <<  8) // (MCI) High Speed Mode
#define AT91C_MCI_LSYNC       (0x1 << 12) // (MCI) Synchronize on last block
#define AT91C_MCI_LSYNC_CURRENT              (0x0 << 12) // (MCI) Pending command sent at end of current data block
#define AT91C_MCI_LSYNC_INFINITE             (0x1 << 12) // (MCI) Pending command sent at end of block transfer when transfer length is not infinite
// -------- MCI_WPCR : (MCI Offset: 0xe4) Write Protection Control Register -------- 
#define AT91C_MCI_WP_EN       (0x1 <<  0) // (MCI) Write Protection Enable
#define AT91C_MCI_WP_EN_DISABLE              (0x0) // (MCI) Write Operation is disabled (if WP_KEY corresponds)
#define AT91C_MCI_WP_EN_ENABLE               (0x1) // (MCI) Write Operation is enabled (if WP_KEY corresponds)
#define AT91C_MCI_WP_KEY      (0xFFFFFF <<  8) // (MCI) Write Protection Key
// -------- MCI_WPSR : (MCI Offset: 0xe8) Write Protection Status Register -------- 
#define AT91C_MCI_WP_VS       (0xF <<  0) // (MCI) Write Protection Violation Status
#define AT91C_MCI_WP_VS_NO_VIOLATION         (0x0) // (MCI) No Write Protection Violation detected since last read
#define AT91C_MCI_WP_VS_ON_WRITE             (0x1) // (MCI) Write Protection Violation detected since last read
#define AT91C_MCI_WP_VS_ON_RESET             (0x2) // (MCI) Software Reset Violation detected since last read
#define AT91C_MCI_WP_VS_ON_BOTH              (0x3) // (MCI) Write Protection and Software Reset Violation detected since last read
#define AT91C_MCI_WP_VSRC     (0xF <<  8) // (MCI) Write Protection Violation Source
#define AT91C_MCI_WP_VSRC_NO_VIOLATION         (0x0 <<  8) // (MCI) No Write Protection Violation detected since last read
#define AT91C_MCI_WP_VSRC_MCI_MR               (0x1 <<  8) // (MCI) Write Protection Violation detected on MCI_MR since last read
#define AT91C_MCI_WP_VSRC_MCI_DTOR             (0x2 <<  8) // (MCI) Write Protection Violation detected on MCI_DTOR since last read
#define AT91C_MCI_WP_VSRC_MCI_SDCR             (0x3 <<  8) // (MCI) Write Protection Violation detected on MCI_SDCR since last read
#define AT91C_MCI_WP_VSRC_MCI_CSTOR            (0x4 <<  8) // (MCI) Write Protection Violation detected on MCI_CSTOR since last read
#define AT91C_MCI_WP_VSRC_MCI_DMA              (0x5 <<  8) // (MCI) Write Protection Violation detected on MCI_DMA since last read
#define AT91C_MCI_WP_VSRC_MCI_CFG              (0x6 <<  8) // (MCI) Write Protection Violation detected on MCI_CFG since last read
#define AT91C_MCI_WP_VSRC_MCI_DEL              (0x7 <<  8) // (MCI) Write Protection Violation detected on MCI_DEL since last read
// -------- MCI_VER : (MCI Offset: 0xfc)  VERSION  Register -------- 
#define AT91C_MCI_VER         (0xF <<  0) // (MCI)  VERSION  Register

#endif //#ifndef __MCI_HS_H__
