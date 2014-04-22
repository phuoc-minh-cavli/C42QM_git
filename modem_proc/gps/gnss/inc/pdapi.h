#ifndef PDAPI_H
#define PDAPI_H

/*===========================================================================

    Position Determination Session Manager Header File

DESCRIPTION
  This header file contains all the definitions necessary for
  clients to interface with the Position Determination module.

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2016 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/inc/pdapi.h#2 $
  $DateTime: 2021/02/01 07:20:20 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- . 
10/30/17   nath    Updated data type of loc_unc_horizontal,alongAxisUnc and perpAxisUnc to float.
09/14/17   sum     Added Macro PDAPI_NMEA_GPDTM 
03/22/17   sum     Added Macro PDAPI_NMEA_GAGNS
03/08/17   sum     Added a new field velHorU for velocity greater than 0x7fff.
02/17/17   skm     BLE and Barometer support for LPPE Phase 2 
12/09/16   mj      Remove "PDSM_PD_CMD_ERR_STATE_S"
10/28/16   muk     Enable QZSS meas and Poly data
10/26/16   nl      SET/ GET mismatch fix for nmea_ext_sentence_mask  
10/25/16   rk      Added position source provider type in PDSM external CPI struct to support
                   inconsistency in CPI.
08/01/16   gurdarsh  Add bit mask control for UAX3
05/25/16   mj      Add separate masks for health and IONO for GAL and BDS
04/27/16   ak      Added ENU velocity and uncertainity fields to pdsm_lite_pd_data_ext_s_type.
03/08/16   ss      Supporting emergency mode in CPI request. CPI cleanup
02/29/16   muk     Add support to output numClockResets
02/17/16   mj      Added NV support to enable/disable CPI injections and early exit logic 
11/11/15   ms      Add NHz Support
09/22/15   rk      Rename AGLO 3G UP position method type similar to AGPS (i.e. protocol centric)
09/16/15   skm     BDS Health delete fix 
09/07/15   jv      Removed obsolete comment for iode in pdsm_lite_GnssSvInfoType
07/30/15   js      Remove NMEA conflict between GAGGA and GSV_EXTENDED  
07/03/15   jv      Added QZSS Support
06/10/2015 muk     Added support for Secure Get Available Location
05/26/15   jv      sbas_user_preference changed from bool to uint8
05/19/15   ssu     Adding GM in memory logging.
05/22/15   skm     Added new DPO disengage reason
05/01/15   rh      Added get gps lock synchrous API for LBS
03/27/15   ssu     GM LOWI Integration
01/14/15   rk      Added LPP >24 AD NV support.
01/08/15   skm     Return End event (NO_SESS) when session end called, when there is not active session. 
09/15/14   jv      Added support for Galio GNSS
08/21/14   jv      Added support for 16-bit SVID
05/12/14   ah      Added support for Set XTRA Version Check
04/14/14   ah      Added support for DPO disengage reason for GSM
03/19/14   yy      Change prisoner sv to gnss sv blacklist
03/19/14   rh      Added PrisonerSV support 
03/10/14   ah      Added support for Set Premium Services Cfg & Get Avail WWAN Pos
01/18/14   msk     Added PA commands support for PDSM_PA_SUPL_UDP_ENABLE_CONFIG 
06/13/13   mj      Add BEST_AVAIL_POS into the status event mask define.
06/13/13   rh      Added pdsm_lite_GnssNavSvInfo in PDAPI for SV info
06/12/13   ah      Updated PDSM_GNSS_MAX_SATELLITES to include BDS (for SVs in view)
06/07/13   rh      Changed PA_DELETE bitmask to 64 bit for BDS support
05/01/13   ss      Added support for SUPL certificate operation with NO EFS access
04/01/13   ss      Added support to report SUPL Session End Indication
03/18/13   ss      Added support to report Network Initiated Session Reject Reason
12/19/12   rk      Added pdsm sensor (gyro/accel) bias parameter structure
12/04/12   rk      Extended SV parameters in the PD Fix Report structure
11/25/13   msk     Added SUPL end reasons to pdsm_lite_pd_end_e_type enum structure 
11/08/12   ssu     Added pdsm_lite_end_session_ex()
10/09/12   rh      Added a few error codes for IS801 sessions 
10/03/12   rh      Added a few PA commands for SUPL configuration  
08/22/12   rh      Added Cell Info and NI message injection API   
06/13/12   rk      Added new parameter 'PDSM_PA_ASST_GLONASS_POS_PROTOCOL' to get/set 
                   positioning protocols (RRLP, RRC, LPP) for A-Glonass U & C plane.
06/11/12   rk      Added new extended 'pdsm_lite_client_pa_reg_ex' function to register 
                   PDSM PA event parameter callbacks with PDSM.
04/18/12   ssu     Added pdsm_lite_pd_get_best_avail_pos() 
04/05/12   gk      Added floating point value for time unc in delete params API
03/29/12   ssu     Adding data type for engine DPO status report. 
02/22/12   rh      Added pdsm_lite_pd_inject_external_position()
02/16/12   gk      Added floating point value for altitude
12/06/11   jb      Added PDSM_PD_CMD_INJECT_MOTION_DATA command
10/06/11   rh      Added PDSM_PA_PE_CONFIG_PARAM command
06/02/11   rh      Added pdsm_lite_lcs_set_vx_config() and Get/Set SUPL security
04/13/11   rh      Added support for Get/Set SUPL version
04/08/11   ss      Added support for enabling/disabling data
04/08/11   ss      Added support for Get/Set PDCOMM APN profiles.
03/28/11   rh      Added heading untertainty, azimuth and elevation in rad.
02/16/11   rh      Added time untertainty gps_time_unc_ms as float value
02/10/11   ad      Added altitude source, altitude linkage (with horizontal position)
                   and altitude uncertainty coverage in position injection
01/07/11   vp      Added extended time source variable and values
12/01/10   ad      Added PDSM_PD_POSITION_SOURCE_EPI
11/08/10   ad      Added 2 new APIs for coarse position injection, 2 XTRA-T set/get parameters. Fix report
                   enhanced with 2 new reliability fields and 2 new sensor fields.
09/13/10   rh      Added new error codes for PDSM's PD/PA/LCS/XTRA/XTRA-T commands
                   Added 0xF0/0xF1 values to support DPO Control update without NV change
07/26/10   sj      Added PDSM_PD_HEADING_VALID flag to indicate valid heading for position fix,
                   Added new COMM protocol type, PDSM_PD_COMM_PROTOCOL_1X_CP.
12/07/09   kc      Added new XSPI event and related api
09/01/09   gk      Added comments for EFS_DATA operation
04/02/10   gk      Added an event callback to inject position
01/11/10   rb      Add a flag to indicate that UTC time is valid in the position structure

12/07/09   gk      Added comments for position strucutres, unified header files 
                   accross all branches 4.1 and higher
10/26/09   gk      Added comments for position strucutres, addes SENSORS client
10/09/09   rb      Added Operation Mode for Cell ID
09/01/09   gk      Added comments for EFS_DATA operation
08/23/09   gk      Added new event for guard timer expiry and MM ind
08/10/09   gk      Added new validity flags in the position report
07/23/09   gk      Added DELETE_SV_NO_EXIST flag for delete params
07/23/09   gk      Added new END reason
05/21/09   gk      Add new defines for position modes
05/14/09   gk      fixed some typos
02/10/09   jd      Added WPS fix and scan log reporting
02/03/09   atien   Support MOLR vs AD call flow
                   Remove unused client_data_ptr in pdsm_lite_xtra_t_get_celldb_report_s_type
01/02/09   atien   Support XTRA-T
01/05/09   ns      Add WPS Event
06/15/08   kj      De-Featurize this Public API header file
06/12/08   lt      Support for external coarse position injection.
04/26/08   gk      On-Demand Changes
11/13/07   gk      SUPL certivicate upload via pdapi
09/14/07   gk      NMEA CONFIG set parameters support
07/20/07   gk      Removed obsoleted functions
06/26/07   rw      XTRA related changes
06/18/07   ld      Added support for Deleting Cell DB Information.
05/27/07   rw      Additonal support for JGPS (KDDI) protocol
05/23/07   gk      removed multiple definition of msec.
05/09/07   gk      KDDI related changes.
05/01/07   lt      Restored mo_method enum. back to MO_CP and MO_UP.
04/25/07   gk      Added security and WM related changes.
04/16/07   gk      Renumbered delete params.
04/17/07   lt      Change to expand mo_method enum to include request type of LocEst and AssistData.
04/16/07   gk      Renumbered delete params.
04/02/07   gk      Added Clock  injection API, PDE network address
03/02/07   gk      Added SUPL QOP to the notify verify and VX changes
02/15/07   gk      fixed HTORPC errors for dual proc
02/14/07   gk      Made flags for SUPL and UMTS_CP as 16 bytes 
02/06/07   gk      Modified SUPL notify verify, fixed lint errors 
01/19/07   gk      Fixed HTORPC syntax errors 
01/11/07   gk      Changed Delete parms to include more info 
12/06/06   gk      Changed LCS API, measurement report changes
12/04/06   gk      Added LCS API and minor mods added some of the deleted OR stuff, code review changes
11/08/06   gk      PDAPI for converged GPS
===========================================================================*/


#include "pdapi_lite.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define PDSM_API_MAJ_V PDSM_LITE_API_MAJ_V 
#define PDSM_API_MIN_V PDSM_LITE_API_MIN_V /*<<--- QZSS:ATTENTION! Merge version number properly*/

/* ========================================================================
** ================ PDSM Client Interface Block ===========================
** ========================================================================
*/

/**
 * @brief Minimum TBF value for fix rate parameters
 *
 * This value is used as a lower limit for TBF parameter.
 *
 * @sa pdsm_lite_pd_fix_rate_s_type
 */
#define PDSM_MINIMUM_TBF_MS PDSM_LITE_MINIMUM_TBF_MS /* 100 milliseconds */


/* Client ID type to differentiate PDSM client objects from one another.
*/
typedef pdsm_lite_client_id_type pdsm_client_id_type;

/*Dummy type for MGP compilation fix*/
typedef uint8 pdsm_pd_motion_data_s_type;

#define PDSM_CLIENT_TYPE_NONE PDSM_LITE_CLIENT_TYPE_NONE		/* FOR INTERNAL USE OF PDSM ONLY! */

#define PDSM_CLIENT_TYPE_TEST2        PDSM_LITE_CLIENT_TYPE_TEST2   /* Test client types, Added/Deleted in future */
#define PDSM_CLIENT_TYPE_OEM          PDSM_LITE_CLIENT_TYPE_OEM     /* Reserved for OEM Clients */
#define PDSM_CLIENT_TYPE_XTRA         PDSM_LITE_CLIENT_TYPE_XTRA    /* Reserved for gpsOneXTRA Clients */
#define PDSM_CLIENT_TYPE_XTRA_T       PDSM_LITE_CLIENT_TYPE_XTRA_T  /* Reserved for gpsOneXTRA-T Clients */
#define PDSM_CLIENT_TYPE_PDA          PDSM_LITE_CLIENT_TYPE_PDA     /* Reserver for LocMW */
#define  PDSM_CLIENT_TYPE_MAX         PDSM_LITE_CLIENT_TYPE_MAX     /* FOR INTERNAL USE OF PDSM ONLY! */

#define PDSM_CLIENT_TYPE_TEST2_LEGACY_VAL PDSM_LITE_CLIENT_TYPE_TEST2_LEGACY_VAL 
                                                  /*To aid diag testing previous 
                                                  client type value used by legacy script
                                                  will be translated to new value
                                                  */
#define PDSM_CLIENT_TYPE_E_SIZE PDSM_LITE_CLIENT_TYPE_E_SIZE  /* To force enum to int */

typedef pdsm_lite_client_type_e_type pdsm_client_type_e_type;

/* Valid Service type mask */
/*
** Option types for Position Determination.
** Option type is not used as bitmask. Since user is registered he will anyway be
** informed about other PD events asynchronously even if he hasn't called the fnc.
*/
#define PDSM_PD_SESS_TYPE_MIN PDSM_LITE_PD_SESS_TYPE_MIN /* Internal use of PDSM */
#define PDSM_PD_SESS_TYPE_NEW PDSM_LITE_PD_SESS_TYPE_NEW /* Get new position */
#define PDSM_PD_SESS_TYPE_TRACK_IND PDSM_LITE_PD_SESS_TYPE_TRACK_IND
#define PDSM_PD_SESS_TYPE_MAX PDSM_LITE_PD_SESS_TYPE_MAX /* For internal use of PDSM */
#define PDSM_PD_SESS_TYPE_E_SIZE PDSM_LITE_PD_SESS_TYPE_E_SIZE  /* Fix the enum size to int */

typedef  pdsm_lite_pd_session_e_type pdsm_pd_session_e_type;

/*The reliability indicator is used to show the probability of a position 
  outlier. �Position outlier� is defined as the truth position being grossly 
  inconsistent with the Gaussian-model uncertainty parameters � e.g. not within
  10-standard deviations. A higher reliability metric means lower  
  probability of having a position outlier. Basically, this indicator tells, to 
  what extent the position can be trusted.  
  (Four levels of the reliability metric are defined).

  Benefit: Together with other information in the fix report such as position 
  uncertainty, the reliability indicator provides additional information.  This 
  allows a user of this information to make better use of a given position. For 
  example, a very low reliability output may be used simply for pre-loading 
  likely-needed-soon map information tiles, but not for end-user display.  
  Additionally, if external positioning sources are available, the reliability
  of the GNSS solution (cross-check with injected position if available) may be
  used for other system level comparisons.

  Definition of cross-check: Using redundant information to verify a position
  fix. Information here includes GNSS measurements, externally provided 
  positions, and any other input that can help the positioning process.

  How to set the reliability indicator[input]: It is suggested that reliability
  indicator is not used by customers unless they have an advanced usage plan
  for this input or output.  Pre-existing GPS system performance will remain
  the same if this feature is not used (set to 
  PDSM_LITE_POSITION_RELIABILITY_NOT_SET).
*/
#define PDSM_POSITION_RELIABILITY_NOT_SET PDSM_LITE_POSITION_RELIABILITY_NOT_SET /* Not set, reliability info is not present */
#define PDSM_POSITION_RELIABILITY_VERY_LOW PDSM_LITE_POSITION_RELIABILITY_VERY_LOW /* Not checked, use at user's own risk */
#define PDSM_POSITION_RELIABILITY_LOW PDSM_LITE_POSITION_RELIABILITY_LOW  /* Limited cross-checked */
#define PDSM_POSITION_RELIABILITY_MEDIUM PDSM_LITE_POSITION_RELIABILITY_MEDIUM		/* Medium level cross-checked */
#define PDSM_POSITION_RELIABILITY_HIGH PDSM_LITE_POSITION_RELIABILITY_HIGH 		/* Robust cross-checked */
#define PDSM_POSITION_RELIABILITY_MAX PDSM_LITE_POSITION_RELIABILITY_MAX  /* Forced to 32 bit */

typedef pdsm_lite_pd_reliability_e_type pdsm_pd_reliability_e_type;

/* Altitude source of injected position
*/
#define PDSM_ALTITUDE_SOURCE_UNKNOWN PDSM_LITE_ALTITUDE_SOURCE_UNKNOWN 
#define PDSM_ALTITUDE_SOURCE_GPS PDSM_LITE_ALTITUDE_SOURCE_GPS 
#define PDSM_ALTITUDE_SOURCE_CID PDSM_LITE_ALTITUDE_SOURCE_CID 
#define PDSM_ALTITUDE_SOURCE_TERRESTRIAL PDSM_LITE_ALTITUDE_SOURCE_TERRESTRIAL
#define PDSM_ALTITUDE_SOURCE_TERRESTRIAL_HYBRID PDSM_LITE_ALTITUDE_SOURCE_TERRESTRIAL_HYBRID
#define PDSM_ALTITUDE_SOURCE_ALTITUDE_DATABASE PDSM_LITE_ALTITUDE_SOURCE_ALTITUDE_DATABASE
#define PDSM_ALTITUDE_SOURCE_BAROMETRIC_ALTIMETER PDSM_LITE_ALTITUDE_SOURCE_BAROMETRIC_ALTIMETER 
#define PDSM_ALTITUDE_SOURCE_OTHER PDSM_LITE_ALTITUDE_SOURCE_OTHER
#define PDSM_ALTITUDE_SOURCE_MAX PDSM_LITE_ALTITUDE_SOURCE_MAX /* Forced to 32 bit */

typedef pdsm_lite_pd_altitude_source_e_type pdsm_pd_altitude_source_e_type;

/* Horizontal position and altitude source linkage.
   This field specifies the dependence between the horizontal
   position and altitude components of injected position.
   This information provides the ability to the position
   engine to remove the bad components of position independently
   */
#define PDSM_ALT_HOR_LINK_NOT_SPECIFIED PDSM_LITE_ALT_HOR_LINK_NOT_SPECIFIED 
#define PDSM_ALT_HOR_LINK_FULLY_INTERDEPENDENT PDSM_LITE_ALT_HOR_LINK_FULLY_INTERDEPENDENT 
#define PDSM_ALT_HOR_LINK_ALT_DEP_LAT_LONG PDSM_LITE_ALT_HOR_LINK_ALT_DEP_LAT_LONG 
#define PDSM_ALT_HOR_LINK_FULLY_INDEPENDENT PDSM_LITE_ALT_HOR_LINK_FULLY_INDEPENDENT 
#define PDSM_ALT_HOR_LINK_MAX PDSM_LITE_ALT_HOR_LINK_MAX /* Forced to 32 bit */

typedef pdsm_lite_pd_altitude_horizontal_linkage_e_type pdsm_pd_altitude_horizontal_linkage_e_type;

/* Altitude uncertainty coverage
   Point uncertainty means that the altitude 
   uncertainty is valid only at the injected horizontal
   position coordinates. Full uncertainty mean that the
   altitude uncertainty applies to the device regardless
   of the horizontal position (within the horizontal 
   uncertainty region if provided).
*/
#define PDSM_ALT_COVERAGE_NOT_SPECIFIED PDSM_LITE_ALT_COVERAGE_NOT_SPECIFIED 
#define PDSM_ALT_COVERAGE_POINT_UNC PDSM_LITE_ALT_COVERAGE_POINT_UNC 
#define PDSM_ALT_COVERAGE_FULL_UNC PDSM_LITE_ALT_COVERAGE_FULL_UNC
#define PDSM_ALT_COVERAGE_MAX PDSM_LITE_ALT_COVERAGE_MAX /* Forced to 32 bit */

typedef pdsm_lite_pd_altitude_unc_coverage_e_type pdsm_pd_altitude_unc_coverage_e_type;
/* define for continuous mode.
 * @brief Fix rate parameters
 *
 * Parameters include total number of fixes and TBF value in milliseconds.
 *
 * @sa PDSM_LITE_MINIMUM_TBF_MS
 */
typedef  pdsm_lite_pd_fix_rate_s_type pdsm_pd_fix_rate_s_type;
// TODO: <Nath>:see if enum required for a single enum
#define PDSM_SESSION_OPERATION_MIN PDSM_LITE_SESSION_OPERATION_MIN,
#define PDSM_SESSION_OPERATION_STANDALONE_ONLY PDSM_LITE_SESSION_OPERATION_STANDALONE_ONLY  
#define PDSM_SESSION_OPERATION_MAX PDSM_LITE_SESSION_OPERATION_MAX 
typedef pdsm_lite_pd_session_operation_e_type pdsm_pd_session_operation_e_type;
/* Typedef structure used by PDSM */
typedef pdsm_lite_pd_option_s_type pdsm_pd_option_s_type;
#define PDSM_NO_ACCURACY_THRESHOLD PDSM_LITE_NO_ACCURACY_THRESHOLD    /* Threshold users can use so that PDSM ignores the check */
#define PDSM_MIN_ACCURACY_THRESHOLD PDSM_LITE_MIN_ACCURACY_THRESHOLD        /* At least 1m should be specified */

