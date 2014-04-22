#ifndef CONSOLE_PRINTF_API_H
#define CONSOLE_PRINTF_API_H

/*===========================================================================
                         CONSOLE_PRINTF_API_H

DESCRIPTION
  This header file contains API's which are exposed to external tasks/applications
  to log into QCLI

  Copyright (c)2019 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*==============================================================================

                            EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    -------------------------------------------------------------
04/07/2019   sr     created file
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

/* This API is used to log debugs to QCLI Console
*/
void Console_Printf(const uint8_t *format, ...);
#endif

