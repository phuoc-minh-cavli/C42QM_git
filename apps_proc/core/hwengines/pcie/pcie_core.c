/**
 * @file pcie_core.c
 * @brief
 * Implements interfaces to access the PCIe Core HW.
 */
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================
Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "pcie_core.h"
#include "pcie_hwio.h"
#include "pcie_res.h"
#include "pcie_dma.h"
#include "pcie_tgt.h"

#define PCIE_L1SS_EXIT_MAX_LATENCY_US 500              /** Maximum time for the link to exit from L1SS  */
#define PCIE_LTR_EXIT_L1SS_TIMEOUT_MS 1000             /** Maximum time for the link to exit from L1SS while sending LTR */
#define PCIE_SET_40_BIT               0x00000100       /** Upper 32 bits for setting 40th bit */

#define PCIE_PCS_TEST_BUS_INDEX_MAX             0x48   /** Maximum Index for PCS test bus */
#define PCIE_PCS_TESTBUS_INDEX_FOR_QSERDES_COM  0x2C   /** Value to be set in PCS test bus for selecting QSERDES COM */
#define PCIE_QSERDES_COM_TEST_BUS_INDEX_MAX     0x06   /** Maximum Index for QSERDES COM */
#define PCIE_PCS_TESTBUS_INDEX_FOR_QSERDES_LANE 0x30   /** Value to be set in PCS test bus for selecting QSERDES Lane */
#define PCIE_QSERDES_LANE_TEST_BUS_INDEX_MAX    0x03   /** Maximum Index for QSERDES Lane */
#define PCIE_PCS_MISC_DEBUG_BUS_INDEX_MAX       0x0C   /** Maximum Index for PCS Misc debug bus */
#define PCIE_PHY_DEBUG_BUS_SELECTOR             0x1D   /** Testbus selector value to select PHY debug bus */

/* Target configuration */
static pcie_tgtcfg_type *pcie_core_tgt_cfg;

/* ============================================================================
**  Function : pcie_core_req_enter_l1(void)
** ============================================================================
*/
/**
 * Request to enter L1 link state.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_core_req_enter_l1(void)
{
#ifndef PCIE_BOOT
   uint32 cnt=0;
   pcie_osal_intlock();
   if(pcie_dev->pcie_req_l1_cnt)
   {
      cnt = pcie_osal_atomic_dec(&pcie_dev->pcie_req_l1_cnt);
   }
   if(0 == cnt)
   {
      pcie_i_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_PM_CTRL, REQ_NOT_ENTR_L1), 0X0);
   }
   pcie_osal_intfree();
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_core_req_exit_l1(void)
** ============================================================================
*/
/**
 * Request to exit L1 link state.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_core_req_exit_l1(void)
{
#ifndef  PCIE_BOOT
   uint32 pm_stts_1,cnt;
   uint64 time_elapsed = 0;
   uint64 t1, t2;

   cnt = pcie_osal_atomic_inc(&pcie_dev->pcie_req_l1_cnt);
   if(1==cnt)
   {
      pcie_i_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_PM_CTRL, REQ_NOT_ENTR_L1), 0X1);
      pcie_osal_memorybarrier();

      /* Sanity checking */
      pm_stts_1 = pcie_i_read_reg(pcie_dev->parf_base, PCIE20_PARF_PM_STTS_1);

      /* Check were not out of substates */
      if(0 != (pm_stts_1 & (PCIE20_PARF_PM_STTS_1_MAC_PHY_RXELECIDLE_DIS_MASK)))
      {
         t1 = pcie_osal_get_timetick();
         while((0 != (pm_stts_1 & (PCIE20_PARF_PM_STTS_1_MAC_PHY_RXELECIDLE_DIS_MASK))) && (PCIE_L1SS_EXIT_MAX_LATENCY_US > time_elapsed))
         {
            pcie_osal_busywait_us(20);
            t2 = pcie_osal_get_timetick();

            time_elapsed = pcie_osal_timetick_diff_us(t2, t1);

            pm_stts_1 = pcie_i_read_reg(pcie_dev->parf_base, PCIE20_PARF_PM_STTS_1);
         }
         if(0 != (pm_stts_1 & (PCIE20_PARF_PM_STTS_1_MAC_PHY_RXELECIDLE_DIS_MASK)))
         {
            pcie_osal_debug(MSG_FATAL, "Link is in l1sub with time elapsed = %d", time_elapsed);
         }
      }
   }
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_core_initerrata
** ============================================================================
*/
/**
 * Additional PCIe Core HW configurations during initialization.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_core_initerrata(void)
{
   uint32 regval;
   
   /* Enable active configutation mode and Relocate the DBI base address*/
   pcie_i_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_SLV_ADDR_MSB_CTRL, SLV_ADDR_MSB_CTRL), 1);
   pcie_i_write_reg(pcie_dev->parf_base, PCIE20_PARF_SLV_ADDR_SPACE_SIZE_HI, (uint32)(pcie_dev->parf_slv_addr_space_size>>32));
   pcie_i_write_reg(pcie_dev->parf_base, PCIE20_PARF_SLV_ADDR_SPACE_SIZE, (uint32)(pcie_dev->parf_slv_addr_space_size&0xFFFFFFFF));
   pcie_i_write_reg(pcie_dev->parf_base, PCIE20_PARF_DBI_BASE_ADDR_HI, PCIE_SET_40_BIT);
   pcie_i_write_reg(pcie_dev->parf_base, PCIE20_PARF_DBI_BASE_ADDR, pcie_dev->dbi_base_phys);

   /* Enable “L1 indication” to IPA via pcie20_int_msi_dev0 output */
   regval = pcie_i_read_reg(pcie_dev->parf_base, PCIE20_PARF_SPARE_0_1_BITS);
   pcie_i_write_reg(pcie_dev->parf_base, PCIE20_PARF_SPARE_0_1_BITS, regval|0x100);

   /* Configure the PCIE20_PARF_SLAVE_AXI_ERROR_REPORT with the correct default value 32'h00016C85 for getting error responses properly. */
   pcie_i_write_reg(pcie_dev->parf_base, PCIE20_PARF_SLAVE_AXI_ERROR_REPORT, 0x00016C85);

   /* Enable ECAM Blocker */
   pcie_i_lock_regions(0, pcie_dev->dbi_reg_base_offs);

   /* Disable the debouncers (MST_WKP, SLV_WKP,DBI_WKP,RMVL_DBNCR,INSR_DBNCR) */
   pcie_i_write_reg(pcie_dev->parf_base, PCIE20_PARF_DB_CTRL, 0x73);

   /* Enable Auxiliary Power Detect */
   pcie_i_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_SYS_CTRL, AUX_PWR_DET), 1);

   /* Enable the bit to exit l1ss when sending LTR and MSI */
   pcie_i_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_CFG_BITS,REQ_EXIT_L1SS_MSI_LTR_EN),1);
   pcie_core_enable_mhi_lpm();

   /* Enable CS for RO(CS) register writes */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(MISC_CONTROL_1_REG, DBI_RO_WR_EN), 0x1);
   pcie_osal_memorybarrier();

   /* Set the INT_LINE Register field to 0 */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_MAX_LATENCY_MIN_GRANT_INTERRUPT_PIN_INTERRUPT_LINE_REG_1, INT_LINE), 0x0);

   /* Set the PMC Register - to support PME in D0, D3hot and D3cold */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_CAP_ID_NXT_PTR_REG_1, PME_SUPPORT), 0x19);

   /* Set the frequency for the AUX clock to 19.2MHz */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_AUX_CLK_FREQ_REG, AUX_CLK_FREQ), 0x14);

   /* Set the Endpoint L0s Acceptable Latency to 1us (max) */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_DEVICE_CAPABILITIES_REG_1, PCIE_CAP_EP_L0S_ACCPT_LATENCY), 0x7);

   /* Set the Endpoint L1 Acceptable Latency to 2 us (max) */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_DEVICE_CAPABILITIES_REG_1, PCIE_CAP_EP_L1_ACCPT_LATENCY), 0x7);

   /* Set the L0s Exit Latency to 2us-4us = 0x6 */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_LINK_CAPABILITIES_REG_1, PCIE_CAP_L1_EXIT_LATENCY), 0x6);

   /* Set the L1 Exit Latency to be 32us-64 us = 0x6 */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_LINK_CAPABILITIES_REG_1, PCIE_CAP_L0S_EXIT_LATENCY), 0x6);

   /* Enable Clock Power Management */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_LINK_CAPABILITIES_REG_1, PCIE_CAP_CLOCK_POWER_MAN), 0x1);

   /* Disable CS for RO(CS) register writes */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(MISC_CONTROL_1_REG, DBI_RO_WR_EN), 0x0);
   pcie_osal_memorybarrier();

   /* Enable writes for RO(CS2) */
   pcie_i_write_reg(pcie_dev->elbi_base, PCIE20_ELBI_CS2_ENABLE, 0x1);
   pcie_osal_memorybarrier();

   /* Set the Common Clock L0s Exit Latency to 2us-4us = 0x6 */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_LINK_CAPABILITIES_REG_1, PCIE_CAP_L1_EXIT_LATENCY), 0x6);

   /* Set the Common Clock L1 Exit Latency to be 32us-64 us = 0x6 */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_LINK_CAPABILITIES_REG_1, PCIE_CAP_L0S_EXIT_LATENCY), 0x6);

   /* Disable writes for RO(CS2) */
   pcie_i_write_reg(pcie_dev->elbi_base, PCIE20_ELBI_CS2_ENABLE, 0x0);
   pcie_osal_memorybarrier();

   /* T_Power_Off */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_L1_SUBSTATES_REG, L1SUB_T_POWER_OFF), 0x0);

   pcie_osal_memorybarrier();
}

