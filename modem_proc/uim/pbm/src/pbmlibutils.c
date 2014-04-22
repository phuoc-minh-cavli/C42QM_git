/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER LIBRARY UTILS

GENERAL DESCRIPTION
  This file contains the common utility functions used by PBM lib (external interface).

  Copyright (c) 2002 - 2014, 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE
$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbmlibutils.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/16   nr      ECC optimizations for IoE devices
05/26/16   sp      Logging improvements
05/16/16   sp      Logging improvements
02/06/16   nr      ECC only support in PBM
01/31/14   NR      Use v2.0 diag macros
04/10/14   NR      Added memset to set the *ptyped_fields_buf_ptr allocated in function
                   pbm_allocate_fields ()
01/12/14   NR      TMC header deprecation changes
10/14/09  krishnac Support for feature DSDS
07/10/09  ashwanik Support for CSIM and Dual Sim Features
05/04/09   krishna changes for CR 167785
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
10/10/06   cvs     Move messages to PBM's diag msg ID
10/03/06   cvs     Add support for MBN
08/02/06   cvs     Header file cleanup for WINCE
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
03/24/06   cvs     %x -> 0x%x in log messages
02/28/06   cvs     Add support for MBDN
01/09/06   cvs     Lint fixes
11/16/05   cvs     Lint fixes
11/16/05   cvs     Moved pbm_record_id_to_file_id() to pbmutils.c
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/12/05   AT      Added support for UCS2 encoding.
04/21/05   cvs     Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "uim_msg.h"
#include "stringl/stringl.h"
#include "pbmutils.h"
#include "pbmlib.h"

/*===========================================================================
                         DEFINITIONS
===========================================================================*/

/* The phone book's record number uses the top 5 bits
 * to indicate the phonebook.  Below is the mapping.
 * Use PBM_RESERVED for new phonebooks in future */
pbm_device_type pbm_pb_bits_to_pb_id[32] = {
PBM_EFS,            /* Phonebook bits 00000 */
PBM_SDN,            /* Phonebook bits 00001 */
PBM_ADN,            /* Phonebook bits 00010 */
PBM_FDN,            /* Phonebook bits 00011 */
PBM_MSISDN,         /* Phonebook bits 00100 */
PBM_ECC,            /* Phonebook bits 00101 */
PBM_SDL,            /* Phonebook bits 00110 */
PBM_MBDN,           /* Phonebook bits 00111 */
PBM_DIALED,         /* Phonebook bits 01000 */
PBM_RCVD,           /* Phonebook bits 01001 */
PBM_MBN,            /* Phonebook bits 01010 */
PBM_MISS,           /* Phonebook bits 01011 */
PBM_LND,            /* Phonebook bits 01100 */
PBM_ECC_OTHER,      /* Phonebook bits 01101 */
PBM_ICI,            /* Phonebook bits 01110 */
PBM_GRPNAME,        /* Phonebook bits 01111 */
PBM_OCI,            /* Phonebook bits 10000 */
PBM_ADN2,           /* Phonebook bits 10001 */
PBM_FDN2,           /* Phonebook bits 10010 */
PBM_BDN2,           /* Phonebook bits 10011 */
PBM_LND2,           /* Phonebook bits 10100 */
PBM_OCI2,           /* Phonebook bits 10101 */
PBM_ICI2,            /* Phonebook bits 10110 */
PBM_SDN2,            /* Phonebook bits 10110 */
PBM_MSISDN2,         /* Phonebook bits 10110 */
PBM_MBDN2,           /* Phonebook bits 10110 */
PBM_MBN2,            /* Phonebook bits 10110 */
PBM_ECC2,            /* Phonebook bits 10110 */
PBM_AAS,             /* Phonebook bits 10111 */
PBM_RESERVED, /* For further use */
PBM_RESERVED, /* For further use */
PBM_RESERVED  /* For further use */
};

/*===========================================================================
                         FUNCTIONS
===========================================================================*/


