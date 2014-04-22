/*!
  @file
  rflm_api_nb.h

  @brief
  RF LM API definitions for LTE NBIOT
  
  @detail
  
  
  @todo
 

*/

/*==============================================================================

  Copyright (c) 2015 - 2018 Qualcomm Technologies, Inc. (QTI). All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rflm.mpss/1.10/api/fw/rflm_api_nb.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/13/20   gk      Added API(NB-IOT) to get lna offsets for all gains
08/30/18   sk      Combine Rx PLL and Sigpath toggle api's
08/15/18   qzh     Add fast mode AGC support
09/05/17   qzh     Update the lower limit of TxAGC activation time advance
07/31/17   as      Enable support for Short DTx type SF in NB1
03/28/17   as      Support for high speed ADC rate for certain EARFCNs to avoid desense due to ADC spurs 
10/27/16   can     Update RFLM_LTE_NB_TXAGC_NUM_SLOT_PER_SF
09/08/16   qzh     Initial version
==============================================================================*/

#ifndef RFLM_API_NB_H
#define RFLM_API_NB_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "rflm_api_cmn.h"
#include "rflm_api_lte.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

#define RFLM_LTE_NB_TXAGC_NUM_SLOT_PER_SF     1

#define RFLM_LTE_NB_MIN_TX_ACT_TIME_USTMR      7680 // 7680 ustmr counts = 400us
#define RFLM_LTE_NB_MAX_TX_ACT_TIME_USTMR      19200 // 19200 ustmr counts = 1000us


/*==============================================================================

                     Input/Output structure definitions
  
==============================================================================*/


/*==============================================================================
                TXAGC related input/output structure definitions
==============================================================================*/

typedef enum
{
   RFLM_LTE_UL_SF_CMD_ACTIVE = 0,
   RFLM_LTE_UL_SF_CMD_DTX,             
   RFLM_LTE_UL_SF_CMD_SHORT_DTX,                               
   RFLM_LTE_UL_SF_CMD_NUM,           
   RFLM_LTE_UL_SF_CMD_INVALID = 0xFF
} rflm_lte_nb_ul_sf_type_e;

/*! @ brief enum to represent action time type. 
Default action time is used for both ACTIVE and DTX type SF, 
Short Dtx action time is used for SHORT_DTX type SF. 
*/
typedef enum
{
   RFLM_LTE_NB_DEFAULT_ACTION_TIME= 0,
   RFLM_LTE_NB_SHORT_DTX_ACTION_TIME= 1,
   RFLM_LTE_NB_ACTION_TIME_NUM
} rflm_lte_nb_ul_action_time_e;

typedef enum
{
  RFLM_LTE_NB_CARRIER_ANCHOR = 0,
  RFLM_LTE_NB_CARRIER_NON_ANCHOR,
  RFLM_LTE_NB_CARRIER_NUM,
  RFLM_LTE_NB_CARRIER_INVALID = 0XFF
}rflm_lte_nb_carrier_type_e;

/*! @brief rflm_lte_update_tx_sf_input_s is the input from FW to schedule TXAGC 
  per SF per cell
  -     nb_ul_sf_type: ul subframe cmd type for NBIOT
  -	sf_num: Subframe number (0 - 9) 
  -	action_time: Action time for each slot. The unit is USTMR 
  -	rflm_lte_pwr_db10: Desired Tx Power, unit is dB10.  
  -     afc_sf_active: Flags if AFC correction is needed for this SF
  -     afc_action_time: The time to enable AFC for this SF
*/
typedef struct
{
  rflm_lte_nb_ul_sf_type_e       nb_ul_sf_type;
  uint8                          sf_num;
  uint32                         action_time[RFLM_LTE_NB_ACTION_TIME_NUM];
  int32                          rflm_lte_pwr_db10;
  boolean                        afc_sf_active;
  uint32                         afc_action_time;
  uint32                         frame_num;
}rflm_lte_nb_update_tx_sf_input_s;


/*! @brief rflm_lte_update_tx_sf_return_s is the output from FED to FW 
  per SF per cell
  -	txagc_success: Flag successful run of TxAGC
  -     wmore_clipper: wmore clipper setting.
*/
typedef struct
{  
  boolean                         txagc_success;
  rflm_lte_wmore_clipper_t        wmore_clipper;
}rflm_lte_nb_update_tx_sf_return_s;

