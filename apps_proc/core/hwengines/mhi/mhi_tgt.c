/**
 * @file mhi_tgt.c
 * @brief
 * Has the target configurable parameters.
 */
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================

Copyright (c) 2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi_tgt.h"
#include "mhi_osal.h"

#ifdef MHI_XML_PROPS
   /** DALSYS property handle for target */
   DALSYS_PROPERTY_HANDLE_DECLARE(hPropmhi);
#endif

extern const mhi_core_tgt_config_t mhi_core_tgt_config;

/** 
 * @brief      mhi_tgt_init
 *
 *             Performs initialization to get target specific information 
 *              
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_tgt_init(void)
{
#ifdef MHI_XML_PROPS  
  static boolean initialized=FALSE;
  DALResult status = DAL_SUCCESS;

  if (!initialized)
  {
     status = DALSYS_GetDALPropertyHandleStr("/core/hwengines/mhi", hPropmhi);
     if(status != DAL_SUCCESS)
     {
        mhi_osal_debug(MSG_FATAL, "DALSYS_GetDALPropertyHandleStr failed with code %d",status);
     } 
  }
#endif  
}

/** 
 * @brief      mhi_get_core_tgt_info
 *
 *             Get target specific information for core 
 *              
 * 
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Pointer to target specific information for core
 * 
 * @sideeffect None 
 */
const mhi_core_tgt_config_t *mhi_get_core_tgt_info(void)
{
#ifdef MHI_XML_PROPS
   DALSYSPropertyVar mhiprop;
   DALResult status = DAL_SUCCESS;

   mhi_tgt_init();
   status = DALSYS_GetPropertyValue(hPropmhi, "mhi_core_tgt_config", 0, &mhiprop);
   if(status != DAL_SUCCESS || NULL == (mhi_core_tgt_config_t * )mhiprop.Val.pStruct)
      mhi_osal_debug(MSG_FATAL, "DALSYS_GetPropertyValue failed with code %d",status);
   
   return (mhi_core_tgt_config_t * )mhiprop.Val.pStruct;   
#else
   return &mhi_core_tgt_config;
#endif
}

