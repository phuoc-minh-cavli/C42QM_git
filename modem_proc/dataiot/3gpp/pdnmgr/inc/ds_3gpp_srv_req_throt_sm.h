#ifndef DS_3GPP_SRV_REQ_THROT_SM_H
#define DS_3GPP_SRV_REQ_THROT_SM_H

/*===========================================================================
                      DS_3GPP_SRV_REQ_THROT_SM.C
DESCRIPTION
This file defines service request throttle SM APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_3gpp_srv_req_throt_sm.h#2 $
  $DateTime: 2020/04/22 03:56:17 $$Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd     First checkin
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"
#include "ds_3gpp_throttle_sm.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_3GPP_THROT_MAX_LOWER_LAYER_CNT 5

/*------------------------------------------------------------------------------
  This is the structure to be used to implement the system wide srv req throt 
  state machine. This structure holds an array of pointers to the core throt sm.

  We will instantiate only one object of this type. However, inside this object
  we have an array of 8 pointers to core state machines. This means we will hold
  throttling state of 8 PLMNS simultaneously. 

  If all throt sm's are in use and there is a PLMN
  reselection, we will not be able to throttle on the PLMN that the UE 
  reselected to.
------------------------------------------------------------------------------*/
typedef struct
{
  ds_3gppi_throttle_sm_s *sm_ptr[DS_3GPP_MAX_PLMN_SUPPORTED]; 
                                       // array of pointers to the core throt sm
  uint8                  free_count;   // counter to hold the number of free 
                                       // core sm
} ds_3gpp_srv_req_throt_sm_s;

/*------------------------------------------------------------------------------
This enum specifies the possible values of the NV DS_3GPP_CFG_AUTH_TYPE_FALLBACK. 
------------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN = 0, /* Service req throttling with PLMN blocking */
  DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN_PREF_T3402, /* Service request throttling with PLMN blocking for T3402 duration */
  DS_3GPP_SRV_REQ_THROT_NO_PLMN_BLOCK, /* Service req throt with no plmn blocking */
  DS_3GPP_NO_SRV_REQ_THROT_NO_BLOCK_PLMN,/* No service req throt and no plmn blocking*/
  DS_3GPP_NO_SRV_REQ_THROT_MAX
} ds_3gpp_srv_req_throt_behavior_e;
 
/*-------------------------------------------------------------------------- 
  Enum to represent various throttling actions
  --------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_SRV_REQ_THROT_NO_ACTION = 0,
  DS_3GPP_SRV_REQ_THROT_DISABLE_LTE = 1,
  DS_3GPP_SRV_REQ_THROT_DISABLE_LTE_PER_PLMN = 2,
  DS_3GPP_SRV_REQ_THROT_ENABLE_LTE = 3,
  DS_3GPP_SRV_REQ_THROT_ENABLE_LTE_PER_PLMN = 4,
} ds_3gpp_srv_req_throt_action_e_type;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION DS_3GPP_SRV_REQ_THROT_INIT

DESCRIPTION
  This function initializes Service Request Throttle SM.

PARAMETERS  
  None

DEPENDENCIES 
  None.

RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_srv_req_throt_init
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_SRV_REQ_GET_THROT_SM

DESCRIPTION
  This function will be invoked by clients to get a handle to the singleton
  object that will be maintained for system wide throttling of service reqs.
  
PARAMETERS  
  create_if_not_present - This flag is used to determine if we have to create
                          a core sm if the wrapper sm does not have one for
                          the current PLMN
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  void* - A non-null void pointer will be given to clients. This is an opaque
          handle for the clients

  NULL  - If the singleton object for system wide throttling of service req was
          not created during power up. 

SIDE EFFECTS 
  None.
  
===========================================================================*/
void* ds_3gpp_srv_req_get_throt_sm
(
  boolean                create_if_not_present
);

