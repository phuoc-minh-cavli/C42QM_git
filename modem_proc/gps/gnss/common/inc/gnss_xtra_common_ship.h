#ifndef GNSS_XTRA_COMMON_SHIP_H
#define GNSS_XTRA_COMMON_SHIP_H
/*=============================================================================

                             GNSS XTRA Common Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes 
  relevant for PE and SM to support XTRA. All the definitions and function
  prototypes are proprietary and shall not be released to external customers.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Copyright (c) 2017 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/common/inc/gnss_xtra_common_ship.h#1 $ 
  $DateTime: 2020/07/21 21:23:44 $ 
  $Author: pwbldsvc $ 



when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/17   rn      Add support for XTRA Integrity.
08/24/09   kc      Created
=============================================================================*/

#include "gps_variation.h"
#ifdef WIN32
#pragma once
#endif

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#include "customer.h"
#include "comdef.h"
#include "gps_types.h"
#include "gnss_consts.h"
#include "qw.h"
#include "cmd.h"
#include "gnss_common.h"

/* Version number of XTRA data */
typedef enum
{
  XTRA_VERSION_UNKNOWN = 0, /* XTRA data not populated */
  XTRA_VERSION_1 = 1,       /* XTRA1 */
  XTRA_VERSION_2 = 2,       /* XTRA2 */
  XTRA_VERSION_3 = 3,       /* XTRA3 */
  XTRA_VERSION_51 = 51,     /* XTRA Integrity */
                            /* Add new version here before XTRA_INVALID */
  XTRA_INVALID = 255        /* Memory is not allocated for XTRA */
} gnss_XtraVersionEnumType;

/*============================================================================
FUNCTION gnss_InitXtraMutexForDynLoading

DESCRIPTION
    Initialize XTRA Data mutex for 9205 platform supporting dynamic loading

    The XTRA data is commonly accessed by SM and CD. Due to its very large size,
    passing XTRA data through IPC messages is NOT efficient. Instead, mutex is
    used here. 
    This initialization function is called by CD during its startup because CD 
    is the one who accesses XTRA data earlier than all other GNSS components

PARAMETERS
    None

RETURN VALUE
    boolean - Mutex initialization status, TRUE: succeeded.

DEPENDENCIES
    gnssb_MutexCount --> Mutex used count
    gnssv_MutexInitialized --> Mutex intialized count
     

LIMITATIONS
=============================================================================*/
boolean gnss_InitXtraMutexForDynLoading(void);

/*=============================================================================

  FUNCTION      gnss_ResetCommonXtraInfo()

  DESCRIPTION   clear all data in the XTRA TOA cache

  DEPENDENCIES  TOA cache

  PARAMETERS: None
                        
  RETURN VALUE: None

  SIDE EFFECTS: None

=============================================================================*/
void gnss_ResetCommonXtraInfo(void);

#endif /* GNSS_XTRA_COMMON_SHIP_H */

