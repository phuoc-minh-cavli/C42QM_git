#ifndef __GNSS_NV_LOADER_H__
#define __GNSS_NV_LOADER_H__
/*============================================================================
  @file gnss_nv_loader.h

                  GNSS NV Loader Definitions 

GENERAL DESCRIPTION

  This file containts various NV definitions for GNSS NV Loader module

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

            Copyright (c) 2013-2014 QUALCOMM ATHEROS, INCORPORATED.
            All Rights Reserved
            Qualcomm Confidential and Proprietary

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/common/inc/gnss_nv_loader.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/30/14   tf      Change default URLS for XTRA2 to cloud-hosted servers
11/06/13   rh      Changed default URLs to XTRA3 if FEATURE_GNSS_XTRA3 defined 
10/07/13   rh      Initial version

=============================================================================*/

/* ---------------------------------------------------------------------------
 * Include files
 * ---------------------------------------------------------------------------*/
#include "customer.h"
#include "comdef.h"

/* Default XTRA file URL */
#ifdef FEATURE_GNSS_XTRA3
#define NV_AAGPS_DEFAULT_XTRA_PRIMARY_SERVER_URL        "http://iot1.xtracloud.net/xtra3gr.bin"
#define NV_AAGPS_DEFAULT_XTRA_SECONDARY_SERVER_URL      "http://iot2.xtracloud.net/xtra3gr.bin"
#define NV_AAGPS_DEFAULT_XTRA_TERTIARY_SERVER_URL       "http://iot3.xtracloud.net/xtra3gr.bin"
#else /* FEATURE_GNSS_XTRA3 */
#define NV_AAGPS_DEFAULT_XTRA_PRIMARY_SERVER_URL        "http://xtrapath1.izatcloud.net/xtra2.bin"
#define NV_AAGPS_DEFAULT_XTRA_SECONDARY_SERVER_URL      "http://xtrapath2.izatcloud.net/xtra2.bin"
#define NV_AAGPS_DEFAULT_XTRA_TERTIARY_SERVER_URL       "http://xtrapath3.izatcloud.net/xtra2.bin"
#endif /* FEATURE_GNSS_XTRA3 */

/* Default XTRA SNTP time server URL */
#ifdef FEATURE_GNSS_XTRA3
#define  NV_AAGPS_DEFAULT_XTRA_PRIMARY_SNTP_SERVER_URL          "time.xtracloud.net"
#define  NV_AAGPS_DEFAULT_XTRA_SECONDARY_SNTP_SERVER_URL        "time.xtracloud.net"
#define  NV_AAGPS_DEFAULT_XTRA_TERTIARY_SNTP_SERVER_URL         "time.xtracloud.net"
#else /* FEATURE_GNSS_XTRA3 */
#define  NV_AAGPS_DEFAULT_XTRA_PRIMARY_SNTP_SERVER_URL          "time.xtracloud.net"
#define  NV_AAGPS_DEFAULT_XTRA_SECONDARY_SNTP_SERVER_URL        "time.xtracloud.net"
#define  NV_AAGPS_DEFAULT_XTRA_TERTIARY_SNTP_SERVER_URL         "time.xtracloud.net"
#endif /* FEATURE_GNSS_XTRA3 */


#endif  /* __GNSS_NV_LOADER_H__ */
