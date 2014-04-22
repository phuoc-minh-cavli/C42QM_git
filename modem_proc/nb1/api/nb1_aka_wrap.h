/*!
  @file nb1_aka_wrap.h

  @brief
  Provides AKA wrapper functions for MILENAGE and XOR based algorithms

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef NB1_AKA_H
#define NB1_AKA_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>

#define NB1_AKA_ROOT_KEY_LEN 16 /*!<Size of Root Key*/
#define NB1_AKA_CK_IK_LEN 16 /*!< Length of CK and IK in bytes*/
#define NB1_AKA_MILENAGE_OP_LEN 16/*!< Operator Specific Configuration for MILENAGE*/
#define NB1_AKA_AUTN_LEN 16  /*!< AUTN length*/
#define NB1_AKA_RAND_LEN 16  /*!< RAND length*/
#define NB1_AKA_MILENAGE_RES_LEN 8 /*!< RES length is 8 bytes for MILENAGE*/
#define NB1_AKA_MILENAGE_AUTH_CNF_LEN 44 /*!< AUTH CNF length for MILENAGE*/
#define NB1_AKA_MILENAGE_ERR_SQN_AUTH_CNF_LEN 15 /*!< AUTH CNF length for MILENAGE SQN Error*/
#define NB1_AKA_MILENAGE_ERR_MAC_CNF_LEN 1 /*!< AUTH CNF length for MILENAGE MAC Error*/
#define NB1_AKA_MAX_AUTH_CNF_LEN 52 /*!< MAX AUTH CNF LEN*/


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
typedef enum
{
  NB1_AUTH_3G_UMTS_SUCCESS = 0xDB, /*!< Authentication Success*/
  NB1_AUTH_SYNCH_FAILURE = 0xDC,  /*!< Authentication Sync failure*/
  NB1_AUTH_ERROR_INCORRECT_MAC = 0xDD, /*!< Authentication Failure with Incorrect*/
  NB1_AUTH_INVALID = 0xFF, /*Invalid */
}nb1_auth_errno_e;
/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

  FUNCTION:  nb1_init_milenage_aka

===========================================================================*/
/*!
    @brief
    Should be called initially to initialize the AKA instance

    @return
    None

*/
/*=========================================================================*/


extern void nb1_init_milenage_aka(uint8 *root_key_ptr,/*!<Ptr to the Root key--K*/
                                  uint8 root_key_len,/*!< Length of the root key*/
                                  uint8 *op_ptr, /*!< Ptr to Operator specific config data*/
                                  uint8 op_len, /*!< Length of the OP parameter in bytes*/
                                  uint64 seq /*Highest seq received from network*/);


/*===========================================================================

  FUNCTION:  nb1_perform_milenage_aka

===========================================================================*/
/*!
    @brief
    Should be called to perform AKA authentication

    @return
    None

*/
/*=========================================================================*/


extern nb1_auth_errno_e nb1_perform_milenage_aka(uint8 *autn_ptr,/*!< Ptr to AUTN*/
                                                uint8 autn_len,/*!< Length of AUTN in bytes*/
                                                uint8 *rand_ptr,/*!< Ptr to rand */
                                                uint8 rand_len,/*!< Length of rand in bytes*/
                                                uint8 *cnf_ptr,/*!< Ptr to Auth cnf*/
                                                /*RES is 8 bytes starting from cnf_ptr[2]*/
                                                uint8 *cnf_len_ptr,/*!<Ptr to Len of CNF*/
                                                uint8 res_len/*!< Length of RES*/);


/*===========================================================================

  FUNCTION:  nb1_perform_xor_aka

===========================================================================*/
/*!
    @brief
    Should be called to perform XOR AKA authentication

    @return
    None

*/
/*=========================================================================*/

extern nb1_auth_errno_e nb1_perform_xor_aka(uint8 *root_key_ptr, /*!< Ptr of the root key*/
                                           uint8 root_key_len, /*!< Length of the root key*/
                                           uint8 *autn_ptr,/*!< Ptr to AUTN*/
                                           uint8 autn_len,/*!< Length of AUTN in bytes*/
                                           uint8 *rand_ptr,/*!< Ptr to rand*/
                                           uint8 rand_len,/*!< Length of rand in bytes*/
                                           uint8 *cnf_ptr,/*!<Ptr to Auth Cnf*/
                                           /*RES is 8 bytes starting from cnf_ptr[2]*/
                                           uint8 *cnf_len,/*!<Ptr to Len of CNF*/
                                           uint8 res_len/*!<RES length in bytes*/);

#define nb1_init_milenage_aka     nb1_init_milenage_aka
#define nb1_perform_milenage_aka  nb1_perform_milenage_aka
#define nb1_perform_xor_aka       nb1_perform_xor_aka

#endif/*NB1_AKA_H*/
