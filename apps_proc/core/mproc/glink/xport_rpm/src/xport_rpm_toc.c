/*===========================================================================

       GLink RPM transport Table of Channels Configuration Structures

=============================================================================

  @file
    sport_rpm_toc.c

    Initialize table of channels for Glink RPM trasnport in MSG RAM.

   Copyright (c) 2014-2015,2018 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Qualcomm Technologies, Inc. Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/glink/xport_rpm/src/xport_rpm_toc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/20/14   an      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "xport_rpm_config.h"
#include "rpm_messageram.h"
#include "smem_type.h"

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/

/*===========================================================================
                              GLOBAL DATA DECLARATIONS
===========================================================================*/

extern const char* xport_rpm_msg_ram;
extern const void* xport_rpm_msg_ram_toc;
extern const uint32 xport_rpm_toc_config_num;

const xport_rpm_toc_config_type* xport_rpm_get_toc_config(uint32 ind);

/*===========================================================================
FUNCTION      xport_rpm_toc_init
===========================================================================*/
/**

  Initializes Table of Channels for RPM transport.
  Must be called before any other operations are done.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void xport_rpm_toc_init(void)
{
  uint32 i;
  uint32* msg_ram_toc = (uint32*)xport_rpm_msg_ram_toc;
  xport_rpm_toc_entry_type* entry;

  entry = (xport_rpm_toc_entry_type*)&msg_ram_toc[XPORT_RPM_TOC_ENTRIES_IDX];

  for (i = 0; i < xport_rpm_toc_config_num; i++, entry++)
  {
    const xport_rpm_toc_config_type *pcfg = xport_rpm_get_toc_config(i);
    entry->fifo_size = pcfg->fifo_size;
    entry->fifo_offset = (uint32)message_ram_malloc(
                           pcfg->host,
                           entry->fifo_size + 2*sizeof(uint32)) - 
                         (uint32)xport_rpm_msg_ram;
    entry->fifo_id = pcfg->fifo_id;
  }

  /* finalize ToC initialization */
  msg_ram_toc[XPORT_RPM_TOC_LENGTH_IDX] = xport_rpm_toc_config_num;
  msg_ram_toc[XPORT_RPM_TOC_MAGIC_IDX] = XPORT_RPM_TOC_MAGIC;
}
