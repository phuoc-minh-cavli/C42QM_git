/*!
  @file
  ftm_calibration_data_module.c

  @brief
  This file is used to store all data during calibration for xPT
 
  @addtogroup FTM_CALIBRATION_DATA_MODULE
  @{
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //components/rel/rfa.mpss/3.10/rf/common/ftm/src/ftm_calibration_data_module.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/09/17   gk      KW error fix
12/07/16   pvsr   Fix for KW errors
02/27/15   aa      KW warning fix
02/25/15   aa      AMPM scaling factor computation
09/03/14   cd      Remove legacy Tx override fields from data module
08/26/14   cd      Provide channel search utility to DPD map, to support 
                   multiple channel data co-existing at the same time
08/18/14   php     Fix power search to limit to only number of entries
07/10/14   cd/cri  Pin Cal Merge
06/27/14   aa      Fixed memory allocation size for RGI delta tables
04/09/14   ska    Pin Cal/char updates to remove the ref_chan operations such as finding ndB compression point etc
04/07/14   zhw     Fix compiler warning and call flow bug
03/19/14   zhh     limit the index of ftm_cal_data_dpd_map_row_type table
03/14/14   zhh     use debug_mode_en to control some ET debug info
03/06/14   zhw     Normalized Pin Cal support
01/08/14   cac     Add RGI delta table for EPT ET in different PA states
12/27/13   rsr     Added support for XPT capture results opcode.
12/11/13   cd      Change Tx override handlers to use RFLM Tx override for xPT 
                   cal
11/25/13   cd      Set initial state for current Tx override
11/14/13   cd      Support for RFLM-based Tx AGC override
09/10/13   shk     Support to pack M Line results in the response packet
07/24/13   cri     DPD capture buffer reuse support
07/23/13   jmf     Fix off-target compiler error
07/17/13   jmf     [xPT] New Delay Table separate from DPD data module
07/12/13   jmf     [xPT] Define and use increased DPD Map size of 20
07/05/13   jmf     [xPT] Calv4 ET delay fix lookup
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/21/13   jmf     [xPT] ET delay implementation for Cal V4
06/04/13   kai     Add setting DPD pointers to NULL after mem free
05/02/13   cd      - Compute IQ backoff correctly during Pin alignment
                   - IQ gain comes from RF overrides
04/30/13   cd      Fixed compiler warnings
04/29/13   cd      Apply Pin alignment during calibration
04/16/13   cri     Update to save row index when pushing an entry input by API user
04/12/13   cri     Add APIs to get and set unity HWIDs
04/11/13   cri     Use XPT mode in power lookup for uniqueness
04/09/13   cri     Add init and deinit for cal data module
04/09/13    id     Fixed bug to point to correct DPD Map Table
04/08/13   cri     Support for 2nd sweep of XPT calv4
04/02/13   cri     Update data module initalization 
03/29/13   cri     Moving the data module to rfcommon_data 
03/05/13   cri     Initial check in for data module

==============================================================================*/

#include "stringl.h" /* for memscpy */
#include "rfcom.h"
#include "rfcommon_msg.h"
#include "modem_mem.h"
#include "rfcommon_core_txlin_types.h"
#include "ftm_calibration_data_module.h"
#include "ftm_common_xpt.h"
#include "rfcommon_mdsp.h"
#include "fw_dpd_intf.h"
#include "ftm_common_data.h"
#include <math.h>

/*Local data structure*/

boolean ampm_regression_done_for_ref_channel = FALSE;
uint8 ref_chan_compression_point = 0;
uint8 ref_chan_row_index = 0;

/* LOCAL search APIs */
//static uint8 ftm_cal_data_search_unique_row(int8 pa_state, int16 rgi, int16 bias);
//static uint8 ftm_cal_data_search_power(int16 power, uint8 xpt_mode);
//static uint8 ftm_cal_data_search_cap_id(uint8 res_id);
//static uint8 ftm_cal_data_search_res_id(uint8 res_id);
//static uint8 ftm_cal_data_search_chan(uint32 chan);

