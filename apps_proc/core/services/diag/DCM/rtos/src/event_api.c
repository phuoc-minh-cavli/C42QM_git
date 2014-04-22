
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            Event Service

General Description
  This file containsthe implementation for the event reporting service.

  An event consists of a 12 bit event ID and a timestamp.

  The requirement is that the DMSS will never drop events as long as it does
  not exceed bandwidth limitations with event reporting.  Event reporting
  has the highest priority of all unsolicited diagnostic data.

  To minimize impact of the caller's task, the event is generated and placed
  in a queue.  The DIAG task will build the final packet structure and send
  to the external device.

  Just in case an event is dropped, a "drop event" will be reported and a
  count of dropped events will be reproted as payload.

Initialization and Sequencing Requirements
  Call event_init() before reporting any events.

Copyright (c) 2011-2015, 2019-2020 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                              Edit History

$Header: //components/rel/core.tx/6.0/services/diag/DCM/rtos/src/event_api.c#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/16/15   sa      Removed the references of mobile view code.
04/24/14   tbg     Fixed issues with extended event pkts. 
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0
10/14/13   sr      Added support for extended event, log and F3 pkts
10/01/13   xy      Removed code under FEATURE_WINCE 
09/20/13   tbg     Added support for Diag over STM 
04/22/13   ph      converted diag_listeners heap usage to system heap.
04/03/13   sr      Replace memcpy() and memmove() with memscpy() and memsmove() 
08/06/12   rh      Moved event listeners to using diag_listeners_malloc and free 
04/27/11   is      Resolve compiler warnings
10/29/10   vs      Changes by debugtools for lock-less implementation
09/28/10   sg     Moved diag_time_get to diag_cfg.h
09/17/10   is      Migrate from deprecated clk driver APIs
08/25/10   sg      Fixed compiler warnings
06/30/10   sg      Moved the inclusion of diagi_v.h up for DIAG_QDSP6_APPS_PROC
                   to be defined earlier before using it on adsp
04/20/10   sg     Created file.

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "event.h"
#include "eventi.h"
#include "msg.h"
#include "osal.h"
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  #include "diag_f3_tracei.h"
  #include "err.h"
#endif
#include <stringl/stringl.h>
#include "diagtarget.h"
#include "diagdiag_v.h" 
#include "diag_cfg.h"
#include "diagi_v.h"

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"

#include "dog.h"

#endif

#include "diag_stm.h"

#ifdef DIAG_LISTENERS_SUPPORT
  #define EVENT_CONFIG_LISTENER_MASK 0x02
  void event_service_listener (event_store_type *event_store);
  typedef struct
  {
    diag_event_listener listener;
    void *param; /* User's parameter */
  } event_listener_entry_type;
  static diag_searchlist_type event_listeners = {FALSE, {0}, 0, 0, 0, 0, NULL};
#endif /* DIAG_LISTENERS_SUPPORT */

#if defined (FEATURE_SAVE_DEBUG_TRACE)
  #define EVENT_SET_UP_HEADER(x) ((ERR_F3_TRACE_EVT_BIT << 13) | x)
 
extern osal_mutex_arg_t diag_debug_record_buf_cs;

#endif

extern byte event_read_stream_id(event_id_enum_type event_id);

/* This data member is used for the event listener functinality.
   The API is in diag.h. */
/*lint -save -e{785} */

extern uint8 event_config;
#ifdef FEATURE_DEBUG_DIAG_HEAP

  #ifdef DIAG_LISTENERS_SUPPORT
    uint32 event_listener_match_and_free_cnt =0;
    uint32 diag_add_event_listener_malloc_cnt = 0;
    uint32 diag_add_event_listener_free_cnt = 0;
  #endif /* DIAG_LISTENERS_SUPPORT */

#endif /* FEATURE_DEBUG_DIAG_HEAP */

/*lint -restore */

#if  defined (FEATURE_SAVE_DEBUG_TRACE)
/*=========================================================================*/
void
event_save_trace
(
  event_id_enum_type event_id_enum,
  evt_payload_len_type length,
  void* payload,
  event_store_type* diag_event
)
{
  /* No longer supporting event save to F3 trace buffer */
 
  return;
}                               /* event_save_trace */

