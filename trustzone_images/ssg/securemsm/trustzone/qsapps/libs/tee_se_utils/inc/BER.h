#ifndef BER_H_INCLUDED
#define BER_H_INCLUDED
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                                    <BER.h>
  DESCRIPTION
    Definitions and declarations for ASN.1 Basic Encoding Rules.

  Copyright (c) 2015-2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================
                                EDIT HISTORY

  $Header: //components/rel/ssg.tz/1.1.3/securemsm/trustzone/qsapps/libs/tee_se_utils/inc/BER.h#1 $

    when     who     what, where, why
  --------   ---     -------------------------------------------------------
  18/08/15   ADG     Initial version
  ==========================================================================*/

/*==========================================================================*
 *                             INCLUDE FILES                                *
 *==========================================================================*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
/*==========================================================================*
 *                              DATA TYPES                                  *
 *==========================================================================*/
typedef uint8_t  BEROctet;
typedef size_t   BERSize;

typedef uint32_t BERTag;
typedef BERSize  BERLength;

/** @brief Encoding for BER Tag values */
typedef enum
{
  /* First, or only, Tag octet */
  BERTAG_CLASS_MASK         = 0xC0,   /**< Mask to extract the CLASS bits */
  BERTAG_CLASS_UNIVERSAL    = 0x00,   /**< Tag CLASS: Universal */
  BERTAG_CLASS_APPLICATION  = 0x40,   /**< Tag CLASS: Application Specific */
  BERTAG_CLASS_CONTEXT      = 0x80,   /**< Tag CLASS: Context Specific */
  BERTAG_CLASS_PRIVATE      = 0xC0,   /**< Tag CLASS: Private */

  BERTAG_PC_MASK            = 0x20,   /**< Mask to extract the P/C bit */
  BERTAG_PC_PRIMITIVE       = 0x00,   /**< Primitive: No implication for TLV Value field */
  BERTAG_PC_CONSTRUCTED     = 0x20,   /**< Constructed: TLV Value field is a sequence of TLVs */

  BERTAG_TAG_MASK           = 0x1F,   /**< Mask to extract Tag value */
  BERTAG_TAG_EXTENDED       = 0x1F,   /**< Tag value extends to subsequent octets */

  /* Subsequent Tag octets */
  BERTAG_CONTD_MASK         = 0x80,   /**< Mask to extract continuation bit */
  BERTAG_CONTD_MORE         = 0x80,   /**< Tag continues into a further octet */
  BERTAG_CONTD_NOMORE       = 0x00,   /**< No more Tag octets to follow */

} BERTagBits;

/** @brief Encoding for BER Length values */
typedef enum
{
  /* Mask to extract the Short/Long Form bit */
  BERLEN_FORM_MASK          = 0x80,   /**< Mask to extract the Short/Long Form bit */
  BERLEN_FORM_SHORT         = 0x00,   /**< Short Form - length 0 to 127 */
  BERLEN_FORM_LONG          = 0x80,   /**< Long Form - length 0 to (256 ^ 127) - 1 */

  /* Short Form, encodes lengths up to 127 */
  BERLEN_SHORT_MASK_LENGTH  = 0x7F,   /**< Length value is 7 bits */

  /* Long Form, encodes length 0 to (256 ^ 127) - 1 */
  BERLEN_LONG_MASK_LENGTH   = 0x7F,   /**< Number of length bytes following */

  /* Indeterminate Form, length implicit in Value structure */
  BERLEN_INDETERMINATE      = 0x80,   /**< Length implicit in Value */

} BERLengthBits;

/*==========================================================================*
 *                              DEFINITIONS                                 *
 *==========================================================================*/

/**
 *  @brief  Extract a Tag value from a byte buffer in accordance with the
 *          ASN.1 Basic Encoding Rules.
 *
 *  @param  [in]      pInput        Pointer to the bytes from which the Tag
 *                                  is to be extracted.
 *  @param  [in,out]  pInputLength  The input length on entry;
 *                                  the number of bytes unconsumed on exit.
 *  @param  [out]     pTag          Pointer to the variable to receive the
 *                                  extracted Tag.
 *
 *  @return Pointer to the byte in the input buffer following the Tag,
 *          if successful, or NULL otherwise.
 */