/* ============================================================================
**  Function : pcie_core_config
** ============================================================================
*/
/**
 * Configure PCIe core.
 *
 * @param[in]      void
 *
 * @return         void
 *
 * @dependencies
 * None.
 */
void pcie_core_config(void)
{
   pcie_config_type pcie_config;
   pcie_return_status status;
   uint8 bar, atu_index;
   pcie_atuconfig_type atu_config = { 0 };

   pcie_osal_debug(MSG_HIGH, "Begin");

   pcie_core_tgt_cfg = pcie_tgt_get_tgt_config();

   /* Set the default parameters */
   pcie_config.dev_type  = PCIE_DEV_EP;
   pcie_config.tgt_speed = PCIE_CORE_TARGETSPEED;
   pcie_config.device_id = pcie_core_tgt_cfg->device_id;
   pcie_config.vendor_id = pcie_core_tgt_cfg->vendor_id;

   /* Check for PCIe Link up */
   if(pcie_i_read_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_PM_STTS, XMLH_LINK_UP)) == 0x1)
   {
      /* Settings Override */
      pcie_core_initerrata();
	  pcie_osal_debug(MSG_HIGH, "Link is up. Errata done. Exit");
	  return;
   }

   status = pcie_core_init(&pcie_config);
   if(status != PCIE_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "pcie_core_init failed");
   }

   atu_index = 0x0;
   for(bar = 0;bar < pcie_dev->max_bars;bar++)
   {
      pcie_osal_debug(MSG_HIGH, "Configuring bar %d", bar);
      pcie_core_setbarconfig(&pcie_core_tgt_cfg->bar_config[bar]);

      /* Program the inbound ATU if the BAR is enabled */
      if(0x0 != pcie_core_tgt_cfg->bar_config[bar].mask)
      {
         /* Configure the inbound iATU regions */
         atu_config.index           = atu_index;
         atu_config.dir             = PCIE_ATUDIR_INBOUND;
         atu_config.mode            = PCIE_ATUMODE_BARMATCHED;
         atu_config.bar             = bar;
         atu_config.target_addr_low = pcie_core_tgt_cfg->bar_config[bar].local_addr_low;
         atu_config.target_addr_hi  = pcie_core_tgt_cfg->bar_config[bar].local_addr_hi;
         pcie_core_setatuconfig(TRUE, &atu_config);
         atu_index++;
      }
   }
   /* The first BAR is to be routed to MHI */
   pcie_core_configmhigate(&pcie_core_tgt_cfg->bar_config[0]);

   pcie_osal_debug(MSG_HIGH, "Exit");
}

/* ============================================================================
**  Function : pcie_core_init
** ============================================================================
*/
/**
 * Initializes the PCIe Core and the PHY.
 *
 * @param[in,out]  pcie_cfg    Configuration to be programmed in the PCIe core
 *
 * @return         pcie_return_status
 *    Status of the operation
 */
