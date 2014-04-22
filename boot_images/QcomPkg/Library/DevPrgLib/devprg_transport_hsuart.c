/**************************************************************************
 * FILE: devprg_transport_hsuart.c
 *
 * Packet layer which provides data integrity of transmitted data.
 *
 * Copyright (c) 2019, 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/DevPrgLib/devprg_transport_hsuart.c#2 $
  $DateTime: 2020/01/28 12:49:31 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2020-01-07   as    Add enhancements in hsuart transport layer
2019-04-30   as    Initial Revision
===========================================================================*/

#include "devprg_hsuart_pkt.h"
#include "devprg_packet_id.h"
#include "devprg_transport.h"
#include "devprg_hsuart.h"
#include "devprg_log.h"
#include "LoaderUtils.h"
#include "DALSys.h"
#include "string.h"
#include "comdef.h"

/* MAX_COBS_OVERHEAD = overhead of (packet id + CRC + max overhead possible with 
   COBS algorithm)*/
#define MAX_COBS_OVERHEAD 21

static uint32 rx_bytes;
static uint32 hsuart_rx_error = 0;
static uint32 hsuart_tx_error = 0;

static uint8* transport_packet_buffer = NULL;
static UINTN  transport_packet_buffer_size = 0;
static uint8* devprg_cobs_buffer = NULL;
static UINTN  devprg_cobs_buffer_size = -1;


/*The callback is called when UART sets the error code while reading data and
  this callback is called by device programmer as part of error handelling.*/
static void devprg_hsuart_rx_cb();
static boolean devprg_hsuart_pkt_verify_version();

static void devprg_hsuart_pkt_nak_tx()
{
  uint32 tx_length;

  uint8 nak_packet =  NAK_PACKET;
  *transport_packet_buffer = nak_packet;

  tx_length = sizeof(nak_packet);
  devprg_hsuart_write(transport_packet_buffer, sizeof(nak_packet));
}

static int devprg_hsuart_pkt_wait_for_ack()
{
  uint8 ack_packet = ACK_PACKET;
  int status;

  memset(transport_packet_buffer, 0, transport_packet_buffer_size);
  (void) devprg_hsuart_read(transport_packet_buffer, sizeof(ack_packet), TRUE,
                            devprg_hsuart_rx_cb);

  if (*transport_packet_buffer == ACK_PACKET)
  {
    status = 1;
  }
  else if (*transport_packet_buffer == NAK_PACKET)
  {
    status = 0;
  }
  else
  {
    status = -1;
  }
  return status;
}

static void devprg_hsuart_pkt_ack_tx()
{
  uint32 tx_length;
  uint8 ack_packet =  ACK_PACKET;
  *transport_packet_buffer = ack_packet;

  tx_length = sizeof(ack_packet);
  devprg_hsuart_write(transport_packet_buffer, sizeof(ack_packet));
}


static boolean devprg_hsuart_pkt_is_restart()
{
  boolean version_check = FALSE;
  devprg_hsuart_read_purge_buf(transport_packet_buffer, 
                               transport_packet_buffer_size);
  devprg_hsuart_pkt_nak_tx();
  version_check = devprg_hsuart_pkt_verify_version();
  return version_check;
}

static void devprg_hsuart_rx_cb()
{
  devprg_hsuart_pkt_nak_tx();
}

static int devprg_hsuart_pkt_rx_helper(uint8* read_buf, uint32 length,
                       enum devprg_packet_id_t* packet_id_recvd,
                       int is_restart)
{
  uint32 bytes_read, decoded_data_length;
  int result = -1;
  enum devprg_hsuart_packet_error_t error;

  while(1)
  {
    memset(devprg_cobs_buffer, 0, devprg_cobs_buffer_size);
    bytes_read = devprg_hsuart_read(devprg_cobs_buffer, length, FALSE,
                                    devprg_hsuart_rx_cb);
    decoded_data_length = devprg_hsuart_packet_decode(packet_id_recvd,
                  devprg_cobs_buffer, bytes_read, read_buf, length, &error);

    if(error == HSUART_PACKET_SUCCESS)
    {
      if((is_restart == 0) && (*packet_id_recvd == VERSION_PACKET))
      {
        break;
      }
      devprg_hsuart_pkt_ack_tx();
      result = decoded_data_length;
      break;
    }
    else
    {
      devprg_hsuart_pkt_nak_tx();
    }
  }
  return result;;
}

