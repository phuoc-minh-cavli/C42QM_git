/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                P B M _ C O M M O N _ L I B.C

GENERAL DESCRIPTION
  This file contains the APIs exported by PBM service which is executed as a
  library within the PBM client's task.

  Copyright (c) 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbm_common_lib.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/16   nr      ECC optimizations for IoE devices
05/25/16   sp      F3 improvements
03/10/16   nr      Logging improvements
31/01/16   nr      Intial Version
===========================================================================*/
#include "customer.h"
#include "uim_msg.h"
#include "pbm.h"
#include "pbmlib.h"
#include "pbmutils.h"
#include "pbmcache.h"
#include "pbmuim.h"
#include "pbmgsdi.h"
#include "pbmtask_i.h"
#include "stdlib.h"
#include "stringl/stringl.h"

/*===========================================================================
                         DEFINITIONS
===========================================================================*/

/*===========================================================================
    Declare static globals for pbm_enum_rec_init() and pbm_enum_next_rec()
===========================================================================*/

#define PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE 0
#define PBM_ENUM_REC_INIT_HANDLE_IN_USE     1

static int g_search_enumerator_ext[PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT] = {0};
static pbm_record_id_type *g_search_results_ext[PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT] = {NULL};
static int g_search_length_ext[PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT] = {0};
static int g_pbm_session_rec_init_handles[PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT] = {0};

/*===========================================================================*
 *              PHONE BOOK MANAGER RECORD BASED API                          *
 *                                                                           *
 * These functions supply the API for the record based requests.             *
 *===========================================================================*/

/*===========================================================================
FUNCTION PBM_SESSION_CALCULATE_FIELDS_SIZE_FROM_ID

DESCRIPTION
  Calculates how much memory is needed for a copy of the fields structure
  based on the record ID.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_session_calculate_fields_size_from_id(
  pbm_record_id_type        rec_id
)
{
  PB_CACHE                     *cache                   = NULL;
  int                           size                    = 0;
  int                           num_fields_uncompressed = 0;
  uint16                        location                = 0;
  pbm_return_type               ret                     = PBM_ERROR;
  pbm_phonebook_type            pb_id                   = {PBM_GPB};
  uint8                         i                       = 0;
  pbm_field_s_type             *uncompressed_data_buf   = NULL;

  pb_id    = pbm_session_record_id_to_phonebook_type(rec_id);
  location = pbm_session_record_id_to_location(rec_id);

  rex_enter_crit_sect(&pbm_crit_sect);

  if(pb_id.device_type == PBM_ECC)
  {
    boolean                     loc_found        = FALSE;
    pbm_device_type             pbm_ecc_pb_ids[] = { PBM_ECC, PBM_ECC_OTHER};
    pbm_session_enum_type       session_type     = PBM_SESSION_MAX;

    session_type = pbm_record_id_to_session_type (rec_id);
    for (i = 0; i < ARR_SIZE(pbm_ecc_pb_ids) && loc_found == FALSE; ++i)
    {
      /* check the number of records for EFs in EFS */
      if ( NULL != (cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[i])))
      {
        if (cache->status == PBM_STATUS_READY)
        {
          if(location <= cache->num_of_records)
          {
            pb_id.device_type = pbm_ecc_pb_ids[i];
            pb_id.prov_type   =	pbm_session_type_to_provision_type(session_type);
            pb_id.pb_category = pbm_session_type_to_pb_category(session_type);
            loc_found         = TRUE;
          }
          else
          {
            location -= cache->num_of_records;
          }
        }
      }
    }
  }

  do
  {
    if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
    {
      ret = PBM_ERROR;
      break;
    }

    if ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS &&
         ret != PBM_ERROR_SYNC)
    {
      break;
    }

    if (location > cache->num_of_records)
    {
      UIM_MSG_ERR_2("Location %d is higher than length of cache %d.",
                    location, cache->num_of_records);
      ret = PBM_ERROR_INDEX_INVALID;
      break;
    }

    if (cache->circular == TRUE)
    {
      /* For circular buffers we need to so some special translation for location */
      location = PBM_CIRC_CACHE_NTHLOC(cache, location);
    }

    if (location == 0)
    {
      UIM_MSG_ERR_0("Location cannot be zero.");
      ret = PBM_ERROR_NOT_FOUND;
      break;
    }

    if ((cache->pb_cache_array == NULL) ||
        (cache->pb_cache_array[location] == NULL) ||
        (cache->pb_cache_array[location]->fields == NULL) ||
        (cache->pb_cache_array[location]->num_fields == 0))
    {
      ret = PBM_ERROR_NOT_FOUND;
      break;
    }

    if(PBM_SUCCESS != pbm_uncompress_fields(cache->pb_cache_array[location]->fields,
                                            cache->pb_cache_array[location]->num_fields,
                                            &uncompressed_data_buf,
                                            &num_fields_uncompressed,
                                            FALSE))
    {
      UIM_MSG_ERR_0("Can't copy fields");
      ret = PBM_ERROR;
    }
  } while (0);

  rex_leave_crit_sect(&pbm_crit_sect);

  if (ret == PBM_SUCCESS)
  {
    size = pbm_calculate_size_from_fields(uncompressed_data_buf, num_fields_uncompressed);
  }

  PBM_MEM_FREEIF(uncompressed_data_buf);
  return size;
}/* pbm_session_calculate_fields_size_from_id */


