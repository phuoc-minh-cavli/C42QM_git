/*=============================================================================	
   Copyright (c) 2013 Qualcomm Technologies Incorporated.	
   All Rights Reserved.
	$Header: 
=============================================================================*/
#ifndef __SYS_M_PL_INFO_H__
#define __SYS_M_PL_INFO_H__

#include "DALStdDef.h"

typedef struct SYS_M_PL_INFO SYS_M_PL_INFO; 
struct SYS_M_PL_INFO 
{
   uint32 platform;    /**< Type of the current target. */
   uint32 version;     /**< Version of the platform in use. */
   uint32 subtype;     /**< Sub-type of the platform. */
   uint32 fusion;      /**< TRUE if Fusion; FALSE otherwise. */

   uint32 mdm2ap_status_gpio;
   uint32 mdm2ap_err_fatal_gpio;
   uint32 ap2mdm_status_gpio;
   uint32 ap2mdm_err_fatal_gpio;
};

#endif
