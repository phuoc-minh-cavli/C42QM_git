
/*==============================================================================

                      D N T NV Item  R E G I S T R Y

  DESCRIPTION
    The DNT NV item Registry is a central location for configuration item storage.
    It serves as an interface to potentially various data sources. Currently its
    main data source is nonvolatile memory, via the EFS item interface.

  Copyright (c) 2010 - 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/debugtools/internal/dnt_nv_efs/src/dnt_nv_efs.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
04/03/14     psu     Update diag macros using optimized versions.
10/15/11     mcg     stealing code from cgps
05/03/10      gk      initial checkin
==============================================================================*/

#include "customer.h"
#include "comdef.h"

#include <math.h>
#include <float.h>
#include <stdio.h>

#include "fs_public.h"
#include "dnt_nv_efs.h"
#include "msg.h"
#include "assert.h"

/*===========================================================================
=============================================================================

   Private data structures and declarations
   
=============================================================================
===========================================================================*/
typedef struct
{
  dnt_nv_efs_reg_item_type item_start;
  dnt_nv_efs_reg_item_type item_end;
  char sz_pathname_prefix[10];
  const dnt_nv_efs_reg_item_struct* table_ptr;
} dnt_nv_efs_subsys_data_type;

static dnt_nv_efs_subsys_data_type dnt_nv_efs_subsys_table[DNT_NV_EFS_SUBSYS_MAX];

static const char* dnt_nv_efs_file_name[] =
{
  "dnt_err", /* DNT_NV_EFS_ERR */
  /* add new names above this line */
  "dnt_invalid"
};
static const char* dnt_nv_efs_file_ext=".conf";


/*IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

                            Private Functions
   
IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII*/
/*==============================================================================
  FUNCTION
    dnt_nv_efs_find_subsys_entry

  DESCRIPTION
    Helper function to find the subsystem table entry for the given NV item.
 
  RETURNS
    index into the table
  
==============================================================================*/
static const dnt_nv_efs_subsys_data_type* dnt_nv_efs_find_subsys_entry(
  dnt_nv_efs_reg_item_type item_id
)
{
  const dnt_nv_efs_subsys_data_type* entry_ptr = NULL;
  uint8 table_idx;
  for (table_idx = 0; 
       (table_idx < ARR_SIZE(dnt_nv_efs_subsys_table)) && (entry_ptr == NULL);
       table_idx++)
  {
    if ((item_id >= dnt_nv_efs_subsys_table[table_idx].item_start) &&
        (item_id <= dnt_nv_efs_subsys_table[table_idx].item_end) &&
        (NULL != dnt_nv_efs_subsys_table[table_idx].table_ptr))
    {
      /* found it */
      entry_ptr = &dnt_nv_efs_subsys_table[table_idx];
      break;
    }
  }
  return entry_ptr;
}

/*==============================================================================
  FUNCTION
    dnt_nv_efs_reg_get_item_fname

  DESCRIPTION
    This is a local function
    Constructs the full pathname of item file name and returns the length of
    the filename char array and file_name_size. file_name_size value will be 0 if error.
    No bound checks for the array are done here. Make sure the checks are done before
    passing it to this function. function is intended to be a used within this file. 
 
  RETURNS
  
==============================================================================*/
static uint8 dnt_nv_efs_reg_get_item_fname(
  char* file_name, 
  uint8 file_name_buf_size,
  const dnt_nv_efs_reg_item_struct *reg_items_ptr,
  const char* psz_pathname_prefix
)
{
  uint8 file_name_size = 0;
  /* item is already range checked in the calling function */
  file_name_size = (uint8)snprintf( file_name, 
                                    file_name_buf_size, 
                                    "/nv/item_files/dnt/%s/%s", 
                                    psz_pathname_prefix,
                                    reg_items_ptr->file_name );
  return file_name_size;
}

