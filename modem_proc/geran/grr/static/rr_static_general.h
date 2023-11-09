#ifndef RR_STATIC_GENERAL_H
#define RR_STATIC_GENERAL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               rr_static_general.h

GENERAL DESCRIPTION
   This module contains general functions for use of GRR under static environment

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2017-2017 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header:

when        who     what, where, why
--------    ---     ----------------------------------------------------------

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "sys.h"
#include "list.h"
#include "mm_rr.h"
#include "rr_defs.h"
#include "nv_items.h"
#include "rr_pscan.h"
#include "geran_multi_sim.h"
#include "rr_public_store.h"
#include "rr_sys_info_cache.h"
#include "rr_candidate_cell_db.h"
#include "rr_acq_db.h"
#include "gprs_odm_info.h"
#include "rr_ps_domain.h"

/*===========================================================================

                 Preprocessor Definitions and Constants

===========================================================================*/
#ifdef FEATURE_RR_SI_CACHE
#define MAX_NUM_CELLS_TO_CACHE 10
#define MAX_NUM_SI_PER_CELL    17
#define MAX_NUMBER_CACHED_SI   (NUM_GERAN_DATA_SPACES * MAX_NUM_CELLS_TO_CACHE * MAX_NUM_SI_PER_CELL)
#endif
#define MAX_RR_ACQ_DB_ENTRIES 971
#define MAX_RR_ACQ_DB_EFS_ENTRIES 200

#ifdef DEBUG_RR_TRACE_MSG
#define RR_NUM_TRACE_MSGS 50
#endif /* DEBUG_RR_TRACE_MSG */

#define RR_DEBUG_MM_RR_TRACE_BUFFER_SIZE          50
#define RR_DEBUG_RR_L1_TRACE_BUFFER_SIZE          50
#define RR_DEBUG_MAC_RR_TRACE_BUFFER_SIZE         10
#define RR_DEBUG_RR_MAC_TRACE_BUFFER_SIZE         10
#define RR_DEBUG_WCDMA_RRC_RR_TRACE_BUFFER_SIZE   10
#define RR_DEBUG_TDS_RRC_RR_TRACE_BUFFER_SIZE     10
#ifdef FEATURE_MSGR_RR_DEBUG
#define RR_DEBUG_MSGR_RR_TRACE_BUFFER_SIZE        10
#endif
#define RR_DEBUG_PANIC_RESET_TRACE_BUFFER_SIZE    10
#define RR_DEBUG_STATIC_DYN_TRACE_BUFFER_SIZE     100
#define RR_DEBUG_STATIC_SAVED_TRACE_BUFFER_SIZE   100
#define RR_DEBUG_STATIC_ADDED_TRACE_BUFFER_SIZE   100

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/
#ifdef DEBUG_RR_TRACE_MSG

typedef struct
{
  uint32            timestamp;       // 4 bytes
  rr_grr_state_mc_T sm_id;           // 1 byte
  byte              state;           // 1 byte
  rr_event_T        event;           // 2 bytes
  byte              message_set;     // 1 byte
  byte              message_id;      // 1 byte
  byte              message_frag[2]; // 2 bytes (remaining space that would otherwise be padding)
} rr_trace_msg_T;

#endif /* DEBUG_RR_TRACE_MSG */

typedef struct
{
  uint32                  timestamp;
  mm_rr_message_id_enum_T message_id;
  uint32                  pended_count;
} rr_debug_mm_rr_trace_buffer_entry_t;

typedef struct
{
  uint32                   timestamp_fn;
  timetick_type            timestamp_clk;
  mac_grr_sig_id_t         message_id;
  uint32                   pended_count;
} rr_debug_mac_rr_trace_buffer_entry_t;

typedef struct
{
  uint32                   timestamp_fn;
  timetick_type            timestamp_clk;
  grr_mac_sig_id_t         message_id;
} rr_debug_rr_mac_trace_buffer_entry_t;


