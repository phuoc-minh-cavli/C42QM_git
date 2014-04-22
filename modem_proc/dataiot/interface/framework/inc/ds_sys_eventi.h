#ifndef DS_SYS_EVENTI_H
#define DS_SYS_EVENTI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D S _ S Y S _ E V E N T I . H 


GENERAL DESCRIPTION
  This is internal header file that defines functions, enums and structs
  for registering and deregistering events

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module. 
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/framework/inc/ds_sys_eventi.h#3 $ $DateTime: 2023/05/09 08:36:53 $ $Author: pwbldsvc $
 
when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
08/11/20    sp     Added support for VZW PCO requirements.
09/09/11    bvd    Created module 
 
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "queue.h"
#include "dstaski.h"
#include "ds_sys.h"
#include "ds_sys_event.h"
#include "ds_sys_ioctl.h"
#ifdef QWES_FEATURE_ENABLE
#include "ds_caas_mgr_api.h"
#endif
/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/


/** 
  @brief This is buffer that is used to allocate event callbacks, tech_type and
         others and passed on to PS task in order for callback to be called
*/
typedef struct
{
  q_link_type                        link;
  q_type                            *event_q_ptr;
  ds_sys_event_cback_f_ptr_type      event_cback_f_ptr;
  ds_sys_tech_enum_type              tech_type;
  uint64                             event_mask;
  void                               *user_data_ptr;
} ds_sys_eventi_buf_type;



/** 
  @brief This is structure sent along with invoke event callback command to 
          ps task
  */
typedef struct
{
  ds_sys_tech_enum_type               tech_type;
  uint64                              event;
  void                              * event_info;
} ds_sys_eventi_cmd_type;

#ifdef QWES_FEATURE_ENABLE
/**
  @brief Data type to be used with DS_SYS_EVENT_QTEELS_AVAILABLE_EV
   
  @see   DS_SYS_EVENT_QTEELS_AVAILABLE_EV 
*/
typedef struct
{ 
  boolean  is_avail;
} ds_sys_event_qteels_avail_info_type;

/**
  @brief CAAS Feature Id list
   
  @see   DS_SYS_CONF_CAAS_FEATURE_ID_LIST 
*/
typedef struct
{
  uint8   feature_cnt;
  /** Number of feature Ids */  
  int     feature_ids[DS_CAAS_MAX_NUM_FEATURE_IDS];
  /** List of feature Ids */
} ds_sys_caas_feature_list_type;

typedef enum
{
  DS_SYS_CAAS_FEATURE_STATUS_ALLOWED = 0,
  DS_SYS_CAAS_FEATURE_STATUS_NOT_ALLOWED  = 1,
} ds_sys_caas_feature_status_enum_type;



typedef struct
{
  int                             feature_id;
  /** Feature Id */
  
  ds_sys_caas_feature_status_enum_type feature_status;
  /** Feature status */
} ds_sys_caas_feature_status_type;


/**
  @brief CAAS Feature Status List
   
  @see   DS_SYS_EVENT_CAAS_FEATURE_STATUS 
*/
typedef struct
{ 
  ds_sys_caas_feature_status_type  status[DS_CAAS_MAX_NUM_FEATURE_IDS];
  /** Feature status */  
} ds_sys_caas_feature_status_list_type;


typedef boolean ds_sys_event_sync_complete_broadcast_type;
#endif

/**
  @brief Union of all event structures. Its used in ps_sys_event_ind function.
*/
typedef union
{ 
  ds_sys_system_status_type                   system_status_ex;
  ds_sys_roaming_disallowed_info_type         roaming_disallowed_info;
  ds_sys_3gpp_roaming_status_type             roaming_status;
  ds_sys_pdn_throttle_info_type               pdn_throttle_info;
#ifdef QWES_FEATURE_ENABLE
  ds_sys_event_qteels_avail_info_type         qteels_avail_info;
  ds_sys_caas_feature_status_list_type        feature_status_info;
  ds_sys_event_sync_complete_broadcast_type   broadcast_info;
#endif
  ds_sys_event_msisdn_info_type               msisdn_info;
  ds_sys_3gpp_ul_data_delivery_ind_info_type  ul_data_delivery;

} ds_sys_eventi_info_type;

/**
  @brief Temporary storage for configuration data 
*/
typedef struct
{
  ds_sys_system_status_type          * system_status_ex_ptr;
  ds_sys_event_msisdn_info_type      * msisdn_info_ptr;
} ds_sys_confi_tech_all;

