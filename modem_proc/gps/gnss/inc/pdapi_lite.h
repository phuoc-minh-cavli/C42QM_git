#ifndef PDAPI_LITE_H
#define PDAPI_LITE_H

/*===========================================================================

    Position Determination Session Manager Header File

DESCRIPTION
  This header file contains all the definitions necessary for
  clients to interface with the Position Determination module.

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2016 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2020 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/inc/pdapi_lite.h#2 $
  $DateTime: 2020/09/14 11:42:17 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- . 
08/11/20   sai     Added support for deletion of cell DB postion support from QMI
05/21/20   katta   Added new api to handle enhanced position request from the apps
10/01/20   ska     Changes done for MGP state to OFF when TBF>=5sec
08/21/19   ska     Added support for NMEA GSV
04/01/19   nath    Added debug NMEA related code.
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


#include "comdef.h"    /* Definition for basic types and macros */
#include "qw.h"
#include "locEng_sm_common_msg_v01.h"
#include "location_service_iot_v02.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define PDSM_LITE_API_MAJ_V 8
#define PDSM_LITE_API_MIN_V 6 /*<<--- QZSS:ATTENTION! Merge version number properly*/

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
#define PDSM_LITE_MINIMUM_TBF_MS (100) /* 100 milliseconds */


/* Client ID type to differentiate PDSM client objects from one another.
*/
typedef int  pdsm_lite_client_id_type;

typedef enum pdsm_lite_client_type_e {

  PDSM_LITE_CLIENT_TYPE_NONE = -1,        /* FOR INTERNAL USE OF PDSM ONLY! */

  PDSM_LITE_CLIENT_TYPE_TEST2,            /* Test client types, Added/Deleted in future */
  PDSM_LITE_CLIENT_TYPE_OEM,              /* Reserved for OEM Clients */
  PDSM_LITE_CLIENT_TYPE_XTRA,             /* Reserved for gpsOneXTRA Clients */
  PDSM_LITE_CLIENT_TYPE_XTRA_T,           /* Reserved for gpsOneXTRA-T Clients */
  PDSM_LITE_CLIENT_TYPE_PDA,              /* Reserver for LocMW */
  PDSM_LITE_CLIENT_TYPE_MAX,              /* FOR INTERNAL USE OF PDSM ONLY! */

  PDSM_LITE_CLIENT_TYPE_E_SIZE = 0x100000 /* To force enum to int */

} pdsm_lite_client_type_e_type;
#define PDSM_LITE_CLIENT_TYPE_TEST2_LEGACY_VAL  8 /*To aid diag testing previous 
                                                  client type value used by legacy script
                                                  will be translated to new value
                                                  */
/* Valid Service type mask */
/*
** Option types for Position Determination.
** Option type is not used as bitmask. Since user is registered he will anyway be
** informed about other PD events asynchronously even if he hasn't called the fnc.
*/

typedef enum pdsm_lite_pd_option_e {
  PDSM_LITE_PD_SESS_TYPE_MIN = -1,            /* Internal use of PDSM */
  PDSM_LITE_PD_SESS_TYPE_NEW = 1,                 /* Get new position */
  PDSM_LITE_PD_SESS_TYPE_TRACK_IND =2,
  PDSM_LITE_PD_SESS_TYPE_MAX,                 /* For internal use of PDSM */
  PDSM_LITE_PD_SESS_TYPE_E_SIZE = 0x10000000    /* Fix the enum size to int */
} pdsm_lite_pd_session_e_type;

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

typedef enum
{
  PDSM_LITE_POSITION_RELIABILITY_NOT_SET = 0, /* Not set, reliability info is not present */
  PDSM_LITE_POSITION_RELIABILITY_VERY_LOW,    /* Not checked, use at user's own risk */
  PDSM_LITE_POSITION_RELIABILITY_LOW,         /* Limited cross-checked */
  PDSM_LITE_POSITION_RELIABILITY_MEDIUM,      /* Medium level cross-checked */
  PDSM_LITE_POSITION_RELIABILITY_HIGH,        /* Robust cross-checked */
  PDSM_LITE_POSITION_RELIABILITY_MAX = 0x10000000    /* Forced to 32 bit */
} pdsm_lite_pd_reliability_e_type;

/* Altitude source of injected position
*/
typedef enum
{
  PDSM_LITE_ALTITUDE_SOURCE_UNKNOWN = 0,
  PDSM_LITE_ALTITUDE_SOURCE_GPS,
  PDSM_LITE_ALTITUDE_SOURCE_CID,
  PDSM_LITE_ALTITUDE_SOURCE_TERRESTRIAL,
  PDSM_LITE_ALTITUDE_SOURCE_TERRESTRIAL_HYBRID,
  PDSM_LITE_ALTITUDE_SOURCE_ALTITUDE_DATABASE,
  PDSM_LITE_ALTITUDE_SOURCE_BAROMETRIC_ALTIMETER,
  PDSM_LITE_ALTITUDE_SOURCE_OTHER,
  PDSM_LITE_ALTITUDE_SOURCE_MAX = 0x10000000    /* Forced to 32 bit */
} pdsm_lite_pd_altitude_source_e_type;

/* Horizontal position and altitude source linkage.
   This field specifies the dependence between the horizontal
   position and altitude components of injected position.
   This information provides the ability to the position
   engine to remove the bad components of position independently
   or dependently.
*/
typedef enum
{
  PDSM_LITE_ALT_HOR_LINK_NOT_SPECIFIED = 0,
  PDSM_LITE_ALT_HOR_LINK_FULLY_INTERDEPENDENT,
  PDSM_LITE_ALT_HOR_LINK_ALT_DEP_LAT_LONG,
  PDSM_LITE_ALT_HOR_LINK_FULLY_INDEPENDENT,
  PDSM_LITE_ALT_HOR_LINK_MAX = 0x10000000    /* Forced to 32 bit */
} pdsm_lite_pd_altitude_horizontal_linkage_e_type;

/* Altitude uncertainty coverage
   Point uncertainty means that the altitude 
   uncertainty is valid only at the injected horizontal
   position coordinates. Full uncertainty mean that the
   altitude uncertainty applies to the device regardless
   of the horizontal position (within the horizontal 
   uncertainty region if provided).
*/
typedef enum
{
  PDSM_LITE_ALT_COVERAGE_NOT_SPECIFIED = 0,
  PDSM_LITE_ALT_COVERAGE_POINT_UNC,
  PDSM_LITE_ALT_COVERAGE_FULL_UNC,
  PDSM_LITE_ALT_COVERAGE_MAX = 0x10000000    /* Forced to 32 bit */
} pdsm_lite_pd_altitude_unc_coverage_e_type;
/* define for continuous mode.
 * @brief Fix rate parameters
 *
 * Parameters include total number of fixes and TBF value in milliseconds.
 *
 * @sa PDSM_LITE_MINIMUM_TBF_MS
 */
typedef struct {
  uint32 num_fixes;             /**< @brief Total number of fixes to be done */
  uint32 time_between_fixes_ms; /**< @brief Time between fixes in milliseconds*/
} pdsm_lite_pd_fix_rate_s_type;
// TODO: <Nath>:see if enum required for a single enum

typedef enum {
  PDSM_LITE_SESSION_OPERATION_MIN = 0,
  PDSM_LITE_SESSION_OPERATION_STANDALONE_ONLY,  
  PDSM_LITE_SESSION_OPERATION_MAX = 0x10000000
} pdsm_lite_pd_session_operation_e_type;
/* Typedef structure used by PDSM */
typedef struct {
  pdsm_lite_pd_session_e_type         session;       /* PD option type */
  pdsm_lite_pd_session_operation_e_type  operation_mode; /* operation mode */
  pdsm_lite_pd_fix_rate_s_type        fix_rate;      /* Fix rate for tracking sessions. */
  uint32                         class_id;       /* ClassId in case of multiple apps */
} pdsm_lite_pd_option_s_type;
#define PDSM_LITE_NO_ACCURACY_THRESHOLD 0xFFFFFFF0   /* Threshold users can use so that PDSM ignores the check */
#define PDSM_LITE_MIN_ACCURACY_THRESHOLD 0x1         /* At least 1m should be specified */

/* QOS to be used for PD sessions.
*/
typedef struct pdsm_lite_pd_qos_struct {
  dword accuracy_threshold;    /* desired accruracy threshold in meters */
  byte  gps_session_timeout;   /* Desired time for positioning engine to work on a fix. (0-255)secs */
} pdsm_lite_pd_qos_type;

/* ------------------------------------------------------------------------
** End of type declarations for position determination session command.
** -------------------------------------------------------------------------
*/

/* Enumeration of commands PDSM.
*/
typedef enum pdsm_lite_pd_cmd_e
{
  PDSM_LITE_PD_CMD_NONE = -1,            /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_LITE_PD_CMD_GET_POS,             /* Get postion Command */
  PDSM_LITE_PD_CMD_END_SESSION,         /* End PD session */
  PDSM_LITE_PD_CMD_INJECT_TIME,         /* Inject External time */
  PDSM_LITE_PD_CMD_INJECT_POS,          /* Inject external position */
  PDSM_LITE_PD_CMD_GET_GPS_STATE,       /* Get the GPS State information */
  PDSM_LITE_PD_CMD_GET_BEST_AVAIL_POS,     /* Get the best available position command */
  PDSM_LITE_PD_CMD_END_SESSION_EX,       /* Extended End PD session */
  PDSM_LITE_PD_CMD_INJECT_CELL_INFO,
  PDSM_LITE_PD_CMD_GET_AVAIL_WWAN_POS,
  PDSM_LITE_PD_CMD_MAX,                 /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_LITE_CMD_E_SIZE = 0x10000000       /* To force enum to 32 bit for windows NT */
} pdsm_lite_pd_cmd_e_type;
/*~ SENTINEL pdsm_lite_pd_cmd_e_type.PDSM_LITE_PD_CMD_MAX */

/* Type declarations for Set/Get parameters */

/* Enum defined for allowing different levels of GPS locks.
*/
/*Remove if not used by tm_xtra*/
enum
{
  PDSM_LITE_FS_CREATE_WRITE_FILE,
  PDSM_LITE_FS_APPEND_FILE,
  PDSM_LITE_FS_DELETE_FILE,
  PDSM_LITE_FS_READ_FILE,
  PDSM_LITE_FS_PURGE_DIR, /* Delete all files from a directory */
  PDSM_LITE_FS_DELETE_FILE_LIST /* Delete specified files from a directory */
};

#define PDSM_LITE_PD_FS_MAX_PATH_LEN_BYTES  64   /* Max file name length in bytes that can be written*/
#define PDSM_LITE_PD_EFS_MAX_FILE_LEN_BYTES 2000 /* Max file size in bytes that can be written*/


/* typedef for parameter set mask */
typedef enum pdsm_lite_pa_e {
  PDSM_LITE_PA_NONE = 0,           /* Internal range */
  PDSM_LITE_PA_GPS_LOCK,           /* GPS LOCK */
  PDSM_LITE_PA_EFS_DATA,
  PDSM_LITE_PA_DPO_CONTROL,        /* DPO Control */
  PDSM_LITE_PA_XTRAT_CLIENT_TOKEN, /* Set XTRA-T Client token */
  PDSM_LITE_PA_XTRAT_USER_SESSION_CONTROL, /* Used to set the XTRA-T User session control */
  PDSM_LITE_PA_PE_CONFIG_PARAM,           /* PE config parameter */
  PDSM_LITE_PA_DELETE_PARAMS_EX1,         /* delete GPS / GNSS params from database */
  PDSM_LITE_PA_UAX3_CONFIG,
  PDSM_LITE_PA_XTRA_APN_PROFILE,
  PDSM_LITE_PA_XTRA_VERSION_CHECK,        /* XTRA Version Check */
  PDSM_LITE_PA_GTP_WWAN_FIX_TIMER_CONFIG,  /*Configer duration of timer used while waiting for GTP WWAN FIX*/
  PDSM_LITE_PA_GTP_WWAN_MIN_INTERVAL_BTWN_REQ,/*Configer duration of time between two calls to GTP WWAN server*/
  PDSM_LITE_PA_BYPASS_SYS_UNC_CHECK,
  PDSM_LITE_PA_NMEA_SENTENCE_TYPE,
  PDSM_LITE_PA_MAX,                       /* Internal use. */
  PDSM_LITE_PA_SIZE = 0x10000000          /* Fix the size as int */
} pdsm_lite_pa_e_type;
/*~ SENTINEL pdsm_lite_pa_e_type.PDSM_LITE_PA_MAX */

typedef enum {
  PDSM_LITE_GPS_LOCK_MIN = -1,
  PDSM_LITE_GPS_LOCK_NONE,
  PDSM_LITE_GPS_LOCK_MI,
  PDSM_LITE_GPS_LOCK_MT,
  PDSM_LITE_GPS_LOCK_ALL,
  PDSM_LITE_GPS_LOCK_MAX = 0x10000000
} pdsm_lite_gps_lock_e_type;






/* sets the specified bit in a u32 bit mask. bit counts range: 0-31 */
#define PDSM_LITE_SET_U32BITMASK_BIT(n) (((uint32)0x1) << (n))

/* sets the specified bit in a u64 bit mask. bit counts range: 0-63 */
#define PDSM_LITE_SET_U64BITMASK_BIT(n) (((uint64)0x1) << (n))

#define PDSM_LITE_PA_DELETE_BITMASK(n)  PDSM_LITE_SET_U64BITMASK_BIT(n)

/* the following flags, where no GNSS system is specified, 
   affect only GPS, unless mentioned otherwise. E.g.
   PDSM_LITE_PA_DELETE_EPH would only delete GPS ephemeris. 

   See also: C_DELETE_INFO_xxx flags used by MGP API
*/
#define PDSM_LITE_PA_DELETE_EPH    PDSM_LITE_PA_DELETE_BITMASK(0) /*0x0001*/
#define PDSM_LITE_PA_DELETE_ALM    PDSM_LITE_PA_DELETE_BITMASK(1) /*0x0002*/
#define PDSM_LITE_PA_DELETE_POS    PDSM_LITE_PA_DELETE_BITMASK(2) /*0x0004*/
#define PDSM_LITE_PA_DELETE_TIME   PDSM_LITE_PA_DELETE_BITMASK(3) /*0x0008*/
/* PDSM_LITE_PA_DELETE_TIME  will delete all GNSS time For finer control 
   of individual clock elements, use PDSM_LITE_PA_DELETE_CLOCK_INFO instead  */
#define PDSM_LITE_PA_DELETE_IONO   PDSM_LITE_PA_DELETE_BITMASK(4) /*0x0010*/
#define PDSM_LITE_PA_DELETE_UTC    PDSM_LITE_PA_DELETE_BITMASK(5) /*0x0020*/
#define PDSM_LITE_PA_DELETE_HEALTH PDSM_LITE_PA_DELETE_BITMASK(6) /*0x0040*/
#define PDSM_LITE_PA_DELETE_SVDIR  PDSM_LITE_PA_DELETE_BITMASK(7) /*0x0080*/
#define PDSM_LITE_PA_DELETE_SVSTEER PDSM_LITE_PA_DELETE_BITMASK(8) /*0x0100*/
#define PDSM_LITE_PA_DELETE_SADATA PDSM_LITE_PA_DELETE_BITMASK(9) /*0x0200*/
#define PDSM_LITE_PA_DELETE_RTI    PDSM_LITE_PA_DELETE_BITMASK(10) /*0x0400*/

#define PDSM_LITE_PA_TIME_UNC      PDSM_LITE_PA_DELETE_BITMASK(11) /*0x0800*/ /* this will modify all GNSS time unc */
#define PDSM_LITE_PA_POS_UNC       PDSM_LITE_PA_DELETE_BITMASK(12) /*0x1000*/
#define PDSM_LITE_PA_TIME_OFFSET   PDSM_LITE_PA_DELETE_BITMASK(13) /*0x2000*/
#define PDSM_LITE_PA_POS_OFFSET    PDSM_LITE_PA_DELETE_BITMASK(14) /*0x4000*/

#define PDSM_LITE_PA_DELETE_CELLDB_INFO    PDSM_LITE_PA_DELETE_BITMASK(15) /*0x8000*/

#define PDSM_LITE_PA_DELETE_ALM_CORR    PDSM_LITE_PA_DELETE_BITMASK(16) /*0x10000*/
#define PDSM_LITE_PA_DELETE_FREQ_BIAS_EST PDSM_LITE_PA_DELETE_BITMASK(17) /*0x20000*/
#define PDSM_LITE_PA_DELETE_SV_NO_EXIST  PDSM_LITE_PA_DELETE_BITMASK(18) /*0x40000*/
/* additional constellation flags */
#define PDSM_LITE_PA_DELETE_EPH_GLO   PDSM_LITE_PA_DELETE_BITMASK(19)
#define PDSM_LITE_PA_DELETE_EPH_SBAS   PDSM_LITE_PA_DELETE_BITMASK(20)


#define PDSM_LITE_PA_DELETE_ALM_GLO   PDSM_LITE_PA_DELETE_BITMASK(21)
#define PDSM_LITE_PA_DELETE_ALM_SBAS  PDSM_LITE_PA_DELETE_BITMASK(22)

#define PDSM_LITE_PA_DELETE_SVDIR_GLO    PDSM_LITE_PA_DELETE_BITMASK(23)
#define PDSM_LITE_PA_DELETE_SVDIR_SBAS   PDSM_LITE_PA_DELETE_BITMASK(24)


#define PDSM_LITE_PA_DELETE_SVSTEER_GLO     PDSM_LITE_PA_DELETE_BITMASK(25)
#define PDSM_LITE_PA_DELETE_SVDIR_SBAS      PDSM_LITE_PA_DELETE_BITMASK(24)

#define PDSM_LITE_PA_DELETE_ALM_CORR_GLO    PDSM_LITE_PA_DELETE_BITMASK(27)

#define PDSM_LITE_PA_DELETE_TIME_GPS   PDSM_LITE_PA_DELETE_BITMASK(28)
#define PDSM_LITE_PA_DELETE_TIME_GLO   PDSM_LITE_PA_DELETE_BITMASK(29)

#define PDSM_LITE_PA_DELETE_CLOCK_INFO PDSM_LITE_PA_DELETE_BITMASK(30) /* indicates delete_clock_info_mask
                                                                 is valid. see PDSM_LITE_PA_DELETE_CLOCK_INFOxxx*/

#define PDSM_LITE_PA_DELETE_TIME_BDS      PDSM_LITE_PA_DELETE_BITMASK(31)
#define PDSM_LITE_PA_DELETE_EPH_BDS       PDSM_LITE_PA_DELETE_BITMASK(32)
#define PDSM_LITE_PA_DELETE_ALM_BDS       PDSM_LITE_PA_DELETE_BITMASK(33)
#define PDSM_LITE_PA_DELETE_SVDIR_BDS     PDSM_LITE_PA_DELETE_BITMASK(34)
#define PDSM_LITE_PA_DELETE_SVSTEER_BDS   PDSM_LITE_PA_DELETE_BITMASK(35)
#define PDSM_LITE_PA_DELETE_ALM_CORR_BDS  PDSM_LITE_PA_DELETE_BITMASK(36)

