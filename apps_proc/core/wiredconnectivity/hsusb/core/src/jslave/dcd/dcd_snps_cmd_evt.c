/*
===========================================================================

FILE:         dcd_snps_cmd_evt.c

DESCRIPTION: This file handles command and event utilities for the
             Device Controller Driver for SNPS USB3.0 core. 

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/src/jslave/dcd/dcd_snps_cmd_evt.c#1 $

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
#include "hsu_os_abstract_bus.h"
#include "hsu_platform.h"
#include "hsu_lpm_snps.h"

/*==============================================================================
Internal Function Definitions
==============================================================================*/

/*=============================== Events Utils ===============================*/

/**
* Function name:  snps_dcd_set_event_buffer_address
* Description: Sets a particular event buffer physical memory address
* Parameters:
*     @sc: (IN) The DCD SW context
*     @event_buffer_index: (IN) The event buffer index
*     @address: (IN) Physical memory address for the event buffer
*
* Return value: None
* Scope: global
**/
void snps_dcd_set_event_buffer_address(snps_dcd_soft_dc_t *sc, 
                                       juint8_t event_buffer_index, 
                                       void* address)
{
  HWIO_GEVNTADRLO_REGS_p_GEVNTADRLO_OUTI(event_buffer_index,(juint32_t)address);
  HWIO_GEVNTADRLO_REGS_p_GEVNTADRHI_OUTI(event_buffer_index,0x0);
}

/**
* Function name:  snps_dcd_set_event_buffer_size
* Description: Sets a particular event buffer size
* Parameters:
*     @sc: (IN) The DCD SW context
*     @event_buffer_index: (IN) The event buffer index
*     @size: (IN) Size of the event buffer
*
* Return value: None
* Scope: global
**/
void snps_dcd_set_event_buffer_size(snps_dcd_soft_dc_t *sc, 
                                    juint8_t event_buffer_index, 
                                    juint16_t size)
{
  HWIO_GEVNTADRLO_REGS_p_GEVNTSIZ_OUTMI(event_buffer_index, HWIO_GEVNTADRLO_REGS_p_GEVNTSIZ_EVENTSIZ_BMSK,
    (size << HWIO_GEVNTADRLO_REGS_p_GEVNTSIZ_EVENTSIZ_SHFT));
}

/**
* Function name:  snps_dcd_set_event_buffer_interrupt
* Description: Enables/Disables a particular event buffer
*              for generating interrupts
* Parameters:
*     @sc: (IN) The DCD SW context
*     @event_buffer_index: (IN) The event buffer index
*     @enable: (IN) TRUE - enable, FALSE - disable
*
* Return value: None
* Scope: global
**/
void snps_dcd_set_event_buffer_interrupt(snps_dcd_soft_dc_t *sc, 
                                         juint8_t event_buffer_index, 
                                         boolean enable)
{
  HWIO_GEVNTADRLO_REGS_p_GEVNTSIZ_OUTMI(event_buffer_index, HWIO_GEVNTADRLO_REGS_p_GEVNTSIZ_EVNTINTRPTMASK_BMSK,
    ((!enable ? 0x1 : 0x0) << HWIO_GEVNTADRLO_REGS_p_GEVNTSIZ_EVNTINTRPTMASK_SHFT));
}

/**
* Function name:  snps_dcd_set_event_buffer_count
* Description: Dequeues an event from a particular event buffer
* Parameters:
*     @sc: (IN) The DCD SW context
*     @event_buffer_index: (IN) The event buffer index
*     @count: (IN) Number of bytes to dequeue
*
* Return value: None
* Scope: global
**/
void snps_dcd_set_event_buffer_count(snps_dcd_soft_dc_t *sc, 
                                     juint8_t event_buffer_index, 
                                     juint16_t count)
{
  HWIO_GEVNTADRLO_REGS_p_GEVNTCOUNT_OUTI(event_buffer_index, count);
}

