/*!
  @file nb1_security.h

  @brief
  The commmon security header file for NB1

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

#ifndef NB1_SECURITY_H
#define NB1_SECURITY_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <IxErrno.h>
#include <nb1.h>
#include <dsm_item.h>
#include <a2_common.h>

/*===========================================================================

                   MACROS

===========================================================================*/


/*===========================================================================

MACRO MUTILS_HTONL()

DESCRIPTION
  Converts host-to-network long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The network byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -emacro(572,htonl) The bit shifting is correct*/
#define mutils_htonl(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
        (((uint32)(x) & 0x0000ff00U) <<  8) | \
        (((uint32)(x) & 0x00ff0000U) >>  8) | \
        (((uint32)(x) & 0xff000000U) >> 24)))

/*===========================================================================

MACRO MUTILS_NTOHL()

DESCRIPTION
  Converts network-to-host long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define mutils_ntohl(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
        (((uint32)(x) & 0x0000ff00U) <<  8) | \
        (((uint32)(x) & 0x00ff0000U) >>  8) | \
        (((uint32)(x) & 0xff000000U) >> 24)))


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Ciphering Algorithm
*/
typedef enum
{
  /*! No Ciphering; equivalent to eea0 */
  NB1_CIPHER_ALGO_NONE    = A2_CIPHER_ALGO_NONE,
  /*! SNOW-3G; equivalent to eea1 */
  NB1_CIPHER_ALGO_SNOW3G  = A2_CIPHER_ALGO_LTE_SNOW3G,
  /*! AES; equivalent to eea2 */
  NB1_CIPHER_ALGO_AES     = A2_CIPHER_ALGO_AES,
  /*!< NB1 ZUC; equivalent to eea3 or spare5 */
  NB1_CIPHER_ALGO_ZUC     = A2_CIPHER_ALGO_ZUC
} nb1_cipher_algo_e;

/*! @brief Integrity Algorithm
*/
typedef enum
{
  /*!< For initialization usage */
  NB1_INTEGRITY_ALGO_NONE    = A2_INTEGRITY_ALGO_NONE,
  /*!< SNOW-3G; equivalent to eia1 */
  NB1_INTEGRITY_ALGO_SNOW3G  = A2_INTEGRITY_ALGO_LTE_SNOW3G,
  /*!< AES; equivalent to eia2 */
  NB1_INTEGRITY_ALGO_AES     = A2_INTEGRITY_ALGO_AES,
  /*!< NB1 AES; equivalent to eia3-v11xy or spare5 */
  NB1_INTEGRITY_ALGO_ZUC     = A2_INTEGRITY_ALGO_ZUC
} nb1_integrity_algo_e;
typedef  errno_enum_type nb1_sec_errno_e;

typedef enum
{
  NB1_SECURITY_ALGO_NAS_ENC = 0x01 , /*!< NAS Encryption Algo*/
  NB1_SECURITY_ALGO_NAS_INT = 0x02 , /*!< NAS Integrity Algo*/
  NB1_SECURITY_ALGO_RRC_ENC = 0x03 , /*!< RRC Encryption Algo*/
  NB1_SECURITY_ALGO_RRC_INT = 0x04 , /*!< RRC Integrity Algo*/
  NB1_SECURITY_ALGO_UP_ENC = 0x05 , /*!< NAS Integrity Algo*/
}nb1_security_algorithm_distinguisher_e;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

  FUNCTION:  nb1_security_stream_cipher

===========================================================================*/
/*!
    @brief
    Does ciphering on the input data stream
    @return
    nb1_sec_errno_e

 */
/*=========================================================================*/


extern nb1_sec_errno_e nb1_security_stream_cipher
(
  nb1_cipher_algo_e         algo, /*!<Ciphering Algorithm  */
  uint8                     key[NB1_CIPHER_INTEGRITY_KEY_LEN],/*!< Input key*/
  uint8                     *in_msg_ptr, /*!< Ptr to the input msg*/
  uint16                    in_msg_byte_len, /*!< Input Msg length in bytes*/
  uint8                     *out_msg_ptr, /*!< Output msg ptr*/
  uint8                     bearer, /*!< Bearer ID ranges from 0 to 31*/
  uint32                    count /*!< Count*/
);


/*===========================================================================

  FUNCTION:  nb1_security_stream_decipher

===========================================================================*/
/*!
    @brief
    Does deciphering on the input data stream
    @return
    nb1_sec_errno_e

 */
/*=========================================================================*/

extern nb1_sec_errno_e nb1_security_stream_decipher
(
  nb1_cipher_algo_e         algo, /*!<Ciphering Algorithm  */
  uint8                     key[NB1_CIPHER_INTEGRITY_KEY_LEN],/*!< Input key*/
  uint8                     *in_msg_ptr, /*!< Ptr to the input msg*/
  uint16                    in_msg_byte_len,/*!< Input Msg length in bytes*/
  uint8                     *out_msg_ptr,/*!< Output msg ptr*/
  uint8                     bearer,/*!< Bearer ID ranges from 0 to 31*/
  uint32                    count /*!< Count*/

);

/*===========================================================================

  FUNCTION:  nb1_security_dsm_cipher

===========================================================================*/
/*!
    @brief
    Does ciphering on the input DSM chain. The *out_msg_byte_len_ptr value
    should match the packet length in the DSM item.
    @return
    nb1_sec_errno_e

 */
/*=========================================================================*/

