#ifndef UIMGLOBALS_H
#define UIMGLOBALS_H
/*===========================================================================

                            I N T E R N A L   U I M
                       G L O B A L   D E F I N I T I O N S

DESCRIPTION
  This contains all the global definitions for UIM

Copyright (c) 2013 - 2020 by QUALCOMM Technologies, Inc. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/uimglobals.h#2 $
$DateTime: 2020/08/12 08:04:16 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/28/20   rkg     Enable suspend resume feature
03/06/19   vgd     GET EID for M2M cards where LPA is not supported
02/20/19   ng      Delay local card power up in Modem SSR in BTSAP mode
10/12/18   ku      FR�49111: Reducing RCInit Timeline
03/20/17   gm      Added extra bytes to prevent uart buffer overflow
10/14/16   gm      Added support for Poll interval negotiation
10/03/16   ks      Polling alignment with eDRX
06/08/16   gm      Remove the test code of Simulate Null byte
06/03/16   ks      Remove the feature flag UIM_TEMP_DISABLED_POLL_FR
05/19/16   sam     Remove EFS logging feature
                   Remove unused legacy NVs
05/09/16   sam     Queue a open channel command internally after terminal profile
04/19/16   ssr     Fix compilation error
03/09/16   vdc     Removed JCDMA related code
12/01/15   ll      Removing compile time disable polling feature flag  
11/23/15   ks      Recovery upon failure of close channel 
                   where ARAM app is selected
11/05/15   ks      Rel12 polling enhancement
10/16/15   sam     Added support to queue Explicit ADF selection
04/27/15   vr      Support to check the pending proactive command on card
02/28/15   ks      Explicit MF selection logic revisited
01/23/15   ks      Reselect MF after Store ESN commands
11/17/14   nmb     Remove dead directory maintenance code
11/10/14   akv     Support for user to customize LDO settings
09/29/14   ks      Attempting power-up with 3v after few recoveries
09/18/14   akv     UIMDRV UT enhancements
09/10/14   ks      Allowing AUTH command even though the 7FFF selection fails
08/28/14   akv     UIMDRV-QMI interactions - Indications and acks for LDO events
08/14/14   ks      Hotswap enhancement for flaky mechanical switches
08/14/14   sam     Support for extended recovery
07/25/14   na      Allowing back to back silent recovery request
07/25/14   lxu     Read cmd_rsp_time from nv for usb mode, give cmd_rsp_time and
                   voltage_class to ap when send usb power up to ap
07/18/14   ks      Updating the stream apdu handling
07/15/14   ll      NV refresh upon hotswap feature
05/30/14   akv     UART_DM register logging
04/16/14   lxu     Add usb init timer for waiting connection available event
04/15/14   ks      Reverting the changes of CR556030 that introduced sw wwr
04/14/14   lxu     Fixes for Hotswap in USB mode when performed in quick succession
04/11/14   na      Added support for SWP NV item to encode SWP in Terminal Capability
04/06/14   ks      Introducing software wwt running parallel to h/w wwt
03/31/14   lxu     Fix USB UICC recovery, hotswap and iso card issue
03/27/14   lxu     USB UICC modem implement
03/06/14   nmb     Directory maintenance I&T bug fixes
03/03/14   ks      Triggering recovery upon recepiton of even
                   single RX break error
02/20/14   ks      Provision for explicit MF or ADF selction
                   And provision for inverse convention bootup
02/09/14   sam     Adding variables for idle parity, RX-break and overrun errors
02/07/14   ks      Removal of mmgsdi NV dependency on ICC pup states
01/29/14   sj      ASSERT on Memory Leaks in UIMDRV
01/29/14   akv     RX ISR refactoring changes
01/14/14   nmb     Move MANAGE CHANNEL command to generic cmd processing
01/09/14   akv     Handle to vote for mss bus freq made uim instance specific
12/21/13   am      Silent Recover Feature Implementation
12/18/13   ll      Old NULL Byte timer logic clean up for SIM BUSY feature
12/12/13   ks      BTSAP in client mode, intial implementation
12/11/13   js      Fix dont_skip_PPS flag for RESET response
11/27/13   akv     Changes for voting against/for sleep on 9x35
11/11/13   js      Updated handling of task stop signal
10/24/13   akv     Removed unnecessary feature flags
10/22/13   ll      Add UIM BUSY Response feature
10/22/13   ll      Check UIM GPIOs as inputs upon timeout w/recovery disabled
10/08/13   ks      Support for clock off for BT SAP
10/07/13   ll      Disable polling for internal power measurement
09/30/13   akv     Removal of multiple efs reads performed by uim tasks
09/20/13   ak      Hotswap debug message cleanup
09/19/13   akv     Reference CLK changes to support 4.8MHz
09/16/13   yk      Removed the code for old targets and for old features
09/11/13   sam     API to enable disabling of uim recovery during run time
08/21/13   hyo/js  log_previous_char_slot_ID for APDU log in UT
08/21/13   sam     Power down UIM on technical problems through NV control
08/14/13   ks      More time proactive command handling
08/13/31   js      UIMDRV Memory De-Couple changes
08/09/13   js      INS byte need to be preserved through  the transaction
08/1/13    sam     APDU logging mask variable moved to uim_instance_global_type
07/25/13   nmb     Avoid duplicating streaming apdu commands on prop. apps
05/13/13   nmb     Initial Revision
===========================================================================*/
#include "dog.h"
#include "uimt1.h"
#include "uimi.h"
#include "uimgen.h"
#include "uim_p.h"
#include "uimcdma.h"
#include "uimgsm.h"
#include "uimisim.h"
#include "uimusim.h"
#include "uimdrv.h"
#include "uimutil.h"
#include "uim_nvcfg.h"
#include "uim_logging.h"
#include "uim_hotswap.h"
#include "uimdrv_pmic.h"
#include "ds_sys_ioctl.h"
#include "uim_simulate_null.h"
#include "msgr_msg.h"
#include "gstk_exp.h"


