/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                P B M G S D I _ W R I T E.C

GENERAL DESCRIPTION
  This file contains the PBM wrapper functions for GSDI

  Copyright (c) 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE
$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbmgsdi_write.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/16   ar      Add check for cache variable NULL
12/19/16   ar      Return error if cache is not available
06/23/16   nr      ECC optimizations for IoE devices
05/19/16   sp      Logging improvements
05/16/16   sp      Logging improvements
02/01/16   nr      Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "pbm_common_types.h"
#include "pbm.h"
#include "time_jul.h"
#include "string.h"
#include "pbmgsdi.h"
#include "pbmutils.h"
#include "pbmtask_i.h"
#include "pbm_conversion.h"
#include "pbmuim.h"
#include "stringl/stringl.h"
#include "pbmlib.h"
#include "pbmutils.h"
#include "uim_msg.h"
#include "uim_pbm.h"
#include "pbmefs.h"

/*===========================================================================
                         DEFINITIONS
===========================================================================*/

/* This list contains the last known good record for each pending write. This is
 * used in bringing the cache to a sane state when a write operation fails. If a
 * record does not exist in this list, the existing cache entry is implied to be
 * the last committed record.
 * */
pbm_addr_node_s_type *pbm_committed_list = NULL;

/*===========================================================================*
 *              PHONE BOOK MANAGER TO GSDI INTERFACE FUNCTIONS.              *
 *              These function are only used internally by PBM               *
 *              and provide the interface to GSDI.                           *
 *===========================================================================*/

/*===========================================================================
FUNCTION PBM_RECOVER_USIM_RECORD

DESCRIPTION
   Restores pb cache to previous entry.  Creates a write command for restoring
   data written to SESSION.  Backup list mechanism is not used, so if this write
   command request fails, then we give up on trying to fix it.

DEPENDENCIES
   Should be called only from PBM task and from pbm_list_recover.  Only used when
   failure adding, deleting, or updating fails on a current or previous record
   entry that contains a USIM field.

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_recover_usim_record (
  const pbm_write_record_s_type *write
)
{
  pbm_write_record_s_type     *write_cmd  = NULL;
  pbm_phonebook_type           pb_id;
  PB_CACHE                    *cache      = NULL;
  pbm_return_type              ret;
  pbm_addr_cache_s_type       *prev_rec   = NULL;
  pbm_addr_cache_s_type       *record     = NULL;
  uint16                       unique_id;
  uint8                       *curr_index = NULL;
  uint8                       *prev_index = NULL;

  PBM_CHECK_PTR2_RET(write, write->prev_rec, PBM_ERROR);

  pb_id = pbm_session_record_id_to_phonebook_type(write->rec_id);
  cache = pbm_pb_id_to_cache(pb_id);

  if (!cache)
  {
    UIM_MSG_ERR_1("Invalid phonebook id %d", pb_id.device_type);
    return PBM_ERROR;
  }

  do
  {
    /* Allocate index array if USIM fields are present in previous or current
     * record and fill it.
     * There's a race condition here.  If you write 2 records back to back
     * that both require allocating USIM fields, the second one will get the
     * same copy of the IAP, and will overwrite the first record's fields */
    if (!pbm_allocate_index(write->rec_id, write->prev_rec->fields,
                            write->prev_rec->num_fields, &prev_index, &curr_index) ||
        (curr_index && !pbm_get_usim_index(write->rec_id,
                                           write->prev_rec->fields, write->prev_rec->num_fields, curr_index))
         || (prev_index && !pbm_copy_usim_index(write->rec_id, prev_index)))
    {
      UIM_MSG_ERR_3("No space available for USIM fields record %d %d %d",
                    write->rec_id, prev_index, curr_index);
      ret = PBM_ERROR_MEM_FULL;
      break;
    }

    /* Allocate PBM write command */
    write_cmd = (pbm_write_record_s_type *)
                  pbm_cmd_alloc(sizeof(pbm_write_record_s_type));
    if (!write_cmd)
    {
      UIM_MSG_ERR_0("Unable to allocate PBM command");
      ret = PBM_ERROR_MEM_FULL;
      break;
    }

    /* Get a reference to failed current record.
    This will now be our previous record entry for clean up purposes*/
    prev_rec = pbm_cache_record_get(write->rec_id);
    if (!prev_rec)
    {
      UIM_MSG_ERR_1("Unexpected NULL previous rec 0x%x", write->rec_id);
      ret = PBM_ERROR;
      break;
    }

    /* Use our old record as our restoring record */
    record = write->prev_rec;
    if (!record)
    {
      UIM_MSG_ERR_0("Unable to allocate PBM command");
      ret = PBM_ERROR;
      break;
    }

    if (pb_id.device_type == PBM_ADN)
    {
      if (prev_rec->num_fields) /* deletion or update */
      {
        (void) pbm_uim_generate_sync_values(cache, &write_cmd->sync, FALSE);
        unique_id = prev_rec->unique_id;
        /* we didn't generate new UID, so we don't care about wrap around */
      } else
      {
        unique_id = pbm_uim_generate_sync_values(cache,
                                                 &write_cmd->sync, TRUE);
        if (!unique_id)
        {
          /* Do not allow unique id to wrap around */
          UIM_MSG_ERR_0("Unable to allocate SIM unique id");
          /* This could return an error as well, but we don't use SIM UID
           * anywhere, so let's just keep going
           * This really shouldn't happen, as we regerate Sim UIDs when we
           * detect rollover inside pbm_uim_generate_sync_values() */
        }
      }
    } else
    {
      unique_id = 0xFFFF;
    }
    record->unique_id = unique_id;

    /* Restore prevoius record into pbm cache */
    ret = pbm_cache_add(cache, record, FALSE);
    if (ret != PBM_SUCCESS)
    {
      UIM_MSG_ERR_1("Cache add failed with error %d.", ret);
      break;
    }

    /* pbm_cache_add doesn't do this, so we need to do this here. */
    record->nref++;

    /* Allocate index array if USIM fields are present in previous or current
     * record and fill it */
    if (pbm_master_usim_cache_update(write->rec_id, prev_index, curr_index)
          != PBM_SUCCESS)
    {
      (void) pbm_cache_delete(cache, write->rec_id);
      UIM_MSG_ERR_1("Unable to update master usim cache for %d",
                     write->rec_id);
      ret = PBM_ERROR;
      break;
    }

    /* Build and Send restoring PBM command */
    write_cmd->cmd_type   = PBM_CMD_WRITE;
    write_cmd->rec_id     = write->rec_id;
    write_cmd->prev_rec   = prev_rec;
    write_cmd->proc_func  = NULL;
    write_cmd->user_data  = NULL;
    write_cmd->curr_rec        = pbm_cache_record_get(write->rec_id);
    write_cmd->curr_usim_index = curr_index;
    write_cmd->prev_usim_index = prev_index;
    pbm_client_cmd_q_put(PBM_CMD_PTR(write_cmd));

    ret = PBM_SUCCESS;
  } while (0);

  /* Error Handling */
  if (ret != PBM_SUCCESS)
  {
    pbm_free_index(prev_index, curr_index);
    if (write_cmd)
      pbm_cmd_free(PBM_CMD_PTR(write_cmd));
    if (prev_rec)
      (void) pbm_cache_record_free(prev_rec);
    if (record)
      (void) pbm_cache_record_free(record);
  }
  return ret;
}/* pbm_recover_usim_record */


