
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    W A T C H D O G     T A S K

GENERAL DESCRIPTION
  The Watchdog task monitors the other software tasks to ensure that they
  execute properly and prevents the watchdog timer from resetting the
  microprocessor.

EXTERNALIZED FUNCTIONS
  dog_report
    Report to the Watchdog task, so that it can tell that the monitored
    task is still functioning properly.
  dog_autokick
    This routine enables/disables the hardware watchdog auto-kick function.
  dog_kick
    This routine kicks the hardware watchdog.  This is not to be used except
    in unusual start-up/shutdown conditions.
  dog_task
    This is the entry procedure for the Watchdog task.
  dog_register
    This is for new tasks to request dog monitoring
  dog_get_report_period
    This is how new tasks determine their dog report timer period.
  dog_deregister
    This is how registered tasks inform dog to cease monitoring them. 

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Dog_task must be defined prior to all other tasks which
  are to be monitored, and signaled to start after the last task has started.
  The reason for doing this is to allow the Watchdog task to start a timer
  when the task is initialized.  If the timer expires before the task is
  started, the Watchdog task initiates fatal error processing.

Copyright (c) 2002 - 2016,2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/debugtools/dog/src/dog.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/10   esh     Removed Proc-comm
07/02/09   va      Added TCXO Manager Task
05/26/09   st      Added GFX task
03/12/09   vas     Added entries for IPAUKCC_IPC
03/05/09   am      Added DCC Task.
01/27/09   bdh     Added proc comm function set_arm9_sw_watchdog_state
                   for disable/enable sw watchdog when Apps proc is in debugbreak
                   and continue states
11/21/08   ry      Added IMAGE_APPS_PROC check for FEATURE_SENSORS.
11/10/08   bdh     Added dog task disable function to disable sw watchdog
                   to support WinMob ARM11 debugbreak/continue in PB.
07/29/08   ar      Added QMI task.
07/21/08   atien   Increase GPS's CD/NF task maximum timeout to align the value
                   closer to the next higher priority task
06/27/08   apu	   Added support for FEATURE_VBATT_TASK	
03/21/08   gfr     Wrap PDTCP task with FEATURE_CGPS_PD_COMMS_TCP_ON_MODEM.
03/12/08   gfr     Wrap SND task with FEATURE_SND_TASK.
02/28/08   cr/aar  Added PGI Task.
09/26/07   gfr     Do not monitor HS task on WinMobile yet.
09/13/07   gfr     Support for HS polling on the modem.
08/08/07   ah      Replaced defined(FEATURE_CGPS_PD_COMMS_TCP_ON_MODEM) with
                   !defined(FEATURE_DATA_ON_APPS).
06/21/07   gfr     Support for QVP RTP task.
06/18/07   ry      Added support for FEATURE_SENSORS
05/10/07   gfr     Support for WLAN
04/27/07   gfr     HSU updates.
04/19/07   gfr     Add missing tramp.h header file.
04/12/07   gfr     Added support for QTV_STATIC_DLAGENT_TASK
04/09/07   ptm     Removed bio_led.h.
03/23/07   gfr     Disable dog monitoring of BTPF task when appropriate.
11/28/06   gfr     Disable dog monitoring of GPS if FEATURE_CGPS is defined.
11/22/06   gfr     Add FEATURE_DISABLE_CGPS
11/01/06   pc      Added support for FEATURE_HS_USB
10/03/06   pc      Added support for QTV_TASK10
08/29/06   pc      Added support for FEATURE_EXT_USB_P1
08/20/06   taw     Process STOP sig first in dog task. Stop the dog_keepalive
                   state machine when the STOP signal is received.
08/14/06   pc      Added feature FEATURE_QTV_STATIC_TASKS.
08/09/06   pc      Added support for QTV_VIDEO_RENDERER and QTV_AUDIO
07/25/06   pc      Added support for FEATURE_CPU_BASED_FLOW_CONTROL
07/17/06   pc      Added support for FEATURE_DMTASK.
07/17/06   sb      Corrected the FEATUREes for the USB
06/29/06   giri    Core services build support. Feature defined dog_autokick and
                   dog_kick for core services (none of the cdma features are 
                   defined).
06/19/06   pc      Added support for FEATURE_SEC_USFS.
06/15/06   dsn     Move BT task to Apps processor for multiprocessor targets.
06/12/06   taw     Added support for watchdog bark interrupt, under 
                   FEATURE_DOG_BARK.
05/19/06   pp      Add dog_autokick() support for 7200.
05/05/06   ac      Added support for FEATURE_QDJTEST
04/24/06   bt      Added support for UBM L1 task.
03/24/06   ptm     Included bio_led.h for 7K.
03/27/06   ddh     Modified dog_autokick to FREEZE the WDOG for 7500.
03/24/06   pc      Added support for SECRND task
03/23/06   cl      Added PDSM_COMM_TASK
03/23/06   xz      Fixed the dog timeout issue for QVP in 7500 branch
01/23/06   rlp     Added HID Support.
01/02/06   dle     Added FEATURE_CAMERA_DRV_TASK.
12/22/05   ~SN     Added feature DOG KEEPALIVE
12/10/05   pc      Introduced changes under FEATURE_L4_OPT_DOG
11/28/05   pc      Doubled the dog time-out period for Sleep task for 6275.
11/10/05   pc      Added support for AVS_SBCENC_TASK
11/08/05   pc      Added support for GSM_PLT task
10/13/05   ck      Increased the size of DOG_MAX_TASK_NAME
09/12/05   pc      Added support for FEATURE_USB_HOST_ENHANCED.
08/26/05   pc      Added support for DISP_TASK.
07/14/05   bhd     Added ATI graphics task support
07/13/05   adm     Merged in changes related to supporting BootLoader compilation
                   for L4 builds
06/12/06   hba     Added support for WLAN Sec task.
06/02/05   pc      Removed inclusion of dmod.h
05/28/05   grl     Added support for run-time sw watchdog disabling.
05/26/05   hcg     Extended timeouts for lower priorty tasks for FEATURE_L4 build.
04/29/05   ibm     Added support for heap tracker.
04/26/05   sr      Added a delay of 120us between kicking of watchdog and write
                   to autokick register for MSM6275
04/21/05   pc      Added support for FEATURE_MFLO
04/09/05   pc      Added support for CNV task
04/12/05   rp      Added support for MSM6800
03/15/05   wal     Added APS task for L4/Linux under FEATURE_APS
03/15/05   adg     Added QDIAG task for L4/Linux under FEATURE_IG_QDIAG
03/15/05   jqi     Added support for FS_FEATURE_RMTEFS_SERVER, FEATURE_L4
                   FEATURE_PM_SERVER,FEATURE_IG_UI_CLIENT_SELECT
02/29/05   jqi      Lint fix.
02/24/05 t_jarmis  Added FEATURE_FM for FM-RDS Radio.
           jqi     fixed compilation error.
02/22/05   jqi     Lint fix.
02/10/05   sr      Featurized correctly for WPLT
02/05/05   aku     Added support for WLAN tasks.
02/04/05   dwp     Added QVP support for SIP.
02/03/05   epr     Increase the batery manager timeout as it is a low priority task
02/03/05   epr     Addition of Sirius specific tasks using define SIRIUS_PLATFORM
01/31/05   jqi     Revised FEATURE_MGPCC_TASK report ID
                   Mainline MSMHW_GSM_BONDING for 6500 target.
01/21/05   ld/sr   Added support MGPCC task (needed for GPS). Added support for
                   MSM6275
01/12/05   jqi     Lint fixes.
01/06/05   aar     Added support for GPSSRCH demod task.
01/05/05   pa      Increased PBM watchdog timeout.
12/13/04   tbg     Merged in changes for FEATURE_SOFTWARE_PROFILE_CPU_STARV_DET
11/19/04   jz      Added QVP VFE Task Dog
11/19/04   jqi     Fixed compilation warnings.
11/18/04   pa      Added support for PBM task.
10/22/04   jqi     Featurization fixes.
10/15/04   rp      Added support for page and interupt tasks.
           jqi     Increased CB dog time out counter.
10/11/04   ck      Turned off autokicker for dog_autokick(FALSE) also
10/08/04   jqi     Added support for FEATURE_HSDPA.
09/29/04   ss      Added support for DTV (Digital TV)
09/28/04   jqi     Fixed ADS1.2 and RVCT2.1 compilation error.
07/06/04    hl     Added support for FEATURE_JOYST_TASK
08/10/04   sr      RF API called to turn off PA when dog_init fails needs to
                   assume that RF Multimode is not initialised and also needs
                   to ensure that MDSP clocks are turned on and the correct
                   MDSP image is loaded. rf_disable_wcdma_tx is replaced with
                   rf_turn_off_pa in all places where dog does an err_Fatal.
06/04/04   bt      Changed 50us wait before starting autokick to 120us.
05/21/04   vr      Added BCDB and HDRBC tasks.
           mykevin Added FEATURE_UI_CORE_REMOVED.
04/05/04   sr      Made writes to SLEEP_CTL register - to kick hw watchdog -
                   atomic by enclosing within INTLOCK/INTFREE
04/01/04   sg      removed support for LP task as task no longer exists.
02/23/04   balas   Added support for Bluetooth External Profiles feature support .
02/19/04   Jayanth Added support for FEATURE_SEC_DRM.
02/13/04   jqi     Back out some changes related to turn off PA.
02/12/04   vdrapk  Added support for FEATURE_GSM_CB.
02/10/04   jqi     Call rf PAI to turn off PA when there is a dog startup
                   timeout for UMTS target.Unified enc API.
02/06/04   jqi     Removed turn off PA when there is a dog startup timeout
                   for UMTS target.
12/05/03    sr     Lock interrupts when writing to NV.
11/07/03    dd     Added support for GPS Search task.
10/03/03   ajn     Removed MSM5500's HDR autokicker & related code
10/13/03   dwp     Added initial QVPHONE support.
10/13/03   jqi     Fixed compile warning.
09/08/03    sr     Added feature T_SLEEP in addition to FEATURE_DOG_IGNORE_SLEEP_TASK
08/18/03    gr     Fixed a bug in the asynchronous calls to the filesystem
                   functions.
