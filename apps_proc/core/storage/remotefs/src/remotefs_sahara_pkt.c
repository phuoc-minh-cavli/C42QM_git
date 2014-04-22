/*==============================================================================

                      RMTS Sahara Protocol Interface Packet Implemenation.

DESCRIPTION
   This file contains RMTS Sahara Protocol Interface Packet Implemenations.

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/remotefs/src/remotefs_sahara_pkt.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

YYYY-MM-DD       who     what, where, why
--------------   ---     -------------------------------------------------------
2014-12-05       dks     Create.
==============================================================================*/

#include "comdef.h"
#include "assert.h"
#include "remotefs_sahara_pkt.h"
#include "remotefs_sahara.h"

/* Generic shara packet header */
PACKED struct remotefs_sahara_packet_header
{
  uint32 command;
  uint32 length;
}PACKED_POST;

static enum remotefs_sahara_status
remotefs_sahara_interpret_hello_resp_pkt (
        struct remotefs_sahara_packet *sahara_pkt, uint32 pkt_len)
{
  struct sahara_packet_hello_resp *hello_resp;
  hello_resp = &sahara_pkt->pkt.hello_resp;

  if (pkt_len != sizeof(struct sahara_packet_hello_resp))
  {
    return SAHARA_NAK_INVALID_PACKET_SIZE;
  }

  if (hello_resp->version < hello_resp->version_supported)
  {
    return SAHARA_NAK_INVALID_HOST_PROTOCOL;
  }

  if ((hello_resp->version > SAHARA_VERSION_MAJOR) ||
      (hello_resp->version < SAHARA_VERSION_MAJOR_SUPPORTED))
  {
    return SAHARA_NAK_PROTOCOL_MISMATCH;
  }

  if ((hello_resp->version > SAHARA_VERSION_MAJOR) ||
      (hello_resp->version < SAHARA_VERSION_MAJOR_SUPPORTED))
  {
    return SAHARA_NAK_PROTOCOL_MISMATCH;
  }

  sahara_pkt->cmd_id = SAHARA_HELLO_RESP_ID;
  sahara_pkt->pkt_len = sizeof(struct sahara_packet_hello_resp);

  return SAHARA_NAK_SUCCESS;
}

static enum remotefs_sahara_status
remotefs_sahara_interpret_sahara_generic_pkt(
                                   struct remotefs_sahara_packet *sahara_pkt,
                                   uint32 pkt_len, uint32 expected_len)
{
  struct remotefs_sahara_packet_header *hdr;
  hdr = (struct remotefs_sahara_packet_header *)&sahara_pkt->pkt;

  if (pkt_len != expected_len)
  {
    return SAHARA_NAK_INVALID_PACKET_SIZE;
  }
  else if (hdr->length != expected_len)
  {
    return SAHARA_NAK_INVALID_PACKET_SIZE;
  }

  sahara_pkt->cmd_id = hdr->command;
  sahara_pkt->pkt_len = hdr->length;
  return SAHARA_NAK_SUCCESS;
}

static enum remotefs_sahara_status
remotefs_sahara_interpret_done_pkt (
                                   struct remotefs_sahara_packet *sahara_pkt,
                                   uint32 pkt_len)
{
  return remotefs_sahara_interpret_sahara_generic_pkt (sahara_pkt,
                pkt_len, sizeof (struct sahara_packet_done_resp));
}

static enum remotefs_sahara_status
remotefs_sahara_interpret_reset_pkt (
                                   struct remotefs_sahara_packet *sahara_pkt,
                                   uint32 pkt_len)
{
  return remotefs_sahara_interpret_sahara_generic_pkt (
          sahara_pkt, pkt_len, sizeof (struct sahara_packet_reset));
}

static enum remotefs_sahara_status
remotefs_sahara_interpret_mem_read_pkt (
                                   struct remotefs_sahara_packet *sahara_pkt,
                                   uint32 pkt_len)
{
  struct sahara_packet_memory_read *mem_read;
  uint32 mem_read_len = sizeof(struct sahara_packet_memory_read);

  mem_read = &sahara_pkt->pkt.mem_read;

  if (pkt_len != mem_read_len)
  {
    return SAHARA_NAK_INVALID_PACKET_SIZE;
  }

  if (mem_read->length != mem_read_len)
  {
    return SAHARA_NAK_INVALID_PACKET_SIZE;
  }

  sahara_pkt->cmd_id = SAHARA_MEMORY_READ_ID;
  sahara_pkt->pkt_len = mem_read_len;

  return SAHARA_NAK_SUCCESS;
}

