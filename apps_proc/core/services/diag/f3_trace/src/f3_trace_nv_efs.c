
/*==============================================================================

                      F3 TRACE NV Item  R E G I S T R Y

  DESCRIPTION
    The F3 TRACE NV item Registry is a central location for configuration item storage.
    It serves as an interface to potentially various data sources. Currently its
    main data source is nonvolatile memory, via the EFS item interface.

  Copyright (c) 2010 - 2011, 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/services/diag/f3_trace/src/f3_trace_nv_efs.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
08/17/16      nk     Uncommented EFS calls
11/27/13      ph     Updated MSG 1.0 API references to MSG.2.0
11/19/13      rh     No longer write in defaults if NV items do not exist
05/28/13      sm     Resolve Klockwork issue in f3_trace_nv_efs_reg_get_default() 
                     by casting switch expression to same type as case constants
10/15/11      mcg    stealing code from cgps
05/03/10      gk     initial checkin
==============================================================================*/

#include "customer.h"
#include "comdef.h"

#include <math.h>
#include <float.h>
#include <stdio.h>

#include "fs_public.h"
#include "f3_trace_nv_efs.h"
#include "msg.h"
#include "assert.h"

/*===========================================================================
=============================================================================

   Private data structures and declarations
   
=============================================================================
===========================================================================*/
typedef struct
{
  f3_trace_nv_efs_reg_item_type item_start;
  f3_trace_nv_efs_reg_item_type item_end;
  char sz_pathname_prefix[10];
  const f3_trace_nv_efs_reg_item_struct* table_ptr;
} f3_trace_nv_efs_subsys_data_type;

static f3_trace_nv_efs_subsys_data_type f3_trace_nv_efs_subsys_table[F3_TRACE_NV_EFS_SUBSYS_MAX];

static const char* f3_trace_nv_efs_file_name[] =
{
  "f3_trace", /* F3_TRACE_NV_EFS_F3_TRACE */
  /* add new names above this line */
  "f3_trace_invalid"
};
static const char* f3_trace_nv_efs_file_ext=".conf";


/* ensure the order of the items in this table is same as f3_trace_nv_efs_reg_item_type */
static const f3_trace_nv_efs_reg_item_struct f3_trace_nv_reg_items[] =
{
  /* data location, item file name, size */

  /* F3_TRACE_NV_EFS_F3_TRACE_CONTROL */
  {"f3_trace_control", sizeof(uint8), 0, F3_TRACE_NV_EFS_REG_TYPE_INTEGRAL},
  /* F3_TRACE_NV_EFS_F3_TRACE_DETAIL */
  {"f3_trace_detail", sizeof(uint8), 0, F3_TRACE_NV_EFS_REG_TYPE_INTEGRAL},  
  /* F3_TRACE_NV_EFS_F3_TRACE_MAX_FILES */
  {"f3_trace_max_files", sizeof(uint8), 0, F3_TRACE_NV_EFS_REG_TYPE_INTEGRAL}

  /* add entries below this line and make sure you update f3_trace_nv_efs_reg_item_type*/
};


/*IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

                            Private Functions
   
IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII*/
/*==============================================================================
  FUNCTION
    f3_trace_nv_efs_find_subsys_entry

  DESCRIPTION
    Helper function to find the subsystem table entry for the given NV item.
 
  RETURNS
    index into the table
  
==============================================================================*/
static const f3_trace_nv_efs_subsys_data_type* f3_trace_nv_efs_find_subsys_entry(
  f3_trace_nv_efs_reg_item_type item_id
)
{
  const f3_trace_nv_efs_subsys_data_type* entry_ptr = NULL;
  uint8 table_idx;
  for (table_idx = 0; 
       (table_idx < ARR_SIZE(f3_trace_nv_efs_subsys_table)) && (entry_ptr == NULL);
       table_idx++)
  {
    if ((item_id >= f3_trace_nv_efs_subsys_table[table_idx].item_start) &&
        (item_id <= f3_trace_nv_efs_subsys_table[table_idx].item_end) &&
        (NULL != f3_trace_nv_efs_subsys_table[table_idx].table_ptr))
    {
      /* found it */
      entry_ptr = &f3_trace_nv_efs_subsys_table[table_idx];
      break;
    }
  }
  return entry_ptr;
}

