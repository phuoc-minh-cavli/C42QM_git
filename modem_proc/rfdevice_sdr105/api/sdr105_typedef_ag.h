#ifndef SDR105_TYPEDEF_AG_H
#define SDR105_TYPEDEF_AG_H
/*
  WARNING: This file is auto-generated.

  Generated at:    0
  Generated using: AutoGenerate.pl
  Generated from:  v60 of the hubble spreadsheet
*/

/*!
  @file
  sdr105_typedef_ag.h

  @brief
  External type definitions

  @details
  This file is auto-generated and it contains external enum types to support the interaction with the QUALCOMM sdr105 chip
The enumerations in this file are exposed to RF card factory to enable configuration of the device by customer.


  @addtogroup Common Mapping Table
  @{
*/

/*=============================================================================
  Copyright (c) 2015-2016 by QUALCOMM Technologies, Inc.  All Rights Reserved.

  Qualcomm Technologies Proprietary and Confidential

  Export of this technology or software is regulated by the U.S. 
  Government. Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //components/rel/rfdevice_sdr105.mpss/1.10/api/sdr105_typedef_ag.h#9 $
  $DateTime: 2023/10/17 23:12:52 $
  $Author: mplcsds1 $
=============================================================================*/

/*========================= INCLUDE FILES FOR MODULE ========================*/
#include "comdef.h" 
#include "rfdevice_trx_typedef_ag.h" 

/*===========================================================================*/
  

typedef enum
{
  SDR105_GSM_BAND1800_RX_MB_GSM = 0,
  SDR105_GSM_BAND1900_RX_MB_GSM,
  SDR105_GSM_BAND850_RX_LB_GSM,
  SDR105_GSM_BAND900_RX_LB_GSM,
  SDR105_GSM_RX_RFC_PORT_COMMON_NUM,
  SDR105_GSM_RX_RFC_PORT_COMMON_INVALID
} sdr105_gsm_rx_rfc_port_common_type;

typedef enum
{
  SDR105_GSM_BAND1800_TX_MB2_GSM = 0,
  SDR105_GSM_BAND1900_TX_MB2_GSM,
  SDR105_GSM_BAND850_TX_LB2_GSM,
  SDR105_GSM_BAND900_TX_LB2_GSM,
  SDR105_GSM_TX_RFC_PORT_COMMON_NUM,
  SDR105_GSM_TX_RFC_PORT_COMMON_INVALID
} sdr105_gsm_tx_rfc_port_common_type;

typedef enum
{
  SDR105_LTE_BAND12_LB_LNA_IN = 0,
  SDR105_LTE_BAND13_LB_LNA_IN,
  SDR105_LTE_BAND14_LB_LNA_IN,
  SDR105_LTE_BAND17_LB_LNA_IN,
  SDR105_LTE_BAND18_LB_LNA_IN,
  SDR105_LTE_BAND19_LB_LNA_IN,
  SDR105_LTE_BAND1_MB_LNA_IN,
  SDR105_LTE_BAND20_LB_LNA_IN,
  SDR105_LTE_BAND23_MB_LNA_IN,
  SDR105_LTE_BAND255_GNSS_IN,
  SDR105_LTE_BAND256_MB_LNA_IN,
  SDR105_LTE_BAND25_MB_LNA_IN,
  SDR105_LTE_BAND26_LB_LNA_IN,
  SDR105_LTE_BAND27_LB_LNA_IN,
  SDR105_LTE_BAND28_LB_LNA_IN,
  SDR105_LTE_BAND2_MB_LNA_IN,
  SDR105_LTE_BAND31_RX_LB_AUX,
  SDR105_LTE_BAND39_MB_LNA_IN,
  SDR105_LTE_BAND3_MB_LNA_IN,
  SDR105_LTE_BAND4_MB_LNA_IN,
  SDR105_LTE_BAND5_LB_LNA_IN,
  SDR105_LTE_BAND66_MB_LNA_IN,
  SDR105_LTE_BAND71_RX_LB_AUX,
  SDR105_LTE_BAND72_RX_LB_AUX,
  SDR105_LTE_BAND73_RX_LB_AUX,
  SDR105_LTE_BAND85_LB_LNA_IN,
  SDR105_LTE_BAND86_LB_LNA_IN,
  SDR105_LTE_BAND87_RX_LB_AUX,
  SDR105_LTE_BAND88_RX_LB_AUX,
  SDR105_LTE_BAND8_LB_LNA_IN,
  SDR105_LTE_RX_RFC_PORT_COMMON_NUM,
  SDR105_LTE_RX_RFC_PORT_COMMON_INVALID
} sdr105_lte_rx_rfc_port_common_type;

