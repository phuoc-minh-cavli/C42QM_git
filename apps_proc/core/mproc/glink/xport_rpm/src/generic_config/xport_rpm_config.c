/*===========================================================================

            GLink RPM transport 9x55 APSS Configuration Structures

=============================================================================

  @file
    xport_rpm_config.c

    Contains structures to be used in Glink RPM trasnport configuration.

  Copyright (c) 2014-2015,2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/13/15   db      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "xport_rpm_config.h"
#include "msmhwiobase.h"
#include "smem_type.h"

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/

#define XPORT_RPM_MSG_RAM_BASE        RPM_SS_MSG_RAM_START_ADDRESS_BASE
#define XPORT_RPM_MSG_RAM_SIZE        RPM_SS_MSG_RAM_START_ADDRESS_BASE_SIZE

/** Size of Glink RPM transport ToC in MSG RAM */
#define XPORT_RPM_MSG_TOC_SIZE        0x100

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/
const xport_rpm_config_type xport_rpm_config[] =
{
  /* sparrow->Rpm */
  {
    "rpm",                /* Remote host name        */
    XPORT_RPM_AP2R_ID,    /* Tx FIFO ID in RPM ToC   */
    XPORT_RPM_R2AP_ID,    /* Rx FIFO ID in RPM ToC   */
    {                     /* Outgoing interrupt      */
      DALIPCINT_PROC_RPM,
      DALIPCINT_GP_0
    },
    200                    /* Incoming interrupt. */
  }
  
};

const uint32 xport_rpm_config_num = ARRAY_LENGTH(xport_rpm_config);
const char* xport_rpm_msg_ram = (char*)XPORT_RPM_MSG_RAM_BASE;
const uint32* xport_rpm_msg_ram_toc = (uint32*)(XPORT_RPM_MSG_RAM_BASE + XPORT_RPM_MSG_RAM_SIZE - XPORT_RPM_MSG_TOC_SIZE);

/*===========================================================================
FUNCTION      xport_rpm_get_config
===========================================================================*/
/**

  Provides a pointer to transport config strucutre.

  @param[in]  ind    Index of the config

  @return     Pointer to transport config strucutre.

  @sideeffects  None.
*/
/*=========================================================================*/
const xport_rpm_config_type* xport_rpm_get_config(uint32 ind)
{
  if (ind >= xport_rpm_config_num)
  {
    return NULL;
  }

  return &xport_rpm_config[ind];
}

