/*!
  @file
  ftm_unified_query_interface.c

  @brief
  Common framework to perform query RF related info in FTM mode
*/

/*======================================================================================================================

  Copyright (c) 2015 - 2022 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document are confidential 
  and proprietary information of Qualcomm Technologies Incorporated and all rights therein are 
  expressly reserved. By accepting this material the recipient agrees that this material and the 
  information contained therein are held in confidence and in trust and will not be used, copied, 
  reproduced in whole or in part, nor its contents revealed in any manner to others without the 
  express written permission of Qualcomm Technologies Incorporated.

======================================================================================================================*/

/*======================================================================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rffactory.mpss/1.0/src/ftm_unified_query_interface.c#26 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------------------------------------------
06/01/22   abr     Add support for mmW cal path info
01/10/19   abr     Add antenna module type information to HW connection table query's property list
11/08/18   ude     Adding LINKID and UL_TYPE properties to CA_CAPABILITY_QUERY command
10/28/18   sb      Adding paired beam ID prop for Subarray query
10/23/18   dpk     Adding Band, Channel and BW info for ACA response
10/18/18   abr     Define property type for sub-band frequency information in HW connection table query property list
06/01/18   WXL     FR 48638 - Request to return BCS and associated BW_MASK in CA Combo Query command response 
04/24/18   abr     Added properties to support v1 response of HW connection query
04/10/18   abr     Added Antenna Relative Positions properties to Sub Array query property list
03/07/18   abr     Remove properties representing number of L1/L2/L3 beams from Sub Array query property list
01/18/18   abr     Replace antenna feed mask with antenna feed pin in Sub-Array information query property list
01/03/18   abr     Add Beam List properties to Sub-Array information query property list
08/22/17   zhw     Added support for PLL ID in ACA command
08/14/17   abr     Update property type array for Sub-Array information
07/13/17   abr     Added Sub-Array information query support
11/09/16   zhw     Added CA combo query support
08/03/15   chm     Initial Release

======================================================================================================================*/

#include "comdef.h"
#include "rflm_defs.h"
#include "ftm_unified_query_interface.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Active Client Allocations Property Name Display Info
*/
const char *ftm_trm_aca_property_names[] = 
{
  "UNASSIGNED",               /* FTM_TRM_ACA_PROP_UNASSIGNED */
  "SUB_IDX",                  /* FTM_TRM_ACA_PROP_SUB_IDX */
  "TECH",                     /* FTM_TRM_ACA_PROP_TECH */
  "RXTX",                     /* FTM_TRM_ACA_PROP_RXTX */
  "CHAIN",                    /* FTM_TRM_ACA_PROP_CHAIN */
  "CARRIER_IDX",              /* FTM_TRM_ACA_PROP_CARRIER_IDX */
  "RFM_DEVICE",               /* FTM_TRM_ACA_PROP_DEVICE */
  "SIG_PATH",                 /* FTM_TRM_ACA_PROP_SIGNAL_PATH */
  "ANTENNA_PATH",             /* FTM_TRM_ACA_PROP_ANTENNA_PATH */
  "ANTENNA_NUM",              /* FTM_TRM_ACA_PROP_ANTENNA_NUM */
  "CURR_ASDIV_CFG",           /* FTM_TRM_ACA_PROP_CURR_ASDIV_CFG */
  "CURR_ASDIV_CAL",           /* FTM_TRM_ACA_PROP_CURR_CAL_STATE */
  "PEND_ASDIV_CFG",           /* FTM_TRM_ACA_PROP_PENDING_ASDIV_CFG */
  "PEND_ASDIV_CAL",           /* FTM_TRM_ACA_PROP_PENDING_CAL_STATE */
  "PLL_ID",                   /* FTM_TRM_ACA_PROP_PLL_ID */
  "BAND",                     /* FTM_TRM_ACA_PROP_BAND */
  "CHANNEL",                  /* FTM_TRM_ACA_PROP_CHANNEL */
  "BANDWIDTH",                /* FTM_TRM_ACA_PROP_BW */
};