08/14/03    sr     Switch FS task priority upon err_fatal to one below DOG task
08/11/03    sr     WCDMA sleep timer when doing cell reselection causes
                   DOG Monitor to expire. Added more checks to catch this
                   condition.
07/23/03    sr     Disable GPTimer when dog times out on a task.
07/23/03    bs     Merged ThinUI changes from 6200 archive. This is featurized
                   as FEATURE_THIN_UI.
07/21/03   mk/drl  Added dog_gsm_autokick.
07/11/03    sr     Added feature FEATURE_DOG_IGNORE_SLEEP_TASK to ignore if
                   Sleep task does not pet the watchdog.
07/11/03    gr     Made the filesystem calls that are part of
                   FEATURE_DOG_FATAL_NV_REPORT asynchronous to avoid the
                   possibility of a timeout causing a hardware watchdog reset.
07/08/03   drl/mk  Added 6300 sleep support
07/08/03    sr     Added ability to monitor by way of an interrupt whether dog
                   task is getting time to run or not. If not then err_fatal
                   is called. FEATURE_MONITOR_DOG needs to be used. Currently
                   to be used only on MSM6200
06/20/03   bgc     Added autokicker for GSM to dog_autokick().
06/17/03    aw     Increased UI dog timeouts to 40secs form 20secs.
05/29/03   Om      Added support for ARM_CRYPTO and IPSEC tasks,
05/28/03   TMR/dgy Added changes for WCDMA single-mode build.
05/14/03    ld     Added support for MGP related tasks
04/30/03    sr     Added feature FEATURE_DOG_FATAL_NV_REPORT to write out the
                   dog task information to the flash when a task times out.
04/18/03    sr     Removed inclusion of ftm.h and also reference to T_RF_ZRF6200
                   as the differentiating condition to include 6200 specific
                   code.
03/31/03   jqi     Increased UI, CM, WMS dog timeouts to 20secs from 10seconds.
03/24/03   jqi     Increased GSTK dog time out.
03/18/03   jqi     Reverse back TMC MMOC entry, added feature around UI and
                   HS task in dog_task_info.
03/14/03   jqi     Added GSM/WCDMA feature around TMC task.
03/12/03   djd     Added support for GSM watchdog.
03/07/03    gr     Added support for changing the initial startup time allowed
                   before the dog becomes active. This is a way to handle
                   situations with long initialization times (ex: EFS startup
                   on an empty filesystem).
03/04/03    lz     Removed FEATURE_MOBILE_SUITE.
02/21/03   jay     Added support for SSL task and virtual SAT task.
02/18/03   jqi/jar Added Support for the Generic SIM Toolkit Task (FEATURE_GSTK)
02/14/03   jqi     Added support for FTM task.
02/06/03   jqi     FTM support with all tasks start for UMTS target.
02/04/03   jqi     Fixed WCDMA call drops by merging dog_autokick().
01/15/03   jqi     Merged with msmshared/umts/services/task/dog.c#48.
                   1. do not check dog_auto_kicking for 6200 target in dog_kick().
                   2. added a temporary support, do not check task timeout
                   for UMTS ftm unless we can start all tasks in ftm mode.
12/31/02   jqi     Merged with msmshared/umts/services/task/dog.c#47
                   1.Added support for FEATURE_ID_FATAL_TASK.
                   2.Added support for 6200 target.
12/10/02    lz     Added dog_task_info[] entry for Mobile Suite task under
                   FEATURE_MOBILE_SUITE.
12/04/02   lam     Added support for HWTC.
11/19/02   jqi     Remove FTM support for GSM feature.
10/25/02   jqi     Featurized HWIO_OUTM
10/24/02   jqi     Clean up dog_kick and dog_autokick to support various
                   Targets.
10/18/02   jqi     Added FTM support for GSM feature.
09/23/02   jqi     Change the misspelt feature name FEATURE_WCDMA.
09/19/02   ck      Changed the misspelt feature name FEATURE_CDAM_1900
09/18/02   jqi     Moved to MSMSAHRED/SERVICES/TASK directory.
09/16/02   jqi     Merged COMMON with MSMSAHRED/UMTS archive.

===========================================================================*/


 

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "stdio.h"
#include <stringl/stringl.h>
#include "HALdog.h"
#include "target.h"             /* Target specific definitions          */
#include "customer.h"        /* Customer Specific Definitions        */
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "qurt.h"              /* definition of Qurt data types         */
#include "err.h"              /* Error logging macros                 */
#include "assert.h"           /* Definition of the ASSERT macro       */
#include "dog.h"                /* Watchdog typedefs and prototypes     */
#include "tms_utils.h"
#include "busywait.h"
#include "rcevt_qurt.h"
#include "timer.h"



#define DOG_TASK_START_SIG 0x00008000

#define DOG_DUMMY_THREAD_ID 0xFFFFFFFF
#define DOG_STARTUP_TIMER_SIG 0x10000

/* Use 'legacy' defines from clk module */
#define CLK_MS_PER_TICK                                  5
#define DOG_TO_QURT_TIME(t)      ((t) * CLK_MS_PER_TICK)


qurt_mutex_t dog_state_tbl_crit_sect;  /* mutex to protect dog_state_tabl */

#define DOG_ID_MANGLER          0x600df1d0
#define DOG_ID_MANGLER_MASK     0x600d0000

//This ratio describes the desired timeout:report period ratio
#define DOG_REPORT_RATIO		8
//Divisor is used in dog_get_report_period to return proper
//get report period ratio
#define DOG_REPORT_DIVISOR      (DOG_REPORT_RATIO * CLK_MS_PER_TICK)

#define FAST_REPORT             (DOG_TO_QURT_TIME(     97 )) 
#define NORM_REPORT             (DOG_TO_QURT_TIME(   1009 ))

/* MUTEX OPERATION */
// TODO: Disbling for PSM requirement testing
#if 0
static qurt_mutex_t dog_mutex_obj;
#define DOG_LOCK_INIT() qurt_mutex_init(&dog_mutex_obj)
#define DOG_LOCK()  qurt_mutex_lock(&dog_mutex_obj)
#define DOG_FREE()  qurt_mutex_unlock(&dog_mutex_obj)
#endif

static unsigned long dog_int_flag = 0;
#define DOG_LOCK_INIT()
#define DOG_LOCK()  INTLOCK_SAVE(dog_int_flag)
#define DOG_FREE()  INTFREE_RESTORE(dog_int_flag)





extern boolean dog_intentional_timeout_flag;

/* Signal object used for the wait procedures */
qurt_signal_t dog_sig_obj;


/******************************************************************
|\     /|(  ___  )(  ____ )( (    /|\__   __/( (    /|(  ____ \( )
| )   ( || (   ) || (    )||  \  ( |   ) (   |  \  ( || (    \/| |
| | _ | || (___) || (____)||   \ | |   | |   |   \ | || |      | |
| |( )| ||  ___  ||     __)| (\ \) |   | |   | (\ \) || | ____ | |
| || || || (   ) || (\ (   | | \   |   | |   | | \   || | \_  )(_)
| () () || )   ( || ) \ \__| )  \  |___) (___| )  \  || (___) | _ 
(_______)|/     \||/   \__/|/    )_)\_______/|/    )_)(_______)(_)

LEGACY DOG IS DEPRECATED.  DO NOT EDIT. ALL CHANGES WILL BE REJECTED
******************************************************************/

/* ===============================================*/
/* BEGIN: LEGACY DOG CHANGES                      */
/* ===============================================*/