/*===========================================================================
FUNCTION PBM_ALLOCATE_FIELDS

DESCRIPTION
  Allocate the complete memory needed for the field array in the cache
  structure.  Allocate memory from the global heap.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_allocate_fields(uint8 **fields_buf_ptr,
                            uint32 *fields_buf_size,
                            uint32 num_fields,
                            const uint32 *sizes)
{
   uint32 i;
   uint32 total_size = 0;
   uint32 mem_offset = 0;
   const uint32 size_one_field = ALIGNED_SIZE(sizeof(pbm_field_s_type));
   pbm_field_s_type **ptyped_fields_buf_ptr = (pbm_field_s_type **)
                                              fields_buf_ptr;

   PBM_CHECK_PTR3_RET(ptyped_fields_buf_ptr, sizes, fields_buf_size, FALSE);

   *ptyped_fields_buf_ptr = NULL;
   total_size = num_fields * size_one_field;
   for (i = 0; i < num_fields; i++)
   {
      total_size += ALIGNED_SIZE (sizes[i]);
   }

   *fields_buf_size = total_size;

   if (total_size > 0)
   {
      //here intentionaly mem_malloc being used instead PBM_MEM_ALLOC since this
      //call be used from the clients directly.
      *ptyped_fields_buf_ptr = (pbm_field_s_type *)malloc(total_size);
      if (*ptyped_fields_buf_ptr == NULL)
      {
         return FALSE;
      }

      memset(*ptyped_fields_buf_ptr,0,total_size);
      mem_offset += num_fields * size_one_field;
      for (i = 0; i < num_fields; i++)
      {
         (*ptyped_fields_buf_ptr)[i].buffer_offset = mem_offset;
         mem_offset += ALIGNED_SIZE(sizes[i]);
         mem_offset -= size_one_field; /* take #bytes from next field to its data */
      }
   }
   UIM_MSG_LOW_2("PBM allocated from libc %d bytes 0x%x",
           total_size, fields_buf_ptr ? *fields_buf_ptr : 0);
   return TRUE;
}


/*===========================================================================
FUNCTION PBM_FREE_FIELDS

DESCRIPTION
  Frees the memory allocated by pbm_allocate_fields.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_free_fields( uint8 **fields, int num_fields)
{
   PBM_CHECK_PTR_RET(fields, VOID);

   if ((num_fields > 0) && (*fields != NULL))
   {
      UIM_MSG_LOW_1("PBM freed from libc heap 0x%x", *fields);
   
      //here intentionaly mem_free being used instead PBM_MEM_FREEIF since this
      //call be used from the clients directly. The memory allocated also done using mem_malloc
      free(*fields);
   } else
   {
      UIM_MSG_ERR_2("PBM not freeing memory, num fields %d, ptr 0x%x", num_fields, *fields);
   }
   *fields = NULL;

}


/*===========================================================================
FUNCTION PBM_GET_FIELD_DATA_PTR

DESCRIPTION
  This function is just a shortcut return the pointer to the data buffer for
  a given field number.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void * pbm_get_field_data_ptr(const void *field_buf,
                              int field_num)
{
   uint8 *ptr;
   uint32 offset;
   pbm_field_s_type *ptyped_field_buf = (pbm_field_s_type*)field_buf;

   PBM_CHECK_PTR_RET(ptyped_field_buf, NULL);

   offset = ptyped_field_buf[field_num].buffer_offset;

   ptr = (uint8*)&(ptyped_field_buf[field_num]) + offset;

   return(void*)ptr;
}


/*===========================================================================
FUNCTION PBM_FILL_FIELD

DESCRIPTION
  This function is just a shortcut to fill in the four different fields
  which make up the field structure.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_fill_field(pbm_field_s_type *field, pbm_field_id_e_type field_id,
                    pbm_field_type_e_type field_type, const byte *buffer, uint16 data_len)
{
   void *field_data_ptr;

   PBM_CHECK_PTR2_RET(field, buffer, VOID);

   if (!data_len)
   {
      UIM_MSG_ERR_1("Invalid data len passed field_id 0x%x", field_id);
      return;
   }

   field->field_id   = field_id;
   field->field_type = field_type;

   field_data_ptr = pbm_get_field_data_ptr(field,0);
   memscpy(field_data_ptr, data_len, buffer, data_len);
   field->data_len   = data_len;
}


/*===========================================================================
FUNCTION pbm_session_record_id_to_location

DESCRIPTION
Converts a record ID to a location (extracts the location information from
the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_session_record_id_to_location(pbm_record_id_type record_id)
{
   /* Return lower 11 bits of record ID*/
   return((uint16)(record_id & PBM_LOC_MASK));
}

