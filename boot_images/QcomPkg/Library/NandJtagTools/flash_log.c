/*=======================================================================
* FILE:        flash_log.c
*
* SERVICES:    Routines to support flash logging
*
* DESCRIPTION :
*
* Copyright (c) 2010,2015,2019 by Qualcomm Technologies, Inc. All Rights Reserved.
*=======================================================================*/

/*===================================================================
*
*                       EDIT HISTORY FOR FILE
*
*   This section contains comments describing changes made to the
*   module. Notice that changes are listed in reverse chronological
*   order.
*
*
* when         who     what, where, why
* ----------   ---     ----------------------------------------------
* 01/28/15     sb      Fix Klocwork errors
* 07/12/10     nr      Initial version
*==================================================================*/

/*===================================================================
*
*                     Include Files
*
====================================================================*/

#include "comdef.h"
#include <bits/alltypes.h>
#include <stdio.h>
#include "string.h"
#include "stdarg.h"
#include <flash_log.h>

#define NAND_OUTPUT_BUFFER_SIZE (0x2000)
byte output_buffer[NAND_OUTPUT_BUFFER_SIZE];
UINTN output_buffer_length;

void jprintf(const char* format, ...) {
    int retval;
    byte *buffer = &output_buffer[output_buffer_length];
    va_list args;
    va_start (args, format);

    UINTN buffer_space = sizeof(output_buffer) - output_buffer_length;

    retval = vsnprintf((char *)buffer,
                       buffer_space,
                       format,
							  args);

    output_buffer_length += MIN(buffer_space, retval);
    if(output_buffer_length > NAND_OUTPUT_BUFFER_SIZE)
      output_buffer_length = 0;
}