#define PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_GPS  PDSM_LITE_PA_DELETE_BITMASK(37)
#define PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_GLO  PDSM_LITE_PA_DELETE_BITMASK(38)
#define PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_BDS  PDSM_LITE_PA_DELETE_BITMASK(39)
#define PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_GAL  PDSM_LITE_PA_DELETE_BITMASK(40)
#define PDSM_LITE_PA_DELETE_TIME_GAL               PDSM_LITE_PA_DELETE_BITMASK(41)
#define PDSM_LITE_PA_DELETE_EPH_GAL                PDSM_LITE_PA_DELETE_BITMASK(42)
#define PDSM_LITE_PA_DELETE_ALM_GAL                PDSM_LITE_PA_DELETE_BITMASK(43)
#define PDSM_LITE_PA_DELETE_SVDIR_GAL              PDSM_LITE_PA_DELETE_BITMASK(44)
#define PDSM_LITE_PA_DELETE_SVSTEER_GAL            PDSM_LITE_PA_DELETE_BITMASK(45)
#define PDSM_LITE_PA_DELETE_ALM_CORR_GAL           PDSM_LITE_PA_DELETE_BITMASK(46)
#define PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_QZSS PDSM_LITE_PA_DELETE_BITMASK(47)

#define PDSM_LITE_PA_DELETE_IONO_QZSS              PDSM_LITE_PA_DELETE_BITMASK(48)
#define PDSM_LITE_PA_DELETE_EPH_QZSS               PDSM_LITE_PA_DELETE_BITMASK(49)
#define PDSM_LITE_PA_DELETE_ALM_QZSS               PDSM_LITE_PA_DELETE_BITMASK(50)
#define PDSM_LITE_PA_DELETE_SVDIR_QZSS             PDSM_LITE_PA_DELETE_BITMASK(51)
#define PDSM_LITE_PA_DELETE_SVSTEER_QZSS           PDSM_LITE_PA_DELETE_BITMASK(52)
#define PDSM_LITE_PA_DELETE_ALM_CORR_QZSS          PDSM_LITE_PA_DELETE_BITMASK(53)
#define PDSM_LITE_PA_DELETE_HEALTH_QZSS            PDSM_LITE_PA_DELETE_BITMASK(54)

#define PDSM_LITE_PA_DELETE_IONO_BDS               PDSM_LITE_PA_DELETE_BITMASK(55)
#define PDSM_LITE_PA_DELETE_IONO_GAL               PDSM_LITE_PA_DELETE_BITMASK(56)

/* Deletes Glonass SV health info for all GLONASS satellites*/
#define PDSM_LITE_PA_DELETE_HEALTH_GLO    PDSM_LITE_PA_DELETE_BITMASK(57)
/* Deletes BDS SV health info for all BDS satellites*/
#define PDSM_LITE_PA_DELETE_HEALTH_BDS    PDSM_LITE_PA_DELETE_BITMASK(58)
/* Deletes GAL SV health info for all GAL satellites*/
#define PDSM_LITE_PA_DELETE_HEALTH_GAL    PDSM_LITE_PA_DELETE_BITMASK(59)
/*Deletes group delay*/
#define PDSM_LITE_PA_DELETE_TGD           PDSM_LITE_PA_DELETE_BITMASK(60)
#define PDSM_LITE_PA_DELETE_IONO_GLO      PDSM_LITE_PA_DELETE_BITMASK(61)

#define PDSM_LITE_PA_DELETE_ALL (\
   PDSM_LITE_PA_DELETE_EPH | \
   PDSM_LITE_PA_DELETE_ALM | \
   PDSM_LITE_PA_DELETE_POS | \
   PDSM_LITE_PA_DELETE_TIME | \
   PDSM_LITE_PA_DELETE_IONO | \
   PDSM_LITE_PA_DELETE_UTC | \
   PDSM_LITE_PA_DELETE_HEALTH | \
   PDSM_LITE_PA_DELETE_SVDIR | \
   PDSM_LITE_PA_DELETE_SVSTEER | \
   PDSM_LITE_PA_DELETE_SADATA | \
   PDSM_LITE_PA_DELETE_RTI | \
   PDSM_LITE_PA_DELETE_CELLDB_INFO | \
   PDSM_LITE_PA_DELETE_ALM_CORR |\
   PDSM_LITE_PA_DELETE_SV_NO_EXIST |\
   PDSM_LITE_PA_DELETE_FREQ_BIAS_EST|\
   PDSM_LITE_PA_DELETE_EPH_GLO |\
   PDSM_LITE_PA_DELETE_ALM_GLO |\
   PDSM_LITE_PA_DELETE_SVDIR_GLO |\
   PDSM_LITE_PA_DELETE_SVSTEER_GLO |\
   PDSM_LITE_PA_DELETE_ALM_CORR_GLO |\
   PDSM_LITE_PA_DELETE_TIME_GPS |\
   PDSM_LITE_PA_DELETE_TIME_GLO |\
   PDSM_LITE_PA_DELETE_TIME_BDS |\
   PDSM_LITE_PA_DELETE_EPH_BDS |\
   PDSM_LITE_PA_DELETE_ALM_BDS |\
   PDSM_LITE_PA_DELETE_SVDIR_BDS |\
   PDSM_LITE_PA_DELETE_SVSTEER_BDS |\
   PDSM_LITE_PA_DELETE_ALM_CORR_BDS |\
   PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_GPS |\
   PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_GLO |\
   PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_BDS |\
   PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_QZSS |\
   PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_GAL |\
   PDSM_LITE_PA_DELETE_IONO_QZSS |\
   PDSM_LITE_PA_DELETE_EPH_QZSS |\
   PDSM_LITE_PA_DELETE_ALM_QZSS |\
   PDSM_LITE_PA_DELETE_SVDIR_QZSS |\
   PDSM_LITE_PA_DELETE_SVSTEER_QZSS |\
   PDSM_LITE_PA_DELETE_ALM_CORR_QZSS|\
   PDSM_LITE_PA_DELETE_TIME_GAL |\
   PDSM_LITE_PA_DELETE_EPH_GAL |\
   PDSM_LITE_PA_DELETE_ALM_GAL |\
   PDSM_LITE_PA_DELETE_SVDIR_GAL |\
   PDSM_LITE_PA_DELETE_SVSTEER_GAL |\
   PDSM_LITE_PA_DELETE_ALM_CORR_GAL |\
   PDSM_LITE_PA_DELETE_HEALTH_GLO |\
   PDSM_LITE_PA_DELETE_HEALTH_QZSS |\
   PDSM_LITE_PA_DELETE_HEALTH_BDS |\
   PDSM_LITE_PA_DELETE_HEALTH_GAL |\
   PDSM_LITE_PA_DELETE_IONO_BDS |\
   PDSM_LITE_PA_DELETE_IONO_GAL |\
   PDSM_LITE_PA_DELETE_IONO_GLO |\
   PDSM_LITE_PA_DELETE_TGD  \
   )

#define PDSM_LITE_PA_DELETE_ALL_U32BITMASK              ((uint32)0xFFFFFFFF)

/* Cell DB info delete bit mask */
#define PDSM_LITE_PA_DELETE_CELLDB_MASK_POS_INFO            PDSM_LITE_SET_U32BITMASK_BIT(0)
#define PDSM_LITE_PA_DELETE_CELLDB_MASK_LATEST_GPS_POS_INFO PDSM_LITE_SET_U32BITMASK_BIT(1)
#define PDSM_LITE_PA_DELETE_CELLDB_MASK_OTA_POS_INFO        PDSM_LITE_SET_U32BITMASK_BIT(2)
#define PDSM_LITE_PA_DELETE_CELLDB_MASK_EXT_REF_POS_INFO    PDSM_LITE_SET_U32BITMASK_BIT(3)
#define PDSM_LITE_PA_DELETE_CELLDB_MASK_TIMETAG_INFO        PDSM_LITE_SET_U32BITMASK_BIT(4)
#define PDSM_LITE_PA_DELETE_CELLDB_MASK_CELLID_INFO         PDSM_LITE_SET_U32BITMASK_BIT(5)
#define PDSM_LITE_PA_DELETE_CELLDB_MASK_CACHED_CELLID_INFO  PDSM_LITE_SET_U32BITMASK_BIT(6)
#define PDSM_LITE_PA_DELETE_CELLDB_MASK_LAST_SRV_CELL_INFO  PDSM_LITE_SET_U32BITMASK_BIT(7)
#define PDSM_LITE_PA_DELETE_CELLDB_MASK_CUR_SRV_CELL_INFO   PDSM_LITE_SET_U32BITMASK_BIT(8)
#define PDSM_LITE_PA_DELETE_CELLDB_MASK_NEIGHBOR_INFO_INFO  PDSM_LITE_SET_U32BITMASK_BIT(9)
	 
#define PDSM_LITE_PA_DELETE_ALL_CELLDB                      PDSM_LITE_PA_DELETE_ALL_U32BITMASK

#define PDSM_LITE_PA_DELETE_CLOCK_INFO_TIME_EST                 PDSM_LITE_SET_U32BITMASK_BIT(0) /* deletes all GNSS time */
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_FREQ_EST                 PDSM_LITE_SET_U32BITMASK_BIT(1)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_WEEK_NUMBER              PDSM_LITE_SET_U32BITMASK_BIT(2)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_RTC_TIME                 PDSM_LITE_SET_U32BITMASK_BIT(3)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_TIME_TRANSFER            PDSM_LITE_SET_U32BITMASK_BIT(4)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_GPSTIME_EST              PDSM_LITE_SET_U32BITMASK_BIT(5)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_GLOTIME_EST              PDSM_LITE_SET_U32BITMASK_BIT(6)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_GLODAY_NUMBER            PDSM_LITE_SET_U32BITMASK_BIT(7)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_GLO4YEAR_NUMBER          PDSM_LITE_SET_U32BITMASK_BIT(8)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY         PDSM_LITE_SET_U32BITMASK_BIT(9)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_DISABLE_TT               PDSM_LITE_SET_U32BITMASK_BIT(10)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_GG_LEAPSEC               PDSM_LITE_SET_U32BITMASK_BIT(11)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_GG_GGTB                  PDSM_LITE_SET_U32BITMASK_BIT(12)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_BDSTIME_EST              PDSM_LITE_SET_U32BITMASK_BIT(13)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_GB_GBTB                  PDSM_LITE_SET_U32BITMASK_BIT(14)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_BG_BGTB                  PDSM_LITE_SET_U32BITMASK_BIT(15) /* Beidou to Glonass time bias-related */
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_BDSWEEK_NUMBER           PDSM_LITE_SET_U32BITMASK_BIT(16)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_BDS_RF_GRP_DELAY         PDSM_LITE_SET_U32BITMASK_BIT(17)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_GPS_GAL_TB               PDSM_LITE_SET_U32BITMASK_BIT(18)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_GLO_GAL_TB               PDSM_LITE_SET_U32BITMASK_BIT(19)
#define PDSM_LITE_PA_DELETE_CLOCK_INFO_GAL_BDS_TB               PDSM_LITE_SET_U32BITMASK_BIT(20)

#define PDSM_LITE_PA_DELETE_ALL_CLOCK_INFO (\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_TIME_EST |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_FREQ_EST |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_WEEK_NUMBER |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_RTC_TIME |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_TIME_TRANSFER |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_GPSTIME_EST |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_GLOTIME_EST |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_GLODAY_NUMBER |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_GLO4YEAR_NUMBER |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_DISABLE_TT |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_GG_LEAPSEC |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_GG_GGTB |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_BDSTIME_EST |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_GB_GBTB |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_BG_BGTB |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_BDSWEEK_NUMBER |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_BDS_RF_GRP_DELAY |\
     PDSM_LITE_PA_DELETE_CLOCK_INFO_GPS_GAL_TB | \
     PDSM_LITE_PA_DELETE_CLOCK_INFO_GLO_GAL_TB | \
     PDSM_LITE_PA_DELETE_CLOCK_INFO_GAL_BDS_TB  \
     )

typedef struct {
  uint64 pdsm_lite_delete_parms_flags;  /* 64 bits now with BDS support */
  uint32 time_unc; /* time unc in msec */
  uint32 pos_unc;  /* pos unc in meters */
  uint32 time_offset; /* time offset in msec*/
  uint32 pos_offset; /* pos offset in meters. currently not supported */
  uint32 eph_sv_mask; /* delete targeted GPS EPH*/
  uint32 alm_sv_mask; /* delete targeted GPS ALM*/
  uint32 glo_eph_sv_mask; /* delete targeted GLO EPH */
  uint32 glo_alm_sv_mask; /* delete targeted GLO ALM */
  uint64 bds_eph_sv_mask; /* delete targeted BDS EPH */
  uint64 bds_alm_sv_mask; /* delete targeted BDS ALM */
  uint64 gal_eph_sv_mask; /* delete targeted GAL EPH */
  uint64 gal_alm_sv_mask; /* delete targeted GAL ALM */
  uint32 qzss_eph_sv_mask; /* delete targeted QZSS EPH */
  uint32 qzss_alm_sv_mask; /* delete targeted QZSS ALM*/
  uint32 gps_gnss_sv_blacklist_mask;  /* delete targeted GPS SV in GNSS SV blacklist */
  uint32 glo_gnss_sv_blacklist_mask;  /* delete targeted GLO SV in GNSS SV blacklist */
  uint64 bds_gnss_sv_blacklist_mask;  /* delete targeted BDS SV in GNSS SV blacklist */
  uint64 gal_gnss_sv_blacklist_mask;  /* delete targeted GAL SV in GNSS SV blacklist */
  uint32 qzss_gnss_sv_blacklist_mask; /* delete targeted QZSS SV in GNSS SV blacklist */
  uint32 delete_celldb_mask; /* delete cell db */
  uint32 delete_clock_info_mask; /* must set PDSM_LITE_PA_DELETE_CLOCK_INFO.
                                 see PDSM_LITE_PA_DELETE_CLOCK_INFOxxx for 
                                  permitted flags */
  float f_time_unc; /* time unc in msec, can accept sub-ms values in the decimal point*/
  float f_time_offset; /* time offset in msec can accept submeter value in the decimal point */
  float f_pos_unc;  /* pos unc in meters. can accept submeter values in the decimal point */
} pdsm_lite_delete_parms_type;

#define PDAPI_LITE_NMEA_DEBUG 0x00040000   /* NMEA debugging enable*/

/*******************************************************************************
* UE supported position method types (a.k.a positioning modes)                 *
*******************************************************************************/


/* GPS position method types */
/* UP MSBased/MSAssisted/Autonomous bits */
#define PDSM_LITE_POS_MODE_STANDALONE        0x0001  /* GNSS Standalone */

#define PDSM_LITE_PA_PE_CONFIG_PARAM_MASK_USE_INJECTED_POSITION  0x00000001
#define PDSM_LITE_PA_PE_CONFIG_PARAM_MASK_FILTER_SV_USED         0x00000002
#define PDSM_LITE_PA_PE_CONFIG_PARAM_MASK_REPORT_ACCURATE_SVDIR    0x00000008

typedef struct
{
  /* bitmask from above definition, can config one or multiple parameters
     This field is used by PDAPI client to pass in bitmasks for changing some/all
     PE config parameters. 
     For event PDSM_LITE_PA_EVENT_PE_CONFIG_PARAM sent back to PDAPI client, this field
     is used to indicate success(if bit is 1) or failure(if bit is 0) for requested
     PE config parameter SET/GET. For event triggered by SET, the 3 PE config parameters
     below will contain original values passed in by PDAPI client regardless of success
     or failure. For event triggered by GET, the returned PE config parameter value is 
     meaningful only if the corresponding bit in bitmask is set to 1 (i.e. success) */
  uint32  peConfigMask;

  /* 0x00 (FALSE) -- Do not use the injected position in direct position calculation. 
     0x01 (TRUE) --  Use the injected position in direct position calculation. */
  boolean useInjectedPosition;

  /* 0x00 (FALSE) -- Do not filter the usage of SV's in the fix         
     0x01 (TRUE) -- Filter the usage of SV's in the fix. */
  boolean filterSvUsed;

  /* 0x00 (FALSE) --  Do not report accurate SV direction.  
     0x01 (TRUE) --  Report Accurate SV direction */
  boolean rptAccurateSvDir;

} pdsm_lite_pa_pe_config_s_type;

typedef struct pdsm_lite_efs_data_s {
  uint8          efs_data_filename_len;   /* This field is deprecated */
  char          *efs_data_filename;       /* NULL terminated File name information. The length will be 
                                             determined by the length of this string */
  /*~ FIELD pdsm_lite_efs_data_s.efs_data_filename STRING */
  uint8          efs_data_file_operation; /* File Operation. PDSM_LITE_FS_READ_FILE not supported */
  uint32         efs_data_reserved;       /* Reserved for future use */
  uint32         efs_data_len;            /* Length (in bytes) of data content, max length defined by  PDSM_LITE_PD_EFS_MAX_FILE_LENGTH_BYTES*/
  unsigned char *efs_data_ptr;            /* Data Content */
  /*~ FIELD pdsm_lite_efs_data_s.efs_data_ptr VARRAY LENGTH pdsm_lite_efs_data_s.efs_data_len */
  uint8          efs_data_part_number;    /* data part number */
  uint8          efs_data_total_parts;    /* total data parts */
} pdsm_lite_efs_data_s_type;

/** UAX3 Operations */
#define PDSM_LITE_PA_UAX3_MASK_ENABLE  0x01
#define PDSM_LITE_PA_UAX3_MASK_ALL     PDSM_LITE_PA_UAX3_MASK_ENABLE

/* configuration for XTRA Version Check */
typedef enum
{
  PDSM_LITE_PA_XTRA_VERSION_CHECK_DISABLED, /* XTRA Version Check Disabled (not required) */
  PDSM_LITE_PA_XTRA_VERSION_CHECK_AUTO,     /* XTRA Version Check Automatic (based on pre-provisioned XTRA version config) */
  PDSM_LITE_PA_XTRA_VERSION_CHECK_XTRA2,    /* XTRA Version Check with XTRA2 format */
  PDSM_LITE_PA_XTRA_VERSION_CHECK_XTRA3,    /* XTRA Version Check with XTRA3 format */
  PDSM_LITE_PA_XTRA_VERSION_CHECK_MAX,      /* validity checking */
  PDSM_LITE_PA_XTRA_VERSION_CHECK_MAX_RESERVED = 0x10000000, /* force to 4 bytes */
} pdsm_lite_pa_xtra_ver_check_e_type;

