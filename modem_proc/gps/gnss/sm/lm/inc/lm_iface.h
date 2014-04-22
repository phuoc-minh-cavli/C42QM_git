/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               Location Manager (LM) Interface Header File

GENERAL DESCRIPTION
  This file contains the LM (Location Manager) data structure 
  definitions and prototypes for all functions that are visible to other tasks.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/lm/inc/lm_iface.h#1 $ $DateTime: 2020/07/21 21:23:44 $ $Author: pwbldsvc $ 

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef LM_IFACE_H
#define LM_IFACE_H


/*
 * Include files
*/

#include "gps_types.h"
#include "gps_consts.h"
#include "aries_os_globals.h"
#include "gps_common.h"
#include "sm_api.h"
#include "sm_gpsdiag_api.h"
#include "lm_api.h"
/*
 * Constant definitions
*/

#define IS_LM_MSG(x) (((x)> C_SM_LM_MSG_FIRST) && \
                       ((x)< C_SM_LM_MSG_END) )
/*
 * Structure definitions
*/


/*
 * Function definitions
*/


#endif /* LM_IFACE_H */

