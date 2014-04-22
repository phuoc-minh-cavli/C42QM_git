/*!
  @file
  ipa_vote.h

  @brief
  This file contain the API of Q6 IPA driver for the vote feature.

*/
/*===========================================================================

  Copyright (c) 2018 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/
/*===========================================================================*/
#ifndef VS_IPA_VOTE_STUB_H
#define VS_IPA_VOTE_STUB_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include "vs_ipa_stub.h"

/*==============================================================================

                           EXTERNAL TYPE DEFINITIONS

==============================================================================*/
/*! @brief vote client type */
typedef enum {
  IPA_VOTE_CLIENT_TYPE_AUDIO_UL = 0x0,
  IPA_VOTE_CLIENT_TYPE_AUDIO_DL = 0x1,
  IPA_VOTE_CLIENT_TYPE_MHI = 0x2,
  IPA_VOTE_CLIENT_TYPE_MAX
} ipa_vote_client_type_e;

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS

==============================================================================*/
/*=============================================================================

  FUNCTION:  ipa_vote

=============================================================================*/
/*!
    @brief
    This function is used to vote IPA and set parameter for this transfer session.

    @param client_type - client type
    @param options - 0 - no flags (for future growth)
    
    @return
    @retval IPA_SUCCESS        On success
    @retval IPA_ERR_FAIL       On unrecoverable error

    @dependencies
    This may be called only after IPA completed init.
*/
ipa_err_code_e ipa_vote(ipa_vote_client_type_e client_type, uint32 options);

/*=============================================================================

  FUNCTION:  ipa_devote

=============================================================================*/
/*!
    @brief
    This function is used to devote IPA and close this transfer session.

    @param client_type - client type

    @return
    @retval IPA_SUCCESS        On success
    @retval IPA_ERR_FAIL       On unrecoverable error

    @dependencies
    This may be called only after IPA is clocked, calling ipa_dma_vote().
    Caller should ensure that this precondition is met.
*/
ipa_err_code_e ipa_devote(ipa_vote_client_type_e client_type);

#endif
