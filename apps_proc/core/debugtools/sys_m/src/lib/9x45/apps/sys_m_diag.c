/*===========================================================================

           S Y S _ M_DIAG . C

DESCRIPTION

Copyright (c) 2011-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/debugtools/sys_m/src/lib/9x45/apps/sys_m_diag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/26/14   dav     File Created

===========================================================================*/

#include "diagpkt.h"
#include "diagcmd.h"
#include "sys_m_diag.h"
#include "tms_diag.h"

int sys_m_shutdown_delay_secs = 0;
typedef PACK(struct) PACKED_POST
{
  diagpkt_subsys_header_type hdr;
  uint8 delay_secs;
} sys_m_shutdown_delay_req_type;

/*===========================================================================

FUNCTION SYS_M_SHUTDOWN_DELAY_DIAG

DESCRIPTION
  This callback is registered with diag (send_data 75 37 11 16 <delay>)
  When executed it will delay shutdown completion by <delay> seconds.
  This facilitates automated testing by allowing additional time for diag
  packets to be flushed.

===========================================================================*/
PACK(void *) sys_m_shutdown_delay_diag (PACK(void *) req_ptr, uint16 pkt_len)
{
  sys_m_shutdown_delay_req_type *req;

  /* build response */
  tms_diag_send_generic_response(req_ptr);

  // decode request
  req = (sys_m_shutdown_delay_req_type *) req_ptr;

  // default parameters
  if (pkt_len <= sizeof(req->hdr)) 
  {
    // no delay given so clear it
    sys_m_shutdown_delay_secs = 0;
  }
  else
  {
    sys_m_shutdown_delay_secs = (req->delay_secs);
  }

  return(NULL);
}
