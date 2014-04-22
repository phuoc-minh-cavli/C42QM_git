#ifndef HSU_COMMON_H
#define HSU_COMMON_H
/*=============================================================================

            hsu_common   ---   H E A D E R   F I L E

=============================================================================*/
/**
@file hsu_common.h

Contains common definitions,Constants and Macros for High-Speed USB.

Externalized function:
- hsu_os_is_giant_locked_by_curr_task()
*/

/*=============================================================================
Copyright (c) 2006-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/wiredconnectivity/hsusb/hsu_common.h#1 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/24/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
05/17/06  sk  Creation

=============================================================================*/

#include "hsu_os_abstract_bus_ext.h"

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

/*=============================================================================
  Constants and Macros
=============================================================================*/

#ifdef FEATURE_HS_USB_MBIM
    /*
     * Defines for the NTB Parameters
     **/
  #ifndef USB_NTB_IN_MAX_SIZE
  #define USB_NTB_IN_MAX_SIZE (0x2000)
  #endif /*USB_NTB_IN_MAX_SIZE*/
    
  #ifndef USB_NTB_FORMATS_SUPPORTED
  #define USB_NTB_16 (0x1) /*For NTB's that are shorter than 64k*/
  #define USB_NTB_32 (0x2) /*For NTB's that are greater than 64k */
  #define USB_NTB_FORMATS_SUPPORTED USB_NTB_16
  #endif /*USB_NTB_FORMATS_SUPPORTED*/
    
  #ifndef USB_NDP_IN_DIVISOR
  #define USB_NDP_IN_DIVISOR (0x1)
  #endif /*USB_NDP_IN_DIVISOR*/
    
  #ifndef USB_NDP_IN_PAYLOAD_REMAINDER
  #define USB_NDP_IN_PAYLOAD_REMAINDER (0x0)
  #endif /*USB_NDP_IN_PAYLOAD_REMAINDER*/
    
  #ifndef USB_NDP_IN_ALIGNMENT
  #define USB_NDP_IN_ALIGNMENT (0x4)
  #endif /*USB_NDP_IN_ALIGNMENT*/
    
  #ifndef USB_NTB_OUT_MAX_SIZE
  #define USB_NTB_OUT_MAX_SIZE (0x800)
  #endif /*USB_NTB_OUT_MAX_SIZE*/
    
  #ifndef USB_NDP_OUT_DIVISOR
  #define USB_NDP_OUT_DIVISOR (0x4)
  #endif /*USB_NDP_OUT_DIVISOR*/
    
  #ifndef USB_NDP_OUT_PAYLOAD_REMAINDER
  #define USB_NDP_OUT_PAYLOAD_REMAINDER (0x0)
  #endif /*USB_NDP_OUT_PAYLOAD_REMAINDER*/
    
  #ifndef USB_NDP_OUT_ALIGNMENT
  #define USB_NDP_OUT_ALIGNMENT (0x4)
  #endif /*USB_NDP_OUT_ALIGNMENT*/
    
  #ifndef USB_NTB_OUT_MAX_DATAGRAMS
  #define USB_NTB_OUT_MAX_DATAGRAMS (0x10)
  #endif /*USB_NTB_OUT_MAX_DATAGRAMS*/
    
    /*Functional Headers related Macros*/
  #ifndef USB_MBB_VERSION
  #define USB_MBB_VERSION (0x100)
  #endif /*USB_MBB_VERSION*/
    
  #ifndef USB_MAX_CONTROL_MESSAGE
  #define USB_MAX_CONTROL_MESSAGE (0x1000)
  #endif /*USB_MAX_CONTROL_MESSAGE*/
    
  #ifndef USB_MAX_SEGMENT_SIZE
  #define USB_MAX_SEGMENT_SIZE (2048)
  #endif /*USB_MAX_SEGMENT_SIZE*/
    
  #ifndef USB_NETWORK_CAPABILITIES
  #define USB_NETWORK_CAPABILITIES (0x20)
  #endif /*USB_NETWORK_CAPABILITIES*/
    
  #ifndef USB_MAX_DATAGRAMS_IN_NTB
  #define USB_MAX_DATAGRAMS_IN_NTB (0x0A)
  #endif
    
  #ifndef USB_NUMBER_POWER_FILTERS 
  #define USB_NUMBER_POWER_FILTERS (32)
  #endif /*USB_NUMBER_POWER_FILTERS*/
      
  #ifndef USB_MAX_FILTER_SIZE
  #define USB_MAX_FILTER_SIZE (128)
  #endif /*USB_MAX_FILTER_SIZE*/
    
  #ifndef USB_MAX_OUTSTANDING_CMDMSG_SIZE
  #define USB_MAX_OUTSTANDING_CMDMSG_SIZE (64)
  #endif /*USB_MAX_OUTSTANDING_CMDMSG_SIZE*/
    
  #ifndef USB_DEFAULT_MTU_SIZE
  #define USB_DEFAULT_MTU_SIZE (1500)
  #endif /*USB_DEFAULT_MTU_SIZE*/
  
  #ifndef USB_MIN_MTU_SIZE
  #define USB_MIN_MTU_SIZE (1280)
  #endif /*USB_MIN_MTU_SIZE*/
  
#endif /* FEATURE_HS_USB_MBIM */

#define HSU_ECM_DSM_ITEM_SIZE   (5*1024)
#define HSU_ECM_DSM_ITEM_CNT    (25)


/** For use with the Jungo jresult_t type.
*/
#define HSU_JSUCCESS 0

/** For use with HSU_MSG_HIGH, HSU_MSG_MED, etc.
*/
#define HSU_MAX_MSG_LEN 80 

/** To overcome lint error 715: "Symbol 'Symbol' (Location) not referenced".
*/
#define HSU_USE_PARAM(x) (void)x

/** To overcome lint error 818: "Pointer parameter 'Symbol' (Location) could be
declared ptr to const".
*/
#define HSU_PARAM_COULD_BE_CONST(x) (void)x

/** Change a pointer to be 32 bytes aligned.
*/
#define HSU_ALIGN_PTR_TO_32_BYTE(ptr) ((byte *)((((uint32)(ptr)) + 31) & ~31))

/** Get the closest multiple of 32 (round up).
*/
#define HSU_ROUND_UP_TO_32(size) (((size) + 31) & ~31)

/** When the code is commercial, map this to (void)().
*/
#define HSU_ASSUME HSU_ASSERT

/** Macro for adding debug capabilities to os_safe_enter.
*/
#define os_safe_enter() hsu_os_safe_enter_d(__FILE__, __LINE__)

/** Macro for adding debug capabilities to os_safe_leave.
*/
#define os_safe_leave() hsu_os_safe_leave_d()

/** Internal implementation for os_safe_enter.
*/
void hsu_os_safe_enter_d(char *file, uint32 line);

/** Internal implementation for os_safe_leave.
*/
void hsu_os_safe_leave_d(void);

/*=============================================================================
FUNCTION    hsu_os_is_giant_locked_by_curr_task
=============================================================================*/
/**
Returns TRUE if the Giant mutex is currently locked by the running task.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The Giant mutex is currently locked by the running task.\n
FALSE -- The Giant mutex is not currently locked by the running task.
*/
boolean hsu_os_is_giant_locked_by_curr_task(void);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* HSU_COMMON_H */
