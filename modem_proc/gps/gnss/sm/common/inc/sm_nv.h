/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Session Manager NV Items Interface Header File

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for 
  the SM Non-Volatile Memory interface.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2005, 2006 - 2011, 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Copyright (c) 2013-2014 QUALCOMM Atheros, Inc.
All Rights Reserved.
QCA Proprietary and Confidential.
 
Copyright (c) 2015-2016 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/common/inc/sm_nv.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $ 

=============================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/16   ss     Using OEM NV Bit 15 to configure Cell CPI Fix for Geofencing
10/26/16   nl      SET/ GET mismatch fix for nmea_ext_sentence_mask
03/03/16   gk      Added support for SUPL 2.0.2
03/15/15   rh      Added support for MultiSIM NV API  
11/04/14    ah     Support SUPL TD-SCDMA LocID Config through OEM Feature Mask NV
10/27/14    gk     NV item for terminating SUPL after final fix 
04/17/14   skm     NV Async Read changes 
01/31/14    ss     Supporting Max acceptable GF PUNC confguration from QMI
10/08/13    ah     Moved Tunnel Detection enablement bit to resolve bit conflict
05/21/10    gk     RC thresolds were set incorrectly from defaults
04/13/10   atien   NV_AAGPS_POSITIONING_MODES_SUPPORTED_V support all except GSM NMR
08/22/08   jlp     Added support of emergency call simulation for simulating
                   E911 A-GPS
09/14/07    gk     NMEA CONFIG set parameters support
04/17/07    lt     Changed default aagps_positioning_modes_supported to "all".
04/12/07    ah     Added default read values for all SM-related NV items.
11/14/06    ah     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef SM_NV_H
#define SM_NV_H

/*
 * Include files
*/

#include "comdef.h"

#include "aries_os_api.h"
#include "aries_os_globals.h"

#include "nv.h"
#include "pdapi_lite.h"
#include "queue.h"

#include "custcgps.h"
#include "sys.h"

/*
 * Constant definitions
*/
#define SM_NV_CLIENT_HANDLE_MAX      5
#define SM_NV_CLIENT_HANDLE_ERROR    0xFF

/* SM may define lower max-subs configuration if necessary in order to save NV cache memory (default 3 subs) */
#define SM_CONFIG_MIN_SUB_ID    SYS_MODEM_AS_ID_1
#define SM_CONFIG_MAX_SUB_ID    SYS_MODEM_AS_ID_MAX
#define SM_CONFIG_MAX_SUBS     (SM_CONFIG_MAX_SUB_ID+1)  /* max number of subscriptons */


/* current Subs ID for carrier-specific legacy/EFS NV, default is 0 (subscription independent)
   This value must NOT be -1, should always be valid 0~1 or 0~2 (even if phone is OOS)
   For run-time value sm_nv_max_sub, if it's single SIM, value should be 0, otherwise 1 or 2 (DSDS/TSTS) */
extern sys_modem_as_id_e_type sm_nv_current_sub;
//  TODO [an][del]  if sub is being removed, we can remove this as well,   Check OEM here

/*
 * Structure definitions
*/

/* Type for SM NV write command callback function
 *
 * A client that wants to be notified of the result of a SM NV write
 * command needs to supply a pointer to a function of this type as the
 * only parameter to the sm_nv_init() function.
 */
typedef void (sm_nv_cmd_cb_f_type) (

  /* NV item that this write command status is for */
  nv_items_enum_type    item,

  /* Pointer to SM NV module provided write data */
  void*                 data,

  /* Command status code indicating write pass/fail */
  nv_stat_enum_type     status
);

typedef struct
{
  q_link_type           link;
  nv_items_enum_type    item;
  nv_item_type          data;
  uint32                handle;
  nv_func_enum_type     cmd;
} sm_nv_item_s_type;

typedef struct
{
  boolean               active;
  uint32                thread_id;
  sm_nv_cmd_cb_f_type*  nv_write_cb_fn;
  sm_nv_cmd_cb_f_type*  nv_read_cb_fn;
} sm_nv_clnt_s_type;


/*
 * Function definitions
*/

