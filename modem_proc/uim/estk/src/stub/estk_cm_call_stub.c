/*===========================================================================
FILE:  estk_cm_call_stub.c


GENERAL DESCRIPTION 
  This source file contains dummy implmentation of functions handled in estk_cm_call.c

  Copyright (c) 2016, 2018 QUALCOMM Technologies, Inc (QTI) and its licensors.
  All Rights Reserved.	QUALCOMM Technologies Proprietary.
  Export of this technology or software
  is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/estk/src/stub/estk_cm_call_stub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/18   ks      Split ME and slot specific NV
02/11/16   gs      Init and activation with CM for Supplementary services
02/09/16   gs      Supplementary services and WMS registration
02/05/16   gs      Initial Version

=============================================================================*/

/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "uim_msg.h"
#include "estk.h"
#include "estk_cm.h"
#include "estk_sms.h"

#if defined(FEATURE_ESTK)

/*===========================================================================

                             DECLARATIONS

===========================================================================*/


/*===========================================================================

                             FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

FUNCTION: estk_cm_init

DESCRIPTION:
  Dummy Functionlity

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_cm_init( void )
{
  cm_client_status_e_type cm_status = CM_CLIENT_ERR_NONE;
  uint8                   index     = 0;

  UIM_MSG_ERR_0("Voice, DTMF not supported");

  /* Register with WMS for client id and events */
  estk_sms_reg();

  for(index = 0; index < ESTK_CALL_ID_MAX; index++)
  {
    estk_shared_info.call_info.call_list[index].call_id       = 0xFF;
    estk_shared_info.call_info.call_list[index].call_sys_mode = SYS_SYS_MODE_NO_SRV;
    estk_shared_info.call_info.call_list[index].call_active   = FALSE;
  }

  if(gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_SUPPS_INTERFACE) == FALSE)
  {
    cm_status = cm_client_init(CM_CLIENT_TYPE_GSTK,
        &(estk_shared_info.cm_client_id));
  
    if ( cm_status != CM_CLIENT_OK )
    {
      UIM_MSG_ERR_1("cm_client_init() returned bad status 0x%x",
                    (int32)cm_status);
      return;
    }
    
    /* CM Client Activation */
    cm_status = cm_client_act(estk_shared_info.cm_client_id);
    if ( cm_status != CM_CLIENT_OK )
    {
      UIM_MSG_ERR_1( "ESTK: Client cm_client_act Status 0x%x", cm_status );
      return;
    }
    /* Register for Sups related events */
    cm_status = cm_client_sups_reg( estk_shared_info.cm_client_id, /* client id */
                                    estk_cm_sups_event_cb,
                                    CM_CLIENT_EVENT_REG,             /* event registration */
                                    CM_SUPS_EVENT_REGISTER,          /* from this sups event */
                                    CM_SUPS_EVENT_SS_MOD_TO_ORIG,    /* to this sups event */
                                    estk_cm_sups_reg_cmd_cb);
    if (cm_status != CM_CLIENT_OK)
    {
      UIM_MSG_ERR_1("Failed to register for CM Sups event 0x%x", cm_status);
      return;
    }
  }
}/* estk_cm_init */

/*===========================================================================

FUNCTION: estk_process_setup_call_req

DESCRIPTION:
  Dummy Functionlity

PARAMETERS:
  gstk_req_ptr : [Input] gstk set up call cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_UNSUPPORTED

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_setup_call_req(
  gstk_cmd_from_card_type      *gstk_req_ptr)
{
  (void) gstk_req_ptr;
  return ESTK_UNSUPPORTED;
}/* estk_process_send_setup_call_req */

/*===========================================================================

FUNCTION: estk_process_cm_call_event_cmd

DESCRIPTION :
   Dummy Functionlity

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_UNSUPPORTED

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cm_call_event_cmd(
  const estk_cmd_type *cmd_ptr
)
{
  (void) cmd_ptr;
  return ESTK_UNSUPPORTED;
}/* estk_process_cm_call_event_cmd */

/*===========================================================================

FUNCTION: estk_process_cm_call_list_cmd

DESCRIPTION:
   Dummy Functionlity

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cm_call_list_cmd(
  const estk_cmd_type *cmd_ptr
)
{
  (void) cmd_ptr;
  return ESTK_UNSUPPORTED;
}/* estk_process_cm_call_list_cmd */

/*===========================================================================
FUNCTION:   estk_setup_call_cleanup

DESCRIPTION:
   Dummy Functionlity

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_setup_call_cleanup(void)
{

}/* estk_setup_call_cleanup */

/*===========================================================================
FUNCTION:   estk_send_dtmf_cleanup

DESCRIPTION:
   Dummy Functionlity

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_send_dtmf_cleanup(void)
{

}/* estk_send_dtmf_cleanup */

/*===========================================================================
FUNCTION: estk_process_send_dtmf_req

DESCRIPTION:
  Dummy Functionlity

PARAMETERS:
  gstk_req_ptr : [Input] gstk set up call cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type
    ESTK_UNSUPPORTED

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_send_dtmf_req(
  gstk_cmd_from_card_type  *gstk_req_ptr)
{
  (void) gstk_req_ptr;
  return ESTK_UNSUPPORTED;
}/* estk_process_send_dtmf_req */

/*===========================================================================

FUNCTION: estk_process_cm_inband_event_cmd

DESCRIPTION :
  Dummy Functionlity

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_UNSUPPORTED

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cm_inband_event_cmd(
  estk_cmd_type *cmd_ptr)
{
  (void) cmd_ptr;
  return ESTK_UNSUPPORTED;
}/* estk_process_cm_inband_event_cmd */


/*===========================================================================

FUNCTION: estk_process_setup_call_user_cnf

DESCRIPTION :
  Dummy Functionlity

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_UNSUPPORTED    

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_setup_call_user_cnf_cmd(
  const estk_cmd_type *cmd_ptr
)
{
  (void) cmd_ptr;
  return ESTK_UNSUPPORTED;
}/* estk_process_setup_call_user_cnf */

#endif /* FEATURE_ESTK */
