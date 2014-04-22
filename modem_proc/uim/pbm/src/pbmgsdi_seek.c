/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           P B M G S D I_ S E E K.C

GENERAL DESCRIPTION
  This file contains the PBM search realated functions for GSDI

  Copyright (c) 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE
$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbmgsdi_seek.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
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

/*===========================================================================
FUNCTION PBM_INIT_SEEK_DATA

DESCRIPTION
   Initialize the members of pbm_seek_data for the session_type

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_init_seek_data(
  pbm_session_enum_type   session_type
)
{
  int           i = 0;

  pbm_seek_data[session_type].status = PBM_ERROR;
  pbm_free_seek_data(session_type); /* it is better to free the data here, since in case of card error there is memory leak */
  pbm_seek_data[session_type].in_use_records = NULL;
  pbm_seek_data[session_type].num_of_records = 0;
  pbm_seek_data[session_type].record_length = 0;
  pbm_seek_data[session_type].num_files = 0;
  pbm_seek_data[session_type].cur_file = 0;

  for (i = 0; i < (int) PBM_MAX_NUM_PB_SETS; ++i)
  {
    pbm_seek_data[session_type].records_in_pb_set[i] = 0;
    pbm_seek_data[session_type].file_ids[i] = PBM_FILE_NONE;
  }
} /* pbm_init_seek_data */


/*===========================================================================
FUNCTION PBM_ALLOC_SEEK_DATA

DESCRIPTION
   Allocate memory for in_use_records and set all the records to USED for
   the corresponding session_type

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_alloc_seek_data(
  pbm_session_enum_type  session_type,
  int                    num_of_records
)
{
  if (num_of_records > 0)
  {
    PBM_MEM_ALLOC(pbm_seek_data[session_type].in_use_records, \
                  (pbm_seek_data[session_type].num_of_records+1)*sizeof(uint8));

    /* Mark all records as used */
    if (pbm_seek_data[session_type].in_use_records != NULL)
    {
      memset(pbm_seek_data[session_type].in_use_records, TRUE,
             (uint32) (pbm_seek_data[session_type].num_of_records+1)*sizeof(uint8));
    } else
    {
      UIM_MSG_ERR_0("Could not allocate memory for SEEK data");
    }
  }
} /* pbm_alloc_seek_data */


/*===========================================================================
FUNCTION PBM_FREE_SEEK_DATA

DESCRIPTION
   Free memory for in_use_records in pbm_session_seek_data corresponding to
   session_type

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_free_seek_data(
  pbm_session_enum_type    session_type
)
{
   PBM_MEM_FREEIF(pbm_seek_data[session_type].in_use_records);
} /* pbm_free_seek_data */


/*===========================================================================
FUNCTION PBM_MMGSDI_SEEK_CB

DESCRIPTION
   Call back for the mmgsdi_seek(). Enqueues a PBM_GSDI_SEEK_CB command for
   PBM task to process the SEEKed data.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pbm_mmgsdi_seek_cb(
  mmgsdi_return_enum_type    status,
  mmgsdi_cnf_enum_type       data_type,
  const mmgsdi_cnf_type     *data_ptr
)
{
  pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr   = NULL;
  uint16                  read_cnf_data_len = 0;

  mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
  PBM_CHECK_PTR_RET(mmgsdi_resp_ptr, VOID);

  mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_SEEK_CB;

  mmgsdi_resp_ptr->data_type = data_type;
  mmgsdi_resp_ptr->status = status;

  memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));

  read_cnf_data_len  = (uint16)data_ptr->search_cnf.searched_record_nums.data_len;
  if (read_cnf_data_len > 0)
  {
    PBM_MEM_ALLOC(mmgsdi_resp_ptr->data.search_cnf.searched_record_nums.data_ptr,
                  (uint32)read_cnf_data_len);
    PBM_CHECK_PTR_RET(mmgsdi_resp_ptr->data.search_cnf.searched_record_nums.data_ptr,VOID);
    memscpy((void *)mmgsdi_resp_ptr->data.search_cnf.searched_record_nums.data_ptr, (uint32)read_cnf_data_len,
            (void *)data_ptr->search_cnf.searched_record_nums.data_ptr, read_cnf_data_len);  //deep copy
  } else
  {
    mmgsdi_resp_ptr->data.search_cnf.searched_record_nums.data_ptr = NULL;
  }

  pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
} /* pbm_mmgsdi_seek_cb */