#ifdef FEATURE_UIM_USB_UICC
#include "uimusb.h"
#endif /* FEATURE_UIM_USB_UICC */

#ifdef FEATURE_UIM_ENABLE_UARTDM_REGISTER_LOGGING
#include "uimdrv_uartdm.h"
#endif /* FEATURE_UIM_ENABLE_UARTDM_REGISTER_LOGGING */

#define   DALCHIPINFO_FAMILY_MSM8917      71

/* As per protocol we can expect maximum of 256 data bytes, 
   along with 1 byte of ACK and 2 bytes of status words.
   So total of 259 bytes is expected from UART which is less than 65*4 bytes
*/
#define  UART_RX_BUF_MAX_SIZE                            65


typedef struct 
{
  byte     uicc_max_power_consumption;         
  byte     operator_defined_timeout; 
  boolean  increased_idle_current_required;
}uim_ef_umpc_data_type;

typedef struct
{
  uim_int_terminal_profile_data_type int_term_profile_data;
  uim_chv_struct_type     chv1;
  uim_chv_struct_type     chv2;
  uim_iccid_buf_type      iccid_buf;
  uim_ef_umpc_data_type   ef_umpc_data;
}uim_global_cache_type;

typedef struct
{
  rex_crit_sect_type clk_ctl;
#ifdef FEATURE_UIM_ENABLE_UARTDM_REGISTER_LOGGING
  rex_crit_sect_type uartdm_logging;
#endif /* FEATURE_UIM_ENABLE_UARTDM_REGISTER_LOGGING */
}uim_global_crit_sect_type;

typedef struct
{
 /* Flag to indicate whether a selection of Last selected file is needed before executing
    any part of a command */
  boolean required;
  uint8   channel;  
} uim_explicit_last_selection_type;

typedef struct
{
  uint32 msgr_init_done;
  uint32 indications_received;
  /* Debug variables to maintain total counts since power_up */
  uint32 tot_idle_bytes_count;
  uint32 tot_parity_error_count;
  uint32 tot_overrun_error_count;
  uint32 tot_rx_break_error_count;
  uint32 tot_recovery_count;
  uint32 warm_reset_count;
  uint32 rxlev_count;

  uim_log_data_type       log_data;
  byte                    log_previous_char_slot_ID;
  byte                    log_previous_char_attribute;
  uim_apdu_type           last_sent_apdu;
  uim_log_char_type       log_char;
  /* Number of times the convention (inverse/direct) is changed
     initialized as 1 */
  byte convention_change_count;

  /* A static variable to count the RXLEVs happened */
  uim_apdu_log_mask_type  apdu_log_mask;
  /* A variable to set log mask used in apdu logging*/
  uim_gpio_info_struct    uim_timeout_gpio_info[UIM_NUM_TIMEOUT_INFO];

  /* The time of simulate NULL timer in sec of UIM BUSY Response Feature testing*/
  uim_feature_busy_response_simulate_null_type uim_feature_busy_response_simulate_null;
  /* The time of simulate NULL timer in sec of UIM BUSY Response Feature testing*/
  uint32 uim_simulate_null_timer_val_s;

#ifdef FEATURE_UIM_ENABLE_UARTDM_REGISTER_LOGGING
  uim_reg_log_type     uart_reg_log;
#endif /* FEATURE_UIM_ENABLE_UARTDM_REGISTER_LOGGING */
}uim_global_debug_type;

