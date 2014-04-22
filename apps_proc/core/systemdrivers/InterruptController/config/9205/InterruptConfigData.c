
/*==============================================================================

  D A L I N T E R R U P T   C O N T R O L L E R

  DESCRIPTION
    This auto-generated file contains the interrupt configuration data for
    all the interrupts for this platform.

  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary/GTDR
===========================================================================

  $Header: //components/rel/core.tx/6.0/systemdrivers/InterruptController/config/9205/InterruptConfigData.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $
===========================================================================*/

#include "DALReg.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "l4_qgic/DALInterruptControllerConfig.h"
#include <HALqgic.h>



/*=========================================================================

                           DATA DEFINITIONS

===========================================================================*/

/*
 * QGIC_SPI_INTERRUPT_OFFSET
 * 
 *  MSS IRQ shifted 32
 */
#define QGIC_SPI_INTERRUPT_OFFSET 32
#define QGIC_PPI_INTERRUPT_OFFSET 16

#define INTERRUPT_LOWEST_PRIORITY_LEVEL      248
#define INTERRUPT_PRIORITY_INCREMENTS        8

#define INTERRUPT_MEDIAN_PRIORITY_NS ( INTERRUPT_MEDIAN_PRIORITY + INTERRUPT_START_PRIORITY )

#define INTERRUPT_SET_PRIORITY(x)  \
(((x * INTERRUPT_PRIORITY_INCREMENTS) + INTERRUPT_START_PRIORITY) < INTERRUPT_LOWEST_PRIORITY_LEVEL ?  \
 ((x * INTERRUPT_PRIORITY_INCREMENTS) + INTERRUPT_START_PRIORITY) : INTERRUPT_LOWEST_PRIORITY_LEVEL)


/*
 * InterruptConfigs
 * BSP data for this target's interrupts.
 */
