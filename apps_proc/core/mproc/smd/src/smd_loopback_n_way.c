/*===========================================================================

        S M D   L O O P B A C K   A P I   I M P L E M E N T A T I O N

===========================================================================*/
/**
  @file smd_loopback_n_way.c
*/
/* 
  Defines the general private API and structures used to implement the N-way 
  shared memory driver loopback clients and servers.
*/

/*===========================================================================
    Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_loopback_n_way.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/13   pa      Update N-Way lock to use smd_cs_event_create which
                   will result in a non-preemptive lock on WCNSS.
09/14/12   bt      Use changed version mask name SMEM_FULL_VERSION_MASK.
06/30/11   bt      Initial implementation.
===========================================================================*/
/*===========================================================================
                   INCLUDE FILES
===========================================================================*/
#include "smd_loopback_n_way.h"
#include "smd_os_common.h"
#include "smem.h"

/*===========================================================================
                        MACRO AND DATA DEFINITIONS
===========================================================================*/
/* Pointer to the loopback command register, to be shared by both client and
 * server (if applicable) to reduce smem spinlocking. */
smd_loopback_command_type *smd_lb_nway_cmd_reg = NULL;
/* The maximum number of supported loopback channels, which stays constant but
 * is based on the size of the command register allocated at boot time. */
uint32                     smd_lb_nway_max_chnls = 0;
void                      *smd_lb_nway_cs = NULL;

/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smd_loopback_nway_init
===========================================================================*/
/**
  Initializes the SMD Nway Loopback structures, to avoid locking the
  smem allocation spinlock more than once.

  @return
  None.

  @dependencies
  Shared memory must have been initialized, and the SMD Nway loopback register
  must have already been allocated by the bootloaders.
*/
/*==========================================================================*/
void smd_loopback_nway_init( void )
{
  uint32 lb_reg_byte_sz;
  
  if (smem_version_set(SMEM_VERSION_SMD_NWAY_LOOP, SMD_NWAY_LOOP_VERSION, 
                       SMEM_FULL_VERSION_MASK) == TRUE)
  {
    smd_lb_nway_cmd_reg = (smd_loopback_command_type *)
                    smem_get_addr(SMEM_SMD_LOOPBACK_REGISTER, &lb_reg_byte_sz);
    smd_lb_nway_max_chnls = lb_reg_byte_sz / sizeof(smd_loopback_command_type);
    smd_lb_nway_cs = smd_cs_event_create();
  }
  else
  {
    /* If a processor has a different Nway Loopback version, it won't work.  
     * Print a non-fatal message. */
    ERR("smd_loopback_nway_init: Nway Loopback version (%d) does not match!", 
        SMD_NWAY_LOOP_VERSION, 0, 0);
  }
}
