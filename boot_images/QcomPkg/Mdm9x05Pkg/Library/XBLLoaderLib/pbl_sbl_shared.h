/******************************************************************************
**+-------------------------------------------------------------------------+**
**|                                                                         |**
**|                          Qualcomm Technologies INCORPORATED                          |** 
**|                                                                         |**
**|                                                                         |**
**|                Copyright (c) 2014, 2015 Qualcomm Technologies, Inc      |**
**|                           ALL RIGHTS RESERVED                           |**
**|                                                                         |**
**|        No rights to reproduce, use, or disseminate this computer        |**
**|        program, whether in part or in whole, are granted.               |**
**|                                                                         |**
**+-------------------------------------------------------------------------+**
******************************************************************************/

#ifndef PBL_SBL_SHARED_H
#define PBL_SBL_SHARED_H
 /**===========================================================================
 **
 **                        PRIMARY BOOT LOADER
 **                        ------------------- 
 ** FILE 
 **     pbl_sbl_shared.h
 ** 
 ** GENERAL DESCRIPTION 
 **     This header file contains PBL-SBL shared data structure
 **     specific details.
 ** 
 **     Any new shared data that has to be passed to SBL should be
 **     added in this file. Any dependencies, like enums, defines etc ...
 **     that are supposed to be passed in this structure should be present
 **     in this file.
 ** 
 **     SBL code relies heavily on this structure and hence
 **     everytime there is a change in the shared data-structure
 **     (additions/deletions of fields/new values etc) ... structure
 **     version should be incremented appropriately.
 ** 
 **     Any new shared data field that has to be added has to be
 **     at the end of the structure.
 **
 **     NOTE:
 **     -----
 **
 **     ANY CHANGES INT THIS FILE OR ANY FIELDS RELATED TO THE 
 **     SHARED DATA STRUCTURE MUST BE REVIEWED WITH SBL IMAGE
 **     OWNERS.
 **
 **==========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module..
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/XBLLoaderLib/pbl_sbl_shared.h#3 $

  when       who         what, where, why
  --------   -------     ---------------------------------------------------
  12/13/17   rohik       Added pbl_cache_mmu_share_info_type to boot_pbl_shared_data_type
  10/29/15   plc         Updated for Sdm660 pbl shared data structure
  11/20/14   kpa         Added FORCE_DLOAD_MASK_V1 for dload cookie bit on chip version 1
  11/20/14   ck          Replaced PBL_TIMESTAMP_TIMER_FREQ_HZ with PS_PER_PBL_TIMESTAMP_TICK
  09/30/14   ck          Added boot_pbl_log_milestones
  09/30/14   ck          Added PBL_TIMESTAMP_TIMER_FREQ_HZ
  08/24/14   as          Initial version created
 
============================================================================*/


/******************************************************************************
                                MODULE INCLUDES
                         ADD NEW ONES UNDER THIS LINE
******************************************************************************/
#include "sec_img_auth.h"
#include "secboot_hw.h"
#include "XBLRamDump/boot_sahara.h"


/******************************************************************************
                             PUBLIC MACROS/DEFINES
                         ADD NEW ONES UNDER THIS LINE  
******************************************************************************/
/* Shared data structure Version */
/* Version Format: 0xMMMMNNNN
          MMMM : Major Version 
          NNNN : Minor Version */
#define PBL_APPS_SHARED_DATA_REVISION       0x00010000
#define BL_RESERVED_LENGTH                  32  

/* Definitons for PBL to enter HS-USB or SD download mode. */
#define FORCE_DLOAD_BOOT_NONE       0x0
#define FORCE_DLOAD_BOOT_USB        0x1
#define FORCE_DLOAD_BOOT_SD_USB     0x2
#define FORCE_DLOAD_BOOT_SD         0x3

/* MASK for reading force download boot option. */
#define FORCE_DLOAD_MASK 0xF
#define FORCE_DLOAD_MASK_V1 0x000F0000

/******************************************************************************
                         PUBLIC TYPES and TYPE-DEFINES
                         ADD NEW ONES UNDER THIS LINE  
******************************************************************************/
#ifndef _ARM_ASM_