typedef struct
{
  boolean command_in_progress;
  boolean processing_download_error;
  boolean send_all_remaining_bytes;
  /* Flag to indicate that un-known status words have been received */
  boolean bad_status_words_error;
  boolean disable_return_df_fcp_inpoll;
  boolean check_card_problem;
  boolean interface_gone_bad;

  /* Flag to indicate if an external command has been translated to an
   internal command */
  boolean internal_command_used;
  /* Variable to indicate whether a warning or error is indicated by the card */
  boolean force_get_response;
  /* Flag to indicate if the driver has encountered an overrun error */
  boolean overrun_error;
  /* Flag to indicate if the driver has maxed the parity error count */
  boolean max_parity_error;
  boolean recovery_triggered;
  /* Flag to indicate if the driver has maxed the parity error count */
  boolean max_overrun_error;
  /* Flag to indicate if the driver has maxed the parity error count */
  boolean max_rx_break_error;
  /* Boolean to indicate if SW1 status byte is received */
  boolean sw1_byte_rcvd;
  /* Boolean to indicate if SW2 status byte is received */
  boolean sw2_byte_rcvd;
  /* Boolean to indicate if the UIM response bytes are ready to be processed */
  boolean resp_bytes_being_rcvd;
  /* Flag to indicate if the instruction class needs to be changed */
  boolean toggle_instrn_class;
  /* Flag to indicate that a poll is pending */
  boolean poll_pending;
  /* Flag to indicate that a fetch is pending */
  boolean fetch_pending;
  /* Indicates we have received a task stop from kernel. We must power the interface
   down. */
  boolean powering_down_task;
  /* Flag to indicate that task has been deregistered from RC_INIT */
  boolean task_dereg_from_rcinit;
  boolean clear_timeout_on_response;
  /* Indicates whether UIM clock stop is allowed in passive mode */
  boolean passive_clk_stop_allowed;
  boolean iccid_select_failed;
  boolean invalid_pps_received;
  boolean command_requeued;
  /* Indicates if the mobile is to be powered up and is set to FALSE when
   the power up is complete
  initialized TRUE */
  boolean me_powerup;
  /* Indicates if UIM can be powered down */
  boolean powerdown;

  /* Variable to indicate if sw1 warning1 that needs to be treated as normal
   * end has occured or not */
  boolean sw1_warnings1_normal_end_occured;
  /* Flag to indicate that the task is starting up */
  boolean task_startup;
  /* Holds whether we got the voltage information from ATR or not */
  boolean voltage_class_known_from_atr;
  /* Indicator as to whether static command buffer was used */
  boolean static_buffer_used;
  /* A boolean flag to keep track of the CMD RSP TIMER */
  boolean active_cmd_rsp_timer;
  /* A boolean flag to check if recovery disabling is asked during runtime */
  boolean runtime_disable_recovery;
  /* Flag to indicate if we need to skip PPS */
  boolean dont_skip_PPS;
  boolean uim_auth_cdma_df_first;
  /* Flag to check whether SWP to be encoded
     in Terminal Capability or NOT*/
  boolean uim_swp_in_terminal_capability;
  /*flag to check if a temporay card error has already been notified to mmgsdi*/
  boolean temporary_card_error_notified;
  /* Flag to indicate if external command needs to be flushed
     upon internal selection failure */
  boolean uim_do_not_flush_ext_cmd;
  /* This flag is to determine if the card
     supports both Class B and Class C */
  boolean voltage_class_c_and_b_supported;
  /* Flag to indicate if power up the local card is not allowed for
     remote connect request */
  boolean uim_not_allow_local_card_power_up;
  boolean is_euicc;
 /*Boolean to check if card is a rel_12 card*/
  boolean is_rel_12_uicc;
} uim_global_flag_type;

typedef struct
{
  dog_report_type  dog_rpt_var;
  timer_group_type always_on_timer_group;
  q_type           cmd_q;

}uim_global_task_type;

typedef struct
{
  uim_ldo_settings_type     uim_ldo;
  /* NPA handle used for CPU latency voting  */
  npa_client_handle         sleep_latency_voting_handle;
  /* NPA handle for /clk/mss/config_bus */
  npa_client_handle         mss_bus_clk_voting_handle;
  DalDeviceHandle           *TLMM_handle;
}uim_global_hardware_type;

typedef struct
{
  /* State configuration for the commands. */
  uim_cdma_state_type    cdma_states[UIM_MAX_NUMBER_CDMA_STATES];
  /* Array to hold the different command states */
  uim_gsm_state_type     gsm_states[UIM_MAX_NUMBER_GSM_STATES];
  uim_generic_state_type generic_states[UIM_MAX_NUMBER_GEN_STATES];
  /* Array to hold the different command states */
  uim_isim_state_type    isim_states[UIM_MAX_NUMBER_ISIM_STATES];
  /* Array to hold the different command states */
  uim_usim_state_type    usim_states[UIM_MAX_NUMBER_USIM_STATES];
}uim_global_mode_type;

/* Type to check whether rel 12 polling is enabled or not */
typedef enum
{
  UIM_REL12_POLLING_ENHANCHMENT_ENABLE_UNKNOWN = 0,
  UIM_REL12_POLLING_ENHANCHMENT_ENABLE_OFF,
  UIM_REL12_POLLING_ENHANCHMENT_ENABLE_ON
}uim_rel12_polling_enhancement_enable_type;

/* Type to check state of proactive polling */
typedef enum
{
  UIM_PD_POLLING_OFF = 0,
  UIM_PD_POLLING_SUSPENDED,
  UIM_PD_POLLING_ACTIVE
}uim_presence_detection_polling_state_type;

/* Type to check whether polling is enabled or not */
typedef enum
{
  UIM_DISABLE_POLLING_UNKNOWN = 0,
  UIM_DISABLE_POLLING_OFF,
  UIM_DISABLE_POLLING_ON
}uim_disable_polling_state_type;

typedef struct
{
  rex_timer_type no_wake_up_poll_timer;
  /* Polling timer capable of waking up the ME from sleep */
  rex_timer_type wake_up_poll_timer;
  boolean is_proactive_polling_enabled;  
  uim_rel12_polling_enhancement_enable_type  rel12_polling_enhancement_enable;
  uim_presence_detection_polling_state_type  presence_detection_state;
  uim_disable_polling_state_type             uim_disable_polling;
}uim_global_poll_type;