/*==============================================================================
  FUNCTION
    f3_trace_nv_efs_reg_get_item_fname

  DESCRIPTION
    This is a local function
    Constructs the full pathname of item file name and returns the length of
    the filename char array and file_name_size. file_name_size value will be 0 if error.
    No bound checks for the array are done here. Make sure the checks are done before
    passing it to this function. function is intended to be a used within this file. 
 
  RETURNS
  
==============================================================================*/
static uint8 f3_trace_nv_efs_reg_get_item_fname(
  char* file_name, 
  uint8 file_name_buf_size, 
  boolean newline,
  const f3_trace_nv_efs_reg_item_struct *reg_items_ptr,
  const char* psz_pathname_prefix
)
{
  uint8 file_name_size = 0;
  /* item is already range checked in the calling function */
  file_name_size = (uint8)snprintf( file_name, 
                                    file_name_buf_size, 
                                    "/nv/item_files/f3_trace/%s/%s", 
                                    psz_pathname_prefix,
                                    reg_items_ptr->file_name );  

  if (newline)
  {
    /* needed only between entries of config file */
    file_name_size = (uint8)strlcat(file_name, "\n", file_name_buf_size);
    if (file_name_size >= file_name_buf_size)
    {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_MED, "F3_TRACE_NV_EFS_REG: could not add new line to file name file_size %d", 
              file_name_size);
      file_name_size = 0;
    }
  }
  return file_name_size;
}

