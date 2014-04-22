/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  Determines error action and configures TZ for it.

Copyright (c) 1992-2017 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.tx/6.0/debugtools/err/src/err_action.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/15   din     Removed dead code and updated file.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "erri.h"
#include <stdio.h>
#include "err_nv_efs.h"
#include "tms_dload.h"
#include "sys_m_internal.h"
#include "tz_syscalls.h"
#include "scm.h"
#include "msg.h"

/*===========================================================================

                      Prototypes for External functions

===========================================================================*/

extern err_action_type sys_m_get_action(void);
extern void err_nv_efs_init(void);

/*===========================================================================

                      Prototypes for internal functions

===========================================================================*/

static void err_call_tz_set_err_action(uint32 action);
void err_action_init(void);


/*===========================================================================

                 Defines and variable declarations for module

===========================================================================*/

/* The default is used by the err_emergency_error_recovery function
 * if it is called before NV values have been read  */
static err_action_type err_auto_action = ERR_DEFAULT_ACTION;

/*=========================================================================

FUNCTION ERR_CALL_TZ_SET_NO_RESET_ERR_ACTION
DESCRIPTION
  Calls into TZ to set no reset error action. Called from err_init.
===========================================================================*/
static void err_call_tz_set_err_action(uint32 action)
{
  /* TZ SCM Call */
  #define TZBSP_OEM_SET_RESET_TYPE 03
  #define TZBSP_SVC_OEM 254
	
  uint32 result = 1;
  struct	 	 
  {	 	 
    uint32   err_fatal;	 	 
  } oem_err_fatal_cmd_buffer; 

  oem_err_fatal_cmd_buffer.err_fatal = action;
 
  /* Syscall for OEM Error Handling Request */
  /* No STM call is used deliberately since this is called from err_init */
  (void)scm_call(TZBSP_SVC_OEM, TZBSP_OEM_SET_RESET_TYPE, 
       &oem_err_fatal_cmd_buffer, sizeof(oem_err_fatal_cmd_buffer),
       &result, sizeof(result));

} /* err_call_tz_set_err_action */



/*===========================================================================

FUNCTION err_set_auto_action

DESCRIPTION
  Set the action to be taken when an error is encountered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void err_set_auto_action(err_action_type action)
{
  #define TZBSP_RESET_ON_ERR   0x0
  #define TZBSP_WAIT_ON_ERR    0x1 

  /* Set default action */
  err_call_tz_set_err_action(TZBSP_RESET_ON_ERR);

  if(action >= SIZEOF_ERR_ACTION)
  {
    MSG_1(MSG_SSID_TMS, MSG_LEGACY_MED,
	 "ERR: NV value %u for ERR_FATAL_ACTION exceeds enum", action);
    return;
  }

  err_auto_action = action;

  if ( err_auto_action == ERR_NO_ACTION )
  {
    /* Set No reset configuration in TZ */
    err_call_tz_set_err_action(TZBSP_WAIT_ON_ERR);
  }

  return;

} /* err_set_auto_action */

/*===========================================================================

FUNCTION err_get_auto_action

DESCRIPTION
  Gets the action to be taken when an error is encountered.

DEPENDENCIES
  None

RETURN VALUE
  action

SIDE EFFECTS
  None

===========================================================================*/

err_action_type err_get_auto_action(void)
{
  return err_auto_action;
}


void err_action_init(void)
{
  uint8 u8_buff;

  /* Initialize err_nv_efs structures */
  err_nv_efs_init();

  /* Override default and configurable action based on System Monitor */
  if ( sys_m_get_action() == ERR_DLOAD )
  {
    err_set_auto_action(ERR_DLOAD);
    tms_dload_write_magic_numbers();
    return;
  }

  /* Get value for err fatal action (read will return default if NV not set) */
  if (err_nv_efs_reg_item_read(DNT_NV_EFS_ERR_FATAL_ACTION, sizeof(uint8), &u8_buff)) 
  {
    u8_buff = u8_buff - '0' ; //converting read char's ASCII to its equivalent numerical value
    err_set_auto_action((err_action_type)u8_buff);
    if (ERR_DLOAD == (err_action_type)u8_buff)
      tms_dload_write_magic_numbers();
    if (ERR_RESET == (err_action_type)u8_buff)
      tms_dload_clear_magic_numbers();
  }
  else
  {
    // we should be getting a default value at least, so fatal error if nothing returned
    ERR_FATAL("Error reading NV settings and defaults.",0,0,0);
  }

#if defined FEATURE_DLOAD_HW_RESET_DETECT
  /* Get value for hw reset detect enable (read will return default if NV not set) */
  if (err_nv_efs_reg_item_read(DNT_NV_EFS_ERR_HW_RESET_DETECT_ENABLE, sizeof(uint8), &u8_buff)) 
  {
    u8_buff = u8_buff - '0' ; //converting read char's ASCII to its equivalent numerical value
    err_set_auto_action((err_action_type)u8_buff);
    if(u8_buff) //hw_reset_detect enabled
    {
      tms_dload_write_magic_numbers();
    }
    else //hw_reset_detect disabled
    {
      tms_dload_clear_magic_numbers();
    }
  }
  else
  {
    // we should be getting a default value at least, so fatal error if nothing returned
    ERR_FATAL("Error reading NV settings and defaults.",0,0,0);
  }
#endif /* FEATURE_DLOAD_HW_RESET_DETECT */

} /* err_action_init */

