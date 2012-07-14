/*
 *------------------------------------------------------------------------------
 * 
 *------------------------------------------------------------------------------
 */

#ifndef __MEDIA_H__
#define __MEDIA_H__

#include "types.h"

//------------------------------------------------------------------------------
//      Definitions
//------------------------------------------------------------------------------

// Operation result code returned by media methods
#define MED_STATUS_SUCCESS      0x0
#define MED_STATUS_ERROR        0x1
#define MED_STATUS_BUSY         0x2
#define MED_STATUS_PROTECTED    0x4

// Media statuses
#define MED_STATE_READY         0x0    /// Media is ready for access
#define MED_STATE_BUSY          0x1    /// Media is busy

//------------------------------------------------------------------------------
//      Types
//------------------------------------------------------------------------------
typedef struct _Media Media;
typedef void (*MediaCallback)(void *argument, U8 status, U32 transferred, U32 remaining);
typedef U8 (*Media_write)(U32 address, void *data, U32 length, MediaCallback callback, void *argument);
typedef U8 (*Media_read)(U32 address, void *data, U32 length, MediaCallback callback, void *argument);
typedef U8 (*Media_cancelIo)(void);
typedef U8 (*Media_lock)(U32 start, U32 end, U32 *pActualStart, U32 *pActualEnd);
typedef U8 (*Media_unlock)(U32 start, U32 end, U32 *pActualStart, U32 *pActualEnd);
typedef U8 (*Media_ioctl)(U8 ctrl, void *buff);
typedef U8 (*Media_flush)(void);
typedef void (*Media_handler)(void);

// Media transfer
// TransferCallback
typedef struct
{
	void						*data;				// Pointer to the data buffer
	U32							address;			// Address where to read/write the data
	U32							length;				// Size of the data to read/write
	MediaCallback		callback;			// Callback to invoke when the transfer done
	void						*argument;		// Callback argument
} MEDTransfer;

// Media object
// MEDTransfer
struct _Media {
	Media_write			write;				// Write method
	Media_read			read;					// Read method
	Media_cancelIo	cancelIo;			// Cancel pending IO method
	Media_lock			lock;					// lock method if possible
	Media_unlock		unlock;				// unlock method if possible
	Media_flush			flush;				// Flush method
	Media_handler		handler;			// Interrupt handler
	U32							blockSize;		// Block size in bytes (1, 512, 1K, 2K ...)
	U32							baseAddress;	// Base address of media in number of blocks
	U32							size;					// Size of media in number of blocks
	MEDTransfer			transfer;			// Current transfer operation
	U8							bReserved:4,
									mappedRD:1,		// Mapped to memory space to read
									mappedWR:1,		// Mapped to memory space to write
									protected:1,	// Protected media?
									removable:1;	// Removable/Fixed media?
	U8							state;				// Status of media
	U16							reserved;
};

extern Media sdMed;

//------------------------------------------------------------------------------
//      Inline Functions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//! \brief  Writes data on a media
//! \param  media    Pointer to a Media instance
//! \param  address  Address at which to write
//! \param  data     Pointer to the data to write
//! \param  length   Size of the data buffer
//! \param  callback Optional pointer to a callback function to invoke when
//!                   the write operation terminates
//! \param  argument Optional argument for the callback function
//! \return Operation result code
//! \see    TransferCallback
//------------------------------------------------------------------------------
static inline U8 MEDIA_Write(U32 address, void *data, U32 length, MediaCallback callback, void *argument)
{
	return sdMed.write(address, data, length, callback, argument);
}

//------------------------------------------------------------------------------
//! \brief  Reads a specified amount of data from a media
//! \param  media    Pointer to a Media instance
//! \param  address  Address of the data to read
//! \param  data     Pointer to the buffer in which to store the retrieved
//!                   data
//! \param  length   Length of the buffer
//! \param  callback Optional pointer to a callback function to invoke when
//!                   the operation is finished
//! \param  argument Optional pointer to an argument for the callback
//! \return Operation result code
//! \see    TransferCallback
//------------------------------------------------------------------------------
static inline U8 MEDIA_Read(U32 address, void *data, U32 length, MediaCallback callback, void *argument)
{
	return sdMed.read(address, data, length, callback, argument);
}

//------------------------------------------------------------------------------
//! \brief  Locks all the regions in the given address range.
//! \param  media    Pointer to a Media instance
/// \param  start  Start address of lock range.
/// \param  end  End address of lock range.
/// \param  pActualStart  Start address of the actual lock range (optional).
/// \param  pActualEnd  End address of the actual lock range (optional).
/// \return 0 if successful; otherwise returns an error code.
//------------------------------------------------------------------------------
static inline U8 MEDIA_Lock(U32 start, U32 end, U32 *pActualStart, U32 *pActualEnd)
{
	if(sdMed.lock)
	{
		return sdMed.lock(start, end, pActualStart, pActualEnd);
	}
	else
	{
		return MED_STATUS_SUCCESS;
	}
}

//------------------------------------------------------------------------------
//! \brief  Unlocks all the regions in the given address range
//! \param  media    Pointer to a Media instance
/// \param start  Start address of unlock range.
/// \param end  End address of unlock range.
/// \param pActualStart  Start address of the actual unlock range (optional).
/// \param pActualEnd  End address of the actual unlock range (optional).
/// \return 0 if successful; otherwise returns an error code.
//------------------------------------------------------------------------------
static inline U8 MEDIA_Unlock(U32 start, U32 end, U32 *pActualStart, U32 *pActualEnd)
{
	if(sdMed.unlock)
	{
		return sdMed.unlock(start, end, pActualStart, pActualEnd);
	}
	else
	{
		return MED_STATUS_SUCCESS;
	}
}

//------------------------------------------------------------------------------
//! \brief  
//! \param  media Pointer to the Media instance to use
//------------------------------------------------------------------------------
static inline U8 MEDIA_Flush(void)
{
	if(sdMed.flush)
	{
		return sdMed.flush();
	}
	else
	{
		return MED_STATUS_SUCCESS;
	}
}

//------------------------------------------------------------------------------
//! \brief  Invokes the interrupt handler of the specified media
//! \param  media Pointer to the Media instance to use
//------------------------------------------------------------------------------
static inline void MEDIA_Handler(void)
{
	if(sdMed.handler)
	{
		sdMed.handler();
	}
}

//------------------------------------------------------------------------------
//      Exported functions
//------------------------------------------------------------------------------

void Init_Media(void);

#endif /* __MEDIA_H__ */
