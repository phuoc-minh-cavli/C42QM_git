/*
===========================================================================

FILE:         dcd_snps_config.c

DESCRIPTION: This file handles controller configuration utilities for the  
             Device Controller Driver for SNPS USB3.0 core. 

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/src/jslave/dcd/dcd_snps_config.c#1 $

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
#include "hsu_platform.h"


/*==============================================================================
Internal Function Definitions
==============================================================================*/

/*==================== General core configuration utils ======================*/

/**
* Function name:  snps_dcd_read_general_params
* Description: Read controller configuration parameters
* Parameters:
*     @sc: (IN) The DCD SW context
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_read_general_params(snps_dcd_soft_dc_t *sc)
{
  /* Read SNPS core version */
  sc->snpsid = (juint16_t)HWIO_GSNPSID_IN;

  /* Read EPs quantity */
  sc->max_available_eps = (juint8_t)(HWIO_GHWPARAMS3_INM(HWIO_GHWPARAMS3_GHWPARAMS3_17_12_BMSK)
    >>  HWIO_GHWPARAMS3_GHWPARAMS3_17_12_SHFT);
  
  sc->max_available_in_eps = (juint8_t)(HWIO_GHWPARAMS3_INM(HWIO_GHWPARAMS3_GHWPARAMS3_22_18_BMSK)
      >>  HWIO_GHWPARAMS3_GHWPARAMS3_22_18_SHFT);

  if ( (sc->max_available_eps <= 1)
    || (sc->max_available_in_eps == 0)
    || (sc->max_available_in_eps >= sc->max_available_eps))
  {
    HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "DCD, wrong_ep_quantity, av_ep %d, av_in_eps %d", sc->max_available_eps, sc->max_available_in_eps);
    return JEINVAL;
  }

  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_reset_core
* Description: Reset the controller to initial state
* Parameters:
*     @sc: (IN) The DCD SW context
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_reset_core(snps_dcd_soft_dc_t *sc)
{
  juint8_t retries;
  juint32_t DCTL_CSFTRST;
  jresult_t rc = HSU_JSUCCESS;

  /* Core Soft Reset */
  HWIO_DCTL_OUTM(HWIO_DCTL_CSFTRST_BMSK, (0x1 << HWIO_DCTL_CSFTRST_SHFT));

  /* Wait for core to come out of reset */
  for (retries = 0; retries < SNPS_DCD_SOFT_RESET_RETRIES; retries++)
  {
    DCTL_CSFTRST = (HWIO_DCTL_INM(HWIO_DCTL_CSFTRST_BMSK) 
      >> HWIO_DCTL_CSFTRST_SHFT);
    if (!DCTL_CSFTRST)
    {
      break;
    }
    jdelay_us(SNPS_DCD_SOFT_RESET_DELAY_US);
  }

  /* Core didn't come out of reset */
  if (retries == SNPS_DCD_SOFT_RESET_RETRIES)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, device_soft_reset_failed");
    rc = JETIMEDOUT;
  }

  return rc;
}

/**
* Function name:  snps_dcd_config_phy
* Description: Configuration for High-Speed and Super-Speed PHYs 
* Parameters:
*     @sc: (IN) The DCD SW context
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_config_phy(snps_dcd_soft_dc_t *sc)
{
 // snps_dcd_GUSB2PHYCFG GUSB2PHYCFG;

  /* Configuration for High-Speed PHY */
  /* Set width to 8-bit UTMI+ for SNPS PHY */
  HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_OUTMI(0x0, HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_USBTRDTIM_BMSK,
    (SNPS_DCD_GUSB2PHYCFG_USBTRDTIM_8BITS << HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_USBTRDTIM_SHFT));

  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_set_U1
