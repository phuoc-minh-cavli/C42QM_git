/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

				  P B M G S D I _ R E A D.C

GENERAL DESCRIPTION
  This file contains the PBM wrapper functions for GSDI

  Copyright (c) 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE
$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbmgsdi_read.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
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
#include "pbmefs.h"
#include "stringl/stringl.h"
#include "pbmlib.h"
#include "uim_msg.h"
#include "uim_pbm.h"
#include "cmutil.h"

/*===========================================================================
                         DEFINITIONS
===========================================================================*/
typedef PACKED struct
{
  byte num_digits;
  byte number[8]; /* BCD coding */
  byte ton_npi;
  byte pi_si;	  /* 0xFF if not used */
} pbm_fixed_mdn_rec_s_type;


/*===========================================================================*
 *              PHONE BOOK MANAGER TO GSDI INTERFACE FUNCTIONS.              *
 *              These function are only used internally by PBM               *
 *              and provide the interface to GSDI.                           *
 *===========================================================================*/

/*===========================================================================
FUNCTION pbm_mmgsdi_process_mdn_data

DESCRIPTION
 This is the special processing for the CSIM MSISDN file data 
 The file is EF MDN for CSIM 
 Base bytes 11 

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static boolean pbm_mmgsdi_process_mdn_data(
  pbm_session_enum_type          session_type,
  pbm_file_type                  file_id,
  int                            index,
  byte                          *local_pbm_data_buffer,
  pbm_num_text_record_s_type    *return_data
)
{
  pbm_fixed_mdn_rec_s_type   *fixed_mdn_record;
  char                        number[PBM_NUM_SIZE]; /* + and null for string */
  PB_CACHE                   *cache = (PB_CACHE *)pbm_file_id_to_cache(session_type, file_id);

  PBM_CHECK_PTR3_RET(return_data, local_pbm_data_buffer, cache, FALSE);

  /* Initialize return record to 0's */
  memset(return_data, 0, sizeof(pbm_num_text_record_s_type));

  /* Store the index as this index. */
  return_data->index = index;

  /* Point to the SIM record starting at the number. */
  fixed_mdn_record = (pbm_fixed_mdn_rec_s_type *) local_pbm_data_buffer;
  //interchange nibbles of TON NPI ,to be compatible with GW order
  fixed_mdn_record->ton_npi = (fixed_mdn_record->ton_npi & 0x07) << 4 |
                              (fixed_mdn_record->ton_npi & 0x78) >> 3;
  fixed_mdn_record->num_digits = fixed_mdn_record->num_digits & 0x0F;
  /* If the number is not blank */
  if (fixed_mdn_record->num_digits != 0)
  {
    /* Convert the number to ASCII. */
    if (!pbm_mdn_to_ascii(fixed_mdn_record->number,
                          (fixed_mdn_record->num_digits+1)/2,
                           fixed_mdn_record->ton_npi,
                           (byte *) number ))
    {
      UIM_MSG_ERR_1("SIM ERROR record %d could not be converted to ascii", index);
    }
    if (!pbm_ascii_to_bcd((byte *)number, fixed_mdn_record->ton_npi, return_data->bcd_number))
      return FALSE;
      
  }
  return TRUE;          /* Processing of this buffer is correct. */
}/* pbm_mmgsdi_process_mdn_data */


/*===========================================================================
FUNCTION PBM_MMGSDI_SIM_READ

DESCRIPTION
  This function is the interface to the gsdi_sim_read function.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_sim_read(
  pbm_session_enum_type               pbm_session_type,
  pbm_file_type                       sim_filename,
  mmgsdi_file_structure_enum_type     sim_filetype,
  int                                 rec_num,
  int                                 required_data_len,
  uint32                              client_ref,
  mmgsdi_callback_type                async_cb
)
{
  mmgsdi_return_enum_type          mmgsdi_retval;
  pbm_return_type                  pbm_ret = PBM_SUCCESS;
  mmgsdi_session_id_type           session_id;
  mmgsdi_access_type               file_struct;

  if ( PBM_FILE_FDN == sim_filename )
  {
   pbm_ret = pbm_populate_enum_file_access(pbm_session_type,&file_struct, sim_filename);
  }
  else
  {
    pbm_ret = pbm_populate_file_access(pbm_session_type,&file_struct, sim_filename);
  }

  if(pbm_ret != PBM_SUCCESS)
  {
    UIM_MSG_HIGH_3("PBM: in pbm_mmgsdi_sim_read pbm_populate_file_access returned error %d, no file path found for file %d, session %d",pbm_ret,sim_filename,pbm_session_type);
    return PBM_ERROR;
  }
  session_id = pbm_session_info[pbm_session_type].session_id;
  switch (sim_filetype)
  {
    case MMGSDI_LINEAR_FIXED_FILE:
    case MMGSDI_CYCLIC_FILE:
      mmgsdi_retval = mmgsdi_session_read_record (session_id,
                                                  file_struct,
                                                  rec_num,
                                                  required_data_len,
                                                  async_cb,
                                                  client_ref);
      break;

    case MMGSDI_TRANSPARENT_FILE:
      mmgsdi_retval = mmgsdi_session_read_transparent (session_id,
                                                       file_struct,
                                                       0,
                                                       required_data_len,
                                                       async_cb,
                                                       client_ref);
      break;

    default:
      UIM_MSG_ERR_1("pbm_mmgsdi_sim_read got invalid file type %d", sim_filetype);
      return PBM_ERROR;
  }

  if (mmgsdi_retval != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("mmgsdi_read_record returned %d", mmgsdi_retval);
    return PBM_ERROR;
  }
  return PBM_SUCCESS;
}/* pbm_mmgsdi_sim_read */


/*===========================================================================
FUNCTION PBM_MMGSDI_SESSION_GET_FILE_ATTRIBUTES

DESCRIPTION
  This function is the interface to the gsdi_get_file_attributes function.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_get_file_attributes (
  pbm_session_enum_type          session_type,
  pbm_file_type                  sim_filename,
  uint32                         client_ref,
  mmgsdi_callback_type           async_cb
)
{
  mmgsdi_return_enum_type   mmgsdi_retval;
  pbm_return_type           pbm_ret = PBM_SUCCESS;
  mmgsdi_access_type        file_access_info;
  boolean                   skip_uicc_arr = TRUE;

  memset(&file_access_info, 0, sizeof(file_access_info));

  if ( PBM_FILE_FDN == sim_filename )
  {
    pbm_ret = pbm_populate_enum_file_access(session_type,&file_access_info, sim_filename);
  }
  else
  {
    pbm_ret = pbm_populate_file_access(session_type,&file_access_info, sim_filename);
  }

  if(pbm_ret != PBM_SUCCESS)
  {
    UIM_MSG_HIGH_3("error %d session_type %d sim_filename %d",pbm_ret,session_type,sim_filename);
    return PBM_ERROR;
  }
  if(pbm_file_id_to_cache_type(sim_filename) == PBM_CACHE_TYPE_PB)
  {
    //For only phonebook EFs we need the security attributes .
    //For the rest of EFs such as EXT1 , EXT2 , EMAIL , ANR etc ..
    //will inherit the attributes from the Phonebook
    //which they belong to
    //Hence we doesnt need to read
    //security attributes for EFs
    skip_uicc_arr = FALSE;
  }
  mmgsdi_retval =  mmgsdi_session_get_file_attr_ext (
                                                pbm_session_info[session_type].session_id,
                                                file_access_info,
                                                async_cb,
                                                client_ref,
                                                skip_uicc_arr);
  if (mmgsdi_retval != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("mmgsdi_session_get_file_attr_ext returned %d", mmgsdi_retval);
    return PBM_ERROR;
  }
  return PBM_SUCCESS;
}/* pbm_mmgsdi_get_file_attributes */