/* QOS to be used for PD sessions.
*/
typedef pdsm_lite_pd_qos_type pdsm_pd_qos_type;

/* ------------------------------------------------------------------------
** End of type declarations for position determination session command.
** -------------------------------------------------------------------------
*/

/* Enumeration of commands PDSM.
*/
#define PDSM_PD_CMD_NONE PDSM_LITE_PD_CMD_NONE  		   /* FOR INTERNAL USE OF PDSM ONLY! */
#define PDSM_PD_CMD_GET_POS PDSM_LITE_PD_CMD_GET_POS			  /* Get postion Command */
#define PDSM_PD_CMD_END_SESSION PDSM_LITE_PD_CMD_END_SESSION		  /* End PD session */
#define PDSM_PD_CMD_INJECT_TIME PDSM_LITE_PD_CMD_INJECT_TIME		  /* Inject External time */
#define PDSM_PD_CMD_INJECT_POS PDSM_LITE_PD_CMD_INJECT_POS		  /* Inject external position */
#define PDSM_PD_CMD_GET_GPS_STATE PDSM_LITE_PD_CMD_GET_GPS_STATE 	  /* Get the GPS State information */
#define PDSM_PD_CMD_GET_BEST_AVAIL_POS PDSM_LITE_PD_CMD_GET_BEST_AVAIL_POS	 /* Get the best available position command */
#define PDSM_PD_CMD_END_SESSION_EX PDSM_LITE_PD_CMD_END_SESSION_EX	   /* Extended End PD session */
#define PDSM_PD_CMD_INJECT_CELL_INFO  PDSM_LITE_PD_CMD_INJECT_CELL_INFO
#define PDSM_PD_CMD_GET_AVAIL_WWAN_POS PDSM_LITE_PD_CMD_GET_AVAIL_WWAN_POS
#define PDSM_PD_CMD_MAX PDSM_LITE_PD_CMD_MAX 			  /* FOR INTERNAL USE OF PDSM ONLY! */
#define PDSM_CMD_E_SIZE PDSM_LITE_CMD_E_SIZE 	/* To force enum to 32 bit for windows NT */

typedef pdsm_lite_pd_cmd_e_type pdsm_pd_cmd_e_type;
/*~ SENTINEL pdsm_lite_pd_cmd_e_type.PDSM_LITE_PD_CMD_MAX */

/* Type declarations for Set/Get parameters */

/* Enum defined for allowing different levels of GPS locks.
*/
/*Remove if not used by tm_xtra*/

#define  PDSM_FS_CREATE_WRITE_FILE PDSM_LITE_FS_CREATE_WRITE_FILE
#define  PDSM_FS_APPEND_FILE PDSM_LITE_FS_APPEND_FILE
#define  PDSM_FS_DELETE_FILE PDSM_LITE_FS_DELETE_FILE
#define  PDSM_FS_READ_FILE PDSM_LITE_FS_READ_FILE
#define  PDSM_FS_PURGE_DIR PDSM_LITE_FS_PURGE_DIR /* Delete all files from a directory */
#define  PDSM_FS_DELETE_FILE_LIST PDSM_LITE_FS_DELETE_FILE_LIST /* Delete specified files from a directory */


#define PDSM_PD_FS_MAX_PATH_LEN_BYTES  PDSM_LITE_PD_FS_MAX_PATH_LEN_BYTES   /* Max file name length in bytes that can be written*/
#define PDSM_PD_EFS_MAX_FILE_LEN_BYTES PDSM_LITE_PD_EFS_MAX_FILE_LEN_BYTES /* Max file size in bytes that can be written*/


#define PDSM_PA_NONE PDSM_LITE_PA_NONE /* Internal range */
#define PDSM_PA_GPS_LOCK PDSM_LITE_PA_GPS_LOCK			 /* GPS LOCK */
#define PDSM_PA_EFS_DATA PDSM_LITE_PA_EFS_DATA,
#define PDSM_PA_DPO_CONTROL PDSM_LITE_PA_DPO_CONTROL		 /* DPO Control */
#define PDSM_PA_XTRAT_CLIENT_TOKEN PDSM_LITE_PA_XTRAT_CLIENT_TOKEN /* Set XTRA-T Client token */
#define PDSM_PA_XTRAT_USER_SESSION_CONTROL PDSM_LITE_PA_XTRAT_USER_SESSION_CONTROL /* Used to set the XTRA-T User session control */
#define PDSM_PA_PE_CONFIG_PARAM PDSM_LITE_PA_PE_CONFIG_PARAM			/* PE config parameter */
#define PDSM_PA_DELETE_PARAMS_EX1 PDSM_LITE_PA_DELETE_PARAMS_EX1 		/* delete GPS / GNSS params from database */
#define PDSM_PA_UAX3_CONFIG PDSM_LITE_PA_UAX3_CONFIG
#define PDSM_PA_XTRA_APN_PROFILE PDSM_LITE_PA_XTRA_APN_PROFILE
#define PDSM_PA_XTRA_VERSION_CHECK PDSM_LITE_PA_XTRA_VERSION_CHECK		/* XTRA Version Check */
#define PDSM_PA_GTP_WWAN_FIX_TIMER_CONFIG PDSM_LITE_PA_GTP_WWAN_FIX_TIMER_CONFIG  /*Configer duration of timer used while waiting for GTP WWAN FIX*/
#define PDSM_PA_BYPASS_SYS_UNC_CHECK PDSM_LITE_PA_BYPASS_SYS_UNC_CHECK 
#define PDSM_PA_MAX PDSM_LITE_PA_MAX						/* Internal use. */
#define PDSM_PA_SIZE PDSM_LITE_PA_SIZE/* Fix the size as int */

/* typedef for parameter set mask */
typedef pdsm_lite_pa_e_type pdsm_pa_e_type;
/*~ SENTINEL pdsm_lite_pa_e_type.PDSM_LITE_PA_MAX */

#define PDSM_GPS_LOCK_MIN PDSM_LITE_GPS_LOCK_MIN
#define PDSM_GPS_LOCK_NONE PDSM_LITE_GPS_LOCK_NONE
#define PDSM_GPS_LOCK_MI PDSM_LITE_GPS_LOCK_MI
#define PDSM_GPS_LOCK_MT PDSM_LITE_GPS_LOCK_MT
#define PDSM_GPS_LOCK_ALL PDSM_LITE_GPS_LOCK_ALL
#define PDSM_GPS_LOCK_MAX PDSM_LITE_GPS_LOCK_MAX

typedef pdsm_lite_gps_lock_e_type pdsm_gps_lock_e_type;






/* sets the specified bit in a u32 bit mask. bit counts range: 0-31 */
#define PDSM_SET_U32BITMASK_BIT PDSM_LITE_SET_U32BITMASK_BIT

/* sets the specified bit in a u64 bit mask. bit counts range: 0-63 */
#define PDSM_SET_U64BITMASK_BIT PDSM_LITE_SET_U64BITMASK_BIT

#define PDSM_PA_DELETE_BITMASK PDSM_LITE_PA_DELETE_BITMASK
/* the following flags, where no GNSS system is specified, 
   affect only GPS, unless mentioned otherwise. E.g.
   PDSM_LITE_PA_DELETE_EPH would only delete GPS ephemeris. 

   See also: C_DELETE_INFO_xxx flags used by MGP API
*/
#define PDSM_PA_DELETE_EPH    PDSM_LITE_PA_DELETE_BITMASK(0) /*0x0001*/
#define PDSM_PA_DELETE_ALM    PDSM_LITE_PA_DELETE_BITMASK(1) /*0x0002*/
#define PDSM_PA_DELETE_POS    PDSM_LITE_PA_DELETE_BITMASK(2) /*0x0004*/
#define PDSM_PA_DELETE_TIME   PDSM_LITE_PA_DELETE_BITMASK(3) /*0x0008*/
#define PDSM_PA_DELETE_IONO   PDSM_LITE_PA_DELETE_BITMASK(4) /*0x0008*/

/* PDSM_LITE_PA_DELETE_TIME  will delete all GNSS time For finer control 
   of individual clock elements, use PDSM_LITE_PA_DELETE_CLOCK_INFO instead  */
#define PDSM_PA_DELETE_UTC    PDSM_LITE_PA_DELETE_BITMASK(5) /*0x0020*/
#define PDSM_PA_DELETE_HEALTH PDSM_LITE_PA_DELETE_BITMASK(6) /*0x0040*/
#define PDSM_PA_DELETE_SVDIR  PDSM_LITE_PA_DELETE_BITMASK(7) /*0x0080*/
#define PDSM_PA_DELETE_SVSTEER PDSM_LITE_PA_DELETE_BITMASK(8) /*0x0100*/
#define PDSM_PA_DELETE_SADATA PDSM_LITE_PA_DELETE_BITMASK(9) /*0x0200*/
#define PDSM_PA_DELETE_RTI    PDSM_LITE_PA_DELETE_BITMASK(10) /*0x0400*/

#define PDSM_PA_TIME_UNC      PDSM_LITE_PA_DELETE_BITMASK(11) /*0x0800*/ /* this will modify all GNSS time unc */
#define PDSM_PA_POS_UNC       PDSM_LITE_PA_DELETE_BITMASK(12) /*0x1000*/
#define PDSM_PA_TIME_OFFSET   PDSM_LITE_PA_DELETE_BITMASK(13) /*0x2000*/
#define PDSM_PA_POS_OFFSET    PDSM_LITE_PA_DELETE_BITMASK(14) /*0x4000*/

#define PDSM_PA_DELETE_ALM_CORR    PDSM_LITE_PA_DELETE_BITMASK(16) /*0x10000*/
#define PDSM_PA_DELETE_FREQ_BIAS_EST PDSM_LITE_PA_DELETE_BITMASK(17) /*0x20000*/
#define PDSM_PA_DELETE_SV_NO_EXIST  PDSM_LITE_PA_DELETE_BITMASK(18) /*0x40000*/
/* additional constellation flags */
#define PDSM_PA_DELETE_EPH_GLO   PDSM_LITE_PA_DELETE_BITMASK(19)
#define PDSM_PA_DELETE_EPH_SBAS   PDSM_LITE_PA_DELETE_BITMASK(20)


#define PDSM_PA_DELETE_ALM_GLO   PDSM_LITE_PA_DELETE_BITMASK(21)
#define PDSM_PA_DELETE_ALM_SBAS  PDSM_LITE_PA_DELETE_BITMASK(22)

#define PDSM_PA_DELETE_SVDIR_GLO    PDSM_LITE_PA_DELETE_BITMASK(23)
#define PDSM_PA_DELETE_SVDIR_SBAS   PDSM_LITE_PA_DELETE_BITMASK(24)


#define PDSM_PA_DELETE_SVSTEER_GLO     PDSM_LITE_PA_DELETE_BITMASK(25)
#define PDSM_PA_DELETE_SVDIR_SBAS      PDSM_LITE_PA_DELETE_BITMASK(24)

#define PDSM_PA_DELETE_ALM_CORR_GLO    PDSM_LITE_PA_DELETE_BITMASK(27)

#define PDSM_PA_DELETE_TIME_GPS   PDSM_LITE_PA_DELETE_BITMASK(28)
#define PDSM_PA_DELETE_TIME_GLO   PDSM_LITE_PA_DELETE_BITMASK(29)

#define PDSM_PA_DELETE_CLOCK_INFO PDSM_LITE_PA_DELETE_BITMASK(30) /* indicates delete_clock_info_mask
                                                                 is valid. see PDSM_LITE_PA_DELETE_CLOCK_INFOxxx*/

#define PDSM_PA_DELETE_TIME_BDS      PDSM_LITE_PA_DELETE_BITMASK(31)
#define PDSM_PA_DELETE_EPH_BDS       PDSM_LITE_PA_DELETE_BITMASK(32)
#define PDSM_PA_DELETE_ALM_BDS       PDSM_LITE_PA_DELETE_BITMASK(33)
#define PDSM_PA_DELETE_SVDIR_BDS     PDSM_LITE_PA_DELETE_BITMASK(34)
#define PDSM_PA_DELETE_SVSTEER_BDS   PDSM_LITE_PA_DELETE_BITMASK(35)
#define PDSM_PA_DELETE_ALM_CORR_BDS  PDSM_LITE_PA_DELETE_BITMASK(36)

#define PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GPS  PDSM_LITE_PA_DELETE_BITMASK(37)
#define PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GLO  PDSM_LITE_PA_DELETE_BITMASK(38)
#define PDSM_PA_DELETE_GNSS_SV_BLACKLIST_BDS  PDSM_LITE_PA_DELETE_BITMASK(39)
#define PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GAL  PDSM_LITE_PA_DELETE_BITMASK(40)
#define PDSM_PA_DELETE_TIME_GAL               PDSM_LITE_PA_DELETE_BITMASK(41)
#define PDSM_PA_DELETE_EPH_GAL                PDSM_LITE_PA_DELETE_BITMASK(42)
#define PDSM_PA_DELETE_ALM_GAL                PDSM_LITE_PA_DELETE_BITMASK(43)
#define PDSM_PA_DELETE_SVDIR_GAL              PDSM_LITE_PA_DELETE_BITMASK(44)
#define PDSM_PA_DELETE_SVSTEER_GAL            PDSM_LITE_PA_DELETE_BITMASK(45)
#define PDSM_PA_DELETE_ALM_CORR_GAL           PDSM_LITE_PA_DELETE_BITMASK(46)
#define PDSM_PA_DELETE_GNSS_SV_BLACKLIST_QZSS PDSM_LITE_PA_DELETE_BITMASK(47)

#define PDSM_PA_DELETE_IONO_QZSS              PDSM_LITE_PA_DELETE_BITMASK(48)
#define PDSM_PA_DELETE_EPH_QZSS               PDSM_LITE_PA_DELETE_BITMASK(49)
#define PDSM_PA_DELETE_ALM_QZSS               PDSM_LITE_PA_DELETE_BITMASK(50)
#define PDSM_PA_DELETE_SVDIR_QZSS             PDSM_LITE_PA_DELETE_BITMASK(51)
#define PDSM_PA_DELETE_SVSTEER_QZSS           PDSM_LITE_PA_DELETE_BITMASK(52)
#define PDSM_PA_DELETE_ALM_CORR_QZSS          PDSM_LITE_PA_DELETE_BITMASK(53)
#define PDSM_PA_DELETE_HEALTH_QZSS            PDSM_LITE_PA_DELETE_BITMASK(54)

#define PDSM_PA_DELETE_IONO_BDS               PDSM_LITE_PA_DELETE_BITMASK(55)
#define PDSM_PA_DELETE_IONO_GAL               PDSM_LITE_PA_DELETE_BITMASK(56)

/* Deletes Glonass SV health info for all GLONASS satellites*/
#define PDSM_PA_DELETE_HEALTH_GLO    PDSM_LITE_PA_DELETE_BITMASK(57)
/* Deletes BDS SV health info for all BDS satellites*/
#define PDSM_PA_DELETE_HEALTH_BDS    PDSM_LITE_PA_DELETE_BITMASK(58)
/* Deletes GAL SV health info for all GAL satellites*/
#define PDSM_PA_DELETE_HEALTH_GAL    PDSM_LITE_PA_DELETE_BITMASK(59)

#define PDSM_PA_DELETE_ALL PDSM_LITE_PA_DELETE_ALL

#define PDSM_PA_DELETE_CLOCK_INFO_TIME_EST                 PDSM_LITE_SET_U32BITMASK_BIT(0) /* deletes all GNSS time */
#define PDSM_PA_DELETE_CLOCK_INFO_FREQ_EST                 PDSM_LITE_SET_U32BITMASK_BIT(1)
#define PDSM_PA_DELETE_CLOCK_INFO_WEEK_NUMBER              PDSM_LITE_SET_U32BITMASK_BIT(2)
#define PDSM_PA_DELETE_CLOCK_INFO_RTC_TIME                 PDSM_LITE_SET_U32BITMASK_BIT(3)
#define PDSM_PA_DELETE_CLOCK_INFO_TIME_TRANSFER            PDSM_LITE_SET_U32BITMASK_BIT(4)
#define PDSM_PA_DELETE_CLOCK_INFO_GPSTIME_EST              PDSM_LITE_SET_U32BITMASK_BIT(5)
#define PDSM_PA_DELETE_CLOCK_INFO_GLOTIME_EST              PDSM_LITE_SET_U32BITMASK_BIT(6)
#define PDSM_PA_DELETE_CLOCK_INFO_GLODAY_NUMBER            PDSM_LITE_SET_U32BITMASK_BIT(7)
#define PDSM_PA_DELETE_CLOCK_INFO_GLO4YEAR_NUMBER          PDSM_LITE_SET_U32BITMASK_BIT(8)
#define PDSM_PA_DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY         PDSM_LITE_SET_U32BITMASK_BIT(9)
#define PDSM_PA_DELETE_CLOCK_INFO_DISABLE_TT               PDSM_LITE_SET_U32BITMASK_BIT(10)
#define PDSM_PA_DELETE_CLOCK_INFO_GG_LEAPSEC               PDSM_LITE_SET_U32BITMASK_BIT(11)
#define PDSM_PA_DELETE_CLOCK_INFO_GG_GGTB                  PDSM_LITE_SET_U32BITMASK_BIT(12)
#define PDSM_PA_DELETE_CLOCK_INFO_BDSTIME_EST              PDSM_LITE_SET_U32BITMASK_BIT(13)
#define PDSM_PA_DELETE_CLOCK_INFO_GB_GBTB                  PDSM_LITE_SET_U32BITMASK_BIT(14)
#define PDSM_PA_DELETE_CLOCK_INFO_BG_BGTB                  PDSM_LITE_SET_U32BITMASK_BIT(15) /* Beidou to Glonass time bias-related */
#define PDSM_PA_DELETE_CLOCK_INFO_BDSWEEK_NUMBER           PDSM_LITE_SET_U32BITMASK_BIT(16)
#define PDSM_PA_DELETE_CLOCK_INFO_BDS_RF_GRP_DELAY         PDSM_LITE_SET_U32BITMASK_BIT(17)
#define PDSM_PA_DELETE_CLOCK_INFO_GPS_GAL_TB               PDSM_LITE_SET_U32BITMASK_BIT(18)
#define PDSM_PA_DELETE_CLOCK_INFO_GLO_GAL_TB               PDSM_LITE_SET_U32BITMASK_BIT(19)
#define PDSM_PA_DELETE_CLOCK_INFO_GAL_BDS_TB               PDSM_LITE_SET_U32BITMASK_BIT(20)

#define PDSM_PA_DELETE_ALL_CLOCK_INFO PDSM_LITE_PA_DELETE_ALL_CLOCK_INFO
     

typedef pdsm_lite_delete_parms_type pdsm_delete_parms_type;

/*******************************************************************************
* UE supported position method types (a.k.a positioning modes)                 *
*******************************************************************************/


/* GPS position method types */
/* UP MSBased/MSAssisted/Autonomous bits */
#define PDSM_POS_MODE_STANDALONE       PDSM_LITE_POS_MODE_STANDALONE  /* GNSS Standalone */

