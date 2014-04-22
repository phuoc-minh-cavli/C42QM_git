/*=======================================================================
 * FILE:        nand_wrapper.h
 *
 * SERVICES:    None
 *
 * GENERAL DESCRIPTION
 *    Declarations for read/write API wrapper functions
 *    in flash driver.
 *
 * Copyright (c) 2010, 2019 Qualcomm Technologies, Inc. All rights reserved.
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/NandJtagTools/nand_wrapper.h#3 $ $DateTime: 2019/10/14 05:05:36 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 06/11/19     sl      Added support for multipage read.
 * 09/10/10     bb      Fix for unaligned memcpy failure with RVCT 4.0
 * 08/12/10     nr      Added FLASHLOG wrapper
 * 02/18/09     jz      Renamed nand_ to flash_prog_ for nand tool
 * 01/09/09     mm      Removed DAL Framework files for mibib.mbn
 *                      generation.
 * 11/26/08     bb      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#ifndef BUILD_FOR_X86
#include "DALFramework.h"
#include "flash.h"
#else
/*This needs to be after any files that include comdef.h*/
#include "nand_sim.h"
#warning "NAND SIM being compiled"
#endif

#ifdef FLASH_LOG_H
  #include FLASH_LOG_H
#else
  #define FLASHLOG(l, x)
#endif

/*
 * Wrapper for memcpy API. Introduced as a
 * Workaround for unaligned memcpy failure with RVCT 4.0
 */
void* nand_memcpy(void *out, const void *in, uint32 n);

/*
 * Wrapper function for flash read pages driver API
 */
int flash_prog_read_page(flash_handle_t handle, struct flash_info *dev_info,
  uint32 page, uint32 read_type, void *buffer);
/*
 * Wrapper function for flash read pages driver API
 */
int flash_prog_read_mpages(flash_handle_t handle, struct flash_info *dev_info,
  uint32 page, uint32 page_count, uint32 read_type, void *buffer);

/*
 * Wrapper function for flash write pages driver API
 */
int flash_prog_write_page(flash_handle_t handle, struct flash_info *dev_info,
  uint32 page, uint32 write_type, void *buffer);

/*
 * Wrapper function for flash write pages with udata
 */
int flash_prog_write_page_and_udata (flash_handle_t handle, struct flash_info *dev_info,
  uint32 page, uint32 write_type, void *buffer, void *spare_udata,
  uint32 spare_udata_blen);

/*
 * Wrapper function for flash read pages with udata
 */
int flash_prog_read_page_and_udata (flash_handle_t handle, struct flash_info *dev_info,
  uint32 page, uint32 read_type, void *buffer, void *spare_udata,
  uint32 spare_udata_blen);

/*
 * Wrapper function for flash read pages with udata
 */
int flash_prog_read_mpages_and_udata (flash_handle_t handle, struct flash_info *dev_info,
  uint32 page, uint32 page_count, uint32 read_type, void *buffer, void *spare_udata,
  uint32 spare_udata_blen);