static enum remotefs_sahara_status
remotefs_sahara_interpret_switch_mode_pkt (
                                  struct remotefs_sahara_packet *sahara_pkt,
                                  uint32 pkt_len)
{
  struct sahara_packet_cmd_switch_mode *switch_mode;
  uint32 switch_mode_len = sizeof(struct sahara_packet_cmd_switch_mode);

  switch_mode = &sahara_pkt->pkt.switch_mode;

  if (pkt_len != switch_mode_len)
  {
    return SAHARA_NAK_INVALID_PACKET_SIZE;
  }

  if (switch_mode->length != switch_mode_len)
  {
    return SAHARA_NAK_INVALID_PACKET_SIZE;
  }

  if (switch_mode->mode != SAHARA_MODE_MEMORY_DEBUG)
  {
    return SAHARA_NAK_INVALID_MODE_SWITCH;
  }

  sahara_pkt->cmd_id = SAHARA_CMD_SWITCH_MODE_ID;
  sahara_pkt->pkt_len = switch_mode_len;

  return SAHARA_NAK_SUCCESS;
}


static enum remotefs_sahara_status
remotefs_sahara_interpret_cmd_exe_pkt (
                                  struct remotefs_sahara_packet *sahara_pkt,
                                  uint32 pkt_len)
{
  struct sahara_packet_cmd_exec *cmd_exec;
  uint32 cmd_exec_len = sizeof(struct sahara_packet_cmd_exec);
  cmd_exec = &sahara_pkt->pkt.cmd_exec;

  if (pkt_len != cmd_exec_len)
  {
    return SAHARA_NAK_INVALID_PACKET_SIZE;
  }

  if (cmd_exec->length != cmd_exec_len)
  {
    return SAHARA_NAK_INVALID_PACKET_SIZE;
  }

  if (cmd_exec->cli_cmd != SHARA_READ_SERIAL)
  {
    return SAHARA_NAK_EXEC_CMD_UNSUPPORTED;
  }

  sahara_pkt->cmd_id = SAHARA_CMD_EXEC_ID;
  sahara_pkt->pkt_len = cmd_exec_len;

  return SAHARA_NAK_SUCCESS;
}


static enum remotefs_sahara_status
remotefs_sahara_interpret_cmd_exe_data_pkt (
                                  struct remotefs_sahara_packet *sahara_pkt,
                                  uint32 pkt_len)
{
  struct sahara_packet_cmd_exec_data *cmd_data;
  uint32 cmd_data_len = sizeof(struct sahara_packet_cmd_exec_data);
  cmd_data = &sahara_pkt->pkt.cmd_data;

  if (pkt_len != cmd_data_len)
  {
    return SAHARA_NAK_INVALID_PACKET_SIZE;
  }

  if (cmd_data->length != cmd_data_len)
  {
    return SAHARA_NAK_INVALID_PACKET_SIZE;
  }

  if (cmd_data->cli_cmd != SHARA_READ_SERIAL)
  {
    return SAHARA_NAK_EXEC_CMD_UNSUPPORTED;
  }

  sahara_pkt->cmd_id = SAHARA_CMD_EXEC_DATA_ID;
  sahara_pkt->pkt_len = cmd_data_len;

  return SAHARA_NAK_SUCCESS;

}

/* Function assumes the caller knowns not to call into interpret when RAW data
   was expected. */
enum remotefs_sahara_status
remotefs_sahara_interpret_pkt (struct remotefs_sahara_packet *sahara_pkt,
                               uint32 pkt_len)
{
  enum remotefs_sahara_status status;
  struct remotefs_sahara_packet_header *hdr;

  hdr = (struct remotefs_sahara_packet_header *)&sahara_pkt->pkt;
  switch (hdr->command)
  {
    case SAHARA_HELLO_RESP_ID:
      status = remotefs_sahara_interpret_hello_resp_pkt (sahara_pkt, pkt_len);
      break;

    case SAHARA_DONE_ID:
      status = remotefs_sahara_interpret_done_pkt (sahara_pkt, pkt_len);
      break;

    case SAHARA_RESET_ID:
      status = remotefs_sahara_interpret_reset_pkt (sahara_pkt, pkt_len);
      break;

    case SAHARA_MEMORY_READ_ID:
      status = remotefs_sahara_interpret_mem_read_pkt (sahara_pkt, pkt_len);
      break;

    case SAHARA_CMD_SWITCH_MODE_ID:
      status = remotefs_sahara_interpret_switch_mode_pkt (sahara_pkt, pkt_len);
      break;

    case SAHARA_CMD_EXEC_ID:
      status = remotefs_sahara_interpret_cmd_exe_pkt (sahara_pkt, pkt_len);
      break;

    case SAHARA_CMD_EXEC_DATA_ID:
      status = remotefs_sahara_interpret_cmd_exe_data_pkt (sahara_pkt, pkt_len);
      break;

    default:
      status = SAHARA_NAK_INVALID_CMD;
      break;
  }

  return status;
}

