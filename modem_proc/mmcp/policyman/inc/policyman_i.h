#ifndef _POLICYMAN_I_H_
#define _POLICYMAN_I_H_

/**
  @file policyman_i.h

  @brief Internal interface to Policy Manager
*/

/*
    Copyright (c) 2013-2017 QUALCOMM Technologies Incorporated.
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/inc/policyman_i.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "mre.h"
#include "policyman.h"

typedef enum
{
  POLICYMAN_BASE_NONE,
  POLICYMAN_BASE_CONFIG,
  POLICYMAN_BASE_HARDWARE
} policyman_base_t;

typedef enum
{
  SUBS_NONE,
  SUBS_MMODE,
  SUBS_NON_MMODE,
  SUBS_THIS,
  SUBS_OTHER,
  SUBS_ALL,
} policyman_subs_t;

typedef enum
{
  POLICYMAN_CMP_INVALID,
  POLICYMAN_CMP_EQUALS,
  POLICYMAN_CMP_EXCEEDS,
  POLICYMAN_CMP_EQUALS_OR_EXCEEDS,
} policyman_comparator_t;

#define POLICYMAN_UNNAMED_OBJ         MRE_UNNAMED_OBJ
#define POLICYMAN_NAMED_PLMN_SET      MRE_NAMED_PLMN_SET
#define POLICYMAN_NAMED_RF_BANDS      MRE_NAMED_RF_BANDS
#define POLICYMAN_NAMED_TIMER         MRE_NAMED_TIMER
#define POLICYMAN_NAMED_MCC_SET       MRE_NAMED_MCC_SET
#define POLICYMAN_NAMED_BOOLEAN       MRE_NAMED_BOOLEAN
#define POLICYMAN_NAMED_CONFIG        MRE_NAMED_CONFIG
#define policyman_named_object_type_t mre_named_object_type_t


#define POLICYMAN_NUM_SUBS            MAX_AS_IDS

#define POLICYMAN_MAX_SUB       ((sys_modem_as_id_e_type) (POLICYMAN_NUM_SUBS + 1))
#define SUBS_ID_TO_NSIM(subsId) ((size_t) (subsId))
#define NSIM_TO_SUBS_ID(nSim)   ((sys_modem_as_id_e_type) (nSim))


/*  Opaque type definitions
 */
typedef struct policyman_config_info_t  policyman_config_info_t;
typedef struct policyman_cmd_t          policyman_cmd_t;
typedef struct policyman_policy_t       policyman_policy_t;
typedef struct policyman_state_t        policyman_state_t;
typedef struct policyman_timer_t        policyman_timer_t;
typedef struct policyman_condition_t    policyman_condition_t;
typedef struct policyman_action_t       policyman_action_t;

#define policyman_set_t                 mre_set_t
#define policyman_xml_element_t         mre_xml_element_t

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
typedef struct rf_bands_item_t          rf_bands_item_t;
#endif

typedef struct rf_bands_t               rf_bands_t; 


#define BASE_EXECUTE_CTX                  \
  policyman_state_t       *pState;        \
  policyman_set_t         *pItemSet;      \
  sys_modem_as_id_e_type   asubs_id;      \
  boolean                  haveService  

#if defined FEATURE_DISABLE_DYNAMIC_POLICIES
#define POLICYMAN_ITEM                  \
  _REF_CNT_OBJ;                         \
  policyman_config_info_t *pCfgInfo;    \
  sys_modem_as_id_e_type   asubs_id;    \
  size_t                   seqno
#endif

typedef struct
{
  BASE_EXECUTE_CTX;

} policy_execute_ctx_t;


#define PM_CHECK_PTR_GOTO(x, label)                      \
  if ((x) == NULL) {                                     \
    POLICYMAN_MSG_ERROR_0("Unexpected NULL pointer "#x); \
    goto label;                                          \
  }

#define PM_VALIDATE_GOTO(x, label)                        \
  if (!(x))                                               \
  {                                                       \
    POLICYMAN_MSG_ERROR_0("condition "#x " failed!");     \
    goto label;                                           \
  }                                                       \

void
policyman_init(
  void
  );


void
policyman_per_subs_init(
  size_t subs
  );

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

void
policyman_per_subs_deinit(
  size_t subs
  );


boolean
policyman_is_initializing(
  void
  );


boolean
policyman_is_test_mode(
  void
  );

boolean
policyman_is_FTM(
  void
  );

/*-------- policyman_set_test_mode --------*/
void
policyman_set_test_mode(
  boolean value
  );

/*-------- policyman_set_FTM --------*/
void
policyman_set_FTM(
  boolean value
  );

/*-------- policyman_retrieve_rats_bands --------*/
boolean
policyman_retrieve_rats_bands(
  sys_modem_as_id_e_type  subsId,
  uint32                  *pRatMask,
  sys_band_mask_type      *pGWBand,
  sys_lte_band_mask_e_type  *pLTEBand,
  sys_band_mask_type      *pTDSBand
);

/*-------- policyman_condition_mcc_dtor --------*/
void
policyman_condition_mcc_dtor(
  void  *pObj
  );

#endif /* _POLICYMAN_I_H_ */