typedef struct
{
  uint32                  timestamp;
  rr_l1_message_id_enum_T message_id;
  uint32                  pended_count;
} rr_debug_rr_l1_trace_buffer_entry_t;

#define RR_DEBUG_NAS_TRANSACTION_PAIRS 6

typedef struct
{
  mm_rr_message_id_enum_T incoming_msg;
  timetick_type           incoming_clk;
  mm_rr_message_id_enum_T outgoing_msg;
  timetick_type           outgoing_clk;
  timetick_type           elapsed_time;
  boolean                 valid_record;
} rr_debug_nas_transaction_record_t;

#ifdef FEATURE_WCDMA

typedef struct
{
  uint32            timestamp;
  rrc_rr_cmd_e_type message_id;
  uint32            pended_count;
} rr_debug_wcdma_rrc_rr_trace_buffer_entry_t;

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS

typedef struct
{
  uint32               timestamp;
  tdsrr_rrc_cmd_e_type message_id;
  uint32               pended_count;
} rr_debug_tds_rrc_rr_trace_buffer_entry_t;

#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_MSGR_RR_DEBUG
typedef struct
{
  uint32         timestamp;
  msgr_umid_type message_id;
  uint32         pended_count;
} rr_debug_msgr_rr_trace_buffer_entry_t;
#endif

#ifdef FEATURE_AWARE_CT21
typedef struct
{
  rr_cmd_type             *cmd_ptr;
  rr_cmd_type             *pend_queue_ptr;
  uint32                   timestamp;
  mm_rr_message_id_enum_T  message_id;
  uint8                    message_set;
} rr_debug_static_mm_rr_dyn_buffer_entry_t;

typedef struct
{
  rr_cmd_type             *saved_q_ptr;
  uint32                  timestamp;
  mm_rr_message_id_enum_T message_id;
  uint8                   message_set;
} rr_debug_static_freed_saved_msg_entry_t;

typedef struct
{
  rr_cmd_type             *saved_q_ptr;
  uint32                  timestamp;
  mm_rr_message_id_enum_T message_id;
  uint8                   message_set;
} rr_debug_static_added_saved_msg_entry_t;
#endif

