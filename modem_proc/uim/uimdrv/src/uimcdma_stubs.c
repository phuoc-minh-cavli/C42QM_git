/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


              U I M   C D M A   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the CDMA protocol of the UIM
  interface.

EXTERNALIZED FUNCTIONS

  uim_process_cdma_command
    Processes a generic UIM server command

  uim_send_cdma_err_rpt
    Processes an error condition for a cdma UIM server command

  uim_cdma_command_response
    This function processes the UIM responses from cdma APDUs.

Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/uimcdma_stubs.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/12/16   sam     Avoid CDMA DF selection if IOE feature is enabled
06/08/16   na      Stubing out the CDMA\GSM\ICC specific functionality 
05/19/16   sam     Added api to check CDMA support in UIM
05/19/16   na      Optimization for LTE only devices
02/01/16   ssr     Adding uimcdma stubs for IOE variant

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "intconv.h"

/*lint -esym(766,..\..\services\utils\comdef.h)
  comdef.h was previously included to get rid of compiler warning about
  no external translation unit.  The best thing to do would be
  to not compile uimcdma.c based on a rule..... but till then.....
  let us calm down the lint
*/
#include "bit.h"
#include "uimdrv_msg.h"
#include "err.h"
#include "uimi.h"
#include "uim.h"
#include "uimdrv.h"
#include "uimcdma.h"
#include "uimutil.h"
#include <memory.h>
#include "uimglobals.h"


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*===========================================================================

FUNCTION UIM_GET_ESN_ME

DESCRIPTION
  This function returns the HW ME ESN cached by TMC at power up.

DEPENDENCIES
  None

RETURN VALUE
  The HW ME ESN.  If the ESN was not cached properly, 0 is returned

SIDE EFFECTS
  None

===========================================================================*/
uint32 uim_get_esn_me ( void )
{
  UIM_MSG_HIGH_0("uim_get_esn_me is now deprecated because tmc_get_esn is deprecated");
  /* Return 0 so that users of the api use nv function to get ESN */
  return 0;
} /* uim_get_esn_me */


