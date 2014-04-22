/*!
  @file
  

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/conn_establishment/lte_rrc_cep_test_asn1_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef LTE_RRC_TEST_CEP_ASN1_COMMON_H
#define LTE_RRC_TEST_CEP_ASN1_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include  "comdef.h"
#include  "lte_rrc_osys_asn1.h"
#include  "lte_rrc_osys_asn1util.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief External type for my clients
*/

 typedef struct
{
  uint32 msg_c1_choice;
  uint32 msg_choice;
  uint32 conn_req_r8_choice;
  uint8 est_cause;

}lte_rrc_cep_test_conn_req_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

   FUNCTION:  lte_rrc_test_cep_decode_conn_req

===========================================================================*/
/*!
    @brief
    Wrapper function to do decoding of ConnReq msg

    @detail

    @return
    

*/
/*=========================================================================*/

extern void lte_rrc_test_cep_decode_conn_req
(
  void *encoded_msg, 
  void *my_conn_req,
  uint16 length
);


#endif /* LTE_RRC_TEST_CEP_ASN1_COMMON_H */