/*============================================================================*/
/*!
  @name Radio Setup

  @brief
  This section contains the functions used by the Radio Setup opcode
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Init Data Module

  @details
  This function will initialize all data module parameters
*/
void ftm_cal_data_init_data_module(void)
{
  uint8 i;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  memset(module_ptr,0,sizeof(ftm_cal_data_module_type));

  for(i=0;i<RFCOM_TXLIN_NUM_CHANNELS;i++)
  {
    module_ptr->data_module[i].chan = FTM_COMMON_XPT_INV_CHAN;
  }

  /* Initialize the curr Tx override information */
  module_ptr->curr_tx_override.rflm_handle = 0;
  module_ptr->curr_tx_override.modem_chain = 0;
  module_ptr->curr_tx_override.txagc_seq = RFCOMMON_XPT_TX_AGC_OVR_SEQ_INVALID;
  module_ptr->curr_tx_override.xpt_mode = 0;
  module_ptr->curr_tx_override.iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
  module_ptr->curr_tx_override.iq_gain_action_type =
                                               FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
  module_ptr->curr_tx_override.env_scale = 
                                       FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID;
  module_ptr->curr_tx_override.env_scale_action_type =
                                               FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
  module_ptr->curr_tx_override.latch_immediate = FALSE;
  module_ptr->curr_tx_override.rgi = 0;
  module_ptr->curr_tx_override.papm_bias = RFDEVICE_PAPM_MIN_TX_BIAS_VOLTAGE;
  module_ptr->curr_tx_override.pa_state = RFM_PA_GAIN_STATE_0;
  module_ptr->curr_tx_override.pa_range = RFM_PA_R1_R0_0_0;
  module_ptr->curr_tx_override.pa_curr = 0;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tear Down Data Module

  @details
  This function will free all of the memory associated with the DPD
  map for all channels. This can be called with radio setup where master
  and slave state are 1, or called implicitly at the end of cal. 
*/
void ftm_cal_data_teardown_data_module(void)
{
  uint8 i;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  for(i=0;i<RFCOM_TXLIN_NUM_CHANNELS;i++)
  {
    module_ptr->data_module[i].chan = FTM_COMMON_XPT_INV_CHAN;
    if(module_ptr->data_module[i].rgi_delta_table != NULL)
    {
      modem_mem_free(module_ptr->data_module[i].rgi_delta_table,MODEM_MEM_CLIENT_RFA);
      module_ptr->data_module[i].rgi_delta_table = NULL;
    }
    if(module_ptr->data_module[i].rgi_delta_table_two != NULL)
    {
      modem_mem_free(module_ptr->data_module[i].rgi_delta_table_two,MODEM_MEM_CLIENT_RFA);
      module_ptr->data_module[i].rgi_delta_table_two = NULL;
    }
  }
}

/*============================================================================*/
/*!
  @name Tx Override

  @brief
  This section contains the functions used by the TX Override opcode
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set Current Override row index

  @details
  This function will set the current override row into which the Tx override
  data and other override information must be copied over by any
  subsequent calls to the data module

  @param override_row
  Override row into which data will be copied
 
  @return
  void
*/
void ftm_cal_data_set_current_override_row(uint8 override_row)
{
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  module_ptr->current_override_row = override_row;
}

/*! \} */


/*============================================================================*/
/*!
  @name XPT

  @brief
  This section contains the functions used by the XPT opcode
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Init Current Channel Map

  @details
  This function will check if the current channel that is selected has a
  DPD map already allocted. If it does not then it will create a new map
  and initialize the values. 

  @param void
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_init_cur_channel_map(uint32 chan)
{
  uint8 i;
  boolean status = TRUE;
  boolean table_exists = FALSE;
  uint8 temp_chan_idx;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  /* Check if we have initialized the data module for this 
     channel already. If we have, use that. */
  for(i=0;i<RFCOM_TXLIN_NUM_CHANNELS;i++)
  {
    if(module_ptr->data_module[i].chan == chan)
    {
      table_exists = TRUE;
      temp_chan_idx = i;
      break;
    }
  }

  if(table_exists == FALSE)
  {
    /* Find an unused table for our current channel */
    for(i=0;i<RFCOM_TXLIN_NUM_CHANNELS;i++)
    {
      /* Check for first unused channel */
      if(module_ptr->data_module[i].chan == FTM_COMMON_XPT_INV_CHAN)
      {
        /* Save current index to what we matched */
        temp_chan_idx = i;
        break;
      }
    }
    /* We didn't find any modem_mem_free tables */
    if(i==RFCOM_TXLIN_NUM_CHANNELS)
    {
      RF_MSG(RF_ERROR, "ftm_cal_data_init_data_module: Allocated too many tables");
      return FALSE;
    }

    /* Initialize pointers */
    module_ptr->data_module[temp_chan_idx].rgi_delta_table = NULL;
    module_ptr->data_module[temp_chan_idx].rgi_delta_table_two = NULL;

    module_ptr->data_module[temp_chan_idx].rgi_delta_table = 
      modem_mem_alloc(sizeof(ftm_cal_data_rgid_type),MODEM_MEM_CLIENT_RFA);
    if(module_ptr->data_module[temp_chan_idx].rgi_delta_table == NULL)
    {
      RF_MSG_1(RF_FATAL, "ftm_cal_data_init_data_module: Failed to allocate RGI table, size = %d", sizeof(ftm_cal_data_rgid_type));
      return FALSE;
    }
     module_ptr->data_module[temp_chan_idx].rgi_delta_table_two = 
      modem_mem_alloc(sizeof(ftm_cal_data_rgid_type),MODEM_MEM_CLIENT_RFA);
    if(module_ptr->data_module[temp_chan_idx].rgi_delta_table_two == NULL) 
    {
      RF_MSG_1(RF_FATAL, "ftm_cal_data_init_data_module: Failed to allocate RGI table Two, size = %d", sizeof(ftm_cal_data_rgid_type));
      return FALSE;
    }

    module_ptr->data_module[temp_chan_idx].chan = chan;
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Has the current channel been initialized

  @details
  This function will check if the desired channel has been initialized yet.
  This will be used by the FW processing response to see if it should
  push the results into the DPD map or not. Proto cal will not be using the
  DPD map so this will return false.

  @param uint32 chan
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_is_chan_initialized(uint32 chan)
{
  uint8 i;
  boolean table_exists = FALSE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  /* Check if we have initialized the data module for this 
     channel already. If we have, use that. */
  for(i=0;i<RFCOM_TXLIN_NUM_CHANNELS;i++)
  {
    if(module_ptr->data_module[i].chan == chan)
    {
      table_exists = TRUE;
      break;
    }
  }

  return table_exists;
}


#if 0

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Init Pin Table

  @details
  This function will initialize the Pin table and set num of entries to 0
*/
void ftm_cal_data_init_pin_map(void)
{
#if 0
  uint8 i;
  ftm_cal_data_pin_map_type *pin_table_ptr;

  /* Get a pointer to the Pin Table */
  pin_table_ptr = ftm_common_data_get_pin_table_ptr();

  memset(pin_table_ptr,0,sizeof(ftm_cal_data_pin_map_type));

  for(i=0;i<FTM_COMMON_XPT_NUM_PIN_MAP_ROWS;i++)
  {
    pin_table_ptr->table[i].chan = FTM_COMMON_XPT_INV_CHAN;
    pin_table_ptr->table[i].status = FTM_COMMON_XPT_INVALID;
  }

  pin_table_ptr->num_map_entries = 0;

  ampm_regression_done_for_ref_channel = FALSE;
#endif
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function stores the pin_cal_offset value into NV based on channel
  list stored in static Tx Char Chan, PA state and channel passed in. In error
  condition, returns FALSE
 
  @param lin_temp_comp_data
  temp comp data which contains Tx Char channel list, which contains
  Pin_cal_vs_freq NV structure
 
  @param pin_cal_nv
  Pin_cal_nv structure to be written to
 
  @param pa_state
  PA state
 
  @param channel
  uint32 Channel number
 
  @return
  TRUE if successful. FALSE if fail
*/
boolean
ftm_cal_data_store_pin_cal_offset
(  
  const rfcom_txlin_temp_comp_nv_data_type* lin_temp_comp_data,
  rfcommon_core_txlin_pin_cal_vs_freq_type* pin_cal_nv, 
  uint8 pa_state,
  uint32 channel,
  int16 pin_val
)
{
  boolean api_status = TRUE;

  if ( pin_cal_nv == NULL || lin_temp_comp_data == NULL || 
       pa_state >= RFCOM_TXLIN_NUM_LINEARIZERS )
  {
    RF_MSG_1( RF_ERROR, "ftm_cal_data_store_pin_cal_offset:"
                        "NULL pointer or invalide PA state %d.", pa_state );
    api_status = FALSE;
  }

  if (api_status == TRUE) 
  {
    uint8 chan_idx; 
    boolean channel_found = FALSE;
    
    for ( chan_idx = 0; ( chan_idx <  lin_temp_comp_data->tx_char_chan_size )&& 
                        ( chan_idx < RFCOM_TXLIN_NUM_CHANNELS) ; 
          chan_idx ++ )
    {
      if ( channel == lin_temp_comp_data->tx_char_chan[ chan_idx ] )
      {
        pin_cal_nv->pin[pa_state][chan_idx] = pin_val;

        pin_cal_nv->has_pin_freq_comp_cal_nv[pa_state] = TRUE;

        RF_MSG_3( RF_MED, "Pin_cal_offset stored %d PA state %d chan %d ", 
                          pin_cal_nv->pin[pa_state][chan_idx], pa_state, channel);
        channel_found = TRUE;
        break;
      }      
    }

    if ( channel_found == FALSE )
    {
      RF_MSG_3( RF_ERROR, "ftm_cal_data_store_pin_cal_offset: Channel %d failed to be located "
                          "for PA state %d chan list size %d", 
                channel, pa_state, lin_temp_comp_data->tx_char_chan_size  );
      api_status = FALSE;
    }
  }

  return api_status;

}/* ftm_cal_data_store_pin_cal_offset */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the pin_cal_offset value based on pin_cal NV, channel
  list stored in static Tx Char Chan, PA state and channel passed in. In error
  condition, returns 0. The return value is normalized against the value from
  Ref channel (subtracted ref channel value from curr channel value)
 
  @param ref_chan
  Reference channel for normalization
 
  @param lin_temp_comp_data
  temp comp data which contains Tx Char channel list, which contains
  Pin_cal_vs_freq NV structure
 
  @param pa_state
  PA state
 
  @param channel
  uint32 Channel number
 
  @return pin_offset
  int16 pin_offset value.
*/
int16
ftm_cal_data_get_pin_cal_offset
( 
  uint16 ref_chan,
  const rfcom_txlin_temp_comp_nv_data_type* lin_temp_comp_data,
  uint8 pa_state,
  uint32 channel
)
{
  boolean api_status = TRUE;
  int16 pin_offset = 0;
  int16 ref_chan_pin_offset = 0;

  if ( lin_temp_comp_data == NULL || pa_state >= RFCOM_TXLIN_NUM_LINEARIZERS )
  {
    RF_MSG_1( RF_ERROR, "ftm_cal_data_get_pin_cal_offset:"
                        "NULL pointer or invalide PA state %d.", pa_state );
    api_status = FALSE;
  }

  if (api_status == TRUE) 
  {
    int8 chan_idx; 
    boolean pin_offset_found = FALSE;

    for ( chan_idx = 0; ( chan_idx <  lin_temp_comp_data->tx_char_chan_size )&& 
                        ( chan_idx < RFCOM_TXLIN_NUM_CHANNELS) ; 
          chan_idx ++ )
    {
      if ( channel == lin_temp_comp_data->tx_char_chan[ chan_idx ] )
      {
        pin_offset = lin_temp_comp_data->pin_cal_vs_freq.pin[ pa_state ][ chan_idx ];
        pin_offset_found = TRUE;
        RF_MSG_4( RF_MED, "pin_cal_offset %d found for PA state %d channel %d chan_index %d", 
                  pin_offset ,pa_state, channel, chan_idx );
        break;
      }      
    }

    if ( pin_offset_found == FALSE )
    {
      RF_MSG_3( RF_ERROR, "ftm_cal_data_get_pin_cal_offset: "
                          "Pin Cal offset failed to be located "
                          "for PA state %d channel %d chan list size %d", 
                pa_state, channel, lin_temp_comp_data->tx_char_chan_size  );
      api_status = FALSE;
    }
  }

  if ( api_status == TRUE )
  {    
    int8 chan_idx; 
    boolean ref_chan_found = FALSE;

    for ( chan_idx = 0; ( chan_idx <  lin_temp_comp_data->tx_char_chan_size )&& 
                        ( chan_idx < RFCOM_TXLIN_NUM_CHANNELS) ; 
          chan_idx ++ )
    {
      if ( ref_chan == lin_temp_comp_data->tx_char_chan[ chan_idx ] )
      {
        ref_chan_pin_offset = lin_temp_comp_data->
                                  pin_cal_vs_freq.pin[ pa_state ][ chan_idx ];
        ref_chan_found = TRUE;
        RF_MSG_4( RF_MED, "ref_chan pin_cal_offset %d found "
                          "for PA state %d channel %d chan_index %d", 
                  pin_offset ,pa_state, ref_chan, chan_idx );
        break;
      }      
    }

    if ( ref_chan_found == FALSE )
    {
      RF_MSG_3( RF_ERROR, "ftm_cal_data_get_pin_cal_offset: Ref Chan "
                          "Pin Cal offset failed to be located "
                          "for PA state %d channel %d chan list size %d", 
                pa_state, ref_chan, lin_temp_comp_data->tx_char_chan_size  );
    }
  }

  pin_offset -= ref_chan_pin_offset;

  RF_MSG_1( RF_MED, "ftm_cal_data_get_pin_cal_offset: final pin_cal_offset %d", 
            pin_offset );

  return pin_offset;
}/* ftm_cal_data_get_pin_cal_offset */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get data required for Pin alignment in Tx linearizer format

  @details
  This API uses the current data maintained within the data module to apply 
  Pin alignment during calibration.
 
  @param ref_chan
  The reference channel information for applying Pin alignment

  @param in_override_row
  Current row on which Pin alignment must be applied

  @param out_override_row
  Row to store data after Pin alignment

  @param pcomp_val
  Pin compensation value

  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean
ftm_cal_data_apply_pin_alignment
(
  uint32 ref_chan,
  int8 pa_state,
  ftm_cal_data_dpd_map_row_type *in_override_row,
  ftm_cal_data_dpd_map_row_type *out_override_row,
  int16 pcomp_val
)
{
  /* Holder for RGI delta table information */
  ftm_cal_data_rgid_type rgi_delta_table;
  uint8 valid_first_lut_entries, i;
  uint8 rgid_lut_index = 0;
  uint8 lut_beg;
  int16 new_power = 0;
  uint8 iq_offset;

  /* Null pointer check */
  if( in_override_row == NULL || out_override_row == NULL )
  {
    RF_MSG( RF_ERROR, "ftm_cal_data_apply_pin_alignment: Null input and "
                      "output data structures");
    return FALSE;
  }

  /* Copy over input data to output data for retaining peripheral row 
  values and if required in case of early return */
  memscpy( (void*)out_override_row,
           sizeof(ftm_cal_data_dpd_map_row_type),
          (void*)in_override_row, 
          sizeof(ftm_cal_data_dpd_map_row_type) );
  
  /* Get the RGI delta table from the calibration data module */
  ftm_cal_data_get_rgid_table( ref_chan, pa_state, &rgi_delta_table );

  /* Find the number of valid entries in first sweep */
  for ( valid_first_lut_entries = 0; 
        valid_first_lut_entries < RFCOM_TXLIN_NUM_SEGMENTS; 
        valid_first_lut_entries++ )
  {  
    if( rgi_delta_table.table[valid_first_lut_entries].rgi == 0xFFFF )
    {
      break;
    }
  }

  if (valid_first_lut_entries == 0)
  {
    RF_MSG( RF_ERROR, "ftm_cal_data_apply_pin_alignment: "
                      "No valid RGI delta sweep linearizer entries");
    return TRUE;
  }

  /* For the input RGI, find the corresponding index in the RGI delta table 
  for the same RGI */
  for( i = 0; i < valid_first_lut_entries; i++ )
  {
    /* Find FS index */
    if( rgi_delta_table.table[i].rgi == in_override_row->curr_tx_override.rgi )
    {
      rgid_lut_index = i;
      break;
    }
  }

  if( i == valid_first_lut_entries )
  {
    RF_MSG_1( RF_ERROR, "ftm_cal_data_apply_pin_alignment: "
                        "Could not find RGI %d in first sweep. "
                        "Pin compensation failed",
              in_override_row->curr_tx_override.rgi );
    return TRUE;
  }

  /* Update the pout in the table to reflect correct estimated Pout */
  out_override_row->power = in_override_row->power;

  /* calculating the new power with temp comp val */
  new_power = rgi_delta_table.table[rgid_lut_index].power + (-1)*pcomp_val;

  /* Only RGI will be replaced in the new second sweep LUT */
  if( new_power >= rgi_delta_table.table[valid_first_lut_entries-1].power )
  {
    /* RGI should be coming from first sweep */
    out_override_row->curr_tx_override.rgi = 
                        rgi_delta_table.table[valid_first_lut_entries-1].rgi;
    /* Set IQ offset to zero, no backoff required */
    iq_offset = 0;

  } /* if(new_power >=  power) */
  else if ( new_power <= rgi_delta_table.table[0].power )
  {
    /* RGI from first sweep */
    out_override_row->curr_tx_override.rgi = rgi_delta_table.table[0].rgi;
    /* Calc IQ based on diff */
    iq_offset = rgi_delta_table.table[0].power - new_power;
    iq_offset = (iq_offset > RFCOM_TXLIN_PIN_COMP_IQ_GAIN_MAX_ADJUST) ? 
                      RFCOM_TXLIN_PIN_COMP_IQ_GAIN_MAX_ADJUST : iq_offset;

  }/* if(new_power <=  power) */
  else
  {
    /* Linear search implementation
       Start from rgid_lut_index instead of lut_index as we need to 
       start from matching RGI. From here, we can go up or down based on 
       compensation val */
    lut_beg = rgid_lut_index;
    while( (lut_beg <= (valid_first_lut_entries-1)) && 
           (lut_beg < (RFCOM_TXLIN_NUM_SEGMENTS-1)) &&
           (pcomp_val != 0) )
    {
      if( ( lut_beg != 0 ) && 
          ( rgi_delta_table.table[lut_beg].power >= new_power ) &&
          ( rgi_delta_table.table[lut_beg-1].power < new_power ) )
      {
        break;
      }
      else
      {
        /* if temp comp is negative, search for lower indices,
           if temp comp is positive , search for higher indices */
        (pcomp_val < 0) ? (lut_beg++) : (lut_beg--);
      }
    } /* while() */

    /* Here we find the exact RGI for the new power */
    out_override_row->curr_tx_override.rgi = rgi_delta_table.table[lut_beg].rgi;
    /* storing the difference as IQ offset */
    iq_offset = rgi_delta_table.table[lut_beg].power - new_power;
    RF_MSG_1( RF_HIGH, "ftm_cal_data_apply_pin_alignment: IQ backoff %d ",
              iq_offset );
    iq_offset = (iq_offset > RFCOM_TXLIN_PIN_COMP_IQ_GAIN_MAX_ADJUST) ? 
                      RFCOM_TXLIN_PIN_COMP_IQ_GAIN_MAX_ADJUST : iq_offset;
  } /* else */

  /* RGI and power updates are complete. Based on the IQ backoff computed, 
  update the IQ gain value in the output override row */
  out_override_row->curr_tx_override.iq_gain =
    (uint32)( (in_override_row->curr_tx_override.iq_gain * 
              rfcommon_core_txlin_compute_iq_gain_scaling( iq_offset*(-1) ))
              >> 15 );

  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Push Pin Result into Pin Table

  @details
  This function will take the current Pin IQ capture params to store in 
  Pin Map. On processing completion, corresponding row is updated with
  SW processed Pin value. The counter is incremented for a new channel. If
  any channel previously existed, data for it will be cleared to store new
  value.
  
  @param chan
  Channel for which Pin is computed.
 
  @param cap_buf_idx
  This will keep track of shared memory buffer where corresponding IQ capture
  data exists.
 
  @param res_buf_idx
  This will keep track of shared memory buffer where corresponding Pin 
  processing result exists.

  @return
  TRUE is set successfully; FALSE otherwise
*/

boolean ftm_cal_data_push_pin_data_to_pin_map(
                           uint32 chan,
                           uint8 cap_buf_idx,
                           uint8 res_buf_idx)
{
  uint8 i;
  boolean row_found, status;
#if 0  
  ftm_cal_data_pin_map_type *pin_map;

  status = TRUE;
  row_found = FALSE;
  pin_map = ftm_common_data_get_pin_table_ptr();

  /* Find row from table matching channel*/
  for ( i = 0; i < pin_map->num_map_entries; i++ )
  {
    if (pin_map->table[i].chan == chan)
    {
      /* invalidate any previous data */
      pin_map->table[i].cap_buf_idx = cap_buf_idx;
      pin_map->table[i].res_buf_idx = res_buf_idx;
      pin_map->table[i].status = FTM_COMMON_XPT_CUR_ORIDE_LOADED;
      pin_map->table[i].pin_compensation = 0;
      row_found = TRUE;
      status &= TRUE;
      RF_MSG_2(RF_HIGH,"ftm_cal_data_push_pin_data_to_pin_map: row %d exists with chan %d", i, chan);
      break;
    }
  }

  /* if entry didn't exist create new row */
  if (row_found == FALSE)
  {
    /* Enter new row data only if space exist */
    if ( pin_map->num_map_entries < FTM_COMMON_XPT_NUM_PIN_MAP_ROWS )
    {
      RF_MSG_2(RF_HIGH,"ftm_cal_data_push_pin_data_to_pin_map: new row %d with chan %d", pin_map->num_map_entries, chan);
      pin_map->table[pin_map->num_map_entries].chan = chan;
      pin_map->table[i].cap_buf_idx = cap_buf_idx;
      pin_map->table[i].res_buf_idx = res_buf_idx;
      pin_map->table[i].status = FTM_COMMON_XPT_CUR_ORIDE_LOADED;
      pin_map->table[i].pin_compensation = 0;
      pin_map->num_map_entries++;
      status &= TRUE;
    }
    else
    {
      RF_MSG_2(RF_ERROR, "ftm_cal_data_push_pin_data_to_pin_map: Row Index %d exceeds Total allowed %d",
                         pin_map->num_map_entries, FTM_COMMON_XPT_NUM_PIN_MAP_ROWS);
      status &= FALSE;
    }
  }
  #endif
  return status;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Push Current Override

  @details
  This function will take the current overrides that were populated in the
  local structure and push them onto the DPD map. The DPD map keeps a counter
  of how many items are currently in the map, so that gets incremented once
  this value is added. This is maintained so insertion can happen with
  out needing to search. 

  @param cap_buf_idx
  This will be needed when capture is complete to set the current status
 
  @param res_buf_idx
  This will be needed when processing is complete to get the results from SM
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_push_cur_override_to_dpd_map(
                                      uint32 chan,
                                      uint8 cap_buf_idx,
                                      uint8 res_buf_idx)
{
  uint8 cur_index;
  ftm_cal_data_dpd_map_type *cur_map;
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(chan != module_ptr->dpd_map_cur_chan)
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_push_cur_override_to_dpd_map: " 
                         "Accessing DPD map with invalid chan %d", chan);
      return FALSE;
    }
  }
  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_push_cur_override_to_dpd_map:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return FALSE;
  }

  /* If override row is invalid, use next valid entry */
  if(module_ptr->current_override_row == FTM_COMMON_XPT_INV_OVR_ROW)
  {
    cur_index = module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->num_map_entries;
  }
  else
  {
    cur_index = module_ptr->current_override_row;
  }

  cur_map = module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map;

  /* Copy override params */
  memscpy((void*)&cur_map->table[cur_index].curr_tx_override,
          sizeof(rfcommon_core_xpt_tx_override_type),
         (void*)&module_ptr->curr_tx_override, 
         sizeof(rfcommon_core_xpt_tx_override_type));

  /* Set status to CUR_ORIDE_LOADED */
  cur_map->table[cur_index].status = FTM_COMMON_XPT_CUR_ORIDE_LOADED;

  /* Set capture and results buffer index */
  cur_map->table[cur_index].cap_buf_idx = cap_buf_idx;
  cur_map->table[cur_index].res_buf_idx = res_buf_idx;

  /* Increment size so we point to the next available table entry for the next write */
  /* This only happens if we inserted a new item as instructed by the current 
     override status */
  if(module_ptr->current_override_row == FTM_COMMON_XPT_INV_OVR_ROW)
  {
    module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->num_map_entries++;
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set Pin IQ Capture Complete

  @details
  Update the state machine of the row index of Pin table associated with 
  the Pin IQ capture buffer index to say that capture was successful 
  for this entry. 
 
  @param chan
  Channel to look up in delay table

  @param cap_buf_id
  This will keep track of shared memory buffer where corresponding IQ capture
  data exists.

  @return
  TRUE is set successfully; FALSE otherwise
*/

boolean ftm_cal_data_set_pin_capture_complete ( uint32 chan, uint8 cap_buf_id )
{
  boolean status;
#if 0
  ftm_cal_data_pin_map_type *pin_map;
  uint8 i;

  status = FALSE;
  pin_map = ftm_common_data_get_pin_table_ptr();

  /* search delay table by chan, cap_buf_id and capt_type */
  for (i = 0; i < pin_map->num_map_entries; i++)
  {
    /* if row found set status to capture complete */
    if ( ( pin_map->table[i].chan == chan ) && 
         ( pin_map->table[i].cap_buf_idx == cap_buf_id ))
    {
      pin_map->table[i].status = FTM_COMMON_XPT_IQ_CAPTURE_COMPLETE;
      status = TRUE;
      RF_MSG_3(RF_HIGH,"ftm_cal_data_set_pin_capture_complete: found entry %d to cap complete - chan %d"
                       " cap_buf %d",
                        i,chan,cap_buf_id);
      break;
    }
  }
  /* if not found return error */
  if (status == FALSE)
  {
      RF_MSG_2(RF_ERROR,"ftm_cal_data_set_pin_capture_complete: Could not find"
                        " chan %d cap_id %d to set complete",
                        chan, cap_buf_id);
  }
#endif
  return status;

}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set Capture Complete

  @details
  Update the state machine of the row index associated with the input capture
  buffer index to say that capture was successful for this entry. 
 
  @param cap_buf_idx
  Tells us which row is the one we want to update. 
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_set_capture_complete(uint32 chan, uint8 cap_buf_id)
{
  uint8 cap_idx;
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(chan != module_ptr->dpd_map_cur_chan)
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_set_capture_complete: " 
                         "Accessing DPD map with invalid chan %d", chan);
      return FALSE;
    }
  }
  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_set_capture_complete:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return FALSE;
  }

  /* update status to capture complete */
  cap_idx = ftm_cal_data_search_cap_id(cap_buf_id);

  if(cap_idx == 0xFF)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_set_capture_complete:"
                       " Could not find capture entry %d",cap_buf_id);
    return FALSE;
  }

  module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[cap_idx].status = FTM_COMMON_XPT_IQ_CAPTURE_COMPLETE;

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set Pin Processing Complete

  @details
  Update the state machine of the pin table row index associated with 
  the pin processing results to indicate processing status for this entry. 
 
  @param res_buf_idx
  Tells us which row is the one we want to update. 

  @return
  TRUE is set successfully; FALSE otherwise