/**
  @brief Temporary storage for 3gpp configuration data 
*/
typedef struct
{
  ds_sys_roaming_disallowed_info_type   *roaming_disallowed_info_ptr;  
  ds_sys_3gpp_roaming_status_type        roaming_status;  
  ds_sys_pdn_throttle_info_type         *pdn_throttle_info;
} ds_sys_confi_tech_3gpp;

/**
  @brief Defines different ioctl types
*/
typedef enum
{
  DS_SYS_SYSTEM_STATUS_EV                   =1,
  DS_SYS_3GPP_ROAMING_STATUS_EV             =2,
  DS_SYS_3GPP_PDN_THROTTLE_INFO_EV          =3,
  DS_SYS_DATA_ACTIVITY_RESUMED_EV           =4,
  DS_SYS_3GPP_ROAMING_DISALLOWED_INFO_EV    =5,
  DS_SYS_EVENT_QTEELS_AVAILABLE_EV          =6,
  DS_SYS_EVENT_CAAS_FEATURE_STATUS_EV       =7,
  DS_SYS_EVENT_CAAS_SYNC_COMPLETE_BROADCAST_EV   =8,
  DS_SYS_EVENT_APN_MSISDN_INFO_CHANGE_EV    =9,
  DS_SYS_EVENT_3GPP_UL_DATA_DELIVERY_EV     =10
 }ds_sys_event_enum_type;

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION  DS_SYS_EVENTI_INVOKE_CBACKS()
  
 DESCRIPTION
  @brief  This function calls all callbacks for given event.

  PARAMETERS
  @param[in] tech_type            States Technology type
  @param[in] event                Name of event
  @param[in] event_info           Event information 

  RETURN VALUE
   None

  
  DEPENDENCIES
   None

  SIDE EFFECTS
   None
===========================================================================*/
void ds_sys_eventi_invoke_cbacks
(
  ds_sys_tech_enum_type                      tech_type,
  uint64                                     event_mask,
  void                                      *event_info
);

/*===========================================================================
  FUNCTION  DS_SYS_EVENTI_FREE_CBACK_BUF()
   
  DESCRIPTION
  @brief  This function deallocates event callback buffer. 

  PARAMETERS
  @param[in] buf             Event Buffer to deallocate 

 RETURN VALUE
   None

  DEPENDENCIES
   None

  SIDE EFFECTS
   None
  
===========================================================================*/
void ds_sys_eventi_free_cback_buf
(
  void *buf
);


/*===========================================================================
  FUNCTION  DS_SYS_EVENTI_ALLOC_CBACK_BUF()
    
  DESCRIPTION
  @brief  This function allocates buffer for given event and tech
          type and subscription id.

  PARAMETERS
  @param[in] tech_type            States Technology type

  @param[in] event_cback_f_ex_ptr Callback for event
  @param[in] user_data_ptr        Pointer to User data 
  
  RETURN VALUE
  @return                          Allocated Memory Buffer or Null on error 

    DEPENDENCIES
   None

  SIDE EFFECTS
   None
===========================================================================*/

void *ds_sys_eventi_alloc_cback_buf
(
  ds_sys_tech_enum_type             tech_type,
  ds_sys_event_cback_f_ptr_type     event_cback_f_ptr,
  void                             *user_data_ptr,
  uint64                               event_mask
);

/*===========================================================================
  FUNCTION  DS_SYS_EVENTI_CBACK_CMD_HANDLER()
    
  DESCRIPTION
  @brief  This function is callback for command DS_SYS_EVENT_CMD_HANDLER
           called in Ds task context

  PARAMETERS
  @param[in] cmd                  Command type 
  @param[in] user_data_ptr        Pointer to User data 
  
  RETURN VALUE
   @return                         None 
   
  DEPENDENCIES
   None

  SIDE EFFECTS
   None
===========================================================================*/

void ds_sys_eventi_cback_cmd_handler
(
  ds_command_type  *cmd
);


/*===========================================================================
  FUNCTION  DS_SYS_EVENTI_CHECK_EVENT_CBACK_EXIST()
    
  DESCRIPTION
  @brief  This function checks if event callback for given tech type
          and event exists. It accepts two callback function. 
          One of them should be non NULL.

  PARAMETERS
  @param[in] tech_type            States Technology type 
  @param[in] event_name           Name of event  
  @param[in] event_cback_f_ptr    Callback for event  
  @param[in] event_cback_f_ex_ptr Callback for event

  RETURN VALUE
  @retval TRUE                    Event callback exists 
  @retval FALSE                   Event Callback doesnt exist  

 DEPENDENCIES
   None

  SIDE EFFECTS
   None
===========================================================================*/

