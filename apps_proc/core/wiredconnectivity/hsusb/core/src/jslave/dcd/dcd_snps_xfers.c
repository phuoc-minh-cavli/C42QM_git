/*
===========================================================================

FILE:         dcd_snps_cmd_xfers.c

DESCRIPTION: Device Controller Driver for SNPS USB3.0 core. 

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/src/jslave/dcd/dcd_snps_xfers.c#2 $

===========================================================================

===========================================================================
Copyright © 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary
===========================================================================
*/

/*==============================================================================
Include files
=============================================================================*/

#include "dcd_snps.h"
#include "hsu_common_int.h"
#include <jusb_common_int.h>
#include "jos_int.h"
#include "hsu_hwio_dependency.h"
#include "hsu_log.h"
#include "hsu_platform.h"

/*==============================================================================
Internal Function Definitions
==============================================================================*/
static void snps_dcd_prepare_trb_for_infinite_transfer_mode(const snps_dcd_ep_desc_t* ep);

/*============================== Transfer utils ==============================*/

/**
* Function name:  snps_dcd_enqueue_zlp_trb
* Description: Adds a zero length packet TRB to the list of existing TRBs
* Parameters:
*     @ep: (IN) The EP descriptor
*
* Return value: None.
* Scope: local
**/
static void snps_dcd_enqueue_zlp_trb(const snps_dcd_ep_desc_t* ep)
{
  juint16_t stream_id = SNPS_DCD_DEFAULT_STREAM;
  snps_dcd_trb* trb_ptr;
  snps_dcd_trb* link_trb;

  HSU_CHK_EP_STREAM_ID(ep, stream_id);
  
  /* Prepare a single TRB */
  trb_ptr = ep->streams[stream_id].trb_soft_ptr;

  /* Continue only if this TRB in in SW control */
  if (trb_ptr->WORD3.HWO)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, run_out_of_TRB_resources");
    HSU_ERR_FATAL("The EP # should not be busy already", ep, 0, 0);
  }

  /* Set up TRB paramteres */
  trb_ptr->WORD0.BPTRL = NULL;
  trb_ptr->WORD1.BPTRH = NULL;
  trb_ptr->WORD2.BUFSIZE = 0x0;
  trb_ptr->WORD2.PCM1 = 0;
  trb_ptr->WORD2.reserved = 0;
  trb_ptr->WORD2.TRBSTS = 0;
  trb_ptr->WORD3.LST = 1;         // LST bit must be set for ZL TRB
  trb_ptr->WORD3.CSP = 0;

  /* Set TRB type according to EP type and EP0 control state machine */
  if (ep->soft_addr == 0)
  {
    trb_ptr->WORD3.TRBCTL = ep->sc->ep0_trb_type;
  }
  else
  {
    trb_ptr->WORD3.TRBCTL = (PIPE_ISOC == HSU_GET_PIPE_TYPE(ep)) ?
                            TRB_CONTROL_ISOC_FIRST : TRB_CONTROL_NORMAL;
  }

  trb_ptr->WORD3.IOC = 0;
  trb_ptr->WORD3.CHN = 0;
  trb_ptr->WORD3.ISP = 1;
  trb_ptr->WORD3.reserved1 = 0;
  trb_ptr->WORD3.StrmID_SOFNum = stream_id;
  trb_ptr->WORD3.reserved2 = 0;

  if ((ep->is_in) && (PIPE_BULK == HSU_GET_PIPE_TYPE(ep)))
  {
    // for updatetransfer
    trb_ptr->WORD3.LST = 0;
    trb_ptr->WORD3.IOC = 0;
    trb_ptr->WORD3.ISP = 0;
  }

  // Queue the TRB into the HW
  trb_ptr->WORD3.HWO = 1;

  link_trb = ep->streams[stream_id].soft_trb_array.virt + ((ep->max_trbs) - 1);

  // Update trb_soft_ptr to point to next available TRB in list
  HSU_INC_SW_TRB_PTR(ep->streams[stream_id].trb_soft_ptr, ep, stream_id, link_trb, ep->streams[stream_id].soft_trb_array.virt);

  HSU_ULOG_1(NO_MSG, SNPS_LOG, "EP 0x%x", ep->hw_addr);
}

/**
* Function name:  snps_dcd_update_trb_list_for_request
* Description: Updates the TRB list for a transfer request
* Parameters:
*     @ep: (IN) The EP descriptor
*     @request: (IN) The request
*
* Return value: 0 on success, otherwise an error code
* Scope: local
**/
static jresult_t snps_dcd_update_trb_list_for_request(const snps_dcd_ep_desc_t* ep, 
                                                      request_t* request,
                                                      jbool_t last_request,
                                                      jbool_t first_request)
{
  jresult_t rc = HSU_JSUCCESS;
  boolean enqueue_zlp_trb_flag = FALSE;
  /* TODO: USB3.0 streams support: Take stream_id from request. */
  juint16_t stream_id = SNPS_DCD_DEFAULT_STREAM;
  snps_dcd_trb* link_trb;

  HSU_CHK_EP_STREAM_ID(ep, stream_id);

  /* TODO: Handle the case where multiple TRBs are required for a single request - larger than 16M. */
  /*       The following code assumes single TRB per request                                        */
  {
    /* Prepare a single TRB */
    snps_dcd_trb* trb_ptr = ep->streams[stream_id].trb_soft_ptr;
#ifdef FEATURE_HS_USB_BAM    
    if (request->sps_mode)
    {
      snps_dcd_prepare_trb_for_infinite_transfer_mode(ep);
    }
    else
#endif /* FEATURE_HS_USB_BAM */
    {

      /* Continue only if this TRB in in SW control */
      if (trb_ptr->WORD3.HWO)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, run_out_of_TRB_resources");
        if (!ep->pipe || (PIPE_ISOC != HSU_GET_PIPE_TYPE(ep)))
        {
          HSU_ERR_FATAL("The EP # should not be busy already", ep, 0, 0);
        }
      }
  
      /* Set up TRB paramteres */
      trb_ptr->WORD0.BPTRL = (juint32_t)request->buffer.dma_addr;
      trb_ptr->WORD1.BPTRH = 0x0;
      trb_ptr->WORD2.BUFSIZE = request->transfer_size;
      trb_ptr->WORD2.PCM1 = 0;
      trb_ptr->WORD2.reserved = 0;
      trb_ptr->WORD2.TRBSTS = 0;
  
      if (request->direction == DIRECTION_OUT)
      {
        pipe_type_t type = HSU_GET_PIPE_TYPE(ep);
  
        /* Add padding to the max packet size boundary */
        juint32_t align = core_get_max_packet_size(type, ep->sc->speed);
  
        request->padding_size = request->transfer_size % align;
        if (request->padding_size)
        {
          request->padding_size = align - request->padding_size;
          trb_ptr->WORD2.BUFSIZE += request->padding_size;
        }
      }
      else
      {
        request->padding_size = 0;
      }
  
  
      /* CSP will allow controller to move to next TRB upon completion of OUT xfer due to short packet */
      trb_ptr->WORD3.CSP = (DIRECTION_OUT == request->direction) && !request->single_request;
      
      /* Set TRB type according to EP type and EP0 control state machine */
      if (ep->soft_addr == 0)
      {
        trb_ptr->WORD3.TRBCTL = ep->sc->ep0_trb_type;
      }
      else
      {
        trb_ptr->WORD3.TRBCTL = (PIPE_ISOC == HSU_GET_PIPE_TYPE(ep)) ?
          TRB_CONTROL_ISOC_FIRST : TRB_CONTROL_NORMAL;
      }

      trb_ptr->WORD3.IOC = 0;

      if(request->timeout == 0 && ep->is_in == FALSE)
      {
        trb_ptr->WORD3.IOC = 1;
      }

      trb_ptr->WORD3.CHN = 0;

      // If software wants to indicate a transfer completion to the host by sending a zero-length packet after a
      // multiple of MaxPacketSize, it must set up a zero-length TRB following the last TRB in the transfer.
      if ( ep->is_in 
          && last_request
          && (request->transfer_size > 0)
          && (request->transfer_size%ep->packet_size == 0) 
         )
      {
        trb_ptr->WORD3.LST = 0;
        trb_ptr->WORD3.ISP = 0;
        enqueue_zlp_trb_flag = (ep->pipe && (PIPE_ISOC == HSU_GET_PIPE_TYPE(ep))) ? FALSE : TRUE;
      }
      else if ((PIPE_BULK == HSU_GET_PIPE_TYPE(ep)) && (ep->is_in))
      {
        // Keep LST bit cleared to always use UpdateXfer unless interrupted by EndXfer
        // ISP ignored for IN direction
        trb_ptr->WORD3.LST = 0;
        trb_ptr->WORD3.ISP = 0;
      }
      else
      {
        /* LST bit should be set to complete last OUT xfer when HW queueing is used */ 
        trb_ptr->WORD3.LST = request->single_request || last_request;
        trb_ptr->WORD3.ISP = request->single_request || last_request;
      }
      
      if (PIPE_ISOC == HSU_GET_PIPE_TYPE(ep))
      {
        trb_ptr->WORD3.CSP= 1;
        trb_ptr->WORD3.ISP = last_request | first_request;
        //LST should be set to zero for ISOC ( DWC usb3 sec. 8.3.4)
        trb_ptr->WORD3.LST = 0;
        trb_ptr->WORD3.IOC = last_request;
      }
      
      trb_ptr->WORD3.reserved1 = 0;
      trb_ptr->WORD3.StrmID_SOFNum = stream_id;
      trb_ptr->WORD3.reserved2 = 0;
  
      /* Queue the TRB into the HW */
      trb_ptr->WORD3.HWO = 1;
  
      /* Update trb_soft_ptr to point to next available TRB in list */

      link_trb = ep->streams[stream_id].soft_trb_array.virt + ((ep->max_trbs) - 1);
      
      // Update trb_soft_ptr to point to next available TRB in list
      HSU_INC_SW_TRB_PTR(ep->streams[stream_id].trb_soft_ptr, ep, stream_id, link_trb, ep->streams[stream_id].soft_trb_array.virt);

      if(enqueue_zlp_trb_flag)
      {
        request->requires_zlp = TRUE;
        snps_dcd_enqueue_zlp_trb(ep);
      }
      else
      {
        request->requires_zlp = FALSE;
      }

      //Debug only. Gets printed too often.
      //HSU_ULOG_4(NO_MSG, SNPS_LOG, "snps_dcd_update_trb_list_for_request, queue_trb, trb_word0 0x%08X, trb_word1 0x%08X, trb_word2 0x%08X, trb_word3 0x%08X"",
      //  *(uint32*)&trb_ptr->WORD0, *(uint32*)&trb_ptr->WORD1, *(uint32*)&trb_ptr->WORD02, *(uint32*)&trb_ptr->WORD3);
    }
  }

  return rc;
}