*/

boolean ftm_cal_data_set_pin_proc_complete( uint32 chan, uint8 res_buf_id )
{
  boolean status;
#if 0
  ftm_cal_data_pin_map_type *pin_map;
  uint16 i,j;
  fw_dpd_cal_result_t *cur_results;

  status = FALSE;
  pin_map = ftm_common_data_get_pin_table_ptr();

  /* search delay table by chan, res_buf_id and capt_type */
  for (i = 0; i < pin_map->num_map_entries; i++)
  {
    /* if row found set status to capture complete */
    if ( ( pin_map->table[i].chan == chan ) && 
         ( pin_map->table[i].res_buf_idx == res_buf_id ))
    {
      /* get results from MDSP */
      cur_results = rfcommon_mdsp_get_xpt_dpd_data(res_buf_id );

      if(cur_results == NULL)
      {
        RF_MSG(RF_ERROR, "ftm_cal_data_set_proc_complete: get DPD results failure");
        return FALSE;
       }

       /* Copy results into the pin map */
       RF_MSG_1(RF_MED, "***Chan %d", chan );
       for(j=0;j<XPT_CAL_MAX_AMAM_SIZE;j++)
       {
          pin_map->table[i].vout[j] = (float)cur_results->am_dist[j];
          pin_map->table[i].phase_out[j] = (float)cur_results->pm_dist[j];

          RF_MSG_4(RF_MED, "AMAM[%d] = %d, AMPM[%d] = %d", j, (int32)pin_map->table[i].vout[j] , j, (int32)pin_map->table[i].phase_out[j]);
       }
      pin_map->table[i].status = FTM_COMMON_XPT_IQ_PROCESSING_COMPLETE;
      status = TRUE;

      RF_MSG_3(RF_HIGH,"ftm_cal_data_set_pin_proc_complete: found entry %d to proc complete"
                       " - chan %d cap_buf %d",
                        i,chan,res_buf_id);
      break;
    }
  }

  /* if not found, return error */
  if (status == FALSE)
  {
    RF_MSG_2(RF_ERROR, "ftm_cal_data_set_pin_proc_complete:"
                       " Could not find chan %d res_id %d",
                       chan,
                       res_buf_id);
  }
#endif
  return status;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set Processing Complete

  @details
  Update the state machine of the row index associated with the input results
  buffer index to say that processing was successful for this entry. 
 
  @param res_buf_idx
  Tells us which row is the one we want to update. 
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_set_proc_complete(uint32 chan, uint8 res_buf_id)
{
  uint8 res_idx;
  fw_dpd_cal_result_t *cur_results;
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(chan != module_ptr->dpd_map_cur_chan)
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_set_proc_complete: " 
                         "Accessing DPD map with invalid chan %d", chan);
      return FALSE;
    }
  }
  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_set_proc_complete:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return FALSE;
  }

  /* update status to process complete */
  res_idx = ftm_cal_data_search_res_id(res_buf_id);

  if(res_idx == 0xFF)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_set_capture_complete:"
                       " Could not find result entry %d",res_idx);
    return FALSE;
  }

  module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[res_idx].status = FTM_COMMON_XPT_IQ_PROCESSING_COMPLETE;

  /* get results from MDSP */