#endif /* defined (FEATURE_SAVE_DEBUG_TRACE) */
/*=========================================================================*/
#if defined FEATURE_DIAG_STM
/*===========================================================================

FUNCTION EVENT_GENERATE_STM

DESCRIPTION
  Generate STM event report for event being generated, and send immediately

PARAMETERS
  event_id      ID of the event to be generated
  length        Length of the payload pointed to by 'payload'
  *payload      Pointer to the event payload
  version       Extended packet version information
                  0 - Standard event packet (no extended header)
                  1 - Extended packet event (v1)
  *parameter    Parameter associated with extended packets

=============================================================================*/
static void event_generate_stm (event_id_enum_type event_id, uint8 length, void *payload, unsigned int version, void *parameter)
{
  event_rpt_type stm_event_hdr;
  uint8 *event_pkt, *ptr;
  event_id_type stm_event_id;
  uint32 pkt_length;
  uint32 copy_bytes;
  uint32 hdr_size = sizeof(event_rpt_type)-1;
  qword diag_ts;
  diag_cmd_ext_v1_type *event_ext_hdr = NULL;

  /* Calculate packet length for event report (single event) */
  pkt_length = sizeof(event_rpt_type) - 1;
  pkt_length += sizeof(event_type);
  pkt_length += length;
  if (length > EVENT_PAY_TWO_BYTE)
  {
    /* include extra byte for payload length field */
    pkt_length += 1;
  }

  /* Check if this is an extended report and allow space for header */
  if (version == DIAGBUF_VERSION_1)
  {
    if (parameter == NULL)
    {
      /* No parameter includes with v1...return without sending anything */
      return;
    }
    pkt_length += sizeof(diag_cmd_ext_v1_type);
  }

  /* Allocate packet for event report */
  event_pkt = (uint8 *)diag_stm_event_alloc(pkt_length);

  if (event_pkt)
  {
    ptr = event_pkt;

    if (version == DIAGBUF_VERSION_1)
    {
      /* Populate extended header information*/
      event_ext_hdr = (diag_cmd_ext_v1_type*)event_pkt;
      event_ext_hdr->cmd_code = DIAG_CMD_EXT_F;
      event_ext_hdr->id = *(uint32*)parameter;
      event_ext_hdr->version = DIAGBUF_VERSION_1;
      event_ext_hdr->proc_id = DIAG_MY_PROC_ID;

      /* Move pionter past header to start of event report */
      ptr += sizeof(diag_cmd_ext_v1_type);
    }

    /*  Note: Not accounting for dropped STM events here if alloc fails.
        Should be added?
     */

    /* Set fields of event_report */
    stm_event_hdr.cmd_code = DIAG_EVENT_REPORT_F;
    stm_event_hdr.length = pkt_length - hdr_size;

    copy_bytes = memscpy(ptr,pkt_length,&stm_event_hdr,hdr_size);
    ptr += copy_bytes;

    stm_event_id.id = event_id;
    if (length <= EVENT_PAY_TWO_BYTE)
    {
      stm_event_id.payload_len = length;
    }
    else
    {
      stm_event_id.payload_len = EVENT_PAY_LENGTH;
    }
    stm_event_id.time_trunc_flag = FALSE;

    copy_bytes = memscpy(ptr,(pkt_length-(ptr-event_pkt)),
                          &stm_event_id,sizeof(event_id_type));
    ptr += copy_bytes;

    diag_time_get(diag_ts);
    copy_bytes = memscpy(ptr,(pkt_length-(ptr-event_pkt)),diag_ts,sizeof(qword));
    ptr += copy_bytes;

    if (payload)
    {
      if (length > EVENT_PAY_TWO_BYTE)
      {
        *ptr++ = length;
      }
      /* Copy payload */
      copy_bytes = memscpy(ptr,(pkt_length-(ptr-event_pkt)),payload,length);
    }

    /* Send event report to STM */
    diag_stm_write_pkt((void*)event_pkt,pkt_length);

    /* Free event report */
    diag_stm_event_free((void*)event_pkt);
  }
}
#endif /*/ FEATURE_DIAG_STM */


