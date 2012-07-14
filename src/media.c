/* ----------------------------------------------------------------------------
 *		 
 *        mboot for Lophilo embedded system
 *		
 * ----------------------------------------------------------------------------
 */

#include "types.h"
#include "trace.h"
#include "option.h"

//#include "sfr.h"
#include "system.h"
#include "mci_hs.h"
#include "sdmmc.h"
#include "media.h"

static U8 SdMed_Read(U32 address, void *data, U32 length, MediaCallback callback, void *argument);
static U8 SdMed_Write(U32 address, void *data, U32 length, MediaCallback callback, void *argument);

extern SdCard sdDrv; /// SDCard driver instance

Media sdMed;

void Init_Media(void)
{
	MCI_Init();
	DMAD_Initialize(BOARD_MCI_DMA_CHANNEL, DMAD_NO_DEFAULT_IT);
	
	if (SD_Init()) // Initialize the SD card driver
	{
		TRACE_ERR("SD Card (OS Boot) initialize failed.");
		while(1);
	}
	else
	{
		TRACE_MSG("OS Boot SD Card (%d MB,%5.1f MByte/sec) initialized.", (S32)(MMC_GetTotalSizeKB()/1024), (double)((sdDrv.transSpeed)/2000000));
	}
	MCI_SetSpeed(sdDrv.transSpeed, sdDrv.transSpeed, BOARD_MCK);
	
	// Initialize media fields
	//--------------------------------------------------------------------------
	sdMed.write = SdMed_Write;
	sdMed.read = SdMed_Read;
	sdMed.lock = 0;
	sdMed.unlock = 0;
	sdMed.handler = 0;
	sdMed.flush = 0;

	sdMed.blockSize = SD_BLOCK_SIZE;
	sdMed.baseAddress = 0;
	if (sdDrv.totalSize == 0xFFFFFFFF) sdMed.size = sdDrv.blockNr;
	else sdMed.size = sdDrv.totalSize/SD_BLOCK_SIZE;

	sdMed.mappedRD  = 0;
	sdMed.mappedWR  = 0;
	sdMed.protected = 0;
	sdMed.removable = 1;

	sdMed.state = MED_STATE_READY;

	sdMed.transfer.data = 0;
	sdMed.transfer.address = 0;
	sdMed.transfer.length = 0;
	sdMed.transfer.callback = 0;
	sdMed.transfer.argument = 0;
}

static U8 SdMed_Read(U32 address, void *data, U32 length, MediaCallback callback, void *argument)
{
	U8 error;

	if(sdMed.state != MED_STATE_READY) // Check that the media is ready
	{
		TRACE_WRN("Media busy.");
		return MED_STATUS_BUSY;
	}
	if((length + address) > sdMed.size) // Check that the data to read is not too big
	{
		TRACE_WRN("MEDSdcard_Read: Data too big: %d, %d\n\r", (int)length, (int)address);
		return MED_STATUS_ERROR;
	}
	sdMed.state = MED_STATE_BUSY; // Enter Busy state
	error = SD_ReadBlock(address, length, data);
	sdMed.state = MED_STATE_READY; // Leave the Busy state
	if (callback != 0) // Invoke callback
	{
		callback(argument, error ? MED_STATUS_ERROR : MED_STATUS_SUCCESS, 0, 0);
	}
	return MED_STATUS_SUCCESS;
}

static U8 SdMed_Write(U32 address, void *data, U32 length, MediaCallback callback, void *argument)
{
	U8 error;

	if(sdMed.state != MED_STATE_READY) // Check that the media if ready
	{
		TRACE_WRN("MEDSdcard_Write: Media is busy\n\r");
		return MED_STATUS_BUSY;
	}
	if ((length + address) > sdMed.size) // Check that the data to write is not too big
	{
		TRACE_WRN("MEDSdcard_Write: Data too big\n\r");
		return MED_STATUS_ERROR;
	}
	sdMed.state = MED_STATE_BUSY; // Put the media in Busy state
	error = SD_WriteBlock(address, length, data);
	sdMed.state = MED_STATE_READY; // Leave the Busy state
	if (callback != 0) // Invoke the callback if it exists
	{
		callback(argument, error ? MED_STATUS_ERROR : MED_STATUS_SUCCESS, 0, 0);
	}
	return MED_STATUS_SUCCESS;
}
