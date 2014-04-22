#ifndef DIAG_V_H
#define DIAG_V_H
/*==========================================================================

                     Diagnostic Task Header File

Description
  Global Data declarations of the diag_task.

Copyright (c) 1991-2014,2016,2019 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                         Edit History

      $Header: //components/rel/core.tx/6.0/services/diag/LSM/common/src/diag_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/22/16   nk      Change to take priority from rcinit api
08/09/16   ph      Adjust the priorities of DIAG tasks.
07/20/16   nk      Changes to support ThreadX
11/25/14   ph      Mainlined the feature DIAG_CONSUMER_API.
07/16/14   xy      Update Diag delayed response command to take parameter
03/10/14   sa      Removed declaration of diag_set_drain_timer
                   as it is declared in diagi_v.h
02/21/14   is      Command/response separation
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
10/14/13   sr      Added support for extended event, log and F3 pkts
10/01/13   xy      Removed code under FEATURE_WINCE
08/23/13   sr      Added support for diag mask change notification for
                   registered users
01/25/13   sr      Fixed compiler warnings
11/27/12   sa      Replaced DIAG_MP_SLAVE_LAST with !DIAG_MP_MASTER
11/27/12   sa      Removed DIAG_MP_SLAVE_INT
11/27/12   sa      Mainlined DIAG_CENTRAL_ROUTING
09/26/12   is      Support for preset masks
08/31/12   ra      Use accessor macros instead of accessing private task
                   info directly; accomodated changes for User space
                   task priority changes
05/15/12   is      Allow "build time" mask queries from Master Diag
05/03/12   rs      Moved delayed response from diag buffer to AMSS heap.
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
04/18/11   is      Update feature flag for SMD-Lite
03/04/11   is      Support for Diag over SMD-Lite
02/04/11   vs      Removed check for MASTER to not have RSP_SEND
11/26/10   vs      if a9 on 9k - have proc ID be modem_proc
11/22/10   hm      diag_v.h clean up. Moved the defines to respective custdiag.h files
10/25/10   is      Diag feature flag cleanup
07/23/10   mad     Added prototype for diagpkt_check_validity_request_ondevice()
07/12/10   mad     Changes for apps-only diag bring-up
06/30/10   sg      diag_client_maskchange() is made static
04/20/10   sg      Moved diag_client_maskchange() here form diag_shared_i.h
06/07/10   is      Replace external IMAGE_* feature flags with internal Diag
                   versions
04/21/10   JV      Added prototypes for functions that manage the new heap for
                   events.
12/14/09   sg      Added diag_rx_rsp_notify()
07/14/09   vs      Added diag_set_drain_timer
08/05/09   JV      Removed the CUST_H featurization around the inclusion of
                   customer.h.
07/31/09   JV      Added new internal feature to single out the Q6 on 8x50
                   DIAG_QDSP6_APPS_PROC
07/31/09   JV      Merged Q6 diag code back to mainline
07/28/09   as      Removed public API's from internal file.
07/17/09   mad     Featurized includion of customer.h for WM.
07/10/09   as      Decoupled feature FEATURE_DIAG_NON_STREAMING
05/07/09   vk      moved declerations into diag.h that were being referred from there
01/15/09   as      Fixed corner case in proc ID
12/12/08   vg      split public and private information diag.h and diag_v.h
10/03/08   vg      Update configuration to enable msm 8200, 2 modem processors
12/22/06   as      Moved proc ID macros to diag.h
12/05/06   as      Added signal for Diag drain timer
11/21/06   as      Moved DIAG internal features from diagi.h to diag.h
03/30/05   sl      Added support for SPC_UNLOCK_TTL to unlock/lock the sp_state
10/17/03   ph      For Rel A builds, redirect to new MC subsystem command.
09/23/03    gr     Function prototypes related to changes that enable more
                   efficient detection of the condition where diag is out of
                   heap space.
08/20/01   jal     Changes to support more Diag packets.  Support for powerup
                   online/offline state, service programming lock state.
04/06/01   lad     Cosmetic changes.
02/23/01   lad     Rearchitected the diagnostics subsystem to be a service
                   rather than a module.  All coupling to targets has been
                   moved to target-specific implementations.  This file now
                   contains an API.  No other information or coupling remains
                   except for featurized legacy code that impacts existing
                   targets.
                   Old history comments have been removed since most of them
                   are no longer applicable.

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "rcinit.h"
#include "rcinit_qurt.h"
#include "qw.h"
#include "diag.h"
#include "osal.h"


/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
/* Diagnostics version (protocol revision).
*/
/* DIAG_DIAGVER
   Diagnostics version, used to ensure compatibility of the DM and the DMSS.
   1   Original
   2   Changes to status packet and verno packet. Removed verstr packet
   3   for release to factory. Sends RF mode in status packet
   4   Adds release directory to verno packet and dipswitch packets
   5   Many changes in DM.
   6   Added Vocoder PCM and PKT loopback, and Analog IS-55 tests
   7   New FER data in tagraph packet
   8   Streaming protocol enhancements and event reporting.
*/

#define DIAG_DIAGVER 8