/*===========================================================================
FUNCTION PBM_MMGSDI_SESSION_GET_FILE_ATTRIBUTES_PBR

DESCRIPTION
  This function is the interface to the gsdi_get_file_attributes function.
  It should only be used for the files which are to be accessed via enum.
  PBR is the only file as per now.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_get_file_attributes_pbr (
  pbm_session_enum_type  session_type,
  mmgsdi_file_enum_type  sim_filename,
  uint32                 client_ref,
  mmgsdi_callback_type   async_cb
)
{
  mmgsdi_return_enum_type   mmgsdi_retval;
  mmgsdi_access_type        file_access_info;

  memset(&file_access_info, 0, sizeof(file_access_info));
  file_access_info.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access_info.file.file_enum = sim_filename;
  mmgsdi_retval =  mmgsdi_session_get_file_attr_ext (
                                                 pbm_session_info[session_type].session_id,
                                                 file_access_info,
                                                 async_cb,
                                                 client_ref,
                                                 TRUE);
  if (mmgsdi_retval != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("mmgsdi_session_get_file_attr_ext returned %d", mmgsdi_retval);
    return PBM_ERROR;
  }
  return PBM_SUCCESS;
}/* pbm_mmgsdi_get_file_attributes_pbr */


/*===========================================================================
FUNCTION PBM_MMGSDI_READ_CB

DESCRIPTION
  After MMGSDI has read any SIM/CSIM/USIM record this function is called.
  This function makes sure that the data sent by GSDI is sane.

  The appropriate pbm read callback is called based on the file type.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_read_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    data_type,
  const mmgsdi_cnf_type  *data_ptr
)
{
  uint32                   client_ref;
  pbm_session_enum_type    session_type;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  client_ref = data_ptr->response_header.client_data;

  if (data_type != MMGSDI_READ_CNF)
  {
    UIM_MSG_ERR_1("Invalid response type to a GSDI read. %d",
                  data_type);
    return;
  }

  if ((client_ref >= PBM_ASYNC_BUFFERS) ||
      (pbm_buffers[client_ref].in_use == FALSE))
  {
    UIM_MSG_ERR_1("Invalid client_ref found: %d\n", client_ref);
    return;
  }

  session_type = pbm_mmgsdi_session_to_pbm_session(
                                                  data_ptr->response_header.session_id);

  if (pbm_buffers[client_ref].record.session_type != session_type)
  {
    UIM_MSG_ERR_3("Wrong callback called for file %d and session %d, %d",
                  pbm_buffers[client_ref].record.file_id,
                  pbm_buffers[client_ref].record.session_type, session_type);
    return;
  }

  // Restart initialization if required
  if (pbm_session_restart & PBM_SESSIONID_TO_SESSION_MASK(session_type))
  {
    UIM_MSG_HIGH_1("PBM restarting init of SIM %d", session_type);
    pbm_restart_fileinfo_internal(session_type);
    pbm_free_record((int) client_ref);
    return;
  }

  // Stop initialization if required
  if (!(pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(session_type)))
  {
    UIM_MSG_HIGH_1("PBM stopping init of SIM %d", session_type);
    pbm_free_record((int) client_ref);
    return;
  }

  /* GSDI data looks fine */
  switch (pbm_file_id_to_cache_type(pbm_buffers[client_ref].record.file_id))
  {
    case PBM_CACHE_TYPE_PB:
      pbm_mmgsdi_sim_read_cb(status, data_ptr);
      break;

    case PBM_CACHE_TYPE_EXT:
      pbm_mmgsdi_ext_read_cb(status, data_ptr);
      break;

    case PBM_CACHE_TYPE_USIM:
      pbm_mmgsdi_usim_read_cb(status, data_ptr);
      break;

    case PBM_CACHE_TYPE_SYNC1:
    case PBM_CACHE_TYPE_SYNC2:
      pbm_mmgsdi_sync_read_cb(status, data_ptr);
      break;

    case PBM_CACHE_TYPE_INDEX:
      pbm_mmgsdi_index_read_cb(status, data_ptr);
      break;

    default:
      UIM_MSG_ERR_1("Unknown file id %d", pbm_buffers[client_ref].record.file_id);
      pbm_free_record((int) client_ref);
      break;
  }

  return;

} /* pbm_mmgsdi_read_cb...*/