typedef enum
{
  SDR105_LTE_BAND12_TX_LB1_LTE = 0,
  SDR105_LTE_BAND12_TX_LB2_GSM,
  SDR105_LTE_BAND13_TX_LB1_LTE,
  SDR105_LTE_BAND13_TX_LB2_GSM,
  SDR105_LTE_BAND14_TX_LB1_LTE,
  SDR105_LTE_BAND14_TX_LB2_GSM,
  SDR105_LTE_BAND17_TX_LB1_LTE,
  SDR105_LTE_BAND17_TX_LB2_GSM,
  SDR105_LTE_BAND18_TX_LB1_LTE,
  SDR105_LTE_BAND18_TX_LB2_GSM,
  SDR105_LTE_BAND19_TX_LB1_LTE,
  SDR105_LTE_BAND19_TX_LB2_GSM,
  SDR105_LTE_BAND1_TX_MB1_LTE,
  SDR105_LTE_BAND1_TX_MB2_GSM,
  SDR105_LTE_BAND20_TX_LB1_LTE,
  SDR105_LTE_BAND20_TX_LB2_GSM,
  SDR105_LTE_BAND23_TX_MB1_LTE,
  SDR105_LTE_BAND23_TX_MB2_GSM,
  SDR105_LTE_BAND255_TX_MB1_LTE,
  SDR105_LTE_BAND255_TX_MB2_GSM,
  SDR105_LTE_BAND256_TX_MB1_LTE,
  SDR105_LTE_BAND256_TX_MB2_GSM,
  SDR105_LTE_BAND25_TX_MB1_LTE,
  SDR105_LTE_BAND25_TX_MB2_GSM,
  SDR105_LTE_BAND26_TX_LB1_LTE,
  SDR105_LTE_BAND26_TX_LB2_GSM,
  SDR105_LTE_BAND27_TX_LB1_LTE,
  SDR105_LTE_BAND27_TX_LB2_GSM,
  SDR105_LTE_BAND28_TX_LB1_LTE,
  SDR105_LTE_BAND28_TX_LB2_GSM,
  SDR105_LTE_BAND2_TX_MB1_LTE,
  SDR105_LTE_BAND2_TX_MB2_GSM,
  SDR105_LTE_BAND31_TX_LB3_AUX,
  SDR105_LTE_BAND39_TX_MB1_LTE,
  SDR105_LTE_BAND39_TX_MB2_GSM,
  SDR105_LTE_BAND3_TX_MB1_LTE,
  SDR105_LTE_BAND3_TX_MB2_GSM,
  SDR105_LTE_BAND4_TX_MB1_LTE,
  SDR105_LTE_BAND4_TX_MB2_GSM,
  SDR105_LTE_BAND5_TX_LB1_LTE,
  SDR105_LTE_BAND5_TX_LB2_GSM,
  SDR105_LTE_BAND66_TX_MB1_LTE,
  SDR105_LTE_BAND66_TX_MB2_GSM,
  SDR105_LTE_BAND71_TX_LB1_LTE,
  SDR105_LTE_BAND71_TX_LB2_GSM,
  SDR105_LTE_BAND72_TX_LB3_AUX,
  SDR105_LTE_BAND73_TX_LB3_AUX,
  SDR105_LTE_BAND85_TX_LB1_LTE,
  SDR105_LTE_BAND85_TX_LB2_GSM,
  SDR105_LTE_BAND86_TX_LB1_LTE,
  SDR105_LTE_BAND86_TX_LB2_GSM,
  SDR105_LTE_BAND87_TX_LB3_AUX,
  SDR105_LTE_BAND88_TX_LB3_AUX,
  SDR105_LTE_BAND8_TX_LB1_LTE,
  SDR105_LTE_BAND8_TX_LB2_GSM,
  SDR105_LTE_TX_RFC_PORT_COMMON_NUM,
  SDR105_LTE_TX_RFC_PORT_COMMON_INVALID
} sdr105_lte_tx_rfc_port_common_type;

