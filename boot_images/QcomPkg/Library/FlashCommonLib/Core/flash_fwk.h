#ifndef FLASH_DAL_FWK_H
#define FLASH_DAL_FWK_H
/*=============================================================================
 *
 * FILE:      flash_dal_fwk.h
 *
 * DESCRIPTION: Function declarations specific to Flash DAL framework
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 *        Copyright © 2008-2009 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *            QUALCOMM Proprietary/GTDR
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/FlashCommonLib/Core/flash_fwk.h#1 $ $DateTime: 2018/08/01 23:51:05 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 08/28/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "DALStdDef.h"
#include "DALSys.h"
#include "nand_api.h"
#include "flash_miparti.h"

#ifdef FLASH_LOG_H
  #include FLASH_LOG_H
#else
  #define FLASHLOG(l, x)
#endif

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */


/** Definitions for Error-Correcting Code (ECC) disabling/enabling. ECC can be enabled in two modes.
 * FLASH_ECC_MAIN_ENABLED - In this mode ECC is enabled only on the Main Area
 * FLASH_ECC_MAIN_SPARE_ENABLED - In this mode, ECC is enabled on the
 * Main and the spare area
 */
enum nand_ecc_state
{
  NAND_ECC_NONE,               /**< No ECC Support */
  NAND_ECC_DISABLED,           /**< ECC Disabled */
  NAND_ECC_MAIN_ENABLED,       /**< ECC Enabled only on Main Area */
  NAND_ECC_MAIN_SPARE_ENABLED, /**< ECC Enabled on Main and Spare Area */
  NAND_ECC_UNKNOWN = 0x7FFFFFFF /* Force 32-bit enum */
};

/* Declaring a "Flash" Device and Client Context */
typedef struct flash_dev_ctxt     flash_dev_ctxt;
typedef struct flash_client_ctxt  flash_client_ctxt;

/* Flash Device Info */
struct flash_dev_info
{
  const char *device_name;			  /* Device string */
  uint32 maker_id;                    /* Maker Identification */
  uint32 device_id;                   /* Device Identification */
  uint32 version_id;                  /* Version Identification */
  uint32 block_count;                 /* Number of blocks in actual device */
  uint32 pages_per_block;             /* Number of pages in block */
  uint32 page_size_bytes;             /* Number of bytes not including spare */
  uint32 total_page_size_bytes;       /* Number of bytes including spare */
  uint32 flags1;                      /* Extra device specific data/instance */
  uint32 flags2;                      /* Extra device specific data/instance */
  uint32 reserved1;                   /* Reserved for future use */
  uint32 reserved2;                   /* Reserved for future use */
  void *dev_specific_data;			  /* Runtime malloc'd device shared data */
};

/* Flash Device Partition Table Info */
struct flash_parti_entry
{
   char  name_lc[FLASH_PART_NAME_LENGTH]; /* partition name in lower case */
};
   
/*------------------------------
 * Flash Device Context
 *----------------------------*/
struct flash_dev_ctxt
{
  DALSYS_SYNC_OBJECT(dev_sync_object); /* Synchronizaton object */
  DALSYSSyncHandle handle_dev_sync;               /* Handle to sync object */
  struct flash_dev_info      dev_info;            /* Device info */
  struct nand_mibib          mibib_info;          /* MIBIB information */
  struct flash_parti_entry   parti_entry[FLASH_NUM_PART_ENTRIES]; /* List of partition table entries*/
  void                      *parti_ptr;           /* Partition table */
};

/*---------------------------------------------------------------------------
Structure to encapsulate the CS Interface object. The first member is the
falsh_handle_id followed by vtable pointer (CS requirement).
The last (third) member is a pointer to the Client  Context
----------------------------------------------------------------------------*/
typedef struct  flash_handle  flash_handle;
struct  flash_handle
{
  flash_client_ctxt        *flash_client_ctxt;
};

/*------------------------------
 * Flash Client Data
 *----------------------------*/
struct flash_client_data
{
  uint32 hal_id;                      /* flash hal id */
  uint32 which_cs;                    /* Chipselect */
  uint32 block_count;                 /* Number of blocks allocated for client */
  uint32 pages_per_block;             /* Number of pages in block */
  uint32 page_size_bytes;             /* Number of bytes not including spare */
  uint32 total_page_size_bytes;       /* Number of bytes including spare */
  uint32 base_address;                /* Base address of device for client */
  uint32 max_spare_udata_bytes;       /* Max spare user data in bytes */
  uint32 block_size_shift;            /* Block <-> page conversion shift */
  uint32 page_size_shift;             /* log to base 2 of page size in bytes */
  enum nand_ecc_state ecc_state;	  /* ECC state of client */

  /*------------------------------
   * Partition specific data
   *----------------------------*/
  char  partition_name[FLASH_PART_NAME_LENGTH]; /* partition name string */
  uint32 partition_attributes;        /* Partition attributes */
  uint32 partition_block_start;       /* Partition physical start block*/
  uint32 partition_block_limit;       /* Partition physical end address */
  uint32 partition_block_count;       /* Partition length in blocks */
  uint32 partition_block_size_bytes;  /* Block size in bytes */
  uint32 partition_last_page;         /* Last page ID in NOR partition */
  void *client_specific_data;         /* Additional client specific private data */
};

/*---------------------------------------------------------------------------
Declaring a "Flash" Client Context
----------------------------------------------------------------------------*/
struct flash_client_ctxt
{
  //Base Members
  uint32  refs;                          /* Reference count */
  flash_dev_ctxt *flash_dev_ctxt;        /* Device context */

  /* Flash Client state can be added by developers here */
  struct flash_client_data client_data;  /* Client specific data */
};

typedef struct flash_driver_ctxt
{
  flash_client_ctxt    flash_handle[FLASH_NUM_PART_ENTRIES]; /* for flash open/read/write APIs */
  uint8                nand_initialized; /* nand driver init flag */
  uint8                clnt_cnt;         /* number of clients for this partition */
} flash_driver_ctxt_t;

int flash_fwk_attach(const char *pszArg, DALDEVICEID dev_id, 
  DalDeviceHandle **handle_dal_device);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef FLASH_DAL_FWK_H */

