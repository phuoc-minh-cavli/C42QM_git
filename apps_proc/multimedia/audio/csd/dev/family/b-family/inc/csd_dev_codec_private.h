#ifndef _CSD_DEV_CODEC_PRIVATE_H_
#define _CSD_DEV_CODEC_PRIVATE_H_
/*==============================================================================
  @file csd_dev_codec_private.h
  
  This header contains the device structure definition. The majority of the 
  structures are depended on ACDB module. Since acdb returns data in a buffer
  referred by void*, you can uses these structures to make sense of the data.
  
  Copyright (c) 2008-13 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/family/b-family/inc/csd_dev_codec_private.h#1 $
  $DateTime: 2018/12/25 22:39:07 $
  $Author: pwbldsvc $
  $Change: 17929171 $
  $Revision: #1 $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------

==============================================================================*/
#include "adsp_afe_service_commands.h"


/**
 * This function is used to retrieve the ADIE Codec slimbus 
 * configuration parameters. 
 */
int32_t csd_dev_codec_get_slimbus_cfg_data ( void **codec_sb_cfg_data, 
                                             uint32_t *buf_size);

/**
 * This function is used to retrieve the ADIE Codec AANC Register 
 * configuration parameters. 
 */
int32_t csd_dev_codec_get_aanc_reg_cfg_data ( void **codec_cfg_data, 
                                              uint32_t *codec_reg_count, 
                                              uint32_t *hw_version);

/**
 * This function is used to pack the codecs slimbus 
 * configuration params into AFE structure 
 */
int32_t csd_dev_codec_pack_slimbus_cfg_data(void *pCodecData, 
                                            uint32_t nCdcDataSize, 
                                            afe_cdc_slimbus_slave_cfg_param_v1_t* pAfeCfg, 
                                            uint32_t nAfeDataSize);

/**
 * This function is used to pack the codecs aanc register
 * configuration params into AFE structure 
 */
int32_t csd_dev_codec_pack_aanc_cdc_reg_cfg_data (void *codec_cfg_data, 
                                                  afe_cdc_reg_cfg_param_v1_t *afe_cfg_data,  
                                                  uint32_t reg_index);


#endif /* _CSD_DEV_CODEC_PRIVATE_H_ */

