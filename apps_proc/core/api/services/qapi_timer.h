/*=======================================================================
 Copyright (c) 2017-2020,2022 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/

/* $Header: //components/rel/core.tx/6.0/api/services/qapi_timer.h#5 $ */

/*==========================================================================
when       who     what, where, why
--------   ---    ----------------------------------------------------------
01/15/18   leo    (Tech Comm) Edited/added Doxygen comments and markup.
09/04/17   leo    (Tech Comm) Edited/added Doxygen comments and markup.
04/24/17   leo    (Tech Comm) Edited/added Doxygen comments and markup.
05/01/16   gp     Created
===========================================================================*/

#ifndef __QAPI_TIMER_H__
#define __QAPI_TIMER_H__

/*=================================================================================
 *
 *                       TIMER SERVICES
 *
 *================================================================================*/

/** @file qapi_timer.h
 *
 * @brief Timer Services 
 *
 * @addtogroup qapi_timer
 * @{
 *
 * @details This interface implements Advanced Time Services (ATS) timer services. 
 *          This timer service is different than the RTOS timer service. 
 *          This timer service will be available in SOM mode.
 * 
 * 
 * @note1hang  These routines are fully re-entrant. In order to prevent memory leaks,
 *          whenever timer usage is done, the timer should be undefined using the 
 *          qapi_Timer_Undef() API. Timer callbacks should do minimal processing. 
 *          Time callbacks implementation should not contain any mutex or RPC.
 *          
 *
 * @code
 *
 *    * The code snippet below demonstrates usage of timer interface. In the
 *    * example below, a client defines a timer, sets a timer, stops the timer,
 *    * and undefines a timer.
 *    * For brevity, the sequence assumes that all calls succeed. 
 * 
 *    qapi_TIMER_handle_t timer_handle;	  
 *    
 *    qapi_TIMER_def_attr_t timer_def_attr; 
 *    timer_def_attr.cb_type = TIMER_FUNC1_CB_TYPE; //notification type
 *    timer_def_attr.sigs_func_ptr = &timer_test_cb; //callback to call when
 *                                                   //the timer expires
 *    timer_def_attr.sigs_mask_data = 0x1; //this data will be returned in
 *                                         //the callback
 *    timer_def_attr.deferrable = false; //set to true for nondeferrable timer
 *    
 *    //define the timer. Note: This call allocates memory and hence
 *    //qapi_Timer_Undef() should be called whenever the timer usage is done.
 *    qapi_Timer_def( &timer_handle, &timer_def_attr);
 *
 *    qapi_TIMER_set_attr_t timer_set_attr; 
 *    timer_set_attr.reload = reload_duration; //Timer would expire after every
                                               //reload_duration value.
 *    timer_set_attr.time = time_duration; 
 *    timer_set_attr.unit = T_MSEC; 
 *    
 *    //set or start the timer
 *    qapi_Timer_set( timer_handle, &timer_set_attr);
 *
 *    //stop a running timer
 *    qapi_Timer_stop( timer_handle);
 *
 *    //Undef the timer. Releases memory allocated in qapi_Timer_Def()
 *    qapi_Timer_undef( timer_handle);
 *
 *
 * @endcode
 *
 *  @}
*/

/*==================================================================================

                               TYPE DEFINITIONS

==================================================================================*/
#include "qapi_status.h"
#include "tx_port.h"
#include "txm_module.h"


#define   QAPI_TIMER_INDEX_MASK     0xDEAD0000
#define   QAPI_MAX_TIMER_TABLE_SIZE 128

/** @addtogroup qapi_timer
@{ */
/**
* Timer notification type.
*
* Enumeration of the notifications available on timer expiry. 
*/
typedef enum {
  QAPI_TIMER_NO_NOTIFY_TYPE,          /**< No notification. */  
  QAPI_TIMER_NATIVE_OS_SIGNAL_TYPE,   /**< Signal an object. */    
  QAPI_TIMER_FUNC1_CB_TYPE,           /**< Call back a function. */    
  QAPI_TIMER_INVALID_NOTIFY_TYPE    
} qapi_TIMER_notify_t;