typedef struct pbl_secboot_shared_info_type
{
  secboot_verified_info_type pbl_verified_info;    /**<  */
  secboot_ftbl_type          pbl_secboot_ftbl;     /**< Contains pointers to PBL secboot routines */
  secboot_hw_ftbl_type       pbl_secboot_hw_ftbl;  /**< Contains pointers to PBL secboot hw routines */
  sec_img_auth_ftbl_type     pbl_sec_img_auth_ftbl;/**< Contains pointers to PBL sec_img_auth routines */
} pbl_secboot_shared_info_type;

typedef struct
{
  uint32 shared_data_version;
  uint32 pbl_entry_timestamp;
  uint32 bootable_media_detect_entry_timestamp;
  uint32 bootable_media_detect_success_timestamp;
  uint32 elf_loader_entry_timestamp;
  uint32 auth_hash_seg_entry_timestamp;
  uint32 auth_hash_seg_exit_timestamp;
  uint32 elf_segs_Hash_verify_entry_timestamp;
  uint32 elf_segs_Hash_verify_exit_timestamp;
  uint32 auth_xbl_sec_hash_seg_entry_timestamp;
  uint32 auth_xbl_sec_hash_seg_exit_timestamp;
  uint32 xbl_sec_segs_Hash_verify_entry_timestamp;
  uint32 xbl_sec_segs_Hash_verify_exit_timestamp;
  uint32 pbl_exit_timestamp;
} pbl_timestamp_type;

/*
 * pbl_clock_sdcc_freq_type
 *
 */
typedef enum
{
  PBL_CLOCK_SDCC_FREQ_400KHZ,
  PBL_CLOCK_SDCC_FREQ_19P2MHZ,
  PBL_CLOCK_SDCC_FREQ_25MHZ,
  
  PBL_CLOCK_SDCC_FREQ_TOTAL,
  PBL_CLOCK_SDCC_FREQ_FORCE32BITS = 0x7FFFFFFF
} pbl_clock_sdcc_freq_type;

/* Define Force Download Device type.
 
   If this field is anything other than FORCE_DLOAD_NONE
   then it indicates that boot option was ignored and
   corresponding force download option was taken to boot
   into SBL. */

typedef enum
{
  FORCE_DLOAD_NONE  = 0x0,
  FORCE_DLOAD_HS_USB_GPIO,
  FORCE_DLOAD_HS_USB_MAGIC_NUMBER,
  FORCE_DLOAD_SD_USB,
  FORCE_DLOAD_SD,
  FORCE_DLOAD_FORCE32BITS = 0x7FFFFFFF
} force_dload_type;

/* Define the FLASH types.
 
   Based on the boot option that is configured, this field
   will indicate the exact boot media that has been used to
   successfully load the SBL image.
 
   If control gets transferred to SBL then the field will
   not have NO_FLASH*/
typedef enum
{
  NO_FLASH        = 0,
  NOR_FLASH       = 1,
  NAND_FLASH      = 2,
  ONENAND_FLASH   = 3,
  SDC_FLASH       = 4,
  MMC_FLASH       = 5,
  SPI_FLASH       = 6,
  PCIE_FLASH      = 7,
  RESERVED_0_FLASH= 8,
  RESERVED_1_FLASH= 9,
  USB_FLASH       =10,           /* this is not a real flash type, it's used only for sw_type */
  SPI_NAND_FLASH  =11,
  SIZE_FLASH_TYPE = 0x7FFFFFFF    /* Force 4 byte alignment */
} boot_flash_type;

/* Define the types of flash controllers that may be in use */
typedef enum
{
  NO_CTRL,                          /* Invalid controller     */
  NOR_CTRL,                         /* NOR controller         */
  NAND_CTRL,                        /* NAND controller        */
  SFLASH_CTRL,                      /* Sync flash controller          */
  SDC_CTRL,                         /* Secure Digital Card controller */
  SPI_CTRL,                         /* SPI controller         */
  UFS_CTRL,                         /* UFS controller         */
  USB_CTRL,                         /* USB controller         */
  SIZE_FLASH_CTRL_TYPE = 0x7FFFFFFF /* Force 4 byte alignment */
} boot_flash_ctrl_type;

/* Define the types of ECC Mode being used by NAND Controller */
typedef enum
{
  ECC_4_BIT_MODE         = 0,
  ECC_6_BIT_MODE,
  ECC_8_BIT_MODE,
  ECC_RESERVED_BIT_MODE_0,
  ECC_16_BIT_MODE,
  ECC_RESERVED_BIT_MODE_1,
  ECC_NOT_ENABLED,
  ECC_BIT_MODE_ENUM_COUNT,
  EBM_SIZE_ENUM          = 0x7FFFFFFF          /* Size enum to 32 bits. */
} boot_ecc_bit_mode_enum_type;

