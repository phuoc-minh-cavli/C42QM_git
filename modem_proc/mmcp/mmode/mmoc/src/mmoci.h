#ifndef MMOCI_H
#define MMOCI_H
/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
               M U L T I M O D E   C O N T R O L L E R ( MMoC )

                 I N T E R N A L    H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the internal types necessary for implementing
  the MMoC.

Copyright (c) 2002-2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM_MM/APPS/vcs/mmoci.h_v   1.2   18 Jun 2002 16:40:50   sjaikuma  $
  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/mmoc/src/mmoci.h#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/29/14   jvo     Refactored message macros
01/24/14   rm      Adding debug buffers 
08/28/13   jvo     Cleanup remaining Fusion pieces
07/19/13   jvo     Remove all MDM/MSM Fusion features
01/25/13   gm      Avoid duplicate initialization of SD
01/25/13   gm      FR 3059 - ONLINE optimization - skip duplicate subscription
09/27/12   vm      Removing DIME featurization and cleanup
09/21/12   vm      Added ignore_protocol_activate in mmoc_cmd_suspend_ss_s_type
08/08/12   vm      Removing is_rf_initialized flag as rfm is handled
                          in cm
05/18/12   xs      Reset redir sanity timer
05/01/12   gm      Add offline notify send from MMOC
04/20/12   sk      Fixing compiler warnings
04/16/12   gm      Add new function for initializing before task.
03/21/12   ab      Added support for handling Auto deact ind
                   whem MMOC is waiting for open session confirm
03/02/12   rk      Adding support for deativating dormant protocol through
                   MMOC
02/15/12   vk      Replace usage of AEE library function with corresponding
                   CoreBSP library
01/17/12   aj/cl   SC SVLTE SO 33 changes
01/11/11   xs      Remove MMOC_TRANS_PWR_SAVE_EXIT transaction state
09/14/11   vk      New MMOC transaction "MMOC_TRANS_DEACT_1XCSFB_CMD" related
                   changes.
08/18/11   fc      Depreciate QTF.
07/22/11   xs      MMOC SC_SVLTE 1.0 merging to Mainline
06/15/11   rk      correcting the featurization for FEATURE_MMOC_MMGSDI
05/20/11   ak      Adding new Transaction for blocked scan permission toggling
05/16/11   xs      Added another stop mode rquest to deactivate NAS in suspend
                   state
04/09/11   cl/ak   Current state from the mmoc_info_ptr instead of gen_cmd.
03/25/11   xs      Extend MMOC sanity timer to 84 sec for PS detach operation
03/09/11   cl      Add support for SVLTE2 MMSS arbitration
02/28/11   aj      Add wait for activation_cnf state in DO to LTE reselection
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
01/11/11   am      MUTE migration to qtf changes
01/04/11   rm      CMI-4: Moved #def to internal files
12/16/10   am      Cleaning the code
12/10/10   rk      MO/MT 1XCSFB call successful scenarios support
12/02/10   gm      Fix to MOB compiler warnings
11/10/10   am      Changes for MUTE to MOB migration
11/05/10   sv/gm   Changes to wait for session open confirmation
10/27/10   rk      Regarding support for deativating dormant protocol
10/27/10   rk      Adding support for deativating dormant protocol through
                   MMOC
10/05/10   pm      Replace clk.h with timer.h & remove FEATURE_USE_TIME_VU
08/02/10   nm      Removed definition of EXTERN
06/10/10   ak      Mainlining memory optimization changes.
05/24/10   mp      Added support for session close and
                   MMOC_TRANS_MMGSDI_INFO_IND transaction to avoid nested
                   callbacks
04/26/10   mp      Changed union to struct for mmoc_trans_info_s_type.
03/21/10   mp      MLPL/MSPL/PLMN Interface for MMSS 3.1 support
02/11/10   rn      LTE-1x redirection/reselection
01/28/10   mp      EF_EPRL Implementation
12/11/09   rn      LTE-eHRPD Support
12/08/09   fj      Added lte_band_pref.
12/03/09   ak      Enabling FEATURE_HICPS_STACK_SYNC_UP on target by default.
10/13/09   sg      ZI memory reduction. Mmode should call new Heap Manager API
                   to allocate memory dynamically.
07/13/09   np      Added code for LTE support
12/22/08   rn      Added support for TMC memory utilization
11/14/08   jd      Corrected Lint errors
09/19/08   sv      Corrected Featurization FEATURE_MMOC_LOW_MEM_TARGET
09/19/08   jd      Memory reduction changes
09/12/08   fj/aj   Added support for Modem Reset
06/16/08   sv      Added MMOC_ENUM_IS_INRANGE
04/28/08   fj      Lint cleanup, CR 144389
04/16/07   ic      Moved inclusion of AEE* headers to mmoci.h
03/19/07   ka      Replacing banned apis
01/31/07   ka      Adding new state in MMOC to wait for TMC to ack operating
                   mode change.
11/15/06   sk      Added hybrid GW support.
08/29/06   sk      Added WLAN Adhoc support.
04/03/06   ka      Changing copyright.
12/30/05   ka      Adding string maps to mmoc commands, reports and state
                   transitions.
07/07/05   ka      Adding new transaction states to complete deactivation
                   for HDR.
03/02/05   ka      Handling subscription available along with protocol name
                   that has subscription available.
08/20/04   sj      Added support for prl_pref.
05/10/04   sj      Merged in changes for srch inactive state.
02/24/04   sj      Removed LINT warnings.
02/05/04   SJ      Increased srch response timer to 60 seconds.
01/13/04   sj      Added REX sig for duplicate command detection.
10/21/03   sj      Increased the cmd q size to 15 & sanity timeout to 1min.
09/15/03   sj      Fixed issues with HDR <-> 1x runtime switching.
08/15/03   sj      Changed usage of #ifdef T_PC to #if (TG==T_PC).
08/12/03   sj      Updates for placing srch in standby during power save
                   and LPM.
03/17/03   SJ      Increased mmoc's sanity timer to 20 seconds.
02/26/03   SJ      Modified sanity timeout err_fatal to print the offending
                   protocol task's name instead of mmoc.c.
02/14/03   SJ      Added support for new MMoC wakeup from pwr_save
                   transaction.
01/31/03   SJ      Increased insanity timer to 15 seconds.
12/18/02   SJ      Added mode_info_s_type type.
09/03/02   SJ      Unfeaturized GSM support in MMoC.
06/10/02   SJ      Corrected sanity timer value.
03/28/02   SJ      Initial release.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */
#include "cmd.h"      /* Command header for inter-task communication */
#include "timer.h"    /* Interface for timer services */

