/*==============================================================================

  High Speed USB \ Health Monitoring Module
  
  GENERAL DESCRIPTION
    

  EXTERNALIZED FUNCTIONS
   

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.

  Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/hsu_diag/hsu_health_diag.c#1 $
  $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  12/15/10  cy      


==============================================================================*/

/*==============================================================================
                            INCLUDE FILES FOR MODULE
==============================================================================*/

#include "comdef.h"

#include "hsu_health_diag.h"
#include "hsu_health_diag_init.h"

#ifndef USB_THREADX
#include "diag_v.h"
#include "diagcmd.h"
#include "diagi_v.h"
#include "diagpkt.h"

#include "diagdiag_v.h"
#include "diagtarget.h"

#include "diagcomm_v.h"
#include "diagbuf_v.h"
#endif
#include "event.h"
#include "err.h"


#include "assert.h"
#include <stdlib.h> 

#include "msg.h"
#include "jtypes_int.h"
 
#include "hsu_conf_sel_types_int.h"
#include "hsu_conf_sel_i.h"
#ifdef FEATURE_HS_USB_BAM
#include "hsu_bam_sm.h"
#endif /* FEATURE_HS_USB_BAM */
#include "hsu_log.h"
#include "hsu_core_config_int.h"
#include "hsu_mdm_apis.h"
#include "hsu_platform.h"
#include "hsu_al_task.h"
#include "rdevmap.h"

#ifdef FEATURE_HS_USB_HEALTH_MONITOR

#ifdef FEATURE_HS_USB_BAM
#pragma pack(1)
typedef struct
{
  diagpkt_subsys_header_type header; /* Sub System header */
  uint32 event;
} hsu_diag_bam_sm_send_event_req;

#pragma pack(1)
typedef struct
{
  diagpkt_subsys_header_type header; /* Sub System header */
  uint8 error_code;
} hsu_diag_bam_sm_send_event_rsp;

#endif /* FEATURE_HS_USB_BAM */

#pragma pack(1)
typedef struct
{
  diagpkt_subsys_header_type header; /* Sub System header */
  uint16 flag;
} hsu_vdd_min_req;

#pragma pack(1)
typedef struct
{
  diagpkt_subsys_header_type header; /* Sub System header */
  uint8 error_code;
} hsu_vdd_min_rsp;

#pragma pack(1)
typedef struct
{
  diagpkt_subsys_header_type header; /* Sub System header */
} hsu_counters_req;

#pragma pack(1)
typedef struct
{
  diagpkt_subsys_header_type header; /* Sub System header */
  hsu_counters_type counts;
} hsu_counters_rsp;


typedef enum {
  HSU_HELP_INFO       = 0,
  HSU_USB_CORE_INFO   = 1,
  HSU_DUN_LOG_INFO    = 2,
  HSU_TUNE_INFO       = 3,
#ifdef FEATURE_HS_USB_THREAD_PROFILE
  HSU_THREAD_PROFILE    = 4,
  HSU_THREAD_PROFILE_TIMER_SETTING = 5,
#endif
  HSU_DIAG_CLOSE_OPEN = 10,
} hsu_info_type;

#ifdef FEATURE_HS_USB_THREAD_PROFILE
static void hsu_thread_profile_update_sample_time(uint8 update_time);
static void hsu_thread_profile_process_arg(uint8 update_time);


typedef enum{
  PROFILE_START_SNAPSHOT,
  PROFILE_START_PERIODIC,
  PROFILE_END,
  PROFILE_PRINT_ALL_ZERO_FILTERED,
  PROFILE_PRINT_AVG_ZERO_FILTERED,
  PROFILE_PRINT_ALL,
  PROFILE_PRINT_AVG,
  PROFILE_PRINT_ULOG,
} hsu_thread_profile_arg_type;
#endif

