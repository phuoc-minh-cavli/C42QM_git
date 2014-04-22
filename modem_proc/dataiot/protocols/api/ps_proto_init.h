#ifndef PS_PROTO_INIT_H
#define PS_PROTO_INIT_H

/*===========================================================================

                   P S _ P R O T O _ I N I T . H

DESCRIPTION
  Protocols Generic Initialization Functions.

EXTERNALIZED FUNCTIONS
 NONE

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_proto_utils.h_v   1.0   08 Aug 2002 11:19:50   akhare  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/api/ps_proto_init.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
09/28/18    ds     Code changes for chiltepin(9x05).
07/30/14    vl      Created the initial version and merged all functions from
                    mmgsdi_client.h.
==========================================================================*/

#include "comdef.h"
#include "customer.h"

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================
FUNCTION PS_MMGSDI_CLIENT_REG_INIT()

DESCRIPTION
  This function registers PS as client to the MMGSDI module.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_mmgsdi_client_reg_init
(
  void
);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION PS_PROTO_REFRESH_NV_EFS()

DESCRIPTION
  This function invokes all NV items of different protocol modules that was 
  changed due to SIM swap. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_proto_refresh_nv_efs
(
  void
);

/*===========================================================================
FUNCTION PS_PROTO_REFRRESH_CB()

DESCRIPTION
  Callback function to be invoked when NV refresh event is received from 
  MCFG framework.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_proto_refresh_cb

(
  ps_utils_refresh_notify_info  ps_refresh_info
);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*===========================================================================
FUNCTION PS_PROTO_INIT()

DESCRIPTION
  This function invokes all protocol initialization functions from all its 
  different modules. It also registers to PS framework for NV refresh 
  notification. In addition this registers with PS for SIM ready event. This 
  function is called from psi_init() in PS task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_proto_init
( 
  void
);


void ps_proto_nv_read
(
  void
);

#endif  /* PS_PROTO_INIT_H */
