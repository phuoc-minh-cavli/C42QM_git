/*=============================================================================

                  T I M E  - I P C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements IPC time task

  Slow Clock Frequency          Granularity     Timer Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs

EXTERNALIZED FUNCTIONS
  time_ipc_task
    Initializes time Time_IPC task

  INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2016 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/services/time/src/time_ipc.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
07/12/10   din     Made changes to compile on apps as well.
03/25/10   din     Updated time_ipc to be used for single proc targets.
03/11/10   din     Moved efs write to Time IPC task.
02/22/10   din     Fixed compilation issue incase DOG_DYNAMIC_API is not 
                   defined.
02/19/10   din     Moved genoff RPC calls from modem proc to TIME_IPC task.
02/08/10   sri     Made changes for DOG_DYNAMIC_API
05/22/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "timer.h"
#include "time_ipc.h"
#include "time_genoff_v.h"
#include "qurt.h"
#include "dog.h"
#include "time_efs.h"
#include "msg.h"
#include "rcinit_qurt.h"
#include "qmi_time_client.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef FEATURE_QMI_TIME_REMOTE_CLNT
#include "time_service_v01.h"
#include "qmi_client.h"
extern qmi_client_type time_qmi_client_handle;
#endif/*FEATURE_QMI_TIME_REMOTE_CLNT*/

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/* Watchdog report signal */
#define TIME_IPC_RPT_TIMER_SIG   0x0004

/*  Dog variables - now we MAY be dynamic       */
uint32  dog_time_ipc_rpt       = 0;       /* dog report ID */
uint32  dog_time_ipc_rpt_time;            /* Report Period */

#ifdef DOG_DYNAMIC_API
#define  DOG_TIME_IPC_RPT       dog_time_ipc_rpt        /* rpt_def */
#define  DOG_TIME_IPC_RPT_TIME  dog_time_ipc_rpt_time   /* time_def */

#endif /* DOG_DYNAMIC_API */

/* Time_IPC watchdog report timer. */
timer_type       time_ipc_rpt_timer;

/* Boolean to indicate if the signal object is initialized */
static boolean time_ipc_signal_initialized = FALSE;

/* Time IPC signal object pointer */
qurt_signal_t * time_ipc_qurt_sig_ptr = NULL;


#ifdef FEATURE_TIME_IPC_TASK

/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/


/*===========================================================================

FUNCTION TIME_IPC_KICK_WATCHDOG

DESCRIPTION
  This procedure reports to the watchdog and resets the timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None


===========================================================================*/
static void time_ipc_kick_watchdog(void)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* report to DOG task */
  dog_report(DOG_TIME_IPC_RPT);

  /* Start timer to report to dog */
  timer_set(&time_ipc_rpt_timer,
 			(timetick_type)DOG_TIME_IPC_RPT_TIME,
 			0,
 			T_MSEC);
} /* time_ipc_kick_watchdog */



/*===========================================================================

FUNCTION TIME_IPC_READY_START_TASK

DESCRIPTION
  This procedure is called  by timer IPC when it is ready to start its task.
  It will wait on TASK_START_SIG after this call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None


===========================================================================*/
static void time_ipc_ready_start_task (void)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rcinit_handshake_startup();                                                   /* Handshake allows startup to progress */

  MSG(MSG_SSID_ATS, MSG_LEGACY_MED,"TIME IPC TASK initialization successfull");

} /* time_ipc_ready_start_task */

#endif /* FEATURE_TIME_IPC_TASK */


/*===========================================================================

FUNCTION TIME_IPC_READY_START_TASK

DESCRIPTION
  This procedure is called  by timer IPC when it is ready to start its task.
  It will wait on TASK_START_SIG after this call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None


===========================================================================*/
void time_ipc_initialize_signal_object(void)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (time_ipc_signal_initialized == TRUE)
  	return;

  time_ipc_signal_initialized = TRUE;

  /* Allocate memory for the signal object */
  time_ipc_qurt_sig_ptr = malloc(sizeof(qurt_signal_t));
  if (time_ipc_qurt_sig_ptr == NULL)
  	{
  		ERR_FATAL("Cannot alloate memory for time IPC signal object",0,0,0);
		return;
  	}

  /* Initialize the signal object */
  qurt_signal_init(time_ipc_qurt_sig_ptr);

} /* time_ipc_ready_start_task */