dog_report_type  dog_tx_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_rx_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_srch_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_rxtx_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_mc_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_hs_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_diag_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_ui_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_nv_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_auth_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_ds_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_ps_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_tcptest_rpt                = 0; /* phoney rpt_val */
dog_report_type  dog_sleep_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_qmip_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_dh_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_graph_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_fs_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_cm_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_pdsm_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_pdsmsclient_rpt            = 0; /* phoney rpt_val */
dog_report_type  dog_cd_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_nf_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_pb_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_usbdc_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_sfat_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_bt_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_tdso_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_sec_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_uim_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_hdrmc_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_hdrrx_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_hdrtx_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_hdrsrch_rpt                = 0; /* phoney rpt_val */
dog_report_type  dog_hwtc_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_wms_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_tmc_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_mmoc_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_gsdi_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_mdsp_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_gcsd_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_gsm_l1_rpt                 = 0; /* phoney rpt_val */
dog_report_type  dog_gsm_l2_rpt                 = 0; /* phoney rpt_val */
dog_report_type  dog_gllc_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_grlc_ul_rpt                = 0; /* phoney rpt_val */
dog_report_type  dog_grlc_dl_rpt                = 0; /* phoney rpt_val */
dog_report_type  dog_gmac_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_l1_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_l2dl_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_l2ul_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_lsm_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_mn_cnm_rpt                 = 0; /* phoney rpt_val */
dog_report_type  dog_mm_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_plt_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_test_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_reg_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_rr_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_rrc_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_sm_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_tc_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_gpssrch_demod_rpt          = 0; /* phoney rpt_val */
dog_report_type  dog_ftm_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_gstk_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_virtsat_rpt                = 0; /* phoney rpt_val */
dog_report_type  dog_secssl_rpt                 = 0; /* phoney rpt_val */
dog_report_type  dog_mgpnf_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_mgpcd_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_mgpai_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_interrupt_rpt              = 0; /* phoney rpt_val */
dog_report_type  dog_seccryptarm_rpt            = 0; /* phoney rpt_val */
dog_report_type  dog_secips_rpt                 = 0; /* phoney rpt_val */
dog_report_type  dog_qvp_ae_rpt                 = 0; /* phoney rpt_val */
dog_report_type  dog_qvpio_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_qvppl_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_qvp_app_rpt                = 0; /* phoney rpt_val */
dog_report_type  dog_qvp_mpeg4_rpt              = 0; /* phoney rpt_val */
dog_report_type  dog_gpssrch_rpt                = 0; /* phoney rpt_val */
dog_report_type  dog_cb_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_secdrm_rpt                 = 0; /* phoney rpt_val */
dog_report_type  dog_bt_pf_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_hdrbc_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_bcdb_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_joyst_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_dtv_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_mac_hs_dl_rpt              = 0; /* phoney rpt_val */
dog_report_type  dog_pbm_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_qvp_vfe_rpt                = 0; /* phoney rpt_val */
dog_report_type  dog_mgpcc_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_irda_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_sirius_test_rpt            = 0; /* phoney rpt_val */
dog_report_type  dog_aam_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_bm_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_sipt_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_qvp_rtp_rpt                = 0; /* phoney rpt_val */
dog_report_type  dog_wlan_cp_rpt                = 0; /* phoney rpt_val */
dog_report_type  dog_wlan_trp_rpt               = 0; /* phoney rpt_val */
dog_report_type  dog_fmrds_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_qdiag_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_aps_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_efs_srvr_rpt               = 0; /* phoney rpt_val */
dog_report_type  dog_pms_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_cnv_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_qipcall_rpt                = 0; /* phoney rpt_val */
dog_report_type  dog_mfmc_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_mfpt_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_mfrxd_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_wlan_sec_rpt               = 0; /* phoney rpt_val */
dog_report_type  dog_disp_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_usbhc_urb_rpt              = 0; /* phoney rpt_val */
dog_report_type  dog_usbhc_dev_rpt              = 0; /* phoney rpt_val */
dog_report_type  dog_usbhc_test_rpt             = 0; /* phoney rpt_val */
dog_report_type  dog_sbcenc_rpt                 = 0; /* phoney rpt_val */
dog_report_type  dog_camera_drv_rpt             = 0; /* phoney rpt_val */
dog_report_type  dog_graphics_rpt               = 0; /* phoney rpt_val */
dog_report_type  dog_ixhttpsvc_rpt              = 0; /* phoney rpt_val */
dog_report_type  dog_ats_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_comp_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_pdsm_comm_rpt              = 0; /* phoney rpt_val */
dog_report_type  dog_secrnd_rpt                 = 0; /* phoney rpt_val */
dog_report_type  dog_ubml1_rpt                 = 0; /* phoney rpt_val */
dog_report_type  dog_qdjtest_rpt                = 0; /* phoney rpt_val */
dog_report_type  dog_sfs_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_dmtask_rpt                 = 0; /* phoney rpt_val */
dog_report_type  dog_fc_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_qtv_video_renderer_rpt     = 0; /* phoney rpt_val */
dog_report_type  dog_qtv_audio_rpt              = 0; /* phoney rpt_val */
dog_report_type  dog_usbebi2_rpt                = 0; /* phoney rpt_val */
dog_report_type  dog_usbapi_rpt                 = 0; /* phoney rpt_val */
dog_report_type  dog_usbd_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_qtv_task10_rpt             = 0; /* phoney rpt_val */
dog_report_type  dog_hsu_al_ms_rpt              = 0; /* phoney rpt_val */
dog_report_type  dog_hsu_al_task_rpt            = 0; /* phoney rpt_val */
dog_report_type  dog_mgpmc_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_pp_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_cc_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_gpsfft_rpt                 = 0; /* phoney rpt_val */
dog_report_type  dog_lm_rpt                     = 0; /* phoney rpt_val */
dog_report_type  dog_sm_tm_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_pdcommtcp_rpt              = 0; /* phoney rpt_val */
dog_report_type  dog_pdcommwms_rpt              = 0; /* phoney rpt_val */
dog_report_type  dog_qtv_dlagenttask_rpt        = 0; /* phoney rpt_val */
dog_report_type  dog_sns_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_pgi_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_ukcc_rpt                   = 0; /* phoney rpt_val */
dog_report_type  dog_chg_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_vbatt_rpt                  = 0; /* phoney rpt_val */
dog_report_type  dog_qmi_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_loc_middleware_rpt         = 0; /* phoney rpt_val */
dog_report_type  dog_dcc_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_ipa_main_rpt               = 0; /* phoney rpt_val */
dog_report_type  dog_ipa_schan_rpt              = 0; /* phoney rpt_val */
dog_report_type  dog_ipa_pdchan_rpt             = 0; /* phoney rpt_val */
dog_report_type  dog_ipaukcc_ipc_rpt            = 0; /* phoney rpt_val */
dog_report_type  dog_rtp_manager_rpt            = 0; /* phoney rpt_val */
dog_report_type  dog_ftm_camera_rpt             = 0; /* phoney rpt_val */
dog_report_type  dog_gfx_rpt                    = 0; /* phoney rpt_val */
dog_report_type  dog_tcxomgr_rpt                = 0; /* phoney rpt_val */