/* -------------------------------------------------------------------------
** Diag Internal features
** ------------------------------------------------------------------------- */

/* Proc ID definitions */
#if defined(DIAG_IMAGE_APPS_PROC)
 #define DIAG_MY_PROC_ID DIAG_APP_PROC
 #define DIAGDIAG_STRESS_TEST_DELAYED_RSP DIAGDIAG_STRESS_TEST_DELAYED_RSP_APP
 #if defined(DIAG_IMAGE_QDSP6_PROC)          // For audio q6
  #define DIAG_MY_PROC_ID DIAG_QDSP6_PROC
  #define DIAGDIAG_STRESS_TEST_DELAYED_RSP DIAGDIAG_STRESS_TEST_DELAYED_RSP_Q6
 #endif
#elif defined(DIAG_IMAGE_MODEM_PROC)
 #define DIAG_MY_PROC_ID DIAG_MODEM_PROC
 #define DIAGDIAG_STRESS_TEST_DELAYED_RSP DIAGDIAG_STRESS_TEST_DELAYED_RSP_MODEM
#elif defined(DIAG_IMAGE_RIVA_PROC)
  #define DIAG_MY_PROC_ID DIAG_RIVA_PROC
  #define DIAGDIAG_STRESS_TEST_DELAYED_RSP DIAGDIAG_STRESS_TEST_DELAYED_RSP_RIVA
#else
// Not defined
#endif

/* Runtime Device Map port selection for diag */
#if defined (FEATURE_RUNTIME_DEVMAP) && !defined(DIAG_APPS_ONLY_BRINGUP)
#if defined (DIAG_MP)
  #if defined (DIAG_MP_MASTER)|| defined(FEATURE_DIAG_MODEM_ONLY)

    #define DIAG_RUNTIME_DEVMAP

  #endif
#else
   #define DIAG_RUNTIME_DEVMAP
#endif
#endif

#if defined(FEATURE_DATA_SERIALIZER) && defined(DIAG_RUNTIME_DEVMAP)
  #define FEATURE_DIAG_ON_DATA_PORT
#endif /* FEATURE_DATA_SERIALIZER */

/* Error cases */

#if defined (DIAG_MP) && !defined(FEATURE_SMD) && !defined(FEATURE_SMD_LITE)
  #error "Error: FEATURE_SMD or FEATURE_SMD_LITE is required for DIAG_MP"
#endif

#if defined (FEATURE_DIAG_MODEM_ONLY) &&  \
    !(defined (DIAG_MP) || defined (DIAG_IMAGE_MODEM_PROC))
  #error "Error: Invalid configuration for DIAG MP"
#endif


#if defined(DIAG_MP) && !defined (DIAG_MP_MASTER)
 #if defined (DIAG_FRAMING)
   #error "Error: DIAG_FRAMING defined on Slave"
 #endif
#endif


#define DIAG_FTM_SWITCH_VAL 2

extern osal_tcb_t diag_task_tcb;

/* Determine how to retrieve diag priority */

  #if !defined (GET_DIAG_PRI)
    #define GET_DIAG_PRI()   rcinit_lookup_prio("diag")
  #endif


/* Determine how to retrieve sleep priority */
#if defined (DIAG_QDSP6_APPS_PROC)
    #define GET_SLEEP_PRI()  255
#elif !defined (FEATURE_SLEEP)
    #define GET_SLEEP_PRI()  255
#else
  #if !defined (GET_SLEEP_PRI)
    #define GET_SLEEP_PRI()  rcinit_lookup_prio("sleep")
  #endif
#endif

/* -------------------------------------------------------------------------
** Function Definitions
** ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif



/*These define the bit postion in diag_mask_notify
  for mask change notification*/
#define DIAG_EVENT_MASK_NOTIFY_BIT 1
#define DIAG_LOG_MASK_NOTIFY_BIT   2
#define DIAG_MSG_MASK_NOTIFY_BIT   4

/*Defines the maximum version number we support currently for diagbuf
  pkts*/
typedef enum
{
    DIAGBUF_VERSION_0  = 0,
    DIAGBUF_VERSION_1 = 1,
    DIAGBUF_MAX_VERSION_NUM = DIAGBUF_VERSION_1
}diagbuf_pkt_version;

/* ==================================================================
FUNCTION DIAG_MALLOC

DESCRIPTION
   Dynamically allocates a specified number of bytes and returns a
   pointer to this chunk of memory.
===================================================================== */
  void *diag_malloc (unsigned long num_bytes);


/* ==================================================================
FUNCTION DIAG_EVENT_MALLOC

DESCRIPTION
   Dynamically allocates a specified number of bytes and returns a
   pointer to this chunk of memory.
===================================================================== */
  void *diag_event_malloc (unsigned long num_bytes);

  int diag_event_alloc_check (unsigned int num_allocs, uint32 *alloc_sizes);

/* ==================================================================
FUNCTION DIAG_EVENT_FREE

DESCRIPTION
   Frees a chunk of memory allocated using rex_malloc.
===================================================================== */
  void diag_event_free (void *mem_ptr);