typedef struct
{
  /* Varible to hold configurable parameters stored in the NV */
  nv_uim_config_params_type config_params;
  /* To store list of status of uimdrv features */
  uim_features_status_list_type features_status_list;
  /* This holds the polling duration of the UIM
   Initially this is set to the in traffic poll time
   initialized as UIM_POLL_TIME */
  dword poll_time_delay;
  /* Time delay for polling
     initialized as UIM_POLL_TIME */
  dword proactive_time_delay;
  /* This is the delay used before sending a byte to the UIM card directly after
     processing a byte from the card.
     initialized as UIM_RESPONSE_BYTE_DELAY_DEFAULT */
  dword response_byte_delay;
  /*Stores the maximum number of command attempts allowed*/
  uint8   max_num_cmd_attempts;
  /* Initialized as UIM_WAKE_UP_POLL_OFFSET */
  dword   wakeup_poll_offset;
  /*This holds the busy indications timer values*/
  uim_feature_busy_response_type uim_feature_busy_response;
  boolean                        pdown_on_tech_prob_read;
  /* PDOWN ON CONSECUTIVE TECH PROB 6F00 configuration */
  uim_pdown_on_tech_problem_type pdown_on_tech_prob;

  boolean                            card_supports_suspend_resume;
  boolean                            device_supports_suspend_resume;
  boolean                            suspend_resume_enabled;
  boolean                            is_terminal_capability_supported;
}uim_global_setting_type;

typedef struct
{
 /* Flag to indicate whether a select MF is needed before executing
    any part of a command */
  boolean required;
  uint8   channel;  
} uim_explicit_mf_selection_type;

typedef struct
{
 /* Flag to indicate whether a select MF is needed before executing
    any part of a command */
  boolean required;
  uint8   channel;  
} uim_explicit_adf_selection_type;

typedef struct
{
/* Flag to indicate whether a select MF is needed before executing
    any part of a command */
  boolean select_aram_failed;
  uint8   channel;  
} uim_query_aram_app_type;

typedef struct
{
  boolean registered_for_data_exchange_notfication;
  uint64  call_mask;
  ds_sys_ioctl_global_stats_type cached_data_stats;
}uim_call_info_type;

typedef struct
{
  boolean manage_channel_allowed;
  uint8   unallocated_channel;
}
uim_internal_open_channel_info_type;


typedef struct
{
  uint32                voltage_class;

  /* Holds the mode for the current command */
  uim_command_mode_type cmd_mode;

  uim_convention_type   convention;
  /* uim_ctl initialized as:
     This is set to UIM_MC in R-UIM targets so that the UIM is powered down
     only after MC indicates that is finished with power up */
  uim_voter_type        ctl;

  /* This holds the protocol selected in the ATR processing */
  uim_transport_protocol_type operating_protocol;

  /* Technology present in UIM1 */
  uim_dfs_present_type  dfs_1;

  /* This holds the current protocol used by the server with the card */
  uim_transport_protocol_type current_protocol;

  uim_voltage_class_type current_voltage_class;

  uim_voltage_class_type max_possible_voltage_class;

  /* Static variable to hold the current baud rate parameters */
  /* initialized UIM_CRCF_372_1*/
  uim_clock_rate_conversion_factor_type FI_current;

  /* initialized UIM_BRAF_1*/
  uim_baud_rate_adjustment_factor_type  DI_current;

  /* Copy of the last instrn case */
  uim_instrn_case_type  last_instrn_case;

  /* Holds if the channel is open or not corresponding to each channel
    Array index signifies the channel number */
  /* Channel 0 is open by default initialized */
  boolean is_ch_open[UIM_MAX_CHANNELS];

  /* Number of attempts to select MF */
  /* initialized as 1*/
  uint32   mf_reselect_count;

  uim_op_params_type op_params_buf;

  /* Status of UIM initialization */
  /* initialized UIM_UNINITIALIZED_S */
  uim_status_type status;

  /* Protocols supported by the card */
  uim_prot_sup_by_card_type prot_sup_by_card;

  /* Information on the toolkit capability of the card */
  uim_proactive_uim_data_type type;

  /* This structure holds the data required during recovery */
  uim_recovery_data_type recovery_data;

  /* Pointer to save UIM Request buffer between states */
  uim_req_buf_type saved_req_buf;

  /* Work waiting time parameter */
  rex_timer_cnt_type  WI;

  /* First instruction class for the command */
  nv_uim_first_inst_class_type first_inst_class;

  /* Variable to hold the previous mode */
  uim_mode_type save_mode;

  /* Set clock to FALSE to start with. Appropriate value shall be set in uim_dev_init. */
  boolean clock_is_on;

  /* Byte to hold the file characteristics byte */
  byte file_char_byte;

  /* Clock stop control */
  uim_clock_stop_cntrl_type clock_stop_cntrl;
  /* This will be used in UIM_PASSIVE mode */
  uim_clock_stop_cntrl_type clock_stop_cntrl_passive_mode;

  uim_current_path_type current_path[UIM_MAX_CHANNELS];

  /*NV refresh client reference id*/
  uint32 nv_refresh_reference_id;

  uim_explicit_mf_selection_type mf_selection;

  uim_explicit_last_selection_type last_selected;

  uim_explicit_adf_selection_type adf_selection;

  uim_call_info_type uim_call_info; 

  uim_query_aram_app_type aram_app;
  
  uim_slot_connection_enum_type  last_connection_mode;

  uint8                          local_card_pup_delay;
  
  uim_internal_open_channel_info_type channel_info;
}uim_global_state_type;