/*===========================================================================
FUNCTION PBM_MMGSDI_UIM_WRITE

DESCRIPTION
  This function is the interface to the mmgsdi_sim_write function.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_uim_write (
  pbm_session_enum_type                session_type,
  pbm_file_type                        sim_filename,
  mmgsdi_file_structure_enum_type      sim_filetype,
  int                                  rec_num,
  byte                                *data_p,
  int                                  data_len,
  uint32                               client_ref,
  mmgsdi_callback_type                 async_cb
)
{
  mmgsdi_return_enum_type           mmgsdi_retval;
  pbm_return_type                   pbm_ret = PBM_SUCCESS;
  mmgsdi_session_id_type            mmgsdi_session_id;
  mmgsdi_write_data_type            write_data;
  mmgsdi_access_type                file_struct;

  UIM_MSG_HIGH_9("PBM: Writing to SIM file: 0x%x, record 0x%x, len 0x%x sim data: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                 sim_filename,
                 rec_num,
                 data_len,
                 ((data_p != NULL) && (data_len > 0)) ? data_p[0] : 0,
                 ((data_p != NULL) && (data_len > 1)) ? data_p[1] : 0,
                 ((data_p != NULL) && (data_len > 2)) ? data_p[2] : 0,
                 ((data_p != NULL) && (data_len > 3)) ? data_p[3] : 0,
                 ((data_p != NULL) && (data_len > 4)) ? data_p[4] : 0,
                 ((data_p != NULL) && (data_len > 5)) ? data_p[5] : 0);

  write_data.data_len = data_len;
  write_data.data_ptr = data_p;
  mmgsdi_session_id = pbm_session_info[session_type].session_id;
  if( PBM_FILE_FDN == sim_filename )
  {
    pbm_ret = pbm_populate_enum_file_access(session_type,&file_struct, sim_filename);
  }
  else
  {
    pbm_ret = pbm_populate_file_access(session_type,&file_struct, sim_filename);
  }
  if(pbm_ret != PBM_SUCCESS)
  {
    UIM_MSG_HIGH_3("error %d session_type %d sim_filename %d",pbm_ret,session_type,sim_filename);
    return PBM_ERROR;
  }
  switch (sim_filetype)
  {
    case MMGSDI_LINEAR_FIXED_FILE:
    case MMGSDI_CYCLIC_FILE:
    {
      mmgsdi_retval = mmgsdi_session_write_record (mmgsdi_session_id,
                                                   file_struct,
                                                   sim_filetype,
                                                   rec_num,
                                                   write_data,
                                                   async_cb,
                                                   client_ref);

    }
    break;

    case MMGSDI_TRANSPARENT_FILE:
      mmgsdi_retval =  mmgsdi_session_write_transparent (mmgsdi_session_id,
                                                         file_struct,
                                                         0,
                                                         write_data,
                                                         async_cb,
                                                         client_ref);
      break;

    default:
      return PBM_ERROR;
  }
  if (mmgsdi_retval != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("mmgsdi_write_record returned %d", mmgsdi_retval);
    return PBM_ERROR;
  }
  return PBM_SUCCESS;
}/* pbm_mmgsdi_uim_write */


/*===========================================================================
FUNCTION PBM_MMGSDI_UIM_WRITE_CB

DESCRIPTION
  After MMGSDI has written any SIM/USIM/CSIM record, this function is
  called. The appropriate callback function is called based on the
  file type.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_uim_write_cb(
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      data_type,
  const mmgsdi_cnf_type    *data_ptr
)
{
   uint32 client_ref = 0;

  if (!data_ptr || data_type != MMGSDI_WRITE_CNF ||
      ((client_ref = data_ptr->response_header.client_data) >= PBM_ASYNC_BUFFERS) || 
      (!pbm_buffers[client_ref].in_use))
  {
    (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
    return;
  }

  UIM_MSG_HIGH_1("pbm_mmgsdi_uim_write_cb file_id 0x%x",
   	          pbm_buffers[client_ref].record.file_id);

  switch (pbm_file_id_to_cache_type(pbm_buffers[client_ref].record.file_id))
  {
    case PBM_CACHE_TYPE_PB:
      pbm_sim_write_cb(status, data_ptr);
      break;

    case PBM_CACHE_TYPE_USIM:
      pbm_usim_write_cb(status, data_ptr);
      break;

    case PBM_CACHE_TYPE_SYNC1:
    case PBM_CACHE_TYPE_SYNC2:
      pbm_sync_write_cb(status, data_ptr);
      break;

    case PBM_CACHE_TYPE_INDEX:
      pbm_index_write_cb(status, data_ptr);
      break;

    case PBM_CACHE_TYPE_EXT:
      pbm_ext_write_cb(status, data_ptr);
      break;

      default:
         break;
   }
}/* pbm_mmgsdi_uim_write_cb */


