/*=============================================================================

        P E R I P H E R A L   I M A G E   L O A D E R    M O D U L E
                   U T I L I T I E S   A P I S
              
GENERAL DESCRIPTION
  Implements Utilities Apis from external modules for PIL requirements


REGIONAL FUNCTIONS 
  None
  
EXTERNALIZED FUNCTIONS

  pil_utils_init()
    Does any required initializations required to use external apis

  pil_get_timetick()
    Returns current timetick
    
  pil_busywait(usecs)
    simply wait loops for given number of micro seconds.
    
  pil_strcmp(str1, str2)
    compares str1 with str2
  
  pil_flash_open_partition(partition name, elf handle)
    opens flash partition for given partition name
  
  pil_flash_close_partition(elf handle)
    closes flash partition
  
  pil_flash_read_partition(elf handle, dest_buffer, offset, size)
    reads data from partition at given offset for given size and 
    copies to dest_buffer
  
  pil_map_and_get_virt_addr(phys_addr, size, virt_addr, MemObj)
    Maps given physical address for given size in MMU and returns 
    virtual address
  
  pil_unmap_virt_addr(MemObj)
    Unmaps previous given virtual address at MMU

  pil_query_virt_addr(phys_addr, size, virt_addr, MemObj)
    Queries virtual address for a given physical address
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
    None.

Copyright (c) 2016 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/pil_utils.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

09/15/16   ab      File created.

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "pil_utils.h"
#include "pil_os.h"
#include "pil_config.h"
#include "pil.h"
#include "timer.h"
#include "qurt_memory.h"
#include "flash_ftl.h"
#include "pil_elf_loader.h"
#include "msg.h"
#include "err.h"
#include "DDITimetick.h"
#include "npa.h"
#include "busywait.h"
#include "VCSDefs.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
pil_mutex_obj_type pil_utils_mutex;

static DalDeviceHandle *pil_timetick_handle = NULL;
static uint32 pil_tt_freq;

ULogHandle pil_log_err_handle;
ULogHandle pil_log_info_handle;

static npa_client_handle pil_npa_bimc_client = NULL;
static npa_client_handle pil_npa_sleep_client = NULL;
static npa_client_handle pil_npa_cx_client = NULL;
static int32 pil_npa_cmn_clk_votes = 0;

#define PIL_LOG_ERR_BUFFER_SIZE        (2048)
#define PIL_LOG_INFO_BUFFER_SIZE       (8192)

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================
FUNCTION 
  pil_get_timetick

DESCRIPTION
  This function returns current timetick

DEPENDENCIES
  pil_utils_init should be called before.

RETURN VALUE
  64 bit timetick
=============================================================================*/
uint64 pil_get_timetick(void)
{
  uint64 tt;
  DalTimetick_GetTimetick64(pil_timetick_handle, &tt);
  return tt;
} /* pil_get_timetick */


/*=============================================================================
FUNCTION 
  pil_busywait

DESCRIPTION
  This function wait loops for given number of usecs and returns

DEPENDENCIES
  pil_utils_init should be called before.

RETURN VALUE
  64 bit timetick
=============================================================================*/
void pil_busywait(uint32 pause_time_us)
{
  busywait(pause_time_us);

  //PIL_LOG_LOW(2,"pil_busywait: pause_time: %d, end timetick: %d, start timetick: %d", pause_time_us, pil_get_timetick());
  return;
} /* pil_busywait */


/*=============================================================================
FUNCTION 
  pil_strcmp

DESCRIPTION
  This function compares two strings

DEPENDENCIES
  None

RETURN VALUE
  0 if they are same
  non-zero in case they are different
=============================================================================*/
int pil_strcmp(const char *s1, const char *s2)
{
  return strcmp( s1, s2 );
} /* pil_strcmp */