/*==============================================================================
                RXAGC related input/output structure definitions
==============================================================================*/
typedef struct
{
  int32             total_rssi_dB10;
  int32             inband_rssi_dB10;
  int8              gain_state;
  rflm_handle_rx_t  handle_id;
  boolean           fast_agc_mode;
}rflm_lte_nb_rx_info_s;

typedef struct
{
  rflm_lte_nb_rx_info_s   rx_info;
  uint8                sf_num;
}rflm_lte_nb_update_rx_sf_input_s;


typedef struct
{
uint8 gain_state;
uint8 sf_num; 
}rflm_lte_nb_get_lna_settings_input_s;

typedef struct
{
 uint8 sf_num; 
}rflm_lte_nb_get_all_lna_settings_input_s;


typedef struct
{
uint8 gain_state;
uint8 sf_num;
int32 action_time;
uint32 vsrc_count;
}rflm_lte_nb_set_lna_state_input_s;

/* ------------------------------------------------------------------------- */

typedef struct
{
boolean                   enable_rx_path;
uint8                     sf_num;
int32                     action_time;
rflm_handle_rx_t          handle_id; 
}rflm_lte_nb_enable_rx_path_input_s;

/* ------------------------------------------------------------------------- */

typedef struct
{
  boolean                   enable_rx_path;
  int32                     action_time;
  rflm_handle_rx_t          handle_id;   
} rflm_lte_nb_enable_disable_rx_path_input_s;

/* ------------------------------------------------------------------------- */

typedef struct
{
  boolean                   toggle_on_rx_pll_sigpath;
  uint8                     sf_num;
  int32                     action_time;
  rflm_handle_rx_t          handle_id;   
}rflm_lte_nb_rx_toggle_pll_sigpath_input_s;

/* ------------------------------------------------------------------------- */

typedef struct
{
  uint32                      channel;         /*EARFCN*/
  int32                       action_time;
  rflm_handle_rx_t            handle_id;
} rflm_lte_nb_fs_retune_input_s;

/* ------------------------------------------------------------------------- */
typedef struct
{
  rflm_lte_nb_carrier_type_e  carrier_type;
  int32                       action_time;
  rflm_handle_rx_t            handle_id;
} rflm_lte_nb_rx_retune_input_s;
/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
              API input/output function definitions -- TXAGC 
==============================================================================*/

/*!
  @brief API to do NBIOT txagc update

  @detail 
  This API is called by FW to do txagc update, including pa 
  state machine, txagc lookup, build txagc ccs issue_seq script and commit to 
  CCS, and program iq_gain,env_scale and dpd table via DRIF driver
 
  This API will be called only once for a given SF and all information
  regarding slot0, slot1 and SRS will be provided per CELL.
 
  If UL CA is active, and SCELL is present, then Modem FW will make
  2 calls of the following function, in which the CELL ID will be provided
  as an argument. 

  @param handle
  tx handle that contains information to de-referene tx LUT and tx script.

  @param in
  pointer to a structure that holds input parameters from FW

  @param out
  pointer to a stucture that holds output parameter to FW
 
  Need to confirm

  @return
  enum of API status  
*/

rflm_err_t rflm_lte_nb_update_tx_sf
( 
   rflm_handle_tx_t                   handle_id,
   rflm_lte_nb_update_tx_sf_input_s   *nb_tx_sf_input,
   rflm_lte_nb_update_tx_sf_return_s     *nb_tx_sf_return 
);


/*==============================================================================
                API input/output function definitions -- RXAGC
==============================================================================*/

/*!
  @brief API to do LTE rxagc update for NBIOT

  @detail 
  This API is called by FW to do rxagc update, including all the RX
  LNA state machine related settings for each path of each CELL. 
  Since this is NBIOT, only PCell and PRx is valid
 
  The function is responsible for interaction with the data layer
  and the write to the CCS layer to send out the HW settings at the
  action time. 

  @param handle
  tx handle that contains information to de-referene tx LUT and tx script.

  @param in
  pointer to a structure that holds input parameters from FW, including

  @param out
  pointer to a stucture that holds output parameter to FW, including gain_state,
  gain in dB, and a boolean to validate the return result.

  @return
  enum of API status  
*/

rflm_err_t rflm_lte_nb_update_rx_sf
( 
   rflm_lte_nb_update_rx_sf_input_s   *nb_rx_sf_input,
   rflm_lte_update_rx_sf_return_s     *rx_sf_return
);


