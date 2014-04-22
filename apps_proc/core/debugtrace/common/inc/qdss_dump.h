#ifndef QDSS_DUMP_H
#define QDSS_DUMP_H

/*=============================================================================

FILE:         qdss_dump.h

DESCRIPTION:  Defines and functions used by various Debug drivers to handle
              saving of registers & associated memory to DDR for crash analysis

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "comdef.h"
#include "product_dump.h"
#include "DALStdDef.h"
#include "DALStdErr.h"

/* Header at the beginning of the QDSS Dump region. Parser looks for this
   magic and then uses the rest of the table to get the relative offsets of all
   other dumps */
typedef PACK(struct)
{
   uint32 magic;
   uint32 version;
   uint32 dump_table_offset;
   uint32 etb_regs_offset;
   uint32 etr_regs_offset;
   uint32 etb_data_offset;
   uint32 etr_data_offset;
   uint32 dcc_regs_offset;
   uint32 dcc_data_offset;
} qdss_buf_hdr;
/* Magic written at the start of the header */
#define QDSS_DUMP_MAGIC_NUMBER 0x5D1DB1BF

#define QDSS_BUF_DUMP_TABLE_OFFSET  0X400
#define QDSS_BUF_ETB_REGS_OFFSET   0X1000
#define QDSS_BUF_ETR_REGS_OFFSET   0X3000
#define QDSS_BUF_ETB_DATA_OFFSET   0X5000
#define QDSS_BUF_ETR_DATA_OFFSET   0X16000
#define QDSS_BUF_DCC_DATA_OFFSET   (QDSS_BUF_ETR_DATA_OFFSET - 0x2000)
#define QDSS_BUF_DCC_REGS_OFFSET   (QDSS_BUF_DCC_DATA_OFFSET - 0x2000)

#define QDSS_TMC_REGS_PAGE_SIZE  0x1000
#define QDSS_ETB_DUMP_SIZE  (QDSS_BUF_DCC_REGS_OFFSET - QDSS_BUF_ETB_DATA_OFFSET)

#define QDSS_DUMP_DATA_TYPE_HEADER_LEN 0x1000

#define QDSS_DDR_BUFFER_SIZE_512KB	512*1024
#define QDSS_DDR_BUFFER_SIZE_256KB	256*1024
#define QDSS_DDR_BUFFER_SIZE_128KB	128*1024
#define QDSS_DDR_BUFFER_SIZE_64KB	64*1024


/* Locates apps dump table and initializes the QDSS Dump region with the header
   containing all other offsets */
void qdss_debug_dump_init (void);

/* Return the address of the QDSS Dump start for drivers to compute their dump
   addresses as offsets from here */
DALResult qdss_debug_dump_get_dump_buffer_base (uint32 *debug_trace_buffer_phy,
                                                uint32 *debug_trace_buffer_virt,
                                                uint32 *debug_trace_buffer_size);

/* Adds an entry into the apps dumps table for SDI to save the data. Called by
   all drivers for any region they want saved */
DALResult qdss_debug_dump_add_dump_entry (const dump_entry_type *entry);

#endif /* QDSS_DUMP_H */
