/**
 * @file pcie_tgt_config.c
 * @brief
 * Has the target configurable parameters.
 */
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================

Copyright (c) 2014-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "pcie_hwio.h"
#include "pcie_tgt.h"
#include "VCSDefs.h"

/** define IRQ for PCIe interrupts */
#define PCIE_GLOBAL_INT_IRQ              81     /** PCIe Global Interrupt IRQ */
#define PCIE_MSI_0_INT_IRQ               85     /** PCIe MSI 0 Interrupt  IRQ */
#define PCIE_MSI_1_INT_IRQ               86     /** PCIe MSI 1 Interrupt  IRQ */
#define PCIE_MSI_2_INT_IRQ               87     /** PCIe MSI 2 Interrupt  IRQ */
#define PCIE_MSI_3_INT_IRQ               88     /** PCIe MSI 3 Interrupt  IRQ */
#define PCIE_MSI_4_INT_IRQ               89     /** PCIe MSI 4 Interrupt  IRQ */
#define PCIE_MSI_5_INT_IRQ               90     /** PCIe MSI 5 Interrupt IRQ */
#define PCIE_MSI_6_INT_IRQ               91     /** PCIe MSI 6 Interrupt  IRQ */
#define PCIE_MSI_7_INT_IRQ               92     /** PCIe MSI 7 Interrupt IRQ  */
#define PCIE_EDMA_INT_IRQ                93     /** PCIe EDMA Interrupt  IRQ */


/** define gpios pin numbers for PCIe */
#define PCIE_GPIO_WAKE_PINOUT                 61                   /** WAKE GPIO */
#define PCIE_GPIO_CLKREQ_PINOUT               64                   /** CLKREQ GPIO */
#define PCIE_GPIO_PERST_PINOUT                65                   /** PERST GPIO */

#define PCIE_MAX_BARS                         (6)                  /** Max Number of BARs per spec */
#define PCIE_LINK_TIMEOUT_MS                  (4000)               /** Link Training Timeout is 1 second per spec */

#define PCIE_ENUMERATION_TIMEOUT_MS           1500                 /** PCIe Enumeration Timeout */
#define PCIE_DEFAULT_LTR                      16000                /** Default value for LTR */

#define PCIE_PCNOC_SVS_MODE_SPEED_KHZ         50000                /** PCNOC SVS mode Frequency in KHz*/
#define PCIE_PCNOC_TURBO_MODE_SPEED_MHZ       133                  /** PCNOC Turbo mode Frequency in MHz */
#define PCIE_SNOC_SVS_MODE_SPEED_KHZ          85000                /** SNOC SVS mode Frequency in KHz */

#define PCIE_PARF_SLV_ADDR_SPACE_SIZE         0x0000020000000000   /** PARF Slave Address Space size */

const pcie_gpio_cfg_type pcie_gpio_config[] =
{
   /* for PERST GPIO */
   {
      PCIE_GPIO_PERST,              /* GPIO ID */
      TRUE,                         /* TLMM cfg */
      TRUE,                         /* Interrupt enbled */
      NULL,                         /* ISR */
      PCIE_GPIO_PERST_PINOUT,       /* GPIO number */
      0,                            /* GPIO func */
      PCIE_GPIO_INPUT,              /* GPIO dir */
      PCIE_GPIO_PULL_DOWN,          /* GPIO pull */
      PCIE_GPIO_2MA,                /* GPIO drive */
      PCIE_GPIO_ENABLE,             /* GPIO default config enable/Disable */
      NULL,                         /* GPIO Alternate cofiguration */
      PCIE_GPIO_LOW                 /* Default output value */
   },

   /* for wake gpio */
   {
      PCIE_GPIO_WAKE,               /* GPIO ID */
      TRUE,                         /* TLMM cfg */
      FALSE,                        /* Interrupt enbled */
      NULL,                         /* ISR */
      PCIE_GPIO_WAKE_PINOUT,        /* GPIO num */
      0,                            /* GPIO func */
      PCIE_GPIO_OUTPUT,             /* GPIO dir */
      PCIE_GPIO_NO_PULL,            /* GPIO pull */
      PCIE_GPIO_2MA,                /* GPIO drive */
      PCIE_GPIO_ENABLE,             /* GPIO default config enable/Disable */
      PCIE_GPIO_CFG_OUT(PCIE_GPIO_WAKE_PINOUT, 0, PCIE_GPIO_OUTPUT, PCIE_GPIO_NO_PULL, PCIE_GPIO_2MA, PCIE_GPIO_HIGH),   /* GPIO Alternate cofiguration */
      PCIE_GPIO_HIGH                /* Default output value */
   },

   /* for CLKREQ gpio */
   {
      PCIE_GPIO_CLKREQ,             /* GPIO ID */
      TRUE,                         /* TLMM cfg */
      /* Interrupt enbled */
#ifdef PCIE_CLKREQ_INTR_SUPPORTED
      TRUE,
#else
      FALSE,
#endif
      NULL,                         /* ISR */
      PCIE_GPIO_CLKREQ_PINOUT,      /* GPIO num */
      1,                            /* GPIO func */
      PCIE_GPIO_INPUT,              /* GPIO dir */
      PCIE_GPIO_NO_PULL,            /* GPIO pull */
      PCIE_GPIO_2MA,                /* GPIO drive */
      PCIE_GPIO_ENABLE,             /* GPIO default config enable/Disable */
      NULL,                         /* GPIO Alternate cofiguration */
      PCIE_GPIO_LOW                 /* Default output value */
   },

   /* for invalid */
   {
      PCIE_MAX_GPIO,                /* GPIO ID */
      FALSE,                        /* TLMM cfg */
      FALSE,                        /* Interrupt enbled */
      NULL,                         /* ISR */
      0,                            /* GPIO num */
      0,                            /* GPIO func */
      PCIE_GPIO_INPUT,              /* GPIO dir */
      PCIE_GPIO_NO_PULL,            /* GPIO pull */
      PCIE_GPIO_2MA,                /* GPIO drive */
      PCIE_GPIO_DISABLE,            /* GPIO default config enable/Disable */
      NULL,                         /* GPIO Alternate cofiguration */
      PCIE_GPIO_LOW                 /* Default output value */
   },
};

