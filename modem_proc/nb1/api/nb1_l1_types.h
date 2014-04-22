/*!
  @file
  nb1_l1_types.h

  @brief
  This header file defines the basic NB1 L1 types. It is shared by entire
  Access Stratum and LL. It should be included in the nb1_ml_fw.h(TBD) and
  nb1_cphy_sap.h. It should be also included by NB1 L2 and L3 software
  when the CPHY interfaces is in use.

  @detail
    This interface is generated from
    $Header: //components/rel/nb1.mpss/2.2/api/nb1_l1_types.h#1 $
*/

/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/nb1.mpss/2.2/api/nb1_l1_types.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
                    Change value of NB1_L1_MAX_MTCH_PER_PMCH to 29 from 32
                     nb1_l1_tdd_ul_dl_cfg_index_e
                   supposed to replace nb1_l1_tdd_subframe_assign_e later.
07-05-2008 jyw     Created the file.
===========================================================================*/

#ifndef NB1_L1_TYPES_H
#define NB1_L1_TYPES_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

//#include "comdef.h"
#include <nb1_as.h>

/*===========================================================================

                          GENERAL DEFINITIONS

===========================================================================*/
/*! @brief
  Used as a placeholder when the type is not known
*/
typedef uint32 nb1_l1_TBD_t;

/*! @brief Time type for absolute system time since power up */
typedef uint64 nb1_ml1_time_t;

/*! @brief Invalid value of time */
#define NB1_ML1_COMMON_INVALID_VALUE_TIME    ((uint64)(-1))

#define NB1_CPHY_NULL_SFN                    0xFFFF

#define NB1_CHY_NULL_SUBFN                   0xFF

#define NB1_CPHY_NULL_HYPER_SFN              0xFFFF

/*! Maximum UE Tx power */
#define NB1_ML1_UE_MAX_OUTPUT_POWER          23

/*! Maximum UE Tx power calss 2*/
#define NB1_ML1_UE_MAX_OUTPUT_POWER_CLASS2_UE 26

/*! Maximum UE Tx power calss 5*/
#define NB1_ML1_UE_MAX_OUTPUT_POWER_CLASS5_UE 20

/*! Maximum UE Tx power calss 6*/
#define NB1_ML1_UE_MAX_OUTPUT_POWER_CLASS6_UE 14

/*! Invalid value of RSRP */
#define NB1_ML1_INVALID_VALUE_RSRP           -255

/*! Invalid value of RSRP */
#define NB1_ML1_INVALID_VALUE_DL_PL_RSRP     0xFFFFFFFF

/*! Invalid value of RSRQ */
#define NB1_ML1_INVALID_VALUE_RSRQ           -255

/*! Minimum value of RSRP */
#define NB1_ML1_MIN_VALUE_RSRP               -127

/*! @brief Value of report_amt to be used if number of reports to send is infinity */
#define NB1_CPHY_MEAS_REPORT_AMT_INFINITY    0xFF

/*! Maximum number of SCells sent in RRC config req message */
#define NB1_CPHY_MAX_S_CELL   4

/*! Maximum number of Serving cells one Pcell + upto 4Scells*/
#define NB1_CPHY_MAX_SERVING_CELL   5

/*! Maxumum value for n1_pucch_an_list_size */
#define MAX_N1_PUCCH_AN_LIST_SIZE 4

/*! @brief Maximum PMCH per MBSFN area */
#define NB1_L1_MAX_PMCH_PER_MBSFN_AREA          15

/*! @brief Maximum MTCH Logical channels per PMCH */
#define NB1_L1_MAX_MTCH_PER_PMCH                29

/*! @brief Maximum possible subframes with PMCH in a frame */
#define NB1_L1_MAX_PMCH_SUBF_PER_FRAME          6 //TDD = 5, FDD = 6

/*! @brief MTCH not scheduled field value in MSI */
#define NB1_L1_MSI_MTCH_NOT_SCHEDULE_VAL        2047

/*! @breif Invalid value for frame offset */
#define NB1_ML1_FRAME_OFFSET_INVALID            0xFFFFFFFF

/*<! @brief  Maximum number of bands to update
            got from EFS ("update_band_range")*/
#define NB1_L1_MAX_BANDS_TO_UPDATE    3
/*! @brief MeasSubframe Pattern max size */
#define NB1_L1_MAX_MEAS_SUBFM_PATTERN_SIZE      3

/*! NB1 RTC ticks per millisecond */
#define NB1_L1_RTC_TICKS_PER_MS                 30720

/* Max num bands in bs_bw_supported */
#define NB1_L1_MAX_BS_BW_SUPPORTED 10

/*! ID for PCC */
#define NB1_CPHY_PCC_ID            0

/*! STAG-ID for PCC */
#define NB1_CPHY_PCC_STAG_ID       0

/*! Maximum STAG ID */
#define NB1_CPHY_MAX_STAG_ID       3

/*! Maximum number of CCs supported with configured uplink */
#define NB1_CPHY_MAX_UL_CC         2

/*! Maximum number of possible non anchor carriers*/
#define NB1_CPHY_MAX_NON_ANCHOR_CARRIER    15

/*! @breif Unkown raster offset */
#define NB1_ML1_RASTER_OFFSET_UNKNOWN      0xFFFFFFFF

/*! @brief
    Enumeration defines action for MBMSFN areas physical channel
*/
typedef enum {
  /*! No actions */
  NB1_CPHY_MBSFN_AREA_NONE = 0,
  /*! Monitor on all potential occasions unless specific scheduling info available */
  NB1_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS = 1,
  /*! Monitor only at modifucation boundaries occasion */
  NB1_CPHY_MBSFN_AREA_MONITOR_ONLY_MODIFICATION_BOUNDARY = 2,
  /*! Don't monitor the PMCH */
  NB1_CPHY_MBSFN_AREA_NO_MONITOR = 3
} nb1_cphy_mbsfn_area_monitor_pattern_e;

/*! @brief
    Enumeration defines action of configuration
    NB1_L1_CONFIG_NO_ACTION = No action = No params passed
    NB1_L1_DECONFIG = Erase/Remove/deconfig from L1
    NB1_L1_CONFIG = Update/Reconfig/NewConfig
*/
typedef enum {
  NB1_L1_CONFIG_NO_ACTION = 0,
  NB1_L1_DECONFIG = 1,
  NB1_L1_CONFIG = 2
} nb1_l1_config_action_e;

/*===========================================================================

          RF CHANNEL, BANDWIDTH, ANTENNA, BAND SCAN RELATED TYPES

===========================================================================*/
/*! @brief
  Enumeraion of the down link transmission mode
  Defined in lte_LL1_dl_pdsch_params_struct
  from Espresso UE LL-ML API Specification
  Defined in 36.213 Table 7.1-1
*/
typedef enum
{
  /*----------------------------------------------------------------------------
                            Values signalled over the air
   ---------------------------------------------------------------------------*/
  NB1_L1_DL_TM1 = 1,
  NB1_L1_DL_TM2 = 2,
  NB1_L1_DL_TM3 = 3,
  NB1_L1_DL_TM4 = 4,
  NB1_L1_DL_TM5 = 5,
  NB1_L1_DL_TM6 = 6,
  NB1_L1_DL_TM7 = 7,
  NB1_L1_DL_TM8_REL9 = 8,
  NB1_L1_DL_TM9_REL10 = 9,
  NB1_L1_DL_TM10_REL11 = 10,
  NB1_L1_DL_TRANSMISSIONMODE_TOTAL,

  /*----------------------------------------------------------------------------
                            Translated values for L1
   ---------------------------------------------------------------------------*/
  /*! Invalid mode */
  NB1_L1_DL_TX_MODE_INVALID = 0,

  /*! Single antenna port */
  NB1_L1_DL_TX_MODE_SINGLE_ANT_PORT0 = NB1_L1_DL_TM1,

  /*! Transmit diversity */
  NB1_L1_DL_TX_MODE_TD_RANK1 = NB1_L1_DL_TM2,

  /*! Open-loop spatial multiplexing */
  NB1_L1_DL_TX_MODE_OL_SM = NB1_L1_DL_TM3,

  /*! Close-loop spatial multiplexing */
  NB1_L1_DL_TX_MODE_CL_SM = NB1_L1_DL_TM4,

  /*! Mulit-user MIMO */
  NB1_L1_DL_TX_MODE_MU_MIMO = NB1_L1_DL_TM5,

  /*! Close-loop rank1 precoding */
  NB1_L1_DL_TX_MODE_CL_RANK1_PC = NB1_L1_DL_TM6,

  /*! Single-antenna port(5), single layer beam forming */
  NB1_L1_DL_TX_MODE_SINGLE_ANT_PORT5 = NB1_L1_DL_TM7,

  /*! Two-antenna port(7) (8), beam forming */
  NB1_L1_DL_TX_MODE_TWO_ANT_PORT7_PORT8 = NB1_L1_DL_TM8_REL9,

  /*! Antenna ports 7 to 14, single user beam forming with up to 8x8 MIMO */
  NB1_L1_DL_TX_MODE_UP_TO_EIGHT_LAYER_BEAMFORMING = NB1_L1_DL_TM9_REL10,

  /*! COMP */
  NB1_L1_DL_TX_MODE_COMP = NB1_L1_DL_TM10_REL11,

  NB1_L1_DL_TX_MAX_NUM_MODES = NB1_L1_DL_TRANSMISSIONMODE_TOTAL
} nb1_l1_dl_tx_mode_e;

/*! @brief
    Enumeration indicates whether UE transmit antenna selection control is
    closed-loop or open-loop. (ref: 36.331, 6.3.2)

*/
typedef enum {
  NB1_L1_TX_ANTENNA_SELECTION_CLOSED_LOOP,
  NB1_L1_TX_ANTENNA_SELECTION_OPEN_LOOP
} nb1_l1_tx_antenna_selection_e;


/*! @brief
    Represents the number of cell specific antenna ports where an1 corresponds
    to 1, an2 to 2 antenna ports etc. (ref: 36.331, 6.3.2)
    see 36.211, 6.2.1. A UE in IDLE mode acquires the information about the
    number of transmit antenna ports according to TS 36.212, 5.3.1.1.
*/
typedef enum {
  NB1_L1_ANTENNA_INFO_PORTS_COUNT_1 = 1,
  NB1_L1_ANTENNA_INFO_PORTS_COUNT_2 = 2,
  NB1_L1_ANTENNA_INFO_PORTS_COUNT_4 = 4,
  NB1_L1_ANTENNA_INFO_PORTS_COUNT_8 = 8
} nb1_l1_antenna_info_ports_count_e;


/*===========================================================================

                    SYSTEM INFORMATION TYPES

===========================================================================*/
/*! @brief
  SFN type used in the cell timming; only the lower 10 bits are valid
  from 0 ... 1023.
  Defined in 36.331 MasterInformationBlock
*/
typedef uint16 nb1_l1_sfn_t;

/*! @brief
  Subframe number type used in the cell timming from 0 ... 9.
  Defined in 36.211 Section 4.1 and 4.2
*/
typedef uint8  nb1_l1_subfn_t;

/*! @brief
 Hyper SFN type used in the cell timming for eMTC cases; only the lower 10 bits are valid
from 0 ?... 1023.
Defined in 36.331 SIB1 contents
*/
typedef uint16 nb1_l1_hyper_sfn_t;

/*! @brief
  Frame reference time type derived from RTC cnt used in the searched
  cell timming
*/
typedef uint64 nb1_l1_frame_ref_time_t;


/*! @brief
  Cell system time
  Used in NB1_LL_SYS_SF_IND defined in Espresso UE ML-LL interface
*/
typedef struct
{
  /*! System frame number */
  nb1_l1_sfn_t    sfn;

  /*! Subframe number from 0 .. 9. */
  nb1_l1_subfn_t  subfn;
} nb1_l1_cell_systime_s;

/*! @brief
  Band Frequency range Structure got from
  EFS ("update_band_range")*/
typedef struct
{
  /* Band index */
  uint16       band_id;
  /* Min earfcn supported */
  uint16       start_freq;
  /* Max earfcn supported */
  uint16       end_freq;
} nb1_l1_band_freq_range_s;

/*! @brief
  Band Frequency range Structure got from 
  EFS ("update_band_range")*/
typedef struct 
{
  /* Band index */
  nb1_efs_extnd_band_enum_t band_id;
  /* Min earfcn supported */
  nb1_earfcn_t start_freq;
  /* Max earfcn supported */
  nb1_earfcn_t end_freq;
} nb1_l1_band_extnd_freq_range_s;

/*! @brief
  Band Updated info structure got from
  EFS ("update_band_range")*/
typedef struct
{
  /* Number of bands */
  uint16                          num_bands;
  /* Band freq info structure */
  nb1_l1_band_freq_range_s  nb1_band_range[NB1_L1_MAX_BANDS_TO_UPDATE];
} nb1_l1_bands_updated_info_s;

/*! @brief
  Band Updated info structure got from  
  EFS ("update_band_range")*/
typedef struct 
{
  /* Number of bands */
  uint16                          num_bands;
  /* Band freq info structure */
  nb1_l1_band_extnd_freq_range_s  nb1_band_range[NB1_L1_MAX_BANDS_TO_UPDATE];
} nb1_l1_extnd_bands_updated_info_s;
/*! @brief
   Struct for storing bandwidths supported for band scan */
typedef struct
{
  uint8   num_bands;

  /* Band */
  uint8   band[NB1_L1_MAX_BS_BW_SUPPORTED];

  /* earfcn within the band user wants UE to camp on */
  uint8   bw_supported[NB1_L1_MAX_BS_BW_SUPPORTED];
} nb1_l1_bs_bw_supported_s;

/*! @brief
  Minimum system frame number.
*/
#define NB1_L1_MIN_FRAME_NUM 0
/*! @brief
  Maximum system frame number.
*/
#define NB1_L1_MAX_FRAME_NUM 1023

/*! @brief Invalid value of sub-FN */
#define NB1_ML1_COMMON_INVALID_VALUE_SUB_FN  0xFF

/*===========================================================================

                    CELL RELATED TYPES

===========================================================================*/
/*! @brief
  Miximum number of the NB1 physical cell id .
  Defined in 36.331 Section 6.3.3
*/
#define NB1_L1_MAX_PHY_CELL_ID 503

/*! @brief
  Cell Id value to be used when that the cell id is not present
  */
#define NB1_L1_NO_CELL_ID      0xFFFF


/*! @brief
  Physical cell ID and range
*/
typedef struct
{
  /*! Lowest PCI in the range */
  nb1_phy_cell_id_t start_pci;

  /*! Highest PCI in the range (for single PCI, equals start_pci) */
  nb1_phy_cell_id_t end_pci;

} nb1_phy_cell_id_and_range_s;


/*! @brief
  NB1 cell duplexing modes
  Defined in 36.201 Section 4.2.1
*/
typedef enum
{
  /*! Frequency division Duplex */
  NB1_L1_CELL_DUPLEX_MODE_FDD = 0,

  /*! Time Division Duplex */
  NB1_L1_CELL_DUPLEX_MODE_TDD,

  /*! Half Duplex */
  NB1_L1_CELL_DUPLEX_MODE_HALF,

  /*! Maximum number of duplex modes */
  NB1_L1_MAX_NUM_CELL_DUPLEX_MODES
} nb1_l1_cell_duplex_e;


