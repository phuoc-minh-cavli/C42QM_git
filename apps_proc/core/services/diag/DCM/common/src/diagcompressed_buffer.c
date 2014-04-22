/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             Diagnostic Compressed Buffer Manager

General Description
  This file contains routines to compress the diag traffic and manage 
  diag compressed buffer.
  This allocation scheme is modeled as a circular buffer.


Initialization and Sequencing Requirements


Copyright (c) 2014-2016 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                              Edit History

 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcompressed_buffer.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/16   nk      Changes to support ThreadX
03/02/16   nk      Add a mechanism to stop compression when alloc fails along
                   with correction of total bytes in compression bufferin circular buffering mode.
09/10/15   rh      Add a pad to estimation of whether stuff fits in a DSM item
07/03/15   sa      Update the status of the compressed header within the lock
06/24/15   sa      Check if the packets need to be drained from the compression
                   buffer before draining the same.
06/23/15   sa      Update head of the compression buffer within a mutex
05/28/15   rh      diagcompressedbuf_alloc() no longer sets packets drainable.
05/04/15   rh      Fixed several infinite loops and concurrency bugs
01/28/15   sa      Start Compression when the compression flag is set
05/19/14   sa      New functions to return total bytes in compression
                   buffer and internal buffer to be compressed.
04/14/14   is      Add zero_init attribute to compression buffer
03/26/14   sa      Corrected the type of diag_fwd_tx_dsm_cnt.
02/26/14   sa      Created file.

===========================================================================*/
#include <stdio.h>
#include "customer.h"

#include "comdef.h"
#include "assert.h"
#include <stringl/stringl.h>
#include "diagcomm_v.h"
#include "diagdiag_v.h"
#include "diagbuf_v.h"
#include "diagi_v.h"
#include "diagdsm_v.h"

#include "err.h"
#include "lz4.h"

#include "osal.h"
#include "diag_cfg.h"
#include "diagcompress_bufferi.h"
#include "diag_compress_v.h"
#include "diag_fwd_v.h"


#ifdef DIAG_COMPRESSION_ZLIB
#include "zlib.h"
#endif

#define LZ4_VERNUM 0x0001;
#define LZ4_Compression 0x0;
#ifdef DIAG_COMPRESSION_ZLIB
#define ZLIB_Compression 0x1;
#endif

/* Extern variables used from other files */
extern diag_tx_mode_type diag_tx_mode;
extern osal_mutex_arg_t diag_compression_mutex;
extern osal_mutex_arg_t diag_tx_mode_drain_mutex;

uint32 diagbufc_new_chain = 0;

/* Total Number of Bytes in the Compressed Buffer */
uint32 total_bytes_in_compressed_buffer= 0;

/* Total number of bytes to compress in the internal buffer */
uint32 total_length_to_compress = 0;


/* To reset the compression variables when we switch to streaming mode
   from buffering mode */
static boolean bDiagCompressionReset = FALSE;

/* Wrap Count for the Compressed Buffer */
uint32 diagbufc_wrap_count = 0;
static uint8 tx_mode = 0;
static boolean bCircularBufferHeadSet = TRUE; /* To point to proper head in Circular buffering */


extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */


/* Internal buffer to be used with Circular buffering.
    The data is compressed in this buffer and then copied to the compressed buffer 
    after creating proper space in the buffer */
uint32 compressed_buffer_for_cir_mode[DIAG_COMPRESSION_INT_BUF_SIZE];

#ifndef DIAG_COMPRESSION_ZLIB
static char buffer_to_compress[DIAG_COMPRESSION_INT_BUF_SIZE];
#endif

/* Boolean to check if peripherals support hdlc offloading */
boolean bIsHdlcEncoded = TRUE;


/*lint -esym(751, diagbuf_header_status_enum_type) */

/* Supress local typedef 'diagbuf_header_status_enum_type' not referenced */



/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */

/*===========================================================================

MACRO DIAGBUFC_USED
MACRO DIAGBUFC_FREE

DESCRIPTION
  Status calculating macros for the diag compressed buffer.

PARAMETERS
  h - head index
  t - tail index

RETURN VALUE
  Number of bytes.

===========================================================================*/
#define DIAGBUFC_USED(h, t) ((h >= t) ? (h - (t)) : (h - (t) + DIAGBUFC_SIZE))

#define DIAGBUFC_FREE(h, t) (DIAGBUFC_SIZE - DIAGBUFC_USED(h, t))


/*===========================================================================
MACRO DIAGBUFC_WRAP

DESCRIPTION
  Wraps abstract buffer index.

PARAMETERS
  Abstract buffer index.

DEPENDENCIES
  0 <= x < ( DIAGBUFC_SIZE)

===========================================================================*/
#define DIAGBUFC_WRAP(x) \
   x = 0; \


typedef enum
{
  DIAGBUFC_USED_S = 0xA0,	/* Buffer is ready for draining */
  DIAGBUFC_WRAP_S,      	/* Notify that the buffer has to be wrapped  */
  DIAGBUFC_DRAINED_S,		/* Buffer has already been drained */
  DIAGBUFC_UNUSED_S,		/* Buffer is not yet ready for draining */
  DIAGBUFC_HOLE_SMALL_S,	/* For holes of size less than the header size */
  DIAGBUFC_HOLE_S			/* Buffer should be skipped over */
} diagbufc_header_status_enum_type;


/* To fill in diagbuf header for a hole, alignement need not be 8 byte aligned 
   on q6, macro below aligns on 32 bit boundary */
