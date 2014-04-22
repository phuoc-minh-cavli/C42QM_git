/*
===========================================================================

FILE:         dcd_snps.c

DESCRIPTION: Device Controller Driver for SNPS USB3.0 core. 

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/src/jslave/dcd/dcd_snps.c#2 $

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
#include <jusb_chp9_int.h>
#include <jusb_core_dcd.h>
#include "jos_int.h"
#include "hsu_hwio_dependency.h"
#include "hsu_al_bam_utils.h"
#include "hsu_platform.h"

/*==============================================================================
Constants and Macros
==============================================================================*/
#define REMOTE_WAKEUP_U0_CHECK_LOOP_COUNT_MAX (20000)
#define HSU_FUNCTION_WAKE_TNOTIFICATION_MS    (2500)
#define LINK_STATE_SUSPEND_U3 (3)

/*==============================================================================
File Static Variables
==============================================================================*/
// For JTAG debug script usage
static uint32 snps_dcd_max_trbs_per_stream = SNPS_DCD_MAX_TRBS_PER_STREAM;

dcd_snps_eps_dbm_ctx_type dcd_snps_eps_dbm_ctx;

/* Pointer which allows easier debug access to SNPS DCD software 
   context which is held inside the CORE. */
snps_dcd_soft_dc_t *snps_dcd_sc; 

static void snps_dcd_ops_set_bam_pipe_params(jdevice_t dev, void *connection, pipe_desc_t *out_pipe, uint32 producer_pipe_index, 
                                    uint32 producer_data_fifo_addr, uint32 producer_data_fifo_size,
                                    uint32 producer_desc_fifo_addr, uint32 producer_desc_fifo_size,
                                    pipe_desc_t *in_pipe, uint32 consumer_pipe_index,
                                    uint32 consumer_data_fifo_addr, uint32 consumer_data_fifo_size,
                                    uint32 consumer_desc_fifo_addr, uint32 consumer_desc_fifo_size);

static void snps_dcd_dbm_gen_cfg_init(jdevice_t dev);

/* The DCD Operations structure (CORE->DCD) that is registered with the CORE */
static dcd_ops_t ops = {
    snps_dcd_ops_enable,                /* dcd_enable */ 
    snps_dcd_ops_disable,               /* dcd_disable */
    snps_dcd_ops_wake_the_host,         /* wake_the_host */
    NULL,                               /* set_self_power */
    NULL,                               /* dcd_is_otg */
    NULL,                               /* dcd_invoke_otg_ioctl */
    snps_dcd_ops_vbus_connect,          /* vbus_connect */
    NULL,                               /* vbus_draw_amount */
    snps_dcd_ops_vbus_disconnect,       /* vbus_disconnect */
    snps_dcd_connect,                   /* dcd_connect */
    NULL,                               /* dcd_disconnect */
    snps_dcd_ops_enable_ep,             /* dcd_enable_ep */
    snps_dcd_ops_disable_ep,            /* dcd_disable_ep */
    snps_dcd_ops_abort_ep,              /* dcd_abort_ep */
    snps_dcd_ops_stall_ep,              /* dcd_stall_ep */
    snps_dcd_ops_send_io_request,       /* dcd_send_io_request */
    snps_dcd_ops_alloc_pipe,            /* dcd_alloc_pipe */
    snps_dcd_ops_free_pipe,             /* dcd_free_pipe */
    snps_dcd_ops_get_ep_status,         /* dcd_get_ep_status */
    snps_dcd_ops_set_address_state,     /* dcd_set_address_state */
    snps_dcd_ops_get_device_address,    /* dcd_get_device_address */ 
    snps_dcd_ops_handle_feature,        /* dcd_handle_feature */
    snps_dcd_ops_set_config_state,      /* dcd_set_config_state */
    snps_dcd_ops_get_max_packet0,       /* dcd_get_max_packet0 */       
    snps_dcd_ops_handle_tx_completion,  /* dcd_handle_tx_completion */       
    snps_dcd_ops_set_test_mode,         /* dcd_set_test_mode */
    snps_dcd_ops_allow_enumeration,     /* dcd_allow_enumeration    */
    snps_dcd_ops_disallow_enumeration,  /* dcd_disallow_enumeration */
    snps_dcd_ops_register_events,       /* dcd_register_events */
    snps_usb3_dcd_ioctl,                /* dcd_ioctl */
    snps_dcd_ops_set_bam_pipe_params,   /* dcd_set_bam_pipe_params */ 
    snps_dcd_ops_dbm_ep_init,           /* dcd_dbm_ep_init */
    NULL,                               /* dcd_dbm_ep_deinit */
    snps_dcd_ops_dbm_ep_reset,          /* dcd_dbm_ep_reset */
    snps_dcd_ops_dbm_init,              /* dcd_dbm_init */
    snps_dcd_ops_dbm_init_qdss,         /* dcd_dbm_init_qdss */
    snps_dcd_function_wakeup_request,   /* dcd_function_wakeup_request */
    snps_dcd_ops_accept_u1_u2,          /* dcd_accept_u1_u2 */
    snps_dcd_ops_get_iface_index,       /* dcd_get_iface_index */

    snps_dcd_setup_event_buf_and_gsi_interrupters, /* dcd_setup_event_buf_and_gsi_interrupters */
    snps_dcd_reset_event_buffer,                   /* dcd_reset_event_buffer */
    snps_dcd_usb_gsi_endpoint_config,              /* dcd_usb_gsi_endpoint_config */
    snps_dcd_ops_enable_ep_data_struct_only,       /* dcd_enable_ep_data_struct_only */
    snps_dcd_ops_disable_ep_hw_accel,              /* dcd_disable_ep_hw_accel */
    snps_dcd_ops_abort_ep_hw_accel,                /* dcd_abort_ep_hw_accel */
    snps_dcd_ops_begin_hw_accel_xfer,              /* dcd_begin_hw_accel_xfer */
    snps_dcd_ops_gsi_post_channel_connect_init,    /* dcd_gsi_post_channel_connect_init */
    snps_dcd_ops_init_ep_trbs_hw_accel,            /* dcd_init_ep_trbs_hw_accel */
    snps_dcd_ops_gsi_general_cfg,                  /* dcd_gsi_general_cfg */
    snps_dcd_ops_populate_ep_trb_hw_accel,         /* dcd_populate_ep_trb_hw_accel */
    snps_dcd_ops_set_hwo_bit__updatexfer_ul,       /* dcd_set_hwo_bit__updatexfer_ul */
    snps_dcd_ops_get_ep_num,                       /* dcd_get_ep_num */
    snps_dcd_ops_get_ep_cur_req,                   /* dcd_get_ep_cur_req */
};

/*==============================================================================
External Function Definitions
==============================================================================*/

/**
 * Function name:  snps_dcd_ops_vbus_connect
 * Description: Enable the pull up resistor on the D+ line, causing to vBus to
 *              rise
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
jresult_t snps_dcd_ops_vbus_connect(jdevice_t dev)
{
    snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
    jbus_h snps_dcd_bus = j_device_get_bus(dev);
    hsu_platform_info_type const * platform_info = hsu_platform_get_info();
  
    /* Check if DCD is disabled - we shouldn't start VBUS unless we are 
       enabled */
    if (sc->conn_status < SNPS_DCD_STATE_ENABLED)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, dcd_is_disabled, cannot_start_vbus");
      return JEPERM;
    }

    if (sc->enumeration_is_allowed == FALSE)
    {
      HSU_ULOG(NO_MSG, BUS_LOG, "enum_allowed_false");
      // Mark that we wish to start enumeration as soon as the next "allow enumeration" comes
      sc->enumeration_is_pending = TRUE;
      return HSU_JSUCCESS;
    }

    // Enumeration is allowed
    HSU_ULOG(NO_MSG, BUS_LOG, "pending_enum_allowed");
    sc->enumeration_is_pending = FALSE;

    /* Run controller */
    HWIO_DCTL_OUTM(HWIO_DCTL_RUN_STOP_BMSK, (0x1UL << HWIO_DCTL_RUN_STOP_SHFT));

    HWIO_HS_PHY_CTRL_OUTM(HWIO_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_BMSK, (0x1 << HWIO_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_SHFT));
    HWIO_HS_PHY_CTRL_OUTM(HWIO_HS_PHY_CTRL_SW_SESSVLD_SEL_BMSK, (0x1 << HWIO_HS_PHY_CTRL_SW_SESSVLD_SEL_SHFT));
#if 0
    if (SPEED_SUPER == sc->speed)
    {
      HWIO_SS_PHY_CTRL_OUTM(HWIO_SS_PHY_CTRL_LANE0_PWR_PRESENT_BMSK, (0x1 << HWIO_SS_PHY_CTRL_LANE0_PWR_PRESENT_SHFT));
    }
#endif
    return HSU_JSUCCESS;
}

/**
 * Function name:  snps_dcd_ops_vbus_disconnect
 * Description: Disable the pull up resistor on the D+ line, causing to vBus to
 *              fall
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_dcd_ops_vbus_disconnect(jdevice_t dev)
{
    snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
    jbus_h snps_dcd_bus = j_device_get_bus(dev);

    /* Check if DCD is disabled - we shouldn't stop VBUS unless we are 
       enabled */
    if (sc->conn_status < SNPS_DCD_STATE_ENABLED)
    {
        DBG_W(DSLAVE_DCD, ("snps_dcd_ops_vbus_disconnect: DCD is disabled, can't stop "
            "vbus\n"));
        return JEPERM;
    }

    // The RS bit to 0 is moved to snps_dcd_handle_disconnect()
    sc->enumeration_is_pending = FALSE;

    return HSU_JSUCCESS;
}

/**
 * Function name:  snps_dcd_connect
 * Description: Runs the controller
 * Parameters:
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static int snps_dcd_connect(jdevice_t dev)
{
  snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);

  /* Initialize the controller to RX Detect State */
  HWIO_DCTL_OUTM(HWIO_DCTL_ULSTCHNGREQ_BMSK, (0x0 << HWIO_DCTL_ULSTCHNGREQ_SHFT));

  if (SPEED_SUPER == ((core_t*)sc->core_ctx)->max_speed)
  {
    HWIO_DCTL_OUTM(HWIO_DCTL_ULSTCHNGREQ_BMSK, (0x5 << HWIO_DCTL_ULSTCHNGREQ_SHFT));
  }

  return 0;
}

/**
* Function name:  snps_dcd_ops_free_pipe
* Description: Called by CORE to free an allocated DCD endpoint 
* Parameters: 
*     @dev: (IN) pointer to the device structure
*     @pipe: (IN) CORE pipe context
*
* Return value: 0 on success, otherwise an error code
* Scope: local
**/
static jresult_t snps_dcd_ops_free_pipe(jdevice_t dev, pipe_desc_t *pipe)
{
    snps_dcd_ep_desc_t *ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;

    /* Mark the endpoint as free */
    ep->claimed_config = 0;
    ep->claimed_iface = 0;
    ep->claimed_alt = 0;

    return HSU_JSUCCESS;
}
/**
 * Function name:  snps_dcd_ops_alloc_pipe
 * Description: Called by CORE to allocate DCD endpoint for the specific pipe
 *              properties
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *     @iface_index: (IN) interface number for the requested pipe
 *     @alt_index: (IN) alternate setting for the requested pipe
 *     @config_index: (IN) config value for the requested pipe
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_dcd_ops_alloc_pipe(jdevice_t dev, pipe_desc_t *pipe, 
    juint8_t config_index, juint8_t iface_index, juint8_t alt_index)
{
  snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
  jint_t i;

  HSU_ULOG_3(NO_MSG, SNPS_LOG, "find_matching_iface, pipe 0x%08X, iface_index %u, alt_index %u", 
    (uint32) pipe, iface_index+1, alt_index+1);

  for (i = 2; i < sc->max_available_eps; i++)
  {
    DBG_X(DSLAVE_DCD, ("snps_dcd_ops_alloc_pipe: allocating pipe (ep = %d)\n",
      sc->ep_desc_table[i].soft_addr));

    HSU_ASSERT(config_index >= sc->ep_desc_table[i].claimed_config);

    if (sc->ep_desc_table[i].claimed_config == config_index)
    {
      /* Check that pipe not claimed by other FD */
      if (sc->ep_desc_table[i].claimed_iface != (iface_index  + 1) &&
        sc->ep_desc_table[i].claimed_iface)
      {
        continue;
      }

      DBG_X(DSLAVE_DCD, ("snps_dcd_ops_alloc_pipe: unclaimed pipe was found\n"));

      HSU_ASSERT(alt_index + 1 >= sc->ep_desc_table[i].claimed_alt);

      /* Cant have same alt-interface claim same pipes */
      if (sc->ep_desc_table[i].claimed_alt == (alt_index + 1))
        continue;
    }

    DBG_X(DSLAVE_DCD, ("snps_dcd_ops_alloc_pipe: unclaimed pipe by alt iface was found\n"));

    /* Check that the direction we need is supported */
    if ((pipe->direction == DIRECTION_OUT && sc->ep_desc_table[i].is_in) ||
      (pipe->direction == DIRECTION_IN && !sc->ep_desc_table[i].is_in)) 
      continue;

    DBG_X(DSLAVE_DCD,("snps_dcd_ops_alloc_pipe: pipe supports the direction\n"));

    /* Requested Max Packet Size will be verified when enabling the pipe, 
       according to the connection speed. */

    /* Check that the pipe type we need is supported */
    if (!SUPPORTED_PIPE_TYPE(sc->ep_desc_table[i].type_map, pipe->type))
    {
      DBG_X(DSLAVE_DCD, ("snps_dcd_ops_alloc_pipe: pipe does NOT support type %d\n",
        pipe->type));
      continue;
    }

    DBG_X(DSLAVE_DCD, ("snps_dcd_ops_alloc_pipe: pipe supports type %d\n", pipe->type));

    /* Found it */
    break;
  }

  /* Check why we exited the loop */
  if (i == sc->max_available_eps)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, cannot_match_req_pipe_config_with_availble_EPs");
    return JOUTOFEPS; 
  }

  sc->ep_desc_table[i].claimed_alt = alt_index + 1;
  sc->ep_desc_table[i].claimed_iface = iface_index + 1;
  sc->ep_desc_table[i].claimed_config = config_index;

  /* Setting address */
  pipe->address = sc->ep_desc_table[i].soft_addr;

  /* Setting direction */
  if (pipe->direction == DIRECTION_IN)
    pipe->address |= USB_DIR_IN;

  /* Storing references pipe <--> EP */ 
  pipe->dcd_handle = (void *)(&sc->ep_desc_table[i]);

  HSU_ULOG_2(NO_MSG, SNPS_LOG, "ep_matched_to_address, soft_addr 0x%08X, pipe_addr 0x%08X",
    sc->ep_desc_table[i].soft_addr, pipe->address);

  return HSU_JSUCCESS;
}

