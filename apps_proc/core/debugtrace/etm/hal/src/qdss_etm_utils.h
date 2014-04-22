#ifndef QDSS_ETM_UTILS_H
#define QDSS_ETM_UTILS_H

/*=============================================================================

FILE:         qdss_etm_utils.h

DESCRIPTION:

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/

#include "qdss_throw.h"
#include "qdss_utils.h"

#define bit(n)       (1 << (n))
#define bit_mask(lsb, msb)     ( bit(msb) | (bit(msb)-1) &  ~(bit(lsb)-1) )
#define bit_field_value(val, lsb, msb) ((val & bit_mask(lsb, msb)) >> lsb)
#define bit_value(val, n)     ((val & bit(n)) >> n)



#endif //QDSS_ETM_UTILS_H
