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

$Header: //components/rel/core.tx/6.0/debugtrace/hwevent/config/9x25/hwevent_config.c#1 $

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
{0xfc302fb0, 0xC5ACCE55},//Unlock QDSS CSR
{0xfc321fb0, 0xC5ACCE55},//unlock QDSS STM
{0xfc321d64, 0x0}, //Disable STMHEMCR
{0xfc321d20, 0xf33003},//STMHETER
{0xfc321d00, 0xf33003}, //STMHEER
{0xfc321d64, 0x1 },//Enable STMHEMCR
{0xfc302020, 0xb}, //STMEXTHWCTRL0
{0xfc302024, 0xf0f9f0}, //STMEXTHWCTRL1
{0xfc302028, 0x6}, //STMEXTHWCTRL2
{0xfc30202c, 0x90080d00}, //STMEXTHWCTRL3   
{NULL,NULL} /*indicates end of table*/
};


#ifndef HWEVENT_CONFIG_TABLE_SIZE
#define HWEVENT_CONFIG_TABLE_SIZE sizeof(hwevent_config_table)/sizeof(HWEventRegisterValue)
#endif


const HWEventTableSize table_size_array[]={
   {HWEVENT_CONFIG_TABLE_SIZE},
 };

const HWEventAddrRange hwevent_addr_table[]={
{0xf9011038,0xf901103C},//HWIO_APCS_HWEVENT_SEL1,2_ADDR
{0xfc401600,0xfc40167c},//HWIO_GCC_QDSS_STM_EVENTn_CTL_REG_ADDR
{0xfc302fb0,0xfc302fb0},//HWIO_QDSS_CS_QDSSCSR_LOCKACCESS_ADDR
{0xfc321fb0,0xfc321fb0},//HWIO_QDSS_STMLAR_ADDR
{0xfc321d00,0xfc321d00},//HWIO_QDSS_STMHEER_ADDR
{0xfc321d20,0xfc321d20},//HWIO_QDSS_STMHETER_ADDR
{0xfc321d64,0xfc321d64},//HWIO_QDSS_STMHEMCR_ADDR
{0xfc302020,0xfc30202c},//HWIO_QDSS_CS_QDSSCSR_STMEXTHWCTRLn_ADDR
{0xfd4ab160,0xfd4ab1dc},//HWIO_TCSR_PHSS_QDSS_HW_EVENTS_CTRL_n_ADDR
{NULL,NULL}
};

#define HWEVENT_ADDR_RANGE_TABLE_SIZE  sizeof(hwevent_addr_table)/sizeof(HWEventAddrRange)

const HWEventTableSize addr_range_table_size[]={
   {HWEVENT_ADDR_RANGE_TABLE_SIZE}
 };