/**
* Function name:  snps_dcd_enqueue_single_request
* Description: Enqueue a single request as a transfer to an EP 
* Parameters:
*     @ep: (IN) pointer to the endpoint structure
*     @request: (IN) pointer to the request structure
*
* Return value: 0 on success, otherwise an error code
* Scope: local
**/
static jresult_t snps_dcd_enqueue_single_request(const snps_dcd_ep_desc_t* ep, 
                                                 request_t* request,
                                                 jbool_t last_request,
                                                 jbool_t first_request)
{
  jresult_t rc = HSU_JSUCCESS;

  /* Update the TRB list for the request */
  rc = snps_dcd_update_trb_list_for_request(ep, request, last_request, first_request);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, failed_building_TRB_list");
    return rc;
  }

  /* Flush cache before starting DMA */
  if (ep->is_in && request->transfer_size)
  {
    j_cache_flush(request->buffer.vaddr, request->buffer.buffer_size);
  }

  return rc;
}

/**
* Function name:  snps_dcd_submit_isoc_request
* Description: submit  an ISOC request as a transfer to an EP 
* Parameters:
*     @ep: (IN) pointer to the endpoint structure
*     @event_params: (IN) event parameter during xfer_nrdy for isoc, contains current microframe number
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_submit_isoc_request(snps_dcd_ep_desc_t* ep, juint16_t event_params, jbool_t new_req)
{
    juint16_t stream_id = SNPS_DCD_DEFAULT_STREAM; 
    jresult_t rc = HSU_JSUCCESS;
    juint32_t DEPCMD;
    snps_dcd_DEPCMD_P1 p1;
    snps_dcd_DEPCMD_P0 p0;
    juint16_t CmdParams;
    juint32_t uf_for_xfer = 0;

    HSU_CHK_EP_STREAM_ID(ep, stream_id);

    /*Program the next uf to send data, by getting the nearest poll interval base from the current microframe */
    /*Next microframe must be integral multiple of poll interval, SNPS spec 8.3.5 (step3) */
    if(new_req == TRUE)
    {
      uf_for_xfer = (event_params & (~(ep->isoc_interval -1))) + 2 * ep->isoc_interval;
      ep->xfer_in_progress = 1;
      ep->xfer_nrdy_elapsed = FALSE;
      
      HSU_ULOG_2(NO_MSG, AUDIO_LOG, "snps_sub_new, req, EP 0x%X, uf_for_xfer %u", ep->hw_addr, uf_for_xfer);
      
      /* Build and issue DEPSTRTXFER command */
      p0.value = 0;
      p1.value = 0;
      p1.DEPSTRTXFER.TDAddrLow = (juint32_t)ep->streams[stream_id].soft_trb_array.phys;
      p0.DEPSTRTXFER.TDAddrHigh = 0;
      DEPCMD = 0;
      DEPCMD = HSU_VAR_OUTM(DEPCMD, uf_for_xfer, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_SHFT);  
      DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
      DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
      DEPCMD = HSU_VAR_OUTM(DEPCMD, SNPS_DCD_DEPCMD_CMDTYPE_STARTTRANSFER, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);
    }  
    else
    {
      HSU_ULOG_2(NO_MSG, AUDIO_LOG, "snps_sub_next, req, EP 0x%X, xfer_rsc_idx %u", ep->hw_addr, ep->streams[stream_id].xfer_rsc_idx);
      /* Build and issue DEPSTRTXFER command */
      p0.value = 0;
      p1.value = 0;
      DEPCMD = 0;
      DEPCMD = HSU_VAR_OUTM(DEPCMD, ep->streams[stream_id].xfer_rsc_idx, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_SHFT);  
      DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
      DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
      DEPCMD = HSU_VAR_OUTM(DEPCMD, SNPS_DCD_DEPCMD_CMDTYPE_UPDATETRANSFER, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);
    }  
    
    rc = snps_dcd_write_ep_command(ep->sc, ep->hw_addr, DEPCMD, p0.value, p1.value, 0, &CmdParams);

    if (rc != HSU_JSUCCESS)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_write_ep_command_failed");        
        return rc;
    }

    ep->streams[stream_id].xfer_rsc_idx = CmdParams & SNPS_EP_XFER_RSC_INDEX_BMASK;

    return rc;
     
}


#define ISOC_EP PIPE_ISOC == HSU_GET_PIPE_TYPE(ep)

/**
* Function name:  snps_dcd_add_request_to_stream
**/
jresult_t snps_dcd_add_request_to_stream(
                                   snps_dcd_ep_desc_t* ep, 
                                   request_t* request,
                                   juint16_t stream_id)
{
  snps_dcd_stream *stream = &(ep->streams[stream_id]);
  request_t *curr_request;
  LIST_HEAD(stream_cur_req_head, request_t) stream_cur_req_list;

  HSU_CHK_EP_STREAM_ID(ep, stream_id);

  if (PIPE_ISOC == HSU_GET_PIPE_TYPE(ep)) // Do not flatten request for ISOC endpoints
  {
    stream->cur_req = request;
    return HSU_JSUCCESS;
  }

  // Request added to the end of the stream list (horizontal)
  if (stream->cur_req)
  {
    LIST_FIRST(&stream_cur_req_list) = stream->cur_req;
    LIST_FOREACH(curr_request, &stream_cur_req_list, next_request_in_list)
    {
      // Reached end of list
      if (NULL == LIST_NEXT(curr_request, next_request_in_list))
      {
        LIST_NEXT(curr_request, next_request_in_list) = request;
        break;
      }
    }
  }
  else
  {
    // Have to iterate the request chain and mark last request to keep track of unique request chain
    stream->cur_req = request;
  }
  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_enqueue_request
* Description: Enqueue a request as a transfer to an EP 
* Parameters:
*     @ep: (IN) pointer to the endpoint structure
*     @request: (IN) pointer to the request structure
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_enqueue_request(snps_dcd_ep_desc_t* ep, 
                                   request_t* request)
{
  static boolean hsu_test_skip_update_xfer = FALSE;
  
  /* TODO: USB3.0 Streams support: Take stream_id from request. */
  juint16_t stream_id = SNPS_DCD_DEFAULT_STREAM; 
  jresult_t rc = HSU_JSUCCESS;
  request_t *curr_request;
  jbool_t ep_is_updatable, ep_is_active;
  juint32_t DEPCMD;
  snps_dcd_DEPCMD_P1 p1;
  snps_dcd_DEPCMD_P0 p0;
  juint16_t CmdParams;
  LIST_HEAD(req_list_head, request_t) req_list;
  juint8_t num_req = 0;
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  HSU_CHK_EP_STREAM_ID(ep, stream_id);

  if ((ep->is_in) && (PIPE_BULK == HSU_GET_PIPE_TYPE(ep)))
  {
    ep_is_updatable = 1;
  }
  else
  {
    ep_is_updatable = 0;
  }
  ep_is_active = (NULL != ep->streams[stream_id].cur_req);

  if (!ep_is_updatable && ep_is_active && (PIPE_ISOC != HSU_GET_PIPE_TYPE(ep)))
  {
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, ep_is_busy, EP 0x%X", ep->hw_addr);
  }
  
  if (PIPE_ISOC != HSU_GET_PIPE_TYPE(ep))
  {
    HSU_ASSERT(ep_is_updatable || !ep_is_active);
  }
  
  if (FALSE == ep_is_updatable)
  {
    /* Initialize the TRB list  */
    rc = snps_dcd_init_stream_trbs(ep, stream_id);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, abort_ep0, init_TRBs_failed, EP 0x%X", ep->hw_addr);
      return rc;
    }
  }
  else
  {
    if ((ep->is_in && PIPE_BULK == HSU_GET_PIPE_TYPE(ep)) && (0 == ep->streams[stream_id].xfer_rsc_idx))
    {
      /* Initialize the TRB list  */
      rc = snps_dcd_init_stream_trbs(ep, stream_id);
      if (rc != HSU_JSUCCESS)
      {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, abort_ep0, init_TRBs_failed, EP 0x%X", ep->hw_addr);
        return rc;
      }
    }
  }
  
  if ((ep->is_in && (PIPE_BULK == HSU_GET_PIPE_TYPE(ep))) || (!ep_is_active))
  {
    snps_dcd_add_request_to_stream(ep, request, stream_id);
  }
  
  LIST_FIRST(&req_list) = request;
  // Debug only: Copy head node in request linked list in to EP debug_req structure
  ep->debug_req = *request;
  LIST_FOREACH(curr_request, &req_list, next_request_in_list)
  {
    rc = snps_dcd_enqueue_single_request(ep, curr_request, 
           LIST_NEXT(curr_request, next_request_in_list) == NULL,
          ((!ep_is_active) &&(curr_request == LIST_FIRST(&req_list))));
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, enqueue_single/multi_request_failed, EP 0x%X", ep->hw_addr);
      break;
    }
  
    num_req++;
  } //LIST_FOREACH
  
  hsu_perf_log(PERF( SNPS_DCD_ENQUEUE_REQUEST, Enqueued_all_trbs), num_req);
  
  //For ISOC, we wait for xfer_nrdy for request submission to HW 
  if (PIPE_ISOC == HSU_GET_PIPE_TYPE(ep))
  {
    //Do update xfer, if the xfer is in progress   
    if(ep->xfer_in_progress)
    {
      rc = snps_dcd_submit_isoc_request(ep, 0, FALSE);
      return rc;
    }  
    
    //if a xfer_nrdy elpsed before we can submit, then we will  have to abort the xfers on the EP for a fresh xfer_nrdy
    //dwc spec. 8.3.5, step4 
    if(ep->xfer_nrdy_elapsed)
    {
      rc = snps_dcd_abort_xfer(ep);
      if (rc != HSU_JSUCCESS)
      {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_write_ep_cmd_failed, EP 0x%X", ep->hw_addr);        
      }
      else
      {
        ep->xfer_nrdy_elapsed = FALSE;
      }     
    }
    //else, we just return and wait for xfer_nrdy, to submit the request
    return rc;
  }

  p0.value = 0;
  p1.value = 0;
  
  // The very first time a TRB is used, it must be a start xfer
  if (ep_is_updatable && (ep->streams[stream_id].xfer_rsc_idx != 0))
  {
    switch (platform_info->hsu_platform_id)
    {
      case HSU_PLATFORM_VIRTIO:
      case HSU_PLATFORM_RUMI:
      {
        if(ep->hw_addr == 3)
        {
          //HSU_ULOG(NO_MSG, DATA_LOG, "diag_tx_entry");
        }
      }
      break;
      default:
      break;
    }
    
    /* Build and issue UPDATETRANSFER command */
    DEPCMD = 0;
    DEPCMD = HSU_VAR_OUTM(DEPCMD, ep->streams[stream_id].xfer_rsc_idx, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_SHFT);  
    DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_HIPRI_FORCERM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_HIPRI_FORCERM_SHFT);
    DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
    DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
    DEPCMD = HSU_VAR_OUTM(DEPCMD, SNPS_DCD_DEPCMD_CMDTYPE_UPDATETRANSFER, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);
    HSU_ULOG_2(NO_MSG, SNPS_LOG, "UPDATE_XFER, EP 0x%X, xfer_rsc = %u", ep->hw_addr, ep->streams[stream_id].xfer_rsc_idx);

    if (hsu_test_skip_update_xfer)
    {
      return HSU_JSUCCESS;
    }
  }
  else
  {
    /* Build and issue DEPSTRTXFER command */
    p0.value = 0;
    p1.value = 0;
    p1.DEPSTRTXFER.TDAddrLow = (juint32_t)ep->streams[stream_id].soft_trb_array.phys + ((uint32)(ep->streams[stream_id].trb_hw_ptr) - (uint32)(ep->streams[stream_id].soft_trb_array.virt));
    p1.DEPSTRTXFER.TDAddrLow = (juint32_t)ep->streams[stream_id].soft_trb_array.phys + ((uint32)(ep->streams[stream_id].trb_hw_ptr) - (uint32)(ep->streams[stream_id].soft_trb_array.virt));
    p0.DEPSTRTXFER.TDAddrHigh = 0;
    DEPCMD = 0;
    DEPCMD = HSU_VAR_OUTM(DEPCMD, stream_id, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_SHFT);  
    DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
  #ifdef FEATURE_HS_USB_BAM
    if (request->sps_mode)
    {
      DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
    }
    else
    {
      DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
    }
  #else /* ~FEATURE_HS_USB_BAM */
    DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
  #endif /* ~FEATURE_HS_USB_BAM */
    DEPCMD = HSU_VAR_OUTM(DEPCMD, SNPS_DCD_DEPCMD_CMDTYPE_STARTTRANSFER, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);
    HSU_ULOG_1(NO_MSG, SNPS_LOG, "START_XFER, EP 0x%X", ep->hw_addr);
  }
  
  rc = snps_dcd_write_ep_command(ep->sc, ep->hw_addr, 
    DEPCMD, p0.value, p1.value, 0, &CmdParams);
  
  switch (platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_VIRTIO:
    case HSU_PLATFORM_RUMI:
    {
      if(ep->hw_addr == 3)
      {
        //HSU_ULOG(NO_MSG, DATA_LOG, "diag_tx");
      }
    }
    break;
    default:
    break;
  }
  
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_write_ep_cmd_failed, EP 0x%X", ep->hw_addr);
    return rc;
  }
  else
  {
#if 0
    if (!ep_is_active)
#endif
    {
      if (0 == ep->streams[stream_id].xfer_rsc_idx)
      {
        /* Save transfer resource ID in request structure */
        ep->streams[stream_id].xfer_rsc_idx = (juint8_t)CmdParams & SNPS_EP_XFER_RSC_INDEX_BMASK;
        HSU_ULOG_2(NO_MSG, SNPS_LOG, "SAVE_XFER_RSC %u, EP 0x%X", ep->streams[stream_id].xfer_rsc_idx, ep->hw_addr);
      }
    }
  }
  
  if (num_req > 1)
  {
    HSU_ULOG_1(NO_MSG, SNPS_LOG, "multiple_requests, EP 0x%X", ep->hw_addr);
    hsu_perf_log(PERF(SNPS_DCD_ENQUEUE_REQUEST, Multiple_requests_queued),num_req);
  }
  else
  {
    HSU_ULOG_1(NO_MSG, SNPS_LOG, "single_request, EP 0x%X", ep->hw_addr);
  }
  return rc;
}

