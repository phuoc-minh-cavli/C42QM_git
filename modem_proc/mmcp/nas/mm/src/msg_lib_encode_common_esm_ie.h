
/*==============================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*Author: hnam
  Reference Document: 3GPP TS 24.301 V1.1.1 (2008-10)
  START DATE: 2/5/2009*/
/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/msg_lib_encode_common_esm_ie.h#1 $
$Author: pwbldsvc $
$DateTime: 2023/04/25 14:39:18 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
==============================================================================*/

#ifndef _MSG_LIB_ENCODE_ESM_COMMON_IE_H_
#define _MSG_LIB_ENCODE_ESM_COMMON_IE_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "comdef.h"
#include "lte_nas_emm_message_type.h"

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/* ------- ESM ENCODING ------ */

extern void encode_prot_conf_opt(protocol_cfg_options_T *prot_conf_opt, byte *ota_buf, word *pos, word *esm_container_len);
extern void encode_ext_prot_conf_opt(ext_protocol_cfg_options_T *ext_prot_conf_opt, byte *ota_buf, word *pos, word *esm_container_len,size_t ota_buf_len);
extern void encode_eps_qos(sdf_qos_T *eps_qos, byte *ota_buf, word *pos, word *esm_container_len);

extern void encode_esm_tft(tft_type_T *tft, byte *ota_buf, word *pos, word *esm_container_len);

#ifdef FEATURE_LTE_REL10
/*===========================================================================

FUNCTION  ENCODE_ESM_DEVICE_PROPERTIES_IE

DESCRIPTION
  This function encodes the DEVICE PROPERTIES information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void encode_esm_device_properties_ie
(
  boolean *low_priority_indicator,
  byte *ota_buf,
  word *pos,
  word *esm_container_len
);
#endif

#ifdef FEATURE_CIOT
/*------------------------------------------------
       HC CONFIGURATION IE
--------------------------------------------------*/
/*===========================================================================

FUNCTION  ENCODE_HC_CONFIG_IE

DESCRIPTION
  This function encodes the ENCODE_HC_CONFIG_IE information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_hc_config_ie(hc_config_T *hc_config, byte *ota_buf, word *pos, word *esm_container_len);
#endif
#endif

#endif /*FEATURE_LTE*/
