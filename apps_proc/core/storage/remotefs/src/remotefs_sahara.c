/*==============================================================================

              Sahara Protocol for Remote Storage Server

DESCRIPTION
  This file contains implementation of the Sahara transport service for Remote Storage
  Server.

Copyright (c) 2011-14 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/

/*==============================================================================

               EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header:
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

YYYY-MM-DD      who    what, where, why
--------------   ---     -------------------------------------------------------
2015-03-05      dks    Get serial number from fs_hwio API interface.
2014-12-05      dks    Add device serial number read support.
2014-06-04      ab     Fix racecondition between TX and RX.
2013-10-20      bb     Added force SIO flush for RESET_RSP after write iovec
2013-05-10      bb     Addressed integer overflow with address checking logic
2013-03-11      bb     Removed HELLO handshake during server init
2013-01-22      ak     Add strict validation for address(s) and range(s) that we get in
                 sahara read requests from app/KS.
2012-07-02      vk     No timeout for first hello message
2012-06-22      vk     Added debug info
2011-11-17      vk     Initial version
==============================================================================*/

#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "rex.h"
#include "assert.h"
#include "remotefs_sahara.h"
#include "remotefs_sahara_pkt.h"
#include "remotefs_sio.h"
#include "remotefs_msg.h"
#include "remotefs_comm_api.h"
#include "fs_hwio.h"
#include "stringl/stringl.h"

#define REMOTEFS_SAHARA_RX_TIMEOUT_MS      10000
#define REMOTEFS_SAHARA_NO_FLUSH           0x0
#define REMOTEFS_SAHARA_FORCE_FLUSH        0x1
#define REMOTEFS_SAHARA_MAX_DEBUG_ITEMS    20

typedef struct
{
  enum remotefs_sahara_state sahara_state;
  enum remotefs_sahara_status sahara_status;
}remotefs_sahara_debug_info;

struct remotefs_sahara_info_type
{
  rex_crit_sect_type cs;
  enum remotefs_sahara_state sahara_state;
  enum remotefs_sahara_status sahara_status;
  struct remotefs_sahara_packet rx_pkt;
  struct remotefs_sahara_packet tx_pkt;
  uint32 rx_bytes;
  uint32 serial_number;
  uint32 hello_seq_num;
  uint32 rx_timeout_ms;
  mem_debug_type mem_debug_info;
  remotefs_sahara_debug_info debug_info[REMOTEFS_SAHARA_MAX_DEBUG_ITEMS];
  uint32 curr_debug_index;
};
static struct remotefs_sahara_info_type remotefs_sahara_info;

static void
remotefs_sahara_add_debug_info (enum remotefs_sahara_state sahara_state,
                                enum remotefs_sahara_status sahara_status)
{
  remotefs_sahara_debug_info *debug_data;

  debug_data =
    &remotefs_sahara_info.debug_info[remotefs_sahara_info.curr_debug_index];
  debug_data->sahara_state = sahara_state;
  debug_data->sahara_status = sahara_status;

  remotefs_sahara_info.curr_debug_index++;
  if (remotefs_sahara_info.curr_debug_index == REMOTEFS_SAHARA_MAX_DEBUG_ITEMS)
  {
    remotefs_sahara_info.curr_debug_index = 0;
  }
}

static void
remotefs_sahara_set_state (enum remotefs_sahara_state sahara_state,
                           enum remotefs_sahara_status sahara_status)
{
  remotefs_sahara_info.sahara_state = sahara_state;
  remotefs_sahara_info.sahara_status = sahara_status;
  remotefs_sahara_add_debug_info (sahara_state, sahara_status);
}

void
remotefs_sahara_init (rex_tcb_type *tcb)
{
  memset (&remotefs_sahara_info, 0x0, sizeof (remotefs_sahara_info));
  rex_init_crit_sect (&remotefs_sahara_info.cs);

  rex_enter_crit_sect (&remotefs_sahara_info.cs);

  remotefs_sahara_info.rx_timeout_ms = REMOTEFS_SAHARA_RX_TIMEOUT_MS;

  remotefs_sio_init (tcb);

  remotefs_sahara_set_state (SAHARA_READY, SAHARA_NAK_SUCCESS);

  rex_leave_crit_sect (&remotefs_sahara_info.cs);
}