#include "sd.h"       /* Interface for System Determination services */
#include "sd_v.h"
#include "prot.h"     /* Common header for mmoc <-> protocol communications*/
#include "prot_v.h"
#include "mmoc.h"     /* External interface definitions for MMoC */
#include "mmoc_v.h"
#include "mmoc_msgr.h" /* For support to send MSGR notify to other modules */

#include "task.h"     /* Interface for global task resources */

#include <stdarg.h>   /* vsprintf and va_end functions */

/* Gives prototype for std_xx calls
*/
#include <string.h>

#include <stringl/stringl.h>

#include "mmgsdilib.h"
#include "mmgsdilib_common.h"
#include "mmgsdisessionlib.h"
#include "cmll.h"
#include "cm_i.h"

#ifdef MMOC_DEBUG
#error code not present
#endif /* MMOC_DEBUG */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
=============================================================================
=============================================================================
============================== SUPPORTED PROTOCOL DEFINES ===================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup SPD Supported Protocol Defines
** @{
*/

/** CDMA supported define.
*/
#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) && \
    !defined(FEATURE_MMODE_REMOVE_1X))
#define MMOC_CDMA_SUPPORTED
#endif

/** HDR supported define.
*/
#ifdef FEATURE_HDR
#define MMOC_HDR_SUPPORTED
#endif

/** GSM and/or WCDMA supported define.
*/
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)
#define MMOC_GW_SUPPORTED
#endif

/** GW or LTE supported definition
*/
#if defined(FEATURE_MMOC_LTE) || defined(MMOC_GW_SUPPORTED)
#define MMOC_GWL_SUPPORTED
#endif

/** HDR supported define.
*/
#if defined(FEATURE_HDR) || defined(MMOC_HYBR_GW_SUPPORTED)
#define MMOC_HYBR_1_SUPPORTED
#endif

/** FTM supported define.
*/
#ifdef FEATURE_FACTORY_TESTMODE
#define MMOC_FTM_SUPPORTED
#endif

/** Define to indicate if MMOC should use DB services.
*/
#if defined(MMOC_CDMA_SUPPORTED)||defined(MMOC_HDR_SUPPORTED)
#define MMOC_USES_DB_SERVICES
#endif

/** Define if HDR hybrid is supported.
*/
#if defined(FEATURE_HDR) && defined(FEATURE_HDR_HYBRID)
#define MMOC_HYBR_SUPPORTED
#endif

/** Define if LTE to HDR redirection is supported.
*/
#if defined(FEATURE_LTE_TO_HDR)
#define MMOC_LTE_TO_HDR
#endif

/** Define if LTE to 1X redirection is supported.
*/
#if defined(FEATURE_LTE_TO_1X)
#define MMOC_LTE_TO_1X
#endif

/** @}
**
*/


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC DEBUG RELATED STUFF ===============================
=============================================================================
=============================================================================
===========================================================================*/

/*---------------------------------------------------------------------------
      Compile MMoC with debug information, asserts and messages.
---------------------------------------------------------------------------*/
#if !defined(_lint)
#define LOCALF
#define LOCALD extern
#else
#define LOCALF static
#define LOCALD static
#endif

/* General message macros, some are for testing stubs */
#ifndef MSG

#if defined(MMOC_DEBUG) || defined(CM_DEBUG)
#error code not present
#else
#define MSG(   SSID, level, str )                 MSG_BASE(level,str, 0, 0, 0)
#define MSG_1( SSID, level, str, a )              MSG_BASE(level,str,a,0,0)
#define MSG_2( SSID, level, str, a, b )           MSG_BASE(level,str,a,b,0)
#define MSG_3( SSID, level, str, a, b, c )        MSG_BASE(level,str,a,b,c)
#endif

#define MMODE_MSG_LEGACY_FATAL         MSG_LVL_FATAL
#define MMODE_MSG_LEGACY_ERROR         MSG_LVL_ERROR
#define MMODE_MSG_LEGACY_HIGH          MSG_LVL_HIGH
#define MMODE_MSG_LEGACY_MED           MSG_LVL_MED
#define MMODE_MSG_LEGACY_LOW           MSG_LVL_LOW
#endif /* ! MSG */

#ifndef MMODE_MSG_LEGACY_FATAL

#define MMODE_MSG_LEGACY_FATAL         MSG_LEGACY_FATAL
#define MMODE_MSG_LEGACY_ERROR         MSG_LEGACY_ERROR
#define MMODE_MSG_LEGACY_HIGH          MSG_LEGACY_HIGH
#define MMODE_MSG_LEGACY_MED           MSG_LEGACY_MED
#define MMODE_MSG_LEGACY_LOW           MSG_LEGACY_LOW

#endif /* MMODE_MSG_LEGACY_FATAL */


/*-------------------------------------------------------------------------*/

/* MAIN MESSAGE MACROS */
/* Messages have numbers to designate the number of parameters, not counting
** the string parameter.  For example, if the format string is "x:%d, y:%y",
** use MMOC_MSG_HIGH_2("x:%d, y:%y", x, y) because there are two parameters in
** the format string.
**
** MMOC_MSG_LOW_x, MMOC_MSG_MED_x, and MMOC_MSG_HIGH_x simply print to the message log
**   with their designated message level.
** 
** MMOC_ERR_x quits during testing or uses MSG_MED on target
**
** MMOC_ERR_FATAL_x prints to the message log and exits the software.
*/

#if defined(CM_DEBUG) || defined(MMOC_DEBUG) || defined(SD_DEBUG)
#error code not present
#else
#define MMOC_ERR_0(str)                MSG( MSG_SSID_MMOC,           \
                                          MMODE_MSG_LEGACY_MED,      \
                                          "=MMOC= " str )
#define MMOC_ERR_1(str, a)             MSG_1( MSG_SSID_MMOC,         \
                                          MMODE_MSG_LEGACY_MED,      \
                                          "=MMOC= " str, (a) )
#define MMOC_ERR_2(str, a, b)          MSG_2( MSG_SSID_MMOC,         \
                                          MMODE_MSG_LEGACY_MED,      \
                                          "=MMOC= " str, (a), (b) )
#define MMOC_ERR_3(str, a, b, c)       MSG_3( MSG_SSID_MMOC,         \
                                          MMODE_MSG_LEGACY_MED,      \
                                          "=MMOC= " str, (a), (b), (c) )

#endif /* CM_DEBUG || MMOC_DEBUG || SD_DEBUG */

#define MMOC_MSG_HIGH_0(str)             MSG( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=MMOC= " str )
#define MMOC_MSG_MED_0(str)              MSG( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=MMOC= " str )
#define MMOC_MSG_LOW_0(str)              MSG( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=MMOC= " str )

/*-------------------------------------------------------------------------*/

