#ifndef _CSD_DEV_ACDB_PRIVATE_H_
#define _CSD_DEV_ACDB_PRIVATE_H_
/*==============================================================================
  @file csd_dev_device_info.h
  
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

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/family/b-family/inc/csd_dev_acdb_private.h#1 $
  $DateTime: 2018/12/25 22:39:07 $
  $Author: pwbldsvc $
  $Change: 17929171 $
  $Revision: #1 $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------
  02/25/13   suns    Device Module unification.
  09/14/12   unni    B Family base code
                     -Reformatting to a common format
  07/13/12   rp      Added 9x25 AFE support.
==============================================================================*/


/*=============================================================================

  FUNCTION      csd_acdb_get_adie_codec_profile

  DESCRIPTION   Query ACDB with codec path id to get codec related profile

  DEPENDENCIES  ACDB: AcdbAdiePathProfileV2CmdType,AcdbQueryResponseType
                WCD : Acdb_WCD93xx_B_CodecProfile (Specific to Taiko)

  PARAMETERS    @param codec_path_id : id specific to the related codec path
                        which needs to be queried
                @param codec_profile : reference to allocated space to copy
                        profiles got from acdb

  RETURN VALUE  CSD error code

  SIDE EFFECTS  None
=============================================================================*/
int32_t csd_acdb_get_adie_codec_profile ( uint32_t codec_path_id,
                                          uint32_t codec_param_id,
                                          uint8_t *codec_profile );

/*=============================================================================

  FUNCTION      csd_dev_acdb_get_anc_profile

  DESCRIPTION   Query ACDB with device id to get anc codec related profile

  DEPENDENCIES  n/a

  PARAMETERS    @param dev_id : reference device id
                @param anc_profile : reference to allocated space to copy
                                     anc profiles got from acdb
                @param buf_size : available buffer size

  RETURN VALUE  CSD error code

  SIDE EFFECTS  None
=============================================================================*/
int32_t csd_dev_acdb_get_anc_profile(uint32_t dev_id,
                                     uint8_t *anc_profile,
                                     uint32_t buf_size);

/*=============================================================================

  FUNCTION      csd_acdb_get_afe_aanc_cfg

  DESCRIPTION   Get AANC CFG1 and CFG2 Params

  DEPENDENCIES  n/a

  PARAMETERS    TODO

  RETURN VALUE  CSD error code

  SIDE EFFECTS  None
=============================================================================*/
int32_t csd_acdb_get_afe_aanc_cfg(uint32_t tx_dev_id, 
                                  void *aanc_cfg1, 
                                  uint32_t *cfg1_buf_size, 
                                  void *aanc_cfg2, 
                                  uint32_t *cfg2_buf_size);

/*=============================================================================

  FUNCTION      csd_acdb_get_adie_codec_anc_db

  DESCRIPTION   Get ADIE CODEC specfic ANC/AANC Database 

  DEPENDENCIES  n/a

  PARAMETERS    TODO

  RETURN VALUE  CSD error code

  SIDE EFFECTS  None
=============================================================================*/
int32_t csd_acdb_get_adie_codec_anc_db(uint32_t rx_dev_id, 
                                       uint32_t codec_pid, 
                                       uint8_t *anc_db_buf, 
                                       uint32_t *anc_buf_size);

/*=============================================================================

  FUNCTION      csd_acdb_get_afe_aanc_enable

  DESCRIPTION   Get ADIE CODEC specfic ANC/AANC Database 

  DEPENDENCIES  n/a

  PARAMETERS    TODO

  RETURN VALUE  CSD error code

  SIDE EFFECTS  None
=============================================================================*/
int32_t csd_acdb_get_afe_aanc_enable(uint32_t tx_dev_id, 
                                     void *aanc_enable, 
                                     uint32_t *buf_size);

/*=============================================================================

  FUNCTION      csd_acdb_get_adie_sidetone

  DESCRIPTION   This function must be called to retrieve the adie sidetone
                parameters from the acdb for a pair of devices.

  DEPENDENCIES  n/a

  PARAMETERS    *adie_sidetone_tbl contains the sidetone parameters, see
                payload of ACDB_CMD_GET_ADIE_SIDETONE_TABLE .

  RETURN VALUE  CSD error code

  SIDE EFFECTS  None
=============================================================================*/
int32_t csd_acdb_get_adie_sidetone(uint32_t rx_id, 
                                   uint32_t tx_id, 
                                   uint8_t *adie_sidetone_tbl,
                                   uint32_t buf_size);



#endif /* _CSD_DEV_ACDB_PRIVATE_H_ */


