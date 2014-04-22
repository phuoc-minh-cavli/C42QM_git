#ifndef DIAGI_V_H
#define DIAGI_V_H
/*==========================================================================

                 Diagnostic Subsystem Internal Header File

Description
  Shared declarations and prototypes internal to the Diagnostic subsystem.

Copyright (c) 1990-2017, 2019, 2022 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                            Edit History

$Header: //components/rel/core.tx/6.0/services/diag/LSM/common/src/diagi_v.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/19   kdey    Porting of Qsockets
02/24/17   gn      Changes to support DIAG QAPIs with DAM framework
08/05/16   nk      Fixed Signal macro values to not use upper 8 bits of signal mask
07/20/16   nk      Changes to support ThreadX
01/13/16   is      Propagate STM info in Diag context
01/16/15   sa      Updated the name for the commands injected in Diag Cmd Queue.
12/17/14   ph      Removed diag_strlcpy() function.
10/08/14   xy      Implement DCI discovery and DIAG DCI over PCIe 
09/01/14   ph      Added mutex around diag_internal_sigs to resolve concurrency
                   issues with feature mask update missing.
04/24/14   rh      Added internal flush signal.
02/26/14   sa      Added support for compression in buffering mode.
02/21/14   is      Command/response separation
01/20/14   rh      Added support for centralized build masks 
01/07/14   xy      Added fusion DCI feature  
11/01/13   xy      Added struct diag_dci_enc_pkt  
08/23/13   sr      Added support for diag mask change notification for 
                   registered users    
05/11/13   is      Move drain timer from client conext to diag context
01/16/13   rh      Turned DIAG_TX_WMQ_SIG into DIAG_SMDL_WRITE_SIG
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
11/27/12   rh      Mainlined DIAG_CENTRAL_ROUTING
11/05/12   is      Support for preset masks
10/17/12   rh      Added support for t-put improvements with SMDL
10/12/12   rh      Added definition for diag_drain_timer_enabled() 
08/24/12   ra      Support for querying Command Registration Tables
05/03/12   rs      Moved delayed response from diag buffer to AMSS heap. 
01/24/12   is      Support for buffered circular mode
11/04/11   is      Support for buffered threshold mode
10/07/11   is      Support sending log mask per equipment id
09/28/11   is      Support dynamic sleep voting
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/15/11   is      Modifed "set mask" via CTRL msgs fot EA support
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
08/08/11   wjg     Added signal to bring smdl_read into DIAG context 
04/28/11   is      Update featurizations for central routing
04/27/11   is      Resolve compiler warnings
04/18/11   is      Ensure CTRL msg sizes are constant
03/04/11   is      Move diagpkt_fwd_cmd_reg() prototype here
03/01/11   is      Add function prototype for diag_strlcpy()
12/14/10   mad     Added prototype for diagpkt_ctrl_get_msg_type() 
09/28/10   mad     Added diag_control_sleep_voting and diag_ctrl_drain_timer_len
09/14/10   is      New control channel and central routing features
08/10/10   sg      Added a new signal DIAG_COMM_SIO_TX_FLUSH_SIG that is set when 
                   the smd port is closed and callback  is triggered
06/20/10   sg      Added boolean to check whether diag task is initialized 
06/07/10   is      Removed unused DIAG_TEST_CMD_SIG
02/11/10   JV      Eliminating the event_pending timer and event_pending_q
01/20/10   JV      Added a new signal that when set, drains events
12/10/09   sg      Introduced DIAG_RX_RSP_SIG to handle rx traffic responses
09/16/09   sg      Removed the code under FEATURE_DIAG_SPC_TIMEOUT as it is no longer used.
07/10/09   as      Decoupled code under FEATURE_DIAG_COUPLED_ARCHITECTURE
05/12/09   JV      Introduced the OS abstraction layer for rex.
05/07/09   vk      moved declerations into diagi.h that were being referred from 
                   there
12/12/08   vg      CMI creating diagi.h diagi_v.h
10/03/08   vg      Updated code to use PACK() vs. PACKED
11/21/06   as      Moved DIAG internal features from diagi.h to diag.h
08/28/06   as      Added win mobile featurization support.
03/28/06   pc      Changed DIAG_TX_APP_SIG from 0x00800000 to 0x00080000.
10/19/05   as      Modified diag_searchlist_type to make it 4-byte aligned.
07/05/05   sl      Added support for SPC_TIMEOUT to double the timeout value
                   on consecutive incorrect SPCs.
06/16/05   as      New signal for DM to communicate completion.
05/17/05   as      Added new signal for Application processor data.
10/24/01   jal     New signal for SIO to indicate the UART is flushed.
                   New signal for SIO to indicate port closure is complete.
08/20/01   jal     Supported more Diag packets.  Added NV cache typedef,
                   prototypes for diag_set_sp_state(), downloader status.
06/27/01   lad     Various non-functional changes to facilitate update of
                   log services.
04/06/01   lad     Added definitions of DIAG_TASK_xxx sigs to decouple from 
                   task.h.
                   ASYNC con<F2st definitions moved to diagcomm.h?
                   Externalized msg_nice[] and log_nice[] for streaming config 
                   support.
                   Moved prototype of diag_kick_watchdog() to diagtarget.h.
                   Added prototype of diagpkt_process_request() (moved from 
                   diagpkt.h).
                   Updated prototype for diag_do_escaping().
                   Removed prototype for diag_do_unescaping().
                   Removed references to diagpkt_refresh_cache().
02/23/01   lad     Rearchitected the diagnostics subsystem to be a service 
                   rather than a module.  All coupling to targets has been
                   moved to target-specific implementations.  The only coupling
                   that remains are common services and REX.
                   Old history comments have been removed since most of them
                   are no longer applicable.

===========================================================================*/
#include "comdef.h"
#include "diag_v.h"
#include "diagbuf_v.h"
#include "log.h"
#include "diagcomm_smd.h"
#include "diagcomm_ctrl.h" /* For diag_ctrl_msg_type */

