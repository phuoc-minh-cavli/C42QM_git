/*===========================================================================

                    I P C    R O U T E R   I N I T

   This file does the node and OS specific initialization of IPC Router
   and sets up all the links.

  ---------------------------------------------------------------------------
  Copyright (c) 2010-2015,2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.10/mproc/ipc_router/src/ipc_router_init.c#3 $
$DateTime: 2019/03/25 06:25:23 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_core.h"
#include "ipc_router_xal.h"
#ifdef IPC_ROUTER_XAL_GLINK
#include "ipc_router_xal_glink.h"
#endif
#include "DALSys.h"
#include "DALSysTypes.h"

/* EXAMPLE INIT FOR MPSS DIME (8974) */

/* Change default for each image PW */
#define IPC_ROUTER_DEFAULT_PROCESSOR_ID 0

#ifdef IPC_ROUTER_XAL_GLINK
//smdtask priority. idea is to have glink tasks priority higher than ipcrtr tasks priority
//glink uses SMD_HIGH(0x5C) task priority, ipcrtr using smdtask priority (0x8F)
#define IPC_ROUTER_XAL_GLINK_PRIO 0x8F    
#define IPC_ROUTER_XAL_GLINK_STACK_SIZE 4 * 1024
#endif

static int ipc_router_inited = 0;
extern void ipc_router_qdi_init(void);

static unsigned int ipc_router_get_processor_id(void)
{
  DALSYS_PROPERTY_HANDLE_DECLARE(hSpmDevCfg);
  DALSYSPropertyVar prop;
  DALResult result;

  result = DALSYS_GetDALPropertyHandleStr("/dev/core/mproc/ipc_router", hSpmDevCfg);
  if(result != DAL_SUCCESS)
  {
    return IPC_ROUTER_DEFAULT_PROCESSOR_ID;
  }
  result = DALSYS_GetPropertyValue( hSpmDevCfg, "local_processor_id", 0, &prop );
  if(result != DAL_SUCCESS)
  {
    return IPC_ROUTER_DEFAULT_PROCESSOR_ID;
  }
  return (unsigned int)prop.Val.dwVal;
}

extern void qsocket_init(void);

void ipc_router_init(void)
{
  if(ipc_router_inited)
    return;
  ipc_router_inited = 1;
  /* Initialize router and start transports */
  ipc_router_core_init(ipc_router_get_processor_id());

  qsocket_init();

#ifdef FEATURE_IPC_ROUTER_QDI_DRIVER
  ipc_router_qdi_init();
#endif
#ifdef IPC_ROUTER_XAL_GLINK
  /* Link to Apps */
  {
    static ipc_router_xal_glink_param_type param = {
      "SMEM",
      "apss", "IPCRTR", GLINK_OPT_INITIAL_XPORT, 
      IPC_ROUTER_XAL_GLINK_PRIO,
      IPC_ROUTER_XAL_GLINK_STACK_SIZE,
      {{128,16}, {512, 8}, {1024+128, 5}, {8192+128, 1}}};

    ipc_router_xal_start_xport(&ipc_router_xal_glink, (void *)&param, 0, "APSS");
  }
#endif
}

