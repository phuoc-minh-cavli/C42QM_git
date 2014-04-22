/*
 * Copyright (c) 2018-2019, ams AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "sns_tmd2725_sensor.h"

static const sns_sensor_uid tmd2725_als_oc_suids[TMD2725_NUM_DEVICES] = {
    { .sensor_uid = { 0x2b, 0x44, 0xa0, 0xc8, 0xa1, 0xa6, 0x49, 0x38, 0x87, 0x75, 0xf9, 0xa0, 0x37, 0x79, 0xdc, 0xb2 }},
    { .sensor_uid = { 0x09, 0xeb, 0x21, 0x9e, 0x2d, 0x34, 0x4a, 0xf8, 0x9d, 0x5d, 0x91, 0x72, 0x73, 0x5a, 0xa8, 0x58 }}
};
static const sns_sensor_uid tmd2725_als_st_suids[TMD2725_NUM_DEVICES] = {
    { .sensor_uid = { 0xc0, 0x55, 0x83, 0xf4, 0x66, 0xa5, 0x40, 0x77, 0xa7, 0xe5, 0x58, 0x80, 0xf2, 0x6e, 0x23, 0x74 }},
    { .sensor_uid = { 0x0c, 0x15, 0x7f, 0xb4, 0x00, 0xb7, 0x45, 0x34, 0x8e, 0x6c, 0x7a, 0x70, 0x3b, 0xbb, 0x07, 0x25 }}
};
static const sns_sensor_uid tmd2725_prox_oc_suids[TMD2725_NUM_DEVICES] = {
    { .sensor_uid = { 0x3b, 0xe5, 0x7f, 0x1c, 0xa7, 0x8c, 0x43, 0x74, 0x90, 0x85, 0xad, 0x14, 0x1f, 0xc4, 0xe1, 0xd4 }},
    { .sensor_uid = { 0x96, 0x47, 0x8d, 0x81, 0x64, 0x85, 0x4c, 0x56, 0x87, 0xfc, 0x8a, 0xc4, 0xfd, 0xe3, 0xdf, 0x0a }}
};
static const sns_sensor_uid tmd2725_prox_st_suids[TMD2725_NUM_DEVICES] = {
    { .sensor_uid = { 0x20, 0x16, 0x45, 0xbc, 0xbb, 0x01, 0x44, 0xce, 0x92, 0x10, 0xd9, 0x90, 0x7a, 0x6a, 0xe8, 0x85 }},
    { .sensor_uid = { 0x09, 0xc1, 0x4a, 0x8a, 0xd7, 0x5b, 0x43, 0x3d, 0xa1, 0x5c, 0x4e, 0xe4, 0x2b, 0xd5, 0x60, 0xb1 }}
};

sns_sensor_uid const* tmd2725_get_als_oc_sensor_uid(sns_sensor const *const this)
{
  uint32_t hwid = this->cb->get_registration_index(this);
  return tmd2725_get_suid_with_sensortype_and_hwid(AMS_ALS_OC, hwid);
}

sns_sensor_uid const* tmd2725_get_als_strm_sensor_uid(sns_sensor const *const this)
{
  uint32_t hwid = this->cb->get_registration_index(this);
  return tmd2725_get_suid_with_sensortype_and_hwid(AMS_ALS_STRM, hwid);
}

sns_sensor_uid const* tmd2725_get_prox_oc_sensor_uid(sns_sensor const *const this)
{
  uint32_t hwid = this->cb->get_registration_index(this);
  return tmd2725_get_suid_with_sensortype_and_hwid(AMS_PROX_OC, hwid);
}

sns_sensor_uid const* tmd2725_get_prox_strm_sensor_uid(sns_sensor const *const this)
{
  uint32_t hwid = this->cb->get_registration_index(this);
  return tmd2725_get_suid_with_sensortype_and_hwid(AMS_PROX_STRM, hwid);
}

sns_sensor_uid const* tmd2725_get_suid_with_sensortype_and_hwid(ams_sensor_type type, int64_t hwid)
{
    if (type == AMS_ALS_OC)
    {
        return &tmd2725_als_oc_suids[hwid];
    }
    else if (type == AMS_ALS_STRM)
    {
        return &tmd2725_als_st_suids[hwid];
    }
    else if (type == AMS_PROX_OC)
    {
        return &tmd2725_prox_oc_suids[hwid];
    }
    else if (type == AMS_PROX_STRM)
    {
        return &tmd2725_prox_st_suids[hwid];
    }
    else
    {
        /* Should not happen, but we need to return something non-null */
        return &tmd2725_als_oc_suids[hwid];
    }
}

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
sns_sensor_api tmd2725_als_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &tmd2725_als_init,
  .deinit             = &tmd2725_als_deinit,
  .get_sensor_uid     = &tmd2725_get_als_oc_sensor_uid,
  .set_client_request = &tmd2725_set_client_request,
  .notify_event       = &tmd2725_sensor_notify_event,
};
sns_sensor_api tmd2725_als_streaming_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &tmd2725_als_streaming_init,
  .deinit             = &tmd2725_als_deinit,
  .get_sensor_uid     = &tmd2725_get_als_strm_sensor_uid,
  .set_client_request = &tmd2725_set_client_request,
  .notify_event       = &tmd2725_streaming_sensor_notify_event,
};
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
sns_sensor_api tmd2725_prox_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &tmd2725_prox_init,
  .deinit             = &tmd2725_prox_deinit,
  .get_sensor_uid     = &tmd2725_get_prox_oc_sensor_uid,
  .set_client_request = &tmd2725_set_client_request,
  .notify_event       = &tmd2725_sensor_notify_event,
};
sns_sensor_api tmd2725_prox_streaming_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &tmd2725_prox_streaming_init,
  .deinit             = &tmd2725_prox_deinit,
  .get_sensor_uid     = &tmd2725_get_prox_strm_sensor_uid,
  .set_client_request = &tmd2725_set_client_request,
  .notify_event       = &tmd2725_streaming_sensor_notify_event,
};
#endif