#if defined(FEATURE_DIAG_NV)
#include "nv.h"
#endif
#include "osal.h"

#include <string.h>
#include "diagi.h"

#define DIAG_NON_HDLC_ENCODED 1  /* DSM packet is not HDLC encoded. Type 'diag_non_hdlc_type' */
#define DIAG_HDLC_ENCODED 0      /* DSM packet is HDLC encoded */

/* -------------------------------------------------------------------------
** Diag Task Signal Definitions
** ------------------------------------------------------------------------- */

#define DIAG_EVENT_TIMER_SEC_SIG          0x00000000   /* Signal to finish terminate
												          the current event reporting
												          buffer of secondary stream. */

#define DIAG_EVENT_DRAIN_SEC_SIG          0x00000000   /* Signal used to drain events
													      of secondary stream*/

#define DIAG_TASK_STOP_SIG                0x00000000
									   

#define DIAG_RPT_TIMER_SIG                0x00000001    /* This signal is set when the 
												           diag_rpt_timer expires */
												   
#define DIAG_RX_SIG                       0x00000002    /* This signal is set by the 
												           Serial I/O Services when an 
												           HDLC packet is received. */

#define DIAG_COMM_SIO_TX_FLUSH_SIG        0x00000008    /* Signal used to turn off the masks
													       and empty the wmq*/
												   
																								   
#define DIAG_EVENT_DRAIN_SIG              0x00000010    /* Signal used to drain events */

#define DIAG_FLASH_LOGGING_SIG            0x00000020    /* Signal used to enable diag logging over flash */

#define DIAG_RX_DELAY_RSP_SIG             0x00000040    /* Signal is set when delayed 
                                                           responses are generated for 
                                                           RX traffic */
                                                   

#define DIAG_TX_SIG                       0x00000080    /* This signal is set by the 
                                                           Serial I/O Services when a
                                                           packet has been sent. */

#define DIAG_TOUT_TIMER_SIG               0x00000100    /* This signal is a timer
                                                           signal for timeout if bad 
                                                           SPC is rec'd */

#define DIAG_RX_RSP_SIG                   0x00000200    /* Signal is set when responses 
                                                           are generated for RX traffic */

#define DIAG_REFRESH_CACHE_SIG            0x00000400    /* Signal to tell DIAG to refresh
                                                           any cache it may have. */

#define DIAG_COMM_OPEN_SIG                0x00000800    /* Signal that SIO is availabe
                                                           to be opened for use. */
														   /* 0x2000 - 0x8000 are reserved task signals.  
														   ** In MSM builds it is defined in task.h
														   */

#define DIAG_COMM_CLOSE_SIG               0x00001000    /* Signal that SIO is no
                                                           longer available for use. */
                                                           
#define DIAG_RX_CTRL_DCI_SIG              0x00002000    /* This signal is set by the 
														   Serial I/O Services when a
														   packet is received from DCI stream.*/
#ifdef TASK_START_SIG
  #define DIAG_TASK_START_SIG TASK_START_SIG
#else
  #define DIAG_TASK_START_SIG 0x8000
#endif

														   
#define DIAG_EVENT_TIMER_SIG              0x00010000    /* Signal to finish terminate
														   the current event reporting
														   buffer. */
																										  
													  
