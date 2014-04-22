 /*!
  @file
  nb1fw_log_sys.h

  @brief
  NB1 FW SYS logging interface

  @detail
  NB1 FW SYS logging interface

*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/nb1/api/nb1fw_log_sys.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/19/17    SV      Created the header file for SYS/Common Logging
===========================================================================*/

#ifndef NB1FW_LOG_SYS_H
#define NB1FW_LOG_SYS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "intf_nbfw_app.h"
#include "intf_nb1fw_sys.h"
#include "intf_nb1fw_srch.h"
#include "intf_nb1fw_dl.h"
#include "intf_nb1fw_ul.h"

/*===========================================================================

      Macros

===========================================================================*/

/**< Size od EFS data sent through Sys EFS log packet */
#define NB1FW_LOG_EFS_NUM_BYTES 320

/**< number of packets buffered before submit for DRX log packet */
#define NB1FW_LOG_SYS_DRX_NUM_BUFS 20

/*===========================================================================

      Typedefs

===========================================================================*/

/*==========================================================================*/
/* NB1 FW Sys EFS Summary (Log code ID: 0xB202)
 *---------------------------------------------------------------------------*/

/**< EFS Log packet shows the FW EFS values configured 
  * Min Size = 352 bytes 
  * Max Size = 352 bytes 
  * Display Format:
  */
typedef struct
{
  /* 0 .. 32 */
  uint32  version      : 8;  /**< version number of log packet */
  uint32               : 24; /**< not used */

  /* 32 .. 352 */
  uint8  efsData[NB1FW_LOG_EFS_NUM_BYTES]; /**< EFS Data buffer of 320 bytes */
} nb1fw_log_sys_efs_ind_s;


/*==========================================================================*/
/* NB1 FW Sys Disable Rx (or) Sleep Summary (Log code ID: 0xB203)
 *---------------------------------------------------------------------------*/

/** 
 *  DRX logging structure
 */
typedef struct
{

  /*16..23*/
  uint8  logType        : 1;         /**< Indication if the log data is for Resource (or) DRX State
                                         * 0 = Resource info, 1 = DRX state info*/
  uint8                 : 7;         /**< not used */

  union
  {
    struct
    {
      uint8  vote       : 2;   /**< Indication that log entry is for resource vote
                                  * 0 = DISABLED, 1 = INACTIVE, 2 = VOTE_DRX, >=3 = Dont Display
                                  */
      uint8  voteRsc    : 4;   /**< Type of resource that voted
                                  * 0 = PBCH, 1 = SIB, 2 = PDCCH, 3 = PDSCH, 4 = NRS, 8 = EXTENDED_SLEEP, Else Dont Display
                                  */
      uint8             : 2;   /**< not used */
    } rscData;

    struct
    {
      uint8 hdMode      : 1;   /**< Current Half-Duplex mode
                                  * Displayed as String 0 = DL, 1 = NON_DL
                                  */  
      uint8 drxState    : 1;   /**< Current DRX State
                                  * Displayed as String 0 = INACTIVE, 1 = ACTIVE
                                  */
      uint8 rfOffMode   : 2;   /**< Current Rx mode
                                  * Displayed as String 0 = None, 1 = Signal, 2 = Full
                                  */
      uint8             : 4;   /**< not used */
    } drxData;
  };

  /*24..47*/
  uint16 startGSubFn;       /**< Start SubFrame number for current state */      
  uint16 endGSubFn;        /**< End Subframe number when state changes back */       

} nb1fw_log_sys_drx_s;

/**< The DRx summary shows information on when the Rx Signal/Complete path were turned off,
  * resource states that led to this Rx state change 
  * Min Size = 2 bytes 
  * Max Size = 122 bytes 
  * Display Format:
  * |-----------------------------------------------------------------------|
  * | DRx Summary                                                           |
  * |------------------------------------------------------------------------|
  * | HD Mode | State    | Rf Mode   | Curr SubFn  | End SubFn  | Resources  |
  * |------------------------------------------------------------------------| 
  * |         |          |           |             |            | vote | end | 
  * |------------------------------------------------------------------------| 
  * | DL      | ACTIVE   | Signal    | 640         | 643        | 0    |     |
  * |         |          |           |             |            | 2    | 643 |
  * |         |          |           |             |            | 2    | 653 |
  * |         |          |           |             |            | 0    |     |
  * |         |          |           |             |            | 0    |     |
  * |------------------------------------------------------------------------| 
  * | DL      | INACTIVE |           | 643         |            | 0    |     |
  * |         |          |           |             |            | 1    | 645 |
  * |         |          |           |             |            | 2    | 653 |
  * |         |          |           |             |            | 0    |     |
  * |         |          |           |             |            | 0    |     |
  * |------------------------------------------------------------------------| 
  */
