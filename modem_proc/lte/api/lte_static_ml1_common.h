/*!
  @file lte_static_ml1_common.h

  @brief
  External static APIs for ML1

  @detail
  External static APIs for ML1

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/lte.mpss/4.7.2/api/lte_static_ml1_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/04/16   sc      Initial Revision

===========================================================================*/

#ifndef LTE_STATIC_ML1_COMMON_H
#define LTE_STATIC_ML1_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <lte_static_ml1_ext_api.h>
#include "lte_static_as_msg.h"
#include "ULog.h"
#include "ULogFront.h"

typedef void (*lte_ds_jammer_detect_cback_type)(sys_radio_access_tech_e_type rat, uint32 jamming_status, boolean soft_jamming_status);
extern lte_ds_jammer_detect_cback_type lte_jammer_notify_cb;

typedef enum
{
  ML1_SYS_TYPE_LTE,
  ML1_SYS_TYPE_NB1,
} lte_static_ml1_sys_type_e;

typedef struct
{
  lte_ml1_gps_event_cb_t     gps_event_cb;
  ml1_init_req_t             ml1_mgr_init_req;
  ml1_deinit_req_t           ml1_mgr_deinit_req;
  ml1_init_req_t             gm_init_req;
  ml1_deinit_req_t           gm_deinit_req; 
  lte_ml1_common_rssi_cmapi_tx_power_atomic_s *cmapi_tx_power_atomic_ptr;
  ml1_init_req_t             offload_init_req;
  ml1_deinit_req_t           offload_deinit_req;
  lte_cphy_ce_info_rpt_req_s ce_info_report_req_cached;
}lte_static_ml1_pre_init_data_s;


typedef struct
{
  /*! the current index of the earfcn being searched, within the earfcn
      channels directly received from the RRC request.
   */
  uint16                              rrc_chan_idx;

  /*! the current index of the band being searched, within the band
      list directly from the RRC request
   */
  uint16                              rrc_band_idx;
 
  uint64                              start_earfcn;
 
  uint64                              end_earfcn;

  /*! number of frequency included in this request */
  uint8                           num_chans;

  /*! the list of channels to be searched for PLMN */
  nb1_cphy_bplmn_chan_s          chans[NB1_CPHY_BPLMN_MAX_FREQS];

  /*! number of bands included in this request */
  uint8                           num_bands;

  /*! the bands to be searched for PLMN */
  sys_sband_lte_e_type            bands[NB1_CPHY_BPLMN_MAX_BANDS];
}lte_static_ml1_bplmn_data_s;


#if defined (FEATURE_DYNAMIC_LOADING_LTE) || defined (FEATURE_EXTERNAL_PAGING)
/*===========================================================================

  FUNCTION:  lte_static_ml1_get_active_intf_id

===========================================================================*/
/*!
  @brief
  Returns active interface id for corresponding stack.

  @return
  void 
*/
/*=========================================================================*/
uint32 lte_static_ml1_get_active_intf_id ( void );
#endif

/*===========================================================================

  FUNCTION:  lte_static_ml1_get_intf

===========================================================================*/
/*!
  @brief
  Returns static interface for corresponding stack.

  @return
  void 
*/
/*=========================================================================*/
lte_static_ml1_ext_api_s * lte_static_ml1_get_intf
(
   void
);

/*===========================================================================

  FUNCTION:  lte_static_ml1_get_pre_init_data

===========================================================================*/
/*!
  @brief
  Gets LTE pre init ext data (M1 or NB1)

  @return
  void 
*/
/*=========================================================================*/
lte_static_ml1_pre_init_data_s* lte_static_ml1_get_pre_init_data
(
   void
);

/*===========================================================================

  FUNCTION:  lte_static_ml1_send_cphy_init_cnf

===========================================================================*/
/*!
    @brief
    ML1 static common function to send CPHY INIT CNF to RRC

    @details

    @return
*/
/*=========================================================================*/
errno_enum_type lte_static_ml1_send_cphy_init_cnf
(
  lte_as_task_mask_t     cnf_task_mask,
  errno_enum_type        status,
  boolean                init_cnf
);

/*===========================================================================

  FUNCTION:  lte_static_ml1_send_cphy_load_unload_cnf

===========================================================================*/
/*!
    @brief
    ML1 static common function to send CPHY INIT CNF to RRC

    @details

    @return
*/
/*=========================================================================*/
errno_enum_type lte_static_ml1_send_cphy_load_unload_cnf
(
  boolean                 is_load,
  lte_ml1_task_mask_t     cnf_task_mask,
  errno_enum_type         status
);

