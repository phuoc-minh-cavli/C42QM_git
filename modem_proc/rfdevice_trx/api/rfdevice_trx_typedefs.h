#ifndef RFDEVICE_TRX_TYPEDEFS_H
#define RFDEVICE_TRX_TYPEDEFS_H

/*!
  @file
  rfdevice_trx_typedef.h

  @addtogroup RFDEVICE_TRX_COMMON
*/
/*==============================================================================

  Copyright (c) 2015-2020 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //components/rel/rfdevice_trx.mpss/1.20/api/rfdevice_trx_typedefs.h#2 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
10/26/20   up     Added enum RFDEVICE_TRX_TRIG_TX_RESTORE_TBLS 
07/26/19   up     Added call flow type RFDEVICE_TRX_PRELOAD_IOTFE_THERM.
03/18/19   bm     Added call flow type RFDEVICE_TRX_TRIG_RX_RESTORE_TBLS
02/26/19   tv     Added call flow type RFDEVICE_TRX_LOAD_JDET_TBLS to load JDET templates
02/19/19   up     Added call flow type RFDEVICE_TRX_LOAD_THERM_TEMPLATE
02/06/19   up     rfdevice_rx_config_info_type freq unit is changed to Hz
01/15/19   am     Added PSP_HDET callflow support
10/22/18   bm     Updated the abs_wait offset in bytes within RGI template 
10/01/18   am     Added support for cmn_rxtx_mapping_tbl, bsp_rxtx_mapping_tbl.
                  Modified super_call_flow types
09/27/18   up     Modified rfdevice_tx_config_info_type::initialize
09/27/18   up     Changes to remove FFH banking changes for LTE RX.
09/18/18   up     Added RFDEVICE_TRX_LTE_RX_PLL_DATA_FIXUP_LEN defintion
08/14/18   up     Added structure rfdevice_trx_rffe_script_entry_holder_type
08/08/18   up     Modified rfdevice_rx_config_info_type
08/09/18   bm     Added the macro ABS_TMR_OFFSET_IN_RGI_TMPLT
08/08/18   up     Renamed macro MAX_PLL_OPTION_PER_BAND to MAX_PLL_OPTION_PER_CHAN
08/03/18   up     Added new structure rfdevice_trx_pll_opt_reg_bank_mapping_type
07/25/18   up     Modified rfdevice_tx_config_info_type
07/24/18   am     Added structure to load HDET mapping table as part of TX Tune
07/19/18   up     Added #defines for timer fixup data in RXAGC script
07/19/18   up     Modified supercallflow enum and structure rfdevice_tx_ag_tbls_type
                  for RGI0 script preloading.
07/13/18   up     Updated tx_config_info.initialize to update BW param too
07/06/18   tv     Added call flows for BSP_TRIG & LIN_MODE
06/28/18   up     Removed the enum rfdevice_trx_rffe_core_type, not required
05/10/18   up     Added bw entry to rfdevice_tx_config_info_type and 
                  rfdevice_rx_config_info_type
05/10/18   up     Changes to merge timer and data fixup enums and coresponding 
                  driver changes
04/26/18   up     Added two data fixup tokens RFDEVICE_TRX_DATA_FIXUP_TOKEN_TX_FFH
                  and RFDEVICE_TRX_DATA_FIXUP_TOKEN_RX_FFH.
04/19/18   up     Modified rfdevice_rx_agc_mapping_tbl_type 
03/16/18   up     Removed rfdevice_trx_sig_path_mask_arr_type for SDR105 and 
                  Code cleanup
02/16/18   ssr    Added RFFE bus info struct. Also added Script wrapper mode enum
02/16/18   up     Code cleanup
02/09/18   up     FR 46405: SDR105 TRX (IOTRF)
                  Added a element of type rfdevice_trx_spll_tx_pll_info_type
                  to the structure rfdevice_rx_config_info_type. 
                  rfdevice_trx_spll_tx_pll_info_type holds TX pll details
03/03/17   ssr    Compilation error fix
03/02/17   ssr    Updates for Static LIF feature implementation
02/21/17   aks    Read IIP2 data from QFPROM if present
02/16/17 snk/aguo GSM Rx tune script building optimization
01/20/17   aks    Fix crash when tuning single chain PRXonD
01/20/17   aks    Added is_standalone_mode flag to reg_rf_cfg struct
01/11/17   aks    Added elements iip2_icode and iip2_qcode to rfdevice_rx_ag_tbls_type struct
12/23/16   ssr    Changes to support DLIF feature implementation
12/14/16   aks    Removed pll_option from rfdevice_tx_config_info_type struct
12/14/16   aguo   Added rfdevice_trx_data_fixup_event_type enum definition
                  Added boolean fixup_with_zeros to struct rfdevice_rx_agc_mapping_tbl_type and a new initialization func for the struc (used for GSM case only)
12/13/16   snk    Updated rfdevice_trx_pll_config_type, RX/Tx ag_tbls_type struct
11/22/16   aguo   Moved the struct rfdevice_trx_txagc_data_buffer_type and #define RFDEVICE_TRX_MAX_TX_RGI to rflm_rfdevice_typedefs.h
10/07/16   ash    Added code for Dynamic ports (MIMO only ports and RX0/RX0B)
10/07/16   ssr    Added SPLL-TDD specific members to Rx config info type struct
08/18/16   ash    Changes to account for Path Assn dependency on RX IIP2
08/23/16   ssr    afc_freq_hz offset frequency type changed from uint64 to int32
08/16/16   aks    Changes to pick correct bbf_sca mode in LTE intra CA
08/05/16   ash    Added entry to store num of bytes of memory reclaimed
08/10/16   aks    Pick PLL option based on DAC Rate for GSM in Cal mode
08/09/16   aks    Allocate memory on the heap for req_rf_cfg struct in API: get_config_chain_params() 
08/05/16   aks    Renamed (default_band_mode, actual_band_mode) to (rfc_port_band_mode, tune_port_band_mode)
07/26/16   ssr    Changes to config info member variable nominal frequency to be in Hz
07/25/16   shb    Implemented control of RxSM during RX enable/disable
07/22/16   ssr    Added offset frequency update during config_info initialize
07/19/16   ash    Added qfprom rev bmsk dependency for rx iip2
07/19/16   aks    Return union of ADC/DAC rates
07/19/16   bm     Added dlif enable/disable template pointers to 
                  rfdevice_rx_ag_tbls_type struct
07/15/16   ssr    Added common FBRx mode into FBRx config info struct
07/12/16   ka     Added support for discontiguous rx agc gain data
06/24/16   sd     Moved all struct member functions to rfdevice_trx_typedefs.cpp
06/15/16   ash    Added support for RX IIP2
06/06/16   bm     Removed path_assn_mapping_tbl from fbrx_ag_tbls_type structure
05/19/16   aks    Updated Scouting API to return the Rx/Tx port for LTE B46 based on frequency
05/04/16   ash    Moved applicable structs to rflm_rfdevice_typedefs.h
05/03/16   shb    Added rfdevice_trx_sig_path_mask_arr_type
05/04/16   aks    Ensure ADC rate does not change in LTE Standalone mode during Tx ON/OFF
04/29/16   aks    Add support to avoid Rx retune during Tx ON/OFF in standalone mode
04/28/16   aks    Added macro: RFDEVICE_TRX_STATIC_ASSERT
04/27/16   hm     Port mapping with sig path dimension(RX + CDMA Tx)
04/25/16   aks    Updated scouting API to return both BBF and LNA modes
04/20/16   ash    Added support for TX RSB
04/08/16   sd     Support for WTR scouting during every burst
04/08/16   sd     Support for LINUX OFT
04/05/16   aks    Added an extra parameter to chain_info struct: full_band
02/09/16   aks    Updates to the scouting API to return notches and ADC/DAC clk rates
02/02/16   hm     wxe protection implementation
01/28/16   snk    Added retune_req_on_tuneback to rfdevice_trx_chain_specific_info_for_scouting
01/28/16   shb    Removed Tx LO div ratio enum hardcoding
01/26/16   snk    Added support for chip process in OFT
01/25/16   snk    Moved chip_oper_mode to rfdevice_trx_config_chain_params_type
01/19/16   aks    Update scouting API to return PLL option/LO Div ratio for each chain
01/15/16   hm     Therm implementation
01/14/16   aks    Support to pick the LO div Ratio based on LTE CA/WCDMA DBDC Combination
01/08/16   aks    Added struct: rfdevice_trx_chain_specific_info_for_scouting
                  Added is_active flag to rfdevice_rx_chain_info_type struct
11/18/15   shb    Return DA Pout in get_txagc_data()
10/09/15   hm     Moved gains tate info from rx_config_info to new struct for GSM Gx implementaiton
10/08/15   aks    Moved enum rfdevice_trx_fbrx_oper_mode_type to rfdevice_cmn_type_defs.h
10/02/15   shb    Implemented GSM TX PLL calculation
10/02/15   hm     Added ilna_gs and bbf_gs for DRX 
10/02/15   shb    Reduced RFDEVICE_TRX_MAX_TX_RGI to 100 to save heap memory
10/01/15   shb    Added rfdevice_trx_txagc_data_buffer_type
10/01/15   shb    Updated Tx scouting struct to contain LO div ratio enum
10/01/15   aks    Re-ordered struct rfdevice_trx_config_rx_chain_params_type
09/28/15   aks    Updated initialize method in struct rfdevice_trx_wxe_mem_info_type
                  to accept ref to wxe template and trig addr 
09/28/15   hm     bug fix in rfdevice_trx_wxe_mem_info_type.Initialize()
09/26/15   shb    Added ilna_gs, bbf_gs to rx_config_info struct
09/25/15   aks    Added overloaded initialize method in struct rfdevice_trx_wxe_mem_info_type
                  for template programming 
09/23/15   aks    Set PLL Option by default to 1
09/22/15   aks    Include ptr to other Rx chain in rfdevice_trx_config_rx_chain_params_type struct
                  Added API to get VCO combinations which can share LNA from WTR intf
09/09/14   aks    Scouting API update - Init Tx chain info
09/04/14   aks    Scouting API updates- Only loop through the active VCOs in the system
                  Added constructors for structures used by Scouting API
09/02/15   hm     Added struct rfdevice_tx_rgi_ag_tbls_type for RGI template programming
09/01/15   aks    Scouting API updates- Store scouting API params in 
                  rfdevice_trx struct instead of wtr5975 struct
08/28/15   aks    Updates for Scouting API
08/17/15   shb    Moved rfdevice_trx_tech_type to rfdevice_interface
07/24/15   hm     MOVE implementation for working around MOVE16 HW bug
07/17/15   aks    Added pwr_mode, pll_option and pll_config to rfdevice_fbrx_config_info_type struct
07/16/15   aks    Added enum to indicate FBRx Oper mode 
06/22/15   aks    Updated load_tx_ag_tbl to handle ext ref:RFDEVICE_TRX_EXTERNAL_LUT_TXRGI_0_REF 
06/19/15   aks    Added support for WXE EOC in super call flow
06/18/15   hm     Added #defines for invalid value definitions
                  Defined enumeration for wxe trigger
06/18/15   aks    Updated init_dc_cal_tbls to return ptr to all Rx AGC gain words
06/17/15   aks    Added struct rfdevice_tx_config_info_type, rfdevice_trx_chip_process_list
06/17/15   aks    Added struct rfdevice_tx_ag_tbls_type
06/16/15   aks    Added struct rfdevice_rx_pll_tune_params_type
06/16/15   shb    Added rfdevice_trx_pll_config_type
06/16/15   shb    Deleted RFDEVCE_TRX_PATH_ASSN_ALL
06/15/15   hm     Script building bug fixes
06/12/15   aks    Added enum: rfdevice_trx_super_call_flow_type
06/11/15   aks    Expanded struct rfdevice_rx_tune_ag_tbls_type
06/08/15   aks    Moved rfdevice_trx_write_wxe_ag_tbl_return_type to within rfdevice_trx_common_class
06/02/15   aks    Moved #defines to indicate size of coded instr/data word to within common_trx_class driver
06/02/15   aks    Copied enums for PLL and script buffer class to separate files
05/26/15   aks    Added struct: rfdevice_trx_mapping_tbl_type and enum RFDEVCE_TRX_PATH_ASSN_ALL
04/20/15    hs    Initial version
==============================================================================*/

