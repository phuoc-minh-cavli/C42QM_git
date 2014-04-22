#ifndef ERR_TYPES_H
#define ERR_TYPES_H

/*===========================================================================

                    Error Handling Service Typedefs Header File

Description

Copyright (c) 2013,15,18 by Qualcomm Technologies Incorporated.  All Rights Reserved.

$Header: //components/rel/core.mpss/3.10/debugtools/err/src/err_types.h#1 $

===========================================================================*/

#include "errlog.h"

#define ERR_FILE_NAME_LEN 8
typedef struct
{
  byte address;         /* Storage address 0 to ERR_MAX_LOG-1 */
  byte err_count;       /* Number of occurances (0=empty,FF=full) */
  byte file_name[ERR_FILE_NAME_LEN];
  word line_num;
  boolean fatal;
} err_log_type;

#if defined (ERR_F3_TRACE_TO_SMEM)

typedef unsigned long long err_data_flag_type;

typedef struct {
  err_data_flag_type flag;
  err_log_type nv_log;
  unsigned long length;
  char   data[ERR_DATA_MAX_SIZE];
} err_data_log_type;

typedef struct {
  err_data_flag_type reset_flag;
  err_data_log_type err_log;
} err_data_type;

#endif /* ERR_F3_TRACE_TO_SMEM */

#endif /* ERR_TYPES_H */