static enum remotefs_sahara_status
remotefs_sahara_verify_mem_read_address (uint32 mem_addr, uint32 mem_len)
{
  uint32 start_addr, end_addr, addr_len;

  /* If reading memory region, check against mem address range */
  if (remotefs_sahara_info.sahara_state == SAHARA_MEM_READ)
  {
    start_addr = remotefs_sahara_info.mem_debug_info.mem_base;
    addr_len = remotefs_sahara_info.mem_debug_info.length;
    end_addr = start_addr + addr_len;
  }
  else /* If reading Mem Table, check for Mem table address and size */
  {
    start_addr = (uint32) &remotefs_sahara_info.mem_debug_info;
    addr_len = sizeof(remotefs_sahara_info.mem_debug_info);
    end_addr = start_addr + addr_len;
  }

  if (mem_addr < start_addr || mem_addr > end_addr)
  {
    RMTS_MSG_ERR ("Mem Read of Bad address range %d %d %d",
                  mem_addr, start_addr, end_addr);
    return SAHARA_NAK_INVALID_MEMORY_READ;
  }

  if ((mem_len > addr_len) || (mem_addr + mem_len > end_addr))
  {
    RMTS_MSG_ERR ("Mem Read of Bad address length %d %d %d",
                  mem_addr, mem_len, end_addr);
    return SAHARA_NAK_INVALID_DATA_SIZE_REQUEST;
  }

  return SAHARA_NAK_SUCCESS;
}

/* Based on current state and the packet received from host, suggest the next
   state to transition to */