#include "rfdevice_trx_typedef_ag.h"
#include "rfdevice_cmn_type_defs.h"
#include "modem_mem.h"
#include "rflm_ccs_task_defs.h"

/*===========================================================================*/
/*                       Preprocessor directives                             */
#define RFDEVICE_TRX_INVALID_8_BIT    0xFF
#define RFDEVICE_TRX_INVALID_16_BIT   0xFFFF
#define RFDEVICE_TRX_INVALID_32_BIT   0XFFFFFFFF
#define RFDEVICE_TRX_MAX_RX_VCO_PER_CHIP 1
#define RFDEVICE_TRX_MAX_TX_VCO_PER_CHIP 1
#define RFDEVICE_TRX_INVALID_LO_DIV   0xFF
#define RFDEVCE_TRX_INVALID_PLL_OPTION 0
#define RFDEVICE_TRX_MAX_RX_TX_VCOS   RFDEVICE_TRX_MAX_RX_VCO_PER_CHIP + RFDEVICE_TRX_MAX_TX_VCO_PER_CHIP

#define ABS_TMR_OFFSET_IN_RXAGC_TMPLT 16 
#define ABS_TMR_OFFSET_IN_RGI_TMPLT 32 // 1 instr word + 7 data words before abs_wait instr
#define TMR_FIXUP_CLR_DATA 0x24000000
#define RFDEVICE_TX_AGC_REG2_IDX 1
#define MAX_PLL_OPTION_PER_CHAN  3
#define RFDEVICE_TRX_LTE_RX_PLL_DATA_FIXUP_LEN 36
/*--------------------------------------------------------------------------------------------------------------------*/
/*! 
    @brief
    Macro to convert mirco-second unit to USTMR unit (19.2MHz)
    ceil((time_us * 19.2) 
*/
#define RFDEVICE_TRX_CONV_US_TO_USTMR(u)  (u * 96 + 4 )/5