/*===========================================================================
FUNCTION pbm_record_id_to_location

DESCRIPTION
Converts a record ID to a location (extracts the location information from
the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_record_id_to_location(uint16 record_id)
{
   /* Return lower 11 bits of record ID*/
   return((uint16)(record_id & PBM_LOC_MASK));
}


/*===========================================================================
FUNCTION PBM_LOCATION_TO_RECORD_ID

DESCRIPTION
Converts a a phonebook ID plus a location to a record ID

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_location_to_record_id(pbm_device_type pb_id, int location)
{
  uint16 pb_code = 0;
  uint16 i;

  for (i = 0; i < (uint16) ARR_SIZE(pbm_pb_bits_to_pb_id); i++)
  {
    if (pbm_pb_bits_to_pb_id[i] == pb_id)
    {
      pb_code = i;
      break;
    }
  }

  if (i >= (uint16) ARR_SIZE(pbm_pb_bits_to_pb_id))
  {
    UIM_MSG_ERR_1("Invalid pbm_device_type for conversion: %d.", pb_id);
  }

  /* Put pb_id in upper 5 bits and location in lower 11 bits*/
  return ( ((pb_code & PBM_PB_MASK) << PBM_NUM_LOC_BITS) |
           ((uint16) location & PBM_LOC_MASK) );
}

/*===========================================================================
FUNCTION pbm_session_location_to_record_id

DESCRIPTION
Converts a a phonebook ID plus a location to a record ID

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_record_id_type pbm_session_location_to_record_id(pbm_phonebook_type pb_id, int location)
{
   uint16 pb_code = 0;
   uint16 i;

   for (i = 0; i < (pbm_record_id_type) ARR_SIZE(pbm_pb_bits_to_pb_id); i++)
   {
      if (pbm_pb_bits_to_pb_id[i] == pb_id.device_type)
      {
         pb_code = i;
         break;
      }
   }

   if (i >= (pbm_record_id_type) ARR_SIZE(pbm_pb_bits_to_pb_id))
   {
      UIM_MSG_ERR_1("Invalid pbm_phonebook_type for conversion: %d.", pb_id.device_type);
   }

   /* Put prov_type <3 bits> + slot <2 bits> + pb_category <1 bit> + pb_code <5 bits> + location <11 bits> */
   return((((uint32)pb_id.prov_type & PBM_PROV_MASK) << (PBM_NUM_SLOT_BITS + PBM_NUM_PB_TYPE_BITS + PBM_NUM_PB_BITS + PBM_NUM_LOC_BITS)) |
          ((pb_id.slot_id & PBM_SLOT_MASK) << (PBM_NUM_PB_TYPE_BITS + PBM_NUM_PB_BITS + PBM_NUM_LOC_BITS)) |
          (((uint32)pb_id.pb_category & PBM_PB_TYPE_MASK) << (PBM_NUM_PB_BITS + PBM_NUM_LOC_BITS)) |
          ((pb_code & PBM_PB_MASK) << PBM_NUM_LOC_BITS) |
          ((uint32) location & PBM_LOC_MASK) );
}


/*===========================================================================
FUNCTION pbmutils_wstrlen;

DESCRIPTION
  int pbmutils_wstrlen(const uint16 *ucs2_str);

  ucs2_str:   [in]   The UCS2 string to count the characters.

  This function counts the number of UCS2 characters in a string.  The
  string must be terminated by 0x0000.

DEPENDENCIES
  None.

RETURN VALUE
  The number of UCS2 characters in the given string.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 pbmutils_wstrlen(const uint16 *ucs2_str)
{
  uint32 retval = 0;                  /* Value to be returned. */

  if (ucs2_str)
    while (*ucs2_str++)
    {
      retval++;
    } /* Increment retval until 0x0000 */
  return retval;                      /* Return what we counted. */
} /* pbmutils_wstrlen */
