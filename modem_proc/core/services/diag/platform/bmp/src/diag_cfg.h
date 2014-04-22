#ifndef DIAG_CFG_H
#define DIAG_CFG_H
/*===========================================================================

                                  D I A G _ C F G . H

  GENERAL DESCRIPTION
  Contains configuration information for diag per platform

Copyright (c) 2010-2015,2018-2019 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
$Header: //components/rel/core.mpss/3.10/services/diag/platform/bmp/src/diag_cfg.h#6 $

when        who    what, where, why
--------    ---   -------------------------------------------------------------
09/04/18    gn    Memory optimizations
08/07/15    ph    Updated the send buffer pad length for NRT mode.
08/04/15    ph    Time Sync between processors within the chip-Diag changes. 
05/15/15    sr    Changed the SMDL FIFO size to 8k  
04/08/15    ps    Diag over sockets and dsm removal
07/17/14    xy    Utilized optimized time get API  
06/05/14    rh    Support for 8kB commands and responses
04/29/13    ph    Added a limit on active listeners.
01/16/13    rh    Changed diagbuf NRT drain parameters 
01/03/13    sg    Command Req/Response Channel separation
05/10/12    rh    Added separate heap for Diag responses. 
10/26/11    hm    Extended Event/Log Listener APIs added  
02/28/11    sg    Power saving changes
02/28/11    vs    Increased event heap size 
12/10/10    vs    Reverted 8 byte alignement for q6
11/18/10    vs    Moved definition of DIAGBUF_NEXT_ALIGNED_BYTE here
11/10/10    mad   Increased event heap size to 8K
10/27/10    SG    Moved DIAG_TX_SLEEP_THRESHOLD_DEFAULT,DIAG_TX_SLEEP_TIME_DEFAULT
                  to here from diagbuf_v.h
06/07/10    VG    Updated DIAGBUF_SIZE to be able to configure it in custdiag.h
03/10/10    JV    Reduced size of diag heap back to 8K. Use system heap for
                  stress test.
03/03/10    as    Increased Diag heap to accomadate Ondevice mask processing
02/16/10    vs    Changed diagbuf size to 128K for 9k q6
01/18/10    JV    Reduce the size of diagbuf to 32K
07/17/09    JV    Created for BMP
===========================================================================*/
#include "diag_v.h"
#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h" /* For stubbed ts_get(); for Q6 modem */
#else
#include "time_svc.h" /* for time_get(); for Q6 apps and all other targets */
#endif


#define DIAGBUF_NEXT_ALIGNED_BYTE(p) (((p) + 3) & ~3)

#if defined(DIAG_SOCKET_SUPPORT)


#if defined(DIAG_IMAGE_MODEM_PROC)
#define DIAG_INST_ID_BASE 0  //DIAG socket channels for modem will have instance ID range from 0--63
#elif defined (DIAG_IMAGE_QDSP6_PROC)
#define DIAG_INST_ID_BASE 64  //ADSP sockets will range from 64-127
#endif

#define DIAG_SOCKET_INST_ID_CTRL (DIAG_INST_ID_BASE)
#define DIAG_SOCKET_INST_ID_CMD (DIAG_INST_ID_BASE+1)
#define DIAG_SOCKET_INST_ID_DATA (DIAG_INST_ID_BASE+2)
#define DIAG_SOCKET_INST_ID_DCI_CMD (DIAG_INST_ID_BASE+3)
#define DIAG_SOCKET_INST_ID_DCI_DATA (DIAG_INST_ID_BASE+4)

#define DIAG_MAX_NUM_SOCKETS 6
#endif

/* Not adding the DIAG_MAX_TX_PKT_SIZ and DIAG_MAX_RX_PKT_SIZ here because 
they need to be exposed to clients. Currently defined in diagpkt.h */

/* DIAGBUF_SIZE is the size, in bytes, of diag's output buffer.  If this buffer
 * fills up, logs and messages will be dropped.  It is recommended to make this
 * number a power of 2 to allow the compiler to optimize the arithmetic using
 * this number to use shift operations instead of brute force math.
 *
 * Note: This must be larger than DIAG_MAX_TX_PKT_SIZ. */

/* DIAGBUF_SIZE defines the size of the diagbuffer from which logs and messages are allocated. Reducing this size may lead to increase in drop counts.*/
#ifndef DIAGBUF_SIZE
#define DIAGBUF_SIZE (1024*32*2)
#endif