#define DIAG_INTERNAL_SIG                 0x00020000    /* Signal that an internal Diag signal
														   needs to be handled */
													  
#define DIAG_BLOCK_SIG                    0x00040000    /* Generic block mechanism for
														   clients to suspend DIAG task */
													  
#define DIAG_CTRL_MSG_SIG                 0x00080000    /* Signal to process inbound Diag
														   control traffic. 0x00080000 is also 
														   defined as DIAG_FWD_TX_SLAVE_SIG in diag_fwd_v.h 
														   for use in diag_fwd_task_tcb. */
													  
#define DIAG_SIO_TOUT_TIMER_SIG           0x00100000    /* This signal is a timer
														   signal for timeout if sio 
														   callback is not called */
														   
#define DIAG_RX_DCI_SIG                   0x00200000   /* This signal is set by the 
														  Serial I/O Services when a
														  packet is received from DCI stream.*/


#define DIAG_SIO_SWITCH_UART_SIG          DIAG_COMM_CLOSE_SIG
                                                      /* Signal indicating that the 
                                                         diag should go back to the
                                                         aux uart */


/*#ifdef TASK_STOP_SIG
#define DIAG_TASK_STOP_SIG TASK_STOP_SIG
#else
#define DIAG_TASK_STOP_SIG 0x4000
#endif

#ifdef TASK_OFFLINE_SIG
#define DIAG_TASK_OFFLINE_SIG TASK_OFFLINE_SIG
#else
#define DIAG_TASK_OFFLINE_SIG 0x2000
#endif */


/* -------------------------------------------------------------------------
** Diag Internal Signals
** ------------------------------------------------------------------------- */
#define DIAG_INT_CTRL_REG_SIG         0x00000001   /* Signal to forward Diag 
                                                      registration msgs to Master
                                                      via CTRL channel */

#define DIAG_INT_MASK_UPDATE_SIG      0x00000002   /* Signal for Master to send
                                                      updated F3/Event/Log masks 
                                                      to Slaves */

#define DIAG_INT_FEAT_UPDATE_SIG      0x00000004   /* Signal to send internal
                                                      Diag feature flag mask to
                                                      other procs */
													  
#define DIAG_INT_REG_TBL_RSP_SIG      0x00000008   /* Signal to send the
  						      Command Registration table
                                                      as a delayed response */                                                   
#define DIAG_INT_SET_DRAIN_TIMER_SIG  0x00000200   /* Signal to set Diag drain 
                                                      timer in Diag's context. */

#define DIAG_INT_LOG_MASK_SSM_UPDATE_SIG 0x00000010    /* Signal to update internal
                                                          Diag log mask ssm
                                                          table */

#define DIAG_INT_EVENT_MASK_SSM_UPDATE_SIG 0x00000020  /* Signal to update internal
                                                          Diag event mask ssm
                                                          table */
 
#define DIAG_INT_OPEN_DCI_CHANNEL_SIG 0x00000040       /* Signal to notify Diag to open 
                                                          DCI channel*/

#define DIAG_INT_CLOSE_DCI_CHANNEL_SIG 0x00000080      /* Signal to notify Diag to close
                                                          the DCI channel*/

#define DIAG_INT_MASK_CHANGE_NOTIFY_SIG  0x00000400    /*Signal to notify clients about 
                                                       diag mask change */

#define DIAG_INT_DRAIN_DSM_BUFFER_SIG 0x00001000       /* Signal to notify Diag to drain
														  buffered DSMs */

#define DIAG_INT_DRAIN_DCI_DSM_BUFFER_SIG 0x00002000   /* Signal to notify Diag to drain
														  buffered DCI DSMs */

#define DIAG_INT_CONN_STATUS_SEND_SIG    0x00400000    /* Signal to send the
                                                          dci connection status 
                                                          of peripherals*/

#define DIAG_INT_FLUSH_BUF_SIG       0x00008000       /* Signal for diagbuf flush */

#define DIAG_INT_FUSION_OPEN_SIG         0x00010000    /* Signal to open fusion dci channel */

#define DIAG_INT_FUSION_CLOSE_SIG       0x00020000    /* Signal to close fusion dci channel*/

#define DIAG_INT_STM_UPDATE_SIG         0x00100000      /* Signal to update peripherals of STM status */


#define DIAG_INT_RESEND_CTRL_SIG           0x01000000    /*Signal to resend ctrl command from SMDL*/

#define DIAG_INT_RESEND_CMD_SIG            0x02000000    /*Signal to resend command from SMDL*/

#define DIAG_INT_RESEND_DATA_SIG           0x04000000    /*Signal to resend data from SMDL*/