/**
* Function name:  snps_dcd_dequeue_request
* Description: Aborts an endpoint transfer
* Parameters:
*     @ep: (IN) pointer to the endpoint structure
*     @request: (IN) pointer to the request structure
*     @soft_trb: (IN) pointer to the soft_trb structure
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_dequeue_request(snps_dcd_ep_desc_t* ep, 
                                   request_t* request)
{
  request_t *curr_req;
  LIST_HEAD(req_list_head, request_t) req_list;
  juint16_t stream_id = SNPS_DCD_DEFAULT_STREAM;

  HSU_CHK_EP_STREAM_ID(ep, stream_id);
  HSU_ASSERT(request);

  if (request->complete)
  {
    // Invoke the CORE completion callback
    request->completed_req = 0;

    LIST_FIRST(&req_list) = request;  
    LIST_FOREACH(curr_req, &req_list, next_request_in_list)
    {
      // Update request
      curr_req->status = REQUEST_CANCELLED;

      // First request (head) should keep count of total completed.
      request->completed_req++;
    }

    HSU_ULOG_2(NO_MSG, SNPS_LOG, "req_is_dequeued_for_ep, EP 0x%X, count %d", ep->hw_addr, request->completed_req);

    request->complete(request);
  }
  else
  {
    HSU_ULOG_2(NO_MSG, DBG_E_LOG, "complete_cb_missing, EP 0x%X, req 0x%X", ep->hw_addr, (uint32)request);        
  }
  
  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_abort_ep0
* Description: Aborts pending transfer on the EP0
* Parameters:
*     @sc: (IN) The DCD SW context
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_abort_ep0(snps_dcd_soft_dc_t *sc)
{
  snps_dcd_ep_desc_t* ep;
  request_t* request;
  jresult_t rc = HSU_JSUCCESS;
  juint32_t DEPCMD;
  juint8_t dir;
  juint16_t stream_id = SNPS_DCD_DEFAULT_STREAM;

  for (dir = SNPS_DCD_EP0_OUT_ADDR; dir <= SNPS_DCD_EP0_IN_ADDR; dir++)
  {
    ep = &sc->ep_desc_table[dir];
    HSU_CHK_EP_STREAM_ID(ep, stream_id);

    if (NULL == ep->streams)
    {
      return HSU_JSUCCESS;
    }
    request = ep->streams[stream_id].cur_req;

    if (request && sc->ep0_trb_type == TRB_CONTROL_CONTROL_SETUP)
    {
      HSU_ULOG(ERROR_MSG, SNPS_LOG, "DCD, abort_ep_with_contrl_setup_trb");
      return HSU_JSUCCESS;
    }

    /* If a request is pending at EP0 out, abort it */
    if (request != NULL)
    {
      rc = snps_dcd_stall_ep(ep, TRUE);
      if (rc != HSU_JSUCCESS)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, error_stalling_ep0_out");
        return rc;
      }

      /* Build and issue ENDTRANSFER command */
      DEPCMD = 0;
      DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_HIPRI_FORCERM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_HIPRI_FORCERM_SHFT);
	  //harcoding resouce index to 0x1 since xfer_rsc_idx is getting reset to 0 for ep tx in certain cases
	  if(ep->hw_addr == 1)
	  {
	  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_SHFT);
	  }
	  else
	  {
      DEPCMD = HSU_VAR_OUTM(DEPCMD, ep->streams[SNPS_DCD_DEFAULT_STREAM].xfer_rsc_idx, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_SHFT);
	  }	  
      DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
      DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
      DEPCMD = HSU_VAR_OUTM(DEPCMD, SNPS_DCD_DEPCMD_CMDTYPE_ENDTRANSFER, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);

      rc = snps_dcd_write_ep_command(ep->sc, ep->hw_addr, DEPCMD, 0, 0, 0, NULL);
      if (rc != HSU_JSUCCESS)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_write_ep_cmd_failed");
      }

      ep->streams[stream_id].xfer_rsc_idx = 0;
      ep->streams[stream_id].stream_initialized = FALSE;
            
      // Upon abort endpoint, reset all HW and SW TRB pointers back to the front of the array
      ep->streams[stream_id].trb_soft_ptr
        = ep->streams[stream_id].trb_hw_ptr 
        = ep->streams[stream_id].soft_trb_array.virt;

      ep->streams[stream_id].cur_req = NULL;

      if (snps_dcd_dequeue_request(ep, request) != HSU_JSUCCESS)
      {
        rc = EUNKNOWN;
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, failed_dequeuing_request");        
      }
    }
  }    

  // Do not queue setup packet when bus is suspended or speed is UNKNOWN
  // E.g. disconnect, bus reset
  if ( sc->speed != SPEED_UNKNOWN
       && sc->core_ctx  
       && ((core_t*)sc->core_ctx)->core_state != CORE_SUSPENDED )
  {
    // Setup the first transfer on EP0 OUT (SETUP phase)
    rc = snps_dcd_send_setup_request(sc);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, error_in_snps_dcd_send_setup_request");      
      return rc;
    }
  }

  return rc;
}

/**
* Function name:  snps_dcd_send_setup_request
* Description: Enqueue a SETUP phase of a control transfer to EP0
* Parameters:
*     @sc: (IN) The DCD SW context
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_send_setup_request(snps_dcd_soft_dc_t* sc)
{
  snps_dcd_ep_desc_t* ep = &sc->ep_desc_table[SNPS_DCD_EP0_OUT_ADDR];
  request_t* request = &sc->ep0_request;
  jresult_t rc = HSU_JSUCCESS;
  boolean ep_is_active;
  juint16_t stream_id = SNPS_DCD_DEFAULT_STREAM; 

  HSU_CHK_EP_STREAM_ID(ep, stream_id);

  ep_is_active = (NULL != ep->streams[stream_id].cur_req);
  if (!ep_is_active)
  {
    /* Create the request. Save the TRB type. It will be used to identify the 
    current control transfer state */
    sc->ep0_trb_type = TRB_CONTROL_CONTROL_SETUP;
    request->direction = DIRECTION_OUT;
    request->transfer_size = sizeof(struct usb_ctrlrequest);
    request->status = REQUEST_INPROGRESS;
    request->completed_req = 0;

    /* Enqueue the request to the controller */
    rc = snps_dcd_enqueue_request(ep, request);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, error_enqueuing_ep0_out_setup_req");

      rc = snps_dcd_stall_ep(ep, TRUE);
      if (rc != HSU_JSUCCESS)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, error_stalling_ep0_out");        
        return rc;
      }
      return rc;
    }
    else
    {
      sc->ctrl_state = SNPS_CTRL_STATE_SETUP_RCVD;
      sc->xfer_nrdy_rcvd = FALSE;
      sc->ctrl_pending_request = NULL;
    }
    
  }

  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_send_status_request
* Description: Enqueues a STATUS phase of a control transfer to EP0
* Parameters:
*     @dcd: (IN) pointer to the device structure
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_send_status_request(snps_dcd_soft_dc_t* sc)
{
  snps_dcd_ep_desc_t* ep = &sc->ep_desc_table[SNPS_DCD_EP0_OUT_ADDR];
  request_t* request = &sc->ep0_request;
  jresult_t rc1 = HSU_JSUCCESS;
  jresult_t rc2 = HSU_JSUCCESS;
  boolean ep_is_active;
  juint16_t stream_id = SNPS_DCD_DEFAULT_STREAM; 

  HSU_CHK_EP_STREAM_ID(ep, stream_id);

  ep_is_active = (NULL != ep->streams[stream_id].cur_req);
  if (!ep_is_active)
  {
    /* Create the request. Save the TRB type. It will be used to identify the 
    current control transfer state */
    sc->ep0_trb_type = TRB_CONTROL_CONTROL_STATUS_3;
    request->direction = DIRECTION_OUT;
    request->transfer_size = /*sizeof(struct usb_ctrlrequest)*/0;
    request->status = REQUEST_INPROGRESS;
    request->completed_req = 0;

    /* Enqueue the request to the controller */
    rc1 = snps_dcd_enqueue_request(ep, request);
    if (rc1 != HSU_JSUCCESS)
    {
      HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, error_enqueing_ep0_out_status_request, rc1 %u", rc1);      
      rc2 = snps_dcd_ops_stall_ep(sc->dev, NULL, TRUE);
      if (rc2 != HSU_JSUCCESS)
      {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, error_stalling_ep0_out, rc2 %u", rc2);
        return rc2;
      }
      return rc1;
    }
  }

  return HSU_JSUCCESS;
}