/*==============================================================================
  FUNCTION
    f3_trace_nv_efs_init_internal

  DESCRIPTION
    Helper function for initialization required for NV items in the EFS.
    A config file is created and will hold the EFS file names of all NV items
    in the registry for the given subsystem.
 
  RETURN VALUE
    TRUE  - Success
    FALSE - Error during initialization
==============================================================================*/
static boolean f3_trace_nv_efs_init_internal(
  f3_trace_nv_efs_subsys_type type, 
  f3_trace_nv_efs_reg_item_type table_start,
  f3_trace_nv_efs_reg_item_type table_end,
  const f3_trace_nv_efs_reg_item_struct* f3_trace_nv_efs_reg_items,
  const char* psz_pathname_prefix
)
{
  uint8 file_size = 0;
  boolean ret_val = TRUE; 
  char conf_dir_name_full[]="/nv/item_files/conf/";
  const char* conf_file_name = NULL;
  char conf_name[F3_TRACE_NV_MAX_EFS_FILEPATH_SIZE];
  char file_name[F3_TRACE_NV_MAX_EFS_FILEPATH_SIZE];
  char garbage_buf[F3_TRACE_NV_MAX_EFS_FILEPATH_SIZE];
  uint16 table_entry = table_start;


  MSG(MSG_SSID_DIAG, MSG_LEGACY_MED,"F3_TRACE_NV_EFS: Registry Initialization"); 

  if (f3_trace_nv_efs_reg_items == NULL)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_MED,"F3_TRACE_NV_EFS_REG: Null table pointer.");
    ret_val = FALSE;
  } /* if null table*/
  else if (table_start > table_end)
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_MED,"F3_TRACE_NV_EFS_REG: invalid range: %d, %d", table_start, table_end);
    ret_val = FALSE;
  }
  else
  {
    switch (type)
    {
      case F3_TRACE_NV_EFS_CONFIG:
        conf_file_name = f3_trace_nv_efs_file_name[type];
        break;
      default:
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"F3_TRACE_NV_EFS: No config file available for this type %d", type); 
        ret_val = FALSE;
        break;
    }
    if (conf_file_name != NULL) 
    {
      while (table_entry <= table_end) 
      {
        /* build a .conf file for each entry to avoid buffering everything */
        (void)snprintf(conf_name, sizeof(conf_name), "%s%s%u%s", 
                       conf_dir_name_full, conf_file_name, (table_entry-table_start), f3_trace_nv_efs_file_ext);

        /* get full NV item file path
           note: "\n" is likely superfluous since we are 1:1 */
        file_size = f3_trace_nv_efs_reg_get_item_fname(
                        file_name, sizeof(file_name), TRUE,
                        &f3_trace_nv_efs_reg_items[table_entry],
                        psz_pathname_prefix);

        // 'proper' asserts - safe to NOP in production code (here to appease KW)
        ASSERT(file_size<=sizeof(garbage_buf));
        ASSERT(file_size<=sizeof(file_name));

        // test if conf file already exists before writing (cr412055) 
        // note: EFS requires the size to be exact or we could optimize better
        if (-1 != efs_get(conf_name, garbage_buf, file_size))
        {
          MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "F3_TRACE_NV_EFS: Skipping %u.conf file, already exists", (table_entry-table_start));
        }
        else //need to write conf file
        {
          /* write it to EFS */
          if (efs_put(conf_name,
                      file_name,
                      file_size,
                      O_AUTODIR|O_CREAT|O_RDWR,
                      F3_TRACE_NV_EFS_CONF_FILE_PERM) < 0)
          {
            MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"F3_TRACE_NV_EFS: Error writing %u.conf file", (table_entry-table_start));
            ret_val = FALSE;
          }
        }
        ++table_entry;
      }
    } /* ret_val = TRUE*/

    if (ret_val != FALSE)
    {
      /* bookkeeping */
      f3_trace_nv_efs_subsys_table[type].item_start = table_start;
      f3_trace_nv_efs_subsys_table[type].item_end = table_end;
      f3_trace_nv_efs_subsys_table[type].table_ptr = f3_trace_nv_efs_reg_items;
      strlcpy(f3_trace_nv_efs_subsys_table[type].sz_pathname_prefix,
              psz_pathname_prefix, 
              sizeof(f3_trace_nv_efs_subsys_table[type].sz_pathname_prefix));
    }
  } /* if f3_trace_nv_efs_reg_items != NULL */

  return  ret_val;
}

/*==============================================================================
  FUNCTION
    f3_trace_nv_efs_item_read_internal
 
  DESCRIPTION
    Read the specified item from EFS2 special items file. If the
    item is not found, a default value is returned. The function logs the read
    write activity.
 
  RETURN VALUE
    F3_TRACE_NV_EFS_REG_RW_STAT_OK         - Success
    F3_TRACE_NV_EFS_REG_RW_STAT_RD_DEFAULT - Returning default value
    F3_TRACE_NV_EFS_REG_RW_STAT_RD_ERR     - Error during read operation
    F3_TRACE_NV_EFS_REG_RW_STAT_INVALID    - Invalid paramters
==============================================================================*/
static f3_trace_nv_efs_reg_rw_stat_enum_type f3_trace_nv_efs_item_read_internal(
  f3_trace_nv_efs_reg_item_type item,
  uint32 data_size,
  void *data_ptr
)
{
  char file_name[F3_TRACE_NV_MAX_EFS_FILEPATH_SIZE]; /* file name with absolute address */
  f3_trace_nv_efs_reg_rw_stat_enum_type status = F3_TRACE_NV_EFS_REG_RW_STAT_INVALID; 
  const f3_trace_nv_efs_subsys_data_type* entry_ptr = NULL;
  const f3_trace_nv_efs_reg_item_struct* item_reg_ptr;

  if ((entry_ptr = f3_trace_nv_efs_find_subsys_entry(item)) == NULL)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"F3_TRACE_NV_EFS_READ: subsystem not initialized for item:%d", item); 
  }
  else
  {
    item_reg_ptr = &entry_ptr->table_ptr[(int)(item - entry_ptr->item_start)];
    if (data_size != item_reg_ptr->size)
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"F3_TRACE_NV_EFS_READ: Destination buffer size incorrect");
    }
    else
    {
      /* Generate the absolute path for the file */
      f3_trace_nv_efs_reg_get_item_fname(file_name, sizeof(file_name), FALSE, 
                                     item_reg_ptr, entry_ptr->sz_pathname_prefix);
  
      /* Check if the file is present or not. */
      status = F3_TRACE_NV_EFS_REG_RW_STAT_OK;
      if (efs_get(file_name, data_ptr, data_size) == -1)
      {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"F3_TRACE_NV_EFS_READ: EFS read error. Returning error.");
        status = F3_TRACE_NV_EFS_REG_RW_STAT_RD_ERR;
      }
    }
  } /* if f3_trace_nv_efs_reg_items != NULL */
  return status;
}

