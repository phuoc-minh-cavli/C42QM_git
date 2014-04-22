#ifndef GL1_MUTEX_H
#define GL1_MUTEX_H
/*==========================================================================*/
/*!
  @file GL1_MUTEX.H

  GERAN Driver Mutex definitions

  Provides mutex definitions shared within GERAN Driver mdule

  Copyright (c) 2008 - 2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/gdrivers/inc/gl1_mutex.h#4 $ */

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ---------------------------------------------------------- 
26/06/18   km       CR1087547 Enhanced sleep wakeup adaptive timeline algorithm
05/03/18   sk       CR2230579 (FR 50568): Porting the Single Voice Session FR changes onto CT.1.0
21/12/16   mk       CR1094331 : GL1 Improvements related to CS feature
01/11/14   df       CR748602 Rework and enable adaptive timeline
30/09/14   pa       CR732428: Mutex lock grm api to prevent ISR and Task to write grm_info at the same time.
12/09/14   fan      CR636420: Run Sleep commit after GSTMR ISR at wakeup
13/08/14   cja      CR709452 Race condition between QTA cleanup and QTA fw rf init.
29/07/14   ap       CR695234: Changes for GL1's New Rach interface
27/07/14   ap       CR678067: Race condition between APP mode as ACTIVE and Device mode change as DSDA leads to crash
07/03/14   og       CR628402. mutex protect the mdsp async intf commands.
02/10/13   cja      CR552831 Add mutex for logging handler timestamps
23/08/13   ws       CR 531414 Implemented critical section for Cipher clock on/off
13/08/13   pjr      CR521654 added critical section to protect gsm_l1_common_rfa_task_msgr_client
04/09/13   cs       CR532298 Integrate all wakeup-mgr changes for Multi-SIM
25/06/13   sai      CR503864: Fix MCPM-WMGR mutex deadlock when W and G go to sleep at the same time.
14/02/13   pjr      CR452272 added support to set multisim mode at activation time
19/11/12   cja      CR422874 Add pseudo Hw FN for Dime
23/04/12   ws       CR355017 Fix Compile errors and warnings introduced to fix FR 352228
28/07/98   hv       Created to support KxMutex
===========================================================================*/

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "gmutex.h"
#include "gl1_defs.h"
#include "gl1_defs_g.h"


/*----------------------------------------------
** Critical Section to protect GL1 shared vars.
**----------------------------------------------
*/

extern rex_crit_sect_type gl1_local_crit_sec;
extern rex_crit_sect_type gl1_hw_sleep_warmup_cb_crit_sect;

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
extern rex_crit_sect_type gl1_multi_sim_mode_crit_sec;
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

/*----------------------------------------------
** Critical Section to rfa msgr clients
**----------------------------------------------
*/
extern rex_crit_sect_type gdrivers_rfa_msgr_crit_sec;

/*----------------------------------------------
** Critical Section to Cipher clock on/off
**----------------------------------------------
*/
extern rex_crit_sect_type gdrivers_cipher_crit_sec;

extern void  gdrivers_FN_mtx_lock( void );
extern void  gdrivers_FN_mtx_unlock( void );
extern rex_crit_sect_type gl1_trm_crit_sec;

extern void  gstmr_log_mtx_lock( void  );
extern void  gstmr_log_mtx_unlock( void );
extern rex_crit_sect_type gl1_hw_sleep_wakeup_cancel_crit_sect;

extern void  mdsp_async_intf_mtx_lock( void );
extern void  mdsp_async_intf_mtx_unlock( void );

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
extern void  gl1_rach_mutex_lock( void );
extern void  gl1_rach_mutex_unlock( void );
#endif /*( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM ) */
extern void  gl1_grm_info_mutex_lock(void);
extern void  gl1_grm_info_mutex_unlock(void);
extern void  gl1_hw_qta_mutex_lock( void );
extern void  gl1_hw_qta_mutex_unlock( void );
extern void  gl1_hw_app_mode_cfg_mutex_lock( void );
extern void  gl1_hw_app_mode_cfg_mutex_unlock( void );
#endif /* GL1_MUTEX_H */
