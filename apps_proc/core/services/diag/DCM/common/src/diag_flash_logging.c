/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics Flash Logging 

General Description
  Diagnostic flash logging core routines.

Copyright (c) 2022 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#if defined(DIAG_FLASH_LOGGING_SUPPORT)

#include "comdef.h"
#include "com_dtypes.h"
#include "diag_flash_logging_internal.h"
#include "msg.h"
#include "msgi.h"
#include "diaglogi.h"
#include "diagcomm_io.h"
#include "diagcomm_v.h"
#include "diag_v.h"
#include "diagi_v.h"
#include "diag_fs.h"
#include "ULog_Diag.h"
#include "event.h"
#include "eventi.h"
#include "fs_public.h"
#include "fs_fcntl.h"
#include "crc.h"
#include "osal.h"
#include "diag_fs.h"
#include "dsm.h"
#include "err.h"
#include "dog.h"
#include "fs_sys_types.h"
#include "string.h"
#include "malloc.h"

/* -------------------------------------------------------------------------
** Macros
** ------------------------------------------------------------------------- */
#define DIAG_CFG_READ_SRC_BUF_SIZE   256

/* -------------------------------------------------------------------------
** Externs
** ------------------------------------------------------------------------- */
extern boolean eof_reached;
extern boolean diag_hdlc_disabled;
extern uint32 bytes_written_to_flash;
extern osal_tcb_t diag_task_tcb;
extern boolean diag_hdlc_decode (diag_hdlc_decode_type * hdlc);

/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */

/*! @brief
    Structure of diag_fs_type for the .cfg
    file which contains the mask set commands.
*/

diag_fs_type diag_cfg = {
  0,
  0,
  0xFFFFFFFFU,          /* Unused field */
  -1,
  DIAG_FLASH_LOGGING_CFG,
};  

/*! @brief
    Array containing the two diag flash files.
    Diag logs will be written to these files.
*/

diag_fs_type diag_flash_log_files[2] = {
  {
    0,                // read_pos
    0,                // write_pos
    0xFFFFFFFFU,      // max_size, unused
    -1,               // handle
    DIAG_FLASH_FILE1  // fname
  },

  {
    0,				  // read_pos
    0,				  // write_pos
    0xFFFFFFFFU,	  // max_size, unused
    -1,				  // handle
    DIAG_FLASH_FILE2  // fname
  }
};


/*! @brief 
    This boolean object indicates the mode of diag logging.
    If TRUE, outbound data will be routed to flash logging 
    in the file system. If FALSE, outbound traffic will be
    routed to the default SIO interface (USB in this case).
*/
boolean diag_flash_logging_enabled = FALSE;

/*! @brief 
    Indicates if the size of a single command in .cfg 
    exceeds the buffer to copy each command.
*/
boolean buffer_overflow = FALSE;

/*! @brief 
    Indicates whether the CRC of a mask set command packet
    in .cfg is proper or not.
*/
boolean bad_crc = FALSE;

/*! @brief
    This counter tells the number of bytes read from .cfg
*/
uint32 bytes_read_frm_cfg = 0;

/*! @brief
    This header must be placed at the beginning of the flash
    qmdl2 files
*/
diag_qmdl2_hdr qmdl2_hdr = {DIAG_QMDL2_V1_HDR_LEN, 1, 0, 0};

/*! @brief
    This global is to capture whether the request is for
    enabling or disabling flash logging.
    This is used while handling the flash logging signal
*/
boolean diag_current_flash_logging_request = FALSE;


/* -------------------------------------------------------------------------
** Function implementations
** ------------------------------------------------------------------------- */