typedef struct
{
  /* Pointer to the command under execution */
  uim_cmd_type *cmd_ptr;

  /* indicates number of times a command has been re-queued */
  int command_requeue_counter;

  /* Static buffer for internally generated commands */
  uim_cmd_type static_cmd_buf;

  /* Variable to indicate the mode in which UIM is operating */
  uim_mode_type mode;

  /* additional info for read and write and inc commands*/
  uim_rd_wr_info_type rd_wr_info;

  /* State of the UIM */
  uim_cdma_state_type *cdma_state_ptr;

  /* Pointer to the Global UIM Request buffer */
  uim_req_buf_type *cdma_req_buf_ptr;

  /* Command Response Timer */
  rex_timer_type cmd_rsp_timer;

  /* State of the UIM */
  const uim_generic_state_type *generic_state_ptr;

  /* State of the UIM */
  uim_gsm_state_type *gsm_state;

  /* Pointer to the Global UIM Request buffer */
  uim_req_buf_type *gsm_req_buf_ptr;

  /* State of the UIM */
  uim_isim_state_type *isim_state;

  /* Pointer to the Global UIM Request buffer */
  uim_req_buf_type *isim_req_buf_ptr;

  /* State of the UIM */
  uim_usim_state_type *usim_state;

  /* Pointer to the Global UIM Request buffer */
  uim_req_buf_type *usim_req_buf_ptr;

  /* Request buffer for a UIM command */
  uim_req_buf_type req_buf;

  /* Response buffer */
  uim_rsp_buf_type rsp_buf;

  /* Buffer to hold report to be returned to task issuing command */
  uim_rpt_type  rpt_buf;

  /* additional info for auth commands*/
  uim_rd_wr_info_type auth_info;

  /* Used to keep track of how many apdus have been sent in a chained Compute
   IP Auth Command */
  word    compute_ip_auth_num_blocks_sent;
  boolean compute_ip_auth_more_blocks_req;

  /* global to set when card requests MORE TIME so that UIM clock
   is not stopped until the next command is needed to be sent */
  boolean more_time_pcmd_fetched;

}uim_global_command_type;

typedef struct
{
  byte *apdu_command_data_ptr;

  /* Store INS byte from C-APDU header for the transaction */
  byte ins_byte;

  /*
   * This is an APDU command buffer that holds the command for a supplemental
   * APDU transaction that is compiled and sent by the UIM driver
   */
  uim_req_buf_type cmd_req;

  /* Keep a counter of the bytes that were already read prior
     to the Reinitialization of RX transfer */
  uint32 total_bytes_read;

  /* A static variable to track the number of bytes received */
  uint32 num_bytes_received;

  /* A static variable to track the number of words received */
  uint32 num_words_received;

  /* A static buffer to track of the dwords received */
  dword   uart_rx_buf[UART_RX_BUF_MAX_SIZE];

  word num_resp_data_bytes_expected;

  uim_protocol_type uimdrv_protocol;

  /* Holds the current channel being used */
  uim_channel_type cur_channel;
  byte             num_cmd_data_bytes_to_send;
  int              num_bytes_to_send;
  byte            *request_buffer_ptr;
  /* This points to the response buffer for the current APDU transaction.
     uim resp buf type has both the response byte buffer and additional
     info (sw1/2 size etc. */
  uim_rsp_buf_type *resp_buf_ptr;

  /* this points to the current byte within the uim rsp_buf_ptr->rsp
     it's incremented per byte received (instead of maintaining and index
     and indexing into resp_buf_ptr->rsp[index++] we resp_buf_byte_ptr++ */
  byte *resp_buf_byte_ptr;

  /* Timers for UIM BUSY RESPONSE Feature */
  rex_timer_type uim_busy_ind_timer;
  rex_timer_type uim_trans_timer;
  /* Timers for UIM BUSY RESPONSE Feature test ONLY*/
  rex_timer_type uim_simulate_null_timer;

  word  num_resp_bytes_rcvd;
  word  num_resp_bytes_rcvd_total;

  /* Status Word 1 used for Get Resp */
  uim_sw1_type get_resp_sw1;
  /* Status word 2 used for Get Resp */
  uim_sw2_type get_resp_sw2;

  /* Count for repeated technical problems */
  int inc_tech_prob_count;
  /* Global to count the null procedure butes */
  uint32 infinite_null_count;

  /* Instruction toggle counter initialized 1 */
  byte instrn_toggle_count;
  byte overrun_error_count;
  byte parity_error_count;

  byte idle_parity_error_count;
  byte idle_rx_break_error_count;
  byte idle_overrun_error_count;

  uint16 uart_dm_interrupt_wait_count;

  /* Flags to indicate whether we received any warning while
     processing Envelope Command */
  boolean warning1_indicator;
  boolean warning2_indicator;

  /* Work waiting timeout (msecs) */
  rex_timer_cnt_type  work_waiting_time;
  rex_timer_cnt_type  original_work_waiting_time;
  rex_timer_cnt_type  extended_work_waiting_time;

  /* Work waiting timeout (ETUs) */
  rex_timer_cnt_type  work_waiting_time_etus;
  rex_timer_cnt_type  original_work_waiting_time_etus;
  rex_timer_cnt_type  extended_work_waiting_time_etus;

  /* States for UIM BUSY RESPONSE Feature */
  uim_busy_response_timer_state_type uim_busy_ind_timer_state;
  uim_busy_response_timer_state_type uim_trans_timer_state;
  /* State for UIM BUSY RESPONSE Feature test ONLY*/
  uim_busy_response_timer_state_type uim_simulate_null_timer_state;
  word stream_apdu_rpt_data_length;
}uim_global_card_cmd_type;