/*================================================*/ 
/* BEGIN extern time definitions.... For dog.c */ 
/*================================================*/ 
const uint32  dog_sbcenc_rpt_time               = DOG_TO_QURT_TIME( 463 );   /* time_val */  
const uint32  dog_qvp_ae_rpt_time               = DOG_TO_QURT_TIME( 947 );   /* time_val */  
const uint32  dog_qvpio_rpt_time                = DOG_TO_QURT_TIME( 439 );   /* time_val */  
const uint32  dog_qvppl_rpt_time                = DOG_TO_QURT_TIME( 443 );   /* time_val */  
const uint32  dog_qvp_mpeg4_rpt_time            = DOG_TO_QURT_TIME( 449 );   /* time_val */  
const uint32  dog_qvp_app_rpt_time              = DOG_TO_QURT_TIME( 829 );   /* time_val */  
const uint32  dog_qvp_vfe_rpt_time              = DOG_TO_QURT_TIME( 587 );   /* time_val */  
const uint32  dog_qvp_rtp_rpt_time              = DOG_TO_QURT_TIME( 883 );   /* time_val */  
const uint32  dog_sipt_rpt_time                 = DOG_TO_QURT_TIME( 937 );   /* time_val */  
const uint32  dog_gsdi_rpt_time                 = DOG_TO_QURT_TIME( 349 );   /* time_val */  
const uint32  dog_graphics_rpt_time             = DOG_TO_QURT_TIME( 617 );   /* time_val */  
const uint32  dog_graph_rpt_time                = DOG_TO_QURT_TIME( 163 );   /* time_val */  
const uint32  dog_gfx_rpt_time                  = DOG_TO_QURT_TIME(161);   /* time_val */   
const uint32  dog_camera_drv_rpt_time           = DOG_TO_QURT_TIME( 881 );   /* time_val */  
const uint32  dog_hdrmc_rpt_time                = DOG_TO_QURT_TIME( 227 );   /* time_val */  
const uint32  dog_hdrrx_rpt_time                = DOG_TO_QURT_TIME( 229 );   /* time_val */  
const uint32  dog_hdrtx_rpt_time                = DOG_TO_QURT_TIME( 233 );   /* time_val */  
const uint32  dog_hdrsrch_rpt_time              = DOG_TO_QURT_TIME( 239 );   /* time_val */  
const uint32  dog_mmoc_rpt_time                 = DOG_TO_QURT_TIME( 241 );   /* time_val */  
const uint32  dog_tmc_rpt_time                  = DOG_TO_QURT_TIME( 251 );   /* time_val */  
const uint32  dog_uim_rpt_time                  = DOG_TO_QURT_TIME( 223 );   /* time_val */  
const uint32  dog_sec_rpt_time                  = DOG_TO_QURT_TIME( 919 );   /* time_val */  
const uint32  dog_secrnd_rpt_time               = DOG_TO_QURT_TIME( 857 );   /* time_val */  
const uint32  dog_sfs_rpt_time                  = DOG_TO_QURT_TIME( 797 );   /* time_val */  
const uint32  dog_seccryptarm_rpt_time          = DOG_TO_QURT_TIME( 977 );   /* time_val */  
const uint32  dog_secips_rpt_time               = DOG_TO_QURT_TIME( 491 );   /* time_val */  
const uint32  dog_secssl_rpt_time               = DOG_TO_QURT_TIME( 643 );   /* time_val */  
const uint32  dog_secdrm_rpt_time               = DOG_TO_QURT_TIME( 761 );   /* time_val */  
const uint32  dog_sfat_rpt_time                 = DOG_TO_QURT_TIME( 211 );   /* time_val */  
const uint32  dog_bt_rpt_time                   = DOG_TO_QURT_TIME( 199 );   /* time_val */  
const uint32  dog_bt_pf_rpt_time                = DOG_TO_QURT_TIME( 457 );   /* time_val */  
const uint32  dog_cm_rpt_time                   = DOG_TO_QURT_TIME( 997 );   /* time_val */  //HDTEMPO was 181 
const uint32  dog_wms_rpt_time                  = DOG_TO_QURT_TIME( 991 );   /* time_val */  
const uint32  dog_usbdc_rpt_time                = DOG_TO_QURT_TIME( 193 );   /* time_val */  
const uint32  dog_usbhc_urb_rpt_time            = DOG_TO_QURT_TIME( 613 );   /* time_val */  
const uint32  dog_usbhc_dev_rpt_time            = DOG_TO_QURT_TIME( 877 );   /* time_val */  
const uint32  dog_usbhc_test_rpt_time           = DOG_TO_QURT_TIME( 863 );   /* time_val */  
const uint32  dog_hwtc_rpt_time                 = DOG_TO_QURT_TIME( 181 );   /* time_val */  
const uint32  dog_pdsm_rpt_time                 = DOG_TO_QURT_TIME( 809 );   /* time_val */  
const uint32  dog_pdsmsclient_rpt_time          = DOG_TO_QURT_TIME( 811 );   /* time_val */  
const uint32  dog_cd_rpt_time                   = DOG_TO_QURT_TIME( 823 );   /* time_val */  
const uint32  dog_nf_rpt_time                   = DOG_TO_QURT_TIME( 821 );   /* time_val */  
const uint32  dog_pb_rpt_time                   = DOG_TO_QURT_TIME( 827 );   /* time_val */  
const uint32  dog_gpssrch_rpt_time              = DOG_TO_QURT_TIME(661);   /* time_val */  
const uint32  dog_gpssrch_demod_rpt_time        = DOG_TO_QURT_TIME(691);   /* time_val */  
const uint32  dog_pdsm_comm_rpt_time            = DOG_TO_QURT_TIME(853);   /* time_val */  
const uint32  dog_fs_rpt_time                   = DOG_TO_QURT_TIME( 191 );   /* time_val */  
const uint32  dog_tcptest_rpt_time              = DOG_TO_QURT_TIME( 929 );   /* time_val */  
const uint32  dog_dh_rpt_time                   = DOG_TO_QURT_TIME( 179 );   /* time_val */  
const uint32  dog_qmip_rpt_time                 = DOG_TO_QURT_TIME( 167 );   /* time_val */  
const uint32  dog_pd_rpt_time                   = DOG_TO_QURT_TIME( 157 );   /* time_val */  
const uint32  dog_ps_rpt_time                   = DOG_TO_QURT_TIME( 151 );   /* time_val */  
const uint32  dog_dcc_rpt_time                  = DOG_TO_QURT_TIME( 1129 );   /* time_val */  
const uint32  dog_wlan_cp_rpt_time              = DOG_TO_QURT_TIME( 907 );   /* time_val */  
const uint32  dog_wlan_trp_rpt_time             = DOG_TO_QURT_TIME( 941);   /* time_val */  
const uint32  dog_wlan_sec_rpt_time             = DOG_TO_QURT_TIME( 953 );   /* time_val */  
const uint32  dog_auth_rpt_time                 = DOG_TO_QURT_TIME( 149 );   /* time_val */  
const uint32  dog_ds_rpt_time                   = DOG_TO_QURT_TIME( 139 );   /* time_val */  
const uint32  dog_comp_rpt_time                 = DOG_TO_QURT_TIME( 659 );   /* time_val */  
const uint32  dog_diag_rpt_time                 = DOG_TO_QURT_TIME( 137 );   /* time_val */  
const uint32  dog_hs_rpt_time                   = DOG_TO_QURT_TIME( 131 );   /* time_val */  
const uint32  dog_mc_rpt_time                   = DOG_TO_QURT_TIME( 127 );   /* time_val */  
const uint32  dog_nv_rpt_time                   = DOG_TO_QURT_TIME( 113 );   /* time_val */  
const uint32  dog_rx_rpt_time                   = DOG_TO_QURT_TIME( 109 );   /* time_val */  
const uint32  dog_rxtx_rpt_time                 = DOG_TO_QURT_TIME( 107 );   /* time_val */  
const uint32  dog_srch_rpt_time                 = DOG_TO_QURT_TIME( 101 );   /* time_val */  
const uint32  dog_tx_rpt_time                   = DOG_TO_QURT_TIME(  97 );   /* time_val */  
const uint32  dog_ui_rpt_time                   = DOG_TO_QURT_TIME( 983 );   /* time_val */   // HDTEMPO was 89 
const uint32  dog_sleep_rpt_time                = DOG_TO_QURT_TIME(  83 );   /* time_val */  
const uint32  dog_tdso_rpt_time                 = DOG_TO_QURT_TIME( 197 );   /* time_val */  
const uint32  dog_mdsp_rpt_time                 = DOG_TO_QURT_TIME( 499 );   /* time_val */  
const uint32  dog_gcsd_rpt_time                 = DOG_TO_QURT_TIME( 257 );   /* time_val */  /* GSM GCSD Task */ 
const uint32  dog_gsm_l1_rpt_time               = DOG_TO_QURT_TIME( 269 );   /* time_val */  
const uint32  dog_gsm_l2_rpt_time               = DOG_TO_QURT_TIME( 271 );   /* time_val */  
const uint32  dog_l1_rpt_time                   = DOG_TO_QURT_TIME( 277 );   /* time_val */  
const uint32  dog_l2dl_rpt_time                 = DOG_TO_QURT_TIME( 283 );   /* time_val */  
const uint32  dog_l2ul_rpt_time                 = DOG_TO_QURT_TIME( 293 );   /* time_val */  
const uint32  dog_lsm_rpt_time                  = DOG_TO_QURT_TIME( 307 );   /* time_val */  
const uint32  dog_mm_rpt_time                   = DOG_TO_QURT_TIME( 311 );   /* time_val */  
const uint32  dog_mn_cnm_rpt_time               = DOG_TO_QURT_TIME( 313 );   /* time_val */  
const uint32  dog_plt_rpt_time                  = DOG_TO_QURT_TIME( 317 );   /* time_val */  
const uint32  dog_reg_rpt_time                  = DOG_TO_QURT_TIME( 331 );   /* time_val */  
const uint32  dog_rr_rpt_time                   = DOG_TO_QURT_TIME( 337 );   /* time_val */  
const uint32  dog_rrc_rpt_time                  = DOG_TO_QURT_TIME( 347 );   /* time_val */  
const uint32  dog_sm_rpt_time                   = DOG_TO_QURT_TIME( 353 );   /* time_val */  
const uint32  dog_tc_rpt_time                   = DOG_TO_QURT_TIME( 359 );   /* time_val */  
const uint32  dog_test_rpt_time                 = DOG_TO_QURT_TIME( 367 );   /* time_val */  /* GSM TEST task */ 
const uint32  dog_gllc_rpt_time                 = DOG_TO_QURT_TIME( 263 );   /* time_val */  
const uint32  dog_grlc_ul_rpt_time              = DOG_TO_QURT_TIME( 373 );   /* time_val */  
const uint32  dog_grlc_dl_rpt_time              = DOG_TO_QURT_TIME( 379 );   /* time_val */  
const uint32  dog_gmac_rpt_time                 = DOG_TO_QURT_TIME( 383 );   /* time_val */  
const uint32  dog_ftm_rpt_time                  = DOG_TO_QURT_TIME( 389 );   /* time_val */  
const uint32  dog_gstk_rpt_time                 = DOG_TO_QURT_TIME( 397 );   /* time_val */  
const uint32  dog_virtsat_rpt_time              = DOG_TO_QURT_TIME( 401 );   /* time_val */  
const uint32  dog_mgpmc_rpt_time                = DOG_TO_QURT_TIME( 409 );   /* time_val */  
const uint32  dog_mgpnf_rpt_time                = DOG_TO_QURT_TIME( 419 );   /* time_val */  
const uint32  dog_mgpcd_rpt_time                = DOG_TO_QURT_TIME( 421 );   /* time_val */  
const uint32  dog_mgpai_rpt_time                = DOG_TO_QURT_TIME( 431 );   /* time_val */  
const uint32  dog_mgpcc_rpt_time                = DOG_TO_QURT_TIME( 461 );   /* time_val */  
const uint32  dog_pp_rpt_time                   = DOG_TO_QURT_TIME( 419 );   /* time_val */  
const uint32  dog_cc_rpt_time                   = DOG_TO_QURT_TIME( 421 );   /* time_val */  
const uint32  dog_pgi_rpt_time                  = DOG_TO_QURT_TIME( 429 );   /* time_val */  
const uint32  dog_lm_rpt_time                   = DOG_TO_QURT_TIME( 701 );   /* time_val */  
const uint32  dog_pdcommtcp_rpt_time            = DOG_TO_QURT_TIME( 709 );   /* time_val */  
const uint32  dog_pdcommwms_rpt_time            = DOG_TO_QURT_TIME( 751 );   /* time_val */  
const uint32  dog_sm_tm_rpt_time                = DOG_TO_QURT_TIME( 719 );   /* time_val */  
const uint32  dog_gpsfft_rpt_time               = DOG_TO_QURT_TIME( 743 );   /* time_val */  
const uint32  dog_loc_middleware_rpt_time       = DOG_TO_QURT_TIME( 800 );   /* time_val */  
const uint32  dog_cb_rpt_time                   = DOG_TO_QURT_TIME( 509 );   /* time_val */  
const uint32  dog_hdrbc_rpt_time                = DOG_TO_QURT_TIME( 503 );   /* time_val */  
const uint32  dog_bcdb_rpt_time                 = DOG_TO_QURT_TIME( 521 );   /* time_val */  
const uint32  dog_dtv_rpt_time                  = DOG_TO_QURT_TIME( 967 );   /* time_val */  
const uint32  dog_joyst_rpt_time                = DOG_TO_QURT_TIME( 971 );   /* time_val */  
const uint32  dog_mac_hs_dl_rpt_time            = DOG_TO_QURT_TIME( 523 );   /* time_val */  
const uint32  dog_interrupt_rpt_time            = DOG_TO_QURT_TIME(  433 );   /* time_val */  
const uint32  dog_pbm_rpt_time                  = DOG_TO_QURT_TIME( 911 );   /* time_val */  
const uint32  dog_irda_rpt_time                 = DOG_TO_QURT_TIME( 541 );   /* time_val */  
const uint32  dog_sirius_test_rpt_time          = DOG_TO_QURT_TIME( 547 );   /* time_val */  
const uint32  dog_aam_rpt_time                  = DOG_TO_QURT_TIME( 557 );   /* time_val */  
const uint32  dog_bm_rpt_time                   = DOG_TO_QURT_TIME( 563 );   /* time_val */  
const uint32  dog_qdiag_rpt_time                = DOG_TO_QURT_TIME( 593 );   /* time_val */  
const uint32  dog_aps_rpt_time                  = DOG_TO_QURT_TIME( 599 );   /* time_val */  
const uint32  dog_ats_rpt_time                  = DOG_TO_QURT_TIME( 733 );   /* time_val */  
const uint32  dog_efs_srvr_rpt_time             = DOG_TO_QURT_TIME( 601 );   /* time_val */  
const uint32  dog_pms_rpt_time                  = DOG_TO_QURT_TIME( 607 );   /* time_val */  
const uint32  dog_cnv_rpt_time                  = DOG_TO_QURT_TIME( 577 );   /* time_val */  
const uint32  dog_dmtask_rpt_time               = DOG_TO_QURT_TIME( 631 );   /* time_val */  
const uint32  dog_qipcall_rpt_time              = DOG_TO_QURT_TIME(571);   /* time_val */  
const uint32  dog_disp_rpt_time                 = DOG_TO_QURT_TIME( 887 );   /* time_val */  
const uint32  dog_mfmc_rpt_time                 = DOG_TO_QURT_TIME( 467 );   /* time_val */  
const uint32  dog_mfpt_rpt_time                 = DOG_TO_QURT_TIME( 479 );   /* time_val */  
const uint32  dog_mfrxd_rpt_time                = DOG_TO_QURT_TIME( 487 );   /* time_val */  
const uint32  dog_fc_rpt_time                   = DOG_TO_QURT_TIME( 727 );   /* time_val */  
const uint32  dog_ixhttpsvc_rpt_time            = DOG_TO_QURT_TIME( 859 );   /* time_val */  
const uint32  dog_ubm_l1_rpt_time               = DOG_TO_QURT_TIME( 739 );   /* time_val */  
const uint32  dog_qdjtest_rpt_time              = DOG_TO_QURT_TIME(619);   /* time_val */  
const uint32  dog_usbebi2_rpt_time              = DOG_TO_QURT_TIME(653);   /* time_val */  
const uint32  dog_usbapi_rpt_time               = DOG_TO_QURT_TIME(673);   /* time_val */  
const uint32  dog_usbd_rpt_time                 = DOG_TO_QURT_TIME(677);   /* time_val */  
const uint32  dog_qtv_video_renderer_rpt_time   = DOG_TO_QURT_TIME(641);   /* time_val */  
const uint32  dog_qtv_audio_rpt_time            = DOG_TO_QURT_TIME(647);   /* time_val */  
const uint32  dog_qtv_task10_rpt_time           = DOG_TO_QURT_TIME(683);   /* time_val */  
const uint32  dog_tcxomgr_rpt_time              = DOG_TO_QURT_TIME(647);   /* time_val */  
const uint32  dog_hsu_al_ms_rpt_time            = DOG_TO_QURT_TIME( 773 );   /* time_val */  
const uint32  dog_hsu_al_task_rpt_time          = DOG_TO_QURT_TIME( 787 );   /* time_val */  
const uint32  dog_qtv_dlagenttask_rpt_time      = DOG_TO_QURT_TIME(757);   /* time_val */  
const uint32  dog_sns_rpt_time                  = DOG_TO_QURT_TIME( 769 );   /* time_val */  
const uint32  dog_ukcc_rpt_time                 = DOG_TO_QURT_TIME( 1009 );   /* time_val */  
const uint32  dog_ipa_main_rpt_time             = DOG_TO_QURT_TIME( 1061 );   /* time_val */  
const uint32  dog_ipa_schan_rpt_time            = DOG_TO_QURT_TIME( 1063 );   /* time_val */  
const uint32  dog_ipa_pdchan_rpt_time           = DOG_TO_QURT_TIME( 1069 );   /* time_val */  
const uint32  dog_ipaukcc_ipc_rpt_time          = DOG_TO_QURT_TIME( 1163 );   /* time_val */  
const uint32  dog_rtp_manager_rpt_time          = DOG_TO_QURT_TIME(1151);   /* time_val */   
const uint32  dog_chg_rpt_time                  = DOG_TO_QURT_TIME( 500 );   /* time_val */  
const uint32  dog_vbatt_rpt_time                = DOG_TO_QURT_TIME( 997 );   /* time_val */  
const uint32  dog_ftm_camera_rpt_time           = DOG_TO_QURT_TIME( 703 );   /* time_val */  

