#ifndef DIAGCOMPRESS__BUFFERI_H
#define DIAGCOMPRESS__BUFFERI_H
/*==========================================================================

               Diagnostics Output Buffer Allocation Manager

Description
  Type and function definitions for the diag compression allocation manager.

Copyright (c) 2014-2015 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                          Edit History

      $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcompress_bufferi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/14/15   rh   Moved HDLC padding magic number to #define
05/19/14   sa   Added support for new functions to get the total
                number of bytes in compression buffer and internal buffer.
13/03/14   sa   Created.
--------------------------------------------------------------------------*/

#include "comdef.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for structures needed for compression.

===========================================================================*/

typedef struct
{
   uint8  status; /* Status of the packet in the compression buffer  Used internally for buffer management*/
   uint8 cmd_code; /* Command code for the compressed packet*/
   uint16 length;  /* stores the length of the compressed chunk */
   uint8 hdlc_encoded;  /* Indicates if the individual packets in the compressed chunk are hdlc encoded or not */
   uint8 comp_algo;  /* 0 = lz4, 1 = zlib */
   uint16 comp_ver;  /* zlib/lz4 version number */
}diagbuf_compressed_header_type;

typedef struct
{
   diagbuf_compressed_header_type diagbufc_hdr;
   uint32 comp_data[1];
}diagcompressed_data_type;

/* This is an estimate of how much the length of a compressed buffer packet increases through HDLC encoding.
   Based on existing testing, it looks to usually be about 1% of total length, and packets range from 6kB to 15kB
   If one increases this, it reflects an assumption that more HDLC escape codes will be added. */
#define DIAG_COMPRESS_HDLC_ESTIMATE (100)

/* -------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------- */

#ifdef __cplusplus
  extern "C" {
#endif


/*===========================================================================
FUNCTION DIAGBUFC_RESET
DESCRIPTION
  This function resets the compression variables.
  No Mutexes or timers should be created in this function as this is 
  supposed to reset the variables used in compression.
  
PARAMETERS
SIDEEFFECTS
RETURN VALUES
============================================================================*/
void 
diagbufc_reset(void);
/*===========================================================================
FUNCTION DIAGCOMPRESSEDBUF_ALLOC
DESCRIPTION
 This function allocates the specified amount of space in the diag compressed 
 buffer
 
PARAMETERS
length              Length requested.
SIDEEFFECTS
RETURN VALUES
Pointer to allocated buffer if successful
============================================================================*/
PACK(void *) 
diagcompressedbuf_alloc(uint16 length); /* Length of requested buffer */


/*===========================================================================
FUNCTION DIAG_COMPRESS
DESCRIPTION
 This function compresses the input data and stores it in the compressed buffer.
 
PARAMETERS
data                Data to be compressed
length              Length requested.
drain               boolean to indicate if drain signal is set
SIDEEFFECTS
RETURN VALUES
============================================================================*/

void 
diag_compress(uint8* data, uint16 length, boolean drain);


/*===========================================================================
FUNCTION DIAG_PER_COMPRESSION_BUFFER_FULL
DESCRIPTION
 This function returns the percentage compression buffer full.
 
PARAMETERS
SIDEEFFECTS
RETURN VALUES
  Percentage Compression Buffer Used.
============================================================================*/
uint8 diag_per_compression_buffer_full(void);
/*===========================================================================
FUNCTION DIAG_COMPRESSION_BYTES_INTERNAL_BUFFER
DESCRIPTION
 This function returns the number of bytes in the internal buffer to be compressed.
 
PARAMETERS
SIDEEFFECTS
RETURN VALUES
  Number of Bytes in the internal buffer which are yet to undergo comperssion.
============================================================================*/
uint32 diag_compression_bytes_internal_buffer(void);
/*===========================================================================
FUNCTION DIAGCOMPRESSEDBUF_DRAIN
DESCRIPTION
 This function drains the compressed chunks from the compressed buffer.
 
PARAMETERS
SIDEEFFECTS
RETURN VALUES
============================================================================*/
void 
diagcompressedbuf_drain(void);

#ifdef __cplusplus
  }
#endif


#endif

