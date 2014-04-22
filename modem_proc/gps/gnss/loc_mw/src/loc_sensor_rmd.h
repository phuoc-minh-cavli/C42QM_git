/*============================================================================
 @file loc_sensor_rmd.h

 loc MW geofencing modules

 GENERAL DESCRIPTION

 This file defines the loc sensor client in loc middleware.

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_sensor_rmd.h#1 $
 $DateTime: 2020/07/21 21:23:44 $
 $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 11/29/12   ss     Initial version

 =============================================================================*/

#ifndef _LOC_SENSOR_RMD_H_
#define _LOC_SENSOR_RMD_H_

#include "comdef.h"
#include "slim_api.h"

#ifdef FEATURE_GEOFENCE_SENSOR_SUPPORT

#define GEOFENCE_RMD_IND_TYPE_ERROR           0
#define GEOFENCE_RMD_IND_TYPE_ENABLE_RESP     1 
#define GEOFENCE_RMD_IND_TYPE_DISABLE_RESP    2
#define GEOFENCE_RMD_IND_TYPE_REPORT          3

typedef struct
{
  uint32 rmd_indication_type;
  
  union
  {
    slimErrorEnumT     rmd_err_type;
    slimQmdDataStructT rmd_report_ind;
  }u;
} loc_geofence_rmd_ind;

/** loc_geofence_sensor_start_rmd
 *    Handles RMD Start request from Geofence module. 
 *  @param [in] report_period
 * */
boolean loc_geofence_sensor_start_rmd(uint32 report_period);

/** loc_geofence_sensor_stop_rmd
 *    Handles RMD Stop request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_stop_rmd(void);


#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/
#endif // _LOC_SENSOR_RMD_H_
