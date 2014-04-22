#ifndef HWIOQDSS_CTI_H
#define HWUIQDSS_CTI_H

/****************************************************************************
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 *
 * FILE:            core/debugtrace/cti/hal/inc/hwioqdss_cti.h
 *
 * DESCRIPTION:     Minimum set of HWIO definitions for CTI.
 *
 *
 * EDIT HISTORY FOR MODULE
 * --------------------------------------------------------------------------
 *
 * when         who         what, where, why
 * ----------   -------     -----------------
 * 2014-01-24   rpoddar     Initial revision.
 *
 ****************************************************************************/

/****************************************************************************
 * Include Files
 ****************************************************************************/

#include "HALhwio.h"

/****************************************************************************
 * HWIO Definitions
 ****************************************************************************/

/* Can be the base address of any CTI block */
extern volatile uint32 QDSS_CTI_BLOCK_BASE;

/* Lock Access Register */
#define HWIO_QDSS_CTI_LAR_ADDR          (QDSS_CTI_BLOCK_BASE + 0xFB0)
#define HWIO_QDSS_CTI_LAR_IN            in_dword(HWIO_QDSS_CTI_LAR_ADDR)
#define HWIO_QDSS_CTI_LAR_OUT(v)        out_dword(HWIO_QDSS_CTI_LAR_ADDR, (v)) 

/* Control register */
#define HWIO_QDSS_CTI_CONTROL_ADDR      (QDSS_CTI_BLOCK_BASE + 0x000) 
#define HWIO_QDSS_CTI_CONTROL_IN        in_dword(HWIO_QDSS_CTI_CONTROL_ADDR)
#define HWIO_QDSS_CTI_CONTROL_OUT(v)    out_dword(HWIO_QDSS_CTI_CONTROL_ADDR, (v))

/* Appset register */
#define HWIO_QDSS_CTI_APPSET_ADDR      (QDSS_CTI_BLOCK_BASE + 0x014) 
#define HWIO_QDSS_CTI_APPSET_IN        in_dword(HWIO_QDSS_CTI_APPSET_ADDR)
#define HWIO_QDSS_CTI_APPSET_OUT(v)    out_dword(HWIO_QDSS_CTI_APPSET_ADDR, (v))


/* Appclear register */
#define HWIO_QDSS_CTI_APPCLEAR_ADDR      (QDSS_CTI_BLOCK_BASE + 0x018) 
#define HWIO_QDSS_CTI_APPCLEAR_IN        in_dword(HWIO_QDSS_CTI_APPCLEAR_ADDR)
#define HWIO_QDSS_CTI_APPCLEAR_OUT(v)    out_dword(HWIO_QDSS_CTI_APPCLEAR_ADDR, (v))

/* Apppulse register */
#define HWIO_QDSS_CTI_APPPULSE_ADDR      (QDSS_CTI_BLOCK_BASE + 0x01C) 
#define HWIO_QDSS_CTI_APPPULSE_IN        in_dword(HWIO_QDSS_CTI_APPPULSE_ADDR)
#define HWIO_QDSS_CTI_APPPULSE_OUT(v)    out_dword(HWIO_QDSS_CTI_APPPULSE_ADDR, (v))

/* 
 * Input trigger registers 
 *
 * There is one CTIINEN register per trigger, so use the trigger number
 * as an offset to the base CTIINEN[0] register.
 */ 
#define HWIO_QDSS_CTI_INENn_ADDR(n)     (QDSS_CTI_BLOCK_BASE + 0x020 + ((n) << 2))
#define HWIO_QDSS_CTI_INENn_INI(n)      in_dword(HWIO_QDSS_CTI_INENn_ADDR((n)))
#define HWIO_QDSS_CTI_INENn_OUTI(n, v)  out_dword(HWIO_QDSS_CTI_INENn_ADDR((n)), (v))

/* 
 * Output trigger registers 
 *
 * There is one CTIOUTNEN register per trigger, so use the trigger number
 * as an offset to the base CTIOUTEN[0] register.
 */ 
#define HWIO_QDSS_CTI_OUTENn_ADDR(n)     (QDSS_CTI_BLOCK_BASE + 0x0A0 + ((n) << 2))
#define HWIO_QDSS_CTI_OUTENn_INI(n)      in_dword(HWIO_QDSS_CTI_OUTENn_ADDR((n)))
#define HWIO_QDSS_CTI_OUTENn_OUTI(n, v)  out_dword(HWIO_QDSS_CTI_OUTENn_ADDR((n)), (v))

#endif /* HWIOQDSS_CTI_H */
