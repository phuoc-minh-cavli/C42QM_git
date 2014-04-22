/*!
  @file
  ftm_common_autopin.c

  @brief
  This module contains implementation to acquire and release locks for critical
  section code execution
 
  @addtogroup RF_COMMON_FTM_AUTOPIN
  @{
*/

/*==============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rfa.mpss/3.10/rf/common/ftm/src/ftm_common_autopin.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/21/15   kma     Added CCS support for WTR capture
10/10/15   ag      Changes for correct Tx dev in WCDMA for Jolokia
10/08/15   ag      Fix W Tx device update sequence
09/30/15   kma     Initial revision

==============================================================================*/
#if 0
#include "ftm_common_autopin.h"
#include "rfcommon_core_utils.h"
#include "comdef.h"
#include "ftm.h"
#include "ftm_task.h"
#include "ftm_msg.h"
#include "ftmdiag.h"
#include "stringl.h" /* for memscpy */
#include "rflm_ccs_intf.h"

#ifdef FEATURE_LTE
#include "rflte_util.h"
#include "rflte_mc.h"
#include "rfdevice_lte_interface.h"
#include "rflte_ftm_mc.h"
#endif

#ifdef FEATURE_WCDMA
#include "rfwcdma_mc.h"
#include "ftm_hwtc_wcdma_ino.h"
#include "rfdevice_wcdma_intf.h"
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
#include "rfdevice_tdscdma_intf.h"
#include "rf_tdscdma_core_util.h"
#include "rf_tdscdma_mc.h"
#include "rflm_tds_ccs.h"
#include "rflm_tds_mc.h"
#include "rf_tdscdma_mdsp_rf_event.h"
extern rflm_tds_rf_mode_ctl_s rflmTdsRfModeCtlState;
#endif

#ifdef FEATURE_CDMA1X
#include "rf_cdma_data.h"
#include "rf_cdma_utils_freq.h"
#endif

/*Fixed 300us action time with respect to current USTMR*/
#define RF_AUTOPIN_CCS_ACTION_TIME ((uint32)(19200000/1000000.0*300 + 1))

extern uint32 * 
rflm_ccs_get_data_buf_item_ptr
( 
   rflm_ccs_data_buf_item_type data_buf_item
);

#ifdef FEATURE_FACTORY_TESTMODE

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_fbrx_init_tx
 
  @details
  This function tests the common FBRx init tx API
 
*/
ftm_rsp_pkt_type ftm_common_autopin_capture(ftm_common_pkt_type *cmd_ptr)
{
  ftm_autopin_response_type* response_ptr = NULL;
  ftm_autopin_capture_param_type* param_ptr = NULL;
  boolean capture_status = TRUE;
  rfdevice_fbrx_autopin_data_type wtr_capture_result;
  rfdevice_rxtx_common_class* device_ptr = NULL;
  ftm_rf_technology_type curr_ftm_rf_tech = FTM_RF_TECH_UNKNOWN;
  rfm_device_enum_type tx_device = RFM_DEVICE_0;
  rfcom_band_type_u band_union;
  uint32 refTime;
  uint32 *ccsDestPtr;

  #ifdef FEATURE_CDMA
  const rf_cdma_data_status_type *dev_status_r;
  #endif

  #ifdef FEATURE_TDSCDMA
  rflm_tds_rf_ccs_intf_s  *ccsIntfPtr = &(rflmTdsRfModeCtlState.ccsInfo.rfCcsIntf);
  rflm_tds_ccs_exec_single_task_info_t taskInfo;
  rftdscdma_mdsp_common_event_data_type rf_tds_build_event_data;
  #endif

  ftm_rsp_pkt_type rsp = ftmdiag_create_new_pkt(sizeof(ftm_autopin_response_type));

  if(rsp.pkt_payload == NULL)
  {
    FTM_MSG_3( FTM_ERROR, "Cannot allocate memory for response packet payload, cmd id = %u", cmd_ptr->ftm_hdr.cmd_id, 0, 0);
    return rsp;
  }

  response_ptr = (ftm_autopin_response_type*) rsp.pkt_payload;

  // Copy data from request to response
  memscpy(response_ptr, sizeof(ftm_composite_cmd_header_type), 
          cmd_ptr, sizeof(ftm_composite_cmd_header_type));

  param_ptr = (ftm_autopin_capture_param_type*)&(cmd_ptr->ftm_extended_params);

  curr_ftm_rf_tech = ftm_get_rf_technology();

  switch (curr_ftm_rf_tech)
  {
#if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA)
     case FTM_RF_TECH_CDMA:
       //tx_device = rf_cdma_data_get_curr_tx_device();
       dev_status_r = rf_cdma_get_device_status(tx_device);     // what is the current tx device?
       if (dev_status_r != NULL)
       {
         band_union.cdma_band = dev_status_r->curr_band;
         device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_1X_MODE, band_union);
       } else
       {
         FTM_MSG(FTM_ERROR, "rfcommon_core_get_tx_device_ptr(0) returns NULL");
       }
       break;
#endif

#ifdef FEATURE_WCDMA
     case FTM_RF_TECH_WCDMA:
       {

#ifdef ATLAS_RF_WCDMA_PRESI
         rfm_wcdma_ftm_radio_state_data_type radio_state;
         tx_device = rfm_wcdma_ftm_tx_get_rfm_device();
         if (tx_device < RFM_MAX_WAN_DEVICES)
         {
           rfm_wcdma_ftm_tx_get_radio_state(tx_device, &radio_state);
           band_union.wcdma_band = (rfcom_wcdma_band_type)radio_state.rf_band;
           device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_WCDMA_MODE, band_union);
         } else
         {
           device_ptr = NULL;
         }
#else
         tx_device = RFM_DEVICE_0;
         band_union.wcdma_band = rfwcdma_mc_state.car_path_state[RF_PATH_0][RFCOM_MULTI_CARRIER_ID__0].curr_band_tx;
         device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_WCDMA_MODE, band_union);