const BEROctet *BERGetTag(const BEROctet *pInput,
                          BERSize        *pInputLength,
                          BERTag         *pTag);

/**
 *  @brief   Put a Tag value into a byte buffer in accordance with the
 *           ASN.1 Basic Encoding Rules.
 *
 *  @param   pOutput       [out]    Pointer to the buffer into which the Tag
 *                                  is to be copied.
 *  @param   pOutputLength [in,out] The output buffer length on entry;
 *                                  the number of bytes remaining on exit.
 *  @param   tag           [in]     Tag value to copy into the buffer.
 *
 *  @return  Pointer to the byte in the ouput buffer following the Tag,
 *           if successful, or NULL otherwise.
 */
BEROctet *BERPutTag(BEROctet *pOutput,
                    BERSize  *pOutputLength,
                    BERTag    tag);

/**
 *  @brief  Extract a Length value from a byte buffer in accordance with the
 *          ASN.1 Basic Encoding Rules.
 *
 *  @param  [in]      pInput        Pointer to the bytes from which the Length
 *                                  is to be extracted.
 *  @param  [in,out]  pInputLength  The input length on entry;
 *                                  the number of bytes unconsumed on exit.
 *  @param  [out]     pLength       Pointer to the variable to receive the
 *                                  extracted Length.
 *
 *  @return Pointer to the byte in the input buffer following the Length,
 *          if successful, or NULL otherwise.
 */
const BEROctet *BERGetLength(const BEROctet *pInput,
                             BERSize        *pInputLength,
                             BERLength      *pLength);

/**
 *  @brief  Put a Length value into a byte buffer in accordance with the
 *          ASN.1 Basic Encoding Rules.
 *
 *  @param  [out]     pOutput       Pointer to the buffer into which the Length
 *                                  is to be copied.
 *  @param  [in,out]  pOutputLength The output buffer length on entry;
 *                                  the number of bytes remaining on exit.
 *  @param  [in]      length        Length value to copy into the buffer.
 *
 *  @return Pointer to the byte in the ouput buffer following the Length,
 *          if successful, or NULL otherwise.
 */
BEROctet *BERPutLength(BEROctet  *pOutput,
                       BERSize   *pOutputLength,
                       BERLength  length);

/**
 *  @brief  Extract a TLV from a byte buffer in accordance with the
 *          ASN.1 Basic Encoding Rules.
 *
 *  @param  [in]      pInput        Pointer to the buffer from which the TLV
 *                                  is to be copied.
 *  @param  [in,out]  pInputLength  The input length on entry;
 *                                  the number of bytes unconsumed on exit.
 *  @param  [out]     pTag          Pointer to the variable to receive the
 *                                  extracted Tag.
 *  @param  [out]     pLength       Pointer to the variable to receive the
 *                                  extracted Length.
 *  @param  [out]     ppValue       Pointer to a pointer to receive the
 *                                  address of the Value.
 *
 *  @return Pointer to the byte in the ouput buffer following the TLV,
 *          if successful, or NULL otherwise.
 */
const BEROctet *BERGetTLV(const BEROctet  *pInput,
                          BERSize         *pInputLength,
                          BERTag          *pTag,
                          BERLength       *pLength,
                          const BEROctet **ppValue);

/**
 *  @brief  Put a TLV into a byte buffer in accordance with the
 *          ASN.1 Basic Encoding Rules.
 *
 *  @param  [out]     pOutput       Pointer to the buffer into which the TLV
 *                                  is to be copied.
 *  @param  [in,out]  pOutputLength The output buffer length on entry;
 *                                  the number of bytes remaining on exit.
 *  @param  [in]      tag           Tag value to copy into the buffer.
 *  @param  [in]      length        Length value to copy into the buffer.
 *  @param  [in]      pValue        Pointer to the Value to copy into the buffer.
 *
 * @return  Pointer to the byte in the ouput buffer following the TLV,
 *          if successful, or NULL otherwise.
 */
BEROctet *BERPutTLV(BEROctet       *pOutput,
                    BERSize        *pOutputLength,
                    BERTag          tag,
                    BERLength       length,
                    const BEROctet *pValue);

/*==========================================================================*
 *                              END OF FILE                                 *
 *==========================================================================*/
#ifdef __cplusplus
}
#endif

#endif /* BER_H_INCLUDED */

