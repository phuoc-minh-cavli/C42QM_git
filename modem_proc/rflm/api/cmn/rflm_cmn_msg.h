/*!
  @file
  rflm_cmn_msg.h

  @brief
  RFLM CMN MSGS
 
  @addtogroup RFLM_API_COMMON
  @{
*/

/*===========================================================================

  Copyright (c) 2011-13 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rflm.mpss/1.10/api/cmn/rflm_cmn_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/13/14   zhw     Initial Revision

===========================================================================*/
#ifndef RFLM_CMN_MSG_H
#define RFLM_CMN_MSG_H

#include "rflm_msg.h"

/*! @brief Macro containing header and response payload */
#define RFLM_INTF_RSP_MSG( MSG_NAME )                 \
typedef struct {                                   \
  msgr_hdr_s                            msg_hdr;       \
  rflm_cmn_##MSG_NAME##_rsp             msg_rsp;   \
} rflm_cmn_##MSG_NAME##_rsp_t

/*! @brief Structure to send token for tuner AOL */
typedef struct
{
  /*! @brief token of antenna tuner script */
  uint32 token;
} rflm_cmn_tuner_aol_rsp;

RFLM_INTF_RSP_MSG(tuner_aol);

/*! @brief Structure to send qtf CL measurement for tuner AOL */
typedef struct
{
  /*! @brief token of antenna tuner script */
  uint32 token;
} rflm_cmn_tuner_cl_rsp;

RFLM_INTF_RSP_MSG(tuner_cl);

/*! @brief Structure to send asd token for antenna switch diversity command script */
typedef struct
{
  /*! @brief token of antenna tuner script */
  uint32 token;
} rflm_cmn_asd_token_rsp;

RFLM_INTF_RSP_MSG(asd_token);

/* MSGR enum */
enum {
  /* Response messages */
  MSGR_DEFINE_UMID( RFLM, CMN, RSP,  TUNER_AOL,         0x00, rflm_cmn_tuner_aol_rsp_t ),    /* Tuner AOL response token message */
  MSGR_DEFINE_UMID( RFLM, CMN, RSP,  TUNER_CL,          0x01, rflm_cmn_tuner_cl_rsp_t ),     /* Tuner CL resonse token message */ 
  MSGR_DEFINE_UMID( RFLM, CMN, RSP,  ASD,               0x02, rflm_cmn_asd_token_rsp_t )     /* ASD response token message */
};
 
#ifdef __cplusplus
} // extern "C"
#endif

#endif // RFLM_CMN_MSG_H