#endif 

       }
       break;
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
     case FTM_RF_TECH_LTE:
       band_union.lte_band = rflte_ftm_mc_sm_get_band();
       tx_device = rflte_ftm_mc_sm_get_tx_device();
       device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_LTE_MODE, band_union);
       break;
#endif
#ifdef FEATURE_TDSCDMA
     case FTM_RF_TECH_TDSCDMA:
       band_union.tdscdma_band = rf_tdscdma_mc_state.curr_band_tx;
       //tx_device = rf_tdscdma_mc_get_curr_tx_device();
       device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_TDSCDMA_MODE, band_union);
       break;
#endif
     default:
       FTM_MSG(FTM_ERROR, "ftm_common_autopin_capture: Unsupported Tech Type");
  }

  switch (param_ptr->capture_type)
  {
     case FTM_AUTOPIN_PAR_WTR_CAPTURE:
       capture_status &= rfdevice_cmn_fbrx_autopin_enable(device_ptr,
                                                          RFDEVICE_EXECUTE_IMMEDIATE,
                                                          NULL,
                                                          param_ptr->fbrx_gain_state);
       if (capture_status)
       {
         capture_status &= rfdevice_cmn_fbrx_autopin_read_and_disable(device_ptr,&wtr_capture_result);
       }
       else
       {
         FTM_MSG_1( FTM_HIGH, "ftm_common_autopin_capture: WTR capture trigger status %d", capture_status);
       }

       if (capture_status)
       {
         response_ptr->wtr_capture_response.sum_i_q_sqr = wtr_capture_result.sum_i_q_sqr;
         response_ptr->wtr_capture_response.sum_i_and_q_sqr_ave = wtr_capture_result.sum_i_and_q_sqr_ave;
         response_ptr->wtr_capture_response.p_ave = wtr_capture_result.p_ave;
         response_ptr->wtr_capture_response.sum_i = wtr_capture_result.sum_i;
         response_ptr->wtr_capture_response.sum_q = wtr_capture_result.sum_q;
         response_ptr->wtr_capture_response.i_dc = wtr_capture_result.i_dc;
         response_ptr->wtr_capture_response.q_dc = wtr_capture_result.q_dc;
         response_ptr->wtr_capture_response.fbrx_accum = wtr_capture_result.fbrx_accum;
         response_ptr->wtr_capture_response.sum_peaks = wtr_capture_result.sum_peaks;
         response_ptr->wtr_capture_response.num_peaks = wtr_capture_result.num_peaks;
         response_ptr->wtr_capture_response.par = wtr_capture_result.par;
         FTM_MSG_4( FTM_HIGH, "ftm_common_autopin_capture: PAR %d, Number of peaks %d, sum of peaks %d, Avg power %d", 
                    wtr_capture_result.par, 
                    wtr_capture_result.num_peaks,
                    wtr_capture_result.sum_peaks,
                    wtr_capture_result.p_ave );
         FTM_MSG_4( FTM_HIGH, "ftm_common_autopin_capture: sum i %d, sum q %d, i dc %d, q dc %d", 
                    wtr_capture_result.sum_i, 
                    wtr_capture_result.sum_q,
                    wtr_capture_result.i_dc,
                    wtr_capture_result.q_dc );
       }
       response_ptr->status = capture_status;
       
       break;

     case FTM_AUTOPIN_PAR_WTR_REFLOG_CAPTURE:
       switch (curr_ftm_rf_tech)
       {
       #if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA)
          case FTM_RF_TECH_CDMA:

            break;
       #endif

       #ifdef FEATURE_WCDMA
          case FTM_RF_TECH_WCDMA:

            break;
       #endif /* FEATURE_WCDMA */

       #ifdef FEATURE_LTE
          case FTM_RF_TECH_LTE:

            break;
       #endif

       #ifdef FEATURE_TDSCDMA
          case FTM_RF_TECH_TDSCDMA:
            /*Build CCS event*/
            //rf_tds_build_event_data.tx_device = rf_tdscdma_mc_state.curr_tx_device;
            rf_tds_build_event_data.band = rf_tdscdma_mc_state.curr_band_tx;
            //rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_BUILD_AUTOPIN, &rf_tds_build_event_data);

            #ifdef FEATURE_TABASCO_MODEM
            /*Acquire current USTMR time and add an offset*/
            refTime = HWIO_IN(STMR_TIME_RD) + RF_AUTOPIN_CCS_ACTION_TIME;
            #endif

            #ifdef FEATURE_JOLOKIA_MODEM
            /*Acquire current USTMR time and add an offset*/
            refTime = HWIO_IN(UNIV_STMR_MSTMR) + RF_AUTOPIN_CCS_ACTION_TIME;
            #endif

            /*Hard code TQ0*/
            ccsDestPtr = rflm_ccs_get_data_buf_item_ptr((rflm_ccs_data_buf_item_type)0);
            FTM_MSG_1( FTM_HIGH, "ftm_common_autopin_capture: Enqueue at USTMR time %d", refTime);

            //ccsDestPtr = rflm_tds_ccs_event_commit(rflmTdsRfModeCtlState.script.autopinWriteScriptPtrArray[param_ptr->fbrx_gain_state-1], 
              //                                     ccsDestPtr,
                //                                   ccsIntfPtr->ccsRfPairPtr.tq0_handle,
                  //                                 refTime,
                    //                               ftm_common_autopin_wtr_callback,
                      //                             &taskInfo,
                        //                           NULL,
                          //                         NULL);

            //FTM_MSG_1( FTM_HIGH, "ftm_common_autopin_capture: Enqueue is done at USTMR time %d", HWIO_IN(STMR_TIME_RD));
            break;
       #endif

          default:
            FTM_MSG(FTM_ERROR, "ftm_common_autopin_capture: Unsupported Tech Type");
       }
       break;

     case FTM_AUTOPIN_PAR_FETCH_RESULTS:
       /*FET WTR PAR result*/
       capture_status &= rfdevice_cmn_fbrx_autopin_read_and_disable(device_ptr,&wtr_capture_result);

       if (capture_status)
       {
         response_ptr->wtr_capture_response.sum_i_q_sqr = wtr_capture_result.sum_i_q_sqr;
         response_ptr->wtr_capture_response.sum_i_and_q_sqr_ave = wtr_capture_result.sum_i_and_q_sqr_ave;
         response_ptr->wtr_capture_response.p_ave = wtr_capture_result.p_ave;
         response_ptr->wtr_capture_response.sum_i = wtr_capture_result.sum_i;
         response_ptr->wtr_capture_response.sum_q = wtr_capture_result.sum_q;
         response_ptr->wtr_capture_response.i_dc = wtr_capture_result.i_dc;
         response_ptr->wtr_capture_response.q_dc = wtr_capture_result.q_dc;
         response_ptr->wtr_capture_response.fbrx_accum = wtr_capture_result.fbrx_accum;
         response_ptr->wtr_capture_response.sum_peaks = wtr_capture_result.sum_peaks;
         response_ptr->wtr_capture_response.num_peaks = wtr_capture_result.num_peaks;
         response_ptr->wtr_capture_response.par = wtr_capture_result.par;
         FTM_MSG_4( FTM_HIGH, "ftm_common_autopin_capture: PAR %d, Number of peaks %d, sum of peaks %d, Avg power %d", 
                    wtr_capture_result.par, 
                    wtr_capture_result.num_peaks,
                    wtr_capture_result.sum_peaks,
                    wtr_capture_result.p_ave );
         FTM_MSG_4( FTM_HIGH, "ftm_common_autopin_capture: sum i %d, sum q %d, i dc %d, q dc %d", 
                    wtr_capture_result.sum_i, 
                    wtr_capture_result.sum_q,
                    wtr_capture_result.i_dc,
                    wtr_capture_result.q_dc );
       }
       else
       {
         FTM_MSG(FTM_ERROR, "ftm_common_autopin_capture: Failed to fetch WTR PAR result");
       }
       response_ptr->status = capture_status;
       
       break;

     default:
       FTM_MSG_1( FTM_HIGH, "ftm_common_autopin_capture: Unsupported capture type %d", param_ptr->capture_type);

  }

  return rsp;
}

void ftm_common_autopin_wtr_callback(rflm_ccs_rf_tq_handle_t *tq, uint32 task_idx, ccs_rf_task_status_t status, void *arg)
{
  //Pending implementation
  FTM_MSG_1( FTM_HIGH, "ftm_common_autopin_capture: WTR PAR Capture executed %d", NULL);
  return;
}


#endif /*FEATURE_FACTORY_TESTMODE */


#endif
