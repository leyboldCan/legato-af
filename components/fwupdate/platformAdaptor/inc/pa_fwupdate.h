/**
 * @page c_pa_fwupdate Firmware Update Platform Adapter API
 *
 * @ref pa_fwupdate.h "API Reference"
 *
 * <HR>
 *
 * @section pa_fwupdate_toc Table of Contents
 *
 *  - @ref pa_fwupdate_intro
 *  - @ref pa_fwupdate_rational
 *
 *
 * @section pa_fwupdate_intro Introduction
 *
 * These APIs are on the top of the platform-dependent adapter layer. They are independent of the
 * implementation. They guarantee the portability on different kind of platform without any changes
 * for the components developped upon these APIs.
 *
 *
 * @section pa_fwupdate_rational Rational
 * These functions are all blocking functions, so that they return when the modem has answered or
 * when a timeout has occured due to an interrupted communication with the modem.
 *
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless Inc.
 */

//--------------------------------------------------------------------------------------------------
/** @file pa_fwupdate.h
 *
 * Legato @ref c_pa_fwupdate include file.
 *
 * Copyright (C) Sierra Wireless Inc.
 */

#ifndef LEGATO_PA_FWUPDATE_INCLUDE_GUARD
#define LEGATO_PA_FWUPDATE_INCLUDE_GUARD

#include "legato.h"

//--------------------------------------------------------------------------------------------------
/**
 * Enumerate all SW update states (used by pa_fwupdate_SetState)
 */
//--------------------------------------------------------------------------------------------------
typedef enum pa_fwupdate_state
{
    PA_FWUPDATE_STATE_NORMAL = 1, ///< Normal state
    PA_FWUPDATE_STATE_SYNC,       ///< Synchronization state
    PA_FWUPDATE_STATE_INVALID     ///< Invalid entry;
}
pa_fwupdate_state_t;

//--------------------------------------------------------------------------------------------------
/**
 * Update status
 *
 */
//--------------------------------------------------------------------------------------------------
typedef enum pa_fwupdate_UpdateStatus
{
    PA_FWUPDATE_UPDATE_STATUS_OK,              ///< Last update succeeded
    PA_FWUPDATE_UPDATE_STATUS_PARTITION_ERROR, ///< At least, one partition is corrupted
    PA_FWUPDATE_UPDATE_STATUS_DWL_ONGOING,     ///< Downloading in progress
    PA_FWUPDATE_UPDATE_STATUS_DWL_FAILED,      ///< Last downloading failed
    PA_FWUPDATE_UPDATE_STATUS_DWL_TIMEOUT,     ///< Last downloading stopped due to timeout
    PA_FWUPDATE_UPDATE_STATUS_UNKNOWN          ///< Unknown status. It has to be the last one.
} pa_fwupdate_UpdateStatus_t;


