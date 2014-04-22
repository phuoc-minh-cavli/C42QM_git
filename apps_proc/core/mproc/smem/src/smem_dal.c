/*===========================================================================

                         S H A R E D   M E M O R Y

DESCRIPTION
    Defines the OS and API specific helper functions for SMEM API.  DAL is 
    fully supported by all ModemSW and 8974 ADSP, which both also happen to have 
    equivalent memory-mapping requirements.

Copyright (c) 2014-2015 by Qualcomm Technologies, Incorporated.  All 
Rights Reserved.

===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/25/15   rv      Removed software/asm spinlock support.
11/22/13   rv      VA mapping is added for HW WONCE rsegion.
11/19/13   bt      Rename IMEM defines to more-generic target info.
05/31/13   bt/bm   Use DAL to map SMEM region.
02/08/13   bt      KW error: Add assertion for return of DAL_DeviceAttach.
01/09/13   pa      Added smem_set_spin_lock_api.
12/04/12   bt      Separate smem_map_memory_va into Qurt and Rex versions.
10/05/12   bt      Resolve Klocwork error in reference of hwio_mutexes_handle.
06/14/12   bm      Remove macro SMEM_RPM_MSG_RAM_BASE_OFFSET
03/19/12   bm      Fix smem_map_memory_va for returning correct virtual addr
                   for RPM message RAM.
01/20/12   bt      Integrated smem_map_memory_va, replacing smem_map_base_va.
02/21/12   bt      Add timetick DAL OS abstractions.
02/14/12   bt      Initial revision, based on best of smem_qube + smem_rex.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smem_v.h"
#include "smem_target.h"
#include "smem_os_common.h"
#include "smem_targ_info.h"

#include "DDIHWIO.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDITimetick.h"
#include "DDIChipInfo.h"
#include "err.h"

/*===========================================================================
                  LOCAL DATA DECLARATIONS
===========================================================================*/

/** Defines the HWIO base address used to get target info structure addr */
uint8 *smem_targ_info_addr_base_ptr = NULL;

/** DAL Timetick handle */
static DalDeviceHandle* smem_timetick_handle = NULL;

/** Address containing the SMEM memory info, e.g physical address and size, 
 *  and possibly other information. */
static smem_targ_info_type *smem_targ_info_ptr = 
    (smem_targ_info_type *)SMEM_INVALID_ADDR;

/*===========================================================================
                  EXTERNAL DATA DECLARATIONS
===========================================================================*/

/** Function pointers for internal spinlock implementation */
extern void (*spin_lock_internal_fn_ptr)(uint32);
extern void (*spin_unlock_internal_fn_ptr)(uint32);
extern uint32 (*spin_lock_get_fn_ptr)(uint32);

/*===========================================================================
                      PRIVATE FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  smem_map_hwio_region_va
===========================================================================*/
/**
  Creates a virtual address of the HWIO region.

  @param[in]  base_name     HWIO base name to map. i.e. "CORE_TOP_CSR".

  @return
  Virtual address if successfully mapped, or NULL
  otherwise.

  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
static void *smem_map_hwio_region_va(const char *base_name)
{
  DalDeviceHandle *hwio_handle;
  uint8           *base_va = NULL;
  
  if ((DAL_DeviceAttach(DALDEVICEID_HWIO, &hwio_handle) == DAL_SUCCESS)
        && (hwio_handle != NULL))
  {
    if ((DalHWIO_MapRegion(hwio_handle, SMEM_HW_MUTEX_BASE_NAME, 
                          &base_va) != DAL_SUCCESS) || (base_va == NULL))
    {
      ERR_FATAL("smem_map_hwio_region_va: Failed to map HW %s region.", 
                base_name, 0, 0);
    }
  }
  else 
  {
    ERR_FATAL("smem_map_hwio_region_va: Failed to attach HWIO DalDevice.", 
              0, 0, 0);
  }
  return base_va;
}


/*===========================================================================
                  PUBLIC FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smem_map_spinlock_region_va
===========================================================================*/
/**
  Creates a virtual address of the HW Mutexes region, used for SMEM spinlocks,
  to enable use of HWIO macros.

  @return
  The spinlocks region base virtual address if successfully mapped, or NULL
  otherwise.

  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
void *smem_map_spinlock_region_va( void )
{
  return smem_map_hwio_region_va(SMEM_HW_MUTEX_BASE_NAME);
}

/*===========================================================================
  FUNCTION  smem_timetick_init
===========================================================================*/
/**
  Creates the handle for the OS-specific timetick handle, if necessary.
 
  @return
  SMEM_ERROR on failure
  SMEM_SUCCESS on success
*/
/*=========================================================================*/
int32 smem_timetick_init( void )
{
  int8 ret;
  if ( (DalTimetick_Attach("SystemTimer", &smem_timetick_handle) != DAL_SUCCESS)
          || (smem_timetick_handle == NULL) )
  {
    ERR("smem_timetick: DAL timetick handle failed to attach.", 0, 0, 0);
    ret = SMEM_ERROR;
  }
  else
  {
    ret = SMEM_SUCCESS;
  }
  return ret;
}

