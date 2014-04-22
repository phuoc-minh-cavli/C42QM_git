#ifndef RFRX_H
#define RFRX_H

/*==========================================================================

              RF  RX  H E A D E R    F I L E

DESCRIPTION

  This header file contains all the definitions necessary for the RF driver
  to interface with the MDSP.

  Copyright (c) 2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rfa.mpss/3.10/rf/common/stubs/inc/rfrx.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/01/12   lcl     Removed empty mdsp interface header file  

===========================================================================*/

#include "rfa_variation.h"
#ifdef FEATURE_GSM
#include "mdsp_intf.h"
#endif /* FEATURE_GSM */

#include "rfdevice_type_defs.h"

#endif /* RFRX_H */