typedef struct
{
  uim_rx_state_type rx_state;
  uim_rx_state_type rx_sub_state;
  uim_rx_state_type rx_t1_block_sub_state;
  uim_tx_state_type tx_state;
}uim_global_rxtx_state_type;

typedef struct
{
  uim_me_hotswap_configuration_type hotswap_me_conf;
  timer_type                        hotswap_slot_debounce_timer;
  uint16                            hotswap_ins_sample_count;
  uint16                            hotswap_rem_sample_count;
  boolean                           hotswap_debounce_in_progress;
  boolean                           hotswap_me_power_down;
  boolean                           hotswap_notify_card_removed;
  /* This flag is used to disable the hotswap capability at runtime,
     UIM will not allow to make a card status query when this flag is TRUE.  */
  boolean                           hotswap_disabled_uicc_seq_off;
  uim_hotswap_slot_capability       hotswap_capability;
  /* Card inserted request queue for hot swap*/
  q_type                            hotswap_ins_cmd_q;
  /* Card removed request queue for hot swap*/
  q_type                            hotswap_rem_cmd_q;
  /* Queue for hot swap req buffer */
  q_type                            hotswap_free_q;
  /* Maximum allocated request buffers for hot swap */
  uim_cmd_type                      hotswap_cmd_pool[UIM_HOTSWAP_NUM_CMD_BUFS];
  /* For runtime conntrol of FEATURE HOTSWAP */
  uim_feature_support_hotswap_type  feature_support_hotswap;
  /* initialized as UIM_CARD_UNCHANGED */
  uim_card_swap_enum_type           card_status;
  uint16                            hotswap_debounce_retry_count;
  /* First instruction class for the command */
  nv_uim_first_inst_class_type      hotswap_first_ins_class;
  /* initialized to UIM_CARD_UNCHANGED (2)*/
  uim_card_swap_enum_type           hotswap_card_status;
  /* Global to indicate if hotswap - card_removed is supicious */
  boolean                           hotswap_card_removed_suspicious;
  /* Global to indicate if flaky hotswap switch feature is supported */
  boolean                           hotswap_flaky_switch_support;
}uim_global_hotswap_type;

typedef enum {
  UIM_EF_AD_NOT_READ,
  UIM_EF_AD_SUPPORTS_POLLING_ALIGNMENT,
  UIM_EF_AD_DO_NOT_SUPPORT_POLLING_ALIGNMENT
}uim_ef_ad_support_for_alignment_type;


typedef struct
{
    /* Card inserted request queue for hot swap */
  q_type                                 msgr_cmd_q;
  msgr_client_t                          msgr_rrc_ml1_client;
  uim_ef_ad_support_for_alignment_type   ef_ad_value;
  boolean                                configured;
  uint32                                 cycle_length;
  boolean                                allow_poll_alignment_with_edrx;
}uim_global_edrx_type;


