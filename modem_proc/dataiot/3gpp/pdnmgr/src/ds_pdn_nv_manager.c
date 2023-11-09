/*===========================================================================
                      DS_PDN_NV_MANAGER.C
DESCRIPTION
This file defines PDN NV manager APIs.

Copyright (c) 2018 - 2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_pdn_nv_manager.c#9 $
  $DateTime: 2023/09/26 01:04:13 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_nv_manager.h"
#include "ds_pdn_cp_data_hdlr.h"
#include "ds_eps_pdn_manager.h"
#include "ds3gmmgsdiif.h"
#include "lte_rrc_ext_api.h"
#include "emm_ext_msg.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#define DS_PDN_NV_DATA_CONFIG_PATH "/nv/item_files/conf/data_config_info.conf"

static ds_pdn_mgr_nv_info_type ds_pdn_nv_info = {0};

#ifdef FEATURE_DATA_LTE
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
#endif

#define DS_PDN_THROT_SPEC_DEF_SM_RETRY_TIMER_VALUE 12


/*---------------------------------------------------------------------------
  Data services task NV command item.
---------------------------------------------------------------------------*/
static nv_cmd_type  ds_pdn_nv_command;


/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_EFS_FILE_INIT

DESCRIPTION   The function intializes the state machine and
              also opens the file.

DEPENDENCIES  None.

RETURN VALUE  0  : SUCCESS: The file is good, readable,
                            State Machine Initialized.
              -1 : FAILURE: The file cannot be opened/ readable.

SIDE EFFECTS  None.
===========================================================================*/
int ds_pdn_nv_manager_efs_file_init
(
  const char                 *file_path,
  ds_pdn_nv_efs_token_type   *sm
)
{
  int             ret_val = -1;
  struct fs_stat  temp_buf;
  size_t          efs_file_path_size;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the structure variables and open the file in read mode.
    Care must be taken to not initialize this structure variable before
    calling this function
  -------------------------------------------------------------------------*/
  if(sm == NULL)
  {
    return ret_val;
  }

  memset(sm, 0, sizeof(ds_pdn_nv_efs_token_type));

  sm->seperator   = ';';
  sm->curr        = sm->buffer;
  sm->end_pos     = sm->buffer;
  sm->eof         = FALSE;
  sm->eol         = FALSE;
  sm->skip_line   = FALSE;
  sm->bol         = TRUE;
  sm->buffer                = NULL;
  sm->efs_file_path         = NULL;
  sm->efs_get_performed     = FALSE;

  if( mcfg_fs_stat(file_path,
                   &temp_buf,
                   MCFG_FS_TYPE_EFS,
                   (mcfg_fs_sub_id_e_type)SYS_MODEM_AS_ID_1)
       != MCFG_FS_STATUS_OK )
  {
    return ret_val;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Allocate efs file path, If it succeeds try allocating buffer.
      If buffer allocation fails, free the file_path allocation.
      Also check for ret_val = -1 to print an appropriate message.
    -----------------------------------------------------------------------*/
    sm->read_buffer_size = temp_buf.st_size;

    if(sm->read_buffer_size <= 0)
    {
      return ret_val;
    }

    DS_SYSTEM_HEAP_MEM_ALLOC(sm->buffer,sizeof(char) * sm->read_buffer_size, char*);
    if( sm->buffer != NULL )
    {
      memset(sm->buffer, 0, sizeof(char) * sm->read_buffer_size);
      efs_file_path_size = sizeof(char) * (strlen(file_path)+1);
      DS_SYSTEM_HEAP_MEM_ALLOC(sm->efs_file_path, efs_file_path_size, char*);

      if( sm->efs_file_path != NULL )
      {
        memset(sm->efs_file_path, 0, efs_file_path_size);
        (void)strlcpy(sm->efs_file_path,file_path,efs_file_path_size);
        ret_val = 0;
      }
      else
      {
        DS_SYSTEM_HEAP_MEM_FREE(sm->buffer);
      }
    }
  }

  return ret_val;
} /* ds3gcfgmgr_efs_file_init_ex() */

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_EFS_TOKENIZER

DESCRIPTION   The is the function that reads data from the opened file.
              The data read is looked for tokens
              1. Each token is seperated by ';'
              2. Successive ';' means empty token
              3. Line begining with '#' is comment
              4. '\n' is the end of token and record
              5. Empty line is ignored
              6. Insufficient tokens is a record is considered bad record

DEPENDENCIES  File should have already been opened.

RETURN VALUE
              SUCCESS : Success => Found Token.
                        *begin points to the begining of token.
                        *end points to the end of token.
              EOL     : End of line is reached => record end
                        => no token extracted
              END     : End of file is reached => file end => feof
                        => no token extracted
              FAILURE : Failed

SIDE EFFECTS  None.
===========================================================================*/
ds_pdn_nv_efs_token_parse_status_type ds_pdn_nv_manager_efs_tokenizer
(
  ds_pdn_nv_efs_token_type    *sm,
  char                       **begin,
  char                       **end
)
{
  int                                     bytes_read = 0;
  char                                   *dummy;
  ds_pdn_nv_efs_token_parse_status_type   retval;
  mcfg_fs_status_e_type                   mcfg_retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *begin = 0;
  *end   = 0;

  /*-------------------------------------------------------------------------
     Traversed to end of file => return
  -------------------------------------------------------------------------*/
  if( sm->eof )
  {
    return DS_PDN_NV_EFS_TOKEN_PARSE_EOF;
  }

  /*-------------------------------------------------------------------------
     Have some bytes to read from the buffer
  -------------------------------------------------------------------------*/
  while( sm->curr < sm->end_pos )
  {
    /*-----------------------------------------------------------------------
      Skip over all carriage return characters (\r) added if file was
      editted using a windows machine
    -----------------------------------------------------------------------*/
    if (*sm->curr == '\r')
    {
      sm->skip_line = FALSE;
      sm->curr++;
      continue;
    }

    /*-----------------------------------------------------------------------
       Lines begining the record with # are comments. Continue to read
       until we reach the end of file.
    -----------------------------------------------------------------------*/
    if( sm->bol && *sm->curr ==  '#' )
    {
      sm->skip_line = TRUE;
      sm->bol = FALSE;
      sm->curr++;
      continue;
    }

    if( sm->skip_line )           /* reading a comment */
    {
      if( *sm->curr == '\n' )     /* End of comment */
      {
        sm->skip_line = FALSE;
        sm->eol = TRUE;
        sm->bol = TRUE;
      }
      sm->curr++;
      continue;                   /*Continue to read until the end of line */
    }

    /*-----------------------------------------------------------------------
      Look for the token. If ';' found at the begining then it is
      an empty token.
      There could be a  case where we hit '\n' while we are looking for a
      token so skip over all the new lines.
    -----------------------------------------------------------------------*/
    if( *begin == 0 )                   /* Find the beginning of token */
    {
      if( *sm->curr == sm->seperator )  /* an empty token */
      {
        if( sm->bol == TRUE )
        {
          sm->bol = FALSE;
        }

        *begin = sm->curr;
        *end   = sm->curr;
        sm->curr++;
        return DS_PDN_NV_EFS_TOKEN_PARSE_SUCCESS;
      }

      if( *sm->curr == '\n' )           /* Possibly an empty token */
      {
        if( sm->eol )                   /* Skip over any successive new lines */
        {
          sm->curr++;
          continue;
        }

        *begin  = sm->curr;
        *end    = sm->curr;
        sm->eol = TRUE;
        sm->bol = TRUE;
        sm->curr++;
        return DS_PDN_NV_EFS_TOKEN_PARSE_SUCCESS;
      }

      /*---------------------------------------------------------------------
       Before beginning a new token, return an end of record for
       previous record.
      ---------------------------------------------------------------------*/
      if( sm->eol )
      {
        sm->eol = FALSE;
        return DS_PDN_NV_EFS_TOKEN_PARSE_EOL;
      }

      *begin = sm->curr;                /* Initialize to beginning of token */
    }
    else if(*sm->curr == sm->seperator || *sm->curr == '\n' ||
            ((sm->seperator == '*') && ((*sm->curr == ':')|| (*sm->curr == ';'))))
    {
      /*----------------------------------------------------------------------
        Only in cases where the seperator is "Wildcard",
        overwrite the seperator with current value (: or ;)
      ----------------------------------------------------------------------*/
      if(sm->seperator == '*')
      {
        sm->seperator = *sm->curr;
      }
   
      *end = sm->curr++;                /* Found end of token */

      /*---------------------------------------------------------------------
         This is a end of line. Save the state and send
         end of line event when a next token is requested .
      ---------------------------------------------------------------------*/
      if( **end == '\n' )
      {
        sm->eol = TRUE;
        sm->bol = TRUE;
      }
      return DS_PDN_NV_EFS_TOKEN_PARSE_SUCCESS;
    }

    sm->curr++;
  }/* while */

  /*-------------------------------------------------------------------------
    In the middle of token and we ran out characters in the buffer
  -------------------------------------------------------------------------*/
  if( *begin )
  {
    if( *begin != sm->buffer )
    {
      /*---------------------------------------------------------------------
        Move the partial token over to beginning of buffer
      ---------------------------------------------------------------------*/
      /*lint -e732 */
      memscpy( sm->buffer,DS_EFS_READ_BUFFER_SZ, *begin, (sm->curr - *begin) );
      /*lint +e732 */
      sm->curr = sm->buffer + (sm->curr - *begin);
      *begin = sm->buffer;
    }
    else
    {
      return DS_PDN_NV_EFS_TOKEN_PARSE_FAILURE;
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      No token or data exists in the buffer
    -----------------------------------------------------------------------*/
    sm->curr = sm->buffer;
  }

  /*-------------------------------------------------------------------------
    efs_get has to be performed only once, If it has already been
    performed return EOF
  -------------------------------------------------------------------------*/
  if(!sm->efs_get_performed)
  {
    /*-----------------------------------------------------------------------
      Read data from the efs file.
    -----------------------------------------------------------------------*/
    mcfg_retval = mcfg_fs_read((const char*)sm->efs_file_path,
                         (void *)sm->curr,
                               sm->read_buffer_size,
                               MCFG_FS_TYPE_EFS,
                               (mcfg_fs_sub_id_e_type)SYS_MODEM_AS_ID_1);

    if(mcfg_retval != MCFG_FS_STATUS_OK)
    {
      return DS_PDN_NV_EFS_TOKEN_PARSE_FAILURE;
    }

   bytes_read = sm->read_buffer_size;

    sm->efs_get_performed = TRUE;
  }
  else
  {
    sm->eof = TRUE;
    return DS_PDN_NV_EFS_TOKEN_PARSE_EOL;
  }

  if( bytes_read > 0 )
  {
    sm->end_pos = sm->curr + bytes_read;
    sm->eof    = FALSE;

    if(*begin != 0)
    {
      retval= ds_pdn_nv_manager_efs_tokenizer( sm, &dummy, end ); /* Call the function
                                            again because you could be in the
                                            middle of reading a token */
    }
    else
    {
      retval = ds_pdn_nv_manager_efs_tokenizer( sm, begin, end);
    }

    return retval;
  }
  else
  {
    /* No bytes read => reached the end of file. */
    if(*begin == 0)
    {
      sm->eof = 1;
      return DS_PDN_NV_EFS_TOKEN_PARSE_EOL;
    }
    else
    {
      /*---------------------------------------------------------------------
        If a token was found return the token and
        when next token is requested send EOF
      ---------------------------------------------------------------------*/
      *end = sm->curr;
      if(bytes_read == 0)
      {
        /*-------------------------------------------------------------------
         If the EOF character is missing in the file and the bytes read
         are zero all the time then we are trying to bail out of this.

         NOTE: We might have to revisit this later again if different
         modules
        -------------------------------------------------------------------*/
        sm->eof = 1;
        return DS_PDN_NV_EFS_TOKEN_PARSE_EOL;
      }
      return DS_PDN_NV_EFS_TOKEN_PARSE_SUCCESS;
    }
  }/* End of bytes read*/
} /* ds_pdn_nv_manager_efs_tokenizer() */

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_EFS_FILE_CLOSE

DESCRIPTION   The function closes file and releases the state machine

DEPENDENCIES  The file should have opened already.

RETURN VALUE  NONE

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_nv_manager_efs_file_close
(
  ds_pdn_nv_efs_token_type  *sm
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(sm == NULL)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    For an efs_get we will not have to close, However free all allocated
    dynamic memory
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_FREE(sm->efs_file_path);
  sm->efs_file_path = NULL;
  DS_SYSTEM_HEAP_MEM_FREE(sm->buffer);
  sm->buffer = NULL;

  return;
} /* ds_pdn_nv_manager_efs_file_close() */

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_READ_LEGACY_NV

DESCRIPTION   This function reads legacy NV item.

DEPENDENCIES  NV task is expected to be available.  Must run in the
              DS task context.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds_pdn_nv_manager_read_legacy_nv
(
  nv_items_enum_type      item,
  nv_item_type           *data_ptr
)
{

#ifdef TEST_FRAMEWORK
  #error code not present
#else
//  ASSERT(IS_IN_DS_TASK());
  ds_pdn_nv_command.tcb_ptr = rex_self();
  ds_pdn_nv_command.sigs = DS_NV_CMD_SIGNAL;
#endif
  ds_pdn_nv_command.done_q_ptr = NULL;
  ds_pdn_nv_command.item = item;
  ds_pdn_nv_command.cmd = NV_READ_F;

  /* Set up NV so that it will read the data into the correct location */
  ds_pdn_nv_command.data_ptr    = data_ptr;

  /* Clear the return signal, call NV, and wait for a response */
#ifndef TEST_FRAMEWORK
  (void) rex_clr_sigs( rex_self(), DS_NV_CMD_SIGNAL );
#endif

  nv_cmd( &ds_pdn_nv_command );

#ifndef TEST_FRAMEWORK
  (void) ds_wait( DS_NV_CMD_SIGNAL );
#endif

  return ds_pdn_nv_command.status;
} /* ds_pdn_nv_manager_read_legacy_nv() */ 

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_READ_EFS_NV

DESCRIPTION   This function reads EFS NV item.
                        data_ptr should be correctly by initailized by clients.

DEPENDENCIES  None.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds_pdn_nv_manager_read_efs_nv
(
  const char             *item_path,
  void                   *data_ptr,
  uint32                  item_size
)
{
  mcfg_fs_status_e_type  read_status;
  nv_stat_enum_type      ret_val = NV_DONE_S;
  struct fs_stat         temp_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (item_path == NULL) || (data_ptr == NULL) )
  {
    return NV_FAIL_S;
  }

  /* Check if the file is present or not. */
  if( mcfg_fs_stat(item_path,
                   &temp_buf,
                   MCFG_FS_TYPE_EFS,
                   (mcfg_fs_sub_id_e_type)SYS_MODEM_AS_ID_1)
       != MCFG_FS_STATUS_OK )
  {
    return NV_NOTACTIVE_S;
  }

  if((temp_buf.st_size) != (item_size))
  {
    return NV_NOTACTIVE_S;
  }

  /* Obtain the contents from EFS */
  read_status = mcfg_fs_read(item_path,
                             data_ptr,
                             item_size,
                             MCFG_FS_TYPE_EFS,
                             (mcfg_fs_sub_id_e_type)SYS_MODEM_AS_ID_1);

  if (read_status != MCFG_FS_STATUS_OK)
  {
    ret_val = NV_FAIL_S;
  }
  else
  {
    ret_val = NV_DONE_S;
  }

  return ret_val;
} /* ds3gcfgmgr_read_efs_nv_ex() */

