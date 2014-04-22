/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  C A L L   M A N A G E R   M B M S   M O D U L E

GENERAL DESCRIPTION

  This module contains the Call Manager MBMS module.

  The MBMS object is responsible for:
  1. Processing clients' MBMS commands.
  2. Processing LL replies for clients' MBMS commands.
  3. Processing LL MBMS related notifications.
  4. Notifying the client list of MBMS events.


EXTERNALIZED FUNCTIONS

  cmmbms_timer_proc
  cmmbms_client_cmd_proc
  cmmbms_init
  cmmbms_ptr
  cmmbms_mc_rpt_proc


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2004 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/cm/src/cmmbms.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/11/11   am      Cleaning stubs and runnning qtf SUTE
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
07/01/08   rm      Mainlining of MBMS changes
06/25/07   rp      Adding support for CM_MBMS_EVENT_GENERIC_CMD
03/13/07   rp      Initial release for MBMS phase 2A integration

===========================================================================*/



/****************************************************************************

 Organization of the file:

    The file is divided into multiple sections.
    You can jump from one sections to the other by searching for / followed
    by 2 *'s. The order of objects defined is as follows:

        includes
        forward declarations
        #defines

        functions
        - Internal
        - External

****************************************************************************/

#include "mmcp_variation.h"
#include "customer.h"
/*lint -esym(766,customer.h)*/

#if defined (FEATURE_MBMS)
/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
#include "cmmbms.h"
#include "cmdbg.h"
/*lint -esym(766,cmclient.h)*/
#include "cmclient.h"

#ifdef CM_DEBUG
#error code not present
#endif