PACK(void *) hsu_vdd_min_handler(
  PACK(void *) req_pkt,
  uint16 pkt_len
)
{
  PACK(void *) rsp = NULL;
  
  //hsu_vdd_min_req * req_ptr = NULL;
  hsu_vdd_min_rsp * rsp_ptr = NULL;
  diagpkt_subsys_cmd_code_type cmd_code;
  
  //req_ptr = (hsu_vdd_min_req *) req_pkt;

  //HSU_DIAG_MSG_HIGH_1("hsu_vdd_min_handler: recieved flag = %u", req_ptr->flag);

  cmd_code = diagpkt_subsys_get_cmd_code (req_pkt);

  rsp = diagpkt_subsys_alloc(
    DIAG_SUBSYS_COREBSP, cmd_code, sizeof(hsu_vdd_min_rsp));

  //hsu_vddmin_log_print_f3();  

  rsp_ptr = (hsu_vdd_min_rsp *) rsp;

  if (rsp_ptr != NULL)
  {
    /* return success result */
    rsp_ptr->error_code = 0;
  }
  
  return (rsp);
}

/*===========================================================================

FUNCTION : hsu_info_handler

DESCRIPTION
   

RETURN VALUE
 

============================================================================*/
#pragma pack(1)
  
  typedef struct {
    diagpkt_subsys_header_type header; /* Sub System header */
    uint16 flag;
  } hsu_info_req;
  
#pragma pack(1)
  
  typedef struct {
    diagpkt_subsys_header_type header; /* Sub System header */
    uint8 error_code;
  } hsu_info_rsp;


void hsu_rdm_assign_diag_cb
(
  rdm_assign_status_type status,
  rdm_service_enum_type service,
  rdm_device_enum_type device
)
{
  HSU_ULOG_1(HIGH_MSG, DATA_LOG, "DIAG, rdm_assign_cb, status=%u", status);
}


void hsu_rdm_assign_port(void *param)
{
  HSU_ULOG(HIGH_MSG, DATA_LOG, "DIAG, rdm_asiign");

  rdm_assign_port(
    RDM_DIAG_SRVC, 
    RDM_USB_DIAG_DEV, 
    hsu_rdm_assign_diag_cb);
}


void hsu_rdm_close_diag_cb
(
  rdm_assign_status_type status,
  rdm_service_enum_type service,
  rdm_device_enum_type device
)
{
  HSU_ULOG_1(HIGH_MSG, DATA_LOG, "DIAG, rdm_close_cb, status=%u", status);

  hsu_al_task_enqueue_cmd(hsu_rdm_assign_port, NULL);
}


void hsu_diag_close_and_open(void)
{
  HSU_ULOG(HIGH_MSG, DATA_LOG, "DIAG, rdm_close");
  #ifdef CI_MERGE
  rdm_close_device(
    RDM_DIAG_SRVC, 
    RDM_USB_DIAG_DEV, 
    hsu_rdm_close_diag_cb);

  #endif
}


