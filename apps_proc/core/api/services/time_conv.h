#ifndef TIME_CONV_H
#define TIME_CONV_H

/**
@file time_conv.h
@brief
This module implements the conversion of time into different units. 
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ATS_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ATS_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the time_services group 
      description in the ATS_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2009-2012, 2020 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements conversion of time into different units .


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/services/time_conv.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/08/12   cp      Edited to take care of 19.2MHz clk and 64bit native math.
11/29/10   scd     (Tech Pubs) Edited Doxygen markup and comments.
10/27/10   EBR     Doxygenated File
17/10/09   din     File created. 

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_types.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/* Frequency of Sclk used */
#define TIME_CONV_SCLK_FREQ            19200000
/* Number of ticks per ms */
#define TIME_CONV_SCLK_FREQ_MS         (TIME_CONV_SCLK_FREQ/1000)


/*-----------------------------------------------------------------------------
                      SYSTEM TIMESTAMP FORMAT


   |<------------ 48 bits --------------->|<----- 16 bits ------->|
   +--------------------------------------+-----------------------+
   |      1.25 ms counter                 |   1/32 chip counter   |
   +--------------------------------------+-----------------------+
         (11K years of dynamic range)          (1.25 ms dynamic
                                                range. Rolls over
                                                at count 49152)

-----------------------------------------------------------------------------*/
/** @addtogroup time_services
@{ */


/** 
Converts a timestamp from system time to milliseconds. 
 
@param[in,out] ts_val Time goes in as timestamp format. Time is returned in 
                      milliseconds via this variable. 
 
@return 
None. 
 
@dependencies 
None. 
*/ 
void time_conv_to_ms
(
  time_type                       ts_val
);


/**
Converts a timestamp from system time to seconds. 
 
@param[in] ts_val Time in timestamp format. 
 
@return 
Time in seconds.
 
@dependencies 
None.

@sideeffects
ts_val is destroyed after this function is finished.
*/
uint32 time_conv_to_sec
(
  time_type                       ts_val
);


/**
Converts from milliseconds to timestamp format. 
 
@param[in,out] ts_val Time goes in as milliseconds. Time is returned in 
                      timestamp format.
 
@return 
None. 
 
@dependencies 
None. 
*/
void time_conv_ms_to_timestamp
(
  time_type                       ts_val
);


/** 
Converts from seconds to timestamp format. 
 
@param[in,out] ts_val Time goes in as timestamp format. Time is returned in 
                      milliseconds via this variable.

@return
None.

@dependencies
None.

*/ 
void time_conv_secs_to_timestamp
(
  time_type                       ts_val
);


/**
Converts from timestamp format to 20-millisecond frame time format.

@param[in,out] ts_val Time goes in as timestamp format. Time is returned in 
                      20-millisecond frame time via this variable.

@return
None.

@dependencies
None.
*/
void time_conv_to_20ms_frame_time
(
  time_type                       ts_val
);


/**
Adds an offset to the specified timestamp.

@param[in,out] ts_val Timestamp to have value added onto. New value is returned 
                      via this variable.
@param[in] offset     Offset to be added. Offset is given in milliseconds.

@return
None.

@dependencies
None.
*/
void time_conv_add_offset_to_timestamp
(
  time_type                       ts_val,
  int64                           offset
);


/**
Increments a timestamp by a specified number of sclks.

@param[in,out]  timestamp_io    Timestamp to increment.
@param[in]      sclks           Number of sclks to increment 
                                timestamp.

@return
None.

@dependencies
None.
*/
void timetick_sclk64_add_sclks
(
  time_type                       timestamp_io,
    /* In: Base timestamp;  Out: Timestamp incremented by sclks */
  uint64                          sclks
    /* Number of slow clocks to advance timestamp by */
);


/**
Increments a timestamp by a specified number of XOs.

@param[in,out]  timestamp_io    Timestamp to increment.
@param[in]      xos             Number of XOs to increment 
                                timestamp.

@return
None.

@dependencies
None.
*/
void timetick_sclk64_add_xo
(
  time_type                       timestamp_io,
    /* In: Base timestamp;  Out: Timestamp incremented by sclks */
  uint64                          xos
    /* Number of XOs to advance timestamp by */
);


/** @} */ /* end_addtogroup time_services */

#endif /* TIME_CONV_H */





