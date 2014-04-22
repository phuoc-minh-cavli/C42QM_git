/*===========================================================================
                      DS_3GPP_PDN_LIMIT_HDLR.C
DESCRIPTION
This file defines PDN limit throttling APIs.

Copyright (c) 2018 - 2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_3gpp_pdn_limit_hdlr.c#2 $
  $DateTime: 2023/01/19 23:06:43 $$Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd     First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"
#include "ds_eps_pdn_manager.h"
#include "ds_flow.h"
#include "ds_bearer_manager.h"
#include "ds_gprs_bearer_manager.h"
#include "ds_pdn_nv_manager.h"
#include "ds_prof.h"
#include "ds3gmmgsdiif.h"
#include "ds_pdn_hdlr.h"
#include "ps_mem.h"
#include "ds_sys_eventi.h"
#include "dssysmgr.h"

#include "ds_3gpp_pdn_limit_hdlr.h"
#include "ds_3gpp_pdn_throt_spec.h"
#include "ds_3gpp_throttle_sm.h"
#include "ds_3gpp_pdn_throttle_sm.h"
//#include "ds_3gpp_srv_req_throt_sm.h"
#include "ds_3gpp_pdn_throt_rules.h"
#include "ds_3gpp_pdn_throt_config.h"
//#include "ds_3gpp_network_override_hdlr.h"
//#include "ds_3gpp_global_throt_hdlr.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define DEF_PDN_REQ_THROTTLING_TIME 15 /*Minutes*/

#define DS_3GPP_PDN_LIMIT_THROT_DIR_NAME "pdn_limit_throttle"

const char* ds_3gpp_pdn_limit_throttle_file_name = 
                                         {"pdn_limit_throttle_params.txt"};

#define DS_3GPP_PDN_LIMIT_EFS_FLUSH_TIME 90000 /*EFS Flush Time*/
#define DS_3GPP_PDN_LIMIT_EFS_INFO_VALID_COOKIE 0xADBCCBDA

/* This structure stores PDN Connection Limit Information for a particular APN.
 
  max_pdn_conn_per_blk: Specifies the maximum number of PDN connections in a
                        time block that the UE is allowed to perform with
                        the network.
 
  max_pdn_conn_time:    Specifies the time duration in seconds during which
                        the UE counts the PDN connections already made.
 
  pdn_req_wait_time:    Specifies the minimum time interval between the new
                        PDN connection request and the last successful UE
                        initiated PDN disconnection.
 
  time_blk_size:        Specifies the time block duration during which at
                        maximum max_pdn_conn_per_blk successful PDN connection
                        requests are allowed to make for a given PDN from the
                        UE.
 
  pdn_conn_cntr:        This counter counts the number of successful PDN
                        connections since the PDNConnTimer is started.
 
  pdn_conn_tmr:         Timer which will be set to max_pdn_conn_time. 
 
*/

typedef struct
{
  byte                 apn[DS_PROFILE_APN_STRING_LEN+1]; /* Apn name */

  uint16               max_pdn_conn_per_blk; 
  uint16               max_pdn_conn_time; 
  uint16               pdn_req_wait_time;  
  uint16               time_blk_size;
  uint16               pdn_conn_cntr;
  ps_timer_handle_type pdn_conn_tmr;
  void*                client_data_ptr;
}ds_3gpp_pdn_limit_dyn_tbl_s;

typedef struct
{
  ds_3gpp_pdn_limit_dyn_tbl_s* pdn_limit_dyn_tbl[DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES];
} ds_3gpp_pdn_limit_tbl_s; 

typedef struct
{
  void   *data_ptr;
  uint32  data_len;
} ds_3gpp_pdn_limit_efs_data_buf_type;

/*=========================================================================== 
  This is the main structure which stores PDN Conn Limit information for all
  APNs for which PDN Conn Timer, PDN Wait Timer or PDN Throttling Timer is
  running.
=============================================================================*/
typedef struct
{
  ds_3gpp_pdn_limit_efs_info_s efs_info;
  ds_3gpp_pdn_limit_tbl_s      *pdn_limit_tbl;
}ds_3gpp_pdn_limit_info_s;

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

static ds_3gpp_pdn_limit_info_s *ds_3gpp_pdn_limit_info_p={NULL};

static ds_3gpp_pdn_limit_throt_info_type 
                   *ds_3gpp_pdn_limit_throt_info_ptr = {NULL};