/*===========================================================================
FUNCTION PBM_MMGSDI_EXT_READ_CB

DESCRIPTION
  After GSDI has retreived the extension record from EXT1 or EXT5,
  PBM_EXT_READ_CB is called to copy the data in to a pbm_buffer
  for later processing.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_ext_read_cb(
  mmgsdi_return_enum_type        status,
  const mmgsdi_cnf_type         *data_ptr)
{
  uint32                      client_ref    = PBM_ASYNC_BUFFERS;
  byte                       *bcd_number    = NULL;
  pbm_field_id_e_type         field_id      = PBM_FIELD_NONE;
  pbm_return_type             ret_val       = PBM_ERROR;
  pbm_session_enum_type       session_type  = PBM_SESSION_DEFAULT;
  pbm_ext_cache_s_type       *cache         = NULL;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  client_ref = data_ptr->response_header.client_data;
  if((client_ref >= PBM_ASYNC_BUFFERS) ||
     (pbm_buffers[client_ref].in_use == FALSE))
  {
    UIM_MSG_ERR_1("Invalid client_ref found: 0x%x", client_ref);
    return;
  }

  session_type = pbm_mmgsdi_session_to_pbm_session(
                                    data_ptr->response_header.session_id);
  if(session_type == PBM_SESSION_MAX)
  {
    UIM_MSG_ERR_1("Invalid session_type 0x%x", session_type);
    pbm_free_record((int) client_ref);
    return;
  }

  cache = (pbm_ext_cache_s_type *)pbm_file_id_to_cache(session_type,
                                                       pbm_buffers[client_ref].record.file_id);
  if(cache == NULL)
  {
    pbm_free_record((int) client_ref);
    return;
  }

  pbm_buffers[client_ref].record.file_id = pbm_buffers[client_ref].m_file_id;
  if (status == MMGSDI_SUCCESS &&
      cache->ext_rec_length == data_ptr->read_cnf.read_data.data_len)
  {
    field_id = pbm_file_id_to_field_id(pbm_buffers[client_ref].m_file_id);
    if (field_id == PBM_FIELD_ADDITIONAL_NUMBER_EXT)
    {
      bcd_number = (byte *)&pbm_buffers[client_ref].record.data.anr.anr_data[1];
      if(data_ptr->read_cnf.read_data.data_ptr[1] != 0xFF &&
         ((bcd_number[BCD_LEN] + data_ptr->read_cnf.read_data.data_ptr[1]) > 
           sizeof(pbm_buffers[client_ref].record.data.anr.anr_data)))
      {
        /* saving truncated addtional number if EF-ANR length + extension number length
           saved in EF-EXT exceeds size of destination buffer */
        data_ptr->read_cnf.read_data.data_ptr[1] = 
             sizeof(pbm_buffers[client_ref].record.data.anr.anr_data) - bcd_number[BCD_LEN];
      }
    }
    else
    {
      bcd_number = (byte *)&pbm_buffers[client_ref].record.data.num_text.bcd_number;
      if(data_ptr->read_cnf.read_data.data_ptr[1] != 0xFF &&
         ((bcd_number[BCD_LEN] + data_ptr->read_cnf.read_data.data_ptr[1]) > 
           sizeof(pbm_buffers[client_ref].record.data.anr.anr_data)))
      {
        /* saving truncated BCD number */
        data_ptr->read_cnf.read_data.data_ptr[1] = 
              sizeof(pbm_buffers[client_ref].record.data.num_text.bcd_number) - bcd_number[BCD_LEN];
      }
    }

    (void)pbm_mmgsdi_process_ext_data(data_ptr->read_cnf.read_data.data_ptr, bcd_number);
    ret_val = PBM_SUCCESS;
  }

  /* Read data processing */
  if (pbm_buffers[client_ref].proc_func != NULL)
  {
    (*pbm_buffers[client_ref].proc_func)(ret_val,
                                         &pbm_buffers[client_ref].record);
  }
  pbm_free_record((int) client_ref);
  return;
} /* pbm_mmgsdi_ext_read_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_INDEX_READ_CB

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_index_read_cb(
  mmgsdi_return_enum_type   status,
  const mmgsdi_cnf_type    *data_ptr)
{
  uint32                      client_ref    = PBM_ASYNC_BUFFERS;
  pbm_session_enum_type       session_type  = PBM_SESSION_DEFAULT;
  PB_MASTER_USIM_CACHE       *iap_cache     = NULL;
  pbm_return_type             ret_val       = PBM_ERROR;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  client_ref = data_ptr->response_header.client_data;
  if((client_ref >= PBM_ASYNC_BUFFERS) ||
     (pbm_buffers[client_ref].in_use == FALSE))
  {
    UIM_MSG_ERR_1("Invalid client_ref found: 0x%x", client_ref);
    return;
  }

  session_type = pbm_mmgsdi_session_to_pbm_session(
                                    data_ptr->response_header.session_id);
  if(session_type == PBM_SESSION_MAX)
  {
    UIM_MSG_ERR_1("Invalid session_type 0x%x", session_type);
    pbm_free_record((int) client_ref);
    return;
  }

  iap_cache = (PB_MASTER_USIM_CACHE *)pbm_file_id_to_cache(session_type,
                                                           pbm_buffers[client_ref].record.file_id);
  if(iap_cache == NULL)
  {
    pbm_free_record((int) client_ref);
    return;
  }

  if (status == MMGSDI_SUCCESS &&
      iap_cache->iap_rec_len == data_ptr->read_cnf.read_data.data_len)
  {
    if (pbm_mmgsdi_process_index_data(pbm_buffers[client_ref].record.session_type,
                                      pbm_buffers[client_ref].record.file_id,
                                      data_ptr->read_cnf.accessed_rec_num,
                                      data_ptr->read_cnf.read_data.data_ptr,
                                      &pbm_buffers[client_ref].record.data.iap) == PBM_SUCCESS)
    {
      ret_val = PBM_SUCCESS;
    }
  }

  /* Read failed */
  if (pbm_buffers[client_ref].proc_func != NULL)
  {
    (*pbm_buffers[client_ref].proc_func) (ret_val,
                                          &pbm_buffers[client_ref].record);
  }

  pbm_free_record((int) client_ref);
  return;
} /* pbm_mmgsdi_index_read_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_SYNC_READ_CB

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_sync_read_cb(
  mmgsdi_return_enum_type        status,
  const mmgsdi_cnf_type         *data_ptr)
{
  uint32                   client_ref    = PBM_ASYNC_BUFFERS;
  pbm_return_type          ret           = PBM_SUCCESS;
  byte                    *data          = NULL;
  void                    *cache         = NULL;
  pbm_session_enum_type    session_type  = PBM_SESSION_DEFAULT;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  client_ref   = data_ptr->response_header.client_data;
  if((client_ref >= PBM_ASYNC_BUFFERS) ||
     (pbm_buffers[client_ref].in_use == FALSE))
  {
    UIM_MSG_ERR_1("Invalid client_ref found: 0x%x", client_ref);
    return;
  }

  session_type = pbm_mmgsdi_session_to_pbm_session(
                                    data_ptr->response_header.session_id);
  if(session_type == PBM_SESSION_MAX)
  {
    UIM_MSG_ERR_1("Invalid session_type 0x%x", session_type);
    pbm_free_record((int) client_ref);
    return;
  }

  cache = (void *)pbm_file_id_to_cache(session_type,
                               pbm_buffers[client_ref].record.file_id);
  if(cache == NULL)
  {
    pbm_free_record((int) client_ref);
    return;
  }

  if (status == MMGSDI_SUCCESS)
  {
    data = data_ptr->read_cnf.read_data.data_ptr;
    switch (pbm_buffers[client_ref].record.file_id)
    {
      case PBM_FILE_PSC:
      {
        PB_SYNC1_CACHE *sync1_cache = (PB_SYNC1_CACHE *)cache;

        /* As per TS 31.102 4.4.2.12.2 EF-PSC (Phone book Synchronisation Counter) 
           length should be 4 bytes */
        if(sync1_cache->record_length >= 4)
        {
          pbm_buffers[client_ref].record.data.dword = data[0] << 24 |
                                                      data[1] << 16 |
                                                      data[2] << 8  |
                                                      data[3];
        }
        else
        {
          /* returning error for non-spec complaint cards */
          ret = PBM_ERROR;
        }
      }
        break;

      case PBM_FILE_PUID:
      case PBM_FILE_CC:
      {
        PB_SYNC1_CACHE *sync1_cache = (PB_SYNC1_CACHE *)cache;

        /* As per TS 31.102 4.4.2.12.3 EF-CC and 4.4.2.12.4 EF-PUID
           lengths should be 2 bytes */
        if(sync1_cache->record_length >= 2)
        {
          pbm_buffers[client_ref].record.data.word = data[0] << 8 |
                                                     data[1];
        }
        else
        {
          /* returning error for non-spec complaint cards */
          ret = PBM_ERROR;
        }
      }
      break;
      case PBM_FILE_PBC:
      case PBM_FILE_PBC1:
      case PBM_FILE_PBC2:
      case PBM_FILE_PBC3:
      case PBM_FILE_UID:
      case PBM_FILE_UID1:
      case PBM_FILE_UID2:
      case PBM_FILE_UID3:
      {
        PB_SYNC2_CACHE *sync2_cache = (PB_SYNC2_CACHE *)cache;

        /* As per TS 31.102 4.4.2.5 EF-PBC and 4.4.2.12.1 EF-UID
           lengths should be 2 bytes */
        if(sync2_cache->record_length >= 2)
        {
          pbm_buffers[client_ref].record.data.word = data[0] << 8 |
                                                     data[1];
        }
        else
        {
          /* returning error for non-spec complaint cards */
          ret = PBM_ERROR;
        }
      }
      break;

      default:
        UIM_MSG_ERR_1("Unexpected file_id %d",
                       pbm_buffers[client_ref].record.file_id);
        ret = PBM_ERROR;
        break;
    }
  }

  if (pbm_buffers[client_ref].proc_func != NULL)
  {
    (*pbm_buffers[client_ref].proc_func)( ret,
                                          &pbm_buffers[client_ref].record);
  }

  pbm_free_record((int) client_ref);
  return;
} /* pbm_mmgsdi_sync_read_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_SIM_READ_CB

DESCRIPTION
  After GSDI has completed reading data from the SIM, PBM_READ_CB is
  called to copy the data in to a pbm_buffer for later processing.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_sim_read_cb(
  mmgsdi_return_enum_type     status,
  const mmgsdi_cnf_type      *data_ptr)
{
  int                           extension_rec_num  = 0;
  /* this file id points to the file from buffer, i.e. for which read_cb has been called */
  pbm_file_type                 file_id            = PBM_FILE_NONE;
  uint32                        client_ref         = PBM_ASYNC_BUFFERS;
  PB_CACHE                     *cache              = NULL;
  pbm_session_enum_type         session_type       = PBM_SESSION_DEFAULT;
  boolean                       sim_data_processed = FALSE;
  pbm_pb_set_type               pb_set             = PBM_USIM_PB_INVALID;
  pbm_return_type               ret_val            = PBM_ERROR;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  client_ref = data_ptr->response_header.client_data;
  if ((client_ref >= PBM_ASYNC_BUFFERS) ||
      (pbm_buffers[client_ref].in_use == FALSE))
  {
    UIM_MSG_ERR_1("Invalid client_ref found: 0x%x", client_ref);
    return;
  }
  file_id = pbm_buffers[client_ref].record.file_id;

  pb_set = pbm_file_id_to_pb_set(file_id);
  if(file_id != PBM_FILE_ADN && file_id != PBM_FILE_ADN1 && 
     file_id != PBM_FILE_ADN2 && file_id != PBM_FILE_ADN3)
  {
    pb_set = PBM_USIM_PB_ONE;
  }

  if(!pbm_validate_pb_set(pb_set))
  {
    pbm_free_record((int) client_ref);
    return;
  }
  session_type = pbm_mmgsdi_session_to_pbm_session(data_ptr->response_header.session_id);

  if(session_type == PBM_SESSION_MAX)
  {
    UIM_MSG_ERR_1("Invalid session_type 0x%x", session_type);
    pbm_free_record((int) client_ref);
    return;
  }

  cache = (PB_CACHE *) pbm_file_id_to_cache (session_type, file_id);
  if (!cache)
  {
    pbm_free_record((int) client_ref);
    UIM_MSG_ERR_2("No Cache found for session_type %d, file_id %d\n",
                  session_type, file_id);
    return;
  }

  /* If the read was successful, process the extension if needed */
  if (status == MMGSDI_SUCCESS &&
      cache->record_length[pb_set] == data_ptr->read_cnf.read_data.data_len)
  {
    if(file_id == PBM_FILE_GAS || file_id == PBM_FILE_AAS)
    {
      memscpy(pbm_buffers[client_ref].record.data.num_text.sim_text,
              sizeof(pbm_buffers[client_ref].record.data.num_text.sim_text),
              data_ptr->read_cnf.read_data.data_ptr,cache->text_length[pb_set]);
      /* Processing was complete. Life is good. */
      ret_val = PBM_SUCCESS;
    } 
    /* MDN in RUIM & CSIM case is read as PBM_FILE_MSISDN and it need a 
       special processing , since the rec structure is different to MSISDN in GW */
    else if(file_id == PBM_FILE_MSISDN &&
             (USE_CSIM_LOCAL_SUPPORT(session_type) || USE_CDMA_SUPPORT(session_type)))
    {
      if( pbm_mmgsdi_process_mdn_data(session_type,
                                      file_id,
                                      pbm_buffers[client_ref].record.index,
                                      data_ptr->read_cnf.read_data.data_ptr,
                                      &pbm_buffers[client_ref].record.data.num_text))
      {
        sim_data_processed = TRUE;
      }
    }
    else if ( pbm_mmgsdi_process_sim_data(session_type,
                                          file_id,
                                          pbm_buffers[client_ref].record.index,
                                          data_ptr->read_cnf.read_data.data_ptr,
                                          &pbm_buffers[client_ref].record.data.num_text,
                                          &extension_rec_num))
    {
      sim_data_processed = TRUE;
    }
    if(sim_data_processed == TRUE)
    {
      /* Processing was complete. Life is good. */
      ret_val = PBM_SUCCESS;
    }
    else
    {
      if (extension_rec_num && cache->extension_cache &&
          (cache->extension_cache->file_id != PBM_FILE_NONE))
      {
        pbm_buffers[client_ref].m_file_id      = file_id;
        pbm_buffers[client_ref].record.file_id =
                   cache->extension_cache->file_id;
        pbm_buffers[client_ref].gsdi_resp_type = PBM_MMGSDI_READ_CB;
        if (pbm_send_read_request(pbm_buffers[client_ref].record.session_type,
                                  cache->extension_cache->file_id,
                                  extension_rec_num, (int) client_ref,
                                  pbm_mmgsdi_async_cb))
        {
          return;
        }
        UIM_MSG_ERR_0("Could not issue read request");
      }
      else
      {
        UIM_MSG_ERR_2("PBM could not find a valid extension record for %d from cache 0x%x",
                      file_id, cache);
      }
    }
  }
  else
  {
    UIM_MSG_HIGH_3("status 0x%x session_type 0x%x file_id 0x%x",
                   status, session_type, file_id);
  }

  if (pbm_buffers[client_ref].proc_func != NULL)
  {
    (*pbm_buffers[client_ref].proc_func) ( ret_val,
                                           &pbm_buffers[client_ref].record);
  }
  pbm_free_record((int) client_ref);
}/* pbm_mmgsdi_sim_read_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_USIM_READ_CB

DESCRIPTION
  After GSDI has completed reading data from the USIM, pbm_mmgsdi_usim_read_cb is
  called to copy the data in. At this point we process the data from USIM into
  a generic SIM record type.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_usim_read_cb(
  mmgsdi_return_enum_type       status,
  const mmgsdi_cnf_type        *data_ptr)
{
  pbm_return_type                  ret               = PBM_SUCCESS;
  int                              extension_rec_num = 0;
  pbm_ext_cache_s_type            *ext_cache         = NULL;
  uint32                           client_ref        = PBM_ASYNC_BUFFERS;
  PB_USIM_CACHE                   *usim_cache        = NULL;
  pbm_file_type                    file_id           = PBM_FILE_NONE;
  pbm_session_enum_type            session_type      = PBM_SESSION_DEFAULT;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  session_type = pbm_mmgsdi_session_to_pbm_session(data_ptr->response_header.session_id);
  if(session_type == PBM_SESSION_MAX)
  {
    UIM_MSG_ERR_1("Invalid session_type 0x%x", session_type);
    pbm_free_record((int) client_ref);
    return;
  }

  client_ref = data_ptr->response_header.client_data;
  if ((client_ref >= PBM_ASYNC_BUFFERS) ||
      (pbm_buffers[client_ref].in_use == FALSE))
  {
    UIM_MSG_ERR_1("Invalid client_ref found: 0x%x", client_ref);
    return;
  }
  file_id = pbm_buffers[client_ref].record.file_id;

  usim_cache = (PB_USIM_CACHE *) pbm_file_id_to_cache(session_type, file_id);
  if (!usim_cache)
  {
    pbm_free_record((int) client_ref);
    UIM_MSG_ERR_2("No cache found for session_type %d, file_id %d\n",
                  session_type, file_id);
    return;
 }

  do
  {
    if (status != MMGSDI_SUCCESS ||
        (usim_cache->record_length != data_ptr->read_cnf.read_data.data_len))
    {
      UIM_MSG_ERR_2("PBM Error %d from GSDI for file %d",
                     status, file_id);
      ret = PBM_ERROR;
      break;
    }

      switch (file_id)
      {
         case PBM_FILE_EMAIL:
         case PBM_FILE_EMAIL1:
         case PBM_FILE_SNE:
         case PBM_FILE_SNE1:
         case PBM_FILE_EMAIL2:
         case PBM_FILE_EMAIL3:
         case PBM_FILE_SNE2:
         case PBM_FILE_SNE3:
            ret = pbm_mmgsdi_process_text_data(session_type,
                                               file_id,
                                               data_ptr->read_cnf.accessed_rec_num,
                                               data_ptr->read_cnf.read_data.data_ptr,
                                               &pbm_buffers[client_ref].record.data.text);
            break;

         case PBM_FILE_GRP:
         case PBM_FILE_GRP1:
         case PBM_FILE_GRP2:
         case PBM_FILE_GRP3:
            ret = pbm_mmgsdi_process_grp_data(session_type,
                                              file_id,
                                              data_ptr->read_cnf.accessed_rec_num,
                                              data_ptr->read_cnf.read_data.data_ptr,
                                              &pbm_buffers[client_ref].record.data.grp);
            break;

         case PBM_FILE_ANR:
         case PBM_FILE_ANRA:
         case PBM_FILE_ANRB:
         case PBM_FILE_ANRC:
         case PBM_FILE_ANR1:
         case PBM_FILE_ANRA1:
         case PBM_FILE_ANRB1:
         case PBM_FILE_ANRC1:
         case PBM_FILE_ANR2:
         case PBM_FILE_ANRA2:
         case PBM_FILE_ANRB2:
         case PBM_FILE_ANRC2:
         case PBM_FILE_ANR3:
         case PBM_FILE_ANRA3:
         case PBM_FILE_ANRB3:
         case PBM_FILE_ANRC3:
            ret = pbm_mmgsdi_process_anr_data(session_type,
                                              file_id,
                                              data_ptr->read_cnf.accessed_rec_num,
                                              data_ptr->read_cnf.read_data.data_ptr,
                                              &pbm_buffers[client_ref].record.data.anr,
                                              &extension_rec_num);
            break;

         default:
            ret = PBM_ERROR;
            break;
      }

      if (ret != PBM_SUCCESS)
      {
         UIM_MSG_ERR_2("Error %d in processing data from file %d", ret, file_id);
         break;
      }

      /* If there are no extension records, we are done here */
      if (!extension_rec_num)
         break;

      if (pbm_file_id_to_field_id(file_id) != PBM_FIELD_ADDITIONAL_NUMBER_EXT)
      {
         UIM_MSG_ERR_2("File id %d is not supposed to have extension record %d",
                     file_id, extension_rec_num);
         ret = PBM_ERROR;
         break;
      }

      ext_cache = &ext1_cache[session_type];
      if (ext_cache->file_id == PBM_FILE_NONE)
      {
         UIM_MSG_ERR_1("PBM could not find a valid extension record for %d",
                     file_id);
         ret = PBM_ERROR;
         break;
      }

      pbm_buffers[client_ref].m_file_id = file_id;
      pbm_buffers[client_ref].record.file_id = ext_cache->file_id;
      pbm_buffers[client_ref].gsdi_resp_type = PBM_MMGSDI_READ_CB;
      if (!pbm_send_read_request(pbm_buffers[client_ref].record.session_type,
                                 ext_cache->file_id,
                                 extension_rec_num, (int) client_ref,
                                 pbm_mmgsdi_async_cb))
      {
         UIM_MSG_ERR_2("Could not issue read request for %d index %d",
                     ext_cache->file_id, extension_rec_num);
         ret = PBM_ERROR;
         break;
      }

      return; /* We have sent an extension request, buffer is not freed yet */

   } while (FALSE);

   if (pbm_buffers[client_ref].proc_func != NULL)
   {
      (*pbm_buffers[client_ref].proc_func)( ret,
                                            &pbm_buffers[client_ref].record);
   }

   pbm_free_record((int) client_ref);
   return;
}/* pbm_mmgsdi_usim_read_cb */


