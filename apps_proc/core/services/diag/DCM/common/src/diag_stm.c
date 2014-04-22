/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         Diagnostics over STM

General Description
This routines in this file are used to send data over STM using the debugtrace
interfaces.

Copyright (c) 2013-2014,2016-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diag_stm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/17   gn      Modified control flow when ASSERT is disabled in diag_stm_write_pkt
07/20/16   nk      Changes to support ThreadX
07/06/14   ph      Fixed KW errors and compilation errors.
04/24/14   tbg     Added support for extended Diag APIs.
09/20/13   tbg     File creation
===========================================================================*/

#include "comdef.h"
#include "tracer.h"

#include "diagdiag_v.h"
#include "diagcmd.h"
#include "diagbuf_v.h"
#include "diagi_v.h"
#include "assert.h"

#include "diag_stm.h"
#include "osal.h"
#include <stdlib.h> /*For malloc api*/


#include <stringl/stringl.h>  /* For memscpy */

#ifdef FEATURE_DIAG_STM
/* Safe guard to ensure file is not compiled if feature is not enabled */


/* Local variable declarations */
static boolean diag_stm_initialized = FALSE; // Indicates if diag tracer has been initialized
boolean diag_stm_enabled = FALSE; // Transmit data over STM

boolean diag_pthread_initialized = FALSE; // TLS key initialized for STM. Note: Must be done before diag_stm_enabled can be set to TRUE.
osal_tls_key_t diag_tracer_tls_key; // TLS key for diag tracer handle;

typedef struct
{
  uint32 id;
  diagbuf_header_type diagbuf_hdr;
} diag_stm_log_v1_pre_header;

/* additional space required to hold data in an extended log packet in STM.
   This should match the size of whatever is included in stm_log_v1_pre_header, minus the
   diagbuf_header_type itself
   */

#define DIAG_STM_LOG_V1_OFFSET 4

typedef union
{
  diag_stm_log_v1_pre_header pre_hdr;
  diag_cmd_ext_v1_type post_hdr;
} diag_stm_log_v1_hdr;

   /*  How extended packets are handled when allocated by Diag STM.
       diag_stm_log_v1_hdr is a union used to help map these two use cases.
       This allows us to allocate a buffer once and reuse it withotu having to copy the entire
       packet to a new buffer.

       Initial allocation:
       ----------------------------------------------------------------------------------------
       | ID (4 bytes)                     |  Diagbuf_hdr (4 bytes) | Log hdr and log  (N bytes)|
       ----------------------------------------------------------------------------------------
       (diag_stm_log_v1_hdr.pre_hdr)

       Prior to sending to STM, reclaim diagbuf_hdr space and repurpose to match v1 header:
       -----------------------------------------------------------------------------------------
       | CMD CODE, Ver, Proc ID (4 bytes) |       ID (4 bytes)     | Log hdr and log  (N bytes)|
       -----------------------------------------------------------------------------------------
       (diag_stm_log_v1_hdr.post_hdr)
       */

/* Local #defines */
#define TRACER_PROTOCOL_DIAG 1 // Protocol ID value; not used by tracer interface.

/* Defines used for enabling Diag over STM */
#if (defined (DIAG_IMAGE_MODEM_PROC) || defined(DIAG_SINGLE_PROC)) && !defined (DIAG_IMAGE_QDSP6_PROC)
  /* The ARM9 modem and NOT QDSP6 modem */
  #define DIAG_STM_PROC_MASK DIAG_STM_PROC_MASK_MODEM
#elif defined (DIAG_IMAGE_APPS_PROC) && !defined (DIAG_IMAGE_QDSP6_PROC)
  /* Brew apps and WM apps */
  #define DIAG_STM_PROC_MASK DIAG_STM_PROC_MASK_APPS
#elif defined(DIAG_IMAGE_QDSP6_PROC)
  /* QDSP6 modem */
  #define DIAG_STM_PROC_MASK DIAG_STM_PROC_MASK_ADSP
#elif defined(DIAG_IMAGE_RIVA_PROC)
  /* RIVA */
  #define DIAG_STM_PROC_MASK DIAG_STM_PROC_MASK_WCNSS
#endif


