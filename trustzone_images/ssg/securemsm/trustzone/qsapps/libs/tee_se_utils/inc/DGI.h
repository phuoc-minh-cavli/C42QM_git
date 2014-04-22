#ifndef DGI_H_INCLUDED
#define DGI_H_INCLUDED
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                                    <DGI.h>
  DESCRIPTION
    Definitions and declarations for Data Grouping Identifier coding.

  Copyright (c) 2015-2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================
                                EDIT HISTORY

  $Header: //components/rel/ssg.tz/1.1.3/securemsm/trustzone/qsapps/libs/tee_se_utils/inc/DGI.h#1 $

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
typedef uint8_t  DGIOctet;
typedef size_t   DGISize;

typedef uint32_t DGITag;
typedef DGISize  DGILength;

#define DGI_TAG_BYTES(t)            (DGIOctet) (((t) >> 8) & 0xFF), \
                                    (DGIOctet) (((t) >> 0) & 0xFF)

/*==========================================================================*
 *                              DEFINITIONS                                 *
 *==========================================================================*/

/**
 *  @brief  Extract a Tag value from a byte buffer in accordance with the
 *          DGI Encoding Rules.
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
const DGIOctet *DGIGetTag(const DGIOctet *pInput,
                          DGISize        *pInputLength,
                          DGITag         *pTag);

/**
 *  @brief   Put a Tag value into a byte buffer in accordance with the
 *           DGI Encoding Rules.
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
DGIOctet *DGIPutTag(DGIOctet *pOutput,
                    DGISize  *pOutputLength,
                    DGITag    tag);

/**
 *  @brief  Extract a Length value from a byte buffer in accordance with the
 *          DGI Encoding Rules.
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
const DGIOctet *DGIGetLength(const DGIOctet *pInput,
                             DGISize        *pInputLength,
                             DGILength      *pLength);

/**
 *  @brief  Put a Length value into a byte buffer in accordance with the
 *          DGI Encoding Rules.
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
DGIOctet *DGIPutLength(DGIOctet  *pOutput,
                       DGISize   *pOutputLength,
                       DGILength  length);

/**
 *  @brief  Extract a TLV from a byte buffer in accordance with the
 *          DGI Encoding Rules.
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
const DGIOctet *DGIGetTLV(const DGIOctet  *pInput,
                          DGISize         *pInputLength,
                          DGITag          *pTag,
                          DGILength       *pLength,
                          const DGIOctet **ppValue);

/**
 *  @brief  Put a TLV into a byte buffer in accordance with the
 *          DGI Encoding Rules.
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
DGIOctet *DGIPutTLV(DGIOctet       *pOutput,
                    DGISize        *pOutputLength,
                    DGITag          tag,
                    DGILength       length,
                    const DGIOctet *pValue);

/*==========================================================================*
 *                              END OF FILE                                 *
 *==========================================================================*/
#ifdef __cplusplus
}
#endif

#endif /* DGI_H_INCLUDED */

