/**
 * @file pcie_core.h
 * @brief
 * Provides interfaces to access the PCIe Core HW.
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

#ifndef __PCIE_CORE_H__
#define __PCIE_CORE_H__

#include "comdef.h"
#include "pcie.h"
#include "pcie_hwio.h"
#include "pcie_osal.h"

/** Link Status Type */
typedef enum {
   PCIE_LINKSTATE_DETECT_QUIET=0,    /** < -- Detect Quiet state */
   PCIE_LINKSTATE_DETECT_ACT,        /** < -- Detect Active State */
   PCIE_LINKSTATE_POLL_ACTIVE,       /** < -- Polling Active State */
   PCIE_LINKSTATE_POLL_COMPLIANCE,   /** < -- Polling Compliance State */
   PCIE_LINKSTATE_POLL_CONFIG,       /** < -- Polling Config State */
   PCIE_LINKSTATE_PRE_DETECT_QUIET,  /** < -- Exit from polling config to Detect.Quiet */
   PCIE_LINKSTATE_DETECT_WAIT,       /** < -- Wait before entering Detect.Quiet */
   PCIE_LINKSTATE_CFG_LINKWD_START,  /** < -- Config.LinkWidth.Start State */
   PCIE_LINKSTATE_CFG_LINKWD_ACEPT,  /** < -- Config.LinkWidth.Accept State */
   PCIE_LINKSTATE_CFG_LANENUM_WAIT,  /** < -- Config.LaneNumber.Wait State */
   PCIE_LINKSTATE_CFG_LANENUM_ACEPT, /** < -- Config.LaneNumber.Accept State */
   PCIE_LINKSTATE_CFG_COMPLETE,      /** < -- Config.Complete State */
   PCIE_LINKSTATE_CFG_IDLE,          /** < -- Config.Idle State */
   PCIE_LINKSTATE_RCVRY_LOCK,        /** < -- Recovery RcvrLock State */
   PCIE_LINKSTATE_RCVRY_SPEED,       /** < -- Recovery Speed State */
   PCIE_LINKSTATE_RCVRY_RCVRCFG,     /** < -- Recovery RcvrCfg State */
   PCIE_LINKSTATE_RCVRY_IDLE,        /** < -- Recovery Idle State */
   PCIE_LINKSTATE_L0,                /** < -- L0 State */
   PCIE_LINKSTATE_L0S,               /** < -- L0 Substate */
   PCIE_LINKSTATE_L123_SEND_EIDLE,   /** < -- L123 Send Enter IDLE State */
   PCIE_LINKSTATE_L1_IDLE,           /** < -- L1 Idle State */
   PCIE_LINKSTATE_L2_IDLE,           /** < -- L2 Idle State */
   PCIE_LINKSTATE_L2_WAKE,           /** < -- L2 Wake State */
   PCIE_LINKSTATE_DISABLED_ENTRY,    /** < -- Disabled State Entry */
   PCIE_LINKSTATE_DISABLED_IDLE,     /** < -- Disabled State Idle */
   PCIE_LINKSTATE_DISABLED,          /** < -- Disable State */
   PCIE_LINKSTATE_LPBK_ENTRY,        /** < -- Loopback Entry */
   PCIE_LINKSTATE_LPBK_ACTIVE,       /** < -- Loopback Active */
   PCIE_LINKSTATE_LPBK_EXIT,         /** < -- Loopback Exit */
   PCIE_LINKSTATE_LPBK_EXIT_TIMEOUT, /** < -- Loopback Exit Timeout */
   PCIE_LINKSTATE_HOT_RESET_ENTRY,   /** < -- Hot Reset Entry */
   PCIE_LINKSTATE_HOT_RESET,         /** < -- Hot Reset */
   PCIE_LINKSTATE_RCVRY_EQ0,         /** < -- Recovery Equalization Phase 0 */
   PCIE_LINKSTATE_RCVRY_EQ1,         /** < -- Recovery Equalization Phase 1 */
   PCIE_LINKSTATE_RCVRY_EQ2,         /** < -- Recovery Equalization Phase 2 */
   PCIE_LINKSTATE_RCVRY_EQ3          /** < -- Recovery Equalization Phase 3 */
}pcie_core_linkstate_type;

/** Different states of a PCIe link */
typedef enum _pcie_linkstatus_type
{
   PCIE_LINKSTATUS_DOWN       = 0, /** < -- Link is down */
   PCIE_LINKSTATUS_UP         = 1, /** < -- Link is Up */
   PCIE_LINKSTATUS_ENUMERATED = 2  /** < -- Link is enumerated and allocated an
                                    * MMIO space by the RC. Implies that
                                    * the link is up */
}pcie_linkstatus_type;

