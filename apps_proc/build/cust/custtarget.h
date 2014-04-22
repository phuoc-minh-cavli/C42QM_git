#ifndef CUSTTARGET_H
#define CUSTTARGET_H
/*===========================================================================

                           C U S T U S U R F

DESCRIPTION
  Customer file for the MSM6280 UMTS (GSM + WCDMA) SURF build.

  Copyright (c) 1996-2008 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/build.tn/4.0/cust/custtarget.h#1 $ $DateTime: 2018/06/25 01:51:12 $ $Author: pwbldsvc $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
# 10/02/08 gdp     Initial version for 76XX.
---------------------------------------------------------------------------
                            Common Features
---------------------------------------------------------------------------*/

#if defined(FEATURE_WCDMA) && defined(FEATURE_CDMA)
	//Multimode
	#define CUST_MOB_MODEL 4036
	#define CUST_MOB_MODEL_EXT 4036
#elif defined(FEATURE_WCDMA)
	//UMTS only
	#define CUST_MOB_MODEL 4037
	#define CUST_MOB_MODEL_EXT 4037
#else
   //C2K only
   #define CUST_MOB_MODEL 4038
   #define CUST_MOB_MODEL_EXT 4038
#endif

#endif /* CUSTTARGET_H */


/*****************************************************************************/

