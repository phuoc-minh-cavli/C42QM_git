/*===========================================================================

                    Q S O C K E T\

  --------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/apr.common/4.0/datalink/apr_qsocket/stub_src/apr_qsocket.c#1 $
$DateTime: 2019/05/14 20:28:09 $

=============================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "mmstd.h"
#include "apr_dbg_msg.h"
#include "apr_errcodes.h"

/*===========================================================================
FUNCTION apr_qsocket_init
Initalizes the needed qsockets and spawns the worker threads for receiving the packets. 
===========================================================================*/

int32_t apr_qsocket_init (void)
{
    return APR_EOK;
  
} /*<<end  qsocket init >> */


/*Function to send the packets to remote processors/addresses using qsocket*/
int32_t apr_qsocket_tx(uint32_t port_index, void *buf, uint32_t length)
{
   return APR_EOK;
   
}
   
   
   