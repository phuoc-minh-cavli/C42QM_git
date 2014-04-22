/*
===========================================================================

FILE:         dcd_snps_eps.c

DESCRIPTION: This file handles End Point configuration utilities for the 
             Device Controller Driver for SNPS USB3.0 core. 

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/src/jslave/dcd/dcd_snps_eps.c#1 $

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


#define SNPS_DCD_TX_FIFO_ENTRY_SIZE  (16)
static const uint16 snps_dcd_total_tx_fifo_depth = 0xA65; // Total used for error checking
static uint16 snps_dcd_tx_fifo_depth[SNPS_DCD_TX_FIFO_ENTRY_SIZE] =
{
  0x42,    // 0
  0x184,   // 1
  0x82,    // 2
  0x82,    // 3
  0x82,    // 4
  0x82,    // 5
  0x82,    // 6
  0x287,   // 7
  0x82,    // 8
  0x82,    // 9
  0x82,    // 10
  0x82,    // 11
  0x82,    // 12
  0x82,    // 13
  0x82,    // 14
  0x00,    // 15
};
static uint16 snps_dcd_tx_fifo_offset[SNPS_DCD_TX_FIFO_ENTRY_SIZE];


/*==============================================================================
Internal Function Definitions
==============================================================================*/

/*========================== EP configuration utils ===========================*/

/**
* Function name:  snps_dcd_get_ep_desc
* Description: Search and return the snps_dcd_ep_desc_t entry that corresponds 
to the EP with the given USB address
* Parameters: 
*     @sc: (IN) snps_dcd software context
*     @ep: (IN) EP USB address
*
* Return value: Pointer to snps_dcd_ep_desc_t entry
* Scope: global
**/
snps_dcd_ep_desc_t* snps_dcd_get_ep_desc(snps_dcd_soft_dc_t *sc, juint8_t usb_ep_address)
{
  juint8_t soft_address = usb_ep_address & ~USB_DIR_IN;
  snps_dcd_ep_desc_t* ep;

  if (usb_ep_address & USB_DIR_IN)
  {
    /* IN EPs maps to odd physical addresses. */
    ep = &sc->ep_desc_table[soft_address*2 + 1];
  }
  else
  {
    /* OUT EPs maps to even physical addresses. */
    ep = &sc->ep_desc_table[soft_address*2];
  }

  return ep;
}

/**
* Function name:  snps_dcd_init_stream_trbs
* Description: Initialize TRB array for a given stream
* Parameters: 
*     @ep: (IN) Endpoint descriptor
*     @stream: (IN) Stream
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_init_stream_trbs(snps_dcd_ep_desc_t *ep, juint16_t stream_id)
{
  juint16_t i; 
  snps_dcd_trb* curr_trb;
  uint8 dbm_ep_num;
  snps_dcd_stream* stream;
  
  hsu_perf_log( PERF( SNPS_DCD_INIT_STREAM_TRBS, Entry), 0);

  HSU_CHK_EP_STREAM_ID(ep, stream_id);
  stream = &(ep->streams[stream_id]);

  if ((PIPE_ISOC == HSU_GET_PIPE_TYPE(ep)) && (stream->stream_initialized == TRUE))
  {
    hsu_perf_log( PERF( SNPS_DCD_INIT_STREAM_TRBS, Skipped), 0);   
    return HSU_JSUCCESS;
  }

  for (i=0, curr_trb = stream->soft_trb_array.virt; 
      i<ep->max_trbs-1; 
      curr_trb++, i++)
  {
    /* TRB must be initialized as SW controlled */
    curr_trb->WORD3.HWO = 0;
  }

  #ifdef FEATURE_HS_USB_BAM
  /* Only set LINK TRB as last TRB when it's not HW accelerated endpoint */
  /* Read/Write Infinite will take care of the LINK TRB properly */
  dbm_ep_num = snps_dcd_get_dbm_ep_from_hw_ep(ep->hw_addr);
  if (HSU_UNDEFINED_EP_VAL == dbm_ep_num)
  #endif /* FEATURE_HS_USB_BAM  */
  {
    /* Set last TRB as Link TRB */
    curr_trb->WORD3.HWO = 1;
    curr_trb->WORD0.BPTRL = (juint32_t)stream->soft_trb_array.phys;
    curr_trb->WORD1.BPTRH = 0x0;
    curr_trb->WORD2.BUFSIZE = 0;
    curr_trb->WORD3.TRBCTL = TRB_CONTROL_LINK;
    curr_trb->WORD3.LST = 0x0;
    curr_trb->WORD3.CHN = 0x0;
    curr_trb->WORD3.IOC = 0x0;
    curr_trb->WORD3.ISP = 0x0;
    curr_trb->WORD3.CSP = 0x0;
    curr_trb->WORD3.StrmID_SOFNum = 0x0;
  }

  /* First available TRB point to start of array */
  stream->trb_soft_ptr = stream->trb_hw_ptr = stream->soft_trb_array.virt;
  stream->stream_initialized = TRUE;

  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_init_ep_streams
