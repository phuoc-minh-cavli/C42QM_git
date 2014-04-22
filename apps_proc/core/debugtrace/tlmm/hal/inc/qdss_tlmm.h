#ifndef QDSS_TLMM_H
#define QDSS_TLMM_H

/*=============================================================================

FILE:         qdss_tlmm.h

DESCRIPTION:  QDSS interface to access:
              TLMM (top-level mode multiplexor modules),
              PMIC (power management integrated circuit), and
              SPMI-GPIO (general purpose input/output via
                         system power mgmt interface ).

================================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/

#include "DALStdDef.h"

int qdss_tlmm_sd_enable(void);
int qdss_tlmm_sd_disable(void);
int qdss_tlmm_config_cti_gpio(uint8 option);


#endif //QDSS_TLMM_H

