/*==============================================================================

Dynamic Loading Support Libs

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

--------------------------------------------------------------------------------
 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when               who         what, where, why
 --------           ---         -----------------------------------------------
 1/22/15          amitkulk      created 
==============================================================================*/
#include <errno.h>
#include <assert.h>
#include <kernel.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "sys_mman.h"


static okl4_page_perms_t _NormalizeProtFlags(int prot)
{
   okl4_page_perms_t okl4_prot_flags = OKL4_PAGE_PERMS_RW;
   //Normalize Protection flags
   switch(prot)
   {
		case PROT_NONE:
			okl4_prot_flags = OKL4_PAGE_PERMS_NONE;
			break;
		case PROT_EXEC:
			okl4_prot_flags = OKL4_PAGE_PERMS_X;
			break;
		case PROT_WRITE:
			okl4_prot_flags = OKL4_PAGE_PERMS_W;
			break;
		case PROT_READ:
			okl4_prot_flags = OKL4_PAGE_PERMS_R;
			break;
		case (PROT_READ | PROT_EXEC) :
			okl4_prot_flags = OKL4_PAGE_PERMS_RX;
			break;
		case (PROT_READ | PROT_WRITE) :
			okl4_prot_flags = OKL4_PAGE_PERMS_RW;
			break;
		case (PROT_EXEC | PROT_WRITE) :
			okl4_prot_flags = OKL4_PAGE_PERMS_RW;
			break;
    	case (PROT_READ | PROT_WRITE | PROT_EXEC):
      		okl4_prot_flags = OKL4_PAGE_PERMS_RX;
      		break;
		default:
			okl4_prot_flags = OKL4_PAGE_PERMS_RW;
      		break;
   }
   
   return okl4_prot_flags;
}


int sys_mprotect (void *addr, size_t len, int prot)
{

	qct_ms_t *msMemsection = NULL;
	okl4_page_perms_t okl4_prot_flags = OKL4_PAGE_PERMS_RW;
	okl4_memsection_update_attr_t update_attr;

	okl4_prot_flags = _NormalizeProtFlags(prot);

	//Look up memsection
	if (addr)
	{
		msMemsection = (qct_ms_t *)okl4_pd_lookup_memsection(_okl4_env_main_pd, (okl4_word_t)addr);
		
		if(msMemsection!=0)
		{
			okl4_memsection_update_attr_init(&update_attr);
			okl4_memsection_update_attr_setpermissions(&update_attr, okl4_prot_flags);
			//okl4_memsection_update((memsection_ref_t)msMemsection, &update_attr);
			okl4_memsection_update_extended((memsection_ref_t)msMemsection, &update_attr, 0x1000, (okl4_word_t)addr, len);
			return 0;
		}
	}
	return -1;
}