//  cur_results = rfcommon_mdsp_get_xpt_dpd_data(res_buf_id);

  if(cur_results == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_set_proc_complete: get DPD results failure");
    return FALSE;
  }

  RF_MSG(RF_HIGH,"Computing compression in proc done handler");

  /* Copy results into the data module */
  memscpy((void*)&module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_table->table[res_idx],
          sizeof(fw_dpd_cal_result_t),
         (void*)cur_results,
         sizeof(fw_dpd_cal_result_t));

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Store Nominal EPDT table

  @details
  This will store the nominal EPDT table. This is needed for later to scale.
 
  @param nom_det_table_ptr
  The nominal EPDT table to store. 
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_store_nom_epdt_table(
                                    ftm_cal_data_det_type *nom_det_table_ptr)
{
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(nom_det_table_ptr == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_store_nom_epdt_table: Null input");
    return FALSE;
  }

  memscpy((void*)&module_ptr->data_module_nom_det,
         sizeof(ftm_cal_data_det_type),
         (void*)nom_det_table_ptr,
         sizeof(ftm_cal_data_det_type));

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Nominal EPDT table

  @details
  This will return the nominal EPDT table. 
 
  @param nom_det_table_ptr
  The nominal EPDT table to return. 
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_get_nom_epdt_table(ftm_cal_data_det_type* nom_det_ptr)
{
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(nom_det_ptr == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_get_nom_epdt_table: Null input");
    return FALSE;
  }

  memscpy((void*)nom_det_ptr,
          sizeof(ftm_cal_data_det_type),
         (void*)&module_ptr->data_module_nom_det,
         sizeof(ftm_cal_data_det_type));

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Unity DPD HWIDs

  @details
  This will return the unity HWIDs. Needs to be stored between the first and
  the second sweep so that m-line can push the location of the unity tables
  into the DPD map so that override by power will have the necessary info. 
 
  @param ftm_cal_data_unity_hwid_type
  The unity HWIDs to get. 
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_get_unity_hwids(
                                    ftm_cal_data_unity_hwid_type* unity_hwids)
{
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(unity_hwids == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_get_unity_hwids: Null input");
    return FALSE;
  }

  memscpy((void*)unity_hwids,
          sizeof(ftm_cal_data_unity_hwid_type),
         (void*)&module_ptr->unity_hwid,
         sizeof(ftm_cal_data_unity_hwid_type));

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Store Unity DPD HWIDs

  @details
  This will store the unity HWIDs. Needs to be stored between the first and
  the second sweep so that m-line can push the location of the unity tables
  into the DPD map so that override by power will have the necessary info. 
 
  @param ftm_cal_data_unity_hwid_type
  The unity HWIDs to store. 
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_set_unity_hwids(
                                    ftm_cal_data_unity_hwid_type* unity_hwids)
{
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(unity_hwids == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_set_unity_hwids: Null input");
    return FALSE;
  }

  memscpy((void*)&module_ptr->unity_hwid,
         sizeof(ftm_cal_data_unity_hwid_type),
         (void*)unity_hwids,
         sizeof(ftm_cal_data_unity_hwid_type));

  return status;
}


/*!
  @brief
  Push RGI Delta Table

  @details
  This will store the RGI delta table in the the data module. This is needed
  for m-line computation.
 
  @param rgid_table
  The RGI delta table to store. 
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_push_rgid_data(uint32 chan,
                                    ftm_cal_data_rgid_type *rgid_table)
{
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(rgid_table == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_get_rgid_table: Null input");
    return FALSE;
  }
  if(chan != module_ptr->dpd_map_cur_chan)
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_push_rgid_data: " 
                         "Accessing DPD map with invalid chan %d", chan);
      return FALSE;
    }
  }
  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_set_proc_complete:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return FALSE;
  }

  module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].mline.multiple_mline = FALSE;
  module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].mline.pa_state_one = 0xFF;
  module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].mline.pa_state_two = 0xFF;

  memscpy((void*)module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].rgi_delta_table,
         sizeof(ftm_cal_data_rgid_type),
         (void*)rgid_table,
         sizeof(ftm_cal_data_rgid_type));

  return status;
}

/*----------------------------------------------------------------------------*/

/*!
  @brief
  Push RGI Delta Table according with the PA state and XPT mode

  @details
  This will store the RGI delta table in the the data module. This is needed
  for multiple m-line computation.
 
  @param rgid_table
  The RGI delta table to store. 
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_push_multiple_rgid_data(uint32 chan,
                                    uint8 pastate,
                                    uint8 xptMode,
                                    ftm_cal_data_rgid_type *rgid_table)
{
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(rgid_table == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_get_rgid_table: Null input");
    return FALSE;
  }


  if(chan != module_ptr->dpd_map_cur_chan)
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_push_rgid_data: " 
                         "Accessing DPD map with invalid chan %d", chan);
      return FALSE;
    }
  }

  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_set_proc_complete:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return FALSE;
  }

  module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].mline.multiple_mline = TRUE;

  module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].mline.num_pa_states++;
  
  if (module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].mline.num_pa_states == 1) 
  {
    module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].mline.pa_state_one = pastate;

  memscpy((void*)module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].rgi_delta_table,
         sizeof(ftm_cal_data_rgid_type),
         (void*)rgid_table,
         sizeof(ftm_cal_data_rgid_type));

  }
  else if (module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].mline.num_pa_states == 2)
  {
    module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].mline.pa_state_two = pastate;

    memscpy((void*)module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].rgi_delta_table_two,
         sizeof(ftm_cal_data_rgid_type),
         (void*)rgid_table,
         sizeof(ftm_cal_data_rgid_type));
  }
  
  
  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get RGI Delta table

  @details
  This will return the RGI delta table. 
 
  @param rgid_table
  The place to store the requested RGI delta table. 
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_get_rgid_table(uint32 chan,
                                    int8 pa_state,
                                    ftm_cal_data_rgid_type *rgid_table)
{
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(rgid_table == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_get_rgid_table: Null input");
    return FALSE;
  }
  if(chan != module_ptr->dpd_map_cur_chan)
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_get_rgid_table: " 
                         "Accessing DPD map with invalid chan %d", chan);
      return FALSE;
    }
  }
  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_get_rgid_table:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return FALSE;
  }

  if (!module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].mline.multiple_mline) 
  {

    memscpy((void*)rgid_table,
          sizeof(ftm_cal_data_rgid_type),
         (void*)module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].rgi_delta_table,
         sizeof(ftm_cal_data_rgid_type));
  }
  else
  {
    if (module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].mline.pa_state_one == (uint8) pa_state) 
    {
  memscpy((void*)rgid_table,
          sizeof(ftm_cal_data_rgid_type),
         (void*)module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].rgi_delta_table,
         sizeof(ftm_cal_data_rgid_type));

      RF_MSG_1(RF_HIGH, " Accessing RGI Table One %d",
                       module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].mline.pa_state_one);
    }
    else if (module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].mline.pa_state_two == (uint8) pa_state) 
    {
       memscpy((void*)rgid_table,
          sizeof(ftm_cal_data_rgid_type),
         (void*)module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].rgi_delta_table_two,
         sizeof(ftm_cal_data_rgid_type));

       RF_MSG_1(RF_HIGH, " Accessing RGI Table Two %d",
                       module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].mline.pa_state_two);
    }
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set EPDT HWID

  @details
  This will save the DPD HWID locations to the entry associated with a given
  power. This is because mutliple scaled detroughs need to be loaded into the
  phone at once and then selected during Tx override. 
 
  @param power
  Power entry for the row we want to commit to.
 
  @param epdt_hwid
  HWID that tells us where in the DPD tables our EPDT table is. 
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_set_epdt_hwid(uint32 chan,
                                   uint16 power, 
                                   uint8 xpt_mode,
                                   rfcommon_mdsp_dpd_table_info_t epdt_hwid)
{
  uint8 row_idx;
  ftm_cal_data_dpd_map_row_type *cur_row;
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(chan != module_ptr->dpd_map_cur_chan)
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_set_epdt_hwid: " 
                         "Accessing DPD map with invalid chan %d", chan);
      return FALSE;
    }
  }
  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_set_epdt_hwid:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return FALSE;
  }

  row_idx = ftm_cal_data_search_power(power,xpt_mode);


  if( row_idx < FTM_COMMON_XPT_NUM_DPD_MAP_ROWS )
  	{
  		cur_row = &(module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[row_idx]);
  	}
  else
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_set_epdt_hwid: Could not find power entry %d",power);
    return FALSE;
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get DPD results

  @details
  This will return the DPD results buffer for a given row entry based on power. 
 
  @param result
  A result buffer based on power.
 
  @param power
  The power used to find the row entry we want the results for.
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_get_dpd_result(uint32 chan,
                                    uint16 power,
                                    uint8 xpt_mode,
                                    void* result)
{
  uint8 row_idx;
  uint8 res_buf_id;
  fw_dpd_cal_result_t* cur_results;
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(chan != module_ptr->dpd_map_cur_chan)
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_get_dpd_result: " 
                         "Accessing DPD map with invalid chan %d", chan);
      return FALSE;
    }
  }
  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_get_dpd_result:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return FALSE;
  }
  if(result == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_get_dpd_result: Null Input");
    return FALSE;
  }

  row_idx = ftm_cal_data_search_power(power,xpt_mode);

  if(row_idx >= FTM_COMMON_XPT_NUM_DPD_MAP_ROWS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_get_dpd_result: Could not find power entry %d",power);
    return FALSE;
  }

  res_buf_id = module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[row_idx].res_buf_idx;

  /* get results from MDSP */
//  cur_results = rfcommon_mdsp_get_xpt_dpd_data(res_buf_id);

  if(cur_results == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_set_proc_complete: get DPD results failure");
    return FALSE;
  }

  /* Copy results into the data module */
  memscpy((void*)result,
          sizeof(fw_dpd_cal_result_t),
         (void*)cur_results,
         sizeof(fw_dpd_cal_result_t));

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set HWIDs

  @details
  This will save the DPD HWID locations to the entry associated with a given
  power. This is needed in sweep three when all of the DPD tables will be
  programmed and tx override needs to select the correct DPD table locations.
  With these stored we can select a whole row entry and get all the valid
  data and DPD locations at once. 
 
  @param power
  Power entry for the row we want to commit to.
 
  @param amam_hwid
  HWID that tells us where in the DPD tables our AMAM table is.
 
  @param ampm_hwid
  HWID that tells us where in the DPD tables our AMPM table is.
 
  @param epdt_hwid
  HWID that tells us where in the DPD tables our EPDT table is. 
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_set_hwid_by_power(uint32 chan,
                                       uint16 power, 
                                       uint8 xpt_mode,
                                       rfcommon_mdsp_dpd_table_info_t amam_hwid,
                                       rfcommon_mdsp_dpd_table_info_t ampm_hwid,
                                       rfcommon_mdsp_dpd_table_info_t epdt_hwid)
{
  uint8 row_idx;
  ftm_cal_data_dpd_map_row_type *cur_row;
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(chan != module_ptr->dpd_map_cur_chan)
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_set_hwid_by_power: " 
                         "Accessing DPD map with invalid chan %d", chan);
      return FALSE;
    }
  }
  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_set_hwid_by_power:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return FALSE;
  }

  row_idx = ftm_cal_data_search_power(power,xpt_mode);

  if( row_idx<FTM_COMMON_XPT_NUM_DPD_MAP_ROWS )
  	{
  		cur_row = &(module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[row_idx]);
  	}
  else
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_set_hwid_by_power: Could not find power entry %d",power);
    return FALSE;
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Retrieve ET Pin value and send back to PC

  @details
  This API will calculate pin back off value with respect to
  reference channel by using the compression point specified
  in the argument.
 
  @param chan
  Channel for which to search Pin
 
  @param compression_point
  Compression point used for calculating pin compensation. The
  resolution of the argument is db*10. For example, if we want
  3.5db compression, this value will be set to 35.
 
  @param ref_chan
  Ref channel used for pin normalization
 
  @param *pin_cal_status
  Flag to indicate if Pin calculation is successful
 
  @return
  The Pin value from the result buffer