/* ==================================================================
FUNCTION DIAG_EVENT_MALLOC_SEC

DESCRIPTION
   Dynamically allocates a specified number of bytes from diag secondary
   event heap and returns a   pointer to this chunk of memory.
===================================================================== */
#ifdef DIAG_CONSUMER_API
  void *diag_event_malloc_sec (unsigned long num_bytes);

  int diag_event_alloc_check_sec (unsigned int num_allocs, uint32 *alloc_sizes);

/* ==================================================================
FUNCTION DIAG_EVENT_FREE

DESCRIPTION
   Frees a chunk of memory allocated using rex_malloc from diag_event_sec_heap.
===================================================================== */
  void diag_event_free_sec (void *mem_ptr);
#endif /* #ifdef DIAG_CONSUMER_API */




/*===========================================================================
FUNCTION DIAG_SUSPEND_TASK

DESCRIPTION
  This procedure suspends the DIAG task for 'duration' milliseconds.  This
  routine has no impact on the flow of diagnostic data other than
  suspending the DIAG task.

===========================================================================*/
  void diag_suspend (unsigned int duration);

/* ==================================================================
FUNCTION DIAG_BLOCK

DESCRIPTION
   Blocks the DIAG task until diag_release() is called.  The DIAG
   task will be suspended completely until diag_release() is called.
===================================================================== */
  void diag_block (void);

    /* ==================================================================
FUNCTION DIAG_LISTENERS_FREE

DESCRIPTION
   This function frees the listener
===================================================================== */
#ifdef DIAG_LISTENERS_SUPPORT
  void diag_listeners_free(void *mem_ptr);
#endif

/* ==================================================================
FUNCTION DIAG_LISTENERS_MALLOC

DESCRIPTION
   This function allocates a listener
===================================================================== */
#ifdef DIAG_LISTENERS_SUPPORT
  void * diag_listeners_malloc (unsigned long num_bytes);
#endif

/* ==================================================================
FUNCTION DIAG_RSP_FREE
===================================================================== */
  void diag_rsp_free(void *mem_ptr);

  /*===========================================================================

FUNCTION DIAG_MASKS_ENABLED

DESCRIPTION
  This function is used to determine if Diag has been activated to process
  logs, events, and/or F3 messages.

  It calculates its return value by checking two places:
  1) All mask options which vote against sleep
  2) All connection state variables which get activated because of listeners

RETURN VALUE
  Boolean indicating the whether Diag has any logging services enabled.
  FALSE indicates that none are enabled.
  TRUE indicates that at least one feature is enabled.

===========================================================================*/
boolean diag_masks_enabled(void);

/* ==================================================================
FUNCTION DIAG_RELEASE

DESCRIPTION
   Blocks the DIAG task until diag_release() is called.  The DIAG
   task will be suspended completely until diag_release() is called.
===================================================================== */
  void diag_release (void);

/*===========================================================================
FUNCTION DIAG_TX_NOTIFY

DESCRIPTION
  This routine notifies DIAG that data is ready to be sent without causing
  a context switch immediately.

  This eliminates concern that INTLOCK's may be violated from a context
  switch.

RETURN VALUE
  None.

===========================================================================*/
  void diag_tx_notify (void);

/*===========================================================================
FUNCTION DIAG_RX_NOTIFY

DESCRIPTION
  This callback routine is called when inbound command data has been received
  and is ready be read by the diag task.

===========================================================================*/
  void diag_rx_notify (void);

/*===========================================================================
FUNCTION DIAG_RX_RSP_NOTIFY

DESCRIPTION
  This function notifies DIAG that RX response data is ready to be sent .

===========================================================================*/
  void diag_rx_rsp_notify(void);

/*===========================================================================

FUNCTION DIAG_RX_DELAY_RSP_NOTIFY

DESCRIPTION
  This routine sets the DIAG_RX_DELAY_RSP_SIG

===========================================================================*/
void diag_rx_delay_rsp_notify( void );

#ifdef DIAG_CONSUMER_API
/*===========================================================================
FUNCTION DIAG_RX_DCI_NOTIFY

DESCRIPTION
  This function notifies DIAG that DCI RX data is ready to be received.

===========================================================================*/
void diag_rx_dci_notify(void);
#endif /* #ifdef DIAG_CONSUMER_API */




/* Data associated with synch. event for mask-change. */
#define DIAG_EVENTSVC_MASK_CHANGE   0
#define DIAG_LOGSVC_MASK_CHANGE     1
#define DIAG_MSGSVC_MASK_CHANGE     2

/*===========================================================================
FUNCTION DIAG_CLIENT_MASKCHANGE

DESCRIPTION
  Notifies all diag clients (Diag_LSMs) of mask change.
============================================================================*/
boolean diag_client_maskchange(int mask_type);

//ToDo: Move to diagpkti_v.h
boolean diagpkt_check_validity_request_ondevice (void * buf);

#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
void diagpkt_fwd_cmd_reg( void );
#endif

#if defined(DIAG_MP_MASTER)
boolean diagpkt_send_comm_rsp( void );
#endif

#ifdef __cplusplus
}
#endif
#endif /*DIAG_V_H*/
