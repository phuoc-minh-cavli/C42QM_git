#ifndef TIME_GENOFF_H
#define TIME_GENOFF_H

/**
@file time_genoff.h
@brief
This module implements the Generic framework to handle time bases.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ATS_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ATS_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the time_genoff group 
      description in the ATS_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2009-2012,2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================

 T I M E   S E R V I C E   G E N E R I C  F R A M E W O R K  S U B S Y S T E M

GENERAL DESCRIPTION
 Implements Generic framework to handle Time bases. 

EXTERNALIZED FUNCTIONS

  time_genoff_opr
   Does time set/get operation on the basis of Inputs

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Should be initialized with following sequence

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/services/time_genoff.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/29/10   scd     (Tech Pubs) Edited Doxygen markup and comments.
10/27/10   EBR     Doxygenated File.
06/23/10   din     Added new time bases and attached a proxy base for a base.
05/18/10   din     Shifted time_remote_opr_type from time_genoff_v.h
10/07/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "time_types.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/*--------------------------------------------------------------------------
  Linting...
--------------------------------------------------------------------------*/

/*lint -esym(715,unused_*)  parameters prefixed with unused_ are not used */


/*-----------------------------------------------------------------------------
  Macros used for converting to/from ts_timestamp format
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Time Bases static information
-----------------------------------------------------------------------------*/

/** @addtogroup time_genoff
@{ */

/** Various time bases.
*/
typedef enum time_bases
{
  ATS_RTC = 0, /**< Real time clock timebase.*/
  ATS_TOD,     /**< Proxy base for number of bases.*/
  ATS_USER,    /**< User timebase. */
  ATS_SECURE,  /**< Secure timebase. */
  ATS_DRM,     /**< Digital rights management timebase. */
  ATS_USER_UTC,   /**< Universal Time Coordinated user timebase. */   
  ATS_USER_TZ_DL, /**< Global time zone user timebase. */
  ATS_GPS,    /**< Base for GPS time. \n
                   @note1hang When ATS_GSTK is modified, changes are also 
                   reflected on ATS_TOD. */
  ATS_1X,     /**< Base for 1X time. \n
                   @note1hang When ATS_1X is modified, changes are also 
                   reflected on ATS_TOD. */
  ATS_HDR,    /**< Base for HDR time. \n
                   @note1hang When ATS_HDR is modified, changes are also 
                   reflected on ATS_TOD. */
  ATS_WCDMA,  /**< Base for WCDMA time. \n
                   @note1hang When ATS_WCDMA is modified, changes are also 
                   reflected on ATS_TOD. */
  ATS_MFLO,   /**< Base for MediaFLO time. \n
                   @note1hang When ATS_MFLO is modified, changes are also 
                   reflected on ATS_TOD. */
  ATS_3GPP,   /**< LTE timebase. */
  ATS_UTC,    /**< ATS_UTC is on apps */
  ATS_LTE_HR, /**< Base for storing LTE SIB16 UTC Time. SIB16 has the same 
                 granularity as 1x. It will be handled similar to ATS_3GPP */
  ATS_LTE_HR_GPS, /**< Base for storing LTE SIB16 GMT Time. SIB16 has the same 
                 granularity as 1x. It will also have TOD as a proxy base */
  ATS_WLAN,   /**<  Base for WLAN time. \n
                   @note1hang When TIME_SERVICE_BASE_WLAN is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD.  */
  ATS_5G,     /**<  Base for 5G time. \n
                   @note1hang When TIME_SERVICE_BASE_5G is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD.  */
  ATS_PSM,    /**< Base for storing timer inaccuracy value when the device
                   goes into PSM. */
  ATS_PRIVATE = 0x1000000, /**< Holder for Private Bases that are declared 
                                outside of generic time framework */
  ATS_INVALID = 0x10000000
} time_bases_type;

#define ATS_MAX     ( ATS_PSM + 1 )

/** Time unit type. 
*/
typedef enum time_unit
{
  TIME_STAMP,      /**< Time is in timestamp format. */
  TIME_MSEC,       /**< Time is in millisecond format. */
  TIME_SECS,       /**< Time is in second format. */
  TIME_JULIAN,     /**< Time is in Julian calendar format. */
  TIME_20MS_FRAME, /**< Time is in 20-millisecond frames format. */
  TIME_NONE        /**< Time format is undefined. */
} time_unit_type ;

/** Time the generic offset operation is to be done. 
*/
typedef enum time_genoff_opr
{
  T_SET = 0x1, /**< Genoff_Opr sets the time. */
  T_GET = 0x2, /**< Genoff_Opr gets the time. */
  T_SET_EXT = 0x10, /**< Genoff_Opr sets the time from an external generic offset structure. */
  T_GET_EXT = 0x20, /**< Genoff_Opr gets the time from an external generic offset structure. */
  T_INIT_EXT = 0x100, /**< Genoff_Opr initializes external generic offset structure. */
  T_MAX = 0x8000  /**< Placeholder for maximum enumerator value. */
} time_genoff_opr_type;

#define TIME_GENOFF_OPR_SET_MASK      (T_SET | T_SET_EXT)
#define TIME_GENOFF_OPR_GET_MASK      (T_GET | T_GET_EXT)
#define TIME_GENOFF_OPR_INIT_MASK     (T_INIT_EXT)
#define TIME_GENOFF_OPR_EXTERNAL_MASK (T_SET_EXT | T_GET_EXT)

/* Max number of callback functions */
#define MAX_CB_FUNC              16

/* Max number of callback functions */
#define MAX_REMOTE_CB_FUNC       5

/* Timetick value valid flag */
#define TIME_GENOFF_TIMETICK_VALID 0x5E15E15E

