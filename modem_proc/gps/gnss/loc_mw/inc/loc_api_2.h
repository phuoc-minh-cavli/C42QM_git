
/******************************************************************************
  @file: loc_api_2.h
  @brief: Location API header file, Major verion 2

  DESCRIPTION
    Qualcommm Location API header 2.0

  INITIALIZATION AND SEQUENCING REQUIREMENTS
    N/A

  -----------------------------------------------------------------------------
  Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
  All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
08/21/19   ska     Added support for NMEA GSV
03/31/17   yh       loc_ioctl clean up
10/05/16   yh       QMI_LOC_SECURE_GET_AVAILABLE_POSITION_REQ_V02 clean up
05/19/16   yh       LocMW refactoring
01/11/16   yh       Loc_Wifi cleanup
10/05/15   yh       QZSS
06/11/15   sj       LB 2.0 Integration
05/18/15   jv       Added deletion bits for SBAS eph & alm.
09/05/14   ssu     GM LOWI Integration
08/21/14   jv       Added support for 16-bit SVID  
04/15/14   yh       XTRA version check added  
03/10/14   ah       Added support for Set Premium Services Cfg & Get Avail WWAN Pos
01/24/14   ah       Added GLO support for Get/Set NMEA Type Requests in Loc MW
02/25/13   ssu      Adding a new WWAN type used for AGNSS Emergency connections. 
07/14/11   ly       Initial version

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/inc/loc_api_2.h#1 $
$DateTime: 2020/07/21 21:23:44 $
======================================================================*/

#ifndef LOC_API_2_H
#define LOC_API_2_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "comdef.h"    /* Definition for basic types and macros */
#include "pdapi_lite.h"
#include "location_service_iot_v02.h"

/*=============================================================================
 *
 *                             DATA DECLARATION
 *
 *============================================================================*/


/******************************************************************************
 *  Constants and configuration
 *****************************************************************************/
#define LOC_API_MAJOR_VERSION_NUMBER   2
#define LOC_API_MINOR_VERSION_NUMBER   4

/* Return value for loc_open in case of failure. */
#define LOC_CLIENT_HANDLE_INVALID     -1

/* Return value of loc api calls for loc_close, loc_start_fix, loc_stop_fix 
 * These are also the status for the ioctl callback */
#define LOC_API_SUCCESS                     (0)
#define LOC_API_GENERAL_FAILURE             (1)
#define LOC_API_UNSUPPORTED                 (2)
#define LOC_API_INVALID_HANDLE              (4)
#define LOC_API_INVALID_PARAMETER           (5)
#define LOC_API_ENGINE_BUSY                 (6)
#define LOC_API_PHONE_OFFLINE               (7)
#define LOC_API_TIMEOUT                     (8)
#define LOC_API_XTRA_VERSION_CHECK_FAILURE  (9)
#define LOC_API_GNSS_DISABLED               (10)
#define LOC_API_GNSS_SERVICE_NOT_AVAILABLE  (11)
/******************************************************************************
 *  Misc global data
 *****************************************************************************/
/* Client handle that will be returned to LocAPI Clients */
typedef int32 loc_client_handle_type;

/*=============================================================================
 *
 *           Location data types for API and callback functions
 *
 *============================================================================*/

/*  Conversions of Accuracy Levels from QMI-Loc Levels to Meters */
#define LOC_ACCURACY_HIGH      (50)
#define LOC_ACCURACY_MED       (500)
#define LOC_ACCURACY_LOW       (1000)
#define LOC_DEFAULT_ACCURACY   LOC_ACCURACY_LOW  /* Accuracy is LOW by default */

/******************************************************************************
 *
 *           Union of all location-event request and callback data
 *
 *****************************************************************************/
typedef enum
{
  LOC_CMD_TYPE_PD_SCHEDULE,
  LOC_CMD_TYPE_REPORT_POSITION,
  LOC_CMD_TYPE_NOTIFY_CLIENT,
  LOC_CMD_TYPE_INJECT_XTRA_DATA,
  LOC_CMD_TYPE_SILENTLY_DENY_NI_REQUEST,
  LOC_CMD_TYPE_REPORT_SV_INFO,
}LocMW_LocCmdEnumT;

typedef struct
{
  loc_client_handle_type           l_ClientHandle;
  LocMW_LocCmdEnumT                e_CmdType;
  qmiLocEventRegMaskT_v02            t_EventType;

  // data must be allocated with loc_calloc and freed by loc_free
  // if not freed and set to NULL explicitly by the receiver,
  // locMW_HandleLocCmd would call loc_free for it
  void *p_QmiEventIndMsg;
} LocMW_LocCmdStructT;

/* -----------------------------------------------------------------------*//**
@brief 
 Structure with information that is required for loc_MW handled indication
*/ /* ------------------------------------------------------------------------*/
typedef struct
{
  loc_client_handle_type l_ClientHandle;
  uint32                 q_QmiIndMsgId;

  // QMI_LOC Data must be allocated with loc_calloc and freed by loc_free
  // if not freed and set to NULL explicitly by the receiver,
  // locQmi_ProcessInd would call loc_free for it
  void *p_QmiIndMsg;
  uint32                 q_LocApiStatus;
}locMW_QmiLocIndMsgStructT;


/*9x05: locPd_GetSecureAvailPosType not supported */




/******************************************************************************
 *  Location event callback function type
 *****************************************************************************/
typedef boolean(loc_event_cb_f_type)
   (
    // The loc client for this event, only client registered for this event will be called
    loc_client_handle_type     l_ClientHandle,

    qmiLocEventRegMaskT_v02      t_EventType,

    const void *p_EventPayload
    /*~ PARAM loc_event_payload POINTER DISC loc_event */
    );
/*~ CALLBACK loc_event_cb_f_type */

/*===========================================================================
 *
 *                          FUNCTION DECLARATION
 *
 *==========================================================================*/

/*~ FUNCTION loc_open
     RELEASE_FUNC loc_close(_RESULT_) */

/*===========================================================================
FUNCTION loc_close

DESCRIPTION
  This function discconect the client from the location engine.

PARAMETERS
  handle:  the handle returned by loc_open

DEPENDENCIES

RETURN VALUE
  On success: it returns 0 (LOC_API_SUCCESS).
  On failure, it will return one-zero error code.

SIDE EFFECTS

===========================================================================*/
extern int32 loc_close
(
   loc_client_handle_type handle
   );
/*~ FUNCTION loc_close */

/*===========================================================================
FUNCTION loc_start_fix

DESCRIPTION
  This function starts positioning process. If client is currently requesting
  fixes, this function will be silently ignored and LOC_API_SUCCESS will be
  returned

  When position is available, position result will be passed to client via
  the callback funtion pointer provided in loc_open.

PARAMETERS
  handle:  the handle returned by loc_open

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern int32 loc_start_fix
(
   loc_client_handle_type handle
   );
/*~ FUNCTION loc_start_fix  */

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
extern int32 loc_stop_fix
(
   loc_client_handle_type handle
   );
/*~ FUNCTION loc_stop_fix */

/*---------------------------------------------------------------------------
@brief
  Function to check if loc_MW task is started or not

@retval    TRUE  loc_MW task is started
           FALSE loc_MW task is not started
---------------------------------------------------------------------------*/
extern boolean loc_middleware_initialized (void);




#ifdef __cplusplus
}
#endif

#endif /* LOC_API_2_H */


