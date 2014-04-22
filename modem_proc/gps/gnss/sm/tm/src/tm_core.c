/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM Core

GENERAL DESCRIPTION
  This module makes up the TM-Core module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2021 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/src/tm_core.c#12 $
  $DateTime: 2021/08/13 05:09:56 $
  $Author: pwbldsvc $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/21   sai     Expanding TUNC/FUNC NV 74194 to bypass FUNC explicitly
01/27/21   sai     Added NV for Controlling Fix related operations, currently controls cache fix propogation and its reporting to client
01/08/20   sai     Added API to query SM if GTP 2.0 fix is possible or not
01/08/20   sai     Workaround to handle Random MGP RCVR ACK OFF sent by MGP in ON state
09/01/20   sai     Changes done to support Geofence GTP Enhancement requirements
08/11/20   sai     Added support for deletion of cell DB postion support from QMI
08/31/20   sai     Changes made to remove unnecessary return condition during memory allocation failure
08/28/20 katta    Added new flag to indicate if wwan ranging is used and not to propagate the PUNC in wwan ranging server
06/26/20   sai     Added QZSS default enablement based on config NV as part of FR63097
07/23/20   katta   Added a NULL check to resolve crash when xtrat position is requested from GM
05/21/20   katta   Handling additional parameters passed from QMI and triggeer appropriate TLE API 
29/04/20   ska     Added changes for asking less proc duration when EPH is valid
05/08/20   ska     Smart compilation issues
22/03/20   ska     Changes done to support delete aiding data when MGP unload
02/18/20   ska     Reducing proc_duration when last fix EPH is valid
02/20/20   py      check correct time while propagating the fix according to source
10/01/20   ska     Changes done for MGP state to OFF when TBF>=5sec
24/01/20   ska     sarf status is still set to TRUE event after SARF_OFF
11/07/19   ska     Handling no protocol case in tm_core_abort_recover() by calling tm_core_handle_non_gnss_session_complete()
10/16/19   ska     If gtp cached fix hepe>75k then get the fix from gtp server forcefully
10/09/19   py      Stop UNC timer on non-GNSS session completion
10/14/19   ska     Added fix to force MGP to off when there is explicit loc stop request
09/18/19   py      CHeck if any active GNSS session before starting from XTRA avail done
08/21/19   ska     Added support for NMEA GSV
08/29/19   py      Reset and send DONE from one place for all the NON-GNSS session completion
08/06/19   py      set TM session state early to not allow multiple internal sessions.
06/19/19   kat     Propagating fix source is gtp
06/10/19   py      If WWAN is unavailable sent out GENERAL ERROR as status
05/15/19   nath    Added qualified enablement related changes
04/29/19   sum     Correcting condition for GTP to request server fix
04/23/19   sum     Request GNSS server fix if MGP is not loaded.
04/17/19   nath    CPI injection, Cached pos deletion, gts time deletion
04/12/19   nath    Using gts_GetTime to get time.
03/20/19   py      Restructuring of assitance data deletion 
10/26/17   rn      Notification from GPS FS task regarding XTRA EFS write operation.
02/26/18   mj      Changes to allow MO apptrack while in E911 state when 1930 is LOW priority
12/14/17   mj      Block sessions during SARF 
10/30/17   nath    Updated loc_unc_horizontal,alongAxisUnc and perpAxisUnc to float
10/24/17   rk      In L+L mode choose the correct srv_system field to know the RAT.
10/09/17   ak      Changes to start early RX start session on connected RAT.
07/25/17   ak      Changes to clean-up LPC/LDM on RAT change
07/13/17   skm     Check for 911 session ON when turning rcvr OFF on GERA client Disable from LM
06/16/17   mj      Changes to not remove E911 client from LDM when E911 call ends
06/15/17   kpn     Correct parts of code to pass the appropriate AS ID
06/06/17   mj      Fix issue where VX fails with E911 concurrency code changes
05/24/17   ak      Code optimization and code review comments addressed for E911 concurrency feature.
05/21/17   ak      Trigger CPI/Time injection on ME/PE recovery.
05/19/17   gk      Dont request wishlist during a tech sel or best pos
05/08/17   rn      Modified NV for LPP12 2 timer feature config to support LPP CP.
05/05/17   kpn     Clear cache of constellations whose meas blk is not received
04/14/17   mj      Fix to allow E911 to continue when apptrack ends in concurrent mode
04/14/17   skm     stop GET_BEST_POS from triggering TCP connection request to download AD
03/30/17   sum     Added support for Horizontal velocity greater than 0x7fff in tm_core_xlate_pos_to_ext_status()
03/27/17   mj      Fix to avoid injecting wishlist info during MSA in concurrent mode
03/21/17   gk      Allow Xtra injection during 911
02/21/17   kpn     Support of 1x CP on Sub2
02/17/17   skm     Add SRN (BTLE) and UBP (Baro) support for Lppe 2
01/24/17   kpn     L+L changes and support for E911 CP session on non-DD sub
01/16/17   muk     Fix for Meas Sequence ordering
01/15/17   gk      LPPM SV status should be mapped to TRACK
01/04/17   sum     Removed unnecessary call to tm_sim_mmgsdi_init(),same client ID and sessions handle can be used in all SIM swap cases
11/16/16   mj      E911 and apptracking concurrency support
11/04/16   muk     Enable QZSS meas and Poly data
10/26/16   nl      SET/ GET mismatch fix for nmea_ext_sentence_mask 
10/25/16   rk      Start & stop inconsistency tracking when ODCPI request is placed.
09/09/16   rk      Simulated e911 status isn't propagated to LM
09/06/16   mj      Fix issue when there is no session on going, TM did not remove client from GERA
09/01/16   yh      Convert position heading from Radian to Degree
08/30/16   skm     Added NV for default LPP12 2 timer feature config
08/30/16   kpn     Add slot id into mmsgsdi global session id table to support pbm_session_record_read()
08/12/16   mj      Initialize Leap Second Info struct during tm_core init
07/15/16   mj      Indicate the source of assistance data sent to LM
07/05/16   rk      Pass all measurements to protocols. It will decide which measurements should be
                   reported to server.
06/30/16   gk      LPPe config changes cleanup
06/20/16   mj      Support to retrieve Leap Second from MGP
05/26/16   muk     Send all the Poly reports to active Loc_MW session
05/25/16   mj      Add separate masks for health and IONO for GAL and BDS
05/15/16   ss      Removing CPI request based on Punc and AD download failure
                     CPI will be requested in GNSS session only based on TechSel Rules
05/07/16   gk      cache and use the right subid when refresh is received
05/01/16   muk     Change NV73542, to Enable Gnss Measurement by Default 
04/27/16   ak      Populate ENU velocity and uncertainity in position report
04/15/16   ld      Added NHz NV support  
04/04/16   mj      Add GERA support
03/25/16   pk      During OTA Reset, Call PGI api to reset SIB8/16 Decode
02/29/16   muk     Add support to output numClockResets
01/20/16   mj      Add a new flag b_e911_session to indicate e911 session in progress
01/05/16   mc      Dynamically allocate variables for huge structures to reduce stack usage
01/04/16   mj      Move changes to check sub id sanity to the call back function
11/25/15   mc      removed debug message in tm_core_is_on_demand_session_on()
02/17/16   mj      Add support to handle bestPosition fixes from LM
02/17/16   mj      Added NV support to enable/disable CPI injections and early exit logic
02/17/16   gk      During E911 force injection of refL
02/07/16   gk      Add LPPe config support
10/27/15   mj      Handle OTA Reset for UMTS
10/20/15   gk      request CPI for Standalone sessions
09/16/15   rk      Use the pre-defined GLONASS & 4G position method bits in NV 1920 to
                   enabled or advertise GLONASS & 4G position method support respectively.
09/16/15   skm     BDS Health delete fix
08/26/15   rk      Added new OTDOA stop engine request processing support.
08/26/15   muk     TL 1.0 integration
08/05/15   rh      Moved carrier-specific NV 1920 (pos mode support) to TM core 
08/04/15   js      Default Enablement of XTRA download 
07/31/15   mj      Changes to fix E911 on second SUB in G+G config
07/08/15   rh      Changed NV refresh to be done on one specifc sub
07/03/15   jv      Added QZSS Support
06/25/15   gk      Abort SVLTE session when 1x CP starts only when there is a ongoing session
06/22/15   gk      Write leap second change to NV at GPS off if there was a change during the session
06/09/15   ssu      Adding NV for GM filtering of WLAN freq. 
06/09/15   mj      Add support for W+W feature
04/30/15   mj      Add support for early session start for WCDMA for E911
04/16/15   jv      Lowering priority of some messages from MSG_ERROR to MSG_MED
04/06/15   mj      Add new function tm_core_is_active_sess_mo
03/14/15   rh      Added support for MultiSIM NV
01/16/15   rh      Fixed missing SVInfo issue
12/18/14   rh      Fixed wrong merge for LPP CP transmission delay EFS NV read
11/27/14   jv      expand PDSM_LITE_PA_DELETE_HEALTH to GLO, BDS and GAL too.
11/29/14   ssu     GM LOWI Integration
11/26/14   am      GAL lm delete request initialization was added
03/12/14   gk      Handle OTA reset when session is active. consider as end of the session
11/25/14   gk      NV item for greater than 24 AD 
11/14/14   gk      NV item for terminating SUPL after final fix 
11/13/14   rh      Added configurable LPP CP transmission delay (via EFS NV)
10/25/14   jv      Added Galileo Support
09/10/14   gk      Added delete throttle timer to prevent B-b delete commands
08/25/14   jv      16-bit SVID Support
08/19/14   ah      Copy pointer contents of GPS state info response once received
08/02/14   gk      do not abort early rcvr session with a NI session is started. 
07/21/14   ssu     Support for GM WWAN motion detector
06/07/14   ah      Map Premium Service NV value with correct SAP enablement value
05/20/14   hs      Replacing the the idx type from gnss_ to bcn_ for TDP support
05/20/14   ssu     Removing redundant code. 
05/19/14   gk     E911 NI during MO should be accepted
05/15/14   rh      Fixed Wiper not stopped issue if session is triggered as MSB initially
04/24/14   yy      Change prisoner sv to gnss sv blacklist
04/24/14   rh      Added delete prisoner SV support
04/17/14   skm     NV Async Read changes
04/01/14   ssu    Addressing a few KW errors .
04/01/14   ssu    Addressing a few KW errors .
03/26/14   ah      Added trigger of GTP WWAN session if PUNC>25km for Get Avail WWAN Pos
02/21/14   ssu     Adding NV for configuring motion sensing distance in case of challenging GPS env.
02/20/14   ssu     Adding a NV for modifying the medium responsiveness backoff value and gps challenging env backoff config.
03/10/14   ah      Added support for Set Premium Services Cfg & Get Avail WWAN Pos
02/10/14   ssu     Adding a NV for modifying the rate at which GM requests CPI injection. 
01/31/14   ss      Supporting Max acceptable GF PUNC confguration from QMI
01/27/14   mj      Remove featurization to fix issue where SGLTE code was featured out
01/13/14   rh      Added requesting XTRA download during SUPL feature 
01/10/14   ss      Supporting High Responsiveness Geofencing Config
12/23/13   mj      Remove FEATURE_DUAL_SIM and FEATURE_TRIPLE_SIM
12/02/13   ah      Support GPS State Info requests for multiple clients simultaneously
09/27/13   mj      TSTS support: Define tertiary mmgsdi sessions for triple SIM
09/12/13   mj      Additional check for app_type while retrieving mmgsdi session id
06/26/13   rh      Fixed a bug related to PA Delete bitmask which BDS-only deletion is failing
06/13/13   rh      Used same pdsm_lite_GnssNavSvInfo in SM_API & PDAPI for SV info
06/12/13   ah      Added measurement report support for BDS standalone (not yet for AGNSS)
06/11/13   mj      Report intermediate fixes for protocols when sysd is enabled
06/07/13   rh      Added assistance data deletion support for BDS
06/07/13   rh      Removed diag timer and NO_FIX report handling
03/20/13   mj      Modified callback logging
03/01/13   kj      Move to the DOG Heartbeat mechanism on DIME and beyond
01/31/13   mj      Added new handler tm_core_api_callback_handler and tm_core_api_cb_timer_handler
02/21/13   mj      Adding missed code for GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES NV read support
01/22/13   ah      Support extraction of MPG event from union tm_info_s_type
01/06/13   gk     ECID changes
12/27/12   rk      Update fix report translation function to copy sensor (gyro/accel) 
                   bias report structure. 
12/04/12   rk      CR 408753 - Changes relating to extended SV parameter enhancement in GNSSFixReport.
11/06/12   mj      Fixed issue where NV_CGPS_UTC_GPS_TIME_OFFSET_I was not updated
10/09/12   rh      Added a few error code mappings for IS801 session
09/07/12   mj      Added tm_core_external_cell_db_proc and support for existing cell db proc for SGLTE
09/03/12   rh      Added support for NI message injection
08/01/12   mj      Added handler for GET PARAM request (tm_core_get_param_msg_handler)
07/18/12   mj      Added GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES NV read support
06/11/12   rk      Added GNSS_NV_EFS_SM_TM_ASSISTED_GLO_PROTOCOL_SELECT NV read support 
04/18/12   ssu     Using sm_PositionReportStructType when reporting GPS state from ME. 
03/01/12   rh      Added tm_core_handle_engine_error_recovery_report()
02/16/12   gk      Altitude also reported in float now
02/06/12   gk      LPP enablement via PDAPI
11/23/11   ssu     Adding GFC_QMI feature.
12/13/11   gk      LPP related fixes
11/14/11   gk      Added new variable for PDAPI position inj req event
07/26/11   rb      Added handling for ptr set to NULL in TM_PHONE_EVENT_DSDS_SUBS_CHANGED  
06/03/11   gk      Clear cached position  when delete param or gps lock is sent
06/16/11   rh      Added TM_PHONE_EVENT_OTASP_STATUS_COMMITTED event processing
05/18/11   rc      Lowering priority of some standalone messages from MSG_HIGH to MSG_LOW
05/03/11   gk      cache the positin info from AD request
01/03/11   gk      Eph throttling rework
11/23/10   rh      Added TM feature to send NO_FIX as Intermediate Report
10/05/10   ns      Port CL1333689: Save status of gps state info req by SYSD.
09/22/10   MG      Not Generating gpgga, gprmc when giving out Wi-Fi fixes.
08/23/10   gk      Added new handling for optimal modes
06/06/10   rb      Add tm_core_time_validity to determine if GPS time is valid
06/22/10   gk      Request postiion injection if MSB fails
06/03/10   bp      adding smart MSB support for V1/V2
05/25/10   gk      Add new NV item for XTRA validity
04/07/10   rb      Update UTC Time Offset NV item when needed
04/02/10   gk      Added an event callback to inject position
03/03/10   ns      DSDS Support
01/13/10   gk      end session forces receiver off if OEM client.
01/08/10   gk      add rf loss to c/no for the PDAPI measurement report.
01/06/10   jlp     Changed CGPS_WHOLE_EARTH_HEPE to CGPS_WHOLE_EARTH_CEP99.
12/02/09   gk      intermediate position fixes
11/11/09   gk      set optional flags for final position, also set the uncs correctly
11/10/09   atien   Check for com failure if allowing
                   MSB fallback to Standalone with XTRA download if no XTRA data
10/12/09   gk      Force receiver off
09/30/09   gk      Support ODP 2.0
07/27/09   gk      tm_core_translate_end_status() - translate TM core end session
                   error into PDSM PD End Event error reason
08/11/09   gk      Addition ODP 2.0 changes
07/27/09   gk      tm_core_translate_end_status() - translate TM core end session
                   error into PDSM PD End Event error reason
07/23/09   gk      Support ODP 2.0
07/23/09   gk      Added DELETE_SV_NO_EXIST flag for delete params
07/13/09   gk      process end session command even when no TM session
07/10/09   atien   Spirent requested feature:  allow XTRA dload after MSB dload comm failure
07/09/09   gk      ODP 1.1 related fixes for ready mode
07/08/09   gk      undo the previous change for not reporting >1 intermediate positions
06/10/09   gk      Added Get UTC offset from NV
06/10/09   gk      ODP 1.1 support
06/04/09   gk      other intermediate position changes
05/14/09   gk      fixed gps state info handling
04/16/09   gk      Fill in xtra data correctly in the gps state
04/09/09   ns      REF LR intermediate reported routed to tm_qwip_core.c to check
                   Wifi fix status.
03/27/09   jd      Added a generic abort handling mechanism for SysD
04/02/09   ns      support for sending diag info to sysd.
03/12/09   ns      Init Sysd triggered flag
03/04/09   ns      Added support for passing gps state info to qwip
02/04/09   atien   MO-LR vs AD Call Flow Select support
01/13/09   atien   XTRA-T support
01/25/09   ns      Added support for Qwip
09/15/08   gk      Rename the on-demand struct to sm_ from cgps_
08/26/08   gk      On-Demand GPS Lock Changes
08/20/08   cl      Set op_request flag in session continue request.
08/13/08   gk      Enable force receiver off
08/08/08   cl      Enable non-encrypted lat/long when security is activated
07/30/08   cl      Set elevation/azimuth only when u_SvDirValid is valid
07/11/08   cl      Inform 1x/UMTS modules when switching serving systems
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec
04/25/08   gk      On-demand changes
04/04/08   cl      Inform 1x_up when entering UMTS system
03/13/08   cl      For standalone pos, not encrypt.
02/06/08   cl      Added accommodating "forced TM termination".
01/22/08   gk      fixed compiler warning that didnt init the vel correctly
01/21/08   cl      Allow overlapping dload and fix (fix and dload) sessions
11/13/07   gk      do not set heading to 0 if speed is 0
10/30/07   cl      Added smart MSB logic
09/14/07    gk     NMEA CONFIG set parameters support
08/14/07   ank     Inform MGP about nonexistent SVs.
09/11/07   cl      Populate ext_data when security is not enabled
08/01/07   rw      Add support for XTRA feature
08/02/07   lt      Added handling of SV-not-exist message.
07/30/07   gk      Add handling for last position
07/26/07   cl      Populate client id for gps start and end events
07/25/07   cl      Add more mapping from interal end status to external end status
07/17/07   lt      L1 interface.
06/27/07   lt      Added the passing of WCDMA in-service to UMTS submodules.
06/18/07   ld      Added support for Deleting Cell DB Information.
06/12/07   cl      Only allow PD API access when TM task is active
05/30/07   lt      Mods. to make TM's RRC and MM iface symmetrical to CM and RR.
05/27/07   rw      Add support for JGPS (KDDI) module
05/23/07   mr      Added call to generate NMEA PSTIS string
05/16/07   mr      Removed NMEA string generation for intermediate fixes
05/16/07   mr      Added calls to generate NMEA string for MSA fix from server
04/24/07   ank     Change to use CGPS_SRV_SYS enum in place of TM_CM_SRV_SYS.
04/11/07   lt      Ready for activating UMTS CGPS protocol modules.
04/04/07   gk/mr   PC Time injection handling
04/03/07   cl      Feed LR to MGP.
02/15/07   cl      Moved pdsm_lite_getbuf in tm_core_lm_sess_info_handler
02/15/07   cl      Add support to inject pre-fix ppm
02/01/07   cl      Add feature to check if CDMA is supported
02/01/07   lt      Added UTC model handling
01/31/07   cl      Added LCS handling
12/04/06   cl      Added NV functionality
12/04/06   cl      pd api event report interface change
10/13/06   cl      Change to use SM_TM for dog report
07/12/06   cl      Initial version

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */
#include <stdio.h>
#include "ldm_api.h"  //adding explicitly to make sm work #9x05
#include "msg.h"
#include "event.h"
#include "aries_os_api.h"
#include "aries_ipc_diag_log.h"
#include "sm_nv.h"
#include "sm_log.h"
#include "tm_data.h"    /* Definition of TM task data structure */
#include "tm_common.h"   /* Definition of TM task data structure */
#include "lm_api.h"
#include "tm_pdapi_client.h"
#include "tm_cm_iface.h"
#include "tm_prtl_iface.h"
#include "tm_standalone.h"
#include "tm_diag.h"
#include <math.h>
#include "time_svc.h"
#include "tm_nv.h"
#include "memory.h"
#include "gps_common.h"
#include "sm_util.h"
#include "pdapibuf.h"
#include "sm_dm_event.h"
//#include "tech_sel_api.h"

#ifdef FEATURE_CGPS_XTRA
#include "tm_xtra.h"
#endif /* FEATURE_CGPS_XTRA */

#include "mgp_api.h"

#ifdef FEATURE_CGPS_XTRA_T
#include "tm_xtra_t.h"
#endif /* FEATURE_CGPS_XTRA_T */

#define INV_SQRT_2 0.7071067811865
/* The Punc value threshold for Xtra T download request ( in meters ), changed 75000m from 25000 as per input from PE */
#define PUNC_THRESHOLD_FOR_XTRA_T 75000


#include "sm_nv_efs.h"

#ifdef FEATURE_CGPS_XTRA_T
#include "tle_api.h"
#endif

#include "loc_wwan_me_api.h"

#include "gm_api.h"
#include "gfc_qmi.h"
#include "gnss_calendar_util.h"

#ifdef FEATURE_GNSS_LPPE
#ifndef FEATURE_GNSS_LEAN_BUILD
#include "tm_auxtech_iface.h"
#endif /* ! FEATURE_GNSS_LEAN_BUILD */
#endif /* FEATURE_GNSS_LPPE */

#include "gnss_consts.h"
//#include "wle_api.h"
#include "location_service_iot_v02.h"
#include "gts_api.h"
#include "gts_loc_api.h"
#include "location_common.h"
#include "lm_data.h"
/*===========================================================================
                 Data Definition
===========================================================================*/
#define TM_CORE_PRTL_CB_ARR_INDEX_STANDALONE         0
#define TM_CORE_PRTL_CB_ARR_INDEX_DIAG               1
#define TM_CORE_PRTL_CB_ARR_INDEX_GFC_QMI            2
#define TM_CORE_PRTL_CB_ARR_INDEX_XTRA_T             3
#define TM_CORE_PRTL_CB_ARR_INDEX_MAX                4 /* Must be set to      */

/* same value as above */

#define PA_AIDING_DATA_DELETE_MASK  (  PDSM_LITE_PA_DELETE_TIME    \
                                     | PDSM_LITE_PA_DELETE_POS     \
                                     | PDSM_LITE_PA_DELETE_EPH     \
                                     | PDSM_LITE_PA_DELETE_ALM     \
                                     | PDSM_LITE_PA_DELETE_UTC     \
                                     | PDSM_LITE_PA_DELETE_HEALTH  \
                                     | PDSM_LITE_PA_DELETE_SVDIR   \
                                     | PDSM_LITE_PA_DELETE_SVSTEER \
                                     | PDSM_LITE_PA_DELETE_SADATA  \
                                     | PDSM_LITE_PA_DELETE_RTI     \
                                     | PDSM_LITE_PA_DELETE_CELLDB_INFO  \
                                     | PDSM_LITE_PA_DELETE_ALM_CORR \
                                     | PDSM_LITE_PA_DELETE_FREQ_BIAS_EST \
                                     | PDSM_LITE_PA_DELETE_SV_NO_EXIST \
                                     | PDSM_LITE_PA_DELETE_ALM_CORR_GLO \
                                     | PDSM_LITE_PA_DELETE_ALM_GLO \
                                     | PDSM_LITE_PA_DELETE_EPH_GLO \
                                     | PDSM_LITE_PA_DELETE_SVDIR_GLO \
                                     | PDSM_LITE_PA_DELETE_SVSTEER_GLO \
                                     | PDSM_LITE_PA_DELETE_TIME_GLO \
                                     | PDSM_LITE_PA_DELETE_TIME_GPS \
                                     | PDSM_LITE_PA_DELETE_TIME_BDS \
                                     | PDSM_LITE_PA_DELETE_EPH_BDS \
                                     | PDSM_LITE_PA_DELETE_ALM_BDS \
                                     | PDSM_LITE_PA_DELETE_SVDIR_BDS \
                                     | PDSM_LITE_PA_DELETE_SVSTEER_BDS \
                                     | PDSM_LITE_PA_DELETE_ALM_CORR_BDS \
                                     | PDSM_LITE_PA_DELETE_EPH_QZSS \
                                     | PDSM_LITE_PA_DELETE_ALM_QZSS \
                                     | PDSM_LITE_PA_DELETE_SVDIR_QZSS \
                                     | PDSM_LITE_PA_DELETE_SVSTEER_QZSS \
                                     | PDSM_LITE_PA_DELETE_HEALTH_QZSS \
                                     | PDSM_LITE_PA_DELETE_ALM_CORR_QZSS \
                                     | PDSM_LITE_PA_DELETE_TIME_GAL \
                                     | PDSM_LITE_PA_DELETE_EPH_GAL \
                                     | PDSM_LITE_PA_DELETE_ALM_GAL \
                                     | PDSM_LITE_PA_DELETE_SVDIR_GAL \
                                     | PDSM_LITE_PA_DELETE_SVSTEER_GAL \
                                     | PDSM_LITE_PA_DELETE_ALM_CORR_GAL \
                                     | PDSM_LITE_PA_DELETE_CLOCK_INFO \
                                     | PDSM_LITE_PA_DELETE_HEALTH_GLO\
                                     | PDSM_LITE_PA_DELETE_HEALTH_BDS\
                                     | PDSM_LITE_PA_DELETE_HEALTH_GAL\
                                     )

#define PA_AIDING_DATA_INJECT_MASK  (PDSM_LITE_PA_TIME_UNC | PDSM_LITE_PA_POS_UNC | PDSM_LITE_PA_TIME_OFFSET)

/* using 75% of the entire constellation, same percentage as   
  C_COMPLETE_CONSTALLATION_ALM_THRESHOLD to GPS */
#define C_COMPLETE_CONSTALLATION_GLO_ALM_THRESHOLD 18

#define TM_CORE_GTS_TUNC_VALIDITY_LIMIT 100

#define TM_CORE_API_CALLBACK_INTERVAL_MS (1000)

#define TM_CORE_CLIENT_BITMASK(x) (1 << (x))

/* Define Leap Second offset uncertainity during module init */
#define TM_CORE_LEAP_SEC_INIT_UNC (1)
/*Max HEPE */
#define TM_MAX_HEPE 20000.0f
/* This is a global variable which stores all the information of TM-core */
tm_core_param_s_type     tm_core_info;

/* Define nominal Time-Between-Fixes to be 1000ms
   Most applications use this. 
   For NHz operation, the TBF will be less than 1000ms.
   If NHz mode is not enable, default the TBF to 1000ms
*/
#define TM_CORE_NOMINAL_TBF_MS (1000)

#define TM_CORE_INVALID_WEEK_NUMBER 65535

/*This is for NV GNSS_NV_EFS_SM_GNSS_CONTROL_FIX_OPERATION handling, 
Bit 0 is to control SM cache fix, further defines need to be added in hexa format 0x2,0x4 etc accorrdingly in case if NV is expanded*/
#define GNSS_CONTROL_FIX_OPERATION_SM_CACHE 0x01

/* NV buffer, used to read NV items */
static nv_item_type      *tm_nv_read_buffer = NULL;

/*TODO: [MOD][APP] #9x05: z_cached_pos_info is being used for now in gpsdiag_PositionEventCb so we are making
it non static temporarily*/
#if 0
 tm_core_cached_pos_s_type z_cached_pos_info;
static tm_core_cached_ref_pos_s_type z_cached_ref_pos_info;
#endif
/* Counter for EVENT_GPS_PD_FIX_START */
uint16 tm_core_event_counter = 0;

static const uint8 u_DaysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static const uint8 u_DaysInMonthLeap[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

uint8 b_bypass_xtra_validity_check = GNSS_NV_EFS_SM_BYPASS_XTRA_VALIDITY_CHECK_DEFAULT; 

#if 0
typedef struct
{
  /* TUNC in millisecs */
  boolean b_TuncValid;
  FLT f_ClkTimeUncMs;

  /* FUNC in Millisec/Second */
  boolean b_FuncValid;
  FLT f_FreqEstUncMs;
} gts_PreCheckInfoType;
#endif
static void  tm_core_xlate_gps_state_to_ext_status(tm_core_gps_state_info_s_type *p_gps_state_info,
                                                   pdsm_lite_pd_gps_state_s_type *pd_gps_state_info);
static void tm_core_xlate_pd_wwan_pos_to_qmi(const sm_FixInfoStructType *p_GnssFixRpt,
                                                        qmiLocGetAvailWwanPositionIndMsgT_v02  *p_PdsmExtPos );

static void tm_core_xlate_pd_best_avail_pos_to_qmi(const sm_FixInfoStructType *p_GnssFixRpt,
                                                        qmiLocGetBestAvailablePositionIndMsgT_v02  *p_PdsmExtPos );

void tm_core_handle_non_gnss_session_complete(pdsm_lite_pd_info_s_type *pd_info, tm_pdapi_sess_type sess_type);

static float tm_core_calculate_2d_punc(const sm_FixInfoStructType *pz_GnssFixRpt);
boolean tm_core_start_gnss_session(void);
void tm_core_handle_gts_sys_unc_wait_timer_expire();
void tm_core_handle_gtp_wwan_fix_timer_expire();
extern void gts_GetPreCheckSystemUnc( gts_PreCheckInfoType *p_PreCheckData );
tm_core_system_unc_state tm_core_req_gts_for_sys_unc(boolean is_first_attempt, boolean* v_is_tunc_valid);
extern pdsm_lite_pd_event_type tm_util_xlate_fixt_report_to_qmi_loc_pos_rpt_Ind(sm_FixInfoStructType *lr_data,
                                                        qmiLocEventPositionReportIndMsgT_v02* pz_PositionReportIndMsg , 
                                                        prtl_sess_info_e_type orig_sess_info_type );
extern uint16 tm_util_xlate_meas_rpt_to_qmi_loc_SV_Info_Ind( gnss_MeasBlkStructType *lr_data,qmiLocEventGnssSvInfoIndMsgT_v02 *pz_SVInfoIndMsg,uint16 sv_index);


FLT tm_core_get_hepe_for_propagated_fix_report(sm_FixInfoStructType  * p_input_fix_rpt, sm_FixInfoStructType * p_output_propagated_fix_rpt);

void tm_core_handle_gps_state_info_req_type_get_gtp_wwan_fix(sm_FixInfoStructType * p_fixReport);
extern uint32 locFM_getGnssServiceFeatureId(void);
extern  uint32 locFM_getGtpWwanServiceFeatureId(void);

#ifdef FEATURE_CGPS_XTRA_T
  #ifdef FEATURE_GNSS_TDP_10
  extern void tle_TriggerTerPosUpdate(boolean isCachedFixRequired);
  #else
  extern void tle_TriggerTerPosUpdate(boolean isCachedFixRequired, sm_ReqEnumType source);
  #endif /* FEATURE_GNSS_TDP_10 */
#endif
#define TM_MAX_PUNC_VALUE_FOR_POS_INJ_REQ 10000 /* 10km */


static prtl_sess_info_param_u_type *up_cache_sess_info_param = NULL;

tm_core_gnss_pos_cache_s_type z_gnss_position_cache;

/*This is the minimum time that should have elapsed since last 
request to GTP-WWAN server, befor a new request to the server is made
*/
#define TM_CORE_GTP_WWAN_MIN_INTERVAL_BTWN_REQ  5*60*1000
#define TM_CORE_MIN_GTP_SERVING_HEPE            500 /*HEPE of 500m*/



/* 
This keeps a record whethe GTP WWAN fix was requested 
from local cache or server. 
If the request was made for cached fix and the fix received does not satisfy
HEPE then fix would be requested from server.
*/

/*This function is defined in tm_util.c. It is used for getting gnss constellation to be used in a country for navigation*/
extern void tm_util_update_constellation_on_mcc_change(uint32 mcc);
extern void tm_util_get_constellation_info_from_efs(uint32 * p_gnss_config, uint8 * p_constellation_id);
extern lm_dmm_state_e_type lm_get_dmm_state(void);
/*===========================================================================

FUNCTION tm_core_update_session_state

DESCRIPTION
  This function determines if a state transition request is valid and
  set tm session state accordingly.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_update_session_state
(
   tm_core_session_state_e_type new_state
   )
{
  if (new_state != tm_core_info.session_info.session_state)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core state transition [%d] -> [%d] for handle [%d]",
          tm_core_info.session_info.session_state,
          new_state,
          tm_core_info.session_info.active_sess_handle);

    tm_core_info.session_info.session_state = new_state;
  }
}


/*===========================================================================
TODO: NATH: CHECK THE UTILITY #9x05
FUNCTION tm_core_read_carrier_specific_efs_nv

DESCRIPTION
  This function read EFS NV items which may be carrier-specific
  (at bootup time or during SIM hot swap)
  Will read from hardware and may save to EFS NV cache(if available)
  
  It will refresh NV for a specific sub.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void tm_core_read_carrier_specific_efs_nv(void)
{
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Read CS EFS NV invalid sub %d", 0);
    return;
}

/*===========================================================================

FUNCTION tm_core_read_nv

DESCRIPTION
  This function read NV items to set TM-Core configuration info structure.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_read_nv(void)
{
  uint32 j, k;

  /***************************************************************************
  * Non-EFS NV Items
  ***************************************************************************/
  sm_nv_init();

  /* read carrier-specific NVs */

  /* initialize the new NV items that use EFS here */

  tm_core_info.config_info.gm_config.v_sm_gm_accept_cell_cpi_fixes = TRUE;

  (void)sm_nv_read(NV_MOB_MODEL_I, (void *)tm_nv_read_buffer);

  /***************************************************************************
  * EFS NV Items
  ***************************************************************************/
  sm_nv_efs_init();

    /* read carrier-specific EFS NVs */
    tm_core_read_carrier_specific_efs_nv();

  if (FALSE == sm_nv_efs_reg_item_read(CGPS_NV_EFS_SM_XTRA_PREFERRED_MAX_VALID_AGE,
                              sizeof(uint16),
                              &tm_core_info.config_info.preferred_xtra_age))
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.preferred_xtra_age = CGPS_NV_EFS_SM_XTRA_PREFERRED_MAX_VALID_AGE_DEFAULT;
  }

  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_SEND_NO_FIX_REPORT,
                              sizeof(uint8), &tm_core_info.config_info.send_no_fix_report))
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.send_no_fix_report = GNSS_NV_EFS_SM_SEND_NO_FIX_REPORT_DEFAULT;
  }

  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_CONFIG_QMI_SERVICE_EXTERNAL,
                              sizeof(uint32), &tm_core_info.config_info.qmi_cfg.enableMask))
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.qmi_cfg.enableMask = GNSS_NV_EFS_CONFIG_QMI_SERVICE_EXTERNAL_DEFAULT;
  }


  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_GNSS_UNAVAIL_IND_TIMEOUT,
                              sizeof(uint32), &tm_core_info.config_info.gm_config.sm_gm_gnss_unavail_ind_timeout))
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_gnss_unavail_ind_timeout = GNSS_NV_EFS_SM_GM_GNSS_UNAVAIL_IND_TIMEOUT_DEFAULT;
  }    

  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_MOTION_DETECTION_SOURCES,
                              sizeof(uint32), &tm_core_info.config_info.gm_config.sm_gm_motion_detection_sources))
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_motion_detection_sources = GNSS_NV_EFS_SM_GM_MOTION_DETECTION_SOURCES_DEFAULT;
  }

  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_POSITION_SOURCES,
                              sizeof(uint32), &tm_core_info.config_info.gm_config.sm_gm_position_sources))
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_position_sources = GNSS_NV_EFS_SM_GM_POSITION_SOURCES_DEFAULT;
  }

  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESS_TIMEOUT,
                              sizeof(uint8), &tm_core_info.config_info.gm_config.sm_gm_gnss_pos_qos_sess_timeout))
  {
    /* if returned false for any reason, force it to default value which is GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESS_TIMEOUT_DEFAULT*/
    tm_core_info.config_info.gm_config.sm_gm_gnss_pos_qos_sess_timeout = GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESS_TIMEOUT_DEFAULT;
  }

  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_GNSS_MAX_POS_UNC_ACCEPTED,
                              sizeof(uint32), &tm_core_info.config_info.gm_config.sm_gm_gnss_max_pos_unc_accepted))
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_gnss_max_pos_unc_accepted = GNSS_NV_EFS_SM_GM_GNSS_MAX_POS_UNC_ACCEPTED_DEFAULT;
  }  

  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_HIGH_RESPONSIVENESS_CONFIG,
                              sizeof(uint32), &tm_core_info.config_info.gm_config.sm_gm_high_responsiveness_config))
  {
    /* if returned false for any reason, force it to default value which is GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESS_TIMEOUT_DEFAULT*/
    tm_core_info.config_info.gm_config.sm_gm_high_responsiveness_config = GNSS_NV_EFS_SM_GM_HIGH_RESPONSIVENESS_CONFIG_DEFAULT;
  }

  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_CPI_REQUEST_RATE,
                              sizeof(uint32), &tm_core_info.config_info.gm_config.sm_gm_cpi_request_rate))
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_cpi_request_rate = GNSS_NV_EFS_SM_GM_CPI_REQUEST_RATE_DEFAULT;
  }

  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_MED_RESP_BACKOFF,
                              sizeof(uint32), &tm_core_info.config_info.gm_config.sm_gm_med_resp_backoff))
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_med_resp_backoff = GNSS_NV_EFS_SM_GM_MED_RESP_BACKOFF_DEFAULT;
  }

  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_CONFIG,
                              sizeof(gm_chal_gps_env_backoff_config_s_type), &tm_core_info.config_info.gm_config.sm_gm_chal_gps_env_bo_config))
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_chal_gps_env_bo_config.q_backoff_min = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_MIN_DEFAULT;
    tm_core_info.config_info.gm_config.sm_gm_chal_gps_env_bo_config.q_backoff_max = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_MAX_DEFAULT;

  }
  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_CONFIG,
                              sizeof(gm_chal_gps_env_ms_dist_config_s_type),
                              &tm_core_info.config_info.gm_config.sm_gm_chal_gps_env_ms_dist_config))
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_chal_gps_env_ms_dist_config.q_dist_used_for_gps = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_GPS_DEFAULT;
    tm_core_info.config_info.gm_config.sm_gm_chal_gps_env_ms_dist_config.q_dist_used_for_cpi = GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_CPI_DEFAULT;
  }

  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_MS_SPEED_CONFIG,
                              sizeof(gm_ms_speed_config_s_type),
                              &tm_core_info.config_info.gm_config.sm_gm_motion_state_speed_config))
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.sm_gm_motion_state_speed_config.gm_ms_walk_speed = GNSS_NV_EFS_SM_GM_MS_WALK_SPEED_DEFAULT;
    tm_core_info.config_info.gm_config.sm_gm_motion_state_speed_config.gm_ms_run_speed = GNSS_NV_EFS_SM_GM_MS_RUN_SPEED_DEFAULT;
  }

  if (FALSE == sm_nv_efs_reg_item_read(
        GNSS_NV_EFS_SM_GM_IOD_CONFIG_SWITCHES_MASK,
        sizeof(tm_core_info.config_info.gm_config.q_sm_gm_iod_config_switch_mask),
        &tm_core_info.config_info.gm_config.q_sm_gm_iod_config_switch_mask))
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.q_sm_gm_iod_config_switch_mask = GNSS_NV_EFS_SM_GM_IOD_CONFIG_SWITCHES_MASK_DEFAULT;
    tm_core_info.config_info.gm_config.q_sm_gm_iod_config_switch_mask = 5;
  }

  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GM_COMP_LOGGING_RATE,
                              sizeof(uint32), &tm_core_info.config_info.gm_config.q_gm_comp_logging_rate))
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gm_config.q_gm_comp_logging_rate = GNSS_NV_EFS_SM_GM_COMP_LOGGING_RATE_DEFAULT;
  }

  /* Read the GNSS NHz Config */
  if (FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_NHZ_CONFIG,
                              sizeof(uint32), &tm_core_info.config_info.gnss_nhz_config))
  {
    /* if returned false for any reason, force it to default value */
    tm_core_info.config_info.gnss_nhz_config = GNSS_NV_EFS_SM_NHZ_CONFIG_DEFAULT;
  }

  if(FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GTP_WWAN_MIN_INTERVAL_BTWN_REQ,
                            sizeof(uint16),
                            &tm_core_info.min_wait_interval_btwn_req_to_gtp_server))
  {
    tm_core_info.min_wait_interval_btwn_req_to_gtp_server = GNSS_NV_EFS_SM_GTP_WWAN_MIN_INTERVAL_BTWN_REQ_VAL_DEFAULT;
  }

  if(FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_BYPASS_GNSS_AIDING_CHECK,
                            sizeof(uint8),
                            &tm_core_info.config_info.bypass_gnss_aiding_check))
  {
    tm_core_info.config_info.bypass_gnss_aiding_check = GNSS_NV_EFS_SM_BYPASS_SYS_UNC_CHECK_VAL_DEFAULT;
}

  if(CGPS_NV_EFS_REG_RW_STAT_OK != gps_nv_efs_reg_item_read(GNSS_NV_EFS_ME_GNSS_CONFIG,sizeof(uint32), &tm_core_info.efs_nv_gnss_config))
  {
    tm_core_info.efs_nv_gnss_config|= (C_RCVR_GNSS_CONFIG_GPS_ENABLED | C_RCVR_GNSS_CONFIG_GLO_ENABLED|C_RCVR_GNSS_CONFIG_QZSS_ENABLED_OUTSIDE_OF_US);
  }
  
  if(FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GTP_WWAN_FIX_TIMER_DURATION,
                            sizeof(uint32),
                            &tm_core_info.config_info.gtp_wwan_fix_timer_val))
  {
    tm_core_info.config_info.gtp_wwan_fix_timer_val = GNSS_NV_EFS_SM_GTP_WWAN_FIX_TIMER_DURATION_VAL_DEFAULT;
}

  if(FALSE == sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_GNSS_CONTROL_FIX_OPERATION,
							sizeof(uint8),
							&tm_core_info.config_info.gnss_control_fix_operation))
  {
	tm_core_info.config_info.gnss_control_fix_operation = GNSS_NV_EFS_SM_GNSS_CONTROL_FIX_OPERATION_DEFAULT;
  }
}
/*===========================================================================

FUNCTION tm_core_refresh_carrier_specific_nv

DESCRIPTION
  This function refresh carrier-specific NVs (mainly for SIM hot swap) for a
  specific subscription (passed in p_data)

  sm_nv_read() will always read from HW and save in cache. 
  sm_nv_read_ex() may read from cache if available.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_refresh_carrier_specific_nv(void *p_data)
{
  int i, j;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  sys_modem_as_id_e_type sub;

  if (p_data == NULL)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NV Refresh NULL ptr");
    return;
  }

  /* only parameter is uint8 sub for now (0~2) */
  sub = (sys_modem_as_id_e_type)(*((uint8 *)p_data));

  tm_core_read_carrier_specific_efs_nv();

#endif /* FEATURE_MODEM_CONFIG_REFRESH */
}

/*===========================================================================

FUNCTION tm_core_get_gps_lock_state

DESCRIPTION
  This function determines if gps is locked

RETURN VALUE
  TRUE if gps is locked, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_gps_lock_state(void)
{
  if (TM_CORE_CONFIG_DD(gps_lock) != PDSM_LITE_GPS_LOCK_NONE)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*===========================================================================

FUNCTION tm_core_is_internal_session_on

DESCRIPTION
  This function determines if internal session is ongoing

RETURN VALUE
  TRUE if internal session is running, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_internal_session_on(void)
{
  if (tm_core_info.internal_session)
  {
    return TRUE;
  }
  else
    return FALSE;
}

#ifdef FEATURE_CGPS_XTRA

/*===========================================================================
FUNCTION tm_core_is_xtra_operation_allowed

DESCRIPTION
Check if assisted mode GPS session are in progress.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE
XTRA command status
===========================================================================*/
boolean tm_core_is_xtra_operation_allowed
(
   void
   )
{
  /* Local Variable */
  boolean           ret_val = FALSE;

  /* Allow XTRA data or time dload during idle, standalone, or diag sessions */
  if (tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_NONE
      || tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_STANDALONE
      || tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_DIAG
      )
  {
    ret_val = TRUE;
  }

  return ret_val;
} /* End tm_core_is_xtra_operation_allowed()*/


/*===========================================================================

FUNCTION tm_xtra_init_and_read_nv

DESCRIPTION
  This function reads NV items to set TM-XTRA configuration info structure.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_xtra_init_and_read_nv
(
   void
   )
{
  /* Local Variables */
  tm_xtra_state_type *xtra_state_ptr;
  #if 0
  tm_xtraint_state_type *xtraint_state_ptr;
  #endif
  nv_item_type       *nv_write_buffer = NULL;

  /*----------------------------------------------------------------------
     Read all XTRA NV items
  ----------------------------------------------------------------------*/

  if (sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_BYPASS_XTRA_VALIDITY_CHECK,
                              sizeof(uint8),
                              &b_bypass_xtra_validity_check)==FALSE)
    
  {
    b_bypass_xtra_validity_check = GNSS_NV_EFS_SM_BYPASS_XTRA_VALIDITY_CHECK_DEFAULT;  
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Failed to read XTRA validity check from NV");
  }


  /* Enable/Disable XTRA module */
  (void)sm_nv_read(NV_GPS1_XTRA_ENABLED_I, (void *)tm_nv_read_buffer);
  nv_write_buffer = (nv_item_type*)os_MemAlloc(sizeof(nv_item_type), OS_MEM_SCOPE_TASK);
  if (nv_write_buffer == NULL)
  {
   MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory");
   return;
  }

  if ((boolean)tm_nv_read_buffer->gps1_xtra_enabled)
  {
    /*Dynamically allocate memory for XTRA state pointer*/
    if (!tm_xtra_init_state_ptr())
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Dynamic memory allocation for XTRA failed. Disabling XTRA feature", 0, 0, 0);
      nv_write_buffer->gps1_xtra_enabled = 0;
      (void)tm_core_nv_write(NV_GPS1_XTRA_ENABLED_I, (void *)nv_write_buffer);
    os_MemFree((void * *) &nv_write_buffer);

      return;
    }
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "XTRA feature disabled", 0, 0, 0);
    return;
  }
  os_MemFree((void * *) &nv_write_buffer);

  xtra_state_ptr = tm_xtra_get_state_ptr();

  if (xtra_state_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "XTRA STATE PTR found NULL", 0, 0, 0);
    return;
  }

 xtra_state_ptr->xtra_enabled = (boolean)tm_nv_read_buffer->gps1_xtra_enabled;
#if 0  //For Xtra Integrity
  xtraint_state_ptr = tm_xtraint_get_state_ptr();
  
  if (xtraint_state_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "XTRAINT STATE PTR found NULL", 0, 0, 0);
    return;
  }
#endif  /* #if 0 */


  /* Number of hours between automatic downloads */
  (void)sm_nv_read(NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I, (void *)tm_nv_read_buffer);
  xtra_state_ptr->download_interval
     = (uint16)tm_nv_read_buffer->gps1_xtra_download_interval;

  /* Number of unsuccessful download attempts before aborting (xtra client) */
  (void)sm_nv_read(NV_GPS1_XTRA_NUM_DOWNLOAD_ATTEMPTS_I, (void *)tm_nv_read_buffer);
  xtra_state_ptr->num_download_attempts
     = (uint8)tm_nv_read_buffer->gps1_xtra_num_download_attempts;

  /* Number of minutes between download attempts */
  (void)sm_nv_read(NV_GPS1_XTRA_TIME_BETWEEN_ATTEMPTS_I, (void *)tm_nv_read_buffer);
  xtra_state_ptr->time_between_attempts
     = (uint8)tm_nv_read_buffer->gps1_xtra_time_between_attempts;

  /* Enable/disable automatic periodic downloads of gpsOneXTRA files */
  (void)sm_nv_read(NV_GPS1_XTRA_AUTO_DOWNLOAD_ENABLED_I, (void *)tm_nv_read_buffer);
  xtra_state_ptr->auto_download_enabled
     = (boolean)tm_nv_read_buffer->gps1_xtra_auto_download_enabled;

#ifdef FEATURE_INTERNAL_USE_FFABUILDS_ONLY
  /* Primary XTRA Server URL */
  (void)sm_nv_read(NV_GPS1_XTRA_PRIMARY_SERVER_URL_I, (void *)tm_nv_read_buffer);
  memscpy((void *)&xtra_state_ptr->primary_data_server_url[0],
          sizeof(xtra_state_ptr->primary_data_server_url),
          (const void *)&tm_nv_read_buffer->gps1_xtra_primary_server_url[0],
          sizeof(tm_nv_read_buffer->gps1_xtra_primary_server_url));

  /* Secondary XTRA Server URL */
  (void)sm_nv_read(NV_GPS1_XTRA_SECONDARY_SERVER_URL_I, (void *)tm_nv_read_buffer);
  memscpy((void *)&xtra_state_ptr->secondary_data_server_url[0],
          sizeof(xtra_state_ptr->secondary_data_server_url),
          (const void *)&tm_nv_read_buffer->gps1_xtra_secondary_server_url[0],
          sizeof(tm_nv_read_buffer->gps1_xtra_secondary_server_url));

  /* Tertiary gpsOneXTRA Server URL */
  (void)sm_nv_read(NV_GPS1_XTRA_TERTIARY_SERVER_URL_I, (void *)tm_nv_read_buffer);
  memscpy((void *)&xtra_state_ptr->tertiary_data_server_url[0],
          sizeof(xtra_state_ptr->tertiary_data_server_url),
          (const void *)&tm_nv_read_buffer->gps1_xtra_tertiary_server_url[0],
          sizeof(tm_nv_read_buffer->gps1_xtra_tertiary_server_url));
#else
  /* Primary XTRA Server URL */
  memscpy((void *)&xtra_state_ptr->primary_data_server_url[0],
          sizeof(xtra_state_ptr->primary_data_server_url),
          (const void *) NV_AAGPS_DEFAULT_XTRA_PRIMARY_SERVER_URL,
          sizeof(NV_AAGPS_DEFAULT_XTRA_PRIMARY_SERVER_URL));

  /* Secondary XTRA Server URL */
  memscpy((void *)&xtra_state_ptr->secondary_data_server_url[0],
          sizeof(xtra_state_ptr->secondary_data_server_url),
          (const void *) NV_AAGPS_DEFAULT_XTRA_SECONDARY_SERVER_URL,
          sizeof(NV_AAGPS_DEFAULT_XTRA_SECONDARY_SERVER_URL));

  /* Tertiary gpsOneXTRA Server URL */
  memscpy((void *)&xtra_state_ptr->tertiary_data_server_url[0],
          sizeof(xtra_state_ptr->tertiary_data_server_url),
          (const void *) NV_AAGPS_DEFAULT_XTRA_TERTIARY_SERVER_URL,
          sizeof(NV_AAGPS_DEFAULT_XTRA_TERTIARY_SERVER_URL));
#endif /* FEATURE_INTERNAL_USE_FFABUILDS_ONLY */


  /*----------------------------------------------------------------------
     Read all XTRA time NV items
  ----------------------------------------------------------------------*/
  /* Enable/Disable XTRA time info */
  (void)sm_nv_read(NV_GPS1_XTRA_TIME_INFO_ENABLED_I, (void *)tm_nv_read_buffer);
  xtra_state_ptr->time_info_enabled
     = (boolean)tm_nv_read_buffer->gps1_xtra_time_info_enabled;

  /* One-way delay failover threshold */
  (void)sm_nv_read(NV_GPS1_XTRA_TIME_INFO_DELAY_THRESH_I, (void *)tm_nv_read_buffer);
  xtra_state_ptr->time_info_oneway_delay_thresh
     = (uint32)tm_nv_read_buffer->gps1_xtra_time_info_delay_thresh;

  /* time uncertainty threshold */
  (void)sm_nv_read(NV_GPS1_XTRA_TIME_INFO_UNC_THRESH_I, (void *)tm_nv_read_buffer);
  xtra_state_ptr->time_info_uncertainty_thresh
     = (uint32)tm_nv_read_buffer->gps1_xtra_time_info_unc_thresh;

#ifdef FEATURE_INTERNAL_USE_FFABUILDS_ONLY
  /* Primary SNTP Server URL */
  (void)sm_nv_read(NV_GPS1_XTRA_PRIMARY_SNTP_SERVER_URL_I, (void *)tm_nv_read_buffer);
  memscpy((void *)&xtra_state_ptr->primary_sntp_server_url[0],
          sizeof(xtra_state_ptr->primary_sntp_server_url),
          (const void *)&tm_nv_read_buffer->gps1_xtra_primary_sntp_server_url[0],
          sizeof(tm_nv_read_buffer->gps1_xtra_primary_sntp_server_url));

  /* Secondary SNTP Server URL */
  (void)sm_nv_read(NV_GPS1_XTRA_SECONDARY_SNTP_SERVER_URL_I, (void *)tm_nv_read_buffer);
  memscpy((void *)&xtra_state_ptr->secondary_sntp_server_url[0],
          sizeof(xtra_state_ptr->secondary_sntp_server_url),
          (const void *)&tm_nv_read_buffer->gps1_xtra_secondary_sntp_server_url[0],
          sizeof(tm_nv_read_buffer->gps1_xtra_secondary_sntp_server_url));

  /* Tertiary SNTP Server URL */
  (void)sm_nv_read(NV_GPS1_XTRA_TERTIARY_SNTP_SERVER_URL_I, (void *)tm_nv_read_buffer);
  memscpy((void *)&xtra_state_ptr->tertiary_sntp_server_url[0],
          sizeof(xtra_state_ptr->tertiary_sntp_server_url),
          (const void *)&tm_nv_read_buffer->gps1_xtra_tertiary_sntp_server_url[0],
          sizeof(tm_nv_read_buffer->gps1_xtra_tertiary_sntp_server_url));
#else
  /* Primary SNTP Server URL */
  memscpy((void *)&xtra_state_ptr->primary_sntp_server_url[0],
          sizeof(xtra_state_ptr->primary_sntp_server_url),
          (const void *) NV_AAGPS_DEFAULT_XTRA_PRIMARY_SNTP_SERVER_URL,
          sizeof(NV_AAGPS_DEFAULT_XTRA_PRIMARY_SNTP_SERVER_URL));

  /* Secondary SNTP Server URL */
  memscpy((void *)&xtra_state_ptr->secondary_sntp_server_url[0],
          sizeof(xtra_state_ptr->secondary_sntp_server_url),
          (const void *) NV_AAGPS_DEFAULT_XTRA_SECONDARY_SNTP_SERVER_URL,
          sizeof(NV_AAGPS_DEFAULT_XTRA_SECONDARY_SNTP_SERVER_URL));

  /* Tertiary SNTP Server URL */
  memscpy((void *)&xtra_state_ptr->tertiary_sntp_server_url[0],
          sizeof(xtra_state_ptr->tertiary_sntp_server_url),
          (const void *) NV_AAGPS_DEFAULT_XTRA_TERTIARY_SNTP_SERVER_URL,
          sizeof(NV_AAGPS_DEFAULT_XTRA_TERTIARY_SNTP_SERVER_URL));
#endif /* FEATURE_INTERNAL_USE_FFABUILDS_ONLY */

#ifdef FEATURE_GNSS_XTRA3
  /* XTRA3 last key index */
  if (sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_XTRA_LAST_XTRA3_KEY_INDEX,
                              sizeof(uint16),
                              (void *)&xtra_state_ptr->xtra3_last_key_index) == FALSE)
  {
    /* if returned false for any reason, force it to default value */
    xtra_state_ptr->xtra3_last_key_index = GNSS_NV_EFS_SM_XTRA_LAST_XTRA3_KEY_INDEX_DEFAULT;
  }
#endif /* FEATURE_GNSS_XTRA3 */

  tm_xtra_init();

  /* UAX3 Config */
  xtra_state_ptr->uax3_config = GNSS_NV_EFS_SM_TM_UAX3_CONFIG_DEFAULT;
  if (sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_TM_UAX3_CONFIG,
                              sizeof(uint32),
                              (void *)&xtra_state_ptr->uax3_config) == FALSE)
  {

    TM_XTRA_MSG_ERROR("Failed to read UAX3  config %d from NV",
                      xtra_state_ptr->uax3_config, 0, 0);

  }
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "UAX3 Config value from NV = %u", xtra_state_ptr->uax3_config);
  /* send the value to MGP */
  mgp_UpdateUax3Config(xtra_state_ptr->uax3_config);
 
#if 0  //For Xtra Integrity
  /* Xtra Integrity Auto Download Enable */
  {
    uint8 u_integ_auto_dl_enable = FALSE;
    xtraint_state_ptr->integ_auto_download_enabled = GNSS_NV_EFS_SM_XTRAINT_AUTO_DOWNLOAD_ENABLE_DEFAULT;
    if (sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_XTRAINT_AUTO_DOWNLOAD_ENABLE,
                                sizeof(uint8),
                                (void *)&u_integ_auto_dl_enable) == FALSE)
    {
      TM_XTRA_MSG_ERROR("Failed to read Integrity auto dl enable %u from NV",
                        u_integ_auto_dl_enable, 0, 0);
    }
    else
    {
      xtraint_state_ptr->integ_auto_download_enabled = (boolean)u_integ_auto_dl_enable;
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Integrity auto dl enable %u from NV", 
                                             xtraint_state_ptr->integ_auto_download_enabled);
    }
  }
  /* Xtra Integrity Download Interval */
  xtraint_state_ptr->integ_download_interval = GNSS_NV_EFS_SM_XTRAINT_DOWNLOAD_INTERVAL_DEFAULT;
  if (sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_XTRAINT_DOWNLOAD_INTERVAL,
                              sizeof(uint16),
                             (void *)&xtraint_state_ptr->integ_download_interval) == FALSE)
  {
    TM_XTRA_MSG_ERROR("Failed to read Integrity dl interval %u from NV",
                      xtraint_state_ptr->integ_download_interval, 0, 0);
  }
  else
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Integrity auto dl enable from NV = %u", 
                                           xtraint_state_ptr->integ_download_interval);
  }
#endif  /* #if 0 */
  

  /* Initialize XTRA module */

  return;
} /* End tm_xtra_read_nv() */
#endif /* FEATURE_CGPS_XTRA */
/*===========================================================================

#9x05: since DD sub isn't needed FUNCTION tm_core_set_current_subs_cfg removed

FUNCTION tm_core_init

DESCRIPTION
  This function is to initialize TM-Core data structures.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_init(void)
{
  int i;
  uint8 sub = 0;
  tm_nv_read_buffer = (nv_item_type*)os_MemAlloc(sizeof(nv_item_type), OS_MEM_SCOPE_TASK);
  if (tm_nv_read_buffer == NULL)
  {
   MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory");
   return;
  }

  /* Read NV settings */
  tm_core_read_nv();

  /* Initialize Leap Second Info */
  memset((void *)&tm_core_info.config_info.z_LeapSecondInfo, 0,
         sizeof(tm_core_info.config_info.z_LeapSecondInfo));
  tm_core_info.config_info.z_LeapSecondInfo.u_DeltaLeapSec = GPS_UTC_OFFSET;
  /* Intialize the uncertainity to 1. When ME updates, this value will have to correct uncertainity */
  tm_core_info.config_info.z_LeapSecondInfo.u_DeltaLeapSecUnc = TM_CORE_LEAP_SEC_INIT_UNC;

  /* Clearing stored Eph expiry time info which sent by PE */
 (void)gnss_DeleteEphAvailStatus(&tm_core_info.z_eph_exp_time);
  
  /* Initialize SARF status to disabled at init */
  tm_core_info.config_info.v_sarf_status = FALSE;

  /* Initialize session info */
  tm_core_info.session_info.active_sess_handle      = TM_SESS_HANDLE_NONE;
  tm_core_info.session_info.op_mode                 = TM_SESS_OPERATION_MODE_NONE;
  tm_core_info.session_info.op_requested            = TM_OP_REQ_NONE;
  tm_core_info.session_info.from_protocol           = TM_PRTL_TYPE_NONE;
  tm_core_info.session_info.pd_param.client_type    = PDSM_LITE_CLIENT_TYPE_NONE;
  tm_core_info.session_info.pd_param.client_id      = -1;
  tm_core_info.session_info.session_event_firing    = TRUE;
  tm_core_info.session_info.mo_sess_handle_seed     = 0;
  tm_core_info.session_info.eph_svmask              = 0;
  tm_core_info.session_info.alm_svmask              = 0;
  tm_core_info.session_info.queued_request          = FALSE;
  tm_core_info.session_info.q_FixTimeRemainingMSec    = 0;
  tm_core_info.session_info.force_eph_download      = FALSE;
  tm_core_info.session_info.glo_force_eph_download  = FALSE;
  tm_core_info.session_info.bds_force_eph_download  = FALSE;
  tm_core_info.session_info.gal_force_eph_download  = FALSE;
  tm_core_info.session_info.eph_need_sv_mask = 0;
  tm_core_info.session_info.glo_eph_need_sv_mask = 0;
  tm_core_info.session_info.bds_eph_need_sv_mask = 0;
  tm_core_info.session_info.gal_eph_need_sv_mask = 0;
  tm_core_info.session_info.sess_info_valid_flags = 0;
  tm_core_info.session_info.receiver_off_needed     = FALSE;
  tm_core_info.session_info.utc                     = 1; /* utc invalid, 1 means MGP has
                                                         requested utc from AGPS */
  tm_core_info.session_info.health_svmask           = 0;
  tm_core_info.session_info.valid_svmask            = 0;
  tm_core_info.session_info.glo_eph_svmask          = 0;
  tm_core_info.session_info.glo_alm_svmask          = 0;
  tm_core_info.session_info.bds_eph_svmask          = 0;
  tm_core_info.session_info.bds_alm_svmask          = 0;

  tm_core_info.session_info.qzss_eph_svmask         = 0;
  tm_core_info.session_info.qzss_alm_svmask         = 0;
  tm_core_info.session_info.gal_eph_svmask          = 0;
  tm_core_info.session_info.gal_alm_svmask          = 0;
  tm_core_info.session_info.v_xtra_info_requested    = FALSE;

  tm_core_info.session_info.q_num_fixes             = 0;
  tm_core_info.w_RecvdMeasBlkMask                   = 0;

  tm_core_info.cs_on_demand_active                  = FALSE;
  tm_core_info.ext_inj_allowed                      = TRUE;


  memset((void *)&z_gnss_position_cache, 0, sizeof(tm_core_gnss_pos_cache_s_type));
#if 0
  memset((void *)&z_cached_ref_pos_info, 0, sizeof(tm_core_cached_ref_pos_s_type));
#endif


  tm_core_info.e_cached_rcvr_state = C_GPS_RCVR_STATE_OFF;

  tm_core_update_session_state(TM_CORE_SESS_STATE_INIT);

  /* Initialize TM-Core / Protocol Interface */
  for (i = 0; i < TM_PRTL_NUM; i++)
  {
    tm_core_info.prtl_func_cb_table[i].init_fp             = NULL;
    tm_core_info.prtl_func_cb_table[i].start_sess_req_fp   = NULL;
    tm_core_info.prtl_func_cb_table[i].stop_sess_req_fp    = NULL;
    tm_core_info.prtl_func_cb_table[i].sess_req_data_fp    = NULL;
    tm_core_info.prtl_func_cb_table[i].sess_info_fp        = NULL;
    tm_core_info.prtl_func_cb_table[i].timer_cb_fp         = NULL;
    tm_core_info.prtl_func_cb_table[i].event_cb_fp         = NULL;
  }

  /* Setup init function for supported protocols */
  tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_STANDALONE].init_fp = tm_standalone_prtl_init;

  tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_DIAG].init_fp       = tm_diag_prtl_init;

  /* Init the GFC_QMI module */
  tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_GFC_QMI].init_fp = tm_gfc_qmi_prtl_init;

#ifdef FEATURE_CGPS_XTRA_T
  /* Init the XTRA-T module */
  tm_core_info.prtl_func_cb_table[TM_CORE_PRTL_CB_ARR_INDEX_XTRA_T].init_fp = tm_xtra_t_init;
#endif

  /* Initialize supported protocol sub-modules */
  for (i = 0; i < TM_PRTL_NUM; i++)
  {
    if (tm_core_info.prtl_func_cb_table[i].init_fp != NULL)
    {
      tm_core_info.prtl_func_cb_table[i].init_fp();
    }
  }
  
#ifdef FEATURE_CGPS_XTRA
  /* Make sure tm_sec_init is called before tm_core_read_security_nv */
  tm_xtra_init_and_read_nv();
  tm_core_info.internal_session = FALSE;
#endif /* FEATURE_CGPS_XTRA */
  tm_core_info.tm_task_info.task_active = TRUE;
  tm_util_get_constellation_info_from_efs(&tm_core_info.q_mgp_curr_gnss_constellation_config, &tm_core_info.u_sm_curr_serving_sec_gnss_constellation);
  /* initialize the delete all throttle timer */
  tm_core_info.tm_delete_param_throttle_timer = os_TimerCreate((uint32)TM_CORE_TIMER_ID_DELETE_THROTTLE_ID, (uint32)THREAD_ID_SM_TM);
  tm_core_info.tm_core_gtp_wwan_fix_timer =  os_TimerCreate( (uint32)TM_CORE_GTP_WWAN_FIX_TIMER_ID,  THREAD_ID_SM_TM);
  tm_core_info.tm_core_gts_sys_unc_wait_timer = os_TimerCreate( TM_CORE_GTS_SYS_UNC_WAIT_TIMER_ID,  THREAD_ID_SM_TM);

#ifdef QWES_FEATURE_ENABLE
  /*By default GNSS service is disabled will be update by CAAS*/
  tm_core_info.config_info.subs_cfgs.gps_lock = PDSM_LITE_GPS_LOCK_ALL;
  /*By default GTP WWAN service is disabled will be updated by CAAS*/
  tm_core_info.config_info.b_isGtpWwanServiceFeatureEnabled = FALSE; 
#else
  /*By default GNSS service is enabled if CAAS is disabled*/
  tm_core_info.config_info.subs_cfgs.gps_lock = PDSM_LITE_GPS_LOCK_NONE;
  /*By default GTP WWAN service is enabled if CAAS is disabled*/
  tm_core_info.config_info.b_isGtpWwanServiceFeatureEnabled = TRUE; 
#endif
  os_MemFree((void * *) &tm_nv_read_buffer);
}

/*===========================================================================

FUNCTION tm_core_get_prtl_idx_by_type

DESCRIPTION
  This function returns protocol array index based on protcol type

DEPENDENCIES

RETURN VALUE
  -1, if protocol is not supported.
  Otherwise, array index for the protocol type

SIDE EFFECTS

===========================================================================*/
int tm_core_get_prtl_idx_by_type(tm_prtl_type prtl_type)
{
  switch (prtl_type)
  {
    case TM_PRTL_TYPE_STANDALONE:
      return TM_CORE_PRTL_CB_ARR_INDEX_STANDALONE;
    case TM_PRTL_TYPE_DIAG:
      return TM_CORE_PRTL_CB_ARR_INDEX_DIAG;

    case TM_PRTL_TYPE_GFC_QMI:
      return TM_CORE_PRTL_CB_ARR_INDEX_GFC_QMI;

#ifdef FEATURE_CGPS_XTRA_T
    case TM_PRTL_TYPE_XTRA_T:
      return TM_CORE_PRTL_CB_ARR_INDEX_XTRA_T;
#endif

    default:
      return -1;
  }
}
/*===========================================================================

FUNCTION tm_core_new_mo_sess_handle

DESCRIPTION
  This function generates a new MO session handle for incoming MO fix request.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
tm_sess_handle_type tm_core_new_mo_sess_handle(void)
{
  tm_sess_handle_type mo_sess_handle;

  tm_core_info.session_info.mo_sess_handle_seed %= 8;

  mo_sess_handle = 1 << tm_core_info.session_info.mo_sess_handle_seed;

  tm_core_info.session_info.mo_sess_handle_seed++;

  return mo_sess_handle;
}
extern os_TimerMsgType* tm_core_trial_timer_20;
/*===========================================================================

FUNCTION tm_core_timer_handler

DESCRIPTION
  This function is to figure out which TM-Core timer has expired and call proper
  timer handler.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_timer_handler(const os_TimerExpiryType *timer_data)
{
  uint32 timer_dur = 0;
  if (timer_data != NULL)
  {
    switch (timer_data->q_TimerId)
    {
      
      case TM_CORE_TIMER_ID_DELETE_THROTTLE_ID:
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "DELETE timer expired", 0, 0, 0);
        break;

      case TM_CORE_GTP_WWAN_FIX_TIMER_ID:
          tm_core_handle_gtp_wwan_fix_timer_expire();
        break;
      case TM_CORE_GTS_SYS_UNC_WAIT_TIMER_ID:
         tm_core_handle_gts_sys_unc_wait_timer_expire();
        break;
      default:
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Unknown timer expired %u", timer_data->q_TimerId);
        break;
    }
  }
  else /* null pointer check */
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Can not proess null timer msg");
  }
}

/*===========================================================================

FUNCTION tm_core_get_prtl_idx_by_timer_id

DESCRIPTION
  This function returns protocol array index based on timer ID

DEPENDENCIES

RETURN VALUE
  -1, if timer ID can not be found.
  Otherwise, protocol array index for the timer ID.

SIDE EFFECTS

===========================================================================*/
int tm_core_get_prtl_idx_by_timer_id(uint32 timer_id)
{
  switch (timer_id & TM_CORE_TIMER_DISPATCH_MASK)
  {
    case TM_STANDALONE_TIMER_ID_RANGE:
      return TM_CORE_PRTL_CB_ARR_INDEX_STANDALONE;

    case TM_DIAG_TIMER_ID_RANGE:
      return TM_CORE_PRTL_CB_ARR_INDEX_DIAG;

    default:
      return -1;
  }
}

/*===========================================================================

FUNCTION tm_core_timer_dispatcher

DESCRIPTION
  This function is to figure out which TM software sub-module the timer expiry event
  belongs to and call the proper timer handler (provided to TM-Core by each sub-module)

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_timer_dispatcher(void *timer_data)
{
  os_TimerExpiryType *timer_param = (os_TimerExpiryType *)timer_data;
  int prtlTableIndex;

  
  if (timer_param != NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_CORE Timer with Timer id = %d expired ",timer_param->q_TimerId , 0, 0);
    switch (timer_param->q_TimerId & TM_CORE_TIMER_DISPATCH_MASK)
    {
      case TM_CORE_TIMER_ID_RANGE:
        tm_core_timer_handler(timer_param);
        break;

      case TM_PDAPI_TIMER_ID_RANGE:
        //  pdApi_TimerHandler(timer_param);
        break;

#ifdef FEATURE_CGPS_XTRA
      case TM_XTRA_TIMER_ID_RANGE:
        tm_xtra_handle_timer_expiry(timer_param);
        break;
#endif /* FEATURE_CGPS_XTRA */

      case TM_STANDALONE_TIMER_ID_RANGE:
      case TM_DIAG_TIMER_ID_RANGE:
        /* Timer for protocol sub-modules */
        prtlTableIndex = tm_core_get_prtl_idx_by_timer_id(timer_param->q_TimerId);

        if ((prtlTableIndex >= 0) && (prtlTableIndex < TM_PRTL_NUM))
        {
          if (tm_core_info.prtl_func_cb_table[prtlTableIndex].timer_cb_fp != NULL)
          {
            tm_core_info.prtl_func_cb_table[prtlTableIndex].timer_cb_fp(timer_data);
          }
        }
        break;

      default:
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Can not find proper timer handler", 0, 0, 0);
        break;
    }
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Can not dispatch null timer msg", 0, 0, 0);
  }
  return;
}
/*===========================================================================

FUNCTION tm_core_session_active

DESCRIPTION
  This function tells if Tm-Core is in any active PD session (MI or MT).

  If there is an active PD on-demand session, we return FALSE

  If there is an active Periodic PPM session, we return FALSE

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_session_active(void)
{
  /* check if there is a GPS session or on-demand session is active*/
  if (tm_core_info.session_info.session_state == TM_CORE_SESS_STATE_INIT)
  {
    return FALSE;
  }
  else
  {
    return TRUE; /*lint !e506 */
  }
}

/*===========================================================================

FUNCTION tm_core_session_ok_to_start_new_session

DESCRIPTION
  This function tells if Tm-Core is willing to start a new PD session

  If there is an already existing session, this function will return FALSE
  EXCEPT in the case of:~
    If there is an active PD on-demand session, we return FALSE
    If there is an active Periodic PPM session, we return FALSE

  A PD session will always preempt these two types of sessions.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_session_ok_to_start_new_session(void)
{
  /* check if there is a GPS session or on-demand session is active*/
  if (tm_core_info.session_info.session_state == TM_CORE_SESS_STATE_INIT)
  {
    return TRUE;
  }
  else
  {
    return FALSE; /*lint !e506 */
  }
}


/*===========================================================================

FUNCTION tm_core_translate_end_status

DESCRIPTION
  This function translate internal abort reason to external end status.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_lite_pd_end_e_type tm_core_translate_end_status
(
   tm_sess_stop_reason_e_type  abort_reason
   )
{
  pdsm_lite_pd_end_e_type ret_val = PDSM_LITE_PD_END_SESS_NONE;

  switch (abort_reason)
  {
    case TM_STOP_REASON_USER_TERMINATED:
      ret_val = PDSM_LITE_PD_END_CLIENT_END;
      break;
    case TM_STOP_REASON_FIX_TIMEOUT:
      ret_val = PDSM_LITE_PD_END_TIMEOUT;
      break;
    case TM_STOP_REASON_GENERAL_ERROR:
	case TM_STOP_REASON_GNSS_RANDOM_RCVR_OFF_ERROR:
      ret_val = PDSM_LITE_PD_END_FIX_ERROR;
      break;
    case TM_STOP_REASON_GNSS_SERVICE_NOT_AVAILABLE:
      ret_val = PDSM_LITE_PD_END_GNSS_SRVC_NOT_AVAILABLE;
	  break;
    default:
      ret_val = PDSM_LITE_PD_END_SESS_NONE;
      break;
  }
  return ret_val;
}

/*===========================================================================

FUNCTION tm_core_inform_pd_event

DESCRIPTION
  This function report PD event

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void tm_core_inform_pd_event(prtl_event_cb_f_type *p_event_cb_fp, pdsm_lite_pd_event_type const cpd_event)
{
  prtl_event_u_type event_payload;

  if (p_event_cb_fp != NULL)
  {
    event_payload.pd_event_payload.pd_event = cpd_event;
    p_event_cb_fp(PRTL_EVENT_TYPE_PD, &event_payload);
  }
}

/*===========================================================================

FUNCTION tm_core_req_lm_recvr_off

DESCRIPTION
  This function request LM to turn off the receiver

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void tm_core_req_lm_recvr_off()
{
  lm_request_info_s_type    lm_info_req_param;

    lm_info_req_param.e_req_type = LM_REQUEST_RECVR_OFF;
  (void)lm_request(&lm_info_req_param);

}

/*===========================================================================

FUNCTION tm_core_lm_sess_stop_req

DESCRIPTION
  This function request LM to stop the session

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void tm_core_lm_sess_stop_req(tm_sess_handle_type const active_sess_handle,
                                     uint32 q_req_mask,  boolean const cv_force_mgp_off)
{
  lm_session_request_info_s_type lm_sess_req_param;

  lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_STOP;
  lm_sess_req_param.q_handle   = active_sess_handle;
  lm_sess_req_param.q_req_mask = q_req_mask;

  if (LM_SESSION_REQUEST_FIX == lm_sess_req_param.q_req_mask)
  {
    lm_sess_req_param.z_request.z_fix_request.u_force_mgp_off = cv_force_mgp_off;
  }
  lm_session_request(&lm_sess_req_param);
}

/*===========================================================================

FUNCTION tm_core_abort_recover

DESCRIPTION
  This function performs TM-Core software recovery.

DEPENDENCIES

RETURN VALUE
    None

SIDE EFFECTS

===========================================================================*/
void tm_core_abort_recover
(
   boolean                       ask_prtl,
   tm_sess_stop_reason_e_type    abort_reason
   )
{
  int                            prtlTableIndex = -1;
  boolean                        prtl_abort_session = TRUE;
  pdsm_lite_client_id_type            client_id;
  tm_pdapi_pd_event_s_type       *pz_pd_event_type = NULL;
  boolean                        fire_event;
  pdsm_lite_client_type_e_type           client_type;
  tm_cm_phone_state_info_s_type phone_state_info;
  pdsm_lite_pd_end_e_type            e_end_status = PDSM_LITE_PD_END_SESS_NONE;


  MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Abort recover called");

  /* Init */
  pz_pd_event_type = (tm_pdapi_pd_event_s_type *)os_MemAlloc(sizeof(tm_pdapi_pd_event_s_type), OS_MEM_SCOPE_TASK);

  if (NULL == pz_pd_event_type)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Falling back to stack since memalloc fail for tm_pdapi_pd_event_s_type in tm_core_abort_recover");
  }
  else
  {
    memset(pz_pd_event_type, 0, sizeof(*pz_pd_event_type));
  }
  tm_cm_iface_get_phone_state(&phone_state_info);

#ifdef FEATURE_CGPS_XTRA
  /* Handle the abort reason for XTRA sessions */
  tm_xtra_handle_core_abort_recover(abort_reason);
#endif /* FEATURE_CGPS_XTRA */

  /* Check if tm_core is in active state or if on-demand session is on or if we got a set params
  for delete parms abort the session */
  if (tm_core_session_active())
  {
    prtlTableIndex = tm_core_get_prtl_idx_by_type(tm_core_info.session_info.from_protocol);

    if (((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM)))
    {

      /* If an abort request need a protocol sub-modules' consent, only proceed if a protocol sub-moulde allows it */
      if (ask_prtl)
      {
        /* Set reason to general error for now, if return value */
        if (tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp)
        {
        
          prtl_abort_session =
             tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp(
             tm_core_info.session_info.active_sess_handle,
             PRTL_SESS_STOP_SINGLE_FIX,
             abort_reason);

          if (prtl_abort_session == TRUE)
          {
            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Protocol aborted the session", 0, 0, 0);
          }
          else
          {
            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Protocol continue with the session", 0, 0, 0);
          }
        }
        else
        {
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "No protocol callback installed 2", 0, 0, 0);
        }
      }  /* if(ask_prtl == TRUE)  */
      else
      {
        tm_core_update_session_state(TM_CORE_SESS_STATE_RESOURCE_CLEANUP);
        fire_event = tm_core_info.session_info.session_event_firing;
        if (tm_core_is_internal_session_on())
        {
          fire_event = FALSE;
          /* Notify the GM if the HLOS has ended the session ( USER_TERMINATED ) or
             if the HLOS has started a new session in which case the internal session is terminated (ABORT_INTERNAL_SESSION)*/
          if ((abort_reason == TM_STOP_REASON_USER_TERMINATED) ||
              (abort_reason == TM_STOP_REASON_ABORT_INTERNAL_SESSION) ||
              (abort_reason == TM_STOP_REASON_INTERNAL_SESSION_DELETE_PARAM))
          {
            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Sending abort reason %d to GM ", abort_reason, 0, 0);
            gm_position_fix_request_fail(GEOFENCE_POS_FIX_SRC_GNSS);
          }
        }
		
        if (NULL != pz_pd_event_type)
        {
          pz_pd_event_type->pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
          pz_pd_event_type->pd_event_data.pd_info_type.pd_info.end_status = PDSM_LITE_PD_END_SESS_NONE;
          /* Fire End event if fix does not complete successfully */
          if( (abort_reason != TM_STOP_REASON_COMPLETED) &&
              (fire_event == TRUE))
          {
            pz_pd_event_type->e_pd_event = PDSM_LITE_PD_EVENT_END;
            pz_pd_event_type->pd_event_data.pd_info_type.pd_info.end_status = tm_core_translate_end_status(abort_reason);
            tm_pdapi_pd_event_callback(pz_pd_event_type, TM_PDAPI_SESS_TYPE_NONE);
          }

          e_end_status = pz_pd_event_type->pd_event_data.pd_info_type.pd_info.end_status;
        }
        else
        {
          tm_pdapi_pd_event_s_type       z_pd_event_type = {0};

          z_pd_event_type.pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
          z_pd_event_type.pd_event_data.pd_info_type.pd_info.end_status = PDSM_LITE_PD_END_SESS_NONE;
          
          /* Fire End event if fix does not complete successfully */
          if( (abort_reason != TM_STOP_REASON_COMPLETED) &&
               (fire_event == TRUE)
             )
          {
            z_pd_event_type.e_pd_event = PDSM_LITE_PD_EVENT_END;
            z_pd_event_type.pd_event_data.pd_info_type.pd_info.end_status = tm_core_translate_end_status(abort_reason);
            tm_pdapi_pd_event_callback(&z_pd_event_type, TM_PDAPI_SESS_TYPE_NONE);
          }
          e_end_status = z_pd_event_type.pd_event_data.pd_info_type.pd_info.end_status;
        }

        client_type = pdsmclient_lite_get_client_type_map(tm_core_info.session_info.pd_param.client_id);

        /* if abort reason is delete parms, check if on demand is enabled. we need to force the receiver
        to go off before honoring the delete parms.
        if the user terminated the session, check if the client type is OEM. we
        need to turn off the rcvr off immediately */
        if (abort_reason == TM_STOP_REASON_USER_TERMINATED)
        {
          tm_core_lm_sess_stop_req(tm_core_info.session_info.active_sess_handle, LM_SESSION_REQUEST_FIX | LM_SESSION_REQUEST_SESSION, TRUE);
        }
        /* Below is redundent but will be useful in future to handle seperately */
        else if((abort_reason == TM_STOP_REASON_GNSS_SERVICE_NOT_AVAILABLE ) || (abort_reason == TM_STOP_REASON_GNSS_RANDOM_RCVR_OFF_ERROR) )
        {
           /*If GNSS service is not available then clear session as well as turn the receiver off*/
           tm_core_lm_sess_stop_req(tm_core_info.session_info.active_sess_handle, LM_SESSION_REQUEST_SESSION | LM_SESSION_REQUEST_FIX , TRUE);
        }
        else
        {
           tm_core_lm_sess_stop_req(tm_core_info.session_info.active_sess_handle, LM_SESSION_REQUEST_SESSION, FALSE);
        }
        /* Fire EVENT_GPS_PD_FIX_END */
        sm_report_event_gps_fix_end(e_end_status);

        client_id  = tm_core_info.session_info.pd_param.client_id;


        /* Go back to clean state */
        tm_core_info.session_info.active_sess_handle      = TM_SESS_HANDLE_NONE;
        tm_core_info.session_info.op_mode                 = TM_SESS_OPERATION_MODE_NONE;
        tm_core_info.session_info.op_requested            = TM_OP_REQ_NONE;
        tm_core_info.session_info.from_protocol           = TM_PRTL_TYPE_NONE;
        tm_core_info.session_info.pd_param.client_type    = PDSM_LITE_CLIENT_TYPE_NONE;
        tm_core_info.session_info.pd_param.client_id      = -1;
        tm_core_info.session_info.session_event_firing    = TRUE;
        tm_core_info.session_info.eph_svmask              = 0;
        tm_core_info.session_info.alm_svmask              = 0;
        tm_core_info.session_info.force_eph_download      = FALSE;
        tm_core_info.session_info.glo_force_eph_download  = FALSE;
        tm_core_info.session_info.bds_force_eph_download  = FALSE;
        tm_core_info.session_info.gal_force_eph_download  = FALSE;
        tm_core_info.session_info.eph_need_sv_mask = 0;
        tm_core_info.session_info.glo_eph_need_sv_mask = 0;
        tm_core_info.session_info.bds_eph_need_sv_mask = 0;
        tm_core_info.session_info.gal_eph_need_sv_mask = 0;
        tm_core_info.session_info.sess_info_valid_flags = 0;

        tm_core_info.session_info.q_ReqClientMask         = SM_GPS_STATE_INFO_REQ_TYPE_NONE;
        tm_core_info.session_info.receiver_off_needed     = FALSE;
        tm_core_info.session_info.utc                     = 1; /* utc invalid, 1 means MGP has
                                                               requested utc from AGPS */
        tm_core_info.session_info.health_svmask           = 0;
        tm_core_info.session_info.valid_svmask            = 0;
        tm_core_info.session_info.glo_eph_svmask          = 0;
        tm_core_info.session_info.glo_alm_svmask          = 0;
        tm_core_info.session_info.bds_eph_svmask          = 0;
        tm_core_info.session_info.bds_alm_svmask          = 0;
        tm_core_info.session_info.gal_eph_svmask          = 0;
        tm_core_info.session_info.gal_alm_svmask          = 0;
        tm_core_info.session_info.q_num_fixes             = 0;
        tm_core_info.session_info.v_xtra_info_requested    = FALSE;

        tm_core_info.internal_session = FALSE;

        tm_core_update_session_state(TM_CORE_SESS_STATE_INIT);

        /* Fire Done event */
        if (NULL != pz_pd_event_type)
        {
          if (TRUE == fire_event)
          {
            pz_pd_event_type->pd_event_data.pd_info_type.client_id = client_id;
            {
              pz_pd_event_type->e_pd_event = PDSM_LITE_PD_EVENT_DONE;
              tm_pdapi_pd_event_callback(pz_pd_event_type, TM_PDAPI_SESS_TYPE_NONE);

              /* Check prtl_func_cb_table bounds */
              if ((prtlTableIndex >= 0) && (prtlTableIndex < TM_PRTL_NUM))
              {
                /* This is added for internal Vx NI client */
                if (NULL != tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp)
                {
                  tm_core_inform_pd_event(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp,
                                          PDSM_LITE_PD_EVENT_DONE);
                }
              }
              else
              {
                MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Attempt for an out of bound access to prtl_func_cb_table with index = %d", prtlTableIndex, 0, 0);
              }
            }

          }
          if ((TRUE == fire_event) ||
              (TM_STOP_REASON_INTERNAL_SESSION_DELETE_PARAM == abort_reason)
              )
          {
            /*Send Done event to GM core as well*/
            pz_pd_event_type->pd_event_data.pd_info_type.client_id = client_id;
            pz_pd_event_type->e_pd_event = PDSM_PD_EVENT_DONE;
            gm_update_pd_event(pz_pd_event_type->e_pd_event, abort_reason);
          }
        }
        else 
        {
          tm_pdapi_pd_event_s_type       z_pd_event_type = { 0 };

          if (TRUE == fire_event)
          {
            z_pd_event_type.pd_event_data.pd_info_type.client_id = client_id;
            z_pd_event_type.e_pd_event = PDSM_LITE_PD_EVENT_DONE;
            tm_pdapi_pd_event_callback(&z_pd_event_type, TM_PDAPI_SESS_TYPE_NONE);

          }
          if ((fire_event == TRUE) ||
              (abort_reason == TM_STOP_REASON_INTERNAL_SESSION_DELETE_PARAM)
              )
          {
            /*Send Done event to GM core as well*/
            z_pd_event_type.pd_event_data.pd_info_type.client_id = client_id;
            z_pd_event_type.e_pd_event = PDSM_PD_EVENT_DONE;
            gm_update_pd_event(z_pd_event_type.e_pd_event, abort_reason);
          }
        }

        if (tm_core_info.session_info.queued_request == TRUE)
        {
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Resume pending get position fix request", 0, 0, 0);
          (void)tm_core_resume_get_pos();
        }
      } /* If(ask_prtl == FALSE) */
    }
    else
    {
      /* When there is no protocol then handle non gnss session complete*/
	  if (NULL != pz_pd_event_type)
	  {
       pz_pd_event_type->pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
       pz_pd_event_type->pd_event_data.pd_info_type.pd_info.end_status = tm_core_translate_end_status(abort_reason);
       tm_core_handle_non_gnss_session_complete(&(pz_pd_event_type->pd_event_data.pd_info_type), TM_PDAPI_SESS_TYPE_NONE);
    }
	  else
	  {
        tm_pdapi_pd_event_s_type       z_pd_event_type = { 0 };
		z_pd_event_type.pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
		z_pd_event_type.pd_event_data.pd_info_type.pd_info.end_status = tm_core_translate_end_status(abort_reason);
        tm_core_handle_non_gnss_session_complete(&(z_pd_event_type.pd_event_data.pd_info_type), TM_PDAPI_SESS_TYPE_NONE);
  }
  }
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TM-Core not in active state", 0, 0, 0);
  
    for (prtlTableIndex = 0; prtlTableIndex < TM_CORE_PRTL_CB_ARR_INDEX_MAX; prtlTableIndex++)
    {
        /* If an abort request is coming from the user, send it to protocol modules */
       if (abort_reason == TM_STOP_REASON_USER_TERMINATED )
        {
            /* Set reason to general error for now, if return value */
            if (tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp)
            {

              prtl_abort_session = tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp(tm_core_info.session_info.active_sess_handle,
                                                        PRTL_SESS_STOP_SINGLE_FIX,
                                                        abort_reason);


              if (prtl_abort_session == TRUE)
              {
                MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Protocol aborted the session", 0, 0, 0);
              }
              else
              {
                MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Protocol continue with the session", 0, 0, 0);
              }
            }
            else
            {
              MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "No protocol callback installed 3", 0, 0, 0);
            }
        }
    }
    if (abort_reason == TM_STOP_REASON_USER_TERMINATED )
    {
        /* inform LM to turn off the receiver if user sent an end session */
        /*Do Force Receiver off only if OEM client. Else to normal receiver off*/
        tm_core_req_lm_recvr_off();

        if (NULL != pz_pd_event_type)
        {
          pz_pd_event_type->pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
          pz_pd_event_type->e_pd_event = PDSM_LITE_PD_EVENT_END;
          pz_pd_event_type->pd_event_data.pd_info_type.pd_info.end_status = tm_core_translate_end_status(abort_reason);
          tm_pdapi_pd_event_callback(pz_pd_event_type, TM_PDAPI_SESS_TYPE_NONE);
        }
        else
        {
          tm_pdapi_pd_event_s_type     z_pd_event_type = { 0 };
          z_pd_event_type.pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
          z_pd_event_type.e_pd_event = PDSM_LITE_PD_EVENT_END;
          z_pd_event_type.pd_event_data.pd_info_type.pd_info.end_status = tm_core_translate_end_status(abort_reason);
          tm_pdapi_pd_event_callback(&z_pd_event_type, TM_PDAPI_SESS_TYPE_NONE);
        }
    }

    /* Go back to clean state */
    tm_core_info.session_info.active_sess_handle      = TM_SESS_HANDLE_NONE;
    tm_core_info.session_info.op_mode                 = TM_SESS_OPERATION_MODE_NONE;
    tm_core_info.session_info.op_requested            = TM_OP_REQ_NONE;
    tm_core_info.session_info.from_protocol           = TM_PRTL_TYPE_NONE;
    tm_core_info.session_info.pd_param.client_type    = PDSM_LITE_CLIENT_TYPE_NONE;
    tm_core_info.session_info.pd_param.client_id      = -1;
  //tm_core_info.session_info.session_event_firing    = TRUE;
    tm_core_info.session_info.eph_svmask              = 0;
    tm_core_info.session_info.alm_svmask              = 0;
    tm_core_info.session_info.q_ReqClientMask         = SM_GPS_STATE_INFO_REQ_TYPE_NONE;
    tm_core_info.session_info.receiver_off_needed     = FALSE;
    tm_core_info.session_info.end_odp_session         = FALSE;
    tm_core_info.session_info.force_eph_download      = FALSE;
    tm_core_info.session_info.glo_force_eph_download  = FALSE;
    tm_core_info.session_info.bds_force_eph_download  = FALSE;
    tm_core_info.session_info.gal_force_eph_download  = FALSE;
    tm_core_info.session_info.eph_need_sv_mask = 0;
    tm_core_info.session_info.glo_eph_need_sv_mask = 0;
    tm_core_info.session_info.bds_eph_need_sv_mask = 0;
    tm_core_info.session_info.gal_eph_need_sv_mask = 0;
    tm_core_info.session_info.sess_info_valid_flags = 0;
    tm_core_info.session_info.q_num_fixes             = 0;
    tm_core_info.internal_session = FALSE;
  
    tm_core_update_session_state(TM_CORE_SESS_STATE_INIT);
  }

  if (NULL != pz_pd_event_type)
  {
    (void)os_MemFree((void **)&pz_pd_event_type);
  }
}


/*===========================================================================

FUNCTION tm_core_convert_op_mode

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
lm_gps_mode_e_type tm_core_convert_op_mode(tm_sess_operation_mode_e_type op_mode,
                                           tm_prtl_type                  prtl_type)
{

  tm_cm_phone_state_info_s_type phone_state_info;
  cgps_SrvSystemType selected_srv_system = CGPS_SRV_SYS_OTHER;

  /* Query TM-CM module to get current acquired system */
  tm_cm_iface_get_phone_state(&phone_state_info);
    selected_srv_system = phone_state_info.srv_system;

  switch (op_mode)
  {
    case TM_SESS_OPERATION_MODE_STANDALONE:
      return LM_GPS_MODE_STANDALONE;

    default:
      return LM_GPS_MODE_NONE;
  }
}

/*TODO: [DEL] #9x05: Removing this function as the position report is directly
               being converted to qmi and so this function will no longer be required
              Following functions are being removed
              tm_core_fix_time_get
              tm_core_date_get
              tm_core_pos_get
              tm_core_velocity_get
              tm_core_velocity_get
              tm_core_generate_ext_pd_data
               */





/*===========================================================================

FUNCTION tm_core_sess_req_start

DESCRIPTION
  This function is to figure out if TM-Core accepts session start request from
  protocol sub-module.

DEPENDENCIES

RETURN VALUE
  true  - if the session was started successfully
  false - otherwise

SIDE EFFECTS

===========================================================================*/
boolean tm_core_sess_req_start(tm_prtl_type                    prtl_type,
                               tm_sess_handle_type             sess_handle,
                               tm_sess_req_start_param_s_type  sess_start_param)
{
  boolean retVal = FALSE;
  lm_session_request_info_s_type       lm_sess_req_param;
  lm_request_info_s_type               lm_info_req_param;
  sm_event_pd_fix_start_payload_e_type dm_op_mode;
  tm_cm_phone_state_info_s_type        phone_state_info;
  pdsm_lite_delete_parms_type               delete_param;
  FLT f_punc;
  FLT f_tunc;
  lm_info_s_type                       lm_info_data;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core entering function [tm_core_sess_req_start]  sess_handle is %d, prtl is %d", sess_handle, prtl_type, 0);

  memset((void *)&lm_sess_req_param, 0, sizeof(lm_session_request_info_s_type));
  memset((void *)&lm_info_req_param, 0, sizeof(lm_request_info_s_type));
  tm_cm_iface_get_phone_state(&phone_state_info);

  /* Accept requests when there is no active session, by starting a new LM-TM
   * session */
  if ((tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_NONE) )
  {
    /* TM-Core accepts the incoming request, so update TM-Core session info */
    tm_core_info.session_info.active_sess_handle = sess_handle;

    tm_core_info.session_info.from_protocol      = prtl_type;
    tm_core_info.session_info.op_mode            = sess_start_param.op_mode;


    /*
      Now request session start to LM
      If life is good, LM should accept it.
      If there is something wrong, LM rejects it (asynchronously), we need to make sure we do recovery properly
    */
    lm_sess_req_param.q_handle = sess_handle;
    lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_START;
    lm_sess_req_param.q_req_mask = (uint32)LM_SESSION_REQUEST_SESSION;
    lm_sess_req_param.z_request.z_session_request.e_mode = tm_core_convert_op_mode(sess_start_param.op_mode, prtl_type);
     lm_sess_req_param.z_request.z_session_request.q_num_fixes = sess_start_param.num_fixes;
    lm_sess_req_param.z_request.z_session_request.q_tbf = sess_start_param.tbf_ms;

    tm_core_info.session_info.q_num_fixes = lm_sess_req_param.z_request.z_session_request.q_num_fixes;
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Setting LM num_fixes = %u", tm_core_info.session_info.q_num_fixes);

    /* Check to see if NHz operating mode is supported.
       If it is not, restrict the TBF to 1000ms minimum 
    */
    if (!(tm_core_info.config_info.gnss_nhz_config & GNSS_NHZ_ENABLED)
        && (sess_start_param.tbf_ms < TM_CORE_NOMINAL_TBF_MS))
    {
      lm_sess_req_param.z_request.z_session_request.q_tbf = TM_CORE_NOMINAL_TBF_MS;
    }

      lm_sess_req_param.z_request.z_session_request.u_do_data_demod = TRUE;

    if (sess_start_param.op_req & TM_OP_REQ_LR)
    {
      lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_FIX;
      lm_sess_req_param.z_request.z_fix_request.q_timeout = sess_start_param.lr_qos * 1000;
      lm_sess_req_param.z_request.z_fix_request.q_qos_accuracy_threshold  = sess_start_param.accuracy_threshold;
      lm_sess_req_param.z_request.z_fix_request.u_dynamic_qos_enabled  = sess_start_param.dynamic_qos_enabled;
    }

  /* for MO OTDOA no need to start LM session */
    if (sess_start_param.op_mode == TM_SESS_OPERATION_MODE_STANDALONE)
    {
      retVal = lm_session_request(&lm_sess_req_param);
      tm_core_info.session_info.op_requested |= sess_start_param.op_req;
    retVal = TRUE;
    }
    
    /* Fire Evt PD_FIX start, req Aiding Data Wishlist, Ref Loc, and start the
     * diag pos timer only if session is not PERIODIC_PPM */
    if (retVal == TRUE) /*lint !e506 !e731 */
    {
      /* Fire EVENT_GPS_PD_FIX_START */
      sm_report_event_gps_fix_start( tm_core_event_counter++, MS_BASED_OP );

      if (sess_start_param.op_req & TM_OP_REQ_REF_LOC)
      {
        lm_info_req_param.e_req_type = LM_REQUEST_POS_ESTIMATE;
        retVal = lm_request(&lm_info_req_param);
      }
      tm_core_info.session_info.q_FixTimeRemainingMSec = tm_core_info.session_info.pd_param.pd_qos.gps_session_timeout * 1000;
#ifdef FEATURE_CGPS_XTRA_T
      if (sess_start_param.op_mode == TM_SESS_OPERATION_MODE_STANDALONE)
      {
        /* TODO : ahmad to add new TLE API call here! */
      }
#endif
    }

    if (retVal == FALSE)
    {
      /* LM does not allow the request, abort and recover */
      tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
    }
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Reject start req because TM-Core already is in another session", 0, 0, 0);
    ERR_FATAL(" Going to crash %d %d",tm_core_info.session_info.active_sess_handle,tm_core_info.session_info.session_state ,0 ); 
    retVal = FALSE;
  }

  return (retVal);
}
/*===========================================================================

FUNCTION tm_core_sess_req_continue

DESCRIPTION
  This function is to figure out if TM-Core accepts session continue request from
  protocol sub-module.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_sess_req_continue(tm_prtl_type                       prtl_type,
                                  tm_sess_handle_type                sess_handle,
                                  tm_sess_req_continue_param_s_type  sess_continue_param)
{
  boolean retVal = TRUE;
  lm_session_request_info_s_type lm_sess_req_param;
  lm_request_info_s_type         lm_info_req_param;
  tm_op_req_type                 new_op_req;
  tm_op_req_type                 existing_op_req;
  tm_cm_phone_state_info_s_type phone_state_info;
  int                            prtl_tbl_idx = -1;
  tm_sess_req_start_param_s_type z_sess_start_req_param;
  pdsm_lite_client_id_type            pd_client_id;

  (void)prtl_type;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM-Core Recv'd Sess Continue Req: %lu, %lu, %u", prtl_type, sess_handle, tm_core_info.session_info.session_state);

  memset((void *)&lm_sess_req_param, 0, sizeof(lm_session_request_info_s_type));
  memset((void *)&lm_info_req_param, 0, sizeof(lm_request_info_s_type));
  memset((void *)&phone_state_info,  0, sizeof(tm_cm_phone_state_info_s_type));
  memset((void *)&z_sess_start_req_param, 0, sizeof(tm_sess_req_start_param_s_type));

  /* Query TM-CM module to get current phone status */
  tm_cm_iface_get_phone_state(&phone_state_info);

  
  /*
     This is the case where protocol sub-modules need to perform authetication procedure
     Protocol sub-modules finished authenticaion process and now is ready to kick off PD session
     At this time, end-to-end session to LM has NOT been established yet
  */

  if ((sess_handle == tm_core_info.session_info.active_sess_handle) &&
      (tm_core_info.session_info.session_state == TM_CORE_SESS_STATE_WAIT_AUTH))
  {
    tm_core_update_session_state(TM_CORE_SESS_STATE_ACCESS_GRANTED);

    /* Update information in TM-Core */
    tm_core_info.session_info.op_mode            = sess_continue_param.op_mode;

    /*
      Now request session start to LM
      If life is good, LM should accept it.
      If there is something wrong, LM rejects it (asynchronously), we need to make sure we do recovery properly
    */

    lm_sess_req_param.q_handle = sess_handle;
    /*
     Since smart MSB is supported for 1x non trusted, we send REQUEST_CONTINUE for the MSB case and
     REQUEST_START for everything else.
    */
    {
      lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_START;
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Setting continue to START: mode=%u",
            tm_core_info.session_info.pd_param.pd_option.operation_mode);
    }
    lm_sess_req_param.q_req_mask = (uint32)LM_SESSION_REQUEST_SESSION;
    lm_sess_req_param.z_request.z_session_request.e_mode = tm_core_convert_op_mode(sess_continue_param.op_mode, prtl_type);
     lm_sess_req_param.z_request.z_session_request.q_num_fixes = sess_continue_param.num_fixes;
    lm_sess_req_param.z_request.z_session_request.q_tbf = sess_continue_param.tbf_ms;

    tm_core_info.session_info.q_num_fixes = lm_sess_req_param.z_request.z_session_request.q_num_fixes;
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "GERA:Setting LM num_fixes = %u",
          tm_core_info.session_info.q_num_fixes);\
    if (sess_continue_param.op_req & TM_OP_REQ_LR)
    {
      lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_FIX;
      lm_sess_req_param.z_request.z_fix_request.q_timeout = sess_continue_param.lr_qos * 1000;
      lm_sess_req_param.z_request.z_fix_request.q_qos_accuracy_threshold  = sess_continue_param.accuracy_threshold;
      lm_sess_req_param.z_request.z_fix_request.u_dynamic_qos_enabled  = sess_continue_param.dynamic_qos_enabled;
    }
    lm_sess_req_param.z_request.z_session_request.u_do_data_demod = TRUE;
    retVal = lm_session_request(&lm_sess_req_param);
    /*
     For the MSB case, the op_requested field is populated in the next block instead of here.
     We do so because the next block expects the tm_core_info.session_info.op_requested value
     to be untouched since the start of the function.
    */
    tm_core_info.session_info.op_requested |= sess_continue_param.op_req;
  
    if (retVal == TRUE) /*lint !e506 !e731 */
    {
     retVal = lm_request(&lm_info_req_param);

      /*
      For the MSB case, these lm_request operations are sent in the next block instead of here,
      since we fall through to that block anyways.
      */
      if (sess_continue_param.op_req & TM_OP_REQ_REF_LOC)
       {
          lm_info_req_param.e_req_type = LM_REQUEST_POS_ESTIMATE;
          retVal = lm_request(&lm_info_req_param);
       }

      if (sess_continue_param.op_req & TM_OP_REQ_RF_INFO)
       {
        /* Query TM-CM module to get current acquired system */
        tm_cm_iface_get_phone_state(&phone_state_info);

          retVal = lm_request(&lm_info_req_param);
       }
    }

    if (retVal == FALSE)
    {
      /* LM does not allow the request, abort and recover */
      tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
      return retVal;
    }
      return retVal;
  }
  /*
    Protocol sub-modules has already start a end-to-end session to LM,
    It now calls continue to perform the call flow.

    Note: we intentionally fall through from the previous case into this block for MSB.
    The fall through happens since tm_core_info.session_info.session_state was set to
    TM_CORE_SESS_STATE_ACCESS_GRANTED in the previous block.
    Most of the requested aiding data flags (PPM, PRM, LR) are populated here.
    For the case when we fallback from V2 to SA, only the LR request flag is set in the
    incoming request.
  */
  if ((sess_handle == tm_core_info.session_info.active_sess_handle)
      && (tm_core_info.session_info.session_state == TM_CORE_SESS_STATE_ACCESS_GRANTED))
  {
    /*
      Now forward continue request to LM
      If life is good, LM should accept it.
      If there is something wrong, LM rejects it (asynchronously), we need to make sure we do recovery properly
    */
      /* If operation mode has been changed (Optimal mode fallback sessions, or E911 that starts with MSB and now the network
         requests MSA for example ), notify LM to change the mode*/
    if (tm_core_info.session_info.op_mode != sess_continue_param.op_mode)
    {

    MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED,
          "Exiting op mode (%u) != cont.op_mode (%u), tm_core.op_requested = %x",
          tm_core_info.session_info.op_mode,
          sess_continue_param.op_mode,
          tm_core_info.session_info.op_requested,
          sess_continue_param.op_req);

    tm_core_info.session_info.op_mode = sess_continue_param.op_mode;
    lm_sess_req_param.q_handle = sess_handle;
    lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_CONTINUE;
    lm_sess_req_param.q_req_mask = (uint32)LM_SESSION_REQUEST_SESSION;
    lm_sess_req_param.z_request.z_session_request.e_mode = tm_core_convert_op_mode(sess_continue_param.op_mode, prtl_type);
  

    lm_sess_req_param.z_request.z_session_request.u_do_data_demod = TRUE;
    retVal = lm_session_request(&lm_sess_req_param);
    }

    if (retVal)
      {
       lm_sess_req_param.q_req_mask = 0;

      /*
      For operatons which have not been previously started, use start to LM
      For operatons which have already been started, use continue to LM
      */
     existing_op_req = sess_continue_param.op_req & tm_core_info.session_info.op_requested;
     new_op_req      = sess_continue_param.op_req & ~(existing_op_req);

     MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
            "continue_param.op_req = %lu, tm_core_info.session_info.op_requested = %lu",
            sess_continue_param.op_req, tm_core_info.session_info.op_requested);

     /*
     For operatons which have not been previously started, use start to LM
     */
      if (new_op_req & (TM_OP_REQ_LR))
      {
          lm_sess_req_param.q_handle   = sess_handle;
          lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_START;

        if (new_op_req & TM_OP_REQ_LR)
        {
          lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_FIX;
          lm_sess_req_param.z_request.z_fix_request.q_timeout = sess_continue_param.lr_qos * 1000;
          lm_sess_req_param.z_request.z_fix_request.q_qos_accuracy_threshold  = sess_continue_param.accuracy_threshold;
          lm_sess_req_param.z_request.z_fix_request.u_dynamic_qos_enabled  = sess_continue_param.dynamic_qos_enabled;
        }
        retVal = lm_session_request(&lm_sess_req_param);

      }

     tm_core_info.session_info.op_requested |= new_op_req;


     /*
     For operatons which have already been previously started, use continue to LM
     */
      if (existing_op_req & (TM_OP_REQ_LR))
      {
        lm_sess_req_param.q_handle   = sess_handle;
        lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_CONTINUE;

        if (existing_op_req & TM_OP_REQ_LR)
        {
          lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_FIX;
          lm_sess_req_param.z_request.z_fix_request.q_timeout = sess_continue_param.lr_qos * 1000;
          lm_sess_req_param.z_request.z_fix_request.q_qos_accuracy_threshold  = sess_continue_param.accuracy_threshold;
          lm_sess_req_param.z_request.z_fix_request.u_dynamic_qos_enabled  = sess_continue_param.dynamic_qos_enabled;
        }
          retVal = lm_session_request(&lm_sess_req_param);
      }
    }

    if (retVal == TRUE) /*lint !e506 !e731 */
    {
      if (sess_continue_param.op_req & TM_OP_REQ_REF_LOC)
      {
        lm_info_req_param.e_req_type = LM_REQUEST_POS_ESTIMATE;
        retVal = lm_request(&lm_info_req_param);
      }
      if (sess_continue_param.op_req & TM_OP_REQ_RF_INFO)
      {
        /* Query TM-CM module to get current acquired system */
        tm_cm_iface_get_phone_state(&phone_state_info);


        retVal = lm_request(&lm_info_req_param);
      }
    }

    if (retVal == FALSE)
    {
      /* LM does not allow the request, abort and recover */
      tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
    }
  }
  /*
    Return error if either the session handle is invalid or the session state was
    neither WAIT_AUTH nor GRANTED.
 */
  if ((sess_handle != tm_core_info.session_info.active_sess_handle) ||
      ((tm_core_info.session_info.session_state != TM_CORE_SESS_STATE_WAIT_AUTH) &&
       (tm_core_info.session_info.session_state != TM_CORE_SESS_STATE_ACCESS_GRANTED)))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Continue req rejected", 0, 0, 0);
    retVal = FALSE;
  }

  return retVal;
}

/*===========================================================================

FUNCTION tm_core_sess_req_stop

DESCRIPTION
  This function is to figure out if TM-Core accepts stop request from
  protocol sub-module.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_sess_req_stop(tm_prtl_type                       prtl_type,
                              tm_sess_handle_type                sess_handle,
                              tm_sess_req_stop_param_s_type      sess_stop_param)
{
  boolean retVal = FALSE;
  lm_session_request_info_s_type lm_sess_req_param;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM-Core Recv'd Sess Stop Req from %d, sess_handle %d", prtl_type, sess_handle, 0);

  memset((void *)&lm_sess_req_param, 0, sizeof(lm_session_request_info_s_type));

  /* If stop req belongs to active session and session is not in init state */
  if ((sess_handle == tm_core_info.session_info.active_sess_handle)
      && (tm_core_info.session_info.session_state != TM_CORE_SESS_STATE_INIT))
  {
    if (sess_stop_param.stop_type == TM_STOP_TYPE_OPERATION)
    {
      if (sess_stop_param.op_to_stop & (TM_OP_REQ_LR))
      {
        lm_sess_req_param.e_req_type = LM_SESSION_REQUEST_STOP;
        lm_sess_req_param.q_handle   = sess_handle;

        if (sess_stop_param.op_to_stop & TM_OP_REQ_LR)
        {
          lm_sess_req_param.q_req_mask |= (uint32)LM_SESSION_REQUEST_FIX;
        }

        tm_core_info.session_info.op_requested &= ~sess_stop_param.op_to_stop;
        retVal = lm_session_request(&lm_sess_req_param);
      }

      if (retVal == FALSE)
      {
        tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
      }
    }
    else if (sess_stop_param.stop_type == TM_STOP_TYPE_SESSION)
    {
      /* MSB session handover logic
         Only allow session handover when all the following conditions are satisfied
         1. OP mode is MSB
         2. Phone is not in OoS (because data download is needed)
      */
        /* No need to abort protocol sub-module again, because protocol sub-module initiated the abort */
      tm_core_abort_recover(FALSE, sess_stop_param.stop_reason); /*lint !e506 !e730 */
      retVal = TRUE; /*lint !e506 */
    }
  }
  else if ((sess_handle == TM_SESS_HANDLE_FORCE_STOP)
           && (tm_core_info.session_info.session_state != TM_CORE_SESS_STATE_INIT)
           && (sess_stop_param.stop_type == TM_STOP_TYPE_SESSION))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Protocl forced session termination", 0, 0, 0);
    tm_core_abort_recover(FALSE, sess_stop_param.stop_reason); /*lint !e506 !e730 */
    retVal = TRUE;
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Stop req rejected", 0, 0, 0);
    retVal = FALSE;
  }
  return retVal;
}


/*===========================================================================
#9x05: related to protocols only, no standalone calls
tm_core_recv_data
FUNCTION tm_core_recv_event

DESCRIPTION
  This function is to handle PD API Events sent from protocol sub-modules.

DEPENDENCIES

RETURN VALUE
  TRUE: If TM-Core sub-module accepts it
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
boolean tm_core_recv_event(
                                  pdsm_lite_pd_event_type   pd_event,
                                  pdsm_lite_pd_info_s_type *pd_info_ptr,
                                  sm_FixInfoStructType *lr_data
                                  )

{
   tm_pdapi_pd_event_s_type p_pd_info_type;
   MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TM-Core Recv'd PD events (%d) from protocol", pd_event, 0, 0);
  /* Init */
   memset(&p_pd_info_type, 0, sizeof(p_pd_info_type));

  if ((pd_event & PDSM_LITE_PD_EVENT_POSITION) ||
      (pd_event & PDSM_LITE_PD_EVENT_VELOCITY) ||
      (pd_event & PDSM_LITE_PD_EVENT_HEIGHT))
  {
  if (lr_data == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null pd_info_ptr", 0, 0, 0);
    return FALSE;
  }
    /* Copy ext data over */
/*    memscpy((void *)&(pd_info_ptr->pd_info.pd_ext_data),
            sizeof(pd_info_ptr->pd_info.pd_ext_data),
            (void *)&(tm_core_info.pd_ext_data),
            sizeof(tm_core_info.pd_ext_data));

    if (tm_core_info.ext_opt_field_mask & PDSM_LITE_PD_HEADING_VALID)
      pd_info_ptr->pd_info.pd_data.ext_opt_field_mask |= PDSM_LITE_PD_HEADING_VALID;
 */
     if(pd_info_ptr->pd_info.pz_PositionReportIndMsg.sessionStatus == eQMI_LOC_SESS_STATUS_IN_PROGRESS_V02 )
     {
       sm_log_diag_position(pd_info_ptr->client_id, lr_data, 0 , lr_data->z_NavPos.z_PosFlags.b_IsSft, lr_data->z_NavPos.z_PosFlags.b_IsKf);
     }
     else
     {
       tm_pdapi_pos_log_generation(
                                  pd_event,
                                  pd_info_ptr->client_id,
                                  lr_data,
                                  lr_data->z_NavPos.z_PosFlags.b_IsSft,
                                  lr_data->z_NavPos.z_PosFlags.b_IsKf
                                 );
       /*Store GNSS final fix in cache*/
       z_gnss_position_cache.cached_gnss_pos_valid = TRUE;
       memscpy(&z_gnss_position_cache.z_cached_gnss_pos_rpt, sizeof(z_gnss_position_cache.z_cached_gnss_pos_rpt), lr_data, sizeof(*lr_data));
        /*Since it will be used as best avail pos and b_IsPuncValid is true in best available position so seting this flag*/
        z_gnss_position_cache.z_cached_gnss_pos_rpt.z_NavPos.z_SmPosFlags.b_IsPuncValid = TRUE;
      }
    tm_pdapi_pos_event_callback(pd_event,(pdsm_lite_pd_info_s_type*)pd_info_ptr, TM_PDAPI_SESS_TYPE_NONE);
  }
  else if (pd_event & PDSM_LITE_PD_STATUS_SV_INFO_REPORT)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"PDSM_LITE_PD_STATUS_SV_INFO_REPORT",0,0,0);
    p_pd_info_type.pd_event_data.pd_info_type = *pd_info_ptr;
    
	
	p_pd_info_type.e_pd_event = PDSM_LITE_PD_STATUS_SV_INFO_REPORT;
    tm_pdapi_pd_event_callback(&p_pd_info_type, TM_PDAPI_SESS_TYPE_NONE);
  }
  else 
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unexpected Event",pd_event,0,0);  
  }
  return TRUE;
}

/*===========================================================================

FUNCTION tm_core_translate_lm_error

DESCRIPTION
  This function is to translate LM error to tm_core - prtl error cause

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
tm_sess_stop_reason_e_type tm_core_translate_lm_error
(
   tm_session_error_cause_e_type lm_err
   )
{
  switch (lm_err)
  {
    case TM_ERROR_CAUSE_FIX_TIMEOUT:
      return TM_STOP_REASON_FIX_TIMEOUT;
    case TM_ERROR_MGP_LOADING_ERROR:
	  return TM_STOP_REASON_GNSS_SERVICE_NOT_AVAILABLE;
	case TM_ERROR_MGP_RANDOM_RCVR_OFF_ERROR:
	  return TM_STOP_REASON_GNSS_RANDOM_RCVR_OFF_ERROR;
    default:
      return TM_STOP_REASON_GENERAL_ERROR;
  }
}

/*===========================================================================

FUNCTION tm_core_lm_sess_req_handler

DESCRIPTION
  This function is to handle sess request from LM

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_lm_sess_req_handler(void *lm_sess_req_data)
{
  tm_session_request_s_type *sess_req_ptr = (tm_session_request_s_type *)lm_sess_req_data;
  int prtlTableIndex = tm_core_get_prtl_idx_by_type(tm_core_info.session_info.from_protocol);
  tm_sess_stop_reason_e_type stop_reason;
  sm_gm_position_fix_fail_type z_gmpos_fixfail = {0};

  if (sess_req_ptr)
  {
    switch (sess_req_ptr->e_request_type)
    {
      case TM_REQUEST_ERROR_NOTIFY:
        if (sess_req_ptr->q_handle == tm_core_info.session_info.active_sess_handle)
        {
          /* Send abort request to Protocol sub-module, and let protocl sub-module decide what to do */

          if ((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
          {            
            if (tm_core_is_internal_session_on() == TRUE)
            {
              /* An internal session is going ON . Could be started from GM . Send the fix fail
               indication to it */
              MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Sending position fix fail update to GM . Reason %d",
                    sess_req_ptr->z_request.e_errorCause, 0, 0);
			  if(sess_req_ptr->z_request.e_errorCause == TM_ERROR_MGP_STATUS_TIMEOUT)
			  {
			    z_gmpos_fixfail.pos_fix_src = GEOFENCE_POS_FIX_SRC_GNSS;
				z_gmpos_fixfail.e_fail_reason = GEOFENCE_POS_FIX_FAIL_REASON_TIMEOUT;
                gm_position_fix_fail_update(&z_gmpos_fixfail);
			  }
            }
            /* Set reason to general error for now, if return value */
            if (tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp)
            {
              MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Got error notification from LM, foward to protocol modules", 0, 0, 0);
              stop_reason = tm_core_translate_lm_error(sess_req_ptr->z_request.e_errorCause);
              tm_core_abort_recover(TRUE, stop_reason); /*lint !e506 !e730 */
            }
            else
            {
              MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "No protocol callback installed 4", 0, 0, 0);
              tm_core_abort_recover(FALSE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
            }
          }
          else
          {
              MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid prtl index", 0, 0, 0);
          }
        } /* sess_req_ptr->q_handle == tm_core_info.session_info.active_sess_handle */
        else if (tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_NONE)
        {
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "LM - TM state not in sync, abort everything", 0, 0, 0);
          tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
        }
        break;

      default:
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unknown LM Sess Requst", 0, 0, 0);
        break;
    }
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null sess_req_ptr", 0, 0, 0);
    tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
  }
}

/*===========================================================================

FUNCTION tm_core_xlate_sv_state

DESCRIPTION 
Translates the SV state from the measurement report to PDAPI state 

DEPENDENCIES 
None 

RETURN VALUE 
PDSM state 

SIDE EFFECTS

===========================================================================*/
pdsm_lite_sv_state tm_core_xlate_sv_state(gnss_SvStateEnumType in_sv_state)
{
  switch (in_sv_state)
  {
    case C_GNSS_SV_STATE_IDLE:
      return PDSM_LITE_SV_STATE_IDLE;

    case C_GNSS_SV_STATE_SEARCH:
      return PDSM_LITE_SV_STATE_SEARCH;

    case C_GNSS_SV_STATE_SEARCH_VERIFY:
      return PDSM_LITE_SV_STATE_SEARCH_VERIFY;

    case C_GNSS_SV_STATE_GLO_10MSEC_BIT_EDGE:
    case C_GNSS_SV_STATE_BIT_EDGE:
      return PDSM_LITE_SV_STATE_BIT_EDGE;

    case C_GNSS_SV_STATE_VERIFY_TRACK:
    case C_GNSS_SV_STATE_TRACK:
    case C_GNSS_SV_STATE_DPO_TRACK:
      return PDSM_LITE_SV_STATE_TRACK;

    default:
      return PDSM_LITE_SV_STATE_IDLE;
  }
}

void tm_xlate_prms_to_qmi_loc(qmiLocEventGnssSvInfoIndMsgT_v02* pz_SVInfoIndMsg,
                                 gnss_MeasBlkStructType  *source_buf_ptr_gps,
                                 gnss_MeasBlkStructType  *source_buf_ptr_glo,
                                 gnss_MeasBlkStructType  *source_buf_ptr_bds,
                                 gnss_MeasBlkStructType  *source_buf_ptr_gal,
                                 gnss_MeasBlkStructType  *source_buf_ptr_qzss)
{
     uint16 pz_validSv_count =0;
	 MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"in tm_xlate_prms_to_qmi_loc");

	 if((pz_SVInfoIndMsg == NULL) ||
        (source_buf_ptr_gps == NULL) ||
        (source_buf_ptr_glo == NULL) ||
        (source_buf_ptr_bds == NULL) ||
        (source_buf_ptr_qzss == NULL) ||
        (source_buf_ptr_gal == NULL))
     {
         return;
     }

     /**As altitude assumed not using anywhere, making always FALSE and will get this info only in Fix report*/
   pz_SVInfoIndMsg->altitudeAssumed = FALSE;
   pz_SVInfoIndMsg->svList_valid = TRUE;


   pz_validSv_count = tm_util_xlate_meas_rpt_to_qmi_loc_SV_Info_Ind(source_buf_ptr_gps,pz_SVInfoIndMsg,pz_validSv_count);

   if (pz_validSv_count < QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02)
   {
       pz_validSv_count = tm_util_xlate_meas_rpt_to_qmi_loc_SV_Info_Ind(source_buf_ptr_glo,pz_SVInfoIndMsg,pz_validSv_count);
   }
   else
   {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"No space for GLO prms: GPS SVs > QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02 ", 0, 0, 0);
   }
   if (pz_validSv_count < QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02)
   {
       pz_validSv_count = tm_util_xlate_meas_rpt_to_qmi_loc_SV_Info_Ind(source_buf_ptr_bds,pz_SVInfoIndMsg,pz_validSv_count);
   }
   else
   {
       MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"No space for  BDS prms: GPS/GLO SVs > QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02 ", 0, 0, 0);
   }
   if (pz_validSv_count < QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02)
   {
	   pz_validSv_count = tm_util_xlate_meas_rpt_to_qmi_loc_SV_Info_Ind(source_buf_ptr_qzss,pz_SVInfoIndMsg,pz_validSv_count);
   }
   else
   {
       MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"No space for  QZSS prms: GPS/GLO/BDS SVs > QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02 ", 0, 0, 0);
   }
   if (pz_validSv_count < QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02)
   {
	   pz_validSv_count = tm_util_xlate_meas_rpt_to_qmi_loc_SV_Info_Ind(source_buf_ptr_gal,pz_SVInfoIndMsg,pz_validSv_count);
   }
   else
   {
       MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"No space for  GAL prms: GPS/GLO/BDS/QZSS SVs  > QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02 ", 0, 0, 0);
   }   

   pz_SVInfoIndMsg->svList_len = pz_validSv_count;
   
   MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"pz_SVInfoIndMsg->svList_len=%d",pz_SVInfoIndMsg->svList_len,0,0);

   return;

}






/*===========================================================================

FUNCTION tm_core_lm_sess_update_handler

DESCRIPTION
  This function is to handle sess update from LM

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_lm_sess_update_handler(void *lm_sess_update_data)
{
  tm_session_update_info_s_type *sess_update_ptr = NULL;
  int  prtlTableIndex = tm_core_get_prtl_idx_by_type(tm_core_info.session_info.from_protocol);
  prtl_sess_info_param_u_type   *sess_info_param_ptr = NULL;
  boolean                       retVal = FALSE;
  sm_GpsMeasRptStructType       *p_zMeas = NULL;
  gnss_MeasBlkStructType        *p_MeasBlkSrc = NULL;
  sm_FixInfoStructType          *p_zFix       = NULL;
  prtl_sess_info_e_type         sess_info_type = PRTL_SESS_INFO_NONE;
  sm_gm_pos_fix_info            fix_info;
  pdsm_lite_pd_event_type       pd_event;
  pdsm_lite_pd_info_s_type      *pz_md_info_buffer_ptr = NULL;
  boolean bBestAvailPos   = FALSE;
  pdsm_lite_pd_info_s_type      *pz_pd_info_buffer_ptr = NULL;
  sm_FixInfoStructType *p_zUTCFix = NULL;
  
  
  if (lm_sess_update_data == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL Pointer passed", 0, 0, 0);
    return;
  }

  sess_update_ptr = (tm_session_update_info_s_type *)lm_sess_update_data;
  if (sess_update_ptr)
  {
    // Dont check the session Handle for SV POLY, as its not per TM session.
     /* TODO: Poly report needs to be handled even when not in a session */
    if (sess_update_ptr->q_handle == tm_core_info.session_info.active_sess_handle)
    {
      sess_info_param_ptr = (prtl_sess_info_param_u_type *)pdsm_getbuf(sizeof(prtl_sess_info_param_u_type));
      if(sess_info_param_ptr == NULL)
      {
     tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
     return;
    }
     switch(sess_update_ptr->e_update_type)
     {
  case TM_INFO_STATUS_REPORT:
       if(sess_update_ptr->z_update.z_status_rpt.e_status == TM_STATUS_NACK)
     {
      tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
     }
  break;
    
  case TM_INFO_FINAL_FIX_REPORT:
  {
     p_zFix = &sess_update_ptr->z_update.z_fix_rpt.z_GpsFixRpt;
     sess_info_type = PRTL_SESS_INFO_LR;

     if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
       {
     /* Set reason to general error for now, if return value */
      if(tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp)
      {
        /* Generate extended pd data TODO:[an][del] new func to be added*/
        //tm_core_generate_ext_pd_data(p_zFix);
        memscpy((void *)&(sess_info_param_ptr->lr_data), sizeof(sess_info_param_ptr->lr_data),
        (void *)p_zFix, sizeof(*p_zFix));
        tm_core_info.session_info.op_requested &= ~TM_OP_REQ_LR;
        retVal = tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp(tm_core_info.session_info.active_sess_handle,
				  	                                                                    sess_info_type,sess_info_param_ptr);
        if(retVal == FALSE)
       {
        tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
       }
      }
    }

    /*Send the LR to Geofence module*/
    fix_info.fix_type = GEOFENCE_POS_FIX_TYPE_FINAL; 
      fix_info.y_pos_fix_ext_info.z_gnss_fix_ext_info.b_is_nw_asst_fix = FALSE;
    fix_info.fix_src = GEOFENCE_POS_FIX_SRC_GNSS;
    fix_info.pos_fix = *(&(sess_info_param_ptr->lr_data));
    (void)gm_position_update(&fix_info);

    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Timesource Final Fix: 0x%x Tunc(ns): %d Wk: %d",
    (uint8)  sess_update_ptr->z_update.z_fix_rpt.z_GpsFixRpt.z_NavPos.u_InitGpsTimeSrc,
    (uint32)(sess_update_ptr->z_update.z_fix_rpt.z_GpsFixRpt.z_NavPos.f_ClockBiasUncMs*1000) ,
       sess_update_ptr->z_update.z_fix_rpt.z_GpsFixRpt.z_NavPos.w_GpsWeek);
  }
  break;
  
  case TM_INFO_INTERMEDIATE_FIX_REPORT:
  {
  p_zFix = &sess_update_ptr->z_update.z_fix_rpt.z_GpsFixRpt;
  sess_info_type = PRTL_SESS_INFO_INTERM_LR;

  if (p_zFix->z_NavPos.z_SmPosFlags.b_IsPosBestAvail)
    {
       bBestAvailPos = TRUE; /* original NO_FIX report: CellDB fix, WiFi fix etc non-GNSS fix */
      }

  if (p_zFix->z_NavPos.w_GpsWeek != C_GPS_WEEK_UNKNOWN)
    {
    /* According to PE, if GPSWeek is valid, UTC time is guaranteed to be valid, 
    can output UTC time in NMEA, otherwise empty UTC */
    p_zUTCFix = p_zFix;
    }

  tm_core_info.session_info.q_FixTimeRemainingMSec = sess_update_ptr->z_update.z_fix_rpt.q_FixTimeRemainingMSec;

  /** not all procotol submodules consumes INTERM_LR: UMTS does */
  if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
      {
       prtl_sess_info_e_type session_info_type; 
     memscpy((void *)&(sess_info_param_ptr->lr_data), sizeof(sess_info_param_ptr->lr_data),
       (void *)&(sess_update_ptr->z_update.z_fix_rpt.z_GpsFixRpt),
       sizeof(sess_update_ptr->z_update.z_fix_rpt.z_GpsFixRpt));
     
    /* for BestAvailPos (originally NO_FIX), let AGPS handle it like NO_FIX since it's not GNSS fix */
      session_info_type = (bBestAvailPos ? PRTL_SESS_INFO_NOFIX : PRTL_SESS_INFO_INTERM_LR);
    (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp(
                                                       tm_core_info.session_info.active_sess_handle,session_info_type,
             sess_info_param_ptr);
    }

    /* Fire Postion PD API event */
    pz_pd_info_buffer_ptr = (pdsm_lite_pd_info_s_type *)os_MemAlloc(sizeof(pdsm_lite_pd_info_s_type), OS_MEM_SCOPE_TASK);
    if(pz_pd_info_buffer_ptr == NULL)
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cannot allocate memory for pdsm_lite_pd_info_s_type in tm_core_lm_sess_info_handler");
      break;
    }

    memset(&pz_pd_info_buffer_ptr->pd_info.pz_PositionReportIndMsg, 0 , sizeof(pz_pd_info_buffer_ptr->pd_info.pz_PositionReportIndMsg));

    pz_pd_info_buffer_ptr->client_id = tm_core_info.session_info.pd_param.client_id;

             pd_event = tm_util_xlate_fixt_report_to_qmi_loc_pos_rpt_Ind(p_zFix, &pz_pd_info_buffer_ptr->pd_info.pz_PositionReportIndMsg , 
                                                                sess_info_type);


             (void)tm_core_recv_event(pd_event,pz_pd_info_buffer_ptr,p_zFix);

    (void)os_MemFree((void **)&pz_pd_info_buffer_ptr);
                                 
    if(bBestAvailPos == FALSE)
     {     
     /* Send the LR to Geofence module */
      fix_info.fix_type = GEOFENCE_POS_FIX_TYPE_INTERMEDIATE;                      
      fix_info.y_pos_fix_ext_info.z_gnss_fix_ext_info.b_is_nw_asst_fix = FALSE;
      fix_info.fix_src = GEOFENCE_POS_FIX_SRC_GNSS;              
      fix_info.pos_fix = *(&(sess_info_param_ptr->lr_data));
   (void)gm_position_update(&fix_info);
     }
    
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Timesource Int Fix: 0x%x Tunc(ns): %d Wk: %d",
         (uint8)  p_zFix->z_NavPos.u_InitGpsTimeSrc,
         (uint32)(p_zFix->z_NavPos.f_ClockBiasUncMs*1000),
                  p_zFix->z_NavPos.w_GpsWeek);
  }
  break;
         case TM_INFO_PRM_REPORT:
         {
              MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Handling PRM_REPORT",0,0,0);
              sess_info_type = PRTL_SESS_INFO_SV_INFO;
              p_zMeas = &(sess_update_ptr->z_update.z_measure_rpt.z_GpsMeasRpt);
              p_MeasBlkSrc = &(p_zMeas->z_MeasBlk);
  	          if ((prtlTableIndex <= -1) || (prtlTableIndex >= TM_PRTL_NUM))
              {
                  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Invalid prtl index, can not report PRM", 0, 0, 0);
              }
              else
              {                
                 /* save measurement to tm_core_info */
                 switch (p_MeasBlkSrc->e_MeasBlkSrc)
                 {
                     case GNSS_MEAS_BLK_SRC_GPS:
                        tm_core_info.w_RecvdMeasBlkMask |= (1 << GNSS_MEAS_BLK_SRC_GPS);
                        memscpy((void *)&(tm_core_info.gps_meas_blk), sizeof(tm_core_info.gps_meas_blk),
                                (void *)p_MeasBlkSrc, sizeof(*p_MeasBlkSrc));
                     break;
                     case GNSS_MEAS_BLK_SRC_GLO:
                          tm_core_info.w_RecvdMeasBlkMask |= (1 << GNSS_MEAS_BLK_SRC_GLO);
                          memscpy((void *)&(tm_core_info.glo_meas_blk), sizeof(tm_core_info.glo_meas_blk),
                                  (void *)p_MeasBlkSrc, sizeof(*p_MeasBlkSrc));
                     break;
                     case GNSS_MEAS_BLK_SRC_BDS:
                          tm_core_info.w_RecvdMeasBlkMask |= (1 << GNSS_MEAS_BLK_SRC_BDS);
                          memscpy((void *)&(tm_core_info.bds_meas_blk), sizeof(tm_core_info.bds_meas_blk),
                                  (void *)p_MeasBlkSrc, sizeof(*p_MeasBlkSrc));
                      break;
                     case GNSS_MEAS_BLK_SRC_QZSS_SBAS:
                          tm_core_info.w_RecvdMeasBlkMask |= (1 << GNSS_MEAS_BLK_SRC_QZSS_SBAS);
                          memscpy((void *)&(tm_core_info.qzss_meas_blk), sizeof(tm_core_info.qzss_meas_blk),
                                  (void *)p_MeasBlkSrc, sizeof(*p_MeasBlkSrc));
                     break;
                     case GNSS_MEAS_BLK_SRC_GAL:
                          tm_core_info.w_RecvdMeasBlkMask |= (1 << GNSS_MEAS_BLK_SRC_GAL);
                          memscpy((void *)&(tm_core_info.gal_meas_blk), sizeof(tm_core_info.gal_meas_blk),
                                  (void *)p_MeasBlkSrc, sizeof(*p_MeasBlkSrc));
                     break;
  default:
                         MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"tm_core_lm_sess_update_handler: Unknown PRM source", 0, 0, 0);
                         tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
		    	    break;
                 }

				 
                /* Send extra pdapi event for PRM */
                if (p_MeasBlkSrc->u_SeqNum == p_MeasBlkSrc->u_MaxMessageNum)
  {
                    /* Conditionally clear the constellation meas blk validity if the corresponding meas blk is not received. */
                    if (!(tm_core_info.w_RecvdMeasBlkMask & (1 << GNSS_MEAS_BLK_SRC_GPS)))
                    {
                        tm_core_info.gps_meas_blk.u_NumSvs = 0;
                    }
                    if (!(tm_core_info.w_RecvdMeasBlkMask & (1 << GNSS_MEAS_BLK_SRC_GLO)))
                    {
                         tm_core_info.glo_meas_blk.u_NumSvs = 0;
                    }
                    if (!(tm_core_info.w_RecvdMeasBlkMask & (1 << GNSS_MEAS_BLK_SRC_BDS)))
                    {
                         tm_core_info.bds_meas_blk.u_NumSvs = 0;
                    }
                    if (!(tm_core_info.w_RecvdMeasBlkMask & (1 << GNSS_MEAS_BLK_SRC_GAL)))
                    {
                        tm_core_info.gal_meas_blk.u_NumSvs = 0;
                    }
                    if (!(tm_core_info.w_RecvdMeasBlkMask & (1 << GNSS_MEAS_BLK_SRC_QZSS_SBAS)))
                    {
                        tm_core_info.qzss_meas_blk.u_NumSvs = 0;
                    }
                    /* Clear the measblk mask as well. */
                    tm_core_info.w_RecvdMeasBlkMask = 0;
                    /* Fire SV info PD API event */
					
                    pz_md_info_buffer_ptr = (pdsm_lite_pd_info_s_type *)os_MemAlloc(sizeof(pdsm_lite_pd_info_s_type), OS_MEM_SCOPE_TASK);
                    if (pz_md_info_buffer_ptr == NULL)
                    {
                        MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cannot allocate memory for pdsm_lite_pd_info_s_type in tm_core_lm_sess_info_handler");
                        break;
                    }

                    memset(&pz_md_info_buffer_ptr->pz_SVInfoIndMsg, 0 , sizeof(pz_md_info_buffer_ptr->pz_SVInfoIndMsg));

                    pz_md_info_buffer_ptr->client_id = tm_core_info.session_info.pd_param.client_id;

					tm_xlate_prms_to_qmi_loc(&(pz_md_info_buffer_ptr->pz_SVInfoIndMsg),&(tm_core_info.gps_meas_blk),
						&(tm_core_info.glo_meas_blk),&(tm_core_info.bds_meas_blk),&(tm_core_info.gal_meas_blk),&(tm_core_info.qzss_meas_blk));
                    
                    (void)tm_core_recv_event(PDSM_LITE_PD_STATUS_SV_INFO_REPORT,
                             pz_md_info_buffer_ptr,
                             NULL);
			        (void)os_MemFree((void **)&pz_md_info_buffer_ptr);
               }
             }
        }
        break;
  default:
  {
     //do nothing
  }
   break;
   }
  (void) pdsm_freebuf((char *)sess_info_param_ptr);
 } /* sess_update_ptr->q_handle == tm_core_info.session_info.active_sess_handle */
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Rejecting Invalid Handle %d ActiveHandle %d UpdateType %d",
            sess_update_ptr->q_handle,
            tm_core_info.session_info.active_sess_handle,
            sess_update_ptr->e_update_type);
    }
  }
}

/*===========================================================================

FUNCTION tm_core_lm_sess_info_handler

DESCRIPTION
  This function is to handle session information from LM. The session info
  is available assitance data status from PE (CD).

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_lm_sess_info_handler(void *lm_sess_info_data)
{
  tm_info_s_type *sess_info_ptr = (tm_info_s_type *)lm_sess_info_data;
  int prtlTableIndex = tm_core_get_prtl_idx_by_type(tm_core_info.session_info.from_protocol);
  prtl_sess_info_param_u_type  *sess_info_param_ptr = NULL;
  uint32 q_long_word2 = 0;
  uint8  u_sv_cnt = 0, u_i;
  boolean retVal = FALSE;
  tm_pdapi_pd_event_s_type *pz_pd_event_info;
  pdsm_lite_pd_info_s_type  *pz_pd_info;
  tm_cm_phone_state_info_s_type   z_phone_state_info;
  lm_request_info_s_type               *lm_info_req_param=NULL;
  pz_pd_event_info = (tm_pdapi_pd_event_s_type *)os_MemAlloc(sizeof(tm_pdapi_pd_event_s_type), OS_MEM_SCOPE_TASK);
  
  if (NULL == pz_pd_event_info)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cannot allocate memory for pdsm_lite_ext_status_info_s_type in tm_core_lm_sess_info_handler");
    tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
    return;
  }

  pz_pd_info = &(pz_pd_event_info->pd_event_data.pd_info_type);
  memset(pz_pd_event_info, 0, sizeof(*pz_pd_event_info));

  if (sess_info_ptr)
  {
    sess_info_param_ptr = (prtl_sess_info_param_u_type *)pdsm_getbuf(sizeof(prtl_sess_info_param_u_type));

    if (sess_info_param_ptr == NULL)
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Failed to get buffer for sess_info_param_ptr", 0, 0, 0);
      (void)os_MemFree((void **)&pz_pd_info);
      tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
      return;
    }

    switch (sess_info_ptr->e_info_type)
    {
      case TM_INFO_PD_API_EVENT:
        {
      tm_pdapi_pd_event_s_type *pz_pd_event_info;
      
      pz_pd_event_info = (tm_pdapi_pd_event_s_type *)os_MemAlloc(sizeof(tm_pdapi_pd_event_s_type), OS_MEM_SCOPE_TASK);
      if (NULL == pz_pd_event_info)
      {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cannot allocate memory for tm_pdapi_pd_event_s_type in tm_core_lm_sess_info_handler");
      } 
      else
      {
      memset( pz_pd_event_info, 0, sizeof( *pz_pd_event_info ) );
      }

          if (sess_info_ptr->z_info.e_pdapi_event == TM_PD_EVENT_GPS_BEGIN)
          {
            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM: Received event TM_PD_EVENT_GPS_BEGIN from LM", 0, 0, 0);
            /*update the cached MGP receiver sate*/
            tm_core_info.e_cached_rcvr_state = C_GPS_RCVR_STATE_ON;

            if (NULL != pz_pd_event_info)
            {
              pz_pd_event_info->e_pd_event = PDSM_LITE_PD_EVENT_GPS_BEGIN;
              pz_pd_event_info->pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;

              tm_pdapi_pd_event_callback(pz_pd_event_info, TM_PDAPI_SESS_TYPE_NONE);
            }
          }
          else if (sess_info_ptr->z_info.e_pdapi_event == TM_PD_EVENT_GPS_DONE)
          {

            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM: Received event TM_PD_EVENT_GPS_DONE from LM", 0, 0, 0);
            /*update the cached MGP receiver state*/
            tm_core_info.e_cached_rcvr_state = C_GPS_RCVR_STATE_OFF;

            if (NULL != pz_pd_event_info)
            {
              pz_pd_event_info->e_pd_event = PDSM_LITE_PD_EVENT_GPS_DONE;
              pz_pd_event_info->pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;

              tm_pdapi_pd_event_callback(pz_pd_event_info, TM_PDAPI_SESS_TYPE_NONE);
            }
          }
          else if (sess_info_ptr->z_info.e_pdapi_event == TM_PD_EVENT_GPS_IDLE)
          {
            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM: Received event TM_PD_EVENT_GPS_IDLE from LM", 0, 0, 0);
            /*update the cached MGP receiver sate. There is no corresponding PDAPI event
             to be generated*/
            tm_core_info.e_cached_rcvr_state = C_GPS_RCVR_STATE_IDLE;
          }
          if (NULL != pz_pd_event_info)
          {
            (void)os_MemFree((void **)&pz_pd_event_info);
          }
        }
        break;

      case TM_INFO_POS_ESTIMATE:
        {
          boolean u_valid_fix = TRUE;
          if (!sess_info_ptr->z_info.z_pos_estimate.z_GpsFixRpt.z_NavPos.z_PosFlags.b_IsValid)
          {
            if ((sess_info_ptr->z_info.z_pos_estimate.z_GpsFixRpt.z_NavPos.w_GpsWeek == C_GPS_WEEK_UNKNOWN) &&
                (sess_info_ptr->z_info.z_pos_estimate.z_GpsFixRpt.z_NavPos.q_GpsTimeMs == 0))
            {
              u_valid_fix = FALSE;
            }
          }
          if ((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
          {
            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Rcvd Pos Estimate result from MGP, fix valid %d", u_valid_fix, 0, 0);
            if (u_valid_fix)
            {
              /* Set reason to general error for now, if return value */
              if (tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp)
              {
                memscpy((void *)&(sess_info_param_ptr->ref_lr_data),
                        sizeof(sess_info_param_ptr->ref_lr_data),
                        (void *)&(sess_info_ptr->z_info.z_pos_estimate),
                        sizeof(sess_info_ptr->z_info.z_pos_estimate));
                retVal = tm_core_info.prtl_func_cb_table[prtlTableIndex].sess_info_fp(tm_core_info.session_info.active_sess_handle,
                                                                                      PRTL_SESS_INFO_REF_LR,
                                                                                      sess_info_param_ptr);
                if (retVal == FALSE)
                {
                  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Protocol rejects Ref Loc data", 0, 0, 0);
                  tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506 !e730 */
                }
              }

              else
              {
                MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "No protocol callback installed 12", 0, 0, 0);
              }
            }
          }
          else if (tm_core_info.session_info.q_ReqClientMask != SM_GPS_STATE_INFO_REQ_TYPE_NONE)
          {
            sm_OnDemandPosPuncTuncStruct on_demand_info;

            memset(&on_demand_info, 0, sizeof(on_demand_info));
            memscpy((void *)&(sess_info_param_ptr->ref_lr_data),
                    sizeof(sess_info_param_ptr->ref_lr_data),
                    (void *)&(sess_info_ptr->z_info.z_pos_estimate),
                    sizeof(sess_info_ptr->z_info.z_pos_estimate));
            if (u_valid_fix)
            {
              memscpy((void *)&(sess_info_param_ptr->ref_lr_data),
                      sizeof(sess_info_param_ptr->ref_lr_data),
                      (void *)&(sess_info_ptr->z_info.z_pos_estimate),
                      sizeof(sess_info_ptr->z_info.z_pos_estimate));
              tm_core_info.gps_state_info.lat = (int32)(C_2_32_OVER_PI * sess_info_param_ptr->ref_lr_data.z_NavPos.d_PosLla[0]);
              tm_core_info.gps_state_info.lon = (int32)(C_2_31_OVER_PI * sess_info_param_ptr->ref_lr_data.z_NavPos.d_PosLla[1]);
              tm_core_info.gps_state_info.flags |= PDSM_LITE_PD_GPS_STATE_POS_VALID;
              tm_core_info.gps_state_info.flags |= PDSM_LITE_PD_GPS_STATE_ALT_VALID;
              tm_core_info.gps_state_info.altitude = (int32)(sess_info_param_ptr->ref_lr_data.z_NavPos.d_PosLla[2] * 10.0 + 0.5);
              tm_core_info.gps_state_info.loc_unc_vertical = (uint16)sess_info_param_ptr->ref_lr_data.z_NavPos.f_FilteredAltUnc;
              tm_core_info.gps_state_info.pos_unc = (uint32)sqrt((sess_info_param_ptr->ref_lr_data.z_NavPos.f_ErrorEllipse[1] *
                                                                  sess_info_param_ptr->ref_lr_data.z_NavPos.f_ErrorEllipse[1]) +
                                                                 (sess_info_param_ptr->ref_lr_data.z_NavPos.f_ErrorEllipse[2] *
                                                                  sess_info_param_ptr->ref_lr_data.z_NavPos.f_ErrorEllipse[2]));
              if (!sess_info_param_ptr->ref_lr_data.z_NavPos.z_PosFlags.b_Is3D)
              {
                tm_core_info.gps_state_info.flags &= ~PDSM_LITE_PD_GPS_STATE_ALT_VALID;
              }
            }
            else
            {
              tm_core_info.gps_state_info.flags &= ~PDSM_LITE_PD_GPS_STATE_POS_VALID;
              tm_core_info.gps_state_info.flags &= ~PDSM_LITE_PD_GPS_STATE_ALT_VALID;
            }

            if (tm_core_info.session_info.q_ReqClientMask & (TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE)))
            {
              tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].w_TmRcvdDataMask |= TM_RCVD_POS_FROM_PE;
              if (tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].w_TmRcvdDataMask == TM_RCVD_ALL)
              {
                pz_pd_event_info->e_pd_event = PDSM_LITE_PD_STATUS_GPS_STATE_INFO;
                pz_pd_info->client_id = tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].q_ClientId;
                tm_core_xlate_gps_state_to_ext_status(&tm_core_info.gps_state_info, &(pz_pd_info->pd_info.pz_gps_state_info));
                tm_pdapi_pd_event_callback(pz_pd_event_info, TM_PDAPI_SESS_TYPE_NONE);
                tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE));
                tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].w_TmRcvdDataMask = 0;
              }
            }
          }
          else
          {
            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid prtl index, can not report aiding data status", 0, 0, 0);
          }
        }
        break;

      case TM_GPS_TIME:

#ifdef FEATURE_CGPS_XTRA
        tm_xtra_gps_time_report_handler(
           &(sess_info_ptr->z_info.z_gps_time_info.z_gps_time_info_struct.GpsClock.z_Time)
           );
#endif /* FEATURE_CGPS_XTRA */
        break;

      case TM_INFO_MGP_EVENT:
        {
          tm_info_mgp_event_s_type *sess_info_ptr = (tm_info_mgp_event_s_type *)lm_sess_info_data;

          switch (sess_info_ptr->z_mgp_event.e_EventType)
          {
            case SM_EVENT_PE_NEW_NAVDATA:
              MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "NEW EPH Decoded 0x%x", sess_info_ptr->z_mgp_event.q_EventData);
              tm_core_info.session_info.eph_svmask |= 1 << (sess_info_ptr->z_mgp_event.q_EventData - 1);
              break;

            case SM_EVENT_PE_NEW_ALMANAC:
              MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "NEW ALM Decoded %x", sess_info_ptr->z_mgp_event.q_EventData);
              tm_core_info.session_info.alm_svmask |= 1 << (sess_info_ptr->z_mgp_event.q_EventData - 1);
              break;
            default:
              break;
          }
        }
        break;

        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "OEMDRETD: NEW CTRL Flag decoded as %d", sess_info_ptr->z_info.e_oemdre_td_ctrl, 0, 0);
        break;

      case TM_INFO_REPORT_SARF_STATUS:

        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Received SARF notification from LM = %u", sess_info_ptr->z_info.v_sarf_status);

        if (TRUE ==  sess_info_ptr->z_info.v_sarf_status)
        {
          tm_core_info.config_info.v_sarf_status = TRUE;

          /* Clean up TM core sessions if any */
          tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR);
        }
		else
		{
    	   tm_core_info.config_info.v_sarf_status = FALSE;
		}
        break;

      default:
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unsupported TM info type %u", sess_info_ptr->e_info_type);
        break;

    }  /* switch(sess_info_ptr->e_info_type) */

    (void)pdsm_freebuf((char *)sess_info_param_ptr);
  }
  (void)os_MemFree((void **)&pz_pd_event_info);
}

/*===========================================================================

FUNCTION tm_core_delete_inject_aiding_data

DESCRIPTION
  This function is called to delete or inject aiding data.

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_delete_inject_aiding_data
(
   pdsm_lite_delete_parms_type *delete_param_ptr,
   boolean force_rcvr_off,
   boolean delete_all,
   pdsm_lite_pa_e_type delete_type
   )
{
  lm_request_info_s_type lm_delete_req_info;
  lm_info_s_type         *lm_inject_req_info_ptr;
  boolean b_delete_mgp = FALSE;
  char b_Buffer[250];

/******************/

/*q_DeletePosMask is set as part of tm_core_clear_cached_pos(), so removed from here*/

gnss_DeleteInfoStructType *lm_mgp_info = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info);

mgp_GnssType *lm_e_GnssType = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.e_GnssType);

uint32 *lm_DeleteClockMask = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteClockMask);

uint32 *lm_DeleteEphMask = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteGpsEphMask);

uint32 *lm_DeleteAlmMask = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteGpsAlmMask);

boolean *lm_DeleteXtra = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.u_DeleteXtra);

uint32 *lm_DeleteRtiMask = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteRtiMask);

uint32 *lm_DeleteCellDBMask = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteCellDBMask);

uint32 *lm_DeletePosMask = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeletePosMask);

uint32 *lm_DeleteSvHealthMask = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteGpsSvHealthMask);

uint64 *lm_DeleteSvDirMask = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvDirMask);

uint64 *lm_DeleteSvSteerMask = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvSteerMask);

uint32 *lm_DeleteSaDataMask = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteSaDataMask);

boolean *lm_DeleteFreqBiasEst = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.u_DeleteFreqBiasEst);

uint64 *lm_DeleteSvNoExistMask = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvNoExistMask);

uint64 *lm_DeleteBlacklistGpsMask = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteBlacklistGpsMask);

boolean *lm_DeleteUtc = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.u_DeleteUtc);

  boolean *lm_DeleteIono = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.v_DeleteIono);

  boolean *lm_DeleteTgd = &(lm_delete_req_info.z_request.z_delete_request.z_mgp_info.v_DeleteTgd);
/******************/

  memset(&lm_delete_req_info, 0, sizeof(lm_request_info_s_type));

  if (delete_param_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null delete_param_ptr", 0, 0, 0);
    return;
  }

  (void)snprintf(b_Buffer, sizeof(b_Buffer), "Deletion mask: %llx, %d, %d, %d \r\n",
        delete_param_ptr->pdsm_lite_delete_parms_flags, force_rcvr_off, delete_all, delete_type);
  MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", b_Buffer);

  if (TRUE == delete_all)
  {
     /*
     If MGP is not loaded and delete all command is received 
     LM will have to delete XTRA data,GTS time, GTP data and
     cached position report
     */
	 if (LM_DMM_STATE_LOADED != lm_get_dmm_state())
     {
        (void)tm_xtra_delete_efs_data( XTRA_VERSION_3);/*Delete XTRA data*/
        (void)tm_xtra_delete_efs_data( XTRA_VERSION_51);/*Delete XTRA data*/
        gts_DelTime(TRUE);                            /*Delete GTS time*/
        tle_DeleteData(0x1FF);                        /*Delete GTP datat*/
     }

     /*[DEL] The cached position is stored in other structure for 
     propogation and so there is no need to clear this cache the
     supporting structures have been deleted*/

     tm_core_clear_cached_pos();

	 /* Clearing stored Eph expiry time info which sent by PE */
	 (void)gnss_DeleteEphAvailStatus(&tm_core_info.z_eph_exp_time);

	 MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Requesting deleteAll to MGP");

     lm_delete_req_info.e_req_type = LM_REQUEST_DELETE_MGP_INFO;
     lm_delete_req_info.z_request.z_delete_request.delete_all = TRUE;
     lm_delete_req_info.z_request.z_delete_request.force_rcvr_off = force_rcvr_off;

     /* Deletes GPS and Glonass data */
     *lm_e_GnssType = MGP_GNSS_TYPE_ALL;
     
     (void)lm_request(&lm_delete_req_info);

     return;
  }
  
  
  /*If MGP is not loaded, instead of going through MGP, call directly the APIs to delete.*/
  if (LM_DMM_STATE_LOADED != lm_get_dmm_state())
  {    
	 if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM_CORR)
     {
        (void)tm_xtra_delete_efs_data( XTRA_VERSION_3);/*Delete XTRA data*/
        (void)tm_xtra_delete_efs_data( XTRA_VERSION_51);/*Delete XTRA data*/
		
		delete_param_ptr->pdsm_lite_delete_parms_flags &= ~(PDSM_LITE_PA_DELETE_ALM_CORR);  
     }
     
     if (delete_param_ptr->pdsm_lite_delete_parms_flags & (PDSM_LITE_PA_DELETE_POS|PDSM_LITE_PA_POS_UNC|PDSM_LITE_PA_POS_OFFSET))
     {
         tle_DeleteData(0x1FF);                        /*Delete GTP datat*/
         tm_core_clear_cached_pos();                /*Delete cached position report*/	  
     }    
  }

  /* Deleting GTS time and EPH data unconditionally */
      
  if(delete_param_ptr->pdsm_lite_delete_parms_flags & (PDSM_LITE_PA_DELETE_TIME|PDSM_LITE_PA_TIME_UNC|PDSM_LITE_PA_TIME_OFFSET))
  {
      gts_DelTime(TRUE);                            /*Delete GTS time*/
  }

  if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_EPH)
  {
     /* Clearing stored Eph expiry time info which sent by PE */
	 (void)gnss_DeleteEphAvailStatus(&tm_core_info.z_eph_exp_time);
  }

  if (delete_param_ptr->pdsm_lite_delete_parms_flags & PA_AIDING_DATA_DELETE_MASK)
  {

    lm_delete_req_info.e_req_type = LM_REQUEST_DELETE_MGP_INFO;
    lm_delete_req_info.z_request.z_delete_request.delete_all = FALSE;
    lm_delete_req_info.z_request.z_delete_request.force_rcvr_off = force_rcvr_off;

    if (delete_param_ptr->pdsm_lite_delete_parms_flags & (PDSM_LITE_PA_POS_UNC|PDSM_LITE_PA_POS_OFFSET))
    {
      /* Position is independent of GPS/GLO, so technically we don't need to mark GPS.
         But we need this otherwise no call will be made to delete data. */
      b_delete_mgp = TRUE;
      tm_core_clear_cached_pos();
    }

    /*GPS deletion*/
    {
      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_TIME)
      {
        *lm_DeleteClockMask = C_DELETE_INFO_CLOCK_INFO;
        /* Time/clock info is independent of GPS/GLO, so technically we don't need to mark GPS.
           But we need this otherwise no call will be made to delete data. */
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_CLOCK_INFO)
      {
        *lm_DeleteClockMask = delete_param_ptr->delete_clock_info_mask;
        /* Clock is structure is common for GPS and Glo, hence calling either delete is fine*/
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_TIME_GPS)
      {
        *lm_DeleteClockMask |= C_DELETE_INFO_CLOCK_GPSTIME_EST;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_CELLDB_INFO)
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Deleting CellDB information. Mask %08X", delete_param_ptr->delete_celldb_mask, 0, 0);

        /* Translate the Delete Mask from PDAPI definitions to GPS core mask definitions */
        *lm_DeleteCellDBMask = 0;

        /* Celldb is independent of GPS/GLO, so technically we don't need to mark GPS.
           But we need this otherwise no call will be made to delete data. */
        b_delete_mgp = TRUE;

        if (delete_param_ptr->delete_celldb_mask & PDSM_LITE_PA_DELETE_CELLDB_MASK_POS_INFO)
        {
          *lm_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_POS;
        }
        if (delete_param_ptr->delete_celldb_mask & PDSM_LITE_PA_DELETE_CELLDB_MASK_LATEST_GPS_POS_INFO)
        {
          *lm_DeleteCellDBMask|= C_DELETE_INFO_CELLDB_LATEST_GPS_POS;
        }
        if (delete_param_ptr->delete_celldb_mask & PDSM_LITE_PA_DELETE_CELLDB_MASK_OTA_POS_INFO)
        {
          *lm_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_OTA_POS;
        }
        if (delete_param_ptr->delete_celldb_mask & PDSM_LITE_PA_DELETE_CELLDB_MASK_EXT_REF_POS_INFO)
        {
          *lm_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_EXT_REF_POS;
        }
        if (delete_param_ptr->delete_celldb_mask & PDSM_LITE_PA_DELETE_CELLDB_MASK_TIMETAG_INFO)
        {
          *lm_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_TIMETAG;
        }
        if (delete_param_ptr->delete_celldb_mask & PDSM_LITE_PA_DELETE_CELLDB_MASK_CELLID_INFO)
        {
          *lm_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_CELLID;
        }
        if (delete_param_ptr->delete_celldb_mask & PDSM_LITE_PA_DELETE_CELLDB_MASK_CACHED_CELLID_INFO)
        {
          *lm_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_CACHED_CELLID;
        }
        if (delete_param_ptr->delete_celldb_mask & PDSM_LITE_PA_DELETE_CELLDB_MASK_LAST_SRV_CELL_INFO)
        {
          *lm_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_LAST_SRV_CELL;
        }
        if (delete_param_ptr->delete_celldb_mask & PDSM_LITE_PA_DELETE_CELLDB_MASK_CUR_SRV_CELL_INFO)
        {
          *lm_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_CUR_SRV_CELL;
        }
        if (delete_param_ptr->delete_celldb_mask & PDSM_LITE_PA_DELETE_CELLDB_MASK_NEIGHBOR_INFO_INFO)
        {
          *lm_DeleteCellDBMask |= C_DELETE_INFO_CELLDB_NEIGHBOR_INFO;
        }
      }
	  
      if (delete_param_ptr->pdsm_lite_delete_parms_flags & (PDSM_LITE_PA_DELETE_POS))
      {        
        /* Position is independent of GPS/GLO, so technically we don't need to mark GPS.
           But we need this otherwise no call will be made to delete data. */
        *lm_DeletePosMask = C_DELETE_INFO_POS_INFO;
        b_delete_mgp = TRUE;

        tm_core_clear_cached_pos();

      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_EPH)
      {
        *lm_DeleteEphMask = delete_param_ptr->eph_sv_mask;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM)
      {
        *lm_DeleteAlmMask = delete_param_ptr->alm_sv_mask;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM_CORR)
      {
        *lm_DeleteXtra = TRUE;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_RTI)
      {
        *lm_DeleteRtiMask = C_DELETE_INFO_RTI_INFO;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_HEALTH)
      {
        *lm_DeleteSvHealthMask = (uint32)C_DELETE_INFO_GPS_SV_HEALTH_INFO;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_SVDIR)
      {
        *lm_DeleteSvDirMask = (uint64)C_DELETE_INFO_SV_DIR_INFO & 0x00000000FFFFFFFFULL;
        b_delete_mgp = TRUE;
      }
      
      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_SVSTEER)
      {
        *lm_DeleteSvSteerMask = (uint64)C_DELETE_INFO_SV_STEER_INFO & 0x00000000ffffffffULL;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_SADATA)
      {
        *lm_DeleteSaDataMask = C_DELETE_INFO_SA_DATA_INFO;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_FREQ_BIAS_EST)
      {
        *lm_DeleteFreqBiasEst = C_DELETE_INFO_FREQ_BIAS_EST;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_SV_NO_EXIST)
      {
        *lm_DeleteSvNoExistMask = C_DELETE_INFO_SV_NON_EXIST;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_GPS)
      {
        *lm_DeleteBlacklistGpsMask = (uint64)delete_param_ptr->gps_gnss_sv_blacklist_mask;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_UTC)
      {
        *lm_DeleteUtc = TRUE;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_IONO)
      {
        *lm_DeleteIono = TRUE;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_TGD)
      {
        *lm_DeleteTgd = TRUE;
        b_delete_mgp = TRUE;
      }

    if (b_delete_mgp)
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Requesting GPS aiding data deletion");
        *lm_e_GnssType = MGP_GNSS_TYPE_GPS;
        (void)lm_request(&lm_delete_req_info);
      }  
    }


    /*GLO deletion*/
    {
      b_delete_mgp = FALSE;
      memset(lm_mgp_info, 0x00, sizeof(gnss_DeleteInfoStructType));
    
      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_TIME_GLO)
      {
        *lm_DeleteClockMask |= C_DELETE_INFO_CLOCK_GLOTIME_EST;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_EPH_GLO)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteGloEphMask = delete_param_ptr->glo_eph_sv_mask;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM_GLO)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteGloAlmMask = delete_param_ptr->glo_alm_sv_mask;
        b_delete_mgp = TRUE;
      }
    
      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM_CORR_GLO)
      {
        *lm_DeleteXtra = TRUE;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_SVDIR_GLO)
      {
        *lm_DeleteSvDirMask = C_DELETE_INFO_SV_DIR_INFO;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_SVSTEER_GLO)
      {
        *lm_DeleteSvSteerMask = C_DELETE_INFO_SV_STEER_INFO;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_GLO)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteBlacklistGloMask = delete_param_ptr->glo_gnss_sv_blacklist_mask;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_HEALTH_GLO)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteGloSvHealthMask = (uint32)C_DELETE_INFO_GLO_SV_HEALTH_INFO;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_UTC)
      {
        *lm_DeleteUtc = TRUE;
        b_delete_mgp = TRUE;
      }
    
      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_IONO_GLO)
      {
        *lm_DeleteIono = TRUE;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_TGD)
      {
        *lm_DeleteTgd = TRUE;
        b_delete_mgp = TRUE;
      }

    if (b_delete_mgp)
    {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Requesting GLO aiding data deletion");
    *lm_e_GnssType = MGP_GNSS_TYPE_GLO;
    (void)lm_request(&lm_delete_req_info);
    }
    }

    /*BDS deletion*/
    {
      b_delete_mgp = FALSE;
      memset(lm_mgp_info, 0x00, sizeof(gnss_DeleteInfoStructType));
    
      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_TIME_BDS)
      {
        *lm_DeleteClockMask |= C_DELETE_INFO_CLOCK_BDSTIME_EST;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_EPH_BDS)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteBdsEphMask = delete_param_ptr->bds_eph_sv_mask;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM_BDS)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteBdsAlmMask = delete_param_ptr->bds_alm_sv_mask;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM_CORR_BDS)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.u_DeleteXtra = TRUE;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_SVDIR_BDS)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteBdsSvDirMask = C_DELETE_INFO_BDS_SV_DIR_INFO;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_SVSTEER_BDS)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteBdsSvSteerMask = C_DELETE_INFO_BDS_SV_STEER_INFO;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_BDS)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteBlacklistBdsMask = delete_param_ptr->bds_gnss_sv_blacklist_mask;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_HEALTH_BDS)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteBdsSvHealthMask = C_DELETE_INFO_BDS_SV_HEALTH_INFO;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_UTC)
      {
        *lm_DeleteUtc = TRUE;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_IONO_BDS)
      {
        *lm_DeleteIono = TRUE;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_TGD)
      {
        *lm_DeleteTgd = TRUE;
        b_delete_mgp = TRUE;
      }

    if (b_delete_mgp)
    {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Requesting BDS aiding data deletion");
    *lm_e_GnssType = MGP_GNSS_TYPE_BDS;
    (void)lm_request(&lm_delete_req_info);
    }
    }

    /*QZSS deletion*/
    {
      b_delete_mgp = FALSE;
      memset(lm_mgp_info, 0x00, sizeof(gnss_DeleteInfoStructType));
    
      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_EPH_QZSS)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteQzssEphMask = delete_param_ptr->qzss_eph_sv_mask;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM_QZSS)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteQzssAlmMask = delete_param_ptr->qzss_alm_sv_mask;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM_CORR_QZSS)
      {
        *lm_DeleteXtra = TRUE;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_HEALTH_QZSS)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.q_DeleteQzssSvHealthMask = (uint32)C_DELETE_INFO_QZSS_SV_HEALTH_INFO;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_SVDIR_QZSS)
      {
        *lm_DeleteSvDirMask = (uint64)C_DELETE_INFO_SV_DIR_INFO << 32;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_SVSTEER_QZSS)
      {
        *lm_DeleteSvSteerMask = (uint64)C_DELETE_INFO_SV_STEER_INFO << 32;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_QZSS)
      {
        *lm_DeleteBlacklistGpsMask = ((uint64)delete_param_ptr->qzss_gnss_sv_blacklist_mask) << 32;
        b_delete_mgp = TRUE;
      }
    
      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_IONO_QZSS)
      {
        *lm_DeleteIono = TRUE;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_TGD)
      {
        *lm_DeleteTgd = TRUE;
        b_delete_mgp = TRUE;
      }
    
    if (b_delete_mgp)
    {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Requesting QZSS aiding data deletion");
    *lm_e_GnssType = MGP_GNSS_TYPE_QZSS;
    (void)lm_request(&lm_delete_req_info);
    }  
    }

    /* GAL deletion */
    {
       b_delete_mgp = FALSE;
      memset(lm_mgp_info, 0x00, sizeof(gnss_DeleteInfoStructType));

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_TIME_GAL)
      {
        *lm_DeleteClockMask |= C_DELETE_INFO_CLOCK_GALTIME_EST;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_EPH_GAL)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteGalEphMask = delete_param_ptr->gal_eph_sv_mask;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM_GAL)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteGalAlmMask = delete_param_ptr->gal_alm_sv_mask;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM_CORR_GAL)
      {
        *lm_DeleteXtra = TRUE;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_SVDIR_GAL)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteGalSvDirMask = C_DELETE_INFO_GAL_SV_DIR_INFO;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_SVSTEER_GAL)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteGalSvSteerMask = C_DELETE_INFO_GAL_SV_STEER_INFO;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_GAL)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteBlacklistGalMask = delete_param_ptr->gal_gnss_sv_blacklist_mask;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_HEALTH_GAL)
      {
        lm_delete_req_info.z_request.z_delete_request.z_mgp_info.t_DeleteGalSvHealthMask = C_DELETE_INFO_GAL_SV_HEALTH_INFO;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_UTC)
      {
        *lm_DeleteUtc = TRUE;
        b_delete_mgp = TRUE;
      }
    
      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_IONO_GAL)
      {
        *lm_DeleteIono = TRUE;
        b_delete_mgp = TRUE;
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_TGD)
      {
        *lm_DeleteTgd = TRUE;
        b_delete_mgp = TRUE;
      }

    if (b_delete_mgp)
    {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Requesting GAL aiding data deletion");
    *lm_e_GnssType = MGP_GNSS_TYPE_GAL;
    (void)lm_request(&lm_delete_req_info);
    }
  }
    
  }
        
  if (delete_param_ptr->pdsm_lite_delete_parms_flags & PA_AIDING_DATA_INJECT_MASK)
  {
    lm_inject_req_info_ptr = (lm_info_s_type *)pdsm_getbuf(sizeof(lm_info_s_type));

    if (lm_inject_req_info_ptr != NULL)
    {
      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_TIME_UNC)
      {
        lm_inject_req_info_ptr->e_info_type = LM_SESSION_INFO_TIME_UNC;
        if (delete_type == PDSM_LITE_PA_DELETE_PARAMS_EX1)
        {
          lm_inject_req_info_ptr->z_info.f_new_time_unc = delete_param_ptr->f_time_unc;
        }
        else
        {
          lm_inject_req_info_ptr->z_info.f_new_time_unc = delete_param_ptr->time_unc;
        }

        (void)lm_info(lm_inject_req_info_ptr);
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_POS_UNC)
      {
        lm_inject_req_info_ptr->e_info_type = LM_SESSION_INFO_POS_UNC;

        if (delete_type == PDSM_LITE_PA_DELETE_PARAMS_EX1)
        {
          lm_inject_req_info_ptr->z_info.f_new_pos_unc = delete_param_ptr->pos_unc;
        }
        else
        {
          lm_inject_req_info_ptr->z_info.f_new_pos_unc = delete_param_ptr->pos_unc;
        }

        (void)lm_info(lm_inject_req_info_ptr);
      }

      if (delete_param_ptr->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_TIME_OFFSET)
      {
        lm_inject_req_info_ptr->e_info_type = LM_SESSION_INFO_TIME_OFFSET;

        if (delete_type == PDSM_LITE_PA_DELETE_PARAMS_EX1)
        {
          lm_inject_req_info_ptr->z_info.f_time_offset = delete_param_ptr->f_time_offset;
        }
        else
        {
          lm_inject_req_info_ptr->z_info.f_time_offset = delete_param_ptr->time_offset;
        }

        (void)lm_info(lm_inject_req_info_ptr);
        
      }
      
      (void)pdsm_freebuf((char *)lm_inject_req_info_ptr);
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Failed to get buffer for lm_inject_req_info_ptr", 0, 0, 0);
    }

  }

  return;
  
}

/*===========================================================================

FUNCTION tm_core_inject_ext_time

DESCRIPTION
  This function is called to inject external time into MGP through LM

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_inject_ext_time
(
   pdsm_lite_pd_external_time_info_type *ext_time_ptr
   )
{
  lm_request_info_s_type lm_ext_time_info;

  if (ext_time_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null ext_time_ptr", 0, 0, 0);
    return;
  }

  lm_ext_time_info.e_req_type = LM_REQUEST_SLOW_CLK_TIME_INJECT;

  lm_ext_time_info.z_request.z_slow_clk_inject_request.z_slow_clk_time.t_Milliseconds  =
     ext_time_ptr->time_msec;

  lm_ext_time_info.z_request.z_slow_clk_inject_request.z_slow_clk_time.q_Uncertainty  =
     ext_time_ptr->time_unc_msec;

  lm_ext_time_info.z_request.z_slow_clk_inject_request.z_slow_clk_time.e_TimeSource  =
     (gps_SlowClkExtTimeSourceType)ext_time_ptr->time_source;

  if (ext_time_ptr->time_is_gps)
  {
    lm_ext_time_info.z_request.z_slow_clk_inject_request.z_slow_clk_time.e_SlowClkSrc  =
       GNSS_SLOW_CLOCK_SRC_GPS;
  }
  else
  {
    lm_ext_time_info.z_request.z_slow_clk_inject_request.z_slow_clk_time.e_SlowClkSrc  =
       GNSS_SLOW_CLOCK_SRC_UTC;
  }

  /* force source to be either GPS or UTC */
  lm_ext_time_info.z_request.z_slow_clk_inject_request.e_source =
     ext_time_ptr->time_is_gps ? GNSS_SLOW_CLOCK_SRC_GPS : GNSS_SLOW_CLOCK_SRC_UTC;

  lm_ext_time_info.z_request.z_slow_clk_inject_request.z_slow_clk_time.u_DiscontinuityCnt  =
     ext_time_ptr->discontinuity_count;

  lm_ext_time_info.z_request.z_slow_clk_inject_request.z_slow_clk_time.b_ForceFlag  =
     ext_time_ptr->force_flag;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Injecting External time from source: %d", (int)ext_time_ptr->time_source, 0, 0);

  (void)lm_request(&lm_ext_time_info);
}

/*===========================================================================

FUNCTION tm_core_inject_ext_pos

DESCRIPTION
  This function is called to inject external position into MGP through LM

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_convert_ext_pos_to_fix_rpt(pdsm_lite_pd_external_position_info_type *p_ext_pos_ptr,
                                        sm_FixInfoStructType *p_fix_rpt)
{
  boolean u_sanity_ok = TRUE;
  sm_gm_pos_fix_info    *fix_info = NULL;
  float  horizontal_unc = 0;
  uint8  horizontal_conf = 0;
  float  f_scale_up_to_99pct_conf;

  if ((p_ext_pos_ptr == NULL) || (NULL == p_fix_rpt))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null Ext Pos Ptr", 0, 0, 0);
    return;
  }

  memset(p_fix_rpt, 0, sizeof(*p_fix_rpt));

  /* Update validity flags */
  p_fix_rpt->z_NavPos.z_PosFlags.b_IsExternal = TRUE;
  p_fix_rpt->z_NavPos.z_PosFlags.b_IsCoarsePos = TRUE;

  p_fix_rpt->z_NavPos.z_PosFlags.b_IsVelValid = FALSE;
  /* Velocity valid is set to false */
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "CPI injection velocity valid = %d", p_fix_rpt->z_NavPos.z_PosFlags.b_IsVelValid, 0, 0);

  /* Heading valid is set to false */
  p_fix_rpt->z_NavPos.z_PosFlags.b_IsHeadingValid = FALSE;

  /* Convert valid, coarse pos time stamp to GPS week and GPS msec */
  if (p_ext_pos_ptr->opt_field_mask & PDSM_LITE_PD_EXTERN_COARSE_POS_GPS_TIME_VALID)
  {
    U32 q_CoarsePosGpsTotalSec;
    if (p_ext_pos_ptr->time_info_type == PDSM_LITE_PD_EXTERN_COARSE_POS_TIME_GPS)
    {
      q_CoarsePosGpsTotalSec = p_ext_pos_ptr->timestamp_sec;
      p_fix_rpt->z_NavPos.w_GpsWeek =
         (U16)(q_CoarsePosGpsTotalSec / (U32)WEEK_SECS);
      p_fix_rpt->z_NavPos.q_GpsTimeMs =
         (q_CoarsePosGpsTotalSec % (U32)WEEK_SECS) * 1000;
      p_fix_rpt->z_NavPos.q_UtcTimeMs =
         ((p_ext_pos_ptr->timestamp_sec - (U32)cgps_GetNvUtcOffset()) % (U32)WEEK_SECS) * 1000;

      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "CPI injection extern time type= GPS GPS Week = %u, GPS Ms = %u",
            p_fix_rpt->z_NavPos.w_GpsWeek, p_fix_rpt->z_NavPos.q_GpsTimeMs,
            p_fix_rpt->z_NavPos.q_UtcTimeMs);
    }
    else if (p_ext_pos_ptr->time_info_type == PDSM_LITE_PD_EXTERN_COARSE_POS_TIME_UTC)
    {
      q_CoarsePosGpsTotalSec = p_ext_pos_ptr->timestamp_sec
         + (U32)cgps_GetNvUtcOffset();
      p_fix_rpt->z_NavPos.w_GpsWeek =
         (U16)(q_CoarsePosGpsTotalSec / (U32)WEEK_SECS);
      p_fix_rpt->z_NavPos.q_GpsTimeMs =
         (q_CoarsePosGpsTotalSec % (U32)WEEK_SECS) * 1000;
      p_fix_rpt->z_NavPos.q_UtcTimeMs =
         (p_ext_pos_ptr->timestamp_sec % (U32)WEEK_SECS) * 1000;

      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "CPI injection extern time type= UTC GPS Week = %u, GPS Ms = %u, UTC ms = %u",
            p_fix_rpt->z_NavPos.w_GpsWeek, p_fix_rpt->z_NavPos.q_GpsTimeMs,
            p_fix_rpt->z_NavPos.q_UtcTimeMs);
    }
    else
    {
      p_fix_rpt->z_NavPos.w_GpsWeek = C_GPS_WEEK_UNKNOWN;
      p_fix_rpt->z_NavPos.q_GpsTimeMs = 0;
    }
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "ExternCoarsePosPut: No valid gps time info", 0, 0, 0);
    p_fix_rpt->z_NavPos.w_GpsWeek = C_GPS_WEEK_UNKNOWN;
    p_fix_rpt->z_NavPos.q_GpsTimeMs = 0;
  }

  if (p_ext_pos_ptr->opt_field_mask & PDSM_LITE_PD_EXTERN_COARSE_POS_LAT_LONG_VALID)
  {
    float  horizontal_unc = 0;
    uint8  horizontal_conf = 0;

    /* mark the position as valid */
    p_fix_rpt->z_NavPos.z_PosFlags.b_IsValid = TRUE;

    /* mark the position as CPI */
    p_fix_rpt->z_NavPos.z_SmPosFlags.b_IsCoarsePos = TRUE;

    /* Convert to radians from degrees */
    p_fix_rpt->z_NavPos.d_PosLla[0] = (DBL)p_ext_pos_ptr->latitude * DEG2RAD;

    /* Convert to radians from degrees */
    p_fix_rpt->z_NavPos.d_PosLla[1] = (DBL)p_ext_pos_ptr->longitude * DEG2RAD;

    p_fix_rpt->z_NavPos.d_PosLla[2] = p_ext_pos_ptr->altitude;

    /*Check from where to pick up Horizontal Unc and Horizontal Confidence*/
    if (p_ext_pos_ptr->opt_field_mask & PDSM_LITE_PD_EXTERN_COARSE_POS_RAW_CIRCULAR_UNC_VALID)
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Taking Hor Unc and Confidence from Raw field", 0, 0, 0);
      horizontal_unc = p_ext_pos_ptr->loc_unc_hor_raw;
      if (p_ext_pos_ptr->opt_field_mask & PDSM_LITE_PD_EXTERN_COARSE_POS_RAW_HORIZ_CONF_VALID)
      {
        horizontal_conf = p_ext_pos_ptr->confidence_hor_raw;
      }
      else
      {
        horizontal_conf = 50;
      }
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Not Taking Hor Unc and Confidence from Raw field", 0, 0, 0);
      horizontal_unc = p_ext_pos_ptr->loc_unc_hor;
      if (p_ext_pos_ptr->opt_field_mask & PDSM_LITE_PD_EXTERN_COARSE_POS_HORIZ_CONF_VALID)
      {
        horizontal_conf = p_ext_pos_ptr->confidence_hor;
      }
      else
      {
        horizontal_conf = 50;
      }
    }

    f_scale_up_to_99pct_conf = tm_util_get_cep_scale_ratio_from_pct((float)horizontal_conf * 0.01f, 0.99f);

    /* Update the error ellipse for horizontal position uncertainty and ensure
     the result is within bounds */
    p_fix_rpt->z_NavPos.f_ErrorEllipse[0] = 0.0;
    p_fix_rpt->z_NavPos.f_ErrorEllipse[1] = (FLT)((float)horizontal_unc * (float)f_scale_up_to_99pct_conf)
       * (FLT)(1. / 3.03); /*CEP 99% -> Error Ellipse 39%*/

    p_fix_rpt->z_NavPos.f_ErrorEllipse[2] = p_fix_rpt->z_NavPos.f_ErrorEllipse[1];

    /* Set the latitude and longitude position uncertaintes as the error ellipse
     directly. The semi-major axis corresponds to longitude b/c alpha is 0. */
    p_fix_rpt->z_NavPos.f_PuncLatLonMeters[0] = p_fix_rpt->z_NavPos.f_ErrorEllipse[2];
    p_fix_rpt->z_NavPos.f_PuncLatLonMeters[1] = p_fix_rpt->z_NavPos.f_ErrorEllipse[1];

    p_fix_rpt->z_NavPos.z_SmPosFlags.b_IsPuncValid = TRUE;

    /* Horizontal reliability */
    if (p_ext_pos_ptr->e_HoriRelIndicator == PDSM_LITE_POSITION_RELIABILITY_NOT_SET)
    {
      p_fix_rpt->z_NavPos.e_HoriRelIndicator = PDSM_LITE_POSITION_RELIABILITY_LOW;
    }
    else
    {
      p_fix_rpt->z_NavPos.e_HoriRelIndicator =(p_ext_pos_ptr->e_HoriRelIndicator);
    }

    /* Update vertical position uncertainties */
    if ((p_ext_pos_ptr->opt_field_mask & PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_VALID)
        && (p_ext_pos_ptr->e_VertRelIndicator != PDSM_LITE_POSITION_RELIABILITY_VERY_LOW))
    {
      /* If HAE, accept uncertainty directly */
      if (p_ext_pos_ptr->alt_def == PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_HAE)
      {
        p_fix_rpt->z_NavPos.f_PuncVertMeters = (FLT)p_ext_pos_ptr->loc_unc_vert;
      }
      else
      {
        /* On receipt of an altitude in MSL from coarse position, increase uncertainty
          to compensate difference between MSL height and ellipsoidal height */
        /* Otherwise assume MSL and increase uncertainty. Optimally, the
          altitude reference could be adjusted to HAE by accounting for geoid
          in PE. This would avoid the unnecessary uncertainty increase. */
        p_fix_rpt->z_NavPos.f_PuncVertMeters =
           (FLT)p_ext_pos_ptr->loc_unc_vert + (FLT)100.0;
      }
      /* Vertical reliability */
      if (p_ext_pos_ptr->e_VertRelIndicator == PDSM_LITE_POSITION_RELIABILITY_NOT_SET)
      {
        p_fix_rpt->z_NavPos.e_VertRelIndicator = PDSM_LITE_POSITION_RELIABILITY_LOW;
      }
      else
      {
        p_fix_rpt->z_NavPos.e_VertRelIndicator = p_ext_pos_ptr->e_VertRelIndicator;
      }
    }

    /* Compute the 3D RSS position uncertainty in terms of the components and
     ensure the result is within bounds */
    p_fix_rpt->z_NavPos.f_Punc3dMeters =
       (FLT)sqrt(p_fix_rpt->z_NavPos.f_ErrorEllipse[1] * p_fix_rpt->z_NavPos.f_ErrorEllipse[1]
                 + p_fix_rpt->z_NavPos.f_ErrorEllipse[2] * p_fix_rpt->z_NavPos.f_ErrorEllipse[2]
                 + p_fix_rpt->z_NavPos.f_PuncVertMeters * p_fix_rpt->z_NavPos.f_PuncVertMeters);

    /* Set the total RSS velocity uncertainty to maximum */
    p_fix_rpt->z_NavPos.f_Vunc3dMps = (FLT)30.0;
  }
  else if ((p_ext_pos_ptr->opt_field_mask & PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_VALID)
           && (p_ext_pos_ptr->e_VertRelIndicator != PDSM_LITE_POSITION_RELIABILITY_VERY_LOW))
  {
    /* Process the altitude information, ensuring ECEF and LLA positions are
     consistent */
    p_fix_rpt->z_NavPos.d_PosLla[2] = p_ext_pos_ptr->altitude;

    /* If HAE, accept uncertainty directly */
    if (p_ext_pos_ptr->alt_def == PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_HAE)
    {
      p_fix_rpt->z_NavPos.f_PuncVertMeters = (FLT)p_ext_pos_ptr->loc_unc_vert;
    }
    else
    {
      /* Otherwise assume MSL and increase uncertainty. Optimally, the
        altitude reference could be adjusted to HAE by accounting for geoid
        in PE. This would avoid the unnecessary uncertainty increase. */
      p_fix_rpt->z_NavPos.f_PuncVertMeters =
         (FLT)p_ext_pos_ptr->loc_unc_vert + (FLT)100.0;
    }

    /* Vertical reliability */
    if (p_ext_pos_ptr->e_VertRelIndicator == PDSM_LITE_POSITION_RELIABILITY_NOT_SET)
    {
      p_fix_rpt->z_NavPos.e_VertRelIndicator =PDSM_LITE_POSITION_RELIABILITY_LOW;
    }
    else
    {
      p_fix_rpt->z_NavPos.e_VertRelIndicator = p_ext_pos_ptr->e_VertRelIndicator;
    }
  }

  return;
}


/*===========================================================================

FUNCTION tm_core_inject_ext_pos

DESCRIPTION
  This function is called to inject external position into MGP through LM

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_inject_ext_pos
(
   pdsm_lite_cmd_s_type *p_cmd_type
   )
{
  pdsm_lite_pd_external_position_info_type *ext_pos_ptr;
  lm_request_info_s_type     *lm_ext_pos_info=NULL;
  gnss_ExternCoarsePosInjectionType   *p_lm_extern_coase_pos_inj;
  sm_FixInfoStructType *p_fix_rpt = NULL;

  lm_request_info_s_type       *lm_info_req_param = NULL;
  tm_pdapi_pd_event_s_type     *pd_event_info=NULL;
  boolean                      u_sanity_ok = TRUE;
  float                        f_scale_up_to_99pct_conf;

  ext_pos_ptr = &p_cmd_type->cmd.pd.info.ext_pos_info;

  sm_log_extern_coarse_pos_inj_start(p_cmd_type); /* DM log */
  sm_report_event_coase_pos_inj_start(); /* DM event */

  pd_event_info = (tm_pdapi_pd_event_s_type*)os_MemAlloc(sizeof(tm_pdapi_pd_event_s_type), OS_MEM_SCOPE_TASK);
  if (pd_event_info==NULL)
  {
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory");
  return;
  }

  pd_event_info->pd_event_data.pd_info_type.client_id =
     p_cmd_type->cmd.pd.client_id;

  if (ext_pos_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null ext_pos_ptr", 0, 0, 0);
    u_sanity_ok = FALSE;
  }
  else
  {
    if (!(ext_pos_ptr->opt_field_mask &
             PDSM_LITE_PD_EXTERN_COARSE_POS_LAT_LONG_VALID) &&
        !(ext_pos_ptr->opt_field_mask &
             PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_VALID)
        )
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "No hor. or vert. pos", 0, 0, 0);
      u_sanity_ok = FALSE; /* no hor. or vert. pos. info., reject */
    }
    if ((ext_pos_ptr->opt_field_mask &
            PDSM_LITE_PD_EXTERN_COARSE_POS_HORIZ_CONF_VALID) &&
        ((ext_pos_ptr->confidence_hor == 0) ||
         (ext_pos_ptr->confidence_hor > 100)))
    {
      u_sanity_ok = FALSE;
    }
    if ((ext_pos_ptr->opt_field_mask &
            PDSM_LITE_PD_EXTERN_COARSE_POS_CIRCULAR_UNC_VALID) &&
        (ext_pos_ptr->loc_unc_hor == 0))
    {
      u_sanity_ok = FALSE; /* hor. unc = 0 unreasonable, reject */
    }
    if ((ext_pos_ptr->opt_field_mask &
            PDSM_LITE_PD_EXTERN_COARSE_POS_ALT_VALID) &&
        ((ext_pos_ptr->loc_unc_vert == 0) ||
         (ext_pos_ptr->confidence_vert == 0) ||
         (ext_pos_ptr->confidence_vert > 100)
         )
        )
    {
      u_sanity_ok = FALSE; /* vert. unc = 0 unreasonable, reject */
    }
    if ((ext_pos_ptr->opt_field_mask &
            PDSM_LITE_PD_EXTERN_COARSE_POS_ELLIPTICAL_UNC_VALID) &&
        ((ext_pos_ptr->horizontal_unc_elliptical.semimajor == 0) ||
         (ext_pos_ptr->horizontal_unc_elliptical.semiminor == 0)))
    {
      u_sanity_ok = FALSE;
    }
  }

  if (!u_sanity_ok) /* reject the injection command due to sanity failure */
  {
    pd_event_info->e_pd_event = PDSM_LITE_PD_EVENT_COARSE_POS_INJ_ERROR;
    tm_pdapi_pd_event_callback(pd_event_info, TM_PDAPI_SESS_TYPE_NONE);
  os_MemFree((void * *) &pd_event_info);
    return;
  }

  p_fix_rpt = (sm_FixInfoStructType *)os_MemAlloc(sizeof(sm_FixInfoStructType), OS_MEM_SCOPE_TASK);
  if (p_fix_rpt != NULL)
  {
    tm_core_convert_ext_pos_to_fix_rpt(ext_pos_ptr, p_fix_rpt);

    /*Send to GFC_QMI*/
    tm_gfc_qmi_inject_ext_pos(p_fix_rpt);

    /*Send to TechSel*/
//    techSel_ReportPosFix(p_fix_rpt); commented #9x05
    /*Free away with the Fix Rpt pointer. We are done with it*/
    os_MemFree((void **)&p_fix_rpt);
  }
else{
  /*Fix free all the memory dynamically allocated in this function before returning out of this function*/
  os_MemFree((void **)&pd_event_info);
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory");
    return;
}

  /* regardless if the corresponding position info. is valid, it does not hurt
     to rescale the confidence to 99 if 100 is given */
  if (ext_pos_ptr->confidence_hor == 100)
  {
    ext_pos_ptr->confidence_hor = 99;
  }
  if (ext_pos_ptr->confidence_hor_raw == 100)
  {
    ext_pos_ptr->confidence_hor_raw = 99;
  }
  if (ext_pos_ptr->confidence_vert == 100)
  {
    ext_pos_ptr->confidence_vert = 99;
  }

  lm_ext_pos_info = (lm_request_info_s_type*)os_MemAlloc(sizeof(lm_request_info_s_type), OS_MEM_SCOPE_TASK);
  if (lm_ext_pos_info==NULL)
  {
  os_MemFree((void **)&pd_event_info);
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Unable to allocate memory");
  return;
  }

  /* populate the LM interface structure */

  p_lm_extern_coase_pos_inj =
     &(lm_ext_pos_info->z_request.z_extern_coarse_pos_inject_request.z_extern_coarse_pos);

  lm_ext_pos_info->e_req_type = LM_REQUEST_EXTERNAL_COARSE_POS_INJECT;

  p_lm_extern_coase_pos_inj->opt_field_mask = (gps_extern_coarse_pos_opt_field)ext_pos_ptr->opt_field_mask;

  p_lm_extern_coase_pos_inj->time_info_type = (gps_extern_coarse_pos_time_stamp_e_type)ext_pos_ptr->time_info_type;

  p_lm_extern_coase_pos_inj->timestamp_sec = ext_pos_ptr->timestamp_sec;

  p_lm_extern_coase_pos_inj->source = (gps_extern_coarse_pos_source_e_type)ext_pos_ptr->source;

  p_lm_extern_coase_pos_inj->network_source = EXTERN_COARSE_POS_NETWORK_SOURCE_UNDEFINED; /* default to undefined */
  if (1 == ext_pos_ptr->m.posSrcProviderValid)
  {
    switch (ext_pos_ptr->src_provider)
    {
      case PDSM_LITE_PD_EXTERN_CPI_SRC_EXTERNAL :
        p_lm_extern_coase_pos_inj->network_source = EXTERN_COARSE_POS_NETWORK_SOURCE_EXTERNAL;
        break;
      case PDSM_LITE_PD_EXTERN_CPI_SRC_INTERNAL :
        p_lm_extern_coase_pos_inj->network_source = EXTERN_COARSE_POS_NETWORK_SOURCE_INTERNAL;
        break;
      default:
        /* Do nothing */
        break;
    }
  }

  p_lm_extern_coase_pos_inj->latitude = ext_pos_ptr->latitude;

  p_lm_extern_coase_pos_inj->longitude = ext_pos_ptr->longitude;

  /* scale position unc. to that of 99% confidence level */
  f_scale_up_to_99pct_conf = tm_util_get_cep_scale_ratio_from_pct((float)ext_pos_ptr->confidence_hor * 0.01f, 0.99f);

  p_lm_extern_coase_pos_inj->loc_unc_hor = ((float)ext_pos_ptr->loc_unc_hor * (float)f_scale_up_to_99pct_conf);

  f_scale_up_to_99pct_conf = tm_util_get_cep_scale_ratio_from_pct((float)ext_pos_ptr->confidence_hor_raw * 0.01f, 0.99f);

  p_lm_extern_coase_pos_inj->loc_unc_hor_raw = ((float)ext_pos_ptr->loc_unc_hor_raw * (float)f_scale_up_to_99pct_conf);

  p_lm_extern_coase_pos_inj->alt_def = (gps_extern_coarse_pos_alt_def_e_type)ext_pos_ptr->alt_def;

  p_lm_extern_coase_pos_inj->altitude = ext_pos_ptr->altitude;

  /* TBD: scale position unc. to that of 99% confidence level */
  p_lm_extern_coase_pos_inj->loc_unc_vert = ext_pos_ptr->loc_unc_vert;

  p_lm_extern_coase_pos_inj->e_HoriRelIndicator = *((pdsm_pd_reliability_e_type*)&(ext_pos_ptr->e_HoriRelIndicator));
  p_lm_extern_coase_pos_inj->e_VertRelIndicator = *((pdsm_pd_reliability_e_type*)&(ext_pos_ptr->e_VertRelIndicator));

  p_lm_extern_coase_pos_inj->e_AltitudeSource   = *((pdsm_pd_altitude_source_e_type*)&(ext_pos_ptr->e_AltitudeSource));
  p_lm_extern_coase_pos_inj->e_AltHorLinkage    = *((pdsm_pd_altitude_horizontal_linkage_e_type*)&(ext_pos_ptr->e_AltHorLinkage));
  p_lm_extern_coase_pos_inj->e_AltUncCoverage   = *((pdsm_pd_altitude_unc_coverage_e_type*)&(ext_pos_ptr->e_AltUncCoverage));

  p_lm_extern_coase_pos_inj->t_TimeTickMsec = cgps_TimeTickGetMsec();

  p_lm_extern_coase_pos_inj->horizontal_unc_elliptical.semiMajorUnc = ext_pos_ptr->horizontal_unc_elliptical.semimajor * f_scale_up_to_99pct_conf;
  p_lm_extern_coase_pos_inj->horizontal_unc_elliptical.semiMinorUnc = ext_pos_ptr->horizontal_unc_elliptical.semiminor * f_scale_up_to_99pct_conf;
  p_lm_extern_coase_pos_inj->horizontal_unc_elliptical.semiMajorAzimuthDegrees = ext_pos_ptr->horizontal_unc_elliptical.semimajor_azimuth;

  /* Determine if ext inj is allowed */
  if(TRUE == tm_core_session_active())
  {
    if(TRUE == tm_core_info.cs_on_demand_active && TRUE == tm_core_info.internal_session)
    {
     tm_core_info.ext_inj_allowed = FALSE;
    }
   else
    {
     tm_core_info.ext_inj_allowed = TRUE;
    }
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"tm_core_info.internal_session=%d, tm_core_info.ext_inj_allowed=%d",
            tm_core_info.internal_session, tm_core_info.ext_inj_allowed, 0);
  }
  else
  {
    if(TRUE == tm_core_info.cs_on_demand_active)
    {
      tm_core_info.ext_inj_allowed = FALSE;
    }
    else
    {
      tm_core_info.ext_inj_allowed = TRUE;
    }
  }

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Injecting external coarse pos if allowed(%d)", tm_core_info.ext_inj_allowed, 0, 0);

  if (TRUE == tm_core_info.ext_inj_allowed && lm_request(lm_ext_pos_info))
  {
    /* Request the current GPS time from MGP */
    //lm_info_req_param.e_req_type = LM_REQUEST_POS_ESTIMATE;

    /* Send request to LM->MGP */
    //lm_request(&lm_info_req_param);

    pd_event_info->e_pd_event = PDSM_LITE_PD_EVENT_COARSE_POS_INJ_DONE;
    tm_pdapi_pd_event_callback(pd_event_info, TM_PDAPI_SESS_TYPE_NONE);
    sm_report_event_coase_pos_inj_end(DM_EVT_EXTERN_COARSE_POS_INJ_END_REASON_DONE);
  }

  else
  {
    pd_event_info->e_pd_event = PDSM_LITE_PD_EVENT_COARSE_POS_INJ_ERROR;
    tm_pdapi_pd_event_callback(pd_event_info, TM_PDAPI_SESS_TYPE_NONE);
    sm_report_event_coase_pos_inj_end(DM_EVT_EXTERN_COARSE_POS_INJ_END_REASON_ERROR);
  }
  os_MemFree((void * *) &lm_ext_pos_info);
  os_MemFree((void * *) &pd_event_info);

}

/*===========================================================================
FUNCTION tm_core_handle_gtp_wwan_unavailability

DESCRIPTION
  This function is called to handle the scenario, When GTP WWAn is disabled through Caas.

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_handle_gtp_wwan_unavailability(sm_ReqEnumType source)
{
  sm_FixReportStructType p_PosInfo;
  memset((void *)&p_PosInfo, 0, sizeof(p_PosInfo));
  p_PosInfo.b_ReqType = source;
  p_PosInfo.z_FixInfo.z_NavPos.z_PosFlags.b_IsValid = FALSE;
  tm_api_update_gps_state_info(&p_PosInfo);
  return;
}

/*===========================================================================

FUNCTION tm_core_get_gps_state

DESCRIPTION
  This function is called to get the GPS Receiver state information

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_get_gps_state
(
   tm_core_get_gps_state_param_s_type *p_get_gps_state_param
)
{
  boolean ret_val = FALSE;
  boolean isCachedFixRequired = TRUE;
  lm_request_info_s_type               lm_info_req_param;
  pdsm_lite_cmd_s_type *p_cmd_type;
  lm_dmm_state_e_type dmm_state ; 
  boolean error = FALSE, request_wishlist = FALSE, request_pos_estimate = TRUE;

  if (NULL == p_get_gps_state_param ||
      p_get_gps_state_param->source > SM_GPS_STATE_INFO_REQ_TYPE_MAX)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core_get_gps_state] passed invalid paramaters",
          0, 0, 0);
    return;
  }

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core entering function [tm_core_get_gps_state] source %d",
        p_get_gps_state_param->source, 0, 0);

  memset((void *)&lm_info_req_param, 0, sizeof(lm_request_info_s_type));

  tm_core_info.session_info.q_ReqClientMask |= (TM_CORE_CLIENT_BITMASK(p_get_gps_state_param->source));
  tm_core_info.session_info.z_ReqClients[p_get_gps_state_param->source].w_TmRcvdDataMask = FALSE;

  dmm_state = lm_get_dmm_state();
  p_cmd_type = p_get_gps_state_param->cmd.p_cmd_type;
  
  switch (p_get_gps_state_param->source)
  {
    case SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE:
      /* Indicate that the data needs to be sent to PDAPI = TRUE */
      memset(&tm_core_info.gps_state_info, 0, sizeof(tm_core_info.gps_state_info));
      tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].q_ClientId = p_cmd_type->cmd.pd.client_id;
      request_wishlist = TRUE;
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI:
      /* Indicate that the data needs to be sent to PDAPI = TRUE */
      tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI].q_ClientId = p_cmd_type->cmd.pd.client_id;
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_XTRA_T:
      /* Indicate that the data needs to be sent to PDAPI = FALSE */
      tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_XTRA_T].q_ClientId = 0xFFFF;
      request_pos_estimate = FALSE;
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI:
      /* Indicate that the data needs to be sent to PDAPI = TRUE */
      if(!TM_CORE_CONFIG_IS_GTP_WWAN_AVAIL())
      {
         error = TRUE; 
         break;
      }
      tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI].q_ClientId = p_cmd_type->cmd.pd.client_id;
      if(LM_DMM_STATE_LOADED != dmm_state)
      {
        isCachedFixRequired = FALSE;
      }
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_GNSS_AVAIL_POS_QMI:
      /* Indicate that the data needs to be sent to PDAPI = TRUE */
      tm_core_info.session_info.z_ReqClients[p_get_gps_state_param->source].q_ClientId = p_cmd_type->cmd.pd.client_id;
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_GFC:
      /* Indicate that the data needs to be sent to PDAPI = FALSE */
      tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_GFC].q_ClientId = 0xFFFF;
      request_pos_estimate = FALSE;
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_GTP_WWAN_FIX:
      if(os_TimerCheck(tm_core_info.tm_core_gtp_wwan_fix_timer) > 0)
      {
        isCachedFixRequired = FALSE;
      }
      tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_GTP_WWAN_FIX].q_ClientId = p_cmd_type->cmd.pd.client_id;
      break;
    default:
      MSG(MSG_SSID_GPS, MSG_LEGACY_ERROR,"Unknown gps request source encountered");
      return;
  }

  MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "DMM state = %d, isCachedFixRequired = %d Errro = %d is gtp available : %d",
                                          dmm_state ,isCachedFixRequired, error, TM_CORE_CONFIG_IS_GTP_WWAN_AVAIL());

  if (FALSE == error)
  {
    if(LM_DMM_STATE_LOADED == dmm_state)
    {
      mgp_GetGPSStateInfo(p_get_gps_state_param->source);
  
      if (request_pos_estimate)
      {
#ifdef FEATURE_CGPS_ODP_2_0
        // need to ask PE for the position now not MGP
        lm_info_req_param.e_req_type = LM_REQUEST_POS_ESTIMATE;
        ret_val = lm_request(&lm_info_req_param);
#endif
      }
    }
    else
    {
#ifdef FEATURE_CGPS_XTRA_T
       if(TM_CORE_CONFIG_IS_GTP_WWAN_AVAIL())
       {
        if( (NULL != p_cmd_type) && (0 != p_cmd_type->cmd.pd.info.qos.gps_session_timeout))
        {
          tle_TriggerEnhancedPosUpdate(p_get_gps_state_param->source,p_cmd_type->cmd.pd.info.qos.gps_session_timeout);
        }
        else
        {
       #ifdef FEATURE_GNSS_TDP_10
       tle_TriggerTerPosUpdate(isCachedFixRequired);
       #else
       tle_TriggerTerPosUpdate(isCachedFixRequired,p_get_gps_state_param->source);
       #endif /* FEATURE_GNSS_TDP_10 */
       }
      }
       else
       {
        tm_core_handle_gtp_wwan_unavailability(p_get_gps_state_param->source);
       }
       ret_val = TRUE;
#endif
       }
    }
  else
  {
    tm_core_handle_gtp_wwan_unavailability(p_get_gps_state_param->source);
    ret_val = TRUE;
  }

  if(FALSE == ret_val) 
  {
    MSG(MSG_SSID_GPS, MSG_LEGACY_ERROR,"Error encountered while requesting GPS state");
  }
}
/*===========================================================================

FUNCTION tm_core_get_prtl_idx_by_gm_tr_id

DESCRIPTION
  This function is to get protocol array index by transaction ID

DEPENDENCIES

RETURN VALUE
  -1, if transaction ID mask can not be found.
  Otherwise, protocol array index for the transaction ID

SIDE EFFECTS

===========================================================================*/
int tm_core_get_prtl_idx_by_gm_tr_id(uint32 transaction_id)
{
  switch (transaction_id & TM_CORE_TRANSACTION_ID_DISPATCH_MASK)
  {
    case TM_GFC_QMI_GM_TRANSACTION_ID_BASE:
      return TM_CORE_PRTL_CB_ARR_INDEX_GFC_QMI;
#ifdef FEATURE_CGPS_XTRA_T
    case TM_XTRA_T_GM_TRANSACTION_ID_BASE:
      return TM_CORE_PRTL_CB_ARR_INDEX_XTRA_T;
#endif
    default:
      return -1;
  }
}

/*===========================================================================

FUNCTION tm_core_gm_evt_handler

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_gm_evt_handler(uint32 msg_id, void *p_Data)
{
  uint32  *tr_id_ptr;
  int prtlTableIndex;

  if (p_Data == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null p_Data", 0, 0, 0);
    return;
  }

  tr_id_ptr = (uint32 *)p_Data;

  prtlTableIndex = tm_core_get_prtl_idx_by_gm_tr_id(*tr_id_ptr);

  if ((prtlTableIndex >= 0) && (prtlTableIndex < TM_PRTL_NUM))
  {
    if (tm_core_info.prtl_func_cb_table[prtlTableIndex].gm_event_handle_fp != NULL)
    {
      (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].gm_event_handle_fp(
         (uint32)(msg_id - TM_CORE_GM_MSG_ID_BASE),
         p_Data);
    }
  }
  return;
}

/*===========================================================================

FUNCTION tm_core_cm_broadcast_event

DESCRIPTION
  This function is called by TM-CM module to inform TM-Core phone events

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_cm_broadcast_event
(
   uint32                        prtl_mask,
   prtl_event_u_type            *prtl_event_payload_ptr
   )
{
  int i;
  uint32 prtl_type = 0;
  int prtlTableIndex;

  if (prtl_event_payload_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "prtl_event_payload_ptr NULL", 0, 0, 0);
    return;
  }

  for (i  = 0; i < TM_PRTL_NUM; i++)
  {
    prtl_type = prtl_mask & (1 << i);

    if (prtl_type)
    {
      prtlTableIndex = tm_core_get_prtl_idx_by_type(prtl_type);

      if ((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
      {
        if (tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
        {
          (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(
             PRTL_EVENT_TYPE_PHONE,
             prtl_event_payload_ptr);
        }
      }
    }
  }
}

/*===========================================================================

FUNCTION tm_core_cm_notify_event

DESCRIPTION
  This function is called by TM-CM module to inform TM-Core phone events

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_cm_notify_event
(
   tm_phone_event_e_type          phone_event_type,
   tm_phone_event_payload_u_type  *evt_payload_ptr
   )
{
  boolean prtl_abort_session = TRUE;
  int prtlTableIndex = -1;
  prtl_event_u_type  *event_payload_ptr = NULL;
  sm_gm_srv_sys_notification_type z_gm_srv_sys_notify;
  tm_cm_phone_state_info_s_type phone_state_info;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "CM event %d", phone_event_type, 0, 0);
  event_payload_ptr = (prtl_event_u_type *)pdsm_getbuf(sizeof(prtl_event_u_type));
  if (event_payload_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cant get buffer", 0, 0, 0);
    return;
  }

  memset(&phone_state_info, 0, sizeof(tm_cm_phone_state_info_s_type));
  tm_cm_iface_get_phone_state(&phone_state_info);

  switch (phone_event_type)
  {
    case TM_PHONE_EVENT_TC_UP:
    case TM_PHONE_EVENT_TC_EXIT:
    case TM_PHONE_EVENT_NEW_SS_INFO:
     
      break;

    case TM_PHONE_EVENT_TC_CONNECTED:
    case TM_PHONE_EVENT_TC_DISCONNECTED:
      break;

    case TM_PHONE_EVENT_SS_SRV_STATUS:
    case TM_PHONE_EVENT_SS_SRV_DOMAIN:
      if (phone_event_type == TM_PHONE_EVENT_SS_SRV_STATUS)
      {
        /* For the following service transitions abort the currently ongoing session */
        if (((evt_payload_ptr->ss_info.prev_sys_mode == SYS_SYS_MODE_GSM)) ||
            ((evt_payload_ptr->ss_info.prev_sys_mode == SYS_SYS_MODE_GSM) && (evt_payload_ptr->ss_info.sys_mode == SYS_SYS_MODE_NO_SRV)))
        {
          if (tm_core_session_active())
          {
            prtlTableIndex = tm_core_get_prtl_idx_by_type(tm_core_info.session_info.from_protocol);

            if ((prtlTableIndex > -1) &&
                (prtlTableIndex < TM_PRTL_NUM) )
            {
              if (tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp)
              {
                MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "sys_mode change from %d to %d. Abort ongoing UMTS CP assisted sess",
                      evt_payload_ptr->ss_info.prev_sys_mode, evt_payload_ptr->ss_info.sys_mode, 0);

                prtl_abort_session = tm_core_info.prtl_func_cb_table[prtlTableIndex].stop_sess_req_fp(
                   tm_core_info.session_info.active_sess_handle,
                   PRTL_SESS_STOP_SINGLE_FIX,
                   TM_STOP_REASON_SYS_MODE_CHANGE);
                if (prtl_abort_session == TRUE)
                {
                  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "UMTS CP sess aborted due to sys_mode chg", 0, 0, 0);
                }
                else
                {
                  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "UMTS CP sess could not abort", 0, 0, 0);
                }
              }
            }
          }
        }
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "UMTS CP sess not aborted if sys_mode chg to OOS %d", evt_payload_ptr->ss_info.sys_mode );
        // Update the SRV system changed information to GM module
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Serving system %d update to GM", evt_payload_ptr->ss_info.sys_mode, 0, 0);
        z_gm_srv_sys_notify.e_curr_srv_sys = evt_payload_ptr->ss_info.sys_mode;
        gm_srv_sys_update(&z_gm_srv_sys_notify);
      }

      break;

    case TM_PHONE_EVENT_OOS:
    case TM_PHONE_EVENT_SYS_MODE_LPM:
      {
        cgps_CellInfoCachedStruct cell_db_data;

        memset((void *)&cell_db_data, 0, sizeof(cell_db_data));

        cell_db_data.e_ActiveRAT = CELLDB_ACTIVERAT_NONE;

        /*Send the update to Geofencing module*/
        (void)gm_cell_db_update(CELLDB_ACTIVERAT_NONE, &cell_db_data);


        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Serving system %d update to GM", SYS_SYS_MODE_NO_SRV, 0, 0);
        z_gm_srv_sys_notify.e_curr_srv_sys = SYS_SYS_MODE_NO_SRV;
        gm_srv_sys_update(&z_gm_srv_sys_notify);
      }
      break;


    case TM_PHONE_EVENT_OTASP_STATUS_COMMITTED:
      /* Send OTASP Status Committed event to 1X UP for potential MSID update */
     
      break;

    default:
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Phone event %d not handled", phone_event_type, 0, 0);
      break;
  }
  (void)pdsm_freebuf((char *)event_payload_ptr);
}


/*===========================================================================

FUNCTION tm_core_xlate_gps_state_to_ext_status

DESCRIPTION
  This function is called by TmCore to translate a gps state information
to external status

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void  tm_core_xlate_gps_state_to_ext_status(tm_core_gps_state_info_s_type *p_gps_state_info,
                                            pdsm_lite_pd_gps_state_s_type *pd_gps_state_info)
{
  pdsm_lite_xtra_download_info_s_type p_download_info;
  boolean xtra_valid;

  pd_gps_state_info->flags = p_gps_state_info->flags;
  pd_gps_state_info->lat = p_gps_state_info->lat;
  pd_gps_state_info->lon = p_gps_state_info->lon;
  pd_gps_state_info->altitude = p_gps_state_info->altitude;
  pd_gps_state_info->f_loc_unc_horizontal = p_gps_state_info->pos_unc;
  pd_gps_state_info->loc_unc_vertical = p_gps_state_info->loc_unc_vertical;

  pd_gps_state_info->timestamp_tow_ms = p_gps_state_info->timestamp_tow_ms;
  pd_gps_state_info->timestamp_gps_week = p_gps_state_info->timestamp_gps_week;  /*GPS week number */
  /*Time uncertainty in milli seconds, floor at 20 ms */
  if (p_gps_state_info->time_unc_us < 20000)
  {
    pd_gps_state_info->time_unc_ms = 20;
  }
  else
  {
    pd_gps_state_info->time_unc_ms = p_gps_state_info->time_unc_us / 1000;
  }
  pd_gps_state_info->engine_state = p_gps_state_info->engine_state; /*  GPS engine state on/off. always present, no flag */
  pd_gps_state_info->eph_svmask = p_gps_state_info->eph_svmask; /* SV Mask for the ephemeris */
  pd_gps_state_info->alm_svmask = p_gps_state_info->alm_svmask; /* SV Mask for the Almanac */
  pd_gps_state_info->health_svmask = p_gps_state_info->health_svmask; /* SV mask for the health */
  pd_gps_state_info->visible_svmask = p_gps_state_info->visible_svmask; /* sv mask for visible SVs*/
  xtra_valid = tm_xtra_set_xtra_time_and_validity(&p_download_info);
  if (xtra_valid)
  {
    pd_gps_state_info->flags |= PDSM_LITE_PD_GPS_STATE_XTRA_VALID;
    pd_gps_state_info->xtra_start_gps_week = tm_core_info.gps_state_info.start_gps_week = p_download_info.startGpsWeek;
    pd_gps_state_info->xtra_start_gps_minutes = tm_core_info.gps_state_info.start_gps_minutes = p_download_info.startGpsMinutes;
    pd_gps_state_info->xtra_valid_duration_hours = tm_core_info.gps_state_info.valid_duration_hours =
       p_download_info.validDurationHours;
  }
}


/*===========================================================================

FUNCTION tm_core_xlate_pd_ext_pos_to_qmi

DESCRIPTION
  This function is called by TM Core to translate the SM position report
  structure to a QMI defined position report.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void tm_core_xlate_pd_best_avail_pos_to_qmi
(
   const sm_FixInfoStructType *p_GnssFixRpt,
   qmiLocGetBestAvailablePositionIndMsgT_v02    *p_PdsmExtPos
   )
{


#ifndef FEATURE_GNSS_LEAN_BUILD

  double f_tmp = 0, f_vel_east = 0, f_vel_north = 0;
  uint32 i = 0, count = 0;
  float f_Punc;
  pdsm_lite_utc_time_s_type z_pdsm_lite_utc_time;

  /* UTC timestamp in millisecond since Jan 1st, 1970 */
  uint64 t_timestamp_utc_ms, t_timestamp_utc_s;

  f_Punc = tm_core_calculate_2d_punc(p_GnssFixRpt);
  memset(p_PdsmExtPos, 0, sizeof(pdsm_lite_pd_ext_pos_rpt_s_type));
  if (p_GnssFixRpt->z_NavPos.z_PosFlags.b_IsValid)
  {
    p_PdsmExtPos->status = eQMI_LOC_SUCCESS_V02;
    // Convert the latitude from radians into degrees.
    // radians = degrees * (PI/180)
    // degrees = (radians * 180)/PI

    /* Latitude */
    p_PdsmExtPos->latitude = (p_GnssFixRpt->z_NavPos.d_PosLla[0] * 180) / C_PI;
    p_PdsmExtPos->latitude_valid = TRUE;

    /*   Longitude */
    // Convert the longitude from radians into degrees.
    p_PdsmExtPos->longitude =  (p_GnssFixRpt->z_NavPos.d_PosLla[1] * 180) / C_PI;
    p_PdsmExtPos->longitude_valid = TRUE;

    /*   Circular Horizontal Position Uncertainty */
    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsPuncValid)
    {
      p_PdsmExtPos->horUncCircular = f_Punc;
      p_PdsmExtPos->horUncCircular_valid = TRUE;
    }

    /*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
    p_PdsmExtPos->horUncEllipseSemiMajor = p_GnssFixRpt->z_NavPos.f_ErrorEllipse[2];
    p_PdsmExtPos->horUncEllipseSemiMajor_valid = TRUE;

    /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
    p_PdsmExtPos->horUncEllipseSemiMinor = p_GnssFixRpt->z_NavPos.f_ErrorEllipse[1];
    p_PdsmExtPos->horUncEllipseSemiMinor_valid = TRUE;
    /*  Elliptical Horizontal Uncertainty Azimuth */
    p_PdsmExtPos->horUncEllipseOrientAzimuth = p_GnssFixRpt->z_NavPos.f_ErrorEllipse[0];
    p_PdsmExtPos->horUncEllipseOrientAzimuth_valid = TRUE;

    /*  Horizontal circular confidence */
    p_PdsmExtPos->horCircularConfidence = 63; // Hard coded as of now
    p_PdsmExtPos->horCircularConfidence_valid = TRUE;

    /*  Horizontal Elliptical Confidence */
    p_PdsmExtPos->horEllipticalConfidence = 39;
    p_PdsmExtPos->horEllipticalConfidence_valid = TRUE;

    /*  Horizontal Reliability */
    switch (p_GnssFixRpt->z_NavPos.e_HoriRelIndicator)
    {
      case PDSM_LITE_POSITION_RELIABILITY_NOT_SET:
        /* Not set, reliability info is not present */
        p_PdsmExtPos->horReliability = eQMI_LOC_RELIABILITY_NOT_SET_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_VERY_LOW:
        /* Not checked, use at user's own risk */
        p_PdsmExtPos->horReliability = eQMI_LOC_RELIABILITY_VERY_LOW_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_LOW:
        /* Limited cross-checked */
        p_PdsmExtPos->horReliability = eQMI_LOC_RELIABILITY_LOW_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_MEDIUM:
        /* Limited cross-checked */
        p_PdsmExtPos->horReliability = eQMI_LOC_RELIABILITY_MEDIUM_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_HIGH:
        /* Robust cross-checked */
        p_PdsmExtPos->horReliability = eQMI_LOC_RELIABILITY_HIGH_V02;
        break;

      default:
        /* Default reliability . Not set */
        p_PdsmExtPos->horReliability = eQMI_LOC_RELIABILITY_NOT_SET_V02;
        break;
    }
    p_PdsmExtPos->horReliability_valid = TRUE;

    /* Compute horizontal velocity from components and scale
    * to units of 0.25 m/s.
    */

    f_vel_east = p_GnssFixRpt->z_NavPos.f_VelEnu[0];
    f_vel_north = p_GnssFixRpt->z_NavPos.f_VelEnu[1];

    f_tmp = sqrt(f_vel_east * f_vel_east + f_vel_north * f_vel_north);

    if (f_tmp < TM_CORE_MIN_VELOCTY)
    {
      p_PdsmExtPos->horSpeed = 0;
    }
    else
    {
      p_PdsmExtPos->horSpeed =
         (float)tm_util_nearest_long(4.0 * f_tmp);
    }
    p_PdsmExtPos->horSpeed_valid = TRUE;

    /*  Horizontal Speed Uncertainty */
    p_PdsmExtPos->horSpeedUnc = (float)f_tmp;
    /* suds84. This is actually 3d uncertainty??*/
    p_PdsmExtPos->horSpeedUnc_valid = TRUE;

    /*  Altitude With Respect to Ellipsoid */
    p_PdsmExtPos->altitudeWrtEllipsoid = p_GnssFixRpt->z_NavPos.d_PosLla[2];
    p_PdsmExtPos->altitudeWrtEllipsoid_valid = TRUE;

    /*  Altitude With Respect to Sea Level */
    p_PdsmExtPos->altitudeWrtMeanSeaLevel = p_GnssFixRpt->z_NavPos.d_PosLla[2] + 500;
    p_PdsmExtPos->altitudeWrtMeanSeaLevel_valid = TRUE;

    /*  Vertical Uncertainty */
    p_PdsmExtPos->vertUnc = p_GnssFixRpt->z_NavPos.f_VuncVertMps;
    p_PdsmExtPos->vertUnc_valid = TRUE;

    /*  Vertical Confidence */
    p_PdsmExtPos->vertConfidence = 68;
    p_PdsmExtPos->vertConfidence_valid = TRUE;

    /*  Vertical Reliability */
    switch (p_GnssFixRpt->z_NavPos.e_VertRelIndicator)
    {
      case PDSM_LITE_POSITION_RELIABILITY_NOT_SET:
        /* Not set, reliability info is not present */
        p_PdsmExtPos->vertReliability = eQMI_LOC_RELIABILITY_NOT_SET_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_VERY_LOW:
        /* Not checked, use at user's own risk */
        p_PdsmExtPos->vertReliability = eQMI_LOC_RELIABILITY_VERY_LOW_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_LOW:
        /* Limited cross-checked */
        p_PdsmExtPos->vertReliability = eQMI_LOC_RELIABILITY_LOW_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_MEDIUM:
        /* Limited cross-checked */
        p_PdsmExtPos->vertReliability = eQMI_LOC_RELIABILITY_MEDIUM_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_HIGH:
        /* Robust cross-checked */
        p_PdsmExtPos->vertReliability = eQMI_LOC_RELIABILITY_HIGH_V02;
        break;

      default:
        /* Default reliability . Not set */
        p_PdsmExtPos->vertReliability = eQMI_LOC_RELIABILITY_NOT_SET_V02;
        break;
    }
    p_PdsmExtPos->vertReliability_valid = TRUE;

    /*  Vertical Speed */
    p_PdsmExtPos->vertSpeed = p_GnssFixRpt->z_NavPos.f_VelEnu[2];
    p_PdsmExtPos->vertSpeed_valid = TRUE;

    /*  Vertical Speed Uncertainty */
    p_PdsmExtPos->vertSpeedUnc = p_GnssFixRpt->z_NavPos.f_VuncVertMps;

    p_PdsmExtPos->vertSpeedUnc_valid = TRUE;

    /*  Heading */
    p_PdsmExtPos->heading = (p_GnssFixRpt->z_NavPos.f_HeadingRad * 180) / C_PI;
    p_PdsmExtPos->heading_valid = TRUE;

    /*  Heading Uncertainty */
    p_PdsmExtPos->headingUnc = (p_GnssFixRpt->z_NavPos.f_HeadingUncRad * 180) / C_PI;
    p_PdsmExtPos->headingUnc_valid = TRUE;

    /*  Magnetic Deviation */
    p_PdsmExtPos->magneticDeviation = p_GnssFixRpt->z_NavPos.f_MagDeviation;
    p_PdsmExtPos->magneticDeviation_valid = TRUE;

    /*  Technology Used */
    p_PdsmExtPos->technologyMask_valid = TRUE;
    p_PdsmExtPos->technologyMask = 0;

    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsCellId)
    {
      p_PdsmExtPos->technologyMask |= QMI_LOC_POS_TECH_MASK_CELLID_V02;
    }

    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsSatellite)
    {
      p_PdsmExtPos->technologyMask |= QMI_LOC_POS_TECH_MASK_SATELLITE_V02;
    }

    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsRefLoc)
    {
      p_PdsmExtPos->technologyMask |= QMI_LOC_POS_TECH_MASK_REFERENCE_LOCATION_V02;
    }

    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsCoarsePos)
    {
      p_PdsmExtPos->technologyMask |= QMI_LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION_V02;
    }

    /*Set the technology valid mask to FALSE if we could not get the technology used*/
    if (p_PdsmExtPos->technologyMask == 0)
    {
      p_PdsmExtPos->technologyMask_valid = FALSE;
    }

    /* Position dilution of precision.*/
    p_PdsmExtPos->DOP.PDOP = p_GnssFixRpt->z_NavPos.f_PDOP;
    /* Horizontal dilution of precision.*/
    p_PdsmExtPos->DOP.HDOP = p_GnssFixRpt->z_NavPos.f_HDOP;
    /* Vertical dilution of precision. */
    p_PdsmExtPos->DOP.VDOP = p_GnssFixRpt->z_NavPos.f_VDOP;

    p_PdsmExtPos->DOP_valid = TRUE;

    /*  UTC Timestamp */
    if (tm_util_calculate_utc(p_GnssFixRpt,&z_pdsm_lite_utc_time, NULL) == TRUE)
    {
      // We have the UTC time in YearMonthHourMinuteSecond calculated from GPS time ( Since Jan 6 1980 ) .
      // Convert the UTC time we have into UNIX timestamp ( Since Jan1 1970 )
      gnss_GetUnixEpochFromUTC((pdsm_utc_time_s_type*)&z_pdsm_lite_utc_time,
                               &t_timestamp_utc_s);
      /* Scale it to milliseconds, as the TimeStamp now is in seconds. */
      t_timestamp_utc_ms = t_timestamp_utc_s * 1000;
      /* Now lets add more precision.
         utc_time.sec --> is seconds scaled by 100.
                          ( ie. 1sec is reported as 100 &
                            1.99 sec is reported as 199 which is 1990msec)
         GetUnixEpochFromUTC gives us back in seconds. So we can add the tenths
         of a second and hundredth of a second.
   
         Note: Currently the precision of hundredth of a second is lost in
               translation in SM.
      */
      t_timestamp_utc_ms +=
         ((z_pdsm_lite_utc_time.sec % 100) * 10);
      p_PdsmExtPos->timestampUtc = t_timestamp_utc_ms;
      p_PdsmExtPos->timestampUtc_valid = TRUE;
    }
    else
    {
      p_PdsmExtPos->timestampUtc_valid = FALSE;
    }

    /*  GPS Time */
    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsGpsTimeValid)
    {
      /* Week number of the fix*/
      p_PdsmExtPos->gpsTime.gpsWeek = p_GnssFixRpt->z_NavPos.w_GpsWeek;
      p_PdsmExtPos->gpsTime.gpsTimeOfWeekMs = p_GnssFixRpt->z_NavPos.q_GpsTimeMs;
      p_PdsmExtPos->gpsTime_valid = TRUE;
    }

    /*  Time Uncertainty */
    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsTuncValid)
    {
      p_PdsmExtPos->timeUnc = p_GnssFixRpt->z_NavPos.f_ClockBiasUncMs;
      p_PdsmExtPos->timeUnc_valid = TRUE;
    }

    /*  Time Source */
    /* Convert the time source which is of the type pdsm_lite_extended_time_src_e_type into
       the type used by extended status report */
    switch (p_GnssFixRpt->z_NavPos.u_InitGpsTimeSrc)
    {
      case PDSM_LITE_GNSS_TIME_SRC_UNKNOWN:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_UNKNOWN_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_RESERVED:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_SYSTEM_TIMETICK_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_NETWORK_TIME_TRANSFER:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_NETWORK_TIME_TRANSFER_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_NETWORK_TIME_TAGGING:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_NETWORK_TIME_TAGGING_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_EXTERNAL_INPUT:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_EXTERNAL_INPUT_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_GPS_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_TOW_DECODE_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_TOW_CONFIRMED:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_TOW_CONFIRMED_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_TOW_WEEK_CONFIRMED:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_TOW_AND_WEEK_CONFIRMED_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_NAV_SOLUTION:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_NAV_SOLUTION_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_SOLVE_FOR_TIME:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_SOLVE_FOR_TIME_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_GLO_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_GLO_TOW_DECODE_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_TIME_TRANSFORM :
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_TIME_TRANSFORM_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_GSM_SLEEP_TIME_TAGGING:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_GSM_SLEEP_TIME_TAGGING_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_QZSS_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_QZSS_TOW_DECODE_V02;
        break;
      case  PDSM_LITE_GNSS_TIME_SRC_BDS_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_BDS_TOW_DECODE_V02;
        break;
      case  PDSM_LITE_GNSS_TIME_SRC_GAL_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_GAL_TOW_DECODE_V02;
        break;

      default:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_INVALID_V02;
        break;
    }
    if (p_PdsmExtPos->timeSrc != eQMI_LOC_TIME_SRC_INVALID_V02)
    {
      p_PdsmExtPos->timeSrc_valid = TRUE;
    }

    /*  SV's Used to Calculate the Fix */
    //LOC_ENG_MAX_SV_USED_LIST_LENGTH_V01;

    count = 0;  /* up to 80 SVs allowed in QMI, won't overflow (16+14+16=46) */
    // Pack the SV used information ( GNSS and GLO SVs ) into the same array for the extended status callback
    // GPS SVs used information
    for (i = 0; i < ARR_SIZE(p_GnssFixRpt->z_NavSvInfo.z_GpsSvInfo); i++)
    {
      if (p_GnssFixRpt->z_NavSvInfo.z_GpsSvInfo[i].q_SvInfoMask & (PDSM_LITE_GNSS_SV_INFO_USED_IN_POS_FIX | PDSM_LITE_GNSS_SV_INFO_USED_IN_VEL_FIX))
      {
        p_PdsmExtPos->gnssSvUsedList[count++] = (uint16)p_GnssFixRpt->z_NavSvInfo.z_GpsSvInfo[i].w_SvId;
      }
    }
    // GLO SVs used information
    for (i = 0; i < ARR_SIZE(p_GnssFixRpt->z_NavSvInfo.z_GloSvInfo); i++)
    {
      if (p_GnssFixRpt->z_NavSvInfo.z_GloSvInfo[i].q_SvInfoMask & (PDSM_LITE_GNSS_SV_INFO_USED_IN_POS_FIX | PDSM_LITE_GNSS_SV_INFO_USED_IN_VEL_FIX))
      {
        p_PdsmExtPos->gnssSvUsedList[count++] = (uint16)p_GnssFixRpt->z_NavSvInfo.z_GloSvInfo[i].w_SvId;
      }
    }
    // BDS SVs used information
    for (i = 0; i < ARR_SIZE(p_GnssFixRpt->z_NavSvInfo.z_BdsSvInfo); i++)
    {
      if (p_GnssFixRpt->z_NavSvInfo.z_BdsSvInfo[i].q_SvInfoMask & (PDSM_LITE_GNSS_SV_INFO_USED_IN_POS_FIX | PDSM_LITE_GNSS_SV_INFO_USED_IN_VEL_FIX))
      {
        p_PdsmExtPos->gnssSvUsedList[count++] = (uint16)p_GnssFixRpt->z_NavSvInfo.z_BdsSvInfo[i].w_SvId;
      }
    }

    // QZSS SVs used information
    for (i = 0; i < ARR_SIZE(p_GnssFixRpt->z_NavSvInfo.z_QzssSvInfo); i++)
    {
      if (p_GnssFixRpt->z_NavSvInfo.z_QzssSvInfo[i].q_SvInfoMask & (PDSM_LITE_GNSS_SV_INFO_USED_IN_POS_FIX | PDSM_LITE_GNSS_SV_INFO_USED_IN_VEL_FIX))
      {
        p_PdsmExtPos->gnssSvUsedList[count++] = (uint16)p_GnssFixRpt->z_NavSvInfo.z_QzssSvInfo[i].w_SvId;
      }
    }

    // GAL SVs used information
    for (i = 0; i < ARR_SIZE(p_GnssFixRpt->z_NavSvInfo.z_GalSvInfo); i++)
    {
      if (p_GnssFixRpt->z_NavSvInfo.z_GalSvInfo[i].q_SvInfoMask & (PDSM_LITE_GNSS_SV_INFO_USED_IN_POS_FIX | PDSM_LITE_GNSS_SV_INFO_USED_IN_VEL_FIX))
      {
        p_PdsmExtPos->gnssSvUsedList[count++] = (uint16)p_GnssFixRpt->z_NavSvInfo.z_GalSvInfo[i].w_SvId;
      }
    }

    p_PdsmExtPos->gnssSvUsedList_len = count;
    p_PdsmExtPos->gnssSvUsedList_valid = TRUE;
  } // end of if (p_GnssFixRpt->z_NavPos.z_PosFlags.b_IsValid)
  else
  {
    p_PdsmExtPos->status = eQMI_LOC_GENERAL_FAILURE_V02;
  }
#endif
}

/*===========================================================================

FUNCTION tm_core_xlate_pd_wwan_pos_to_qmi

DESCRIPTION
  This function is called by TM Core to translate the SM position report
  structure to a QMI defined position report.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void tm_core_xlate_pd_wwan_pos_to_qmi
(
   const sm_FixInfoStructType *p_GnssFixRpt,
   qmiLocGetAvailWwanPositionIndMsgT_v02    *p_PdsmExtPos
   )
{


#ifndef FEATURE_GNSS_LEAN_BUILD

  double f_tmp = 0, f_vel_east = 0, f_vel_north = 0;
  uint32 i = 0;
  float f_Punc;
  pdsm_lite_utc_time_s_type z_pdsm_lite_utc_time;

  /* UTC timestamp in millisecond since Jan 1st, 1970 */
  uint64 t_timestamp_utc_ms, t_timestamp_utc_s;

  f_Punc = tm_core_calculate_2d_punc(p_GnssFixRpt);
  memset(p_PdsmExtPos, 0, sizeof(qmiLocGetAvailWwanPositionIndMsgT_v02));
  if (p_GnssFixRpt->z_NavPos.z_PosFlags.b_IsValid)
  {
    p_PdsmExtPos->status = eQMI_LOC_SUCCESS_V02; 
    // Convert the latitude from radians into degrees.
    // radians = degrees * (PI/180)
    // degrees = (radians * 180)/PI

    /* Latitude */
    p_PdsmExtPos->latitude = (p_GnssFixRpt->z_NavPos.d_PosLla[0] * 180) / C_PI;
    p_PdsmExtPos->latitude_valid = TRUE;

    /*   Longitude */
    // Convert the longitude from radians into degrees.
    p_PdsmExtPos->longitude =  (p_GnssFixRpt->z_NavPos.d_PosLla[1] * 180) / C_PI;
    p_PdsmExtPos->longitude_valid = TRUE;

    /*   Circular Horizontal Position Uncertainty */
    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsPuncValid)
    {
      p_PdsmExtPos->horUncCircular = f_Punc;
      p_PdsmExtPos->horUncCircular_valid = TRUE;
    }

    /*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
    p_PdsmExtPos->horUncEllipseSemiMajor = p_GnssFixRpt->z_NavPos.f_ErrorEllipse[2];
    p_PdsmExtPos->horUncEllipseSemiMajor_valid = TRUE;

    /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
    p_PdsmExtPos->horUncEllipseSemiMinor = p_GnssFixRpt->z_NavPos.f_ErrorEllipse[1];
    p_PdsmExtPos->horUncEllipseSemiMinor_valid = TRUE;
    /*  Elliptical Horizontal Uncertainty Azimuth */
    p_PdsmExtPos->horUncEllipseOrientAzimuth = p_GnssFixRpt->z_NavPos.f_ErrorEllipse[0];
    p_PdsmExtPos->horUncEllipseOrientAzimuth_valid = TRUE;

    /*  Horizontal circular confidence */
    p_PdsmExtPos->horCircularConfidence = 63; // Hard coded as of now
    p_PdsmExtPos->horCircularConfidence_valid = TRUE;

    /*  Horizontal Elliptical Confidence */
    p_PdsmExtPos->horEllipticalConfidence = 39;
    p_PdsmExtPos->horEllipticalConfidence_valid = TRUE;

    /*  Horizontal Reliability */
    switch (p_GnssFixRpt->z_NavPos.e_HoriRelIndicator)
    {
      case PDSM_LITE_POSITION_RELIABILITY_NOT_SET:
        /* Not set, reliability info is not present */
        p_PdsmExtPos->horReliability = eQMI_LOC_RELIABILITY_NOT_SET_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_VERY_LOW:
        /* Not checked, use at user's own risk */
        p_PdsmExtPos->horReliability = eQMI_LOC_RELIABILITY_VERY_LOW_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_LOW:
        /* Limited cross-checked */
        p_PdsmExtPos->horReliability = eQMI_LOC_RELIABILITY_LOW_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_MEDIUM:
        /* Limited cross-checked */
        p_PdsmExtPos->horReliability = eQMI_LOC_RELIABILITY_MEDIUM_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_HIGH:
        /* Robust cross-checked */
        p_PdsmExtPos->horReliability = eQMI_LOC_RELIABILITY_HIGH_V02;
        break;

      default:
        /* Default reliability . Not set */
        p_PdsmExtPos->horReliability = eQMI_LOC_RELIABILITY_NOT_SET_V02;
        break;
    }
    p_PdsmExtPos->horReliability_valid = TRUE;

    /* Compute horizontal velocity from components and scale
    * to units of 0.25 m/s.
    */


    /*  Altitude With Respect to Ellipsoid */
    p_PdsmExtPos->altitudeWrtEllipsoid = p_GnssFixRpt->z_NavPos.d_PosLla[2];
    p_PdsmExtPos->altitudeWrtEllipsoid_valid = TRUE;

    /*  Altitude With Respect to Sea Level */
    p_PdsmExtPos->altitudeWrtMeanSeaLevel = p_GnssFixRpt->z_NavPos.d_PosLla[2] + 500;
    p_PdsmExtPos->altitudeWrtMeanSeaLevel_valid = TRUE;

    /*  Vertical Uncertainty */
    p_PdsmExtPos->vertUnc = p_GnssFixRpt->z_NavPos.f_VuncVertMps;
    p_PdsmExtPos->vertUnc_valid = TRUE;

    /*  Vertical Confidence */
    p_PdsmExtPos->vertConfidence = 68;
    p_PdsmExtPos->vertConfidence_valid = TRUE;

    /*  Vertical Reliability */
    switch (p_GnssFixRpt->z_NavPos.e_VertRelIndicator)
    {
      case PDSM_LITE_POSITION_RELIABILITY_NOT_SET:
        /* Not set, reliability info is not present */
        p_PdsmExtPos->vertReliability = eQMI_LOC_RELIABILITY_NOT_SET_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_VERY_LOW:
        /* Not checked, use at user's own risk */
        p_PdsmExtPos->vertReliability = eQMI_LOC_RELIABILITY_VERY_LOW_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_LOW:
        /* Limited cross-checked */
        p_PdsmExtPos->vertReliability = eQMI_LOC_RELIABILITY_LOW_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_MEDIUM:
        /* Limited cross-checked */
        p_PdsmExtPos->vertReliability = eQMI_LOC_RELIABILITY_MEDIUM_V02;
        break;

      case PDSM_LITE_POSITION_RELIABILITY_HIGH:
        /* Robust cross-checked */
        p_PdsmExtPos->vertReliability = eQMI_LOC_RELIABILITY_HIGH_V02;
        break;

      default:
        /* Default reliability . Not set */
        p_PdsmExtPos->vertReliability = eQMI_LOC_RELIABILITY_NOT_SET_V02;
        break;
    }
    p_PdsmExtPos->vertReliability_valid = TRUE;

    /*  Vertical Speed */
    
    /*  UTC Timestamp */
    if (tm_util_calculate_utc(p_GnssFixRpt,&z_pdsm_lite_utc_time, NULL) == TRUE)
    {
      // We have the UTC time in YearMonthHourMinuteSecond calculated from GPS time ( Since Jan 6 1980 ) .
      // Convert the UTC time we have into UNIX timestamp ( Since Jan1 1970 )
      gnss_GetUnixEpochFromUTC((pdsm_utc_time_s_type*)&z_pdsm_lite_utc_time,
                               &t_timestamp_utc_s);
      /* Scale it to milliseconds, as the TimeStamp now is in seconds. */
      t_timestamp_utc_ms = t_timestamp_utc_s * 1000;
      /* Now lets add more precision.
         utc_time.sec --> is seconds scaled by 100.
                          ( ie. 1sec is reported as 100 &
                            1.99 sec is reported as 199 which is 1990msec)
         GetUnixEpochFromUTC gives us back in seconds. So we can add the tenths
         of a second and hundredth of a second.
   
         Note: Currently the precision of hundredth of a second is lost in
               translation in SM.
      */
      t_timestamp_utc_ms +=
         ((z_pdsm_lite_utc_time.sec % 100) * 10);
      p_PdsmExtPos->timestampUtc = t_timestamp_utc_ms;
      p_PdsmExtPos->timestampUtc_valid = TRUE;
    }
    else
    {
      p_PdsmExtPos->timestampUtc_valid = FALSE;
    }

    /*  GPS Time */
    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsGpsTimeValid)
    {
      /* Week number of the fix*/
      p_PdsmExtPos->gpsTime.gpsWeek = p_GnssFixRpt->z_NavPos.w_GpsWeek;
      p_PdsmExtPos->gpsTime.gpsTimeOfWeekMs = p_GnssFixRpt->z_NavPos.q_GpsTimeMs;
      p_PdsmExtPos->gpsTime_valid = TRUE;
    }

    /*  Time Uncertainty */
    if (p_GnssFixRpt->z_NavPos.z_SmPosFlags.b_IsTuncValid)
    {
      p_PdsmExtPos->timeUnc = p_GnssFixRpt->z_NavPos.f_ClockBiasUncMs;
      p_PdsmExtPos->timeUnc_valid = TRUE;
    }

    /*  Time Source */
    /* Convert the time source which is of the type pdsm_lite_extended_time_src_e_type into
       the type used by extended status report */
    switch (p_GnssFixRpt->z_NavPos.u_InitGpsTimeSrc)
    {
      case PDSM_LITE_GNSS_TIME_SRC_UNKNOWN:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_UNKNOWN_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_RESERVED:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_SYSTEM_TIMETICK_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_NETWORK_TIME_TRANSFER:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_NETWORK_TIME_TRANSFER_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_NETWORK_TIME_TAGGING:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_NETWORK_TIME_TAGGING_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_EXTERNAL_INPUT:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_EXTERNAL_INPUT_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_GPS_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_TOW_DECODE_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_TOW_CONFIRMED:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_TOW_CONFIRMED_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_TOW_WEEK_CONFIRMED:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_TOW_AND_WEEK_CONFIRMED_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_NAV_SOLUTION:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_NAV_SOLUTION_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_SOLVE_FOR_TIME:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_SOLVE_FOR_TIME_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_GLO_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_GLO_TOW_DECODE_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_TIME_TRANSFORM :
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_TIME_TRANSFORM_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_GSM_SLEEP_TIME_TAGGING:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_GSM_SLEEP_TIME_TAGGING_V02;
        break;
      case PDSM_LITE_GNSS_TIME_SRC_QZSS_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_QZSS_TOW_DECODE_V02;
        break;
      case  PDSM_LITE_GNSS_TIME_SRC_BDS_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_BDS_TOW_DECODE_V02;
        break;
      case  PDSM_LITE_GNSS_TIME_SRC_GAL_TOW_DECODE:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_GAL_TOW_DECODE_V02;
        break;

      default:
        p_PdsmExtPos->timeSrc = eQMI_LOC_TIME_SRC_INVALID_V02;
        break;
    }
    if (p_PdsmExtPos->timeSrc != eQMI_LOC_TIME_SRC_INVALID_V02)
    {
      p_PdsmExtPos->timeSrc_valid = TRUE;
    }

  }
  else
  {
    p_PdsmExtPos->status = eQMI_LOC_GENERAL_FAILURE_V02;
  }
#endif
}

/*===========================================================================

FUNCTION tm_core_calculate_2d_punc

DESCRIPTION
  This function is used to calculate the 2D Punc from the
  gnss_NavSolutionStructType data.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
float tm_core_calculate_2d_punc(const sm_FixInfoStructType *pz_GnssFixRpt)
{
  float f_Punc = CGPS_WHOLE_EARTH_CEP99;
  if (pz_GnssFixRpt->z_NavPos.z_PosFlags.b_IsValid)
  {
    f_Punc = (FLT)sqrt(pz_GnssFixRpt->z_NavPos.f_ErrorEllipse[1] *
                       pz_GnssFixRpt->z_NavPos.f_ErrorEllipse[1] +
                       pz_GnssFixRpt->z_NavPos.f_ErrorEllipse[2] *
                       pz_GnssFixRpt->z_NavPos.f_ErrorEllipse[2]);
  }
  return f_Punc;
}

/*===========================================================================
tm_core_internal_update_gps_state_data

Description:
   This function is used to update the internal state of TM's GPS state info data.

Parameters: 
  p_gnss_fix_rpt: The position report from MGP.  

Return value: 
  Void.
=============================================================================*/
void tm_core_internal_update_gps_state_data(const sm_FixInfoStructType *p_gnss_fix_rpt)
{
  float f_Punc = 0;

  f_Punc = tm_core_calculate_2d_punc(p_gnss_fix_rpt);
  /* Convert latitude from radians to degrees and scale. */
  if (p_gnss_fix_rpt->z_NavPos.z_PosFlags.b_IsValid)
  {
    tm_core_info.gps_state_info.flags |= PDSM_LITE_PD_GPS_STATE_POS_VALID;
  }

  if (p_gnss_fix_rpt->z_NavPos.f_PuncVertMeters < 10000)
  {
    tm_core_info.gps_state_info.loc_unc_vertical = (uint16)p_gnss_fix_rpt->z_NavPos.f_PuncVertMeters;
    tm_core_info.gps_state_info.flags |= PDSM_LITE_PD_GPS_STATE_ALT_VALID;
  }
  else
  {
    tm_core_info.gps_state_info.loc_unc_vertical = 10000;
  }
  tm_core_info.gps_state_info.lat = (int32)(p_gnss_fix_rpt->z_NavPos.d_PosLla[0] * C_2_32_OVER_PI);

  /* Convert longitude from radians to degrees and scale */
  tm_core_info.gps_state_info.lon = (int32)(p_gnss_fix_rpt->z_NavPos.d_PosLla[1] * C_2_31_OVER_PI);
  tm_core_info.gps_state_info.pos_unc  = (uint32)f_Punc;
  tm_core_info.gps_state_info.altitude = (int16)(tm_util_nearest_long(p_gnss_fix_rpt->z_NavPos.d_PosLla[2]));
  tm_core_info.gps_state_info.engine_state = tm_core_info.e_cached_rcvr_state;

  tm_core_info.gps_state_info.loc_unc_vertical = (uint16)(p_gnss_fix_rpt->z_NavPos.f_PuncVertMeters);
  if (p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsGpsTimeValid)
  {
    tm_core_info.gps_state_info.flags |= PDSM_LITE_PD_GPS_STATE_TIME_MS_VALID;
    tm_core_info.gps_state_info.flags |= PDSM_LITE_PD_GPS_STATE_TIME_WN_VALID;
  }
  tm_core_info.gps_state_info.timestamp_gps_week = p_gnss_fix_rpt->z_NavPos.w_GpsWeek;
  tm_core_info.gps_state_info.timestamp_tow_ms = p_gnss_fix_rpt->z_NavPos.q_UtcTimeMs;

  if (p_gnss_fix_rpt->z_NavPos.z_SmPosFlags.b_IsTuncValid)
  {
    tm_core_info.gps_state_info.flags |= PDSM_LITE_PD_GPS_STATE_TUNC_VALID;
  }
  tm_core_info.gps_state_info.time_unc_us = (uint32)(p_gnss_fix_rpt->z_NavPos.f_ClockBiasUncMs);
}

/*===========================================================================
tm_core_handle_gps_state_info_req_type_query_state

Description:
   This function is used to handle the GPS state information repsonse to the request made by
   querying from PDAPI. 

Parameters: 
  p_gnss_fix_rpt: The position report from MGP.  

Return value: 
  Void.
=============================================================================*/
static void tm_core_handle_gps_state_info_req_type_query_state(const sm_FixInfoStructType *p_gnss_fix_rpt)
{

  tm_pdapi_pd_event_s_type pz_pd_event_info;
  tm_core_internal_update_gps_state_data(p_gnss_fix_rpt);

  if (tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].w_TmRcvdDataMask == TM_RCVD_ALL)
  {
    pz_pd_event_info.e_pd_event = PDSM_LITE_PD_STATUS_GPS_STATE_INFO;
    tm_core_xlate_gps_state_to_ext_status(&tm_core_info.gps_state_info, &(pz_pd_event_info.pd_event_data.pd_info_type.pd_info.pz_gps_state_info));
    tm_pdapi_pd_event_callback(&pz_pd_event_info, TM_PDAPI_SESS_TYPE_NONE);

    tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE));
    tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE].w_TmRcvdDataMask = 0;
  }
}
/*===========================================================================
tm_core_handle_gps_state_info_req_type_best_avail_pos_qmi

Description:
   This function is used to handle the GPS state information repsonse to the request made for
   best available position from  PDAPI.

Parameters: 
  p_gnss_fix_rpt: The position report from MGP. 

Return value: 
  Void.
=============================================================================*/
static void tm_core_handle_gps_state_info_req_type_best_avail_pos_qmi(const sm_FixInfoStructType *p_gnss_fix_rpt)
{
  tm_pdapi_pd_event_s_type pz_pd_event_info = {0};
  qmiLocGetBestAvailablePositionIndMsgT_v02 *p_BestAvailPosInd = NULL;
  
  pdsm_lite_pd_cmd_err_e_type      e_cmd_err = PDSM_LITE_PD_CMD_ERR_NOERR;
  //pdsm_lite_cmd_s_type *p_cmd_type;
#ifdef FEATURE_CGPS_XTRA_T
  float                       f_Punc = tm_core_calculate_2d_punc(p_gnss_fix_rpt);
#endif

  // This is in response from ME to the get best available position request from PDAPI .
  // Convert the data into the format of QMI and send it as an extended status event callback.
  p_BestAvailPosInd = & (pz_pd_event_info.pd_event_data.pd_info_type.pd_info.pz_BestAvailPosInd);
  pz_pd_event_info.pd_event_data.pd_info_type.client_id = 
    tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI].q_ClientId;
  pz_pd_event_info.e_pd_event = PDSM_LITE_PD_EVENT_BEST_AVAIL_POS_REPORT;
  tm_core_xlate_pd_best_avail_pos_to_qmi(p_gnss_fix_rpt,
                                  p_BestAvailPosInd);
  e_cmd_err = tm_core_get_best_avail_pos_err_check();
  if (e_cmd_err == PDSM_LITE_PD_CMD_ERR_NOERR)
  {
    tm_pdapi_pd_event_callback(&pz_pd_event_info, TM_PDAPI_SESS_TYPE_NONE);
  }
  else
  {
    // Notify the client that there was an error
    //tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
  }

  tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI));
  tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI].w_TmRcvdDataMask = 0;
}

/*===========================================================================
tm_core_handle_gps_state_info_req_type_avail_wwan_pos_qmi

Description:
   This function is used to handle the GPS state information repsonse to the request made for
   available WWAN position from PDAPI.

Parameters:
  p_gnss_fix_rpt: The position report from MGP.

Return value:
  Void.
=============================================================================*/
static void  tm_core_handle_gps_state_info_req_type_avail_wwan_pos_qmi(const sm_FixInfoStructType *p_gnss_fix_rpt)
{
  tm_pdapi_pd_event_s_type pz_pd_event_info = {0};
  qmiLocGetAvailWwanPositionIndMsgT_v02 * p_AvailWwanPosInd;

  pdsm_lite_pd_cmd_err_e_type      e_cmd_err = PDSM_LITE_PD_CMD_ERR_NOERR;
#ifdef FEATURE_CGPS_XTRA_T
  float                       f_Punc = tm_core_calculate_2d_punc(p_gnss_fix_rpt);
#endif

  // This is in response from ME to the get available WWAN position request from PDAPI.
  // Convert the data into the format of QMI and send it as an extended status event callback.
  p_AvailWwanPosInd = &(pz_pd_event_info.pd_event_data.pd_info_type.pd_info.pz_AvailWwanPosInd);
  pz_pd_event_info.pd_event_data.pd_info_type.client_id  = 
  tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI].q_ClientId;
  pz_pd_event_info.e_pd_event = PDSM_LITE_PD_STATUS_EVENT_AVAIL_WWAN_POS_REPORT;
  tm_core_xlate_pd_wwan_pos_to_qmi(p_gnss_fix_rpt,
                                  p_AvailWwanPosInd);
  e_cmd_err = tm_core_get_best_avail_pos_err_check();
  if (e_cmd_err == PDSM_LITE_PD_CMD_ERR_NOERR)
  {
      tm_pdapi_pd_event_callback(&pz_pd_event_info, TM_PDAPI_SESS_TYPE_NONE);
  }

  tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI));
  tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI].w_TmRcvdDataMask = 0;
}

#if 0
/*===========================================================================
tm_core_handle_gps_state_info_req_type_available_pos_qmi

Description:
   This function is used to handle the GPS state information repsonse to the request made for
   available GNSS position from PDAPI.

Parameters:
  p_gnss_fix_rpt: The position report from MGP.
  b_ReqType:    The position report source type from MGP.

Return value:
  Void.
=============================================================================*/
static void tm_core_handle_gps_state_info_req_type_available_pos_qmi(const sm_FixInfoStructType *p_gnss_fix_rpt, sm_ReqEnumType b_ReqType)
{
  tm_pdapi_pd_event_s_type pz_pd_event_info = { 0 };
  pdsm_lite_pd_cmd_err_e_type      e_cmd_err = PDSM_LITE_PD_CMD_ERR_NOERR;

  if ((NULL == p_gnss_fix_rpt) ||
      (b_ReqType >= GNSS_ARRAY_SIZEOF(tm_core_info.session_info.z_ReqClients)))
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_core: NULL p_gnss_fix_rpt? %d, ReqType %d", NULL == p_gnss_fix_rpt, b_ReqType);
    return;
  }

#ifdef FEATURE_CGPS_XTRA_T
  float  f_Punc = tm_core_calculate_2d_punc(p_gnss_fix_rpt);
#endif

  // This is in response from ME to the get available position request from PDAPI.
  // Convert the data into the format of QMI and send it as an extended status event callback.
  pd_ext_status_info.client_id       = tm_core_info.session_info.z_ReqClients[b_ReqType].q_ClientId;
  pd_ext_status_info.ext_status_type = PDSM_LITE_EXT_STATUS_AVAIL_POS_REPORT;
  tm_core_xlate_pd_ext_pos_to_qmi(p_gnss_fix_rpt,
                                  &pd_ext_status_info.ext_status_info.ext_pos_report_qmi_type);
  e_cmd_err = tm_core_get_best_avail_pos_err_check();
  if (e_cmd_err == PDSM_LITE_PD_CMD_ERR_NOERR)
  {
#ifdef FEATURE_CGPS_XTRA_T
#ifndef FEATURE_CGPS_TLE_IOT
    tle_StartKeepWarmPeriod();
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core PUNC:%u", (uint32)f_Punc, 0, 0);
    if (f_Punc > PUNC_THRESHOLD_FOR_XTRA_T)
    {
      // Punc is greater than 25 Km.
      // Start the Xtra-T download session.
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core: initiate a GTP session", 0, 0, 0);
      tle_StartPuncBasedSession(f_Punc);
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core: No need to initiate GTP session", 0, 0, 0);
    }
#endif
#endif
  }

  tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(b_ReqType));
  tm_core_info.session_info.z_ReqClients[b_ReqType].w_TmRcvdDataMask = 0;
}
#endif
/*===========================================================================

FUNCTION tm_core_report_gps_state_info

DESCRIPTION
  This function is called by MGP to return the gps state info 
  for on-demand positioning and user requested state info
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_report_gps_state_info(void *pos_data)
{
  sm_FixReportStructType   z_GpsBestAvailPos = { 0 };
  sm_FixInfoStructType *p_FixInfo = &z_GpsBestAvailPos.z_FixInfo;
  sm_FixInfoStructType propogated_fix = {0};
   FLT f_hepeForPropgatedCachedFix = (FLT)INT16_MAX;
   FLT f_hepeForTLeFix = (FLT)INT16_MAX;
  /* Function arguments sanity check */
  if (NULL == pos_data)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_core_report_gps_state_info: NULL argument received");
    return;
  }

  /* Copy contents of pointer argument passed by MGP before it gets overwritten */
  z_GpsBestAvailPos = *((sm_FixReportStructType *)pos_data);

  /* Pointer arguments sanity check */
  if (z_GpsBestAvailPos.b_ReqType > SM_GPS_STATE_INFO_REQ_TYPE_MAX)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_core_report_gps_state_info: NULL pointer argument received");
    return;
  }

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_core_report_gps_state_info: Rcvd response for request %d from MGP", z_GpsBestAvailPos.b_ReqType);

  /* propagate fix in case if the fix is gtp fix*/
  if( (p_FixInfo->z_NavPos.z_PosFlags.b_IsValid) && 
       ((p_FixInfo->z_NavPos.z_SmPosFlags.b_IsCellId) && (p_FixInfo->z_NavPos.z_SmPosFlags.b_IsWwanRanging != TRUE)) )
  {
    f_hepeForTLeFix = tm_core_get_hepe_for_propagated_fix_report(p_FixInfo, &propogated_fix);
    *p_FixInfo = propogated_fix;
  }

  tm_core_info.session_info.z_ReqClients[z_GpsBestAvailPos.b_ReqType].w_TmRcvdDataMask |= TM_RCVD_TUNC_FROM_ME;
  tm_core_info.session_info.z_ReqClients[z_GpsBestAvailPos.b_ReqType].w_TmRcvdDataMask |= TM_RCVD_POS_FROM_PE;

  switch (z_GpsBestAvailPos.b_ReqType)
  {
    case SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE:
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Gps state info for query state recv. ");
      tm_core_handle_gps_state_info_req_type_query_state(p_FixInfo);
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI:
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Gps state info for Best avail Pos recv for QMI. ");

      if(LM_DMM_STATE_LOADED != lm_get_dmm_state())
      {
        /*Calcullate hepe for Cached fix*/
        if(z_gnss_position_cache.cached_gnss_pos_valid)
        {
          f_hepeForPropgatedCachedFix = tm_core_get_hepe_for_propagated_fix_report(&z_gnss_position_cache.z_cached_gnss_pos_rpt, &propogated_fix);
        }

        if(f_hepeForPropgatedCachedFix < f_hepeForTLeFix)
        {
          *p_FixInfo = propogated_fix;
        }
      }
      tm_core_handle_gps_state_info_req_type_best_avail_pos_qmi(p_FixInfo);
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI:
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Gps state info for Avail WWAN Pos recv for QMI. ");
      tm_core_handle_gps_state_info_req_type_avail_wwan_pos_qmi(p_FixInfo);
	  tm_gfc_qmi_inject_ext_pos(p_FixInfo);
      break;
/*  [TBD][APP]These cases are most probably not required as this function sends IPC message ID PDSM_LITE_EXT_STATUS_AVAIL_POS_REPORT 
     to loc_mw. This message gets eventually converted to QMI_LOC_SECURE_AVAILABLE_POSITION_V02. Which is not supported 
*/
#if 0 
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_GNSS_AVAIL_POS_QMI:
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_REFLOC_AVAIL_POS_QMI:
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_COARSE_AVAIL_POS_QMI:
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_GILE_AVAIL_POS_QMI:
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Gps state info for Avail Pos recv for QMI. ");
      tm_core_handle_gps_state_info_req_type_available_pos_qmi(p_FixInfo, z_GpsBestAvailPos.b_ReqType);
      break;
#endif
#ifdef FEATURE_CGPS_XTRA_T
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_XTRA_T:
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Gps state info for Best avail Pos recv for XTRA_T. ");
      tm_xtra_t_handle_best_avail_pos(p_FixInfo);

      tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_XTRA_T));
      tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_XTRA_T].w_TmRcvdDataMask = 0;
      break;
#endif

    case SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_GFC:
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Gps state info for Avail WWAN Pos recv for GFC. ");
      tm_gfc_qmi_inject_ext_pos(p_FixInfo);
      tm_core_info.session_info.q_ReqClientMask &= ~(TM_CORE_CLIENT_BITMASK(SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_GFC));
      tm_core_info.session_info.z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_GFC].w_TmRcvdDataMask = 0;
      break;
    case SM_GPS_STATE_INFO_REQ_TYPE_GET_GTP_WWAN_FIX:
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Gps state info for Avail WWAN Pos recv for session. ");
      tm_core_handle_gps_state_info_req_type_get_gtp_wwan_fix(p_FixInfo);
      break;
    default:
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Get GPS State unknown Req Type %d", z_GpsBestAvailPos.b_ReqType);
      break;
  }
}


/*===========================================================================

FUNCTION tm_core_get_utc_offset

DESCRIPTION
  This function returns the UTC offset obtained from MGP. If UTC offset is invalid, then the UTC offset will contain
  default value from MGP.

RETURN VALUE
  Boolean: TRUE: If UTC offset is valid
                FALSE: If UTC offset is invalid

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean tm_core_get_utc_offset(uint8 *p_utc_offset)
{
  boolean v_leap_sec_valid = FALSE;

  *p_utc_offset = tm_core_info.config_info.z_LeapSecondInfo.u_DeltaLeapSec;

  if (0 == tm_core_info.config_info.z_LeapSecondInfo.u_DeltaLeapSecUnc)
  {
    v_leap_sec_valid = TRUE;
  }

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Leap Second = %u, valid = %u",
        tm_core_info.config_info.z_LeapSecondInfo.u_DeltaLeapSec, v_leap_sec_valid);

  return v_leap_sec_valid;
}

#if 0
/*===========================================================================

FUNCTION tm_core_save_ref_position

DESCRIPTION
  This function saves the reference position received from the network

RETURN VALUE


DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_save_ref_position(pdsm_lite_ext_status_info_s_type   *pd_ext_status_info)
{

  z_cached_ref_pos_info.cached_ref_pos_valid = TRUE;
  memscpy(&z_cached_ref_pos_info.pd_ext_status_info, sizeof(z_cached_ref_pos_info.pd_ext_status_info),
          pd_ext_status_info, sizeof(*pd_ext_status_info));

  z_cached_ref_pos_info.ref_pos_reported = FALSE;
}
#endif
/*====================================================================
FUNCTION tm_core_get_xtra_validity_age

DESCRIPTION

  This function gets the NV item for the xtra validity age

DEPENDENCIES

RETURN VALUE


SIDE EFFECTS
  none

=====================================================================*/
void tm_core_get_xtra_validity_age(uint16 *p_age)
{

  if (p_age)
  {
    *p_age = tm_core_info.config_info.preferred_xtra_age;
 
  // Limit min age to between 72 and 168 hours
    if(*p_age < TM_XTRA_MIN_FILE_AGE_HRS)
	{
     *p_age = TM_XTRA_MIN_FILE_AGE_HRS;
  }
    if(*p_age > TM_XTRA_MAX_FILE_AGE_HRS)
	{
     *p_age = TM_XTRA_MAX_FILE_AGE_HRS;
	}
  }
}


/*===========================================================================

FUNCTION tm_core_time_validity

DESCRIPTION
   Reports whether the time from time_get_ms is valid or not

DEPENDENCIES

RETURN VALUE
  boolean: TRUE - time is valid
           FALSE - time is not valid

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_core_time_validity()
{


#ifndef FEATURE_GNSS_LEAN_BUILD

  time_type curr_time;
  time_get_ms(curr_time);

  if (curr_time[1] > MSB_MSEC_ELAPSED_SINCE_DEFAULT)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TSG: Time is valid", 0, 0, 0);
    return TRUE;
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TSG: Time is invalid", 0, 0, 0);
    return FALSE;
  }


#else
  return FALSE;
#endif

}
/*[DEL] This is probably not needed because we are not storing position in 
cache for position injection*/


/*===========================================================================
FUNCTION tm_core_clear_cached_pos

DESCRIPTION Clears the RAM copy of the position

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_clear_cached_pos(void)
{
  lm_request_info_s_type p_req;
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM_CORE: clearing cached position");
  
  memset(&p_req, 0, sizeof(lm_request_info_s_type));
  
#if 0
  memset((void *)&z_cached_pos_info, 0, sizeof(tm_core_cached_pos_s_type));
  memset((void *)&z_cached_ref_pos_info, 0, sizeof(tm_core_cached_ref_pos_s_type));
#endif

  /*Clear SM cached position*/
  z_gnss_position_cache.cached_gnss_pos_valid = FALSE;
  memset((void *)&z_gnss_position_cache, 0, sizeof(z_gnss_position_cache));

  /*Clear CPI cached position*/
  p_req.e_req_type = LM_REQUEST_DELETE_MGP_INFO;
  p_req.z_request.z_delete_request.z_mgp_info.q_DeletePosMask = C_DELETE_INFO_POS_INFO;
  (void)lm_request(&p_req);

}

/*===========================================================================
FUNCTION tm_core_is_standalone_active

DESCRIPTION
Check if Standalone session is in progress.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE
XTRA command status
===========================================================================*/
boolean tm_core_is_standalone_active(void)
{
  if (tm_core_info.session_info.from_protocol == TM_PRTL_TYPE_STANDALONE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* tm_core_is_standalone_active */
/*===========================================================================
FUNCTION tm_core_handle_engine_error_recovery_report

DESCRIPTION
  This function handles engine error recovery report (usually it'll be sent to
  QMI)
  
DEPENDENCIES

RETURN VALUE
  TRUE if success

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_engine_error_recovery_report(gnss_engine_err_recovery_rpt *pErrReport)
{
  boolean v_ret_val = FALSE;
  tm_cm_phone_state_info_s_type phone_state_info;
  if (pErrReport == NULL)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL engine error recovery report");
    return FALSE;
  }

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Recovery:Reset type %d", pErrReport->reset_type);

  /*Get the phone state to check for E911 state*/
  memset(&phone_state_info, 0, sizeof(tm_cm_phone_state_info_s_type));
  tm_cm_iface_get_phone_state(&phone_state_info);
  return v_ret_val;
}

/*===========================================================================
FUNCTION tm_core_handle_engine_dpo_status_report

DESCRIPTION
  This function handles engine DPO status report from MGP(usually it'll be sent to
  QMI)
  
DEPENDENCIES

RETURN VALUE
  TRUE if success

SIDE EFFECTS

===========================================================================*/
extern void tm_core_handle_engine_dpo_status_report(sm_DpoStatusReportStructType *p_DpoStatusReport)
{
  tm_pdapi_pd_event_s_type pz_pd_event_info;
  memset(&pz_pd_event_info, 0, sizeof(pz_pd_event_info));
  if (p_DpoStatusReport == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL DPO status report from MGP", 0, 0, 0);
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Rcvd DPO status report from MGP Engaged %x DisEngagement Reason %x", p_DpoStatusReport->dpo_status_mask, p_DpoStatusReport->dpo_disengage_reason, 0);
    pz_pd_event_info.e_pd_event = PDSM_LITE_PD_STATUS_DPO_STATUS;
    /*Send the DPO status indication to Geofence Module*/
    gm_update_dpo_status_indication(p_DpoStatusReport);
  }
  pz_pd_event_info.pd_event_data.pd_info_type.client_id =  
  tm_core_info.session_info.pd_param.client_id;
  pz_pd_event_info.e_pd_event = PDSM_LITE_PD_STATUS_DPO_STATUS;
  tm_pdapi_pd_event_callback(&pz_pd_event_info,TM_PDAPI_SESS_TYPE_NONE);
}

/*===========================================================================
FUNCTION tm_core_handle_best_avail_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  TRUE if best avail pos was requested
SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_best_avail_pos
(
   tm_core_get_best_avail_pos_param_s_type *p_get_best_avail_pos_param
   )
{
  tm_core_get_gps_state_param_s_type get_gps_state_param;

  if (p_get_best_avail_pos_param == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null input pointer", 0, 0, 0);
    return FALSE;
  }
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Calling tm_core_handle_best_avail_pos() for source %d ", p_get_best_avail_pos_param->source, 0, 0);

  get_gps_state_param.source = p_get_best_avail_pos_param->source;
  get_gps_state_param.cmd.p_cmd_type = p_get_best_avail_pos_param->cmd.p_cmd_type;
  tm_core_get_gps_state(&get_gps_state_param); /*lint !e506 */
  return TRUE;
}

/*===========================================================================
FUNCTION tm_core_handle_avail_wwan_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  TRUE if avail WWAN pos was requested
SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_avail_wwan_pos
(
   tm_core_get_avail_wwan_pos_param_s_type *p_GetAvailWwanPosParam
   )
{
  tm_core_get_gps_state_param_s_type z_GetGpsStateParam;

  if (p_GetAvailWwanPosParam == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null input pointer", 0, 0, 0);
    return FALSE;
  }
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Calling tm_core_handle_avail_wwan_pos() for source %d", p_GetAvailWwanPosParam->source, 0, 0);

  z_GetGpsStateParam.source = p_GetAvailWwanPosParam->source;
  z_GetGpsStateParam.cmd.p_cmd_type = p_GetAvailWwanPosParam->cmd.p_cmd_type;
  tm_core_get_gps_state(&z_GetGpsStateParam);
  return TRUE; /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_core_get_param_msg_handler

DESCRIPTION
  This function handles the get param ipc msg, and sends an ipc msg as a response
  with the param value retrieved.
  
DEPENDENCIES

RETURN VALUE TRUE/FALSE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_param_msg_handler(void *p_Data)
{
  os_IpcMsgType             *ipc_msg_ptr = NULL;

  uint32  length = 0;
  boolean status = FALSE;
  uint8   i      = 0;

  if (!p_Data)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL input p_Data ptr", 0, 0, 0);
    return status;
  }
  return status;
}

/*===========================================================================
FUNCTION tm_core_enable_DpoStatusReport

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  TRUE is DPO Status Report was enabled

SIDE EFFECTS

===========================================================================*/
boolean tm_core_enable_DpoStatusReport(boolean enable)
{


#ifndef FEATURE_GNSS_LEAN_BUILD
  return mgp_EnableDpoStatusReport(enable);

#else
  return FALSE;
#endif

}

/*===========================================================================

FUNCTION tm_core_external_cell_db_proc

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of 
  an incoming external cell database update message. 
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/


boolean tm_core_external_cell_db_proc(pdsm_lite_cell_info_s_type *external_cell_db_info_ptr)
{
  prtl_event_u_type  *event_payload_ptr = NULL;
  boolean ret_val = TRUE;

  //Update tm_cm with external RAT changes
  tm_cm_process_external_srv_changed(/*This is to be removed once tm_cm_iface is modified*/
                                       (pdsm_lite_cell_info_s_type *)external_cell_db_info_ptr);

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TM got external CellDB update", 0, 0, 0);


  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "External cell info: MCC = %d, MNC = %d, LAC = %d",
        external_cell_db_info_ptr->cellInfo.gsmCell.MCC,
        external_cell_db_info_ptr->cellInfo.gsmCell.MNC,
        external_cell_db_info_ptr->cellInfo.gsmCell.LAC);

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "External cell info: CID = %d, serviceStatus = %d, RAT = %d",
        external_cell_db_info_ptr->cellInfo.gsmCell.CID,
        external_cell_db_info_ptr->serviceStatus,
        external_cell_db_info_ptr->activeRAT);


  /* allocate buffer for sending event to protocol submodule */
  event_payload_ptr = (prtl_event_u_type *)pdsm_getbuf(sizeof(prtl_event_u_type));
  if (event_payload_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Can't get buffer", 0, 0, 0);
    return FALSE;
  }

  event_payload_ptr->cell_db_event_payload.cell_db_event = PRTL_EXTERNAL_CELL_DB_UPD;
  event_payload_ptr->cell_db_event_payload.cell_db_event_payload.external_modem_cell_db_payload.cellInfo =
     external_cell_db_info_ptr->cellInfo;
  event_payload_ptr->cell_db_event_payload.cell_db_event_payload.external_modem_cell_db_payload.serviceStatus =
     external_cell_db_info_ptr->serviceStatus;
  event_payload_ptr->cell_db_event_payload.cell_db_event_payload.external_modem_cell_db_payload.activeRAT =
     external_cell_db_info_ptr->activeRAT;


  (void)pdsm_freebuf((char *)event_payload_ptr);

  return ret_val;

}


/*===========================================================================

FUNCTION tm_core_get_gm_engine_config

DESCRIPTION
  This function returns the GM Engine configuration

RETURN VALUE
  The timeout value in secs.

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_get_gm_engine_config(geofence_engine_config_s_type **gm_config)
{
  *gm_config = &(tm_core_info.config_info.gm_config);
}
#if 0
/*===========================================================================
  tm_core_handle_start_cpi

  Description:

     This function handles start CPI request.
     The only called of this function shall be the Tech Selector.

 Parameters:
    v_isEmergency: Is Emergency CPI

  Return value: 
    boolean: Pass\Fail
 
=============================================================================*/
boolean tm_core_handle_start_cpi(boolean v_isEmergency)
{
  tm_pdapi_pd_event_s_type pd_event_info = { 0 };

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "STARTING CPI", 0, 0, 0);
  tm_pdapi_pd_event_callback(&pd_event_info, TM_PDAPI_SESS_TYPE_NONE);

  return TRUE;

}

/*===========================================================================
  tm_core_handle_stop_cpi

  Description:
  
     This function handles stop CPI request.
     The only called of this function shall be the Tech Selector.

 Parameters:
    void

  Return value: 
    boolean: Pass\Fail

=============================================================================*/
boolean tm_core_handle_stop_cpi(boolean v_isEmergency)
{
  tm_pdapi_pd_event_s_type pd_event_info = { 0 };

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "STOPPING CPI", 0, 0, 0);
  tm_pdapi_pd_event_callback(&pd_event_info, TM_PDAPI_SESS_TYPE_NONE);

  return TRUE;
}
#endif
/*===========================================================================

FUNCTION tm_core_cache_leap_second_info

DESCRIPTION
  This function caches the Leap Second Info into TM, sent from ME

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_cache_leap_second_info(mgp_LeapSecondInfoStructType *pz_LeapSecInfo)
{

  if (NULL == pz_LeapSecInfo)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL pointer sent to tm_core_cache_leap_second_info");
    return;
  }

  /* Cache leap second info from ME */
  tm_core_info.config_info.z_LeapSecondInfo = *pz_LeapSecInfo;

  MSG_5(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Leap Second Info Updated: Leap Sec: %u, Leap Sec Unc: %u, Leap Sec Src: %u, UTC valid: %u, Correction: %d",
        pz_LeapSecInfo->u_DeltaLeapSec, pz_LeapSecInfo->u_DeltaLeapSecUnc, pz_LeapSecInfo->u_DeltaLeapSecSrc, pz_LeapSecInfo->u_UtcValid, pz_LeapSecInfo->b_Correction);
}

/*===========================================================================

FUNCTION tm_core_is_active_sess_mo

DESCRIPTION
  Checks if current on-going sess is MO AppTrack/Standalone

RETURN VALUE
  TRUE: If the on-going session is MO AppTrack/Standalone
  FALSE: If the on-going session is not MO AppTrack/Standalone

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_active_sess_mo()
{
  boolean retVal = FALSE;
  tm_sess_handle_type sess_handle = TM_SESS_HANDLE_NONE;

  sess_handle = tm_core_info.session_info.active_sess_handle;

  if (sess_handle > TM_SESS_HANDLE_NONE)
  {
    retVal = TRUE;
  }
  return retVal;
}

/*===========================================================================

FUNCTION tm_core_active_session_mode

DESCRIPTION
  Function to get the Session mode for the session in TM Core.

RETURN VALUE
  Return : TM OP_Mode if a session is active.

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
tm_sess_operation_mode_e_type tm_core_active_session_mode()
{
  return (tm_core_info.session_info.op_mode);
}

/*===========================================================================

FUNCTION tm_core_get_sarf_status

DESCRIPTION
  This function returns the current SARF status from TM config 

DEPENDENCIES

RETURN VALUE boolean: TRUE: RF test in progress; FALSE: No RF test in progress

SIDE EFFECTS

===========================================================================*/

boolean tm_core_get_sarf_status(void)
{
  return (tm_core_info.config_info.v_sarf_status);
}

boolean tm_core_is_hepe_satisfied(FLT f_ErrorEllipse[], FLT *f_hepe)
{
  boolean retval = FALSE;
  *f_hepe = (FLT)sqrt((f_ErrorEllipse[1] *
                      f_ErrorEllipse[1]) +
                     (f_ErrorEllipse[2] *
                     f_ErrorEllipse[2]));
  if(tm_core_info.session_info.pd_param.pd_qos.accuracy_threshold >= *f_hepe)
  {
    retval = TRUE;
  }
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Calculated HEPE = %d requested HEPE = %d", *f_hepe, 
  tm_core_info.session_info.pd_param.pd_qos.accuracy_threshold , 0);
  return retval;
}
/*===========================================================================

FUNCTION tm_core_get_hepe_for_propagated_fix_report()

DESCRIPTION
  This function takes the cached gnss fix propogates it and calculates HEPE
  of propogate fix.  

DEPENDENCIES

RETURN VALUE FLT: Returns HEPE of propogated fix. 

SIDE EFFECTS

===========================================================================*/


FLT tm_core_get_hepe_for_propagated_fix_report(sm_FixInfoStructType  * p_input_fix_rpt, sm_FixInfoStructType * p_output_propagated_fix_rpt)
{
  FLT  f_out_hepe = TM_MAX_HEPE ;
  FLT f_in_hepe = TM_MAX_HEPE ;
  sm_NavPositionStructType *pz_FixData = NULL;
  uint64 t_CurrentTimeMs = 0;
  FLT    f_FixAgeSecs = 0;
  uint64 t_CurrentTimeTickMs = 0;
  uint64 t_FixTimeMs = 0;
  gts_TimeType pz_GtsTime = {0};
  loc_InitUncDataStructType z_UncData;
  uint32 q_LeapSeconds = 0;
  uint8  utcoffset = 0;
  glo_TimeStructType glo_time = {0};
  gps_TimeStructType gps_time = {0};
  boolean b_isGtstime_valid = FALSE;
  char b_Buffer[150];

  if( (NULL != p_output_propagated_fix_rpt) && (p_input_fix_rpt!= NULL) )
  {
    pz_FixData = &p_input_fix_rpt->z_NavPos;
    if ( (TRUE == gts_GetTime(&pz_GtsTime)) && (TRUE == pz_GtsTime.b_Valid) )
    {
      b_isGtstime_valid = TRUE;
    }

    if( pz_FixData->z_SmPosFlags.b_IsSatellite ) 
    {
      /*PE is the source of fix*/
       if(TRUE == b_isGtstime_valid )
       {
         t_CurrentTimeMs = ((uint64)WEEK_MSECS * pz_GtsTime.w_GpsWeek) + (uint64)pz_GtsTime.q_GpsMsec; //Current GPS time in millisecond 
         t_FixTimeMs =    (uint64)pz_FixData->w_GpsWeek * WEEK_MSECS + pz_FixData->q_GpsTimeMs;// GPS time of fix in millisecond
       }
       else
       {
         MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "GTS Time is invalid. Can't propagate the fix");
         return  f_out_hepe;
       }
    }
    else if( (pz_FixData->z_SmPosFlags.b_IsCellId) && (pz_FixData->z_SmPosFlags.b_IsWwanRanging != TRUE)) 
    {
      /*GTP is the source of fix */
      t_CurrentTimeMs = (uint64)cgps_TimeTickGetMsec() ; //Current time in millisecond Since boot up
      t_FixTimeMs =    (uint64)pz_FixData->q_RefFCount ;//  Time of fix in millisecond Since boot up
    }
    else
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Not a valid source to propagate fix");
      return  f_out_hepe;
    }

    f_FixAgeSecs =      (FLT)((int64)(t_CurrentTimeMs - t_FixTimeMs)) / 1000.0f; //Age of fix in seconds

    (void)snprintf(b_Buffer, sizeof(b_Buffer), "t_CurrentTimeMs =%lld, t_FixTimeMs =%lld \r\n",
          t_CurrentTimeMs, t_FixTimeMs);
    MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", b_Buffer);

    /*Copy cached fix before propogation*/
    memscpy(p_output_propagated_fix_rpt,sizeof(*p_output_propagated_fix_rpt),
        p_input_fix_rpt, sizeof(*p_output_propagated_fix_rpt));
    memset(&z_UncData, 0, sizeof(z_UncData));
    z_UncData.f_ErrorEllipse[0] = pz_FixData->f_ErrorEllipse[0];
    z_UncData.f_ErrorEllipse[1] = pz_FixData->f_ErrorEllipse[1];
    z_UncData.f_ErrorEllipse[2] = pz_FixData->f_ErrorEllipse[2];
    z_UncData.f_PuncLatLonMeters[0] = pz_FixData->f_PuncLatLonMeters[0];
    z_UncData.f_PuncLatLonMeters[1] = pz_FixData->f_PuncLatLonMeters[1];
    z_UncData.f_PuncVertUncMeters = pz_FixData->f_PuncVertMeters;
    z_UncData.f_VuncEastNorthMps[0] = pz_FixData->f_VuncEastNorthMps[0];
    z_UncData.f_VuncEastNorthMps[1] = pz_FixData->f_VuncEastNorthMps[1];
    z_UncData.f_VuncVertMps = pz_FixData->f_VuncVertMps;
    z_UncData.q_GpsTimeMs = pz_FixData->q_GpsTimeMs;
    z_UncData.w_GpsWeek = pz_FixData->w_GpsWeek;

    /* Propagation of fix*/
    loc_PropagatePuncVelUnc(
    p_output_propagated_fix_rpt->z_NavPos.f_ErrorEllipse,
    &p_output_propagated_fix_rpt->z_NavPos.f_Punc3dMeters, 
    &p_output_propagated_fix_rpt->z_NavPos.f_Vunc3dMps,
    p_output_propagated_fix_rpt->z_NavPos.f_VuncEastNorthMps, 
    &p_output_propagated_fix_rpt->z_NavPos.f_VuncVertMps,
    p_output_propagated_fix_rpt->z_NavPos.f_PuncLatLonMeters,
    f_FixAgeSecs,
    &z_UncData,
    loc_getDefaultCarSpeedUncMps());

    if( b_isGtstime_valid )
    {
      p_output_propagated_fix_rpt->z_NavPos.w_GpsWeek = pz_GtsTime.w_GpsWeek;
      p_output_propagated_fix_rpt->z_NavPos.q_GpsTimeMs = pz_GtsTime.q_GpsMsec;
   
      tm_core_get_utc_offset(&utcoffset);
      q_LeapSeconds = utcoffset;
      if (p_output_propagated_fix_rpt->z_NavPos.q_GpsTimeMs / 1000L < q_LeapSeconds)
      {
        p_output_propagated_fix_rpt->z_NavPos.q_UtcTimeMs = p_output_propagated_fix_rpt->z_NavPos.q_GpsTimeMs + (WEEK_SECS - q_LeapSeconds) * 1000L;
      }
      else
      {
        p_output_propagated_fix_rpt->z_NavPos.q_UtcTimeMs = p_output_propagated_fix_rpt->z_NavPos.q_GpsTimeMs - q_LeapSeconds * 1000L;
      }

      p_output_propagated_fix_rpt->z_NavPos.f_ClockBiasUncMs = p_output_propagated_fix_rpt->z_NavPos.f_ClockBiasUncMs +
        fabsf((t_CurrentTimeMs - t_FixTimeMs) / 1000 * p_output_propagated_fix_rpt->z_NavPos.f_ClockDriftRate);

      /*Get Glonnas time using GPS time*/
      gps_time.q_GpsMsec = pz_GtsTime.q_GpsMsec;
      gps_time.w_GpsWeek =pz_GtsTime.w_GpsWeek;
      gps_time.f_ClkTimeBias = p_output_propagated_fix_rpt->z_NavPos.f_ClockBias;
      gps_time.f_ClkTimeUncMs = p_output_propagated_fix_rpt->z_NavPos.f_ClockBiasUncMs;

      gnss_ConvertGpsTime2GloTime(&gps_time, utcoffset,&glo_time);

      p_output_propagated_fix_rpt->z_NavPos.q_GloTimeMs = glo_time.q_GloMsec;
      p_output_propagated_fix_rpt->z_NavPos.u_GloNumFourYear = glo_time.u_FourYear;
      p_output_propagated_fix_rpt->z_NavPos.w_GloNumDaysInFourYear = glo_time.w_Days;
    }
    else
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "GTS time is not valid ");
    }

    /* calculating hepe of input fix report just to proint*/
    f_in_hepe = (FLT)sqrt((p_input_fix_rpt->z_NavPos.f_ErrorEllipse[1] *
                 p_input_fix_rpt->z_NavPos.f_ErrorEllipse[1]) +
                 (p_input_fix_rpt->z_NavPos.f_ErrorEllipse[2] *
                 p_input_fix_rpt->z_NavPos.f_ErrorEllipse[2]));

    /*Calculate HEPE for the propogated fix*/
     f_out_hepe = (FLT)sqrt((p_output_propagated_fix_rpt->z_NavPos.f_ErrorEllipse[1] *
              p_output_propagated_fix_rpt->z_NavPos.f_ErrorEllipse[1]) +
               (p_output_propagated_fix_rpt->z_NavPos.f_ErrorEllipse[2] *
              p_output_propagated_fix_rpt->z_NavPos.f_ErrorEllipse[2]));

    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Hepe =%d Propagate Hepe =%d",f_in_hepe,f_out_hepe);

  }
  return  f_out_hepe;
}

/*===========================================================================

FUNCTION tm_core_propogate_cached_gnss_position()

DESCRIPTION
  This function tries to propogate cached gnss position.  

DEPENDENCIES

RETURN VALUE boolean: TRUE: cached position is propogated sucessfuly; 
                      FALSE:cached position propogation failed. 

SIDE EFFECTS

===========================================================================*/

boolean tm_core_propogate_cached_gnss_position()
{
  boolean retval = FALSE;
  sm_NavPositionStructType *pz_FixData = NULL;
  gts_TimeType pz_GtsTime = {0};
  loc_InitUncDataStructType z_UncData;
  FLT f_hepe = 0;
  pdsm_lite_pd_event_type   pd_event;
  pdsm_lite_pd_info_s_type  pd_info = {0};
  qmiLocEventPositionReportIndMsgT_v02 * pz_PositionReportIndMsg = NULL;
  sm_FixInfoStructType propogated_fix = {0};
  sm_gm_pos_fix_info        fix_info;

  if(!(tm_core_is_gnss_control_fix_operation_allowed(GNSS_CONTROL_FIX_OPERATION_SM_CACHE)))
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "SM Cache Fix Propagation is disabled in the NV");
    return retval;
  }
  
  if(TRUE == z_gnss_position_cache.cached_gnss_pos_valid)
  {
    pz_FixData = &z_gnss_position_cache.z_cached_gnss_pos_rpt.z_NavPos;
    
    /*Try to get the current GTS time*/
    /*Calculate HEPE for the propogated fix*/
    f_hepe = tm_core_get_hepe_for_propagated_fix_report(&z_gnss_position_cache.z_cached_gnss_pos_rpt, &propogated_fix);
    if(f_hepe <= tm_core_info.session_info.pd_param.pd_qos.accuracy_threshold)
    {
      /*As the HEPE is satisfied before sending out the final fix diag needs 
      to be informed that the session was sucessfully started. We send this 
      event to diag only when HEPE is satisfied because if HEPE is not satisfied
      and we eventually have to fall to GNSS sesion the legacy code will again
      send out this event to diag. To avoid multiple transmission of same event
      we have to make sure that GNSS session will not be started.
      */
      sm_report_event_gps_fix_start( tm_core_event_counter++, MS_BASED_OP );
      pd_info.client_id = tm_core_info.session_info.pd_param.client_id;
      pz_PositionReportIndMsg = &pd_info.pd_info.pz_PositionReportIndMsg;
      fix_info.fix_type = GEOFENCE_POS_FIX_TYPE_FINAL; 
      fix_info.y_pos_fix_ext_info.z_gnss_fix_ext_info.b_is_nw_asst_fix = FALSE;
      fix_info.fix_src = GEOFENCE_POS_FIX_SRC_GNSS; /*#9x05 needs to be checked what should be stated 
                     as source of position report*/
      fix_info.pos_fix = propogated_fix;
      /*Send the LR to Geofence module*/
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Cached fix after propogation satisfies HEPE- sending to QMI", 0, 0, 0);
      (void)gm_position_update(&fix_info); 
      
      /*Translate the propogated fix to QMI*/
      pd_event = tm_util_xlate_fixt_report_to_qmi_loc_pos_rpt_Ind(&propogated_fix, pz_PositionReportIndMsg , 
                                                     PRTL_SESS_INFO_LR);
      /*Passing the fix to diag*/
      tm_pdapi_pos_log_generation(
                        pd_event,
                        tm_core_info.session_info.pd_param.client_id,
                        &propogated_fix,
                        propogated_fix.z_NavPos.z_PosFlags.b_IsSft,
                        propogated_fix.z_NavPos.z_PosFlags.b_IsKf
                       );
      tm_pdapi_pos_event_callback(pd_event,&pd_info, TM_PDAPI_SESS_TYPE_NONE);

      tm_core_handle_non_gnss_session_complete(&pd_info, TM_PDAPI_SESS_TYPE_NONE);

      retval = TRUE;
    }
    else
    {
      /*if fix doesn't meet the HEPE required cached gnss fix must be marked as 
      invalid
      */
      /*[QUERY] Should we mark this as invalid since this might not be satisfying for this 
       request but might satisfy a client with higher accurarcy threshold
      */
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Cached fix after propogation Doesnot satisfy HEPE- sending to QMI", 0, 0, 0);
    }
  }

  return retval;
}

/*===========================================================================

FUNCTION tm_core_notify_xtra_process_done()

DESCRIPTION
  This function is called by XTRA to inform the about
  the availability of XTRA data. Based on the status
  provided by XTRA it is decided whether to start 
  GNSS session or not.

DEPENDENCIES

RETURN VALUE  gnss constellation ID

SIDE EFFECTS

===========================================================================*/

void tm_core_notify_xtra_process_done (boolean status)
{
  tm_pdapi_pd_event_s_type p_pd_info_type;
   
  boolean v_is_any_gnss_session_active = tm_core_is_active_sess_mo();

  /* If current TM session requested for XTRA avail*/
  if(TRUE == tm_core_info.session_info.v_xtra_info_requested)
  {
    tm_core_info.session_info.v_xtra_info_requested = FALSE;
    
    /* XTRA is available */
    if(TRUE == status)
    { 
      if(FALSE == v_is_any_gnss_session_active)
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "XTRA available. Starts GNSS session");
        tm_core_start_gnss_session();
      }
      else
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Ignore as a GNSS session already active");
      }
    }
    else
    {  
     /* XTRA is not available.*/
    
      if(FALSE == v_is_any_gnss_session_active)
      {
      /*If no GNSS session is active, Send DONE to loc_mw*/
       MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "XTRA failure occured. Sending DONE event");
        p_pd_info_type.pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
        p_pd_info_type.e_pd_event = PDSM_LITE_PD_EVENT_END;
        p_pd_info_type.pd_event_data.pd_info_type.pd_info.end_status = PDSM_LITE_PD_END_GNSS_SRVC_NOT_AVAILABLE;
        tm_pdapi_pd_event_callback(&p_pd_info_type, TM_PDAPI_SESS_TYPE_NONE);
        sm_report_event_gps_fix_end(PDSM_LITE_PD_END_SESS_NONE);  
      
        /*Send PDSM_LITE_PD_EVENT_DONE to loc_mw to Schedule next fix*/
        tm_core_handle_non_gnss_session_complete(&p_pd_info_type.pd_event_data.pd_info_type, TM_PDAPI_SESS_TYPE_MI);
      }
      else
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Unexpected XTRA failure occured");
    }    
  }
  }
  else
  {
  /* If previous session which is no longer presnet on TM has asked for this request*/
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "XTRA processing completes for invalid TM session. Ignore.");
  }
 
}

/*===========================================================================

FUNCTION 
  tm_core_get_gtp_wwan_fix_timer_val()

DESCRIPTION
  This function calcullates the GTP WWAN time for active RAT. 

DEPENDENCIES

RETURN VALUE 

SIDE EFFECTS
===========================================================================*/

void tm_core_get_gtp_wwan_fix_timer_val(uint32 *q_TimerCount)
{
   tm_cm_phone_state_info_s_type     phone_state_info;
  uint32 gtp_wwan_fix_timer_val = tm_core_info.gtp_wwan_fix_timer_val;

  tm_cm_iface_get_phone_state(&phone_state_info);

            switch(phone_state_info.srv_system)
            {
   #ifdef FEATURE_CGPS_TLE_IOT
              case CGPS_SRV_SYS_LTE_NB1:
               /*Third byte has the timer duration*/
      *q_TimerCount =  ((gtp_wwan_fix_timer_val & 0x00FF0000)>>(0x18)) * 1000;
      break;

              case CGPS_SRV_SYS_LTE_M1:
               /*second byte has the timer duration*/
      *q_TimerCount =  ((gtp_wwan_fix_timer_val & 0x0000FF00)>>(0x10)) * 1000;
               break;
   #endif

              case CGPS_SRV_SYS_GSM:
               /*First byte has the timer duration*/
      *q_TimerCount = (gtp_wwan_fix_timer_val & 0x0000FF)* 1000;
               break;

              default:
      *q_TimerCount = 20*1000;//changing timer duration to 2 sec for trial
            }

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "serving system[%d] timer duration : %d ",
                                               phone_state_info.srv_system,
                                               q_TimerCount);
  return ;
}

/*===========================================================================

FUNCTION tm_core_req_gtp_wwan_fix_from_server(boolean b_Forced)

DESCRIPTION
  This function request fix from GTP wwan server. 

DEPENDENCIES

RETURN VALUE boolean: TRUE: if gtp fix is requested; 
                      FALSE:if gtp fix is not requested. 

SIDE EFFECTS

===========================================================================*/

boolean tm_core_req_gtp_wwan_fix_from_server(boolean b_Forced)
{
  uint32 q_TimerCount = 0;
  time_type            z_TimeMs={0};
  uint64 t_CurrentTimeMs = 0;
  tm_core_get_gps_state_param_s_type get_gps_state_param;
  pdsm_lite_cmd_s_type s_cmd ={0};

  get_gps_state_param.source = SM_GPS_STATE_INFO_REQ_TYPE_GET_GTP_WWAN_FIX;
  s_cmd.cmd.pd.client_id = tm_core_info.session_info.pd_param.client_id;
  get_gps_state_param.cmd.p_cmd_type = &s_cmd;

  /*Get time required to fetch fix from GTP WWAN*/
  tm_core_get_gtp_wwan_fix_timer_val(&q_TimerCount);

  /*If its not a forced request*/ 
  if(!b_Forced)
         {
    /*HEPE requirement is very accurate*/
    if(tm_core_info.session_info.pd_param.pd_qos.accuracy_threshold < TM_CORE_MIN_GTP_SERVING_HEPE)
            {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Accuracy thresold %d is very high. Not going to GTP server",
                                               (tm_core_info.session_info.pd_param.pd_qos.accuracy_threshold));
      return FALSE;
            }

    /*TBF is less than the time to fetch the fix from server*/
    if((tm_core_info.session_info.pd_param.pd_option.fix_rate.num_fixes != 1 ) &&
       (tm_core_info.session_info.pd_param.pd_option.fix_rate.time_between_fixes_ms < q_TimerCount ))
            {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TBF %d is very low. Not going to GTP server",
                                               (tm_core_info.session_info.pd_param.pd_option.fix_rate.time_between_fixes_ms));
      return FALSE;
            }

    /*Get current time*/
    time_get_uptime_ms (z_TimeMs);
    t_CurrentTimeMs = QW_CVT_Q2N(z_TimeMs);

    /*Fix from GTP server was fatched recently*/
    if((0 != tm_core_info.tm_core_last_fail_gtp_wwan_fix_time) &&
       (t_CurrentTimeMs > tm_core_info.tm_core_last_fail_gtp_wwan_fix_time) &&
       (tm_core_info.min_wait_interval_btwn_req_to_gtp_server*1000 >
         t_CurrentTimeMs - tm_core_info.tm_core_last_fail_gtp_wwan_fix_time)
      )
       {
      MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "GTP fix fetched recently.Not going to GTP server Last fix time : %d Min wait time : %d",
                                               (tm_core_info.tm_core_last_fail_gtp_wwan_fix_time),
                                               (tm_core_info.min_wait_interval_btwn_req_to_gtp_server));
      return FALSE;
       }
    
    }

  if(os_TimerStart(tm_core_info.tm_core_gtp_wwan_fix_timer,q_TimerCount , 0))
    {
        tm_core_get_gps_state(&get_gps_state_param);
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "gtp wwan fix requested from server q_TimerCount : %d timer-id : %d ", 
                                           q_TimerCount, 
                                           TM_CORE_GTP_WWAN_FIX_TIMER_ID);
    return TRUE;
     }  
  
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "gtp wwan fix timer : %d failed to start. Not going to server",
                                         tm_core_info.tm_core_gtp_wwan_fix_timer);
  return FALSE;
   }

/*===========================================================================

FUNCTION tm_core_req_gtp_wwan_fix_from_cache()

DESCRIPTION
  This function request fix from GTP wwan cache. 

DEPENDENCIES

RETURN VALUE boolean: TRUE: if gtp fix is requested; 
                      FALSE:if gtp fix is not requested. 

SIDE EFFECTS

===========================================================================*/

boolean tm_core_req_gtp_wwan_fix_from_cache()
      {
  tm_core_get_gps_state_param_s_type get_gps_state_param;
  pdsm_lite_cmd_s_type s_cmd ={0};
      
  get_gps_state_param.source = SM_GPS_STATE_INFO_REQ_TYPE_GET_GTP_WWAN_FIX;
  s_cmd.cmd.pd.client_id = tm_core_info.session_info.pd_param.client_id;
  get_gps_state_param.cmd.p_cmd_type = &s_cmd;

  tm_core_get_gps_state(&get_gps_state_param);
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "gtp wwan fix requested from cache");
  return TRUE;
}
/*===========================================================================

FUNCTION tm_core_handover_to_xtra()

DESCRIPTION
      This function is called by TM to request XTRA module about
      the availability of XTRA data. It marks this request made in global first,
      to keep the context . XTRA module can decide whether to start 
      GNSS session or not based on this global.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void tm_core_handover_to_xtra(boolean v_is_tunc_valid)
{
  if(TRUE == tm_core_session_active())
  {
   tm_core_info.session_info.v_xtra_info_requested = TRUE;
   tm_xtra_process_avail_request(v_is_tunc_valid);
}
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Dropping XTRA avail req. as no active TM session");
  }
}

/*===========================================================================

FUNCTION tm_core_handle_gtp_wwan_fix_timer_expire()

DESCRIPTION
  This function handles expired timer with id 
  TM_CORE_GTP_WWAN_FIX_TIMER_ID . It calls  
  tm_core_req_gts_for_sys_unc and proceeds with GNSS session check.

DEPENDENCIES

RETURN VALUE  

SIDE EFFECTS

===========================================================================*/

void tm_core_handle_gtp_wwan_fix_timer_expire(void)
{
   boolean v_is_tunc_valid = TRUE;

   MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TM_CORE_GTP_WWAN_FIX_TIMER_ID expired");

   /* if sync unc doesnt succeed, a retry will happen after sync unc retry timer expired*/
   if(TM_CORE_SYSTEM_UNC_VALID == tm_core_req_gts_for_sys_unc(TRUE, &v_is_tunc_valid))
   {
     /*This is a function to tell XTRA to start a gnss session. This is not the exact 
   function to be called. The api to start GNSS session through XTRA will be provided
    by Sudeep.*/

     /*If device is in US, then update the config and secondary constelation to default. 
     This should be done before XTRA download starts, so that XTRA can download the relevant data
     Clear the flag once the constellation and config are updated*/
     
     tm_core_update_constellation_info_if_in_US();

     tm_core_handover_to_xtra(v_is_tunc_valid);
   }
}



/*===========================================================================

FUNCTION tm_core_handle_gts_sys_unc_wait_timer_expire()

DESCRIPTION
  This function handles expired timer with id 
  TM_CORE_GTS_SYS_UNC_WAIT_TIMER_ID . If second attempt for sys unc fails,
  send DONE event to LocMW.

DEPENDENCIES

RETURN VALUE  

SIDE EFFECTS

===========================================================================*/

void tm_core_handle_gts_sys_unc_wait_timer_expire(void)
{
   tm_pdapi_pd_event_s_type p_pd_info_type;
   boolean v_is_tunc_valid = TRUE;
   
   MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TM_CORE_WWAN_ME_SYS_UNC_WAIT_TIMER expired request GTS again ",0,0,0);
   if(TM_CORE_SYSTEM_UNC_INVALID == tm_core_req_gts_for_sys_unc(FALSE, &v_is_tunc_valid))
   {
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "SYSTEM UNC is invalid report no fix ");
     /*Send no fix to loc_mw*/    
     p_pd_info_type.pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
     p_pd_info_type.e_pd_event = PDSM_LITE_PD_EVENT_END;
     p_pd_info_type.pd_event_data.pd_info_type.pd_info.end_status = PDSM_LITE_PD_END_GNSS_SRVC_NOT_AVAILABLE;
     tm_pdapi_pd_event_callback(&p_pd_info_type, TM_PDAPI_SESS_TYPE_NONE);    
     sm_report_event_gps_fix_end(PDSM_LITE_PD_END_SESS_NONE);  

     tm_core_handle_non_gnss_session_complete(&p_pd_info_type.pd_event_data.pd_info_type, TM_PDAPI_SESS_TYPE_MI);
     
   }
   else
   {
     /*If device is in US, then update the config and secondary constelation to default. 
     This should be done before XTRA download starts, so that XTRA can download the relevant data
     Clear the flag once the constellation and config are updated*/
     
     tm_core_update_constellation_info_if_in_US();
     
     tm_core_handover_to_xtra(v_is_tunc_valid);
   }
}


/*===========================================================================

FUNCTION tm_core_req_gts_for_sys_unc()

DESCRIPTION
  This function request GTP wwan to get system uncertainity.  

DEPENDENCIES

RETURN VALUE System Uncertainity

SIDE EFFECTS

===========================================================================*/
tm_core_system_unc_state tm_core_req_gts_for_sys_unc(boolean is_first_attempt, boolean* v_is_tunc_valid)
{
  gts_PreCheckInfoType z_sysUncInfo = {0};

  /*If bit 0 of NV74194 is set to 1 then TUNC and FUNC checks are bypassed
  */
  if(0 != ((tm_core_info.config_info.bypass_gnss_aiding_check)&(TM_CORE_GTS_SYS_TUNC_AND_FUNC_BYPASS)))
   {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "NV set to bypass sys unc. check");
     return TM_CORE_SYSTEM_UNC_VALID;
   }

  /*Ask GTS for system uncertainity*/
  gts_GetPreCheckSystemUnc(&z_sysUncInfo);

  *v_is_tunc_valid = z_sysUncInfo.b_TuncValid;
 
  if(z_sysUncInfo.b_FuncValid && z_sysUncInfo.b_TuncValid)
  { 
        /*This check regarding cached fix has been added because initialy
         GTS has received one GNSS cached fix it will not be able to provide
         valid TUNC. Once a GNSS fix has been received it will stored in cache
         of LM module too. Hence if there is no fix present in cache of LM module
         the check on TUNC and FUNC can be bypassed.
     */

    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "System Uncertainty is valid");
    return TM_CORE_SYSTEM_UNC_VALID;
    }

  /* If this is the first attempt to get uncertainty and it failed, we start a 3 sec timer
     and will be retrying later on timer expiry.
    */
  if(is_first_attempt)
    {
    os_TimerStart(tm_core_info.tm_core_gts_sys_unc_wait_timer,TM_CORE_GTS_SYS_UNC_WAIT_TIMER_VAL , NULL);
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "unc_wait_timer started with 3 second: TUNC valid %d, FUNC valid %d", z_sysUncInfo.b_TuncValid, z_sysUncInfo.b_FuncValid);
    return TM_CORE_SYSTEM_UNC_WAIT;
    }

  /*After retry,despite TUNC invalid pass it over to XTRA for further processing*/
  /*If bit 1 of NV74194 is set to 1 then only FUNC check is bypassed
  */
  if((z_sysUncInfo.b_FuncValid) ||
  	 (0 != ((tm_core_info.config_info.bypass_gnss_aiding_check)&(TM_CORE_GTS_SYS_FUNC_BYPASS))))
    {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "After retry: TUNC valid %d, FUNC valid %d", z_sysUncInfo.b_TuncValid, z_sysUncInfo.b_FuncValid);
    return TM_CORE_SYSTEM_UNC_VALID;
    }

  /* There will be atmost one retry and For all further attempts, if FUNC is invalid we return INVALID unc */  
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "System uncertainity is not valid");
  return TM_CORE_SYSTEM_UNC_INVALID;
    
   }

/*===========================================================================

FUNCTION tm_core_handle_updated_mcc()

DESCRIPTION
  This function is called when mcc of serving cell changes.It checks
  if the gnss constellation corresponding to new MCC is different 
  from the gnss constellation being currently used. If there is a 
  change in constellation it saves the new constellation and informs 
  XTRA about chnage in constellation.

DEPENDENCIES

RETURN VALUE 

SIDE EFFECTS

===========================================================================*/


void tm_core_handle_updated_mcc(uint32 mcc)
{
  tm_util_update_constellation_on_mcc_change(mcc);   
  }
   
/*===========================================================================

FUNCTION tm_core_get_curr_sec_gnss_constellation()

DESCRIPTION
  This function provides the gnss constellation currently being used, along 
  with GPS, for navigation.

DEPENDENCIES

RETURN VALUE  gnss constellation ID

SIDE EFFECTS

===========================================================================*/

uint8 tm_core_get_curr_sec_gnss_constellation(void)
{
  return tm_core_info.u_sm_curr_serving_sec_gnss_constellation;
}

/*===========================================================================

FUNCTION tm_core_get_curr_gnss_constellation_configS()

DESCRIPTION
  This function provides the gnss constellation currently being used, along 
  with GPS, for navigation.

DEPENDENCIES

RETURN VALUE  gnss constellation ID

SIDE EFFECTS

===========================================================================*/

uint32 tm_core_get_curr_gnss_constellation_config(void)
{
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "tm_core_info.q_mgp_curr_gnss_constellation_config = %d", tm_core_info.q_mgp_curr_gnss_constellation_config, 0, 0);  
  return tm_core_info.q_mgp_curr_gnss_constellation_config;
}


/*===========================================================================

FUNCTION tm_core_start_gnss_session()

DESCRIPTION
  This function starts the GNSS session 

DEPENDENCIES

RETURN VALUE  

SIDE EFFECTS

===========================================================================*/


boolean tm_core_start_gnss_session(void)
{

     int                               prtl_tbl_idx = -1;
     tm_prtl_type                      active_prtl;
     prtl_start_sess_req_result_e_type reqResult;
     tm_sess_req_start_param_s_type    start_actions;
     boolean                           retVal = FALSE;
     tm_sess_handle_type               mo_sess_handle;
     tm_cm_phone_state_info_s_type     phone_state_info;
     tm_pdapi_pd_event_s_type          p_pd_info_type;
   
   if (tm_core_info.session_info.op_mode == TM_SESS_OPERATION_MODE_STANDALONE)
   {
    /*
       For MSB fix request, we need to query protocol sub-modules to see if they want standalone module to
       handle the MSB fix request
    */
    active_prtl = TM_PRTL_TYPE_STANDALONE;
    prtl_tbl_idx = tm_core_get_prtl_idx_by_type(active_prtl);

  }
  else
  {
    
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_core_start_gnss_session requested operation mode[%d] not supported ", tm_core_info.session_info.op_mode, 0, 0);
    return retVal ;

  }
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core dispatches get_pos req to [%d]", active_prtl, 0, 0);

  if ((prtl_tbl_idx >= 0) && (prtl_tbl_idx < TM_PRTL_NUM))
  {  
    mo_sess_handle = tm_core_new_mo_sess_handle();

    if(tm_core_info.prtl_func_cb_table[prtl_tbl_idx].start_sess_req_fp)
    { 
      reqResult = tm_core_info.prtl_func_cb_table[prtl_tbl_idx].start_sess_req_fp(
       mo_sess_handle,
       &(tm_core_info.session_info.pd_param),
       &start_actions);


      if (reqResult == PRTL_START_SESS_REQ_ALLOWED)
      {
        /* If prototocol return PRTL_START_SESS_REQ_ALLOWED, call tmCore_SessReqStart */
        retVal = tm_core_sess_req_start(active_prtl,
                                        mo_sess_handle,
                                        start_actions);

        

        /* Call SDP to send #fixes and tbf information */
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SDP_TEMP_DEBUG: SDP send fix rate info. NumFixes %d, TBF %d", 
          tm_core_info.session_info.pd_param.pd_option.fix_rate.num_fixes, 
            tm_core_info.session_info.pd_param.pd_option.fix_rate.time_between_fixes_ms, 0);

        if(tm_core_info.internal_session)
        {
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Internal Session",0,0,0);
        }        
      }
      else
      {
        /* If protocol return PRTL_START_SESS_REQ_NOT_ALLOWED, report to PD API */
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Protocol sub-module rejects start session request", 0, 0, 0);

        p_pd_info_type.pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
        p_pd_info_type.e_pd_event = PDSM_LITE_PD_EVENT_END;
        p_pd_info_type.pd_event_data.pd_info_type.pd_info.end_status = PDSM_LITE_PD_END_FIX_ERROR;
        tm_pdapi_pd_event_callback(&p_pd_info_type, TM_PDAPI_SESS_TYPE_MI);

        /*Send PDSM_LITE_PD_EVENT_DONE to loc_mw to Schedule next fix*/
        tm_core_handle_non_gnss_session_complete(&p_pd_info_type.pd_event_data.pd_info_type, TM_PDAPI_SESS_TYPE_MI);

        retVal = FALSE;
      }
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid prtlTableIndex", 0, 0, 0);
      retVal = FALSE;
    }
  }
  else
  {

    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid prtlTableIndex", 0, 0, 0);
    retVal = FALSE;
  }

  if (retVal == FALSE)
  {
    tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506  !e730 */
  }
  return retVal;
}

/*===========================================================================

FUNCTION tm_core_handle_gps_state_info_req_type_get_gtp_wwan_fix()

DESCRIPTION
  This function handles the fix report received from GTP-WWAN. It 
  translates the report into QMI_LOC_POSITION_REPORT_IND and 
  passes it to PDAPI.

DEPENDENCIES

RETURN VALUE  

SIDE EFFECTS

===========================================================================*/

void tm_core_handle_gps_state_info_req_type_get_gtp_wwan_fix(sm_FixInfoStructType * p_fixReport)
{
  boolean iscachedFix = TRUE;
  boolean b_IsPosValid = FALSE;
  boolean b_IsHepeSatisfied = FALSE;
  boolean v_is_tunc_valid = TRUE;
  tm_pdapi_pd_event_s_type p_pd_info_type;
  FLT f_hepe = 0;
  pdsm_lite_pd_info_s_type  pd_info = {0};
  qmiLocEventPositionReportIndMsgT_v02 * pz_PositionReportIndMsg = NULL;
  pdsm_lite_pd_event_type   pd_event;
  boolean                   gtp_wwan_fix_failed = FALSE;
  sm_gm_pos_fix_info        fix_info;
  time_type               z_TimeMs={0};
  tm_core_system_unc_state sys_unc;

  if(os_TimerCheck(tm_core_info.tm_core_gtp_wwan_fix_timer) > 0)
  {
    iscachedFix = FALSE;
  }
  if(TRUE == iscachedFix)
  {
    if((NULL != p_fixReport) && (0 != p_fixReport->z_NavPos.z_PosFlags.b_IsValid))
    {
      b_IsPosValid = TRUE;
      if(tm_core_is_hepe_satisfied( p_fixReport->z_NavPos.f_ErrorEllipse, &f_hepe))
      {
        /*As the HEPE is satisfied before sending out the final fix diag needs 
         to be informed that the session was sucessfully started. We send this
         event to diag only when HEPE is satisfied because if HEPE is not satisfied
         and we eventually have to fall to GNSS sesion the legacy code will again
         send out this event to diag. To avoid multiple transmission of same event
         we have to make sure that GNSS session will not be started.
        */
        b_IsHepeSatisfied = TRUE;
        sm_report_event_gps_fix_start( tm_core_event_counter++, MS_BASED_OP );
        pd_info.client_id = tm_core_info.session_info.pd_param.client_id;
        fix_info.fix_type = GEOFENCE_POS_FIX_TYPE_FINAL; 
        fix_info.y_pos_fix_ext_info.z_gnss_fix_ext_info.b_is_nw_asst_fix = FALSE;
        fix_info.fix_src = GEOFENCE_POS_FIX_SRC_GNSS; /*#9x05 needs to be checked what should be stated 
                                  as source of position report*/

        if(TRUE == p_fixReport->z_NavPos.z_SmPosFlags.b_IsCellId)
        {
          // its a GTP cell based fix 
          if(FALSE == p_fixReport->z_NavPos.z_SmPosFlags.b_IsWwanRanging)
          {
            // Its a GTP 1.0 fix
            fix_info.fix_src = GEOFENCE_POS_FIX_SRC_GTP_1_0;
          }
          else
          {
            // Its a GTP 2.0 fix
            fix_info.fix_src = GEOFENCE_POS_FIX_SRC_GTP_2_0; 
          }
        }

        fix_info.pos_fix = *p_fixReport;
        /*Send the LR to Geofence module*/
        (void)gm_position_update(&fix_info); 

        pz_PositionReportIndMsg = &pd_info.pd_info.pz_PositionReportIndMsg;
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "GTP Cache satisfies HEPE translate to QMI", 0, 0, 0);
        /*Translate the propogated fix to QMI*/
        pd_event = tm_util_xlate_fixt_report_to_qmi_loc_pos_rpt_Ind(p_fixReport, pz_PositionReportIndMsg , 
                                                        PRTL_SESS_INFO_LR);
        /*Passing the fix to diag*/
        tm_pdapi_pos_log_generation(
                           pd_event,
                           tm_core_info.session_info.pd_param.client_id,
                           p_fixReport,
                           p_fixReport->z_NavPos.z_PosFlags.b_IsSft,
                           p_fixReport->z_NavPos.z_PosFlags.b_IsKf
                          );
        tm_pdapi_pos_event_callback(pd_event,&pd_info, TM_PDAPI_SESS_TYPE_NONE);

        /*Send PDSM_LITE_PD_EVENT_DONE to loc_mw to Schedule next fix*/
        tm_core_handle_non_gnss_session_complete(&pd_info, TM_PDAPI_SESS_TYPE_NONE);

       }
	   	  /* If hepe is > 75km then force TLE to download position from server */
	   if(f_hepe > PUNC_THRESHOLD_FOR_XTRA_T)
	   {
	     b_IsPosValid = FALSE;
	   }
    }
    
    if( !b_IsPosValid || !b_IsHepeSatisfied )
    {
      if(LM_DMM_STATE_LOADED == lm_get_dmm_state())
      {
        /*Check for XTRA validity. If valid, start a GNSS session otherwise downloads XTRA first and then triggers a session*/
        sys_unc = tm_core_req_gts_for_sys_unc(TRUE, &v_is_tunc_valid);
        if(TM_CORE_SYSTEM_UNC_VALID == sys_unc)
        {
          /*If device is in US, then update the config and secondary constelation to default. 
          This should be done before XTRA download starts, so that XTRA can download the relevant data
          Clear the flag once the constellation and config are updated*/
          
          tm_core_update_constellation_info_if_in_US();
          
          tm_core_handover_to_xtra(v_is_tunc_valid);
        }
      }
      else if(!b_IsPosValid)
      {
        /*TLE cache is invalid,Force TLE to download position from server, 
        It will be injected to MGP as coarse position
        */
        tm_core_req_gtp_wwan_fix_from_server(TRUE);
      }
      else
      {
      /*reattempt GTP fix from server, If all the criteria meets will start GNSS session and will return TRUE*/
        if(tm_core_req_gtp_wwan_fix_from_server(FALSE) == FALSE)
        {
          MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "GTP Server fix failed");
          sys_unc = tm_core_req_gts_for_sys_unc(TRUE, &v_is_tunc_valid);
          if(TM_CORE_SYSTEM_UNC_VALID == sys_unc)
          {
            /*If device is in US, then update the config and secondary constelation to default. 
            This should be done before XTRA download starts, so that XTRA can download the relevant data
            Clear the flag once the constellation and config are updated*/
            
            tm_core_update_constellation_info_if_in_US();
            
            tm_core_handover_to_xtra(v_is_tunc_valid);
          }
          else if(TM_CORE_SYSTEM_UNC_INVALID == sys_unc)
          {
            MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "System uncs invalid, Sending no fix");
            /*Send no fix to loc_mw*/
            p_pd_info_type.pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
            p_pd_info_type.pd_event_data.pd_info_type.pd_info.end_status = PDSM_LITE_PD_END_FIX_ERROR;

            p_pd_info_type.e_pd_event = PDSM_LITE_PD_EVENT_END;
            tm_pdapi_pd_event_callback( &p_pd_info_type,TM_PDAPI_SESS_TYPE_NONE );
            /*Send PDSM_LITE_PD_EVENT_DONE to loc_mw to Schedule next fix*/
            tm_core_handle_non_gnss_session_complete(&p_pd_info_type.pd_event_data.pd_info_type, TM_PDAPI_SESS_TYPE_MI);
           }
         }
       }
     }
  }
   else
  {
      /*Since fix from Server is received we should stop the timer even 
      if the fix is invalid*/
      os_TimerStop(tm_core_info.tm_core_gtp_wwan_fix_timer);
      if((NULL == p_fixReport)|| (p_fixReport->z_NavPos.z_PosFlags.b_IsValid == 0))
      {
        gtp_wwan_fix_failed = TRUE;
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "GTP Server fix NULL fix reported, gtp_wwan_fix_failed = %d",gtp_wwan_fix_failed, 0, 0);
      }
      else
      {
        if(tm_core_is_hepe_satisfied( p_fixReport->z_NavPos.f_ErrorEllipse, &f_hepe))
        {
          /*As the HEPE is satisfied before sending out the final fix diag needs 
          to be informed that the session was sucessfully started. We send this
          event to diag only when HEPE is satisfied because if HEPE is not satisfied
          and we eventually have to fall to GNSS sesion the legacy code will again
          send out this event to diag. To avoid multiple transmission of same event
          we have to make sure that GNSS session will not be started.
            */
          sm_report_event_gps_fix_start( tm_core_event_counter++, MS_BASED_OP );
          pd_info.client_id = tm_core_info.session_info.pd_param.client_id;

          fix_info.fix_type = GEOFENCE_POS_FIX_TYPE_FINAL; 
          fix_info.y_pos_fix_ext_info.z_gnss_fix_ext_info.b_is_nw_asst_fix = FALSE;
          fix_info.fix_src = GEOFENCE_POS_FIX_SRC_GNSS; /*#9x05 needs to be checked what should be stated 
                             as source of position report*/
          fix_info.pos_fix = *p_fixReport;
          if(TRUE == p_fixReport->z_NavPos.z_SmPosFlags.b_IsCellId)
          {
            // its a GTP cell based fix 
            if(FALSE == p_fixReport->z_NavPos.z_SmPosFlags.b_IsWwanRanging)
            {
              // Its a GTP 1.0 fix
              fix_info.fix_src = GEOFENCE_POS_FIX_SRC_GTP_1_0;
            }
            else
            {
              // Its a GTP 2.0 fix
              fix_info.fix_src = GEOFENCE_POS_FIX_SRC_GTP_2_0; 
            }
          }

          /*Send the LR to Geofence module*/
          (void)gm_position_update(&fix_info); 

          pz_PositionReportIndMsg = &pd_info.pd_info.pz_PositionReportIndMsg;
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "GTP server fix satisfies HEPE translate to QMI", 0, 0, 0);
          /*Translate the propogated fix to QMI*/
          pd_event = tm_util_xlate_fixt_report_to_qmi_loc_pos_rpt_Ind(p_fixReport, pz_PositionReportIndMsg , 
                                                              PRTL_SESS_INFO_LR);
          /*Passing the fix to diag*/
          tm_pdapi_pos_log_generation(
                                 pd_event,
                                 tm_core_info.session_info.pd_param.client_id,
                                 p_fixReport,
                                 p_fixReport->z_NavPos.z_PosFlags.b_IsSft,
                                 p_fixReport->z_NavPos.z_PosFlags.b_IsKf
                                );
          tm_pdapi_pos_event_callback(pd_event,&pd_info, TM_PDAPI_SESS_TYPE_NONE);

          /*Send PDSM_LITE_PD_EVENT_DONE to loc_mw to Schedule next fix*/
          tm_core_handle_non_gnss_session_complete(&pd_info, TM_PDAPI_SESS_TYPE_NONE);

          gtp_wwan_fix_failed = FALSE;
        }
        else
        {
          gtp_wwan_fix_failed = TRUE;
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "GTP Server fix -> HEPE not satisfied , gtp_wwan_fix_failed =%d",gtp_wwan_fix_failed , 0, 0);
        }
      }
      if(TRUE == gtp_wwan_fix_failed)
      {
        time_get_uptime_ms (z_TimeMs);
        tm_core_info.tm_core_last_fail_gtp_wwan_fix_time = QW_CVT_Q2N(z_TimeMs);
          
        if(TM_CORE_SYSTEM_UNC_VALID == tm_core_req_gts_for_sys_unc(TRUE, &v_is_tunc_valid))
        {
          /*This is a function to tell XTRA to start a gnss session. This is not the exact 
           function to be called. The api to start GNSS session through XTRA will be provided
           by Sudeep.*/
         
          /*If device is in US, then update the config and secondary constelation to default. 
          This should be done before XTRA download starts, so that XTRA can download the relevant data
          Clear the flag once the constellation and config are updated*/

          tm_core_update_constellation_info_if_in_US();
            
          tm_core_handover_to_xtra(v_is_tunc_valid);
        }
      }
  }
   
}

void tm_core_xtra_efs_gps_fs_task_done(sm_GpsFsTaskNotificationDataType *pz_NotificationData)
{
  char u_string[256];

  memset(u_string, 0, sizeof(u_string));

  if (NULL == pz_NotificationData)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL GPS FS Notification Ptr");
    return;
  }

  //For now just output information.
  (void) snprintf(u_string, sizeof(u_string),"tm_core_xtra_efs_gps_fs_task_done - EFS File: %s, File OP:%u, ErrorCode: %u",
         (char *)(pz_NotificationData->u_Filename),
         pz_NotificationData->u_FileOp,
         (unsigned int)pz_NotificationData->q_EfsStatus);
  MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", u_string);

}

void tm_core_handle_feature_status_update(LocFM_FeatureStatusStructT *pz_featureStatus)
{

  if(NULL != pz_featureStatus)
  {
    if(pz_featureStatus ->q_FeatureID == locFM_getGnssServiceFeatureId())	
    {   
        if(TRUE == pz_featureStatus->v_Status)
        {
          tm_core_info.config_info.subs_cfgs.gps_lock = PDSM_LITE_GPS_LOCK_NONE ;
        }
        else
        {
          tm_core_info.config_info.subs_cfgs.gps_lock = PDSM_LITE_GPS_LOCK_ALL ;
        }

    }
	else if (pz_featureStatus ->q_FeatureID == locFM_getGtpWwanServiceFeatureId())
    {
        tm_core_info.config_info.b_isGtpWwanServiceFeatureEnabled = pz_featureStatus->v_Status;
    }
	else
	{
	  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Invalid feature id =%d",pz_featureStatus ->q_FeatureID, 0, 0);	
	}
  }
}

/*===========================================================================
FUNCTION tm_nmea_pdapi_send

DESCRIPTION
  tm_nmea_pdapi_send() puts the constructed NMEA sends the NMEA strings on
  PDAPI interface.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS
 
===========================================================================*/
void tm_nmea_pdapi_send(char *p_str, pdsm_pa_nmea_type e_nmea_type)
{

  tm_pdapi_pd_event_s_type *pz_pd_event_info = NULL;
  
  if (NULL == p_str)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL NMEA string received for PDAPI send");
    return;
  }
  pz_pd_event_info = (tm_pdapi_pd_event_s_type *)os_MemAlloc(sizeof(tm_pdapi_pd_event_s_type), OS_MEM_SCOPE_TASK);
  
  if (NULL == pz_pd_event_info)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cannot allocate memory for pdsm_ext_status_info_s_type in tm_nmea_pdapi_send");
    return;
  }
  memset(pz_pd_event_info, 0, sizeof(*pz_pd_event_info));
  
  pz_pd_event_info->pd_event_data.pd_info_type.client_id = tm_core_info.session_info.pd_param.client_id;
  pz_pd_event_info->pd_event_data.pd_info_type.pd_info.nmea_pos_report_type.nmea_type = e_nmea_type;
  pz_pd_event_info->pd_event_data.pd_info_type.pd_info.nmea_pos_report_type.nmea_length = strlen(p_str);
  pz_pd_event_info->e_pd_event = PDSM_LITE_PD_STATUS_NMEA_POS_REPORT;

  (void)GNSS_STRLCPY((char *)(pz_pd_event_info->pd_event_data.pd_info_type.pd_info.nmea_pos_report_type.nmea_data),
                     p_str, PDSM_LITE_NMEA_DATA_LENGTH);

  tm_pdapi_pd_event_callback(pz_pd_event_info,TM_PDAPI_SESS_TYPE_NONE);
  (void)os_MemFree((void **)&pz_pd_event_info);
}


/*===========================================================================
FUNCTION tm_nmea_debug_report

DESCRIPTION
  This function sends the debug NMEA sentences to LocMW directly via TM and also
  on the NMEA COM port if it is open. This function is not to be confused with
  tm_nmea_debug.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_nmea_debug_report(sm_DebugNMEA *pDebugNMEA)
{
  if (NULL == pDebugNMEA)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL NMEA string");
  }

  /*Send NMEA string over to PDAPI*/
  tm_nmea_pdapi_send((char *)&(pDebugNMEA->u_Buffer[0]), PDAPI_LITE_NMEA_DEBUG);
}

/*===========================================================================

FUNCTION tm_core_nmea_debug_info

DESCRIPTION
  This function processes NMEA debug string in TM And forwards
  it to HLOS for debugging purpose. Should not be mixed with NMEA strings
  generated with Measurement reports.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_nmea_debug_info(sm_DebugNMEA *pz_DebugNMEACustom)
{
  if (NULL == pz_DebugNMEACustom)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL Nmea string");
    return;
  }

  if (TM_CORE_CONFIG_DD(nmea_output_format) & PDAPI_LITE_NMEA_DEBUG)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "tm_core_nmea_debug_info: Debug NMEA string");
    tm_nmea_debug_report(pz_DebugNMEACustom);
  }
}


/*===========================================================================

FUNCTION tm_core_is_rcvr_config_update_needed

DESCRIPTION
  This function returns if current config update is required

  RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_rcvr_config_update_needed(boolean v_RcvrInUS)
{
  if (!v_RcvrInUS)
  {
   if(tm_core_info.v_is_update_config_needed)
   {
    return TRUE;
   }
  }
  else
  {
    if(C_RCVR_GNSS_CONFIG_BDS_ENABLED_OUTSIDE_OF_US & tm_core_info.q_mgp_curr_gnss_constellation_config)
    {
      if(!tm_core_info.v_is_update_config_needed)
      {
       return TRUE;
      }
    }
    
  }
  return FALSE;
}

/*===========================================================================

FUNCTION tm_core_update_constellation_info_if_in_US

DESCRIPTION
  This function update the constellation config and constellation ID
  when receiver is in US.
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_update_constellation_info_if_in_US(void)
{
  if(tm_core_info.v_is_update_config_needed == TRUE)
   {
     MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Is update needed=%d mgp const=%d",tm_core_info.v_is_update_config_needed, tm_core_info.q_mgp_curr_gnss_constellation_config, 0);	
     tm_core_info.q_mgp_curr_gnss_constellation_config = (C_RCVR_GNSS_CONFIG_GPS_ENABLED|C_RCVR_GNSS_CONFIG_GLO_ENABLED);
     tm_core_info.u_sm_curr_serving_sec_gnss_constellation =TM_GNSS_GLO_CONSTELL;
     tm_core_info.v_is_update_config_needed = FALSE;
   }

}
/*===========================================================================

FUNCTION tm_core_get_eph_validity

DESCRIPTION
  This function checks if ephemeris is still valid or not based on last cached fix
  
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/

boolean tm_core_get_eph_validity(void)
{
  gts_TimeType z_GtsTime = {0};
  gps_TimeStructType z_currentGpsTime = {0};
  boolean v_retVal;

  /* Assigning invalid week number*/
  z_currentGpsTime.w_GpsWeek = TM_CORE_INVALID_WEEK_NUMBER;
  /* Eph validity now checking by PE. When a session ends, PE sends Eph expiry time info and that is stored in tm core.
     And now, need to call an PE provided API with stored Eph expiry time info along with current GPS time to check the validity. 
     And TUNC also needs to be less than 100ms then we can request MARB with 6 sec */
     
  if (z_gnss_position_cache.cached_gnss_pos_valid)
  {                       
    /* Current GPS time from GTS*/
    if ((TRUE == gts_GetTime(&z_GtsTime)) && (TRUE == z_GtsTime.b_Valid))
    {
       z_currentGpsTime.e_SourceOfTime = z_GtsTime.e_TimeSrc;
	   z_currentGpsTime.f_ClkTimeBias = z_GtsTime.f_ClkTimeBias;
	   z_currentGpsTime.f_ClkTimeUncMs = z_GtsTime.f_ClkTimeUncMs;
	   z_currentGpsTime.q_GpsMsec = z_GtsTime.q_GpsMsec;
	   z_currentGpsTime.w_GpsWeek = z_GtsTime.w_GpsWeek;
    }
	
    if ((TM_CORE_GTS_TUNC_VALIDITY_LIMIT > z_GtsTime.f_ClkTimeUncMs) && 
		(gnss_GetEphAvailStatusToSetSessionTime(&tm_core_info.z_eph_exp_time,&z_currentGpsTime)))
    {
       v_retVal = TRUE;
    }
    else
    {
       v_retVal = FALSE;
    }
  }
  else
  {
    v_retVal = FALSE;
  }  
  return v_retVal;
}

/*=============================================================================

FUNCTION 
  tm_core_get_sys_status_info

DESCRIPTION
  This function provides the expected session duration in enum format based on system uncertainties

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  uint32 : System status validity

SIDE EFFECTS
  None.

=============================================================================*/
uint32 tm_core_get_sys_status_info()
{ 
  uint32 e_sys_status = C_SYS_STATUS_VALID_NONE;
  gts_PreCheckInfoType PreCheckData = {0};
  boolean b_xtra_available = tm_xtra_is_valid_xtra_available();
  boolean b_eph_validity = tm_core_get_eph_validity();
  gts_GetPreCheckSystemUnc(&PreCheckData); /* fetch FUNC  and TUNC */

  if(b_eph_validity)
  {
    e_sys_status  = C_SYS_STATUS_EPH_VALID; 
  }
  else if(PreCheckData.b_TuncValid && PreCheckData.b_FuncValid && b_xtra_available)
  {
    /* We should be here *always*..use default read from nv*/
    e_sys_status  = C_SYS_STATUS_VALID_ALL;
  }
  else
  {
    if(b_xtra_available)
    {
      e_sys_status  = C_SYS_STATUS_VALID_ONLY_XTRA;  // device is OOO/LPM but old XTRA is present.
    }
  }
  return(e_sys_status );
}
/*=============================================================================

FUNCTION 
  tm_core_handle_non_gnss_session_complete

DESCRIPTION
  This is a common fucntion for all the non_GNSS session completion
  1. Will reset any session related information in tm_core_info
  2. Send PDSM_LITE_PD_EVENT_DONE to LocMW to schedule next session

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

void tm_core_handle_non_gnss_session_complete(pdsm_lite_pd_info_s_type *pd_info, tm_pdapi_sess_type sess_type)
{
    tm_pdapi_pd_event_s_type p_pd_info_type;
      
    /*Send PDSM_LITE_PD_EVENT_DONE to loc_mw to Schedule next fix*/
    p_pd_info_type.e_pd_event = PDSM_LITE_PD_EVENT_DONE;
    p_pd_info_type.pd_event_data.pd_info_type = *pd_info;
    tm_pdapi_pd_event_callback(&p_pd_info_type, sess_type);      
    sm_report_event_gps_fix_end(PDSM_LITE_PD_END_SESS_NONE);

    os_TimerStop(tm_core_info.tm_core_gtp_wwan_fix_timer);
    os_TimerStop(tm_core_info.tm_core_gts_sys_unc_wait_timer);
    tm_core_info.session_info.v_xtra_info_requested    = FALSE;

    tm_core_info.internal_session = FALSE;
    tm_core_update_session_state(TM_CORE_SESS_STATE_INIT);
}

/*===========================================================================

FUNCTION tm_core_handle_eph_exp_time

DESCRIPTION
  This function stores the eph expiry time info from PE in tm core.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_handle_eph_exp_time(gnss_EphExpTimeStructType *pz_eph_exp_time)
{
  if (NULL == pz_eph_exp_time)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL Eph exp time info");
    return;
  }
  tm_core_info.z_eph_exp_time = *pz_eph_exp_time;
}

/*===========================================================================

FUNCTION tm_core_is_gnss_control_fix_operation_allowed

DESCRIPTION
  This function checks if specified GNSS fix control operation is allowed or not  .

RETURN VALUE
  TRUE : If specified bit is set in the NV
  FALSE: If specified bit is not set in the NV

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
bool tm_core_is_gnss_control_fix_operation_allowed(uint8 u_GnssControlFixOperationMask)
{
  switch(u_GnssControlFixOperationMask)
  {
    case GNSS_CONTROL_FIX_OPERATION_SM_CACHE:
    /*This case is for bit 0 of NV *GNSS_NV_EFS_SM_GNSS_CONTROL_FIX_OPERATION for controlling cache fix propagation and reporting*/
     return !(tm_core_info.config_info.gnss_control_fix_operation & u_GnssControlFixOperationMask);
    break;
    default:
     return FALSE;
   }
}

/*===========================================================================
FUNCTION tm_core_notify_gnss_availability

DESCRIPTION
  This function call gm apis to update MARB notifications and failure indications.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_notify_gnss_availability(boolean u_GnssAvail)
{
  sm_gm_mgp_load_avail_type z_mgp_load_avail = {0};
  if ((FALSE == u_GnssAvail) && (TRUE == tm_core_is_internal_session_on()))
  {
    gm_mgp_update_load_availability(&z_mgp_load_avail);
  }
  else if(TRUE == u_GnssAvail)
  {
    z_mgp_load_avail.loadAvailable = TRUE;
    gm_mgp_update_load_availability(&z_mgp_load_avail);
  }
  return;
}

/*===========================================================================
FUNCTION tm_core_is_qzss_enabled()

DESCRIPTION
  This function returns True if QZSS is enabled in config mask

DEPENDENCIES

RETURN VALUE  
  True if QZSS enabled else False

SIDE EFFECTS


===========================================================================*/

boolean tm_core_is_qzss_enabled(void)
{
  if((tm_core_info.q_mgp_curr_gnss_constellation_config)&
  	 (C_RCVR_GNSS_CONFIG_QZSS_ENABLED_WORLDWIDE|C_RCVR_GNSS_CONFIG_QZSS_ENABLED_OUTSIDE_OF_US))
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION tm_core_is_gtp_2_0_fix_possible()

DESCRIPTION
  This function returns TRUE if GTP 2.0 fix is possible else returns FALSE

DEPENDENCIES

RETURN VALUE  
  True if GTP 2.0 fix is possible else False

SIDE EFFECTS


===========================================================================*/

boolean tm_core_is_gtp_2_0_fix_possible(void)
{
  if (LM_DMM_STATE_UNLOADED == lm_get_dmm_state())
  /*In MGP unloaded case TM returns TRUE because MGP cannnot provide GTP 2.0 position in loaded,unloading and loading cases */
  {
    return TRUE;
  }

  return FALSE;
}

