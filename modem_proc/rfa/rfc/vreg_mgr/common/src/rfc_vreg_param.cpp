/*!
   @file rfc_vreg_param.cpp

   @brief


   @details
   This file implements the rfc_vreg_mgr abstract class.

*/
/*===========================================================================
Copyright (c) 2018-2021 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2021/01/19 20:55:14 $ $Author: mplcsds1 $
$Header: //components/rel/rfa.mpss/3.10/rfc/vreg_mgr/common/src/rfc_vreg_param.cpp#2 $

when       who     what, where, why
------------------------------------------------------------------------------- 
01/18/2021 gk     F3 reduction( during Tx tune)
04/17/19   tik    Separate PAM client voting for PC3 vs PC5
04/02/12   sr     Initial revision.
05/03/12   sb     Fixed KW warning
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
extern "C" {
#include "rfcom.h"
#include "msg.h"
}
#include "rfc_vreg_param.h"

/* ----------------------------------------------------------------------- */
/*!
   @brief
   constructor.

   @details

*/
rfc_vreg_param::rfc_vreg_param(void)
: mode(RFM_INVALID_MODE), band(0), rf_chip_num(RF_CHIP_MAX), path(RF_PATH_MAX), rf_state(RF_PATH_MAX_STATE)
{

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   constructor.

   @details

*/
rfc_vreg_param::rfc_vreg_param(rfcom_mode_enum_type  mode)
{
  this->band = 0;
  this->rf_chip_num = RF_CHIP_MAX;
  this->path = RF_PATH_MAX;
  this->rf_state = RF_PATH_MAX_STATE;
  set_mode(mode);
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   constructor.

   @details

*/
rfc_vreg_param::rfc_vreg_param(rf_path_enum_type  path, rfcom_mode_enum_type  mode)
{
  this->band = 0;
  this->rf_chip_num = RF_CHIP_MAX;
  this->path = RF_PATH_MAX;
  this->rf_state = RF_PATH_MAX_STATE;
  set_path(path);
  set_mode(mode);
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   Destructor.

   @details

*/
rfc_vreg_param::~rfc_vreg_param()
{
  // Do nothing
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
boolean rfc_vreg_param::set_path(rf_path_enum_type  path)
{
  if (path < RF_PATH_0 || path >= RF_PATH_MAX )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid path (path=%d)",path);
    return FALSE;
  }

  this->path = path;
  return TRUE;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
boolean rfc_vreg_param::set_mode(rfcom_mode_enum_type  mode)
{
  if (mode < RFM_PARKED_MODE || mode >= RFM_INVALID_MODE )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid mode (mode=%d)",mode);
    return FALSE;
  }

  this->mode = mode;
  return TRUE;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
boolean rfc_vreg_param::set_band(uint16  band)
{
  this->band = band;
  return TRUE;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
boolean rfc_vreg_param::set_rfchip_number(rfc_rf_chip_number  rf_chip_num)
{
  if (rf_chip_num < RF_CHIP_0 || rf_chip_num >= RF_CHIP_MAX)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid rf_chip_num (rf_chip_num=%d)",rf_chip_num);
    return FALSE;
  }
  this->rf_chip_num = rf_chip_num;
  return TRUE;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
boolean rfc_vreg_param::set_rf_state(rfm_path_state  rf_state)
{
  if (rf_state < RF_PATH_SLEEP_STATE || rf_state >= RF_PATH_MAX_STATE)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid rf state (rf_state=%d)",rf_state);
    return FALSE;
  }
  this->rf_state = rf_state;
  return TRUE;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/

boolean rfc_vreg_param::set_pwr_class(rfc_rf_pwr_cls  pwr_cls)
{
  if (pwr_cls >= RFC_RF_POWER_CLASS_MAX)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid LTE Power class (pwr_cls=%d)",pwr_cls);
    return FALSE;
  }
  this->pwr_cls = pwr_cls;
  //MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH," LTE Power class (pwr_cls=%d)",pwr_cls);
  return TRUE;
}


/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
rf_path_enum_type rfc_vreg_param::get_path(void)
{
  if (path == RF_PATH_MAX )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"trying to use invalid path (path=%d)",path);
  }

  return path;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
rfcom_mode_enum_type rfc_vreg_param::get_mode(void)
{
  if (mode == RFM_INVALID_MODE )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"trying to use invalid mode (mode=%d)",mode);
  }

  return mode;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
uint16 rfc_vreg_param::get_band(void)
{
  return band;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
rfc_rf_chip_number rfc_vreg_param::get_rfchip_number(void)
{
  if (rf_chip_num == RF_CHIP_MAX)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"trying to use invalid rf_chip_num (rf_chip_num=%d)",rf_chip_num);
  }

  return rf_chip_num;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
rfm_path_state rfc_vreg_param::get_rf_state(void)
{
  if (rf_state == RF_PATH_MAX_STATE)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"trying to use invalid rf state (rf_state=%d)",rf_state);
  }

  return rf_state;
}

rfc_rf_pwr_cls rfc_vreg_param::get_pwr_class(void)
{
  if (pwr_cls >= RFC_RF_POWER_CLASS_MAX)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"trying to use invalid power class (pwr_cls=%d)",pwr_cls);
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"Power class returned (pwr_cls=%d)",pwr_cls);
  return pwr_cls;
}