/*=============================================================================

FUNCTION TIME_IPC_TASK

DESCRIPTION
  Initializes the Time_IPC Task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void  time_ipc_task
(
  /* Unused parameter - required to match the task function prototype */
  dword                           parm    /*lint -esym(715,ignored) */
)
{

#ifdef FEATURE_TIME_IPC_TASK
  /* Signals returned from qurt wait procedure */
  uint32    sigs;
  int ret = QMI_NO_ERR;
  
  #ifdef FEATURE_QMI_TIME_REMOTE_CLNT
  time_get_leap_sec_resp_msg_v01 time_resp;
  time_genoff_leap_sec_info_type leap_sec_info;
  /* Remove compiler warnings */
  time_resp.resp.error = QMI_ERR_NONE_V01;
  time_resp.leap_second = 0;
  #endif /*FEATURE_QMI_TIME_REMOTE_CLNT*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Keep Lint happy */

  #ifdef _lint
    parm = parm;
  #endif


  /* Init the signal object */
  time_ipc_initialize_signal_object();

  /* Initialize the watchdog report timer */
#ifdef DOG_DYNAMIC_API
  dog_time_ipc_rpt      = dog_register_qurt( qurt_thread_get_id(), \
                                  DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE);
  dog_time_ipc_rpt_time = dog_get_report_period( dog_time_ipc_rpt );
#else  /* ! DOG_DYNAMIC_API */
  dog_time_ipc_rpt      = DOG_TIME_IPC_RPT;       /* ID */
  dog_time_ipc_rpt_time = DOG_TIME_IPC_RPT_TIME;  /* Report Period */
#endif /* ! DOG_DYNAMIC_API */

  /* If the return value from dog_get_report_period() was zero then we MUST
  ** NOT report to dog directly, as DOG will monitor us in a different manner.
  ** Hence we won't initialize or start any timer.
  */
  if ( dog_time_ipc_rpt_time > 0 ) {
  /* Initialize time_ipc watchdog report timer */
  timer_def_osal(&time_ipc_rpt_timer,
				NULL,
				TIMER_NATIVE_OS_SIGNAL_TYPE,
				(time_osal_notify_obj_ptr)time_ipc_qurt_sig_ptr,
				(time_osal_notify_data)TIME_IPC_RPT_TIMER_SIG);
  }

  /* Ready to start task */
  time_ipc_ready_start_task();

  if ( dog_time_ipc_rpt_time > 0 ) {
  /* Kick the wathcdog and start task operations. */
  time_ipc_kick_watchdog();
  }

  for(;;)
  {
    /* Wait for some task signal */
    sigs = qurt_signal_wait( time_ipc_qurt_sig_ptr,
                        TIME_IPC_RPT_TIMER_SIG
                        | TIME_IPC_GENOFF_UPDATE
                        | TIME_IPC_EFS_WRITE
                        | TIME_IPC_MM_REGISTRATION
                        | TIME_IPC_SEND_REMOTE_MODEM_UDPATE
                        | TIME_IPC_SEND_REMOTE_USER_UDPATE
                        | TIME_IPC_SEND_REMOTE_USER_TZ_DL_UDPATE
                        | TIME_IPC_QMI_REMOTE_COMM
                        | TIME_IPC_QMI_REMOTE_COMM_TIMEOUT
                        | TIME_IPC_QMI_REMOTE_COMM2
						| TIME_IPC_QMI_CLIENT_INIT,
                        QURT_SIGNAL_ATTR_WAIT_ANY);
   
    /* Just in case TIME_IPC_RPT_TIMER_SIG fired which would bring us here*/
    if ( sigs & TIME_IPC_RPT_TIMER_SIG  )
    {
      /*signal handler for TIME_IPC_RPT_TIMER_SIG goes here*/

	  qurt_signal_clear(time_ipc_qurt_sig_ptr,TIME_IPC_RPT_TIMER_SIG);

      /* Kick watchdog and start the watchdog report timer.   */
      if ( dog_time_ipc_rpt_time > 0 ) 
      {
        time_ipc_kick_watchdog();
      }
    }
	
	if(sigs & TIME_IPC_QMI_CLIENT_INIT)
	 {
		 qurt_signal_clear(time_ipc_qurt_sig_ptr,TIME_IPC_QMI_CLIENT_INIT);
		 ret = qmi_time_client_connect();
		 
		 if(ret != QMI_NO_ERR)
			 MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"qmi_time_client_connect failed : %d",ret);
	 }

    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                     TIME_IPC EFS_WRITE SIGNAL                           */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    if ( sigs & TIME_IPC_EFS_WRITE )
    {
	  qurt_signal_clear(time_ipc_qurt_sig_ptr,TIME_IPC_EFS_WRITE);
      /* Process the expired timer */
      #ifdef FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
      time_efs_write();
      #endif /* FEATURE_TIME_SINGLE__MULTI_SERVER_PROC  */
    }

     /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                     TIME_MM_REGISTRATION_SIGNAL SIGNAL                           */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    /* We don't support this anymore */
    if ( sigs & TIME_IPC_MM_REGISTRATION )
    {
      MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Unsupported signal set on time_ipc_task: 0x%x",TIME_IPC_MM_REGISTRATION);
	  qurt_signal_clear(time_ipc_qurt_sig_ptr,TIME_IPC_MM_REGISTRATION);
    }

    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                     TIME_IPC_SEND_REMOTE_MODEM_UDPATE SIGNAL                           */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    if ( sigs & TIME_IPC_SEND_REMOTE_MODEM_UDPATE )
    {
	  qurt_signal_clear(time_ipc_qurt_sig_ptr,TIME_IPC_SEND_REMOTE_MODEM_UDPATE);
      MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"mm_debug_fusion:Processing request for remote modem update");
      /* Process the mm timer expiry */
      time_genoff_set_server_modem_time(ATS_TOD);
    }

    if ( sigs & TIME_IPC_SEND_REMOTE_USER_UDPATE )
    {
	  qurt_signal_clear(time_ipc_qurt_sig_ptr,TIME_IPC_SEND_REMOTE_USER_UDPATE);
      MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"mm_debug_fusion:Processing request for remote user time update");//auto-gen, to change remove 'QSR_' and first param
      /* Process the mm timer expiry */
      time_genoff_set_server_modem_time(ATS_USER);
    }

    if ( sigs & TIME_IPC_SEND_REMOTE_USER_TZ_DL_UDPATE )
    {
	  qurt_signal_clear(time_ipc_qurt_sig_ptr,TIME_IPC_SEND_REMOTE_USER_TZ_DL_UDPATE);
      MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"mm_debug_fusion:Processing request for remote user tz dl update");//auto-gen, to change remove 'QSR_' and first param
      /* Process the mm timer expiry */
      time_genoff_set_server_modem_time(ATS_USER_TZ_DL);
    }
   
    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                       TIME_TOD RESYNC SIGNAL                           */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    if ( sigs & TIME_TOD_RESYNC_SIG )
    {
      MSG_1(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Unsupported signal set on time_ipc_task: 0x%x",TIME_TOD_RESYNC_SIG);
	  qurt_signal_clear(time_ipc_qurt_sig_ptr,TIME_TOD_RESYNC_SIG);
    }

    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                       GENOFF OFFSET UPDATE SIGNAL                       */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    if ( sigs & TIME_IPC_GENOFF_UPDATE )
    {
	  qurt_signal_clear(time_ipc_qurt_sig_ptr,TIME_IPC_GENOFF_UPDATE);
      #if (defined FEATURE_TIME_MULTI_SERVER_PROC || defined FEATURE_QMI_TIME_REMOTE)
      time_genoff_remote_through_time_ipc();
      #endif /* FEATURE_TIME_MULTI_SERVER_PROC  */
    }

    if(sigs & TIME_IPC_QMI_REMOTE_COMM) 
    {
	  qurt_signal_clear(time_ipc_qurt_sig_ptr,TIME_IPC_QMI_REMOTE_COMM);
    }

    if(sigs & TIME_IPC_QMI_REMOTE_COMM_TIMEOUT) 
    {
	  qurt_signal_clear(time_ipc_qurt_sig_ptr,TIME_IPC_QMI_REMOTE_COMM_TIMEOUT);
    }

    if(sigs & TIME_IPC_QMI_REMOTE_COMM2) 
    {
	  qurt_signal_clear(time_ipc_qurt_sig_ptr,TIME_IPC_QMI_REMOTE_COMM2);

      #ifdef FEATURE_QMI_TIME_REMOTE_CLNT
      /*if its a TOD offset, then set the leap second info*/
      (void) qmi_client_send_msg_sync(time_qmi_client_handle,
                                      QMI_TIME_LEAP_SEC_GET_REQ_V01,
                                      NULL,
                                      0,
                                      &time_resp,
                                      sizeof(time_resp),
                                      5000);
    
       if(QMI_ERR_NONE_V01 == time_resp.resp.error) 
       {
         leap_sec_info.leap_sec_valid = TRUE;
         leap_sec_info.leap_sec_value = time_resp.leap_second;
         time_genoff_set_leap_sec_info(leap_sec_info);
         MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Success");
       }

      #endif
    }
  } /* for (;;) */

#else
  MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"TIME_IPC : FEATURE_TIME_IPC_TASK not defined.");
  return;
#endif /* FEATURE_TIME_IPC_TASK */

} /* time_ipc_task */