* Description: Initialize EP streams array.
* Parameters: 
*     @ep: (IN) Endpoint descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_init_ep_streams(snps_dcd_ep_desc_t *ep)
{
  juint16_t i;
  jresult_t rc = HSU_JSUCCESS;

  /* Allocate array of streams */
  ep->streams = (snps_dcd_stream*)
    jmalloc(sizeof(snps_dcd_stream) * ep->max_streams, M_ZERO);
  if (!ep->streams)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, error_allocat_ep_stream");
    return JENOMEM;
  }

  hsu_trace_ep_set(ep->hw_addr);
  hsu_trace_inf_set(ep->max_streams);

  /* Initialize each steam structure */
  for (i = 0; i < ep->max_streams; i++)
  {
    ep->streams[i].cur_req = NULL;

    hsu_trace_msg1_set(SNPS_DCD_INIT_EP_STREAMS__TRC);

    //Audio FD queues multiple requests which requires multiple TRB's ( 20 TRB's for each )
    if (PIPE_ISOC == HSU_GET_PIPE_TYPE(ep))
    {
        ep->max_trbs = SNPS_DCD_MAX_AUDIO_TRBS_PER_STREAM;
        ep->xfer_in_progress = 0;
    }    

    if ((PIPE_BULK == HSU_GET_PIPE_TYPE(ep)) && (ep->is_in == TRUE))
    {
      ep->max_trbs = SNPS_DCD_MAX_BULK_IN_TRBS_PER_STREAM;
    } 

    /* Pre-allocate TRB array for the stream */
    rc = jdma_alloc(sizeof(snps_dcd_trb) * ep->max_trbs, 
      sizeof(snps_dcd_trb), (void **)&(ep->streams[i].soft_trb_array.virt), 
      (void **)&(ep->streams[i].soft_trb_array.phys), M_ZERO, &ep->streams[i].soft_trb_array.cookie);

    hsu_trace_msg1_clear();
    
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, allocation_of_TRBs_for_stream_failed");      
      return rc;
    }
    ep->streams[i].stream_initialized = FALSE;
  }

  hsu_trace_ep_clear();
  hsu_trace_inf_clear();

  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_uninit_ep_streams
* Description: Un-initialize EP streams array.
* Parameters: 
*     @ep: (IN) Endpoint descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_uninit_ep_streams(snps_dcd_ep_desc_t *ep)
{
  juint16_t i;
  
  HSU_ULOG_1(NO_MSG, BAM_LOG, "ep 0x%08X", (uint32)ep);
  /* Free memory for each stream structure */
  if (ep->streams)
  {
    for (i = 0; i < ep->max_streams; i++)
    {
      if (ep->streams[i].soft_trb_array.cookie)
      {
        jdma_free(ep->streams[i].soft_trb_array.cookie);
        ep->streams[i].soft_trb_array.cookie = NULL;
      }
    }

    /* Free the array of streams */
    jfree(ep->streams);
    ep->streams = NULL;
  }
  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_init_endpoints
* Description: Initialize EP descriptor table according to the controller
*              capabilities (EP - hardware wise, PIPE - driver wise)
* Parameters: 
*     @sc: (IN) snps_dcd software context
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_init_endpoints(snps_dcd_soft_dc_t *sc)
{
  jint_t i;
  juint8_t ep_out_index=0, ep_in_index=0;
  jresult_t rc = HSU_JSUCCESS;

  /* Allocate EP descriptor table */
  sc->ep_desc_table = (snps_dcd_ep_desc_t *)
    jmalloc(sizeof(snps_dcd_ep_desc_t) * sc->max_available_eps, M_ZERO);
  if (!sc->ep_desc_table)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, error_allocat_ep_descriptors_array");
    return JENOMEM;
  }

  /* Set up parameters for each EP */
  for (i = 0 ; i < sc->max_available_eps; i++)
  {
    snps_dcd_ep_desc_t* ep = &sc->ep_desc_table[i];

    ep->hw_addr = i;
    ep->sc = sc;
    ep->max_trbs = SNPS_DCD_MAX_TRBS_PER_STREAM;
    ep->type_map = (i < 2) ? SNPS_DCD_TYPE_CTRL : SNPS_DCD_TYPE_ALL;

    /* EP mapping is not flexible. IN EPs maps to odd physical addresses, OUT 
       EPs maps to even physical addresses. */
    if (i % 2)
    {
      /* EPn IN */
      ep->max_trbs = SNPS_DCD_MAX_IN_TRBS_PER_STREAM;
      ep->soft_addr = ep_in_index;
      ep->fifo_number = ep_in_index;
      ep->is_in = TRUE;
      ep_in_index++;
    }
    else
    {
      /* EPn OUT */

      // Enable option to use multiple TRBs for OUT EPs except for Control EP.
      if (ep->type_map != SNPS_DCD_TYPE_CTRL)
      {
        ep->max_trbs = SNPS_DCD_MAX_OUT_TRBS_PER_STREAM;
      }

      ep->soft_addr = ep_out_index;
      ep->fifo_number = 0;
      ep_out_index++;
      ep->is_in = FALSE;
    }
  }

  return rc;
}

/**
* Function name:  snps_dcd_config_ep_cfg
* Description: Program the controller with specific EP configuration
* Parameters:
*     @ep: (IN) The EP descriptor
*     @ignore_seq: (IN) Don't reset EP sequence number / Toggle
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_config_ep_cfg(snps_dcd_ep_desc_t *ep, jbool_t ignore_seq, jbool_t is_modify_cfg_action)
{
  juint8_t address;
  boolean direction; 
  snps_dcd_soft_dc_t *sc;
  jresult_t rc = HSU_JSUCCESS;

  snps_dcd_DEPCMD_P1 p1;
  snps_dcd_DEPCMD_P0 p0;
  juint32_t DEPCMD;

  KASSERT(ep != NULL, ("ep is NULL,\n"));
  address = ep->soft_addr;
  direction = ep->is_in; 
  sc = ep->sc;

  HSU_ULOG_1(NO_MSG, SNPS_LOG, "EP 0x%X", ep->hw_addr);

  /* Create DEPCFG command with required EP configuration */
  p1.value = 0;
  p1.DEPCFG.LEP = address;
  p1.DEPCFG.EpDir = direction ? 1 : 0;
  p1.DEPCFG.StrmCap = (ep->max_streams > 1) ? 1 : 0;
  p1.DEPCFG.StreamEvtEn = 0;

  if (PIPE_ISOC == HSU_GET_PIPE_TYPE(ep))
  {
    p1.DEPCFG.bInterval_m1 = ep->pipe->poll_interval_high - 1;
  }
  else if(ep->pipe && ep->pipe->rate_u.nak_rate)
  {
    p1.DEPCFG.bInterval_m1 = ep->pipe->rate_u.nak_rate - 1;
  }
  else
  {
    p1.DEPCFG.bInterval_m1 = 0;
  }