/*===========================================================================
FUNCTION EVENT_REPORT_EXT

DESCRIPTION
   Report an event without payload data . Similar to event_report() except
   that this function can take additional parameters - version and void * to
   specify information regarding where the event is being generated.

PARAMETERS
   event_id       ID of the event to be reported.Event IDs are defined in event_defs.h
   version        Specifies the version - currently, there are two versions
                  0 - same as event_report() (for backwards compatibility)
                  1 - The outgoing event report header will have support to
                      include ID field to differentiate the events coming
                      from different instances of the processor.
  *parameter      Based on the version passed in, this pointer is interpreted
                  as follows-
                  version       pointer
                  0             NULL
                  1             uint32 * . The uint32 value this pointer points to,
                                is the ID of the instance generating this event.

RETURN VALUE
  TRUE: The event was successfully queued
  FALSE: The event was not queued
=========================================================================*/
boolean event_report_ext(event_id_enum_type event_id, unsigned int version, void *parameter)
{
  event_store_type *event= NULL;
  #ifdef DIAG_CONSUMER_API
    event_store_type *event_ctrl = NULL;
  #endif
  byte stream_id = 0;
  uint32 *id = NULL;
  boolean ret_value = FALSE;

  if (version > DIAGBUF_MAX_VERSION_NUM)
  {
    return FALSE;
  }

  if ((version == DIAGBUF_VERSION_1) && parameter == NULL)
  {
    return FALSE;
  }


  /* This will check which masks are enabled */
  stream_id = event_read_stream_id(event_id);
   
#if defined FEATURE_DIAG_STM
  /* Is STM enabled, allocate entire event report and send event immediately. */
  if( (diag_stm_enabled) && (stream_id & DIAG_STREAM_STM))
  {
    event_generate_stm(event_id,0,NULL,version,parameter);
    /* Remove STREAM_STM from consideration now that STM has been sent */
    /* If a listener is active that STREAM flag will still be set    */
    stream_id &= ~DIAG_STREAM_STM;
  }
#endif /* FEATURE_DIAG_STM */

  /* Checks for event registration on stream_id and listeners done in
     event_q_alloc() */
  if (stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER))
  {
    event = (event_store_type *) event_q_alloc (event_id, 0,
            (stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER)));
  if (event)
  {
      event->stream_id = (stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER));
      switch (version)
      {
      case DIAGBUF_VERSION_0:
        event->version = DIAGBUF_VERSION_0;
        event->id = 0;
        break;

      case DIAGBUF_VERSION_1:
        id = (uint32 *)parameter;
        event->version = DIAGBUF_VERSION_1;
        event->id = *id;
        break;

      default:
        return FALSE;
      }
      event_q_put (event,(stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER)));
      ret_value = TRUE;
    }
    else
    {
      /*Error: could not allocate event - increase the drop count*/
    }
  }

  #ifdef DIAG_CONSUMER_API
    if(stream_id & DIAG_STREAM_2)
    {
      event_ctrl = (event_store_type *) event_q_alloc (event_id, 0,DIAG_STREAM_2);
      if (event_ctrl)
      {
        event_ctrl->stream_id = DIAG_STREAM_2;
        switch (version)
        {
          case 0:
            event_ctrl->version = 0;
            event_ctrl->id = 0;
            break;
          case 1:
            /*DCI does not support extended packets yet*/
            event_ctrl->version = 0;
            event_ctrl->id = 0;
            break;
          default:
            return FALSE;
        }
        event_q_put (event_ctrl,DIAG_STREAM_2);
        ret_value = TRUE;
      }
      else
      {
        /*Error: could not allocate event */
      }

    }
  #endif /* #ifdef DIAG_CONSUMER_API */
  return ret_value;
}

/*===========================================================================
FUNCTION EVENT_REPORT

DESCRIPTION
   Static events are reported by the mobile device to indicate actions, such
   as state changes and configuration, which are directly related to the
   operating standards of the system.The intent is to provide a reporting
   mechanism that minimizes resource usage (in particular, bandwidth) and
   maximizes storage and transmission priority for operating information that
   is considered most important in software test and verification.

PARAMETERS
   event_id   ID of the event to be reported.Event IDs are defined in event_defs.h

RETURN VALUE
   None
=========================================================================*/
void
event_report (event_id_enum_type event_id)
{
  event_report_ext(event_id, 0, NULL);
  return;
}                               /* event_report */

/*===========================================================================
FUNCTION EVENT_REPORT_PAYLOAD

DESCRIPTION
   Report an event with payload data .Payload structure format must be published

PARAMETERS
   event_id   ID of the event to be reported.Event IDs are defined in event_defs.h
   length     Length of the associated  payload
   *data      Pointer to the payload structure .Payload should be published.

RETURN VALUE
   None
=========================================================================*/
void
event_report_payload (event_id_enum_type event_id, uint8 length, void *payload)
{
  event_report_payload_ext(event_id, length, payload, 0, NULL);
  return;
}                               /* event_report_payload */

