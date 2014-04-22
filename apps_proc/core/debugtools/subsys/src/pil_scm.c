/*======================================================================
                        pil_scm.c 

GENERAL DESCRIPTION
  PIL SCM calls

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2020  by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "scm.h"
#include "qurt_memory.h"
#include "qurt_error.h"
#include "pil_scm.h"
#include "pil_utils.h"
#include "pil.h"

/*======================================================================
                          GLOBALS
=======================================================================*/


/*======================================================================
                          FUNCTIONS
=======================================================================*/

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
pil_error_type pil_validate_elf(uint8* virt_addr, uint32 size)
{
	uint32 res, smc_id;
	struct scm_desc desc = {0};
	
	smc_id = TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL, PIL_INIT_IMAGE_SCM_ID);
	res = qurt_mem_cache_clean((qurt_addr_t)virt_addr,size,QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE);
	
	desc.arginfo = PIL_INIT_IMAGE_PARAM_ID;
	desc.args[0] = MODEM_PROC_ID;
	desc.args[1] = qurt_lookup_physaddr((qurt_addr_t)virt_addr);
	res = scm_call2(smc_id,&desc);

	if(res!=PIL_SUCCESS) // Non zero value indicates an error
	{
	  PIL_LOG_ERR(1,"scm error code is %d. 0 means success, anything other than 0 is an error", res);
	  return PIL_SCM_FAILURE;
	}
	
	return PIL_SUCCESS;
}

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
pil_error_type pil_validate_all_segments(uint8* segment, uint32 size)
{
	uint32 res, smc_id;
	struct scm_desc desc = {0};
	
	res = qurt_mem_cache_clean((qurt_addr_t)segment,size,QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE);

	smc_id = TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL, PIL_MEM_SETUP_SCM_ID);
	desc.arginfo = PIL_MEM_SETUP_PARAM_ID;
	desc.args[0] = MODEM_PROC_ID;
	desc.args[1] = qurt_lookup_physaddr((qurt_addr_t)segment);
	desc.args[2] = (uint32)size;
	res = scm_call2(smc_id,&desc);

	if(res!=PIL_SUCCESS) // Non zero value indicates an error
	{
	  PIL_LOG_ERR(1,"scm error code is %d. 0 means success, anything other than 0 is an error", res);
	  return PIL_SCM_FAILURE;
	}
	
	return PIL_SUCCESS;
}

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
pil_error_type pil_auth_reset(void)
{
	uint32 res, smc_id;
	struct scm_desc desc = {0};
	
	smc_id = TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_SIP, TZ_SVC_PIL, PIL_AUTH_RESET_SCM_ID);
	desc.arginfo = PIL_AUTH_RESET_PARAM_ID;
	desc.args[0] = MODEM_PROC_ID;
	res = scm_call2(smc_id,&desc);

	if(res!=PIL_SUCCESS) // Non zero value indicates an error
	{
	  PIL_LOG_ERR(1,"scm error code is %d. 0 means success, anything other than 0 is an error", res);
	  return PIL_SCM_FAILURE;
	}
	
	return PIL_SUCCESS;
}

#endif /* PIL_TZ_FILTER */