// TODO : FEATURE_HS_USB_PRE_SILICON_BU
//  p1.DEPCFG.RxTxfifoEvtEn = 1; 
//  p1.DEPCFG.RxTxfifoEvtEn = 0; 

  // Enable XferNotReady for Control EPs.
  // SNPS databook: "The XferNotReady event must not be disabled for control endpoints because the event 
  // is an integral part of control transfer handling."
  //ISOC EPs require xferNotReady for getting the current  frame number and scheduling next request on future interval
  if(ep->soft_addr == 0 || (PIPE_ISOC == HSU_GET_PIPE_TYPE(ep)))
  {
    p1.DEPCFG.XferNRdyEn = 1;
  }

  p1.DEPCFG.XferInProgEn = 1;
  p1.DEPCFG.XferCmplEn = 1;
  p1.DEPCFG.IntrNum = SNPS_DCD_DEFAULT_EVT_BUF_ID;
  p0.value = 0;
  p0.DEPCFG.BrstSiz = ep->max_burst;
  p0.DEPCFG.FIFONum = ep->fifo_number;
  p0.DEPCFG.CfgAction = 0;

  if (PIPE_CONTROL == HSU_GET_PIPE_TYPE(ep))
  {
    if (is_modify_cfg_action)
    {
      p0.DEPCFG.CfgAction = 2; // Modify endpoint
    }
  }
  p0.DEPCFG.MPS = ep->packet_size;
  p0.DEPCFG.EPType = (NULL == ep->pipe || 0 == address) ? 0 : ep->pipe->type;
// TODO : FEATURE_HS_USB_PRE_SILICON_BU
//  p0.DEPCFG.IgnoreSeqNum = ignore_seq; 9x35VBU

  /* Issue DEPCFG command */
  DEPCMD = 0;
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
  DEPCMD = HSU_VAR_OUTM(DEPCMD, SNPS_DCD_DEPCMD_CMDTYPE_SETEPCONF, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);

  rc = snps_dcd_write_ep_command(sc, ep->hw_addr,
    DEPCMD, p0.value, p1.value, 0, NULL);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_write_ep_cmd_failed");
    return rc;
  }

  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_config_ep_xfer
* Description: Program the controller with specific EP configuration
* Parameters:
*     @ep: (IN) The EP descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_config_ep_xfer(snps_dcd_ep_desc_t *ep)
{
  snps_dcd_soft_dc_t *sc = ep->sc;
  jresult_t rc = HSU_JSUCCESS;

  snps_dcd_DEPCMD_P0 p0;
  juint32_t DEPCMD;

  HSU_ULOG_1(NO_MSG, SNPS_LOG, "EP 0x%X", ep->hw_addr);

  /* Create DEPXFERCFG command with required number of EP transfer resources */
  p0.value = 0;
  p0.DEPXFERCFG.NumXferRes = ep->max_streams;

  /* Issue DEPXFERCFG command */
  DEPCMD = 0;
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
  DEPCMD = HSU_VAR_OUTM(DEPCMD, SNPS_DCD_DEPCMD_CMDTYPE_SETEPXFERRES, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);
  rc = snps_dcd_write_ep_command(sc, ep->hw_addr,
    DEPCMD, p0.value, 0, 0, NULL);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_write_ep_cmd_failed");
    return rc;
  }

  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_config_tx_fifo_size
* Description: Adjust the GTXFIFOSIZ for each endpoint
* Parameters:
*     @ep: (IN) The EP descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_config_tx_fifo_size(snps_dcd_ep_desc_t *ep)
{
  uint32 i;
  uint32 tx_fifo_offset;
  uint32 tx_fifo_depth;
  static boolean run_once = FALSE;

  // Validate snps_dcd_tx_fifo_depth against max size
  if (FALSE == run_once)
  {
    run_once = TRUE;
    snps_dcd_tx_fifo_offset[0] = 0;
    for (i = 1; i < SNPS_DCD_TX_FIFO_ENTRY_SIZE; i++)
    {
      snps_dcd_tx_fifo_offset[i] = snps_dcd_tx_fifo_offset[i-1] + snps_dcd_tx_fifo_depth[i-1];
    }
    if (snps_dcd_total_tx_fifo_depth < snps_dcd_tx_fifo_offset[SNPS_DCD_TX_FIFO_ENTRY_SIZE-1])
    {
      HSU_ERR_FATAL("tx_fifo_offset exceeded max tx fifo depth", 0, 0, 0);
    }
  }

  if (ep->is_in)
  {
#if 0 // TODO: ----- Akash ------
    uint32 addr = 0;
    uint32 val = 0;
    tx_fifo_offset = snps_dcd_tx_fifo_offset[ep->soft_addr];
    tx_fifo_depth = snps_dcd_tx_fifo_depth[ep->soft_addr];

    switch (ep->soft_addr)
    {
      case 0: addr = HWIO_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ0_ADDR(0); break;
      case 1: addr = HWIO_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ1_ADDR(0); break;
      case 2: addr = HWIO_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ2_ADDR(0); break;
      case 3: addr = HWIO_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ3_ADDR(0); break;
      case 4: addr = HWIO_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ4_ADDR(0); break;
      case 5: addr = HWIO_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ5_ADDR(0); break;
      case 6: addr = HWIO_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ6_ADDR(0); break;
      case 7: addr = HWIO_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ7_ADDR(0); break;
      default:
        HSU_ERR_FATAL("Undefined soft_addr %d", ep->soft_addr, 0, 0);
        return EUNKNOWN;
    }

    if (addr)
    {
      val =   ((tx_fifo_offset << HWIO_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ0_TXFSTADDR_N_SHFT) & HWIO_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ0_TXFSTADDR_N_BMSK)
            | ((tx_fifo_depth << HWIO_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ0_TXFDEP_N_SHFT) & HWIO_GTXFIFOSIZ_REGS_p_GTXFIFOSIZ0_TXFDEP_N_BMSK);
      hsu_out_dword(addr, val);
    }
#endif
  }
  // else // OUT ENDPOINT, POR programming already correct

  return HSU_JSUCCESS;
}