static int devprg_hsuart_pkt_tx_helper(uint8* src_buf, uint32 src_length,
                                     enum devprg_packet_id_t packet_id,
                                     int* is_restart)
{
  uint32 encoded_data_length = -1;
  int status = -1;
  int result = -1, tx_length = -1;
  enum devprg_hsuart_packet_error_t error;

  memset(devprg_cobs_buffer, 0, devprg_cobs_buffer_size);
  encoded_data_length  = devprg_hsuart_packet_encode(packet_id,
                              src_buf, src_length, devprg_cobs_buffer,
                              devprg_cobs_buffer_size, &error);

  if (error != HSUART_PACKET_SUCCESS)
  {
    result = -1;
    goto End;
  }

  while(result < 0)
  {
    tx_length = devprg_hsuart_write(devprg_cobs_buffer, encoded_data_length);
    status = devprg_hsuart_pkt_wait_for_ack();
    if(status == 1)
    {
      result = tx_length;
      break;
    }
    else if(status == 0)
    {
      continue;
    }
    else
    {
      result = tx_length;
      *is_restart = 1;
      break;
    }
  }

End:
  return result;
}

static boolean devprg_hsuart_pkt_verify_version()
{
  boolean version_check = FALSE;
  int tx_length, rx_length, is_restart;
  enum devprg_packet_id_t packet_id_recvd;

  while(version_check != TRUE)
  {
    /* 1. Recieve the version packet and sanity check it*/
    memset(transport_packet_buffer, 0, transport_packet_buffer_size);
    rx_length = devprg_hsuart_pkt_rx_helper((uint8*)transport_packet_buffer,
                          transport_packet_buffer_size, &packet_id_recvd, 1);
    if((rx_length < 0) || (packet_id_recvd != VERSION_PACKET))
    {
      version_check = FALSE;
      goto End;
    }

    /* 2.Form our version packet and send it */
    memset(transport_packet_buffer, 0, transport_packet_buffer_size);
    tx_length = devprg_hsuart_pkt_tx_helper(transport_packet_buffer,
                               transport_packet_buffer_size, VERSION_PACKET,
                               &is_restart);
    if(tx_length < 0)
    {
      version_check = FALSE;
      goto End;
    }
    version_check = TRUE;
  }

End:
  return version_check;
}

boolean devprg_hsuart_pkt_init()
{
  int result;

  devprg_cobs_buffer_size = devprg_hsuart_get_max_packet_size();
  transport_packet_buffer_size = devprg_cobs_buffer_size - MAX_COBS_OVERHEAD;

  result = DALSYS_Malloc(devprg_cobs_buffer_size, (void*)&devprg_cobs_buffer);
  if(DAL_SUCCESS != result)
  {
    return FALSE;
  }
  result = DALSYS_Malloc(transport_packet_buffer_size,
                        (void*)&transport_packet_buffer);
  if(DAL_SUCCESS != result)
  {
    return FALSE;
  }

  if(devprg_hsuart_initialize() != 0)
  {
    return FALSE;
  }

  devprg_hsuart_read_purge_buf(transport_packet_buffer, 
                               transport_packet_buffer_size);
  return devprg_hsuart_pkt_verify_version();
}

/* Note: This will be returning encoded length sent and not data length*/
int devprg_hsuart_pkt_tx(uint8* buf, uint32 length, uint32* err_code)
{
  uint32 remaining_data = length, tx_length;
  uint8* tx_buf = buf;
  enum devprg_packet_id_t packet_id;
  int is_restart = 0;
  int result;

  while(remaining_data > 0)
  {
    if(remaining_data <= transport_packet_buffer_size)
    {
      tx_length = remaining_data;
      packet_id  = END_OF_TRANSFER_PACKET;
    }
    else
    {
      tx_length = transport_packet_buffer_size;
      packet_id  = PROTOCOL_PACKET;
    }

    result = devprg_hsuart_pkt_tx_helper(tx_buf, tx_length, packet_id,
                                         &is_restart);
    if(result <= 0)
    {
      *err_code = result;
      goto End;
    }
    else
    {
      if(is_restart == 1)
      {
        if(devprg_hsuart_pkt_is_restart() != TRUE)
		{
	      result = -1;
		}
        *err_code = -1;
        goto End;
      }
      remaining_data -= tx_length;
      tx_buf += tx_length;
      *err_code = 0;
    }
  }

End:
  return result;
}