/**
* Function name:  snps_dcd_get_event_buffer_count
* Description: Returns number of bytes enqueued by the controller to a 
*              particular event buffer
* Parameters:
*     @sc: (IN) The DCD SW context
*     @event_buffer_index: (IN) The event buffer index
*
* Return value: Number of bytes
* Scope: global
**/
juint16_t snps_dcd_get_event_buffer_count(snps_dcd_soft_dc_t *sc, 
                                          juint8_t event_buffer_index)
{
  return (juint16_t)HWIO_GEVNTADRLO_REGS_p_GEVNTCOUNT_INI(event_buffer_index);
}

//
//
//
// USB IPA GSI variation of event buffer creation
//
//
//
/**
* Function name:  snps_dcd_setup_event_buf_and_gsi_interrupters
* Description: 
* Parameters:
*
* Return value: None
* Scope: global
**/

void snps_dcd_setup_event_buf_and_gsi_interrupters(jdevice_t dev, uint32 idx, uint32 num_of_trbs)
{  
  // Only run if there was a restart core
  juint32_t reg = 0;
  juint32_t count;
  snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
  HSU_ULOG_FUNC(BAM_LOG);

  // idx is GSI interrupt index

  HSU_ULOG_1(NO_MSG, BAM_LOG, "snps_dcd_setup_event_buf_and_gsi_interrupters, idx %d", (uint32)idx);

  /* Event buffer for device-specific interrupts */
  // TODO AC check if this is required for alternate interrupts
  //  HWIO_DCFG_OUTM(HWIO_DCFG_INTRNUM_BMSK, (event_buffer_index << HWIO_DCFG_INTRNUM_SHFT));

  // Use same index as GSI interrupter for event buffer indexing
  // Choose an arbitrary large factor of 16 for size of TRBs
  snps_dcd_init_event_buffer(sc, idx, num_of_trbs * 16);

  snps_dcd_set_event_buffer_address_gsi(sc, idx, sc->event_buffer[idx].paddr);
  // bits: [(55-32):(54-32)] means bits  [23:22], set value to 0x3
  reg |= (0x3 << 22);
  // bits: [(50-32):(48-32)] means bits  [18:16], set value to match 0xn where n = [1..3]
  reg |= (idx << 16);
  HWIO_GEVNTADRLO_REGS_p_GEVNTADRHI_OUTI(idx, reg);

  HWIO_GEVNTADRLO_REGS_p_GEVNTSIZ_OUTMI(idx, HWIO_GEVNTADRLO_REGS_p_GEVNTSIZ_EVNTINTRPTMASK_BMSK, 0x1UL << HWIO_GEVNTADRLO_REGS_p_GEVNTSIZ_EVNTINTRPTMASK_SHFT);

  snps_dcd_set_event_buffer_size(sc, idx, (juint16_t)(sc->event_buffer[idx].buffer_size));

  /* Read all events to reset the Event Buffer content */
  count = snps_dcd_get_event_buffer_count(sc, idx);
  snps_dcd_set_event_buffer_count(sc, idx, count);

  /* Disable the interrupt for the Event buffer */
  snps_dcd_set_event_buffer_interrupt(sc, idx, FALSE);
}



void snps_dcd_reset_event_buffer(jdevice_t dev, uint32 idx)
{
  juint32_t reg = 0;
  juint32_t count = 0;
  snps_dcd_soft_dc_t *sc = (snps_dcd_soft_dc_t *)j_device_get_softc(dev);
  HSU_ULOG_FUNC(BAM_LOG);

  snps_dcd_set_event_buffer_address_gsi(sc, idx, sc->event_buffer[idx].paddr);
  // bits: [(55-32):(54-32)] means bits  [23:22], set value to 0x3
  reg |= (0x3 << 22);
  // bits: [(50-32):(48-32)] means bits  [18:16], set value to match 0xn where n = [1..3]
  reg |= (idx << 16);
  HWIO_GEVNTADRLO_REGS_p_GEVNTADRHI_OUTI(idx, reg);

  HWIO_GEVNTADRLO_REGS_p_GEVNTSIZ_OUTMI(idx, HWIO_GEVNTADRLO_REGS_p_GEVNTSIZ_EVNTINTRPTMASK_BMSK, 0x1UL << HWIO_GEVNTADRLO_REGS_p_GEVNTSIZ_EVNTINTRPTMASK_SHFT);

  snps_dcd_set_event_buffer_size(sc, idx, (juint16_t)(sc->event_buffer[idx].buffer_size));

  /* Read all events to reset the Event Buffer content */
  count = snps_dcd_get_event_buffer_count(sc, idx);
  snps_dcd_set_event_buffer_count(sc, idx, count);

  /* Disable the interrupt for the Event buffer */
  snps_dcd_set_event_buffer_interrupt(sc, idx, FALSE);
}


