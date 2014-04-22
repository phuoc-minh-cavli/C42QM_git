#ifndef GNSS_HAL_H
#define GNSS_HAL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  GNSS Hardware abstraction layer (HAL) Header
GENERAL DESCRIPTION
   This file contains constants, classes, structures and functions prototypes
   related to GNSS HAL.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
==============================================================================*/


/*==============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

Version Control

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/hal/inc/gnss_hal.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

when        who     what, where, why
--------    ---   ----------------------------------------------------------
9/13/2015  ahmadh Initial version

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "aries_os_api.h"
#include "cgps_api.h"
#include "gps_types.h" 
#include <new>
extern "C" 
{
  #include "msg.h"
  #include "customer.h"
  #include "err.h"
}


/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

class gnss_HalClass;

/*=============================================================================

       Constants

=============================================================================*/


/*=============================================================================

       Macros

=============================================================================*/
#define C_HEAP_FREE_NO_SIZE  (0x01)


#define GNSS_NEW_NOARGS(module, type, pObj) \
          if ( NULL != ((pObj) = (type *)(gnss_HalClass::MemAlloc(module, sizeof(type)))) ) \
          { new (pObj) (type); }

#define GNSS_NEW_ARGS(module, type, pObj, ...) \
          if ( NULL != ((pObj) = (type *)(gnss_HalClass::MemAlloc(module, sizeof(type)))) ) \
          { new (pObj) (type)(__VA_ARGS__); }

#define GNSS_DELETE(module, type, pObj) \
          if ( NULL != (pObj) ) \
          { \
            size_t Size = pObj->GetSize();\
            (pObj)->~type(); \
            gnss_HalClass::MemFree(module, (void **) &pObj, Size); \
          }

#define GNSS_DESTROY(pObj) \
          if ( NULL != (pObj) ) \
          { \
            (pObj)->~type(); \
          }

#define GNSS_CONSTRUCT_ARGS(type, pObj, ...) \
          new (pObj) (type)(__VA_ARGS__);

#define HAL_MSG_LOW_0(str)         MSG(MSG_SSID_GPS, MSG_LEGACY_LOW, str)
#define HAL_MSG_LOW_1(str,a)       MSG_1(MSG_SSID_GPS, MSG_LEGACY_LOW, str, a)
#define HAL_MSG_LOW_2(str,a,b)     MSG_2(MSG_SSID_GPS, MSG_LEGACY_LOW, str, a, b)
#define HAL_MSG_LOW_3(str,a,b,c)   MSG_3(MSG_SSID_GPS, MSG_LEGACY_LOW, str, a, b, c)
#define HAL_MSG_LOW_4(str,a,b,c,d) MSG_4(MSG_SSID_GPS, MSG_LEGACY_LOW, str, a, b, c, d)

#define HAL_MSG_MED_0(str)         MSG(MSG_SSID_GPS, MSG_LEGACY_MED, str)
#define HAL_MSG_MED_1(str,a)       MSG_1(MSG_SSID_GPS, MSG_LEGACY_MED, str, a)
#define HAL_MSG_MED_2(str,a,b)     MSG_2(MSG_SSID_GPS, MSG_LEGACY_MED, str, a, b)
#define HAL_MSG_MED_3(str,a,b,c)   MSG_3(MSG_SSID_GPS, MSG_LEGACY_MED, str, a, b, c)
#define HAL_MSG_MED_4(str,a,b,c,d) MSG_4(MSG_SSID_GPS, MSG_LEGACY_MED, str, a, b, c, d)

#define HAL_MSG_HIGH_0(str)         MSG(MSG_SSID_GPS, MSG_LEGACY_HIGH, str)
#define HAL_MSG_HIGH_1(str,a)       MSG_1(MSG_SSID_GPS, MSG_LEGACY_HIGH, str, a)
#define HAL_MSG_HIGH_2(str,a,b)     MSG_2(MSG_SSID_GPS, MSG_LEGACY_HIGH, str, a, b)
#define HAL_MSG_HIGH_3(str,a,b,c)   MSG_3(MSG_SSID_GPS, MSG_LEGACY_HIGH, str, a, b, c)
#define HAL_MSG_HIGH_4(str,a,b,c,d) MSG_4(MSG_SSID_GPS, MSG_LEGACY_HIGH, str, a, b, c, d)

#define HAL_MSG_ERR_0(str)         MSG(MSG_SSID_GPS, MSG_LEGACY_ERROR, str)
#define HAL_MSG_ERR_1(str,a)       MSG_1(MSG_SSID_GPS, MSG_LEGACY_ERROR, str, a)
#define HAL_MSG_ERR_2(str,a,b)     MSG_2(MSG_SSID_GPS, MSG_LEGACY_ERROR, str, a, b)
#define HAL_MSG_ERR_3(str,a,b,c)   MSG_3(MSG_SSID_GPS, MSG_LEGACY_ERROR, str, a, b, c)
#define HAL_MSG_ERR_4(str,a,b,c,d) MSG_4(MSG_SSID_GPS, MSG_LEGACY_ERROR, str, a, b, c,d)