typedef enum
{
  SDR105_GNSS_LNA_IN,
  SDR105_GNSS_MB_LNA_IN,
  SDR105_GNSS_RFC_PORT_COMMON_NUM,
  SDR105_GNSS_RFC_PORT_COMMON_INVALID
}sdr105_gnss_rfc_port_common_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the sdr105_lut_pwr_data_type enum.
*/
typedef enum 
{
  SDR105_LP_LUT_TYPE,  /* Low Power Tx AGC LUT */
  SDR105_HP_LUT_TYPE,  /* High Power Tx AGC LUT */
  SDR105_LUT_PWR_NUM,  /* Number of Tx AGC LUT options */
  SDR105_LUT_PWR_INVALID,  /* Indicator for invalid LUT type */
} sdr105_lut_pwr_data_type;


/*----------------------------------------------------------------------------*/
/*!
   It defines the sdr105_catm1_lna_gain_state_type enum.
*/
typedef enum 
{
  SDR105_CATM1_LNA_GAIN_STATE_0, 
  SDR105_CATM1_LNA_GAIN_STATE_1, 
  SDR105_CATM1_LNA_GAIN_STATE_2, 
  SDR105_CATM1_LNA_GAIN_STATE_3, 
  SDR105_CATM1_LNA_GAIN_STATE_4, 
  SDR105_CATM1_LNA_GAIN_STATE_5, 
  SDR105_CATM1_LNA_GAIN_STATE_6, 
  SDR105_CATM1_LNA_GAIN_STATE_7, 
  SDR105_CATM1_LNA_GAIN_STATE_8, 
  SDR105_CATM1_LNA_GAIN_STATE_NUM, 
  SDR105_CATM1_LNA_GAIN_STATE_INVALID = 0xF, 
} sdr105_catm1_lna_gain_state_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the sdr105_gsm_lna_gain_state_type enum.
*/
typedef enum 
{
  SDR105_GSM_LNA_GAIN_STATE_0, 
  SDR105_GSM_LNA_GAIN_STATE_1, 
  SDR105_GSM_LNA_GAIN_STATE_2, 
  SDR105_GSM_LNA_GAIN_STATE_3, 
  SDR105_GSM_LNA_GAIN_STATE_4, 
  SDR105_GSM_LNA_GAIN_STATE_NUM, 
  SDR105_GSM_LNA_GAIN_STATE_INVALID = 0xF, 
} sdr105_gsm_lna_gain_state_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the sdr105_nb1_lna_gain_state_type enum.
*/
typedef enum 
{
  SDR105_NB1_LNA_GAIN_STATE_0, 
  SDR105_NB1_LNA_GAIN_STATE_1, 
  SDR105_NB1_LNA_GAIN_STATE_2, 
  SDR105_NB1_LNA_GAIN_STATE_3, 
  SDR105_NB1_LNA_GAIN_STATE_4, 
  SDR105_NB1_LNA_GAIN_STATE_5, 
  SDR105_NB1_LNA_GAIN_STATE_6, 
  SDR105_NB1_LNA_GAIN_STATE_7, 
  SDR105_NB1_LNA_GAIN_STATE_8, 
  SDR105_NB1_LNA_GAIN_STATE_NUM, 
  SDR105_NB1_LNA_GAIN_STATE_INVALID = 0xF, 
} sdr105_nb1_lna_gain_state_type;


#endif /* SDR105_TYPEDEF_AG_H */

/*! @} */ 
