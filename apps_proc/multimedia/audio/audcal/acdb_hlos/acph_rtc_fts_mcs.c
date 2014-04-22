/*! \file acph_rtc_fts.c
\brief STUB ON MDM for File Transfer Service and 
         Media Contrl service RTC Clients of ACPH

Copyright (c) 2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Proprietary and Confidential.
*/

/*===========================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/audcal/acdb_hlos/acph_rtc_fts_mcs.c#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

============================================================================*/

/*===========================================================================
Include Files
===========================================================================*/
#include "acph_rtc_fts.h"
#include "acph_rtc_mcs.h"

/*===========================================================================
Macro
===========================================================================*/

/*============================================================================
Function Declarations
============================================================================*/

/******************************************************************************
                  QNX FILE SERVICE HELPER FUCNTIONS
******************************************************************************/

/*
------------------------------------
| Externalized Function Definitions |
------------------------------------
*/
/**
* FUNCTION : acph_rtc_fs_ioctl
*
* DESCRIPTION : acph FileService rtc function call
*
* DEPENDENCIES : NONE
*
* PARAMS:
*   nCommandId - command Id;
*   req_buf_ptr - pointer to request buffer
*   resp_buf_ptr - pointer to response buffer
*   resp_buf_length - length of the response buffer
*
* RETURN VALUE : ACPH_SUCCESS/ACPH_FAILURE/
                 ACPH_ERR_INVALID_COMMAND/ACPH_ERR_INVALID_FILE_PATH
*
* SIDE EFFECTS : None
*/
int32_t acph_rtc_fts_ioctl(uint16_t nCommandId,
                            uint8_t *req_buf_ptr,
                            uint32_t req_buf_len,
                            uint8_t *resp_buf_ptr,
                            uint32_t resp_buf_len,
                            uint32_t *resp_buf_bytes_filled)
{

   return ACPH_SUCCESS;
}

/**
* FUNCTION : acph_rtc_fs_init
*
* DESCRIPTION : Initialize FileService RTC
*
* DEPENDENCIES : ACPH_SUCCESS or ACPH_FAILURE
*
* PARAMS: None
*
* RETURN VALUE : None
*
* SIDE EFFECTS : None
*/
int32_t acph_rtc_fts_init(void)
{
   return ACPH_SUCCESS;
}

/**
* FUNCTION : acph_rtc_fs_init
*
* DESCRIPTION : De-initialize File Service RTC
*
* DEPENDENCIES : NONE
*
* PARAMS: None
*
* RETURN VALUE : ACPH_SUCCESS or ACPH_FAILURE
*
* SIDE EFFECTS : None
*/
int32_t acph_rtc_fts_deinit(void)
{
   return ACPH_SUCCESS;
}

/**
* FUNCTION : acph_rtc_mcs_ioctl
*
* DESCRIPTION : acph Media Control Service rtc function call
*
* DEPENDENCIES : NONE
*
* PARAMS:
*   nCommandId - command Id;
*   req_buf_ptr - pointer to request buffer
*   resp_buf_ptr - pointer to response buffer
*   resp_buf_length - length of the response buffer
*
* RETURN VALUE : ACPH_SUCCESS/ACPH_FAILURE/ACPH_ERR_INVALID_COMMAND/
                 ACPH_ERR_NO_ACTIVE_SESSION
*
* SIDE EFFECTS : None
*/
int32_t acph_rtc_mcs_ioctl(uint16_t nCommandId,
                            uint8_t *req_buf_ptr,
                            uint32_t req_buf_len,
                            uint8_t *resp_buf_ptr,
                            uint32_t resp_buf_len,
                            uint32_t *resp_buf_bytes_filled)
{
   return ACPH_SUCCESS;
}

/**
* FUNCTION : acph_rtc_mcs_init
*
* DESCRIPTION : Initialize Media Control Service RTC
*
* DEPENDENCIES : ACPH_SUCCESS or ACPH_FAILURE
*
* PARAMS: None
*
* RETURN VALUE : None
*
* SIDE EFFECTS : None
*/
int32_t acph_rtc_mcs_init(void)
{
   return ACPH_SUCCESS;
}

/**
* FUNCTION : acph_rtc_mcs_init
*
* DESCRIPTION : De-initialize Media Control Service RTC
*
* DEPENDENCIES : NONE
*
* PARAMS: None
*
* RETURN VALUE : ACPH_SUCCESS or ACPH_FAILURE
*
* SIDE EFFECTS : None
*/
int32_t acph_rtc_mcs_deinit(void)
{
   return ACPH_SUCCESS;
}