static remotefs_status_type
remotefs_sahara_get_next_state (void)
{
  enum remotefs_sahara_status sahara_status;
  enum remotefs_sahara_state curr_state;
  struct remotefs_sahara_packet *rx_pkt;

  curr_state = remotefs_sahara_info.sahara_state;
  rx_pkt = &remotefs_sahara_info.rx_pkt;

  sahara_status = remotefs_sahara_interpret_pkt (&remotefs_sahara_info.rx_pkt,
                                                 remotefs_sahara_info.rx_bytes);
  if (sahara_status != SAHARA_NAK_SUCCESS)
  {
    /* Bad packet; switch to EOT */
    remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER, sahara_status);
    return RMFS_ERROR_PROTOCOL;
  }

  switch (curr_state)
  {
    case SAHARA_MEM_DBG_START:
    {
      if (rx_pkt->cmd_id == SAHARA_HELLO_RESP_ID)
      {
        switch (rx_pkt->pkt.hello_resp.mode)
        {
          case SAHARA_MODE_MEMORY_DEBUG:
          {
            remotefs_sahara_set_state (SAHARA_MEM_DBG, SAHARA_NAK_SUCCESS);
            break;
          }
          case SAHARA_MODE_COMMAND:
          {
            remotefs_sahara_set_state (SAHARA_CMD_EXEC_READY,
                                       SAHARA_NAK_SUCCESS);
            break;
          }
          default:
          {
            RMTS_MSG_ERR ("Unexpected Mode in Hello resp : %d"
,
                           rx_pkt->pkt.hello_resp.mode, 0, 0);
            remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                                       SAHARA_NAK_INVALID_HOST_MODE);
            break;
          }
        }
      }
      else
      {
        RMTS_MSG_ERR ("Unexpected RX pkt : %d in current state: %d",
                       rx_pkt->cmd_id, curr_state, 0);
        remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                                   SAHARA_NAK_INVALID_CMD);
      }
      break;
    }

    case SAHARA_MEM_DBG:
    {
      if (rx_pkt->cmd_id == SAHARA_MEMORY_READ_ID)
      {
        sahara_status = remotefs_sahara_verify_mem_read_address (
                               rx_pkt->pkt.mem_read.memory_addr,
                               rx_pkt->pkt.mem_read.memory_length);
        if (sahara_status == SAHARA_NAK_SUCCESS)
        {
          remotefs_sahara_set_state (SAHARA_MEM_READ, SAHARA_NAK_SUCCESS);
        }
        else
        {
          RMTS_MSG_ERR ("Invalid read address for Mem table, status = %d",
                         sahara_status, 0, 0);
          remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,sahara_status);
        }
      }
      else
      {
        RMTS_MSG_ERR ("Unexpected RX pkt : %d in current state: %d",
                       rx_pkt->cmd_id, curr_state, 0);
        remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                                   SAHARA_NAK_INVALID_CMD);
      }
      break;
    }

    case SAHARA_MEM_READ:
    {
      switch (rx_pkt->cmd_id)
      {
        case SAHARA_MEMORY_READ_ID:
        {
          sahara_status = remotefs_sahara_verify_mem_read_address (
                                 rx_pkt->pkt.mem_read.memory_addr,
                                 rx_pkt->pkt.mem_read.memory_length);

          if (sahara_status == SAHARA_NAK_SUCCESS)
          {
            remotefs_sahara_set_state (SAHARA_MEM_READ, SAHARA_NAK_SUCCESS);
          }
          else
          {
            RMTS_MSG_ERR ("Invalid read address for data region, status = %d",
                           sahara_status, 0, 0);
            remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                                       sahara_status);
          }

          break;
        }
        case SAHARA_RESET_ID:
        {
          remotefs_sahara_set_state (SAHARA_RESET, SAHARA_NAK_SUCCESS);
          break;
        }
        default:
        {
          RMTS_MSG_ERR ("Unexpected RX pkt : %d in current state: %d",
                         rx_pkt->cmd_id, curr_state, 0);
          remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                                     SAHARA_NAK_INVALID_CMD);
          break;
        }
      }
      break;
    }

    case SAHARA_CMD_EXEC_READY:
    {
      if (rx_pkt->cmd_id == SAHARA_CMD_EXEC_ID)
      {
        remotefs_sahara_set_state (SAHARA_CMD_EXEC, SAHARA_NAK_SUCCESS);
      }
      else
      {
        RMTS_MSG_ERR ("Unexpected RX pkt : %d in current state: %d",
                       rx_pkt->cmd_id, curr_state, 0);
        remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                                   SAHARA_NAK_INVALID_CMD);
      }
      break;
    }

    case SAHARA_CMD_EXEC:
    {
      if (rx_pkt->cmd_id == SAHARA_CMD_EXEC_DATA_ID)
      {
        remotefs_sahara_set_state (SAHARA_CMD_EXEC_DATA_TX,SAHARA_NAK_SUCCESS);
      }
      else
      {
        RMTS_MSG_ERR ("Unexpected RX pkt : %d in current state: %d",
                       rx_pkt->cmd_id, curr_state, 0);
        remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                                   SAHARA_NAK_INVALID_CMD);
      }
      break;
    }

    case SAHARA_CMD_EXEC_DATA_TX:
    {
      switch (rx_pkt->cmd_id)
      {
        case SAHARA_CMD_SWITCH_MODE_ID:
        {
          if (rx_pkt->pkt.switch_mode.mode == SAHARA_MODE_MEMORY_DEBUG)
          {
            remotefs_sahara_set_state (SAHARA_MEM_DBG_START,SAHARA_NAK_SUCCESS);
          }
          else
          {
            RMTS_MSG_ERR ("Invalid Mode switch : %d",
                          rx_pkt->pkt.switch_mode.mode, 0, 0);
            remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                                       SAHARA_NAK_INVALID_MODE_SWITCH);
          }
          break;
        }
        case SAHARA_CMD_EXEC_ID:
        {
          remotefs_sahara_set_state (SAHARA_CMD_EXEC, SAHARA_NAK_SUCCESS);
          break;
        }
        default:
        {
          RMTS_MSG_ERR ("Unexpected RX pkt : %d in current state: %d",
                         rx_pkt->cmd_id, curr_state, 0);
          remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                                     SAHARA_NAK_INVALID_CMD);
          break;
        }
      }

      break;
    }

    default:
    {
      RMTS_MSG_ERR ("Error in state machine, unexpected state : %d. Check code",
                     remotefs_sahara_info.sahara_state, 0, 0);
      return RMFS_ERROR_BAD_STATE;
    }
  }

  if (remotefs_sahara_info.sahara_state == SAHARA_END_OF_IMG_TRANSFER)
  {
    return RMFS_ERROR_PROTOCOL;
  }

  return RMFS_NO_ERROR;
}