/*! @brief
  Supported frame structures enum
  Defined in 36.201 Section 4.2.1
*/
typedef enum
{
  /*! Frame structure 1 (generic frame structure) */
  NB1_L1_FRAME_STRUCTURE_FS1 = 0,

  /*! Frame structure 2 (alternative frame structure) */
  NB1_L1_FRAME_STRUCTURE_FS2,

  /*! Maximum number of frame structure for NB1 cells */
  NB1_L1_FRAME_MAX_NUM_STRUCTURES
} nb1_l1_frame_struct_e;


/*! @brief
  Supported cyclix prefix modes
  Defined in 36.211 Table 6.12-1 for normal and extended mode.
  TBD: fill in the spec reference for the extended_msbfn mode.
*/
typedef enum
{
  /*! Normal CP */
  NB1_L1_CP_MODE_NORMAL = 0,

  /*! Extended CP */
  NB1_L1_CP_MODE_EXTENDED,

  /*! Long CP for MBSFN
     @todo Do we need list the CP_MODE_EXTENDED MBSFN in this enum? */
  NB1_L1_CP_MODE_EXTENDED_MBSFN,

  NB1_L1_MAX_NUM_CP_MODES
} nb1_l1_cp_e;


/*! @brief
  Cell DL information used in the CPHY interface
  Defined in TBD
*/
typedef struct
{
  /*! Downlink center frequency */
  nb1_earfcn_t     earfcn;

  /*! Downlink bandwidth  */
  nb1_bandwidth_e  dl_bandwidth;
} nb1_l1_dl_cell_info_s;


/*! @brief
  Cell ul information used in the CPHY interface
  Defined in TBD
*/
typedef struct
{
  /*! Uplink center frequency */
  nb1_earfcn_t        earfcn;

  /*! Uplink bandwidth */
  nb1_bandwidth_e     ul_bandwidth;

  /*! TBD */
  nb1_l1_TBD_t        additional_spectrum_emission;
} nb1_l1_ul_cell_info_s;

/*! @brief
  Cell information used in the CPHY interface
  Defined in TBD
*/
typedef struct
{
  /*! Cell physical identity */
  nb1_phy_cell_id_t       phy_cell_id;

  /*! DL cell info */
  nb1_l1_dl_cell_info_s   dl_cell_info;

  /*! UL cell info */
  nb1_l1_ul_cell_info_s   ul_cell_info;
} nb1_l1_cell_info_param_s;


/*! @brief
  PHICH duration in terms of OFDM symbols.
  Defined in PHICH
*/
typedef enum
{
  /*! Normal PHICH duration */
  NB1_L1_PHICH_DURATION_NORMAL = 0,
  /*! Extended PHICH duration */
  NB1_L1_PHICH_DURATION_EXTENDED,
  /*! Max number of PHICH duration modes */
  NB1_L1_PHICH_MAX_NUM_DURATION_MODES,
  /*! Minimum supported PHICH duration */
  NB1_L1_PHICH_DURATION_MIN = NB1_L1_PHICH_DURATION_NORMAL,
  /*! Maximum supported PHICH duration */
  NB1_L1_PHICH_DURATION_MAX = NB1_L1_PHICH_DURATION_EXTENDED
} nb1_l1_phich_duration_e;

/*! @brief
    Indicates DL/UL subframe configuration where sa0 point to Configuration 0,
    sa1 to Configuration 1 etc. as specified in the 36.211, table 4.2.2.
    (ref: 36.331, 6.3.2)
*/
typedef enum {
  NB1_L1_TDD_SUBFRAME_ASSIGN_0 = 0,
  NB1_L1_TDD_SUBFRAME_ASSIGN_1,
  NB1_L1_TDD_SUBFRAME_ASSIGN_2,
  NB1_L1_TDD_SUBFRAME_ASSIGN_3,
  NB1_L1_TDD_SUBFRAME_ASSIGN_4,
  NB1_L1_TDD_SUBFRAME_ASSIGN_5,
  NB1_L1_TDD_SUBFRAME_ASSIGN_6,
  NB1_L1_TDD_SUBFRAME_ASSIGN_MAX  /* Invalid value */
} nb1_l1_tdd_subframe_assign_e;


/*! @brief
    Indicates TDD DL/UL configurations where NB1_L1_TDD_UL_DL_CFG_0 points to Configuration 0,
    NB1_L1_TDD_UL_DL_CFG_1 points to Configuration 1,  etc. as specified in the 36.211, table 4.2.2.
    (ref: 36.331, 6.3.2)

*/
typedef enum
{
  NB1_L1_TDD_UL_DL_CFG_0 = 0,
  NB1_L1_TDD_UL_DL_CFG_1 = 1,
  NB1_L1_TDD_UL_DL_CFG_2 = 2,
  NB1_L1_TDD_UL_DL_CFG_3 = 3,
  NB1_L1_TDD_UL_DL_CFG_4 = 4,
  NB1_L1_TDD_UL_DL_CFG_5 = 5,
  NB1_L1_TDD_UL_DL_CFG_6 = 6,
  NB1_L1_TDD_UL_DL_CFG_UNKNOWN_WITH_UNKNOWN_10MS_FRAME = 7,
  NB1_L1_TDD_UL_DL_CFG_UNKONWN_WITH_KNOWN_10MS_FRAME   = 8 ,
  NB1_L1_TDD_UL_DL_CFG_NUM
} nb1_l1_tdd_ul_dl_cfg_index_e;

typedef enum
{
	NB1_L1_FDD_DL_REFERENCE_CFG = 0,
  NB1_L1_TDD_DL_REFERENCE_CFG_0,
  NB1_L1_TDD_DL_REFERENCE_CFG_1,
  NB1_L1_TDD_DL_REFERENCE_CFG_2,
  NB1_L1_TDD_DL_REFERENCE_CFG_3,
  NB1_L1_TDD_DL_REFERENCE_CFG_4,
  NB1_L1_TDD_DL_REFERENCE_CFG_5,
  NB1_L1_TDD_DL_REFERENCE_CFG_6,
  NB1_L1_MAX_DL_REFERENCE_CFG
} nb1_l1_dl_reference_cfg_index_e;

typedef enum
{
	NB1_L1_FDD_UL_REFERENCE_CFG = 0,
  NB1_L1_TDD_UL_REFERENCE_CFG_0,
  NB1_L1_TDD_UL_REFERENCE_CFG_1,
  NB1_L1_TDD_UL_REFERENCE_CFG_2,
  NB1_L1_TDD_UL_REFERENCE_CFG_3,
  NB1_L1_TDD_UL_REFERENCE_CFG_4,
  NB1_L1_TDD_UL_REFERENCE_CFG_5,
  NB1_L1_TDD_UL_REFERENCE_CFG_6,
  NB1_L1_MAX_UL_REFERENCE_CFG
} nb1_l1_ul_reference_cfg_index_e;

/*! @brief
    Indicates DL/UL subframe configuration where ssp0 point to Configuration 0,
    ssp1 to Configuration 1 etc. as specified in the 36.211, table 4.2.1.
    (ref: 36.331, 6.3.2)
*/
typedef enum {
  NB1_L1_TDD_SPECIAL_SUBFRAME_PATTERN_0 = 0,
  NB1_L1_TDD_SPECIAL_SUBFRAME_PATTERN_1,
  NB1_L1_TDD_SPECIAL_SUBFRAME_PATTERN_2,
  NB1_L1_TDD_SPECIAL_SUBFRAME_PATTERN_3,
  NB1_L1_TDD_SPECIAL_SUBFRAME_PATTERN_4,
  NB1_L1_TDD_SPECIAL_SUBFRAME_PATTERN_5,
  NB1_L1_TDD_SPECIAL_SUBFRAME_PATTERN_6,
  NB1_L1_TDD_SPECIAL_SUBFRAME_PATTERN_7,
  NB1_L1_TDD_SPECIAL_SUBFRAME_PATTERN_8,
  NB1_L1_TDD_SPECIAL_SUBFRAME_PATTERN_9, /* Rel-11 SSF9 */
  NB1_L1_TDD_SPECIAL_SUBFRAME_PATTERN_MAX /* Invalid value */
} nb1_l1_tdd_special_subframe_pattern_e;

/*! @brief
  PHICH resource settings
  Defined in TBD
*/
typedef enum
{
  NB1_L1_PHICH_RESOURCE_ONE_SIXTH = 0,
  NB1_L1_PHICH_RESOURCE_ONE_HALF,
  NB1_L1_PHICH_RESOURCE_ONE,
  NB1_L1_PHICH_RESOURCE_TWO,
  NB1_L1_PHICH_MAX_NUM_RESOURCE_MODES,
  /*! Minimum supported PHICH resource */
  NB1_L1_PHICH_RESOURCE_MIN = NB1_L1_PHICH_RESOURCE_ONE_SIXTH,
  /*! Maximum supported PHICH resource */
  NB1_L1_PHICH_RESOURCE_MAX = NB1_L1_PHICH_RESOURCE_TWO
} nb1_l1_phich_resource_e;

/* NB1 operation mode types */
typedef enum
{
  NB1_OP_MODE_SAME_PCI = 0,
  NB1_OP_MODE_INBAND_DIFFERENT_PCI = 1,
  NB1_OP_MODE_GUARDBAND = 2,
  NB1_OP_MODE_STANDALONE = 3,
  NB1_OP_MODE_UNKNOWN = 4
} nb1_l1_operation_mode_e;

/* Raster offset values : one of {-7.5kHz,-2.5kHz,+2.5kHz,+7.5kHz} */
typedef enum
{
  NB1_RO_MINUS_7_DOT_5_KHZ = 0,
  NB1_RO_MINUS_2_DOT_5_KHZ = 1,
  NB1_RO_PLUS_2_DOT_5_KHZ  = 2,
  NB1_RO_PLUS_7_DOT_5_KHZ  = 3
} nb1_l1_raster_offset_e;

/* CRS ports for Inband-different PCI mode */
typedef enum
{
  NB1_RO_SAME_AS_NRS_PORTS = 0,
  NB1_RO_FOUR_PORTS = 1
} nb1_l1_eutra_nums_crs_ports_e;


/* @brief
  Parameter struct for L1M Acquisition Request
  Defined in 36.331 MIB format
*/
typedef struct
{
  /*! Number of Tx antennas detected */
  uint8                      num_tx_antennas;

  /*! Downlink bandwidth */
  nb1_bandwidth_e            dl_bandwidth;

} nb1_l1_mib_info_param_s;

/*! Inband same PCI mode paramaters */
typedef struct
{
  uint8   eutra_crs_sequence_info;
} nb1_l1_inband_same_pci_info_s;

/*! Inband different PCI mode paramaters */
typedef struct
{
  nb1_l1_eutra_nums_crs_ports_e  crs_ports;
  nb1_l1_raster_offset_e         raster_offset;
} nb1_l1_inband_difft_pci_info_s;

/*! Standalone mode paramaters */
typedef struct
{
  /* No parameters as of now */
  uint8 dummy;
} nb1_l1_standalone_info_s;

/*! Guardband mode paramaters */
typedef struct
{
  nb1_l1_raster_offset_e raster_offset;
} nb1_l1_guardband_info_s;

/*! @brief Union of cat NB1 operation mode types
*/
typedef union
{
  nb1_l1_inband_same_pci_info_s   inband_same_pci;
  nb1_l1_inband_difft_pci_info_s  inband_difft_pci;
  nb1_l1_standalone_info_s        standalone;
  nb1_l1_guardband_info_s         guardband;
} nb1_l1_opmode_info_struct_u;
#ifdef TEST_FRAMEWORK
#error code not present
#endif


/*! @brief Decoded MIB information for detected NB1 cells (if known)
    @note num_antennas = 0 indicates an invalid (unknown) MIB
*/
typedef struct
{
  /*! 4 bits MSB of system frame number */
  uint8                           sfn_msb;
  /*! 2 bits LSB of hyper system frame number */
  uint8                           hypersfn_lsb;
  /*! SIB1 scheduling index/ repetition. Range 0,1,..11*/
  uint8                           schedulingInfoSIB1;
  /*! SIB information tag */
  uint8                           systemInfoValueTag;
  /*! Flag to indicate if access barring is enabled */
  boolean                         access_barring_enabled;
  /* NB1 operation/deployment mode */
  nb1_l1_operation_mode_e         opmode;
  /* Information specific to the current opmode*/
  nb1_l1_opmode_info_struct_u     opmode_info;
  /*No of Tx antennas*/
  uint8                      num_tx_antennas;  
#ifdef TEST_FRAMEWORK
  #error code not present
#endif
} nb1_l1_nb_mib_info_param_s;

/*! @brief PRB and raster pair
*/
typedef struct
{
  int16 prb_offset;
  int8  raster_q4;
} nb1_l1_raster_prb_pair_s;

/*! @brief Invalid value of frequency is of type uint32*/
#define NB1_ML1_COMMON_INVALID_VALUE_FREQ    0xFFFFFFFF

/*! @brief Invalid value of cell id */
#define NB1_ML1_COMMON_INVALID_VALUE_PCI     0xFFFF

/*===========================================================================

             DL/UL PHYSICAL CHANNEL CONFIGURATION TYPES

===========================================================================*/
/*
  DL channel configuration mask used in the LL channel config interface
  Used in Espresso UE ML-LL interface lte_LL1_dl_ch_config_req_struct
*/
/*! @brief DL PCFICH config mask */
#define NB1_L1_DL_CH_PCFICH_CONFIG_MASK 0

/*! @brief DL PHICH config mask */
#define NB1_L1_DL_CH_PHICH_CONFIG_MASK  2

/*! @brief DL PDCCH config mask */
#define NB1_L1_DL_CH_PDCCH_CONFIG_MASK  4

/*! @brief DL PDSCH config mask */
#define NB1_L1_DL_CH_PDSCH_CONFIG_MASK  8


/*
  UL channel configuration mask used in the LL channel config interface
  Used in Espresso UE ML-LL interface lte_LL1_ul_ch_config_req_struct
*/
/*! @brief UL PRACH config mask */
#define NB1_L1_UL_CH_PRACH_CONFIG_MASK  0

/*! @brief UL PUSCH config mask */
#define NB1_L1_DL_CH_PUSCH_CONFIG_MASK  2

/*! @brief UL PUCCH config mask */
#define NB1_L1_DL_CH_PUCCH_CONFIG_MASK  4


/*! @brief
  Physical channel enumeration.
  Defined in 36.201 Section 4.2.2
*/
typedef enum
{
  NB1_L1_DL_CH_PCFICH = 0,
  NB1_L1_DL_CH_PHICH,
  NB1_L1_DL_CH_PDCCH,
  NB1_L1_DL_CH_PDSCH,
  NB1_L1_UL_CH_PRACH,
  NB1_L1_UL_CH_PUSCH,
  NB1_L1_UL_CH_PUCCH,
  NB1_L1_UL_CH_PBCH,
  NB1_L1_MAX_NUM_CHS,
  NB1_L1_UL_CH_INVALID = NB1_L1_MAX_NUM_CHS
} nb1_l1_phy_chan_e;