/*===========================================================================
FUNCTION DS_PDN_NV_PATH_IS_DIRECTORY

DESCRIPTION
  To check if the EFS directory exists

DEPENDENCIES
  None

PARAMETERS
  dirname - Directory path

RETURN VALUE
   0         - success
   efs_errno - EFS error
   -1        - Other error

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_pdn_nv_path_is_directory
(
  const char  *dirname
)
{
  mcfg_fs_status_e_type  rsp;
  struct fs_stat         stat_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rsp = mcfg_fs_stat( dirname,
                      &stat_info,
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE );

  if( MCFG_FS_STATUS_OK != rsp )
  {
    rsp = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
    if( ENOENT != rsp )
    {
      return rsp;
    }
  }
  else if( S_ISDIR (stat_info.st_mode))
  {
    return 0;
  }

  return -1;
}

#ifdef FEATURE_DATA_LTE
#ifdef TEST_FRAMEWORK
#error code not present
#endif
#endif

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_get_nv_lte_rel_version

DESCRIPTION
  This function returns  the LTE release version, read during nv manager
  initialzizing.

PARAMETERS
  void

DEPENDENCIES
  None.

RETURN VALUE
  lte_3gpp_release_ver_e enum type

SIDE EFFECTS
  None

===========================================================================*/
lte_3gpp_release_ver_e ds_pdn_nv_manager_get_nv_lte_rel_version()
{
#ifdef TEST_FRAMEWORK
  #error code not present
#else
  return lte_rrc_get_3gpp_release_version(); 
#endif /* TEST_FRAMEWORK */
}/* ds_3gpp_pdn_get_nv_lte_rel_version */


#ifdef FEATURE_DATA_LTE

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_ENABLE_SEND_TRM_PRIORITY_NV

DESCRIPTION
  This function reads the ENABLE_SEND_TRM_PRIORITY NV(73562) item for each
  subscription

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE  
  None

SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_enable_send_trm_priority_nv()
{
  char item_path[] = "/nv/item_files/modem/data/3gpp/lteps/enable_send_trm_priority";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
  	                                &ds_pdn_nv_info.ds_eps_enable_send_trm_priority,
  	                                sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_eps_enable_send_trm_priority = FALSE;
  }

  return;
}

/*===========================================================================
FUNCTION  DS_PDN_NV_MANAGER_GET_ENABLE_SEND_TRM_PRIORITY

DESCRIPTION
  This function gets the value of NV 73562 ENABLE_SEND_TRM_PRIORITY per
  subscription

PARAMETERS 
  subs_id subscription id
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - ENABLE_SEND_TRM_PRIORITY = 1.
  False - ENABLE_SEND_TRM_PRIORITY = 0.
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_nv_manager_get_enable_send_trm_priority()
{
  return ds_pdn_nv_info.ds_eps_enable_send_trm_priority;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_NULL_APN_SUPPORT_FOR_NON_ATTACH_PDN

DESCRIPTION
  This function reads null_apn_support_for_non_attach_pdn NV item

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_null_apn_support_for_non_attach_pdn
(
  void
)
{
  char item_path[] = "/nv/item_files/modem/data/3gpp/lteps/null_apn_support_for_non_attach_pdn";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.null_apn_support_for_non_attach_pdn,
                                   sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.null_apn_support_for_non_attach_pdn = FALSE;
  }
  else
  {
    ds_pdn_nv_info.is_null_apn_support_set = TRUE;
  }
  
  return;
}/*ds_pdn_nv_manager_read_null_apn_support_for_non_attach_pdn_from_nv*/

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_NULL_APN_SUPPORT_FOR_NON_ATTACH_PDN

DESCRIPTION
  This function gets null_apn_support_for_non_attach_pdn NV item

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_null_apn_support_for_non_attach_pdn
(
  void
)
{
  /* For Rel 13, enable NULL APN support by default */
  if(!ds_pdn_nv_info.is_null_apn_support_set)
  {
    return ds_pdn_nv_manager_is_rel_13_or_above_supported();
  }
  
  return ds_pdn_nv_info.null_apn_support_for_non_attach_pdn;
}/*ds_pdn_nv_manager_get_null_apn_support_for_non_attach_pdn_from_nv*/

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_remove_unused_pdn

DESCRIPTION
  This function reads the remove unused pdn NV item  

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/

void ds_pdn_nv_manager_read_remove_unused_pdn
(
   void
)
{
  char  item_path[] = "/nv/item_files/modem/data/3gpp/ps/remove_unused_pdn";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.ds_eps_remove_unused_pdn,
                                   sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_eps_remove_unused_pdn = FALSE;
  }
  
  return;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_APN_PARAM_CHG_CFG

DESCRIPTION
  This function reads the apn_param_chg_cfg NV(73603 ) item for each
  subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_3gpp_nv_manager_read_apn_param_chg_cfg
(
  void
)
{
  char  item_path[] = "/nv/item_files/modem/data/3gpp/ps/enable_apn_param_chg";
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.enable_apn_param_chg,
                                   sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.enable_apn_param_chg = FALSE;
  }
  
  return;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_apn_param_chg_cfg

DESCRIPTION
  This function gets the apn_param_chg_cfg NV item  

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/

boolean ds_pdn_nv_manager_get_apn_param_chg_cfg
(
   void
)
{
  return ds_pdn_nv_info.enable_apn_param_chg; 
}


/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_remove_unused_pdn

DESCRIPTION
  This function gets the remove unused pdn NV item  

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/

boolean ds_pdn_nv_manager_get_remove_unused_pdn
(
   void
)
{
  return ds_pdn_nv_info.ds_eps_remove_unused_pdn; 
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_ignore_inactivity_timer

DESCRIPTION
  This function reads ignore inactivity timer nv

PARAMETERS 
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/

void ds_pdn_nv_manager_read_ignore_inactivity_timer
(
  void
)
{  
  char  item_path[] = "/nv/item_files/modem/data/3gpp/ignore_last_pdn_ia_timer";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.ignore_inactivity_timer,
                                   sizeof(uint8)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ignore_inactivity_timer = FALSE;
  }
  
  return;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_ignore_inactivity_timer

DESCRIPTION
  This function returns true if ignore inactivity timer nv is set to true

PARAMETERS 
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - if call orig is allowed before ps attach
  False - 
 
SIDE EFFECTS
  None.
===========================================================================*/

uint8 ds_pdn_nv_manager_get_ignore_inactivity_timer
(
  void
)
{
  return ds_pdn_nv_info.ignore_inactivity_timer;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_CP_DATA_WINDOW_SIZE

DESCRIPTION
  This function reads cp_data_window_size NV item

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_cp_data_window_size
(
  void
)
{
  char  item_path[] = "/nv/item_files/modem/data/3gpp/ps/cp_data_window_size";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.cp_data_window_size,
                                   sizeof(uint8)) != NV_DONE_S)
  {
    ds_pdn_nv_info.cp_data_window_size = 
                                  DS_PDN_CP_DATA_DEF_SENDING_WINDOW_SIZE;
  }
  
  return;
}/* ds_3gpp_nv_manager_read_cp_data_window_size */

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_CP_DATA_WINDOW_SIZE

DESCRIPTION
  This function gets cp_data_window_size NV item

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS

===========================================================================*/
uint8 ds_pdn_nv_manager_get_cp_data_window_size
(
  void
)
{
  return ds_pdn_nv_info.cp_data_window_size;
}/* ds_3gpp_nv_manager_read_cp_data_window_size */

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_apn_rate_control_config

DESCRIPTION
  This function reads the APN Rate Control configuration NV

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_apn_rate_control_config
(
  void
)
{
  char  item_path[] = "/nv/item_files/modem/data/3gpp/ps/apn_rate_control_config";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.apn_rate_control_config,
                                   sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.apn_rate_control_config = FALSE;
  }

  return;
}/*ds_pdn_nv_manager_read_apn_rate_control_config*/

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_additional_read_apn_rate_control_config

DESCRIPTION
  This function reads the Additional APN Rate Control configuration NV

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_additional_apn_rate_control_config
(
  void
)
{
  char  item_path[] = "/nv/item_files/modem/data/3gpp/ps/additional_apn_rate_control_config";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.additional_apn_rate_control_config,
                                   sizeof(boolean)) != NV_DONE_S)
  {

    if(ds_pdn_nv_manager_is_rel_14_or_above_supported() == TRUE)
    {
      ds_pdn_nv_info.additional_apn_rate_control_config = TRUE;
    }
    else
    {	
    ds_pdn_nv_info.additional_apn_rate_control_config = FALSE;
  }
  }

  return;
}/*ds_pdn_nv_manager_read_additional_apn_rate_control_config*/

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_apn_rate_control_config

DESCRIPTION
  This function gets the boolean that shows whether or not APN Rate
  control configuration is enabled or not.

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_apn_rate_control_config
(
  void
)
{  
  return ds_pdn_nv_info.apn_rate_control_config;
}/*ds_pdn_nv_manager_get_apn_rate_control_config */

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_additional_apn_rate_control_config

DESCRIPTION
  This function gets the boolean that shows whether or not Additional APN Rate
  control configuration is enabled or not.

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_additional_apn_rate_control_config
(
  void
)
{  
  return ds_pdn_nv_info.additional_apn_rate_control_config;
}/*ds_pdn_nv_manager_get_additional_apn_rate_control_config */

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_LTE_ATTACH_DUAL_IP_FB_IP_TYPE

DESCRIPTION
  This NV will be read soon after Init. The IP type read will be used,
  to redial attach request after the initial request fails with ESM cause
  #28 (Unknown PDN Type).
  
  If the NV value is not set, currently, the IP type defaults to IPV4

PARAMETERS
  None
.
DEPENDENCIES
  None.

RETURN VALUE
  None  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_nv_manager_read_lte_attach_dual_ip_fb_ip_type
(
  void
)
{
  char                    item_path[] = "/nv/item_files/modem/data/3gpp/ps/dual_ip_fallback";
  ds_eps_attach_fb_info_type  attach_fb_info = {0};
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if( (ds_pdn_nv_manager_read_efs_nv(item_path,
                            (uint8*)&(attach_fb_info), 
                            sizeof(ds_eps_attach_fb_info_type)) != NV_DONE_S) ||
      (attach_fb_info.fallback_enabled == FALSE) )
  {
    ds_eps_pdn_set_lte_attach_dual_ip_fb_ip_type(FALSE, 
                                            DS_EPS_ATTACH_FB_IP_TYPE_MIN);
  }
  else
  {
    ds_eps_pdn_set_lte_attach_dual_ip_fb_ip_type(TRUE, 
                                            attach_fb_info.fallback_ip_type);
  }

  return;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_allow_default_pdn_teardown

DESCRIPTION
  This function reads the always  default pdn tear down NV item

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_allow_default_pdn_teardown
(
  void
)
{
  char  item_path[] = "/nv/item_files/modem/data/3gpp/lteps/allow_def_lte_pdn_teardown";
  uint8 eps_allow_def_pdn_teardown = FALSE;
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   (unsigned char *)&eps_allow_def_pdn_teardown,
                                   sizeof(uint8)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_eps_allow_def_pdn_teardown = FALSE;
  }
  else
  {
    if(eps_allow_def_pdn_teardown == 0x1)
    {
      ds_pdn_nv_info.ds_eps_allow_def_pdn_teardown = TRUE;
    }
    else
    {
      ds_pdn_nv_info.ds_eps_allow_def_pdn_teardown = FALSE;
    }
  }

  return;
}/*ds_pdn_nv_manager_read_allow_default_pdn_teardown*/

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_allow_default_pdn_teardown

DESCRIPTION
  This function gets the boolean that shows whether default PDN tear down is allowed

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  boolean.

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_allow_default_pdn_teardown
(
  void
)
{  
  return ds_pdn_nv_info.ds_eps_allow_def_pdn_teardown;
}/*ds_pdn_nv_manager_get_allow_default_pdn_teardown */

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_always_connect_def_pdn_flag

DESCRIPTION
  This function reads the always connect default PDN flag.

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_always_connect_def_pdn_flag
(
  void
)
{
  char  item_path[] = "/nv/item_files/modem/data/3gpp/lteps/auto_connect_def_pdn";
  uint8 eps_always_connect_def_pdn_flag = FALSE;
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   (unsigned char *)&eps_always_connect_def_pdn_flag,
                                   sizeof(uint8)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_eps_always_connect_def_pdn_flag = FALSE;
  }
  else
  {
    if(eps_always_connect_def_pdn_flag == 0x1)
    {
      ds_pdn_nv_info.ds_eps_always_connect_def_pdn_flag = TRUE;
    }
    else
    {
      ds_pdn_nv_info.ds_eps_always_connect_def_pdn_flag = FALSE;
    }
  }

  return;
}/*ds_pdn_nv_manager_read_always_connect_def_pdn_flag*/

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_always_connect_def_pdn_flag

DESCRIPTION
  This function gets the boolean that shows whether always connect default PDN is set

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  boolean.

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_always_connect_def_pdn_flag
(
  void
)
{  
  return ds_pdn_nv_info.ds_eps_always_connect_def_pdn_flag;
}/*ds_pdn_nv_manager_get_always_connect_def_pdn_flag */

/*===========================================================================
FUNCTION ds_pdn_nv_manager_read_disable_apn_matching_from_nv

DESCRIPTION
  This function reads the disable_apn_matching NV(73529 ) item 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_disable_apn_matching_from_nv
(
   void
)
{
  char  item_path[] = "/nv/item_files/modem/data/3gpp/lteps/disable_apn_matching";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                &ds_pdn_nv_info.ds_3gpp_disable_apn_matching,
                                   sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_3gpp_disable_apn_matching = TRUE;
  }
  
  return;
}

/*===========================================================================
FUNCTION ds_pdn_nv_manager_get_disable_apn_matching_from_nv

DESCRIPTION
  This function gets the disable_apn_matching NV(73529 ) item 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_disable_apn_matching_from_nv
(
   void
)
{
  return ds_pdn_nv_info.ds_3gpp_disable_apn_matching;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_switch_apn_on_reject

DESCRIPTION
  This function reads the apn switch on reject  NV item 

PARAMETERS
 None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_switch_apn_on_reject
(
  void
)
{
  char  item_path[] = "/nv/item_files/modem/data/3gpp/lteps/switch_apn_on_reject";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                &ds_pdn_nv_info.ds_eps_switch_apn_on_reject,
                                   sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_eps_switch_apn_on_reject = TRUE;
  }
  
  return;
}/* ds_pdn_nv_manager_read_switch_apn_on_reject */

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_switch_apn_on_reject

DESCRIPTION
  This function gets the apn switch on reject  NV item 

PARAMETERS
 None

DEPENDENCIES
  None.

RETURN VALUE  
  boolean

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_switch_apn_on_reject
(
  void
)
{
  return ds_pdn_nv_info.ds_eps_switch_apn_on_reject;
}/* ds_pdn_nv_manager_get_switch_apn_on_reject */

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_rai_config

DESCRIPTION
  This function reads RAI timer NV

PARAMETERS
 None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_rai_config
(
  void
)
{
  char  item_path[] = "/nv/item_files/modem/data/3gpp/rai_config";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.rai_config,
                                   sizeof(ds_pdn_rai_config_type))
                                   != NV_DONE_S)
  {
    ds_pdn_nv_info.rai_config.rai_timer = 1;
    ds_pdn_nv_info.rai_config.rai_delay_thres = 10;
  }

  return;
}/* ds_pdn_nv_manager_read_rai_config */


#ifdef FEATURE_FAST_DORMANCY
/*========================================================================
FUNCTION  ds_pdn_nv_manager_read_fd_config
 
DESCRIPTION
  This function reads Fast dormancy NV
 
PARAMETERS
 None
 
DEPENDENCIES
  None.
 
RETURN VALUE  
 
 
SIDE EFFECTS
 
===========================================================================*/
void ds_pdn_nv_manager_read_fd_config
(
  void
)
{
  char item_path[] = "/nv/item_files/modem/data/3gpp/fd_config";

  if(ds_pdn_nv_manager_read_efs_nv(item_path,
  	                               &ds_pdn_nv_info.fd_config,
                                   sizeof(ds_pdn_fd_timer_config))
                                   != NV_DONE_S)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Failed to read FD config, setting default"
		                    " values");
	
    ds_pdn_nv_info.fd_config.tn_timer  = DS_FAST_DORMANCY_DEFAULT_TIMER_VALUE_TN;
	ds_pdn_nv_info.fd_config.ntn_timer = DS_FAST_DORMANCY_DEFAULT_TIMER_VALUE_NTN;
  }

   return;
}/* ds_pdn_nv_manager_read_fd_config */
#endif /* FEATURE_FAST_DORMANCY */

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_rai_info

DESCRIPTION
  This function gets the value of rai timer from NV

PARAMETERS
 None