pcie_return_status pcie_core_init(pcie_config_type *pcie_cfg)
{
   pcie_return_status status = PCIE_SUCCESS;

   pcie_osal_debug(MSG_HIGH, "Begin");
   if((NULL == pcie_dev) || (NULL == pcie_cfg))
   {
      status = PCIE_FAILED;
      return(status);
   }

   /* Configure the Device Type */
   pcie_core_setdevicetype(pcie_cfg->dev_type);

   /* Settings Override */
   pcie_core_initerrata();

   /* Configure the device id and vendor id */
   pcie_core_setcoreid(pcie_cfg->device_id, pcie_cfg->vendor_id);

   /* Configure the device speed */
   pcie_core_settargetspeed(pcie_cfg->tgt_speed);

   pcie_osal_debug(MSG_HIGH, "Exit");

   return(status);
}

/* ============================================================================
**  Function : pcie_core_getversion
** ============================================================================
*/
/**
 * Returns the revision of the core.
 *
 * @param[in]  void
 *
 * @return     uint32
 *    The revision of the core.
 */
uint32 pcie_core_getversion(void)
{
   if(NULL == pcie_dev)
   {
      return(0x0);
   }
   /* Read the revision register */
   return(pcie_i_read_reg(pcie_dev->elbi_base, PCIE20_ELBI_VERSION));
}

/* ============================================================================
**  Function : pcie_core_setcoreid
** ============================================================================
*/
/**
 * Sets the device id and vendor id for the device.
 *
 * @param[in]      device_id    Device ID
 * @param[in]      vendor_id    Vendor ID
 *
 * @return         void
 *
 * @dependencies
 * Link must not have been enumerated.
 */
void pcie_core_setcoreid(uint32 device_id, uint32 vendor_id)
{
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(MISC_CONTROL_1_REG, DBI_RO_WR_EN), 0x1);
   pcie_osal_memorybarrier();

   pcie_i_write_reg(pcie_dev->dbi_base, TYPE0_DEVICE_ID_VENDOR_ID_REG_1,
                    PCIE_FVAL(TYPE0_DEVICE_ID_VENDOR_ID_REG_1, PCI_TYPE0_DEVICE_ID, device_id) |
                       PCIE_FVAL(TYPE0_DEVICE_ID_VENDOR_ID_REG_1, PCI_TYPE0_VENDOR_ID, vendor_id)
                   );
   pcie_osal_memorybarrier();

   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(MISC_CONTROL_1_REG, DBI_RO_WR_EN), 0x0);
   pcie_osal_memorybarrier();
}

/* ============================================================================
**  Function : pcie_core_enableltssm
** ============================================================================
*/
/**
 * Enables the Link Training and Status State Machine. Once the state machine
 * completes successfully, the link to the root complex should be in L0. This
 * can be confirmed by polling pcie_core_getlinkstate
 *
 * @param[in]      void
 *
 * @return         void
 *
 * @sa
 * pcie_core_getlinkstate.
 */
void pcie_core_enableltssm(void)
{
   pcie_osal_debug(MSG_HIGH, "Begin");
   if(NULL == pcie_dev)
   {
      /* Error will manifest in link not being up */
      pcie_osal_debug(MSG_HIGH, "Error: pcie_dev is NULL");
      return;
   }
   /* Enable LTSSM */
   pcie_osal_debug(MSG_HIGH, "Enabling PCIe Core LTSSM");
   pcie_i_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_LTSSM, LTSSM_EN), 1);
   pcie_osal_debug(MSG_HIGH, "Exit");
}

/* ============================================================================
**  Function : pcie_core_get_link_status
** ============================================================================
*/
/**
 * Gets the status of the link.
 *
 * @param[in,out]  link_state    State of the link
 *
 * @return
 *    State of the link (up or down).
 */
pcie_linkstatus_type pcie_core_get_link_status(pcie_core_linkstate_type *link_state)
{
   uint32 link_status;

   if((NULL == pcie_dev) || (NULL == link_state))
   {
      return(PCIE_LINKSTATUS_DOWN);
   }

   *link_state = (pcie_core_linkstate_type)pcie_i_read_reg_field(pcie_dev->parf_base,
                                                                 PCIE_FLD(PCIE20_PARF_LTSSM, LTSSM_STATE));

   link_status = pcie_i_read_reg_field(pcie_dev->parf_base,
                                       PCIE_FLD(PCIE20_PARF_PM_STTS, XMLH_LINK_UP));

   pcie_osal_debug(MSG_HIGH, "LTSSM State=%d PARF PM STTS = %d",*link_state,link_status);

   return((link_status) ? PCIE_LINKSTATUS_UP : PCIE_LINKSTATUS_DOWN);
}

/* ============================================================================
**  Function : pcie_core_isenumerated
** ============================================================================
*/
/**
 * Detects if the link has been enumerated by the root complex.
 * After the link is up and trained, the root complex will have to assign
 * resources to this end point and "enable" bus master access to it. Once that
 * is done, the link is said to have been enumerated.
 *
 * @param[in]      void
 *
 * @return         boolean
 *    TRUE  - if link is enumerated
 *    FALSE - if link is NOT enumerated
 */
boolean pcie_core_isenumerated(void)
{
   if(NULL == pcie_dev)
   {
      return(FALSE);
   }

   return((pcie_i_read_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_STATUS_COMMAND_REG_1, PCI_TYPE0_BUS_MASTER_EN)) == 1) ? TRUE : FALSE);
}

/* ============================================================================
**  Function : pcie_core_is_in_l1ss
** ============================================================================
*/
/**
 * Detects if the PCIe core is in L1ss
 *
 * @param[in]      void
 *
 * @return         boolean
 *    TRUE  - if in L1ss
 *    FALSE - if NOT in L1ss
 */
boolean pcie_core_is_in_l1ss(void)
{
   uint32 pm_stts_1;
   uint32 pm_stts;

   pm_stts_1 = pcie_i_read_reg(pcie_dev->parf_base, PCIE20_PARF_PM_STTS_1);
   pm_stts = pcie_i_read_reg(pcie_dev->parf_base, PCIE20_PARF_PM_STTS);

   pcie_osal_debug(MSG_HIGH, "Checking if link is in l1ss with pm_stts = 0x%x pm_stts_1= 0x%x", pm_stts, pm_stts_1);

   if(pm_stts_1 & PCIE20_PARF_PM_STTS_1_MAC_PHY_RXELECIDLE_DIS_MASK)
   {
      return (TRUE);
   }
   else
   {
      return (FALSE);
   }
}

/* ============================================================================
**  Function : pcie_core_send_ltr
** ============================================================================
*/
/**
 * Reads if LTR is enabled and writes the LTR value
 *
 * @param[in]      ltr_us    LTR value in usecs (max value is ~ 30 seconds)
 *
 * @return         void
 */
