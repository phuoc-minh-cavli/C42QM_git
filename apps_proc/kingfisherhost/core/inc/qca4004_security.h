/*****************************************************************************
  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/

#ifndef __QCA4004_SECURITY_H__
#define __QCA4004_SECURITY_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "qapi_types.h"


/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
 * Initalize the MD5 context.
 *
 * @return
 * None.
 *
 * @dependencies MD5.
 */
void qca4004_md5_init(void);

/**
 * MD5 block update operation.  This function continues the MD5
 * operation, processing another message block and updating the context.
 
 * @param[in] data		Block of input.
 * @param[in] len		Length of the input block.

 * @return
 * None.
 *
 * @dependencies MD5.
 */
void qca4004_md5_update(uint8_t *data, uint32_t len);

/**
 * Final step of the MD5 operation.
 *
 * @param[out] md5_val[16]   Buffer where the digest is to be written.
 * @return
 * None.
 *
 * @dependencies MD5.
 */
void qca4004_md5_final(uint8_t md5_val[]);

/**
 * Calculates a 16-bit CRC over a specified number of data bits.
 *
 * @param[in] buf	Pointer to the bytes containing the data for the CRC.
 * @param[in] len	Number of data bytes over which to calculate the CRC.
 *
 * @return
 * A word holding 16 bits that are the contents of the CRC
 * register as calculated over the specified data bits.
 *
 * @dependencies CRC.
 */
uint16_t qca4004_crc16_calc(uint8_t *buf, uint16_t len);
#endif