DEPENDENCIES
  None.

RETURN VALUE  
  timer value in seconds

SIDE EFFECTS

===========================================================================*/
ds_pdn_rai_config_type ds_pdn_nv_manager_get_rai_info
(
  void
)
{
  return ds_pdn_nv_info.rai_config;
}/* ds_pdn_nv_manager_get_rai_info */


#ifdef FEATURE_FAST_DORMANCY
/*===========================================================================
 FUNCTION  ds_pdn_nv_manager_get_fd_info
  
 DESCRIPTION
   This function gets the fast dormancy timer info from NV
  
 PARAMETERS
  None
  
 DEPENDENCIES
   None.
  
 RETURN VALUE  
   timer value in seconds.
  
 SIDE EFFECTS
  
===========================================================================*/
ds_pdn_fd_timer_config ds_pdn_nv_manager_get_fd_info
(
  void
)
{
  return ds_pdn_nv_info.fd_config;
}/* ds_pdn_nv_manager_get_fd_info */
#endif /* FEATURE_FAST_DORMANCY */
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION  DS_PDN_NV_MANAGER_READ_RATE_CTRL_INTERVAL_FACTOR

DESCRIPTION
  This function reads the Rate Control interval factor structure

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_rate_ctrl_interval_factor()
{
  ds_pdn_rate_ctrl_interval_factor_s   rate_ctrl_interval_factor;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* NV 74060 */
  char item_path[] = "/nv/item_files/modem/data/3gpp/ps/rate_ctrl_interval_factor";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                      &rate_ctrl_interval_factor,
                      sizeof(ds_pdn_rate_ctrl_interval_factor_s)) != NV_DONE_S)
  {
    memset(&rate_ctrl_interval_factor,
           0,
           sizeof(ds_pdn_rate_ctrl_interval_factor_s));
    rate_ctrl_interval_factor.apn_rc_intrvl_factor =
                                            DS_PDN_RATE_CTRL_DEFAULT_INTERVAL;
    rate_ctrl_interval_factor.splmn_rc_intrvl_factor =
                                            DS_PDN_RATE_CTRL_DEFAULT_INTERVAL;

  }

  /* for now, limit the interval factor to DS_PDN_RATE_CTRL_DEFAULT_INTERVAL */
  ds_pdn_nv_info.apn_rc_intrvl_factor =
         (rate_ctrl_interval_factor.apn_rc_intrvl_factor >
                                DS_PDN_RATE_CTRL_DEFAULT_INTERVAL) ?
          DS_PDN_RATE_CTRL_DEFAULT_INTERVAL:
          rate_ctrl_interval_factor.apn_rc_intrvl_factor;

  ds_pdn_nv_info.additional_apn_rc_intrvl_factor = ds_pdn_nv_info.apn_rc_intrvl_factor;

  ds_pdn_nv_info.splmn_rc_intrvl_factor = 
          (rate_ctrl_interval_factor.splmn_rc_intrvl_factor > 
                                DS_PDN_RATE_CTRL_DEFAULT_INTERVAL) ?
           DS_PDN_RATE_CTRL_DEFAULT_INTERVAL:
           rate_ctrl_interval_factor.splmn_rc_intrvl_factor;

  return;
}/*ds_pdn_nv_manager_read_rate_ctrl_interval_factor*/

/*===========================================================================
FUNCTION  DS_PDN_NV_MANAGER_GET_RC_INTERVAL_FACTOR

DESCRIPTION
  This function return the APN Rate Control interval factor structure

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
uint8 ds_pdn_nv_manager_get_rc_interval_factor
(
  ds_pdn_rate_ctrl_type     current_rate_ctrl_type
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (current_rate_ctrl_type)
  {
    case DS_PDN_RATE_CTRL_TYPE_SPLMN:
      return ds_pdn_nv_info.splmn_rc_intrvl_factor;
      break;
    case DS_PDN_RATE_CTRL_TYPE_APN:
      return ds_pdn_nv_info.apn_rc_intrvl_factor;
      break;
    case DS_PDN_RATE_CTRL_TYPE_ADDITONAL_APN:
      return ds_pdn_nv_info.additional_apn_rc_intrvl_factor;
      break;
    default:
      break;
  }
  return 0;
}/*ds_pdn_nv_manager_get_rc_interval_factor*/

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_efnas_config

DESCRIPTION
  This function reads the NAS configuration NV

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_efnas_config
(
  void
)
{
  char  item_path[] = "/nv/item_files/modem/nas/efnas_config";

  /*------------------------------------------------------------------------ 
    Allocate memory for nasconfig from the heap
   ------------------------------------------------------------------------*/
  if (ds_pdn_nv_info.efsnas_config == NULL)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(ds_pdn_nv_info.efsnas_config,
    	                        DS_PDN_EFNAS_CONFIG_SIZE,
    	                        byte*);
    if (ds_pdn_nv_info.efsnas_config == NULL )
    {
      DS_ERROR_LOG_0("Heap alloc for efsnasconfig failed");
      return;
    }
  }
   
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   (byte*)ds_pdn_nv_info.efsnas_config,
                                   DS_PDN_EFNAS_CONFIG_SIZE) != NV_DONE_S)
  {
    DS_ERROR_LOG_0("EFNAS read failed.");
    DS_SYSTEM_HEAP_MEM_FREE(ds_pdn_nv_info.efsnas_config);
    ds_pdn_nv_info.efsnas_config = NULL;
  }

  return;
}/*ds_3gpp_nv_manager_read_efnas_config*/

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_efnas_config

DESCRIPTION
  This function gets the NAS configuration

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  byte*

SIDE EFFECTS
  None
===========================================================================*/
byte* ds_pdn_nv_manager_get_efnas_config
(
  void
)
{
  return ds_pdn_nv_info.efsnas_config;
}/*ds_pdn_nv_manager_get_efnas_config*/

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_global_throttling_nv

DESCRIPTION
  This function reads the global throttling NV item 
 

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_global_throttling_nv
(
  void
)
{
  char  item_path[] = "/nv/item_files/modem/data/3gpp/global_throttling";
  ds_pdn_nv_global_throttle_s  global_throt_info = {0};
   
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                             (unsigned char *)&global_throt_info,
                             sizeof(ds_pdn_nv_global_throttle_s)) != NV_DONE_S)
  {
    ds_pdn_nv_info.global_throttling = FALSE;
  }
  else
  {
    ds_pdn_nv_info.global_throttling = global_throt_info.global_throttling;
  }
  
  return;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_global_throttling_nv

DESCRIPTION
  This function reads the global throttling NV item 
 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
 boolean

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_global_throttling_nv
(
  void
)
{
  return ds_pdn_nv_info.global_throttling;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_regular_pdn_throttling_nv

DESCRIPTION
  This function reads the regular_pdn_throttling NV item 
 

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_regular_pdn_throttling_nv
(
  void
)
{
  char  item_path[] = "/nv/item_files/modem/data/3gpp/ps/reg_pdn_throttling";
   
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                    &ds_pdn_nv_info.ds_3gpp_reg_pdn_throttling,
                                    sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_3gpp_reg_pdn_throttling = FALSE;
  }
  
  return;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_regular_pdn_throttling_nv

DESCRIPTION
  This function reads the regular_pdn_throttling NV item 
 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
 boolean

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_regular_pdn_throttling_nv
(
  void
)
{
  return ds_pdn_nv_info.ds_3gpp_reg_pdn_throttling;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_allow_infinite_throttling_nv

DESCRIPTION
  This function reads the allow infinite throt NV item 
 

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_allow_infinite_throttling_nv
(
  void
)
{
  char  item_path[] = "/nv/item_files/modem/data/3gpp/ps/allow_infinite_throt";
   
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                    &ds_pdn_nv_info.ds_eps_allow_infinite_throt,
                                    sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_eps_allow_infinite_throt = FALSE;
  }
  
  return;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_allow_infinite_throttling

DESCRIPTION
  This function fetches the allow infinite throt NV item 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
 boolean

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_allow_infinite_throttling_nv
(
  void
)
{
  return ds_pdn_nv_info.ds_eps_allow_infinite_throt;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_enable_pdn_throt_per_rat_nv

DESCRIPTION
  This function reads the enable_pdn_throt_per_rat NV item

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_enable_pdn_throt_per_rat_nv
(
  void
)
{
  char  item_path[] = "/nv/item_files/modem/data/3gpp/ps/enable_pdn_throttling_per_rat";
   
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                    &ds_pdn_nv_info.enable_pdn_throt_per_rat,
                                    sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.enable_pdn_throt_per_rat = FALSE;
  }
  
  return;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_enable_pdn_throt_per_rat

DESCRIPTION
  This function returns true if the enable_pdn_throt_per_rat NV item 
  is set to TRUE

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
 boolean

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_enable_pdn_throt_per_rat
(
  void
)
{
  return ds_pdn_nv_info.enable_pdn_throt_per_rat;
}

/*===========================================================================
FUNCTION ds_pdn_nv_manager_read_rel_10_throttling_per_plmn_from_nv

DESCRIPTION
  This function reads the rel_10_throttling_per_plmn from NV item 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_rel_10_throttling_per_plmn_from_nv
(
  void
)
{
  char  item_path[] = 
                "/nv/item_files/modem/data/3gpp/ps/rel_10_throttling_per_plmn";
   
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.nv_rel10_throttling_per_plmn,
                                   sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.nv_rel10_throttling_per_plmn = FALSE;
  }

  return;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_rel_10_throttling_per_plmn_from_nv

DESCRIPTION
  This function reads the rel_10_throttling_per_plmn NV item 
 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
 boolean

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_rel_10_throttling_per_plmn_from_nv
(
  void
)
{
  return ds_pdn_nv_info.nv_rel10_throttling_per_plmn;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGGER_READ_SER_REQ_THOR_SM_NV

DESCRIPTION
  This function is used to read service request throttle info nv item
 
PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void  ds_pdn_nv_manager_read_ser_req_thor_sm_nv
(
  void
)
{

  char  item_path[] = 
                "/nv/item_files/modem/data/3gpp/ps/ser_req_throttle_behavior";
   
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                           (unsigned char *)&ds_pdn_nv_info.ser_req_throt_sm_nv,
                            sizeof(uint32)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ser_req_throt_sm_nv = 0;
  }

  return;	
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_SER_REQ_THOR_SM_NV

DESCRIPTION
  This function is used to retrieve service request throttle info per
  subscription
 
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint32. - Service request throttling info

SIDE EFFECTS
  None.

===========================================================================*/
uint32  ds_pdn_nv_manager_get_ser_req_thor_sm_nv
(
  void
)
{  
  return ds_pdn_nv_info.ser_req_throt_sm_nv;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_send_ps_data_avail_on_pwr_save

DESCRIPTION
  This function reads the send PS data avail on power save NV. 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_nv_manager_read_send_ps_data_avail_on_pwr_save
(
  void
)
{
  char  item_path[] = 
  	          "/nv/item_files/modem/data/3gpp/ds_3gpp_send_ps_data_avail_on_power_save";

  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                (uint8*)&ds_pdn_nv_info.send_ps_data_avail_on_power_save,
                             sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.send_ps_data_avail_on_power_save = TRUE;
  }
  
  return;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_send_ps_data_avail_on_pwr_save

DESCRIPTION
  This function gets the send PS data avail on power save NV. 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_nv_manager_get_send_ps_data_avail_on_pwr_save
(
  void
)
{
  return ds_pdn_nv_info.send_ps_data_avail_on_power_save;
}

/*===========================================================================
  FUNCTION ds_pdn_nv_manager_read_nas_umts_rel_version
  
  DESCRIPTION
  This function is is called in 3gpp nv manager init.It reads the NAS
  Complaince NV item and updates the variable ds_3gpp_nv_umts_rel_version
  accordingly for umts specific throttling scenerios.
    
  PARAMETERS  
  None.
      
  DEPENDENCIES 
  None.
    
  RETURN VALUE 
  None.
     
  SIDE EFFECTS 
  None.   
===========================================================================*/
void ds_pdn_nv_manager_read_nas_umts_rel_version()
{
  nv_stat_enum_type               nv_status = NV_DONE_S;
  nv_item_type                    ds_nv_item = {{0}};


  nv_status = ds_pdn_nv_manager_read_legacy_nv(NV_NAS_RELEASE_COMPLIANCE_I, &ds_nv_item);

  if(nv_status == NV_DONE_S)
  {
    ds_pdn_nv_info.ds_3gpp_nv_umts_rel_version = 
    	                     (ue_nas_rel_compliance_type)ds_nv_item.nas_release_compliance;
  }
  else
  {
    ds_pdn_nv_info.ds_3gpp_nv_umts_rel_version = NV_NAS_COMPLIANCE_REL7;
  }

  return;
} /* ds_pdn_nv_manager_read_nas_umts_rel_version */

/*===========================================================================
FUNCTION ds_pdn_nv_manager_get_nv_umts_rel_version

DESCRIPTION
  This Function reads the NAS Complaince NV item and updates the global variable 
  ds_3gpp_nv_umts_rel_version accordingly for umts specific scenerios.

PARAMETERS
  void

DEPENDENCIES
  None.

RETURN VALUE
  ue_nas_rel_compliance_type enum type

SIDE EFFECTS
  None

===========================================================================*/
ue_nas_rel_compliance_type ds_pdn_nv_manager_get_nv_umts_rel_version()
{
  return ds_pdn_nv_info.ds_3gpp_nv_umts_rel_version; 
}/* ds_pdn_nv_manager_get_nv_umts_rel_version */

/*===========================================================================
FUNCTION ds_pdn_nv_manager_read_bcm_validation

DESCRIPTION
  This function reads the nv bcm validation.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
..None.

SIDE EFFECTS
 .None
===========================================================================*/

void ds_pdn_nv_manager_read_bcm_validation()
{
  ds_pdn_bcm_info_validation_s  bcm_info_validation={0};
  char item_path[] = "/nv/item_files/modem/data/3gpp/bcm_validation_on_bearer_mod";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
  	                                &bcm_info_validation,
  	                                sizeof(bcm_info_validation)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_3gpp_nv_bcm_validation_on_bearer_mod = TRUE;
  }

  return;
}

/*===========================================================================
FUNCTION ds_pdn_nv_manager_get_bcm_info_validation_nv

DESCRIPTION
  This function gets the BCM validation NV setting.

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  TRUE /FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_nv_manager_get_bcm_info_validation_nv()
{
  return ds_pdn_nv_info.ds_3gpp_nv_bcm_validation_on_bearer_mod;
}

/*===========================================================================
FUNCTION ds_pdn_nv_manager_read_umts_nw_init_qos_support_nv

DESCRIPTION
 This function gets the ds_3gpp_nv_umts_nw_init_qos_support NV item
 NV: 70331
  /nv/item_files/modem/data/3gpp/umts_nw_initiated_qos_support

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
..None.

SIDE EFFECTS
 .None
===========================================================================*/
void ds_pdn_nv_manager_read_umts_nw_init_qos_support_nv()
{
  char item_path[] = "/nv/item_files/modem/data/3gpp/umts_nw_initiated_qos_support";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.ds_3gpp_nv_umts_nw_init_qos_support,
                                   sizeof(uint8)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_3gpp_nv_umts_nw_init_qos_support = DS_PDN_NV_ENABLED;
  }
  
  return;
}

/*===========================================================================
FUNCTION ds_pdn_nv_manager_get_umts_nw_init_qos_support_nv

DESCRIPTION
  This function gets the ds_3gpp_nv_umts_nw_init_qos_support NV item
  NV: 70331
  /nv/item_files/modem/data/3gpp/umts_nw_initiated_qos_support

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  1 - If NV enabled
  0 - otherwise

SIDE EFFECTS
  None

===========================================================================*/
uint8 ds_pdn_nv_manager_get_umts_nw_init_qos_support_nv()
{
  return ds_pdn_nv_info.ds_3gpp_nv_umts_nw_init_qos_support;
}

/*===========================================================================
FUNCTION ds_pdn_nv_manager_read_multi_pdn_dun_nv

DESCRIPTION
  This function reads the multi pdn dun nv.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
..None.

SIDE EFFECTS
 .None
===========================================================================*/
void ds_pdn_nv_manager_read_multi_pdn_dun_nv()
{
  uint8 multi_pdn_dun_is_enabled = FALSE;  
  char item_path[] = "/nv/item_files/modem/data/3gpp/umts/enable_multi_pdn_dun";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &multi_pdn_dun_is_enabled,
                                   sizeof(uint8)) != NV_DONE_S)
  {
    ds_pdn_nv_info.dsumts_rmsmi_multi_pdn_dun_is_enabled = FALSE;
  }
  else
  {  
    if(multi_pdn_dun_is_enabled == 0x1)
    {
      ds_pdn_nv_info.dsumts_rmsmi_multi_pdn_dun_is_enabled = TRUE;
    }
    else
    {
      ds_pdn_nv_info.dsumts_rmsmi_multi_pdn_dun_is_enabled = FALSE;
    }
  }
  	  
  return;
}

