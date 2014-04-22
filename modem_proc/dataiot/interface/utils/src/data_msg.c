/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
               DATA_MSG. C
           
GENERAL DESCRIPTION
  Utility functions for DS module

Copyright (c) 2018 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "data_msg.h"
#include "assert.h"


/*===========================================================================

FUNCTION DS_ERR_FATAL_WRAPPER

DESCRIPTION
  Wrapper function for ERROR_FATALS
 
DEPENDENCIES 
  None

RETURN VALUE
  None
 
SIDE EFFECTS

===========================================================================*/
void ds_err_fatal_wrapper
(  
  const char      *file_name,
  unsigned int     line_num,
  const char       *str
)
{ 

  /* Print the file name and line number before calling ERR_FATAL */
  if (NULL != file_name) 
  {
    DS_MSG_SPRINTF_2(MSG_LEGACY_HIGH, "%s line:%d: ",
                          file_name, line_num);
  }
      
  if (NULL == str)
  {
    DS_ERROR_FATAL("Error Fatal encountered");
    return;
  }
    
  DS_ERR_FATAL(str);

}/* ds__err_fatal_wrapper */

/*===========================================================================

FUNCTION DS_ASSERT_WRAPPER

DESCRIPTION
  Wrapper function for DS_ASSERTION
 
DEPENDENCIES 
  None

RETURN VALUE
  None
 
SIDE EFFECTS

===========================================================================*/
void ds_assert_wrapper
(
  const char      *file_name,
  unsigned int     line_num,
  int              expression
)
{
  /* Print the file name and line number before calling ERR_FATAL */
  if ((NULL != file_name) && (!expression))
  {
    DS_MSG_SPRINTF_2(MSG_LEGACY_HIGH, "%s line:%d: ",
                            file_name, line_num);
  }

  ASSERT(expression);
}/* ds_assert_wrapper */

/*===========================================================================

FUNCTION DS_ERROR_LOG_0_WRAPPER

DESCRIPTION
  Wrapper function for reporting error messages
 
DEPENDENCIES 

RETURN VALUE
 
SIDE EFFECTS

===========================================================================*/
void ds_error_log_0_wrapper
(
  const char*   str,
  unsigned int  line_num,  
  const char*   format
)
{
  if (NULL == str)
  {
    return;
  }

  DS_MSG_SPRINTF_3(MSG_LEGACY_ERROR,\
                  "%s line: %d: %s ", str, line_num, format);
  
}/* ds_error_log_0_wrapper */

/*===========================================================================

FUNCTION DS_ERROR_LOG_1_WRAPPER

DESCRIPTION
  Wrapper function for reporting error messages
 
DEPENDENCIES 

RETURN VALUE
 
SIDE EFFECTS

===========================================================================*/
void ds_error_log_1_wrapper
(
  const char*   str,
  unsigned int  line_num,  
  const char*   format,
  uint32        arg1
)
{
  if (NULL == str)
  {
    return;
  }
  DS_MSG_SPRINTF_4(MSG_LEGACY_ERROR,\
                  "%s line: %d: %s, arg1: %lu", str, line_num, format, arg1);  
}/* ds_error_log_1_wrapper */

/*===========================================================================

FUNCTION DS_ERROR_LOG_2_WRAPPER

DESCRIPTION
  Wrapper function for reporting error messages
 
DEPENDENCIES 

RETURN VALUE
 
SIDE EFFECTS

===========================================================================*/
void ds_error_log_2_wrapper
(
  const char*   str,
  unsigned int  line_num,  
  const char*   format,
  uint32        arg1,
  uint32        arg2
)
{

  if ((NULL == str) || (NULL == format))
  {
    return;
  }
  
  DS_MSG_SPRINTF_5( MSG_LEGACY_ERROR,      \
                  "%s line: %d: %s, arg1: %lu, arg2: %lu", \
                  str, line_num, format, arg1, arg2);    
} /* ds_error_log_2_wrapper */

/*===========================================================================

FUNCTION DS_ERROR_LOG_3_WRAPPER

DESCRIPTION
  Wrapper function for reporting error messages
 
DEPENDENCIES 

RETURN VALUE
 
SIDE EFFECTS

===========================================================================*/
void ds_error_log_3_wrapper
(
  const char*   str,
  unsigned int  line_num,  
  const char*   format,
  uint32        arg1,
  uint32        arg2,
  uint32        arg3
)
{
  if (NULL == str)
  {
    return;
  }

  DS_MSG_SPRINTF_6( MSG_LEGACY_ERROR,\
                    "%s line: %d: %s arg1: %lu, arg2: %lu, arg3 %lu", 
                    str, line_num, format, arg1, arg2, arg3);
} /* ds_error_log_3_wrapper */

/*===========================================================================

FUNCTION DS_ERROR_LOG_4_WRAPPER

DESCRIPTION
  Wrapper function for reporting error messages
 
DEPENDENCIES 

RETURN VALUE
 
SIDE EFFECTS

===========================================================================*/
void ds_error_log_4_wrapper
(
  const char*   str,
  unsigned int  line_num,  
  const char*   format,
  uint32        arg1,
  uint32        arg2,
  uint32        arg3,
  uint32        arg4
)
{
  if (NULL == str)
  {
    return;
  }
  DS_MSG_SPRINTF_7( MSG_LEGACY_ERROR,                             \
                  "%s line: %d: %s , arg1: %lu, arg2: %lu, arg3 %lu, arg4: %lu",  \
                  str, line_num, format, arg1, arg2, arg3, arg4);
}/* ds_error_log_4_wrapper */

