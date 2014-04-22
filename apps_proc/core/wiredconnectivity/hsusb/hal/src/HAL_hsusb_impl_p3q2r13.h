#ifndef HAL_HSUSB_impl_p3q2r13
#define HAL_HSUSB_impl_p3q2r13
/*
===========================================================================

FILE:         HAL_hsusb_impl_p3q2r13.h

DESCRIPTION:  
This is the external interface for the HS-USB core usb_otg_hs_xp3q2r13 implementation.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/hal/src/HAL_hsusb_impl_p3q2r13.h#1 $

===========================================================================

===========================================================================
Copyright © 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INTERFACES
** ----------------------------------------------------------------------- */

#include "HAL_hsusb_impl_3r0.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* Qualcomm registers */
#define HAL_HSUSB_GEN_CONFIG_DMA_HPROT_CTRL_BMSK   0x1000
#define HAL_HSUSB_GEN_CONFIG_DMA_HPROT_CTRL_SHFT      0xc
#define HAL_HSUSB_GEN_CONFIG_USB_BAM_DISABLE_BMSK  0x2000
#define HAL_HSUSB_GEN_CONFIG_USB_BAM_DISABLE_SHFT     0xd
#define HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_4_0_BMSK  0x4000
#define HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_4_0_SHFT     10

#define HAL_HSUSB_ENDPT_PIPE_ID_ADDR(n)   (0x000001fc + 4 * (n))  /* 1<=n<=15 */
#define HAL_HSUSB_ENDPT_PIPE_ID_TX_BMSK                 0x1f0000
#define HAL_HSUSB_ENDPT_PIPE_ID_TX_SHFT                     0x10
#define HAL_HSUSB_ENDPT_PIPE_ID_RX_BMSK                     0x1f
#define HAL_HSUSB_ENDPT_PIPE_ID_RX_SHFT                        0

/* -----------------------------------------------------------------------
**                           INTERFACES DESCRIPTION
** ----------------------------------------------------------------------- */

/* 
* C++ wrapper
*/
#ifdef __cplusplus
extern "C" {
#endif

void HAL_hsusb_ConstructImplp3q2r13(struct HAL_HSUSB_CoreIfType* this);

#ifdef __cplusplus
}
#endif

#endif /* HAL_HSUSB_impl_p3q2r13 */
