/******************************************************************************
  @file:  loc_api.c
  @brief: Location Middleware task

  DESCRIPTION
  This module defines the loc API routines

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Copyright (c) 2013-2014 QUALCOMM Atheros, Inc.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  -----------------------------------------------------------------------------
 ******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ------------------------------------------------------- 
05/19/16   yh       LocMW refactoring 1
01/11/16   yh       LocMW refactoring 0
06/11/15   sjk      LB 2.0 Integration
09/05/14   ssu      GM LOWI Integration
04/15/14   yh       XTRA version check added   
03/10/14   ah       Added support for Set Premium Services Cfg & Get Avail WWAN Pos
05/02/11   pn       Added handler for OC_IOCTL_ERROR_ESTIMATE_CONFIG.
04/07/10   ns       Make sure that invalid client handle is returned if
                    the loc mw task is not up or if initialization
                    fails during loc_open   

$Id: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_api.c#1 $
$DateTime: 2020/07/21 21:23:44 $
======================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "target.h"

//#include "msg.h"
#include "queue.h"

#include "aries_os_api.h"
#include "pdapi_lite.h"
#include "pdsm_atl.h"
#include "loc_api_2.h"

#include "loc_client.h"
#include "loc_conn.h"
#include "loc_xtra.h"
#include "loc_sm_interface.h"
#include "loc_ni.h"
#include "loc_wifi.h"
#include "loc_api_internal.h"
#include "gnss_api.h"
#include "loc_geofence.h"
#include "loc_batching.h"
#include "loc_aon.h"
#include "loc_utils.h"

// Global data variable

// zero-initialize the global variable, so this flag loc_mw_task_started would be FALSE at start up,
// even before our lock-less RPC server gets to run. This works only if we live in the same process so data initialization is
// always done by compiler generated code before any C code.
// what we really want is {.loc_mw_task_started = FALSE}, but that would require a C99 compiler, which we may or may not have
// on various platforms.
loc_middleware_data_s_type loc_middleware_data = {0};

/*===========================================================================
FUNCTION loc_initialize_modules

DESCRIPTION
  This function initializes the sub-modules of loc-mw based on the events that 
  need to be generated

PARAMETERS
  t_EventRegMask:          Events which the client is interested in receiving.
                           The modules which generate these events will be 
                           initialized.

DEPENDENCIES

RETURN VALUE
  TRUE: If modules are initialized successfully
  FALSE: If modules are not initialized successfully

SIDE EFFECTS
  race condition could happen if more than one LocAPI is calling loc_open.
  loc_middleware_data.crit_sec is mostly used for data protection 
  we need some mutex in safe guarding state machines or procedures, 
  which implies redesigning of LocMW

===========================================================================*/

boolean loc_initialize_modules (const qmiLocEventRegMaskT_v02 t_EventRegMask)
{
   boolean result = FALSE;

   // poor man's exception handling: do...while with break
   do
   {
       LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);

       // Location sub-module will be initialzied when the first location client
       // registers for the corresponding event
       if (t_EventRegMask & QMI_LOC_EVENT_MASK_POSITION_REPORT_V02) 
       /*9x05: Wifi not supported*/
       {
          if (loc_middleware_data.pd_module_initialized == FALSE)
          {
             LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
             if(FALSE == locPd_Init())
             {
                break;
             }
             LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
             loc_middleware_data.pd_module_initialized = TRUE;
          }

          if (loc_middleware_data.pa_module_initialized  == FALSE)
          {     
             LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
             if(FALSE == locPa_Init())
             {
                break;
             }
             LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
             loc_middleware_data.pa_module_initialized = TRUE;
          }
       }
      /*#9x05 Not supported*/


       if (t_EventRegMask & QMI_LOC_EVENT_MASK_INJECT_POSITION_REQ_V02)
       {
           if (loc_middleware_data.xtra_module_initialized == FALSE)
           {
              LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
              if(FALSE == locXtra_Init())
              {
                break;
              }
              LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
              loc_middleware_data.xtra_module_initialized = TRUE;
           }
       }

        /*
          #9x05: LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST 
                 LOC_EVENT_WPS_NEEDED_REQUEST not supported
        */

       if ( FALSE == loc_middleware_data.pa_module_initialized )
       {
          LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
          if(FALSE == locPa_Init())
          {
            break;
          }
          LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
          loc_middleware_data.pa_module_initialized = TRUE;
       }

       if (loc_middleware_data.geofence_module_initialized == FALSE)
       {
          LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
          if(FALSE == locGeofenceInit())
          {
             break;
          }
          LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
          loc_middleware_data.geofence_module_initialized = TRUE;
       }

       if (loc_middleware_data.aon_module_initialized == FALSE)
       {
          LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);
          if(FALSE == locAonInit())
          {
             break;
          }
          LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.crit_sec);
          loc_middleware_data.aon_module_initialized = TRUE;
       }
       LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.crit_sec);

       // finally, we're done with initialization
       result = TRUE;
   }while(0);

   return result;
}