/******************************************************************
|\     /|(  ___  )(  ____ )( (    /|\__   __/( (    /|(  ____ \( )
| )   ( || (   ) || (    )||  \  ( |   ) (   |  \  ( || (    \/| |
| | _ | || (___) || (____)||   \ | |   | |   |   \ | || |      | |
| |( )| ||  ___  ||     __)| (\ \) |   | |   | (\ \) || | ____ | |
| || || || (   ) || (\ (   | | \   |   | |   | | \   || | \_  )(_)
| () () || )   ( || ) \ \__| )  \  |___) (___| )  \  || (___) | _ 
(_______)|/     \||/   \__/|/    )_)\_______/|/    )_)(_______)(_)

LEGACY DOG IS DEPRECATED.  DO NOT EDIT. ALL CHANGES WILL BE REJECTED
******************************************************************/

/* ===============================================*/
/* END: LEGACY DOG CHANGES                        */
/* ===============================================*/

/* Enable this for bringup or to help debug multiple dog timeout errors */
// #define DOG_BRINGUP  

#ifdef FEATURE_PLT
  #define NO_DOG
#endif

/* This specifies, in milliseconds, the maximum length of the grace period
** the Watchdog Task allows during system initialization and startup.
*/
#define DOG_MAX_STARTUP_TIME    800000

/* This specifies, in milliseconds, the default length of the grace period
** the Watchdog Task allows during system initialization and startup.
*/
#define DOG_DEFAULT_STARTUP_TIME    40000


/* Number of milliseconds between each wakeup of the Watchdog Task.
** This value is dependent on how often we must reset the watchdog
** timer circuit.  It has nothing to do with how often tasks report.
*/
#define DOG_WAKEUP_TIME         100

/* Number of milliseconds until halt timer expires, and watchdog
** task receives the signal to halt the modem. This ensures sync
** between apps proc and modem proc, so that when apps proc is in
** broken state, modem proc does not error fatal.
*/
#define DOG_HALT_TIME           40000

/* Number of milliseconds between each check of the all the task counters.
** Should be an integer multiple of DOG_WAKEUP_TIME.
*/
#define DOG_CHECK_TIME          1000

#ifdef FEATURE_DOG_BARK
/* Number of sleep clocks before asserting the Watchdog "bark" 
** interrupt. The value can be between 0 and 0x31F3 (12787) (the 
** time when the watchdog times out). If programmed to be greater
** than 0x31F3 (default value is 0x3FFF), the "bite" will fire 
** first, effectively disabling the "bark".
*/
#define DOG_BARK_ENABLE_TIMEOUT_SLCK    0x3000  /* 12288 slcks */

/* If set to this, disables the "bark" interrupt. */
#define DOG_BARK_DISABLE_TIMEOUT_SLCK   0x3FFF  /* 16383 slcks */
#endif /* FEATURE_DOG_BARK */

/* Number of DOG_WAKEUP_TIME intervals between each check of the task
** counters.
*/
#define CHECK_CNT               (DOG_CHECK_TIME / DOG_WAKEUP_TIME)

/* Convert time in milliseconds to appropriate number of counter ticks
*/
#define DOG_MS_TO_COUNTS(t)     (( t  + DOG_CHECK_TIME/2) / DOG_CHECK_TIME )
#define DOG_COUNTS_TO_MS(t)     ( t * DOG_CHECK_TIME )

/* Maximum length of a task name in the dog_state_table array.
*/
#define DOG_MAX_TASK_NAME DOG_QURT_TASK_NAME

#ifdef FEATURE_DOG_FATAL_NV_REPORT
  /* Time in milliseconds that the Dog task will wait for the FS task to
  ** complete an operation.
  */
  #define DOG_FS_TMO 5000
#endif


/* Structure for storing task state info */
typedef struct {
#ifdef DOG_BRINGUP
  uint32                   err_cnt;
  uint32                   rpt_cnt;
#endif
  char                     task[DOG_MAX_TASK_NAME+1];
  uint32                   count;        /* remaining count */
  uint32                   timeout;      /* max count       */
  qurt_thread_t            thread_id;
  boolean                  is_blocked;
  dog_deadlockcheck_type   deadlockcheck_flag;  /* Not yet supported */
} dog_state_table_type;


/*lint ++flb */ /* turn on library flag */
/* Structure for storing task name and timeout interval */

/*lint --flb */

#ifdef FEATURE_MONITOR_DOG
/* 300MS is the dog monitor timeout value. If dog task does not run
   for 300ms then this timer will alert the AMSS code
*/
LOCAL uint32 dog_monitor_timeout = 300;
#endif

/* Timer for waking up and resetting the watchdog circuit
*/
LOCAL timer_type dog_wakeup_timer;
LOCAL timer_type dog_startup_timer;

/* Timer for halting the system upon expiration
*/
LOCAL timer_type dog_halt_timer;

/* Whether or not the watchdog is being auto-kicked by the MSM.
** Avoid kicking while the MSM is auto-kicking, as this causes
** the MSM to stop auto-kicking AND does not kick (MSM 1.0 bug).
*/
LOCAL boolean dog_auto_kicking = FALSE;


/* 
** This refers to the software watchdog.  This variable has no
** impact on the HW watchdog.
*/
static boolean dog_enabled = TRUE;

/*
**  This flag determines whether or not to set the dog startup timer.  Disable via debugger during init
**  Default value is true
**/
boolean dog_enable_startup_timer = TRUE;


/* Timeout times for each task, and a string to associate with each task.
** The string will be dislayed by err_fatal if its task times out.
** Each timeout item contains the total number of counts the associated
** task has to report in. The index corresponding to each task in this
** array must equal the value of the DOG_XXX_RPT constant in dog.h that
** corresponds to the task. For instance, DOG_RX_RPT has the value 2, so
** the entry for the RX task should be at array index 2.
**
** Note that the descriptive string can be at most DOG_MAX_TASK_NAME
** characters long.
*/

/* Support 'legacy' L4 values */
#ifdef FEATURE_L4
  #define CNT_SCALE  2
#else
  #define CNT_SCALE  1
#endif

#define DOG_NUM_TASKS   15


/* Provide additional debugging information */
#ifdef DOG_BRINGUP
uint32  dog_err_bringup_timeouts = 0;
uint32  dog_err_rpt1_cnt         = 0;
uint32  dog_err_rpt2_cnt         = 0;   
uint32  dog_err_pause_zero       = 0;
uint32  dog_err_pause_err        = 0;
uint32  dog_err_resume_zero      = 0;
uint32  dog_err_resume_err       = 0;
uint32  dog_err_pause_arr[DOG_NUM_TASKS];
uint32  dog_err_resume_arr[DOG_NUM_TASKS];
uint32  dog_err_rpt1_arr[DOG_NUM_TASKS];
uint32  dog_err_rpt2_arr[DOG_NUM_TASKS];
#endif /* DOG_BRINGUP */

/* Stores the current state of each task in the system. Indicates the
** number of counts remaining for a task to check in and whether or not
** the task is blocked on call. Initialized in dog_init().
*/
LOCAL dog_state_table_type dog_state_table[ DOG_NUM_TASKS ];



#ifdef FEATURE_SOFTWARE_PROFILE_CPU_STARV_DET
/* lowest pri task scheduled since last poll */
LOCAL dword lo_pri_task_pri;
LOCAL qurt_thread_t lo_pri_task_tid;
#define LO_PRI_TASK_SCHED_HIST_LEN 12
LOCAL dword lowest_pri_tasks_sched_history[LO_PRI_TASK_SCHED_HIST_LEN];
LOCAL byte  lo_pri_task_hist_index = 0;
#endif