/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

                            Public Functions
   
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/



/*==============================================================================
  FUNCTION
    f3_trace_nv_efs_init

  DESCRIPTION
    Performs initialization required for NV items in the EFS.
    Currently this means generating the config file for diag tools.
    Requires a table of the NV items that need to be input to the config file.

  RETURN VALUE
    TRUE  - Success
    FALSE - Error during initialization
==============================================================================*/
boolean f3_trace_nv_efs_init (void)
{
  boolean ret_val = TRUE;

  MSG(MSG_SSID_DIAG, MSG_LEGACY_MED, "F3_TRACE_NV_EFS: Registry Init Started"); 

  ret_val =  f3_trace_nv_efs_init_internal(F3_TRACE_NV_EFS_CONFIG, F3_TRACE_NV_EFS_CONFIG_MIN_ITEM,
                               F3_TRACE_NV_EFS_CONFIG_MAX_ITEM, f3_trace_nv_reg_items, "config");
  if (ret_val)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_MED,"ERR_NV_EFS: NV EFS init successful");
  }
  else
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"ERR_NV_EFS: NV EFS init failed");
  }

  return ret_val;

}

/*==============================================================================
  FUNCTION
    f3_trace_nv_efs_reg_item_read

  DESCRIPTION
    Reads the specified item from EFS2 special items file.
 
    The subsystem that the given item belongs to must have already been
    initialized, or this function will return F3_TRACE_NV_EFS_REG_RW_STAT_INVALID.
 
  RETURN VALUE
    F3_TRACE_NV_EFS_REG_RW_STAT_OK         - Success
    F3_TRACE_NV_EFS_REG_RW_STAT_RD_ERR     - Error during read operation. if file
                                         does not exist, returns an error
    F3_TRACE_NV_EFS_REG_RW_STAT_INVALID    - Invalid paramters
==============================================================================*/
f3_trace_nv_efs_reg_rw_stat_enum_type f3_trace_nv_efs_reg_item_read(
  f3_trace_nv_efs_reg_item_type const item, 
  uint32 data_size,
  void *data_ptr
)
{
  f3_trace_nv_efs_reg_rw_stat_enum_type status = F3_TRACE_NV_EFS_REG_RW_STAT_INVALID;    

  MSG_1(MSG_SSID_DIAG, MSG_LEGACY_MED,"F3_TRACE_NV_EFS: NV EFS file read started for item:%d", item);

  status =  f3_trace_nv_efs_item_read_internal(item, data_size, data_ptr);

  if (status == F3_TRACE_NV_EFS_REG_RW_STAT_OK )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_MED,"F3_TRACE_NV_EFS: NV EFS file read succeeded for item = %d", item);
    return status;
  }
  else
  {
    return F3_TRACE_NV_EFS_REG_RW_STAT_RD_ERR;
  }
}


