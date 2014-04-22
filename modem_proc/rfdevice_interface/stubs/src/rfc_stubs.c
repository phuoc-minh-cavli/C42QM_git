/*!
  @file
  dog_stubs.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008-2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfdevice_interface.mpss/1.40/stubs/src/rfc_stubs.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
#09/27/15   sr     update the get_grfc_num() to return the passed-in grfc_num
#07/23/15   avi    Stubs updated with rfcommon_core_error
#07/16/15   avi    Initial Version
===========================================================================*/

/*===========================================================================
File:C:/Q6/SCMSync/MOBs/paragk_PARAGK_modem.1h09.tip/core//dog/inc/dog.h
===========================================================================*/
#include "comdef.h"
#include "rfc_common.h"
#include "rfc_hwid.h"
#include "rfcommon_core.h"

boolean rfc_common_set_grfc(uint8 grfc, rfc_logic_type logic)
{
  return TRUE;
}

int32 rfc_common_get_grfc_num(int32 rf_sig_name)
{
  return rf_sig_name;
}

rf_hw_type rfcommon_nv_get_rf_config(void)
{
  return (rf_hw_type) 0;
}


void rfcommon_core_error( const unsigned int line, const char *file, 
                          const char *message )
{

  
  MSG_SPRINTF_2( MSG_SSID_RF, 
                 MSG_LEGACY_ERROR, 
                 "RF library error occurred in file %s at line %d",file,line );
  #if defined(FEATURE_GSM) || defined (FEATURE_CDMA1X) || defined(FEATURE_WCDMA)
  err_put_log ( line, file ); /*lint !e734*/
  #endif

}
