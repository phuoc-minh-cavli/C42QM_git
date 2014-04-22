/*=============================================================================

FILE:         qdss_sio.c

DESCRIPTION:  This file has functions that interace with the USB stack
              sio API is the interface that USB stack exposes

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "qdss_sio.h"
#include "qdss_control_priv.h"
#include "qdss_control.h"

/*-------------------------------------------------------------------------*/

/**
  @brief Callback for handling DTR changes

  This function is called by the USB stack when USB cable is
  connected/disconnected

  @return None
 */
static void qdss_sio_dtr_event_handler()
{
   int nErr;

   sio_ioctl_param_type ioctl_param;
   boolean dte_ready_asserted = FALSE;
   ioctl_param.dte_ready_asserted = &dte_ready_asserted;

   if (qdss.sio.handle == SIO_NO_STREAM_ID) {
      //DTR event on a closed stream
      return;
   }

   sio_ioctl(qdss.sio.handle,SIO_IOCTL_DTE_READY_ASSERTED, &ioctl_param);

   if (dte_ready_asserted) {
      TRY(nErr, qdss_trace_sink_enable());
   }
   else {
      TRY(nErr, qdss_trace_sink_disable());
   }
   CATCH(nErr);

   IGNORE(nErr);
}

/*-------------------------------------------------------------------------*/

/**
  @brief This function sets the SIO parameters for bam mode

  In BAM mode after bam pipes are set up there is no more
  device software interaction for sending data over USB

  @return None
 */
static void qdss_sio_set_params_bam_mode(void)
{
   qdss.sio.open_props.port_id = SIO_PORT_USB_QDSS;
   qdss.sio.open_props.stream_mode = SIO_GENERIC_MODE;

   qdss.sio.open_param.dsm_pool_param.dsm_mempool_id = DSM_DS_SMALL_ITEM_POOL;
   qdss.sio.open_param.dsm_pool_param.usb_mode = SIO_USB_MODE_SPS_USB_BAM;
   qdss.sio.open_props.open_param = &qdss.sio.open_param;


   //we don't use this, but sio_open needs this
   dsm_queue_init (&qdss.sio.tx_wm_q,0,&qdss.sio.tx_q);
   qdss.sio.open_props.tx_queue = &qdss.sio.tx_wm_q;
}

/*-------------------------------------------------------------------------*/

/**
  @brief Null rx function. PCIe sio interface requires one.
  
  @param                  ppd [in ] :  Not used
 */
void qdss_sio_rx(dsm_item_type **ppd)
{
   IGNORE(ppd);
   return;
}

/*-------------------------------------------------------------------------*/

/**
  @brief This function sets the SIO parameters for SW FIFO mode

  In BUFFERED mode the device software is involved in sending data
  collected in DDR buffers

  @return None
 */