/* This is a pdsm parameter info, IP address, PTLM mode and GPS lock status.
** This strucure is passed as it is to different clients whenever
** they are updates. (const takes care of clients not being able to
** modify the data structure.
*/
typedef union pdsm_lite_pa_info
{
  pdsm_lite_gps_lock_e_type              gps_lock;
  pdsm_lite_delete_parms_type            delete_params;
  uint8                             dpo_control;                    /* DPO Control. 0 is to Disable(change NV), 1 to Enable(change NV)
                                                                       0xF0 is to Disable(no NV change), 0xF1 is to Enable(no NV change) 
                                                                      (If NV is changed, new setting is applicable across device power cycles) */
  uint8                             call_flow_select_mask;           /* Call Flow Select Mask */	 	             	 	 
  uint32                            client_cookie;                   /* XTRA-T client cookie for data transfer with XTRA-T server */	 	 
  uint8                             session_control;                 /* User Control: (0 - disable)/(1 - Enable) XTRA-T session */	 	 
  boolean                           data_enable;                     /* enable/disable data */																	   
  pdsm_lite_pa_pe_config_s_type          pe_config_info;                 /* PE configuration parameter */
  pdsm_lite_pa_xtra_ver_check_e_type     xtra_version_check;             /* XTRA file version check type */
  uint64                            pcid_info;                      /* PCID for XTRA */
  uint32                            uax3_config_info;               /* UAX3 Config Info. 0=UAX3 is enabled, 1=UAX3 disabled*/
  uint32                            gtp_wwan_fix_timer_duration;
  uint16                            gtp_wwan_min_interval_btwn_req;
  uint8                             bypass_sys_unc_check;
  uint32                            nmea_sentence_type;
} pdsm_lite_pa_info_type;
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
typedef enum pdsm_lite_pa_cmd_e {
  PDSM_LITE_PA_CMD_NONE = -1,           /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_LITE_PA_CMD_SET_PARAM,         /* Set Parameter Command */
  PDSM_LITE_PA_CMD_GET_PARAM,         /* Get Parameter command */
  PDSM_LITE_PA_CMD_MAX,               /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_LITE_PA_CMD_E_SIZE = 0x10000000  /* To force enum to 32 bit for windows NT */
} pdsm_lite_pa_cmd_e_type;
/*~ SENTINEL pdsm_lite_pa_cmd_e_type.PDSM_LITE_PA_CMD_MAX */
/*- - - - - - - - - - - - - - EVENTS - - - - - - - - - - - - - - - - - - - - - -*/

#define PDSM_LITE_NMEA_DATA_LENGTH 200

/* Postion Determination Events.
** Clients are able to register with PDSM to be notified of any subsets
** of these events.
*/
typedef uint32 pdsm_lite_pd_event_type;
/* General PDSM Events */
#define PDSM_LITE_PD_EVENT_POSITION 0x01
#define PDSM_LITE_PD_EVENT_VELOCITY 0x02
#define PDSM_LITE_PD_EVENT_HEIGHT   0x04
#define PDSM_LITE_PD_EVENT_DONE     0x08
#define PDSM_LITE_PD_EVENT_END      0x10    /* Indicating End of session. */
#define PDSM_LITE_PD_EVENT_BEGIN    0x20    /* Indicate the start of session. */
#define PDSM_LITE_PD_EVENT_BEST_AVAIL_POS_REPORT  0x40
#define PDSM_LITE_PD_STATUS_GPS_STATE_INFO        0x80        /* GPS State information */
#define PDSM_LITE_PD_STATUS_DPO_STATUS            0x100       /* DPO engage status */
#define PDSM_LITE_PD_STATUS_AVAIL_POS_REPORT      0x200           /* Available  position report */
#define PDSM_LITE_PD_STATUS_ENGINE_MONITOR_REPORT 0x400           /* GNSS Engine Monitor report */
#define PDSM_LITE_PD_STATUS_EVENT_AVAIL_WWAN_POS_REPORT 0x800
#define PDSM_LITE_PD_STATUS_NMEA_POS_REPORT       0x1000
#define PDSM_LITE_PD_STATUS_SV_INFO_REPORT        0x2000

#define PDSM_LITE_PD_GENERAL_EVENT_MASK ( (PDSM_LITE_PD_EVENT_POSITION) | (PDSM_LITE_PD_EVENT_VELOCITY)\
                            | (PDSM_LITE_PD_EVENT_HEIGHT) | (PDSM_LITE_PD_EVENT_DONE)\
                            | (PDSM_LITE_PD_EVENT_END) | (PDSM_LITE_PD_EVENT_BEGIN)\
                            | (PDSM_LITE_PD_EVENT_UPDATE_FAILURE)\
                            | (PDSM_LITE_PD_EVENT_BEST_AVAIL_POS_REPORT)\
                            | (PDSM_LITE_PD_STATUS_ENGINE_MONITOR_REPORT)\
                            | (PDSM_LITE_PD_STATUS_EVENT_AVAIL_WWAN_POS_REPORT))
                            

/* Positioning Processing related events */
#define PDSM_LITE_PD_EVENT_GPS_BEGIN        0x00004000
#define PDSM_LITE_PD_EVENT_GPS_DONE         0x00008000

#define PDSM_LITE_PD_GPS_EVENT_MASK ( (PDSM_LITE_PD_EVENT_GPS_BEGIN) | (PDSM_LITE_PD_EVENT_GPS_DONE) )

/* Coase Position Injection related */
#define PDSM_LITE_PD_EVENT_COARSE_POS_INJ_DONE     0x00200000
#define PDSM_LITE_PD_EVENT_COARSE_POS_INJ_FAILED   0x00400000
#define PDSM_LITE_PD_EVENT_COARSE_POS_INJ_ERROR    0x00800000
#define PDSM_LITE_PD_EVENT_COARSE_POS_INJ_MASK ( (PDSM_LITE_PD_EVENT_COARSE_POS_INJ_DONE) \
                                            | (PDSM_LITE_PD_EVENT_COARSE_POS_INJ_FAILED) \
                                            | (PDSM_LITE_PD_EVENT_COARSE_POS_INJ_ERROR))

#define PDSM_LITE_PD_EVENT_UPDATE_FAILURE           0x01000000  /* Update event indicating Failure */

#define PDSM_LITE_PD_EVENT_MASK  ( (PDSM_LITE_PD_GENERAL_EVENT_MASK) \
                             | (PDSM_LITE_PD_GPS_EVENT_MASK) \
                             | (PDSM_LITE_PD_EVENT_COARSE_POS_INJ_MASK) \
                             | (PDSM_LITE_PD_STATUS_NMEA_POS_REPORT) \
                             | (PDSM_LITE_PD_STATUS_SV_INFO_REPORT) \
                            )
                            
/* --------------------- EVENT CALLBACK DATA STRUCTURES -------------------*/

#define PDSM_LITE_PD_POSITION_SOURCE_GPS         0x0001  /* GPS Positions*/
#define PDSM_LITE_PD_POSITION_SOURCE_AFLT        0x0004  /* AFLT position */
#define PDSM_LITE_PD_LOCATION_SOURCE_HYBRID      0x0008  /* HYBRID position */
#define PDSM_LITE_PD_POSITION_SOURCE_DEFAULT     0x0020  /* REFERENCE position from the network in a MSB/MSA or CELL ID call flow */
#define PDSM_LITE_PD_POSITION_SOURCE_UNKNOWN     0x0040  /* source is unknown, could be an externally injected or propaged position */
#define PDSM_LITE_PD_POSITION_SOURCE_GLO         0x0080  /* Source is GLONASS */

/*
  EPI stands for External Position Injection. It is a general term used
  to mean any externally provided 2D or 3D position was used to constraint
  the position fix generated by the position engine. EPI examples are Coarse
  position injection, Cell Database position, Reference Location from the 
  AGPS server, etc.
*/
#define PDSM_LITE_PD_POSITION_SOURCE_EPI         0x0100  /* Externally Injected Position constraint used */
#define PDSM_LITE_PD_POSITION_SOURCE_BDS         0x0800  /* Source is BeiDou */
#define PDSM_LITE_PD_POSITION_SOURCE_GAL         0x1000  /* Source is Galileo */
#define PDSM_LITE_PD_POSITION_SOURCE_QZSS        0x2000  /* Source is QZSS  */
#define PDSM_LITE_PD_POSITION_SOURCE_CPI         0x4000  /* Specifically indicate that the EPI is a CPI */



/* valid Opitonal Fields */
//typedef byte pdsm_lite_pd_opt_field;

#define PDSM_LITE_PD_ALTITUDE_VALID 0x01
#define PDSM_LITE_PD_VELOCITY_VALID  0x02  /* Velocity means speed and direction */
#define PDSM_LITE_PD_UTCOFFSET_VALID 0x04
#define PDSM_LITE_PD_UNCERTAINTY_VALID 0x08
#define PDSM_LITE_PD_EXT_STATUS_VALID  0x10
#define PDSM_LITE_PD_MSEC_VALID 0x20       /* Millisecond field available */
#define PDSM_LITE_PM_DATA_AVAIL 0x40       /* PM data available */

#define PDSM_LITE_PD_MEAS_DEBUG 0x80       /* Additional measurement info */

#define PDSM_LITE_PD_OPT_FIELD_MASK  ( (PDSM_LITE_PD_ALTITUDE_VALID) | (PDSM_LITE_PD_VELOCITY_VALID) |\
                                  (PDSM_LITE_PD_UTCOFFSET_VALID) | (PDSM_LITE_PD_UNCERTAINTY_VALID) | \
                                  (PDSM_LITE_PD_EXT_STATUS_VALID) | (PDSM_LITE_PD_MSEC_VALID) )
typedef uint32 pdsm_lite_pd_opt_field_ext;
#define PDSM_LITE_PD_LAT_LONG_VALID 0x01 /* indicates if the lat/lon in the position report and ext pos is valid */
#define PDSM_LITE_PD_GPS_TIME_VALID 0x02 /* indicates if the time stamp in the ext pos has a valid GPS time */
#define PDSM_LITE_PD_XTRA_DATA_VALID 0x04 /* indicates if the XTRA data in the ext pos is valid */
#define PDSM_LITE_PD_UTC_TIME_VALID 0x08 /* indicates if the UTC time stamp in the ext pos is valid */
#define PDSM_LITE_PD_HEADING_VALID  0x10 /*indicates if heading in position report and ext pos is valid*/
#define PDSM_LITE_PD_EXT_VELOCITY_VALID  0x20 /*indicates if ITAR Velocity velHorU is valid*/

typedef uint16 gnss_sv_id_type;      /* SV IDs for GNSS constellations is given below */
#define  PDSM_LITE_PD_SV_ID_GPS_MIN      1
#define  PDSM_LITE_PD_SV_ID_GPS_MAX      32
#define  PDSM_LITE_PD_SV_ID_SBAS_MIN     33
#define  PDSM_LITE_PD_SV_ID_SBAS_MAX     64
#define  PDSM_LITE_PD_SV_ID_GLO_MIN      65
#define  PDSM_LITE_PD_SV_ID_GLO_MAX      96
#define  PDSM_LITE_PD_SV_ID_QZSS_MIN     193
#define  PDSM_LITE_PD_SV_ID_QZSS_MAX     197
#define  PDSM_LITE_PD_SV_ID_BDS_MIN     201
#define  PDSM_LITE_PD_SV_ID_BDS_MAX     237
#define  PDSM_LITE_PD_SV_ID_GAL_MIN     301
#define  PDSM_LITE_PD_SV_ID_GAL_MAX     336
#define  PDSM_LITE_PD_SV_ID_UNKNOWN     0xffff
/* Fix data Stuct for Pos completetion msg SCU=>MS data in Text format.*/
typedef struct pdsm_lite_utc_time_s
{

  /* UTC Time  for Hour, Minute, Sec */
  uint8         hour;                /*  in range of  00..23 */
  uint8         minute;              /*  in range of 00..59  */
  uint16        sec;                 /*  ssss [x100 sec]  in range of  0..5999 */

  uint32        date;                /* UTC Date ddmmyyyy 
                                         dd  in range of [1..31]
                                         mm in range of [1..12]
                                         yyyy in range of [0..9999]   
                                     */
} pdsm_lite_utc_time_s_type;

typedef uint8 pdsm_lite_gnss_time_source_mask;
#define PDSM_LITE_GNSS_TIME_GPS    (0x01<<0)
#define PDSM_LITE_GNSS_TIME_GLO    (0x01<<1)
#define PDSM_LITE_GNSS_TIME_BDS    (0x01<<2)
#define PDSM_LITE_GNSS_TIME_GAL    (0x01<<3)

typedef enum {
  PDSM_LITE_GNSS_TIME_SRC_UNKNOWN                  = 0x00, /* Source of the Time is unknown */
  PDSM_LITE_GNSS_TIME_SRC_NETWORK_TIME_TRANSFER    = 0x01, /* Time is set by 1x system */
  PDSM_LITE_GNSS_TIME_SRC_NETWORK_TIME_TAGGING     = 0x02, /* Time is set by WCDMA/GSM time tagging.
                                                             That is, associating network time with GPS time */
  PDSM_LITE_GNSS_TIME_SRC_EXTERNAL_INPUT           = 0x03, /* Time is set by an external injection */
  PDSM_LITE_GNSS_TIME_SRC_GPS_TOW_DECODE           = 0x04, /* Time is set after decoding over the air GPS navigation data
                                                             from one GPS satellite */
  PDSM_LITE_GNSS_TIME_SRC_TOW_CONFIRMED            = 0x05, /* Time is set after decoding over the air GPS navigation data
                                                             from multiple satellites */
  PDSM_LITE_GNSS_TIME_SRC_TOW_WEEK_CONFIRMED       = 0x06, /* Both time of the week and the GPS week number known */
  PDSM_LITE_GNSS_TIME_SRC_RESERVED                 = 0x07, /* Reserved */
  PDSM_LITE_GNSS_TIME_SRC_NAV_SOLUTION             = 0x08, /* Time is set by position engine
                                                             after the fix is obtained. */
  PDSM_LITE_GNSS_TIME_SRC_SOLVE_FOR_TIME           = 0x09, /* Time is set by the position engine after performing SFT.
                                                             This is done when the clock time uncertainty is large */
  PDSM_LITE_GNSS_TIME_SRC_GLO_TOW_DECODE           = 0x0A, /* Time is set after decoding GLO satellites */
  PDSM_LITE_GNSS_TIME_SRC_TIME_TRANSFORM           = 0x0B, /* Time is set after transforming the GPS to GLO time */
  PDSM_LITE_GNSS_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING = 0x0C, /* Time is set by the sleep timetag
                                                             provided by WCDMA network */
  PDSM_LITE_GNSS_TIME_SRC_GSM_SLEEP_TIME_TAGGING   = 0x0D, /* Time is set by the sleep timetag
                                                             provided by GSM network */
  PDSM_LITE_GNSS_TIME_SRC_QZSS_TOW_DECODE          = 0x0E, /* Time is set after decoding QZSS satellites */
  PDSM_LITE_GNSS_TIME_SRC_BDS_TOW_DECODE           = 0x0F, /* Time is set after decoding BDS satellites */
  PDSM_LITE_GNSS_TIME_SRC_GAL_TOW_DECODE           = 0x10, /* Time is set after decoding GAL satellites */
  PDSM_LITE_GNSS_TIME_SRC_INVALID                  = 0xFF, /* Invalid time */
  PDSM_LITE_GNSS_TIME_SRC_MAX                      = 0x10000000  /* force enum to 32 bit */
} pdsm_lite_extended_time_src_e_type;

/* Originally defined in gnss_consts.h, redefined in pdapi.h to avoid including gnss_consts.h */
#define PDSM_LITE_N_ACTIVE_GPS_CHAN     16  /* Total number of active GPS channels */
#define PDSM_LITE_N_ACTIVE_GLO_CHAN     14  /* Total number of active Glonass channels */
#define PDSM_LITE_N_ACTIVE_BDS_CHAN     16  /* Total number of active BDS channels */
#define PDSM_LITE_N_ACTIVE_GAL_CHAN     16  /* Total number of active GAL channels */
#define PDSM_LITE_N_ACTIVE_QZSS_CHAN    16  /* Total number of active QZSS channels */
/* If set, GNSS SV measurement is valid (detected by searcher) */
#define PDSM_LITE_GNSS_SV_INFO_MEAS_VALID       0x00000001
/* If set, GNSS SV is usable: {GPS|BDS} PRNs or {GLO} Slot IDs */
#define PDSM_LITE_GNSS_SV_INFO_USABLE           0x00000002
/* If set, GNSS SV is used for pos fix:   {GPS|BDS} PRNs or {GLO} Slot IDs */
#define PDSM_LITE_GNSS_SV_INFO_USED_IN_POS_FIX  0x00000004
/* If set, GNSS SV is used for velocity fix: {GPS|BDS} PRNs or {GLO} Slot IDs */
#define PDSM_LITE_GNSS_SV_INFO_USED_IN_VEL_FIX  0x00000008


#ifdef FEATURE_GNSS_LEAN_BUILD
#pragma anon_unions
#endif

typedef struct
{
  uint32 q_SvInfoMask;  /* See SM_GNSS_SV_INFO_XXX definition bits above.
                         * For XXX_VALID bits, if set, then corresponding data is valid, otherwise not valid */

  uint16 w_SvId;        /* GNSS SV: {GPS|BDS|GAL} PRN or {GLO} SlotID. */

  /* GNSS SV-specific data for SVs used in Position Fix (valid if PDSM_LITE_GNSS_SV_INFO_USED_POS_FIX is set) */
  float  f_PrResMeters; /* Pseudo-range measurement residuals of GNSS SV */
  float  f_PrUncMeters; /* Pseudo-range weights (uncertainties) of GNSS SV */

  /* GNSS SV-specific data for SVs used in Velocity Fix (valid if SM_GNSS_SV_INFO_VEL_FIX_VALID is set) */
  float  f_PrrResMps;   /* Pseudo-range rate (doppler) measurement residuals of GNSS SV */
  float  f_PrrUncMps;   /* Pseudo-range rate (doppler) uncertainties of GNSS SV */

  union {
    uint16 w_IODE;      // IODE {GPS|BDS|GAL}
    uint8  u_GloTb;     // Tb for {GLO} SV
  };

  /* GNSS SV-specific data for SVs detected by searcher (valid if SM_GNSS_SV_INFO_MEAS_VALID is set) */
  int8   b_FreqNum;     /* Frequency number(-7~+6) of the Glonass SV, invalid for GPS/BDS SV
                           valid if PDSM_LITE_GNSS_SV_INFO_USED_POS_FIX is set as well*/

  uint8  u_SvFSAvail;   /* Indication for FineSpeed Availability 1:Available 0:NotAvailable */
  uint8  u_SvPrtyGood;  /* Indication for whether any parity errors have been
                         * observed in the last second for each GNSS SV
                         * = 1:Parity Good
                         * = 0:Parity Bad
                         */
  uint16 w_SvPrtyErr;   /* {GPS|BDS} the 10 LSB to indicate parity errors in the 10 words of current subframe
                         *     {GLO} boolean type
                         * (bit0 for word 1, ... bit9 for word 10)
                         * = 1:error
                         * = 0:no error
                         */
} pdsm_lite_GnssSvInfoType;

/* SV-specific data per GNSS for supplemental SV information
   Please note that SvMaskUsed & SvMaskUsable bitmask is redundant information, it can be generated
   by going through the SvInfo array and checking the q_SvInfoMask. It's kept here because of
   backward compatiblity */