/*===========================================================================
FUNCTION PBM_GET_SEEK_PATTERN

DESCRIPTION
   This function returns the SEEK pattern to use for different files

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
uint8 pbm_get_seek_pattern(
  uint32         client_ref
)
{
  pbm_file_type   file_id;
  uint8           pattern;

  file_id = (pbm_file_type)((int) client_ref % (int) PBM_FILE_MAX_REQUESTS);

  switch (file_id)
  {
    case PBM_FILE_UID:
    case PBM_FILE_PBC:
    case PBM_FILE_GRP:
    case PBM_FILE_UID1:
    case PBM_FILE_PBC1:
    case PBM_FILE_GRP1:
    case PBM_FILE_UID2:
    case PBM_FILE_PBC2:
    case PBM_FILE_GRP2:
    case PBM_FILE_UID3:
    case PBM_FILE_PBC3:
    case PBM_FILE_GRP3:
      pattern = 0x00;
      break;

    default:
      pattern = 0xFFU;
      break;
   }

   return pattern;
} /* pbm_get_seek_pattern */


/*===========================================================================
FUNCTION PBM_SEND_SEEK_REQUEST

DESCRIPTION
   This function sends the SEEK request to GSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_send_seek_request(
  pbm_session_enum_type session_type,
  uint32                client_ref
)
{
  pbm_return_type                ret_val = PBM_ERROR;
  pbm_pb_set_type                pb_set;
  pbm_file_type                  file_id = pbm_seek_data[session_type].file_ids[pbm_seek_data[session_type].cur_file];
  mmgsdi_option_type             dummy_option;
  mmgsdi_access_type             file_access;
  mmgsdi_seek_data_type          seek_pattern;
  mmgsdi_return_enum_type        seek_ret;
  pbm_file_type                  pbm_file = PBM_FILE_NONE;
  mmgsdi_search_offset_data_type dummy_offset;

  if (!USE_USIM_SUPPORT(session_type) && !USE_CSIM_SUPPORT(session_type))
  {
    /* Search not supported for 2G cards */
    UIM_MSG_HIGH_0("PBM Search not supported for 2G cards");
    return PBM_ERROR;
  }

  memset(&seek_pattern, 0, sizeof(seek_pattern));
  memset(&dummy_option, 0, sizeof(dummy_option));
  memset(&dummy_offset, 0, sizeof(dummy_offset));

  pb_set = pbm_file_id_to_pb_set(file_id);
  if(((file_id == PBM_FILE_ADN)    
      || (file_id == PBM_FILE_ADN1)
      || (file_id == PBM_FILE_ADN2)
      || (file_id == PBM_FILE_ADN3)
     ))
  {
    PB_CACHE *cache = (PB_CACHE *) pbm_file_id_to_cache(session_type,file_id);
    PBM_CHECK_PTR_RET(cache, PBM_ERROR);
    seek_pattern.data_len = cache->record_length[pb_set];
  }
  else
  {
    seek_pattern.data_len = pbm_seek_data[session_type].record_length;
  }

  PBM_MEM_ALLOC(seek_pattern.data_ptr, seek_pattern.data_len);
  PBM_CHECK_PTR_RET(seek_pattern.data_ptr, PBM_ERROR_MEM_FULL);
  memset(seek_pattern.data_ptr, pbm_get_seek_pattern(client_ref),
                                               seek_pattern.data_len);

  pbm_file = pbm_seek_data[session_type].file_ids[pbm_seek_data[session_type].cur_file];

  if ( PBM_FILE_FDN == pbm_file )
  {
    ret_val = pbm_populate_enum_file_access(session_type,&file_access, pbm_file);
  }
  else
  {
    ret_val = pbm_populate_file_access(session_type,&file_access, pbm_file);
  }

  if(ret_val != PBM_SUCCESS)
  {
    UIM_MSG_HIGH_3("PBM: in pbm_send_seek_request pbm_populate_file_access returned error %d session %d file %d",
                    ret_val,session_type,pbm_file);
    /* Free the memory allocated for the SEEK pattern */
    PBM_MEM_FREEIF(seek_pattern.data_ptr);
    return ret_val;
  }
  if ((seek_ret = mmgsdi_session_search(
                                      pbm_session_info[session_type].session_id,
                                       file_access,
                                       MMGSDI_UICC_SIMPLE_SEARCH, 1, MMGSDI_SEARCH_FORWARD_FROM_REC_NUM,
                                       dummy_offset, seek_pattern,
                                       pbm_mmgsdi_seek_cb, client_ref)
      ) == MMGSDI_SUCCESS)
  {
    ret_val = PBM_SUCCESS;
  } 
  else
  {
    UIM_MSG_ERR_1("mmgsdi_search returned 0x%x", seek_ret);
    ret_val = PBM_ERROR;
  }
  /* Free the memory allocated for the SEEK pattern */
  PBM_MEM_FREEIF(seek_pattern.data_ptr);

  return ret_val;
}/* pbm_send_seek_request */