/*! @brief
  Cyclic Delay Diversity mode
  Defined in 36.211 Section 6.3.4.2.1
*/
typedef enum
{
  NB1_L1_CDD_NO_DELAY = 0,
  NB1_L1_CDD_SMALL_DELAY,
  NB1_L1_CDD_LARGE_DELAY
} nb1_l1_cdd_delay_e;

/*! @brief
  Traffic Power control step
  Defined in TBD
*/
typedef enum
{
  /*! TBD  */
  NB1_L1_TPC_SETUP_ENUM_TBD_ENTRY = 0,
} nb1_l1_tpc_step_e;



/*! @brief
    DCI format 3 or 3A used for TPC Index.
*/
typedef enum
{
  /* Index of N when DCI format 3 is used.*/
  NB1_L1_TPC_DCI_FORMAT3 = 0,

  /* Index of M when DCI format 3A is used. */
  NB1_L1_TPC_DCI_FORMAT3A = 1
} nb1_l1_tpc_dci_format_e;

/*! The index of the UE within the TPC-PDCCH for uplink power control */
typedef struct
{
  /* DCI format */
  nb1_l1_tpc_dci_format_e dci_format;

  /* TPC index - Range is [1-15] for DCI format 3 and [1-31] for format 3A*/
  uint8 index_value;
}nb1_l1_tpc_index_t;

/******************************************************************************
                            UL Cyclic Prefix Length
*******************************************************************************/
/*! @brief
    UL Cyclic Prefix Length.  len1 corresponds to normal cyclic prefix and
    len2 corresponds to extended cyclic prefix.
*/
typedef enum
{
  NB1_CPHY_UL_CYCLIC_PREFIX_LENGTH_1,
  NB1_CPHY_UL_CYCLIC_PREFIX_LENGTH_2,
  NB1_CPHY_UL_CYCLIC_PREFIX_LENGTH_MAX /* Invalid Value */
}nb1_cphy_ul_cyclic_prefix_length_e;
/*! @brief
    Alpha enum (ref: 36.331, 6.3.2)
    where al0 corresponds to 0, al04 corresponds to value 0.4, al05 to 0.5, al06
    to 0.6, al07 to 0.7, al08 to 0.8, al09 to 0.9 and al1 corresponds to 1
*/
typedef enum {
  NB1_L1_UL_POWER_CTRL_ALPHA_0= 0,
  NB1_L1_UL_POWER_CTRL_ALPHA_04,
  NB1_L1_UL_POWER_CTRL_ALPHA_05,
  NB1_L1_UL_POWER_CTRL_ALPHA_06,
  NB1_L1_UL_POWER_CTRL_ALPHA_07,
  NB1_L1_UL_POWER_CTRL_ALPHA_08,
  NB1_L1_UL_POWER_CTRL_ALPHA_09,
  NB1_L1_UL_POWER_CTRL_ALPHA_1
} nb1_l1_ul_power_ctrl_alpha_e;

/*! @brief
    DCI Format enum
*/
typedef enum
{
  NB1_L1_DCI_N0 = 0,
  NB1_L1_DCI_N1,
  NB1_L1_DCI_N2,
  NB1_L1_DCI_NUM_FORMATS,
  NB1_L1_DCI_INVALID = NB1_L1_DCI_NUM_FORMATS
} nb1_l1_dci_format_e;

/*! @brief
    Group PUSCH assignment (ref: 36.331, 6.3.2)
*/
typedef uint8  nb1_cphy_pusch_grp_pusch_assign_t;

/*! @brief
    en0 corresponds to value 0 corresponding to state "disabled". en1
    corresponds to value 1.25 corresponding to "enabled"
    (ref: [36.213, 5.1.1.1.],  [36.331, 6.3.2])
*/
typedef enum {
  NB1_L1_UL_POWER_CTRL_DELTA_MCS_ENABLE_0 = 0,
  NB1_L1_UL_POWER_CTRL_DELTA_MCS_ENABLE_1
} nb1_l1_ul_power_ctrl_delta_mcs_enable_e;

/*! @brief
    Filter Coefficient enum
*/
typedef enum {
  NB1_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_0 = 0,
  NB1_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_1,
  NB1_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_2,
  NB1_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_3,
  NB1_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_4,
  NB1_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_5,
  NB1_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_6,
  NB1_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_7,
  NB1_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_8,
  NB1_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_9,
  NB1_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_11,
  NB1_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_13,
  NB1_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_15,
  NB1_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_17,
  NB1_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_19
} nb1_l1_ul_power_ctrl_filter_coefficient_e;

/*! @brief
  P0 PUSCH setup parameters
  Defined in TBD
*/
typedef struct
{
  /*! P0 value for PUSCH for persistent scheduling range TBD. */
  int16 persistent;

  /*! P0 value for PUSCH for non-persistent scheduling range TBD.*/
  int16 non_persistent;
} nb1_l1_p0_pusch_s;

/*! @brief The maximum number of HARQ transmissions */
typedef enum
{
  NB1_L1_n1 = 1,
  NB1_L1_n2 = 2,
  NB1_L1_n3 = 3,
  NB1_L1_n4 = 4,
  NB1_L1_n5 = 5,
  NB1_L1_n6 = 6,
  NB1_L1_n7 = 7,
  NB1_L1_n8 = 8,
  NB1_L1_n10 = 10,
  NB1_L1_n12 = 12,
  NB1_L1_n16 = 16,
  NB1_L1_n20 = 20,
  NB1_L1_n24 = 24,
  NB1_L1_n28 = 28
} nb1_l1_max_harq_tx_e;

/*! @brief The semi-persistant scheduling intervals */
typedef enum
{
  NB1_L1_SPS_INTERVAL_SF10 = 10,
  NB1_L1_SPS_INTERVAL_SF20 = 20,
  NB1_L1_SPS_INTERVAL_SF30 = 30,
  NB1_L1_SPS_INTERVAL_SF32 = 32,
  NB1_L1_SPS_INTERVAL_SF40 = 40,
  NB1_L1_SPS_INTERVAL_SF60 = 60,
  NB1_L1_SPS_INTERVAL_SF64 = 64,
  NB1_L1_SPS_INTERVAL_SF80 = 80,
  NB1_L1_SPS_INTERVAL_SF120 = 120,
  NB1_L1_SPS_INTERVAL_SF128 = 128,
  NB1_L1_SPS_INTERVAL_SF160 = 160,
  NB1_L1_SPS_INTERVAL_SF320 = 320,
  NB1_L1_SPS_INTERVAL_SF640 = 640,
  NB1_L1_SPS_INTERVAL_NONE = 0
} nb1_l1_sps_interval_e;

/*! @brief
    p0_nominal value for PUCCH Range: -8..7(ref: 36.331, 6.3.2)
*/
typedef int8 nb1_l1_ul_power_ctrl_p0_ue_t;

typedef enum
{
  NB1_L1_DSR_TRANS_MAX_N4 = 4,
  NB1_L1_DSR_TRANS_MAX_N8 = 8,
  NB1_L1_DSR_TRANS_MAX_N16 = 16,
  NB1_L1_DSR_TRANS_MAX_N32 = 32,
  NB1_L1_DSR_TRANS_MAX_N64 = 64
}nb1_l1_dsr_trans_max_e ;


/*! @brief
  The CQI format Indicator
*/
typedef enum
{
  NB1_L1_CQI_FORMAT_INDICATOR_PERIODIC_WIDEBAND,
  NB1_L1_CQI_FORMAT_INDICATOR_PERIODIC_SUBBAND
}nb1_l1_cqi_format_indicator_e;

/*! @brief
    Parameter: reporting mode. Value rm12 corresponds to Mode 1-2, rm20
    corresponds to Mode 2-0, rm22 corresponds to Mode 2-2 etc. PUSCH reporting
    modes are described in TS 36.213 [23, 7.2.1].
*/
typedef enum
{
  NB1_L1_CQI_MODE_APERIODIC_RM12 = 0,
  NB1_L1_CQI_MODE_APERIODIC_RM20,
  NB1_L1_CQI_MODE_APERIODIC_RM22,
  NB1_L1_CQI_MODE_APERIODIC_RM30,
  NB1_L1_CQI_MODE_APERIODIC_RM31,
  NB1_PUSCH_CSF_MAX_REPORTING_MODES,
} nb1_l1_cqi_reporting_mode_aperiodic_e;


/*! @brief
  csi-ConfigIndex
  E-UTRAN configures csi-ConfigIndex only for PCell and only if
  csi-SubframePatternConfig is configured. The UE shall release csi-ConfigIndex
  if csi-SubframePatternConfig is released. CSI config Index
*/

typedef struct
{
   /*! CSI config index enable flag */
  boolean enable;
   /*! cqi-pmi-ConfigIndex2-r10 	  	 INTEGER   (0 .  . 1023 ) */
  uint8 cqi_pmi_config_index2;
  /*! RI Config index2 enable flag */
  boolean enable_ri_cfg_index2;
  /*! ri-ConfigIndex2-r10 	  	  	  	 INTEGER   (0 .  . 1023 ) */
  uint8 ri_cfg_index2;

}nb1_l1_csi_cfg_idx_s;

/*! @brief Periodic PUCCH CQI reporting types added for CSF schedule message.
    Types are semi-statically configured through higher layers 36.213 Tab. 7.2.2-1.
*/
typedef enum
{
   NB1_PUCCH_CSF_REPORT_TYPE_1 = 1,  ///< Type 1: SB CQI
   NB1_PUCCH_CSF_REPORT_TYPE_2 = 2,  ///< Type 2: WB CQI, PMI
   NB1_PUCCH_CSF_REPORT_TYPE_3 = 3,  ///< Type 3: RI
   NB1_PUCCH_CSF_REPORT_TYPE_4 = 4,  ///< Type 4: WB CQI
   NB1_PUCCH_CSF_REPORT_TYPE_5 = 5,  ///< Type 5 report: RI and WB PMI_1
   NB1_PUCCH_CSF_REPORT_TYPE_6 = 6,  ///< Type 5 report: RI and WB PMI_1
   NB1_PUCCH_CSF_REPORT_TYPE_1A = 7, ///< type 1A: SB CQI and PMI_2
   NB1_PUCCH_CSF_REPORT_TYPE_2A = 8, ///< type 2A: WB PMI
   NB1_PUCCH_CSF_REPORT_TYPE_2B = 9, ///< type 2B: WB CQI, WB PMI_1, differential WB CQI_2 if RI>1
   NB1_PUCCH_CSF_REPORT_TYPE_2C = 10,///< type 2C: WB CQI, WB PMI_1, WB PMI_2, differential WB CQI_2 if RI>1
   NB1_PUCCH_CSF_INVALID_REPORT_TYPE
}lte_LL1_nb1_pucch_csf_report_type_enum;

/*! @brief
  CQI reporting Periodic
*/

typedef struct
{

  /*! Reporting Periodic enable flag */
  boolean enable;

  /*! PUCCH resource Index. Range: 0-767 */
  uint16 pucch_resource_index;

  /*! CQI/PMI Periodicity and Offset Configuration Index.  Range: 0-511*/
  uint16 cqi_pmi_cfg_index;

  /* PUCCH CQI Feedback Type, see TS 36.213 [23, table 7.2.2-1]. Depending on
     transmissionMode, reporting mode is implicitly given from the table. */
  nb1_l1_cqi_format_indicator_e format_indicator;

  /* Paramater K - Used only if CQI format indicator is set to
     NB1_L1_CQI_FORMAT_INDICATOR_PERIODIC_SUBBAND.  Range: 1-4 */
  uint8 subband_cqi_k;

  /* RI cfg index enable / disable flag */
  boolean ri_cfg_index_enable;

  /* Parameter RI cfg index */
  uint16 ri_cfg_index;

  /*! Parameter: Simultaneous-AN-and-CQI. see TS 36.213 [23, 10.1] TRUE indicates
      that simultaneous transmission of ACK/NACK and CQI is allowed. */
  boolean sim_ack_nak_cqi;

  /*! csi-ConfigIndex only for PCell and only if csi-SubframePatternConfig is configured.
      The UE shall release csi-ConfigIndex if csi-SubframePatternConfig is
      released. CSI config Index */
  nb1_l1_csi_cfg_idx_s csi_cfg_idx;

}nb1_l1_cqi_reporting_periodic_s;

/*! @brief
  The CQI Report mode
*/
typedef enum
{
  NB1_L1_CQI_SUB_MODE_1 ,
  NB1_L1_CQI_SUB_MODE_2
} nb1_l1_csi_report_mode_e;

/*! @brief
  The CQI periodicty factor
*/
typedef enum
{
  NB1_L1_CQI_PERIOD_FACT_N2 ,
  NB1_L1_CQI_PERIOD_FACT_N4
} nb1_l1_csi_periodicity_factor_e;

/*! @brief
  The CQI config index action
*/
typedef enum
{
  NB1_L1_CQI_CONFIG_INDEX_SETUP ,
  NB1_L1_CQI_CONFIG_INDEX_RELEASE
} nb1_l1_csi_config_index_action_e;

  /*! @brief
  aperiodicCSI-Trigger as part of CQI-ReportAperiodic r10
  */
typedef struct
{
  /*! Indicates if field within this structure are valid or not */
  boolean enable;
  /*! indicates for which serving cell(s) the aperiodic CSI report is triggered when one or more SCells are configured.
  trigger1 corresponds to the CSI request field 10 and trigger2 corresponds to the CSI request field 11, see TS 36.213 [23, table 7.2.1-1A].
  The leftmost bit, bit 0 in the bit string corresponds to the cell with ServCellIndex=0 and bit 1 in the bit string corresponds to the cell with
  ServCellIndex=1 etc.Bit 0 is Most Significant Bit(Left Most bit) of the BITSTRING*/
  uint8 trigger1;
  uint8 trigger2;
}nb1_l1_cqi_aperiodic_rel10_param_s;

/*! @brief
  csi-ConfigIndex
  E-UTRAN configures csi-ConfigIndex only for PCell and only if
  csi-SubframePatternConfig is configured. The UE shall release csi-ConfigIndex
  if csi-SubframePatternConfig is released. CSI config Index
*/
typedef struct
{
  /*! Indicates if field within this structure are valid or not */
  boolean enable;

  /*cqi_pmi_config_Index2     INTEGER   (0 .  . 1023 )  ,  */
  uint16 cqi_pmi_config_Index2;
  /*valid only when csi_config_index_action = SETUP, ri_config_index2    INTEGER   (0 .  . 1023 )  ,      */
  boolean ri_config_index2_presnt;
  /*RI Config Index IRI, see TS 36.213 [23, 7.2.2-1B]*/
  uint16 ri_config_index2;

}nb1_l1_csi_cfg_index_rel10_param_s;


