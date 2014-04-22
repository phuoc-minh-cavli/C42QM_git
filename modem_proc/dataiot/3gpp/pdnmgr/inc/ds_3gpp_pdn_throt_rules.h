#ifndef DS_3GPP_PDN_THROT_RULES_H
#define DS_3GPP_PDN_THROT_RULES_H

/*===========================================================================
                      DS_PDN_THROT_RULES.H
DESCRIPTION
This file exposes PDN throttling rules APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_3gpp_pdn_throt_rules.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"
#include "ds_eps_pdn_manager.h"

/*===========================================================================

                         EXTERNAL TYPE DECLRATIONS

===========================================================================*/

/*-------------------------------------------------------------------------
  This enumeration type defines all possible PDN Throttling Rules.
  Each PDN Throttle Module shall register 2 function pointers corresponding 
  to its  Throttle Rule
  The function pointers shall handle:
  1. Conditions under which the Rule Passes
  2. Action to be taken if the rule passes.
 
  NOTE: Common Rule should always be the first rule in the list.
  Use the common rule to define behavior that is common across all
  throttling modules
------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_THROT_RULE_MIN = -1,  
  DS_PDN_THROT_RULE_COMMON = 0, 
  DS_PDN_THROT_RULE_SPEC,
  DS_PDN_THROT_RULE_DATA_RETRY,
  DS_PDN_THROT_RULE_GLOBAL_THROT,
  DS_PDN_THROT_RULE_CC_SP,
  DS_PDN_THROT_RULE_DAM,
  DS_PDN_THROT_RULE_RPM,
  DS_PDN_THROT_RULE_MAX
} ds_pdn_throt_rule_e_type;

/*------------------------------------------------------------------------
  Clients shall register a function which checks the conditions under which
  the PDN Throttle Rule Passes and  the action that needs to be taken for
  each rule. The below function pointer will point to that
  function
-------------------------------------------------------------------------*/
typedef boolean (*ds_pdn_throt_rules_f_type)
(
  ds_pdn_mgr_s                  *pdn_cntx_p,
  ds_bearer_deact_params_s  *mode_info_p,
 	sys_sys_mode_e_type            sys_mode,
  ds_net_down_reason_type        down_reason
);

/*---------------------------------------------------------------------------
 
  A table of rules, conditions under which the rules will pass and the action
  that needs to be taken for each rule.
----------------------------------------------------------------------------*/
typedef struct
{  
  ds_pdn_throt_rules_f_type    condition_f;
  ds_pdn_throt_rules_f_type    action_f;
} ds_pdn_throt_rule_s;


/*===========================================================================

                      EXTERNAL FUNCTION DECLRATIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_PDN_THROT_RULES_REGISTER

DESCRIPTION
  This function is used by various PDN THrottling modules to register
  the conditions under which a rule can pass and the action
  that needs to be taken if the rule passes.

PARAMETERS 
  rule_id     - Rule id
  rule_p      - Pointer to the structure holding the rule condition and
                rule action

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_throt_rule_register
(
	ds_pdn_throt_rule_e_type  rule_id,
 ds_pdn_throt_rule_s      *rule_p
);

/*===========================================================================
FUNCTION DS_PDN_THROT_RULES_REGISTER

DESCRIPTION
  This function will go through the list of PDN Throttle rules and execute
  the rule for which the rule condition gets satisfied. 

PARAMETERS 
  rejected_pdn_cntx_p - PDN Context Pointer
  mode_info_p         - Mode Info passed by CM
  sys_mode            - Sys Mode for which Throttling needs to be performed
  ps_down_reason      - Down Reason

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_throt_rule_notify
(
  ds_pdn_mgr_s                    *rejected_pdn_cntx_p,
  ds_bearer_deact_params_s    *mode_info_p,
  sys_sys_mode_e_type              sys_mode,
  ds_net_down_reason_type          down_reason
);

#endif /* DS_3GPP_PDN_THROT_RULES_H */
