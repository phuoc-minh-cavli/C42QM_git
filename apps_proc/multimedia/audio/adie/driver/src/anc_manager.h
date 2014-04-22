#ifndef WCD_ANC_MANAGER_INTF_
#define WCD_ANC_MANAGER_INTF_

/**
@file anc_manager.h
@brief 
External WCD ANC Manager
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
    All rights reserved.
    Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                        Edit History
$Header: //source/qcom/qct/multimedia2/Audio/drivers/hw/Adie/AdieCodec/
			Common3/main/latest/inc/anc_manager.h $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/15/10   SK      Initial revision
===============================================================================*/

#include "wcd_utility.h"
#include "wcd_types.h"

#define MAX_INT                       2147483647
#define WCD_ANC_COEFF_INT_BITS        3
#define WCD_ANC_COEFF_FRAC_BITS       13
#define WCD_ANC_GAIN_TABLE_OFFSET     12
#define WCD_ANC_GAIN_TABLE_FRAC_BITS  13

typedef struct
{
  int32 gain;
  uint32 multiplier;
} WCD_ANC_Gain_Table_Type;

#define WCD_ANC_PRIMARY_CHANNEL 1
#define WCD_ANC_MAX_CHANNELS    2

/**
 * \brief Enables ANC using the supplied ANC configuration. Also reserves the input elements
 * along with the micbias managers (needed for ANC) inside the usecase
 */
wcd_result wcd_enable_anc(wcd_usecase* usecase, wcd_adie_codec_db_anc_config* anc_configuration);

/**
 * \brief Disables ANC from the usecase
 */
wcd_result wcd_disable_anc(wcd_usecase* usecase);

#endif


