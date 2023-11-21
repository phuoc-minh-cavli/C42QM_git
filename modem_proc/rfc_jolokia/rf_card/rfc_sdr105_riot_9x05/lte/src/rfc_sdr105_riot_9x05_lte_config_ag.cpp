
/*
WARNING: This file is auto-generated.

Generated using: rfc_autogen.exe
Generated from:  V5.9.414 of RFC_HWSWCD.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2017 Qualcomm Technologies Incorporated.  All Rights Reserved.

$Header: //components/rel/rfc_jolokia.mpss/2.10/rf_card/rfc_sdr105_riot_9x05/lte/src/rfc_sdr105_riot_9x05_lte_config_ag.cpp#2 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_sdr105_riot_9x05_lte_config_ag.h" 
#include "rfc_sdr105_riot_9x05_cmn_ag.h" 
#include "rfc_common.h" 
#include "rfcom.h" 



extern "C" 
{
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_init_lte_rx_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_init_lte_rx_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_init_lte_tx_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_init_lte_tx_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b1_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b1_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b1_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b1_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b2_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b2_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b2_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b2_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b3_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b3_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b3_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b3_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b4_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b4_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b4_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b4_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b5_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b5_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b5_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b5_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b8_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b8_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b8_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b8_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b12_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b12_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b12_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b12_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b13_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b13_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b13_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b13_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b14_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b14_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b14_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b14_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b18_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b18_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b18_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b18_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b19_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b19_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b19_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b19_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b20_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b20_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b20_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b20_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b25_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b25_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b25_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b25_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b26_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b26_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b26_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b26_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b27_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b27_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b27_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b27_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b28_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b28_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b28_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b28_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b66_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b66_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b66_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b66_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b71_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b71_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b71_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b71_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b85_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b85_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b85_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b85_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_rx0_lte_b86_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_rx0_lte_b86_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_9x05_tx0_lte_b86_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_9x05_tx0_lte_b86_sig_cfg;

  extern rfc_timing_info_type rfc_msm_timing_info[RFC_TIMING_PARAMS_NUM];

  extern rfc_timing_info_type rfc_iotfe_timing_info[RFC_TIMING_PARAMS_NUM];
} /* extern "C" */


rfc_lte_data * rfc_sdr105_riot_9x05_lte_ag::get_instance()
{
  if (rfc_lte_data_ptr == NULL)
  {
    rfc_lte_data_ptr = (rfc_lte_data *)new rfc_sdr105_riot_9x05_lte_ag();
  }
  return( (rfc_lte_data *)rfc_lte_data_ptr);
}

//constructor
rfc_sdr105_riot_9x05_lte_ag::rfc_sdr105_riot_9x05_lte_ag()
  :rfc_lte_data()
{
}

boolean rfc_sdr105_riot_9x05_lte_ag::sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr )
{

  boolean ret_val = FALSE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  if (NULL==cfg)
  {
    *ptr = NULL;
    return FALSE;
  }

  *ptr = NULL;

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) )
  { *ptr = &(rf_card_sdr105_riot_9x05_init_lte_rx_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_init_lte_tx_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b2_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b2_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B3 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b3_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B3 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b3_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b4_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b4_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b5_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b5_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b8_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b8_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b12_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b12_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b13_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b13_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B14 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b14_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B14 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b14_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B18 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b18_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B18 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b18_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B19 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b19_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B19 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b19_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B20 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b20_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B20 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b20_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b25_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b25_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b26_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b26_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B27 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b27_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B27 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b27_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B28 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b28_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B28 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b28_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B66 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b66_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B66 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b66_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B71 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b71_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B71 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b71_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B85 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b85_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B85 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b85_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B86 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b86_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B86 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b86_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  return ret_val;
}

boolean rfc_sdr105_riot_9x05_lte_ag::devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr )
{

  boolean ret_val = FALSE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  if (NULL==cfg)
  {
    *ptr = NULL;
    return FALSE;
  }

  *ptr = NULL;

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) )
  { *ptr = &(rf_card_sdr105_riot_9x05_init_lte_rx_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_init_lte_tx_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b2_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b2_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B3 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b3_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B3 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b3_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b4_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b4_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b5_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b5_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b8_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b8_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b12_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b12_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b13_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b13_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B14 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b14_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B14 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b14_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B18 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b18_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B18 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b18_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B19 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b19_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B19 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b19_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B20 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b20_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B20 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b20_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b25_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b25_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b26_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b26_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B27 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b27_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B27 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b27_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B28 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b28_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B28 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b28_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B66 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b66_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B66 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b66_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B71 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b71_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B71 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b71_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B85 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b85_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B85 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b85_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B86 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_rx0_lte_b86_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B86 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_9x05_tx0_lte_b86_device_info);  ret_val = TRUE; }

  return ret_val;
}

boolean rfc_sdr105_riot_9x05_lte_ag::timing_cfg_data_get( rfc_cfg_params_type *cfg, rfc_timing_info_type **ptr )
{

  boolean ret_val = FALSE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  if (NULL==cfg)
  {
    *ptr = NULL;
    return FALSE;
  }

  *ptr = NULL;

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B3 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B3 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B14 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B14 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B18 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B18 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B19 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B19 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B20 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B20 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B27 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B27 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B28 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B28 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B66 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B66 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B71 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B71 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B85 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B85 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B86 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B86 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  return ret_val;
}

boolean rfc_sdr105_riot_9x05_lte_ag::iotfe_timing_cfg_data_get( rfc_cfg_params_type *cfg, rfc_timing_info_type **ptr )
{

  boolean ret_val = FALSE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  if (NULL==cfg)
  {
    *ptr = NULL;
    return FALSE;
  }

  *ptr = NULL;

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B3 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B3 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B14 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B14 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B18 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B18 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B19 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B19 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B20 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B20 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B27 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B27 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B28 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B28 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B66 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B66 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B71 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B71 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B85 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B85 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B86 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_LTE_B86 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_iotfe_timing_info[0]);  ret_val = TRUE; }

  return ret_val;
}

boolean rfc_sdr105_riot_9x05_lte_ag::ca_scell_log_path_swap_data_get( rfc_band_pair_list_type **ptr )
{
  if (NULL==ptr)
  {
    return FALSE;
  }

  *ptr = NULL;

  return TRUE;
}

boolean rfc_sdr105_riot_9x05_lte_ag::band_split_cfg_data_get( rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr )
{
  boolean ret_val = FALSE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  if (NULL==cfg)
  {
    *ptr = NULL;
    return FALSE;
  }

  *ptr = NULL;

  return ret_val;
}