#define MMOC_MSG_HIGH_1(str, a)        MSG_1( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=MMOC= " str, (a) )
#define MMOC_MSG_MED_1(str, a)         MSG_1( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=MMOC= " str, (a) )
#define MMOC_MSG_LOW_1(str, a)         MSG_1( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=MMOC= " str, (a) )
/*-------------------------------------------------------------------------*/
#define MMOC_MSG_HIGH_2(str, a, b)     MSG_2( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=MMOC= " str, (a), (b) )
#define MMOC_MSG_MED_2(str, a, b)      MSG_2( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=MMOC= " str, (a), (b) )
#define MMOC_MSG_LOW_2(str, a, b)      MSG_2( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=MMOC= " str, (a), (b) )

/*-------------------------------------------------------------------------*/

#define MMOC_MSG_HIGH_3(str, a, b, c)  MSG_3( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=MMOC= " str, (a), (b), (c) )
#define MMOC_MSG_MED_3(str, a, b, c)   MSG_3( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=MMOC= " str, (a), (b), (c) )
#define MMOC_MSG_LOW_3(str, a, b, c)   MSG_3( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=MMOC= " str, (a), (b), (c) )
/*-------------------------------------------------------------------------*/

#define MMOC_MSG_HIGH_4(str, a, b, c, d)  MSG_4( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=MMOC= " str, (a), (b), (c), (d) )
#define MMOC_MSG_MED_4(str, a, b, c, d)   MSG_4( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=MMOC= " str, (a), (b), (c), (d) )
#define MMOC_MSG_LOW_4(str, a, b, c, d)   MSG_4( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=MMOC= " str, (a), (b), (c), (d) )


#define MMOC_MSG_HIGH_5(str, a, b, c, d, e)  MSG_5( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=MMOC= " str, (a), (b), (c), (d), (e) )
#define MMOC_MSG_MED_5(str, a, b, c, d, e)   MSG_5( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=MMOC= " str, (a), (b), (c), (d), (e) )
#define MMOC_MSG_LOW_5(str, a, b, c, d, e)   MSG_5( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=MMOC= " str, (a), (b), (c), (d), (e) )


#define MMOC_MSG_HIGH_6(str, a, b, c, d, e, f)  MSG_6( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=MMOC= " str, (a), (b), (c), (d), (e), (f) )
#define MMOC_MSG_MED_6(str, a, b, c, d, e, f)   MSG_6( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=MMOC= " str, (a), (b), (c), (d), (e) , (f) )
#define MMOC_MSG_LOW_6(str, a, b, c, d, e, f)   MSG_6( MSG_SSID_MMOC,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=MMOC= " str, (a), (b), (c), (d), (e), (f) )





/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*---------------------------------------------------------------------------
                  MMOC_DEBUG/T_PC independent macros.
---------------------------------------------------------------------------*/

#define MMOC_PRINT_MSG_HIGH                mmocdbg_print_message

#ifdef MMODE_ADDITIONAL_DEBUG_INFO
#define MMOC_MAX_DEBUG_BUFFER_SIZE         10
#define MMOC_MAX_RPT_COUNT                 10
#define MMOC_MAX_TASK_NAME_LEN             10
#define MMOC_DBG_BUF_TRANS_ADDL_INFO_SIZE  6
#endif
/* <EJECT> */
/*---------------------------------------------------------------------------
                   MMOC_DEBUG/T_PC dependent macros.
---------------------------------------------------------------------------*/


  
#ifdef MMOC_DEBUG
#error code not present
#else
  #define MMOC_ERR_FATAL_CHECK(str,a,b,c )      ERR_FATAL( "=MMOC= ",a,b,c )