/*===========================================================================

  FUNCTION:  lte_static_ml1_send_cphy_load_unload_req

===========================================================================*/
/*!
    @brief
    ML1 static common function to send CPHY LOAD/UNLOAD REQ to ML1 modules

    @details

    @return
*/
/*=========================================================================*/
errno_enum_type lte_static_ml1_send_cphy_load_unload_req
(
  boolean is_load
);

/*===========================================================================

  FUNCTION:  lte_ml1_common_get_mgr_init_req_handler

===========================================================================*/
/*!
  @brief
  Returns a pointer to the CATM1 ML1 MGR CPHY INIT REQ handler

  @return
  void 
*/
/*=========================================================================*/
void lte_ml1_common_get_mgr_init_req_handler
(
  void
);

/*===========================================================================

  FUNCTION:  lte_ml1_common_get_mgr_deinit_req_handler

===========================================================================*/
/*!
  @brief
  Returns a pointer to the CATM1 ML1 MGR CPHY INIT REQ handler for DEINIT

  @return
  void 
*/
/*=========================================================================*/
void lte_ml1_common_get_mgr_deinit_req_handler
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_ml1_common_get_mgr_init_req_handler

===========================================================================*/
/*!
  @brief
  Returns a pointer to the CATM1 ML1 MGR CPHY INIT REQ handler

  @return
  void 
*/
/*=========================================================================*/
void nb1_ml1_common_get_mgr_init_req_handler
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_ml1_common_get_mgr_deinit_req_handler

===========================================================================*/
/*!
  @brief
  Returns a pointer to the CATM1 ML1 MGR CPHY INIT REQ handler for DEINIT

  @return
  void 
*/
/*=========================================================================*/
void nb1_ml1_common_get_mgr_deinit_req_handler
(
  void
);

/*===========================================================================

  FUNCTION:  lte_ml1_common_get_gm_init_req_handler

===========================================================================*/
/*!
  @brief
  Returns a pointer to the CATM1 ML1 GM CPHY INIT REQ handler

  @return
  void 
*/
/*=========================================================================*/
void lte_ml1_common_get_gm_init_req_handler
(
  void
);

/*===========================================================================

  FUNCTION:  lte_ml1_common_get_gm_deinit_req_handler

===========================================================================*/
/*!
  @brief
  Returns a pointer to the CATM1 ML1 GM CPHY INIT REQ handler for DEINIT

  @return
  void 
*/
/*=========================================================================*/
void lte_ml1_common_get_gm_deinit_req_handler
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_ml1_common_get_gm_init_req_handler

===========================================================================*/
/*!
  @brief
  Returns a pointer to the NB1 ML1 GM CPHY INIT REQ handler

  @return
  void 
*/
/*=========================================================================*/
void nb1_ml1_common_get_gm_init_req_handler
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_ml1_common_get_gm_deinit_req_handler

===========================================================================*/
/*!
  @brief
  Returns a pointer to the NB1 ML1 GM CPHY INIT REQ handler for DEINIT

  @return
  void 
*/
/*=========================================================================*/
void nb1_ml1_common_get_gm_deinit_req_handler
(
  void
);

/*===========================================================================

  FUNCTION:  lte_ml1_common_get_offload_init_req_handler

===========================================================================*/
/*!
  @brief
  Returns a pointer to the CATM1 ML1 OFFLOAD CPHY INIT REQ handler

  @return
  void 
*/
/*=========================================================================*/
void lte_ml1_common_get_offload_init_req_handler
(
  void
);

/*===========================================================================

  FUNCTION:  lte_ml1_common_get_offload_deinit_req_handler

===========================================================================*/
/*!
  @brief
  Returns a pointer to the CATM1 ML1 OFFLOAD CPHY INIT REQ handler for DEINIT

  @return
  void 
*/
/*=========================================================================*/
void lte_ml1_common_get_offload_deinit_req_handler
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_ml1_common_get_offload_init_req_handler

===========================================================================*/
/*!
  @brief
  Returns a pointer to the NB1 ML1 OFFLOAD CPHY INIT REQ handler

  @return
  void 
*/
/*=========================================================================*/
void nb1_ml1_common_get_offload_init_req_handler
(
  void
);

/*===========================================================================

  FUNCTION:  nb1_ml1_common_get_offload_deinit_req_handler

===========================================================================*/
/*!
  @brief
  Returns a pointer to the NB1 ML1 OFFLOAD CPHY INIT REQ handler for DEINIT

  @return
  void 
*/
/*=========================================================================*/
void nb1_ml1_common_get_offload_deinit_req_handler
(
  void
);

/*===========================================================================

  FUNCTION:  lte_static_ml1_get_bplmn_data

===========================================================================*/
/*!
    @brief
    ML1 static common function to get data from stored database

    @details

    @return
*/
/*=========================================================================*/