static void qdss_sio_set_params_buffered_mode(sio_port_id_type port_id)
{
   qdss.sio.open_props.port_id = port_id;
   qdss.sio.open_props.stream_mode = SIO_DS_AUTODETECT_MODE;

   qdss.sio.open_param.dsm_pool_param.dsm_mempool_id = DSM_DS_SMALL_ITEM_POOL;
   qdss.sio.open_param.dsm_pool_param.usb_mode = SIO_USB_MODE_LEGACY;
   qdss.sio.open_props.open_param = &qdss.sio.open_param;


   dsm_queue_init
      (&qdss.sio.tx_wm_q,  /* watermark queue */
       DSM_DS_SMALL_ITEM_CNT * DSM_DS_SMALL_ITEM_SIZ, /*dont_exceed_cnt */
       &qdss.sio.tx_q); /* queue */

   dsm_set_low_wm
      (&qdss.sio.tx_wm_q,
       DSM_DS_SMALL_ITEM_CNT * DSM_DS_SMALL_ITEM_SIZ);

   dsm_set_hi_wm
      (&qdss.sio.tx_wm_q,
       DSM_DS_SMALL_ITEM_CNT * DSM_DS_SMALL_ITEM_SIZ);

   qdss.sio.open_props.tx_queue = &qdss.sio.tx_wm_q;

   qdss.sio.open_props.rx_func_ptr = qdss_sio_rx;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Opens the USB QDSS device

  @param                 mode [in ] :  Indicates the desired mode to open
                                       the device in - bam or buffered.

  @return 0 if successful, error code otherwise
 */

int qdss_sio_open(sio_port_id_type port_id, uint8 mode)
{
   int nErr = QDSS_CONTROL_ERROR;
   sio_ioctl_param_type ioctl_param;


   if (qdss.sio.handle != SIO_NO_STREAM_ID) {
      //For now SIO port once opened will remain open
      //Just check the DTR to enable trace
      qdss_sio_dtr_event_handler();
      return QDSS_CONTROL_SUCCESS;
   }

   memset(&qdss.sio.open_props,0,sizeof(sio_open_type));
   memset(&qdss.sio.open_param,0,sizeof(sio_open_param_type));

   if (TMC_TRACESINK_USB==mode) {
      qdss_sio_set_params_bam_mode();
   }
   else if (TMC_TRACESINK_USB_BUFFERED == mode) {
      qdss_sio_set_params_buffered_mode(port_id);
   }
   else {
      THROW(nErr,QDSS_CONTROL_ERROR);
   }

   qdss.sio.handle = sio_open (&qdss.sio.open_props);
   if (qdss.sio.handle == SIO_NO_STREAM_ID) {
      THROW(nErr,QDSS_CONTROL_ERROR);
   }


   ioctl_param.enable_dte_ready_event = qdss_sio_dtr_event_handler;
   sio_ioctl(qdss.sio.handle,SIO_IOCTL_ENABLE_DTR_EVENT,&ioctl_param);

   nErr = QDSS_CONTROL_SUCCESS;
   CATCH(nErr) {}
   return nErr;

}

/*-------------------------------------------------------------------------*/

/**
  @brief  sio_close call back function

  @return None
 */
void qdss_sio_close_cb(void)
{
   qdss.sio.handle = SIO_NO_STREAM_ID;
}

/*-------------------------------------------------------------------------*/

/**
  @brief Closes the USB QDSS device

  @return 0 if successful, error code otherwise
 */
int qdss_sio_close(void)
{
   int nErr = QDSS_CONTROL_ERROR;
   sio_ioctl_param_type ioctl_param;

   if (SIO_NO_STREAM_ID == qdss.sio.handle) {
      THROW(nErr,QDSS_CONTROL_BAD_STATE);
   }

   sio_ioctl(qdss.sio.handle,SIO_IOCTL_DISABLE_DTR_EVENT_EXT,&ioctl_param);
   sio_close(qdss.sio.handle,qdss_sio_close_cb);

   nErr = QDSS_CONTROL_SUCCESS;

   CATCH(nErr) {}

   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief Sends a continuous block of data over USB

  @param                 addr [in ] :  Start physical address of data block
  @param                 size [in ] :  Size of data block in bytes

  @return 0 if successful, error code otherwise
 */
int qdss_sio_send_chunk(uint32 addr, uint32 size)
{
   int nErr = 0;
   dsm_item_type *dsm_buf = NULL;

   dsm_buf = dsm_new_buffer (DSM_DS_SMALL_ITEM_POOL);

   if (NULL== dsm_buf) {
      qdss.etr.no_dsm_bufs++;
      THROW(nErr, -1);
   }


   dsm_buf->app_ptr = (void *)addr;
   dsm_buf->app_field = size;
   sio_transmit (qdss.sio.handle, dsm_buf);

   qdss.etr.chunks_sent++;

   nErr = 0;

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Sends a sepecified amount of data

  This function maintians a pointer to the ETR circular buffer to keep track of
  the last point until which we sent.

  @param            data_size [in ] : Size of dta to be transmitted

  @return 0 if successful, error code otherwise
 */
int qdss_sio_send_data(uint32 data_size)
{
   int nErr = 0;

   if ((qdss.etr.sio_tx_ptr + data_size)  <= qdss.etr.buf_end_addr) {
      TRY(nErr,qdss_sio_send_chunk(qdss.etr.sio_tx_ptr, data_size));

      qdss.etr.sio_tx_ptr += data_size;
      if (qdss.etr.sio_tx_ptr == qdss.etr.buf_end_addr) {
         qdss.etr.sio_tx_ptr = qdss.etr.buf_start_addr;
      }
   }
   else {
      /*buffer wrapped around */
      uint32 chunk1_size = qdss.etr.buf_end_addr - qdss.etr.sio_tx_ptr;
      uint32 chunk2_size = data_size - chunk1_size;

      TRY(nErr,qdss_sio_send_chunk(qdss.etr.sio_tx_ptr, chunk1_size));
      TRY(nErr,qdss_sio_send_chunk(qdss.etr.buf_start_addr, chunk2_size));

      qdss.etr.sio_tx_ptr = qdss.etr.buf_start_addr+chunk2_size;
   }


   CATCH(nErr) {}
   return nErr;
}