/*! @brief
   CQI-Reportperiodic r10
*/
typedef struct
{
  /*! Indicates if field within this structure are valid or not */
  boolean enable;

  /*! mL1 to look at pucch_resource_indexp1 only if pucch_resource_indexp1_present is TRUE*/
  boolean pucch_resource_indexp1_present;
  /*! PUCCH resource Index. Range: (0 .  . 1184 ) */
  uint16 pucch_resource_indexp1;

  /*csi_reportmode is valid only if format_indicator = NB1_L1_CQI_FORMAT_INDICATOR_PERIODIC_WIDEBAND*/
  boolean csi_reportmode_valid;
  nb1_l1_csi_report_mode_e csi_reportmode;

  /* Paramater periodicityFactor - Used only if CQI format indicator is set to
     NB1_L1_CQI_FORMAT_INDICATOR_PERIODIC_SUBBAND.  Range: n2,n4 */
  nb1_l1_csi_periodicity_factor_e subband_cqi_periodicity_factor;

  /*! csi-ConfigIndex-r10 */
  nb1_l1_csi_cfg_index_rel10_param_s csi_cfg_index;
}nb1_l1_cqi_periodic_rel10_param_s;

/*! @brief
  MeasSubframePattern type
*/
typedef enum
{
  NB1_L1_MEAS_SUBFM_PATTERN_FDD ,
  NB1_L1_MEAS_SUBFM_PATTERN_TDD
} nb1_l1_meas_subfm_pattern_type_e;


/*! @brief
   MeasSubframePattern-r10
*/
typedef struct
{
  /*! Indicate which of TDD or FDD cfg valid */
  nb1_l1_meas_subfm_pattern_type_e type;

  /*! The IE MeasSubframePattern is used to specify time domain measurement
      resource restriction. The first/leftmost bit corresponds to the
      subframe #0 of the radio frame satisfying SFN mod x = 0, where SFN is that
      of PCell and x is the size of the bit string divided by 10. "1" denotes
      that the corresponding subframe is used for measurement.
      FDD pattern is 40 bits.
      TDD could be 20, 70 or 60 bits.
  */
  uint8 subfm_num_bits;
  uint32 subfm_pattern[NB1_L1_MAX_MEAS_SUBFM_PATTERN_SIZE];

  /*add TDD part when we will add CA support for TDD*/

}nb1_l1_meas_subfm_pattern_param_s;


/*! @brief
   csi-SubframePatternConfig-r10
*/
typedef struct
{
  /*! Indicates if field within this structure are valid or not */
  boolean enable;

  nb1_l1_meas_subfm_pattern_param_s csi_meas_subfm_set1;

  nb1_l1_meas_subfm_pattern_param_s csi_meas_subfm_set2;

}nb1_l1_csi_subfm_pattern_cfg_param_s;


/*! @brief
   measurement subframe pattern used for RRM and RLM
*/
typedef struct
{
  /*! Indicates if field within this structure are valid or not */
  boolean enable;

  nb1_l1_meas_subfm_pattern_param_s meas_subfm;

}nb1_l1_subfm_pattern_cfg_param_s;


/*! @brief
  CQI reporting Release 10 parameters
*/
typedef struct
{
  /*!CQI-ReportAperiodic -r10 */
  nb1_l1_cqi_aperiodic_rel10_param_s cqi_aperiodic_cfg;

  /*!CQI-Reportperiodic -r10 */
  nb1_l1_cqi_periodic_rel10_param_s cqi_periodic_cfg;

  /*!csi-SubframePatternConfig-r10, needed only for on enhanced-ICIC  feature */
  nb1_l1_csi_subfm_pattern_cfg_param_s csi_subfm_pattern_cfg;

}nb1_l1_cqi_rel10_param_s;


/*! @brief
  CQI reporting Release 9 parameters
*/

typedef struct
{

  /*! CQI Mask enable flag */
  boolean cqi_mask_enable;

  /*! PMI_RI report configured flag */
  boolean pmi_ri_report_configured;

}nb1_l1_cqi_rel9_param_s;


/*! @brief
  PDSCH assignment type
  Defined in TBD
*/
typedef enum
{
  NB1_L1_DL_PDSCH_DYNAMIC_ASSIGN = 0,
  NB1_L1_DL_PDSCH_PERSISTENT_ASSIGN
} nb1_l1_dl_pdsch_assignment_e;


/*! @brief
  PHICH decoder result enum
  Defined in TBD
*/
typedef enum
{
  NB1_L1_PHICH_RESULT_NACK = 0,
  NB1_L1_PHICH_RESULT_ACK,             // same as before

  NB1_L1_PHICH_RESULT_NACK1_NACK0 = 0, //  i.e. 0 0
  NB1_L1_PHICH_RESULT_NACK1_ACK0,      //       0 1
  NB1_L1_PHICH_RESULT_ACK1_NACK0,      //       1 0
  NB1_L1_PHICH_RESULT_ACK1_ACK0,       //       1 1

} nb1_l1_phich_result_e;

/*! @brief
  PDCCH header type.
  Defined in 3.2.3.2 pdcch header from Espresso UE ML-LL interface
  TBD: to discuss with LL folks to include the detail pdcch header struct
  in the this file.
*/
typedef uint64 nb1_l1_pdcch_header_t;


/*! @brief
  PDCCH header type.
  Defined in 3.2.3.3 PDCCH payload from Espresso UE ML-LL interface
  TBD: to discuss with LL folks to include the detail pdcch header struct
*/
typedef uint64 nb1_l1_pdcch_payload_t;


/*! @brief
  PDSCH result
  Defined in 3.2.3.4 LTE_LL1_DL_PDSCH_RESULT_IND of Espresso UE ML-LL interface
  TBD: to discuss with LL folks to include the detail PDSCH result struct
*/
typedef struct
{
  /*! subframe index within a radio frame [0..9] */
  nb1_l1_subfn_t subfn;
} nb1_l1_pdsch_result_s;


/*! @brief
  PDSCH deocder header strcut
  Defined in 3.2.3.5 pdsch_decoder_data header from Espresso UE ML-LL interface
  TBD: to discuss with LL folks to include the detail struct
*/
typedef struct
{
  /*! SFN index tagged by the DDE*/
  nb1_l1_sfn_t sfn_idx;
} nb1_l1_pdsch_decoder_header_s;

/*! @brief
    The offset between PDSCH and RS EPRE. (ref:36.213, 5.2). P_A provides
    information about the exact power setting of the PDSCH transmission.
    dB_n6 corresponds to -6 dB, dB_3 corresponds to 3 dB etc.
*/
typedef enum {
  NB1_L1_PA_DB_N6 = 0,
  NB1_L1_PA_DB_N4DOT77,
  NB1_L1_PA_DB_N3,
  NB1_L1_PA_DB_N1DOT77,
  NB1_L1_PA_DB_0,
  NB1_L1_PA_DB_1,
  NB1_L1_PA_DB_2,
  NB1_L1_PA_DB_3,
  /* Used when PA unknown*/
  NB1_L1_PA_FW_HYPOTHESIS,
  NB1_L1_MAX_NUM_PA
} nb1_l1_pa_e;

/*! @brief
    Parameter: PERIODIC_PHR_TIMER [36.321]. Value in number of sub-frames. Value
    sf10 corresponds to 10 subframes, sf20 corresponds to 20 subframes and so on.
*/
typedef enum {
  NB1_L1_PERIODIC_PHR_TIMER_SF10 = 10,
  NB1_L1_PERIODIC_PHR_TIMER_SF20 = 20,
  NB1_L1_PERIODIC_PHR_TIMER_SF50 = 50,
  NB1_L1_PERIODIC_PHR_TIMER_SF100 = 100,
  NB1_L1_PERIODIC_PHR_TIMER_SF200 = 200,
  NB1_L1_PERIODIC_PHR_TIMER_SF500 = 500,
  NB1_L1_PERIODIC_PHR_TIMER_SF1000 = 1000,
  NB1_L1_PERIODIC_PHR_TIMER_INFINITE = 0
} nb1_l1_periodic_phr_timer_e;

/*! @brief
    Parameter: PROHIBIT PHR TIMER [36.321]. Value in number of sub-frames. Value
    sf10 corresponds to 10 subframes, sf20 corresponds to 20 subframes and so on.
*/
typedef enum {
  NB1_L1_PROHOBIT_PHR_TIMER_SF0 = 0,
  NB1_L1_PROHOBIT_PHR_TIMER_SF10 = 10,
  NB1_L1_PROHOBIT_PHR_TIMER_SF20 = 20,
  NB1_L1_PROHOBIT_PHR_TIMER_SF50 = 50,
  NB1_L1_PROHOBIT_PHR_TIMER_SF100 = 100,
  NB1_L1_PROHOBIT_PHR_TIMER_SF200 = 200,
  NB1_L1_PROHIBIT_PHR_TIMER_SF500 = 500,
  NB1_L1_PROHOBIT_PHR_TIMER_SF1000 = 1000
} nb1_l1_prohibit_phr_timer_e;

/*! @brief
    Parameter: DL PathlossChange [36.321]. Value in dB. Value dB1 corresponds to
    1 dB, dB3 corresponds to 3 dB and so on.
*/
typedef enum {
  NB1_L1_DL_PATHLOSS_DB_1 = 1,
  NB1_L1_DL_PATHLOSS_DB_3 = 3,
  NB1_L1_DL_PATHLOSS_DB_6 = 6,
  NB1_L1_DL_PATHLOSS_DB_INFINITE
} nb1_l1_dl_pathloss_change_e;

/*! @brief
  SPS configuration for Downlink
*/
typedef struct
{
  /*! Indicates if field within this structure are valid or not */
  boolean valid;

  /*! Flag to enable\disable SPS config for DL */
  boolean enable;

  /*! The downlink semi-persistant scheduling interval */
  nb1_l1_sps_interval_e sps_interval;

  /*! Number of configured SPS processes.  Range: 1-8*/
  uint8 num_conf_sps_processes;

  /*! Parameter: N1 PUCCH.  see TS 36.213, [23, 10.1]*/
  uint8 n1_pucch_an_list_size;
  uint16 n1_pucch_an_persistent[MAX_N1_PUCCH_AN_LIST_SIZE];

}nb1_l1_sps_cfg_dl_s;

/*! @brief
  SPS configuration for Uplink
*/
typedef struct
{
  /*! Indicates if field within this structure are valid or not */
  boolean valid;

  /*! Flag to enable\disable SPS config for UL */
  boolean enable;

  /*! The uplink semi-persistant scheduling interval */
  nb1_l1_sps_interval_e sps_interval;

  /*! Flag to enable\disable two-intervals-SPS for uplink */
  boolean two_intervals_config;

  /*! Flag to indicate if p0_persistent is signaled or not; The flag applies
    to p0_nominal_pusch_persistent and p0_ue_pusch_persistent fields below.
    If set to TRUE, ML1 should apply the values signaled in these fields;
    If set to FALSE, ML1 should apply the values as follows, per 36.331 sec 6.3.2.
    p0-NominalPUSCH for p0-NominalPUSCH-Persistent
    p0-UE-PUSCH for p0-UE-PUSCH-Persistent. */
  boolean p0_persistent_present;

  /*! See TS 36.213, 5.1.1.1, unit dBm step 1. This field is applicable for
      persistent scheduling, only. Range -126-24 */
  int16 p0_nominal_pusch_persistent;

  /*! See TS 36.213, 5.1.1.1, unit dB. This field is applicable for persistent
      scheduling, only.  Range -8-7 */
  nb1_l1_ul_power_ctrl_p0_ue_t p0_ue_pusch_persistent;


}nb1_l1_sps_cfg_ul_s;

/*===========================================================================

                          SEARCH RELATED TYPES

===========================================================================*/
/*! @brief
  Search coherent accumulation length
  Defined 3.4.1.2  LTE_LL1_SRCH_REQ_INIT_ACQ from Espresso UE ML-LL interface
*/
typedef enum
{
  NB1_L1_SRCH_COH_ACC_LEN_64 = 0,
  NB1_L1_SRCH_COH_ACC_LEN_32,
  NB1_L1_SRCH_COH_ACC_LEN_16
} nb1_l1_srch_coh_acc_len_e;


/*===========================================================================

                    CELL SELECTION/RESELECTION/HANDOVER TYPES

===========================================================================*/
/*! @brief
  Handover info parameteters
  Defined in 36.331 Section 6.3.4 MobilityControlInformation
  TODO fill in this structure
*/
typedef struct
{
  nb1_l1_TBD_t tbd_params;
} nb1_l1_handover_info_param_s;

/*! @brief
  Cell selection parameters for Release 9
*/
typedef struct
{
  /*! Minimum required quality level in the cell in dB, Range (-34 .. -3),
    default value NB1_CPHY_INT8_NEG_INFINITY when not signaled */
  int8 q_qualmin;

  /*! Offset to the signaled q_qualmin, Range (1 .. 8), default value 0
    when not signaled */
  uint8 q_qualmin_offset;

} nb1_l1_cell_select_info_rel9_s;

/*===========================================================================

                    DRX PARAMTER TYPES

===========================================================================*/

/*! NB parameter used to specify the number of paging groups
  36.331 Section 6.3.2
 */
typedef enum
{
  NB1_L1_NB_FOURT = 0,
  NB1_L1_NB_TWOT,
  NB1_L1_NB_ONET,
  NB1_L1_NB_HALFT,
  NB1_L1_NB_QUARTERT,
  NB1_L1_NB_ONEEIGHTT,
  NB1_L1_NB_ONSIXTEENTHT,
  NB1_L1_NB_ONETHIRTYSECONDT,
  NB1_L1_NB_ONESIXTYFOURTHT,
  NB1_L1_NB_ONEONETWENTYEIGHTHT,
  NB1_L1_NB_ONETWOFIFTYSIXTHT,
  NB1_L1_NB_ONEFIVEONETWELVETHT,
  NB1_L1_NB_ONETENTWENTYFOURTHT
} nb1_l1_nb_e;

/*! ACK NACK Number of repititions
  Number of repetitions for the ACK NACK resource unit carrying
    HARQ response to NPDSCH
 */
typedef enum
{
  NB1_L1_ACK_NACK_NUM_REP_R1 = 0,
  NB1_L1_ACK_NACK_NUM_REP_R2,
  NB1_L1_ACK_NACK_NUM_REP_R4,
  NB1_L1_ACK_NACK_NUM_REP_R8,
  NB1_L1_ACK_NACK_NUM_REP_R16,
  NB1_L1_ACK_NACK_NUM_REP_R32,
  NB1_L1_ACK_NACK_NUM_REP_R64,
  NB1_L1_ACK_NACK_NUM_REP_R128
} nb1_l1_ack_nack_num_rep_e;

/*!
  Downlink configuration for the non-anchor carrier
 */
typedef enum
{
  NB1_L1_DL_CFG_NON_ANCHOR_NONE = 0,
  NB1_L1_DL_CFG_NON_ANCHOR_NO_CFG,
  NB1_L1_DL_CFG_NON_ANCHOR_ANCHOR_CFG,
  NB1_L1_DL_CFG_NON_ANCHOR_EXPLICIT_CFG
} nb1_l1_dl_cfg_non_anchor_e;

/*!
  PCI Indicator
 */
typedef enum
{
  NB1_L1_PCI_INDICATOR_SAME,
  NB1_L1_PCI_INDICATOR_DIFF
} nb1_l1_pci_indicator_e;


