#ifndef RFC_CONFIGURABLE_TEST_CARD_TDSCDMA_DATA
#define RFC_CONFIGURABLE_TEST_CARD_TDSCDMA_DATA
/*!
  @file
  rfc_configurable_test_card_tdscdma_data.h

  @brief
  This file contains the class definition for the rfc_configurable_test_card_tdscdma_data, which implements the
  functionality to get/set the RFC related data.

*/

/*===========================================================================

Copyright (c) 2011-12 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfa.mpss/3.10/rfc/rf_card/rfc_configurable_test_card/tdscdma/inc/rfc_configurable_test_card_tdscdma_data.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------   
02/09/13   sr      Added Destructor.
11/08/12   sr      initial version
============================================================================*/
#ifdef FEATURE_TDSCDMA
#include "comdef.h"
#include "rfc_msm_typedef.h" 
#include "rfc_common.h" 
#include "rfc_tdscdma_data.h" 

#ifdef __cplusplus
extern "C" {
#endif


class rfc_configurable_test_card_tdscdma_data:public rfc_tdscdma_data
{
public:
  static rfc_tdscdma_data * get_instance();

  boolean sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr );
  boolean devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr );

  boolean sig_cfg_data_set( rfc_cfg_params_type *cfg, rfc_sig_info_type *ptr );
  boolean devices_cfg_data_set( rfc_cfg_params_type *cfg, rfc_device_info_type *ptr );

  // Destructor
  virtual ~rfc_configurable_test_card_tdscdma_data();

protected:
  rfc_configurable_test_card_tdscdma_data(void);  /*  Constructor  */
};


#ifdef __cplusplus
}
#endif

#endif

#endif


