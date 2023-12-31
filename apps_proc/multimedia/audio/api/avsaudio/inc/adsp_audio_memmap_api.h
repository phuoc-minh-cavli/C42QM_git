/*========================================================================*/
/**
@file adsp_audio_memmap_api.h

This file contains the structure definitions for the memory map 
commands used in client-ASM,ADM communications.
*/

/*===========================================================================
NOTE: The description above does not appear in the PDF. 

      The Audio_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and Latex. To edit or update 
      any of the file/group text in the PDF, edit the Audio_mainpage.dox file 
      contact Tech Pubs.
===========================================================================*/

/*===========================================================================
Copyright (c) 2010-2012 Qualcomm Technologies, Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
======================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/audio.tx/2.0/audio/api/avsaudio/inc/adsp_audio_memmap_api.h#1 $

when       who     what, where, why
--------   ---      -------------------------------------------------------
08/03/12   sw      (Tech Pubs) Edited Doxygen comments for 2.0 updates;
                   updated Doxygen markup to Dox 1.7.6.1.
05/30/11   sw      (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/12/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
07/06/10   kk      Created file.

========================================================================== */

#ifndef _ADSP_AUDIO_MEMMAP_API_H_
#define _ADSP_AUDIO_MEMMAP_API_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup asm_svc_cmd_mem_map
@{ */

/** Enumeration for the shared memory 8_4K pool ID. */
#define ADSP_MEMORY_MAP_SHMEM8_4K_POOL      3

/** Definition of the is_virtual flag shift value. */
#define ADSP_MEMORY_MAP_SHIFT_IS_VIRTUAL                          0

/** Definition of the is_virtual flag bitmask. */
#define ADSP_MEMORY_MAP_BIT_MASK_IS_VIRTUAL                       (0x00000001UL)

/** Definition of the virtual memory flag. */
#define ADSP_MEMORY_MAP_VIRTUAL_MEMORY                         1

/** Definition of the physical memory flag. */
#define ADSP_MEMORY_MAP_PHYSICAL_MEMORY                        0

/** @} */ /* end_addtogroup asm_svc_cmd_mem_map */


#include "adsp_begin_pack.h"

/** @weakgroup weak_avs_cmd_shared_mem_map_regions_t
@{ */
/* Memory map regions command payload used by the
    ASM_CMD_SHARED_MEM_MAP_REGIONS and ADM_CMD_SHARED_MEM_MAP_REGIONS commands. */
/** This structure is used to map multiple shared memory regions in a single
    command. Following this structure are num_regions of
    avs_shared_map_region_payload_t.
*/
struct avs_cmd_shared_mem_map_regions_t
{
    uint16_t                  mem_pool_id;
    /**< Type of memory on which this memory region is mapped.

         Supported values:
         - #ADSP_MEMORY_MAP_SHMEM8_4K_POOL
         - Other values are reserved
         
         The memory ID implicitly defines the characteristics of the memory.
         Characteristics may include alignment type, permissions, etc.

         SHMEM8_4K is shared memory, byte addressable, and 4 KB aligned. */
    
    uint16_t                  num_regions;
    /**< Number of regions to map.*/

    uint32_t                  property_flag;
    /**< Configures one common property for all regions in the payload. No
         two regions in the same #ASM_CMD_SHARED_MEM_MAP_REGIONS or
         #ADM_CMD_SHARED_MEM_MAP_REGIONS can have different properties.
    
         @values: 0x00000000 to 0x00000001

         Bit 0 is the IsVirtual flag that indicates physical or virtual
         mapping:
         - 0 -- The shared memory address provided in
                avs_shared_map_regions_payload is a physical address. The
                shared memory must be mapped (a hardware TLB entry), and a
                software entry must be added for internal bookkeeping.
                
         - 1 -- The shared memory address provided in the map
                payload[usRegions] is a virtual address. The shared memory
                must not be mapped (because the hardware TLB entry is already
                available), but a software entry must be added for internal
                bookkeeping. \n
                This is useful if two services within the aDSP are
                communicating via APR. They can directly communicate via
                the virtual address instead of a physical address. The virtual
                regions must be contiguous.    
                
         Bits 31 to 1 are reserved and must be set to zero. */
} 
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avs_cmd_shared_mem_map_regions_t */

#include "adsp_begin_pack.h"

/** @weakgroup weak_avs_shared_map_region_payload_t
@{ */
/* Payload of the map region; used by the avs_cmd_shared_mem_map_regions_t
    structure.
*/
struct avs_shared_map_region_payload_t
{
    uint32_t                  shm_addr_lsw;
    /**< Lower 32 bits of the shared memory address of the memory region to
         map. This word must be contiguous memory, and it must be
         4&nbsp;KB aligned. */

    uint32_t                  shm_addr_msw;
    /**< Upper 32 bits of the shared memory address of the memory region to
         map. This word must be contiguous memory, and it must be
         4&nbsp;KB aligned.

         For a 32-bit shared memory address, this field must be set to zero.

         For a 36-bit shared memory address, bits 31 to 4 must be set to zero. */
         
    uint32_t                  mem_size_bytes;
    /**< Number of bytes in the region.

         The aDSP always maps the regions as virtual contiguous memory, but
         the memory size must be in multiples of 4 KB to avoid gaps in the
         virtually contiguous mapped memory. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avs_shared_map_region_payload_t */

#include "adsp_begin_pack.h"

/** @weakgroup weak_avs_cmd_shared_mem_unmap_regions_t
@{ */
/* Payload of the memory unmap regions command used by
    ASM_CMD_SHARED_MEM_UNMAP_REGIONS and ADM_CMD_SHARED_MEM_UNMAP_REGIONS.
*/
struct avs_cmd_shared_mem_unmap_regions_t
{
    uint32_t                  mem_map_handle;
    /**< Unique identifier for an address. This memory map handle is returned
         by the aDSP through the #ASM_CMD_SHARED_MEM_MAP_REGIONS or
         #ADM_CMD_SHARED_MEM_MAP_REGIONS command. The aDSP uses this handle to
         retrieve the associated shared memory regions and unmap them. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avs_cmd_shared_mem_unmap_regions_t */

#include "adsp_begin_pack.h"

/** @weakgroup weak_avs_cmdrsp_shared_mem_map_regions_t
@{ */
/* Payload of the memory map command response used by
    ASM_CMDRSP_SHARED_MEM_MAP_REGIONS and ADM_CMDRSP_SHARED_MEM_MAP_REGIONS. */
/** This structure is an AVS response to the memory mapping regions command.
    The mem_map_handle field can later be used by the client to unmap regions
    that were previously mapped.
*/
struct avs_cmdrsp_shared_mem_map_regions_t
{
    uint32_t                  mem_map_handle;
    /**< Unique identifier for an address. This memory map handle is returned
         by the aDSP through the #ASM_CMD_SHARED_MEM_MAP_REGIONS or
         #ADM_CMD_SHARED_MEM_MAP_REGIONS command.
         
         This handle can be used as an abstract representation of the shared
         memory regions that are being mapped. The aDSP uses this handle to
         retrieve the shared memory attributes. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avs_cmdrsp_shared_mem_map_regions_t */


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_AUDIO_MEMMAP_API_H_ */