/**
 * Function name:  snps_dcd_ops_get_ep_status
 * Description: Return the endpoint status (Stalled or not)
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @status: (OUT) endpoint status
 *     @ep_num: (IN) EP USB address
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_dcd_ops_get_ep_status(jdevice_t dev, juint8_t *status, 
    juint8_t usb_ep_address)
{
    snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
    snps_dcd_ep_desc_t *ep = snps_dcd_get_ep_desc(sc, usb_ep_address);

    /* If EP is stalled returns TRUE else returns FALSE */
    *status = (ep->pipe) ? (PIPE_STALLLED == ep->pipe->status) : FALSE;

    return HSU_JSUCCESS;
}

/**
 * Function name:  snps_dcd_ops_enable_ep
 * Description: Called by CORE to enable a specific endpoint
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_dcd_ops_enable_ep(jdevice_t dev, pipe_desc_t *pipe)
{
    snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
    snps_dcd_ep_desc_t *ep;
    jresult_t rc = HSU_JSUCCESS;

    if ((NULL == pipe) || (NULL == pipe->dcd_handle))
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, bad_pipe");
      return JEINVAL;
    }

    ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;
    ep->pipe = pipe;

    /* Update packet size EP parameter according to speed */
    if (pipe->max_pkt_size <= core_get_max_packet_size(ep->pipe->type,sc->speed))
    {
      ep->packet_size = pipe->max_pkt_size;
    }
    else
    {
      /* Wrong pipe requirement */
      HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, wrong_max_packet_size %u", pipe->max_pkt_size);
      return JEINVAL;
    }

    ep->abort_timeout_task = 0;
    rc = jtask_init(&ep->timeout_task, TASK_CONTROLLER);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, timeout_task_init_failed");
      return rc;
    }

    /* Update Super-Speed parameters from pipe structure */
    ep->max_streams = 1 << pipe->ss_attributes.max_streams; 
    ep->max_burst = pipe->ss_attributes.max_burst;
    ep->mult = pipe->ss_attributes.mult;
    ep->isoc_interval = 1 << (pipe->poll_interval_high - 1);

    rc = snps_dcd_init_ep_streams(ep);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, init_EP_streams_failed");
      return rc;
    }

    /* Configure the controller with EP parameters */
    rc = snps_dcd_config_ep(ep);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_config_ep_failed");
      return rc;
    }

    snps_dcd_dalepena_ep(ep);
    pipe->status = PIPE_ENABLED;

    return HSU_JSUCCESS;
}


/**
 * Function name:  snps_dcd_ops_enable_ep_data_struct_only
 * Description: Called by CORE to enable a specific endpoint, but only initialize data structure section
 *    The remaining initializations are to be done by the XDCI GSI state machine
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_dcd_ops_enable_ep_data_struct_only(jdevice_t dev, pipe_desc_t *pipe)
{
  snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
  snps_dcd_ep_desc_t *ep;
  HSU_ULOG_FUNC(BAM_LOG);
  
  if ((NULL == pipe) || (NULL == pipe->dcd_handle))
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_ops_enable_ep_data_struct_only, bad_pipe");
    return JEINVAL;
  }
  
  ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;
  ep->pipe = pipe;
  
  /* Update packet size EP parameter according to speed */
  if (pipe->max_pkt_size <= core_get_max_packet_size(ep->pipe->type, sc->speed))
  {
    ep->packet_size = pipe->max_pkt_size;
  }
  else
  {
    /* Wrong pipe requirement */
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_ops_enable_ep_data_struct_only, wrong_max_packet_size %u", pipe->max_pkt_size);
    return JEINVAL;
  }
  
  ep->abort_timeout_task = 0;
#if 0 // For HW acceleration skip the use of timeout task
  rc = jtask_init(&ep->timeout_task, TASK_CONTROLLER);
  if (rc != HSU_JSUCCESS)
  {
    DBG_E(DSLAVE_DCD, ("snps_dcd_ops_enable_ep: timeout task init failed\n"));
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_ops_enable_ep, timeout_task_init_failed");
    return rc;
  }
#endif

#if 0
  /* Update Super-Speed parameters from pipe structure */
  ep->max_streams = 1 << pipe->ss_attributes.max_streams; 
  ep->max_burst = pipe->ss_attributes.max_burst;
  ep->mult = pipe->ss_attributes.mult;
  ep->isoc_interval = 1 << (pipe->poll_interval_high - 1);
#endif
  // DEPCFG
  // DEPXFERCFG
  // DALEPENA
  // are to be completed by state machine invocation of snps_dcd_usb_gsi_endpoint_config( )
  
  return HSU_JSUCCESS;
}


/* 4.3.2.2 EP configuration to DBM Mode */
/* DBM EP --> regular EP */
/* 1. Disable the BAM pipe of the DBM EP */
/* 2. Write to USB30_DBM_REGFILE_DBM_EP_CFG_ADDRn of the relevant DBM EP. 
 * All the register fields should be reset to 0, accept USB3_EPNUM which should be set to the correct physical EP num. */
/* 3.	Use the EP as regular device EP. */
/* DBM EP --> regular EP via BAM Reset (Method 2, not implemented)*/
/* 1. After the DBM EP was enabled, execute BAM reset. */
/* 2. Disable the DBM EP */
/* 3. Use the EP as regular device EP. */
/* Related Notes from 4.4.1.7 DBM Reset */
/*   Endxfer command for a usb ep that is DBM enabled, requires the related dbm ep and bam pipe to be reset too. */
/*   There can't be a dbm ep sw reset without a pipe reset, or an Endxfer command without dbm ep sw reset and pipe reset. */
/*   To end the DBM EP "endless" transfer, SW must wait until BAM is empty and not active and then give ENDXFER command. */
/*   From Torino bam pipe reset causes dbm soft reset for corresponding dbm ep -- unless disabled using DBM_EP_CFG.DBM_BAM_P_SW_RST_DISABLE. */
void snps_dcd_abort_dbm_ep(snps_dcd_soft_dc_t *sc, uint32 dbm_ep_num)
{
  uint32 phy_ep_num;
  /* 1. Disable the BAM pipe of the DBM EP */
  //TODO AC notify state machine that BAM pipe is going to be disabled
  if (dbm_ep_num <= HWIO_DBM_EP_CFG_ADDRn_MAXn)
  {
    HSU_ULOG_1(NO_MSG, BAM_LOG, "dbm_ep_num %u", dbm_ep_num);
    dcd_snps_eps_dbm_ctx.is_dbm_ep_initialized[dbm_ep_num] = FALSE;

    /* 2. Write to USB30_DBM_REGFILE_DBM_EP_CFG_ADDRn of the relevant DBM EP. 
     * All the register fields should be reset to 0, except USB3_EPNUM which should be set to the correct physical EP num. */
    phy_ep_num = (uint32)dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[dbm_ep_num].hw_ep_num;
    HWIO_DBM_EP_CFG_ADDRn_OUTI(dbm_ep_num, phy_ep_num << HWIO_DBM_EP_CFG_ADDRn_USB3_EPNUM_SHFT);
  }
  else
  {
    HSU_ERR_FATAL("Out of bound dbm_ep_num", 0, 0, 0);
  }
}

/**
 * Function name:  snps_dcd_ops_disable_ep
 * Description: Call by CORE to disable a specific endpoint
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_dcd_ops_disable_ep(jdevice_t dev, pipe_desc_t *pipe)
{
    snps_dcd_ep_desc_t *ep;
    jresult_t rc = HSU_JSUCCESS;

    if ((NULL == pipe) || (NULL == pipe->dcd_handle))
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, bad_pipe");
      return JEINVAL;
    }
    ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;

    rc = snps_dcd_ops_abort_ep(dev, pipe);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, aborting_EP_failed");
      return rc;
    }

    rc = snps_dcd_stall_ep(ep, FALSE);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, clearing_EP_stall_failed");
      return rc;
    }

    /* Un-Configure the controller from the EP parameters */
    rc = snps_dcd_unconfig_ep(ep);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, unconfig_EP_streams_failed");
      return rc;
    }

    ep->pipe = NULL;

    rc = snps_dcd_uninit_ep_streams(ep);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, uninit_ep_streams_failed");
      return rc;
    }

    if (ep->timeout_task)
    {
      jtask_uninit(ep->timeout_task);
      ep->timeout_task = NULL;
    }

    pipe->status = PIPE_DISABLED;

    return HSU_JSUCCESS;
}

/**
 * Function name:  snps_dcd_ops_disable_ep_hw_accel
 * Description: Call by CORE to disable a specific endpoint
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_dcd_ops_disable_ep_hw_accel(jdevice_t dev, pipe_desc_t *pipe)
{
  snps_dcd_ep_desc_t *ep;
  jresult_t rc = HSU_JSUCCESS;
  HSU_ULOG_FUNC(BAM_LOG);

  if ((NULL == pipe) || (NULL == pipe->dcd_handle))
  {
    DBG_E(DSLAVE_DCD, ("snps_dcd_ops_disable_ep for bad pipe\n"));
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_ops_disable_ep, bad_pipe");
    return JEINVAL;
  }
  ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;

  rc = snps_dcd_ops_abort_ep_hw_accel(dev, pipe);
  if (rc != HSU_JSUCCESS)
  {
    DBG_E(DSLAVE_DCD, ("snps_dcd_ops_disable_ep: aborting EP failed\n"));
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_ops_disable_ep, aborting_EP_failed");
    return rc;
  }
  rc = snps_dcd_stall_ep(ep, FALSE);
  if (rc != HSU_JSUCCESS)
  {
    DBG_E(DSLAVE_DCD, ("snps_dcd_ops_disable_ep: clearing EP stall failed\n"));
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_ops_disable_ep, clearing_EP_stall_failed");
    return rc;
  }

  /* Un-Configure the controller from the EP parameters */
  rc = snps_dcd_unconfig_ep(ep);
  if (rc != HSU_JSUCCESS)
  {
    DBG_E(DSLAVE_DCD, ("snps_dcd_ops_disable_ep: unconfig EP streams failed\n"));
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_ops_disable_ep, unconfig_EP_streams_failed");
    return rc;
  }

  ep->pipe = NULL;

  pipe->status = PIPE_DISABLED;

  return HSU_JSUCCESS;
}