static remotefs_status_type
remotefs_sahara_handle_mem_dbg_start (void)
{
  remotefs_status_type status;

  RMTS_MSG_HIGH ("mem_dbg_hello : start", 0, 0, 0);

  ASSERT (remotefs_sahara_info.sahara_state == SAHARA_MEM_DBG_START);
  if (remotefs_sahara_info.sahara_state != SAHARA_MEM_DBG_START)
  {
    RMTS_MSG_ERR ("mem_dbg_hello: Error in state machine,unexpected state :%d",
                   remotefs_sahara_info.sahara_state, 0, 0);
    return RMFS_ERROR_BAD_STATE;
  }

  remotefs_sahara_form_hello_pkt (&remotefs_sahara_info.tx_pkt,
                                  SAHARA_MODE_MEMORY_DEBUG,
                                  remotefs_sahara_info.hello_seq_num++);

  status = remotefs_sio_tx ((uint8*)&remotefs_sahara_info.tx_pkt.pkt,
                            remotefs_sahara_info.tx_pkt.pkt_len,
                            REMOTEFS_SAHARA_NO_FLUSH,
                            REMOTEFS_SAHARA_FLUSH_TIMEOUT_MAX_MS,
                            (uint8*)&remotefs_sahara_info.rx_pkt.pkt,
                            sizeof (remotefs_sahara_info.rx_pkt.pkt), 0);

  if (status != RMFS_NO_ERROR)
  {
    RMTS_MSG_ERR ("mem_dbg_hello: remotefs_sio_tx error %d", status, 0, 0);
    remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                               SAHARA_NAK_TIMEOUT_TX);
    goto End;
  }

  status = remotefs_sio_rx (remotefs_sahara_info.rx_timeout_ms,
                            &remotefs_sahara_info.rx_bytes);
  if (status != RMFS_NO_ERROR)
  {
    RMTS_MSG_ERR ("mem_dbg_hello: remotefs_sio_rx error %d", status, 0, 0);
    remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                               SAHARA_NAK_TIMEOUT_RX);
    goto End;
  }

  status = remotefs_sahara_get_next_state ();

  RMTS_MSG_HIGH ("mem_dbg_hello: done. status %d", status, 0, 0);

End:
  return status;
}

static void
remotefs_sahara_construct_debug_table (const char *filename,
                                       uint8* data_address, uint32 size)
{
  uint32 dbg_tbl_len;
  dbg_tbl_len = sizeof (remotefs_sahara_info.mem_debug_info);
  memset (&remotefs_sahara_info.mem_debug_info, 0x0, dbg_tbl_len);
  remotefs_sahara_info.mem_debug_info.mem_base = (uint32)data_address;
  remotefs_sahara_info.mem_debug_info.length = size;
  strlcpy (remotefs_sahara_info.mem_debug_info.filename, filename,
           sizeof (remotefs_sahara_info.mem_debug_info.filename));
}