/*===========================================================================
FUNCTION PBM_SEND_READ_REQUEST

DESCRIPTION
  Populate the msg struct for GSDI including the callback function the
  call gsdi to process the read request.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_send_read_request(
  pbm_session_enum_type session_type,
  pbm_file_type         file_id,
  int                   rec_num,
  int                   pbm_buffer_index,
  mmgsdi_callback_type  callback
)
{
  int                                len = 0;
  boolean                            ret_val = FALSE;
  PB_CACHE                          *cache;
  pbm_ext_cache_s_type              *ext_cache;
  mmgsdi_file_structure_enum_type    sim_filetype = MMGSDI_MAX_FILE_STRUCTURE_ENUM;
  PB_USIM_CACHE                     *usim_cache;
  PB_MASTER_USIM_CACHE              *m_usim_cache;
  PB_SYNC1_CACHE                    *sync1_cache;
  PB_SYNC2_CACHE                    *sync2_cache;

  switch (pbm_file_id_to_cache_type(file_id))
  {
    case PBM_CACHE_TYPE_PB:
      if ((cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
      {
        pbm_pb_set_type pb_set = pbm_file_id_to_pb_set(file_id);
        if(pb_set != PBM_USIM_PB_INVALID)
        {
          len = cache->record_length[pb_set];
        }
        else
        {
          len = cache->record_length[0];
        }

		sim_filetype = cache->file_type;
      }
      break;

    case PBM_CACHE_TYPE_USIM:
      if ((usim_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
      {
        len = usim_cache->record_length;
        sim_filetype = usim_cache->file_type;
      }
      break;

    case PBM_CACHE_TYPE_INDEX:
      if ((m_usim_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
      {
        len = m_usim_cache->iap_rec_len;
        sim_filetype = m_usim_cache->file_type;
      }
      break;

    case PBM_CACHE_TYPE_SYNC1:
      if ((sync1_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
      {
        len = sync1_cache->record_length;
        sim_filetype = sync1_cache->file_type;
      }
      break;

    case PBM_CACHE_TYPE_SYNC2:
      if ((sync2_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
      {
        len = sync2_cache->record_length;
        sim_filetype = sync2_cache->file_type;
      }
      break;

    case PBM_CACHE_TYPE_EXT:
      if ((ext_cache = pbm_file_id_to_cache(session_type, file_id)) != NULL)
      {
        len = ext_cache->ext_rec_length;
        sim_filetype = ext_cache->file_type;
      }
      break;

    default:
      UIM_MSG_ERR_1("Unknown file id %d", file_id);
      break;
  }

  if (len == 0)
  {
    UIM_MSG_ERR_2("Could not issue read request for file_id 0x%x on Session 0x%x",
                  file_id, session_type);
  } else
  {
    pbm_return_type pbm_retval;

    /* Now issue the asynchronous command to GSDI. */
    pbm_retval = pbm_mmgsdi_sim_read(session_type,            /* Session */
                                     file_id,      /* file id */
                                     sim_filetype,      /* filetype */
                                     rec_num,           /* Record number */
                                     len,               /* Required data length */
                                     (uint32) pbm_buffer_index,  /* Client Ref. */
                                     callback);         /* Asynchronous callback. */
    if (pbm_retval != PBM_SUCCESS)
    {
      UIM_MSG_ERR_3("PBM pbm_mmgsdi_sim_read returned an error 0x%x for file_id 0x%x on session 0x%x",
                     pbm_retval, file_id, session_type);
    } else
    {
      ret_val = TRUE;
    }
  }

  return ret_val;
}/* pbm_send_read_request */


