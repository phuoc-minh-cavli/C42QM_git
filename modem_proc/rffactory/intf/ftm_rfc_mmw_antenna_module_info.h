#ifndef FTM_RFC_MMW_ANTENNA_MODULE_INFO_H
#define FTM_RFC_MMW_ANTENNA_MODULE_INFO_H
/*!
  @file
  ftm_rfc_mmw_antenna_module_info.h

  @brief
  This file contains the antenna module enum definitions for 5g mmw rfc data.
*/

/*==============================================================================
  Copyright (c) 2021-2022 Qualcomm Technologies, Inc. All Rights Reserved.

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

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rffactory.mpss/1.0/intf/ftm_rfc_mmw_antenna_module_info.h#21 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/24/23   asn     Add QTM567_PC5_* modules
02/15/23   asn     Add QTM567_*_V1 modules
02/03/23   asn     Add QTM565_*_V2 modules
11/30/22   asn     Add QTM565_2 modules
06/22/22   asn     Add QTM547_V2_HB_V2
05/03/22   dyc     Add QTM555_1_V2
02/24/22   dyc     Add QTM555_1 antenna modules
01/12/22   asn     Add QTM547_V2*_V2 ant module
12/16/21   dyc     Add QTM555 antenna modules
09/08/21   asn     Add new antenna modules
05/10/21   dyc     Added QTM545_* V2 modules
04/27/21   abr     Initial Revision.
==============================================================================*/