* Description: Set link U1 enabled parameter 
* Parameters:
*     @sc: (IN) The DCD SW context
*     @flag: (IN) Set/Clear
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_set_U1(snps_dcd_soft_dc_t *sc, jbool_t flag)
{
  if (FALSE == hsu_conf_sel_nv_efs_get(HSU_EFS_COMPLIANCE_TEST_MODE_ENABLED))
  {
    HWIO_DCTL_OUTM(HWIO_DCTL_INITU1ENA_BMSK, ((flag ? 0x1 : 0x0) << HWIO_DCTL_INITU1ENA_SHFT));
  }
  
  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_set_U2
* Description: Set link U2 enabled parameter 
* Parameters:
*     @sc: (IN) The DCD SW context
*     @flag: (IN) Set/Clear
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_set_U2(snps_dcd_soft_dc_t *sc, jbool_t flag)
{
  if (FALSE == hsu_conf_sel_nv_efs_get(HSU_EFS_COMPLIANCE_TEST_MODE_ENABLED))
  {
    HWIO_DCTL_OUTM(HWIO_DCTL_INITU2ENA_BMSK, ((flag ? 0x1 : 0x0) << HWIO_DCTL_INITU2ENA_SHFT));
  }
  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_get_U1
* Description: Get link U1 enabled state
* Parameters:
*     @sc: (IN) The DCD SW context
*     @flag: (OUT) Set/Clear
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
static jresult_t snps_dcd_get_U1(snps_dcd_soft_dc_t *sc, jbool_t* flag)
{
  *flag =   (HWIO_DCTL_INM(HWIO_DCTL_INITU1ENA_BMSK)
    >> HWIO_DCTL_INITU1ENA_SHFT);

  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_get_U2
* Description: Get link U2 enabled state
* Parameters:
*     @sc: (IN) The DCD SW context
*     @flag: (OUT) Set/Clear
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
static jresult_t snps_dcd_get_U2(snps_dcd_soft_dc_t *sc, jbool_t* flag)
{
  *flag =  (HWIO_DCTL_INM(HWIO_DCTL_INITU2ENA_BMSK)
    >> HWIO_DCTL_INITU2ENA_SHFT);

  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_handle_set_SEL_request
* Description: Configurate the controller with set SEL request parameters
* Parameters:
*     @sc: (IN) The DCD SW context
*     @set_SEL_request: (IN) The set SEL request
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_handle_set_SEL_request(snps_dcd_soft_dc_t *sc, 
                                          struct usb_set_SEL_request *set_SEL_request)
{
  juint16_t system_exit_latency;
  jbool_t u1enabled, u2enabled;
  juint32_t DGCMD;
  snps_dcd_DGCMDPAR DGCMDPAR;
  jresult_t rc = HSU_JSUCCESS;

  /* Get U1 & U2 enabled state */
  rc = snps_dcd_get_U2(sc, &u2enabled);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, get_u2_failed");
    return rc;
  }

  rc = snps_dcd_get_U1(sc, &u1enabled);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, get_ul_failed");
    return rc;
  }

  /* Calculate system exit latency */ 
  if (u2enabled)
  {
    system_exit_latency = (set_SEL_request->U2PEL > SNPS_DCD_MAX_SYSTEM_EXIT_LATENCY_US) ? 
      0 : set_SEL_request->U2PEL;
  }
  else if (u1enabled)
  {
    system_exit_latency = (set_SEL_request->U1PEL > SNPS_DCD_MAX_SYSTEM_EXIT_LATENCY_US) ? 
      0 : set_SEL_request->U1PEL;
  }
  else
  {
    system_exit_latency = 0;
  }

  /* Issue SET_PERIODIC_PARAMS general command */
  DGCMD = 0;
  DGCMD = HSU_VAR_OUTM(DGCMD, 0x1, HWIO_DGCMD_CMDACT_BMSK, HWIO_DGCMD_CMDACT_SHFT);
  DGCMD = HSU_VAR_OUTM(DGCMD, 0x0, HWIO_DGCMD_CMDIOC_BMSK, HWIO_DGCMD_CMDIOC_SHFT);
  DGCMD = HSU_VAR_OUTM(DGCMD, SNPS_DCD_DGCMD_CMDTYPE_SET_PERIODIC_PARAMS, HWIO_DGCMD_CMDTYP_BMSK, HWIO_DGCMD_CMDTYP_SHFT);

  DGCMDPAR.value = 0;
  DGCMDPAR.DGCMD_SET_PERIODIC_PARAMS.SystemExitLatency = system_exit_latency;

  rc = snps_dcd_write_general_command(sc, DGCMD, DGCMDPAR.value);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, setting_system_exit_latency_failed");
    return rc;
  }

  return rc;
}

/**
* Function name:  snps_dcd_config_core
* Description: Initial configuration for the controller 
* Parameters:
*     @sc: (IN) The DCD SW context
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_config_core(snps_dcd_soft_dc_t *sc)
{
  juint32_t GCTL, DCFG;
  core_t *core = (core_t *)sc->core_ctx;
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();
  juint8_t speed;
  
  /*** Setup the device configuration ***/
  DCFG = HWIO_DCFG_IN;

  /* Set max speed to High-Speed / Super-Speed */
  speed = (core->max_speed == SPEED_SUPER) ? 
  SNPS_DCD_DCFG_DEVSPD_SS : SNPS_DCD_DCFG_DEVSPD_HS;
  DCFG = HSU_VAR_OUTM(DCFG, speed, HWIO_DCFG_DEVSPD_BMSK, HWIO_DCFG_DEVSPD_SHFT);
  
  /* Device address to default*/
  DCFG = HSU_VAR_OUTM(DCFG, 0x0, HWIO_DCFG_DEVADDR_BMSK, HWIO_DCFG_DEVADDR_SHFT);
  
  /* Enable LPM capability*/
  DCFG = HSU_VAR_OUTM(DCFG, 0x1, HWIO_DCFG_LPMCAP_BMSK, HWIO_DCFG_LPMCAP_SHFT);
  
  /* Set NumP to default */
  DCFG = HSU_VAR_OUTM(DCFG, SNPS_DCD_DCFG_DEFAULT_NUMP, HWIO_DCFG_NUMP_BMSK, HWIO_DCFG_NUMP_SHFT);

  HWIO_DCFG_OUT(DCFG);

  // Global Register Settings
  // AXI Master interface configuration
	// Set Burst Length
	HWIO_GSBUSCFG0_OUT(HWIO_GSBUSCFG0_INCR4BRSTENA_BMSK|HWIO_GSBUSCFG0_INCR8BRSTENA_BMSK|HWIO_GSBUSCFG0_INCR16BRSTENA_BMSK);
	// Use the default values for all other General Config Registers

  /*** Setup the core configuration ***/
  GCTL = HWIO_GCTL_IN;
  
  /* Disable clock gating */
  GCTL = HSU_VAR_OUTM(GCTL, 0x1, HWIO_GCTL_DSBLCLKGTNG_BMSK, HWIO_GCTL_DSBLCLKGTNG_SHFT);
  GCTL = HSU_VAR_OUTM(GCTL, 0x0, HWIO_GCTL_RAMCLKSEL_BMSK, HWIO_GCTL_RAMCLKSEL_SHFT);
  GCTL = HSU_VAR_OUTM(GCTL, 0x0, HWIO_GCTL_DEBUGATTACH_BMSK, HWIO_GCTL_DEBUGATTACH_SHFT);

  switch (platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_VIRTIO:
    case HSU_PLATFORM_RUMI:
    {
      GCTL = HSU_VAR_OUTM(GCTL, 0x1, HWIO_GCTL_U2RSTECN_BMSK, HWIO_GCTL_U2RSTECN_SHFT);
    }
    break;
    default:
    {
      if (core->max_speed == SPEED_SUPER)
      {
        GCTL = HSU_VAR_OUTM(GCTL, 0x1, HWIO_GCTL_U2RSTECN_BMSK, HWIO_GCTL_U2RSTECN_SHFT);
      }
    }
    break;
  }
  
  /* Enable Scrambling */
  GCTL = HSU_VAR_OUTM(GCTL, 0x0, HWIO_GCTL_DISSCRAMBLE_BMSK, HWIO_GCTL_DISSCRAMBLE_SHFT);

  /* Port Capability Direction for Device */
  GCTL = HSU_VAR_OUTM(GCTL, SNPS_DCD_GCTL_PRTCAPDIR_DEVICE, HWIO_GCTL_PRTCAPDIR_BMSK, HWIO_GCTL_PRTCAPDIR_SHFT);
  GCTL = HSU_VAR_OUTM(GCTL, 0x2, HWIO_GCTL_PWRDNSCALE_BMSK, HWIO_GCTL_PWRDNSCALE_SHFT);
  HWIO_GCTL_OUT(GCTL);

  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_config_events
* Description: Configure the controller to receive events in an event buffer 
* Parameters:
*     @sc: (IN) The DCD SW context
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_config_events(snps_dcd_soft_dc_t *sc, juint8_t event_buffer_index)
{
  juint16_t count = 0;

  /* Event buffer for device-specific interrupts */
  HWIO_DCFG_OUTM(HWIO_DCFG_INTRNUM_BMSK, (event_buffer_index << HWIO_DCFG_INTRNUM_SHFT));

  /* Setup the event buffers for the device and all endpoints. Use single 
  event buffer for all events */

  /* Configure the Event Buffer */
  snps_dcd_set_event_buffer_address(sc, event_buffer_index, sc->event_buffer[event_buffer_index].paddr);
  snps_dcd_set_event_buffer_size(sc, event_buffer_index, (juint16_t)(sc->event_buffer[event_buffer_index].buffer_size));

  snps_dcd_dbm_set_event_buffer_address_and_size(sc);

  /* Read all events to reset the Event Buffer content */
  count = snps_dcd_get_event_buffer_count(sc, event_buffer_index);
  snps_dcd_set_event_buffer_count(sc, event_buffer_index, count);

  /* Enable the interrupt for the Event buffer */
  snps_dcd_set_event_buffer_interrupt(sc, event_buffer_index, TRUE);

  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_config_device_resources
* Description: Program the controller with amount of transfer resources 
required by the DCD
* Parameters:
*     @sc: (IN) The DCD SW context
*     @res_num: (IN)  Number of transfer resources
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_config_device_resources(snps_dcd_soft_dc_t* sc, juint16_t res_num)
{
  jresult_t rc = HSU_JSUCCESS;
  juint32_t DEPCMD;

  /* Issue STARTNEWCFG command with required number of resources */
  DEPCMD = 0;
  DEPCMD = HSU_VAR_OUTM(DEPCMD, res_num, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_SHFT);  
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x1, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);
  DEPCMD = HSU_VAR_OUTM(DEPCMD, 0x0, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDIOC_SHFT);
  DEPCMD = HSU_VAR_OUTM(DEPCMD, SNPS_DCD_DEPCMD_CMDTYPE_STARTNEWCFG, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDTYP_SHFT);
  rc = snps_dcd_write_ep_command(sc, SNPS_DCD_EP0_OUT_ADDR, DEPCMD, 
    0, 0, 0, NULL);
  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, snps_dcd_write_ep_cmd_failed");
    return rc;
  }

  return rc;
}

