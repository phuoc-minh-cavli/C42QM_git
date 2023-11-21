/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   L O C K   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock functions.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2016, 2021 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/simlock/src/simlock.c#2 $$ $DateTime: 2021/07/07 00:29:01 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/07/21   tq      Enabled support for SIMLOCK
12/05/16   bcho    Postpone creation of simlock config files
09/28/16   sn      Added support to retrieve retry attempts left for locking
09/28/16   sn      Send control key presence in get config status
09/08/16   vdc     Extend full and expired service policies on any slot
05/30/16   bcho    Introduce support for SimLock fuse
05/26/16   vdc     Skip checking simlock policies when no category is enabled
05/25/16   vdc     Remove F3 messages for memory allocation failure
01/18/16   stv     Remote simlock support
08/19/15   stv     Re-locate the public key file
07/09/15   bcho    Support for whitelist and blacklist codes in same category
07/09/15   bcho    Redundant checks for SIMLOCK
07/03/15   stv     Unlock/Relock fail when locked with zero max retries 
05/15/15   vv      Support for RSU set configuration in simlock
05/15/15   vv      Support for RSU in simlock
04/17/15   at      Update error code when max retries are reached
03/19/15   at      Introducing new relock feature in SIMLock
03/11/15   vv      Added support for new perso status
02/17/15   vv      Indicate the reason for invalid subscription
12/17/14   vv      Relocate the simlock data files
11/10/14   vv      Add support for setting emergency mode for all the RATs
10/28/14   vv      Update the simlock init status in globals
10/03/14   vv      Do not allow temporary unlock if the lock is not set
09/29/14   vv      Added support for the remote SFS
09/29/14   vv      Added support for emergency only mode
09/18/14   tl      Change logic when determining if is card valid
09/17/14   tl      Add support for perso and deperso secure
08/29/14   vv      Added support for temporary unlock
08/27/14   tl      Introduce network related SIM Lock slot policies
08/25/14   tl      Introduce master slot 1 slot policy
06/20/14   vv      Added support for diag protection
04/15/14   vv      Added support for quad SIM
04/14/14   vv      Added support for identifying conflicting codes
03/25/14   tl      Secondary revision
02/14/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlock.h"
#include "simlock_config.h"
#include "simlock_3gpp.h"
#include "simlock_3gpp2.h"
#include "simlock_crypto.h"
#include "simlock_file.h"
#include "simlock_util.h"
#include "simlock_category.h"
#include "simlock_category_conflicts.h"
#include "simlock_timer.h"
#include "simlock_cm.h"
#include "simlock_rsu_key.h"
#include "simlock_temp_unlock.h"
#include "simlock_time.h"
#include "simlock_validity.h"
#include "simlock_fuse.h"
#include "amssassert.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_CARD_INVALID_REASON_TYPE

   DESCRIPTION:
     This enum specifies the reason for considering a card as invalid
-----------------------------------------------------------------------------*/
typedef enum
{
  SIMLOCK_CARD_INVALID_REASON_NONE,
  SIMLOCK_CARD_INVALID_REASON_NO_SUBSCRIPTION,
  SIMLOCK_CARD_INVALID_REASON_INVALID_SUBSCRIPTION,
} simlock_card_invalid_reason_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_IS_CARD_VALID

DESCRIPTION
  This function checks if a card on a slot is valid. All subscriptions on card
  must be valid for the card to be considered valid

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE   : If the card is valid
  FALSE  : Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
static boolean simlock_is_card_valid
(
  const simlock_run_algorithm_msg_req_type   * req_msg_ptr,
  simlock_run_algorithm_msg_resp_type        * result_msg_ptr,
  simlock_slot_enum_type                       slot,
  simlock_card_invalid_reason_type           * invalid_reason_ptr
)
{
  uint8       num_sub     = 0;
  uint8       slot_index  = 0;

  if((req_msg_ptr == NULL) ||
     (result_msg_ptr == NULL) ||
     (invalid_reason_ptr == NULL))
  {
    return FALSE;
  }

  if(simlock_util_get_slot_index(slot, &slot_index) !=
                     SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("failed to get slot index for slot, 0x%x", slot);
    return FALSE;
  }

  if(slot_index >= SIMLOCK_SLOT_COUNT_MAX)
  {
    SIMLOCK_MSG_ERR_1("invalid slot index, 0x%x", slot_index);
    return FALSE;
  }

  /* Cards with no valid subscription should not be considered valid */
  if(req_msg_ptr->card_data[slot_index].num_subscriptions == 0 ||
     req_msg_ptr->card_data[slot_index].num_subscriptions > SIMLOCK_SUBS_COUNT_MAX)
  {
    *invalid_reason_ptr = SIMLOCK_CARD_INVALID_REASON_NO_SUBSCRIPTION;
    return FALSE;
  }

  /* All subscriptions on card must be valid for the card to be considered valid */
  for(num_sub = 0; num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions; num_sub++)
  {
    if(!result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid)
    {
      *invalid_reason_ptr = SIMLOCK_CARD_INVALID_REASON_INVALID_SUBSCRIPTION;
      return FALSE;
    }
  }

  return TRUE;
} /* simlock_is_card_valid */


/*===========================================================================
FUNCTION SIMLOCK_APPLY_SUB_POLICY

DESCRIPTION
  This function updates the validity of a subscription on a card, based on
  the subscription policy set in the SIMLock feature.

  If the sub policy is set to one of the following, then the card is
  considered as valid only if the respective subscription(s) is/are present
  and valid on the card.
  1. Valid 3GPP required
  2. Valid 3GPP2 required
  3. Valid 3GPP and 3GPP2 required

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_apply_sub_policy
(
  const simlock_run_algorithm_msg_req_type   * req_msg_ptr,
  simlock_run_algorithm_msg_resp_type        * result_msg_ptr,
  simlock_subscription_policy_enum_type        sub_policy,
  simlock_slot_enum_type                       slot
)
{
  uint8                                       num_sub              = 0;
  uint8                                       slot_index           = 0;
  boolean                                     is_card_valid        = TRUE;
  boolean                                     is_3gpp_app_present  = FALSE;
  boolean                                     is_3gpp2_app_present = FALSE;
  simlock_subscription_invalid_reason_type    failure_reason       = SIMLOCK_SUBSCRIPTION_INVALID_REASON_CONFIG_POLICY;

  if((req_msg_ptr == NULL) ||
     (result_msg_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_apply_sub_policy");

  if(simlock_util_get_slot_index(slot, &slot_index) !=
            SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("failed to get slot index for slot, 0x%x", slot);
    return SIMLOCK_GENERIC_ERROR;
  }

  if(slot_index >= SIMLOCK_SLOT_COUNT_MAX)
  {
    SIMLOCK_MSG_ERR_1("invalid slot index, 0x%x", slot_index);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* apply the subscription policy */
  for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                    num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
  {
    if((sub_policy == SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP_REQUIRED) ||
       (sub_policy == SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP_AND_3GPP2_REQUIRED))
    {
      if(req_msg_ptr->card_data[slot_index].subscription_data[num_sub].sub_type ==
           SIMLOCK_SUBSCRIPTION_3GPP)
      {
        is_3gpp_app_present = TRUE;
        if(!result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid)
        {
          failure_reason = SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_REQUIRED_BUT_LOCKED;
          is_card_valid  = FALSE;
          break;
        }
      }
    }
    else if ((sub_policy == SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP2_REQUIRED) ||
             (sub_policy == SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP_AND_3GPP2_REQUIRED))
    {
      if(req_msg_ptr->card_data[slot_index].subscription_data[num_sub].sub_type ==
           SIMLOCK_SUBSCRIPTION_3GPP2)
      {
        is_3gpp2_app_present = TRUE;
        if(!result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid)
        {
          failure_reason = SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_REQUIRED_BUT_LOCKED;
          is_card_valid = FALSE;
          break;
        }
      }
    }
    else
    {
      /* Nothing to do */
    }
  }

  if(is_card_valid)
  {
    switch(sub_policy)
    {
      case SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP_REQUIRED:
        /* if the 3gpp app presence is not known,
        mark card as not valid */
        if(!is_3gpp_app_present)
        {
          failure_reason = SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_REQUIRED_BUT_NOT_PRESENT;
          is_card_valid = FALSE;
        }
        break;

      case SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP2_REQUIRED:
        /* if the 3gpp2 app presence is not known,
         mark card as not valid */
        if(!is_3gpp2_app_present)
        {
          failure_reason = SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_REQUIRED_BUT_NOT_PRESENT;
          is_card_valid = FALSE;
        }
        break;

      case SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP_AND_3GPP2_REQUIRED:
        /* if the 3gpp/3gpp2 app presence is not known,
         mark card as not valid */
        if(!is_3gpp_app_present)
        {
          failure_reason = SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_REQUIRED_BUT_NOT_PRESENT;
          is_card_valid = FALSE;
        }
        else if(!is_3gpp2_app_present)
        {
          failure_reason = SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_REQUIRED_BUT_NOT_PRESENT;
          is_card_valid = FALSE;
        }
        break;

      default:
         break;
    }
  }

  if(req_msg_ptr->card_data[slot_index].num_subscriptions)
  {
    SIMLOCK_MSG_MED_2("is card valid, 0x%x on slot: 0x%x",
                      is_card_valid, slot);
  }

  /* if the card is valid, it means the policy check
     is OK. So need to update the card validity status */
  if(is_card_valid)
  {
    return SIMLOCK_SUCCESS;
  }

  /* Update the validity of the subscriptions on a slot based on the
     subscription policy */
  for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                    num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
  {
    if(sub_policy == SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP_REQUIRED)
    {
      if(req_msg_ptr->card_data[slot_index].subscription_data[num_sub].sub_type ==
            SIMLOCK_SUBSCRIPTION_3GPP2)
      {
        result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
          FALSE;
        if(result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason ==
             SIMLOCK_SUBSCRIPTION_INVALID_REASON_NONE)
        {
          result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason =
            failure_reason;
        }
      }
    }
    else if(sub_policy == SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP2_REQUIRED)
    {
      if(req_msg_ptr->card_data[slot_index].subscription_data[num_sub].sub_type ==
            SIMLOCK_SUBSCRIPTION_3GPP)
      {
        result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
          FALSE;
        if(result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason ==
             SIMLOCK_SUBSCRIPTION_INVALID_REASON_NONE)
        {
          result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason =
            failure_reason;
        }
      }
    }
    else if(sub_policy == SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP_AND_3GPP2_REQUIRED)
    {
      result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
        FALSE;
      if(result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason ==
             SIMLOCK_SUBSCRIPTION_INVALID_REASON_NONE)
      {
        result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason =
          failure_reason;
      }
    }
  }

  return SIMLOCK_SUCCESS;
} /* simlock_apply_sub_policy */