/*=========================== Transfer Completion ===========================*/

/**
* Function name:  snps_dcd_start_next_transfer
* Description: Starts next transfer enqueued (SW queing) for the given EP
* Parameters:
*     @ep: (IN) The EP descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: local
**/
static jresult_t snps_dcd_start_next_transfer(snps_dcd_ep_desc_t* ep)
{
  request_t *next;
  jresult_t rc = HSU_JSUCCESS;

  if (!ep->pipe)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, cant_start_next_transfer_for_ep0");    
    return JEINVAL;
  }

  /* No more requests */
  if (STAILQ_EMPTY(&ep->pipe->req_queue))
  {
    HSU_ULOG_1(NO_MSG, SNPS_LOG, "pipe req q empty, EP 0x%X", ep->hw_addr);
    return HSU_JSUCCESS;
  }

  /* Start the next request on queue */
  HSU_ULOG_1(NO_MSG, SNPS_LOG, "w pipe req q, EP 0x%X", ep->hw_addr);

  next = STAILQ_FIRST(&ep->pipe->req_queue);

  if ((PIPE_ISOC == HSU_GET_PIPE_TYPE(ep)) || ((ep->is_in) && (PIPE_BULK == HSU_GET_PIPE_TYPE(ep))))
  {
    // For OUT or single EP, send_io_request() puts into req_queue
    // Upon completion, it is removed.  So, only remove for OUT or ISOC
    STAILQ_REMOVE_HEAD(&ep->pipe->req_queue, next_request);
  }

  rc = snps_dcd_enqueue_request(ep, next);

  return rc;
}


/**
* Function name:  snps_dcd_ctrl_sm_error
* Description: Handles errors in control transfer state machine
* Parameters:
*     @ep: (IN) The EP descriptor
*
* Return value: None.
* Scope: local
**/
static void snps_dcd_ctrl_sm_error(snps_dcd_ep_desc_t* ep)
{
  jresult_t rc = HSU_JSUCCESS; 

  HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, unexpected_pack/event, state %u", ep->sc->ctrl_state);   

  rc = snps_dcd_abort_ep0(ep->sc);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, abort_ep0_failed");            
  }
}

/**
* Function name:  snps_dcd_abort_xfer
* Description: Abort transfer on an EP
* Parameters:
*     @ep: (IN) The EP descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_abort_xfer(snps_dcd_ep_desc_t* ep)
{
  jresult_t rc = HSU_JSUCCESS;
  juint32_t DEPCMD;
  snps_dcd_DEPCMD_P1 p1;
  snps_dcd_DEPCMD_P0 p0;
  juint16_t CmdParams;

  HSU_CHK_EP_STREAM_ID(ep, SNPS_DCD_DEFAULT_STREAM);

  /* Build and issue ENDTXFER command */
  p0.value = 0;
  p1.value = 0;
  
  DEPCMD = 0;
  DEPCMD = HSU_VAR_OUTM(DEPCMD, ep->streams[SNPS_DCD_DEFAULT_STREAM].xfer_rsc_idx, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_SHFT);  
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_HIPRI_FORCERM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_HIPRI_FORCERM_SHFT);
  DEPCMD = HSU_VAR_OUTM(DEPCMD, SNPS_DCD_DEPCMD_CMDTYPE_ENDTRANSFER, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);

  rc = snps_dcd_write_ep_command(ep->sc, ep->hw_addr, DEPCMD, p0.value, p1.value, 0, &CmdParams);

  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_submit_isoc_request, snps_dcd_write_ep_command_failed");      
    return rc;
  }

  return rc;
}