#endif
  

  #define MMOC_FUNC_NOT_IMP( func ) \
      MMOC_MSG_MED_0( #func " NOT Implemented!")

  #define MMOC_CASE_NOT_IMP( _case ) \
      MMOC_MSG_MED_0( #_case " case NOT Implemented")

#define MMOC_ERR_FATAL_0(str)            MMOC_ERR_FATAL_CHECK( str, 0, 0, 0 )
#define MMOC_ERR_FATAL_1(str, a)         MMOC_ERR_FATAL_CHECK( str, (a), 0, 0 )
#define MMOC_ERR_FATAL_2(str, a, b)      MMOC_ERR_FATAL_CHECK( str, (a), (b), 0 )
#define MMOC_ERR_FATAL_3(str, a, b, c)   MMOC_ERR_FATAL_CHECK( str, (a), (b), (c) )

/*---------------------------------------------------------------------------
                  Asserts and verifications macros
---------------------------------------------------------------------------*/

#ifdef MMOC_DEBUG
  #error code not present
#else
  #define MMOC_ASSERT( exp )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define MMOC_ENUM_IS_INRANGE( val, max ) \
                       /*lint -e568 -e641 Converting enum to int*/ \
                       ( (val) >= 0 && (val) < (max) ) \
                       /*lint +e568 -e641*/


#ifdef MMOC_DEBUG
  #error code not present
#else
  #define MMOC_ASSERT_ENUM_IS_INRANGE( val, max )

#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef MMOC_DEBUG
  #error code not present
#else
  #define MMOC_ASSERT_ENUM_IS_INRANGE_INC( val, max )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef MMOC_DEBUG
  #error code not present
#else
  #define MMOC_ASSERT_ENUM_IS_INRANGE_OR( val, max, equal )
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*
** UOOS should no longer be enabled. MMODE no longer enables it. However if
** the feature is enabled, it is outside the scope of MMODE and MMODE will
** disable the HICPS feature to avoid functional conflicts.
*/
#if defined( FEATURE_UMTS_UNIFORM_OOS_HANDLING )
#undef FEATURE_HICPS_STACK_SYNC_UP
#endif

/*---------------------------------------------------------------------------
                               Bit operation MACROS
---------------------------------------------------------------------------*/

/* Macro to check if e1 contains e0.
*/
#define MMOC_IS_BIT_CONTAIN( e1, e0 ) ( ((word) e1 & (word) e0) )
/* Macro to return bits that are set either in e1 or in e0.
*/

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC COMMAND INTERFACE RELATED TYPES ===================
=============================================================================
=============================================================================
===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with subscription changed
** command to MMoC task.
*/
typedef struct {

  /* Type of subscription change.
  */
  mmoc_subsc_chg_e_type                chg_type;

  prot_subsc_chg_e_type                prot_subsc_chg;
  /* Enumeration of protocols that have a change in
  ** subscription available status
  */
  byte                                 nam;
  mmoc_subs_data_s_type                gw;
} mmoc_cmd_subsc_chgd_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* A type for parameters that are associated with operating mode changed
** command to MMoC task.
*/
typedef struct {

  /* Indicates the new operating mode (ONLINE, OFFLINE etc) for the phone.
  */
  sys_oprt_mode_e_type                 oprt_mode;

} mmoc_cmd_oprt_mode_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/* A type for parameters that are associated with wakeup from pwr_save
** command to MMoC task.
*/
typedef struct {

  /* Indicates the instance to be woken up from pwr_save.
  */
  sd_ss_e_type                         ss;

} mmoc_cmd_wakeup_pwrsave_s_type;


/* A type for parameters that are associated with suspend ss
** command to MMoC task.
*/
typedef struct {

  /* Indicates whether to suspend or resume
  */
  boolean                              is_suspend;

  /* Indicates whether to activate protocol or not
  */
  boolean                              ignore_protocol_activate;

  /* Indicates suspension reason
  */
  prot_deact_e_type                    susp_reason;

} mmoc_cmd_suspend_ss_s_type;


/* Command header type.  Each command to MMoC starts with this header.
*/
typedef struct {

  /* Command header */
  cmd_hdr_type                         hdr;

  /* Command name */
  mmoc_cmd_e_type                      name;

} mmoc_cmd_hdr_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with queuing a command to MMoC
** task.
*/
typedef struct {

  /* The header information that is associated with this command.
  */
  mmoc_cmd_hdr_s_type                  cmd;

  /* union of parameters that are associated with the different commands.
  */
  union   {

    /*  Parameters for subscription changed command.
    */
    mmoc_cmd_subsc_chgd_s_type         subsc_chgd;

    /* Parameters for operating mode changed command.
    */
    mmoc_cmd_oprt_mode_s_type          oprt_mode_chgd;

    /* Parameters for protocol generic command.
    */
    prot_gen_cmd_s_type                gen_prot_cmd;

    /* Parameters for wakeup from pwr_save.
    */
    mmoc_cmd_wakeup_pwrsave_s_type     wakeup;

    /* Parameters for suspend SS.
    */
    mmoc_cmd_suspend_ss_s_type         suspend_ss;

  } param;

} mmoc_cmd_msg_s_type;



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC REPORT INTERFACE RELATED TYPES ====================
=============================================================================
=============================================================================
===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration for the type of report queued in the report queueof MMoC
** task.
*/
typedef enum {

  /* 0 */
  MMOC_RPT_PROT_DEACTD_CNF,            /* Protocol deactivated confirmation
                                       ** report.
                                       */

  MMOC_RPT_PROT_AUTO_DEACTD_IND,       /* Autonomous protocol deactivated
                                       ** indication report.
                                       */

  MMOC_RPT_PH_STAT_CHGD_CNF,           /* Acknowledgement report for phone
                                       ** status changed command.
                                       */

  MMOC_RPT_PROT_GEN_CMD_CNF,           /* Acknowledgement report for Generic
                                       ** command received by the protocols.
                                       */
  /* 4 */
  MMOC_RPT_PROT_AUTO_ACTD_IND,         /* Autonomous protocol activated
                                       ** indication report.
                                       */

  MMOC_RPT_ACTIVATION_CNF,             /* Acknowledgement report from CMREGPRX
                                       ** about activation
                                       */
  /* 8 */
  MMOC_RPT_PS_DETACH_CNF,            /* Acknowledgement report for
                                      ** PS detach cnf command
                                      */

  MMOC_RPT_MAX                         /* For internal use. */

} mmoc_rpt_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef MMODE_ADDITIONAL_DEBUG_INFO

/* Enumeration for type of task that queue reports to MMOC
** 
*/
typedef enum {

/* 0 */
MMOC_TASK_NAME_NONE,     

MMOC_TASK_NAME_MMOC,     

MMOC_TASK_NAME_CM,

MMOC_TASK_NAME_MAX

}mmoc_rpt_task_name_e_type;

#endif /* MMODE_ADDITIONAL_DEBUG_INFO */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* A type for parameters that are associated with protocol deactivated
** confirmation report to MMoC task.
*/
typedef struct {

  /* Transaction Id that associated with the corresponding deactivation
  ** command.
  */
  prot_trans_type                      trans_id;


} mmoc_rpt_deactd_cnf_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with Ps detach
** confirmation report to MMoC task.
*/
typedef struct {

  /* Transaction Id that associated with the corresponding PS detach
  ** command.
  */
  prot_trans_type                      trans_id;


}mmoc_rpt_ps_detach_cnf_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with protocol deactivated
** indication report to MMoC task.
*/
typedef struct {

  /* Reason for the protocol deactivation.
  */
  prot_deact_e_type                    reason;

  /* Protocol which sent the autonomous deactivation.
  */
  prot_state_e_type                    prot_state;


} mmoc_rpt_auto_deactd_ind_s_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with protocol activated
** indication report to MMoC task.
*/
typedef struct {

  /* Reason for the protocol activation.
  */
  prot_act_e_type                      reason;

  /* Protocol which sent the autonomous activation.
  */
  prot_state_e_type                    prot_state;

} mmoc_rpt_auto_actd_ind_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with information on phone
** status changed acknowledgement protocol report.
*/
typedef struct {

  /* The sequence number that associated with the corresponding phone status
  ** changed command.
  */
  prot_trans_type                      trans_id;

  /* Protocol responding with the cnf.
  */
  mmoc_mode_e_type                     prot;

  mmoc_ph_stat_e_type                  mc_status;
  /* status of the command - success or error code. */

} mmoc_rpt_ph_stat_chgd_cnf_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with the acknowledgement
** for the generic command from the MMoC.
*/
typedef struct {

  /* Indicates whether the MMoC's generic command was processed or not.
  ** true - command was processed,
  ** false - command not processed.
  */
  boolean                              is_cmd_processed;

  /* Transaction Id that is associated with the corresponding generic
  ** command.
  */
  prot_trans_type                      trans_id;

  /* Indicates if the protocol is deactivating, because of processing
  ** the generic command. Valid only when is_cmd_processed = TRUE.
  */
  boolean                              is_prot_deactivating;

} mmoc_rpt_prot_gen_cmd_cnf_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Structure used by CMREGPRX to acknowledge activation RSP
*/

typedef struct {

  /* Protocol sending the activation confirmation
  */
  prot_state_e_type                      prot_state;

} mmoc_rpt_activation_cnf_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Structure used by TMC to acknowledge operating
** mode changes.
*/

typedef struct {

  /* Transaction Id that is associated with the corresponding generic
  ** command.
  */
  prot_trans_type                      trans_id;

} mmoc_rpt_tmc_mode_cnf_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Report header type.  Each report to MMoC starts with this header.
*/

typedef struct {

  /* header info */
  cmd_hdr_type                         hdr;

  /* Report name */
  mmoc_rpt_e_type                      name;

 #ifdef MMODE_ADDITIONAL_DEBUG_INFO

  /* Name of the task from where report is queued 
  */
  mmoc_rpt_task_name_e_type            task_name;
  #endif

  /* is_reused used to determine whether this report 
  ** needs to be freed or reused later */
  boolean                              is_reused;

} mmoc_rpt_hdr_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* A type for parameters that are associated with queuing a report to
** MMoC task.
*/
typedef struct {

  /* The header information that is associated with this report.
  */
  mmoc_rpt_hdr_s_type                  rpt;

  /* Union of parameters that are associated with the different reports.
  */
  union   {

    /* Parameters for protocol deactivated confirmation report.
    */
    mmoc_rpt_deactd_cnf_s_type           prot_deactd_cnf;

    /* Parameters for protocol deactivated indication report.
    */
    mmoc_rpt_auto_deactd_ind_s_type      prot_deactd_ind;

    /* Parameters for protocol activated indication report.
    */
    mmoc_rpt_auto_actd_ind_s_type        prot_actd_ind;

    /* Parameters for phone status changed acknowledgement report.
    */
    mmoc_rpt_ph_stat_chgd_cnf_s_type     ph_stat_cnf;

    /* Parameters for generic command acknowledgement report.
    */
    mmoc_rpt_prot_gen_cmd_cnf_s_type     gen_cmd_cnf;

    /* Parameter used by TMC to acknowledge operating
    ** mode change.
    */
    mmoc_rpt_tmc_mode_cnf_s_type         tmc_mode_cnf;

  
   /* Parameter for activation CNF from CMREGPRX
   */
    mmoc_rpt_activation_cnf_s_type       activation_cnf;

   /* Parameter for PS detach CNF from CMREGPRX
   */
    mmoc_rpt_ps_detach_cnf_s_type        ps_detach_cnf;   

  } param;

} mmoc_rpt_msg_s_type;

/* Type for insanity err_fatal function to be implemented by the protocols.
*/
typedef void (mmoc_sanity_err_f_type) (

  /* Sanity error reason, either no response to deactivate req, generic cmd
  ** or phone status changed requests etc.
  */
  mmoc_sanity_err_e_type               reason

);

/* No. of fixed size requests that must be satisfied by the MMOC heap.
*/
#define   MMOC_HEAP_SLOT_COUNT         6

/* The additional 15 bytes is to take into account the paragraph boundary
** alignment done by mem_init_heap().
*/
#define   MMOC_HEAP_BUFFER_SIZE        ((MMOC_MAX_ACTUAL_ALLOC_SIZE * \
                                      MMOC_HEAP_SLOT_COUNT) + 15)


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC REX TASK RELATED TYPES ============================
=============================================================================
===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef FEATURE_MMOC_TMC_HEAP
/* The number of command buffers or command q size.
*/
#ifdef FEATURE_MMOC_LOW_MEM_TARGET
#define  MMOC_CMD_BUFS_NUM               6
#else
#define  MMOC_CMD_BUFS_NUM               15
#endif

