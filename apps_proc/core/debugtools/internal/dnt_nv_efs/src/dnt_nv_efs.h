
/*==============================================================================

                    DNT NV   R E G I S T R Y

  DESCRIPTION
 
    The DNT Registry is a central location for configuration item storage.
    It serves as an interface to potentially various data sources. Currently its
    main data source is nonvolatile memory, via the EFS item interface.

  Copyright (c) 2010 - 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

#ifndef _DNTNVEFS_H
#define _DNTNVEFS_H


/*==============================================================================
  To add new items to the registry:
  1. Add entry to the end of existing enum dnt_nv_efs_reg_item_type. Don't insert,
     always append.
  2. Add entry in table <tech>_nv_efs.c 
     Make sure the order of the items is the same as in enum dnt_nv_efs_reg_item_type.
     Create a new table in the subsystem modules if a new subsystem is added.
  3. Add a default value in <tech>_nv_efs_get_default() in <tech>_nv_efs.c.
  4. Create a new enum in dnt_nv_efs_subsys_type for new subsystems and create
     new MIN and MAX items for the subsystem in dnt_nv_efs_reg_item_type.
  
==============================================================================*/

typedef enum
{
  /* ------------------------------------------------------------------
     ERR items
     ------------------------------------------------------------------ */
  DNT_NV_EFS_ERR_MIN_ITEM = 0,
  DNT_NV_EFS_ERR_FATAL_ACTION = DNT_NV_EFS_ERR_MIN_ITEM, /*  */
  DNT_NV_EFS_ERR_MPROC_TIMEOUT_VALUE, /*  */
  DNT_NV_EFS_ERR_HW_RESET_DETECT_ENABLE, /*  */
  /* add other ERR items here . make sure the max item is updated with the new value */
  DNT_NV_EFS_ERR_MAX_ITEM = DNT_NV_EFS_ERR_HW_RESET_DETECT_ENABLE,

  DNT_NV_EFS_MAX_NUM_ITEMS /* This should be the last item. Add all entries above this line */
} dnt_nv_efs_reg_item_type;


/* other structs and constants */

    #define DNT_MAX_NV_EFS_FILEPATH_SIZE 120 /* max file path size */
    #define DNT_MAX_NV_EFS_FILENAME_SIZE 60 /* max file name size */
    #define DNT_NV_EFS_CONF_FILE_PERM    0777

typedef enum
{
  DNT_NV_EFS_REG_OP_GET_ITEM,
  DNT_NV_EFS_REG_OP_SET_ITEM
}dnt_nv_efs_operation_type;

typedef enum
{
  DNT_NV_EFS_REG_TYPE_INTEGRAL,
  DNT_NV_EFS_REG_TYPE_FLOAT,
  DNT_NV_EFS_REG_TYPE_DOUBLE
  /* add new data types here if needed */
}dnt_nv_efs_data_type;

typedef enum
{
  DNT_NV_EFS_REG_RW_STAT_OK,
  DNT_NV_EFS_REG_RW_STAT_RD_ERR,
  DNT_NV_EFS_REG_RW_STAT_WR_ERR,
  DNT_NV_EFS_REG_RW_STAT_INVALID
} dnt_nv_efs_reg_rw_stat_enum_type;

typedef struct
{
  char file_name[DNT_MAX_NV_EFS_FILENAME_SIZE]; /* name of item file */
  uint32 size; /* size of item in bytes */
  uint8 precision_digits; /* number of digits of precision for float/double */
  dnt_nv_efs_data_type type_data; /* if integer/float/double */
} dnt_nv_efs_reg_item_struct;

typedef enum 
{
  DNT_NV_EFS_ERR,
  /* if you add a new type, make sure you upate the dnt_nv_efs_conf_init() function with the
     appropriate config file name for your subsystem*/
  DNT_NV_EFS_INVALID,
  DNT_NV_EFS_SUBSYS_MAX = DNT_NV_EFS_INVALID
} dnt_nv_efs_subsys_type;

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
                         const dnt_nv_efs_reg_item_struct  *dnt_nv_efs_reg_items,
                         const char* psz_pathname_prefix
                         ); 

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
                         uint8 item_data
                         ); 

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
  dnt_nv_efs_reg_item_type const item_id, 
  uint32 data_size,  /* number of bytes of data in *data_ptr */
  void* data_ptr    /* source of data */
);


#endif /* _DNTNVEFS_H */
  