PACK(void *) hsu_info_handler(
  PACK(void *) req_pkt,
  uint16 pkt_len
)
{
  PACK(void *) rsp = NULL;
  
  hsu_info_req * req_ptr = NULL;
  hsu_info_rsp * rsp_ptr = NULL;
  diagpkt_subsys_cmd_code_type cmd_code;
  uint8 cmd_type;
#ifdef FEATURE_HS_USB_THREAD_PROFILE      
  uint8 cmd_arg;
#endif      

  req_ptr = (hsu_info_req *) req_pkt;

  HSU_ASSERT(req_pkt);
  HSU_DIAG_MSG_HIGH_1("hsu_usb_core_info_handler: recieved flag = %u", req_ptr->flag);

  cmd_code = diagpkt_subsys_get_cmd_code (req_pkt);

  rsp = diagpkt_subsys_alloc(
    DIAG_SUBSYS_COREBSP, cmd_code, sizeof(hsu_info_rsp));

  cmd_type = (req_ptr->flag)&0x00FF;     //this is the first byte (A):  ex: 75 75 213 8 A B;
#ifdef FEATURE_HS_USB_THREAD_PROFILE      
  cmd_arg = ((req_ptr->flag)&0xFF00)>>8; //this is the second byte (B): ex: 75 75 213 8 A B;  
#endif
  
  switch(cmd_type)
  {
    case HSU_HELP_INFO:
      HSU_DIAG_MSG_HIGH("send_data 75 75 213 8 0 0:  Prints this help message");
      HSU_DIAG_MSG_HIGH("send_data 75 75 213 8 1 0:  Prints USB core info");
      HSU_DIAG_MSG_HIGH("send_data 75 75 213 8 2 0:  Prints DUN logs");
      HSU_DIAG_MSG_HIGH("send_data 75 75 213 8 3 0:  Print TUNE value");

#ifdef FEATURE_HS_USB_THREAD_PROFILE      
      HSU_DIAG_MSG_HIGH("send_data 75 75 213 8 4 0:  Start cpu profile snap shot");
      HSU_DIAG_MSG_HIGH("                      4 1:  Start cpu periodic profile");
      HSU_DIAG_MSG_HIGH("                      4 2:  End cpu periodic profile");
      HSU_DIAG_MSG_HIGH("                      4 3:  print all cpu profile(filter zero)");
      HSU_DIAG_MSG_HIGH("                      4 4:  print avg cpu profile(filter zero)");
      HSU_DIAG_MSG_HIGH("                      4 99: print help msg for cpu profile");
      HSU_DIAG_MSG_HIGH("send_data 75 75 213 8 5 X:  change cpu profile sample period to X*100 ms");
#endif      
    break;

    case HSU_USB_CORE_INFO:
      hsu_usb_core_info_log_print_f3();
    break;

    case HSU_DUN_LOG_INFO:
      hsu_print_dun_log();
    break;

    case HSU_TUNE_INFO:
    //  hsu_api_print_qusb2_phy_tune_value();
    break;

#ifdef FEATURE_HS_USB_THREAD_PROFILE
    case HSU_THREAD_PROFILE:
      hsu_thread_profile_process_arg(cmd_arg);
    break;

    case HSU_THREAD_PROFILE_TIMER_SETTING:
      hsu_thread_profile_update_sample_time(cmd_arg);
    break;
#endif    

    default:
#ifdef FEATURE_HS_USB_THREAD_PROFILE
      HSU_DIAG_MSG_HIGH_2("hsu_info_handler: unknown cmd = %u, arg = %u", 
        cmd_type, cmd_arg);
#else // ~FEATURE_HS_USB_THREAD_PROFILE
      HSU_DIAG_MSG_HIGH_1("hsu_info_handler: unknown cmd = %u", 
        cmd_type);
#endif // ~FEATURE_HS_USB_THREAD_PROFILE    
    break;
  }

  rsp_ptr = (hsu_info_rsp *) rsp;

  if (rsp_ptr != NULL)
  {
    /* return success result */
    rsp_ptr->error_code = 0;
  }
  
  return (rsp);
}

#ifdef FEATURE_HS_USB_BAM_RESET
/*===========================================================================

FUNCTION hsu_comp_switch_handler

DESCRIPTION
   

RETURN VALUE
 

============================================================================*/
PACK(void *) hsu_diag_bam_sm_send_event (
  PACK(void *) req_pkt,
  uint16 pkt_len
)
{
  static uint32 diag_last_event = 0;
  hsu_diag_bam_sm_send_event_rsp * rsp;
  hsu_diag_bam_sm_send_event_req * req_ptr;
  diagpkt_subsys_cmd_code_type cmd_code;
  uint32 event;

  req_ptr = (hsu_diag_bam_sm_send_event_req *) req_pkt;
  event = req_ptr->event;
  cmd_code = diagpkt_subsys_get_cmd_code (req_pkt);

  if (diag_last_event != event)
  {
    hsu_bam_sm_send_sig_evt((hsu_bam_sig_evt_type)event);
  }
  diag_last_event = event;

  rsp = (hsu_diag_bam_sm_send_event_rsp *)diagpkt_subsys_alloc(
                                    DIAG_SUBSYS_COREBSP, 
                                    cmd_code, 
                                    sizeof(hsu_diag_bam_sm_send_event_rsp));
  HSU_ASSERT(rsp);
  rsp->error_code = 0;
  return (rsp);
}
#endif /* FEATURE_HS_USB_BAM_RESET */

