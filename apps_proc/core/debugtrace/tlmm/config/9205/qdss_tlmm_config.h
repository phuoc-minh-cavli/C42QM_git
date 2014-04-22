#ifndef QDSS_TLMM_CONFIG_H
#define QDSS_TLMM_CONFIG_H

/*=============================================================================

FILE:         qdss_tlmm_config.h

DESCRIPTION:

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/tlmm/config/9205/qdss_tlmm_config.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
=============================================================================*/

#define QDSS_TLMM_BASE_PHYS              (0x0119E000)
#define QDSS_TLMM_OFFSET                 (0)
#define QDSS_TLMM_SIZE                   4096

#define QDSS_TLMM_ETM_MODE_ON_VAL               0x00000001
#define QDSS_TLMM_SDC2_HDRV_PULL_CTL_ON_VAL     0x0000016D

#define QDSS_SPMI_GPIO4_MODE_CTL_ADDR       0x0C340
#define QDSS_SPMI_GPIO4_MODE_CTL_POR_VAL    0
#define QDSS_SPMI_GPIO4_MODE_CTL_SD_VAL     0x11


#include "DDITlmm.h"

struct QDSS_CTI_GPIO
{
   uint8 num;
   uint8 function;
   DALGpioDirectionType direction;
};

struct QDSS_CTI_GPIO qdss_cti_gpios[] = {
   {62, 1, DAL_GPIO_OUTPUT},  /* Option 0 : qdss_cti_trig1_out_a  */
   {62, 2, DAL_GPIO_INPUT},   /* Option 1 : qdss_cti_trig2_in_b  */
   {8,  6, DAL_GPIO_OUTPUT},  /* Option 2 : qdss_cti_trig1_out_b  */
   {9,  6, DAL_GPIO_INPUT},   /* Option 3 : qdss_cti_trig1_in_b  */
   {52, 2, DAL_GPIO_OUTPUT},  /* Option 4 : qdss_cti_trig2_out_c  */
   {53, 2, DAL_GPIO_INPUT},   /* Option 5 : qdss_cti_trig2_in_c  */
   {63, 1, DAL_GPIO_INPUT},   /* Option 6 : qdss_cti_trig1_in_a  */
   {63, 2, DAL_GPIO_OUTPUT},  /* Option 7 : qdss_cti_trig2_out_b  */
   {67, 1, DAL_GPIO_OUTPUT},  /* Option 8 : qdss_cti_trig2_out_a  */
   {70, 1, DAL_GPIO_INPUT},   /* Option 9 : qdss_cti_trig2_in_a  */
   {92, 3, DAL_GPIO_OUTPUT},  /* Option 10 : qdss_cti_trig1_out_c  */
   {93, 3, DAL_GPIO_INPUT}    /* Option 11 : qdss_cti_trig1_in_c  */
};
 
uint8 qdss_cti_gpios_len = sizeof(qdss_cti_gpios) / 
                                sizeof(struct QDSS_CTI_GPIO);

#endif //QDSS_TLMM_CONFIG_H