/**
 * Function name:  snps_dcd_ops_abort_ep
 * Description: Called by CORE to abort all pending transfer on the specific
 *              pipe
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
jresult_t snps_dcd_ops_abort_ep(jdevice_t dev, pipe_desc_t *pipe)
{
    snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
    juint32_t DEPCMD;
    jresult_t rc = HSU_JSUCCESS;
    request_t *curr_req;

    /* Abort is for EP0 */
    if (NULL == pipe)
    {
      if (snps_dcd_abort_ep0(sc))
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, abort_ep0_failed");
        return EUNKNOWN;
      }
    }
    /* Abort is not for EP0 */
    else
    {
      if (NULL == pipe->dcd_handle)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, bad_pipe");
        return HSU_JSUCCESS;
      }
      else
      {
        snps_dcd_ep_desc_t *ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;
        juint16_t stream_id = SNPS_DCD_DEFAULT_STREAM;

        #ifdef FEATURE_HS_USB_BAM
        juint8_t dbm_ep_num = snps_dcd_get_dbm_ep_from_hw_ep(ep->hw_addr);
        #endif /* FEATURE_HS_USB_BAM */

        HSU_CHK_EP_STREAM_ID(ep, stream_id);

        if(pipe->type == PIPE_ISOC)
        {
          ep->xfer_in_progress = 0;
          ep->xfer_nrdy_elapsed = TRUE;
          ep->streams[0].stream_initialized = FALSE;
          snps_dcd_init_stream_trbs(ep, 0);
        }

        pipe->status = PIPE_ABORTING;

        /* Go over all streams of the EP */
        for (stream_id = 0; stream_id < ep->max_streams; stream_id++)
        {
          
          if (NULL == ep->streams[stream_id].cur_req)
          {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, nothing_to_abort, EP 0x%X", ep->hw_addr);
          }
          else
          {
            #ifdef FEATURE_HS_USB_BAM
            if (HSU_UNDEFINED_EP_VAL != dbm_ep_num)
            {
              HWIO_DBM_DIS_UPDXFER_OUTM(1 << dbm_ep_num, HWIO_DBM_DIS_UPDXFER_DIS_UPDXFER_EP0_BLOCK_UPDXFER_FVAL << dbm_ep_num);
            }
            #endif /* FEATURE_HS_USB_BAM */

            /* Build and issue ENDTRANSFER command */
            DEPCMD = 0;
            DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_HIPRI_FORCERM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_HIPRI_FORCERM_SHFT);
            DEPCMD = HSU_VAR_OUTM(DEPCMD, ep->streams[stream_id].xfer_rsc_idx, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_SHFT);  
            DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
            DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
            DEPCMD = HSU_VAR_OUTM(DEPCMD, SNPS_DCD_DEPCMD_CMDTYPE_ENDTRANSFER, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);
            rc = snps_dcd_write_ep_command(ep->sc, ep->hw_addr, DEPCMD, 0, 0, 0, NULL);
            if (rc != HSU_JSUCCESS)
            {
              // Continue execution to clean up the requests in cur_req and req_queue
              HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_write_ep_cmd_failed, EP 0x%X", ep->hw_addr);
            }  

            #ifdef FEATURE_HS_USB_BAM
            /* TODO AC if EP is BAM mode, ensure it's empty and inactive before ENDTRANSFER CMD */
            /* To end the DBM EP "endless" transfer, 
             * SW must wait until BAM is empty and not active and then give ENDXFER command. */
            if (HSU_UNDEFINED_EP_VAL != dbm_ep_num)
            {
              HSU_ULOG_1(NO_MSG, BAM_LOG, "trb_for_abort_xfer_infinite_mode, dbm_ep_num %u", dbm_ep_num);
            }
            #endif /* FEATURE_HS_USB_BAM */

            // Set xfer_rsc_idx to 0 such that StartXfer will be invoked again
            if (pipe->type != PIPE_ISOC)
            {
              ep->streams[stream_id].xfer_rsc_idx = 0;
              ep->streams[stream_id].stream_initialized = FALSE;
            }

            // Upon abort endpoint, reset all HW and SW TRB pointers back to the front of the array
            ep->streams[stream_id].trb_soft_ptr
              = ep->streams[stream_id].trb_hw_ptr 
              = ep->streams[stream_id].soft_trb_array.virt;

            // Free EP's cur_req
            curr_req = ep->streams[stream_id].cur_req;
            ep->streams[stream_id].cur_req = NULL;
        
            if ((curr_req->direction == DIRECTION_OUT)
              || curr_req->single_request || (pipe->type == PIPE_ISOC))
            {
              // Need to remove req_queue's first for OUT || single || PIPE_ISOC
              // Since it is same one as req_queue
        
              // ep->stream[N].cur_req must be same as head of pipe->req_queue.
              // Different request, it will result in request memory leak and 
              // dangling pointer for freed request 
              HSU_ASSERT(STAILQ_FIRST(&pipe->req_queue) == curr_req);
              STAILQ_REMOVE_HEAD(&pipe->req_queue, next_request);
            }
        
            snps_dcd_dequeue_request(ep, curr_req);
          }
        }

        // Free EP's req_queue
        while (FALSE == STAILQ_EMPTY(&pipe->req_queue))
        {
          curr_req = STAILQ_FIRST(&pipe->req_queue);
          STAILQ_REMOVE_HEAD(&pipe->req_queue, next_request);
          
          snps_dcd_dequeue_request(ep, curr_req);
        }
        
        pipe->status = PIPE_ENABLED;
      }
    }

    return HSU_JSUCCESS;
}

/**
 * Function name:  snps_dcd_ops_abort_ep_hw_accel
 * Description: Called by CORE to abort all pending transfer on the specific
 *              pipe
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
jresult_t snps_dcd_ops_abort_ep_hw_accel(jdevice_t dev, pipe_desc_t *pipe)
{
  juint32_t DEPCMD;
  jresult_t rc = HSU_JSUCCESS;

  if (NULL == pipe->dcd_handle)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_ops_abort_ep_hw_accel, bad_pipe");
    return HSU_JSUCCESS;
  }
  else
  {
    snps_dcd_ep_desc_t *ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;

    pipe->status = PIPE_ABORTING;

    /* Build and issue ENDTRANSFER command */
    /* CMDACT and CMDIOCT needs to be set to 0 to prevent enumeration failure on bus RESET */
    DEPCMD = 0;
    DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_HIPRI_FORCERM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_HIPRI_FORCERM_SHFT);
    DEPCMD = HSU_VAR_OUTM(DEPCMD, pipe->xfer_rsc_idx_hw_accelerated, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_SHFT);  
    DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
    DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
    DEPCMD = HSU_VAR_OUTM(DEPCMD, SNPS_DCD_DEPCMD_CMDTYPE_ENDTRANSFER, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);
    rc = snps_dcd_write_ep_command(ep->sc, ep->hw_addr, DEPCMD, 0, 0, 0, NULL);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_ops_abort_ep_hw_accel, snps_dcd_write_ep_cmd_failed, EP 0x%X", ep->hw_addr);
      //If abort fails here, ISOC has additional abort while submission
      if(pipe->type != PIPE_ISOC)
      {
        // HSU_ERR_FATAL("Write EP command to USB HW timeout", ep->hw_addr, 0, 0);
        return rc;
      }
    }
    else
    {
      HSU_ULOG_1(NO_MSG, BAM_LOG, "DCD, snps_dcd_ops_abort_ep_hw_accel, ENDXFER, EP 0x%X", ep->hw_addr); 
    }
    pipe->status = PIPE_ENABLED;
    pipe->xfer_rsc_idx_hw_accelerated = 0;
  }
  return HSU_JSUCCESS;
}

/**
 * Function name:  snps_dcd_ops_stall_ep
 * Description: Set/Clear stall on the specific pipe
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *     @flag: 1 for stall the endpoint, 0 for clear stalled endpoint
 *
 * Return value: 
 * Scope: local
 **/
jresult_t snps_dcd_ops_stall_ep(jdevice_t dev, pipe_desc_t *pipe, jbool_t flag)
{
    snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
    snps_dcd_ep_desc_t* ep0_out = &sc->ep_desc_table[SNPS_DCD_EP0_OUT_ADDR];
    request_t* request = ep0_out->streams[SNPS_DCD_DEFAULT_STREAM].cur_req;
    jresult_t rc;

    /* Change STALL state for EP0 */
    if (pipe == NULL)
    {
      if (request == NULL)
      {
        rc = snps_dcd_stall_ep(ep0_out, TRUE);
        if (rc != HSU_JSUCCESS)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, error_stalling_ep0_out");
        }
      }

      rc =  snps_dcd_abort_ep0(sc);
      if (rc != HSU_JSUCCESS)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_abort_ep0_failed");
        return JEINVAL;
      }
    }
    else
    {
      snps_dcd_ep_desc_t *ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;
      rc = snps_dcd_stall_ep(ep, flag);
      if (rc != HSU_JSUCCESS)
      {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, stall_ep%d_failed", (uint32) ep);
      }
    }

    return rc;
}

/**
 * Function name:  snps_dcd_ops_send_io_request
 * Description: Called by CORE to queue a request on the hardware
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *     @core_req: (IN) CORE request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_dcd_ops_send_io_request(jdevice_t dev, pipe_desc_t *pipe, 
    request_t *core_req)
{
    snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
    snps_dcd_ep_desc_t* ep;
    jresult_t rc;

    if (core_req == NULL)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, bad_request");
      return JEINVAL;
    }

    /* Request is for EP0 */
    if (NULL == pipe)
    {
      ep = (core_req->direction == DIRECTION_OUT) ? 
        &sc->ep_desc_table[SNPS_DCD_EP0_OUT_ADDR] : &sc->ep_desc_table[SNPS_DCD_EP0_IN_ADDR]; 
    }
    /* Request is not for EP0 */
    else
    {
      if (NULL == pipe->dcd_handle)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, bad_pipe");
        return JEINVAL;
      }
      ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;
    }

    if (ep->timeout_task && core_req->timeout)
    {
      jtask_schedule(ep->timeout_task, core_req->timeout,
        snps_dcd_handle_timeout, ep);
      ep->abort_timeout_task = 1;
    }

    if(ep->soft_addr == 0)
    {
      HSU_ULOG_1(NO_MSG, SNPS_CTRL_LOG, "ctrl_pending_request, EP 0x%X, event 3", ep->hw_addr);
      snps_dcd_ctrl_sm_notify(ep, core_req, SNPS_CTRL_EVT_CORE_REQ);
      return HSU_JSUCCESS;
    }

    /* Enqueue the request to the controller for the required EP */
    rc = snps_dcd_enqueue_request(ep, core_req);
    
    return rc;
}



/**
 * Function name:  snps_dcd_ops_begin_hw_accel_xfer
 * Description: 
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *     @core_req: (IN) CORE request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_dcd_ops_begin_hw_accel_xfer(jdevice_t dev, pipe_desc_t *pipe, void* trb_ring_pa_addr, juint32_t gsi_interrupter_idx, boolean is_ep_resume)
{
  snps_dcd_ep_desc_t* ep;
  juint16_t CmdParams;
  jresult_t rc;

  juint32_t DEPCMD;
  snps_dcd_DEPCMD_P1 p1;
  snps_dcd_DEPCMD_P0 p0;
    
  if (NULL == pipe->dcd_handle)
  {
    DBG_E(DSLAVE_DCD, ("dcd_send_io_request for bad pipe\n"));
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_ops_send_io_request, bad_pipe");
    return JEINVAL;
  }
  ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;

  p0.value = 0;
  p1.value = 0;

  // The very first time a TRB is used, it must be a start xfer
  if (pipe->xfer_rsc_idx_hw_accelerated != 0)
  {    
    /* Build and issue UPDATETRANSFER command */
    DEPCMD = 0;
    DEPCMD = HSU_VAR_OUTM(DEPCMD, pipe->xfer_rsc_idx_hw_accelerated, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_SHFT);  
    DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_HIPRI_FORCERM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_HIPRI_FORCERM_SHFT);
    DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
    DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
    DEPCMD = HSU_VAR_OUTM(DEPCMD, SNPS_DCD_DEPCMD_CMDTYPE_UPDATETRANSFER, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);
    HSU_ULOG_2(NO_MSG, BAM_LOG, "snps_dcd_ops_begin_hw_accel_xfer, UPDATE_XFER, EP 0x%X, xfer_rsc = %u", ep->hw_addr, pipe->xfer_rsc_idx_hw_accelerated);
  }
  else
  {
    /* Build and issue DEPSTRTXFER command */
    p0.value = 0;
    p1.value = 0;
    // TODO AC to double check correct address
    if (is_ep_resume)
    {
      // Start transfer from writeback capture address
      p1.DEPSTRTXFER.TDAddrLow = (juint32_t)HWIO_USB30_GSI_IF_CAP_WB_ADDRn_INI(gsi_interrupter_idx - 1);
    }
    else
    {
      // New start transfer
      p1.DEPSTRTXFER.TDAddrLow = (juint32_t)trb_ring_pa_addr;
    }

    // For hardware acceleration
    //a.  Bit[53]=1'b1 == bit 21 of HiAddr
    //b.  Bit[55]=1'b1 == bit 23 of HiAddr
    //c.  Bit[50:48]=interrupt number == bit [18:16] of HiAddr
    if ((PIPE_BULK == pipe->type) && (DIRECTION_IN == pipe->direction))
    {
      p0.DEPSTRTXFER.TDAddrHigh = (1 << 21) | (1 << 23) | (gsi_interrupter_idx << 16);
    }
    else if ((PIPE_BULK == pipe->type) && (DIRECTION_OUT == pipe->direction))
    {
      p0.DEPSTRTXFER.TDAddrHigh = (1 << 21) | (1 << 23) | (gsi_interrupter_idx << 16);
    }
    // Else the pipe is assumed valid due to the previous check
    DEPCMD = 0;
    DEPCMD = HSU_VAR_OUTM(DEPCMD, 0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_SHFT);  
    DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
    // TODO AC is CMDIOC bit required for initiating xfer for GSI purpose?
    DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
    DEPCMD = HSU_VAR_OUTM(DEPCMD, SNPS_DCD_DEPCMD_CMDTYPE_STARTTRANSFER, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);
    HSU_ULOG_1(NO_MSG, BAM_LOG, "snps_dcd_ops_begin_hw_accel_xfer, START_XFER, EP 0x%X", ep->hw_addr);
  }

  rc = snps_dcd_write_ep_command(ep->sc, ep->hw_addr, 
    DEPCMD, p0.value, p1.value, 0, &CmdParams);

  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_ops_begin_hw_accel_xfer, snps_dcd_write_ep_cmd_failed, EP 0x%X", ep->hw_addr);
    return rc;
  }
  else
  {
    if (0 == pipe->xfer_rsc_idx_hw_accelerated)
    {
      /* Save transfer resource ID in request structure */
      pipe->xfer_rsc_idx_hw_accelerated = (juint8_t)CmdParams & SNPS_EP_XFER_RSC_INDEX_BMASK;
      HSU_ULOG_2(NO_MSG, BAM_LOG, "snps_dcd_ops_begin_hw_accel_xfer, SAVE_XFER_RSC %u, EP 0x%X", pipe->xfer_rsc_idx_hw_accelerated, ep->hw_addr);
    }
  }

  return HSU_JSUCCESS;
}