/*===========================================================================
  FUNCTION ds_pdn_nv_manager_get_multi_pdn_dun_nv

  DESCRIPTION
    This function gets the multi pdn dun nv item

  PARAMETERS
    None

  DEPENDENCIES
    None.

  RETURN VALUE  
    TRUE /FALSE

  SIDE EFFECTS
===========================================================================*/
boolean ds_pdn_nv_manager_get_multi_pdn_dun_nv()
{  
  return ds_pdn_nv_info.dsumts_rmsmi_multi_pdn_dun_is_enabled;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_EFS_3GPP_REL_VERSION

DESCRIPTION 
  This function reads the 3GGP_REL_VERSION NV and saves for caching 
 
PARAMETERS
  None
DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_nv_manager_read_efs_3gpp_rel_version()
{

  char item_path[] = "/nv/item_files/modem/data/3gpp/ps/3gpp_rel_version";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.ds_pdn_3gpp_rel_version,
                                   sizeof(uint8)) != NV_DONE_S)
  {
#ifdef FEATURE_DATA_PRE_REL8
    ds_pdn_nv_info.ds_pdn_3gpp_rel_version = (uint8)DS_3GPP_REL7;
#else
    ds_pdn_nv_info.ds_pdn_3gpp_rel_version = (uint8)DS_3GPP_REL8;
                             /* Default to MAX version supported */
#endif /* FEATURE_DATA_PRE_REL8 */
  }

  return;
}

/*===========================================================================
FUNCTION ds_pdn_nv_manager_get_3gpp_rel_version

DESCRIPTION 
  This function returns 3GGP_REL_VERSION from cache  
 
PARAMETERS
  None
DEPENDENCIES
  None.

RETURN VALUE
  cached 3GPP_REL_VERSION NV value
SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds_pdn_nv_manager_get_3gpp_rel_version()
{
  return ds_pdn_nv_info.ds_pdn_3gpp_rel_version;
}

/*===========================================================================
FUNCTION  DS_PDN_NV_MANAGER_READ_REL12_QCI_SUPPORTED

DESCRIPTION
  This Function reads the Release 12 QCI Support Nv item 

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_nv_manager_read_rel12_qci_support( void )
{
  char item_path[] = "/nv/item_files/modem/data/3gpp/ps/rel12_qci_supported";
  
  if(ds_pdn_nv_manager_is_rel_13_or_above_supported())
  {
    ds_pdn_nv_info.ds_3gpp_rel12_qci_supported = TRUE;
  }
  else
  {
    if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.ds_3gpp_rel12_qci_supported,
                                   sizeof(boolean)) != NV_DONE_S)
    {
      ds_pdn_nv_info.ds_3gpp_rel12_qci_supported = FALSE;
    }
  }
  return;  
}

/*===========================================================================
FUNCTION  DS_PDN_NV_MANAGER_GET_REL12_QCI_SUPPORT

DESCRIPTION
  This function gets the value of NV Release 12 QCI Support Nv item

PARAMETERS 
  None.
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - Release 12 qci supported
  False - Release 12 qci not supported
 
SIDE EFFECTS
  None.
 
===========================================================================*/
boolean ds_pdn_nv_manager_get_rel12_qci_support()
{
  return ds_pdn_nv_info.ds_3gpp_rel12_qci_supported;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_CIOT_CAP

DESCRIPTION
  This function is to read the CIOT capability information from NAS NV item.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_nv_manager_read_ciot_cap(void)
{
  char item_path[] = "/nv/item_files/modem/nas/nas_ciot_capability_config";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.ds_3gpp_ciot_bit_mask,
                                   sizeof(uint16)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_3gpp_ciot_bit_mask = NAS_CIOT_DEFAULT_CONFIG;
  }
  
  return;  
} /* ds_3gpp_nv_manager_read_ciot_cap */

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_CIOT_CAP

DESCRIPTION
  This function is to get the CIOT capability information from NAS NV item.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint16 - Bitmask with CIOT capability

SIDE EFFECTS
  None.
===========================================================================*/
uint16 ds_pdn_nv_manager_get_ciot_cap
(
  void
)
{
  return ds_pdn_nv_info.ds_3gpp_ciot_bit_mask;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_APM_APN_SWITCHING

DESCRIPTION
  This function is used to get APM APN switching flag

  PARAMETERS
  None
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void ds_pdn_nv_manager_read_apm_apn_switching
(
  void
)
{
  char item_path[] = "/nv/item_files/data/dsd/ds_apn_switching";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.apm_apn_switching,
                                   sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.apm_apn_switching = TRUE;
  }

  return ;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_APM_APN_SWITCHING

DESCRIPTION
  This function is to get the APM APN switching flag

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - APM APN switching flag

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_nv_manager_get_apm_apn_switching
(
  void
)
{
  return ds_pdn_nv_info.apm_apn_switching;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_AUTH_FALLBACK

DESCRIPTION
  This function is used to get auth fb
  
PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void ds_pdn_nv_manager_read_auth_fallback
(
  void
)
{
  char item_path[] = "/nv/item_files/modem/data/3gpp/ps/3gpp_auth_type_fallback";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.auth_fb_enabled,
                                   sizeof(uint8)) != NV_DONE_S)
  {
    ds_pdn_nv_info.auth_fb_enabled = FALSE;
  }

  return ;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_AUTH_FALLBACK

DESCRIPTION
  This function is to get the auth fb enabled flag

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - AUTH FB flag

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_nv_manager_get_auth_fallback
(
  void
)
{
  return ds_pdn_nv_info.auth_fb_enabled;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGGER_READ_MAX_RAB_REESTAB_REQ_RETRY_CNT

DESCRIPTION
  This function is used to read mx rab reestablishment count nv item
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_nv_manager_read_max_rab_reestab_req_retry_cnt
(
  void
)
{
  char item_path[] = "/nv/item_files/data/3gpp/max_rab_reestab_retry_cnt";

  ds_pdn_nv_info.max_rab_reestab_req_retry_cnt = RAB_REESTAB_RETRY_CNT_INVALID;
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.max_rab_reestab_req_retry_cnt,
                                   sizeof(uint32)) != NV_DONE_S)
  {
    ds_pdn_nv_info.max_rab_reestab_req_retry_cnt = RAB_REESTAB_RETRY_CNT_DEFAULT;
  }
  
  return;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_MAX_RAB_REESTAB_RETRY_CNT

DESCRIPTION
  This function is used to retrieve umax rab reestablishment retry count 
 
  PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  uint32 -max rab reestab count

SIDE EFFECTS
  None.

===========================================================================*/

uint32 ds_pdn_nv_manager_get_max_rab_reestab_retry_cnt
(
  void
)
{
  return ds_pdn_nv_info.max_rab_reestab_req_retry_cnt;     
}

/*===========================================================================
FUNCTION ds_pdn_nv_manager_read_check_ehplmn_list_from_nv

DESCRIPTION
  This function reads the check_ehplmn_list NV item for each
  subscription

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
===========================================================================*/
void ds_pdn_nv_manager_read_check_ehplmn_list_from_nv
(
   void
)
{
  char item_path[] = "/nv/item_files/modem/data/3gpp/ps/check_ehplmn_list";

  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.check_ehplmn_list,
                                   sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.check_ehplmn_list = TRUE;
  }
  
  return;
} /* ds_pdn_nv_manager_read_check_ehplmn_list_from_nv */

/*===========================================================================
FUNCTION ds_pdn_nv_manager_get_check_ehplmn_list

DESCRIPTION
  This function returns true if check_ehplmn_list NV item is set to TRUE

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_nv_manager_get_check_ehplmn_list
(
  void
)
{
  return ds_pdn_nv_info.check_ehplmn_list;
} /* ds_3gpp_nv_manager_get_check_ehplmn_list */

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_call_orig_before_ps_attach

DESCRIPTION
  This function returns true if call orig is allowed before ps attach.

PARAMETERS 
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - if call orig is allowed before ps attach
  False - 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_nv_manager_read_call_orig_before_ps_attach
(
  void
)
{
  char item_path[] = "/nv/item_files/modem/data/3gpp/call_orig_allowed_before_ps_attach";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.call_orig_before_ps_attach,
                                   sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.call_orig_before_ps_attach = TRUE;
  }
  
  return;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_call_orig_before_ps_attach

DESCRIPTION
  This function returns true if call orig is allowed before ps attach.

PARAMETERS 
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - if call orig is allowed before ps attach
  False - 
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_nv_manager_get_call_orig_before_ps_attach
(
  void
)
{
  return ds_pdn_nv_info.call_orig_before_ps_attach;  
}


/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_READ_CONFIG_PDN_LEVEL_AUTH

DESCRIPTION   Read config_pdn_level_auth NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_nv_manager_read_config_pdn_level_auth
(
  void
)
{
  char  item_path[] = "/nv/item_files/modem/data/dsmgr/config_pdn_level_auth";

  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.ds_pdn_level_auth,
                                   sizeof(uint8)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_pdn_level_auth = DS_PDN_AUTH_CHAP;
  }
  
  return;
} /* ds_pdn_nv_manager_read_config_pdn_level_auth */

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_GET_CONFIG_PDN_LEVEL_AUTH

DESCRIPTION   Gets config_pdn_level_auth NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
uint8 ds_pdn_nv_manager_get_config_pdn_level_auth
(
  void
)
{
  return ds_pdn_nv_info.ds_pdn_level_auth;
} /* ds_pdn_nv_manager_get_config_pdn_level_auth */

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_MTU_FROM_NV

DESCRIPTION
  This function is used to read mtu value from nv item 

  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_nv_manager_read_mtu_from_nv 
(
   void
)
{
  char  item_path[] = "/nv/item_files/data/3gpp/ds_3gpp_mtu";

  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.mtu_size,
                                   sizeof(uint16)) != NV_DONE_S)
  {
    ds_pdn_nv_info.mtu_size = DS_PDN_DEFAULT_DATA_UNIT;
    ds_pdn_nv_info.default_mtu = TRUE;
  }
  
  return;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_MTU_FROM_NV

DESCRIPTION
  This function is used to get mtu value from nv item 

  PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 ds_pdn_nv_manager_get_mtu_from_nv 
(
   void
)
{
  if( ds_pdn_nv_info.default_mtu &&
     (emm_get_active_rat() == SYS_RAT_LTE_NB1_RADIO_ACCESS
#ifdef FEATURE_NBIOT_NTN
      || SYS_RAT_LTE_NB1_NTN_RADIO_ACCESS == emm_get_active_rat()
#endif /* FEATURE_NBIOT_NTN */
    ))
  {
    return DS_PDN_CP_DATA_DEF_MTU_LEN;
  }
  else
  {
    return ds_pdn_nv_info.mtu_size;    
  }
  
}

/*===========================================================================
FUNCTION ds_pdn_nv_manager_is_rel_10_or_above_supported

DESCRIPTION
  This Function returns the NAS Complaince NV item  read during nv manager
  initialization

PARAMETERS
  void

DEPENDENCIES
  None.

RETURN VALUE
  boolean - True -> Umts Nas release version 10 or above
          - False -> Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_nv_manager_is_rel_10_or_above_supported()
{
  boolean ret_val = FALSE;

  if( (ds_pdn_nv_manager_get_nv_umts_rel_version() >= NV_NAS_COMPLIANCE_REL10)
#ifdef FEATURE_DATA_LTE 
      || (ds_pdn_nv_manager_get_nv_lte_rel_version() >= LTE_3GPP_REL10)
#endif /* FEATURE_DATA_LTE */
    )
  {
    ret_val=TRUE;
  }

  return ret_val; 
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_nv_lte_rel_10

DESCRIPTION
  This function returns true if the nv item corresponding to the file
  /nv/item_files/modem/lte/lte_3gpp_release_ver is LTE_3GPP_REL10
  and release 10 throttling is enabled

PARAMETERS
  subs_id  subscription 
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_nv_manager_get_nv_lte_rel_10
(
  void
)
{
  boolean ret_val = FALSE;

  if(ds_pdn_nv_manager_get_nv_lte_rel_version() >= LTE_3GPP_REL10)
  {
    ret_val = TRUE;
  }
  return ret_val; 
}

/*===========================================================================
FUNCTION ds_pdn_nv_manager_is_rel_11_or_above_supported

DESCRIPTION
  Determine if UE is release 11 ready or not

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE 
   TRUE - if device is release 11 ready
   FALSE - Otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_is_rel_11_or_above_supported
(
   void
)
{
#ifdef FEATURE_DATA_REL11
  if( ds_pdn_nv_manager_get_nv_umts_rel_version() >= NV_NAS_COMPLIANCE_REL11 
#ifdef FEATURE_DATA_LTE
    || ds_pdn_nv_manager_get_nv_lte_rel_version() >= LTE_3GPP_REL11
#endif /* FEATURE_DATA_LTE */
  )
  {
     return TRUE;
  }
#endif

     return FALSE;
} /* ds_pdn_nv_manager_is_rel_11_or_above_supported */

/*===========================================================================
FUNCTION ds_pdn_nv_manager_is_rel_12_or_above_supported

DESCRIPTION
  Determine if UE is release 12 ready or not

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE 
   TRUE - if device is release 12 ready
   FALSE - Otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_is_rel_12_or_above_supported
(
   void
)
{
  if( ds_pdn_nv_manager_get_nv_umts_rel_version() >= NV_NAS_COMPLIANCE_REL12  || 
      ds_pdn_nv_manager_get_nv_lte_rel_version() >= LTE_3GPP_REL12 )
  {
    return TRUE;
  }

  return FALSE;
} /* ds_pdn_nv_manager_is_rel_12_or_above_supported */

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_IS_REL_13_OR_ABOVE_SUPPORTED

DESCRIPTION
  Determine if UE is release 13 ready or not

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE 
   TRUE - if device is release 13 ready
   FALSE - Otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_is_rel_13_or_above_supported
(
   void
)
{
  /* Hit a compilation issue while porting the changes and hence making the NAS NV check
     to be greater than NV_NAS_COMPLIANCE_REL12 instead >= NV_NAS_COMPLIANCE_REL13 */
  if( ds_pdn_nv_manager_get_nv_umts_rel_version() > NV_NAS_COMPLIANCE_REL12  || 
      ds_pdn_nv_manager_get_nv_lte_rel_version() >= LTE_3GPP_REL13 )
  {
    return TRUE;
  }

  return FALSE;
} /* ds_pdn_nv_manager_is_rel_13_or_above_supported */

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_IS_REL_14_OR_ABOVE_SUPPORTED

DESCRIPTION
  Determine if UE is release 14 ready or not

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE 
   TRUE - if device is release 14 ready
   FALSE - Otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_is_rel_14_or_above_supported
(
   void
)
{
  /* Hit a compilation issue while porting the changes and hence making the NAS NV check
     to be greater than NV_NAS_COMPLIANCE_REL12 instead >= NV_NAS_COMPLIANCE_REL13 */
  if( ds_pdn_nv_manager_get_nv_umts_rel_version() > NV_NAS_COMPLIANCE_REL12  || 
      ds_pdn_nv_manager_get_nv_lte_rel_version() >= LTE_3GPP_REL14 )
  {
    return TRUE;
  }

  return FALSE;
} /* ds_pdn_nv_manager_is_rel_14_or_above_supported */

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_ONLY_REL_11_SUPPORTED

DESCRIPTION
  Determine if UE has release only 11 support or not

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE 
   TRUE - if device is release 11 ready only
   FALSE - Otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_only_rel_11_supported
