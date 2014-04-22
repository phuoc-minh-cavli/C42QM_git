/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
High Speed USB utility functions for achieving statistics on the
storage FD.
  
  $Header: 
  $DateTime: 
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  
    
==============================================================================*/
#ifndef _FD_STORAGE_STATISTICS_H_
#define _FD_STORAGE_STATISTICS_H_

#include "jtypes_int.h"

#ifndef timetick_type
  #define uint32 timetick_type
#endif

/* The following statistic data structures are relevant for SDCC transfers only. */
typedef struct {
  juint32_t req_cnt;
  /* time is measured in usec*/
  juint32_t total_time_usec;
  juint32_t min_time_usec;
  juint32_t max_time_usec;
  juint32_t avr_time_usec;
  timetick_type start_time;
  timetick_type end_time;
  juint32_t timer_timeout_cnt;
  juint32_t turnaround_timeout_cnt;
} io_statistics;

typedef struct {
  io_statistics busy;
  io_statistics idle;
} io_statistics_type;

typedef struct {
  io_statistics_type write;
  io_statistics_type read;
} io_disk_statistics_type;

/**
* Function type:  init_statistics
* Description:    initialize disk statistics data structure.   
* Requirement:    Mandatory
* Parameters:
*     @disk: (IN) read/write req. state 
*
* Return value:   None
**/
void init_statistics(io_statistics_type *disk);
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
void update_statistics_for_disk_done(io_statistics_type* disk, juint32_t num_of_blocks);
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
void update_statistics_for_disk_job(io_statistics_type* disk, juint32_t num_of_blocks);
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
void update_statistics_for_usb_job(io_statistics_type* disk, juint32_t num_of_blocks);
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
void update_statistics_for_usb_done(io_statistics_type* disk, juint32_t num_of_blocks);

#endif /* _FD_STORAGE_STATISTICS_H_ */