/*===========================================================================

FUNCTION DOG_TASK_DISABLE

DESCRIPTION
  This procedure will disable software watchdog feature in watchdog task so
  taks counts will not decrement or expire.  Dog_task will still run but not
  expire.

DEPENDENCIES
  This should only be called when we have received a debugbreak notification
  from ARM11.

RETURN VALUE
  None

SIDE EFFECTS
  This will prevent the software watchdog from decrementing timeouts for sw tasks
  in the sw dog's list.

===========================================================================*/
void dog_task_disable
(
  boolean disable
)
{
  /* set the sotftware dog_enable on/off
  */
    if ( disable ) {

      /* Disable sw dog task tracking
      */
      dog_enabled = FALSE;

    } else {

      /* Enable sw dog task tracking
      */
      dog_enabled = TRUE;
    }

}


/*===========================================================================

FUNCTION DOG_REPORT

DESCRIPTION
  Report to the Watchdog task, so that it can tell that the monitored
  task is still functioning properly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dog_report
(
  dog_report_type report_id
    /* Tell the Watchdog task which task is reporting */
)
{

  dog_report_type index = report_id ^ DOG_ID_MANGLER;

#ifdef DOG_BRINGUP
  /* Catch unmangled ids, but exclude the case where, e.g., DOG_ID_MANGLER = 0 */
  if (( report_id < DOG_NUM_TASKS ) && ( index >= DOG_NUM_TASKS ) ) {
    dog_err_rpt1_cnt++;
    dog_err_rpt1_arr[report_id]++;
  } 
  else if ( index >= DOG_NUM_TASKS ) {
    dog_err_rpt2_cnt++;
  }
#endif

  if ( index < DOG_NUM_TASKS )
  {

    /* Reset counter to maximum */
    dog_state_table[index].count = dog_state_table[index].timeout;
#ifdef DOG_BRINGUP
    dog_state_table[index].rpt_cnt++; /* Record the index for debug */
#endif

  }

} /* end of dog_report */


/*===========================================================================

FUNCTION DOG_SET_AUTOKICKING

DESCRIPTION
  This procedure sets the autokicking state for the dog task.

DEPENDENCIES
  Only call when in an INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  Enables or disables the dog_wakeup_timer, depending on autokick state.

===========================================================================*/
LOCAL void dog_set_autokicking
(
  boolean enable
)
{

  static timetick_type dog_save_timer = DOG_WAKEUP_TIME;

  /* Check if we are switching the dog_auto_kicking state
  */
  if ( dog_auto_kicking != enable ) {

    if ( enable ) {

      /* Now auto-kicking.  Pause Watchdog's wakeup timer
      */
      dog_save_timer = timer_clr( &dog_wakeup_timer, T_MSEC);

    } else {

      /* No longer auto-kicking.  Resume watchdog wakeup timer
      */
      (void) timer_set( &dog_wakeup_timer, dog_save_timer, 0, T_MSEC );

    }

    /* Record the new state
      */
    dog_auto_kicking = enable;
  }


} /* end of dog_set_autokicking */




/*===========================================================================

FUNCTION DOG_AUTOKICK

DESCRIPTION
  This procedure enables/disables the auto-kick feature.  Auto-kick
  is used when the microprocessor is expected to be asleep.

DEPENDENCIES
  This should only be called when the sleep timer has been programmed
  or when the warm-up timer has expired.

RETURN VALUE
  None

SIDE EFFECTS
  When enabled, pauses the watchdog's wakeup timer.  The timer resumes
  when autokick is disabled via this function.

===========================================================================*/
void dog_autokick
(
  boolean enable
)
{
  DOG_LOCK( );

   HAL_dogKick();

  /* Update global auto-kicking flag, and start/stop dog_wakeup_timer
  */
  dog_set_autokicking( enable );

    if ( enable )
    {
		/* Busywait */
		busywait(120);

		/* Kick the watchdog */
        HAL_dogDisable();

    }
    else 
    {
        /* Disable the autokicker */
        HAL_dogEnable();

		/* Busywait */
		busywait(120);

		/* Kick the watchdog */
        HAL_dogKick();
    }

  DOG_FREE( );

} /* end of dog_autokick */





/*===========================================================================

FUNCTION DOG_KICK

DESCRIPTION
  This procedure resets the watchdog timer circuit.  This is
  not to be used except in unusual start-up/shutdown conditions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the circuit so that we have another N milliseconds before
  the circuit will reset the system.  Arms auto-kick.

===========================================================================*/
void dog_kick( void )
{

 /* current L4 targets don't need to lock interrupts to pet the watch dog, since
  it is a sigle-bit register thus no need to update shadow and or mask all the
  non-existing bits. L4 has higher overhead, so any omtimization will help L4.
  */
#ifndef FEATURE_L4
	DOG_LOCK();
#endif

    HAL_dogKick();
	
#ifndef FEATURE_L4
	DOG_FREE();
#endif

} /* end of dog_kick */

/*===========================================================================

FUNCTION DOG_FORCE_KICK

DESCRIPTION
  This procedure resets the watchdog timer circuit.  This function is a wrapper
  for a direct hardware register write, to be used in exceptional situations 
  (err handling, boot, etc)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the circuit so that we have another N milliseconds before
  the circuit will reset the system.

===========================================================================*/
void dog_force_kick( void )
{
    HAL_dogKick();
}

void dog_force_bite( void )
{
    HAL_dogForceBite();
}


/*===========================================================================

FUNCTION DOG_BARK_ISR

DESCRIPTION
   This function is called when the watchdog bark interrupt occurs.  It
   calls ERR_FATAL, in order to restart AMSS.
===========================================================================*/
extern void dog_bark_isr()
{
#ifdef FEATURE_DOG_BARK
  /* The dog bark interrupt has fired, which means bad things are
   * happening, and we need to reset, so ERR_FATAL, and let the reset
   * code handle it. */

  /* Kick the dog, just in case the dog bite is about to fire.  We 
   * would rather just reset AMSS here, instead of allowing the dog
   * bite to reset the entire chip. */
  dog_kick();     

  #if (!defined(NO_DOG) && !defined(DOG_BRINGUP))
  ERR_FATAL( "Dog bark timeout.", 0, 0, 0 );
  #endif 
#endif /* FEATURE_DOG_BARK */
} /* END dog_bark_isr */


#if 0

/*===========================================================================

FUNCTION DOG_REGISTER

DESCRIPTION
   Dynamic method for tasks to register for dog services.
   A replacement for hard coded timeouts in dog.c, _AND_ for CMI compliance.

PARAMETERS
   timeout: Starvation detection threshhold in milliseconds
   deadlockcheck_flag: Check for long blocks on crit sect?
       DOG_DEADLKCHK_ENABLE      - check for deadlocks enabled
       DOG_DEADLKCHK_DISABLE     - check for deadlocks disabled

===========================================================================*/
LOCAL dog_report_type dog_register(
  rex_tcb_type             *p_tcb, 
  uint32                   timeout,
  dog_deadlockcheck_type   deadlkchk
)
{
  uint32  i        = 0;
  boolean found_reg = FALSE; 
  char qurt_task_name[DOG_QURT_TASK_NAME+1];
  int err_code;

  if ( p_tcb == NULL ) {        
    ERR_FATAL( "DOG IntRegister with NULL tcb.", 0, 0, 0 );
  }

  /* Get the thread name for the caller */
  err_code = qurt_thread_get_name(&qurt_task_name[0],sizeof(qurt_task_name));
  if (QURT_EINVALID == err_code)
  {
    ERR_FATAL("Dog IntRegister BAD thread id", 0, 0, 0);
  }  

  /* Although we won't be using a mutex in general for now, we DO want mutex for 
  ** calls to dog_register() 
  */
  qurt_mutex_lock(&dog_state_tbl_crit_sect);

  /* Reregistration is allowed, first check if already registered...
   * If reregistering, we'll update the existing settings.
   */
  while ( i < DOG_NUM_TASKS ) {
    if ( dog_state_table[i].p_tcb == p_tcb ) {
      found_reg = TRUE; 
      break;
    }
    i++;
  } 



  /* If not found, then we start over and look for a free slot. */
  if ( ! found_reg ) { /* i == DOG_NUM_TASKS */
    i = 0;
    while ( ( i < DOG_NUM_TASKS ) && ((dog_state_table[i].p_tcb) != NULL ) ) {
        i++;   
    }
  }
  
  /* Error if table is full. */
  if ( i == DOG_NUM_TASKS ) {                 
    ERR_FATAL ("DOG IntRegister: Table full.",  0, 0, 0 );
  }

  /* Otherwise fill in or update the table entry... */

  strlcpy (dog_state_table[i].task, qurt_task_name, 
                                             sizeof (dog_state_table[i].task));



  dog_state_table[i].p_tcb              = p_tcb;
  dog_state_table[i].is_blocked         = FALSE;
  dog_state_table[i].timeout            = DOG_MS_TO_COUNTS(timeout);
  dog_state_table[i].count              = DOG_MS_TO_COUNTS(timeout);

                              
  dog_state_table[i].deadlockcheck_flag = DOG_DEADLKCHK_UNDEF;  

#ifdef DOG_BRINGUP
  dog_state_table[i].err_cnt        = 0;
  dog_state_table[i].rpt_cnt        = 0;
#endif

  qurt_mutex_unlock(&dog_state_tbl_crit_sect);

  return ( i ^ DOG_ID_MANGLER );
}  /* End of dog_register_internal() */
#endif


