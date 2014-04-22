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

static qct_ms_t* _CreateL4MemSection(MemAddr *VirtualAddr, int prot, uint32 dwLen)
{
   qct_ms_t * msMemsection = NULL;
   qct_ms_attr_t attr;
   //okl4_mapping_attributes_t map_attr;
   //okl4_page_perms_t okl4_prot_flags = OKL4_PAGE_PERMS_RW;
      
   //Set permissions and attributes
   //map_attr.permissions = okl4_prot_flags;
  
   //Init attributes, set mapping size and set attributes
   qct_ms_attr_init(&attr);
   qct_ms_attr_setsize(&attr, dwLen);
   //qct_ms_attr_setmappingattributes(&attr, &map_attr);

   //Set virtual address if provided by user
   if(NULL  != *VirtualAddr)
      qct_ms_attr_setvirtaddr(&attr, *VirtualAddr);

   //Pre-paged 
   qct_ms_attr_setprepaged(&attr, 1);

   //if(OKL4_OK == qct_ms_create_extended(&msMemsection, &attr, 0x0000))
   if(OKL4_OK == qct_ms_create_extended(&msMemsection, &attr, 0x1000))
   //if(OKL4_OK == qct_ms_create(&msMemsection, &attr))
   {
      *VirtualAddr = (MemAddr)okl4_memsection_getbase((okl4_memsection_t *)msMemsection);
   }//return 0 in case of failure, even assign null to memsection
   else
   {
      msMemsection = NULL;
	  *VirtualAddr = NULL;
   }
   
   return msMemsection;

}
/* If addr is NULL, kernel chooses the address to create the mapping; 
   If addr is not NULL, then the kernel takes it as a hint
   about where to place the mapping;created at a nearby page boundary.  
   The address of the new mapping is returned as the result of the call.
*/
//ToDo: Copy data from file if fd and offset is provided
void* sys_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
{
	qct_ms_t *msMemsection = NULL;
	MemAddr VirtualAddr;
	okl4_word_t PageSize = OKL4_DEFAULT_PAGESIZE;
	okl4_page_perms_t okl4_prot_flags = OKL4_PAGE_PERMS_RW;
	okl4_memsection_update_attr_t update_attr;

	//Virutal address
	VirtualAddr= (MemAddr)addr;
	
	//Normalize Virtual address
	if(0 != VirtualAddr) //if user specifies a virtual address
    {
			//Calculate actual Virtual Start Address
            VirtualAddr = (VirtualAddr & ~(PageSize-1));
    }
	
  //Normalize Length, Round size to be on page boundary.
	len = ((PageSize-1)&len) ? ((len+PageSize) & ~(PageSize-1)):len;
   
  //Create memsection
	msMemsection = _CreateL4MemSection(&VirtualAddr, prot, len);
	
	if(msMemsection != 0) 
	{
		//mmap should zero-out the allocated memory
    //Becuase we are about to update permissions fromRW to user provided
    memset((void*)VirtualAddr, 0, len -1);

		/*Set permissions*/
		okl4_prot_flags = _NormalizeProtFlags(prot);
		okl4_memsection_update_attr_init(&update_attr);
		okl4_memsection_update_attr_setpermissions(&update_attr, okl4_prot_flags);
		okl4_memsection_update((memsection_ref_t)msMemsection, &update_attr);

		return (void*)VirtualAddr;
	}
	else return MAP_FAILED;

}


int sys_munmap(void *addr, size_t len) 
{

  qct_ms_t *msMemsection = NULL;

  //Look up memsection
  if (addr)
  {
    msMemsection = (qct_ms_t *)okl4_pd_lookup_memsection(_okl4_env_main_pd, (okl4_word_t)addr);
  }


  //Remove memsection
  if(msMemsection != 0)
  { 
	//return value check
    qct_ms_delete_extended((qct_ms_t *)msMemsection, 0x1000);
    return 0;
  }
  else
  {
    errno = EINVAL;
    return -1;
  }
    
}
