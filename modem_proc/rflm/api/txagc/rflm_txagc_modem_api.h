#ifndef RFLM_TXAGC_MODEM_API_H
#define RFLM_TXAGC_MODEM_API_H
/*
  @file
  rflm_txagc_modem_api.h

  @brief
  This file contains RF FED tx agc definitions, enumerations and prototypes that
  are exposed to external modules, such as RF SW and FW.

  @details
  This file defines RF FED txagc interface,APIs, and commons structures

  @addtogroup RFLM_TXAGC_MODEM_INTF
  @{
*/

/*=============================================================================

                R F  F E D TXAGC   H E A D E R   F I L E 
       
DESCRIPTION
  This file contains declarations and definitions to support RF frent end driver txagc module.

Copyright (c) 2013-2019  by Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rflm.mpss/1.10/api/txagc/rflm_txagc_modem_api.h#1 $

when       who    what, where, why
--------   ---    -------------------------------------------------------------
02/04/19   as      ADC/DAC control in rfsw
05/25/17   as     Add API to support Immediate IQ gain programming
07/30/16   qzh    Add CATM1 ul hopping related changes
01/14/15   zhw    1x FED dynamic TXC timestamp support
01/05/15   spa    Change dpd_data to be per Tx modem chain
10/17/14   br     Added support to apply TxRSB IQMC values to Tx.
07/23/14   hdz    Added rflm_txagc_modem_compute_delaymatch_params()
05/07/14   kai    Added DAC Iref immediate trigger
04/11/14     cri     DAC management changes for LTE TxAGC
03/19/13   cri     Split sample capture initial revision
03/06/14   zhw    DPM DPD init Stub API
02/20/14   oei    Don't include "rfcmd_msg.h"
02/19/14   zhw    DPM Time stamp API support
02/12/14   oei    Remove Bolt-specifc rflm_txagc_modem_dma_trig_msg_t
01/23/14   kai    Added API to set PA phase comp offset
12/20/13   cri    Enable ET Delay 
12/09/13   kai    Add support for dpd table selection
12/04/13   cri    Changes to enable ET DAC and to get default IQ Gain
11/25/13   kai    Add support for rflm_txagc_modem_set_xpt_mode API
11/25/13   id     Load DPD - Index Management
11/22/13   id     Load DPD changes for BOLT
11/21/13   ljl    Enable DMA trigger related features
11/18/13   cd     Add Env Scale programming to LMEM
11/17/13   hdz    Added rflm_txagc_modem_cal_dpd_start_ofs()
11/03/13   hdz    Added et_dac_seq()
10/25/13   id     Added initial support for Load DPD
10/25/13   hdz    Added handle id to dac config Iref API
10/21/13   cd     Add IREF override functionality
10/21/13   cd     Add IQ gain programming to LMEM
10/20/13   hdz    Added rflm_txagc_modem_dma_payload()
04/04/13   hdz    Initial creation

===========================================================================*/
#include "rflm.h"
#include "txlm_intf.h"

#if (RFLM_FEATURE_BUILD_MODE==RFLM_FEATURE_BUILD_NORMAL)
#include "msgr.h"
#endif

#include "rflm_txagc_api.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  rflm_txagc_xpt_mode_t xpt_mode;
  uint8  power_level_idx;
  /*DTR HW table sel of 0 or 1*/
  uint8  dtr_table_sel;
  /*drif group a or group b*/
  uint8  drif_group_idx;
  /* drif set index of 0,1, or 2*/
  uint8  drif_set_idx;
}rflm_txagc_modem_lmem_cfg_t; 

/* @brief
 This structure is used in FTM cal mode by rflm_txagc_modem_drif_dma_trig only.
 In this case, all techs will only DMA one block;
*/
typedef struct
{
  /*! @brief chain index */
  uint32 chain_idx;
  
  /*! @brief TXAGC DMA structure */
  rflm_dtr_tx_xfer_list_s rfcmd_txagc_dma;
  
} rflm_txagc_modem_dma_payload;


/*----------------------------------------------------------------------------*/
/*! @detail
   data type used in txagc override cmd for all control data

*/
typedef struct
{
  /*! @brief  new pa state     */
  rflm_txagc_pa_state_data_t pa_state;

  /*! @brief new pa action   */
  rflm_txagc_pa_action_t pa_action;

  /*! @brief action time    */
  uint32 action_time; 

}rflm_txagc_modem_override_ctl_data_t;

/*----------------------------------------------------------------------------*/
/*! @detail
    data type used in txagc override cmd for all input params
*/
typedef struct
{
  /*! @brief  new iq gain value     */
  uint32 iq_gain;

  /*! @brief new env_scale   */
  uint32 env_scale;

  /*! @brief new et_path delay    */
  int32 et_delay;

  /*! @brief new xpt mode*/
  rflm_txagc_xpt_mode_t xpt_mode;
  
}rflm_txagc_modem_override_param_data_t;