/*===========================================================================
FUNCTION PBM_CREATE_SIM_RECORD

DESCRIPTION
  Does the writes of the non-extension record.
  If the phone number is longer than the max for the file in question
  (e.g. >ADN_MAX_PHONE_NUMBER), the remainder of the phone number was
  stored in the extension file after it was converted into BCD.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_create_sim_record(
  pbm_session_enum_type          session_type,
  pbm_file_type                  file_id,
  pbm_num_text_record_s_type    *record,
  int                            buffer_index
)
{
  pbm_return_type                         pbm_retval;
  mmgsdi_file_structure_enum_type         filetype;
  byte                                    local_pbm_data_buffer[ PBM_MAX_DATA_BLOCK_LEN ];
  pbm_fixed_sim_rec_s_type               *fixed_sim_record;
  time_julian_type                        julian_time;
  PB_CACHE                               *cache;
  int                                     index;
  int                                     cache_rec_len = 0;
  int                                     cache_text_len = 0;
  pbm_pb_set_type                         pb_set = pbm_file_id_to_pb_set(file_id);

  PBM_CHECK_PTR_RET(record,PBM_ERROR);
  cache = pbm_file_id_to_cache(session_type, file_id);
  if (cache == NULL)
  {
    UIM_MSG_ERR_2("No cache session_type = %d, file_id = %d", session_type, file_id);
    return PBM_ERROR;
  }
   
  if(pb_set != PBM_USIM_PB_INVALID)
  {
    cache_rec_len  = cache->record_length[pb_set];
    cache_text_len = cache->text_length[pb_set];
  }
  else
  {
    cache_rec_len  = cache->record_length[0];
    cache_text_len = cache->text_length[0];
  }

  index = pbm_pb_record_index_to_fileloc(cache, record->index);
  filetype = cache->file_type;

  if (file_id == PBM_FILE_NONE || index < 0)
  {
    UIM_MSG_ERR_3("Invalid cache/index %d, %d, %d", file_id, record->index,
                  cache->num_files);
    return PBM_ERROR_INDEX_INVALID;
  }

  /* Set local buffer to FF's. Therefore, all unwritten fields will be 0xFF. */
  memset ( (byte *)local_pbm_data_buffer, 0xFF,
           sizeof (local_pbm_data_buffer));
  if (((uint16*)record->sim_text)[0] != 0xFFFF)
  {
    memscpy(local_pbm_data_buffer,sizeof(local_pbm_data_buffer),
            record->sim_text,cache_text_len);
  }
  if(pbm_buffers[buffer_index].record.file_id != PBM_FILE_GAS
     && pbm_buffers[buffer_index].record.file_id != PBM_FILE_AAS)
  {
    fixed_sim_record = (pbm_fixed_sim_rec_s_type *) (local_pbm_data_buffer +
                                                    cache_text_len);

    /* The phone number length is stored in buffer[0].  Check length
     * and copy the local buffer. */
    if (record->bcd_number[0] > (PBM_SIM_DIALING_NUMBER_LEN + 1))
    {
      memscpy((byte *)&fixed_sim_record->ton_npi, 1, &record->bcd_number[1], 1);
      memscpy((byte *)fixed_sim_record->number, PBM_SIM_DIALING_NUMBER_LEN, &record->bcd_number[2],
                PBM_SIM_DIALING_NUMBER_LEN);
      fixed_sim_record->bcd_len = (PBM_SIM_DIALING_NUMBER_LEN + 1);
    } 
    else
    {
      if (record->bcd_number[0])
        memscpy((byte *)fixed_sim_record, record->bcd_number[0]+1, record->bcd_number, record->bcd_number[0]+1);
    }
	  
    fixed_sim_record->ext_id = pbm_buffers[buffer_index].extension;
    if ((cache_rec_len - cache_text_len) >= OCI_KNOWN_REC_LEN)
    {
      time_jul_from_secs(record->time, &julian_time);
      fixed_sim_record->date_time[0] = pbm_bin_to_bcd( (byte) (julian_time.year % 100) );
      fixed_sim_record->date_time[1] = pbm_bin_to_bcd( (byte) (julian_time.month) );
      fixed_sim_record->date_time[2] = pbm_bin_to_bcd( (byte) (julian_time.day) );
      fixed_sim_record->date_time[3] = pbm_bin_to_bcd( (byte) (julian_time.hour) );
      fixed_sim_record->date_time[4] = pbm_bin_to_bcd( (byte) (julian_time.minute) );
      fixed_sim_record->date_time[5] = pbm_bin_to_bcd( (byte) (julian_time.second) );
      fixed_sim_record->date_time[6] = 0xFF;  /* time zone not supported */

      fixed_sim_record->duration[0] = (byte) ((record->duration & 0xFF0000) >> 16);
      fixed_sim_record->duration[1] = (byte) ((record->duration & 0x00FF00) >> 8);
      fixed_sim_record->duration[2] = (byte) ( record->duration & 0x0000FF);

      if ((cache_rec_len - cache_text_len) >= ICI_KNOWN_REC_LEN)
      {
        /* ICI status LSB = 0 if call answered, 1 if call not answered */
        fixed_sim_record->ici_oci_ending.ici_end.ici_status =
                            ( record->is_answered ? 0x00 : 0x01 );
      }
    }
  }
  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
  pbm_retval = pbm_mmgsdi_uim_write(session_type,
                                    file_id,
                                    filetype,
                                    index,
                                    local_pbm_data_buffer,
                                    cache_rec_len,
                                    (uint32) buffer_index,
                                    pbm_mmgsdi_async_cb);

  if (pbm_retval != PBM_SUCCESS)
  {
    return PBM_ERROR;
  }

  return PBM_SUCCESS;
}/* pbm_create_sim_record */