/* -------------------------------------------------------------------------
** Update Mask Bit Flags
** ------------------------------------------------------------------------- */
#define DIAG_UPDATE_MASK_NONE            0x00000000
#define DIAG_UPDATE_MASK_F3              0x00000001
#define DIAG_UPDATE_MASK_EVENTS          0x00000002
#define DIAG_UPDATE_MASK_LOGS            0x00000004
#define DIAG_UPDATE_MASK_PARTIAL_F3      0x00000008
#define DIAG_UPDATE_MASK_PARTIAL_LOGS    0x00000010
#define DIAG_UPDATE_MASK_ALL         (DIAG_UPDATE_MASK_F3     | \
                                      DIAG_UPDATE_MASK_EVENTS | \
                                      DIAG_UPDATE_MASK_LOGS)

/* Error codes for diag_set_current_preset_mask_id() */
#define DIAG_SET_PRESET_ID_SUCCESS       0
#define DIAG_SET_PRESET_ID_NOT_SUPPORTED 1
#define DIAG_CONSUMER_API_VER_1 0x01

#define DIAG_SEND_BUF_SIZE 0x8000 /* Buffer size of flat buffer used to send data to SMDL/Sockets */
#define DIAG_MAX_SEND_DATA_SIZE 1024*8
#define DIAG_WORD_ALIGN(x) ((x+3)&(~3))

#define DIAG_READ_BUF_SIZE (8*1024)

#define DIAG_SEND_BUF_PAD_LEN 0x500 //Pad length used to determine if a new block should be added or not in send buffer
#define DIAG_SEND_BUF_PAD_LEN_NRT 0x1000 //Pad length used to determine if a new block should be added or not in NRT mode

/* -------------------------------------------------------------------------
** Externalized data members
** ------------------------------------------------------------------------- */
/* Timers.
*/
extern osal_timer_t diag_rpt_timer;   /* Watchdog report timer */

/* Masks to indicate the operating state. */
boolean diag_set_security_state (diag_security_type security);
boolean diag_set_sp_state (diag_sp_type sp);


/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
#if defined(DIAG_BUFFERING_SUPPORT)
typedef struct
{
  uint8 mode;
  boolean drain;
  
  uint8 buffered_upper_threshold;  /* Upper Threshold Percentage Level */
  uint8 buffered_lower_threshold;  /* Lower Threshold Percentage Level */
  uint32 buffered_many_mark;  /* Upper Threshold as # of bytes */
  uint32 buffered_few_mark;  /* Lower Threshold as # of bytes */
  
  uint32 byte_dropped_cnt;
  uint32 byte_alloc_cnt;
  
  //Stores previous counts before a mode switch/reset
  uint8 prev_mode;
  uint32 prev_byte_dropped_cnt;
  uint32 prev_byte_alloc_cnt;
  
  
} diag_tx_mode_type;
#endif

typedef struct
{
  /* (Input) raw (HDLC encoded) stream buffer.  This is never changed
     by the packet extraction/decoding functions. */
  byte *src_ptr;

  /* (I/O) The current byte array index of src_ptr.
     When src_idx == src_size, the src_ptr buffer is complete. */
  unsigned int src_idx;

  /* (Input) This is the number of bytes to convert from src_ptr. */
  unsigned int src_size;

  /* NOTE: Since a packet can cpan mulitple source blocks, dest_idx
     must be stored to maintain the state of the packet extraction.
     For src_ptr, this is not necessary. */

  /* (Input) The extracted packet.  This is never changed
     by the packet extraction/decoding functions. */
  byte *dest_ptr;

  /* (I/O) The array index into dest_ptr of the next unwritten byte.
     Upon successful decode of a packet, this field is the length. */
  unsigned int dest_idx;

  /* This is the number of bytes that can be stored in dest_ptr. */
  unsigned int dest_size;

  /* Used internally: Are we in the middle of an escape code?
     This boolean is needed in order to be able to handle the
     case where an escape code straddles two packets. */
  boolean escaping;

}
diag_hdlc_decode_type;


/*===========================================================================

FUNCTION DIAG_SLEEP_INIT

DESCRIPTION
  This procedure initializes variables for Diag sleep voting.

===========================================================================*/
void diag_sleep_init( void );

/*===========================================================================

FUNCTION DIAG_ALLOW_SLEEP

DESCRIPTION
  Tell the sleep task that we are in a state where we can sleep.

SIDE EFFECTS
  The sleep task may take us up on our offer.

===========================================================================*/
void diag_allow_sleep (void);

