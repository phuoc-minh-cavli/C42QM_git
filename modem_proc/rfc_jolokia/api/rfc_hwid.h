#ifndef RFC_HWID_H
#define RFC_HWID_H

/*===========================================================================


      R F  D r i v e r  C o m m o n  H a r d w a r e  I d e n t i f i e r

                            H e a d e r  F i l e

DESCRIPTION
  This file contains the Hardware IDs for RF Cards used across multiple
  layers of the RF driver.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 1999 - 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved .
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rfc_jolokia.mpss/2.10/api/rfc_hwid.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/15   fhuo    Added support for RFC_WTR2955_CHINA_CMCC_5M_DLCA
03/19/15   rp      Added Dummy HW IDs to resolve compile time dependency on ULCA changes.
04/23/14   sd      Initial revision for DimePM 2.0 RFC HW IDs.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* -------------------------------------------------------
** The RF Card Id used in the target
** Note: The Id needs to be sequential
** ------------------------------------------------------- */
typedef enum {
  RF_HW_UNDEFINED                         = (uint32)0,
  RF_TARGET_NONE                          = RF_HW_UNDEFINED,
  RF_HW_SDR105_RIOT                       = (uint32)417,
  RF_HW_SDR105_RIOT_CLS3                  = (uint32)475,
  RF_HW_SDR105_RIOT_9X05                  = (uint32)538,
  RF_HW_SDR105_RIOT_CLS3_9X05             = (uint32)539,
  RF_HW_SDR105_RIOT_9X05_GSM              = (uint32)541,
  RF_HW_SDR105_RIOT_9X05_GNSS             = (uint32)553,
  /* Add any new HW ID before this line */
  RF_HW_MAX,
  RF_HW_DEFAULT = RF_HW_MAX, /* Default card for Off-Target Builds */
  RF_HW_EFS_CARD                          = (uint32)191, /* EFS Card */

} rf_hw_type;

#ifdef __cplusplus
}
#endif

#endif  /* RFC_HWID_H */