/*===========================================================================

FUNCTION DIAG_STM_INIT

DESCRIPTION
  Prepares Diag to be able to send traffic over STM. Note this does not enable
  Diag traffic over STM when called, it simply allows Diag to switch it on
  when needed.

  Specific init steps taken:
    1. Register any command handlers needed for enabling STM (if applicable)
    2. Create the TLS key used to store tracer handlers

  diag_stm_initialized will be set to TRUE only if all steps are successfully
  completed, especially the TLS key creation.

PARAMETERS
  None

DEPENDENCIES
  None

===========================================================================*/
void diag_stm_init(void)
{
  int rval;

  if(!diag_stm_initialized)
  {
    /* Create TLS key for storage of Diag tracer client handles.
       This key MUST be created before other Diag calls try to access
       the Diag tracer TLS information.
     */

    rval = osal_tls_key_create(&diag_tracer_tls_key, NULL);

    if(rval == OSAL_SUCCESS) // Success
    {
      diag_stm_initialized = TRUE;
    }
  }
}


/*===========================================================================

FUNCTION DIAG_STM_WRITE_PKT

DESCRIPTION
  Writes the specified packet to STM using the tracer APIs using the tracer
  handle that exists for this thread in TLS. If a handle does not exist, one
  is created and then stored using TLS.

PARAMETERS
  ptr      Pointer to data to be written
  length   Number of bytes to write from ptr

DEPENDENCIES
  diag_stm_initliazed is TRUE

===========================================================================*/
void diag_stm_write_pkt(void* ptr, uint32 length)
{
  tracer_return_enum_t rval;
  static tracer_options_t diag_tracer_options = TRACER_OPT_TIMESTAMP;
  static uint32 diag_tracer_reg_error_cnt = 0;
  static uint32 diag_tracer_write_error_cnt = 0;
  tracer_client_handle_t client_handle=NULL;
  boolean write_flag = TRUE;

  if(( ptr != NULL) && diag_stm_initialized && diag_stm_enabled && (length > 0))
  {
    /* Check if tracer handle has been written into TLS */
    client_handle = osal_tls_getspecific(diag_tracer_tls_key);
    if(client_handle == NULL)
    {
      write_flag = FALSE;
      /* No handle exists; need to create a handle for this thread */
      rval = tracer_register(&client_handle, TRACER_ENTITY_DIAG, TRACER_PROTOCOL_DIAG);
      if (rval == TRACER_RETURN_SUCCESS)
      {
        if(osal_tls_setspecific(diag_tracer_tls_key, (void*)client_handle) != OSAL_SUCCESS)
        {
          return;
        }
        write_flag = TRUE;
      }
      else
      {
        // tracer didn't intiialize -- why?
        // Don't send error MSG, as it may result in an recursive loop
        // Increment debug counter for tracer registration
        diag_tracer_reg_error_cnt++;
      }
    }
    /* Send packet to tracer */
    if(write_flag)
    {

      rval = tracer_data(client_handle, diag_tracer_options, length, ptr);

      if (rval != TRACER_RETURN_SUCCESS)
      {
        // If write fails, increment debug counter for write failures
        diag_tracer_write_error_cnt++;
      }
    }
  }
}

/*===========================================================================

FUNCTION DIAG_STM_CONTROL_MODE

DESCRIPTION
  Used to enable or disable Diag over STM. Function will only allow it to be
  enabled if diag_stm_initialized is TRUE.

PARAMETERS
  mode_info    parameter indicating enable/disable of Diag over STM
               (DIAG_STM_ENABLE or DIAG_STM_DISABLE)

DEPENDENCIES

RETURN
  Returns the current state of Diag over STM
    DIAG_STM_DISABLE  Diag STM is currently disabled
    DIAG_STM_ENABLE   Diag STM is currently enabled

===========================================================================*/
uint8 diag_stm_control_mode(uint8 mode_info)
{
  uint8 rval = DIAG_STM_DISABLE;

  if(diag_stm_initialized)
  {
    if (mode_info == DIAG_STM_ENABLE)
    {
      diag_stm_enabled = TRUE;
      rval = DIAG_STM_ENABLE;
    }
    else if (mode_info == DIAG_STM_DISABLE)
    {
      diag_stm_enabled = FALSE;
      rval = DIAG_STM_DISABLE;
    }
    else
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid paramater receieved in diag_control_stm_mode");
      if (diag_stm_enabled)
      {
        rval = DIAG_STM_ENABLE;
      }
    }
  }
  else
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_stm_control_mode called but Diag STM is not initialized");
  }
  return rval;
}

