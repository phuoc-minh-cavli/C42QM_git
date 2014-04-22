#ifndef _HSUART_SETTINGS_H_
#define _HSUART_SETTINGS_H_
/*===========================================================================

  Header defining UART settings/properties structure and its API

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright 2018 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who        what, where, why
--------   --------   ----------------------------------------------------------
12/04/18   SS         HSUart Enablement
============================================================================*/

#include <Library/BaseLib.h>

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef enum
{
   LEGACY,
   BADGER,
   BEAR,
   HONEYBADGER,
   DRAGONFLY,
} CHIPSET_FAMILY;

typedef struct
{
  CHIPSET_FAMILY family;
  UINT32 platform;
  UINT32 uart_instance;
  UINT32 gsbi_base;
  UINT32 uart_base;
  UINT32 clock_base;
  UINT32 rts_tlmm_base;
  UINT32 rts;
  UINT32 cts_tlmm_base;
  UINT32 cts;
  UINT32 rx_tlmm_base;
  UINT32 rx;
  UINT32 tx_tlmm_base;
  UINT32 tx;
  BOOLEAN enable_loopback;
  UINTN baud_rate;
  BOOLEAN enable_flow_control;
} UART_PROPS;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

UART_PROPS* hsuart_get_properties(void);
#endif   // _HSUART_SETTINGS_H_