void pcie_core_send_ltr(uint32 ltr_us)
{
#ifndef PCIE_BOOT
   uint32 ltr_value, ltr_msg_val;
   uint32 nosnoop_ltr, snoop_ltr;
   uint32 ltr_enabled;
   uint64 t1,t2,time_elapsed,time_start = 0;

   pcie_osal_debug(MSG_HIGH, "Begin");

   ltr_enabled =  pcie_i_read_reg_field(pcie_dev->dbi_base, PCIE_FLD(DEVICE_CONTROL2_DEVICE_STATUS2_REG, PCIE_CAP_LTR_EN));
   if(!ltr_enabled)
   {
      pcie_osal_debug(MSG_HIGH, "LTR bit not enabled");
      return;
   }

   if(ltr_us)
   {
      ltr_value = ((ltr_us * 1000) >> 15); /* Need the value to be a unit of 32768 ns */
      if((ltr_value & ~0x3FF) != 0)
      {
         pcie_osal_debug(MSG_HIGH, "LTR usec was too high to fit in a message : %d", ltr_us);
         return;
      }
      /* Req    | Scale (3) | Value  */
      nosnoop_ltr = snoop_ltr = ((1 << 15) | (3 << 10) | (ltr_value & 0x3FF));
      ltr_msg_val = ((nosnoop_ltr << 16) | snoop_ltr);
   }
   else
   {
      /* Disable LTR message */
      ltr_msg_val = 0x0;
   }

   pcie_i_write_reg(pcie_dev->parf_base, PCIE20_PARF_LTR_MSG_GEN, ltr_msg_val);

   /* Poll until PCIE20_PARF_LTR_MSI_EXIT_L1SS .LTR_MSG_EXIT_L1SS is cleared (‘0’).
   When this bit is cleared, the request was synchronized to the SNPS core (and caused the core to exit automatically from L1SS if needed). */
   time_start = pcie_osal_get_systime_ms();
   t1 = pcie_osal_get_timetick();
   while(pcie_i_read_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_LTR_MSI_EXIT_L1SS,LTR_MSG_EXIT_L1SS))
         && (PCIE_LTR_EXIT_L1SS_TIMEOUT_MS > (pcie_osal_get_systime_ms() - time_start)))
   {
      t2 = pcie_osal_get_timetick();
	  time_elapsed = pcie_osal_timetick_diff_us(t2, t1);
      if((1000<time_elapsed)&&pcie_i_read_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_LTR_MSI_EXIT_L1SS,LTR_MSG_EXIT_L1SS)))
      {
         pcie_osal_busywait_us(500);
         t1 = pcie_osal_get_timetick();
      }
   }
   if(pcie_i_read_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_LTR_MSI_EXIT_L1SS,LTR_MSG_EXIT_L1SS)))
   {
      pcie_osal_debug(MSG_FATAL, "Did not exit from l1ss within timeout");
   }
   pcie_osal_debug(MSG_HIGH, "LTR Msg = 0x%X ; LTR usec = %d usecs", ltr_msg_val, ltr_us);
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_core_setdevicetype
** ============================================================================
*/
/**
 * Sets the operational mode of the PCIe core. There are two valid modes of
 * operations: a. Root Complex mode b. End Point mode
 *
 * @param[in]      devtype     Device type to be configured to
 *
 * @return
 *    None.
 */
void pcie_core_setdevicetype(pcie_devmode_type devtype)
{
   if(NULL == pcie_dev)
   {
      return;
   }

   pcie_i_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_DEVICE_TYPE, DEVICE_TYPE), (devtype == PCIE_DEV_EP) ? 0 : 4);
}

/* ============================================================================
**  Function : pcie_core_getdevicetype
** ============================================================================
*/
/**
 * Returns the device type.
 *
 * @param[in]      void
 *
 * @return         pcie_devmode_type
 *    Type of the device (Root Complex or End Point).
 */
pcie_devmode_type pcie_core_getdevicetype(void)
{
   uint32 devtype = 0x0;
   if(NULL != pcie_dev)
   {
      devtype = pcie_i_read_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_DEVICE_TYPE, DEVICE_TYPE));
   }
   return((devtype == 0x0) ? PCIE_DEV_EP : PCIE_DEV_RC);
}

/* ============================================================================
**  Function : pcie_core_settargetspeed
** ============================================================================
*/
/**
 * Sets the maximum speed supported by the link. This value is used by the
 * endpoint during link training sequence with the root complex.
 *
 * @param[in]      targetspeed  Maximum speed supported as per the spec
 *
 * @return         void
 */
void pcie_core_settargetspeed(pcie_targetspeed_type targetspeed)
{
   if(NULL == pcie_dev)
   {
      return;
   }
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(LINK_CONTROL2_LINK_STATUS2_REG, PCIE_CAP_TARGET_LINK_SPEED),
                          targetspeed);
}

/* ============================================================================
**  Function : pcie_core_gettargetspeed
** ============================================================================
*/
/**
 * Gets the maximum speed supported by the link.
 *
 * @param[in]      void
 *
 * @return         pcie_targetspeed_type
 *    Maximum speed supported by the link.
 */
pcie_targetspeed_type pcie_core_gettargetspeed(void)
{
   if(NULL == pcie_dev)
   {
      return(PCIE_CORE_TARGETSPEED);
   }
   return((pcie_targetspeed_type)pcie_i_read_reg_field(pcie_dev->dbi_base,
                                                       PCIE_FLD(LINK_CONTROL2_LINK_STATUS2_REG, PCIE_CAP_TARGET_LINK_SPEED)));
}

/* ============================================================================
**  Function : pcie_core_setatuconfig
** ============================================================================
*/
/**
 * Configures the internal Address Translation Unit (iATU). This is required to
 * map the inbound traffic to internal memory and outbound traffic to host memory
 * space.
 *
 * @param[in]      enable       Enable the ATU by default
 * @param[in,out]  atu_config   Configuration for the ATU
 *
 * @return         void
 */
