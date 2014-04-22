#ifndef GMUTEX_H
#define GMUTEX_H
/*==========================================================================*/
/*!
  @file GMUTEX.h

  GERAN Mutual Exclusion Definitions

  Provides the switching points to support either REX legacy INTLOCK/TASKLOCK or
  proper KxMutex based on the featurisation FEATURE_GERAN_KXMUTEX_SUPPORT

  Copyright (c) 2008-2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/gcommon/inc/gmutex.h#2 $ 
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 23/08/13    ws     CR531414 Implemented critical section for Cipher clock on/off
*** 13/08/13    pjr    CR521654 added critical section to protect gsm_l1_common_rfa_task_msgr_client
*** 14/02/13    pjr    CR452272 added support to set multisim mode at activation time
*** 28/09/12    hv     Added GERAN Dual Data Space support for GERAN user-plane
***
*****************************************************************************/

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */



/* GERAN critical sections */

/* GL1 */

#ifdef FEATURE_GL1_LOG_MTX_WAIT
#define GL1_ISR_LOCK()            {uint32 gl1_crit_sec_t1, gl1_crit_sec_t2; \
                                    gl1_crit_sec_t1 = timetick_get(); \
                                    rex_enter_crit_sect(&gl1_local_crit_sec); \
                                    gl1_crit_sec_t2 = timetick_get(); \
                                    gl1_log_mtx_wait(gl1_crit_sec_t1, \
                                    gl1_crit_sec_t2);}
#else
#define GL1_ISR_LOCK()             rex_enter_crit_sect(&gl1_local_crit_sec);
#endif /* FEATURE_GL1_LOG_MTX_WAIT */

#define GL1_ISR_UNLOCK()           rex_leave_crit_sect(&gl1_local_crit_sec);

#define GL1_ISR_SAVE_LOCK()        GL1_ISR_LOCK()
#define GL1_ISR_SAVE_UNLOCK()      GL1_ISR_UNLOCK()

#define GL1_TRM_LOCK()              rex_enter_crit_sect(&gl1_trm_crit_sec);
#define GL1_TRM_UNLOCK()            rex_leave_crit_sect(&gl1_trm_crit_sec);

/* Drivers */
#define GDRV_ISR_LOCK()            GL1_ISR_LOCK()
#define GDRV_ISR_UNLOCK()          GL1_ISR_UNLOCK()

#define GDRV_ISR_SAVE_LOCK()        GL1_ISR_SAVE_LOCK()
#define GDRV_ISR_SAVE_UNLOCK()      GL1_ISR_SAVE_UNLOCK()

#define GL1_MULTI_SIM_MODE_LOCK()   rex_enter_crit_sect(&gl1_multi_sim_mode_crit_sec)
#define GL1_MULTI_SIM_MODE_UNLOCK() rex_leave_crit_sect(&gl1_multi_sim_mode_crit_sec)

#define GDRV_RFA_MSGR_LOCK()       rex_enter_crit_sect(&gdrivers_rfa_msgr_crit_sec)
#define GDRV_RFA_MSGR_UNLOCK()     rex_leave_crit_sect(&gdrivers_rfa_msgr_crit_sec)

#define GDRV_CIPHER_LOCK()       rex_enter_crit_sect(&gdrivers_cipher_crit_sec)
#define GDRV_CIPHER_UNLOCK()     rex_leave_crit_sect(&gdrivers_cipher_crit_sec)


/* services\mdsp */
/* As these are outside GERAN at present, keep the definitions in
** services\mdsp. 
*/ 

/* GSNDCP */
#define GSN_LOCK()                        rex_enter_crit_sect(&gsn_crit_sec)
#define GSN_UNLOCK()                      rex_leave_crit_sect(&gsn_crit_sec)

/* GLLC */
#define GLLC_LOCK()                 rex_enter_crit_sect(&gllc_crit_sec)
#define GLLC_UNLOCK()               rex_leave_crit_sect(&gllc_crit_sec)