/*===========================================================================
FUNCTION PBM_FREE_EXTENSION_RECORD

DESCRIPTION
  When we will overwrite a record, this routine will find the
  extension record associated with that record and free that in
  our cache structures. It returns the record it freed, or 0.

PARAMETERS:

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_free_extension_record(
  pbm_ext_cache_s_type *ext_cache, 
  int                   index,
  pbm_file_type         file_id
)
{
  int                        i;
  int                        ext_num_of_rec;
  pbm_ext_rec_in_use_s_type *ext_rec_in_use;

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
      /* erase */
      ext_rec_in_use[i].uim_device = PBM_FILE_NONE;
      ext_rec_in_use[i].index = 0;

      return i;
    }
  }
  /* if we got here, we didn't find it */
  return 0;
}/* pbm_free_extension_record */


/*===========================================================================
FUNCTION pbm_mmgsdi_process_text_data

DESCRIPTION
  Parses the data returned by GSDI and fills up the text record given as
  input.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_process_text_data( 
  pbm_session_enum_type    session_type,
  pbm_file_type            file_id,
  int                      index,
  byte                    *data_buffer,
  pbm_text_record_s_type  *return_data)
{
  uint16            text_length;
  PB_USIM_CACHE    *usim_cache = (PB_USIM_CACHE *)pbm_file_id_to_cache(session_type,
                                                                     file_id);
  boolean           convert = TRUE;

  pbm_pb_set_type pb_set = pbm_file_id_to_pb_set(file_id);
  if ( ! pbm_validate_pb_set(pb_set) )
  {
    return PBM_ERROR; 
  }

  PBM_CHECK_PTR3_RET(data_buffer, return_data, usim_cache, PBM_ERROR);

  memset(return_data->sim_text, 0xFF, sizeof(return_data->sim_text));
  return_data->adn_record_id = 0;

  text_length = (uint16) usim_cache->record_length;
  if ((text_length > 2) && (usim_cache->mapping_type == PBM_MAPPING_TYPE_2))
  {
    /* This is OK if the record is empty, otherwise not OK */
    if (data_buffer[0] == 0xff)
    {
      convert = FALSE;
	}
    text_length -= 2;
  }

  if (convert)
  {
    memscpy(return_data->sim_text,sizeof(return_data->sim_text),data_buffer,text_length);  
  }
  return PBM_SUCCESS;
}/* pbm_mmgsdi_process_text_data */