/*===========================================================================
FUNCTION loc_close

DESCRIPTION
  This function discconect the client from the location engine.

PARAMETERS
  handle:  the handle returned by loc_open

DEPENDENCIES

RETURN VALUE
  the closed client handle

SIDE EFFECTS

===========================================================================*/
int32 loc_close(loc_client_handle_type handle)
{
   int32                      l_RetVal = LOC_API_INVALID_HANDLE;
   locQmiShimCltInfoStructT    *pz_LocClient = NULL;
   qmiLocEventRegMaskT_v02      t_EventRegMask = 0;

   pz_LocClient = locClnt_FindHandle(handle);

   if (pz_LocClient == NULL)
   {
      LOC_MSG_ERROR("loc_close, invalid client handle %d", handle,0, 0);
      return l_RetVal;
   }
   l_RetVal = LOC_API_SUCCESS;
   t_EventRegMask = pz_LocClient->t_EventMask;

   /* first stop the session */
   (void) loc_stop_fix (handle);

   /* stop any TBB requests from this client */
   /* check if this is batching client and do cleanup */
   if (TRUE == pz_LocClient->batching_info.is_batching_client)
   {
      // force stop all AON requests
      locAonForceStopAll( handle);

      /* free the buffer and reset client info */
      (void)loc_batching_free_and_reset(pz_LocClient);

      LOC_MSG_HIGH("loc_close : cleaned up TBB client", 0, 0, 0);
   }

   /* stop any OTB requests from this client */
   /* check if this is OTB client and do cleanup */
   if (TRUE == pz_LocClient->z_OtbInfo.v_OtbClient)
   {
      // force stop all AON requests
      locAonForceStopAll( handle);

      /* free the buffer and reset client info */
      //(void)locOtb_FreeAndReset(pz_LocClient);

      LOC_MSG_HIGH("loc_close : cleaned up OTB client", 0, 0, 0);
   }

   

   LOC_MSG_HIGH ("loc_close handle %d returned %d", handle, l_RetVal, 0);
   return l_RetVal;
}

/*===========================================================================
FUNCTION loc_close

DESCRIPTION
  This function discconects the client from the location engine.

PARAMETERS
  handle:  the handle returned by loc_open

DEPENDENCIES

RETURN VALUE
  On success: it returns 0 (LOC_API_SUCCESS).
  On failure, it will return one-zero error code.

SIDE EFFECTS

===========================================================================*/
int32 loc_start_fix
(
  loc_client_handle_type handle
)
{
   int                       ret_val;

   ret_val = locClnt_StartFix (handle);
   LOC_MSG_HIGH ("loc_start_fix, client = %d, ret_val = %d", handle, ret_val, 0);

   return ret_val;
}

/*===========================================================================
FUNCTION loc_stop_fix

DESCRIPTION
  This function stops positioning process for this client. If client is not
  currently requesting fixes, it will return LOC_API_SUCCESS.

PARAMETERS
  handle:  the handle returned by loc_open

DEPENDENCIES

RETURN VALUE
  On success: it returns 0 (LOC_API_SUCCESS).
  On failure, it will return one-zero error code.

SIDE EFFECTS

===========================================================================*/
int32 loc_stop_fix
(
  loc_client_handle_type handle
)
{
   int  ret_val;

   ret_val = locClnt_StopFix (handle);
   LOC_MSG_HIGH ("loc_stop_fix, client = %d, ret_val = %d", handle, ret_val, 0);
   return ret_val;
}