/*!
  @brief
  This enum defines the antenna module device type.
 
  ***** NOTE - External tools dependency for ordering.
  *****        MUST ADD antenna modules to the END of the list.
*/
typedef enum
{
  RFC_ANT_MODULE_QTM052_3,
  RFC_ANT_MODULE_QTM052_5,
  RFC_ANT_MODULE_QTM052_6,
  RFC_ANT_MODULE_QTM052_6X,
  RFC_ANT_MODULE_QTM052_7L,
  RFC_ANT_MODULE_QTM052_8L,
  RFC_ANT_MODULE_QTM052_8R,
  RFC_ANT_MODULE_QTM052_8X,

  RFC_ANT_MODULE_QTM525_2,
  RFC_ANT_MODULE_QTM525_4,
  RFC_ANT_MODULE_QTM525_5,
  RFC_ANT_MODULE_QTM525_6,
  RFC_ANT_MODULE_QTM525_5BA,
  RFC_ANT_MODULE_QTM525_6BA,
  RFC_ANT_MODULE_QTM527_1,
  RFC_ANT_MODULE_QTM527_2,
  RFC_ANT_MODULE_QTM525_2_V2,
  RFC_ANT_MODULE_QTM525_5_V2,
  RFC_ANT_MODULE_QTM525_5_LOW,
  RFC_ANT_MODULE_QTM525_5_HIGH,
  RFC_ANT_MODULE_QTM525_6_LOW,
  RFC_ANT_MODULE_QTM525_6_HIGH,
  RFC_ANT_MODULE_TANGIER,
  RFC_ANT_MODULE_TANGIER_LOW,
  RFC_ANT_MODULE_TANGIER_HIGH,
  RFC_ANT_MODULE_QTM525_6_V2,
  RFC_ANT_MODULE_QTM527_1_V2,
  RFC_ANT_MODULE_QTM527_2_V2,
  RFC_ANT_MODULE_TANGIER_V2,
  RFC_ANT_MODULE_TANGIER_V3,
  RFC_ANT_MODULE_FEZ,
  RFC_ANT_MODULE_QTM535_2,
  RFC_ANT_MODULE_QTM535_2BA,
  RFC_ANT_MODULE_QTM535_5BA,
  RFC_ANT_MODULE_QTM525_5_V2_1,
  RFC_ANT_MODULE_QTM535_5,
  RFC_ANT_MODULE_QTM527_2JP_V2,
  RFC_ANT_MODULE_QTM535_7BA,
  RFC_ANT_MODULE_QTM535_8BA,
  RFC_ANT_MODULE_TANGIER_MOD1,
  RFC_ANT_MODULE_HORNET,
  RFC_ANT_MODULE_TANGIER_MOD1_LOW,
  RFC_ANT_MODULE_TANGIER_MOD1_HIGH,
  RFC_ANT_MODULE_TANGIER_MOD2_LOW,
  RFC_ANT_MODULE_TANGIER_MOD2_HIGH,
  RFC_ANT_MODULE_HORNET_LOW,
  RFC_ANT_MODULE_HORNET_HIGH,
  RFC_ANT_MODULE_QTM545_1_V1,
  RFC_ANT_MODULE_QTM545_1_V1_LB,
  RFC_ANT_MODULE_QTM545_1_V1_HB,
  RFC_ANT_MODULE_TANGIER_MOD3_LOW,
  RFC_ANT_MODULE_TANGIER_MOD3_HIGH,
  RFC_ANT_MODULE_TANGIER_MOD4_LOW,
  RFC_ANT_MODULE_TANGIER_MOD4_HIGH,
  RFC_ANT_MODULE_CASSATA_DEV1_HB,
  RFC_ANT_MODULE_CASSATA_DEV1_LB,
  RFC_ANT_MODULE_SERNIK_DEV1_HB,
  RFC_ANT_MODULE_SERNIK_DEV1_LB,
  RFC_ANT_MODULE_QTM547_V1_LB,
  RFC_ANT_MODULE_QTM547_V1_HB,
  RFC_ANT_MODULE_QTM545_2_V1_LB,
  RFC_ANT_MODULE_QTM545_2_V1_HB,
  RFC_ANT_MODULE_QTM545_1_V2_LB,
  RFC_ANT_MODULE_QTM545_1_V2_HB,  
  RFC_ANT_MODULE_QTM545_2_V2_LB,
  RFC_ANT_MODULE_QTM545_2_V2_HB,  
  RFC_ANT_MODULE_SERNIK_DEV2_HB,
  RFC_ANT_MODULE_SERNIK_DEV2_LB,
  RFC_ANT_MODULE_HANDEL_HB,
  RFC_ANT_MODULE_HANDEL_LB,
  RFC_ANT_MODULE_BACH_HB,
  RFC_ANT_MODULE_BACH_LB,
  RFC_ANT_MODULE_CASSATA_DEV3_HB,
  RFC_ANT_MODULE_CASSATA_DEV3_LB,
  RFC_ANT_MODULE_SERNIK_DEV3_HB,
  RFC_ANT_MODULE_SERNIK_DEV3_LB,
  RFC_ANT_MODULE_CASSATA_TYPE1_HB,
  RFC_ANT_MODULE_CASSATA_TYPE1_LB,
  RFC_ANT_MODULE_CASSATA_TYPE2_HB,
  RFC_ANT_MODULE_CASSATA_TYPE2_LB,
  RFC_ANT_MODULE_SERNIK_HB,
  RFC_ANT_MODULE_SERNIK_LB,
  RFC_ANT_MODULE_QTM547_V2_LB,
  RFC_ANT_MODULE_QTM547_V2_HB,
  RFC_ANT_MODULE_QTM555_V1_LB,
  RFC_ANT_MODULE_QTM555_V1_HB,
  RFC_ANT_MODULE_QTM547_V2_LB_V2,
  RFC_ANT_MODULE_QTM555_1_V1_LB,
  RFC_ANT_MODULE_QTM555_1_V1_HB,
  RFC_ANT_MODULE_QTM565_1_LB,
  RFC_ANT_MODULE_QTM565_1_HB,
  RFC_ANT_MODULE_QTM555_1_V2_LB,
  RFC_ANT_MODULE_QTM555_1_V2_HB,
  RFC_ANT_MODULE_QTM547_V2_HB_V2,
  RFC_ANT_MODULE_QTM565_2_LB,
  RFC_ANT_MODULE_QTM565_2_HB,
  RFC_ANT_MODULE_QTM565_1_V2_LB,
  RFC_ANT_MODULE_QTM565_1_V2_HB,
  RFC_ANT_MODULE_QTM565_2_V2_LB,
  RFC_ANT_MODULE_QTM565_2_V2_HB,
  RFC_ANT_MODULE_QTM567_V1_LB,
  RFC_ANT_MODULE_QTM567_V1_HB,
  RFC_ANT_MODULE_QTM567_LB,
  RFC_ANT_MODULE_QTM567_HB,
  RFC_ANT_MODULE_QTM567_PC5_LB,
  RFC_ANT_MODULE_QTM567_PC5_HB,
  RFC_ANT_MODULE_ENUM_MAX, /* end point for enum list */
  RFC_ANT_MODULE_INVALID = 0xFF
} rfc_ant_module_device_enum_type;

#endif /* FTM_RFC_MMW_ANTENNA_MODULE_INFO_H */