/* The number of report buffers or report q size.
*/
#define  MMOC_RPT_BUFS_NUM               6
#endif /* FEATURE_MMOC_TMC_HEAP */

/* This signal is set by FTM for resuming MMOC
in case of ftm_protocol_activate and ftm_protocol_deactivate.
*/
#define MMOC_FTM_RESUME_MMOC_SIG     0x0200
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if defined(FEATURE_CIOT)
/* The MMoC's sanity timer value. Used to check is MMoC is not stuck with
** a particular transaction.
** This timer value should be used for all transactions except powerdown.
** So for all transactions MMOC will error fatal after 90 seconds based
** on INSANITY_COUNT of 5.
*/
#define  MMOC_SANITY_TIME                 (15000)
#else
/* The MMoC's sanity timer value. Used to check is MMoC is not stuck with
** a particular transaction.
** This timer value should be used for all transactions except powerdown.
** So for all transactions MMOC will error fatal after 72 seconds based
** on INSANITY_COUNT of 6.
*/
#define  MMOC_SANITY_TIME                 (12000)
#endif

#if defined(FEATURE_CIOT)
/* MMoC's sanity timer value for power down transaction.
** For power down transaction, MMoC will error fatal after 90 seconds based
** on INSANITY_COUNT of 6.
*/
#define  MMOC_PWRDOWN_SANITY_TIME         (15000)
#else
/* MMoC's sanity timer value for power down transaction.
** For power down transaction, MMoC will error fatal after 72 seconds based
** on INSANITY_COUNT of 6.
*/
#define  MMOC_PWRDOWN_SANITY_TIME         (12000)
#endif

#if defined(FEATURE_CIOT)
/* MMoC's sanity timer value for ps detach operation in CIOT flavors
** For ps detach operation, MMoC will error fatal after 1284 seconds based
** on INSANITY_COUNT of 6.
*/
#define  MMOC_PS_DETACH_SANITY_TIME         (214000)
#else
/* MMoC's sanity timer value for ps detach operation.
** For ps detach operation, MMoC will error fatal after 84 seconds based
** on INSANITY_COUNT of 6.
*/
#define  MMOC_PS_DETACH_SANITY_TIME         (14000)
#endif


/* The max count for which the MMoC's transaction was not changed, after
** this, MMoC will do ERR_FATAL.
*/
#define  MMOC_MAX_INSANITY_COUNT          (6)

/* MMOC error fatal message maximum length
*/
#define MMOC_ERR_FATAL_MSG_MAX_LENGTH (100)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define  MMOC_MAX_STRING_LENGTH 300