/*! @brief 
  Macro to catch assertions at Compile time.  Based on requirement that array size > 0.
  The opening/closing braces create a local namespace so that we don't get multiple
  redefinition errors when macro is invoked back-to-back with the same msg */
#define RFDEVICE_TRX_STATIC_ASSERT(condition, msg) \
  { typedef char msg[condition ? 1:-1]; }

/*===========================================================================*/

/*===========================================================================*/
/*                            Enum definitions                               */
/*===========================================================================*/

/*! It defines the rfdevice_trx_super_call_flow_type enum */
enum rfdevice_trx_super_call_flow_type
{
  RFDEVICE_TRX_LOAD_CMN_TBLS,
  RFDEVICE_TRX_LOAD_BSP_TBLS,
  RFDEVICE_TRIG_BSP_GSM,  
  RFDEVICE_TRX_LOAD_LIN_TBLS,
  RFDEVICE_TRX_LOAD_TUNE_TBLS,
  RFDEVICE_TRX_LOAD_LNA_MODE_TBLS,
  RFDEVICE_TRX_LOAD_PATH_ASSN_TBLS,
  RFDEVICE_TRIG_WXE_MEM_START,
  RFDEVICE_TRX_INSERT_WXE_EOC,
  RFDEVICE_TRX_LOAD_RGI0,
  RFDEVICE_TRX_INSERT_EOC_DATA_FIXUP,
  RFDEVICE_TRX_LOAD_CMN_RXTX_TBLS,
  RFDEVICE_TRX_LOAD_BSP_RXTX_TBLS,
  RFDEVICE_TRX_LOAD_PATH_ASSN_HDET_TBLS,
  RFDEVICE_TRX_LOAD_RX_RESTORE_TBLS,
  RFDEVICE_TRX_LOAD_TX_RESTORE_TBLS,
  RFDEVICE_TRX_LOAD_THERM_TEMPLATE,
  RFDEVICE_TRX_LOAD_JDET_TBLS,
  RFDEVICE_TRX_TRIG_RX_RESTORE_TBLS,
  RFDEVICE_TRX_TRIG_TX_RESTORE_TBLS,
  RFDEVICE_TRX_PRELOAD_IOTFE_THERM,
  RFDEVICE_TRX_SUPER_CALL_FLOW_SEQ_END,
};


/*===========================================================================*/
/*                          Structure definitions                            */
/*===========================================================================*/