typedef struct
{
#if defined( FEATURE_UIM_T_1_SUPPORT )
  /* This holds the current protocol used by the server with the card */
  byte t_1_node_address; /* Set to this value by default */

  /* This holds the last S-block code sent by the MSM */
  byte t_1_s_block_code;
  /* This holds the last S-block data sent by the MSM */
  byte t_1_s_block_data;
  /* This holds the last S-block data size sent by the MSM */
  byte t_1_s_block_data_size;

  /* This holds the IFS for the card */
  word t_1_ifsc;

  /* This holds the IFS for the MSM */
  word t_1_ifsd;

  /* Preferred protocol of operation */
  /* initialized NV_UIM_PREF_PROTOCOL_T_1 */
  nv_uim_pref_protocol_type nv_pref_protocol;
  /* This holds the current T=1 protocol state */
  uim_t_1_protocol_states_type t_1_protocol_state;

  /* Holds whether or not we are receiving a chain */
  boolean t_1_receiving_chained_block;

  /* Back-up copy of the response buffer pointer */
  byte *back_up_uim_apdu_rsp_buffer_ptr;


  /* This points to the APDU command buffer */
  uim_req_buf_type *t_1_apdu_req_buf_ptr;

  /* This points to the transport layer response buffer */
  uim_rsp_buf_type  *t_1_apdu_resp_buf;

  /* APDU request buffer pointer */
  byte *apdu_req_buffer_ptr;

  /* Number of bytes to send in the APDU command */
  word apdu_request_bytes_to_send;

  /* Total number of bytes received in the APDU response */
  word apdu_rsp_num_bytes;

  /* This holds the number of failed transactions for this command */
  int t_1_error_count;

  /* APDU response buffer pointer */
  byte *apdu_rsp_buffer_ptr;

  /****** These buffers are utilized between the ISR and this package */

  /* This holds a running count of the number of transmitted bytes it is
     used to keep track of the transmitted bytes read from the receive FIFO */
  int num_txd_bytes;

  /* This holds the prologue field of the transmitted T=1 block */
  uim_prologue_field_type tx_prologue_field;

  /* This holds the current EDC selection */
  uim_t_1_edc_type t_1_edc;

  /* This points to the transmitted prologue field */
  byte *rx_prologue_field_buffer_ptr;

  /* This holds the length of the last sent Chained I Block */
  word last_tx_i_block_length;

  /* This holds the epilogue field of the T=1 block */
  uim_epilogue_field_type tx_epilogue_field;


  /* This holds the size of the epilogue field */
  int epilogue_field_size;

  /* Holds the block waiting time index */
  rex_timer_cnt_type t_1_bwi;

  /* Holds the character waiting time index */
  rex_timer_cnt_type t_1_cwi;

  /* This holds the character waiting time */
  rex_timer_cnt_type t_1_cwt;

  /* This holds the character waiting time in ETUs */
  rex_timer_cnt_type t_1_cwt_etus;

  /* This holds the block waiting time */
  rex_timer_cnt_type t_1_bwt;

  /* This holds the block waiting time in ETUs */
  rex_timer_cnt_type t_1_bwt_etus;

  /* This holds the block waiting time */
  /* initialized as 1 */
  rex_timer_cnt_type t_1_wtx;

  /* Block guard time when T=1 */
  rex_timer_cnt_type t_1_bgt;

  /* This holds the EDC for Rx block */
  word error_detection_code;

 /* This holds the expected send sequence number of an I-block from the card */
  byte card_send_sequence_num;
  /* This holds the next send sequence number of the next I-block to send to
     the card */
  byte me_send_sequence_num;

  /* This holds the control to change the send sequence number for the ME
     This value is set to 1 when an I-block is to be acknowledged and set to
     0 if the I-block has been acknowledged and are receiving a chained
     response */
  byte t_1_changed_me_send_seq_number;

  /* This holds the transmitted prologue field of the T=1 block */
  uim_prologue_field_type rx_prologue_field_buffer;

#endif /* FEATURE_UIM_T_1_SUPPORT */

  /* Indicates use minimum guardtime from ATR */
  boolean use_minimum_guardtime_size;

}uim_global_t1_type;

typedef struct
{
  uim_atr_buf_type       atr_buf;
  boolean                atr_received;
  boolean                atr_pps_done;
  uim_hist_char_buf_type hist_char_buf;
  int                    num_historical_chars;
  byte                   pps_char_buf[UIM_MAX_PPS_CHARS];
  byte                   check_byte;
  boolean                check_byte_expected;
  timer_type             atr_40k_timer;
  uim_pps_req_buf_type   pps_req_buf;
  int                    num_bytes_to_skip;
  boolean                is_parity_in_atr;
  uim_atr_pps_rx_mode_type receive_mode;
  /* boolean variable indicating if a byte is received since reset */
  boolean                atr_byte_received;
  /* global to store the TC1 byte value */
  uint8                  atr_tc1_byte;
}uim_global_atr_type;

typedef struct
{
  uim_time_unit_type       t_unit_min;
  uint8                    t_min;
  uim_time_unit_type       t_unit_max;
  uint8                    t_max;
  
}uim_suspend_time_type;
typedef struct
{
  uim_suspend_time_type    suspend_time;
  uim_time_unit_type       t_unit_actual;
  uint8                    t_actual;
  boolean                  is_card_suspended;
  uint8                    resume_token[UIM_RESUME_TOKEN_LENGTH]; 
  timer_type               uim_inactivity_timer;
  timer_type               uim_resume_timer;
  boolean                  is_terminal_capability_sent;
}uim_suspend_resume_type;



typedef struct
{
  byte                      check_byte;
  int                       num_bytes_to_skip;
  uim_atr_pps_rx_mode_type  receive_mode;
}uim_global_pps_type;

#ifdef FEATURE_UIM_USB_UICC
typedef struct
{
  /* UICC card support USB mode*/
  boolean              usb_mode_card_supported;

  /* UIM USB interface status */
  uim_usb_interface_status_type usb_interface_status;

  /* UIM USB fallback to iso timer */
  rex_timer_type       usb_fallback_to_iso_timer;

  /* UIM USB init timer */
  rex_timer_type       usb_init_timer;
}uim_global_usb_type;
#endif /* FEATURE_UIM_USB_UICC */

#ifdef FEATURE_UIM_BTSAP_CLIENT
typedef struct
{
  uint8  handle;
  uint32  btsap_cmd_rsp_timer_value;
}uim_remote_btsap_data_type;
#endif /* FEATURE_UIM_BTSAP_CLIENT */

#ifdef FEATURE_UIM_REMOTE_UIM
/* states of uim in remote client mode */
typedef enum {
  WAIT_FOR_NOTHING,
  WAIT_FOR_CONNECTION_AVAILABLE,
  WAIT_FOR_ATR,
  WAIT_FOR_RESPONSE,
  WAIT_FOR_POWER_DOWN_RSP,
} uim_client_mode_state_type;

/* Data structure for extended data */
typedef struct
{
  uint8   *extended_data_ptr;
  uint8    ext_sw1;
  uint8    ext_sw2;
  uint16   offset;
  uint16   length;
  boolean  is_get_response_allowed;
} uim_extended_data_type;

/* Structure to store NV data for remote response timer,
   This structure has been updated, version should be set to 1*/