//--------------------------------------------------------------------------------------------------
/**
 * This function starts a package download to the device.
 *
 * @warning This API is a blocking API. It needs to be called in a dedicated thread.
 *
 * @return
 *      - LE_OK              On success
 *      - LE_BAD_PARAMETER   If an input parameter is not valid
 *      - LE_TIMEOUT         After 900 seconds without data received
 *      - LE_NOT_POSSIBLE    The systems are not synced
 *      - LE_CLOSED          File descriptor has been closed before all data have been received
 *      - LE_FAULT           On failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t pa_fwupdate_Download
(
    int fd   ///< [IN] File descriptor of the image, opened to the start of the image.
);

//--------------------------------------------------------------------------------------------------
/**
 * Return the update package write position.
 *
 * @note This is actually the position within the update package, not the one once the update
 * package is processed (unzipping, extracting, ... ).
 *
 * @return
 *      - LE_OK            on success
 *      - LE_BAD_PARAMETER bad parameter
 *      - LE_FAULT         on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t pa_fwupdate_GetResumePosition
(
    size_t *positionPtr     ///< [OUT] Update package read position
);


//--------------------------------------------------------------------------------------------------
/**
 * Return the last update status.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER Invalid parameter
 *      - LE_FAULT on failure
 *      - LE_UNSUPPORTED not supported
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t pa_fwupdate_GetUpdateStatus
(
    pa_fwupdate_UpdateStatus_t *statusPtr, ///< [OUT] Returned update status
    char *statusLabelPtr,                  ///< [OUT] String matching the status
    size_t statusLabelLength               ///< [IN] Maximum length of the status description
);


//--------------------------------------------------------------------------------------------------
/**
 * Read the image file from the modem.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t pa_fwupdate_Read
(
    int* fdPtr         ///< [OUT] File descriptor for the image, ready for reading.
);


//--------------------------------------------------------------------------------------------------
/**
 * Get the firmware version string
 *
 * @return
 *      - LE_OK            on success
 *      - LE_NOT_FOUND     if the version string is not available
 *      - LE_OVERFLOW      if version string to big to fit in provided buffer
 *      - LE_BAD_PARAMETER bad parameter
 *      - LE_FAULT for any other errors
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t pa_fwupdate_GetFirmwareVersion
(
    char* versionPtr,        ///< [OUT] Firmware version string
    size_t versionSize       ///< [IN] Size of version buffer
);


//--------------------------------------------------------------------------------------------------
/**
 * Get the bootloader version string
 *
 * @return
 *      - LE_OK            on success
 *      - LE_NOT_FOUND     if the version string is not available
 *      - LE_OVERFLOW      if version string to big to fit in provided buffer
 *      - LE_BAD_PARAMETER bad parameter
 *      - LE_FAULT for any other errors
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t pa_fwupdate_GetBootloaderVersion
(
    char* versionPtr,        ///< [OUT] Firmware version string
    size_t versionSize       ///< [IN] Size of version buffer
);

//--------------------------------------------------------------------------------------------------
/**
 * Program a synchronization between active and update systems
 *
 * @return
 *      - LE_OK             on success
 *      - LE_UNSUPPORTED    the feature is not supported
 *      - LE_FAULT          on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t pa_fwupdate_DualSysSync
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Request a full system reset with a systems swap and optionally a sync
 *
 * @note On success, a device reboot is initiated without returning any value.
 *
 * @return
 *      - LE_BUSY           download is ongoing, swap is not allowed
 *      - LE_UNSUPPORTED    the feature is not supported
 *      - LE_FAULT          on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t pa_fwupdate_DualSysSwap
(
    bool isSyncReq      ///< [IN] Indicate if a synchronization is requested after the swap
);

//--------------------------------------------------------------------------------------------------
/**
 * Function which indicates if Active and Update systems are synchronized
 *
 * @return
 *      - LE_OK            on success
 *      - LE_UNSUPPORTED   the feature is not supported
 *      - LE_BAD_PARAMETER bad parameter
 *      - LE_FAULT         on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t pa_fwupdate_DualSysGetSyncState
(
    bool *isSyncPtr ///< [OUT] Indicates if both systems are synchronized
);

//--------------------------------------------------------------------------------------------------
/**
 * Function to reset the device. This function does not return any error code.
 *
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void pa_fwupdate_Reset
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Function which indicates if a Sync operation is needed (swap & sync operation)
 *
 * @return
 *      - LE_OK            on success
 *      - LE_UNSUPPORTED   the feature is not supported
 *      - LE_BAD_PARAMETER bad parameter
 *      - LE_FAULT         else
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t pa_fwupdate_DualSysCheckSync
(
    bool *isSyncReqPtr ///< [OUT] Indicates if synchronization is requested
);

//--------------------------------------------------------------------------------------------------
/**
 * This API is to be called at the beginning of a SYNC operation.
 * It updates the SW update state field in SSDATA
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t pa_fwupdate_SetState
(
    pa_fwupdate_state_t state   ///< [IN] state to set
);

//--------------------------------------------------------------------------------------------------
/**
 * request to the modem to delete NVUP files
 *
 * @return
 *      - LE_OK             on success
 *      - LE_UNSUPPORTED    the feature is not supported
 *      - LE_FAULT          on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t pa_fwupdate_NvupDelete
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Write a NVUP file in UD system
 *
 * @return
 *      - LE_OK             on success
 *      - LE_UNSUPPORTED    the feature is not supported
 *      - LE_FAULT          on failure
 *      - others            Depending of the underlying operations
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t pa_fwupdate_NvupWrite
(
    size_t length,                      ///< [IN] data length
    uint8_t* dataPtr,                   ///< [IN] input data
    bool isEnd                          ///< [IN] flag to indicate the end of the file
);

//--------------------------------------------------------------------------------------------------
/**
 * request the modem to apply the NVUP files in UD system
 *
 * @return
 *      - LE_OK             on success
 *      - LE_UNSUPPORTED    the feature is not supported
 *      - LE_FAULT          on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t pa_fwupdate_NvupApply
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the resume context
 *
 * @return
 *      - LE_OK             on success
 *      - LE_UNSUPPORTED    the feature is not supported
 *      - LE_FAULT          on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t pa_fwupdate_InitDownload
(
    void
);

#endif // LEGATO_PA_FWUPDATE_INCLUDE_GUARD