static InterruptConfigType InterruptConfigs[] =
{
  {17, INTERRUPT_MEDIAN_PRIORITY_NS, "spm_vote_int"},
  {18, INTERRUPT_SET_PRIORITY(0), "APCS_sysWdogBiteInt[0]"},
  {19, INTERRUPT_SET_PRIORITY(1), "WDT_barkInt"},
  {20, INTERRUPT_MEDIAN_PRIORITY_NS, "APCN_AvsSvicInt_Q"},
  {21, INTERRUPT_MEDIAN_PRIORITY_NS, "APCN_AvsSvicIntSwDone_Q"},
  {22, INTERRUPT_MEDIAN_PRIORITY_NS, "APCN_dbgCommRxFull"},
  {23, INTERRUPT_MEDIAN_PRIORITY_NS, "APCN_dbgCommTxEmpty"},
  {24, INTERRUPT_MEDIAN_PRIORITY_NS, "APCN_qgicPerfMonIrptReq"},
  {25, INTERRUPT_MEDIAN_PRIORITY_NS, "APCN_avsDown"},
  {26, INTERRUPT_MEDIAN_PRIORITY_NS, "nCNTVIRQ"},
  {27, INTERRUPT_MEDIAN_PRIORITY_NS, "nCNTHPIRQ"},
  {28, INTERRUPT_MEDIAN_PRIORITY_NS, "nCNTPNSIRQ"},
  {29, INTERRUPT_MEDIAN_PRIORITY_NS, "nCNTPSIRQ"},
  {35, INTERRUPT_MEDIAN_PRIORITY_NS, "CTI_SPI_intx"},
  {36, INTERRUPT_MEDIAN_PRIORITY_NS, "CTI_SPI_inty"},
  {38, INTERRUPT_MEDIAN_PRIORITY_NS, "QTMR_qgicFrm0VirtIrq"},
  {39, INTERRUPT_MEDIAN_PRIORITY_NS, "QTMR_qgicFrm0PhyIrq"},
  {40, INTERRUPT_MEDIAN_PRIORITY_NS, "QTMR_qgicFrm1PhyIrq"},
  {41, INTERRUPT_MEDIAN_PRIORITY_NS, "QTMR_qgicFrm2PhyIrq"},
  {42, INTERRUPT_MEDIAN_PRIORITY_NS, "QTMR_qgicFrm3PhyIrq"},
  {43, INTERRUPT_MEDIAN_PRIORITY_NS, "QTMR_qgicFrm4PhyIrq"},
  {44, INTERRUPT_MEDIAN_PRIORITY_NS, "QTMR_qgicFrm5PhyIrq"},
  {45, INTERRUPT_MEDIAN_PRIORITY_NS, "QTMR_qgicFrm6PhyIrq"},
  {46, INTERRUPT_MEDIAN_PRIORITY_NS, "QTMR_qgicFrm7PhyIrq"},
  {56, INTERRUPT_MEDIAN_PRIORITY_NS, "q6_wdog_expired_irq"},
  {57, INTERRUPT_MEDIAN_PRIORITY_NS, "mss_to_apps_irq[0]"},
  {58, INTERRUPT_MEDIAN_PRIORITY_NS, "mss_to_apps_irq[1]"},
  {59, INTERRUPT_MEDIAN_PRIORITY_NS, "mss_to_apps_irq[2]"},
  {60, INTERRUPT_MEDIAN_PRIORITY_NS, "mss_to_apps_irq[3]"},
  {61, INTERRUPT_MEDIAN_PRIORITY_NS, "mss_a2_bam_irq"},
  {62, INTERRUPT_MEDIAN_PRIORITY_NS, "nAXIERRIRQ_int"},
  {64, INTERRUPT_MEDIAN_PRIORITY_NS, "summary_irq_mdio"},
  {65, INTERRUPT_MEDIAN_PRIORITY_NS, "pka_core_irq_tz"},
  {66, INTERRUPT_MEDIAN_PRIORITY_NS, "pka_core_irq_hv"},
  {67, INTERRUPT_MEDIAN_PRIORITY_NS, "pka_core_irq_ee"},
  {68, INTERRUPT_MEDIAN_PRIORITY_NS, "qdss_tpda_gerr_irq"},
  {71, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_lpa_dsp[5]"},
  {72, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_lpa_dsp[4]"},
  {74, INTERRUPT_MEDIAN_PRIORITY_NS, "usb20s_power_event_irq"},
  {75, INTERRUPT_MEDIAN_PRIORITY_NS, "usb20s_ee1_irq"},
  {76, INTERRUPT_MEDIAN_PRIORITY_NS, "cri_cm_irq_tz"},
  {77, INTERRUPT_MEDIAN_PRIORITY_NS, "cri_cm_irq_hyp"},
  {78, INTERRUPT_MEDIAN_PRIORITY_NS, "qpic_irq"},
  {106, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_lpa_dsp[2]"},
  {107, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_lpa_dsp[1]"},
  {115, INTERRUPT_MEDIAN_PRIORITY_NS, "tsync"},
  {117, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_lpa_dsp[0]"},
  {118, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_sensors[7]"},
  {119, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_sensors[6]"},
  {120, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_sensors[5]"},
  {121, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_sensors[4]"},
  {122, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_sensors[3]"},
  {123, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_sensors[2]"},
  {124, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_sensors[1]"},
  {125, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_sensors[0]"},
  {127, INTERRUPT_MEDIAN_PRIORITY_NS, "peripheral_irq[4]"},
  {128, INTERRUPT_MEDIAN_PRIORITY_NS, "peripheral_irq[5]"},
  {129, INTERRUPT_MEDIAN_PRIORITY_NS, "peripheral_irq[6]"},
  {130, INTERRUPT_MEDIAN_PRIORITY_NS, "peripheral_irq[7]"},
  {139, INTERRUPT_MEDIAN_PRIORITY_NS, "peripheral_irq[0]"},
  {140, INTERRUPT_MEDIAN_PRIORITY_NS, "peripheral_irq[1]"},
  {151, INTERRUPT_MEDIAN_PRIORITY_NS, "peripheral_irq[2]"},
  {152, INTERRUPT_MEDIAN_PRIORITY_NS, "peripheral_irq[3]"},
  {160, INTERRUPT_MEDIAN_PRIORITY_NS, "qpic_nandc_wr_er_done_irq"},
  {161, INTERRUPT_MEDIAN_PRIORITY_NS, "qpic_nandc_op_done_irq"},
  {163, INTERRUPT_MEDIAN_PRIORITY_NS, "qpic_lcdc_irq"},
  {164, INTERRUPT_MEDIAN_PRIORITY_NS, "qpic_bam_irq[0]"},
  {165, INTERRUPT_MEDIAN_PRIORITY_NS, "o_tcsr_s1lm_modem_to_apps_intr"},
  {166, INTERRUPT_MEDIAN_PRIORITY_NS, "usb20s_hs_phy_irq"},
  {168, INTERRUPT_MEDIAN_PRIORITY_NS, "usb2phy_irq"},
  {175, INTERRUPT_MEDIAN_PRIORITY_NS, "usb20s_bam_irq[0]"},
  {176, INTERRUPT_MEDIAN_PRIORITY_NS, "usb20s_bam_irq[1]"},
  {177, INTERRUPT_MEDIAN_PRIORITY_NS, "usb20s_bam_irq[2]"},
  {178, INTERRUPT_MEDIAN_PRIORITY_NS, "usb20s_bam_irq[3]"},
  {179, INTERRUPT_MEDIAN_PRIORITY_NS, "usb20s_bam_irq[4]"},
  {180, INTERRUPT_MEDIAN_PRIORITY_NS, "usb20s_bam_irq[5]"},
  {181, INTERRUPT_MEDIAN_PRIORITY_NS, "usb20s_bam_irq[6]"},
  {182, INTERRUPT_MEDIAN_PRIORITY_NS, "usb20s_bam_irq[7]"},
  {192, INTERRUPT_MEDIAN_PRIORITY_NS, "audio_out0_irq"},
  {193, INTERRUPT_MEDIAN_PRIORITY_NS, "summary_irq_sensors"},
  {196, INTERRUPT_MEDIAN_PRIORITY_NS, "channel3_apps_hv_trans_done_irq"},
  {197, INTERRUPT_MEDIAN_PRIORITY_NS, "ee3_apps_hv_spmi_periph_irq"},
  {198, INTERRUPT_MEDIAN_PRIORITY_NS, "qdss_etrbytecnt_irq"},
  {199, INTERRUPT_MEDIAN_PRIORITY_NS, "qdss_usb_trace_bam_irq[0]"},
  {200, INTERRUPT_SET_PRIORITY(3), "rpm_ipc[4]"},
  {201, INTERRUPT_SET_PRIORITY(3), "rpm_ipc[5]"},
  {202, INTERRUPT_SET_PRIORITY(3), "rpm_ipc[6]"},
  {203, INTERRUPT_SET_PRIORITY(3), "rpm_ipc[7]"},
  {204, INTERRUPT_SET_PRIORITY(3), "rpm_ipc[24]"},
  {205, INTERRUPT_SET_PRIORITY(3), "rpm_ipc[25]"},
  {206, INTERRUPT_SET_PRIORITY(3), "rpm_ipc[26]"},
  {207, INTERRUPT_SET_PRIORITY(3), "rpm_ipc[27]"},
  {208, INTERRUPT_MEDIAN_PRIORITY_NS, "rbif_irq[0]"},
  {209, INTERRUPT_MEDIAN_PRIORITY_NS, "rbif_irq[1]"},
  {210, INTERRUPT_MEDIAN_PRIORITY_NS, "rbif_irq[2]"},
  {211, INTERRUPT_MEDIAN_PRIORITY_NS, "o_bimc_intr[1]"},
  {213, INTERRUPT_MEDIAN_PRIORITY_NS, "secure_wdog_bark_irq"},
  {214, INTERRUPT_MEDIAN_PRIORITY_NS, "tsens_max_min_int"},
  {215, INTERRUPT_MEDIAN_PRIORITY_NS, "o_bimc_intr[0]"},
  {216, INTERRUPT_MEDIAN_PRIORITY_NS, "tsens_upper_lower_int"},
  {217, INTERRUPT_MEDIAN_PRIORITY_NS, "o_ocimem_nonsec_irq"},
  {218, INTERRUPT_MEDIAN_PRIORITY_NS, "channel1_apps_trustzone_trans_done_irq"},
  {219, INTERRUPT_MEDIAN_PRIORITY_NS, "channel0_apps_hlos_trans_done_irq"},
  {220, INTERRUPT_MEDIAN_PRIORITY_NS, "spmi_protocol_irq"},
  {221, INTERRUPT_MEDIAN_PRIORITY_NS, "ee1_apps_trustzone_spmi_periph_irq"},
  {222, INTERRUPT_MEDIAN_PRIORITY_NS, "ee0_apps_hlos_spmi_periph_irq"},
  {223, INTERRUPT_MEDIAN_PRIORITY_NS, "spdmo_offline_irq"},
  {224, INTERRUPT_MEDIAN_PRIORITY_NS, "spdmo_realtime_irq[1]"},
  {226, INTERRUPT_MEDIAN_PRIORITY_NS, "pcnoc_sbm_mainFault"},
  {227, INTERRUPT_MEDIAN_PRIORITY_NS, "o_tcsr_xpu2_sec_summary_intr"},
  {228, INTERRUPT_MEDIAN_PRIORITY_NS, "o_tcsr_xpu2_non_sec_summary_intr"},
  {229, INTERRUPT_MEDIAN_PRIORITY_NS, "o_timeout_slave_kpss_summary_intr"},
  {230, INTERRUPT_MEDIAN_PRIORITY_NS, "o_tcsr_vmidmt_client_sec_summary_intr"},
  {231, INTERRUPT_MEDIAN_PRIORITY_NS, "o_tcsr_vmidmt_client_non_sec_summary_intr"},
  {232, INTERRUPT_MEDIAN_PRIORITY_NS, "o_tcsr_vmidmt_cfg_sec_summary_intr"},
  {233, INTERRUPT_MEDIAN_PRIORITY_NS, "o_tcsr_vmidmt_cfg_non_sec_summary_intr"},
  {236, INTERRUPT_MEDIAN_PRIORITY_NS, "crypto_core_irq[0]"},
  {237, INTERRUPT_MEDIAN_PRIORITY_NS, "crypto_core_irq[1]"},
  {238, INTERRUPT_MEDIAN_PRIORITY_NS, "crypto_bam_irq[0]"},
  {239, INTERRUPT_MEDIAN_PRIORITY_NS, "crypto_bam_irq[1]"},
  {240, INTERRUPT_MEDIAN_PRIORITY_NS, "summary_irq_apss"},
  {241, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_apss[7]"},
  {242, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_apss[6]"},
  {243, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_apss[5]"},
  {244, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_apss[4]"},
  {245, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_apss[3]"},
  {246, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_apss[2]"},
  {247, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_apss[1]"},
  {248, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_apss[0]"},
  {249, INTERRUPT_MEDIAN_PRIORITY_NS, "summary_irq_apss_tz"},
  {250, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_sensors[9]"},
  {251, INTERRUPT_MEDIAN_PRIORITY_NS, "dir_conn_irq_sensors[8]"},
  {258, INTERRUPT_SET_PRIORITY(3), "rpm_ipc[28]"},
  {259, INTERRUPT_SET_PRIORITY(3), "rpm_ipc[29]"},
  {269, INTERRUPT_MEDIAN_PRIORITY_NS, "rpm_wdog_expired_irq"},
  {270, INTERRUPT_MEDIAN_PRIORITY_NS, "bam_irq[0]"},
  {274, INTERRUPT_MEDIAN_PRIORITY_NS, "to_error_int"},
  {275, INTERRUPT_SET_PRIORITY(3), "rpm_ipc[30]"},
  {276, INTERRUPT_SET_PRIORITY(3), "rpm_ipc[31]"},
  {280, INTERRUPT_MEDIAN_PRIORITY_NS, "task_done_int"},
  {281, INTERRUPT_MEDIAN_PRIORITY_NS, "crc_error_int"},
  {INVALID_INTERRUPT, INTERRUPT_MEDIAN_PRIORITY_NS, ""}
};

 /*
  * InterruptBaseAddressType
  * Mapping for QGIC addresses.
  */
static InterruptBaseAddressType QGICBaseAddressData =
{
  (uint8*)0x0B000000, /* qgic_base_address */
  0,                  /* qgic_distributor offset */
  0x2000,             /* qgic_cpu_address offset */
  0x1000,             /* hypervisor_address offset */
  0x4000,             /* virtualization_cpu_address offset */
};


/*
 * pInterruptControllerConfigData
 * This is used by the interrupt controller driver to query platform specific data.
 */
InterruptPlatformDataType pInterruptControllerConfigData[] =
{
  {
    QGIC_MODE_TRUSTZONE,        /* eMode */ 
    282,                        /* nMaxIRQ */
    InterruptConfigs,           /* pIRQConfigs */
    &QGICBaseAddressData,       /* psQgicAddr */
  }
};