typedef struct
{
  /* 0..15 */
  uint8  version; /**< version number of log packet */
  uint8  bufCnt;  /**< Number of valid items logged */
    
  /* 16..975 */
  nb1fw_log_sys_drx_s buf[NB1FW_LOG_SYS_DRX_NUM_BUFS]; /**< Logging buffer */

} nb1fw_log_sys_drx_ind_s;


/*==========================================================================*/
/* NB1 FW Sys FW External command and responses (Log code ID: 0xB200)
 *---------------------------------------------------------------------------*/

typedef union
{
  /**< Command to enable NB1FW App */
  nbfw_app_enable_req_s app_enable_req;
  /**< Responce to enable NB1FW App */
  nbfw_app_enable_cnf_s app_enable_cnf;

  /**< Command to disable NB1FW App */
  nbfw_app_disable_req_s  app_disable_req;
  /**< Responce to disable NB1FW App */
  nbfw_app_disable_cnf_s  app_disable_cnf;

  /**< Command to update cell */
  nb1fw_sys_cell_update_req_s sys_cell_update_req;          
  /**< Responce to update cell */
  nb1fw_sys_cell_update_cnf_s sys_cell_update_cnf;

  /**< Command to config cell */
  nb1fw_sys_cell_config_req_s sys_cell_config_req;
  /**< Responce to config cell */
  nb1fw_sys_cell_config_cnf_s sys_cell_config_cnf;

  /**< Command to config NB1FW App */
  nb1fw_sys_config_app_req_s sys_config_app_req;
  /**< Responce to config NB1FW App */
  nb1fw_sys_config_app_cnf_s sys_config_app_cnf;

  /**< Command to deconfig cell */
  nb1fw_sys_cell_deconfig_req_s sys_cell_deconfig_req;      
  /**< Responce to deconfig cell */
  nb1fw_sys_cell_deconfig_cnf_s sys_cell_deconfig_cnf;

  /**< Command to carrier switch */
  nb1fw_sys_mc_config_req_s nb1fw_sys_mc_config_req;
  /**< Responce to carrier switch */
  nb1fw_sys_mc_config_cnf_s nb1fw_sys_mc_config_cnf;
    
  /**< Command to config cell */
  nb1fw_sys_cell_mc_config_req_s sys_cell_mc_config_req;    
  /**< Responce to config cell */
  nb1fw_sys_cell_mc_config_cnf_s sys_cell_mc_config_cnf;

  /**< Command to get cell stats */
  nb1fw_sys_cell_stat_req_s sys_cell_stat_req;
  /**< Responce to get cell stats */
  nb1fw_sys_cell_stat_cnf_s sys_cell_stat_cnf;

  /**< Command to config log mask */
  nb1fw_sys_log_mask_config_req_s sys_log_mask_config_req;
  /**< Responce to config log mask */
  nb1fw_sys_log_mask_config_cnf_s sys_log_mask_config_cnf;

  /**< Cell stats Indication */
  nb1fw_sys_cell_stat_ind_s sys_cell_stat_ind;

  /**< Command to scan full search */
  nb1fw_srch_full_freq_scan_req_s srch_full_freq_scan_req;
  /**< Responce to scan full search */  
  nb1fw_srch_full_freq_scan_cnf_s srch_full_freq_scan_cnf;

  /**< Command to scan search list */
  nb1fw_srch_list_freq_scan_req_s srch_list_freq_scan_req;  
  /**< Responce to scan search list */
  nb1fw_srch_list_freq_scan_cnf_s srch_list_freq_scan_cnf;

  /**< Command to search blocked cell list */
  nb1fw_srch_blocked_cell_list_req_s srch_blocked_cell_list_req;
  /**< Responce to search blocked cell list */
  nb1fw_srch_blocked_cell_list_cnf_s srch_blocked_cell_list_cnf;

  /**< Command to start init cell search */
  nb1fw_srch_init_acq_req_s srch_init_acq_req;
  /**< Responce to start init cell search */
  nb1fw_srch_init_acq_cnf_s srch_init_acq_cnf;

  /**< Command to start cell search */
  nb1fw_srch_cell_srch_req_s srch_cell_srch_req;
  /**< Responce to start cell search */
  nb1fw_srch_cell_srch_cnf_s srch_cell_srch_cnf;

  /**< Command to abort search */
  nb1fw_srch_abort_req_s srch_abort_req;
  /**< Responce to abort search */
  nb1fw_srch_abort_cnf_s srch_abort_cnf;

  /**< Command to decode pbch */
  nb1fw_dl_pbch_decode_req_s dl_pbch_decode_req;
  /**< Responce to decode pbch */
  nb1fw_dl_pbch_decode_cnf_s dl_pbch_decode_cnf;

  /**< Command to stop pbch decode */
  nb1fw_dl_pbch_decode_stop_req_s dl_pbch_decode_stop_req;
  /**< Responce to stop pbch decode */
  nb1fw_dl_pbch_decode_stop_cnf_s dl_pbch_decode_stop_cnf;

  /**< Command to update SIB info */
  nb1fw_dl_si_update_info_req_s dl_si_update_info_req;
  /**< Responce to update SIB info */
  nb1fw_dl_si_update_info_cnf_s dl_si_update_info_cnf;

  /**< Command to start collecting SIB */
  nb1fw_dl_si_start_collect_req_s dl_si_start_collect_req;
  /**< Responce to start collecting SIB */
  nb1fw_dl_si_start_collect_cnf_s dl_si_start_collect_cnf;

  /**< Command to stop collecting SIB */
  nb1fw_dl_si_stop_collect_req_s dl_si_stop_collect_req;
  /**< Responce to stop collecting SIB */
  nb1fw_dl_si_stop_collect_cnf_s dl_si_stop_collect_cnf;

  /**< Command to config pdcch */
  nb1fw_dl_pdcch_config_req_s dl_pdcch_config_req;
  /**< Responce to config pdcch */
  nb1fw_dl_pdcch_config_cnf_s dl_pdcch_config_cnf;

  /**< Command to change pdcch rnti */
  nb1fw_dl_pdcch_rnti_change_req_s dl_pdcch_rnti_change_req;
  /**< Responce to change pdcch rnti */
  nb1fw_dl_pdcch_rnti_change_cnf_s dl_pdcch_rnti_change_cnf;

  /**< Command to deconfig pdcch */
  nb1fw_dl_pdcch_deconfig_req_s dl_pdcch_deconfig_req;  
  /**< Responce to deconfig pdcch */
  nb1fw_dl_pdcch_deconfig_cnf_s dl_pdcch_deconfig_cnf;

  /**< Command to get cell measurement */
  nb1fw_dl_meas_cell_req_s dl_meas_cell_req;
  /**< Responce to get cell measurement */
  nb1fw_dl_meas_cell_cnf_s dl_meas_cell_cnf;

  /**< Command to abort cell measurement */
  nb1fw_dl_meas_cell_abort_req_s dl_meas_cell_abort_req;
  /**< Responce to abort cell measurement */
  nb1fw_dl_meas_cell_abort_cnf_s dl_meas_cell_abort_cnf;

  /**< MIB Indication */
  nb1fw_dl_pbch_mib_ind_s dl_pbch_mib_ind;

  /**< SIB Indication */
  nb1fw_dl_sib_ind_s dl_sib_ind;

  /**< PAGE Indication */
  nb1fw_dl_page_failed_ind_s dl_page_failed_ind;

  /**< PDCCH DCI Indication */
  nb1fw_dl_pdcch_dci_ind_s dl_pdcch_dci_ind;

  /**< PDSCH Data Indication */
  nb1fw_dl_data_ind_s dl_data_ind;

  /**< DL decode stats Indication */
  nb1fw_dl_decode_stat_ind_s dl_decode_stat_ind;

  /**< DL Channel end Indication */
  nb1fw_dl_chan_end_ind_s dl_chan_end_ind;

  /**< RLM Indication */
  nb1fw_dl_rlm_ind_s dl_rlm_ind;

  /**< Command to config pusch */
  nb1fw_ul_pusch_config_req_s ul_pusch_config_req;
  /**< Responce to config pusch */
  nb1fw_ul_pusch_config_cnf_s ul_pusch_config_cnf;

  /**< Command to encode pusch */
  nb1fw_ul_pusch_encode_req_s ul_pusch_encode_req;
  /**< Responce to encode pusch */
  nb1fw_ul_pusch_encode_cnf_s ul_pusch_encode_cnf;

  /**< Command to schedule pusch data */
  nb1fw_ul_pusch_schedule_data_req_s ul_pusch_schedule_data_req;
  /**< Responce to schedule pusch data */
  nb1fw_ul_pusch_schedule_data_cnf_s ul_pusch_schedule_data_cnf;

  /**< Command to schedule pusch ack/nack */
  nb1fw_ul_pusch_schedule_ack_nack_req_s ul_pusch_schedule_ack_nack_req;  
  /**< Responce to schedule pusch ack/nack */
  nb1fw_ul_pusch_schedule_ack_nack_cnf_s ul_pusch_schedule_ack_nack_cnf;

  /**< Command to schedule prach */
  nb1fw_ul_prach_schedule_req_s ul_prach_schedule_req;        
  /**< Responce to schedule prach */
  nb1fw_ul_prach_schedule_cnf_s ul_prach_schedule_cnf;

  /**< Command to update pusch */
  nb1fw_ul_pusch_ta_update_req_s ul_pusch_ta_update_req;    
  /**< Responce to update pusch */
  nb1fw_ul_pusch_ta_update_cnf_s ul_pusch_ta_update_cnf;

  /**< Command to deconfig pusch */
  nb1fw_ul_pusch_deconfig_req_s ul_pusch_deconfig_req;      
  /**< Responce to deconfig pusch */
  nb1fw_ul_pusch_deconfig_cnf_s ul_pusch_deconfig_cnf;

  /**< Command to abort pusch */
  nb1fw_ul_pusch_abort_req_s ul_pusch_abort_req;            
  /**< Responce to abort pusch */
  nb1fw_ul_pusch_abort_cnf_s ul_pusch_abort_cnf;

  /**< Command to abort prach */
  nb1fw_ul_prach_abort_req_s ul_prach_abort_req;            
  /**< Responce to abort prach */
  nb1fw_ul_prach_abort_cnf_s ul_prach_abort_cnf;

  /**< UL Freq. error Indication */
  nb1fw_ul_freq_error_update_ind_s ul_freq_error_update_ind;

  /**< UL Channel end Indication */
  nb1fw_ul_chan_end_ind_s ul_chan_end_ind;

  /**< HDET Update Indication */
  nb1fw_ul_hdet_update_ind_s ul_hdet_update_ind;

} nb1fw_log_cmd_rsp_collection_u;

/**< The Request Response packet shows information on the requests sent to FW
  *  and the corresponding responses from FW 
  * Min Size = 8 bytes 
  * Max Size = 560 bytes 
  * Display Format:
  * |-----------------------------------------------------------------------|
  * | FW Request/Response                                                   |
  * |-----------------------------------------------------------------------|
  *   Source  : 7756
  *   UMID    : 0x1e470203
  *   Payload : [params in packed format]
  */
typedef struct
{
  uint32                          version  : 8;    /**< version number of log packet */
  uint32                          source   : 16;   /**< Source module of the command/responce */
  uint32                                   : 8;    /**< not used */
  uint32                          umid;            /**< UMID of the command/responce
                                                     * Displayed as Hex value
                                                     */
  nb1fw_log_cmd_rsp_collection_u  params;          /**< Parameters sent with the command/response
                                                     * Display packed data. No Parsing needed
                                                     */

} nb1fw_log_sys_cmd_rsp_ind_s;

#endif /* NB1FW_LOG_SYS_H */


