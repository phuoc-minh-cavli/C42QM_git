#ifndef _RTC_ADIE_INTF_H_
#define _RTC_ADIE_INTF_H_
/** 
  \file **************************************************************************
 *
 *                    Online_Intf   H E A D E R    F I L E
 *
 *DESCRIPTION
 * This header file contains all the definitions necessary for Audio Calibration
 * Packet Handler to handle request buffer and operate ACDB 
 * This acph works only in ARM9
 *  
 *Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 *******************************************************************************
 */
/**
  \file ***************************************************************************
 *
 *                      EDIT HISTORY FOR FILE
 *
 *  This section contains comments describing changes made to this file.
 *  Notice that changes are listed in reverse chronological order.
 *   
 *  $Header: 
 *
 *when       who     what, where, why
 *--------   ---     ----------------------------------------------------------
 *08/03/11   ernanl  initial draft
 ********************************************************************************
 */
/* $Header: //components/rel/audio.tx/2.0/audio/audcal/acdb_hlos/acph_rtc_adie.h#1 $ */
/*
   --------------------
   |include files                |
   --------------------
   */
#include "acph.h"

/*
   --------------------
   | External functions |
   --------------------
   */
/**
 * FUNCTION : acph_rtc_adie_ioctl
 *
 * DESCRIPTION : ACPH RTC ADIE function call
 *
 * DEPENDENCIES : NONE
 *
 * PARAMS:
 *   nCommandId - command Id;
 *   req_buf_ptr - pointer to request buffer
 *   req_buf_len - length of request buffer
 *   resp_buf_ptr - pointer to response buffer
 *   resp_buf_len - length of the response buffer
 *   resp_buf_bytes_filled - length of the output data in response buffer
 *
 * RETURN VALUE : ACPH_SUCCESS/ACPH_FAILURE/ACPH_ERR_INVALID_COMMAND
 *
 * SIDE EFFECTS : None
 */
int32_t acph_rtc_adie_ioctl(uint16_t commandId,
                            uint8_t *req_buf_ptr,
                            uint32_t req_buf_len,
                            uint8_t *resp_buf_ptr,
                            uint32_t resp_buf_len,
                            uint32_t *resp_buf_bytes_filled
                           );

/**
 * FUNCTION : acph_rtc_adie_init
 *
 * DESCRIPTION : Initialize RTC ADIE calibration
 *
 * DEPENDENCIES : NONE
 *
 * PARAMS: NONE
 *
 * RETURN VALUE : ACPH_SUCCESS or ACPH_FAILURE
 *
 * SIDE EFFECTS : None
 */
int32_t acph_rtc_adie_init(void);
/**
 * FUNCTION : acph_rtc_adie_deinit
 *
 * DESCRIPTION : De-initialize RTC ADIE calibration
 *
 * DEPENDENCIES : NONE
 *
 * PARAMS: NONE
 *
 * RETURN VALUE : ACPH_SUCCESS or ACPH_FAILURE
 *
 * SIDE EFFECTS : None
 */
int32_t acph_rtc_adie_deinit(void);

#endif //_RTC_ADIE_INTF_H_

