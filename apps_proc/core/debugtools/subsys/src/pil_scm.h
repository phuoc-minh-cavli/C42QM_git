#ifndef __PIL_SCM_H__
#define __PIL_SCM_H__

#include "pil.h"

/**
  @file pil_scm.h
  @brief Functions in which we are making scm calls
  

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2020 by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
 
 /*=============================================================================
                        MACROS
=============================================================================*/

#define PIL_INIT_IMAGE_PARAM_ID 		0x82
#define PIL_AUTH_RESET_PARAM_ID 		0x1
#define PIL_MEM_SETUP_PARAM_ID 			0x3
#define PIL_DEINIT_IMAGE_PARAM_ID 		0x1

#define PIL_INIT_IMAGE_SCM_ID 			0x01
#define PIL_AUTH_RESET_SCM_ID 			0x05
#define PIL_MEM_SETUP_SCM_ID 			0x02
#define PIL_UNLOCK_IMAGE_SCM_ID 		0x06

#define MODEM_PROC_ID					0x04

#define TZ_OWNER_SIP                     2
#define TZ_SVC_PIL                		 2       /**< Peripheral image loading.      */

#define TZ_SYSCALL_CREATE_SMC_ID(o, s, f) \
  ((uint32)((((o & 0x3f) << 24 ) | (s & 0xff) << 8) | (f & 0xff)))
 
/*=============================================================================
                        FUNCTIONS
=============================================================================*/

/*=============================================================================
FUNCTION 
  pil_validate_elf

DESCRIPTION
  Make an scm call to TZ to validate the given virtual address which
  contains the elf header, program headers and hash segment.
  
        
DEPENDENCIES
  None.
  
INPUTS
  virt_addr - initialized elf structure handle
  size - size of the buffer to which virt_addr is the start location

RETURN VALUE
PIL_SUCCESS if scm call returns success else it returns PIL_SCM_FAILURE
=============================================================================*/

#ifdef PIL_TZ_FILTER
pil_error_type pil_validate_elf(uint8* virt_addr, uint32 size);



/*=============================================================================
FUNCTION 
  pil_validate_all_segments

DESCRIPTION
  Make an scm call to TZ to validate the given segment.
  
        
DEPENDENCIES
  None.
  
INPUTS
  segment - pointer to the segment to be verified
  size - size of the segment

RETURN VALUE
PIL_SUCCESS if scm call returns success else it returns PIL_SCM_FAILURE
=============================================================================*/
pil_error_type pil_validate_all_segments(uint8* segment, uint32 size);


/*=============================================================================
FUNCTION 
  pil_auth_reset

DESCRIPTION
  Make an scm call to TZ to authenticate and reset the modem.
  
        
DEPENDENCIES
  None.
  
INPUTS
  None

RETURN VALUE
PIL_SUCCESS if scm call returns success else it returns PIL_SCM_FAILURE
=============================================================================*/
pil_error_type pil_auth_reset(void);
#endif /* PIL_TZ_FILTER */

/*=============================================================================
FUNCTION 
  pil_cleanup

DESCRIPTION
  Make an scm call to cleanup.
  
        
DEPENDENCIES
  None.
  
INPUTS
  None

RETURN VALUE
Value returned from the scm call
=============================================================================*/
//uint32 pil_cleanup(void);

#endif /* __PIL_SCM_H__ */