/*==============================================================================
  FUNCTION
    dnt_nv_efs_conf_init_internal

  DESCRIPTION
    Helper function for initialization required for NV items in the EFS.
    A config file is created and will hold the EFS file names of all NV items
    in the registry for the given subsystem.
 
  RETURN VALUE
    TRUE  - Success
    FALSE - Error during initialization
==============================================================================*/
static boolean dnt_nv_efs_conf_init_internal(
  dnt_nv_efs_subsys_type subsys, 
  dnt_nv_efs_reg_item_type table_start,
  dnt_nv_efs_reg_item_type table_end,
  const dnt_nv_efs_reg_item_struct* dnt_nv_efs_reg_items,
  const char* psz_pathname_prefix
)
{
  uint8 file_size = 0;
  boolean ret_val = TRUE; 
  char conf_dir_name_full[]="/nv/item_files/conf/";
  const char* conf_file_name = NULL;
  char conf_name[DNT_MAX_NV_EFS_FILEPATH_SIZE];
  char file_name[DNT_MAX_NV_EFS_FILEPATH_SIZE];
  char garbage_buf[DNT_MAX_NV_EFS_FILEPATH_SIZE];
  uint16 table_entry = table_start;


  MSG(MSG_SSID_TMS, MSG_LEGACY_MED,"DNT_NV_EFS: Registry Initialization"); 

  if (dnt_nv_efs_reg_items == NULL)
  {
    MSG(MSG_SSID_TMS, MSG_LEGACY_MED,"DNT_NV_EFS_REG: Null table pointer.");
    ret_val = FALSE;
  } /* if null table*/
  else if (table_start > table_end)
  {
    MSG_2(MSG_SSID_TMS, MSG_LEGACY_MED,"DNT_NV_EFS_REG: invalid range: %d, %d", table_start, table_end);
    ret_val = FALSE;
  }
  else
  {
    switch (subsys)
    {
      case DNT_NV_EFS_ERR:
        conf_file_name = dnt_nv_efs_file_name[subsys];
        break;
      default:
        MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,"DNT_NV_EFS: No config file available for this subsys %d", subsys); 
        ret_val = FALSE;
        break;
    }
    if (conf_file_name != NULL) 
    {
      while (table_entry <= table_end) 
      {
        /* build a .conf file for each entry to avoid buffering everything */
        (void)snprintf(conf_name, sizeof(conf_name), "%s%s%u%s", 
                       conf_dir_name_full, conf_file_name, (table_entry-table_start), dnt_nv_efs_file_ext);

        /* get full NV item file path*/
        file_size = dnt_nv_efs_reg_get_item_fname(
                        file_name, sizeof(file_name),
                        &dnt_nv_efs_reg_items[table_entry],
                        psz_pathname_prefix);

        // 'proper' asserts - safe to NOP in production code (here to appease KW)
        ASSERT(file_size<=sizeof(garbage_buf));
        ASSERT(file_size<=sizeof(file_name));

        // test if conf file already exists before writing (cr412055) 
        // note: EFS requires the size to be exact or we could optimize better
        if (-1 != efs_get(conf_name, garbage_buf, file_size))
        {
          MSG_LOW("DNT_NV_EFS: Skipping %u.conf file, already exists", (table_entry-table_start), 0, 0);
        }
        else
        {
          /* write it to EFS */
          if (efs_put(conf_name,
                      file_name,
                      file_size,
                      O_AUTODIR|O_CREAT|O_RDWR,
                      DNT_NV_EFS_CONF_FILE_PERM) < 0)
          {
            MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,"DNT_NV_EFS: Error writing %u.conf file", (table_entry-table_start));
            ret_val = FALSE;
          }
        }
        ++table_entry;
      }
    } /* ret_val = TRUE*/

    if (ret_val != FALSE)
    {
      /* bookkeeping */
      dnt_nv_efs_subsys_table[subsys].item_start = table_start;
      dnt_nv_efs_subsys_table[subsys].item_end = table_end;
      dnt_nv_efs_subsys_table[subsys].table_ptr = dnt_nv_efs_reg_items;
      strlcpy(dnt_nv_efs_subsys_table[subsys].sz_pathname_prefix,
              psz_pathname_prefix, 
              sizeof(dnt_nv_efs_subsys_table[subsys].sz_pathname_prefix));
    }
  } /* if dnt_nv_efs_reg_items != NULL */

  return  ret_val;
}