/*===========================================================================
FUNCTION pbm_mmgsdi_process_anr_data

DESCRIPTION
  Parses the data returned by GSDI and fills up the anr record given as
  input.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_process_anr_data(
  pbm_session_enum_type     session_type,
  pbm_file_type             file_id,
  int                       index,
  byte                     *data_buffer,
  pbm_anr_record_s_type    *return_data,
  int                      *extension_index)
{
  pbm_adn_record_id_type       adn_rec_id  = 0;
  PB_USIM_CACHE               *usim_cache  = NULL;
  pbm_ext_cache_s_type        *ext_cache   = NULL;
  pbm_pb_set_type              pb_set      = PBM_USIM_PB_INVALID;

  usim_cache = (PB_USIM_CACHE *)pbm_file_id_to_cache(session_type, file_id);
  pb_set     = pbm_file_id_to_pb_set(file_id);
  if (!pbm_validate_pb_set(pb_set) )
  {
    return PBM_ERROR;
  }

  PBM_CHECK_PTR4_RET(data_buffer, return_data, usim_cache, extension_index, PBM_ERROR);

  memset(return_data, 0, sizeof(pbm_anr_record_s_type));

  /* Check if the number is not blank and number length is configured
     as per spec TS 31.102 4.4.2.9 EF-ANR (Additional Number) or not */
  switch(usim_cache->mapping_type)
  {
    case PBM_MAPPING_TYPE_1:
    {
      adn_rec_id = pbm_location_to_adn_rec_id(session_type, pb_set, (uint8)index);
      if(usim_cache->record_length != 15)
      {
        return PBM_ERROR;
      }
    }
    break;

    case PBM_MAPPING_TYPE_2:
    {
      if(usim_cache->record_length != 17)
      {
        return PBM_ERROR;
      }
    }
    break;

	case PBM_MAPPING_TYPE_3:
    default:
      return PBM_ERROR;
  }

  if(data_buffer[1] != 0xFF)
  {
    /* As per spec TS 31.102 4.4.2.9 EF-ANR value of Length of BCD number/SSC
       contents  should be not more than 11 bytes */
    if(data_buffer[1] > (PBM_SIM_DIALING_NUMBER_LEN + 1))
    {
      data_buffer[1] = PBM_SIM_DIALING_NUMBER_LEN + 1;
    }

    /* one byte for AAS Id,
       one byte for BCD len,
       one byte for TON/NPI,
       ten bytes for BCD number */
    memscpy(&return_data->anr_data, sizeof(return_data->anr_data),
            data_buffer, 3 + PBM_SIM_DIALING_NUMBER_LEN);
  }

  return_data->adn_record_id = adn_rec_id;

  if (data_buffer[14] == 0xFF)
  {
    *extension_index = 0;
  } 
  else
  {
    ext_cache = &ext1_cache[session_type];
    if (ext_cache == NULL || data_buffer[14] < 1 ||
        data_buffer[14] > (byte)ext_cache->num_of_rec)
    {
      UIM_MSG_ERR_3("USIM record index %d file %d wants extension record %d",
                     index, file_id, (int) data_buffer[14]);
    } 
    else
    {
      *extension_index = (int)data_buffer[14];
      ext_cache->in_use_array[*extension_index].uim_device =
      usim_cache->file_id;
      ext_cache->in_use_array[*extension_index].index = (uint8)index;
    }
  }

  return PBM_SUCCESS;
}/* pbm_mmgsdi_process_anr_data */


/*===========================================================================
FUNCTION pbm_mmgsdi_process_grp_data

DESCRIPTION
  Parses the data returned by GSDI and fills up the text record given as
  input.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_process_grp_data( 
  pbm_session_enum_type  session_type,
  pbm_file_type          file_id,
  int                    index,
  byte                  *data_buffer,
  pbm_grp_record_s_type *return_data)
{
  int            i;
  PB_USIM_CACHE *usim_cache = (PB_USIM_CACHE *)pbm_file_id_to_cache(session_type,
                                                                     file_id);
  PBM_CHECK_PTR3_RET(data_buffer, return_data, usim_cache, PBM_ERROR);

  memset(return_data, 0, sizeof(pbm_grp_record_s_type));

  for (i = 0; i < MIN(usim_cache->record_length,PBM_MAX_NUM_GROUPS); i++)
  {
    /* Coding:TS 31.1.02 4.4.2.6 EFGRP (Grouping file)
	 * - '00' – no group indicated;
	 *   'XX' – record number in EFGAS containing the alpha string naming 
     *          the group of which the phone book entry is a member.
	 */
    if (data_buffer[i] != 0xFF && 
        data_buffer[i] < grpname_cache[session_type].num_of_records)
    {
      return_data->group_id[i] = (uint16) data_buffer[i];
    }
  }

  return PBM_SUCCESS;
}/* pbm_mmgsdi_process_grp_data */


/*===========================================================================
FUNCTION pbm_mmgsdi_process_index_data

DESCRIPTION
  Parses the data returned by GSDI and fills up the text record given as
  input.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_mmgsdi_process_index_data(
  pbm_session_enum_type  session_type,
  pbm_file_type          file_id,
  int                    index,
  byte                  *data_buffer,
  pbm_iap_record_s_type *return_data)
{
  int                   i;
  PB_MASTER_USIM_CACHE *m_usim_cache = (PB_MASTER_USIM_CACHE *)
                                        pbm_file_id_to_cache(session_type, file_id);

  PBM_CHECK_PTR3_RET(data_buffer, return_data, m_usim_cache, PBM_ERROR);

  memset(return_data, 0, sizeof(pbm_iap_record_s_type));

  for (i = 0; i < MIN(m_usim_cache->iap_rec_len,PBM_MAX_NUM_TYPE2_FILES); i++)
    if (data_buffer[i] != 0xFF)
      return_data->index[i] = data_buffer[i];

  return PBM_SUCCESS;
}/* pbm_mmgsdi_process_index_data */


