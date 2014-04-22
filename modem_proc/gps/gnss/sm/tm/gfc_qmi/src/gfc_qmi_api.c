/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GFC_QMI module which is the interface between QMI and GM

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/gfc_qmi/src/gfc_qmi_api.c#1 $
$DateTime: 2020/07/21 21:23:44 $

=============================================================================

EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/20/15   ssu     Adding default geofence confidence to HIGH
04/02/15    ssu   Batch Dwell notification for geofence.
03/24/12   ss       Supporting operation mode settting to effect Geofencing operation mode
02/29/12   ssu     Support for purge geofence.
02/23/12   ssu     Adding NULL parameter validation and support geofence origin in
                   querying geofence ids
11/15/11   ssu     Initial creation of file .

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "gfc_qmi.h"
#include "gfc_qmi_api.h"
#include "gm_api.h"
#include "gfc_qmi_internal.h"
#include "time.h"



boolean gfc_qmi_register(
  const gfc_qmi_cb_rsp_s_type* p_geofence_func_cbs
)
{
  if(NULL == p_geofence_func_cbs)
  {
    return FALSE;
  }
  else
  {
    gfc_qmi_internal_register_cbs(p_geofence_func_cbs);
    return TRUE;
  }
}