/*===========================================================================
FUNCTION PBM_SESSION_ENUM_REC_INIT_EXT

DESCRIPTION
Initializes searches of the address book for specific records

Does search to set up for subsequent calls to pbm_session_enum_next_rec_ext(), which
iterates through the records that match the given criteria.

Searching rules are as follows:
1)  Only records with the given category are matched.  If PBM_CAT_NONE is
    specified, then all categories are matched.
2)  Only records that contain at least one instance of a field with the
    given field ID are matched.  If PBM_FIELD_NONE is specified, then
    this criterion is ignored.
3)  Only records with data in the specified field that "matches" the data
    specified by data_ptr are matched.  If data_ptr is NULL, then this
    criterion is ignored.

  What "matches" means depends on the setting of flags:
          Default (0x0) is case insensitive, starts with, string match
          with FIELD_NONE.
          Bit 0:    0   - PBM_CASE_INSENSITIVE
          Bit 0:    1   - PBM_CASE_SENSITIVE
          Bit 1-2:  0   - PBM_SEARCHTYPE_STARTSWITH
          Bit 1-2:  1   - PBM_SEARCHTYPE_EXACT
          Bit 1-2:  10  - PBM_SEARCHTYPE_CONTAINS
          Bit 3:    0   - PBM_DATATYPE_STR
          Bit 3:    1   - PBM_DATATYPE_BYTE
          Bit 4:    0   - PBM_FIELDID_NONE
          Bit 4:    1   - PBM_FIELDID_PHONE (Any type of phone)
          Bit 5:    0   - PBM_CALLHISTORY_NONE
          Bit 5:    1   - PBM_CALLHISTORY_ALL (incoming, outgoing, missed)
          Bit 6:    0   - PBM_ENUMERATE_ALL
          Bit 6:    1   - PBM_ENUMERATE_AVAILABLE
          Bit 7:    0   - PBM_MATCHING_NORMAL
          Bit 7:    1   - PBM_MATCHING_GREATER (Not supported with SEARCHTYPE_CONTAINS).
          Bit 8:    0   - PBM_SORT_LASTNAME_FIRST
          Bit 8:    1   - PBM_SORT_LASTNAME_SECOND

  Example:  pbm_session_enum_rec_init_ext(pb_id, PBM_CAT_NONE, PBM_FIELD_NONE, "joh", 3,
                         PBM_CASE_INSENSITIVE | PBM_SEARCHTYPE_STARTSWITH, &handle)
            would match on the names "John Smith" and "Johann Bach"

DEPENDENCIES
pbm_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_enum_rec_init_ext(
  pbm_phonebook_type        pb_id,
  uint16                    category,
  pbm_field_id_e_type       field_id,
  const uint8              *data_ptr,
  uint16                    data_size,
  uint32                    flags,
  uint8                    *handle
)
{
  pbm_return_type               ret = PBM_SUCCESS;
  PB_CACHE                     *cache;
  pbm_session_enum_type         session_type = PBM_SESSION_DEFAULT;
  pbm_record_id_type           *temp_results = NULL; /* Will point to array of record IDs */
  int                           result_index;
  int                           size = 0;
  int                           num_caches = 0;
  int                           caches_used = 0;
  int                           i, handle_iterator;
  int                           cidx;
  pbm_cat_e_type                my_cat = (pbm_cat_e_type)category;
  pbm_internal_field_info      *field_info_ptr = NULL;
  boolean                       avail_flag = (((flags & PBM_ENUMERATE_MASK) ==
                                                PBM_ENUMERATE_AVAILABLE) ? TRUE : FALSE);
  PB_CACHE                     *pb_cache_list[PBM_RESERVED] = {NULL,}; /* Cache list. */
  pbm_device_type              *pb_ids = NULL;                  /* Phonebook list */
  PB_CACHE                     *pb_cache = NULL;    /* Used when list is one big. */
  pbm_device_type               ch_pb_ids[] = {PBM_DIALED, PBM_RCVD, PBM_MISS};
  pbm_device_type               pbm_default_pb_ids[] = {PBM_EFS, PBM_SDL, PBM_ADN, PBM_FDN, PBM_SDN};

  /* First make sure the category is valid for the search. */
  if (pb_id.device_type != PBM_ECC_OTHER &&
      my_cat != PBM_CAT_NONE && pb_id.device_type != pbm_cat_to_phonebook(my_cat))
  {
    UIM_MSG_ERR_2("Category %d and phonebook %d does not match",
                  my_cat, pb_id.device_type);
    return PBM_ERROR_CAT_NOT_SUPPORTED;
  }

  PBM_MEM_ALLOC( field_info_ptr,sizeof(pbm_internal_field_info));
  if (NULL == field_info_ptr)
  {
    UIM_MSG_ERR_0("malloc failed for field_info_ptr");
    return PBM_ERROR;
  }

  if(pbm_get_internal_field_info(pb_id,field_info_ptr) == PBM_SUCCESS)
  {
    /* Now if they are searching for a field, make sure they can. */
    if (field_id != PBM_FIELD_NONE)
    {
      for (i = 0; i < field_info_ptr->num_fields; i++)
      {
        if (field_id == field_info_ptr->info[i].field_id)
        {
          if (((my_cat == PBM_CAT_NONE) && !field_info_ptr->info[i].field_search) ||
              ((my_cat != PBM_CAT_NONE) && !field_info_ptr->info[i].field_cat_search))
          {
            PBM_MEM_FREEIF (field_info_ptr);
            return PBM_ERROR_NOT_SUPPORTED;
          }
          break;
        }
      }
    }
  }
  else
  {
    PBM_MEM_FREEIF (field_info_ptr);
    return PBM_ERROR;
  }

  PBM_MEM_FREEIF (field_info_ptr);
  /* select the handle for the current client */
  rex_enter_crit_sect(&pbm_crit_sect);
  for(handle_iterator=0; handle_iterator< PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT; handle_iterator++)
  {
    if(g_pbm_session_rec_init_handles[handle_iterator]== PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE)
    {
      *handle = handle_iterator; /* this handle has to be used by clients while calling the enum_next_rec_id */
      g_pbm_session_rec_init_handles[handle_iterator]=PBM_ENUM_REC_INIT_HANDLE_IN_USE;
      break;
    }
  }
  if(handle_iterator == PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT )
  {
    *handle = 0xff;
    ret = PBM_ERROR; /* return error in this case as there is no free handle */
	goto END_FUNC_ENUM_REC_INIT_LEAVE_CS;
  }
  PBM_MEM_FREEIF(g_search_results_ext[handle_iterator]);

  /* When all call history phonebooks are enumerated, pb_id
   * could be PBM_DEFAULT */
  if (pb_id.device_type == PBM_DEFAULT)
  {
    if ((flags & PBM_CALLHISTORY_MASK) == PBM_CALLHISTORY_ALL)
    {
      pb_ids = ch_pb_ids;
      num_caches = ARR_SIZE(ch_pb_ids);
    } else
    {
      pb_ids = pbm_default_pb_ids;
      num_caches = ARR_SIZE(pbm_default_pb_ids);
    }
    /* Check if the caches are ready.
     * For PBM_DEFAULT, no SIM and no SIM file should not be returned as an
     * error */
    for (cidx = 0; cidx < num_caches; ++cidx)
    {/* case for all EFs in pbm_default_pb_ids */
      switch (pb_ids[cidx])
      {
        case PBM_MISS:
        case PBM_RCVD:
        case PBM_DIALED:
        case PBM_EFS:
        case PBM_SDL:
          //session_type is optional since for computing above phonebook's caches it is not required
          if ( NULL != (cache = pbm_device_id_to_cache(session_type, pb_ids[cidx])))
          {/* check the number of records for files in EFS */
            if (cache->status == PBM_STATUS_READY)
            {
              pb_cache_list[caches_used++] = cache;
            } else
            {
              UIM_MSG_ERR_2("Cache for PB %d not available %d", (pb_ids?pb_ids[cidx]:pb_id.device_type), ret);
              if (!avail_flag &&
                  ret != PBM_ERROR_NO_SIM && ret != PBM_ERROR_NOT_AVAILABLE && ret != PBM_ERROR_RESTRICTED)
              {
                g_pbm_session_rec_init_handles[handle_iterator]=PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE; /* as error is returned handle is not in use */
                *handle = 0xff;
                goto END_FUNC_ENUM_REC_INIT_LEAVE_CS;
              }
            }
          }
          break;

		case PBM_ADN:
        case PBM_SDN:
        case PBM_FDN:
        default:
          /* Follow app preference for traversing all EFs present under ADF DIR */
          for (session_type = PBM_SESSION_DEFAULT; session_type < PBM_SESSION_MAX; session_type++)
          {
            if ( NULL != (cache = pbm_device_id_to_cache(session_type, pb_ids[cidx])))
            {/* check the number of records for files in EFS */
              if (cache->status == PBM_STATUS_READY)
              {
                pb_cache_list[caches_used++] = cache;
              }
              else
              {
                UIM_MSG_ERR_2("Cache for PB %d not available %d", ((pb_ids)?pb_ids[cidx]:pb_id.device_type), ret);
                //(ret != PBM_ERROR_RESTRICTED)check is to skip if particular cache have restricted access CR146463
                //ex - some SIMs may restrict access to ADN records when FDN is disabled
                if (!avail_flag &&
                    ret != PBM_ERROR_NO_SIM && ret != PBM_ERROR_NOT_AVAILABLE && ret != PBM_ERROR_RESTRICTED)
                {
                  g_pbm_session_rec_init_handles[handle_iterator]=PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE; /* as error is returned handle is not in use */
                  *handle = 0xff;
                  goto END_FUNC_ENUM_REC_INIT_LEAVE_CS;
                }
              }
            }
          }
          break;
      }
    }
  } /*  if (pb_id.device_type == PBM_DEFAULT)... */
  //process all the ECC caches
  else if (pb_id.device_type == PBM_ECC)
  {
    pbm_device_type pbm_ecc_pb_ids[] = {PBM_ECC, PBM_ECC_OTHER};

    session_type = pbm_mapping_to_session(pb_id.pb_category,(pbm_slot_type) pb_id.slot_id, pb_id.prov_type);
    num_caches   = ARR_SIZE(pbm_ecc_pb_ids);
    for (cidx = 0; cidx < num_caches; ++cidx)
    {
      if ( NULL != (cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[cidx])))
      {
        /* check the number of records for files in EFS */
        if (cache->status == PBM_STATUS_READY)
        {
          pb_cache_list[caches_used++] = cache;
        }
      }
    }
  }
  else
  {
    /* Check if the cache is ready */
    if ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS)
    {
      g_pbm_session_rec_init_handles[handle_iterator]=PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE; /* as error is returned handle is not in use */
      *handle = 0xff;
	  goto END_FUNC_ENUM_REC_INIT_LEAVE_CS;
    }

    pb_cache_list[0] = pbm_pb_id_to_cache(pb_id);
    caches_used = 1;
  }

  /* Allocate the maximum number of possible results. */
  for (cidx = 0; cidx < caches_used; ++cidx)
  {
    size += pb_cache_list[cidx]->records_in_use;
  }

  if (size == 0)
  {
    /* as we are returning success here in this condition handle is still in use */
    g_search_length_ext[handle_iterator] = 0;
    g_search_enumerator_ext[handle_iterator] = 0;
    g_pbm_session_rec_init_handles[handle_iterator] = PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE;
    UIM_MSG_HIGH_1("PBM pbm_session_enum_rec_init_ext handle returned is %d, number of records matched = 0", handle_iterator);
    ret = PBM_ERROR_NOT_FOUND;
    goto END_FUNC_ENUM_REC_INIT_LEAVE_CS;
  }

  /* Allocate srch results array*/
  PBM_MEM_ALLOC(temp_results,(uint32)size * sizeof(pbm_record_id_type));

  if (temp_results == NULL)
  {
    g_pbm_session_rec_init_handles[handle_iterator] = PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE; /* in case of error returned handle should be free */
    g_search_length_ext[handle_iterator] = 0;
    g_search_enumerator_ext[handle_iterator] = 0;
    UIM_MSG_HIGH_1("PBM pbm_session_enum_rec_init_ext handle returned is %d, number of records matched = 0, temp_results not allocated memory",
                    handle_iterator);
    ret = PBM_ERROR;
    goto END_FUNC_ENUM_REC_INIT_LEAVE_CS;
  }

  result_index = 0;

  /* All call history phonebooks */
  /* pb_id could be PBM_DEFAULT or any one of the call history phonebooks */
  for (cidx = 0; cidx < caches_used; ++cidx)
  {
    pb_cache = pb_cache_list[cidx];

    if (!pb_cache)
    {
      UIM_MSG_ERR_1("Cache was NULL for index %d", cidx);
      continue;
    }

    for ( i = 1; i <= pb_cache->num_of_records; i++ )
    {
      int index;

      if (pb_cache->circular)
        index = PBM_CIRC_CACHE_NTHLOC(pb_cache, i);
      else
        index = i;

      if ( (pb_cache->pb_cache_array) && (pb_cache->pb_cache_array[index]) )
      {
        if (match_found(my_cat, field_id, data_ptr, data_size, flags,
                        pb_cache->pb_cache_array[index]))
        {
          if (result_index >= size)
          {
            UIM_MSG_ERR_2("Allocated %d records, index %d", size, result_index);
            break;
          }
          if (pb_cache->circular)
          {
            //Hide the location shifting details for circular files from outside apps.
            temp_results[result_index++] = pbm_session_location_to_record_id(pb_cache->pb_id, i);
          } else
          {
            temp_results[result_index++] = pb_cache->pb_cache_array[index]->record_id;
          }
        }
      }
    }
  }
  /* Allocate g_search_results[] and copy results in*/
  do
  {
    if (result_index == 0)
    {
      g_search_results_ext[handle_iterator] = NULL;
      g_search_length_ext[handle_iterator] = 0;
    } else
    {
      PBM_MEM_ALLOC(g_search_results_ext[handle_iterator], (uint32)result_index * sizeof(g_search_results_ext[0][0]));
      PBM_CHECK_PTR_BREAK(g_search_results_ext[handle_iterator], ret, PBM_ERROR_MEM_FULL);

      /* g_search_results is a 0-based array*/
      for (i=0; i<result_index; i++)
      {
        g_search_results_ext[handle_iterator][i] = temp_results[i];
      }

      g_search_length_ext[handle_iterator] = (int) result_index;
    }
  } while ( 0 );

  /* clean up the temp results */
  PBM_MEM_FREEIF(temp_results);

  /* Initialize enumerator for this client handle*/
  g_search_enumerator_ext[handle_iterator] = 0;
  ret = PBM_SUCCESS;
  UIM_MSG_HIGH_2("PBM pbm_session_enum_rec_init_ext handle returned is %d, number of records matched = %d",
  handle_iterator , g_search_length_ext[handle_iterator]);