/*===========================================================================

FUNCTION DIAG_FORBID_SLEEP

DESCRIPTION
  Tell the sleep task that we are NOT in a state where we can sleep.

===========================================================================*/
void diag_forbid_sleep (void);

/* -------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------- */

/* Connection state */
#define DIAG_CX_NONE_S (0x00)
#define DIAG_CX_COMM_S (0x01)
#define DIAG_CX_CMD_S  (0x02)
#define DIAG_CX_LOG_LISTEN_S (0x04)
#define DIAG_CX_EVENT_LISTEN_S (0x08)

#define DIAG_CX_DCI_S (0x80)

#define DIAG_CX_FLUSH_S (0x10) /* Used for buffer flush */
#define DIAG_CX_ANY_S (0xFFFFFFFF)

#define DIAG_SET_MASK(xx_field, xx_mask) ((xx_field) |= (xx_mask))
#define DIAG_CLR_MASK(xx_field, xx_mask) ((xx_field) &= ~(xx_mask))
#define DIAG_CHECK_MASK(xx_field, xx_mask) \
  (((xx_field) & xx_mask) ? TRUE : FALSE)

extern uint32 diag_cx_state;

/* These are shared in multiple diagpkt_* files.  De-couple later. */
int diagpkt_rxc_sc_good_frames (void);
int diagpkt_rxc_pc_good_frames (void);


typedef enum
{
  DIAG_SLEEP_DIAG = 0,
  DIAG_SLEEP_EVENT,
  DIAG_SLEEP_LOG,
  DIAG_SLEEP_MSG,
  DIAG_SLEEP_CNT
}
diag_sleep_vote_enum_type;

/*===========================================================================

FUNCTION DIAG_SLEEP_VOTE

DESCRIPTION
  This function is called to vote to allow or disallow sleep.

===========================================================================*/
void diag_sleep_vote (diag_sleep_vote_enum_type diag_svc, boolean vote);

/*===========================================================================
FUNCTION diag_control_sleep_voting

DESCRIPTION
  This function is called to enable/disable diag's sleep-voting.
===========================================================================*/
void diag_control_sleep_voting(unsigned int enable_vote);

/*===========================================================================

  Diagnostics Binary Search List (Array Implementation)

  This list implementation is optimized for efficient searches on a generic
  list.
  
  The list is implemented as an array of nodes, sorted by key and a pointer
  specified by the caller.
  
  This list has capabilities to act on duplicate entries.

===========================================================================*/

typedef struct
{
  unsigned int key;
  void *goo;
}
diag_searchlist_node_type;

typedef struct
{
  boolean initialized;          /* Must be initialized to 
                                   FALSE when allocated. */
  byte rsvd[3];                 /* Added padding to make it 4-byte aligned */
  osal_crit_sect_t crit_sect; /* May also be used by caller */
  uint32 seq_num;               /* Incremented during each search. */
  int count;
  int size;
  diag_searchlist_node_type *r; /* Array allocated w/ 'size' elements */
}
diag_searchlist_type;

/*===========================================================================

FUNCTION TYPE DIAG_SEARCHLIST_NOTIFY

DESCRIPTION
  This function type is passed to diag_searchlist_search_all() and is 
  called with a pointer to the given payload of that search and a pointer
  to the matched node for every node that matches 'key'.
  
===========================================================================*/
typedef void (*diag_searchlist_notify) (uint32 seq_num, void *payload, 
                                        diag_searchlist_node_type *node);

/*===========================================================================

FUNCTION TYPE DIAG_SEARCHLIST_MATCH

DESCRIPTION
  This function type is passed to diag_searchlist_delete() and is 
  called with a pointer to the given "match node" and the node found in
  the list.  This is used for a delete operation.  This function is called
  for every match of 'key' until this function returns TRUE, indicating a
  match.

RETURN VALUE
  TRUE is the nodes match.
  FALSE if the nodes do not match.
  
===========================================================================*/
typedef boolean (*diag_searchlist_match) (diag_searchlist_node_type *match_node,
                                          diag_searchlist_node_type *list_node);

/*===========================================================================

FUNCTION TYPE DIAG_SEARCHLIST_ADD

DESCRIPTION
  This function adds the specified node, passed by value, to the given list.
  It allocates the list using the diagnostics heap, increasing the size
  of the allocated list when the current allocation is exhausted.
  
LIMITATIONS
  This routine provides task-level critical section protection only.  It will
  fail if called from ISR context.
  
  This function handles critical sections internally and does not need to be
  protected by critical section by the caller.

RETURN VALUE
  TRUE if the add operation was successful.
  FALSE if the not successful, indicating insufficient resources in the heap.
  
===========================================================================*/
boolean
diag_searchlist_add (diag_searchlist_type *list, unsigned int key, void *goo);