/*===========================================================================

FUNCTION DIAG_STM_LOG_EXT_HANDLER

Function for 'extended' log packets which need the extended header populated
prior to transmission.

In the case a packet is allocated from diagbuf (and not using STM malloc),
An addition copy will be required to assemble the full packet so that it
can be passes to the tracer APIs.

===========================================================================*/
void diag_stm_log_ext_handler(void* pkt_ptr)
{
  /* pkt_ptr points to the head of the diagbuf_header */
  diagbuf_header_type *diagbuf_hdr2 = (diagbuf_header_type*)pkt_ptr;

  ASSERT(diagbuf_hdr2->version == DIAGBUF_VERSION_1);


  /* Need to check if this allocated from STM or from diagbuf.*/
  if(diagbuf_hdr2->status == DIAGBUF_STM_BUFFER_S)
  {
    /* STM Buffer allocation, so memory for packet already exists.
       Check with diag_stm_log_v1_hdr comments at the top of the file for details.
       */
    diag_stm_log_v1_hdr* ext_pkt_hdr = (diag_stm_log_v1_hdr*)((char*)diagbuf_hdr2 - DIAG_STM_LOG_V1_OFFSET);
    diagbuf_header_type orig_diagbuf_hdr;
    uint32 id;
    uint32 length;

    /* Backup original header details */
    memscpy(&orig_diagbuf_hdr,sizeof(diagbuf_header_type),
            &(ext_pkt_hdr->pre_hdr.diagbuf_hdr),sizeof(diagbuf_header_type));

    /* Read ID field, currently stored at head of allocation */
    id = ext_pkt_hdr->pre_hdr.id;

    /* Read original length in pkt_header */
    length = ext_pkt_hdr->pre_hdr.diagbuf_hdr.length;

    /* Populated extended header */
    ext_pkt_hdr->post_hdr.id = id;
    ext_pkt_hdr->post_hdr.cmd_code = DIAG_CMD_EXT_F;
    ext_pkt_hdr->post_hdr.version = DIAGBUF_VERSION_1;
    ext_pkt_hdr->post_hdr.proc_id = DIAG_MY_PROC_ID;

    /* Calculate new length, including the extended header */
    length += sizeof(diag_cmd_ext_v1_type);

    diag_stm_write_pkt(ext_pkt_hdr,length);

    /* Need to restore original header so calling function can correctly ID this as an STM allocation */
    memscpy(&ext_pkt_hdr->pre_hdr.diagbuf_hdr,sizeof(diagbuf_header_type),
            &orig_diagbuf_hdr,sizeof(diagbuf_header_type));

  }
  else
  {
    /* diagbuf buffer. Full packet needs to be populated in a new buffer which can be
       passed to the tracer APIs. */
    void* full_ext_log_pkt;
    diag_cmd_ext_v1_type *ext_hdr;
    uint32 full_length = 0;
    uint8* cpy_ptr = NULL;
    diagbuf_header_type *diagbuf_hdr1;


    /* Calculate length of full packet, and allocate needed memory */
    full_length = diagbuf_hdr2->length + sizeof(diag_cmd_ext_v1_type);
    full_ext_log_pkt = (void *)diag_stm_alloc(full_length);

    /* Packet is in two segments currently; Need to reassemble into a single buffer */

    if(full_ext_log_pkt)
    {
        diagbuf_hdr1 = diagbuf_hdr2 - diagbuf_ver_len[DIAGBUF_VERSION_1-1];
        ext_hdr = (diag_cmd_ext_v1_type*)diagbuf_hdr1;

        diagbuf_hdr1 -= 1;

        ext_hdr->cmd_code = DIAG_CMD_EXT_F;
        cpy_ptr = (uint8 *)full_ext_log_pkt;
        memscpy((void*)cpy_ptr,full_length,(void *)ext_hdr,sizeof(diag_cmd_ext_v1_type));
        cpy_ptr += sizeof(diag_cmd_ext_v1_type);

        memscpy((void*)cpy_ptr,full_length-sizeof(diag_cmd_ext_v1_type),
                (void*)(diagbuf_hdr2+1),diagbuf_hdr2->length);

        diag_stm_write_pkt(full_ext_log_pkt,full_length);
        diag_stm_free(full_ext_log_pkt);
    }
  }
}

