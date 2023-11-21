
/*
WARNING: This file is auto-generated.

Generated using: rfc_autogen.exe
Generated from:  V5.9.415 of RFC_HWSWCD.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2017 Qualcomm Technologies Incorporated.  All Rights Reserved.

$Header: //components/rel/rfc_jolokia.mpss/2.10/rf_card/rfc_sdr105_riot_cls3_9x05/gsm/src/rfc_sdr105_riot_cls3_9x05_gsm_config_ag.cpp#4 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_sdr105_riot_cls3_9x05_gsm_config_ag.h" 
#include "rfc_sdr105_riot_cls3_9x05_cmn_ag.h" 
#include "rfc_common.h" 
#include "rfcom.h" 



extern "C" 
{
  extern rfc_device_info_type rf_card_sdr105_riot_cls3_9x05_init_gsm_rx_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_cls3_9x05_init_gsm_rx_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_cls3_9x05_init_gsm_tx_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_cls3_9x05_init_gsm_tx_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g850_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g850_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g850_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g850_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g900_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g900_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g900_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g900_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g1800_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g1800_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g1800_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g1800_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g1900_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g1900_sig_cfg;
  extern rfc_device_info_type rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g1900_device_info;
  extern rfc_sig_info_type rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g1900_sig_cfg;
} /* extern "C" */


rfc_gsm_data * rfc_sdr105_riot_cls3_9x05_gsm_ag::get_instance()
{
  if (rfc_gsm_data_ptr == NULL)
  {
    rfc_gsm_data_ptr = (rfc_gsm_data *)new rfc_sdr105_riot_cls3_9x05_gsm_ag();
  }
  return( (rfc_gsm_data *)rfc_gsm_data_ptr);
}

//constructor
rfc_sdr105_riot_cls3_9x05_gsm_ag::rfc_sdr105_riot_cls3_9x05_gsm_ag()
  :rfc_gsm_data()
{
}

boolean rfc_sdr105_riot_cls3_9x05_gsm_ag::sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr )
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
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_init_gsm_rx_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_init_gsm_tx_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM850 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g850_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM850 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g850_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM900 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g900_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM900 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g900_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM1800 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g1800_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM1800 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g1800_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM1900 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g1900_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM1900 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g1900_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  return ret_val;
}

boolean rfc_sdr105_riot_cls3_9x05_gsm_ag::devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr )
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
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_init_gsm_rx_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_init_gsm_tx_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM850 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g850_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM850 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g850_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM900 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g900_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM900 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g900_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM1800 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g1800_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM1800 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g1800_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM1900 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_rx0_gsm_g1900_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_GSM1900 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_sdr105_riot_cls3_9x05_tx0_gsm_g1900_device_info);  ret_val = TRUE; }

  return ret_val;
}

boolean rfc_sdr105_riot_cls3_9x05_gsm_ag::band_split_cfg_data_get( rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr )
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

extern "C"
{
  extern rfc_gsm_properties_type rfc_sdr105_riot_cls3_9x05_gsm_properties;
}

boolean rfc_sdr105_riot_cls3_9x05_gsm_ag::get_gsm_properties(rfc_gsm_properties_type **ptr)
{
  if (NULL==ptr)
  {
    return FALSE;
  }

  *ptr = &rfc_sdr105_riot_cls3_9x05_gsm_properties;
  return TRUE;
}