/**
* Function name:  snps_dcd_ctrl_sm_notify
* Description: Control transfer state machine
* Parameters:
*     @ep: (IN) The EP descriptor
*     @request: (IN) The request
*     @event: (IN) EP Event
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/

jresult_t snps_dcd_ctrl_sm_notify
(
  snps_dcd_ep_desc_t* ep, 
  request_t* request, 
  snps_dcd_ep_event_type event
)
{
  jresult_t rc = HSU_JSUCCESS;
  struct usb_ctrlrequest* ctrl;

  HSU_ULOG_ENUM_1(NO_MSG, SNPS_CTRL_LOG, "entry, EP 0x%X", HSU_SNPS_EP_EVNT_OFFSET2, event, ep->hw_addr);

  // Sanity check for EP address
  if (ep->soft_addr != 0)
  {
    HSU_ERR_FATAL("snps_dcd_ctrl_sm_notify: Incompatible EP, ep=%d, req=0x%x, evt=%d", 
          ep->hw_addr, request, event);
    return JEINVAL;
  }

  // Check if valid event
  if (event > SNPS_CTRL_EVT_CORE_REQ)
  {
    HSU_ERR_FATAL("snps_dcd_ctrl_sm_notify: Invalid event, ep=%d, req=0x%x, evt=%d", 
      ep->hw_addr, request, event);
    return JEINVAL;
  }

  // For events XFER_COMPLETE and SNPS_CTRL_EVT_CORE_REQ, request cannot be null
  // for event XFER_NOTREADY request should be NULL
  if (((request == NULL) && (event == SNPS_CTRL_EVT_XFER_COMPLETE || event == SNPS_CTRL_EVT_CORE_REQ))
      || (request && (event == SNPS_CTRL_EVT_XFER_NOTREADY)))
  {
    HSU_ERR_FATAL("snps_dcd_ctrl_sm_notify: Incompatible request, ep=%d, req=0x%x, evt=%d", 
      ep->hw_addr, request, event);
    return JEINVAL;
  }

  if (request && !request->single_request)
  {
    
    HSU_ERR_FATAL("snps_dcd_ctrl_sm_notify: Probable request corruption, ep=%d, req=0x%x, evt=%d", 
      ep->hw_addr, request, event);
    return JEINVAL;
  }

  // This simplifies state machine logic for DATA_OR_STATUS2 and STATUS3_IN states
  if(ep->sc->ctrl_pending_request)
  {
    request = ep->sc->ctrl_pending_request;
  }

  // If there was error detected on TRB such as setup_pending_bit, then restart the state machine
  if (event == SNPS_CTRL_EVT_XFER_ERROR)
  {
    snps_dcd_ctrl_sm_error(ep);
    return HSU_JSUCCESS;
  }

  // Control Transfer States
  switch(ep->sc->ctrl_state)
  {
    case SNPS_CTRL_STATE_SETUP_START:
    {
      HSU_ULOG_ENUM_1(NO_MSG, SNPS_CTRL_LOG, "state, setup_start, EP 0x%X", HSU_SNPS_EP_EVNT_OFFSET2, event, ep->hw_addr);
      if (event == SNPS_CTRL_EVT_XFER_COMPLETE)
      {
        HSU_ULOG(NO_MSG, SNPS_CTRL_LOG, "queue, setup_request");
        rc = snps_dcd_send_setup_request(ep->sc);
        if (rc != HSU_JSUCCESS)
        {
          return rc;
        } 
      }
      else
      {
        snps_dcd_ctrl_sm_error(ep);
      }
    }
    break;

    case SNPS_CTRL_STATE_SETUP_RCVD:
    {
      HSU_ULOG_ENUM_1(NO_MSG, SNPS_CTRL_LOG, "state, setup_rcvd, EP 0x%X", HSU_SNPS_EP_EVNT_OFFSET2, event, ep->hw_addr);
      if (event == SNPS_CTRL_EVT_XFER_COMPLETE 
         && ep->is_in == FALSE
         && ep->sc->ep0_trb_type == TRB_CONTROL_CONTROL_SETUP)
      {
        ep->sc->ctrl_state = SNPS_CTRL_STATE_DATA_OR_STATUS2;

        // A setup packet has completed
        ctrl = (struct usb_ctrlrequest*)request->buffer.vaddr;
        ctrl->wValue = le16toh(ctrl->wValue);
        ctrl->wIndex = le16toh(ctrl->wIndex);
        ctrl->wLength = le16toh(ctrl->wLength);
        ep->sc->ep0_trb_type = (ctrl->wLength)
          ? TRB_CONTROL_CONTROL_DATA : TRB_CONTROL_CONTROL_STATUS_2;

        // Decode setup packet.
        if (ep->sc->events->core_handle_ep0(ep->sc->core_ctx, 
            (juint8_t*)ctrl, ep->sc->speed))
        {
          // Set stall on bad packet
          rc = snps_dcd_ops_stall_ep(ep->sc->dev, NULL, TRUE);
          if (rc != HSU_JSUCCESS)
          {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, error_stalling_ep0_out, rc %u", rc);   
          }
          return JEINVAL;
        }
      }
      else
      {
        snps_dcd_ctrl_sm_error(ep);
      }
    }
    break;

    case SNPS_CTRL_STATE_DATA_OR_STATUS2:
    {
      HSU_ULOG_ENUM_1(NO_MSG, SNPS_CTRL_LOG, "state, DATA_OR_STATUS2, EP 0x%X", HSU_SNPS_EP_EVNT_OFFSET2, event, ep->hw_addr);

      // Queue DATA or STATUS2 request only when we get XferNotReady and Core Layer Notification
      if ((event == SNPS_CTRL_EVT_CORE_REQ && ep->sc->xfer_nrdy_rcvd == TRUE)
          || (event == SNPS_CTRL_EVT_XFER_NOTREADY && ep->sc->ctrl_pending_request))
      {
        if (ep->is_in)
        {
          // STATUS2
          // Do not interpret zero length Data phase as 2-stage control transfer
          // E.g. Get encapsulated response with zero length data phase.
          if ((request->transfer_size == 0) && (ep->sc->ep0_trb_type == TRB_CONTROL_CONTROL_STATUS_2))
          {
            HSU_ULOG(NO_MSG, SNPS_CTRL_LOG, "queue, status_IN_request");
            snps_dcd_enqueue_request(ep,request);
            
            // Restore original state for ctrl_pending_request and xfer_nrdy_rcvd
            ep->sc->ctrl_pending_request = NULL;
            ep->sc->xfer_nrdy_rcvd = FALSE;      

            ep->sc->ctrl_state = SNPS_CTRL_STATE_SETUP_START;
          }
          else  // DATA IN
          {
            HSU_ULOG(NO_MSG, SNPS_CTRL_LOG, "queue, data_IN_request");
            snps_dcd_enqueue_request(ep,request);

            // Restore original state for ctrl_pending_request and xfer_nrdy_rcvd
            ep->sc->ctrl_pending_request = NULL;
            ep->sc->xfer_nrdy_rcvd = FALSE;

            ep->sc->ctrl_state = SNPS_CTRL_STATE_DATA_INTERPRETE;
          }
        }
        else  // DATA OUT
        {
          HSU_ULOG(NO_MSG, SNPS_CTRL_LOG, "queue, data_OUT_request");
          snps_dcd_enqueue_request(ep,request);

          // Restore original state for ctrl_pending_request and xfer_nrdy_rcvd
          ep->sc->ctrl_pending_request = NULL;
          ep->sc->xfer_nrdy_rcvd = FALSE;

          ep->sc->ctrl_state = SNPS_CTRL_STATE_DATA_INTERPRETE;
        }
      }
      else if (event == SNPS_CTRL_EVT_CORE_REQ && ep->sc->xfer_nrdy_rcvd == FALSE)
      {
        // Copy request and wait till we receive XferNotReady event.
        ep->sc->ctrl_pending_request = request;
      }
      else if (event == SNPS_CTRL_EVT_XFER_NOTREADY && !ep->sc->ctrl_pending_request)
      {
        // Set xfer_nrdy_rcvd flag and wait till we receive request from Core Layer
        ep->sc->xfer_nrdy_rcvd = TRUE;
      }
      else
      {
        snps_dcd_ctrl_sm_error(ep);
      }
    }
    break;

    case SNPS_CTRL_STATE_DATA_INTERPRETE:
    {
      HSU_ULOG_ENUM_1(NO_MSG, SNPS_CTRL_LOG, "state, DATA_INTERPRETE, EP 0x%X", HSU_SNPS_EP_EVNT_OFFSET2, event, ep->hw_addr);
      if (event == SNPS_CTRL_EVT_XFER_COMPLETE)
      {
        if (ep->is_in)  // Data IN phase completed.
        {
          // Set stall and restart control state machine in case of bad data packet
          if (request->transfer_size != request->bytes_transferred)
          {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, transfer_size_mismatch_error, stall_EP0");
            rc = snps_dcd_ops_stall_ep(ep->sc->dev, NULL, TRUE);
            if (rc != HSU_JSUCCESS)
            { 
               HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, error_stalling_ep0_out, rc %u", rc);   
            }
            return JEINVAL;
          }
          ep->sc->ctrl_state = SNPS_CTRL_STATE_STATUS3_OUT;
        }
        else // Data OUT phase completed.
        {
          ep->sc->ctrl_state = SNPS_CTRL_STATE_STATUS3_IN;
        }
      }
      else if (event == SNPS_CTRL_EVT_XFER_NOTREADY) // Ignore any XferNotReady at this stage
      {
        HSU_ULOG_ENUM_1(NO_MSG, SNPS_CTRL_LOG, "ignore_XFER_NOTEREADY, EP 0x%X", HSU_SNPS_EP_EVNT_OFFSET2, event, ep->hw_addr);
      }
      else
      {
        snps_dcd_ctrl_sm_error(ep);
      }
    }
    break;

    case SNPS_CTRL_STATE_STATUS3_OUT:
    {
      HSU_ULOG_ENUM_1(NO_MSG, SNPS_CTRL_LOG, "state, STATUS3_OUT, EP 0x%X", HSU_SNPS_EP_EVNT_OFFSET2, event, ep->hw_addr);
      if (event == SNPS_CTRL_EVT_XFER_NOTREADY && ep->is_in == FALSE)
      {
        HSU_ULOG(NO_MSG, SNPS_CTRL_LOG, "queue, Status_Out_request");
        snps_dcd_send_status_request(ep->sc);
        ep->sc->ctrl_state = SNPS_CTRL_STATE_SETUP_START;
      }
      else
      {
         snps_dcd_ctrl_sm_error(ep);
      }
    }
    break;

    case SNPS_CTRL_STATE_STATUS3_IN:
    {
      HSU_ULOG_ENUM_1(NO_MSG, SNPS_CTRL_LOG, "state, STATUS3_IN, EP 0x%X", HSU_SNPS_EP_EVNT_OFFSET2, event, ep->hw_addr);
      if (((event == SNPS_CTRL_EVT_CORE_REQ && ep->sc->xfer_nrdy_rcvd == TRUE)
          || (event == SNPS_CTRL_EVT_XFER_NOTREADY && ep->sc->ctrl_pending_request))
          && ep->is_in)
      {
        HSU_ULOG(NO_MSG, SNPS_CTRL_LOG, "queue, status_IN_request");
        ep->sc->ep0_trb_type = TRB_CONTROL_CONTROL_STATUS_3;
        snps_dcd_enqueue_request(ep, request);

        // Restore original state for ctrl_pending_request and xfer_nrdy_rcvd
        ep->sc->ctrl_pending_request = NULL;
        ep->sc->xfer_nrdy_rcvd = FALSE;

        ep->sc->ctrl_state = SNPS_CTRL_STATE_SETUP_START;
      }
      // Core Layer Request received before XferNotReady - Status IN
      else if (ep->is_in  && event == SNPS_CTRL_EVT_CORE_REQ && ep->sc->xfer_nrdy_rcvd == FALSE)
      {
        // Copy request and wait till we receive XferNotReady event.
        ep->sc->ctrl_pending_request = request;
      }
      //XferNotReadyStatus - IN received before Core Layer Request notification
      else if (ep->is_in && event == SNPS_CTRL_EVT_XFER_NOTREADY && !ep->sc->ctrl_pending_request)
      {
        // Set xfer_nrdy_rcvd flag and wait till we receive request from Core Layer
        ep->sc->xfer_nrdy_rcvd = TRUE;
      }
      else
      {
        snps_dcd_ctrl_sm_error(ep);
      }
    }
    break;

  default:
    {
      HSU_ERR_FATAL("snps_dcd_ctrl_sm_notify: Unknown State", ep->hw_addr, event, ep->sc->ctrl_state);
    }
    break;
  }
  HSU_ULOG_ENUM_1(NO_MSG, SNPS_CTRL_LOG, "exit, EP 0x%X", HSU_SNPS_EP_EVNT_OFFSET2, event, ep->hw_addr);
  return rc;
}

/**
* Function name:  snps_dcd_handle_xfer_completion
* Description: Handle the event of transfer completion 
* Parameters:
*     @ep: (IN) The EP descriptor
*     @stream_id: (IN) The stream ID
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_handle_xfer_completion(snps_dcd_ep_desc_t* ep, juint16_t stream_id)
{
  request_t *curr_req;
  snps_dcd_trb *trb, *link_trb;
  jresult_t rc = HSU_JSUCCESS;
  request_t *req;
  request_t *prev_req = NULL;
  jbool_t check_TRBs = TRUE;
  jint_t request_completed = 1;
  juint32_t byte_not_xfered = 0;
  jint_t ep0_error_detected = 0;

  LIST_HEAD(req_list_head, request_t) req_list;

  HSU_CHK_EP_STREAM_ID(ep, stream_id);
  req = ep->streams ? ep->streams[stream_id].cur_req : NULL;

  LIST_FIRST(&req_list) = req;

  #ifdef FEATURE_HSU_DCD_DEBUG_LOGS
  if (req && req->complete && (ep->soft_addr == 1)) // log completions of EP 0 traffic
  {
    hsu_ep0_log(DCD__DCD_HANDLE_COMP_LOG, HSU_CLAIMED_IFACE_TO_RMNET_IFACE(ep->claimed_iface), 0);
  }
  #endif // FEATURE_HSU_DCD_DEBUG_LOGS

  if (!req)
  {
    return 0;
  }

  if (ep->pipe && ep->pipe->status == PIPE_ABORTING) 
  {
    DBG_I(DSLAVE_DCD, ("DCD: HANDLE COMP: recieved completion on aborting pipe\n"));
    check_TRBs = FALSE;
  }
  else
  {
    curr_req = req;
    while (curr_req)
    {
      if (curr_req->status == REQUEST_CANCELLED)
      {
        check_TRBs = FALSE;
        break;
      }
      curr_req = LIST_NEXT(curr_req, next_request_in_list);
    }
  }

  // Assumption:  req is ep->stream[n]'s cur_req, and req is not NULL.
  // If the ep->stream[n]'s cur_req is PENDING status, this is not expected.
  // Print error log and recover by switching the status to INPROGRESS.
  if (req->status == REQUEST_PENDING)
  {
    
    HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "first req is PENDING, cur_req 0x%8X, EP 0x%X", (uint32)req, ep->hw_addr);

    // Run the iteration, to change the PENDING status back to INPROGRESS
    LIST_FIRST(&req_list) = ep->streams[stream_id].cur_req;

    // Initializing the status of the non completed requests
    LIST_FOREACH(curr_req, &req_list, next_request_in_list)
    {
      curr_req->status = REQUEST_INPROGRESS;
    }
    return 0;
  }

  link_trb = ep->streams[stream_id].soft_trb_array.virt + ((ep->max_trbs) - 1);

  // Check for TRB completion loop:
  // Iterate through the EP's request linked list and TRB ring together to check if all TRBs in the ring are completed.
  // If handle_timeout() is called prior to mark the request status to PENDING, 
  //   this means partial completion is supported, and partial completion will be handled outside this iteration.
  // The IN request with MAX PACKET SIZE transfer will own another TRB for ZLP.
  // If the ZLP is not completed, function will exit till ZLP completion interrupt in invoked.
  if (check_TRBs)
  {
    // Check if all TRB's completed, 
    // else wait till EP finishes processing all TRBs.
    for (curr_req = req, trb = ep->streams[stream_id].trb_hw_ptr;
        trb < link_trb && curr_req;)
    {
      if (curr_req->status == REQUEST_PENDING)
      {
        request_completed = 0;
        break;
      }
      // HSU addition: SPS
      #ifdef FEATURE_HS_USB_BAM
      // TODO: Update comment
      // When using infinite eTD, refer to status as TDI_4X_TD_ACTIVE. 
      // This is done as a workaround for current HW behaviour, which clears
      // the TDI_4X_TD_ACTIVE in the infinite ETD when transfer is done. 
      // Remove this workaround if and when HW fix will be available.
      if (curr_req->sps_mode && (curr_req->tbe == 0))
      {
        // "Infinite" mode
        curr_req->bytes_transferred = 0;
        return 0;
      }
      #endif // FEATURE_HS_USB_BAM
      // End of HSU addition

      if (trb->WORD3.HWO)
      {
        // Exit out if active transfer still exists.
        return 0;
      }

      if (trb->WORD2.TRBSTS & SNPS_DCD_TRB_STATUS_SETUP_PENDING)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, setup_pending_bit_is_on");

		ep0_error_detected = 1;

        if (snps_dcd_ops_stall_ep(ep->sc->dev, NULL, TRUE) != HSU_JSUCCESS)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, error_stalling_ep0_out");
        }
        break;
      }
 
      HSU_INC_TRB_PTR(trb, ep, stream_id, link_trb);
      if (curr_req->requires_zlp)
      {
        if (trb->WORD3.HWO)
        {
          // Exit out if ZLP is still in active transfer
          hsu_event_log(EVT_WAIT_FOR_ZLP_TO_COMPLETE_IN_REQ, ((core_t*)(ep->sc->core_ctx))->core_number);
          return 0;
        }
        // Next request is after the ZLP TRB
        HSU_INC_TRB_PTR(trb, ep, stream_id, link_trb);
      }

      if (!req->single_request)
      {
        curr_req = LIST_NEXT(curr_req, next_request_in_list);
      }
      else
      {
        break;
      }
    }
  }    

  if (!request_completed)
  {
    // Going over all the requests.
    LIST_FOREACH(curr_req, &req_list, next_request_in_list)
    {
      if (curr_req->status == REQUEST_PENDING)
      {
        // Separating the completed request list from the non completed
        ep->streams[stream_id].cur_req = curr_req;
        break;
      }
      prev_req = curr_req;
    }

    if (prev_req)
    {
      LIST_NEXT(prev_req, next_request_in_list) = NULL;
    }


    // After this line, req is pointing to head of the completed requests.  EP's curr_req is pointing to requests still in progress.
    // Run the iteration, to change the PENDING status back to INPROGRESS
    LIST_FIRST(&req_list) = ep->streams[stream_id].cur_req;

    // Initializing the status of the non completed requests
    LIST_FOREACH(curr_req, &req_list, next_request_in_list)
    {
      curr_req->status = REQUEST_INPROGRESS;
    }
  }
  else
  {
    // Free request from queue
    ep->streams[stream_id].cur_req = NULL;
  }

  // Iterate through completed requests
  for (curr_req = req, trb = ep->streams[stream_id].trb_hw_ptr;
      trb < link_trb && curr_req;)
  {
    // HSU addition: SPS */
    #ifdef FEATURE_HS_USB_BAM
    if (curr_req->sps_mode && !curr_req->tbe)
    {
      // "Infinite" mode
      curr_req->bytes_transferred = 0;
    }
    else
    #endif /* FEATURE_HS_USB_BAM */
      /* End of HSU addition */
    {
      byte_not_xfered = trb->WORD2.BUFSIZE;

      /* Updating bytes transferred counter */
      curr_req->bytes_transferred = curr_req->padding_size + curr_req->transfer_size - byte_not_xfered;
    }

    // Update trb_hw_ptr to point to next TRB that HW may own
    HSU_INC_HW_TRB_PTR(ep->streams[stream_id].trb_hw_ptr, ep, stream_id, link_trb, ep->streams[stream_id].soft_trb_array.virt);

    HSU_INC_TRB_PTR(trb, ep, stream_id, link_trb);
 
    if (PIPE_ISOC != HSU_GET_PIPE_TYPE(ep))
    {
      // Account for ZLPs as they were left as active TRBs even though HWO turned 0
      if (curr_req->requires_zlp)
      {
        HSU_INC_HW_TRB_PTR(ep->streams[stream_id].trb_hw_ptr, ep, stream_id, link_trb, ep->streams[stream_id].soft_trb_array.virt);
        HSU_INC_TRB_PTR(trb, ep, stream_id, link_trb);
      }
    }

    HSU_ULOG_2(NO_MSG, SNPS_LOG, "bytes_xferred %u, EP 0x%X", curr_req->bytes_transferred, ep->hw_addr);

    if (!req->single_request)
    {
      curr_req = LIST_NEXT(curr_req, next_request_in_list);
    }
    else
    {
      break;
    }
  }

  if (!rc && req->direction == DIRECTION_OUT && ep->pipe && (PIPE_ISOC != HSU_GET_PIPE_TYPE(ep))
      // HSU addition: SPS //
      #ifdef FEATURE_HS_USB_BAM
      && !req->sps_mode
      #endif // FEATURE_HS_USB_BAM //
     )
  {
    // For single request for OUT direction, SW only queues 1 TRB (max 16MB buffer).
    // SNPS HW reuqirement is that all RX buffer has to be multiples of MAX PACKET SIZE.  
    //
    // If the TRB's entire buffer is completed, SW has to queue another buffer 
    //   to receive more data within the transfer until it receives the short packet or ZLP.
    // If short packet or ZLP is received, then byte_not_xfered will be greater than 0.
    if (req->single_request && req->bytes_transferred < req->transfer_size
        && byte_not_xfered == 0)
    {
      rc = snps_dcd_start_next_transfer(ep);
      if (rc != HSU_JSUCCESS)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, failed_starting_next_transfer");
      }
      return rc;
    }
  }

  #ifdef CONFIG_DATA_PATH_DEBUG
  DBG_I(DSLAVE_DCD, ("DCD: Finished ep %d [%d] transfer %s size %d "
        "status %d done %d left %d req %p (%p)\n",
        ep->soft_addr, ep->hw_addr,
        req->direction == DIRECTION_IN ? "IN" : "OUT", 
        req->transfer_size, rc, req->bytes_transferred,
        req->transfer_size - req->bytes_transferred, req, req->complete));
  #endif // CONFIG_DATA_PATH_DEBUG

  LIST_FIRST(&req_list) = req;

  // Set the request's status appropriately
  LIST_FOREACH(curr_req, &req_list, next_request_in_list)
  {
    switch(curr_req->status)
    {
      case REQUEST_INPROGRESS:
        {
          if (!rc)
            curr_req->status = REQUEST_COMPLETED;
          else 
            curr_req->status = REQUEST_ERROR;
          req->completed_req++;
        }
        break;

      case REQUEST_CANCELLED:
        {
          req->completed_req++;
        }
        break;

      default:
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, request_status_incompatible_for_completion");
        hsu_event_log(EVT_INVALID_REQUEST_STATUS, ((core_t*)(ep->sc->core_ctx))->core_number);
    }
  }

  // Handle control transfer completion
  if (ep->soft_addr == 0)
  {
    if (ep0_error_detected)
    {
      snps_dcd_ctrl_sm_notify(ep, req, SNPS_CTRL_EVT_XFER_ERROR);
    }
    else
    {
      snps_dcd_ctrl_sm_notify(ep, req, SNPS_CTRL_EVT_XFER_COMPLETE);
    }
  }

  if (ep->abort_timeout_task && ep->timeout_task && req->timeout)
    jtask_stop(ep->timeout_task);

  if (ep->streams[stream_id].cur_req)
  {
    ep->abort_timeout_task = 1;
  }

  if (ep->pipe && (!ep->is_in || req->single_request || (PIPE_ISOC == HSU_GET_PIPE_TYPE(ep))))
  {
    // Remove current request from the queue
    STAILQ_REMOVE_HEAD(&ep->pipe->req_queue, next_request);
    if (request_completed)
    {
      if (ep->pipe->status != PIPE_ABORTING)
      {
        rc = snps_dcd_start_next_transfer(ep);
        if (rc != HSU_JSUCCESS)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, failed_starting_next_transfer");
          return rc;
        }
        //For ISOC, End the current xfer for generating a new xfer_nrdy event 
        if ((PIPE_ISOC == HSU_GET_PIPE_TYPE(ep)) && (ep->xfer_nrdy_elapsed == TRUE))
        {
          ep->xfer_in_progress = FALSE;
          rc = snps_dcd_abort_xfer(ep);
          if (rc != HSU_JSUCCESS)
          {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, abort_xfer_failed_for_ISOC");            
            return rc;
          }  
        }
      }
    }
    else
    {
      STAILQ_INSERT_HEAD(&ep->pipe->req_queue, ep->streams[stream_id].cur_req, next_request);
    }
  }

  // calling request callback
  if (req->complete)
  {
    // Invalidate the cache before calling the completion call back
    LIST_FIRST(&req_list) = req;
    LIST_FOREACH(curr_req, &req_list, next_request_in_list)
    {
      if (!ep->is_in && 
          // HSU addition: SPS
          #ifdef FEATURE_HS_USB_BAM
          !curr_req->sps_mode &&
          #endif// FEATURE_HS_USB_BAM
          // End of HSU addition
          curr_req->bytes_transferred)
      {
        j_cache_invalidate(curr_req->buffer.vaddr,
            curr_req->bytes_transferred);
      }
    }

    hsu_perf_log(PERF(SNPS_DCD_HANDLE_XFER_COMPLETION, Callback_request_complete),
        (uint32)(&curr_req));
    req->complete(req);
  }
 
  return rc;
}

