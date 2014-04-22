/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UL & RLC DL TASK CONTROL INTERFACE
***
***
*** DESCRIPTION
***
***  Provides an interface to the RLC Uplink Task and RLC Downlink Task
***  REX scheduling frameworks.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //components/rel/geran.mpss/5.2.0/grlc/inc/grlctsk.h#5 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 03/15/01    hv     Defined RLC UP & DL task control functions
*** 07/01/04    hv     Added support for GRLC_DL_PL1_EPDAN_REQ_SIG
***
*****************************************************************************/

#ifndef INC_RLC_TSK_H
#define INC_RLC_TSK_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "queue.h"
#include "rex.h"

#include "geran_multi_sim.h"
#include "grlci.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/* ---------------------------------------------------------------------------
    RLC UL Task Signal Definitions
   ------------------------------------------------------------------------ */

/* Signal mask for UL L1 signals
*/
#define  GRLC_UL_PL1_SIG            0x01UL

/* Signal mask for UL MAC signals
*/
#define  GRLC_UL_MAC_SIG            0x02UL

/* Signal mask for UL LLC signals
*/
#define  GRLC_UL_LLC_PDU_SIG        0x04UL
#define  GRLC_UL_LLC_MSG_SIG        0x08UL

/* Signal mask for UL TIMER signals
*/
#define  GRLC_UL_TMR_SIG            0x10UL


/* Signal mask for uplink internal signal
*/
#define  GRLC_UL_ITN_SIG            0x20UL

/* Signal mask for the temporary pdu queue
*/
#define GRLC_UL_LLC_TMP_PDU_SIG     0x080UL

/* Signal mask for resumption after dynamic loading
*/
#define GRLC_UL_DYN_RESUME_SIG     0x100UL


/* Combine all signals into the UL RLC master signal mask
*/


#define  GRLC_UL_MASTER_SIG          \
         (                           \
           GRLC_UL_PL1_SIG         | \
           GRLC_UL_MAC_SIG         | \
           GRLC_UL_LLC_PDU_SIG     | \
           GRLC_UL_LLC_MSG_SIG     | \
           GRLC_UL_LLC_TMP_PDU_SIG | \
           GRLC_UL_ITN_SIG         | \
           GRLC_UL_TMR_SIG         | \
           GRLC_UL_DYN_RESUME_SIG    \
         )


/* Signal for Watchdog Reporting
*/
#define  GRLC_UL_DOG_RPT_TMR_SIG    0x40UL


/* ---------------------------------------------------------------------------
    RLC DL Task Signal Definitions
   ------------------------------------------------------------------------ */

/* Signal mask for DL L1 signals
*/
#define  GRLC_DL_PL1_SIG            0x01UL

/* Signal mask for DL MAC signals
*/
#define  GRLC_DL_MAC_SIG            0x02UL

/* Signal mask for DL TIMER signals
*/
#define  GRLC_DL_TMR_SIG            0x04UL

/*---------------------------------------------------------------------------
** Signal mask for EGPRS RLC DL to receive EPDAN processing request messages
** from EGPRS PL1
**---------------------------------------------------------------------------
*/
#define  GRLC_DL_PL1_EPDAN_REQ_SIG  0x08UL

/* Combine all signals into the DL RLC master signal mask
*/
#define  GRLC_DL_MASTER_SIG             \
         (                              \
           GRLC_DL_PL1_SIG            | \
           GRLC_DL_PL1_EPDAN_REQ_SIG  | \
           GRLC_DL_MAC_SIG            | \
           GRLC_DL_TMR_SIG              \
         )

/* Signal for Watchdog Reporting
*/
#define  GRLC_DL_DOG_RPT_TMR_SIG    0x10UL


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/

/*------------------------------
** UL task and DL task
**------------------------------
**
*/
extern rex_sigs_type      grlc_ul_task_sigs;
extern rex_sigs_type      grlc_dl_task_sigs;

/*------------------------------
** 5 uplink queues
**------------------------------
**
*/
extern q_type  grlc_ul_llc_pdu_q;
extern q_type  grlc_ul_llc_tmp_pdu_q;
extern q_type  grlc_ul_llc_msg_q;
extern q_type  grlc_ul_mac_q;
extern q_type  grlc_ul_tmr_q;


/*--------------------------------
** 3 downlink queues
**--------------------------------
*/
extern q_type  grlc_dl_mac_q;
extern q_type  grlc_dl_tmr_q;
extern q_type  grlc_dl_pl1_q;

extern gprs_sysinfo_t  sysinfo;

/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/
extern void grlc_ul_init(void);
extern void grlc_dl_init(void);

/* Dynamically loaded APIs */
extern void geran_grlc_init(void);
extern void geran_grlc_deinit(void);
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
extern void geran_grlc_resume(void);
extern void geran_grlc_suspend(void);
#endif

#endif /* INC_RLC_TSK_H */

/*** EOF: don't remove! ***/
