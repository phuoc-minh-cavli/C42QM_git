/*!
  @file
  lte_rrc_mh_asn1_test_util.c

  @brief
  File containing ASN1 utility functions that cannot be a part of the .cpp test
  file.
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/rrc_mh/lte_rrc_mh_asn1_test_util.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/09   amit    Replaced assert with proper error handling where invalid value 
                   is assigned to mandatory fields in ASN.1 decoded messages
09/15/08   amit    Initial Version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_mh_asn1_test_util.h"
#include "lte_static_rrc_mem_util.h"
#include <lte_assert.h>


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/





/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/




/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/

void *lte_rrc_mh_test_get_CDMA2000_CSFBParametersRequest(void)
{
  lte_rrc_osys_UL_DCCH_Message *ret_ptr;
  ret_ptr = (lte_rrc_osys_UL_DCCH_Message *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_UL_DCCH_Message));

  ret_ptr->message.t = T_lte_rrc_osys_UL_DCCH_MessageType_c1;
  ret_ptr->message.u.c1 = (lte_rrc_osys_UL_DCCH_MessageType_c1 *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_UL_DCCH_MessageType_c1));
  ret_ptr->message.u.c1->t = T_lte_rrc_osys_UL_DCCH_MessageType_c1_csfbParametersRequestCDMA2000;
  ret_ptr->message.u.c1->u.csfbParametersRequestCDMA2000 = (lte_rrc_osys_CSFBParametersRequestCDMA2000 *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CSFBParametersRequestCDMA2000));
#if 0
  ret_ptr->message.u.c1->u.csfbParametersRequestCDMA2000->criticalExtensions.t = T_lte_rrc_osys_CSFBParametersRequestCDMA2000_criticalExtensions_csfbParametersRequestCDMA2000_r8;
  ret_ptr->message.u.c1->u.csfbParametersRequestCDMA2000->criticalExtensions.u.csfbParametersRequestCDMA2000_r8 = (lte_rrc_osys_CSFBParametersRequestCDMA2000_r8_IEs *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CSFBParametersRequestCDMA2000_r8_IEs));
  ret_ptr->message.u.c1->u.csfbParametersRequestCDMA2000->criticalExtensions.u.csfbParametersRequestCDMA2000_r8->m.nonCriticalExtensionPresent = 0;
  
#endif  

  return ret_ptr;
}

void *lte_rrc_mh_test_get_ul_msg_enc_fail(void)
{
  lte_rrc_osys_UL_DCCH_Message *ret_ptr;
  ret_ptr = (lte_rrc_osys_UL_DCCH_Message *)lte_rrc_mem_calloc(sizeof(lte_rrc_osys_UL_DCCH_Message));

  ret_ptr->message.t = T_lte_rrc_osys_UL_DCCH_MessageType_c1;
  ret_ptr->message.u.c1 = (lte_rrc_osys_UL_DCCH_MessageType_c1 *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_UL_DCCH_MessageType_c1));
  ret_ptr->message.u.c1->t = T_lte_rrc_osys_UL_DCCH_MessageType_c1_csfbParametersRequestCDMA2000;
  ret_ptr->message.u.c1->u.csfbParametersRequestCDMA2000 = (lte_rrc_osys_CSFBParametersRequestCDMA2000 *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CSFBParametersRequestCDMA2000));
#if 0
  ret_ptr->message.u.c1->u.csfbParametersRequestCDMA2000->criticalExtensions.t = 0;
#endif

  return ret_ptr;
}

long lte_rrc_mh_test_get_CDMA2000_CSFBParametersResponse(unsigned char **encoded_msg)
{
  lte_rrc_osys_DL_DCCH_Message *dl_msg_ptr;
  lte_rrc_pdu_buf_s encoded_pdu_buf;
  int encode_status;
  dl_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_mem_calloc(sizeof(lte_rrc_osys_DL_DCCH_Message));

  dl_msg_ptr->message.t = T_lte_rrc_osys_DL_DCCH_MessageType_c1;
  dl_msg_ptr->message.u.c1 = (lte_rrc_osys_DL_DCCH_MessageType_c1 *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_DL_DCCH_MessageType_c1));
  dl_msg_ptr->message.u.c1->t = T_lte_rrc_osys_DL_DCCH_MessageType_c1_csfbParametersResponseCDMA2000;
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000 = (lte_rrc_osys_CSFBParametersResponseCDMA2000 *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CSFBParametersResponseCDMA2000));
#if 0
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->rrc_TransactionIdentifier = 2;
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.t = T_lte_rrc_osys_CSFBParametersResponseCDMA2000_criticalExtensions_csfbParametersResponseCDMA2000_r8;
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8 = (lte_rrc_osys_CSFBParametersResponseCDMA2000_r8_IEs *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CSFBParametersResponseCDMA2000_r8_IEs));
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8->m.nonCriticalExtensionPresent = 0;
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8->rand_.numbits = 32;
  *(dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8->rand_.data) = 4;
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8->mobilityParameters.numocts = 1;
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8->mobilityParameters.data = (unsigned char *)lte_rrc_asn1_osys_IE_malloc(sizeof(char));
#endif

  encode_status = lte_rrc_osys_asn1_encode_pdu(dl_msg_ptr, &encoded_pdu_buf, DL_DCCH_Message);
  lte_rrc_mem_free(dl_msg_ptr);
  //lte_rrc_mem_free(dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8->rand.value);
  //lte_rrc_mem_free(dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8->mobilityParameters.value);
  //lte_rrc_mem_free(dl_msg_ptr);


  *encoded_msg = encoded_pdu_buf.value;

  return encoded_pdu_buf.length;



}

long lte_rrc_mh_test_get_CDMA2000_CSFBParametersResponse_encoding_fail(unsigned char **encoded_msg)
{
  lte_rrc_osys_DL_DCCH_Message *dl_msg_ptr;
  lte_rrc_pdu_buf_s encoded_pdu_buf;
  int encode_status;
  dl_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_mem_calloc(sizeof(lte_rrc_osys_DL_DCCH_Message));

  dl_msg_ptr->message.t = T_lte_rrc_osys_DL_DCCH_MessageType_c1;
  dl_msg_ptr->message.u.c1 = (lte_rrc_osys_DL_DCCH_MessageType_c1 *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_DL_DCCH_MessageType_c1));
  dl_msg_ptr->message.u.c1->t = T_lte_rrc_osys_DL_DCCH_MessageType_c1_csfbParametersResponseCDMA2000;
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000 = (lte_rrc_osys_CSFBParametersResponseCDMA2000 *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CSFBParametersResponseCDMA2000));
  #if 0
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->rrc_TransactionIdentifier = 2;
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.t = T_lte_rrc_osys_CSFBParametersResponseCDMA2000_criticalExtensions_csfbParametersResponseCDMA2000_r8;
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8 = (lte_rrc_osys_CSFBParametersResponseCDMA2000_r8_IEs *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_CSFBParametersResponseCDMA2000_r8_IEs));
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8->m.nonCriticalExtensionPresent = 0;
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8->rand_.numbits = 4;
  *(dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8->rand_.data) = 4;
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8->mobilityParameters.numocts = 1;
  dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8->mobilityParameters.data = (unsigned char *)lte_rrc_asn1_osys_IE_malloc(sizeof(char));
#endif

  encode_status = lte_rrc_osys_asn1_encode_pdu(dl_msg_ptr, &encoded_pdu_buf, DL_DCCH_Message);

  //lte_rrc_mem_free(dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8->rand.value);
  //lte_rrc_mem_free(dl_msg_ptr->message.u.c1->u.csfbParametersResponseCDMA2000->criticalExtensions.u.csfbParametersResponseCDMA2000_r8->mobilityParameters.value);
  //lte_rrc_mem_free(dl_msg_ptr);


  *encoded_msg = encoded_pdu_buf.value;

  return encoded_pdu_buf.length;



}


/*===========================================================================

                                UNIT TEST

===========================================================================*/