END_FUNC_ENUM_REC_INIT_LEAVE_CS:
  rex_leave_crit_sect(&pbm_crit_sect);
  return ret;
}/* pbm_session_enum_rec_init_ext */


/*===========================================================================
FUNCTION PBM_SESSION_ENUM_REC_INIT_EXT_FREE_HANDLE

DESCRIPTION
  To be called by clients in order to free the handle that has been allocated to
  the client via pbm_session_enum_rec_init_ext.

  This API is to be used in case all the records are not read via
  pbm_session_enum_next_rec_id_ext. If all the records are read then handle
  gets free automatically.
  Handle wil be marked not in use and memory allocated to array containing the
  record ids will be deallocated.

DEPENDENCIES
pbm_session_enum_rec_init_ext() needs to be called first

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_enum_rec_init_ext_free_handle(
  uint8 handle
)
{
  if (handle >= PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT)
  {
    return PBM_ERROR ;
  }

  rex_enter_crit_sect(&pbm_crit_sect);

  PBM_MEM_FREEIF(g_search_results_ext[handle]);
  g_pbm_session_rec_init_handles[handle] = PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE;

  rex_leave_crit_sect(&pbm_crit_sect);

  UIM_MSG_HIGH_1("PBM pbm_session_enum_rec_init_ext_free_handle called for handle 0x%x", handle);

  return PBM_SUCCESS;
} /* pbm_session_enum_rec_init_ext_free_handle */