/**
* Function name:  snps_dcd_config_ep
* Description: Program the controller with specific EP configuration
* Parameters:
*     @ep: (IN) The EP descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_config_ep(snps_dcd_ep_desc_t *ep)
{
  jresult_t rc = HSU_JSUCCESS;

  HSU_ULOG_1(NO_MSG, SNPS_LOG, "EP 0x%X", ep->hw_addr);

  rc = snps_dcd_config_ep_cfg(ep, FALSE, FALSE);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_config_ep_cfg_failed");
    return rc;
  }

  rc = snps_dcd_config_ep_xfer(ep);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_config_ep_xfer_failed");
    return rc;
  }

#ifdef FEATURE_HS_USB_BURST_MODE
  // Setup GTXFIFOSIZ for each endpoint
  rc = snps_dcd_config_tx_fifo_size(ep);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_config_tx_fifo_size failed");
    return rc;
  }
#endif // FEATURE_HS_USB_BURST_MODE
  return HSU_JSUCCESS;
}


/**
* Function name:  snps_dcd_dalepena_ep
* Description: Endpoint enablement
* Parameters:
*     @ep: (IN) The EP descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_dalepena_ep(snps_dcd_ep_desc_t *ep)
{
  juint32_t value = 0x0; 

  /* Enable the EP */
  value = HWIO_DALEPENA_IN;
  value &= ~(0x0);
  value |= (1UL << ep->hw_addr);
  HWIO_DALEPENA_OUT(value);
  HSU_ULOG_1(NO_MSG, BAM_LOG, "dalepna_set, EP 0x%X", ep->hw_addr);

  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_unconfig_ep
* Description: Disable EP configuration
* Parameters:
*     @ep: (IN) The EP descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_unconfig_ep(snps_dcd_ep_desc_t *ep)
{
  juint32_t value= 0x0; 
  HSU_ULOG_1(NO_MSG, SNPS_LOG, "EP 0x%X", ep->hw_addr);

  /* Disable the EP */
  value = HWIO_DALEPENA_IN;
  value &= ~((1UL << ep->hw_addr));
  value |= (0x0); 
  HWIO_DALEPENA_OUT(value);
  HSU_ULOG_1(NO_MSG, BAM_LOG, "dalepna_clr, EP 0x%X", ep->hw_addr);
  return HSU_JSUCCESS;
}


/**
* Function name:  snps_dcd_enable_ep0
* Description: Program the controller to enable EP0 
* Parameters:
*     @sc: (IN) The DCD SW context
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_enable_ep0(snps_dcd_soft_dc_t* sc)
{
  snps_dcd_ep_desc_t* ep0_out = &sc->ep_desc_table[SNPS_DCD_EP0_OUT_ADDR];
  snps_dcd_ep_desc_t* ep0_in = &sc->ep_desc_table[SNPS_DCD_EP0_IN_ADDR];
  request_t* ep0_request = &sc->ep0_request;
  buffer_t* ep0_buffer = &ep0_request->buffer;
  jresult_t rc = HSU_JSUCCESS;

  /* Update packet size EP parameter according to speed */
  ep0_out->packet_size = core_get_max_packet_size(PIPE_CONTROL, sc->speed);
  ep0_in->packet_size = core_get_max_packet_size(PIPE_CONTROL, sc->speed);

  /* Init Super-Speed parameters */
  ep0_out->max_streams = 1; 
  ep0_out->max_burst   = 0;
  ep0_out->mult        = 0;
  ep0_in->max_streams  = 1; 
  ep0_in->max_burst    = 0;
  ep0_in->mult         = 0;

  rc = snps_dcd_init_ep_streams(ep0_out);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, init_EP_OUT_streams_failed");
    return rc;
  }

  rc = snps_dcd_init_ep_streams(ep0_in);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, init_EP_IN_streams_failed");    
    return rc;
  }

  /* Reset resources counter inside the controller to 0. */
  if (snps_dcd_config_device_resources(sc, SNPS_DCD_RES_NUM_CONNECTED))
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, config_device_resource_failed");
    return EUNKNOWN;
  }

  /* Program the controller with EP0 configuration */
  rc = snps_dcd_config_ep(ep0_out);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, config_ep_out_failed");
    return rc;
  }

  rc = snps_dcd_config_ep(ep0_in);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, config_ep_in_failed");
    return rc;
  }

  hsu_trace_msg1_set(SNPS_DCD_ENABLE_EP0__TRC);

  /* Allocate a single buffer for control requests */
  ep0_buffer->buffer_size = USB_CONTROL_REQUEST_SIZE;
  rc = jdma_alloc(ep0_buffer->buffer_size, 2,
    &ep0_buffer->vaddr, &ep0_buffer->dma_addr,
    0, &ep0_buffer->cookie);

  hsu_trace_msg1_clear();
  
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, failed_allocate_buffer_for_req, size %1uByte",
      ep0_buffer->buffer_size);
    (void)snps_usb3_dcd_detach(sc->dev);
    return rc;
  }

  /* Update EP0 request */
  ep0_request->is_buffer_allocated = 1;
  ep0_request->completed_req = 0;
  ep0_request->timeout = 0;
  ep0_request->single_request = 1;
  LIST_NEXT(ep0_request, next_request_in_list) = NULL;

  // Start Control State Machine
  sc->ctrl_pending_request = NULL;
  sc->xfer_nrdy_rcvd = FALSE;

  rc = snps_dcd_send_setup_request(sc);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_send_setup_request_failed");    
    return rc;
  }

  snps_dcd_dalepena_ep(ep0_out);
  snps_dcd_dalepena_ep(ep0_in);

  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_disable_ep0
