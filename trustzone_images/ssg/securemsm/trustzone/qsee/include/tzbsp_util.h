#ifndef TZBSP_UTIL_H
#define TZBSP_UTIL_H

/**
@file tzbsp_util.h
@brief Utility APIs

Contains common defintion utility APIs in QSEE.

*/
/*===========================================================================
   Copyright (c) 2016,2023 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <comdef.h>
typedef uintptr_t reg_t;

typedef union 
{
  struct {
    reg_t id;
    reg_t param[13];
  }smc;

  struct {
    reg_t id;
    reg_t sub_cmd;
    reg_t param[4];
    reg_t session_id;
    reg_t elr;
    reg_t hcr;
    reg_t spsr;
    reg_t scr;
    reg_t qsee_cmn_entry_addr;
    reg_t qsee_boot_sp;
    reg_t reserved;
  }mon_call;
}tzbsp_cmn_param_t;

/**
 * Structure for mapping the DDR regions, which will be used for DDR pagetable
 * entries, and verifying pointers/structures reside in DDR.
 */
typedef struct tzbsp_ddr_region_s
{
  /* Start address of the DDR region */
  uintptr_t start;
  /* Size of the memory region, in bytes */
  size_t size;
} tzbsp_ddr_region_t;

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/**
 * Returns number of cpus in a cluster. 
 *  
 * @param cluster_num Cluster number and it has to be less than 
 *                 TZBSP_CLUSTER_COUNT
 * @returns number of cores
 */
uint32_t tzbsp_get_num_cpus_in_cluster(uint32_t cluster_num);

/**
 * Returns number of clusters in SOC. 
 *  
   @returns number of clusters
*/
uint32_t tzbsp_get_cluster_num(void);

/**
 * Sets MMU lock address. (Only applicable to Bear Targets) 
 * TODO: work with QIPL team to remove this api 
 *  
 * @param lock_addr pointer to mmu lock address
 * @returns None.
 */
void tzbsp_set_qsee_mmu_lock_addr_arch(void** lock_addr);

/**
 * Sets the DDR Size, at runtime, to account for various HW configurations.
 */
int tzbsp_read_ddr_size(uint32_t*           num_ddr_regions,
                        tzbsp_ddr_region_t* ddr_regions,
                        uint32_t            ddr_regions_sz);

/**
 * Returns boot milestone state
 * @returns true if boot milestone is reached, false otherwise
 */
bool tzbsp_get_boot_milestone_state(void);

/**
 * Sets boot milestone state as complete
 * @returns None.
 */
void tzbsp_boot_milestone_complete(void);

/**
 * This API logs the error into the TZ Diag, sets the dload mode cookie,
 * and triggers a WDB. It is called when an error occurs during pre-mmu
 * cold init.
 *
 * @param [in] err   Error to be logged
 */
void tzbsp_cold_boot_assert(uint32_t err);

/**
   Acquire HWIO lock and disable interrupt
   @returns interrupt mask.
 */
uint32_t tzbsp_acquire_hwio_lock_int_disable(void);

/**
   Release HWIO lock and restore interrupt mask
   @param int_mask interrupt mask returned by 
                   tzbsp_acquire_hwio_lock_int_disable
   @returns None.
 */
void tzbsp_release_hwio_lock_int_restore(uint32_t int_mask);

/**
 *  \brief zero out memory using VFP NEON
 *  
 *  \param [in] dst : pointer to address
 *  \param [in] size : size that needs to be zero'ed
 *  \return success/failure
 *  
 */
int tzbsp_memzi(void * dst, uint32_t size);

/**
 *
 * \brief api to detect if its a mav target
 * \return true/false
 *
 */
bool tzbsp_is_mav_target(void);

/**
 * Returns the status whether to update rollback version
 * or not for boot loader, pil images and secure apps
 * @returns true if anti rollback version can be updated 
 * This api should be called by hlos/uefi 
 * in order to update rollback version
 */
bool tzbsp_update_image_rollback_version(void);

#endif /* TZBSP_UTIL_H */