/** Target specific configuration data */
const pcie_tgtcfg_type pcie_tgt_config =
{
   0x0302,           /* Device ID */
   0x17CB,           /* Vendor ID */
   {
      /* bar,    loc_addr_lo,         loc_addr_hi,     mask    mode,                        type,                  memspace */
      {  0,      PCIE_MHI_BASE_PHYS,  0x00000000,      0xFFF,  PCIE_BAR_NONPREFETCHABLE,    PCIE_BARTYPE_64BIT,    PCIE_BARMEMSPACE_MEM },
      {  1,      0x0,                 0x0,             0x0,    PCIE_BAR_NONPREFETCHABLE,    PCIE_BARTYPE_32BIT,    PCIE_BARMEMSPACE_MEM },       /*Disabled since BAR0 is 64bit*/
      {  2,      PCIE_MHI_BASE_PHYS,  0x00000000,      0xFFF,  PCIE_BAR_NONPREFETCHABLE,    PCIE_BARTYPE_64BIT,    PCIE_BARMEMSPACE_MEM },       /*Disabled since BAR2 is 64bit*/
      {  3,      0x0,                 0x0,             0x0,    PCIE_BAR_NONPREFETCHABLE,    PCIE_BARTYPE_32BIT,    PCIE_BARMEMSPACE_MEM },       /*Unused*/
      {  4,      0x0,                 0x0,             0x0,    PCIE_BAR_NONPREFETCHABLE,    PCIE_BARTYPE_32BIT,    PCIE_BARMEMSPACE_MEM },       /*Unused*/
      {  5,      0x0,                 0x0,             0x0,    PCIE_BAR_NONPREFETCHABLE,    PCIE_BARTYPE_32BIT,    PCIE_BARMEMSPACE_MEM },

   },            /* bar_config */

   1,           /* needs_init */
   0,           /* timeout_ms */

   PCIE_PCIE20_WRAPPER_AHB_BASE_PHYS,
   PCIE_PCIE20_WRAPPER_AHB_BASE_SIZE,

   PCIE_PCIE20_WRAPPER_AXI_BASE_PHYS,
   PCIE_PCIE20_WRAPPER_AXI_BASE_SIZE,

   PCIE20_ELBI_REG_BASE_OFFS,
   PCIE20_PARF_REG_BASE_OFFS,
   PCIE20_MHI_REG_BASE_OFFS,
   DWC_PCIE_DM_REG_BASE_OFFS,
   PCIE20_PHY_REG_BASE_OFFS,
   PCIE_PARF_SLV_ADDR_SPACE_SIZE,

   {
      { PCIE_GLOBAL_INT_IRQ,     TRUE,    LEVEL_TRIGGERED_HIGH },
      { PCIE_MSI_0_INT_IRQ,      FALSE,   LEVEL_TRIGGERED_HIGH },
      { PCIE_MSI_1_INT_IRQ,      FALSE,   LEVEL_TRIGGERED_HIGH  },
      { PCIE_MSI_2_INT_IRQ,      FALSE,   LEVEL_TRIGGERED_HIGH  },
      { PCIE_MSI_3_INT_IRQ,      FALSE,   LEVEL_TRIGGERED_HIGH  },
      { PCIE_MSI_4_INT_IRQ,      FALSE,   LEVEL_TRIGGERED_HIGH  },
      { PCIE_MSI_5_INT_IRQ,      FALSE,   LEVEL_TRIGGERED_HIGH  },
      { PCIE_MSI_6_INT_IRQ,      FALSE,   LEVEL_TRIGGERED_HIGH  },
      { PCIE_MSI_7_INT_IRQ,      FALSE,   LEVEL_TRIGGERED_HIGH  },
      { PCIE_EDMA_INT_IRQ,       FALSE,   LEVEL_TRIGGERED_HIGH  },
   },

   /* Order of the clocks here should match the enum for the clock ids defined in pcie_res.h */
   {
      "gcc_pcie_cfg_ahb_clk",
      "gcc_pcie_axi_mstr_clk",
      "gcc_pcie_axi_clk",
      "gcc_pcie_pipe_clk",
      "gcc_pcie_sleep_clk",
      "gcc_pciephy_phy_clk",
      "gcc_pcie_gpio_ref_clk_en",
   },

   19200000, /* Aux clock frequency */

   "VDD_PCIE",

   PCIE_MAX_BARS,
   PCIE_LINK_TIMEOUT_MS,

   PCIE_ENUMERATION_TIMEOUT_MS,
   PCIE_DEFAULT_LTR,

   {
      "/clk/pcnoc",
      "pcie_bus_pcnoc_npa_client",
      PCIE_PCNOC_SVS_MODE_SPEED_KHZ,
   },

   {
      "/clk/snoc",
      "pcie_bus_snoc_npa_client",
      PCIE_SNOC_SVS_MODE_SPEED_KHZ,
   },

   {
      VCS_NPA_RESOURCE_VDD_CX,
      "pcie_cx_npa_client",
      VCS_CORNER_LOW,
   },

   {
      VCS_NPA_RESOURCE_VDD_MX,
      "pcie_mx_npa_client",
      VCS_CORNER_LOW,
   },

   PCIE_PCNOC_TURBO_MODE_SPEED_MHZ,

   pcie_gpio_config
};