/**
* Timer unit type.
*
* Enumeration of the units in which timer duration can 
* be specified. 
*/
typedef enum {
  QAPI_TIMER_UNIT_TICK,     /**< Return time in ticks. */
  QAPI_TIMER_UNIT_USEC,     /**< Return time in microseconds. */
  QAPI_TIMER_UNIT_MSEC,     /**< Return time in milliseconds. */
  QAPI_TIMER_UNIT_SEC,      /**< Return time in seconds. */
  QAPI_TIMER_UNIT_MIN,      /**< Return time in minutes. */
  QAPI_TIMER_UNIT_HOUR,     /**< Return time in hours. */
  QAPI_TIMER_UNIT_MAX
}qapi_TIMER_unit_type;

/**
* Timer information type.
*
* Enumeration of the types of information that can be 
* obtained for a timer. 
*/
typedef enum{
  QAPI_TIMER_TIMER_INFO_ABS_EXPIRY = 0, /**< Return the timetick of timer expiry in native ticks. */
  QAPI_TIMER_TIMER_INFO_TIMER_DURATION, /**< Return the total duration of the timer in specified units. */
  QAPI_TIMER_TIMER_INFO_TIMER_REMAINING, /**< Return the remaining duration of the timer in specified units. */
  QAPI_TIMER_TIMER_INFO_MAX, 
}qapi_TIMER_info_type;


/**
* Timer define attribute type.
*
* This type is used to specify parameters when defining a timer. 
* @verbatim 
* sigs_func_ptr will depend on the value of qapi_TIMER_notify_t.
* qapi_TIMER_notify_t == QAPI_TIMER_NO_NOTIFY_TYPE, 
* sigs_func_ptr = Don't care 
*  
* qapi_TIMER_notify_t == QAPI_TIMER_NATIVE_OS_SIGNAL_TYPE, 
* sigs_func_ptr = qurt signal object 
*  
* qapi_TIMER_notify_t == QAPI_TIMER_FUNC1_CB_TYPE, 
* sigs_func_ptr == specify a callback of type qapi_TIMER_cb_t
* @endverbatim
*/
typedef struct
{
  qbool_t                   deferrable; /**< FALSE = deferrable. */  
  qapi_TIMER_notify_t       cb_type;    /**< Type of notification to receive. */
  void*                     sigs_func_ptr; /**< Specify the signal object or callback function.  */  
  uint32_t                  sigs_mask_data; /**< Specify the signal mask or callback data. */
}qapi_TIMER_define_attr_t;

/**
* Type used to get a user space expired timer's callback information.
*
* This type is used to get a user space expired timer's callback information. 
* @verbatim 
* data = Specify the callback data for func_ptr,
* func_ptr = function pointer needs to be invoked.
*  
* @endverbatim
*/
typedef struct
{
  void*                     func_ptr; /**< Specify the callback function.  */  
  uint32_t                  data;     /**< Specify the callback data. */  
} qapi_TIMER_get_cbinfo_t;

/**
* Type used to specify parameters when starting a timer.
*/
typedef struct
{
  uint64_t              time;   /**< Timer duration. */  
  uint64_t              reload; /**< Reload duration. */
  qapi_TIMER_unit_type  unit;   /**< Specify units for timer duration. */  
}qapi_TIMER_set_attr_t;

/**
* Type used to get information for a given timer.
*/
typedef struct
{
  qapi_TIMER_info_type   timer_info;   /**< Timer information type. */
  qapi_TIMER_unit_type   unit;   /**< Specify units to use for return. */
}qapi_TIMER_get_info_attr_t;

/** @} */

/** @addtogroup qapi_timer
@{ */

