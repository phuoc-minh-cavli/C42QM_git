#ifndef GL1_MSGRIF_INIT_H
#define GL1_MSGRIF_INIT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  GL1 MSGR Interface Initialisation
                       Header File

GENERAL DESCRIPTION
   This header file contains declarations relevant to L1 task processing.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gmsgrif/static/inc/gl1_msgrif_init.h#3 $ $DateTime: 2020/03/29 06:09:26 $ $Author: pwbldsvc $

when       who      what, where, why
--------   -------- ---------------------------------------------
27/03/20   ktg      CR2575302 FR 58047: FSK based alarm reporting on jamming detection for IIOT
17/11/14   pg       CR729410 make DRX exit mode MSGR RSP handling non-blocking 
27/10/14   am       CR646584 WLAN coex
07/04/14   cja      CR644445 Mainline feature FEATURE_GSM_ASYNC_FW_INTF
19/03/14   cja      CR632037 Use RF Task for X2G
15/02/14   ip/npt  CR547955: Define multi thread usage on wake up from sleep
01/11/13   pa	   CR569692: X2G via RF Task. 
14/10/13   pa      CR536820: Added GL1 Msgr If Task. Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#include "comdef.h"
#include "msgr_types.h"
#include "queue.h"
#include "rfgsm_msg.h"
#include "msgr_geran.h"


/*===========================================================================

                     DATA STRUCTURES

===========================================================================*/

#define GL1_MSGRIF_RF_TASK_MSGR_QUEUE_SIZE 13
#ifdef FEATURE_WLAN_COEX_SW_CXM
#define NUM_GERAN_GFW_ROUTER_MSGS               4
#else
#ifdef FEATURE_FSK_SUPPORT
#define NUM_GERAN_GFW_ROUTER_MSGS               5
#else
#define NUM_GERAN_GFW_ROUTER_MSGS               3
#endif
#endif

typedef union
{
  msgr_hdr_struct_type                   hdr;
#if defined FEATURE_GSM_MULTI_THREAD_WAKEUP
  rfa_rf_gsm_wakeup_cnf_s                rfa_rf_gsm_wakeup_cnf;
#endif
  rfa_rf_gsm_rx_burst_cnf_s              rfa_rf_gsm_rx_burst_cnf;
  rfa_rf_gsm_set_tx_band_cnf_s           rfa_rf_gsm_tx_band_cnf;
  rfa_rf_gsm_tx_burst_cnf_s              rfa_rf_gsm_set_tx_burst_cnf;
  rfa_rf_gsm_get_timing_infor_cnf_s      rfa_rf_gsm_get_timing_infor_cnf;
  rfa_rf_gsm_idle_frame_processing_cnf_s rfa_rf_gsm_idle_frame_processing_cnf;
  rfa_rf_gsm_vreg_on_cnf_s               rfa_rf_gsm_vreg_on_cnf;
  rfa_rf_gsm_vreg_off_cnf_s              rfa_rf_gsm_vreg_off_cnf;
  rfa_rf_gsm_cm_enter_cnf_s              rfa_rf_gsm_cm_enter_cnf;
  rfa_rf_gsm_cm_exit_cnf_s               rfa_rf_gsm_cm_exit_cnf;
#ifdef GERAN_L1_IP2CAL
  rfa_rf_gsm_start_ip2_cal_cnf_s         rfa_rf_gsm_start_ip2_cal_cnf;
#endif
#ifdef GERAN_L1_HLLL_LNASTATE
  rfa_rf_gsm_process_burst_metrics_cnf_s rfa_rf_gsm_get_metrics_cnf;
#endif
} u_gl1_msgrif_rf_task_msgr;


typedef struct
{
  q_link_type           link;
  u_gl1_msgrif_rf_task_msgr  msg;

} gl1_msgrif_rf_task_msgr_t;

#define  GL1_MSGRIF_GFW_ASYNC_MSGR_QUEUE_SIZE  15

typedef union
{
  msgr_hdr_struct_type              hdr;
  GfwHostAsyncRsp                   gfwHostAsyncRsp;
  GfwHostSleepRsp                   gfwHostSleepRsp;
  GfwHostRfmExitModeRsp             gfwHostRfmExitModeRsp;
#ifdef FEATURE_WLAN_COEX_SW_CXM
  GfwWlanCxmConfigRsp               gfwWlanCxmConfigRsp;
#endif
} u_gl1_msgrif_gfw_async_msgr;

typedef struct
{
  q_link_type       link;
  u_gl1_msgrif_gfw_async_msgr  msg;
} gl1_msgrif_gfw_async_msgr_t;

#ifdef FEATURE_FSK_SUPPORT
#define GL1_MSGRIF_QMI_MSGR_QUEUE_SIZE 13

typedef union
{
  msgr_hdr_struct_type                   hdr;
  geran_fsk_debug_req_type               geran_fsk_debug_req;
  geran_fsk_debug_query_req_type         geran_fsk_debug_query_req;
  geran_fsk_data_req_type                geran_fsk_data_req;
  geran_fsk_hop_table_req_type           geran_fsk_hop_table_req;
  geran_fsk_hop_table_query_req_type     geran_fsk_hop_table_query_req;
  geran_fsk_start_req_type               geran_fsk_start_req;
  geran_fsk_stop_req_type                geran_fsk_stop_req;
  geran_fsk_pcl_req_type                 geran_fsk_pcl_req;
  geran_fsk_pcl_query_req_type           geran_fsk_pcl_query_req;
} u_gl1_msgrif_qmi_msgr;


typedef struct
{
  q_link_type           link;
  u_gl1_msgrif_qmi_msgr  msg;
} gl1_msgrif_qmi_msgr_t;
#endif

/*===========================================================================

                         FUNCTION DECLARATIONS

===========================================================================*/

void gl1_msgrif_initialise(void);
void gl1_msgrif_task_mutex_unlock(void);
void gl1_msgrif_task_mutex_lock(void);

void gl1_msgrif_init_rf_task_msgr_intf(void);
void gl1_msgrif_init_async_msgr_intf(void);
#ifdef FEATURE_FSK_SUPPORT
void gl1_msgrif_init_qmi_msgr_intf(void);
#endif
extern void gl1_msgrif_init_teardown(void);

#endif /* GL1_MSGRIF_INIT_H */
