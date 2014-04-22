/******************************************************************************
#@file    psmd.c
#@brief   PSM daemon
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "psm_core.h"
#include "psm_common.h"

ril_platform_thread_t      psm_event_thread_id;

/*========================================================================================
FUNCTION main

main function of psmd.

ARGUMENTS
    none

RETURN VALUE
    void
========================================================================================*/
int psmd_task_entry
(
    dword ignored
)
{
    int rc = -1;

    rc = psm_core_server_init();
    if ( rc != 0 )
    {
        PSM_LOG_INFO("Failed to create socket");
    }

    // psm serve init is done, signal rcinit
    rcinit_handshake_startup();

    psm_core_init();

    psm_do_listen_client();

    return rc;
}