/*===========================================================================
FUNCTION PBM_CREATE_EXT_RECORD

DESCRIPTION
  This creates an extension record, and writes it to GSDI.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_create_ext_record(
  uint32                    buffer_index,
  pbm_ext_cache_s_type     *ext_cache,
  const byte               *num_buffer,
  uint16                    m_index,
  pbm_file_type             m_filename
)
{
  byte                     *gsdi_buffer=NULL;
  pbm_session_enum_type     session_type = pbm_buffers[buffer_index].record.session_type;
  int                       extension_record;
  uint32                    extension_len;
  pbm_return_type           pbm_retval;

  PBM_CHECK_PTR2_RET(ext_cache, num_buffer, PBM_ERROR);

  PBM_MEM_ALLOC(gsdi_buffer, (uint32)ext_cache->ext_rec_length);
  PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
  memset(gsdi_buffer, 0xFF, (uint32)ext_cache->ext_rec_length);

  /* buffer contains the bcd in the format    */
  /* byte  0      1      2 thru buffer[0]     */
  /*     <len><ton/npi><bcd packed number>    */
  /* Note that length is the length including */
  /* the TON/NPI field.                       */

  extension_record = pbm_find_extension_record(ext_cache, m_index,
                                                m_filename);
  if (!extension_record)
  {
    UIM_MSG_ERR_3("pbm_find_extension_record() failed %d %d %d", ext_cache, m_index, m_filename);
    PBM_MEM_FREEIF(gsdi_buffer);
    return PBM_ERROR_MEM_FULL;
  }

  pbm_buffers[buffer_index].extension = (byte) extension_record;
  extension_len = MIN((num_buffer[0] - 1) - PBM_SIM_DIALING_NUMBER_LEN,
                       PBM_SIM_DIALING_NUMBER_LEN);

  gsdi_buffer[0] = (byte) EXT_REC_TYPE_ADDITIONAL_DATA;
  gsdi_buffer[1] = (byte) extension_len;

  memscpy(&gsdi_buffer[2], extension_len, &num_buffer[PBM_SIM_DIALING_NUMBER_LEN + 2],
          extension_len);

  pbm_buffers[buffer_index].record.file_id = ext_cache->file_id;
  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
  pbm_retval = pbm_mmgsdi_uim_write ( session_type,
                                      ext_cache->file_id,
                                      ext_cache->file_type,
                                      extension_record,
                                      gsdi_buffer,
                                      EXT_KNOWN_REC_LEN,
                                      (uint32) buffer_index,
                                      pbm_mmgsdi_async_cb);

  PBM_MEM_FREEIF(gsdi_buffer);
  return pbm_retval;
}/* pbm_create_ext_record */


/*===========================================================================
FUNCTION PBM_CLEAR_EXT_RECORD

DESCRIPTION
  This finds the extension record associated with a record, and
  writes a blank record in its place to GSDI.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_clear_ext_record(
  uint32                  buffer_index,
  pbm_ext_cache_s_type   *ext_cache,
  uint16                  m_index,
  pbm_file_type           m_filename
)
{
  byte                            *gsdi_buffer=NULL;
  pbm_session_enum_type            session_type = pbm_buffers[buffer_index].record.session_type;
  int                              extension_record, i=0;
  pbm_return_type                  pbm_retval;
  pbm_g_other_field_info_s_type   *field_info;

  PBM_CHECK_PTR_RET(ext_cache, PBM_ERROR_MEM_FULL);

  PBM_MEM_ALLOC(gsdi_buffer, (uint32)ext_cache->ext_rec_length);
  PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
  memset(gsdi_buffer, 0xFF, (uint32)ext_cache->ext_rec_length);

  /* buffer contains the bcd in the format    */
  /* byte  0      1      2 thru buffer[0]     */
  /*     <len><ton/npi><bcd packed number>    */
  /* Note that length is the length including */
  /* the TON/NPI field.                       */

  extension_record = pbm_free_extension_record(ext_cache,
                                               m_index,
                                               m_filename);

  if (!extension_record)
  {
    UIM_MSG_ERR_3("pbm_free_extension_record() failed %d %d %d", ext_cache, m_index, m_filename);
    PBM_MEM_FREEIF(gsdi_buffer);
    return PBM_ERROR;
  }
  /* Since the one of the extension record has been free
     PBM can make the number length as 42 for ADN/FDN field info */
  field_info = &g_sim_field_info[session_type][0]; /* ADN info */
  for ( i = 0; i < field_info->num_fields; i++)
  {
    if (field_info->info[i].field_type == PBM_FT_PHONE)
    {
      //increment by 2 to accomodate '+' sign & NULL char
      field_info->info[i].max_length = 2 + PBM_MAX_NUM_LENGTH;
    }
  }
  field_info = &g_sim_field_info[session_type][1]; /* FDN info*/
  for ( i = 0; i < field_info->num_fields; i++)
  {
    if (field_info->info[i].field_type == PBM_FT_PHONE)
    {
      //increment by 2 to accomodate '+' sign & NULL char
      field_info->info[i].max_length = 2 + PBM_MAX_NUM_LENGTH;
    }
  }
  pbm_buffers[buffer_index].extension = (byte) 0xff;
  gsdi_buffer[0] = (byte) EXT_REC_TYPE_FREE;
  pbm_buffers[buffer_index].record.file_id = ext_cache->file_id;
  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
  pbm_retval = pbm_mmgsdi_uim_write ( session_type,
                                      ext_cache->file_id,
                                      ext_cache->file_type,
                                      extension_record,
                                      gsdi_buffer,
                                      EXT_KNOWN_REC_LEN,
                                      (uint32) buffer_index,
                                      pbm_mmgsdi_async_cb);

  PBM_MEM_FREEIF(gsdi_buffer);
  return pbm_retval;
}/* pbm_clear_ext_record */