/*==============================================================================
  FUNCTION
    dnt_nv_efs_item_init_internal

  DESCRIPTION
    Helper function for initialization required for NV items in the EFS.
    A Item file is created and will hold the data for a NV item
 
  RETURN VALUE
    TRUE  - Success
    FALSE - Error during initialization
==============================================================================*/
static boolean dnt_nv_efs_item_init_internal(
  dnt_nv_efs_reg_item_type table_entry,
  const dnt_nv_efs_reg_item_struct* dnt_nv_efs_reg_items,
  const char* psz_pathname_prefix,
  uint8 item_data
)
{
  uint8 file_size = 0;
  boolean ret_val = TRUE; 
  char conf_dir_name_full[]="/nv/item_files/conf/";
  char file_name[DNT_MAX_NV_EFS_FILEPATH_SIZE];
  char garbage_buf[DNT_MAX_NV_EFS_FILEPATH_SIZE];

  MSG(MSG_SSID_TMS, MSG_LEGACY_MED,"DNT_NV_EFS: Item Registry Initialization"); 

  if (dnt_nv_efs_reg_items == NULL)
  {
    MSG(MSG_SSID_TMS, MSG_LEGACY_MED,"DNT_NV_EFS_REG: Null table pointer.");
    ret_val = FALSE;
  } /* if null table*/
  else
  {
    /* get full NV item file path*/
    file_size = dnt_nv_efs_reg_get_item_fname(
                    file_name, sizeof(file_name),
                    &dnt_nv_efs_reg_items[table_entry],
                    psz_pathname_prefix);

    // 'proper' asserts - safe to NOP in production code (here to appease KW)
    ASSERT(file_size<=sizeof(garbage_buf));
    ASSERT(file_size<=sizeof(file_name));


    // test if nv file already exists before writing (cr412055) 
    // note: EFS requires the size to be exact or we could optimize better
    if (-1 != efs_get(file_name, garbage_buf, sizeof(uint8)))
    {
      MSG_LOW("DNT_NV_EFS: Skipping %s file, already exists", file_name, 0, 0);
    }
    else
    {
      /* write it to EFS */
      if (efs_put(file_name,
                  &item_data,
                  sizeof(uint8),
                  O_AUTODIR|O_CREAT|O_RDWR,
                  DNT_NV_EFS_CONF_FILE_PERM) < 0)
      {
        MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,"DNT_NV_EFS: Error writing %s file", file_name);
        ret_val = FALSE;
      }
    }
  }

  return  ret_val;
}

/*==============================================================================
  FUNCTION
    dnt_nv_efs_item_read_internal
 
  DESCRIPTION
    Read the specified item from EFS2 special items file. If the
    item is not found, a default value is returned. The function logs the read
    write activity.
 
  RETURN VALUE
    DNT_NV_EFS_REG_RW_STAT_OK         - Success
    DNT_NV_EFS_REG_RW_STAT_RD_DEFAULT - Returning default value
    DNT_NV_EFS_REG_RW_STAT_RD_ERR     - Error during read operation
    DNT_NV_EFS_REG_RW_STAT_INVALID    - Invalid paramters
==============================================================================*/
static dnt_nv_efs_reg_rw_stat_enum_type dnt_nv_efs_item_read_internal(
  dnt_nv_efs_reg_item_type item,
  uint32 data_size,
  void *data_ptr
)
{
  char file_name[DNT_MAX_NV_EFS_FILEPATH_SIZE]; /* file name with absolute address */
  dnt_nv_efs_reg_rw_stat_enum_type status = DNT_NV_EFS_REG_RW_STAT_INVALID; 
  const dnt_nv_efs_subsys_data_type* entry_ptr = NULL;
  const dnt_nv_efs_reg_item_struct* item_reg_ptr;

  if ((entry_ptr = dnt_nv_efs_find_subsys_entry(item)) == NULL)
  {
    MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,"DNT_NV_EFS_READ: subsystem not initialized for item:%d", item); 
  }
  else
  {
    item_reg_ptr = &entry_ptr->table_ptr[(int)(item - entry_ptr->item_start)];
    if (data_size != item_reg_ptr->size)
    {
      MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR,"DNT_NV_EFS_READ: Destination buffer size incorrect");
    }
    else
    {
      /* Generate the absolute path for the file */
      dnt_nv_efs_reg_get_item_fname(file_name, sizeof(file_name),
                                     item_reg_ptr, entry_ptr->sz_pathname_prefix);
  
      /* Check if the file is present or not. */
      if (efs_get(file_name, data_ptr, data_size) == -1)
      {
        MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR,"DNT_NV_EFS_READ: EFS read error. Returning error.");
        status = DNT_NV_EFS_REG_RW_STAT_RD_ERR;
      }
      else
      {
        MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR,"DNT_NV_EFS_READ: EFS read success.");
        status = DNT_NV_EFS_REG_RW_STAT_OK;
      }
    }
  } /* if dnt_nv_efs_reg_items != NULL */
  return status;
}