static remotefs_status_type
remotefs_sahara_handle_mem_dbg (const char *filename, uint8* data_address,
                                uint32 size)
{
  remotefs_status_type status;

  RMTS_MSG_HIGH ("mem_dbg_core: start", 0, 0, 0);

  ASSERT (remotefs_sahara_info.sahara_state == SAHARA_MEM_DBG);
  if (remotefs_sahara_info.sahara_state != SAHARA_MEM_DBG)
  {
    RMTS_MSG_ERR ("mem_dbg_core: Error in state machine, unexpected state:%d",
                   remotefs_sahara_info.sahara_state, 0, 0);
    return RMFS_ERROR_BAD_STATE;
  }

  remotefs_sahara_construct_debug_table (filename, data_address, size);
  remotefs_sahara_form_mem_dbg_pkt (&remotefs_sahara_info.tx_pkt,
                            (uint32) &remotefs_sahara_info.mem_debug_info,
                            sizeof (remotefs_sahara_info.mem_debug_info));

  status = remotefs_sio_tx ((uint8 *)&remotefs_sahara_info.tx_pkt.pkt,
                            remotefs_sahara_info.tx_pkt.pkt_len,
                            REMOTEFS_SAHARA_NO_FLUSH,
                            REMOTEFS_SAHARA_FLUSH_TIMEOUT_MAX_MS,
                            (uint8*)&remotefs_sahara_info.rx_pkt.pkt,
                            sizeof (remotefs_sahara_info.rx_pkt.pkt), 0);
  if (status != RMFS_NO_ERROR)
  {
    RMTS_MSG_ERR ("mem_dbg_core: Mem Table Tx failed %d", status, 0, 0);
    remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                               SAHARA_NAK_TIMEOUT_TX);
    goto End;
  }

  status = remotefs_sio_rx (remotefs_sahara_info.rx_timeout_ms,
                            &remotefs_sahara_info.rx_bytes);
  if (status != RMFS_NO_ERROR)
  {
    RMTS_MSG_ERR ("mem_dbg_core: Mem Table Rx failed %d", status, 0, 0);
    remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                               SAHARA_NAK_TIMEOUT_RX);
    goto End;
  }

  status = remotefs_sahara_get_next_state ();
  if (status != RMFS_NO_ERROR)
  {
    goto End;
  }

  /* Process pkts until error */
  while (remotefs_sahara_info.sahara_state != SAHARA_RESET)
  {
    switch (remotefs_sahara_info.sahara_state)
    {
      case SAHARA_MEM_READ:
      {
        status = remotefs_sio_tx (
                  (uint8*)remotefs_sahara_info.rx_pkt.pkt.mem_read.memory_addr,
                  remotefs_sahara_info.rx_pkt.pkt.mem_read.memory_length,
                  REMOTEFS_SAHARA_NO_FLUSH,
                  REMOTEFS_SAHARA_FLUSH_TIMEOUT_MAX_MS,
                  (uint8*)&remotefs_sahara_info.rx_pkt.pkt,
                  sizeof (remotefs_sahara_info.rx_pkt.pkt), 0);
        if (status != RMFS_NO_ERROR)
        {
          RMTS_MSG_ERR ("mem_dbg_core: Mem Read Tx failed %d", status, 0, 0);
          remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                                     SAHARA_NAK_TIMEOUT_TX);
          goto End;
        }

        break;
      }

      default:
      {
        RMTS_MSG_ERR ("mem_dbg_core:Error in state machine,unexpected state:%d",
                       remotefs_sahara_info.sahara_state, 0, 0);
        ASSERT (0);
        status = RMFS_ERROR_BAD_STATE;
        goto End;
      }
    }

    status = remotefs_sio_rx (remotefs_sahara_info.rx_timeout_ms,
                              &remotefs_sahara_info.rx_bytes);
    if (status != RMFS_NO_ERROR)
    {
      RMTS_MSG_ERR ("mem_dbg_core: Mem Read Rx failed %d", status, 0, 0);
      remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                                 SAHARA_NAK_TIMEOUT_RX);
      goto End;
    }

    status = remotefs_sahara_get_next_state ();
    if (status != RMFS_NO_ERROR)
    {
      goto End;
    }
  }

  RMTS_MSG_HIGH ("mem_dbg_core: Done. Reset received. Status %d", status,0,0);

