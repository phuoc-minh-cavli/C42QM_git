/*****************************************************************************
  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#include <md5.h>
#include <crc.h>
#include "qca4004_security.h"

MD5_CTX qca4004_md5_ctx;

void qca4004_md5_init(void)
{
    memset(&qca4004_md5_ctx, 0, sizeof (MD5_CTX));
    MD5Init(&qca4004_md5_ctx);
}

void qca4004_md5_update(uint8_t *data, uint32_t len)
{
    MD5Update(&qca4004_md5_ctx, data, len);
}

void qca4004_md5_final(uint8_t md5_val[])
{
    uint8_t digest[16];
    int i;

    MD5Final(digest, &qca4004_md5_ctx);

    for(i=0;i<16;i++){
        md5_val[i]=digest[i];
    }
    return;
}

uint16_t qca4004_crc16_calc(uint8_t *buf, uint16_t len)
{
	uint16_t crc = 0;
	uint16_t seed = 0xffff;
	crc = crc_16_step(seed, buf, len);
	return ~crc;
}

