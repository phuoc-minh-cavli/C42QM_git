#ifndef _POLICYMAN_POLICY_H_
#define _POLICYMAN_POLICY_H_

/**
  @file policyman_policy.h

  @brief Interface to policies.
*/

/*
    Copyright (c) 2013,2014 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/inc/policyman_policy.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "mre.h"
#include "policyman_rules.h"
#include "policyman_set.h"
#include "policyman_state.h"

#include "policyman_i.h"


typedef enum
{
  POLICYMAN_POLICY_ID_GENERIC = 1,
  POLICYMAN_POLICY_ID_SGLTE,
} policyman_policy_id_t;



typedef enum
{
  POLICYMAN_POLICY_ERROR_NONE = 0,                    /** MRE Status  */

  POLICYMAN_POLICY_ERROR_FEATURE_MISMATCH,            /**< FEATURE mismatch */
} policyman_policy_error_cause;



/*-----------------------------------------------------------------------------
  Defines for policy preconditions.
-----------------------------------------------------------------------------*/

#define POLICYMAN_PRECOND_NONE        0x00000000  /* Only for initialization, not testing */
#define POLICYMAN_PRECOND_IMSI        0x00000001  /* Policyman has the IMSI from UIM */
#define POLICYMAN_PRECOND_SS          0x00000002  /* Policyman has received an SS event from CM */
#define POLICYMAN_PRECOND_SIMTYPE     0x00000004  /* SIM type is known*/
#define POLICYMAN_PRECOND_CDMASIMTYPE 0x00000008  /* CSIM type is known*/
#define POLICYMAN_PRECOND_LOCATION    0x00000010  /* UE location is known*/


typedef size_t  policyman_policy_enum_ctx;



/*-------- policyman_policy_vtbl --------*/
/**
@brief  Dispatch table for a policy.

Each policy has this dispatch table, which contains pointers to functions for
actions on the policy.  A pointer to this table is used in all policy-related
code.

Only the init() and deinit() functions need to be defined for each policy -
the others are defined by policy manager.
*/
typedef struct
{
  policyman_policy_t* (*newInstance)(char const *pName);
                          /**< Return a new instance of this policy if the name passed is one of its names */
  void                (*initStatus)(policyman_policy_t *pPolicy, policyman_status_t status);
                          /**< Function to handle status of initialization of policy */
  policyman_status_t  (*configure)(policyman_policy_t *pPolicy, policy_execute_ctx_t *pCtx);
                          /**< Function that tells policy to do its configuration */
  boolean             (*execute)(policyman_policy_t *pPolicy, policy_execute_ctx_t *pCtx);
                          /**< Entry point to policy for non-rules-based policies */
  void                (*notifyService)(policyman_policy_t *pPolicy, policy_execute_ctx_t *pCtx);
                          /**< Entry point to notify policy of a service change */
  void                (*handleUserPrefChange)(policyman_policy_t *pPolicy, policy_execute_ctx_t *pCtx);
                          /**< Entry point to notify policy of a user preference change */
  void                (*handleUimUpdate)(policyman_policy_t *pPolicy, policy_execute_ctx_t *pCtx);
                         /**< Entry point to notify policy of a UIM updates related to subscription availability */
} policyman_policy_vtbl_t;



typedef uint32  policy_attributes_t;

#define POLICY_ATTRIBUTE_NONE               0x00000000
#define POLICY_ATTRIBUTE_ENABLED            0x00000001


#define POLICYMAN_POLICY_ERROR_CAUSE(p)  (p->error_cause)


/*-------- POLICYMAN_POLICY_BASE --------*/
/**
@brief  Base structure for a policy.

This macro must be the first element of any policy structure; private elements
specific to a given policy should be added after this.
*/


#define POLICYMAN_POLICY_BASE                   \
  MRE_POLICY_BASE;                              \
  policyman_policy_vtbl_t       *pVtbl;         \
  size_t                        schema_ver;     \
  policyman_policy_id_t         id;             \
  policyman_policy_error_cause  error_cause;    \
  sys_modem_as_id_e_type        subs;           \
  boolean                       immediate_full_rat