/**
 * Function name:  snps_dcd_ops_gsi_post_channel_connect_init
 * Description: 
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *     @core_req: (IN) CORE request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_dcd_ops_gsi_post_channel_connect_init(jdevice_t dev, pipe_desc_t *pipe, juint32_t gsi_interrupter_idx, void* gsi_doorbell_reg, void* trb_ring_pa_addr)
{
  HSU_ULOG_FUNC(BAM_LOG);

  HWIO_USB30_GSI_DBL_ADDR_Hn_OUTI(gsi_interrupter_idx - 1, 0);

  // Configure doorbell addrress
  // usb30_reg_usb30_gsi_dbl_addr_ln[n-1] = address of the GSI doorbell register; (n=1,2,3)
  HWIO_USB30_GSI_DBL_ADDR_Ln_OUTI(gsi_interrupter_idx - 1, (juint32_t)gsi_doorbell_reg);

  // Configure ring base address
  // usb30_reg_usb30_gsi_ring_base_addr_ln[n-1] = base address of the TRB ring; (n=1,2,3)
  HWIO_USB30_GSI_RING_BASE_ADDR_Ln_OUTI(gsi_interrupter_idx - 1, (juint32_t)trb_ring_pa_addr);

  return HSU_JSUCCESS;
}


/**
 * Function name:  snps_dcd_ops_gsi_general_cfg
 * Description: 
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *     @core_req: (IN) CORE request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_dcd_ops_gsi_general_cfg(void)
{
  // Wait for all the GSI endpoint initialization
  // Set general GSI config
  // usb30_reg_usb30_gsi_general_cfg[USB30_GSI_CLK_EN] = 0x1;
  // usb30_reg_usb30_gsi_general_cfg[USB30_GSI_RESTART_DBL_PNTR] = 0x1;
  // usb30_reg_usb30_gsi_general_cfg[USB30_GSI_RESTART_DBL_PNTR] = 0x0;
  HSU_ULOG_FUNC(BAM_LOG);

  HWIO_USB30_GSI_GENERAL_CFG_OUTM(HWIO_USB30_GSI_GENERAL_CFG_USB30_GSI_CLK_EN_BMSK, (1 << HWIO_USB30_GSI_GENERAL_CFG_USB30_GSI_CLK_EN_SHFT));
  HWIO_USB30_GSI_GENERAL_CFG_OUTM(HWIO_USB30_GSI_GENERAL_CFG_USB30_GSI_RESTART_DBL_PNTR_BMSK, (1 << HWIO_USB30_GSI_GENERAL_CFG_USB30_GSI_RESTART_DBL_PNTR_SHFT));
  
  HWIO_USB30_GSI_GENERAL_CFG_OUTM(HWIO_USB30_GSI_GENERAL_CFG_USB30_GSI_RESTART_DBL_PNTR_BMSK, (0 << HWIO_USB30_GSI_GENERAL_CFG_USB30_GSI_RESTART_DBL_PNTR_SHFT));
  
  // Set GSI enable bit
  // usb30_reg_usb30_gsi_general_cfg [USB30_GSI_EN] = 0x1;
  HWIO_USB30_GSI_GENERAL_CFG_OUTM(HWIO_USB30_GSI_GENERAL_CFG_USB30_GSI_EN_BMSK, (1 << HWIO_USB30_GSI_GENERAL_CFG_USB30_GSI_EN_SHFT));
  
  return HSU_JSUCCESS;
}

/**
 * Function name:  snps_dcd_ops_enable
 * Description: Enable DCD by setting up the core and registering interrupt 
                handlers (ISR & DSR)
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @unused: (IN) parameter is obsolete
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_dcd_ops_enable(jdevice_t dev, jbool_t unused)
{
    jresult_t rc;
    snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
    hsu_platform_info_type const * platform = hsu_platform_get_info();

    HSU_USE_PARAM(unused);
    HSU_USE_PARAM(snps_dcd_max_trbs_per_stream);

#if 0
    /* Reset the controller to initial state */
    rc = snps_dcd_reset_core(sc);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, soft_reset_failed");
      return rc;
    }
#endif

// TODO AC check the contents of this function
    /* Configuration for High-Speed and Super-Speed PHYs */
#if 0
    if (snps_dcd_config_phy(sc))
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, config_PHY_failed");
      return EUNKNOWN;
    }
#endif

    /* Initial configuration for the controller */
    if (snps_dcd_config_core(sc))
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, config_core_failed");
      return EUNKNOWN;
    }

    if (HSU_PLATFORM_VIRTIO != platform->hsu_platform_id)
    {
      snps_dcd_dbm_gen_cfg_init(dev);
      snps_dcd_dbm_reset(sc);
    }

    /* Configure the controller to receive events */
    if (snps_dcd_config_events(sc, SNPS_DCD_DEFAULT_EVT_BUF_ID))
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, config_events_failed");
      return EUNKNOWN;
    }

    /* Register the DCD's ISR & DSR with the OS */
    rc = snps_dcd_config_interrupts(sc);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, config_interrupt_failed");
      return rc;
    }

    sc->conn_status = SNPS_DCD_STATE_ENABLED;

    snps_dcd_connect(dev);

    // Set GBUSCFG1 <PIPETRANSMITLIMIT> to 0. Default value = 7
    // This is AXI max burst limit.
    HWIO_GSBUSCFG1_OUTM(HWIO_GSBUSCFG1_PIPETRANSLIMIT_BMSK, (0x0 << HWIO_GSBUSCFG1_PIPETRANSLIMIT_SHFT));
    HWIO_DCTL_OUTM(HWIO_DCTL_LPM_NYET_THRES_BMSK, (0x8 << HWIO_DCTL_LPM_NYET_THRES_SHFT));
    HWIO_DCTL_OUTM(HWIO_DCTL_HIRDTHRES_BMSK, (0x7 << HWIO_DCTL_HIRDTHRES_SHFT));


    /* Enable EP0 for control transfers */
    if (snps_dcd_enable_ep0(sc))
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, error_enabling_ep0");      
      return EUNKNOWN;
    }

    /* Enable VBUS */
    snps_dcd_ops_vbus_connect(dev);

    /* Set RAM1_REG set 0’for Torino */
    HWIO_OUT(RAM1_REG, 0);

    return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_ops_disable
* Description: Disable DCD operation by disabling all USB interrupts
* Parameters: 
*     @dev: (IN) pointer to the device structure
*
* Return value: 0 on success, otherwise an error code
* Scope: local
**/
static void snps_dcd_ops_disable(jdevice_t dev)
{
  snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
  juint16_t count;
  juint8_t event_buffer_index = SNPS_DCD_DEFAULT_EVT_BUF_ID;

  /* Disable EP0 */ 
  if (snps_dcd_disable_ep0(sc))
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, disable_ep0_failed");
    return;
  }

  /* Disable VBUS */
  snps_dcd_ops_vbus_disconnect(dev);

  /* Disable event interrupts */
  HWIO_DEVTEN_OUT(0x0);

  /* Clear the event buffers for the device and all endpoints */
  snps_dcd_set_event_buffer_interrupt(sc, event_buffer_index, FALSE);
  count = snps_dcd_get_event_buffer_count(sc, event_buffer_index);
  snps_dcd_set_event_buffer_count(sc, event_buffer_index, count);

  /* Clear the device configuration */
  HWIO_DCFG_OUT(0x0);

  /* Disable clock getting and power */
  HWIO_GCTL_OUTM(HWIO_GCTL_DSBLCLKGTNG_BMSK, ( 0x1 << HWIO_GCTL_DSBLCLKGTNG_SHFT));

  /* Unregister ISR & DSR */
  if (sc->interrupt_h)
  {
    jinterrupt_teardown(sc->irq_res, sc->interrupt_h);
    sc->interrupt_h = NULL;
  }

  sc->conn_status = SNPS_DCD_STATE_ATTACHED;
}


/**
 * Function name:  snps_dcd_ops_wake_the_host
 * Description: Send remote wake-up to the host
 *              
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @test_mode: (IN) 
 *
 * Return value: jresult code
 * Scope: local
 **/
static jresult_t snps_dcd_ops_wake_the_host(jdevice_t dev)
{
    uint32 link_state;
	snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
    jbus_h snps_bus = j_device_get_bus(dev);
    jresult_t rc;
    core_t * core = (core_t *)sc->core_ctx;

    HSU_ASSERT(core);

    rc = jnotify_os(NOTIFY_DEVICE_REMOTE_WAKEUP, (void *)snps_bus);

    if (JEAGAIN == rc)
    {
       HSU_ULOG(NO_MSG, BUS_LOG, "try_again_exception");
       
       // Still return success since the remote wakeup is being delayed until
       // HW is fully out of low power mode.

       return HSU_JSUCCESS;
    }

    if (HSU_JSUCCESS != rc)
    {
       // All non-success error should be treated as no remote wakeup allowed.
       HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, remote_wakeup_prevented_by_porting_layer_err, rc 0x%08X", rc);
       return JEPERM;
    }
    
    // The Remote wakeup request should be issued 2us after the device goes 
    // into suspend state. In addition, there is 125us delay required 
    // between OUT_L2 and Remote Wakuep
    hsu_os_delay_us(200);
	
	link_state = HWIO_DSTS_INM(HWIO_DSTS_USBLNKST_BMSK) >> HWIO_DSTS_USBLNKST_SHFT;
	
	if(link_state != LINK_STATE_SUSPEND_U3)
	{
		HSU_ULOG(NO_MSG, BUS_LOG, "link already resumed when try wakeup");
		return HSU_JSUCCESS;
	}


    // Set remote wakeup to pending and enable link state change event 
    // temporarily
    // TODO: -- Akash -- Why is this set to TRUE?
    sc->is_remote_wakeup_pending = TRUE;
    snps_dcd_set_usb_link_state_change_event_enable(TRUE);
    
    HWIO_DCTL_OUTM(HWIO_DCTL_ULSTCHNGREQ_BMSK, (0x0 << HWIO_DCTL_ULSTCHNGREQ_SHFT));

    // Initiate remote wakeup
    HWIO_DCTL_OUTM(HWIO_DCTL_ULSTCHNGREQ_BMSK, (0x8 << HWIO_DCTL_ULSTCHNGREQ_SHFT));

    HSU_ULOG(NO_MSG, BUS_LOG, "exit_success");

    return HSU_JSUCCESS;    
}

/**
 * Function name:  snps_dcd_ops_set_test_mode
 * Description: Configure the device into electrical test mode
 *              
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @test_mode: (IN) 
 *
 * Return value: 
 * Scope: local
 **/
static void snps_dcd_ops_set_test_mode(jdevice_t dev, juint16_t test_mode)
{
   HSU_ULOG_1(NO_MSG, SNPS_LOG, "setting_test_mode %u", test_mode);

   /* Set the test mode */
   HWIO_DCTL_OUTM(HWIO_DCTL_TSTCTL_BMSK, (test_mode << HWIO_DCTL_TSTCTL_SHFT));

   return;
}

/**
 * Function name:  snps_dcd_ops_handle_feature
 * Description: Called by CORE when clear/set feature request is received from
 *              the host
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @feature: (IN) the feature to handle
 *     @index: (IN) endpoint number
 *     @set: (IN) 1 for set the feature, 0 for clear it
 *
 * Return value: 
 * Scope: local
 **/