/*===========================================================================
FUNCTION PBM_CREATE_ANR_RECORD

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_create_anr_record(
  pbm_session_enum_type          session_type,
  pbm_file_type                  file_id,
  pbm_anr_record_s_type         *anr_record,
  uint32                         buffer_index
)
{
  byte                                            *gsdi_buffer=NULL;
  PB_USIM_CACHE                                   *usim_cache;
  pbm_async_gsdi_buffer                           *buffer = &pbm_buffers[buffer_index];
  uint16                                           index;
  mmgsdi_file_structure_enum_type                  filetype;
  pbm_return_type                                  pbm_retval;
  byte                                            *num_buffer;
  pbm_fixed_sim_rec_s_type                        *sim_rec;

  PBM_CHECK_PTR_RET(anr_record, PBM_ERROR);
  usim_cache = pbm_file_id_to_cache(session_type, file_id);
  PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);

  PBM_MEM_ALLOC(gsdi_buffer, (uint32) usim_cache->record_length);
  PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
  memset(gsdi_buffer, 0xFF, (uint32) usim_cache->record_length);
  filetype = usim_cache->file_type;
  index = (uint16) buffer->record.index;
  sim_rec = (pbm_fixed_sim_rec_s_type *) &gsdi_buffer[1];
  if (anr_record->anr_data[1] != 0)
  {
    /* Updating the ANR record identifier with the following values
     * - '00' – no additional number description;
     * - 'xx' – record number in EFAAS describing the type of number (e.g. "FAX");
     * - 'FF' – free record.
     */
    gsdi_buffer[0] = anr_record->anr_data[0];
    num_buffer = &anr_record->anr_data[1];

    if (num_buffer[0] > (PBM_SIM_DIALING_NUMBER_LEN + 1))
    {
      memscpy((byte *)&sim_rec->ton_npi, 1, (byte *)&num_buffer[1],1);
      memscpy((byte *)sim_rec->number, PBM_SIM_DIALING_NUMBER_LEN, (byte *)&num_buffer[2],
              PBM_SIM_DIALING_NUMBER_LEN);
      sim_rec->bcd_len = (PBM_SIM_DIALING_NUMBER_LEN + 1);
    } else
    {
      memscpy((byte *)sim_rec, num_buffer[0]+1, num_buffer, num_buffer[0]+1);
    }
  }

  sim_rec->ext_id = pbm_buffers[buffer_index].extension;
  if (usim_cache->mapping_type == PBM_MAPPING_TYPE_2 &&
      pbm_buffers[buffer_index].state == PBM_WRITE_CURR_USIM)
  {
    pbm_retval = pbm_fill_adn_sfi(anr_record->adn_record_id,
                                  &gsdi_buffer[usim_cache->record_length-2],session_type);
    if (PBM_SUCCESS != pbm_retval)
    {
      UIM_MSG_ERR_1("Error %d in filling ADN SFI", pbm_retval);
      PBM_MEM_FREEIF(gsdi_buffer);
      return PBM_ERROR;
    }
  }

  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
  pbm_retval = pbm_mmgsdi_uim_write(session_type,
                                    file_id,
                                    filetype,
                                    index,
                                    gsdi_buffer,
                                    usim_cache->record_length,
                                    buffer_index,
                                    pbm_mmgsdi_async_cb);

  PBM_MEM_FREEIF(gsdi_buffer);
  return pbm_retval;
}/* pbm_create_anr_record */


/*===========================================================================
FUNCTION PBM_FIND_EXTENSION_RECORD

DESCRIPTION
  When its time to pick an extension record, this routine will find one.
  It returns the record it found, or 0.

PARAMETERS:

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_find_extension_record(
  pbm_ext_cache_s_type *ext_cache, 
  int                   index,
  pbm_file_type         file_id
)
{
  int                        i;        /* Control variable for loop */
  int                        ext_num_of_rec;
  pbm_ext_rec_in_use_s_type *ext_rec_in_use;
  int                        extension_record = 0;

  PBM_CHECK_PTR_RET(ext_cache, 0);

  ext_num_of_rec = ext_cache->num_of_rec;
  ext_rec_in_use = ext_cache->in_use_array;

  /* Now search through the extension records to see if there is
   * any available.
   */
  for (i = ext_num_of_rec; i > 0; i--)
  {
    /* Is the current record already using an extension record? */
    if ((ext_rec_in_use[i].uim_device == file_id) &&
        (ext_rec_in_use[i].index == index))
    {
      extension_record = i;
      break;
    } else /* pick a new extension record */
    {
      if (ext_rec_in_use[i].index == 0)
          extension_record = i;
    }
  }

  if (extension_record==0)
  {
    UIM_MSG_ERR_2("No more Extension records available %d %d", ext_num_of_rec, file_id);
  }

  return extension_record;
}/* pbm_find_extension_record */


/*===========================================================================
FUNCTION PBM_CHECK_EXTENSION_RECORD

DESCRIPTION
  This routine will return TRUE if a record is currently using an extension
  record, or FALSE if not.

PARAMETERS:

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_check_extension_record(
  pbm_ext_cache_s_type *ext_cache, 
  int                   index,
  pbm_file_type         file_id
)
{
  int                         i;
  int                         ext_num_of_rec;
  pbm_ext_rec_in_use_s_type  *ext_rec_in_use;

  PBM_CHECK_PTR_RET(ext_cache, FALSE);

  ext_num_of_rec = ext_cache->num_of_rec;
  ext_rec_in_use = ext_cache->in_use_array;

  /* Now search through the extension records */
  for (i = ext_num_of_rec; i > 0; i--)
  {
    /* Is the current record the right one? */
    if ((ext_rec_in_use[i].uim_device == file_id) &&
        (ext_rec_in_use[i].index == index))
    {
      return i;
    }
  }

  /* if we got here, we didn't find it */
  return 0;
}/* pbm_check_extension_record */