/*!
@brief
A Struct describing the type of DL Bitmap config
*/
typedef enum
{
  NB1_CPHY_DL_BITMAP_CFG_TYPE_DEFAULT,
  NB1_CPHY_DL_BITMAP_CFG_TYPE_EXPLICIT
}nb1_cphy_dl_bitmap_cfg_type_e;

/*!
@brief
A Struct describing the DL Bitmap
*/
typedef enum
{
  NB1_CPHY_DL_BITMAP_SUBFRAME_PATTERN_SIZE_10_BITS,
  NB1_CPHY_DL_BITMAP_SUBFRAME_PATTERN_SIZE_40_BITS
}nb1_cphy_dl_bitmap_type_e;


/*!
  @brief
  An enum describing the eutra control region size
  */
typedef enum
{
  NB1_CPHY_CONTROL_REGION_SIZE_N1 = 0,
  NB1_CPHY_CONTROL_REGION_SIZE_N2,
  NB1_CPHY_CONTROL_REGION_SIZE_N3
}nb1_cphy_eutra_control_region_size_e;



/*! NPDCCH Number of repititions
 Maximum number of repetitions for NPDCCH UE specific search space
 UE monitors one set of values (consisting of aggregation level, #number of repetitions and
 #number of blind decodes) according to the configured maximum number of repetitions
 */
typedef enum
{
  NB1_L1_NPDCCH_NUM_REP_R1 = 0,
  NB1_L1_NPDCCH_NUM_REP_R2,
  NB1_L1_NPDCCH_NUM_REP_R4,
  NB1_L1_NPDCCH_NUM_REP_R8,
  NB1_L1_NPDCCH_NUM_REP_R16,
  NB1_L1_NPDCCH_NUM_REP_R32,
  NB1_L1_NPDCCH_NUM_REP_R64,
  NB1_L1_NPDCCH_NUM_REP_R128,
  NB1_L1_NPDCCH_NUM_REP_R256,
  NB1_L1_NPDCCH_NUM_REP_R512,
  NB1_L1_NPDCCH_NUM_REP_R1024,
  NB1_L1_NPDCCH_NUM_REP_R2048
} nb1_l1_npdcch_num_rep_e;

/*! NPDCCH Start SF USS
 Starting subframe configuration for an NPDCCH UE-specific search space
 Value v1dot5 corresponds to 1.5, value 2 corresponds to 2 and so on.
 */
typedef enum
{
  NB1_L1_NPDCCH_START_SF_USS_V1DOT5 = 0,
  NB1_L1_NPDCCH_START_SF_USS_V2,
  NB1_L1_NPDCCH_START_SF_USS_V4,
  NB1_L1_NPDCCH_START_SF_USS_V8,
  NB1_L1_NPDCCH_START_SF_USS_V16,
  NB1_L1_NPDCCH_START_SF_USS_V32,
  NB1_L1_NPDCCH_START_SF_USS_V48,
  NB1_L1_NPDCCH_START_SF_USS_V64
} nb1_l1_npdcch_start_sf_uss_e;

/*! NPDCCH Offset USS
 Fractional period offset of starting subframe for NPDCCH UE specific search space USS
 */
typedef enum
{
  NB1_L1_NPDCCH_OFFSET_USS_ZERO = 0,
  NB1_L1_NPDCCH_OFFSET_USS_ONEEIGHTH,
  NB1_L1_NPDCCH_OFFSET_USS_ONEFOURTH,
  NB1_L1_NPDCCH_OFFSET_USS_THREEEIGHTH
} nb1_l1_npdcch_offset_uss_e;

//Number of CRS ports
typedef enum
{
  NB1_EUTRA_NUM_CRS_PORTS_SAME = 0,
  NB1_EUTRA_NUM_CRS_PORTS_FOUR
}nb1_eutra_num_crs_ports_e;


/*! SRS Subframe Config
       sc0 corresponds to value 0, sc1 to value 1 and so on
 */
typedef enum
{
  NB1_L1_SRS_SF_CFG_SC0 = 0,
  NB1_L1_SRS_SF_CFG_SC1,
  NB1_L1_SRS_SF_CFG_SC2,
  NB1_L1_SRS_SF_CFG_SC3,
  NB1_L1_SRS_SF_CFG_SC4,
  NB1_L1_SRS_SF_CFG_SC5,
  NB1_L1_SRS_SF_CFG_SC6,
  NB1_L1_SRS_SF_CFG_SC7,
  NB1_L1_SRS_SF_CFG_SC8,
  NB1_L1_SRS_SF_CFG_SC9,
  NB1_L1_SRS_SF_CFG_SC10,
  NB1_L1_SRS_SF_CFG_SC11,
  NB1_L1_SRS_SF_CFG_SC12,
  NB1_L1_SRS_SF_CFG_SC13,
  NB1_L1_SRS_SF_CFG_SC14,
  NB1_L1_SRS_SF_CFG_SC15
} nb1_l1_srs_sf_cfg_e;

/*!
  NPRACH Periodicity
  Periodicity of a NPRACH resource. Unit in millisecond
 */
typedef enum
{
  NB1_L1_NPRACH_PERIODICITY_40 = 0,
  NB1_L1_NPRACH_PERIODICITY_80,
  NB1_L1_NPRACH_PERIODICITY_160,
  NB1_L1_NPRACH_PERIODICITY_240,
  NB1_L1_NPRACH_PERIODICITY_320,
  NB1_L1_NPRACH_PERIODICITY_640,
  NB1_L1_NPRACH_PERIODICITY_1280,
  NB1_L1_NPRACH_PERIODICITY_2560
} nb1_l1_nprach_periodicity_e;

/*! NPRACH Start time in ms
    Start time of the NPRACH resource in one period. Unit in millisecond
 */
typedef enum
{
  NB1_L1_NPRACH_START_TIME_8 = 0,
  NB1_L1_NPRACH_START_TIME_16,
  NB1_L1_NPRACH_START_TIME_32,
  NB1_L1_NPRACH_START_TIME_64,
  NB1_L1_NPRACH_START_TIME_128,
  NB1_L1_NPRACH_START_TIME_256,
  NB1_L1_NPRACH_START_TIME_512,
  NB1_L1_NPRACH_START_TIME_1024
} nb1_l1_nprach_start_time_e;

/*! NPRACH Sub Carrier Offset
  Frequency location of the NPRACH resource.
  In number of subcarriers, offset from sub-carrier 0 .
 */
typedef enum
{
  NB1_L1_NPRACH_SUB_CARR_OFFSET_0 = 0,
  NB1_L1_NPRACH_SUB_CARR_OFFSET_12,
  NB1_L1_NPRACH_SUB_CARR_OFFSET_24,
  NB1_L1_NPRACH_SUB_CARR_OFFSET_36,
  NB1_L1_NPRACH_SUB_CARR_OFFSET_2,
  NB1_L1_NPRACH_SUB_CARR_OFFSET_18,
  NB1_L1_NPRACH_SUB_CARR_OFFSET_34
} nb1_l1_nprach_sub_carr_offset_e;

/*! NPRACH Number of Sub Carriers
  Number of sub-carriers in a NPRACH resource. In number of subcarriers
 */
typedef enum
{
  NB1_L1_NPRACH_NUM_SUB_CARR_12 = 0,
  NB1_L1_NPRACH_NUM_SUB_CARR_24,
  NB1_L1_NPRACH_NUM_SUB_CARR_36,
  NB1_L1_NPRACH_NUM_SUB_CARR_48
} nb1_l1_nprach_num_sub_carr_e;

/*! NPRACH Number of CBRA start sub carriers
 */
typedef enum
{
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_8 = 0,
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_10,
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_11,
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_12,
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_20,
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_22,
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_23,
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_24,
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_32,
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_34,
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_35,
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_36,
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_40,
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_44,
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_46,
  NB1_L1_NPRACH_NUM_CBRA_START_SUB_CARR_48
} nb1_l1_nprach_num_cbra_start_sub_carr_e;

/*!
   NPRACH Sub Carrier MSG3 range start
   Fraction for calculating the starting subcarrier index of the range reserved for indication of
   UE support for multi-tone Msg3 transmission, within the NPRACH resource..
   Multi-tone Msg3 transmission is not supported for {32, 64, 128} repetitions of NPRACH.
   For at least one of the NPRACH resources with the number of NPRACH repetitions other
   than {32, 64, 128}, the value of nprach-SubcarrierMSG3-RangeStart should be less than 1
 */
typedef enum
{
  NB1_L1_NPRACH_SC_MSG3_RANGE_START_ZERO = 0,
  NB1_L1_NPRACH_SC_MSG3_RANGE_START_ONETHIRD,
  NB1_L1_NPRACH_SC_MSG3_RANGE_START_TWOTHIRD,
  NB1_L1_NPRACH_SC_MSG3_RANGE_START_ONE
} nb1_l1_nprach_sc_msg3_range_start_e;

/*!
  Max number of Preamble Attempt
  Maximum number of preamble transmission attempts per NPRACH resource
 */
typedef enum
{
  NB1_L1_NPRACH_MAX_NUM_PREAMBLE_ATTEMPT_3 = 0,
  NB1_L1_NPRACH_MAX_NUM_PREAMBLE_ATTEMPT_4,
  NB1_L1_NPRACH_MAX_NUM_PREAMBLE_ATTEMPT_5,
  NB1_L1_NPRACH_MAX_NUM_PREAMBLE_ATTEMPT_6,
  NB1_L1_NPRACH_MAX_NUM_PREAMBLE_ATTEMPT_7,
  NB1_L1_NPRACH_MAX_NUM_PREAMBLE_ATTEMPT_8,
  NB1_L1_NPRACH_MAX_NUM_PREAMBLE_ATTEMPT_10
} nb1_l1_nprach_max_num_preamble_attempt_e;

/*!
  Number of repetitions Per Preamble Attempt
  Number of NPRACH repetitions per attempt for each NPRACH resource
 */
typedef enum
{
  NB1_L1_NPRACH_NUM_REP_PER_PREAMBLE_ATTEMPT_1  = 0,
  NB1_L1_NPRACH_NUM_REP_PER_PREAMBLE_ATTEMPT_2,
  NB1_L1_NPRACH_NUM_REP_PER_PREAMBLE_ATTEMPT_4,
  NB1_L1_NPRACH_NUM_REP_PER_PREAMBLE_ATTEMPT_8,
  NB1_L1_NPRACH_NUM_REP_PER_PREAMBLE_ATTEMPT_16,
  NB1_L1_NPRACH_NUM_REP_PER_PREAMBLE_ATTEMPT_32,
  NB1_L1_NPRACH_NUM_REP_PER_PREAMBLE_ATTEMPT_64,
  NB1_L1_NPRACH_NUM_REP_PER_PREAMBLE_ATTEMPT_128,
  NB1_L1_NPRACH_NUM_REP_PER_PREAMBLE_MAX
} nb1_l1_nprach_num_rep_per_preamble_attempt_e;


/*!
  NPDCCH Number of repetitions
  Maximum number of repetitions for NPDCCH common search space (CSS)
  for RAR, Msg3 retransmission and Msg4
 */
typedef enum
{
  NB1_L1_NPRACH_NPDCCH_NUM_REP_1 = 0,
  NB1_L1_NPRACH_NPDCCH_NUM_REP_2,
  NB1_L1_NPRACH_NPDCCH_NUM_REP_4,
  NB1_L1_NPRACH_NPDCCH_NUM_REP_8,
  NB1_L1_NPRACH_NPDCCH_NUM_REP_16,
  NB1_L1_NPRACH_NPDCCH_NUM_REP_32,
  NB1_L1_NPRACH_NPDCCH_NUM_REP_64,
  NB1_L1_NPRACH_NPDCCH_NUM_REP_128,
  NB1_L1_NPRACH_NPDCCH_NUM_REP_256,
  NB1_L1_NPRACH_NPDCCH_NUM_REP_512,
  NB1_L1_NPRACH_NPDCCH_NUM_REP_1024,
  NB1_L1_NPRACH_NPDCCH_NUM_REP_2048
} nb1_l1_nprach_npdcch_num_rep_e;

/*! NPDCCH Start SF CCS
  Starting subframe configuration for NPDCCH common search space (CSS),
  including RAR, Msg3 retransmission, and Msg4,
 */
typedef enum
{
  NB1_L1_NPRACH_NPDCCH_START_SF_CSS_1DOT5 = 0,
  NB1_L1_NPRACH_NPDCCH_START_SF_CSS_2,
  NB1_L1_NPRACH_NPDCCH_START_SF_CSS_4,
  NB1_L1_NPRACH_NPDCCH_START_SF_CSS_8,
  NB1_L1_NPRACH_NPDCCH_START_SF_CSS_16,
  NB1_L1_NPRACH_NPDCCH_START_SF_CSS_32,
  NB1_L1_NPRACH_NPDCCH_START_SF_CSS_48,
  NB1_L1_NPRACH_NPDCCH_START_SF_CSS_64
} nb1_l1_nprach_npdcch_start_sf_css_e;

/*! NDPCCH Offset
  Fractional period offset of starting subframe for NPDCCH common search space
 */
typedef enum
{
  NB1_L1_NPRACH_NPDCCH_OFFSET_ZERO = 0,
  NB1_L1_NPRACH_NPDCCH_OFFSET_ONEEIGTH,
  NB1_L1_NPRACH_NPDCCH_OFFSET_ONEFOURTH,
  NB1_L1_NPRACH_NPDCCH_OFFSET_THREEEIGHTH
} nb1_l1_nprach_npdcch_offset_e;

/*! Number of Repitition paging (nPDCCH) specified in
  36.331 Section 6.3.2
   PP2 corresponds to 2 PDCCH periods
 */
typedef enum
{
  NB1_L1_RA_RSP_WIN_SIZE_PP2 = 0,
  NB1_L1_RA_RSP_WIN_SIZE_PP3,
  NB1_L1_RA_RSP_WIN_SIZE_PP4,
  NB1_L1_RA_RSP_WIN_SIZE_PP5,
  NB1_L1_RA_RSP_WIN_SIZE_PP6,
  NB1_L1_RA_RSP_WIN_SIZE_PP7,
  NB1_L1_RA_RSP_WIN_SIZE_PP8,
  NB1_L1_RA_RSP_WIN_SIZE_PP10
} nb1_l1_ra_rsp_win_size_e;

/*!
   NPRACH CP Length
   Cyclic prefix length for NPRACH transmisision
   Value us66dot7 corresponds to 66.7 microseconds and
   value us266dot7 corresponds to 266.7 microseconds
 */
typedef enum
{
  NB1_L1_PRACH_CP_LEN_US66DOT7 = 0,
  NB1_L1_PRACH_CP_LEN_US266DOT7
} nb1_l1_prach_cp_len_e;


/*! Number of Repitition paging (nPDCCH) specified in
  36.331 Section 6.3.2
 */