#define PDSM_PA_PE_CONFIG_PARAM_MASK_USE_INJECTED_POSITION  PDSM_LITE_PA_PE_CONFIG_PARAM_MASK_USE_INJECTED_POSITION
#define PDSM_PA_PE_CONFIG_PARAM_MASK_FILTER_SV_USED PDSM_LITE_PA_PE_CONFIG_PARAM_MASK_FILTER_SV_USED
#define PDSM_PA_PE_CONFIG_PARAM_MASK_REPORT_ACCURATE_SVDIR    PDSM_LITE_PA_PE_CONFIG_PARAM_MASK_REPORT_ACCURATE_SVDIR
typedef pdsm_lite_pa_pe_config_s_type pdsm_pa_pe_config_s_type;

typedef pdsm_lite_efs_data_s_type pdsm_efs_data_s_type;

/** UAX3 Operations */
#define PDSM_PA_UAX3_MASK_ENABLE  PDSM_LITE_PA_UAX3_MASK_ENABLE
#define PDSM_PA_UAX3_MASK_ALL     PDSM_LITE_PA_UAX3_MASK_ALL

/* configuration for XTRA Version Check */
#define PDSM_PA_XTRA_VERSION_CHECK_DISABLED PDSM_LITE_PA_XTRA_VERSION_CHECK_DISABLED /* XTRA Version Check Disabled (not required) */
#define PDSM_PA_XTRA_VERSION_CHECK_AUTO PDSM_LITE_PA_XTRA_VERSION_CHECK_AUTO	  /* XTRA Version Check Automatic (based on pre-provisioned XTRA version config) */
#define PDSM_PA_XTRA_VERSION_CHECK_XTRA2 PDSM_LITE_PA_XTRA_VERSION_CHECK_XTRA2	  /* XTRA Version Check with XTRA2 format */
#define PDSM_PA_XTRA_VERSION_CHECK_XTRA3 PDSM_LITE_PA_XTRA_VERSION_CHECK_XTRA3	  /* XTRA Version Check with XTRA3 format */
#define PDSM_PA_XTRA_VERSION_CHECK_MAX PDSM_LITE_PA_XTRA_VERSION_CHECK_MAX  /* validity checking */
#define PDSM_PA_XTRA_VERSION_CHECK_MAX_RESERVED PDSM_LITE_PA_XTRA_VERSION_CHECK_MAX_RESERVED /* force to 4 bytes */

typedef pdsm_lite_pa_xtra_ver_check_e_type pdsm_pa_xtra_ver_check_e_type;

/* This is a pdsm parameter info, IP address, PTLM mode and GPS lock status.
** This strucure is passed as it is to different clients whenever
** they are updates. (const takes care of clients not being able to
** modify the data structure.
*/
typedef pdsm_lite_pa_info_type pdsm_pa_info_type;
/*~ IF (_DISC_ == PDSM_LITE_PA_APP_INFO) pdsm_lite_pa_info.app_info */
/*~ IF (_DISC_ == PDSM_LITE_PA_GPS_LOCK)    pdsm_lite_pa_info.gps_lock */
/*~ IF (_DISC_ == PDSM_LITE_PA_KEEP_WARM)    pdsm_lite_pa_info.keep_warm_enable */
/*~ IF ((_DISC_ ==  PDSM_LITE_PA_DELETE_PARAMS) || (_DISC_ ==  PDSM_LITE_PA_DELETE_PARAMS_EX) || PDSM_LITE_PA_DELETE_PARAMS_EX1) pdsm_lite_pa_info.delete_params */
/*~ IF (_DISC_ ==  PDSM_LITE_PA_POSITION_MODE) pdsm_lite_pa_info.position_mode */
/*~ IF (_DISC_ ==  PDSM_LITE_PA_MT_LR_SUPPORT) pdsm_lite_pa_info.mt_lr_support */
/*~ IF (_DISC_ ==  PDSM_LITE_PA_MO_METHOD) pdsm_lite_pa_info.mo_method */
/*~ IF (_DISC_ == PDSM_LITE_PA_NMEA_SENTENCE_TYPE) pdsm_lite_pa_info.nmea_sentence_type */
/*~ IF ((_DISC_ == PDSM_LITE_PA_1X_PDE_NETWORK_ADRS) || (_DISC_ == PDSM_LITE_PA_1X_MPC_NETWORK_ADRS) || (_DISC_ == PDSM_LITE_PA_UMTS_PDE_NETWORK_ADRS))      pdsm_lite_pa_info.server_address */
/*~ IF (_DISC_ == PDSM_LITE_PA_SSD_DATA)     pdsm_lite_pa_info.ssd_info */
/*~ IF (_DISC_ == PDSM_LITE_PA_SEC_UPDATE_RATE)  pdsm_lite_pa_info.sec_rate */
/*~ IF (_DISC_ == PDSM_LITE_PA_PDAPI_ENABLE)  pdsm_lite_pa_info.pdapi_enable */
/*~ IF (_DISC_ == PDSM_LITE_PA_NMEA_CONFIG_INFO)  pdsm_lite_pa_info.nmea_config_info */
/*~ IF (_DISC_ == PDSM_LITE_PA_EFS_DATA)    pdsm_lite_pa_info.efs_data */
/*~ IF (_DISC_ == PDSM_LITE_PA_DPO_CONTROL)    pdsm_lite_pa_info.dpo_control */
/*~ IF (_DISC_ == PDSM_LITE_PA_ON_DEMAND_LOW_POWER_MODE)    pdsm_lite_pa_info.on_demand_lpm */
/*~ IF (_DISC_ == PDSM_LITE_PA_ON_DEMAND_READY_MODE)    pdsm_lite_pa_info.on_demand_ready */
/*~ IF (_DISC_ == PDSM_LITE_PA_CALL_FLOW_SELECT)        pdsm_lite_pa_info.call_flow_select_mask */
/*~ IF (_DISC_ == PDSM_LITE_PA_EXTERNAL_SPI) pdsm_lite_pa_info.external_spi */
/*~ IF (_DISC_ == PDSM_LITE_PA_XTRAT_CLIENT_TOKEN) pdsm_lite_pa_info.client_cookie */
/*~ IF (_DISC_ == PDSM_LITE_PA_XTRAT_USER_SESSION_CONTROL) pdsm_lite_pa_info.session_control */
/*~ IF ((_DISC_ == PDSM_LITE_PA_LBS_APN_PROFILE) || (_DISC_ == PDSM_LITE_PA_XTRA_APN_PROFILE))      pdsm_lite_pa_info.apn_profiles */
/*~ IF (_DISC_ == PDSM_LITE_PA_DATA_ENABLE) pdsm_lite_pa_info.data_enable */
/*~ IF (_DISC_ == PDSM_LITE_PA_SUPL_VERSION) pdsm_lite_pa_info.supl_version */
/*~ IF (_DISC_ == PDSM_LITE_PA_SUPL_SECURITY) pdsm_lite_pa_info.supl_security */
/*~ IF (_DISC_ == PDSM_LITE_PA_PE_CONFIG_PARAM) pdsm_lite_pa_info.pe_config_info */
/*~ IF (_DISC_ == PDSM_LITE_PA_LPP_CONFIG_INFO) pdsm_lite_pa_info.lpp_config_info */
/*~ IF (_DISC_ == PDSM_LITE_PA_ASST_GLONASS_POS_PROTOCOL) pdsm_lite_pa_info.asst_glo_protocol */
/*~ IF (_DISC_ == PDSM_LITE_PA_SUPL_IS801) pdsm_lite_pa_info.supl_is801 */
/*~ IF (_DISC_ == PDSM_LITE_PA_SUPL_TLS_VERSION) pdsm_lite_pa_info.supl_hash_algorithm */
/*~ IF (_DISC_ == PDSM_LITE_PA_SUPL_HASH_ALGORITHM) pdsm_lite_pa_info.asst_glo_protocol */
/*~ IF (_DISC_ == PDSM_LITE_PA_PREMIUM_SERVICES_CONFIG) pdsm_lite_pa_info.premium_svc_cfg */
/*~ IF (_DISC_ == PDSM_LITE_PA_XTRA_VERSION_CHECK) pdsm_lite_pa_info.xtra_version_check */
/*~ IF (_DISC_ == PDSM_LITE_PA_PCID) pdsm_lite_pa_info.pcid_info */
/*~ DEFAULT pdsm_lite_pa_info.void */

/* Enumeration of commands PDSM.
*/
#define PDSM_PA_CMD_NONE PDSM_LITE_PA_CMD_NONE 		  /* FOR INTERNAL USE OF PDSM ONLY! */
#define PDSM_PA_CMD_SET_PARAM PDSM_LITE_PA_CMD_SET_PARAM 		/* Set Parameter Command */
#define PDSM_PA_CMD_GET_PARAM PDSM_LITE_PA_CMD_GET_PARAM 		/* Get Parameter command */
#define PDSM_PA_CMD_MAX PDSM_LITE_PA_CMD_MAX				/* FOR INTERNAL USE OF PDSM ONLY! */
#define PDSM_PA_CMD_E_SIZE PDSM_LITE_PA_CMD_E_SIZE /* To force enum to 32 bit for windows NT */

typedef pdsm_lite_pa_cmd_e_type pdsm_pa_cmd_e_type;
/*~ SENTINEL pdsm_lite_pa_cmd_e_type.PDSM_LITE_PA_CMD_MAX */
/*- - - - - - - - - - - - - - EVENTS - - - - - - - - - - - - - - - - - - - - - -*/


/* Postion Determination Events.
** Clients are able to register with PDSM to be notified of any subsets
** of these events.
*/
typedef pdsm_lite_pd_event_type pdsm_pd_event_type;
/* General PDSM Events */
#define PDSM_PD_EVENT_POSITION PDSM_LITE_PD_EVENT_POSITION
#define PDSM_PD_EVENT_VELOCITY PDSM_LITE_PD_EVENT_VELOCITY
#define PDSM_PD_EVENT_HEIGHT   PDSM_LITE_PD_EVENT_HEIGHT
#define PDSM_PD_EVENT_DONE     PDSM_LITE_PD_EVENT_DONE
#define PDSM_PD_EVENT_END      PDSM_LITE_PD_EVENT_END      /* Indicating End of session. */
#define PDSM_PD_EVENT_BEGIN    PDSM_LITE_PD_EVENT_BEGIN    /* Indicate the start of session. */
#define PDSM_PD_EVENT_BEST_AVAIL_POS_REPORT  PDSM_LITE_PD_EVENT_BEST_AVAIL_POS_REPORT  
#define PDSM_PD_STATUS_GPS_STATE_INFO        PDSM_LITE_PD_STATUS_GPS_STATE_INFO        /* GPS State information */
#define PDSM_PD_STATUS_DPO_STATUS            PDSM_LITE_PD_STATUS_DPO_STATUS            /* DPO engage status */
#define PDSM_PD_STATUS_AVAIL_POS_REPORT      PDSM_LITE_PD_STATUS_AVAIL_POS_REPORT      /* Available  position report */
#define PDSM_PD_STATUS_ENGINE_MONITOR_REPORT PDSM_LITE_PD_STATUS_ENGINE_MONITOR_REPORT /* GNSS Engine Monitor report */
#define PDSM_PD_STATUS_EVENT_AVAIL_WWAN_POS_REPORT PDSM_LITE_PD_STATUS_EVENT_AVAIL_WWAN_POS_REPORT 

#define PDSM_PD_GENERAL_EVENT_MASK PDSM_LITE_PD_GENERAL_EVENT_MASK 
                            

/* Positioning Processing related events */
#define PDSM_PD_EVENT_GPS_BEGIN        PDSM_LITE_PD_EVENT_GPS_BEGIN
#define PDSM_PD_EVENT_GPS_DONE         PDSM_LITE_PD_EVENT_GPS_DONE

#define PDSM_PD_GPS_EVENT_MASK PDSM_LITE_PD_GPS_EVENT_MASK

/* Coase Position Injection related */
#define PDSM_PD_EVENT_COARSE_POS_INJ_DONE     PDSM_LITE_PD_EVENT_COARSE_POS_INJ_DONE
#define PDSM_PD_EVENT_COARSE_POS_INJ_FAILED   PDSM_LITE_PD_EVENT_COARSE_POS_INJ_FAILED
#define PDSM_PD_EVENT_COARSE_POS_INJ_ERROR    PDSM_LITE_PD_EVENT_COARSE_POS_INJ_ERROR
#define PDSM_PD_EVENT_COARSE_POS_INJ_MASK     PDSM_LITE_PD_EVENT_COARSE_POS_INJ_MASK

#define PDSM_PD_EVENT_UPDATE_FAILURE     PDSM_LITE_PD_EVENT_UPDATE_FAILURE  /* Update event indicating Failure */

#define PDSM_PD_EVENT_MASK  PDSM_LITE_PD_EVENT_MASK
                            
/* --------------------- EVENT CALLBACK DATA STRUCTURES -------------------*/

#define PDSM_PD_POSITION_SOURCE_GPS         PDSM_LITE_PD_POSITION_SOURCE_GPS  /* GPS Positions*/
#define PDSM_PD_POSITION_SOURCE_AFLT        PDSM_LITE_PD_POSITION_SOURCE_AFLT  /* AFLT position */
#define PDSM_PD_LOCATION_SOURCE_HYBRID      PDSM_LITE_PD_LOCATION_SOURCE_HYBRID  /* HYBRID position */
#define PDSM_PD_POSITION_SOURCE_DEFAULT     PDSM_LITE_PD_POSITION_SOURCE_DEFAULT  /* REFERENCE position from the network in a MSB/MSA or CELL ID call flow */
#define PDSM_PD_POSITION_SOURCE_UNKNOWN     PDSM_LITE_PD_POSITION_SOURCE_UNKNOWN  /* source is unknown, could be an externally injected or propaged position */
#define PDSM_PD_POSITION_SOURCE_GLO         PDSM_LITE_PD_POSITION_SOURCE_GLO  /* Source is GLONASS */

/*
  EPI stands for External Position Injection. It is a general term used
  to mean any externally provided 2D or 3D position was used to constraint
  the position fix generated by the position engine. EPI examples are Coarse
  position injection, Cell Database position, Reference Location from the 
  AGPS server, etc.
*/
#define PDSM_PD_POSITION_SOURCE_EPI         PDSM_LITE_PD_POSITION_SOURCE_EPI  /* Externally Injected Position constraint used */
#define PDSM_PD_POSITION_SOURCE_BDS         PDSM_LITE_PD_POSITION_SOURCE_BDS  /* Source is BeiDou */
#define PDSM_PD_POSITION_SOURCE_GAL         PDSM_LITE_PD_POSITION_SOURCE_GAL  /* Source is Galileo */
#define PDSM_PD_POSITION_SOURCE_QZSS        PDSM_LITE_PD_POSITION_SOURCE_QZSS  /* Source is QZSS  */
#define PDSM_PD_POSITION_SOURCE_CPI         PDSM_LITE_PD_POSITION_SOURCE_CPI  /* Specifically indicate that the EPI is a CPI */



/* valid Opitonal Fields */
//typedef byte pdsm_lite_pd_opt_field;

#define PDSM_PD_ALTITUDE_VALID PDSM_LITE_PD_ALTITUDE_VALID
#define PDSM_PD_VELOCITY_VALID  PDSM_LITE_PD_VELOCITY_VALID  /* Velocity means speed and direction */
#define PDSM_PD_UTCOFFSET_VALID PDSM_LITE_PD_UTCOFFSET_VALID
#define PDSM_PD_UNCERTAINTY_VALID PDSM_LITE_PD_UNCERTAINTY_VALID
#define PDSM_PD_EXT_STATUS_VALID  PDSM_LITE_PD_EXT_STATUS_VALID
#define PDSM_PD_MSEC_VALID PDSM_LITE_PD_MSEC_VALID       /* Millisecond field available */
#define PDSM_PM_DATA_AVAIL PDSM_LITE_PM_DATA_AVAIL       /* PM data available */

#define PDSM_PD_MEAS_DEBUG PDSM_LITE_PD_MEAS_DEBUG       /* Additional measurement info */

#define PDSM_PD_OPT_FIELD_MASK  PDSM_LITE_PD_OPT_FIELD_MASK
typedef pdsm_lite_pd_opt_field_ext pdsm_pd_opt_field_ext;
#define PDSM_PD_LAT_LONG_VALID PDSM_LITE_PD_LAT_LONG_VALID /* indicates if the lat/lon in the position report and ext pos is valid */
#define PDSM_PD_GPS_TIME_VALID PDSM_LITE_PD_GPS_TIME_VALID /* indicates if the time stamp in the ext pos has a valid GPS time */
#define PDSM_PD_XTRA_DATA_VALID PDSM_LITE_PD_XTRA_DATA_VALID /* indicates if the XTRA data in the ext pos is valid */
#define PDSM_PD_UTC_TIME_VALID PDSM_LITE_PD_UTC_TIME_VALID /* indicates if the UTC time stamp in the ext pos is valid */
#define PDSM_PD_HEADING_VALID  PDSM_LITE_PD_HEADING_VALID /*indicates if heading in position report and ext pos is valid*/
#define PDSM_PD_EXT_VELOCITY_VALID  PDSM_LITE_PD_EXT_VELOCITY_VALID /*indicates if ITAR Velocity velHorU is valid*/

typedef uint16 gnss_sv_id_type;      /* SV IDs for GNSS constellations is given below */
#define  PDSM_PD_SV_ID_GPS_MIN      PDSM_LITE_PD_SV_ID_GPS_MIN
#define  PDSM_PD_SV_ID_GPS_MAX      PDSM_LITE_PD_SV_ID_GPS_MAX
#define  PDSM_PD_SV_ID_SBAS_MIN     PDSM_LITE_PD_SV_ID_SBAS_MIN
#define  PDSM_PD_SV_ID_SBAS_MAX     PDSM_LITE_PD_SV_ID_SBAS_MAX
#define  PDSM_PD_SV_ID_GLO_MIN      PDSM_LITE_PD_SV_ID_GLO_MIN
#define  PDSM_PD_SV_ID_GLO_MAX      PDSM_LITE_PD_SV_ID_GLO_MAX
#define  PDSM_PD_SV_ID_QZSS_MIN     PDSM_LITE_PD_SV_ID_QZSS_MIN
#define  PDSM_PD_SV_ID_QZSS_MAX     PDSM_LITE_PD_SV_ID_QZSS_MAX
#define  PDSM_PD_SV_ID_BDS_MIN     PDSM_LITE_PD_SV_ID_BDS_MIN
#define  PDSM_PD_SV_ID_BDS_MAX     PDSM_LITE_PD_SV_ID_BDS_MAX
#define  PDSM_PD_SV_ID_GAL_MIN     PDSM_LITE_PD_SV_ID_GAL_MIN
#define  PDSM_PD_SV_ID_GAL_MAX     PDSM_LITE_PD_SV_ID_GAL_MAX
#define  PDSM_PD_SV_ID_UNKNOWN     PDSM_LITE_PD_SV_ID_UNKNOWN
/* Fix data Stuct for Pos completetion msg SCU=>MS data in Text format.*/
typedef pdsm_lite_utc_time_s_type pdsm_utc_time_s_type;

typedef pdsm_lite_gnss_time_source_mask pdsm_gnss_time_source_mask;
#define PDSM_GNSS_TIME_GPS    PDSM_LITE_GNSS_TIME_GPS    
#define PDSM_GNSS_TIME_GLO    PDSM_LITE_GNSS_TIME_GLO
#define PDSM_GNSS_TIME_BDS    PDSM_LITE_GNSS_TIME_BDS
#define PDSM_GNSS_TIME_GAL    PDSM_LITE_GNSS_TIME_GAL