typedef struct
{
     
#ifdef DEBUG_RR_TRACE_MSG
  rr_trace_msg_T rr_trace_msg_buf[RR_NUM_TRACE_MSGS];
  byte           rr_trace_msg_buf_index;
#endif

  rr_debug_mm_rr_trace_buffer_entry_t          mm_rr_trace_buffer[RR_DEBUG_MM_RR_TRACE_BUFFER_SIZE];
  uint32                                       mm_rr_trace_buffer_index;

  rr_debug_mac_rr_trace_buffer_entry_t         mac_rr_trace_buffer[RR_DEBUG_MAC_RR_TRACE_BUFFER_SIZE];
  uint32                                       mac_rr_trace_buffer_index;

  rr_debug_rr_mac_trace_buffer_entry_t         rr_mac_trace_buffer[RR_DEBUG_RR_MAC_TRACE_BUFFER_SIZE];
  uint32                                       rr_mac_trace_buffer_index;

  rr_debug_rr_l1_trace_buffer_entry_t          rr_l1_trace_buffer[RR_DEBUG_RR_L1_TRACE_BUFFER_SIZE];
  uint32                                       rr_l1_trace_buffer_index;

#ifdef FEATURE_WCDMA
  rr_debug_wcdma_rrc_rr_trace_buffer_entry_t   wcdma_rrc_rr_trace_buffer[RR_DEBUG_WCDMA_RRC_RR_TRACE_BUFFER_SIZE]; 
  uint32                                       wcdma_rrc_rr_trace_buffer_index;
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
  rr_debug_tds_rrc_rr_trace_buffer_entry_t     tds_rrc_rr_trace_buffer[RR_DEBUG_TDS_RRC_RR_TRACE_BUFFER_SIZE];
  uint32                                       tds_rrc_rr_trace_buffer_index;
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_MSGR_RR_DEBUG
  rr_debug_msgr_rr_trace_buffer_entry_t        msgr_rr_trace_buffer[RR_DEBUG_MSGR_RR_TRACE_BUFFER_SIZE];
  uint32                                       msgr_rr_trace_buffer_index;
#endif /* FEATURE_MSGR_RR_DEBUG */

  uint32                                       panic_reset_trace_buffer[RR_DEBUG_PANIC_RESET_TRACE_BUFFER_SIZE];
  uint32                                       panic_reset_trace_buffer_index;

  rr_debug_nas_transaction_record_t            nas_transactions[RR_DEBUG_NAS_TRANSACTION_PAIRS];

#ifdef FEATURE_AWARE_CT21
  rr_debug_static_mm_rr_dyn_buffer_entry_t     rr_static_dyn_mm_trace_buffer[RR_DEBUG_STATIC_DYN_TRACE_BUFFER_SIZE];
  uint32                                       rr_dyn_static_trace_buffer_index;

  rr_debug_static_freed_saved_msg_entry_t      rr_static_freed_saved_trace_buffer[RR_DEBUG_STATIC_SAVED_TRACE_BUFFER_SIZE];
  uint32                                       rr_freed_static_trace_buffer_index;

  rr_debug_static_added_saved_msg_entry_t      rr_static_added_saved_trace_buffer[RR_DEBUG_STATIC_ADDED_TRACE_BUFFER_SIZE];
  uint32                                       rr_added_static_trace_buffer_index;
#endif
} rr_gprs_debug_data_t;


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
typedef enum
{
  RR_DEACT_REASON_STOP_GSM_MODE_REQ,
  RR_DEACT_REASON_DEACT_REQ,
  RR_DEACT_REASON_LOWER_LAYER_FAILURE,
  RR_DEACT_REASON_STOP_CAMPING,
  RR_DEACT_REASON_STOP_CAMPING_THEN_RESTART,
  RR_DEACT_REASON_SWITCH_TO_OTHER_RAT,
  RR_DEACT_REASON_X2G_BPLMN_SEARCH_COMPLETE
} rr_deact_reason_t;

typedef struct
{
#ifdef FEATURE_RR_SI_CACHE
  uint8                         scell_csi_count;
  csi_entry_t                   scell_csi[MAX_NUM_SI_PER_CELL];
#endif
  byte                          RXLEV_average;
  BSIC_T                        bsic;
  inter_task_BCCH_allocation_T  BA_gprs;
  rr_PLMN_request_details_T     plmn_req_details;
}rr_scell_backup_t;

typedef struct
{
  /* Barred cell list. Cells barred due to authentication or authentication and ciphering failure */
  rr_barred_cell_info_T      rr_barred_cell_list[RR_MAX_NUM_OF_BARRED_CELLS];  
  rr_deact_reason_t          rr_deact_reason;
  rr_mm_gsm_stop_reason_T    rr_mm_gsm_stop_reason;
  gprs_odm_cell_type_cb_fn_t odm_cell_type_cb_fn;
  rr_scell_backup_t         *rr_scell_backup;
  rr_ps_domain_data_t       *ps_domain_backup;
  uint32                     scell_meas_timestamp;
} rr_static_general_data_t;

typedef struct
{
  rrps_change_mark_t               change_mark_si13;
  rrps_change_mark_t               change_mark_psi2;
  rrps_ma_bitmap_t                 ma_valid_bitmap;
  rrps_gprs_cell_options_t         gprs_cell_options;
  rrps_global_power_ctrl_params_t  power_control_params;
  rrps_sim_data_t                  sim_data;
  rrps_gmm_data_t                  gmm_data;
  rrps_ms_data_t                   ms_data;
  uint8                            priority_access_thr;
  uint8                            nw_band_ind;
} rrps_store_data_t;