/**
* Function name:  snps_dcd_init_event_buffer
* Description: Initialize the event buffer SW structure
* Parameters: 
*     @sc: (IN) snps_dcd software context
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_init_event_buffer(snps_dcd_soft_dc_t *sc, juint8_t event_buffer_index, juint32_t event_count)
{
  jresult_t rc;
  snps_dcd_event_buffer_t* event_buffer = &(sc->event_buffer[SNPS_DCD_DEFAULT_EVT_BUF_ID]);

  /* Rule of thumb for Max Event Buffer size */
  if (0 == event_count) // use default value
  {
    event_buffer[event_buffer_index].buffer_size = SNPS_DCD_EVNT_BUF_SIZE_FACTOR * 
      sc->max_available_eps * sizeof(snps_dcd_event_t);

    hsu_trace_msg1_set(SNPS_DCD_INIT_EVENT_BUFFER__TRC);

    /* Allocate memory for event buffer */
    rc = jdma_alloc(event_buffer[event_buffer_index].buffer_size, 1 << 8,
      &event_buffer[event_buffer_index].vaddr, &event_buffer[event_buffer_index].paddr, 0, 
      &event_buffer[event_buffer_index].cookie);

    hsu_trace_msg1_clear();
  }
  else
  {
    event_buffer[event_buffer_index].buffer_size = event_count * sizeof(snps_dcd_event_t);
    hsu_trace_msg1_set(SNPS_DCD_INIT_EVENT_BUFFER__TRC);

    /* Allocate memory for event buffer */
    rc = jdma_alloc(event_buffer[event_buffer_index].buffer_size, 1 << 8,
      &event_buffer[event_buffer_index].vaddr, &event_buffer[event_buffer_index].paddr, 0, 
      &event_buffer[event_buffer_index].cookie);

    hsu_trace_msg1_clear();
  }

  if (rc != HSU_JSUCCESS)
  {
    HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "DCD, failed_to_allocate_buffer, index %u, size %u",
      event_buffer_index, event_buffer[event_buffer_index].buffer_size);
    (void)snps_usb3_dcd_detach(sc->dev);
    return rc;
  }

  event_buffer[event_buffer_index].high_addr = (snps_dcd_event_t*)event_buffer[event_buffer_index].vaddr
    + event_buffer[event_buffer_index].buffer_size/sizeof(snps_dcd_event_t);
  event_buffer[event_buffer_index].sw_current_addr = (snps_dcd_event_t*)event_buffer[event_buffer_index].vaddr;
  event_buffer[event_buffer_index].hw_current_addr = event_buffer[event_buffer_index].sw_current_addr;

  return HSU_JSUCCESS;
}