/*===========================================================================

FUNCTION DOG_REGISTER

DESCRIPTION
   Dynamic method for tasks to register for dog services.
   A replacement for hard coded timeouts in dog.c, _AND_ for CMI compliance.

PARAMETERS
   timeout: Starvation detection threshhold in milliseconds
   deadlockcheck_flag: Check for long blocks on crit sect?
       DOG_DEADLKCHK_ENABLE      - check for deadlocks enabled
       DOG_DEADLKCHK_DISABLE     - check for deadlocks disabled

===========================================================================*/
LOCAL dog_report_type dog_register_qurt(
  qurt_thread_t            thread_id, 
  uint32                   timeout,
  dog_deadlockcheck_type   deadlkchk
)
{
  uint32  i        = 0;
  boolean found_reg = FALSE; 
  char qurt_task_name[DOG_QURT_TASK_NAME+1];
  int err_code;

  /* Get the thread name for the caller */
  err_code = qurt_thread_get_name(&qurt_task_name[0],sizeof(qurt_task_name));

  if (QURT_EINVALID == err_code)
  {
    ERR_FATAL("Dog IntRegister BAD thread id", 0, 0, 0);
  }
  /* Although we won't be using a mutex in general for now, we DO want mutex for 
  ** calls to dog_register() 
  */
  qurt_mutex_lock(&dog_state_tbl_crit_sect);

  /* Reregistration is allowed, first check if already registered...
   * If reregistering, we'll update the existing settings.
   */
  while ( i < DOG_NUM_TASKS ) {
    if ( dog_state_table[i].thread_id == thread_id ) {
      found_reg = TRUE; 
      break;
    }
    i++;
  } 



  /* If not found, then we start over and look for a free slot. */
  if ( ! found_reg ) { /* i == DOG_NUM_TASKS */
    i = 0;
	while ( ( i < DOG_NUM_TASKS ) && ((dog_state_table[i].thread_id) != DOG_DUMMY_THREAD_ID ) ){
        i++;   
    }
  }
  
  /* Error if table is full. */
  if ( i == DOG_NUM_TASKS ) {                 
    ERR_FATAL ("DOG IntRegister: Table full.",  0, 0, 0 );
  }

  /* Otherwise fill in or update the table entry... */

  strlcpy (dog_state_table[i].task, qurt_task_name, 
                                             sizeof (dog_state_table[i].task));



  dog_state_table[i].thread_id          = thread_id;
  dog_state_table[i].is_blocked         = FALSE;
  dog_state_table[i].timeout            = DOG_MS_TO_COUNTS(timeout);
  dog_state_table[i].count              = DOG_MS_TO_COUNTS(timeout);

                              
  dog_state_table[i].deadlockcheck_flag = DOG_DEADLKCHK_UNDEF;  

#ifdef DOG_BRINGUP
  dog_state_table[i].err_cnt        = 0;
  dog_state_table[i].rpt_cnt        = 0;
#endif

  qurt_mutex_unlock(&dog_state_tbl_crit_sect);

  return ( i ^ DOG_ID_MANGLER );
}  /* End of dog_register_internal() */


/*===========================================================================

FUNCTION DOG_AUTO_REGISTER

DESCRIPTION
   Register EACH legacy task thru dynamic registration system.
   Only to be called at init time. (??)

===========================================================================*/
LOCAL void dog_auto_register(void)
{
  /* Before this is done, the tcb->dog_report_val's will be invalid for legacy tasks.
  ** So we probably want to do this pretty early.
  ** If called AFTER tasks are started, we could get dog_monitor_pause() calls
  ** and not be able to actually handle them, resulting in dog timeouts...
  ** BUT if done too early then the tcb's won't contain valid task names.
  */

  /* Register EACH legacy TASK into new dynamic data structure */

/*================================================*/ 
/*   DOG.C AutoRegister Code                      */ 
/*================================================*/ 
/*================================================*/ 
/* BEGIN dog autoregister code .... For dog.c */ 
/*================================================*/ 


/*================================================*/ 
/* END dog autoregister code .... */ 
/*================================================*/ 

}


/*===========================================================================

FUNCTION DOG_INIT

DESCRIPTION
  This procedure performs the initialization for the Watchdog task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dog_init( void )
{
  word i;               /* index for setting up countdown array */

  /* Initialize Critical Section */
  qurt_mutex_init(&dog_state_tbl_crit_sect);

  /* Initialize the mutex for other operations */
  DOG_LOCK_INIT();


  /* Set up task counts. */
  for (i = 0; i < DOG_NUM_TASKS; i++) {
    strlcpy (dog_state_table[i].task, "                      ", 
                                            sizeof (dog_state_table[i].task));

    dog_state_table[i].count              = 0xffffffff;
    dog_state_table[i].timeout            = 0; /* indicates not monitored */
	dog_state_table[i].thread_id          = DOG_DUMMY_THREAD_ID;
    dog_state_table[i].is_blocked         = FALSE;
    dog_state_table[i].deadlockcheck_flag = DOG_DEADLKCHK_UNDEF;
  
#ifdef DOG_BRINGUP
    dog_state_table[i].rpt_cnt = 0;
    dog_state_table[i].err_cnt = 0;
    dog_err_rpt1_arr[i]        = 0;
    dog_err_rpt2_arr[i]        = 0;
    dog_err_pause_arr[i]       = 0;
    dog_err_resume_arr[i]      = 0;
#endif
  }

#ifdef DOG_BRINGUP
  dog_err_rpt1_cnt         = 0;
  dog_err_rpt2_cnt         = 0;
  dog_err_pause_zero       = 0;
  dog_err_pause_err        = 0;
  dog_err_resume_zero      = 0;
  dog_err_resume_err       = 0;
#endif

} /* end of dog_init */

/*===========================================================================

FUNCTION DOG_INTERNAL_INIT

DESCRIPTION
  This procedure performs the initialization for the Watchdog task.
  While waiting for a startup signal from the Main Control task, this
  routine repeatedly resets the watchdog.  If the startup signal from
  the Main Control task is not received before a time-out period, a
  fault is declared.

DEPENDENCIES
  The Watchdog task main processing loop should be started directly after
  this procedure.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dog_internal_init( void )
{
  /* signals returned from the wait procedures */
  uint32 sigs;

  /* Initialize the qurt signal object */\
  qurt_signal_init(&dog_sig_obj);

  /* Define watchdog timer
  */
   timer_def_osal(&dog_wakeup_timer,
   					NULL,
   					TIMER_NATIVE_OS_SIGNAL_TYPE,
   					&dog_sig_obj,
   					DOG_WAKEUP_TIMER_SIG);
   					
   timer_def_osal(&dog_startup_timer,
   					NULL,
   					TIMER_NATIVE_OS_SIGNAL_TYPE,
   					&dog_sig_obj,
   					DOG_STARTUP_TIMER_SIG);


  /* Process task acknowledge procedure.
  */
  rcevt_register_name_qurt(RCINIT_RCEVT_INITGROUPS, &dog_sig_obj, DOG_TASK_START_SIG);
  rcinit_handshake_startup();  
  
  #ifdef FEATURE_DOG_BARK
  /* Enable the watchdog bark timer. */
  HAL_dogEnableBark();
  

  /* Register an ISR for the watchdog bark interrupt. */
  HAL_dogRegisterBark();

  #endif /* FEATURE_DOG_BARK */
  
  HAL_dogEnable();

  /* Wait for startup/shutdown signal, or a timeout.  The task has been
  ** defined, and will be started after all the other tasks have been started.
  ** If some task hangs during initialization, we'll catch it here.
  */
  timer_set( &dog_wakeup_timer, DOG_WAKEUP_TIME, 0, T_MSEC);

   if (dog_enable_startup_timer)
   {
      timer_set( &dog_startup_timer, DOG_DEFAULT_STARTUP_TIME, 0, T_MSEC);
   }
  dog_kick();
                                                     
  while (1) {

	sigs = qurt_signal_wait( &dog_sig_obj,
							DOG_WAKEUP_TIMER_SIG | DOG_TASK_START_SIG | DOG_STARTUP_TIMER_SIG,
							QURT_SIGNAL_ATTR_WAIT_ANY);

    /* Reset the watchdog time while waiting for a startup signal
    ** from the Main Control task
    */
    if ( sigs & DOG_WAKEUP_TIMER_SIG ) 
    {
	  qurt_signal_clear(&dog_sig_obj,DOG_WAKEUP_TIMER_SIG);
      dog_kick();
	  timer_set( &dog_wakeup_timer, DOG_WAKEUP_TIME, 0, T_MSEC);
    }

    
    //dog timed out waiting on startup signala
    if (sigs & DOG_STARTUP_TIMER_SIG)
    {
		qurt_signal_clear( &dog_sig_obj, DOG_STARTUP_TIMER_SIG);
        break;
    }
    
    /* We've been sent the start signal, process task acknowledge procedure
    ** and return.
    */
    if ( sigs & DOG_TASK_START_SIG) 
    {
	  qurt_signal_clear( &dog_sig_obj, DOG_TASK_START_SIG);
      dog_kick();
      return;
    }

  } /* end of wait loop */

  /* The time-out period, between the Watchdog Task activating and the
  ** Main Control Task sending a start-up signal, has expired with no
  ** startup signal being received.  This is a fatal error.  ERR_FATAL
  ** does not return, and the watchdog circuit will reset the system.
  */

  #if (!defined(NO_DOG) && !defined(DOG_BRINGUP))
    ERR_FATAL( "Watchdog startup timeout", 0, 0, 0 );
  #endif /* ! NO_DOG && ! DOG_BRINGUP */

} /* end of dog_internal_init */