PACK(void *) hsu_counters_handler (
  PACK(void *) req_pkt,
  uint16 pkt_len
)
{
  hsu_counters_rsp * rsp;
  diagpkt_subsys_cmd_code_type cmd_code;
  uint32 core_id;

  cmd_code = diagpkt_subsys_get_cmd_code (req_pkt);


  rsp = (hsu_counters_rsp *)diagpkt_subsys_alloc(
                                    DIAG_SUBSYS_COREBSP, 
                                    cmd_code, 
                                    sizeof(hsu_counters_rsp));
  HSU_ASSERT(rsp);
  
  // Invoke function to gether relevant information
  core_id = (hsu_conf_sel_is_rmnet_over_secondary_core() ? HSU_CORE_IDX_SECONDARY : HSU_CORE_IDX_PRIMARY);

  rsp->counts.suspend_count = hsu_get_event_total_cnt(EVT_BUS_SUSPEND, core_id);
  rsp->counts.resume_count = hsu_get_event_total_cnt(EVT_BUS_RESUME, core_id);
  rsp->counts.bus_reset_count = hsu_get_event_total_cnt(EVT_BUS_RESET, core_id);
  rsp->counts.remote_wakeup_count = hsu_get_event_total_cnt(EVT_REMOTE_WAKEUP, core_id);
  rsp->counts.xo_shutdown_count   = hsu_platform_get_counter_xo_shutdown();
  rsp->counts.vdd_min_count       = hsu_platform_get_counter_xosd_with_vddmin();

  HSU_DIAG_MSG_HIGH_1("======= USB Power Event Counters (core index = %u) ===================================", core_id);
  HSU_DIAG_MSG_HIGH_3("suspend       : %-8u, resume      : %-8u, reset        : %-8u", rsp->counts.suspend_count, rsp->counts.resume_count, rsp->counts.bus_reset_count);
  HSU_DIAG_MSG_HIGH_3("remote wakeup : %-8u, xo shutdown : %-8u, xo_sd+vddmin : %-8u", rsp->counts.remote_wakeup_count, rsp->counts.xo_shutdown_count, rsp->counts.vdd_min_count);  
  HSU_DIAG_MSG_HIGH(  "======= End of USB Power Event Counters ==============================================");

  return (rsp);
}

static const diagpkt_user_table_entry_type hsu_subsys_common_tbl[] =
{
#ifdef FEATURE_HS_USB_BAM_RESET
  {HSUSB_DIAG_BAM_SM_SEND_EVENT, HSUSB_DIAG_BAM_SM_SEND_EVENT, hsu_diag_bam_sm_send_event},
#endif /* FEATURE_HS_USB_BAM_RESET */
  {HSUSB_DIAG_VDD_MIN, HSUSB_DIAG_VDD_MIN, hsu_vdd_min_handler},
  {HSUSB_DIAG_INFO, HSUSB_DIAG_INFO, hsu_info_handler},
  {HSUSB_DIAG_COUNTERS, HSUSB_DIAG_COUNTERS, hsu_counters_handler},
};

#ifdef FEATURE_HS_USB_THREAD_PROFILE
static hsu_thread_profile_ctx_type hsu_thread_profile_ctx = 
{
  /* profile_started         */  FALSE,
  /* start_time              */  0,
  /* profile_stop_timer      */  NULL,
  /* profile_hist_idx        */  0,
  /* profile_sample_freq     */  HSU_THREAD_PROFILE_DEFAULT_FREQUENCY,
  /* sleep_thread_idx        */  0,
  /* periodic_flag           */  FALSE,
  /* profile_hist            */  {0,}
};