void pcie_core_setatuconfig(boolean enable, pcie_atuconfig_type *atu_config)
{
   uint32 ctrl2_value = 0;

   pcie_osal_debug(MSG_HIGH, "Begin");
   if(NULL == pcie_dev)
   {
      pcie_osal_debug(MSG_HIGH, "Error: pcie_dev is NULL");
      return;
   }

   /* 3.11 iATU Programming */
   if(PCIE_ATUDIR_INBOUND == atu_config->dir)
   {
      atu_config->index |= 0x80000000;
   }

   /* 1. Setup the Index Register */
   pcie_i_write_reg(pcie_dev->dbi_base, IATU_VIEWPORT_REG, atu_config->index);
   pcie_osal_memorybarrier();

   if(PCIE_ATUMODE_BARMATCHED == atu_config->mode)
   {
      pcie_osal_debug(MSG_HIGH, "PCIe in ATU Bar Matched Mode");

      /* 2. Setup the Target Address Registers */
      pcie_i_write_reg(pcie_dev->dbi_base, PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0, atu_config->target_addr_low);
      pcie_i_write_reg(pcie_dev->dbi_base, PL_IATU_UPPER_TARGET_ADDR_REG_OUTBOUND_0, atu_config->target_addr_hi);

      /* 3. Configure the region through the Region Control 1 Register. 0x0 -- Region is MEM */
      pcie_i_write_reg(pcie_dev->dbi_base, PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0, 0x0);

      /* 4. Enable the region for BAR Match Mode. */
      if(TRUE == enable)
      {
         ctrl2_value = PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_REGION_EN_MASK;
      }
      ctrl2_value |= (PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_MATCH_MODE_MASK |
                     atu_config->bar << PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_BAR_NUMBER_SHFT);
      pcie_i_write_reg(pcie_dev->dbi_base, PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0, ctrl2_value);
      pcie_osal_memorybarrier();
   }
   else
   {
      pcie_osal_debug(MSG_HIGH, "PCIe in ATU Address Matched Mode");

      /* 2. Setup the Region Base and Limit Address Registers. */
      pcie_i_write_reg(pcie_dev->dbi_base, PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0, atu_config->base_addr_low);
      pcie_i_write_reg(pcie_dev->dbi_base, PL_IATU_UPPER_BASE_ADDR_REG_OUTBOUND_0, atu_config->base_addr_hi);
      pcie_i_write_reg(pcie_dev->dbi_base, PL_IATU_LIMIT_ADDR_REG_OUTBOUND_0, (atu_config->base_addr_low + atu_config->size - 1));

      /* 3. Setup the Target Address Registers. */
      pcie_i_write_reg(pcie_dev->dbi_base, PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0, atu_config->target_addr_low);
      pcie_i_write_reg(pcie_dev->dbi_base, PL_IATU_UPPER_TARGET_ADDR_REG_OUTBOUND_0, atu_config->target_addr_hi);

      /* 4. Configure the region through the Region Control 1 Register. */
      pcie_i_write_reg(pcie_dev->dbi_base, PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0, 0x0);

      /* 5. Enable the region. */
      if(TRUE == enable)
      {
         ctrl2_value = PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_REGION_EN_MASK;
      }
      pcie_i_write_reg(pcie_dev->dbi_base, PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0, ctrl2_value);
      pcie_osal_memorybarrier();
   }
   pcie_osal_debug(MSG_HIGH, "Exit");
}

/* ============================================================================
**  Function : pcie_core_setbarconfig
** ============================================================================
*/
/**
 * Sets the BAR Config in the PCIe Config Space. This config determines how big the
 * window into the MDM is for the host.
 *
 * @param[in,out]  bar_config    Bar configuration
 *
 * @return         void
 */
void pcie_core_setbarconfig(pcie_barcfg_type *bar_config)
{
   uint32 bar_reg_offset, bar_type_val;

   pcie_osal_debug(MSG_HIGH, "Begin");
   if((NULL == pcie_dev) || (NULL == bar_config))
   {
      pcie_osal_debug(MSG_HIGH, "Error: pcie_dev or bar_config is NULL");
      return;
   }

   bar_reg_offset = TYPE0_BAR0_REG_1 + (0x4 * bar_config->bar);
   bar_type_val = (PCIE_FVAL(TYPE0_BAR0_REG_1, BAR0_PREFETCH, bar_config->mode) |
                  PCIE_FVAL(TYPE0_BAR0_REG_1, BAR0_TYPE, bar_config->type) |
                  PCIE_FVAL(TYPE0_BAR0_REG_1, BAR0_MEM_IO, bar_config->memspace));

   /* 5.1.8.11 Base Address Registers configuration */

   /* Enable Config access via CS2 */
   pcie_i_write_reg(pcie_dev->elbi_base, PCIE20_ELBI_CS2_ENABLE, 0x1);
   pcie_osal_memorybarrier();

   /* Configure the BAR Mask */
   pcie_i_write_reg(pcie_dev->dbi_base, bar_reg_offset, bar_config->mask);
   pcie_osal_memorybarrier();

   /* Disable config access via CS2 */
   pcie_i_write_reg(pcie_dev->elbi_base, PCIE20_ELBI_CS2_ENABLE, 0x0);
   pcie_osal_memorybarrier();

   /* Enable Config access via CS */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(MISC_CONTROL_1_REG, DBI_RO_WR_EN), 0x1);
   pcie_osal_memorybarrier();

   /* Configure the BAR */
   pcie_osal_debug(MSG_HIGH, "bar_reg_offset = 0x%x , bar_type_val = 0x%x", bar_reg_offset, bar_type_val);
   pcie_i_write_reg(pcie_dev->dbi_base, bar_reg_offset, bar_type_val);
   pcie_osal_memorybarrier();

   /* Disable config access via CS */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(MISC_CONTROL_1_REG, DBI_RO_WR_EN), 0x0);
   pcie_osal_memorybarrier();

   pcie_osal_debug(MSG_HIGH, "Exit");
}

/* ============================================================================
**  Function : pcie_core_getmsiconfig
** ============================================================================
*/
/**
 * Gets the MSI Config from the PCIe Config Space programmed by the host.
 *
 * @param[in,out]  msi_config    MSI Config data to be returned
 *
 * @return         void
 */
void pcie_core_getmsiconfig(pcie_msiconfig_type *msi_config)
{
   if((NULL == pcie_dev) || (NULL == msi_config))
   {
      return;
   }
   msi_config->enabled        = pcie_i_read_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1, PCI_MSI_ENABLE));
   msi_config->addr_hi        = pcie_i_read_reg(pcie_dev->dbi_base, MSI_UPPER_32);
   msi_config->addr_low       = pcie_i_read_reg(pcie_dev->dbi_base, MSI_LOWER_32);
   msi_config->data           = pcie_i_read_reg(pcie_dev->dbi_base, MSI_DATA_REG);
   msi_config->num_multi_msgs = pcie_i_read_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1, PCI_MSI_MULTIPLE_MSG_EN));
}

