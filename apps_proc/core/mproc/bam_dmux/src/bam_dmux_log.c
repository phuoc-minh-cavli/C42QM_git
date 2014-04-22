/*=============================================================================

                            BAM DMUX LOG

 BAM Data Multiplexer logging.

 Copyright  2016,2019 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/bam_dmux/src/bam_dmux_log.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   rv      Initial version.
===========================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "err.h"
#include "ULogFront.h"
#include "bam_dmux_log.h"

/*=============================================================================

                         MACRO DECLARATIONS

=============================================================================*/
#define BAM_DMUX_LOG_TAG "LOG"

#define BAM_DMUX_LOG_DBG_BUFFER_SIZE        (8192)
#define BAM_DMUX_LOG_INFO_BUFFER_SIZE       (8192)

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/


/*=============================================================================

                       LOCAL DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/
ULogHandle bam_dmux_log_dbg_handle;
ULogHandle bam_dmux_log_info_handle;

/*=============================================================================

                     LOCAL FUNCTION DEFINATION

=============================================================================*/


/*=============================================================================

                     PUBLIC FUNCTION DEFINATION

=============================================================================*/

/*=============================================================================
  FUNCTION  bam_dmux_log_init
=============================================================================*/
/**
 * Initializes logging.
 *
 */
/*===========================================================================*/
void bam_dmux_log_init(void)
{
  /* Initialize uLOG buffer */
  ULogFront_RealTimeInit(&bam_dmux_log_info_handle,
                         "BAM_DMUX_INFO",
                         BAM_DMUX_LOG_DBG_BUFFER_SIZE,
                         ULOG_MEMORY_LOCAL,
                         ULOG_LOCK_OS);

  ULogFront_RealTimeInit(&bam_dmux_log_dbg_handle,
                         "BAM_DMUX_DBG",
                         BAM_DMUX_LOG_DBG_BUFFER_SIZE,
                         ULOG_MEMORY_LOCAL,
                         ULOG_LOCK_OS);

  BAM_DMUX_LOG_DBG(0, "BAM_DMUX Log is initialized.");
}