* Description: Disable EP0
* Parameters:
*     @sc: (IN) The DCD SW context
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_disable_ep0(snps_dcd_soft_dc_t* sc)
{
  snps_dcd_ep_desc_t* ep0_out = &sc->ep_desc_table[SNPS_DCD_EP0_OUT_ADDR];
  snps_dcd_ep_desc_t* ep0_in = &sc->ep_desc_table[SNPS_DCD_EP0_IN_ADDR];
  jresult_t rc = HSU_JSUCCESS;

  /* Abort transfer on EP0 */
  rc =  snps_dcd_abort_ep0(sc);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, abort_EP0_failed");      
    return rc;
  }

  /* Unconfig EP0 */
  rc =  snps_dcd_unconfig_ep(ep0_out);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, unconfig_EP0_OUT_failed");     
    return rc;
  }
  
  rc =  snps_dcd_unconfig_ep(ep0_in);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, unconfig_EP0_IN_failed");     
    return rc;
  }

  /* Free EP0 buffer */
  if (sc->ep0_request.buffer.cookie)
  {
    jdma_free(sc->ep0_request.buffer.cookie);
    sc->ep0_request.buffer.cookie = NULL;
  }

  /* Uninit streams for EP0 */
  rc = snps_dcd_uninit_ep_streams(ep0_out);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, uninit_EP0_OUT_stream_failed");     
    return rc;
  }

  rc = snps_dcd_uninit_ep_streams(ep0_in);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, uninit_EP0_IN_stream_failed");
    return rc;
  }

  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_stall_ep
* Description: Handle the event of transfer completion 
* Parameters:
*     @ep: (IN) The EP descriptor
*     @flag: (IN) TRUE for stall, FALSE for remove stall
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_stall_ep(snps_dcd_ep_desc_t *ep, jbool_t flag)
{
  juint32_t DEPCMD;
  juint32_t DEPCMD_CmdTyp;
  jresult_t rc;

  if ((FALSE == flag) && (NULL == ep->pipe))
  {
    DBG_W(DSLAVE_DCD, ("snps_dcd_stall_ep: ep 0x%xcan not be cleared\n", ep->hw_addr));
    return HSU_JSUCCESS;
  }

  /* Build and issue a SETSTALL command with required flag */
  DEPCMD = 0;
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
  DEPCMD_CmdTyp = (flag) ? SNPS_DCD_DEPCMD_CMDTYPE_SETSTALL : 
  SNPS_DCD_DEPCMD_CMDTYPE_CLEARSTALL;
  DEPCMD = HSU_VAR_OUTM(DEPCMD, DEPCMD_CmdTyp, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);
   
  rc = snps_dcd_write_ep_command(ep->sc, ep->hw_addr, DEPCMD, 0, 0, 0, NULL);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, writing_EP_cmd_failed");         
    return rc;
  }

  if (ep->pipe)
  {
    ep->pipe->status = flag ? PIPE_STALLLED : PIPE_ENABLED;
  }

  if (flag)
  {
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, set_stall, EP 0x%X", (uint32) ep->hw_addr);         
  }
  else
  {
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, clear_stall, EP 0x%X", (uint32) ep->hw_addr);         
  }
  return rc;
}