/*----------------------------------------------------------------------------*/
/*! @brief Struct with RFFE channel and slave id pair */
struct rfdevice_trx_rffe_bus_info
{
  uint8 rffe_channel;
  uint8 rffe_slave_id;
  rf_ccs_rffe_clk_enum clk_speed;

  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/
  /*! @brief Constructor ensures elements are initialized to
             invalid values when object is created */
  rfdevice_trx_rffe_bus_info()
  : rffe_channel(RFDEVICE_TRX_INVALID_8_BIT),
    rffe_slave_id(RFDEVICE_TRX_INVALID_8_BIT),
    clk_speed(RF_HAL_RFFE_INVALID_CLK)
  {
  }

  /*===========================================================================*/
  /*                         Member functions                                  */
  /*===========================================================================*/
  /*! @brief Initializes elements of struct */
  void initialize
  (
    uint8 rffe_channel,
    uint8 rffe_slave_id,
    rf_ccs_rffe_clk_enum clk_speed
  );

};

/*! @brief It defines the rfdevice_trx_wxe_mem_info_type struct */
struct rfdevice_trx_wxe_mem_info_type
{
  /*===========================================================================*/
  /*                           Data Members                                    */
  /*===========================================================================*/

  /*! @brief Current WXE Address for instruction */
  uint16 curr_wxe_addr_instr;

  /*! @brief Current WXE Address for data */
  uint16 curr_wxe_addr_data;

  /*! @brief Ptr to the WXE mem table */
  rfdevice_trx_wxe_mem_tbl_type wxe_mem_tbl;

  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/

  rfdevice_trx_wxe_mem_info_type()
  {
    curr_wxe_addr_instr = 0;
    curr_wxe_addr_data = 0;
    memset(&wxe_mem_tbl, 0, sizeof(rfdevice_trx_wxe_mem_tbl_type));
  }

  /*===========================================================================*/
  /*                         Member functions                                  */
  /*===========================================================================*/
  void initialize
  (
    const rfdevice_trx_wxe_mem_tbl_type* wxe_mem_tbl
  );

  void initialize
  (
    const rfdevice_trx_wxe_template_type& wxe_template,
    const rfdevice_trx_wxe_mem_tbl_type& wxe_mem_tbl_info
  );
};

/*----------------------------------------------------------------------------*/

/*! @brief 
  Holds pointers to the AG Tables needed to program LUT
 
  @details
  - Stores ptrs to the instruction, data and register lists
  needed to program mapping table
  - Initializes ptrs to NULL in default constructor
  - "Validate" method needs to be invoked to check if member
  variables have been initialized 
 
 
  */

struct rfdevice_rx_agc_mapping_tbl_type
{
  /*===========================================================================*/
  /*                           Data Members                                    */
  /*===========================================================================*/

  /*! Pointer to the Register table */
  const rfdevice_trx_reg_tbl_type* reg_tbl;

  /*! Pointer to the data table for all gain states  */
  const uint16* data_tbl;

  /*! Number of gain states */
  int num_gain_states;

  /*! Indicated if an extra 16-bit padding is required for 32-bit alignment */
  boolean padding_needed;

  /*! Indicates fixup rx agc script with 0's if TRUE */
  boolean fixup_with_zeros;

  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/

  /*! @brief Constructor ensures elements are initialized to 
    invalid values when object is created */
  rfdevice_rx_agc_mapping_tbl_type()
  : reg_tbl(NULL), data_tbl(NULL), num_gain_states(0), padding_needed(FALSE), fixup_with_zeros(FALSE)
  {
  }

  /*===========================================================================*/
  /*                         Member functions                                  */
  /*===========================================================================*/
  /*! @brief Initializes elements of struct */
  boolean initialize
  (
    const rfdevice_trx_reg_tbl_type* reg_tbl,
    const uint16* data_tbl,
    int num_gain_states,
    boolean padding_needed
  );

  /*! @brief Initializes elements of struct */
  boolean initialize_for_data_fixup
  (
    void
  );
};

/*----------------------------------------------------------------------------*/

/*! @brief 
  Holds pointers to the AG Tables needed to program LUT
 
  @details
  - Stores ptrs to the instruction, data and register lists
  needed to program mapping table
  - Initializes ptrs to NULL in default constructor
  - "Validate" method needs to be invoked to check if member
  variables have been initialized 
 
 
  */

struct rfdevice_trx_mapping_tbl_type
{
  /*===========================================================================*/
  /*                           Data Members                                    */
  /*===========================================================================*/

  /*! Pointer to the data table */
  const uint16* data_tbl;

  /*! Pointer to the Register table */
  const rfdevice_trx_reg_tbl_type* reg_tbl;

  /*! Pointer to the instruction table */
  const rfdevice_trx_instr_tbl_type* instr_tbl;

  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/

  /*! @brief Constructor ensures elements are initialized to 
    invalid values when object is created */
  rfdevice_trx_mapping_tbl_type()
  : instr_tbl(NULL),data_tbl(NULL),reg_tbl(NULL)
  {
  }

  /*===========================================================================*/
  /*                         Member functions                                  */
  /*===========================================================================*/
  /*! @brief Initializes elements of struct */
  void initialize
  (
    const rfdevice_trx_instr_tbl_type* instr_tbl,
    const rfdevice_trx_reg_tbl_type* reg_tbl,
    const uint16* data_tbl
  );
};

/*----------------------------------------------------------------------------*/
/*! 
 
  @brief
  Struct contains the mapping tables which need to be loaded
  as part of Tx tune 
  */
struct rfdevice_tx_ag_tbls_type
{
  /*! @brief Contains ptrs to Cmn Mapping tbls */
  rfdevice_trx_mapping_tbl_type cmn_mapping_tbl;
  
  /*! @brief Contains ptrs to Cmn Mapping tbls */
  rfdevice_trx_mapping_tbl_type cmn_rxtx_mapping_tbl;

