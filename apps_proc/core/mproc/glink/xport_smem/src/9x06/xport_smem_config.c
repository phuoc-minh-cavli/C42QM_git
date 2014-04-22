/*===========================================================================

            GLink SMEM transport 9x55 APSS Configuration Structures

=============================================================================

  @file
    xport_smem_config.c

    Contains structures to be used in Glink SMEM trasnport configuration.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/13/15   db      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "xport_smem_config.h"
#include "xport_smem_qos_generic.h"

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/
static glink_transport_qos_if_type xport_smem_qos_config = 
{
  /** Provides maximum data rate (tokens/sec) associated with transport. */
  20000,
  /** Provides data rate achievable by the transport for given request. */
  xport_smem_get_req_rate_generic,
  /** Allocates context structure to use by transport servicing given QoS request. */
  xport_smem_alloc_req_ctx_generic,
  /** Frees context structure used by transport servicing given QoS request. */
  xport_smem_free_req_ctx_generic,
  /** Starts QoS mode. */
  xport_smem_start_req_generic,
  /** Stops QoS mode. */
  xport_smem_stop_req_generic,
  /** Provides ramp up time in microseconds. */
  xport_smem_get_ramp_time_generic
};

const xport_smem_config_type xport_smem_config[] =
{
  /* sparrow->mpss */
  {
    "mpss",                /* Remote host name        */
    SMEM_MODEM,            /* Remote host ID          */
    16*1024,               /* Tx FIFO size            */
    4*1024,                /* Tx MTU size             */
                           /* Outgoing interrupt      */
    {DALIPCINT_PROC_MDSPSW,  
    DALIPCINT_GP_3},      
    60,                     /* Incoming interrupt      */
    &xport_smem_qos_config,
    NULL,                  /* Use default Alloc FIFO */
    NULL                   /* Use default clear interrupt */
  }
};

const uint32 xport_smem_config_num = sizeof(xport_smem_config)/sizeof(xport_smem_config[0]);
const smem_host_type xport_smem_this_host = SMEM_APPS;


const xport_smem_config_type* xport_smem_get_config(uint32 ind)
{
  if (ind >= xport_smem_config_num)
  {
    return NULL;
  }

  return &xport_smem_config[ind];
}