COMPILE_ASSERT(sizeof(ftm_trm_aca_property_names)/sizeof(ftm_trm_aca_property_names[0]) == FTM_TRM_ACA_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  CA Combo query Property Name Display Info
*/
const char *ftm_ca_combo_query_property_names[] = 
{
  "UNASSIGNED",          /* FTM_CA_COMBO_QUERY_PROP_UNASSIGNED         */ 
  "TECH",                /* FTM_CA_COMBO_QUERY_PROP_TECH               */
  "TOTAL_COUNT",         /* FTM_CA_COMBO_QUERY_PROP_TOTAL_COMBO_COUNT  */
  "COMBO_IDX",           /* FTM_CA_COMBO_QUERY_PROP_COMBO_INDEX        */
  "ANT_LIMIT",           /* FTM_CA_COMBO_QUERY_PROP_ANTENNA_LIMIT      */
  "BAND",                /* FTM_CA_COMBO_QUERY_PROP_BAND               */
  "DL_BW_CLASS",         /* FTM_CA_COMBO_QUERY_PROP_DL_BW_CLASS        */
  "DL_ANT",              /* FTM_CA_COMBO_QUERY_PROP_DL_ANTENNAS        */
  "UL_BW_CLASS",         /* FTM_CA_COMBO_QUERY_PROP_UL_BW_CLASS        */
  "UL_ANT",              /* FTM_CA_COMBO_QUERY_PROP_UL_ANTENNAS        */
  "BW_PER_CARRIER",      /* FTM_CA_COMBO_QUERY_PROP_BW_PER_CARRIER     */
  "LINK_ID",             /* FTM_CA_COMBO_QUERY_PROP_LINK_ID            */
  "UL_TYPE",             /* FTM_CA_COMBO_QUERY_PROP_UL_TYPE            */
};

COMPILE_ASSERT(sizeof(ftm_ca_combo_query_property_names)/sizeof(ftm_ca_combo_query_property_names[0]) == FTM_CA_COMBO_QUERY_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  HW Connection query Property Name Display Info
*/
const char *ftm_hw_conn_query_property_names[] = 
{
  "UNASSIGNED",             /* FTM_HW_CONN_QUERY_PROP_UNASSIGNED */ 
  "BAND_NUMBER",            /* FTM_HW_CONN_QUERY_PROP_BAND */
  "SUBBAND_NUMBER",         /* FTM_HW_CONN_QUERY_PROP_SUBBAND */
  "RAW_RESULT",             /* FTM_HW_CONN_QUERY_PROP_RAW_DATA */
  "IF_TRX_INFO",            /* FTM_HW_CONN_QUERY_PROP_IF_TRX_INFO */
  "RFTRX_ANTMOD_INTF_INFO", /* FTM_HW_CONN_QUERY_PROP_RFTRX_ANTMOD_INTF_INFO */
  "ANT_MODULE_INFO",        /* FTM_HW_CONN_QUERY_PROP_ANT_MODULE_INFO */
  "SUBBAND_FREQ_INFO",      /* FTM_HW_CONN_QUERY_PROP_SUBBAND_FREQ_INFO */
  "ANT_MODULE_TYPE",        /* FTM_HW_CONN_QUERY_PROP_ANT_MODULE_TYPE */
  "TX_RX_CAL_FEED_INFO",    /* FTM_HW_CONN_QUERY_PROP_TXRX_CAL_FEED_INFO */
  "PLATFORM_TYPE",          /* FTM_HW_CONN_QUERY_PROP_PLATFORM_TYPE */
};

COMPILE_ASSERT(sizeof(ftm_hw_conn_query_property_names)/sizeof(ftm_hw_conn_query_property_names[0]) == FTM_HW_CONN_QUERY_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Sub Array query Property Name Display Info
*/
const char *ftm_sub_array_info_query_property_names[] = 
{
  "UNASSIGNED",            /* FTM_SUB_ARRAY_INFO_QUERY_PROP_UNASSIGNED */
  "SUB_ARRAY_ID",          /* FTM_SUB_ARRAY_INFO_QUERY_PROP_SUB_ARRAY_ID */   
  "RF_TRX_ID",             /* FTM_SUB_ARRAY_INFO_QUERY_PROP_RF_TRX_ID */      
  "ANTENNA_GROUP_ID",      /* FTM_SUB_ARRAY_INFO_QUERY_PROP_ANT_GRP_ID */     
  "ANTENNA_FEED_PIN",      /* FTM_SUB_ARRAY_INFO_QUERY_PROP_ANT_FEED_PIN */  
  "POLARIZATION_TYPE",     /* FTM_SUB_ARRAY_INFO_QUERY_PROP_POL_TYPE */       
  "BAND_MASK",             /* FTM_SUB_ARRAY_INFO_QUERY_PROP_BAND_MASK */      
  "AZIMUTH_START",         /* FTM_SUB_ARRAY_INFO_QUERY_PROP_AZIMUTH_START */  
  "AZIMUTH_STOP",          /* FTM_SUB_ARRAY_INFO_QUERY_PROP_AZIMUTH_STOP */   
  "ELEVATION_START",       /* FTM_SUB_ARRAY_INFO_QUERY_PROP_ELEVATION_START */
  "ELEVATION_STOP",        /* FTM_SUB_ARRAY_INFO_QUERY_PROP_ELEVATION_STOP */ 
  "BEAM_SUB_ID",           /* FTM_SUB_ARRAY_INFO_QUERY_PROP_BEAM_ID */ 
  "BEAM_SUB_BAND",         /* FTM_SUB_ARRAY_INFO_QUERY_PROP_BEAM_SUB_BAND */ 
  "BEAM_LIST_QUERY_FLAG",  /* FTM_SUB_ARRAY_INFO_QUERY_PROP_BEAM_LIST_QUERY_FLAG */ 
  "ANT_FEED_X_COORDINATE", /* FTM_SUB_ARRAY_INFO_QUERY_PROP_ANT_FEED_X_COORDINATE */
  "ANT_FEED_Y_COORDINATE", /* FTM_SUB_ARRAY_INFO_QUERY_PROP_ANT_FEED_Y_COORDINATE */
  "ANT_FEED_Z_COORDINATE", /* FTM_SUB_ARRAY_INFO_QUERY_PROP_ANT_FEED_Z_COORDINATE */
  "PAIRED_BEAM_ID",        /* FTM_SUB_ARRAY_INFO_QUERY_PROP_PAIRED_BEAM_ID */
  "ZEB_ID",                /* FTM_SUB_ARRAY_INFO_QUERY_PROP_ZEB_ID */
  "NUM_ACTIVE_ELEMENTS_PER_BEAM",   /* FTM_SUB_ARRAY_INFO_QUERY_PROP_NUM_ACTIVE_ELEMENTS_PER_BEAM */
  "REF_BEAM_LIST_QUERY_FLAG", /* FTM_SUB_ARRAY_INFO_QUERY_PROP_REF_BEAM_LIST_QUERY_FLAG */
  "REF_BEAM_ID_LIST", /* FTM_SUB_ARRAY_INFO_QUERY_PROP_REF_BEAM_ID */
  "REF_BEAM_RF_TRX_MASK", /* FTM_SUB_ARRAY_INFO_QUERY_PROP_REF_BEAM_RF_TRX_MASK */
};

COMPILE_ASSERT(sizeof(ftm_sub_array_info_query_property_names)/sizeof(ftm_sub_array_info_query_property_names[0]) == FTM_SUB_ARRAY_INFO_QUERY_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  mmw calibration path information query Property Name Display Info
*/
const char *ftm_mmw_cal_path_info_query_property_names[] = 
{
  "UNASSIGNED",            /* FTM_MMW_CAL_PATH_INFO_QUERY_PROP_UNASSIGNED */
  "BAND_NUMBER",           /* FTM_MMW_CAL_PATH_INFO_QUERY_PROP_BAND */
  "PATH_ID",               /* FTM_MMW_CAL_PATH_INFO_QUERY_PROP_PATH_ID */
  "ANTENNA_GROUP_ID",      /* FTM_MMW_CAL_PATH_INFO_QUERY_PROP_ANT_GRP_ID */
  "RFM_DEVICE",			   /* FTM_MMW_CAL_PATH_INFO_QUERY_PROP_RFM_DEVICE */
  "PATH_TYPE",			   /* FTM_MMW_CAL_PATH_INFO_QUERY_PROP_PATH_TYPE */
  "ASSOCIATED_TX_PATH_ID", /* FTM_MMW_CAL_PATH_INFO_QUERY_PROP_ASSOC_TX_PATH_ID */
  "ASSOCIATED_RX_PATH_ID", /* FTM_MMW_CAL_PATH_INFO_QUERY_PROP_ASSOC_RX_PATH_ID */				 
};

COMPILE_ASSERT(sizeof(ftm_mmw_cal_path_info_query_property_names)/sizeof(ftm_mmw_cal_path_info_query_property_names[0]) == FTM_MMW_CAL_PATH_INFO_QUERY_PROP_NUM);
