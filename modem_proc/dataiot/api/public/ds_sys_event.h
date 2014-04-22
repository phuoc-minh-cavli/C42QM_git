/**
@file ds_sys_event.h
@brief
This file defines functions, enums and structs use to register for events
*/

#ifndef DS_SYS_EVENT_H
#define DS_SYS_EVENT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D S _ S Y S T E M_ E V E N T . H


GENERAL DESCRIPTION
  This file defines functions, enums and structs used to register for an event

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/api/public/ds_sys_event.h#3 $ $DateTime: 2023/05/09 08:36:53 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/09/18   sk      Create module

===========================================================================*/


/*===========================================================================


                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_sys.h"

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Masks of different evenst which client can register for 
 Check for the MIN and MAX range any new event should be included between 
 the MIN and MAX value for different MH.*/
 
#define DS_SYS_EVENT_MIN_EX                          0x00

#define DS_SYS_EVENT_3GPP_MIN_EX                     0X01

#define DS_SYS_EVENT_3GPP_ROAMING_DISALLOWED_INFO_EX 0x02
/**<
       Event is triggered whenever the mode handler sends the list of apn names.
       Client gets to know the apns on which the client will not attempt to make call.
       Any active call on those apns will be torn down.*/

#define DS_SYS_EVENT_3GPP_ROAMING_STATUS              0x04
/**<		
	  Event is triggered when the UE moves away from the home network and is roaming.
	  User registers for this event to know if the UE is in the home network or roaming.
 */


#define DS_SYS_EVENT_3GPP_PDN_THROTTLE_INFO          0x08
/**< 
	 The event is triggered when PDN is throttled or un-throttled. In 
	 response of event. User gets to know which PDN is throttled right now
	 and what is the remaining time for each PDN throttling. After recieving
	 this event, user has to go through PDN throttle table to see which PDN
	 is throttled and only make the call for the PDN not throttled.
*/

#define DS_SYS_EVENT_3GPP_UL_DATA_DELIVERY_INFO           0x10


#define DS_SYS_EVENT_3GPP_MAX_EX                     0x8000

/*-------------------------------------------------------------------------
   COMMON EVENTS
 -------------------------------------------------------------------------*/

#define DS_SYS_EVENT_COMMON_MIN_EX                   0x10000

#define DS_SYS_EVENT_SYSTEM_STATUS                   0x20000
/**<		
	 Event is triggered if UE camps on	to a different network or 
	 RAT/SO is changed within the same network. In response of event, User
	 gets to know which network is on right now. When user receives the event,
	 it knows which network is UE on. User can register whenever they need to
	 know system status information.
*/
#define DS_SYS_EVENT_DATA_ACTIVITY_RESUMED           0x40000

#define DS_SYS_EVENT_QTEELS_SERVICE_AVAILABLE        0x80000

#define DS_SYS_EVENT_CAAS_FEATURE_STATUS             0x100000

#define DS_SYS_EVENT_CAAS_SYNC_COMPLETE_BROADCAST    0x200000

#define DS_SYS_EVENT_APN_MSISDN_INFO_CHANGE          0x400000

#define DS_SYS_EVENT_COMMON_MAX_EX                   0x20000000


/**
  @brief Callback function provided by client which is to be called when
         particular event is triggered for a given technology
   
  @param[in] tech_type            States Technology type
  @param[in] event_name           Name of event
  @param[in] event_info_ptr       Event related information
  @param[in] user_data_ptr        User Data 
   
  @see ds_sys_event_reg  
*/
typedef void (* ds_sys_event_cback_f_ptr_type)
(
  ds_sys_tech_enum_type                     tech_type,
  uint64                                    event_mask,
  void                                     *event_info_ptr,
  void                                     *user_data_ptr
);

/**
  @brief Callback function provided by client which is to be called when
         user has registered for non-default subscription event
   
  @param[in] tech_type            States Technology type
  @param[in] event_name           Name of event
  @param[in] subscription_id      Subscription id
  @param[in] event_info_ptr       Event related information
  @param[in] user_data_ptr        User Data 
   
  @see ds_sys_event_reg  
*/