/*----------------------------------------------------------------------------*/
/*! @detail
    data type used in txagc override cmd 
*/
typedef struct
{
  /*! @brief  txagc override ctl data     */
  rflm_txagc_modem_override_ctl_data_t ctl;

  /*! @brief txagc overide param data   */
  rflm_txagc_modem_override_param_data_t param;
  
}rflm_txagc_modem_override_input_data_t;



/*----------------------------------------------------------------------------*/
/*! @detail
   this API will set xpt accordingly

   @param xpt_mode
   APT_MODE;
   EPT_MODE;
   ET_MODE;

   @param drif_set_id
   The drif set ID

   @return TRUE if succeeds, FALSE otherwise
*/
rflm_err_t
rflm_txagc_modem_set_xpt_mode
(
  rflm_txagc_xpt_mode_t xpt_mode,
  uint32 handle_id,
  uint8  group_id,
  uint8  drif_set_id
);

/*----------------------------------------------------------------------------*/
/*! @detail
   this API will set xpt accordingly

   @param dpd_table_sel
   0 or 1; alternate
 
   @param handle_id
   Tx RFLM handle ID
 
   @param group_id
   Index identifying whether the value needs to go to group A or group B
 
   @param drif_set_id
   The drif set ID

   @return TRUE if succeeds, FALSE otherwise
*/
rflm_err_t
rflm_txagc_modem_set_tbl_sel
(
  uint8  dpd_table_sel,
  uint32 handle_id,
  uint8  group_id,
  uint8  drif_set_id
);

/*----------------------------------------------------------------------------*/
void 
rflm_txagc_modem_set_dynamic_1x_t1 
(
  uint32 handle_id
);

/*----------------------------------------------------------------------------*/
void 
rflm_txagc_modem_set_timestamp_t1 
(
  uint32 handle_id,
  uint32 timestamp,
  boolean reuse 
);

/*----------------------------------------------------------------------------*/
void 
rflm_txagc_modem_set_timestamp_t2 
(
  uint32 handle_id,
  uint32 timestamp,
  boolean reuse 
);

/*----------------------------------------------------------------------------*/
uint32
rflm_txagc_modem_get_default_iq_gain
(
   uint32 handle_id
);

/*----------------------------------------------------------------------------*/
/*! 
   @brief
   This API will set iq gain in LMEM

   @param iq_gain
   IQ gain in HW format

   @param handle_id
   Tx RFLM handle ID
 
   @param group_id
   Index identifying whether the value needs to go to group A or group B
 
   @param drif_set_id
   The drif set ID
 
   @return TRUE if succeeds, FALSE otherwise
*/
rflm_err_t
rflm_txagc_modem_set_iq_gain
(
  uint32 iq_gain,
  uint32 handle_id,
  uint8  group_id,
  uint8  drif_set_id
);

/*----------------------------------------------------------------------------*/
/*! 
   @brief
   This API will set iq gain in LMEM

   @param iq_gain
   IQ gain in HW format

   @param handle_id
   Tx RFLM handle ID
 
   @param group_id
   Index identifying whether the value needs to go to group A or group B
 
   @param drif_set_id
   The drif set ID
 
   @return TRUE if succeeds, FALSE otherwise
*/
rflm_err_t
rflm_txagc_modem_set_iq_gain_imm
(
  uint32 iq_gain,
  uint32 handle_id,
  uint8  group_id,
  uint8  drif_set_id
);



/*----------------------------------------------------------------------------*/
/*! 
   @brief
   This API will set iq gain set 2

   @param iq_gain
   IQ gain in HW format

   @param handle_id
   Tx RFLM handle ID
 
   @param group_id
   Index identifying whether the value needs to go to group A or group B
 
   @param drif_set_id
   The drif set ID
 
   @return TRUE if succeeds, FALSE otherwise
*/
rflm_err_t
rflm_txagc_modem_set_iq_gain_set2
(
  uint32 iq_gain,
  uint32 handle_id,
  uint8  group_id,
  uint8  drif_set_id
);

/*----------------------------------------------------------------------------*/
/*! 
   @brief
   This API will set iq gain into LMEM and trigger the DMA manually for
   IQ gain override

   @param iq_gain
   IQ gain in HW format

   @param handle_id
   the rflm_dm_handle id for BOlt
   the legacy txlm_handle id for DIME
 
   @param group_id
   Index identifying whether the value needs to go to group A or group B
 
   @param drif_set_id
   The drif set ID
 
   @return TRUE if succeeds, FALSE otherwise
*/
rflm_err_t
rflm_txagc_modem_set_iq_gain_override
(
  uint32 iq_gain,
  uint32 handle_id,
  uint8  group_id,
  uint8  drif_set_id
);

