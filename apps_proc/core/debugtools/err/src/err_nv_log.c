/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     N V   L O G G I N G S E R V I C E S

GENERAL DESCRIPTION
  This file contains code related to the error log stored in the NV.
  Functions allow the caller to retrieve and store error records from
  NV and to obtain certain error status.  The error log is stored in NV
  but is maintained in a RAM based cache to allow direct read and status
  capabilities.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  err_init must be called to initialize the cache from the NV.

Copyright (c) 2009-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.tx/6.0/debugtools/err/src/err_nv_log.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
06/30/09   tbg     FIle created (moved functions from err.c)

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "err.h"
#include "errlog.h"
#include "erri.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "stringl.h"

/* The threshold of number of errors of a unique type which are allowed    */
/* in order to force reset.  Once this number is exceeded on any reported  */
/* error we force a powerdown.  This ensures that we don't recycle power   */
/* forever (and waste NVM write cycles) for an error which occurs with     */
/* alarming regularity.                                                    */

#define ERR_NV_WRITE_THRESHOLD  20

/* Maintain count of unique errors logged.  Note that this is NOT the      */
/* total errors occuring; it is the total number of unique errors in the   */
/* log, since the occurance of an error already in the error log only      */
/* bumps up the error count of that error.                                 */

LOCAL unsigned int err_logged_count = 0;

/* Maintain count of total errors ignored because error log was full.      */

LOCAL unsigned int err_ignored_count = 0;

/* Local error log cache.                                                  */

LOCAL err_log_type err_cache[ERR_MAX_LOG];

/* Specify if error should be logged to the filesystem.
*/
LOCAL boolean err_log_to_fs = TRUE;

/* MUTEX OPERATION */
static qurt_mutex_t err_nv_mutex_obj;
#define ERR_NV_LOCK_INIT() qurt_mutex_init(&err_nv_mutex_obj)
#define ERR_NV_LOCK()  qurt_mutex_lock(&err_nv_mutex_obj)
#define ERR_NV_FREE()  qurt_mutex_unlock(&err_nv_mutex_obj)

PACKED void * err_read_diagpkt (PACKED void *req_pkt_ptr, uint16 pkt_len );
PACKED void * err_clear_diagpkt (PACKED void *req_pkt_ptr, uint16 pkt_len );

/* Specify if error services have been initialized.  Until they are        */
/* initialized any fatal error causes a reset without logging to NV.       */

LOCAL boolean err_online = FALSE;


void err_nv_log_init ( void )
{
  int i;

  /* init the mutex object */
  ERR_NV_LOCK_INIT();

  for (i = 0; i < ERR_MAX_LOG; i++) {
    err_cache[i].address = i;
    err_cache[i].err_count = 0;
  }
  /* We have been initailized and are online from now on. */
  err_online = TRUE;

}


/*===========================================================================

FUNCTION ERR_GET_LOG

DESCRIPTION
  Copy to the caller's pointer a specific error record in the error
  log cache.  The error record is identified by its number, from 0 to
  ERR_MAX_LOG.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place and ERR_NONE_S completion status is returned.

RETURN VALUE
  ERR_DONE_S    - Requested error log existed.
  ERR_NONE_S    - No such log number or operation could not be performed.

SIDE EFFECTS
  None.

===========================================================================*/

err_stat_type err_get_log_ex
(
  byte         log_num,  /* The error log number */
  err_log_type *log_ptr  /* Where to copy the log */
)
{

  /* If the log is within range and if we have initialized then  */
  /* copy the log to the caller's buffer and return call status. */

  if ((err_online) && (log_num < ERR_MAX_LOG)) {
    *log_ptr = err_cache[log_num];
    return (ERR_DONE_S);
  } else {
    return (ERR_NONE_S);
  }
} /* err_get_log */


/*===========================================================================

FUNCTION ERR_CLR_LOG

DESCRIPTION
  This function clears a specific error log both in cache and in NV.
  An error log is cleared simply by clearing the error count field
  and fatal-flag.

  If 'log_num' == 0xFF, all logs will be cleared.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place and ERR_NONE_S completion status is returned.

RETURN VALUE
  ERR_DONE_S    - Requested error record existed and cleared, or the
                  error did not exists (but OK status is still returned).
  ERR_NONE_S    - Error record out of range or clear to NV has failed.

SIDE EFFECTS
  The specified error log is cleared in NV and in cache and is now
  available to accept a new unique error.

===========================================================================*/

