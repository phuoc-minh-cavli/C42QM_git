/**
@file ds_sys_ioctl.h
@brief
This file defines functions, enums and structs used for two modehandlers to 
communicate with each other through ioctls 
*/

#ifndef DS_SYS_IOCTL_H
#define DS_SYS_IOCTL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ S Y S T E M_ I O C T L. H


GENERAL DESCRIPTION
  This file defines functions, enums and structs used for two modehandlers
  to communicate with each other through ioctls

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/api/public/ds_sys_ioctl.h#4 $ $DateTime: 2021/10/04 03:27:13 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/09/18   sk    Created module

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_sys.h"

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/**
  @brief Defines different ioctl types
*/
typedef enum
{
  DS_SYS_IOCTL_MIN                             = 0x00,
  	
  /*3GPP specific ioctl*/
  DS_SYS_IOCTL_3GPP_MIN                        = 1,
  
  DS_SYS_IOCTL_3GPP_LTE_ATTACH_PDN_PARAMS      = DS_SYS_IOCTL_3GPP_MIN + 1,
  /**< 
      User can query PDN name anytime however it was introduced for App 
      Processor to query the modem for the Attach PDN's name whenever it 
      gets notified about LTE SYS via the DATA_SYS_STATUS notification. 
      In response, User should expect attach pdn name.
  */  

  DS_SYS_IOCTL_3GPP_PDN_THROTTLE_INFO          = DS_SYS_IOCTL_3GPP_MIN +2,

  DS_SYS_IOCTL_3GPP_PSM_RESTORATION_STATE      = DS_SYS_IOCTL_3GPP_MIN +3,

  DS_SYS_IOCTL_3GPP_GET_APN_MSISDN_INFO        = DS_SYS_IOCTL_3GPP_MIN+4,
  
  DS_SYS_IOCTL_3GPP_SET_L2_ENABLE_RAI          = DS_SYS_IOCTL_3GPP_MIN+5,

  DS_SYS_IOCTL_3GPP_MAX                        = 100,

  /*Common ioctl*/
  DS_SYS_IOCTL_COMMON_MIN                       = 200,

  DS_SYS_IOCTL_SYS_STATUS                       = DS_SYS_IOCTL_COMMON_MIN + 1,
  
  DS_SYS_IOCTL_GET_GLOBAL_STATS                 = DS_SYS_IOCTL_COMMON_MIN + 2,
  
  DS_SYS_IOCTL_COMMON_MAX                       = 300,
  
  DS_SYS_IOCTL_MAX
} ds_sys_ioctl_enum_type;

/**
  @brief Defines enum type representing different ip types
   
  @see   ds_sys_ioctl_3gpp_lte_attach_pdn_params_type 
*/
typedef enum
{
  DS_SYS_IOCTL_IP_SUPPORT_TYPE_IPV4,     /**<  IPV4 type */
  DS_SYS_IOCTL_IP_SUPPORT_TYPE_IPV6,     /**<  IPV4 type */
  DS_SYS_IOCTL_IP_SUPPORT_TYPE_IPV4IPV6,  /**<  IPV4 and IPV6 type */  
  DS_SYS_IOCTL_IP_SUPPORT_TYPE_NON_IP    /** < NON IP */
} ds_sys_ioctl_ip_support_type_enum_type;

/**
  @brief Defines structure for DS_SYS_IOCTL_3GPP_LTE_ATTACH_PDN_PARAMS ioctl 
         command.
   
  @see   DS_SYS_IOCTL_3GPP_LTE_ATTACH_PDN_PARAMS 
*/ 
typedef struct
{
  ds_sys_ioctl_ip_support_type_enum_type ip_type; /** <  IP types */
  char  apn_string[DS_SYS_MAX_APN_LEN]; /**< Attached PDN name */
  uint8 apn_string_len;                /**<  Length of attached PDN name */
  boolean ota_attach_performed;        /**<  LTE OTA Attach state */
} ds_sys_ioctl_3gpp_lte_attach_pdn_params_type;


/**
  @brief Callback function to be returned for ioctl operation 
*/
typedef int16 (* ds_sys_ioctl_tech_cback_f_ptr_type)
(
  ds_sys_ioctl_enum_type               ioctl_name,
  void                                *arg_val_ptr,
  int16                               *ps_errno
);

/**
  @brief Defines structure where diffent modehandlers can register their ioctl
         functions
*/
typedef struct
{
  ds_sys_ioctl_tech_cback_f_ptr_type      tech_f_ptr; /**< Modehandler specific                                                           Callback */
}ds_sys_tech_func_type;


/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION  DS_SYS_IOCTL()

  DESCRIPTION
  @brief  This function performs the operation for particular ioctl

  PARAMETERS
  @param[in] tech_type            States Technology type
  @param[in] ioctl_name           Operation name
  @param[in] arg_val_ptr          Pointer to operation specific
  @param[out] dss_errno           Error code is filled when return value is not
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
 
  Example
  Connection manager can tell modehandlers whether to allow dun call for
  particular call id  or not.
 
  ds_sys_ioctl_dun_ctrl_response_type * ioctl_info;
  ioctl_info->call_id = xxx;
  ioctl_info->allow_call = TRUE;
 
  ret_val =  ds_sys_ioctl(DS_SYS_TECH_3GPP,
                          DS_SYS_IOCTL_DUN_CTRL_RESPONSE,
                          ioctl_info,
                          &ds_errno);
  if(0 > ret_val)
  {
    LOG_MSG_ERROR("Ioctl failed, err %d", ds_errno,0,0);
  }
 
  @endcode
===========================================================================*/
int16 ds_sys_ioctl
(
  ds_sys_tech_enum_type           tech_type,
  ds_sys_ioctl_enum_type          ioctl_name,
  void                           *arg_val_ptr,
  int16                          *dss_errno
);

/*===========================================================================
  FUNCTION  DS_SYS_IOCTL()
  
  DESCRIPTION
  @brief  This function allows client to register callback for of particular
          technology 

  PARAMETERS
  @param[in] tech_type            States Technology type
  @param[in] tech_cback_f_ptr     Callback function 
  @param[out] ps_errno            DS_EFAULT if parameter is invalid
                                  DS_EINVAL if operation is invalid
                                  DS_NOMEMORY if memory allocation fails

  RETURN VALUE
  @return  DSS_SUCCESS            On Success.
  @return  DSS_ERROR              On failure. 

   
  DEPENDENCIES
   None
  
  SIDE EFFECTS
   None

  COMMENTS
  @code
 
  Example
  3GPP modehandler registers for ioctl callback as follow 
   
  int16  ioctl_3gpp_cback(...)
  {
    .....
  }
 
  ds_sys_ioctl_lte_attach_pdn_params_type * ioctl_info;
 
  ret_val =  ds_sys_ioctl_register_tech_cback(PS_SYS_TECH_3GPP,
                                              &ioctl_3gpp_cback,
                                              &ps_errno);
 
  if(0 > ret_val)
  {
    LOG_MSG_ERROR_1("Callback reg failed, err %d", ps_errno);
  }
   
  @endcode 
===========================================================================*/
int16 ds_sys_ioctl_register_tech_cback
(
  ds_sys_tech_enum_type                 tech_type,
  ds_sys_ioctl_tech_cback_f_ptr_type    tech_cback_f_ptr,
  int16                                *ps_errno
);



/**************************************************************************
  Added to remove the compilation errors in uimglobals.h and uimdrv.h files
***************************************************************************/

typedef struct
{
  uint64 global_pkt_cnt;
} ds_sys_ioctl_global_stats_type;


#endif /* DS_SYS_IOCTL_H */