/* Defines the types of Boot Option is configured on the
   device. */
typedef enum
{
  BOOT_DEFAULT_OPTION = 0,  							/* SPI NAND */
  BOOT_SPI_NAND_OPTION = BOOT_DEFAULT_OPTION,
  BOOT_USB_OPTION,              						/* USB */
  BOOT_NAND_OPTION,                				        /* NAND>eDL */
  BOOT_RSVD_1_OPTION,              						/* Reserved option */
  BOOT_SELECT_OPTION_COUNT,
  BOOT_OPTION_SIZE_ENUM          = 0x7FFFFFFF          	/* Size enum to 32 bits. */
} boot_select_option_type;

/* SDC Flash shared data */
/* This defines the exact SDC device type that is detected */
typedef enum
{
  SDC_CARD_UNKNOWN = 0,
  SDC_CARD_SD      = 1,
  SDC_CARD_MMC     = 2,
  SDC_CARD_SDHC    = 5,
  SDC_CARD_MMCHC   = 6,  /* MMC card with higher than a density of 2GB */
  SDC_CARD_TYPE_SIZE = 0x7FFFFFFF
} boot_flash_sdc_card_type;

/* This defines SDC card states */
typedef enum
{ 
  SDC_CARD_IDLE = 0,
  SDC_CARD_READY,
  SDC_CARD_IDENT,
  SDC_CARD_STBY,
  SDC_CARD_TRAN,
  SDC_CARD_DATA,
  SDC_CARD_RCV,
  SDC_CARD_PRG,
  SDC_CARD_DIS,
  SDC_CARD_BTST,
  SDC_CARD_IGNORE = 0xFF,         /*dummy entry*/
  SDC_CARD_STATE_SIZE = 0x7FFFFFFF
} boot_flash_sdc_card_state_type;

/* This defines the SDC device memory access modes */
typedef enum
{
  SDC_MEM_BYTE_MODE,
  SDC_MEM_BLK_MODE,
  SDC_MEM_MODE_SIZE = 0x7FFFFFFF
} boot_flash_sdc_mem_mode_type;

/* This defines the MMC bus width */
typedef enum
{
  SDC_MMC_BUSWIDTH_1BIT = 0,
  SDC_MMC_BUSWIDTH_4BIT = 1,
  SDC_MMC_BUSWIDTH_8BIT = 2,
  SDC_MMC_BUSWIDTH_COUNT,
  SDC_MMC_BUSWIDTH_SIZE = 0x7FFFFFFF
} pbl_sdc_mmc_buswidth_type;

/* This defines the SDC Port used to boot */
/* !!!Warning!!! order of this port numbers should match with
   sdcc_reg[] array defined in sdcc driver files. sdcc_reg array
   defines base address for particular PORT and if below order
   doesn't match, you could see issues !!! */
typedef enum
{
  SDC_PORT1,
  SDC_PORT2,
  SDC_PORT_INVALID,
  SDC_PORT_SIZE = 0x7FFFFFFF
} boot_flash_sdc_port_type;

/* Structure to define ID to enter download mode in PBL. */ 
typedef struct
{ 
uint32       shared_data_version;
uint32    *page_tbl_base;/* Page table base address */
uint32       page_tbl_size;  /* Page table size */
}pbl_cache_mmu_share_info_type;


typedef struct
{   
uint32        shared_data_version;
void*         elf_img_meta_info;
uint32        num_of_bytes_read_from_elf;  /* Total number of bytes ready by PBL from ELF (BL + BL-sec)*/ 
}pbl_elf_img_share_info_type;

/*
 * Clock Shared Data to facilitate PBL-SBL messaging.
 */
typedef struct {
  uint32                       nVersionInfo;
  uint32                       nProcSpeedMHz;
} pbl_clock_shared_data_type;


/* Defines SDC memory information */
typedef struct
{
   uint32                        block_len;       /* Block size (aka the
                                                     sector size, in bytes) */
   uint32                        block_len_shft;  /* Block length shift bits*/
   boot_flash_sdc_mem_mode_type  block_mode;      /* transfer blocks or bytes */
   uint32                        spec_vers;       /* For MMC only: MMC System
                                                     Spec version */
   uint32                        phy_parti_size;  /* number of blocks */
} boot_flash_sdc_mem_type;