End:
  return status;
}

static enum remotefs_sahara_status
remotefs_sahara_handle_read_serial (void)
{
  remotefs_sahara_info.serial_number = fs_hwio_get_device_serial_number ();

  RMTS_MSG_HIGH ("cmd_exec : Read serial success %d",
                 remotefs_sahara_info.serial_number, 0, 0);
  return SAHARA_NAK_SUCCESS;
}

static enum remotefs_sahara_status
remotefs_sahara_execute_command (uint32 cli_cmd)
{
  if (cli_cmd == SHARA_READ_SERIAL)
  {
    return remotefs_sahara_handle_read_serial();
  }
  else
  {
    return SAHARA_NAK_EXEC_CMD_UNSUPPORTED;
  }
}

static void
remotefs_sahara_form_cmd_exec_resp_pkt_helper (uint32 cli_cmd)
{
  uint32 resp_len;

  ASSERT (cli_cmd == SHARA_READ_SERIAL);
  if (cli_cmd != SHARA_READ_SERIAL)
  {
    resp_len = 0;
  }
  else
  {
    resp_len = sizeof (remotefs_sahara_info.serial_number);
  }

  remotefs_sahara_form_cmd_exec_resp_pkt (&remotefs_sahara_info.tx_pkt,
                                          cli_cmd, resp_len);
}

static void
remotefs_sahara_form_cmd_exec_data_pkt_helper (uint32 cli_cmd)
{
  memset (&remotefs_sahara_info.tx_pkt.pkt, 0x0,
          sizeof (remotefs_sahara_info.tx_pkt.pkt));
  remotefs_sahara_info.tx_pkt.pkt_len = 0;

  ASSERT (cli_cmd == SHARA_READ_SERIAL);
  if (cli_cmd == SHARA_READ_SERIAL)
  {
    memscpy (&remotefs_sahara_info.tx_pkt.pkt,
             sizeof (remotefs_sahara_info.tx_pkt.pkt),
             &remotefs_sahara_info.serial_number,
             sizeof (remotefs_sahara_info.serial_number));
    remotefs_sahara_info.tx_pkt.pkt_len =
             sizeof (remotefs_sahara_info.serial_number);
  }
}