#define GLLC_UL_MSG_LOCK()          rex_enter_crit_sect(&gllc_ul_msg_crit_sec)
#define GLLC_UL_MSG_UNLOCK()        rex_leave_crit_sect(&gllc_ul_msg_crit_sec)

#define GLLC_DL_MSG_LOCK()          rex_enter_crit_sect(&gllc_dl_msg_crit_sec)
#define GLLC_DL_MSG_UNLOCK()        rex_leave_crit_sect(&gllc_dl_msg_crit_sec)

#define GLLC_CIPHER_LOCK()                rex_enter_crit_sect(&gllc_cipher_crit_sec)
#define GLLC_CIPHER_UNLOCK()              rex_leave_crit_sect(&gllc_cipher_crit_sec)

/* GRLC */
#define GRLC_DL_MSG_LOCK()          rex_enter_crit_sect(&grlc_dl_msg_crit_sec)
#define GRLC_DL_MSG_UNLOCK()        rex_leave_crit_sect(&grlc_dl_msg_crit_sec)

#define GRLC_DL_FIFO_LOCK()         rex_enter_crit_sect(&grlc_dl_fifo_crit_sec)
#define GRLC_DL_FIFO_UNLOCK()       rex_leave_crit_sect(&grlc_dl_fifo_crit_sec)

#define GRLC_UL_MSG_LOCK()          rex_enter_crit_sect(&grlc_ul_msg_crit_sec)
#define GRLC_UL_MSG_UNLOCK()        rex_leave_crit_sect(&grlc_ul_msg_crit_sec)

#define GRLC_UL_NO_DATA_LOCK()      rex_enter_crit_sect(&grlc_ul_no_data_crit_sec)
#define GRLC_UL_NO_DATA_UNLOCK()    rex_leave_crit_sect(&grlc_ul_no_data_crit_sec)

#define GRLC_UL_TX_VARS_LOCK()      rex_enter_crit_sect(&grlc_ul_tx_vars_crit_sec)
#define GRLC_UL_TX_VARS_UNLOCK()    rex_leave_crit_sect(&grlc_ul_tx_vars_crit_sec)

#define GRLC_UL_FIFO_LOCK()         rex_enter_crit_sect(&grlc_ul_fifo_crit_sec)
#define GRLC_UL_FIFO_UNLOCK()       rex_leave_crit_sect(&grlc_ul_fifo_crit_sec)

#define GRLC_UL_FIFO_SAVE_LOCK()    rex_enter_crit_sect(&grlc_ul_fifo_crit_sec)
#define GRLC_UL_FIFO_SAVE_UNLOCK()  rex_leave_crit_sect(&grlc_ul_fifo_crit_sec)

#define GRLC_UL_PROCEDURE_LOCK()    rex_enter_crit_sect(&grlc_ul_procedure_crit_sec)
#define GRLC_UL_PROCEDURE_UNLOCK()  rex_leave_crit_sect(&grlc_ul_procedure_crit_sec)

/* GMAC */
#define GMAC_MSG_LOCK()             rex_enter_crit_sect(&gmac_msg_crit_sec)
#define GMAC_MSG_UNLOCK()           rex_leave_crit_sect(&gmac_msg_crit_sec)

/* GRR */
#define GRR_GENERAL_LOCK()          rex_enter_crit_sect(&grr_general_crit_sec)
#define GRR_GENERAL_UNLOCK()        rex_leave_crit_sect(&grr_general_crit_sec)

#define GRR_SYSINFO_LOCK()          rex_enter_crit_sect(&grr_sysinfo_crit_sec)
#define GRR_SYSINFO_UNLOCK()        rex_leave_crit_sect(&grr_sysinfo_crit_sec)

#define GRR_MULTI_BAND_LOCK()       rex_enter_crit_sect(&grr_multi_band_crit_sec)
#define GRR_MULTI_BAND_UNLOCK()     rex_leave_crit_sect(&grr_multi_band_crit_sec)

#define GRR_LOG_LOCK()              rex_enter_crit_sect(&grr_log_crit_sec)
#define GRR_LOG_UNLOCK()            rex_leave_crit_sect(&grr_log_crit_sec)



#endif /* GMUTEX_H */