#define PDSM_GNSS_TIME_SRC_UNKNOWN PDSM_LITE_GNSS_TIME_SRC_UNKNOWN /* Source of the Time is unknown */
#define PDSM_GNSS_TIME_SRC_NETWORK_TIME_TRANSFER	PDSM_LITE_GNSS_TIME_SRC_NETWORK_TIME_TRANSFER /* Time is set by 1x system */
#define PDSM_GNSS_TIME_SRC_NETWORK_TIME_TAGGING	PDSM_LITE_GNSS_TIME_SRC_NETWORK_TIME_TAGGING /* Time is set by WCDMA/GSM time tagging.
														   That is, associating network time with GPS time */
#define PDSM_GNSS_TIME_SRC_EXTERNAL_INPUT			 PDSM_LITE_GNSS_TIME_SRC_EXTERNAL_INPUT /* Time is set by an external injection */
#define PDSM_GNSS_TIME_SRC_GPS_TOW_DECODE			 PDSM_LITE_GNSS_TIME_SRC_GPS_TOW_DECODE /* Time is set after decoding over the air GPS navigation data
														   from one GPS satellite */
#define PDSM_GNSS_TIME_SRC_TOW_CONFIRMED			 PDSM_LITE_GNSS_TIME_SRC_TOW_CONFIRMED /* Time is set after decoding over the air GPS navigation data
														   from multiple satellites */
#define PDSM_GNSS_TIME_SRC_TOW_WEEK_CONFIRMED		 PDSM_LITE_GNSS_TIME_SRC_TOW_WEEK_CONFIRMED /* Both time of the week and the GPS week number known */
#define PDSM_GNSS_TIME_SRC_RESERVED				 PDSM_LITE_GNSS_TIME_SRC_RESERVED /* Reserved */
#define PDSM_GNSS_TIME_SRC_NAV_SOLUTION			 PDSM_LITE_GNSS_TIME_SRC_NAV_SOLUTION /* Time is set by position engine
														   after the fix is obtained. */
#define PDSM_GNSS_TIME_SRC_SOLVE_FOR_TIME			 PDSM_LITE_GNSS_TIME_SRC_SOLVE_FOR_TIME /* Time is set by the position engine after performing SFT.
														   This is done when the clock time uncertainty is large */
#define PDSM_GNSS_TIME_SRC_GLO_TOW_DECODE			 PDSM_LITE_GNSS_TIME_SRC_GLO_TOW_DECODE /* Time is set after decoding GLO satellites */
#define PDSM_GNSS_TIME_SRC_TIME_TRANSFORM			 PDSM_LITE_GNSS_TIME_SRC_TIME_TRANSFORM /* Time is set after transforming the GPS to GLO time */
#define PDSM_GNSS_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING PDSM_LITE_GNSS_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING /* Time is set by the sleep timetag
														   provided by WCDMA network */
#define PDSM_GNSS_TIME_SRC_GSM_SLEEP_TIME_TAGGING	 PDSM_LITE_GNSS_TIME_SRC_GSM_SLEEP_TIME_TAGGING /* Time is set by the sleep timetag
														   provided by GSM network */
#define PDSM_GNSS_TIME_SRC_QZSS_TOW_DECODE 		 PDSM_LITE_GNSS_TIME_SRC_QZSS_TOW_DECODE /* Time is set after decoding QZSS satellites */
#define PDSM_GNSS_TIME_SRC_BDS_TOW_DECODE			 PDSM_LITE_GNSS_TIME_SRC_BDS_TOW_DECODE /* Time is set after decoding BDS satellites */
#define PDSM_GNSS_TIME_SRC_GAL_TOW_DECODE			 PDSM_LITE_GNSS_TIME_SRC_GAL_TOW_DECODE /* Time is set after decoding GAL satellites */
#define PDSM_GNSS_TIME_SRC_INVALID 				 PDSM_LITE_GNSS_TIME_SRC_INVALID /* Invalid time */
#define PDSM_GNSS_TIME_SRC_MAX 					 PDSM_LITE_GNSS_TIME_SRC_MAX /* force enum to 32 bit */

typedef pdsm_lite_extended_time_src_e_type pdsm_extended_time_src_e_type;

/* Originally defined in gnss_consts.h, redefined in pdapi.h to avoid including gnss_consts.h */
#define PDSM_N_ACTIVE_GPS_CHAN     PDSM_LITE_N_ACTIVE_GPS_CHAN  /* Total number of active GPS channels */
#define PDSM_N_ACTIVE_GLO_CHAN     PDSM_LITE_N_ACTIVE_GLO_CHAN  /* Total number of active Glonass channels */
#define PDSM_N_ACTIVE_BDS_CHAN     PDSM_LITE_N_ACTIVE_BDS_CHAN  /* Total number of active BDS channels */
#define PDSM_N_ACTIVE_GAL_CHAN     PDSM_LITE_N_ACTIVE_GAL_CHAN  /* Total number of active GAL channels */
#define PDSM_N_ACTIVE_QZSS_CHAN    PDSM_LITE_N_ACTIVE_QZSS_CHAN  /* Total number of active QZSS channels */
/* If set, GNSS SV measurement is valid (detected by searcher) */
#define PDSM_GNSS_SV_INFO_MEAS_VALID       PDSM_LITE_GNSS_SV_INFO_MEAS_VALID
/* If set, GNSS SV is usable: {GPS|BDS} PRNs or {GLO} Slot IDs */
#define PDSM_GNSS_SV_INFO_USABLE           PDSM_LITE_GNSS_SV_INFO_USABLE
/* If set, GNSS SV is used for pos fix:   {GPS|BDS} PRNs or {GLO} Slot IDs */
#define PDSM_GNSS_SV_INFO_USED_IN_POS_FIX  PDSM_LITE_GNSS_SV_INFO_USED_IN_POS_FIX
/* If set, GNSS SV is used for velocity fix: {GPS|BDS} PRNs or {GLO} Slot IDs */
#define PDSM_GNSS_SV_INFO_USED_IN_VEL_FIX  PDSM_LITE_GNSS_SV_INFO_USED_IN_VEL_FIX


#ifdef FEATURE_GNSS_LEAN_BUILD
#pragma anon_unions
#endif

typedef pdsm_lite_GnssSvInfoType pdsm_GnssSvInfoType;

/* SV-specific data per GNSS for supplemental SV information
   Please note that SvMaskUsed & SvMaskUsable bitmask is redundant information, it can be generated
   by going through the SvInfo array and checking the q_SvInfoMask. It's kept here because of
   backward compatiblity */
typedef pdsm_lite_GnssNavSvInfo pdsm_GnssNavSvInfo;

#define PDSM_PD_CLK_SRC_HOST PDSM_LITE_PD_CLK_SRC_HOST
#define PDSM_PD_CLK_SRC_NETWORK PDSM_LITE_PD_CLK_SRC_NETWORK

typedef pdsm_lite_pd_sensor_bias_data pdsm_pd_sensor_bias_data;
typedef pdsm_lite_pd_sensor_param_report pdsm_pd_sensor_param_report;


typedef pdsm_lite_pd_clk_src_e_type pdsm_pd_clk_src_e_type;
typedef pdsm_lite_pd_external_time_info_type pdsm_pd_external_time_info_type;

/* start Coase Position Injection related */

typedef pdsm_lite_pd_extern_coarse_pos_opt_field pdsm_pd_extern_coarse_pos_opt_field;
#define PDSM_PD_EXTERN_COARSE_POS_GPS_TIME_VALID PDSM_LITE_PD_EXTERN_COARSE_POS_GPS_TIME_VALID
#define PDSM_PD_EXTERN_COARSE_POS_LAT_LONG_VALID PDSM_LITE_PD_EXTERN_COARSE_POS_LAT_LONG_VALID
#define PDSM_PD_EXTERN_COARSE_POS_ALT_VALID      PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_VALID
#define PDSM_PD_EXTERN_COARSE_POS_ELLIPTICAL_UNC_VALID PDSM_LITE_PD_EXTERN_COARSE_POS_ELLIPTICAL_UNC_VALID
#define PDSM_PD_EXTERN_COARSE_POS_CIRCULAR_UNC_VALID   PDSM_LITE_PD_EXTERN_COARSE_POS_CIRCULAR_UNC_VALID
#define PDSM_PD_EXTERN_COARSE_POS_HORIZ_CONF_VALID PDSM_LITE_PD_EXTERN_COARSE_POS_HORIZ_CONF_VALID
#define PDSM_PD_EXTERN_COARSE_POS_RAW_CIRCULAR_UNC_VALID PDSM_LITE_PD_EXTERN_COARSE_POS_RAW_CIRCULAR_UNC_VALID
#define PDSM_PD_EXTERN_COARSE_POS_RAW_HORIZ_CONF_VALID PDSM_LITE_PD_EXTERN_COARSE_POS_RAW_HORIZ_CONF_VALID


#define PDSM_PD_EXTERN_COARSE_POS_SOURCE_UNKNOWN PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_UNKNOWN,
#define PDSM_PD_EXTERN_COARSE_POS_SOURCE_GPS PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_GPS,
#define PDSM_PD_EXTERN_COARSE_POS_SOURCE_CELLID PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_CELLID,
#define PDSM_PD_EXTERN_COARSE_POS_SOURCE_ENH_CELLID PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_ENH_CELLID,
#define PDSM_PD_EXTERN_COARSE_POS_SOURCE_TERRESTRIAL PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_TERRESTRIAL,
#define PDSM_PD_EXTERN_COARSE_POS_SOURCE_GPS_TERRESTRIAL_HYBRID PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_GPS_TERRESTRIAL_HYBRID,
#define PDSM_PD_EXTERN_COARSE_POS_SOURCE_OTHER PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_OTHER,
#define PDSM_PD_EXTERN_COARSE_POS_SOURCE_MAX PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_MAX/* force enum to 32 bit */

typedef pdsm_lite_pd_extern_coarse_pos_source_e_type pdsm_pd_extern_coarse_pos_source_e_type;

#define PDSM_PD_EXTERN_CPI_SRC_EXTERNAL PDSM_LITE_PD_EXTERN_CPI_SRC_EXTERNAL /* Position is sourced from an external module (i.e. GNP or third party) */
#define PDSM_PD_EXTERN_CPI_SRC_INTERNAL PDSM_LITE_PD_EXTERN_CPI_SRC_INTERNAL /* Position is sourced from an internal QCOM module */
#define PDSM_PD_EXTERN_CPI_SRC_MAX PDSM_LITE_PD_EXTERN_CPI_SRC_MAX  /* force enum to 32 bit */

/* CPI position source provider */
typedef pdsm_lite_pd_extern_cpi_src_provider_e_type pdsm_pd_extern_cpi_src_provider_e_type;

#define PDSM_PD_EXTERN_COARSE_POS_TIME_GPS PDSM_LITE_PD_EXTERN_COARSE_POS_TIME_GPS
#define PDSM_PD_EXTERN_COARSE_POS_TIME_UTC PDSM_LITE_PD_EXTERN_COARSE_POS_TIME_UTC
#define PDSM_PD_EXTERN_COARSE_POS_TIME_AGE PDSM_LITE_PD_EXTERN_COARSE_POS_TIME_AGE
#define PDSM_PD_EXTERN_COARSE_POS_TIME_MAX PDSM_LITE_PD_EXTERN_COARSE_POS_TIME_MAX     /* force enum to 32 bit */

typedef pdsm_lite_pd_extern_coarse_pos_time_stamp_e_type pdsm_pd_extern_coarse_pos_time_stamp_e_type;

#define PDSM_PD_EXTERN_COARSE_POS_ALT_HAE PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_HAE  /* height above ellipsoid */
#define PDSM_PD_EXTERN_COARSE_POS_ALT_MSL PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_MSL  /* height above mean sea level */
#define PDSM_PD_EXTERN_COARSE_POS_ALT_MAX PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_MAX   /* force enum to 32 bit */

typedef pdsm_lite_pd_extern_coarse_pos_alt_def_e_type pdsm_pd_extern_coarse_pos_alt_def_e_type; /* definition of altitude */

typedef pdsm_lite_pd_horizontal_unc_elliptical_type pdsm_pd_horizontal_unc_elliptical_type;

typedef pdsm_lite_pd_external_pos_info_type pdsm_pd_external_pos_info_type;

typedef pdsm_lite_pd_external_pos_info_ex_type pdsm_pd_external_pos_info_ex_type;

/* Conversion factor for lat/long rad. <-> degree */
#define PDSM_COARSE_LAT_SCALE_FACTOR  PDSM_LITE_COARSE_LAT_SCALE_FACTOR // 2^32/180
#define PDSM_COARSE_LON_SCALE_FACTOR  PDSM_LITE_COARSE_LON_SCALE_FACTOR   // 2^31/180

#define PDSM_COARSE_LAT_RAD_TO_DEG_SCALE_FACTOR  (1.0/PDSM_LITE_COARSE_LAT_SCALE_FACTOR)
#define PDSM_COARSE_LON_RAD_TO_DEG_SCALE_FACTOR  (1.0/PDSM_LITE_COARSE_LON_SCALE_FACTOR)

typedef pdsm_lite_pd_external_position_info_type pdsm_pd_external_position_info_type;
/* end Coarse Position Injection related */


/* Position Determination Parameter event related. Do we need to combine the events together.
 *  
 * Caution: Don't add any new PA event bitmask. Use 'pdsm_lite_pa_e_type' to define PA events as enums.
*/
typedef pdsm_lite_pa_event_type pdsm_pa_event_type;

#define PDSM_PA_EVENT_NONE                        PDSM_LITE_PA_EVENT_NONE       /* No PA event */
#define PDSM_PA_EVENT_GPS_LOCK                    PDSM_LITE_PA_EVENT_GPS_LOCK       /* GPS lock change */
#define PDSM_PA_EVENT_DELETE_PARAMS               PDSM_LITE_PA_EVENT_DELETE_PARAMS     /* Delete parameters */
#define PDSM_PA_EVENT_DPO_CONTROL                 PDSM_LITE_PA_EVENT_DPO_CONTROL  /* DPO Control */
#define PDSM_PA_EVENT_XTRAT_CLIENT_TOKEN          PDSM_LITE_PA_EVENT_XTRAT_CLIENT_TOKEN /*XTRAT client token*/
#define PDSM_PA_EVENT_XTRAT_SESSION_CONTROL       PDSM_LITE_PA_EVENT_XTRAT_SESSION_CONTROL /*XTRAT session control*/
#define PDSM_PA_EVENT_XTRA_APN_PROFILE_SELECT     PDSM_LITE_PA_EVENT_XTRA_APN_PROFILE_SELECT /*XTRA APN profile select*/

#define PDSM_PA_EVENT_MASK  PDSM_LITE_PA_EVENT_MASK


typedef pdsm_lite_pa_info_s_type pdsm_pa_info_s_type;
/*~ FIELD pdsm_lite_pa_info_s.pa_info DISC pdsm_lite_pa_info_s.pa_event */

/* Typedef structure used by user in providing PA set parameters to be set.
*/
typedef pdsm_lite_pa_set_type pdsm_pa_set_type;
/*~ FIELD pdsm_lite_pa_set_s.info DISC pdsm_lite_pa_set_s.pa_set */

                                      
/* Max Active Channels: GPS 16, GLO 14, BDS 16, GAL 16 less than 80 (defined in QMI-LOC) */
#define PDSM_EXT_MEAS_NUM_SVS PDSM_LITE_EXT_MEAS_NUM_SVS

typedef pdsm_lite_sv_state pdsm_sv_state;


/* Typedef struct for GNSS channel report.
*/
typedef pdsm_lite_cgps_chan_rpt_type pdsm_cgps_chan_rpt_type;

#define PDSM_PD_GPS_STATE_POS_VALID PDSM_LITE_PD_GPS_STATE_POS_VALID /* indicates lat/long and pos_unc_horizontal fields are valid */
#define PDSM_PD_GPS_STATE_ALT_VALID PDSM_LITE_PD_GPS_STATE_ALT_VALID /* indicates alt and loc_Unc_vertical are valid */
#define PDSM_PD_GPS_STATE_TIME_MS_VALID PDSM_LITE_PD_GPS_STATE_TIME_MS_VALID /* indicates time stamp ms field is valid */
#define PDSM_PD_GPS_STATE_TIME_WN_VALID PDSM_LITE_PD_GPS_STATE_TIME_WN_VALID /* indicates timestamp week number field is valid */
#define PDSM_PD_GPS_STATE_TUNC_VALID    PDSM_LITE_PD_GPS_STATE_TUNC_VALID /* indicates timestamp tunc field is valid */
#define PDSM_PD_GPS_STATE_EPH_VALID    PDSM_LITE_PD_GPS_STATE_EPH_VALID /* indicates eph svmask field is valid */
#define PDSM_PD_GPS_STATE_ALM_VALID    PDSM_LITE_PD_GPS_STATE_ALM_VALID /* indicates alm svmask field is valid */
#define PDSM_PD_GPS_STATE_XTRA_VALID    PDSM_LITE_PD_GPS_STATE_XTRA_VALID /* indicates xtra data  svmask field is valid */
#define PDSM_PD_GPS_STATE_HEALTH_VALID  PDSM_LITE_PD_GPS_STATE_HEALTH_VALID
#define PDSM_PD_GPS_STATE_VISIBLE_SV_VALID PDSM_LITE_PD_GPS_STATE_VISIBLE_SV_VALID

/*- - - - - - - - - - - - - -COMMAND ERRORS - - - - - - - - - - - - - - - - - - - - - - -*/

// TODO: review
typedef pdsm_lite_pd_gps_state_s_type pdsm_pd_gps_state_s_type;

/* Enumeration of PD command errors returned by command
** callback functions.
*/
#define PDSM_PD_CMD_ERR_NONE PDSM_LITE_PD_CMD_ERR_NONE
/* FOR INTERNAL USE OF PDSM ONLY! */
#define PDSM_PD_CMD_ERR_NOERR PDSM_LITE_PD_CMD_ERR_NOERR
/* No errors found */

/* Error codes resulting from clients feeding a PD command
** function with bad or inappropriate parameters.
*/

#define PDSM_PD_CMD_ERR_CLIENT_ID_P PDSM_LITE_PD_CMD_ERR_CLIENT_ID_P
/* invalid client ID */

#define PDSM_PD_CMD_ERR_SESS_TYPE_P PDSM_LITE_PD_CMD_ERR_SESS_TYPE_P
/* Bad session type parameter */
#define PDSM_PD_CMD_ERR_PRIVACY_P PDSM_LITE_PD_CMD_ERR_PRIVACY_P /* not used anymore */
#define PDSM_PD_CMD_ERR_NET_ACCESS_P PDSM_LITE_PD_CMD_ERR_NET_ACCESS_P /* not used anymore */
#define PDSM_PD_CMD_ERR_OPERATION_P PDSM_LITE_PD_CMD_ERR_OPERATION_P
#define PDSM_PD_CMD_ERR_NUM_FIXES_P PDSM_LITE_PD_CMD_ERR_NUM_FIXES_P
#define PDSM_PD_CMD_ERR_LSINFO_P PDSM_LITE_PD_CMD_ERR_LSINFO_P
/* Wrong server Information parameters */

#define PDSM_PD_CMD_ERR_TIMEOUT_P PDSM_LITE_PD_CMD_ERR_TIMEOUT_P
/* Error in timeout parameter */

#define PDSM_PD_CMD_ERR_QOS_P PDSM_LITE_PD_CMD_ERR_QOS_P
/* Error in QOS accuracy thershold param */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Error codes resulting from clients trying to instruct
** PDSM to perform inappropriate actions relative to the current
** state.
*/

