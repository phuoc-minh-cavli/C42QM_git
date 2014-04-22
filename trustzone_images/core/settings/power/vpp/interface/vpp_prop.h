#ifndef VPP_PROP_H
#define VPP_PROP_H
/*===========================================================================

  Copyright (c) 2018 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  $Header: 

  ===========================================================================*/

/*===========================================================================
Edit History

when       who        what, where, why
--------   ---        ----------------------------------------------------------
05/16/18   myun       Initial revision
===========================================================================*/
/******************************************************************************
                                INCLUDE FILES
******************************************************************************/
#include <stddef.h>
#include "comdef.h"

typedef struct
{
    uint32 freq_khz;
    int16  uplift_mv;
    uint8  entry_remove;
} vpp_prop_t;

typedef struct
{
    uint8 acd_enable;
    uint8 acd_margin_enable;
    uint8 aging_enable;
    uint8 aging_margin_enable;
    uint8 avg_enable;
    uint8 apm_auto_enable;
} vpp_prop_feature_t;

#endif /* VPP_PROP_H */