typedef struct
{
  uint8               u_SvInfoValid;      /* TRUE if all info in this struct valid, otherwise FALSE */

  uint8               u_GpsNumSvMeas;     /* Number of GPS SVs with valid measurement (detected by searcher) */
  uint8               u_GpsNumSvPosFix;   /* Number of GPS SVs used in Position Fix */
  uint8               u_GpsNumSvVelFix;   /* Number of GPS SVs used in Velocity Fix */
  uint32              q_GpsSvMaskUsed;    /* Bitmask for GPS SVs used for calculating position fix */
  uint32              q_GpsSvMaskUsable;  /* Bitmask for GPS SVs usable for calculating position fix */
  pdsm_lite_GnssSvInfoType z_GpsSvInfo[PDSM_LITE_N_ACTIVE_GPS_CHAN]; /* SV-specific data for GPS supplemental SV information */

  uint8               u_GloNumSvMeas;     /* Number of GLO SVs with valid measurement (detected by searcher) */
  uint8               u_GloNumSvPosFix;   /* Number of GLO SVs used in Position Fix */
  uint8               u_GloNumSvVelFix;   /* Number of GLO SVs used in Velocity Fix */
  uint32              q_GloSvMaskUsed;    /* Bitmask for GLONASS SVs used for calculating position fix */
  uint32              q_GloSvMaskUsable;  /* Bitmask for GLONASS SVs useable for calculating position fix */
  pdsm_lite_GnssSvInfoType z_GloSvInfo[PDSM_LITE_N_ACTIVE_GLO_CHAN]; /* SV-specific data for GLO supplemental SV information */

  uint8               u_BdsNumSvMeas;     /* Number of BDS SVs with valid measurement (detected by searcher) */
  uint8               u_BdsNumSvPosFix;   /* Number of BDS SVs used in Position Fix */
  uint8               u_BdsNumSvVelFix;   /* Number of BDS SVs used in Velocity Fix */
  uint64              t_BdsSvMaskUsed;    /* Bitmask for BDS SVs used for calculating position fix */
  uint64              t_BdsSvMaskUsable;  /* Bitmask for BDS SVs usable for calculating position fix */
  pdsm_lite_GnssSvInfoType z_BdsSvInfo[PDSM_LITE_N_ACTIVE_BDS_CHAN]; /* SV-specific data for BDS supplemental SV information */

  uint8               u_GalNumSvMeas;     /* Number of GAL SVs with valid measurement (detected by searcher) */
  uint8               u_GalNumSvPosFix;   /* Number of GAL SVs used in Position Fix */
  uint8               u_GalNumSvVelFix;   /* Number of GAL SVs used in Velocity Fix */
  uint64              t_GalSvMaskUsed;    /* Bitmask for GAL SVs used for calculating position fix */
  uint64              t_GalSvMaskUsable;  /* Bitmask for GAL SVs usable for calculating position fix */
  pdsm_lite_GnssSvInfoType z_GalSvInfo[PDSM_LITE_N_ACTIVE_GAL_CHAN]; /* SV-specific data for GAL supplemental SV information */

  uint8               u_QzssNumSvMeas;     /* Number of QZSS SVs with valid measurement (detected by searcher) */
  uint8               u_QzssNumSvPosFix;   /* Number of QZSS SVs used in Position Fix */
  uint8               u_QzssNumSvVelFix;   /* Number of QZSS SVs used in Velocity Fix */
  uint8               u_QzssSvMaskUsed;    /* Bitmask for QZSS SVs used for calculating position fix */
  uint8               u_QzssSvMaskUsable;  /* Bitmask for QZSS SVs usable for calculating position fix */
  pdsm_lite_GnssSvInfoType z_QzssSvInfo[PDSM_LITE_N_ACTIVE_QZSS_CHAN]; /* SV-specific data for GPS supplemental SV information */
} pdsm_lite_GnssNavSvInfo;

/**
 * Structure containing Bias information for an individual sensor
 **/
typedef struct
{
  uint16  w_SensorValidMask; /* Use SENSOR_BIAS_VALID_xxx macro to find which of the axes have valid data */
  float f_XaxisValue;       /*  X-axis Bias
                            - Units gyroscope:(radians)/(seconds)
                            - Units accelerometer:(meters)/(seconds^2)*/
  float f_YaxisValue;       /*  Y-axis Bias
                            - Units gyroscope:(radians)/(seconds)
                            - Units accelerometer:(meters)/(seconds^2)*/
  float f_ZaxisValue;       /*  Z-axis Bias
                            - Units gyroscope:(radians)/(seconds)
                            - Units accelerometer:(meters)/(seconds^2)*/
} pdsm_lite_pd_sensor_bias_data;


typedef struct
{
  uint16                    w_SeqenceNumber; /* Incremental packet serial number (starting from 0), to identify missing packets. */
  pdsm_lite_pd_sensor_bias_data  z_GyroBias;     /* Contains axis validity mask + x,y,z values for Gyro bias */
  pdsm_lite_pd_sensor_bias_data  z_AccelBias;    /* Contains axis validity mask + x,y,z values for accel bias */
} pdsm_lite_pd_sensor_param_report;


typedef enum pdsm_lite_pd_clk_src_e
{
  PDSM_LITE_PD_CLK_SRC_HOST,
  PDSM_LITE_PD_CLK_SRC_NETWORK
} pdsm_lite_pd_clk_src_e_type;
typedef struct pdsm_lite_pd_external_time_info
{
  uint64  time_msec;       /* if src is host, then this field is "relative" not abs time. 
                 if src is network, then this is msec since 00:00:00 Jan 6., 1980 */
  uint32  time_unc_msec;    /* Assoc. time uncertainty */
  pdsm_lite_pd_clk_src_e_type time_source; /* time soruce of the slow clock */
  boolean time_is_gps; /* TRUE if time base is GPS, FALSE if time base is UTC . valid only if src is Network */
  uint8 discontinuity_count;   /* if source is Host, then signal any time discontinuties with this flag */
  boolean force_flag;      /* Force acceptance of data */
} pdsm_lite_pd_external_time_info_type;

/* start Coase Position Injection related */

typedef uint32 pdsm_lite_pd_extern_coarse_pos_opt_field;
#define PDSM_LITE_PD_EXTERN_COARSE_POS_GPS_TIME_VALID 0x0001
#define PDSM_LITE_PD_EXTERN_COARSE_POS_LAT_LONG_VALID 0x0002
#define PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_VALID      0x0004
#define PDSM_LITE_PD_EXTERN_COARSE_POS_ELLIPTICAL_UNC_VALID 0x0008
#define PDSM_LITE_PD_EXTERN_COARSE_POS_CIRCULAR_UNC_VALID   0x0010
#define PDSM_LITE_PD_EXTERN_COARSE_POS_HORIZ_CONF_VALID 0x0020
#define PDSM_LITE_PD_EXTERN_COARSE_POS_RAW_CIRCULAR_UNC_VALID 0x0040
#define PDSM_LITE_PD_EXTERN_COARSE_POS_RAW_HORIZ_CONF_VALID 0x0080



typedef enum
{
  PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_UNKNOWN,
  PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_GPS,
  PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_CELLID,
  PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_ENH_CELLID,
  PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_TERRESTRIAL,
  PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_GPS_TERRESTRIAL_HYBRID,
  PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_OTHER,
  PDSM_LITE_PD_EXTERN_COARSE_POS_SOURCE_MAX = 0x10000000  /* force enum to 32 bit */
}pdsm_lite_pd_extern_coarse_pos_source_e_type;

/* CPI position source provider */
typedef enum
{
  PDSM_LITE_PD_EXTERN_CPI_SRC_EXTERNAL, /* Position is sourced from an external module (i.e. GNP or third party) */
  PDSM_LITE_PD_EXTERN_CPI_SRC_INTERNAL, /* Position is sourced from an internal QCOM module */
  PDSM_LITE_PD_EXTERN_CPI_SRC_MAX = 0x10000000  /* force enum to 32 bit */
}pdsm_lite_pd_extern_cpi_src_provider_e_type;

typedef enum
{
  PDSM_LITE_PD_EXTERN_COARSE_POS_TIME_GPS,
  PDSM_LITE_PD_EXTERN_COARSE_POS_TIME_UTC,
  PDSM_LITE_PD_EXTERN_COARSE_POS_TIME_AGE,
  PDSM_LITE_PD_EXTERN_COARSE_POS_TIME_MAX = 0x10000000  /* force enum to 32 bit */
}pdsm_lite_pd_extern_coarse_pos_time_stamp_e_type;

typedef enum
{
  PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_HAE,  /* height above ellipsoid */
  PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_MSL,  /* height above mean sea level */
  PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_MAX = 0x10000000  /* force enum to 32 bit */
}pdsm_lite_pd_extern_coarse_pos_alt_def_e_type; /* definition of altitude */

typedef struct pdsm_lite_pd_horizontal_unc_elliptical
{
  float semimajor; /* semi-major axis length of elliptical horizontal uncertainty */
  float semiminor; /* semi-minor axis length*/
  float semimajor_azimuth; /* angle formed from the north */
} pdsm_lite_pd_horizontal_unc_elliptical_type;

typedef struct pdsm_lite_pd_external_pos_info
{
  /* indicating the presence/absence of the optional fields */
  pdsm_lite_pd_extern_coarse_pos_opt_field          opt_field_mask;

  /* info. of the time associated with the position. */
  pdsm_lite_pd_extern_coarse_pos_time_stamp_e_type  time_info_type;
  uint32                                       timestamp_sec; /** time of week 
                                                                  or age, in 
                                                                  seconds. */

  pdsm_lite_pd_extern_coarse_pos_alt_def_e_type alt_def; /* def. of altitude */

  pdsm_lite_pd_extern_coarse_pos_source_e_type   source;

  int32   latitude;  /* Lat.: rad. LSB = pi/(2^32), 2's comp. */
  int32   longitude; /* Long.: rad, LSB = pi /(2^31), 2's comp. */

  /* horizontal unc: circular shape, radial */
  uint32  loc_unc_hor;   /* horizontal location unc.: meters */

  int16   altitude; /* alt: meters, positive height, negative depth  */

  /* vertical unc */
  uint16   loc_unc_vert;   /* loc unc vertical: meters */

  /* confidence horizontal: percentage, as defined by  ETSI TS 101 109. 
     0 - no info. to be rejected; 100-127 not used; if 100 is
     received, re-interpret to 99 */
  uint8    confidence_hor;

  /* confidence vertical: percentage, as defined by  ETSI TS 101 109. 
     0 - no info. to be rejected; 100-127 not used; if 100 is
     received, re-interpret to 99 */
  uint8    confidence_vert;

  float loc_unc_hor_raw; /*Non optimized Horizontal uncertainity*/

  /* Raw confidence horizontal: percentage, as defined by  ETSI TS 101 109. 
     0 - no info. to be rejected; 100-127 not used; if 100 is
     received, re-interpret to 99 */
  uint8    confidence_hor_raw; /*Non optimized Horizontal Confidence*/

}pdsm_lite_pd_external_pos_info_type;

typedef struct pdsm_lite_pd_external_pos_info_ex
{
  pdsm_lite_pd_external_pos_info_type pos_info;
  pdsm_lite_pd_reliability_e_type e_HoriRelIndicator; /*horizontal reliability*/
  pdsm_lite_pd_reliability_e_type e_VertRelIndicator; /*vertical reliability*/

  /* source of altitude in injected position */
  pdsm_lite_pd_altitude_source_e_type             e_AltitudeSource;
  /* linkage / dependence between injected horizontal position and altitude */
  pdsm_lite_pd_altitude_horizontal_linkage_e_type e_AltHorLinkage;
  /* uncertainty coverage of the injected altitude */
  pdsm_lite_pd_altitude_unc_coverage_e_type       e_AltUncCoverage;

} pdsm_lite_pd_external_pos_info_ex_type;

/* Conversion factor for lat/long rad. <-> degree */
#define PDSM_LITE_COARSE_LAT_SCALE_FACTOR  23860929.4222   // 2^32/180
#define PDSM_LITE_COARSE_LON_SCALE_FACTOR  11930464.7111   // 2^31/180

#define PDSM_LITE_COARSE_LAT_RAD_TO_DEG_SCALE_FACTOR  (1.0/PDSM_LITE_COARSE_LAT_SCALE_FACTOR)
#define PDSM_LITE_COARSE_LON_RAD_TO_DEG_SCALE_FACTOR  (1.0/PDSM_LITE_COARSE_LON_SCALE_FACTOR)

typedef struct pdsm_lite_pd_external_position_info
{
  struct
  {
    unsigned posSrcProviderValid : 1; /* Indicate if position source provide field is valid to read */
  }m;

  /* indicating the presence/absence of the optional fields */
  pdsm_lite_pd_extern_coarse_pos_opt_field          opt_field_mask;

  /* info. of the time associated with the position. */
  pdsm_lite_pd_extern_coarse_pos_time_stamp_e_type  time_info_type;
  uint32                                       timestamp_sec; /** time of week 
                                                                  or age, in 
                                                                  seconds. */

  pdsm_lite_pd_extern_coarse_pos_alt_def_e_type alt_def; /* def. of altitude */

  pdsm_lite_pd_extern_coarse_pos_source_e_type   source;
  pdsm_lite_pd_extern_cpi_src_provider_e_type    src_provider; /* valid when posSrcProviderValid == 1 */

  double   latitude;  /* Lat.: in degrees -90.0 to 90.0, positive for northern */
  double   longitude; /* Long.: in degrees -180.0 to 180.0, positive for eastern */

  /* horizontal unc: circular shape, radial */
  float  loc_unc_hor;   /* horizontal location unc.: meters */

  /* horizontal unc: circular shape, radial */
  float  loc_unc_hor_raw;   /*Non optimized  horizontal location unc.: meters */

  float   altitude; /* alt: meters, positive height, negative depth  */

  /* vertical unc */
  float   loc_unc_vert;   /* loc unc vertical: meters */

  /* confidence horizontal: percentage, as defined by  ETSI TS 101 109. 
     0 - no info. to be rejected; 100-127 not used; if 100 is
     received, re-interpret to 99 */
  uint8    confidence_hor;

  /* Non optimized confidence horizontal: percentage, as defined by  ETSI TS 101 109. 
     0 - no info. to be rejected; 100-127 not used; if 100 is
     received, re-interpret to 99 */
  uint8    confidence_hor_raw;

  /* confidence vertical: percentage, as defined by  ETSI TS 101 109. 
     0 - no info. to be rejected; 100-127 not used; if 100 is
     received, re-interpret to 99 */
  uint8    confidence_vert;

  pdsm_lite_pd_reliability_e_type e_HoriRelIndicator; /*horizontal reliability*/
  pdsm_lite_pd_reliability_e_type e_VertRelIndicator; /*vertical reliability*/

  /* source of altitude in injected position */
  pdsm_lite_pd_altitude_source_e_type             e_AltitudeSource;
  /* linkage / dependence between injected horizontal position and altitude */
  pdsm_lite_pd_altitude_horizontal_linkage_e_type e_AltHorLinkage;
  /* uncertainty coverage of the injected altitude */
  pdsm_lite_pd_altitude_unc_coverage_e_type       e_AltUncCoverage;

  /* Elliptical CPI info */
  pdsm_lite_pd_horizontal_unc_elliptical_type     horizontal_unc_elliptical;

  /*Whether this is an on demand position injection*/
  boolean v_is_on_demand_position_inj;
} pdsm_lite_pd_external_position_info_type;
/* end Coarse Position Injection related */


/* Position Determination Parameter event related. Do we need to combine the events together.
 *  
 * Caution: Don't add any new PA event bitmask. Use 'pdsm_lite_pa_e_type' to define PA events as enums.
*/
typedef int pdsm_lite_pa_event_type;

#define PDSM_LITE_PA_EVENT_NONE                        0x00       /* No PA event */
#define PDSM_LITE_PA_EVENT_GPS_LOCK                    0x40       /* GPS lock change */
#define PDSM_LITE_PA_EVENT_DELETE_PARAMS               0x0100     /* Delete parameters */
#define PDSM_LITE_PA_EVENT_DPO_CONTROL                 0x1000000  /* DPO Control */
#define PDSM_LITE_PA_EVENT_XTRAT_CLIENT_TOKEN          0x10000000 /*XTRAT client token*/
#define PDSM_LITE_PA_EVENT_XTRAT_SESSION_CONTROL       0x20000000 /*XTRAT session control*/
#define PDSM_LITE_PA_EVENT_XTRA_APN_PROFILE_SELECT     0x80000000 /*XTRA APN profile select*/

#define PDSM_LITE_PA_EVENT_MASK  (  (PDSM_LITE_PA_EVENT_GPS_LOCK) \
                             | (PDSM_LITE_PA_EVENT_DELETE_PARAMS) \
                             | (PDSM_LITE_PA_EVENT_DPO_CONTROL) \
                             | (PDSM_LITE_PA_EVENT_XTRAT_CLIENT_TOKEN) \
                             | (PDSM_LITE_PA_EVENT_XTRAT_SESSION_CONTROL) \
                             | (PDSM_LITE_PA_EVENT_XTRA_APN_PROFILE_SELECT) \
                            )


typedef struct pdsm_lite_pa_info_s {
  pdsm_lite_client_id_type      client_id;   /* PDSM originating client */
  pdsm_lite_pa_e_type           pa_event;
  pdsm_lite_pa_info_type        pa_info;     /* Info based on Event set */
  qword                    time_stamp;  /* system time stamp for the event */
} pdsm_lite_pa_info_s_type;
/*~ FIELD pdsm_lite_pa_info_s.pa_info DISC pdsm_lite_pa_info_s.pa_event */

/* Typedef structure used by user in providing PA set parameters to be set.
*/
typedef struct pdsm_lite_pa_set_s {
  pdsm_lite_pa_e_type     pa_set;            /* Parameter mask to be set */
  pdsm_lite_pa_info_type  info;

} pdsm_lite_pa_set_type;
/*~ FIELD pdsm_lite_pa_set_s.info DISC pdsm_lite_pa_set_s.pa_set */

                                      
/* Max Active Channels: GPS 16, GLO 14, BDS 16, GAL 16 less than 80 (defined in QMI-LOC) */
#define PDSM_LITE_EXT_MEAS_NUM_SVS 64

typedef enum
{
  PDSM_LITE_SV_STATE_IDLE,
  PDSM_LITE_SV_STATE_SEARCH,
  PDSM_LITE_SV_STATE_SEARCH_VERIFY,
  PDSM_LITE_SV_STATE_BIT_EDGE,
  PDSM_LITE_SV_STATE_TRACK,
  PDSM_LITE_SV_STATE_MAX
} pdsm_lite_sv_state;


/* Typedef struct for GNSS channel report.
*/
typedef struct pdsm_lite_cgps_chan_rpt {
  int32    ps_doppler;          /* Pseudodoppler */
  word     sv_code_ph_wh;       /* Satellite code phase, whole chips */
  word     sv_code_ph_fr;       /* Satellite code phase, fract chips */
  byte     mul_path_ind;        /* Multipath Indicator */
  int32    ps_range_rms_er;     /* Pseudorange RMS error */
} pdsm_lite_cgps_chan_rpt_type;

#define PDSM_LITE_PD_GPS_STATE_POS_VALID 0x0001 /* indicates lat/long and pos_unc_horizontal fields are valid */
#define PDSM_LITE_PD_GPS_STATE_ALT_VALID 0x0002 /* indicates alt and loc_Unc_vertical are valid */
#define PDSM_LITE_PD_GPS_STATE_TIME_MS_VALID 0x0004 /* indicates time stamp ms field is valid */
#define PDSM_LITE_PD_GPS_STATE_TIME_WN_VALID 0x0008 /* indicates timestamp week number field is valid */
#define PDSM_LITE_PD_GPS_STATE_TUNC_VALID    0x0010 /* indicates timestamp tunc field is valid */
#define PDSM_LITE_PD_GPS_STATE_EPH_VALID    0x0020 /* indicates eph svmask field is valid */
#define PDSM_LITE_PD_GPS_STATE_ALM_VALID    0x0040 /* indicates alm svmask field is valid */
#define PDSM_LITE_PD_GPS_STATE_XTRA_VALID    0x0080 /* indicates xtra data  svmask field is valid */
#define PDSM_LITE_PD_GPS_STATE_HEALTH_VALID  0x0200
#define PDSM_LITE_PD_GPS_STATE_VISIBLE_SV_VALID 0x0400

