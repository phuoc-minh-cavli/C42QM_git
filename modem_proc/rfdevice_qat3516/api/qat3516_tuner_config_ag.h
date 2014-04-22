
#ifndef QAT3516_TUNER_CONFIG_AG_H
#define QAT3516_TUNER_CONFIG_AG_H/*
WARNING: This QAT3516 driver is auto-generated.

Generated using: qtuner_autogen.pl 
Generated from-  

	File: QAT3516_RFFE_Settings.xlsx 
	Released: 
	Author: 
	Revision: 
	Change Note: 
	Tab: qat3516_tuner_settings

*/

/*=============================================================================

          RF DEVICE  A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the QAT3516 TUNER.

Copyright (c) 2013,2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

$Header: //depot/SBA/users/lteswint/ebf/mpl/BF-ENG-RFSW_B71-MPSS.JO.3.0-00379-8937_GENNS_TEST-1/modem_proc/rfdevice_qat3514/etc/qtuner_autogen.pl#1 : lteswint : 2017/10/17 04:09:55 60============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/


#include "comdef.h"
#include "rfc_common.h"
#include "rfdevice_qtuner_typedef.h"
#ifdef __cplusplus
extern "C" {
#endif  
boolean rfdevice_qtuner_qat3516_validate_cfg_ag(rfc_device_cfg_info_type* cfg);

boolean rfdevice_qtuner_qat3516_construct_driver_ag
(
  rfdevice_qtuner_settings_type* qtuner_settings,
  rfdevice_qtuner_func_tbl_type* qtuner_fn_ptrs,
  rfdevice_qtuner_params_type * qtuner_params
);

#ifdef __cplusplus
}
#endif
#endif