/**
* Timer handle.
*
* Handle provided by the timer module to the client. Clients 
* must pass this handle as a token with subsequent timer calls. 
* Note that the clients should cache the handle. Once lost, it 
* cannot be queried back from the module. 
*/

typedef void* qapi_TIMER_handle_t;


/**
* Timer callback type.
*
* Timer callbacks should adhere to this signature.
*/
typedef void (*qapi_TIMER_cb_t)(uint32_t data);

/** @} */

/** @addtogroup qapi_timer
@{ */
/**
* Time type.
*
* Native timestamp type.
*/
typedef unsigned long qapi_qword[ 2 ];


/**
* Time type.
*
* Native timestamp type. Same as qapi_qword.
*/
typedef qapi_qword qapi_time_type;

/**
* Time in Julian format.
*/
typedef struct
{
  uint16_t year;        /**< Year (1980 through 2100). */
  uint16_t month;       /**< Month of the year (1 through 12). */
  uint16_t day;         /**< Day of the month (1 through 31). */
  uint16_t hour;        /**< Hour of the day (0 through 23). */
  uint16_t minute;      /**< Minute of the hour (0 through 59). */
  uint16_t second;      /**< Second of the minute (0 through 59). */
  uint16_t day_of_week; /**< Day of the week (0 through 6 or Monday through 
                           Sunday). */
}
qapi_time_julian_type;

/**
* Timer node 
* 
* Userspace timers are stored in a linked list
* Each node of the linked list is of this type
*/
struct node
{
  uint16_t index;
  struct node *next;
};

typedef struct node qapi_free_timer_node;


/**
* Time unit type.
*
* Enumeration of the types of time that can be
* obtained from time get QAPI.
*/
typedef enum
{
  QAPI_TIME_STAMP = 0x10, /**< Return the time in timestamp format. */
  QAPI_TIME_MSECS, /**< Return the time in millisecond format. */
  QAPI_TIME_SECS, /**< Return the time in second format. */
  QAPI_TIME_JULIAN, /**< Return the time in Julian calendar format. */
}qapi_time_unit_type;

/**
* Used to specify parameters when getting the time. 
* @verbatim 
* Pointers depend on the value of qapi_time_unit_type.
* qapi_time_unit_type == QAPI_TIME_STAMP, 
* time_ts = Of type qapi_time_type
*  
* qapi_time_unit_type == QAPI_TIME_MSECS, 
* time_msecs = Of type unint64_t
*
* qapi_time_unit_type == QAPI_TIME_SECS, 
* time_secs = Of type unint64_t
*
* qapi_time_unit_type == QAPI_TIME_JULIAN, 
* time_julian = Of type qapi_time_julian_type
* @endverbatim
*/
typedef union
{
  qapi_time_type time_ts; /**< Specify the qapi_time_type variable pointer. */
  uint64_t time_msecs; /**< Variable for getting time in msec. */
  uint64_t time_secs; /**< Variable for getting time in sec. */
  qapi_time_julian_type time_julian; /**< Variable for getting time in Julian. */
}qapi_time_get_t;