/** ALL PCIe SW Interrupt IDs */
/** Note: the order here should match the order in the actual
 *  bits. So do not modify this unless the register fields
 *  have changed */
typedef enum
{
   PCIE_INT_UNUSED_0                  = 0,          /** ID for unused 0 bit */
   PCIE_SMLH_REQ_RST_LINK_DOWN        = 1,          /** ID for Linkdown*/
   PCIE_CFG_BUS_MASTER_EN             = 2,          /** ID for BME */
   PCIE_RADM_TURNOFF                  = 3,          /** ID for RADM turn off */
   PCIE_INT_DEBUG                     = 4,          /** ID for INT Debug */
   PCIE_INT_LTR                       = 5,          /** ID for LTR */
   PCIE_INT_MHI_Q6                    = 6,          /** ID for MHI Q6 */
   PCIE_INT_MHI_A7                    = 7,          /** ID for MHI A7 */
   PCIE_INT_DSTATE_CHANGE             = 8,          /** ID for Dstate change */
   PCIE_INT_L1SS_IDLE_TO              = 9,          /** ID for L1ss Idle Timeout */
   PCIE_INT_MMIO_WRITE                = 10,         /** ID for MMIO write */
   PCIE_INT_CFG_WRITE                 = 11,         /** ID for Cfg Write */
   PCIE_INT_LINK_REQ_RST_FLUSH        = 12,         /** ID for Link Request Reset Flush */
   PCIE_INT_PLS_LINK_UP               = 13,         /** ID for Linkup */
   PCIE_INT_AER_LEGACY                = 14,         /** ID for AER legacy */
   PCIE_INT_PLS_ERR                   = 15,         /** ID for PLS error */
   PCIE_INT_PME_LEGACY                = 16,         /** ID for PME legacy */
   PCIE_INT_PLS_PME                   = 17,         /** ID for PLS PME */
   PCIE_INTD                          = 18,         /** ID for INTD */
   PCIE_INTC                          = 19,         /** ID for INTC */
   PCIE_INTB                          = 20,         /** ID for INTB */
   PCIE_INTA                          = 21,         /** ID for INTA*/
   PCIE_INT_EDMA_INT                  = 22,         /** ID for PCIe EDMA */
   PCIE_INT_MSI_DEV_7                 = 23,         /** ID for MSI_DEV_7 */
   PCIE_INT_MSI_DEV_6                 = 24,         /** ID for MSI_DEV_6 */
   PCIE_INT_MSI_DEV_5                 = 25,         /** ID for MSI_DEV_5 */
   PCIE_INT_MSI_DEV_4                 = 26,         /** ID for MSI_DEV_4 */
   PCIE_INT_MSI_DEV_3                 = 27,         /** ID for MSI_DEV_3 */
   PCIE_INT_MSI_DEV_2                 = 28,         /** ID for MSI_DEV_2 */
   PCIE_INT_MSI_DEV_1                 = 29,         /** ID for MSI_DEV_1 */
   PCIE_INT_MSI_DEV_0                 = 30,         /** ID for MSI_DEV_0 */
   PCIE_INT_UNUSED_31                 = 31,         /** ID for unused 31 bit */
   PCIE_INT_SW_ID_MAX                 = 32          /** Max SW interrupt ID */
}pcie_core_sw_int_id_type;

/** PCIe coredump type */
typedef enum {
   PCIE_FULL_DUMP = 0,                  /** < --  Dumps all PCIe registers */
   PCIE_PARTIAL_DUMP = 1                /** < --  Dumps only the PCIe AHB register */
}pcie_core_dump_type;

/** Different Device types supported by a PCIe Core */
typedef enum _pcie_devmode_type
{
   PCIE_DEV_EP = 0, /** < -- Device is an Endpoint */
   PCIE_DEV_RC = 1  /** < -- Device is a Root Complex */
}pcie_devmode_type;

/** PCIe boot chain type */
typedef enum
{
   PCIE_NO_FULL_BOOT = 0,
   PCIE_FULL_BOOT
}pcie_boot_chain_type;

/** PCIe Link Speeds */
typedef enum
{
   PCIE_CORE_TARGETSPEED_DEFAULT = 0, /** < -- Core's default speed */
   PCIE_CORE_TARGETSPEED_GEN1 = 1,    /** < -- Gen1 Speed - 2.5GT/s */
   PCIE_CORE_TARGETSPEED_GEN2 = 2,    /** < -- Gen2 Speed - 5.0GT/s */
   PCIE_CORE_TARGETSPEED_GEN3 = 4     /** < -- Gen3 Speed - 8.0GT/s */
} pcie_targetspeed_type;

