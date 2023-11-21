/*==============================================================================

                     GERAN ENG MODE DATA READ AND WRITE  F U N C T I O N S

GENERAL DESCRIPTION

      This module contains the GERAN ENG MODE DATA READ AND WRITE FUNCTIONS

EXTERNALIZED FUNCTIONS

      geran_eng_mode_data_write()
      geran_eng_mode_data_read()


Copyright (c) 2005-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gcommon/static/src/geran_static_eng_mode_info.c#4 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
03/12/14   npt      CR759467 Fix compiler warnings
26/11/14   jj       CR 760176 fix  compiler warning
17/05/12   cgc      CR332796  add support for Tx power pa info api

==============================================================================*/

/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/
#include "geran_variation.h"
#include "geran_eng_mode_info.h"
#include "comdef.h"
#include "string.h"
#include <stringl.h>
#include "msg.h"
#include "gllc.h"
#include "gllc_v.h"
#include "gllc_v_g.h"
#include "grlclog.h"
#include "cm.h"
#include "geran_msgs.h"
#include "geran_multi_sim.h"
#include "rr_nv_g.h"

/*==============================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  External Functions
==============================================================================*/
/*
** declare locally, include l1_log.h requires scons update
*/
extern uint8 l1_log_get_TxPa_state( void );   /* CMAPI feature for Tx pa gain state */
extern void l1_log_convert_txlev_to_pwr_dBm(eng_mode_sys_band_T band, uint8 txlev, uint8 *power_out);
extern void l1_log_signal_info(geran_eng_mode_signal_info_t *signal_info_ptr);

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/

/* geran_eng_mode_info_type_t info types that can support the call back mechanism */
const geran_eng_mode_info_type_t supported_call_backs[] = {ENG_MODE_SCELL_INFO,
                                                           ENG_MODE_CODEC_HO_INFO,
                                                           ENG_MODE_SCELL_MEASUREMENTS,
                                                           ENG_MODE_NMR_INFO,
                                                           ENG_MODE_CANDIDATE_PLMN_ID};

/* geran_eng_mode_info_type_t info types that can support the call back behaviour modifiers */
const geran_eng_mode_info_type_t supported_cb_behaviour_modifier[] = {ENG_MODE_SCELL_MEASUREMENTS,
                                                                      ENG_MODE_NMR_INFO};

static geran_eng_mode_data_t geran_eng_mode_info[MAX_AS_IDS] = {0};

/*==============================================================================

                    FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/


/*===========================================================================
===
===  FUNCTION    geran_get_eng_mode_data_ptr()
===
===  DESCRIPTION
===  Returns a pointer to engmode data space using the current as_id
===  Note: AS-ID is used, not GAS-ID
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    geran_eng_mode_data_t*
===
===  SIDE EFFECTS
===
===
===========================================================================*/
geran_eng_mode_data_t* geran_get_eng_mode_data_ptr(sys_modem_as_id_e_type as_id)
{
  geran_eng_mode_data_t *eng_mode_data_ptr;

  /* validate the AS ID */
  if (as_id < MAX_AS_IDS)
  {
    eng_mode_data_ptr = &geran_eng_mode_info[as_id];
  }
  else
  {
    MSG_GERAN_ERROR_1("Eng mode bad as_id:%d",(int)as_id);
    eng_mode_data_ptr = NULL;
  }

  return(eng_mode_data_ptr);
}

/*==========================================================================

  FUNCTION    geran_eng_mode_data_read

  DESCRIPTION

    This function is intended to read the eng mode value from public store. This function will give
    requested eng mode info value if it is valid otherwise it will return the invalid value.


  PARAMETERS

    geran_eng_mode_info_type:  geran_eng_mode_info_type_t type
    param_copy            :   void

  DEPENDENCIES
    None.

  RETURN VALUE
    void

  SIDE EFFECTS
    None.

===========================================================================*/
void geran_eng_mode_data_read(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                              void *param_copy)
{
  geran_multisim_eng_mode_data_read(geran_eng_mode_info_type,
                                    param_copy,
                                    SYS_MODEM_AS_ID_1);
}