/** Various time bases.
*/
typedef enum qapi_time_bases
{
  QAPI_ATS_RTC = 0, /**< Real time clock timebase.*/
  QAPI_ATS_TOD,     /**< Proxy base for number of bases.*/
  QAPI_ATS_USER,    /**< User timebase. */
  QAPI_ATS_SECURE,  /**< Secure timebase. */
  QAPI_ATS_DRM,     /**< Digital rights management timebase. */
  QAPI_ATS_USER_UTC,   /**< Universal Time Coordinated user timebase. */   
  QAPI_ATS_USER_TZ_DL, /**< Global time zone user timebase. */
  QAPI_ATS_GPS,    /**< Base for GPS time. \n
                   @note1hang When ATS_GSTK is modified, changes are also 
                   reflected on ATS_TOD. */
  QAPI_ATS_1X,     /**< Base for 1X time. \n
                   @note1hang When ATS_1X is modified, changes are also 
                   reflected on ATS_TOD. */
  QAPI_ATS_HDR,    /**< Base for HDR time. \n
                   @note1hang When ATS_HDR is modified, changes are also 
                   reflected on ATS_TOD. */
  QAPI_ATS_WCDMA,  /**< Base for WCDMA time. \n
                   @note1hang When ATS_WCDMA is modified, changes are also 
                   reflected on ATS_TOD. */
  QAPI_ATS_MFLO,   /**< Base for MediaFLO time. \n
                   @note1hang When ATS_MFLO is modified, changes are also 
                   reflected on ATS_TOD. */  
  QAPI_ATS_3GPP, /**< LTE timebase. */
  QAPI_ATS_UTC,    /**< ATS_UTC is on apps */
  QAPI_ATS_LTE_HR, /**< Base for storing LTE SIB16 UTC Time. SIB16 has the same 
                 granularity as 1x. It will be handled similar to ATS_3GPP */
  QAPI_ATS_LTE_HR_GPS, /**< Base for storing LTE SIB16 GMT Time. SIB16 has the same 
                 granularity as 1x. It will also have TOD as a proxy base */
  QAPI_ATS_WLAN,   /**<  Base for WLAN time. \n
                   @note1hang When TIME_SERVICE_BASE_WLAN is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD.  */
  QAPI_ATS_5G,     /**<  Base for 5G time. \n
                   @note1hang When TIME_SERVICE_BASE_5G is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD.  */
  QAPI_ATS_PSM,    /**< Base for storing timer inaccuracy value when the device
                   goes into PSM. */
  QAPI_ATS_PRIVATE = 0x1000000, /**< Holder for Private Bases that are declared 
                                outside of generic time framework */
  QAPI_ATS_INVALID = 0x10000000
} qapi_time_bases_type;

#define QAPI_ATS_MAX     ( QAPI_ATS_PSM + 1 )

#define QAPI_MAX_CB_FUNC              (QAPI_ATS_MAX)

/** Time the generic offset operation is to be done. 
*/
typedef enum qapi_time_genoff_opr
{
  QAPI_T_SET = 0x1, /**< Genoff_Opr sets the time. */
  QAPI_T_GET = 0x2, /**< Genoff_Opr gets the time. */
  QAPI_T_SET_EXT = 0x10, /**< Genoff_Opr sets the time from an external generic offset structure. */
  QAPI_T_GET_EXT = 0x20, /**< Genoff_Opr gets the time from an external generic offset structure. */
  QAPI_T_INIT_EXT = 0x100, /**< Genoff_Opr initializes external generic offset structure. */
  QAPI_T_MAX = 0x8000  /**< Placeholder for maximum enumerator value. */
} qapi_time_genoff_opr_type;

/** Base source from which to get the time.
*/
typedef enum qapi_base_time_source
{
  QAPI_TIME_SCLK,
  QAPI_TIMETICK_SCLK64
} qapi_base_time_source_type ;

/* Time Generic Offset 'type 1' callback function */
typedef void (*qapi_time_genoff_t1_cb_type)(void);

/* Time Generic Offset 'type 2' callback function */
typedef void (*qapi_time_genoff_t2_cb_type)
(
  /* Base that has registered for callback */
  void       *registered_genoff_ptr,

  /* data to pass to this callback function */
  int64_t      data
);

 typedef struct
{
  /* Mark as TRUE if persistent storage is needed */
  boolean                        initialized;

  /* file name for efs item file */
  char                           f_name[10];

  /* Threshold in ms for writing back in efs item file */
  int64_t                          threshold;

} qapi_time_genoff_per_storage_type;

