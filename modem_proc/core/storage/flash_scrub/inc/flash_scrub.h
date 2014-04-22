/***********************************************************************
 * flash_scrub.h
 *
 * Flash-Scrub module.
 * Copyright (C) 2013, 2019 QUALCOMM Technologies, Inc.
 *
 * Flash-Scrub module APIs.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.10/storage/flash_scrub/inc/flash_scrub.h#2 $ $DateTime: 2019/06/19 10:03:46 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2019-04-08   as    Allow disabling SCRUB feature at runtime.
2013-10-03   dks   Handle Fota update message from apps.
2013-09-05   dks   Fix accuracy of scrub timer when no system time available.
2013-08-29   rp    Recompute MD5 due to FOTA.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-14   rp    Include "scrub_config_i.h" before any other includes.
2013-08-12   rp    Add APIs to change scrub timing values.
2013-08-01   rp    Create

===========================================================================*/

#ifndef __FLASH_SCRUB_H__
#define __FLASH_SCRUB_H__

#include "comdef.h"


/*---------------------------------------------------------------------------
 * Status of Flash-Scrub.
---------------------------------------------------------------------------*/
enum flash_scrub_status
{
  FLASH_SCRUB_STATUS_PENDING    = 1,
  FLASH_SCRUB_STATUS_COMPLETE   = 2,
  FLASH_SCRUB_STATUS_UNKNOWN    = 3,
};


/**----------------------------------------------------------------------------
 * Requests the Scrub-Module to start a SCRUB.
 *
 * This function will do a time-check to see if we are past minimum time check
 * needed between two scrubs and queue a scrub-request to the scrub-task.
 *
 * @param scrub_token
 *    On success a token for the currently queued up scrub request. This token
 *    can be used to query the status of the queued up scrub request.
 *
 * @return
 *    Returns 0 on success on -1 on failure.
----------------------------------------------------------------------------**/
int flash_scrub_start_scrub_request (uint32 *scrub_token);


/**----------------------------------------------------------------------------
 * Requests the Scrub-Module to start a SCRUB. This request from a QMI command.
 *
 * This function will do a time-check to see if we are past minimum time check
 * needed between two scrubs and queue a scrub-request to the scrub-task.
 *
 * @param scrub_token
 *    On success a token for the currently queued up scrub request. This token
 *    can be used to query the status of the queued up scrub.
 *
 * @return
 *    Returns 0 on success on -1 on failure.
----------------------------------------------------------------------------**/
int flash_scrub_start_scrub_request_from_qmi (uint32 *scrub_token);


/**----------------------------------------------------------------------------
 * Requests status of a previously queued up scrub request.
 *
 * This function can be used to query the status of a previously queued up
 * scrub using the scrub-token obtained at the time of queuing.
 *
 * @param scrub_token
 *    Scrub token received at the time of queuing scrub request.
 *
 * @param scrub_status
 *    Status of scrub.
 *      FLASH_SCRUB_STATUS_PENDING   : Scrub is in progress.
 *      FLASH_SCRUB_STATUS_COMPLETE  : Scrub is complete.
 *      FLASH_SCRUB_STATUS_UNKNOWN   : Unknown scrub-token.
 *
 * @return
 *    Returns 0 on success on -1 on failure.
----------------------------------------------------------------------------**/
int flash_scrub_get_scrub_status (uint32 scrub_token,
                                  enum flash_scrub_status *scrub_status);


/**----------------------------------------------------------------------------
 * Requests status of a previously queued up scrub request. This request is
 * from a QMI command.
 *
 * This function can be used to query the status of a previously queued up
 * scrub using the scrub-token obtained at the time of queuing.
 *
 * @param scrub_token
 *    Scrub token received at the time of queuing scrub request.
 *
 * @param scrub_status
 *    Status of scrub.
 *      FLASH_SCRUB_STATUS_PENDING   : Scrub is in progress.
 *      FLASH_SCRUB_STATUS_COMPLETE  : Scrub is complete.
 *      FLASH_SCRUB_STATUS_UNKNOWN   : Unknown scrub-token.
 *
 * @return
 *    Returns 0 on success on -1 on failure.
----------------------------------------------------------------------------**/
int flash_scrub_get_scrub_status_from_qmi (uint32 scrub_token,
                                      enum flash_scrub_status *scrub_status);


