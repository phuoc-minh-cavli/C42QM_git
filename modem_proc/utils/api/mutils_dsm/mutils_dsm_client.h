/*!
  @file
  mutils_dsm_client.h

  @brief
  This file contains data sturcture used by a mutils_dsm client


*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/utils.mpss/2.3.4/api/mutils_dsm/mutils_dsm_client.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/17   cx      Added RLCDL as the client
01/19/17   cx      Added IPAUL as the client
05/16/16   cx      Initial version
===========================================================================*/

#ifndef MUTILS_DSM_CLIENT_H
#define MUTILS_DSM_CLIENT_H

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*!
    @brief
    The client id type
*/
typedef enum
{
  MUTILS_DSM_CLIENT_UDC = 0x00,
  MUTILS_DSM_CLIENT_ROHC = 0x01,
  MUTILS_DSM_CLIENT_IPAUL = 0x02,
  MUTILS_DSM_CLIENT_IPA_UTILS_QBAP = 0x03,
  MUTILS_DSM_CLIENT_OTHER = 0x04,
  MUTILS_DSM_CLIENT_NR5G_RLCDL = 0x05,
  MUTILS_DSM_CLIENT_RLCDL = 0x06,
  MUTILS_DSM_CLIENT_PS = 0x07,
  MUTILS_DSM_CLIENT_DSS = 0x08,
  MUTILS_DSM_CLIENT_MAX
}mutils_dsm_client_e;

#endif /* MUTILS_DSM_CLIENT_H */