typedef enum
{
  NB1_L1_NUM_REP_PAGING_1 = 0,
  NB1_L1_NUM_REP_PAGING_2,
  NB1_L1_NUM_REP_PAGING_4,
  NB1_L1_NUM_REP_PAGING_8,
  NB1_L1_NUM_REP_PAGING_16,
  NB1_L1_NUM_REP_PAGING_32,
  NB1_L1_NUM_REP_PAGING_64,
  NB1_L1_NUM_REP_PAGING_128,
  NB1_L1_NUM_REP_PAGING_256,
  NB1_L1_NUM_REP_PAGING_512,
  NB1_L1_NUM_REP_PAGING_1024,
  NB1_L1_NUM_REP_PAGING_2048
} nb1_l1_num_rep_paging_e;


/*Relative paging weights */
typedef enum
{
	NB1_L1_PAGE_WEIGHT_0 = 0,
	NB1_L1_PAGE_WEIGHT_1 = 1,
	NB1_L1_PAGE_WEIGHT_2 = 2,
	NB1_L1_PAGE_WEIGHT_3 = 3,
	NB1_L1_PAGE_WEIGHT_4 = 4,
	NB1_L1_PAGE_WEIGHT_5 = 5,
	NB1_L1_PAGE_WEIGHT_6 = 6,
	NB1_L1_PAGE_WEIGHT_7 = 7,
	NB1_L1_PAGE_WEIGHT_8 = 8,
	NB1_L1_PAGE_WEIGHT_9 = 9,
	NB1_L1_PAGE_WEIGHT_10 = 10,
	NB1_L1_PAGE_WEIGHT_11 = 11,
	NB1_L1_PAGE_WEIGHT_12 = 12,
	NB1_L1_PAGE_WEIGHT_13 = 13,
	NB1_L1_PAGE_WEIGHT_14 = 14,
	NB1_L1_PAGE_WEIGHT_15 = 15,
	NB1_L1_PAGE_WEIGHT_16 = 16
}nb1_l1_paging_weight_e;

/*! DL Gap threshold
        Coefficient to calculate the gap duration of a DL
        transmission: dl-GapDurationCoeff * dl-GapPeriodicity, Duration in number of subframes.
 */
typedef enum
{
  NB1_L1_DL_GAP_THRESH_N32 = 0,
  NB1_L1_DL_GAP_THRESH_N64,
  NB1_L1_DL_GAP_THRESH_N128,
  NB1_L1_DL_GAP_THRESH_N256
} nb1_l1_dl_gap_thresh_e;

/*! DL Gap Periodicity
   Periodicity of a DL transmission gap in number of subframes
 */
typedef enum
{
  NB1_L1_DL_GAP_PERIODICITY_SF64 = 0,
  NB1_L1_DL_GAP_PERIODICITY_SF128,
  NB1_L1_DL_GAP_PERIODICITY_SF256,
  NB1_L1_DL_GAP_PERIODICITY_SF512
} nb1_l1_dl_gap_periodicity_e;

/*! DL Gap Duration Coeff
  Threshold on the maximum number of repetitions configured for
  NPDCCH before application of DL transmission gap configuration
 */
typedef enum
{
  NB1_L1_DL_GAP_DURATION_COEFF_ONEEIGHTHTH = 0,
  NB1_L1_DL_GAP_DURATION_COEFF_ONEFOURTHTH,
  NB1_L1_DL_GAP_DURATION_COEFF_THREEEIGHTHTH,
  NB1_L1_DL_GAP_DURATION_COEFF_ONEHALF
} nb1_l1_dl_gap_duration_coeff_e;



/*! The ue id to be used for the paging interval */
typedef uint64 nb1_l1_drx_ue_id_t;

/*! Minimum Rx level to camp on cell (-70..-22) * 2 */
typedef int32 nb1_l1_q_rxlevmin_t;

/*! Range: 1..8 */
typedef uint32 nb1_l1_q_rxlevmin_offset_t;

/*! Reselection Threshold Range: 0..63 for C DMA2000; 0..31 for all other RATs */
typedef uint8 nb1_l1_resel_threshold_t;

/*! Invalid value for measurement search thresholds. */
#define NB1_CPHY_MEAS_SRCH_THRESHOLD_INVALID    0xFF

/*! Search Threshold Range: 0..31 */
typedef uint8 nb1_l1_srch_meas_threshold_t;

/*! Resel priority value not provided by NW. */
#define NB1_CPHY_MEAS_RESEL_PRIORITY_NONE    0xFF

/*! Resel priority value when a freq is deprioritized. Priority should be treated as
  lower than the network configured priority 0..7 */
#define NB1_CPHY_MEAS_RESEL_PRIORITY_MIN     -1

/*! Reselection Priority Range: -1..7.
    NB1_CPHY_MEAS_RESEL_PRIORITY_NONE is used for serving freq when NW doesn't
    provide priority info.

    NB1_CPHY_MEAS_RESEL_PRIORITY_MIN is used when a freq is deprioritized and its
    priority should be treated as lower than the network configured priority 0..7

    Non serving freqs and IRAT freqs are filtered out by RRC if NW doesn't provide
    cell resel priority info.
 */
typedef int16 nb1_l1_resel_priority_t;

/*! Reselection Timeout Range: 0..7 */
typedef uint8 nb1_l1_treselection_t;



/*===========================================================================

                   CONNECTED MODE DRX PARAMETER TYPES

===========================================================================*/

typedef enum
{
  NB1_L1_ON_DURATION_TIMER_PSF1 = 1,
  NB1_L1_ON_DURATION_TIMER_PSF2 = 2,
  NB1_L1_ON_DURATION_TIMER_PSF3 = 3,
  NB1_L1_ON_DURATION_TIMER_PSF4 = 4,
  NB1_L1_ON_DURATION_TIMER_PSF5 = 5,
  NB1_L1_ON_DURATION_TIMER_PSF6 = 6,
  NB1_L1_ON_DURATION_TIMER_PSF8 = 8,
  NB1_L1_ON_DURATION_TIMER_PSF10 = 10,
  NB1_L1_ON_DURATION_TIMER_PSF20 = 20,
  NB1_L1_ON_DURATION_TIMER_PSF30 = 30,
  NB1_L1_ON_DURATION_TIMER_PSF40 = 40,
  NB1_L1_ON_DURATION_TIMER_PSF50 = 50,
  NB1_L1_ON_DURATION_TIMER_PSF60 = 60,
  NB1_L1_ON_DURATION_TIMER_PSF80 = 80,
  NB1_L1_ON_DURATION_TIMER_PSF100 = 100,
  NB1_L1_ON_DURATION_TIMER_PSF200 = 200
} nb1_l1_on_duration_timer_e;


typedef enum
{
  NB1_L1_DRX_INACTIVITY_TIMER_PSF0 = 0,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF1 = 1,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF2 = 2,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF3 = 3,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF4 = 4,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF5 = 5,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF6 = 6,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF8 = 8,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF10 = 10,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF20 = 20,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF30 = 30,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF40 = 40,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF50 = 50,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF60 = 60,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF80 = 80,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF100 = 100,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF200 = 200,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF300 = 300,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF500 = 500,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF750 = 750,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF1280 = 1280,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF1920 = 1920,
  NB1_L1_DRX_INACTIVITY_TIMER_PSF2560 = 2560
}nb1_l1_drx_inactivity_timer_e;


typedef enum
{
  NB1_L1_DRX_RETX_TIMER_SF1 = 1,
  NB1_L1_DRX_RETX_TIMER_SF2 = 2,
  NB1_L1_DRX_RETX_TIMER_SF4 = 4,
  NB1_L1_DRX_RETX_TIMER_SF6 = 6,
  NB1_L1_DRX_RETX_TIMER_SF8 = 8,
  NB1_L1_DRX_RETX_TIMER_SF16 = 16,
  NB1_L1_DRX_RETX_TIMER_SF24 = 24,
  NB1_L1_DRX_RETX_TIMER_SF33 = 33
} nb1_l1_drx_retx_timer_e;


typedef enum
{
  NB1_L1_DRX_LONG_CYCLE_SF10 = 10,
  NB1_L1_DRX_LONG_CYCLE_SF20 = 20,
  NB1_L1_DRX_LONG_CYCLE_SF32 = 32,
  NB1_L1_DRX_LONG_CYCLE_SF40 = 40,
  NB1_L1_DRX_LONG_CYCLE_SF64 = 64,
  NB1_L1_DRX_LONG_CYCLE_SF80 = 80,
  NB1_L1_DRX_LONG_CYCLE_SF128 = 128,
  NB1_L1_DRX_LONG_CYCLE_SF160 = 160,
  NB1_L1_DRX_LONG_CYCLE_SF256 = 256,
  NB1_L1_DRX_LONG_CYCLE_SF320 = 320,
  NB1_L1_DRX_LONG_CYCLE_SF512 = 512,
  NB1_L1_DRX_LONG_CYCLE_SF640 = 640,
  NB1_L1_DRX_LONG_CYCLE_SF1024 = 1024,
  NB1_L1_DRX_LONG_CYCLE_SF1280 = 1280,
  NB1_L1_DRX_LONG_CYCLE_SF2048 = 2048,
  NB1_L1_DRX_LONG_CYCLE_SF2560 = 2560
} nb1_l1_drx_long_cycle_e;


typedef enum
{
  NB1_L1_ACK_NAK_REPETITION_FACTOR_N2,
  NB1_L1_ACK_NAK_REPETITION_FACTOR_N4,
  NB1_L1_ACK_NAK_REPETITION_FACTOR_N6
}nb1_l1_ack_nak_repetition_factor_e;

typedef enum
{
  NB1_L1_TDD_ACK_NAK_FEEDBACK_MODE_BUNDLING,
  NB1_L1_TDD_ACK_NAK_FEEDBACK_MODE_MULTIPLEXING
}nb1_l1_tdd_ack_nak_feedback_mode_e;

/*!
  NPRACH Periodicity
  Periodicity of a NPRACH resource. Unit in millisecond
 */
typedef enum
{
  NB1_NPRACH_ENUMVAL_PERIODICITY_40   = 40,
  NB1_NPRACH_ENUMVAL_PERIODICITY_80   = 80,
  NB1_NPRACH_ENUMVAL_PERIODICITY_160  = 160,
  NB1_NPRACH_ENUMVAL_PERIODICITY_240  = 240,
  NB1_NPRACH_ENUMVAL_PERIODICITY_320  = 320,
  NB1_NPRACH_ENUMVAL_PERIODICITY_640  = 640,
  NB1_NPRACH_ENUMVAL_PERIODICITY_1280 = 1280,
  NB1_NPRACH_ENUMVAL_PERIODICITY_2560 = 2560
} nb1_nprach_val_periodicity_e;

/*! NPRACH Start time in ms
    Start time of the NPRACH resource in one period. Unit in millisecond
 */
typedef enum
{
  NB1_NPRACH_ENUMVAL_START_TIME_8   = 8,
  NB1_NPRACH_ENUMVAL_START_TIME_16  = 16,
  NB1_NPRACH_ENUMVAL_START_TIME_32  = 32,
  NB1_NPRACH_ENUMVAL_START_TIME_64  = 64,
  NB1_NPRACH_ENUMVAL_START_TIME_128 = 128,
  NB1_NPRACH_ENUMVAL_START_TIME_256 = 256,
  NB1_NPRACH_ENUMVAL_START_TIME_512 = 512,
  NB1_NPRACH_ENUMVAL_START_TIME_1024 = 1024
} nb1_nprach_val_start_time_e;

/*! Number of Repitition paging (nPDCCH) specified in
  36.331 Section 6.3.2
   PP2 corresponds to 2 PDCCH periods
 */
typedef enum
{
  NB1_NPRACH_ENUMVAL_RA_RSP_WIN_SIZE_PP2 = 2,
  NB1_NPRACH_ENUMVAL_RA_RSP_WIN_SIZE_PP3 = 3,
  NB1_NPRACH_ENUMVAL_RA_RSP_WIN_SIZE_PP4 = 4,
  NB1_NPRACH_ENUMVAL_RA_RSP_WIN_SIZE_PP5 = 5,
  NB1_NPRACH_ENUMVAL_RA_RSP_WIN_SIZE_PP6 = 6,
  NB1_NPRACH_ENUMVAL_RA_RSP_WIN_SIZE_PP7 = 7,
  NB1_NPRACH_ENUMVAL_RA_RSP_WIN_SIZE_PP8 = 8,
  NB1_NPRACH_ENUMVAL_RA_RSP_WIN_SIZE_PP10 = 10
} nb1_nprach_val_ra_rsp_win_size_e;

/*!
  nprach-SubcarrierOffset-r13 ENUMERATED {n0, n12, n24, n36, n2, n18, n34, spare1}
  Mapped to values
*/
typedef enum
{
  NB1_NPRACH_ENUMVAL_SUB_CARR_OFFSET_0 =  0,
  NB1_NPRACH_ENUMVAL_SUB_CARR_OFFSET_12 = 12,
  NB1_NPRACH_ENUMVAL_SUB_CARR_OFFSET_24 = 24,
  NB1_NPRACH_ENUMVAL_SUB_CARR_OFFSET_36 = 36,
  NB1_NPRACH_ENUMVAL_SUB_CARR_OFFSET_2 =  2,
  NB1_NPRACH_ENUMVAL_SUB_CARR_OFFSET_18 = 18,
  NB1_NPRACH_ENUMVAL_SUB_CARR_OFFSET_34 = 34
} nb1_nprach_val_sub_carr_offset_e;

/*!
  NumSubcarriers-r13 ENUMERATED {n12, n24, n36, n48}
  Mapped to values
*/
typedef enum
{
  NB1_NPRACH_ENUMVAL_NUM_SUB_CARR_12 = 12,
  NB1_NPRACH_ENUMVAL_NUM_SUB_CARR_24 = 24,
  NB1_NPRACH_ENUMVAL_NUM_SUB_CARR_36 = 36,
  NB1_NPRACH_ENUMVAL_NUM_SUB_CARR_48 = 48
} nb1_nprach_val_num_sub_carr_e;

/*!
  maxNumPreambleAttemptCE-r13       ENUMERATED {n3, n4, n5, n6, n7, n8, n10, spare1}
  Mapped to values
*/
typedef enum
{
  NB1_NPRACH_ENUMVAL_MAX_NUM_PREAMBLE_ATTEMPT_3 = 3,
  NB1_NPRACH_ENUMVAL_MAX_NUM_PREAMBLE_ATTEMPT_4 = 4,
  NB1_NPRACH_ENUMVAL_MAX_NUM_PREAMBLE_ATTEMPT_5 = 5,
  NB1_NPRACH_ENUMVAL_MAX_NUM_PREAMBLE_ATTEMPT_6 = 6,
  NB1_NPRACH_ENUMVAL_MAX_NUM_PREAMBLE_ATTEMPT_7 = 7,
  NB1_NPRACH_ENUMVAL_MAX_NUM_PREAMBLE_ATTEMPT_8 = 8,
  NB1_NPRACH_ENUMVAL_MAX_NUM_PREAMBLE_ATTEMPT_10 =10
} nb1_nprach_val_max_num_preamble_attempt_e;

/*!
  Number of repetitions Per Preamble Attempt
  Number of NPRACH repetitions per attempt for each NPRACH resource
 */