static void hsu_thread_profile_print_all(boolean filter)
{
  uint32 total_thread = rex_get_thread_total();
  uint32 thread_idx, hist_idx, diag_idx;
  thread_ctx_type* thread_ctx;
  
  for (thread_idx=0; thread_idx< total_thread; thread_idx++)
  {
    thread_ctx = rex_get_thread_ctx(thread_idx);
    if ((NULL == thread_ctx) || (NULL == thread_ctx->thread_name))
    {
      HSU_DIAG_MSG_ERROR_1("hsu_profile_print_all, rex_get_thread_ctx is null, idx %u", thread_idx);
    }
    else
    {
      if (hsu_thread_profile_ctx.profile_hist[thread_idx][HSU_THREAD_PROFILE_MAX_HIST] || filter == FALSE)
      {
        HSU_DIAG_SPRINTF_3("%12s, avg  %4u, max  %4u", thread_ctx->thread_name, 
          hsu_thread_profile_ctx.profile_hist[thread_idx][HSU_THREAD_PROFILE_AVG], 
          hsu_thread_profile_ctx.profile_hist[thread_idx][HSU_THREAD_PROFILE_MAX]);

        for (diag_idx=0, hist_idx = hsu_thread_profile_ctx.profile_hist_idx; diag_idx <HSU_THREAD_PROFILE_MAX_DIAG_PRINT; diag_idx++)
        {
          HSU_DIAG_MSG_HIGH_3("     %u, %u, %u,", hsu_thread_profile_ctx.profile_hist[thread_idx][(hist_idx)%HSU_THREAD_PROFILE_MAX_HIST], 
                                             hsu_thread_profile_ctx.profile_hist[thread_idx][((hist_idx+1)%HSU_THREAD_PROFILE_MAX_HIST)],
                                             hsu_thread_profile_ctx.profile_hist[thread_idx][((hist_idx+2)%HSU_THREAD_PROFILE_MAX_HIST)]);
          hist_idx = ((hist_idx+3)%HSU_THREAD_PROFILE_MAX_HIST);
        } 
        if (HSU_THREAD_PROFILE_REMAIN_DIAG_PRINT == 2)
        {
          HSU_DIAG_MSG_HIGH_2("     %u, %u", hsu_thread_profile_ctx.profile_hist[thread_idx][hist_idx], 
                                        hsu_thread_profile_ctx.profile_hist[thread_idx][((hist_idx+1)%HSU_THREAD_PROFILE_MAX_HIST)]);
          hist_idx = ((hist_idx+2)%HSU_THREAD_PROFILE_MAX_HIST);  
        }
        else if (HSU_THREAD_PROFILE_REMAIN_DIAG_PRINT == 1)
        {
          HSU_DIAG_MSG_HIGH_1("     %u", hsu_thread_profile_ctx.profile_hist[thread_idx][hist_idx]);
          hist_idx = ((hist_idx+1)%HSU_THREAD_PROFILE_MAX_HIST);
        }
      }
    }
  }

}

static void hsu_thread_profile_print_avg(boolean filter)
{
  uint32 total_thread = rex_get_thread_total();
  uint32 thread_idx;
  thread_ctx_type* thread_ctx;

  HSU_DIAG_MSG_HIGH(" THREAD_NAME,       avg,      max, (10000 percent)");
  
  for (thread_idx=0; thread_idx< total_thread; thread_idx++)
  {
    thread_ctx = rex_get_thread_ctx(thread_idx);
    if ((NULL == thread_ctx) || (NULL == thread_ctx->thread_name))
    {
      HSU_DIAG_MSG_ERROR_1("hsu_profile_start, rex_get_thread_ctx is null, idx %u", thread_idx);
    }
    else
    {
      if (hsu_thread_profile_ctx.profile_hist[thread_idx][HSU_THREAD_PROFILE_MAX_HIST] || (filter == FALSE))
      {
        HSU_DIAG_SPRINTF_3("%12s, avg  %4u, max  %4u", thread_ctx->thread_name, 
          hsu_thread_profile_ctx.profile_hist[thread_idx][HSU_THREAD_PROFILE_AVG], 
          hsu_thread_profile_ctx.profile_hist[thread_idx][HSU_THREAD_PROFILE_MAX]);
      }
    }
  }
}

static void hsu_thread_profile_cal_avg(void)
{
  uint32 total_thread = rex_get_thread_total();
  uint32 thread_idx, hist_idx;
  uint32 thread_running_total;
  uint32 max_value;
  for (thread_idx=0; thread_idx< total_thread; thread_idx++)
  {
    thread_running_total=0;    
    max_value =0;
    for (hist_idx =0; hist_idx< HSU_THREAD_PROFILE_MAX_HIST; hist_idx++)
    {
      hsu_thread_profile_ctx.profile_hist[thread_idx][hist_idx] = 
        (hsu_thread_profile_ctx.profile_hist[thread_idx][hist_idx]/hsu_thread_profile_ctx.profile_hist[HSU_THREAD_PROFILE_TOTAL_TIME][hist_idx]); //update entry from timetick to avg cpu usage
      thread_running_total = thread_running_total + hsu_thread_profile_ctx.profile_hist[thread_idx][hist_idx]; 
      if (max_value < hsu_thread_profile_ctx.profile_hist[thread_idx][hist_idx])
      {
        max_value = hsu_thread_profile_ctx.profile_hist[thread_idx][hist_idx];
      }
      
    }
    hsu_thread_profile_ctx.profile_hist[thread_idx][HSU_THREAD_PROFILE_AVG] = thread_running_total/HSU_THREAD_PROFILE_MAX_HIST;  //Avg cpu usage over all sample
    hsu_thread_profile_ctx.profile_hist[thread_idx][HSU_THREAD_PROFILE_MAX] = max_value;  //Avg cpu usage over all sample
  }
}