/**
* Function name:  snps_dcd_handle_fifo_overrun_underrun
* Description: Handle the event of FIFO overrun
* Parameters:
*     @ep: (IN) The EP descriptor
*     @stream_id: (IN) The stream ID
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_handle_fifo_overrun_underrun(snps_dcd_ep_desc_t* ep, juint16_t stream_id)
{
  // Crash the system, rather than graceful Error handling of dequeueing the 
  // transfer 
  HSU_ERR_FATAL("snps_dcd_handle_fifo_overrun_underrun", 0, 0, 0);

  return HSU_JSUCCESS;
}


/**
* Function name:  snps_dcd_usb_gsi_endpoint_config
* Description: 
* Parameters:

*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
void snps_dcd_usb_gsi_endpoint_config(jdevice_t dev, pipe_desc_t *pipe, juint32_t gsi_interrupter_idx)
{
  juint8_t address;
  boolean direction; 
  snps_dcd_soft_dc_t *sc;
  snps_dcd_ep_desc_t *ep;
  jresult_t rc;

  snps_dcd_DEPCMD_P1 p1;
  snps_dcd_DEPCMD_P0 p0;
  juint32_t DEPCMD;
  HSU_ULOG_FUNC(BAM_LOG);

  // Convert pipe to ep and implement entirety of snps_dcd_ops_enable_ep() for hardware accelerated initializations
  // Section adapted from snps_dcd_ops_enable_ep( )

  if ((NULL == pipe) || (NULL == pipe->dcd_handle))
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_usb_gsi_endpoint_config, bad_pipe");
    return;
  }
  
  ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;
  ep->pipe = pipe;

  if ((NULL == pipe) || (NULL == pipe->dcd_handle))
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_usb_gsi_endpoint_config, bad_pipe");
    return;
  }

  KASSERT(ep != NULL, ("ep is NULL,\n"));
  address = ep->soft_addr;
  direction = ep->is_in; 
  sc = ep->sc;
  HSU_ULOG_1(NO_MSG, SNPS_LOG, "snps_dcd_usb_gsi_endpoint_config, EP 0x%X", ep->hw_addr);

  /* Create DEPCFG command with required EP configuration */
  p1.value = 0;
  p1.DEPCFG.LEP = address;
  p1.DEPCFG.EpDir = direction ? 1 : 0;
  p1.DEPCFG.StrmCap = (ep->max_streams > 1) ? 1 : 0;
  p1.DEPCFG.StreamEvtEn = 0;

  if (ep->pipe && ep->pipe->rate_u.nak_rate)
  {
    p1.DEPCFG.bInterval_m1 = ep->pipe->rate_u.nak_rate - 1;
  }
  else
  {
    p1.DEPCFG.bInterval_m1 = 0;
  }

  p1.DEPCFG.XferInProgEn = 1;
  p1.DEPCFG.XferNRdyEn = 0; // Specified by the HPG.  Disable XferNotReady
  p1.DEPCFG.XferCmplEn = 1;

  p1.DEPCFG.reserved2 = 1;

  // Configure interrupt number based on range 1 to 3 (inclusive)
  // Value of the gsi channel index is decided by the state machine
  p1.DEPCFG.IntrNum = gsi_interrupter_idx;

  p0.value = 0;
  p0.DEPCFG.BrstSiz = ep->max_burst;
  p0.DEPCFG.FIFONum = ep->fifo_number;
  p0.DEPCFG.MPS = ep->packet_size;
  p0.DEPCFG.EPType = (NULL == ep->pipe || 0 == address) ? 0 : ep->pipe->type;
  p0.DEPCFG.CfgAction = 0;

  /* Issue DEPCFG command */
  DEPCMD = 0;
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
  DEPCMD = HSU_VAR_OUTM(DEPCMD, SNPS_DCD_DEPCMD_CMDTYPE_SETEPCONF, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);

  rc = snps_dcd_write_ep_command(sc, ep->hw_addr,
      DEPCMD, p0.value, p1.value, 0, NULL);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_config_ep, snps_dcd_write_ep_cmd_failed");
    return;
  }

  // DEPXFERCFG
  snps_dcd_config_ep_xfer(ep);

#ifdef FEATURE_HS_USB_BURST_MODE
    // Setup GTXFIFOSIZ for each endpoint
    rc = snps_dcd_config_tx_fifo_size(ep);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_config_tx_fifo_size failed");
      return;
    }
#endif // FEATURE_HS_USB_BURST_MODE

  // DALEPENA
  snps_dcd_dalepena_ep(ep);

  pipe->status = PIPE_ENABLED;

  return;
}



/**
* Function name:  snps_dcd_round_to_nearest_power_of_two
* Description: Calculate the alignment requirement
* Parameters: 
*     @ep: (IN) Endpoint descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
static jresult_t snps_dcd_round_to_nearest_power_of_two(juint32_t input_val, juint32_t *output_val)
{
  // Start with an arbitrary higher value to reduce calculation time
  uint32 power_of_two = 64;
  while (input_val > power_of_two)
  {
    power_of_two *= 2;
    if (0x8000 < power_of_two)
    {
      ERR_FATAL("snps_dcd_round_to_nearest_power_of_two: TRB ring buffer should be smaller", input_val, 0, 0);
    }
  }
  *output_val = power_of_two;
  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_ops_init_ep_trbs_hw_accel
* Description: 
* Parameters: 
*     @ep: (IN) Endpoint descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_ops_init_ep_trbs_hw_accel(jdevice_t dev, pipe_desc_t *pipe, 
  void **trb_ring_cookie, void **trb_ring_va_addr, void **trb_ring_pa_addr, juint32_t n)
{
  jresult_t rc = HSU_JSUCCESS;
  juint32_t size_to_alloc;
  juint32_t alignment;
  // TRB allocation
  // UL/OUT channel: n+1 TRB (n buffers + 1 Link) with n = 8
  //   Total TRBs needed: 8 + 1 = 9
  // DL/IN Channel: 2n+1+1 TRB (n+1 ZLP, n buffers, 1 Link) with n = 64
  //   Total TRBs needed: 2 * 64 + 1 + 1 = 130

  // Note: Ring base address must be aligned to ring length (RLEN) rounded to power of two.
  //   E.g., if RLEN=12KB, RBASE must be aligned to 16KB. If RLEN=6KB, RBASE must be aligned to 8KB
  snps_dcd_ep_desc_t *ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;
  HSU_ULOG_FUNC(BAM_LOG);

  // Reuse previously allocated memory, do not allocate again
  if (FALSE == *trb_ring_cookie)
  {
    if (ep->is_in)
    {
      ep->max_trbs = 2 * n + 1 + 1;
    }
    else // OUT
    {
      ep->max_trbs = n + 1;
    }
    size_to_alloc = sizeof(snps_dcd_trb) * ep->max_trbs;
    snps_dcd_round_to_nearest_power_of_two(size_to_alloc, &alignment);
    // TODO AC check resultant value of trb_ring_va_addr at the caller
    rc = jdma_alloc(size_to_alloc, 
      alignment, trb_ring_va_addr, 
      trb_ring_pa_addr, M_ZERO, trb_ring_cookie);
  }

  return rc;
}


/**
* Function name:  snps_dcd_init_hw_trbs_fields
* Description: 
* Parameters: 
*     @ep: (IN) Endpoint descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
*/
static void snps_dcd_init_hw_trbs_fields(snps_dcd_trb *trb_ptr, pipe_desc_t* pipe)
{
  trb_ptr->WORD2.PCM1 = 0;
  trb_ptr->WORD2.reserved = 0;
  trb_ptr->WORD2.TRBSTS = 0;
  
  trb_ptr->WORD3.CSP = (DIRECTION_OUT == pipe->direction) ? 1 : 0;
  trb_ptr->WORD3.LST = 0;
  trb_ptr->WORD3.CHN = 0;
  trb_ptr->WORD3.TRBCTL = TRB_CONTROL_NORMAL;
  trb_ptr->WORD3.ISP = 0;
  trb_ptr->WORD3.IOC = 1;
  trb_ptr->WORD3.StrmID_SOFNum = 0;
}