/*===========================================================================

FUNCTION sm_nv_rdrw_reg

DESCRIPTION
  This function is used to register a client that will use this NV module.
  It will register the client's callback function after an Async Read/NV Write
  command has completed, and return a client handle that is needed for future
  read/writes.
  
RETURN VALUE
  Client handle value (or 0xFF if error)

DEPENDENCIES
 
===========================================================================*/

uint32 sm_nv_rdrw_reg (uint32 thread_id, sm_nv_cmd_cb_f_type nv_clnt_write_cb_fn,
                       sm_nv_cmd_cb_f_type nv_clnt_read_cb_fn);

/*===========================================================================

FUNCTION sm_nv_write

DESCRIPTION
  This function is used to queue NV Write commands to NV. The writes to NV
  are performed in asynchronous fashion.

RETURN VALUE
  TRUE  - NV write command successfully sent to NV
  FALSE - NV write command failed to be sent to NV

DEPENDENCIES
  SM NV write callback needs to be registered via call to sm_nv_write_reg

===========================================================================*/

boolean sm_nv_write (nv_items_enum_type nv_item, const nv_item_type* item_ptr, uint32 handle);

/*===========================================================================

FUNCTION sm_nv_read

DESCRIPTION
  This function is used to perform a synchronous read from NV.
  GPS SHALL CALL THIS ONLY DURING BOOTUP INITIALIZATION

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/

boolean sm_nv_read (nv_items_enum_type nv_item, void* item_ptr);

/*===========================================================================

FUNCTION sm_nv_async_read

DESCRIPTION
  This function is used to queue NV Read commands to NV. The Reads to NV
  via this function are performed in asynchronous fashion.

RETURN VALUE
  TRUE  - NV Async Read command successfully sent to NV
  FALSE - NV Async Read failed to be sent to NV

DEPENDENCIES
  SM NV read callback needs to be registered via call to sm_nv_async_read_reg

===========================================================================*/

boolean sm_nv_async_read (nv_items_enum_type nv_item, const nv_item_type* item_ptr, uint32 handle);

/*===========================================================================

FUNCTION   sm_nv_rdrw_done

DESCRIPTION
  This function is used to handle NV write/read callback status. If there are any
  clients who issued write/async-read requests, they will be notified and any pending
  async-read/write requests will be subsequently handled.

RETURN VALUE
  TRUE  - NV write/read success
  FALSE - NV write/read failure

DEPENDENCIES
 
===========================================================================*/

boolean sm_nv_rdrw_done (void);
/*===========================================================================

FUNCTION sm_nv_init

DESCRIPTION
  This function is used to initialize internal SM-NV data structure.
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/

void sm_nv_init (void);

/*===========================================================================

FUNCTION sm_nv_read_ex

DESCRIPTION
  This function is used to perform a read from cache. And if cached value not present, read from Hw.
  This function is used to perform a read from cache. And if cached value not present, read from Hw.
  GPS SHALL CALL THIS API INSTEAD OF sm_nv_read AFTER BOOT UP INITIALIZATION

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_read_ex (nv_items_enum_type nv_item, void* item_ptr);

/*===========================================================================

FUNCTION sm_nv_write_to_sub 
 
DESCRIPTION
  This function is used to queue NV Write commands to a subscription-specific NV.
  The writes to NV are performed in asynchronous fashion.

RETURN VALUE
  TRUE  - NV write command successfully sent to NV
  FALSE - NV write command failed to be sent to NV

DEPENDENCIES
  SM NV write callback needs to be registered via call to sm_nv_write_reg

===========================================================================*/

boolean sm_nv_write_to_sub (nv_items_enum_type nv_item, const nv_item_type* item_ptr, 
                            uint32 handle);

/*===========================================================================

FUNCTION sm_nv_read_from_sub

DESCRIPTION
  This function is used to perform a synchronous read from a subscription-specific NV.
  GPS SHALL CALL THIS ONLY DURING BOOTUP INITIALIZATION

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/

boolean sm_nv_read_from_sub (nv_items_enum_type nv_item, void* item_ptr, sys_modem_as_id_e_type sub);


/*===========================================================================

FUNCTION sm_nv_read_default

DESCRIPTION
  This function is used to return the default value for the NV item to be
  read. This should be used only when failing to read from NV.

RETURN VALUE
  None

DEPENDENCIES
 
===========================================================================*/

void sm_nv_read_default (nv_items_enum_type nv_item, void *item_ptr);

#endif /* SM_NV_H */
