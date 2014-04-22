#ifndef __CSD_DEV_MAIN_H__
#define __CSD_DEV_MAIN_H__
/*==============================================================================
  @file csd_dev_common.h

  This header contains structure and function skeletons which are commonly used
  for managing a device, especially the setup and tear down operation.

  Copyright (c) 2008-13, 2016 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/common/inc/csd_dev_main.h#1 $
  $DateTime: 2018/12/25 22:39:07 $
  $Author: pwbldsvc $
  $Change: 17929171 $
  $Revision: #1 $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------
  11/15/16   rk      Abstracting OSAL layer
  02/15/13   suns    Device module unitfication.
  09/21/12   up      B Family base
                     -Reformatting to a common format
  05/05/10   suns    Initial Creation
==============================================================================*/

#include "DALSys.h"
#include "csd_dev_common.h"
#include "csd_dev_debug.h"
#include "csd_os_dependencies.h"

/* Structure for associating mutex with csd_dev_record */
struct csd_dev_data
{
  csd_lock_t mutex;
  csd_dev_record_t* active_devs_head;
};

/* dev_data is defined in @see csd_dev_main.c */
extern struct csd_dev_data dev_data;

/* tear down all active devices when SSR event happens */
int32_t csd_dev_ssr_teardown_devices(void);

/* tear down all active devices forcefully */
int32_t csd_dev_force_teardown_all_devices(void);

/* Initialize all needed dependent components for CSD device module */
int32_t csd_dev_tgt_init(void);

/* De-initialize all needed dependent components for CSD device module */
int32_t csd_dev_tgt_deinit(void);

/* Route CSD DEV IOCTLs to corresponding functional sub-components */
uint32_t csd_dev_tgt_ioctl( const csd_dev_record_t* rx_dev,
                            const csd_dev_record_t* tx_dev,
                            const csd_dev_record_t* ref_dev,
                            uint32_t cmd_code, void *payload,
                            uint32_t len );

/* Fill in the device info structure for CSD DEV debug module */
int32_t csd_dev_tgt_debug_fill_dev_info(const csd_dev_record_t* a_dev,
                               csd_debug_active_dev_info_query_t* dev_info_rsp);


/* Fill in the device info structure for CSD DEV debug module */
int32_t csd_dev_tgt_debug_fill_dev_info_v2(const csd_dev_record_t* a_dev,
                               csd_debug_active_dev_info_query_v2_t* dev_info_rsp);

/* Tear down the mbhc when SSR event happens */
int32_t csd_dev_ssr_shutdown_mbhc(void);

#endif /* __CSD_DEV_MAIN_H__ */