/*===========================================================================
FUNCTION PBM_MMGSDI_PROCESS_SIM_DATA

DESCRIPTION
  Parse the data buffer read by pbm_gsdi_sim_read then populate the record
  stucture passed in by the caller.  The return value from this function
  is whether the buffer contained a complete record (TRUE), or if an
  extension record needs to be read (FALSE).  If the function returns
  FALSE, it also populates extension_index to contain the record to be
  read.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_mmgsdi_process_sim_data(
  pbm_session_enum_type         session_type,
  pbm_file_type                 file_id,
  int                           index,
  byte                         *local_pbm_data_buffer,
  pbm_num_text_record_s_type   *return_data,
  int                          *extension_index)
{
  pbm_fixed_sim_rec_s_type        *fixed_sim_record = NULL;
  pbm_pb_set_type                  pb_set           = PBM_MAX_NUM_PB_SETS;
  int                              cache_text_len   = 0;
  int                              cache_rec_len    = 0;
  time_julian_type                 julian_time;
  PB_CACHE                        *cache            = NULL;

  memset(&julian_time, 0, sizeof(julian_time));
  cache  = (PB_CACHE *)pbm_file_id_to_cache(session_type, file_id);
  PBM_CHECK_PTR4_RET(return_data, extension_index, local_pbm_data_buffer, cache, FALSE);

  /* Initialize return record to 0's */
  memset(return_data, 0, sizeof(pbm_num_text_record_s_type));
  memset(return_data->sim_text, 0xFF, sizeof(return_data->sim_text));

  /* Store the index as this index. */
  return_data->index = index;

  pb_set = pbm_file_id_to_pb_set(file_id);
  if(pb_set != PBM_USIM_PB_INVALID)
  {
    cache_text_len = cache->text_length[pb_set];
    cache_rec_len  = cache->record_length[pb_set];
  }
  else
  {
    cache_text_len = cache->text_length[0];
    cache_rec_len  = cache->record_length[0];
  }

  /* Point to the SIM record starting at the number. */
  fixed_sim_record = (pbm_fixed_sim_rec_s_type *) (local_pbm_data_buffer +
                                                   cache_text_len);

  if(local_pbm_data_buffer[0] != 0xFF)
  {
    memscpy(return_data->sim_text,sizeof(return_data->sim_text),
            local_pbm_data_buffer,cache_text_len);
  }

  /* If the number is not blank and number length is configured as per spec 
     TS 31.102 4.4.2.3 EF-ADN (Abbreviated dialling numbers),EF-FDN,etc..*/
  if (fixed_sim_record->bcd_len != 0xFF && fixed_sim_record->number[0] != 0xFF &&
      (cache_rec_len - cache_text_len) >= (2 + PBM_SIM_DIALING_NUMBER_LEN))
  {
    /* one byte for BCD len,
       one byte for Type,
       ten bytes for BCD number */
    memscpy(&return_data->bcd_number, sizeof(return_data->bcd_number),
             fixed_sim_record, 2 + PBM_SIM_DIALING_NUMBER_LEN);

    /* as per spec TS 31.102 4.4.2.9 EF-ANR value of Length of BCD number/SSC
       contents  should be not more than 11 bytes */
    if(return_data->bcd_number[BCD_LEN] > (PBM_SIM_DIALING_NUMBER_LEN + 1))
    {
      return_data->bcd_number[BCD_LEN] = PBM_SIM_DIALING_NUMBER_LEN + 1;
    }
  }

  /* These guys have the timestamp. */
  if ((file_id == PBM_FILE_ICI) || (file_id == PBM_FILE_OCI))
  {
    /* Convert timestamp */
    julian_time.year   = pbm_bcd_to_bin(fixed_sim_record->date_time[0]);

    if (julian_time.year >= 80)  /* 80 becomes 1980, 79 becomes 2079 */
      julian_time.year += 1900;
    else
      julian_time.year += 2000;

    julian_time.month  = pbm_bcd_to_bin(fixed_sim_record->date_time[1]);
    julian_time.day    = pbm_bcd_to_bin(fixed_sim_record->date_time[2]);
    julian_time.hour   = pbm_bcd_to_bin(fixed_sim_record->date_time[3]);
    julian_time.minute = pbm_bcd_to_bin(fixed_sim_record->date_time[4]);
    julian_time.second = pbm_bcd_to_bin(fixed_sim_record->date_time[5]);
    return_data->time  = time_jul_to_secs(&julian_time);

    return_data->duration = fixed_sim_record->duration[0] << 16 |
                            fixed_sim_record->duration[1] << 8  |
                            fixed_sim_record->duration[2];

    if ( return_data->duration == 0x00FFFFFF )
    {
      return_data->duration = 0;
    }
  }

  /* Figure out if there is an extension record.  If not, we are done
   * return TRUE.  If there is, say that there is and return FALSE that
   * the processing is not complete.
   */
  if (fixed_sim_record->ext_id != 0xFF)
  {
    if (!cache->extension_cache)
    {
      UIM_MSG_ERR_2("SIM ERROR, record %d no extension %d",
                     index,  (int) fixed_sim_record->ext_id);
    } 
    else if (((int) fixed_sim_record->ext_id < 1) ||
             ((int) fixed_sim_record->ext_id > cache->extension_cache->num_of_rec))
    {
      UIM_MSG_ERR_3("SIM ERROR record %d wants extension record %d of %d",
                     index, (int) fixed_sim_record->ext_id,
                     cache->extension_cache->num_of_rec);
    }
    else
    {
      *extension_index = (int) fixed_sim_record->ext_id;
      cache->extension_cache->in_use_array[*extension_index].uim_device =
                      pbm_pb_record_index_to_filename(cache, return_data->index);
      cache->extension_cache->in_use_array[*extension_index].index =
                     (byte)pbm_pb_record_index_to_fileloc(cache, return_data->index);
      return FALSE;     /* Processing is not complete. */
    }
  }

  return TRUE;          /* Processing of this buffer is correct. */
}/* pbm_mmgsdi_process_sim_data */


/*===========================================================================
FUNCTION PBM_MMGSDI_PROCESS_EXT_DATA

DESCRIPTION
  If the phone number is longer than ADN_MAX_PHONE_NUMBER, the
  remainder of the phone number was stored in the extension record
  in the EXT file.  After the extension data is read from GSDI, this
  function is called with the return_data which was filled in by
  pbm_mmgsdi_process_sim_data and this finishes where that function
  left off.

DEPENDENCIES
  return_data should be filled in from the main gsdi record first.

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_process_ext_data(
  byte         *sim_data,
  byte         *bcd_data)
{
  if(sim_data == NULL || bcd_data == NULL)
  {
    return;
  }

  if (sim_data[0] == EXT_REC_TYPE_ADDITIONAL_DATA &&
      sim_data[1] <= PBM_SIM_DIALING_NUMBER_LEN)
  {
    bcd_data[BCD_LEN] += sim_data[1];
    memscpy(&bcd_data[12],sim_data[1],&sim_data[2],sim_data[1]);
  }
  else
  {
    UIM_MSG_ERR_2("error in Extension data : 0x%x 0x%x",
                   sim_data[0],
                   sim_data[1]);
  }
  return;
}/* pbm_mmgsdi_process_ext_data */