/*===========================================================================
FUNCTION EVENT_REPORT_PAYLOAD_EXT

DESCRIPTION
   Report an event with payload data .Payload structure format must be published

PARAMETERS
   event_id   ID of the event to be reported.Event IDs are defined in event_defs.h
   length     Length of the associated  payload
   *data      Pointer to the payload structure .Payload should be published.
   version    Specifies the version - currently, there are two versions
               0 - same as event_report_payload() (for backwards compatibility)
               1 - The outgoing event report header will have support to
                   include ID field to differentiate the events
                   coming from different instances of the processor.

   *parameter   Based on the version passed in, this pointer is interpreted
                as follows-
                version       pointer
                 0             NULL
                 1             uint32 * . The uint32 value this pointer points to,
                               is the ID of the instance generating this event.

RETURN VALUE
  TRUE:  The event was successfully queued
  FALSE: The event was not queued
=========================================================================*/
boolean
event_report_payload_ext(event_id_enum_type event_id, uint8 length, void *payload, unsigned int version, void *parameter)
{
  event_store_type *event = NULL;
  #ifdef DIAG_CONSUMER_API
    event_store_type *event_ctrl = NULL;
  #endif
  byte stream_id = 0;
  uint32 *id = NULL;
  boolean ret_value = FALSE;
  
  if (version > DIAGBUF_MAX_VERSION_NUM)
  {
    return FALSE;
  }

  if ((version == DIAGBUF_VERSION_1) && (parameter == NULL))
  {
    return FALSE;
  }
   
  if (length > 0 && payload)
  {
  /* Checks for event registration on stream_id and listeners done in
     event_q_alloc() */

    stream_id = event_read_stream_id(event_id);

#if defined FEATURE_DIAG_STM
    if( (diag_stm_enabled) && (stream_id & DIAG_STREAM_STM))
    {
      event_generate_stm(event_id,length,payload,version,parameter);
      /* Remove STREAM_STM from consideration now that STM has been sent */
      /* If a listener is active that STREAM flag will still be set    */
      stream_id &= ~DIAG_STREAM_STM;
    }
#endif /* FEATURE_DIAG_STM */

  /* Checks for event registration on stream_id and listeners done in
     event_q_alloc() */
    if ( (stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER)) )
      event = (event_store_type *) event_q_alloc (event_id, length ,
              (stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER)));

    if (event)
    {
      event->stream_id = (stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER));
      switch (version)
        {
        case DIAGBUF_VERSION_0:
          event->version = DIAGBUF_VERSION_0;
          event->id = 0;
          break;
        case DIAGBUF_VERSION_1:
          id = (uint32 *)parameter;
          event->version = DIAGBUF_VERSION_1;
          event->id = *id;
          break;
        default:
          return FALSE;
        }
      memscpy (event->payload.payload, event->payload.length, payload, length);
      event_q_put(event,(stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER)));
        ret_value = TRUE;
  }
  else
  {
        /*Error: could not allocate event*/
    }

    #ifdef DIAG_CONSUMER_API
      if ((stream_id & DIAG_STREAM_2))
      {
        event_ctrl = (event_store_type *) event_q_alloc (event_id, length ,DIAG_STREAM_2);
        if (event_ctrl)
        {
          event_ctrl->stream_id = DIAG_STREAM_2;
          switch (version)
          {
            case 0:
              event_ctrl->version = 0;
              event_ctrl->id = 0;
              break;
            case 1:
              /* DCI packets do not support extended events yet*/
              event_ctrl->version = 0;
              event_ctrl->id = 0;
              break;
            default:
              return FALSE;
          }
          memscpy (event_ctrl->payload.payload, event_ctrl->payload.length, payload, length);
          event_q_put(event_ctrl,DIAG_STREAM_2);
          ret_value = TRUE;
        }
      }
  #endif /* #ifdef DIAG_CONSUMER_API */
  }
  else
  {
    event_report_ext(event_id, version, parameter);
  }
  return ret_value;
  }
/*===========================================================================

FUNCTION EVENT_LISTENER_NOTIFY

DESCRIPTION
  This routine is called by the searchlist when a matching listener node is
  found.  It is called once per entry.  
  
  This routine is a wrapper to call the registered listeners.
  
===========================================================================*/
#ifdef DIAG_LISTENERS_SUPPORT
static void
event_listener_notify (uint32 seq_num, void *event, diag_searchlist_node_type *node)
{
  event_listener_entry_type *entry = NULL;
  diag_event_type *event_notify = (diag_event_type *) event;
  
  if (node && event)
  {
    entry = (event_listener_entry_type *) node->goo;

    if (entry->listener)
    {
      entry->listener (seq_num, event_notify, entry->param);
    }    
  }
}