/* ------------------------------------------------------------------------- */
/*!
  @brief API to get LTE LNA settings for NBIOT

  @detail 
  This API is called by Modem FW to get the RX gain offset
  value to set the DVGA loop related settings.


  @param handle
  rx handle that is maintained by data manager.

  @param in
  pointer to a structure that holds input parameters from FW, including
  carrier_index, path, gain_state, and subframe_num.
 
  @param out
  pointer to a stucture that holds output parameter to FW, including
  nominal gain, delta_rx_gain_offset, and default_rx_gain_offset.


  @return
  enum of API status  
*/

rflm_err_t rflm_lte_nb_get_lna_settings
( 
   rflm_handle_rx_t                   handle_id,
   rflm_lte_nb_get_lna_settings_input_s  *nb_lna_settings_info,
   rflm_lte_get_lna_settings_return_s    *total_gain_offset
);

/*
 @detail 
  This API is called by Modem FW to get the RX gain offset for all gains
  value to set the DVGA loop related settings.

  @param handle
  rx handle that is maintained by data manager.

  @param in
  pointer to a structure that holds input parameters from FW, including
  carrier_index, path, gain_state, and subframe_num.
 
  @param out
  pointer to a stucture that holds output parameter to FW, including
  nominal gain, delta_rx_gain_offset, and default_rx_gain_offsets for all gains.
*/
rflm_err_t rflm_lte_nb_get_all_lna_settings( rflm_handle_rx_t handle_id,
     rflm_lte_nb_get_all_lna_settings_input_s	 *nb_lna_settings_info,
     rflm_lte_get_all_lna_settings_return_s	  *lna_settings_return );

/* ------------------------------------------------------------------------- */
/*!
  @brief API to set LTE LNA settings

  @detail 
  This API is called by Modem FW to override the RX gain state without
  running the RXAGC loop. 


  @param handle
  rx handle that is maintained by data manager.

  @param in
  pointer to a structure that holds input parameters from FW, including
  carrier_index, path, gain_state, subframe_num, and action_time.
 
  @param out
  None.


  @return
  enum of API status  
*/
rflm_err_t rflm_lte_nb_set_lna_state
( 
   rflm_handle_rx_t                   handle_id, 
   rflm_lte_nb_set_lna_state_input_s     *nb_lna_state_settings_info
);


/* ------------------------------------------------------------------------- */
/*!
  @brief API to control RX path, i.e. on and off

  @detail 
  This API is used to turn on or off the RX path based on the value of the
  Boolean enable. 


  @param handle
  rx handle that is maintained by data manager.

  @param in
  pointer to a structure that holds input parameters from FW, including
  enable, carrier_index, subframe_num, and action_time.
 
  @param out
  None.


  @return
  enum of API status  
*/
rflm_err_t rflm_lte_nb_enable_rx_path
( 
   rflm_lte_nb_enable_rx_path_input_s    *enable_nb_rx_path_info 
);

rflm_err_t rflm_lte_nb_rx_path_enable_disable(rflm_lte_nb_enable_disable_rx_path_input_s *enable_nb_rx_path_info);

rflm_err_t rflm_lte_nb_rx_toggle_pll_and_sigpath
(
  rflm_lte_nb_rx_toggle_pll_sigpath_input_s *toggle_nb_rx_pll_sigpath_info
);

/* ------------------------------------------------------------------------- */
/*!
  @brief API to get RX config status for NBIOT

  @detail 
  This API is called by Modem FW to get the RX config status.

  @param in
 
  @param out
  TRUE or FALSE
  TRUE = rx config is still active
  FALSE = rx config is done

  @return
  enum of API status  
*/

rflm_err_t rflm_lte_nb_get_rx_config_status
( 
   boolean *rx_config_active_status
);

/* ------------------------------------------------------------------------- */

void rflm_lte_nb_get_adc_rate
(
   uint32 earfcn, 
   uint8 *adc_rate
);

/* ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
/*!
  @brief API to get FTM RF enable status flag
*/
boolean rflm_lte_nb_get_ftm_rf_flag(void);

rflm_err_t rflm_lte_nb_fs_retune(rflm_lte_nb_fs_retune_input_s *nb_fs_retune_info);

/* ------------------------------------------------------------------------- */
/*!
  @brief API to get Cal mode status flag
*/
boolean rflm_lte_nb_get_cal_mode_flag(void);

rflm_err_t rflm_lte_nb_get_lif_offset(int32 *lif_offset_hz);

rflm_err_t rflm_lte_nb_rx_carrier_retune(rflm_lte_nb_rx_retune_input_s *nb_retune_info);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_API_LTE_H */