/* A type that associates the MMoC generic info other than state info.
*/
typedef struct
{

  /* System selection timer for SS_MAIN.
  */
  timer_type                           ss_main_timer;

  /* Timer to do rex timed wait
  */
  rex_timer_type                       rex_wait_timer;

  /* Queue for reports to MMoC task.
  */
  q_type                               rpt_q;

  /* Queue for commands to MMoC task.
  */
  q_type                               cmd_q;

  #ifdef FEATURE_MMOC_UNIT_TEST
  #error code not present
#endif

} mmoc_task_info_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC IMPLEMENTATION RELATED TYPES ======================
=============================================================================
=============================================================================
===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of transaction types in the MMoC.
*/
typedef enum {
  /* 0 */
  MMOC_TRANS_NULL,                     /* Null transaction - idle state for
                                       ** MMoC.
                                       */

  MMOC_TRANS_SUBSC_CHGD,               /* Transaction to process the
                                       ** subscription changed command.
                                       */

  MMOC_TRANS_ONLINE,                   /* Transaction to process the ONLINE
                                       ** operating mode command.
                                       */

  MMOC_TRANS_OFFLINE,                  /* Transaction to process the OFFLINE,
                                       ** OFFLINE_CDMA and OFFLINE_AMPS
                                       ** operating mode command.
                                       */

  /* 4 */
  MMOC_TRANS_PROT_GEN_CMD,             /* Transaction to process the generic
                                       ** command bound for protocol.
                                       */

  MMOC_TRANS_PWR_DOWN,                 /* Transaction to process the power down/
                                       ** RESET/LPM operating mode command.
                                       */


  MMOC_TRANS_PWR_SAVE_ENTER,           /* Transaction to process the power save
                                       ** enter request from protocols/SD20.
                                       */

  MMOC_TRANS_PROT_AUTO_DEACTD_IND,     /* Transaction to process the autonomous
                                       ** deactivated indication report from
                                       ** protocol.
                                       */


  MMOC_TRANS_SUSPEND_SS,               /* Transaction to suspend /resume
                                       ** given SS
                                       */

  MMOC_TRANS_MAX                       /* For internal use. */

} mmoc_trans_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of transaction state types in the MMoC. Note that not all
** transaction states are required to process a particular transaction.
*/
typedef enum {

  /* 0 */
  MMOC_TRANS_STATE_NULL,               /* Initial transaction state. Required
                                       ** for processing any transaction.
                                       */

  MMOC_TRANS_STATE_PROT_DEACT_ENTER,   /* Transaction state which sends the
                                       ** protocol deactivate request.
                                       */

  MMOC_TRANS_STATE_WAIT_DEACTD_CNF,    /* Transaction state which waits for
                                       ** deactivated protocol report.
                                       */


  MMOC_TRANS_STATE_PROT_PH_STAT_ENTER, /* Transaction state which sends the
                                       ** phone status changed command to the
                                       ** first supported protocol.
                                       */

  MMOC_TRANS_STATE_WAIT_PH_STAT_CNF,   /* Transaction state which waits for
                                       ** the acknowledgement for phone
                                       ** status changed request.
                                       */

  MMOC_TRANS_STATE_GEN_CMD_ENTER,      /* Transaction state which either
                                       ** sends the generic command to the
                                       ** active protocol or processes it in
                                       ** MMoC if there are no active
                                       ** protocols.
                                       */

  /* 8 */
  MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF,   /* Transaction state which waits for
                                       ** the acknowledgement for generic
                                       ** command.
                                       */

  MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND,/* Transaction state which waits for
                                        ** autonomous deactivated indication
                                        */

  MMOC_TRANS_STATE_WAIT_AUTO_ACTD_IND,  /* Transaction state which waits for
                                        ** autonomous activated indication
                                        */


  MMOC_TRANS_STATE_GEN_CMD_ACTIVATION_ENTER,
                                       /* Transient transaction state from which MMOC enters state
                                       ** WAIT_ACTIVATION_CNF
                                       */


  MMOC_TRANS_STATE_WAIT_ACTIVATION_CNF,
                                       /* Transaction state which wait for activation confirmation
                                       ** before ending the transaction
                                       */
  /* 20 */
  MMOC_TRANS_STATE_WAIT_PS_DETACH_CNF,


  MMOC_TRANS_STATE_WAIT_DEACTD_CNF_GWL,
                                       /* Transaction state which wait for deactivate report from
                                       ** GWL protocol only
                                       */

  MMOC_TRANS_STATE_SUSPEND_SS_ENTER,
                                      /* Transaction state that sets a given
                                      ** SS into suspend state
                                      */
  MMOC_TRANS_STATE_RESUME_SS_ENTER,
                                      /* Transaction state that resumes
                                      ** operation on an SS that was suspended
                                      */

  MMOC_TRANS_STATE_WAIT_HOLD_USER_ACT_CNF,
                                      /* Transaction state twhich waits for
                                      ** HOLD_USER_ACT_CNF from CM.
                                      */

  MMOC_TRANS_STATE_MAX                 /* For internal use. */

} mmoc_trans_state_e_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration that describes whether the event (command or report) was
** processed to completion or not.
*/
typedef enum {

  MMOC_EVT_STATUS_CONSUMED,            /* The event was processed to
                                       ** completion.
                                       */

  MMOC_EVT_STATUS_NOT_CONSUMED,        /* The event was not processed, the
                                       ** MMoC should call the transaction
                                       ** handlers again.
                                       */

  MMOC_EVT_STATUS_MAX                  /* For internal use. */

} mmoc_evt_status_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for transaction exit cleanup function.
*/
typedef mmoc_evt_status_e_type (mmoc_exit_fn_f_type) (

  /* MMOC's state info.
  */
  void                         *info_ptr

);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that holds the information regarding the transaction
** MMOC_TRANS_PROT_GEN_CMD being processed.
*/
typedef struct {

  /* Boolean flag to indicate if SS_MAIN needs to be
  ** activated at the end of transaction.
  */
  boolean                             is_activate_main;

  /* Info for protocol generic command transaction.
  */
  prot_gen_cmd_s_type                  cmd_info;


} mmoc_gen_cmd_trans_info_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that holds the information regarding the transaction
** being processed. Initially it holds the information on the command or report
** which invoked this transaction. If additional information needs to be stored
** as part of trans_info, then new types have to be created.
*/
typedef struct {

  /* Info for subscription changed transaction.
  */
  mmoc_cmd_subsc_chgd_s_type           subsc_chgd;

  /* Info for protocol generic command transaction.
  */
  mmoc_gen_cmd_trans_info_s_type       gen_prot_cmd;

  /* Info for Generic deactivated report transaction.
  */
  prot_deact_s_type                    gen_deact_rpt;

  /* Info for suspend SS command
  */
  mmoc_cmd_suspend_ss_s_type           suspend_ss;

} mmoc_trans_info_s_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type that associates the mode with the task that implements the mode.
*/
typedef struct {

   /* Main task that implements the above mode.
   */
   char*             task_name;

   /* Pointer to the protocol's sanity error function.
   */
   mmoc_sanity_err_f_type    *sanity_err_fn;

   /* Trans info.
   */
   prot_trans_type   trans_id;

} mmoc_mode_info_s_type;




#define MMOC_MAX_SESSION_IDS 6