/*===========================================================================

FUNCTION TYPE DIAG_SEARCHLIST_DELETE

DESCRIPTION
  This function deletes one node from the specified list if it matches
  the specified node.  Every node in the list that matches 'key' will be
  dispatched to 'match_fp' until 'match_fp' returns TRUE.
  
  If a match is found, the node is removed from the list.
  
  This operation maintains sorted ordering of the list.
  
  This routine may make the list smaller.
  
LIMITATIONS
  This routine provides task-level critical section protection only.  It will
  fail if called from ISR context.
  
  This function handles critical sections internally and does not need to be
  protected by critical section by the caller.

RETURN VALUE
  TRUE if a match was found and the node was removed.
  FALSE if a match was not found.
  
===========================================================================*/
boolean
diag_searchlist_delete (diag_searchlist_type *list, 
                        diag_searchlist_node_type node,
                        diag_searchlist_match match_fp);

/*===========================================================================

FUNCTION TYPE DIAG_SEARCHLIST_SEARCH_ALL

DESCRIPTION
  This function searches the specified list for all nodes that match 'key'.
  
  For every match, 'notify' is called with a pointer to the node and the
  specified 'payload' (unmodified).

  This operation does not alter the contents of the list.
  
  If 'notify' is NULL, the routine only checks for the existence of at 
  least 1 matching node and returns status.
  
ASSUMPTIONS
  This operation assumes the list is sorted.
  
LIMITATIONS
  This routine provides task-level critical section protection only.  It will
  fail if called from ISR context.
  
  This function handles critical sections internally and does not need to be
  protected by critical section by the caller.

RETURN VALUE
  TRUE if a match was found.
  FALSE if a match was not found.
  
===========================================================================*/
boolean
diag_searchlist_search_all (unsigned int key, 
                            diag_searchlist_type *list,
                            void *payload,
                            diag_searchlist_notify notify);

/*===========================================================================

FUNCTION DIAGPKT_PROCESS_REQUEST 

DESCRIPTION
  This function processes an inbound request packet.
  
  If 'rsp_func' == NULL, the response is sent to the comm layer.
  If 'rsp_func' != NULL, the response packet will be dispatched to the 
  function in lieu of the comm layer, along with 'param'.
  
RETURN VALUE
  None.
  
===========================================================================*/
void diagpkt_process_request (void * req_pkt, uint16 pkt_len, 
                              diag_cmd_rsp rsp_func, void *param, boolean bFromDiagCmdQ);

/*===========================================================================

FUNCTION EVENT_INIT

DESCRIPTION
  Initialize event buffers.

===========================================================================*/
void event_init (void);

/*=========================================================================== 

FUNCTION EVENT_SERVICE_PENDING

DESCRIPTION
   This routine services the pending events in DIAG context and places 
   them on the event Q.

===========================================================================*/
void event_service_pending (void);

/*=========================================================================== 

FUNCTION EVENT_STALE

DESCRIPTION
  Terminates event reporting buffer if space is available for a new buffer.
  If not, we are dropping events and will retry later.

SIDE EFFECTS
  This function signals the diag task and may cause a context switch.  It 
  is intended to be called from DIAG context.

===========================================================================*/
void event_stale (void);
#ifdef DIAG_CONSUMER_API
  void event_stale_sec (void);
#endif /* #ifdef DIAG_CONSUMER_API */
/*=========================================================================== 

FUNCTION EVENT_DRAIN

DESCRIPTION
  Drains event reporting packet (already buffered) to the outbound diagcomm 
  data stream.  Do escaping, all that jazz.  
  
  This is basically a specialized version of diagbuf_drain().  
  
  If 'mask' is foudn in DIAG's task signals, the routine will exit promptly.
  
RETURN VALUE
  TRUE if event_drain is complete, FALSE if in progress.

SIDE EFFECTS
  Changes the needControlFlag that diagbuf_drain() uses, if neccessary.
  
===========================================================================*/
boolean event_drain (osal_sigs_t mask);
boolean event_drain_sec (osal_sigs_t mask);

/*===========================================================================

FUNCTION EVENT_FLUSH

DESCRIPTION
  Triggers flush operation in the event service.  

===========================================================================*/
void event_flush (void);
void event_flush_sec(void);

/*===========================================================================

FUNCTION LOG_INIT

DESCRIPTION
  Initialize logging services.  All logs are configured to 'disabled' state.
  All entries in the "log on demand" table are erased.
  
===========================================================================*/
void log_init (void);