/*=============================================================================
FUNCTION 
  pil_map_and_get_virt_addr

DESCRIPTION
  This function maps given physical address for given size in MMU and returns
  virtual address

DEPENDENCIES
  None

RETURN VALUE
  PIL_INVALID_PARAMETERS
  PIL_MAPPING_FAILED
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_map_and_get_virt_addr(uint64 phys_addr, uint64 size, uint64 *virt_addr, void *MemObj)
{
#ifdef PIL_DALSYS_AVAILABLE
  static DALSYSMemInfo DalMemInfo;

  if(phys_addr == 0 || size == 0 || virt_addr == NULL)
    return PIL_INVALID_PARAMETERS;
    
  //This is needed only in case when we are using Physical address
  
  //Todo: Need to add this memory range to quartz.xml at build\ms folder.. 
  if(DAL_SUCCESS != DALSYS_MemRegionAlloc( DALSYS_MEM_PROPS_UNCACHED | DALSYS_MEM_PROPS_PHYS_CONT, 
                                           DALSYS_MEM_ADDR_NOT_SPECIFIED, 
                                           phys_addr, size, (DALSYSMemHandle *)MemObj, NULL))    //Todo: Actually phdr_entry_paddr is 64bit type.. At last make sure the code works for both 64bit and 32 bit types
                                           
  {
     PIL_LOG_ERR(3,"failed to map phys addr 0x%x for size %d", (uint32)phys_addr, (uint32)size, 0);
     return PIL_MAPPING_FAILED;
  }
  
  if(DAL_SUCCESS != DALSYS_MemInfo((*(DALSYSMemHandle *)MemObj), &DalMemInfo))
  {
     PIL_LOG_ERR(3,"failed to get MemInfo for phys addr 0x%x and size %d", (uint32)phys_addr, (uint32)size, 0);
     return PIL_MAPPING_FAILED;
  }
  
  //This is because DALSYS_MemRegionAlloc() api will align the given physical address to page boundary 
  //and gets virtual address from there
  PIL_VERIFY((DalMemInfo.dwLen >= size && DalMemInfo.PhysicalAddr <= phys_addr), "Dalsys assumption gone wrong", 0, 0, 0);
  
  *virt_addr = (phys_addr - DalMemInfo.PhysicalAddr) + DalMemInfo.VirtualAddr;
  
  PIL_LOG_LOW(3,"given phys addr: 0x%x, calc phys addr: 0x%x, given size: %d", (uint32)phys_addr, (uint32)DalMemInfo.PhysicalAddr, (uint32)size);
  PIL_LOG_LOW(3,"obtained virt addr: 0x%x, calc virt addr: 0x%x, calc size: %d", (uint32)DalMemInfo.VirtualAddr, (uint32)*virt_addr, (uint32)DalMemInfo.dwLen);
  
  //Copy the Virtual addr for copying segment data
  //*virt_addr = DalMemInfo.VirtualAddr;
  
  if(*virt_addr == 0)
  {
     PIL_LOG_ERR(3,"Virt Addr obtained is NULL", (uint32)phys_addr, (uint32)size, 0);
     return PIL_MAPPING_FAILED;
  }
  
  //Need to touch memory to get mapped in page table for Arm11
  DALSYS_memset((void *)(unsigned int)*virt_addr, 0xFF, 4);
  
#else
  qurt_mem_region_attr_t mem_attr;
  int ret;
  static  qurt_mem_pool_t pil_mmu_pool;
  static uint32 pil_mmu_init = 0;
  
  if(phys_addr == 0 || size == 0 || virt_addr == NULL)
    return PIL_INVALID_PARAMETERS;
    
  qurt_mem_region_attr_init(&mem_attr);
    
  PIL_LOG_LOW(2,"given phys addr: 0x%x, given size: %d", (uint32)phys_addr, (uint32)size);
  
  if(pil_mmu_init == 0)
  {
    ret = qurt_mem_pool_attach("STATIC_POOL", &pil_mmu_pool);
    PIL_VERIFY(ret == QURT_EOK, "qurt_mem_pool_attach returned error", ret, 0, 0);
    
    pil_mmu_init = 1;
  }
  
  mem_attr.permission    = QURT_PERM_READ_WRITE;
  mem_attr.physaddr      = phys_addr;
  mem_attr.size          = size;
  mem_attr.cache_attribs = QURT_MEM_CACHE_OUTER_INNER_UNCACHED; // Need uncached memory
  mem_attr.mapping_type  = QURT_MEM_MAPPING_PHYS_CONTIGUOUS; // Need contiguous memory
    
  ret = qurt_mem_region_create(MemObj, size, pil_mmu_pool, &mem_attr);
  if (ret != QURT_EOK) {
      //ERR_FATAL("Unable to create mem region!", size, ret, 0);
      PIL_LOG_ERR(3,"qurt_mem_region_create returned error %d for phys addr: 0x%x, given size: %d", ret, (uint32)phys_addr, (uint32)size);
      return PIL_MAPPING_FAILED;
  }
  
  ret = qurt_mem_region_attr_get((*(qurt_mem_region_t *)MemObj), &mem_attr);
  if (ret != QURT_EOK) {
      //ERR_FATAL("Unable to create mem region!", size, ret, 0);
      PIL_LOG_ERR(3,"qurt_mem_region_attr_get returned error %d for phys addr: 0x%x, given size: %d", (uint32)ret, (uint32)phys_addr, (uint32)size);
      return PIL_MAPPING_FAILED;
  }
  
  *virt_addr = mem_attr.virtaddr;
  
  //This is because qurt_mem_region_create() api will align the given physical address to page boundary 
  //and gets virtual address from there
  PIL_VERIFY((mem_attr.size >= size && mem_attr.physaddr <= phys_addr), "Qurt mem assumption gone wrong", (uint32)phys_addr, (uint32)size, 0);

  if (mem_attr.physaddr != phys_addr) {
    //happens if input phys address is not 4K aligned
    uint32 offset = phys_addr & 0xFFF; //our offset from the 4K page
    *virt_addr += offset;
  }

  PIL_LOG_LOW(3,"given phys addr: 0x%x, calc phys addr: 0x%x, given size: %d", (uint32)phys_addr, (uint32)mem_attr.physaddr, (uint32)size);
  PIL_LOG_LOW(3,"obtained virt addr: 0x%x, calc virt addr: 0x%x, obtained size: %d", (uint32)mem_attr.virtaddr, (uint32)*virt_addr, (uint32)mem_attr.size);

#endif
  
  return PIL_SUCCESS;
} /* pil_map_and_get_virt_addr */