typedef enum
{
  SPI_PORT,
  SPI_PORT_INVALID,
  SPI_PORT_SIZE = 0x7FFFFFFF
} boot_flash_spi_port_type;


/* This is the main structure that contains
   NAND Flash specific shared data */
typedef struct
{
  uint32                      page_size_bytes;
  uint32                      page_size_shift;
  uint32                      block_size_pages;
  uint32                      block_size_shift;
  boot_ecc_bit_mode_enum_type ecc_mode;
} boot_flash_shared_nand_dev_type;

/* This is the main structure that contains
   SDC Flash specific shared data */
typedef struct
{
   boot_flash_sdc_mem_type         mem;         /* info pertaining to memory device */
   boot_flash_sdc_card_type        card_type;   /* card type  */
   boot_flash_sdc_card_state_type  card_state;  /* card state */
   uint32                          rca;         /* the card's relative address */
   uint32                          mclk;        /* current host clock speed */
   boot_flash_sdc_port_type        port;        /* Port/Slot the device was detected */
   uint32                          boot_cfg;    /* Specify boot parition */
   pbl_sdc_mmc_buswidth_type       data_width;  /* data_width ( 1/4/8 bit) */ 
   boolean                         is_gpt_parti;
   boolean                         is_primary_table;
   uint64                          starting_lba;/* starting LBA of BL partition */
   uint64                          ending_lba;  /* Last LBA of SBL partition */
} boot_flash_shared_sdc_dev_type;

typedef struct
{
  uint32                          chip_select;
  uint32                          addr_len;
  boot_flash_spi_port_type        port;        /* BLSP-QUP SPI Port where device was detected */
} boot_flash_shared_spi_dev_type;


/* This defines Flash shared data */
typedef struct
{
  uint32                shared_data_version;
  boot_flash_type       type;        /* Type of flash                      */
  uint32                CS_base;     /* Base address of chip select device
                                        is connected to.                   */
  uint32                data_width;  /* Bit width of device ie: 8bit/16bit */
  boot_flash_ctrl_type  ctrl_type;   /* Controller used to interface with
                                        device                             */
  union
  {
    boot_flash_shared_nand_dev_type nand;  /* Shared info for NAND device  */
    boot_flash_shared_sdc_dev_type  sdc;   /* Shared info for SDC device   */
    boot_flash_shared_spi_dev_type  spi;   /* Shared info for SPI device   */
    struct boot_sahara_shared_data* sahara;/* Shared info pointer for Sahara flashless boot/eDL */
  }dev_info;
}boot_flash_shared_dev_info_type;


/* Defines the main data structure that is shared
   with the APPs SBL image. */
typedef struct boot_apps_shared_data_type
{
    /* Shared data structure version */
  uint32                          bl_sdata_version;

  /* PBL version */
  uint32                          pbl_version;

    /* PBL Patch version */
  uint32                          pbl_patch_version;

  /* PBL Boot option configured in the fuse */
  boot_select_option_type         boot_option;

  /* BL loader authentication state */
  uint32                          bl_loader_auth_enabled;

  /* BL loader hash integrity check state */
  uint32                          hash_integrity_check_disabled; 

  
  /* BL-Sec boot flow support state */
  uint32                          bl_sec_boot_flow_supported;

  /* BL-Sec authentication state */
  uint32                          bl_sec_auth_disabled;  

  /* PBL stack base */
  uint8                           *boot_stack_base;

  /* PBL stack size */
  uint32                          boot_stack_size;
  
  /*Reserved fields*/
  uint32                          reserved_fields_bl[BL_RESERVED_LENGTH];    

   /*Timestamps at main milestones in PBL*/
  pbl_timestamp_type *        timestamps;
     
  /* PBL shared data structure, maps to: pbl_sdata_type */
  void*                           pbl_shared_data;

  /* Pointer to clock shared data structure, maps to: pbl_clock_shared_data_type */
  void*                           pbl_clock_shared_data;

  /* Pointer to elf image shared data structure, maps to: pbl_elf_img_share_info_type */
  void*                           pbl_elf_shared_data;

  /* Pointer to Secboot shared data structure, maps to: secboot_pbl_shared_info_type */
  void*                           pbl_secboot_shared_data;

  /* Pointer to PCIE shared shared data structure, maps to: pcie_bhi_shared_data_type*/
  void*                           pbl_pcie_shared_data;
    
  /* Pointer to flash shared data structure, maps to: boot_flash_shared_dev_info_type */
  void*                           pbl_flash_shared_data;
  
  /* Pointer to cache mmu shared data structure,maps to: pbl_cache_mmu_share_info_type */
  void*                           pbl_cache_mmu_share_data;

  /* Pointer to cipher Hash shared data structure,maps to: crypto_hash_cipher_shared_info_type */
  void*                           pbl_crypto_hash_cipher_shared_data;

    /*Reserved fields*/
  uint32                          reserved_fields[BL_RESERVED_LENGTH];
} boot_pbl_shared_data_type;