/*===========================================================================

FUNCTION DIAG_ADJUST_CFG_BUFFER_PTR

DESCRIPTION
  This function adjusts the pointers of the source and destination
  buffers to read the next command from .cfg.

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

================================================================================*/
void diag_adjust_cfg_buffer_ptr (diag_hdlc_decode_type * hdlc, diag_fs_type * fs)
{
  if(!hdlc || !fs)
  {
    ULogFront_RealTimePrintf(diag_flash_logging_ulog_handle, 0, "diag_adjust_cfg_buffer_ptr called with NULL param");
    return;
  }	
  
  /* If src_ptr is not exhausted, update read_pos before exit. */
  if (hdlc->src_size > hdlc->src_idx)
  {
    fs->read_pos -= hdlc->src_size - hdlc->src_idx;
    bytes_read_frm_cfg -= hdlc->src_size - hdlc->src_idx;
  }
  
  /* 
    Adjusting the pointers to read the next command
    from .cfg file.
  */
  hdlc->src_size = 0;
  hdlc->src_idx  = 0;
  hdlc->dest_idx = 0;  
}


/*===========================================================================

FUNCTION DIAG_READ_CMD_FRM_CFG

DESCRIPTION
  This function reads a packet from an HDLC encoded .cfg file, 
  and verifies whether CRC is correct. NHDLC encoded cfg file is
  not supported for the purpose of logging to flash file system.

PARAMETERS
  @param hdlc
  Pointer containing information of source and destination buffer
  for copying the hdlc encoded commands from .cfg

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if a valid packet was found.

===========================================================================*/
boolean diag_read_cmd_frm_cfg (diag_hdlc_decode_type * hdlc)
{
  boolean pkt_bnd = FALSE;
  /* Cyclic Redundancy Check of incoming packet. */
  word crc;
  int return_val;
  
  if (hdlc && hdlc->src_ptr && hdlc->dest_ptr)
  {

    pkt_bnd = diag_hdlc_decode (hdlc);

    if (pkt_bnd == TRUE)
    {
      /* 
        At a packet boundary, check the CRC, reset dst_idx for the next
        packet, and return the packet (if valid). 
      */
      crc = crc_16_l_step(CRC_16_L_STEP_SEED, (const void *)hdlc->dest_ptr, (unsigned int)(hdlc->dest_idx - 1));
      if (crc == CRC_16_L_OK)
      {
        hdlc->dest_idx -= DIAG_ASYNC_TAIL_SIZE; 
      }
      else
      {
        ULogFront_RealTimePrintf(diag_flash_logging_ulog_handle, 1, "Bad CRC. hdlc->dest_idx = %lu",hdlc->dest_idx);
        pkt_bnd = FALSE;
        bad_crc = TRUE;
      }
    }
    else if (hdlc->dest_idx >= hdlc->dest_size)
    {
      /* 
         The inbound packet is too large.  Therefore, we cannot recieve it.
         Stop reading the cfg file. 
      */

      ULogFront_RealTimePrintf(diag_flash_logging_ulog_handle, 1, "Buffer overflow. hdlc->dest_idx = %lu",hdlc->dest_idx);
      buffer_overflow = TRUE;
    }
  }
  
  return (pkt_bnd);
}



/*===========================================================================

FUNCTION DIAG_PROCESS_CFG

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

===========================================================================*/