(
   void
)
{
   return(ds_pdn_nv_manager_is_rel_11_or_above_supported() && 
   	      (!ds_pdn_nv_manager_is_rel_12_or_above_supported()));
}  /*ds_pdn_nv_manager_only_rel_11_supported */


/*===========================================================================
FUNCTION DS_PDN_NV_MANAGGER_READ_UM_WM_THRESHOLD

DESCRIPTION
  This function is used to read um watermark threshold information per
  subscription
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_nv_manager_read_um_wm_threshold
(
  void
)
{
  nv_stat_enum_type  retval;
  ds_bearer_mgr_um_wm_threshold_struct_type  um_wm_threshold = {0}; 
  
  /*-----------------------------------------------------------------------
     Initialize the um wm struct with default WM values
   -----------------------------------------------------------------------*/

  /* GPRS */
#ifdef FEATURE_GSM_GPRS
  ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.gprs_ul_dne = DSGPSD_UL_WM_DEFAULT_DNE;
  ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.gprs_ul_hi  = DSGPSD_UL_WM_DEFAULT_HI;
  ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.gprs_ul_lo  = DSGPSD_UL_WM_DEFAULT_LO;

  ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.gprs_dl_dne = DSGPSD_DL_WM_DEFAULT_DNE;
  ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.gprs_dl_hi  = DSGPSD_DL_WM_DEFAULT_HI;
  ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.gprs_dl_lo  = DSGPSD_DL_WM_DEFAULT_LO;
#endif /*FEATURE_GSM_GPRS*/

  /* LTE */
#ifdef FEATURE_DATA_LTE
  ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.lte_ul_dne = DSLTEPS_UL_WM_DEFAULT_DNE;
  ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.lte_ul_hi  = DSLTEPS_UL_WM_DEFAULT_HI;
  ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.lte_ul_lo  = DSLTEPS_UL_WM_DEFAULT_LO;

  ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.lte_dl_dne = DSLTEPS_DL_WM_DEFAULT_DNE;
  ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.lte_dl_hi  = DSLTEPS_DL_WM_DEFAULT_HI;
  ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.lte_dl_lo  = DSLTEPS_DL_WM_DEFAULT_LO;
#endif /* FEATURE_DATA_LTE */

  /*----------------------------------------------------------------------
     Read the NV item
   -----------------------------------------------------------------------*/
  retval = ds_pdn_nv_manager_read_efs_nv("/nv/item_files/data/3gpp/ds_3gpp_um_wm_threshold",
                           (unsigned char *)(&um_wm_threshold),
                           sizeof(um_wm_threshold));
  if (retval == NV_DONE_S)
  {
    if (TRUE == um_wm_threshold.gprs_wm_levels_valid)
    {

      if ((um_wm_threshold.gprs_ul_dne > um_wm_threshold.gprs_ul_hi) &&
          (um_wm_threshold.gprs_ul_hi > um_wm_threshold.gprs_ul_lo))
      {
        ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.gprs_ul_dne 
                                           = um_wm_threshold.gprs_ul_dne;
        ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.gprs_ul_hi 
                                           = um_wm_threshold.gprs_ul_hi;
        ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.gprs_ul_lo 
                                           = um_wm_threshold.gprs_ul_lo;
      }
      if ((um_wm_threshold.gprs_dl_dne > um_wm_threshold.gprs_dl_hi) &&
          (um_wm_threshold.gprs_dl_hi > um_wm_threshold.gprs_dl_lo))
      {
        ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.gprs_dl_dne 
                                           = um_wm_threshold.gprs_dl_dne;
        ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.gprs_dl_hi 
                                           = um_wm_threshold.gprs_dl_hi;
        ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.gprs_dl_lo 
                                           = um_wm_threshold.gprs_dl_lo;
      }
      ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.
        gprs_wm_levels_valid = TRUE;
    }

    if (TRUE == um_wm_threshold.lte_wm_levels_valid)
    {
      if ((um_wm_threshold.lte_ul_dne > um_wm_threshold.lte_ul_hi) &&
          (um_wm_threshold.lte_ul_hi > um_wm_threshold.lte_ul_lo))
      {
        ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.lte_ul_dne 
                                           = um_wm_threshold.lte_ul_dne;
        ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.lte_ul_hi 
                                           = um_wm_threshold.lte_ul_hi;
        ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.lte_ul_lo 
                                           = um_wm_threshold.lte_ul_lo;
      }
      if ((um_wm_threshold.lte_dl_dne > um_wm_threshold.lte_dl_hi) &&
          (um_wm_threshold.lte_dl_hi > um_wm_threshold.lte_dl_lo))
      {
        ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.lte_dl_dne 
                                           = um_wm_threshold.lte_dl_dne;
        ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.lte_dl_hi 
                                           = um_wm_threshold.lte_dl_hi;
        ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.lte_dl_lo 
                                           = um_wm_threshold.lte_dl_lo;
      }
      ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold.
       lte_wm_levels_valid = TRUE;
    }
  }
  
  return;
}

/*===========================================================================
FUNCTION DS_PDN_NV_GET_UM_WM_THRESHOLD

DESCRIPTION
  This function is used to retrieve um watermark threshold values 
 
  PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  ds_bearer_mgr_um_wm_threshold_struct_type - um wm info

SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_mgr_um_wm_threshold_struct_type  ds_pdn_nv_get_um_wm_threshold
(
  void
)
{
  return ds_pdn_nv_info.ds_bearer_mgr_um_wm_threshold;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGGER_READ_BACK_PRESSURE_PARAMS

DESCRIPTION
  This function is used to read back pressure params
 
  PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_nv_manager_read_back_pressure_params
(
  void
)
{
  nv_stat_enum_type  retval;
  ds_bearer_mgr_tput_opt_struct_type  bp_removal_params = {0}; 
  
  /*----------------------------------------------------------------------
     Read the NV item
   -----------------------------------------------------------------------*/
  retval = ds_pdn_nv_manager_read_efs_nv("/nv/item_files/data/common/tput_opt_params",
                                         (uint8*)(&bp_removal_params),
                           sizeof(ds_bearer_mgr_tput_opt_struct_type));
  if (retval == NV_DONE_S)
  {
    ds_pdn_nv_info.ds_bearer_mgr_tput_opt_params.back_pressure_removal 
                                     = bp_removal_params.back_pressure_removal;
    if(bp_removal_params.timer_val == 0)
    {
      ds_pdn_nv_info.ds_bearer_mgr_tput_opt_params.timer_val = 
                                         	DS_BEARER_THROUGHPUT_TMR_DEFAULT_VAL;
    }
    else
    {
      ds_pdn_nv_info.ds_bearer_mgr_tput_opt_params.timer_val = 
                                                 	bp_removal_params.timer_val;
    }
  }
  else
  {
    ds_pdn_nv_info.ds_bearer_mgr_tput_opt_params.back_pressure_removal =
    	                                                                   TRUE;
    ds_pdn_nv_info.ds_bearer_mgr_tput_opt_params.timer_val = 
                                        	DS_BEARER_THROUGHPUT_TMR_DEFAULT_VAL;
  }
  
  return;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGGER_READ_TCP_THRESHOLD_ACK_CNT

DESCRIPTION
  This function is used to read TCP threshold ack count to disable back pressure.
 
  PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_nv_manager_read_tcp_threshold_ack_cnt
(
  void
)
{
  nv_stat_enum_type  retval;
  uint16             threshold_ack_cnt = 0;
  
  /*----------------------------------------------------------------------
     Read the NV item
   -----------------------------------------------------------------------*/
  retval = ds_pdn_nv_manager_read_efs_nv(
                     "/nv/item_files/modem/data/3gpp/ps/tcp_threshold_ack_cnt",
                     &threshold_ack_cnt,
                     sizeof(uint16));
  if (retval == NV_DONE_S && threshold_ack_cnt != 0)
  {
    ds_pdn_nv_info.ds_bearer_mgr_tput_opt_params.tcp_threshold_ack_cnt 
                                                           = threshold_ack_cnt;
  }
  else
  {
    ds_pdn_nv_info.ds_bearer_mgr_tput_opt_params.tcp_threshold_ack_cnt 
                                                   = TCP_THRESHOLD_ACK_CNT_DEF;
  }
  
  return;
}


/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_BACK_PRESSURE_PARAMS

DESCRIPTION
  This function is used to retrieve back pressure params
 
  PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  ds_bearer_mgr_tput_opt_struct_type - BP info

SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_mgr_tput_opt_struct_type  ds_pdn_nv_get_back_pressure_params
(
  void
)
{
  return ds_pdn_nv_info.ds_bearer_mgr_tput_opt_params;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_multi_pdn_same_apn_nv

DESCRIPTION
  This function reads the multi pdn same apn NV item 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/

void ds_pdn_nv_manager_read_multi_pdn_same_apn_nv
(
  void
)
{
  char  item_path[] = "/nv/item_files/data/3gpp/ds_3gpp_multi_pdn_same_apn";

  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds_pdn_nv_info.allow_multi_pdn_same_apn,
                                   sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.allow_multi_pdn_same_apn = FALSE;
  }
  
  return;
}

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_multi_pdn_same_apn_nv

DESCRIPTION
  This function gets the multi pdn same apn NV item 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_multi_pdn_same_apn_nv
(
  void
)
{
  return ds_pdn_nv_info.allow_multi_pdn_same_apn;
}

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_READ_IPV6_NV

DESCRIPTION   Read & store common IPv6 efs items used by MHs 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_nv_manager_read_ipv6_nv
(
  void
)
{
  nv_item_type                ds_nv_item = {{0}};
  nv_stat_enum_type           status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    determine if IPv6 is enabled in this build.
  -------------------------------------------------------------------------*/
  status = ds_pdn_nv_manager_read_legacy_nv( NV_IPV6_ENABLED_I, &ds_nv_item );
  if( NV_DONE_S == status )
  {
    ds_pdn_nv_info.ipv6_nv_efs_info.is_enabled = ds_nv_item.ipv6_enabled;
  }
  else
  {
    ds_pdn_nv_info.ipv6_nv_efs_info.is_enabled = TRUE;
  }

  /*-----------------------------------------------------------------------
    Read the IP6 State Machine config info
  -----------------------------------------------------------------------*/
  status = ds_pdn_nv_manager_read_legacy_nv( NV_IPV6_SM_CONFIG_I,
                                             &ds_nv_item);
  if (NV_DONE_S != status)
  {
    ds_nv_item.ipv6_sm_config.init_sol_delay   =
                                          IP6_SM_DEFAULT_INIT_SOL_DELAY;
    ds_nv_item.ipv6_sm_config.sol_interval     =
                                            IP6_SM_DEFAULT_SOL_INTERVAL;
    ds_nv_item.ipv6_sm_config.resol_interval   =
                                          IP6_SM_DEFAULT_RESOL_INTERVAL;
    ds_nv_item.ipv6_sm_config.max_sol_attempts =
                                        IP6_SM_DEFAULT_MAX_SOL_ATTEMPTS;
    ds_nv_item.ipv6_sm_config.max_resol_attempts =
                                      IP6_SM_DEFAULT_MAX_RESOL_ATTEMPTS;
    ds_nv_item.ipv6_sm_config.pre_ra_exp_resol_time =
                                   IP6_SM_DEFAULT_PRE_RA_EXP_RESOL_TIME;
  }

  ds_pdn_nv_info.ipv6_nv_efs_info.sm_config.init_sol_delay =
                    ((ds_nv_item.ipv6_sm_config.init_sol_delay > 0) ?
                      ds_nv_item.ipv6_sm_config.init_sol_delay :
                      IP6_SM_DEFAULT_INIT_SOL_DELAY
                    );
  
  ds_pdn_nv_info.ipv6_nv_efs_info.sm_config.sol_interval =
                    ((ds_nv_item.ipv6_sm_config.sol_interval > 0) ?
                      ds_nv_item.ipv6_sm_config.sol_interval :
                      IP6_SM_DEFAULT_SOL_INTERVAL
                    );

  ds_pdn_nv_info.ipv6_nv_efs_info.sm_config.resol_interval =
                    ((ds_nv_item.ipv6_sm_config.resol_interval > 0) ?
                      ds_nv_item.ipv6_sm_config.resol_interval :
                      IP6_SM_DEFAULT_RESOL_INTERVAL
                    );

  ds_pdn_nv_info.ipv6_nv_efs_info.sm_config.max_sol_attempts =
                    ((ds_nv_item.ipv6_sm_config.max_sol_attempts > 0) ?
                      ds_nv_item.ipv6_sm_config.max_sol_attempts :
                      IP6_SM_DEFAULT_MAX_SOL_ATTEMPTS
                    );

  ds_pdn_nv_info.ipv6_nv_efs_info.sm_config.max_resol_attempts =
                    ((ds_nv_item.ipv6_sm_config.max_resol_attempts > 0) ?
                      ds_nv_item.ipv6_sm_config.max_resol_attempts :
                      IP6_SM_DEFAULT_MAX_RESOL_ATTEMPTS
                    );

  ds_pdn_nv_info.ipv6_nv_efs_info.sm_config.pre_ra_exp_resol_time =
                    ((ds_nv_item.ipv6_sm_config.pre_ra_exp_resol_time > 0) ?
                      ds_nv_item.ipv6_sm_config.pre_ra_exp_resol_time :
                      IP6_SM_DEFAULT_PRE_RA_EXP_RESOL_TIME
                    );
  status = ds_pdn_nv_manager_read_legacy_nv( NV_PDSN_AS_PROXY_IPV6_DNS_SERVER_I,
                                             &ds_nv_item );
  if (NV_DONE_S == status)
  {
    ds_pdn_nv_info.ipv6_nv_efs_info.pdsn_as_proxy_ipv6_dns_server
                                    = ds_nv_item.pdsn_as_proxy_ipv6_dns_server;
  }
  else
  {
    ds_pdn_nv_info.ipv6_nv_efs_info.pdsn_as_proxy_ipv6_dns_server 
      = FALSE;
  }

  /*-------------------------------------------------------------------------
    Read the values of DNS addrs from NV and store:
    Get Primary DNS IP addresses from NV.
    Make IP addresses as NULL if NV was never written.
  -------------------------------------------------------------------------*/
  status = ds_pdn_nv_manager_read_legacy_nv(NV_IPV6_PRIMARY_DNS_I, &ds_nv_item);
  if (NV_DONE_S == status)
  {
    /*-----------------------------------------------------------------------
      Store number retrieved from NV.
    -----------------------------------------------------------------------*/
    ds_pdn_nv_info.ipv6_nv_efs_info.primary_dns.in6_u.u6_addr64[0] =
      ds_nv_item.ipv6_primary_dns.prefix;
    ds_pdn_nv_info.ipv6_nv_efs_info.primary_dns.in6_u.u6_addr64[1] =
      ds_nv_item.ipv6_primary_dns.iid;
  }

  /*-------------------------------------------------------------------------
    Get Secondary DNS IP addresses from NV.
    Make IP addresses as NULL if NV was never written.
  -------------------------------------------------------------------------*/
  status = ds_pdn_nv_manager_read_legacy_nv(NV_IPV6_SECONDARY_DNS_I, &ds_nv_item);
  if (NV_DONE_S == status)
  {
    /*-----------------------------------------------------------------------
      Store number retrieved from NV.
    -----------------------------------------------------------------------*/
    ds_pdn_nv_info.ipv6_nv_efs_info.secondary_dns.in6_u.u6_addr64[0] =
      ds_nv_item.ipv6_secondary_dns.prefix;
    ds_pdn_nv_info.ipv6_nv_efs_info.secondary_dns.in6_u.u6_addr64[1] =
      ds_nv_item.ipv6_secondary_dns.iid;
  }

  /*-------------------------------------------------------------------------
    Read IPv6 failover config nv
  -------------------------------------------------------------------------*/
  status = ds_pdn_nv_manager_read_legacy_nv(NV_IPV6_FAILOVER_CONFIG_I, &ds_nv_item);
  if(NV_DONE_S == status)
  {
    ds_pdn_nv_info.ipv6_nv_efs_info.failover_mode = 
                (ds_pdn_ip_ver_failover_e_type)ds_nv_item.ipv6_failover_config;
  }
  else
  {
    ds_pdn_nv_info.ipv6_nv_efs_info.failover_mode = IPV6_PREFERRED;
  }

  /*-------------------------------------------------------------------------
    Get IPv6 IID info from NV.
  -------------------------------------------------------------------------*/
  status = ds_pdn_nv_manager_read_legacy_nv( NV_PS_IPV6_IID_I, &ds_nv_item);
  if (NV_DONE_S == status)
  {
    /*-----------------------------------------------------------------------
      Store number retrieved from NV.
    -----------------------------------------------------------------------*/   
    ds_pdn_nv_info.ipv6_nv_efs_info.iid_info.iid_enum_type =
      IPV6_USER_IID;
    ds_pdn_nv_info.ipv6_nv_efs_info.iid_info.iid_val =
      ds_nv_item.ps_ipv6_iid.iid_val;
  }
  else
  {
    /* NV item not populated; use default */
    ds_pdn_nv_info.ipv6_nv_efs_info.iid_info.iid_enum_type = IPV6_RANDOM_IID;
    ds_pdn_nv_info.ipv6_nv_efs_info.iid_info.iid_val = 0;
  }

  return;
} /* ds_pdn_nv_manager_read_ipv6_nv() */

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_GET_IPV6_SM_CONFIG_INFO

