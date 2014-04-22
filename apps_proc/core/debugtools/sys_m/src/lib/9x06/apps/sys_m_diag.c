/*===========================================================================

           S Y S _ M_DIAG . C

DESCRIPTION

Copyright (c) 2011-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/debugtools/sys_m/src/lib/9x06/apps/sys_m_diag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/16   nk      Fixed issue with multiple defines of sysmdiagTimer 
09/26/14   dav     File Created

===========================================================================*/

#include "diagpkt.h"
#include "diagcmd.h"
#include "sys_m_diag.h"
#include "tms_diag.h"
#include "timer.h"
#include "msg.h"
#include "DDIPlatformInfo.h"
#include "subsystem_control_v02.h"
#include <stringl/stringl.h>
#include "sys_m.h"
#include "tms_utils_msg.h"

#define SYS_M_DIAG_RESP_DELAY_MS 50
#define MODE_RESET              2
#define MODE_SHUTDOWN           6

int sys_m_shutdown_delay_secs = 0;
int sys_m_diag_timer_def_status = 0; /* Variable to know whether sysmdiagTimer is defined or not */

typedef PACK(struct) PACKED_POST
{
  diagpkt_subsys_header_type hdr;
  uint8 delay_secs;
} sys_m_shutdown_delay_req_type;

typedef PACKED struct PACKED_POST
{
  byte xx_cmd_code;
  word mode;           
}
sys_m_diag_control_req_type; 

typedef PACK(struct) 
{
  diagpkt_subsys_header_type hdr;
  uint8 h_time_sec;
} sys_m_hibernation_req_type;

static timer_type       sysmdiagTimer;
static timer_group_type sysmdiagTGroup;

/*===========================================================================

FUNCTION SYS_M_SHUTDOWN_DELAY_DIAG

DESCRIPTION
  This callback is registered with diag (send_data 75 37 11 16 <delay>)
  When executed it will delay shutdown completion by <delay> seconds.
  This facilitates automated testing by allowing additional time for diag
  packets to be flushed.

===========================================================================*/
PACK(void *) sys_m_shutdown_delay_diag (PACK(void *) req_ptr, uint16 pkt_len)
{
  sys_m_shutdown_delay_req_type *req;

  /* build response */
  tms_diag_send_generic_response(req_ptr);

  // decode request
  req = (sys_m_shutdown_delay_req_type *) req_ptr;

  // default parameters
  if (pkt_len <= sizeof(req->hdr)) 
  {
    // no delay given so clear it
    sys_m_shutdown_delay_secs = 0;
  }
  else
  {
    sys_m_shutdown_delay_secs = (req->delay_secs);
  }

  return(NULL);
}

/** sys_m_hibernation_diag_cmd_handler
 * Handler for HIBERNATION DIAG CMD from QXDM.
 * CMD: send_data 75 37 14 16 00
 */
PACK(void *) sys_m_hibernation_diag_cmd_handler (PACK(void *) req_ptr, uint16 pkt_len)
{
  sys_m_hibernation_req_type *req = (sys_m_hibernation_req_type *)req_ptr;
  
  tms_diag_send_generic_response(req_ptr);

  // SYSTEM HIBERNATION
  if(SYS_M_HIBERNATION_ERR_INVALID_H_TIME == sys_m_hibernation(req->h_time_sec * 1000))
  {
    // Invalid hibernation time error
    TMS_MSG_ERROR("sys_m_hibernation: Invalid hibernation time error.");
  }
  else
  {
    // No error
    TMS_MSG_HIGH("sys_m_hibernation: Success.");
  }

  return(NULL);
}

/**
 * @brief sys_m_diag_control_execute
 *
 * System monitor diag control callback to perfrom actual MODE 
 * command action. 
 *
 * @param[in] mode: the requested mode transition
 * 
*/
static void sys_m_diag_control_execute(timer_cb_data_type mode)
{
      if (MODE_RESET == mode) //reset
        sys_m_reset();
      else if (MODE_SHUTDOWN == mode) //shutdown
        sys_m_shutdown();
}



