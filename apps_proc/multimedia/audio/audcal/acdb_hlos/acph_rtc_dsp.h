#ifndef _RTC_AUDIO_VOICE_INTF_H_
#define _RTC_AUDIO_VOICE_INTF_H_
/** 
  \file **************************************************************************
 *
 *                   RTC AUDIO & VOICE  H E A D E R    F I L E
 *
 *DESCRIPTION
 * This header file contains all the definitions necessary for Audio Calibration
 * Packet Handler to handle request buffer and operate ACDB 
 * This acph works only in ARM9
 *  
 *Copyright (c) 2011, 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
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
/* $Header: //components/rel/audio.tx/2.0/audio/audcal/acdb_hlos/acph_rtc_dsp.h#1 $ */
/*
   --------------------
   |include files                |
   --------------------
   */
#include <stddef.h>
#include <string.h>
#include "acph.h"
#include "DALSys.h"
#include "acdb_os_includes.h"

//CSD
#include "csd.h"
#include "csd_dev_debug.h"

#include "csd_vs_ioctl.h"
#include "csd_vc_ioctl.h"
#include "csd_voice_debug.h"
#include "csd_voice_debug_ioctl.h"
#include "csd_os_dependencies.h"
#include "customer.h"

#include "vss_common_public_if.h"
#include "vss_imemory_public_if.h"

//APR header, to be removed
#include "apr_comdef.h"
#include "apr_errcodes.h"
#include "apr_api.h"
#include "aprv2_api.h"
#include "aprv2_packet.h"
#include "aprv2_msg_if.h"
#include "aprv2_api_inline.h"
#include "aprv2_ids_services.h"
#include "aprv2_ids_domains.h"

//Error code
#define RTC_INTF_SUCCESS                           0
#define RTC_INTF_EFAILURE                          0x21
#define RTC_INTF_EBADPARAM                         0x22
#define RTC_INTF_ETIMEOUT                          0x23
#define RTC_INTF_ENORESOURCE                       0x24

typedef struct 
{
  csd_event_t sync_event_handle;
} rtcq6intf_transaction;

typedef struct 
{
  csd_lock_t lock;              /* global mutex */  
  uint32_t sync_send_timeout;   
  rtcq6intf_transaction trans_obj;
} rtcq6intf;

typedef struct {
  uint32_t                  descriptor_mapped_handle; /* CVD handle for descriptor table */
  void*                     descriptor_memory_handle; /* Internal DAL memory handle for descriptor table */
  void*                     data_memory_handle;       /* Internal DAL memory handle for RTC buffer */  
  uint8_t*                  pva_descriptor;
  uint8_t*                  pva_data;
  uint64_t                  phy_addr_data;
  uint64_t                  phy_addr_descriptor; 
} rtc_oob_mem_t;
/*
   --------------------
   | External functions |
   --------------------
   */
/**
 * FUNCTION : acph_rtc_dsp_ioctl
 *
 * DESCRIPTION : acph rtc dsp function call
 *
 * DEPENDENCIES : NONE
 *
 * PARAMS:
 *   nCommandId - command Id;
 *   req_buf_ptr - pointer to request buffer
 *   resp_buf_ptr - pointer to response buffer
 *   resp_buf_length - length of the response buffer
 *
 * RETURN VALUE : ACPH_SUCCESS/ACPH_FAILURE/ACPH_ERR_INVALID_COMMAND
 *
 * SIDE EFFECTS : None
 */
int32_t acph_rtc_dsp_ioctl(uint16_t nCommandId,
                           uint8_t *req_buf_ptr,
                           uint32_t req_buf_len,
                           uint8_t *resp_buf_ptr,
                           uint32_t resp_buf_total_len,
                           uint32_t *resp_buf_length
                          );

/**
 * FUNCTION : acph_rtc_dsp_init
 *
 * DESCRIPTION : Initialize audio and voice RTC
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
int32_t acph_rtc_dsp_init(void);

/**
 * FUNCTION : acph_rtc_dsp_deinit
 *
 * DESCRIPTION : De-initialize audio and voice RTC
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
int32_t acph_rtc_dsp_deinit(void);

#endif //_RTC_AUDIO_VOICE_INTF_H_