/*===========================================================================
FUNCTION PBM_SEEK_CB

DESCRIPTION
   This function is called in the context of the PBM task when PBM_GSDI_SEEK_CB
   command is put in the GSDI queue. This function updates the in_use_records[]
   with the list of unused records SEEKed and starts reading the USED records.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_seek_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    data_type,
  const mmgsdi_cnf_type  *data_ptr
)
{
  pbm_session_enum_type     session_type;
  pbm_file_type             file_id;
  uint32                    client_ref;

  PBM_CHECK_PTR_RET(data_ptr,VOID);

  client_ref = data_ptr->response_header.client_data;

  session_type  = pbm_mmgsdi_session_to_pbm_session(data_ptr->response_header.session_id);
  file_id = (pbm_file_type)((int) client_ref % (int) PBM_FILE_MAX_REQUESTS);

  // Restart initialization if required
  if (pbm_session_restart & PBM_SESSIONID_TO_SESSION_MASK(session_type))
  {
    UIM_MSG_HIGH_1("PBM restarting init of session_type %d", session_type);
    pbm_restart_fileinfo_internal(session_type);
    return;
  }

  // Stop initialization if required
  if (!(pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(session_type)))
  {
    UIM_MSG_HIGH_1("PBM stopping init of SIM %d", session_type);
    return;
  }

  if (status == MMGSDI_SUCCESS)
  {
    if ((data_type == MMGSDI_SEARCH_CNF) &&
        (pbm_seek_data[session_type].in_use_records != NULL))
    {
      const mmgsdi_seek_data_type *seek_data;
      int                          i;
      int                          start;  /* start index for this pb set in in_use_records[] */
      int                          file_index = pbm_seek_data[session_type].cur_file;

      /* Calculate the start index in in_use_records[] of this file */
      for (i = 0, start = 0; i < file_index; ++i)
      {
        start += pbm_seek_data[session_type].records_in_pb_set[i];
      }

      seek_data = &data_ptr->search_cnf.searched_record_nums;

      for (i = 0; i < seek_data->data_len; ++i)
      {
        /* Check that the record index is in range */
        if ((seek_data->data_ptr[i] > 0) &&
            (seek_data->data_ptr[i] <= pbm_seek_data[session_type].records_in_pb_set[file_index]) &&
            (start + seek_data->data_ptr[i] <= pbm_seek_data[session_type].num_of_records))
        {
          pbm_seek_data[session_type].in_use_records[start + seek_data->data_ptr[i]] = FALSE;
        } else
        {
          UIM_MSG_ERR_0("Invalid record number returned in pbm_gsdi_seek_cb");
        }
      }

      /* See if we need to SEEK unused records for the next file in the pb set */
      if (++pbm_seek_data[session_type].cur_file < pbm_seek_data[session_type].num_files)
      {
        if (pbm_send_seek_request(session_type, client_ref) == PBM_SUCCESS)
          return;

        UIM_MSG_ERR_2("PBM SEEK failed of PBM file: sim %d, client ref %d", session_type, client_ref);
      }

      /* The SEEK data is valid */
      pbm_seek_data[session_type].status = PBM_SUCCESS;
    }
  } else
  {
    UIM_MSG_ERR_3("PBM SEEK callback returned %d, sim %d, file id %d", status, session_type, file_id);
  }

  /* Start reading the records for this file. Read atleast one record (even for empty files)
     so that the cache could be set to ready in the read callback */
  if (PBM_SUCCESS == pbm_start_record_read(session_type, file_id, 1))
  {
    return;
  }

  /* Move to next file regardless of why we got here. */
  pbm_initialize_index[session_type]++;

  pbm_init_entries(session_type);
} /* pbm_seek_cb */


