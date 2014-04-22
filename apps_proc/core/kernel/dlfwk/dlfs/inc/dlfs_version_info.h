/*==============================================================================

Dynamic Loading Support Libs

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

--------------------------------------------------------------------------------
 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when               who         what, where, why
 --------           ---         -----------------------------------------------
 1/22/15          amitkulk      created 
==============================================================================*/

#ifndef __SHLIB_VERSION_INFO_H__
#define __SHLIB_VERSION_INFO_H__

#include <stdlib.h> 
#include <stdio.h> 

/* Version control Info Type*/
typedef enum {
   VERSION_INFO_MAGIC,
   VERSION_INFO_TIMESTAMP,
   VERSION_INFO_UUID,
   VERSION_INFO_HEX_TOOL_REV,
   VERSION_INFO_UNDEFINED
}version_info_data_type;

/* Version control Info */
typedef struct version_info{
   version_info_data_type version_info_type ;
   union
   {
      int   version_data_int ;
      char  version_data_stream[50];
   }version_info_data;
}version_info;

/* Version control Info Table */
typedef struct version_info_symbol_table
{
   const char     *filename;
   version_info   *version_symbol_name;
}version_info_symbol_table;


//global table externed for version check of shared libraries
extern version_info_symbol_table version_info_symtab[];
extern int num_ver_info_symbols;

#endif
