/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      ERR service crash simulator
                
GENERAL DESCRIPTION
  Command handler for injecting various types of crashes for testing

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Diag must be initialized

Copyright (c) 2013 -2015 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.tx/6.0/debugtools/eic_rpm/src/err_inject_crash_rpm.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/01/13   rks     Fix for CR502558:use non-deferrable timer
10/28/11   mcg     File created.

===========================================================================*/

#ifndef ERR_INJECT_CRASH
  #error Required definition is missing, check configuration
#endif //ERR_INJECT_CRASH

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "err.h"
#include "err_inject_crash_rpm.h"
#include "diagpkt.h"
#include "tms_diag.h"
#include "diagcmd.h"
#include "osal.h"
#include "msg.h"
#include "timer.h"
#include "timetick.h"


#include <stdint.h>

/*===========================================================================

                     EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
PACK(void *) err_inject_crash_rpm_diag (PACK(void *), uint16);
extern unsigned rpm_msg_to_misc_resource_to_abort(void);


/*===========================================================================

                     INTERNAL DATA DECLARATIONS

===========================================================================*/
// default value if no delay is provided (min value for device config input)
#define ERR_INJECT_CRASH_DELAY_DIAG_DEFAULT  1

/* table for diag packet command handler registration */
static diagpkt_user_table_entry_type err_inject_crash_rpm_diagpkt_tbl[TMS_DIAG_TBLSIZE];
typedef PACK(struct) 
{
  diagpkt_subsys_header_type hdr;
  uint8 crash_type;
  uint8 crash_delay;
} err_inject_crash_rpm_req_type;

static timer_type       eicTimer;
static timer_group_type eic_timer_group = {0};
static boolean          crash_pending = FALSE;

typedef struct err_crash_info_s
{
  boolean               is_crash_set;
  err_inject_crash_rpm_type crash;
  uint8                 delay;
} err_crash_info_t;

static err_crash_info_t crash_info;

/*===========================================================================

                        Private function prototypes

===========================================================================*/

static void err_inject_crash_rpm_initiate(err_inject_crash_rpm_type, uint32);
static void err_inject_crash_rpm_execute(timer_cb_data_type);



/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION err_inject_crash_rpm_init

DESCRIPTION
  Initializes the crash injection feature

DEPENDENCIES
  diag must be available to register command handler

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void err_inject_crash_rpm_init (void)
{
  err_inject_crash_rpm_diagpkt_tbl[0] = tms_diag_build_table_entry( TMS_DIAGPKT_RPM_CRASH, err_inject_crash_rpm_diag);
  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (ERR_DIAG_PROC_ID, DIAG_SUBSYS_DEBUG, err_inject_crash_rpm_diagpkt_tbl);
                                         
  //initialize timer group to be safe
  memset(&eic_timer_group, 0, sizeof(eic_timer_group)); 
  timer_def_osal(&eicTimer,
  	             &eic_timer_group,
  	             TIMER_FUNC1_CB_TYPE,
  	             (time_osal_notify_obj_ptr)err_inject_crash_rpm_execute,
  	             (time_osal_notify_data)&crash_info);
}

/*===========================================================================

FUNCTION err_inject_crash_rpm_diag

DESCRIPTION
  Force a crash to occur remotely by calling ERR_FATAL

DEPENDENCIES
  err_init must be called prior

RETURN VALUE
  Pointer to the response packet of the req_ptr

SIDE EFFECTS

===========================================================================*/
PACK(void *)
err_inject_crash_rpm_diag (PACK(void *) req_ptr, uint16 pkt_len)
{
  err_inject_crash_rpm_req_type *req;
  err_inject_crash_rpm_type injected_crash = ERR_INJECT_ERROR;
  uint8 injected_delay = ERR_INJECT_CRASH_DELAY_DIAG_DEFAULT;

  // Diag only handles one request at a time
  // so simple mutual exclusion is sufficient (for now)  
  if (crash_pending)
  {
    MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"err_inject_crash_rpm: crash pending, ignoring subsequent request");
    tms_diag_send_generic_response(req_ptr);
    return(NULL);
  }
  else
  {
    crash_pending = TRUE;  //set before diag response to guarantee no reentrancy issues
    tms_diag_send_generic_response(req_ptr);
  }
  
  // decode request
  req = (err_inject_crash_rpm_req_type *) req_ptr;

  // default parameters
  if (pkt_len <= sizeof(req->hdr)) 
  {
    injected_crash = ERR_INJECT_ERROR;
    injected_delay = ERR_INJECT_CRASH_DELAY_DIAG_DEFAULT;
  }
  // default delay parameter
  else if (pkt_len <= (sizeof(req->hdr) + sizeof(req->crash_type)) )
  {
    injected_crash = (err_inject_crash_rpm_type)(req->crash_type);
    injected_delay = ERR_INJECT_CRASH_DELAY_DIAG_DEFAULT;
  }
  else
  {
    injected_crash = (err_inject_crash_rpm_type)(req->crash_type);
    injected_delay = (err_inject_crash_rpm_type)(req->crash_delay);
  }

  err_inject_crash_rpm_initiate(injected_crash, injected_delay);

  return(NULL);  //return NULL since we already called diagpkt_commit above
}


/*===========================================================================

FUNCTION err_inject_crash_rpm_initiate

DESCRIPTION
  Initiates the crash task which will perform the actual crash once timer expires

DEPENDENCIES
  err_init must be called prior

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void err_inject_crash_rpm_initiate(err_inject_crash_rpm_type req_crash, uint32 req_delay)
{
  // validate crash cause
  if (req_crash >= SIZEOF_ERR_INJECT_CRASH_RPM_TYPE) 
  {
    //do not err_fatal, as this may present false positives
    MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,"err_inject_crash_rpm: invalid crash ID %#x", req_crash);
    return;
  }

  crash_info.crash      = req_crash;
  crash_info.delay      = req_delay;
  //check for immediate crash (from caller context)
  if (0==req_delay)
  {
    err_inject_crash_rpm_execute((timer_cb_data_type)&crash_info);
  }
  else
  {
    //set cb timer to trigger actual crash
    timer_set_64(&eicTimer, (timetick_type)req_delay, 0, T_SEC);
  }

}

void err_inject_crash_rpm_execute(timer_cb_data_type crash_info)
{
  rpm_msg_to_misc_resource_to_abort();
  crash_pending = FALSE;
}