typedef enum
{
  NB1_NPRACH_ENUMVAL_NUM_REP_PER_PREAMBLE_ATTEMPT_1 = 1,
  NB1_NPRACH_ENUMVAL_NUM_REP_PER_PREAMBLE_ATTEMPT_2 = 2,
  NB1_NPRACH_ENUMVAL_NUM_REP_PER_PREAMBLE_ATTEMPT_4 = 4,
  NB1_NPRACH_ENUMVAL_NUM_REP_PER_PREAMBLE_ATTEMPT_8 = 8,
  NB1_NPRACH_ENUMVAL_NUM_REP_PER_PREAMBLE_ATTEMPT_16 =16,
  NB1_NPRACH_ENUMVAL_NUM_REP_PER_PREAMBLE_ATTEMPT_32 =32,
  NB1_NPRACH_ENUMVAL_NUM_REP_PER_PREAMBLE_ATTEMPT_64 = 64,
  NB1_NPRACH_ENUMVAL_NUM_REP_PER_PREAMBLE_ATTEMPT_128 = 128
} nb1_nprach_val_num_rep_per_preamble_attempt_e;


/*!
  NPDCCH Number of repetitions
  Maximum number of repetitions for NPDCCH common search space (CSS)
  for RAR, Msg3 retransmission and Msg4
 */
typedef enum
{
  NB1_NPRACH_ENUMVAL_NPDCCH_NUM_REP_1 = 1,
  NB1_NPRACH_ENUMVAL_NPDCCH_NUM_REP_2 = 2,
  NB1_NPRACH_ENUMVAL_NPDCCH_NUM_REP_4 = 4,
  NB1_NPRACH_ENUMVAL_NPDCCH_NUM_REP_8 = 8,
  NB1_NPRACH_ENUMVAL_NPDCCH_NUM_REP_16 = 16,
  NB1_NPRACH_ENUMVAL_NPDCCH_NUM_REP_32 = 32,
  NB1_NPRACH_ENUMVAL_NPDCCH_NUM_REP_64 = 64,
  NB1_NPRACH_ENUMVAL_NPDCCH_NUM_REP_128 = 128,
  NB1_NPRACH_ENUMVAL_NPDCCH_NUM_REP_256 = 256,
  NB1_NPRACH_ENUMVAL_NPDCCH_NUM_REP_512 = 512,
  NB1_NPRACH_ENUMVAL_NPDCCH_NUM_REP_1024 = 1024 ,
  NB1_NPRACH_ENUMVAL_NPDCCH_NUM_REP_2048 = 2048
} nb1_nprach_val_npdcch_num_rep_e;


/*!
  nprach-SubcarrierOffset-r13 ENUMERATED {n0, n12, n24, n36, n2, n18, n34, spare1}
  Mapped to values
*/
#define NB1_NPRACH_ENUMVAL_NPDCCH_START_SF_CSS_1DOT5 1.5

typedef enum
{
  NB1_NPRACH_ENUMVAL_NPDCCH_START_SF_CSS_2 = 2,
  NB1_NPRACH_ENUMVAL_NPDCCH_START_SF_CSS_4 = 4,
  NB1_NPRACH_ENUMVAL_NPDCCH_START_SF_CSS_8 = 8,
  NB1_NPRACH_ENUMVAL_NPDCCH_START_SF_CSS_16 = 16,
  NB1_NPRACH_ENUMVAL_NPDCCH_START_SF_CSS_32 = 32,
  NB1_NPRACH_ENUMVAL_NPDCCH_START_SF_CSS_48 = 48,
  NB1_NPRACH_ENUMVAL_NPDCCH_START_SF_CSS_64 = 64
} nb1_nprach_val_npdcch_start_sf_css_e;

typedef enum
{
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_8  = 8,
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_10 = 10,
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_11 = 11,
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_12 = 12,
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_20 = 20,
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_22 = 22,
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_23 = 23,
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_24 = 24,
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_32 = 32,
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_34 = 34,
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_35 = 35,
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_36 = 36,
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_40 = 40,
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_44 = 44,
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_46 = 46,
  NB1_NPRACH_ENUMVAL_CBRA_START_SUB_CARR_48 = 48
} nb1_nprach_val_num_cbra_start_sub_carr_e;

typedef enum
{
  NB1_RRC_ENUMVAL_PROBABILITY_ZERO = 0,
  NB1_RRC_ENUMVAL_PROBABILITY_ONESIXTEENTH = 62,
  NB1_RRC_ENUMVAL_PROBABILITY_ONEFIFTEENTH = 66,
  NB1_RRC_ENUMVAL_PROBABILITY_ONEFOURTEENTH = 71,
  NB1_RRC_ENUMVAL_PROBABILITY_ONETHIRTEENTH = 76,
  NB1_RRC_ENUMVAL_PROBABILITY_ONETWELFTH = 83,
  NB1_RRC_ENUMVAL_PROBABILITY_ONEELEVENTH = 90,
  NB1_RRC_ENUMVAL_PROBABILITY_ONETENTH = 100,
  NB1_RRC_ENUMVAL_PROBABILITY_ONENINTH = 111,
  NB1_RRC_ENUMVAL_PROBABILITY_ONEEIGTH = 125,
  NB1_RRC_ENUMVAL_PROBABILITY_ONESEVENTH = 142,
  NB1_RRC_ENUMVAL_PROBABILITY_ONESIXTH = 166,
  NB1_RRC_ENUMVAL_PROBABILITY_ONEFIFTH = 200,
  NB1_RRC_ENUMVAL_PROBABILITY_ONEFOURTH = 250,
  NB1_RRC_ENUMVAL_PROBABILITY_ONETHIRD = 333,
  NB1_RRC_ENUMVAL_PROBABILITY_ONEHALF = 500,
  NB1_RRC_ENUMVAL_PROBABILITY_ONE = 1000
}nb1_rrc_prach_value_anchor_probability_e;

/*===========================================================================

                    PRACH PARAMETER TYPES

===========================================================================*/
/*! @brief
  Defined in TBD
  TBD fill in this struct
*/
typedef struct
{
  nb1_l1_TBD_t params_to_be_filled_in;
} nb1_l1_rach_cfg_s;


/*===========================================================================

                     MBSFN related TYPES

===========================================================================*/

typedef enum
{
  NB1_L1_MBSFN_ALLOCATION_ONE = 0,
  NB1_L1_MBSFN_ALLOCATION_FOUR
} nb1_l1_mbsfn_allocation_e;

typedef enum
{
  NB1_L1_MBSFN_ALLOCATION_PERIOD_1 = 0,
  NB1_L1_MBSFN_ALLOCATION_PERIOD_2,
  NB1_L1_MBSFN_ALLOCATION_PERIOD_4,
  NB1_L1_MBSFN_ALLOCATION_PERIOD_8,
  NB1_L1_MBSFN_ALLOCATION_PERIOD_16,
  NB1_L1_MBSFN_ALLOCATION_PERIOD_32
} nb1_l1_mbsfn_allocation_period_e;

/*! @brief
    Enumeration defines non-mbsfn region length in MBSFN subframe
*/
typedef enum
{
  NB1_L1_NON_MBSFN_SYMBOL_ONE = 1,
  NB1_L1_NON_MBSFN_SYMBOL_TWO = 2
} nb1_l1_non_mbsfn_region_length_e;

/*! @brief
    Enumeration defines MCCH repition period in radio frames
*/
typedef enum
{
  NB1_L1_MBSFN_MCCH_REP_PERIOD_32RF = 32,
  NB1_L1_MBSFN_MCCH_REP_PERIOD_64RF = 64,
  NB1_L1_MBSFN_MCCH_REP_PERIOD_128RF = 128,
  NB1_L1_MBSFN_MCCH_REP_PERIOD_256RF = 256
} nb1_l1_mbsfn_mcch_rep_period_e;

/*! @brief
    Enumeration defines MCCH modification period in radio frames
*/
typedef enum
{
  NB1_L1_MBSFN_MCCH_MOD_PERIOD_512RF = 512,
  NB1_L1_MBSFN_MCCH_MOD_PERIOD_1024RF = 1024
} nb1_l1_mbsfn_mcch_mod_period_e;

/*! @brief
    Enumeration defines signalling MCS: Indicates the Modulation and Coding Scheme (MCS)
    applicable for the subframes indicated by the field sf-AllocInfo and for the first
    subframe of each MCH scheduling period (which may contain the MCH scheduling
    information provided by MAC). Value n2 corresponds with the value 2 for parameter
    in TS 36.213 [23, Table 7.1.7.1-1], and so on.
*/
typedef enum
{
  NB1_L1_MBSFN_SIGNALLING_MCS_N2  = 2,
  NB1_L1_MBSFN_SIGNALLING_MCS_N7  = 7,
  NB1_L1_MBSFN_SIGNALLING_MCS_N13 = 13,
  NB1_L1_MBSFN_SIGNALLING_MCS_N19 = 19
} nb1_l1_mbsfn_signalling_mcs_e;

/*! @brief
    Enumeration defines mcch change notification coefficient.
    Actual change notification repetition period common for all MCCHs
    that are configured = shortest modification period/ notificationRepetitionCoeff.
    The 'shortest modificaton period' corresponds with the lowest value of
    mcch-ModificationPeriod of all MCCHs that are configured.
    Value n2 corresponds to coefficient 2, and so on.
*/
typedef enum
{
  NB1_L1_MBSFN_NOTIFICATION_COEFF_N2 = 2,
  NB1_L1_MBSFN_NOTIFICATION_COEFF_N4 = 4
} nb1_l1_mbsfn_notification_coeff_e;

/*! @brief
    Enumeration defines actual change notification repetition period
    common for all MCCHs that are configured
    = shortest modification period/ notificationRepetitionCoeff.
    The 'shortest modificaton period' corresponds with the lowest value of
    mcch-ModificationPeriod of all MCCHs that are configured.
*/
typedef enum
{
  NB1_L1_MBSFN_NOTIFICATION_PERIOD_128RF = 128,
  NB1_L1_MBSFN_NOTIFICATION_PERIOD_256RF = 256,
  NB1_L1_MBSFN_NOTIFICATION_PERIOD_512RF = 512
} nb1_l1_mbsfn_notification_period_e;

/*! @brief
    Enumeration defines MBSFN area common subframe allocation period in radio frames
*/
typedef enum
{
  NB1_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_4RF = 4,
  NB1_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_8RF = 8,
  NB1_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_16RF = 16,
  NB1_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_32RF = 32,
  NB1_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_64RF = 64,
  NB1_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_128RF = 128,
  NB1_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_256RF = 256
} nb1_l1_mbsfn_common_sf_allocation_period_e;

/*! @brief
    Enumeration defines MCH scheduling period in radio frames
*/
typedef enum
{
  NB1_L1_MBSFN_MCH_SCHDL_PERIOD_8RF = 8,
  NB1_L1_MBSFN_MCH_SCHDL_PERIOD_16RF = 16,
  NB1_L1_MBSFN_MCH_SCHDL_PERIOD_32RF = 32,
  NB1_L1_MBSFN_MCH_SCHDL_PERIOD_64RF = 64,
  NB1_L1_MBSFN_MCH_SCHDL_PERIOD_128RF = 128,
  NB1_L1_MBSFN_MCH_SCHDL_PERIOD_256RF = 256,
  NB1_L1_MBSFN_MCH_SCHDL_PERIOD_512RF = 512,
  NB1_L1_MBSFN_MCH_SCHDL_PERIOD_1024RF = 1024
} nb1_l1_mbsfn_mch_schdl_period_e;

/*! @brief The mbsfn configuration: subframes that are reserved for MBSFN in
    downlink
*/
typedef struct
{

  /*! The mbsfn allocation offset */
  uint8                            allocation_offset;
  /*! The mbsfn allocation periodicity */
  nb1_l1_mbsfn_allocation_period_e allocation_period;

  /*! The number of mbsfn subframes allocated every repetition. */
  nb1_l1_mbsfn_allocation_e        sfn_allocation;

  /*!
  Radio-frames that contain MBSFN subframes occur when equation SFN mod
  radioFrameAllocationPeriod = radioFrameAllocationOffset is satisfied.
  n1 denotes value 1, n2 denotes value 2, and so on. When fourFrames is used,
  the equation defines the first radio frame referred to in the description
  below. Values n1 and n2 are not applicable when fourFrames is used.


   Bit mask identifying which subframes are mbsfn subframes
   the lower 6 bits are used when there is a 1 subframe allocation,
   the lower 24 bits are used if there is a 4 frame allocation.

   The bits are allocated as follows:

   case NB1_L1_MBSFN_ALLOCATION_ONE:

   FDD: The first/leftmost bit defines the MBSFN allocation for subframe #1,
   the second bit for #2, third bit for #3 , fourth bit for #6, fifth bit
   for #7, sixth bit for #8.
   TDD: The first/leftmost bit defines the allocation for subframe #3, the
   second bit for #4, third bit for #7, fourth bit for #8, fifth bit for
   #9. Uplink subframes are not allocated. The last bit is not used.

   case NB1_L1_MBSFN_ALLOCATION_FOUR:

   FDD: Starting from the first radioframe and from the first/leftmost bit
   in the bitmap, the allocation applies to subframes  #1, #2, #3 , #6, #7,
   and #8 in the sequence  of the four radio-frames.
   TDD: Starting from the first radioframe and from the first/leftmost bit
   in the bitmap, the allocation applies to subframes  #3, #4, #7, #8, and
   #9 in the sequence  of the four radio-frames. The last four bits are not
   used. Uplink subframes are not allocated.

   */
  uint32 sfn_allocation_mask;


} nb1_l1_mbsfn_allocation_s;

/*! @brief MBSFN area MCCH schedule config
*/
typedef struct
{
  /*! Defines the interval between transmissions of MCCH information,
      in radio frames, Value rf32 corresponds to 32 radio frames,
      rf64 corresponds to 64 radio frames and so on.
  */
  nb1_l1_mbsfn_mcch_rep_period_e     rep_period;

  /*! MCCH offset (0 -10): Indicates, together with the mcch-RepetitionPeriod,
     the radio frames in which MCCH is scheduled i.e. MCCH is scheduled in
     radio frames for which: SFN mod mcch-RepetitionPeriod = mcch-Offset
  */
  uint8                              offset;

  /*! Defines periodically appearing boundaries, i.e. radio frames for which
      SFN mod mcch-ModificationPeriod = 0, The contents of different transmissions
      of MCCH information can only be different if there is at least one
      such boundary in-between them.
  */
  nb1_l1_mbsfn_mcch_mod_period_e     mod_period;

  /*! Indicates the subframes of the radio frames indicated by the mcch-RepetitionPeriod
      and the mcch-Offset, that may carry MCCH. Value "1" indicates that the corresponding
      subframe is allocated.
      The following mapping applies:
      FDD: The first/ leftmost bit defines the allocation for subframe #1 of the radio frame
      indicated by mcch-RepetitionPeriod and mcch-Offset, the second bit for #2, the third bit for #3 ,
      the fourth bit for #6, the fifth bit for #7 and the sixth bit for #8.

      TDD: The first/leftmost bit defines the allocation for subframe #3 of the radio frame
      indicated by mcch-RepetitionPeriod and mcch-Offset, the second bit for #4, third bit for #7,
      fourth bit for #8, fifth bit for #9. Uplink subframes are not allocated. The last bit is not used.
  */
  uint8                              sf_alloc_info_bitmask;

  /*! Signalling MCS */
  nb1_l1_mbsfn_signalling_mcs_e      mcs;

} nb1_l1_mbsfn_mcch_cfg_s;


