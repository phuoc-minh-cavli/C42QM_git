/*!
  @file
  intf_dl_si.h

  @brief
  This file contains lte LL downlink command id's and parameters structures
  for SI message collection.

 
*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //source/qcom/qct/modem/fw/components/fw_lte.mpss/rel/15.2/api/intf_dl_si.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/09   anm     Added SIB related info for LL1 logging
===========================================================================*/
#ifndef INTF_DL_SI_H
#define INTF_DL_SI_H

#include "intf_common.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/

   

/*===========================================================================

      Typedefs

===========================================================================*/
/*!
  @brief
  An enum describing the bit positions in the lte_l1_sib_mask_t

  CHANGE SIB MASKS IN CPHY_MSG INTERFACE FILE IF NEED TO CHANGE HERE
  */
enum
{
  LTE_LL1_SIB_MASK_BIT_MIB=0x1,  ///< MIB bit Should not be set with 
                                 /// SI_START_COLLECT_REQ. Mentioned just to keep
                                 /// upper layer and LL1 masks same to avoid any 
                                 /// conversion at ML1
  LTE_LL1_SIB_MASK_BIT_SIB1=0x2,
  LTE_LL1_SIB_MASK_BIT_SIB2=0x4,
  LTE_LL1_SIB_MASK_BIT_SIB3=0x8,
  LTE_LL1_SIB_MASK_BIT_SIB4=0x10,
  LTE_LL1_SIB_MASK_BIT_SIB5=0x20,
  LTE_LL1_SIB_MASK_BIT_SIB6=0x40,
  LTE_LL1_SIB_MASK_BIT_SIB7=0x80,
  LTE_LL1_SIB_MASK_BIT_SIB8=0x100,
  LTE_LL1_SIB_MASK_BIT_SIB9=0x200,
  LTE_LL1_SIB_MASK_BIT_SIB10=0x400,
  LTE_LL1_SIB_MASK_BIT_SIB11=0x800,
  LTE_LL1_SIB_MASK_VALID_BITS_MASK = 0xFFF
};

/*! @brief Bitmask that defines System Information Block numbers.

  @see lte_l1_sib_mask_t for the bit positions in this mask
 */
typedef uint32 lte_l1_sib_mask_t;

typedef enum
{
    //LTE_LL1_NOT_EMTC_MODE
    LTE_LL1_EMTC_MASKBIT_SI_IDLE=0,
    LTE_LL1_EMTC_MASKBIT_SIB1_BR=1,
    LTE_LL1_EMTC_MASKBIT_SI_X=2

} emtc_sib_collect_enum;

typedef enum
{
    LTE_LL1_EMTC_SI_REP_EVERY_RF=0,
    LTE_LL1_EMTC_MASKBIT_EVERY_2NDRF=1,
    LTE_LL1_EMTC_MASKBIT_EVERY_4THRF=2,
    LTE_LL1_EMTC_MASKBIT_EVERY_8THRF=3

} emtc_si_rep_pattern_enum   ;

// --------------------------------------------------------------------------
//
// Start SI message collection
//
// --------------------------------------------------------------------------

/*! @brief  Start SI message collection request message.
            This request starts SI message collection for SI-RNTI PDSCH
            with specified the HARQ information.  LL1 autonomously increments 
            RV using sequence (sequence = 0,2,3,1) and keeps NDI as false.  
            SI message collection stops when the message is successfully decoded 
            or a stop SI message collection request is received.
*/
typedef struct {
  lte_LL1_req_header_struct   req_hdr;      ///< Common request parameters
  
  uint8                       rv;           ///< Starting RV value (valid value is 
                                            ///< between 0 & 3)
  
  uint8                       ndi;          ///< New data indicator
                                            ///< 1=force no LLR HARQ combining
                                            ///< 0=force LLR HARQ combining
                                            ///< not(0 or 1)=no ndi state change

  boolean                     not_first_window;
                                          
  uint8                       harq_buf_id;  ///< LLR HARQ symbol buffer id 
                                            ///< (valid values are 0 & 1)
  /****For LL1 logging only *******/
  uint8                       si_msg_num;   ///< 0 represents SIB1
                                            /// 1 -10 represents SImsg1 to SImsg10
                                            
  lte_l1_sib_mask_t           sib_mask;     ///< SIBs present in the requested
                                            ///< SI message  

  uint8                      num_pdcch_per_mi;  ///< num pddch attemps per mi

    /**** for CATM eMTC only ******/
    emtc_sib_collect_enum       emtc_sib_collect_en; /// A bit map Either 0:emtc-sib-disabled, 1:sib1-br, 2:si_x   4-128: reserved
    uint16                      start_sfn;
    /* the index of the narrowband used to broadcast the SI message   towards low complexity UEs and UEs supporting CE. */
    uint8                       catm_si_nb_index;   ///< narrowband index as defined in 36.211 6.2.7
    uint8                       catm_si_start_symbol; ///< start symbol for SIB data
    uint16                     catm_si_tbs;          ///< transport block size


    /* Indicates the radio frames within the SI window used for SI message transmission. Value everyRF corresponds to every radio frame, 
    Value every2ndRF corresponds to every second radio frame, and so on. */
    emtc_si_rep_pattern_enum   si_repetition_pattern;
    uint64                                  si_subfn_bitmap;        ///< 40 bit bitmap (or is 10 sufficient?) note: lsb first in time.
    
    /* SIB hopping parameters */
    boolean                     freq_hop_enabled;      ///< freq hop enabled 
    uint32                      n_dl_ch;  /// Number of consecutive absolute subframes over which PDSCH stays at the same narrowband before hopping to another narrowband
    uint32                      n_dl_ch_hop;   ///Number of narrowbands over which MPDCCH or PDSCH frequency hops 2 or 4 narrow bands                 
    uint32                      f_dl_hop; ///Narrowband offset between one narrowband and the next narrowband an MPDCCH or PDSCH hops to,expressed as a number of downlink narrowbands

  
} lte_LL1_dl_si_start_collect_req_struct;

LTE_LL1_INTF_MSG( dl_si_start_collect_req );


/*! @brief  Start SI message collection confirmation message.
            This message is sent to confirm reception of lte_LL1_dl_start_si
            request message.  Contents of SI payload is transferred out of
            decoder output buffer by A2.  
*/
typedef struct { 
  lte_LL1_cnf_header_struct   cnf_hdr;      ///< Common confirmation parameters
  
} lte_LL1_dl_si_start_collect_cnf_struct;

LTE_LL1_INTF_MSG( dl_si_start_collect_cnf );
                                 

// --------------------------------------------------------------------------
//
// Stop SI message collection
//
// --------------------------------------------------------------------------

/*! @brief  Stop SI message collection request message.
            This request stops SI message collection 
*/
typedef struct {
  lte_LL1_req_header_struct   req_hdr;      ///< Common request parameters
    
} lte_LL1_dl_si_stop_collect_req_struct;

LTE_LL1_INTF_MSG( dl_si_stop_collect_req );


/*! @brief  Stop SI message collection request message.
            This message is sent to confirm lte_LL1_dl_stop_si_collect_req
            was received.
*/
typedef struct { 
  lte_LL1_cnf_header_struct   cnf_hdr;      ///< Common confirmation parameters
  
} lte_LL1_dl_si_stop_collect_cnf_struct;

LTE_LL1_INTF_MSG( dl_si_stop_collect_cnf );


#endif /* INTF_DL_SI_H */
