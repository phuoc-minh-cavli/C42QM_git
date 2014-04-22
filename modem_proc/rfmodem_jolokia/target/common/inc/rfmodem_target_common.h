#ifndef RFMODEM_TARGET_COMMON_H
#define RFMODEM_TARGET_COMMON_H

/*!
   @file
   rfmodem_target_dac_ctl.h

   @brief
   This file implements the Target Specific DAC specifications. These are platform
   dependent.

*/

/*===========================================================================

Copyright (c) 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2020/04/16 12:27:55 $ $Author: pwbldsvc $
$Header: //components/rel/rfmodem_jolokia.mpss/1.10/target/common/inc/rfmodem_target_common.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
04/17/15   dej     Initial version.
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#ifdef _cplusplus
extern "C" {
#endif

typedef enum {
   RFMODEM_TARGET_MSM8909 = 0,
   RFMODEM_TARGET_CHIPSET_ID_MAX,
   RFMODEM_TARGET_CHIPSET_ID_INVALID = RFMODEM_TARGET_CHIPSET_ID_MAX
}rfmodem_chipset_id_enum_type;




#ifdef _cplusplus
}
#endif

#endif /* RFMODEM_TARGET_COMMON_H */

