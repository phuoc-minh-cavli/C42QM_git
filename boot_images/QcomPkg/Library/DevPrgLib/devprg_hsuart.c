/**************************************************************************
 * FILE: devprg_hsuart.c
 *
 * This file talks directly to hsuart driver APIs
 *
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/DevPrgLib/devprg_hsuart.c#2 $
  $DateTime: 2020/01/28 12:49:31 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2020-01-07   as    Add enhancements in hsuart transport layer
2019-05-24   as    Initial Creation.
===========================================================================*/

#include "DALSys.h"
#include "busywait.h"
#include "HSUart.h"
#include "LoaderUtils.h"

// Define the cable detection loop count
#define UART_PKT_POLL_MAX_LOOP_COUNT 20
// Define the cable detection pause time in us
#define UART_PKT_POLL_PAUSE_TIME 10
#define UART_BAUD_RATE 3000000
#define END_OF_PACKET 0x00

/* HSuart ops */
#define UART_DEBUG_ENABLE
#ifdef UART_DEBUG_ENABLE
struct read_debug_t
{
  int read_req_debug_count[10];
  int read_debug_count[10];
};
struct write_debug_t
{
  uint8 write_buf[256];
  int  length;
};

struct write_debug_t write_debug;
struct read_debug_t read_debug ;
int read_debug_count_index = 0;
int non_overrun_err = 0;
int last_hsuart_write = 0;
#endif

uint32 devprg_hsuart_get_max_packet_size(void)
{
  return hsuart_get_max_packet_size();
}

boolean devprg_hsuart_poll(void)
{
  uint32 poll_count = 0;
  boolean poll_state = FALSE;
  do
  {
    poll_state = hsuart_poll();
    if (poll_state == TRUE)
    {
      break;
    }
    busywait (UART_PKT_POLL_PAUSE_TIME);
    poll_count++;
  }while((poll_count) < UART_PKT_POLL_MAX_LOOP_COUNT);

  return (poll_state); 
}

uint32 devprg_hsuart_initialize(void)
{
  HSUART_CONFIG *cf = NULL;
  uint32 result = -1;
  if(DAL_SUCCESS != DALSYS_Malloc(sizeof(HSUART_CONFIG), (void *)&cf))
  {
    result = -1;
  }
  else
  {
    cf->enable_loopback = FALSE;
    cf->baud_rate = UART_BAUD_RATE;
    cf->enable_flow_control = TRUE;
    if(hsuart_initialize(cf) == 0)
    {
      result = 0;
    }
  }
  return result;
}

uint32 devprg_hsuart_write(UINT8* write_buf, uint32 length)
{
#ifdef UART_DEBUG_ENABLE
  last_hsuart_write = length;
#endif
  return hsuart_write(write_buf, length);
}

void devprg_hsuart_close(void)
{
  hsuart_close();
}

void devprg_hsuart_read_purge_buf(uint8* read_buf, uint32 length)
{
  uint32 err_code, bytes_poll;
  do
  {
    busywait (UART_PKT_POLL_PAUSE_TIME);
    bytes_poll = hsuart_read(read_buf, length, &err_code);
  }while(bytes_poll != 0);
  memset(read_buf, 0, length);
}

uint32 devprg_hsuart_read(uint8* read_buf, uint32 length, boolean fixed_length,
                          void (*devprg_hsuart_rx_cb)(void))
{
  int bytes_read = 0, bytes_poll;
  uint32 err_code = 0;
  read_debug_count_index = 0;
  memset(&read_debug.read_req_debug_count[0], 0, 10*sizeof(int));
  memset(&read_debug.read_debug_count[0], 0, 10*sizeof(int));
  uint8* temp_buf_ptr = read_buf;
  /* Poll for hardware events*/
  while(devprg_hsuart_poll() == FALSE);
  do
  {
    bytes_poll = 0;
    err_code = 0;
    bytes_poll = hsuart_read(read_buf, length, &err_code);
  #ifdef UART_DEBUG_ENABLE
   if(bytes_poll != 0)
   {
	read_debug.read_req_debug_count[read_debug_count_index] = length-bytes_read;
    read_debug.read_debug_count[read_debug_count_index] = bytes_poll;
    read_debug_count_index++;
    if(read_debug_count_index == 10)
      read_debug_count_index=0;
   }
  #endif
    read_buf += bytes_poll;
    bytes_read += bytes_poll;
    if(err_code != 0)
    {
      /* Clear out the uart buffer of any data*/
	  #ifdef UART_DEBUG_ENABLE
        if(err_code != HSUART_OVERRUN_ERROR)
  	    {
	      non_overrun_err++;
        }
	  #endif
      read_buf = temp_buf_ptr;
      /* The data in rx buffer needs to be purged and the cb will be called to
         send a NAK to PC */
      devprg_hsuart_read_purge_buf(read_buf, length);
      devprg_hsuart_rx_cb();
      bytes_read = 0;
      bytes_poll = 0;
      err_code = 0;
      continue;
    }
    if(fixed_length && (bytes_read == length))
    {
      return bytes_read;
    }
  }while((bytes_read>0) && (temp_buf_ptr[bytes_read-1] != END_OF_PACKET));
  return bytes_read;
}