#ifndef TZBSP_SYSCALL_PUB_H
#define TZBSP_SYSCALL_PUB_H

/**
   @file tzbsp_syscall_pub.h
   @brief Provide the SYSCALL API infrastructure
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.

      The xxx_mainpage.dox file contains all descriptions that are displayed
      in the output PDF generated using Doxygen and LaTeX. To edit or update
      any of the file/group text in the PDF, edit the xxx_mainpage.dox
      file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
   Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.
   All rights reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.tx/6.0/api/products/scm/syscalls.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
05/29/12   rkk      Added syscall for lock/map AMT chunks, page table init.
05/18/12   sg       Added syscall to restore sec cfg for a device
03/08/12   leo      (Tech Pubs) Edited/added doxygen comments and markup.
09/12/11   kpa      Added services to support Version rollback prevention
08/20/10   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

 /** Service IDs */

/** Boot (cold boot/warm boot). */
#define TZBSP_SVC_BOOT               1
#define SCM_SVC_BOOT                 1

/** Fuse services. */
#define TZBSP_SVC_FUSE               8
#define SCM_SVC_FUSE                 8

/** secure IO  */
#define TZBSP_SVC_IO                 5
#define SCM_SVC_IO                   5

/** Informational service */
#define TZBSP_SVC_INFO               6
#define SCM_SVC_INFO                 6

/** Memory protection service. */
#define TZ_SVC_MEMORY_PROTECTION     12
#define SCM_SVC_MEMORY_PROTECTION    12

/** TZ scheduler service */
#define SCM_SVC_TZSCHEDULER         0xFC

 /** Command IDs */
 
/* Read and write commands for IO call */
#define TZBSP_IO_READ   0x1
#define SCM_IO_READ     0x1
#define TZBSP_IO_WRITE  0x2
#define SCM_IO_WRITE    0x2

/* For setting cold/warm boot address. Used with service SCM_SVC_BOOT */
#define TZBSP_BOOT_ADDR 0x1
#define SCM_BOOT_ADDR   0x1

/* For setting cold/warm boot address. Used with service SCM_SVC_BOOT */
#define TZBSP_BOOT_ADDR_MC 0x11
#define SCM_BOOT_ADDR_MC   0x11

/* Command to power collapse core */
#define TZBSP_CMD_TERMINATE_PC  0x2
#define SCM_CMD_TERMINATE_PC    0x2

/**
   @ingroup write_qfprom_fuse

   Writes a Qualcomm Field Programmable Read Only Memory (QFPROM) fuse row.

   @command_id
     0x00002003

   @sys_call_params{tzbsp_qfprom_write_row_s}
     @table{weak__tzbsp__qfprom__write__row__s}

   @com_struct
      tzbsp_syscall_rsp_s

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZBSP_QFPROM_WRITE_ROW_ID                 0x03
#define SCM_QFPROM_WRITE_ROW_ID                   0x03


/**
   @ingroup write_mult_qfprom_rows

   Writes multiple rows of QFPROM fuses.

   @command_id
     0x00002004

   @return
     E_SUCCESS on success; negative value on failure.
 */
#define TZBSP_QFPROM_WRITE_MULTIPLE_ROWS_ID       0x04
#define SCM_QFPROM_WRITE_MULTIPLE_ROWS_ID         0x04