struct policyman_policy_t
{
  POLICYMAN_POLICY_BASE;
};


#define POLICY_IS_ENABLED(pPolicy)                                            \
          mre_policy_is_enabled((mre_policy_t *) pPolicy)

#define POLICY_VTBL(pPolicy)  ((policyman_policy_vtbl_t *)((pPolicy)->pVtbl))

#define POLICY_IS_POLICY(pPolicy, pName)                                      \
          POLICY_VTBL(pPolicy)->isPolicy(pPolicy, pName)

#define POLICY_INIT_STATUS(pPolicy, status)                                   \
          POLICY_VTBL(pPolicy)->initStatus(pPolicy, status)

#define POLICY_CONFIGURE(pPolicy, pCtx)                           \
          POLICY_VTBL(pPolicy)->configure(pPolicy, pCtx)

#define POLICY_EXECUTE(pPolicy, pCtx)                             \
          POLICY_VTBL(pPolicy)->execute(pPolicy, pCtx)

#define POLICY_NOTIFY_SERVICE(pPolicy, pCtx)                                  \
          POLICY_VTBL(pPolicy)->notifyService(pPolicy, pCtx)

#define POLICY_HANDLE_USERPREF_UPDATE(pPolicy, pCtx)                          \
          POLICY_VTBL(pPolicy)->handleUserPrefChange(pPolicy, pCtx)

#define POLICY_HANDLE_UIM_UPDATE(pPolicy, pCtx)                               \
          POLICY_VTBL(pPolicy)->handleUimUpdate(pPolicy, pCtx)

#define POLICY_SCHEMA_VERSION(pPolicy)                                        \
          ((pPolicy)->schema_ver)

#define POLICY_CAN_EXECUTE(pPolicy)                                           \
          (POLICY_VTBL(pPolicy)->execute != NULL)

#define POLICY_CAN_NOTIFY_SERVICE(pPolicy)                                    \
          (POLICY_VTBL(pPolicy)->notifyService != NULL)

#define POLICY_CAN_HANDLE_USERPREF_UPDATE(pPolicy)                            \
          (POLICY_VTBL(pPolicy)->handleUserPrefChange != NULL)

#define POLICY_CAN_HANDLE_UIM_UPDATE(pPolicy)                                 \
          (POLICY_VTBL(pPolicy)->handleUimUpdate != NULL)


#define POLICY_SUBS(pPolicy)  (((policyman_policy_t *)(pPolicy))->subs)
#define ACTION_SUBS(pAction)  (POLICY_SUBS((pAction)->pPolicy))
#define CONDITION_SUBS(pCond) (POLICY_SUBS((pCond)->pPolicy))


/*-------- policyman_get_namespace_info --------*/
mre_namespace_info_t  *policyman_get_namespace_info(
  void
);


/*-------- policyman_policy_dtor --------*/
/**
@brief  Destructor for a base policy, called from destructors of derivied policies.
*/
void
policyman_policy_dtor(
  void  *pObj
  );


/*-------- policyman_policy_init --------*/
/**
@brief  Initialize the policy subsystem.  Private function.

@return
  None.
*/
void policyman_policy_init(
  policyman_state_t *pState
);


/*-------- policyman_policy_per_subs_init --------*/
/**
@brief  Initialize the policy files.  Private function.

@return
  None.
*/
void  policyman_policy_per_subs_init(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
);


/*-------- policyman_policy_init_succeeded --------*/
/**
@brief  Return whether the initialization of the policy succeeded.
        This is used as in unit test in order to determine if
        the XML was successfully parsed into a policy.
*/
boolean
policyman_policy_init_succeeded(
  void
  );


/*-------- policyman_policy_deinit --------*/
/**
@brief  Deinitialize the policy subsystem.  Private function.

@return
  None
*/
void
policyman_policy_deinit(
  void
  );


