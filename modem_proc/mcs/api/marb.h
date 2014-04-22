/*!
@file
marb.h

@brief
Memory Arbitrator Interface File

This file provides arbitration between GNSS and WWAN techs for loading 
and unloading dynamic memory.

*/
/*
  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.

===========================================================================*/

/*===========================================================================

$Header: //components/rel/mcs.mpss/5.1/api/marb.h#6 $

===========================================================================*/

#ifndef MARB_H
#define MARB_H

/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include <comdef.h>
#include "DDITimetick.h"

/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

/*! MARB Client Groups 
 *
 */
typedef enum
{ 
  /*! ... to be used by all GNSS clients */
  MARB_CLI_GROUP_GNSS = 0,
  
  /*! ... to be used by all WWAN clients */
  MARB_CLI_GROUP_WWAN = 1,
  
  MARB_CLI_GROUP_MAX

} marb_client_group_enum_t;


/*! MARB Client types 
 * 
 */
typedef enum
{
  /*! ...GPS client */
  MARB_CLIENT_GPS = 0,
  
  /*! ...NB client         */
  MARB_CLIENT_NB = 1,
  
  /*! ...CATM client       */
  MARB_CLIENT_CATM = 2,
  
  /*! ...GSM client        */
  MARB_CLIENT_GSM = 3,
  
  MARB_CLIENT_MAX

} marb_client_enum_t;


/*! Timestamp type used by MARB  See:DalTimetick_Get() */
typedef DalTimetickTime64Type           marb_time_t;

/*! Time duration type used by MARB See:DalTimetick_Get() */
typedef DalTimetickTime64Type           marb_duration_t;

/*! @brief Procedure for which client is requesting memory 
 */
typedef enum
{ 
  MARB_PROC_NONE = 0,
  
  MARB_PROC_IDLE_SLEEP = 1,
  
  MARB_PROC_IDLE_AWAKE = 2,
  
  MARB_PROC_CONNECTED = 3,
  
  MARB_PROC_ACTIVE = 4,
  
  MARB_PROC_HIGH = 5,
  
  MARB_PROC_MAX
  
} marb_procedure_t;

/*! MARB Result Type  */
typedef enum
{
   MARB_RESULT_FAIL = 0,

   MARB_RESULT_ACCEPT = 1,

   MARB_RESULT_PENDING_ACCEPT = 2,
   
   MARB_RESULT_MAX
   
} marb_result_enum_t;


/*! MARB Mem Indications  */
typedef enum
{
  /*! Memory is unloaded */
  MARB_MEM_UNLOADED = 0,

  /*! Memory is loaded */
  MARB_MEM_LOADED = 1,

  MARB_MEM_IND_MAX
   
} marb_mem_ind_enum_t;

/*! MARB Async Callback Types   */
typedef enum 
{
  /*! ...to indicate memory owner when priority changes */
  MARB_EVENT_NOTIFY_CB = 0,	
  
  /*! ...to send MARB_RESULT_ACCEPT for a previous pending accept */
  MARB_ACCEPT_CB = 1,
  
  /*! ...to request client to unload memory for a higher priority client */
  MARB_UNLOAD_CB = 2

} marb_async_cb_enum_t;

/*! MARB get info type enum  */
typedef enum
{
  /*! To get current app priority stored in MARB */
  MARB_CURRENT_APP_PRIORITY = 0,

  /*! To get current loaded tech */
  MARB_CURRENT_LOAD_STATE = 1,

  MARB_GET_INFO_MAX
   
} marb_get_info_enum_t;

/*! MARB state enum  */
typedef enum
{
  /*! MARB in unloaded state */
  MARB_UNLOADED = 0,
  
  /*! MARB in WANpending state */
  MARB_WANPENDING = 1,

  /*! MARB in GPSpending state */
  MARB_GPSPENDING = 2,

  /*! MARB in WAN loaded state */
  MARB_WANLOADED = 3,

  /*! MARB in GPS loaded state */
  MARB_GPSLOADED = 4,

  MARB_STATE_MAX

}marb_state_type_t;

/*! MARB Request Input Parameters   */
typedef struct 
{
  /*! Procedure being requested... */
  marb_procedure_t              procedure;
   
  /*! Minimum duration for which tech needs to make use of the memory once granted */ 
  marb_duration_t               duration;
   
  /*! time required to honor marb_unload request and send unload confirmation */
  marb_duration_t               cleanup_duration;

} marb_request_input_t;