/*- - - - - - - - - - - - - -COMMAND ERRORS - - - - - - - - - - - - - - - - - - - - - - -*/

// TODO: review
typedef struct
{
  uint32 flags; /* flags to indicate which fields are valid */
  int32 lat;  /* Latitude: LSB = Pi/2^32, Range -Pi/2 to Pi/2; +Ve values indicate North and -Ve values indiate South. valid if flags is set to PDSM_PD_EXT_GPS_STATE_POS_VALID */
  int32 lon;  /* Longitude: LSB = Pi/2^31, Range -Pi to Pi;+ve values indicate East and -ve values indicate West  . valid if flags is set to PDSM_PD_EXT_GPS_STATE_POS_VALID*/
  int16 altitude; /* Altitude in meters. valid if flags is set to PDSM_PD_EXT_GPS_STATE_ALT_VALID */
  float  f_loc_unc_horizontal; /* horizontal Position Unc in meters.  valid if flags is set to PDSM_PD_EXT_GPS_STATE_POS_VALID */
  uint16  loc_unc_vertical; /* vertical unc in meters.  valid if flags is set to PDSM_PD_EXT_GPS_STATE_ALT_VALID  */
  dword timestamp_tow_ms; /* Timestamp in GPS TOW milliseconds.  valid if flags is set to PDSM_PD_EXT_GPS_STATE_TIME_MS_VALID*/
  uint16  timestamp_gps_week;  /*GPS week number. valid if flags is set to PDSM_PD_EXT_GPS_STATE_TIME_WN_VALID */
  uint32  time_unc_ms; /* Time uncertainty in milli seconds, floor at 20 ms. valid if flags is set to PDSM_PD_EXT_GPS_STATE_TUNC_VALID */
  uint8 engine_state; /*  GPS engine state on/off. always present, no flag */
  uint32  eph_svmask; /* SV Mask for the ephemeris. if the bit is set, eph for that SV is available. valid if flags is set to PDSM_PD_EXT_GPS_STATE_EPH_VALID */
  uint32  alm_svmask; /* SV Mask for the Almanac. if the bit is set, alm for that SV is available. valid if flags is set to PDSM_PD_EXT_GPS_STATE_ALM_VALID */
  uint32  iono_valid; /* IONO is Valid. valid if flags is set to PDSM_PD_EXT_GPS_STATE_IONO_VALID */
  uint32  health_svmask; /* SV mask for the health. if the bit is set, health for that SV is good. valid if flags is set to PDSM_PD_EXT_GPS_STATE_HEALTH_VALID */
  uint16  xtra_start_gps_week; /*  Current XTRA info is good starting this GPS week.valid if flags is set to PDSM_PD_EXT_GPS_STATE_XTRA_VALID  */
  uint16  xtra_start_gps_minutes; /*  Current XTRA information is good starting this many minutes into the week . valid if flags is set to PDSM_PD_EXT_GPS_STATE_XTRA_VALID */
  uint16  xtra_valid_duration_hours; /*  XTRA info is valid for this many hours. valid if flags is set to PDSM_PD_EXT_GPS_STATE_XTRA_VALID */

  uint32  visible_svmask; /* SV mask for visible SVs. if bit is set, SV is visible. valid if flags is set to PDSM_PD_EXT_GPS_STATE_VISIBLE_SV_VALID */
} pdsm_lite_pd_gps_state_s_type;

/* Enumeration of PD command errors returned by command
** callback functions.
*/
typedef uint32 pdsm_lite_pa_nmea_type;
#define PDAPI_LITE_NMEA_DEBUG 0x00040000   /* NMEA debugging enable*/

typedef struct
{
  pdsm_lite_pa_nmea_type nmea_type;  /* Nmea type should be PDAPI_NMEA_VAR_LEN_DEBUG */
  uint32   q_Length;            /* Length of NMEA buffer including the null termination */
  uint8   *pu_NmeaBuff;         /* Variable length NMEA debug buffer */
} pdsm_lite_pd_ext_var_len_debug_nmea_s_type;

typedef enum pdsm_lite_pd_cmd_err_e {

  PDSM_LITE_PD_CMD_ERR_NONE = -1,
  /* FOR INTERNAL USE OF PDSM ONLY! */

  PDSM_LITE_PD_CMD_ERR_NOERR,
  /* No errors found */

  /* Error codes resulting from clients feeding a PD command
  ** function with bad or inappropriate parameters.
  */

  PDSM_LITE_PD_CMD_ERR_CLIENT_ID_P,
  /* invalid client ID */

  PDSM_LITE_PD_CMD_ERR_SESS_TYPE_P,
  /* Bad session type parameter */
  PDSM_LITE_PD_CMD_ERR_PRIVACY_P, /* not used anymore */
  PDSM_LITE_PD_CMD_ERR_NET_ACCESS_P, /* not used anymore */
  PDSM_LITE_PD_CMD_ERR_OPERATION_P,
  PDSM_LITE_PD_CMD_ERR_NUM_FIXES_P,
  PDSM_LITE_PD_CMD_ERR_LSINFO_P,
  /* Wrong server Information parameters */

  PDSM_LITE_PD_CMD_ERR_TIMEOUT_P,
  /* Error in timeout parameter */

  PDSM_LITE_PD_CMD_ERR_QOS_P,
  /* Error in QOS accuracy thershold param */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Error codes resulting from clients trying to instruct
  ** PDSM to perform inappropriate actions relative to the current
  ** state.
  */

  PDSM_LITE_PD_CMD_ERR_NO_SESS_S,
  /* No session Active, while trying to end Session */

  PDSM_LITE_PD_CMD_ERR_BUSY_S,
  /* Session Busy status */

  PDSM_LITE_PD_CMD_ERR_OFFLINE_S,
  /* Phone is offline */

  PDSM_LITE_PD_CMD_ERR_GPS_LOCK_S,
  /* GPS is locked */

  PDSM_LITE_PD_CMD_ERR_STATE_S,
  /* The command is invalid in this state. (Ex When is phone is in E911CB) */

  PDSM_LITE_PD_CMD_ERR_NO_BUF_L,
  /* No available PDSM command buffers to queue the command */

  PDSM_LITE_PD_CMD_ERR_SEARCH_COM_L,
  /* Communication problems with Search - e.g. SRCH buffer shortage */

  PDSM_LITE_PD_CMD_ERR_CANT_RPT_NOW_S,
  /* PD Results cannot be reported at this time, retry later or wait. not used anymore */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  PDSM_LITE_PD_CMD_ERR_MODE_NOT_SUPPORTED_S, /* not used anymore */
  PDSM_LITE_PD_CMD_ERR_PERIODIC_NI_IN_PROGRESS_S,

  PDSM_LITE_PD_CMD_ERR_AUTH_FAIL_S,
  /* Client Authentication Failure */

  PDSM_LITE_PD_CMD_ERR_OTHER,
  /* A problem other than the above was found */

  PDSM_LITE_PD_CMD_ERR_FEATURE_NOT_ENABLED,
  /* feature not enabled */

  PDSM_LITE_PD_CMD_ERR_UNKNOWN_CMD,
  /* unknown command */

  PDSM_LITE_PD_CMD_ERR_INVALID_PARAM,
  /* invalid parameters */

  PDSM_LITE_PD_CMD_ERR_GNSS_SERVICE_DISABLED,
  /*GNSS service is disabled*/

  PDSM_LITE_PD_CMD_ERR_GTP_WWAN_SERVICE_DISABLED,
  /*GTP WWAN service is disabled*/

  PDSM_LITE_PD_CMD_ERR_GNSS_SRVC_NOT_AVAILABLE,
  /*GNSS service is unavailable as MGP is not loaded*/
  
  PDSM_LITE_PD_CMD_ERR_MAX,
  /* FOR INTERNAL USE OF PDSM ONLY! */

  PDSM_LITE_PD_CMD_ERR_E_SIZE = 0x10000000
  /* To force enum to 32 bit */

} pdsm_lite_pd_cmd_err_e_type;
/*~ SENTINEL pdsm_lite_pd_cmd_err_e_type.PDSM_LITE_PD_CMD_ERR_MAX */

/* Enumeration of PA command errors returned by command
** callback functions.
*/
typedef enum pdsm_lite_pa_cmd_err_e {
  PDSM_LITE_PA_CMD_ERR_NONE = -1,        /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_LITE_PA_CMD_ERR_NOERR,          /* No errors found */
  PDSM_LITE_PA_CMD_ERR_NO_BUF_L,       /* No buffers available */
  PDSM_LITE_PA_CMD_ERR_CLIENT_ID_P,    /* Invalid Client id */
  PDSM_LITE_PA_CMD_ERR_PARAM_TYPE_P,   /* Error in Parameters */
  PDSM_LITE_PA_CMD_ERR_GPS_LOCK_P,     /* Error in Lock type */
  PDSM_LITE_PA_CMD_ERR_OFFLINE_S,      /* If phone is offline state */
  PDSM_LITE_PA_CMD_ERR_PA_ACTIVE_S,    /* An active pa_cmd is running */
  PDSM_LITE_PA_CMD_ERR_APP_INFO_P,     /* Wrong app info */
  PDSM_LITE_PA_CMD_ERR_DELETE_PARM_P,  /* wrong delete param type */
  PDSM_LITE_PA_CMD_ERR_POSITION_MODE_P, /* wrong position mode type */
  PDSM_LITE_PA_CMD_ERR_SRV_SYS_P,      /* wrong serving system type*/
  PDSM_LITE_PA_CMD_ERR_OTHER,          /* A problem other than the above was found */
  PDSM_LITE_PA_CMD_ERR_UNKNOWN_CMD,    /* Unknown command */
  PDSM_LITE_PA_CMD_ERR_NOT_ALLOWED,    /* Command not allowed in current state */
  PDSM_LITE_PA_CMD_ERR_GNSS_SRVC_NOT_AVAILABLE,
  PDSM_LITE_PA_CMD_ERR_MAX,          /* A problem other than the above was found */
  PDSM_LITE_PA_CMD_ERR_E_SIZE = 0x10000000   /* Fix the size as int */
} pdsm_lite_pa_cmd_err_e_type;
/*~ SENTINEL pdsm_lite_pa_cmd_err_e_type.PDSM_LITE_PA_CMD_ERR_MAX */

/* Enum for gpsOneXTRA command errors
*/
typedef enum pdsm_lite_xtra_cmd_err_e
{
  PDSM_LITE_XTRA_CMD_ERR_NONE = -1,

  PDSM_LITE_XTRA_CMD_ERR_NOERR,
  PDSM_LITE_XTRA_CMD_ERR_CLIENT_ID_P,
  PDSM_LITE_XTRA_CMD_ERR_NO_BUF_L,
  PDSM_LITE_XTRA_CMD_ERR_BAD_CRC,
  PDSM_LITE_XTRA_CMD_ERR_TIME_OF_VALIDITY_OUT_OF_RANGE,
  PDSM_LITE_XTRA_CMD_ERR_DOWNLOAD_INTRVL_OUT_OF_RANGE,
  PDSM_LITE_XTRA_CMD_ERR_INTERNAL_RESOURCE_ERROR,
  PDSM_LITE_XTRA_CMD_ERR_GPS_SUBSYSTEM_BUSY,
  PDSM_LITE_XTRA_CMD_ERR_TIME_INFO_ERROR,
  PDSM_LITE_XTRA_CMD_ERR_OTHER,
  PDSM_LITE_XTRA_CMD_ERR_GPS_LOCK_S,           /* GPS is locked */
  PDSM_LITE_XTRA_CMD_ERR_STATE_S,              /* Command is invalid in this state. (Ex When is phone is in E911CB) */
  PDSM_LITE_XTRA_CMD_ERR_UNKNOWN_CMD,          /* Unknown command */
  PDSM_LITE_XTRA_CMD_ERR_FEATURE_NOT_ENABLED,  /* Feature not enabled */
  PDSM_LITE_XTRA_CMD_ERR_INVALID_PARAM,        /* Invalid parameter */
  PDSM_LITE_XTRA_CMD_ERR_MAX,
  PDSM_LITE_XTRA_CMD_ERR_E_SIZE = 0x10000000
} pdsm_lite_xtra_cmd_err_e_type;
/*~ SENTINEL pdsm_lite_xtra_cmd_err_e_type.PDSM_LITE_XTRA_CMD_ERR_MAX */

/* Enum for XTRA-T command errors
*/
typedef enum
{
  PDSM_LITE_XTRA_T_CMD_ERR_NONE = -1,
  PDSM_LITE_XTRA_T_CMD_ERR_SUCCESS,
  PDSM_LITE_XTRA_T_CMD_ERR_BUSY,          /* Another XTRA-T session is in progress */
  PDSM_LITE_XTRA_T_CMD_ERR_OTHER,          /* Ending due to internal errors - data pointer error, E911, GPS lock, cmd not supported  */
  PDSM_LITE_XTRA_T_CMD_ERR_CLIENT_ID_P,    /* Ending due to XTRA-T client not registered/available */
  PDSM_LITE_XTRA_T_CMD_FEATURE_NOT_ENABLED, /* Ending due to XTRA-T feature is not enabled */
  PDSM_LITE_XTRA_T_CMD_ERR_BAD_GET_DATA_PARAM, /* Ending due to bad XTRA-T get data parameters */
  PDSM_LITE_XTRA_T_CMD_ERR_INTERNAL_RESOURCE, /* Ending due to internal resource errors - TM task not active */
  PDSM_LITE_XTRA_T_CMD_ERR_GPS_LOCK_S,           /* GPS is locked */
  PDSM_LITE_XTRA_T_CMD_ERR_STATE_S,              /* Command is invalid in this state. (Ex When is phone is in E911CB) */
  PDSM_LITE_XTRA_T_CMD_ERR_UNKNOWN_CMD,          /* Unknown command */
  PDSM_LITE_XTRA_T_CMD_ERR_FEATURE_NOT_ENABLED,  /* Feature not enabled */
  PDSM_LITE_XTRA_T_CMD_ERR_INVALID_PARAM,        /* Invalid parameter */
  PDSM_LITE_XTRA_T_CMD_END_MAX,                /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_LITE_XTRA_T_CMD_END_E_SIZE = 0x10000000   /* To force enum to 32 bit for windows NT */
} pdsm_lite_xtra_t_cmd_err_e_type;
/*~ SENTINEL pdsm_lite_xtra_t_cmd_err_e_type.PDSM_LITE_XTRA_T_CMD_END_MAX */

/*
** Enumerations for end of session reasons.
*/

typedef enum pdsm_lite_pd_end_e {
  PDSM_LITE_PD_END_SESS_NONE = -1,     /* For internal use of PDSM */
  PDSM_LITE_PD_END_CLIENT_END,         /* User ended the session */
  PDSM_LITE_PD_END_TIMEOUT,            /* Timeout (viz., for GPS Search) */
  PDSM_LITE_PD_END_FIX_ERROR,             /* Error in Fix */
  PDSM_LITE_PD_END_CLIENT_HAS_NO_SESS,  /* Current session doesn't belong to this client */
  PDSM_LITE_PD_END_GNSS_SRVC_NOT_AVAILABLE, /*GNSS service cannot be provide as MGP cannot be loaded*/
  PDSM_LITE_PD_END_MAX,                /* Maximum for PDSM internal use */
  PDSM_LITE_PD_END_E_SIZE = 0x10000000 /* To fix enum Size as int */
} pdsm_lite_pd_end_e_type;

#define PDSM_LITE_LATLONG_SCALE       (100000000)
/* For SVs in view, active channels only, using 16*3=48 for now */
#define PDSM_LITE_GNSS_MAX_SATELLITES PDSM_LITE_EXT_MEAS_NUM_SVS




 
	 	 	 
#define   PDSM_LITE_PD_FIX_STATUS_DEBUG_FAIL            (0x0000)       /* Unsuccessful fix*/
#define   PDSM_LITE_PD_FIX_STATUS_DEBUG_SUCCESS         (0x0001)       /* Successful Fix.  */
#define   PDSM_LITE_PD_FIX_STATUS_DEBUG_LOW_SV          (0x0002)       /* LOW SV's.*/
#define   PDSM_LITE_PD_FIX_STATUS_DEBUG_HEPE_FAIL       (0x0004)       /* HEPE Check Failed.   */
#define   PDSM_LITE_PD_FIX_STATUS_DEBUG_LOW_RELIABILITY (0x0008)       /* LOW reliability Fix */

typedef enum
{
  PD_LOC_SV_SYSTEM_GPS                    = 1,
  /**< GPS satellite. */

  PD_LOC_SV_SYSTEM_GALILEO                = 2,
  /**< GALILEO satellite. */


  PD_LOC_SV_SYSTEM_COMPASS                = 4,
  /**< COMPASS satellite. */

  PD_LOC_SV_SYSTEM_GLONASS                = 5,
  /**< GLONASS satellite. */

  PD_LOC_SV_SYSTEM_BDS                    = 6,
  /**< BDS satellite. */
  PD_LOC_SV_SYSTEM_QZSS                   = 7,
  PD_LOC_SV_SYSTEM_MAX                    = 8
  /**< QZSS satellite. */
} pdsm_lite_LocSvSystemEnumType;

typedef enum
{
  PD_LOC_FREQ_SOURCE_INVALID = 0,
  /**< Source of the frequency is invalid */
  PD_LOC_FREQ_SOURCE_EXTERNAL = 1,
  /**< Source of the frequency is from external injection */
  PD_LOC_FREQ_SOURCE_PE_CLK_REPORT = 2,
  /**< Source of the frequency is from Navigation engine */
  PD_LOC_FREQ_SOURCE_UNKNOWN = 3
  /**< Source of the frequency is unknown */
} pdsm_lite_LocSourceofFreqEnumType;


typedef struct
{
  float                                        f_clockDrift;
  /**< Receiver clock Drift \n
       - Units: meter per sec \n
  */
  float                                        f_clockDriftUnc;
  /**< Receiver clock Drift uncertainty \n
       - Units: meter per sec \n
  */

  pdsm_lite_LocSourceofFreqEnumType        e_sourceOfFreq;
  /**< Source of the frequency \n
           @ENUM()                        \n
  */
}pdsm_lite_LocRcvrClockFrequencyInfoStructType;

typedef struct
{
  uint8                                        u_leapSec;
  /**< GPS time leap second delta to UTC time  \n
               - Units: sec \n
  */
  uint8                                        u_leapSecUnc;
  /**< Uncertainty for GPS leap second \n
               - Units: sec \n
  */
}pdsm_lite_LocLeapSecondInfoStructType;

typedef enum	 	 
	{	 	 
	  PD_LOC_SYS_TIME_BIAS_VALID                = 0x01,	 	 
	   /**< System time bias valid */	 	 
	 	 	 
	  PD_LOC_SYS_TIME_BIAS_UNC_VALID            = 0x02,	 	 
	   /**< System time bias uncertainty valid */	 	 
	 	 	 
}pdsm_lite_LocInterSystemBiasValidMaskType;

typedef struct
{
  uint32                                        u_validMask;        /* Validity mask as per enum pdsm_lite_LocInterSystemBiasValidMaskType */
  /**< Fields that are valid.

Valid bitmasks: \begin{itemize1}
\item    0x01 -- SYS_TIME_BIAS_VALID
\item    0x02 -- SYS_TIME_BIAS_UNC_VALID
 */
  float                                        f_sys1Sys2TB;
  /**< System-1 to System-2 Time Bias  \n
               - Units: msec \n
  */
  float                                        f_sys1Sys2TBUnc;
  /**< System-1 to System-2 Time Bias uncertainty  \n
               - Units: msec \n
  */
}pdsm_lite_InterSystemBiasStructType;

