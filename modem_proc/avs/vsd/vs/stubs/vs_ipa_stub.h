/*!
  @file
  ipa.h

  @brief
  This file contains definitions of all type definitions and APIs available
  to external modules.


*/
/*===========================================================================

  Copyright (c) 2013-18 QUALCOMM Technologies Incorporated. All Rights Reserved

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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.*/


#ifndef VS_IPA_STUB_H
#define VS_IPA_STUB_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
/*===========================================================================

                              MACROS

===========================================================================*/

/*===========================================================================

                     COMMON DEFINITIONS and TYPES

===========================================================================*/
/*! @brief IPA return error codes */
typedef enum
{
  /*! Common Error Codes */
  IPA_SUCCESS = 0,
  IPA_ERR_INVALID_ARGUMENT,
  IPA_ERR_LOGICAL_SIO_STREAM_UNKNOWN,
  IPA_ERR_PHYSICAL_SIO_STREAM_UNKNOWN,
  IPA_ERR_PERIPHERAL_PIPES_NOT_FOUND,
  IPA_ERR_NOT_SUPPORTED,
  /*! UL Error Codes */
  IPA_ERR_UL_AGGR_HDR_TYPE_NOT_SUPPORTED,
  IPA_ERR_UL_AGGR_PARAMS_INCORRECT,
  /*! DL Error Codes */
  IPA_ERR_DL_AGGR_HDR_TYPE_NOT_SUPPORTED,
  IPA_ERR_DL_AGGR_PARAMS_INCORRECT,
  IPA_ERR_DL_HDR_TYPE_NOT_SUPPORTED,
  /*! IP Filtering Error Codes */
  IPA_ERR_IPFLTR_COMMIT_FAIL,
  IPA_ERR_IPFLTR_RULE_TBL_PENDING_ACTIVATION,
  IPA_ERR_IPFLTR_RULE_TBL_PENDING_REMOTE_ACTIVATION,
  IPA_ERR_IPFLTR_REMOVE_FAIL,
  IPA_ERR_FLTR_TBL_OVERFLOW,
  /*! DMA error */
  IPA_ERR_DMA_CH_FULL,
  /*! Generic error */
  IPA_ERR_FAIL,
  IPA_ERR_BEARER_STATE_NO_CHANGE,
  IPA_ERR_SSR_INPROGRESS,
  IPA_ERR_CODE_MAX,

} ipa_err_code_e;

#endif /* IPA_H */