/*===========================================================================
  FUNCTION  smem_timetick_get
===========================================================================*/
/**
  Returns the current timetick value.
 
  @return
  The current timetick value, measured in ticks.
  0 if an error occurred.
*/
/*=========================================================================*/
uint32 smem_timetick_get( void )
{
  DalTimetickTime32Type time_ret;
  if (DalTimetick_Get(smem_timetick_handle, &time_ret) != DAL_SUCCESS)
  {
    ERR("smem_timetick: Cannot get DAL timetick value.", 0, 0, 0);
    time_ret = 0;
  }
  return (uint32)time_ret;
}

/*===========================================================================
  FUNCTION  smem_set_spin_lock_api
===========================================================================*/
/**
  Lookup chip family and version to determine whether to use one of two 
  spinlock mechanisms:
  For 9x25/8974 below version 2 (and not WCNSS): 
    use exclusive memory access spinlock API
  Otherwise:
    use HW mutex spinlock API.
 
  @return
  None.
*/
/*=========================================================================*/
void smem_set_spin_lock_api( void )
{
  /* Use HW mutex for spinlock access */
  spin_lock_internal_fn_ptr   = spin_lock_internal;
  spin_unlock_internal_fn_ptr = spin_unlock_internal;
  spin_lock_get_fn_ptr        = spin_lock_get;
}

/*===========================================================================
  FUNCTION  smem_map_smem_targ_info
===========================================================================*/
/**
  Map the SMEM target info region (if it has not been mapped by this function 
  already) and return the virtual base address of the structure.

  If this is not the first time this function has been called, it will simply 
  return the saved pointer, rather than attempting to do the mapping again.

  @return
  Pointer to the structure that holds the SMEM target info like physical 
  address and size.
*/
/*=========================================================================*/
smem_targ_info_type *smem_map_smem_targ_info( void )
{
  if (smem_targ_info_ptr == SMEM_INVALID_ADDR)
  {
#if defined(SMEM_RPM_PROC) || defined(SMEM_BOOT_IMAGE) || defined(SMEM_TZ_IMAGE)
    /* Assume mapping is 1-to-1. */
    smem_targ_info_ptr = (smem_targ_info_type *)SMEM_TARG_INFO_ADDR;
#else
    /* Map TCSR WONCE HWIO BASE */
    smem_targ_info_addr_base_ptr = 
        smem_map_hwio_region_va(SMEM_TARG_INFO_ADDR_BASE_NAME);
    
    /* Check if TCSR WONCE registers hold proper SMEM target info address */
    if (smem_targ_info_addr_base_ptr == NULL || SMEM_TARG_INFO_ADDR == 0)
    {
      return NULL;
    }

    /* Map target info physical address to virtual address */
    smem_targ_info_ptr = smem_map_memory_va((void *)SMEM_TARG_INFO_ADDR, 
                                            sizeof(smem_targ_info_type));
#endif
  }

  return smem_targ_info_ptr;
}