typedef struct
{

  uint32  q_refFCount;
  /**< Receiver frame counter value at reference tick. Mandatory field if strcture is present */

  boolean b_SystemRtcValid;
  /**< Validity indicator for System RTC */

  uint64  t_systemRtcMs;
  /**< System RTC value at clock validity \n
          - Units: msec \n
  */
  uint32 q_NumClockResets;
  /**< Cumulative # of Clock resets. >*/

  uint16                 w_systemWeek;
  /**< System week number for GPS, BDS and GAL satellite systems. \n
          Set to 65535 when invalid or not available. \n
          Not valid for GLONASS system. \n
          Mandatory field if strcture is present
  */

  uint8       u_gloFourYear;
  /**< GLONASS four year number from 1996. \n
          Applicable only for GLONASS and shall be ignored for other constellations. \n
          Mandatory field if strcture is present
  */

  uint16      w_gloDays;
  /**< GLONASS day number in four years.
          Applicable only for GLONASS and shall be ignored for other constellations. \n
          Mandatory field if strcture is present
  */

  uint32                 q_systemMsec;
  /**< System time msec. Time of Week for GPS, BDS, GAL and Time of Day for GLONASS.
          - Units: msec \n
          Mandatory field if strcture is present. Interpretation of value based on SourceOfTime field.
  */

  float                  f_systemClkTimeBias;
  /**< System clock time bias \n
                  - Units: msec \n
          System time = systemMsec - systemClkTimeBias \n
          Mandatory field if strcture is present. Interpretation of value based on SourceOfTime field.
  */

  float                  f_systemClkTimeUncMs;
  /**< Single sided maximum time bias uncertainty \n
                          - Units: msec \n
           Mandatory if structure is present.                                
  */

  uint8                 e_SourceOfTime;
  /**< Source of the time. Mandatory if structure is present */

}pdsm_lite_LocSystemTimeStructType;

typedef struct
{
  uint32                                                         q_svMs;
  /**<         Satellite time milisecond.\n
                  For GPS, BDS, GAL range of 0 thru (604800000-1) \n
                  For GLONASS range of 0 thru (86400000-1) \n
                  Valid when PD_LOC_MEAS_STATUS_MS_VALID bit is set in measurement status \n
                  Note: All SV times in the current measurement block are alredy propagated to common reference time epoch. \n
                          - Units: msec \n
                  Mandtory field, interpretation based on measurement status
  */

  float                                                         f_svSubMs;
  /**<         Satellite time sub-millisecond. \n
                  Total SV Time = svMs + svSubMs \n
                          - Units: msec \n
                   Mandtory field, interpretation based on measurement status
  */

  float      f_svTimeUncMs;
  /**<  Satellite Time uncertainty \n
                          - Units: msec \n
                   Mandtory field, interpretation based on measurement status
  */

  float                                                        f_dopplerShift;
  /**< Satellite Doppler \n
               - Units: meter per sec \n
                   Mandtory field, interpretation based on measurement status
  */

  float                                                        f_dopplerShiftUnc;
  /**< Satellite Doppler uncertainty\n
               - Units: meter per sec \n
                   Mandtory field, interpretation based on measurement status
  */

}pdsm_lite_LocSVTimeSpeedStructType;


typedef enum
{
  PD_LOC_SVINFO_MASK_HAS_EPHEMERIS   = 0x01,
  /**< Ephemeris is available for this SV */
  PD_LOC_SVINFO_MASK_HAS_ALMANAC     = 0x02
  /**< Almanac is available for this SV */
}pdsm_lite_LocSvInfoMaskT;


typedef enum
{
  PD_LOC_SV_SRCH_STATUS_IDLE                   = 1,
  /**< SV is not being actively processed */

  PD_LOC_SV_SRCH_STATUS_SEARCH                 = 2,
  /**< The system is searching for this SV */

  PD_LOC_SV_SRCH_STATUS_TRACK                  = 3,
  /**< SV is being tracked */

}pdsm_lite_LocSvSearchStatusEnumT;

typedef struct
{
  uint16                             w_gnssSvId;
  /**< GNSS SV ID.
       \begin{itemize1}
       \item Range:  \begin{itemize1}
         \item For GPS:      1 to 32
         \item For GLONASS:  1 to 32
         \item For BDS:      201 to 237
       \end{itemize1} \end{itemize1}

      The GPS and GLONASS SVs can be disambiguated using the system field. */

  uint8                                                        u_gloFrequency;
  /**< GLONASS frequency number + 7 \n
       Valid only for GLONASS System \n
           Shall be ignored for all other systems \n
               - Range: 1 to 14 \n
  */

  pdsm_lite_LocSvSearchStatusEnumT                                e_svStatus;
  /**< Satellite search state \n
               @ENUM()
       */

  boolean                                                        b_healthStatus_valid;
  /**< SV Health Status validity flag\n
                - 0: Not valid \n
                - 1: Valid \n
*/

  uint8                             u_healthStatus;
  /**< Health status.
       \begin{itemize1}
       \item    Range: 0 to 1; 0 = unhealthy, \n 1 = healthy, 2 = unknown
       \vspace{-0.18in} \end{itemize1}*/

  pdsm_lite_LocSvInfoMaskT                 e_svInfoMask;
  /**< Indicates whether almanac and ephemeris information is available. \n

      @MASK()
  */


  uint64                         t_measurementStatusValid;
  /**< Bitmask indicating SV measurement status bits that are valid

*/

  uint64                    t_measurementStatus;
  /**< Bitmask indicating SV measurement status.

       Valid bitmasks: \n
       @MASK()  */

  uint16                                                        w_CNo;
  /**< Carrier to Noise ratio  \n
               - Units: 0.1 dBHz \n
  */

  uint16                                                  w_gloRfLoss;
  /**< GLONASS Rf loss reference to Antenna. \n
                       - Units: dB, Scale: 0.1 \n
  */

  boolean                                                        b_lossOfLockValid;
  /**< Loss of lock validity flag\n
                   - 0: Loss of lock not valid \n
                   - 1: Loss of lock valid \n
  */
  boolean                                                        b_lossOfLock;
  /**< Loss of signal lock indicator  \n
                   - 0: Signal in continuous track \n
                   - 1: Signal not in track \n
  */
  int16                                                   x_measLatency;
  /**< Age of the measurement. Positive value means measurement precedes ref time. \n
                       - Units: msec \n
  */

  pdsm_lite_LocSVTimeSpeedStructType        z_svTimeSpeed;
  /**< Unfiltered SV Time and Speed information */


  boolean                                                  b_dopplerAccelValid;
  /**< Doppler acceleartion validity flag\n
                   - 0: Doppler acceleation not valid \n
                   - 1: Doppler acceleation estimate valid \n
  */

  float     f_dopplerAccel;
  /**< Satellite Doppler Accelertion\n
               - Units: Hz/s \n
  */


  boolean                                                        b_multipathEstValid;
  /**< Multipath estimate validity flag\n
                   - 0: Multipath estimate not valid \n
                   - 1: Multipath estimate valid \n
  */
  float                                                        f_multipathEstimate;
  /**< Estimate of multipath in measurement\n
               - Units: Meters \n
  */
  boolean                                                        b_fineSpeedValid;
  /**< Fine speed validity flag\n
                   - 0: Fine speed not valid \n
                   - 1: Fine speed valid \n
  */
  float                                                        f_fineSpeed;
  /**< Carrier phase derived speed \n
               - Units: m/s \n
  */
  boolean                                                        b_fineSpeedUncValid;
  /**< Fine speed uncertainty validity flag\n
                   - 0: Fine speed uncertainty not valid \n
                   - 1: Fine speed uncertainty valid \n
  */
  float                                                        f_fineSpeedUnc;
  /**< Carrier phase derived speed \n
               - Units: m/s \n
  */
  boolean                                                        b_carrierPhaseValid;
  /**< Carrier Phase measurement validity flag\n
                   - 0: Carrier Phase not valid \n
                   - 1: Carrier Phase valid \n
  */
  double                                                        d_carrierPhase;
  /**< Carrier phase measurement [L1 cycles] \n
  */

  boolean                                                  b_cycleSlipCountValid;
  /**< Cycle slup count validity flag\n
                   - 0: Not valid \n
                   - 1: Valid \n
  */

  uint8                                                           u_cycleSlipCount;
  /**< Increments when a CSlip is detected */

  boolean                                                         b_svDirectionValid;
  /**< Validity flag for SV direction */

  float                                                           f_svAzimuth;
  /**< Satellite Azimuth
                       - Units: radians \n
       */

  float                                                           f_svElevation;
  /**< Satellite Elevation
                      - Units: radians \n
      */
} pdsm_lite_LocSVMeasurementStructType;


#define PDSM_LITE_EXT_GNSS_MEAS_NUM_SVS             16
/**< Maximum number of satellites in measurement block for given system. */

typedef struct
{
  pdsm_lite_LocSvSystemEnumType                                  e_system;
  /**< Specifies the Satellite System Type
  */
  boolean                                                         b_isSystemTimeValid;
  /**< Indicates whether System Time is Valid:\n
       - 0x01 (TRUE) --  System Time is valid \n
       - 0x00 (FALSE) -- System Time is not valid */
  pdsm_lite_LocSystemTimeStructType                         z_systemTime;
  /**< System Time Information \n
  */

  uint8                u_NumSvs;
  /* Number of SVs in this report block */

  pdsm_lite_LocSVMeasurementStructType                 z_svMeasurement[PDSM_LITE_EXT_GNSS_MEAS_NUM_SVS];
  /**< Satellite measurement Information \n
  */
} pdsm_lite_gnss_clockMeasurementStructType;

typedef struct
{
  uint8                                                  u_seqNum;                         /**< Current message Number */
  uint8                                           u_maxMessageNum;         /**< Maximum number of message that will be sent for present time epoch. */

  boolean                                                        b_LeapSecValid;
  pdsm_lite_LocLeapSecondInfoStructType        z_LeapSec;

  pdsm_lite_InterSystemBiasStructType    z_gpsGloInterSystemBias; /* Inter GNSS time bias structure */

  pdsm_lite_InterSystemBiasStructType    z_gpsBdsInterSystemBias; /* Inter GNSS time bias structure */

  pdsm_lite_InterSystemBiasStructType    z_gpsGalInterSystemBias; /* Inter GNSS time bias structure */

  pdsm_lite_InterSystemBiasStructType    z_bdsGloInterSystemBias; /* Inter GNSS time bias structure */

  pdsm_lite_InterSystemBiasStructType    z_galGloInterSystemBias; /* Inter GNSS time bias structure */

  pdsm_lite_InterSystemBiasStructType    z_galBdsInterSystemBias; /* Inter GNSS time bias structure */

  boolean                                                        b_clockFreqValid;
  pdsm_lite_LocRcvrClockFrequencyInfoStructType z_clockFreq;   /* Freq */

  boolean                                                        b_GnssMeasValid;
  pdsm_lite_gnss_clockMeasurementStructType           z_GnssMeas;
} pdsm_lite_pd_ext_gnss_meas_s_type;



typedef struct
{
  uint16   w_Sv;     /* Sv ID */
  /*GPS: 1-32, GLO: 65-96, QZSS: 193-197,
                   BDS: 201-237 (PRN+200), GAL: 301-336, 0: Invalid
                   All others are reserved*/
  int8    b_FreqNum;      /* Freq index, only valid if u_SysInd is GLO */
  uint16    w_SvPolyFlags;  /* Indicates the validity of data components, see those
                           C_SV_POLY_... defintions above */
  uint16  w_Iode;   /* Ephemeris reference time
                                      GPS:Issue of Data Ephemeris used [unitless].
                                       GLO: Tb 7-bit, refer to ICD02 
                                       GAL 10 bits   */
  double   d_T0;           /* Reference time for polynominal calculations
                           GPS: Secs in week.
                           GLO: Full secs since Jan/01/96 */
  double   pd_xyz0[3];     /* C0X, C0Y, C0Z */
  double   pd_xyzN[9];     /* C1X, C2X ... C2Z, C3Z */
  float   pf_other[4];    /* C0T, C1T, C2T, C3T */
  float   f_PosUnc;       /* SV position uncertainty [m]. */
  float   f_IonoDelay;    /* Ionospheric delay at d_T0 [m]. */
  float   f_IonoDot;      /* Iono delay rate [m/s].  */
  float   f_TropoDelay;   /* Tropospheric delay [m]. */
  float   f_Elevation;    /* Elevation [rad] at d_T0 */
  float   f_ElevDot;      /* Elevation rate [rad/s] */
  float   f_ElevUnc;      /* SV elevation [rad] uncertainty */
  double   d_VelCoef[12];  /* Coefficients of velocity poly */
} pdsm_lite_pd_ext_SvPoly_s_type;
typedef enum
{
  PDSM_LITE_INTERMEDIATE_POS_RPT,
  PDSM_LITE_FINAL_POS_RPT
}pdsm_lite_pos_type;
  
#define PDSM_NMEA_DATA_LENGTH 200

typedef struct
{
  pdsm_lite_pa_nmea_type nmea_type;
  uint16            nmea_length;
  byte              nmea_data[PDSM_NMEA_DATA_LENGTH];
} pdsm_lite_pd_nmea_s_type;

typedef union {
  pdsm_lite_pd_end_e_type                   end_status;        /* Session End status */
  qmiLocEventPositionReportIndMsgT_v02      pz_PositionReportIndMsg;/*Intermediate and final fix reports*/
  qmiLocGetBestAvailablePositionIndMsgT_v02 pz_BestAvailPosInd;     /*best available position report*/
  qmiLocGetAvailWwanPositionIndMsgT_v02     pz_AvailWwanPosInd;     /*Available WWAN position report*/
  pdsm_lite_pd_gps_state_s_type             pz_gps_state_info;      /*GPS state information*/
  qmiLocQueryGNSSEnergyConsumedIndMsgT_v02  z_GnssEnergyConsumedInd;
  pdsm_lite_pd_nmea_s_type                  nmea_pos_report_type;
 } pdsm_lite_pd_client_info_u_type;

/* When notified of PD events, clients are given a reference to this structure.
*/
typedef struct pdsm_lite_pd_info_s {

  pdsm_lite_client_id_type         client_id;   /* PDSM originating client */
  qword                     time_stamp;         /* time stamp */
  pdsm_lite_pd_client_info_u_type  pd_info;     /* Info based on Event set */
  qmiLocEventGnssSvInfoIndMsgT_v02 pz_SVInfoIndMsg;
} pdsm_lite_pd_info_s_type;

/* ------------------ EXTENDED POSITION STATUS EVENT DATA STRUCTURES ------------------------------*/


typedef uint64 pds_ext_dpo_disengage_reason_mask_type;
#define PDSM_LITE_DPO_DISENGAGE_REASON_SIGNAL_CONDITIONS        0x0000000000000001
#define PDSM_LITE_DPO_DISENGAGE_REASON_POSITION_ACCURACY        0x0000000000000002
#define PDSM_LITE_DPO_DISENGAGE_REASON_NAV_DATA_NEEDED          0x0000000000000004
#define PDSM_LITE_DPO_DISENGAGE_REASON_DPO_DISABLED             0x0000000000000008
#define PDSM_LITE_DPO_DISENGAGE_REASON_INSUFF_RESOURCES         0x0000000000000010
#define PDSM_LITE_DPO_DISENGAGE_REASON_UNSUPPORTED_MODE         0x0000000000000020
#define PDSM_LITE_DPO_DISENGAGE_REASON_UNCALIB_CLOCK            0x0000000000000040
#define PDSM_LITE_DPO_DISENGAGE_REASON_HEALTH_DATA_DECODE       0x0000000000000080
#define PDSM_LITE_DPO_DISENGAGE_REASON_CLOCK_ACCURACY           0x0000000000000100
#define PDSM_LITE_DPO_DISENGAGE_REASON_POSITION_NOT_COMPUTED    0x0000000000000200
#define PDSM_LITE_DPO_DISENGAGE_REASON_LTE_B13_TX               0x0000000000000400
#define PDSM_LITE_DPO_DISENGAGE_REASON_GSM_TX                   0x0000000000000800
#define PDSM_LITE_DPO_DISENGAGE_REASON_UNCALIB_XO               0x0000000000001000

typedef uint32 pds_ext_dpo_status_mask_type;
#define PDSM_LITE_DPO_STATUS_ENGAGED        0x00000001   /* DPO is engaged */
#define PDSM_LITE_DPO_STATUS_TOW_VALID      0x00000002   /* Timestamp of the status report in GPS TOW milliseconds is valid */

typedef locEngPositionReportMsgT_v01 pdsm_lite_pd_ext_pos_rpt_s_type;

typedef struct {
  pds_ext_dpo_status_mask_type dpo_status_mask;  /* Mask to provide information on the DPO status */
  dword    timestamp_tow_ms;   /* Timestamp of the status report in GPS TOW milliseconds. Valid if gps_tow_valid set */
  uint16   timestamp_gps_week; /* GPS week number of the position. valid if gps_tow_valid set */
  pds_ext_dpo_disengage_reason_mask_type dpo_disengage_reason;
} pdsm_lite_pd_ext_dpo_status_rpt;

typedef struct
{
  /* GNSS Engine Monitor Status */
  uint64 t_engine_monitor_status;
}pdsm_lite_pd_ext_engine_monitor_report_s_type;

typedef union
{
#if 0
  pdsm_lite_pd_ext_pos_rpt_s_type         ext_pos_report_qmi_type;
  pdsm_lite_pd_ext_SvPoly_s_type                        ext_sv_poly_type;
  pdsm_lite_pd_ext_gnss_meas_s_type       gnss_meas_report_type;
#endif
  pdsm_lite_pd_ext_engine_monitor_report_s_type ext_engine_monitor_report;
} pdsm_lite_ext_status_info_u_type;
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
typedef enum pdsm_lite_xtra_cmd_e {

  PDSM_LITE_XTRA_CMD_NONE = -1,                 /* FOR INTERNAL USE OF PDSM ONLY! */

  PDSM_LITE_XTRA_CMD_SET_DATA,                 /* gpsOneXTRA client is injecting XTRA data to PDAPI */
  PDSM_LITE_XTRA_CMD_INITIATE_DOWNLOAD,        /* gpsOneXTRA client is requesting XTRA download */
  PDSM_LITE_XTRA_CMD_SET_AUTO_DOWNLOAD_PARAMS, /* gpsOneXTRA client is setting download parameters */
  PDSM_LITE_XTRA_CMD_QUERY_DATA_VALIDITY,      /* gpsOneXTRA client is querying data validity */
  PDSM_LITE_XTRA_CMD_TIME_INFO_INJ,            /* gpsOneXTRA client is injecting SNTP time information */
  PDSM_LITE_XTRA_CMD_QUERY_XTRA_CFG_INFO,      /* gpsOneXTRA client is querying xtra config information */

  PDSM_LITE_XTRA_CMD_MAX,                /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_LITE_XTRA_CMD_E_SIZE = 0x10000000  /* To force enum to 32 bit for windows NT */
} pdsm_lite_xtra_cmd_e_type;
/*~ SENTINEL pdsm_lite_xtra_cmd_e_type.PDSM_LITE_XTRA_CMD_MAX */