/* A type for parameters that holds the state information of the MMoC task.
*/
typedef struct {

  /* Information related to the transaction being processed. Valid only
  ** when curr_trans != MMOC_TRANS_NULL.
  */
  mmoc_trans_info_s_type               trans_info;

  /* Timer to check sanity of MMoC.
  */
  rex_timer_type                       sanity_timer;

  /* Pointer to the transaction exit cleanup function.
  */
  mmoc_exit_fn_f_type                  *exit_fn_ptr;

  dword                                last_stop_req_sent_timestamp;

  /* Array used to store buffered auto deact ind */
  mmoc_rpt_msg_s_type                 *buffered_auto_deact;
  
  /* Current transaction that the MMoC is processing. Set to MMOC_TRANS_NULL
  ** if no transactions are being processed.
  */
  mmoc_trans_e_type                    curr_trans;
  
  /* Current state of the transaction that the MMoC is processing.
  */
  mmoc_trans_state_e_type              trans_state;


  /* Current protocol state corresponding to SS-Main and SS-HDR instances.
  */
  prot_state_e_type                    prot_state;

  /* Flag to indicate if SD was initialized with subscription
  ** information.
  */
  boolean                              is_sd_initialized;

  /* Flag to indicate if sd_init is called after starting of subs transaction
  */
  boolean                              is_sd_init_called;

  /* Last trans Id for the command issued by MMoC. Incremented after
  ** sending every acknowledgeable command.
  ** Cannot be set to MMOC_TRANS_AUTONOMOUS or MMOC_TRANS_PENDING.
  */
  prot_trans_type                      trans_id;

   /* Current GSM/WCDMA/LTE subscription availability status.
  */
  boolean                              is_gwl_subs_avail;


  /* Current operating mode status of the phone.
  */
  prot_oprt_mode_e_type                curr_oprt_mode;

  /* Flag to indicate if RESET_MODEM cmd is being processed.
  */
  sys_oprt_mode_e_type                 true_oprt_mode;

  /* Indicates if phone status chgd command to all supported protocols
  ** have already been sent.
  */
  boolean                              is_ph_stat_sent;

  /* Insanity count to sanity check if the MMoC is stuck in a transaction.
  */
  byte                                 insanity_count;

   


  

  /* Flag to indicate if phone is in standby sleep.
  */
  boolean                              is_standby_sleep;

  /* Enumeration of protocols that have a change in
  ** subscription available status
  */
  prot_subsc_chg_e_type                prot_subsc_chg;

  

  /* Sanity timer value depending on the standby preference
  */
  uint8                                max_sanity_time_multiple;

  /* Scan permission allow MDM/MSM to scan. This is used in
  ** SVLTE2
  */
  boolean                              is_scan_permission;
  

  /* Indicate if GWL deactivate command has been sent.
  */
  boolean                              is_gwl_deact_sent;

  /* stack suspend status
  */
  boolean                              is_suspend;


  /* Indicates whether AUTO DEACT IND is buffered or not */
  boolean                              is_buffer_auto_deact_ind;

  

   /*Last protocol state */
  prot_state_e_type                    main_last_prot_state;

  /* Bitmask of stacks suspended due to emergency call */
  uint8                                emerg_susp_ss_mask;

  

 } mmoc_state_info_s_type;


#ifdef MMODE_ADDITIONAL_DEBUG_INFO
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for reports queued per transaction in debug buffer
*/
typedef struct {

  /* Name of the report received
  */
   uint8                               rpt_name;

   /* Name of the task from where report is queued */
   uint8                               task_name;

  /* Protocol states for each of the stack when report is received
  */
   prot_state_e_type                   prot_state;

 

} mmoc_dbg_buf_rpt_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for transaction in mmoc_debug_buffer
*/
typedef struct {

  /* Transaction Id associated with the transaction.
  */
   prot_trans_type                     trans_id;

  /* Transaction name associated with the transaction
  */
   uint8                               trans_name;

  /* Additional information stored for certain transaction types
  */
   uint8                               addl_info[MMOC_DBG_BUF_TRANS_ADDL_INFO_SIZE];

  /* Set of reports received for the current transaction
  */
   mmoc_dbg_buf_rpt_s_type             rpt_queue[MMOC_MAX_RPT_COUNT];


} mmoc_dbg_buf_trans_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for mmoc debug buffer
*/
typedef struct {

  /* Debug buffer array
  */
   mmoc_dbg_buf_trans_s_type          dbg_buf[MMOC_MAX_DEBUG_BUFFER_SIZE];

   /* Current index in debug buffer
   */
   int8                               dbg_buf_idx;


} mmoc_debug_buffer_s_type;


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_dbg_buf_init_rpt_queue

DESCRIPTION
  Initialize the report queue for current transaction in debug_buffer
  This is done to ensure that reports last stored at the same index
  are cleared..

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                            mmoc_dbg_buf_init_rpt_queue
(
   uint8
);
#endif /* MMODE_ADDITIONAL_DEBUG_INFO */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_ss_main_timer_done_cb

DESCRIPTION
  This function is the callback procedure used by system determination when
  system selection timer expires.
  This function will queue the reselection timer expired generic command to
  the MMoC command queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void                            mmoc_ss_main_timer_done_cb
(

       int4                            ms_interval
          /*lint -esym(715,ms_interval)
          ** Have lint not complain about the ignored parameter 'ms_interval'
          ** which is specified to make this routine match the template for
          ** clk_Reg().
          */

);

/*===========================================================================

FUNCTION  mmoc_send_cm_rpt

DESCRIPTION
  This is a utility function called by the transaction state handlers to send
  the report to CM.

  Note! Only CM reports with no parameters can use this function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                            mmoc_send_cm_rpt
(
    cm_name_type                         rpt_name,
       /* Type of report to send.
       */

    const cm_sd_rpt_u_type               * mmoc_rpt_ptr
       /* Payload of report to send.
       */
);


/*===========================================================================

FUNCTION mmoc_update_prot_state

DESCRIPTION
  This function sets the MMoC's protocol state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void                            mmoc_update_prot_state
(

    prot_state_e_type                  new_prot_state,
       /* The new protocol state of the MMoC.
       */

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */

);
/*===========================================================================

FUNCTION mmoc_activate_prot_online_ss_main

DESCRIPTION
  This is a utility function called by the transaction state handlers to
  activate the protocol based on the SD20 action.
  It would set the MMoC's protocol state after activating the protocol based
  on main SS_instance.

  Note! For some SD actions( example power save ), new transactions have to
  be invoked. In those cases, this function will change the current
  transaction and return EVT_STATUS_NOT_CONSUMED.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion. No new
                                 transaction needs to be invoked.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the new transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
extern mmoc_evt_status_e_type          mmoc_activate_prot_online_ss_main
(
    sd_ss_act_e_type                   sd_act,
       /* Activate protocol based on this SD action.
       */

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
);

/* <EJECT> */
/*===========================================================================

FUNCTION get_mmoc_tcb

DESCRIPTION
  Gets pointer MMOC TCB.

DEPENDENCIES
  set_mmoc_tcb.

RETURN VALUE
  Pointer to MMOC TCB.

SIDE EFFECTS
  None.
===========================================================================*/
rex_tcb_type* get_mmoc_tcb(void);


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_task_init

DESCRIPTION
  This function is responsible for initializing the MMoC task before task
  start. This includes
  1. Initializing command and report queues
  2. Performing system determination initializing before task start.

DEPENDENCIES
  This should be called only before task start.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_task_init ( void );


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_event

DESCRIPTION
  This function will process the received event by calling the transaction
  handlers.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
extern void                            mmoc_process_event
(

       mmoc_state_info_s_type          *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
);

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_sanity_timer_exp