static void hsu_thread_profile_start(void)
{
  uint32 total_thread = rex_get_thread_total();
  thread_ctx_type* thread_ctx;
  uint32 thread_idx;
  if (hsu_thread_profile_ctx.profile_started)
  {
    hsu_timer_cancel(&(hsu_thread_profile_ctx.profile_stop_timer));
  }
  hsu_thread_profile_ctx.profile_started = TRUE;
  hsu_thread_profile_ctx.start_time = KxProfile_GetTotalTime();
  
  hsu_timer_set(
    &(hsu_thread_profile_ctx.profile_stop_timer), hsu_thread_profile_ctx.profile_sample_freq, T_MSEC);
  
  for (thread_idx=0; thread_idx< total_thread; thread_idx++)
  {
    thread_ctx = rex_get_thread_ctx(thread_idx);
    if ((NULL == thread_ctx) || (NULL == thread_ctx->thread_name))
    {
      HSU_DIAG_MSG_ERROR_1("hsu_profile_start, rex_get_thread_ctx is null, idx %u", thread_idx);
    }
    else
    {
      if ((hsu_thread_profile_ctx.sleep_thread_idx ==0) && (strncmp(thread_ctx->thread_name, "sleep", 6)==0))
      {
         hsu_thread_profile_ctx.sleep_thread_idx = thread_idx;
         //HSU_DIAG_SPRINTF_3("sleep_thread_idx %u, %s, ID %u", thread_idx, thread_ctx->thread_name, thread_ctx->thread_id);
      }
      KxProfile_ClearThreadTime(thread_ctx->thread_id);
    }
  }

}

static void hsu_thread_profile_timer_cb(unsigned long arg)
{
  uint32 total_thread = rex_get_thread_total();
  uint32 thread_idx;
  thread_ctx_type* thread_ctx;

  if (hsu_thread_profile_ctx.profile_hist_idx >= HSU_THREAD_PROFILE_MAX_HIST)
  {
    HSU_DIAG_MSG_ERROR_1("hsu_profile_end, overflow the index, idx %u", hsu_thread_profile_ctx.profile_hist_idx);
    return;
  }

  if (hsu_thread_profile_ctx.profile_started)
  {

    hsu_thread_profile_ctx.profile_hist[HSU_THREAD_PROFILE_TOTAL_TIME][hsu_thread_profile_ctx.profile_hist_idx]
       = (KxProfile_GetTotalTime()-hsu_thread_profile_ctx.start_time)/HSU_THREAD_PROFILE_RESOLUTION;
    
    for (thread_idx=0; thread_idx< total_thread; thread_idx++)
    {
       thread_ctx = rex_get_thread_ctx(thread_idx);
       if (thread_ctx == NULL)
       {
         HSU_DIAG_MSG_ERROR_1("hsu_profile_end, rex_get_thread_ctx is null, idx %u", thread_idx);
       }
       else
       {
         hsu_thread_profile_ctx.profile_hist[thread_idx][hsu_thread_profile_ctx.profile_hist_idx]
          = KxProfile_GetThreadTime(thread_ctx->thread_id);
       }
    }
    hsu_thread_profile_ctx.profile_started = FALSE;
    hsu_thread_profile_ctx.profile_hist_idx++;
  }

  // If this is last sample, calculate the average.
  if (hsu_thread_profile_ctx.periodic_flag || (hsu_thread_profile_ctx.profile_hist_idx < HSU_THREAD_PROFILE_MAX_HIST))
  {
    if (hsu_thread_profile_ctx.profile_hist_idx >= HSU_THREAD_PROFILE_MAX_HIST)
    {
      hsu_thread_profile_ctx.profile_hist_idx = 0;
    }
    hsu_thread_profile_start();
  }
  else
  {
    hsu_thread_profile_cal_avg();

    HSU_DIAG_MSG_HIGH_3("thread_profile_end2, profile_period %u (ms), num_hist %u, avg_cpu_usage %u (10000%%)", 
      hsu_thread_profile_ctx.profile_sample_freq, HSU_THREAD_PROFILE_MAX_HIST, 
      (10000-hsu_thread_profile_ctx.profile_hist[hsu_thread_profile_ctx.sleep_thread_idx][HSU_THREAD_PROFILE_MAX_HIST])); //none sleeping time is CPU usage
    hsu_thread_profile_print_avg(TRUE);
    HSU_DIAG_MSG_HIGH_1("=======hsu_thread_profile END, Time %u =============", KxProfile_GetTotalTime());
  }
  
}