/** External time generic offset structure to be stored in
    client memory
*/
typedef struct qapi_time_genoff_ext_struct
{
  /* Generic Offset, always stored in ms */
  int64_t                           generic_offset;

  /* Sum of generic_offset of current base and its bases
     in timestamp format */
  uint64_t                          sum_generic_offset;

  /* Flag to indicate if time-of-day has been initialized */
  boolean                         initialized;

  /* Flag to indicate if time-base has ever been set before.
     It is not set to TRUE in time_genoff_set call and
     not set to TRUE, after value from persistent storage */
  boolean                         valid;

  /* Time base Type */
  qapi_time_bases_type                 bases_type;

  /* Initialization Sequence */
  qapi_time_genoff_t1_cb_type          init_func;

  /* Mark if subsys based on another subsys */
  boolean                         reqd_base_genoff;

  /* Base Subsys */
  qapi_time_bases_type                 subsys_base;

  /* Attach Callback of type2 for set offset */
  qapi_time_genoff_t2_cb_type          cb_func[QAPI_MAX_CB_FUNC];

  /* Bases that have registered for callback */
  struct time_genoff_ext_struct*  registered_genoff[QAPI_MAX_CB_FUNC];

  /* Currently, Number of Call Back Attached */
  uint8_t                           number_of_cb_func;

  /* Specification for persistent storage */
  qapi_time_genoff_per_storage_type    per_storage_spec;

  /* Flag to indicate if remote call is needed */
  boolean                         remote_call_needed;

  /* Flag to indicate if remote updates is allowed across power cycle */
  boolean                         allow_remote_updates;

  /* Proxy base : Also updated during set opr if current base is updated */
  qapi_time_bases_type                 proxy_base;


} qapi_time_genoff_struct_type, *qapi_time_genoff_ptr;

/**
@brief Structure to be passed as the argument to set and get functions.
*/
typedef struct qapi_time_genoff_args
{
  qapi_time_bases_type                 base;   /**< Pointer to genoff in
                                               consideration. */
  void *                               ts_val; /**< Time to be passed/returned. */
  qapi_time_unit_type                  unit;   /**< Time unit being used. */
  qapi_time_genoff_opr_type            operation; /**< Time operation to be done. */
  qapi_base_time_source_type           base_source; /**< Base source type. */
  qapi_time_genoff_ptr                 ext_genoff_ptr; /**< External generic offset pointer */
  uint32_t                               dal_timetick_val_valid; /**< Flag to state whether a timestamp
                                                               dal_timetick_val is populated with
                                                               QTimer count from DalTimetick. Populate
                                                               with TIME_GENOFF_TIMETICK_VALID if valid */
  uint64_t                               dal_timetick_val; /**< QTimer cnt value at which timestamp was valid */

} qapi_time_genoff_args_struct_type;

/**
@brief Pointer type of time_genoff_args_struct_type.
*/
typedef struct qapi_time_genoff_args *qapi_time_genoff_args_ptr;

/**
*   
* Gets the time in the specified format. 
*
* @param[in] time_get_unit  Unit in which to get the time. 
* @param[in] time           Pointer to the #qapi_time_get_t variable. 

  @return
  QAPI_OK on success, an error code on failure.
*/
qapi_Status_t qapi_time_get(
   qapi_time_unit_type    time_get_unit,
   qapi_time_get_t* time
);

/**
*   
* Gets the timer inaccuracy in milliseconds. 
*
* @param[in] pargs  Pointer to the arguments.            

  @return
  QAPI_OK on success, an error code on failure.
*/
qapi_Status_t qapi_get_timer_inaccuracy(
	qapi_time_genoff_args_ptr    pargs
);