/*----------------------------------------------------------------------------*/
/*! 
   @brief
   This API will set Env scale value in LMEM

   @param env_scale
   Env Scale in HW format

   @param handle_id
   Tx RFLM handle ID
 
   @param group_id
   Index identifying whether the value needs to go to group A or group B
 
   @param drif_set_id
   The drif set ID
 
   @return TRUE if succeeds, FALSE otherwise
*/
rflm_err_t
rflm_txagc_modem_set_env_scale
(
  uint32 env_scale,
  uint32 handle_id,
  uint8  group_id,
  uint8  drif_set_id
);

/*----------------------------------------------------------------------------*/
/*! 
   @brief
   This API will set IQ MC value in LMEM

   @param iqmc
   Alpha and Beta value 

   @param handle_id
   Tx RFLM handle ID
 
   @param group_id
   Index identifying whether the value needs to go to group A or group B
 
   @param drif_set_id
   The drif set ID
 
   @return TRUE if succeeds, FALSE otherwise
*/
rflm_err_t
rflm_txagc_modem_set_iqmc
(
  rflm_txagc_dtr_iqmc_data_t *iqmc,
  uint32 handle_id,
  uint8  group_id,
  uint8  drif_set_id
);

/*----------------------------------------------------------------------------*/
/*! @detail
   this API will set  Iref in FW format accordly

   @param Iref_val
   Iref_val in raw uA10 format

   @param drif_set_id
   The drif set ID

   @param handle_id
   the rflm_dm_handle id for BOlt;
   the legacy txlm_handle id for DIME;

   @param cfg_data
   pointer to cfg_data structure that carries Iref infor and needs to return

   @return TRUE if succeeds, FALSE otherwise
*/
boolean
rflm_txagc_modem_set_iref
(
  uint8 iref,
  rflm_ustmr_t  wtr_trigger_time,
  uint32 handle_id,
  void *cfg_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Functions for setting iref related configuration in Tx override mode
    
  @details
  this API will set iref into LMEM and trigger DMA for override

  @param iref_lut_index
  The idx of iref in the lUT table, neither the raw RF nor HW uint16 format
 
  @param handle_id
  The rflm_dm_handle id
           
  @return 
  return TRUE if succeeds, FALSE otherwise

*/
rflm_err_t
rflm_txagc_modem_set_iref_override
(
  uint8 iref_lut_index,
  uint32 handle_id
);

/*----------------------------------------------------------------------------*/
/*! @detail
   this API will set   in FW format accordly

   @param input_data
   pointer to Input param for configuring fw data

   @param handle_id
   the rflm_dm_handle id for BOlt;
   the legacy txlm_handle id for DIME;

   @param fw_data
   pointer to fw_data that needs to return back to FW

   @return TRUE if succeeds, FALSE otherwise
*/
rflm_err_t
rflm_txagc_modem_config_fw_data
(
  void *input_data,
  uint32 handle_id,
  void *fw_data
);

/*----------------------------------------------------------------------------*/
/*! @detail
   this API will enable TX-DAC

   @param handle_id
   the rflm_dm_handle id for BOlt;
   the legacy txlm_handle id for DIME;

   @return TRUE if succeeds, FALSE otherwise
*/
rflm_err_t
rflm_txagc_modem_enable_tx_dac
(
  uint32 handle_id,
  uint32 dac_start_time, 
  boolean immediate
);

/*----------------------------------------------------------------------------*/
/*! @detail
   this API will disable TX-DAC

   @param handle_id
   the rflm_dm_handle id for BOlt;
   the legacy txlm_handle id for DIME;

   @return TRUE if succeeds, FALSE otherwise
*/
rflm_err_t
rflm_txagc_modem_disable_tx_dac
(
  uint32 handle_id,
  uint32 dac_stop_time, 
  boolean immediate
  
);

/*----------------------------------------------------------------------------*/
/*! @detail
   this API will enable fcal_clk

   @param handle_id
   the rflm_dm_handle id for BOlt;
   the legacy txlm_handle id for DIME;

   @return TRUE if succeeds, FALSE otherwise
*/
boolean
rflm_txagc_modem_enable_fcal_clk
(
  uint32 handle_id
);

/*----------------------------------------------------------------------------*/
/*! @detail
   this API will disable fcal_clk

   @param handle_id
   the rflm_dm_handle id for BOlt;
   the legacy txlm_handle id for DIME;

   @return TRUE if succeeds, FALSE otherwise
*/
boolean
rflm_txagc_modem_disable_fcal_clk
(
  uint32 handle_id
);

/*----------------------------------------------------------------------------*/
/*! @detail
   this API will enable xo_gate

   @param handle_id
   the rflm_dm_handle id for BOlt;
   the legacy txlm_handle id for DIME;

   @return TRUE if succeeds, FALSE otherwise
*/
boolean
rflm_txagc_modem_enable_xo_gate
(
  uint32 handle_id
);

/*----------------------------------------------------------------------------*/
/*! @detail
   this API will disable xo_gate

   @param handle_id
   the rflm_dm_handle id for BOlt;
   the legacy txlm_handle id for DIME;

   @return TRUE if succeeds, FALSE otherwise
*/
boolean
rflm_txagc_modem_disable_xo_gate
(
  uint32 handle_id
);

/*----------------------------------------------------------------------------*/
/*! @detail
   this API will do txagc_override 

   @param handle_id
   the rflm_dm_handle id for BOlt;
   the legacy txlm_handle id for DIME;

   @return TRUE if succeeds, FALSE otherwise
*/
boolean
rflm_txagc_modem_override_params
(
  uint32 handle_id,
  void *input_data,
  void *output_data
);

/*----------------------------------------------------------------------------*/
/*! @detail
   this API will do txagc_override 

   @param handle_id
   the rflm_dm_handle id for BOlt;
   the legacy txlm_handle id for DIME;

   @return TRUE if succeeds, FALSE otherwise
*/
rflm_err_t
rflm_txagc_modem_drif_dma_trig
(
  uint32 handle_id,
  void *input_data,
  void *output_data
);

/*----------------------------------------------------------------------------*/
/*! @detail
   this API will do rflm dac Iref programming by calling DTR API. This API will 
   config Iref to HW, not to memory (For Blot and new targets, for DIME, it 
   does nothing)

   The difference between this API and rflm_txagc_modem_set_iref is this API 
   commits to HW, while the other API commit to memory (used by LTE only), and 
   return back to FW

   @param update_en
   The flag to DTR to do iref update or set start/stop time back to 0;
   when the flag is set to TRUE, DTR will program iref based on the start/stop 
   time provided;
   when the flag is set to FALSE, DTR will not do the update, instead, DTR will 
   set start /stop time back to 0
   
   @param iref_idx
   The iref idx in the lut; not the uA or HW format unit;

   @param iref_start_time
   ustmr time to indicate the start of iref latch

   @param iref_stop_time
   ustmr time to indicate the stop of iref latch
*/
rflm_err_t
rflm_txagc_modem_config_dac_iref
(
  uint32 handle_id,
  boolean update_en,
  uint32 iref_idx,
  uint32 iref_update_start,
  uint32 iref_update_stop
);

/*----------------------------------------------------------------------------*/
/*! @detail 
   this API will set DAC IREF immediate trigger register on/off
 
   @param handle_id
   the rflm_dm_handle id for BOlt;
   the legacy txlm_handle id for DIME;
 
   @param enable
   The flag to set DAC IREF immediate trigger on or off;
*/
boolean
rflm_txagc_modem_set_iref_immediate_trigger
(
  uint32 handle_id,
  boolean enable
);

/*----------------------------------------------------------------------------*/
boolean
rflm_txagc_modem_enable_tx_dac_seq
(
  uint32 handle_id,
  uint32 dac_start_time, 
  boolean immediate
);

/*----------------------------------------------------------------------------*/
/*! @detail
   this API will set PA phase comp offset

   @param rot_angle
   PA phase comp from rotator fall value. 360/256 in 2's complement format

   @param handle_id
   Tx RFLM handle ID
 
   @param group_id
   Index identifying whether the value needs to go to group A or group B
 
   @param drif_set_id
   The drif set ID

   @return TRUE if succeeds, FALSE otherwise
*/
rflm_err_t
rflm_txagc_modem_set_pa_phase_comp_offset
(
  int16 rot_angle,
  uint32 handle_id,
  uint8  group_id,
  uint8  drif_set_id,
  rflm_drif_out_cfg_t *drif_out_cfg
);

/*----------------------------------------------------------------------------*/
rflm_err_t
rflm_txagc_modem_iqmc_override
(
  uint32 handle_id,
  rflm_txagc_dtr_iqmc_data_t *iqmc
);

/*----------------------------------------------------------------------------*/
boolean 
rflm_txagc_modem_compute_delaymatch_params
(
  uint32 handle_id,
  int64 input_delay, 
  uint32* coarse_TAU, 
  uint32* dm2Fine_TAU, 
  uint32* dm3Frac_TAU
);

#ifdef __cplusplus
}
#endif

/*!   @} */

#endif