/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Functions - internal, common
** --------------------------------------------------------------------------
*/
/*===========================================================================

FUNCTION cmmbms_mbms_cmd_err

DESCRIPTION
  Notify clients of a specified MBMS command error.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmmbms_client_cmd_err(

  const cm_mbms_cmd_s_type       *mbms_cmd_ptr,
    /* Pointer to a CM command
    */
  cm_mbms_cmd_err_e_type    cmd_err
    /* Indicate the MBMS command error
    */
  	 
)
{
  /*lint -esym(613, cmd_ptr) 
  ** NOTE -save/-restore do not work with -esym() 
  */ 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( mbms_cmd_ptr != NULL );
  CM_ASSERT( mbms_cmd_ptr->cmd_type == CM_CMD_TYPE_MBMS );
  CM_ASSERT( BETWEEN( cmd_err, CM_MBMS_CMD_ERR_NONE, CM_MBMS_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at MBMS command component
  */

  CM_MSG_LOW_0("START cmmbms_client_cmd_err() mbms_cmd=%d, cmd_err=%d, client=%p",
                  mbms_cmd_ptr->cmd, cmd_err, mbms_cmd_ptr->client_id );

  /* If commanding client supplied a callback function,
  ** notify this client of command error status.
  */
  if( mbms_cmd_ptr->cmd_cb_func != NULL )
  {
    mbms_cmd_ptr->cmd_cb_func( mbms_cmd_ptr->data_block_ptr,
                                mbms_cmd_ptr->cmd,
                                cmd_err );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command error is OTHER than NO ERROR,
  ** notify the client list of command error, as well.
  */
  if( cmd_err != CM_MBMS_CMD_ERR_NOERR )
  {
    CM_MSG_HIGH_3( "MBMS cmd err!, cmd=%d, err=%d, client=%ld",
                 mbms_cmd_ptr->cmd, cmd_err, mbms_cmd_ptr->client_id );

    cmclient_list_mbms_cmd_err_ntfy( cmd_err, mbms_cmd_ptr );
  }

  /*lint +esym(613, cmd_ptr) 
  ** NOTE -save/-restore do not work with -esym() 
  */ 

}
/*===========================================================================

FUNCTION cmmbms_info_alloc

DESCRIPTION
 Allocate a MBMS info buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated NAS info buffer.

SIDE EFFECTS
  none

===========================================================================*/
static cm_mbms_info_s_type  *cmmbms_info_alloc( void )
{
  static cm_mbms_info_s_type   mbms_info;
      /* Currently there is only a need for one nas info buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Simply return a pointer to the statically allocated
  ** NAS info buffer.
  */
  return &mbms_info;
}

/*===========================================================================

FUNCTION cmmbms_client_cmd_check

DESCRIPTION
  Check for MBMS command parameter errors and whether a specified MBMS
  command is allowed in the current state of the system.
  Currently there is no error checking done in CM for MBMS

DEPENDENCIES

RETURN VALUE
  CM_MBMS_CMD_ERR_NOERR if command is allowed in the current state
  of the MBMS, specific cm_mbms_cmd_err_e_type otherwise.

SIDE EFFECTS
  Might change one or more of the command parameters pointed by cmd_ptr.

===========================================================================*/
static cm_mbms_cmd_err_e_type  cmmbms_client_cmd_check(

  cm_mbms_cmd_s_type                  *mbms_cmd_ptr
    /* Pointer to a CM command
    */
)
{
  /*lint -esym(613, cmd_ptr) 
  ** NOTE -save/-restore do not work with -esym() 
  */ 
  cm_mbms_cmd_info_s_type          *cmd_info_ptr  = NULL;
    /* Point at command information component
    */

  cm_mbms_cmd_err_e_type            cmd_err       = CM_MBMS_CMD_ERR_NOERR;
    /* Initialize command error to NO-ERROR
    */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( mbms_cmd_ptr != NULL );
  CM_ASSERT( mbms_cmd_ptr->cmd_type == CM_CMD_TYPE_MBMS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr  = CMD_INFO_PTR(mbms_cmd_ptr);
  CM_ASSERT( BETWEEN(mbms_cmd_ptr->cmd, CM_MBMS_CMD_NONE, CM_MBMS_CMD_MAX ));

  CM_MSG_LOW_1("START cmmbms_client_cmd_check(), cmd=%d",mbms_cmd_ptr->cmd );

  /* If an error code was already detected while queuing the command, return
  ** this error code now.
  */
  if( cmd_info_ptr->cmd_err != CM_MBMS_CMD_ERR_NOERR )
  {
      return cmd_info_ptr->cmd_err;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return cmd_err;

  /*lint +esym(613, cmd_ptr) 
  ** NOTE -save/-restore do not work with -esym() 
  */ 

} // cmmbms_client_cmd_check()

/*===========================================================================

FUNCTION cmmbms_client_cmd_forward_mmll

DESCRIPTION

  Perform prep work before a client MBMS command is sent to LL and
  (eventually) send the command to LL.
  The command itself may or may not be sent at the time this
  function is called.

DEPENDENCIES


RETURN VALUE

  TRUE if CM needs to wait for a replay from LL before continuing
  processing this command, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static boolean cmmbms_client_cmd_forward_mmll(

  const cm_mbms_cmd_s_type    *mbms_cmd_ptr
    /* Pointer to a CM command
    */
)
{
  /*lint -esym(613, cmd_ptr) 
  ** NOTE -save/-restore do not work with -esym() 
  */ 

  msm_ext_cmd_type               *msm_cmd_ptr = NULL;
  //uint8                           num_services = 0;
  /* cmd ptr to mbms mgr */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( mbms_cmd_ptr != NULL );
  CM_ASSERT( mbms_cmd_ptr->cmd_type == CM_CMD_TYPE_MBMS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get a command buffer for sending cmd to MBMS mgr
  */
  msm_cmd_ptr = cm_msm_get_buf_else_err_fatal ();
  msm_cmd_ptr->cmd_hdr.cmd_id = MBMS_CM_GENERIC_CMD;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at MBMS command component
  */

  
  CM_MSG_LOW_1("START cmmbms_client_cmd_forward_mmll(),cmd=%d",mbms_cmd_ptr->cmd);


  /* Get/fill SM buffer and queue it
  */      
  msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd_type = mbms_cmd_ptr->info.cmd_type;
  
  /* */
  *(&(msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd)) = *(&(mbms_cmd_ptr->info.cmd));
 
  cm_msm_send_cmd(msm_cmd_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

  /*lint +esym(613, cmd_ptr) 
  ** NOTE -save/-restore do not work with -esym() 
  */ 
}
/*===========================================================================

FUNCTION cmmbms_client_cmd_mmll_reply

DESCRIPTION

  Process LL reply to the command, send events to clients
  if necessary.

DEPENDENCIES


RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
static void cmmbms_client_cmd_mmll_reply(

  const cm_mbms_cmd_s_type     *mbms_cmd_ptr,
    /* Pointer to a CM command
    */

  cm_mbms_cmd_err_e_type     cmd_err
    /* Indicate MBMS command error
    */
)
/*lint -esym(715, cmd_err) */
{
  /*lint -esym(613, cmd_ptr) 
  ** NOTE -save/-restore do not work with -esym() 
  */ 
  cm_mbms_info_s_type *mbms_info_ptr = cmmbms_info_alloc();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( mbms_cmd_ptr != NULL );
  CM_ASSERT( cmd_err == CM_MBMS_CMD_ERR_NOERR );
  CM_ASSERT( mbms_cmd_ptr->cmd_type == CM_CMD_TYPE_MBMS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at MBMS command component
  */
  CM_MSG_LOW_2("START cmmbms_client_cmd_mmll_reply(), cmd=%d, err=%d",
                  mbms_cmd_ptr->cmd, cmd_err );

  mbms_info_ptr->mbms_generic_cmd.cmd_type = 
    mbms_cmd_ptr->info.cmd_type;

  *(&(mbms_info_ptr->mbms_generic_cmd.cmd)) = *(&(mbms_cmd_ptr->info.cmd));

  /* Send MBMS event */
  cmclient_list_mbms_event_ntfy( CM_MBMS_EVENT_GENERIC_CMD,
                                 mbms_info_ptr );
   
  /*lint +esym(613, cmd_ptr) 
  ** NOTE -save/-restore do not work with -esym() 
  */ 
}
/*lint +esym(715, cmd_err) */

/*===========================================================================

FUNCTION cmmbms_client_cmd_proc

DESCRIPTION
  Process clients' MBMS commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmmbms_client_cmd_proc(

  cm_mbms_cmd_s_type   *mbms_cmd_ptr
    /* pointer to a CM command
    */
)
{

  cm_mbms_cmd_err_e_type   mbms_cmd_err;
  boolean                  wait_for_reply;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_0("START cmmbms_client_cmd_proc()" );

  CM_ASSERT( mbms_cmd_ptr != NULL );
  CM_ASSERT( mbms_cmd_ptr->cmd_type == CM_CMD_TYPE_MBMS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start by checking whether this is a valid command
  ** relative to the current state of the system.
  */
  mbms_cmd_err = cmmbms_client_cmd_check( mbms_cmd_ptr );

  /*
  ** Notify clients of command processing status.
  ** If error found, return.
  */
  cmmbms_client_cmd_err( mbms_cmd_ptr, mbms_cmd_err );
  if( mbms_cmd_err != CM_MBMS_CMD_ERR_NOERR )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, no errors were found,
  ** so forward the client command to Lower Layer.
  */
  wait_for_reply = cmmbms_client_cmd_forward_mmll( mbms_cmd_ptr );

  /* If NO need to wait for replay from Lower Layer,
  ** call the reply function right a way.
  */
  if( ! wait_for_reply )
  {
    cmmbms_client_cmd_mmll_reply( mbms_cmd_ptr,
                                   CM_MBMS_CMD_ERR_NOERR );
  }

}

/**--------------------------------------------------------------------------
** Functions - external, common
** --------------------------------------------------------------------------
*/
/*===========================================================================

FUNCTION cmmbms_rpt_proc

DESCRIPTION

  Process LL reports related to MBMS


DEPENDENCIES
  MBMS object must have already been initialized with
  cmmbms_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmmbms_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
)

{
  /*lint -save -e826 */
  cm_rpt_type     *cm_rpt_ptr    = (cm_rpt_type *) rpt_ptr;
  /*lint -restore */
  cm_mbms_info_s_type *mbms_info_ptr = cmmbms_info_alloc();
    

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process the report
  */
  switch( cm_rpt_ptr->hdr.cmd )
  {
    case CM_RPT_MBMS_GENERIC :


      mbms_info_ptr->mbms_generic_rpt.rpt_type = 
        cm_rpt_ptr->cmd.mbms_generic_rpt.rpt_type;

      CM_MSG_HIGH_1("MBMS rpt_type %d",mbms_info_ptr->mbms_generic_rpt.rpt_type);

      *(&(mbms_info_ptr->mbms_generic_rpt.rpt)) = *(&(cm_rpt_ptr->cmd.mbms_generic_rpt.rpt));
      
      /* Send MBMS event */
      cmclient_list_mbms_event_ntfy( CM_MBMS_EVENT_GENERIC_RPT,
                                     mbms_info_ptr );
      break;
      
   default:
     break;
  } 
}   

#else
/*===========================================================================
FUNCTION cmmbms_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmmbms_dummy( void )
/*lint -esym(714,cmmbms_dummy) */
/*lint -esym(528,cmmbms_dummy) */
/*lint -esym(765,cmmbms_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}
#endif /* FEATURE_MBMS */