DESCRIPTION   Get IPv6 SM config info configured through NV item

DEPENDENCIES  None

RETURN VALUE  ip6_sm_config_type* - IPv6 SM config info

SIDE EFFECTS  None
===========================================================================*/
ip6_sm_config_type* ds_pdn_nv_manager_get_ipv6_sm_config_info
(
  void
)
{
  return &ds_pdn_nv_info.ipv6_nv_efs_info.sm_config;
} /* ds_pdn_nv_manager_get_ipv6_sm_config_info() */

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_GET_IPV6_PRIMARY_DNS_ADDR

DESCRIPTION   Get IPv6 primary DNS server address

DEPENDENCIES  None

RETURN VALUE  ps_in6_addr* - Ptr containing the primary DNS server addr

SIDE EFFECTS  None
===========================================================================*/
struct ps_in6_addr* ds_pdn_nv_manager_get_ipv6_primary_dns_addr
(
  void
)
{
  return &ds_pdn_nv_info.ipv6_nv_efs_info.primary_dns;
} /* ds_pdn_nv_manager_get_ipv6_primary_dns_addr() */

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_GET_IPV6_SECONDARY_DNS_ADDR

DESCRIPTION   Get IPv6 secondary DNS server address

DEPENDENCIES  None

RETURN VALUE  ps_in6_addr* - Ptr containing the secondary DNS server addr

SIDE EFFECTS  None
===========================================================================*/
struct ps_in6_addr* ds_pdn_nv_manager_get_ipv6_secondary_dns_addr
(
  void
)
{
  return &ds_pdn_nv_info.ipv6_nv_efs_info.secondary_dns;
} /* ds_pdn_nv_manager_get_ipv6_secondary_dns_addr() */

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_IS_IPV6_ENABLED

DESCRIPTION   Returns whether IPv6 is enabled through NV or not.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  TRUE: IPv6 enabled
              FALSE: IPv6 disabled

SIDE EFFECTS  None
===========================================================================*/
boolean ds_pdn_nv_manager_is_ipv6_enabled( void )
{
  return ds_pdn_nv_info.ipv6_nv_efs_info.is_enabled;
} /* ds_pdn_nv_manager_is_ipv6_enabled() */

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_IS_PDSN_AS_PROXY_IPv6_DNS_SERVER

DESCRIPTION   Returns whether PDSN acts as proxy IPv6 DNS server

DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds_pdn_nv_manager_is_pdsn_as_proxy_ipv6_dns_server
(
  void
)
{
  return ds_pdn_nv_info.ipv6_nv_efs_info.pdsn_as_proxy_ipv6_dns_server;
} /* ds_pdn_nv_manager_is_pdsn_as_proxy_ipv6_dns_server() */

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_GET_FAILOVER_MODE

DESCRIPTION   Returns the failover mode set in NV

DEPENDENCIES  None.

RETURN VALUE  Boolean.

SIDE EFFECTS  None.
===========================================================================*/
ds_pdn_ip_ver_failover_e_type ds_pdn_nv_manager_get_failover_mode( void )
{
  return ds_pdn_nv_info.ipv6_nv_efs_info.failover_mode;
} /* ds_pdn_nv_manager_get_failover_mode() */

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_GET_IPV6_IID_INFO

DESCRIPTION   Returns IPv6 iid info

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_nv_manager_get_ipv6_iid_info
(
  ds_pdn_ipv6_iid_type     *iid_info_p
)
{
  iid_info_p->iid_enum_type =
      ds_pdn_nv_info.ipv6_nv_efs_info.iid_info.iid_enum_type;
  iid_info_p->iid_val =
      ds_pdn_nv_info.ipv6_nv_efs_info.iid_info.iid_val;

  return;
} /* ds3g_get_ipv6_iid_info() */

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_redial_no_cause_code_from_nv

DESCRIPTION
  This function reads the redial for no cause from NV item
  NV item: 72563
 
PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_redial_no_cause_code_from_nv
(
  void
)
{
  char item_path[] = "/nv/item_files/modem/data/3gpp/ps/ds_3gpp_redial_for_no_cause";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                          &ds_pdn_nv_info.ds_3gpp_redial_for_no_cause_code,
                                   sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_3gpp_redial_for_no_cause_code = TRUE;
  }
  
  return ;
}/*ds_pdn_nv_manager_read_redial_no_cause_code_from_nv*/

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_redial_no_cause_code_from_nv

DESCRIPTION
  This function reads the redial for no cause from NV item
  NV item: 72563
 
PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_redial_no_cause_code_from_nv
(
  void
)
{
  return ds_pdn_nv_info.ds_3gpp_redial_for_no_cause_code;
}/*ds_pdn_nv_manager_get_redial_no_cause_code_from_nv*/

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_mode_pref_from_nv

DESCRIPTION
  This function reads the redial for no cause from NV item
  NV item: 72563
 
PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_mode_pref_from_nv
(
  void
)
{
  nv_item_type                ds_nv_item = {{0}};
  nv_stat_enum_type           status;
  cm_mode_pref_e_type         cm_mode_pref;

  status = ds_pdn_nv_manager_read_legacy_nv( NV_PREF_MODE_I, &ds_nv_item );
  if( NV_DONE_S == status )
  {
    cm_mode_pref = ds_pdn_nv_manager_convert_nv_mode_to_cm_mode(ds_nv_item.pref_mode.mode);
  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Bad NV read status. Setting mode pref to LTE only");
    cm_mode_pref = CM_MODE_PREF_LTE_ONLY;
  }
  
  dssysmgr_set_mode_pref(cm_mode_pref);
}

/*===========================================================================
FUNCTION      DS3GSUBSMGR_CONVERT_NV_MODE_TO_CM_MODE

DESCRIPTION   This utility function translates the NV mode enum type to 
              CM mode enum.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
cm_mode_pref_e_type ds_pdn_nv_manager_convert_nv_mode_to_cm_mode
(
  nv_mode_enum_type nv_mode_pref
)
{
  cm_mode_pref_e_type cm_mode_pref;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(nv_mode_pref)
  {
    case NV_MODE_DIGITAL_PREF:
    case NV_MODE_DIGITAL_ONLY:
      cm_mode_pref = CM_MODE_PREF_DIGITAL_ONLY;
      break;

    case NV_MODE_ANALOG_PREF:
    case NV_MODE_ANALOG_ONLY:
      cm_mode_pref = CM_MODE_PREF_AMPS_ONLY;
      break;

    case NV_MODE_AUTOMATIC:
      cm_mode_pref = CM_MODE_PREF_AUTOMATIC;
      break;

    case NV_MODE_E911:
      cm_mode_pref = CM_MODE_PREF_EMERGENCY;
      break;

    case NV_MODE_PCS_CDMA_ONLY:
    case NV_MODE_CELL_CDMA_ONLY:
    case NV_MODE_CDMA_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_ONLY;
      break;

    case NV_MODE_HDR_ONLY:
      cm_mode_pref = CM_MODE_PREF_HDR_ONLY;
      break;

    case NV_MODE_CDMA_AMPS_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_AMPS_ONLY;
      break;

    case NV_MODE_GPS_ONLY:
      cm_mode_pref = CM_MODE_PREF_GPS_ONLY;
      break;

    case NV_MODE_GSM_ONLY:
      cm_mode_pref = CM_MODE_PREF_GSM_ONLY;
      break;

    case NV_MODE_WCDMA_ONLY:
      cm_mode_pref = CM_MODE_PREF_WCDMA_ONLY;
      break;

    case NV_MODE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_WLAN_ONLY;
      break;

    case NV_MODE_ANY_BUT_HDR:
      cm_mode_pref = CM_MODE_PREF_ANY_BUT_HDR;
      break;

    case NV_MODE_GSM_WCDMA_ONLY:
      cm_mode_pref = CM_MODE_PREF_GSM_WCDMA_ONLY;
      break;

    case NV_MODE_DIGITAL_LESS_HDR_ONLY:
      cm_mode_pref = CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY;
      break;

    case NV_MODE_CDMA_HDR_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_HDR_ONLY;
      break;

    case NV_MODE_CDMA_AMPS_HDR_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_AMPS_HDR_ONLY;
      break;

    case NV_MODE_CDMA_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_WLAN;
      break;

    case NV_MODE_HDR_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_HDR_WLAN;
      break;

    case NV_MODE_CDMA_HDR_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_HDR_WLAN;
      break;

    case NV_MODE_GSM_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_GSM_WLAN;
      break;

    case NV_MODE_WCDMA_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_WCDMA_WLAN;
      break;

    case NV_MODE_GW_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_GW_WLAN;
      break;

    case NV_MODE_CDMA_AMPS_HDR_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY;
      break;

    case NV_MODE_CDMA_AMPS_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_AMPS_WLAN_ONLY;
      break;

    case NV_MODE_ANY_BUT_HDR_WLAN:
      cm_mode_pref = CM_MODE_PREF_ANY_BUT_HDR_WLAN;
      break;

    case NV_MODE_LTE_ONLY:
      cm_mode_pref = CM_MODE_PREF_LTE_ONLY;
      break;

    case NV_MODE_GWL:
      cm_mode_pref = CM_MODE_PREF_GWL;
      break;

    case NV_MODE_CDMA_LTE_ONLY:
    case NV_MODE_CDMA_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_LTE_ONLY;
      break;

    case NV_MODE_HDR_LTE_ONLY:
    case NV_MODE_HDR_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_HDR_LTE_ONLY;
      break;

    case NV_MODE_GSM_LTE_ONLY:
    case NV_MODE_GSM_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_GSM_LTE;
      break;

    case NV_MODE_WCDMA_LTE_ONLY:
    case NV_MODE_WCDMA_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_WCDMA_LTE;
      break;

    case NV_MODE_CDMA_HDR_LTE_ONLY:
    case NV_MODE_CDMA_HDR_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_HDR_LTE_ONLY;
      break;

    case NV_MODE_CDMA_GSM_LTE_ONLY:
    case NV_MODE_CDMA_GSM_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_GSM_LTE;
      break;

    case NV_MODE_CDMA_WCDMA_LTE_ONLY:
    case NV_MODE_CDMA_WCDMA_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_CDMA_WCDMA_LTE;
      break;

    case NV_MODE_HDR_GSM_LTE_ONLY:
    case NV_MODE_HDR_GSM_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_HDR_GSM_LTE;
      break;

    case NV_MODE_HDR_WCDMA_LTE_ONLY:
    case NV_MODE_HDR_WCDMA_LTE_WLAN_ONLY:
      cm_mode_pref = CM_MODE_PREF_HDR_WCDMA_LTE;
      break;

    case NV_MODE_ANY_BUT_WLAN:
      cm_mode_pref = CM_MODE_PREF_ANY_BUT_WLAN;
      break;

    case NV_MODE_CDMA_GSM_WCDMA:
      cm_mode_pref = CM_MODE_PREF_CDMA_GW;
      break;

    case NV_MODE_CDMA_HDR_GSM_WCDMA:
      cm_mode_pref = CM_MODE_PREF_CDMA_HDR_GW;
      break;

    default:
      cm_mode_pref = CM_MODE_PREF_AUTOMATIC;
      break;
  }
  return cm_mode_pref;
}/* ds3gsubsmgr_convert_nv_mode_to_cm_mode() */

/*===========================================================================
FUNCTION      ds_pdn_nv_manager_efs_trim_white_space

DESCRIPTION   Remove the white spaces at the beginning of the string and at 
              the end of the string. 

PARAMETERS    start: the address of the start pointer of the string.
              end: the address of the end pointer of the string.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_nv_manager_efs_trim_white_space
(
  char    **start,
  char    **end
)
{
  while(**start == ' ' && *start < *end)
  {
    (*start)++;
  }

  while(*(*end-1) == ' ' && *end > *start)
  {
    (*end)--;
  }
} /* ds_pdn_nv_manager_efs_trim_white_space */

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_GET_EFS_APN_STR

DESCRIPTION   This function gets apn string from EFS.

PARAMETERS    char *from, *to - start and end of the character array which
                          holds the apn name
               *apn_name - structure to store apn string


DEPENDENCIES  None

RETURN VALUE  TRUE - get valid APN string
              FALSE - APN string is not valid

SIDE EFFECTS  None
===========================================================================*/
boolean ds_pdn_nv_manager_get_efs_apn_str
(
  char      *from,
  char      *to,
  char      *apn_name
)
{
  uint8   length;
  boolean ret_val = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Remove the extra space */
  /*Note: 'to' points to (end of token+1)*/
  if(NULL == from ||
     NULL == to ||
     NULL == apn_name)
  {
    DS_ERROR_LOG_3("Invalid Input params, From pointer: =x "
                   "To pointer: =x and token id: =x",
                   from, to, apn_name);
    return FALSE;
  }

  ds_pdn_nv_manager_efs_trim_white_space(&from, &to);

  /* Assumption that the token length will not be greater than 255 */
  /*Note: 'to' points to (end of token+1)*/
  length = (uint8)((to-from)/sizeof(char));

  if((from > to) || (length > DS_PROFILE_APN_STRING_LEN))
  {
    ret_val = FALSE;
  }
  else
  {
    (void)memscpy(apn_name, DS_PROFILE_APN_STRING_LEN, from, length);
  }

  return ret_val;
} /* ds_pdn_nv_manager_get_efs_apn_str */

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_APN_OVERRIDE_GET_TOKEN_ID

DESCRIPTION   This function fills the token id i.e. 0 for GSM and 1 for LTE
              associated with each override apn.

PARAMETERS    char *from - start of the token (param name)
              char *to   - end of the token (param name)

DEPENDENCIES  None

RETURN VALUE  0 - success
             -1 - failure

SIDE EFFECTS  None
===========================================================================*/
int8 ds_pdn_nv_manager_apn_override_get_token_id
(
  char  *from,
  char  *to,
  int8  *token_id
)
{
  int16  ret_val = -1; /* return value */
  uint8  length;       /* length of the token (param name) */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(NULL == from ||
     NULL == to ||
     NULL == token_id ||
     (from > to))
  {
    DS_ERROR_LOG_3("Invalid Input params, From pointer: =x "
                   "To pointer: =x and token id: =x",
                   from, to, token_id);
    return ret_val;
  }

  /* Remove the extra space */
  ds_pdn_nv_manager_efs_trim_white_space(&from,&to);

  /* Assumption that the token length will not be greater than 255 */
  length =(uint8)(to-from);
  if (0 == strncmp(from, "GSM",length))
  {
    *token_id = DS_3GPP_OVERRIDE_APN_GSM_RAT;
     ret_val = 0;
  }
  else if (0 == strncmp(from, "LTE_CATM1",length))
  {
    *token_id = DS_3GPP_OVERRIDE_APN_LTE_CATM1_RAT;
     ret_val = 0;
  }
  else if (0 == strncmp(from, "LTE_NB1",length))
  {
    *token_id = DS_3GPP_OVERRIDE_APN_LTE_NB1_RAT;
     ret_val = 0;
  }



  return ret_val;
}/* ds_pdn_nv_manager_apn_override_get_token_id() */

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_PARSE_OVERRIDE_APN

