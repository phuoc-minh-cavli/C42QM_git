/*============================================================================
 @file ldm_client.h

 LDM common module internal client data structures are defined in this file. 

 GENERAL DESCRIPTION

 This file defines the client data structures for the LDM common module.

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/ldm/inc/ldm_client.h#1 $
 $DateTime: 2020/07/21 21:23:44 $
 $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 11/09/16   mj      Initial version

 =============================================================================*/
#ifndef _LDM_CLIENT_H_
#define _LDM_CLIENT_H_

#ifdef __cplusplus
extern "C"
{
#endif
  #include "ldm_api.h"
#ifdef __cplusplus
}
#endif


/*  The maximum number of clients supported by LDM */
#define LDM_NUM_CLIENT_MAX LDM_CLIENT_ID_LAST+1 


/* LDM client request types */
typedef enum
{
  LDM_CLIENT_REQUEST_TYPE_FIRST = 0,
  /* The LDM LOG type for logging LDM client requests */
  LDM_CLIENT_REQUEST_TYPE_REGISTER = LDM_CLIENT_REQUEST_TYPE_FIRST,
  /* The LDM LOG type for logging indications from LDM to clients */
  LDM_CLIENT_REQUEST_TYPE_DEREGISTER = 1,
  LDM_CLIENT_REQUEST_TYPE_ENABLE = 2,
  LDM_CLIENT_REQUEST_TYPE_DISABLE = 3,
  LDM_CLIENT_REQUEST_TYPE_DISABLED = 4,
  LDM_CLIENT_REQUEST_TYPE_RESET = 5,
  LDM_CLIENT_REQUEST_TYPE_LAST = LDM_CLIENT_REQUEST_TYPE_RESET,
  LDM_CLIENT_REQUEST_TYPE_MAX = 0xFFFFFFFF,
} ldm_client_request_e_type;


/* The LDM client class which holds the information of the registered LDM client */
class LdmClient
{
private:
  /* The client ID */
  ldm_client_id_e_type e_client_id;
  /* The current state of the client */
  ldm_client_state_e_type e_client_state;
  /* Bitmask indicating the subscription details (Fix/Meas)*/
  uint8 u_location_subscription_mask;
  /* Callback registered by the client */
  ldm_client_cb_s_type *p_cb;
 
  
public:
  /**
   * Constructor
   * @param e_id The client ID for which the client is created. 
   * @param p_client_cb The callback registerd by the client controller to receive asynchronous responses from LDM.  
   */
  LdmClient(ldm_client_id_e_type e_id, ldm_client_cb_s_type *p_client_cb);
  
  /**
   * Function used by LDM to enable the client .
   * @return  Operation status of LDM 
  */
  ldm_op_status_e_type enableClient(uint8 u_subscription_mask);

  /**
   * Function used by LDM to disable the client. 
   * @return  Operation status of LDM 
  */
  ldm_op_status_e_type disableClient(void);

  /**
   * Function used to retrieve the ID of the registered client . 
   * @return  The client ID of the client.
  */
  ldm_client_id_e_type getClientId(void);

  /**
   * Function used to retrieve the cb func of client. 
   * @return  cb function for the client.
  */
  ldm_client_cb_s_type* getClientCb(void);
  
  /**
   * Function used to check if a client is enabled.
   * @return  The client ID of the client.
  */
  boolean isClientEnabled(void);
    
  /**
   * Function to get the current state of the LDM client
   * @param   None
   * @return  The state of LDM client 
  */
  ldm_client_state_e_type getClientState(void);

  /**
   * Function to get the current subscription of the LDM client
   * @param   None
   * @return  Client subscription bit mask
  */
  uint8 getClientSubscription(void);

  /**
   * Function to set the current subscription of the LDM client
   * @param   Client subscription bit mask
   * @return    none
  */
  void setClientSubscription(uint8 u_client_sub);

  /**
   * Function to convert client ID to client mask in LDM .
   * @param   e_client_id The client ID which needs to be converted to client mask. 
   * @return  The converted client mask . 
   */
  uint32 convertClientIdToMask(ldm_client_id_e_type e_client_id);
  
  /** Destructor*/
  ~LdmClient();
};


/* The static class which maintains registered client list */
class LdmClientList
{
private:
  static LdmClient* q_client_list[LDM_NUM_CLIENT_MAX];

public:
  /**
   * Constructor
   */
  LdmClientList(){};
  /**
   * Function to add a client into the client list maintained by LDM. 
   * @param  p_client - The pointer to the client object which needs to be added to the list . 
   * @param  e_client_id - The client ID which needs to be added to the list . 
  */
  static boolean addClient(LdmClient* p_client, ldm_client_id_e_type e_client_id);

  /**
   * Function to remove a client from the client list maintained by LDM. 
   * @param  e_client_id - The client ID which needs to be removed from the list . 
  */
  static boolean removeClient(ldm_client_id_e_type e_client_id);

  /**
   * Function to get the client object from the client info list maintained by LDM. 
   * @param  e_client_id - The client ID which needs to be removed from the list . 
  */
  static LdmClient* getClient(ldm_client_id_e_type e_client_id);

  /**
   * Function to obtain the status of each of the LDM client in the system.
   * @param  e_list_type - The type of list from which the client information needs to be printed. 
  */
  static uint32 queryClientState(void);

  /**
   * Function to obtain the main and secondary clients.
   * @param  ldm_client_id_e_type - pointer to main and secondary clients.
   * @return  boolean - Indicates if concurrent state or not. TRUE == concurrent, FALSE == not concurrent            
   */
  static boolean queryMainAndSecondaryClients(ldm_client_id_e_type* ep_main_client_id,
										  ldm_client_id_e_type* ep_secondary_client_id);
  
  /**
   * Function to print the client info list maintained by LDM. 
  */
  static void printList(void);

  /**
   * Destructor
   */
  ~LdmClientList(){};
};

#endif // _LDM_CLIENT_H