/*===========================================================================
FUNCTION PBM_SESSION_ENUM_NEXT_REC_ID_EXT

DESCRIPTION
  Retrieves the next record ID that matches the search criteria
  for the client handle returned to client via pbm_session_enum_rec_init_ext() .

  When the end of the enumeration is reached, the index is not reset to the
  beginning of the enumeration.Once all the records are read this API will return
  PBM_EOF Client should not call this API after recieving PBM_EOF as it may return
  stale data after that. The caller must call pbm_session_enum_rec_init_ext() to
  reinitialize the search criteria...

DEPENDENCIES
pbm_session_enum_rec_init_ext() needs to be called first

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_enum_next_rec_id_ext(
  pbm_record_id_type *rec_id_ptr,
  uint8               handle
)
{
  if (handle >= PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT)
  {
    return PBM_ERROR;
  }
  else
  {
    pbm_return_type              ret      = PBM_ERROR;
    PB_CACHE                    *cache    = NULL;
    uint16                       location = 0;
    pbm_phonebook_type           pb_id    = {PBM_GPB};

    PBM_CHECK_PTR_RET(rec_id_ptr, PBM_ERROR);

    rex_enter_crit_sect(&pbm_crit_sect);

    if (g_search_enumerator_ext[handle] >= g_search_length_ext[handle])
    {
      /* once we are here all the reords re being read.the handle must be freed by clients explicitly */
      *rec_id_ptr = PBM_RECID_NULL;
      UIM_MSG_HIGH_1("PBM pbm_session_enum_next_rec_id_ext called for handle %d, all records already read.", handle);
      ret = PBM_EOF;
      goto END_FUNC_ENUM_NEXT_REC_ID_LEAVE_CS;
    }

    if (g_pbm_session_rec_init_handles[handle] == PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE)
    {
      /* in case if the handle has been freed explicitly by the client */
      *rec_id_ptr = PBM_RECID_NULL;
      UIM_MSG_HIGH_1("PBM pbm_session_enum_next_rec_id_ext called for handle %d, handle not in use returning PBM_EOF.", handle);
      ret = PBM_EOF;
      goto END_FUNC_ENUM_NEXT_REC_ID_LEAVE_CS;
    }

    /* Empty list?*/
    if (!g_search_results_ext[handle])  /* need to have return PBM_EOF check before this statement */
    {
      *rec_id_ptr = PBM_RECID_NULL;
      ret = PBM_ERROR_NOT_FOUND;
      goto END_FUNC_ENUM_NEXT_REC_ID_LEAVE_CS;
    }

    /* Skip over blank entries*/
    /* (Blank because they have been deleted from the cache)*/
    while (g_search_enumerator_ext[handle] < g_search_length_ext[handle])
    {
      *rec_id_ptr = g_search_results_ext[handle][g_search_enumerator_ext[handle]];

      /* see if the entry is blank */
      pb_id = pbm_session_record_id_to_phonebook_type(*rec_id_ptr);

      if((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
      {
        *rec_id_ptr = PBM_RECID_NULL;
        ret = PBM_ERROR_NOT_FOUND;
        goto END_FUNC_ENUM_NEXT_REC_ID_LEAVE_CS;
      }

      do
      {
        if((ret = check_pb_ready(pb_id)) != PBM_SUCCESS &&
            ret != PBM_ERROR_SYNC)
        {
          break;
        }

        location = pbm_session_record_id_to_location(*rec_id_ptr);
        if (location > cache->num_of_records)
        {
          UIM_MSG_ERR_0("Location is higher than length of cache.");
          ret = PBM_ERROR_INDEX_INVALID;
          break;
        }

        if (cache->circular == TRUE)
        {
          /* For circular buffers we need to so some special translation for location */
          location = PBM_CIRC_CACHE_NTHLOC(cache, location);
        }
        if (location == 0)
        {
          UIM_MSG_ERR_0("Location cannot be zero.");
          ret = PBM_ERROR_NOT_FOUND;
          break;
        }
        if((cache->pb_cache_array == NULL) ||
           (cache->pb_cache_array[location] == NULL) ||
           (cache->pb_cache_array[location]->fields == NULL) ||
           (cache->pb_cache_array[location]->num_fields == 0))
        {
          ret = PBM_ERROR_NOT_FOUND;
          break;
        }
      } while (0);
      if (ret == PBM_SUCCESS)
      {
        break;
      }
      g_search_enumerator_ext[handle]++;
    }
    if (g_search_enumerator_ext[handle] == g_search_length_ext[handle])  /* for case when ret != success for all the traversed records  or to return the PBM_EOF for very first time*/
    {
      g_search_enumerator_ext[handle]++;
      *rec_id_ptr = PBM_RECID_NULL;
      ret = PBM_EOF;
      goto END_FUNC_ENUM_NEXT_REC_ID_LEAVE_CS;
    }
    g_search_enumerator_ext[handle]++;
    ret = PBM_SUCCESS;

    END_FUNC_ENUM_NEXT_REC_ID_LEAVE_CS:
    rex_leave_crit_sect(&pbm_crit_sect);
    return ret;
  }
}/* pbm_session_enum_next_rec_id_ext */