/*===========================================================================

FUNCTION EVENT_SERVICE_LISTENER

DESCRIPTION
  This routine is to be called once per event before the event is formatted
  for the outbound comm packet.
  
ASSUMPTIONS
  'event_store_type' and 'diag_event_type' are assumed to be formatted 
  the same, with the exception of the q_link at the beginning of the store
  type.  Changing either types could result in undesired behavior.

===========================================================================*/
void
event_service_listener (event_store_type *event_store)
{
  if (event_store && (event_config & EVENT_CONFIG_LISTENER_MASK))
  {
    /* NOTE: diag_event_type and event_store_type purposely use the same
       format, except that event_store_type is preceeded by a Q link
       and the event ID field has a union for internal packet formatting.
       If either types are changed, the service will not function properly.
       &event_store->event_id is cast to diag_event_type * in 
       event_listener_notify(). */
    (void) diag_searchlist_search_all (event_store->event_id.id, 
                                &event_listeners, 
                                (void *) &event_store->event_id, /* cast!!! */
                                event_listener_notify);

  }
}

/*===========================================================================

FUNCTION EVENT_LISTENER_MATCH_AND_FREE

DESCRIPTION
  This routine is passed to and called by diag_searchlist_delete() for each 
  node of the search until this routine returns TRUE or the search is 
  exhausted.  This routine is responsible for freeing the 'entry' which 
  was allocated in diag_add_event_listener().

===========================================================================*/
static boolean
event_listener_match_and_free (diag_searchlist_node_type *match_node, 
                               diag_searchlist_node_type *list_node)
{
  boolean found = FALSE;
  event_listener_entry_type *match = NULL;
  event_listener_entry_type *entry = NULL;

  if (match_node && list_node)
  {
    match = (event_listener_entry_type *) match_node->goo;
    entry = (event_listener_entry_type *) list_node->goo;

    if (match && entry)
    {
      if (match->listener == entry->listener &&
          match->param == entry->param)
      {
        found = TRUE;
        diag_listeners_free (entry);

#ifdef FEATURE_DEBUG_DIAG_HEAP
        event_listener_match_and_free_cnt++;
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "FREE event_listener_match_and_free = %d ",
                event_listener_match_and_free_cnt);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
      }
    }
  }

  return found;
}

/*=========================================================================*/
boolean
diag_add_event_listener (const unsigned int event_id,
             const diag_event_listener listener, void *param)
{
  event_listener_entry_type *entry = NULL;
  boolean success = FALSE;
  
  entry = (event_listener_entry_type *) 
    diag_listeners_malloc (sizeof (event_listener_entry_type));
  
  
  if (entry)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_add_event_listener_malloc_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "MALLOC diag_add_event_listener_malloc = %d Bytes allocated %d",
        diag_add_event_listener_malloc_cnt, sizeof (event_listener_entry_type));
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    entry->listener = listener;
    entry->param = param;

    success = diag_searchlist_add (&event_listeners, event_id, entry);

    if (success)
    {
      /* Assume the critical section is initialized (success, right?). */
      osal_enter_crit_sect (&event_listeners.crit_sect);

      DIAG_SET_MASK (diag_cx_state, DIAG_CX_EVENT_LISTEN_S);

      event_control (
        (event_listeners.count > 0) ? EVENT_CONFIG_LISTENER_MASK : 0, 
        EVENT_CONFIG_LISTENER_MASK);

      osal_exit_crit_sect (&event_listeners.crit_sect);
    }
    else
    {
      /* unable to add, free 'entry' */
      diag_listeners_free ((void *) entry);
#ifdef FEATURE_DEBUG_DIAG_HEAP
      diag_add_event_listener_free_cnt++;
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "Unable to add event listener freeing it,  \
			diag_add_event_listener_free_cnt = %d ",
            diag_add_event_listener_free_cnt);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    }
  }

  return success;
}

/*===========================================================================

FUNCTION EVENT_REMOVE_EVENT_LISTENER

DESCRIPTION
  This is the packet handler for the event service configuration packet.

===========================================================================*/
boolean
diag_remove_event_listener (const unsigned int event_id,
                const diag_event_listener listener, void *param)
{
  event_listener_entry_type entry;
  diag_searchlist_node_type node;
  boolean success = FALSE;
  
  entry.listener = listener;
  entry.param = param;
  
  node.key = event_id;
  node.goo = (void *) &entry;


  success = diag_searchlist_delete (&event_listeners, node,
                                    event_listener_match_and_free);

  if (success)
  {
    /* Assume the critical section is initialized (success, right?). */
    osal_enter_crit_sect (&event_listeners.crit_sect);

    event_control (
      (event_listeners.count > 0) ? EVENT_CONFIG_LISTENER_MASK : 0, 
      EVENT_CONFIG_LISTENER_MASK);

    if (event_listeners.count == 0)
    {
      DIAG_CLR_MASK (diag_cx_state, DIAG_CX_EVENT_LISTEN_S);
    }

    osal_exit_crit_sect (&event_listeners.crit_sect);
  }

  return success;
}
#endif /* DIAG_LISTENERS_SUPPORT */