#define PDSM_PD_CMD_ERR_NO_SESS_S PDSM_LITE_PD_CMD_ERR_NO_SESS_S
/* No session Active, while trying to end Session */

#define PDSM_PD_CMD_ERR_BUSY_S PDSM_LITE_PD_CMD_ERR_BUSY_S
/* Session Busy status */

#define PDSM_PD_CMD_ERR_OFFLINE_S PDSM_LITE_PD_CMD_ERR_OFFLINE_S
/* Phone is offline */

#define PDSM_PD_CMD_ERR_GPS_LOCK_S PDSM_LITE_PD_CMD_ERR_GPS_LOCK_S
/* GPS is locked */

#define PDSM_PD_CMD_ERR_STATE_S PDSM_LITE_PD_CMD_ERR_STATE_S
/* The command is invalid in this state. (Ex When is phone is in E911CB) */

#define PDSM_PD_CMD_ERR_NO_BUF_L PDSM_LITE_PD_CMD_ERR_NO_BUF_L
/* No available PDSM command buffers to queue the command */

#define PDSM_PD_CMD_ERR_SEARCH_COM_L PDSM_LITE_PD_CMD_ERR_SEARCH_COM_L
/* Communication problems with Search - e.g. SRCH buffer shortage */

#define PDSM_PD_CMD_ERR_CANT_RPT_NOW_S PDSM_LITE_PD_CMD_ERR_CANT_RPT_NOW_S
/* PD Results cannot be reported at this time, retry later or wait. not used anymore */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define PDSM_PD_CMD_ERR_MODE_NOT_SUPPORTED_S PDSM_LITE_PD_CMD_ERR_MODE_NOT_SUPPORTED_S /* not used anymore */
#define PDSM_PD_CMD_ERR_PERIODIC_NI_IN_PROGRESS_S PDSM_LITE_PD_CMD_ERR_PERIODIC_NI_IN_PROGRESS_S

#define PDSM_PD_CMD_ERR_AUTH_FAIL_S PDSM_LITE_PD_CMD_ERR_AUTH_FAIL_S
/* Client Authentication Failure */

#define PDSM_PD_CMD_ERR_OTHER PDSM_LITE_PD_CMD_ERR_OTHER
/* A problem other than the above was found */

#define PDSM_PD_CMD_ERR_FEATURE_NOT_ENABLED PDSM_LITE_PD_CMD_ERR_FEATURE_NOT_ENABLED
/* feature not enabled */

#define PDSM_PD_CMD_ERR_UNKNOWN_CMD PDSM_LITE_PD_CMD_ERR_UNKNOWN_CMD
/* unknown command */

#define PDSM_PD_CMD_ERR_INVALID_PARAM PDSM_LITE_PD_CMD_ERR_INVALID_PARAM
/* invalid parameters */

#define PDSM_PD_CMD_ERR_MAX PDSM_LITE_PD_CMD_ERR_MAX
/* FOR INTERNAL USE OF PDSM ONLY! */

#define PDSM_PD_CMD_ERR_E_SIZE PDSM_LITE_PD_CMD_ERR_E_SIZE
/* To force enum to 32 bit */

typedef uint32 pdsm_pa_nmea_type;
//#define PDAPI_LITE_NMEA_VAR_LEN_DEBUG 0x00080000 

typedef pdsm_lite_pd_ext_var_len_debug_nmea_s_type pdsm_pd_ext_var_len_debug_nmea_s_type;

typedef pdsm_lite_pd_cmd_err_e_type pdsm_pd_cmd_err_e_type;
/*~ SENTINEL pdsm_lite_pd_cmd_err_e_type.PDSM_LITE_PD_CMD_ERR_MAX */

/* Enumeration of PA command errors returned by command
** callback functions.
*/
#define PDSM_PA_CMD_ERR_NONE PDSM_LITE_PA_CMD_ERR_NONE 	   /* FOR INTERNAL USE OF PDSM ONLY! */
#define PDSM_PA_CMD_ERR_NOERR PDSM_LITE_PA_CMD_ERR_NOERR 		 /* No errors found */
#define PDSM_PA_CMD_ERR_NO_BUF_L PDSM_LITE_PA_CMD_ERR_NO_BUF_L		 /* No buffers available */
#define PDSM_PA_CMD_ERR_CLIENT_ID_P PDSM_LITE_PA_CMD_ERR_CLIENT_ID_P	 /* Invalid Client id */
#define PDSM_PA_CMD_ERR_PARAM_TYPE_P PDSM_LITE_PA_CMD_ERR_PARAM_TYPE_P /* Error in Parameters */
#define PDSM_PA_CMD_ERR_GPS_LOCK_P PDSM_LITE_PA_CMD_ERR_GPS_LOCK_P	 /* Error in Lock type */
#define PDSM_PA_CMD_ERR_OFFLINE_S PDSM_LITE_PA_CMD_ERR_OFFLINE_S 	 /* If phone is offline state */
#define PDSM_PA_CMD_ERR_PA_ACTIVE_S PDSM_LITE_PA_CMD_ERR_PA_ACTIVE_S	 /* An active pa_cmd is running */
#define PDSM_PA_CMD_ERR_APP_INFO_P PDSM_LITE_PA_CMD_ERR_APP_INFO_P	 /* Wrong app info */
#define PDSM_PA_CMD_ERR_DELETE_PARM_P PDSM_LITE_PA_CMD_ERR_DELETE_PARM_P  /* wrong delete param type */
#define PDSM_PA_CMD_ERR_POSITION_MODE_P PDSM_LITE_PA_CMD_ERR_POSITION_MODE_P /* wrong position mode type */
#define PDSM_PA_CMD_ERR_SRV_SYS_P PDSM_LITE_PA_CMD_ERR_SRV_SYS_P 	 /* wrong serving system type*/
#define PDSM_PA_CMD_ERR_OTHER PDSM_LITE_PA_CMD_ERR_OTHER 		 /* A problem other than the above was found */
#define PDSM_PA_CMD_ERR_UNKNOWN_CMD PDSM_LITE_PA_CMD_ERR_UNKNOWN_CMD	 /* Unknown command */
#define PDSM_PA_CMD_ERR_NOT_ALLOWED PDSM_LITE_PA_CMD_ERR_NOT_ALLOWED	 /* Command not allowed in current state */
#define PDSM_PA_CMD_ERR_MAX PDSM_LITE_PA_CMD_ERR_MAX		   /* A problem other than the above was found */
#define PDSM_PA_CMD_ERR_E_SIZE PDSM_LITE_PA_CMD_ERR_E_SIZE   /* Fix the size as int */

typedef pdsm_lite_pa_cmd_err_e_type pdsm_pa_cmd_err_e_type;
/*~ SENTINEL pdsm_lite_pa_cmd_err_e_type.PDSM_LITE_PA_CMD_ERR_MAX */

/* Enum for gpsOneXTRA command errors
*/
#define PDSM_XTRA_CMD_ERR_NONE PDSM_LITE_XTRA_CMD_ERR_NONE

#define PDSM_XTRA_CMD_ERR_NOERR PDSM_LITE_XTRA_CMD_ERR_NOERR
#define PDSM_XTRA_CMD_ERR_CLIENT_ID_P PDSM_LITE_XTRA_CMD_ERR_CLIENT_ID_P
#define PDSM_XTRA_CMD_ERR_NO_BUF_L PDSM_LITE_XTRA_CMD_ERR_NO_BUF_L
#define PDSM_XTRA_CMD_ERR_BAD_CRC PDSM_LITE_XTRA_CMD_ERR_BAD_CRC
#define PDSM_XTRA_CMD_ERR_TIME_OF_VALIDITY_OUT_OF_RANGE PDSM_LITE_XTRA_CMD_ERR_TIME_OF_VALIDITY_OUT_OF_RANGE
#define PDSM_XTRA_CMD_ERR_DOWNLOAD_INTRVL_OUT_OF_RANGE PDSM_LITE_XTRA_CMD_ERR_DOWNLOAD_INTRVL_OUT_OF_RANGE
#define PDSM_XTRA_CMD_ERR_INTERNAL_RESOURCE_ERROR PDSM_LITE_XTRA_CMD_ERR_INTERNAL_RESOURCE_ERROR
#define PDSM_XTRA_CMD_ERR_GPS_SUBSYSTEM_BUSY PDSM_LITE_XTRA_CMD_ERR_GPS_SUBSYSTEM_BUSY
#define PDSM_XTRA_CMD_ERR_TIME_INFO_ERROR PDSM_LITE_XTRA_CMD_ERR_TIME_INFO_ERROR
#define PDSM_XTRA_CMD_ERR_OTHER PDSM_LITE_XTRA_CMD_ERR_OTHER
#define PDSM_XTRA_CMD_ERR_GPS_LOCK_S PDSM_LITE_XTRA_CMD_ERR_GPS_LOCK_S			 /* GPS is locked */
#define PDSM_XTRA_CMD_ERR_STATE_S PDSM_LITE_XTRA_CMD_ERR_STATE_S 			 /* Command is invalid in this state. (Ex When is phone is in E911CB) */
#define PDSM_XTRA_CMD_ERR_UNKNOWN_CMD PDSM_LITE_XTRA_CMD_ERR_UNKNOWN_CMD 		 /* Unknown command */
#define PDSM_XTRA_CMD_ERR_FEATURE_NOT_ENABLED PDSM_LITE_XTRA_CMD_ERR_FEATURE_NOT_ENABLED  /* Feature not enabled */
#define PDSM_XTRA_CMD_ERR_INVALID_PARAM PDSM_LITE_XTRA_CMD_ERR_INVALID_PARAM		 /* Invalid parameter */
#define PDSM_XTRA_CMD_ERR_MAX PDSM_LITE_XTRA_CMD_ERR_MAX
#define PDSM_XTRA_CMD_ERR_E_SIZE PDSM_LITE_XTRA_CMD_ERR_E_SIZE

typedef pdsm_lite_xtra_cmd_err_e_type pdsm_xtra_cmd_err_e_type;
/*~ SENTINEL pdsm_lite_xtra_cmd_err_e_type.PDSM_LITE_XTRA_CMD_ERR_MAX */

/* Enum for XTRA-T command errors
*/
#define PDSM_XTRA_T_CMD_ERR_NONE PDSM_LITE_XTRA_T_CMD_ERR_NONE
#define PDSM_XTRA_T_CMD_ERR_SUCCESS PDSM_LITE_XTRA_T_CMD_ERR_SUCCESS,
#define PDSM_XTRA_T_CMD_ERR_BUSY PDSM_LITE_XTRA_T_CMD_ERR_BUSY			/* Another XTRA-T session is in progress */
#define PDSM_XTRA_T_CMD_ERR_OTHER PDSM_LITE_XTRA_T_CMD_ERR_OTHER 		 /* Ending due to internal errors - data pointer error, E911, GPS lock, cmd not supported  */
#define PDSM_XTRA_T_CMD_ERR_CLIENT_ID_P PDSM_LITE_XTRA_T_CMD_ERR_CLIENT_ID_P	 /* Ending due to XTRA-T client not registered/available */
#define PDSM_XTRA_T_CMD_FEATURE_NOT_ENABLED PDSM_LITE_XTRA_T_CMD_FEATURE_NOT_ENABLED /* Ending due to XTRA-T feature is not enabled */
#define PDSM_XTRA_T_CMD_ERR_BAD_GET_DATA_PARAM PDSM_LITE_XTRA_T_CMD_ERR_BAD_GET_DATA_PARAM /* Ending due to bad XTRA-T get data parameters */
#define PDSM_XTRA_T_CMD_ERR_INTERNAL_RESOURCE PDSM_LITE_XTRA_T_CMD_ERR_INTERNAL_RESOURCE/* Ending due to internal resource errors - TM task not active */
#define PDSM_XTRA_T_CMD_ERR_GPS_LOCK_S PDSM_LITE_XTRA_T_CMD_ERR_GPS_LOCK_S		   /* GPS is locked */
#define PDSM_XTRA_T_CMD_ERR_STATE_S PDSM_LITE_XTRA_T_CMD_ERR_STATE_S			   /* Command is invalid in this state. (Ex When is phone is in E911CB) */
#define PDSM_XTRA_T_CMD_ERR_UNKNOWN_CMD PDSM_LITE_XTRA_T_CMD_ERR_UNKNOWN_CMD		   /* Unknown command */
#define PDSM_XTRA_T_CMD_ERR_FEATURE_NOT_ENABLED PDSM_LITE_XTRA_T_CMD_ERR_FEATURE_NOT_ENABLED  /* Feature not enabled */
#define PDSM_XTRA_T_CMD_ERR_INVALID_PARAM PDSM_LITE_XTRA_T_CMD_ERR_INVALID_PARAM 	   /* Invalid parameter */
#define PDSM_XTRA_T_CMD_END_MAX PDSM_LITE_XTRA_T_CMD_END_MAX				 /* FOR INTERNAL USE OF PDSM ONLY! */
#define PDSM_XTRA_T_CMD_END_E_SIZE PDSM_LITE_XTRA_T_CMD_END_E_SIZE  /* To force enum to 32 bit for windows NT */

typedef pdsm_lite_xtra_t_cmd_err_e_type pdsm_xtra_t_cmd_err_e_type;
/*~ SENTINEL pdsm_lite_xtra_t_cmd_err_e_type.PDSM_LITE_XTRA_T_CMD_END_MAX */

/*
** Enumerations for end of session reasons.
*/
#define PDSM_PD_END_SESS_NONE PDSM_LITE_PD_END_SESS_NONE	 /* For internal use of PDSM */
#define PDSM_PD_END_CLIENT_END PDSM_LITE_PD_END_CLIENT_END		 /* User ended the session */
#define PDSM_PD_END_TIMEOUT PDSM_LITE_PD_END_TIMEOUT			 /* Timeout (viz., for GPS Search) */
#define PDSM_PD_END_FIX_ERROR PDSM_LITE_PD_END_FIX_ERROR 			/* Error in Fix */
#define PDSM_PD_END_CLIENT_HAS_NO_SESS PDSM_LITE_PD_END_CLIENT_HAS_NO_SESS  /* Current session doesn't belong to this client */
#define PDSM_PD_END_MAX PDSM_LITE_PD_END_MAX				 /* Maximum for PDSM internal use */
#define PDSM_PD_END_E_SIZE PDSM_LITE_PD_END_E_SIZE /* To fix enum Size as int */

typedef pdsm_lite_pd_end_e_type pdsm_pd_end_e_type;

#define PDSM_LATLONG_SCALE PDSM_LITE_LATLONG_SCALE
/* For SVs in view, active channels only, using 16*3=48 for now */
#define PDSM_GNSS_MAX_SATELLITES PDSM_LITE_EXT_MEAS_NUM_SVS




 
	 	 	 
#define   PDSM_PD_FIX_STATUS_DEBUG_FAIL            PDSM_LITE_PD_FIX_STATUS_DEBUG_FAIL       /* Unsuccessful fix*/
#define   PDSM_PD_FIX_STATUS_DEBUG_SUCCESS         PDSM_LITE_PD_FIX_STATUS_DEBUG_SUCCESS /* Successful Fix.  */
#define   PDSM_PD_FIX_STATUS_DEBUG_LOW_SV          PDSM_LITE_PD_FIX_STATUS_DEBUG_LOW_SV     /* LOW SV's.*/
#define   PDSM_PD_FIX_STATUS_DEBUG_HEPE_FAIL       PDSM_LITE_PD_FIX_STATUS_DEBUG_HEPE_FAIL      /* HEPE Check Failed.   */
#define   PDSM_PD_FIX_STATUS_DEBUG_LOW_RELIABILITY PDSM_LITE_PD_FIX_STATUS_DEBUG_LOW_RELIABILITY       /* LOW reliability Fix */

#define PDSM_PA_MAX_APN_NAME_LENGTH 100
typedef struct {
  uint32  srv_system_type; /*Serving system on which this APN should be used*/
  uint32  pdp_type; /*Pdp type of the APN profile*/
  uint32  reserved; /*Reserved for future use*/
  uint8   apn_name[PDSM_PA_MAX_APN_NAME_LENGTH]; /*APN name*/
} pdsm_pa_apn_profiles_type;

typedef  pdsm_lite_LocSvSystemEnumType pdsm_LocSvSystemEnumType;

typedef pdsm_lite_LocSourceofFreqEnumType  pdsm_LocSourceofFreqEnumType;



typedef pdsm_lite_LocRcvrClockFrequencyInfoStructType pdsm_LocRcvrClockFrequencyInfoStructType;

typedef pdsm_lite_LocLeapSecondInfoStructType pdsm_LocLeapSecondInfoStructType;

typedef pdsm_lite_LocInterSystemBiasValidMaskType pdsm_LocInterSystemBiasValidMaskType;

typedef pdsm_lite_InterSystemBiasStructType pdsm_InterSystemBiasStructType;

typedef pdsm_lite_LocSystemTimeStructType pdsm_LocSystemTimeStructType;

typedef pdsm_lite_LocSVTimeSpeedStructType pdsm_LocSVTimeSpeedStructType;


typedef pdsm_lite_LocSvInfoMaskT pdsm_LocSvInfoMaskT;


typedef pdsm_lite_LocSvSearchStatusEnumT pdsm_LocSvSearchStatusEnumT;

typedef pdsm_lite_LocSVMeasurementStructType pdsm_LocSVMeasurementStructType;


#define PDSM_EXT_GNSS_MEAS_NUM_SVS PDSM_LITE_EXT_GNSS_MEAS_NUM_SVS 
/**< Maximum number of satellites in measurement block for given system. */

typedef pdsm_lite_gnss_clockMeasurementStructType pdsm_gnss_clockMeasurementStructType;

typedef pdsm_lite_pd_ext_gnss_meas_s_type pdsm_pd_ext_gnss_meas_s_type;



typedef pdsm_lite_pd_ext_SvPoly_s_type pdsm_pd_ext_SvPoly_s_type;

#define PDSM_INTERMEDIATE_POS_RPT PDSM_LITE_INTERMEDIATE_POS_RPT
#define PDSM_FINAL_POS_RPT PDSM_LITE_FINAL_POS_RPT

typedef pdsm_lite_pos_type pdsm_pos_type;
typedef pdsm_lite_pd_client_info_u_type pdsm_pd_client_info_u_type;

/* When notified of PD events, clients are given a reference to this structure.
*/
typedef pdsm_lite_pd_info_s_type pdsm_pd_info_s_type;



/* ------------------ EXTENDED POSITION STATUS EVENT DATA STRUCTURES ------------------------------*/