/*===========================================================================
FUNCTION SIMLOCK_APPLY_CONFIG_POLICY

DESCRIPTION
  This function updates the validity of a subscription on a card, based on
  the slot and the subscription policy set in the SIMLock feature.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_apply_config_policy
(
  const simlock_run_algorithm_msg_req_type   * req_msg_ptr,
  simlock_config_data_type                   * config_data_ptr,
  simlock_run_algorithm_msg_resp_type        * result_msg_ptr
)
{
  simlock_result_enum_type               simlock_status = SIMLOCK_SUCCESS;
  simlock_slot_policy_enum_type          slot_policy    = SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS;
  simlock_subscription_policy_enum_type  sub_policy     = SIMLOCK_SUBSCRIPTION_POLICY_INDEPENDENT;
  simlock_slot_enum_type                 slot           = SIMLOCK_SLOT_1;
  simlock_card_invalid_reason_type       invalid_reason = SIMLOCK_CARD_INVALID_REASON_NONE;
  uint8                                  slot_index     = 0;
  uint8                                  sub_index      = 0;
  uint8                                  num_sub        = 0;
  boolean                                is_card_valid  = TRUE;

  if((req_msg_ptr == NULL)     ||
     (config_data_ptr == NULL) ||
     (result_msg_ptr == NULL))
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  SIMLOCK_MSG_MED_0("simlock_apply_config_policy");

  slot_policy = config_data_ptr->slot_policy;
  sub_policy  = config_data_ptr->sub_policy;

  /* first apply the sub policy to subcriptions on all supported slots */
  for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
  {
    if(simlock_apply_sub_policy(req_msg_ptr, result_msg_ptr, sub_policy, slot) !=
                  SIMLOCK_SUCCESS)
    {
      return SIMLOCK_GENERIC_ERROR;
    }
  }

  /* now apply the slot policy */
  switch(slot_policy)
  {
    case SIMLOCK_SLOT_POLICY_LOCK_ONLY_SLOT_1:
      for(slot = SIMLOCK_SLOT_2; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        simlock_status = simlock_util_get_slot_index(slot, &slot_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (slot_index < SIMLOCK_SLOT_COUNT_MAX))
        {
          for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                            num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
          {
            result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
              TRUE;
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_VALID_CARD_MUST_ON_SLOT_1:
      /* Unless all the categories on slot-1 are disabled,
         check if there is a valid card on slot-1 */
      if(simlock_util_all_categories_disabled(SIMLOCK_SLOT_1,
                                              config_data_ptr))
      {
        is_card_valid = TRUE;
      }
      else
      {
        is_card_valid = simlock_is_card_valid(req_msg_ptr, result_msg_ptr, SIMLOCK_SLOT_1, &invalid_reason);
      }

      for(slot = SIMLOCK_SLOT_2; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        simlock_status = simlock_util_get_slot_index(slot, &slot_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (slot_index < SIMLOCK_SLOT_COUNT_MAX))
        {
          for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                            num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
          {
            result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
              is_card_valid;
            if(!is_card_valid)
            {
              if(result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason ==
                    SIMLOCK_SUBSCRIPTION_INVALID_REASON_NONE)
              {
                if(invalid_reason == SIMLOCK_CARD_INVALID_REASON_NO_SUBSCRIPTION)
                {
                  result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason =
                      SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_ON_SLOT1_REQUIRED_BUT_NOT_PRESENT;
                }
                else if(invalid_reason == SIMLOCK_CARD_INVALID_REASON_INVALID_SUBSCRIPTION)
                {
                  result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason =
                      SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_ON_SLOT1_REQUIRED_BUT_LOCKED;
                }
              }
            }
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_VALID_CARD_MUST_ON_ANY_SLOT:
      /* check if there is a valid card on any slot */
      for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        is_card_valid = simlock_is_card_valid(req_msg_ptr, result_msg_ptr, slot, &invalid_reason);
        if(is_card_valid)
        {
          break;
        }
      }

      for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        simlock_status = simlock_util_get_slot_index(slot, &slot_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (slot_index < SIMLOCK_SLOT_COUNT_MAX))
        {
          for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                            num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
          {
            result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
              is_card_valid;
            if(!is_card_valid)
            {
              if(result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason ==
                    SIMLOCK_SUBSCRIPTION_INVALID_REASON_NONE)
              {
                if(invalid_reason == SIMLOCK_CARD_INVALID_REASON_INVALID_SUBSCRIPTION)
                {
                  result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason =
                      SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_ON_ANY_SLOT_REQUIRED_BUT_LOCKED;
                }
              }
            }
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_MASTER_SLOT_1:
      if(simlock_util_all_categories_disabled(SIMLOCK_SLOT_1,
                                              config_data_ptr))
      {
        is_card_valid = TRUE;
      }
      else
      {
        is_card_valid = simlock_is_card_valid(req_msg_ptr, result_msg_ptr, SIMLOCK_SLOT_1, &invalid_reason);
      }
      if(is_card_valid)
      {
        for(slot = SIMLOCK_SLOT_2; slot < SIMLOCK_SLOT_NUM; slot++)
        {
          simlock_status = simlock_util_get_slot_index(slot, &slot_index);
          if((simlock_status == SIMLOCK_SUCCESS) &&
             (slot_index < SIMLOCK_SLOT_COUNT_MAX))
          {
            for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                              num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
            {
              result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
                TRUE;
            }
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_BLOCK_ALL_SLOTS_IF_SLOT_1_EXPIRED:
      is_card_valid = TRUE;
      for(sub_index = 0; sub_index < SIMLOCK_SUBS_COUNT_MAX &&
                         sub_index < req_msg_ptr->card_data[0].num_subscriptions; sub_index++)
      {
        if(req_msg_ptr->card_data[0].subscription_data[sub_index].service_status == SIMLOCK_SRV_STATUS_FULL)
        {
          is_card_valid = TRUE;
          break;
        }
        else if(req_msg_ptr->card_data[0].subscription_data[sub_index].service_status == SIMLOCK_SRV_STATUS_EXPIRED)
        {
          is_card_valid = FALSE;
        }
      }

      for(slot = SIMLOCK_SLOT_2; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        simlock_status = simlock_util_get_slot_index(slot, &slot_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (slot_index < SIMLOCK_SLOT_COUNT_MAX))
        {
          for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                            num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
          {
            result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
              is_card_valid;
            if(!is_card_valid)
            {
              if(result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason ==
                    SIMLOCK_SUBSCRIPTION_INVALID_REASON_NONE)
              {
                result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason =
                    SIMLOCK_SUBSCRIPTION_INVALID_REASON_SUB_ON_SLOT1_EXPIRED;
              }
            }
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_WAIT_FOR_FULL_SERVICE_ON_SLOT_1:
      is_card_valid = FALSE;
      for(sub_index = 0; sub_index < SIMLOCK_SUBS_COUNT_MAX &&
                         sub_index < req_msg_ptr->card_data[0].num_subscriptions; sub_index++)
      {
        if(req_msg_ptr->card_data[0].subscription_data[sub_index].service_status == SIMLOCK_SRV_STATUS_FULL)
        {
          is_card_valid = TRUE;
          break;
        }
      }

      for(slot = SIMLOCK_SLOT_2; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        simlock_status = simlock_util_get_slot_index(slot, &slot_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (slot_index < SIMLOCK_SLOT_COUNT_MAX))
        {
          for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                            num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
          {
            result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
              is_card_valid;
            if(!is_card_valid)
            {
              if(result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason ==
                    SIMLOCK_SUBSCRIPTION_INVALID_REASON_NONE)
              {
                result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason =
                    SIMLOCK_SUBSCRIPTION_INVALID_REASON_WAIT_FOR_FULL_SRV_ON_SLOT1;
              }
            }
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_BLOCK_ALL_SLOTS_IF_ALL_VALID_SLOTS_EXPIRED:
      is_card_valid = FALSE;
      for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        if(simlock_is_card_valid(req_msg_ptr, result_msg_ptr, slot, &invalid_reason) &&
           simlock_util_get_slot_index(slot, &slot_index) == SIMLOCK_SUCCESS &&
           slot_index < SIMLOCK_SLOT_COUNT_MAX)
        {
          for(sub_index = 0; sub_index < SIMLOCK_SUBS_COUNT_MAX &&
                             sub_index < req_msg_ptr->card_data[slot_index].num_subscriptions; sub_index++)
          {
            if(req_msg_ptr->card_data[slot_index].subscription_data[sub_index].service_status != SIMLOCK_SRV_STATUS_EXPIRED)
            {
              is_card_valid = TRUE;
              break;
            }
          }

          if(is_card_valid)
          {
            break;
          }
        }
      }

      /* If all valid slots are expired then there is nothing to update 
         for other subscriptions */
      if(!is_card_valid)
      {
        break;
      }

      for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        simlock_status = simlock_util_get_slot_index(slot, &slot_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (slot_index < SIMLOCK_SLOT_COUNT_MAX))
        {
          for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                            num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
          {
            result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
              is_card_valid;
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_WAIT_FOR_FULL_SERVICE_ON_ANY_VALID_SLOT:
      is_card_valid   = FALSE;
      /* Check if there is a valid card on any slot */
      for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        if(simlock_is_card_valid(req_msg_ptr, result_msg_ptr, slot, &invalid_reason) &&
           simlock_util_get_slot_index(slot, &slot_index) == SIMLOCK_SUCCESS         &&
           slot_index < SIMLOCK_SLOT_COUNT_MAX)
        {
          /* Check if there a subscription which has full service that satisfies enabled categories */
          for(sub_index = 0; sub_index < SIMLOCK_SUBS_COUNT_MAX &&
                             sub_index < req_msg_ptr->card_data[slot_index].num_subscriptions; sub_index++)
          {
            if(req_msg_ptr->card_data[slot_index].subscription_data[sub_index].service_status == SIMLOCK_SRV_STATUS_FULL)
            {
              is_card_valid = TRUE;
              break;
            }
          }

          if(is_card_valid)
          {
            break;
          }
        }
      }

      /* Not able to find any subscription with full service. So, nothing to
         update for other subscriptions, but allow only the cards which are
         valid to possibly get full service on it */
      if(!is_card_valid)
      {
        break;
      }

      for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        simlock_status = simlock_util_get_slot_index(slot, &slot_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (slot_index < SIMLOCK_SLOT_COUNT_MAX))
        {
          for(sub_index = 0; sub_index < SIMLOCK_SUBS_COUNT_MAX &&
                             sub_index < req_msg_ptr->card_data[slot_index].num_subscriptions; sub_index++)
          {
            result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid = 
              is_card_valid;
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS:
    case SIMLOCK_SLOT_POLICY_UNIQUE_FOR_EACH_SLOT:
      /* Nothing to do */
      break;

    default:
      return SIMLOCK_GENERIC_ERROR;
  }

  return simlock_status;
} /* simlock_apply_config_policy */


/*===========================================================================
FUNCTION SIMLOCK_PRINT_HCK_DATA

DESCRIPTION
  Prints the entire HCK data buffer

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  VOID

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_print_hck_data
(
  simlock_hck_type  hck_data
)
{
  uint8  i = 0;

  for(i = 0; i < sizeof(simlock_hck_type)-3; i+=4)
  {
    SIMLOCK_MSG_LOW_6("hck[%d-%d]:0x%x, 0x%x, 0x%x, 0x%x",
                      i,
                      i+3,
                      hck_data[i],
                      hck_data[i+1],
                      hck_data[i+2],
                      hck_data[i+3]);
  }
} /* simlock_print_hck_data */


/*===========================================================================
FUNCTION SIMLOCK_INIT

DESCRIPTION
  This is the init function expected to be called during the power-up. If
  config file is not already present, it will create the file.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_init
(
  void
)
{
  simlock_config_file_presence_enum_type config_file_presence 
                                           = SIMLOCK_CONFIG_FILE_PRESENT;

  /* Check if the config file already exists. We allow the SimLock engine to 
     proceed if the file is correct or if the file is missing and the fuse is 
     not blown, which means that SimLock was never enabled before */
  config_file_presence = simlock_config_check_config_file();
  if(SIMLOCK_CONFIG_FILE_CORRUPTED == config_file_presence)
  {
    SIMLOCK_MSG_ERR_0("Config file or dir is corrupted");
    return SIMLOCK_GENERIC_ERROR;
  }
  else if(SIMLOCK_CONFIG_FILE_ABSENT == config_file_presence &&
          simlock_fuse_is_fuse_blown())
  {
    /* Config file is not present. At this point we should not
       continue further with the remaining SimLock init procedures
       if the SimLock fuse is blown.
       Note that the SimLock fuse is blown by SimLock once the lock
       is enabled successfully or if the SimLock config is moved to
       remote FS */
    SIMLOCK_MSG_ERR_0("Device is SimLock locked");
    return SIMLOCK_GENERIC_ERROR;
  }

#ifdef FEATURE_SIMLOCK_RSU
  simlock_time_init();

  simlock_temp_unlock_init();
#endif /* FEATURE_SIMLOCK_RSU */

#if defined(FEATURE_SIMLOCK_RSU) || defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)
  /* read the pbkey file from EFS and store it in the
     SFS. We ignore the return status here */
  (void)simlock_rsu_key_read_and_store_pubkey_file();
#endif /* FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK */

  return SIMLOCK_SUCCESS;
} /* simlock_init */


/*===========================================================================
FUNCTION SIMLOCK_SET_SIMLOCK_POLICY

DESCRIPTION
  This function sets the slot and subscription policy for the SIMLock. It
  checks if the SIMLock is disabled. If yes, it writes the policy to the
  config file located in SFS. If not, it'll return an error.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_set_simlock_policy
(
  const simlock_set_config_policy_msg_req_type   * policy_msg_ptr
)
{
  simlock_result_enum_type    simlock_status = SIMLOCK_SUCCESS;
  simlock_slot_enum_type      slot           = SIMLOCK_SLOT_1;
  simlock_category_enum_type  category       = SIMLOCK_CATEGORY_3GPP_NW;
  boolean                     lock_disabled  = TRUE;
  simlock_config_data_type    config_file_data;

  if(policy_msg_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_set_simlock_policy");

  memset(&config_file_data, 0x00, sizeof(simlock_config_data_type));

  /* read config file data */
  simlock_status = simlock_config_read_config_data(&config_file_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  /* check if simlock is disabled. If not, return error as the
     configuration policies can be set only when the simlock is
     disabled */
  for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM;
      slot++)
  {
    for(category = SIMLOCK_CATEGORY_3GPP_NW; category < SIMLOCK_CATEGORY_COUNT_MAX;
        category++)
    {
      if(simlock_util_check_category_enabled(&config_file_data,
                                             category,
                                             slot))
      {
        SIMLOCK_MSG_ERR_2("simlock is enabled, for category 0x%x on slot 0x%x",
                          category, slot);
        lock_disabled = FALSE;
        break;
      }
    }
    if(!lock_disabled)
    {
      break;
    }
  }

  /* if simlock is not disabled, then policies cannot be set */
  if(!lock_disabled)
  {
    SIMLOCK_MSG_ERR_0("simlock is enabled, cannot set policy");
    return SIMLOCK_INVALID_STATE;
  }

  /* update and write the policy to the config file */
  simlock_status = simlock_config_update_and_write_policies(policy_msg_ptr,
                                                            &config_file_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_0("set policy failed");
  }

  return simlock_status;
} /* simlock_set_simlock_policy */


/*===========================================================================
FUNCTION SIMLOCK_RUN_ALGORITHM

DESCRIPTION
  This is the core function for the SIMLock feature. It validates a SIM
  card present, by verifing the card data against the lock data for all the
  enabled categories. It then applies the configuration policy
  (slot and sub policy), to determine whether a subscription is valid on the
  card.
  If the SIMLock feature is not enabled, all the subcriptions on all the
  cards are considered as valid

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_run_algorithm
(
  const simlock_run_algorithm_msg_req_type   * req_msg_ptr,
  simlock_run_algorithm_msg_resp_type        * resp_msg_ptr
)
{
  simlock_result_enum_type           simlock_status       = SIMLOCK_SUCCESS;
  simlock_slot_enum_type             slot                 = SIMLOCK_SLOT_1;
  uint8                              sub_cnt              = 0;
  uint8                              slot_index           = 0;
  simlock_config_data_type           config_file_data;

  if((req_msg_ptr == NULL)  ||
     (resp_msg_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_run_algorithm");

  memset(&config_file_data, 0x00, sizeof(simlock_config_data_type));

  /* read config file data */
  simlock_status = simlock_config_read_config_data(&config_file_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  /* validate the subscriptions on the available slots */
  for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
  {
    simlock_status = simlock_util_get_slot_index(slot, &slot_index);
    if((simlock_status == SIMLOCK_SUCCESS) &&
       (slot_index < SIMLOCK_SLOT_COUNT_MAX))
    {
      for(sub_cnt = 0;
          (sub_cnt < req_msg_ptr->card_data[slot_index].num_subscriptions &&
           sub_cnt < SIMLOCK_SUBS_COUNT_MAX);
          sub_cnt++)
      {
        if(req_msg_ptr->card_data[slot_index].subscription_data[sub_cnt].sub_type ==
                                                                  SIMLOCK_SUBSCRIPTION_3GPP)
        {
          resp_msg_ptr->card_validity_data[slot_index].num_subscriptions++;
          simlock_status = simlock_3gpp_run_algorithm(&req_msg_ptr->card_data[slot_index].subscription_data[sub_cnt],
                                                      slot,
                                                      &config_file_data,
                                                      &resp_msg_ptr->card_validity_data[slot_index].subscription_data[sub_cnt]);
        }
        else if(req_msg_ptr->card_data[slot_index].subscription_data[sub_cnt].sub_type ==
                                                                  SIMLOCK_SUBSCRIPTION_3GPP2)
        {
          resp_msg_ptr->card_validity_data[slot_index].num_subscriptions++;
          simlock_status = simlock_3gpp2_run_algorithm(&req_msg_ptr->card_data[slot_index].subscription_data[sub_cnt],
                                                       slot,
                                                       &config_file_data,
                                                       &resp_msg_ptr->card_validity_data[slot_index].subscription_data[sub_cnt]);
        }
        else
        {
          /* Nothing to do */
        }
      } /* end for loop, sub_cnt = 0 */
    }
  } /* end for loop, slot = SIMLOCK_SLOT_1 */

  /* apply the config policies */
  if(simlock_status == SIMLOCK_SUCCESS)
  {
    simlock_status = simlock_apply_config_policy(req_msg_ptr,
                                                 &config_file_data, resp_msg_ptr);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_0("apply config policy failed");
    }
  }

  /* Update valid IMSI list */
  simlock_validity_update_imsi_info(req_msg_ptr, resp_msg_ptr);

  return simlock_status;
} /* simlock_run_algorithm */


/*===========================================================================
FUNCTION SIMLOCK_SET_LOCK_CK

DESCRIPTION
  This function sets and enables the SIMLock for a category. It takes the ck
  and the iteration count as the inputs and generates HCK needed for the
  lock. It also generates salt needed for generating the HCK. It then writes
  the lock data to a category file. Also, it updates the config file
  indicating that the lock is enabled for the category, upon successfully
  updating the lock data.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_set_lock_ck
(
  const simlock_set_lock_ck_msg_req_type  * req_msg_ptr
)
{
  simlock_result_enum_type               simlock_status       = SIMLOCK_SUCCESS;
  uint8                                  slot_index           = 0;
  uint8                                  category_index       = 0;
  simlock_category_file_data_type      * category_data_ptr    = NULL;
  simlock_salt_type                      salt_data_buf;
  simlock_hck_type                       hck_data_buf;
  simlock_config_data_type               config_file_data;

  if(req_msg_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_set_lock_ck");

  memset(salt_data_buf, 0x00, SIMLOCK_SALT_LEN);
  memset(hck_data_buf, 0x00, SIMLOCK_HCK_LEN);
  memset(&config_file_data, 0x00, sizeof(simlock_config_data_type));

  /* check if category is supported */
  simlock_status = simlock_util_check_category_supported(
                          req_msg_ptr->category.category_type);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  /* read config file data */
  simlock_status = simlock_config_read_config_data(&config_file_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  simlock_status = simlock_util_get_slot_index(req_msg_ptr->slot, &slot_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get slot index failed for slot, 0x%x", req_msg_ptr->slot);
    return SIMLOCK_GENERIC_ERROR;
  }

  simlock_status = simlock_util_get_category_index(req_msg_ptr->category.category_type,
                                                   &category_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (category_index >= SIMLOCK_CATEGORY_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get category index failed for category, 0x%x",
                      req_msg_ptr->category.category_type);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* check if the category is already enabled */
  if(simlock_util_check_category_enabled(&config_file_data,
                                         req_msg_ptr->category.category_type,
                                         req_msg_ptr->slot))
  {
    SIMLOCK_MSG_ERR_1("category 0x%x already enabled",
                      req_msg_ptr->category.category_type);
    return SIMLOCK_INVALID_STATE;
  }

  /* if autolock is not set, then codes need to be validated */
  if(req_msg_ptr->auto_lock == FALSE)
  {
    /* check if the code data chars are valid */
    simlock_status = simlock_util_check_code_data(&req_msg_ptr->category);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_1("invalid code data for the category, 0x%x",
                        req_msg_ptr->category.category_type);
      return simlock_status;
    }

    /* check if the codes are conflicting */
    simlock_status = simlock_category_verify_codes_for_conflicts(&req_msg_ptr->category,
                                                                 &config_file_data,
                                                                 req_msg_ptr->slot);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_1("conflicting code data for the category, 0x%x",
                        req_msg_ptr->category.category_type);
      return simlock_status;
    }
  }

  /* if the auto_lock is enabled, then the lock codes are set from the
     SIM card. So in this case, codes cannot be blacklist */
  if((req_msg_ptr->auto_lock) &&
     (req_msg_ptr->blacklist))
  {
    SIMLOCK_MSG_ERR_1("both autolock and blacklist are enabled, for category, 0x%x",
                      req_msg_ptr->category.category_type);
    return SIMLOCK_GENERIC_ERROR;
  }

  if(req_msg_ptr->ck.simlock_ck_length > 0)
  {
    /* generate the random salt */
    simlock_status = simlock_crypto_generate_salt(salt_data_buf);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_1("salt generation failed, status: 0x%x", simlock_status);
      return simlock_status;
    }

    /* generate the HCK by running the PBKDF2 algo */
    simlock_status = simlock_crypto_run_pbkdf2_algorithm((const uint8 *)req_msg_ptr->ck.simlock_ck_data,
                                                         (uint16)req_msg_ptr->ck.simlock_ck_length,
                                                         salt_data_buf,
                                                         SIMLOCK_SALT_LEN,
                                                         req_msg_ptr->iteration_cnt,
                                                         hck_data_buf,
                                                         SIMLOCK_HCK_LEN);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_1("hck generation failed, status: 0x%x", simlock_status);
      return simlock_status;
    }

    SIMLOCK_MSG_LOW_0("**HCK generated**");

    simlock_print_hck_data(hck_data_buf);
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr,
                               sizeof(simlock_category_file_data_type));
  if(category_data_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  simlock_status = simlock_category_build_lock_data(req_msg_ptr,
                                                    salt_data_buf,
                                                    hck_data_buf,
                                                    category_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("build lock data failed, status: 0x%x", simlock_status);

    /* this frees the code data in the category */
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    return simlock_status;
  }

  /* write the category data */
  simlock_status = simlock_category_write_lock_data(req_msg_ptr->slot,
                                                    category_data_ptr);
  if(simlock_status == SIMLOCK_SUCCESS)
  {
    simlock_ck_present_type ck_present = SIMLOCK_CK_NOT_PRESENT;

    if(req_msg_ptr->ck.simlock_ck_length > 0)
    {
      ck_present = SIMLOCK_CK_PRESENT;
    }

    /* enable the category in config file */
    simlock_status = simlock_config_update_and_write_category_status(
                                       category_index,
                                       slot_index,
                                       SIMLOCK_ENABLED,
                                       ck_present,
                                       &config_file_data);
    if(simlock_status == SIMLOCK_SUCCESS)
    {
      /* SimLock configuration has been successfully written. Blow the SimLock
         fuse for enhanced security. The function below blows the fuse only
         if it has not been blown yet. So, its ok to call it everytime the set
         lock ck API is called */
      if(SIMLOCK_SUCCESS != simlock_fuse_blow_fuse())
      {
        ASSERT(0);
      }
    }
    else
    {
      SIMLOCK_MSG_ERR_1("update category status failed, status: 0x%x",
                        simlock_status);
    }
  }

  /* this frees the code data in the category */
  simlock_category_free_code_data(category_data_ptr);
  SIMLOCK_MEM_FREE(category_data_ptr);

  return simlock_status;
} /* simlock_set_lock_ck */


/*===========================================================================
FUNCTION SIMLOCK_RELOCK_DEVICE

DESCRIPTION
  This function reenables the SIMLock for the passed category with the input
  CK. As a first step it makes sure that the category is disabled & its
  data - like salt, HCK is present in the file. Then HCK is generated using
  the input CK, stored salt & iteration count. Relock is allowed only when
  the generated & stored HCK matches. Finally, data related to retries & the
  category is updated in the respective files.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_relock_device
(
  const simlock_relock_device_msg_req_type  * req_msg_ptr,
  simlock_relock_device_msg_resp_type       * resp_msg_ptr
)
{
  simlock_result_enum_type               simlock_status       = SIMLOCK_SUCCESS;
  uint8                                  slot_index           = 0;
  uint8                                  category_index       = 0;
  simlock_category_file_data_type      * category_data_ptr    = NULL;
  simlock_hck_type                       hck_data_buf;
  simlock_config_data_type               config_file_data;

  if((req_msg_ptr == NULL) || (resp_msg_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* Cannot proceed if there in no CK passed */
  if(req_msg_ptr->ck.simlock_ck_length == 0)
  {
    SIMLOCK_MSG_ERR_0("input parameters error, 0 simlock_ck_length");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_relock_device");

  memset(hck_data_buf, 0x00, SIMLOCK_HCK_LEN);
  memset(&config_file_data, 0x00, sizeof(simlock_config_data_type));

  /* Read config file data */
  simlock_status = simlock_config_read_config_data(&config_file_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_0("read config file failed");
    return simlock_status;
  }

  simlock_status = simlock_util_get_slot_index(req_msg_ptr->slot, &slot_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get slot index failed for slot, 0x%x", req_msg_ptr->slot);
    return SIMLOCK_GENERIC_ERROR;
  }

  simlock_status = simlock_util_get_category_index(req_msg_ptr->category_type,
                                                   &category_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (category_index >= SIMLOCK_CATEGORY_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get category index failed for category, 0x%x",
                      req_msg_ptr->category_type);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* If the category is still enabled, cannot relock */
  if(simlock_util_check_category_enabled(&config_file_data,
                                         req_msg_ptr->category_type,
                                         req_msg_ptr->slot))
  {
    SIMLOCK_MSG_ERR_1("category 0x%x already enabled",
                      req_msg_ptr->category_type);
    return SIMLOCK_INVALID_STATE;
  }

  /* If the category doesn't have stored CK data irrespective of how the lock
     was enabled/disabled, we cannot proceed */
  if(config_file_data.category_status[slot_index][category_index].ck_present
       == SIMLOCK_CK_NOT_PRESENT)
  {
    SIMLOCK_MSG_ERR_1("category has no stored CK: 0x%x", req_msg_ptr->category_type);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* Now get the category's data */
  SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr,
                               sizeof(simlock_category_file_data_type));
  if(category_data_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Read the lock code header and data */
  simlock_status = simlock_category_read_lock_data(req_msg_ptr->category_type,
                                                   req_msg_ptr->slot,
                                                   category_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    /* this frees the code data in the category */
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    return simlock_status;
  }

  /* Sanity check the file data */
  if(category_data_ptr->category_header.category_type != req_msg_ptr->category_type)
  {
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    SIMLOCK_MSG_ERR_0("invalid lock data");
    return SIMLOCK_GENERIC_ERROR;
  }

  /* If the current unlock attempts count is equal to maximum
     retries allowed then return error */
  if(category_data_ptr->category_header.num_retries_max != 0)
  {
    if(category_data_ptr->category_header.curr_retries ==
         category_data_ptr->category_header.num_retries_max)
    {
      SIMLOCK_MSG_ERR_0("relock attempts expired");
      resp_msg_ptr->num_retries_max = category_data_ptr->category_header.num_retries_max;
      resp_msg_ptr->curr_retries = category_data_ptr->category_header.curr_retries;

      /* this frees the code data in the category */
      simlock_category_free_code_data(category_data_ptr);
      SIMLOCK_MEM_FREE(category_data_ptr);
      return SIMLOCK_MAX_RETRIES_REACHED;
    }
  }

  SIMLOCK_MSG_LOW_0("**HCK from file**");

  simlock_print_hck_data(category_data_ptr->category_header.hck);

  SIMLOCK_MSG_MED_1("iteration_cnt: 0x%x",
                    category_data_ptr->category_header.iteration_cnt);

  /* generate the HCK by running the PBKDF2 algo */
  simlock_status = simlock_crypto_run_pbkdf2_algorithm((uint8 *)req_msg_ptr->ck.simlock_ck_data,
                                                       (uint16)req_msg_ptr->ck.simlock_ck_length,
                                                       category_data_ptr->category_header.salt,
                                                       SIMLOCK_SALT_LEN,
                                                       category_data_ptr->category_header.iteration_cnt,
                                                       hck_data_buf,
                                                       SIMLOCK_HCK_LEN);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("hck generation failed, status: 0x%x", simlock_status);

    /* this frees the code data in the category */
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    return simlock_status;
  }

  SIMLOCK_MSG_LOW_0("**HCK generated**");

  simlock_print_hck_data(hck_data_buf);

  /* If the stored HCK matches the generated HCK, proceed to lock */
  if(memcmp(hck_data_buf, category_data_ptr->category_header.hck, SIMLOCK_HCK_LEN) == 0)
  {
    SIMLOCK_MSG_ERR_0("HCK matches");

    /* Update the retries count */
    category_data_ptr->category_header.curr_retries = 0;

    /* Write the category data */
    simlock_status = simlock_category_write_lock_data(req_msg_ptr->slot,
                                                      category_data_ptr);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_1("write lock data failed, status: 0x%x", simlock_status);
      simlock_status = SIMLOCK_GENERIC_ERROR;
    }
    else
    {
      /* Enable the category in config file */
      simlock_status = simlock_config_update_and_write_category_status(
                                       category_index,
                                       slot_index,
                                       SIMLOCK_ENABLED,
                                       SIMLOCK_CK_PRESENT,
                                       &config_file_data);
      if(simlock_status != SIMLOCK_SUCCESS)
      {
        SIMLOCK_MSG_ERR_1("Update category status failed, status: 0x%x",
                          simlock_status);
        simlock_status = SIMLOCK_GENERIC_ERROR;
      }
    }

    if(simlock_status == SIMLOCK_SUCCESS)
    {
      /* Send retries back in case of successful relocking */
      resp_msg_ptr->num_retries_max =
        category_data_ptr->category_header.num_retries_max;
      resp_msg_ptr->curr_retries =
        category_data_ptr->category_header.curr_retries;

      SIMLOCK_MSG_MED_2("max_retries: 0x%x, curr_retries: 0x%x",
                         resp_msg_ptr->num_retries_max,
                         resp_msg_ptr->curr_retries);
    }
  }
  else
  {
    SIMLOCK_MSG_ERR_0("Cannot relock. mismatch in key data");

    /* Wrong key given for relock.
       Hence first set the status as incorrect key */
    simlock_status = SIMLOCK_INCORRECT_KEY;

    /* Also update the current retry counter */
    if(category_data_ptr->category_header.num_retries_max != 0)
    {
      category_data_ptr->category_header.curr_retries++;

      /* if lock data needs to be written, it has to be a success */
      if(SIMLOCK_SUCCESS == simlock_category_write_lock_data(req_msg_ptr->slot,
                                                             category_data_ptr))
      {
        resp_msg_ptr->num_retries_max =
               category_data_ptr->category_header.num_retries_max;
        resp_msg_ptr->curr_retries =
               category_data_ptr->category_header.curr_retries;

        /* Update error code if max retries reached for this attempt */
        if(category_data_ptr->category_header.curr_retries ==
           category_data_ptr->category_header.num_retries_max)
        {
          simlock_status = SIMLOCK_MAX_RETRIES_REACHED;
        }
      }
    }

    SIMLOCK_MSG_MED_3("max_retries: 0x%x, curr_retries: 0x%x simlock_status 0x%x",
                       resp_msg_ptr->num_retries_max,
                       resp_msg_ptr->curr_retries,
                       simlock_status);
  }

  /* This frees the code data in the category */
  simlock_category_free_code_data(category_data_ptr);
  SIMLOCK_MEM_FREE(category_data_ptr);

  return simlock_status;
} /* simlock_relock_device */


/*===========================================================================
FUNCTION SIMLOCK_UNLOCK_DEVICE

DESCRIPTION
  This function disables the SIMLock for a category. It takes the ck
  as the input. It reads the category file to retrieve the salt,
  iteration count and the HCK. It generates the HCK using the input ck and
  retrieved salt and iteration count. If the generated and retrieved HCK
  matches, it unlocks the device for the category. Also, it updates the
  config file indicating that the lock is disabled for the category,
  upon successfully unlocking.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_unlock_device
(
  const simlock_unlock_device_msg_req_type   * req_msg_ptr,
  simlock_unlock_device_msg_resp_type        * resp_msg_ptr
)
{
  simlock_result_enum_type            simlock_status       = SIMLOCK_SUCCESS;
  simlock_slot_enum_type              slot_id              = SIMLOCK_SLOT_1;
  uint8                               slot_index           = 0;
  uint8                               category_index       = 0;
  simlock_category_file_data_type  *  category_data_ptr    = NULL;
  simlock_hck_type                    hck_data_buf;
  simlock_config_data_type            config_file_data;

  if((req_msg_ptr == NULL) ||
     (resp_msg_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_unlock_device");

  memset(hck_data_buf, 0x00, SIMLOCK_HCK_LEN);
  memset(&config_file_data, 0x00, sizeof(simlock_config_data_type));

  /* read config file data */
  simlock_status = simlock_config_read_config_data(&config_file_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  /* if the slot policy is same on all the available slots, then
     it is expected to have one category file per each enabled category
     i.e. all the codes for that category are in one file */
  if((config_file_data.slot_policy == SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS) &&
     (req_msg_ptr->slot != SIMLOCK_SLOT_1))
  {
    slot_id = SIMLOCK_SLOT_1;
  }
  else
  {
    slot_id = req_msg_ptr->slot;
  }

  simlock_status = simlock_util_get_slot_index(slot_id, &slot_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get slot index failed for slot, 0x%x", slot_id);
    return SIMLOCK_GENERIC_ERROR;
  }

  simlock_status = simlock_util_get_category_index(req_msg_ptr->category,
                                                   &category_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (category_index >= SIMLOCK_CATEGORY_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get category index failed for category, 0x%x",
                      req_msg_ptr->category);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* check if the category is enabled. If not error */
  if(!simlock_util_check_category_enabled(&config_file_data,
                                          req_msg_ptr->category,
                                          slot_id))
  {
    SIMLOCK_MSG_ERR_1("category 0x%x not enabled",
                      req_msg_ptr->category);
    return SIMLOCK_INVALID_STATE;
  }

  /* when a category is locked without CK we should not allow it to be unlocked with CK */
  if(config_file_data.category_status[slot_index][category_index].ck_present == SIMLOCK_CK_NOT_PRESENT &&
     req_msg_ptr->ck.simlock_ck_length > 0)
  {
    SIMLOCK_MSG_ERR_1("category was securely locked without CK: 0x%x",
                      req_msg_ptr->category);
    /* Decreasing the number of retries is not required when category is locked without CK as it
       provides no real value. */
    return SIMLOCK_INCORRECT_KEY;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr,
                               sizeof(simlock_category_file_data_type));
  if(category_data_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* read the lock code header and data */
  simlock_status = simlock_category_read_lock_data(req_msg_ptr->category,
                                                   slot_id,
                                                   category_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    /* this frees the code data in the category */
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    return simlock_status;
  }

  /* sanity check the file data */
  if(category_data_ptr->category_header.category_type !=
      req_msg_ptr->category)
  {
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    SIMLOCK_MSG_ERR_0("invalid lock data");
    return SIMLOCK_GENERIC_ERROR;
  }

  /* if the current unlock attempts count is equal to maximum
     retries allowed then return error */
  if(category_data_ptr->category_header.num_retries_max != 0)
  {
    if(category_data_ptr->category_header.curr_retries ==
       category_data_ptr->category_header.num_retries_max)
    {
      SIMLOCK_MSG_ERR_0("unlock attempts expired");
      resp_msg_ptr->num_retries_max =
         category_data_ptr->category_header.num_retries_max;
      resp_msg_ptr->curr_retries =
         category_data_ptr->category_header.curr_retries;

      /* this frees the code data in the category */
      simlock_category_free_code_data(category_data_ptr);
      SIMLOCK_MEM_FREE(category_data_ptr);
      return SIMLOCK_MAX_RETRIES_REACHED;
    }
  }

  SIMLOCK_MSG_LOW_0("**HCK from file**");

  simlock_print_hck_data(category_data_ptr->category_header.hck);

  SIMLOCK_MSG_MED_1("iteration_cnt: 0x%x",
                    category_data_ptr->category_header.iteration_cnt);

  SIMLOCK_MSG_MED_1("num_codes: 0x%x",
                    category_data_ptr->category_header.num_of_codes);

  if(req_msg_ptr->ck.simlock_ck_length > 0)
  {
    if(category_data_ptr->category_header.iteration_cnt == 0)
    {
      simlock_category_free_code_data(category_data_ptr);
      SIMLOCK_MEM_FREE(category_data_ptr);
      SIMLOCK_MSG_ERR_0("invalid lock data");
      return SIMLOCK_GENERIC_ERROR;
    }

    /* generate the HCK by running the PBKDF2 algo */
    simlock_status = simlock_crypto_run_pbkdf2_algorithm((const uint8 *)req_msg_ptr->ck.simlock_ck_data,
                                                         (uint16)req_msg_ptr->ck.simlock_ck_length,
                                                         category_data_ptr->category_header.salt,
                                                         SIMLOCK_SALT_LEN,
                                                         category_data_ptr->category_header.iteration_cnt,
                                                         hck_data_buf,
                                                         SIMLOCK_HCK_LEN);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_1("hck generation failed, status: 0x%x", simlock_status);

      /* this frees the code data in the category */
      simlock_category_free_code_data(category_data_ptr);
      SIMLOCK_MEM_FREE(category_data_ptr);
      return simlock_status;
    }

    SIMLOCK_MSG_LOW_0("**HCK generated**");

    simlock_print_hck_data(hck_data_buf);
  }

  /* Compare the HCK generated with that in the category file.
     Don't allow unlock operation without CK if simLock is configured with CK
     as unlock without CK is supported for TZ requests. */
  if((req_msg_ptr->ck.simlock_ck_length == 0 && 
      config_file_data.category_status[slot_index][category_index].ck_present == SIMLOCK_CK_NOT_PRESENT)||
     memcmp(hck_data_buf, category_data_ptr->category_header.hck,
            SIMLOCK_HCK_LEN) == 0)
  {
    SIMLOCK_MSG_MED_0("key data matched");

    /* update the config file with category status as disabled */
    simlock_status = simlock_config_update_and_write_category_status(category_index,
                                                                     slot_index,
                                                                     SIMLOCK_DISABLED_FILE_PRESENT,
                                                                     config_file_data.category_status[slot_index][category_index].ck_present,
                                                                     &config_file_data);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_1("update config file failed, status: 0x%x", simlock_status);
    }
    else
    {
      /* in case the category is successfully unlocked while
         the temporary unlock is currently active, stop the timer */
      (void)simlock_timer_stop(slot_id, req_msg_ptr->category);

      /* Also, reset the retries count & write the data back */
      category_data_ptr->category_header.curr_retries = 0;

      /* Write the lock data */
      simlock_status = simlock_category_write_lock_data(slot_id,
                                                        category_data_ptr);
      if(simlock_status != SIMLOCK_SUCCESS)
      {
        SIMLOCK_MSG_ERR_0("write lock data failed");
      }
    }
  }
  else
  {
    SIMLOCK_MSG_ERR_0("cannot unlock. mismatch in key data");

    /* Wrong key given for unlock.
       Hence first set the status as incorrect key */
    simlock_status = SIMLOCK_INCORRECT_KEY;

    if(category_data_ptr->category_header.num_retries_max != 0)
    {
      category_data_ptr->category_header.curr_retries++;

      if (SIMLOCK_SUCCESS == simlock_category_write_lock_data(slot_id,
                                                              category_data_ptr)) 
      {
        resp_msg_ptr->num_retries_max =
               category_data_ptr->category_header.num_retries_max;
        resp_msg_ptr->curr_retries =
               category_data_ptr->category_header.curr_retries;

        /* Update error code if max retries reached for this attempt */
        if(category_data_ptr->category_header.curr_retries ==
           category_data_ptr->category_header.num_retries_max)
        {
          simlock_status = SIMLOCK_MAX_RETRIES_REACHED;
        }
      }
    }

    SIMLOCK_MSG_MED_3("max_retries: 0x%x, curr_retries: 0x%x simlock_status 0x%x",
                       resp_msg_ptr->num_retries_max,
                       resp_msg_ptr->curr_retries,
                       simlock_status);
  }

  /* this frees the code data in the category */
  simlock_category_free_code_data(category_data_ptr);
  SIMLOCK_MEM_FREE(category_data_ptr);

  return simlock_status;
} /* simlock_unlock_device */


/*===========================================================================
FUNCTION SIMLOCK_GET_LOCK_STATUS

DESCRIPTION
  This function retrieves the status of all the categories on all the slots
  supported

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_get_lock_status
(
  simlock_get_status_msg_resp_type * resp_msg_ptr
)
{
  simlock_result_enum_type          simlock_status          = SIMLOCK_SUCCESS;
  simlock_slot_enum_type            slot                    = SIMLOCK_SLOT_1;
  simlock_slot_enum_type            locked_slot             = SIMLOCK_SLOT_1;
  simlock_category_enum_type        category                = SIMLOCK_CATEGORY_3GPP_NW;
  uint8                             slot_index              = 0;
  uint8                             category_index          = 0;
  simlock_category_file_data_type   category_file_data;
  simlock_config_data_type          config_file_data;

  if(resp_msg_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  memset(&category_file_data, 0x00,
            sizeof(simlock_category_file_data_type));
  memset(&config_file_data, 0x00, sizeof(simlock_config_data_type));

  /* read config file data */
  simlock_status = simlock_config_read_config_data(&config_file_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  /* update the policies */
  resp_msg_ptr->slot_policy = config_file_data.slot_policy;
  resp_msg_ptr->subscription_policy = config_file_data.sub_policy;

  for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM;
      slot++)
  {
    simlock_status = simlock_util_get_slot_index(slot, &slot_index);
    if((simlock_status == SIMLOCK_SUCCESS) &&
       (slot_index < SIMLOCK_SLOT_COUNT_MAX))
    {
      for(category = SIMLOCK_CATEGORY_3GPP_NW; category <= SIMLOCK_CATEGORY_3GPP2_RUIM;
          category++)
      {
        simlock_status = simlock_util_get_category_index(category, &category_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (category_index < SIMLOCK_CATEGORY_COUNT_MAX))
        {
          memset(&category_file_data, 0x00,
                 sizeof(simlock_category_file_data_type));

          resp_msg_ptr->lock_info[slot_index].category_info[category_index].category = category;

          /* check if a category is disabled. If yes, continue */
          if(!simlock_util_check_category_enabled(&config_file_data, category, slot) &&
             !simlock_util_check_category_disabled_file_present(&config_file_data, category, slot))
          {
            resp_msg_ptr->lock_info[slot_index].category_info[category_index].category_status = FALSE;
            resp_msg_ptr->lock_info[slot_index].category_info[category_index].category_ok_to_reactivate = FALSE;
            continue;
          }

          /* in this case, codes are present in one file */
          if(config_file_data.slot_policy == SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS)
          {
            locked_slot = SIMLOCK_SLOT_1;
          }
          else
          {
            locked_slot = slot;
          }

          /* read the lock data */
          simlock_status = simlock_category_read_lock_data(category, locked_slot,
                                                           &category_file_data);
          if(simlock_status != SIMLOCK_SUCCESS)
          {
            SIMLOCK_MSG_ERR_2("read lock data failed, category: 0x%x,"
                              " slot: 0x%x", category, locked_slot);

            /* this frees the code data in the category */
            simlock_category_free_code_data(&category_file_data);
            return simlock_status;
          }

          if(simlock_util_check_category_enabled(&config_file_data, category, slot))
          {
            resp_msg_ptr->lock_info[slot_index].category_info[category_index].category_status = TRUE;
            resp_msg_ptr->lock_info[slot_index].category_info[category_index].category_ok_to_reactivate = FALSE;
          }
          else
          {
            resp_msg_ptr->lock_info[slot_index].category_info[category_index].category_status = FALSE;
            resp_msg_ptr->lock_info[slot_index].category_info[category_index].category_ok_to_reactivate = TRUE;
          }
          resp_msg_ptr->lock_info[slot_index].category_info[category_index].ck_present                =
            config_file_data.category_status[slot_index][category_index].ck_present;
          if(category_file_data.category_header.code_type == SIMLOCK_CODE_BLACKLIST)
          {
            resp_msg_ptr->lock_info[slot_index].category_info[category_index].blacklist = TRUE;
          }
          resp_msg_ptr->lock_info[slot_index].category_info[category_index].num_retries_max =
            category_file_data.category_header.num_retries_max;
          resp_msg_ptr->lock_info[slot_index].category_info[category_index].curr_retries =
            category_file_data.category_header.curr_retries;

          simlock_category_free_code_data(&category_file_data);
        }
      } /* end loop categories */
    }
  } /* end loop for slots */

  /* get the temporary unlock remaining time */
  (void)simlock_temp_unlock_get_remaining_time(&resp_msg_ptr->remaining_time);

  return SIMLOCK_SUCCESS;
} /* simlock_get_lock_status */


/*===========================================================================
FUNCTION SIMLOCK_GET_LOCK_CATEGORY_DATA

DESCRIPTION
  This function retrieves the code data of the requested category on
  a slot

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_get_lock_category_data
(
  const simlock_get_category_data_msg_req_type * req_msg_ptr,
  simlock_get_category_data_msg_resp_type      * resp_msg_ptr
)
{
  simlock_result_enum_type         simlock_status = SIMLOCK_SUCCESS;
  simlock_slot_enum_type           slot_id        = SIMLOCK_SLOT_1;
  simlock_slot_policy_enum_type    slot_policy    = SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS;
  simlock_config_data_type         config_file_data;

  if((req_msg_ptr == NULL) ||
     (resp_msg_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if(simlock_util_check_slot(req_msg_ptr->slot) != SIMLOCK_SUCCESS)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_get_category_data");

  /* check if category is supported */
  simlock_status = simlock_util_check_category_supported(
                                     req_msg_ptr->category);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  memset(&config_file_data, 0x00, sizeof(simlock_config_data_type));

  /* read config file data */
  simlock_status = simlock_config_read_config_data(&config_file_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  /* check the slot policy */
  slot_policy = config_file_data.slot_policy;

  /* if the slot policy is same on all the available slots, then
     it is expected to have one category file per each enabled category
     i.e. all the codes for that category are in one file */
  if((slot_policy == SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS) &&
     (req_msg_ptr->slot != SIMLOCK_SLOT_1))
  {
    slot_id = SIMLOCK_SLOT_1;
  }
  else
  {
    slot_id = req_msg_ptr->slot;
  }

  /* check if the category is enabled. If not error */
  if(!simlock_util_check_category_enabled(&config_file_data,
                                          req_msg_ptr->category,
                                          slot_id))
  {
    SIMLOCK_MSG_ERR_1("category 0x%x not enabled",
                      req_msg_ptr->category);
    return SIMLOCK_INVALID_STATE;
  }

  simlock_status = simlock_category_get_code_data(slot_id, req_msg_ptr->category,
                                                  resp_msg_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_0("get code data failed");
  }

  return simlock_status;
} /* simlock_get_lock_category_data */


/*===========================================================================
FUNCTION SIMLOCK_SET_LOCK_HCK

DESCRIPTION
  This function sets and enables the SIMLock for a category. It takes the ck
  and the iteration count as the inputs and generates HCK needed for the
  lock. It then writes the lock data to a category file. Also, it updates
  the config file indicating that the lock is enabled for the category,
  upon successfully updating the lock data.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_set_lock_hck
(
  const simlock_set_lock_hck_msg_req_type  * req_msg_ptr
)
{
  simlock_result_enum_type               simlock_status       = SIMLOCK_SUCCESS;
  uint8                                  slot_index           = 0;
  uint8                                  category_index       = 0;
  simlock_category_file_data_type      * category_data_ptr    = NULL;
  simlock_set_lock_ck_msg_req_type     * lock_ck_msg_ptr      = NULL;
  simlock_config_data_type               config_file_data;

  if(req_msg_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_set_lock_hck");

  memset(&config_file_data, 0x00, sizeof(simlock_config_data_type));

  /* check if category is supported */
  simlock_status = simlock_util_check_category_supported(
                          req_msg_ptr->category.category_type);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  /* read config file data */
  simlock_status = simlock_config_read_config_data(&config_file_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  simlock_status = simlock_util_get_slot_index(req_msg_ptr->slot, &slot_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get slot index failed for slot, 0x%x", req_msg_ptr->slot);
    return SIMLOCK_GENERIC_ERROR;
  }

  simlock_status = simlock_util_get_category_index(req_msg_ptr->category.category_type,
                                                   &category_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (category_index >= SIMLOCK_CATEGORY_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get category index failed for category, 0x%x",
                      req_msg_ptr->category.category_type);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* check if the category is already enabled */
  if(simlock_util_check_category_enabled(&config_file_data,
                                         req_msg_ptr->category.category_type,
                                         req_msg_ptr->slot))
  {
    SIMLOCK_MSG_ERR_1("category 0x%x already enabled",
                      req_msg_ptr->category.category_type);
    return SIMLOCK_INVALID_STATE;
  }

  /* if autolock is not set, then codes need to be validated */
  if(req_msg_ptr->auto_lock == FALSE)
  {
    /* check if the code data chars are valid */
    simlock_status = simlock_util_check_code_data(&req_msg_ptr->category);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_1("no valid code data for the category, 0x%x",
                         req_msg_ptr->category.category_type);
      return simlock_status;
    }

    /* check if the codes are conflicting */
    simlock_status = simlock_category_verify_codes_for_conflicts(&req_msg_ptr->category,
                                                                 &config_file_data,
                                                                 req_msg_ptr->slot);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_1("conflicting code data for the category, 0x%x",
                        req_msg_ptr->category.category_type);
      return simlock_status;
    }
  }

  /* if the auto_lock is enabled, then the lock codes are set from the
     SIM card. So in this case, codes cannot be blacklist */
  if((req_msg_ptr->auto_lock) &&
     (req_msg_ptr->blacklist))
  {
    SIMLOCK_MSG_ERR_1("both autolock and blacklist are enabled, for category, 0x%x",
                      req_msg_ptr->category.category_type);
    return SIMLOCK_GENERIC_ERROR;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr,
                               sizeof(simlock_category_file_data_type));
  if(category_data_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(lock_ck_msg_ptr,
                               sizeof(simlock_set_lock_ck_msg_req_type));
  if(lock_ck_msg_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  lock_ck_msg_ptr->slot = req_msg_ptr->slot;
  simlock_memscpy(&lock_ck_msg_ptr->category,
                  sizeof(simlock_category_data_type),
                  &req_msg_ptr->category,
                  sizeof(simlock_category_data_type));
  lock_ck_msg_ptr->auto_lock = req_msg_ptr->auto_lock;
  lock_ck_msg_ptr->blacklist = req_msg_ptr->blacklist;
  lock_ck_msg_ptr->num_retries_max = req_msg_ptr->num_retries_max;
  lock_ck_msg_ptr->iteration_cnt = req_msg_ptr->iteration_cnt;

  simlock_status = simlock_category_build_lock_data(lock_ck_msg_ptr,
                                                    req_msg_ptr->salt,
                                                    req_msg_ptr->hck,
                                                    category_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("build lock data failed, status: 0x%x", simlock_status);

    /* this frees the code data in the category */
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    SIMLOCK_MEM_FREE(lock_ck_msg_ptr);
    return simlock_status;
  }

  /* write the category data */
  simlock_status = simlock_category_write_lock_data(req_msg_ptr->slot,
                                                    category_data_ptr);
  if(simlock_status == SIMLOCK_SUCCESS)
  {
    /* enable the category in config file */
    simlock_status = simlock_config_update_and_write_category_status(
                                       category_index,
                                       slot_index,
                                       SIMLOCK_ENABLED,
                                       SIMLOCK_CK_PRESENT,
                                       &config_file_data);
    if(simlock_status == SIMLOCK_SUCCESS)
    {
      /* SimLock configuration has been successfully written. Blow the SimLock
         fuse for enhanced security. The function below blows the fuse only
         if it has not been blown yet. So, its ok to call it everytime the set
         lock hck API is called */
      if(SIMLOCK_SUCCESS != simlock_fuse_blow_fuse())
      {
        ASSERT(0);
      }
    }
    else
    {
      SIMLOCK_MSG_ERR_1("update category status failed, status: 0x%x",
                        simlock_status);
    }
  }

  /* this frees the code data in the category */
  simlock_category_free_code_data(category_data_ptr);
  SIMLOCK_MEM_FREE(category_data_ptr);
  SIMLOCK_MEM_FREE(lock_ck_msg_ptr);

  return simlock_status;
} /* simlock_set_lock_hck */


/*===========================================================================
FUNCTION SIMLOCK_TEMPORARY_UNLOCK_REQUEST

DESCRIPTION
  This function sets the temporary unlock for a category or unsets a
  previous temporary unlock.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_temporary_unlock_request
(
  const simlock_temporary_unlock_msg_req_type  * req_msg_ptr
)
{
  simlock_result_enum_type   simlock_status = SIMLOCK_SUCCESS;
  simlock_config_data_type   config_file_data;

  if(req_msg_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  memset(&config_file_data, 0x00, sizeof(simlock_config_data_type));

  SIMLOCK_MSG_MED_1("simlock_temporary_unlock_request, req_type: 0x%x",
                    req_msg_ptr->temporary_unlock_type);

  /* check if slot is correct */
  if(simlock_util_check_slot(req_msg_ptr->slot) != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("slot 0x%x not valid", req_msg_ptr->slot);
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* check if category is supported */
  simlock_status = simlock_util_check_category_supported(req_msg_ptr->category);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("category 0x%x not supported", req_msg_ptr->category);
    return simlock_status;
  }

  /* read config file data */
  simlock_status = simlock_config_read_config_data(&config_file_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_0("read config file failed");
    return simlock_status;
  }

  /* check if the category is enabled. If not error */
  if(!simlock_util_check_category_enabled(&config_file_data,
                                          req_msg_ptr->category,
                                          req_msg_ptr->slot))
  {
    SIMLOCK_MSG_ERR_1("category 0x%x not enabled",
                      req_msg_ptr->category);
    return SIMLOCK_INVALID_STATE;
  }

  switch(req_msg_ptr->temporary_unlock_type)
  {
    case SIMLOCK_TEMPORARY_UNLOCK_SET:
      return simlock_timer_start(req_msg_ptr->slot,
                                 req_msg_ptr->category,
                                 req_msg_ptr->duration);

    case SIMLOCK_TEMPORARY_UNLOCK_UNSET:
      return simlock_timer_stop(req_msg_ptr->slot,
                                req_msg_ptr->category);

    default:
      break;
  }

  return SIMLOCK_GENERIC_ERROR;
} /* simlock_temporary_unlock_request */


/*===========================================================================
FUNCTION SIMLOCK_SET_DEVICE_MODE_REQUEST

DESCRIPTION
  This function sets the device mode to emergency only or normal mode

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_set_device_mode_request
(
  const simlock_set_device_mode_msg_req_type  * req_msg_ptr
)
{
  simlock_result_enum_type    simlock_status = SIMLOCK_SUCCESS;
  simlock_config_data_type    config_file_data;

  if(req_msg_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_1("simlock_set_device_mode_request, req: 0x%x",
                    req_msg_ptr->mode);

  memset(&config_file_data, 0x00, sizeof(simlock_config_data_type));

  /* read config file data */
  simlock_status = simlock_config_read_config_data(&config_file_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_0("read config file failed");
    return simlock_status;
  }

  if(config_file_data.device_mode == req_msg_ptr->mode)
  {
    SIMLOCK_MSG_ERR_1("device is already in mode, 0x%x",
                       req_msg_ptr->mode);
    return SIMLOCK_INVALID_STATE;
  }

  simlock_status = simlock_config_update_and_write_device_mode(req_msg_ptr->mode,
                                                               &config_file_data);
  if(simlock_status == SIMLOCK_SUCCESS)
  {
    /* SimLock configuration has been successfully updated. Blow the SimLock
       fuse for enhanced security. The function below blows the fuse only
       if it has not been blown yet. So, its ok to call it everytime the
       set device mode API is called */
    if(SIMLOCK_SUCCESS != simlock_fuse_blow_fuse())
    {
      ASSERT(0);
    }
  }
  else
  {
    return simlock_status;
  }

  /* this sets the device in the requested mode by calling the
     CM API. This function performs a sync wait until the response
     is received or the timer expires */
  return simlock_set_device_mode_sync(req_msg_ptr->mode);
} /* simlock_set_device_mode_request */


/*===========================================================================
FUNCTION SIMLOCK_CHECK_CK_STATUS

DESCRIPTION
  This function checks if a category is locked with or without CK

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_check_ck_status
(
  simlock_category_enum_type    category,
  simlock_slot_enum_type        slot,
  simlock_ck_present_type    *  ck_status_ptr
)
{
  simlock_result_enum_type    simlock_status       = SIMLOCK_SUCCESS;
  uint8                       slot_index           = 0;
  uint8                       category_index       = 0;
  simlock_config_data_type    config_file_data;

  memset(&config_file_data, 0x00, sizeof(simlock_config_data_type));

  if(ck_status_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  simlock_status = simlock_util_get_slot_index(slot, &slot_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get slot index failed for slot, 0x%x", slot);
    return SIMLOCK_GENERIC_ERROR;
  }

  simlock_status = simlock_util_get_category_index(category,
                                                   &category_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (category_index >= SIMLOCK_CATEGORY_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get category index failed for category, 0x%x",
                      category);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* read config file data */
  simlock_status = simlock_config_read_config_data(&config_file_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_0("read config file failed");
    return simlock_status;
  }

  *ck_status_ptr = config_file_data.category_status[slot_index][category_index].ck_present;

  return SIMLOCK_SUCCESS;
} /* simlock_check_ck_status */


/*===========================================================================
FUNCTION SIMLOCK_CHECK_CATEGORY_SUPPORTED_AND_ENABLED

DESCRIPTION
  This function checks if a category is supported and is currently enabled

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_check_category_supported_and_enabled
(
  simlock_category_enum_type  category,
  simlock_slot_enum_type      slot
)
{
  simlock_result_enum_type   simlock_status = SIMLOCK_SUCCESS;
  simlock_config_data_type   config_file_data;

  memset(&config_file_data, 0x00, sizeof(simlock_config_data_type));

  /* check if category is supported */
  simlock_status = simlock_util_check_category_supported(category);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  /* read config file data */
  simlock_status = simlock_config_read_config_data(&config_file_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  /* check if the category is enabled */
  if(!simlock_util_check_category_enabled(&config_file_data,
                                          category, slot))
  {
    SIMLOCK_MSG_ERR_1("category 0x%x not enabled", category);
    return SIMLOCK_INVALID_STATE;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_check_category_supported_and_enabled */


/*===========================================================================
FUNCTION SIMLOCK_ADD_LOCK_CODES_CK_REQUEST

DESCRIPTION
  This function processes the request to add lock codes to a category, which
  is already enabled. It reads the lock data of the requested category and
  checks if the CK matches. If it matches, the new lock codes are added to
  the existing codes and lock data is written to the category file.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_add_lock_codes_ck_request
(
  const simlock_add_lock_codes_ck_msg_req_type  * req_msg_ptr
)
{
  simlock_result_enum_type               simlock_status       = SIMLOCK_SUCCESS;
  simlock_category_file_data_type      * category_data_ptr    = NULL;
  simlock_ck_present_type                ck_status            = SIMLOCK_CK_NOT_PRESENT;
  simlock_hck_type                       hck_data_buf;

  if(req_msg_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_add_lock_codes_ck_request");

  memset(hck_data_buf, 0x00, SIMLOCK_HCK_LEN);

  simlock_status = simlock_check_category_supported_and_enabled(req_msg_ptr->category.category_type,
                                                                req_msg_ptr->slot);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  simlock_status = simlock_check_ck_status(req_msg_ptr->category.category_type,
                                           req_msg_ptr->slot,
                                           &ck_status);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  /* if the category is locked without CK, do not
     let additional codes with CK */
  if((ck_status == SIMLOCK_CK_NOT_PRESENT) &&
     (req_msg_ptr->ck.simlock_ck_length > 0))
  {
    SIMLOCK_MSG_ERR_1("category was securely locked without CK: 0x%x",
                      req_msg_ptr->category.category_type);
    return SIMLOCK_INCORRECT_KEY;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr,
                               sizeof(simlock_category_file_data_type));
  if(category_data_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* read the lock code header and data */
  simlock_status = simlock_category_read_lock_data(req_msg_ptr->category.category_type,
                                                   req_msg_ptr->slot,
                                                   category_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    /* this frees the code data in the category */
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    return simlock_status;
  }

  /* sanity check the file data */
  if(category_data_ptr->category_header.category_type !=
      req_msg_ptr->category.category_type)
  {
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    SIMLOCK_MSG_ERR_0("invalid lock data");
    return SIMLOCK_GENERIC_ERROR;
  }

  if(category_data_ptr->category_header.auto_lock)
  {
    SIMLOCK_MSG_ERR_0("autolock is enabled");
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    return SIMLOCK_INVALID_STATE;
  }

  if(req_msg_ptr->ck.simlock_ck_length > 0)
  {
    if(category_data_ptr->category_header.iteration_cnt == 0)
    {
      simlock_category_free_code_data(category_data_ptr);
      SIMLOCK_MEM_FREE(category_data_ptr);
      SIMLOCK_MSG_ERR_0("invalid lock data");
      return SIMLOCK_GENERIC_ERROR;
    }

    /* generate the HCK by running the PBKDF2 algo */
    simlock_status = simlock_crypto_run_pbkdf2_algorithm((const uint8 *)req_msg_ptr->ck.simlock_ck_data,
                                                         (uint16)req_msg_ptr->ck.simlock_ck_length,
                                                         category_data_ptr->category_header.salt,
                                                         SIMLOCK_SALT_LEN,
                                                         category_data_ptr->category_header.iteration_cnt,
                                                         hck_data_buf,
                                                         SIMLOCK_HCK_LEN);
    if(simlock_status != SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_ERR_1("hck generation failed, status: 0x%x", simlock_status);

      /* this frees the code data in the category */
      simlock_category_free_code_data(category_data_ptr);
      SIMLOCK_MEM_FREE(category_data_ptr);
      return simlock_status;
    }
  }

  /* compare the HCK generated with that in the category file */
  if((ck_status == SIMLOCK_CK_NOT_PRESENT) ||
     (memcmp(hck_data_buf, category_data_ptr->category_header.hck,
            SIMLOCK_HCK_LEN) == 0))
  {
    simlock_status = simlock_category_append_lock_data(&req_msg_ptr->category,
                                                       req_msg_ptr->blacklist,
                                                       category_data_ptr);
    if(simlock_status == SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_MED_0("key data matched, adding new codes...");

      /* write the category data */
      simlock_status = simlock_category_write_lock_data(req_msg_ptr->slot,
                                                        category_data_ptr);
    }
  }
  else
  {
    SIMLOCK_MSG_ERR_0("cannot add codes. mismatch in key data");
    simlock_status = SIMLOCK_INCORRECT_KEY;
  }

  /* this frees the code data in the category */
  simlock_category_free_code_data(category_data_ptr);
  SIMLOCK_MEM_FREE(category_data_ptr);

  return simlock_status;
} /* simlock_add_lock_codes_ck_request */


/*===========================================================================
FUNCTION SIMLOCK_ADD_LOCK_CODES_HCK_REQUEST

DESCRIPTION
  This function processes the request to add lock codes to a category, which
  is already enabled. It reads the lock data of the requested category and
  checks if the HCK matches. If it matches, the new lock codes are added to
  the existing codes and lock data is written to the category file.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_add_lock_codes_hck_request
(
  const simlock_add_lock_codes_hck_msg_req_type  * req_msg_ptr
)
{
  simlock_result_enum_type               simlock_status       = SIMLOCK_SUCCESS;
  simlock_category_file_data_type      * category_data_ptr    = NULL;

  if(req_msg_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_add_lock_codes_hck_request");

  simlock_status = simlock_check_category_supported_and_enabled(req_msg_ptr->category.category_type,
                                                                req_msg_ptr->slot);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr,
                               sizeof(simlock_category_file_data_type));
  if(category_data_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* read the lock code header and data */
  simlock_status = simlock_category_read_lock_data(req_msg_ptr->category.category_type,
                                                   req_msg_ptr->slot,
                                                   category_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    /* this frees the code data in the category */
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    return simlock_status;
  }

  /* sanity check the file data */
  if(category_data_ptr->category_header.category_type !=
      req_msg_ptr->category.category_type)
  {
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    SIMLOCK_MSG_ERR_0("invalid lock data");
    return SIMLOCK_GENERIC_ERROR;
  }

  if(category_data_ptr->category_header.auto_lock)
  {
    SIMLOCK_MSG_ERR_0("autolock is enabled");
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    return SIMLOCK_INVALID_STATE;
  }

  /* compare the HCK in the request with that in the category file */
  if(memcmp(req_msg_ptr->hck, category_data_ptr->category_header.hck,
            SIMLOCK_HCK_LEN) == 0)
  {
    simlock_status = simlock_category_append_lock_data(&req_msg_ptr->category,
                                                       req_msg_ptr->blacklist,
                                                       category_data_ptr);
    if(simlock_status == SIMLOCK_SUCCESS)
    {
      SIMLOCK_MSG_MED_0("hash key data matched, adding new codes...");

      /* write the category data */
      simlock_status = simlock_category_write_lock_data(req_msg_ptr->slot,
                                                        category_data_ptr);
    }
  }
  else
  {
    SIMLOCK_MSG_ERR_0("cannot add codes. mismatch in hash key data");
    simlock_status = SIMLOCK_INCORRECT_KEY;
  }

  /* this frees the code data in the category */
  simlock_category_free_code_data(category_data_ptr);
  SIMLOCK_MEM_FREE(category_data_ptr);

  return simlock_status;
} /* simlock_add_lock_codes_hck_request */


/*===========================================================================
FUNCTION SIMLOCK_POLICY_USES_SLOT1_CONFIG_FOR_ALL_SLOTS

DESCRIPTION
  This function checks whether slot policy uses slot1 configuration also for
  all additional slots.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE   : If policy uses slot1 config
  FALSE  : Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_policy_uses_slot1_config_for_all_slots
(
  simlock_slot_policy_enum_type slot_policy
)
{
  if(slot_policy == SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS ||
     slot_policy == SIMLOCK_SLOT_POLICY_VALID_CARD_MUST_ON_ANY_SLOT ||
     slot_policy == SIMLOCK_SLOT_POLICY_WAIT_FOR_FULL_SERVICE_ON_ANY_VALID_SLOT ||
     slot_policy == SIMLOCK_SLOT_POLICY_BLOCK_ALL_SLOTS_IF_ALL_VALID_SLOTS_EXPIRED)
  {
    return TRUE;
  }
  return FALSE;
}/* simlock_policy_uses_slot1_config_for_all_slots */

#endif /* FEATURE_SIMLOCK */