static int devprg_hsuart_pkt_rx(uint8* buf, uint32 length, uint32* err_code)
{
  uint32 rx_length_recvd;
  uint32 rx_length_recvd_total = 0;
  uint8* rx_buf = buf;
  enum devprg_packet_id_t packet_id_recvd = PROTOCOL_PACKET;
  int result = -1, write_result, is_restart = 0;

  memset(transport_packet_buffer, 0, transport_packet_buffer_size);
  write_result = devprg_hsuart_pkt_tx_helper(transport_packet_buffer,
                                             transport_packet_buffer_size,
                                             READY_TO_READ_PACKET,
                                             &is_restart);
  if(write_result < 0)
  {
    result = write_result;
    *err_code = write_result;
    goto End;
  }
  else if((write_result >= 0) && (is_restart == 1))
  {
    if(devprg_hsuart_pkt_is_restart() != TRUE)
    {
      result = -1;
    }
    *err_code = -1;
    goto End;
  }

  while(packet_id_recvd !=  END_OF_TRANSFER_PACKET)
  {
    rx_length_recvd = devprg_hsuart_pkt_rx_helper(rx_buf,
                                                  devprg_cobs_buffer_size,
                                                  &packet_id_recvd, 0);
    if(rx_length_recvd < 0)
    {
      *err_code = rx_length_recvd;
      goto End;
    }
    else if((rx_length_recvd >= 0) && (packet_id_recvd == VERSION_PACKET))
    {
      if(devprg_hsuart_pkt_is_restart() != TRUE)
      {
        result = -1;
      }
      *err_code = -1;
      goto End;
    }
    rx_length_recvd_total += rx_length_recvd;
    rx_buf += rx_length_recvd;
    *err_code = 0;
  }

End:
  return rx_length_recvd_total;
}

/* HSuart transport library ops */
dp_res_t devprg_transport_hsuart_set_mode(unsigned int key, unsigned int val)
{

  (void) key;
  (void) val;
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_transport_hsuart_rx_queue(void *data, size_t size)
{
  hsuart_rx_error = 0;
  rx_bytes = devprg_hsuart_pkt_rx(data, size, &hsuart_rx_error);
  if (hsuart_rx_error != 0)
  {
    DP_LOGE("UART RX bulk received failed %d", hsuart_rx_error);
    return DEVPRG_ERROR_RECEIVE;
  }
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_transport_hsuart_rx_queue_check(size_t *rx_num_bytes)
{
  if (hsuart_rx_error != 0)
  {
    return DEVPRG_ERROR_RECEIVE;
  }
  else
  {
    *rx_num_bytes = rx_bytes;
  }
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_transport_hsuart_tx_queue(void *data, size_t size)
{
  (void) devprg_hsuart_pkt_tx (data, size, &hsuart_tx_error);
  if(hsuart_tx_error != 0)
  {
    return DEVPRG_ERROR_TRANSMIT;
  }
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_transport_hsuart_tx_queue_check(size_t size)
{
  (void) size;
  if (hsuart_tx_error != 0)
  {
    return DEVPRG_ERROR_TRANSMIT;
  }
  return DEVPRG_SUCCESS;
}
  
int devprg_transport_hsuart_async_support(void)
{
  return DEVPRG_ERROR_NOT_SUPPORTED;
}
static devprg_transport_ops devprg_transport_hsuart_ops =
{
  devprg_transport_hsuart_set_mode,
  devprg_transport_hsuart_rx_queue,
  devprg_transport_hsuart_rx_queue_check,
  devprg_transport_hsuart_tx_queue,
  devprg_transport_hsuart_tx_queue_check,
  devprg_transport_hsuart_async_support,
};

static struct devprg_transport devprg_transport_hsuart =
{
  DEVPRG_TRANSPORT_UART,
  &devprg_transport_hsuart_ops
};

struct devprg_transport *devprg_transport_hsuart_init(void)
{
  (void) devprg_hsuart_pkt_init();
  return &devprg_transport_hsuart;
}