DESCRIPTION   This function will read from the efs file all the necessary
              data that will populate the gsm and lte apn structure.

              EFS File Format

              Param_Name:Param_Val;

              For example -
              GSM:cmiot;

PARAMETERS    efs_db - structure that contains info about the EFS file

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_nv_manager_parse_override_apn
(
  ds_pdn_nv_efs_token_type     *efs_db
)
{
  char                                    *from, *to;
  ds_pdn_nv_efs_token_parse_status_type   ret_val
                                            = DS_PDN_NV_EFS_TOKEN_PARSE_SUCCESS;
  ds_local_err_val_e_type                 local_err
                                            = DS_LOCAL_ERR_VAL_NONE;
  boolean                                 read_param_name = TRUE;
  int8                                    token_id = -1;
  char                                   *efs_read_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Set the seperator as : */
  efs_db->seperator = ':';

  while (DS_PDN_NV_EFS_TOKEN_PARSE_EOF
         != (ret_val = ds_pdn_nv_manager_efs_tokenizer(efs_db, &from, &to )))
  {
    if (DS_PDN_NV_EFS_TOKEN_PARSE_FAILURE == ret_val)
    {
      /* Token Parse Failure */
      local_err = DS_LOCAL_ERR_VAL_0;
      goto func_exit;
    }

    /*------------------------------------------------------------------------
      Token being read. from points to the beginning of the token and
      to points to the (end of the token+1).

      e.g: string= "token;" => from points to 't' and to points to ';'

      The tokenizer automatically skips blank lines and comments (lines
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if(DS_PDN_NV_EFS_TOKEN_PARSE_EOL == ret_val || from == to)
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      if (FALSE == read_param_name &&
          DS_PDN_NV_EFS_TOKEN_PARSE_EOL == ret_val)
      {
        read_param_name = TRUE;
        efs_db->seperator = ':';
      }
      continue;
    }
    else if ( DS_PDN_NV_EFS_TOKEN_PARSE_SUCCESS == ret_val )
    {
      /*---------------------------------------------------------------------
      check if we are looking for param name or param value
      ---------------------------------------------------------------------*/
      if ( read_param_name == TRUE )
      {
        if(ds_pdn_nv_manager_apn_override_get_token_id(from, to, &token_id) == 0)
        {
        /*-----------------------------------------------------------------
          set read_param_name as FALSE This means the next token is a
          param value
          And set the seperator as ';'
          -----------------------------------------------------------------*/
          read_param_name = FALSE;
          efs_db->seperator = ';';
        }
        DS_MSG1(MSG_LEGACY_HIGH, "Token ID: %d", token_id);
      }

      /*---------------------------------------------------------------------
      This means that the token is a param value
      ---------------------------------------------------------------------*/
      else
      {
        if(efs_read_info != NULL)
        {
          DS_SYSTEM_HEAP_MEM_FREE(efs_read_info);
          efs_read_info = NULL;
        }
        DS_SYSTEM_HEAP_MEM_ALLOC(efs_read_info,
                                 DS_PROFILE_APN_STRING_LEN + 1,
                                 char*);

        if(efs_read_info == NULL)
        {
          local_err = DS_LOCAL_ERR_VAL_1;
          goto func_exit; 
        }
        memset(efs_read_info, '\0', DS_PROFILE_APN_STRING_LEN + 1);

        if (TRUE != ds_pdn_nv_manager_get_efs_apn_str (from, to, efs_read_info))
        {
          /* Unable to extract APN String from EFS */
          DS_SYSTEM_HEAP_MEM_FREE(efs_read_info);
          efs_read_info = NULL;
          local_err = DS_LOCAL_ERR_VAL_2;
          goto func_exit;
        }

        switch( token_id )
        {
          /* GSM apn */
          case DS_3GPP_OVERRIDE_APN_GSM_RAT:
          {
            if(ds_pdn_nv_info.override_apn_info[DS_3GPP_OVERRIDE_APN_GSM_RAT] != NULL)
            {
              DS_SYSTEM_HEAP_MEM_FREE(ds_pdn_nv_info.override_apn_info
                                                 [DS_3GPP_OVERRIDE_APN_GSM_RAT]);
              ds_pdn_nv_info.override_apn_info[DS_3GPP_OVERRIDE_APN_GSM_RAT] = NULL;

            }
            if(NULL != efs_read_info)
            {
              ds_pdn_nv_info.override_apn_info[DS_3GPP_OVERRIDE_APN_GSM_RAT] = 
                                                  efs_read_info;
              efs_read_info = NULL;
            }
          }
            break;
          /* LTE CATM1 apn */
          case DS_3GPP_OVERRIDE_APN_LTE_CATM1_RAT:
          {
            if(ds_pdn_nv_info.override_apn_info[DS_3GPP_OVERRIDE_APN_LTE_CATM1_RAT] != NULL)
            {
              DS_SYSTEM_HEAP_MEM_FREE(ds_pdn_nv_info.override_apn_info
                                                    [DS_3GPP_OVERRIDE_APN_LTE_CATM1_RAT]);
              ds_pdn_nv_info.override_apn_info[DS_3GPP_OVERRIDE_APN_LTE_CATM1_RAT] = NULL;
            }
            if(NULL != efs_read_info)
            {
              ds_pdn_nv_info.override_apn_info[DS_3GPP_OVERRIDE_APN_LTE_CATM1_RAT] = 
                                                  efs_read_info;
              efs_read_info = NULL;
            }
          }
            break;
          /* LTE NB1 apn */
          case DS_3GPP_OVERRIDE_APN_LTE_NB1_RAT:
          {
            if(ds_pdn_nv_info.override_apn_info[DS_3GPP_OVERRIDE_APN_LTE_NB1_RAT] != NULL)
            {
              DS_SYSTEM_HEAP_MEM_FREE(ds_pdn_nv_info.override_apn_info
                                                    [DS_3GPP_OVERRIDE_APN_LTE_NB1_RAT]);
              ds_pdn_nv_info.override_apn_info[DS_3GPP_OVERRIDE_APN_LTE_NB1_RAT] = NULL;
            }
            if(NULL != efs_read_info)
            {
              ds_pdn_nv_info.override_apn_info[DS_3GPP_OVERRIDE_APN_LTE_NB1_RAT] = 
                                                    efs_read_info;
              efs_read_info = NULL;
            }
          }

            break;
          default:
            local_err = DS_LOCAL_ERR_VAL_3;
            goto func_exit;
        }
        token_id = DS_3GPP_OVERRIDE_APN_INVALID_RAT;
      }
    }
    else
    {
      local_err = DS_LOCAL_ERR_VAL_4;
      goto func_exit;
    }
  }

  func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_1("ds_pdn_nv_manager_parse_override_apn local_err [=d]",
                    local_err);
  }
  return;
} /* ds_pdn_nv_manager_parse_override_apn() */

/*===========================================================================
FUNCTION  DS_PDN_NV_MANAGER_READ_OVERRIDE_APN_INFO

DESCRIPTION
  This function reads override apn info for GSM and LTE mode
  from the EFS file.

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_override_apn_info()
{
  ds_pdn_nv_efs_token_type       efs_db;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Read EFS file to get the override apn names.
  -------------------------------------------------------------------------*/
  /* Open EFS file */
  if ( ds_pdn_nv_manager_efs_file_init(DS_OVERRIDE_APN_FILE, &efs_db) == 0 )
  {
    ds_pdn_nv_manager_parse_override_apn(&efs_db);

    /* Close EFS file */
    ds_pdn_nv_manager_efs_file_close(&efs_db);
  }
  return;
}/*ds_pdn_nv_manager_read_override_apn_info*/

/*===========================================================================
FUNCTION  DS_PDN_NV_MANAGER_GET_OVERRIDE_APN_PER_RAT

DESCRIPTION
  This function returns the override apn for a rat
  if it is configured in EFS

PARAMETERS
  apn_name - container to store apn_name

DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  if apn is correctly returned.
  FALSE: if something goes wrong.

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_override_apn_per_rat
(
  char                          *apn_name,
  sys_sys_mode_e_type            curr_mode
)
{
  ds_3gpp_override_apn_rat_type  rat_type = DS_3GPP_OVERRIDE_APN_INVALID_RAT;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (curr_mode)
  {
    case SYS_SYS_MODE_GSM :
      rat_type = DS_3GPP_OVERRIDE_APN_GSM_RAT;
    break;

    case SYS_SYS_MODE_LTE :
    {
	  if (SYS_RAT_LTE_NB1_RADIO_ACCESS == ds_pdn_mgr_get_active_rat())
	  {
		rat_type = DS_3GPP_OVERRIDE_APN_LTE_NB1_RAT;
	  }
	  else
	  {
		rat_type = DS_3GPP_OVERRIDE_APN_LTE_CATM1_RAT;
	  }
    }
    break;

    default:
      rat_type = DS_3GPP_OVERRIDE_APN_INVALID_RAT;
  }

  if ( rat_type == DS_3GPP_OVERRIDE_APN_INVALID_RAT ||
  	    !ds_pdn_nv_info.override_apn_info[rat_type])
  {
    DS_ERROR_LOG_0("Invalid pointer or rat type");
    return FALSE;
  }

  strlcpy((char *) apn_name,
          (char *) ds_pdn_nv_info.override_apn_info[rat_type],
          DS_PROFILE_APN_STRING_LEN +1);
  return TRUE;
}/* ds_pdn_nv_manager_get_override_lte_apn */

/*===========================================================================
FUNCTION ds_pdn_nv_manager_read_allow_single_ip_dun_call_dual_ip_profile

DESCRIPTION
  This function reads the NV item # 74047 which contains parameters for 
  allowing single IP DUN call when profile is Dual IP type.

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE  
..None.

SIDE EFFECTS
 .None
===========================================================================*/

void ds_pdn_nv_manager_read_allow_single_ip_dun_call_dual_ip_pf
(
  void
)
{
  char item_path[] = "/nv/item_files/modem/data/3gpp/ps/single_ip_dun_call_in_dual_ip_profile";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
         &(ds_pdn_nv_info.ds_pdn_allow_single_ip_dun_call_dual_ip_profile),
         sizeof(ds_pdn_dun_call_config_type)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_pdn_allow_single_ip_dun_call_dual_ip_profile
                       .allow_single_ip_dun_call_dual_ip_profile = TRUE;
    ds_pdn_nv_info.ds_pdn_allow_single_ip_dun_call_dual_ip_profile
                       .ppp_up_wait_timer_val = 1000;
  }

  return ;                             
} /* ds_pdn_nv_manager_read_allow_single_ip_dun_call_dual_ip_pf */

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_ppp_wait_timer

DESCRIPTION
  This function gets the timer value of ppp_up_wait_timer_val present in
  NV item single_ip_dun_call_in_dual_ip_profile

PARAMETERS 
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  uint16 timer value in ms.
 
SIDE EFFECTS
  None.
===========================================================================*/

uint16 ds_pdn_nv_manager_get_ppp_wait_timer
(
  void
)
{
  return ds_pdn_nv_info.ds_pdn_allow_single_ip_dun_call_dual_ip_profile
            .ppp_up_wait_timer_val;

} /* ds_pdn_nv_manager_get_ppp_wait_timer */

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_allow_single_IP_call_dualIP_pf

DESCRIPTION
  This function gets the NV value of ds_pdn_allow_single_ip_dun_call_dual_ip_profile

PARAMETERS 
  None
  
DEPENDENCIES
  None.

RETURN VALUE
  Boolean Flag indicating whether allow_single_ip_dun_call_dual_ip_profile nv is enabled.
 
SIDE EFFECTS
  None.
 
===========================================================================*/
boolean ds_pdn_nv_manager_get_allow_single_IP_call_dualIP_pf
(
  void
)
{  	 
  return ds_pdn_nv_info.ds_pdn_allow_single_ip_dun_call_dual_ip_profile
            .allow_single_ip_dun_call_dual_ip_profile;
} /* ds_pdn_nv_manager_get_allow_single_IP_call_dualIP_pf */


#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_CREATE_EFS_CONFIG_FILE

DESCRIPTION   Create a config file in EFS which stores the path of
              EFS item files.

DEPENDENCIES  None

RETURN VALUE  0          - Success
              -1         - Non-EFS related Failures
              efs_errno  - EFS related failures. Meaning of this value can be
                           found in fs_errno.h

SIDE EFFECTS  None
===========================================================================*/
int32 ds_pdn_nv_manager_create_efs_config_file
(
  const char *conf_file_path
)
{
  int32  config_fd, result, mcfg_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == conf_file_path )
  {
    return -1;
  }

  /*-------------------------------------------------------------------------
    Create common directories if needed.
  -------------------------------------------------------------------------*/
  result = ds_pdn_nv_path_is_directory("/nv");
  if( 0 != result )
  {
    /* Directory doesn't exist yet */
    result = mcfg_mkdir( "/nv", S_IREAD|S_IWRITE|S_IEXEC, MCFG_FS_TYPE_EFS);
    if ( -1 == result )
    {
      mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
      return mcfg_errno;
    }
  }

  result = ds_pdn_nv_path_is_directory("/nv/item_files");
  if( 0 != result )
  {
    /* Directory doesn't exist yet */
    result = mcfg_mkdir( "/nv/item_files",
                         S_IREAD|S_IWRITE|S_IEXEC,
                         MCFG_FS_TYPE_EFS);
    if ( -1 == result )
    {
      mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
      return mcfg_errno;
    }
  }

  result = ds_pdn_nv_path_is_directory("/nv/item_files/conf");
  if( 0 != result )
  {
    /* Directory doesn't exist yet */
    result = mcfg_mkdir( "/nv/item_files/conf",
                         S_IREAD|S_IWRITE|S_IEXEC,
                         MCFG_FS_TYPE_EFS);
    if ( -1 == result )
    {
      mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
      return mcfg_errno;
    }
  }

  /*-------------------------------------------------------------------------
    Open conf file. Create conf file if does not exist.
    Resulting file is truncated to zero bytes.
  -------------------------------------------------------------------------*/
  config_fd = mcfg_fopen( conf_file_path,
                          MCFG_FS_O_WRONLY | MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE );
  if ( 0 > config_fd )
  {
    mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
    return mcfg_errno;
  }

  result = mcfg_fclose(config_fd, MCFG_FS_TYPE_EFS);
  if ( 0 != result )
  {
    mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
    return mcfg_errno;
  }
  
  return 0;
} /* ds_pdn_nv_manager_create_efs_config_file() */

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_IOT_RPM_ENABLED

DESCRIPTION
  This function is used to read iot rpm enabled value from nv item
  per subscription

PARAMETERS


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_pdn_nv_manager_read_iot_rpm_enabled()
{
  char item_path[] = "/nv/item_files/modem/data/3gpp/iot_rpm_enabled";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
  	                                &ds_pdn_nv_info.ds_iot_rpm_enabled,
  	                                sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_iot_rpm_enabled = FALSE;
  }

  return;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_GSM_RPM_ENABLED

DESCRIPTION
  This function is used to read GSM rpm enabled value from nv item