/*===========================================================================
FUNCTION DS_3GPP_SRV_REQ_THROT_SM_FEED_FAILURE

DESCRIPTION
  This function feeds Service reject into the throttlig machine. This func
  takes care of figuring out if the number of consecutive failures has gone
  up and uses the core sm to apply appropriate back-off

PARAMETERS  
  void*         - Pointer to the throttling SM - opaque handle from client

DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_srv_req_throt_feed_failure
(
  void*                       throt_sm_ptr,
  ds_3gpp_throt_rab_info_s    *rab_throt_info
);

/*===========================================================================
FUNCTION DS_3GPP_SRV_REQ_THROT_RESET

DESCRIPTION
  This function is used to reset to state machine. This function internally
  cleans up the core state machine.

PARAMETERS  
  void*         - Pointer to the throttling SM - opaque handle from client
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_srv_req_throt_reset
(
  void*                   throt_sm_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_SRV_REQ_THROT_GET_IS_THROTTLED_FLAG

DESCRIPTION
  This function examines the presence of a core SM. The abscence of a core
  SM means no throttling is in place. If the core sm is present, then we
  query the core sm about the throttling state.
  
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine

DEPENDENCIES 
  None.

RETURN VALUE 
  TRUE  - If the SM is in throttled state
  FALSE - If the SM is not in throttled state
   
SIDE EFFECTS 
  None.

===========================================================================*/
boolean ds_3gpp_srv_req_throt_get_is_throttled_flag
(
  void*                   throt_sm_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_SRV_REQ_THROT_GO_NULL

DESCRIPTION
  This function is used to clean up the service request throttling state machine.
  This function internally cleans up the core state machine.

PARAMETERS  
 enable_plmn
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_srv_req_throt_go_null
(
  boolean                enable_plmn
);

/*===========================================================================
FUNCTION DS_3GPP_SRV_REQ_THROT_GET_BACK_OFF_TIMER_VAL

DESCRIPTION
  This function is used to get the time for which throttling will happen.
  The client can use this any way it wants.

PARAMETERS  
  void*         - Pointer to the throttling SM - opaque handle from client
  uint32*       - Pointer to the timer_val - OUT param 
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If all went fine
  FALSE - If anything fails.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_srv_req_throt_get_back_off_timer_val
(
  void*                  throt_sm_ptr, 
  uint32*                timer_val_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_SRV_REQ_THROT_SM_GET_STATE

DESCRIPTION
  This function is used to get the state of the throttling sm

PARAMETERS  
  throt_sm_ptr         - Pointer to the throttling SM - opaque handle from client
  throt_state_ptr      - Pointer to the throt_state - OUT param 
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If all went fine
  FALSE - If anything fails.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_srv_req_throt_get_state
(
  void                    *throt_sm_ptr, 
  boolean                 *throt_state_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_SRV_REQ_THROT_T3402_IS_ENABLED

DESCRIPTION
  This function checks if T3402 throttle is enabled.

PARAMETERS  
  CMD Ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean TRUE :  T3402 enabled
          FALSE : T3402 disabled
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_srv_req_throt_t3402_is_enabled
(
  void
);

/*===========================================================================
FUNCTION  DS_3GPP_SRV_REQ_THROT_GET_BEHAVIOR

DESCRIPTION
  This function returns the 3GPP srv req throttle behavior

PARAMETERS
   None
 
DEPENDENCIES
  None.

RETURN VALUE 
   ds_3gpp_srv_req_throttle_behavior - 3GPP srv req throttle behavior

SIDE EFFECTS
  None.

===========================================================================*/
ds_3gpp_srv_req_throt_behavior_e ds_3gpp_srv_req_throt_get_behavior
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_SRV_REQ_THROT_VALIDATE_MIN_FAILURE_COUNT

DESCRIPTION
  This function used to set the min failure count to given value if it is less 
 than that

PARAMETERS  
  void*         - Pointer to the throttling SM - opaque handle from client 
  uint16 fail_count
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/

void ds_3gpp_srv_req_throt_validate_min_failure_count
(
  void                     *throt_sm_ptr,  
  uint16                    fail_count 
);

/*===========================================================================
FUNCTION      DS3GI_T3346_INFO_IND_HDLR

DESCRIPTION   When T3346 timer expires and data retry is enabled clear 
              service request throttling 

DEPENDENCIES  None

RETURN VALUE  If found, returns the call instance index.
              If not found, then returns DS3GI_INVALID_CALL INSTANCE

SIDE EFFECTS  None
===========================================================================*/
boolean ds_3gpp_srv_throt_t3346_info_ind_hdlr
(
  msgr_umid_type             msgr_type,
  const msgr_hdr_struct_type *dsmsg_ptr
);

/*===========================================================================
FUNCTION ds_3gpp_srv_req_throt_get_core_sm_for_current_plmn
DESCRIPTION
  This function gets core SM ptr for the current PLMN
  
PARAMETERS  

    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Core SM pointer
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
ds_3gppi_throttle_sm_s* ds_3gpp_srv_req_throt_get_core_sm_for_current_plmn();

#ifdef TEST_FRAMEWORK
#error code not present
#endif


#endif /*DS_3GPP_SRV_REQ_THROT_SM_H*/