/* ============================================================================
**  Function : pcie_core_sendparfmsi
** ============================================================================
*/
/**
 * Sends an MSI via PARF mechanism. This approach is used for avoiding the need
 * to allocate a 4K entry in the ATU for the MSI address in the host to which
 * this vector is to be sent.
 *
 * @param[in]      msi_vector    MSI Vector to be triggered at the host
 *
 * @return         void
 *
 * @dependencies
 * MSI must have been enabled in the caps.
 */
void pcie_core_sendparfmsi(uint32 msi_vector)
{
   if(NULL == pcie_dev)
   {
      pcie_osal_debug(MSG_FATAL, "pcie_dev is NULL");
      return;
   }

   /* Use the PARF method to trigger the MSI */
   pcie_i_write_reg(pcie_dev->parf_base, PCIE20_PARF_MSI_GEN, msi_vector);
}

/* ============================================================================
**  Function : pcie_core_configmhigate
** ============================================================================
*/
/**
 * Configure the MHI Gate to allow host access to the MHI MMIO space. It is
 * required since the BHI MMIO is located within the MHI block.
 * Configure the MHI Gate to allow host access to the MHI MMIO space. It is
 * required since the BHI MMIO is located within the MHI block.
 *
 * @param[in,out]  bar_config  Configuration to program the MHI gate
 *
 * @return         void
 */
void pcie_core_configmhigate(pcie_barcfg_type *bar_config)
{

   uint32 mhi_ctrl_val = (PCIE20_PARF_MHI_CLOCK_RESET_CTRL_MSTR_AXI_CLK_EN_MASK |
                         PCIE20_PARF_MHI_CLOCK_RESET_CTRL_AHB_CLK_EN_MASK);

   pcie_osal_debug(MSG_HIGH, "Begin");

   if((NULL == pcie_dev) || (NULL == bar_config))
   {
      pcie_osal_debug(MSG_HIGH, "Error: pcie_dev or bar_config is NULL");
      return;
   }

   /* Enable MHI clocks */
   pcie_i_write_reg(pcie_dev->parf_base, PCIE20_PARF_MHI_CLOCK_RESET_CTRL,
                    mhi_ctrl_val);

   /* Enable Low power mode on MHI (shutdown in L1ss) */
   pcie_core_enable_mhi_lpm();

   /* Configure the MHI base address so that inbound transactions to the
     MHI are routed appropriately to the MHI registers within the PCIe core
    */
   pcie_i_write_reg(pcie_dev->parf_base, PCIE20_PARF_MHI_BASE_ADDR_LOWER,
                    bar_config->local_addr_low);
   pcie_i_write_reg(pcie_dev->parf_base, PCIE20_PARF_MHI_BASE_ADDR_UPPER,
                    bar_config->local_addr_hi);

   /* Clear the MHI State register as well  */
   pcie_i_write_reg(pcie_dev->mhi_base, PCIE20_MHISTATUS, 0x0);

#ifndef PCIE_BOOT
   /* Indicate that we are in AMSS */
   pcie_i_write_reg(pcie_dev->mhi_base, PCIE20_BHI_EXECENV, 0x2);
#endif /* PCIE_BOOT */

   pcie_osal_debug(MSG_HIGH, "Exit");
}

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_core_enter_lpm
** ============================================================================
*/
/**
 * Request PCIe core to enter low power mode
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_core_enter_lpm(void)
{
   pcie_res_clk_disable();
}

/* ============================================================================
**  Function : pcie_core_exit_lpm
** ============================================================================
*/
/**
 * Request PCIe core to exit low power mode
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_core_exit_lpm(void)
{
   pcie_res_clk_enable();
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_core_dump
** ============================================================================
*/
/**
 * Dumps the PCIe registers.
 *
 * @param[in,out]      pcie_coredump_type            Coredump structure.
 * @param[in]          pcie_core_dump_type           Partial/Full dump
 *
 * @return            void
 *
 * @note              Clocks must be turned on before collecting the registers.
 */
void pcie_core_dump(pcie_coredump_type *pcie_coredump, pcie_core_dump_type pcie_dump_type)
{
   uint32 i,j;
   if((NULL == pcie_dev) || (NULL == pcie_coredump))
   {
      return;
   }

   pcie_osal_debug(MSG_HIGH, "Dumping PARF and PHY registers");
   pcie_osal_memcpy_reg((uint32 * )pcie_coredump->parf_regs, (uint32 * )pcie_dev->parf_base, PCIE_PARF_SIZE);
   pcie_osal_memcpy_reg((uint32 * )pcie_coredump->phy_regs, (uint32 * )pcie_dev->phy_base, PCIE_CM_SIZE);
   pcie_osal_memcpy_reg((uint32 * )pcie_coredump->mhi_regs, (uint32 * )pcie_dev->mhi_base, PCIE_MHI_SIZE);

   /* Back up testbus */
   for(i = 0;i < sizeof(pcie_coredump->test_bus) / sizeof(pcie_coredump->test_bus[0]);i++)
   {
      pcie_i_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_SYS_CTRL, TESTBUS_SELECTOR), i);
      pcie_coredump->test_bus[i] = pcie_i_read_reg(pcie_dev->parf_base, PCIE20_PARF_TEST_BUS);
   }
   
   /* Back up PHY testbus */
   
   /* Select the PHY test bus using the testbus selector */
   pcie_i_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_SYS_CTRL, TESTBUS_SELECTOR), PCIE_PHY_DEBUG_BUS_SELECTOR);
   
   /* Read all PCS debug data */
   for(i = 1,j=0;i <= PCIE_PCS_TEST_BUS_INDEX_MAX;i+=4,j++)
   {
      pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL4, i);
      pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL5, i+1);
      pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL6, i+2);
      pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL7, i+3);
      pcie_coredump->phy_test_bus[j] = pcie_i_read_reg(pcie_dev->parf_base, PCIE20_PARF_TEST_BUS);
   }
 
   /* Read all QSERDES Common debug data */
   i = PCIE_PCS_TESTBUS_INDEX_FOR_QSERDES_COM;
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL4, i);
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL5, i+1);
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL6, i+2);
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL7, i+3);

   for(i = 1;i <= PCIE_QSERDES_COM_TEST_BUS_INDEX_MAX;i++,j++)
   {
      pcie_i_write_reg(pcie_dev->phy_base, QSERDES_COM_DEBUG_BUS_SEL, i);
      pcie_coredump->phy_test_bus[j] = pcie_i_read_reg(pcie_dev->parf_base, PCIE20_PARF_TEST_BUS);
   }

   /* Read all QSERDES Lane debug data */
   i = PCIE_PCS_TESTBUS_INDEX_FOR_QSERDES_LANE;
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL4, i);
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL5, i+1);
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL6, i+2);
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL7, i+3);

   for(i = 1;i <= PCIE_QSERDES_LANE_TEST_BUS_INDEX_MAX;i++,j++)
   {
      pcie_i_write_reg(pcie_dev->phy_base, QSERDES_TX_DEBUG_BUS_SEL, i);
      pcie_coredump->phy_test_bus[j] = pcie_i_read_reg(pcie_dev->parf_base, PCIE20_PARF_TEST_BUS);
   }

   /* Shut down PCS debug bus */
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL4, 0x0);
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL5, 0x0);
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL6, 0x0);
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_TEST_CONTROL7, 0x0);
   
   /* Read all PCS_MISC debug data */
   for(i = 1;i <= PCIE_PCS_MISC_DEBUG_BUS_INDEX_MAX;i+=4,j++)
   {
      pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE0_INDEX, i);
      pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE1_INDEX, i+1);
      pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE2_INDEX, i+2);
      pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE3_INDEX, i+3);
      pcie_coredump->phy_test_bus[j] = pcie_i_read_reg(pcie_dev->parf_base, PCIE20_PARF_TEST_BUS);
   }

   /* Shut down PCS_MISC debug bus */
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE0_INDEX, 0x0);
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE1_INDEX, 0x0);
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE2_INDEX, 0x0);
   pcie_i_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE3_INDEX, 0x0);
   
   pcie_bam_regs_dump();

   if(pcie_dump_type == PCIE_FULL_DUMP)
   {
      pcie_osal_debug(MSG_HIGH, "Dumping DBI and ELBI registers");
      pcie_osal_memcpy_reg((uint32 * )pcie_coredump->dbi_regs, (uint32 * )pcie_dev->dbi_base, PCIE_DBI_SIZE);
      pcie_osal_memcpy_reg((uint32 * )pcie_coredump->elbi_regs, (uint32 * )pcie_dev->elbi_base, PCIE_ELBI_SIZE);
   }
}

