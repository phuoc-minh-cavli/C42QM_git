/*===========================================================================

          Shared Memory MSM (2MB SMEM) Device Configuration Structures

=============================================================================

  @file
    smem_partitions.c

    Contains structures to be used in SMEM Device Configuration.

  Copyright (c) 2013-2016,2018 Qualcomm Technologies, Inc. 
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/16/17   ss      ADSP to CDSP partition for VT multi DSP framework
02/08/17   vr      Added changes for qcs405.
02/08/17   vr      Added sdm630 partition entries.
07/26/16   vr      Added MPSS-HYP partition.
08/26/15   an      Added SPSS partitions.
06/07/15   rs      Moved MPSS partitions next to legacy one to reduce XPU usage
04/07/15   rs      + Ordering partitions such that Modems are stacked together 
                     to miinize XPU usage
                   + Added support for legacy partition protections
09/05/14   sm      Compiler warnings fixes.
11/01/13   bt      Enable partitions on Apps edges.
10/17/13   bt      Re-enable non-Apps SMEM partitions.
09/19/13   bt      Enable partitions on non-Apps edges.
05/16/13   pa      Enabling this to be part of packed builds.
08/22/13   bt      Initial revision.  All partitions disabled (size = 0).
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "Library/SmemLib/inc/smem_toc.h"

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/
const smem_toc_config_type smem_toc_partition_entries[] =
{
  /* Apps<->Modem partition. */
  {
    184 * 1024,   /* Size of the partition  (must be 4kB multiple). */
    SMEM_TOC_ENTRY_FLAGS_ENABLE_RW_PROTECTION, /* flags */
    SMEM_APPS,    /* proc0 with RW access to the partition. */
    SMEM_MODEM,   /* proc1 with RW access to the partition. */
    32,           /* The cacheline common multiple, in Bytes. */
    {0, 0, 0, 0}  /* Exclusion regions' sizes within this partition. */
  },

  /* Last entry uses invalid hosts and no protections to signify the end. */
  {
    0,
    0,
    SMEM_INVALID_HOST,
    SMEM_INVALID_HOST,
    0,
    {0, 0, 0, 0}
  }
};
