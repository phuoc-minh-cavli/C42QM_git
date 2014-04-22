/*==============================================================================

EDIT HISTORY FOR MODULE

High Speed USB utility functions for achieving statistics on the
storage FD.

$Header: 
$DateTime: 

when      who     what, where, why
--------  ---     ------------------------------------------------------------


==============================================================================*/
#include <jusb_common_int.h>
#include <timetick.h>
#include "fd_storage_statistics.h"

/* statistics will be collected on transfers which are bigger or equal 
the value of hsu_num_of_blocks_per_xfer */
static juint32_t hsu_num_of_blocks_per_xfer = 128;

static juint32_t hsu_max_idle_time_between_xfer = 10000; /*10 msec*/

#define MAX_TIME_FOR_OPERATION 1000000 

static void update_disk_time_statistics(io_statistics *disk_mode, timetick_type cur_time);

/**
* Function type:  init_statistics
* Description:    initialize disk statistics data structure.   
* Requirement:    Mandatory
* Parameters:
*     @disk: (IN) read/write req. state 
*
* Return value:   None
**/
void init_statistics(io_statistics_type *disk)
{
  disk->busy.req_cnt = 0;
  disk->busy.min_time_usec = MAX_TIME_FOR_OPERATION;
  disk->busy.max_time_usec = 0;
  disk->busy.avr_time_usec = 0;
  disk->busy.total_time_usec = 0;
  disk->busy.start_time = 0;
  disk->busy.timer_timeout_cnt = 0;

  disk->idle.req_cnt = 0;
  disk->idle.min_time_usec = MAX_TIME_FOR_OPERATION;
  disk->idle.max_time_usec = 0;
  disk->idle.avr_time_usec = 0;
  disk->idle.total_time_usec = 0;
  disk->idle.start_time = 0;
  disk->idle.timer_timeout_cnt = 0;
} 

/**
* Function type:  update_statistics_for_disk_done
* Description:    update disk statistics data structure when the disk operation is done.   
* Requirement:    Mandatory
* Parameters:
*     @disk:          (IN) read/write req. state
*     @num_of_blocks: (IN) num_of_blocks of the read/write req. 
*
* Return value:   None
**/
void update_statistics_for_disk_done(io_statistics_type* disk, juint32_t num_of_blocks)
{
  timetick_type cur_time; 

  if (num_of_blocks < hsu_num_of_blocks_per_xfer)
  {
    return;
  }
  /* busy disk statistics */
  disk->busy.end_time = timetick_get();
  disk->idle.start_time = disk->busy.end_time;
  if (disk->busy.start_time > disk->busy.end_time )
  {
    disk->busy.turnaround_timeout_cnt++;
    return;
  }
  cur_time = timetick_diff (disk->busy.start_time, disk->busy.end_time, T_USEC);

  update_disk_time_statistics(&disk->busy, cur_time);
} 
/**
* Function type:  update_statistics_for_disk_job
* Description:    update disk statistics data structure when the disk operation starts.   
* Requirement:    Mandatory
* Parameters:
*     @disk:          (IN) read/write req. state
*     @num_of_blocks: (IN) num_of_blocks of the read/write req. 
*
* Return value:   None
**/
void update_statistics_for_disk_job(io_statistics_type* disk, juint32_t num_of_blocks)
{
  timetick_type cur_time = 0; 

  if (num_of_blocks < hsu_num_of_blocks_per_xfer)
  {
    return;
  }
   disk->busy.start_time = timetick_get();
  /* idle disk statistics */
  if (disk->idle.start_time == 0) /* disk_done() hasn't been called yet */
  {
    return;
  }
  disk->idle.end_time = disk->busy.start_time;
  if (disk->idle.start_time > disk->idle.end_time )
  {
    disk->idle.turnaround_timeout_cnt++;
    return;
  }
 
  cur_time = timetick_diff (disk->idle.start_time, disk->idle.end_time, T_USEC);


  if (cur_time > hsu_max_idle_time_between_xfer )
  {
    disk->idle.timer_timeout_cnt++;
  }
  update_disk_time_statistics(&disk->idle, cur_time);

} 
/**
* Function type:  update_statistics_for_usb_job
* Description:    update usb statistics data structure when the usb operation is done.   
* Requirement:    Mandatory
* Parameters:
*     @disk:          (IN) read/write req. state
*     @num_of_blocks: (IN) num_of_blocks of the read/write req. 
*
* Return value:   None
**/
void update_statistics_for_usb_job(io_statistics_type* disk, juint32_t num_of_blocks)
{

  if (num_of_blocks < hsu_num_of_blocks_per_xfer)
  {
    return;
  }
  disk->busy.start_time = timetick_get();
  
} 
/**
* Function type:  update_statistics_for_usb_done
* Description:    update usb statistics data structure when the usb operation is done.   
* Requirement:    Mandatory
* Parameters:
*     @disk:          (IN) read/write req. state
*     @num_of_blocks: (IN) num_of_blocks of the read/write req. 
*
* Return value:   None
**/
void update_statistics_for_usb_done(io_statistics_type* disk, juint32_t num_of_blocks)
{
  timetick_type cur_time; 

  if (num_of_blocks < hsu_num_of_blocks_per_xfer)
  {
    return;
  }
  /* busy disk statistics */
  disk->busy.end_time = timetick_get();
  if (disk->busy.start_time > disk->busy.end_time )
  {
    disk->busy.turnaround_timeout_cnt++;
    disk->idle.start_time = disk->busy.end_time;
    return;
  }
  cur_time = timetick_diff (disk->busy.start_time, disk->busy.end_time, T_USEC);

  update_disk_time_statistics(&disk->busy, cur_time);
 } 
/**
* Function type:  update_disk_time_statistics
* Description:    update disk statistics data structure when the disk operation starts.   
* Requirement:    Mandatory
* Parameters:
*     @disk_mode:     (IN) read/write req. state
*     @cur_time:      (IN) current measured time
*
* Return value:   None
**/
static void update_disk_time_statistics(io_statistics *disk_mode, timetick_type cur_time)
{
  /* disk_mode can be busy or idle */
  disk_mode->req_cnt++;
  disk_mode->total_time_usec += cur_time;
  disk_mode->max_time_usec = MAX(disk_mode->max_time_usec, cur_time);
  disk_mode->min_time_usec = MIN(disk_mode->min_time_usec, cur_time);
  disk_mode->avr_time_usec = disk_mode->total_time_usec / disk_mode->req_cnt;
}