/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
      FUNCTION  DS_SYS_EVENT_REG()

  DESCRIPTION
  @brief  This function allows client to register for particular event.
          Subscription id defaults to default primary subscription. In 
          order to deregister the event registered using ds_sys_event_reg, 
          user has to use ds_sys_event_dereg API

  PARAMETERS
  @param[in] tech_type            States Technology type
  @param[in] event_mask           Mask of event/events
  @param[in] event_cback_f_ptr    Callback function when event is triggered
  @param[in] user_data_ptr        User Data
  @param[out] dsss_errno          Error code is filled when return value is not
                                  success. Possible values are
                                  @li DS_EFAULT if parameter is invalid
                                  @li DS_EINVAL if operation is invalid
                                  @li DS_NOMEMORY if memory allocation fails

  RETURN VALUE
  @return  DSS_SUCCESS           On Success.
  @return  DSS_ERROR             On failure.

   DEPENDENCIES
    None

  SIDE EFFECTS
    None

 .COMMENTS
  @code
 
  Example :
  In below code snippet, user registers for DS_SYS_EVENT_SYSTEM_STATUS_CHANGE
  event, In caseof success user receives event info in the callback. 
  system_status_change_cb is of type ds_sys_event_cback_f_ptr_type.
 
 
  void system_status_change_cb(..)                                               
  {                                                                         
    ds_sys_system_status_type * event_info = NULL;             
    LOG_MSG_INFO("Event callback recieved");
                                                                            
    event_info =  (ds_sys_system_status_type *)event_info_ptr; 
    LOG_MSG_INFO("Preferred network %d", event_info->preferred_network);   
  }                                                                         
                                                                            
  ret_val =  ds_sys_event_reg(DS_SYS_TECH_3GPP,                             
                              DS_SYS_EVENT_SYSTEM_STATUS_CHANGE,            
                              &system_status_change_cb,                          
                              NULL,                                         
                              &ds_errno);                                   
  if(0 > ret_val)                                                           
  {                                                                         
    LOG_MSG_ERROR("Event registration failed, err %d", ds_errno,0,0);      
  }
 
  @endcode

===========================================================================*/

int16 ds_system_event_reg     
(
  ds_sys_tech_enum_type                  tech_type,
  uint64                                    event_mask,
  ds_sys_event_cback_f_ptr_type          event_cback_f_ptr,
  void                                  *user_data_ptr,
  int16                                 *dss_errno
);

/*===========================================================================
  FUNCTION  DS_SYS_EVENT_DEREG()

  DESCRIPTION
  @brief  This function deregisters callback for the event. Subscription id
          defaults to default primary subscription. Callback registered through 
          ds_sys_event_reg should be deregistered using this API

  PARAMETERS
  @param[in] tech_type           States Technology type
  @param[in] event_mask          Mask of event/events
  @param[in] event_cback_f_ptr   Event callback function to deregister
  @param[out] dss_errno          Error code is filled when return value is 
                                 not success. Possible values are
                                 @li DS_EFAULT if parameter is invalid
                                 @li DS_EINVAL if operation is invalid
                                 @li DS_NOMEMORY if memory allocation fails

  RETURN VALUE
  @return  DSS_SUCCESS           On Success.
  @return  DSS_ERROR             On failure. 
   
  DEPENDENCIES
    None

  SIDE EFFECTS
   None

  COMMENTS
  For the 3GPP events use the the tech type DS_SYS_TECH_3GPP .
  For the SYSYTEM events use the tech type DS_SYS_TECH_ALL.
  @Note The 3GPP and system evnets can not be clubbed together for the 
       registration.
       However clients can club all 3gpp and system events saparetely and pass 
       to the reg api in one api call itself 
  @code 
   
  Example :
  In below code snippet, user registers for DS_SYS_EVENT_SYSTEM_STATUS_CHANGE
  event,  User use same event name, tech type and callback it used to register 
  for deregistering the event. 
 
  void system_status_change_cb(..) 

  { 
    ...
  }
 
  ret_val =  ds_sys_event_reg(DS_SYS_TECH_3GPP,
                              DS_SYS_EVENT_SYSTEM_STATUS_CHANGE,
                              &system_status_change_cb,
                              NULL,
                              &ds_errno);
  if(0 > ret_val) 
  { 
    LOG_MSG_ERROR("Event registration failed, err %d", ds_errno,0,0);
  } 
   
  ret_val =  ds_sys_event_dereg(DS_SYS_TECH_3GPP,
                                DS_SYS_EVENT_SYSTEM_STATUS_CHANGE,
                                &system_status_change_cb,
                                &ds_errno);
  if(0 > ret_val) 
  { 
    LOG_MSG_ERROR("Event deregistration failed, err %d", ds_errno,0,0);
  } 
   
  @endcode  
  
===========================================================================*/
int16 ds_system_event_dereg
(
  ds_sys_tech_enum_type                  tech_type,
  uint64                                 event_mask,
  ds_sys_event_cback_f_ptr_type          event_cback_f_ptr,
  int16                                 *dss_errno
);

#endif /* DS_SYS_EVENT_H */
