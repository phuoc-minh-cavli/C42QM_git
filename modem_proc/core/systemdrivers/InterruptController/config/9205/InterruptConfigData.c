
/*==============================================================================

  D A L I N T E R R U P T   C O N T R O L L E R

  DESCRIPTION
    This auto-generated file contains the interrupt configuration data for
    all the interrupts for this platform.

  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary/GTDR
===========================================================================

  $Header: //components/rel/core.mpss/3.10/systemdrivers/InterruptController/config/9205/InterruptConfigData.c#4 $
  $DateTime: 2019/01/16 11:20:41 $
  $Author: pwbldsvc $
===========================================================================*/

#include "DALReg.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "DALInterruptControllerConfig.h"


/*=========================================================================

                           DATA DEFINITIONS

===========================================================================*/

/*
 * InterruptConfigs
 * BSP data for this target's interrupts.
 * The data layout is the following:
 * {  L2VIC interrupt vector number, priority, interrupt service thread stack size
 *      interrupt name}
 *
 */
static InterruptConfigType InterruptConfigs[] =
{
  {1, 4, DAL_IST_STACK_SIZE, "qdsp6_isdb_irq"},
  {2, 4, DAL_IST_STACK_SIZE, "q6_qtimer_irq_0_"},
  {3, 6, DAL_IST_STACK_SIZE, "q6_qtimer_irq_1_"},
  {4, 8, DAL_IST_STACK_SIZE, "q6_qtimer_irq_2_"},
  {10, 4, DAL_IST_STACK_SIZE, "stmr_evt0"},
  {11, 4, DAL_IST_STACK_SIZE, "stmr_evt1"},
  {12, 4, DAL_IST_STACK_SIZE, "stmr_evt2"},
  {13, 4, DAL_IST_STACK_SIZE, "rxfe_wbpwr0_irq"},
  {24, 4, DAL_IST_STACK_SIZE, "stmr_evt3"},
  {25, 4, DAL_IST_STACK_SIZE, "stmr_evt4"},
  {26, 6, DAL_IST_STACK_SIZE, "stmr_evt5"},
  {27, 4, DAL_IST_STACK_SIZE, "ccs_swi_irq[0]"},
  {28, 5, DAL_IST_STACK_SIZE, "ccs_swi_irq[1]"},
  {29, 2, DAL_IST_STACK_SIZE, "ccs_swi_irq[2]"},
  {31, 2, DAL_IST_STACK_SIZE, "q6ss_cti_irq"},
  {32, 5, DAL_IST_STACK_SIZE, "rpm_ipc[12]"},
  {33, 6, DAL_IST_STACK_SIZE, "rpm_ipc[13]"},
  {34, 2, DAL_IST_STACK_SIZE, "rpm_ipc[14]"},
  {35, 5, DAL_IST_STACK_SIZE, "rpm_ipc[15]"},
  {36, 6, DAL_IST_STACK_SIZE, "APCS_sysIPCInterrupt[12]"},
  {37, 6, DAL_IST_STACK_SIZE, "APCS_sysIPCInterrupt[13]"},
  {38, 6, DAL_IST_STACK_SIZE, "APCS_sysIPCInterrupt[14]"},
  {39, 6, DAL_IST_STACK_SIZE, "APCS_sysIPCInterrupt[15]"},
  {41, 4, DAL_IST_STACK_SIZE, "dir_conn_irq_sensors[0]"},
  {42, 4, DAL_IST_STACK_SIZE, "dir_conn_irq_sensors[1]"},
  {43, 4, DAL_IST_STACK_SIZE, "summary_irq_sensors"},
  {45, 4, DAL_IST_STACK_SIZE, "avtimer_int0_irq"},
  {46, 4, DAL_IST_STACK_SIZE, "audio_out1_irq"},
  {53, 4, DAL_IST_STACK_SIZE, "qpic_nandc_op_done_irq"},
  {54, 4, DAL_IST_STACK_SIZE, "qpic_nandc_wr_er_done_irq"},
  {55, 4, DAL_IST_STACK_SIZE, "qpic_lcdc_irq"},
  {59, 4, DAL_IST_STACK_SIZE, "vfr_irq_mux_out[0]"},
  {60, 4, DAL_IST_STACK_SIZE, "vfr_irq_mux_out[1]"},
  {61, 4, DAL_IST_STACK_SIZE, "dir_conn_irq_lpa_dsp[0]"},
  {62, 4, DAL_IST_STACK_SIZE, "dir_conn_irq_lpa_dsp[1]"},
  {63, 6, DAL_IST_STACK_SIZE, "dir_conn_irq_lpa_dsp[2]"},
  {66, 4, DAL_IST_STACK_SIZE, "clade2_err_int"},
  {68, 4, DAL_IST_STACK_SIZE, "q6_wdog_irq_int"},
  {69, 4, DAL_IST_STACK_SIZE, "o_tcsr_xpu2_msa_summary_intr"},
  {70, 4, DAL_IST_STACK_SIZE, "o_xpu2_msa_intr"},
  {71, 4, DAL_IST_STACK_SIZE, "dir_conn_irq_mss[0]"},
  {72, 4, DAL_IST_STACK_SIZE, "dir_conn_irq_mss[1]"},
  {73, 4, DAL_IST_STACK_SIZE, "crypto_core_irq[0]"},
  {74, 6, DAL_IST_STACK_SIZE, "crypto_bam_irq[0]"},
  {75, 4, DAL_IST_STACK_SIZE, "ee2_mss_spmi_periph_irq"},
  {76, 4, DAL_IST_STACK_SIZE, "channel2_mss_trans_done_irq"},
  {77, 4, DAL_IST_STACK_SIZE, "qpic_bam_irq[1]"},
  {78, 4, DAL_IST_STACK_SIZE, "coxm_uart_intr"},
  {79, 4, DAL_IST_STACK_SIZE, "coxm_typ0_b4_tout_intr"},
  {80, 4, DAL_IST_STACK_SIZE, "coxm_wcn_priority_muxed_intr"},
  {81, 4, DAL_IST_STACK_SIZE, "spdmo_realtime_irq[3]"},
  {82, 4, DAL_IST_STACK_SIZE, "spdmo_offline_irq"},
  {83, 4, DAL_IST_STACK_SIZE, "mss_ahb_access_err_irq"},
  {84, 4, DAL_IST_STACK_SIZE, "nav_irq"},
  {85, 4, DAL_IST_STACK_SIZE, "nav_dm_irq"},
  {86, 4, DAL_IST_STACK_SIZE, "tsync_irq"},
  {87, 4, DAL_IST_STACK_SIZE, "uart0_dm_intr"},
  {89, 4, DAL_IST_STACK_SIZE, "uart0_dm_uim_intr"},
  {92, 4, DAL_IST_STACK_SIZE, "phss_uart_mss_int[1]"},
  {93, 4, DAL_IST_STACK_SIZE, "o_tcsr_s1lm_apps_to_modem_intr"},
  {94, 4, DAL_IST_STACK_SIZE, "coxm_rx_wci2_msg_intr"},
  {95, 4, DAL_IST_STACK_SIZE, "coxm_rx_wci2_type2_misalign_intr"},
  {120, 4, DAL_IST_STACK_SIZE, "mcdma_ch0_done"},
  {121, 4, DAL_IST_STACK_SIZE, "mcdma_ch1_done"},
  {122, 4, DAL_IST_STACK_SIZE, "mcdma_ch2_done"},
  {123, 4, DAL_IST_STACK_SIZE, "mcdma_ch3_done"},
  {124, 4, DAL_IST_STACK_SIZE, "mcdma_vbuf_done"},
  {125, 4, DAL_IST_STACK_SIZE, "error_irq"},
  {138, 5, DAL_IST_STACK_SIZE, "a2_ulper_irq"},
  {139, 5, DAL_IST_STACK_SIZE, "rxfe_wrt_trig0_irq"},
  {140, 5, DAL_IST_STACK_SIZE, "rxfe_wrt_trig1_irq"},
  {146, 5, DAL_IST_STACK_SIZE, "rxfe_nbpwr0_irq"},
  {147, 5, DAL_IST_STACK_SIZE, "rxfe_nbpwr1_irq"},
  {153, 5, DAL_IST_STACK_SIZE, "a2_frag0_irq"},
  {154, 5, DAL_IST_STACK_SIZE, "a2_frag1_irq"},
  {155, 5, DAL_IST_STACK_SIZE, "tx_error_irq"},
  {156, 5, DAL_IST_STACK_SIZE, "tx_axi_wr_done_irq"},
  {157, 5, DAL_IST_STACK_SIZE, "tx_debug_irq"},
  {158, 5, DAL_IST_STACK_SIZE, "tx_ahb_err_irq"},
  {159, 5, DAL_IST_STACK_SIZE, "tx_axi_reader_0_done_irq"},
  {163, 5, DAL_IST_STACK_SIZE, "rxfe_spare0_irq"},
  {164, 5, DAL_IST_STACK_SIZE, "rxfe_spare1_irq"},
  {165, 5, DAL_IST_STACK_SIZE, "rxfe_spare2_irq"},
  {166, 5, DAL_IST_STACK_SIZE, "rxfe_spare3_irq"},
  {167, 5, DAL_IST_STACK_SIZE, "encrypt_a5_done_irq"},
  {168, 4, DAL_IST_STACK_SIZE, "encrypt_gea_done_irq"},
  {174, 2, DAL_IST_STACK_SIZE, "a2_tmr_irq[0]"},
  {175, 2, DAL_IST_STACK_SIZE, "a2_tmr_irq[1]"},
  {176, 2, DAL_IST_STACK_SIZE, "a2_tmr_irq[2]"},
  {177, 2, DAL_IST_STACK_SIZE, "a2_tmr_irq[3]"},
  {178, 2, DAL_IST_STACK_SIZE, "a2_decob_irq"},
  {179, 2, DAL_IST_STACK_SIZE, "demback_sch_descr_done_evt"},
  {180, 2, DAL_IST_STACK_SIZE, "demback_sch_deint_done_irq"},
  {181, 2, DAL_IST_STACK_SIZE, "demback_cchp_engine_done_evt"},
  {188, 2, DAL_IST_STACK_SIZE, "tdec_c_cp_ping_irq"},
  {189, 2, DAL_IST_STACK_SIZE, "tdec_c_dn_ping_irq"},
  {190, 2, DAL_IST_STACK_SIZE, "tdec_evt_irq"},
  {192, 2, DAL_IST_STACK_SIZE, "stmr_evt6"},
  {193, 2, DAL_IST_STACK_SIZE, "stmr_evt7"},
  {194, 2, DAL_IST_STACK_SIZE, "stmr_evt8"},
  {195, 2, DAL_IST_STACK_SIZE, "stmr_evt9"},
  {196, 2, DAL_IST_STACK_SIZE, "stmr_evt10"},
  {197, 1, DAL_IST_STACK_SIZE, "stmr_evt11"},
  {198, 4, DAL_IST_STACK_SIZE, "a2_dma0_irq"},
  {199, 1, DAL_IST_STACK_SIZE, "a2_dma1_irq"},
  {200, 1, DAL_IST_STACK_SIZE, "a2_dma2_irq"},
  {201, 1, DAL_IST_STACK_SIZE, "a2_dma3_irq"},
  {202, 1, DAL_IST_STACK_SIZE, "a2_dma4_irq"},
  {203, 1, DAL_IST_STACK_SIZE, "stmr_evt12"},
  {204, 1, DAL_IST_STACK_SIZE, "stmr_evt13"},
  {205, 1, DAL_IST_STACK_SIZE, "stmr_evt14"},
  {206, 4, DAL_IST_STACK_SIZE, "stmr_evt15"},
  {208, 4, DAL_IST_STACK_SIZE, "ccs_swi_irq[3]"},
  {209, 4, DAL_IST_STACK_SIZE, "ccs_dma0_irq"},
  {210, 4, DAL_IST_STACK_SIZE, "ccs_dma1_irq"},
  {211, 4, DAL_IST_STACK_SIZE, "ccs_dma2_irq"},
  {212, 4, DAL_IST_STACK_SIZE, "ccs_dma3_irq"},
  {213, 4, DAL_IST_STACK_SIZE, "ccs_dma4_irq"},
  {214, 4, DAL_IST_STACK_SIZE, "ccs_dma5_irq"},
  {215, 2, DAL_IST_STACK_SIZE, "ccs_dma6_irq"},
  {216, 2, DAL_IST_STACK_SIZE, "ccs_dma7_irq"},
  {217, 2, DAL_IST_STACK_SIZE, "stmr_evt16"},
  {218, 2, DAL_IST_STACK_SIZE, "stmr_evt17"},
  {219, 2, DAL_IST_STACK_SIZE, "stmr_evt18"},
  {220, 2, DAL_IST_STACK_SIZE, "stmr_evt19"},
  {221, 4, DAL_IST_STACK_SIZE, "stmr_evt20"},
  {222, 4, DAL_IST_STACK_SIZE, "stmr_evt21"},
  {223, 4, DAL_IST_STACK_SIZE, "stmr_evt22"},
  {224, 4, DAL_IST_STACK_SIZE, "stmr_evt23"},
  {225, 4, DAL_IST_STACK_SIZE, "stmr_evt24"},
  {226, 4, DAL_IST_STACK_SIZE, "stmr_evt25"},
  {227, 4, DAL_IST_STACK_SIZE, "stmr_evt26"},
  {228, 4, DAL_IST_STACK_SIZE, "stmr_evt27"},
  {229, 4, DAL_IST_STACK_SIZE, "stmr_evt28"},
  {230, 4, DAL_IST_STACK_SIZE, "stmr_evt29"},
  {231, 4, DAL_IST_STACK_SIZE, "stmr_evt30"},
  {232, 4, DAL_IST_STACK_SIZE, "stmr_evt31"},
  {236, 4, DAL_IST_STACK_SIZE, "usb20s_ee1_irq"},
  {237, 4, DAL_IST_STACK_SIZE, "usb20s_hs_phy_irq"},
  {238, 8, DAL_IST_STACK_SIZE, "o_timeout_slave_mss_summary_intr"},
  {239, 4, DAL_IST_STACK_SIZE, "summary_irq_mss"},
  {240, 4, DAL_IST_STACK_SIZE, "wcn_priority_out"},
  {241, 4, DAL_IST_STACK_SIZE, "mgpi_irq"},
  {243, 4, DAL_IST_STACK_SIZE, "phss_uart_mss_int[0]"},
  {244, 2, DAL_IST_STACK_SIZE, "bam_irq[2]"},
  {245, 4, DAL_IST_STACK_SIZE, "phss_qup_mss_int[0]"},
  {246, 4, DAL_IST_STACK_SIZE, "usb20s_bam_irq[0]"},
  {247, 4, DAL_IST_STACK_SIZE, "cri_cm_irq_msa"},
  {248, 4, DAL_IST_STACK_SIZE, "qpic_irq"},
  {249, 4, DAL_IST_STACK_SIZE, "qdss_etrbytecnt_irq"},
  {250, 4, DAL_IST_STACK_SIZE, "pka_core_irq_msa"},
  {258, 4, DAL_IST_STACK_SIZE, "coxm_dir_uart_trig_intr"},
  {259, 4, DAL_IST_STACK_SIZE, "bus_irq"},
  {INVALID_INTERRUPT, 0, DAL_IST_STACK_SIZE, ""}
};

/*
 * pInterruptControllerConfigData
 * This is used by the interrupt controller driver to query platform specific data.
 */
InterruptPlatformDataType pInterruptControllerConfigData[] =
{
  {
    .pIRQConfigs = InterruptConfigs,
    .nMaxIRQ     = 260
  }
};