/*===========================================================================
FUNCTION pbm_record_id_to_device_type

DESCRIPTION
Converts a record ID to a phonebook ID (extracts the phonebook ID information
from the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static pbm_device_type pbm_record_id_to_device_type(
  pbm_record_id_type record_id
)
{
  uint32          phonebook;
  pbm_device_type retval = PBM_RESERVED;

  /* Get upper 5 bits of record ID*/
  phonebook = (record_id >> PBM_NUM_LOC_BITS) & PBM_PB_MASK;

  if(phonebook < ARR_SIZE(pbm_pb_bits_to_pb_id))
  {
    retval = pbm_pb_bits_to_pb_id[phonebook];
  }

  if (retval == PBM_RESERVED)
  {
    UIM_MSG_ERR_1("Unknown phonebook code 0x%x.", phonebook);
    retval = PBM_EFS;
  }

  return retval;
}/* pbm_record_id_to_device_type */


/*===========================================================================
FUNCTION PBM_INT_RECORD_WRITE

DESCRIPTION
  This function performs the actual write of the record to UIM or EFS.
  It is always called from within PBM Task. The address record is already
  added to cache from within the library function.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_int_record_write (
  pbm_write_record_s_type      *write
)
{
  pbm_phonebook_type              pb_id = {PBM_GPB};
  pbm_device_type                 device_type;
  pbm_record_id_type              rec_id;
  pbm_return_type                 ret_val = PBM_ERROR ;
  pbm_addr_cache_s_type          *record;
  int                             fields_size;
  int                             index;
  pbm_device_type                 outgoing_call_mirror[] = { PBM_LND, PBM_OCI};
  pbm_device_type                 incoming_call_mirror[] = { PBM_ICI};
  pbm_device_type                *mirror = NULL;
  int                             mirror_size = 0;
  PB_CACHE                       *cache;

  PBM_CHECK_PTR_RET(write, PBM_ERROR);
  rec_id = write->rec_id;
  device_type = (pbm_device_type) pbm_record_id_to_device_type(rec_id);
  pb_id = pbm_session_record_id_to_phonebook_type(rec_id);

  /* This can happen when GSDI sends a refresh */
  if (PBM_SUCCESS != (ret_val = check_pb_ready(pb_id)))
  {
    pbm_write_notify(write, ret_val);
    pbm_write_cmd_free(write);
    return ret_val;
  }

  /* Mirror callhistory entry to SIM if it is not a deletion.
   * Because sizes may be different between the SIM and
   * EFS versions, deletions in one file do not necessarily mean
   * a record will be deleted from the mirror file.  Thus, if the
   * SIM file must be deleted, that needs to be done seperately.
   */
  record = write->curr_rec;
  if ( record != NULL )
  {
    if (record->num_fields)
    {
      switch (device_type)
      {
        case PBM_DIALED:
          mirror = outgoing_call_mirror;
          mirror_size = ARR_SIZE(outgoing_call_mirror);
          break;

        case PBM_RCVD:
        case PBM_MISS:
          mirror = incoming_call_mirror;
          mirror_size = ARR_SIZE(incoming_call_mirror);
          break;

        default:
          mirror = NULL;
          mirror_size = 0;
          break;
      } /*  switch(pb_id)... */

      if (NULL != mirror)
      {
        for (index = 0; index < mirror_size; index++)
        {
          pbm_phonebook_type mirror_pb_id = pb_id;

		  mirror_pb_id.device_type = mirror[index];
          cache = pbm_pb_id_to_cache(mirror_pb_id);
          if (!cache)
            continue;

          if (0 < cache->num_of_records)
          {
            rec_id = PBM_SESSION_GET_FIRST_AVAILABLE;
            /* Write the fields to SIM cache too.
             * pbm_record_write() makes a copy */
            fields_size = pbm_calculate_size_from_fields(record->fields,
                                                         record->num_fields);

            ret_val = pbm_session_record_write (mirror_pb_id, &rec_id,
                                                (uint16) PBM_CAT_NONE,
                                                (uint32) record->num_fields,
                                                (uint8*) record->fields,
                                                fields_size, NULL, NULL);
            if (PBM_SUCCESS != ret_val)
            {
              UIM_MSG_ERR_2("Mirroring Call hist to Session %d file failed with error %d",
                             mirror[index], ret_val);
            }
          }
        }
      }
    }

    /* Now do actual write */
    switch (pb_id.device_type)
    {
      case PBM_DIALED:
      case PBM_RCVD:
      case PBM_MISS:
      case PBM_EFS:
      case PBM_SDL:
         ret_val = pbm_record_write_to_efs(write);
         break;

      case PBM_ADN:
      case PBM_SDN:
      case PBM_FDN:
      case PBM_LND:
      case PBM_MSISDN:
      case PBM_MBN:
      case PBM_GRPNAME:
      case PBM_AAS:
      case PBM_OCI:
      case PBM_ICI:
      case PBM_MBDN:
         ret_val = pbm_record_write_to_sim(write);
         break;

      default:
         UIM_MSG_ERR_1("Unrecognized phonebook for write, %d.",
                     pb_id.device_type);
         ret_val = PBM_ERROR;
         break;
    } /* end switch */
  }
  if (ret_val != PBM_SUCCESS)
  {
    pbm_write_notify(write, ret_val);
    pbm_write_cmd_free(write);
  }
  return ret_val;
}/* pbm_int_record_write */


/*===========================================================================
FUNCTION PBM_WRITE_CMD_FREE

DESCRIPTION
  This function frees any references to cache records and frees
  the command structure itself. It also signals the PBM task to go
  ahead and process the next command.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_write_cmd_free(
  pbm_write_record_s_type    *write
)
{
  PBM_CHECK_PTR_RET(write, VOID);

  if (write->prev_rec)
    (void) pbm_cache_record_free(write->prev_rec);

  if (write->curr_rec)
    (void) pbm_cache_record_free(write->curr_rec);

  PBM_MEM_FREEIF(write->curr_usim_index);
  PBM_MEM_FREEIF(write->prev_usim_index);

  pbm_cmd_free(PBM_CMD_PTR(write));
  (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
} /* pbm_write_cmd_free */