static void hsu_thread_profile_update_sample_time(uint8 update_time)
{
  HSU_DIAG_MSG_HIGH_1("=======DIAG_CMD_5: Arg %u====================", update_time);
  if (update_time == 0)
  {
    hsu_thread_profile_ctx.profile_sample_freq = 10;
  }
  else
  {
    hsu_thread_profile_ctx.profile_sample_freq = update_time * 100;
  }
  HSU_DIAG_MSG_HIGH_1("=======DIAG_CMD_5: END, sample_time %u ms ==============", hsu_thread_profile_ctx.profile_sample_freq);
}


static void hsu_thread_profile_process_arg(uint8 arg)
{ 
  HSU_DIAG_MSG_HIGH_1("=======DIAG_CMD_4: Arg %u====================", arg);
  HSU_DIAG_MSG_HIGH_1("hsu_thread_profile_process_arg, Curr Time %u", KxProfile_GetTotalTime());

  switch (arg)
  {
    case PROFILE_START_SNAPSHOT:
      HSU_DIAG_MSG_HIGH("=== PROFILE_START_SNAPSHOT ===");
      HSU_DIAG_MSG_HIGH("send_data 75 75 213 8 4 99 for HELP msg");
      HSU_DIAG_MSG_HIGH_3("Curr_clk %u, profile_period %u (ms), num_hist %u", 
        KxProfile_GetTotalTime(), hsu_thread_profile_ctx.profile_sample_freq, HSU_THREAD_PROFILE_MAX_HIST);
      
      hsu_thread_profile_ctx.profile_hist_idx =0;
      hsu_thread_profile_ctx.periodic_flag =FALSE;
      hsu_thread_profile_ctx.sleep_thread_idx=0;
      hsu_thread_profile_start();
    break;
    
    case PROFILE_START_PERIODIC:
      HSU_DIAG_MSG_HIGH("=== PROFILE_START_PERIODIC ===");
      HSU_DIAG_MSG_HIGH_2("Curr_clk %u, profile_period %u (ms)", 
        KxProfile_GetTotalTime(), hsu_thread_profile_ctx.profile_sample_freq);
      
      hsu_thread_profile_ctx.profile_hist_idx =0;
      hsu_thread_profile_ctx.periodic_flag =TRUE;
      hsu_thread_profile_ctx.sleep_thread_idx=0;
      hsu_thread_profile_start();
    break;
    
    case PROFILE_END:
      HSU_DIAG_MSG_HIGH("=== PROFILE_END ===");
      HSU_DIAG_MSG_HIGH_2("Curr_clk %u, profile_period %u (ms)", 
        KxProfile_GetTotalTime(), hsu_thread_profile_ctx.profile_sample_freq);
      
      if (hsu_thread_profile_ctx.profile_started)
      {
        hsu_timer_cancel(&(hsu_thread_profile_ctx.profile_stop_timer));
        hsu_thread_profile_ctx.profile_started = FALSE;
      }
      
      //if trigger by Periodic calcaluate the result.
      if (hsu_thread_profile_ctx.periodic_flag == TRUE)
      {
        hsu_thread_profile_cal_avg();
        hsu_thread_profile_ctx.periodic_flag = FALSE;  
      }

      //printing summary
      hsu_thread_profile_print_avg(TRUE);
    break;

    case PROFILE_PRINT_ALL_ZERO_FILTERED:
      HSU_DIAG_MSG_HIGH("=== PROFILE_PRINT_ALL_ZERO_FILTERED ===");
      hsu_thread_profile_print_all(TRUE);   
    break;

    case PROFILE_PRINT_AVG_ZERO_FILTERED:
      HSU_DIAG_MSG_HIGH("=== PROFILE_PRINT_AVG_ZERO_FILTERED ===");
      hsu_thread_profile_print_avg(TRUE);
    break;

    case PROFILE_PRINT_ALL:
      HSU_DIAG_MSG_HIGH("=== PROFILE_PRINT_ALL ===");
      hsu_thread_profile_print_all(FALSE);
    break;
    
    case PROFILE_PRINT_AVG:
      HSU_DIAG_MSG_HIGH("=== PROFILE_PRINT_AVG ===");
      hsu_thread_profile_print_avg(FALSE);
    break;

    case PROFILE_PRINT_ULOG:
      HSU_DIAG_MSG_HIGH("hsu_thread_start_profile: PRINT_PROFILE_ULOG NOT_IMPLEMENT");      
    break;

    default:
      HSU_DIAG_MSG_HIGH_1("hsu_thread_start_profile: unknown profile arg= %u", 
        arg);
      HSU_DIAG_MSG_HIGH("0:PROFILE_START_SNAPSHOT");
      HSU_DIAG_MSG_HIGH("1:PROFILE_START_PERIODIC");
      HSU_DIAG_MSG_HIGH("2:PROFILE_END");
      HSU_DIAG_MSG_HIGH("3:PROFILE_PRINT_ALL_ZERO_FILTERED");
      HSU_DIAG_MSG_HIGH("4:PROFILE_PRINT_AVG_ZERO_FILTERED");
      HSU_DIAG_MSG_HIGH("5:PROFILE_PRINT_ALL");
      HSU_DIAG_MSG_HIGH("6:PROFILE_PRINT_AVG");
      HSU_DIAG_MSG_HIGH("7:PROFILE_PRINT_ULOG");
    break;
  }
  HSU_DIAG_MSG_HIGH("=======DIAG_CMD_4: END====================");
}
#endif


