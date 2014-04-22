/*===========================================================================
                         PSM_APP_COMMON.C

DESCRIPTION
   File contains the utilities for the PSM demo application 
  
   Copyright (c) 2018 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
#include "psm_app_common.h"

/*=========================================================================
  FUNCTION:  psm_dam_format_log_msg

===========================================================================*/
/*!
    @brief
    Format debug message for logging.

    @return
    None
*/
/*=========================================================================*/
void psm_dam_format_log_msg
(
  char *buf_ptr,
  int buf_size,
  char *fmt,
  ...
)
{
  va_list ap;

  /*-----------------------------------------------------------------------*/

  /* validate input param */
  if( NULL == buf_ptr || buf_size <= 0)
  {
    LOG_ERROR("Bad Param buf_ptr:[%p], buf_size:%d",buf_ptr, buf_size);
    return;
  }

  /*-----------------------------------------------------------------------*/

  va_start( ap, fmt );

  vsnprintf( buf_ptr, (size_t)buf_size, fmt, ap );

  va_end( ap );

} /* psm_dam_format_log_msg */

UINT memscpy
(
  void   *dst,
  UINT   dst_size,
  const  void  *src,
  UINT   src_size
)
{
  UINT  copy_size = (dst_size <= src_size)? dst_size : src_size;
  memcpy(dst, src, copy_size);
  return copy_size;
}