static jresult_t snps_dcd_ops_handle_feature(jdevice_t dev, juint16_t feature, 
    juint16_t usb_ep_address, juint8_t set)
{
    snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
    snps_dcd_ep_desc_t *ep = snps_dcd_get_ep_desc(sc, (juint8_t)usb_ep_address);
    jresult_t rc = JENOTSUP;

    /* Handle the feature request */
    switch (feature)
    {
    case USB_ENDPOINT_HALT:
      {
        if (set)
        {
          /* Setting EP stall */
          HSU_ULOG_1(NO_MSG, BUS_LOG, "ep_halt_set set_stall EP %d", ep->hw_addr);
          rc = snps_dcd_ops_stall_ep(dev, ep->pipe, set);
          if (rc != HSU_JSUCCESS)
          {
            HSU_ULOG_1(NO_MSG, DBG_E_LOG, "stall_ep_failed EP %d", ep->hw_addr);
            return rc;
          }
        }
        else
        {
          if (NULL == ep->streams)
          {
            // Clear Feature ENDPOINT_HALT may come into this path while streams are not initialized yet.
            // Simply return success as abort is not needed in this case
            HSU_ULOG(NO_MSG, BUS_LOG, "DCD, abort_ep_skip_stream_not_init");
          }
          else
          {
            // EndXfer
            HSU_ULOG_1(NO_MSG, BUS_LOG, "ep_halt_clear abort_ep EP %d", ep->hw_addr);
            if (ep->is_in)
            {
              rc = snps_dcd_ops_abort_ep(dev, ep->pipe);
              if (rc != HSU_JSUCCESS)
              {
                HSU_ULOG_1(NO_MSG, DBG_E_LOG, "abort_ep_failed EP %d", ep->hw_addr);
                return rc;
              }
            }
          }

          // Clear Stall
          HSU_ULOG_1(NO_MSG, DBG_E_LOG, "ep_halt clear_stall EP %d", ep->hw_addr);
          rc = snps_dcd_ops_stall_ep(dev, ep->pipe, 0);
          if (rc != HSU_JSUCCESS)
          {
            HSU_ULOG_1(NO_MSG, BUS_LOG, "clear_stall_failed EP %d", ep->hw_addr);
            return rc;
          }
          // Let upper layer to restart the transfer
        }
        break;
      }

#ifdef FEATURE_HS_USB_SNPS_ENABLE_U1U2
    case USB_U1_ENABLE:
      /* setting U1 */
      rc = snps_dcd_set_U1(sc, set);
      break;
    case USB_U2_ENABLE:
      /* setting U2 */
      rc = snps_dcd_set_U2(sc, set);
      break;
#endif /* FEATURE_HS_USB_SNPS_ENABLE_U1U2 */

    case USB_LTM_ENABLE:
      /* setting LTM */
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, LTM_ENABLE_is_not_supported");
      rc = HSU_JSUCCESS;
      break;

    default:
      HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, unknown_feature 0x%X", feature);
      rc = snps_dcd_ops_stall_ep(dev, NULL, set);
      if (rc != HSU_JSUCCESS)
      {
        DBG_W(DSLAVE_DCD, ("snps_dcd_ops_handle_feature: Setting EP stall to %d failed", set));
        return rc;
      }
      rc = JENOTSUP;
      break;
    }
    return rc;
}

/**
* Function name:  snps_dcd_ops_set_address_state
* Description: Called by CORE when set address request received from the host
* Parameters: 
*     @dev: (IN) pointer to the device structure
*     @address: (IN) Device address
*     @wait_for_status_stage: (IN) Tells whether the actual update of the 
*      device address will pend until the completion of the SET ADDRESS 
*      command status stage.
*
* Return value: 0 on success, otherwise an error code 
* Scope: local
**/
static jresult_t snps_dcd_ops_set_address_state(jdevice_t dev, juint16_t address, 
                                                jbool_t wait_for_status_stage)
{
  snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);

  HSU_USE_PARAM(wait_for_status_stage);

  /* Set device address */
  HWIO_DCFG_OUTM(HWIO_DCFG_DEVADDR_BMSK, (address << HWIO_DCFG_DEVADDR_SHFT));

  sc->conn_status = SNPS_DCD_STATE_ADRESSED;

  HSU_ULOG_2(NO_MSG, SNPS_LOG, "addr 0x%08X, wait_for_status_stage %u",
     address, wait_for_status_stage);

  //VI
  /* 5.	Set DBM_GEN_CFG (default is USB3)
  *   a.	for usb3 0x1
  *   b.	for usb2 0x0 ==> TODO AC set to USB 2 for now to test on Virtio */
  HSU_ULOG(NO_MSG, BAM_LOG, "dbm_gen_cfg");

  if (SNPS_DCD_DSTS_CONNECTSPD_SS == (HWIO_INM(DSTS, HWIO_DSTS_CONNECTSPD_BMSK) >> HWIO_DSTS_CONNECTSPD_SHFT))
  {
    HWIO_OUTM(DBM_GEN_CFG, HWIO_DBM_GEN_CFG_DBM_EN_USB3_BMSK, 1UL << HWIO_DBM_GEN_CFG_DBM_EN_USB3_SHFT);
  }
  else
  {
    HWIO_OUTM(DBM_GEN_CFG, HWIO_DBM_GEN_CFG_DBM_EN_USB3_BMSK, 0UL << HWIO_DBM_GEN_CFG_DBM_EN_USB3_SHFT);
  }
  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_ops_get_device_address
* Description: Return device address
* Parameters: 
*     @dev: (IN) pointer to the device structure
*     @address_ptr: (OUT) Device address
*
* Return value: 0 on success, otherwise an error code 
* Scope: local
**/
static jresult_t snps_dcd_ops_get_device_address(jdevice_t dev, juint16_t* address_ptr)
{
  /* Read device address */
  *address_ptr = (juint16_t) (HWIO_DCFG_INM(HWIO_DCFG_DEVADDR_BMSK)
    >> HWIO_DCFG_DEVADDR_SHFT);

  return HSU_JSUCCESS;
}

/**
 * Function name:  snps_dcd_ops_set_config_state
 * Description: Called by CORE when set configuration completed
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @config: (IN) The selected configuration value
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_dcd_ops_set_config_state(jdevice_t dev, juint8_t config)
{
    snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
//    snps_dcd_ep_desc_t* ep0_in = &sc->ep_desc_table[SNPS_DCD_EP0_IN_ADDR];
//    juint8_t retry;
    jresult_t rc = HSU_JSUCCESS;
  
    HSU_ULOG(NO_MSG, SNPS_LOG, "set_config");

    if (config == 0)
    {
      return rc;
    }

#if 0
    /* Wait for Rx FIFO to drain */
    for (retry = 0; retry < SNPS_DCD_RX_FIFO_DRAIN_RETRIES; retry++)
    {
      DSTS.value = HWIO_DSTS_IN;
      if (DSTS.reg.RxFIFOEmpty)
      {
        break;
      }
      jdelay_us(SNPS_DCD_RX_FIFO_DRAIN_DELAY_US);
    }

    /* Rx FIFO didn't drain */
    if (retry == SNPS_DCD_RX_FIFO_DRAIN_RETRIES)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, device_usb_reset_failed");
      return EUNKNOWN;
    }

    /* Re-initialize the TX-FIFO allocation through physical ep 1  */
    rc = snps_dcd_config_ep_cfg(ep0_in, FALSE);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_config_ep_cfg_failed");
      return rc;
    }
#endif

    /* Reset resources counter inside the controller. */
    rc = snps_dcd_config_device_resources(sc, SNPS_DCD_RES_NUM_CONFIGURED);
    if (rc != HSU_JSUCCESS)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_config_device_resource_failed");
      return rc;
    }

    sc->conn_status = SNPS_DCD_STATE_CONFIGURED;

    return rc;
}

/**
* Function name:  snps_dcd_ops_get_max_packet0
* Description: Return endpoint 0 max packet size per the connection speed
* Parameters: 
*     @dev: (IN) pointer to the device structure
*
* Return value: Endpoint 0 max packet size
* Scope: local
**/
static juint8_t snps_dcd_ops_get_max_packet0(jdevice_t dev)
{
  snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);

  return (juint8_t)core_get_max_packet_size(PIPE_CONTROL, sc->speed);
}

/**
* Function name:  snps_dcd_ops_handle_tx_completion
* Description: Handle EP timeout
* Parameters:
*     @dev: (IN) pointer to the device structure
*     @pipe: (IN) CORE pipe context
*
* Return value: 0 on success, otherwise an error code
* Scope: local
**/
static void snps_dcd_ops_handle_tx_completion(jdevice_t dev, pipe_desc_t *pipe)
{
  snps_dcd_ep_desc_t *ep;

  HSU_ASSERT(pipe != NULL);

  ep = (snps_dcd_ep_desc_t *)pipe->dcd_handle;
  snps_dcd_handle_timeout(ep);
}

/**
* Function name:  snps_dcd_ops_register_events
* Description: Register the CORE with DCD events
* Parameters: 
*     @dev: (IN) pointer to the device structure
*
* Return value: None
* Scope: Local
**/
void snps_dcd_ops_register_events(jdevice_t dev, const void* events)
{
  snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);

  if (sc == NULL)
  {
    return;
  }

  sc->events = (core_events_t*)events;

}

/*============================= Driver inteface =============================*/

/**
 * Function name:  snps_usb3_dcd_probe
 * Description: Check if dev is an SNPS controller 
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 when there is a match, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_usb3_dcd_probe(jdevice_t dev)
{
  HSU_USE_PARAM(dev);
  return HSU_JSUCCESS;
}

/**
 * Function name:  snps_usb3_dcd_detach
 * Description: Detach a controller
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t snps_usb3_dcd_detach(jdevice_t dev)
{
  snps_dcd_soft_dc_t *sc;
  jbus_h snps_dcd_bus;
  juint32_t i;

  if (!dev)
      return HSU_JSUCCESS;

  snps_dcd_bus = j_device_get_bus(dev);
  sc = (snps_dcd_soft_dc_t*)j_device_get_softc(dev);
  if (!sc)
      return HSU_JSUCCESS;

  if (sc->conn_status < SNPS_DCD_STATE_ATTACHED)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, not_attached");
    return HSU_JSUCCESS;
  }

  core_disable_by_ctx(sc->core_ctx);

  if (sc->core_ctx)
  {
    core_unregister_dcd(sc->core_ctx);
  }

  if (sc->lock)
  {
      jspinlock_uninit(sc->lock);
  }

  /* Free the event buffer */
  for (i = 0; i < SNPS_DCD_MAX_NUM_EVENT_BUF; i++)
  {
    if (sc->event_buffer[i].cookie)
    {
      jdma_free(sc->event_buffer[i].cookie);
      sc->event_buffer[i].cookie = NULL;
    }
  }

  /* Free memory allocated for EP table, and per EP. */
  if (sc->ep_desc_table)
  {
      juint32_t i;
      for (i = 0 ; i < sc->max_available_eps; i++)
      {
        snps_dcd_ep_desc_t *ep = &sc->ep_desc_table[i];
        if (ep->streams)
        {
          if (ep->streams->soft_trb_array.cookie)
          {
            jdma_free(ep->streams->soft_trb_array.cookie);
            ep->streams->soft_trb_array.cookie = NULL;
          }

          jfree(ep->streams);
          ep->streams = NULL;
        }
      }

      jfree(sc->ep_desc_table);
      sc->ep_desc_table = NULL;
  }

  if (sc->reg_res)
  {
      jbus_release_resource(snps_dcd_bus, sc->reg_res);
  }

  if (sc->irq_res)
  {
      jbus_release_resource(snps_dcd_bus, sc->irq_res);
  }

  sc->conn_status = SNPS_DCD_STATE_DETACHED;

  return HSU_JSUCCESS;
}

/**
 * Function name:  snps_dcd_ops_set_bam_pipe_params
 * Description: Function exposed to AL layer for establishing DBM EPs HW EPs associations
 * Parameters: 
 * Return value:
 **/
static void snps_dcd_ops_set_bam_pipe_params(jdevice_t dev, void *connection, pipe_desc_t *out_pipe, uint32 producer_pipe_index, 
                                    uint32 producer_data_fifo_addr, uint32 producer_data_fifo_size,
                                    uint32 producer_desc_fifo_addr, uint32 producer_desc_fifo_size,
                                    pipe_desc_t *in_pipe, uint32 consumer_pipe_index,
                                    uint32 consumer_data_fifo_addr, uint32 consumer_data_fifo_size,
                                    uint32 consumer_desc_fifo_addr, uint32 consumer_desc_fifo_size)
{
  snps_dcd_ep_desc_t *ep;
  static boolean snps_dcd_ops_set_bam_pipe_params_run_once = FALSE;

  if (!out_pipe && !in_pipe)
  {
    HSU_ERR_FATAL("Invalid pipe used for BAM2BAM", 0, 0, 0);
  }
  if (!dev)
  {
    HSU_ERR_FATAL("NULL device structure", 0, 0, 0);
  }

  if (FALSE == snps_dcd_ops_set_bam_pipe_params_run_once)
  {
    uint32 i;
    for (i = 0; i <= HWIO_DBM_EP_CFG_ADDRn_MAXn; i++)
    {
      dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[i].bam_pipe_num = HSU_UNDEFINED_EP_VAL;
      snps_dcd_ops_set_bam_pipe_params_run_once = TRUE;
    }
  }

  // Do not set hw_ep_num when UL_BAM_BYPASS flag is set.
  if(out_pipe)
  {
    ep = (snps_dcd_ep_desc_t *)out_pipe->dcd_handle;
    HSU_ASSERT(ep);
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[producer_pipe_index].connection           = connection;
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[producer_pipe_index].hw_ep_num            = ep->hw_addr;
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[producer_pipe_index].is_bam_pipe_producer = TRUE;
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[producer_pipe_index].bam_pipe_num         = producer_pipe_index;
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[producer_pipe_index].data_fifo_addr       = producer_data_fifo_addr;
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[producer_pipe_index].data_fifo_size       = producer_data_fifo_size;
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[producer_pipe_index].desc_fifo_addr       = producer_desc_fifo_addr;
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[producer_pipe_index].desc_fifo_size       = producer_desc_fifo_size;
  }

  if(in_pipe)
  {
    ep = (snps_dcd_ep_desc_t *)in_pipe->dcd_handle;
    HSU_ASSERT(ep);
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[consumer_pipe_index].connection           = connection;
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[consumer_pipe_index].hw_ep_num            = ep->hw_addr;
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[consumer_pipe_index].is_bam_pipe_producer = FALSE;
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[consumer_pipe_index].bam_pipe_num         = consumer_pipe_index;
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[consumer_pipe_index].data_fifo_addr       = consumer_data_fifo_addr;
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[consumer_pipe_index].data_fifo_size       = consumer_data_fifo_size;
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[consumer_pipe_index].desc_fifo_addr       = consumer_desc_fifo_addr;
    dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[consumer_pipe_index].desc_fifo_size       = consumer_desc_fifo_size;
  }

  return;
}