typedef uint64 pds_ext_dpo_disengage_reason_mask_type;
#define PDSM_DPO_DISENGAGE_REASON_SIGNAL_CONDITIONS        PDSM_LITE_DPO_DISENGAGE_REASON_SIGNAL_CONDITIONS
#define PDSM_DPO_DISENGAGE_REASON_POSITION_ACCURACY        PDSM_LITE_DPO_DISENGAGE_REASON_POSITION_ACCURACY
#define PDSM_DPO_DISENGAGE_REASON_NAV_DATA_NEEDED          PDSM_LITE_DPO_DISENGAGE_REASON_NAV_DATA_NEEDED
#define PDSM_DPO_DISENGAGE_REASON_DPO_DISABLED             PDSM_LITE_DPO_DISENGAGE_REASON_DPO_DISABLED
#define PDSM_DPO_DISENGAGE_REASON_INSUFF_RESOURCES         PDSM_LITE_DPO_DISENGAGE_REASON_INSUFF_RESOURCES
#define PDSM_DPO_DISENGAGE_REASON_UNSUPPORTED_MODE         PDSM_LITE_DPO_DISENGAGE_REASON_UNSUPPORTED_MODE
#define PDSM_DPO_DISENGAGE_REASON_UNCALIB_CLOCK            PDSM_LITE_DPO_DISENGAGE_REASON_UNCALIB_CLOCK
#define PDSM_DPO_DISENGAGE_REASON_HEALTH_DATA_DECODE       PDSM_LITE_DPO_DISENGAGE_REASON_HEALTH_DATA_DECODE
#define PDSM_DPO_DISENGAGE_REASON_CLOCK_ACCURACY           PDSM_LITE_DPO_DISENGAGE_REASON_CLOCK_ACCURACY
#define PDSM_DPO_DISENGAGE_REASON_POSITION_NOT_COMPUTED    PDSM_LITE_DPO_DISENGAGE_REASON_POSITION_NOT_COMPUTED
#define PDSM_DPO_DISENGAGE_REASON_LTE_B13_TX               PDSM_LITE_DPO_DISENGAGE_REASON_LTE_B13_TX
#define PDSM_DPO_DISENGAGE_REASON_GSM_TX                   PDSM_LITE_DPO_DISENGAGE_REASON_GSM_TX
#define PDSM_DPO_DISENGAGE_REASON_UNCALIB_XO               PDSM_LITE_DPO_DISENGAGE_REASON_UNCALIB_XO

typedef uint32 pds_ext_dpo_status_mask_type;
#define PDSM_DPO_STATUS_ENGAGED        PDSM_LITE_DPO_STATUS_ENGAGED   /* DPO is engaged */
#define PDSM_DPO_STATUS_TOW_VALID      PDSM_LITE_DPO_STATUS_TOW_VALID   /* Timestamp of the status report in GPS TOW milliseconds is valid */

typedef pdsm_lite_pd_ext_pos_rpt_s_type pdsm_pd_ext_pos_rpt_s_type;

typedef pdsm_lite_pd_ext_dpo_status_rpt pdsm_pd_ext_dpo_status_rpt;

typedef pdsm_lite_pd_ext_engine_monitor_report_s_type pdsm_pd_ext_engine_monitor_report_s_type;

typedef pdsm_lite_ext_status_info_u_type pdsm_ext_status_info_u_type;
/*~ IF (_DISC_ == PDSM_LITE_EXT_STATUS_POS_REPORT) pdsm_lite_ext_status_info_u_type.ext_pos_report_type */
/*~ IF (_DISC_ == PDSM_LITE_EXT_STATUS_NMEA_POS_REPORT) pdsm_lite_ext_status_info_u_type.ext_nmea_pos_report_type */
/*~ IF (_DISC_ == PDSM_LITE_EXT_STATUS_UNIFIED_NMEA_POS_REPORT) pdsm_lite_ext_status_info_u_type.ext_unified_nmea_report_type */
/*~ IF (_DISC_ == PDSM_LITE_EXT_STATUS_MEASUREMENT) pdsm_lite_ext_status_info_u_type.ext_meas_report_type */
/*~ IF (_DISC_ == PDSM_LITE_EXT_STATUS_GPS_STATE_INFO) pdsm_lite_ext_status_info_u_type.ext_gps_state_type*/
/*~ IF (_DISC_ == PDSM_LITE_EXT_STATUS_GENERIC_EVENT) pdsm_lite_ext_status_info_u_type.ext_generic_event_info */
/*~ IF (_DISC_ == PDSM_LITE_EXT_STATUS_POS_INJECT_REQUEST) pdsm_lite_ext_status_info_u_type.ext_pos_inj_req_type */
/*~ IF (_DISC_ == PDSM_LITE_EXT_STATUS_PERIODIC_PPM_REPORT) pdsm_lite_ext_status_info_u_type.ext_periodic_ppm_report_type */
/*~ IF (_DISC_ == PDSM_LITE_EXT_STATUS_RESET_LOCATION_SERVICE) pdsm_lite_ext_status_info_u_type.ext_reset_location_service_status_type */
/*~ DEFAULT pdsm_lite_ext_status_info_u_type.void */

//ToDo:[DEL][APP] The content for this are now merged with pdsm_lite_pd_info_s_type
#if 0
typedef struct pdsm_lite_ext_status_info_s {
  pdsm_lite_client_id_type client_id;                /* client Id */
  pdsm_lite_ext_status_e_type ext_status_type;       /* event type */
  pdsm_lite_ext_status_info_u_type ext_status_info;  /* event data */
  qword                       time_stamp;       /* timestamp */
} pdsm_lite_ext_status_info_s_type;
#endif 
/*~ FIELD pdsm_lite_ext_status_info_s.ext_status_info DISC pdsm_lite_ext_status_info_s.ext_status_type */


/*------------------------- eXtended Receiver Assistance (XTRA) -------------------------*/

/* Enumeration of gpsOneXTRA commands
*/
#define  PDSM_XTRA_CMD_NONE PDSM_LITE_XTRA_CMD_NONE/* FOR INTERNAL USE OF PDSM ONLY! */

#define   PDSM_XTRA_CMD_SET_DATA PDSM_LITE_XTRA_CMD_SET_DATA                 /* gpsOneXTRA client is injecting XTRA data to PDAPI */
#define   PDSM_XTRA_CMD_INITIATE_DOWNLOAD PDSM_LITE_XTRA_CMD_INITIATE_DOWNLOAD        /* gpsOneXTRA client is requesting XTRA download */
#define   PDSM_XTRA_CMD_SET_AUTO_DOWNLOAD_PARAMS PDSM_LITE_XTRA_CMD_SET_AUTO_DOWNLOAD_PARAMS /* gpsOneXTRA client is setting download parameters */
#define   PDSM_XTRA_CMD_QUERY_DATA_VALIDITY PDSM_LITE_XTRA_CMD_QUERY_DATA_VALIDITY      /* gpsOneXTRA client is querying data validity */
#define   PDSM_XTRA_CMD_TIME_INFO_INJ PDSM_LITE_XTRA_CMD_TIME_INFO_INJ            /* gpsOneXTRA client is injecting SNTP time information */
#define   PDSM_XTRA_CMD_QUERY_XTRA_CFG_INFO PDSM_LITE_XTRA_CMD_QUERY_XTRA_CFG_INFO      /* gpsOneXTRA client is querying xtra config information */

#define   PDSM_XTRA_CMD_MAX PDSM_LITE_XTRA_CMD_MAX                /* FOR INTERNAL USE OF PDSM ONLY! */
#define   PDSM_XTRA_CMD_E_SIZE PDSM_LITE_XTRA_CMD_E_SIZE  /* To force enum to 32 bit for windows NT */

typedef pdsm_lite_xtra_cmd_e_type pdsm_xtra_cmd_e_type;

/*~ SENTINEL pdsm_lite_xtra_cmd_e_type.PDSM_LITE_XTRA_CMD_MAX */

/* Enumeration of gpsOneXTRA Client Status
*/
typedef pdsm_lite_xtra_dc_status_e_type pdsm_xtra_dc_status_e_type;
#define PDSM_XTRA_STATUS_FAILURE PDSM_LITE_XTRA_STATUS_FAILURE
#define PDSM_XTRA_STATUS_SUCCESS PDSM_LITE_XTRA_STATUS_SUCCESS

/* Enumeration of gpsOneXTRA Download status
*/

typedef pdsm_lite_xtra_status_e_type pdsm_xtra_status_e_type;



/* Enumeration for gpsOneXTRA retrieve command
*/

typedef pdsm_lite_xtra_command_e_type pdsm_xtra_command_e_type;

/* Events for gpsOneXTRA 
*/
typedef int pdsm_xtra_event_type;
#define PDSM_XTRA_EVENT_NONE               PDSM_LITE_XTRA_EVENT_NONE
#define PDSM_XTRA_EVENT_DOWNLOAD_REQ       PDSM_LITE_XTRA_EVENT_DOWNLOAD_REQ
#define PDSM_XTRA_EVENT_STATUS             PDSM_LITE_XTRA_EVENT_STATUS
#define PDSM_XTRA_EVENT_TIME_REQ           PDSM_LITE_XTRA_EVENT_TIME_REQ
#define PDSM_XTRA_EVENT_CFG_REQ            PDSM_LITE_XTRA_EVENT_CFG_REQ

#define PDSM_XTRA_EVENT_MASK   PDSM_LITE_XTRA_EVENT_MASK

/* Download info field validiy bit flags
*/
/*Modify ?*/
#define XTRA_DOWNLOAD_INFO_FLAGS_STATUS             0x00000001
#define XTRA_DOWNLOAD_INFO_FLAGS_COMMAND            0x00000002
#define XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_WEEK     0x00000004
#define XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_MINS     0x00000008
#define XTRA_DOWNLOAD_INFO_FLAGS_VALID_DURATION     0x00000010
#define XTRA_DOWNLOAD_INFO_FLAGS_SERVER1            0x00000020
#define XTRA_DOWNLOAD_INFO_FLAGS_SERVER2            0x00000040
#define XTRA_DOWNLOAD_INFO_FLAGS_SERVER3            0x00000080
#define XTRA_DOWNLOAD_INFO_FLAGS_MAX_FILE_PART_SIZE 0x00000100
#define XTRA_DOWNLOAD_INFO_FLAGS_MAX_FILE_SIZE      0x00000200

/* Structure used as payload for gpsOneXTRA events 
** 1. PDSM_LITE_XTRA_EVENT_DOWNLOAD_REQ
** 2. PDSM_LITE_XTRA_EVENT_STATUS
*/
typedef pdsm_lite_xtra_download_info_s_type pdsm_xtra_download_info_s_type;

/* Structure used as payload for gpsOneXTRA events 
** 1. PDSM_LITE_XTRA_EVENT_TIME_REQ
*/
typedef pdsm_lite_xtra_time_info_req_s_type pdsm_xtra_time_info_req_s_type;
/*Modify ?*/
#define XTRA_CFG_INFO_MASK_ABS_AGE         (0x00000001)   // How many hours that the current XTRA information is valid
#define XTRA_CFG_INFO_MASK_REL_AGE         (0x00000002)   //Last XTRA data download time
#define XTRA_CFG_INFO_MASK_XTRA_SERVER     (0x00000004)   // XTRA server URLs
#define XTRA_CFG_INFO_MASK_NTP_SERVER      (0x00000008)   //Network Time Protocol(NTP) server URLs
#define XTRA_CFG_INFO_MASK_TIME_REQUEST    (0x00000010)   //Time requested
#define XTRA_CFG_INFO_MASK_PREF_VALID_AGE  (0x00000020)   // Prefered valid age for the xtra file


typedef pdsm_lite_xtra_config_info_s_type pdsm_xtra_config_info_s_type;

/* Union of the gpsOneXTRA download or time data
*/
typedef  pdsm_lite_xtra_req_u_type pdsm_xtra_req_u_type;
/*~ IF ((_DISC_ == PDSM_LITE_XTRA_EVENT_DOWNLOAD_REQ) || (_DISC_ == PDSM_LITE_XTRA_EVENT_STATUS)) pdsm_lite_xtra_req_u.xtra_download_info */
/*~ IF (_DISC_ == PDSM_LITE_XTRA_EVENT_TIME_REQ)     pdsm_lite_xtra_req_u.xtra_time_info */
/*~ DEFAULT pdsm_lite_xtra_req_u.void */


/* Structure for gpsOneXTRA events and requests
*/
typedef pdsm_lite_xtra_info_s_type pdsm_xtra_info_s_type;
/*~ FIELD pdsm_lite_xtra_info_s.xtra_union DISC pdsm_lite_xtra_info_s.xtra_event */


typedef pdsm_lite_xtra_time_info_type pdsm_xtra_time_info_type;

typedef pdsm_lite_xtra_config_req_info_type pdsm_xtra_config_req_info_type;


/*-------------- End eXtended Receiver Assistance (XTRA) ------------------*/

/*-------------- eXtended Receiver Assistance Terrestrial (XTRA-T) ------------------*/
/* XTRA-T Command Type */

#define PDSM_XTRA_T_CMD_NONE PDSM_LITE_XTRA_T_CMD_NONE 		 /* FOR INTERNAL USE OF PDSM ONLY! */
#define PDSM_XTRA_T_CMD_GET_DATA PDSM_LITE_XTRA_T_CMD_GET_DATA			 /* XTRA-T Client is getting XTRA-T data from PDAPI */
#define PDSM_XTRA_T_CMD_SET_DATA PDSM_LITE_XTRA_T_CMD_SET_DATA			 /* XTRA-T Client is injecting XTRA-T data to PDAPI */
#define PDSM_XTRA_T_CMD_INITIATE_DOWNLOAD PDSM_LITE_XTRA_T_CMD_INITIATE_DOWNLOAD  /* Requesting XTRA-T client to download XTRA-T data */
#define PDSM_XTRA_T_CMD_INITIATE_UPLOAD PDSM_LITE_XTRA_T_CMD_INITIATE_UPLOAD	 /* Requesting XTRA-T client to upload XTRA-T data */

#define PDSM_XTRA_T_CMD_MAX PDSM_LITE_XTRA_T_CMD_MAX				 /* FOR INTERNAL USE OF PDSM ONLY! */
#define PDSM_XTRA_T_CMD_E_SIZE PDSM_LITE_XTRA_T_CMD_E_SIZE  /* To force enum to 32 bit for windows NT */

typedef pdsm_lite_xtra_t_cmd_e_type pdsm_xtra_t_cmd_e_type;
/*~ SENTINEL pdsm_lite_xtra_t_cmd_e_type.PDSM_LITE_XTRA_T_CMD_MAX */


/* XTRA-T Event Mask */
typedef int pdsm_xtra_t_event_type;
#define PDSM_XTRA_T_EVENT_NONE                          PDSM_LITE_XTRA_T_EVENT_NONE
#define PDSM_XTRA_T_EVENT_SESS_BEGIN                    PDSM_LITE_XTRA_T_EVENT_SESS_BEGIN  /*  GPS engine initiates  XTRA-T */
#define PDSM_XTRA_T_EVENT_DATA                          PDSM_LITE_XTRA_T_EVENT_DATA  /*  GPS engine XTRA-T data transfer */
#define PDSM_XTRA_T_EVENT_SESS_DONE                     PDSM_LITE_XTRA_T_EVENT_SESS_DONE  /*  GPS engine requests a termination of XTRA-T session */
#define PDSM_XTRA_T_EVENT_SESS_END                      PDSM_LITE_XTRA_T_EVENT_SESS_END  /*  GPS engine XTRA-T  session abort due to errors */
#define PDSM_XTRA_T_EVENT_DOWNLOAD_REQ                  PDSM_LITE_XTRA_T_EVENT_DOWNLOAD_REQ  /* GPS engine initiates XTRA-T download  - Aries 5.0 */
#define PDSM_XTRA_T_EVENT_UPLOAD_REQ                    PDSM_LITE_XTRA_T_EVENT_UPLOAD_REQ  /* GPS engine initiates XTRA-T upload  - Aries 5.0 */

#define PDSM_XTRA_T_EVENT_MASK PDSM_LITE_XTRA_T_EVENT_MASK

/* XTRA-T payload for PDSM_LITE_XTRA_T_EVENT_SESS_END: data part report to XTRA-T client */
typedef pdsm_lite_xtra_t_get_celldb_report_s_type pdsm_xtra_t_get_celldb_report_s_type;
/*~ FIELD pdsm_lite_xtra_t_get_celldb_report_s.xtra_t_data_ptr POINTER */


/* XTRA-T payload for PDSM_LITE_XTRA_T_EVENT_DATA: XTRA-T session end reason */
/* This is also used for sending status of individual Segmented CellDB message to TM XTRA-T Module */
#define PDSM_XTRA_T_END_NONE	             PDSM_LITE_XTRA_T_END_NONE
#define PDSM_XTRA_T_END_SUCCESS             PDSM_LITE_XTRA_T_END_SUCCESS		/* XTRA-T session success */
#define PDSM_XTRA_T_END_BUSY                PDSM_LITE_XTRA_T_END_BUSY
#define PDSM_XTRA_T_END_OTHER               PDSM_LITE_XTRA_T_END_OTHER
#define PDSM_XTRA_T_END_NO_CLIENT           PDSM_LITE_XTRA_T_END_NO_CLIENT
#define PDSM_XTRA_T_END_FEATURE_NOT_ENABLED PDSM_LITE_XTRA_T_END_FEATURE_NOT_ENABLED
#define PDSM_XTRA_T_END_BAD_BUFFER_LENGTH   PDSM_LITE_XTRA_T_END_BAD_BUFFER_LENGTH
#define PDSM_XTRA_T_END_INTERNAL_RESOURCE   PDSM_LITE_XTRA_T_END_INTERNAL_RESOURCE
#define PDSM_XTRA_T_END_CLIENT_REPORT_ERR   PDSM_LITE_XTRA_T_END_CLIENT_REPORT_ERR
#define PDSM_XTRA_T_END_DTR_PROC_ERR        PDSM_LITE_XTRA_T_END_DTR_PROC_ERR
#define PDSM_XTRA_T_END_MAX                 PDSM_LITE_XTRA_T_END_MAX				/* FOR INTERNAL USE OF PDSM ONLY! */
#define PDSM_XTRA_T_END_E_SIZE              PDSM_LITE_XTRA_T_END_E_SIZE  /* To force enum to 32 bit for windows NT */


typedef pdsm_lite_xtra_t_status_e_type pdsm_xtra_t_status_e_type;


/*
** TM XTRA-T Session Type
*/
#define PDSM_XTRA_T_SESSION_NONE		PDSM_LITE_XTRA_T_SESSION_NONE  /* Invalid session type							  */
#define PDSM_XTRA_T_SESSION_UPLOAD_DTR PDSM_LITE_XTRA_T_SESSION_UPLOAD_DTR	  /* Upload Session using Cell DB data in DTR packets */
#define PDSM_XTRA_T_SESSION_MAX        PDSM_LITE_XTRA_T_SESSION_MAX
#define PDSM_XTRA_T_SESSION_E_SIZE 	PDSM_LITE_XTRA_T_SESSION_E_SIZE	 /* Force to 1 byte */

typedef pdsm_lite_xtra_t_session_e_type pdsm_xtra_t_session_e_type;


/* XTRA-T Event Callback: payload definition */
typedef pdsm_lite_xtra_t_req_u_type pdsm_xtra_t_req_u_type;
/*~ IF (_DISC_ == PDSM_LITE_XTRA_T_EVENT_DATA) pdsm_lite_xtra_t_req_u.data_report */
/*~ IF (_DISC_ == PDSM_LITE_XTRA_T_EVENT_SESS_END)     pdsm_lite_xtra_t_req_u.end_reason */
/*~ DEFAULT pdsm_lite_xtra_t_req_u.void */

/* XTRA-T Event Callback Data: event + payload */
typedef pdsm_lite_xtra_t_info_s_type pdsm_xtra_t_info_s_type;
/*~ FIELD pdsm_lite_xtra_t_info_s.xtra_t_event_payload DISC pdsm_lite_xtra_t_info_s.xtra_t_event */
/* XTRA-T:  pdsm_lite_xtra_t_get_data() related */

