/*============================================================================
 @file ldm_api.h

 Location Distribution Manager (LDM) defined APIs

 GENERAL DESCRIPTION

 This file defines interface for the Location Distribution Manager(LDM)

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited. 

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/ldm/api/ldm_api.h#1 $
 $DateTime: 2020/07/21 21:23:44 $
 $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 07/06/17   ak      Add API to update client subscription for measurement/fix reports
 11/09/16   mj      Initial version

 =============================================================================*/

#ifndef _LDM_API_H_
#define _LDM_API_H_

#include "comdef.h"
#include "gps_common.h"
#include "tm_lm_iface.h"

#ifdef __cplusplus
extern "C" {
#endif

/* sets the specified bit in a u32 bit mask. bit counts range: 0-31 */
#define LDM_SET_U32BITMASK_BIT(n) (((uint32)0x1) << (n))

/* The mask of different clients supported by LDM */
#define LDM_CLIENT_E911_MASK           LDM_SET_U32BITMASK_BIT(0) /* 0x1 */
#define LDM_CLIENT_APPTRK_MASK         LDM_SET_U32BITMASK_BIT(1) /* 0x2 */
#define LDM_CLIENT_SINGLE_SHOT_MASK    LDM_SET_U32BITMASK_BIT(2) /* 0x4 */

/* The mask identifying if Fix or Meas or both are present in the CB from LDM to TM */
#define LDM_CLIENT_FIX_MASK           LDM_SET_U32BITMASK_BIT(0) /* 0x1 */
#define LDM_CLIENT_MEAS_MASK          LDM_SET_U32BITMASK_BIT(1) /* 0x2 */

/* LDM client IDs for different clients */
typedef enum
{
  LDM_CLIENT_ID_FIRST = 0,
  /* Client Id for E911 */
  LDM_CLIENT_ID_E911 = LDM_CLIENT_ID_FIRST,
  /* Client Id for Apptrack */
  LDM_CLIENT_ID_APPTRK = 1,
  LDM_CLIENT_ID_SINGLE_SHOT = 2,
  LDM_CLIENT_ID_LAST = LDM_CLIENT_ID_SINGLE_SHOT,
  LDM_CLIENT_ID_MAX = 0xFFFFFFFF
} ldm_client_id_e_type;

/* Indicates if client is main or secondary */
typedef enum
{
  LDM_CLIENT_TYPE_MAIN = 0,
  LDM_CLIENT_TYPE_SECONDARY = 1,
  LDM_CLIENT_TYPE_MAX = 0xFFFFFFFF  
} ldm_client_e_type;


/* LDM operation status */
typedef enum
{
  /* LDM operation status FAILURE for an unknown reason.  */
  LDM_OP_STATUS_ERROR_UNKNOWN,
  /* LDM operation status FAILURE if client is unregistered. */
  LDM_OP_STATUS_ERROR_CLIENT_UNREGISTERED,
  /* LDM operation status FAILURE if client is already registered. */
  LDM_OP_STATUS_ERROR_CLIENT_ALREADY_REGISTERED,
  /* LDM operation status FAILURE if client is unregistered. */
  LDM_OP_STATUS_ERROR_BAD_PARAMS,
  /* LDM operation status FAILURE for memory allocation failure  */
  LDM_OP_STATUS_ERROR_MEM_ALLOC_FAILURE,
  /* LDM was able to provide the ACCESS grant to the client*/
  LDM_OP_STATUS_ACCESS_GRANT,
  /* LDM was able to provide ACCESS DENIED to the client*/
  LDM_OP_STATUS_ACCESS_DENIED,
  /* LDM operation completed succesfully. */
  LDM_OP_STATUS_REQUEST_SUCCESS,
  /* LDM operation resulted in a failure */
  LDM_OP_STATUS_REQUEST_FAILURE,
  LDM_OP_STATUS_ERROR_MAX = 0xFFFFFFFF
} ldm_op_status_e_type;

/* LDM client states */
typedef enum
{
  /* Unregistered: The client is not registered and thus the client cannot be controlled by any controller.  */
  LDM_CLIENT_STATE_UNREGISTERED,
  /* CLIENT_STATE_DISABLED: The client is registered, but it does not have the access grant . The client is INACTIVE at this point. */
  LDM_CLIENT_STATE_DISABLED,
  /* CLIENT_STATE_ENABLED: The client has access grant. The client is ACTIVE at this point. */
  LDM_CLIENT_STATE_ENABLED,
  LDM_CLIENT_STATE_MAX = 0xFFFFFFFF
} ldm_client_state_e_type;


/* Data type having the information for the LDM client callbacks. */
typedef struct
{  
  /* The client handler identifying the client type */
  ldm_client_id_e_type e_client_id;
  ldm_client_e_type e_client_type;
  tm_session_update_info_s_type z_sess_update_info;  
}ldm_client_cb_data_s_type;

/* -----------------------------------------------------------------------*//**
@brief
  Function callback prototype for the LDM client.
@param[in]   p_client_cb    The additional information about the callbacks.

@retval    NONE

*//* ------------------------------------------------------------------------*/
typedef void (ldm_client_cb_f_type)
(
  const ldm_client_cb_data_s_type *p_client_cb_data
);


typedef struct
{
  ldm_client_cb_f_type *p_cb_func;
}ldm_client_cb_s_type;

/* -----------------------------------------------------------------------*//**
@brief
  This function is called to initialize LDM module.

@retval      ldm_op_status_e_type   The operation status of the LDM.
*//* ------------------------------------------------------------------------*/
ldm_op_status_e_type ldm_init(void);

/* -----------------------------------------------------------------------*//**
@brief
  Function to register the clients with LDM common module.

@param[in]   p_cb                            The client callback needed to regsiter with LDM.
@param[in]   e_client_id                     The client ID which identifies the type of client.
@retval    ldm_op_status_e_type   The operation status of the LDM.

*//* ------------------------------------------------------------------------*/
ldm_op_status_e_type ldm_client_register(const ldm_client_cb_s_type *p_cb,
                                           const ldm_client_id_e_type e_client_id);


/* -----------------------------------------------------------------------*//**
@brief
  Function to de-register the clients with LDM common module.

@param[in]   e_client_id           The client ID which identifies the type of client.
@retval    ldm_op_status_e_type   The operation status of the LDM.

*//* ------------------------------------------------------------------------*/
ldm_op_status_e_type ldm_client_deregister(const ldm_client_id_e_type e_client_id);


/* -----------------------------------------------------------------------*//**
@brief
  This function is called when client controller decides to enable the client.
  LDM uses this function to consider the client's request and synchrnously responds to the client with a decision
  to enable the client or not .
  
@param[in]   e_client_id                The client ID which identifies the type of client. 
@param[in]   q_fix_meas_mask     This mask notifies if the client is requesting for only fixes or only measurements or both                                                                   
@retval    ldm_op_status_e_type   The operation status of the LDM. Th return value of this API indicates the enable request 
                                                   status of the e_client_id. 
*//* ------------------------------------------------------------------------*/
ldm_op_status_e_type ldm_client_enable_request(const ldm_client_id_e_type e_client_id, 
                                                   const uint8 q_fix_meas_request_mask);


/* -----------------------------------------------------------------------*//**
@brief
  This function is called when client controller decides to disable the client.
  LDM uses this function to consider the client's request and synchrnously responds to the client with a decision
  to disable the client or not .
  If this API returns a SUCCESS then the client is expected to clean up its state and synchronously respond to LDM 
  that it has disabled using the ldm_client_disabled API

@param[in]   e_client_id           The client ID which identifies the type of client.

@retval    ldm_op_status_e_type   The operation status of the LDM.
*//* ------------------------------------------------------------------------*/
ldm_op_status_e_type ldm_client_disable_request(const ldm_client_id_e_type e_client_id);


/* -----------------------------------------------------------------------*//**
@brief
  This function is called when fixes are received from LM to TM and it needs to be distributed to 
  all active sessions that subscribed to it.
  
@param[in]   z_sess_update_info          Holds fix related info that needs to be distributed
*//* ------------------------------------------------------------------------*/
void ldm_distribute_location_info(const tm_session_update_info_s_type* zp_sess_update_info);

/* -----------------------------------------------------------------------*//**
@brief
  This function is used by the client controller to query the state of the LDM clients in the system 

@retval  A mask of ENABLED/DISABLED status for each LDM client in the system.
Each bit in the mask corresponds to a LDM client . The bit mask for each client is already defined 
in the API header file .
For each client an ENABLED shall be returned only if the client has been granted ACCESS by LDM.
In all other cases LDM shall return DISABLED for the client. 

*//* ------------------------------------------------------------------------*/
uint32 ldm_client_query_state(void);

/* -----------------------------------------------------------------------*//**
@brief
  This function is used by the LDM to update the subscription of an enabled client
@param[in]   e_client_id                The client ID which identifies the type of client.   
@param[in]   q_fix_meas_mask     This mask notifies if the client is requesting for only fixes or only measurements or both     
@retval    ldm_op_status_e_type   The operation status of the LDM.

*//* ------------------------------------------------------------------------*/
ldm_op_status_e_type ldm_client_update_subscription(const ldm_client_id_e_type e_client_id, const uint8 q_fix_meas_request_mask);

#ifdef __cplusplus
}
#endif

#endif  //_LDM_API_H_

