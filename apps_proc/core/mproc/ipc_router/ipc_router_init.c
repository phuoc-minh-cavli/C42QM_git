/*===========================================================================

                    I P C    R O U T E R   I N I T

   This file does the node and OS specific initialization of IPC Router
   and sets up all the links.

  ---------------------------------------------------------------------------
  Copyright (c) 2010-2015,2018-2019 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/ipc_router/ipc_router_init.c#1 $
$DateTime: 2020/01/30 22:49:35 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_core.h"
#include "ipc_router_xal.h"
#include "ipc_router_protocol.h"
#include "ipc_policy.h"
#include "ipc_router_policy.h"
#ifdef IPC_ROUTER_XAL_GLINK
#include "ipc_router_xal_glink.h"
#endif

#define IPC_ROUTER_PROCESSOR_ID 2

#ifdef IPC_ROUTER_XAL_GLINK
#define IPC_ROUTER_XAL_GLINK_PRIO 47
#define IPC_ROUTER_XAL_GLINK_STACK_SIZE (1024 * 4)
#endif

static int ipc_router_inited = 0;

extern void qsocket_init(void);

void ipc_router_init(void)
{
  if(ipc_router_inited)
    return;
  ipc_router_inited = 1;
  
  /* Initialize router and start transports */
  ipc_router_core_init(IPC_ROUTER_PROCESSOR_ID);

  qsocket_init();

#ifdef FEATURE_IPC_ROUTER_POLICY
  ipc_router_policy_register(&ipc_router_impl_policy);
#endif

#ifdef IPC_ROUTER_XAL_GLINK
  /* Link to MPSS */
  {
    static ipc_router_xal_glink_param_type param = {
      "SMEM", "mpss", "IPCRTR", 0, 
      IPC_ROUTER_XAL_GLINK_PRIO,
      IPC_ROUTER_XAL_GLINK_STACK_SIZE,
      {{128,16}, {512, 8}, {1024+128, 5}}};

    ipc_router_xal_start_xport(&ipc_router_xal_glink, (void *)&param, 0, "APSS");
  }
#endif
}