#define PDSM_XTRA_T_GET_ALL PDSM_LITE_XTRA_T_GET_ALL				   /* All cells in CellDB */
#define PDSM_XTRA_T_GET_ALL_CHANGED PDSM_LITE_XTRA_T_GET_ALL_CHANGED		   /* All changed cells in CellDB (changed = dirty bit is ON) */
#define PDSM_XTRA_T_GET_CHANGED_IN_N_MINUTES PDSM_LITE_XTRA_T_GET_CHANGED_IN_N_MINUTES /* Changed cells in CellDB for the last N minutes */
#define PDSM_XTRA_T_GET_LAST_N_CHANGED PDSM_LITE_XTRA_T_GET_LAST_N_CHANGED	  /* The last N changed cells in CellDB  */
#define PDSM_XTRA_T_GET_MAX PDSM_LITE_XTRA_T_GET_MAX				   /* FOR INTERNAL USE OF PDSM ONLY! */
#define PDSM_XTRA_T_GET_E_SIZE PDSM_LITE_XTRA_T_GET_E_SIZE	/* To force enum to 32 bit for windows NT */

typedef pdsm_lite_xtra_t_get_e_type pdsm_xtra_t_get_e_type;

/* XTRA-T:  pdsm_lite_xtra_t_get_data() related */
typedef pdsm_lite_xtra_t_get_s_type pdsm_xtra_t_get_s_type;
/*-------------- End eXtended Receiver Assistance Terrestrial (XTRA-T) ------------------*/

/*-------------- Start (WIPER) ------------------*/

/*-------------- End (WIPER) ------------------*/

/*---------------------------------------------------------------------------
              QMI Services Enable Type
---------------------------------------------------------------------------*/

typedef uint32 pdsm_QMIServiceEnableMaskType;
/* definitions for QMI service configuration */
#define PDSM_QMI_CONFIG_ENABLE_LOC           PDSM_LITE_QMI_CONFIG_ENABLE_LOC

typedef  pdsm_lite_QMIServiceConfig pdsm_QMIServiceConfig;

/*-------------- End QMI Services Enable Type ------------------*/

/* <EJECT> */
/*---------------------------------------------------------------------------
                      CLIENT RELATED TYPES & FUNCTIONS
---------------------------------------------------------------------------*/

/* Types for PDSM event callback functions.
**
** A client that want to be notified of PD events needs to register
** with the appropriate type of callback function with PDSM.
**
** PDSM calls on the client's callback functions when events to which the
** client is registered for occur. Each client is responsible for the
** implementation of its callback functions. Note that since callback
** functions are being called from PDSM context they are expected to
** only queue commands to their corresponding client and return. No extra
** processing is allowed and in particular no BLOCKING OS calls 
** like os_WaitLocalSignal().
**
** NOTE! callback functions are not allowed to change the content
** of the info structure which they are given a pointer.
**
** NOTE! clients are not allowed to access the info structure after
** the their callback functions return.
*/


/* Type for PD event callback function.
**
** A client that want to be notified of PD events needs to register
** with such a function with PDSM.
*/

typedef void (pdsm_pd_event_f_type)(
   void                         *user_data,  /* user data pointer */

   pdsm_lite_pd_event_type           pd_event,    /* Indicate PD event */

   const pdsm_lite_pd_info_s_type    *pd_info_ptr /* A pointer to a PD state information struct */
   /*~ PARAM pd_info_ptr POINTER */

   );

/* Type for PD event callback function.
**
** A client that want to be notified of PD events needs to register
** with such a function with PDSM.
*/

typedef void (pdsm_pa_event_f_type)(
   void                         *user_data,  /* user data pointer */

   pdsm_lite_pa_event_type           pa_event,    /* Indicate PD event */

   const pdsm_lite_pa_info_s_type    *pa_info_ptr /* A pointer to a PD state information struct */
   /*~ PARAM pa_info_ptr POINTER */

   );

/* After PA event max-out, this is the new PA event-reporting function, no need to define new PA bitmask-style
*  event for each new PDSM_LITE_PA_xxx enum, the enum will be used here directly. client will not be able to register
*  certain events selectively, all PA events will be reported to registered PA client
*/
typedef void (pdsm_pa_event_ex_f_type)(
   void                         *user_data,  /* user data pointer */

   pdsm_lite_pa_e_type               pa_event,    /* Indicate PA event, PDSM_LITE_PA_xxx enum */

   const pdsm_lite_pa_info_s_type    *pa_info_ptr, /* A pointer to a PD state information struct */
   /*~ PARAM pa_info_ptr POINTER */

   pdsm_lite_pa_cmd_err_e_type        status    /* Optional field to indicate success/failure status of PA operation */
   );



typedef void (pdsm_xtra_event_f_type)(
   void *user_data,
   pdsm_lite_xtra_event_type  xtra_event,
   const pdsm_lite_xtra_info_s_type *xtra_info_ptr
   /*~ PARAM xtra_info_ptr POINTER */
   );


/* Type for XTRA-T event callback function */
typedef void (pdsm_xtra_t_event_f_type)(
   void                          *user_data,
   pdsm_lite_xtra_t_event_type         xtra_t_event,     /* See XTRA-T event mask */
   const pdsm_lite_xtra_t_info_s_type *xtra_t_info_ptr   /* see XTRA-T event info */
   /*~ PARAM xtra_t_info_ptr POINTER */
   );


/* Type for PD command error callback function.
**
** A client that want to be notified of OTHER clients PD command errors
** needs to register with such a function with PDSM.
*/
typedef void (pdsm_pd_cmd_err_f_type)(

   pdsm_lite_client_id_type            client_id,
   /* Indicate which client caused the error */

   pdsm_lite_pd_cmd_e_type             cmd,
   /* Indicate for which command this error is reported */

   pdsm_lite_pd_cmd_err_e_type         cmd_err
   /* Indicate nature of error */

   );

/* Type for PD command error callback function.
**
** A client that want to be notified of OTHER clients PD command errors
** needs to register with such a function with PDSM.
*/
typedef void (pdsm_pa_cmd_err_f_type)(

   pdsm_lite_client_id_type         client_id,
   /* Indicate which client caused the error */

   pdsm_lite_pa_cmd_e_type             cmd,
   /* Indicate for which command this error is reported */

   pdsm_lite_pa_cmd_err_e_type         cmd_err
   /* Indicate nature of error */

   );

/* Type for PD command error callback function.
**
** A client that want to be notified of OTHER clients PD command errors
** needs to register with such a function with PDSM.
*/
typedef void (pdsm_ext_status_cmd_err_f_type)(

   pdsm_lite_client_id_type         client_id
   /* Indicate which client caused the error */

   );

typedef void (pdsm_xtra_cmd_err_f_type)(

   pdsm_lite_client_id_type         client_id,
   /* Indicate which client caused the error */

   pdsm_lite_xtra_cmd_e_type             cmd,
   /* Indicate for which command this error is reported */

   pdsm_lite_xtra_cmd_err_e_type         cmd_err
   /* Indicate nature of error */
   );


typedef void (pdsm_xtra_t_cmd_err_f_type)(

   pdsm_lite_client_id_type         client_id,
   /* Indicate which client caused the error */

   pdsm_lite_xtra_t_cmd_e_type             cmd,
   /* Indicate for which command this error is reported */

   pdsm_lite_xtra_t_cmd_err_e_type         cmd_err
   /* Indicate nature of error */
   );

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of client API return status*/

#define PDSM_CLIENT_ERR_NONE PDSM_LITE_CLIENT_ERR_NONE 		 /* FOR INTERNAL USE OF PDSM ONLY! */

#define PDSM_CLIENT_OK PDSM_LITE_CLIENT_OK				   /* No error, request was completed successfully */

#define PDSM_CLIENT_ERR_CLIENT_PTR PDSM_LITE_CLIENT_ERR_CLIENT_PTR		/* Invalid client ID pointer*/

#define PDSM_CLIENT_ERR_CLIENT_ID PDSM_LITE_CLIENT_ERR_CLIENT_ID 	   /* Invalid client ID */

#define PDSM_CLIENT_ERR_CLIENT_TYPE PDSM_LITE_CLIENT_ERR_CLIENT_TYPE	   /* Invalid Client type */

#define PDSM_CLIENT_ERR_EVENT_REG_TYPE PDSM_LITE_CLIENT_ERR_EVENT_REG_TYPE   /* Invalid event registration type */

#define PDSM_CLIENT_ERR_EVENT_MASK PDSM_LITE_CLIENT_ERR_EVENT_MASK	   /* Invalid event mask */

#define PDSM_CLIENT_ERR_CLIENT_INIT_FAIL PDSM_LITE_CLIENT_ERR_CLIENT_INIT_FAIL /* Unable to initialize the client */

#define PDSM_CLIENT_ERR_SESSION_ACTIVE PDSM_LITE_CLIENT_ERR_SESSION_ACTIVE   /* Unable to release a client due to active session */

#define PDSM_CLIENT_ERR_CLIENT_ACTIVE PDSM_LITE_CLIENT_ERR_CLIENT_ACTIVE    /* Unable to release an active client */

#define PDSM_CLIENT_ERR_MAX PDSM_LITE_CLIENT_ERR_MAX			   /* FOR INTERNAL USE OF PDSM ONLY! */

#define PDSM_CLIENT_ERR_E_SIZE PDSM_LITE_CLIENT_ERR_E_SIZE	/* To force enum to 32 bit for windows NT */

typedef pdsm_lite_client_status_e_type pdsm_client_status_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of client event registration type */

#define PDSM_CLIENT_EVENT_NONE PDSM_LITE_CLIENT_EVENT_NONE		   /* FOR INTERNAL USE OF PDSM ONLY! */

#define PDSM_CLIENT_EVENT_REG PDSM_LITE_CLIENT_EVENT_REG 			 /* event registration */

#define PDSM_CLIENT_EVENT_DEREG PDSM_LITE_CLIENT_EVENT_DEREG			 /* event de-registration */

#define PDSM_CLIENT_EVENT_MAX PDSM_LITE_CLIENT_EVENT_MAX 			 /* FOR INTERNAL USE OF PDSM ONLY! */

#define PDSM_CLIENT_EVENT_BIG PDSM_LITE_CLIENT_EVENT_BIG   /* To force enum to int */

typedef pdsm_lite_client_event_reg_e_type pdsm_client_event_reg_e_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*
    This structure contains the options for and end request. 

    If the receiver off is set to false, then if no session is 
    running and receiver is ON due to extended ON feature or due 
    to LM apptracking feature, the receiver wont be turned off. 

    If the boolean was set, it will result in sending receiver off
    to MGP.
*/
typedef pdsm_lite_pd_end_session_opt_s_type pdsm_pd_end_session_opt_s_type;
/*===========================================================================

FUNCTION pdsm_lite_client_init

DESCRIPTION
  Initializing a client object.

  This function must be called exactly once on a client object before
  it is being used, in any way, place, or form. If this function is called
  more that once the same data structure get reassigned and reinitialized.

  Note that in order to get notified of PDSM events a client still needs
  to do the following:
  1. Register its callback functions and specific events with each functional group
  2. Activate its client object with pdsm_lite_client_act()

DEPENDENCIES
  none

RETURN VALUE
  Assigned Handle (client id) to be used for interfacing with PDSM.

SIDE EFFECTS
  none

===========================================================================*/
extern pdsm_lite_client_id_type pdsm_client_init(

   pdsm_client_type_e_type           client_type
   /* Type of client */
   );
/*~ FUNCTION pdsm_lite_client_init 
    RELEASE_FUNC pdsm_lite_client_release(_RESULT_) */

/*===========================================================================

FUNCTION pdsm_lite_client_release

DESCRIPTION
  Release a client object.

  This function should be called if a client doesn't need PDSM anymore.
  This will release the client object. That object can be reused for
  another client. If the client needs to use PDSM again, it should PD
  pdsm_lite_client_init function to establish connection.

DEPENDENCIES
  none

RETURN VALUE
  request staus

SIDE EFFECTS
  none

===========================================================================*/
extern pdsm_lite_client_status_e_type pdsm_client_release(

   pdsm_client_id_type               client_id
   /* Client ID pointer*/
   );
/*~ FUNCTION pdsm_lite_client_release */


/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_lite_client_pd_reg

DESCRIPTION
  This function allows a client to register the following PD-related
  callbacks and events with PDSM:

   1. Callback function to receive registered PD events
   2. Register and de-register PD events
   3. Callback function to receive other clients' PD-command errors.

   If a client registers a callback function to receive PD event
   notification, it also needs to register one or more PD events it is
   interested in receiving.

  If a client registers a callback function to receive other clients'
  PD-command errors, it will receive notifications on all possible
  PD-command errors caused by other clients. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' PD-command errors, it can set
  call_cmd_err_func = NULL. However, if the call_event_func is set to NULL,
  all registered PD events are also de-registered and the event_mask_type
  parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with pdsm_lite_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
pdsm_client_status_e_type pdsm_client_pd_reg(

   pdsm_client_id_type           client_id,
   /* Requesting client */
   void                          *client_data_ptr,

   pdsm_pd_event_f_type          *pd_event_func,
   /* Pointer to a callback function to notify the client of PD
                                             ** events */
   /*~ PARAM   pd_event_func POINTER */

   pdsm_client_event_reg_e_type  event_reg_type,

   pdsm_pd_event_type            event_mask,
   /* register from this event (including) */

   pdsm_pd_cmd_err_f_type        *pd_cmd_err_func
   /* pointer to a callback function to notify the client of PD
                                             ** command errors of OTHER clients */
   /*~ PARAM pd_cmd_err_func POINTER */
   );
/*~ FUNCTION pdsm_lite_client_pd_reg */

/*===========================================================================

FUNCTION pdsm_lite_client_pa_reg

DESCRIPTION
  This function allows a client to register the following Parameter-related
  callbacks and events with PDSM:

   1. Callback function to receive registered Parameter events
   2. Register and de-register PArameter write events events
   3. Callback function to receive other clients' PD-command errors.

  Event reporting in this function only works for PA operation upto & including
  PDSM_LITE_PA_LPP_CONFIG_INFO. Afterwards, client should call pdsm_lite_client_pa_reg_ex()
  to register for PA events
 
DEPENDENCIES
  Client must have already been initialized with pdsm_lite_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
pdsm_client_status_e_type pdsm_client_pa_reg(

   pdsm_client_id_type              client_id,
   /* Requesting client */
   void                             *client_data_ptr,

   pdsm_pa_event_f_type             *pa_event_func,
   /* Pointer to a callback function to notify the client of PA
                                             ** events */
   /*~ PARAM pa_event_func POINTER */
   pdsm_client_event_reg_e_type     event_reg_type,

   pdsm_pa_event_type               event_mask,
   /* register from this event (including) */

   pdsm_pa_cmd_err_f_type           *pa_cmd_err_func
   /* pointer to a callback function to notify the client of PD
                                             ** command errors of OTHER clients */
   /*~ PARAM pa_cmd_err_func POINTER */
   );
/*~ FUNCTION pdsm_lite_client_pa_reg */

/*===========================================================================

FUNCTION pdsm_lite_client_xtra_reg 

DESCRIPTION
This function allows a client to register the following UI-related
callbacks and events with PDSM:

DEPENDENCIES
Client must have already been initialized with pdsm_lite_client_init().

RETURN VALUE
request status

SIDE EFFECTS
none

===========================================================================*/
extern pdsm_client_status_e_type pdsm_client_xtra_reg
(
   pdsm_client_id_type           client_id,
   /* Requesting client */
   void                          *client_data_ptr,

   pdsm_xtra_event_f_type          *xtra_event_func,
   /* Pointer to a callback function to notify the client of XTRA events */
   /*~ PARAM xtra_event_func POINTER */
   pdsm_client_event_reg_e_type  event_reg_type,

   pdsm_xtra_event_type            event_mask,
   /* register from this event (including) */

   pdsm_xtra_cmd_err_f_type        *xtra_cmd_err_func
   /* pointer to a callback function to notify the client of XTRA
 ** command errors of OTHER clients */
   /*~ PARAM xtra_cmd_err_func POINTER */
   );

/*===========================================================================

FUNCTION pdsm_lite_client_pa_reg_ex

DESCRIPTION
  This function allows a client to register the following Parameter-related
  callbacks with PDSM:

   1. Callback function to receive Parameter events, all PA events will be
      reported, client will not be able to selectively register certain
      events.
   2. Register and de-register Parameter events
   3. Callback function to receive other clients' PD-command errors.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  If client calls this function to register PA events, all PA events 
  will be reported via the registered callback function "pa_event_func_ex", 
  including all PA operations upto & including PDSM_PA_LPP_CONFIG_INFO. 

  If client also calls legacy pdsm_client_pa_reg() function to register for the
  old bitmask-style PA event reporting, client will get event via the registered
  callback function "pa_event_func" only for PA operation upto & including
  PDSM_PA_LPP_CONFIG_INFO (client will get event twice in this case, client
  needs to properly handle such case).
 
  If client register using both methods (pdsm_client_pa_reg() & pdsm_client_pa_reg_ex())
  it should also call both legacy & new interface to deregister completely.

===========================================================================*/
pdsm_client_status_e_type pdsm_client_pa_reg_ex(

    pdsm_client_id_type              client_id,
        /* Requesting client */
    void                             *client_data_ptr,

    pdsm_pa_event_ex_f_type          *pa_event_func_ex,
        /* Pointer to a callback function to notify the client of PA
        ** events */
    /*~ PARAM pa_event_func_ex POINTER */
    pdsm_client_event_reg_e_type     event_reg_type,

    pdsm_pa_cmd_err_f_type           *pa_cmd_err_func
      /* pointer to a callback function to notify the client of PD
      ** command errors of OTHER clients */
      /*~ PARAM pa_cmd_err_func POINTER */ 
);


/*~ FUNCTION pdsm_lite_client_xtra_reg */

/*===========================================================================

FUNCTION pdsm_lite_client_xtra_t_reg 

DESCRIPTION
This function allows a XTRA-T client to register the following UI-related
callbacks and events with PDSM:

DEPENDENCIES
Client must have already been initialized with pdsm_lite_client_init().

RETURN VALUE
request status

SIDE EFFECTS
none

===========================================================================*/
extern pdsm_client_status_e_type pdsm_client_xtra_t_reg
(
   pdsm_lite_client_id_type 		  client_id,
	 /* Requesting client */
	 void						   *client_data_ptr,
   
	 pdsm_xtra_t_event_f_type			*xtra_t_event_func,
	 /* Pointer to a callback function to notify the client of PD
   ** events */
	 /*~ PARAM xtra_t_event_func POINTER */
   
	 pdsm_client_event_reg_e_type	event_reg_type,
	 /* Register or deregister events */
   
	 pdsm_lite_xtra_t_event_type			xtra_t_event_mask,
	 /* register from this event (including) */
   
	 pdsm_xtra_t_cmd_err_f_type		*xtra_t_cmd_err_func
	 /* pointer to a callback function to notify the client of XTRA-T  
   ** command errors of OTHER clients */
	 /*~ PARAM xtra_t_cmd_err_func POINTER */
   );
/*~ FUNCTION pdsm_lite_client_xtra_t_reg */
/* <EJECT> */





/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_lite_client_act

DESCRIPTION
  Activate the client object, so that the client will be notified
  of postion determination session manager events (for which it is registered).

DEPENDENCIES
  Client must have already been initialized with pdsm_lite_client_init().

RETURN VALUE
   request status

SIDE EFFECTS
  Add the client object to the client list.

===========================================================================*/
extern pdsm_client_status_e_type pdsm_client_act(

   pdsm_client_id_type               client_id
   /* Requesting client */
   );
/*~ FUNCTION pdsm_lite_client_act */

/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_lite_client_deact

DESCRIPTION
  Deactivate a client object, so that the client will NOT be notified
  of Position Determination Session Manager events (for which it is registered).

DEPENDENCIES
  Client must have already been initialized with pdsm_lite_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  Remove the client object from the client list. If a session is active for this
  client only then the session is terminated otherwise session is made active with
  activation client ID as next requesting client.

===========================================================================*/
extern pdsm_client_status_e_type pdsm_client_deact(

   pdsm_client_id_type               client_id
   /* Requesting client */

   );

