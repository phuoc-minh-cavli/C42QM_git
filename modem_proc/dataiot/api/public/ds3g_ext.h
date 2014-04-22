#ifndef DS3G_EXT_H
#define DS3G_EXT_H

/*===========================================================================
                      DS3G_EXT.H
DESCRIPTION
This file exposes 3GPP APIs to external clients outside DS.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC ENUM/STRUCT DECLARATIONS

===========================================================================*/

typedef enum
{
  DS3G_TX_POWER_BIN_1,                        /* x <= 0dBm */ 
  DS3G_TX_POWER_BIN_2,                        /* 0dBm < x <= 8dBm */ 
  DS3G_TX_POWER_BIN_3,                        /* 8dBm < x <= 15dBm */ 
  DS3G_TX_POWER_BIN_4,                        /* 15dBm < x <= 20dBm */ 
  DS3G_TX_POWER_BIN_5,                        /* x > 20dBm */ 
  DS3G_TX_POWER_BIN_MAX                       /*  MAX */ 
}ds3g_tx_power_bin_e_type;


#endif /* DS3G_EXT_H */