/*===========================================================================

FUNCTION DIAG_STM_LOG_ALLOC

DESCRIPTION
  Allocates a log packet using malloc, intended to be transmitted using the
  tracer APIs and then freed. Logs allocated using this API are not intended to
  be processed by the diag task.

  For consistency in other log handling APIs, the method for allocating the logs
  and the headers used are consistent with the standard diagbuf_alloc function.

  The status member of the diagbuf header is set to DIAGBUF_STM_BUFFER_S to
  allow other Diag APIs to know where the buffer was allocated from.

PARAMETERS
  code                 ID of the log to be reported
  length               The specified length is the length of the entire log,
                       including the log header.
  version              Extended log version number.
                         0 (DIAGBUF_VERSION_0) indicates regular log packet
                         1 (DIAGBUF_VERSION_1) indicates log packet with extended header (v1)
  parameter            Parameter associated with extended packets
DEPENDENCIES

===========================================================================*/
PACK(void *) diag_stm_log_alloc (uint8 code, uint16 length, uint8 version, void *parameter)
{
  uint32 alloc_length;
  PACK(void *) ptr = NULL; /* Pointer to return.        */
  diagbuf_header_type *pkt_hdr;

  if(length==0)
  {
    /* Return NULL if an empty log packet is being requested */
    return NULL;
  }

  alloc_length =
    sizeof(diagbuf_header_type) +  /* Diagbuf header    */
      (uint32)length;     /* Number of bytes requested buffer */

    if (version == DIAGBUF_VERSION_1)
    {
      /* Allocate addition length needed for header. See comments at top of file for more details
         on how this is used. */
      if (parameter == NULL)
      {
        /* No parameter passed with extended v1 packet */
        return NULL;
      }
      alloc_length += DIAG_STM_LOG_V1_OFFSET;
    }

    ptr = malloc(alloc_length);

    if (ptr)
    {
        /* Set header field of buffer */
        pkt_hdr = ((diagbuf_header_type *) ptr);

        if (version == DIAGBUF_VERSION_1)
        {
          diag_stm_log_v1_hdr* ext_pkt_hdr = (diag_stm_log_v1_hdr*)pkt_hdr;

          /* Store ID into initial address */
          ext_pkt_hdr->pre_hdr.id = *((uint32*)parameter);

          pkt_hdr = &ext_pkt_hdr->pre_hdr.diagbuf_hdr;
          pkt_hdr->chain = DIAGBUF_SECOND_PKT_CHAIN;
        }
        else
        {
          pkt_hdr->chain = DIAGBUF_NO_CHAIN;
        }

        pkt_hdr->status = DIAGBUF_STM_BUFFER_S;
        /* Note: pkt_hdr->pad not needed for STM implementation, since this allocation is not
           going into diagbuf */
        pkt_hdr->length = length;
        pkt_hdr->version = version;

        /* Set 'ptr' to pkt_hdr +1 = data following pkt_hdr */
        ptr = pkt_hdr + 1;

        diagpkt_set_cmd_code (ptr, code);
    }

    return ptr;
} /* diag_stm_log_alloc */


/*===========================================================================

FUNCTION DIAG_STM_ALLOC

DESCRIPTION
  Allocates a buffer using malloc, intended to be transmitted using the
  tracer APIs and then freed. Packets allocated using this API are not intended to
  be processed by the diag task.

  This is a direct wrapper for malloc (no additional headers added, etc.)
  Intended use case is allocating memory for messages and event reports, where
  diag will be determining the exact size of the final packet.

PARAMETERS
  length      Length of buffer to allocate

DEPENDENCIES


===========================================================================*/
PACK(void *) diag_stm_alloc(uint32 length)
{
  void *ptr = NULL;
  if(length > 0)
  {
    ptr = malloc(length);
  }
  return ptr;
}

/*===========================================================================

FUNCTION DIAG_STM_FREE

DESCRIPTION
  Frees a buffer allocated using diag_stm_alloc

PARAMETERS
  ptr      Pointer to free

DEPENDENCIES

===========================================================================*/
void diag_stm_free(void *ptr)
{
  free(ptr);
}

/*===========================================================================

FUNCTION DIAG_STM_LOG_FREE

DESCRIPTION
  Frees a buffer allocated using diag_stm_log_alloc (accounts for diagbuf header)

PARAMETERS
  ptr      Pointer to free

DEPENDENCIES

===========================================================================*/
void diag_stm_log_free(void *ptr)
{
  diagbuf_header_type *pkt_hdr;

  if(ptr)
  {
    pkt_hdr = ((diagbuf_header_type *) ptr) - 1;

    /* Check if this is an extended header */
    if (pkt_hdr->chain == DIAGBUF_SECOND_PKT_CHAIN)
    {
      char* head_ptr = (char*)pkt_hdr;

      /* Only version 1 supported currently */
      ASSERT(pkt_hdr->version == DIAGBUF_VERSION_1);

      /* Account for offset before freeing pointer */
      head_ptr -= DIAG_STM_LOG_V1_OFFSET;
      free(head_ptr);
    }
    else
    {
        free(pkt_hdr);
    }
  }
}
#endif /* FEATURE_DIAG_STM */