/**
* Function name:  snps_dcd_ops_populate_ep_trb_hw_accel
* Description: 
* Parameters: 
*     @ep: (IN) Endpoint descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_ops_populate_ep_trb_hw_accel(jdevice_t dev, pipe_desc_t *pipe, 
  juint32_t n, void **next_free_pa_running_ptr, juint32_t client_buf_size,
  void* trb_ring_va_addr, void* trb_ring_pa_addr, void **link_trb_pa_addr, juint32_t usb_gsi_interrupter_index)
{
  // The client_buf_size is specified for each TRB (not total size)
  jresult_t rc = HSU_JSUCCESS;
  HSU_ULOG_FUNC(BAM_LOG);

  if (DIRECTION_IN == pipe->direction)
  {
    juint32_t i;
    snps_dcd_trb *trb_ptr = (snps_dcd_trb *)trb_ring_va_addr;
    juint8_t *running_client_buf_pa_ptr = (juint8_t *)*next_free_pa_running_ptr;

    // 1. FAKE ZLP for n + 1
    // 2. NORMAL TRB for n
    // 3. LINK TRB

    j_memset(trb_ptr, 0, (2 * n + 1 + 1) * sizeof(snps_dcd_trb));

    // Fake ZLP
    for (i = 0; i < n + 1; i++)
    {
      snps_dcd_init_hw_trbs_fields(trb_ptr, pipe);

      trb_ptr->WORD0.BPTRL = 0;
      trb_ptr->WORD1.BPTRH = 0;
      trb_ptr->WORD2.BUFSIZE = 0;
      
      trb_ptr->WORD3.HWO = 0;
      trb_ptr++;
    }

    // Normal TRB
    for (i = 0; i < n; i++)
    {
      snps_dcd_init_hw_trbs_fields(trb_ptr, pipe);

      trb_ptr->WORD0.BPTRL = (juint32_t)running_client_buf_pa_ptr;
      trb_ptr->WORD1.BPTRH = 0; // Need to support 64 bit addressing
      trb_ptr->WORD2.BUFSIZE = 0;
      
      trb_ptr->WORD3.HWO = 0;
      running_client_buf_pa_ptr += client_buf_size; // Note that the caller will not see this pointer move (only local copy)
      *next_free_pa_running_ptr = running_client_buf_pa_ptr;
      trb_ptr++;
    }

    // Link TRB
    trb_ptr->WORD0.BPTRL = (uint32)trb_ring_pa_addr;
    trb_ptr->WORD1.BPTRH = 0x00A00000 | (usb_gsi_interrupter_index << 16);
    trb_ptr->WORD2.BUFSIZE = 0;
    trb_ptr->WORD3.TRBCTL = TRB_CONTROL_LINK;
    trb_ptr->WORD3.HWO = 1;
    *link_trb_pa_addr = (juint8_t*)trb_ring_pa_addr + ((juint8_t*)trb_ptr - (juint8_t*)trb_ring_va_addr);

    trb_ptr++;
  }
  else // DIRECTION_OUT
  {
    juint32_t i;
    snps_dcd_trb *trb_ptr = (snps_dcd_trb *)trb_ring_va_addr;
    juint8_t *running_client_buf_pa_ptr = (juint8_t *)*next_free_pa_running_ptr;

    // 1. NORMAL TRB for n
    // 2. LINK TRB

    j_memset(trb_ptr, 0, (n + 1) * sizeof(snps_dcd_trb));

    // Normal TRB
    for (i = 0; i < n; i++)
    {
      snps_dcd_init_hw_trbs_fields(trb_ptr, pipe);
      trb_ptr->WORD0.BPTRL = (juint32_t)running_client_buf_pa_ptr;
      trb_ptr->WORD1.BPTRH = 0; // Need to support 64 bit addressing
      trb_ptr->WORD2.BUFSIZE = client_buf_size;

      trb_ptr->WORD3.HWO = 0;
      trb_ptr->WORD3.ISP = 1;
      running_client_buf_pa_ptr += client_buf_size; // Note that the caller will not see this pointer move (only local copy)
      *next_free_pa_running_ptr = running_client_buf_pa_ptr;
      trb_ptr++;
    }

    // Link TRB
    trb_ptr->WORD0.BPTRL = (uint32)trb_ring_pa_addr;
    trb_ptr->WORD1.BPTRH = 0x00A00000 | (usb_gsi_interrupter_index << 16);
    trb_ptr->WORD2.BUFSIZE = 0;
    trb_ptr->WORD3.TRBCTL = TRB_CONTROL_LINK;
    trb_ptr->WORD3.HWO = 1;
    *link_trb_pa_addr = (juint8_t*)trb_ring_pa_addr + ((juint8_t*)trb_ptr - (juint8_t*)trb_ring_va_addr);
    trb_ptr++;
  } 
  return rc;  
}


/**
* Function name:  snps_dcd_deinit_ep_trbs_hw_accel
* Description: 
* Parameters: 
*     @ep: (IN) Endpoint descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
//TODO: -- Akash --
#if 0
jresult_t snps_dcd_ops_deinit_ep_trbs_hw_accel(cdc_gsi_ctx_type *sm_ctx, void* param, snps_dcd_ep_desc_t *ep)
{  
  HSU_ULOG_1(NO_MSG, BAM_LOG, "snps_ops_dcd_deinit_ep_trbs_hw_accel, ep 0x%08X", (uint32)ep);
  /* Free memory for each stream structure */
  if (ep->is_in)
  {
    if (sm_ctx->dl_trb_ring_cookie)
    {
      jdma_free(sm_ctx->dl_trb_ring_cookie);
      sm_ctx->dl_trb_ring_cookie = NULL;
      sm_ctx->dl_trb_ring_pa_addr = NULL;
      sm_ctx->dl_trb_ring_va_addr = NULL;
    }
  }
  else
  {
    if (sm_ctx->ul_trb_ring_cookie)
    {
      jdma_free(sm_ctx->ul_trb_ring_cookie);
      sm_ctx->ul_trb_ring_cookie = NULL;
      sm_ctx->ul_trb_ring_pa_addr = NULL;
      sm_ctx->ul_trb_ring_va_addr = NULL;
    }
  }

  return HSU_JSUCCESS;
}
#endif