/* When the total size of data committed reaches DIAGBUF_COMMIT_THRESHOLD we start draining.*/
// Keeping a ratio of 16. This will make sure that on changing DIAGBUF_SIZE value, DIAGBUF_COMMIT_THRESHOLD will also be adjusted
#define DIAGBUF_COMMIT_THRESHOLD (DIAGBUF_SIZE/16)	               /* # of bytes committed to Diag buffer before draining. */

#if (DIAGBUF_SIZE < DIAGBUF_COMMIT_THRESHOLD)
#error "DIAGBUF_SIZE cannot be smaller than DIAGBUF_COMMIT_THRESHOLD. In that case threshold will never be reached."
#endif

#ifndef DIAG_CTRL_BUF_SIZE
#define DIAG_CTRL_BUF_SIZE (8*1024)
#endif

#define DIAG_CTRL_RESEND_BUF_SIZE (16*1024)

#ifndef DIAG_TX_SLEEP_THRESHOLD_DEFAULT
#define DIAG_TX_SLEEP_THRESHOLD_DEFAULT (1024*16)
#endif

#ifndef DIAG_TX_SLEEP_TIME_DEFAULT
#define DIAG_TX_SLEEP_TIME_DEFAULT 10
#endif



#define DIAG_MAX_SOCKETS_DATA_BLOCK_SIZE 0x4000 /* size of block that can be sent on the sockets channel */
#define DIAG_MAX_SMDL_DATA_BLOCK_SIZE 0x2000 /* size of block that can be sent on the SMDL channel */

#define DIAG_CTRL_READ_BUF_SIZE 0x2000 //8k
/* The size of the DIAG heap.  Currently, it is only used for response 
   packets and allocating tasks for stress testing.  DIAGBUF_SIZE only 
   impacts MSG and LOG packets. The size cannot be less than 64 bytes. */
#ifndef DIAG_HEAP_SIZE   
#define DIAG_HEAP_SIZE (1024 * 8)
#endif

/* DIAG_RSP_HEAP_SIZE is the size of the heap from which the responses for the requests are allocated. The size cannot be less than 64.bytes. */
#ifndef DIAG_RSP_HEAP_SIZE
#define DIAG_RSP_HEAP_SIZE (1024 * 16)
#endif
/* These are the non real time values that are used when diag is in power savings mode*/
#define DIAGBUF_COMMIT_THRESHOLD_NRT (DIAGBUF_SIZE/2)

#define DIAG_TX_SLEEP_THRESHOLD_NRT  (DIAGBUF_SIZE/2)

#define DIAG_TX_SLEEP_TIME_NRT       5 

#define EVENT_RPT_PKT_SIZE_NRT (1000)

/* DIAG_EVENT_HEAP_SIZE defines the size of the event heap from which events are allocated. Reducing this size to a very low value will result in event drops.
The size cannot be less than 64 bytes.*/
#ifndef DIAG_EVENT_HEAP_SIZE
#define DIAG_EVENT_HEAP_SIZE (1024 * 16)
#endif

#ifdef DIAG_CONSUMER_API
  #define DIAG_EVENT_SEC_HEAP_SIZE (64)
#endif /* DIAG_CONSUMER_API */

#ifdef DIAG_LISTENERS_SUPPORT
  #define DIAG_MAX_ACTIVE_LISTENERS ( 18 )     				/*Arbitrary limit on active listeners to avoid exhaustion of system heap */
  #define DIAG_LISTENERS_HEAP_SIZE (64)
#endif /* DIAG_LISTENERS_SUPPORT */

#define DIAG_HDLC_PAD_LEN (150)                            /* # of bytes padding to account for hdlc/crc bytes when filling DSM item */

/* Configurations for Diag Fwd task */
#ifndef DIAG_FWD_TX_SLEEP_THRESHOLD_DEFAULT
  #define DIAG_FWD_TX_SLEEP_THRESHOLD_DEFAULT (10)         /* X number of DSMs to drain before suspending */
#endif

#ifndef DIAG_FWD_TX_SLEEP_TIME_DEFAULT
  #define DIAG_FWD_TX_SLEEP_TIME_DEFAULT (10)              /* # of ms to sleep after draining X number of DSMs */
#endif

#endif /* DIAG_CFG_H */