/*===========================================================================
FUNCTION PBM_SESSION_RECORD_READ

DESCRIPTION
Searches the cache appropriate to the given phonebook and retrieves
information contained in a specified record, returning success/failure
indication.  Does not delete the record from the address book.  If no
record is found, PBM_ERROR_NOT_FOUND is returned.

The layout of data_buf when it is returned is that it starts with
*num_fields_ptr number of pbm_field_s_type structures.  After those structures,
is the data for those fields.  pbm_get_field_data_pointer() can get pointers
to the actual field data.


DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_record_read(
  pbm_record_id_type            rec_id,
  uint16                       *category_ptr,
  int                          *num_fields_ptr,
  uint8                        *data_buf,
  uint32                        data_buf_size
)
{
  PB_CACHE                     *cache                 = NULL;
  uint16                        location              = 0;
  pbm_return_type               ret                   = PBM_ERROR;
  pbm_phonebook_type            pb_id                 = {PBM_GPB};
  uint8                         i                     = 0;
  pbm_field_s_type             *uncompressed_data_buf = NULL;
  uint32                        src_buf_size          = 0;

  PBM_CHECK_PTR_RET(num_fields_ptr, PBM_ERROR);

  *num_fields_ptr = 0;

  pb_id = pbm_session_record_id_to_phonebook_type(rec_id);
  location = pbm_session_record_id_to_location(rec_id);

  rex_enter_crit_sect(&pbm_crit_sect);

  if(pb_id.device_type == PBM_ECC)
  {
    boolean                     loc_found        = FALSE;
    pbm_device_type             pbm_ecc_pb_ids[] = {PBM_ECC, PBM_ECC_OTHER};
    pbm_session_enum_type       session_type     = PBM_SESSION_MAX;

    session_type = pbm_record_id_to_session_type (rec_id);
    for (i = 0; i < ARR_SIZE(pbm_ecc_pb_ids) && loc_found == FALSE; ++i)
    {
      /* check the number of records for EFs in EFS */
      if ( NULL != (cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[i])))
      {
        if (cache->status == PBM_STATUS_READY)
        {
          if(location <= cache->num_of_records)
          {
            pb_id.device_type = pbm_ecc_pb_ids[i];
            pb_id.prov_type   =	pbm_session_type_to_provision_type(session_type);
            pb_id.pb_category = pbm_session_type_to_pb_category(session_type);
            loc_found         = TRUE;
          }
          else
          {
            location -= cache->num_of_records;
          }
	    }
      }
    }
  }

  do
  {
    if((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
    {
      ret = PBM_ERROR;
      break;
    }

    if((ret = check_pb_ready(pb_id)) != PBM_SUCCESS &&
        ret != PBM_ERROR_SYNC)
    {
      break;
    }

    if(location > cache->num_of_records)
    {
      UIM_MSG_ERR_0("Location is higher than length of cache.");
      ret = PBM_ERROR_INDEX_INVALID;
      break;
    }

    if(cache->circular == TRUE)
    {
      /* For circular buffers we need to so some special translation for location */
      location = PBM_CIRC_CACHE_NTHLOC(cache, location);
    }

    if(location == 0)
    {
      UIM_MSG_ERR_0("Location cannot be zero.");
      ret = PBM_ERROR_NOT_FOUND;
      break;
    }

    if((cache->pb_cache_array == NULL) ||
       (cache->pb_cache_array[location] == NULL) ||
       (cache->pb_cache_array[location]->fields == NULL) ||
       (cache->pb_cache_array[location]->num_fields == 0))
    {
      ret = PBM_ERROR_NOT_FOUND;
      break;
    }

    if(category_ptr)
    {
      *category_ptr   = cache->pb_cache_array[location]->category;
    }

    if(num_fields_ptr)
    {
      *num_fields_ptr = cache->pb_cache_array[location]->num_fields;
    }

    if(PBM_SUCCESS != pbm_uncompress_fields((const pbm_field_s_type *)
                                             cache->pb_cache_array[location]->fields,
                                             cache->pb_cache_array[location]->num_fields,
                                             &uncompressed_data_buf,
                                             num_fields_ptr,
                                             FALSE))
    {
      UIM_MSG_ERR_0("Can't copy fields");
      ret = PBM_ERROR;
      break;
    }

    src_buf_size = pbm_calculate_size_from_fields(uncompressed_data_buf, *num_fields_ptr);
    memscpy(data_buf, data_buf_size, uncompressed_data_buf, src_buf_size);
    PBM_MEM_FREEIF(uncompressed_data_buf);
    ret = PBM_SUCCESS;

    if(pb_id.device_type != PBM_ECC && pb_id.device_type != PBM_ECC_OTHER)
    {
      UIM_MSG_HIGH_5("PBM: Attempt to read non ECC compress record 0x%x from slot 0x%x, device type 0x%x, pb category 0x%x compress found 0x%x fields",
                      rec_id,
                      pb_id.slot_id,
                      pb_id.device_type,
                      pb_id.pb_category,
                      cache->pb_cache_array[location]->num_fields);
    }
  } while (0);
  rex_leave_crit_sect(&pbm_crit_sect);
  return ret;
}/* pbm_session_record_read */


/*===========================================================================
  FUNCTION pbm_get_slot_info()

  DESCRIPTION
    Get the slot info for the given PBM session
    
  PARAMETERS
    pbm_session_enum_type pbm_session
    
  RETURN VALUE

    mmgsdi_slot_id_enum_type
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint16  pbm_get_slot_info(
  pbm_session_enum_type   pbm_session
)
{
  uint16     mmgsdi_slot_id;

  rex_enter_crit_sect(&pbm_crit_sect);

  mmgsdi_slot_id   = PBM_PBM_SLOT_TO_MMGSDI_SLOT(pbm_session_info[pbm_session].slot_id);

  rex_leave_crit_sect(&pbm_crit_sect);

  return mmgsdi_slot_id;
} /* pbm_get_slot_info */


/*===========================================================================
  FUNCTION pbm_is_card_present_in_slot()
  DESCRIPTION
    Checks if card is present in the given slot or not
  PARAMETERS
    Slot
  RETURN VALUE
    boolean 
    - TRUE if card is present in that slot 
    - FALSE if card is not present in that slot 
  DEPENDENCIES
    None
  SIDE EFFECTS
    None
===========================================================================*/
boolean pbm_is_card_present_in_slot(
  uint8     slot
)
{
  boolean is_present = FALSE ; 

  PBM_VALIDATE_SLOT_RET(slot,is_present);

  rex_enter_crit_sect(&pbm_crit_sect);

  is_present =   pbm_card_status[slot-1];

  rex_leave_crit_sect(&pbm_crit_sect);

  return is_present;
}/* pbm_is_card_present_in_slot */


/*===========================================================================
  FUNCTION pbmif_mmgsdi_session_to_pbm_session()

  DESCRIPTION
    MMGSDI session enum mapping to PBM session type

  PARAMETERS
    mmgsdi_session

  RETURN VALUE
   pbm_session_enum_type 

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
pbm_session_enum_type  pbmif_mmgsdi_session_to_pbm_session(
  mmgsdi_session_type_enum_type       mmgsdi_session
)
{
  pbm_session_enum_type l_pbm_session = PBM_SESSION_GPB_1;

  switch(mmgsdi_session)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      l_pbm_session = PBM_SESSION_LPB_GW;
      break;

    case MMGSDI_1X_PROV_PRI_SESSION:
      l_pbm_session = PBM_SESSION_LPB_1X;
      break;

	case MMGSDI_GW_PROV_SEC_SESSION:
      l_pbm_session = PBM_SESSION_LPB_GW_SECONDARY;
      break;

	case MMGSDI_1X_PROV_SEC_SESSION:
      l_pbm_session = PBM_SESSION_LPB_1X_SECONDARY;
      break;

	case MMGSDI_GW_PROV_TER_SESSION:
      l_pbm_session = PBM_SESSION_LPB_GW_TERTIARY;
      break;

	case MMGSDI_1X_PROV_TER_SESSION:
      l_pbm_session = PBM_SESSION_LPB_1X_TERTIARY;
      break;

	case MMGSDI_CARD_SESSION_SLOT_1:
      l_pbm_session = PBM_SESSION_GPB_1;
      break;

	case MMGSDI_CARD_SESSION_SLOT_2:
      l_pbm_session = PBM_SESSION_GPB_2;
      break;

	case MMGSDI_CARD_SESSION_SLOT_3:
      l_pbm_session = PBM_SESSION_GPB_3;
      break;

	default:
      l_pbm_session = PBM_SESSION_MAX;
      break;

  }
  return l_pbm_session;
}/* pbmif_mmgsdi_session_to_pbm_session */