/**
* Function name:  snps_dcd_set_event_buffer_address_gsi
* Description: Sets a particular event buffer physical memory address
* Parameters:
*     @sc: (IN) The DCD SW context
*     @event_buffer_index: (IN) The event buffer index
*     @address: (IN) Physical memory address for the event buffer
*
* Return value: None
* Scope: global
**/
void snps_dcd_set_event_buffer_address_gsi(void *ctx, 
                                       juint8_t event_buffer_index, 
                                       void* address)
{
  // HWIO generation does not cover special bits designated for GSI; thus, we have to create own mask
  HWIO_GEVNTADRLO_REGS_p_GEVNTADRLO_OUTI(event_buffer_index, (juint32_t)address);
  // Do not write to GEVNTADRHI as it is designated for GSI
}



/*============================== Commands Utils ==============================*/

/**
* Function name:  snps_dcd_write_ep_command
* Description: Writes an EP command to the controller
* Parameters:
*     @sc: (IN) The DCD SW context
*     @ep_index: (IN) The EP HW address
*     @cmd: (IN) The command
*     @p0: (IN) The first command parameter
*     @p1: (IN) The second command parameter
*     @p2: (IN) The third command parameter
*     @cmd_params: (OUT) Command result returned from the controller
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_write_ep_command(snps_dcd_soft_dc_t *sc, juint8_t ep_index,
                                    juint32_t cmd, juint32_t p0, juint32_t p1, 
                                    juint32_t p2, juint16_t* cmd_params)
{
  juint32_t time_begin, time_elapsed = 0;
  juint32_t DEPCMD_CmdAct;
  juint32_t DEPCMD_CmdParam;
  juint32_t DEPCMD;
  boolean hs_phy_suspend_enabled = 0;
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  hsu_os_bus_memory_barrier();

  if (SPEED_SUPER != hsu_lpm_snps_get_curr_speed_from_hw())
  {
    if (HSU_PLATFORM_RUMI != platform_info->hsu_platform_id)
    {
      /* Commands to controller will work only if PHY is not suspended */
      hs_phy_suspend_enabled = HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_INMI(0x0, HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_BMSK);

      if (hs_phy_suspend_enabled)
      {
        HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_OUTMI(0x0, HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_BMSK, 0 << HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_SHFT);
      }
    }
  }

  HWIO_DEPCMDPAR2_REGS_p_DEPCMDPAR2_OUTI(ep_index, p2);
  HWIO_DEPCMDPAR2_REGS_p_DEPCMDPAR1_OUTI(ep_index, p1);
  HWIO_DEPCMDPAR2_REGS_p_DEPCMDPAR0_OUTI(ep_index, p0);
  HWIO_DEPCMDPAR2_REGS_p_DEPCMD_OUTI(ep_index, cmd);

  // Poll DEPCMD CmdAct bit for command completion
  for (time_begin = timetick_get_ms();
       time_elapsed < SNPS_DCD_MAX_CMD_DELAY_MS;
       time_elapsed = timetick_get_ms() - time_begin) // Unsigned subtraction takes care of timer rollover case
  {
    DEPCMD  = HWIO_DEPCMDPAR2_REGS_p_DEPCMD_INI(ep_index);
    DEPCMD_CmdAct = HSU_VAR_INM(DEPCMD, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_CMDACT_SHFT);

    if (!DEPCMD_CmdAct)
    {
      break;
    }
  }

  /* Return command result */
  if (cmd_params != NULL)
  { 
    DEPCMD_CmdParam = HSU_VAR_INM(DEPCMD, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_BMSK, HWIO_DEPCMDPAR2_REGS_p_DEPCMD_COMMANDPARAM_SHFT);
    *cmd_params= (juint16_t) DEPCMD_CmdParam;
  }

  if (SPEED_SUPER != sc->speed)
  {
    if (HSU_PLATFORM_RUMI != platform_info->hsu_platform_id)
    {
      /* Enable suspend of the USB2 PHY */
      if (hs_phy_suspend_enabled)
      {
        HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_OUTMI(0x0, HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_BMSK, 1 << HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_SHFT);
      }
    }
  }

  /* Command didn't complete successfully */
  if (time_elapsed >= SNPS_DCD_MAX_CMD_DELAY_MS)
  {
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, cmd_time_out, EP %d", ep_index);
#ifdef FEATURE_HS_USB_SNPS_CORE
    hsu_event_log(EVT_EP_COMMAND_FAIL, USB3_CORE_INDEX);

  #ifdef FEATURE_HS_USB_PRE_SILICON_BU
    HSU_ERR_FATAL("cmd_time_out, EP %d", ep_index, 0, 0);
  #endif
#endif
    return JETIMEDOUT;
  }

  return HSU_JSUCCESS;
}