/* Enumeration of gpsOneXTRA Client Status
*/
typedef enum pdsm_lite_xtra_dc_status_e {
  PDSM_LITE_XTRA_STATUS_FAILURE,
  PDSM_LITE_XTRA_STATUS_SUCCESS
} pdsm_lite_xtra_dc_status_e_type;

/* Enumeration of gpsOneXTRA Download status
*/

typedef enum pdsm_lite_xtra_status_e
{
  XTRA_DOWNLOAD_STATUS_SUCCESS,
  XTRA_DOWNLOAD_STATUS_BAD_CRC,
  XTRA_DOWNLOAD_STATUS_BAD_BUFFER_LENGTH,
  XTRA_DOWNLOAD_STATUS_TOA_BAD_RANGE,
  XTRA_DOWNLOAD_STATUS_FAILURE,
  XTRA_DOWNLOAD_STATUS_GPS_BUSY,
  XTRA_DOWNLOAD_STATUS_END_E911,
  XTRA_DOWNLOAD_STATUS_RETRY_EXCEEDED,      /* Ending due to maximum retries have been attempted */
  XTRA_DOWNLOAD_STATUS_DATA_TIMEOUT,        /* Ending due to timeout on waiting for data */
  XTRA_DOWNLOAD_STATUS_FILE_TIME_UNCHANGED,
  XTRA_DOWNLOAD_STATUS_VERSION_MISMATCH,    /* Ending due to XTRA Version mismatch */
  XTRA_DOWNLOAD_STATUS_GPS_LOCK,    /* GPS lock */
  XTRA_DOWNLOAD_STATUS_MAX = 0x10000000
} pdsm_lite_xtra_status_e_type;


/* Enumeration for gpsOneXTRA retrieve command
*/
typedef enum pdsm_lite_xtra_command_e
{
  XTRA_COMMAND_RETRIEVE_FILE,
  XTRA_COMMAND_RETRIEVE_TIME_INFO
} pdsm_lite_xtra_command_e_type;
/* Events for gpsOneXTRA 
*/
typedef int pdsm_lite_xtra_event_type;
#define PDSM_LITE_XTRA_EVENT_NONE               0x00
#define PDSM_LITE_XTRA_EVENT_DOWNLOAD_REQ       0x01
#define PDSM_LITE_XTRA_EVENT_STATUS             0x02
#define PDSM_LITE_XTRA_EVENT_TIME_REQ           0x04
#define PDSM_LITE_XTRA_EVENT_CFG_REQ            0x08

#define PDSM_LITE_XTRA_EVENT_MASK   (   (PDSM_LITE_XTRA_EVENT_DOWNLOAD_REQ) \
                                 | (PDSM_LITE_XTRA_EVENT_STATUS) \
                                 | (PDSM_LITE_XTRA_EVENT_TIME_REQ) \
                                 | (PDSM_LITE_XTRA_EVENT_CFG_REQ) \
                                )

/* Download info field validiy bit flags
*/
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
typedef struct pdsm_lite_xtra_download_info_s
{
  uint32                   flags;        // indicates which fields are valid
  pdsm_lite_xtra_status_e_type  status;       // enum: success, bad crc, xtra data file too old
  pdsm_lite_xtra_command_e_type command;      // enum: retrieve xtra data file from internet
  uint16                   startGpsWeek; // current XTRA info is good starting this GPS week
  uint16                   startGpsMinutes; // current XTRA information is good starting
                                            // this many minutes into the week
  uint16   validDurationHours;           // XTRA info is valid for this many hours
  char     xtra_server_primary[128];     // URL of primary XTRA server
  char     xtra_server_secondary[128];   // URL of secondary (backup) XTRA server
  char     xtra_server_tertiary[128];    // URL of tertiary (backup) XTRA server
  uint32   maxFilePartSize;              // due to RPC limitations, the XTRA file can be xferred
                                         // through PDAPI in parts if the file is too large
  uint32   maxFileSize;                  // max total size of XTRA file
  uint32   value;                        //indicates which constellations were accepted
} pdsm_lite_xtra_download_info_s_type;

/* Structure used as payload for gpsOneXTRA events 
** 1. PDSM_LITE_XTRA_EVENT_TIME_REQ
*/
typedef struct pdsm_lite_xtra_time_info_req_s
{
  pdsm_lite_xtra_command_e_type command;    // enum: retrieve SNTP time information from internet
  uint32 oneway_delay_failover_thresh; // one-way delay threshold for failover to backup servers
  char xtra_server_primary[128];         // URL of primary XTRA server
  char xtra_server_secondary[128];     // URL of secondary (backup) XTRA server
  char xtra_server_tertiary[128];      // URL of tertiary (backup) XTRA server
} pdsm_lite_xtra_time_info_req_s_type;

#define XTRA_CFG_INFO_MASK_ABS_AGE         (0x00000001)   // How many hours that the current XTRA information is valid
#define XTRA_CFG_INFO_MASK_REL_AGE         (0x00000002)   //Last XTRA data download time
#define XTRA_CFG_INFO_MASK_XTRA_SERVER     (0x00000004)   // XTRA server URLs
#define XTRA_CFG_INFO_MASK_NTP_SERVER      (0x00000008)   //Network Time Protocol(NTP) server URLs
#define XTRA_CFG_INFO_MASK_TIME_REQUEST    (0x00000010)   //Time requested
#define XTRA_CFG_INFO_MASK_PREF_VALID_AGE  (0x00000020)   // Prefered valid age for the xtra file


typedef struct pdsm_lite_xtra_config_ind_info_s
{
  pdsm_lite_xtra_status_e_type  status;
  uint32   report_mask;

  uint16   prefered_valid_age_hrs;
  uint32   abs_age_hrs;
  uint64   rel_age_utc;

  char     xtra_time_server_primary[128];     // URL of primary sntp server
  char     xtra_time_server_secondary[128];   // URL of secondary (backup) sntp server
  char     xtra_time_server_tertiary[128];    // URL of tertiary (backup) sntp server

  char     xtra_server_primary[128];     // URL of primary XTRA server
  char     xtra_server_secondary[128];   // URL of secondary (backup) XTRA server
  char     xtra_server_tertiary[128];    // URL of tertiary (backup) XTRA server

  uint8    time_request; /** 0 : if time is not requested 1 : if time is requested **/

}pdsm_lite_xtra_config_info_s_type;

/* Union of the gpsOneXTRA download or time data
*/
typedef union pdsm_lite_xtra_req_u
{
  pdsm_lite_xtra_download_info_s_type  xtra_download_info;
  pdsm_lite_xtra_time_info_req_s_type  xtra_time_info;
  pdsm_lite_xtra_config_info_s_type    xtra_cfg_info;
} pdsm_lite_xtra_req_u_type;
/*~ IF ((_DISC_ == PDSM_LITE_XTRA_EVENT_DOWNLOAD_REQ) || (_DISC_ == PDSM_LITE_XTRA_EVENT_STATUS)) pdsm_lite_xtra_req_u.xtra_download_info */
/*~ IF (_DISC_ == PDSM_LITE_XTRA_EVENT_TIME_REQ)     pdsm_lite_xtra_req_u.xtra_time_info */
/*~ DEFAULT pdsm_lite_xtra_req_u.void */


/* Structure for gpsOneXTRA events and requests
*/
typedef struct pdsm_lite_xtra_info_s
{
  pdsm_lite_client_id_type   client_id;
  pdsm_lite_xtra_event_type  xtra_event;
  pdsm_lite_xtra_req_u_type  xtra_union;
} pdsm_lite_xtra_info_s_type;
/*~ FIELD pdsm_lite_xtra_info_s.xtra_union DISC pdsm_lite_xtra_info_s.xtra_event */


typedef struct pdsm_lite_xtra_time_info
{
  /* cummulative GPS time (reference date: Jan 6, 1980) (msec) */
  uint64  d_TimeMsec;
  uint32  f_TimeUncMsec;    /* Assoc. time uncertainty */
  boolean b_RefToUtcTime;   /* Referenced to UTC/GPS time*/
  boolean b_ForceFlag;      /* Force acceptance of data */
} pdsm_lite_xtra_time_info_type;

typedef struct pdsm_lite_gtp_wwan_info
{
  uint8  u_TimeOutSec;    /*time out value for the session*/
} pdsm_lite_gtp_wwan_info_type;

typedef struct pdsm_lite_xtra_config_req_info
{
  uint32     config_mask;
} pdsm_lite_xtra_config_req_info_type;


/*-------------- End eXtended Receiver Assistance (XTRA) ------------------*/

/*-------------- eXtended Receiver Assistance Terrestrial (XTRA-T) ------------------*/
/* XTRA-T Command Type */
typedef enum pdsm_lite_xtra_t_cmd_e
{
  PDSM_LITE_XTRA_T_CMD_NONE = -1,          /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_LITE_XTRA_T_CMD_GET_DATA,           /* XTRA-T Client is getting XTRA-T data from PDAPI */
  PDSM_LITE_XTRA_T_CMD_SET_DATA,           /* XTRA-T Client is injecting XTRA-T data to PDAPI */
  PDSM_LITE_XTRA_T_CMD_INITIATE_DOWNLOAD,  /* Requesting XTRA-T client to download XTRA-T data */
  PDSM_LITE_XTRA_T_CMD_INITIATE_UPLOAD,    /* Requesting XTRA-T client to upload XTRA-T data */

  PDSM_LITE_XTRA_T_CMD_MAX,                /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_LITE_XTRA_T_CMD_E_SIZE = 0x10000000   /* To force enum to 32 bit for windows NT */
} pdsm_lite_xtra_t_cmd_e_type;
/*~ SENTINEL pdsm_lite_xtra_t_cmd_e_type.PDSM_LITE_XTRA_T_CMD_MAX */


/* XTRA-T Event Mask */
typedef int pdsm_lite_xtra_t_event_type;
#define PDSM_LITE_XTRA_T_EVENT_NONE                          0x00
#define PDSM_LITE_XTRA_T_EVENT_SESS_BEGIN                    0x01  /*  GPS engine initiates  XTRA-T */
#define PDSM_LITE_XTRA_T_EVENT_DATA                          0x02  /*  GPS engine XTRA-T data transfer */
#define PDSM_LITE_XTRA_T_EVENT_SESS_DONE                     0x04  /*  GPS engine requests a termination of XTRA-T session */
#define PDSM_LITE_XTRA_T_EVENT_SESS_END                      0x08  /*  GPS engine XTRA-T  session abort due to errors */
#define PDSM_LITE_XTRA_T_EVENT_DOWNLOAD_REQ                  0x10  /* GPS engine initiates XTRA-T download  - Aries 5.0 */
#define PDSM_LITE_XTRA_T_EVENT_UPLOAD_REQ                    0x20  /* GPS engine initiates XTRA-T upload  - Aries 5.0 */

#define PDSM_LITE_XTRA_T_EVENT_MASK (PDSM_LITE_XTRA_T_EVENT_SESS_BEGIN | PDSM_LITE_XTRA_T_EVENT_DATA | PDSM_LITE_XTRA_T_EVENT_SESS_DONE \
                                 | PDSM_LITE_XTRA_T_EVENT_SESS_END | PDSM_LITE_XTRA_T_EVENT_DOWNLOAD_REQ | PDSM_LITE_XTRA_T_EVENT_UPLOAD_REQ)

/* XTRA-T payload for PDSM_LITE_XTRA_T_EVENT_SESS_END: data part report to XTRA-T client */
typedef struct pdsm_lite_xtra_t_get_celldb_report_s
{
  uint32           xtra_t_data_len;  /* Length of XTRA-T data */
  unsigned char   *xtra_t_data_ptr;  /* XTRA-T Data Transfer Report or DATA_TRANSFER_REPORT */
  uint8            part_number;      /* Part number n of "total_parts" being sent by the application */
  uint8            total_parts;      /* Total number of parts that the cell database data was broken into for the report */
} pdsm_lite_xtra_t_get_celldb_report_s_type;
/*~ FIELD pdsm_lite_xtra_t_get_celldb_report_s.xtra_t_data_ptr POINTER */


/* XTRA-T payload for PDSM_LITE_XTRA_T_EVENT_DATA: XTRA-T session end reason */
/* This is also used for sending status of individual Segmented CellDB message to TM XTRA-T Module */
typedef enum pdsm_lite_xtra_t_status_e
{
  PDSM_LITE_XTRA_T_END_NONE    = -1,
  PDSM_LITE_XTRA_T_END_SUCCESS = 0,       /* XTRA-T session success */
  PDSM_LITE_XTRA_T_END_BUSY,
  PDSM_LITE_XTRA_T_END_OTHER,
  PDSM_LITE_XTRA_T_END_NO_CLIENT,
  PDSM_LITE_XTRA_T_END_FEATURE_NOT_ENABLED,
  PDSM_LITE_XTRA_T_END_BAD_BUFFER_LENGTH,
  PDSM_LITE_XTRA_T_END_INTERNAL_RESOURCE,
  PDSM_LITE_XTRA_T_END_CLIENT_REPORT_ERR,
  PDSM_LITE_XTRA_T_END_DTR_PROC_ERR,
  PDSM_LITE_XTRA_T_END_MAX,               /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_LITE_XTRA_T_END_E_SIZE = 0x10000000  /* To force enum to 32 bit for windows NT */
} pdsm_lite_xtra_t_status_e_type;


/*
** TM XTRA-T Session Type
*/
typedef enum {
  PDSM_LITE_XTRA_T_SESSION_NONE       = 0,  /* Invalid session type                             */
  PDSM_LITE_XTRA_T_SESSION_UPLOAD_DTR,      /* Upload Session using Cell DB data in DTR packets */
  PDSM_LITE_XTRA_T_SESSION_MAX,
  PDSM_LITE_XTRA_T_SESSION_E_SIZE     = 0x10   /* Force to 1 byte */
} pdsm_lite_xtra_t_session_e_type;


/* XTRA-T Event Callback: payload definition */
typedef union pdsm_lite_xtra_t_req_u
{
  /* event: PDSM_LITE_XTRA_T_EVENT_DATA  */
  /* payload: data part report to XTRA-T client */
  pdsm_lite_xtra_t_get_celldb_report_s_type             data_report;

  /* event:  PDSM_LITE_XTRA_T_EVENT_SESS_END */
  /* payload: XTRA-T session end reason */
  pdsm_lite_xtra_t_status_e_type                        end_reason;
} pdsm_lite_xtra_t_req_u_type;
/*~ IF (_DISC_ == PDSM_LITE_XTRA_T_EVENT_DATA) pdsm_lite_xtra_t_req_u.data_report */
/*~ IF (_DISC_ == PDSM_LITE_XTRA_T_EVENT_SESS_END)     pdsm_lite_xtra_t_req_u.end_reason */
/*~ DEFAULT pdsm_lite_xtra_t_req_u.void */

/* XTRA-T Event Callback Data: event + payload */
typedef struct pdsm_lite_xtra_t_info_s
{
  pdsm_lite_client_id_type        client_id;
  pdsm_lite_xtra_t_event_type     xtra_t_event;           /* event in the callback */
  pdsm_lite_xtra_t_req_u_type     xtra_t_event_payload;   /* event payload in the callback */
} pdsm_lite_xtra_t_info_s_type;
/*~ FIELD pdsm_lite_xtra_t_info_s.xtra_t_event_payload DISC pdsm_lite_xtra_t_info_s.xtra_t_event */
/* XTRA-T:  pdsm_lite_xtra_t_get_data() related */
typedef enum
{
  PDSM_LITE_XTRA_T_GET_ALL,                  /* All cells in CellDB */
  PDSM_LITE_XTRA_T_GET_ALL_CHANGED,          /* All changed cells in CellDB (changed = dirty bit is ON) */
  PDSM_LITE_XTRA_T_GET_CHANGED_IN_N_MINUTES, /* Changed cells in CellDB for the last N minutes */
  PDSM_LITE_XTRA_T_GET_LAST_N_CHANGED,      /* The last N changed cells in CellDB  */
  PDSM_LITE_XTRA_T_GET_MAX,                  /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_LITE_XTRA_T_GET_E_SIZE = 0x10000000    /* To force enum to 32 bit for windows NT */
} pdsm_lite_xtra_t_get_e_type;

/* XTRA-T:  pdsm_lite_xtra_t_get_data() related */
typedef struct
{
  uint32                    changed_info_type;   /* Used for passing the value of "N" in these get requests:
                                                    PDMS_XTRA_T_GET_CHANGED_IN_N_MINUTES
                                                    PDSM_LITE_XTRAT_T_GET_LAST_N_CHANGED 
                                                    N > 0
                                                 */
  pdsm_lite_xtra_t_get_e_type    get_type;
} pdsm_lite_xtra_t_get_s_type;
/*-------------- End eXtended Receiver Assistance Terrestrial (XTRA-T) ------------------*/

/*-------------- Start (WIPER) ------------------*/

/*-------------- End (WIPER) ------------------*/

/*---------------------------------------------------------------------------
              QMI Services Enable Type
---------------------------------------------------------------------------*/

typedef uint32 pdsm_lite_QMIServiceEnableMaskType;
/* definitions for QMI service configuration */
#define PDSM_LITE_QMI_CONFIG_ENABLE_LOC           (0x00000001)

typedef struct
{
  pdsm_lite_QMIServiceEnableMaskType enableMask;
} pdsm_lite_QMIServiceConfig;

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