uint8 snps_dcd_get_dbm_ep_from_hw_ep(uint32 hw_ep_num)
{
  uint8 i;
  for (i = 0; i <= HWIO_DBM_EP_CFG_ADDRn_MAXn; i++)
  {
    if (hw_ep_num == dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[i].hw_ep_num)
    {
      return i;
    }
  }
  /* caller should guard and ensure HW EP is for BAM */
  return HSU_UNDEFINED_EP_VAL;
}

void snps_dcd_dbm_get_bam_info(uint8 dbm_ep_num, dbm_bam_info_type **info)
{
  if (dbm_ep_num <= HWIO_DBM_EP_CFG_ADDRn_MAXn)
  {
    if (HSU_UNDEFINED_EP_VAL != dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[dbm_ep_num].bam_pipe_num)
    {
      *info = &dcd_snps_eps_dbm_ctx.dbm_to_usb_hw_ep_map[dbm_ep_num];
    }
    else
    {
      HSU_ERR_FATAL("Invalid HW EP detected.  Check dcd_set_bam_pipe_params()", 0, 0, 0);
    }
  }
  else
  {
    HSU_ERR_FATAL("HW EP to DBM EP entry not found.  dbm_ep_num: %d", dbm_ep_num, 0, 0);
  }
  return;
}

/* 4.4.1.6 Torino DBM Initialization sequence */
/* 4.4.1.6.1 DBM Enable */
/* NOTE:
 * For BAM pipe n set DBM configuration for DBM EP n (example for pipe 2, use dbm ep 2)
 * BAM should be configured before enabling DBM (see BAM_NDP HPG) */
/* 1.	Set GENERAL_CFG.DBM_EN = ‘1’*/
/* 2.	Set RAM1_REG – set ‘0’ for Torino
     a.	internal RAM use for USB3 BAM_NDP PipeMemory - set to 0x7
     b.	external BAM users using RAM13 as Pipe Memory – set to 0x4
     c.	else set to 0  ===> We'll use this setting for Torino */
/* 3. Set DBM_GEVNTADR */
/* 4.	Set DBM_GEVNTSIZ */
/* 5.	Set DBM_GEN_CFG (default is USB3)
 *   a.	for usb3 0x1
 *   b.	for usb2 0x0 TODO AC */
/* 6.	set DBM_DATA_FIFO_ADDR_EN per DBM EP */
/* 7.	set DBM_DATA_FIFO_SIZE_EN per DBM EP */
/* This API should be called after enumeration and after BAM configuration */
void snps_dcd_ops_dbm_init(jdevice_t dev)
{
  /* 4.4.1.6.1 DBM Enable */
//MOVED  void *addr;
  dcd_snps_eps_dbm_ctx_type *eps_dbm_ctx;
    
//MOVED  addr = (sc->event_buffer.paddr);
//MOVED  if (NULL == addr)
//MOVED  {
//MOVED    // Core event buffer not initialized, do not attempt to initialize DBM
//MOVED    return;
//MOVED  }

  eps_dbm_ctx = snps_dcd_get_snps_eps_dbm_ctx_hnd();

  /* Initialize DBM once in sniffing mode */
  if (FALSE == eps_dbm_ctx->is_dbm_initialized)
  {
    uint32 i = 0;
    eps_dbm_ctx->is_dbm_initialized = TRUE;

    HSU_ULOG_FUNC(BAM_LOG);
  
    /* 1.	Set GENERAL_CFG.DBM_EN = 1’needs to be moved to earlier in bootup */
  
    /* 2.	Set RAM1_REG set 0’for Torino */
    //TODO AC this is moved upon connect
  
    /* 3. Set DBM_GEVNTADR */
//HPG    HWIO_OUT(DBM_GEVNTADR, (uint32)&addr);
    //TODO AC move to where event buffer is initialized
//VI    HWIO_OUT(DBM_GEVNTADR_LSB, (uint32)&addr);
//VI    HWIO_OUT(DBM_GEVNTADR_MSB, 0);
//VI  
//VI    /* 4.	Set DBM_GEVNTSIZ */
//VI    HWIO_OUT(DBM_GEVNTSIZ, (uint32)sc->event_buffer.buffer_size);
  
//MOVED TO SET ADDRESS/CONNECT    /* 5.	Set DBM_GEN_CFG (default is USB3)
//MOVED TO SET ADDRESS/CONNECT    *   a.	for usb3 0x1
//MOVED TO SET ADDRESS/CONNECT    *   b.	for usb2 0x0 ==> TODO AC set to USB 2 for now to test on Virtio */
//MOVED TO SET ADDRESS/CONNECT    HWIO_OUTM(DBM_GEN_CFG, HWIO_DBM_GEN_CFG_DBM_EN_USB3_BMSK, 0UL << HWIO_DBM_GEN_CFG_DBM_EN_USB3_SHFT);
  
    //+++++++++++++++ SNIFFING MODE
//    /* 6.	set DBM_DATA_FIFO_ADDR_EN per DBM EP */
//    /* Set to 0 to enable sniffing mode for each DBM EP such that DBM can determine DATA_FIFO_ADDR from BAM configuration */
//    /* Non-HPG note: Apply the same setting to ALL DBM controlled EPs */
//    HWIO_OUTM(DBM_DATA_FIFO_ADDR_EN, HWIO_DBM_DATA_FIFO_ADDR_EN_DBM_DATA_FIFO_ADDR_ENN_BMSK, 0UL << HWIO_DBM_DATA_FIFO_ADDR_EN_DBM_DATA_FIFO_ADDR_ENN_SHFT);
//    
//    /* 7.	set DBM_DATA_FIFO_SIZE_EN per DBM EP */
//    /* Non-HPG note: Apply the same setting to ALL DBM controlled EPs */
//    HWIO_OUTM(DBM_DATA_FIFO_SIZE_EN, HWIO_DBM_DATA_FIFO_SIZE_EN_DBM_DATA_FIFO_SIZE_ENN_BMSK, 0UL << HWIO_DBM_DATA_FIFO_SIZE_EN_DBM_DATA_FIFO_SIZE_ENN_SHFT);
    //--------------- SNIFFING MODE
    //
    //+++++++++++++++ NON-SNIFFING MODE
    /* 6.	set DBM_DATA_FIFO_ADDR_EN per DBM EP */
    /* Set to 0 to enable sniffing mode for each DBM EP such that DBM can determine DATA_FIFO_ADDR from BAM configuration */
    /* Non-HPG note: Apply the same setting to ALL DBM controlled EPs */

//TODO AMSS    HWIO_OUTM(DBM_DATA_FIFO_ADDR_EN, HWIO_DBM_DATA_FIFO_ADDR_EN_DBM_DATA_FIFO_ADDR_ENN_BMSK, 0xF << HWIO_DBM_DATA_FIFO_ADDR_EN_DBM_DATA_FIFO_ADDR_ENN_SHFT);
    HWIO_OUTM(DBM_DATA_FIFO_ADDR_EN, HWIO_DBM_DATA_FIFO_ADDR_EN_DBM_DATA_FIFO_ADDR_ENN_BMSK, 0xFF << HWIO_DBM_DATA_FIFO_ADDR_EN_DBM_DATA_FIFO_ADDR_ENN_SHFT);
    
    /* 7.	set DBM_DATA_FIFO_SIZE_EN per DBM EP */
    /* Non-HPG note: Apply the same setting to ALL DBM controlled EPs */

//TODO AMSS    HWIO_OUTM(DBM_DATA_FIFO_SIZE_EN, HWIO_DBM_DATA_FIFO_SIZE_EN_DBM_DATA_FIFO_SIZE_ENN_BMSK, 0xF << HWIO_DBM_DATA_FIFO_SIZE_EN_DBM_DATA_FIFO_SIZE_ENN_SHFT);
    HWIO_OUTM(DBM_DATA_FIFO_SIZE_EN, HWIO_DBM_DATA_FIFO_SIZE_EN_DBM_DATA_FIFO_SIZE_ENN_BMSK, 0xFF << HWIO_DBM_DATA_FIFO_SIZE_EN_DBM_DATA_FIFO_SIZE_ENN_SHFT);

    //TODO AC fix logic to properly iterate through max # of HW accel endpoints/ECM instances
    for (i = 0; i <= HWIO_DBM_EP_CFG_ADDRn_MAXn; i++)
    {
      if ((0 != eps_dbm_ctx->dbm_to_usb_hw_ep_map[i].data_fifo_addr) && (0 != eps_dbm_ctx->dbm_to_usb_hw_ep_map[i].desc_fifo_addr))
      {
        HWIO_DBM_DATA_SIZE_ADDRn_OUTI(i, eps_dbm_ctx->dbm_to_usb_hw_ep_map[i].data_fifo_size);
        HWIO_DBM_DATA_FIFO_ADDR_LSBn_OUTI(i, eps_dbm_ctx->dbm_to_usb_hw_ep_map[i].data_fifo_addr);
      }
    }
    //--------------- NON-SNIFFING MODE
  }
  return;
}

