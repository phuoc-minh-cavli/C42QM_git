
/*==============================================================================

                    F3 TRACE NV   R E G I S T R Y

  DESCRIPTION
 
    The F3 Trace NV Registry is a central location for configuration item storage.
    It serves as an interface to potentially various data sources. Currently its
    main data source is nonvolatile memory, via the EFS item interface.

  Copyright (c) 2010 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

#ifndef _F3TRACENVEFS_H
#define _F3TRACENVEFS_H


/*==============================================================================
  To add new items to the registry:
  1. Add entry to the end of existing enum f3_trace_nv_efs_reg_item_type. Don't insert,
     always append.
  2. Add entry in table <tech>_nv_efs.c 
     Make sure the order of the items is the same as in enum f3_trace_nv_efs_reg_item_type.
     Create a new table in the subsystem modules if a new subsystem is added.
  3. Add a default value in <tech>_nv_efs_get_default() in <tech>_nv_efs.c.
  4. Create a new enum in f3_trace_nv_efs_subsys_type for new subsystems and create
     new MIN and MAX items for the subsystem in f3_trace_nv_efs_reg_item_type.
  
==============================================================================*/

typedef enum
{
  /* ------------------------------------------------------------------
     F3 Trace items
     ------------------------------------------------------------------ */
  F3_TRACE_NV_EFS_CONFIG_MIN_ITEM = 0,   
  F3_TRACE_NV_EFS_F3_TRACE_CONTROL = F3_TRACE_NV_EFS_CONFIG_MIN_ITEM,
  F3_TRACE_NV_EFS_F3_TRACE_DETAIL,
  F3_TRACE_NV_EFS_F3_TRACE_MAX_FILES,
  /* add other CONFIG items here . make sure the max item is updated with the new value */
  F3_TRACE_NV_EFS_CONFIG_MAX_ITEM = F3_TRACE_NV_EFS_F3_TRACE_MAX_FILES,

  /*F3_TRACE_NV_EFS_other_MIN_ITEM,
    F3_TRACE_NV_EFS_F3_TRACE_foo = F3_TRACE_NV_EFS_other_MIN_ITEM,
    F3_TRACE_NV_EFS_F3_TRACE_bar,
    F3_TRACE_NV_EFS_other_MAX_ITEM = F3_TRACE_NV_EFS_F3_TRACE_bar,
  */

  F3_TRACE_NV_EFS_MAX_NUM_ITEMS /* This should be the last item. Add all entries above this line */
} f3_trace_nv_efs_reg_item_type;


/* default values */
#ifdef FEATURE_SAVE_TRACE_ON_BY_DEFAULT
  #if defined(ERR_F3_TRACE_CONTROL_MASK_DEFAULT_VAL) || defined(ERR_F3_TRACE_DETAIL_MASK_DEFAULT_VAL)
    #error ERR_F3_TRACE_* is deprecated, defaults should be configured in f3_trace_nv_efs.scons
  #endif
  #if defined(DIAG_DEBUG_CONTROL_MASK_DEFAULT_VAL) || defined (DIAG_DEBUG_DETAIL_MASK_DEFAULT_VAL)
    #error DIAG_DEBUG_* is deprecated, defaults should be configured in f3_trace_nv_efs.scons
  #endif
#else
/* Clear default values if FEATURE_SAVE_TRACE_ON_BY_DEFAULT is not enabled */
  #ifdef F3_TRACE_CONTROL_MASK_DEFAULT_VAL
    #undef F3_TRACE_CONTROL_MASK_DEFAULT_VAL
  #endif
  #define F3_TRACE_CONTROL_MASK_DEFAULT_VAL 0x0

  #ifdef F3_TRACE_DETAIL_MASK_DEFAULT_VAL
    #undef F3_TRACE_DETAIL_MASK_DEFAULT_VAL
  #endif
  #define F3_TRACE_DETAIL_MASK_DEFAULT_VAL 0x0

#endif


/* other structs and constants */

    #define F3_TRACE_NV_MAX_EFS_FILEPATH_SIZE 120 /* max file path size */
    #define F3_TRACE_NV_MAX_EFS_FILENAME_SIZE 60 /* max file name size */
    #define F3_TRACE_NV_EFS_CONF_FILE_PERM    0777

typedef enum
{
  F3_TRACE_NV_EFS_REG_OP_GET_ITEM,
  F3_TRACE_NV_EFS_REG_OP_SET_ITEM
}f3_trace_nv_efs_operation_type;

typedef enum
{
  F3_TRACE_NV_EFS_REG_TYPE_INTEGRAL,
  F3_TRACE_NV_EFS_REG_TYPE_FLOAT,
  F3_TRACE_NV_EFS_REG_TYPE_DOUBLE
  /* add new data types here if needed */
}f3_trace_nv_efs_data_type;

typedef enum
{
  F3_TRACE_NV_EFS_REG_RW_STAT_OK,
  F3_TRACE_NV_EFS_REG_RW_STAT_RD_ERR,
  F3_TRACE_NV_EFS_REG_RW_STAT_WR_ERR,
  F3_TRACE_NV_EFS_REG_RW_STAT_INVALID
} f3_trace_nv_efs_reg_rw_stat_enum_type;

typedef struct
{
  char file_name[F3_TRACE_NV_MAX_EFS_FILENAME_SIZE]; /* name of item file */
  uint32 size; /* size of item in bytes */
  uint8 precision_digits; /* number of digits of precision for float/double */
  f3_trace_nv_efs_data_type type_data; /* if integer/float/double */
} f3_trace_nv_efs_reg_item_struct;

typedef enum 
{
  F3_TRACE_NV_EFS_CONFIG,
  /* if you add a new type, make sure you upate the f3_trace_nv_efs_init() function with the
     appropriate config file name for your subsystem*/
  F3_TRACE_NV_EFS_INVALID,
  F3_TRACE_NV_EFS_SUBSYS_MAX = F3_TRACE_NV_EFS_INVALID
} f3_trace_nv_efs_subsys_type;

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
boolean f3_trace_nv_efs_init (void); 

/*==============================================================================
  FUNCTION
    f3_trace_nv_efs_reg_item_read

  DESCRIPTION
    Reads the specified item from EFS2 special items file.
 
    The subsystem that the given item belongs to must have already been
    initialized, or this function will return F3_TRACE_NV_EFS_REG_RW_STAT_INVALID.
 
    If the item EFS file is not found, the default value for the item is returned.
 
  RETURN VALUE
    F3_TRACE_NV_EFS_REG_RW_STAT_OK         - Success
    F3_TRACE_NV_EFS_REG_RW_STAT_RD_ERR     - Error during read operation. if file
                                         does not exist, returns an error
    F3_TRACE_NV_EFS_REG_RW_STAT_INVALID    - Invalid paramters
==============================================================================*/
f3_trace_nv_efs_reg_rw_stat_enum_type f3_trace_nv_efs_reg_item_read(
  f3_trace_nv_efs_reg_item_type const item_id, 
  uint32 data_size,  /* number of bytes of data in *data_ptr */
  void* data_ptr    /* source of data */
);


#endif /* _F3TRACENVEFS_H */
  