/*-------- policyman_policy_per_subs_deinit --------*/
/**
@brief  Deinitialize the policy subsystem for a specific 
        subscription.  Private function.

@return
  None
*/
void policyman_policy_per_subs_deinit(
  sys_modem_as_id_e_type subsId
);


/*-------- policyman_policy_run_policy_check --------*/
/**
@brief  Run a policy check and collect updates into the itemset passed.

@param[in]  pState    Pointer to current state.
@param[in]  asubs_id  The subscription on which to run the policy check.
@param[in]  pItemSet  The itemset to which to add items.

@return

*/
void
policyman_policy_run_policy_check(
  policyman_state_t       *pState,
  sys_modem_as_id_e_type  asubs_id,
  policyman_set_t         *pItemSet
  );


/*-------- policyman_policy_notify_service --------*/
/**
@brief  A serving system update has occurred.  Notify policies of service
        state.

@param

@return

*/
void
policyman_policy_notify_service(
  boolean                haveService,
  sys_modem_as_id_e_type asubs_id
  );


/*-------- policyman_policy_get_name --------*/
/**
@brief  Get the name of a policy.

@param[in]  pMe   Pointer to a policy.

@return
  A pointer to the name of the given policy.
*/
#define policyman_policy_get_name(p)  ((policyman_policy_t *) (p))->pName


/*-------- policyman_policy_get_id --------*/
/**
@brief

@param

@return

*/
#define policyman_policy_get_id(p)    ((policyman_policy_t *) (p))->id


/*-------- policyman_policy_handle_user_pref_update --------*/
/**
@brief  Get policies to handle phone update for user preference change

@param[in]  pState Pointer to a policyman subscription state.

@return
  None
*/
void
policyman_policy_handle_user_pref_update(
  sys_modem_as_id_e_type asubs_id
);


/*-------- policyman_policy_handle_uim_update --------*/
/**
@brief  Get policies to handle UIM updates

@param[in]  pState Pointer to a policyman state.

@return
  None
*/
void
policyman_policy_handle_uim_update(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
);


/*-------- policyman_policy_is_suspended --------*/
/**
@brief  Check if Policy Manager is in a suspended state.

@return
  TRUE if state is suspended, FALSE otherwise
*/
boolean
policyman_policy_is_suspended(
  void
 );


/*-------- policyman_policy_file_enum_init --------*/
void
policyman_policy_file_enum_init(
  policyman_policy_enum_ctx *pCtx
  );


/*-------- policyman_policy_file_enum_next --------*/
boolean policyman_policy_file_enum_next(
  policyman_policy_enum_ctx *pCtx,
  char                      *pFileName,
  size_t                     nameLength
);


/*-------- policyman_policy_are_preconditions_met --------*/
/**
@brief  Returns whether the policy currently being evaluated has its
        preconditions met.
*/
boolean
policyman_policy_are_preconditions_met(
  policyman_policy_t  *pPolicy,
  policyman_state_t   *pState
  );


/*-------- policyman_policy_check_for_immediate_full_rat --------*/
void  policyman_policy_check_for_immediate_full_rat(
  policyman_policy_t  *pPolicy
);

/*-------- policyman_policy_execute --------*/
void policyman_policy_execute(
  policyman_policy_t    *pPolicy,
  policy_execute_ctx_t  *pCtx
);


/*-------- policyman_evaluate_cond --------*/
/**
@brief  Evaluates a condition with a subs attribute.
*/
boolean policyman_evaluate_cond(
  policyman_condition_t const *pCond,
  void                        *pCtx,
  pfnEvaluateCondForSubs       pfnEvaluate
);


/*-------- policyman_execute_action --------*/
/**
@brief  Executes an action with a subs attribute.
*/
boolean policyman_execute_action(
  policyman_action_t const *pAction,
  void                     *pCtx,
  pfnExecuteActionForSubs   pfnExecute
);


#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

#endif /* _POLICYMAN_POLICY_H_ */