void lte_static_ml1_get_bplmn_data(lte_static_ml1_bplmn_data_s *bplmn_data);

/*===========================================================================

  FUNCTION:  lte_static_ml1_update_bplmn_data

===========================================================================*/
/*!
    @brief
    ML1 static common function to update data to SHIM layer

    @details

    @return
*/
/*=========================================================================*/

void lte_static_ml1_update_bplmn_data(lte_static_ml1_bplmn_data_s *bplmn_data);

/*===========================================================================
FUNCTION lte_static_ml1_common_get_ulog_handle

DESCRIPTION
  Returns ULOG Handle for LTE ML1
  
DEPENDENCIES
  None

RETURN VALUE
  ULogHandle

SIDE EFFECTS
  None
===========================================================================*/
ULogHandle lte_static_ml1_common_get_ulog_handle( void );

/*===========================================================================

  FUNCTION:  ds_nb1_jammer_detect_cb_register

===========================================================================*/
/*!
    @brief
    This function registers the callback function for jammer detection functionality

    @dependencies
    None.

    @sideeffects
    None.

    @return
    TRUE if registration is success, FALSE otherwise
*/
/*=========================================================================*/
void ds_lte_jammer_detect_cb_register(lte_ds_jammer_detect_cback_type lte_ds_send_jamming_sts_cb);

#define LTE_ML1_ULOG_0(fmt)       \
if(lte_static_ml1_common_get_ulog_handle() != NULL)  \
{                                          \
  ULOG_RT_PRINTF_0(lte_static_ml1_common_get_ulog_handle(), (fmt));  \
}

#define LTE_ML1_ULOG_1(fmt, a)       \
if(lte_static_ml1_common_get_ulog_handle() != NULL)  \
{                                          \
  ULOG_RT_PRINTF_1(lte_static_ml1_common_get_ulog_handle(), (fmt), (a) );  \
}

#define LTE_ML1_ULOG_2(fmt, a, b)       \
if(lte_static_ml1_common_get_ulog_handle() != NULL)  \
{                                          \
  ULOG_RT_PRINTF_2(lte_static_ml1_common_get_ulog_handle(), (fmt), (a), (b));  \
}

#define LTE_ML1_ULOG_3(fmt, a, b, c)       \
if(lte_static_ml1_common_get_ulog_handle() != NULL)  \
{                                          \
  ULOG_RT_PRINTF_3(lte_static_ml1_common_get_ulog_handle(), (fmt), (a), (b), (c) );  \
}

#define LTE_ML1_ULOG_4(fmt, a, b, c, d)       \
if(lte_static_ml1_common_get_ulog_handle() != NULL)  \
{                                          \
  ULOG_RT_PRINTF_4(lte_static_ml1_common_get_ulog_handle(), (fmt), (a), (b), (c), (d) );  \
}

#define LTE_ML1_ULOG_5(fmt, a, b, c, d, e)       \
if(lte_static_ml1_common_get_ulog_handle() != NULL)  \
{                                          \
  ULOG_RT_PRINTF_5(lte_static_ml1_common_get_ulog_handle(), (fmt), (a), (b), (c), (d), (e) );  \
}

#define LTE_ML1_ULOG_6(fmt, a, b, c, d, e, f)       \
if(lte_static_ml1_common_get_ulog_handle() != NULL)  \
{                                          \
  ULOG_RT_PRINTF_6(lte_static_ml1_common_get_ulog_handle(), (fmt), (a), (b), (c), (d), (e), (f) );  \
}

#define LTE_ML1_ULOG_7(fmt, a, b, c, d, e, f, g)       \
if(lte_static_ml1_common_get_ulog_handle() != NULL)  \
{                                          \
  ULOG_RT_PRINTF_7(lte_static_ml1_common_get_ulog_handle(), (fmt), (a), (b), (c), (d), (e), (f), (g) );  \
}

#define LTE_ML1_ULOG_8(fmt, a, b, c, d, e, f, g, h)       \
if(lte_static_ml1_common_get_ulog_handle() != NULL)  \
{                                          \
  ULOG_RT_PRINTF_8(lte_static_ml1_common_get_ulog_handle(), (fmt), (a), (b), (c), (d), (e), (f), (g), (h) );  \
}

#define LTE_ML1_ULOG_9(fmt, a, b, c, d, e, f, g, h, i)       \
if(lte_static_ml1_common_get_ulog_handle() != NULL)  \
{                                          \
  ULOG_RT_PRINTF_9(lte_static_ml1_common_get_ulog_handle(), (fmt), (a), (b), (c), (d), (e), (f), (g), (h), (i) );  \
}
#ifdef TEST_FRAMEWORK
#error code not present
#endif
#endif /* LTE_STATIC_ML1_COMMON_H */