  /*! @brief Contains ptrs to BSP Mapping tbls */
  rfdevice_trx_mapping_tbl_type bsp_mapping_tbl;

  /*! @brief Contains ptrs to Cmn Mapping tbls */
  rfdevice_trx_mapping_tbl_type bsp_rxtx_mapping_tbl;

  /*! @brief Contains ptrs to Path Assn Mapping tbls */
  rfdevice_trx_mapping_tbl_type path_assn_mapping_tbl;

  /*! @brief Contains ptrs to Tune Mapping tbls */
  rfdevice_trx_mapping_tbl_type tune_mapping_tbl;

  /*! @brief Contains ptrs to LO Div Mapping tbls */
  rfdevice_trx_mapping_tbl_type lo_div_mapping_tbl;

  /*! @brief Contains ptrs to RGI0 Mapping tbls */
  rfdevice_trx_mapping_tbl_type rgi0_mapping_tbl;

  /*! Stores ptr to the BBF LUT */
  const rfdevice_trx_txbbf_lut_type* bbf_lut;

  /*! Stores ptr to RGI LUT */
  const rfdevice_trx_txagc_lut_type* rgi_lut;

  /*! @brief contains instr for PLL template */
  //const rfdevice_trx_wxe_template_type* pll_wxe_templ;

  /*! array of template pointers 0 - pll_calc_template, 1 - pl_option 2, pll_option 3*/
  const rfdevice_trx_wxe_template_type* pll_template[MAX_PLL_OPTION_PER_CHAN];

  /*! @brief contains instr for RGI0 template */
  const rfdevice_trx_wxe_template_type* rgi0_wxe_templ;

  /*! @brief contains wxe_mem_info for RGI0 template  */
   rfdevice_trx_wxe_mem_info_type rgi0_wxe_mem_info;

  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/

  rfdevice_tx_ag_tbls_type()
  : bbf_lut(NULL), rgi_lut(NULL)
  {
    for (uint8 idx =0; idx <MAX_PLL_OPTION_PER_CHAN; idx++)
    {
      pll_template[idx] = NULL;
    }
  }

};

/*----------------------------------------------------------------------------*/
/*! 
 
  @brief
  Struct contains the mapping tables which need to be loaded
  as part of Tx RGI template 
  */
struct rfdevice_tx_rgi_ag_tbls_type
{
  /*! Stores ptr to the BBF LUT */
  const rfdevice_trx_txbbf_lut_type* bbf_lut;

  /*! Stores ptr to RGI LUT */
  const rfdevice_trx_txagc_lut_type* rgi_lut;

  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/

  rfdevice_tx_rgi_ag_tbls_type()
  : bbf_lut(NULL), rgi_lut(NULL)
  {
  }
};

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_rx_iip2_qfprom_info_type struct.
   This holds the QFPROM Sym and the Match Found Flag, both of which
   are used as input to AG seq to obtain rx iip2 code   
*/
struct rfdevice_trx_rx_iip2_qfprom_info_type
{
  /*! QFPROM Match Found Flag, input to AG Seq */
  boolean qfprom_match_found;  

  /*! QFPROM RX IIP2 Symbol */
  uint16  qfprom_rx_iip2_sym;  

  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/

  rfdevice_trx_rx_iip2_qfprom_info_type():
    qfprom_match_found(FALSE), qfprom_rx_iip2_sym(0xFFFF)
  {

  }
};

/*----------------------------------------------------------------------------*/
/*! 
 
  @brief
  Struct contains the mapping tables which need to be loaded
  as part of Rx tune 
  */
struct rfdevice_rx_ag_tbls_type
{
  /*! @brief Contains ptrs to Cmn Mapping tbls */
  rfdevice_trx_mapping_tbl_type cmn_mapping_tbl;

  /*! @brief Contains ptrs to Cmn Mapping tbls */
  rfdevice_trx_mapping_tbl_type cmn_rxtx_mapping_tbl;

  /*! @brief Contains ptrs to BSP Mapping tbls */
  rfdevice_trx_mapping_tbl_type bsp_mapping_tbl;

  /*! @brief Contains ptrs to BSP Mapping tbls */
  rfdevice_trx_mapping_tbl_type bsp_rxtx_mapping_tbl;

  /*! @brief Contains ptrs to BW Mapping tbls */
  rfdevice_trx_mapping_tbl_type bw_mapping_tbl;

    /*! @brief Contains ptrs to Path Assn Mapping tbls */
  rfdevice_trx_mapping_tbl_type lna_mode_mapping_tbl;

  /*! @brief Contains ptrs to Linearity mode Mapping tbls */
  rfdevice_trx_mapping_tbl_type lin_mapping_tbl;

  /*! @brief Contains ptrs to Path Assn Mapping tbls */
  rfdevice_trx_mapping_tbl_type path_assn_mapping_tbl;

  /*! @brief Contains ptrs to Path Assn HDET Mapping tbls */
  rfdevice_trx_mapping_tbl_type path_assn_hdet_mapping_tbl;

  /*! @brief Contains ptrs to Tune Mapping tbls */
  rfdevice_trx_mapping_tbl_type tune_mapping_tbl;

  /*! @brief Contains ptrs to LO Div Mapping tbls */
  rfdevice_trx_mapping_tbl_type lo_div_mapping_tbl;

  /*! @brief Contains ptrs to DLNA Mapping tbls - Needed for DC Cal */
  rfdevice_rx_agc_mapping_tbl_type drx_lna_path_ilna_mapping_tbl;
  rfdevice_rx_agc_mapping_tbl_type drx_lna_path_bbf_mapping_tbl;

  /*! @brief Contains ptrs to PLNA Mapping tbls - Needed for DC Cal */
  rfdevice_rx_agc_mapping_tbl_type prx_lna_path_ilna_mapping_tbl;
  rfdevice_rx_agc_mapping_tbl_type prx_lna_path_bbf_mapping_tbl;