/*===========================================================================

FUNCTION LOG_PROCESS_EXT_LOGMASK

DESCRIPTION
  This function sets the logmask from the logmask bit field specified in the
  diag_ext_logmask_req_type packet.

  This command effects what type of entries are logged.
  
===========================================================================*/
void log_process_ext_logmask (PACK(uint8 *) logmask_ptr,
                  unsigned int num_bits);

/*===========================================================================

FUNCTION LOG_FLUSH

DESCRIPTION
  Prepares the logging service for a flush.  The latest log that was 
  committed is marked to be sent.  All others are discarded.

===========================================================================*/
void log_flush (void);

/*===========================================================================

FUNCTION LOG_PANIC

DESCRIPTION
  Prepares the logging service for panic mode.  All logs are configured off.
  All listeners are deleted.

===========================================================================*/
void log_panic (void);

/*===========================================================================

FUNCTION MSG_SET_LIMIT

DESCRIPTION
  Set the minimum message level.

===========================================================================*/
void msg_set_limit (uint8 msg_level_limit);

/*----------------------------------------------------------------------------
  Declaration of the 8 bytes in the check block which is the security password
----------------------------------------------------------------------------*/
//TODO: This is in diagtarget.h - get rid of it here.
extern byte const chk_security_code_ptr[];

extern boolean diag_services_initialized;

extern boolean diagcomm_apps_only_config;

/*===========================================================================
FUNCTION DIAG_SET_DRAIN_TIMER
DESCRIPTION
  This function sets the drain timer.
===========================================================================*/
void diag_set_drain_timer(void);
/*===========================================================================

FUNCTION DIAG_DRAIN_TIMER_ENABLED

DESCRIPTION
	Returns TRUE if drain timer length set is greater than 0 indicating
	drain timer is enabled.

===========================================================================*/

boolean diag_drain_timer_enabled(void);

/*===========================================================================

FUNCTION DIAG_CLR_DRAIN_TIMER

DESCRIPTION
  This function clears the drain timer.

===========================================================================*/

void diag_clr_drain_timer(void);
/*===========================================================================

FUNCTION DIAG_CTRL_DRAIN_TIMER_LEN

DESCRIPTION
  This function is used to update the global drain timer length.

===========================================================================*/

void diag_ctrl_drain_timer_len(unsigned int drain_timer_len);
/*===========================================================================

FUNCTION DIAGPKT_CTRL_GET_MSG_TYPE

DESCRIPTION
  This function returns the ctrl msg type in the specified diag ctrl packet.

PARAMETERS
  ptr - Pointer to a Diag control packet
  
RETURN VALUE
  The control packet's msg type value
  
===========================================================================*/

uint32 diagpkt_ctrl_get_msg_type( PACK(void *) ptr );

/*===========================================================================

FUNCTION DIAG_SET_INTERNAL_SIGS

DESCRIPTION
  Sets Diag internal signal mask, which also sets DIAG_INTERNAL_SIG. When 
  diag_handle_sigs() handles DIAG_INTERNAL_SIG, diag_handle_internal_sig() 
  is executed to handle all Diag internal sigs.

PARAMETERS
  sigs - Diag internal signals to set.
    
DEPENDENCIES
  None

RETURN VALUE
  Returns the updated internal signal mask.

SIDE EFFECTS

===========================================================================*/

dword diag_set_internal_sigs( dword sigs );

/*===========================================================================

FUNCTION DIAG_CLEAR_INTERNAL_SIGS

DESCRIPTION
  Clears Diag internal sig.  If all Diag internal sigs are cleared,
  DIAG_INTERNAL_SIG is also cleared.
  
PARAMETERS
  sigs - Diag internal signals to clear.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns the updated internal signal mask.

SIDE EFFECTS

===========================================================================*/

dword diag_clear_internal_sigs( dword sigs );

/*===========================================================================

FUNCTION DIAG_GET_INTERNAL_SIGS

DESCRIPTION
	Retrieves the mask of internal signals set.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  Returns the 32bit internal signal mask value.

SIDE EFFECTS

===========================================================================*/
dword diag_get_internal_sigs( void );

uint8 diag_set_current_preset_mask_id( uint8 preset_id );

uint8 diag_get_current_preset_mask_id( void );

/* DCI encoded Packet Structure */
typedef PACK(struct) {
    byte start;             /* Start Character -> 7E */
    byte version;           /* Version Information */
    uint16 cmd_len;         /* Length of Packet (including cmd_code and tag */
    byte cmd_req_code;      /* Code that identifies a command request */
    uint32 cmd_tag;         /* Tag to identify the client that sent this request */
    byte command[1];        /* Command packet (ends with a 7E) */
}diag_dci_enc_pkt;