/*==========================================================================

  FUNCTION    geran_multisim_eng_mode_data_read

  DESCRIPTION

    This function is intended to read the eng mode value from public store. This function will give
    requested eng mode info value if it is valid otherwise it will return the invalid value.


  PARAMETERS

    geran_eng_mode_info_type:  geran_eng_mode_info_type_t type
    param_copy            :   void

  DEPENDENCIES
    None.

  RETURN VALUE
    void

  SIDE EFFECTS
    None.

===========================================================================*/
void geran_multisim_eng_mode_data_read(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                              void *param_copy,
                              sys_modem_as_id_e_type asubs_id)
{
  geran_eng_mode_data_t *eng_mode_data_ptr = geran_get_eng_mode_data_ptr(asubs_id);

  if (eng_mode_data_ptr == NULL)
  {
    return;
  }

  MSG_GERAN_HIGH_3_G("Eng mode read type %d, req mask %d, updated mask %d",
                     geran_eng_mode_info_type,
                     eng_mode_data_ptr->geran_stats_info.requested_bitmask.geran_modem_stats_mask,
                     eng_mode_data_ptr->geran_eng_mode_info.updated_info.geran_modem_stats_mask);

  if (eng_mode_data_ptr->store_initialised == FALSE)
  {
    geran_eng_initialise_store(asubs_id);
    eng_mode_data_ptr->store_initialised = TRUE;
  }

  switch(geran_eng_mode_info_type)
  {
    case ENG_MODE_TIMING_ADVANCE:
        *((uint32 *) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.timing_advance_value;
    break;

    case ENG_MODE_RADIO_LINK_TIMEOUT_VALUE:
      *((uint32 *) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.radio_link_timeout_value;
    break;

#ifndef FEATURE_GERAN_REDUCED_DEBUG
    case ENG_MODE_TOTAL_UPLINK_LLC_PDUS_TRANSMITTED:
      {
        const gllc_diag_pdu_stat_rsp_t  *gllc_diag_pdu_stat_rsp;
        uint32 total_l3pdu_tx_cnt = 0;
        uint8 sapi;

        for (sapi=3; sapi<=11; sapi+=2)
        {
          gllc_diag_pdu_stat_rsp = gllc_diag_get_pdu_stats(sapi);
          total_l3pdu_tx_cnt += gllc_diag_pdu_stat_rsp->ack_l3pdu_tx_cnt;
          total_l3pdu_tx_cnt += gllc_diag_pdu_stat_rsp->ack_l3pdu_retx_cnt;
          total_l3pdu_tx_cnt += gllc_diag_pdu_stat_rsp->unack_l3pdu_tx_cnt;
        }
        *((uint32 *) param_copy) = total_l3pdu_tx_cnt;
      }

    break;

    case ENG_MODE_TOTAL_DOWNLINK_LLC_PDUS_RECEIVED:
      {
        const gllc_diag_pdu_stat_rsp_t  *gllc_diag_pdu_stat_rsp;
        uint32 total_l3pdu_rx_cnt = 0;
        uint8 sapi;

        for (sapi=3; sapi<=11; sapi+=2)
        {
          gllc_diag_pdu_stat_rsp = gllc_diag_get_pdu_stats(sapi);
          total_l3pdu_rx_cnt += gllc_diag_pdu_stat_rsp->ack_l3pdu_rx_cnt;
          total_l3pdu_rx_cnt += gllc_diag_pdu_stat_rsp->unack_l3pdu_rx_cnt;
        }
        *((uint32 *) param_copy) = total_l3pdu_rx_cnt;
      }

    break;

    case ENG_MODE_TOTAL_UPLINK_LLC_PDUS_RETRANSMITTED:
      {
        const gllc_diag_pdu_stat_rsp_t  *gllc_diag_pdu_stat_rsp;
        uint32 repeated_l3pdu_tx_cnt = 0;
        uint8 sapi;

        for (sapi=3; sapi<=11; sapi+=2)
        {
          gllc_diag_pdu_stat_rsp = gllc_diag_get_pdu_stats(sapi);
          repeated_l3pdu_tx_cnt += gllc_diag_pdu_stat_rsp->ack_l3pdu_retx_cnt;
        }
        *((uint32 *) param_copy) = repeated_l3pdu_tx_cnt;
      }

    break;

    case ENG_MODE_TOTAL_DOWNLINK_LLC_PDUS_RETRANSMITTED:
      {
        const gllc_diag_pdu_stat_rsp_t  *gllc_diag_pdu_stat_rsp;
        uint32 repeated_l3pdu_rx_cnt = 0;
        uint8 sapi;

        for (sapi=3; sapi<=11; sapi+=2)
        {
          gllc_diag_pdu_stat_rsp = gllc_diag_get_pdu_stats(sapi);
          repeated_l3pdu_rx_cnt += gllc_diag_pdu_stat_rsp->ack_l3pdu_rerx_cnt;
        }
        *((uint32 *) param_copy) = repeated_l3pdu_rx_cnt;
      }

    break;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

    case ENG_MODE_LLC_GEA_ALGORITHM:
      {
        *((gmmllc_gea_e_type *) param_copy) = gllc_llme_get_gea_algorithm();
      }

    break;

#ifndef FEATURE_GERAN_REDUCED_DEBUG
    case ENG_MODE_TOTAL_UPLINK_RLC_BLOCKS_TRANSMITTED:
      {
        uint32 total_uplink_rlc_blocks_transmitted;
        rlc_log_rfblks_cnts_t *rlc_log_blks_cnts;

        rlc_log_blks_cnts = rlc_log_rfblks_cnts();

        total_uplink_rlc_blocks_transmitted = rlc_log_blks_cnts->ul_retx + rlc_log_blks_cnts->ul_tx;

        *((uint32 *) param_copy) = total_uplink_rlc_blocks_transmitted;
      }

    break;

    case ENG_MODE_TOTAL_DOWNNLINK_RLC_BLOCKS_RECEIVED:
      {
        uint32 total_downlink_rlc_blocks_received;
        rlc_eng_mode_dl_cnts_t  *rlc_eng_mode_dl_cnts;

        rlc_eng_mode_dl_cnts = rlc_log_dl_blks_cnts();

        total_downlink_rlc_blocks_received = rlc_eng_mode_dl_cnts->dl_rx + rlc_eng_mode_dl_cnts->dl_reprx;

        *((uint32 *) param_copy) = total_downlink_rlc_blocks_received;
      }

    break;

    case ENG_MODE_TOTAL_UPLINK_RLC_BLOCKS_RETRANSMITTED:
      {
        rlc_log_rfblks_cnts_t *rlc_log_blks_cnts;

        rlc_log_blks_cnts = rlc_log_rfblks_cnts();

        *((uint32 *) param_copy) = rlc_log_blks_cnts->ul_retx;
      }

    break;

    case ENG_MODE_TOTAL_DOWNLINK_REPEATED_RLC_BLOCKS_RECEIVED:
      {
        rlc_eng_mode_dl_cnts_t *rlc_eng_mode_dl_cnts;

        rlc_eng_mode_dl_cnts = rlc_log_dl_blks_cnts();

        *((uint32 *) param_copy) = rlc_eng_mode_dl_cnts->dl_reprx;
      }

    break;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

    case ENG_MODE_HOPPING_FREQUENCY_LIST:
      if(eng_mode_data_ptr->geran_eng_mode_info.hopping_freq_list_valid_flag)
      {
        memscpy((eng_mode_frequency_list_T*) param_copy,
                sizeof(eng_mode_frequency_list_T),
                &(eng_mode_data_ptr->geran_eng_mode_info.hopping_freq_list),
                sizeof(eng_mode_frequency_list_T));
      }
      else
      {
        memset((eng_mode_frequency_list_T*) param_copy,0,sizeof(eng_mode_frequency_list_T));
      }

    break;

    case ENG_MODE_RX_QUAL:
      *((uint32 *) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.rx_qual;
    break;

    case ENG_MODE_GMSK_CV_VALUE:
      if(eng_mode_data_ptr->geran_eng_mode_info.gmsk_cv_value_valid_flag)
      {
        *((uint32 *) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.gmsk_cv_value;
      }
      else
      {
        *((uint32 *) param_copy) = GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
      }

    break;

    case ENG_MODE_PSK_CV_VALUE:
      if(eng_mode_data_ptr->geran_eng_mode_info.psk_cv_value_valid_flag)
      {
        *((uint32 *) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.psk_cv_value;
      }
      else
      {
        *((uint32 *) param_copy) = GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
      }

    break;

    case ENG_MODE_SCELL_INFO:
      *((eng_mode_scell_info_t*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.scell_info;
    break;

    case ENG_MODE_SCELL_MEASUREMENTS:
      *((eng_mode_scell_measurements_t*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.scell_measurements;
    break;

    case ENG_MODE_TX_PWR:
      if(eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_in_use_valid_flag)
      {
        memscpy((int16 *) param_copy,
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_in_use),
                eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_in_use,
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_in_use));
      }
      else
      {
        memset((int16 *) param_copy, 0,
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_in_use));
      }

    break;

    case ENG_MODE_NMR_INFO:
      *((eng_mode_nmr_info_t*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.nmr_info;
    break;

    case ENG_MODE_AMR_INFO:
      *((eng_mode_amr_info_t*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.amr_info;
    break;

    case ENG_MODE_DTX_UL_STATUS:
      *((uint8*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.dtx_ul_status;
    break;

    case ENG_MODE_TBF_INFO:
      *((eng_mode_tbf_info_t*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.tbf_info;
    break;

    case ENG_MODE_NW_PARAMS:
      *((eng_mode_nw_params_t*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.nw_params;
    break;

    case ENG_MODE_GPRS_UL_CS:
      *((uint8*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.gprs_ul_cs;
    break;

    case ENG_MODE_GPRS_DL_CS:
      *((uint8*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.gprs_dl_cs;
    break;

    case ENG_MODE_EGPRS_UL_MCS:
      *((uint8*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.egprs_ul_mcs;
    break;

    case ENG_MODE_EGPRS_DL_MCS:
      *((uint8*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.egprs_dl_mcs;
    break;

    case ENG_MODE_EDGE_MEAS:
      *((eng_mode_edge_meas_t*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.edge_meas;
    break;

    case ENG_MODE_SCELL_CONFIG:
      *((eng_mode_scell_config_t*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.scell_config;
    break;

    case ENG_MODE_HOPPING_STATUS:
      *((eng_mode_hopping_status_t*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.hopping_status;
    break;

    case ENG_MODE_CODEC_HO_INFO:
      *((sys_codec_ho_info*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.ho_codec;
    break;

#ifdef FEATURE_ENHANCED_AMR
    case ENG_MODE_CODEC_HO_SAMPLE_RATE:
      ( (geran_eng_mode_codec_ho_sample_rate_t*) param_copy)->speech_enc_samp_freq =
              eng_mode_data_ptr->geran_eng_mode_info.ho_codec.speech_enc_samp_freq;
    break;
#endif /*FEATURE_ENHANCED_AMR*/

    case ENG_MODE_SIGNAL_INFO:
      {
        sys_modem_as_id_e_type asubs_id = geran_map_gas_id_to_nas_id();

        l1_log_signal_info((geran_eng_mode_signal_info_t*)(&eng_mode_data_ptr->geran_eng_mode_info.signal_info.band));
      /*
      ** add the Tx info, either i/f may be used.
      */
        geran_multisim_eng_mode_data_read(ENG_MODE_TX_PWR_PA_INFO,
                                          (void*)&eng_mode_data_ptr->geran_eng_mode_info.signal_info.tx_power, asubs_id);

        *((geran_eng_mode_signal_info_t*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.signal_info;

      }
    break;
    /*
    ** CgC new api feature to read Tx power ( dBm) and PA gain state. called from cmapi.c
    */
    case ENG_MODE_TX_PWR_PA_INFO:
        {
          eng_mode_tx_pwr_pa_info_t *ptr=(eng_mode_tx_pwr_pa_info_t*)param_copy;
          uint8 pwr_dBm;
          ptr->is_valid = eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.is_valid;

          if(ptr->is_valid == TRUE)
          {
            l1_log_convert_txlev_to_pwr_dBm(eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.band,
                                            eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.tx_lev,
                                            &pwr_dBm );

            eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.tx_pwr = pwr_dBm;
            ptr->tx_pwr   = pwr_dBm;
            ptr->tx_pa    = eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.tx_pa;
          }
          eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.is_valid = FALSE;   /* clear after its been read */
        }
    break;

    case ENG_MODE_CURRENT_L1_TIMESLOT:
      *((eng_mode_curr_l1_ts_t*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.current_timeslot;
    break;

    case ENG_MODE_GSM_CIPHER_INFO:
      *((geran_eng_mode_gsm_cipher_info_t*) param_copy) = eng_mode_data_ptr->geran_eng_mode_info.gsm_cipher;
    break;

#ifdef FEATURE_MODEM_STATISTICS
    case ENG_MODE_GERAN_MODEM_STATS_INFO:
    {
      /*Updated Bitmap*/
      ((geran_eng_mode_modem_stats_t*)param_copy)->updated_info = eng_mode_data_ptr->geran_eng_mode_info.updated_info;

      /*ENG_MODE_TIMING_ADVANCE*/
      ((geran_eng_mode_modem_stats_t*)param_copy)->timing_advance_value = eng_mode_data_ptr->geran_eng_mode_info.timing_advance_value;

      /*ENG_MODE_NMR_INFO*/
      memscpy (&(((geran_eng_mode_modem_stats_t*)param_copy)->nmr_info),
               sizeof(eng_mode_data_ptr->geran_eng_mode_info.nmr_info),
               &eng_mode_data_ptr->geran_eng_mode_info.nmr_info,
               sizeof(eng_mode_data_ptr->geran_eng_mode_info.nmr_info));

      /*ENG_MODE_RX_QUAL*/
      ((geran_eng_mode_modem_stats_t*)param_copy)->rx_qual = eng_mode_data_ptr->geran_eng_mode_info.rx_qual;

      /*ENG_MODE_TX_PWR*/
      memscpy (((geran_eng_mode_modem_stats_t*)param_copy)->tx_pwr_in_use,
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_in_use),
                &eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_in_use,
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_in_use));

      /*ENG_MODE_SCELL_INFO*/
      memscpy (&(((geran_eng_mode_modem_stats_t*)param_copy)->scell_info),
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.scell_info),
                &eng_mode_data_ptr->geran_eng_mode_info.scell_info,
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.scell_info));

      /*ENG_MODE_SCELL_MEASUREMENTS*/
      memscpy (&(((geran_eng_mode_modem_stats_t*)param_copy)->scell_measurements),
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.scell_measurements),
                &eng_mode_data_ptr->geran_eng_mode_info.scell_measurements,
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.scell_measurements));

      /*ENG_MODE_AMR_INFO*/
      memscpy (&(((geran_eng_mode_modem_stats_t*)param_copy)->amr_info),
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.amr_info),
                &eng_mode_data_ptr->geran_eng_mode_info.amr_info,
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.amr_info));

      /*ENG_MODE_DTX_UL_STATUS*/
      ((geran_eng_mode_modem_stats_t*)param_copy)->dtx_ul_status = eng_mode_data_ptr->geran_eng_mode_info.dtx_ul_status;

      /*ENG_MODE_TBF_INFO*/
      memscpy (&(((geran_eng_mode_modem_stats_t*)param_copy)->tbf_info),
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.tbf_info),
                &eng_mode_data_ptr->geran_eng_mode_info.tbf_info,
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.tbf_info));

      /*ENG_MODE_NW_PARAMS*/
      memscpy (&(((geran_eng_mode_modem_stats_t*)param_copy)->nw_params),
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.nw_params),
                &eng_mode_data_ptr->geran_eng_mode_info.nw_params,
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.nw_params));

      /*ENG_MODE_GPRS_UL_CS*/
      ((geran_eng_mode_modem_stats_t*)param_copy)->gprs_ul_cs = eng_mode_data_ptr->geran_eng_mode_info.gprs_ul_cs;

      /*ENG_MODE_GPRS_DL_CS*/
      ((geran_eng_mode_modem_stats_t*)param_copy)->gprs_dl_cs = eng_mode_data_ptr->geran_eng_mode_info.gprs_dl_cs;

      /*ENG_MODE_EGPRS_UL_MCS*/
      ((geran_eng_mode_modem_stats_t*)param_copy)->egprs_ul_mcs = eng_mode_data_ptr->geran_eng_mode_info.egprs_ul_mcs;

      /*ENG_MODE_EGPRS_DL_MCS*/
      ((geran_eng_mode_modem_stats_t*)param_copy)->egprs_dl_mcs = eng_mode_data_ptr->geran_eng_mode_info.egprs_dl_mcs;

      /*ENG_MODE_EDGE_MEAS*/
      memscpy (&(((geran_eng_mode_modem_stats_t*)param_copy)->edge_meas),
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.edge_meas),
                &eng_mode_data_ptr->geran_eng_mode_info.edge_meas,
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.edge_meas));

      /*ENG_MODE_SCELL_CONFIG*/
      memscpy (&(((geran_eng_mode_modem_stats_t*)param_copy)->scell_config),
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.scell_config),
                &eng_mode_data_ptr->geran_eng_mode_info.scell_config,
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.scell_config));

      /*ENG_MODE_HOPPING_STATUS*/
      memscpy (&(((geran_eng_mode_modem_stats_t*)param_copy)->hopping_status),
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.hopping_status),
                &eng_mode_data_ptr->geran_eng_mode_info.hopping_status,
                sizeof(eng_mode_data_ptr->geran_eng_mode_info.hopping_status));

      /*ENG_MODE_CURRENT_L1_TIMESLOT*/
      ((geran_eng_mode_modem_stats_t*)param_copy)->current_timeslot = eng_mode_data_ptr->geran_eng_mode_info.current_timeslot;
    }
    break;
#endif /* FEATURE_MODEM_STATISTICS */

    case ENG_MODE_ALL_INFORMATION:
      MSG_GERAN_ERROR_0_G("ENG_MODE_ALL_INFORMATION obsolete");
      *((uint32 *) param_copy) = GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
    break;

    default:
      MSG_GERAN_HIGH_1_G("geran_eng_mode_data_read function is called with wrong data to read %d",
                          geran_eng_mode_info_type);
      *((uint32 *) param_copy) = GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;

    break;
  }

  /* Clear the bit in 'updated_info' to show this info has been read
     If all info was read, clear all the bits */
  if (geran_eng_mode_info_type == ENG_MODE_GERAN_MODEM_STATS_INFO)
  {
    eng_mode_data_ptr->geran_eng_mode_info.updated_info.geran_modem_stats_mask = 0;
  }
  else
  {
    if (geran_eng_mode_info_type < ENG_MODE_MAX_BITMAPPED_INFO_TYPE_ELEMENTS)
    {
      eng_mode_data_ptr->geran_eng_mode_info.updated_info.geran_modem_stats_mask &= ~(GERAN_ENG_MODE_INFO_TYPE_TO_BITMASK(geran_eng_mode_info_type));
    }
  }

}/*geran_eng_mode_data_read*/

/*===========================================================================

FUNCTION geran_eng_mode_register_callback

DESCRIPTION

  This function enables upper layers to register a callback function.
  Once the function is registered, it will be called whenever there is a change in value.
  MAX_NUM_CALLBACK_REGISTRATIONS indicates the maximum number of callbacks per info type.


PARAMETERS

  geran_eng_mode_info_type - a callback is registered per info type.
  callback_func_ptr - pointer to function that requires registration, see Call back Function prototypes.
  asubs_id - Access stratum subs ID


RETURN VALUE
  Boolean - TRUE - registration worked
            FALSE - registration failed.

SIDE EFFECTS
  reads callback_supported
  write to eng_mode_cb_funcs

===========================================================================*/

boolean geran_eng_mode_register_callback(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                                         void *callback_func_ptr)
{
  return geran_multisim_eng_mode_register_callback(geran_eng_mode_info_type,
                                                   callback_func_ptr,
                                                   SYS_MODEM_AS_ID_1);
}

/*===========================================================================

FUNCTION geran_multisim_eng_mode_register_callback

DESCRIPTION

  This function enables upper layers to register a callback function.
  Once the function is registered, it will be called whenever there is a change in value.
  MAX_NUM_CALLBACK_REGISTRATIONS indicates the maximum number of callbacks per info type.


PARAMETERS

  geran_eng_mode_info_type - a callback is registered per info type.
  callback_func_ptr - pointer to function that requires registration, see Call back Function prototypes.
  asubs_id - Access stratum subs ID


RETURN VALUE
  Boolean - TRUE - registration worked
            FALSE - registration failed.

SIDE EFFECTS
  reads callback_supported
  write to eng_mode_cb_funcs

===========================================================================*/

boolean geran_multisim_eng_mode_register_callback(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                                                  void *callback_func_ptr,
                                                  sys_modem_as_id_e_type asubs_id)
{
  uint16 i = 0;
  boolean callback_supported = FALSE;
  geran_eng_mode_data_t *eng_mode_data_ptr = geran_get_eng_mode_data_ptr(asubs_id);

  if (eng_mode_data_ptr == NULL)
  {
    return FALSE;
  }

  /* loop around number of elements in supported_call_backs to check requested info
     type is supported as a callback */
  for (i=0; i< (sizeof(supported_call_backs) / sizeof(supported_call_backs[0])); i++)
  {
    if (geran_eng_mode_info_type == supported_call_backs[i])
    {
      callback_supported = TRUE;
    }
  }

  if (callback_supported)
  {
    if (callback_func_ptr)
    {
      /* First go through the list to make sure func does
       * not get registered twice.
       */
      for (i = 0; i < MAX_NUM_CALLBACK_REGISTRATIONS; i++)
      {
        if (eng_mode_data_ptr->eng_mode_cb_funcs[geran_eng_mode_info_type][i].func_ptr == callback_func_ptr)
        {
          MSG_GERAN_HIGH_2("GERAN Eng Mode Function already registered ind: %d cb: 0x%x",i,callback_func_ptr);
          return TRUE;
        }
      }

      /* Insert callback function in next available space */
      for (i = 0; i < MAX_NUM_CALLBACK_REGISTRATIONS; i++)
      {
        if (eng_mode_data_ptr->eng_mode_cb_funcs[geran_eng_mode_info_type][i].func_ptr == NULL)
        {
          MSG_GERAN_HIGH_3("GERAN Eng Mode registering eng_mode_type: 0x%x, callback index: %d callback_func_ptr: 0x%x",
                           geran_eng_mode_info_type,i,callback_func_ptr);
          eng_mode_data_ptr->eng_mode_cb_funcs[geran_eng_mode_info_type][i].func_ptr = callback_func_ptr;
          memset(&eng_mode_data_ptr->eng_mode_cb_funcs[geran_eng_mode_info_type][i].behaviour,0,sizeof(geran_eng_mode_cb_behaviour_t));
          return TRUE;
        }
      }

      if (i == MAX_NUM_CALLBACK_REGISTRATIONS)
      {
        MSG_GERAN_ERROR_1("GERAN Eng Mode callback full cannot register cb: 0x%x",callback_func_ptr);
      }
    }
    else
    {
      MSG_GERAN_ERROR_0("GERAN Eng Mode cannot register NULL callback");
    }
  }
  else
  {
    MSG_GERAN_ERROR_1("GERAN Eng Mode info_type:0x%x is not supported as a callback", geran_eng_mode_info_type);
  }

  return FALSE;
}

/*===========================================================================

FUNCTION geran_eng_mode_deregister_callback

DESCRIPTION

  This function enables upper layers to deregister a callback function.


PARAMETERS

  geran_eng_mode_info_type - a callback is registered per info type.
  callback_func_ptr - pointer to function that requires deregistration, see Call back Function prototypes.
  asubs_id - Access stratum subs ID


RETURN VALUE
  Boolean - TRUE - deregistration worked
            FALSE - deregistration failed.

SIDE EFFECTS
  write to eng_mode_cb_funcs

===========================================================================*/

boolean geran_eng_mode_deregister_callback(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                                           void *callback_func_ptr)
{
  return geran_multisim_eng_mode_deregister_callback(geran_eng_mode_info_type,
                                                     callback_func_ptr,
                                                     SYS_MODEM_AS_ID_1);
};

/*===========================================================================

FUNCTION geran_multisim_eng_mode_deregister_callback

DESCRIPTION

  This function enables upper layers to deregister a callback function.


PARAMETERS

  geran_eng_mode_info_type - a callback is registered per info type.
  callback_func_ptr - pointer to function that requires deregistration, see Call back Function prototypes.
  asubs_id - Access stratum subs ID


RETURN VALUE
  Boolean - TRUE - deregistration worked
            FALSE - deregistration failed.

SIDE EFFECTS
  write to eng_mode_cb_funcs

===========================================================================*/

boolean geran_multisim_eng_mode_deregister_callback(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                                                    void *callback_func_ptr,
                                                    sys_modem_as_id_e_type asubs_id)
{
  uint16 i = 0;
  geran_eng_mode_data_t *eng_mode_data_ptr = geran_get_eng_mode_data_ptr(asubs_id);

  if (eng_mode_data_ptr == NULL)
  {
    return FALSE;
  }

  for (i = 0; i < MAX_NUM_CALLBACK_REGISTRATIONS; i++)
  {
    if (eng_mode_data_ptr->eng_mode_cb_funcs[geran_eng_mode_info_type][i].func_ptr == callback_func_ptr)
    {
      MSG_GERAN_HIGH_3("GERAN Eng Mode Deregistering CB info_type:0x%x, index: %d, cb_ptr: 0x%x",
                       geran_eng_mode_info_type,i,callback_func_ptr);
      eng_mode_data_ptr->eng_mode_cb_funcs[geran_eng_mode_info_type][i].func_ptr = NULL;
      memset(&eng_mode_data_ptr->eng_mode_cb_funcs[geran_eng_mode_info_type][i].behaviour,0,sizeof(geran_eng_mode_cb_behaviour_t));
      return TRUE;
    }
  }

  if (i == MAX_NUM_CALLBACK_REGISTRATIONS)
  {
    MSG_GERAN_ERROR_2("GERAN Eng Mode Cannot find cb: 0x%x in info_type: 0x%x list",callback_func_ptr,geran_eng_mode_info_type);
  }

  return FALSE;
}

/*===========================================================================

FUNCTION geran_eng_mode_modify_callback_behaviour

DESCRIPTION

  This function enables upper layers to alter the behaviour of a registered callback.


PARAMETERS

  geran_eng_mode_info_type - info type used for registration
  callback_func_ptr - pointer to registered function
  cb_behaviour - updated cb behaviour

RETURN VALUE
  Boolean - TRUE - cb behaviour update worked
            FALSE - cb behaviour update failed.

SIDE EFFECTS

===========================================================================*/
boolean geran_eng_mode_modify_callback_behaviour(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                                                 void *callback_func_ptr,
                                                 geran_eng_mode_cb_behaviour_t cb_behaviour)
{
  return geran_multisim_eng_mode_modify_callback_behaviour(geran_eng_mode_info_type,
                                                           callback_func_ptr,
                                                           cb_behaviour,
                                                           SYS_MODEM_AS_ID_1);
}

/*===========================================================================

FUNCTION geran_multisim_eng_mode_modify_callback_behaviour

DESCRIPTION

  This function enables upper layers to alter the behaviour of a registered callback.


PARAMETERS

  geran_eng_mode_info_type - info type used for registration
  callback_func_ptr - pointer to registered function
  cb_behaviour - updated cb behaviour
  asubs_id - Access stratum subs ID

RETURN VALUE
  Boolean - TRUE - cb behaviour update worked
            FALSE - cb behaviour update failed.

SIDE EFFECTS

===========================================================================*/
boolean geran_multisim_eng_mode_modify_callback_behaviour(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                                                          void *callback_func_ptr,
                                                          geran_eng_mode_cb_behaviour_t cb_behaviour,
                                                          sys_modem_as_id_e_type asubs_id)
{
  uint16 i = 0;
  boolean modifier_supported = FALSE;
  geran_eng_mode_data_t *eng_mode_data_ptr = geran_get_eng_mode_data_ptr(asubs_id);

  if (eng_mode_data_ptr == NULL)
  {
    return FALSE;
  }

  /* loop around number of elements in supported_call_backs to check requested info
     type is supported as a callback */
  for (i=0; i< (sizeof(supported_cb_behaviour_modifier) / sizeof(supported_cb_behaviour_modifier[0])); i++)
  {
    if (geran_eng_mode_info_type == supported_cb_behaviour_modifier[i])
    {
      modifier_supported = TRUE;
      break;
    }
  }

  if (modifier_supported)
  {
    for (i = 0; i < MAX_NUM_CALLBACK_REGISTRATIONS; i++)
    {
      if (eng_mode_data_ptr->eng_mode_cb_funcs[geran_eng_mode_info_type][i].func_ptr == callback_func_ptr)
      {
        MSG_GERAN_HIGH_3("GERAN Eng Mode Modifying CB info_type:0x%x, cb_ptr: 0x%x, always_report:%d",
                         geran_eng_mode_info_type,
                         callback_func_ptr,
                         cb_behaviour.always_report);
        eng_mode_data_ptr->eng_mode_cb_funcs[geran_eng_mode_info_type][i].behaviour = cb_behaviour;
        return TRUE;
      }
    }

    if (i == MAX_NUM_CALLBACK_REGISTRATIONS)
    {
      MSG_GERAN_ERROR_2("GERAN Eng Mode Cannot find cb: 0x%x in info_type: 0x%x list",
                        callback_func_ptr,geran_eng_mode_info_type);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1("GERAN Eng Mode modifier for info_type: 0x%x not supported",
                      geran_eng_mode_info_type);
  }

  return FALSE;

}

/*===========================================================================

FUNCTION geran_eng_initialise_store

DESCRIPTION
  Initialises geran_eng_mode_info to invalid values.


PARAMETERS
  sys_modem_as_id_e_type as_id

RETURN VALUE
  None

SIDE EFFECTS
  Write to geran_eng_mode_info

===========================================================================*/
 void geran_eng_initialise_store(sys_modem_as_id_e_type as_id)
{
  geran_eng_mode_data_t *eng_mode_data_ptr = geran_get_eng_mode_data_ptr(as_id);

  MSG_GERAN_HIGH_0("Eng Mode Initialise store");

  if (eng_mode_data_ptr == NULL)
  {
    return;
  }

  eng_mode_data_ptr->geran_eng_mode_info.updated_info.geran_modem_stats_mask = 0;

  /*ENG_MODE_TIMING_ADVANCE*/
  eng_mode_data_ptr->geran_eng_mode_info.timing_advance_value = (uint32)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  eng_mode_data_ptr->geran_eng_mode_info.timing_advance_valid_flag = FALSE;

  /*ENG_MODE_RADIO_LINK_TIMEOUT_VALUE*/
  eng_mode_data_ptr->geran_eng_mode_info.radio_link_timeout_value = (uint32)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  eng_mode_data_ptr->geran_eng_mode_info.radio_link_timeout_valid_flag = FALSE;

  /*ENG_MODE_TOTAL_UPLINK_LLC_PDUS_TRANSMITTED*/
  eng_mode_data_ptr->geran_eng_mode_info.total_uplink_llc_pdus_transmitted = 0;
  eng_mode_data_ptr->geran_eng_mode_info.total_uplink_llc_pdus_transmitted_valid_flag = FALSE;

  /*ENG_MODE_TOTAL_DOWNLINK_LLC_PDUS_RECEIVED*/
  eng_mode_data_ptr->geran_eng_mode_info.total_downlink_llc_pdus_received = 0;
  eng_mode_data_ptr->geran_eng_mode_info.total_downlink_llc_pdus_received_valid_flag = FALSE;

  /*ENG_MODE_TOTAL_UPLINK_LLC_PDUS_RETRANSMITTED*/
  eng_mode_data_ptr->geran_eng_mode_info.total_uplink_llc_pdus_retransmitted = 0;
  eng_mode_data_ptr->geran_eng_mode_info.total_uplink_llc_pdus_retransmitted_valid_flag = FALSE;

  /*ENG_MODE_TOTAL_DOWNLINK_LLC_PDUS_RETRANSMITTED*/
  eng_mode_data_ptr->geran_eng_mode_info.total_downlink_llc_pdus_retransmitted = 0;
  eng_mode_data_ptr->geran_eng_mode_info.total_downlink_llc_pdus_retransmitted_valid_flag = FALSE;

  /*ENG_MODE_TOTAL_UPLINK_RLC_BLOCKS_TRANSMITTED*/
  eng_mode_data_ptr->geran_eng_mode_info.total_uplink_rlc_blocks_transmitted = 0;
  eng_mode_data_ptr->geran_eng_mode_info.total_uplink_rlc_blocks_transmitted_valid_flag = FALSE;

  /*ENG_MODE_TOTAL_DOWNNLINK_RLC_BLOCKS_RECEIVED*/
  eng_mode_data_ptr->geran_eng_mode_info.total_downlink_rlc_blocks_received = 0;
  eng_mode_data_ptr->geran_eng_mode_info. total_downlink_rlc_blocks_received_valid_flag = FALSE;

  /*ENG_MODE_TOTAL_UPLINK_RLC_BLOCKS_RETRANSMITTED*/
  eng_mode_data_ptr->geran_eng_mode_info.total_uplink_rlc_blocks_retransmitted = 0;
  eng_mode_data_ptr->geran_eng_mode_info.total_uplink_rlc_blocks_retransmitted_valid_flag = FALSE;

  /*ENG_MODE_TOTAL_DOWNLINK_REPEATED_RLC_BLOCKS_RECEIVED*/
  eng_mode_data_ptr->geran_eng_mode_info.total_downlink_repeated_rlc_blocks_received = 0;
  eng_mode_data_ptr->geran_eng_mode_info.total_downlink_repeated_rlc_blocks_received_valid_flag = FALSE;

  /*ENG_MODE_HOPPING_FREQUENCY_LIST*/
  memset(&eng_mode_data_ptr->geran_eng_mode_info.hopping_freq_list,
         0,
         sizeof(eng_mode_data_ptr->geran_eng_mode_info.hopping_freq_list));
  eng_mode_data_ptr->geran_eng_mode_info.hopping_freq_list_valid_flag = FALSE;

  /*ENG_MODE_RX_QUAL*/
  eng_mode_data_ptr->geran_eng_mode_info.rx_qual = (uint32)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  eng_mode_data_ptr->geran_eng_mode_info.rx_qual_valid_flag = FALSE;

  /*ENG_MODE_GMSK_CV_VALUE*/
  eng_mode_data_ptr->geran_eng_mode_info.gmsk_cv_value = (uint32)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  eng_mode_data_ptr->geran_eng_mode_info.gmsk_cv_value_valid_flag = FALSE;

  /*ENG_MODE_PSK_CV_VALUE*/
  eng_mode_data_ptr->geran_eng_mode_info.psk_cv_value = (uint32)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  eng_mode_data_ptr->geran_eng_mode_info.psk_cv_value_valid_flag = FALSE;

  /*ENG_MODE_SCELL_INFO*/
  memset(&eng_mode_data_ptr->geran_eng_mode_info.scell_info,
         (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
         sizeof(eng_mode_data_ptr->geran_eng_mode_info.scell_info));

  eng_mode_data_ptr->geran_eng_mode_info.scell_info_valid_flag = FALSE;

  /*ENG_MODE_SCELL_MEASUREMENTS*/
  memset(&eng_mode_data_ptr->geran_eng_mode_info.scell_measurements,
         0,
         sizeof(eng_mode_data_ptr->geran_eng_mode_info.scell_measurements));
  eng_mode_data_ptr->geran_eng_mode_info.scell_measurements.rx_qual_sub = (uint16)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  eng_mode_data_ptr->geran_eng_mode_info.scell_measurements.rx_qual_full = (uint16)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  eng_mode_data_ptr->geran_eng_mode_info.scell_measurements.rx_lev_sub = (uint16)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  eng_mode_data_ptr->geran_eng_mode_info.scell_measurements.rx_lev_full = (uint16)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  eng_mode_data_ptr->geran_eng_mode_info.scell_measurements_valid_flag = FALSE;

  /*ENG_MODE_TX_PWR*/
  memset(eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_in_use,
         0,
         sizeof(eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_in_use));
  eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_in_use_valid_flag = FALSE;

  /*ENG_MODE_NMR_INFO*/
  memset(&eng_mode_data_ptr->geran_eng_mode_info.nmr_info,
         0,
         sizeof(eng_mode_data_ptr->geran_eng_mode_info.nmr_info));
  eng_mode_data_ptr->geran_eng_mode_info.nmr_info_valid_flag = FALSE;

  /*ENG_MODE_AMR_INFO*/
  memset(&eng_mode_data_ptr->geran_eng_mode_info.amr_info,
         (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
         sizeof(eng_mode_data_ptr->geran_eng_mode_info.amr_info));
  eng_mode_data_ptr->geran_eng_mode_info.amr_info.dl_dtx = 0;
  eng_mode_data_ptr->geran_eng_mode_info.amr_info_valid_flag = FALSE;

  /*ENG_MODE_DTX_UL_STATUS*/
  eng_mode_data_ptr->geran_eng_mode_info.dtx_ul_status = (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  eng_mode_data_ptr->geran_eng_mode_info.dtx_ul_status_valid_flag = FALSE;

  /*ENG_MODE_TBF_INFO*/
  memset(&eng_mode_data_ptr->geran_eng_mode_info.tbf_info,
         (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
         sizeof(eng_mode_data_ptr->geran_eng_mode_info.tbf_info));
  eng_mode_data_ptr->geran_eng_mode_info.tbf_info_valid_flag = FALSE;

  /*ENG_MODE_NW_PARAMS*/
  memset(&eng_mode_data_ptr->geran_eng_mode_info.nw_params,
         (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
         sizeof(eng_mode_data_ptr->geran_eng_mode_info.nw_params));
  eng_mode_data_ptr->geran_eng_mode_info.nw_params_valid_flag = FALSE;

  /*ENG_MODE_GPRS_UL_CS*/
  eng_mode_data_ptr->geran_eng_mode_info.gprs_ul_cs = (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  eng_mode_data_ptr->geran_eng_mode_info.gprs_ul_cs_valid_flag = FALSE;

  /*ENG_MODE_GPRS_DL_CS*/
  eng_mode_data_ptr->geran_eng_mode_info.gprs_dl_cs = (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  eng_mode_data_ptr->geran_eng_mode_info.gprs_dl_cs_valid_flag = FALSE;

  /*ENG_MODE_EGPRS_UL_MCS*/
  eng_mode_data_ptr->geran_eng_mode_info.egprs_ul_mcs = (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  eng_mode_data_ptr->geran_eng_mode_info.egprs_ul_mcs_valid_flag = FALSE;

  /*ENG_MODE_EGPRS_DL_MCS*/
  eng_mode_data_ptr->geran_eng_mode_info.egprs_dl_mcs = (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  eng_mode_data_ptr->geran_eng_mode_info.egprs_dl_mcs_valid_flag = FALSE;

  /*ENG_MODE_EDGE_MEAS*/
  memset(&eng_mode_data_ptr->geran_eng_mode_info.edge_meas,
         (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
         sizeof(eng_mode_data_ptr->geran_eng_mode_info.edge_meas));
  eng_mode_data_ptr->geran_eng_mode_info.edge_meas_valid_flag = FALSE;

  /*ENG_MODE_SCELL_CONFIG*/
  memset(&eng_mode_data_ptr->geran_eng_mode_info.scell_config,
         (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
         sizeof(eng_mode_data_ptr->geran_eng_mode_info.scell_config));
  eng_mode_data_ptr->geran_eng_mode_info.scell_config_valid_flag = FALSE;

  /*ENG_MODE_HOPPING_STATUS*/
  memset(&eng_mode_data_ptr->geran_eng_mode_info.hopping_status,
         (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
         sizeof(eng_mode_data_ptr->geran_eng_mode_info.hopping_status));
  eng_mode_data_ptr->geran_eng_mode_info.hopping_status_valid_flag = FALSE;

  /*ENG_MODE_CODEC_HO_INFO*/
  memset(&eng_mode_data_ptr->geran_eng_mode_info.ho_codec,
         0,
         sizeof(eng_mode_data_ptr->geran_eng_mode_info.ho_codec));
  eng_mode_data_ptr->geran_eng_mode_info.ho_codec_valid_flag = FALSE;

  /*ENG_MODE_SIGNAL_INFO */
  memset(&eng_mode_data_ptr->geran_eng_mode_info.signal_info,
         (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
         sizeof(eng_mode_data_ptr->geran_eng_mode_info.signal_info));
  eng_mode_data_ptr->geran_eng_mode_info.signal_info_valid_flag = FALSE;

  /*ENG_MODE_TX_PWR_PA_INFO */
  eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.tx_pwr     = 0;
  eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.tx_pa      = 0;
  eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.band       = 0;
  eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.is_valid   = FALSE;
  eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info_valid_flag = FALSE;

  /*ENG_MODE_CURRENT_L1_TIMESLOT */
  memset(&eng_mode_data_ptr->geran_eng_mode_info.current_timeslot,
          (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
          sizeof(eng_mode_data_ptr->geran_eng_mode_info.current_timeslot));
  eng_mode_data_ptr->geran_eng_mode_info.current_timeslot_valid_flag = FALSE;

  /*ENG_MODE_CANDIDATE_PLMN_ID */
  memset(&eng_mode_data_ptr->geran_eng_mode_info.candidate_plmn_id,
          0xFF,
          sizeof(eng_mode_data_ptr->geran_eng_mode_info.candidate_plmn_id));
  eng_mode_data_ptr->geran_eng_mode_info.candidate_plmn_valid_flag = FALSE;

  /*ENG_MODE_GSM_CIPHER_INFO*/
  eng_mode_data_ptr->geran_eng_mode_info.gsm_cipher.cipher_alg = ENG_MODE_GSM_CIPHER_NONE;
  eng_mode_data_ptr->geran_eng_mode_info.gsm_cipher_valid_flag = FALSE;
  return;
}

/*===========================================================================

FUNCTION geran_eng_mode_set_modem_stats_mask

DESCRIPTION

  Sets the current bitmask for reporting data changes (bitmask of zero means that no changes
  are reported) for ENG_MODE_GERAN_MODEM_STATS_INFO.  The bitmask positions are defined in
  geran_eng_mode_info_type_t. Update CM with the latest information when the mask
  has been changed to ensure that the client doesn't have to wait for information
  to change before it gets an update.


PARAMETERS

  bitmask


RETURN VALUE
  None

SIDE EFFECTS
  writes to internal bitmask stored variable.

===========================================================================*/
void geran_eng_mode_set_modem_stats_mask(geran_eng_mode_info_bitmask_t bitmask)
{
  geran_multisim_eng_mode_set_modem_stats_mask(bitmask,SYS_MODEM_AS_ID_1);
}

/*===========================================================================

FUNCTION geran_multisim_eng_mode_set_modem_stats_mask

DESCRIPTION

  Sets the current bitmask for reporting data changes (bitmask of zero means that no changes
  are reported) for ENG_MODE_GERAN_MODEM_STATS_INFO.  The bitmask positions are defined in
  geran_eng_mode_info_type_t. Update CM with the latest information when the mask
  has been changed to ensure that the client doesn't have to wait for information
  to change before it gets an update.


PARAMETERS

  bitmask


RETURN VALUE
  None

SIDE EFFECTS
  writes to internal bitmask stored variable.

===========================================================================*/
void geran_multisim_eng_mode_set_modem_stats_mask(geran_eng_mode_info_bitmask_t bitmask, sys_modem_as_id_e_type asubs_id)
{
#ifdef FEATURE_MODEM_STATISTICS
  geran_eng_mode_data_t *eng_mode_data_ptr = geran_get_eng_mode_data_ptr(asubs_id);

  if (eng_mode_data_ptr == NULL)
  {
    return;
  }

  /* Update CM with the latest information whenever the mask has been changed */
  (void)cm_per_subs_stats_set_modem_info_signal(SYS_MODEM_MODULE_GSM, asubs_id);

  MSG_GERAN_HIGH_1("Eng Mode Stats Mask set to:0x%x",bitmask.geran_modem_stats_mask);
  eng_mode_data_ptr->geran_stats_info.requested_bitmask = bitmask;
#else /* FEATURE_MODEM_STATISTICS */
  MSG_GERAN_ERROR_0("FEATURE_MODEM_STATISTICS is disabled");
  NOTUSED(asubs_id);
#endif /* !FEATURE_MODEM_STATISTICS */
}

/* EOF */


