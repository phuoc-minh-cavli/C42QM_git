/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                P B M G S D I_ R E F R E S H.C

GENERAL DESCRIPTION
  This file contains the PBM wrapper functions for GSDI

  Copyright (c) 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE
$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbmgsdi_refresh.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/16   nr      ECC optimizations for IoE devices
05/26/16   sp      Logging improvements
05/16/16   sp      Logging improvements
02/01/16   nr      Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "pbm_common_types.h"
#include "pbm.h"
#include "string.h"
#include "pbmgsdi.h"
#include "pbmutils.h"
#include "pbmuim.h"
#include "stringl/stringl.h"
#include "pbmlib.h"
#include "uim_msg.h"
#include "uim_pbm.h"

/*===========================================================================
                         DEFINITIONS
===========================================================================*/

/* Save refresh information in PBM for each subscription */
PBM_REFRESH_INFO pbm_refresh_info[PBM_SESSION_MAX];

/*===========================================================================
FUNCTION PBM_FILENAME_TO_REFRESH_GROUP

DESCRIPTION
  This function will return the refresh file group corresponding to the
  pbm file.

SIDE EFFECTS
===========================================================================*/
static pbm_file_type pbm_filename_to_refresh_group(
  pbm_file_type                pbm_file,
  pbm_session_enum_type        session_type
)
{
  int              i;
  uint32           j = 0;

  j =pbm_util_return_fileset_for_session(session_type);

  /* find this file */
  for ( i = 0; i < (int) ARR_SIZE(pbm_file_to_uim_file); i++)
  {
    if (pbm_file == pbm_file_to_uim_file[i].pbm_session_filename[j])
    {
      return pbm_file_to_uim_file[i].pbm_file_group[j];
    }
  }

  UIM_MSG_ERR_2("Could not find group for file %d, session %d, USIM_LOCAL",
                 pbm_file, session_type); /*, (int)USE_USIM_LOCAL(session_type)*/
  return PBM_FILE_NONE;
} /* pbm_filename_to_refresh_group */


/*===========================================================================
FUNCTION PBM_UPDATE_REFRESH_GLOBALS

DESCRIPTION
  This function is called to update the REFRESH Related Global Variables
  contained in PBM.

DEPENDENCIES
  None.

SIDE EFFECTS
  refresh_num_files and refresh_file_list
===========================================================================*/
static boolean pbm_update_refresh_globals(
  pbm_refresh_file_list_type  *pbm_refresh_file_list_p,
  pbm_session_enum_type session_type
)
{
  uint8              num_file_devices;
  int                refresh_file;
  int                file_group_index;
  int                file_device_index;
  int                i;
  pbm_file_type      pbm_file_group;
  pbm_file_type      affected_groups[PBM_FILE_MAX_REQUESTS];
  int                num_affected_groups;
  uint32             file_set = 0;


  if ( pbm_refresh_file_list_p->num_files > PBM_MAX_FILE_CHANGE_NOTIFICATIONS )
  {
    UIM_MSG_ERR_1("PBM pbm_update_refresh_globals refresh too many files %d",
                  pbm_refresh_file_list_p->num_files);
    return FALSE;
  }

  /* Copy Number of Files */
  pbm_refresh_info[session_type].pbm_refresh_num_files = pbm_refresh_file_list_p->num_files;

  if ( pbm_refresh_file_list_p->file_list_ptr == NULL )
  {
    return FALSE;
  }

  /* Copy File List */
  memscpy(pbm_refresh_info[session_type].pbm_refresh_file_list,
          sizeof(pbm_refresh_info[session_type].pbm_refresh_file_list),
          pbm_refresh_file_list_p->file_list_ptr,
          sizeof(pbm_file_type) * pbm_refresh_file_list_p->num_files);
  if(pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(session_type))
  {
    pbm_session_info[session_type].pbm_refresh_init = PBM_REFRESH_DURING_INIT;
    /* refresh has come when session was still initializing */
  }
  else
  {
    pbm_session_info[session_type].pbm_refresh_init = PBM_REFRESH_ONLY;
    /* refresh event came after session got initialized */
  }

  /* figure out which groups of files were affected */
  pbm_session_info[session_type].pbm_refresh_num_file_devices = 0;
  num_file_devices = 0;
  num_affected_groups = 0;

  memset(pbm_session_info[session_type].pbm_refresh_pbm_file_devices,
         0,
         sizeof(pbm_session_info[session_type].pbm_refresh_pbm_file_devices));
  memset(affected_groups,0,sizeof(affected_groups));

  for (refresh_file=0; refresh_file < pbm_refresh_info[session_type].pbm_refresh_num_files; refresh_file++)
  {
    pbm_file_group = pbm_filename_to_refresh_group(
                         pbm_refresh_info[session_type].pbm_refresh_file_list[refresh_file],
                                                                              session_type);
    /* add it to the list, if not already there */
    if (pbm_file_group != PBM_FILE_NONE)
    {
      PBM_ADD_UNIQUE_ITEM_TO_LIST(pbm_file_group,
                                  affected_groups,
                                  num_affected_groups,
                                  file_group_index);
    }
  }

  /* now translate the affected groups into the list of files */

  /* go through the list of file groups */
  /* possible optimization: if PBM_FILE_ALL is found, or array becomes full, stop looking */
  file_set = pbm_util_return_fileset_for_session(session_type);
  for (file_group_index=0; file_group_index < num_affected_groups; file_group_index++)
  {
    /* find the files associated */
    for (i=0; i<(int)ARR_SIZE(pbm_file_to_uim_file); i++)
    {
      if ((affected_groups[file_group_index] ==
           pbm_file_to_uim_file[i].pbm_file_group[file_set]) ||
           (affected_groups[file_group_index] == PBM_FILE_ALL))
      {
        /* This one is affected.  add it to the list if not already there */
        PBM_ADD_UNIQUE_ITEM_TO_LIST(pbm_file_to_uim_file[i].pbm_filename,
                                    pbm_session_info[session_type].pbm_refresh_pbm_file_devices,
                                    num_file_devices,
                                    file_device_index);
      }
    }
  } /* end loop over affected file groups */

  if (session_type < PBM_SESSION_MAX)
  {
    //Add in any files that haven't yet been read the first time
    for (i=pbm_initialize_index[session_type]; i<(int)ARR_SIZE(pbm_initialize_list); i++)
    {
      PBM_ADD_UNIQUE_ITEM_TO_LIST(pbm_initialize_list[i],
                                  pbm_session_info[session_type].pbm_refresh_pbm_file_devices,
                                  num_file_devices,
                                  file_device_index);
    }
    pbm_session_info[session_type].pbm_refresh_num_file_devices = num_file_devices;
  }

  return TRUE;
} /* pbm_update_refresh_globals */