*/
int32 ftm_cal_data_get_pin_from_table(uint32 chan, 
                                      uint8 compression_point,
                                      uint32 ref_chan,
                                      uint8 *peak_compression,
                                      boolean *pin_cal_status,
                                      boolean calculate_ampm_scale_factor,
                                      int32 *ampm_scale_factor,
                                      boolean *ampm_scale_status)
{
  int32 pin_value = 0;
#if 0
  ftm_cal_data_pin_map_type *pin_map;
  uint16 i,j;
  int32 pin_value = 0;
  int32 pin_ref_value = 0;
  boolean row_found = FALSE;
  uint8 foundCompPt = 0;
  uint8 ndb_point = 0;
  uint8 max_compression = 0;
  boolean ref_chan_valid = FALSE;
  float regression_a,regression_b;
  float bias = 0.0,expandFactor;
  rf_time_tick_type prof_cal_func; /* Function Profiling Variable */
  rf_time_type prof_cal_func_val = 0; /* Function Profiling Variable */
  float *amam_vin_expanded;
  float *phase_out_interpolate;
  uint16 discard, discardInt;
  float numerator = 0;
  float denumerator = 0;
  prof_cal_func = rf_time_get_tick(); /* Get Time tick for Function Profile */

  pin_map = ftm_common_data_get_pin_table_ptr();
  *peak_compression = 0;

  RF_MSG_3(RF_HIGH, "ftm_cal_data_get_pin_from_table: ref channel is %d, current channel is %d, calculate AMPM scaling %d", ref_chan, chan, calculate_ampm_scale_factor);

  if (ref_chan == chan)
  {
      *ampm_scale_factor = 1000;
      *ampm_scale_status = TRUE;
  }
  /* Find regression coefficient for ref channel and save them */
  if (!ampm_regression_done_for_ref_channel  && calculate_ampm_scale_factor)
  {
    for (i = 0; i < pin_map->num_map_entries; i++)
    {
      if (pin_map->table[i].chan == ref_chan)
      {
        row_found = TRUE;
        ref_chan_row_index = i;

        if (pin_map->table[i].status == FTM_COMMON_XPT_IQ_PROCESSING_COMPLETE)
        {
          ftm_common_xpt_find_max_gain_ndb_point(amam_vin,
                                                 pin_map->table[i].vout,
                                                 XPT_CAL_MAX_AMAM_SIZE,
                                                 (float)compression_point / 10,
                                                 &max_compression,
                                                 &ndb_point,
                                                 &foundCompPt,
                                                 0);


          if (foundCompPt == 1)
          {
            ref_chan_compression_point = ndb_point;
            ampm_regression_done_for_ref_channel = TRUE;
            *ampm_scale_status = TRUE;
            RF_MSG_2(RF_ERROR, "AMPM for reference channel %d: compression point %d, bias index = %d ", ref_chan, ref_chan_compression_point );

          }
          else
          {
            *ampm_scale_status = FALSE;
            RF_MSG_2(RF_HIGH, "ftm_cal_data_get_pin_from_table: ampm scaling calcuation failure for ref channel, Pin at row %d has status %d - no compression point was found",
                     i, pin_map->table[i].status);
          }

        }
        else
        {
        /*IQ capture is not complete*/
        pin_map->table[i].status = FTM_COMMON_XPT_PIN_CALCULATION_FAILED;
        *pin_cal_status = FALSE;
          RF_MSG_2(RF_HIGH, "ftm_cal_data_get_pin_from_table: ampm scaling calcuation failure for ref channel Pin at row %d has status %d - proc not complete",
                   i, pin_map->table[i].status);
       }
        break;
      }

    }
      if (row_found == FALSE)
      {
        *ampm_scale_status = FALSE;
        RF_MSG_1(RF_ERROR,"ftm_cal_data_get_pin_from_table: ampm scaling calcuation failure, can't find reference channel %d in Pin map table", ref_chan);
      }

  }

  /* Find Pin and AMPM regression coefficient for current channel and calculate AMPM scaling factor */
  for (i = 0; i < pin_map->num_map_entries; i++)
  {
    if ( pin_map->table[i].chan == chan )
    {
      row_found = TRUE;
      if (pin_map->table[i].status == FTM_COMMON_XPT_IQ_PROCESSING_COMPLETE)
      {
        /*Calculate xdb compression pt and normalize pin comp against ref chan*/
        ftm_common_xpt_find_max_gain_ndb_point( amam_vin,
                                                pin_map->table[i].vout,
                                                XPT_CAL_MAX_AMAM_SIZE,
                                                (float)compression_point/10,
                                                &max_compression,
                                                &ndb_point,
                                                &foundCompPt,
                                                0);
        *peak_compression = max_compression;
        if(foundCompPt == 1)
        {
          pin_map->table[i].pin_compensation = (int32)200*log10((float)ndb_point/XPT_CAL_MAX_AMAM_SIZE);
          pin_map->table[i].pin_compensation = pin_map->table[i].pin_compensation;
          pin_value = pin_map->table[i].pin_compensation;
          pin_map->table[i].status = FTM_COMMON_XPT_PIN_CALCULATION_COMPLETE;
          *pin_cal_status = TRUE;
          RF_MSG_3(RF_HIGH,"ftm_cal_data_get_pin_from_table: found row %d with chan %d and Pin value %d", i, chan, pin_value);
          if (  (calculate_ampm_scale_factor ) && (ref_chan != chan))
          {
            if ( ampm_regression_done_for_ref_channel == TRUE  )
            {
             expandFactor = (float) (((float)ndb_point)/((float)ref_chan_compression_point));
             RF_MSG_3(RF_ERROR, "AMPM for channel %d: compression point %d, expandFactor %d", chan, ndb_point, (int32)(expandFactor*1000)) ;

             amam_vin_expanded = (float *)modem_mem_alloc((XPT_CAL_MAX_AMAM_SIZE * sizeof(float)), MODEM_MEM_CLIENT_RFA); 
             phase_out_interpolate = (float *)modem_mem_alloc((XPT_CAL_MAX_AMAM_SIZE * sizeof(float)), MODEM_MEM_CLIENT_RFA); 

             if ( ( amam_vin_expanded != NULL ) &&  ( phase_out_interpolate != NULL )  )
             {

               for (j = 0 ; j < XPT_CAL_MAX_AMAM_SIZE ; j++ )
               {
                 amam_vin_expanded[j] = amam_vin[j]*expandFactor;
               }

               interp1(amam_vin , XPT_CAL_MAX_AMAM_SIZE , pin_map->table[i].phase_out , amam_vin_expanded , XPT_CAL_MAX_AMAM_SIZE , phase_out_interpolate);

               discardInt = (uint32) ceil( XPT_CAL_AMPM_DISCARD_SAMPLES * expandFactor );


               if (discardInt > XPT_CAL_AMPM_DISCARD_SAMPLES )
               {
                 discard = discardInt;
               }else
               {
                 discard = XPT_CAL_AMPM_DISCARD_SAMPLES;
               }

   

               RF_MSG_4(RF_ERROR, "AMPM for channel %d: discardInt %d, discardInt %d, bias %d", chan, discardInt, discard, (int32) bias  ) ;


               calcLinearFactors(&pin_map->table[ref_chan_row_index].phase_out[discard], &phase_out_interpolate[discard], &regression_a, &regression_b, ref_chan_compression_point - discard);


               *ampm_scale_factor = (int32)(1000*regression_a) ;

                 RF_MSG_1(RF_ERROR, "AMPM scale factor = %d", *ampm_scale_factor );
                 *ampm_scale_status = TRUE;
             }
             else
             {
               *ampm_scale_status = FALSE;
               RF_MSG_2(RF_ERROR, "Memory allocation failure amam_vin_expanded = 0x%x ,  phase_out_interpolate = 0x%x ", amam_vin_expanded , phase_out_interpolate );
             }

            modem_mem_free((void *)amam_vin_expanded, MODEM_MEM_CLIENT_RFA);
            modem_mem_free((void *)phase_out_interpolate, MODEM_MEM_CLIENT_RFA);

            }
            else
            {
               *ampm_scale_status = FALSE;
               RF_MSG_1(RF_ERROR, "AMPM regression for channel %d: no regression data on reference channel ", chan);
            }

          }
        }
        else
        {
          /*Failed to compute pin backoff and use max Pin value*/
          pin_map->table[i].pin_compensation = 0;
          pin_map->table[i].status = FTM_COMMON_XPT_PIN_CALCULATION_COMPLETE;
          *pin_cal_status = TRUE;
        }
      }
      else
      {
        /*IQ capture is not complete*/
        pin_map->table[i].status = FTM_COMMON_XPT_PIN_CALCULATION_FAILED;
        *pin_cal_status = FALSE;
        RF_MSG_2(RF_HIGH,"ftm_cal_data_get_pin_from_table: Pin at row %d has status %d - proc not complete",
                 i,pin_map->table[i].status);
      }
      break;
    }
  }
  if (row_found == FALSE)
  {
    *pin_cal_status = FALSE;
    RF_MSG_1(RF_ERROR,"ftm_cal_data_get_pin_from_table: could not find row with chan %d ", chan);
  }

  /* Get time taken for this function to execute */
  prof_cal_func_val = rf_time_get_elapsed( prof_cal_func, RF_USEC );

  RF_MSG_1( RF_ERROR, "ftm_cal_data_get_pin_from_table() "
                        "execution time %d",prof_cal_func_val);
#endif  
  return pin_value;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Row Data

  @details
  This will return a row from the data module associated with the desired PA
  state, RGI, and bias passed in. 
 
  @param row
  A row from the data module.
 
  @param pa
  The pa state used to find the row entry we want the results for.
 
  @param rgi
  The rgi used to find the row entry we want the results for.
 
  @param bias
  The bias used to find the row entry we want the results for.
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_get_row_data(uint32 chan,
                                  ftm_cal_data_dpd_map_row_type* row,
                                  int8 pa, 
                                  int16 rgi, 
                                  int16 bias)
{
  uint8 row_idx;
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(chan != module_ptr->dpd_map_cur_chan)
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_get_row_data: " 
                         "Accessing DPD map with invalid chan %d", chan);
      return FALSE;
    }
  }
  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_get_row_data:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return FALSE;
  }
  if(row == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_get_row_data: Null Input");
    return FALSE;
  }

  row_idx = ftm_cal_data_search_unique_row(pa,rgi,bias);

  if(row_idx != 0xFF)
  {
    memscpy((void*)row,
            sizeof(ftm_cal_data_dpd_map_row_type),
           (void*)&module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[row_idx],
           sizeof(ftm_cal_data_dpd_map_row_type));
  }
  else
  {
    status = FALSE;
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set Row Data

  @details
  Takes the input data and inserts it into the data module
 
  @param row
  The row data we want to commit to the data module

  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_set_row_data(uint32 chan,
                                  ftm_cal_data_dpd_map_row_type* row)
{
  uint8 row_idx; 
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(chan != module_ptr->dpd_map_cur_chan)
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_set_row_data: " 
                         "Accessing DPD map with invalid chan %d", chan);
      return FALSE;
    }
  }
  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_set_row_data:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return FALSE;
  }

  row_idx = module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->num_map_entries;

  /* Copy row params */
  memscpy((void*)&module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[row_idx],
          sizeof(ftm_cal_data_dpd_map_row_type),
         (void*)row, 
         sizeof(ftm_cal_data_dpd_map_row_type));

  /* Save the row index where we stored this item */
  module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[row_idx].row_idx = row_idx;

  module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[row_idx].dpd = 
    &(module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_table->table[row_idx]);

  /* Increment size so we point to the next available table entry for the next write */
  module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->num_map_entries++;

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Row Data

  @details
  This will return a row from the data module associated with the desired
  power passed in. 
 
  @param row
  A row from the data module.
 
  @param power
  The power used to find the row entry we want the results for.
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_get_row_data_by_power(uint32 chan,
                                           ftm_cal_data_dpd_map_row_type* row,
                                           uint16 power,
                                           uint8 xpt_mode)
{
  uint8 row_idx;
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(chan != module_ptr->dpd_map_cur_chan)
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_get_row_data_by_power: " 
                         "Accessing DPD map with invalid chan %d", chan);
      return FALSE;
    }
  }
  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_get_row_data_by_power:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return FALSE;
  }
  if(row == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_get_row_data_by_power: Null Input");
    return FALSE;
  }

  row_idx = ftm_cal_data_search_power(power,xpt_mode);

  if( row_idx <FTM_COMMON_XPT_NUM_DPD_MAP_ROWS )
  {
    memscpy((void*)row,
           sizeof(ftm_cal_data_dpd_map_row_type),
           (void*)&module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[row_idx],
           sizeof(ftm_cal_data_dpd_map_row_type));
  }
  else
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_get_row_data_by_power: Could not find power entry %d",power);
    status = FALSE;
  }

  return status;
} 