diag_process_cfg_enum diag_process_cfg (diag_fs_type * fs, void *buf, unsigned int size)
{
  diag_hdlc_decode_type hdlc;
  unsigned int src_buf_size = 0;
  diag_process_cfg_enum ret_val = DIAG_PROCESS_CFG_READ_FAILURE;

  /* Length of packet, if boundary is found.
     0 is returned if no packet found. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize HDLC structure */
  memset (&hdlc, 0, sizeof (hdlc));

  /* Allocate a buffer from the comm layer to use as a work buffer
     for the file stream input processing. */
  src_buf_size = DIAG_CFG_READ_SRC_BUF_SIZE;
  hdlc.src_ptr = malloc (src_buf_size);
  bytes_read_frm_cfg = 0;

  if (hdlc.src_ptr)
  {

    if (fs && fs->handle >= 0)
    {
      hdlc.dest_ptr = buf;
      hdlc.dest_size = size;

      do
      {
        /* If the src buffer has been exhausted, time to read more */
        if (hdlc.src_size == hdlc.src_idx)
        {
          hdlc.src_size = diag_fs_read (fs, hdlc.src_ptr, src_buf_size);
          bytes_read_frm_cfg += hdlc.src_size;

          if (hdlc.src_size > 0)
          {
            hdlc.src_idx = 0;
          }
          else
          {
            ret_val = (eof_reached==TRUE)? DIAG_PROCESS_CFG_SUCCESS : DIAG_PROCESS_CFG_READ_FAILURE;
            eof_reached = FALSE; // resetting to false for reading next file
            break;
          }
        }

        if (hdlc.src_size > hdlc.src_idx)
        {
          if (diag_read_cmd_frm_cfg(&hdlc) == TRUE)
          {
            // Call the callback of the command
            diagpkt_process_request (buf, (uint16) hdlc.dest_idx, NULL, NULL,FALSE);

            // Adjust the src and dest pointers to start reading the next command 
            diag_adjust_cfg_buffer_ptr(&hdlc,fs);
          }
          else if(bad_crc)
          {
            bad_crc = FALSE;

            // Adjust the src and dest pointers to start reading the next command 
            diag_adjust_cfg_buffer_ptr(&hdlc,fs);
          }
          else if(buffer_overflow)
          {
            buffer_overflow = FALSE;

            // Size of the command is more than the buffer size
            ret_val = DIAG_PROCESS_CFG_BUFFER_OVERFLOW_FAILURE;
            break;
          }
        }
      }
      while (fs->handle >= 0);
    }

    // Free the memory allocated from heap
    free(hdlc.src_ptr);
  
    // Close the .cfg file after reading
    diag_fs_terminate(fs, FALSE);
  }
  else
  {
    ULogFront_RealTimePrintf(diag_flash_logging_ulog_handle, 0, "Malloc failure, could not allocate memory to read .cfg file");
    ret_val = DIAG_PROCESS_CFG_MALLOC_FAILURE;
  }  

    return ret_val;
}



/*===================================================================================
FUNCTION DIAG_ENABLE_FLASH_LOGGING

DESCRIPTION
  This routine is called from the external world to enable/disable
  writing of diag packets to flash/EFS. 

  This routine will execute the following steps
  * Clear masks from mask tables and sets signal to diag task to send mask update
    control packets
  * Sets signal to diag task for enabling or disabling flash logging

PARAMETERS
  @param config
  Pointer to a diag_flash_config type structure object which 
  indicates the mode of logging
  
  config->flash_logging_enable = TRUE to start flash logging
  config->version = 1, version 1. Currently this field is ignored
  config->reserved, this field is ignored currently
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns enum values from diag_flash_logging_enum in case
  of success/failure.

  Returning success means signal is set to diag task succesfully
  for enabling/disabling flash logging
============================================================================================*/