err_stat_type err_clr_log (
  byte  log_num
)
{
  unsigned int idx = 0;
  err_stat_type  status;    /* Function return status */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (log_num == 0xFF) {
    /* Clear all logs */

    status = ERR_DONE_S;

    /* Recursively clear all error logs */
    for(idx = 0; idx < ERR_MAX_LOG; idx++) {
      if (err_clr_log((byte)idx) == ERR_NONE_S) {
        /* Report failure even if only one fails.
        */
        status = ERR_NONE_S;
      }
    }
  } else {

    /* If we are initialized and in range then check the error */
    /* record.  If it is not empty then clear the error record */
    /* cache and NV, decrement error count and return status.  */

    status = ERR_NONE_S;
    if (err_online && err_log_to_fs && (log_num < ERR_MAX_LOG)) {

      status = ERR_DONE_S;
      if (err_cache[log_num].err_count != 0) 
      {

        err_cache[log_num].err_count   = 0;
        (void) memset((void *)err_cache[log_num].file_name,
                      0, ERR_FILE_NAME_LEN);
        err_cache[log_num].line_num    = 0;
        err_cache[log_num].fatal       = FALSE;

      }
    }
  }

  return status;
} /* err_clr_log */


/*===========================================================================

FUNCTION ERR_LOG_STORE

DESCRIPTION
  Store a new error record to cache and NV.  If there is room for the
  new record then it is stored and the error count incremented.  If
  there is no room then the record is discarded and the errors ignored
  count is incremented.  If an error log of exact same type (file code
  and line number) already exists then we just increment the error count
  of that log.

DEPENDENCIES
  This internal function assumes that the caller has checked that we
  have been initialized before it is called.

RETURN VALUE
  The return value allows the caller to determine what action should be
  taken after trying to log the error.  The returned value is a boolean:
    TRUE  - This error record was created or incremented and is below
            threshold.
    FALSE - This error record was not created nor incremented, or it has
            been incremented but it had now exceeded threshold.

SIDE EFFECTS
  NV and cache are updated if there was room for new record or if this
  was an existing record whose error count could be incremented.

===========================================================================*/
LOCAL boolean err_log_store (
  word        line,              /* Comes from __LINE__ */
  const char *file_ptr,          /* Comes from __FILE__ */
  boolean     fatal              /* TRUE if fatal error */
)
{
  boolean   found;              /* Error record found flag */
  boolean   wrote;              /* NV written OK flag */
  boolean   ignore;             /* Error record ignore flag */
  word      i, index;           /* Working variables */
  word      non_fatal_index;    /* First non_fatal log in buffer */
  byte      file_buff[ERR_FILE_NAME_LEN];  /* File name buffer */
  uint16    length;
  int16     start_idx;
 
  memset(file_buff,0,ERR_FILE_NAME_LEN);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy file name to local buffer, strip off the */
  /* file extension and replace it with nulls.     */

  /* NOTE on file_buff[] -- As defined in the ICD, an "Error Record Retrieval
   * Response Message" contains an 8 byte field for the file name.  This is 
   * NOT described as a NULL terminated string.  It is just an eight byte
   * field, where long file names are "truncated to the first eight
   * characters".  As such, file_buff[] is not necessarrily NULL terminated.
   *
   * HOWEVER, as there is some external code (nv) that treats this as a string
   * and introduces a null characters at the end, file_buff[] will be
   * NULL terminated when the NV is being used (for now) so that the file name
   * comparisons are correct.
   */

  /* First work backwards to determine where the path starts (if it is present)
   * As there are only 8 chars to work with we want to leave out any path info.
   */
  length=strlen(file_ptr);
  for (start_idx=length-1; start_idx>=0; start_idx--)
  {
    if ((file_ptr[start_idx] == '\\') || (file_ptr[start_idx] == '/'))
    {
      start_idx++; /* Increment to previous character */
      break;
    }
  }

  if(start_idx<0)
  {
    /* No path found, start at 0 */
    start_idx=0;
  }

  /* Now copy the file name */
  for (i = 0; (i < ERR_FILE_NAME_LEN)
           && (start_idx < length)
           && (file_ptr[start_idx] != '\0')
           && (file_ptr[start_idx] != '.'); i++, start_idx++)
  {
    file_buff[i] = (byte) file_ptr[start_idx];
  }

  /* Fill remaining buffer with NULL chars */
  for (; i < ERR_FILE_NAME_LEN; i++) {
    file_buff[i] = '\0';
  }

  /* If there is an existing error record with the same file name    */
  /* and line number then increment the error count of that record.  */
  /* Increment only up to saturation point (255).  Note also that    */
  /* we increment even if we pass ERR_NV_WRITE_THRESHOLD as we want  */
  /* to track number of times error has occured for this error type. */

  index           = 0x00FF;
  non_fatal_index = 0x00FF;
  found           = FALSE;
  ignore          = FALSE;

  ERR_NV_LOCK();

  for (i = 0; i < ERR_MAX_LOG; i++) {

    /* If we find an empty log, record its position so we don't have to
       loop again later. */

    if (non_fatal_index == 0x00FF && err_cache[i].fatal == FALSE)
      non_fatal_index = i; /* In case we need to overwrite a non-fatal*/

    if (err_cache[i].err_count == 0) {
      if (index == 0x00FF)
        index = i;     /* Save first open entry for new log */

    }
    else if ((err_cache[i].line_num == line) &&
             (strncmp((const char *)err_cache[i].file_name,
                      (const char *)file_buff, ERR_FILE_NAME_LEN) == 0)) {
      if (err_cache[i].err_count == 255) {
        ignore = TRUE;
      } else {
        err_cache[i].err_count++;
      }
      found = TRUE;
      index = i;
      break;
    }
  }

  /* Otherwise find the first empty error record, update all the   */
  /* record entries, and increment the unique errors logged count. */
  /* If this is a fatal error, and there are no empty logs,        */
  /* overwrite a non-fatal log                                     */

  if (!found) {

    /* If fatal and no open logs, overwrite a non-fatal log */
    if ((index != 0x00FF) || (fatal && non_fatal_index != 0x00FF)) {

      /* If replacing a non-fatal err - set index */
      if (index == 0x00FF) index = non_fatal_index;

      /* Check to make sure that we are not accessing out of bound */
      if (index < ERR_MAX_LOG)
      {
        (void) memscpy((void *)err_cache[index].file_name,
                      ERR_FILE_NAME_LEN,
                      (const void *)file_buff, ERR_FILE_NAME_LEN);

        err_cache[index].line_num = line;
        err_cache[index].fatal = fatal;
        err_cache[index].err_count = 1;
        err_logged_count++;
        found = TRUE;
      }
      else
        found = FALSE;
    }
  }
  ERR_NV_FREE();

  /* If we found the error in the log or if we found room  */
  /* for new error then write the record to NV.  If write  */
  /* failed then increment errors ignored error count and  */
  /* set status.  If there is no empty record then we just */
  /* throw away this error and we increment the count of   */
  /* errors ignored.                                       */

  wrote = TRUE;
  if ((found) && (!(ignore))) {
  } else {
    INC_SAT(err_ignored_count);
  }

  /* If we found the error record or created a new one and if we wrote   */
  /* the error record successfully to NV and if the error count is less  */
  /* then the threshold, then we return TRUE.  If we could not find room */
  /* for the new record or if NV write failed or if the current record   */
  /* error count exceeds the threshold then we return FALSE.             */

  if ((found) && (wrote) && (index < ERR_MAX_LOG) &&
      (err_cache[index].err_count <= ERR_NV_WRITE_THRESHOLD)) {
    return (TRUE);
  } else {
    return (FALSE);
  }
} /* err_log_store */


/*===========================================================================

FUNCTION ERR_PUT_LOG

DESCRIPTION
  This function is invoked from the ERR(...) macro.  It then calls
  err_log_store to store the error.  Since this is a non fatal error
  we do not bother to check if the error could be logged or not.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place.

RETURN VALUE
  None.

SIDE EFFECTS
  Attempt will be made to write an error record to NV and cache.

===========================================================================*/

void err_put_log (
  word   line,                /* from __LINE__ */
  const char   *file_ptr      /* From __FILE__ */
)
{
  if (err_online && err_log_to_fs) {
    (void) err_log_store(line, file_ptr, FALSE);
  }
}


/*===========================================================================

FUNCTION ERR_ENABLE_FS_LOG
DESCRIPTION
  Enables logging of errors to the filesystem. Logging of errors is enabled
  by default.

============================================================================*/
void err_enable_fs_log (void)
{
  err_log_to_fs = TRUE;
} /* err_enable_fs_log */


/*===========================================================================

FUNCTION ERR_DISABLE_FS_LOG
DESCRIPTION
  Disables logging of errors to the filesystem. Logging of errors is enabled
  by default.

============================================================================*/
void err_disable_fs_log (void)
{
  err_log_to_fs = FALSE;
} /* err_disable_fs_log */