/**
* Function name:  snps_dcd_write_general_command
* Description: Writes a general command to the controller
* Parameters:
*     @sc: (IN) The DCD SW context
*     @cmd: (IN) The command
*     @parameter: (IN) The command parameter
*
* Return value: 0 on success, otherwise an error code
* Scope: global
**/
jresult_t snps_dcd_write_general_command(snps_dcd_soft_dc_t *sc,
                                         juint32_t cmd, juint32_t parameter)
{
  juint32_t time_begin, time_elapsed = 0;
  juint32_t DGCMD_CmdAct;
  boolean hs_phy_suspend_enabled = 0;
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  if (SPEED_SUPER != hsu_lpm_snps_get_curr_speed_from_hw())
  {
    if (HSU_PLATFORM_RUMI != platform_info->hsu_platform_id)
    {
      /* Commands to controller will work only if PHY is not suspended */
      hs_phy_suspend_enabled = HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_INMI(0x0, HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_BMSK);

      if (hs_phy_suspend_enabled)
      {
        HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_OUTMI(0x0, HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_BMSK, 0 << HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_SHFT);
      }
    }
  }

  HWIO_DGCMDPAR_OUT(parameter);
  HWIO_DGCMD_OUT(cmd);

  /* Poll DGCMD CmdAct bit for command completion */
  for (time_begin = timetick_get_ms();
       time_elapsed < SNPS_DCD_MAX_CMD_DELAY_MS;
       time_elapsed = timetick_get_ms() - time_begin) // Unsigned subtraction takes care of timer rollover case
  {

    DGCMD_CmdAct = HWIO_DGCMD_INM(HWIO_DGCMD_CMDACT_BMSK) >> HWIO_DGCMD_CMDACT_SHFT;
    if (!DGCMD_CmdAct)
    {
      break;
    }
  }

  if (SPEED_SUPER != sc->speed)
  {
    if (HSU_PLATFORM_RUMI != platform_info->hsu_platform_id)
    {
      /* Enable suspend of the USB2 PHY */
      if (hs_phy_suspend_enabled)
      {
        HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_OUTMI(0x0, HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_BMSK, 1 << HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_SHFT);
      }
    }
  }

  /* Command didn't complete successfully */
  if (time_elapsed >= SNPS_DCD_MAX_CMD_DELAY_MS)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCD, cmd_time_out");
#ifdef FEATURE_HS_USB_SNPS_CORE
    hsu_event_log(EVT_GEN_COMMAND_FAIL, USB3_CORE_INDEX);
  #ifdef FEATURE_HS_USB_PRE_SILICON_BU
    HSU_ERR_FATAL("cmd_time_out, core %d", USB3_CORE_INDEX, 0, 0);
  #endif
#endif
    return JETIMEDOUT;
  }

  return HSU_JSUCCESS;
}
