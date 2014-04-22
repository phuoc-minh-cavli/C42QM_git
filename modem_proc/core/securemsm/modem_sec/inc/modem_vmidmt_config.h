#ifndef MODEM_VMIDMT_CONFIG_H
#define MODEM_VMIDMT_CONFIG_H

/**
* @file modem_vmidmt_config.h
* @brief Modem VMIDMT pipe configuration file.
*
* This file implements vmidmt pipe configuration for modem signal generation.
*
*/
/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.10/securemsm/modem_sec/inc/modem_vmidmt_config.h#2 $
  $DateTime: 2019/01/30 04:57:31 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
01/03/2018        Initial version
===========================================================================*/

/** Status macros for vmidmt config invocation */
#define VMIDMT_INIT_NOT_INVOKED        0
#define VMIDMT_INIT_START              1
#define VMIDMT_COUNT_FAIL              2
#define VMIDMT_BASE_ADDR_MATCH_FAIL    3
#define VMIDMT_PT_MAP_FAIL             4
#define VMIDMT_PT_UNMAP_FAIL           5
#define VMIDMT_BASE_INVALID_SID_INDEX  6
#define VMIDMT_READ_WRITE_MISMATCH     7
#define VMIDMT_INIT_END                8
#define VMIDMT_DEVICE_ATTACH_FAILED    9

#define VMIDMT_MSDR_OFFSET      0x480
#define VMIDMT_IDR5_OFFSET       0x34
#define VMIDMT_ADDR_TYPE(a) HWIO_ADDR(a)
typedef enum{
    HAL_VMIDMT_BAM_BLSP1_DMA = 0,   /**< BAM BLSP DMA*/
    HAL_VMIDMT_CRYPTO0_BAM   = 1,   /**< CRYPTO BAM */
    HAL_VMIDMT_IPA           = 2,   /**< IPA */  
    HAL_VMIDMT_RPM_MSGRAM    = 3,   /**< RPM MSGRAM */
    HAL_VMIDMT_SPMI          = 4,
    HAL_VMIDMT_QPIC_BAM      = 5,   /**< QPIC BAM */
	HAL_VMIDMT_SSC_QUPV3     = 6,
	HAL_VMIDMT_SSC_SDC       = 7,
	HAL_VMIDMT_AOP           = 8,
    HAL_VMIDMT_QUPV3_0       = 9,
	HAL_VMIDMT_QUPV3_1       = 10,
	HAL_VMIDMT_QUPV3_2       = 11,
	HAL_VMIDMT_PCNOC_SNOC2   = 12,
    HAL_VMIDMT_COUNT,
    HAL_VMIDMT_SIZE = 0x7FFFFFFF     /* force to 32-bit enum */
}HAL_vmidmt_InstanceType;   

typedef struct{
	HAL_vmidmt_InstanceType vmidmt;
	char *uMapAddress;
	uint32 uBaseAddress;
}HAL_vmidmt_VmidmtBaseAddressInfoType;

void modem_qpic_vmidmt_configure();
/**
* @ param vmidmtInstance -- vmidmt to configure pipe
* @ param pipe           -- pipe for which MSA signal needs to be generated
* @return E_SUCCESS in case of success else E_FAILURE
*
*/
int modem_vmidmt_configure(HAL_vmidmt_InstanceType vmidmtInstance, uint32 sidIndex);

#endif