/**
* Function name:  snps_dcd_ops_set_hwo_bit__updatexfer_ul
* Description: 
* Parameters: 
*     @ep: (IN) Endpoint descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_ops_set_hwo_bit__updatexfer_ul(jdevice_t dev, pipe_desc_t *pipe, juint32_t n, void *trb_ring_va_addr)
{
  juint32_t i;
  juint32_t DEPCMD;
  snps_dcd_trb *trb_ptr = (snps_dcd_trb *)trb_ring_va_addr;
  jresult_t rc;
  snps_dcd_DEPCMD_P1 p1;
  snps_dcd_DEPCMD_P0 p0;
  juint16_t CmdParams;
  snps_dcd_ep_desc_t *ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;
  HSU_ULOG_FUNC(BAM_LOG);

  // 1. NORMAL TRB for n
    
  // Normal TRB
  for (i = 0; i < n; i++)
  {
    trb_ptr->WORD3.HWO = 1;
    trb_ptr++;
  }

  p0.value = 0;
  p1.value = 0;

  /* Build and issue UPDATETRANSFER command */
  DEPCMD = 0;
  DEPCMD = HSU_VAR_OUTM(DEPCMD, pipe->xfer_rsc_idx_hw_accelerated, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_SHFT);  
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_HIPRI_FORCERM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_HIPRI_FORCERM_SHFT);
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
  DEPCMD = HSU_VAR_OUTM(DEPCMD, SNPS_DCD_DEPCMD_CMDTYPE_UPDATETRANSFER, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);
  HSU_ULOG_2(NO_MSG, SNPS_LOG, "snps_dcd_ops_set_hwo_bit__updatexfer_ul, UPDATE_XFER, EP 0x%X, xfer_rsc = %u", ep->hw_addr, ep->pipe->xfer_rsc_idx_hw_accelerated);

  rc = snps_dcd_write_ep_command(ep->sc, ep->hw_addr, 
    DEPCMD, p0.value, p1.value, 0, &CmdParams);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ERR_FATAL("UpdateXfer failed", 0, 0, 0);
  }
  return rc;
}


/**
* Function name:  snps_dcd_ops_get_ep_num
* Description: 
* Parameters: 
*     @ep: (IN) Endpoint descriptor
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_ops_get_ep_num(jdevice_t dev, pipe_desc_t *pipe, juint8_t *ep_hw_addr)
{
  jresult_t rc = HSU_JSUCCESS;
  snps_dcd_ep_desc_t *ep;
  HSU_ULOG_FUNC(BAM_LOG);

  if (pipe && pipe->dcd_handle)
  {
    ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;
    *ep_hw_addr = ep->hw_addr;
  }
  else
  {
    HSU_ERR_FATAL("snps_dcd_ops_get_ep_num NULL", 0, 0, 0);
  }
 
  return rc;
}

/**
* Function name:  snps_dcd_ops_get_ep_cur_req
* Description: 
* Parameters: 
*     @pipe: (IN) Pipe information
*
* Return value: current request point on success, NULL
* Scope: global
**/
request_t* snps_dcd_ops_get_ep_cur_req(pipe_desc_t *pipe)
{
  request_t* req = NULL;
  snps_dcd_ep_desc_t *ep;
  
  if (pipe && pipe->dcd_handle)
  {
    ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;
    if (ep->streams)
    {
      // Only first stream's request is supported
      req = ep->streams[0].cur_req;
    }
    else 
    {
      HSU_ULOG_2(NO_MSG, DATA_LOG, "steam_not_init, pipe 0x%X, ep_hw_addr 0x%X", (uint32)pipe->address, ep->hw_addr);     
    }
  }
  else
  {
    HSU_ULOG_1(NO_MSG, DATA_LOG, "param_null_error, pipe 0x%X", (uint32)pipe);
  }
 
  return req;
}