/*! \} */


/*============================================================================*/
/*!
  @name DATA

  @brief
  This section contains the functions used by the DATA opcode
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Clear Current DPD Map

  @details
  This function will clear the current dpd map based on the channel selected

  @param void

  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_clear_current_dpd_map(uint32 chan)
{
  uint8 i;
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(chan != module_ptr->dpd_map_cur_chan)
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_clear_current_dpd_map: " 
                         "Accessing DPD map with invalid chan %d", chan);
      return FALSE;
    }
  }
  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_clear_current_dpd_map:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return FALSE;
  }

  module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->num_map_entries = 0;

  /* If we never check this as part of a search, it might not be necessary to maintain it's state */
  /* TODO: what to re-initialize if anything other than saying no map entries */
  for(i=0;i<FTM_COMMON_XPT_NUM_DPD_MAP_ROWS;i++)
  {
    module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->table[i].status = FTM_COMMON_XPT_INVALID;
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set Therm ADC

  @details
  This will set the thermistor ADC value for the current channel. 
 
  @param chan
  Channel to write to for the associated therm ADC value.
 
  @param therm_adc
  Desired therm ADC read to store
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_set_therm_adc(uint32 chan, uint16 *therm_adc)
{
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(therm_adc == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_set_therm_adc: Null input");
    return FALSE;
  }
  if(chan != module_ptr->dpd_map_cur_chan)
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_set_therm_adc: " 
                         "Accessing DPD map with invalid chan %d", chan);
      return FALSE;
    }
  }
  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_set_therm_adc:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return FALSE;
  }

  module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].therm_adc = *therm_adc;

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Therm ADC

  @details
  This will return the thermistor ADC value for the current channel. 
 
  @param chan
  Channel to look into for the associated therm ADC value.
 
  @param therm_adc
  Container to return the desired therm ADC read
 
  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean ftm_cal_data_get_therm_adc(uint32 chan, uint16 *therm_adc)
{
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(therm_adc == NULL)
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_get_therm_adc: Null input");
    return FALSE;
  }
  if(chan != module_ptr->dpd_map_cur_chan)
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_get_therm_adc: " 
                         "Accessing DPD map with invalid chan %d", chan);
      return FALSE;
    }
  }
  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_get_therm_adc:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return FALSE;
  }

  *therm_adc = module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].therm_adc;

  return status;
}