/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

                            Public Functions
   
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/



/*==============================================================================
  FUNCTION
    dnt_nv_efs_conf_init

  DESCRIPTION
    Performs initialization required for NV items in the EFS.
    Currently this means generating the config file for diag tools.
    Requires a table of the NV items that need to be input to the config file.

  RETURN VALUE
    TRUE  - Success
    FALSE - Error during initialization
==============================================================================*/
boolean dnt_nv_efs_conf_init (dnt_nv_efs_subsys_type subsys, 
                         dnt_nv_efs_reg_item_type table_start,
                         dnt_nv_efs_reg_item_type table_end,
                         const dnt_nv_efs_reg_item_struct* dnt_nv_efs_reg_items,
                         const char* psz_pathname_prefix)
{
  MSG_1(MSG_SSID_TMS, MSG_LEGACY_MED,"DNT_NV_EFS: Registry Init Started for subsystem:%d", subsys); 
  return (dnt_nv_efs_conf_init_internal(subsys, table_start, table_end,
                                    dnt_nv_efs_reg_items, 
                                    psz_pathname_prefix));
}

/*==============================================================================
  FUNCTION
    dnt_nv_efs_item_init

  DESCRIPTION
    Performs initialization required for NV items in the EFS.
    Currently this means generating the item files mentioned in conf files.

  RETURN VALUE
    TRUE  - Success
    FALSE - Error during initialization
==============================================================================*/
boolean dnt_nv_efs_item_init (dnt_nv_efs_reg_item_type table_entry,
                         const dnt_nv_efs_reg_item_struct  *dnt_nv_efs_reg_items,
                         const char* psz_pathname_prefix,
                         uint8 item_data)
{
  MSG_1(MSG_SSID_TMS, MSG_LEGACY_MED,"DNT_NV_EFS: Item Init Started for table_entry:%u",(uint32)table_entry); 
  return (dnt_nv_efs_item_init_internal(table_entry,
                                    dnt_nv_efs_reg_items, 
                                    psz_pathname_prefix,item_data));
}

/*==============================================================================
  FUNCTION
    dnt_nv_efs_reg_item_read

  DESCRIPTION
    Reads the specified item from EFS2 special items file.
 
    The subsystem that the given item belongs to must have already been
    initialized, or this function will return DNT_NV_EFS_REG_RW_STAT_INVALID.
 
    If the item EFS file is not found, the default value for the item is returned.
 
  RETURN VALUE
    DNT_NV_EFS_REG_RW_STAT_OK         - Success
    DNT_NV_EFS_REG_RW_STAT_RD_ERR     - Error during read operation. if file
                                         does not exist, returns an error
    DNT_NV_EFS_REG_RW_STAT_INVALID    - Invalid paramters
==============================================================================*/
dnt_nv_efs_reg_rw_stat_enum_type dnt_nv_efs_reg_item_read(
  dnt_nv_efs_reg_item_type const item, 
  uint32 data_size,
  void *data_ptr
)
{
  MSG_1(MSG_SSID_TMS, MSG_LEGACY_MED,"DNT_NV_EFS: Registry Item Write Started for item:%d", item); 
  return (dnt_nv_efs_item_read_internal(item, data_size, data_ptr));
}