#define DIAGBUFC_NEXT_ALIGNED_BYTE_4(p) (((p) + 3) & ~3)

#define DIAGBUFC_ALIGNED_BYTE_4(p) ((p) & ~3)
/* Diag's output buffer.  The ADS 1.0 compiler will not align a byte array on
 * 32-bit boundary.  The ARM 2.5 compiler had a setting for this, but not ADS
 * 1.0.  Therefore, we must declare it to be a array of 32-bit values and
 * assign a byte* to point to it. */
 
 #define DIAGBUFC_NEXT_ALIGNED_BYTE(p) (((p) + 3) & ~3)
static uint32 __attribute__((section("DIAG_LOG"),zero_init)) diagbufc_buf_aligned[(DIAGBUFC_SIZE + 3)/4];
static uint8 *diagcompressedbuf_buf = (uint8 *) diagbufc_buf_aligned;

/* Head and tail of circular buffer.
*/

uint32 diagcompressedbuf_head = 0;
uint32 diagcompressedbuf_tail = 0;
static boolean bDrainInitialized = FALSE;

/* Lock for updating compression buffer tail in compress and fwd task*/
osal_mutex_arg_t diagbufc_tail_mutex;
boolean bTailMutexInitialized = FALSE;

/* This is the length of unused compression buffer between the end of the 
last compressed packet and the end of compression buffer. */
uint16 diagbufc_wrap_length = 0;

int zstream_err = 0;


/* Variables to store drop count for DIAG Health */
static uint32 alloc_compressed_buffer_count = 0;

/* Variables to store allocation requests for DIAG Health */
static uint32 diag_compression_flow_cnt = 0;

#ifdef DIAG_COMPRESSION_ZLIB
void diag_zerr(int ret);
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
diagbufc_reset (void)
{

  diagcompressedbuf_head = 0;
  diagcompressedbuf_tail = 0;
  
  diagbufc_wrap_count = 0;
  diagbufc_wrap_length = 0;
  bDrainInitialized = FALSE;
  bDiagCompressionReset = TRUE;
  bCircularBufferHeadSet = TRUE;
  
} /* diagbufc_init */


/*===========================================================================

FUNCTION DIAGCOMPRESSEDBUF_ALLOC

DESCRIPTION
  This function allocates the specified amount of space in the diag compressed buffer.

DEPENDENCIES
  This function must only be called from within this file by externalized
  alloc functions.

============================================================================*/
PACK(void *) diagcompressedbuf_alloc(
  uint16 length) /* Length of requested buffer */
{
   PACK(void *) ptr = NULL;  /* Pointer to return.        */
   uint16 buffer_used;          /* Length to allocate        */
   diagbuf_compressed_header_type *pkt_hdr = NULL;
   static diagbuf_compressed_header_type *prev_pkt_hdr = NULL;
   int return_val;
   osal_sigs_t return_sigs;

   /* Local indices to manipulate buffer */
   int iHead = 0;
   int iTail = 0;

   uint32 diagcompressedbuf_free_bytes = 0;
   uint32 diagcompressedbuf_used_bytes = 0;


   buffer_used =
            sizeof(diagbuf_compressed_header_type) + 
             (uint16)length  ;  /* Number of bytes requested buffer */

   buffer_used = DIAGBUFC_NEXT_ALIGNED_BYTE(buffer_used);

  /*if( buffer_used > DIAGCOMPRESBUF_SIZE )
    ERR_FATAL("diagbuf_alloc_internal: buffer_used (%d) > DIAGBUF_SIZE (%d)", buffer_used, DIAGBUF_SIZE, 0); */

   iTail = diagcompressedbuf_tail;
   iHead = diagcompressedbuf_head;

   ASSERT (iHead < DIAGBUFC_SIZE);
   ASSERT (iTail < DIAGBUFC_SIZE);


   /* Set a local pointer to the header */
   pkt_hdr = ((diagbuf_compressed_header_type *)&diagcompressedbuf_buf[iHead]);
   pkt_hdr->status = DIAGBUFC_UNUSED_S;


  if(tx_mode == DIAG_TX_MODE_BUFFERED_CIR )
  {
    if(iHead + buffer_used >= DIAGBUFC_SIZE)
    {
      pkt_hdr->length = DIAGBUFC_SIZE - iHead;
      pkt_hdr->status = DIAGBUFC_WRAP_S;
      diagbufc_wrap_length = pkt_hdr->length;

      /* Wrap */
      DIAGBUFC_WRAP(iHead);
      ASSERT ((iHead == 0));

      /*
        In circular buffering mode , if tail is ahead of head and incoming packet does not
        fit at head, then first make head zero and tail to point packet after head.
      */
      osal_lock_mutex(&diagbufc_tail_mutex);
      if(diagcompressedbuf_head <= diagcompressedbuf_tail)
      {
        diagcompressedbuf_tail = (((diagbuf_compressed_header_type *)&diagcompressedbuf_buf[iHead])->length)+sizeof(diagbuf_compressed_header_type);
      }
      osal_unlock_mutex(&diagbufc_tail_mutex);

      diagcompressedbuf_head = iHead;
      diagbufc_wrap_count++;
    }

    /* reassigning the pkt_hdr to take care of the updated ihead during wrap condition */
    pkt_hdr = ((diagbuf_compressed_header_type *)&diagcompressedbuf_buf[iHead]);
	pkt_hdr->status = DIAGBUFC_UNUSED_S;
    ptr = pkt_hdr;
  }
  else if(tx_mode == DIAG_TX_MODE_BUFFERED_THRESH )
  {
    if (iTail <= iHead          &&       /* Not wrapped &&  */
    iHead - iTail < DIAGBUFC_SIZE  &&       /* Not full &&     */
                                            /* No room here!   */
    (uint32)iHead + buffer_used >= DIAGBUFC_SIZE) 
    {
      prev_pkt_hdr = pkt_hdr;
      prev_pkt_hdr->cmd_code = DIAG_COMPRESSED_PKT;
      prev_pkt_hdr->length = DIAGBUFC_SIZE - iHead;
      prev_pkt_hdr->status = DIAGBUFC_WRAP_S;
      diagbufc_wrap_length = prev_pkt_hdr->length;
      diagbufc_wrap_count++;

      /* Wrap */
      DIAGBUFC_WRAP(iHead);
      ASSERT ((iHead == 0));
      diagcompressedbuf_head = iHead;
    }

    diagcompressedbuf_used_bytes = DIAGBUFC_USED(iHead,iTail);

    if(prev_pkt_hdr)
    {
      if(prev_pkt_hdr->status == DIAGBUFC_WRAP_S)
      {
        if(iHead == iTail)
        diagcompressedbuf_used_bytes = DIAGBUFC_SIZE;
      }
    }

    diagcompressedbuf_free_bytes = DIAGBUFC_SIZE - diagcompressedbuf_used_bytes;


    if (buffer_used <= diagcompressedbuf_free_bytes) 
    {
      /* Set header field of buffer */
      pkt_hdr = ((diagbuf_compressed_header_type *) &diagcompressedbuf_buf[iHead]);
	  pkt_hdr->status = DIAGBUFC_UNUSED_S;
      prev_pkt_hdr = pkt_hdr;

      ptr = pkt_hdr;
    }
  }
  else
  {
    return NULL;
  }

  if (ptr) 
  {
    alloc_compressed_buffer_count++;
  }
  else 
  {
    /* Mechanism to stop compression if alloc fails */
    diag_compression_flow_cnt++;

    osal_lock_mutex(&diag_tx_mode_drain_mutex);
    diag_tx_mode.drain = TRUE;
    osal_unlock_mutex(&diag_tx_mode_drain_mutex);
      
    /* Reset the compression signal */
    return_val = osal_reset_sigs(&diag_compress_task_tcb, DIAG_TX_COMPRESS_SIG, &return_sigs);
    ASSERT(OSAL_SUCCESS == return_val);
      
    /* Set the drain signal to drain data from compression buffer */
    return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_FWD_TX_DRAIN_SIG, &return_sigs);
    ASSERT(OSAL_SUCCESS == return_val);
  }

  return ptr;

} /* diagcompressedbuf_alloc */

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