/*===========================================================================
FUNCTION PBM_GET_NEXT_USED_RECORD

DESCRIPTION
   Returns the next used record number starting after prev_index. If the
   memory allocation for SEEK data failed or the SEEK was unsuccessful,
   returns the next record in the sequence starting from prev_index.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
int pbm_get_next_used_record(
  pbm_session_enum_type session_type, 
  int                   prev_index
)
{
  int   num_records = pbm_seek_data[session_type].num_of_records;
  int   next_index  = prev_index + 1;

  /* If memory allocation failed or if SEEK was unsuccessful, read all
     the records in sequence */
  if (pbm_seek_data[session_type].in_use_records != NULL &&
      pbm_seek_data[session_type].status == PBM_SUCCESS)
  {
    for (; next_index <= num_records; ++next_index)
    {
      if (pbm_seek_data[session_type].in_use_records[next_index] == TRUE)
      {
        break;
      }
    }
  }

  return(next_index <= num_records) ? next_index : num_records+1;
} /* pbm_get_next_used_record */


/*===========================================================================
FUNCTION PBM_SEEK_UNUSED_RECORDS

DESCRIPTION
   Get the list of unused records via mmgsdi_seek(). These are returned via
   the pbm_mmgsdi_seek_cb().

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
pbm_return_type pbm_seek_unused_records(
  pbm_session_enum_type session_type,
  pbm_file_type         file_id
)
{
  pbm_cache_type_e_type cache_type;
  uint32                client_ref;

  UIM_MSG_HIGH_2("PBM starts SEEK of PBM file: session %d, File id %d",
                 session_type, file_id);

  /* Free data allocated for the previous file_id */
  pbm_free_seek_data(session_type);
  pbm_init_seek_data(session_type);

  cache_type = pbm_file_id_to_cache_type(file_id);

  switch (cache_type)
  {
    case PBM_CACHE_TYPE_PB:
    {
      PB_CACHE          *pb_cache = (PB_CACHE *)pbm_file_id_to_cache(
                                                                 session_type, file_id);
      pbm_pb_set_type    pb_set = pbm_file_id_to_pb_set(file_id);

      PBM_CHECK_PTR_RET(pb_cache, PBM_ERROR);  /* KW error */
      pbm_seek_data[session_type].num_files      = pb_cache->num_files;
      pbm_seek_data[session_type].num_of_records = pb_cache->num_of_records;
      if(pb_set != PBM_USIM_PB_INVALID)
      {
        pbm_seek_data[session_type].record_length = pb_cache->record_length[pb_set];
      }
      else
      { 
        pbm_seek_data[session_type].record_length = pb_cache->record_length[0];
      }

      memscpy(pbm_seek_data[session_type].file_ids, sizeof(pbm_seek_data[session_type].file_ids),
              pb_cache->file_ids, sizeof(pbm_seek_data[session_type].file_ids));

      memscpy(pbm_seek_data[session_type].records_in_pb_set,
              sizeof(pbm_seek_data[session_type].records_in_pb_set),
              pb_cache->records_in_pb_set,
              sizeof(pbm_seek_data[session_type].records_in_pb_set));
      break;
    }

    case PBM_CACHE_TYPE_USIM:
    {
      PB_USIM_CACHE *usim_cache =
                    (PB_USIM_CACHE *)pbm_file_id_to_cache(session_type, file_id);

      PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);  /* KW error */
      pbm_seek_data[session_type].num_files      = 1;
      pbm_seek_data[session_type].num_of_records = usim_cache->num_of_records;
      pbm_seek_data[session_type].record_length  = usim_cache->record_length;
      pbm_seek_data[session_type].file_ids[PBM_USIM_PB_ONE] =
      usim_cache->file_id;
      pbm_seek_data[session_type].records_in_pb_set[PBM_USIM_PB_ONE] =
      usim_cache->num_of_records;
      break;
    }

    case PBM_CACHE_TYPE_INDEX:
    {
      PB_MASTER_USIM_CACHE *m_usim_cache =
           (PB_MASTER_USIM_CACHE *)pbm_file_id_to_cache(session_type,
                                                         file_id);
      PBM_CHECK_PTR_RET(m_usim_cache, PBM_ERROR);  /* KW error */
      pbm_seek_data[session_type].num_files      = 1;
      pbm_seek_data[session_type].num_of_records =
                                  m_usim_cache->iap_num_records;

      pbm_seek_data[session_type].record_length  = m_usim_cache->iap_rec_len;
      pbm_seek_data[session_type].file_ids[PBM_USIM_PB_ONE] =
                       m_usim_cache->file_id;

      pbm_seek_data[session_type].records_in_pb_set[PBM_USIM_PB_ONE] =
                    m_usim_cache->iap_num_records;
      break;
    }

    case PBM_CACHE_TYPE_SYNC2:
    {
      PB_SYNC2_CACHE *sync2_cache =
           (PB_SYNC2_CACHE *) pbm_file_id_to_cache(session_type,
                                                    file_id);
      PBM_CHECK_PTR_RET(sync2_cache, PBM_ERROR);  /* KW error */
      pbm_seek_data[session_type].num_files      = sync2_cache->num_files;
      pbm_seek_data[session_type].num_of_records = sync2_cache->num_of_records;
      pbm_seek_data[session_type].record_length  = sync2_cache->record_length;
      memscpy(pbm_seek_data[session_type].file_ids,
              sizeof(pbm_seek_data[session_type].file_ids),
              sync2_cache->file_ids,
              sizeof(pbm_seek_data[session_type].file_ids));
      memscpy(pbm_seek_data[session_type].records_in_pb_set,
              sizeof(pbm_seek_data[session_type].records_in_pb_set),
              sync2_cache->records_in_pb_set,
              sizeof(pbm_seek_data[session_type].records_in_pb_set));
      break;
    }

    default:
      return PBM_ERROR;
  }

  pbm_alloc_seek_data(session_type,
                      pbm_seek_data[session_type].num_of_records);

  client_ref =(uint32)( session_type * PBM_FILE_MAX_REQUESTS) + (uint32) file_id;
  if (PBM_SUCCESS == pbm_send_seek_request(session_type, client_ref))
  {
    return PBM_SUCCESS;
  }

  UIM_MSG_ERR_2("PBM SEEK failed of PBM file: sim %d, File id %d",
                 session_type, file_id);

  /* SEEK failed, so try reading records sequentially */
  if (PBM_SUCCESS == pbm_start_record_read(session_type, file_id, 1))
  {
    return PBM_SUCCESS;
  }

  /* Something went wrong, start reading next file */
  return PBM_ERROR;
} /* pbm_seek_unused_records */