typedef PACKED struct PACKED_POST{
  uint8                      version;
  uint32                     timer; /* in units of millisec */
  uint8                      RFU[3];
}uim_remote_cmd_rsp_timer_nv_type;

/* Data structure for remote clients */
typedef struct
{
  uint8  handle;
  uint16                     rsp_buf_offset;
  uint16                     rsp_buf_length;
  uim_client_mode_state_type state;
  boolean                     uim_remote_timer_read;
  uim_extended_data_type     extended_data;
  uim_req_buf_type           *cmd_ptr;
  rex_timer_type             remote_cmd_rsp_timer;
  #ifdef FEATURE_UIM_BTSAP_CLIENT
  uim_remote_btsap_data_type btsap;
  #endif /* FEATURE_UIM_BTSAP_CLIENT */
}uim_global_remote_type;
#endif /* FEATURE_UIM_REMOTE_UIM */

typedef struct
{
  /* type of user defined call back */
  uim_silent_recovery_cb_type uim_silent_recovery_callback;
  /* user data is supplied by client and pass it back to client */
  const void *user_data;
}uim_global_silent_recovery_type;

 /*Enum which says if extended recovery feature is enabled or not. based on NV*/
typedef enum
{
  UIM_EXTENDED_RECOVERY_UNKNOWN,
  UIM_EXTENDED_RECOVERY_ON,
  UIM_EXTENDED_RECOVERY_OFF
}uim_extended_recovery_enable_type;


typedef struct
{
  /*global to determine if extended recovery (fresh powerup process) has already started*/
  boolean                                extended_recovery_in_progress;
  /*Enum which says if extended recovery feature is enabled or not. based on NV*/
  uim_extended_recovery_enable_type      extended_recovery_enabled;
  /*global to decide delay between recoveries*/
  uint16                                 recovery_delay;
  /*extended recovery configuration*/
  uim_feature_extended_recovery_nv_type  recovery_config;
  /*timer for timing the fresh powerups*/
  rex_timer_type                         extended_recovery_timer;
  /*global to track the current fresh powerup count*/
  int                                    count;
}uim_global_extended_recovery_type;

typedef struct
{
 uimdrv_qmi_indications_type          last_sent_indication_to_qmi;
 uimdrv_qmi_acknowledgements_type     required_ack_from_qmi;
 uimdrv_qmi_acknowledgements_type     received_ack_from_qmi;
 rex_timer_type                       uimdrv_qmi_sanity_timer;
 rex_timer_type                      *uimdrv_qmi_sanity_timer_ptr;
}uim_global_qmi_interactions_type;

typedef struct
{
  gstk_client_id_type                 uimdrv_gstk_client_id;
}uim_global_uimdrv_client_id;

struct uim_instance_global_type
{
  uim_instance_enum_type                id;
  rex_tcb_type                         *tcb_ptr;
  uim_global_cache_type                 cache;
  uim_global_crit_sect_type             crit_sect;
  uim_global_debug_type                 debug;
  uim_global_flag_type                  flag;
  uim_global_task_type                  task;
  uim_global_hardware_type              hardware;
  uim_global_mode_type                  mode;
  uim_global_poll_type                  poll;
  uim_global_setting_type               setting;
  uim_global_state_type                 state;
  uim_global_command_type               command;
  uim_global_card_cmd_type              card_cmd;
  uim_global_rxtx_state_type            rxtx_state_machine;
  uim_global_hotswap_type               hotswap;
  uim_global_t1_type                    t1;
  uim_global_atr_type                   atr;
  uim_global_pps_type                   pps;
#ifdef FEATURE_UIM_USB_UICC
  uim_global_usb_type                   usb;
#endif /* FEATURE_UIM_USB_UICC */
#ifdef FEATURE_UIM_REMOTE_UIM
  uim_global_remote_type                remote;
#endif /* FEATURE_UIM_REMOTE_UIM */
/* Added for SILENT RECOVER UICC/ICC */
 uim_global_silent_recovery_type        silent_recover;
 uim_global_extended_recovery_type      extended_recovery;
 uim_global_qmi_interactions_type       uim_qmi_interaction;
 uim_global_edrx_type                   edrx;
 uim_global_uimdrv_client_id            uimdrv_client_id;
 uim_suspend_resume_type                suspend_resume;
 
};

/* Note - uim_shared_globals to be used only in sections of code guarded
   by critical sections.*/
struct uim_shared_global_type
{
  rex_crit_sect_type   nv_read_crit_section;
  boolean              nv_read_completed;
  uint8                nv_read_by_instance;
};

uim_instance_global_type *uim_get_instance_ptr
(
  uim_instance_enum_type id
);

void uim_set_instance_ptr_null
(
  uim_instance_enum_type id
);

uim_shared_global_type *uim_get_common_globals_ptr
(
   void
);

void uim_init_common_globals
(
   void
);

void uim_alloc_global_instance
(
  uim_instance_enum_type id
);


/*===========================================================================

FUNCTION UIM_RESET_GLOBALS

DESCRIPTION
  This function resets necessary UIM globals if UIM encounters an
  unexpected error during power up or an internal wake up and chooses to
  notify its clients by calling uim_notify_error.
  It is also called in the event of a card removal if hotswap is enabled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_reset_globals
(
  uim_instance_global_type *uim_ptr
);
#endif /* UIMGLOBALS_H */