void diag_compress(uint8* data, uint16 length, boolean drain)
{

#ifdef DIAG_COMPRESSION_ZLIB
  static z_stream zstream;  /* zlib stream */
  static int zstream_flush = 0;
  static int zlib_init_ret = 0;
  static boolean bIszlibInitialized = FALSE;
  int zlib_deflate_ret = 0;
  static boolean bIsBufferAllocated = 0; /* boolean to check if buffer is allocated from the compressed buffer */
#else
  //for l4z
  static uint32 buffer_to_compress_index = 0;
#endif

  static diagcompressed_data_type* compressed_data = NULL;
  diagcompressed_data_type* diagbufc_hdr_cir_mode = NULL;
  diagcompressed_data_type* temp_hdr = NULL;
  uint32 itail;
  uint32 ihead;
  uint32 buffer_free = 0;
  static uint16 compressed_length = 0;  /*Compressed Data Length*/
  uint16 compressed_length_aligned = 0; /*Compressed Data Length with padding for alignment in compressed buffer */
  uint32 len_compressed_data_in_buffer = 0;
  uint16 l4z_len_to_compress = 0; 


  int diagbufc_ihead = 0;
  int diagbufc_itail = 0;

  static uint32 diagbufc_hole_length = 0;
  
  /* In streaming mode bDiagCompressionReset is made true to reset the compression buffer data */
  if(bDiagCompressionReset)
  {
#ifdef DIAG_COMPRESSION_ZLIB   
    
    if(bIszlibInitialized)
      deflateEnd(&zstream);
    zlib_init_ret = -1;   
    bIszlibInitialized = FALSE;
    bIsBufferAllocated = FALSE;
#else
    buffer_to_compress_index = 0;
#endif

    compressed_data = NULL;
    compressed_length = 0;
    total_length_to_compress = 0;
    bDiagCompressionReset = FALSE;
    
  }
  if(!bTailMutexInitialized)
  {
    int return_val = 0;
    /* Initialize the Compression Mutex*/
    diagbufc_tail_mutex.name = "MUTEX_COMPRESSION_TAIL";
    return_val = osal_init_mutex(&diagbufc_tail_mutex);
    ASSERT(OSAL_SUCCESS == return_val);
    bTailMutexInitialized = TRUE;
  }


  tx_mode = diag_get_tx_mode();

  if(tx_mode == DIAG_TX_MODE_STREAMING)
  {
    bDiagCompressionReset = TRUE;
    return;
  }
   
#ifdef DIAG_COMPRESSION_ZLIB   
  if(bIszlibInitialized == FALSE)
  {
    bIszlibInitialized = TRUE;
    zstream.zalloc = Z_NULL;
    zstream.zfree = Z_NULL;
    zstream.opaque = Z_NULL;
    zlib_init_ret = deflateInit(&zstream, Z_DEFAULT_COMPRESSION);
    diag_zerr(zlib_init_ret);
  }
  if(zlib_init_ret == Z_OK)
  {
    /* Allocate buffer from the Compressed Buffer if not already allocated */ 
    if(bIsBufferAllocated == FALSE)
    {
      compressed_data = (diagcompressed_data_type*)diagcompressedbuf_alloc(DIAG_COMPRESSION_INT_BUF_SIZE + ZLIB_HEADER_TRAILER_BYTES);

      if (tx_mode == DIAG_TX_MODE_BUFFERED_CIR && diagbufc_wrap_count > 0)
      {
        compressed_data = (diagcompressed_data_type*)compressed_buffer_for_cir_mode;
        compressed_data->diagbufc_hdr.hdr.status = DIAGBUFC_USED_S;

        diagbufc_hdr_cir_mode = (diagcompressed_data_type*)diagcompressedbuf_alloc(DIAG_COMPRESSION_INT_BUF_SIZE + ZLIB_HEADER_TRAILER_BYTES);
      }
    }
   
    if(compressed_data)
    {

      if(data != NULL)
      {
        /* Copy the data in the DSM items to a buffer till approx 20kb data is accumulated */
        
        bIsBufferAllocated = TRUE;

      /* Initilize zstream flush to hold the compression */
        zstream_flush = Z_NO_FLUSH;

        /* total length of the data to be compressed */
        total_length_to_compress +=  length;

        /* avail_in is the length of the data to be compressed */
        zstream.avail_in = length;

        /* next_in points to the buffer to be compressed */
        zstream.next_in = data;

        /* avail_out contains the length of the output buffer where compressed data will be stored */
        /*After deflate, it is updated to the size remaining in this output buffer*/
        zstream.avail_out = DIAG_COMPRESSION_INT_BUF_SIZE + ZLIB_HEADER_TRAILER_BYTES ;

        /* next_out refers to the output buffer to store the compressed data */
        zstream.next_out = (Byte*)compressed_data->comp_data;
      }

     /* Compress the data when the drain flag is set*/
      if(drain == TRUE)
      {
       zstream_flush = Z_FINISH;
      }                                

      /* call deflate to compress the data */
      zlib_deflate_ret= deflate(&zstream, zstream_flush);
      diag_zerr(zlib_deflate_ret);

      /* Length of the Compressed Data */
      compressed_length += ((DIAG_COMPRESSION_INT_BUF_SIZE + ZLIB_HEADER_TRAILER_BYTES) - zstream.avail_out);

#else       
          /* Copy the data in the DSM items to a buffer till diag_dsm_item_count number of
            DSM items are accumulated */
          /* only either of the drain or data will be true at the same time */
      if(data != NULL)
      {
        
        memscpy(&buffer_to_compress[buffer_to_compress_index], (sizeof(buffer_to_compress)-buffer_to_compress_index) , data, length);
        buffer_to_compress_index += length;
        
        ASSERT(buffer_to_compress_index <= sizeof(buffer_to_compress));

        /* Update the total length of the data to be compressed */
        total_length_to_compress +=  length;
      }
         
#endif         

      //start the compression if the drain flag is set 
      if(drain == TRUE)
      {

#ifdef DIAG_COMPRESSION_ZLIB
        compressed_data->diagbufc_hdr.hdr.cmd_code = DIAG_COMPRESSED_PKT;      
        compressed_data->diagbufc_hdr.comp_algo = ZLIB_Compression;
        compressed_data->diagbufc_hdr.comp_ver = ZLIB_VERNUM;
        compressed_data->diagbufc_hdr.hdlc_encoded = bIsHdlcEncoded;

#else
        l4z_len_to_compress = LZ4_compressBound(total_length_to_compress);
        /* Provides the maximum size that LZ4 may output in a "worst case" scenario (input data not compressible) */
        compressed_data = (diagcompressed_data_type*)diagcompressedbuf_alloc(l4z_len_to_compress);

        if(compressed_data)
        {
          if (tx_mode == DIAG_TX_MODE_BUFFERED_CIR && diagbufc_wrap_count > 0)
          {
            compressed_data = (diagcompressed_data_type*)compressed_buffer_for_cir_mode;
            compressed_data->diagbufc_hdr.status = DIAGBUFC_UNUSED_S; 

          }

          compressed_length = LZ4_compress(&buffer_to_compress[0], (char*)compressed_data->comp_data, total_length_to_compress );

          compressed_data->diagbufc_hdr.cmd_code = DIAG_COMPRESSED_PKT;          
          compressed_data->diagbufc_hdr.comp_algo = LZ4_Compression;
          compressed_data->diagbufc_hdr.comp_ver = LZ4_VERNUM;
          compressed_data->diagbufc_hdr.hdlc_encoded = bIsHdlcEncoded;


#endif

          /* Length of the compressed data */
          compressed_data->diagbufc_hdr.length =  compressed_length ;
          compressed_length_aligned = DIAGBUFC_NEXT_ALIGNED_BYTE(compressed_length);
          /* Update the number of bytes allocated in the compressed buffer */
          if( (diag_tx_mode.byte_alloc_cnt + (compressed_length_aligned + sizeof(diagbuf_compressed_header_type))) >= MAX_VALUE_UINT32 )
            diag_tx_mode.byte_alloc_cnt = MAX_VALUE_UINT32;
          else
            diag_tx_mode.byte_alloc_cnt += (compressed_length_aligned + sizeof(diagbuf_compressed_header_type));

          if(tx_mode == DIAG_TX_MODE_BUFFERED_CIR && diagbufc_wrap_count > 0 )
          {
            //FIXME I don't think this does anything useful --rhalstea
            if(bCircularBufferHeadSet == FALSE)
            {
              itail = diagcompressedbuf_tail;
              ihead = diagcompressedbuf_head;
              buffer_free = DIAGBUFC_FREE(ihead, itail);
              while(buffer_free >= MAX_VALUE_UINT16)
              {
                /*Create a hole at the head */
                temp_hdr = (diagcompressed_data_type*)&diagcompressedbuf_buf[ihead];
                temp_hdr->diagbufc_hdr.status = DIAGBUFC_HOLE_S;
                if(itail > ihead)
                {
                  temp_hdr->diagbufc_hdr.length = MAX_VALUE_UINT16 - sizeof(diagbuf_compressed_header_type);
                  ihead = DIAGBUFC_NEXT_ALIGNED_BYTE( ihead + MAX_VALUE_UINT16);									
                  buffer_free = DIAGBUFC_FREE(ihead, itail);
                }
                else
                {
                  buffer_free = (DIAGBUFC_SIZE - ihead);
                  if( buffer_free >= MAX_VALUE_UINT16)
                  {
                    temp_hdr->diagbufc_hdr.length = MAX_VALUE_UINT16 - sizeof(diagbuf_compressed_header_type);
                    ihead = DIAGBUFC_NEXT_ALIGNED_BYTE( ihead + MAX_VALUE_UINT16);
                  }
                }
              }
              temp_hdr = (diagcompressed_data_type*)&diagcompressedbuf_buf[ihead];
              if(buffer_free!= 0 && buffer_free < sizeof(diagbuf_compressed_header_type))
              {
                temp_hdr->diagbufc_hdr.status = DIAGBUFC_HOLE_SMALL_S;
                temp_hdr->diagbufc_hdr.length = 4;
              }
              else if(buffer_free >= sizeof(diagbuf_compressed_header_type))
              {
                temp_hdr->diagbufc_hdr.status = DIAGBUFC_HOLE_S;
                if(itail > diagcompressedbuf_head)
                {
                  temp_hdr->diagbufc_hdr.length = buffer_free - sizeof(diagbuf_compressed_header_type);
                }
                else
                {
                  temp_hdr->diagbufc_hdr.length = (DIAGBUFC_SIZE - ihead) - sizeof(diagbuf_compressed_header_type);
                }
              }
              bCircularBufferHeadSet = TRUE;
            }
            diagbufc_ihead = diagcompressedbuf_head;
            /* Take care of the holes which are less then header size created while over-writing the data in circular buffering*/
            if ((diagbufc_hole_length != 0) && (diagbufc_hole_length < sizeof(diagbuf_compressed_header_type)) )
            {
              len_compressed_data_in_buffer = 4;
              diagbufc_ihead += len_compressed_data_in_buffer;
            }
            diagbufc_hdr_cir_mode = (diagcompressed_data_type*)&diagcompressedbuf_buf[diagbufc_ihead];

            len_compressed_data_in_buffer += DIAGBUFC_NEXT_ALIGNED_BYTE(diagbufc_hdr_cir_mode->diagbufc_hdr.length + sizeof(diagbuf_compressed_header_type));

            /*
              Loop through the compressed packets to fit in the new compressed data

              This is to find out the number of compressed packets in the compressed 
              buffer to be overwritten for the new compressed packet to fit in. If the 
              packet present at the head has less size than new packet coming in then
              move head to next packet to create more space.
            */
            while(len_compressed_data_in_buffer < (compressed_length_aligned + sizeof(diagbuf_compressed_header_type)))
            {
              diagbufc_ihead += DIAGBUFC_NEXT_ALIGNED_BYTE(diagbufc_hdr_cir_mode->diagbufc_hdr.length + sizeof(diagbuf_compressed_header_type));
              diagbufc_hdr_cir_mode = (diagcompressed_data_type *)&diagcompressedbuf_buf[diagbufc_ihead];
              len_compressed_data_in_buffer += DIAGBUFC_NEXT_ALIGNED_BYTE(diagbufc_hdr_cir_mode->diagbufc_hdr.length + sizeof(diagbuf_compressed_header_type));
            }
            if((diag_tx_mode.byte_dropped_cnt + len_compressed_data_in_buffer) >= MAX_VALUE_UINT32)
              diag_tx_mode.byte_dropped_cnt = MAX_VALUE_UINT32;
            else
              diag_tx_mode.byte_dropped_cnt += len_compressed_data_in_buffer;

            diagbufc_itail = diagbufc_ihead + DIAGBUFC_NEXT_ALIGNED_BYTE(diagbufc_hdr_cir_mode->diagbufc_hdr.length + sizeof(diagbuf_compressed_header_type)); 



            /* If we have data in the buffer, check if it's full */
            osal_lock_mutex(&diag_compression_mutex);
            if(total_bytes_in_compressed_buffer > 0)
            {
              /* Update tail if it is overwritten */
              if(diagcompressedbuf_tail >= diagcompressedbuf_head)
              {
                //Case where tail is ahead of head and if the tail is over written, then only update tail, else tail may be ahead of head
                if(diagbufc_itail > diagcompressedbuf_tail)
                {
                  if( (diagbufc_itail + diagbufc_wrap_length) >= DIAGBUFC_SIZE )
                  {
                    diagbufc_itail = 0;
                  }
                  osal_lock_mutex(&diagbufc_tail_mutex);
                  diagcompressedbuf_tail = diagbufc_itail;
                  osal_unlock_mutex(&diagbufc_tail_mutex);
                }
              }
              else
              {
                /* If drain is not initialized then only update the tail */
                if(!bDrainInitialized)
                {
                  if(diagcompressedbuf_head >= diagcompressedbuf_tail)
                  {
                    if( (diagbufc_itail + diagbufc_wrap_length) >= DIAGBUFC_SIZE )
                    {
                      diagbufc_itail = 0;
                    }
                    osal_lock_mutex(&diagbufc_tail_mutex);                
                    diagcompressedbuf_tail = diagbufc_itail;
                    osal_unlock_mutex(&diagbufc_tail_mutex);
                  }
                }            
              }
            }
            osal_unlock_mutex(&diag_compression_mutex);

			/* Note: this puts a fully-formed packet into the buffer, complete with marking it used. */
            memscpy((void*)&diagcompressedbuf_buf[diagcompressedbuf_head], l4z_len_to_compress ,compressed_buffer_for_cir_mode, (sizeof(diagbuf_compressed_header_type)+ compressed_length_aligned));
            /*Need to mark the extra bytes as hole */
            diagbufc_hole_length = len_compressed_data_in_buffer - (compressed_length_aligned + sizeof(diagbuf_compressed_header_type));
            
            osal_lock_mutex(&diag_compression_mutex);

            temp_hdr = (diagcompressed_data_type*)&diagcompressedbuf_buf[diagcompressedbuf_head];
               
            /*Update the head pointer in the compressed buffer */
            diagcompressedbuf_head += sizeof(diagbuf_compressed_header_type) + compressed_length_aligned;
            ASSERT(diagcompressedbuf_head < DIAGBUFC_SIZE);

            if( (total_bytes_in_compressed_buffer + diagbufc_wrap_length + compressed_length_aligned + sizeof(diagbuf_compressed_header_type)) >= DIAGBUFC_SIZE)
            {   
              /*If Hole and wrap locations are adjacent ,then correctly update the total_bytes_in_compression_buffer*/
              if(DIAGBUFC_SIZE-diagcompressedbuf_head+sizeof(diagbuf_compressed_header_type) == diagbufc_hole_length)
              {
                total_bytes_in_compressed_buffer = DIAGBUFC_SIZE-diagbufc_hole_length + sizeof(diagbuf_compressed_header_type) ;
              }
              else
              {
                total_bytes_in_compressed_buffer = (DIAGBUFC_SIZE - diagbufc_wrap_length) - diagbufc_hole_length ;            
              }
            }
            else
            {
                total_bytes_in_compressed_buffer += (sizeof(diagbuf_compressed_header_type) + compressed_length_aligned);
            }
              
            temp_hdr->diagbufc_hdr.status = DIAGBUFC_USED_S;
            osal_unlock_mutex(&diag_compression_mutex);            

            if(diagbufc_hole_length != 0)
            {
              diagbufc_hdr_cir_mode = (diagcompressed_data_type*)&diagcompressedbuf_buf[diagcompressedbuf_head];
              diagbufc_hdr_cir_mode->diagbufc_hdr.cmd_code = DIAG_COMPRESSED_PKT;

                       
              if(diagbufc_hole_length > 4)
              {
                diagbufc_hdr_cir_mode->diagbufc_hdr.status = DIAGBUFC_HOLE_S;              
               #ifdef DIAG_COMPRESSION_ZLIB
                diagbufc_hdr_cir_mode->diagbufc_hdr.comp_ver = ZLIB_VERNUM;
                diagbufc_hdr_cir_mode->diagbufc_hdr.comp_algo = ZLIB_Compression;
               #else
                diagbufc_hdr_cir_mode->diagbufc_hdr.comp_ver = LZ4_VERNUM;
                diagbufc_hdr_cir_mode->diagbufc_hdr.comp_algo = LZ4_Compression;								
               #endif
                diagbufc_hdr_cir_mode->diagbufc_hdr.length = (diagbufc_hole_length - sizeof(diagbuf_compressed_header_type));
              }
               else
               {
                   diagbufc_hdr_cir_mode->diagbufc_hdr.status = DIAGBUFC_HOLE_SMALL_S;				
                   diagbufc_hdr_cir_mode->diagbufc_hdr.length = 0;
               }
            }
          }
          else if( (tx_mode == DIAG_TX_MODE_BUFFERED_THRESH) || (tx_mode == DIAG_TX_MODE_BUFFERED_CIR ))
          {
            if(tx_mode == DIAG_TX_MODE_BUFFERED_THRESH && diagbufc_wrap_count > 0)
            {
              /* If we move to Circular Buffering now, need to set the head to point to right location first*/
              bCircularBufferHeadSet = FALSE; 
            }
            osal_lock_mutex(&diag_compression_mutex);

            diagcompressedbuf_head = diagcompressedbuf_head + (sizeof(diagbuf_compressed_header_type) + compressed_length_aligned);
            ASSERT(diagcompressedbuf_head < DIAGBUFC_SIZE);
            total_bytes_in_compressed_buffer += (sizeof(diagbuf_compressed_header_type) + compressed_length_aligned);

            compressed_data->diagbufc_hdr.status = DIAGBUFC_USED_S;
            osal_unlock_mutex(&diag_compression_mutex);
          }

          compressed_data = NULL;
          compressed_length = 0;
          total_length_to_compress = 0;
          

#ifdef DIAG_COMPRESSION_ZLIB
          deflateEnd(&zstream);
          bIszlibInitialized = FALSE;               
          zlib_init_ret = -1;
          bIsBufferAllocated = FALSE;        
#else
          buffer_to_compress_index = 0;
#endif
        }
      }
#ifdef DIAG_COMPRESSION_ZLIB        
    }
#endif
}

/*===========================================================================
FUNCTION DIAGCOMPRESSEDBUF_DRAIN

DESCRIPTION
 This function drains the compressed chunks from the compressed buffer.
 
PARAMETERS

SIDEEFFECTS

RETURN VALUES
============================================================================*/

void diagcompressedbuf_drain(void)
{
  static dsm_item_type* item_ptr = NULL;
  diagbuf_compressed_header_type *compressed_data_hdr;   
  int itail =0 ;

  int compressed_data_length = 0;
  diag_send_desc_type send_desc = {NULL, NULL, DIAG_SEND_STATE_START, FALSE, FALSE};

  static diag_hdlc_dest_type hdlc_enc = {NULL, NULL, 0};
  static byte* outbound_sav = NULL;
  byte* outbound = NULL;
  int pad = 0;
  int temp = 0;
  static uint32 outbound_used = 0;
  boolean dsm_full = FALSE;
  int diagbufc_hole_length = 0;
  static boolean first_packet = TRUE;
  static boolean new_chain = FALSE;

    osal_lock_mutex(&diagbufc_tail_mutex);
    itail = diagcompressedbuf_tail;


  outbound = outbound_sav;
  compressed_data_hdr = (diagbuf_compressed_header_type*)&diagcompressedbuf_buf[itail];
  bDrainInitialized = TRUE;
     
  if(compressed_data_hdr->status == DIAGBUFC_WRAP_S)
  {
    itail = 0;
    diagcompressedbuf_tail = itail;
  }
  else if(compressed_data_hdr->status == DIAGBUFC_HOLE_S)
  {
    diagbufc_hole_length = compressed_data_hdr->length + sizeof(diagbuf_compressed_header_type);
    itail = DIAGBUFC_NEXT_ALIGNED_BYTE(itail + diagbufc_hole_length);
    if(itail >= DIAGBUFC_SIZE)
      itail = 0;
    diagcompressedbuf_tail = itail;
  }
  else if(compressed_data_hdr->status == DIAGBUFC_HOLE_SMALL_S)
  {
    diagbufc_hole_length = 4;
    itail = DIAGBUFC_NEXT_ALIGNED_BYTE( itail + diagbufc_hole_length );
    if(itail >= DIAGBUFC_SIZE)
      itail = 0;
    diagcompressedbuf_tail = itail;
  }
  else if(compressed_data_hdr->status == DIAGBUFC_USED_S)
  {
    compressed_data_length = compressed_data_hdr->length + sizeof(diagbuf_compressed_header_type);

    /* send_desc.pkt to point to the data starting from cmd_code */
    send_desc.pkt = (void*)&diagcompressedbuf_buf[itail + FPOS(diagbuf_compressed_header_type,cmd_code)];
    send_desc.last = (void*)&diagcompressedbuf_buf[itail + compressed_data_length - 1];
    send_desc.terminate = TRUE;
    send_desc.state  = DIAG_SEND_STATE_START;
    send_desc.priority = FALSE;

    //update itail to point to the end of the data      
    itail = itail  + compressed_data_length ; //+ FPOS(diagcompressed_data_type, comp_data);      
    temp = itail;

    itail = DIAGBUFC_NEXT_ALIGNED_BYTE(itail);


    pad = itail - temp;
    if(itail >= DIAGBUFC_SIZE)
      itail = 0;

    osal_lock_mutex(&diag_compression_mutex);
    total_bytes_in_compressed_buffer -= (compressed_data_length + pad);
    osal_unlock_mutex(&diag_compression_mutex);

    if (outbound == NULL)
    {
      /* Allocate memory from the communications layer */
      item_ptr = diagcomm_outbound_alloc_compressed ((unsigned int*)&outbound_used, FALSE, send_desc.priority);   
      new_chain = FALSE;

      if (item_ptr)
      outbound = item_ptr->data_ptr;
      else 
      outbound = NULL;

      hdlc_enc.dest = (void*)outbound;
      hdlc_enc.dest_last = (void*)((byte*)outbound + outbound_used - 1);
    }

    if (outbound != NULL)
    {
      int hdlc_pad = DIAG_COMPRESS_HDLC_ESTIMATE;

      outbound_used = (uint32) hdlc_enc.dest - (uint32) outbound;

      /* The condition below checks if the packet is a packet whose source size won't
      fit in the existing DSM item size, AND either that the packet is not a large packet
      which wouldn't fit in the DSM or that packet is too big to fit in this DSM combined 
      with a single newly-allocated packet.  If so, send the existing data right away
      and allocate a new buffer for this packet to avoid unnecessary chaining */
      if (((((int)send_desc.last - (int)send_desc.pkt) + outbound_used + hdlc_pad) >= DIAGBUF_SIO_TX_ITEM_SIZ)
      && (TRUE == first_packet) && (!send_desc.priority) && (((int)send_desc.last - (int)send_desc.pkt) <= DIAGBUF_SIO_TX_ITEM_SIZ
             || (hdlc_pad + outbound_used + (int)send_desc.last - (int)send_desc.pkt) >= (DIAGBUF_SIO_TX_ITEM_SIZ * 2)))
      {   
        /* We are assuming that this check always passes, but,
        if it doesn't, any existing data will get discarded. */
        ASSERT(item_ptr != NULL);
        if (item_ptr)
        {
          item_ptr->used = outbound_used;
          diagcomm_send_compressed(item_ptr, outbound_used);
        }
        outbound = NULL; 

        item_ptr = diagcomm_outbound_alloc_compressed ((unsigned int*)&outbound_used, FALSE, FALSE);
        if (item_ptr)
          outbound = item_ptr->data_ptr;
        else
          outbound = NULL;

        hdlc_enc.dest = (void *)outbound;
        hdlc_enc.dest_last = (void *) ((byte *) outbound + outbound_used - 1);
      }

      if (outbound) 
      {
        diag_hdlc_encode (&send_desc, &hdlc_enc);
      }

      /* Check for case if exactly DSM SIZE or DSM_SIZE - 1 bytes are in dsm, then packet is full,
      needs to be sent right away */
      if ((hdlc_enc.dest >= hdlc_enc.dest_last) && (send_desc.pkt > send_desc.last ) && send_desc.state == DIAG_SEND_STATE_COMPLETE) 
      {
        dsm_full = TRUE;
      }
      else 
      /* Check to see if the packet fit, if not loop through and chain the end of packet*/
      while  ((hdlc_enc.dest >= hdlc_enc.dest_last) && (send_desc.state != DIAG_SEND_STATE_COMPLETE)
      && (item_ptr)) 
      {
        outbound_used = (uint32) hdlc_enc.dest - (uint32) outbound;
        /* Update the used field for the current DSM item */
        item_ptr->used = outbound_used;

        item_ptr = diagcomm_outbound_alloc_compressed ((unsigned int*)&outbound_used, TRUE, send_desc.priority);
        diagbufc_new_chain++;
        if (item_ptr) 
        {
          outbound = item_ptr->data_ptr;    
          new_chain = TRUE;
          hdlc_enc.dest = (void *) outbound;
          hdlc_enc.dest_last = (void *) ((byte *) outbound + outbound_used - 1);


          /* Fit the remainder of the packet */
          diag_hdlc_encode (&send_desc, &hdlc_enc);       
        }
        else outbound = NULL;
      }

      /* If it is a new chain and a terminal packet - send the data OR
      if it is a priority packet send the data */
      if (((send_desc.terminate) && (new_chain || dsm_full) && (item_ptr)) || ((send_desc.priority) && (item_ptr))) 
      {
        outbound_used = (uint32) hdlc_enc.dest - (uint32) outbound;
        item_ptr->used = outbound_used;   


        diagcomm_send_compressed(item_ptr, outbound_used);

        outbound = NULL;
        new_chain = FALSE;
        dsm_full = FALSE;
      }

      if (send_desc.terminate) 
      {
        first_packet = TRUE;
      }
      else 
      {
        first_packet = FALSE;
      }
    }

    diagcompressedbuf_tail = itail;
    ASSERT(diagcompressedbuf_tail < DIAGBUFC_SIZE);

    compressed_data_hdr->status = DIAGBUFC_DRAINED_S;

    outbound_sav = outbound;
  }
  else if(compressed_data_hdr->status != DIAGBUFC_UNUSED_S)
  {
    //Don't crash if this is one of the case where we genuinely want to come back later.
    ERR_FATAL("Unexpected buffer header %d", compressed_data_hdr->status, 0, 0);
  }

  osal_unlock_mutex(&diagbufc_tail_mutex);
}

/*===========================================================================
FUNCTION DIAG_PER_COMPRESSION_BUFFER_FULL
DESCRIPTION
 This function returns percentage compression buffer full.
 
PARAMETERS
SIDEEFFECTS
RETURN VALUES
  Percentage Compression Buffer Full.
============================================================================*/

uint8 diag_per_compression_buffer_full()
{
  return ((total_bytes_in_compressed_buffer *100)/(DIAGBUFC_SIZE));
}

/*===========================================================================
FUNCTION DIAG_COMPRESSION_BYTES_INTERNAL_BUFFER
DESCRIPTION
 This function returns the number of bytes in the internal buffer to be compressed.
 
PARAMETERS
SIDEEFFECTS
RETURN VALUES
  Number of Bytes in the internal buffer which are yet to undergo compression.
============================================================================*/
uint32 diag_compression_bytes_internal_buffer()
{
  return total_length_to_compress;
}

uint32 diagcompressed_buffer_free_bytes()
{
   int used_bytes = 0;
   used_bytes = DIAGBUFC_USED(diagcompressedbuf_head, diagcompressedbuf_tail);
   
   return (DIAGBUFC_SIZE - used_bytes);
}

#ifdef DIAG_COMPRESSION_ZLIB


/*===========================================================================

FUNCTION DIAG_ZERR

DESCRIPTION
  This function handles the different error codes from zlib compression.
  
PARAMETERS:
  ret: ZLIB Error code

DEPENDENCIES

============================================================================*/

void diag_zerr(int ret)
{

   zstream_err = ret;
   switch (ret) {
      case Z_ERRNO:
         MSG_1("File Operation Error = %d",Z_ERRNO);
      break;
      case Z_STREAM_ERROR:
         MSG_1("Invalid Data Stream Error = %d",Z_STREAM_ERROR );
      break;
      case Z_DATA_ERROR:
         MSG_1("Corrupted Data Error = %d",Z_DATA_ERROR);
      break;
      case Z_MEM_ERROR:
         MSG_1("Out Of Memory Error = %d",Z_MEM_ERROR );
      break;
      case Z_VERSION_ERROR:
         MSG_1("Incompatible ZLIB Version Error = %d",Z_VERSION_ERROR);
      break;
      case Z_BUF_ERROR:
         MSG_1("Incompatible Buffer Error = %d",Z_BUF_ERROR);
      break;
   }
}
#endif

