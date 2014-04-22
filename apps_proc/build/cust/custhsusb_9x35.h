#ifndef CUSTHSUSB_H
#define CUSTHSUSB_H

/*===========================================================================

DESCRIPTION
Configuration for HS-USB. 9x35

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/build.tn/4.0/cust/custhsusb_9x35.h#1 $ $DateTime: 2018/06/25 01:51:12 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/27/13   jch     Initial Version

Copyright (c) 2013  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/

/* Enables HS-USB for the target 
*/
#define FEATURE_HS_USB
#define FEATURE_HS_USB_ECM
#define FEATURE_HS_USB_NMEA
#define FEATURE_HS_USB_SER3_PORT
#define FEATURE_HS_USB_SER4_PORT
#define FEATURE_HS_USB_CSVT_PORT
#define FEATURE_HS_USB_SMCT_PORT
#define FEATURE_HS_USB_QDSS_PORT
#define FEATURE_HS_USB_STANDARD_ECM
#ifdef FEATURE_QBI
  /* Enable MBIM USB composition */
  #define FEATURE_HS_USB_MBIM
  #define FEATURE_HS_USB_STANDARD_ECM
#endif /* FEATURE_QBI */

#endif /* CUSTHSUSB_H */