PARAMETERS


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_pdn_nv_manager_read_gsm_rpm_enabled()
{
  char item_path[] = "/nv/item_files/modem/data/3gpp/gsm_rpm_enabled";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
  	                                &ds_pdn_nv_info.ds_gsm_rpm_enabled,
  	                                sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_gsm_rpm_enabled = FALSE;
  }

  return;
}
/*===========================================================================
FUNCTION  DS_PDN_NV_MANAGER_READ_EFNAS_FAILURE_DEF_RETRY_TIMER 
 
DESCRIPTION
  This Function reads efnas failure default retry timer

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_nv_manager_read_efnas_failure_def_retry_timer 
(
   void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  char item_path[] = "/nv/item_files/modem/data/3gpp/ps/efnas_failure_def_retry_timer_per_rat";

  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                  &ds_pdn_nv_info.ds_3gpp_nv_efnas_read_failure_def_retry_timer,
                                  sizeof(uint32)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_3gpp_nv_efnas_read_failure_def_retry_timer = 
                              DS_PDN_THROT_SPEC_DEF_SM_RETRY_TIMER_VALUE;
  }

}

/*===========================================================================
FUNCTION  DS_PDN_NV_MANAGER_GET_EFNAS_READ_FAILURE_DEF_RETRY_TIMER

DESCRIPTION
  This Function reads efnas_read_failure_def_retry_timer

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
uint32 ds_pdn_nv_manager_get_efnas_read_failure_def_retry_timer
(
   void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return (ds_pdn_nv_info.ds_3gpp_nv_efnas_read_failure_def_retry_timer * 60 * 1000);

}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_IS_IOT_RPM_ENABLED

DESCRIPTION
  This function is used to retrieve it rpm enabled value

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  boolean-iot_rpm_enabled

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_pdn_nv_manager_is_iot_rpm_enabled()
{
  return ds_pdn_nv_info.ds_iot_rpm_enabled;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_IS_GSM_RPM_ENABLED

DESCRIPTION
  This function is used to retrieve GSM rpm enabled value

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  boolean-gsm_rpm_enabled

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_pdn_nv_manager_is_gsm_rpm_enabled()
{
  return ds_pdn_nv_info.ds_gsm_rpm_enabled;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_INIT_EFS_DATA_CONFIG

DESCRIPTION
  Write all required item files to the config file.
  Any new item file must be added directly within this init function.
 
	PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int32 ds_pdn_nv_manager_init_efs_data_config()
{
  char            data_conf_info_path[] = DS_PDN_NV_DATA_CONFIG_PATH;
  int             config_fd, result, mcfg_errno;
  char           *efs_read_buf_ptr = NULL;
  int             bytes_read = 0, efs_ret = 0; 
  boolean         conf_file_valid = TRUE;
  struct fs_stat  stat_buf; 
  int             ret_val = 0;
  char            ds_pdn_nv_item_file_path[] = 
  "/nv/item_files/modem/data/3gpp/lteps/attach_profile\n\
/nv/item_files/modem/data/3gpp/lteps/enable_send_trm_priority\n\
/nv/item_files/modem/data/3gpp/lteps/null_apn_support_for_non_attach_pdn\n\
/nv/item_files/modem/data/3gpp/ps/remove_unused_pdn\n\
/nv/item_files/modem/data/3gpp/ignore_last_pdn_ia_timer\n\
/nv/item_files/modem/data/3gpp/ps/cp_data_window_size\n\
/nv/item_files/modem/data/3gpp/ps/apn_rate_control_config\n\
/nv/item_files/modem/data/3gpp/ps/additional_apn_rate_control_config\n\
/nv/item_files/modem/data/3gpp/ps/dual_ip_fallback\n\
/nv/item_files/modem/data/3gpp/lteps/allow_def_lte_pdn_teardown\n\
/nv/item_files/modem/data/3gpp/lteps/auto_connect_def_pdn\n\
/nv/item_files/modem/data/3gpp/lteps/disable_apn_matching\n\
/nv/item_files/modem/data/3gpp/lteps/switch_apn_on_reject\n\
/nv/item_files/modem/data/3gpp/global_throttling\n\
/nv/item_files/modem/data/3gpp/ps/reg_pdn_throttling\n\
/nv/item_files/modem/data/3gpp/ps/rel_10_throttling_per_plmn\n\
/nv/item_files/modem/data/3gpp/ps/ser_req_throttle_behavior\n\
/nv/item_files/modem/data/3gpp/ds_3gpp_send_ps_data_avail_on_power_save\n\
/nv/item_files/modem/data/3gpp/bcm_validation_on_bearer_mod\n\
/nv/item_files/modem/data/3gpp/umts_nw_initiated_qos_support\n\
/nv/item_files/modem/data/3gpp/umts/enable_multi_pdn_dun\n\
/nv/item_files/modem/data/3gpp/ps/3gpp_rel_version\n\
/nv/item_files/modem/data/3gpp/ps/rel12_qci_supported\n\
/nv/item_files/modem/data/3gpp/ps/3gpp_auth_type_fallback\n\
/nv/item_files/modem/data/3gpp/ps/check_ehplmn_list\n\
/nv/item_files/modem/data/3gpp/call_orig_allowed_before_ps_attach\n\
/nv/item_files/modem/data/dsmgr/config_pdn_level_auth\n\
/nv/item_files/modem/data/3gpp/ps/ds_3gpp_redial_for_no_cause\n\
/nv/item_files/modem/data/3gpp/ps/enable_apn_param_chg\n\
/nv/item_files/data/dsd/ds_apn_switching\n\
/nv/item_files/data/3gpp/max_rab_reestab_retry_cnt\n\
/nv/item_files/data/3gpp/ds_3gpp_mtu\n\
/nv/item_files/data/3gpp/ds_3gpp_um_wm_threshold\n\
/nv/item_files/data/common/tput_opt_params\n\
/nv/item_files/modem/data/3gpp/ps/tcp_threshold_ack_cnt\n\
/nv/item_files/data/3gpp/ds_3gpp_multi_pdn_same_apn\n\
/nv/item_files/modem/data/3gpp/lteps/profile_list\n\
/nv/item_files/modem/data/3gpp/lteps/pdn_conn_limit_handling\n\
/nv/item_files/data/3gpp/access_barring_rab_retry_timer\n\
/nv/item_files/data/3gpp/access_barring_rab_retry_counter\n\
/nv/item_files/modem/data/3gpp/ps/3gpp_data_pkt_orig_str\n\
/nv/item_files/modem/data/3gpp/dual_ip_fallback_cause_codes\n\
/nv/item_files/modem/data/3gpp/ps/home_plmn_list\n\
/nv/item_files/modem/data/3gpp/ps/allow_infinite_throt\n\
/nv/item_files/modem/data/3gpp/ps/rate_ctrl_interval_factor\n\
/nv/item_files/modem/data/3gpp/ps/disable_mtu_request\n\
/nv/item_files/modem/data/3gpp/ps/single_ip_dun_call_in_dual_ip_profile\n\
/nv/item_files/modem/data/3gpp/iot_rpm_enabled\n\
/nv/item_files/modem/data/3gpp/ps/efnas_failure_def_retry_timer_per_rat\n\
/nv/item_files/modem/data/3gpp/ps/rand_backoff_throt\n\
/nv/item_files/modem/data/3gpp/rai_config\n\
/nv/item_files/modem/data/3gpp/gsm_rpm_enabled\n\
/nv/item_files/modem/data/3gpp/fd_config\n";
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Open conf file. If it doesn't exist create it and reopen it.
  -------------------------------------------------------------------------*/
  config_fd = mcfg_fopen (data_conf_info_path,
                          MCFG_FS_O_RDWR,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);
  if ( 0 > config_fd )
  {
    mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
    if ( ENOENT == mcfg_errno ) /* Conf file does not exist */
    {
      conf_file_valid = FALSE;

      result = ds_pdn_nv_manager_create_efs_config_file( data_conf_info_path );
      if ( 0 != result )
      {
        mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
        return mcfg_errno;
      }

      config_fd = mcfg_fopen(data_conf_info_path,
                             MCFG_FS_O_RDWR | MCFG_FS_O_APPEND,
                             MCFG_FS_ALLPERMS,
                             MCFG_FS_TYPE_EFS,
                             MCFG_FS_SUBID_NONE);
      if ( 0 > config_fd )
      {
        mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
        return mcfg_errno;
      }
    }
    else /* Could not open conf file for some other reason */
    {
      mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
      return mcfg_errno;
    }
  }

  /*-------------------------------------------------------------------------
    Check the file size to decide whether writing the file is required or not
  -------------------------------------------------------------------------*/
  if(conf_file_valid)
  {
    if( (mcfg_fstat(config_fd, &stat_buf, MCFG_FS_TYPE_EFS)
           != MCFG_FS_STATUS_OK )
        ||
        (stat_buf.st_size != strlen(ds_pdn_nv_item_file_path)) )
    {
      conf_file_valid = FALSE;
    }
  }

  /*-------------------------------------------------------------------------
    Read the efs config info file if it is valid & compare the contents read
    with the one stored in the dynamic buffer. If they don't match write the
    buffer info into the efs conf file
  -------------------------------------------------------------------------*/
  if(conf_file_valid)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(efs_read_buf_ptr, 
                             strlen(ds_pdn_nv_item_file_path),
                             char*) ;
    if(NULL == efs_read_buf_ptr)
    {
      (void)mcfg_fclose(config_fd, MCFG_FS_TYPE_EFS);
      return -1;
    }

    do
    {
      efs_ret = mcfg_fread(config_fd,
                           &efs_read_buf_ptr[bytes_read],
                           (strlen(ds_pdn_nv_item_file_path) - bytes_read),
                           MCFG_FS_TYPE_EFS);
    } while (efs_ret > 0 &&
             ((bytes_read += efs_ret) < (int)strlen(ds_pdn_nv_item_file_path))
            );

    if( (!(bytes_read == strlen(ds_pdn_nv_item_file_path))) ||
        (0 != memcmp(ds_pdn_nv_item_file_path,
                     efs_read_buf_ptr,
                     strlen(ds_pdn_nv_item_file_path))
        )
      )
    {
      conf_file_valid = FALSE;
    }
  }

  /*-------------------------------------------------------------------------
    Write the efs item file paths into conf file if the contents are not
    valid.
  -------------------------------------------------------------------------*/
  if(FALSE == conf_file_valid)
  {
    mcfg_ftruncate(config_fd, 0, MCFG_FS_TYPE_EFS);
    mcfg_lseek(config_fd,0,SEEK_SET, MCFG_FS_TYPE_EFS);

    if( (int)strlen(ds_pdn_nv_item_file_path) !=
          mcfg_fwrite(config_fd,
                    ds_pdn_nv_item_file_path,
                      (uint32)strlen(ds_pdn_nv_item_file_path),
                      MCFG_FS_TYPE_EFS)
      )
    {
      ret_val = -1;
    }
  }

  DS_SYSTEM_HEAP_MEM_FREE(efs_read_buf_ptr);
   
  /* EFS Close */
  result = mcfg_fclose (config_fd, MCFG_FS_TYPE_EFS);
  if ( 0 != result )
  {
    mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Error closing config file, error %d", mcfg_errno);
    return mcfg_errno;
  }

  return ret_val;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_INIT

DESCRIPTION
  This function initializes the nv manager module. This function is called
  during boot up. This function is responsible for intializing the nv info
  for primary subscription.
 
	PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_pdn_nv_manager_init()
{
  ds_pdn_nv_manager_init_efs_data_config();

#ifdef FEATURE_DATA_LTE

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  ds_pdn_nv_manager_read_null_apn_support_for_non_attach_pdn();
  ds_pdn_nv_manager_read_remove_unused_pdn();
  ds_3gpp_nv_manager_read_apn_param_chg_cfg();
  
#endif

  ds_pdn_nv_manager_read_efnas_config();
  ds_pdn_nv_manager_read_nas_umts_rel_version();
  ds_pdn_nv_manager_read_bcm_validation();
  ds_pdn_nv_manager_read_umts_nw_init_qos_support_nv();
  ds_pdn_nv_manager_read_multi_pdn_dun_nv();  
  ds_pdn_nv_manager_read_efs_3gpp_rel_version();
  ds_pdn_nv_manager_read_rel12_qci_support();
  ds_pdn_nv_manager_read_ciot_cap();
  ds_pdn_nv_manager_read_apm_apn_switching();
  ds_pdn_nv_manager_read_auth_fallback();
  ds_pdn_nv_manager_read_max_rab_reestab_req_retry_cnt();
  ds_pdn_nv_manager_read_check_ehplmn_list_from_nv();
  ds_pdn_nv_manager_read_call_orig_before_ps_attach();
  ds_pdn_nv_manager_read_config_pdn_level_auth();
  ds_pdn_nv_manager_read_mtu_from_nv();
  ds_pdn_nv_manager_read_um_wm_threshold();
  ds_pdn_nv_manager_read_ignore_inactivity_timer();
  ds_pdn_nv_manager_read_cp_data_window_size();
  ds_pdn_nv_manager_read_apn_rate_control_config();
  ds_pdn_nv_manager_read_additional_apn_rate_control_config();
  ds_pdn_nv_manager_read_multi_pdn_same_apn_nv();
  ds_pdn_nv_manager_read_ipv6_nv();
  ds_pdn_nv_manager_read_lte_attach_dual_ip_fb_ip_type();
  ds_pdn_nv_manager_read_allow_default_pdn_teardown();
  ds_pdn_nv_manager_read_always_connect_def_pdn_flag();
  ds_pdn_nv_manager_read_redial_no_cause_code_from_nv();
  ds_pdn_nv_manager_read_disable_apn_matching_from_nv();
  ds_pdn_nv_manager_read_switch_apn_on_reject();
  ds_pdn_nv_manager_read_global_throttling_nv();
  ds_pdn_nv_manager_read_regular_pdn_throttling_nv();
  ds_pdn_nv_manager_read_allow_infinite_throttling_nv();
  ds_pdn_nv_manager_read_enable_pdn_throt_per_rat_nv();
  ds_pdn_nv_manager_read_rel_10_throttling_per_plmn_from_nv();
  ds_pdn_nv_manager_read_ser_req_thor_sm_nv();
  ds_pdn_nv_manager_read_send_ps_data_avail_on_pwr_save();
  ds_pdn_nv_manager_read_back_pressure_params();
  ds_pdn_nv_manager_read_tcp_threshold_ack_cnt();
  ds_pdn_nv_manager_read_send_ps_data_avail_on_pwr_save();
  ds_pdn_nv_manager_read_override_apn_info();
  ds_pdn_nv_manager_read_allow_single_ip_dun_call_dual_ip_pf();
  ds_pdn_nv_manager_read_rate_ctrl_interval_factor();
  ds_pdn_nv_manager_read_disable_mtu_request();
  ds_pdn_nv_manager_read_rand_backoff_throt();
  ds_pdn_nv_manager_read_iot_rpm_enabled();
  ds_pdn_nv_manager_read_rai_config();
#ifdef FEATURE_FAST_DORMANCY
  ds_pdn_nv_manager_read_fd_config();
#endif /* FEATURE_FAST_DORMANCY */
ds_pdn_nv_manager_read_efnas_failure_def_retry_timer();
  ds3g_mmgsdi_read_refresh_vote_ok_nv();

  return;
}

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_DISABLE_MTU_REQUEST

DESCRIPTION
  This function is used to read disable mtu request value from nv item
  per subscription

PARAMETERS


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_nv_manager_read_disable_mtu_request()
{
  char item_path[] = "/nv/item_files/modem/data/3gpp/ps/disable_mtu_request";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
  	                                &ds_pdn_nv_info.ds_3gpp_disable_mtu_request,
  	                                sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_3gpp_disable_mtu_request = FALSE;
  }

  return;
} /* ds_3gpp_nv_manager_read_disable_mtu_request */

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_DISABLE_MTU_REQUEST

DESCRIPTION
  This function is used to retrieve disable mtu request value per subscription

  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  boolean-disable mtu request

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_get_disable_mtu_request()
{
  return ds_pdn_nv_info.ds_3gpp_disable_mtu_request;
  
} /* ds_pdn_nv_manager_get_disable_mtu_request */

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_RAND_BACKOFF_THROT

DESCRIPTION
  This function is used to read random backoff throt from nv item # 74318

PARAMETERS


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_nv_manager_read_rand_backoff_throt()
{
  char item_path[] = "/nv/item_files/modem/data/3gpp/ps/rand_backoff_throt";
  
  if(ds_pdn_nv_manager_read_efs_nv(item_path,
  	                                &ds_pdn_nv_info.ds_pdn_rand_backoff_throt,
  	                                sizeof(boolean)) != NV_DONE_S)
  {
    ds_pdn_nv_info.ds_pdn_rand_backoff_throt = FALSE;
  }

  return;
} /* ds_pdn_nv_manager_read_rand_backoff_throt */

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_RAND_BACKOFF_THROT

DESCRIPTION
  This function is used to get random backoff throttling NV

  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  boolean-disable mtu request

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_get_rand_backoff_throt()
{
  return ds_pdn_nv_info.ds_pdn_rand_backoff_throt;
} /* ds_pdn_nv_manager_get_rand_backoff_throt */


#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

