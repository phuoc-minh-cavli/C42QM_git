/*!
  @file
  nb1.h

  @brief
  This file contains definitions for the Common type and constants required
  by different modules within NB1 Access Stratum(AS) as well as NB1 Non-Access
  Stratum(NAS)

*/

/*=============================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
=============================================================================*/

#ifndef NB1_H
#define NB1_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/

#include <comdef.h>
#include <IxErrno.h>
#include <lte.h>

/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/

/*! @brief Maximum Number of EPS Ids
*/
#define NB1_MAX_EPS 16

/*! @brief Size of ciphering/integrity key in bytes
*/
#define NB1_CIPHER_INTEGRITY_KEY_LEN 16

/*! @brief Size of the generator key in bytes
*/
#define NB1_GENERATOR_KEY_LEN 32

/*! @brief Size of the MAC-I (Integrity message digest) in bytes
*/
#define NB1_MACI_LEN 4

/*!
** REX signals used by NB1.
*/
#define NB1_NV_READ_SIG 0x0001

/*! @brief UE Category
*/
#define NB1_UE_CATEGORY                                      4

/*! @brief Maximum active eMBMS sessions
*/
#define NB1_EMBMS_MAX_ACTIVE_SESSIONS 32

/*! @brief Maximum available eMBMS sessions for now
  (Thereotical max sessions are 3472 -> (8 MBSFN areas * 15 PMCHs * 29 LC IDs) - 8 MCCHs)
*/
#define NB1_EMBMS_MAX_AVAIL_SESSIONS 128

/*=============================================================================

                   EXTERNAL ENUMERATION TYPES

=============================================================================*/

/*! @brief Enumeration for LTE 3GPP Spec version
*/
typedef lte_3gpp_release_ver_e nb1_3gpp_release_ver_e;

/*=============================================================================

                   EXTERNAL STRUCTURE/UNION TYPES

=============================================================================*/



/*=============================================================================

                   DATA TYPES

=============================================================================*/

/*! @brief error codes used within NB1 Access Stratum
*/
typedef errno_enum_type nb1_errno_e;

/*! @brief This defines the EPS identity as described in 36.331(RRC)
*/
typedef uint8 nb1_eps_id_t;

/*! @brief REQUIRED brief description of this structure typedef
  This type is used for the active EPS bearer indication sent from
  RRC to NAS and for loopback testing
*/
typedef struct
{
  nb1_eps_id_t eps_id;   /*!< EPS bearer ID */
  uint8 rb_id;   /*!< RB ID */

} nb1_rb_eps_bearer_map_info_s;

#endif /* NB1_H */

