#ifndef L1_EDRX_PEO_DEFINES_H
#define L1_EDRX_PEO_DEFINES_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L 1   PEO DEFINES  H E A D E R   F I L E

DESCRIPTION
   Layer 1 PEO defines.


Copyright (c) 2001-2015 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gl1/inc/l1_edrx_peo_defines.h#1 $
$DateTime: 2018/02/08 01:15:27 $ $Author: pwbldsvc $

when       who      what, where, why
--------   -------- --------------------------------------------- 
18/07/17    nm       CR2040158 Optimization to reschedule BCCH if not enough time for timing sync
11/07/17    nm       CR2038718 skip some PEO activities based on last successful Sync FN
04/04/17    nm       CR2025170 Skip an extra SCH done for EDRX >30 
15/03/17    nm       CR2019706 Fix static analysis erros
17/02/17    nm       CR2005498 EDRX/PEO Gl1 incorrectly updating timing error to GSTMR even though SCH decode fails
11/01/17    nm       CR1087215 FR 36703: Rel.13 GERAN Power Efficient Operation (PEO)
27/01/17    nm       CR1116718 correct featurization for SP build

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "l1_utils.h"
#include "l1_sc_int.h"
#include "gl1_msg_g.h"


#if defined (FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION)
/*===========================================================================

                     MACRO DEFINITIONS                                       

===========================================================================*/

#define NUM_POSSIBLE_EDRX_CYCLES 12
#define NUM_POSSIBLE_PEO_ACTIVITIES 6
#define NUM_POSSIBLE_PEO_ACTIVITIES_RACH 3

#define NO_OF_MEAS_SAMPLES_PEO 3

#define FRAMES_IN_300_MS 66
#define INVALID_FN               (0xFFFF)
#define PEO_INVALID_INDEX 0xFF

/*For eDRX > 30s*/
#define FRAMES_TO_DO_PEO_ACT_BEFORE_PAGE_HIGH_EDRX 3250 //currently 15 seconds before page, can be changed later

#define PEO_INDEX_HIGH_DRX_ACT_PAGE 0 //start from first activity i.e. monitor
#define PEO_INDEX_LOW_DRX_ACT_PAGE  3 //only reconf activity

/*For eDRX < 30s*/
#define FRAMES_TO_DO_PEO_ACT_BEFORE_PAGE_LOW_EDRX 65//300MS
#define PEO_MON_SPREAD_DURATION 210 //970MS
#define PEO_MON_OFFSET_FRAMES_LOW_EDRX (2*PEO_MON_SPREAD_DURATION + 3*PEO_ACT_DURATION_MON + 8*PEO_OVERHEAD_FRAMES) 
#define PEO_OVERHEAD_FRAMES 2
#define PEO_OVERHEAD_FRAMES_RACH 4 //to cater rude wake up duration

#define NUM_FRAMES_IN_1_SECOND  (217) 
#define NUM_FRAMES_IN_30_SECOND  (30*NUM_FRAMES_IN_1_SECOND) 
#define PEO_MIN_FRAMES_FOR_SYNC_LOSS 823//3.8sec seconds 
/* NOTE - 1ppm error means 1 micro sec delay in 1 sec; 3.8 sec gives 3.8 micro seconds 
***which is 1 GSM symbol.. We can cover 1 GSM symbol offset.
***with our current PCH burst decode where we have  a margin of 3 GSM symbols*/

#define PEO_MIN_FRAMES_FOR_SYNC_AGAIN 15 //min time for doing FCCH/SCH
#define PEO_ACT_DURATION_SCH 4
#define PEO_ACT_DURATION_MON 4

#define MAX_PAGE_COUNT_FOR_PEO_MEAS 17

#define L1_PEO_PWR_MAX_MEAS_PER_CALL  1

#define IS_PEO_ACT_VALID()\
 !((l1_peo_data.peo_activity_FN == INVALID_FN) && \
 (l1_peo_data.current_activity == PEO_ACT_NO_ACT))

#define IS_PEO_ACT_IN_PROGRESS()\
(IS_PEO_ACT_VALID() && \
( (l1_peo_data.act_status == PEO_ACT_STATUS_WAITING_FOR_CB) || \
  (l1_peo_data.act_status == PEO_ACT_STATUS_ABORTING)))


/*===========================================================================

                     ENUM/STRUCTURE DEFINITIONS                                       

===========================================================================*/