void hsu_diag_qxdm_init (void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_COREBSP, 
        hsu_subsys_common_tbl);
  
#ifdef FEATURE_HS_USB_THREAD_PROFILE  
  hsu_thread_profile_ctx.profile_stop_timer = hsu_timer_init(
        HSU_TIMER_CID__CPU_PROFILE,
        HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
        hsu_thread_profile_timer_cb,
        0);
#endif
}

/*===========================================================================*/






/*===========================================================================*/
/* To enable thread cpu profile. do the following 4 step
1. uncommon out the feature flag FEATURE_HS_USB_THREAD_PROFILE in wiredconnectivity\hsusb\build\SConscript
2. Put the following code to Rex.h


#define HSU_THREAD_PROFILE_MAX_THREAD 64
typedef struct 
{
  uint32     thread_id;
  char*       thread_name;

} thread_ctx_type;


thread_ctx_type* rex_get_thread_ctx(uint32 idx);
uint32 rex_get_thread_total(void);



3. put the following code to rex_os_thread.c


thread_ctx_type rex_thread_array [HSU_THREAD_PROFILE_MAX_THREAD];
static uint32 rex_thread_index = 0;

static void rex_reg_thread_ctx(rex_tcb_type* thread_ctx)
{
  if(thread_ctx && (thread_ctx->thread_id!= 0) && (thread_ctx->p->task_name))
  {
    if (rex_thread_index<HSU_THREAD_PROFILE_MAX_THREAD)
    {
      rex_thread_array[rex_thread_index].thread_id = (uint32)thread_ctx->thread_id;
      rex_thread_array[rex_thread_index].thread_name = thread_ctx->p->task_name;
      rex_thread_index++;
    }
  }
  
}
thread_ctx_type* rex_get_thread_ctx(uint32 idx)
{
  if(idx<rex_thread_index)
  {
    return &rex_thread_array[idx];
  }
  else
  {
    return NULL;   //RETURN the last reg thread
  }
}
uint32 rex_get_thread_total(void)
{
  return rex_thread_index;
}



4. add 
  rex_reg_thread_ctx(p_tcb); 
at the beginning of rex_os_thread_entry_func in rex_os_thread.c like below


void * rex_os_thread_entry_func(void *param)
{
   rex_tcb_type *p_tcb = (rex_tcb_type *)param;
   rex_os_tcb_type *p_os_tcb = NULL;

   rex_reg_thread_ctx(p_tcb);  //hsu_cpu_profile
   ...
}


*/










#endif /*FEATURE_HS_USB_HEALTH_MONITOR */