/*! MARB Sleep Input Parameters */
typedef struct 
{
  /*! Next procedure on wakeup */
  marb_procedure_t              procedure;
      
  /*! time required to honor marb_unload request */
  marb_duration_t               cleanup_duration;
  
  /*! Time when the procedure is going to start. Should be > 0 */
  marb_time_t                   start_time;

} marb_sleep_input_t;

/*! MARB Indication Input Parameters  */
typedef struct 
{ 
  marb_mem_ind_enum_t           indication;	

} marb_indication_input_t;

/*! MARB Output Parameters   */
typedef struct 
{
  /*! Minimum time to wait before sending next marb_request */
  marb_time_t                   min_wait;
   
  /*! Time required to load the dyn memory */
  marb_time_t                   load_wait;

} marb_wait_time_output_t;

/*! MARB Output Parameters   */
typedef struct 
{
  /*! WWAN sleep start time*/
  marb_time_t                   sleep_start;
   
  /*! WWAN sleep end time */
  marb_time_t                   sleep_end;

} marb_sleep_info_output_t;

/*! MARB Accept Async Callback data */
typedef struct 
{
  /*! Should be MARB_RESULT_ACCEPT */
  marb_result_enum_t            result;
  
  /* place-holder for more arguments */
	
} marb_accept_cb_t;

/*! MARB Unload Async Callback data  */
typedef struct 
{
  /*! Wait time for next request */
  marb_wait_time_output_t       wait_times;	
  
} marb_unload_cb_t;

/*! MARB Notify Async Callback data  */
typedef struct 
{
  /*! Indicates if memory is loaded or not */
  marb_mem_ind_enum_t           mem_indication;	

  /*! If memory is loaded, the current active group */
  marb_client_group_enum_t      active_group;

  marb_sleep_info_output_t      sleep_info;

} marb_event_notify_cb_t;

/*! MARB Async Callback Union         */
typedef union 
{ 
  marb_event_notify_cb_t        notify;
  
  marb_accept_cb_t              accept; 
  
  marb_unload_cb_t              unload;
  	
} marb_async_cb_data_t;

/*! MARB Async Callback Input (MARB --> Client)  */
typedef struct
{
  /*! Client to which async cb is invoked */
  marb_client_enum_t            client;
  
  /*! Type of async callback invoked */
  marb_async_cb_enum_t          cb_type;

  /*! Content of async callback... varies based on cb_type */
  marb_async_cb_data_t          cb_data;  
	
} marb_async_callback_input_t;

/*! MARB get info Input Parameters  */
typedef struct 
{
  marb_get_info_enum_t           info_type;

} marb_get_info_input_t;

/*! MARB get info output data  */
typedef union 
{
  marb_client_group_enum_t       current_app_priority;

  marb_state_type_t              current_loaded_tech;

} marb_get_info_output_data;

/*! MARB get info Output Parameters  */
typedef struct 
{
  marb_get_info_enum_t           info_type;

  marb_get_info_output_data      info ;

} marb_get_info_output_t;

/*! @brief 
 *  This is a prototype for callback function used by the MARB to inform the 
 *  clients to load or unload its dynamic memory
 *  
 *  @note
 *  The callback will be invoked by the MARB from a task context of another 
 *  client, or from interrupt context. Heavy processing, mutexes should be avoided
 *  in this context.
 *  
 *  @return None
 */
typedef void (* marb_async_callback_t)
(
  marb_async_callback_input_t   *cb_input
);

/*! MARB Register Input Parameters     */
typedef struct 
{
  /*! Use marb_async_cb_enum_t as bit positions to indicate which callback
   *  events interested in. ACCEPT and UNLOAD callbacks are sent irrespective
   *  of this bitmask */
  uint32                  cb_bitmask;
  
  /*! Function callback pointer */
  marb_async_callback_t   cb_ptr;
  	
} marb_register_input_t; 

/*! MARB Client Load Duration Info     */
typedef struct 
{
  /*! Client for which load duration is changed */
  marb_client_enum_t            client;
  
  /*! Client Load Time */
  marb_duration_t               load_time;

  /*! Client Unload Time */
  marb_duration_t               unload_time;  

} marb_client_load_duration_t; 

