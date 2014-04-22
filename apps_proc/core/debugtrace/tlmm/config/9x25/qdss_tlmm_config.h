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
  $Header: //components/rel/core.tx/6.0/debugtrace/tlmm/config/9x25/qdss_tlmm_config.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
=============================================================================*/


#define QDSS_TLMM_BASE_PHYS              (0xFD500000)
#define QDSS_TLMM_OFFSET                 (0x00012000)
#define QDSS_TLMM_SIZE                   4096

#define QDSS_TLMM_ETM_MODE_ON_VAL               0x00000001
#define QDSS_TLMM_SDC2_HDRV_PULL_CTL_ON_VAL     0x0000016D

#define QDSS_SPMI_GPIO4_MODE_CTL_ADDR       0x0C340
#define QDSS_SPMI_GPIO4_MODE_CTL_POR_VAL    0
#define QDSS_SPMI_GPIO4_MODE_CTL_SD_VAL     0x11

#endif //QDSS_TLMM_CONFIG_H

