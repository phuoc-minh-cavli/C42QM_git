#ifndef DIAG_FWD_V_H
#define DIAG_FWD_V_H
/*==========================================================================

                     Diagnostic Fwd Task Header File

Description
  Global Data declarations for the diag_fwd_task.

Copyright (c) 2010-2016, 2019 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                         Edit History

      $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diag_fwd_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/19   kdey    Porting of Qsockets
08/09/16   ph      Adjust the priorities of DIAG tasks.
07/20/16   nk      Changes to support TX
08/25/15   nk      Defined diag_fwd_task variables in diag_fwd.c and
                   extern them in other files.
03/07/14   sa      Renamed DIAG_TX_SLAVE_SIG to DIAG_FWD_TX_SLAVE_SIG
                   Renamed DIAG_RX_RSP_TX_SIG to DIAG_FWD_RX_RSP_TX_SIG
02/26/14   sa      Added support for compression in buffering mode
02/21/14   is      Command/response separation
01/07/14   xy      Added fusion DCI feature
12/05/13   sr      Added drain timer for forward channel
10/01/13   xy      Removed code under FEATURE_WINCE
08/31/12   ra      Use accessor macros instead of accessing private task
                   info directly; accomodated changes for User space
                   task priority changes
01/24/12   is      Support for buffered circular mode
09/09/10   SG      Increased the diag_fwd_task stack size to 16k
05/25/10   JV      Added comments
04/06/10   JV      Created
===========================================================================*/

#include "osal.h"

#define DIAG_FWD_STACK_SIZE 2048  /* type is unsigned long long, so stack size will be 16K */

#define DIAG_FWD_READ_DCI_SIG    	0x00000004     /*Signal to read the DCI traffic received from Slave */

#define DIAG_FWD_TX_DRAIN_SIG          0x00000010   /* Signal to Drain the Data from
                                                       Compression buffer */

#define DIAG_FWD_CMD_READ_SIG    	0x00000020     /*Signal to read the command responses received from Slave */

#define DIAG_FWD_TOUT_TIMER_SIG     0x00000100

#define DIAG_FWD_DRAIN_TIMER_SIG    0x00000200    /* Timer signal to drain DSM
                                                     items in fwd channel if there
                                                     is less traffic*/

#define DIAG_FWD_TX_SLAVE_SIG       0x00080000    /* Signal to process Diag traffic
                                                   generated at slave Processors */
												   
#define DIAG_FWD_READ_SIG    		0x00800000    /*Signal to read the data traffic received from Slave */



#define DIAG_FWD_RPT_TIMER_SIG      0x02000000    /* timer signal for watchdog
                                                   for the diag_fwd_task */				

#define	DIAG_FWD_TX_DCI_SLAVE_SIG	0x40000000	/*Signal to process Diag dci traffic generated at slave Processors*/


#define DIAG_FWD_NUM_COMPRESSED_ITEMS_TO_DRAIN  10 /* number of compressed chunks to drain at a time */
/* Configurations for Diag Fwd task */
#ifndef DIAG_FWD_TX_SLEEP_THRESHOLD_DEFAULT
  #define DIAG_FWD_TX_SLEEP_THRESHOLD_DEFAULT (10)         /* X number of DSMs to drain before suspending */
#endif
#ifndef DIAG_FWD_TX_SLEEP_TIME_DEFAULT
  #define DIAG_FWD_TX_SLEEP_TIME_DEFAULT (10)              /* # of ms to sleep after draining X number of DSMs */
#endif
#if defined(DIAG_QDSP6_APPS_PROC)
    #include "diagstub.h"
    #define DIAG_FWD_PRI (GET_DIAG_PRI() + 1)
#else
    #define DIAG_FWD_PRI (GET_DIAG_PRI() + 1);
#endif

#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)

extern osal_tcb_t diag_fwd_task_tcb;
extern osal_thread_attr_t diag_fwd_task_attr;

#endif



/*===========================================================================

FUNCTION DIAG_FWD_TASK_START

DESCRIPTION
  This function contains the diagnostic fwd task. This task processes TX traffic
  from the slave processor.

  This procedure does not return.

===========================================================================*/
void diag_fwd_task_start(uint32 params);

/*===========================================================================

FUNCTION DIAG_FWD_KICK_WATCHDOG

DESCRIPTION
  This function kicks the watchdog for the diagnostic fwd task

===========================================================================*/
void diag_fwd_kick_watchdog(void);

/*===========================================================================

FUNCTION DIAG_FWD_SUSPEND
DESCRIPTION
  This function suspend the diag fwd task for a defined time period
===========================================================================*/
void diag_fwd_suspend (unsigned int);
/*===========================================================================
FUNCTION DIAG_FWD_HANDLE_SIGS

DESCRIPTION
  This routine waits for
  1. DIAG_FWD_TX_SLAVE_SIG and then processes the TX
     traffic from slave processors and the apss traffic.
  2. DIAG_FWD_TX_DRAIN_SIG and then drains the data
     data from the compression buffer.


===========================================================================*/
void diag_fwd_handle_sigs(void);

/*===========================================================================

FUNCTION DIAG_CTRL_FWD_DRAIN_TIMER_LEN

DESCRIPTION
  This function modifies the forward channel drain timer length based on
  the timer length value that is passed in

PARAMETERS
  timer_len  Set the fwd drain timer if this value is greater than 0
             else, clear the fwd drain timer

===========================================================================*/
void diag_ctrl_fwd_drain_timer_len(unsigned int timer_len);


/*===========================================================================

FUNCTION DIAG_SET_FWD_DRAIN_TIMER

DESCRIPTION
  This function sets the forward channel drain timer.

===========================================================================*/
void
diag_set_fwd_drain_timer(void);

/*===========================================================================

FUNCTION DIAG_CLR_FWD_DRAIN_TIMER

DESCRIPTION
  This function clears the forward channel drain timer.

===========================================================================*/
void
diag_clr_fwd_drain_timer(void);


#endif /*DIAG_FWD_V_H*/