/*=============================================================================
FUNCTION 
  pil_unmap_virt_addr

DESCRIPTION
  Unmaps previously provided virtual address in MMU

DEPENDENCIES
  None

RETURN VALUE
  PIL_INVALID_PARAMETERS
  PIL_UNMAPPING_FAILED
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_unmap_virt_addr(void *MemObj)
{
  #ifdef PIL_DALSYS_AVAILABLE
   if(DAL_SUCCESS != DALSYS_DestroyObject((*(DALSYSMemHandle *)MemObj)))
     return PIL_UNMAPPING_FAILED;
 #else
   if(qurt_mem_region_delete((*(qurt_mem_region_t *)MemObj)) != QURT_EOK)
     return PIL_UNMAPPING_FAILED;
 #endif
   return PIL_SUCCESS; 
} /* pil_unmap_virt_addr */


/*=============================================================================
FUNCTION 
  pil_query_virt_addr

DESCRIPTION
  Queries virtual address for a given physical address from MMU. This is used
  for Imem memory since that will be already mapped.

DEPENDENCIES
  None

RETURN VALUE
  PIL_INVALID_PARAMETERS
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_query_virt_addr(uint64 phys_addr, uint64 size, uint64 *virt_addr, void *MemObj)
{
#ifndef PIL_DALSYS_AVAILABLE
  qurt_mem_region_attr_t mem_attr;
  int ret;

  if(phys_addr == 0 || size == 0 || virt_addr == NULL)
    return PIL_INVALID_PARAMETERS;

  qurt_mem_region_attr_init(&mem_attr);
    
  PIL_LOG_LOW(3,"pil_query_virt_addr: given phys addr: 0x%x, given size: %d", phys_addr, size, 0);
  
  ret = qurt_mem_region_query(MemObj, QURT_INVALID_ADDR, phys_addr);
  if (ret != QURT_EOK) {
      //ERR_FATAL("Unable to create mem region!", size, ret, 0);
      PIL_LOG_ERR(3,"pil_query_virt_addr: qurt_mem_region_query returned error %d for phys addr: 0x%x, given size: %d", ret, phys_addr, size);
      return PIL_MAPPING_FAILED;
  }
  
  ret = qurt_mem_region_attr_get((*(qurt_mem_region_t *)MemObj), &mem_attr);
  if (ret != QURT_EOK) {
      //ERR_FATAL("Unable to create mem region!", size, ret, 0);
      PIL_LOG_ERR(3,"pil_query_virt_addr: qurt_mem_region_attr_get returned error %d for phys addr: 0x%x, given size: %d", ret, phys_addr, size);
      return PIL_MAPPING_FAILED;
  }
  
  *virt_addr = mem_attr.virtaddr;    
  
  //This is because DALSYS_MemRegionAlloc() api will align the given physical address to page boundary 
  //and gets virtual address from there
  PIL_VERIFY((mem_attr.size >= size && mem_attr.physaddr <= phys_addr), "Qurt mem assumption gone wrong", 0, 0, 0);

  if (mem_attr.physaddr != phys_addr) {
    //Todo: Check this assumption for Qurt_mapping_create.. Imp
    //could happen if our address is not 4K aligned
    uint32 offset = phys_addr & 0xFFF; //our offset from the 4K page
    *virt_addr += offset;
  }

  PIL_LOG_LOW(3,"pil_query_virt_addr: given phys addr: 0x%x, calc phys addr: 0x%x, given size: %d", (uint32)phys_addr, (uint32)mem_attr.physaddr, (uint32)size);
  PIL_LOG_LOW(3,"pil_query_virt_addr: obtained virt addr: 0x%x, calc virt addr: 0x%x, obtained size: %d", (uint32)mem_attr.virtaddr, (uint32)*virt_addr, (uint32)mem_attr.size);
#endif
  return PIL_SUCCESS;
} /* pil_query_virt_addr */