typedef struct
{
  rrps_store_data_t rrps_store_data;
} rr_public_store_data_t;

typedef struct
{
  uint8 num_of_plmns;
  sys_plmn_id_s_type *plmn_list_ptr;
} rr_plmn_white_list_t;

#ifdef FEATURE_LTE
typedef struct
{
  boolean              x2g_fast_si_acq_white_list_present;
  rr_plmn_white_list_t x2g_fast_si_acq_white_list;
} rr_efs_x2g_fast_si_acq_info_t;
#endif
typedef struct
{
  list_link_type     list_link;
  boolean            valid;
  sys_plmn_id_s_type plmn_id;
  ARFCN_T            arfcn;
#ifdef FEATURE_PS_PREFERRED_CELL
  rr_ps_support_status_t ps_support;
#endif
} rr_acq_db_entry_t;

typedef struct
{
  rr_acq_db_entry_t array[MAX_RR_ACQ_DB_ENTRIES];
  list_type list;
} rr_acq_db_t;

typedef PACKED struct PACKED_POST
{
  uint16 hashed_arfcn;
  uint8  plmn_id[3];
} rr_acq_db_efs_entry_t;

typedef struct
{
  rr_acq_db_t           rr_acq_db;
  rr_acq_db_efs_entry_t rr_acq_db_efs_data[MAX_RR_ACQ_DB_EFS_ENTRIES];
} rr_acq_db_data_t;

#ifdef _WIN32
#pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
typedef PACK(struct)
{
  boolean g2w_enabled;
  boolean g2t_enabled;
  boolean g2l_enabled;
} rr_efs_g2x_measurement_control_t;
#endif

#ifdef _WIN32
#pragma pack(pop) // Revert alignment to what it was previously
#endif

typedef struct
{
  uint8    split_page_cycle_enc;
  uint16   split_page_cycle;
  uint8    non_drx_timer_enc;
  uint8    non_drx_timer;
  uint8    multislot_class;
  uint8    anite_gcf_flag;
  boolean  egprs_enabled;
  uint8    egprs_multislot_class;
  boolean  egprs_8psk_ul_enabled;
  uint8    egprs_8psk_power_capability;
  boolean  geran_feature_pack_1_enabled;
#ifdef FEATURE_WCDMA
  uint8    interrat_nacc_support;
#endif
#ifdef FEATURE_GSM_DTM
  boolean  dtm_enabled;
  uint8    dtm_egprs_multislot_subclass;
#ifdef FEATURE_GSM_EDTM
  boolean  edtm_enabled;
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */
  uint8    gea_supported;
#ifdef FEATURE_GSM_GPRS_MSC33
  uint8    hmsc_gprs_coded_ms_class;
  uint8    hmsc_egprs_coded_ms_class;
#endif /* FEATURE_GSM_GPRS_MSC33 */
} rr_nv_data_t;

