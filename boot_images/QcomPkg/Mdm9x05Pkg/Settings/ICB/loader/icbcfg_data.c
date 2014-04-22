/*==============================================================================

FILE:      icbcfg_data.c

DESCRIPTION: This file implements the ICB Configuration driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Settings/ICB/loader/icbcfg_data.c#4 $ 
$DateTime: 2019/03/15 07:00:37 $
$Author: pwbldsvc $
$Change: 18603132 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2016/11/30  pra    Remove OCMEM workaround, as POR value of OCIMEM_GEN_CTL for SDM660 is 0x00180000
2016/06/21  pra    Add SDM660 support
2016/03/31  sds    Add 8997 support
2016/03/14  sds    Settings recommendation update
2015/11/05  sds    Settings recommendation due to QCTDD02644034:
                   BIMC redirect for RICISH/RICOSH only.
2015/10/27  sds    After verification on RUMI, additional enable required.
2015/10/20  sds    Add initial QM config
2015/10/12  sds    SW workaround for QCTDD02523582:
                   OCIMEM exclusive granularity -> 64B
2015/08/24  sds    Created
 
           Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
                        All Rights Reserved.
                     QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "icbcfg.h"
#include "icbcfg_hwio.h"
#include "../../../../Library/ICBLib/icbcfgi.h"
#include "../../../../Library/ICBLib/HALbimc.h"
#include "../../../../Library/ICBLib/HALbimcHwioGeneric.h"

/*---------------------------------------------------------------------------*/
/*          Macro and constant definitions                                   */
/*---------------------------------------------------------------------------*/
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

/* BIMC register value macros */
#define SLAVE_SEGMENT(slave,index,addr_base,addr_mask)  \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_SEGMENTn_ADDR_BASEm_LOWER_ADDR((uint8_t *)BIMC_BASE,slave,index), \
   BIMC_SEGMENTn_ADDR_BASEm_LOWER_RMSK, \
   BIMC_SEGMENTn_ADDR_BASEm_LOWER_RMSK, \
   (addr_base) }, \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_SEGMENTn_ADDR_MASKm_LOWER_ADDR((uint8_t *)BIMC_BASE,slave,index), \
   BIMC_SEGMENTn_ADDR_MASKm_LOWER_RMSK, \
   BIMC_SEGMENTn_ADDR_MASKm_LOWER_RMSK, \
   (addr_mask) }
   
#define SLAVE_SEGMENT_MASK(slave,index,addr_mask)  \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_SEGMENTn_ADDR_MASKm_LOWER_ADDR(BIMC_BASE,slave,index), \
   BIMC_SEGMENTn_ADDR_MASKm_LOWER_RMSK, \
   BIMC_SEGMENTn_ADDR_MASKm_LOWER_RMSK, \
   (addr_mask) }

#define BIMC_DEFAULT(value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_DEFAULT_SEGMENT_ADDR(BIMC_BASE), \
   BIMC_DEFAULT_SEGMENT_RMSK, \
   BIMC_DEFAULT_SEGMENT_RMSK, \
   (value) }

#define ARB_MODE(slave,mode) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_S_ARB_MODE_ADDR((uint8_t *)BIMC_BASE,slave), \
   BIMC_S_ARB_MODE_RMSK, \
   BIMC_S_ARB_MODE_RMSK, \
   (mode) }

#define SWAY_INTERRUPT_ENABLE(slave,mode) \
        {ICBCFG_32_BIT_REG, \
         (void *)BIMC_S_SWAY_INTERRUPT_ENABLE_ADDR(BIMC_BASE,slave), \
         BIMC_S_SWAY_INTERRUPT_ENABLE_RMSK, \
         BIMC_S_SWAY_INTERRUPT_ENABLE_RMSK, \
         (mode) }

#define SWAY_CLOCK_CTRL(slave,mode) \
    {ICBCFG_32_BIT_REG, \
     (void *)BIMC_S_SWAY_CLOCK_CTRL_ADDR(BIMC_BASE,slave), \
     BIMC_S_SWAY_CLOCK_CTRL_RMSK, \
     BIMC_S_SWAY_CLOCK_CTRL_RMSK, \
     (mode) }

#define SWAY_MAX_OUTSTANDING_REQUESTS(slave,mode) \
      {ICBCFG_32_BIT_REG, \
       (void *)BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_ADDR(BIMC_BASE,slave), \
       BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_RMSK, \
       BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_RMSK, \
       (mode) }

#define SWAY_GATHERING(slave,mode) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_S_SWAY_GATHERING_ADDR(BIMC_BASE,slave), \
   BIMC_S_SWAY_GATHERING_RMSK, \
   BIMC_S_SWAY_GATHERING_RMSK, \
   (mode) }

