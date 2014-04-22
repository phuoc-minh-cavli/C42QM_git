#ifndef _CSD_Q6ATL_MAIN_H_
#define _CSD_Q6ATL_MAIN_H_
/*============================================================================

   FILE:       csd_q6atl_main.h

   DESCRIPTION: 

   PUBLIC CLASSES:  Not Applicable

   INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2010-2012, 2016 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
 ===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ----------------------------------------------------------
04/27/10   mingchen initial draft 
============================================================================*/


#include "csd_q6atl.h"
#include "DALSys.h"
#include "csd_list.h"
#include "apr_comdef.h"
#include "apr_errcodes.h"
#include "apr_api.h"
#include "aprv2_api.h"
#include "aprv2_packet.h"

#define Q6ATL_Q6_SYNC_TRANS_MAX_NUM  64       /**< max num sync transactions */
#define Q6ATL_APR_SERVICES_MAX_NUM   3        /**< max num APR services. Now 3 - AS, AC, DEV */
#define CSD_Q6ATL_TIMEOUT_MICRO_SEC  2500000  /**< sync send timeout = changing from 10 seconds to 2.5 seconds*/

struct csd_q6atl_sync_transaction{
  struct csd_list_node node;
  uint32_t apr_token;   /**< apr token */
  uint16_t port;        /**< apr src_port - sender's port */
  uint16_t addr;        /**< apr src_addr - sender's addr */
  void* rsp_buf;        /**< used to contain rsp packet */
  uint32_t* rsp_buf_len;/**< rsp len */
  csd_event_t sync_event_handle;
};

struct csd_q6atl_session_obj {
  struct csd_list_node node;
  csd_q6atl_session_listener_type rx_cb;  /**< async session's call back func ptr */
  void* rx_cb_data;                       /**< async call back's client data */
  uint16_t port;                          /**< async session's session id */
};

struct csd_q6atl_apr_service_obj {
  bool_t used;
  struct csd_list used_session_list;
  uint16_t addr;                             /**< service's address - AS, AC or DEV */
  uint32_t apr_handle;                       /**< apr returned handle */
};

struct csd_q6atl{
  csd_lock_t lock;              /**< global mutex */
  uint32_t sync_send_timeout;
  struct csd_q6atl_apr_service_obj apr_service_obj[Q6ATL_APR_SERVICES_MAX_NUM];
  struct csd_list used_sync_trans_list;   /**< in use sync trans list */
  struct csd_list free_sync_trans_list;   /**< free sync trans list */
};


#endif /* _CSD_Q6ATL_MAIN_H_ */
