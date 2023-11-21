#ifndef RFC_VREG_PARAM_H
#define RFC_VREG_PARAM_H
/*!
   @file
   rfc_vreg_param.h

   @brief


   @details

*/
/*===========================================================================

Copyright (c) 2019 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2020/04/16 12:27:00 $ $Author: pwbldsvc $
$Header: //components/rel/rfa.mpss/3.10/rfc/vreg_mgr/common/inc/rfc_vreg_param.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
04/17/19   tik     Separate PAM client voting for PC3 vs PC5
04/02/12   sr     Initial revision.

===========================================================================
                           INCLUDE FILES
===========================================================================*/
extern "C" {
#include "rfcom.h"
#include "rfc_card.h"
}



class rfc_vreg_param
{
public:

  rfc_vreg_param(void);  // constructor
  rfc_vreg_param(rfcom_mode_enum_type  mode);
  rfc_vreg_param(rf_path_enum_type  path, rfcom_mode_enum_type  mode);

  boolean set_path(rf_path_enum_type  path);
  boolean set_mode(rfcom_mode_enum_type  mode);
  boolean set_band(uint16  band);
  boolean set_rfchip_number(rfc_rf_chip_number  rf_chip_num);
  boolean set_rf_state(rfm_path_state  rf_state);
  boolean set_pwr_class(rfc_rf_pwr_cls  pwr_cls);
  
  rf_path_enum_type get_path(void);
  rfcom_mode_enum_type get_mode(void);
  uint16 get_band(void);
  rfc_rf_chip_number get_rfchip_number(void);
  rfm_path_state get_rf_state();
  rfc_rf_pwr_cls get_pwr_class (void);


  virtual ~rfc_vreg_param();

protected:

private:

  rfcom_mode_enum_type  mode;        //1x, EVDO, LTE, WCDMA, GSM, GPS
  uint16                band;        //current band
  rfc_rf_chip_number    rf_chip_num; //the RTR on the RF card to be used (0,1,...)
  rf_path_enum_type     path;        //path on the RTR (0, 1 or 2)
  rfm_path_state        rf_state;    // RX, RXTX, or Sleep
  rfc_rf_pwr_cls        pwr_cls;  

};


#endif /* RFC_VREG_PARAM_H */