/*=============================================================================
FUNCTION 
  pil_flash_open_partition

DESCRIPTION
  Opens flash partition for given name and updates flash handle in elf structure

DEPENDENCIES
  None

RETURN VALUE
  PIL_PARTITION_OPEN_FAILURE
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_flash_open_partition(pil_elf_struct_type *handle, char* partition_name)
{
  FLASH_FTL_STATUS status;
  status = flash_ftl_open(&handle->flash_if, (const uint8 *)partition_name);
  
  if(status != FLASH_FTL_OK)
  {
    PIL_LOG_ERR(2,"pil_flash_open_partition: Failed to open with error %d partition %s", status, partition_name);
    return PIL_PARTITION_OPEN_FAILURE;
  }
  
  return PIL_SUCCESS;
} /* pil_flash_open_partition */


/*=============================================================================
FUNCTION 
  pil_flash_close_partition

DESCRIPTION
  Closes previously opened flash partition 

DEPENDENCIES
  None

RETURN VALUE
  PIL_PARTITION_OPEN_FAILURE
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_flash_close_partition(pil_elf_struct_type *handle)
{
  FLASH_FTL_STATUS status;
  status = flash_ftl_close(&handle->flash_if);
  if(status != FLASH_FTL_OK)
  {
    PIL_LOG_ERR(3,"pil_flash_closed_partition: Failed to close partition with error %d", status, 0, 0);
    return PIL_FAILURE;
  }
  
  return PIL_SUCCESS;
} /* pil_flash_close_partition */