boolean ds_sys_eventi_check_event_cback_exist
(
  ds_sys_tech_enum_type                          tech_type,
  uint64                                            event,
  ds_sys_event_cback_f_ptr_type                  event_cback_f_ptr
);

/*===========================================================================
  FUNCTION  DS_SYS_EVENT_IND()

  DESCRIPTION
  @brief  This function sends the indication to all the user who has
          registered for the event for particular technology and default
          subscription

  PARAMETERS
  @param[in] tech_type            States Technology type
  @param[in] event_name           Name of event
  @param[in] event_info           Event specific information 
  @param[out] ps_errno            Error code is filled when return value is not
                                  success. Possible values are
                                  @li DS_EFAULT if parameter is invalid
                                  @li DS_EINVAL if operation is invalid
                                  @li DS_NOMEMORY if memory allocation fails

  RETURN VALUE
  @return  DSS_SUCCESS            On Success.
  @return  DSS_ERROR              On failure. 

   DEPENDENCIES
   None

  SIDE EFFECTS
   None
   
  COMMENTS
  @code 
   
  Example : 
  Assuming user has already registered for PS_SYS_EVENT_3GPP_LTE_ATTACH_INIT. 
  Modehandlers sends out event indication if there is context transfer from 
  other RAT as follow 
   
  ps_sys_event_3gpp_lte_attach_init_type event_info = NULL;
  event_info.attach_profile = 344;
   
  ret_val =  ds_sys_event_ind(PS_SYS_TECH_3GPP,
                              DS_SYS_EVENT_3GPP_ROAMING_DISALLOWED_INFO,
                              &event_info,
                              &ps_errno);
  if(0 > ret_val)
  {
    LOG_MSG_ERROR_1("Event indication failed, err %d", ps_errno);
  } 
   
  @endcode 
===========================================================================*/
int16 ds_sys_event_ind
(
  ds_sys_tech_enum_type             tech_type,
  ds_sys_event_enum_type            event_name,
  void                             *event_info
);

/*===========================================================================
  FUNCTION  DS_SYS_COMMON_IOCTL_HDLR()

  DESCRIPTION
   @brief  This function handles all the common ioctls.

  PARAMETERS

   @param[in] ioctl_name	 Ioctl name
   @param[in] arg_val_ptr	 Pointer to the ioctl result buffer	   
   @param[in] dss_errno      Pointer to the dss_errno
  
  RETURN VALUE
  @return  0                 Success.    

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/

int16 ds_sys_common_ioctl_hdlr
(
  ds_sys_ioctl_enum_type          ioctl_name,
  void                           *arg_val_ptr,
  int16                          *dss_errno
);

/*===========================================================================
  FUNCTION  DS_SYS_INIT()

  DESCRIPTION
   @brief  This function inits the ds_sys module.
           This api inits the internal event queue and inits the DS SYS crit 
           section

  PARAMETERS
   None
   
  RETURN VALUE
   None    

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ds_sys_init 
(
  void 
);

/*===============================================     =================
  FUNCTION  DS_SYS_IOCTL_REGSITERI_CBACK()

  DESCRIPTION
    @brief    This function allows client to register callback for of 
              particular   technology 
  
  PARAMETERS
     @param[in] tech_type             States Technology type
      @param[in] tech_cback_f_ptr      Callback function 
     @param[out] ps_errno             DS_EFAULT if parameter is invalid
                                   DS_EINVAL if operation is invalid
                                   DS_NOMEMORY if memory allocation fails
   RETURN VALUE
     @return   0         On Success.
     @return  -1           On failure. 
  
   COMMENTS
     @code
     Example
      3GPP modehandler registers for ioctl callback as follow 
       
      int16  ioctl_3gpp_cback(...)
      {
        .....
      }
     
      ds_sys_ioctl_lte_attach_pdn_params_type * ioctl_info;
     
      ret_val =  ds_sys_ioctl_register_tech_cback(DS_SYS_TECH_3GPP,
                                              &ioctl_3gpp_cback,
                                              &ps_errno);
     
      if(0 > ret_val)
      {
        LOG_MSG_ERROR_1("Callback reg failed, err %d", ps_errno);
    }
  
      @endcode 
    *
  DEPENDENCIES
    None
    
  SIDE EFFECTS
    None
  ==================================================================*/
int16 ds_sys_ioctl_registeri_cback
(  
 ds_sys_tech_enum_type                        tech_type,
 ds_sys_ioctl_tech_cback_f_ptr_type	          tech_cback_f_ptr,
 int16                                       *ps_errno
);

#endif /* DS_SYS_EVENTI_H */