/* ============================================================================
**  Function : pcie_core_control_l1ss_inact_timeout
** ============================================================================
*/
/**
 * Controls the L1ss Inactivity timeout
 *
 * @param[in]       ctrl        True to enable interrut,False to disable interrupt.
 * @param[in]       maxtimer    Max time out value in us
 *
 * @return          uint32      Returns the AHB frequency if the timeout is enabled else return 0
 */
uint32 pcie_core_control_l1ss_inact_timeout(boolean ctrl,uint32 maxtimer)
{
   uint32 nfreq=0;

#ifndef PCIE_BOOT
   if (ctrl)
   {
#ifdef PCIE_L1SS_INACT_FREQ_CHECK
      pcie_res_get_clock_freq(PCIE_AUX_CLK, &nfreq);

      if(0 == nfreq)
      {
         pcie_osal_debug(MSG_FATAL,"Received AUX clk frequency as 0");
      }
#else
      nfreq = pcie_core_tgt_cfg->aux_clk_freq_hz;
#endif /* PCIE_L1SS_INACT_FREQ_CHECK */
      pcie_i_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_L1SUB_AHB_CLK_MAX_TIMER, RESET), 0x1);
      /* Wait for 3 AUX clk cycles */
      pcie_osal_busywait_us(3);
      pcie_i_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_L1SUB_AHB_CLK_MAX_TIMER,CNT_MAX),(uint32)(maxtimer*(nfreq/1000000)));
      /* Wait for 1 AUX clk cycles */
      pcie_osal_busywait_us(1);
      pcie_i_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_L1SUB_AHB_CLK_MAX_TIMER,RESET),0x0);
      pcie_i_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_DEBUG_INT_EN,L1SUB_TIMEOUT_BIT),0X1);
   }
   else
   {
      pcie_i_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_DEBUG_INT_EN,L1SUB_TIMEOUT_BIT),0X0);
      nfreq = 0;
   }
#endif /* PCIE_BOOT */
   return (nfreq);
}

/* ============================================================================
**  Function : pcie_core_enable_interrupt
** ============================================================================
*/
/**
 * Enables the interrupt bits for specific interrupts
 *
 * @param[in]       void
 *
 * @return          void
 */
void pcie_core_enable_interrupt(void)
{
#ifndef PCIE_BOOT
   /* Enable Device State change interrupt bit */
   pcie_i_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_DEBUG_INT_EN, PM_DSTATE_CHANGE_BIT), 0X1);

   /* Disable L1ss inactivity interrupt */
   pcie_control_l1ss_inact_interrupt(FALSE, PCIE_L1SUB_TIMER_ZERO_VAL);
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_core_get_link_aspm
** ============================================================================
*/
/**
 * Gets the current value of the Active State PM Control
 *
 * @param[in]       void
 *
 * @return          pcie_core_aspm_config_type
 */
pcie_core_aspm_config_type pcie_core_get_link_aspm(void)
{
   pcie_core_aspm_config_type val;
   /* Read current value of ASPM capability */
   val = (pcie_core_aspm_config_type) pcie_i_read_reg_field(pcie_dev->dbi_base, PCIE_FLD(LINK_CONTROL_LINK_STATUS_REG, PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL));
   return(val);
}

/* ============================================================================
**  Function : pcie_core_set_link_aspm
** ============================================================================
*/
/**
 * Sets the value of the Active State PM control
 *
 * @param[in]       val     Value to be set for the Active State PM Control
 *
 * @return          void
 */
void pcie_core_set_link_aspm(pcie_core_aspm_config_type val)
{
   /* Sets the value of the Active State link PM control */
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(LINK_CONTROL_LINK_STATUS_REG, PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL), val);
}

/* ============================================================================
**  Function : pcie_core_set_aux_clk_freq_mhz
** ============================================================================
*/
/**
 * Sets the value of the Aux clk frequency register
 *
 * @param[in]       val     Value to be set for the Aux clk frequency register in MHz
 *
 * @return          void
 */
void pcie_core_set_aux_clk_freq_mhz(uint32 val)
{
   /* Sets the value of the Aux clk frequency register */
   pcie_i_write_reg(pcie_dev->dbi_base,TYPE0_AUX_CLK_FREQ_REG, val);
}