/*! MARB Update Load Duration Input Parameters */
typedef struct 
{
  /*Array of clients load duration info*/
  marb_client_load_duration_t     client_load_info[MARB_CLIENT_MAX];

  /*Num of clients whose load times need to be changed*/
  uint8                           num_clients;
	
} marb_load_duration_input_t; 

/*============================================================================

                   E X T E R N A L  F U N C T I O N S

  This section contains definitions for external apis

============================================================================*/


/*! @brief 
 *  This function is used by tech to register itself with MARB.
 * 
 *  @return
 *  TRUE if registration goes through OR 
 *  FALSE if registration fails in case like invalid arguments or  some other client 
 *  in same group already registered.
 */
boolean marb_register
(
   /*! Client registering with MARB */
   marb_client_enum_t           client,
   
   /*! Client Group type */
   marb_client_group_enum_t     group,
      
   /*! Register params like callback function pointers etc */
   marb_register_input_t        params
);

/*! @brief 
 *  This function is used by tech to deregister itself with MARB.
 * 
 *  @note 
 *  Tech should unload its memory and send MARB_MEM_UNLOADED marb_indication() 
 *  before invoking this
 *
 *  @return
 *  MARB_RESULT_ACCEPT if tech is in Unloaded state and has no pending 
 *  requests
 *  MARB_RESULT_FAIL otherwise.
 */
marb_result_enum_t marb_deregister
(
   /*! Deregister client with MARB */
   marb_client_enum_t           client
);

/*! @brief 
 *  This function is used by tech to request for memory resource
 * 
 *  @note
 *  Client should be registered first with marb_register()
 *
 *  @return 
 *   MARB_RESULT_ACCEPT when request can be accepted right away
 *   MARB_RESULT_PENDING_ACCEPT when request needs to wait for currently
 *     active tech to unload.
 *   MARB_RESULT_FAIL when request cannot be accepted at this time.
 *   
 *   output->min_wait: updated when return value is MARB_RESULT_FAIL or 
 *     MARB_RESULT_PENDING_ACCEPT
 *   output->load_wait: updated for all return types
 */
marb_result_enum_t marb_request
(
   /*! Client Indicator */
   marb_client_enum_t           client,

   /*! Pointer to input parameters */
   marb_request_input_t         *input,
   
   /*! Pointer to output paramerts */
   marb_wait_time_output_t      *output
);

/*! @brief 
 *  This function is used by tech when it is going to sleep and put in a  
 *  request for memory resource at a future time. MARB would change 
 *  priority to MARB_PROC_SLEEP on accepting this.
 *
 *  @note
 *  Client should have a MARB_RESULT_ACCEPT for a prior marb_request 
 *  before invoking this. 
 *
 *  @return marb_result_enum_t
 */
marb_result_enum_t marb_sleep
(
   /*! Client Indicator */
   marb_client_enum_t           client,

   /*! Pointer to input parameters */
   marb_sleep_input_t           *input   
);

/*! @brief 
 *  This function is used by tech to notify MARB if memory is loaded or 
 *  unloaded 
 *
 *  @note
 *  Should be invoked in response to MARB_RESULT_ACCEPT or marb_unload()
 *
 *  @return None
 */
void marb_indication
(
  /*! Client Indicator */
  marb_client_enum_t           client,

  /*! Pointer to input parameters */
  marb_indication_input_t      *input   
);

/*! @brief 
 *  This function is used to set the application level priority
 *
 *  @return
 *  MARB_RESULT_ACCEPT if update is success 
 *  MARB_RESULT_FAIL otherwise.
 */
marb_result_enum_t marb_update_app_priority
(
   marb_client_group_enum_t     app_prio  
);

/*! @brief 
 *  This function is used to update load and unload times for a client or 
 *  for all clients. Units of msec used for the load/unload duration.
 *
 *  @return FAIL if any input argument is invalid like if durations are 0 
 */
marb_result_enum_t marb_update_load_durations
(
  /*! Pointer to input parameters */
  marb_load_duration_input_t    *input
);

/*! @brief 
 *  This function is used to initialize Memory Arbitrator.
 * 
 *  @note
 *  Should be invoked before any other marb function
 *
 *  @return None
 */
void marb_taskinit(void);

/*! @brief 
 *  This function is used to get info from MARB.
 * 
 *  @note
 *  Currently invoked by app
 *
 *  @return None
 */
marb_result_enum_t marb_get_info(marb_get_info_input_t *input, marb_get_info_output_t *output);

#endif /* MARB_H */
