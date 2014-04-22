/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC TASK
***
***
*** DESCRIPTION
***
***  Handles the initialisation of the MAC task and the signal reception
***  handler.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //components/rel/geran.mpss/5.2.0/gmac/static/gmac_static_task.h#7 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 05/01/18   asr     Created
***
*****************************************************************************/

#ifndef INC_GMAC_STATIC_TASK_H
#define INC_GMAC_STATIC_TASK_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "gmacsigl1.h"
#include "gmacsigl1_g.h"
#include "geran_tasks.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "log.h" 
#include "rex.h"

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/* The timer id used to in rex_def_timer_ex is 32 bits. Consists of:
   MSB
   octet 4 - not used
   octet 3 - not used
   octet 2 - gas_id
   octet 1 - Timer Id from mac_mac_sig_id_t - range 0x80-0xff
   LSB*/

#define SET_REX_TIMER(timer_id) ((uint32)timer_id)

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/
typedef void (*gprs_mac_signal_handler_t)(void);
typedef void (*gmac_store_trace_send_msg_buf_t)(uint8, void*);
typedef void (*gprs_l1_mac_acc_func_t)(mac_l1_sig_t*);
typedef void (*gmac_start_t3166_t)(void);
typedef void (*new_gmac_trans_calc_e_pdan_size_t)(egprs_chn_qual_rpt_t*,uint16*, uint16*, boolean*);
#ifndef FEATURE_GERAN_REDUCED_DEBUG
typedef log_on_demand_status_enum_type (*mac_log_on_demand_t)(log_code_type log_code);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
typedef boolean (*gprs_odm_info_is_assigned_tbf_egprs_t)(void);

extern void mac_timer_callback(const uint32 timer_id);
extern void gmac_static_timer_callback(const uint32 timer_id);
extern void gprs_mac_signal_handler_dyn(void);
extern void gmac_store_trace_send_msg_buf_dyn(uint8 message_set, void* message_ptr);
extern void gprs_l1_mac_acc_func_dyn(mac_l1_sig_t *common_msg_ptr);
extern void gmac_start_t3166_dyn(void);
extern void new_gmac_trans_calc_e_pdan_size_dyn( egprs_chn_qual_rpt_t *egprs_qual_rpt,
                                                 uint16 *size_with_qaul,
                                                 uint16 *size_without_qual,
                                                 boolean  *chan_req_present
                                               );
#ifndef FEATURE_GERAN_REDUCED_DEBUG
extern log_on_demand_status_enum_type mac_log_on_demand_dyn(log_code_type log_code);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
extern boolean gprs_odm_info_is_assigned_tbf_egprs_dyn(void);

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
/* Static API fptr struct*/
typedef struct
{
  gprs_mac_signal_handler_t        gprs_mac_signal_handler;
  gmac_store_trace_send_msg_buf_t  gmac_store_trace_send_msg_buf;
  gprs_l1_mac_acc_func_t           gprs_l1_mac_acc_fnc;
  gmac_start_t3166_t               start_t3166;
  new_gmac_trans_calc_e_pdan_size_t gmac_trans_calc_e_pdan_size;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  mac_log_on_demand_t               mac_log_on_demand;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
  gprs_odm_info_is_assigned_tbf_egprs_t gprs_odm_info_is_assigned_tbf_egprs;
} geran_static_gmac_ext_api_s;
#endif /*FEATURE_EXTERNAL_PAGING_GERAN*/

#endif /* INC_GMAC_STATIC_TASK_H */
/*** EOF: don't remove! ***/
