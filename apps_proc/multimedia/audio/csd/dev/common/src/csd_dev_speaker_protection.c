/*-----------------------------------------------------------------------------
Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
Edit History
$Header: //components/rel/audio.tx/2.0/audio/csd/dev/common/src/csd_dev_speaker_protection.c#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/20/13   TBK     Initial revision
===============================================================================*/
#include "csd_dev_speaker_protection.h"

int32_t csd_dev_fb_spkr_prot_is_fb_device(uint32_t dev_id )
{
  return CSD_EUNSUPPORTED;
}

int32_t csd_dev_fb_spkr_prot_get_tx_device_record(const csd_dev_record_t *rx_device_record,
  csd_dev_record_t **fb_tx_device_record )
{
  return CSD_EUNSUPPORTED;
}

void csd_dev_fb_spkr_prot_device_enabled(int32_t dev_id)
{
}

void csd_dev_fb_spkr_prot_device_disabled(int32_t dev_id)
{
}

int32_t csd_dev_fb_spkr_prot_init_cfg_params(struct csd_dev_entry *new_dev, 
                                             csd_dev_spkr_calib_ctrl_t* spkr_calib_cfg)
{
  return CSD_EUNSUPPORTED;
}


int32_t csd_dev_fb_spkr_perform_calibration(int32_t t0)
{
  return CSD_EUNSUPPORTED;
}

int32_t csd_dev_fb_spkr_prot_init(void)
{
  /* Avoid init break down */
  return CSD_EOK;
}

int32_t csd_dev_fb_spkr_prot_deinit(void)
{
  /* Avoid de-init break down */
  return CSD_EOK;
}