/**
 * @brief sys_m_diag_control
 *
 * System monitor diag handling function.  This is used to
 * receive and handle reset/poweroff requests
 *
 * @param[in] *req_pkt: the diag packet
 * @param[in] pkt_len: length of the diag packet
 * 
*/
__packed void * sys_m_diag_control (__packed void *req_pkt, uint16 pkt_len)
{
    sys_m_diag_control_req_type *req = NULL;
    sys_m_diag_control_req_type *rsp = NULL;
    const int rsp_len = sizeof (sys_m_diag_control_req_type);
    
    req = (sys_m_diag_control_req_type *) req_pkt;

    //build response
    rsp = (sys_m_diag_control_req_type *)
    diagpkt_alloc (DIAG_CONTROL_F, rsp_len);
    if (NULL != rsp)
        memscpy( (void*) rsp, rsp_len, (void*) req, pkt_len);

    if ( (req->mode == MODE_RESET)|| (req->mode == MODE_SHUTDOWN)) //shutdown
    {
        diagpkt_commit(rsp); //queue diag response

        if(sys_m_diag_timer_def_status == 0)
		{
		   sys_m_diag_timer_def_status = 1;
		   MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"SYS_M_DIAG received reset/shutdown  request");
		   
		   //set timer to trigger actual response (allows time for diag response to go out)
           timer_def_osal(&sysmdiagTimer, &sysmdiagTGroup, TIMER_FUNC1_CB_TYPE, sys_m_diag_control_execute, (timer_cb_data_type)req->mode);
           timer_set_64(&sysmdiagTimer, (timetick_type)SYS_M_DIAG_RESP_DELAY_MS, 0, T_MSEC);
		}
		else{ 
		   MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"SYS_M_DIAG received reset/shutdown request Multiple times");
		}
    }
    
    // return NULL since we either committed a response or got bad request
    return NULL;
}

/**
 * @brief sys_m_diag_control_stub
 *
 * System monitor diag handling function.  This is used to
 * receive and handle reset/poweroff requests; on fusion these
 * requests rae ignored
 *
 * @param[in] *req_pkt: the diag packet
 * @param[in] pkt_len: length of the diag packet
 * 
*/
__packed void * sys_m_diag_control_stub (__packed void *req_pkt, uint16 pkt_len)
{
    sys_m_diag_control_req_type *req = NULL;
    sys_m_diag_control_req_type *rsp = NULL;
    const int rsp_len = sizeof (sys_m_diag_control_req_type);
    
    req = (sys_m_diag_control_req_type *) req_pkt;

    rsp = (sys_m_diag_control_req_type *)
        diagpkt_alloc (DIAG_CONTROL_F, rsp_len);
        
    if (rsp != NULL)
        memscpy( (void*) rsp, rsp_len, (void*) req, pkt_len);
    return rsp;
}

static const diagpkt_user_table_entry_type sys_m_tbl[] = {
  {DIAG_CONTROL_F, DIAG_CONTROL_F, sys_m_diag_control}
}; 
static const diagpkt_user_table_entry_type sys_m_tbl_stub[] = {
  {DIAG_CONTROL_F, DIAG_CONTROL_F, sys_m_diag_control_stub}
}; 
static diagpkt_user_table_entry_type sys_m_tbl_diag[2] = {0}; 


/**
 * @brief sys_m_diag_init
 *
 * Initializes System monitor diag handling.  
 *
 * 
*/
void sys_m_diag_init (void)
{
    if (DalPlatformInfo_IsFusion())
    {
        //in fusion configuration, want to receive mode reset but do not act on it
        DIAGPKT_DISPATCH_TABLE_REGISTER_PROC(DIAG_APP_PROC, DIAGPKT_NO_SUBSYS_ID, sys_m_tbl_stub);
    }
    else 
    {
        DIAGPKT_DISPATCH_TABLE_REGISTER_PROC(DIAG_APP_PROC, DIAGPKT_NO_SUBSYS_ID, sys_m_tbl);
    }
    sys_m_tbl_diag[0] = tms_diag_build_table_entry(TMS_DIAGPKT_SYS_M_SHUTDOWN_DELAY, sys_m_shutdown_delay_diag);
    sys_m_tbl_diag[1] = tms_diag_build_table_entry(TMS_DIAGPKT_SYS_M_HIBERNATION, sys_m_hibernation_diag_cmd_handler);
    DIAGPKT_DISPATCH_TABLE_REGISTER_PROC(DIAG_APP_PROC, DIAG_SUBSYS_DEBUG, sys_m_tbl_diag);

    //initialize timer group to be safe
    memset(&sysmdiagTGroup, 0, sizeof(sysmdiagTGroup)); 
  
}