/* Slave indexes */

#define SLAVE_DDR_CH0   0

#define SLAVE_DDR_CH0_ARB   0
#define SLAVE_SYS_ARB       3
#define SLAVE_DEFAULT_ARB   5


#define SLAVE_SYS_SWAY      0
#define SLAVE_DEFAULT_SWAY  1

#define MPORT_MODE(master,mode) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_MODE_ADDR((uint8_t *)BIMC_BASE,master), \
   BIMC_M_MODE_RMSK, \
   BIMC_M_MODE_RMSK, \
   (mode) }

#define MPORT_PIPE0_GATHERING(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_PIPE0_GATHERING_ADDR(BIMC_BASE,master), \
   BIMC_M_PIPE0_GATHERING_RMSK, \
   BIMC_M_PIPE0_GATHERING_RMSK, \
   (value) }

#define MPORT_RD_TRACKING_INDEX(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_RD_TRACKING_INDEX_ADDR(BIMC_BASE,master), \
   BIMC_M_RD_TRACKING_INDEX_RMSK, \
   BIMC_M_RD_TRACKING_INDEX_RMSK, \
   (value) }

#define MPORT_WR_TRACKING_INDEX(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_WR_TRACKING_INDEX_ADDR(BIMC_BASE,master), \
   BIMC_M_WR_TRACKING_INDEX_RMSK, \
   BIMC_M_WR_TRACKING_INDEX_RMSK, \
   (value) }

#define MPORT_BKE_ENABLE(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE_ENABLE_ADDR(BIMC_BASE,master), \
   BIMC_M_BKE_ENABLE_RMSK, \
   BIMC_M_BKE_ENABLE_RMSK, \
   (value) }

#define MPORT_PL_OVERRIDE(master, value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_PRIORITYLVL_OVERRIDE_ADDR(BIMC_BASE,master), \
   BIMC_M_PRIORITYLVL_OVERRIDE_RMSK, \
   BIMC_M_PRIORITYLVL_OVERRIDE_RMSK, \
   (value) }

#define MPORT_READ_COMMAND_OVERRIDE(master, value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_READ_COMMAND_OVERRIDE_ADDR(BIMC_BASE,master), \
   BIMC_M_READ_COMMAND_OVERRIDE_RMSK, \
   BIMC_M_READ_COMMAND_OVERRIDE_RMSK, \
   (value) }

#define MPORT_WRITE_COMMAND_OVERRIDE_0(master, value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_WRITE_COMMAND_OVERRIDE_ADDR(BIMC_BASE,master), \
   BIMC_M_WRITE_COMMAND_OVERRIDE_RMSK, \
   BIMC_M_WRITE_COMMAND_OVERRIDE_RMSK, \
   (value) }


#define MPORT_BKE1_ENABLE(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE1_ENABLE_ADDR(BIMC_BASE,master), \
   BIMC_M_BKE1_ENABLE_RMSK, \
   BIMC_M_BKE1_ENABLE_RMSK, \
   (value) }

#define MPORT_BKE2_ENABLE(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE2_ENABLE_ADDR(BIMC_BASE,master), \
   BIMC_M_BKE2_ENABLE_RMSK, \
   BIMC_M_BKE2_ENABLE_RMSK, \
   (value) }

#define MPORT_BKE3_ENABLE(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE3_ENABLE_ADDR((uint8_t *)BIMC_BASE,master), \
   BIMC_M_BKE3_ENABLE_RMSK, \
   BIMC_M_BKE3_ENABLE_RMSK, \
   (value) }

#define MPORT_BKE3_GRANT_PERIOD(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE3_GRANT_PERIOD_ADDR((uint8_t *)BIMC_BASE,master), \
   BIMC_M_BKE3_GRANT_PERIOD_RMSK, \
   BIMC_M_BKE3_GRANT_PERIOD_RMSK, \
   (value) }

#define MPORT_BKE3_GRANT_COUNT(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE3_GRANT_COUNT_ADDR((uint8_t *)BIMC_BASE,master), \
   BIMC_M_BKE3_GRANT_COUNT_RMSK, \
   BIMC_M_BKE3_GRANT_COUNT_RMSK, \
   (value) }

#define MPORT_BKE3_THRESHOLD_MEDIUM(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE3_THRESHOLD_MEDIUM_ADDR((uint8_t *)BIMC_BASE,master), \
   BIMC_M_BKE3_THRESHOLD_MEDIUM_RMSK, \
   BIMC_M_BKE3_THRESHOLD_MEDIUM_RMSK, \
   (value) }

#define MPORT_BKE3_THRESHOLD_LOW(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE3_THRESHOLD_LOW_ADDR((uint8_t *)BIMC_BASE,master), \
   BIMC_M_BKE3_THRESHOLD_LOW_RMSK, \
   BIMC_M_BKE3_THRESHOLD_LOW_RMSK, \
   (value) }

