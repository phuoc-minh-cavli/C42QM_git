/*===========================================================================
                        mcfg_feature_config.h

DESCRIPTION

   Feature config supprot for the MCFG module
 
Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/api/mcfg_feature_config.h#3 $ $DateTime: 2022/05/25 07:59:49 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
1/18/12      sbt   Create

===========================================================================*/

#ifndef MODEM_CFG_FEATURE_CONFIG_H
#define MODEM_CFG_FEATURE_CONFIG_H

#include "comdef.h"

/* Mux support */
#define FEATURE_MODEM_CFG_MUX_SUPPORT

/* --------------------------------------------------------- */
//#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
   /* All MCFG libraries are not compiled for PAGEONLY image.
      Hence do not enable EFS log for PAGEONLY image */
   //#define MCFG_DEBUG_EFS_LOG_SUPPORT
//#endif
/* --------------------------------------------------------- */

//#define FEATURE_MCFG_ACTIVATE_W_NO_RESET

/* Disable default config when NV is not set */
// #define FEATURE_MCFG_DISABLE_DEFAULT_CONFIG_W_NV_INACTIVE

/* For Auto IMSI Selection */
#define FEATURE_MCFG_SELECTION_BY_IMSI 

/* For Multi-SIM support */
#define FEATURE_MCFG_MULTISIM_SUPPORT 
/* For 1x IMSI based autoselection support (i.e of CT card) */
//#define FEATURE_MCFG_SELECTION_BY_1X_IMSI

/* Enforce HW-SW Config activation sequency, comment it if HW config is not required */
//#define FEATURE_MCFG_HW_CONFIG_REQUIRED

/* For Modem inititated graceful rest */
#define FEATURE_MCFG_SSREQ

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  #define FEATURE_MCFG_REFRESH
#endif

/* For MCFG FS API Support */
#define FEATURE_MCFG_FS_API_SUPPORT

#ifdef MCFG_DEBUG
  #define FEATURE_MCFG_DIAG_SUPPORT_EXTENDED
#endif

#endif /* MODEM_CFG_FEATURE_CONFIG_H */
