#ifndef HSU_HEALTH_DIAG_H
#define HSU_HEALTH_DIAG_H

#ifdef FEATURE_HS_USB_HEALTH_MONITOR
/*==========================================================================

                      Diagnostic Packet Definitions

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

  !!! NOTE: All member structures of diag packets must be PACK.
  
  !!! WARNING: Each command code number is part of the externalized
  diagnostic command interface.  This number *MUST* be assigned
  by a member of QCT's tools development team.

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            Edit History

  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/hsu_diag/hsu_health_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/13/10   CY


===========================================================================*/

#include "comdef.h"

#ifndef FEATURE_WINCE
#include "customer.h"
#include "feature.h"
#endif /* FEATURE_WINCE */

#include "diagcmd.h"
#include "diagpkt.h"
#include "log_codes.h"
#include "qw.h"
#include "diag.h"
#include "diagdiag.h"
#include "log.h" 
//#include "rex.h"
#include "hsu_timer.h"

/*==============================================================================
 DIAG_SUBSYS_COREBSP = 75, CoreBSP 

 Command code allocation: 
  [0 - 2047]      - HWENGINES
  [2048 - 2147]   - MPROC
  [2148 - 2247]   - BUSES
  [2248 - 2347]   - HSUSB 
  [2348 - 65535]  - Reserved
==============================================================================*/

#ifdef FEATURE_HS_USB_BAM
#define HSUSB_DIAG_BAM_SM_SEND_EVENT     0x08d2
#endif /* FEATURE_HS_USB_BAM */

#define HSUSB_DIAG_VDD_MIN               0x08d4
#define HSUSB_DIAG_INFO                  0x08d5
#define HSUSB_DIAG_COUNTERS              0x08d6

#ifdef FEATURE_HS_USB_BAM
DIAGPKT_SUBSYS_REQ_DEFINE( DIAG_SUBSYS_COREBSP, HSUSB_DIAG_BAM_SM_SEND_EVENT)
DIAGPKT_SUBSYS_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE( DIAG_SUBSYS_COREBSP, HSUSB_DIAG_BAM_SM_SEND_EVENT) 
DIAGPKT_SUBSYS_RSP_END
#endif /* FEATURE_HS_USB_BAM */

DIAGPKT_SUBSYS_REQ_DEFINE( DIAG_SUBSYS_COREBSP, HSUSB_DIAG_VDD_MIN)
DIAGPKT_SUBSYS_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE( DIAG_SUBSYS_COREBSP, HSUSB_DIAG_VDD_MIN) 
DIAGPKT_SUBSYS_RSP_END

DIAGPKT_SUBSYS_REQ_DEFINE( DIAG_SUBSYS_COREBSP, HSUSB_DIAG_INFO)
DIAGPKT_SUBSYS_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE( DIAG_SUBSYS_COREBSP, HSUSB_DIAG_INFO)
DIAGPKT_SUBSYS_RSP_END

DIAGPKT_SUBSYS_REQ_DEFINE( DIAG_SUBSYS_COREBSP, HSUSB_DIAG_COUNTERS)
DIAGPKT_SUBSYS_REQ_END

typedef PACKED struct
{
  uint32    suspend_count;
  uint32    resume_count;
  uint32    bus_reset_count;
  uint32    remote_wakeup_count;
  uint32    xo_shutdown_count;
  uint32    vdd_min_count;
}hsu_counters_type;

DIAGPKT_SUBSYS_RSP_DEFINE( DIAG_SUBSYS_COREBSP, HSUSB_DIAG_COUNTERS)
  hsu_counters_type counts;
DIAGPKT_SUBSYS_RSP_END



#ifdef FEATURE_HS_USB_THREAD_PROFILE

#define HSU_THREAD_PROFILE_MAX_HIST 16
#define HSU_THREAD_PROFILE_TOTAL_TIME     (HSU_THREAD_PROFILE_MAX_THREAD)
#define HSU_THREAD_PROFILE_AVG   (HSU_THREAD_PROFILE_MAX_HIST)
#define HSU_THREAD_PROFILE_MAX   (HSU_THREAD_PROFILE_MAX_HIST + 1)


#define HSU_THREAD_PROFILE_MAX_DIAG_PRINT     (HSU_THREAD_PROFILE_MAX_HIST/3)
#define HSU_THREAD_PROFILE_REMAIN_DIAG_PRINT  (HSU_THREAD_PROFILE_MAX_HIST%3)
#define HSU_THREAD_PROFILE_RESOLUTION 10000
#define HSU_THREAD_PROFILE_DEFAULT_FREQUENCY 600


typedef struct 
{
  // If the profile timer start already.
  boolean profile_started;

  //global Timestamp at the beginning of profile.
  uint32 start_time;

  //timer cb handler
  hsu_timer_handle_type profile_stop_timer;

  uint32 profile_hist_idx;

  // sample frequency for periodic timer
  uint32 profile_sample_freq;

  // sleep thread idx in the array, use this to know overall CPU usage
  uint32 sleep_thread_idx;

  //if trigger non-stop peridoic profile.
  boolean periodic_flag;
  
  //Last row (HSU_THREAD_PROFILE_TOTAL_TIME) is for Total Time for each sample,
  //second to last column (HSU_THREAD_PROFILE_AVG) is thread_avg,
  //last column (HSU_THREAD_PROFILE_MAX) is MAX
  //Value will store in timetick before profile end, and will convert to 10000% when profile completed.
  uint32 profile_hist[HSU_THREAD_PROFILE_MAX_THREAD+1][HSU_THREAD_PROFILE_MAX_HIST+2];    
} hsu_thread_profile_ctx_type;

#endif

#endif /* FEATURE_HS_USB_HEALTH_MONITOR */
#endif /* HSU_HEALTH_DIAG_H */
