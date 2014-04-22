/*===========================================================================
   FILE:         HALmpmintPlatform.c

   Reference chip release: MDM9x07 (Lykan) lykan_interrupts_unlinked_v3.4
 
   DESCRIPTION:   This is the platform hardware abstraction layer implementation for the
                  MPM interrupt controller block.
                  This platform is for the RPM on 8916.

                 Copyright © 2012-2020 QUALCOMM Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.

  =============================================================================
  =============================================================================
$Header: //components/rel/core.mpss/3.10/power/mpm/config/asic/9205/MPMIntDevCfgData.c#3 $
$DateTime: 2020/03/08 22:38:32 $
$Author: pwbldsvc $
========================================================================*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "vmpm_internal.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

/**
 * Table mapping the MPM Pin number to the GPIO and/or IRQ pin 
 * number per the IP Catalog. 
 */
mpm_int_pin_mapping_type devcfg_MpmInterruptPinNum_Mapping[] =
{
  /* Trigger                 Local Pin#         Pin Type   */
  /* -------                 -------------      --------   */
  /*0*/
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  5               , MPM_GPIO },
  /*5*/                                                  
  { MPM_TRIGGER_RISING_EDGE,  0               , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  2               , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  9               , MPM_GPIO },
  /*10*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  13              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  1               , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  20              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  21              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  22              , MPM_GPIO },
  /*15*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  50              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  52              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  28              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  51              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  26              , MPM_GPIO },
/*20*/                                                   
  { MPM_TRIGGER_RISING_EDGE,  4               , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  47              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  6               , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  8               , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  14              , MPM_GPIO },
  /*25*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  19              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  27              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  30              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  36              , MPM_GPIO },
  { MPM_TRIGGER_RISING_EDGE,  46              , MPM_GPIO },
  /*30*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  /*35*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  /*40*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  /*45*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  /*50*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  49              , MPM_GPIO }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  /*55*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },
  /*60*/                                                 
  { MPM_TRIGGER_RISING_EDGE,  89              , MPM_IRQ  }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN }, 
  { MPM_TRIGGER_RISING_EDGE,  75              , MPM_IRQ  }, 
  { MPM_TRIGGER_RISING_EDGE,  MPM_UNMAPPED_INT, MPM_OPEN },  
  /*64 - End of the table entry */                       
  { MPM_TRIGGER_LEVEL_HIGH, MPM_UNMAPPED_INT  , MPM_EOT  },
};