/*! \} */


/*============================================================================*/
/*!
  @name Search APIs

  @brief
  This section contains the functions used locally to search the DPD Map
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Search for Row by PA/RGI/Bias

  @details
  This will search for a row in the data module that contains the requested
  PA/RGI/Bias combination and return the row index. 
 
  @param pa_state
  The PA state value we are looking for
 
  @param rgi
  The rgi value we are looking for
 
  @param bias
  The bias value we are looking for
 
  @return
  The row index
*/
static uint8 ftm_cal_data_search_unique_row(int8 pa_state, int16 rgi, int16 bias)
{
  uint8 i;
  uint8 idx = 0xFF;
  ftm_cal_data_dpd_map_type *cur_map;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_search_unique_row:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return idx;
  }

  cur_map = module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map;

  for(i=0;i<FTM_COMMON_XPT_NUM_DPD_MAP_ROWS;i++)
  {
    if( (cur_map->table[i].curr_tx_override.rgi == rgi) &&
        (cur_map->table[i].curr_tx_override.pa_state == pa_state) &&
        (cur_map->table[i].curr_tx_override.papm_bias == bias))
    {
      idx = i;
      break;
    }
  }
  return idx;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Search for Row by Power

  @details
  This will search for a row in the data module that contains the requested
  power and return the row index. 
 
  @param power
  The power value we are looking for
 
  @return
  The row index