/**
* Function name:  snps_dcd_handle_timeout
* Description: Handle EP timeout
* Parameters:
*     @ep: (IN) The EP descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
void snps_dcd_handle_timeout(void *arg)
{
  snps_dcd_ep_desc_t *ep = (snps_dcd_ep_desc_t*)arg;
  request_t *cur_req;
  jresult_t rc = HSU_JSUCCESS;
  juint16_t stream_id = SNPS_DCD_DEFAULT_STREAM; 
  jbool_t call_completion = 0;
  snps_dcd_trb *trb, *link_trb;

  HSU_CHK_EP_STREAM_ID(ep, stream_id);

  if (NULL == ep->streams)
  {
    return;
  }
  
  link_trb = ep->streams[stream_id].soft_trb_array.virt + ((ep->max_trbs) - 1);
  if (PIPE_ISOC == HSU_GET_PIPE_TYPE(ep))
  {
    if (ep->streams[stream_id].trb_hw_ptr >= link_trb)
    {
      ep->streams[stream_id].trb_hw_ptr = ep->streams[stream_id].soft_trb_array.virt;
    }
  
    // Check if all TRB's completed, 
    // else wait till EP finishes processing all TRBs.
    for (cur_req = ep->streams[stream_id].cur_req, trb = ep->streams[stream_id].trb_hw_ptr;
       trb < link_trb && cur_req; trb++)
    {
      call_completion = 1;
      
      if (trb->WORD3.HWO)
      {
          // If first TRB has HWO set then stop and skip call to xfer completion
          if (cur_req == ep->streams[stream_id].cur_req)
          {
              call_completion = 0;
              break;
          }
          else
          {
              cur_req->status = REQUEST_PENDING;
          }
      }
  
      if (!ep->streams[stream_id].cur_req->single_request)
        cur_req = LIST_NEXT(cur_req, next_request_in_list);
    }
  
    if (ep->streams[stream_id].cur_req && ep->streams[stream_id].cur_req->timeout)
    {
      rc = jtask_schedule(ep->timeout_task, ep->streams[stream_id].cur_req->timeout,
        snps_dcd_handle_timeout, ep);
      if (rc != HSU_JSUCCESS)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, error_scheduling_timeout_task");      
        return;
      }
    }
  
    if (call_completion)
    {
      ep->abort_timeout_task = 0;
      rc = snps_dcd_handle_xfer_completion(ep, SNPS_DCD_DEFAULT_STREAM);
      if (rc != HSU_JSUCCESS)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, error_in_snps_dcd_handle_xfer_completion");            
      }
    }
  }
  else
  {
    // Check if all TRB's completed, 
    // else wait till EP finishes processing all TRBs.
    for (cur_req = ep->streams[stream_id].cur_req, trb = ep->streams[stream_id].trb_hw_ptr;
       trb < link_trb && cur_req;)
    {
      call_completion = 1;
      
      if (trb->WORD3.HWO)
      {
        // If first TRB has HWO set then stop and skip call to xfer completion
        if (cur_req == ep->streams[stream_id].cur_req)
        {
          call_completion = 0;
          break;
        }
        else
        {
          cur_req->status = REQUEST_PENDING;
        }
      }
  
      HSU_INC_TRB_PTR(trb, ep, stream_id, link_trb);
      if (cur_req->requires_zlp)
      {
        if (trb->WORD3.HWO)
        {
          // If first TRB has HWO set then stop and skip call to xfer completion
          if (cur_req == ep->streams[stream_id].cur_req)
          {            
            HSU_ULOG_2(NO_MSG, SNPS_LOG, "first req ZLP_HWO=1, cur_req 0x%8X, EP 0x%X", (uint32)cur_req, ep->hw_addr);
            call_completion = 0;
            break;
          }
          else
          {            
            // if payload TRB HWO=0 and ZLP TRB HWO=1,
            // set the request PENDING and wait till ZLP is completed.
            HSU_ULOG_2(NO_MSG, SNPS_LOG, "req=PENDING ZLP_HWO=1, cur_req 0x%8X, EP 0x%X", (uint32)cur_req, ep->hw_addr);
            cur_req->status = REQUEST_PENDING;
          }
        }
        
        // Next request is after the ZLP TRB
        HSU_INC_TRB_PTR(trb, ep, stream_id, link_trb);
      }
  
      if (!ep->streams[stream_id].cur_req->single_request)
      {
        cur_req = LIST_NEXT(cur_req, next_request_in_list);
      }
      else
      {
        break;
      }
    }
  
    if (ep->streams[stream_id].cur_req && ep->streams[stream_id].cur_req->timeout)
    {
      rc = jtask_schedule(ep->timeout_task, ep->streams[stream_id].cur_req->timeout,
        snps_dcd_handle_timeout, ep);
      if (rc != HSU_JSUCCESS)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, error_scheduling_timeout_task");      
        return;
      }
    }
  
    if (call_completion)
    {
      ep->abort_timeout_task = 0;
      rc = snps_dcd_handle_xfer_completion(ep, SNPS_DCD_DEFAULT_STREAM);
      if (rc != HSU_JSUCCESS)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, error_in_snps_dcd_handle_xfer_completion");            
      }
    }
  }

}

static void snps_dcd_prepare_trb_for_infinite_transfer_mode(const snps_dcd_ep_desc_t* ep)
{
  /* 4.3.2 TRB Management */
  /* 1.	TRBs must be placed in system memory aligned to a 16-byte boundary – TRB address[3:0]=0. */

  /* Prepare First TRB */
  /* 2. TRB Buffer pointer low
   *    DBM_DATA_FIFO_ADDR_LSBn
   *    For LINK TRB, this field is relevant, and should point to a real TRB address */

  /* TODO: USB3.0 streams support: Take stream_id from request. */
  juint16_t stream_id = SNPS_DCD_DEFAULT_STREAM;

  uint32 dbm_ep_num = snps_dcd_get_dbm_ep_from_hw_ep(ep->hw_addr);
  uint32 phy_ep_num = ep->hw_addr;
  boolean is_dbm_producer = !ep->is_in;
  snps_dcd_trb *trb_ptr;
  snps_dcd_stream *stream;
  dcd_snps_eps_dbm_ctx_type *eps_dbm_ctx;
  uint32 i = 0;

  HSU_CHK_EP_STREAM_ID(ep, stream_id);
  trb_ptr = ep->streams[stream_id].trb_soft_ptr;
  stream = &(ep->streams[stream_id]);
  HSU_ASSERT(trb_ptr);

  if (dbm_ep_num > HWIO_DBM_EP_CFG_ADDRn_MAXn)
  {
    HSU_ERR_FATAL("dbm_ep_num out of range!", 0, 0, 0);
  }

  /* Double check for HWO */
  if (trb_ptr->WORD3.HWO)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, run_out_of_TRB_resource");          
    HSU_ERR_FATAL("The EP # should not be busy already", ep, 0, 0);
  }
  
  HSU_ULOG_1(NO_MSG, BAM_LOG, "trb_for_infinite_mode, dbm_em_num %u", dbm_ep_num);
  memset(trb_ptr, 0, sizeof(snps_dcd_trb));