  /*! @brief Contains ptrs to mapping table for rx iip2 */
  rfdevice_trx_mapping_tbl_type rx_iip2_mapping_tbl;

  /*! @brief Contains qfprom bitmask and symbol for rx iip2  */
  rfdevice_trx_rx_iip2_qfprom_info_type rx_iip2_qfprom_info;


  /*! @brief I and Q cal codes */
  uint16 iip2_icode;
  uint16 iip2_qcode;

  /*! @brief contains instr for PLL template */
  //const rfdevice_trx_wxe_template_type* pll_wxe_templ;

  /*! array of template pointers index 0-pll_calc_template, 1-pll_option2, 2-pll_option3*/
  const rfdevice_trx_wxe_template_type* pll_wxe_templ;

  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/

  rfdevice_rx_ag_tbls_type()
    :iip2_icode(0), iip2_qcode(0), pll_wxe_templ(NULL)
  {
  }
  
};

/*----------------------------------------------------------------------------*/
/*!
 
  @brief
  Struct contains the mapping tables which need to be loaded
  as part of TX Tune 
  */
struct rfdevice_hdet_ag_tbls_type
{
	/*! @brief Contains ptrs to Cmn Mapping tbls */
	rfdevice_trx_mapping_tbl_type cmn_mapping_tbl;
	
	/*! @brief Contains ptrs to BSP Mapping tbls */
    rfdevice_trx_mapping_tbl_type bsp_mapping_tbl;

	/*! @brief Contains ptrs to BW Mapping tbls */
	rfdevice_trx_mapping_tbl_type bw_mapping_tbl;

	/*! @brief Contains ptrs to LNA Mapping tbls */
    rfdevice_trx_mapping_tbl_type lna_mapping_tbl;
	
	/*! @brief Contains ptrs to PLNA Mapping tbls - Needed for DC Cal */
  	rfdevice_rx_agc_mapping_tbl_type prx_lna_path_ilna_mapping_tbl;
  	rfdevice_rx_agc_mapping_tbl_type prx_lna_path_bbf_mapping_tbl;
	
};

/*----------------------------------------------------------------------------*/
/*!
 
  @brief
  Struct contains the mapping tables which need to be loaded
  as part of FBRx enable 
  */
struct rfdevice_fbrx_ag_tbls_type
{
  /*! @brief Contains ptrs to Cmn Mapping tbls */
  rfdevice_trx_mapping_tbl_type cmn_mapping_tbl;

  /*! @brief Contains ptrs to BSP Mapping tbls */
  rfdevice_trx_mapping_tbl_type bsp_mapping_tbl;

  /*! @brief Contains ptrs to LO DIV Mapping tbls */
  rfdevice_trx_mapping_tbl_type lo_div_mapping_tbl;

  /*! @brief Contains ptrs to Tune Mapping tbls */
  rfdevice_trx_mapping_tbl_type tune_mapping_tbl;

  /*! @brief Contains ptrs to the FBRx AGC Mapping tbl */
  rfdevice_rx_agc_mapping_tbl_type fbrx_agc_mapping_tbl;
  
};

/*----------------------------------------------------------------------------*/
/*! 
 
  @brief
  Struct used to pass around PLL config data
*/
struct rfdevice_trx_pll_config_type
{
  /*! @brief Indicates the LO div ratio in the PLL configuration */
  uint8 lo_div_ratio;

  /*! @brief Indicates the PLL option picked */
  uint8 pll_option;

  /*! @brief  Indicates the row in PLL LUT */
  uint16 pll_lut_index;

  /*! @brief  Indicates fixup rx pll script with 0's if TRUE */
  boolean fixup_with_zeros;

  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/

  /*! @brief Default constructor to initialize elements of 
             struct to invalid values */
  rfdevice_trx_pll_config_type() 
  : lo_div_ratio(0xFF), pll_option(0xFF),
    pll_lut_index(0xFF), fixup_with_zeros(FALSE)
  {
  }

  boolean initialize
  (
    uint8 lo_div_ratio,
    uint8 pll_option,
    uint16 pll_lut_index
  );

  boolean initialize_for_data_fixup
  (
    void
  );
};

typedef struct{
  const uint16* pll_lut_indx;
  const uint16* pll_lut;
}rfdevice_trx_spll_tx_pll_info_type;

#if 0
/*----------------------------------------------------------------------------*/
/*! 
 
  @brief
  Struct containing params to tune FBRx PLL
*/
struct rfdevice_fbrx_config_info_type
{
  /*! @brief Current tech of operation */
  rfdevice_trx_tech_type tech;

  /*! @brief Current band in uint16 as defined in the tech's LUT index table. 
    Will be used to find start/stop index in main LUT */
  uint16 band;

  /*! @brief Frequency the Rx PLL needs to be tuned to in Hertz */
  uint64 nominal_freq_hz;

  /*! @brief Error frequency the Rx PLL needs to be corrected to in Hertz */
  int32 afc_freq_hz;

  /*! @brief Path Assn */
  uint8 path_assn;

  /*! FBRx power mode */
//  rfdevice_trx_fbrx_power_mode_enum_type pwr_mode;

  /*! FBRx mode */
  uint8 fbrx_mode;

  /*! @brief PLL option is used to select between various choices of PLL 
    configuration. PLL option may be selected based on band/chan combos
    on various CA paths */
  uint8 pll_option;

  /*! @brief Contains the lo div ratio and pll lut index that can be used to 
    quickly index in to Rx PLL LUT */
  rfdevice_trx_pll_config_type pll_config;

  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/