/*===========================================================================
FUNCTION PBM_CLEAR_REFRESH_CACHES

DESCRIPTION
  This function will clear the caches associated with the global refresh file
  list.

PARAMETERS

SIDE EFFECTS
===========================================================================*/
static void pbm_clear_refresh_caches(
  pbm_session_enum_type session_type
)
{
  int                       i;
  pbm_file_type             file_id;
  PB_CACHE                 *cache;
  pbm_ext_cache_s_type     *ext_cache;
  PB_USIM_CACHE            *usim_cache;
  PB_MASTER_USIM_CACHE     *m_usim_cache;
  PB_SYNC1_CACHE           *sync1_cache;
  PB_SYNC2_CACHE           *sync2_cache;

  /* go through the list of files that you need to clear out */
  /* find the files associated */
  for (i=0; i<pbm_session_info[session_type].pbm_refresh_num_file_devices; i++)
  {
    /* clear the cache */
    file_id = pbm_session_info[session_type].pbm_refresh_pbm_file_devices[i];
    /* if we're doing the ADN group, we need to clear the USIM caches, too. */
    if (file_id == PBM_FILE_ADN)
    {
      pbm_clear_hidden_cache(session_type);
      pbm_clear_usim_caches(session_type);
    }
    switch (pbm_file_id_to_cache_type(file_id))
    {
      case PBM_CACHE_TYPE_PB:
        if ((cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
        {
          /* Notifying cache cache status to QMI PBM to clear the PB bitmask for the affected */
          pbm_notify_pb_cache_status_update(cache);
          rex_enter_crit_sect(&pbm_crit_sect);
          pbm_clear_cache(cache);
          rex_leave_crit_sect(&pbm_crit_sect);
        }
        break;

      case PBM_CACHE_TYPE_EXT:
        if ((ext_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
        {
          rex_enter_crit_sect(&pbm_crit_sect);
          /*for ext caches, you can just clear the in use array */
          if (ext_cache->num_of_rec && ext_cache->in_use_array != NULL)
          {
            memset(ext_cache->in_use_array, 0, (uint32)(ext_cache->num_of_rec + 1) *
                   sizeof(pbm_ext_rec_in_use_s_type));
          }
          rex_leave_crit_sect(&pbm_crit_sect);
        }
        break;

      case PBM_CACHE_TYPE_USIM:
        if ((usim_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
        {
          rex_enter_crit_sect(&pbm_crit_sect);
          pbm_clear_usim_cache(session_type, usim_cache);
          rex_leave_crit_sect(&pbm_crit_sect);
        }
        break;

      case PBM_CACHE_TYPE_INDEX:
        if ((m_usim_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
        {
          rex_enter_crit_sect(&pbm_crit_sect);
          pbm_clear_index_cache(m_usim_cache);
          rex_leave_crit_sect(&pbm_crit_sect);
        }
        break;

      case PBM_CACHE_TYPE_SYNC1:
        if ((sync1_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
        {
          rex_enter_crit_sect(&pbm_crit_sect);
          pbm_clear_sync1_cache(sync1_cache);
          rex_leave_crit_sect(&pbm_crit_sect);
        }
        break;

      case PBM_CACHE_TYPE_SYNC2:
        if ((sync2_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
        {
          rex_enter_crit_sect(&pbm_crit_sect);
          pbm_clear_sync2_cache(sync2_cache);
          rex_leave_crit_sect(&pbm_crit_sect);
        }
        break;

      default:
        UIM_MSG_ERR_1("Unknown file id %d", file_id);
        break;
    }
  }
} /* pbm_clear_refresh_caches */


/*===========================================================================
FUNCTION pbm_mmgsdi_refresh_cb()

DESCRIPTION
  register for refresh mmgsdi cb function.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void pbm_mmgsdi_refresh_cb(
  mmgsdi_return_enum_type    status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
)
{
  /* DO Nothing */
  if ((cnf != MMGSDI_REFRESH_CNF) || (status != MMGSDI_SUCCESS))
  {
    UIM_MSG_ERR_2("Refresh Complete Ack failed %d %d",
                  cnf, status);
  }
} /* pbm_mmgsdi_refresh_cb */


/*===========================================================================
FUNCTION pbm_mmgsdi_register_fcn

DESCRIPTION
  Initialize MMGSDI file change notifications

DEPENDENCIES
  sim init complete and card apps available

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_register_fcn(
  pbm_session_enum_type session_type
)
{
  mmgsdi_return_enum_type            mmgsdi_status = MMGSDI_SUCCESS;
  pbm_return_type                    pbm_ret = PBM_ERROR;
  mmgsdi_refresh_file_list_type      mmgsdi_refresh_file_list;
  int                                i = 0;
  uint8                              path_index = 0;
  uint32                             fileset = 0;
  pbm_file_type                      pbm_file = PBM_FILE_NONE;

  memset(&mmgsdi_refresh_file_list, 0, sizeof(mmgsdi_refresh_file_list));
  //register with files only applicable for this session
  fileset = pbm_util_return_fileset_for_session(session_type);

  //wee need to allocate the buffer for 8 bytes for every file
  PBM_MEM_ALLOC(mmgsdi_refresh_file_list.file_path_ptr,(int)PBM_FILE_MAX_REQUESTS*4*sizeof(uint16));
  PBM_CHECK_PTR_RET(mmgsdi_refresh_file_list.file_path_ptr, VOID);
  /* register for FCN on all UIM files in SIM */
  for (i=0; i<(int)ARR_SIZE(pbm_file_to_uim_file); i++)
  {
    if (pbm_file_to_uim_file[i].pbm_session_filename[fileset] != PBM_FILE_NONE)
    {
      mmgsdi_access_type file_struct;
      pbm_file = pbm_file_to_uim_file[i].pbm_session_filename[fileset];
      pbm_ret = pbm_populate_file_access(session_type,&file_struct, pbm_file);
      if(pbm_ret == PBM_SUCCESS)
      {
        if(file_struct.access_method == MMGSDI_BY_PATH_ACCESS)
        {
          //below loop copies paths in uint16 array to byte array
          //below logic will takes care of both little endian and big endian
          for (path_index= 0; path_index< file_struct.file.path_type.path_len; path_index++)
          {
            mmgsdi_refresh_file_list.file_path_ptr[mmgsdi_refresh_file_list.file_path_len++] =
              (uint8)(file_struct.file.path_type.path_buf[path_index] >> 8);
            mmgsdi_refresh_file_list.file_path_ptr[mmgsdi_refresh_file_list.file_path_len++] =
              (uint8)(file_struct.file.path_type.path_buf[path_index] & 0x00FF);
          }
        }
      }
    }
  }
  mmgsdi_status = mmgsdi_session_register_for_refresh (
                                                      pbm_session_info[session_type].session_id,       /* sending mmgsdi session_id */
                                                      mmgsdi_refresh_file_list,
                                                      FALSE,
                                                      pbm_mmgsdi_refresh_cb,
                                                      0);
  /* Check to see if REQ for Registration was ok */
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Error %d in Request Refresh FCN Registration",mmgsdi_status);
  }
  PBM_MEM_FREEIF(mmgsdi_refresh_file_list.file_path_ptr);
  return;
} /* pbm_mmgsdi_register_fcn */


/* ==========================================================================
FUNCTION PBM_UTIL_CONVERT_TO_REFRESH_FILE_PATHS

DESCRIPTION
  This function shall accept byte buffer that has list of file paths.
  Each file path begins from MF, hence byte value 0x 3F shall be used
  as a delimiter. The individual file paths shall be stored in the
  array of mmgsdi_path_type.
  Input buffer: 0x3F 0x00 0x2F 0x02 0x3F 0x00 0x7F 0x10 0x6F 0xD2
                 |
                First byte is number of files
  Should give output -
  *file_path_pptr[0].path_buf={0x3F 0x00 0x2F 0x02 0x00 0x00 0x00 0x00 0x00 0x00}
  *file_path_pptr[0].path_len = 2
  *file_path_pptr[1].path_buf={0x3F 0x00 0x7F 0x10 0x6F 0xD2 0x00 0x00 0x00 0x00}
  *file_path_pptr[1].path_len = 3
  num_files = 2

DEPENDENCIES
  NONE

LIMITATIONS


RETURN VALUE
  mmgsdi_return_enum_type
  PBM_SUCCESS          - Command processed,
  PBM_ERROR            - Other unknown failure

SIDE EFFECTS
  Calling function should free memory allocated to file_path_pptr
----------------------------------------------------------------------------*/
pbm_return_type  pbm_util_convert_to_refresh_file_paths(
  uint32                  data_len,
  const uint8            *data_ptr,
  mmgsdi_path_type       *file_path_pptr,
  uint8                   num_files
)
{
  uint8                   file_num                    = 0;
  byte                    index                       = 0;
  byte                    path_index                  = 0;
  word                    path[MMGSDI_MAX_PATH_LEN];

  /* Check for input params */
  PBM_CHECK_PTR2_RET(data_ptr, file_path_pptr, PBM_ERROR);

  if(data_len == 0)
  {
    UIM_MSG_ERR_1("Bad Input Params data_len=0x%x",data_len);
    return PBM_ERROR;
  }

  memset(path, 0x00,(sizeof(uint16)*(MMGSDI_MAX_PATH_LEN)));

  while (( index < data_len ) && (file_num < num_files))
  {
    path_index = 0;

    /* Parse for each file path */
    do
    {
      path[path_index] = data_ptr[index++] << 8;
      path[path_index] = path[path_index] | data_ptr[index++];
      UIM_MSG_HIGH_1("File path info - 0x%x", path[path_index]);
      path_index++;
    }while ((index < data_len) && (data_ptr[index] != 0x3F) && (path_index<ARR_SIZE(path)));

    /* Path index was incremented by 1 in the last iteration of do-while loop,
       hence we have path_len = path index here */
    file_path_pptr[file_num].path_len = path_index;

    /* Copy path buffer */
    memscpy(file_path_pptr[file_num].path_buf, sizeof(file_path_pptr[file_num].path_buf),
           path, (path_index) * sizeof(uint16));
    file_num++;
  } /* while (index < data_len ) */
  return PBM_SUCCESS;
} /* pbm_util_convert_to_refresh_file_paths */


/*===========================================================================
FUNCTION PBM_MMGSDI_REFRESH_COMPLETE_CB

DESCRIPTION
  This function is called in response to gdi_refresh_complete by GSDI

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_refresh_complete_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr)
{
  /* Do Nothing */
  if ((cnf != MMGSDI_REFRESH_CNF) || (status != MMGSDI_SUCCESS))
  {
    UIM_MSG_ERR_2("Refresh Complete Ack failed %d %d",
                   status, cnf);
  }
} /* pbm_mmgsdi_refresh_complete_cb */


/*==============================================================================

FUNCTION NAME pbm_refresh_file_change_notification_cb

DESCRIPTION
  Function that gets called by GSDI when a REFRESH Request is made and a
  determination must be made whether to proceed or not.

  Currently we refresh all SIM phonebooks irrespective of which files were
  changed. This should actually be based on the file list that GSDI sends us.

RETURN VALUE

  void

DEPENDENCIES
  None
==============================================================================*/
void pbm_refresh_file_change_notification_cb
(
  mmgsdi_refresh_file_list_type  *refresh_file_list_p, /*list of changed files*/
  mmgsdi_session_id_type          mmgsdi_session_id
)
{
  boolean                     globals_updated = FALSE;
  pbm_session_enum_type       session_type;
  pbm_phonebook_type          pb_id = {PBM_GPB};
  uint32                      i, j;
  uint8                       local = 0;     /* Local vs Global phonebook */
  pbm_file_type               pbm_file_iterator;
  pbm_file_type               pbm_file;
  mmgsdi_path_type           *l_file_path_ptr=NULL;
  uint8                       l_num_path_files = 0;
  pbm_refresh_file_list_type  pbm_refresh_file_list;
  
  memset(&pbm_refresh_file_list,0,sizeof(pbm_refresh_file_list));

  PBM_CHECK_PTR_RET(refresh_file_list_p, VOID);
  session_type = pbm_mmgsdi_session_to_pbm_session(mmgsdi_session_id);
  for (i = 0; i < (refresh_file_list_p->file_path_len); i+=2)
  {
    if ((refresh_file_list_p->file_path_ptr[i] == 0x3F) &&
        (refresh_file_list_p->file_path_ptr[i+1] == 0x00))
    {
      l_num_path_files++;
    }
  }
  /* allocate memory to file_path_array */
  PBM_MEM_ALLOC(l_file_path_ptr,l_num_path_files * sizeof(mmgsdi_path_type));
  PBM_CHECK_PTR_RET(l_file_path_ptr,VOID);
  (void)pbm_util_convert_to_refresh_file_paths(refresh_file_list_p->file_path_len,
                                          refresh_file_list_p->file_path_ptr,
                                          l_file_path_ptr,
                                          l_num_path_files);

  PBM_MEM_ALLOC(pbm_refresh_file_list.file_list_ptr,
                (sizeof(pbm_file_type) * (l_num_path_files + refresh_file_list_p->num_files)));
  /* why we have l_num_path_files + refresh_file_list_p->num_files, can there be path and enum notifications from GSDI
     if true then why do we have a below mentioned for loop for l_num_path_files only */
  if(pbm_refresh_file_list.file_list_ptr == NULL)
  {
    PBM_MEM_FREEIF(l_file_path_ptr);   
    return;
  }
  
  for(i=0;i<l_num_path_files;i++)
  {
    if(PBM_SUCCESS == pbm_convert_path_to_file_enum(
                                            session_type,
                                            l_file_path_ptr[i],
                                            &pbm_file_iterator))
    {
      pbm_refresh_file_list.file_list_ptr[refresh_file_list_p->num_files] = pbm_file_iterator;
      refresh_file_list_p->num_files++;
    }
   }
   pbm_refresh_file_list.num_files = refresh_file_list_p->num_files;
   /* since pbm_refresh_file_list_p would be used in updating refresh globals below*/
   /* Populate Static Globals for PBM */
   globals_updated = pbm_update_refresh_globals(&pbm_refresh_file_list,
                                                session_type);
   if ( globals_updated == TRUE)
   {
     local = pbm_util_return_fileset_for_session(session_type);

     /* Clear out the PBM Caches affected */
     pbm_clear_refresh_caches(session_type);
     if (pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(session_type))
     {
       /* Session is already in progress and the session init done will be sent to clients eventually 
          So no need of sending per phonebook refresh to client now*/
       UIM_MSG_HIGH_1("Already session init - No need for refresh event to clients now %d",session_type);
       PBM_SESSION_SET(pbm_session_restart, session_type);
       PBM_MEM_FREEIF(pbm_refresh_file_list.file_list_ptr);
       PBM_MEM_FREEIF(l_file_path_ptr);
       return;
     }
     else
     {
        UIM_MSG_HIGH_1("Set session init and continuing for FCN refresh processing %d",session_type);
        PBM_SESSION_SET(pbm_session_initializing, session_type);
     }
	   
     /* Send notification about refresh start for each phonebook that is
      * changed */
     /* For each SESSION phonebook file */
     for (i = 0; i < (uint32)ARR_SIZE(pbm_file_to_uim_file); i++)
     {
       pbm_file_iterator = pbm_file_to_uim_file[i].pbm_filename;

       /* Check if the phonebook was changed */
       for (j = 0; j < refresh_file_list_p->num_files; j++)
       {
         /*Check both the local and global versions*/
           pbm_file = pbm_file_to_uim_file[i].pbm_session_filename[local];
         /* since pbm and uim files name are same now, this check would double sure that if files is not valid for some session
            and still refresh has come, pb refresh should not be sent */
         if (pbm_file == pbm_refresh_file_list.file_list_ptr[j])
         {
           pb_id = pbm_file_id_to_pb_id(session_type, pbm_file_iterator);
           if (pb_id.device_type != PBM_DEFAULT)
           {
             pbm_session_event_data_u_type evt_data = {0};
             evt_data.pb_id = pb_id;
             pbm_add_notify(PBM_EVENT_PB_REFRESH_START, evt_data);
           }
         }
       }
    }

    PBM_MEM_FREEIF(pbm_refresh_file_list.file_list_ptr);
    PBM_MEM_FREEIF(l_file_path_ptr);

    /* Reinitialize the PBM Cache */
    pbm_init_fileinfo_internal(session_type);
  }
} /* pbm_refresh_file_change_notification_cb */


/*===========================================================================
FUNCTION pbm_mmgsdi_ok_to_refresh_cb()

DESCRIPTION
  Init Complete mmgsdi cb function.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pbm_mmgsdi_ok_to_refresh_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr)
{
  /* DO Nothing */
  if ((cnf != MMGSDI_REFRESH_CNF) || (status != MMGSDI_SUCCESS))
  {
    UIM_MSG_ERR_2("Ok to refresh Ack failed %d %d",
                  cnf, status);
  }
} /* pbm_mmgsdi_ok_to_refresh_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_REFRESH_COMPLETE

DESCRIPTION
  This function sends refresh complete to GSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_refresh_complete(
  pbm_session_enum_type       session_type
)
{
  if ( (pbm_session_info[session_type].pbm_refresh_init == PBM_REFRESH_ONLY) ||
       (pbm_session_info[session_type].pbm_refresh_init == PBM_REFRESH_DURING_INIT) )
  {
    /* This ession Initialization is due to refresh - Send refresh complete to MMGSDI */
    (void) mmgsdi_session_refresh_complete(pbm_session_info[session_type].session_id,
                                           TRUE,
                                           pbm_mmgsdi_refresh_complete_cb,
                                           0);

    if( pbm_session_info[session_type].pbm_refresh_init == PBM_REFRESH_ONLY )
    {
      /*Send Refresh End events on respective phonebooks only if the Refresh is not during PBM Init
        During PBM init there is no need of sending this event as All PB init done or SIM init done are not yet received to the client 
        and anyways will eventually be sent */
      pbm_file_type          pbm_file_iterator;
      pbm_file_type          pbm_file;
      pbm_phonebook_type     pb_id = {PBM_GPB};
      int                    i = 0, j = 0 ;
      uint8                  local = 0;

      local = pbm_util_return_fileset_for_session(session_type);
      for (i = 0; i < (int) ARR_SIZE(pbm_file_to_uim_file); i++)
      {
        pbm_file = pbm_file_to_uim_file[i].pbm_filename;
        /* Check if the phonebook was changed */
        for (j = 0; j < pbm_refresh_info[session_type].pbm_refresh_num_files; j++)
        {
          /*Check both the local and global versions*/
          pbm_file_iterator = pbm_file_to_uim_file[i].pbm_session_filename[local];
          if (pbm_file_iterator == pbm_refresh_info[session_type].pbm_refresh_file_list[j])
          {
            pb_id = pbm_file_id_to_pb_id(session_type, pbm_file);
            if (pb_id.device_type != PBM_DEFAULT)
            {
              pbm_session_event_data_u_type evt_data = {0};

              evt_data.pb_id = pb_id;
              pbm_add_notify(PBM_EVENT_PB_REFRESH_DONE, evt_data);
            }
          }
        }
      }
    }

    pbm_session_info[session_type].pbm_refresh_init = PBM_REFRESH_NONE;
  }
} /* pbm_mmgsdi_refresh_complete */
