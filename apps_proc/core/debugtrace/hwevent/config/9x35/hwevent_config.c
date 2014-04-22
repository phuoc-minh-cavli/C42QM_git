/*==============================================================================

  HW Event Configuration

DESCRIPTION
  This file has port configurations for trace funnels for a given chipset.

REFERENCES

        Copyright © 2011 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtrace/hwevent/config/9x35/hwevent_config.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2/7/2011   prasads  First draft created. 
===========================================================================*/
#include "com_dtypes.h"

typedef struct HWEventRegisterValue_t HWEventRegisterValue;
typedef struct HWEventAddrRange_t HWEventAddrRange;

/*
This structure defines the topology of a funnel configuration.
*/
struct HWEventRegisterValue_t{
   uint32 reg_phys_addr;
   uint32 reg_value;
};

struct HWEventAddrRange_t
{
   uint32 lower_range;
   uint32 higher_range;
   uint32 unlock_value;   
};

typedef struct HWEventTableSize_t HWEventTableSize;
struct HWEventTableSize_t {
   uint32 size;
};


const HWEventRegisterValue hwevent_config_table[]={
/**
0: snoc_obs_mainFault_int
13: RPM-bite
10: MSSQ6-bite
11: LPASSQ6-bite
26: summary_timeout_slave_irq_1
28: rpm-bark
31: MSSQ6-bark
 **/
{NULL,NULL} /*indicates end of table*/
};


#ifndef HWEVENT_CONFIG_TABLE_SIZE
#define HWEVENT_CONFIG_TABLE_SIZE sizeof(hwevent_config_table)/sizeof(HWEventRegisterValue)
#endif


const HWEventTableSize table_size_array[]={
   {HWEVENT_CONFIG_TABLE_SIZE},
 };

const HWEventAddrRange hwevent_addr_table[]={
{0xF9011038,0xF90110B8,0x0},//HWIO_APCS_HWEVENT_SEL1,2_ADDR
{0xFC401600,0xFC40167C,0x0},//HWIO_GCC_QDSS_STM_EVENTn_CTL_REG_ADDR
{0xFC301020,0xFC30102C,'U'},//HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRLn_ADDR
{0xFD4AB160,0xFD4AB1E0,0x0},//HWIO_TCSR_PHSS_QDSS_HW_EVENTS_CTRL_n_ADDR
{0xFC520058,0xFC5200D8,0x0},//PCIE_PCIE20_PARF_HW_EVn
{0xFC520000,0xFC520000,0x0},//PCIE_PCIE20_PARF_SYS_CTRL
{NULL,NULL,NULL}
};

#define HWEVENT_ADDR_RANGE_TABLE_SIZE  sizeof(hwevent_addr_table)/sizeof(HWEventAddrRange)

const HWEventTableSize addr_range_table_size[]={
   {HWEVENT_ADDR_RANGE_TABLE_SIZE}
 };