/*=============================================================================
FUNCTION 
  pil_flash_read_partition

DESCRIPTION
  Reads data from flash storage from given offset for given size and copies to 
  given buffer. Assumption is buffer has memory of given size.

DEPENDENCIES
  None

RETURN VALUE
  PIL_FLASH_READ_FAILED
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_flash_read_partition(
   pil_elf_struct_type *handle,    
   void   *dest_addr,              /* Destination address */
   uint32  byte_offset,            /* Logical source byte address */
   uint32  size                    /* Size to copy in bytes */
)
{
  FLASH_FTL_STATUS status;
  
  status = flash_ftl_read_bytes(handle->flash_if, byte_offset, size, dest_addr);
  if(status != FLASH_FTL_OK)
  {
    PIL_LOG_ERR(3,"pil_flash_read_partition: Failed to read partition %d at offset %d for size %d", handle->flash_if, byte_offset, size);
    return PIL_FLASH_READ_FAILED;
  }
  return PIL_SUCCESS;
} /* pil_flash_read_partition */


/*=============================================================================
FUNCTION 
  pil_common_clocks_vote

DESCRIPTION
  Votes for common system clocks like BIMC etc

DEPENDENCIES
  NPA handle should be initialized

RETURN VALUE
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_common_clocks_vote(void)
{
   uint32 prev_vote_cnt;
   
   PIL_LOG_LOW(1, "pil_clocks_vote called. current votes: %d", pil_npa_cmn_clk_votes);
   
   pil_os_mutex_lock(&pil_utils_mutex);
   
   prev_vote_cnt = pil_npa_cmn_clk_votes++;
   
   pil_os_mutex_unlock(&pil_utils_mutex);

   if(prev_vote_cnt != 0)
     return PIL_SUCCESS;
   
   PIL_LOG_LOW(2, "voting for commong clks. vote_cnt: %d, prev_vote_cnt: %d", pil_npa_cmn_clk_votes, prev_vote_cnt);
   
   /* Voting for BIMC as in cases of usb not connected, system is running at very slow
      frequency and pil loading of modem is taking few minutes instead of few secs */
   npa_issue_required_request(pil_npa_bimc_client, PIL_BIMC_NOMINAL_MODE_SPEED_KHZ);
  
  //Todo: if voting for xo, cx are specific to modem, move to appropriate file/section
  //vote against sleep till modem is loaded
  if(pil_npa_sleep_client)
      npa_issue_required_request(pil_npa_sleep_client, 1);

  //Todo: Move this to pil_modem.c
  //Vote for cx rail
  if(pil_npa_cx_client)
      npa_issue_required_request(pil_npa_cx_client, VCS_CORNER_NOMINAL);
   
   return PIL_SUCCESS;
} /* pil_common_clocks_vote */


/*=============================================================================
FUNCTION 
  pil_common_clocks_devote

DESCRIPTION
  Devotes for common system clocks like BIMC etc

DEPENDENCIES
  pil_common_clocks_vote should be called before calling this

RETURN VALUE
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_common_clocks_devote(void)
{
  uint32 vote_cnt;

  PIL_LOG_LOW(1, "pil_clocks_devote called. current votes: %d", pil_npa_cmn_clk_votes);
   
   pil_os_mutex_lock(&pil_utils_mutex);
   
   vote_cnt = --pil_npa_cmn_clk_votes;
   
   PIL_VERIFY( pil_npa_cmn_clk_votes>=0, "pil_npa_cmn_clk_votes(%d) value gone -ve", pil_npa_cmn_clk_votes, 0, 0 );
   
   pil_os_mutex_unlock(&pil_utils_mutex);

   if(vote_cnt != 0)
     return PIL_SUCCESS;
   
   PIL_LOG_LOW(1, "devoting for commong clks. vote_cnt: %d", pil_npa_cmn_clk_votes);
   
  if(pil_npa_sleep_client)
      npa_complete_request(pil_npa_sleep_client);

  if(pil_npa_cx_client)
      npa_complete_request(pil_npa_cx_client);
    
   npa_complete_request(pil_npa_bimc_client);
   
   return PIL_SUCCESS;
} /* pil_common_clocks_devote */


