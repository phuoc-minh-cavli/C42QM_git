/**************************************************************************
 * FILE: devprg_usb.h
 *
 * USB abstraction layer for TX and RX. Includes VIP.
 *
 * Copyright (c) 2016, 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/DevPrgLib/devprg_usb.h#2 $
  $DateTime: 2019/06/19 22:23:08 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2019-05-24   as    Added support for devprg over HSUART.
2016-11-07   wek   Create.

===========================================================================*/

#ifndef __DEVPRG_USB_H__
#define __DEVPRG_USB_H__

#include "comdef.h"
#include "devprg_transport.h"


/* Initializes USB and returns the function pointers for USB TX/RX. */
struct devprg_transport *devprg_transport_usb_init(void);

#endif /* __DEVPRG_USB_H__ */