/*~ FUNCTION pdsm_lite_client_deact */


/* <EJECT> */
/*---------------------------------------------------------------------------
                      COMMAND RELATED TYPES & FUNCTIONS
---------------------------------------------------------------------------*/


/* Type for PD command callback functions.
**
** A client that wants to be notified of the result of a  PD command
** needs to supply a pointer to a function of this type as the first
** parameter to a PD-command-function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, PDSM context simultaneously.
*/
typedef pdsm_lite_pd_cmd_cb_f_type pdsm_pd_cmd_cb_f_type;


/* Type for PA command callback functions.
**
** A client that wants to be notified of the result of a  PA command
** needs to supply a pointer to a function of this type as the first
** parameter to a PA-command-function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, PDSM context simultaneously.
*/
typedef pdsm_lite_pa_cmd_cb_f_type pdsm_pa_cmd_cb_f_type;

typedef pdsm_lite_ext_pos_cmd_cb_f_type pdsm_ext_pos_cmd_cb_f_type;



/* Type for XTRA command callback functions.
**
** A client that wants to be notified of the result of an XTRA command
** needs to supply a pointer to a function of this type as the first
** parameter to a XTRA-command-function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, PDSM context simultaneously.
*/
typedef pdsm_lite_xtra_cmd_cb_f_type pdsm_xtra_cmd_cb_f_type ;

/* Type for  XTRA-T Command Callback Function
**
** A client that wants to be notified of the result of an XTRA-T command
** needs to supply a pointer to a function of this type as the first
** parameter to a XTRA-T-command-function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, PDSM context simultaneously.
*/
typedef pdsm_lite_xtra_t_cmd_cb_f_type pdsm_xtra_t_cmd_cb_f_type;

/* <EJECT> */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* PDSM command functions
**
** The PDSM command functions defines the Client->PDSM interface.
**
** Each command function takes a pointer to a client provided callback
** function, a client provided void pointer and the command's required set
** of fields.
**
** If the command function find one or more of the parameters to contain
** invalid values ( e.g. not one of the enum values for an enum parameter),
** it calls on the client's callback function IMMEDIATELY with the
** appropriate command error code. NOTE that in this case the callback
** function is actually being called from the client context - NOT PDSM.
**
** If all parameters found to be valid, the command function queues a
** command to PDSM. When PDSM gets the command it checks whether the command
** is valid relative to the current state of the phone.
**
** In case the command is not valid, PDSM calls on the client callback function
** with the appropriate command error code. NOTE that in this case the callback
** function is indeed being called from the PDSM context.
**
** If the command is successful, PDSM calls on the client callback function
** with an error code that indicates successful command. In addition PDSM
** notified registered clients of the corresponding PDSM event (e.g. if a
** client successfully ends a session, PDSM notify clients that are registered
** for the pd-end-event).
*/


/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_lite_get_position

DESCRIPTION
  Command PDSM to obtain PD information.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_lite_client_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
extern boolean pdsm_get_position(
   pdsm_pd_cmd_cb_f_type    pd_cb_func,         /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,   /* user data pointer */
   pdsm_pd_option_s_type    *option_ptr,      /* Fix rate for tracking sessions. */
   /*~ PARAM option_ptr POINTER */
   pdsm_pd_qos_type         *qos_ptr,           /* Ptr to user QOS for this request */
   /*~ PARAM qos_ptr POINTER */
   pdsm_client_id_type      client_id           /* Client id of user */
   );
/*~ FUNCTION pdsm_lite_get_position */

/*===========================================================================

FUNCTION pdsm_lite_end_session

DESCRIPTION
  Command PDSM to end PD session.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_lite_client_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/

extern boolean pdsm_end_session(
   pdsm_pd_cmd_cb_f_type        pd_cb_func,        /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                         *client_data_ptr,  /* Client specified ptr */
   pdsm_client_id_type          client_id          /* requesting client ID */
   );
/*~ FUNCTION pdsm_lite_end_session */

/*===========================================================================

FUNCTION pdsm_lite_pd_get_best_avail_pos

DESCRIPTION
  This API initiates a command to the engine to return the best available position. 
The response to this request is an asynchronous fix report sent to PDAPI. There are 
no assumptions made on the response delay for this fix .  

DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_lite_pd_get_best_avail_pos
(
   pdsm_pd_cmd_cb_f_type   cmd_cb_func, /* command result callback function */
   /*~ PARAM pa_motion_data_cb_func POINTER */
   pdsm_client_id_type   client_id,
   void   *client_data_ptr /* client provided reference data pointer */
   );
/*===========================================================================

FUNCTION pdsm_pd_get_avail_wwan_pos

DESCRIPTION
  This API initiates a command to the engine to return the available WWAN position. 
The response to this request is an asynchronous fix report sent to PDAPI. There are 
no assumptions made on the response delay for this fix.

DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_pd_get_avail_wwan_pos
(
  pdsm_pd_cmd_cb_f_type   cmd_cb_func, /* command result callback function */
  /*~ PARAM pa_motion_data_cb_func POINTER */
  pdsm_client_id_type     client_id,
  void                   *client_data_ptr /* client provided reference data pointer */
);

/*===========================================================================
FUNCTION pdsm_lite_pd_inject_external_time
DESCRIPTION
  Command PDSM to inject time information from an external clock.
  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
DEPENDENCIES
  pdsm_lite_client_init( ) must have been already done.
RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request
SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.
===========================================================================*/
extern boolean pdsm_pd_inject_external_time(
   pdsm_pd_cmd_cb_f_type    pd_cb_func,         /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,   /* user data pointer */
   pdsm_pd_external_time_info_type    *ext_time_ptr,        /* external time */
   /*~ PARAM ext_time_ptr POINTER */
   pdsm_client_id_type      client_id           /* Client id of user */
   );
/*~ FUNCTION pdsm_lite_pd_inject_external_time */
   
/*===========================================================================
DESCRIPTION
  Command PDSM to inject coarse position information from an external source
  with reliability associated with the injected position.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
DEPENDENCIES
  pdsm_lite_client_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
extern boolean pdsm_pd_inject_external_position(
   pdsm_pd_cmd_cb_f_type    cmd_cb_func,           /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,      /* user data pointer */
   pdsm_pd_external_position_info_type  *ext_pos_ptr, /* external position */
   /*~ PARAM ext_pos_ptr POINTER */
   pdsm_client_id_type      client_id              /* Client id of user */
   );
/*~ FUNCTION pdsm_lite_pd_inject_external_position */


/*===========================================================================
FUNCTION pdsm_lite_pd_query_gps_state
DESCRIPTION
  Command PDSM to return the state information of the GPS engine.
  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
DEPENDENCIES
  pdsm_lite_client_init( ) must have been already done.
RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request
SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM extended status events.
===========================================================================*/
extern boolean pdsm_pd_query_gps_state(
   pdsm_pd_cmd_cb_f_type     cmd_cb_func,       /* Command callback */
   /*~ PARAM cmd_cb_func POINTER */
   void                      *client_data_ptr,  /* Not sure if this is required */
   /*~ PARAM gps_state_ptr POINTER */
   pdsm_client_id_type       client_id          /* Requesting client id */
   );
/*~ FUNCTION pdsm_lite_pd_query_gps_state */

/*============================================================================

FUNCTION pdsm_lite_set_parameters

DESCRIPTION
  Command to set PDSM Parameters

DEPENDENCIES
  pdsm_lite_client_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/

extern boolean pdsm_set_parameters(
   pdsm_lite_pa_cmd_cb_f_type     cmd_cb_func,       /* Command callback */
   /*~ PARAM cmd_cb_func POINTER */
   void                      *client_data_ptr,  /* Not sure if this is required */
   pdsm_lite_pa_e_type            pa_set,            /* Parameter mask to be set */
   pdsm_lite_pa_info_type         *pa_ptr,           /* Pointer to Parameter values to bet set to */
   /*~ PARAM pa_ptr POINTER DISC pa_set*/

   pdsm_lite_client_id_type       client_id          /* Requesting client id */
   );
/*~ FUNCTION pdsm_lite_set_parameters */


/*===========================================================================

FUNCTION PDSM_LITE_GET_PARAMETERS

DESCRIPTION
  This function is used to get PDSM session parameters.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_get_parameters
(
   pdsm_lite_pa_cmd_cb_f_type    cmd_cb_func,        /* Command callback fn */
   /*~ PARAM cmd_cb_fund POINTER */
   void                     *client_data_ptr,   /* Client provided data pointer */
   pdsm_pa_e_type           pa_get,             /* Type of parameter to Get */
   pdsm_client_id_type      client_id
   );
/*~ FUNCTION pdsm_lite_get_parameters */

/*===========================================================================

FUNCTION pdsm_lite_get_pdapi_version

DESCRIPTION

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
uint32 pdsm_get_pdapi_version(void);
/*~ FUNCTION pdsm_lite_get_pdapi_version */


/*===========================================================================

FUNCTION pdsm_lite_xtra_set_data

DESCRIPTION
  gpsOneXTRA client calls this function to send parts of the XTRA file to GPS 
  engine.  
  
DEPENDENCIES

RETURN VALUE
  TRUE - Data part has been accepted
  FALSE - Data part has been rejected
  
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_xtra_set_data(
   pdsm_xtra_cmd_cb_f_type    xtra_cb_func,       /* Command callback */
   /*~ PARAM xtra_cb_func POINTER */
   pdsm_client_id_type        client_id,          /* Client id of user */
   void                       *client_data_ptr,   /* user data pointer */
   uint32                     xtra_data_len,      /* Length of XTRA data */
   unsigned char             *xtra_data_ptr,     /* XTRA data */
   uint8                      part_number,        /* data part number */
   uint8                      total_parts,        /* total data parts */

   pdsm_xtra_dc_status_e_type xtra_dc_status      /* status of download */
   );
/*~ FUNCTION pdsm_lite_xtra_set_data */


/*===========================================================================

FUNCTION pdsm_lite_xtra_client_initiate_download_request

DESCRIPTION
   gpsOneXTRA client calls this function when it needs to download an extended 
   assistance file but it does not have necessary information 
   (i.e. server URL, etc). Upon getting the request, TM should send 
   gpsOneXTRA PD API event which contains required information to the 
   gpsOneXTRA client.
   
DEPENDENCIES

RETURN VALUE
   TRUE - Initiate download request success
   FALSE - Initiate download request failed
   
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_lite_xtra_client_initiate_download_request(
   pdsm_xtra_cmd_cb_f_type    xtra_cb_func,      /* Command callback */
   /*~ PARAM xtra_cb_func POINTER */
   pdsm_client_id_type        client_id,         /* Client id of user */
   void                       *client_data_ptr   /* user data pointer */
   );
/*~ FUNCTION pdsm_lite_xtra_client_initiate_download_request */

/*===========================================================================

FUNCTION pdsm_lite_xtra_query_config_info

DESCRIPTION
   gpsOneXTRA client calls this function when it needs to get config info 
   (i.e. xtra server URL,ntp server url , current xtra file age etc). 
    Upon getting the request, TM should send gpsOneXTRA PD API event which 
    contains required information to the gpsOneXTRA client.
   
DEPENDENCIES

RETURN VALUE
   TRUE - Initiate download request success
   FALSE - Initiate download request failed
   
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_xtra_query_config_info
(
   pdsm_xtra_cmd_cb_f_type  xtra_cb_func,       /* Command callback */
   pdsm_client_id_type      client_id,          /* Client id of user */
   void                     *client_data_ptr,    /* user data pointer */
   pdsm_xtra_config_req_info_type *pXtraCfgReqInfo
   );
/*~ FUNCTION pdsm_lite_xtra_query_config_info */

/*===========================================================================

FUNCTION pdsm_lite_xtra_set_auto_download_params

DESCRIPTION
  gpsOneXTRA client calls this function to modify parameters related to the 
  automatic download functionality.  enable_auto_download flag is used 
  to enable or disable the functionality.  The download_interval parameter 
  specifies the number of hours between automatic downloads and its range 
  is 1-168 (168 = number of hours in 1 week).
  
DEPENDENCIES

RETURN VALUE
  TRUE - Auto Download Parameters are updated
  FALSE - Auto Download Parameters are NOT updated
  
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_xtra_set_auto_download_params(
   pdsm_xtra_cmd_cb_f_type    xtra_cb_func,         /* Command callback */
   /*~ PARAM xtra_cb_func POINTER */
   pdsm_client_id_type        client_id,            /* Client id of user */
   void                       *client_data_ptr,     /* user data pointer */
   boolean                    enable_auto_download, /* turn on/off auto download */
   uint16                     download_interval     /* time between downloads */
   );
/*~ FUNCTION pdsm_lite_xtra_set_auto_download_params */


/*===========================================================================

FUNCTION pdsm_lite_xtra_query_data_validity

DESCRIPTION
  gpsOneXTRA client calls this function to request validity information. 
  Upon getting the request, the starting GPS week and GPS minutes 
  along with the duration of the current extended assistance data 
  will be sent to the gpsOneXTRA client.  
  
  If there is no data in the subsystem, the values of the start time and 
  duration will be set to 0.
  
DEPENDENCIES

RETURN VALUE
  TRUE - request is a success
  FALSE - request failed
  
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_lite_xtra_query_data_validity(
   pdsm_xtra_cmd_cb_f_type    xtra_cb_func,       /* Command callback */
   /*~ PARAM xtra_cb_func POINTER */
   pdsm_client_id_type        client_id,          /* Client id of user */
   void                       *client_data_ptr    /* user data pointer */
   );
/*~ FUNCTION pdsm_lite_xtra_query_data_validity */


/*===========================================================================

FUNCTION pdsm_lite_xtra_inject_time_info

DESCRIPTION
   gpsOneXTRA client calls this function to inject XTRA Time
   
DEPENDENCIES

RETURN VALUE
    TRUE - Time injection success
    FALSE - Time injection failed
    
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_xtra_inject_time_info(
   pdsm_xtra_cmd_cb_f_type  xtra_cb_func,       /* Client command callback funct. */
   /*~ PARAM xtra_cb_func POINTER */
   pdsm_client_id_type      client_id,          /* Client id of user */
   void                     *client_data_ptr,   /* Pointer to client data block */
   pdsm_xtra_time_info_type *time_info_ptr      /* Pointer to time information data */
   /*~ PARAM time_info_ptr POINTER */
   );
/*~ FUNCTION pdsm_lite_xtra_inject_time_info */

/*===========================================================================

FUNCTION pdsm_lite_xtra_t_get_data

DESCRIPTION
   XTRA-T client calls this function to get CellDB data from GPS Engine.
   
DEPENDENCIES

RETURN VALUE
    TRUE  - XTRA-T get data command success
    FALSE - XTRA-T get data command failure

SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_xtra_t_get_data
(
   pdsm_xtra_t_cmd_cb_f_type   xtra_t_cb_func,      /* Command callback */
   void                       *client_data_ptr,     /* Not sure if this is required */
   /*~ PARAM client_data_ptr POINTER */
   pdsm_xtra_t_get_s_type      xtra_t_get_type,     /* Parameter mask to be set */
   pdsm_client_id_type         client_id            /* Requesting client id */
   );
/*~ FUNCTION pdsm_lite_xtra_t_get_data */

/*===========================================================================

FUNCTION pdsm_lite_enable_dpo_status_report

DESCRIPTION
    This function enables periodic DPO status reporting from MGP through SM layer.
    When enabled, the DPO status is reported at a nominal 1 Hz rate when receiver
    is active. This control is persistent across GNSS sessions, i.e. when the
    configuration is set, it stays till the phone is powered off (or changed
    by a new configuration).
 
DEPENDENCIES 
   * Client must have already been initialized with pdsm_lite_client_init().
   * Client must have been registered using API pdsm_lite_client_ext_status_reg() for the
     event PDSM_LITE_EXT_STATUS_EVENT_DPO_STATUS_REPORT. 
 
PARAMETERS
   enable : TRUE - Send the command to enable DPO status report.
            FALSE - Send the command to disable DPO status report.
 
RETURN VALUE
   TRUE - If the command to enable DPO status report is succesfull
   FALSE - If the command to enable DPO status report is not succesfull

SIDE EFFECTS 
  Only clients registered with event PDSM_LITE_EXT_STATUS_EVENT_DPO_STATUS_REPORT would receive this report .
  Clients not regsitered for this event would not get any report if even if they call this API with parameter TRUE
 
===========================================================================*/
boolean pdsm_enable_dpo_status_report(
   boolean enable
   );
/*~ FUNCTION pdsm_lite_pd_enable_dpo_report */

/*
******************************************************************************
* pdsm_lite_read_qmi_services_config
*
* Function description:
*
*  This synchronous API is called by QMI services to read QMI configuration at 
*  bootup. This function may be called only after PDAPI is initialized. The QMI 
*  configuration is used by the caller to register with PDAPI 
*
* Parameters:
*  qmi_Config : Configuration for QMI initialization
*
* Return value:
*  TRUE  - Request processed successfully
*  FALSE - Request could not be processed
*
* ******************************************************************************
*/
boolean pdsm_read_qmi_services_config(pdsm_QMIServiceConfig *qmi_Config);


/* Define different types of cell database */


/* Enumeration of the Cell Service Status */
typedef pdsm_lite_cell_service_status_e_type pdsm_cell_service_status_e_type;

/* GSM Cell Information  (activeRAT is CELLDB_ACTIVERAT_GSM)
   The fields should be all valid when activeRAT is GSM */
typedef pdsm_lite_gsm_cell_info_s_type pdsm_gsm_cell_info_s_type;

typedef pdsm_lite_active_RAT_e_type pdsm_active_RAT_e_type;

typedef pdsm_lite_cell_info_u_type pdsm_cell_info_u_type;

/* Cell Inforamtion */
typedef pdsm_lite_cell_info_s_type pdsm_cell_info_s_type;


/*===========================================================================

FUNCTION pdsm_lite_pd_inject_external_cell_info

DESCRIPTION
  PDSM client calls this function to inject external cell information to SM
  Typically this is the cell info updated from HLOS for external/3rd-party
  modem. This external cell info will be used in SUPL if it's valid, otherwise
  CM's cell info will be used (typically for internal modem)
 
  The supplied callback function pd_cb_func will be called to report this
  command's success/failure status. There will be no PDAPI event for this command.
 
DEPENDENCIES

RETURN VALUE
  TRUE - request is a success
  FALSE - request failed
  
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_pd_inject_external_cell_info(
   pdsm_pd_cmd_cb_f_type    pd_cb_func,         /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,    /* user data pointer */
   pdsm_client_id_type      client_id,           /* Client id of user */
   pdsm_cell_info_s_type    *cell_info           /* Pointer to external cell info */
   /*~ PARAM cell_info POINTER */
   );
/*~ FUNCTION pdsm_lite_pd_inject_external_cell_info */

/* Maximum log size permissible for GM in memory logging */
#define PDSM_LITE_GM_IN_MEM_MAX_LOG_SIZE_B 1024 * 1000 
/* Minimum periodicity for GM in mem logs */
#define PDSM_LITE_GM_IN_MEM_MIN_PERIOD_S 120


/*===========================================================================

FUNCTION pdsm_lite_get_gps_lock_status()

DESCRIPTION
  PDSM client calls this function to get current GPS lock status
 
  This is a synchronous API, will return immediately 
 
DEPENDENCIES

RETURN VALUE
  Current GPS lock status
  
SIDE EFFECTS

===========================================================================*/
extern pdsm_gps_lock_e_type pdsm_get_gps_lock_status(void);
/*===========================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* PDAPI_LITE_H */