/** PCIe DSTATE */
typedef enum
{
   POWER_D0 = 0,        /** < -- D0 state */
   POWER_D1,            /** < -- D1 state */
   POWER_D2,            /** < -- D2 state */
   POWER_D3             /** < -- D3 state */
} pcie_power_dstate;
/** PCIe Config structure */
typedef struct _pcie_config_type
{
   pcie_devmode_type dev_type;       /** < -- Device Type */
   pcie_targetspeed_type tgt_speed;  /** < -- Max speed to negotiate with */
   uint32 device_id;                 /** < -- Device ID */
   uint32 vendor_id;                 /** < -- Vendor ID */
}pcie_config_type;

/** Direction of the traffic for Address Translation Unit (ATU) */
typedef enum _pcie_atudir_type
{
   PCIE_ATUDIR_OUTBOUND = 0,   /** < -- Outbound ATU */
   PCIE_ATUDIR_INBOUND  = 1    /** < -- Inbound ATU */
}pcie_atudir_type;

/** ATU Modes */
typedef enum _pcie_atumode_type
{
   PCIE_ATUMODE_ADDRESSMATCHED = 0,   /** < -- Performs matches based on address range */
   PCIE_ATUMODE_BARMATCHED = 1        /** < -- Performs matches based on bar range */
}pcie_atumode_type;

/** ATU Config structure */
typedef struct _pcie_atuconfig_type
{
   uint32 index;                    /** < -- Index of the ATU region to be programmed */
   pcie_atudir_type dir;            /** < -- Direction of the ATU region */
   pcie_atumode_type mode;          /** < -- Mode of the ATU region (only address and bar matched modes supported) */
   uint32 bar;                      /** < -- Bar Index to match. Only valid for mode == PCIE_ATUMODE_BARMATCHED */
   uint32 base_addr_low;            /** < -- Untranslated lower 32-bits of the Source (untranslated) address. Valid for mode == PCIE_ATUMODE_ADDRESSMATCHED. Must be 4K aligned */
   uint32 base_addr_hi;             /** < -- Untranslated upper 32-bits of the Source (untranslated) address */
   uint32 size;                     /** < -- Size of the region to be translated. Valid for mode == PCIE_ATUMODE_ADDRESSMATCHED. Must be 4K aligned */
   uint32 target_addr_low;          /** < -- Translated lower 32-bits of the target (translated) address. Valid for both modes. Must be 4K aligned */
   uint32 target_addr_hi;           /** < -- Translated upper 32-bits of the target (translated) address. */
}pcie_atuconfig_type;


/** ASPM Control */
typedef enum {
   PCIE_DISABLE_ASPM = 0,    /** < -- ASPM Disabled */
   PCIE_L0S_ENTRY_ENABLE,    /** < -- L0s Entry enabled */
   PCIE_L1_ENTRY_ENABLE,     /** < -- L1 Entery enabled */
   PCIE_L0S_L1_ENTRY_ENABLE  /** < -- L0s and L1 Entry enabled */
}pcie_core_aspm_config_type;

/** Prefetch mode of a BAR */
typedef enum {
   PCIE_BAR_NONPREFETCHABLE = 0,  /** < -- BAR is non-prefetchable */
   PCIE_BAR_PREFETCHABLE = 1      /** < -- BAR is pre-fetchable */
}pcie_core_barmode_type;

/** Memspace type of a bar */
typedef enum {
   PCIE_BARMEMSPACE_MEM = 0,    /** < -- BAR is of memory type */
   PCIE_BARMEMSPACE_IO = 1      /** < -- BAR is of IO type */
}pcie_core_barmemspace_type;

/** Address mode of a BAR */
typedef enum {
   PCIE_BARTYPE_32BIT = 0,       /** < -- BAR is 32-bit addressable */
   PCIE_BARTYPE_64BIT = 2        /** < -- BAR is 64-bit addressable */
}pcie_core_bartype_type;

/** Bar Config structure */
typedef struct _pcie_barconfig_type
{
   uint32 bar;                           /** < -- Bar Index */
   uint32 local_addr_low;                /** < -- Lower 32-bit of local address */
   uint32 local_addr_hi;                 /** < -- Upper 32-bit of local address */
   uint32 mask;                          /** < -- Mask of the local address range */
   pcie_core_barmode_type mode;          /** < -- Mode of the Bar */
   pcie_core_bartype_type type;          /** < -- Type of the Bar */
   pcie_core_barmemspace_type memspace;  /** < -- Access type of the Bar */
}pcie_barcfg_type;

