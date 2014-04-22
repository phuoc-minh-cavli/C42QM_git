#ifndef DIAG_COMPRESS_V_H
#define DIAG_COMPRESS_V_H
/*==========================================================================

                     Diagnostic Compression Task Header File

Description
  Global Data declarations for the diag_compress_task.

Copyright (c) 2014, 2016, 2018 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                         Edit History

      $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diag_compress_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/18   gn      Memory optimizations
08/09/16   ph      Adjust the priorities of DIAG tasks.
02/02/16   as      Updated the Max Buffer Threshold bytes for Compression Buffer
02/26/14   sa      Created
===========================================================================*/

#include "osal.h"

#define DIAG_COMPRESS_STACK_SIZE 256  /* type is unsigned long long, so stack size will be 1KB */



#define DIAG_TX_COMPRESS_SIG       0x00000001

#define DIAG_COMPRESS_RPT_TIMER_SIG      0x00000010    /* timer signal for watchdog
                                                   for the diag_fwd_task */

#define DIAG_COMPRESS_TOUT_TIMER_SIG     0x00000100

#define DIAG_NUM_DSM_ITEMS_TO_PROCESS 5 /* Number of DSM items to compress at a time */

#define DIAG_COMPRESSION_INT_BUF_SIZE 0x8  /*Max size of data to be compressed at a time*/

/* Max Threshold Value for the compressed buffer => Max Size  - 2*Buffer needed for one Actual Compression
   Twice of the buffer needed for one compression is used to account for the empty wrap length
   in the compression buffer */
#define DIAG_COMPRESSED_BUFFER_MAX_THRESHOLD (DIAGBUFC_SIZE - (2*DIAG_COMPRESSION_INT_BUF_SIZE))

/* Ensure that the UPPER Threshold Percentage bytes in the compression buffer is always less than or
   equal to DIAG_COMPRESSED_BUFFER_MAX_THRESHOLD else diag compression task will never be able to reach
   the threshold to start draining. */
#define DIAG_COMPRESSION_UPPER_THRESHOLD 95  /* Compressed Buffer Upper Threshold (as Percentage) */

#define DIAG_COMPRESSION_LOWER_THRESHOLD 0 /* Compressed Buffer Lower Threshold (as Percentage)  */

#ifdef DIAG_COMPRESSION_ZLIB
#define ZLIB_HEADER_TRAILER_BYTES 8
#endif


/* Keep the Prio of Compression Task lower than the Fwd task */

#define DIAG_COMPRESS_PRI (GET_DIAG_PRI() + 2);

extern osal_tcb_t diag_compress_task_tcb;


/*===========================================================================

FUNCTION DIAG_COMPRESS_TASK_START

DESCRIPTION
  This function contains the diagnostic compression task. This task compresses the TX traffic
  from the slave processor and apss processor

  This procedure does not return.

===========================================================================*/
void diag_compress_task_start(uint32 params);


/*===========================================================================

FUNCTION DIAG_COMPRESS_KICK_WATCHDOG

DESCRIPTION
  This function kicks the watchdog for the diagnostic compression task

===========================================================================*/
void diag_compress_kick_watchdog(void);

/*===========================================================================

FUNCTION DIAG_COMPRESS_SUSPEND

DESCRIPTION
  This function suspends the diag compression task
  for a specified time interval

===========================================================================*/

void diag_compress_suspend (unsigned int);
/*===========================================================================

FUNCTION DIAG_COMPRESS_HANDLE_SIGS

DESCRIPTION
  This routine waits for the DIAG_TX_COMPRESS_SIG and then processes the diag
  traffic from peripherals and apss.

  When this signal is set, the DSM Items are de-queued from the dsm queue
  and are further compressed in the compression buffer.

===========================================================================*/
void diag_compress_handle_sigs(void);

#endif /*DIAG_COMPRESS_V_H*/