/*===========================================================================
FUNCTION PBM_MMGSDI_FILE_INFO_CB

DESCRIPTION
  This function is called by GSDI when it has the results of our last
  get_file_attributes request.  It puts a command into PBM GSDI response qeuue
  and the actual handling is done in pbm_fileinfo_cb.

PARAMETERS
   sim_data [in] The GSDI results.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_mmgsdi_fileinfo_cb (
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    data_type,
  const mmgsdi_cnf_type  *data_ptr)
{
  mmgsdi_client_data_type           client_ref;
  mmgsdi_file_structure_enum_type   file_type;
  int32                             num_protection_pin;
  uint32                            pin_data_size;
  pbm_mmgsdi_resp_s_type           *mmgsdi_resp_ptr;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  client_ref = data_ptr->response_header.client_data;
  if (client_ref >= ARR_SIZE(pbm_fileinfo_requests))
  {
    UIM_MSG_ERR_1("Invalid client_ref found: %d\n", client_ref);
    return;
  }

  mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
  PBM_CHECK_PTR_RET(mmgsdi_resp_ptr, VOID);

  mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_FILE_INFO_CB;

  mmgsdi_resp_ptr->data_type = data_type;
  mmgsdi_resp_ptr->status = status;
  memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));
  file_type = mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_type;
  /* deep copy the protection_pin_ptr since they are to be used in pbm_fileinfo_cb */
  switch (file_type)
  {
	 case MMGSDI_LINEAR_FIXED_FILE:
	    num_protection_pin = data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.write.num_protection_pin;
		if(num_protection_pin > 0)
		{
		   pin_data_size = (num_protection_pin) * (sizeof(mmgsdi_pin_enum_type));
		   PBM_MEM_ALLOC(mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.write.protection_pin_ptr,
			  pin_data_size);
           PBM_CHECK_PTR_RET(mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.write.protection_pin_ptr,VOID);
           memscpy((void *)mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.write.protection_pin_ptr, pin_data_size,
	              (void *)data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.write.protection_pin_ptr,
	               pin_data_size); //deep copy
		}
	    break;

	 case MMGSDI_CYCLIC_FILE:
	    num_protection_pin = data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.write.num_protection_pin;
		if(num_protection_pin > 0)
		{
		   pin_data_size = (num_protection_pin) * (sizeof(mmgsdi_pin_enum_type));
		   PBM_MEM_ALLOC(mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.write.protection_pin_ptr,
			  pin_data_size);
                   PBM_CHECK_PTR_RET(mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.write.protection_pin_ptr,VOID);
           memscpy((void *)mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.write.protection_pin_ptr, pin_data_size,
	              (void *)data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.write.protection_pin_ptr, pin_data_size); //deep copy
		}
	    break;

	 case MMGSDI_TRANSPARENT_FILE :
		 num_protection_pin = data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.write.num_protection_pin;
		if(num_protection_pin > 0)
		{
		   pin_data_size = (num_protection_pin) * (sizeof(mmgsdi_pin_enum_type));
		   PBM_MEM_ALLOC(mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.write.protection_pin_ptr,
			  pin_data_size);
                   PBM_CHECK_PTR_RET(mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.write.protection_pin_ptr,VOID);
           memscpy((void *)mmgsdi_resp_ptr->data.get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.write.protection_pin_ptr, pin_data_size,
	              (void *)data_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security.write.protection_pin_ptr, pin_data_size); //deep copy
		}
		break;


	 default:
	    break;
  }

  pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
}/* pbm_mmgsdi_fileinfo_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_READ_PBR_ATTR_CB

DESCRIPTION
  This function is called by GSDI with the interpreted results of the PBR table.

PARAMETERS
   sim_data [in] The GSDI results.

SIDE EFFECTS
===========================================================================*/

void pbm_mmgsdi_read_pbr_attr_cb (
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type	   data_type,
  const mmgsdi_cnf_type	  *data_ptr)
{
  pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
  PBM_CHECK_PTR_RET(mmgsdi_resp_ptr, VOID);

  mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_READ_PBR_ATTR_CB;

  mmgsdi_resp_ptr->data_type = data_type;
  mmgsdi_resp_ptr->status = status;

  memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));

  pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
}/* pbm_mmgsdi_read_pbr_attr_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_READ_PBR_REC_CB

DESCRIPTION
  This function is called by GSDI with the interpreted results of the PBR table.

PARAMETERS
   sim_data [in] The GSDI results.

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_read_pbr_rec_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    data_type,
  const mmgsdi_cnf_type  *data_ptr
)
{
  pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;
  uint16                  read_cnf_data_len = 0;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
  if (!mmgsdi_resp_ptr)
  {
    UIM_MSG_ERR_0("Unable to allocate PBM command");
    (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
    return;
  }

  mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_READ_PBR_REC_CB;

  mmgsdi_resp_ptr->status = status;
  mmgsdi_resp_ptr->data_type = data_type;
  memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));

  if (data_ptr->read_cnf.read_data.data_len > 0)
  {
    read_cnf_data_len = data_ptr->read_cnf.read_data.data_len;
    PBM_MEM_ALLOC(mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr,
                  data_ptr->read_cnf.read_data.data_len);
    PBM_CHECK_PTR_RET(mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr,VOID);
    memscpy((void *)mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr, data_ptr->read_cnf.read_data.data_len,
            (void *)data_ptr->read_cnf.read_data.data_ptr, read_cnf_data_len); //deep copy
  } else
  {
    mmgsdi_resp_ptr->data.read_cnf.read_data.data_ptr = NULL;
  }

  pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
} /* pbm_mmgsdi_read_pbr_rec_cb */


/*===========================================================================
FUNCTION pbm_local_phonebook_check_cb

DESCRIPTION
  Checks the local phonebook support available on card or not

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_local_phonebook_check_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    data_type,
  const mmgsdi_cnf_type  *data_ptr
)
{
  mmgsdi_session_id_type mmgsdi_session_id;
  pbm_session_enum_type  pbm_session_id;

  UIM_MSG_HIGH_3("Local phonebook resp from MGMSDI: status 0x%x srv_available 0x%x data_type 0x%x",
                 status,
                 data_ptr->srv_available_cnf.srv_available,
                 data_type);

  if ( MMGSDI_SRV_AVAILABLE_CNF != data_type )
  {
    return;
  }

  mmgsdi_session_id = data_ptr->srv_available_cnf.response_header.session_id;
  pbm_session_id = pbm_mmgsdi_session_to_pbm_session(mmgsdi_session_id);

  /* Check if the 1st bit in UST(denoted by data_ptr->srv_available_cnf.srv_available) is set*/
  if (!data_ptr->srv_available_cnf.srv_available)
  {
    /* local phonebook not supported */
    pbm_session_info[pbm_session_id].in_use_adn_pb = PBM_PB_LOCAL_PBR_NOT_PRESENT;
  }

  /* even 1st bit in UST is not we would still required to read like FDN,SDN etc ..
   * prepare cache for local phonebook available 
   */
  pbm_init_fileinfo_internal(pbm_session_id);

  return;
}/* pbm_local_phonebook_check_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_SERVICE_AVAILABLE_CB

DESCRIPTION
  This function is called by MMGSDI if service requested is available on card

PARAMETERS

SIDE EFFECTS

===========================================================================*/
void pbm_mmgsdi_service_available_cb (
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    data_type,
  const mmgsdi_cnf_type  *data_ptr)
{
  pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
  PBM_CHECK_PTR_RET(mmgsdi_resp_ptr, VOID);

  mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_SERVICE_INFO_CB;

  mmgsdi_resp_ptr->data_type = data_type;
  mmgsdi_resp_ptr->status = status;

  memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));

  pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
}/* pbm_mmgsdi_service_available_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_LOCAL_PB_SRV_CB

DESCRIPTION
  This function is called by MMGSDI to check if service requested is available on card

PARAMETERS

SIDE EFFECTS

===========================================================================*/
void pbm_mmgsdi_local_pb_srv_cb (
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    data_type,
  const mmgsdi_cnf_type  *data_ptr)
{
  pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;
 
  PBM_CHECK_PTR_RET(data_ptr, VOID);

  mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
  PBM_CHECK_PTR_RET(mmgsdi_resp_ptr, VOID);

  mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_LOCAL_PB_SRV_CB;

  mmgsdi_resp_ptr->data_type = data_type;
  mmgsdi_resp_ptr->status = status;

  memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));

  pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
}/* pbm_mmgsdi_local_pb_srv_cb */