/*===========================================================================

FUNCTION UIM_GET_CDMA_SVC_TABLE_ITEM

DESCRIPTION
  This function checks availabilities for a specified service.

DEPENDENCIES
  The CDMA service table has to be read first.

RETURN VALUE
  TRUE :   The service is activated.
  FALSE:   The service is unavailable.

SIDE EFFECTS
  None.

===========================================================================*/
uim_svc_table_return_type uim_return_cdma_svc_availabililty
(
  uim_cdma_svc_table_enum_type item,  /* The service to be checked*/
  const byte                   *table /* Pointer to the service table */
)
{
  uim_svc_table_return_type svc;
  svc.activated = FALSE;
  svc.allocated = FALSE;

  switch (item)
  {
    case UIM_CDMA_SVC_CHV_DISABLE:
      /* CHV disable function */
      if (table[0] & UIM_CDMA_SVC_CHV_DISABLE_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[0] & UIM_CDMA_SVC_CHV_DISABLE_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_ADN:
      /* Abbreviated Dialling Numbers (ADN) */
      if (table[0] & UIM_CDMA_SVC_ADN_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[0] & UIM_CDMA_SVC_ADN_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_FDN:
      /* Fixed Dialling Numbers (FDN) */
      if (table[0] & UIM_CDMA_SVC_FDN_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[0] & UIM_CDMA_SVC_FDN_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_SMS:
      /* Short Message Storage (SMS) */
      if (table[0] & UIM_CDMA_SVC_SMS_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[0] & UIM_CDMA_SVC_SMS_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_EXT1:
      /* Extension 1 */
      if (table[2] & UIM_CDMA_SVC_EXT1_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[2] & UIM_CDMA_SVC_EXT1_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_EXT2:
      /* Extension 2 */
      if (table[2] & UIM_CDMA_SVC_EXT2_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[2] & UIM_CDMA_SVC_EXT2_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_SMS_PARAMS:
      /* SMS Parameters */
      if (table[2] & UIM_CDMA_SVC_SMS_PARAMS_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[2] & UIM_CDMA_SVC_SMS_PARAMS_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_LND:
      /* Last Number Dialled (LND) */
      if (table[3] & UIM_CDMA_SVC_LND_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[3] & UIM_CDMA_SVC_LND_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_HOME_SVC_PROVIDER_NAME:
      /* CDMA Home Service Provider Name */
      if (table[4] & UIM_CDMA_SVC_HOME_SVC_PVDR_NAME_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[4] & UIM_CDMA_SVC_HOME_SVC_PVDR_NAME_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_SDN:
      /* Service Dialling Numbers */
      if (table[4] & UIM_CDMA_SVC_SDN_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[4] & UIM_CDMA_SVC_SDN_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_EXT3:
      /* Extension 3 */
      if (table[4] & UIM_CDMA_SVC_EXT3_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[4] & UIM_CDMA_SVC_EXT3_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

#ifdef FEATURE_UIM_TOOLKIT_UTK
    case UIM_CDMA_SVC_SMS_PP_DOWNLOAD:
      /* Check CDMA SMS PP download service */
      if ( table[6] & UIM_CDMA_SVC_SMS_PP_DOWNLOAD_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[6] & UIM_CDMA_SVC_SMS_PP_DOWNLOAD_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_MENU_SEL:
      /* Check menu selection service */
      if ( table[6] & UIM_CDMA_SVC_MENU_SEL_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[6] & UIM_CDMA_SVC_MENU_SEL_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_PROACTIVE_UIM:
      /* Check proactive uim service */
      if ( table[7] & UIM_CDMA_SVC_PROACTIVE_UIM_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[7] & UIM_CDMA_SVC_PROACTIVE_UIM_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;
#endif /* FEATURE_UIM_TOOLKIT_UTK */

    case UIM_CDMA_SVC_3GPD_SIP:
      /* Check 3GPD SIP service */
      if ( table[4] & UIM_CDMA_SVC_3GPD_SIP_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[4] & UIM_CDMA_SVC_3GPD_SIP_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_3GPD_MIP:
      /* Check 3GPD MIP service */
      if ( table[9] & UIM_CDMA_SVC_3GPD_MIP_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[9] & UIM_CDMA_SVC_3GPD_MIP_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_HRPD:
      /* Check HRPD service */
      if ( table[1] & UIM_CDMA_SVC_HRPD_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[1] & UIM_CDMA_SVC_HRPD_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_3GPD_MSG_EXT:
      /* Check 3GPD / Messaging Extensions service */
      if ( table[3] & UIM_CDMA_SVC_3GPD_MSG_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[3] & UIM_CDMA_SVC_3GPD_MSG_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_BCSMS:
      /* Check BCSMS service */
      if ( table[3] & UIM_CDMA_SVC_BCSMS_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[3] & UIM_CDMA_SVC_BCSMS_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_RC:
      /* Check Root Certificates service */
      if ( table[3] & UIM_CDMA_SVC_RC_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[3] & UIM_CDMA_SVC_RC_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_BROWSER:
      /* Check Browser service */
      if ( table[5] & UIM_CDMA_SVC_BROWSER_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[5] & UIM_CDMA_SVC_BROWSER_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_JAVA:
      /* Check Java service */
      if ( table[5] & UIM_CDMA_SVC_JAVA_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[5] & UIM_CDMA_SVC_JAVA_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_BREW:
      /* Check BREW service */
      if ( table[5] & UIM_CDMA_SVC_BREW_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[5] & UIM_CDMA_SVC_BREW_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_LBS:
      /* Check LBS service */
      if ( table[5] & UIM_CDMA_SVC_LBS_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[5] & UIM_CDMA_SVC_LBS_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_IPV6:
      /* Check IPv6 service */
      if ( table[7] & UIM_CDMA_IPV6_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[7] & UIM_CDMA_IPV6_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

     default:
      /* This function gets called in NON-UIM context always. UIM instance does
         not matter. Keeping instance as 0 */
      UIM_MSG_MED_0("No such service!");
      break;
  } /* end of switch */

  /* Return the result of the query */
  return svc;
} /* uim_return_cdma_svc_availabililty */


/*===========================================================================

FUNCTION UIM_SEND_CDMA_ERR_RPT

DESCRIPTION
  This function sends an error report for a generic command that is being
  requested. We pass cmd_ptr explicitly here since we may be sending err
  report for am unprocessed command that was present in command queue while
  processing hotswap card removal or card error.

DEPENDENCIES
  This function queues an error report onto a client queue.

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_send_cdma_err_rpt
(
  uim_cmd_type             *cmd_ptr, /* the command to queue up */
  uim_instance_global_type *uim_ptr
)
{
  if (cmd_ptr == NULL)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id, "uim_send_cdma_err_rpt cmd_ptr is null");
    return;
  }

  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Copy user data from received command to report buffer */
  uim_ptr->command.rpt_buf.user_data = cmd_ptr->hdr.user_data;
  /* Report back the status as UIM_FAIL indicating failure in the R-UIM */
  uim_ptr->command.rpt_buf.rpt_status = UIM_FAIL;

  /* Check if the client task has supplied a report back function. */
  if (cmd_ptr->hdr.rpt_function != NULL)
  {
     UIMDRV_MSG_HIGH_1(uim_ptr->id, "Reporting error for CDMA command 0x%x ",
                       cmd_ptr->hdr.command );
    /* Process the error report based on the command that failed. */
    switch(cmd_ptr->hdr.command)
    {
      case UIM_STORE_ESN_ME_F:              /* Store ESN_ME command */
      {
          uim_ptr->command.rpt_buf.rpt_type = UIM_STORE_ESN_ME_R;
      } /* end case - UIM_STORE_ESN_ME_F */
      break;

      case UIM_SSD_UPDATE_F:                /* Begin process to update SSD */
      {
         uim_ptr->command.rpt_buf.rpt_type = UIM_SSD_UPDATE_R;

      } /* end case - UIM_SSD_UPDATE_F */
      break;

      case UIM_CONFIRM_SSD_F:         /* Finish process to update SSD */
      {
        /* Did the client give a reportback functiom. */
          uim_ptr->command.rpt_buf.rpt_type = UIM_CONFIRM_SSD_R;
          /* Indicate the SSD update failed. */
          uim_ptr->command.rpt_buf.rpt.ssd_update_ok = FALSE;
      } /* end case - UIM_CONFIRM_SSD_F */
      break;

      case UIM_RUN_CAVE_F:                 /* Perform auth signature */
      {
        /* Report back auth signature, if requested */
          uim_ptr->command.rpt_buf.rpt_type = UIM_RUN_CAVE_R;
          uim_ptr->command.rpt_buf.rpt.run_cave.auth = 0;
      } /* end case - UIM_RUN_CAVE_F */
      break;

      case UIM_AKA_AUTH_F:          /* Perform AKA authentication */
      {
          uim_ptr->command.rpt_buf.rpt_type = UIM_AKA_AUTH_R;
      } /* end case - UIM_AKA_AUTH_F */
      break;

      case UIM_GENERATE_KEYS_F:              /* Generate CMEA key and VPM */
      {
        /* Report back the result, if requested */
          uim_ptr->command.rpt_buf.rpt_type = UIM_GENERATE_KEYS_R;
      } /* end case - UIM_GENERATE_KEYS_F */
      break;

      case UIM_BS_CHAL_F:                   /* Do a Base station Challenge */
      {
          uim_ptr->command.rpt_buf.rpt_type = UIM_BS_CHAL_R;
          uim_ptr->command.rpt_buf.rpt.bs_chal.randbs = 0;
      } /* end case - UIM_BS_CHAL_F */
      break;

      case UIM_COMMIT_F:              /* Commit Request */
      {
        /* Report back that Commit was not completed */
          uim_ptr->command.rpt_buf.rpt_type = UIM_COMMIT_R;
      } /* end case - UIM_COMMIT_F */
      break;

      case UIM_MS_KEY_REQ_F:          /* MS Key Request */
      {
          uim_ptr->command.rpt_buf.rpt_type = UIM_MS_KEY_REQ_R;
      } /* end case - UIM_MS_KEY_REQ_F */
      break;

      case UIM_KEY_GEN_REQ_F:         /* Key Generation Request */
      {
          uim_ptr->command.rpt_buf.rpt_type = UIM_KEY_GEN_REQ_R;
          uim_ptr->command.rpt_buf.rpt.key_gen.ms_result_len = 0;
      } /* end case - UIM_KEY_GEN_REQ_F */
      break;

      case UIM_SERV_KEY_GEN_REQ_F:         /* Service Key Generation Request */
      {
          uim_ptr->command.rpt_buf.rpt_type = UIM_SERV_KEY_GEN_REQ_R;
          uim_ptr->command.rpt_buf.rpt.serv_key_gen.result = 0;
      } /* end case - UIM_SERV_KEY_GEN_REQ_F */
      break;

      case UIM_VALIDATE_F:            /* Validation Request */
      {
        /* Report back that Validation Request was not completed */
          uim_ptr->command.rpt_buf.rpt_type = UIM_VALIDATE_R;
      } /* end case - UIM_VALIDATE_F */
      break;

      case UIM_CONFIG_REQ_F:             /* Configuration Request */
      {
        /* Report back that Config Request was not completed */
          uim_ptr->command.rpt_buf.rpt_type = UIM_CONFIG_REQ_R;
          /* Fill the block id in the response from the command */
          uim_ptr->command.rpt_buf.rpt.configuration.block_id =
            cmd_ptr->configuration.block_id;
          uim_ptr->command.rpt_buf.rpt.configuration.block_length = 0;
      } /* end case - UIM_CONFIG_REQ_F */
      break;

      case UIM_DOWNLOAD_REQ_F:           /* Download Request */
      {
          uim_ptr->command.rpt_buf.rpt_type = UIM_DOWNLOAD_REQ_R;
      } /* end case - UIM_DOWNLOAD_REQ_F */
      break;

      case UIM_SSPR_CONFIG_REQ_F:        /* SSPR Configuration Request */
      {
          uim_ptr->command.rpt_buf.rpt_type = UIM_SSPR_CONFIG_REQ_R;
          uim_ptr->command.rpt_buf.rpt.sspr_config.block_length = 0;
      } /* end case - UIM_SSPR_CONFIG_REQ_F */
      break;

      case UIM_SSPR_DOWNLOAD_REQ_F:      /* SSPR Download Reqeust */
      {
         uim_ptr->command.rpt_buf.rpt_type = UIM_SSPR_DOWNLOAD_REQ_R;
      } /* end case - UIM_SSPR_DOWNLOAD_REQ_F */
      break;

      case UIM_OTAPA_REQ_F:           /* Otapa Request */
      {
         uim_ptr->command.rpt_buf.rpt_type = UIM_OTAPA_REQ_R;
      } /* end case - UIM_OTAPA_REQ_F */
      break;

      case UIM_COMPUTE_IP_AUTH_F:
      {
          uim_ptr->command.rpt_buf.rpt_type = UIM_COMPUTE_IP_AUTH_R;
      } /* end case - UIM_COMPUTE_IP_AUTH_F */
      break;

      case UIM_STORE_ESN_MEID_ME_F:              /* Store ESN_MEID_ME command */
      {
        /* Report back that store ESN_MEID_ME was not completed */
          uim_ptr->command.rpt_buf.rpt_type = UIM_STORE_ESN_MEID_ME_R;
      } /* end case - UIM_STORE_ESN_MEID_ME_F */
      break;

      default:
        UIMDRV_MSG_ERR_1(uim_ptr->id,
                      "Unknown command 0x%x in uim_send_cdma_err_rpt",
                         cmd_ptr->hdr.command );
        break;

    }  /* switch */

     /* Fill in necessary fields of command block */
      uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

      /* Report to requesting task */
      (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

    /* We only allocate cmd_ptr memory for the external commands that have
       valid callback. Internal UIM commands that use static buffers
       have callback set to NULL. */
    if (cmd_ptr == uim_ptr->command.cmd_ptr)
    {
      /* If cmd_ptr is same as current command in progress set global
         cmd_ptr to NULL as well */
      uim_ptr->command.cmd_ptr = NULL;
    }
    UIM_FREE(cmd_ptr);

  } /* end if - the client has supplied a report back function. */
}


/*===========================================================================

FUNCTION UIM_PROCESS_CDMA_COMMAND

DESCRIPTION
  This procedure processes a command sent to the UIM task. It sets the UIM
  state variable and calls the UIM state machine to process the command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  It sets the UIM_CDMA state variable amongst others.

===========================================================================*/
boolean uim_process_cdma_command
(
  uim_instance_global_type *uim_ptr
)
{

  /* This indicates that the command is processed by the state machine */
  if (uim_ptr->command.cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("uim_process_gsm_command cmd_ptr is NULL");
    return FALSE;
  }

  /* Send an error report to the client and release the buffer */
  uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) 0;
  uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) 0;
  uim_ptr->command.rpt_buf.user_data = uim_ptr->command.cmd_ptr->hdr.user_data;	
  uim_ptr->command.rpt_buf.cmd_transacted  = FALSE;	
  uim_send_cdma_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
  return FALSE;
}
 /* uim_process_cdma_command() */



/*===========================================================================

FUNCTION UIM_CDMA_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a generic comamnd that has been
  recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_ptr->command.cdma_state_ptr.

===========================================================================*/

uim_cmd_status_type uim_cdma_command_response
(
  uim_rsp_buf_type const *rsp_ptr,
  uim_instance_global_type *uim_ptr
)
{

  if (rsp_ptr == NULL || uim_ptr == NULL)
  {
    UIM_MSG_ERR_0("uim_cdma_command_response either uim_ptr or rsp_ptr is NULL");
    return UIM_CMD_ERR;	
  }
  uim_ptr->command.cmd_ptr->hdr.cmd_count = uim_ptr->setting.max_num_cmd_attempts;
  return UIM_CMD_ERR;
   
} /* uim_cdma_command_response */



/*===========================================================================

FUNCTION UIM_CURRENT_DIR_CDMA_CMD

DESCRIPTION
  This procedure determines if an intermediate select is needed for all
  CDMA commands

===========================================================================*/
void uim_current_dir_cdma_cmd
(
  uim_instance_global_type     *uim_ptr,
  uint8                         curr_ch,
  uim_intermediate_select_type *intermediate_sel_ptr
)
{

  intermediate_sel_ptr->is_needed    = FALSE;
} /* uim_current_dir_cdma_cmd */


/*===========================================================================

FUNCTION UIM_IS_CDMA_SUPPORTED

DESCRIPTION
  This procedure determines if CDMA is supported in the build

===========================================================================*/
boolean uim_is_cdma_supported(void)
{
  /*CDMA functionality is not present. so return FALSE*/
  return FALSE;
}/*uim_is_cdma_supported*/


/*===========================================================================
FUNCTION uim_check_cdma_df_select_before_tp

DESCRIPTION
  This function checks weather to selct CDMA DF explicitly,
  based on the nv data taken from mmgsdi

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/

boolean uim_check_cdma_df_select_before_tp(void)
{
  return (FALSE);
  
} /* uim_check_cdma_df_select_before_tp */


/*===========================================================================

FUNCTION UIM_CHECK_EXPLICIT_SELECT_DF_CDMA_BEFORE_TELECOM_ACCESS

DESCRIPTION
  This function checks weather an explicit selection of CDMA DF is required,
  before telecom access for RUIM Card

DEPENDENCIES
  It is based on MMGSDI_NV_EXPLICIT_SELECT_DFCDMA_BEFORE_TELECOM_ADN_READ_I
  NV item.

RETURN VALUE
  boolean, It returns true when RUIM card is used and last selected directory
  was not the TELECOM or CDMA DF, false otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_check_explicit_select_df_cdma_before_telecom_access
(
  uim_instance_global_type     *uim_ptr
)
{
  (void) uim_ptr;
  return (FALSE);
} /* uim_check_explicit_select_df_cdma_before_telecom_access */