/**----------------------------------------------------------------------------
 * Requests last scrub-time and scrub-timings configuration parameters.
 *
 * @param last_scrub_time_in_secs
 *    Time of that last scrub.
 *
 * @param min_time_in_secs_between_2_scrubs
 *    Configured minimum time interval to enforce between 2 scrubs.
 *
 * @param automatic_scrub_timer_value_in_secs
 *    Configured automatic scrub timer value.
 *
 * @return
 *    Returns 0 on success on -1 on failure.
----------------------------------------------------------------------------**/
int flash_scrub_get_time_status (uint64 *last_scrub_time_in_secs,
                                 uint64 *min_time_in_secs_between_2_scrubs,
                                 uint64 *automatic_scrub_timer_value_in_secs);


/**----------------------------------------------------------------------------
 * Requests last scrub-time and scrub-timings configuration parameters. This
 * request is from a QMI command.
 *
 * @param last_scrub_time_in_secs
 *    Time of that last scrub in posix-time.
 *    POSIX(TM) time starts at Jan 1, 1970.
 *
 * @param min_time_in_secs_between_2_scrubs
 *    Configured minimum time interval to enforce between 2 scrubs.
 *
 * @param automatic_scrub_timer_value_in_secs
 *    Configured automatic scrub timer value.
 *
 * @return
 *    Returns 0 on success on -1 on failure.
----------------------------------------------------------------------------**/
int flash_scrub_get_time_status_from_qmi (uint64 *last_scrub_time_in_secs,
                             uint64 *min_time_in_secs_between_2_scrubs,
                             uint64 *automatic_scrub_timer_value_in_secs);


/**----------------------------------------------------------------------------
 * Forcefully queue a scrub request without enforcing any timing requirement
 * checks.
 *
 * @return
 *    Returns 0 on success on -1 on failure.
----------------------------------------------------------------------------**/
int flash_scrub_do_a_forced_scrub (void);


/**----------------------------------------------------------------------------
 * Change the automatic periodic scrub-timer value.
 *
 * @param automatic_scrub_timer_value_in_secs
 *    New automatic periodic scrub-timer values in seconds.
 *
 * @return
 *    Returns 0 on success on -ve on failure.
----------------------------------------------------------------------------**/
int flash_scrub_set_automatic_scrub_timer (
          uint64 automatic_scrub_timer_value_in_secs);

/**----------------------------------------------------------------------------
 * Change the timer value for saving time in EFS.
 *
 * @param timer_value_in_secs_for_saving_time_in_efs
 *    New timer value to save time in EFS.
----------------------------------------------------------------------------**/
int flash_scrub_set_timer_value_for_saving_time_in_efs (
          uint64 timer_value_in_secs_for_saving_time_in_efs);


/**----------------------------------------------------------------------------
 * Change the minimum time-interval value to enforce between 2 scrubs.
 *
 * @param min_time_in_secs_between_2_scrubs
 *    New minimum time-interval value to enforce between 2 scrubs.
 *
 * @return
 *    Returns 0 on success on -ve on failure.
----------------------------------------------------------------------------**/
int flash_scrub_set_min_time_between_2_scrubs (
          uint64 min_time_in_secs_between_2_scrubs);


/**----------------------------------------------------------------------------
 * Notify the flash-scrub module about FOTA update.
----------------------------------------------------------------------------**/
int flash_scrub_notify_fota_update (void);


/**----------------------------------------------------------------------------
 * Notify the flash-scrub module about FOTA request to disable scrubbing.
 *
 * @return
 *    Returns 0 on succcess and -ve on failure.
----------------------------------------------------------------------------**/
int flash_scrub_disable_scrub (void);

/**----------------------------------------------------------------------------
 * Notify the flash-scrub module about FOTA request to unlock scrub partition.
 * @param recompute_md5
 *    Notify the flash-scrub module to recompute MD5 of code partitions
 *
 * @return
 *    Returns one of the state as in enum partition_state_t.
----------------------------------------------------------------------------**/
int flash_scrub_enable_scrub (int recompute_md5);

#endif /* not __FLASH_SCRUB_H__ */
