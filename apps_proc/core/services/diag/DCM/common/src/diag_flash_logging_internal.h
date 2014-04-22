#ifndef DIAG_FLASH_LOGGING_INTERNAL_H
#define DIAG_FLASH_LOGGING_INTERNAL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics Flash Logging Internal Header File 

General Description
  Global data declarations for diagnostics flash logging.

Copyright (c) 2022 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#if defined(DIAG_FLASH_LOGGING_SUPPORT)

#include "diag_flash_logging.h"
#include "diag_fs.h"
#include "dsm_pool.h"
#include "dsm_item.h"
#include "diagi_v.h"


/* -------------------------------------------------------------------------
** Macros
** ------------------------------------------------------------------------- */

/*! @brief
    Path in efs where the .cfg file will be saved.
    We define this macro in diag only if it is not
    defined at top level.   
*/
#ifndef DIAG_FLASH_LOGGING_CFG
  #define DIAG_FLASH_LOGGING_CFG "/diag.cfg"
#endif


/*! @brief
    Path in efs where the diag files will be saved.
    We define this macro in diag only if it is not
    defined at top level. The diag files would contain
    the diag packets (logs/f3s/events).

    Diag packets will be written in a ping pong manner 
    in these two files. Once diag_log1 file gets filled 
    to its capacity, we start writing in the diag_log2 file.
    Once diag_log2 file gets filled, the file diag_log1
    gets cleared and the incoming packets get written to 
    diag_log1. After this diag_log2 gets cleared and
    filled. Always, the oldest packets gets cleared when
    both the files are filled.

    Size of each file is defined by the macro 
    DIAG_FLASH_LOG_FILE_SIZE. Partial packets are never 
    written, a packet is written to the files only if there 
    is enough space for the entire packet to be written.
*/
#ifndef DIAG_FLASH_FILE1
  #define DIAG_FLASH_FILE1 "/diag_log1.qmdl2"
#endif

#ifndef DIAG_FLASH_FILE2
  #define DIAG_FLASH_FILE2 "/diag_log2.qmdl2"
#endif


/*! @brief 
    Max size of a mask command from .cfg file. 
    Clients should enable minimal logging to 
    ensure that size of a mask command does 
    not exceed 2048 bytes.    
*/
#define DIAG_CFG_MAX_CMD_SIZE   2048

/*! @brief
   Size of the flash files which will store
   diag packets.

*/
#ifndef DIAG_FLASH_LOG_FILE_SIZE
  #define DIAG_FLASH_LOG_FILE_SIZE	1024*32
#endif	

/*! @brief
    Header length of .qmdl2 v1.
*/
#define DIAG_QMDL2_V1_HDR_LEN    10

/* -------------------------------------------------------------------------
** Type definitions
** ------------------------------------------------------------------------- */

/*! @brief
    These enum values are returned by diag_process_cfg_file function
*/
typedef enum{

  DIAG_PROCESS_CFG_SUCCESS = 0,
  DIAG_PROCESS_CFG_BUFFER_OVERFLOW_FAILURE = 1,
  DIAG_PROCESS_CFG_MALLOC_FAILURE = 2,
  DIAG_PROCESS_CFG_READ_FAILURE = 3
}diag_process_cfg_enum;


/*! @brief
    This is the header structure for .qmdl2 version 1.
    Please note that since we do not have rfs APIs to download
    the qsr hash db, the guild_entry_count should be set to 0. 
*/
typedef struct{
    uint32 hdr_length;
    uint8  version;
    uint8  hdlc_data_type;
    uint32 guid_entry_count;
}diag_qmdl2_hdr;


/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------
** Function prototypes
** ------------------------------------------------------------------------ */

/*===========================================================================

FUNCTION DIAG_ADJUST_CFG_BUFFER_PTR

DESCRIPTION
  This function adjusts the pointers of the source and destination
  buffer to read the next command from .cfg.

PARAMETERS
  @param hdlc
  Pointer to diag_hdlc_decode_type structure which contains information
  of source and destination buffers to read the commands from .cfg

  @param fs
  Pointer to diag_fs_type structure which contains information of .cfg

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void diag_adjust_cfg_buffer_ptr (diag_hdlc_decode_type * hdlc, diag_fs_type * fs);

/*===========================================================================

FUNCTION DIAG_READ_CMD_FRM_CFG

DESCRIPTION
  This function reads a packet from an HDLC encoded .cfg file, 
  and verifies whether CRC is correct.

PARAMETERS
  @param hdlc
  Pointer containing information of source and destination buffer
  for copying the hdlc encoded commands from .cfg

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if a valid packet was found.

===========================================================================*/

boolean diag_read_cmd_frm_cfg (diag_hdlc_decode_type * hdlc);

/*===========================================================================

FUNCTION DIAG_PROCESS_CFG_FILE

DESCRIPTION
  This function processes the commands from the .cfg file
  which is stored in the EFS path as mentioned in macro 
  DIAG_FLASH_LOGGING_CFG_PATH.

  This function is called by the API diag_enable_flash_logging

PARAMETERS
  @param fs
  The internal fs type structure for diag_fs_config which stores
  the .cfg file

  @param buf
  Buffer in which the command will be copied to after reading 
  from .cfg file

  @param size
  Size of the buffer

DEPENDENCIES
  None

RETURN VALUE
  Returns enum values from diag_process_cfg_enum in case
  of success/failure.

================================================================================*/

diag_process_cfg_enum diag_process_cfg (diag_fs_type * fs, void *buf, unsigned int size);

/*===================================================================================

FUNCTION DIAG_OPEN_FLASH_FILE

DESCRIPTION
  This routine is called internally to open the flash files
  in which diag packets will be stored. This function is
  also used to reset the flash files by truncating its size
  to zero and opening the file. Reseting of the file is done
  when there is no space in either of the two flash files and
  the oldest diag packets need to be cleared to make space for
  the new incoming diag packets to be written on flash.

PARAMETERS
  @param fs
  Pointer to a diag_fs_type type structure object for the
  flash diag file.

  @param reset
  Boolean flag to indicate whether the flash file needs to be
  reset by clearing out oldest diag packets and make room for 
  the new incoming diag packets.
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns a non negative value on success, -1 on failure. 
============================================================================================*/

int diag_open_flash_file(diag_fs_type* fs, boolean reset);

/*===================================================================================

FUNCTION DIAG_WRITE_TO_FLASH

DESCRIPTION
  This routine is called internally to write diag packets to the flash files
  
PARAMETERS
  @param fs
  Pointer to a diag_fs_type type structure object for the
  flash log file

  @param dsm_ptr
  Dsm item pointer which contains the data to be written to flash
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if all the writes are successful and FALSE otherwise. 
============================================================================================*/

boolean diag_write_to_flash(diag_fs_type* fs, dsm_item_type* dsm_ptr);

#endif // DIAG_FLASH_LOGGING_SUPPORT && DIAG_FEATURE_EFS2

#endif // DIAG_FLASH_LOGGING_INTERNAL_H
