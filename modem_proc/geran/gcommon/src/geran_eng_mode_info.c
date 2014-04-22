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

$Header: //components/rel/geran.mpss/5.2.0/gcommon/src/geran_eng_mode_info.c#4 $

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
extern uint8 l1_log_get_TxPa_state( void  );   /* CMAPI feature for Tx pa gain state */

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/



/*==============================================================================

                    FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/
static void geran_eng_mode_notify_callbacks(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                                            boolean always_update_cb
                                             );
static boolean geran_eng_nmr_info_diff(eng_mode_nmr_info_t* a_ptr,
                                       eng_mode_nmr_info_t* b_ptr);
static boolean geran_eng_scell_meas_diff(eng_mode_scell_measurements_t* a_ptr,
                                         eng_mode_scell_measurements_t* b_ptr);
static boolean geran_eng_amr_info_diff(eng_mode_amr_info_t* a_ptr,
                                       eng_mode_amr_info_t* b_ptr);
static boolean geran_eng_tbf_info_diff(eng_mode_tbf_info_t* a_ptr,
                                       eng_mode_tbf_info_t* b_ptr);
static boolean geran_eng_edge_meas_diff(eng_mode_edge_meas_t* a_ptr,
                                        eng_mode_edge_meas_t* b_ptr);
static boolean geran_eng_scell_config_diff(eng_mode_scell_config_t* a_ptr,
                                           eng_mode_scell_config_t* b_ptr);
static boolean geran_eng_nw_params_diff(eng_mode_nw_params_t* a_ptr,
                                        eng_mode_nw_params_t* b_ptr);

static boolean geran_eng_tx_pwr_diff(int16* a_ptr, int16* b_ptr , uint8* highPwr);

static boolean geran_eng_hopping_status_diff(eng_mode_hopping_status_t* a_ptr,
                                             eng_mode_hopping_status_t* b_ptr);

static boolean geran_eng_mode_always_update_cb(geran_eng_mode_info_type_t geran_eng_mode_info_type
                                                );

static boolean geran_eng_curr_l1_timeslot_diff(eng_mode_curr_l1_ts_t* a_ptr,
                                               eng_mode_curr_l1_ts_t* b_ptr);

static boolean geran_eng_gsm_cipher_diff(geran_eng_mode_gsm_cipher_info_t* a_ptr,
                                         geran_eng_mode_gsm_cipher_info_t* b_ptr);

static boolean geran_eng_mode_enhanced_logging_enabled(void);

/*==========================================================================

  FUNCTION    geran_eng_mode_data_write

  DESCRIPTION

    This function is intended to write the eng mode public store


  PARAMETERS

    geran_eng_mode_info_type  :  geran_eng_mode_info_type_t type

    param_copy                :   void

    valid_flag:                   The value of the parameter is valid or not

    gas_id                    : GERAN access stratum id

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void geran_eng_mode_data_write(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                               void *param_copy,
                               boolean valid_flag
                                )
{
  boolean data_changed = FALSE;
  boolean always_update_cb = FALSE;
  boolean geran_eng_mode_logging_enabled;
  sys_modem_as_id_e_type as_id = geran_map_gas_id_to_nas_id();
  geran_eng_mode_data_t *eng_mode_data_ptr = geran_get_eng_mode_data_ptr(as_id);

  if (eng_mode_data_ptr == NULL)
  {
    return;
  }

  geran_eng_mode_logging_enabled = geran_eng_mode_enhanced_logging_enabled();
  
  if (geran_eng_mode_logging_enabled)
  {
    MSG_GERAN_LOW_4_G("Eng mode update type:%d, valid %d, req mask:0x%x, updated mask:0x%x",
                     geran_eng_mode_info_type,
                     valid_flag,
                     eng_mode_data_ptr->geran_stats_info.requested_bitmask.geran_modem_stats_mask,
                     eng_mode_data_ptr->geran_eng_mode_info.updated_info.geran_modem_stats_mask);
  }

  if (eng_mode_data_ptr->store_initialised == FALSE)
  {
    geran_eng_initialise_store(as_id);
    eng_mode_data_ptr->store_initialised = TRUE;
  }

  switch(geran_eng_mode_info_type)
  {
    case ENG_MODE_TIMING_ADVANCE:
    {
      uint32 new_data;

      if (geran_eng_mode_logging_enabled && valid_flag)
      {
        MSG_GERAN_LOW_2_G("Eng mode TA curr %d, new %d",
                          eng_mode_data_ptr->geran_eng_mode_info.timing_advance_value,
                          ( (uint32)(*(uint8*)param_copy) ));
      }

      if (valid_flag)
      {
        new_data = (uint32)(*(uint8*)param_copy);
        eng_mode_data_ptr->geran_eng_mode_info.timing_advance_valid_flag = TRUE;
      }
      else
      {
        new_data = (uint32)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
        eng_mode_data_ptr->geran_eng_mode_info.timing_advance_valid_flag = FALSE;
      }

      if (new_data != eng_mode_data_ptr->geran_eng_mode_info.timing_advance_value)
      {
        eng_mode_data_ptr->geran_eng_mode_info.timing_advance_value = new_data;
        data_changed = TRUE;
      }
    }
    break;

    case ENG_MODE_RADIO_LINK_TIMEOUT_VALUE:
    {
      uint32 new_data;

      if (geran_eng_mode_logging_enabled && valid_flag)
      {
        MSG_GERAN_LOW_2_G("Eng mode radio link timeout curr %d, new %d",
                          eng_mode_data_ptr->geran_eng_mode_info.radio_link_timeout_value,
                          ( (uint32)(*(uint8*)param_copy) ));
      }

      if (valid_flag)
      {
        new_data = (uint32)(*(uint8*)param_copy);
        eng_mode_data_ptr->geran_eng_mode_info.radio_link_timeout_valid_flag = TRUE;
      }
      else
      {
        new_data = (uint32)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
        eng_mode_data_ptr->geran_eng_mode_info.radio_link_timeout_valid_flag = FALSE;
      }

      if (new_data != eng_mode_data_ptr->geran_eng_mode_info.radio_link_timeout_value)
      {
        eng_mode_data_ptr->geran_eng_mode_info.radio_link_timeout_value = new_data;
        data_changed = TRUE;
      }
    }
    break;

    case ENG_MODE_HOPPING_FREQUENCY_LIST:
      eng_mode_data_ptr->geran_eng_mode_info.hopping_freq_list_valid_flag = valid_flag;
      if(valid_flag)
      {
        /*TODO - Bad code - should not use memcmp on a struct!*/
        if(memcmp(&eng_mode_data_ptr->geran_eng_mode_info.hopping_freq_list,
                  param_copy,
                  sizeof(eng_mode_frequency_list_T)) != 0)
        {
          memscpy(&(eng_mode_data_ptr->geran_eng_mode_info.hopping_freq_list),
                   sizeof(eng_mode_data_ptr->geran_eng_mode_info.hopping_freq_list),
                   (eng_mode_frequency_list_T *)param_copy,
                   sizeof(eng_mode_frequency_list_T));
        }
      }

    break;

    case ENG_MODE_RX_QUAL:
    {
      uint32 new_data;

      if (geran_eng_mode_logging_enabled && valid_flag)
      {
        MSG_GERAN_LOW_2_G("Eng mode RX qual curr %d, new %d",
                          eng_mode_data_ptr->geran_eng_mode_info.rx_qual,
                          ( (uint32)(*(uint8*)param_copy) ));
      }

      if (valid_flag)
      {
        new_data = (uint32)(*(uint8*)param_copy);
        eng_mode_data_ptr->geran_eng_mode_info.rx_qual_valid_flag = TRUE;
      }
      else
      {
        new_data = (uint32)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
        eng_mode_data_ptr->geran_eng_mode_info.rx_qual_valid_flag = FALSE;
      }

      if (new_data != eng_mode_data_ptr->geran_eng_mode_info.rx_qual)
      {
        eng_mode_data_ptr->geran_eng_mode_info.rx_qual = new_data;
        data_changed = TRUE;
      }
    }
    break;

    case ENG_MODE_GMSK_CV_VALUE:
      if (geran_eng_mode_logging_enabled && valid_flag)
      {
        MSG_GERAN_LOW_2_G("Eng mode GMSK CV Value curr %d, new %d",
                          eng_mode_data_ptr->geran_eng_mode_info.gmsk_cv_value,
                          ( (uint32)(*(uint8*)param_copy) ));
      }

      eng_mode_data_ptr->geran_eng_mode_info.gmsk_cv_value_valid_flag = valid_flag;
      eng_mode_data_ptr->geran_eng_mode_info.gmsk_cv_value            = (uint32)(*(uint8*)param_copy);

    break;

    case ENG_MODE_PSK_CV_VALUE:
      if (geran_eng_mode_logging_enabled && valid_flag)
      {
        MSG_GERAN_LOW_2_G("Eng mode PSK CV Value curr %d, new %d",
                          eng_mode_data_ptr->geran_eng_mode_info.psk_cv_value,
                          ( (uint32)(*(uint8*)param_copy) ));
      }

      eng_mode_data_ptr->geran_eng_mode_info.psk_cv_value_valid_flag = valid_flag;
      eng_mode_data_ptr->geran_eng_mode_info.psk_cv_value            = (uint32)(*(uint8*)param_copy);

    break;

    case ENG_MODE_SCELL_INFO:
      MSG_GERAN_LOW_3_G("Eng mode Scell Info curr CI:%d, new CI:%d, valid_flag:%d",
                        eng_mode_data_ptr->geran_eng_mode_info.scell_info.cell_id,
                        ((*(eng_mode_scell_info_t*)param_copy).cell_id),
                        valid_flag);

      MSG_GERAN_LOW_2_G("Eng mode req mask:0x%x, updated mask:0x%x",
                     eng_mode_data_ptr->geran_stats_info.requested_bitmask.geran_modem_stats_mask,
                     eng_mode_data_ptr->geran_eng_mode_info.updated_info.geran_modem_stats_mask);

      if (geran_eng_mode_logging_enabled && valid_flag)
      {
        eng_mode_scell_info_t *new_scell_info_ptr = (eng_mode_scell_info_t*)param_copy;

        MSG_GERAN_LOW_4_G("Eng mode Scell Info LAC %d, PLMN %6x BSIC %d RAC %d",
                          new_scell_info_ptr->lac,
                          ( (new_scell_info_ptr->plmn.plmn_id[0] << 16) + \
                          (  new_scell_info_ptr->plmn.plmn_id[1] << 8)  + \
                          new_scell_info_ptr->plmn.plmn_id[2]),
                          new_scell_info_ptr->bsic,
                          new_scell_info_ptr->rac);

        MSG_GERAN_LOW_2_G("Eng mode Scell Info ARFCN %d, Band %d",new_scell_info_ptr->arfcn,
                                                                  new_scell_info_ptr->band);
      }

      if (valid_flag == TRUE)
      {
        if ((eng_mode_data_ptr->geran_eng_mode_info.scell_info.cell_id) != (*(eng_mode_scell_info_t*)param_copy).cell_id)
        {
          eng_mode_data_ptr->geran_eng_mode_info.scell_info = *(eng_mode_scell_info_t*)param_copy;
          time_get(eng_mode_data_ptr->geran_eng_mode_info.scell_info.time_stamp);
          data_changed = TRUE;
        }
      }
      else
      {
        if (eng_mode_data_ptr->geran_eng_mode_info.scell_info.cell_id != GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER)
        {
          memset(&eng_mode_data_ptr->geran_eng_mode_info.scell_info,0,sizeof(eng_mode_data_ptr->geran_eng_mode_info.scell_info));
          eng_mode_data_ptr->geran_eng_mode_info.scell_info.cell_id = GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
          data_changed = TRUE;
        }
      }
    break;

    case ENG_MODE_SCELL_MEASUREMENTS:
    {
      if (geran_eng_mode_logging_enabled && valid_flag)
      {
        eng_mode_scell_measurements_t *new_scell_meas_ptr = (eng_mode_scell_measurements_t*)param_copy;

        MSG_GERAN_LOW_5_G("Eng mode Scell Meas C1 %d, C2 %d, rx_lev %d, rx qual sub %d, chan_type %d",
                          new_scell_meas_ptr->C1,
                          new_scell_meas_ptr->C2,
                          new_scell_meas_ptr->rx_lev,
                          new_scell_meas_ptr->rx_qual_sub,
                          new_scell_meas_ptr->channel_type);

        MSG_GERAN_LOW_5_G("Eng mode Scell Meas rx lev sub %d, rx lev full %d, rx qual full %d, C31 %d, C32 %d",
                          new_scell_meas_ptr->rx_lev_sub,
                          new_scell_meas_ptr->rx_lev_full,
                          new_scell_meas_ptr->rx_qual_full,
                          new_scell_meas_ptr->C31,
                          new_scell_meas_ptr->C32);
      }

      /* If toggled between valid/non-valid, assume data has changed */
      if (valid_flag != eng_mode_data_ptr->geran_eng_mode_info.scell_measurements_valid_flag)
      {
        data_changed = TRUE;

        /* if now valid, copy over new data */
        if (valid_flag)
        {
          eng_mode_data_ptr->geran_eng_mode_info.scell_measurements = *(eng_mode_scell_measurements_t *)param_copy;
          time_get(eng_mode_data_ptr->geran_eng_mode_info.scell_measurements.time_stamp);
          eng_mode_data_ptr->geran_eng_mode_info.scell_measurements_valid_flag = TRUE;
        }
        else
        /* if now not valid, invalidate all data */
        {
          memset(&eng_mode_data_ptr->geran_eng_mode_info.scell_measurements,
                 0,
                 sizeof(eng_mode_scell_measurements_t));
          eng_mode_data_ptr->geran_eng_mode_info.scell_measurements_valid_flag = FALSE;
        }
      }
      else
      {
        /* if data previously valid, then see if value has changed and update if so */
        if (valid_flag)
        {
          if (geran_eng_scell_meas_diff(&eng_mode_data_ptr->geran_eng_mode_info.scell_measurements,(eng_mode_scell_measurements_t *)param_copy) == TRUE)
          {
            eng_mode_data_ptr->geran_eng_mode_info.scell_measurements = *(eng_mode_scell_measurements_t *)param_copy;
            time_get(eng_mode_data_ptr->geran_eng_mode_info.scell_measurements.time_stamp);
            data_changed = TRUE;
          }
          else
          {
            if (geran_eng_mode_always_update_cb(ENG_MODE_SCELL_MEASUREMENTS))
            {
              /* the data has not changed but as we have a callback that is set to always_update,
                 update the time stamp and trigger the callback */
              time_get(eng_mode_data_ptr->geran_eng_mode_info.scell_info.time_stamp);
              always_update_cb = TRUE;
            }
          }
        }
      }
      /* if data previously not valid, it should be invalidated, so nothing to do */
      break;
    }

    case ENG_MODE_TX_PWR:
      if(valid_flag)
      {
        uint8  TxLevHi;

        if(geran_eng_tx_pwr_diff(eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_in_use,(int16*)param_copy, &TxLevHi) == TRUE)
        {
          memscpy(eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_in_use,
                  sizeof(eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_in_use),
                  (int16 *)param_copy,
                  sizeof(eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_in_use));
          data_changed = TRUE;
        }
        /*
        ** add the info required for calculating power in dBm.
        */
        eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.tx_pa    = l1_log_get_TxPa_state();
        eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.tx_lev   = TxLevHi;
        eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.is_valid = TRUE;  /* still valid if no change */

        if (geran_eng_mode_logging_enabled)
        {
          MSG_GERAN_LOW_4_G("Eng mode TX PWR band %d, tx lev %d, tx pa %d tx pwr %d",
                            eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.band,
                            eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.tx_lev,
                            eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.tx_pa,
                            eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.tx_pwr);
     }
     }
    break;

    case ENG_MODE_TX_BAND:
      if(valid_flag)
      {
          if (geran_eng_mode_logging_enabled)
          {
            MSG_GERAN_LOW_1_G("Eng mode TX band %d", (*(eng_mode_sys_band_T*)param_copy));
          }

          /* need to know the band to calculate the power */
          eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info.band       = *(eng_mode_sys_band_T*)param_copy;
          eng_mode_data_ptr->geran_eng_mode_info.tx_pwr_pa_info_valid_flag = TRUE;
      }
    break;

    case ENG_MODE_AMR_INFO:
    {
      if (geran_eng_mode_logging_enabled && valid_flag)
      {
        eng_mode_amr_info_t *amr_info_ptr = (eng_mode_amr_info_t *)param_copy;

        MSG_GERAN_LOW_4_G("Eng mode AMR Info ul_chan_mode %d, dl_chan_mode %d, dl_dtx %d, dl_c_over_i %d",
                           amr_info_ptr->ul_chan_mode,
                           amr_info_ptr->dl_chan_mode,
                           amr_info_ptr->dl_dtx,
                           amr_info_ptr->dl_c_over_i);

        MSG_GERAN_LOW_4_G("Eng mode AMR Info ul_acs %d %d %d %d", amr_info_ptr->ul_acs[0],
                                                                  amr_info_ptr->ul_acs[1],
                                                                  amr_info_ptr->ul_acs[2],
                                                                  amr_info_ptr->ul_acs[3]);

        MSG_GERAN_LOW_4_G("Eng mode AMR Info dl_acs %d %d %d %d", amr_info_ptr->dl_acs[0],
                                                                  amr_info_ptr->dl_acs[1],
                                                                  amr_info_ptr->dl_acs[2],
                                                                  amr_info_ptr->dl_acs[3]);
      }

      /* If toggled between valid/non-valid, assume data has changed */
      if (valid_flag != eng_mode_data_ptr->geran_eng_mode_info.amr_info_valid_flag)
      {
        data_changed = TRUE;

        /* if now valid, copy over new data */
        if (valid_flag)
        {
          eng_mode_data_ptr->geran_eng_mode_info.amr_info = *(eng_mode_amr_info_t *)param_copy;
          eng_mode_data_ptr->geran_eng_mode_info.amr_info_valid_flag = TRUE;
        }
        else
        /* if now not valid, invalidate all data */
        {
          memset(&(eng_mode_data_ptr->geran_eng_mode_info.amr_info),
                 (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
                 sizeof(eng_mode_amr_info_t));
          eng_mode_data_ptr->geran_eng_mode_info.amr_info_valid_flag = FALSE;
        }
      }
      else
      {
        /* if data previously valid, then see if value has changed and update if so */
        if (valid_flag)
        {
          if(geran_eng_amr_info_diff(&eng_mode_data_ptr->geran_eng_mode_info.amr_info,(eng_mode_amr_info_t*)param_copy) == TRUE)
          {
            eng_mode_data_ptr->geran_eng_mode_info.amr_info = *(eng_mode_amr_info_t *)param_copy;

            data_changed = TRUE;
          }
        }
      }
      /* if data previously not valid, it should be invalidated, so nothing to do */
      break;
    }

    case ENG_MODE_DTX_UL_STATUS:
    {
      uint8 new_data;

      if (valid_flag)
      {
        if (geran_eng_mode_logging_enabled)
        {
          MSG_GERAN_LOW_1_G("Eng mode DTX UL Status %d", ((uint8)(*(uint8*)param_copy)));
        }

        new_data = (uint8)(*(uint8*)param_copy);
        eng_mode_data_ptr->geran_eng_mode_info.dtx_ul_status_valid_flag = TRUE;
      }
      else
      {
        new_data = (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
        eng_mode_data_ptr->geran_eng_mode_info.dtx_ul_status_valid_flag = FALSE;
      }

      if (new_data != eng_mode_data_ptr->geran_eng_mode_info.dtx_ul_status)
      {
        eng_mode_data_ptr->geran_eng_mode_info.dtx_ul_status = new_data;
        data_changed = TRUE;
      }

      break;
    }

    case ENG_MODE_TBF_INFO:
    {
      /* If toggled between valid/non-valid, assume data has changed */
      if (valid_flag != eng_mode_data_ptr->geran_eng_mode_info.tbf_info_valid_flag)
      {
        data_changed = TRUE;

        /* if now valid, copy over new data */
        if (valid_flag)
        {
          if (geran_eng_mode_logging_enabled)
          {
            eng_mode_tbf_info_t *tbf_info_ptr = (eng_mode_tbf_info_t *)param_copy;

            MSG_GERAN_LOW_2_G("Eng mode TBF INFO MAC Mode %d TBF Mode %d", tbf_info_ptr->mac_mode, tbf_info_ptr->tbf_mode);
          }

          eng_mode_data_ptr->geran_eng_mode_info.tbf_info = *(eng_mode_tbf_info_t *)param_copy;
          eng_mode_data_ptr->geran_eng_mode_info.tbf_info_valid_flag = TRUE;
        }
        else
        /* if now not valid, invalidate all data */
        {
          memset(&(eng_mode_data_ptr->geran_eng_mode_info.tbf_info),
                 (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
                 sizeof(eng_mode_tbf_info_t));
          eng_mode_data_ptr->geran_eng_mode_info.tbf_info_valid_flag = FALSE;
        }
      }
      else
      {
        /* if data previously valid, then see if value has changed and update if so */
        if (valid_flag)
        {
          if(geran_eng_tbf_info_diff(&eng_mode_data_ptr->geran_eng_mode_info.tbf_info,(eng_mode_tbf_info_t *)param_copy) == TRUE)
          {
            eng_mode_data_ptr->geran_eng_mode_info.tbf_info = *(eng_mode_tbf_info_t *)param_copy;

            data_changed = TRUE;
          }
        }
      }
      /* if data previously not valid, it should be invalidated, so nothing to do */
      break;
    }

    case ENG_MODE_NW_PARAMS:
    {
      if (geran_eng_mode_logging_enabled && valid_flag)
      {
        eng_mode_nw_params_t *nw_params_ptr = (eng_mode_nw_params_t *)param_copy;

        MSG_GERAN_LOW_4_G("Eng mode NW Params NC mode %d, NMO %d, MSC Rev %d, SGSN Rev %d",
                          nw_params_ptr->nc_mode,
                          nw_params_ptr->nmo,
                          nw_params_ptr->msc_rev,
                          nw_params_ptr->sgsn_rev);
      }

      /* If toggled between valid/non-valid, assume data has changed */
      if (valid_flag != eng_mode_data_ptr->geran_eng_mode_info.nw_params_valid_flag)
      {
        data_changed = TRUE;

        /* if now valid, copy over new data */
        if (valid_flag)
        {
          eng_mode_data_ptr->geran_eng_mode_info.nw_params = *(eng_mode_nw_params_t*)param_copy;
          eng_mode_data_ptr->geran_eng_mode_info.nw_params_valid_flag = TRUE;
        }
        else
        /* if now not valid, invalidate all data */
        {
          memset(&(eng_mode_data_ptr->geran_eng_mode_info.nw_params),
                 (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
                 sizeof(eng_mode_nw_params_t));
          eng_mode_data_ptr->geran_eng_mode_info.nw_params_valid_flag = FALSE;
        }
      }
      else
      {
        /* if data previously valid, then see if value has changed and update if so */
        if (valid_flag)
        {
          if (geran_eng_nw_params_diff(&eng_mode_data_ptr->geran_eng_mode_info.nw_params,
                                       (eng_mode_nw_params_t*)param_copy) == TRUE)
          {
            eng_mode_data_ptr->geran_eng_mode_info.nw_params = *(eng_mode_nw_params_t *)param_copy;

            data_changed = TRUE;
          }
        }
      }
      /* if data previously not valid, it should be invalidated, so nothing to do */
      break;
    }

    case ENG_MODE_GPRS_UL_CS:
    {
      uint8 new_data;

      if (valid_flag)
      {
        if (geran_eng_mode_logging_enabled)
        {
          MSG_GERAN_LOW_1_G("Eng mode GPRS UL CS %d", ((uint8)(*(uint8*)param_copy)) );
        }

        new_data = (uint8)(*(uint8*)param_copy);
        eng_mode_data_ptr->geran_eng_mode_info.gprs_ul_cs_valid_flag = TRUE;
      }
      else
      {
        new_data = (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
        eng_mode_data_ptr->geran_eng_mode_info.gprs_ul_cs_valid_flag = FALSE;
      }

      if (new_data != eng_mode_data_ptr->geran_eng_mode_info.gprs_ul_cs)
      {
        eng_mode_data_ptr->geran_eng_mode_info.gprs_ul_cs = new_data;
        data_changed = TRUE;
      }
      break;
    }

    case ENG_MODE_GPRS_DL_CS:
    {
      uint8 new_data;

      if (valid_flag)
      {
        if (geran_eng_mode_logging_enabled)
        {
          MSG_GERAN_LOW_1_G("Eng mode GPRS DL CS %d", ((uint8)(*(uint8*)param_copy)) );
        }

        new_data = (uint8)(*(uint8*)param_copy);
        eng_mode_data_ptr->geran_eng_mode_info.gprs_dl_cs_valid_flag = TRUE;
      }
      else
      {
        new_data = (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
        eng_mode_data_ptr->geran_eng_mode_info.gprs_dl_cs_valid_flag = FALSE;
      }

      if (new_data != eng_mode_data_ptr->geran_eng_mode_info.gprs_dl_cs)
      {
        eng_mode_data_ptr->geran_eng_mode_info.gprs_dl_cs = new_data;
        data_changed = TRUE;
      }

      break;
    }

    case ENG_MODE_EGPRS_UL_MCS:
    {
      uint8 new_data;

      if (valid_flag)
      {
        if (geran_eng_mode_logging_enabled)
        {
          MSG_GERAN_LOW_1_G("Eng mode EGPRS UL CS %d", ((uint8)(*(uint8*)param_copy)) );
        }

        new_data = (uint8)(*(uint8*)param_copy);
        eng_mode_data_ptr->geran_eng_mode_info.egprs_ul_mcs_valid_flag = TRUE;
      }
      else
      {
        new_data = (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
        eng_mode_data_ptr->geran_eng_mode_info.egprs_ul_mcs_valid_flag = FALSE;
      }

      if (new_data != eng_mode_data_ptr->geran_eng_mode_info.egprs_ul_mcs)
      {
        eng_mode_data_ptr->geran_eng_mode_info.egprs_ul_mcs = new_data;
        data_changed = TRUE;
      }

      break;
    }

    case ENG_MODE_EGPRS_DL_MCS:
    {
      uint8 new_data;

      if (valid_flag)
      {
        if (geran_eng_mode_logging_enabled)
        {
          MSG_GERAN_LOW_1_G("Eng mode EGPRS DL CS %d", ((uint8)(*(uint8*)param_copy)) );
        }

        new_data = (uint8)(*(uint8*)param_copy);
        eng_mode_data_ptr->geran_eng_mode_info.egprs_dl_mcs_valid_flag = TRUE;
      }
      else
      {
        new_data = (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
        eng_mode_data_ptr->geran_eng_mode_info.egprs_dl_mcs_valid_flag = FALSE;
      }

      if (new_data != eng_mode_data_ptr->geran_eng_mode_info.egprs_dl_mcs)
      {
        eng_mode_data_ptr->geran_eng_mode_info.egprs_dl_mcs = new_data;
        data_changed = TRUE;
      }

      break;
    }

    case ENG_MODE_EDGE_MEAS:
    {
      if (geran_eng_mode_logging_enabled && valid_flag)
      {
        eng_mode_edge_meas_t *edge_meas_ptr = (eng_mode_edge_meas_t *)param_copy;

        MSG_GERAN_LOW_2_G("Eng mode EDGE Meas lq_meas_mode %d, bep_period %d",edge_meas_ptr->lq_meas_mode, edge_meas_ptr->bep_period);
      }

      /* If toggled between valid/non-valid, assume data has changed */
      if (valid_flag != eng_mode_data_ptr->geran_eng_mode_info.edge_meas_valid_flag)
      {
        data_changed = TRUE;

        /* if now valid, copy over new data */
        if (valid_flag)
        {
          eng_mode_data_ptr->geran_eng_mode_info.edge_meas = *(eng_mode_edge_meas_t *)param_copy;
          eng_mode_data_ptr->geran_eng_mode_info.edge_meas_valid_flag = TRUE;
        }
        else
        /* if now not valid, invalidate all data */
        {
          memset(&(eng_mode_data_ptr->geran_eng_mode_info.edge_meas),
                 (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
                 sizeof(eng_mode_edge_meas_t));
          eng_mode_data_ptr->geran_eng_mode_info.edge_meas_valid_flag = FALSE;
        }
      }
      else
      {
        /* if data previously valid, then see if value has changed and update if so */
        if (valid_flag)
        {
          if(geran_eng_edge_meas_diff(&eng_mode_data_ptr->geran_eng_mode_info.edge_meas,
                                      (eng_mode_edge_meas_t *)param_copy) == TRUE)
          {
            eng_mode_data_ptr->geran_eng_mode_info.edge_meas = *(eng_mode_edge_meas_t *)param_copy;

            data_changed = TRUE;
          }
        }
        /* if data previously not valid, it should be invalidated, so nothing to do */
      }
      break;
    }

    case ENG_MODE_SCELL_CONFIG:
    {
      if (geran_eng_mode_logging_enabled && valid_flag)
      {
        eng_mode_scell_config_t *scell_config_ptr = (eng_mode_scell_config_t *)param_copy;

        MSG_GERAN_LOW_3_G("Eng mode Scell Config PBCCH present %d, GPRS RX lev acc min %d, GPRS MS TXPWR MAX CCH %d",
                          scell_config_ptr->pbcch_present,
                          scell_config_ptr->gprs_rxlev_access_min,
                          scell_config_ptr->gprs_ms_txpwr_max_cch);
      }

      /* If toggled between valid/non-valid, assume data has changed */
      if (valid_flag != eng_mode_data_ptr->geran_eng_mode_info.scell_config_valid_flag)
      {
        data_changed = TRUE;

        /* if now valid, copy over new data */
        if (valid_flag)
        {
          eng_mode_data_ptr->geran_eng_mode_info.scell_config = *(eng_mode_scell_config_t *)param_copy;
          eng_mode_data_ptr->geran_eng_mode_info.scell_config_valid_flag = TRUE;
        }
        else
        /* if now not valid, invalidate all data */
        {
          memset(&(eng_mode_data_ptr->geran_eng_mode_info.scell_config),
                 (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
                 sizeof(eng_mode_scell_config_t));
          eng_mode_data_ptr->geran_eng_mode_info.scell_config_valid_flag = FALSE;
        }
      }
      else
      {
        /* if data previously valid, then see if value has changed and update if so */
        if (valid_flag)
        {
          if(geran_eng_scell_config_diff(&eng_mode_data_ptr->geran_eng_mode_info.scell_config,
                                         (eng_mode_scell_config_t *)param_copy) == TRUE)
          {
            eng_mode_data_ptr->geran_eng_mode_info.scell_config = *(eng_mode_scell_config_t *)param_copy;

            data_changed = TRUE;
          }
        }
      }
      /* if data previously not valid, it should be invalidated, so nothing to do */
      break;
    }

    case ENG_MODE_NMR_INFO:
    {
      if (geran_eng_mode_logging_enabled && valid_flag)
      {
        uint8 i = 0;

        eng_mode_nmr_info_t *nmr_info_ptr = (eng_mode_nmr_info_t * )param_copy;

        for (i=0; i<nmr_info_ptr->num_of_items && i<ARR_SIZE(nmr_info_ptr->nmr); i++)
        {
          MSG_GERAN_LOW_5_G("Eng mode NMR %d arfcn %d, bsic %d, rxlev %d, C1 %d",
                             i,
                             nmr_info_ptr->nmr[i].arfcn,
                             nmr_info_ptr->nmr[i].bsic,
                             nmr_info_ptr->nmr[i].rxlev,
                             nmr_info_ptr->nmr[i].C1);


          MSG_GERAN_LOW_4_G("Eng mode C2 %d, CI %d LAC %d PLMN %6x",
                             nmr_info_ptr->nmr[i].C2,
                             nmr_info_ptr->nmr[i].optional_info.cell_id,
                             nmr_info_ptr->nmr[i].optional_info.lac,
                             ( (nmr_info_ptr->nmr[i].optional_info.plmn.plmn_id[0] << 16) + \
                               (nmr_info_ptr->nmr[i].optional_info.plmn.plmn_id[1] << 8)  + \
                                nmr_info_ptr->nmr[i].optional_info.plmn.plmn_id[2]));
        }
      }

      /* If toggled between valid/non-valid, assume data has changed */
      if (valid_flag != eng_mode_data_ptr->geran_eng_mode_info.nmr_info_valid_flag)
      {
        data_changed = TRUE;

        /* if now valid, copy over new data */
        if (valid_flag)
        {
          eng_mode_data_ptr->geran_eng_mode_info.nmr_info = *(eng_mode_nmr_info_t *)param_copy;
          eng_mode_data_ptr->geran_eng_mode_info.nmr_info_valid_flag = TRUE;
        }
        else
        /* if now not valid, invalidate all data */
        {
          memset(&eng_mode_data_ptr->geran_eng_mode_info.nmr_info,
                 0,
                 sizeof(eng_mode_nmr_info_t));
          eng_mode_data_ptr->geran_eng_mode_info.nmr_info_valid_flag = FALSE;
        }
      }
      else
      {
        /* if data previously valid, then see if value has changed and update if so */
        if (valid_flag)
        {
          if(geran_eng_nmr_info_diff(&eng_mode_data_ptr->geran_eng_mode_info.nmr_info,
                                     (eng_mode_nmr_info_t*)param_copy) == TRUE)
          {
            eng_mode_data_ptr->geran_eng_mode_info.nmr_info = *(eng_mode_nmr_info_t *)param_copy;

            data_changed = TRUE;
          }
          else
          {
            if (geran_eng_mode_always_update_cb(ENG_MODE_NMR_INFO))
            {
              /* the data has not changed but as we have a callback that is set to always_update,
                 update the time stamp and trigger the callback */
              time_get(eng_mode_data_ptr->geran_eng_mode_info.nmr_info.time_stamp);
              always_update_cb = TRUE;
            }
          }
        }
      }
      /* if data previously not valid, it should be invalidated, so nothing to do */
      break;
    }

    case ENG_MODE_HOPPING_STATUS:
    {
      if (geran_eng_mode_logging_enabled && valid_flag)
      {
        MSG_GERAN_LOW_1_G("Eng mode Hopping status %d", ((eng_mode_hopping_status_t *)param_copy)->hopping_status);
      }

      /* If toggled between valid/non-valid, assume data has changed */
      if (valid_flag != eng_mode_data_ptr->geran_eng_mode_info.hopping_status_valid_flag)
      {
        data_changed = TRUE;

        /* if now valid, copy over new data */
        if (valid_flag)
        {
          eng_mode_data_ptr->geran_eng_mode_info.hopping_status = *(eng_mode_hopping_status_t *)param_copy;
          eng_mode_data_ptr->geran_eng_mode_info.hopping_status_valid_flag = TRUE;
        }
        else
        /* if now not valid, invalidate all data */
        {
          memset(&eng_mode_data_ptr->geran_eng_mode_info.hopping_status,
                 (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
                 sizeof(eng_mode_hopping_status_t));
          eng_mode_data_ptr->geran_eng_mode_info.hopping_status_valid_flag = FALSE;
        }
      }
      else
      {
        /* if data previously valid, then see if value has changed and update if so */
        if (valid_flag)
        {
          if (geran_eng_hopping_status_diff(&eng_mode_data_ptr->geran_eng_mode_info.hopping_status,
                                            (eng_mode_hopping_status_t *)param_copy) == TRUE)
          {
            eng_mode_data_ptr->geran_eng_mode_info.hopping_status = *(eng_mode_hopping_status_t *)param_copy;

            data_changed = TRUE;
          }
        }
      }
      /* if data previously not valid, it should be invalidated, so nothing to do */
      break;
    }

    case ENG_MODE_SIGNAL_INFO:
      eng_mode_data_ptr->geran_eng_mode_info.signal_info_valid_flag = valid_flag;
      if(valid_flag)
      {
        memscpy(&(eng_mode_data_ptr->geran_eng_mode_info.signal_info),
                 sizeof(eng_mode_data_ptr->geran_eng_mode_info.signal_info),
                 (geran_eng_mode_signal_info_t *)param_copy,
                 sizeof(geran_eng_mode_signal_info_t));
      }
    break;

    case ENG_MODE_CODEC_HO_INFO:
    {
      if (geran_eng_mode_logging_enabled)
      {
        sys_codec_ho_info *codec_info_ptr = (sys_codec_ho_info *)param_copy;

        MSG_GERAN_LOW_4_G("Eng mode Codec info cmd %d, codec %d, samp freq %d, HO type %d",
                          codec_info_ptr->cmd,
                          codec_info_ptr->speech_codec,
                          codec_info_ptr->speech_enc_samp_freq,
                          codec_info_ptr->ho_type);
      }

      /*Codec HO does not use valid flag for diff, RR always wants an update when the write cmd is called*/
      eng_mode_data_ptr->geran_eng_mode_info.ho_codec = *(sys_codec_ho_info *)param_copy;
      eng_mode_data_ptr->geran_eng_mode_info.ho_codec.as_id = as_id;
      eng_mode_data_ptr->geran_eng_mode_info.ho_codec_valid_flag = TRUE;

      data_changed = TRUE;
    }
    break;

#ifdef FEATURE_ENHANCED_AMR
    case ENG_MODE_CODEC_HO_SAMPLE_RATE:
      {
        if (geran_eng_mode_logging_enabled)
        {
          MSG_GERAN_LOW_1_G("Eng mode Codec sample rate %d", (((geran_eng_mode_codec_ho_sample_rate_t *)param_copy)->speech_enc_samp_freq));
        }

        sys_codec_ho_info ho_codec = eng_mode_data_ptr->geran_eng_mode_info.ho_codec;
        boolean ho_codec_valid_flag = eng_mode_data_ptr->geran_eng_mode_info.ho_codec_valid_flag;

        /*update the sampling rate*/
        ho_codec.speech_enc_samp_freq = ((geran_eng_mode_codec_ho_sample_rate_t *)param_copy)->speech_enc_samp_freq;

        MSG_GERAN_LOW_1_G("eng mode sample rate update: %d",ho_codec.speech_enc_samp_freq);

        /*sample rate update is a codec info update*/
        ho_codec.cmd = SYS_SPEECH_CODEC_INFO;

        geran_eng_mode_data_write(ENG_MODE_CODEC_HO_INFO, (void *)&ho_codec, ho_codec_valid_flag);

        data_changed = FALSE;
      }
    break;
#endif /*FEATURE_ENHANCED_AMR*/

    case ENG_MODE_CURRENT_L1_TIMESLOT:
    {
      if (geran_eng_mode_logging_enabled && valid_flag)
      {
        MSG_GERAN_LOW_1_G("Eng mode Curr L1 timeslot %d", ((eng_mode_curr_l1_ts_t *)param_copy)->timeslot );
      }

      /* If toggled between valid/non-valid, assume data has changed */
      if (valid_flag != eng_mode_data_ptr->geran_eng_mode_info.current_timeslot_valid_flag)
      {
        data_changed = TRUE;

        /* if now valid, copy over new data */
        if (valid_flag)
        {
          eng_mode_data_ptr->geran_eng_mode_info.current_timeslot = *(eng_mode_curr_l1_ts_t *)param_copy;
          eng_mode_data_ptr->geran_eng_mode_info.current_timeslot_valid_flag = TRUE;
        }
        else
        /* if now not valid, invalidate all data */
        {
          memset(&eng_mode_data_ptr->geran_eng_mode_info.current_timeslot,
                 GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER,
                 sizeof(eng_mode_curr_l1_ts_t));
          eng_mode_data_ptr->geran_eng_mode_info.current_timeslot_valid_flag = FALSE;
        }
      }
      else
      {
        /* if data previously valid, then see if value has changed and update if so */
        if (valid_flag)
        {
          if(geran_eng_curr_l1_timeslot_diff(&eng_mode_data_ptr->geran_eng_mode_info.current_timeslot,(eng_mode_curr_l1_ts_t*)param_copy) == TRUE)
          {
            eng_mode_data_ptr->geran_eng_mode_info.current_timeslot = *(eng_mode_curr_l1_ts_t *)param_copy;

            data_changed = TRUE;
          }
        }
      }
    }
    break;

    case ENG_MODE_CANDIDATE_PLMN_ID:
    {
      if (geran_eng_mode_logging_enabled)
      {
        eng_mode_plmn_id_t *plmn_id_ptr = (eng_mode_plmn_id_t *)param_copy;

        MSG_GERAN_LOW_1_G("Eng mode Candidate PLMN ID %6x", ( (plmn_id_ptr->plmn_id[0] << 16) + \
                                                              (plmn_id_ptr->plmn_id[1] << 8)  + \
                                                               plmn_id_ptr->plmn_id[2]) );
      }

      /* PLMN ID does not use valid flag for diff, client always wants an update when the write cmd is called */
      eng_mode_data_ptr->geran_eng_mode_info.candidate_plmn_id = *(eng_mode_plmn_id_t *)param_copy;
      eng_mode_data_ptr->geran_eng_mode_info.candidate_plmn_valid_flag = TRUE;

      data_changed = TRUE;
    }
    break;

    case ENG_MODE_GSM_CIPHER_INFO:

      if (geran_eng_mode_logging_enabled && valid_flag)
      {
        geran_eng_mode_gsm_cipher_info_t *cipher_info_ptr = (geran_eng_mode_gsm_cipher_info_t *)param_copy;

        MSG_GERAN_LOW_1_G("Eng mode Cipher info algorithm %d", cipher_info_ptr->cipher_alg);
      }

      /* If toggled between valid/non-valid, assume data has changed */
      if (valid_flag != eng_mode_data_ptr->geran_eng_mode_info.gsm_cipher_valid_flag)
      {
        data_changed = TRUE;

        /* if now valid, copy over new data */
        if (valid_flag)
        {
          eng_mode_data_ptr->geran_eng_mode_info.gsm_cipher = *(geran_eng_mode_gsm_cipher_info_t *)param_copy;
          eng_mode_data_ptr->geran_eng_mode_info.gsm_cipher_valid_flag = TRUE;
        }
        else
        /* if now not valid, invalidate all data */
        {
          eng_mode_data_ptr->geran_eng_mode_info.gsm_cipher.cipher_alg = ENG_MODE_GSM_CIPHER_NONE;
          eng_mode_data_ptr->geran_eng_mode_info.gsm_cipher_valid_flag = FALSE;
        }
      }
      else
      {
        /* if data previously valid, then see if value has changed and update if so */
        if (valid_flag)
        {
          if(geran_eng_gsm_cipher_diff(&eng_mode_data_ptr->geran_eng_mode_info.gsm_cipher,(geran_eng_mode_gsm_cipher_info_t*)param_copy) == TRUE)
          {
            eng_mode_data_ptr->geran_eng_mode_info.gsm_cipher = *(geran_eng_mode_gsm_cipher_info_t *)param_copy;

            data_changed = TRUE;
          }
        }
      }
    break;

    case ENG_MODE_ALL_INFORMATION:
      if (valid_flag == FALSE)
      {
        geran_eng_initialise_store(as_id);
      }
      else
      {
        MSG_GERAN_LOW_0_G("ENG_MODE_ALL_INFORMATION Obsolete");
      }
    break;

    default:
      MSG_GERAN_LOW_1_G("geran_eng_mode_data_write function is called with wrong data to write %d",
                          geran_eng_mode_info_type);

    break;
  }

  if (data_changed)
  {
    if (geran_eng_mode_info_type < ENG_MODE_MAX_BITMAPPED_INFO_TYPE_ELEMENTS)
    {
      eng_mode_data_ptr->geran_eng_mode_info.updated_info.geran_modem_stats_mask |= GERAN_ENG_MODE_INFO_TYPE_TO_BITMASK(geran_eng_mode_info_type);
    }

    if (geran_eng_mode_logging_enabled)
    {
      MSG_GERAN_LOW_3_G("Eng Mode type %d data changed, req mask 0x%x, updated mask 0x%x",
                        geran_eng_mode_info_type,
                        eng_mode_data_ptr->geran_stats_info.requested_bitmask.geran_modem_stats_mask,
                        eng_mode_data_ptr->geran_eng_mode_info.updated_info.geran_modem_stats_mask);
    }

    geran_eng_mode_notify_callbacks(geran_eng_mode_info_type, FALSE);
  }
  else if (always_update_cb)
  {
    if (geran_eng_mode_logging_enabled)
    {
      MSG_GERAN_LOW_0_G("Eng Mode always update set");
    }

    /*Do not update modem_stats as the data has not changed*/
    geran_eng_mode_notify_callbacks(geran_eng_mode_info_type, always_update_cb);
  }
  else
  {
    if (geran_eng_mode_logging_enabled)
    {
      MSG_GERAN_LOW_1_G("Eng Mode type %d data not changed",geran_eng_mode_info_type);
    }
  }
} /* geran_eng_mode_data_write */


/*===========================================================================

FUNCTION geran_eng_mode_always_update_cb

DESCRIPTION

  Check if there any callbacks which have behaviour: always update


PARAMETERS

  geran_eng_mode_info_type


RETURN VALUE
  Boolean - TRUE - cb found which always updates
            FALSE - cb not found which always updates

SIDE EFFECTS

===========================================================================*/
static boolean geran_eng_mode_always_update_cb(geran_eng_mode_info_type_t geran_eng_mode_info_type )
{
  uint16 i = 0;
  boolean always_update_cb = FALSE;
  sys_modem_as_id_e_type as_id = geran_map_gas_id_to_nas_id();
  geran_eng_mode_data_t *eng_mode_data_ptr = geran_get_eng_mode_data_ptr(as_id);

  if (eng_mode_data_ptr == NULL)
  {
    return FALSE;
  }

  for (i = 0; i < MAX_NUM_CALLBACK_REGISTRATIONS; i++)
  {
    if (eng_mode_data_ptr->eng_mode_cb_funcs[geran_eng_mode_info_type][i].func_ptr != NULL)
    {
      if (eng_mode_data_ptr->eng_mode_cb_funcs[geran_eng_mode_info_type][i].behaviour.always_report == TRUE)
      {
        always_update_cb = TRUE;
        break;
      }
    }
  }

  return always_update_cb;
}

/*===========================================================================

FUNCTION geran_eng_mode_notify_callbacks

DESCRIPTION
  Called when an eng value (that supports callbacks) has changed.  Calls
  all registerd callbacks for a geran_eng_mode_info_type.


PARAMETERS
  geran_eng_mode_info_type - a callback is registered per info type.
  data_ptr - callback data content

RETURN VALUE
  None

SIDE EFFECTS
  Read eng_mode_cb_funcs
  Calls external callback functions

===========================================================================*/

static void geran_eng_mode_notify_callbacks(geran_eng_mode_info_type_t geran_eng_mode_info_type,
                                            boolean always_update_cb
                                             )
{
  uint16 i = 0;
  boolean geran_eng_mode_logging_enabled;
  sys_modem_as_id_e_type as_id = geran_map_gas_id_to_nas_id();
  geran_eng_mode_data_t *eng_mode_data_ptr = geran_get_eng_mode_data_ptr(as_id);

  if (eng_mode_data_ptr == NULL)
  {
    return;
  }

  geran_eng_mode_logging_enabled = geran_eng_mode_enhanced_logging_enabled();

  switch (geran_eng_mode_info_type)
  {
    case ENG_MODE_SCELL_INFO:
    {
      scell_info_cb_fnc_ptr scell_cb_fnc_ptr = NULL;

      for (i = 0; i < MAX_NUM_CALLBACK_REGISTRATIONS; i++)
      {
        if (eng_mode_data_ptr->eng_mode_cb_funcs[ENG_MODE_SCELL_INFO][i].func_ptr != NULL)
        {
          scell_cb_fnc_ptr = (scell_info_cb_fnc_ptr)(eng_mode_data_ptr->eng_mode_cb_funcs[ENG_MODE_SCELL_INFO][i].func_ptr);
          scell_cb_fnc_ptr(eng_mode_data_ptr->geran_eng_mode_info.scell_info);

          if (geran_eng_mode_logging_enabled)
          {
            MSG_GERAN_LOW_1_G("Eng Mode trigger scell_info call back 0x%x",scell_cb_fnc_ptr);
          }
        }
      }
    }
    break;

    case ENG_MODE_CODEC_HO_INFO:
    {
      codec_ho_cb_fnc_ptr fnc_ptr = NULL;

      for (i = 0; i < MAX_NUM_CALLBACK_REGISTRATIONS; i++)
      {
        if (eng_mode_data_ptr->eng_mode_cb_funcs[ENG_MODE_CODEC_HO_INFO][i].func_ptr != NULL)
        {
          fnc_ptr = (codec_ho_cb_fnc_ptr)(eng_mode_data_ptr->eng_mode_cb_funcs[ENG_MODE_CODEC_HO_INFO][i].func_ptr);
          fnc_ptr(eng_mode_data_ptr->geran_eng_mode_info.ho_codec);

          if (geran_eng_mode_logging_enabled)
          {
            MSG_GERAN_LOW_1_G("Eng Mode trigger Codec HO info call back 0x%x",fnc_ptr);
          }
        }
      }
    }
    break;

    case ENG_MODE_SCELL_MEASUREMENTS:
    {
      scell_meas_cb_fnc_ptr scell_meas_fnc_ptr = NULL;

      for (i = 0; i < MAX_NUM_CALLBACK_REGISTRATIONS; i++)
      {
        if (eng_mode_data_ptr->eng_mode_cb_funcs[ENG_MODE_SCELL_MEASUREMENTS][i].func_ptr != NULL)
        {
          boolean trigger_cb = FALSE;

          /* only trigger CB with behaviour set to always report because the data has not changed*/
          if (always_update_cb)
          {
            if (eng_mode_data_ptr->eng_mode_cb_funcs[ENG_MODE_SCELL_MEASUREMENTS][i].behaviour.always_report)
            {
              trigger_cb = TRUE;
            }
          }
          else
          {
            trigger_cb = TRUE; /* data has changed trigger the callback*/
          }

          if (trigger_cb)
          {
            scell_meas_fnc_ptr = (scell_meas_cb_fnc_ptr)(eng_mode_data_ptr->eng_mode_cb_funcs[ENG_MODE_SCELL_MEASUREMENTS][i].func_ptr);
            scell_meas_fnc_ptr(eng_mode_data_ptr->geran_eng_mode_info.scell_measurements);

            if (geran_eng_mode_logging_enabled)
            {
              MSG_GERAN_LOW_1_G("Eng Mode trigger scell meas call back 0x%x",scell_meas_fnc_ptr);
            }
          }
        }
      }
    }
    break;

    case ENG_MODE_NMR_INFO:
    {
      nmr_info_cb_fnc_ptr nmr_fnc_ptr = NULL;

      for (i = 0; i < MAX_NUM_CALLBACK_REGISTRATIONS; i++)
      {
        if (eng_mode_data_ptr->eng_mode_cb_funcs[ENG_MODE_NMR_INFO][i].func_ptr != NULL)
        {
          boolean trigger_cb = FALSE;

          /* only trigger CB with behaviour set to always report because the data has not changed*/
          if (always_update_cb)
          {
            if (eng_mode_data_ptr->eng_mode_cb_funcs[ENG_MODE_NMR_INFO][i].behaviour.always_report)
            {
              trigger_cb = TRUE;
            }
          }
          else
          {
            trigger_cb = TRUE; /* data has changed trigger the callback*/
          }

          if (trigger_cb)
          {
            nmr_fnc_ptr = (nmr_info_cb_fnc_ptr)(eng_mode_data_ptr->eng_mode_cb_funcs[ENG_MODE_NMR_INFO][i].func_ptr);
            nmr_fnc_ptr(eng_mode_data_ptr->geran_eng_mode_info.nmr_info);

            if (geran_eng_mode_logging_enabled)
            {
              MSG_GERAN_LOW_1_G("Eng Mode trigger NMR info call back 0x%x",nmr_fnc_ptr);
            }
          }
        }
      }
    }
    break;

    case ENG_MODE_CANDIDATE_PLMN_ID:
    {
      candidate_plmn_cb_fnc_ptr fnc_ptr = NULL;

      for (i = 0; i < MAX_NUM_CALLBACK_REGISTRATIONS; i++)
      {
        if (eng_mode_data_ptr->eng_mode_cb_funcs[ENG_MODE_CANDIDATE_PLMN_ID][i].func_ptr != NULL)
        {
          fnc_ptr = (candidate_plmn_cb_fnc_ptr)(eng_mode_data_ptr->eng_mode_cb_funcs[ENG_MODE_CANDIDATE_PLMN_ID][i].func_ptr);
          fnc_ptr(eng_mode_data_ptr->geran_eng_mode_info.candidate_plmn_id);

          if (geran_eng_mode_logging_enabled)
          {
            MSG_GERAN_LOW_1_G("Eng Mode trigger Cand PLMN Id call back 0x%x",fnc_ptr);
          }
        }
      }
    }
    break;

    default:
      /*No action for unsupported call back*/
    break;
  }

#ifdef FEATURE_MODEM_STATISTICS
  /* FTD - ENG_MODE_GERAN_MODEM_STATS_INFO */
  if (eng_mode_data_ptr->geran_stats_info.requested_bitmask.geran_modem_stats_mask != 0)
  {
    uint64 requested_mask = eng_mode_data_ptr->geran_stats_info.requested_bitmask.geran_modem_stats_mask;
    uint64 updated_mask = eng_mode_data_ptr->geran_eng_mode_info.updated_info.geran_modem_stats_mask;

    /* Mask off bits not requested */
    updated_mask &= requested_mask;

    if (updated_mask != 0)
    {
      (void)cm_per_subs_stats_set_modem_info_signal(SYS_MODEM_MODULE_GSM, as_id);

      if (geran_eng_mode_logging_enabled)
      {
        MSG_GERAN_LOW_2_G("Eng Mode call CM stat info, updated mask %x, req mask %d",
                          updated_mask, requested_mask);
      }
    }
  }
#endif /* FEATURE_MODEM_STATISTICS */
}


/*===========================================================================

FUNCTION geran_eng_nmr_info_diff

DESCRIPTION

  Diffs to nmr info structs.


PARAMETERS

  eng_mode_nmr_info_t a_ptr - comparison struct 1
  eng_mode_nmr_info_t b_ptr - comparison struct 2


RETURN VALUE
  boolean - 1 - structs differ, 0 - No difference

SIDE EFFECTS


===========================================================================*/
static boolean geran_eng_nmr_info_diff(eng_mode_nmr_info_t* a_ptr,
                                       eng_mode_nmr_info_t* b_ptr)
{
  uint8 i = 0;

  if (a_ptr->num_of_items != b_ptr->num_of_items)
  {
    return TRUE;
  }

  for (i=0;i<a_ptr->num_of_items;i++)
  {
    if (a_ptr->nmr[i].arfcn != b_ptr->nmr[i].arfcn ||
        a_ptr->nmr[i].bsic  != b_ptr->nmr[i].bsic ||
        a_ptr->nmr[i].rxlev != b_ptr->nmr[i].rxlev ||
        a_ptr->nmr[i].C1 != b_ptr->nmr[i].C1 ||
        a_ptr->nmr[i].C2 != b_ptr->nmr[i].C2 ||
        a_ptr->nmr[i].C31 != b_ptr->nmr[i].C31 ||
        a_ptr->nmr[i].C32 != b_ptr->nmr[i].C32)
    {
      return TRUE;
    }

    if (a_ptr->nmr[i].optional_info.cell_id != b_ptr->nmr[i].optional_info.cell_id)
    {
      return TRUE;
    }

    if (a_ptr->nmr[i].optional_info.cell_id != GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER)
    {
      if ((a_ptr->nmr[i].optional_info.lac != b_ptr->nmr[i].optional_info.lac) ||
          (a_ptr->nmr[i].optional_info.plmn.plmn_id[0] != b_ptr->nmr[i].optional_info.plmn.plmn_id[0]) ||
          (a_ptr->nmr[i].optional_info.plmn.plmn_id[1] != b_ptr->nmr[i].optional_info.plmn.plmn_id[1]) ||
          (a_ptr->nmr[i].optional_info.plmn.plmn_id[2] != b_ptr->nmr[i].optional_info.plmn.plmn_id[2]) )
      {
        return TRUE;
      }
    }
  }

  return FALSE;
}

/*===========================================================================

FUNCTION geran_eng_scell_meas_diff

DESCRIPTION

  Diffs to scell meas structs.


PARAMETERS

  eng_mode_scell_measurements_t a_ptr - comparison struct 1
  eng_mode_scell_measurements_t b_ptr - comparison struct 2


RETURN VALUE
  boolean - 1 - structs differ, 0 - No difference

SIDE EFFECTS


===========================================================================*/
static boolean geran_eng_scell_meas_diff(eng_mode_scell_measurements_t* a_ptr,
                                         eng_mode_scell_measurements_t* b_ptr)
{
  if (a_ptr->C1 != b_ptr->C1)
  {
    return TRUE;
  }

  if (a_ptr->C2 != b_ptr->C2)
  {
    return TRUE;
  }

  if (a_ptr->channel_type != b_ptr->channel_type)
  {
    return TRUE;
  }

  if (a_ptr->rx_lev != b_ptr->rx_lev)
  {
    return TRUE;
  }

  if (a_ptr->rx_qual_sub != b_ptr->rx_qual_sub)
  {
    return TRUE;
  }

  if (a_ptr->rx_qual_full != b_ptr->rx_qual_full)
  {
    return TRUE;
  }

  if (a_ptr->rx_lev_sub != b_ptr->rx_lev_sub)
  {
    return TRUE;
  }

  if (a_ptr->rx_lev_full != b_ptr->rx_lev_full)
  {
    return TRUE;
  }

  if (a_ptr->C31 != b_ptr->C31)
  {
    return TRUE;
  }

  if (a_ptr->C32 != b_ptr->C32)
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION geran_eng_amr_info_diff

DESCRIPTION

  Diffs to amr structs.


PARAMETERS

  eng_mode_amr_info_t a_ptr - comparison struct 1
  eng_mode_amr_info_t b_ptr - comparison struct 2


RETURN VALUE
  boolean - 1 - structs differ, 0 - No difference

SIDE EFFECTS


===========================================================================*/
static boolean geran_eng_amr_info_diff(eng_mode_amr_info_t* a_ptr,
                                       eng_mode_amr_info_t* b_ptr)
{
  if (a_ptr->ul_chan_mode != b_ptr->ul_chan_mode)
  {
    return TRUE;
  }

  if (a_ptr->dl_chan_mode != b_ptr->dl_chan_mode)
  {
    return TRUE;
  }

  if (memcmp(a_ptr->ul_acs,b_ptr->ul_acs,sizeof(a_ptr->ul_acs)) != 0)
  {
    return TRUE;
  }

  if (memcmp(a_ptr->dl_acs,b_ptr->ul_acs,sizeof(a_ptr->dl_acs)) != 0)
  {
    return TRUE;
  }

  if (a_ptr->dl_dtx != b_ptr->dl_dtx)
  {
    return TRUE;
  }

  if (a_ptr->dl_c_over_i != b_ptr->dl_c_over_i)
  {
    return TRUE;
  }

  return FALSE;
}


/*===========================================================================

FUNCTION geran_eng_tbf_info_diff

DESCRIPTION

  Diffs to tbf info structs.


PARAMETERS

  eng_mode_tbf_info_t a_ptr - comparison struct 1
  eng_mode_tbf_info_t b_ptr - comparison struct 2


RETURN VALUE
  boolean - 1 - structs differ, 0 - No difference

SIDE EFFECTS


===========================================================================*/
static boolean geran_eng_tbf_info_diff(eng_mode_tbf_info_t* a_ptr,
                                       eng_mode_tbf_info_t* b_ptr)
{
  if (a_ptr->mac_mode != b_ptr->mac_mode)
  {
    return TRUE;
  }

  if (a_ptr->tbf_mode != b_ptr->tbf_mode)
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION geran_eng_edge_meas_diff

DESCRIPTION

  Diffs to tbf info structs.


PARAMETERS

  eng_mode_edge_meas_t a_ptr - comparison struct 1
  eng_mode_edge_meas_t b_ptr - comparison struct 2


RETURN VALUE
  boolean - 1 - structs differ, 0 - No difference

SIDE EFFECTS


===========================================================================*/
static boolean geran_eng_edge_meas_diff(eng_mode_edge_meas_t* a_ptr,
                                        eng_mode_edge_meas_t* b_ptr)
{
  if (a_ptr->bep_period != b_ptr->bep_period)
  {
    return TRUE;
  }

  if (a_ptr->lq_meas_mode != b_ptr->lq_meas_mode)
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION geran_eng_scell_config_diff

DESCRIPTION

  Diffs to tbf info structs.


PARAMETERS

  eng_mode_scell_config_t a_ptr - comparison struct 1
  eng_mode_scell_config_t b_ptr - comparison struct 2


RETURN VALUE
  boolean - 1 - structs differ, 0 - No difference

SIDE EFFECTS


===========================================================================*/
static boolean geran_eng_scell_config_diff(eng_mode_scell_config_t* a_ptr,
                                           eng_mode_scell_config_t* b_ptr)
{
  if ((a_ptr->pbcch_present != b_ptr->pbcch_present) ||
      (a_ptr->gprs_rxlev_access_min != b_ptr->gprs_rxlev_access_min) ||
      (a_ptr->gprs_ms_txpwr_max_cch != b_ptr->gprs_ms_txpwr_max_cch) )
  {
    return TRUE;
  }

  return FALSE;
}


/*===========================================================================

FUNCTION geran_eng_nw_params_diff

DESCRIPTION

  Diffs to tbf info structs.


PARAMETERS

  eng_mode_nw_params_t a_ptr - comparison struct 1
  eng_mode_nw_params_t b_ptr - comparison struct 2


RETURN VALUE
  boolean - 1 - structs differ, 0 - No difference

SIDE EFFECTS


===========================================================================*/
static boolean geran_eng_nw_params_diff(eng_mode_nw_params_t* a_ptr,
                                        eng_mode_nw_params_t* b_ptr)
{
  if (a_ptr->nc_mode != b_ptr->nc_mode)
  {
    return TRUE;
  }

  if (a_ptr->nmo != b_ptr->nmo)
  {
    return TRUE;
  }

  if (a_ptr->msc_rev != b_ptr->msc_rev)
  {
    return TRUE;
  }

  if (a_ptr->sgsn_rev != b_ptr->sgsn_rev)
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION geran_eng_tx_pwr_diff

DESCRIPTION

  Diffs to tx pwr arrays.


PARAMETERS

  int16 a_ptr - comparison struct 1
  int16 b_ptr - comparison struct 2


RETURN VALUE
  boolean - 1 - structs differ, 0 - No difference

SIDE EFFECTS


===========================================================================*/
static boolean geran_eng_tx_pwr_diff(int16* a_ptr, int16* b_ptr , uint8* highPwr)
{
  uint8 i = 0;
  boolean  ret_val = FALSE;

 *highPwr = b_ptr[0];

  for (i=0;i<ENG_MODE_TX_PWR_NUM_TIMESLOTS;i++)
  {
    if (a_ptr[i] != b_ptr[i])
    {
      ret_val = TRUE;
    }

    if(!b_ptr[i]) break;       /* if we find 0 stop   */

    if( *highPwr < b_ptr[i])
    {
        *highPwr = b_ptr[i];   /* return lowest level == highest pwr */
    }
  }

  return ret_val;
}

/*===========================================================================

FUNCTION geran_eng_hopping_status_diff

DESCRIPTION

  Diffs to hopping status.

PARAMETERS

  eng_mode_hopping_status_t a_ptr - comparison struct 1
  eng_mode_hopping_status_t b_ptr - comparison struct 2


RETURN VALUE
  boolean - 1 - structs differ, 0 - No difference

SIDE EFFECTS


===========================================================================*/
static boolean geran_eng_hopping_status_diff(eng_mode_hopping_status_t* a_ptr,
                                             eng_mode_hopping_status_t* b_ptr)
{
  if (a_ptr->hopping_status != b_ptr->hopping_status)
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION geran_eng_curr_l1_timeslot_diff

DESCRIPTION

  Diffs two curr l1 timeslots.

PARAMETERS

  eng_mode_curr_l1_ts_t a_ptr - comparison struct 1
  eng_mode_curr_l1_ts_t b_ptr - comparison struct 2


RETURN VALUE
  boolean - 1 - structs differ, 0 - No difference

SIDE EFFECTS


===========================================================================*/
static boolean geran_eng_curr_l1_timeslot_diff(eng_mode_curr_l1_ts_t* a_ptr,
                                               eng_mode_curr_l1_ts_t* b_ptr)
{
  if (a_ptr->timeslot != b_ptr->timeslot)
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION geran_eng_gsm_cipher_diff

DESCRIPTION

  Diffs gsm_cipher

PARAMETERS

  geran_eng_mode_gsm_cipher_info_t a_ptr - comparison struct 1
  geran_eng_mode_gsm_cipher_info_t b_ptr - comparison struct 2


RETURN VALUE
  boolean - 1 - structs differ, 0 - No difference

SIDE EFFECTS


===========================================================================*/
static boolean geran_eng_gsm_cipher_diff(geran_eng_mode_gsm_cipher_info_t* a_ptr,
                                         geran_eng_mode_gsm_cipher_info_t* b_ptr)
{
  if (a_ptr->cipher_alg != b_ptr->cipher_alg)
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION geran_multisim_eng_mode_refresh_cm

DESCRIPTION

  Refreshes CM with the stored eng mode data.

PARAMETERS

  geran_eng_mode_info_type - data for refresh
   


RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void geran_multisim_eng_mode_refresh_cm(geran_eng_mode_info_type_t geran_eng_mode_info_type
                                         )
{
#ifdef FEATURE_MODEM_STATISTICS
  sys_modem_as_id_e_type as_id = geran_map_gas_id_to_nas_id();
  geran_eng_mode_data_t *eng_mode_data_ptr = geran_get_eng_mode_data_ptr(as_id);

  if (eng_mode_data_ptr == NULL)
  {
    return;
  }

  if (geran_eng_mode_info_type < ENG_MODE_MAX_BITMAPPED_INFO_TYPE_ELEMENTS)
  {
    MSG_GERAN_LOW_1_G("Refreshing CM with eng mode type:%d",geran_eng_mode_info_type);
    eng_mode_data_ptr->geran_eng_mode_info.updated_info.geran_modem_stats_mask |= GERAN_ENG_MODE_INFO_TYPE_TO_BITMASK(geran_eng_mode_info_type);
    (void)cm_per_subs_stats_set_modem_info_signal(SYS_MODEM_MODULE_GSM, as_id);
  }
  else
  {
    MSG_GERAN_LOW_1_G("Refreshing type:%d outside of modem stats range",geran_eng_mode_info_type);
  }
#else /* FEATURE_MODEM_STATISTICS */
  NOTUSED(geran_eng_mode_info_type);
#endif /* !FEATURE_MODEM_STATISTICS */
}

/*===========================================================================

FUNCTION geran_eng_mode_enhanced_logging_enabled

DESCRIPTION

  Returns eng mode enhanced logging status.

PARAMETERS
  sys_modem_as_id_e_type as_id

RETURN VALUE
  Boolean - TRUE - enhanced logging enabled, FALSE - enhanced logging not enabled

SIDE EFFECTS


===========================================================================*/
static boolean geran_eng_mode_enhanced_logging_enabled(void)
{
  return rr_nv_eng_mode_debug_is_enabled();
}

/* EOF */