/* Time Generic Offset 'type 1' callback function */
typedef void (*time_genoff_t1_cb_type)(void);

/* Time Generic Offset 'type 2' callback function */
typedef void (*time_genoff_t2_cb_type)
(
  /* Base that has registered for callback */
  void       *registered_genoff_ptr,
  
  /* data to pass to this callback function */
  int64      data
);

typedef struct 
{
  /* Mark as TRUE if persistent storage is needed */
  boolean                        initialized;

  /* file name for efs item file */
  char                           f_name[10];

  /* Threshold in ms for writing back in efs item file */
  int64                          threshold;

} time_genoff_per_storage_type;

/*-----------------------------------------------------------------------------
  Generic offset information
-----------------------------------------------------------------------------*/
/** Base source from which to get the time.
*/
typedef enum base_time_source
{
  TIME_SCLK,
  TIMETICK_SCLK64
} base_time_source_type ;

/** External time generic offset structure to be stored in
    client memory
*/
typedef struct time_genoff_ext_struct
{
  /* Generic Offset, always stored in ms */
  int64                           generic_offset;

  /* Sum of generic_offset of current base and its bases 
     in timestamp format */
  uint64                          sum_generic_offset;

  /* Flag to indicate if time-of-day has been initialized */
  boolean                         initialized;

  /* Flag to indicate if time-base has ever been set before.
     It is not set to TRUE in time_genoff_set call and 
     not set to TRUE, after value from persistent storage */
  boolean                         valid;

  /* Time base Type */
  time_bases_type                 bases_type;               
  
  /* Initialization Sequence */
  time_genoff_t1_cb_type          init_func;

  /* Mark if subsys based on another subsys */
  boolean                         reqd_base_genoff;

  /* Base Subsys */
  time_bases_type                 subsys_base;

  /* Attach Callback of type2 for set offset */
  time_genoff_t2_cb_type          cb_func[MAX_CB_FUNC];

  /* Bases that have registered for callback */
  struct time_genoff_ext_struct*  registered_genoff[MAX_CB_FUNC];

  /* Currently, Number of Call Back Attached */
  uint8                           number_of_cb_func;

  /* Specification for persistent storage */
  time_genoff_per_storage_type    per_storage_spec;

  /* Flag to indicate if remote call is needed */
  boolean                         remote_call_needed;

  /* Flag to indicate if remote updates is allowed across power cycle */
  boolean                         allow_remote_updates;

  /* Proxy base : Also updated during set opr if current base is updated */
  time_bases_type                 proxy_base;


} time_genoff_struct_type, *time_genoff_ptr;

/**
@brief Structure to be passed as the argument to set and get functions. 
*/
typedef struct time_genoff_args
{
  time_bases_type                 base;   /**< Pointer to genoff in 
                                               consideration. */  
  void *                          ts_val; /**< Time to be passed/returned. */
  time_unit_type                  unit;   /**< Time unit being used. */
  time_genoff_opr_type            operation; /**< Time operation to be done. */
  base_time_source_type           base_source; /**< Base source type. */
  time_genoff_ptr                 ext_genoff_ptr; /**< External generic offset pointer */
  uint32                          dal_timetick_val_valid; /**< Flag to state whether a timestamp
                                                               dal_timetick_val is populated with
                                                               QTimer count from DalTimetick. Populate
                                                               with TIME_GENOFF_TIMETICK_VALID if valid */
  uint64                          dal_timetick_val; /**< QTimer cnt value at which timestamp was valid */

} time_genoff_args_struct_type;

/**
@brief Pointer type of time_genoff_args_struct_type.
*/
typedef struct time_genoff_args *time_genoff_args_ptr;

/**
@brief Time type to be passed as the remote type.
*/
typedef union time_remote_opr
{
  time_type                       timestamp; /**< Time to be passed/returned
                                                  in timestamp format. */
  time_julian_type                julian;    /**< Time to be passed/returned
                                                  in time_julian format. */
}time_remote_opr_type;
/*~ CASE TIME_STAMP TIME_MSEC TIME_SECS TIME_20MS_FRAME time_remote_opr.timestamp */ 
/*~ CASE TIME_JULIAN time_remote_opr.julian */


/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 
Performs the time set/get operation based on the inputs passed in.

@param[in] pargs Pointer to the arguments.

@return
None.

@dependencies
None.
*/
void time_genoff_opr
(
  time_genoff_args_ptr         pargs
);

/** 
Syncronizes the TOD genoff data with the Julian time.

@param[in] ts_val Timestamp with which to synchronize.

@return
None.
 
@dependencies  
None.
*/
void time_genoff_sync_with_tod
(
  time_type ts_val
);

/**
Determines whether the remote modem is to be updated.

@param[in] update_remote_modem Boolean value to determine whether the remote 
                               modem is to be updated.

@return
None.

@dependencies  
None.
*/
void time_genoff_set_remote_modem_update
(
  boolean update_remote_modem
);

/**
Returns whether the remote modem is being updated.

@return
TRUE  -- Remote modem is being updated. \n
FALSE -- Remote modem is not being updated.
      
@dependencies  
None.
*/
boolean time_genoff_get_remote_modem_update
(
  void
);

#ifdef FEATURE_QMI_TIME_REMOTE_CLNT
/*=============================================================================

FUNCTION TIME_GENOFF_GET_SYS_TIME_IN_SEC_WITH_LP_SEC

DESCRIPTION
  This function sets gets the system time alongwith leap second info

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
uint32 time_genoff_get_sys_time_in_sec_with_lp_sec
(
  void
);
#endif
/** @} */ /* end_addtogroup time_genoff */

#endif /* TIME_GENOFF_H */