diag_flash_logging_enum diag_enable_flash_logging(diag_flash_config* config)
{
  osal_sigs_t return_sigs = 0;
  
  if(!config)
  {
    ULogFront_RealTimePrintf(diag_flash_logging_ulog_handle, 1,"Param config is NULL, caller thread is 0x%x",osal_thread_self());
    return DIAG_FLASH_LOGGING_FAILURE;
  }	

  // No need to change the mode of logging
  if(config->flash_logging_enable == diag_flash_logging_enabled)
    return DIAG_FLASH_LOGGING_SUCCESS;

  /* Clear the masks for f3s, logs and events.
     Mask update control packets will be sent to modem by diag task. */

  msg_set_all_rt_masks_adv (0, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  diagcomm_mask_update(DIAG_UPDATE_MASK_F3, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL, FALSE);
  
  
  log_config_mask(-1, FALSE, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  log_config_mstr_mask(-1, FALSE, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  diagcomm_mask_update(DIAG_UPDATE_MASK_LOGS, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL, FALSE);
  
  
  event_set_all_rt_masks(0, 0);
  diagcomm_mask_update(DIAG_UPDATE_MASK_EVENTS, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL, FALSE);

  if(config->flash_logging_enable == TRUE)
  {
    diag_current_flash_logging_request = TRUE;
  }	
  else
  {
    diag_current_flash_logging_request = FALSE;
  }

  (void) osal_set_sigs (&diag_task_tcb, DIAG_FLASH_LOGGING_SIG, &return_sigs);
  
  return DIAG_FLASH_LOGGING_SUCCESS;
}

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

int diag_open_flash_file(diag_fs_type* fs, boolean reset)
{
  int fs_rsp = 0;
  boolean ret_val = FALSE;
    
  if(!fs)
    return -1;

  if(reset == TRUE)
  {
    /* 
       First close the file and then open it again with 
       O_TRUNC option to truncate the file to zero length.

       Calling efs_open without closing the file might
       throw up "Too many open files error", err code 1024.
    */  
    diag_fs_terminate(fs, FALSE);
	
    // O_TRUNC makes sure that the file size becomes 0
    fs_rsp = efs_open(fs->fname, O_TRUNC | O_WRONLY);
  }
  else
  {
    // Create the file, and open the file with write-only permission
    fs_rsp = efs_open(fs->fname, O_CREAT | O_WRONLY); 
  }
  
  if(fs_rsp >= 0)
  {
    fs->handle = fs_rsp;
    fs->write_pos = 0;

    /* Seek to the end of the file. We are not doing SEEK_SET to zero as
       the file might be existing from the previous iteration of flash logging. */
    fs_rsp = efs_lseek(fs->handle,0,SEEK_END);
  
    if(fs_rsp < 0)
    {
      ULogFront_RealTimePrintf(diag_flash_logging_ulog_handle, 2, "diag_open_flash_file: Error %d while setting file offset for flash log file 0x%x",efs_errno,fs);
      return fs_rsp;
    }

    if(fs_rsp == 0) // efs_lseek returned 0 offset. Happens for the first time when the file gets created or when the file is reset
    {
      // Update the qmdl2 header
      qmdl2_hdr.hdlc_data_type = (diag_hdlc_disabled == TRUE)? 0 : 1;
      ret_val = diag_fs_write(fs, (void*)&qmdl2_hdr, DIAG_QMDL2_V1_HDR_LEN);

      if(ret_val == FALSE)
      {
        ULogFront_RealTimePrintf(diag_flash_logging_ulog_handle, 1, "Error while trying to write qmdl2 header to flash log file %s",fs->fname);
        return -1;
      } 

      fs_rsp = fs->write_pos;
    }
  
    /* Update the write pointer. Write position will be at 10 if 
       the file got created for the first time as qmdl2 header length
       is 10, else it will point to the end of the file. */
    fs->write_pos = fs_rsp;
  }
  else
  {
    ULogFront_RealTimePrintf(diag_flash_logging_ulog_handle, 2, "diag_open_flash_file: Error %d while opening flash file 0x%x",efs_errno,fs);
  }
  
  return fs_rsp;
}

/*===================================================================================

FUNCTION DIAG_WRITE_TO_FLASH

DESCRIPTION
  This routine is called internally to write diag packets to the flash files
  
PARAMETERS
  @param fs
  Pointer to a diag_fs_type type structure object for the
  flash diag file

  @param dsm_ptr
  Dsm item pointer which contains the data to be written to flash
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if all the writes are successful and FALSE otherwise. 
============================================================================================*/

boolean diag_write_to_flash(diag_fs_type* fs, dsm_item_type* dsm_ptr)
{
  boolean ret_val = TRUE;
  dword old_write_pos = 0;

  if(!fs || !dsm_ptr)
    return FALSE;

  while(dsm_ptr)
  {
    old_write_pos = fs->write_pos;
    ret_val = diag_fs_write(fs, dsm_ptr->data_ptr, dsm_ptr->used);

    if(ret_val == FALSE)
    {
      ULogFront_RealTimePrintf(diag_flash_logging_ulog_handle, 1, "Error while trying to write to flash log file %s",fs->fname);
      return ret_val;
    }

    bytes_written_to_flash += fs->write_pos - old_write_pos;

    /* Move on to the next chained dsm item
       for chained dsm packets. */
    dsm_ptr = dsm_ptr->pkt_ptr;
  }

  return ret_val;
}

#endif /* DIAG_FLASH_LOGGING_SUPPORT && DIAG_FEATURE_EFS2 */