/*! @brief MBSFN area PMCH schedule config ( PMCH allocation)
*/
typedef struct
{
  /* PMCH unique identifier: MBSFN area ID + Unique PMCH ID allocate by RRC */
  /*! PMCH ID:  Only used by higher layers TB-PMCH identification mapping [0 - 15]

      Unique PMCH id allocated by RRC and passed from ML1->LL1
      and LL1 can add it in meta data info to upper layers
      0 = Unknown/Reserved (used by ML1 during MCCH decode where PMCHs config is not known)
  */
  nb1_pmch_id_t                   pmch_id;

  /*! sf alloc start [0, 1535]: Indicates the first subframe allocated to this (P)MCH within a period
      identified by field commonSF-AllocPeriod. The subframes allocated to (P)MCH corresponding
      with the nth entry in pmch-InfoList are the subsequent subframes starting from either the
      subframe identified by sf-AllocEnd of the (n-1)th listed (P)MCH or, for n=1, the first subframe,
      through the subframe identified by sf-AllocEnd of the nth listed (P)MCH.
      Value 0 corresponds with the first subframe defined by field commonSF-Alloc.

      sf_alloc_start <= sf_alloc_end
  */
  uint16                          sf_alloc_start;

  /*! sf alloc end [0, 1535]: Indicates the last subframe allocated to this (P)MCH within a period
      identified by field commonSF-AllocPeriod. The subframes allocated to (P)MCH corresponding
      with the nth entry in pmch-InfoList are the subsequent subframes starting from either the
      subframe identified by sf-AllocEnd of the (n-1)th listed (P)MCH or, for n=1, the first subframe,
      through the subframe identified by sf-AllocEnd of the nth listed (P)MCH.
      Value 0 corresponds with the first subframe defined by field commonSF-Alloc.
  */
  uint16                          sf_alloc_end;
  /*! Data MCS [0, 28]: Indicates the value for parameter  in TS 36.213 [23, Table 7.1.7.1-1],
      which defines the Modulation and Coding Scheme (MCS) applicable for the subframes
      of this (P)MCH as indicated by the field commonSF-Alloc.
      The MCS does however neither apply to the subframes that may carry MCCH
      i.e. the subframes indicated by the field sf-AllocInfo within SystemInformationBlockType13
      nor for the first subframe of each MCH scheduling period
      (which may contain the MCH scheduling information provided by MAC).
  */
  uint8                            mcs;

  /*! MCH scheduling period: Indicates the MCH scheduling period i.e. the periodicity used for
      providing MCH scheduling information at lower layers (MAC) applicable for an MCH.
      Value rf8 corresponds to 8 radio frames, rf16 corresponds to 16 radio frames and so on
      i.e. radio frames for which SFN mod mch-SchedlingPeriod = 0.
  */
  nb1_l1_mbsfn_mch_schdl_period_e  schdl_period;

} nb1_l1_mbsfn_pmch_cfg_s;

/*! @brief MSI MTCHs schedule info
*/
typedef struct
{
  /*! Whether UE is interested in monitoring this scheduled MTCH or not
      N/A action: NB1_CPHY_MBSFN_AREA_MONITOR_ONLY_MODIFICATION_BOUNDARY
  */
  nb1_cphy_mbsfn_area_monitor_pattern_e  monitor_action;
  /*! UE interested MTCH/PMCH ordinal subframe info
      First MTCH in OTA MAC control element start ordinal subframe  = 1st
      Nth MTCH in OTA MAC control element start ordinal subframe =
      Scheduled previous MTCH (may be N-1) stop ordinal subframe
      Nth MTCH end ordinal subframe = MTCH stop field value of this MTCH
  */
  uint16                                 stop_ordinal_subfn;
  /*! Logical channel id */
  uint8                                  lcid;

} nb1_l1_mbsfn_msi_ordinal_subfn_info_s;

/*! @brief MCH scheduling information (MSI)
*/
typedef struct
{
  /*! MBSFN area id: 0 - 255 */
  nb1_mbsfn_area_id_t                       area_id;

  /*! PMCH ID:  Only used by higher layers TB-PMCH identification mapping [0 - 15]

      Unique PMCH id allocated by RRC and passed from ML1->LL1
      and LL1 can add it in meta data info to upper layers
      0 = Unknown/Reserved (used by ML1 during MCCH decode where PMCHs config is not known)
  */
  nb1_pmch_id_t                             pmch_id;

  /*! MSI OTA SFN */
  nb1_l1_sfn_t                              sfn;

  /*! MSI OTA Subfn */
  nb1_l1_subfn_t                            subfn;

  /*! Network scheduled all MTCH/PMCH ordinal subframe info in order
      First MTCH in OTA MAC control element start ordinal subframe  = 1st
      Nth MTCH in OTA MAC control element start ordinal subframe =
      Scheduled previous MTCH (may be N-1) stop ordinal subframe
  */
  nb1_l1_mbsfn_msi_ordinal_subfn_info_s     monitor_pmch_info[NB1_L1_MAX_MTCH_PER_PMCH];

} nb1_l1_mbsfn_msi_cfg_s;


/******************************************************************************
                      UE Timers and Constants Parameter Types
*******************************************************************************/

/*! @brief Enumeration of t310 values
*/
typedef enum
{
    NB1_CPHY_T310_0 = 0,
    NB1_CPHY_T310_200,
    NB1_CPHY_T310_500,
    NB1_CPHY_T310_1000,
    NB1_CPHY_T310_2000,
    NB1_CPHY_T310_4000,
    NB1_CPHY_T310_8000
} nb1_cphy_t310_e;

/*! @brief Enumeration of n310 values
*/
typedef enum
{
    NB1_CPHY_N310_1 = 1,      /*!<  1 */
    NB1_CPHY_N310_2 = 2,      /*!< 2 */
    NB1_CPHY_N310_3 = 3,      /*!< 3 */
    NB1_CPHY_N310_4 = 4,      /*!< 4 */
    NB1_CPHY_N310_6 = 6,      /*!< 6 */
    NB1_CPHY_N310_8 = 8,     /*!< 8 */
    NB1_CPHY_N310_10 = 10,    /*!< 10 */
    NB1_CPHY_N310_20 = 20   /*!< 20 */

} nb1_cphy_n310_e;

/*! @brief Enumeration of n311 values
*/
typedef enum
{
    NB1_CPHY_N311_1 = 1,      /*!<  1 */
    NB1_CPHY_N311_2 = 2,      /*!< 2 */
    NB1_CPHY_N311_3 = 3,      /*!< 3 */
    NB1_CPHY_N311_4 = 4,      /*!< 4 */
    NB1_CPHY_N311_5 = 5,      /*!< 5 */
    NB1_CPHY_N311_6 = 6,      /*!< 6 */
    NB1_CPHY_N311_8 = 8,     /*!< 8 */
    NB1_CPHY_N311_10 = 10    /*!< 10 */

} nb1_cphy_n311_e;

/*! @brief Enumeration of OOS reason
*/
typedef enum
{
    NB1_CPHY_OOS_REASON_NORMAL = 1,      /*!<  1 */
    NB1_CPHY_OOS_REASON_RESET = 2,      /*!< 2 */

    NB1_CPHY_OOS_REASON_TMR   = 3,       /*!< 3 */
    NB1_CPHY_OOS_REASON_RESET_1   = 4,       /*!< 4 */
    NB1_CPHY_OOS_REASON_RESET_2   = 5,       /*!< 5 */
    NB1_CPHY_OOS_REASON_RESET_3   = 6,       /*!< 6 */
    NB1_CPHY_OOS_REASON_RESET_4   = 7,       /*!< 7 */
    NB1_CPHY_OOS_REASON_RESET_5   = 8,       /*!< 8 */
    NB1_CPHY_OOS_REASON_RF_UNAVAILABLE = 9,   /*!< 9 */
    NB1_CPHY_OOS_REASON_RSRP_CAMPING = 10   /*!< 10 */
    /*If a new OOS indication is being added (non FW recovery indication related)
         then please update cause in nb1_rrc_csp_sm_camped_oos_ind handler
         wherein RRC should ignore Non FW recovery indication in SUSPENDED state*/
} nb1_l1_oos_reason_e;

/*! @brief Enumeration of RLF reason
*/
typedef enum
{
    NB1_CPHY_RLF_REASON_NORMAL = 1,      /*!<  1 */
    NB1_CPHY_RLF_REASON_RESET = 2,      /*!< 2 */
    NB1_CPHY_RLF_REASON_TMR = 3,        /*!< 3 */
    NB1_CPHY_RLF_REASON_RESET_1   = 4,       /*!< 4 */
    NB1_CPHY_RLF_REASON_RESET_2   = 5,       /*!< 5 */
    NB1_CPHY_RLF_REASON_RESET_3   = 6,       /*!< 6 */
    NB1_CPHY_RLF_REASON_RESET_4   = 7,       /*!< 7 */
    NB1_CPHY_RLF_REASON_RESET_5   = 8,       /*!< 8 */
    NB1_CPHY_RLF_REASON_RF_UNAVAILABLE   = 9,       /*!< 9 */
    NB1_CPHY_RLF_REASON_CDRX_COLLISION   = 10,       /*!< 10 */
    NB1_CPHY_RLF_REASON_MSG4_ACK_NOT_SENT   = 11       /*!< 11 */

} nb1_l1_rlf_reason_e;

/*! @Brief GSM cell Cell ID structure */
typedef struct
{
  uint8 ncc;
  uint8 bcc;
} nb1_l1_gsm_cell_id_s;

/*! ARFCN type used to specify a GSM frequency.  This should match
  with the type from the GSM interface, but has been placed here to remove
  the dependancy. */
typedef uint16 nb1_l1_gsm_arfcn_t;

typedef enum
{
  NB1_L1_GSM_FIRST_BAND,
  NB1_L1_GSM_BAND_EGSM_900 = NB1_L1_GSM_FIRST_BAND,
  NB1_L1_GSM_BAND_PGSM_900,
  NB1_L1_GSM_BAND_CELL_850,
  NB1_L1_GSM_BAND_DCS1800,
  NB1_L1_GSM_BAND_PCS1900,
  NB1_L1_GSM_NUM_BANDS
} nb1_l1_gsm_band_e;

/*! RSSI measurement quantity type.  Quantity is in dBx16 or 1 unit
    represents 1/16 of a dB
 */
typedef int32 nb1_l1_gsm_rssi_t;

/*! GSM quarter symbol value */
typedef uint32 nb1_l1_gsm_qs_type_t;

/*! Complete GSM ARFCN type which unambiguously identifies the GSM frequency
  by using a band indicator
  */
typedef struct
{
  /*! ARFCN number of the GSM cell */
  nb1_l1_gsm_arfcn_t arfcn;
  /*! Band that the GSM cell resides on. */
  nb1_l1_gsm_band_e band;

} nb1_l1_gsm_complete_arfcn_s;


/*! @brief GSM frame number */
typedef struct
{
  /*! Tie breaking number used to simply handle wraparound cases.  Should
    be ignored in most cases and is used internally.
    */
  uint32 hyperframe:8;
  /*! The frame of the gfn */
  uint32 frame:24;
} nb1_l1_gfn_s;

/*! @brief Quarter Symbol accurate GSM timing offset */
typedef struct
{
  /*! The GFN Frame/Superframe number */
  nb1_l1_gfn_s gfn;
  /*! The quarter symbol offset in the frame */
  nb1_l1_gsm_qs_type_t qs_offset;
} nb1_l1_gsm_timing_s;

/*! @brief Transmission type enum */
typedef enum
{
  NB1_L1_TX_TYPE_NEW,
  NB1_L1_TX_TYPE_RETX,
  NB1_L1_TX_TYPE_DTX
} nb1_l1_tx_type_e;

/*===========================================================================
                     Time Transfer related TYPES
===========================================================================*/
/*! WCDMA STMR structure */
typedef struct
{
  uint32 frame_count;
  uint32 chip_x8_offset;
} nb1_l1_stmr_w_s;

/*! TDS STMR structure */
typedef struct
{
  uint16 subframe;
  uint16 cx8_offset;
} nb1_l1_stmr_t_s;

/*! GSM STMR structure */
typedef struct
{
  uint32 frame;
  uint16 qs_offset;
  uint16 raw_13M;
  uint32 prescale; /* Deprecated */
} nb1_l1_stmr_g_s;

/*! HDR STMR structure */
typedef struct
{
  uint32 cdma_cx8;
} nb1_l1_stmr_h_s;

/*! CDMA 1x STMR structure */
typedef struct
{
  uint32 cdma_cx8;
} nb1_l1_stmr_c_s;

/*! NB1 STMR structure */
typedef struct
{
  uint64 subframe_ref_time; /*!< RTC value */
  uint32 o_stmr;            /*!< OSTMR value */
  uint32 o_stmr_phase;      /*!< OSTMR phase */
  uint32 snapshot_offset;   /*!< Offset from SF boundary */
  uint32 sfn;               /*!< NB1 system frame number */
  uint32 subfn;             /*!< NB1 subframe number */
  uint32 ustmr;             /*!< XO count */
} nb1_l1_stmr_l_s;

/*! Union of other RAT STMR types */
typedef union
{
  nb1_l1_stmr_w_s w; /* WCDMA STMR */
  nb1_l1_stmr_t_s t; /* TDS STMR */
  nb1_l1_stmr_g_s g; /* GSM STMR */
  nb1_l1_stmr_c_s c; /* CDMA 1x STMR */
  nb1_l1_stmr_h_s h; /* HDR STMR */
} nb1_l1_stmr_u;

/*! ORAT STMR snapshot */
typedef struct
{
  nb1_rat_e rat;
  nb1_l1_stmr_u stmr;
} nb1_l1_orat_stmr_snapshot_s;

/*! Snapshot of STMR values for NB1 and another RAT */
typedef struct
{
  nb1_l1_stmr_l_s l; /*!< NB1 STMR value */
  nb1_l1_orat_stmr_snapshot_s orat; /*!< ORAT STMR value */
} nb1_l1_nb1_stmr_snapshot_s;

/*! @brief  CDRX Collision parameters (configured via EFS)
*/
typedef struct
{
  /*!<  CDRX Number of RLF Threshold configured via EFS (to be used in ML1)
             Count to indicate TRM Rejects during CDRX*/
  uint8 num_rlf_thresh;

  /*!<  CDRX Number of Reestabishment Threshold configured via EFS (to be used in RRC)
             Count to indicate Reestablishment attempts for
               RLF due to TRM rejects during CDRX*/
  uint8 num_reest_thresh;
}nb1_l1_cdrx_collision_params_s;


//! NB1 Specific types
/* Frame type for NB1FW - [0,1023] 10 bits */
typedef uint16 nb1_fw_sfn_t;
/* Subframe type for NB1FW - [0,9] 4 bits */
typedef uint16 nb1_fw_subfn_t;

#endif /*  NB1_L1_TYPES_H */