  /*! @brief Constructor to initialize elements of struct to default values */
  rfdevice_fbrx_config_info_type()
  : tech(RFDEVICE_TRX_STD_INVALID),
    band(0xFF), nominal_freq_hz(0), afc_freq_hz(0),
    path_assn(0),pll_option(0),
    pwr_mode(RFDEVICE_TRX_FBRX_POWER_MODE_INVALID),
    fbrx_mode(RFDEVICE_TRX_INVALID_8_BIT),pll_config()
  {
  }

  /*===========================================================================*/
  /*                         Member functions                                  */
  /*===========================================================================*/

  /*! @brief Helper function to initialize all elements of struct */
  void initialize
  (
    rfdevice_trx_tech_type tech,
    uint16 band,
    uint8 path_assn,
    uint64 nominal_freq_khz,
    int32 afc_freq_hz,
    rfdevice_trx_fbrx_power_mode_enum_type pwr_mode,
    uint8 fbrx_mode,
    const rfdevice_trx_pll_config_type& pll_config
  );
};
#endif

/*----------------------------------------------------------------------------*/
/*! 
 
  @brief
  Struct containing params to tune Rx PLL
*/
struct rfdevice_rx_config_info_type
{
  /*! @brief Current tech of operation */
  rfdevice_trx_tech_type tech;

  /*! @brief Current band in uint16 as defined in the tech's LUT index table. 
    Will be used to find start/stop index in main LUT */
  uint16 band;

  /*! @brief Current bandwidth in uint8 as defined in the tech's LUT index table. 
    Will be used to find start/stop index in main LUT */
  uint8 bw;

  /*! @brief Frequency the Rx PLL needs to be tuned to in Hertz */
  uint64 nominal_freq_hz;

  /*! @brief Error frequency the Rx PLL needs to be corrected to in Hertz */
  int32 afc_freq_hz;

  /*! @brief Common RX mode indicates which PLL is being used.*/
  uint8 mode;

  /*! @brief Frequency the Tx PLL needs to be tuned to in Hertz during SPLL case */
  uint64 spll_tx_nominal_freq_hz;

  /*! @brief SPLL TX mode indicates which PLL is being used.*/
  uint8 spll_tx_mode;

  /*! @brief LIF offset in KHz */
  int32  lif_offset_khz;

  /*! @brief Indicates if LIF will be enabled for this configuration */
  boolean is_lif_enabled;

  /*! @brief Path Assn */
  uint8 path_assn;

  rfdevice_trx_spll_tx_pll_info_type tx_pll_info;

  /*! @brief Contains the lo div ratio and pll lut index that can be used to 
    quickly index in to Rx PLL LUT */
  rfdevice_trx_pll_config_type pll_config;

  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/

  /*! @brief Constructor to initialize elements of struct to default values */
  rfdevice_rx_config_info_type()
  : tech(RFDEVICE_TRX_STD_INVALID),
    band(0xFF), nominal_freq_hz(0), afc_freq_hz(0),
    mode(0), path_assn(0), pll_config(),
    spll_tx_mode(0), spll_tx_nominal_freq_hz(0),
    is_lif_enabled(FALSE)
  {
  }

  /*===========================================================================*/
  /*                         Member functions                                  */
  /*===========================================================================*/

  /*! @brief Helper function to initialize all elements of struct */
  void initialize
  (
    rfdevice_trx_tech_type tech,
    uint8 mode,
    uint16 band,
    uint8 path_assn,
    uint64 nominal_freq_hz,
    int32 afc_freq_hz,
    const rfdevice_trx_pll_config_type& pll_config
  );

  /*! @brief Helper function to initialize all elements of struct */
  void initialize_spll_tdd_params
  (
    uint8 spll_tx_mode,
    uint32 spll_tx_nominal_freq_khz
  );  


  /*! @brief Helper function to initialize all elements of struct */
  void initialize
  (
    rfdevice_trx_tech_type tech,
    uint8 mode,
    uint16 band,
    uint8 path_assn
  );
};

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Struct containing Rx AGC Gain states
*/
struct rfdevice_rx_gain_state_info_type
{
  /*! @brief Contains WTR iLNA gain state to be used for RX AGC data loading(PRx LNA path) */
  uint8 ilna_gs_prx_lna_path;

  /*! @brief Contains WTR BBF gain state to be used for RX AGC data loading(PRx LNA path) */
  uint8 bbf_gs_prx_lna_path;

  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/

  /*! @brief Constructor to initialize elements of struct to default values */
  rfdevice_rx_gain_state_info_type()
  : ilna_gs_prx_lna_path(0xFF),
    bbf_gs_prx_lna_path(0xFF)
  {
  }
  
  /*===========================================================================*/
  /*                         Member functions                                  */
  /*===========================================================================*/

  /*! @brief Helper function to initialize all elements of struct */
  void initialize
  (
    uint8 ilna_gs_prx_lna_path,
    uint8 bbf_gs_prx_lna_path
  );
};


/*----------------------------------------------------------------------------*/
/*! 
 
  @brief
  Struct containing params to tune Tx PLL
*/
struct rfdevice_tx_config_info_type
{
  /*! @brief Current tech of operation */
  rfdevice_trx_tech_type tech;

  /*! @brief Current band in uint16 as defined in the tech's LUT index table.
    Will be used to find start/stop index in main LUT */
  uint16 band;

  /*! @brief Current bandwidth in uint8
    Will be used to find start/stop index in BBF lut */
  uint8 bw;

  /*! @brief Frequency the Tx PLL needs to be tuned to in Hertz */
  uint64 nominal_freq_hz;

  /*! @brief Error frequency the Tx PLL needs to be corrected to in Hertz */
  int32 afc_freq_hz;