/* ============================================================================
**  Function : pcie_core_enable_mhi_lpm
** ============================================================================
*/
/**
 * Enable Low power mode on MHI
 *
 * @param[in]       void
 *
 * @return          void
 */
void pcie_core_enable_mhi_lpm(void)
{
   /* Enable Low power mode on MHI (shutdown in L1ss) */
   /* a. Clear PCIE20_PARF_MHI_CLOCK_RESET_CTRL .MSTR_AXI_CLK_EN. */
   pcie_i_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_MHI_CLOCK_RESET_CTRL,MSTR_AXI_CLK_EN),0);
   /* b. Clear PCIE20_PARF_SYS_CTRL .MSTR_ACLK_CGC_DIS. */
   pcie_i_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_SYS_CTRL,MSTR_ACLK_CGC_DIS),0);
}

/* ============================================================================
**  Function : pcie_core_sw_int_ctrl
** ============================================================================
*/
/**
 * Enable/disable the SW interrupt set in the mask
 *
 * @param[in]      mask    Mask for the SW interrupt
 * @param[in]      ctrl    Control value(1-Enable/0-disable)
 *
 * @return          void
 */
void pcie_core_sw_int_ctrl(uint32 mask,boolean ctrl)
{
   uint32 regval;
   regval = pcie_i_read_reg(pcie_dev->parf_base,PCIE20_INT_ALL_MASK);

   if(ctrl)
   {
      regval = regval | mask;
      pcie_dev->pcie_sw_int_mask = regval;
   }
   else
   {
      regval = regval & (~mask);
   }
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_INT_ALL_MASK,regval);

   if(!ctrl)
   {
      pcie_dev->pcie_sw_int_mask = regval;
   }
}

/* ============================================================================
**  Function : pcie_core_get_sw_int_mask
** ============================================================================
*/
/**
 * Get the current mask for the SW interrupts
 *
 * @param[in]       void
 *
 * @return          uint32
 */
uint32 pcie_core_get_sw_int_mask(void)
{
   uint32 mask;
   mask = pcie_i_read_reg(pcie_dev->parf_base,PCIE20_INT_ALL_MASK);
   return mask;
}

/* ============================================================================
**  Function : pcie_core_get_sw_int_status
** ============================================================================
*/
/**
 * Get the status for the SW interrupts
 *
 * @param[in]       void
 *
 * @return          uint32
 */
uint32 pcie_core_get_sw_int_status(void)
{
   uint32 status;
   status = pcie_i_read_reg(pcie_dev->parf_base,PCIE20_INT_ALL_STATUS);
   return(status);
}

/* ============================================================================
**  Function : pcie_core_set_sw_int_clear
** ============================================================================
*/
/**
 * Set the clear status for the specified SW interrupts
 *
 * @param[in]       mask     Mask value for the register
 *
 * @return          void
 */
void pcie_core_set_sw_int_clear(uint32 mask)
{
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_INT_ALL_CLEAR,mask);
}

/* ============================================================================
**  Function : pcie_core_enable_ecam_blocker
** ============================================================================
*/
/**
 * Enables the blocking of AXI transactions for the region specified
 *
 * @param[in]       offset     Start offset of the region
 * @param[in]       size       Size of the region
 *
 * @return          void
 */
void pcie_core_enable_ecam_blocker(uint32 offset, uint32 size)
{
   uint32 pcie_axi_block_region_base, pcie_axi_block_region_base_hi;

   /*  Set the base address for blocking based on Active Mode */
   pcie_axi_block_region_base    = PCIE_PCIE20_WRAPPER_AXI_BASE_PHYS;

   if(TRUE == pcie_core_is_in_active_mode())
   {
      pcie_axi_block_region_base_hi = PCIE_SET_40_BIT;
   }
   else
   {
      pcie_axi_block_region_base_hi = 0x00000000;
   }

   /* Set Absolute base address for ECAM */
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_PARF_ECAM_BASE, 0);
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_PARF_ECAM_BASE_HI, 0);
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_PARF_ECAM_OFFSET_REMOVAL_BASE, 0);
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_PARF_ECAM_OFFSET_REMOVAL_BASE_HI, 0);
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_PARF_ECAM_OFFSET_REMOVAL_LIMIT, 0);
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_PARF_ECAM_OFFSET_REMOVAL_LIMIT_HI, 0);

   /* Se Base address for write blocking feature */
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE, ((pcie_axi_block_region_base + offset)&~0xFFF));
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_HI, pcie_axi_block_region_base_hi);

   /* Set Limit address for write blocking feature */
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT, ((pcie_axi_block_region_base + offset + size)&~0xFFF));
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_HI, pcie_axi_block_region_base_hi);

   /* Set Base address for read blocking feature */
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE, ((pcie_axi_block_region_base + offset)&~0xFFF));
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_HI, pcie_axi_block_region_base_hi);

   /* Set Limit address for read blocking feature */
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT,((pcie_axi_block_region_base + offset + size)&~0xFFF));
   pcie_i_write_reg(pcie_dev->parf_base,PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_HI, pcie_axi_block_region_base_hi);

   /* Enable ECAM blocker */
   pcie_i_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_SYS_CTRL, ECAM_BLOCKER_EN), 1);
   pcie_i_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_SYS_CTRL, ECAM_REMOVE_OFFSET_EN), 0);
}

/* ============================================================================
**  Function : pcie_core_disable_ecam_blocker
** ============================================================================
*/
/**
 * Disables the blocking of AXI transactions
 *
 * @param[in]       void
 *
 * @return          void
 */
void pcie_core_disable_ecam_blocker(void)
{
   /* Disable the ECAM blocking */
   pcie_i_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_SYS_CTRL, ECAM_BLOCKER_EN), 0);
}

/* ============================================================================
**  Function : pcie_core_is_in_active_mode
** ============================================================================
*/
/**
 * Checks if the Active mode configuration is enabled
 *
 * @param[in]       void
 *
 * @return          TRUE  if active mode is set
 *                  FALSE if active mode is not set
 */
boolean pcie_core_is_in_active_mode(void)
{
   uint32 val;

   /* Read the PARF to see if active config is set or not */
    val = pcie_i_read_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_SLV_ADDR_MSB_CTRL,SLV_ADDR_MSB_CTRL));

    if(0x1 == val)
    {
       return(TRUE);
    }
    else
    {
       return(FALSE);
    }
}