/** PCIe Core Dump Structure for HW state */
typedef struct _pcie_coredump_type
{
   uint32 parf_regs[PCIE_PARF_SIZE/sizeof(uint32)];    /** < --  Register for storing PCIe PARF Registers */
   uint32 test_bus[0x33];                              /** < --  Register for storing test bus Registers */
   uint32 phy_test_bus[30];                            /** < --  Register for storing PHY testbus registers */
   uint32 mhi_regs[PCIE_MHI_SIZE/sizeof(uint32)];      /** < --  Register for storing PCIe MHI registers */
   uint32 dbi_regs[PCIE_DBI_SIZE/sizeof(uint32)];      /** < --  Register for storing PCIe DBI Registers */
   uint32 elbi_regs[PCIE_ELBI_SIZE/sizeof(uint32)];    /** < --  Register for storing PCIe ELBI Registers */
   uint32 phy_regs[PCIE_CM_SIZE/sizeof(uint32)];       /** < --  Register for storing PCIe PHY Registers */
}pcie_coredump_type;

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
void pcie_core_req_enter_l1(void);

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
void pcie_core_req_exit_l1(void);

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
void pcie_core_initerrata(void);

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
void pcie_core_config(void);

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
enum _pcie_return_status pcie_core_init(pcie_config_type *pcie_cfg);

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
uint32 pcie_core_getversion(void);

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
void pcie_core_setcoreid(uint32 device_id, uint32 vendor_id);

/* ============================================================================
**  Function : pcie_core_enableltssm
** ============================================================================
*/
/**
 * Enables the Link Training and Status State Machine. Once the state machine
 * completes successfully, the link to the root complex should be complete. This
 * can be confirmed by polling pcie_core_getlinkstate
 *
 * @param[in]      void
 *
 * @return         void
 *
 * @sa
 * pcie_core_getlinkstate.
 */
void pcie_core_enableltssm(void);

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
pcie_linkstatus_type pcie_core_get_link_status(pcie_core_linkstate_type *link_state);

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
 *    TRUE - if link is enumerated
 *    FALSE - if link is NOT enumerated
 */
boolean pcie_core_isenumerated(void);

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
boolean pcie_core_is_in_l1ss(void);

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
void pcie_core_send_ltr(uint32 ltr_us);

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
void pcie_core_setdevicetype(pcie_devmode_type devtype);

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
pcie_devmode_type pcie_core_getdevicetype(void);

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
void pcie_core_settargetspeed(pcie_targetspeed_type targetspeed);

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
pcie_targetspeed_type pcie_core_gettargetspeed(void);

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
void pcie_core_setatuconfig(boolean enable, pcie_atuconfig_type *atu_config);

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
void pcie_core_setbarconfig(pcie_barcfg_type *bar_config);

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
void pcie_core_getmsiconfig(pcie_msiconfig_type *msi_config);

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
void pcie_core_sendparfmsi(uint32 msi_vector);

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
 * @param[in,out]  pcie_dev    Handle to the pcie device object
 * @param[in,out]  bar_config  Configuration to program the MHI gate
 *
 * @return         void
 */
void pcie_core_configmhigate(pcie_barcfg_type *bar_config);

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
void pcie_core_enter_lpm(void);

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
void pcie_core_exit_lpm(void);

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
void pcie_core_dump(pcie_coredump_type *pcie_coredump, pcie_core_dump_type pcie_dump_type);

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
uint32 pcie_core_control_l1ss_inact_timeout(boolean ctrl,uint32 maxtimer);

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
void pcie_core_enable_interrupt(void);

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
pcie_core_aspm_config_type pcie_core_get_link_aspm(void);

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
void pcie_core_set_link_aspm(pcie_core_aspm_config_type val);

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
void pcie_core_set_aux_clk_freq_mhz(uint32 val);

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
void pcie_core_enable_mhi_lpm(void);

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
void pcie_core_sw_int_ctrl(uint32 mask,boolean ctrl);

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
uint32 pcie_core_get_sw_int_mask(void);

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
uint32 pcie_core_get_sw_int_status(void);

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
void pcie_core_set_sw_int_clear(uint32 mask);

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
void pcie_core_enable_ecam_blocker(uint32 offset, uint32 size);

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
void pcie_core_disable_ecam_blocker(void);

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
boolean pcie_core_is_in_active_mode(void);
#endif /* __PCIE_CORE_H__ */
