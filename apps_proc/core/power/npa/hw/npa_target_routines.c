/*==============================================================================

FILE:      npa_target_routines.c

DESCRIPTION: NPA target specific routines for RCINIT-ified targets.

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2010 - 2016 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.tx/6.0/power/npa/hw/npa_target_routines.c#1 $

==============================================================================*/
#include "npa_config.h"
#include "npa_scheduler_internal.h"
#include "rcinit.h"
#include "CoreVerify.h"

/* RPM available callback, defined in npa_remote_rpm_protocol.c */
extern void npa_rpm_available_cb( void*, unsigned int, void*, unsigned int );

void npa_target_init( void )
{
#ifdef NPA_REMOTING_SUPPORTED
  /* npa_rpm_available_cb is only defined when remoting is supported. */
  npa_resource_available_cb( "/init/rpm", npa_rpm_available_cb,  NULL );
#endif
}

#ifdef NPA_SCHEDULED_REQUESTS_SUPPORTED
/* Create the NPA Scheduler Thread */
void npa_scheduler_init_rctask( void )
{
  RCINIT_INFO handle = rcinit_lookup("npaScheduler");

  if (RCINIT_NULL != handle)
  {
    rcinit_initfn_spawn_task(handle, npaScheduler_task);
  }
}

#endif /* NPA_SCHEDULED_REQUESTS_SUPPORTED */