static remotefs_status_type
remotefs_handle_cmd_execute (void)
{
  remotefs_status_type status = RMFS_NO_ERROR;
  enum remotefs_sahara_status sahara_status;

  RMTS_MSG_HIGH ("cmd_exe: start", 0, 0, 0);

  ASSERT (remotefs_sahara_info.sahara_state == SAHARA_CMD_EXEC_READY);
  if (remotefs_sahara_info.sahara_state != SAHARA_CMD_EXEC_READY)
  {
    RMTS_MSG_ERR ("cmd_exe: Error in state machine, unexpected state : %d",
                   remotefs_sahara_info.sahara_state, 0, 0);
    return RMFS_ERROR_BAD_STATE;
  }

  /* Process commands until Command mode exits or error encountered */
  while (status == RMFS_NO_ERROR)
  {
    switch (remotefs_sahara_info.sahara_state)
    {
      case SAHARA_MEM_DBG_START:
      {
        goto End;
      }

      case SAHARA_CMD_EXEC_READY:
      {
        remotefs_sahara_form_cmd_ready_pkt (&remotefs_sahara_info.tx_pkt);
        break;
      }

      case SAHARA_CMD_EXEC:
      {
        sahara_status = remotefs_sahara_execute_command (
                           remotefs_sahara_info.rx_pkt.pkt.cmd_exec.cli_cmd);
        if (sahara_status == SAHARA_NAK_SUCCESS)
        {
          remotefs_sahara_form_cmd_exec_resp_pkt_helper (
                     remotefs_sahara_info.rx_pkt.pkt.cmd_exec.cli_cmd);
        }
        else
        {
          RMTS_MSG_ERR ("cmd_exec: Client exec failed %d %d",
                        remotefs_sahara_info.rx_pkt.pkt.cmd_exec.cli_cmd,
                        sahara_status, 0);
          remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,sahara_status);
          status = RMFS_ERROR_DEVICE;
          goto End;
        }
        break;
      }

      case SAHARA_CMD_EXEC_DATA_TX:
      {
        remotefs_sahara_form_cmd_exec_data_pkt_helper (
                             remotefs_sahara_info.rx_pkt.pkt.cmd_data.cli_cmd);
        break;
      }

      default:
      {
        RMTS_MSG_ERR ("cmd_exe: Error in state machine, unexpected state : %d",
                       remotefs_sahara_info.sahara_state, 0, 0);
        ASSERT (0); /* not expected */
        status = RMFS_ERROR_BAD_STATE;
        goto End;
      }
    }

    status = remotefs_sio_tx ((uint8 *)&remotefs_sahara_info.tx_pkt.pkt,
                              remotefs_sahara_info.tx_pkt.pkt_len,
                              REMOTEFS_SAHARA_NO_FLUSH,
                              REMOTEFS_SAHARA_FLUSH_TIMEOUT_MAX_MS,
                              (uint8*)&remotefs_sahara_info.rx_pkt.pkt,
                              sizeof (remotefs_sahara_info.rx_pkt.pkt), 0);

    if (status != RMFS_NO_ERROR)
    {
      RMTS_MSG_ERR ("cmd_exe: remotefs_sio_tx failed %d", status, 0, 0);
      remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                                 SAHARA_NAK_TIMEOUT_TX);
      continue;
    }

    status = remotefs_sio_rx (remotefs_sahara_info.rx_timeout_ms,
                              &remotefs_sahara_info.rx_bytes);
    if (status != RMFS_NO_ERROR)
    {
      RMTS_MSG_ERR ("cmd_exe: remotefs_sio_rx failed %d", status, 0, 0);
      remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                                 SAHARA_NAK_TIMEOUT_RX);
      continue;
    }

    status = remotefs_sahara_get_next_state ();
    if (status != RMFS_NO_ERROR)
    {
      continue;
    }
  }

End:
  RMTS_MSG_HIGH ("cmd_exe: done. Status %d", status, 0, 0);
  return status;
}

static remotefs_status_type
remotefs_sahara_handle_reset (void)
{
  remotefs_status_type status;

  remotefs_sahara_form_reset_resp_pkt (&remotefs_sahara_info.tx_pkt);

  status = remotefs_sio_tx ((uint8 *)&remotefs_sahara_info.tx_pkt.pkt,
                            remotefs_sahara_info.tx_pkt.pkt_len,
                            REMOTEFS_SAHARA_FORCE_FLUSH,
                            REMOTEFS_SAHARA_FLUSH_TIMEOUT_MAX_MS,
                            (uint8*)&remotefs_sahara_info.rx_pkt.pkt,
                            sizeof (remotefs_sahara_info.rx_pkt.pkt), 0);
  if (status != RMFS_NO_ERROR)
  {
    RMTS_MSG_ERR ("reset_resp: remotefs_sio_tx failed %d", status, 0, 0);
    remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                               SAHARA_NAK_TIMEOUT_TX);
  }
  else
  {
    remotefs_sahara_set_state (SAHARA_READY, SAHARA_NAK_SUCCESS);
  }

  return status;
}

static remotefs_status_type
remotefs_sahara_handle_end_of_img_transfer (uint32 img_id,
                enum remotefs_sahara_status sahara_error)
{
  remotefs_status_type status;

  /* If ending transfer due to TX or RX errors, no more pkts to send. Return */
  if (sahara_error == SAHARA_NAK_TIMEOUT_RX ||
      sahara_error == SAHARA_NAK_TIMEOUT_TX)
  {
    return RMFS_NO_ERROR;
  }

  remotefs_sahara_form_end_of_img_transfer_pkt (&remotefs_sahara_info.tx_pkt,
                                                img_id, (uint32)sahara_error);

