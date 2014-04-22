/*=============================================================================

                            BAM DMUX OS

 BAM Data Multiplexer OS interface.

 Copyright  2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/bam_dmux/src/bam_dmux_os_qurt.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   rv      Initial version.
===========================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "qurt.h"
#include "smem.h"
#include "bam_dmux_log.h"
#include "bam_dmux_cfg.h"
#include "bam_dmux_os.h"

/*=============================================================================

                         MACRO DECLARATIONS

=============================================================================*/
#undef BAM_DMUX_LOG_TAG
#define BAM_DMUX_LOG_TAG "OS"

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/


/*=============================================================================

                       LOCAL DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/
bam_dmux_os_mutex_type bam_dmux_os_atomic;

/*=============================================================================

                     LOCAL FUNCTION DEFINATION

=============================================================================*/


/*=============================================================================

                     PUBLIC FUNCTION DEFINATION

=============================================================================*/

/*=============================================================================
  FUNCTION  bam_dmux_os_uncached_malloc
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
void *bam_dmux_os_uncached_malloc(uint32 size, uint32 *paddr)
{
  static uint32 smem_size = 0;
  static uint8  *base_va = NULL;
  static uint32 base_pa = 0;
  static uint32 offset = 0;
  void *ret_addr;

  if (base_va == NULL)
  {
    //smem_size = (BAM_DMUX_CFG_MAX_TX_BUFS + BAM_DMUX_CFG_MAX_RX_BUFS + 2);
    //smem_size *= 8; //BAM_DMUX_BAM_DESC_SIZE
    smem_size = 0x1000;

    base_va = smem_alloc(SMEM_A2_BAM_DESCRIPTOR_FIFO, smem_size);

    if (base_va == NULL)
    {
      BAM_DMUX_LOG_FATAL(1, "SMEM_A2_BAM_DESCRIPTOR_FIFO allocation failed smem_size=%d.",
          smem_size);
    }

    base_pa = bam_dmux_os_va_to_pa(base_va);
  }

  if ((smem_size - offset) < size)
  {
    BAM_DMUX_LOG_FATAL(3, "There is sufficient uncached memory smem_size=%d, "
        "offset=%d, size=%d.", smem_size, offset, size);
  }

  ret_addr = base_va + offset;
  if (paddr != NULL)
  {
    *paddr = base_pa + offset;
  }

  offset += size;

  return ret_addr;
}

/*=============================================================================
  FUNCTION  bam_dmux_os_get_thread_id
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
uint32 bam_dmux_os_get_thread_id(void)
{
  return (uint32)qurt_thread_get_id();
}