/*===========================================================================
FUNCTION PBM_WRITE_NOTIFY

DESCRIPTION
  This function sends the asynchronous result of a PBM write operation.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_write_notify (
  const pbm_write_record_s_type *write,
  pbm_return_type                ret_val
)
{
  pbm_notify_event_e_type        event;
  pbm_writecb_data_s_type        cb_data;
  pbm_session_event_data_u_type  evt_data = {0};
  pbm_phonebook_type             pb_id;
  PB_CACHE                      *cache = NULL;


  PBM_CHECK_PTR_RET(write, VOID);

  /* notify async callback passed in with write cmd */
  cb_data.ret = ret_val;
  //since this is the old rec_id we would need just 16 LSBs
  (void)pbm_new_recid_to_old_recid(&cb_data.rec_id,write->rec_id);
  //copy the new 32 bit rec_id
  cb_data.sess_rec_id = write->rec_id;
  cb_data.user_data = write->user_data;

  if (write->proc_func)
    (*write->proc_func)(&cb_data);

  /* Send notifications using registered functions */
  if (PBM_SUCCESS == ret_val)
  {
    /* Check # fields to find if there is a record.
     * If both current and previous records are present, this is an update.
     * If current record is present and previous record is not present,
     * this is an add operation. Otherwise this is a deletion.
     */
    if (write->curr_rec && write->curr_rec->num_fields)
    {
      if (write->prev_rec && write->prev_rec->num_fields)
        event = PBM_EVENT_REC_UPDATE;
      else
        event = PBM_EVENT_REC_ADD;
    } else
    {
      event = PBM_EVENT_REC_DELETE;
    }
  } else
  {
    event = PBM_EVENT_REC_FAILED;
  }
  evt_data.rec_id = write->rec_id;
  pb_id = pbm_session_record_id_to_phonebook_type(write->rec_id);
  cache = pbm_pb_id_to_cache(pb_id);
  PBM_CHECK_PTR_RET(cache,VOID);

  if ( !( (cache->status == PBM_STATUS_INIT_SYNC) && (pb_id.device_type == PBM_ADN)))
  {
    pbm_add_notify(event, evt_data);
  }
  else
  {
    UIM_MSG_HIGH_1("Event not notified for rec_id 0x%x",write->rec_id);
  }
}/* pbm_write_notify */


/*===========================================================================
FUNCTION PBM_LIST_BACKUP

DESCRIPTION
  This function makes sure that there is a valid entry in the committed list for
  this record id. If there are no pending writes on this record, the current
  cache entry is saved to committed list. If the previous entry is a NULL, we
  replace it with a dummy entry.

DEPENDENCIES
  Cache should be initialized before. Record id should be valid.
  Should be called from within pbm_crit_sect.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_list_backup(
  pbm_record_id_type     rec_id
)
{
  PB_CACHE              *cache = pbm_pb_id_to_cache(pbm_session_record_id_to_phonebook_type(rec_id));
  uint16                 location = pbm_record_id_to_location(rec_id);
  pbm_addr_node_s_type  *curr_node = NULL;
  pbm_addr_node_s_type  *prev_node = NULL;
  pbm_addr_node_s_type  *node = NULL;
  pbm_addr_cache_s_type *dummy = NULL;
  int                    committed_list_size = 0;

  PBM_CHECK_PTR_RET(cache, PBM_ERROR);

  if (PBM_SESSION_GET_FIRST_AVAILABLE == rec_id)
    return PBM_ERROR;

  /* Search for a node with given record id or go till the end */
  curr_node = pbm_committed_list;
  while (curr_node)
  {
    if (pbm_compare_rec_ids(curr_node->rec_id , rec_id))
      return PBM_SUCCESS; /* We are done */
    prev_node = curr_node;
    curr_node = curr_node->next;
  }

  /* Allocate a new node */
  PBM_MEM_ALLOC(node, sizeof(pbm_addr_node_s_type));
  PBM_CHECK_PTR_RET(node, PBM_ERROR_MEM_FULL);
  PBM_CHECK_PTR_RET(cache->pb_cache_array, PBM_ERROR);

  /* If entry is NULL, add a dummy entry in cache for future comparison */
  if (!cache->pb_cache_array[location])
  {
    dummy = pbm_cache_record_new();
    if (!dummy)
    {
      PBM_MEM_FREEIF(node);
      return PBM_ERROR_MEM_FULL;
    }
    pbm_cache_record_fill(dummy, rec_id, 0, 0, 0, NULL);
    cache->pb_cache_array[location] = dummy;
  }

  node->rec_id = rec_id;
  node->record = pbm_cache_record_get(rec_id);
  node->next = NULL;

  /* Add node to committed list (end of list) */
  if (prev_node)
    prev_node->next = node;
  else
    pbm_committed_list = node;

  /* check committed list size */
  node = pbm_committed_list;
  committed_list_size = 0;
  while (node != NULL)
  {
    committed_list_size++;
    node = node->next;
  }
  if (committed_list_size > 10)
  {
    UIM_MSG_ERR_1("Committed list unusual size %d", committed_list_size);
  }
  return PBM_SUCCESS;
}/* pbm_list_backup */