*/
static uint8 ftm_cal_data_search_power(int16 power, uint8 xpt_mode)
{
  uint8 i;
  uint8 idx = 0xFF;
  ftm_cal_data_dpd_map_type *cur_map;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_search_power:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return idx;
  }

  cur_map = module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map;

  for(i=0;i<module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map->num_map_entries;i++)
  {
    if((cur_map->table[i].power == power) &&
       (cur_map->table[i].curr_tx_override.xpt_mode == xpt_mode)) 
    {
      idx = i;
    }
  }
  return idx;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Search for Row by Power

  @details
  This will search for a row in the data module that contains the requested
  capture buffer ID and return the row index. 
 
  @param cap_id
  The capture buffer ID we are looking for
 
  @return
  The row index
*/
static uint8 ftm_cal_data_search_cap_id(uint8 cap_id)
{
  uint8 i;
  uint8 idx = 0xFF;
  ftm_cal_data_dpd_map_type *cur_map;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_search_cap_id:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return idx;
  }

  cur_map = module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map;

  for(i=0;i<FTM_COMMON_XPT_NUM_DPD_MAP_ROWS;i++)
  {
    if( cur_map->table[i].cap_buf_idx == cap_id &&
        cur_map->table[i].status == FTM_COMMON_XPT_CUR_ORIDE_LOADED) 
    {
      idx = i;
      break;
    }
  }
  return idx;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Search for Row by Power

  @details
  This will search for a row in the data module that contains the requested
  results buffer ID and return the row index. 
 
  @param res_id
  The result buffer ID we are looking for
 
  @return
  The row index
*/
static uint8 ftm_cal_data_search_res_id(uint8 res_id)
{
  uint8 i;
  uint8 idx = 0xFF;
  ftm_cal_data_dpd_map_type *cur_map;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if(module_ptr->dpd_map_cur_chan_idx >= RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_search_res_id:"
                       " Accessing DPD map with invalid chan idx %d",
                       module_ptr->dpd_map_cur_chan_idx);
    return idx;
  }

  cur_map = module_ptr->data_module[module_ptr->dpd_map_cur_chan_idx].dpd_map;

  for(i=0;i<FTM_COMMON_XPT_NUM_DPD_MAP_ROWS;i++)
  {
    if( cur_map->table[i].res_buf_idx == res_id && 
        cur_map->table[i].status == FTM_COMMON_XPT_IQ_CAPTURE_COMPLETE) 
    {
      idx = i;
      break;
    }
  }
  return idx;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Search for requested channel in DPD map

  @details
  This will search for a requested channel in the DPD map and update the
  dpd_map_cur_chan_idx to point to the table for that channel. The data module
  supports upto 16 channels and a map may be created and being used for any 
  of these 16 channels. The below function allows a user to query for the data 
  corresponding to any one of upto 16 channels. If the current channel doesn't 
  match, the below function will find the index, and update the master status 
  of the data module to reflect the new channel.
 
  @param chan
  The channel we're looking for
 
  @return
  TRUE if channel was found, else FALSE
*/
static boolean ftm_cal_data_search_chan(uint32 chan)
{
  uint8 i;
  boolean table_exists = FALSE;
  uint8 temp_chan_idx = 0;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  /* Check if we have initialized the data module for this 
     channel already. If we have, use that. */
  for(i=0;i<RFCOM_TXLIN_NUM_CHANNELS;i++)
  {
    if(module_ptr->data_module[i].chan == chan)
    {
      table_exists = TRUE;
      temp_chan_idx = i;
      break;
    }
  }

  /* Update the global dpd_map_cur_chan_idx to the index we found, 
  so as to minimise future searching */
  module_ptr->dpd_map_cur_chan_idx = temp_chan_idx;
  module_ptr->dpd_map_cur_chan = chan;

  if ( table_exists == FALSE )
  {
    RF_MSG_1(RF_ERROR, "ftm_cal_data_search_chan: " 
                       "Accessing DPD map with invalid chan %d", chan);
  }

  return table_exists;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prints the current pin table
 
  @details
  This prints the pin table and is only used for debug
 
  @param void
 
  @return void
*/

void ftm_cal_data_print_pin_table(void)
{
#if 0
  uint8 i;
  ftm_cal_data_pin_map_type *pin_map;

  pin_map = ftm_common_data_get_pin_table_ptr();

  RF_MSG(RF_HIGH, "---- Pin Table ----");
  RF_MSG(RF_HIGH, " i chan    status     PinVal    capId    ResId");

  for ( i = 0; i < pin_map->num_map_entries; i++ )
  {
      RF_MSG_6(RF_HIGH,"[%d]  %d  | %d  | %d  | %d  | %d", 
               i, pin_map->table[i].chan, pin_map->table[i].status,
               pin_map->table[i].pin_compensation, pin_map->table[i].cap_buf_idx,
               pin_map->table[i].res_buf_idx
               );
  }
#endif  
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prints the current DPD Map
 
  @details
  This prints the DPD map and is only used for debug
 
  @param void
 
  @return void
*/
void ftm_cal_data_print_dpd_map(void)
{
  uint8 i,j;
  uint8 num_elements;
  ftm_cal_data_module_type *module_ptr;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  RF_MSG(RF_LOW, "---DPD Map---");
  for(i=0;i<RFCOM_TXLIN_NUM_CHANNELS;i++)
  {
    if(module_ptr->data_module[i].chan != 0xFFFFFFFF)
    {
      RF_MSG_1(RF_LOW, " Channel %d",module_ptr->data_module[i].chan);
      num_elements = module_ptr->data_module[i].dpd_map->num_map_entries;
      for(j=0;j<num_elements;j++)
      {
        RF_MSG_7(RF_LOW, " PA %d RGI %d Bias %d Pwr %d "
                         "CapID %d ResID %d Status %d",
         module_ptr->data_module[i].dpd_map->table[j].curr_tx_override.pa_state,
         module_ptr->data_module[i].dpd_map->table[j].curr_tx_override.rgi,
         module_ptr->data_module[i].dpd_map->table[j].curr_tx_override.papm_bias,
               module_ptr->data_module[i].dpd_map->table[j].power,
               module_ptr->data_module[i].dpd_map->table[j].cap_buf_idx,
               module_ptr->data_module[i].dpd_map->table[j].res_buf_idx,
               module_ptr->data_module[i].dpd_map->table[j].status);
      }
    }
  }
} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Populate M Line results
 
  @details
  Store the M Line result pointer after M Line calculation to be copied to
  the data module for dispatch
 
  @param chan
  Channel to set associated MLine results
 
  @param xpt_type
  XPT type EPT or ET
 
  @param mline_results
  Pointer which holds address of EPT or ET M Line results
 
  @return status
  TRUE of results were populated successfully; FALSE otherwise
*/
boolean ftm_cal_data_store_mline_res_ptr(uint32 chan,
                                       rf_common_xpt_type xpt_type,
                                       ftm_calibration_mline_result_xpt_type* mline_results)
{
  boolean status = TRUE;
  ftm_cal_data_module_type *module_ptr;
  ftm_calibration_mline_result_xpt_type *xpt_type_res_ptr = NULL;

  /* Get a pointer to the Data Module */
  module_ptr = ftm_common_data_get_data_module_ptr();

  if ( chan != module_ptr->dpd_map_cur_chan )
  {
    /* If the current channel being used by DPD map doesn't match 
    the channel passed in, then search through DPD map for the channel 
    passed in. If found, use the table for that channel, and also update 
    DPD map status to point to new channel. This minimizes searching through 
    the DPD map for channel of interest. Upto 16 channels are supported.
    If channel is not found, error out */
    if( ftm_cal_data_search_chan(chan) == FALSE )
    {
      RF_MSG_1(RF_ERROR, "ftm_cal_data_store_mline_res_ptr: " 
                         "Trying to return MLine result map with invalid chan %d", chan);
      return FALSE;
    }
  }
  if ( mline_results == NULL )
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_store_mline_res_ptr: "
                       " Cannot access M Line results.");
    return FALSE;
  }

  if ( xpt_type == XPT_TYPE_EPT )
  {
    xpt_type_res_ptr = &module_ptr->mline_results.ept_mline_results;

    memscpy((void*)xpt_type_res_ptr,
            sizeof(ftm_calibration_mline_result_xpt_type),
            (void*)mline_results,
            sizeof(ftm_calibration_mline_result_xpt_type));
  }
  else if ( xpt_type == XPT_TYPE_ET )
  {
    xpt_type_res_ptr = &module_ptr->mline_results.et_mline_results;

    memscpy((void*)xpt_type_res_ptr,
            sizeof(ftm_calibration_mline_result_xpt_type),
            (void*)mline_results,
            sizeof(ftm_calibration_mline_result_xpt_type));
  }
  else
  {
    RF_MSG(RF_ERROR, "ftm_cal_data_store_mline_res_ptr: "
                       " Invalid XPT TYPE received. ");
    status =  FALSE;
  }
  return status;
}
#endif


/*! \} */

/*! @} */