  status = remotefs_sio_tx ((uint8*)&remotefs_sahara_info.tx_pkt.pkt,
                            remotefs_sahara_info.tx_pkt.pkt_len,
                            REMOTEFS_SAHARA_FORCE_FLUSH,
                            REMOTEFS_SAHARA_FLUSH_TIMEOUT_MAX_MS,
                            (uint8*)&remotefs_sahara_info.rx_pkt.pkt,
                            sizeof (remotefs_sahara_info.rx_pkt.pkt), 0);
  if (status != RMFS_NO_ERROR)
  {
    RMTS_MSG_ERR ("end_of_transfer: remotefs_sio_tx failed %d", status, 0, 0);
    remotefs_sahara_set_state (SAHARA_END_OF_IMG_TRANSFER,
                               SAHARA_NAK_TIMEOUT_TX);
  }

  return status;
}

remotefs_status_type
remotefs_sahara_memory_debug (const char *filename, uint8* data_address,
                              uint32 size)
{
  remotefs_status_type status = RMFS_NO_ERROR;

  rex_enter_crit_sect (&remotefs_sahara_info.cs);

  RMTS_MSG_HIGH ("memory_debug : start", 0, 0, 0);

  ASSERT (remotefs_sahara_info.sahara_state == SAHARA_READY);
  if (remotefs_sahara_info.sahara_state != SAHARA_READY)
  {
    RMTS_MSG_ERR ("memory_debug : Error in state machine, unexpected state:%d",
                   remotefs_sahara_info.sahara_state, 0, 0);
    status = RMFS_ERROR_BAD_STATE;
    goto Quit;
  }

  remotefs_sahara_set_state (SAHARA_MEM_DBG_START, SAHARA_NAK_SUCCESS);

  /* Process states until done or until an error is encountered. */
  while (status == RMFS_NO_ERROR &&
         remotefs_sahara_info.sahara_state != SAHARA_READY)
  {
    switch (remotefs_sahara_info.sahara_state)
    {
      case SAHARA_MEM_DBG_START:
      {
        status = remotefs_sahara_handle_mem_dbg_start ();
        break;
      }

      case SAHARA_MEM_DBG:
      {
        status = remotefs_sahara_handle_mem_dbg (filename, data_address, size);
        break;
      }

      case SAHARA_CMD_EXEC_READY:
      {
        status = remotefs_handle_cmd_execute ();
        break;
      }

      case SAHARA_RESET:
      {
        status = remotefs_sahara_handle_reset ();
        break;
      }

      default:
      {
        RMTS_MSG_ERR ("memory_debug:Error in state machine,unexpected state:%d",
                       remotefs_sahara_info.sahara_state, 0, 0);
        ASSERT (0);
        status = RMFS_ERROR_BAD_STATE;
        goto Quit;
      }
    }
  }

  if (status == RMFS_ERROR_BAD_STATE) /* Got bad state from handlers. Quit */
  {
    goto Quit;
  }

  if (status == RMFS_NO_ERROR)
  {
    RMTS_MSG_HIGH ("memory_debug: Success! Status : %d", status, 0, 0);
  }
  else
  {
    ASSERT (remotefs_sahara_info.sahara_status == SAHARA_END_OF_IMG_TRANSFER);
    if (remotefs_sahara_info.sahara_status != SAHARA_END_OF_IMG_TRANSFER)
    {
      status = RMFS_ERROR_BAD_STATE;
      goto Quit;
    }

   (void) remotefs_sahara_handle_end_of_img_transfer(0, //dummy img Id
                          remotefs_sahara_info.sahara_status);

    RMTS_MSG_ERR ("memory_debug: Failed! Status : %d", status, 0, 0);

    /* Set state back to READY to handle next transfer */
    remotefs_sahara_set_state (SAHARA_READY, SAHARA_NAK_SUCCESS);
  }

Quit:
  rex_leave_crit_sect(&remotefs_sahara_info.cs);
  return status;
}
