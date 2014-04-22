/*!
  @file
  rflm_dm_modem_defs.h

  @brief
  This header file contains the RFLM DM(data manager) target-specific definitions

*/

/*===========================================================================

  Copyright (c) 2013 - 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfmodem_jolokia.mpss/1.10/api/dm/rflm_dm_modem_defs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/05/19   sk      RF PAGE ONLY changes for NB/CATM
01/28/19   mpa     Changes to support THERM_CHANGES
12/06/18   sk      SDR105 L2L scripts support
10/31/18   sbo     Added 3k buffers
08/21/18   as     Added support for 5MHz DL Hopping in CATM1
03/29/16   mah    Reduce DM Memory FootPrint on IoE
07/21/15   bp     Increase RFLM_TECH_BUF_SIZE_UINT32 from 15360 to 17408 for JO1.1.1+WTR2955  
09/04/14   aw     Created the initial version
===========================================================================*/

#ifndef RFLM_DM_MODEM_DEFS_H
#define RFLM_DM_MODEM_DEFS_H


/* Modem specific DM workarounds */
#include "rflm_dm_modem_workaround.h"

/*===========================================================================
						   CONSTANTS
===========================================================================*/
/* Handle counts*/
#ifndef FEATURE_RF_LTE_PAGE_ONLY
#define RFLM_HANDLE_COUNTS 			            3
#else
#define RFLM_HANDLE_COUNTS 			            2
#endif

/* Max number of scripts for a handle */
#define RFLM_MAX_NUM_HANDLE_SCRIPT_BUF          38

/* Max caller history number for stack trace */
#define RFLM_DM_MAX_CALLER_HISTORY              8 

/* The size in uint32 for tech buffer */
#define RFLM_TECH_BUF_SIZE_UINT32               (22000)

/* The size in uint32 of the DTR RX and TX buffer */
#define RFLM_DM_DTR_RX_BUF_UINT32_SIZE          0x200
#define RFLM_DM_DTR_TX_BUF_UINT32_SIZE          0x200

/* The size in uint32 of the script buffer */
// --- no padding ---
#define RFLM_DM_SBUF_2HB_UINT32_SIZE            50
#define RFLM_DM_SBUF_5HB_UINT32_SIZE            128
#define RFLM_DM_SBUF_1KB_UINT32_SIZE            256
#define RFLM_DM_SBUF_2KB_UINT32_SIZE            512
#define RFLM_DM_SBUF_3KB_UINT32_SIZE            768

/* The number of items in the script buff*/
#define RFLM_SCRIPT_BUF_2HB_COUNTS              12
#define RFLM_SCRIPT_BUF_5HB_COUNTS              50
#ifndef FEATURE_RF_LTE_PAGE_ONLY
#define RFLM_SCRIPT_BUF_1KB_COUNTS              12
#define RFLM_SCRIPT_BUF_2KB_COUNTS              10
#define RFLM_SCRIPT_BUF_3KB_COUNTS              2      
#else
#define RFLM_SCRIPT_BUF_1KB_COUNTS              3
#define RFLM_SCRIPT_BUF_2KB_COUNTS              3
#define RFLM_SCRIPT_BUF_3KB_COUNTS              1   
#endif   

/* DTR buffer counts*/
#define RFLM_DM_DTR_RX_BUF_COUNTS               3 
#define RFLM_DM_DTR_TX_BUF_COUNTS               1

/* Tech buffer counts*/
  #define RFLM_TECH_BUF_POOL_SIZE               1

/* Max event dependency number */
#define RFLM_DM_MAX_DEPENDENCY_NUM_PER_EVENT    1 

/* Request size ranges */
typedef enum
{
  BUFF_SIZE_IN_BYTE_2H = 200,
  BUFF_SIZE_IN_BYTE_5H = 512,
  BUFF_SIZE_IN_BYTE_1K = 1024,
  BUFF_SIZE_IN_BYTE_2K = 2048,
  BUFF_SIZE_IN_BYTE_3K = 3072
} rflm_dm_buff_size_e;


#endif /* RFLM_DM_MODEM_DEFS_H */
