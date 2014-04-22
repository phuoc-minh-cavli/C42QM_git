#ifndef __GL1LSMIF_H__
#define __GL1LSMIF_H__
/*==========================================================================

             L1 LSM Interface Header File

DESCRIPTION
  This file contains global declarations and external references
  for the GSM L1 -to- LSM (Location Services Manager) Interface

  Copyright (c) 2003-2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/geran.mpss/5.2.0/api/gl1lsmif.h#6 $  $DateTime: 2018/10/11 00:25:52 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
28/09/17   shm     CR2292513 Instantaneous support for WWAN timing and frequency information
01/10/18   ng      CR2325381 Move all GPS code under feature flag 'FEATURE_CGPS_TIME_TAGGING'
16/08/18   akm     CR2297635 Compilation error EXTPL flavor
07/08/18   shm     CR 2274892 | Instantaneous WWAN timing and frequency information (rude wakeup) - GL1 changes
10-04-01   tjw     Add customer.h and comdef.h to all SU API files
10-03-30   tjw     Explicitly include dependent headers to make SU API files
                   stand alone
02/03/07   cs      Add support for new CGPS timetag
04/11/05   gfr     Added support for sleep timetag
02/06/04   gfr     Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "nas_exp.h"

#include "sys.h"
#include "sys_type.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Message ids passed from LSM to L1 */
enum
{
  GL1_GTS_MSG_TIMETAG_REQUEST = 1
};

enum
{
	MS_GTS_GL1
};

typedef struct
{
   IMH_T            message_header;
} gl1_cgps_timetag_type;

/*===========================================================================

FUNCTION  gl1_TimeTagRequest

DESCRIPTION
  This function allows the CGPS task to request GL1 to perform a sleep timetag
  which can be forced to trigger a sleep tag request for rude wakeup.

===========================================================================*/

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
void gl1_TimeTagRequest(boolean allow_sleep_timetag, sys_modem_as_id_e_type as_id);
#else
void gl1_TimeTagRequest(boolean allow_sleep_timetag);
#endif

/* Message ids passed from LSM to L1 */
enum
{
  GL1_LSM_MSG_TIMETAG_REQUEST
};

typedef struct
{
   IMH_T            message_header;
   boolean          allow_sleep_timetag;
} gl1_lsm_timetag_type;


#ifdef FEATURE_CGPS_TIME_TAGGING
/*These parameters were used by Gl1 to populate msg sent to GPS module 'cgps_GsmCellInfoUpdate( msg )'
  This is no longer needed, as timing info is sent by GTS module */

/* Update types passed to LSM */
typedef enum
{
  GL1_TIMETAG_REQUEST,
  GL1_TIMETAG_PRE_CELL_CHANGE,
  GL1_TIMETAG_POST_CELL_CHANGE
} gl1_cell_update_e_type;


/*
 * From GSM/GPRS L1 -to- LSM
*/

/* This structure provides FN, time slot and symbol count for the reference cell */
typedef struct {

   /* Frame Number 0..(26L*51L*2048L-1)  ~4.615ms frames */
   uint32     fn;

   /* Time Slot Number 0..7 */
   uint8      time_slot;

   /* QSymbol Count (sub fn) */
   uint32     q_symbol_count;

} gl1_cell_time_type;


/* Cell Information <bsic,freq> */
typedef struct {

   /* Base Station Identification code */
   uint8 bsic;

   /* Control Channel Frequency */
   ARFCN_T arfcn;

} gl1_gsm_cell_info_type;


/* Structure provides information specific to the cell, identity and timing */
typedef struct {

  /* Always Valid, when cell information is provided */
  gl1_gsm_cell_info_type  cell_info;     /* Control Channel-Freq, and BSIC */

  /* Always Valid, when cell information is provided */
  gl1_cell_time_type      cell_time;

} gl1_cell_timing_type;


/* Structure carries cell timing information to LSM */
typedef struct {

  /* GSM Layer 1 Main States, ie L1_NULL_MODE... */
  uint8                l1_state;

  /* Indicates whether this message is the result of a cell change or update
     request.  */
  gl1_cell_update_e_type   update_type;

  /* TRUE if the time-tag is based on the sleep timer */
  boolean              sleep_timetag;

  /* TRUE if the GPS timer is valid */
  boolean              gps_timer_valid;

  /* 32-bit GPS timer value. */
  uint32               gps_timer;

  /* Set FALSE if not in service or timing not available */
  boolean              cell_valid;

  /* Cell information */
  gl1_cell_timing_type cell;

} gl1_cell_timing_update_type;

#endif/*FEATURE_CGPS_TIME_TAGGING*/

#endif