/*PEO feature current status*/
typedef enum {
   PEO_STATUS_INACTIVE = 0,
   PEO_STATUS_ACTIVE,
   PEO_STATUS_ABORTING, //leaving idle mode
   PEO_STATUS_GOING_INACTIVE,//intermediate state from Active to inactive
} l1_peo_status_T;

/*PEO Activity Type*/
typedef enum {
   PEO_ACT_NO_ACT = 0,
   PEO_ACT_MON,
   PEO_ACT_SCH_RECONF, //to be done before every page for reconfirmation for eDRX > 1.9sec
   PEO_ACT_FCCH_SCH_STAND_ALONE, //irrespective of whether previous SCH failed or not
   PEO_ACT_FCCH_SCH, //to be done only if SCH reconf fails
   PEO_ACT_CCCH,
} l1_peo_activity_T;

/*EDRX value enum*/
typedef enum {
   eDRX_1_9_SEC = 0,
   eDRX_3_8_SEC,
   eDRX_7_5_SEC,
   eDRX_12_2_SEC,
   eDRX_24_5_SEC,
   eDRX_49_SEC,
   eDRX_1_63_MIN,
   eDRX_3_25_MIN,
   eDRX_6_5_MIN,
   eDRX_13_MIN,
   eDRX_26_MIN,
   eDRX_52_MIN,
} l1_eDRX_cycle_T;

/*PEO activity status*/
typedef enum {
   PEO_ACT_STATUS_NULL = 0,
   PEO_ACT_STATUS_INIT,
   PEO_ACT_STATUS_WAIT,
   PEO_ACT_STATUS_WAITING_FOR_CB,
   PEO_ACT_STATUS_CB_RECEIVED,
   PEO_ACT_STATUS_ABORTING,
} l1_peo_activity_status_T;

/* L1 eDRX Data */
typedef struct {
  l1_eDRX_cycle_T current_eDRX;
  boolean eDRX_enabled;
}eDRX_data_T;

typedef struct
{
  gl1_msg_metrics_rpt      sch_metrics_rpt[4];
  boolean sch_decoded;
  boolean sch_data_cb_received;
  boolean sch_metrics_cb_received;
  gl1_msg_decode_rpt    data_rpt;
}l1_sch_data_peo_t;

typedef struct
{
  gl1_msg_rx_cb_type       rx_cb;
  sys_algo_agc_T          *agc_ptr;
  gl1_msg_decode_rpt       acq_decode_data_rpt;
  gl1_msg_rx_rpt           acq_rpt;
  boolean                  acq_success;
}l1_acq_data_peo_t;

typedef struct
{
  ARFCN_T                       arfcn[L1_PEO_PWR_MAX_MEAS_PER_CALL];
  sys_algo_agc_T                *agc_ptr[L1_PEO_PWR_MAX_MEAS_PER_CALL];
  gl1_msg_pwr_meas_result_type  pwr_meas_cb_param;
  dBx16_T                       pwr_meas_results[L1_PEO_PWR_MAX_MEAS_PER_CALL];
  boolean                       yielded[L1_PEO_PWR_MAX_MEAS_PER_CALL];
} l1_mon_data_peo_t;

/* L1 PEO Data */
typedef struct {
  l1_peo_status_T     status;
  l1_peo_activity_T   current_activity;
  l1_peo_activity_status_T act_status;

  /*current activity index for idle mode peo measurements before page*/
  uint8               peo_act_index;

  /*current activity index for peo activities to be run on getting SERV_MEAS_REQ*/
  uint8               peo_act_index_rach;

  uint32              peo_activity_FN;
  uint32              last_successful_sync_FN;
  eDRX_data_T         eDRX_data;

  l1_mon_data_peo_t   l1_mon_data;
  uint8               remaining_samples;
  uint8               drx_ref;
  boolean             serv_meas_req_rcd; //indicates if this PEO meas for mph_serv_meas_req/page, TRUE means the former
  l1_sch_data_peo_t   sch_data_peo;
  l1_acq_data_peo_t   l1_acq_data_peo;
  uint32              start_ACQ_FN;
  uint8               peo_page_count;
  boolean             abort_pending;
}peo_data_T;


#endif /*#if defined (FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION)*/
#endif /* L1_EDRX_PEO_DEFINES_H */

/*EOF*/