/******************************************************************************
                            PUBLIC DATA DECLARATION
                         ADD NEW ONES UNDER THIS LINE  
******************************************************************************/
/* Timestamps from PBL are given to SBL as ticks of a timer.  On some
   processors floating point might not be available so calculate
   PS_PER_PBL_TIMESTAMP_TICK (Pico seconds).  The value is not currently passed
   to SBL so it must be hardcoded here. */
#define PS_PER_PBL_TIMESTAMP_TICK   52083  /* 19.2Mhz */


/******************************************************************************
                         PUBLIC FUNCTION DECLARATIONS
                         ADD NEW ONES UNDER THIS LINE  
******************************************************************************/
/*=========================================================================

**  Function :  boot_pbl_get_pbl_version

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the PBL version from PBL shared data.
* 
* @par Dependencies
*   None
*   
* @retval
*   uint32
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_pbl_get_pbl_version
(
  void
);

/*=========================================================================

**  Function :  boot_pbl_get_clock_speed

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the clock speed set by PBL.
* 
* @par Dependencies
*   None
*   
* @retval
*   uint32
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_pbl_get_clock_speed
(
  void
);


/*===========================================================================

**  Function :  boot_pbl_get_flash_shared_data

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the pointer to the flash related information found
*   by PBL.
* 
* @par Dependencies
*   None
* 
* @retval
*   Pointer of boot_flash_shared_dev_info_type type
* 
* @par Side Effects
*   None.
* 
*/
boot_flash_shared_dev_info_type* boot_pbl_get_flash_shared_data
(
  void
);


/*===========================================================================

**  Function :  boot_pbl_is_auth_enabled

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns whether PBL detected that auth was enabled.
* 
* @par Dependencies
*   None
* 
* @retval
*   boot_boolean
* 
* @par Side Effects
*   None.
* 
*/
boot_boolean boot_pbl_is_auth_enabled
(
  void
);


/*===========================================================================

**  Function :  boot_pbl_get_flash_type

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the flash type detected by PBL
* 
* @par Dependencies
*   None
* 
* @retval
*   boot_flash_type
* 
* @par Side Effects
*   None.
* 
*/
boot_flash_type boot_pbl_get_flash_type
(
  void
);


/*===========================================================================

**  Function :  boot_pbl_interface_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Function that will initialize the target interface so that generic SBL
*   code can use it.
* 
* @par Dependencies
*   None
* 
* @retval
*   boot_boolean
* 
* @par Side Effects
*   None
* 
*/
boot_boolean boot_pbl_interface_init
(
  boot_pbl_shared_data_type   *pbl_shared_data  /* Pointer to data coming from PBL */
);


/*===========================================================================

**  Function :  boot_pbl_log_milestones

** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will parse the PBL timestamp milestones passed to SBL
*   and insert them into the boot log.  Currently PBL's unit of measure is
*   clock ticks.  PBL does not pass the clock frequency yet so it is hard
*   wired to 19.2 Mhz.  Also PBL does not pass the names of each field so for
*   now reference a structure of our own to get the names which will have
*   logic ready for the day PBL starts passing them.
*
* @param[in]
*   boot_pbl_shared_data_type
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_pbl_log_milestones(boot_pbl_shared_data_type * pbl_shared_data);


#endif /* _ARM_ASM_ */

#endif /* PBL_SBL_SHARED_H */
/*=============================================================================
                                  END OF FILE
=============================================================================*/