typedef void (pdsm_lite_pd_event_f_type)(
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

typedef void (pdsm_lite_pa_event_f_type)(
   void                         *user_data,  /* user data pointer */

   pdsm_lite_pa_event_type           pa_event,    /* Indicate PD event */

   const pdsm_lite_pa_info_s_type    *pa_info_ptr /* A pointer to a PD state information struct */
   /*~ PARAM pa_info_ptr POINTER */

   );

/* After PA event max-out, this is the new PA event-reporting function, no need to define new PA bitmask-style
*  event for each new PDSM_LITE_PA_xxx enum, the enum will be used here directly. client will not be able to register
*  certain events selectively, all PA events will be reported to registered PA client
*/
typedef void (pdsm_lite_pa_event_ex_f_type)(
   void                         *user_data,  /* user data pointer */

   pdsm_lite_pa_e_type               pa_event,    /* Indicate PA event, PDSM_LITE_PA_xxx enum */

   const pdsm_lite_pa_info_s_type    *pa_info_ptr, /* A pointer to a PD state information struct */
   /*~ PARAM pa_info_ptr POINTER */

   pdsm_lite_pa_cmd_err_e_type        status    /* Optional field to indicate success/failure status of PA operation */
   );



typedef void (pdsm_lite_xtra_event_f_type)(
   void *user_data,
   pdsm_lite_xtra_event_type  xtra_event,
   const pdsm_lite_xtra_info_s_type *xtra_info_ptr
   /*~ PARAM xtra_info_ptr POINTER */
   );


/* Type for XTRA-T event callback function */
typedef void (pdsm_lite_xtra_t_event_f_type)(
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
typedef void (pdsm_lite_pd_cmd_err_f_type)(

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
typedef void (pdsm_lite_pa_cmd_err_f_type)(

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
typedef void (pdsm_lite_ext_status_cmd_err_f_type)(

   pdsm_lite_client_id_type         client_id
   /* Indicate which client caused the error */

   );

typedef void (pdsm_lite_xtra_cmd_err_f_type)(

   pdsm_lite_client_id_type         client_id,
   /* Indicate which client caused the error */

   pdsm_lite_xtra_cmd_e_type             cmd,
   /* Indicate for which command this error is reported */

   pdsm_lite_xtra_cmd_err_e_type         cmd_err
   /* Indicate nature of error */
   );


typedef void (pdsm_lite_xtra_t_cmd_err_f_type)(

   pdsm_lite_client_id_type         client_id,
   /* Indicate which client caused the error */

   pdsm_lite_xtra_t_cmd_e_type             cmd,
   /* Indicate for which command this error is reported */

   pdsm_lite_xtra_t_cmd_err_e_type         cmd_err
   /* Indicate nature of error */
   );

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of client API return status*/

typedef enum pdsm_lite_client_status_e {

  PDSM_LITE_CLIENT_ERR_NONE = -1,          /* FOR INTERNAL USE OF PDSM ONLY! */

  PDSM_LITE_CLIENT_OK,                   /* No error, request was completed successfully */

  PDSM_LITE_CLIENT_ERR_CLIENT_PTR,        /* Invalid client ID pointer*/

  PDSM_LITE_CLIENT_ERR_CLIENT_ID,        /* Invalid client ID */

  PDSM_LITE_CLIENT_ERR_CLIENT_TYPE,      /* Invalid Client type */

  PDSM_LITE_CLIENT_ERR_EVENT_REG_TYPE,   /* Invalid event registration type */

  PDSM_LITE_CLIENT_ERR_EVENT_MASK,       /* Invalid event mask */

  PDSM_LITE_CLIENT_ERR_CLIENT_INIT_FAIL, /* Unable to initialize the client */

  PDSM_LITE_CLIENT_ERR_SESSION_ACTIVE,   /* Unable to release a client due to active session */

  PDSM_LITE_CLIENT_ERR_CLIENT_ACTIVE,    /* Unable to release an active client */

  PDSM_LITE_CLIENT_ERR_MAX,              /* FOR INTERNAL USE OF PDSM ONLY! */

  PDSM_LITE_CLIENT_ERR_E_SIZE = 0x10000000    /* To force enum to 32 bit for windows NT */

} pdsm_lite_client_status_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of client event registration type */

typedef enum pdsm_lite_client_event_reg_e {

  PDSM_LITE_CLIENT_EVENT_NONE = -1,          /* FOR INTERNAL USE OF PDSM ONLY! */

  PDSM_LITE_CLIENT_EVENT_REG,              /* event registration */

  PDSM_LITE_CLIENT_EVENT_DEREG,            /* event de-registration */

  PDSM_LITE_CLIENT_EVENT_MAX,              /* FOR INTERNAL USE OF PDSM ONLY! */

  PDSM_LITE_CLIENT_EVENT_BIG = 0x10000000    /* To force enum to int */

} pdsm_lite_client_event_reg_e_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*
    This structure contains the options for and end request. 

    If the receiver off is set to false, then if no session is 
    running and receiver is ON due to extended ON feature or due 
    to LM apptracking feature, the receiver wont be turned off. 

    If the boolean was set, it will result in sending receiver off
    to MGP.
*/
typedef struct {
  boolean receiver_off; /*Turn the receiver off*/
} pdsm_lite_pd_end_session_opt_s_type;

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
extern pdsm_lite_client_id_type pdsm_lite_client_init(

   pdsm_lite_client_type_e_type           client_type
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
extern pdsm_lite_client_status_e_type pdsm_lite_client_release(

   pdsm_lite_client_id_type               client_id
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
pdsm_lite_client_status_e_type pdsm_lite_client_pd_reg(

   pdsm_lite_client_id_type           client_id,
   /* Requesting client */
   void                          *client_data_ptr,

   pdsm_lite_pd_event_f_type          *pd_event_func,
   /* Pointer to a callback function to notify the client of PD
                                             ** events */
   /*~ PARAM   pd_event_func POINTER */

   pdsm_lite_client_event_reg_e_type  event_reg_type,

   pdsm_lite_pd_event_type            event_mask,
   /* register from this event (including) */

   pdsm_lite_pd_cmd_err_f_type        *pd_cmd_err_func
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
pdsm_lite_client_status_e_type pdsm_lite_client_pa_reg(

   pdsm_lite_client_id_type              client_id,
   /* Requesting client */
   void                             *client_data_ptr,

   pdsm_lite_pa_event_f_type             *pa_event_func,
   /* Pointer to a callback function to notify the client of PA
                                             ** events */
   /*~ PARAM pa_event_func POINTER */
   pdsm_lite_client_event_reg_e_type     event_reg_type,

   pdsm_lite_pa_event_type               event_mask,
   /* register from this event (including) */

   pdsm_lite_pa_cmd_err_f_type           *pa_cmd_err_func
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
extern pdsm_lite_client_status_e_type pdsm_lite_client_xtra_reg
(
   pdsm_lite_client_id_type           client_id,
   /* Requesting client */
   void                          *client_data_ptr,

   pdsm_lite_xtra_event_f_type          *xtra_event_func,
   /* Pointer to a callback function to notify the client of XTRA events */
   /*~ PARAM xtra_event_func POINTER */
   pdsm_lite_client_event_reg_e_type  event_reg_type,

   pdsm_lite_xtra_event_type            event_mask,
   /* register from this event (including) */

   pdsm_lite_xtra_cmd_err_f_type        *xtra_cmd_err_func
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
pdsm_lite_client_status_e_type pdsm_lite_client_pa_reg_ex(

    pdsm_lite_client_id_type              client_id,
        /* Requesting client */
    void                             *client_data_ptr,

    pdsm_lite_pa_event_ex_f_type          *pa_event_func_ex,
        /* Pointer to a callback function to notify the client of PA
        ** events */
    /*~ PARAM pa_event_func_ex POINTER */
    pdsm_lite_client_event_reg_e_type     event_reg_type,

    pdsm_lite_pa_cmd_err_f_type           *pa_cmd_err_func
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
extern pdsm_lite_client_status_e_type pdsm_lite_client_xtra_t_reg
(
   pdsm_lite_client_id_type           client_id,
   /* Requesting client */
   void                          *client_data_ptr,

   pdsm_lite_xtra_t_event_f_type          *xtra_t_event_func,
   /* Pointer to a callback function to notify the client of PD
 ** events */
   /*~ PARAM xtra_t_event_func POINTER */

   pdsm_lite_client_event_reg_e_type  event_reg_type,
   /* Register or deregister events */

   pdsm_lite_xtra_t_event_type            xtra_t_event_mask,
   /* register from this event (including) */

   pdsm_lite_xtra_t_cmd_err_f_type        *xtra_t_cmd_err_func
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
extern pdsm_lite_client_status_e_type pdsm_lite_client_act(

   pdsm_lite_client_id_type               client_id
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
extern pdsm_lite_client_status_e_type pdsm_lite_client_deact(

   pdsm_lite_client_id_type               client_id
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
typedef void (pdsm_lite_pd_cmd_cb_f_type)(

   void                      *data_block_ptr,
   /* Pointer to client provided data block.
                                     **
                                     ** NOTE! Depending on the client implementation of this function
                                     ** the data block pointed by this pointer might be accessed from
                                     ** client, as well as, PDSM context simultaneously */

   pdsm_lite_pd_cmd_e_type        pd_cmd,
   /* Indicate which command is this error status for */

   pdsm_lite_pd_cmd_err_e_type    pd_cmd_err
   /* Command error code, indicating whether command
                                     ** is rejected and the reason */
   );


/* Type for PA command callback functions.
**
** A client that wants to be notified of the result of a  PA command
** needs to supply a pointer to a function of this type as the first
** parameter to a PA-command-function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, PDSM context simultaneously.
*/
typedef void (pdsm_lite_pa_cmd_cb_f_type)(

   void                      *data_block_ptr,
   /* Pointer to client provided data block.
                                     **
                                     ** NOTE! Depending on the client implementation of this function
                                     ** the data block pointed by this pointer might be accessed from
                                     ** client, as well as, PDSM context simultaneously */

   pdsm_lite_pa_cmd_e_type        pa_cmd,
   /* Indicate which command is this error status for */

   pdsm_lite_pa_cmd_err_e_type    pa_cmd_err
   /* Command error code, indicating whether command
                                     ** is rejected and the reason */
   );

typedef void (pdsm_lite_ext_pos_cmd_cb_f_type)(

   void                      *data_block_ptr
   /* Pointer to client provided data block.
                                          **
                                          ** NOTE! Depending on the client implementation of this function
                                          ** the data block pointed by this pointer might be accessed from
                                          ** client, as well as, PDSM context simultaneously */
   );



/* Type for XTRA command callback functions.
**
** A client that wants to be notified of the result of an XTRA command
** needs to supply a pointer to a function of this type as the first
** parameter to a XTRA-command-function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, PDSM context simultaneously.
*/
typedef void (pdsm_lite_xtra_cmd_cb_f_type)(

   void                      *data_block_ptr,
   /* Pointer to client provided data block.
                                       **
                                       ** NOTE! Depending on the client implementation of this function
                                       ** the data block pointed by this pointer might be accessed from
                                       ** client, as well as, PDSM context simultaneously */

   pdsm_lite_xtra_cmd_e_type        xtra_cmd,
   /* Indicate which command is this error status for */

   pdsm_lite_xtra_cmd_err_e_type    xtra_cmd_err
   /* Command error code, indicating whether command
                                       ** is rejected and the reason */
   );

/* Type for  XTRA-T Command Callback Function
**
** A client that wants to be notified of the result of an XTRA-T command
** needs to supply a pointer to a function of this type as the first
** parameter to a XTRA-T-command-function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, PDSM context simultaneously.
*/
typedef void (pdsm_lite_xtra_t_cmd_cb_f_type)
   (
   void                         *data_block_ptr,
   /* Pointer to client provided data block.
    **
    ** NOTE! Depending on the client implementation of this function
    ** the data block pointed by this pointer might be accessed from
    ** client, as well as, PDSM context simultaneously */

   pdsm_lite_xtra_t_cmd_e_type        xtra_t_cmd,
   /* Indicate which command is this error status for */

   pdsm_lite_xtra_t_cmd_err_e_type    xtra_t_cmd_err
   /* Command error code, indicating whether command is rejected and the reason */
   );

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
extern boolean pdsm_lite_get_position(
   pdsm_lite_pd_cmd_cb_f_type    pd_cb_func,         /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,   /* user data pointer */
   pdsm_lite_pd_option_s_type    *option_ptr,      /* Fix rate for tracking sessions. */
   /*~ PARAM option_ptr POINTER */
   pdsm_lite_pd_qos_type         *qos_ptr,           /* Ptr to user QOS for this request */
   /*~ PARAM qos_ptr POINTER */
   pdsm_lite_client_id_type      client_id           /* Client id of user */
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

extern boolean pdsm_lite_end_session(
   pdsm_lite_pd_cmd_cb_f_type        pd_cb_func,        /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                         *client_data_ptr,  /* Client specified ptr */
   pdsm_lite_pd_end_session_opt_s_type *end_session_opt,
   pdsm_lite_client_id_type          client_id          /* requesting client ID */
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
   pdsm_lite_pd_cmd_cb_f_type   cmd_cb_func, /* command result callback function */
   /*~ PARAM pa_motion_data_cb_func POINTER */
   pdsm_lite_client_id_type   client_id,
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
boolean pdsm_lite_pd_get_avail_wwan_pos
(
  pdsm_lite_pd_cmd_cb_f_type   cmd_cb_func, /* command result callback function */
  /*~ PARAM pa_motion_data_cb_func POINTER */
  pdsm_lite_client_id_type     client_id,
  void                   *client_data_ptr /* client provided reference data pointer */
);

/*===========================================================================

FUNCTION pdsm_lite_pd_get_avail_wwan_pos_ex

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
boolean pdsm_lite_pd_get_avail_wwan_pos_ex
(
  pdsm_lite_pd_cmd_cb_f_type   cmd_cb_func, /* command result callback function */
  /*~ PARAM pa_motion_data_cb_func POINTER */
  pdsm_lite_client_id_type     client_id,
  void                   *client_data_ptr,  /* client provided reference data pointer */
  pdsm_lite_gtp_wwan_info_type *info_ptr

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
extern boolean pdsm_lite_pd_inject_external_time(
   pdsm_lite_pd_cmd_cb_f_type    pd_cb_func,         /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,   /* user data pointer */
   pdsm_lite_pd_external_time_info_type    *ext_time_ptr,        /* external time */
   /*~ PARAM ext_time_ptr POINTER */
   pdsm_lite_client_id_type      client_id           /* Client id of user */
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
extern boolean pdsm_lite_pd_inject_external_position(
   pdsm_lite_pd_cmd_cb_f_type    cmd_cb_func,           /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,      /* user data pointer */
   pdsm_lite_pd_external_position_info_type  *ext_pos_ptr, /* external position */
   /*~ PARAM ext_pos_ptr POINTER */
   pdsm_lite_client_id_type      client_id              /* Client id of user */
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
extern boolean pdsm_lite_pd_query_gps_state(
   pdsm_lite_pd_cmd_cb_f_type     cmd_cb_func,       /* Command callback */
   /*~ PARAM cmd_cb_func POINTER */
   void                      *client_data_ptr,  /* Not sure if this is required */
   /*~ PARAM gps_state_ptr POINTER */
   pdsm_lite_client_id_type       client_id          /* Requesting client id */
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

extern boolean pdsm_lite_set_parameters(
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

boolean pdsm_lite_get_parameters
(
   pdsm_lite_pa_cmd_cb_f_type    cmd_cb_func,        /* Command callback fn */
   /*~ PARAM cmd_cb_fund POINTER */
   void                     *client_data_ptr,   /* Client provided data pointer */
   pdsm_lite_pa_e_type           pa_get,             /* Type of parameter to Get */
   pdsm_lite_client_id_type      client_id
   );
/*~ FUNCTION pdsm_lite_get_parameters */

/*===========================================================================

FUNCTION pdsm_lite_get_pdapi_version

DESCRIPTION

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
uint32 pdsm_lite_get_pdapi_version(void);
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
extern boolean pdsm_lite_xtra_set_data(
   pdsm_lite_xtra_cmd_cb_f_type    xtra_cb_func,       /* Command callback */
   /*~ PARAM xtra_cb_func POINTER */
   pdsm_lite_client_id_type        client_id,          /* Client id of user */
   void                       *client_data_ptr,   /* user data pointer */
   uint32                     xtra_data_len,      /* Length of XTRA data */
   unsigned char             *xtra_data_ptr,     /* XTRA data */
   uint8                      part_number,        /* data part number */
   uint8                      total_parts,        /* total data parts */

   pdsm_lite_xtra_dc_status_e_type xtra_dc_status      /* status of download */
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
   pdsm_lite_xtra_cmd_cb_f_type    xtra_cb_func,      /* Command callback */
   /*~ PARAM xtra_cb_func POINTER */
   pdsm_lite_client_id_type        client_id,         /* Client id of user */
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
extern boolean pdsm_lite_xtra_query_config_info
(
   pdsm_lite_xtra_cmd_cb_f_type  xtra_cb_func,       /* Command callback */
   pdsm_lite_client_id_type      client_id,          /* Client id of user */
   void                     *client_data_ptr,    /* user data pointer */
   pdsm_lite_xtra_config_req_info_type *pXtraCfgReqInfo
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
extern boolean pdsm_lite_xtra_set_auto_download_params(
   pdsm_lite_xtra_cmd_cb_f_type    xtra_cb_func,         /* Command callback */
   /*~ PARAM xtra_cb_func POINTER */
   pdsm_lite_client_id_type        client_id,            /* Client id of user */
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
   pdsm_lite_xtra_cmd_cb_f_type    xtra_cb_func,       /* Command callback */
   /*~ PARAM xtra_cb_func POINTER */
   pdsm_lite_client_id_type        client_id,          /* Client id of user */
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
extern boolean pdsm_lite_xtra_inject_time_info(
   pdsm_lite_xtra_cmd_cb_f_type  xtra_cb_func,       /* Client command callback funct. */
   /*~ PARAM xtra_cb_func POINTER */
   pdsm_lite_client_id_type      client_id,          /* Client id of user */
   void                     *client_data_ptr,   /* Pointer to client data block */
   pdsm_lite_xtra_time_info_type *time_info_ptr      /* Pointer to time information data */
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
extern boolean pdsm_lite_xtra_t_get_data
(
   pdsm_lite_xtra_t_cmd_cb_f_type   xtra_t_cb_func,      /* Command callback */
   void                       *client_data_ptr,     /* Not sure if this is required */
   /*~ PARAM client_data_ptr POINTER */
   pdsm_lite_xtra_t_get_s_type      xtra_t_get_type,     /* Parameter mask to be set */
   pdsm_lite_client_id_type         client_id            /* Requesting client id */
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
boolean pdsm_lite_enable_dpo_status_report(
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
boolean pdsm_lite_read_qmi_services_config(pdsm_lite_QMIServiceConfig *qmi_Config);


/* Define different types of cell database */


typedef enum
{
  CELL_DB_NONE = 0,
  CELL_DB_MAIN_MODEM,
  CELL_DB_EXTERNAL_MODEM
}cell_db_type;

/* Enumeration of the Cell Service Status */
typedef enum
{
  STATUS_UNKNOWN = 0,
  STATUS_OUT_OF_SERVICE,
  STATUS_CAMPED_NOT_ROAMING,
  STATUS_CAMPED_ROAMING
} pdsm_lite_cell_service_status_e_type;


/* GSM Cell Information  (activeRAT is CELLDB_ACTIVERAT_GSM)
   The fields should be all valid when activeRAT is GSM */
typedef struct
{
  uint32  MCC;      // Mobile Country Code
  uint32  MNC;      // Mobile Network Code
  uint32  LAC;      // Location Area Code
  uint32  CID;      // Cell Identification
} pdsm_lite_gsm_cell_info_s_type;

typedef enum
{
  PDSM_LITE_ACTIVERAT_NONE,
  PDSM_LITE_ACTIVERAT_GSM,
  PDSM_LITE_ACTIVERAT_LTE,
  PDSM_LITE_ACTIVERAT_MAX
} pdsm_lite_active_RAT_e_type;

typedef union
{
  /* activeRAT = CELLDB_ACTIVERAT_GSM */
  pdsm_lite_gsm_cell_info_s_type gsmCell;

  /* Can add other RATs (WCDMA, CDMA, TD-SCDMA, LTE) as needed */
}pdsm_lite_cell_info_u_type;

/* Cell Inforamtion */
typedef struct
{
  /* service status: if it's UNKNOWN or OOS, activeRAT should be set to
     CELLDB_ACTIVERAT_NONE to indicate that there is no active RAT and
     cellInfo is invalid. When service status is CAMPED_ROAMING or
     CAMPED_NOT_ROAMING, activeRAT should be set to the current active
     RAT */
  pdsm_lite_cell_service_status_e_type serviceStatus; // service status
    /* cell info union, currently only GSM RAT is supported */
  pdsm_lite_cell_info_u_type   cellInfo;
  pdsm_lite_active_RAT_e_type  activeRAT;
} pdsm_lite_cell_info_s_type;


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
extern boolean pdsm_lite_pd_inject_external_cell_info(
   pdsm_lite_pd_cmd_cb_f_type    pd_cb_func,         /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,    /* user data pointer */
   pdsm_lite_client_id_type      client_id,           /* Client id of user */
   pdsm_lite_cell_info_s_type    *cell_info           /* Pointer to external cell info */
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
extern pdsm_lite_gps_lock_e_type pdsm_lite_get_gps_lock_status(void);
/*===========================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* PDAPI_LITE_H */