/* Diag RX Packet Structure */

typedef struct {
    uint8 *dest_ptr;        /* Buffer to hold received packet       */
    unsigned int max_size;  /* Maximum size for request packet      */
    unsigned int pkt_len;   /* Packet length when it is constructed */
    diagcomm_enum_port_type port_num; /* Port on which packet is received*/
}diag_rx_pkt_type;

/* Fusion DCI connection status control packet */
typedef PACK(struct) {
    uint32 version;           /*control packet version is set to 1*/
    uint8 pair_count;         /*Number of ID/Status pair counts below*/
    uint8 peripheral_id;      /*Peripheral ID: 1 MPSS, 2 ADSP; 3 Pronto*/
    uint8 status;             /*0 DCI channel down; 1 DCI channel up*/
    uint8 end;                 /*0x7E terminator for DCI packets*/
}diag_fusion_dci_conn_status_ctrl_pkt;

/* Fusion DCI connection status control packet */
typedef PACK(struct) {
    uint32 version;           /*control packet version is set to 1*/
    uint32 magic_num;         /*magic number*/
    uint8 end;                /*0x7E terminator for DCI packets*/
}diag_fusion_dci_handshake_ctrl_pkt;

typedef PACK(union)
{
   diag_fusion_dci_conn_status_ctrl_pkt dci_conn_status_ctrl_pkt;
   diag_ctrl_msg_event_mask_type ctrl_msg_event_mask;
   diag_ctrl_msg_equip_log_mask_type ctrl_msg_equip_log_mask;
   diag_ctrl_msg_diagmode_type ctrl_msg_diagmode; 
   diag_fusion_dci_handshake_ctrl_pkt dci_handshake_ctrl_pkt; 
} diag_dci_ctrl_msg_data_type;



/* DCI encoded Packet Structure */
typedef PACK(struct) {
    uint32 cmd_tag;         /* Tag to identify the client that sent this request */
    byte command[1];        /* Command packet (ends with a 7E) */
}diag_dci_enc_pkt_type;

/* non-HDLC Packet Structure */
typedef PACK(struct) {
    byte start;             /* Start Character -> 7E */
    byte version;           /* Version Information */
    uint16 cmd_len;         /* Length of Packet (including cmd_code and tag */
    byte pkt[1];            /* Command packet (ends with a 7E) */
}diag_non_hdlc_type;

typedef PACK(struct) {
    uint32 ctrl_pkt_id; 
    uint32 ctrl_pkt_data_len;       
    diag_dci_ctrl_msg_data_type data;        /* Command packet (ends with a 7E) */
}diag_dci_ctrl_pkt_type;

typedef PACK(union)
{
  diag_dci_enc_pkt_type diag_dci_cmd_req_pkt;                             /* DIAG_CTRL_MSG_REG */
  diag_dci_ctrl_pkt_type diag_dci_ctrl_pkt;
} diag_dci_pkt_header_type;

typedef PACK(struct)
{
    byte start;             /* Start Character -> 7E */
    byte version;           /* Version Information */
    uint16 dci_pkt_length;  /* Length of Packet (excluding start, end terminators, version and length filed) */
    byte dci_cmd_code;      /* Code that identifies dci control pkt, 154 */
    diag_dci_pkt_header_type type;
} diag_dci_pkt_type;

typedef struct
{ 
  diagcomm_enum_port_type channel;
  diagpkt_master_table_type mstr_data;
} diagpkt_slave_table_type;



void diag_ctrl_update_event_mask( diag_ctrl_msg_type * ctrl_msg );
void diag_ctrl_update_log_mask( diag_ctrl_msg_type * ctrl_msg );

void diag_ctrl_update_event_preset_mask( diag_ctrl_msg_type * ctrl_msg );
void diag_ctrl_update_log_preset_mask( diag_ctrl_msg_type * ctrl_msg );


#define DIAG_CONSUMER_API_VER_1 0x01
#if !defined (DIAGPKT_MSTR_TBL_SIZE)
  #define DIAGPKT_MSTR_TBL_SIZE 128
#endif

#define DIAGPKT_USER_SPACE_TBL_SIZE 128

#if defined(DIAG_MP_MASTER)
void diag_ctrl_update_msg_bt_mask(diag_ctrl_msg_type *msg_pkt_ptr, uint32 data_pkt_len);
void diag_ctrl_update_log_range(diag_ctrl_msg_type *msg_pkt_ptr, uint32 data_pkt_len);
void diag_ctrl_update_event_last(diag_ctrl_msg_type *msg_pkt_ptr, uint32 data_pkt_len);

#endif

#endif /*DIAGI_V_H*/