//  trb_ptr->WORD0.BPTRL = HWIO_DBM_DATA_FIFO_ADDR_LSBn_PHYS(dbm_ep_num);
  eps_dbm_ctx= snps_dcd_get_snps_eps_dbm_ctx_hnd();
  trb_ptr->WORD0.BPTRL = (uint32)eps_dbm_ctx->dbm_to_usb_hw_ep_map[dbm_ep_num].data_fifo_addr;

  /* 3. TRB Buffer pointer high
   *   a.	DBM HW is using the address's MSBs for the monitoring of transfers related to DBM enabled Eps.  
   *   b.	SW should configure this field as defined in the table */
   /* bit	  field	    value	                              notes
    * 23:0	                                              As described in SNPS data book		
    * 28:24	EP #	    2..31	                              SW should set this field according to the physical EP num when preparing TRB  for DBM EP , otherwise this field should be 0
    * 29	  TRB DMA	‘ 1’this is a TRB read or write	      SW should set this bit to 1 when preparing TRB  for DBM EP, otherwise this field should be 0
    * 30	  obsolete		                                  SW should set this bit to 0.
    * 31	  DBM	‘     0’not a DBM manipulated address 
    * ‘               1’a DBM manipulated address For TRB or DATA DMAs
    *                                                     SW should set this field to 1 when preparing TRB  for DBM EP , otherwise this field should be 0
   */
  if (phy_ep_num >= HSU_MAX_NUM_OF_EP)
  {
    HSU_ERR_FATAL("phy_ep_num out of range!", 0, 0, 0);
  }
  trb_ptr->WORD1.BPTRH = 0;
  trb_ptr->WORD1.BPTRH = (phy_ep_num << HSU_DBM_BPTRH_EP_NUM_SHFT) |
                                  (1UL << HSU_DBM_BPTRH_TRB_DMA_SHFT) |
                                  (1UL << HSU_DBM_BPTRH_DBM_SHFT);

  /* 4.	BUFSIZ use 1K aligned BUFSIZ. 
   *   a.	Minimun BUFSIZ in SW TRB should be 2K, recommended 16K or largest expected transfer size. */
  /* Notes from email: 
       For DBM infinite transfers the SW TRB BUFSIZ is dummy and should be aligned for 1KB and greater than 2KB. For better performance it is recommended to use as big BUFSIZ as possible.
       This value isn’t the DBM_DATA_SIZE and you shouldn't allocate this buffer at all. */
  trb_ptr->WORD2.BUFSIZE = 0x4000;

  /* 5.	CHN - SW should set the CHN bit to 1 in the first NORNMAL TRB. */
  trb_ptr->WORD3.CHN = 1;

  /* 6.	LST - This bit should always be set to 0 for both DBM TRBs */
  trb_ptr->WORD3.LST = 0;

  /* 7.	CSP - This bit should be set to 0 for consumer (Torino: and 1 for producer) */
  trb_ptr->WORD3.CSP = (is_dbm_producer) ? 1 : 0;

  /* Setup other fields according to SNPS (not specified in HPG) */
  trb_ptr->WORD3.TRBCTL = TRB_CONTROL_NORMAL;
  trb_ptr->WORD3.ISP = 0;
  trb_ptr->WORD3.IOC = 0;


  /* Prepare Link TRB */
  trb_ptr++; /* move to Link TRB */

  // HW does not clear the HWO of the link TRB

  memset(trb_ptr, 0, sizeof(snps_dcd_trb));

  /* Set last TRB as Link TRB */
  /* 1.	TRBs must be placed in system memory aligned to a 16-byte boundary – TRB address[3:0]=0. */
  /* 2. For LINK TRB, this field is relevant, and should point to a real TRB address */
  trb_ptr->WORD0.BPTRL = (juint32_t)ep->streams[stream_id].soft_trb_array.phys;

  /* 3. TRB Buffer pointer high
   *   a.	DBM HW is using the address's MSBs for the monitoring of transfers related to DBM enabled Eps.  
   *   b.	SW should configure this field as defined in the table */
   /* bit	  field	    value	                              notes
    * 23:0	                                              As described in SNPS data book		
    * 28:24	EP #	    2..31	                              SW should set this field according to the physical EP num when preparing TRB  for DBM EP , otherwise this field should be 0
    * 29	  TRB DMA	‘ 1’this is a TRB read or write	      SW should set this bit to 1 when preparing TRB  for DBM EP, otherwise this field should be 0
    * 30	  obsolete		                                  SW should set this bit to 0.
    * 31	  DBM	‘     0’not a DBM manipulated address 
    * ‘               1’a DBM manipulated address For TRB or DATA DMAs
    *                                                     SW should set this field to 1 when preparing TRB  for DBM EP , otherwise this field should be 0
   */
  if (phy_ep_num >= HSU_MAX_NUM_OF_EP)
  {
    HSU_ERR_FATAL("dbm_ep_num out of range!", 0, 0, 0);
  }
  trb_ptr->WORD1.BPTRH = 0;
  trb_ptr->WORD1.BPTRH = (phy_ep_num << (uint32)HSU_DBM_BPTRH_EP_NUM_SHFT) |
                                  (1UL << (uint32)HSU_DBM_BPTRH_TRB_DMA_SHFT) |
                                  (1UL << (uint32)HSU_DBM_BPTRH_DBM_SHFT);

  trb_ptr->WORD2.BUFSIZE = 0;

  /* 5.	CHN - SW should set the CHN bit to 0 for LINK TRB. (not in HPG but in SNPS documents) */
  trb_ptr->WORD3.CHN = 0;

  /* 6.	LST - This bit should always be set to 0 for both DBM TRBs */
  trb_ptr->WORD3.LST = 0;

  /* 7.	CSP - This bit should be set to 0 for LINK TRB (not in HPG but in SNPS documents) */
  trb_ptr->WORD3.CSP = 0;

  /* Setup other fields according to SNPS (not specified in HPG) */
  trb_ptr->WORD3.TRBCTL = TRB_CONTROL_LINK;
  trb_ptr->WORD3.ISP = 0;
  trb_ptr->WORD3.IOC = 0;

  /* First available TRB point to start of array */
  stream->trb_soft_ptr = stream->trb_hw_ptr = stream->soft_trb_array.virt;
  stream->stream_initialized = TRUE;
  
  //Run through all TRBs and set HWO
  trb_ptr = ep->streams[stream_id].trb_soft_ptr;
  for (i = 0; i < SNPS_DCD_MAX_TRBS_FOR_INFINITE_MODE; i++)
  {
    trb_ptr->WORD3.HWO = 1;
    trb_ptr++;
  }

  /* Update trb_soft_ptr to point to next available TRB in list */
  ep->streams[stream_id].trb_soft_ptr++;

  ep->streams[stream_id].trb_soft_ptr++;
  // LINK TRBs are never counted as an active TRB associated with a request.  Do not increment.
  //  snps_dcd_inc_active_trb_cnt(&(ep->streams[stream_id]));

  /* Handle TRB wrap around */
  if (ep->streams[stream_id].trb_soft_ptr >= 
   ep->streams[stream_id].soft_trb_array.virt + (ep->max_trbs) - 1)
  {
    ep->streams[stream_id].trb_soft_ptr = ep->streams[stream_id].soft_trb_array.virt;
  }

  HSU_ULOG_4(NO_MSG, SNPS_LOG, 
    "queue_trb, trb_word0 0x%08X, trb_word1 0x%08X, trb_word2 0x%08X, trb_word3 0x%08X",
    *(juint32_t*)&trb_ptr->WORD0, *(juint32_t*)&trb_ptr->WORD1, *(juint32_t*)&trb_ptr->WORD2, *(juint32_t*)&trb_ptr->WORD3);

  HWIO_DBM_DIS_UPDXFER_OUTM(1 << dbm_ep_num, HWIO_DBM_DIS_UPDXFER_DIS_UPDXFER_EP0_DONT_BLOCK_UPDXFER_FVAL << dbm_ep_num);

  /* Additional requirements */
  /* For DBM IN transfers (Device transmits data to HOST) Short packets or ZLPs 
   *   might be created by the DBM when there is a BAM EOT.
   * In this senario the DBM creates a TRB with CHN=0 and
   *   dumps the current SW TRB and than continues to the next one.
   * The last TRB might be of size < 1K, a short packet. */

  /* CmdIOC must be set to 1 for STRTXFER and ENDXFER commands */
}