extern nb1_sec_errno_e nb1_security_dsm_cipher
(
  nb1_cipher_algo_e         algo,/*!<Ciphering Algorithm  */
  uint8                     key[NB1_CIPHER_INTEGRITY_KEY_LEN],/*!< Input key*/
  dsm_item_type             **in_msg_ptr, /*! < Input DSM item ** */
  dsm_item_type             **out_msg_ptr,/*! < Output DSM item ** */
  uint16                    *out_msg_byte_len_ptr,/*!< Ptr to the output msg length*/
  uint8                     bearer,/*!< Bearer ID ranges from 0 to 31*/
  uint32                    count/*!< Count*/

);

/*===========================================================================

  FUNCTION:  nb1_security_dsm_decipher

===========================================================================*/
/*!
    @brief
    Does deciphering on the input DSM chain
    @return
    nb1_sec_errno_e

 */
/*=========================================================================*/

extern nb1_sec_errno_e nb1_security_dsm_decipher
(
  nb1_cipher_algo_e         algo,/*!<Ciphering Algorithm  */
  uint8                     key[NB1_CIPHER_INTEGRITY_KEY_LEN],/*!< Input key*/
  dsm_item_type             **in_msg_ptr,/*! < Input DSM item ** */
  dsm_item_type             **out_msg_ptr,/*! < Output DSM item ** */
  uint16                    *out_msg_byte_len_ptr,/*!< Ptr to the output msg length*/
  uint8                     bearer,/*!< Bearer ID ranges from 0 to 31*/
  uint32                    count/*!< Count*/
);

/*===========================================================================

  FUNCTION:  nb1_security_stream_compute_integrity_maci

===========================================================================*/
/*!
    @brief
    Computes MAC-I on the input data stream
    @return
    nb1_sec_errno_e

 */
/*=========================================================================*/
extern nb1_sec_errno_e nb1_security_stream_compute_integrity_maci
(
  nb1_integrity_algo_e      algo,/*!<Integrity Algorithm  */
  uint8                     key[NB1_CIPHER_INTEGRITY_KEY_LEN],/*!< Input key*/
  uint8                     *in_msg_ptr,/*! < Input Msg ptr  */
  uint16                    in_msg_byte_len, /*! < Input msg len  */
  uint32*                   maci_ptr,/*!<Output MAC-I ptr */
  uint8                     bearer,/*!< Bearer ID ranges from 0 to 31*/
  uint32                    count,/*!< Count*/
  boolean                   direction /*!< UL = 0, DL = 1*/
);
/*===========================================================================

  FUNCTION:  nb1_security_stream_compute_integrity_maci_non_octet

===========================================================================*/
/*!
    @brief
    Computes MAC-I on the input data stream non octet
    @return
    nb1_sec_errno_e

 */
/*=========================================================================*/
extern nb1_sec_errno_e nb1_security_stream_compute_integrity_maci_non_octet
(
  nb1_integrity_algo_e      algo,/*!<Integrity Algorithm  */
  uint8                     key[NB1_CIPHER_INTEGRITY_KEY_LEN],/*!< Input key*/
  uint8                     *in_msg_ptr,/*! < Input Msg ptr  */
  uint16                    in_msg_bit_len, /*! < Input msg len  */
  uint32*                   maci_ptr,/*!<Output MAC-I ptr */
  uint8                     bearer,/*!< Bearer ID ranges from 0 to 31*/
  uint32                    count,/*!< Count*/
  boolean                   direction /*!< UL = 0, DL = 1*/
);

/*===========================================================================

  FUNCTION:  nb1_security_dsm_compute_integrity_maci

===========================================================================*/
/*!
    @brief
    Computes MAC-I on the input DSM chain
    @return
    nb1_sec_errno_e

 */
/*=========================================================================*/

extern nb1_sec_errno_e nb1_security_dsm_compute_integrity_maci
(
  nb1_integrity_algo_e      algo,/*!<Integrity Algorithm  */
  uint8                     key[NB1_CIPHER_INTEGRITY_KEY_LEN],/*!< Input key*/
  dsm_item_type             *in_msg_dsm_ptr,/*! < Input DSM item ptr */
  uint32                    *maci_ptr, /*!<Output MAC-I ptr */
  uint8                     bearer,/*!< Bearer ID ranges from 0 to 31*/
  uint32                    count, /*!< Count*/
  boolean                   direction /*!< UL = 0, DL = 1*/
);

/*===========================================================================

  FUNCTION:  nb1_security_generate_key

===========================================================================*/
/*!
    @brief
    Function to generate the key using HMAC-SHA-256. The output key length
    should be 256(NB1_GENERATOR_KEY_LEN)for HMAC-SHA-256.

    @return
    nb1_sec_errno_e

 */
/*=========================================================================*/



extern nb1_sec_errno_e nb1_security_generate_key
(
  uint8                     in_key[NB1_GENERATOR_KEY_LEN], /*!< Input key*/
  uint8                     *in_string_ptr, /*!< Input string ptr */
  uint8                     *in_string_len, /*!< Input string length*/
  uint8                     *out_key /*!< Output key ptr*/
);

/*===========================================================================

  FUNCTION:  nb1_security_generate_algorithm_key

===========================================================================*/
/*!
    @brief
    Function to generate the algorithm key

    @return
    nb1_sec_errno_e

 */
/*=========================================================================*/
extern nb1_sec_errno_e nb1_security_generate_algorithm_key
(
  uint8 in_key[NB1_GENERATOR_KEY_LEN], /*!< Input key*/
  nb1_security_algorithm_distinguisher_e algorithm_distinguisher,
  /*!< Algorithm distinguisher*/
  uint8 algorithm_type, /*!< Algorithm type*/
  uint8 *out_key_ptr /*!< Output key ptr The length of the optput key is
                          NB1_CIPHER_INTEGRITY_KEY_LEN */
);

#endif /* NB1_SECURITY_H */