#define MPORT_BKE3_HEALTH_0(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE3_HEALTH_0_ADDR((uint8_t *)BIMC_BASE,master), \
   BIMC_M_BKE3_HEALTH_0_RMSK, \
   BIMC_M_BKE3_HEALTH_0_RMSK, \
   (value) }

/* Master indexes */
#define MASTER_APP     0
#define MASTER_DSP     1
#define MASTER_MCDMA   2
#define MASTER_SYS     3


#define QOS_CTRL(value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_QOS_CTRL_ADDR(BIMC_BASE), \
   BIMC_QOS_CTRL_RMSK, \
   BIMC_QOS_CTRL_RMSK, \
   (value) }

#define QOS_CFG(value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_QOS_CFG_ADDR(BIMC_BASE), \
   BIMC_QOS_CFG_RMSK, \
   BIMC_QOS_CFG_RMSK, \
   (value) }

#define QOS_TIMEOUT_CNT_LOW_URGENCY(value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_ADDR(BIMC_BASE), \
   BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_RMSK, \
   BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_RMSK, \
   (value) }

#define QOS_TIMEOUT_CNT_HIGH_URGENCY(value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_ADDR(BIMC_BASE), \
   BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_RMSK, \
   BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_RMSK, \
   (value) }

#define QOS_FSSH_URGENCY_SEL(value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_QOS_FSSH_URGENCY_SEL_ADDR(BIMC_BASE), \
   BIMC_QOS_FSSH_URGENCY_SEL_RMSK, \
   BIMC_QOS_FSSH_URGENCY_SEL_RMSK, \
   (value) }

#define BIMC_BRIC_MSA_LOCKS_ADDR(b)      (BIMC_GLOBAL1_REG_BASE(b) + 0x0300)
#define BIMC_BRIC_PROTNS_LOCKS_ADDR(b)   (BIMC_GLOBAL1_REG_BASE(b) + 0x0310)
#define BIMC_BRIC_SP_LOCKS_ADDR(b)       (BIMC_GLOBAL1_REG_BASE(b) + 0x0320)

#define BRIC_MSA_LOCKS(value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_BRIC_MSA_LOCKS_ADDR((uint8_t *)BIMC_BASE), \
   (value), \
   0xffffffff, \
   (value) }

#define BRIC_PROTNS_LOCKS(value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_BRIC_PROTNS_LOCKS_ADDR((uint8_t *)BIMC_BASE), \
   (value), \
   0xffffffff, \
   (value) }

#define BRIC_SP_LOCKS(value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_BRIC_SP_LOCKS_ADDR((uint8_t *)BIMC_BASE), \
   (value), \
   0xffffffff, \
   (value) }


/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/*---------------------------------------------------------------------------*/
/*          Properties data for device ID  = "icbcfg/boot"                   */
/*---------------------------------------------------------------------------*/

/* ICBcfg Boot Configuration Data*/

icbcfg_data_type icbcfg_boot_data[] = 
{
    /* Add configuration data using
      ICBCFG_HWIO_*() or
      ICBCFG_RAW_*() macros below
      .
      .                          
      . 
                         */

    
    //BIMC_DEFAULT(0x80020002),
    /* Only map the bottom 256 MB to SNOC */
    //SLAVE_SEGMENT_MASK(SLAVE_SNOC, 0, 0xF0000000),
    
    QOS_CFG(0x606),
    QOS_TIMEOUT_CNT_LOW_URGENCY(0x600060),
    QOS_TIMEOUT_CNT_HIGH_URGENCY(0x270027),
    QOS_FSSH_URGENCY_SEL(0x10),

    MPORT_MODE(MASTER_APP,   0x00000092),
    MPORT_MODE(MASTER_DSP,   0x00000092),
    MPORT_MODE(MASTER_MCDMA, 0x00000092),
    MPORT_MODE(MASTER_SYS,   0xA0000092),

    MPORT_PL_OVERRIDE(MASTER_APP,   0x1),
    MPORT_PL_OVERRIDE(MASTER_DSP, 0x301),
    MPORT_PL_OVERRIDE(MASTER_MCDMA, 0x200),

    MPORT_READ_COMMAND_OVERRIDE(MASTER_APP, 0x40),
    MPORT_READ_COMMAND_OVERRIDE(MASTER_DSP, 0x3000040),
    MPORT_READ_COMMAND_OVERRIDE(MASTER_MCDMA, 0x2000040),

    MPORT_WRITE_COMMAND_OVERRIDE_0(MASTER_APP, 0x40),
    MPORT_WRITE_COMMAND_OVERRIDE_0(MASTER_DSP, 0x3000040),
    MPORT_WRITE_COMMAND_OVERRIDE_0(MASTER_MCDMA, 0x2000040),
    
    MPORT_RD_TRACKING_INDEX(MASTER_APP,   0x43210),
    MPORT_RD_TRACKING_INDEX(MASTER_DSP,   0x43210),
    MPORT_RD_TRACKING_INDEX(MASTER_MCDMA, 0x43210),
    MPORT_RD_TRACKING_INDEX(MASTER_SYS,   0x43210),

    MPORT_WR_TRACKING_INDEX(MASTER_APP,   0x43210),
    MPORT_WR_TRACKING_INDEX(MASTER_DSP,   0x43210),
    MPORT_WR_TRACKING_INDEX(MASTER_MCDMA, 0x43210),
    MPORT_WR_TRACKING_INDEX(MASTER_SYS,   0x43210),

    SWAY_CLOCK_CTRL(SLAVE_SYS_SWAY,0x3F030003),
    SWAY_MAX_OUTSTANDING_REQUESTS(SLAVE_SYS_SWAY, 0xffff),

    //SWAY_INTERRUPT_ENABLE(SLAVE_DEFAULT_SWAY, 0x1),
    
    ARB_MODE(SLAVE_DDR_CH0_ARB, 0x1),
    ARB_MODE(SLAVE_SYS_ARB    , 0x0),
    ARB_MODE(SLAVE_DEFAULT_ARB, 0x0),


};