/*===========================================================================

FUNCTION DOG_TASK

DESCRIPTION
  This procedure contains the main processing loop for the Watchdog task.
  It checks that each task has reported in at least once during its task
  report interval.  If a task has not reported within its timeout period,
  the ERR_FATAL macro is used to log the error and reset the
  microprocessor.

DEPENDENCIES
  This procedure should be called in DOG task initialization.  Each task
  must report in at least once each task report interval. (Intervals are
  defined in the dog_state_table[] array.)

RETURN VALUE
  None, this procedure does not return.

SIDE EFFECTS
  The microprocessor may be reset during this task.

===========================================================================*/
void dog_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for qurt_thread_create().
    */
)
{
  uint32 sigs;
    /* signals returned from wait procedures */

  unsigned long i;
    /* loop index for Watchdog task wakeups      */

  unsigned long loop_index;
    /* loop index for checking each task         */


  dog_internal_init();                   /* Initialize task */



  /* Must have received the startup signal, loop forever
  */
  for (;;) {
    for (i = 0; i < CHECK_CNT; i++ ) {

	  timer_set(&dog_wakeup_timer, DOG_WAKEUP_TIME, 0,T_MSEC);
	  sigs = qurt_signal_wait( &dog_sig_obj, DOG_WAKEUP_TIMER_SIG, QURT_SIGNAL_ATTR_WAIT_ANY);

      dog_kick();       /* kick the watchdog timer */

      if ( sigs & DOG_WAKEUP_TIMER_SIG ) 
      {
			qurt_signal_clear( &dog_sig_obj, DOG_WAKEUP_TIMER_SIG);
      }


    }

    /* Loop to check if any task's counter has expired.
    */
    loop_index = 0;

     qurt_mutex_lock(&dog_state_tbl_crit_sect);

    do {


	 /* Check if sw watchdog is enabled */
	 if(dog_enabled == TRUE) {

      // Decrement the count and check for expiration
      if (--(dog_state_table[loop_index].count) == 0) {

        /* A value of 0 for the count does not indicate a DOG timeout
        ** if the task is not being monitored.
        */
        if ((dog_state_table[loop_index].timeout > 0) &&
            !(dog_state_table[loop_index].is_blocked)) {

#ifdef DOG_BRINGUP
          /* record the error for debug and reset the count */
          dog_err_bringup_timeouts++;
          dog_state_table[loop_index].err_cnt++; 
          dog_state_table[loop_index].count = dog_state_table[loop_index].timeout;
#endif




#if (!defined(NO_DOG) && !defined(DOG_BRINGUP))


          /* ERR_FATAL - Watchdog Timeout */
          ERR_FATAL("Watchdog detects task starvation", loop_index, 0, 0);

          /* The compiler doesn't know that ERR_FATAL doesn't return, so 'break'
          ** tells the compiler that we bail out on the loop in this case,
          ** which causes it to optimize the loop for *not* ERR_FATALing.
          *//*lint -e527 */  /* unreachable */
          break;/*lint +e527 */

#endif /* ! NO_DOG && ! DOG_BRINGUP */

        } /* end if (dog_state_table[loop_index].timeout > 0) */
        else
        {
          dog_state_table[loop_index].count = 0xff;
        }

      } /* end if (--dog_state_table[loop_index].count == 0) */
	 } /* if (dog_enabled == TRUE) */ 

      loop_index++;
    } while ( loop_index < DOG_NUM_TASKS );

     qurt_mutex_unlock(&dog_state_tbl_crit_sect);



  } /* end of infinite loop */

} /* end of dog_task */




/*===========================================================================

FUNCTION DOG_MONITOR_PAUSE

DESCRIPTION
   Pauses the monitoring of a specified task.

===========================================================================*/
void dog_monitor_pause(
   unsigned int report_id
)
{  
   uint32 index = report_id ^ DOG_ID_MANGLER;

   /* Some tasks have zero dog_report_val but L4 still calls this */
   if (!report_id) {  
#ifdef DOG_BRINGUP
     dog_err_pause_zero++;
     dog_err_pause_err++;
     dog_err_pause_arr[0]++;
#endif
     return;
   }

#ifndef DOG_BRINGUP
   /*lint -e730 */ /* Boolean argument to function */
   ASSERT( index < DOG_NUM_TASKS );/*lint +e730 */
#endif
   if( index < DOG_NUM_TASKS )
   {
     DOG_LOCK( );
     dog_state_table[index].is_blocked = TRUE;
     DOG_FREE( );
   }
#ifdef DOG_BRINGUP
   else {
     dog_err_pause_err++;
     if (report_id < DOG_NUM_TASKS) {
       dog_err_pause_arr[report_id]++;
     }
   }
#endif
} /* END dog_monitor_pause */


/*===========================================================================

FUNCTION DOG_MONITOR_RESUME

DESCRIPTION
   Resumes the monitoring of a specified task.

===========================================================================*/
void dog_monitor_resume(
   unsigned int report_id
)
{  
   uint32 index = report_id ^ DOG_ID_MANGLER;

   /* Issue here. REX_RESUME_DOG_MONITOR not looking at auto-dog issue:
   ** task, e.g.,  smd_task gets rescheduled. It is not a dog reporter.
   ** THis causes dog_monitor_resume() to get called with a ZERO (never
   ** set) value. After we ^ with DOG_ID_MANGLER we get an ASSERT.
   ** So allow ZERO for now...
   */ 
   if (!report_id) {  /* Erroneous Legacy - ignore - see above */
#ifdef DOG_BRINGUP
     dog_err_resume_zero++;
     dog_err_resume_err++;
     dog_err_resume_arr[0]++;
#endif
     return;
   }

#ifndef DOG_BRINGUP
   /*lint -e730 */
   ASSERT( index < DOG_NUM_TASKS ); /*lint +e730 */
#endif
   if( index < DOG_NUM_TASKS )
   {
     DOG_LOCK( );
     dog_state_table[index].is_blocked = FALSE;
     dog_state_table[index].count =
       ( dog_state_table[index].timeout == 0 ) ?
       0xff : dog_state_table[index].timeout;
     DOG_FREE( );
   }
#ifdef DOG_BRINGUP
   else {
     dog_err_resume_err++;
     if (report_id < DOG_NUM_TASKS) {
       dog_err_resume_arr[report_id]++;
     }
   }
#endif
} /* END dog_monitor_resume */







/*===========================================================================

FUNCTION DOG_CRASH_REPORT

DESCRIPTION
   Uses provided function pointer to print a text dump of dog_state_table

===========================================================================*/
#define LOG_BUF_SIZE 80

void dog_crash_report(void (*print_ptr)(char *))
{
  static tms_chr log_buf[LOG_BUF_SIZE]; // need to find good size for temp string
  int i;

  if(0 > tms_utils_fmt(log_buf,LOG_BUF_SIZE,"Dog Report Information (dog_state_table)\n")) {;}
  print_ptr(log_buf);

   if(0 > tms_utils_fmt(log_buf,LOG_BUF_SIZE," [idx]   Task Name   Pri  Timeout   Count  Is_Blocked\n")) {;}
  print_ptr(log_buf);

  for(i=0; i<DOG_NUM_TASKS; i++)
  {
     if(0 > tms_utils_fmt(log_buf,LOG_BUF_SIZE," [%3d] %11s  %4d   %6d  %6d       %1d\n",
      i,
      dog_state_table[i].task, 
      0, /* WAS: dog_task_info[i].priority,  */ 
      (int)dog_state_table[i].timeout, 
      (int)dog_state_table[i].count, 
      dog_state_table[i].is_blocked)) {;}
      print_ptr(log_buf);
  }

   if(0 > tms_utils_fmt(log_buf,LOG_BUF_SIZE,"End Dog Report\n\n")) {;}
  print_ptr(log_buf);
}



/*===========================================================================

FUNCTION DOG_GET_REPORT_PERIOD

DESCRIPTION
   Function to return the number of msec between expected calls to
   dog_report(). This will be less than the dog timeout period by a safety 
   margin. 

   NOT DESIGNED TO BE CALLED FROM LEGACY TASKS THAT HAVE NOT FORMALLY 
   REGISTERED THROUGH THE NEW API.

RETURN VALUE
   0  - DO NOT REPORT TO DOG.
  >0  - Report to dog every "return_value" msec.

===========================================================================*/
extern uint32 dog_get_report_period(
  dog_report_type dog_report_id
)
{
  dog_report_type    i       = dog_report_id ^ DOG_ID_MANGLER;
  uint32 rtn_val = 0;

  if ( i < DOG_NUM_TASKS ) {

    /* Not designed for legacy un-registered tasks!! */

    if ( dog_state_table[i].timeout == DOG_MS_TO_COUNTS(DOG_FAST_TIMEOUT) ) 
    {
        rtn_val = FAST_REPORT;  
    }
    else if ( dog_state_table[i].timeout == DOG_MS_TO_COUNTS(DOG_DEFAULT_TIMEOUT) ) 
    {
        rtn_val = NORM_REPORT;  
    }
    else {
        rtn_val = DOG_TO_QURT_TIME(DOG_COUNTS_TO_MS(dog_state_table[i].timeout) / DOG_REPORT_DIVISOR);
    }

    /* Report to avoid timeouts during startup. */
    dog_report( dog_report_id );
  }
  else {       /* Not a valid entry - tell them not to report */
    ERR_FATAL ("dog_get_report_period: Invalid ID.",  0, 0, 0 );
  }

  return ( rtn_val );

};

/*===========================================================================

FUNCTION DOG_DEREGISTER

DESCRIPTION
   Dynamic way for tasks to de-register from dog services AND for CMI compliance.

===========================================================================*/
extern void dog_deregister(
  dog_report_type dog_report_id
)
{
  dog_report_type i  = dog_report_id ^ DOG_ID_MANGLER;

  /* NOTE: If this is a legacy task then the globals dog_*_rpt* exist 
  ** BUT will be invalid.  */

  /* Some validation: */ 
  if ( i >= DOG_NUM_TASKS ) {
    ERR_FATAL ("DOG Deregister: Invalid ID.",  0, 0, 0 );
  }

  qurt_mutex_lock(&dog_state_tbl_crit_sect);

  strlcpy (dog_state_table[i].task, "                      "
                                           , sizeof (dog_state_table[i].task));

  dog_state_table[i].count              = 0xffffffff;
  dog_state_table[i].timeout            = 0; /* indicates not monitored */
  dog_state_table[i].thread_id          = DOG_DUMMY_THREAD_ID;
  dog_state_table[i].is_blocked         = FALSE;
  dog_state_table[i].deadlockcheck_flag = DOG_DEADLKCHK_UNDEF;

#ifdef DOG_BRINGUP
  dog_state_table[i].err_cnt        = 0;
  dog_state_table[i].rpt_cnt        = 0;
#endif

  qurt_mutex_unlock(&dog_state_tbl_crit_sect);

};


/*===========================================================================

FUNCTION dog_intentional_timeout

DESCRIPTION
   This function is intended for testing purposes only.  It will prevent 
   kicking the dog to force a timeout.

===========================================================================*/
void dog_intentional_timeout(void)
{
  DOG_LOCK( );
    HAL_dogForceBite();                 //lower bite value
    dog_intentional_timeout_flag = TRUE;  //Flag to block kicks
  DOG_FREE( );
};