DESCRIPTION
  This function will check if a particular transaction is stuck in MMoC
  thereby validating the MMoC sanity.
  The function will do ERR_FATAL if it determines that the MMoC is insane.

  Note! This function should be called at the expiration of the sanity timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void                            mmoc_sanity_timer_exp
(

       mmoc_state_info_s_type          *mmoc_state_ptr
          /* Pointer to MMoC's state information.
          */

);



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_prot_gen_cmd_cnf

DESCRIPTION
  This function is used to acknowledge the generic protocol command.

  Note!  This is a local function and should be called by
  mmoc_proc_prot_gen_cmd.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_prot_gen_cmd_cnf
(

        boolean                        is_cmd_processed,
           /* If the protocols received the generic command in the incorrect
           ** state, then this will be set to FALSE.
           */

        prot_trans_type                trans_id,
           /* Protocols set this same as the trans_id received in the generic
           ** command. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
           */

        boolean                        is_prot_deactivating
          /* Is the protocol deactiavting because of processing the generic
          ** command. Valid when when is_cmd_processed = TRUE.
          */


);
/*===========================================================================

FUNCTION mmoc_send_rpt

DESCRIPTION
  This function takes an already filled out mmoc_rpt_msg_type and places it
  on the mmoc report queue.

DEPENDENCIES
  The MMoC task must have been started up already.  All of the items in
  the mmoc_rpt_msg_s_type must be already set.

RETURN VALUE
  None.

SIDE EFFECTS
  The mmoc_rpt_msg_s_type is placed on the MMoC report queue.  It must not be
  modified until the command has been processed.
===========================================================================*/
extern void   mmoc_send_rpt
(

       mmoc_rpt_msg_s_type             *msg_ptr
       /* Pointer to MMoC report message buffer.
       */

);

/*===========================================================================

FUNCTION mmoc_get_rpt_buf_else_err_fatal

DESCRIPTION
  Allocate a MMoC task report buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated MMoC report buffer.

SIDE EFFECTS
  none
===========================================================================*/
extern mmoc_rpt_msg_s_type*            mmoc_get_rpt_buf_else_err_fatal
(

       void

);

/*===========================================================================

FUNCTION mmoc_clear_transaction

DESCRIPTION
  This function clears the MMoC's transaction.

DEPENDENCIES
  None.

RETURN VALUE
 MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
 MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
extern mmoc_evt_status_e_type          mmoc_clear_transaction
(
       mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
);

/*===========================================================================

FUNCTION mmoc_update_trans_state

DESCRIPTION
  This function sets the MMoC's transaction state as per the value passed in.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void                               mmoc_update_trans_state
(
       mmoc_trans_state_e_type            new_trans_state,
       /* The new transaction state of the MMoC.
       */

       mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
);

/*===========================================================================
FUNCTION MMOC_INIT_AFTER_TASK_START

DESCRIPTION
  Initilize MMOC after task is started

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
extern void mmoc_init_after_task_start
(

       mmoc_state_info_s_type   *mmoc_state_info_ptr
       /* Pointer to MMoC's state information.
       */

);

/*===========================================================================

FUNCTION mmoc_process_common_auto_deact_ind_rpt

DESCRIPTION
 This function handles the common processing of an auto deactivation
 report when MMOC is waiting for other reports such as ph stat chgd cnf
 or deact_cnf.

  In this state, the state handler will
  1. Update the state
  2. Stay in same trans state, so that we can continue to wait for the desired
     report

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.

SIDE EFFECTS
  None.
===========================================================================*/
extern mmoc_evt_status_e_type         mmoc_process_common_auto_deact_ind_rpt
(

       const mmoc_rpt_msg_s_type      *rpt_ptr,
       /* Pointer to the report event that was received.
       ** if this was called because of report event, then
       ** rpt_ptr = null.
       */

       mmoc_state_info_s_type         *mmoc_info_ptr
       /* Pointer to MMoC's state information, may/may not be updated
       ** by the transaction state handlers.
       */

);
/*===========================================================================
=============================================================================
=============================================================================
============================== MMGSDI -> MMOC functional interface ==========
=============================================================================
=============================================================================
===========================================================================*/

/* The following section defines the API when MMOC-MMGSDI has to communicate
** to MMoC using the functional interface.
*/


/* <EJECT> */
#ifdef MMOC_DEBUG
#error code not present
#endif /* if (MMOC_DEBUG) */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_get_state_info_ptr

DESCRIPTION
  This function will return the pointer to the MMoC's state info struct.

DEPENDENCIES
  None.

RETURN VALUE
  mmoc_state_info_s_type* - Pointer to the local definition of MMoC's state
                            info struct.
  Note! Cannot be NULL.

SIDE EFFECTS
  None.
===========================================================================*/
extern  mmoc_state_info_s_type * mmoc_get_state_info_ptr(void);


/*===========================================================================

FUNCTION mmoc_is_featuremode

DESCRIPTION
  Checks featureMode
===========================================================================*/
boolean mmoc_is_featuremode(sys_overall_feature_t fmode);

#ifdef FEATURE_MMOC_UNIT_TEST
#error code not present
#endif


/*===========================================================================
FUNCTION mmoc_is_auto_deact_ind_rpt_buf

DESCRIPTION
 This function will check if auto_deact_ind is buffered for any stack.
 If yes, it will return stack index, else it will return SD_SS_MAX.

DEPENDENCIES
  None.

RETURN VALUE
  stack_idx - on which auto deact is buffered
  SD_SS_MAX - else

SIDE EFFECTS
  None.
===========================================================================*/
extern mmoc_evt_status_e_type  mmoc_buf_auto_deact_ind_rpt
(

       const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */


);
/*===========================================================================
FUNCTION mmoc_is_auto_deact_ind_rpt_buf

DESCRIPTION
 This function will check if auto_deact_ind is buffered for any stack.
 If yes, it will return stack index, else it will return SD_SS_MAX.

DEPENDENCIES
  None.

RETURN VALUE
  stack_idx - on which auto deact is buffered
  SD_SS_MAX - else

SIDE EFFECTS
  None.
===========================================================================*/
extern sd_ss_e_type mmoc_is_auto_deact_ind_rpt_buf
(

       mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */
);


/*===========================================================================
FUNCTION mmoc_is_auto_deact_ind_rpt_buf

DESCRIPTION
 This function will check if auto_deact_ind is buffered for particular ss.
 If yes, it return TRUE, else by default it will return FALSE .
 If is_discard is TRUE and buffered auto deact ind is found, it will
 discard that stored indication and set prot_state to NULL.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if auto_deact report was buffered for ss
  FALSE - else   

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean         mmoc_is_auto_deact_ind_rpt_buf_per_stack
(

       mmoc_state_info_s_type         *mmoc_info_ptr,
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

        boolean                        is_discard

);



#endif /* MMOCI_H */