#define HAL_MSG_FATAL_0(str)          MSG(MSG_SSID_GPS, MSG_LEGACY_FATAL, str)
#define HAL_MSG_FATAL_1(str,a)        MSG_1(MSG_SSID_GPS, MSG_LEGACY_FATAL, str, a)
#define HAL_MSG_FATAL_2(str,a,b)      MSG_2(MSG_SSID_GPS, MSG_LEGACY_FATAL, str, a, b)
#define HAL_MSG_FATAL_3(str,a,b,c)    MSG_3(MSG_SSID_GPS, MSG_LEGACY_FATAL, str, a, b, c)
#define HAL_MSG_FATAL_4(str,a,b,c,d)  MSG_4(MSG_SSID_GPS, MSG_LEGACY_FATAL, str, a, b, c,d)



/*=============================================================================

       Structures and enumerated types

=============================================================================*/



/*==============================================================================

                                  VARIABLES

==============================================================================*/

typedef enum
{
  CGPS_MODULE_MIN_ITEMS = 0,

  /* Common */
  CGPS_COMMON_MODULE_MIN     = CGPS_MODULE_MIN_ITEMS,
  CGPS_COMMON_MODULE_UNKNOWN,
  CGPS_COMMON_MODULE_HAL,
  CGPS_COMMON_MODULE_LIST,
  CGPS_COMMON_MODULE_VECTOR,
  CGPS_COMMON_MODULE_MAX = CGPS_COMMON_MODULE_VECTOR,

  /* TLE */
  CGPS_TLE_MODULE_MIN,
  CGPS_TLE_MODULE_UNKNOWN = CGPS_TLE_MODULE_MIN,
  CGPS_TLE_MODULE_HAL,
  CGPS_TLE_MODULE_TASK,
  CGPS_TLE_MODULE_DBM_DOWNLD,
  CGPS_TLE_MODULE_DBM_UPLD,
  CGPS_TLE_MODULE_PTM, 
  CGPS_TLE_MODULE_DOWNLOAD_MGR,
  CGPS_TLE_MODULE_UPLOAD_MGR,
  CGPS_TLE_MODULE_XTA_MGR,
  CGPS_TLE_MODULE_WWAN_MGR,
  CGPS_TLE_MODULE_FILE_IO,
  CGPS_TLE_MODULE_TEST,
  CGPS_TLE_MODULE_LIST,
  CGPS_TLE_MODULE_VECTOR,
  CGPS_TLE_MODULE_CELL_TREE,
  CGPS_TLE_MODULE_SELF_LEARNED,
  CGPS_TLE_MODULE_SELF_LEARNED_SCANLIST,
  CGPS_TLE_MODULE_BSA_PARTITION,
  CGPS_TLE_MODULE_SNA_PARTITION,
  CGPS_TLE_MODULE_MEMORY_MANAGER,
  CGPS_TLE_MODULE_OSYS,
  CGPS_TLE_MODULE_MAX = CGPS_TLE_MODULE_OSYS,

  /* SM_TM */
  CGPS_SM_TM_MODULE_MIN,
  CGPS_SM_TM_MODULE_LIST = CGPS_SM_TM_MODULE_MIN,
  CGPS_SM_TM_MODULE_CSM_COMMON,
  CGPS_SM_TM_MODULE_MAX = CGPS_SM_TM_MODULE_CSM_COMMON,

  /* SM_GM */
  CGPS_SM_GM_MODULE_MIN,
  CGPS_SM_GM_MODULE_LIST = CGPS_SM_GM_MODULE_MIN,
  CGPS_SM_GM_MODULE_CTXT_CHANGE,
  CGPS_SM_GM_MODULE_MAX = CGPS_SM_GM_MODULE_CTXT_CHANGE,

  /* LocMW */
  CGPS_LOCMW_MODULE_MIN,
  CGPS_LOCMW_MODULE_AON = CGPS_LOCMW_MODULE_MIN,
  CGPS_LOCMW_MODULE_MAX = CGPS_LOCMW_MODULE_AON,


  /* SM_TECH_SEL */
  CGPS_SM_TECHSEL_MODULE_MIN,
  CGPS_SM_TECHSEL_MODULE_COMMON = CGPS_SM_TECHSEL_MODULE_MIN,
  CGPS_SM_TECHSEL_MODULE_MAX = CGPS_SM_TECHSEL_MODULE_COMMON,

  CGPS_MODULE_MAX_ITEMS /* This should be the last item. Add all entries above this line */
} cgps_module_type;

/*=============================================================================

       Class Definition

=============================================================================*/
class gnss_HalClass
{
public:
   static gnss_HalClass * GetInstance();
  ~gnss_HalClass(void);

  size_t GetSize() const
  {
    return sizeof(*this);
  }

  static void * MemAlloc(cgps_module_type e_Module, size_t size);
  static void * MemReAlloc(cgps_module_type e_Module, void **ptr_ptr, size_t size,
                           size_t exist_size = 0);
  static void MemFree(cgps_module_type e_Module, void **  ptr_ptr,  size_t size=0);
  static void Init();
  static void ReportHeapUsage();
  static void GetHeapUsage(uint32 * pq_ArrMemUsage, uint32 q_Count);

private:
  gnss_HalClass();
  static uint8 MapModule(const cgps_module_type &ce_Module);
  
  struct HeapStatusType
  {
    uint32 q_InuseSize;
    uint8  u_Flag; 
  };

  static gnss_HalClass * m_pSingleHal;
  static HeapStatusType m_HeapStatus[CGPS_MODULE_MAX_ITEMS];
};

/*=============================================================================

       Functions

=============================================================================*/


#endif /* GNSS_HAL_H */