  /*! @brief Lte catagory CATM1/NB1.*/
  rfcom_lte_cat_type lte_cat;

  /*! @brief Common RX mode indicates which PLL is being used.*/
    uint8 mode;

  /*! @brief Path Assn */
  uint8 path_assn;

  /*! @brief Flag to indicate SPLL-TDD scenario for the current Tx chain in TX1
      with a corresponding Rx chain in RX0*/
  boolean is_spll_tdd;

  /*! @brief number of PLL options for this band  */
  uint8 num_pll_opt;

  /*! @brief Contains the lo div ratio and pll lut index that can be used to 
    quickly index in to Rx PLL LUT */
  rfdevice_trx_pll_config_type pll_config[MAX_PLL_OPTION_PER_CHAN];


  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/

  /*! @brief Constructor to initialize elements of struct to default values */
  rfdevice_tx_config_info_type()
  : tech(RFDEVICE_TRX_STD_INVALID),
    band(0xFF), nominal_freq_hz(0), afc_freq_hz(0),
    mode(0), path_assn(0), is_spll_tdd(0), pll_config(), num_pll_opt(0)
  {
  }

  /*===========================================================================*/
  /*                         Member functions                                  */
  /*===========================================================================*/

  /*! @brief Helper function to initialize all elements of struct */
  void initialize
  (
    rfdevice_trx_tech_type tech,
    uint8 mode,
    uint16 band,
    uint8 path_assn,
    uint64 nominal_freq_hz,
    int32 afc_freq_hz,
    uint8 bw,
    rfcom_lte_cat_type lte_cat,
    uint8 num_pll_opt,
    const rfdevice_trx_pll_config_type* pll_config
  );

  /*! @brief Helper function to initialize all elements of struct */
  void initialize
  (
    rfdevice_trx_tech_type tech,
    uint8 mode,
    uint16 band,
    uint8 path_assn,
    uint64 nominal_freq_hz,
    int32 afc_freq_hz,
    uint8 bw,
    rfcom_lte_cat_type lte_cat,
    boolean is_spll_tdd,
    uint8 num_pll_opt,
    const rfdevice_trx_pll_config_type* pll_config
  );  
};

/*----------------------------------------------------------------------------*/

/*! @brief Struct containing list of processes */
struct rfdevice_trx_chip_process_list
{
  /*! @brief Default encoded chip process used when loading AG tbls */
  uint8 default_process;

  /*! @brief Encoded chip process used when loading LO Div AG tbls */
  uint8 lo_div_process;

  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/

  /*! @brief Default constructor sets data members to invalid values */
  rfdevice_trx_chip_process_list()
  : default_process(0),
    lo_div_process(0)
  {
  }

  /*===========================================================================*/
  /*                         Member functions                                  */
  /*===========================================================================*/

  void initialize
  (
    uint8 default_process,
    uint8 lo_div_process
  );
};


/*----------------------------------------------------------------------------*/
/*! @brief Structure containing pair of VCO ids */
struct rfdevice_trx_vco_pair_type 
{
  uint8 vco1_idx;
  uint8 vco2_idx;
};

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Debug Struct containing information pertaining to memory reclaiming.

  @details
  The structure contains the total amount of memory reclaimed in bytes and
  also stores the amount of heap mem used to store the mapping tbls in bytes
*/
struct rfdevice_trx_reclaimed_mem_debug_type
{
  /*! number of bytes of memory reclaimed to heap */
  uint32 reclaimed_mem_to_heap;
  
  /*! num bytes of mem allocated on heap to store mapping tbls */
  uint32 heap_mem_for_map_tbls;

  /*! Constructor - initialize members to 0 */
  rfdevice_trx_reclaimed_mem_debug_type()
  : reclaimed_mem_to_heap(0), heap_mem_for_map_tbls(0)
  {
  }
};

/*! 
  @brief 
  Structure containing address and 32 bit data.
  Used for wxe/dtop reads via qlink*/
typedef struct 
{
  /*! Address */
  uint16 addr;

  /*! Data at the address */
  uint32 data;
} rfdevice_trx_addr_data_info_type;

typedef struct 
{
  /*! pll option number */
  uint8 pll_option;
  
  /*! register bank */
  uint8 reg_bank;
} rfdevice_trx_pll_opt_reg_bank_pair_type;

/*! 
  @brief 
  pll option vs CT reg bank mapping*/
typedef struct 
{
  /*! Number of PLL option for the tuned EARFCN */
  uint8  num_pll_option;

  rfdevice_trx_pll_opt_reg_bank_pair_type pll_opt_reg_bank_pair[MAX_PLL_OPTION_PER_CHAN];
} rfdevice_trx_pll_opt_reg_bank_mapping_type;

  /*! 
  @brief
  Script data holder type to accumulate the contiguous data bytes for extended script writes.
  */
typedef struct
{

  /*! Poniter to the buffer where script has to be written */

  rf_buffer_intf* buf;

  /*! Channel for all current stored bytes */

  uint8 channel;

  /*! Usid for all current stored bytes */

  uint8 usid;

  /*starting address for current multibyte write.*/

  uint16 start_addr;

  /*! Buffer to hold the data for extended writes. */

  uint8 data[RF_HAL_BUS_RFFE_MAX_BYTES];

  /*! Number of data bytes present in script entry holder. */

  uint8  num_script_bytes;

  /*! Transaction type for current entry present in the entry holder */

  rf_buffer_transaction_type transaction_type;

  /*! start delta for this script entry */

  uint16 delay;

  /*! flag indicating the presence of new time reference */

  boolean new_time_ref;

} rfdevice_trx_rffe_script_entry_holder_type;

#endif // RFDEVICE_TRX_TYPEDEFS_H
