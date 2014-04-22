/*============================================================================
  FILE:         lpr_definition_cxo.c

  OVERVIEW:     This file provides the init, enter and exit functions for
                different modes of cxo LPR.

  DEPENDENCIES: None

                Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/

#include "mpmint.h"

/*===========================================================================
 *              INTERNAL VARIABLES AND MACROS
 *==========================================================================*/

void cxo_shutdown_init(void)
{
}

void cxo_shutdown_enter(uint64 wakeup_tick)
{
}

void cxo_shutdown_exit( void )
{
  mpmint_trigger_interrupts();
}