/**
   @ingroup read_qfprom_fuse

   Reads QFPROM fuses.

   @command_id
     0x00002005

   @sys_call_params{tzbsp_qfprom_read_row_s}
     @table{weak__tzbsp__qfprom__read__row__s}

   @com_struct
     tzbsp_syscall_rsp_s

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZBSP_QFPROM_READ_ROW_ID                  0x05 
#define SCM_QFPROM_READ_ROW_ID                    0x05 

/**
   @ingroup config_hw_for_offline_ram_dump

   Subroutine used by HLOS to configure HW registers 
   (BOOT_PARTITION_SELECT / WDOG_DEBUG_EN) for offline RAM dump. 

   @command_id
     0x00000409

   @subhead{System call parameters} 
     @table{group__weak__tzbsp__config__hw__for__ram__dump__req__s)

   @com_struct
     tzbsp_syscall_rsp_s

   @return
     E_SUCCESS on success; negative value on failure.
*/
#define TZBSP_CONFIG_HW_FOR_RAM_DUMP_ID         0x09
#define SCM_CONFIG_HW_FOR_RAM_DUMP_ID           0x09
/**
@ingroup force_dload_mode
 
   Subroutine used by the HLOS to configure hardware register
   TCSR_BOOT_MISC_DETECT to force different dload modes.
 
   @command_id
     0x00000410
 
   @sys_call_params{tz_force_dload_mode_req_s}
     @table{weak__tz__force__dload__mode__req__s}
 
   @com_struct
     tz_syscall_rsp_s
 
   @return
     Zero on success; an error code if parameters are invalid or not permitted
*/
#define TZBSP_CONFIG_FORCE_DLOAD_ID          0x10
#define SCM_CONFIG_FORCE_DLOAD_ID            0x10

/**
   @ingroup restore_sec_cfg

   Restores VMIDMT/xPU configuration of a device's System Memory
   Management Unit  global space.

   @note1hang clocks must be ON.

   @command_id
     0x00003002

   @sys_call_params{tz_restore_sec_cfg_req_s}
     @table{weak__tz__restore__sec__cfg__req__s}

   @com_struct
     tz_syscall_rsp_s

   @return
     E_SUCCESS on success; an error code otherwise.
 */
#define TZ_RESTORE_SEC_CFG_CMD_ID                 0x02
#define SCM_RESTORE_SEC_CFG_CMD_ID                0x02

/**
   @weakgroup weak_tzbsp_config_hw_for_ram_dump_req_s
@{
*/
typedef PACKED struct tz_config_hw_for_ram_dump_req_s
{
  uint32 disable_wd_dbg;           /**< If \c TRUE, watchdog debug image
                                        is disabled (i.e. won't run after
                                        the watchdog reset)  */

  uint32 boot_partition_sel;       /**< Value programmed into
                                        BOOT_PARTION_SELECT register */
} tz_config_hw_for_ram_dump_req_t;

/** @} */ /* end_weakgroup */

typedef enum
{
  TZ_DEVICE_VIDEO        =  0,      /**< Video subsystem. */
  TZ_DEVICE_MDSS         =  1,      /**< MDSS subsystem. */
  TZ_DEVICE_LPASS        =  2,      /**< Low-power audio subsystem. */
  TZ_DEVICE_MDSS_BOOT    =  3,      /**< MDSS subsystem at cold boot. */
  TZ_DEVICE_USB1_HS      =  4,      /**< High speed USB. */
  TZ_DEVICE_OCMEM        =  5,      /**< OCMEM registers. @newpage */
  TZ_DEVICE_LPASS_CORE   =  6,
  TZ_DEVICE_VPU          =  7,
  TZ_DEVICE_COPSS_SMMU   =  8,
  TZ_DEVICE_USB3_0       =  9,
  TZ_DEVICE_USB3_1       = 10,
  TZ_DEVICE_PCIE_0       = 11,
  TZ_DEVICE_PCIE_1       = 12,
  TZ_DEVICE_BCSS         = 13,
  TZ_DEVICE_VCAP         = 14,
  TZ_DEVICE_PCIE_20      = 15,
  TZ_DEVICE_IPA          = 16,
  TZ_DEVICE_COUNT,
  TZ_DEVICE_MAX = 0x7FFFFFFF,
}scm_device_e_type;


typedef struct sec_dload_mode_buffer_s
 {
   uint32 dload_mode_req;
   uint32 spare;
 }sec_dload_mode_buffer_t;

#endif