/* This API should be called after enumeration and after BAM configuration */
/* This is called after DBM for IPA has been initialized */
void snps_dcd_ops_dbm_init_qdss(jdevice_t dev, uint32 consumer_pipe_index, uint32 consumer_data_fifo_addr, uint32 consumer_data_fifo_size)
{
  
  if(consumer_data_fifo_addr && consumer_data_fifo_size)
  {  
    HWIO_DBM_DATA_SIZE_ADDRn_OUTI(consumer_pipe_index, consumer_data_fifo_size);
    HWIO_DBM_DATA_FIFO_ADDR_LSBn_OUTI(consumer_pipe_index, consumer_data_fifo_addr);
  }
  else
  {
    HSU_ERR_FATAL("Data fifo address or size invalid !", 0, 0, 0);  
  }
}
/* 4.4.1.6.2 EP init */
/* For BAM pipe n set DBM configuration for DBM EP n (example for pipe 2, use dbm ep 2) */
/* regular EP --> DBM EP */
/* 1.	Configure DBM_REGFILE_DBM_EP_CFG_ADDRn to be DBM enabled, and set all other field to valid values. */
/* 2.	Enable the BAM pipe. */
void snps_dcd_ops_dbm_ep_init(jdevice_t dev, uint32 dbm_ep_num)
{
  /* 1.	set DBM_DATA_FIFO_ADDR_LSB. Register does not need to be written since sniffing mode is enabled. */
  /* 2.	set DBM_DATA_FIFO_ADDR_MSB. Register does not need to be written since sniffing mode is enabled. */
  /* 3. set DBM_DATA_SIZE. Register does not need to be written since sniffing mode is enabled. */
  /* 4. set DBM_EP_CFG – for pipe n configure DBM_EP_CFGn
   *   a.	DBM_BAM_P_SW_RST_DISABLE = ‘0’
   *   b.	DBM_ZLT_DISABLE = ‘0’
   *   c.	DBM_LINK_TAG_DISABLE = ‘0’
   *   d.	DBM_SINGLE_MODE = ‘0’
   *   e.	DBM_INT_RAM_ACC = ‘0’ for Torino
   *   f.	DBM_DISABLE_WB = ‘1’
   *   g.	For producer pipr set DBM_PRODUCER to ‘1’
   *   h.	Set USB3_EPNUM
   *   i.	Set DBM_EN_EP to ‘1’ */
//  uint32 reg = 0;
  dbm_bam_info_type *dbm_bam_info_ptr = NULL; /* to be populated by API */

  HSU_ULOG_1(NO_MSG, BAM_LOG, "dbm_ep_num %u", dbm_ep_num);

  snps_dcd_dbm_get_bam_info(dbm_ep_num, &dbm_bam_info_ptr);

  if (dbm_ep_num > HWIO_DBM_EP_CFG_ADDRn_MAXn)
  {
    HSU_ERR_FATAL("dbm_ep_num out of range!", 0, 0, 0);
  }
  if (dbm_bam_info_ptr->hw_ep_num >= HSU_MAX_NUM_OF_EP)
  {
    HSU_ERR_FATAL("dbm_bam_info_ptr->hw_ep_num out of range!", 0, 0, 0);
  }

  if (TRUE == dcd_snps_eps_dbm_ctx.is_dbm_ep_initialized[dbm_ep_num])
  {
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, DBM_EP %d, already_initialized", dbm_ep_num);
    return;
  }

  dcd_snps_eps_dbm_ctx.is_dbm_ep_initialized[dbm_ep_num] = TRUE;

  /* Since all fields are explictly populated, optimize via a single write to the HW */
//  reg = (0UL << HWIO_DBM_EP_CFG_ADDRn_DBM_BAM_P_SW_RST_DISABLE_SHFT) |
//        (0UL << HWIO_DBM_EP_CFG_ADDRn_DBM_ZLT_DISABLE_SHFT) |
//        (0UL << HWIO_DBM_EP_CFG_ADDRn_DBM_LINK_TAG_DISABLE_SHFT) |
//        (0UL << HWIO_DBM_EP_CFG_ADDRn_DBM_SINGLE_MODE_SHFT) |
//        (0UL << HWIO_DBM_EP_CFG_ADDRn_DBM_INT_RAM_ACC_SHFT) |
//        (1UL << HWIO_DBM_EP_CFG_ADDRn_DBM_DISABLE_WB_SHFT) |
//        (dbm_bam_info_ptr->hw_ep_num << HWIO_DBM_EP_CFG_ADDRn_USB3_EPNUM_SHFT) |
//        ((dbm_bam_info_ptr->is_bam_pipe_producer)?(1UL << HWIO_DBM_EP_CFG_ADDRn_DBM_PRODUCER_SHFT):(0UL << HWIO_DBM_EP_CFG_ADDRn_DBM_PRODUCER_SHFT)) |
//        (1UL << HWIO_DBM_EP_CFG_ADDRn_DBM_EN_EP_SHFT);
//  HWIO_DBM_EP_CFG_ADDRn_OUTI(dbm_ep_num, reg);

  HWIO_DBM_EP_CFG_ADDRn_OUTMI(dbm_ep_num, HWIO_DBM_EP_CFG_ADDRn_DBM_BAM_P_SW_RST_DISABLE_BMSK, 0UL << HWIO_DBM_EP_CFG_ADDRn_DBM_BAM_P_SW_RST_DISABLE_SHFT);
  HWIO_DBM_EP_CFG_ADDRn_OUTMI(dbm_ep_num, HWIO_DBM_EP_CFG_ADDRn_DBM_ZLT_DISABLE_BMSK, 0UL << HWIO_DBM_EP_CFG_ADDRn_DBM_ZLT_DISABLE_SHFT);
  HWIO_DBM_EP_CFG_ADDRn_OUTMI(dbm_ep_num, HWIO_DBM_EP_CFG_ADDRn_DBM_LINK_TAG_DISABLE_BMSK, 0UL << HWIO_DBM_EP_CFG_ADDRn_DBM_LINK_TAG_DISABLE_SHFT);
  HWIO_DBM_EP_CFG_ADDRn_OUTMI(dbm_ep_num, HWIO_DBM_EP_CFG_ADDRn_DBM_SINGLE_MODE_BMSK, 0UL << HWIO_DBM_EP_CFG_ADDRn_DBM_SINGLE_MODE_SHFT);
  HWIO_DBM_EP_CFG_ADDRn_OUTMI(dbm_ep_num, HWIO_DBM_EP_CFG_ADDRn_DBM_INT_RAM_ACC_BMSK, 0UL << HWIO_DBM_EP_CFG_ADDRn_DBM_INT_RAM_ACC_SHFT);


#ifdef FEATURE_HS_USB_DBM_DISABLE_WB_REG_AVAILABLE
  HWIO_DBM_EP_CFG_ADDRn_OUTMI(dbm_ep_num, HWIO_DBM_EP_CFG_ADDRn_DBM_DISABLE_WB_BMSK, 1UL << HWIO_DBM_EP_CFG_ADDRn_DBM_DISABLE_WB_SHFT);
#endif
  HWIO_DBM_EP_CFG_ADDRn_OUTMI(dbm_ep_num, HWIO_DBM_EP_CFG_ADDRn_USB3_EPNUM_BMSK, dbm_bam_info_ptr->hw_ep_num << HWIO_DBM_EP_CFG_ADDRn_USB3_EPNUM_SHFT);
  HWIO_DBM_EP_CFG_ADDRn_OUTMI(dbm_ep_num, HWIO_DBM_EP_CFG_ADDRn_DBM_PRODUCER_BMSK, (dbm_bam_info_ptr->is_bam_pipe_producer)?(1UL << HWIO_DBM_EP_CFG_ADDRn_DBM_PRODUCER_SHFT):(0UL << HWIO_DBM_EP_CFG_ADDRn_DBM_PRODUCER_SHFT));
  HWIO_DBM_EP_CFG_ADDRn_OUTMI(dbm_ep_num, HWIO_DBM_EP_CFG_ADDRn_DBM_EN_EP_BMSK, 1UL << HWIO_DBM_EP_CFG_ADDRn_DBM_EN_EP_SHFT);
  return;
}

/* 4.4.1.7.1 SW Reset sequence */
/* 1. Before generating DBM reset, SW should use Endxfer command for the USB EP. */
/* 2. SW can set a general DBM reset or a DBM EP reset, by writing to DBM_SOFT_RESET register. */
/* 3.	SW should reset the BAM or the BAM PIPEs connected to the DBM EP that were reset. */
/* 4.	SW should re-initialize the DBM after DBM SW reset (or init the DBM EP configuration after DBM EP reset) */
/* NOTE	Starting Torino bam pipe reset causes dbm soft reset for corresponding dbm ep – unless disabled using DBM_EP_CFG.DBM_BAM_P_SW_RST_DISABLE. */
void snps_dcd_ops_dbm_ep_reset(jdevice_t dev, uint32 dbm_ep_num)
{
  uint32 i;
  /* This API is not intended to be called when BAM resets becaues DBM_EP_CFG.DBM_BAM_P_SW_RST_DISABLE bit is set to 1 */

  /* 1. Before generating DBM reset, SW should use Endxfer command for the USB EP. */
  //ENDXFER should have been done by abort ep

  /* 2. SW can set a general DBM reset or a DBM EP reset, by writing to DBM_SOFT_RESET register. */
  /* For optimization write to register value explicity as opposed to via macros as all fields are known */
  if (dbm_ep_num > HWIO_DBM_EP_CFG_ADDRn_MAXn)
  {
    HSU_ERR_FATAL("dbm_ep_num out of range!", 0, 0, 0);
  }
  dcd_snps_eps_dbm_ctx.is_dbm_ep_initialized[dbm_ep_num] = FALSE;

  HWIO_OUT(DBM_SOFT_RESET, dbm_ep_num);
  hsu_os_delay_us(1000);
  HWIO_OUT(DBM_SOFT_RESET, 0);
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
  // Completed as this API is controlled by the state machine

  /* 4.	SW should re-initialize the DBM after DBM SW reset (or init the DBM EP configuration after DBM EP reset) */
  // Completed as this API is controlled by the state machine


  return;
}

/**
 * Function name:  snps_usb3_dcd_attach
 * Description: Attaches a new controller
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_usb3_dcd_attach(jdevice_t dev)
{    
    snps_dcd_soft_dc_t *sc = j_device_get_softc(dev);
    jbus_h snps_dcd_bus = j_device_get_bus(dev);
    jint_t resource_id = 0;
    jresult_t rc = 0;
    hsu_platform_info_type const * platform_info = hsu_platform_get_info();

    DBG_I(DSLAVE_DCD, ("snps_dcd_attach: Attaching controller: %s\n", 
        j_device_get_nameunit(dev)));

    if (sc->conn_status >= SNPS_DCD_STATE_ATTACHED)
    {
      HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, already_attached");
      return EUNKNOWN;
    }

    snps_dcd_sc = sc;
    HSU_USE_PARAM(snps_dcd_sc);

    /* initializing device controller data */
    sc->dev = dev;
    switch (platform_info->hsu_platform_id)
    {
      case HSU_PLATFORM_VIRTIO:
      case HSU_PLATFORM_RUMI:
      {
        sc->speed = SPEED_HIGH;
      }
      break;
      default:
      {
        if (hsu_clkmgr_is_hs_only_mode())
        {
          sc->speed = SPEED_HIGH;
        }
        else
        {
          sc->speed = SPEED_SUPER;
        }
      }
      break;
    }
    sc->conn_status = SNPS_DCD_STATE_ATTACHED;

    sc->enumeration_is_allowed = FALSE;
    sc->enumeration_is_pending = FALSE;

    /* Getting registers base address */
    sc->reg_res = j_bus_alloc_resource(snps_dcd_bus, SYS_RES_MEMORY, resource_id);
    sc->irq_res = j_bus_alloc_resource(snps_dcd_bus, SYS_RES_IRQ, resource_id);
    if (!sc->reg_res || !sc->irq_res)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, error_allocating_bus_resource_for_dcd");
        rc = JENOMEM;
        goto Error;
    }

    /* Read controller configuration parameters */
    rc = snps_dcd_read_general_params(sc);
    if (rc != HSU_JSUCCESS)
      goto Error;

    /* Initialize EP descriptor table */
    rc = snps_dcd_init_endpoints(sc);
    if (rc != HSU_JSUCCESS)
        goto Error;

    /* Initialize the event buffer SW structure */
    rc = snps_dcd_init_event_buffer(sc, SNPS_DCD_DEFAULT_EVT_BUF_ID, 0);
    if (rc != HSU_JSUCCESS)
      goto Error;

    rc = jspinlock_init(&sc->lock);
    if (rc != HSU_JSUCCESS)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, failed_to_allocate_spinlock, rc %d", rc);
        goto Error;
    }
    
    /* Register controller with the CORE */
    rc = core_register_dcd(dev, &ops, &sc->core_ctx);
    if (rc != HSU_JSUCCESS)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, attach_failed");        
        goto Error;
    }

    DBG_I(DSLAVE_DCD, ("snps_dcd_attach: Controller attached, available endpoints %d\n",
        sc->max_available_eps)); 
    return HSU_JSUCCESS;

Error:
    snps_usb3_dcd_detach(dev);
    return rc;
}

/**
 * Function name:  snps_usb3_dcd_suspend
 * Description: Suspends the controller
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_usb3_dcd_suspend(jdevice_t dev)
{
  HSU_USE_PARAM(dev);
  /* Not implemented */
  return HSU_JSUCCESS;
}

/**
 * Function name:  snps_usb3_dcd_resume
 * Description: Resumes the controller
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t snps_usb3_dcd_resume(jdevice_t dev)
{
  HSU_USE_PARAM(dev);
  /* Not implemented */
  return HSU_JSUCCESS;
}

/**
* Function name:  snps_usb3_dcd_ioctl
* Description: Handle IOCTL
* Parameters: 
*     @self: (IN) 
*     @ioctl: (IN) IOCTL type
*     @arg: (IN) Argument to the IOCTL
*
* IOCTL Request: DRV_IOCTL_SET_ISOCH_DELAY
* arg type:      (IN) uint16*
* arg value:     Isochronous delay in ns
*
* IOCTL Request: DRV_IOCTL_SET_SEL
* arg type:      (IN) usb_set_SEL_request*
* arg value:     See usb_set_SEL_request struct
*
* Return value: 0 on success, otherwise an error code
* Scope: local
**/
static jresult_t snps_usb3_dcd_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg)
{
  snps_dcd_soft_dc_t *sc;
  jresult_t rc = HSU_JSUCCESS;

  if (!self)
      goto Exit;

  sc = (snps_dcd_soft_dc_t *)j_device_get_softc(self);
  if (!sc)
      goto Exit;
  
  /* Handle the IOCTL */
  switch(ioctl)
  {
  case DRV_IOCTL_DEV_ENABLE:
      rc = core_enable_by_ctx(sc->core_ctx);
      break;
  case DRV_IOCTL_DEV_DISABLE:
      core_disable_by_ctx(sc->core_ctx);
      break;
  case DRV_IOCTL_DEV_SET_PARAMS:
      break;
  case DRV_IOCTL_SET_ISOCH_DELAY:
    HSU_ULOG(NO_MSG, SNPS_LOG, "set_isoch_delay_is_not_supported");
    return JENOTSUP;
  case DRV_IOCTL_SET_SEL:
    {
      rc = snps_dcd_handle_set_SEL_request(sc, (struct usb_set_SEL_request *)arg);
      if (rc != HSU_JSUCCESS)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_handle_set_SEL_req_failed");
        return rc;
      }
    }
    break;

  default:
      HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, unsupported_IOCTL 0x%X", ioctl);
      rc = JEINVAL;
      break;
  }

Exit:
  return rc;
}

