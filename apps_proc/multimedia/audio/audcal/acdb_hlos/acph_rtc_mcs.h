#ifndef _RTC_MCS_INTF_H_
#define _RTC_MCS_INTF_H_
/*! \file acph_rtc_mcs.h
\brief MCS RTC Client Header of ACPH

Copyright (c) 2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Proprietary and Confidential.
*/

/*===========================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/audcal/acdb_hlos/acph_rtc_mcs.h#1 $
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
#include "acph.h"

/*
   ----------------------------------
   | Externalized Function Definitions    |
   ----------------------------------
   */
/**
 * FUNCTION : acph_rtc_mcs_ioctl
 *
 * DESCRIPTION : acph MediacontrolService rtc function call
 *
 * DEPENDENCIES : NONE
 *
 * PARAMS:
 *   nCommandId - command Id;
 *   req_buf_ptr - pointer to request buffer
 *   req_buf_len - Length of the response buffer
 *   resp_buf_ptr - pointer to response buffer
 *   resp_buf_length - length of the response buffer
 *   resp_buf_bytes_filled - Length of the response buffer filled.
 * RETURN VALUE : ACPH_SUCCESS/ACPH_FAILURE/ACPH_ERR_INVALID_COMMAND
 *
 * SIDE EFFECTS : None
 */
int32_t acph_rtc_mcs_ioctl( uint16_t nCommandId,
                                 uint8_t *req_buf_ptr,
                                 uint32_t req_buf_len,
                                 uint8_t *resp_buf_ptr,
                                 uint32_t resp_buf_len,
                                 uint32_t *resp_buf_bytes_filled);

/**
 * FUNCTION : acph_rtc_mcs_init
 *
 * DESCRIPTION : Initialize MediaControlService RTC
 *
 * DEPENDENCIES : NONE
 *
 * PARAMS:
 *   req_buf_ptr - pointer to request buffer
 *   resp_buf_ptr - pointer to response buffer
 *   resp_buf_length - length of the response buffer
 *
 * RETURN VALUE : ACPH_SUCCESS or ACPH_FAILURE
 *
 * SIDE EFFECTS : None
 */
int32_t acph_rtc_mcs_init(void);

/**
 * FUNCTION : acph_rtc_mcs_deinit
 *
 * DESCRIPTION : De-initialize MediaControlService RTC
 *
 * DEPENDENCIES : NONE
 *
 * PARAMS:
 *   req_buf_ptr - pointer to request buffer
 *   resp_buf_ptr - pointer to response buffer
 *   resp_buf_length - length of the response buffer
 *
 * RETURN VALUE : ACPH_SUCCESS or ACPH_FAILURE
 *
 * SIDE EFFECTS : None
 */
int32_t acph_rtc_mcs_deinit(void);

#endif //_RTC_MCS_INTF_H_