typedef struct
{
#ifdef FEATURE_VAMOS
  boolean                          vamos_enabled;
#endif /* FEATURE_VAMOS */
#ifdef FEATURE_LTE
  boolean                          fast_si_acq_during_csfb_enabled;
#endif
  boolean                          fast_si_acq_when_cs_only_enabled;
  rr_plmn_white_list_t             negative_c1_white_list;
  uint8                            pscan_results_reuse_time_secs;
  boolean                          plmn_search_freq_pruning_enabled;
  boolean                          enable_si3_only;
#ifdef FEATURE_LTE
  rr_efs_x2g_fast_si_acq_info_t    x2g_fast_si_acq_info;
#endif
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
  boolean                          sglte_g2x_conn_mode_resel_enabled;
#endif
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
  boolean                          fast_g2l_using_pseudo_si2q_enabled;
  rr_efs_pseudo_si2q_def_data_t    rr_efs_pseudo_si2q_def_data;
#endif
  int8                             add_crh_db;
  int8                             add_crh_scell_thresh_db;
  boolean                          sys_info_cache_enabled;
#ifdef FEATURE_GSM_BAND_AVOIDANCE
  boolean                          band_avoid_enabled;
  uint8                            band_avoid_ncell_threshold;
  uint8                            band_avoid_max_meas_rpts;
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/
#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER 
  rr_plmn_white_list_t             read_pch_during_transfer_whitelist;
#endif /*FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */
  boolean                          iterative_si_acq_enabled;
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  uint8                            scell_resync_time_secs;
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  boolean                          eng_mode_debug_enabled;
#if defined(FEATURE_LTE) && defined(FEATURE_GERAN_CS_SUPPORT)
  /* Indicates whether G2L blind redirection is enabled.  If enabled, after
  a CS call (for voice, supplementary services or periodic LAU) is released
  RR will initiate a blind redirection to LTE. */
  boolean                          g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled;

  /* Indicates whether G2L blind redirection after CSFB is enabled.  If
  enabled, after a CS call resulting from a CSFB procedure is released, RR
  will initiate a blind redirection to LTE.  This is based on RR_EST_REQ
  indicating whether the call is being set up as a result of CSFB. */
  boolean                          g2l_blind_redir_after_csfb_enabled;
  boolean                          g2l_blind_redir_after_srvcc_enabled;
#endif /* FEATURE_LTE && FEATURE_GERAN_CS_SUPPORT */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  rr_efs_g2x_measurement_control_t g2x_measurement_control;
#endif
#ifdef FEATURE_IDLE_DRX_SCALING
  uint8                            idle_drx_scaling_suspend_time_secs;
#endif /* FEATURE_IDLE_DRX_SCALING */

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION 
 boolean                           peo_enabled;
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK
boolean peo_test_param_present;
uint8   peo_dsc;
uint8   peo_rcc;
uint8   peo_c1_delta_min;
uint8   peo_c1_delta_max;
uint8   peo_bcch_change_mark;
uint8   implicit_reject;
uint8   implicit_reject_during_rach;
boolean allow_peo_rach;
boolean si21_indicator;
uint8   si21_position;
boolean si9_where_indicator;
uint8   si9_where;
boolean eab_config;
boolean special_access_class;
boolean ir_eab_test_param_present;
#endif /*FEATURE_GERAN_R13_TEST_FRAMEWORK*/
#ifdef FEATURE_GERAN_EAB
boolean eab_enabled;
#endif /* FEATURE_GERAN_EAB */
#ifdef FEATURE_PS_PREFERRED_CELL
  boolean ps_preferred_enabled;
  uint8   num_ps_supported_arfcn_thres;
#endif
} rr_efs_nv_data_t;

  /**
  New cipher key should only overwrite the existing cipher key if RR is active
  when it is received.  If RR is not active, this indicates that the key has
  been generated as a result of some security procedure on WCDMA or TDSCDMA,
  but it is not used until ciphering is activated.  This is signalled to RR by
  RRC_CIPHER_SYNC_IND or TDSRR_CIPHER_SYNC_IND.  On receipt of this, RR will
  overwrite the old cipher key with the new one.
  */
typedef struct
{
  mm_sim_efkc_T      cipher_key;
  mm_sim_efkcsn_T    cipher_key_seq_no;
  mm_gsm_kc_128bit_T gsm_kc_128bit;
  mm_sim_efkc_T      new_cipher_key; /*!< New cipher key, to be used after ciphering is activated on W/T. */
  mm_gsm_kc_128bit_T new_gsm_kc_128bit; /*!< New 128 bit cipher key, to be used after ciphering is activated on W/T. */
  mm_sim_efkcsn_T    new_cipher_key_seq_no; /*!< New cipher key sequence number, to be used after ciphering is activated on W/T. */
  mm_sim_efacc_T     access_class;
  mm_sim_efad_T      ms_opmode;
  mm_sim_tmsi_T      tmsi;
  mm_sim_imsi_T      imsi;
  mm_sim_ptmsi_T     ptmsi;

  boolean cipher_key_valid;
  boolean cipher_key_128_valid;
  boolean cipher_key_seq_no_valid;
  boolean new_cipher_key_valid; /*!< Indicates whether new_cipher_key is valid. */
  boolean new_cipher_key_128_valid;
  boolean new_cipher_key_seq_no_valid; /*!< Indicates whether new_cipher_key_seq_no is valid. */
  boolean access_class_valid;
  boolean ms_opmode_valid;
  boolean tmsi_valid;
  boolean imsi_valid;
  boolean ptmsi_valid;
} rr_sim_data_from_nas_t;