dcd_snps_eps_dbm_ctx_type* snps_dcd_get_snps_eps_dbm_ctx_hnd(void)
{
  return &dcd_snps_eps_dbm_ctx;
}

static void snps_dcd_dbm_gen_cfg_init(jdevice_t dev)
{
  HSU_ULOG_FUNC(BAM_LOG);
  /* 1.	Set GENERAL_CFG.DBM_EN = 1’*/
  HWIO_OUTM(GENERAL_CFG, HWIO_GENERAL_CFG_DBM_EN_BMSK, 1UL << HWIO_GENERAL_CFG_DBM_EN_SHFT);
}

void snps_dcd_dbm_set_event_buffer_address_and_size(snps_dcd_soft_dc_t *sc)
{
  void *addr;
  addr = (sc->event_buffer[SNPS_DCD_DEFAULT_EVT_BUF_ID].paddr);
  if (NULL == addr)
  {
    // Core event buffer not initialized, do not attempt to initialize DBM
    HSU_ERR_FATAL("Event buffer not initialized", 0, 0, 0);
    return;
  }
  /* 3. Set DBM_GEVNTADR */
  HWIO_OUT(DBM_GEVNTADR_LSB, (uint32)addr);
  HWIO_OUT(DBM_GEVNTADR_MSB, 0);
  
  /* 4.	Set DBM_GEVNTSIZ */
  HWIO_OUT(DBM_GEVNTSIZ, (uint32)sc->event_buffer[SNPS_DCD_DEFAULT_EVT_BUF_ID].buffer_size);
}

static void snps_dcd_ops_allow_enumeration(jdevice_t dev)
{
  snps_dcd_soft_dc_t *sc = j_device_get_softc(dev);

  if (sc == NULL)
  {
    return;
  }

  sc->enumeration_is_allowed = TRUE;

  if (sc->core_ctx)
  {
    HSU_ULOG_1(NO_MSG, BUS_LOG, "core_num %u", ((core_t *)(sc->core_ctx))->core_number);
  }

  if (sc->enumeration_is_pending)
  {
    snps_dcd_ops_vbus_connect(dev);
  }
  return;
}

static void snps_dcd_ops_disallow_enumeration(jdevice_t dev)
{
  snps_dcd_soft_dc_t *sc = j_device_get_softc(dev);

  if (sc == NULL)
  {
    return;
  }

  if (sc->core_ctx)
  {
    HWIO_DCTL_OUTM(HWIO_DCTL_RUN_STOP_BMSK, (0x0UL << HWIO_DCTL_RUN_STOP_SHFT));
    HSU_ULOG_1(NO_MSG, BUS_LOG, "core_num %u", ((core_t *)(sc->core_ctx))->core_number);
    
  }

  sc->enumeration_is_allowed = FALSE;
  return;
}

/**
 * Function name:  snps_dcd_set_usb_link_state_change_event_enable
 * Description: Enable / Disable link state change event for U0/U1/U2 etc.
 *              
 * Parameters: 
 *     @enable: (IN) TRUE to enable U0/U1/U2 events, FALSE otherwise
 *
 * Scope: global
 **/
void snps_dcd_set_usb_link_state_change_event_enable(jbool_t enable)
{
  // Enabled U0/U1/U2 events
  HWIO_DEVTEN_OUTM(HWIO_DEVTEN_ULSTCNGEN_BMSK, ((enable ? 0x1 : 0x0) << HWIO_DEVTEN_ULSTCNGEN_SHFT));
}

/**
* Function name:  snps_dcd_function_wakeup_request
* Description: Prepare controller to send function wakeup for a particular interface
* Parameters:
*     @sc: (IN) The DCD SW context
*     @cmd: (IN) The command
*     @parameter: (IN) The command parameter
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
void snps_dcd_function_wakeup_request(jdevice_t dev, uint8 iface_num)
{
  snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
  core_t *core = (core_t *)sc->core_ctx;

  if (SPEED_SUPER != core->curr_speed)
  {
    return;
  }

  if (iface_num >= core->configs[core->curr_config-1].interface_count) //???
  {
    HSU_ERR_FATAL("Interface num out of bounds %d", iface_num, 0, 0);
  }
  
  //HSU_ULOG_1(NO_MSG, BUS_LOG, "entry, requesteed %u", (uint8) core->configs[core->curr_config-1].interfaces[iface_num].function_remote_wake_requested);
  if (FALSE == core->configs[core->curr_config-1].interfaces[iface_num].function_remote_wake_requested)
  {
    // Attempt to send function remote wake immediately
    HSU_ULOG_1(NO_MSG, BUS_LOG, "iface_num %u", iface_num);
    hsu_event_log(EVT_CLIENT_REQUESTED_FUNCTION_WAKE, core->core_number);
    core->configs[core->curr_config-1].interfaces[iface_num].function_remote_wake_requested = TRUE;
    core->configs[core->curr_config-1].interfaces[iface_num].function_remote_wake_tnotifification_pending = TRUE;
    // immediately attempt to send tNotification if possible by triggering CB, if remote wakeup is not happend
    if (!core->remote_wakeup_scheduled)
    {
      snps_dcd_function_wakeup_cb(sc);
    }
  }

  return;
}

void snps_dcd_ops_accept_u1_u2(boolean set_value)
{
  juint32_t DCTL;
  /* Set accept U1 and U2 */
  set_value = set_value ? 1 : 0;
  DCTL = HWIO_DCTL_IN;
  DCTL = HSU_VAR_OUTM(DCTL, set_value, HWIO_DCTL_ACCEPTU1ENA_BMSK, HWIO_DCTL_ACCEPTU1ENA_SHFT);
  DCTL = HSU_VAR_OUTM(DCTL, set_value, HWIO_DCTL_ACCEPTU2ENA_BMSK, HWIO_DCTL_ACCEPTU2ENA_SHFT);
  HWIO_DCTL_OUT(DCTL);
}

void snps_dcd_function_wakeup_cb(void* param)
{
  juint8_t i;
  juint32_t DGCMDPAR = 0;
  juint32_t DGCMD =0;
  snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)param;
  core_t *core = (core_t *)sc->core_ctx;

  // For windows 7, one function allow to wakeup, every function allow to wakeup
  boolean wakeup_enabled = core_is_func_remote_wake_enabled(core);
  HSU_ULOG_FUNC(BUS_LOG);

  if (wakeup_enabled)
  {
    // If a remote wake event occurs in multiple functions, each function shall send a Function Wake Notification.
    // If the function has not been accessed for longer than tNotification
    //  (refer to Section 8.13) since sending the last Function Wake Notification, 
    //  the function shall send the Function WakeNotification again until it has been accessed.
    if (core && (SPEED_SUPER == core->curr_speed))
    {
      if (CORE_ENABLED == core->core_state)
      {
        // Spec defines a requirement that device must be in U0
        uint32 link_state = HWIO_DSTS_INM(HWIO_DSTS_USBLNKST_BMSK) >> HWIO_DSTS_USBLNKST_SHFT;
        HSU_ULOG(NO_MSG, BUS_LOG, "enter_U0_ifcase");
        if (SNPS_DCD_LINK_STATE_U0 == link_state)
        {
          // Scan for the various function that has yet to be accessed by the host
          for (i = 0; i < core->configs[core->curr_config-1].interface_count; i++)
          {
            //HSU_ULOG_6(NO_MSG, BUS_LOG, "state_U0, iface %u, rw_enable %u, rw_req %u, sus_state %u, rw_tnotif %u, tnotif_sch %u.",
            //   i, 
            //  (uint8) core->configs[core->curr_config-1].interfaces[i].function_remote_wake_enable,
            //  (uint8) core->configs[core->curr_config-1].interfaces[i].function_remote_wake_requested,
            //  (uint8) core->configs[core->curr_config-1].interfaces[i].is_function_suspend,
            //  (uint8) core->configs[core->curr_config-1].interfaces[i].function_remote_wake_tnotifification_pending,
            //  (uint8) core->func_wake_tnotif_scheduled);

            if (core->configs[core->curr_config-1].interfaces[i].is_function_suspend
                && core->configs[core->curr_config-1].interfaces[i].function_remote_wake_requested
                && core->configs[core->curr_config-1].interfaces[i].function_remote_wake_tnotifification_pending
               )
            {
              // Do not clear "function_remote_wake_requested" flag here
              //   it will be cleared when HOST sends cmd accesing the function.
              //   Continue to send HOST tNotification 
              //   until host access the function (by any usb_cmd)
              hsu_event_log(EVT_SEND_FUNCTION_WAKE , core->core_number);
              
              DGCMDPAR = (1 << SNPS_DCD_DGCMD_FUNCTION_WAKE_SHFT) | (i << SNPS_DCD_DGCMD_FUNCTION_WAKE_IFACE_SHFT);  //iface field.
              DGCMD = (0x1 << HWIO_DGCMD_CMDACT_SHFT) | //Command Active
                    (SNPS_DCD_DGCMD_CMDTYPE_TRANSMIT_DEVICE_NOTIFICATION << HWIO_DGCMD_CMDTYP_SHFT);
              
              HSU_ULOG_3(NO_MSG, BUS_LOG, "state_U0, send_func_wake, iface %u, DGCMDPAR 0x%08X, DGCMD 0x%08X", i, DGCMDPAR, DGCMD );
              snps_dcd_write_general_command(sc, DGCMD, DGCMDPAR);

              // Schedule another snps_dcd_function_wakeup_cb in case host does not access the function within HSU_FUNCTION_WAKE_TNOTIFICATION_MS
              if (core->func_wake_tnotif_task != NULL)
              {
                if (FALSE == jtask_pending(core->func_wake_tnotif_task))
                {
                  HSU_ULOG(NO_MSG, BUS_LOG, "after_wake_the_host, rescedule_notification");
                  jtask_schedule(core->func_wake_tnotif_task, HSU_FUNCTION_WAKE_TNOTIFICATION_MS, 
                                 snps_dcd_function_wakeup_cb, sc);
                  core->func_wake_tnotif_scheduled = TRUE;
                }
              }
              else
              {
                 HSU_ULOG(NO_MSG, DBG_E_LOG, "after_wake_the_host, no_tnofication_task");
              }
            }
            // Else no special handling
          }
        }
        else
        {
          //jslave_wakeup_host(core->core_number);        
          HSU_ULOG(NO_MSG, DBG_E_LOG, "link_state not_U0_U3, reschedule_function_wakeup_cb");
          if (core->func_wake_tnotif_task != NULL)
          {
            if (FALSE == jtask_pending(core->func_wake_tnotif_task))
            {
              jtask_schedule(core->func_wake_tnotif_task, HSU_FUNCTION_WAKE_TNOTIFICATION_MS, 
                  snps_dcd_function_wakeup_cb, sc);
              core->func_wake_tnotif_scheduled = TRUE;
            }
          }
        }
      }
      else if (CORE_SUSPENDED == core->core_state)
      {
        HSU_ULOG(NO_MSG, BUS_LOG, "core_state_suspended (U3), wake_the_host");

        jslave_wakeup_host(core->core_number);
      }
    }
  }
  return;
}

/**
* Function name:  snps_dcd_ops_get_iface_index
* Description: Return interface index
* Parameters: 
*     @pipe: (IN) pointer to the pipe structure
*     @address_ptr: (OUT) interface index
*
* Return value: 0 on success, otherwise an error code 
* Scope: local
**/
static void snps_dcd_ops_get_iface_index(pipe_desc_t *pipe, uint8* iface_ptr)
{
  snps_dcd_ep_desc_t *ep = (snps_dcd_ep_desc_t*)pipe->dcd_handle;
  HSU_ASSERT(iface_ptr);
  //HSU_ULOG_6(NO_MSG, BUS_LOG, "pipe 0x%X, ep %u, ep_hw %u, ep_sw %u, config %u, iface %u", (uint32) pipe, (uint32) ep, (uint8) ep->hw_addr, (uint8) ep->soft_addr, (uint8) ep->claimed_config, (uint8) ep->claimed_iface);
  *iface_ptr = ep->claimed_iface-1;  //to get the other convernsion.
  return;
}

#if 0 
/*============================ Debug Utilities ============================*/

static void snps_dcd_run_scriptrunner_test(juint16_t index)
{
  /* Clear a last test result */
  SNPS_DCD_WRITE_REG(snps_dcd_sc, 0xCB04, 0);
  /* Run USBTestScript%index%.txt test script */
  SNPS_DCD_WRITE_REG(snps_dcd_sc, 0xCB00, 0x80000000 | index | 4 << 16);
}

static boolean snps_dcd_scriptrunner_test_result(void)
{
  /* Get a last test result */
  return (0 == (SNPS_DCD_READ_REG(snps_dcd_sc, 0xCB04) & 0x7FFFFFFF));
}

void snps_dcd_debug_timer_cb(unsigned long callback_param)
{
  /* Enumeration & Feature selectors */
  snps_dcd_run_scriptrunner_test(1);

  HSU_ASSERT(snps_dcd_scriptrunner_test_result() == TRUE);
}
#endif
DRIVER_CONTROLLER_DECLARE(snps_usb3_dcd, sizeof(snps_dcd_soft_dc_t))

