/*===========================================================================

                D S _ Q M I _ P W R M G M T _ S H I M . C

DESCRIPTION

 The Data Services QMI Services Source file. Contains routines for
 power management shim layer between QMI and other subsystems

EXTERNALIZED FUNCTIONS

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/qmidata/src/ds_qmi_pwrmgmt_shim.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/14/10    ar     Created module
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_QMI_APPS_POWER_COLLAPSE

#include "amssassert.h"
#include "ds_qmi_svc.h"
#include "dem.h"

#include "data_msg.h"

/*===========================================================================

                                 DEFINITIONS

===========================================================================*/

/* Array to map external module power events to QMI states.  Used to
 * mitigate possible value range clashes. */
struct {
  void*                          event;
  qmi_svc_pwr_state_handle_type  state;
} static qmi_pwrmgmt_shim_event_map[] =
{
  { (void*)DEM_NOTIFY_POWER_RESTORED,    QMI_SVC_PWRSAVE_STATE_NORMAL  },
  { (void*)DEM_NOTIFY_SUSPEND,           QMI_SVC_PWRSAVE_STATE_SUSPEND },
  { (void*)DEM_NOTIFY_POWERDOWN,         QMI_SVC_PWRSAVE_STATE_PWRDOWN },
};

#define QMI_PWRMGMT_SHIM_EVENT_MAP_SIZ \
        sizeof(qmi_pwrmgmt_shim_event_map) / sizeof(qmi_pwrmgmt_shim_event_map[0])

/* Define QMI instance range used by Apps processor */
#define QMI_SHIM_APPS_PROC_INST_MIN 0
#define QMI_SHIM_APPS_PROC_INST_MAX 4



/*===========================================================================

                               FUNCTIONS

===========================================================================*/


/*===========================================================================
  FUNCTION QMI_PWRMGMT_SHIM_MAP_EVENT_TO_STATE
  
  DESCRIPTION
    Function to map event from external power management module to QMI
    power state handle.
 
  PARAMETERS
    None
   
  RETURN VALUE
    TRUE on successful mapping
    FALSE otherwise
   
  DEPENDENCIES
    None
   
  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_pwrmgmt_shim_map_event_to_state
(
  void*                           event,
  qmi_svc_pwr_state_handle_type  *state_ptr
)
{
  uint8 i = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT( state_ptr );

  /* Search for marching event in lookup table */
  for( i=0; i < QMI_PWRMGMT_SHIM_EVENT_MAP_SIZ ; i++)
  {
    if( event == qmi_pwrmgmt_shim_event_map[i].event )
    {
      /* Match found, return state handle */
      *state_ptr =qmi_pwrmgmt_shim_event_map[i].state; 
      return TRUE;
    }
  }
  
  return FALSE;
} /* qmi_pwrmgmt_shim_map_event_to_state() */



/*===========================================================================
  FUNCTION QMI_PWRMGMT_SHIM_DEM_EVENT_CB
  
  DESCRIPTION
    Function to recieve notification of DEM subsystem power management
    state changes.
 
  PARAMETERS
    None
   
  RETURN VALUE
    None
   
  DEPENDENCIES
    None
   
  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_pwrmgmt_shim_dem_event_cb
(
  struct DEM_NOTIFICATION_STRUCT *info_ptr
)
{
  qmi_svc_pwr_state_handle_type  state_handle = QMI_SVC_PWRSAVE_STATE_INVALID;
  uint8 instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT( NULL != info_ptr );

  DS_MSG1(MSG_LEGACY_MED, "Shim EventCB: event=%d", info_ptr->event );

  /*-------------------------------------------------------------------------
    Lookup map of QMI state handle based on received event.
  -------------------------------------------------------------------------*/
  if( TRUE != qmi_pwrmgmt_shim_map_event_to_state( (void*)info_ptr->event,
                                                   &state_handle ) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Unsupported event received, dropping notificaion" );
    return;
  }
    
  /*-------------------------------------------------------------------------
    Notify QMI service layer of power state change on Apps processor.
    New power state is applied to all associated QMI instances.
  -------------------------------------------------------------------------*/
  for( instance  = QMI_SHIM_APPS_PROC_INST_MIN;
       instance <= QMI_SHIM_APPS_PROC_INST_MAX;
       instance++)
  {
    qmi_svc_pwr_save_cback( instance, state_handle );
  }

} /* qmi_pwrmgmt_shim_dem_event_cb() */



/*===========================================================================
  FUNCTION QMI_PWRMGMT_SHIM_INIT

  DESCRIPTION
    Function to initialize QMI shim layer elements.
 
  PARAMETERS
    None
   
  RETURN VALUE
    TRUE on successful operation
    FALSE otherwise
   
  DEPENDENCIES
    External subsystems must be initialized.

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_pwrmgmt_shim_init( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define DEMRegister(event) \
    if( 0 == DEMNotificationRegister( SMSM_APPS, /* Monitor target */    \
                                      (event),                           \
                                      DEM_NOTIFICATION_PRIORITY_NORMAL,  \
                                      0,                                 \
                                      qmi_pwrmgmt_shim_dem_event_cb ))   \
    {                                                                    \
      DS_MSG1(MSG_LEGACY_ERROR,"Failed to register with DEM subsystem!: %d",            \
                (event));                                            \
      return FALSE;                                                      \
    }

  /*-------------------------------------------------------------------------
    Register callback with DEM subsystem.
  -------------------------------------------------------------------------*/
  DEMRegister( DEM_NOTIFY_POWERDOWN );
  DEMRegister( DEM_NOTIFY_SUSPEND );
  DEMRegister( DEM_NOTIFY_POWER_RESTORED );

  return TRUE;
} /* qmi_pwrmgmt_shim_init() */

#endif /* FEATURE_DATA_QMI_APPS_POWER_COLLAPSE */