void
remotefs_sahara_form_hello_pkt (struct remotefs_sahara_packet *sahara_pkt,
                                uint32 mode, uint32 seq_num)
{
  struct sahara_packet_hello* hello;

  memset (sahara_pkt, 0x0, sizeof (struct remotefs_sahara_packet));
  sahara_pkt->cmd_id = SAHARA_HELLO_ID;
  sahara_pkt->pkt_len = sizeof (struct sahara_packet_hello);

  hello = &sahara_pkt->pkt.hello;
  hello->command = sahara_pkt->cmd_id;
  hello->length = sahara_pkt->pkt_len;
  hello->version = SAHARA_VERSION_MAJOR;
  hello->version_supported = SAHARA_VERSION_MAJOR_SUPPORTED;
  hello->cmd_packet_length = sizeof (struct remotefs_sahara_packet);
  hello->mode = mode;
  hello->reserved0 = seq_num;
}

void
remotefs_sahara_form_mem_dbg_pkt (struct remotefs_sahara_packet *sahara_pkt,
                                  uint32 mem_tbl_addr, uint32 mem_tbl_size)
{
  struct sahara_packet_memory_debug *mem_dbg;
  mem_dbg = (struct sahara_packet_memory_debug*)&sahara_pkt->pkt;

  memset (sahara_pkt, 0x0, sizeof (struct remotefs_sahara_packet));
  sahara_pkt->cmd_id = SAHARA_MEMORY_DEBUG_ID;
  sahara_pkt->pkt_len = sizeof (struct sahara_packet_memory_debug);

  mem_dbg->command = sahara_pkt->cmd_id;
  mem_dbg->length = sahara_pkt->pkt_len;
  mem_dbg->memory_table_addr = mem_tbl_addr;
  mem_dbg->memory_table_length = mem_tbl_size;
}

void
remotefs_sahara_form_cmd_ready_pkt (struct remotefs_sahara_packet *sahara_pkt)
{
  struct sahara_packet_cmd_ready* cmd_ready;
  cmd_ready = (struct sahara_packet_cmd_ready*)&sahara_pkt->pkt;

  memset (sahara_pkt, 0x0, sizeof (struct remotefs_sahara_packet));
  sahara_pkt->cmd_id = SAHARA_CMD_READY_ID;
  sahara_pkt->pkt_len = sizeof (struct sahara_packet_cmd_ready);

  cmd_ready->command = sahara_pkt->cmd_id;
  cmd_ready->length = sahara_pkt->pkt_len;
}

void
remotefs_sahara_form_cmd_exec_resp_pkt (
                                    struct remotefs_sahara_packet *sahara_pkt,
                                    uint32 cmd_id, uint32 resp_len)
{
  struct sahara_packet_cmd_exec_resp* cmd_exec_resp;
  cmd_exec_resp = (struct sahara_packet_cmd_exec_resp*)&sahara_pkt->pkt;

  memset (sahara_pkt, 0x0, sizeof (struct remotefs_sahara_packet));
  sahara_pkt->cmd_id = SAHARA_CMD_EXEC_RESP_ID;
  sahara_pkt->pkt_len = sizeof (struct sahara_packet_cmd_exec_resp);

  cmd_exec_resp->command = sahara_pkt->cmd_id;
  cmd_exec_resp->length = sahara_pkt->pkt_len;
  cmd_exec_resp->cli_cmd = cmd_id;
  cmd_exec_resp->resp_len = resp_len;
}

void
remotefs_sahara_form_reset_resp_pkt (struct remotefs_sahara_packet *sahara_pkt)
{
  struct sahara_packet_reset_resp* reset_resp;
  reset_resp = (struct sahara_packet_reset_resp*)&sahara_pkt->pkt;

  memset (sahara_pkt, 0x0, sizeof (struct remotefs_sahara_packet));
  sahara_pkt->cmd_id = SAHARA_RESET_RESP_ID;
  sahara_pkt->pkt_len = sizeof (struct sahara_packet_reset_resp);

  reset_resp->command = sahara_pkt->cmd_id;
  reset_resp->length = sahara_pkt->pkt_len;
}

void remotefs_sahara_form_end_of_img_transfer_pkt (
                                    struct remotefs_sahara_packet *sahara_pkt,
                                    uint32 image_id, uint32 status)
{
  struct sahara_packet_end_of_image_tx* eot;
  eot = (struct sahara_packet_end_of_image_tx*)&sahara_pkt->pkt;

  memset (sahara_pkt, 0x0, sizeof (struct remotefs_sahara_packet));
  sahara_pkt->cmd_id = SAHARA_END_IMAGE_TX_ID;
  sahara_pkt->pkt_len = sizeof (struct sahara_packet_end_of_image_tx);

  eot->command = sahara_pkt->cmd_id;
  eot->length = sahara_pkt->pkt_len;
  eot->image_id = image_id;
  eot->status = status;
}
