#ifndef _GNSS_UTIL_H
#define _GNSS_UTIL_H

/*======================================================================

                         GNSS util header file

 GENERAL DESCRIPTION
  This file contains wrapper functions for functions that can not be called
  by C++ code directly. 

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/common/inc/gnss_util.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $
when       who   what, where, why
--------   ---   ----------------------------------------------------------
10/20/10   ah    File created
======================================================================*/
#include "gps_common.h"
#include "rfs_api.h"
//#ifdef FEATURE_CGPS_WCDMA
#include "rr_lsm_if.h"
//#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include "time_svc.h"
#include "rfs_api.h"

#ifdef FEATURE_CGPS_USES_CDMA
#include "mccdma_v.h"
#include "db.h"
#endif

int gnss_rfs_open (const char *path, int oflag, int mode);
int gnss_rfs_stat (const char *path, struct rfs_stat_buf *buf);
int gnss_rfs_close (int filedes);
int32 gnss_rfs_read (int filedes, void *buf, uint32 nbyte);
int32 gnss_rfs_write (int filedes, const void *buf, uint32 nbyte);
boolean gnss_GetCurrentBsInfo(uint8 u_SubscrId, current_bs_info_T *pz_BsInfo);
#ifdef FEATURE_CGPS_USES_CDMA
void gnss_db_get(db_items_type item, db_items_value_type *value_ptr);
#endif

void gnss_time_get_uptime_ms(time_type z_TimeMsec);


/*==============================================================================

                                  VARIABLES

==============================================================================*/



#ifdef __cplusplus
}
#endif

#endif /* _GNSS_UTIL_H */