/*=============================================================================
FUNCTION 
  pil_npa_sleep_handle_cb

DESCRIPTION
  Creates NPA client for /core/cpu/vdd node to vote for/against sleep

DEPENDENCIES
  None

RETURN VALUE
  None
=============================================================================*/
static void pil_npa_sleep_handle_cb( void *context,
                                  unsigned int event_type,
                                  void *data,
                                  unsigned int data_size)
{
   if(!pil_npa_sleep_client)
   {
      pil_npa_sleep_client = npa_create_sync_client( "/core/cpu/vdd",
                                            "PIL_SLEEP_CLIENT",
                                            NPA_CLIENT_REQUIRED);
      PIL_VERIFY(pil_npa_sleep_client != NULL, "got NULL handle for NPA sleep clock client", pil_npa_sleep_client, 0, 0);
   }
}

/*=============================================================================
FUNCTION 
  pil_npa_cx_handle_cb

DESCRIPTION
  Creates NPA client for /vdd/cx node

DEPENDENCIES
  None

RETURN VALUE
  None
=============================================================================*/
static void pil_npa_cx_handle_cb( void *context,
                                  unsigned int event_type,
                                  void *data,
                                  unsigned int data_size)
{
   if(!pil_npa_cx_client)
   {
      pil_npa_cx_client = npa_create_sync_client( "/vdd/cx",
                                            "PIL_CX_CLIENT",
                                            NPA_CLIENT_REQUIRED);
      PIL_VERIFY(pil_npa_cx_client != NULL, "got NULL handle for NPA CX client", pil_npa_cx_client, 0, 0);
   }
}


/*=============================================================================
FUNCTION 
  pil_utils_init

DESCRIPTION
  Does any required initialization of external modules that PIL uses. This
  should be called once before using any other pil_utils function.

DEPENDENCIES
  None

RETURN VALUE
  PIL_SUCCESS
=============================================================================*/
pil_error_type pil_utils_init(void)
{
  //Initialize pil utils mutex
  pil_os_mutex_init(&pil_utils_mutex);
  
  //Initialize timetick api
  DalTimetick_Attach("SystemTimer", &pil_timetick_handle);
  
  PIL_VERIFY(pil_timetick_handle != NULL, "Timetick Attach failed", 0, 0, 0);
  
  //Get Timetick Frequency
  DalTimetick_GetFreq(pil_timetick_handle, &pil_tt_freq);
  
  PIL_VERIFY(pil_tt_freq != 0, "Timetick Frequency obtained is 0", pil_tt_freq, 0, 0);
  
  
  /* Initialize uLOG buffer */
  ULogFront_RealTimeInit(&pil_log_info_handle,
                         "SUBSYS_PIL_INFO_LOG",
                         PIL_LOG_INFO_BUFFER_SIZE,
                         ULOG_MEMORY_LOCAL,
                         ULOG_LOCK_OS);

  ULogFront_RealTimeInit(&pil_log_err_handle,
                         "SUBSYS_PIL_ERR_LOG",
                         PIL_LOG_ERR_BUFFER_SIZE,
                         ULOG_MEMORY_LOCAL,
                         ULOG_LOCK_OS);

  //NPA registrations
  {
     pil_npa_bimc_client = npa_create_sync_client("/clk/bimc", "SUBSYS_PIL_BIMC_CLIENT", NPA_CLIENT_REQUIRED);
     PIL_VERIFY(pil_npa_bimc_client != NULL, "got NULL handle for NPA BIMC clock client", pil_npa_bimc_client, 0, 0);

     npa_resource_available_cb( "/core/cpu/vdd", pil_npa_sleep_handle_cb, NULL);
     npa_resource_available_cb( "/vdd/cx", pil_npa_cx_handle_cb, NULL);
  }
  
  

  PIL_LOG_INFO(1, "PIL module is initialized. timetick freq: %d", pil_tt_freq);
  
  return PIL_SUCCESS;
} /* pil_utils_init */