/*===========================================================================


                               FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_TBL_ENTRY

  DESCRIPTION
  This function fetches the PDN Limit Table Entry
  
  PARAMETERS  
  None.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PDN Limit Table Entry
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static ds_3gpp_pdn_limit_tbl_s* ds_3gpp_pdn_limit_get_tbl_entry
(
  void
)
{
  if(ds_3gpp_pdn_limit_info_p != NULL)
  {
    return ds_3gpp_pdn_limit_info_p->pdn_limit_tbl;
  }

  return NULL;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_TBL_ENTRY_PER_APN

  DESCRIPTION
  This function fetches the PDN Limit Table Entry at a particular index
  
  PARAMETERS
  1. Index
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PDN Limit Table Entry at a particular index
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static ds_3gpp_pdn_limit_dyn_tbl_s* ds_3gpp_pdn_limit_get_tbl_entry_per_apn
(
  uint8                   index
)
{
  ds_3gpp_pdn_limit_tbl_s*     pdn_limit_tbl_entry_ptr = NULL;
  /*-------------------------------------------------------------------------*/

  pdn_limit_tbl_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry();

  if (pdn_limit_tbl_entry_ptr != NULL &&
      index < DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {
    return pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl[index];
  }

  return NULL;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_CURRENT_PDN_CONN_TMR_VAL

  DESCRIPTION
  This function checks gets the current PDN Connection Timer Value.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Current PDN Conn Timer Value.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_pdn_limit_get_current_pdn_conn_tmr_val
(
  uint8                  index
)
{
  uint32                       current_timer;
  ds_3gpp_pdn_limit_dyn_tbl_s* per_apn_entry_ptr = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return 0;
  }

  current_timer = (uint32)ps_timer_remaining(per_apn_entry_ptr->pdn_conn_tmr);

  DS_MSG2(MSG_LEGACY_HIGH,
          "Current value of PDN Conn Timer at index %d is %d msecs",
         index, current_timer);

  return current_timer;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_VALIDATE_EFS_ENTRY

  DESCRIPTION
  This function validates the Entry in EFS File
  
  PARAMETERS
  pdn_limit_efs_timer_info:               Entry to be validated 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE : If validation successful
  FALSE: Otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_validate_efs_entry
(
  ds_3gpp_pdn_limit_throt_info_pdn_limit_timer_type *pdn_limit_efs_timer_info
)
{
  boolean           ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pdn_limit_efs_timer_info == NULL)
  {
    return ret_val;
  }

  if(pdn_limit_efs_timer_info->throt_info_pdn_limit_timer_type_dyn_p != NULL)
  {
    if((pdn_limit_efs_timer_info->throt_info_pdn_limit_timer_type_dyn_p->cookie
        == DS_3GPP_PDN_LIMIT_EFS_INFO_VALID_COOKIE) &&
       (pdn_limit_efs_timer_info->throt_info_pdn_limit_timer_type_dyn_p->valid
        == TRUE))
    {
      ret_val = TRUE;
    }
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_WRITE_INTO_EFS

  DESCRIPTION
  This function writes information into the EFS Sampling File
  
  PARAMETERS
  write_buf:                  Data to be written into EFS
  write_offset:               Offset at which data needs to be written
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE : If write operation successful
  FALSE: Otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_write_into_efs
(
  ds_3gpp_pdn_limit_efs_data_buf_type      *write_buf,
  uint32                                    write_offset
)
{
  boolean                                    ret_val = FALSE;
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = 
                                               ds_3gpp_pdn_limit_throt_info_ptr;
  int32                                      fd = -1;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(write_buf == NULL)
  {
    return ret_val;
  }
 
  if(pdn_limit_throt_info != NULL)
  {
    fd = mcfg_fopen(pdn_limit_throt_info->efs_file_name,
                    MCFG_FS_O_RDWR,
                    MCFG_FS_ALLPERMS,
                    MCFG_FS_TYPE_EFS,
                    (mcfg_fs_sub_id_e_type)SYS_MODEM_AS_ID_1);

    if(fd != -1)
    {
      if( mcfg_lseek(fd, write_offset, SEEK_SET, MCFG_FS_TYPE_EFS)
           == write_offset )
      {
        if( mcfg_fwrite(fd,
                        write_buf->data_ptr,
                        write_buf->data_len,
                        MCFG_FS_TYPE_EFS)
              == write_buf->data_len )
        {
          ret_val = TRUE;
        }
      }

      if(mcfg_fclose(fd, MCFG_FS_TYPE_EFS) != 0)
      {
        ret_val = FALSE;
      }
    }
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_CREATE_EFS_FILE

  DESCRIPTION
  This function creates the EFS Sampling File
  
  PARAMETERS
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE : If create operation successful
  FALSE: Otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_create_efs_file
(
  void
)
{
  int32                                      fd = -1;
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = 
                                               ds_3gpp_pdn_limit_throt_info_ptr;
  boolean                                    ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pdn_limit_throt_info != NULL)
  {
    fd = mcfg_fopen(pdn_limit_throt_info->efs_file_name,
                    MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC | MCFG_FS_O_WRONLY,
                    MCFG_FS_DEFFILEMODE,
                    MCFG_FS_TYPE_EFS,
                    (mcfg_fs_sub_id_e_type)SYS_MODEM_AS_ID_1);

    if(fd != -1)
    {
      if(mcfg_lseek(fd, 0, SEEK_SET, MCFG_FS_TYPE_EFS) == 0)
      {
        if(mcfg_fclose(fd, MCFG_FS_TYPE_EFS) == 0)
        {
          ret_val = TRUE;
        }
      }
    }
  }
  
  return ret_val;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_CREATE_AND_INIT_PDN_LIMIT_THROT_FILE

  DESCRIPTION
  This function creates and initializes the EFS Sampling File with IMSI Info
  
  PARAMETERS
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE : If operation successful
  FALSE: Otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_create_and_init_pdn_limit_throt_file
(
  void
)
{
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = 
                                              ds_3gpp_pdn_limit_throt_info_ptr;
  ds_3gpp_pdn_limit_efs_data_buf_type        write_data;
  boolean                                    ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pdn_limit_throt_info != NULL)
  {
    if(ds_3gpp_pdn_limit_create_efs_file() == TRUE)
    {
      memset(&write_data, 0, sizeof(ds_3gpp_pdn_limit_efs_data_buf_type));
      
      write_data.data_ptr = &(pdn_limit_throt_info->imsi_data);
      write_data.data_len = sizeof(nv_ehrpd_imsi_type);
      
      ret_val = ds_3gpp_pdn_limit_write_into_efs (&write_data, 0);
    }
    else
    {
      DS_ERROR_LOG_0("Unable to create the File during INIT");
    }
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_READ_INFO_FROM_EFS

  DESCRIPTION
  This function reads the EFS File to gather the persistant throttling info
  
  PARAMETERS
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE : If read operation successful
  FALSE: Otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_read_info_from_efs
(
  void
)
{
  int                                        fd = -1;
  uint8                                      index = 
                                             DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES;
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = 
                                              ds_3gpp_pdn_limit_throt_info_ptr;
  boolean                                    ret_val = FALSE;
  nv_ehrpd_imsi_type                         imsi_info_on_file;
  throt_info_pdn_limit_timer_type_dyn_s      temp_throt_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pdn_limit_throt_info != NULL)
  {
    fd = mcfg_fopen(pdn_limit_throt_info->efs_file_name,
                    MCFG_FS_O_RDWR,
                    MCFG_FS_ALLPERMS,
                    MCFG_FS_TYPE_EFS,
                    (mcfg_fs_sub_id_e_type)SYS_MODEM_AS_ID_1);

    if(fd != -1)
    {
      memset(&imsi_info_on_file, 0, sizeof(nv_ehrpd_imsi_type));
      if(mcfg_fread(fd,
                    &imsi_info_on_file,
                    sizeof(nv_ehrpd_imsi_type),
                    MCFG_FS_TYPE_EFS)
           == sizeof(nv_ehrpd_imsi_type))
      {
        if(memcmp((void *)&imsi_info_on_file,
               (void *)&pdn_limit_throt_info->imsi_data,
               sizeof(nv_ehrpd_imsi_type)) == 0)
        {
          for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
          {
            memset(&temp_throt_info, 0, 
                   sizeof(throt_info_pdn_limit_timer_type_dyn_s));

            if(mcfg_fread(fd,
                    (void *)&temp_throt_info, 
                          sizeof(throt_info_pdn_limit_timer_type_dyn_s),
                          MCFG_FS_TYPE_EFS)
                 == sizeof(throt_info_pdn_limit_timer_type_dyn_s))
            {
              
              if(temp_throt_info.valid == TRUE && 
                 temp_throt_info.cookie == 
                                   DS_3GPP_PDN_LIMIT_EFS_INFO_VALID_COOKIE)
              {
                DS_SYSTEM_HEAP_MEM_ALLOC(
                  pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                  throt_info_pdn_limit_timer_type_dyn_p,
                  sizeof(throt_info_pdn_limit_timer_type_dyn_s),
                  throt_info_pdn_limit_timer_type_dyn_s *) ;

                if(pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                  throt_info_pdn_limit_timer_type_dyn_p != NULL)
                {
                  memset(pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                         throt_info_pdn_limit_timer_type_dyn_p, 0,
                         sizeof(throt_info_pdn_limit_timer_type_dyn_s));

                  memscpy((void *)pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                           throt_info_pdn_limit_timer_type_dyn_p,
                          sizeof(throt_info_pdn_limit_timer_type_dyn_s),
                          (void *)&temp_throt_info,
                          sizeof(throt_info_pdn_limit_timer_type_dyn_s));
                  ret_val = TRUE;
                } /* Dynamic memory allocation successful */
              } /* EFS entry valid for given index */
            } /* EFS read of throt info success */
          } /* For Loop */
        } /* IMSI Comparison success */
      } /* IMSI Read successful */

      if(mcfg_fclose(fd, MCFG_FS_TYPE_EFS) != 0)
      {
        DS_ERROR_LOG_1("Unable to Close File: =d",fd);
      }
    } /* File Descriptor Valid*/
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_CLEAR_EFS_INFO

  DESCRIPTION
  This function clears the EFS File in case the information inside is
  in-valid
  
  PARAMETERS
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_clear_efs_info
(
  void
)
{
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = 
                                               ds_3gpp_pdn_limit_throt_info_ptr;
  ds_3gpp_pdn_limit_efs_data_buf_type        write_buf;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pdn_limit_throt_info != NULL)
  {
    //CLose the File //No need to open file
    memset(&write_buf, 0, sizeof(ds_3gpp_pdn_limit_efs_data_buf_type));

    write_buf.data_ptr = &(pdn_limit_throt_info->imsi_data);
    write_buf.data_len = sizeof(nv_ehrpd_imsi_type);

    ds_3gpp_pdn_limit_write_into_efs(&write_buf, 0);
  }
 }

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_RESET_EFS_ENTRY

  DESCRIPTION
  This function clears the EFS Entry for a Particular APN
  
  PARAMETERS
  apn:                  APN whose entry needs to be cleared
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_reset_efs_entry 
(
 byte                                      *apn
)
{
  uint8                                     index = 
                                            DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES;
  ds_3gpp_pdn_limit_throt_info_type        *pdn_limit_throt_info = 
                                             ds_3gpp_pdn_limit_throt_info_ptr;
  ds_3gpp_pdn_limit_efs_data_buf_type       write_buf;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(pdn_limit_throt_info != NULL)
  {
    for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index ++)
    {
      if(!ds_3gpp_pdn_limit_validate_efs_entry(
         &pdn_limit_throt_info->pdn_limit_timer_tbl[index]))
      {
        continue;
      }

      DS_MSG_SPRINTF_2(MSG_LEGACY_HIGH,
                       "APN Passed: %s & APN Present on Struct: %s",
                       apn,pdn_limit_throt_info->pdn_limit_timer_tbl
                       [index].throt_info_pdn_limit_timer_type_dyn_p->apn_name);

      if(strncasecmp((const char *)apn,
                (const char *)pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                throt_info_pdn_limit_timer_type_dyn_p->apn_name,
                DS_PROFILE_APN_STRING_LEN+1) == 0)
      {
        memset(pdn_limit_throt_info->pdn_limit_timer_tbl[index].
               throt_info_pdn_limit_timer_type_dyn_p, 0, 
               sizeof(throt_info_pdn_limit_timer_type_dyn_s));

        memset(&write_buf, 0, sizeof(ds_3gpp_pdn_limit_efs_data_buf_type));

        write_buf.data_ptr = pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                             throt_info_pdn_limit_timer_type_dyn_p;

        write_buf.data_len = sizeof(throt_info_pdn_limit_timer_type_dyn_s);

        ds_3gpp_pdn_limit_write_into_efs(&write_buf,
                       sizeof(throt_info_pdn_limit_timer_type_dyn_s)*index +
                       sizeof(nv_ehrpd_imsi_type));

        DS_SYSTEM_HEAP_MEM_FREE(
                            pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                                        throt_info_pdn_limit_timer_type_dyn_p);
        break;
      }
    }
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_START_EFS_SAMPLING_TIMER

  DESCRIPTION
  This function starts the EFS Sampling
  
  PARAMETERS
  calculate_sampling_timer:   Flag that tells whether Sampling timer needs
                              to be calculate or not
     
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_start_efs_sampling_timer
(
  boolean                                   calculate_sampling_timer
)
{
  uint8                                     index = 0;
  uint64                                    curr_max_throt_timer = 0;
  ds_3gpp_pdn_limit_throt_info_type        *pdn_limit_throt_info = 
                                               ds_3gpp_pdn_limit_throt_info_ptr;
  uint32                                    pdn_limit_throt_timer_val = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------------ 
   We select the Max throttling timer among all APN's and samples are 1/10th
   of the Max value
   
   If however the sample time is less than EFS Flush Time then we reset the
   sample time to EFS Flush Time
  -----------------------------------------------------------------------*/
  if(pdn_limit_throt_info != NULL)
  {
    if(calculate_sampling_timer == TRUE)
    {
      for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
      {
        if(!(ds_3gpp_pdn_limit_validate_efs_entry(
                           &pdn_limit_throt_info->pdn_limit_timer_tbl[index])))
        {
          continue;
        }

        curr_max_throt_timer = pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                               throt_info_pdn_limit_timer_type_dyn_p->timer_val;

        if(curr_max_throt_timer > pdn_limit_throt_timer_val)
        {
          pdn_limit_throt_timer_val = curr_max_throt_timer;
        }
      }

      if(pdn_limit_throt_timer_val != 0)
      {
        pdn_limit_throt_info->sampling_timer_val = 
                                             (pdn_limit_throt_timer_val/10);

        if(pdn_limit_throt_info->sampling_timer_val < 
                                      DS_3GPP_PDN_LIMIT_EFS_FLUSH_TIME)
        {
          if(DS_3GPP_PDN_LIMIT_EFS_FLUSH_TIME > pdn_limit_throt_timer_val)
          {
            pdn_limit_throt_info->sampling_timer_val = 
                                              pdn_limit_throt_timer_val;
          }
          else
          {
            pdn_limit_throt_info->sampling_timer_val = 
                                       DS_3GPP_PDN_LIMIT_EFS_FLUSH_TIME;
          }
        }

        DS_MSG1(MSG_LEGACY_HIGH, "EFS Sampling Timer value is: %d",
                pdn_limit_throt_info->sampling_timer_val);
      }
      else
      {
        pdn_limit_throt_info->sampling_timer_val = 0;
      }
    }

    ps_timer_start(pdn_limit_throt_info->sampling_timer,
                   (int64)pdn_limit_throt_info->sampling_timer_val);
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_SAMPLING_TIMER_EXP_CMD

  DESCRIPTION
  This function process the Sampling Timer expiry command by:
  1. Writing a new sample to EFS File
  2. Starting the sampling Timer again
  
  PARAMETERS
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_process_sampling_timer_exp_cmd
(
  void
)
{
  uint8                                     index = 
                                            DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES;
  int64                                     curr_time = 0;
  int64                                     time_elapsed = 0;
  int64                                     remaining_throttling_time = 0;
  boolean                                   sampling_needed = FALSE;
  ds_3gpp_pdn_limit_throt_info_type        *pdn_limit_throt_info = 
                                              ds_3gpp_pdn_limit_throt_info_ptr;
  ds_3gpp_pdn_limit_efs_data_buf_type       write_buf;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------- 
   Since we have a single sampling timer running for all APN's
   
   We check whether Time-Elapsed is less than the remaining throttling
   timer for any APN, if not then we dont need to perform any more sampling
   
   Else
   we mark a variable and start the sampling timer
   ----------------------------------------------------------------------*/

  if(pdn_limit_throt_info != NULL)
  {
    for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
    {
      if(!(ds_3gpp_pdn_limit_validate_efs_entry(
         &pdn_limit_throt_info->pdn_limit_timer_tbl[index])))
      {
        continue;
      }

      curr_time = time_get_secs()*(int64)1000;
      time_elapsed = ((int64)curr_time - pdn_limit_throt_info->
                     pdn_limit_timer_tbl[index].
                     throt_info_pdn_limit_timer_type_dyn_p->timestamp);

      remaining_throttling_time = pdn_limit_throt_info->
                                 pdn_limit_timer_tbl[index].
                                 throt_info_pdn_limit_timer_type_dyn_p->
                                 timer_val - (int64)time_elapsed;

      DS_MSG3(MSG_LEGACY_HIGH, 
              "Current Time: %ld, Time Elapsed: %ld, Remaining Throttling"
              " Time: %ld", curr_time, time_elapsed, remaining_throttling_time);

      if((remaining_throttling_time > 0))
      {
        if((time_elapsed > 0))
        {
          pdn_limit_throt_info->pdn_limit_timer_tbl[index].
          throt_info_pdn_limit_timer_type_dyn_p->timer_val -= time_elapsed;
        }
        pdn_limit_throt_info->pdn_limit_timer_tbl[index].
        throt_info_pdn_limit_timer_type_dyn_p->timestamp = curr_time;
        

        DS_MSG2(MSG_LEGACY_HIGH, "Writing following sample in EFS File, "
                "Throttle Timer: %lu & Timestamp: %lu",
                pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                throt_info_pdn_limit_timer_type_dyn_p->timer_val,
                pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                throt_info_pdn_limit_timer_type_dyn_p->timestamp);

        memset(&write_buf, 0, sizeof(ds_3gpp_pdn_limit_efs_data_buf_type));

        write_buf.data_ptr =  pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                              throt_info_pdn_limit_timer_type_dyn_p;
        write_buf.data_len = sizeof(throt_info_pdn_limit_timer_type_dyn_s);

        ds_3gpp_pdn_limit_write_into_efs(&write_buf,
                   sizeof(throt_info_pdn_limit_timer_type_dyn_s)*index +
                   sizeof(nv_ehrpd_imsi_type));

        sampling_needed = TRUE;
      }
    }

    if(sampling_needed == TRUE)
    {
      ds_3gpp_pdn_limit_start_efs_sampling_timer(FALSE);
    }
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PDN_SAMPLING_TIMER_EXP_CB

  DESCRIPTION
  This function posts a cmd to DS task once it gets a cback saying EFS Sampling
  Timer has expired
  
  PARAMETERS
  callback data:              Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_sampling_timer_exp_cb
(
  void*  callback_data                           
)
{
  ds_command_type       *cmd_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if(cmd_ptr == NULL)
  {
    DS_ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_LIMIT_SAMPLING_TIMER_EXP;
  ds_put_cmd(cmd_ptr);
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PDN_REQ_THROT_TMR_START_CB

  DESCRIPTION
  This function posts a cmd to DS task once it gets a cback saying PDN req
  throttling timer is about to start
  
  PARAMETERS
  throt_sm_ptr:               Throttle SM pointer
  callback data:              Current active PS Subs. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_pdn_req_throt_tmr_start_cb
(
  void                         *throt_sm_ptr,
  void                         *callback_data                           
)
{
  ds_command_type                *cmd_ptr = NULL;
  /*------------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if( cmd_ptr == NULL )
  {
    DS_ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_TMR_START;
  cmd_ptr->cmd_payload_ptr = throt_sm_ptr;

  ds_put_cmd(cmd_ptr);
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_PDN_REQ_TMR_START_CMD

  DESCRIPTION
  This function does the required processing when PDN request wait timer
  starts like:
 
  1. Writing into EFS ro make it persistant
  2. Start the sampling timer so that samples are recorded frequently
 
  
  PARAMETERS
  throt_sm_ptr:               Throttle SM pointer
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_process_pdn_req_tmr_start_cmd
(
  void                                     *throt_sm_ptr
)
{
  uint8                                     index = 0;
  uint8                                     first_free_index = 
                                            DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES;
  uint8                                     efs_write_index = 
                                            DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES;
  uint32                                    sampling_time_left = 0;
  boolean                                   efs_write_needed = FALSE;
  ds_3gppi_throttle_sm_s                   *pdn_limit_throt_sm_ptr = NULL;
  ds_3gpp_pdn_limit_throt_info_type        *pdn_limit_throt_info = 
                                               ds_3gpp_pdn_limit_throt_info_ptr;
  ds_3gpp_pdn_limit_efs_data_buf_type       write_buf;
  throt_info_pdn_limit_timer_type_dyn_s    *pdn_limit_efs_info_entry = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  pdn_limit_throt_sm_ptr = (ds_3gppi_throttle_sm_s *)throt_sm_ptr;
  if(pdn_limit_throt_sm_ptr == NULL)
  {
    return;
  }
  
  /*------------------------------------------------------------------------
   We try to locate an Entry for the APN whose throttling timer is about to
   start,
   
   If Entry is found we update EFS File with an Entry for matched APN
   
   Else we create an Entry for the new APN and write it contents in EFS
   
   We also need to make sure to calculate & start the Sampling Timer only
   if it is not running currently.
  -------------------------------------------------------------------------*/
  if(pdn_limit_throt_info != NULL)
  {
    for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
    {
      if(!(ds_3gpp_pdn_limit_validate_efs_entry(
         &pdn_limit_throt_info->pdn_limit_timer_tbl[index])))
      {
        if(first_free_index == DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES)
        {
          DS_MSG1(MSG_LEGACY_HIGH, "First Free Entry found at index: %d",
                  index);
          first_free_index = index;
        }
        continue;
      }

      if(strncasecmp((const char *)pdn_limit_throt_sm_ptr->apn, 
                (const char *)pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                throt_info_pdn_limit_timer_type_dyn_p->apn_name,
                DS_PROFILE_APN_STRING_LEN+1) == 0)
      {
        first_free_index = DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES;

        if((pdn_limit_throt_info->pdn_limit_timer_tbl[index].
          throt_info_pdn_limit_timer_type_dyn_p->timer_val = 
                      pdn_limit_throt_sm_ptr->throt_tmr_val)>0)
        {
          pdn_limit_throt_info->pdn_limit_timer_tbl[index].
          throt_info_pdn_limit_timer_type_dyn_p->timestamp = time_get_secs()*
                                                                  (uint64)1000;

          memset(&write_buf,0, sizeof(ds_3gpp_pdn_limit_efs_data_buf_type));

          write_buf.data_ptr = pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                                throt_info_pdn_limit_timer_type_dyn_p;
          write_buf.data_len = sizeof(throt_info_pdn_limit_timer_type_dyn_s);

          efs_write_index = index;

          efs_write_needed = TRUE;
        }

        break;
      }
    }

    if(first_free_index != DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES)
    {
      DS_SYSTEM_HEAP_MEM_ALLOC(pdn_limit_efs_info_entry,
                               sizeof(throt_info_pdn_limit_timer_type_dyn_s),
                               throt_info_pdn_limit_timer_type_dyn_s *);
      if(pdn_limit_efs_info_entry != NULL)
      {
        memset(pdn_limit_efs_info_entry, 0, 
               sizeof(throt_info_pdn_limit_timer_type_dyn_s));

        pdn_limit_efs_info_entry->cookie = 
                           DS_3GPP_PDN_LIMIT_EFS_INFO_VALID_COOKIE;

        if((pdn_limit_efs_info_entry->timer_val = 
           pdn_limit_throt_sm_ptr->throt_tmr_val)>0)
        {
          pdn_limit_efs_info_entry->timestamp = time_get_secs()*(uint64)1000;
          pdn_limit_efs_info_entry->valid = TRUE;

          memscpy((void *)pdn_limit_efs_info_entry->apn_name,
                  DS_PROFILE_APN_STRING_LEN+1,
                  (void *)pdn_limit_throt_sm_ptr->apn,
                  DS_PROFILE_APN_STRING_LEN+1);

          write_buf.data_ptr = pdn_limit_efs_info_entry;
          write_buf.data_len = sizeof(throt_info_pdn_limit_timer_type_dyn_s);

          efs_write_index = first_free_index;

          efs_write_needed = TRUE;

          pdn_limit_throt_info->pdn_limit_timer_tbl[first_free_index].
            throt_info_pdn_limit_timer_type_dyn_p = pdn_limit_efs_info_entry;
        }
        else
        {
          DS_SYSTEM_HEAP_MEM_FREE(pdn_limit_efs_info_entry);
        }
      }
    }

    if(efs_write_needed)
    {
      (void)ds_3gpp_pdn_limit_write_into_efs(&write_buf,
                 sizeof(throt_info_pdn_limit_timer_type_dyn_s)*efs_write_index +
                 sizeof(nv_ehrpd_imsi_type));

      sampling_time_left = ps_timer_remaining(
                                          pdn_limit_throt_info->sampling_timer);

      DS_MSG1(MSG_LEGACY_HIGH, 
              "Remaining Sampling Timer: %d", sampling_time_left);
      
      if(sampling_time_left == 0)
      {
        ds_3gpp_pdn_limit_start_efs_sampling_timer (TRUE);
      }
    }
  }
}
/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_CALCULATE_REM_THROT_TIME

  DESCRIPTION
  This function calculates the remaining throttling time after boot-up
  
  PARAMETERS  
  index: Index whose timer values need to be used for calculating throt time
  pdn_limit_throt_info: Global EFS Struct containing the EFS Info
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  uint32: Throttling Timer value
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_pdn_limit_calculate_rem_throt_time
(
  uint8                                        index,
  ds_3gpp_pdn_limit_throt_info_type           *pdn_limit_throt_info
)
{
  int64                                        time_rem = 0;
  int64                                        time_elapsed = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pdn_limit_throt_info != NULL)
  {
    DS_MSG4(MSG_LEGACY_HIGH, 
            "Match found at index: %d, "
            "Throtting Timer Value from EFS: %lu & "
            "Timestamp from EFS: %lu, Current Time-Stamp is %lu",
            index,
            pdn_limit_throt_info->pdn_limit_timer_tbl[index].
            throt_info_pdn_limit_timer_type_dyn_p->timer_val,
            pdn_limit_throt_info->pdn_limit_timer_tbl[index].
            throt_info_pdn_limit_timer_type_dyn_p->timestamp,
            time_get_secs()*(uint64)1000);

    time_elapsed = (time_get_secs()*(int64)1000 - 
                    pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                    throt_info_pdn_limit_timer_type_dyn_p->timestamp);

    if(time_elapsed > 0)
    {
      time_rem = (pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                 throt_info_pdn_limit_timer_type_dyn_p->timer_val - 
                 (int64)time_elapsed);
    }
    else
    {
      time_rem = pdn_limit_throt_info->pdn_limit_timer_tbl[index].
        throt_info_pdn_limit_timer_type_dyn_p->timer_val;
    }    

    if((time_rem > 0) == FALSE)
    { 
      time_rem = 0;
    }    
  }

  return time_rem;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_POWER_UP_THROT_HDLR

  DESCRIPTION
  This function performs the Power Up throttling 
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_power_up_throt_hdlr
(
  void
)
{
  uint8                                      index = 0;
  byte                                      *apn = NULL;
  ds_3gpp_pdn_throt_rat_based_s             *rat_based_pdn_throt_p = NULL;
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = 
                                               ds_3gpp_pdn_limit_throt_info_ptr;
  ds_3gpp_pdn_throt_ident_info_type          throt_ident_info;
  uint32                                     time_rem = 0;
  ds_3gpp_throttle_sm_cb_data_type           cb_data;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pdn_limit_throt_info != NULL)
  {
    for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
    {
      /*------------------------------------------------------------- 
        Allocate an Entry for PDN Limit Throttling in Throttle SM
       
        Register for Timer Start, Expiry cback and a cback to retrieve
        throttling timer value
       
        Perform APN Throttling
      -------------------------------------------------------------*/
      if(!ds_3gpp_pdn_limit_validate_efs_entry(
         &pdn_limit_throt_info->pdn_limit_timer_tbl[index]))
      {
        continue;
      }

      apn = pdn_limit_throt_info->pdn_limit_timer_tbl[index].
            throt_info_pdn_limit_timer_type_dyn_p->apn_name;

      DS_MSG_SPRINTF_1(MSG_LEGACY_HIGH,
                       "APN extracted from EFS Info is: %s", apn);

      if((time_rem = 
          ds_3gpp_pdn_limit_calculate_rem_throt_time(index,
                                                     pdn_limit_throt_info)) > 0)
      {
        
        ds_3gpp_throt_fill_cb_data (&cb_data, NULL, NULL, (void*)time_rem);
        ds_3gpp_pdn_throt_fill_ident_info_ex
        (
           &throt_ident_info, 
           apn,
           TRUE,
           DS_3GPP_THROT_RAT_GLOBAL,
           DS_3GPP_PDN_THROT_PROC_GLOBAL,
           DS_3GPP_PDN_THROT_TYPE_PDN_CONN_LIMIT,
           DS_PROFILE_PDP_IPV4V6,
           FALSE,
           DS_NET_DOWN_REASON_NOT_SPECIFIED,
           NULL,
           &cb_data
        );

        rat_based_pdn_throt_p =
          ds_3gpp_pdn_throt_sm_get_throttle_sm_per_rat(&throt_ident_info, TRUE);

        if (rat_based_pdn_throt_p == NULL)
        {
          DS_ERROR_LOG_0("pdn_throt_sm_p is NULL");
          return;
        }

        ds_3gpp_pdn_throt_perform_apn_throttling
          (rat_based_pdn_throt_p,&throt_ident_info);
      }
    }
  }

  return;
}
/*==============================================================================
FUNCTION DS_3GPP_PDN_LIMIT_STORE_IMSI_INFO_AND_INIT_THROT_DATA 
 
DESCRIPTION
  This function processes the IMSI information passed and initializes the EFS File
  and its info 

PARAMETERS 
  imsi_inf          :  IMSI Info from MMGSDI 
 
DEPENDENCIES 
  None. 

RETURN VALUE 
  TRUE: If Successful
  FALSE: Otherwise
 
SIDE EFFECTS 
  None 

==============================================================================*/
boolean ds_3gpp_pdn_limit_store_imsi_info_and_init_throt_data
(
  nv_ehrpd_imsi_type                        *imsi_info
)
{
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = 
                                               ds_3gpp_pdn_limit_throt_info_ptr;
  struct fs_stat                             fs_buf;
  boolean                                    ret_val = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(imsi_info == NULL)
  {
    ret_val = FALSE;
    return ret_val;
  }

  if(pdn_limit_throt_info != NULL)
  {
    memscpy((void *)&pdn_limit_throt_info->imsi_data, 
            sizeof(nv_ehrpd_imsi_type),
            (void *)imsi_info,
            sizeof(nv_ehrpd_imsi_type));

    if( mcfg_fs_stat(DS_3GPP_PDN_LIMIT_THROT_DIR_NAME,
                     &fs_buf,
                     MCFG_FS_TYPE_EFS,
               (mcfg_fs_sub_id_e_type)SYS_MODEM_AS_ID_1) == MCFG_FS_STATUS_OK )
    {
      if( mcfg_fs_stat(pdn_limit_throt_info->efs_file_name,
                       &fs_buf,
                       MCFG_FS_TYPE_EFS,
                (mcfg_fs_sub_id_e_type)SYS_MODEM_AS_ID_1) == MCFG_FS_STATUS_OK )
      {
        /*------------------------------------------------------------- 
         Read IMSI and Compare the IMSI,
           If not same clear the file
           Else perform power-up throttling
         -------------------------------------------------------------*/
        if(ds_3gpp_pdn_limit_read_info_from_efs() == FALSE)
        {
          //Clear the EFS File, Write IMSI info only to File
          ds_3gpp_pdn_limit_clear_efs_info();
        }
        else
        {
          ds_3gpp_pdn_limit_power_up_throt_hdlr();
        }
      }
      else
      {
        //File Not Present, so create and initialize the File with IMSI
        if(ds_3gpp_pdn_limit_create_and_init_pdn_limit_throt_file() == FALSE)
        {
          ret_val = FALSE;
        }
      }
    }
    else
    {
      //No Directory or File Present, create both and initialize File
      if( mcfg_mkdir(DS_3GPP_PDN_LIMIT_THROT_DIR_NAME,
                     S_IREAD|S_IWRITE|S_IEXEC,
                     MCFG_FS_TYPE_EFS) == 0 )
      {
        if(ds_3gpp_pdn_limit_create_and_init_pdn_limit_throt_file() == FALSE)
        {
          ret_val = FALSE;
        }
      }
      else
      {
        ret_val = FALSE;
      }
    }
  }
  else
  {
    ret_val = FALSE; 
  }

  return ret_val;
}

/*==============================================================================
FUNCTION DS_3GPP_PDN_LIMIT_IMSI_INFO_AVAIL_CMD_HDLR
 
DESCRIPTION
  This function process the IMSI information available CMD so that to read IMSI
  information and perform power-up throttling if needed

PARAMETERS 
  None
 
DEPENDENCIES 
  None. 

RETURN VALUE 
  None.
 
SIDE EFFECTS 
  None 

==============================================================================*/
void ds_3gpp_pdn_limit_imsi_info_avail_cmd_hdlr
(
  void
)
{
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = 
                                               ds_3gpp_pdn_limit_throt_info_ptr;
  nv_ehrpd_imsi_type                         imsi_info;
  boolean                                    imsi_info_available = FALSE;
  boolean                                    clean_up_needed = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pdn_limit_throt_info != NULL)
  {
    imsi_info_available = ds3g_mmgsdi_get_imsi_data(&imsi_info);
    if(imsi_info_available == TRUE)
    {
      pdn_limit_throt_info->imsi_data_avail = TRUE;
      if(ds_3gpp_pdn_limit_store_imsi_info_and_init_throt_data(
                                                          &imsi_info) == FALSE)
      {
        DS_MSG0(MSG_LEGACY_HIGH, 
                "Unable to perform EFS operation after IMSI Ready "
                "event cback, cleaning-up");
        clean_up_needed = TRUE;
      }
    }
  }

  if(clean_up_needed)
  {
    if(pdn_limit_throt_info != NULL)
    {
      PS_TIMER_FREE_HANDLE(pdn_limit_throt_info->sampling_timer);
      DS_SYSTEM_HEAP_MEM_FREE(pdn_limit_throt_info);
    }

    ds_3gpp_pdn_limit_throt_info_ptr = NULL;
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PDN_REQ_THROT_TMR_EXPIRY_CB

  DESCRIPTION
  This function is invoked when the PDN Req Throttling Timer Expires.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_pdn_req_throt_tmr_expiry_cb
(
  void* throt_sm_ptr,
  void* callback_data
)
{
  ds_command_type                *cmd_ptr;
  /*------------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if(cmd_ptr == NULL)
  {
    DS_ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_THROT_TMR_EXP;
  cmd_ptr->cmd_payload_ptr = throt_sm_ptr;
  
  ds_put_cmd(cmd_ptr);
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PDN_WAIT_TMR_EXPIRY_CB

  DESCRIPTION
  This function is invoked when the PDN Wait Timer expires.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/

static void ds_3gpp_pdn_limit_pdn_wait_tmr_expiry_cb
(
  void* throt_sm_ptr,
  void* callback_data
)
{
  ds_command_type                *cmd_ptr;
  /*------------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if( cmd_ptr == NULL )
  {
    DS_ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_LIMIT_PDN_WAIT_TMR_EXP;
  cmd_ptr->cmd_payload_ptr = throt_sm_ptr;

  ds_put_cmd(cmd_ptr);
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_PDN_WAIT_TMR_VAL

  DESCRIPTION
  This function returns the PDN Wait Timer Value for a given APN.
  
  PARAMETERS  
  callback data - Throttle SM Ptr from which we obtain APN information
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PDN Wait Timer Value in milli-seconds
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_pdn_limit_get_pdn_wait_tmr_val
(
  void* throt_sm_void_ptr,
  void* callback_data
)
{
  uint8                          index = 0;
  ds_3gppi_throttle_sm_s        *throt_sm_ptr = 
                                   (ds_3gppi_throttle_sm_s*)throt_sm_void_ptr;
  ds_3gpp_pdn_limit_dyn_tbl_s   *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  if ( (index = ds_3gpp_pdn_limit_tbl_find_entry(throt_sm_ptr->apn)) != 
          DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {
    per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

    if (per_apn_entry_ptr == NULL)
    {
      return 0;
    }

    DS_MSG2(MSG_LEGACY_HIGH, "PDN Wait timer value at index %d is %d secs",
            index,
            per_apn_entry_ptr->pdn_req_wait_time);

    return per_apn_entry_ptr->pdn_req_wait_time * 1000;
  }

  return 0;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_THROTTLING_TMR_VAL

  DESCRIPTION
  This function returns the PDN Req Throttling Timer value.
  
  PARAMETERS  
  callback data 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  PDN Req Throttle Timer Value in milli-seconds
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_pdn_limit_get_throttling_tmr_val
(
  void *throt_sm_ptr,
  void* cb_data
)
{
  DS_MSG1(MSG_LEGACY_HIGH, "PDN Req throttling timer val is %d min",
          (uint32)cb_data);
  return (uint32)cb_data;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_LIMIT_CLEAR_ON_LPM

DESCRIPTION
  This function is called check whether throttling can be cleared when
  Mode LPM is triggered

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if throttling can be cleared
  FALSE, otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_pdn_limit_clear_on_lpm 
(
  void* throt_sm_ptr,
  void* user_data
)
{
  return FALSE;
} /* ds_3gpp_pdn_limit_clear_on_lpm */

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_POWER_UP_THROT_INIT

  DESCRIPTION
  This function performs Powerup Initialization to check for Persistant
  Throttling
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_power_up_throt_init 
(
  void
)
{
  ds_3gpp_pdn_limit_throt_info_type    *pdn_limit_throt_info = 
                                              ds_3gpp_pdn_limit_throt_info_ptr;
  struct fs_stat                        fs_buf;
  nv_ehrpd_imsi_type                    imsi_info;
  boolean                               imsi_info_available = FALSE;
  boolean                               clean_up_needed = FALSE;
  ds_3gpp_pdn_throt_sm_tbl_s            info_tbl;
  uint8                                 index;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pdn_limit_throt_info == NULL)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(pdn_limit_throt_info,
                             sizeof(ds_3gpp_pdn_limit_throt_info_type),
                             ds_3gpp_pdn_limit_throt_info_type *);
    if(pdn_limit_throt_info != NULL)
    {
      pdn_limit_throt_info->sampling_timer = 
                        ps_timer_alloc(ds_3gpp_pdn_limit_sampling_timer_exp_cb,
                                       NULL);
    }
  }
  
  /*------------------------------------------------------------------------- 
    Initialize sampling Timer, File name, IMSI Info
   
    If IMSI Info is not present we wait for cback from MMGSDI for IMSI info
   
    Else
    1.  Check if Directory and File Exist
    2.  If they dont initialize the file with IMSI info
    3.  If they do read the appropriate value, compare IMSI and start
        throttling timer if needed
    4.  Throttling Timer gets it value from EFS, make sure that is read locally
        before throttling is started
  ---------------------------------------------------------------------------*/

  do
  {
    if(pdn_limit_throt_info != NULL)
    {
      ds_3gpp_pdn_limit_throt_info_ptr = pdn_limit_throt_info;
      
      info_tbl.tmr_start_f = ds_3gpp_pdn_limit_pdn_req_throt_tmr_start_cb;
      info_tbl.tmr_exp_f = ds_3gpp_pdn_limit_pdn_req_throt_tmr_expiry_cb;
      info_tbl.get_tmr_val_f = ds_3gpp_pdn_limit_get_throttling_tmr_val;
      info_tbl.clear_on_lpm_f = ds_3gpp_pdn_limit_clear_on_lpm;
      info_tbl.attach_is_allowed = TRUE;
      info_tbl.flush_plmn_allowed = TRUE;

      ds_3gpp_pdn_throt_register_sm_info_tbl 
       (&info_tbl, DS_3GPP_PDN_THROT_TYPE_PDN_CONN_LIMIT);

      info_tbl.tmr_start_f = NULL;
      info_tbl.tmr_exp_f = ds_3gpp_pdn_limit_pdn_wait_tmr_expiry_cb;
      info_tbl.get_tmr_val_f = ds_3gpp_pdn_limit_get_pdn_wait_tmr_val;
      info_tbl.clear_on_lpm_f = ds_3gpp_pdn_limit_clear_on_lpm;
      info_tbl.attach_is_allowed = TRUE;
      info_tbl.flush_plmn_allowed = TRUE;

      ds_3gpp_pdn_throt_register_sm_info_tbl 
       (&info_tbl, DS_3GPP_PDN_THROT_TYPE_PDN_REQ_WAIT_TIME);

      memset(pdn_limit_throt_info, 0, 
             sizeof(ds_3gpp_pdn_limit_throt_info_type));

      pdn_limit_throt_info->sampling_timer_val = 0xFFFFFFFF;


      for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
      {
        pdn_limit_throt_info->pdn_limit_timer_tbl[index].
          throt_info_pdn_limit_timer_type_dyn_p = NULL;
      }

      strlcpy(pdn_limit_throt_info->efs_file_name,
              DS_3GPP_PDN_LIMIT_THROT_DIR_NAME,
              strlen(DS_3GPP_PDN_LIMIT_THROT_DIR_NAME)+1);

      pdn_limit_throt_info->efs_file_name[
        strlen(DS_3GPP_PDN_LIMIT_THROT_DIR_NAME)] = '/';

      strlcat(pdn_limit_throt_info->efs_file_name,
              ds_3gpp_pdn_limit_throttle_file_name,
              (strlen(DS_3GPP_PDN_LIMIT_THROT_DIR_NAME)+1+
              strlen(ds_3gpp_pdn_limit_throttle_file_name)+1));

      pdn_limit_throt_info->efs_file_name[\
        strlen(DS_3GPP_PDN_LIMIT_THROT_DIR_NAME) + 1 +\
        strlen(ds_3gpp_pdn_limit_throttle_file_name)] = '\0';

      imsi_info_available = ds3g_mmgsdi_get_imsi_data(&imsi_info);

      memset(&fs_buf, 0, sizeof(struct fs_stat));

      if(imsi_info_available == TRUE)
      {
        pdn_limit_throt_info->imsi_data_avail = imsi_info_available;

        if(ds_3gpp_pdn_limit_store_imsi_info_and_init_throt_data(
                                                           &imsi_info) == FALSE)
        {
          clean_up_needed = TRUE;
        }
      }
    }
  }while(0);

  if(clean_up_needed)
  {
    if(pdn_limit_throt_info != NULL)
    {
      PS_TIMER_FREE_HANDLE(pdn_limit_throt_info->sampling_timer);
      DS_SYSTEM_HEAP_MEM_FREE(pdn_limit_throt_info);
    }
    
    ds_3gpp_pdn_limit_throt_info_ptr = NULL;
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_POWERUP_INIT

  DESCRIPTION
  This function performs Powerup Initialization of the Module
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_powerup_init
(
  void
)
{
  ds_3gpp_pdn_limit_efs_info_s efs_info;  
  char  item_path[] = 
                "/nv/item_files/modem/data/3gpp/lteps/pdn_conn_limit_handling";
  /*-------------------------------------------------------------------------*/
  
  efs_info.pdn_limit_is_enabled = FALSE;
  efs_info.pdn_req_throttling_time = DEF_PDN_REQ_THROTTLING_TIME;

  if (ds_3gpp_pdn_limit_info_p == NULL)
  {
     DS_SYSTEM_HEAP_MEM_ALLOC(ds_3gpp_pdn_limit_info_p,
                             sizeof(ds_3gpp_pdn_limit_info_s),
                             ds_3gpp_pdn_limit_info_s*);
  }

  if (ds_3gpp_pdn_limit_info_p != NULL)
  {    
    if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                     (unsigned char *)(&efs_info),
                                     sizeof(efs_info)) != NV_DONE_S)
    {
      DS_ERROR_LOG_0("Cannot read, use default values");
    }
   
    ds_3gpp_pdn_limit_info_p->efs_info.pdn_limit_is_enabled = 
      efs_info.pdn_limit_is_enabled;
    ds_3gpp_pdn_limit_info_p->efs_info.pdn_req_throttling_time = 
      efs_info.pdn_req_throttling_time;
    ds_3gpp_pdn_limit_info_p->pdn_limit_tbl = NULL;

    ds_3gpp_pdn_limit_power_up_throt_init();
  }
}

/*===========================================================================
  FUNCTION ds_3gpp_pdn_limit_clear_efs_info_entry_tbl

  DESCRIPTION
  This function clears EFS information stored in the internal table
  
  PARAMETERS  
  None
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_clear_efs_info_entry_tbl
(
  void
)
{
  uint8                                      index = 0;
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = 
                                              ds_3gpp_pdn_limit_throt_info_ptr;
  boolean                                    ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DS_MSG0(MSG_LEGACY_HIGH, 
          "Clearing EFS Entry Table for PDN Limit Persistent throt");

  if(pdn_limit_throt_info != NULL)
  {
    for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index ++)
    {
      if(!ds_3gpp_pdn_limit_validate_efs_entry(
         &pdn_limit_throt_info->pdn_limit_timer_tbl[index]))
      {
        continue;
      }

      DS_SYSTEM_HEAP_MEM_FREE(pdn_limit_throt_info->pdn_limit_timer_tbl[index].
                                        throt_info_pdn_limit_timer_type_dyn_p);
    }
    ret_val = TRUE;
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION ds_3gpp_pdn_limit_clear_persistent_throttle_info

  DESCRIPTION
  This function clears PDN Limit persistent throttling information
  
  PARAMETERS  
  None
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_clear_persistent_throttle_info
(
  void
)
{
  boolean                                    ret_val = FALSE;
  ds_3gpp_pdn_limit_throt_info_type         *pdn_limit_throt_info = 
                                               ds_3gpp_pdn_limit_throt_info_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DS_MSG0(MSG_LEGACY_HIGH,
          "Clearing Persistent throttling info for PDN Limit Hdlr");

  if(pdn_limit_throt_info != NULL)
  {
    ps_timer_cancel(pdn_limit_throt_info->sampling_timer);
    PS_TIMER_FREE_HANDLE(pdn_limit_throt_info->sampling_timer);

    if(ds_3gpp_pdn_limit_clear_efs_info_entry_tbl() == FALSE)
    {
      DS_ERROR_LOG_0("Unable to clear entries of EFS info table");
    }

    DS_SYSTEM_HEAP_MEM_FREE(pdn_limit_throt_info);

    ds_3gpp_pdn_limit_throt_info_ptr = NULL;
    ret_val = TRUE;
  }

  return ret_val;
}


/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_IS_ENABLED

  DESCRIPTION
  This function checks whether PDN Limit handling is enabled.
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE if enabled. FALSE otherwise
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_is_enabled
(
  void
)
{
  if (ds_pdn_hdlr_is_ue_in_ac11to15() && 
      ds_pdn_nv_manager_is_rel_11_or_above_supported())
  {
    return FALSE;
  }

  if (ds_3gpp_pdn_limit_info_p != NULL &&
      ds_3gpp_pdn_limit_info_p->efs_info.pdn_limit_is_enabled == TRUE)
  {
    DS_MSG1(MSG_LEGACY_HIGH, "PDN Limit is enabled: %d", 
            ds_3gpp_pdn_limit_info_p->efs_info.pdn_limit_is_enabled);
    return ds_3gpp_pdn_limit_info_p->efs_info.pdn_limit_is_enabled;
  }
  
  return FALSE;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PDN_CONN_TMR_EXPIRY_CB

  DESCRIPTION
  This function is invoked when the PDN Connection Timer Expires.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_pdn_conn_tmr_expiry_cb
(
  void* callback_data
)
{
  ds_command_type *cmd_ptr = NULL;
  /*------------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if( cmd_ptr == NULL )
  {
    DS_ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_LIMIT_PDN_CONN_TMR_EXP;
  cmd_ptr->cmd_payload_ptr = (void*)callback_data;

  ds_put_cmd(cmd_ptr);
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_ALLOC_DYN_MEM

  DESCRIPTION
  This function allocates an entry in the PDN Limit Table.
  
  PARAMETERS  
  None.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Index of the entry into the PDN Limit Table
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint8 ds_3gpp_pdn_limit_alloc_dyn_mem
(
  void
)
{
  uint32                       index = 0;
  ds_3gpp_pdn_limit_tbl_s     *pdn_limit_tbl_entry_ptr = NULL;
  ds_3gpp_pdn_limit_dyn_tbl_s *apn_based_tbl_ptr  = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(ds_3gpp_pdn_limit_info_p->efs_info.pdn_limit_is_enabled == TRUE);

  pdn_limit_tbl_entry_ptr = ds_3gpp_pdn_limit_info_p->pdn_limit_tbl;

  if(pdn_limit_tbl_entry_ptr == NULL)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(pdn_limit_tbl_entry_ptr,
                             sizeof(ds_3gpp_pdn_limit_tbl_s),
                             ds_3gpp_pdn_limit_tbl_s*);
    ds_3gpp_pdn_limit_info_p->pdn_limit_tbl = pdn_limit_tbl_entry_ptr;
    if (pdn_limit_tbl_entry_ptr != NULL)
    {
      DS_MSG0(MSG_LEGACY_HIGH, "PDN Limit table allocated");
      memset(pdn_limit_tbl_entry_ptr, 0, sizeof(ds_3gpp_pdn_limit_tbl_s));
    }
    else
    {
      DS_ASSERT(0);
      return DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY;
    }
  }

  for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
  {
    if(pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl[index] == NULL)
    {
      DS_SYSTEM_HEAP_MEM_ALLOC(pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl[index],
                               sizeof(ds_3gpp_pdn_limit_dyn_tbl_s),
                               ds_3gpp_pdn_limit_dyn_tbl_s*);
      apn_based_tbl_ptr = pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl[index];

      if (apn_based_tbl_ptr != NULL)
      {
        DS_MSG1(MSG_LEGACY_HIGH,
                "PDN Limit table dyn memory allocated at index: %d", index);
        memset(apn_based_tbl_ptr , 0, sizeof(ds_3gpp_pdn_limit_dyn_tbl_s));

        apn_based_tbl_ptr ->client_data_ptr = (void*)index;

        apn_based_tbl_ptr ->pdn_conn_tmr = ps_timer_alloc(
                                      ds_3gpp_pdn_limit_pdn_conn_tmr_expiry_cb,
                                      (void*)apn_based_tbl_ptr);
      }
      else
      {
        DS_ASSERT(0);
        return DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY;
      }
      break;
    }
  }

  if (index == DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES)
  {
    DS_ERROR_LOG_0("Max PDN Limit TBL entries have been allocated");
  }

  return (uint8)index;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_FREE_TBL_ENTRY_PER_APN

  DESCRIPTION
  This function frees the PDN Limit Table Entry at a particular index and
  for a particular subscription
  
  PARAMETERS
  1. Index
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_free_tbl_entry_per_apn
(
  uint8                   index
)
{
  ds_3gpp_pdn_limit_tbl_s* pdn_limit_tbl_entry_ptr = NULL;
  /*-------------------------------------------------------------------------*/

  do
  {
    pdn_limit_tbl_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry();

    if (pdn_limit_tbl_entry_ptr == NULL)
    {
      break;
    }

    if (index >= DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
    {
      break;
    }

    PS_TIMER_FREE_HANDLE(
              pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl[index]->pdn_conn_tmr);

    DS_SYSTEM_HEAP_MEM_FREE(pdn_limit_tbl_entry_ptr->pdn_limit_dyn_tbl[index]);
  }while (0); 

  return;

}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_FREE_DYN_MEM

  DESCRIPTION
  This function frees an entry in the PDN Limit Table.
  
  PARAMETERS  
  Index of the entry into the PDN Limit Table
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_free_dyn_mem
(
  uint8                  index
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  uint8                        i;
  /*-------------------------------------------------------------------------*/

  ds_3gpp_pdn_limit_free_tbl_entry_per_apn(index);

  DS_MSG1(MSG_LEGACY_HIGH, "PDN Limit Table memory freed at index: %d", index);

  for (i=0; i< DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; i++)
  {
    per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(i);
    if(per_apn_entry_ptr != NULL)
    {
      break;
    }
  }

  if (i == DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Free the PDN Limit Table");

    DS_SYSTEM_HEAP_MEM_FREE(ds_3gpp_pdn_limit_info_p->pdn_limit_tbl);
  }

  return; 
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_MAX_PDN_CONN_PER_BLK

  DESCRIPTION
  This function fetches Max PDN Conn allowed per Block for a Given APN.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Max PDN Conn allowed per Time Block.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint16 ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_per_blk
(
  uint8                  index
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return 0;
  }

  DS_MSG2(MSG_LEGACY_HIGH, "Max PDN Conn Per Blk for index %d is %d",
          index, per_apn_entry_ptr->max_pdn_conn_per_blk);

  return per_apn_entry_ptr->max_pdn_conn_per_blk;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_TIME_BLK_SIZE

  DESCRIPTION
  This function fetches the time block size in mill-seconds.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Time Block Size in milli-secs.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_pdn_limit_tbl_get_time_blk_size
(
  uint8                  index
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return 0;
  }

  DS_MSG2(MSG_LEGACY_HIGH,
          "Time Blk size for index %d is %d secs",
          index, per_apn_entry_ptr->time_blk_size);

  return per_apn_entry_ptr->time_blk_size * 1000;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_CURRENT_TIME_BLK

  DESCRIPTION
  This function returns the Current time block based on the Current
  PDN Connection Timer Value.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  Time in mill-secs for which the PDN Conn Timer has run.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Current Timer Block
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_pdn_limit_tbl_get_current_time_blk
(
  uint8                  index,
  uint32                 time_in_msec
)
{
  uint32 current_time_blk = 0;
  uint32 current_time_blk_size = 0;
  /*-------------------------------------------------------------------------*/

  if (index >= DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {
    return 0;
  }

  current_time_blk_size = ds_3gpp_pdn_limit_tbl_get_time_blk_size(index);
  if (current_time_blk_size != 0)
  {
    current_time_blk = (time_in_msec + current_time_blk_size - 1) 
                         / current_time_blk_size;
  }

  return current_time_blk;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_IS_PDN_REQ_WAIT_TIMER_IN_PROGRESS

  DESCRIPTION
  This function checks whether the PDN Req Wait Timer is running.
  
  PARAMETERS  
  Index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE or FALSE based on whether PDN Wait Timer is running
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_is_pdn_req_wait_timer_in_progress
(
  uint8                  index
)
{
  ds_3gpp_pdn_throt_rat_based_s   *rat_based_pdn_throt_p = NULL;
  byte                            *apn = NULL;
  ds_3gpp_pdn_limit_dyn_tbl_s     *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return FALSE;
  }

  apn = per_apn_entry_ptr->apn;

  rat_based_pdn_throt_p = ds_3gpp_pdn_throt_sm_get_throttle_sm
                  (apn, TRUE, FALSE, DS_3GPP_PDN_THROT_TYPE_PDN_REQ_WAIT_TIME);

  // Could not find PDN Req throttling SM. This means that throttling is not in
  // progress. 
  if (rat_based_pdn_throt_p == NULL)
  {
    return FALSE;
  }

  DS_MSG1(MSG_LEGACY_HIGH, 
          "PDN Req Wait Timer is in progress for index %d", index);
  return TRUE;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_GET_MAX_PDN_CONN_ALLOWED_IN_GIVEN_TIME

  DESCRIPTION
  This function fetches the Max PDN Connections allowed at any instant of time.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  Current value of PDN Con Timer in mill-secs.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Max PDN Connections allowed in given time.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_allowed_in_given_time
(
  uint8                  index,
  uint32                 time_in_msec
)
{
  uint32 max_pdn_conn_allowed = 0;
  uint32 current_time_blk = 0;
  /*-------------------------------------------------------------------------*/

  if (index >= DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {
    return 0;
  }

  current_time_blk = 
    ds_3gpp_pdn_limit_tbl_get_current_time_blk(index, time_in_msec);

  max_pdn_conn_allowed =  
    current_time_blk * ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_per_blk(index);

  DS_MSG3(MSG_LEGACY_HIGH, 
          "Max PDN Conns allowed in current time %d msecs for index %d is %d", 
          time_in_msec, index, max_pdn_conn_allowed);

  return max_pdn_conn_allowed;

}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_FIND_ENTRY

  DESCRIPTION
  This function finds entry corresponding at a given APN in PDN Limit Table.
  
  PARAMETERS  
  APN Name
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Index into the PDN limit Table.
  Returns DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY if cannot find an entry.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint8 ds_3gpp_pdn_limit_tbl_find_entry
(
  byte                   *apn
)
{
  uint8                        loop = DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY;
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  if (ds_3gpp_pdn_limit_is_enabled())
  {
    for( loop = 0; loop < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; loop++)
    {
      per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(loop);

      if (per_apn_entry_ptr != NULL)
      {
        if(strncasecmp((char*)per_apn_entry_ptr->apn, 
                       (char *)apn,
                        DS_PROFILE_APN_STRING_LEN+1) == 0)
        {
          DS_MSG1(MSG_LEGACY_HIGH, "Found entry %d in PDN Limit Tbl", loop);
          break;
        }
      }
    }
  }
  
  return loop;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_FIND_OR_ADD_ENTRY

  DESCRIPTION
  This function tries to finds an entry corresponding to a given APN in the
  PDN Limit Table. If it cannot find an entry it creates a new entry in the
  PDN Limit Table.
  
  PARAMETERS  
  APN Name
  Max PDN Connections Per Block.
  Max PDN Connection Time.
  PDN Req Wait Time.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Index into the PDN limit Table.
  Returns DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY if it cannot make an entry.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint8 ds_3gpp_pdn_limit_tbl_find_or_add_entry
(
  byte                   *apn,
  uint16                  max_pdn_conn_per_blk,
  uint16                  max_pdn_conn_time,
  uint16                  pdn_req_wait_time
)
{
  uint8                        index = DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY;
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  if(ds_3gpp_pdn_limit_is_enabled() == FALSE)
  {
    return index;
  }

  index = ds_3gpp_pdn_limit_tbl_find_entry(apn);

  //Could not find an entry corresponding to the APN in PDN Limit TBL
  if (index == DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {
    //Allocate a new entry in the PDN Limit TBL
    index = ds_3gpp_pdn_limit_alloc_dyn_mem();

    //Successfully allocated a new entry in PDN Limit TBL
    if (index != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
    {
      per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);
      if (per_apn_entry_ptr != NULL)
      {
        (void) strlcpy((char *) per_apn_entry_ptr->apn, 
                       (char *) apn, 
                       DS_PROFILE_APN_STRING_LEN+1);

        per_apn_entry_ptr->max_pdn_conn_per_blk = max_pdn_conn_per_blk;
        per_apn_entry_ptr->max_pdn_conn_time = max_pdn_conn_time;
        per_apn_entry_ptr->pdn_req_wait_time = pdn_req_wait_time;
        per_apn_entry_ptr->time_blk_size = max_pdn_conn_time;
      }
      else
      {
        return DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY;
      }
    }
  }

  return index;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_APN_NAME

  DESCRIPTION
  This function updates the APN Name in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New APN name.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_tbl_update_apn_name
(
  uint8                  index,
  char*                  apn
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return;
  }

  DS_MSG_SPRINTF_2(MSG_LEGACY_HIGH,
                   "APN name: %s updated for index %d in PDN Limit Tbl",
                   apn, index);

  strlcpy((char *) per_apn_entry_ptr->apn, apn, DS_PROFILE_APN_STRING_LEN+1);
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_IS_PDN_REQ_THROTTLING_IN_PROGRESS

  DESCRIPTION
  This function checks whether the PDN Req Throttling Timer is running.
  
  PARAMETERS  
  Index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE or FALSE based on whether PDN is throttled
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_is_pdn_req_throttling_in_progress
(
  uint8                   index
)
{
  ds_3gpp_pdn_throt_rat_based_s   *rat_based_pdn_throt_p = NULL;
  byte                            *apn = NULL;
  ds_3gpp_pdn_limit_dyn_tbl_s     *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return FALSE;
  }

  apn = per_apn_entry_ptr->apn;

  rat_based_pdn_throt_p = ds_3gpp_pdn_throt_sm_get_throttle_sm
                      (apn, TRUE, FALSE, DS_3GPP_PDN_THROT_TYPE_PDN_CONN_LIMIT);

  // Could not find PDN Req throttling SM. This means that throttling is not in
  // progress. 
  if (rat_based_pdn_throt_p == NULL)
  {
    return FALSE;
  }

  DS_MSG1(MSG_LEGACY_HIGH, 
          "PDN Req Throttling is in progress for index %d", index);
  return TRUE;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_VALIDATE_CLEAR_ENTRY

  DESCRIPTION
  This function determines whether the entry corresponding to the given
  index can be cleared from the PDN limit table.
  
  PARAMETERS
  1. Index into the PDN Limit Table.
 
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static boolean ds_3gpp_pdn_limit_validate_clear_entry
(
  uint8                   index
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s* per_apn_entry_ptr = NULL;
  boolean                      clear_entry = TRUE;
  ds_pdn_mgr_s                *v4_pdn_cntx_p = NULL;
  ds_pdn_mgr_s                *v6_pdn_cntx_p = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);
  
  do
  {
    /* If no entry is found for the index, subs id passed in,
       we have nothing to clear*/
    if (per_apn_entry_ptr == NULL)
    {
      clear_entry = FALSE;
      break;
    }

    /* If Throttling is in progress or PDN Connection Timer is running,
       dont clear the entry*/
    if (ds_3gpp_pdn_limit_is_pdn_req_throttling_in_progress(index) ||
        ds_3gpp_pdn_limit_is_pdn_req_wait_timer_in_progress(index) ||
        ds_3gpp_pdn_limit_get_current_pdn_conn_tmr_val(index) != 0)
    {
      clear_entry = FALSE;
      break;
    }

    /* If a V4 or a V6 PDN Context exists for the give APN, do not clear the entry.
       This is done to ensure that when the PDN Connection is terminated,
       PDN Wait Timer can be started */
    if (ds_apn_table_get_v4_pdn((char*)per_apn_entry_ptr->apn, &v4_pdn_cntx_p) ||
        ds_apn_table_get_v6_pdn((char*)per_apn_entry_ptr->apn, &v6_pdn_cntx_p))
    {
      clear_entry = FALSE;
      break;
    }
    else
    {
      clear_entry = TRUE;
    }
  }
  while (0);

  return clear_entry;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_REMOVE_ENTRY
  DESCRIPTION
  This function removes entry at index 0 from PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_tbl_remove_entry
(
  uint8                  index
)
{
  if (ds_3gpp_pdn_limit_validate_clear_entry(index))
  {
    ds_3gpp_pdn_limit_free_dyn_mem(index);
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_PDN_CONN_TMR_EXPIRY_CMD

  DESCRIPTION
  This function processes the PDN Connection Timer expiry event.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_process_pdn_conn_tmr_expiry_cmd
(
  void *cb_data
)
{
  uint32                       index = 0;
  ds_3gpp_pdn_limit_dyn_tbl_s *pdn_limit_dyn_tbl_entry_ptr = 
                                        (ds_3gpp_pdn_limit_dyn_tbl_s*)cb_data;
  /*-------------------------------------------------------------------------*/

  if(pdn_limit_dyn_tbl_entry_ptr == NULL)
  {
    return;
  }

  index = (uint32)pdn_limit_dyn_tbl_entry_ptr->client_data_ptr;
  DS_MSG1(MSG_LEGACY_HIGH, "PDN Conn Timer expired at index %d", index);

  pdn_limit_dyn_tbl_entry_ptr->pdn_conn_cntr = 0;
  ds_3gpp_pdn_limit_tbl_remove_entry(index);
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_MAX_PDN_CONN_TIME

  DESCRIPTION
  This function updates the Max PDN Conn Time in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New Max PDN Conn Time
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_update_max_pdn_conn_time
(
  uint8                  index,
  uint16                 max_pdn_conn_time
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return;
  }

  per_apn_entry_ptr->max_pdn_conn_time = max_pdn_conn_time;
  per_apn_entry_ptr->time_blk_size = max_pdn_conn_time;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_MAX_PDN_CONN_PER_BLOCK

  DESCRIPTION
  This function updates the Max PDN Conn Per Block in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New Max PDN Conn Per Block.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_update_max_pdn_conn_per_blk
(
  uint8  index,
  uint16 max_pdn_conn_per_blk
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return;
  }

  per_apn_entry_ptr->max_pdn_conn_per_blk = max_pdn_conn_per_blk;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_TBL_UPDATE_PDN_REQ_WAIT_TIME

  DESCRIPTION
  This function updates the PDN Req Wait Time in PDN Limit Table.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  New PDN Req Wait Time.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_update_pdn_req_wait_time
(
  uint8  index,
  uint16 pdn_req_wait_time
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return;
  }

  per_apn_entry_ptr->pdn_req_wait_time = pdn_req_wait_time;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_MAX_PDN_CONN_TIME

  DESCRIPTION
  This function gets thE Max PDN Conn Time in milli-seconds.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Max PDN Conn Time in milli-seconds.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static uint32 ds_3gpp_pdn_limit_get_max_pdn_conn_time
(
  uint8                   index
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return 0;
  }

  DS_MSG2(MSG_LEGACY_HIGH, "PDN Conn Timer value at index %d is %d secs",
          index, per_apn_entry_ptr->max_pdn_conn_time);

  return per_apn_entry_ptr->max_pdn_conn_time * 1000;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_START_PDN_CONN_TMR

  DESCRIPTION
  This function starts the PDN Conn Timer if the PDN throttling timer
  is not running and if the PDN Conn Timer has not yet started.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_start_pdn_conn_tmr
(
  uint8                   index
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "Starting PDN Conn Timer at index %d", index);

  if (ds_3gpp_pdn_limit_is_pdn_req_throttling_in_progress(index) == FALSE )
  {
    ps_timer_start(per_apn_entry_ptr->pdn_conn_tmr,
                   (int64)ds_3gpp_pdn_limit_get_max_pdn_conn_time(index));
  }
}

/*===========================================================================
FUNCTION  DS_3GPP_PDN_LIMIT_CLR_PDN_CONN_TMR 

DESCRIPTION
  This function clears the PDN connection timer

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_limit_clr_pdn_conn_tmr
(
   uint8                             index
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s        *per_apn_entry_ptr  = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return;
  }

  ps_timer_cancel(per_apn_entry_ptr->pdn_conn_tmr);

}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_STOP_SAMPLING_TIMER_IF_NEEDED

  DESCRIPTION
  This function stops the sampling timer if no APN's are being throttled
  
  PARAMETERS  
  None
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_stop_sampling_timer_if_needed
(
  void
)
{
  ds_3gpp_pdn_limit_throt_info_type     *pdn_limit_throt_info = 
                                               ds_3gpp_pdn_limit_throt_info_ptr;
  uint8                                  index = 
                                              DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pdn_limit_throt_info != NULL)
  {
    for(index = 0; index < DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES; index++)
    {
      if(ds_3gpp_pdn_limit_validate_efs_entry(&pdn_limit_throt_info->
                                               pdn_limit_timer_tbl[index]))
      {
        break;
      }
    }

    if(index == DS_3GPP_PDN_LIMIT_TBL_MAX_ENTRIES)
    {
      DS_MSG0(MSG_LEGACY_HIGH, 
             "Stopping Sampling Timer since Throtting Timer Exp");
      ps_timer_cancel(pdn_limit_throt_info->sampling_timer);
    }
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_PDN_REQ_THROT_TMR_EXPIRY_CMD

  DESCRIPTION
  This function processes the PDN Req Throttle Timer Expiry Cmd.
  
  PARAMETERS  
  Callback data. This will be the index into the PDN Limit Table.
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_process_pdn_req_throt_tmr_expiry_cmd
(
  void*                   throt_sm_ptr
)
{
  ds_3gppi_throttle_sm_s      *ds_3gppi_throt_sm_ptr = NULL;
  uint8                        index = 0;
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  char                         apn[DS_PROFILE_APN_STRING_LEN+1];
  /*------------------------------------------------------------------------*/

  ds_3gppi_throt_sm_ptr = (ds_3gppi_throttle_sm_s*)throt_sm_ptr;

  if (ds_3gppi_throt_sm_ptr == NULL)
  {
    return;
  }

  memset(apn, 0, DS_PROFILE_APN_STRING_LEN+1);
  strlcpy(apn, (const char*)ds_3gppi_throt_sm_ptr->apn,
          DS_PROFILE_APN_STRING_LEN+1);

  ds_3gpp_pdn_limit_reset_efs_entry (ds_3gppi_throt_sm_ptr->apn);

  ds_3gpp_pdn_throt_clear_throttle_state
    (ds_3gppi_throt_sm_ptr->apn, DS_PROFILE_PDP_IPV4V6,
       DS_3GPP_THROT_RAT_MASK_GLOBAL, DS_3GPP_THROT_PROC_MASK_GLOBAL);

  ds_3gpp_pdn_limit_stop_sampling_timer_if_needed();

  if ((index = ds_3gpp_pdn_limit_tbl_find_entry((byte *)apn))
       != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {

    DS_MSG1(MSG_LEGACY_HIGH, 
            "PDN Req throttling tmr expired at index %d", index);

    per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

    if (per_apn_entry_ptr == NULL)
    {
      return;
    }

    per_apn_entry_ptr->pdn_conn_cntr = 0;
    ds_3gpp_pdn_limit_start_pdn_conn_tmr(index);
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_PROCESS_PDN_WAIT_TMR_EXPIRY_CMD

  DESCRIPTION
  This function processes the PDN Wait Timer Expiry Cmd.
  
  PARAMETERS  
  Callback data. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_process_pdn_wait_tmr_expiry_cmd
(
  void*                   throt_sm_ptr
)
{
  ds_3gppi_throttle_sm_s      *ds_throt_sm_ptr = NULL;
  uint8                        index = 0;
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*------------------------------------------------------------------------*/

  ds_throt_sm_ptr = (ds_3gppi_throttle_sm_s*)throt_sm_ptr;

  if (ds_throt_sm_ptr == NULL)
  {
    return;
  }

  if ((index = ds_3gpp_pdn_limit_tbl_find_entry(ds_throt_sm_ptr->apn))
       != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {
    DS_MSG1(MSG_LEGACY_HIGH, "PDN Req Wait tmr expired at index %d", index);

    per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

    if (per_apn_entry_ptr == NULL)
    {
      return;
    }

    ds_3gpp_pdn_throt_clear_throttle_state
      (per_apn_entry_ptr->apn, DS_PROFILE_PDP_IPV4V6,
         DS_3GPP_THROT_RAT_MASK_GLOBAL, DS_3GPP_THROT_PROC_MASK_GLOBAL);

    ds_3gpp_pdn_limit_tbl_remove_entry(index);
  }
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_START_THROTTLING_TMR

  DESCRIPTION
  This function starts the PDN Req Throttling Timer.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
static void ds_3gpp_pdn_limit_start_throttling_tmr
(
  uint8                   index
)
{
  ds_3gpp_pdn_throt_rat_based_s      *rat_based_pdn_throt_p = NULL;
  byte                               *apn = NULL;
  ds_3gpp_pdn_throt_ident_info_type   throt_ident_info;
  ds_3gpp_pdn_limit_dyn_tbl_s        *per_apn_entry_ptr  = NULL;
  ds_3gpp_throttle_sm_cb_data_type    cb_data;
  uint32                              time_rem = 0;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return;
  }

  if (ds_3gpp_pdn_limit_info_p->efs_info.pdn_req_throttling_time != 0 )
  {
    DS_MSG1(MSG_LEGACY_HIGH,
            "Starting PDN Req throttling timer at index %d",index);

    apn = per_apn_entry_ptr->apn;
    time_rem = ds_3gpp_pdn_limit_info_p->efs_info.pdn_req_throttling_time * 60 * 1000;
    ds_3gpp_throt_fill_cb_data (&cb_data, NULL, NULL, (void*)time_rem);
    ds_3gpp_pdn_throt_fill_ident_info_ex
    (
       &throt_ident_info, 
       apn,
       TRUE,
       DS_3GPP_THROT_RAT_GLOBAL,
       DS_3GPP_PDN_THROT_PROC_GLOBAL,
       DS_3GPP_PDN_THROT_TYPE_PDN_CONN_LIMIT,
       DS_PROFILE_PDP_IPV4V6,
       FALSE,
       DS_NET_DOWN_REASON_NOT_SPECIFIED,
       NULL,
       &cb_data
    );

    rat_based_pdn_throt_p = 
      ds_3gpp_pdn_throt_sm_get_throttle_sm_per_rat(&throt_ident_info, TRUE);

    if (rat_based_pdn_throt_p == NULL)
    {
      return;
    }

    ds_3gpp_pdn_throt_perform_apn_throttling
      (rat_based_pdn_throt_p, &throt_ident_info);
  }

  return;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_START_PDN_WAIT_TMR

  DESCRIPTION
  This function starts the PDN Req Wait Timer.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_start_pdn_wait_tmr
(
  uint8                  index
)
{
  ds_3gpp_pdn_throt_rat_based_s     *rat_based_pdn_throt_p = NULL;
  byte                              *apn = NULL;
  ds_3gpp_pdn_limit_dyn_tbl_s       *per_apn_entry_ptr  = NULL;
  ds_3gpp_pdn_throt_ident_info_type  throt_ident_info;
  /*------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);
  if (per_apn_entry_ptr == NULL)
  {
    return;
  }

  if (per_apn_entry_ptr->pdn_req_wait_time != 0)
  {
    DS_MSG1(MSG_LEGACY_HIGH, "Starting PDN Wait timer at index %d", index);

    apn = per_apn_entry_ptr->apn;
    ds_3gpp_pdn_throt_fill_ident_info_ex
    (
       &throt_ident_info, 
       apn,
       TRUE,
       DS_3GPP_THROT_RAT_GLOBAL,
       DS_3GPP_PDN_THROT_PROC_GLOBAL,
       DS_3GPP_PDN_THROT_TYPE_PDN_REQ_WAIT_TIME,
       DS_PROFILE_PDP_IPV4V6,
       FALSE,
       DS_NET_DOWN_REASON_NOT_SPECIFIED,
       NULL,
       NULL
    );

    rat_based_pdn_throt_p = 
      ds_3gpp_pdn_throt_sm_get_throttle_sm_per_rat(&throt_ident_info, TRUE);
    if (rat_based_pdn_throt_p == NULL)
    {
      return;
    }

    ds_3gpp_pdn_throt_perform_apn_throttling
      (rat_based_pdn_throt_p, &throt_ident_info);
  }

  return;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_INCREMENT_PDN_CONN_CNTR

  DESCRIPTION
  This function increments the PDN Connection Counter.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_increment_pdn_conn_cntr
(
  uint8                   index
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*----------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return;
  }

  per_apn_entry_ptr->pdn_conn_cntr++;

  DS_MSG2(MSG_LEGACY_HIGH, 
          "Incrementing PDN Conn Cntr value at index %d. New value %d", 
          index, per_apn_entry_ptr->pdn_conn_cntr);
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_RESET_PDN_CONN_CNTR

  DESCRIPTION
  This function resets the PDN Connection Counter.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_limit_reset_pdn_conn_cntr
(
  uint8                  index
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "Resetting PDN Conn Ctr at index %d",index);
  per_apn_entry_ptr->pdn_conn_cntr = 0;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_GET_PDN_CONN_CNTR

  DESCRIPTION
  This function fetches the current PDN Connection Counter.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  Current PDN Conn Cntr.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
uint16 ds_3gpp_pdn_limit_get_pdn_conn_cntr
(
  uint8                  index
)
{
  ds_3gpp_pdn_limit_dyn_tbl_s *per_apn_entry_ptr  = NULL;
  /*-------------------------------------------------------------------------*/

  per_apn_entry_ptr = ds_3gpp_pdn_limit_get_tbl_entry_per_apn(index);

  if (per_apn_entry_ptr == NULL)
  {
    return 0;
  }

  DS_MSG2(MSG_LEGACY_HIGH,
          "Current PDN Conn Ctr at index %d is %d",
          index, per_apn_entry_ptr->pdn_conn_cntr);

  return per_apn_entry_ptr->pdn_conn_cntr;
}

/*===========================================================================
  FUNCTION DS_3GPP_PDN_LIMIT_CALL_BRINGUP_IS_ALLOWED

  DESCRIPTION
  This function checks whether call bringup is allowed for the given APN.
  
  PARAMETERS  
  Index into the PDN Limit Table.
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  TRUE if call is allowed.
  FALSE otherwise.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_limit_call_bringup_is_allowed
(
  uint8                  index
)
{
  uint32 current_pdn_conn_tmr_val = 0;
  /*-------------------------------------------------------------------------*/

  if (ds_3gpp_pdn_limit_get_pdn_conn_cntr(index) >= 
        ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_allowed_in_given_time(
                       index, ds_3gpp_pdn_limit_get_max_pdn_conn_time(index)))
  {
    DS_ERROR_LOG_0("Max PDN Conn allowed count is reached. "
                   "Rejecting the call and starting the throttling timer"); 
    ds_3gpp_pdn_limit_start_throttling_tmr(index);
    return FALSE;  
  }

  current_pdn_conn_tmr_val = 
              ds_3gpp_pdn_limit_get_current_pdn_conn_tmr_val(index);
  if (current_pdn_conn_tmr_val == 0)
  {
    return TRUE;
  }

  if (ds_3gpp_pdn_limit_get_pdn_conn_cntr(index) >=  
        ds_3gpp_pdn_limit_tbl_get_max_pdn_conn_allowed_in_given_time
                                             (index, current_pdn_conn_tmr_val))
  {
    DS_ERROR_LOG_0("Max PDN Conn allowed count for "
                   "current time slice is reached. Rejecting the call");
    return FALSE;  
  }

  return TRUE;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_IS_PDN_LIMIT_PARAM_DEFAULT
 
DESCRIPTION
  This function verifies parameters present in profile being passed 
  are non default param

PARAMETERS 
  profile_info: Profile information
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean TRUE: PDN limit parameters present in profile are default
          FALSE: PDN limit parameters present in profile are non-default
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
boolean ds_3gpp_pdn_context_is_pdn_limit_param_default
(
  uint16                max_pdn_conn_per_blk,
  uint16                 max_pdn_conn_time,
  uint16                 pdn_req_wait_time
)
{
  boolean result = FALSE;
  
  result = ((max_pdn_conn_per_blk != DS_PROFILE_DEF_MAX_PDN_CONN_PER_BLK) ||
            (pdn_req_wait_time != DS_PROFILE_DEF_MAX_PDN_CONN_TIME) ||
            (max_pdn_conn_time != DS_PROFILE_DEF_PDN_REQ_WAIT_TIME));
  
  return !result;
}/*ds_3gpp_pdn_context_is_pdn_limit_param_default*/

/*==============================================================================
FUNCTION DS_3GPP_PDN_CNTXT_VALIDATE_AND_UPDATE_PDN_LIMIT_ENTRY
 
DESCRIPTION
  This function initially checks if pdn limit parameters bring passed are non-
  default , 
    If Yes: Add an entry in PDN limit table
    If NO : Check all other profile for non-default pdn limit parameter
            with same apn
            If Yes : Add an entry in PDN limit table with non-default parameters
            If No  : Add an entry in PDN limit table default parameters
 
  Assumptions made 
  1: Non-default Profile PDN limit parameter will be updated over default Profile
     PDN limit parameters with the same APN
  2. First non-default Profile PDN limit parameters will be chosen over multiple
     non-default Profile parameter given they have same APN

PARAMETERS 
  uint8  Profile_id :  Profile ID being passed
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean FALSE : Failure to update PDN Limit Entry
                TRUE : Sucessfully update PDN Limit Entry
 
SIDE EFFECTS 
  None 
  
==============================================================================*/
boolean ds_3gpp_pdn_cntxt_validate_and_update_pdn_limit_entry
(
 uint8                   profile_id,
 uint8                  *handle_id
)
{
  uint8                            i;
  boolean                          ret_val = FALSE;
  ds_profile_status_etype          result;
  uint16                           max_pdn_conn_per_blk ; 
  uint16                           max_pdn_conn_time ; 
  uint16                           pdn_req_wait_time ;    
  ds_profile_type_s                pdp_profile_ptr = {{0}}; 
  byte                             profile_apn[DS_PROFILE_APN_STRING_LEN+1];
/*-----------------------------------------------------------------------------*/

  
  memset(profile_apn, 0, sizeof(DS_PROFILE_APN_STRING_LEN+1));

  result = ds_profile_setting_from_profile_id(profile_id, &pdp_profile_ptr);
  if(result != DS_PROFILE_RESULT_SUCCESS )
  {
    return FALSE;
  }

  memscpy((void*)profile_apn, DS_PROFILE_APN_STRING_LEN+1,
          (void*)pdp_profile_ptr.context.apn, DS_PROFILE_APN_STRING_LEN+1);

  max_pdn_conn_per_blk = pdp_profile_ptr.max_pdn_conn_per_blk;
  max_pdn_conn_time = pdp_profile_ptr.max_pdn_conn_time;
  pdn_req_wait_time =  pdp_profile_ptr.pdn_req_wait_time;

  // Verify pdn limit parameter for profile being passed is default 
  ret_val = ds_3gpp_pdn_context_is_pdn_limit_param_default(
                                          pdp_profile_ptr.max_pdn_conn_per_blk,
                                          pdp_profile_ptr.max_pdn_conn_time,
                                          pdp_profile_ptr.pdn_req_wait_time);
  
  if (ret_val) 
  {
    // Loop through all active profile     
    for (i=1; i<=DS_PROFILE_MAX_PROFILE_NUM; i++)
    {
      if(!ds_profile_number_is_valid(i) ||
         i == profile_id) 
      {
        continue;
      }

      memset(&pdp_profile_ptr, 0, sizeof(ds_profile_type_s));
      result = ds_profile_setting_from_profile_id(i, &pdp_profile_ptr);
      if(result != DS_PROFILE_RESULT_SUCCESS )
      {
        continue;
      }

      // Compare APN names 
      if(strncasecmp((char*)pdp_profile_ptr.context.apn,
                     (char*)profile_apn, DS_PROFILE_APN_STRING_LEN+1)==0)
      {
        ret_val = TRUE;
      }
      else
      {
        ret_val = FALSE;
      }
      
      if((ret_val == TRUE) &&  
         (!ds_3gpp_pdn_context_is_pdn_limit_param_default(
                                         pdp_profile_ptr.max_pdn_conn_per_blk,
                                         pdp_profile_ptr.max_pdn_conn_time,
                                         pdp_profile_ptr.pdn_req_wait_time)))
      {
        // Found profile with non-default PDN limit param with same APN being passed
        DS_MSG1(MSG_LEGACY_HIGH, 
               "PDN limit parameters updated with profile %d info", i);
        
        max_pdn_conn_per_blk = pdp_profile_ptr.max_pdn_conn_per_blk;
        max_pdn_conn_time = pdp_profile_ptr.max_pdn_conn_time;
        pdn_req_wait_time =  pdp_profile_ptr.pdn_req_wait_time;
        break;
      }
    }
  }
  else
  {
    DS_MSG1(MSG_LEGACY_HIGH, 
            "PDN limit parameters updated with profile %d info", profile_id);
  }
  
  // Add an entry into PDN limit table
  *handle_id = ds_3gpp_pdn_limit_tbl_find_or_add_entry
              ( profile_apn,
                max_pdn_conn_per_blk,
                max_pdn_conn_time,
                pdn_req_wait_time   
              );
                                         
  DS_MSG3(MSG_LEGACY_HIGH, "Updating PDN limit parameters "
          "max_pdn_conn_per_blk %d max_pdn_conn_time %d pdn_req_wait_time %d ",
          max_pdn_conn_per_blk, max_pdn_conn_time, pdn_req_wait_time);

  return TRUE;
}/*ds_3gpp_pdn_cntxt_validate_and_update_pdn_limit_entry*/