icbcfg_prop_type icbcfg_boot_prop = 
{
    /* Length of the config  data array */
    ARRAY_SIZE(icbcfg_boot_data),
    /* Pointer to config data array */ 
    icbcfg_boot_data                                    
};

icbcfg_data_type icbcfg_boot_post_data_v1[] = 
{
    //ICBCFG_HWIO_DW(BIMC_BRIC_REDIRECT_MSTR_EN, 0x002E002E),
    //ICBCFG_HWIO_DW(BIMC_BRIC_REDIRECT_CTRL, 0x82000100),

    BRIC_MSA_LOCKS(0x00008000),
    BRIC_PROTNS_LOCKS(0x00008000),
    BRIC_SP_LOCKS(0x00008000),
};

icbcfg_prop_type icbcfg_boot_post_prop_v1 = 
{
    /* Length of the config  data array */
    ARRAY_SIZE(icbcfg_boot_post_data_v1),
    /* Pointer to config data array */ 
    icbcfg_boot_post_data_v1                                    
};


/* DDR map information. */
icbcfg_mem_region_type map_ddr_regions[1] =
{
  { 0x40000000ULL, 0x100000000ULL },
};

uint32 channel_map[1] = { SLAVE_DDR_CH0 };

HAL_bimc_InfoType bimc_hal_info =
{
  (uint8_t *)BIMC_BASE, /* Base address */
  19200,                /* QoS frequency */
  {
    0,
    0,
    0,
    0,
    0,
    0,
    4, /**< Number of segments for address decode. */
  }
};

/* Make sure the config region is always prohibited when "resetting" */
HAL_bimc_SlaveSegmentType safe_reset_seg =
{
  true,
  0x00000000ULL,                 /* start of config region */
  0x40000000ULL,                 /* 256MB */
  BIMC_SEGMENT_TYPE_SUBTRACTIVE,
  BIMC_INTERLEAVE_NONE,
};



icbcfg_device_config_type mdm9205 =
{
  /* Chip version information for this device data. */
  DALCHIPINFO_FAMILY_MDM9205,  /**< Chip family */
  false,                       /**< Exact match for version? */
  0x0,                         /**< Chip version */

  /* Device information. */
  &icbcfg_boot_prop,        /**< ICB_Config_Init() prop data */
  ARRAY_SIZE(channel_map),     /**< Number of DDR channels */
  channel_map,                 /**< Map of array indicies to channel numbers */
  4,                           /**< Number of BRIC segments per slave */
  ARRAY_SIZE(map_ddr_regions), /**< Number of regions in the DDR map */
  map_ddr_regions,             /**< Array of mappable DDR regions */
  &bimc_hal_info,              /**< BIMC HAL info structure */
  &safe_reset_seg,             /**< The segment config to use while reseting segments */
  false,                       /**< Have we entered best effort mode? */
  NULL,                        /**< L2 TCM unmap configuration */
  &icbcfg_boot_post_prop_v1,   /**< ICB_Config_PostInit() prop data */
};



/* Definitions list */
icbcfg_device_config_type *configs[] =
{
  &mdm9205,
};

/* Exported target definitions */
icbcfg_info_type icbcfg_info =
{
  ARRAY_SIZE(configs),
  configs,
};
