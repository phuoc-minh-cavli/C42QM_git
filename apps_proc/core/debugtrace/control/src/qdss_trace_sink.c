/*=============================================================================

FILE:         qdss_trace_sink.c

DESCRIPTION:  Implementation for the trace sink switch command

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/

#include "qdss_control.h"
#include "qdss_control_priv.h"
#include "qdss_etr.h"
#include "qdss_bam_config.h"
#include "qdss_sio.h"
#include "qdss_tlmm.h"
#include "qdss_osal.h"

/*-------------------------------------------------------------------------*/

/**
  @brief Re-enable trace after a trace sink switch.

  @return 0 if successful, error code otherwise
 */
int qdss_trace_sink_enable(void)
{
   int nErr = 0;
   uint8 stm_enabled = 0;

   qdss.bTraceSinkEnabled = TRUE;
   TRY(nErr,  DalTMC_EnableTrace(qdss.hTMC));

   TRY(nErr, qdss_control_get_stm(&stm_enabled));

   if (stm_enabled) {
      //re-enable STM so the trace sink receives a
      //sync channel packets
      TRY(nErr,DalSTMCfg_STMControl(qdss.hSTMCfg,STM_EN,0));
      TRY(nErr,DalSTMCfg_STMControl(qdss.hSTMCfg,STM_EN,1));
   }

   if (qdss_trace_sink_is_buffered(qdss.current_trace_sink))  {
      TRY(nErr,qdss_etr_start_timer());
   }

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief Disable  trace before a trace sink switch

  @return 0 if successful, error code otherwise
 */
int qdss_trace_sink_disable(void)
{
   int nErr = 0;

   if (qdss_trace_sink_is_buffered(qdss.current_trace_sink))  {
      TRY(nErr,qdss_etr_stop_timer());
   }

   qdss.bTraceSinkEnabled = FALSE;
   TRY(nErr,DalTMC_DisableTrace(qdss.hTMC));

   CATCH(nErr) {}
   return nErr;
}


/*-------------------------------------------------------------------------*/

/**
  @brief Switch to ETB and start trace

  @return 0 if successful, error code otherwise
 */
static int qdss_trace_etb_start(void)
{
   int nErr;

   TRY(nErr,DalTMC_SetMode(qdss.hTMC,TMC_TRACESINK_ETB));
   TRY(nErr,qdss_trace_sink_enable());

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief Stop tracing to ETB.

  @return 0 if successful, error code otherwise
 */
static int qdss_trace_etb_stop(void)
{
   int nErr;
   TRY(nErr,qdss_trace_sink_disable());

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief Switch trace sink to DDR and start tracing

  @return 0 if successful, error code otherwise
 */
static int qdss_trace_ddr_start(void)
{
   int nErr;

   TRY(nErr,DalTMC_SetMode(qdss.hTMC,TMC_TRACESINK_RAM));
   TRY(nErr,qdss_etr_configure_buffer(qdss.buf.etr_phys_base,
                                      qdss.buf.etr_buf_size));
   TRY(nErr,qdss_trace_sink_enable());

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Stop tracing to DDR

  @return 0 if successful, error code otherwise
 */
static int qdss_trace_ddr_stop(void)
{
   int nErr;
   TRY(nErr,qdss_trace_sink_disable());

   CATCH(nErr) {}
   return nErr;
}


/*-------------------------------------------------------------------------*/

/**
  @brief  TPIU trace start

  @return 0 if successful, error code otherwise
 */
static int qdss_trace_tpiu_start(void)
{
   int nErr;

   TRY(nErr,DalTMC_SetMode(qdss.hTMC,TMC_TRACESINK_TPIU));
   TRY(nErr,qdss_trace_sink_enable());

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief TPIU trace stop

  @return 0 if successful, error code otherwise
 */
static int qdss_trace_tpiu_stop(void)
{
   int nErr;
   TRY(nErr,qdss_trace_sink_disable());

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief USB bam mode start

  @return 0 if successful, error code otherwise
 */
static int qdss_trace_usb_bam_mode_start(void)
{
   int nErr;

   TRY(nErr,qdss_sio_open(SIO_PORT_USB_QDSS, TMC_TRACESINK_USB));

   CATCH(nErr) {}
   return nErr;
}

int qdss_per_request_pipe(bam_pipe_config_type *qdss_pipe_config_ptr,
                          bam_handle *qdss_pipe_handle_ptr,
                          uint32 *qdss_pipe_num_ptr)
{
   int nErr;

   if ((NULL== qdss_pipe_config_ptr)||
       (NULL==qdss_pipe_handle_ptr)||
       (NULL==qdss_pipe_num_ptr)) {
      return BAM_FAILED;
   }


   TRY(nErr,qdss_bam_init());
   TRY(nErr,qdss_bam_pipe_setup(qdss_pipe_config_ptr,
                                qdss_pipe_handle_ptr,
                                qdss_pipe_num_ptr));

   TRY(nErr,DalTMC_SetMode(qdss.hTMC,TMC_TRACESINK_USB));
   TRY(nErr,qdss_etr_configure_buffer(qdss_pipe_config_ptr->data_base_pa,
                                      qdss_pipe_config_ptr->data_size));


   CATCH(nErr) {}

   return nErr;
}


/*-------------------------------------------------------------------------*/

/**
  @brief USB bam mode stop

  @return 0 if successful, error code otherwise
 */
static int qdss_trace_usb_bam_mode_stop(void)
{

  int nErr;

  TRY(nErr,qdss_trace_sink_disable());
  //TBD sio_close

  CATCH(nErr) {}
  return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  USB buffered mode (buffered mode) start

  @return 0 if successful, error code otherwise
 */

static int qdss_trace_buffered_mode_start(sio_port_id_type port_id)
{
   int nErr;
   
   if (qdss.etr.byte_counter > qdss.buf.etr_buf_size) {
      //Make it a ping pong buffer, use half the size of the buffer
      qdss.etr.byte_counter = (qdss.buf.etr_buf_size >> 1);
   }

   TRY(nErr,DalTMC_SetParam(qdss.hTMC,TMC_ETR_BYTE_COUNTER,
                            qdss.etr.byte_counter));

   TRY(nErr,DalTMC_SetMode(qdss.hTMC,TMC_TRACESINK_USB_BUFFERED));

   TRY(nErr,qdss_etr_configure_buffer(qdss.buf.etr_phys_base,
                                      qdss.buf.etr_buf_size));

   // TRY(nErr,qdss_etr_init_workloop());

   // TRY(nErr,qdss_etr_register_isr());

   TRY(nErr,qdss_sio_open(port_id,TMC_TRACESINK_USB_BUFFERED));

   /*Not expecting a DTR event for PCIe. So enable it right away */
   /*if (SIO_PORT_MHI_QDSS==port_id) {
      TRY(nErr,qdss_trace_sink_enable());
   } */

   CATCH(nErr) {}

   return nErr;

}

/*-------------------------------------------------------------------------*/

/**
  @brief USB buffered mode stop

  @return 0 if successful, error code otherwise
 */
int qdss_trace_buffered_mode_stop(void)
{
   int nErr;

   TRY(nErr,qdss_trace_sink_disable());
   qdss_sio_close();

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  SD trace (6-pin TPIU) start

  @return 0 if successful, error code otherwise
 */

static int qdss_trace_sd_start(void)
{
   int nErr;

   TRY(nErr,DalTMC_SetMode(qdss.hTMC,TMC_TRACESINK_SD));
   TRY(nErr,qdss_tlmm_sd_enable());
   TRY(nErr,qdss_trace_sink_enable());

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief SD trace (6-pin TPIU) stop

  @return 0 if successful, error code otherwise
 */
static int qdss_trace_sd_stop(void)
{
   int nErr;

   TRY(nErr,qdss_tlmm_sd_disable());
   TRY(nErr,qdss_trace_sink_disable());

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Start tracing to a new trace sink

  @param           trace_sink [in ] :   trace sink to start

  @return 0 if successful, error code otherwise
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_PARAM = Invalid parameter.
      QDSS_CONTROL_ERROR = Failed.
 */
static int qdss_trace_sink_start(uint8 trace_sink)
{
   int nErr = QDSS_CONTROL_SUCCESS;

   switch(trace_sink) {
   case TMC_TRACESINK_ETB:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_etb_start(),
               QDSS_CONTROL_ERROR);
      break;

   case TMC_TRACESINK_RAM:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_ddr_start(),
               QDSS_CONTROL_ERROR);

      break;

   case TMC_TRACESINK_TPIU:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_tpiu_start(),
               QDSS_CONTROL_ERROR);
      break;

   case TMC_TRACESINK_USB:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_usb_bam_mode_start(),
               QDSS_CONTROL_ERROR);
      break;

   case TMC_TRACESINK_USB_BUFFERED:
      THROW_IF(nErr,DAL_SUCCESS !=
               qdss_trace_buffered_mode_start(SIO_PORT_USB_QDSS),
               QDSS_CONTROL_ERROR);
      break;

   /*case TRACESINK_PCIE:
      THROW_IF(nErr,DAL_SUCCESS !=
               qdss_trace_buffered_mode_start(SIO_PORT_MHI_QDSS),
               QDSS_CONTROL_ERROR);
      break;*/


   case TMC_TRACESINK_SD:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_sd_start(),
               QDSS_CONTROL_ERROR);
      break;

   default:
      THROW(nErr,QDSS_CONTROL_UNSUPPORTED);
   }

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Stop tracing to a given trace sink

  @param           trace_sink [in ] :  trace sink to stop

  @return 0 if successful, error code otherwise
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_PARAM = Invalid parameter.
      QDSS_CONTROL_ERROR = Failed.
 */
static int qdss_trace_sink_stop(uint8 trace_sink)
{
   int nErr = QDSS_CONTROL_SUCCESS;

   switch(trace_sink) {
   case TMC_TRACESINK_ETB:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_etb_stop(),
               QDSS_CONTROL_ERROR);
      break;

   case TMC_TRACESINK_RAM:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_ddr_stop(),
               QDSS_CONTROL_ERROR);
      break;

   case TMC_TRACESINK_TPIU:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_tpiu_stop(),
               QDSS_CONTROL_ERROR);
      break;

   case TMC_TRACESINK_USB:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_usb_bam_mode_stop(),
               QDSS_CONTROL_ERROR);
      break;

   case TMC_TRACESINK_USB_BUFFERED:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_buffered_mode_stop(),
               QDSS_CONTROL_ERROR);

      break;

   /*case TRACESINK_PCIE:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_buffered_mode_stop(),
               QDSS_CONTROL_ERROR);
      break;*/

   case TMC_TRACESINK_SD:
      THROW_IF(nErr,DAL_SUCCESS != qdss_trace_sd_stop(),
               QDSS_CONTROL_ERROR);
      break;

   default:
      THROW(nErr,QDSS_CONTROL_UNSUPPORTED);
   }

   CATCH(nErr) {}
   return nErr;
}


boolean qdss_trace_sink_is_buffered(uint8 trace_sink)
{
   return ((TRACESINK_PCIE == trace_sink)||
           (TMC_TRACESINK_USB_BUFFERED == trace_sink));
}


static __inline boolean qdss_trace_sink_is_usb(uint8 trace_sink)
{
   return ((TMC_TRACESINK_USB == trace_sink)||
           qdss_trace_sink_is_buffered(trace_sink));
}



/*-------------------------------------------------------------------------*/

/**
  @brief  Trace sink switch diag command handler

  @param                 pReq [in ] :  request packet
  @param              req_len [in ] :  request packet length
  @param                 pRsp [in ] :  response packet
  @param              rsp_len [in ] :  response packet length

  @return 0 if successful, error code otherwise
 */
int qdss_trace_sink_handler(qdss_trace_sink_req  *pReq,
                            int req_len,
                            qdss_trace_sink_rsp *pRsp,
                            int rsp_len)
{
   int nErr;

   TRY(nErr,qdss_control_set_sink(pReq->trace_sink));

   CATCH(nErr) {}
   pRsp->result = nErr;
   return nErr;
}

/* Below are qdss control interface functions. Definitions of these
 * are in qdss_contro.h
 */

/*-------------------------------------------------------------------------*/

int qdss_control_get_sink(uint8 *sinkid)
{
   int nErr;

   if (FALSE==qdss.ctrlMutex_enable) {
      nErr=QDSS_CONTROL_BAD_STATE;
      return nErr;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);
   TRY(nErr,qdss_ensure_hw_ready());

   TRY(nErr,DalTMC_GetMode(qdss.hTMC,sinkid));
   if (TMC_TRACESINK_USB_BUFFERED==(*sinkid)) {
      *sinkid=qdss.current_trace_sink;
   }

   CATCH(nErr) {}

   qdss_osal_mutex_unlock(&qdss.ctrlMutex);
   return nErr;
}

/*-------------------------------------------------------------------------*/
/*
   Return:
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_FUSE_BLOWN = QDSS fuse is blown.
      QDSS_CONTROL_BAD_PARAM = Parameter invalid.
      QDSS_CONTROL_ERROR = Failed.
*/
int qdss_control_set_sink(uint8 sinkid)
{
   int nErr;
   uint8 new_trace_sink= (uint8) sinkid;
   uint8 current_trace_sink =  qdss.current_trace_sink;

   if (FALSE==qdss.ctrlMutex_enable) {
      nErr=QDSS_CONTROL_BAD_STATE;
      return nErr;
   }

   qdss_osal_mutex_lock(&qdss.ctrlMutex);

   TRY(nErr,qdss_ensure_hw_ready());

   current_trace_sink =  qdss.current_trace_sink;

   if ( (new_trace_sink == qdss.current_trace_sink) &&
        qdss_trace_sink_is_usb(qdss.current_trace_sink)) {
      //do nothing
   }
   else {
      TRY(nErr,qdss_trace_sink_stop(qdss.current_trace_sink));
      qdss.current_trace_sink = new_trace_sink;
      TRY(nErr,qdss_trace_sink_start(new_trace_sink));
   }

   nErr = QDSS_CONTROL_SUCCESS;

   CATCH(nErr){
      //had an error - keep the current
      qdss.current_trace_sink = current_trace_sink;
   }

   qdss_osal_mutex_unlock(&qdss.ctrlMutex);

   return nErr;
}


/*-------------------------------------------------------------------------*/
/*
   Return:
      QDSS_CONTROL_SUCCESS = Successful.
*/
int qdss_control_get_sink_list(uint8 *sinkid_list, int list_size, int *num_sinks)
{
#define QDSS_NUM_TRACE_SINKS 7

   if (0!=num_sinks) {
      *num_sinks = QDSS_NUM_TRACE_SINKS;
   }

   if (list_size < QDSS_NUM_TRACE_SINKS) {
      return 1; //error
   }

   sinkid_list[0]=TMC_TRACESINK_ETB;
   sinkid_list[1]=TMC_TRACESINK_RAM;
   sinkid_list[2]=TMC_TRACESINK_TPIU;
   sinkid_list[3]=TMC_TRACESINK_USB;
   sinkid_list[4]=TMC_TRACESINK_USB_BUFFERED;
   sinkid_list[5]=TMC_TRACESINK_SD;
   sinkid_list[6]=TRACESINK_PCIE;

   return QDSS_CONTROL_SUCCESS; //success
}