/**
    
* Allocates internal memory in the timer module. The internal 
* memory is then formatted with parameters provided in 
* timer_def_attr variable. The allocated timer pointer is kept 
* in a table. A unique integer is returned to the client as 
* timer_handle and this handle should be used for any subsequent
* timer operations. 
  
* @param[in] timer_handle  Handle to the timer. 
* @param[in] timer_attr    Attributes for defining the timer. 
  
  @return
  QAPI_OK on success, an error code on failure.
  
* @sideeffects
* Calling this API causes memory allocation. 
* Therefore, whenever the timer usage is done and
* not required, qapi_Timer_Undef() must be called
* to release the memory, otherwise it will cause a
* memory leak.
*/
qapi_Status_t qapi_Timer_Def(
   qapi_TIMER_handle_t*    timer_handle,
   qapi_TIMER_define_attr_t*  timer_attr  
);


/**
*   
* Starts the timer with the duration specified in timer_attr. 
* If the timer is specified as a reload timer in timer_attr, 
* the timer will restart after its expiry.
*
* @param[in] timer_handle  Handle to the timer. 
* @param[in] timer_attr    Attributes for setting the timer. 
*
* @return
* QAPI_OK on success, an error code on failure.
*
* @dependencies
* The qapi_Timer_Def() API should be called for the timer before 
* calling qapi_Timer_Set function.
*/
qapi_Status_t qapi_Timer_Set(
   qapi_TIMER_handle_t    timer_handle,
   qapi_TIMER_set_attr_t* timer_attr  
);

/**
*   
* Gets specified information about the timer.
*
* @param[in] timer_handle  Handle to the timer. 
* @param[out] timer_info   Type of information 
*       needed from the timer.
* @param[out] data         Returned timer information.
*
* @return
* QAPI_OK on success, an error code is returned on failure.
* 
*/
qapi_Status_t qapi_Timer_Get_Timer_Info(
   qapi_TIMER_handle_t         timer_handle,
   qapi_TIMER_get_info_attr_t* timer_info,
   uint64_t*                   data
);

/**
* 
* Timed wait. Blocks a thread for a specified time. 
*
* @param[in] timeout  Specify the duration to block the thread.
* @param[in] unit     Specify the units of the duration.
* @param[in] non_deferrable  TRUE = processor (if in deep 
*       sleep or power collapse) will be awakened on timeout. \n
*       FALSE = processor will not be awakened from deep sleep
*       or power collapse on timeout. \n Whenever the processor
*       wakes up due to some other reason after timeout, the
*       thread will be unblocked.
*
* @return
* QAPI_OK on success, an error code on failure.
* 
*/
qapi_Status_t qapi_Timer_Sleep(
  uint64_t             timeout,
  qapi_TIMER_unit_type unit,
  qbool_t              non_deferrable
);

/**
*   
* Undefines the timer. This API needs to be called whenever 
* timer usage is done. Calling this API will release the 
* internal timer  memory that was allocated when the timer
* was defined, make the corresponding entry in the qapi 
* timer tablec as NULL and add a node in the list of free 
* indices.

*
* @param[in] timer_handle  Timer handle for which to undefine the timer.
*
* @return
* QAPI_OK on success, an error code on failure
* 
*/
qapi_Status_t qapi_Timer_Undef(
    qapi_TIMER_handle_t    timer_handle
);

/**
    
* Stops the timer.
*
* @note1hang This function does not deallocate the 
* memory that was allocated when the timer was defined.
  
* @param[in] timer_handle  Timer handle for which to stop the timer.
  
  @return
  QAPI_OK on success, an error code on failure.
* 
*/
qapi_Status_t qapi_Timer_Stop(
    qapi_TIMER_handle_t    timer_handle
);

qapi_Status_t timer_qapi_handler(UINT request_ID, 
										UINT arg1, 
										UINT arg2, 
										UINT arg3, 
										UINT arg4, 
										UINT arg5, 
										UINT arg6, 
										UINT arg7, 
										UINT arg8, 
										UINT arg9, 
										UINT arg10, 
										UINT arg11, 
										UINT arg12);


/** @} */ /* end_addtogroup_group timer */

#endif /*__QAPI_TIMER_H__ */
