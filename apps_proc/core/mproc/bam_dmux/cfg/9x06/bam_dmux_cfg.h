/*=============================================================================

                            BAM DMUX CFG

 BAM Data Multiplexer Configuration.

 Copyright  2016-2017 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/bam_dmux/cfg/9x06/bam_dmux_cfg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   rv      Initial version.
===========================================================================*/

#ifndef BAM_DMUX_CFG_H
#define BAM_DMUX_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "qurt_cache.h"

/*=============================================================================

                         MACRO DECLARATIONS

=============================================================================*/
/** Max number of channels */
#define BAM_DMUX_CFG_MAX_CHANNELS            (9)

/** Physical Address of the BAM block */
/* From IPCAT: MSS_BAM_NDP_AUTO_SCALE_V2_0	MSS_TOP	0x4044000	0x19000	0x405D000	61 */
#define BAM_DMUX_CFG_BAM_PHYSICAL_ADDR       (0x4044000)

/** Size of the BAM */
#define BAM_DMUX_CFG_BAM_SIZE                (0x19000)

/** IRQ for the BAM block */
#define BAM_DMUX_CFG_BAM_IRQ                 (61)

/** Summing threshold */
#define BAM_DMUX_CFG_BAM_THRESHOLD           (0x1000)

/** TX pipe index */
#define BAM_DMUX_CFG_BAM_TX_PIPE_NUMBER      (4)

/** RX pipe index */
#define BAM_DMUX_CFG_BAM_RX_PIPE_NUMBER      (5)

/** Max number of TX descriptors  */
#define BAM_DMUX_CFG_MAX_TX_BUFS             (32)

/** Max number of RX descriptors  */
#define BAM_DMUX_CFG_MAX_RX_BUFS             (32)

/** RX buffer size */
#ifdef FEATURE_9x07_CAT4
  #define BAM_DMUX_CFG_RX_BUF_SIZE           (0x4000)
#else
  #define BAM_DMUX_CFG_RX_BUF_SIZE           (0x1000)
#endif

/** Inactivity timeout in msec */
#define BAM_DMUX_CFG_MAX_UL_TIMEOUT          (500)

/** Cache line size in bytes */
#define BAM_DMUX_CFG_CACHE_LINE_SIZE         (DCACHE_LINE_SIZE)


/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/


/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/


/*=============================================================================

                    PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* BAM_DMUX_CFG_H */