/* 4.4.1.7 DBM Reset */
/* The USB3 core may experience reset or similar to reset events, such as:
 * 1.	Soft reset sequence (see sequence in USB3 Synopsys databook, starts by writing DCTL.CSftRst)
 * 2.	EndXfer DepCmd
 * 3.	SetStall DepCmd
 * 4.	StartXfer DepCmd
 * 5.	USB bus reset
 * 6.	error scenarios
 * 7.	Bus disconnect */
/* NOTES: */
/* The DBM transfer is composed of 3 entities: the DBM EP, the USB EP and the BAM pipe. All 3 must be addressed when one of these entities is reset. */
/* The DBM will not monitor on these events and they will not trigger an internal  DBM reset in HW. */
/* In Torino: BAM pipe soft reset causes the corresponding DBM EP soft reset, unless disabled using DBM_EP_CFG.DBM_BAM_P_SW_RST_DISABLE. ==> WARNING: bit set to 0 */
/* The DBM will enable the USB3 SW to reset the entire DBM or specific DBM EPs (related to specific PIPEs) using DBM_SFT_RST register. */
/* The USB3 SW must reset the DBM EP and the BAM pipe that are related to a USB EP that experienced reset, as part of the USB3 SW reset negotiation with the relevant BAM Partner. */
/* The DBM will avoid toggling the BAM on DBM reset. */
/* NOTES */
/* Endxfer command for a usb ep that is DBM enabled, requires the related dbm ep and bam pipe to be reset too. */
/* There can't be a dbm ep sw reset without a pipe reset, or an Endxfer command without dbm ep sw reset and pipe reset. */
/* To end the DBM EP "endless" transfer, SW must wait until BAM is empty and not active and then give ENDXFER command. */
/* From Torino bam pipe reset causes dbm soft reset for corresponding dbm ep -- unless disabled using DBM_EP_CFG.DBM_BAM_P_SW_RST_DISABLE. */

/* applies to all endpoints */
void snps_dcd_dbm_reset(snps_dcd_soft_dc_t *sc)
{
  /* 1. Before generating DBM reset, SW should use Endxfer command for the USB EP. */
  //ENDXFER should have been done by abort ep
  uint32 i;
  dcd_snps_eps_dbm_ctx_type *eps_dbm_ctx = snps_dcd_get_snps_eps_dbm_ctx_hnd();
  
  HSU_ULOG_FUNC(BAM_LOG);
  eps_dbm_ctx->is_dbm_initialized = FALSE;

  for (i = 0; i <= HWIO_DBM_EP_CFG_ADDRn_MAXn; i++)
  {
    eps_dbm_ctx->is_dbm_ep_initialized[i] = FALSE;
  }

  /* 2. SW can set a general DBM reset or a DBM EP reset, by writing to DBM_SOFT_RESET register. */
  HWIO_OUTM(DBM_SOFT_RESET, HWIO_DBM_SOFT_RESET_DBM_SFT_RST_BMSK, 1UL << HWIO_DBM_SOFT_RESET_DBM_SFT_RST_SHFT);
  hsu_os_delay_us(1000);
  HWIO_OUTM(DBM_SOFT_RESET, HWIO_DBM_SOFT_RESET_DBM_SFT_RST_BMSK, 0UL << HWIO_DBM_SOFT_RESET_DBM_SFT_RST_SHFT);
  i = 0;
  while (0 != HWIO_IN(DBM_SOFT_RESET))
  {
    if (i > 100)
    {
      HSU_ERR_FATAL("DBM Soft Reset failed", 0, 0, 0);
    }
    i++;
  }

  /* 3.	SW should reset the BAM or the BAM PIPEs connected to the DBM EP that were reset. */
  //TODO AC notify the BAM state machine of a DBM EP reset such that it can invoke BAM pipe reset

  /* 4.	SW should re-initialize the DBM after DBM SW reset (or init the DBM EP configuration after DBM EP reset) */
  //This is to be invoked by client by design whenever necessary
  return;
}

/* 4.3.2.1 BAM SW */
/* Ensure the FIFO buffer calculations are aligned */
/* BAM_P_DATA_FIFO_ADDRn needs to be aligned to 64bit. */
/* BAM_P_DATA_FIFO_ADDRn [2:0] = 0x0 */

/* Recommendation over email */
/* If you want to use the sniffing”option you should configure the DBM before the BAM, otherwise the order between BAM and DBM is not important. */
/* i.	Program DBM based on [HPG 4.4.1.6.1 DBM Enable] */
/* ii.	Program individual DBM EPs based on [HPG 4.4.1.6.2 EP Init] */
/* iii.	Programming the BAM DESC FIFO and DATA FIFO via BAM APIs bam_pipe_init( ) */
/* iv.	Enable BAM via BAM APIs bam_pipe_enable( ) */
/* v.	Program the SNPS into “infinite mode” via [HPG 4.3.2 TRB Management] */

/* 4.3.3.1 DBM - Reception flow (Producer) */
/* When SW sets up data buffers before a host request: in this case the SW TRBs are ready before the 1'st DH arrived from the host: */
/* Step 1: SW sets up the SW TRBs and issues Start Transfer */
/* ... */
/* When SW sets up data buffers after the host request: in this case the SW TRBs are not ready before the 1'st DH arrived from the host */
/* Step 19: Seeing the XferNotReady event, SW sets up the SW TRBs and issues Start Transfer */


/* Uncatagorized Notes as part of DBM - Reception flow */
/* (1) command complete event – In response to the Start Transfer command, the hardware assigns this transfer a resource index number and returns the index in the DEPCMDn register and in the Command Complete event. This index must be used in subsequent Update and End Transfer commands. */
/* (2) if the TRB type is LINK, pass it as it is to the core. */
/* (3) the last normal HW TRB will hold the original TRB parameters (CHN,LST,IOC) */
/* (4) the SW can configure the DBM so that TRB WB is disabled for an EP – may be needed for use case 2 and 3 (networking and debug). */
/* (5) each EP will have an NDP BAM data fifo address (These will be configured by SW in the DBM and NDP BAM). */
/* (6,8) I assume bufsiz=64k for use case 1 (mass storage), 16k for use case 2 (networking) followed by a sw link trb, and 16k For use case 3 (core sight) */

/* 4.3.3.2 DMB - Transmission flow (Consumer) */
/* When SW sets up data buffers before a host request: in this case the SW TRBs are ready before the 1’st ACK arrived from the host: */
/* Step 1. SW sets up the SW TRBs and issues Start Transfer. */
/* When SW sets up data buffers after the host request: in this case the SW TRBs are not ready before the 1’st ACK TP arrived from the host: */
/* Step 19: Seeing the XferNotReady event, SW sets up the SW TRBs and issues Start Transfer. */

/* 4.3.3.3 BAM involvement in data flow */
/* NDP BAM manages the descriptor FIFO */
/* DBM manages the data FIFO */
/* 1.	DBM EP require minimum data fifo size that is 2K */

/* 4.4.1.4 General DBM initialization sequence */
/* 1. BAM configuration - set up the PIPE */
/* 2. Set DBM_EN bit in GENERAL_CFG register */
/* 3. In Torino, use external Pipe Memory.  But unsure how to config the RAM usage. */
/* 4. Init BAM_PIPE_CFG register: mapping between BAM pipe # and DBM ep #.  This pairing is done once per POR, and can't be modified till next POR
 *    Must set BAM pipe # and DBM EP # to be the same
 *    >>>>>>>>> This setting is obsolete on Torino.  The mapping is fixed. */
/* 5. Init DBM_GEVNTADR and DBM_GEVNTSIZ registers according to the USB core configuration */
/* 6. Init DBM_GEN_CFG according to the required SPEED (SS or HS) */
/* 7. If DBM EP is enabled, it can be disable using hard reset only. ===> WARNING: this statement is concerning and contradictory to 4.3.2.2 */
/* NOTE: From Torino use DBM_DATA_FIFO_ADDR_EN and DBM_DATA_FIFO_SIZE_EN for sniffing BAM access. */

/* 4.4.1.5 Initialization for each of the DBM EP's */
/* 1.	If not enabling the sniffing option (available from Aragorn V2) - init DBM_DATA_FIFO_ADDR and DBM_DATA_SIZE_ADDR registers */
/* 2.	configuring the DBM_EP_CFG_ADDR according to the required DBM configuration:
 * 	for QDSS mode SW should set the following fields: DBM_EN_EP, USB3_EPNUM, BAM_PIPE_NUM, DBM_DISABLE_WB, DBM_INT_RAM_ACC. 
 * 	Note: It is recommended to set the TXfifo size of a DBM EP big enough to support bursts, (this is a USB3 Synopsys Core configuration, using GTXFIFOSIZn)
 * 	make sure that USB30_DBM_REGFILE_DBM_EP_CFG_ADDRn bit 13 DBM_ZLT_DISABLE is set to 0. (DBM mode only work with ZLT after aligned data size transfer) */

/* 
4.5.1.1  Disable HS suspend before issue EP command
For Device mode only
Star 9000498951: When connected at 2.0 speeds, a suspend or disconnect may cause endpoint commands to hang 
Workaround – Prior to issuing one of the listed commands, set HS suspend enable bit GUSB2PHYCFG[6] to 0 and back to 1 after the command (HS only). 
Effected Commands are EPXFERCFG, EPCFG, EndXfer, StartXfer, SetStall, ClearStall, SetNRDY */

