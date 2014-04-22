/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              Q M I _ M M O D E _ T A S K . C

DESCRIPTION
  This is the source file for the QMI Mmcp Task. Contained herein are the
  functions needed to initialize all the modules that execute in QMI task
  context and the main task processing loop.

EXTERNALIZED FUNCTIONS 
  qmi_mmode_task()
    QMI Task entry point and main processing loop.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI services can be used unless MC has created the QMI task
  and qmi_mmode_task() has been invoked.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/qmi/src/qmi_mmode_task.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/08/12    sk     Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "task.h"
#include "rex.h"
#include "queue.h"
#include "dog_hb_rex.h"
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif
#include "qmi_mmode_task_svc.h"
#include "qmi_mmode_taski.h"
#include "qmi_mmode_task_cmd.h"
#include "qmi_mmode_svc.h"
#include "amssassert.h"
#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif


/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*--------------------------------------------------------------------------
  Timers needed for the QMI Ctrl Task.
---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
  Dynamic dog variable
---------------------------------------------------------------------------*/
dog_report_type  qmi_mmode_dog_rpt_var            = 0;        /* Dog report ID       */
static boolean   qmi_mmodei_inited                = 0;

extern void qmi_mmode_svc_init
(
  void
);

extern void qmi_mmodei_msgr_init
(
  void
);

/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

                           LOCAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================
FUNCTION QMI_MMODEI_INIT()

DESCRIPTION
  Initialization function for the various QMI services.

DEPENDENCIES
  QMI Modem task should have been started already.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void qmi_mmodei_init
(
  void
)
{
  //qmi_mmode_tcb_ptr = rex_self();

  if (!qmi_mmodei_inited)
  {
    /*-------------------------------------------------------------------------
      Setting DCC signal handler for handling QMI messages
    -------------------------------------------------------------------------*/
    (void)qmi_mmode_set_sig_handler(QMI_MMODE_TASK_INIT_SIGNAL, 
                              qmi_mmode_task_init_cb, NULL);

    /* Set the signal to init again */
    QMI_MMODE_SET_SIGNAL(QMI_MMODE_TASK_INIT_SIGNAL);

    //QMI_MMODE_TASK_INIT_SIGNAL is already set during task init
    return;
  }
  
  /*------------------------------------------------------------------------
    Initialize MSG ROUTER Services here
  --------------------------------------------------------------------------*/
  qmi_mmodei_msgr_init();

  /*-------------------------------------------------------------------------
    Initialize the QMI-Services here
  -------------------------------------------------------------------------*/
  qmi_mmode_svc_init();

} /* qmi_mmodei_init() */

/*===========================================================================
  FUNCTION qmi_mmode_task_init_cb()

  DESCRIPTION
    CallBack function invoked when signal is set by QCSI

  PARAMETERS
    sig : Signal set
    svc_ptr : Service pointer (user data ptr) thats associated with it

  RETURN VALUE
    TRUE - SUCCESS

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_mmode_task_init_cb
(
  qmi_mmode_sig_enum_type      sig,
  void                        *svc_ptr
)
{
  if (!qmi_mmodei_inited)
  {
    /*-------------------------------------------------------------------------
      Setting DCC signal handler for handling QMI messages
    -------------------------------------------------------------------------*/
    qmi_mmodei_inited = TRUE;
    qmi_mmodei_init();
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION qmi_mmode_init_before_task_start

DESCRIPTION
  Initialize qmi mmode cmd queues and other objects that needs to be 
  initialized before the task is started

  NOTE! Any initialization that must be done before other tasks or clients
  tries to contact QMI MMODE, must be done in this initialization function.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void qmi_mmode_init_before_task_start( void )
{


  /*-------------------------------------------------------------------------
    Do power-up initializtion then wait for start signal (from the MC task,
    which controls task startup sequencing)
  -------------------------------------------------------------------------*/
    qmi_mmodei_powerup();
}


/*===========================================================================
FUNCTION QMI_MMODE_INIT()

DESCRIPTION
  This function is the entry point and main processing loop for the QMI task.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_mmode_init()
{

#ifndef FEATURE_MODEM_RCINIT
  qmi_mmode_init_before_task_start();
#endif

#ifndef FEATURE_MODEM_RCINIT
  tmc_task_start();
#endif
  
  qmi_mmodei_init();
}

/*===========================================================================
FUNCTION QMI_MMODE_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the QMI task.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_mmode_task (rex_sigs_type sigs)
  {
  rex_sigs_type     loop_sigs;      /* Signals to be processed within a task
                                       loop */
  uint8             loop_cnt;       /* # of times task loop is iterated
                                       on a signal mask                    */
  uint8             i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    {

    loop_sigs = sigs;
    sigs = 0;

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority.
      If the loop iterates for more than QMI_MMODE_MAX_SIGNALS times, some signal
      is not handled by this loop. So instead of going in to infinite loop
      perform a check
    -----------------------------------------------------------------------*/
    loop_cnt = 0;
    while( loop_sigs && loop_cnt++ < (rex_sigs_type) QMI_MMODE_MAX_SIGNALS )
    {
      /*---------------------------------------------------------------------
       *
        QMI Task command queue signal
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_MMODE_CMD_Q_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_MMODE_CMD_Q_SIGNAL );

        if ( qmi_mmodei_process_cmd() == FALSE )
        {
          sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_MMODE_CMD_Q_SIGNAL );
        }
      }
      /*---`------------------------------------------------------------------
        Check if msgr signal was set.  If set then clear signal,
        and process msgr messages.
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) QMI_MMODE_TASK_MSGR_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) QMI_MMODE_TASK_MSGR_SIGNAL );

        qmi_mmodei_process_msgr();
      }

      /*---------------------------------------------------------------------
        Process other signals with registered handlers
      ---------------------------------------------------------------------*/
      else
      {
        /*----------------------------------------------------------------------
             Handle the QMI Service Signal if set
             -----------------------------------------------------------------------*/
        for( i = QMI_MMODE_TASK_INIT_SIGNAL; i < QMI_MMODE_MAX_SIGNALS; i++)
        {
    
          if( loop_sigs & ( (rex_sigs_type) 1 <<
                                 (rex_sigs_type) i ) )
          {
            loop_sigs &= ~( (rex_sigs_type) 1 <<
                            (rex_sigs_type) i );
    
            if ( qmi_mmodei_sig_handler[i].sig_handler
                 (
                   i ,
                   qmi_mmodei_sig_handler[i].user_data_ptr
                 ) == FALSE )
            {
              sigs |= ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) i);
            }
            /*-----------------------------------------------------------------
                   allowing one signal type at a time to process which will avoid
                   the watchdog timer expiration
                  --------------------------------------------------------------------*/
            break;
          }
        }
      }
    }
    
  } /* forever */

} /* qmi_mmode_task() */