/*===========================================================================
FUNCTION PBM_LIST_UPDATE

DESCRIPTION
  This function updates the PBM committed list after a successful write.

  If the cache entry is same as current write record, there are no other
  pending writes. So we delete the corresponding node from within PBM
  committed list. A dummy record wthin cache is also relaplced with the
  original NULL entry.

  If cache entry is different, we update the corresponding node within
  committed list with the latest record.

DEPENDENCIES
  No critical section dependencies.

SIDE EFFECTS
===========================================================================*/
void pbm_list_update(
  const pbm_write_record_s_type *write
)
{
  pbm_addr_cache_s_type             *cache_rec;
  PB_CACHE                          *cache;
  pbm_addr_node_s_type              *curr_node = NULL;
  pbm_addr_node_s_type              *prev_node = NULL;
  boolean                            found = FALSE;
  uint16                             location;

  PBM_CHECK_PTR_RET(write, VOID);

  cache = pbm_pb_id_to_cache(pbm_session_record_id_to_phonebook_type(write->rec_id));
  PBM_CHECK_PTR2_RET(cache, cache->pb_cache_array, VOID);

  location = pbm_record_id_to_location(write->rec_id);
  if (location > cache->num_of_records)
  {
    UIM_MSG_HIGH_3("Location for record ID out of bounds %d of %d",
                   write->rec_id, location, cache->num_of_records);
    return;
  }

  rex_enter_crit_sect(&pbm_crit_sect);
  cache_rec = cache->pb_cache_array[location];

  if (!cache_rec)
  {
    UIM_MSG_HIGH_1("NULL cache entry found for record id %d",
                   write->rec_id);
    rex_leave_crit_sect(&pbm_crit_sect);
    return;
  }

  /* Search for the record id within committed list */
  PBM_LIST_SEARCH(write->rec_id, prev_node, curr_node, found);
  if (!found || !curr_node)
  {
    UIM_MSG_HIGH_2("Can't find entry in committed list for rec 0x%x, node 0x%x",
                   write->rec_id, curr_node);
    rex_leave_crit_sect(&pbm_crit_sect);
    return;
  }

  if (cache_rec == write->curr_rec)
  {
    /* There are no more pending writes.
     * Remove node from committed list. */
    (void) pbm_cache_record_free(curr_node->record);
    PBM_LIST_REMOVE(prev_node, curr_node);

    /* Remove dummy record */
    if (!cache_rec->num_fields)
    {
      (void) pbm_cache_record_free(cache_rec);
      cache->pb_cache_array[location] = NULL;
    }
  } else
  {
    /* Update node with new record */
    (void) pbm_cache_record_free(curr_node->record);
    curr_node->record = write->curr_rec;
    curr_node->record->nref++;
  }
  rex_leave_crit_sect(&pbm_crit_sect);
}/* pbm_list_update */


/*===========================================================================
FUNCTION PBM_LIST_RECOVER

DESCRIPTION
   This function is called to recover a cache entry in response to a write error.
   The entry is obtained from the PBM committed list.

   If cache entry is same as current write record, there are no other pending
   writes. In this case we have to do the recovery operation. In the normal case
   we take the record from the corresponding node in commit list and add it to
   the cache.

   If the current operation was partially successful ie deletion was successful
   and add failed, we delete the current cache entry. This can happen only
   for an Update operation on EFS or while adding USIM entries.

   In both cases above, we no longer need the node in commiitted list after this
   and hence the node is removed. Also, if the entry in cache is a dummy record,
   it is converted to a NULL.

DEPENDENCIES
   None. No need to be in critical section to call this function.

SIDE EFFECTS
===========================================================================*/
void pbm_list_recover (
  const pbm_write_record_s_type *write,
  boolean                        partial_success
)
{
  PB_CACHE                       *cache;
  pbm_addr_cache_s_type          *cache_rec;
  pbm_addr_node_s_type           *curr_node = NULL;
  pbm_addr_node_s_type           *prev_node = NULL;
  uint16                          location;
  boolean                         found;
  pbm_return_type                 ret;

  PBM_CHECK_PTR_RET(write, VOID);

  cache = pbm_pb_id_to_cache(pbm_session_record_id_to_phonebook_type(write->rec_id));
  PBM_CHECK_PTR2_RET(cache, cache->pb_cache_array, VOID);

  location = pbm_record_id_to_location(write->rec_id);
  rex_enter_crit_sect(&pbm_crit_sect);

  cache_rec = cache->pb_cache_array[location];
  if (!cache_rec)
  {
    UIM_MSG_HIGH_1("NULL cache entry found for record %d", write->rec_id);
    rex_leave_crit_sect(&pbm_crit_sect);
    return;
  }

  /* make sure there isn't another write to the same record pending */
  if (write->curr_rec == cache_rec)
  {
    /* Search for the record id within committed list */
    PBM_LIST_SEARCH(write->rec_id, prev_node, curr_node, found);
    if (!found || !curr_node)
    {
      UIM_MSG_HIGH_2("Can't find entry in committed list- rec 0x%x, node 0x%x",
                      write->rec_id, curr_node);
      rex_leave_crit_sect(&pbm_crit_sect);
      return;
    }

    if (partial_success)
    {
      if ((write->curr_rec && pbm_is_usim_required(write->rec_id,
                                                    write->curr_rec->fields,
                                                    write->curr_rec->num_fields)) ||
          (write->prev_rec && pbm_is_usim_required(write->rec_id,
                                                   write->prev_rec->fields,
                                                   write->prev_rec->num_fields)))
      {
        /*Recover USIM record when Add, Delete or Update operation failed*/
        ret = pbm_recover_usim_record(write);
        PBM_CHECK_ERR(ret, "USIM Record Recovery after error failed");
      } else
      {
        /* Deletion was successful and Add failed for EFS entries.
         * We ignore the commit list in this case. */
        ret = pbm_cache_delete(cache, write->rec_id);
        PBM_CHECK_ERR(ret, "Recovery after partial update failed");
      }
    } else
    {
      /* Normal recovery case when Add, Delete or Update operation failed.
       * In this case we add entry from commit list to the cache. */
      ret = pbm_cache_add(cache, curr_node->record, FALSE);
      PBM_CHECK_ERR(ret, "Recovery after error failed");
      curr_node->record->nref++;
    }

    /* we are done with the node in committed list, so it can be removed */
    (void) pbm_cache_record_free(curr_node->record);
    PBM_LIST_REMOVE(prev_node, curr_node);

    /* Remove dummy record, if present */
    if (cache->pb_cache_array[location] &&
        !cache->pb_cache_array[location]->num_fields)
    {
      (void) pbm_cache_record_free(cache->pb_cache_array[location]);
      cache->pb_cache_array[location] = NULL;
    }
  }
  rex_leave_crit_sect(&pbm_crit_sect);
}/* pbm_list_recover */
