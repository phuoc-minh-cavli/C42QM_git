/*!
  @file lte_static_ml1_ext_api.h

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

$Header: //components/rel/lte.mpss/4.7.2/api/lte_static_ml1_ext_api.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/04/16   sc      Initial Revision

===========================================================================*/

#ifndef LTE_STATIC_ML1_EXT_API_H
#define LTE_STATIC_ML1_EXT_API_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <lte_cphy_rssi_msg.h>
#include <nb1_cphy_rssi_msg.h>
#include <lte_cphy_msg.h>
#include "lte_ml1_ext_api.h"
#include "lte_rrc_ext_msg.h"

#define LTE_STATIC_ASSERT( exp ) \
/*lint -save -e506 -e774 */ \
if(LTE_STATIC_UNLIKELY_TRUE(!(exp)) ) \
{ \
  ERR_FATAL( "Assert " #exp " failed: ", 0, 0, 0 ); \
} \

#if defined(__GNUC__) && !defined(_lint)
  #define LTE_STATIC_LIKELY_TRUE(x) __builtin_expect((x), 1)
  #define LTE_STATIC_UNLIKELY_TRUE(x) __builtin_expect((x), 0)
#else
  #define LTE_STATIC_LIKELY_TRUE(x) (x)
  #define LTE_STATIC_UNLIKELY_TRUE(x) (x)
#endif /* ! (__GNUC__) */

typedef lte_api_signal_info_s_type (*common_rssi_ind_req_t)
(
  void
);

typedef lte_ml1_mgr_cm_ml1_mgr_info_s* (*lte_ml1_common_cmapi_ml1_mgr_req_t)
(
  void
);

typedef nb1_ml1_mgr_cm_ml1_mgr_info_s* (*nb1_ml1_common_cmapi_ml1_mgr_req_t)
(
  void
);

typedef lte_ml1_gm_cm_ml1_gm_info_s* (*lte_ml1_common_cmapi_ml1_gm_req_t)
(
  void
);

typedef nb1_ml1_gm_cm_ml1_gm_info_s* (*nb1_ml1_common_cmapi_ml1_gm_req_t)
(
  void
);

typedef uint16 (*ml1_get_PCC_doppler_measurement_t)
(
  void
);

typedef void (*ml1_gps_request_timetag)
(
  boolean
);

typedef void (*common_nbr_rssi_ind_req_t)
(
  cmapi_lte_ngbr_ind_s*
);

typedef void (*ml1_gps_request_gts_t)
(
  void
);

typedef void (*ml1_get_mgr_init_req_handle_t)
(
  void
);

typedef void (*ml1_get_mgr_deinit_req_handle_t)
(
  void
);

typedef void (*ml1_init_req_t)
(
  void
);

typedef void (*ml1_deinit_req_t)
(
  void
);

/* Function pointer to init CATM/NB GM task */
typedef void (*ml1_gm_init_req_t)( void );

/* Function pointer to deinit CATM/NB GM task */
typedef void (*ml1_gm_deinit_req_t)( void );

/* Function pointer to deinit CATM/NB GM task */
typedef void (*ml1_register_gps_event_t)( lte_ml1_gps_event_cb_t );

typedef void (*ml1_rssi_cmapi_update_tx_global_t)
(
  void
);

/* Function pointer to init CATM/NB offload task */
typedef void (*ml1_offload_init_req_t)( void );

/* Function pointer to deinit CATM/NB offload task */
typedef void (*ml1_offload_deinit_req_t)( void );

typedef void ( *ml1_mgr_handle_ce_info_rpt_req_t )( lte_cphy_ce_info_rpt_req_s* );

typedef void ( *ml1_mgr_handle_get_next_sfn_req_t )( lte_cphy_get_next_sfn_req_s* );

typedef void ( *ml1_mgr_handle_set_sfn_config_req_t )( lte_cphy_set_sfn_config_req_s* );

typedef void ( *ml1_mgr_handle_ce_level_rpt_req_t )( lte_cphy_ce_level_rpt_req_s* );

typedef void ( *ml1_mgr_handle_serv_meas_rpt_req_t )( lte_rrc_rx_power_measurement_s_type * );



typedef struct
{
  common_rssi_ind_req_t                   ml1_common_rssi_ind_req;
  ml1_gps_request_timetag                 ml1_gps_request_timetag_int;
  common_nbr_rssi_ind_req_t               ml1_common_nbr_rssi_ind_req;
  ml1_get_mgr_init_req_handle_t           ml1_get_mgr_init_req_handle;
  ml1_get_mgr_deinit_req_handle_t         ml1_get_mgr_deinit_req_handle;
  ml1_gm_init_req_t                       ml1_gm_init_req;
  ml1_gm_deinit_req_t                     ml1_gm_deinit_req;
  ml1_register_gps_event_t                ml1_register_gps_event;
  lte_ml1_common_cmapi_ml1_mgr_req_t      m1_common_cmapi_ml1_mgr_req;
  nb1_ml1_common_cmapi_ml1_mgr_req_t      nb1_common_cmapi_ml1_mgr_req;
  lte_ml1_common_cmapi_ml1_gm_req_t       m1_common_cmapi_ml1_gm_req;
  nb1_ml1_common_cmapi_ml1_gm_req_t       nb1_common_cmapi_ml1_gm_req;
  ml1_get_PCC_doppler_measurement_t       ml1_get_PCC_doppler_measurement;
  ml1_rssi_cmapi_update_tx_global_t       ml1_rssi_cmapi_update_tx_global;
  ml1_gps_request_gts_t                   lte_ml1_gps_request_gts;
  ml1_offload_init_req_t                  ml1_offload_init_req;
  ml1_offload_deinit_req_t                ml1_offload_deinit_req;
  ml1_mgr_handle_ce_info_rpt_req_t        ml1_mgr_handle_ce_info_rpt_req;
  ml1_mgr_handle_get_next_sfn_req_t       ml1_mgr_handle_get_next_sfn_req;
  ml1_mgr_handle_set_sfn_config_req_t     ml1_mgr_handle_set_sfn_config_req;
  ml1_mgr_handle_ce_level_rpt_req_t       ml1_mgr_handle_ce_level_rpt_req;
  ml1_mgr_handle_serv_meas_rpt_req_t      ml1_mgr_handle_serv_meas_rpt_req;
  
} lte_static_ml1_ext_api_s;

/*===========================================================================

  FUNCTION:  lte_static_ml1_mgr_init_req

===========================================================================*/
/*!
    @brief
    Initializes ML1 MGR for appropriate inerface

    @return
    boolean.

*/
/*=========================================================================*/

extern void lte_static_ml1_mgr_init_req(void);

/*===========================================================================

  FUNCTION:  lte_static_ml1_mgr_deinit_req

===========================================================================*/
/*!
    @brief
    Deinitializes ML1 MGR for appropriate inerface

    @return
    boolean.

*/
/*=========================================================================*/

extern void lte_static_ml1_mgr_deinit_req(void);

/*===========================================================================

  FUNCTION:  lte_ml1_get_lte_vtbl

===========================================================================*/
/*!
  @brief
  Returns a pointer to the LTE vtbl of externally exported APIs

  @return
  void 
*/
/*=========================================================================*/
lte_static_ml1_ext_api_s *lte_ml1_get_lte_vtbl
(
  void
);

#endif /* LTE_STATIC_ML1_EXT_API_H */