typedef struct
{
  rr_sim_data_from_nas_t   sim_data;
  word                     imsi_mod_1000;
#if defined (FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION)   
  uint32                   imsi_mod_10000000;
#endif /* FEATURE_GERAN_EXTENDED_DRX || FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

} rr_sim_data_t;

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
extern rr_static_general_data_t* rr_static_general_get_data_ptr( void );
extern rr_scell_backup_t* rr_static_get_scell_backup_ptr( void );
extern rr_public_store_data_t* rr_public_store_get_data_ptr( void );
extern void rr_public_store_task_start_init(void);

extern uint8 rr_nv_get_data_space_index(const sys_modem_as_id_e_type as_id);
extern rr_nv_data_t* rr_nv_get_data_ptr(void);
extern rr_efs_nv_data_t* rr_nv_get_efs_data_ptr(void);
extern rr_sim_data_t *rr_sim_get_data_ptr( void );

extern forbidden_lai_list_T* rr_get_forbidden_lai_list_ptr(void);
extern void rr_clear_forbidden_lai_list_ptr(void);
extern void rr_store_forbidden_lai_list(rr_forbidden_lai_update_req_T *forbidden_lai_update_req );

extern void rr_init_barred_cell_list(void );
extern uint8 rr_get_barred_cell_count(void );
extern void rr_update_barred_cell_remaining_time(void );
extern void rr_process_barred_cell_timer_expiry(void );

extern rr_l1_gsm_stop_reason_T rr_l1_gsm_stop_reason_get(void );
extern void rr_l1_gsm_stop_reason_set(rr_l1_gsm_stop_reason_T gsm_stop_reason );
extern void set_rr_deact_reason(rr_deact_reason_t reason);
extern rr_deact_reason_t get_rr_deact_reason(void);
extern void set_rr_mm_gsm_stop_reason(rr_mm_gsm_stop_reason_T reason);
extern rr_mm_gsm_stop_reason_T get_rr_mm_gsm_stop_reason(void);
extern void rr_static_general_init(void);
extern void rr_clear_static_backup();
extern void rr_set_scell_meas_timestamp(uint32 scell_meas_timestamp);
extern uint32 rr_get_scell_meas_timestamp(void);
extern void rr_send_cell_select_info_rsp(boolean is_ue_camped, byte rxlev_access_min );
extern void rr_gprs_debug_init(void );
extern void rr_debug_log_rr_l1_message(rr_l1_message_id_enum_T message_id,uint32 pended_count);
extern void rr_debug_log_mm_rr_message(mm_rr_message_id_enum_T message_id,uint32 pended_count);
void rr_static_debug_log_dyn_pend_q_message(
  rr_cmd_type             *cmd_ptr,
  rr_cmd_type             *pend_queue_ptr,
  uint8                   message_set,
  mm_rr_message_id_enum_T message_id);
void rr_static_log_freed_saved_q_message(
  rr_cmd_type             *cmd_ptr,
  uint8                   message_set,
  mm_rr_message_id_enum_T message_id);
void rr_static_log_added_saved_q_message(
  rr_cmd_type             *cmd_ptr,
  uint8                   message_set,
  mm_rr_message_id_enum_T message_id);

#endif
