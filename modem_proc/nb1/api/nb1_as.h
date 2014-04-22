/*!
  @file
  nb1_as.h

  @brief
  This file contains definitions for the Common type and constants required
  by different modules within NB1 Access Stratum(AS)

*/

/*=============================================================================

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_as.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/19/17   ss      CR 2034257 : Invalid LCID so valid LCID extended to MAX 11

=============================================================================*/

#ifndef NB1_AS_H
#define NB1_AS_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/

#include <comdef.h>
#include <nb1_ind_msg.h>

/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/

/*! @brief
    This Macro provides the compiler with branch prediction information.

    Calling NB1_LIKELY_TRUE(expr) informs the compiler that it is expected that
    expr == 1 <NOTE: expr has to be equal to 1 not greater than 1>.

    Calling NB1_UNLIKELY_TRUE(expr) informs the compiler that it is expected
    that expr is going to be 0, hence dont perform prefetching of instructions
    inside.

    @usage
    Use "if (NB1_LIKELY_TRUE(a==b))" {} if expression '(a==b)' is expected to be
    TRUE most of the time else use "if (NB1_UNLIKELY(a==b)"
*/
#if defined(__GNUC__) && !defined(_lint)
  #define NB1_LIKELY_TRUE(x) __builtin_expect((x), 1)
  #define NB1_UNLIKELY_TRUE(x) __builtin_expect((x), 0)
#else
  #define NB1_LIKELY_TRUE(x) (x)
  #define NB1_UNLIKELY_TRUE(x) (x)
#endif /* ! (__GNUC__) */


/*! @brief Maximum number of N PRACH resources
*/
#define NB1_MAX_NPRACH_RESOURCE          3


/*! @brief NB1 3GPP Release supported by the UE
*/
#define NB1_3GPP_REL 8
/*! @brief NB1 3GPP Release Month
*/
#define NB1_3GPP_REL_MONTH 9
/*! @brief NB1 3GPP Release Year
*/
#define NB1_3GPP_REL_YEAR 2008

/*! @brief Maximum Number of Radio Bearers, this only includes Unicast RBs
*/
#define NB1_MAX_RB 32

/*! @brief Maximum Number of Logical Channels, this only includes Unicast LCs
*/
#define NB1_MAX_LC 32

/*!< @brief Maximim Number of MBSFN Areas */

#define NB1_EMBMS_MAX_MBSFN_AREA 8

/*!< @brief Default MCCH RB ID */
#define NB1_EMBMS_MCCH_RB_ID 35

/*! @brief macro for Min RB id of MTCH
*/
#define NB1_MIN_MTCH_RB_ID (NB1_EMBMS_MCCH_RB_ID + 1)

/*! @brief macro for Max RB id of MTCH
*/
#define NB1_MAX_MTCH_RB_ID (NB1_MIN_MTCH_RB_ID + (NB1_EMBMS_MAX_ACTIVE_SESSIONS - 1))

/*! @brief Maximum Number of Multicast Logical Channels
  For now, assume 32 MRBs (32 MTCHs)
*/
#define NB1_MAX_ACTIVE_MRB 32

/*! @brief Maximum Number of de prioritized frequency
*/
#define NB1_MAX_NUM_DEPRI_FREQ 64

/*! @brief Maximum Number of Multicast Logical Channels
  For now, assume 8 MTCHs
*/
#define NB1_MAX_EMBMS_LC NB1_MAX_ACTIVE_MRB

/*! @brief Maximum Number of active Signalling Radio Bearers supported in UE
    TBD: Need to confirm (shall we consider only AM/UM SRBs here?)
*/
#define NB1_MAX_ACTIVE_SRB 3

/*! @brief Maximum Number of active Data Radio Bearers supported in UE
    TBD: Need to change the value of this macro as per decision in 3gpp
*/
#define NB1_MAX_ACTIVE_DRB 8

/*! @brief Maximum Number of Active Radio Bearers (SRB + DRB)
*/
#define NB1_MAX_ACTIVE_RB (NB1_MAX_ACTIVE_SRB + NB1_MAX_ACTIVE_DRB)

/*! @brief Maximum Number of Active Logical Channels (over SRB and DRB)
    Note: Logical channels have one to one mapping with radio bearers
*/
#define NB1_MAX_ACTIVE_LC NB1_MAX_ACTIVE_RB

/*! @brief Maximum Number of Radio Bearer Configuration Index for defining
    array for direct hashing
*/
#define NB1_MAX_RB_CFG_IDX (NB1_MAX_RB + NB1_MAX_ACTIVE_SRB + 1)

#define NB1_RB_CFG_IDX_SRB1_Bis 35

/*! @brief Maximum Number of RLC PDUs that can be received in
    a given transport block (MAC PDU)
*/
#define NB1_MAX_RLC_PDU_PER_TR_BLK 10

/*! @brief Define invalid RA ID
*/
#define NB1_INVALID_RA_ID            0xFF

/*! @brief Define invalid RA ID MASK
*/
#define NB1_INVALID_RA_ID_MASK       0xFF

/*! @brief used to indicate that OTA IE has infinity value.  used for uint16
*/
#define NB1_OTA_IE_INFINITY_UINT16  0xFFFF

/*! @brief used to indicate that OTA IE has infinity value.  used for uint32
*/
#define NB1_OTA_IE_INFINITY_UINT32  0xFFFFFFFF

/*! Default value for p_max */
#define NB1_DEFAULT_PMAX           (23)

/*! @brief unused marcro
*/
#define NB1_UNUSED(x)   NOTUSED(x)

/*=============================================================================

                   EXTERNAL ENUMERATION TYPES

=============================================================================*/


/*! @brief Radio Bearer Mode
*/
typedef enum
{
  NB1_RB_RLC_MODE_INVALID, /*!< For initialization usage */
  NB1_RB_RLC_MODE_AM,      /*!< Acknowledgement mode */
} nb1_rb_rlc_mode_e;


/*! @brief Radio Bearer type
*/
typedef enum
{
  NB1_RB_TYPE_INVALID, /*!< For initialization usage */
  NB1_RB_TYPE_SRB,     /*!< Unicast Signalling Radio Bearer */
  /*!< Unicast Data Radio Bearer */
  NB1_RB_TYPE_DRB
} nb1_rb_type_e;



/*=============================================================================

                   EXTERNAL STRUCTURE/UNION TYPES

=============================================================================*/



/*=============================================================================

                   DATA TYPES

=============================================================================*/
/*! @brief
  Physical cell ID type Range: 0..503
  Defined in 36.331 Section 6.3.4

  @see NB1_L1_MAX_PHY_CELL_ID for the maximum valid cell id value
  @see NB1_L1_NO_CELL_ID to denote that the cell id is not present
*/
typedef uint16 nb1_phy_cell_id_t;

/*! @brief This defines the radio bearer configuration index type
*/
typedef uint8 nb1_rb_cfg_idx_t;

/*! @brief This defines the Radio Bearer identity as described in 36.331 (RRC)
*/
typedef uint8 nb1_rb_id_t;

/*! @brief This defines the Logical Channel identity as described in 36.331(RRC)
*/
typedef uint8 nb1_lc_id_t;

/*! @brief This defines the MBSFN area identity as described in 36.331(RRC)
*/
typedef uint8 nb1_mbsfn_area_id_t;

/*! @brief This defines the PMCH identity as described in 36.331(RRC)
*/
typedef uint8 nb1_pmch_id_t;

/*===========================================================================

                   RF CHANNEL, BANDWIDTH RELATED TYPES

===========================================================================*/
/*! @ brief
    Number of NB1 supported bandwidths
    Defined in 36.101 Section 5.4.2.1
*/
#define  NB1_MAX_NUM_BANDWIDTHS  6

/*! @brief
  NB1 support bandwidths
  Defined in 36.101 Section 5.4.2.1
*/
typedef enum
{
  /*! 1.4MHz bandwidth */
  NB1_BW_NRB_6 = 6,
  /*! 3MHz bandwidth */
  NB1_BW_NRB_15 = 15,
  /*! 5MHz bandwidth */
  NB1_BW_NRB_25 = 25,
  /*! 10MHz bandwidth */
  NB1_BW_NRB_50 = 50,
  /*! 15MHz bandwidth */
  NB1_BW_NRB_75 = 75,
  /*! 20MHz bandwidth */
  NB1_BW_NRB_100 = 100,
  /*! Minimum supported bandwidth */
  NB1_BW_MIN = NB1_BW_NRB_6,
  /*! Maximum supported bandwidth */
  NB1_BW_MAX = NB1_BW_NRB_100,
  /*! Unknown Bandwidth */
  NB1_BW_UNKNOWN
} nb1_bandwidth_e;

/*! @brief
  NB1 supported Carrier Freq Offsets
  Offset of the NB-IoT channel number to EARFCN
   v-10 means -10, v-9 means -9, and so on.

*/
typedef enum
{
  NB1_FREQ_OFFSET_N10 = 0,
  NB1_FREQ_OFFSET_N9,
  NB1_FREQ_OFFSET_N8,
  NB1_FREQ_OFFSET_N7,
  NB1_FREQ_OFFSET_N6,
  NB1_FREQ_OFFSET_N5,
  NB1_FREQ_OFFSET_N4,
  NB1_FREQ_OFFSET_N3,
  NB1_FREQ_OFFSET_N2,
  NB1_FREQ_OFFSET_N1,
  NB1_FREQ_OFFSET_N0DOT5,
  NB1_FREQ_OFFSET_0,
  NB1_FREQ_OFFSET_1,
  NB1_FREQ_OFFSET_2,
  NB1_FREQ_OFFSET_3,
  NB1_FREQ_OFFSET_4,
  NB1_FREQ_OFFSET_5,
  NB1_FREQ_OFFSET_6,
  NB1_FREQ_OFFSET_7,
  NB1_FREQ_OFFSET_8,
  NB1_FREQ_OFFSET_9,
  NB1_FREQ_OFFSET_MAX
} nb1_freq_offset_e;

/*! @brief
  NB1 supported UL Carrier Freq Offset values
  Offset of the NB-IoT channel number to EARFCN
   v-10 means -10, v-9 means -9, and so on.
*/
typedef enum
{
  NB1_FREQ_OFFSET_VAL_N10 = -10,
  NB1_FREQ_OFFSET_VAL_N9,
  NB1_FREQ_OFFSET_VAL_N8,
  NB1_FREQ_OFFSET_VAL_N7,
  NB1_FREQ_OFFSET_VAL_N6,
  NB1_FREQ_OFFSET_VAL_N5,
  NB1_FREQ_OFFSET_VAL_N4,
  NB1_FREQ_OFFSET_VAL_N3,
  NB1_FREQ_OFFSET_VAL_N2,
  NB1_FREQ_OFFSET_VAL_N1,
  NB1_FREQ_OFFSET_VAL_0,
  NB1_FREQ_OFFSET_VAL_1,
  NB1_FREQ_OFFSET_VAL_2,
  NB1_FREQ_OFFSET_VAL_3,
  NB1_FREQ_OFFSET_VAL_4,
  NB1_FREQ_OFFSET_VAL_5,
  NB1_FREQ_OFFSET_VAL_6,
  NB1_FREQ_OFFSET_VAL_7,
  NB1_FREQ_OFFSET_VAL_8,
  NB1_FREQ_OFFSET_VAL_9,
  NB1_FREQ_OFFSET_VAL_INVALID
} nb1_ul_freq_offset_val_e;

/*! @brief
  NB1 support bandwidths as indices
  Defined in 36.101 Section 5.4.2.1
*/
typedef enum
{
  /*! 1.4MHz bandwidth */
  NB1_BW_IDX_NRB_6 = 0,
  /*! 3MHz bandwidth */
  NB1_BW_IDX_NRB_15,
  /*! 5MHz bandwidth */
  NB1_BW_IDX_NRB_25,
  /*! 10MHz bandwidth */
  NB1_BW_IDX_NRB_50,
  /*! 15MHz bandwidth */
  NB1_BW_IDX_NRB_75,
  /*! 20MHz bandwidth */
  NB1_BW_IDX_NRB_100,
  /*! Minimum supported bandwidth */
  NB1_BW_IDX_MIN = NB1_BW_IDX_NRB_6,
  /*! Maximum supported bandwidth */
  NB1_BW_IDX_MAX = NB1_BW_IDX_NRB_100
} nb1_bandwidth_idx_e;

/*! @brief
    NB1 support bandwidths as bits in a bitmask
    Defined in 36.101 Section 5.4.2.1
*/
#define  NB1_BW_BMASK_NRB_6    (1 << (uint32) NB1_BW_IDX_NRB_6)
#define  NB1_BW_BMASK_NRB_15   (1 << (uint32) NB1_BW_IDX_NRB_15)
#define  NB1_BW_BMASK_NRB_25   (1 << (uint32) NB1_BW_IDX_NRB_25)
#define  NB1_BW_BMASK_NRB_50   (1 << (uint32) NB1_BW_IDX_NRB_50)
#define  NB1_BW_BMASK_NRB_75   (1 << (uint32) NB1_BW_IDX_NRB_75)
#define  NB1_BW_BMASK_NRB_100  (1 << (uint32) NB1_BW_IDX_NRB_100)

/*! @brief
    NB1 supported bandwidths in terms of frequency as bits in a bitmask
    Defined in 36.101 Section 5.4.2.1
*/
#define  NB1_BW_BMASK_1_4_MHZ  (1 << (uint32) NB1_BW_IDX_NRB_6)
#define  NB1_BW_BMASK_3_MHZ    (1 << (uint32) NB1_BW_IDX_NRB_15)
#define  NB1_BW_BMASK_5_MHZ    (1 << (uint32) NB1_BW_IDX_NRB_25)
#define  NB1_BW_BMASK_10_MHZ   (1 << (uint32) NB1_BW_IDX_NRB_50)
#define  NB1_BW_BMASK_15_MHZ   (1 << (uint32) NB1_BW_IDX_NRB_75)
#define  NB1_BW_BMASK_20_MHZ   (1 << (uint32) NB1_BW_IDX_NRB_100)


/*! @brief
  Describing the absolue cell's frequency Range: 0..262143
  Defined in 36.1.1 Section 5.4.4
  03/11/14: Extend to 32 bit, Range: 0..4294967295
*/
typedef uint32 nb1_earfcn_t;

/*! @brief
  Describing the absolue cell's frequency 
  32 bit, Range: 0..4294967295
  Used to covert to/from efs/mem format
*/
typedef uint32 nb1_efs_earfcn_t;

/*! @brief
  Describing the absolue cell's frequency Range: 1..64
  Used to covert to/from efs/mem format
*/
typedef uint8 nb1_efs_band_enum_t;

/*! @brief
  Describing the absolue cell's band Range: 1..256
  Used to covert to/from efs/mem format
*/
typedef uint16 nb1_efs_extnd_band_enum_t;
/*! @brief
  UTRA frequency Range: 0..16383
  @TODO remove once better home is found for such #defines
*/

typedef uint16 nb1_uarfcn_t;

/*===========================================================================

                    NB1 SYSTEM INFORMATION TYPES

===========================================================================*/
/*! @brief
  Enumeration of supported RATs
  Defined in 36.331 Section 6.3.4
*/
typedef enum
{
  /*! NB1 type */
  NB1_RAT_EUTRAN = 0,
  /*! WCDMA type */
  NB1_RAT_UTRAN,
  /*! GSM type */
  NB1_RAT_GERAN,
  /*! 1X RTT type */
  NB1_RAT_CDMA1XRTT,
  /*! HRPD type */
  NB1_RAT_CDMAHRPD,
  /*! TDSCDMA type */
  NB1_RAT_UTRAN_TDD,
  /*! MAX number of RATs supported */
  NB1_MAX_NUM_RATS,
  /*! Indicates no RAT */
  NB1_RAT_NONE
} nb1_rat_e;


/*===========================================================================

                    RNTI TYPES

===========================================================================*/

/*!
  @defgroup rntis

  Describes the ranges and values for RNTIs from 36.321 Section 7.1

  @{
  */
#define NB1_SI_RNTI_VAL 0xFFFF
#define NB1_P_RNTI_VAL 0xFFFE   // conflict with  nb1_pdcch_rnti_e (nb1_ml1_gm_ul.h)
#define NB1_M_RNTI_VAL 0xFFFD

#define NB1_RA_RNTI_MIN_FDD 0x1
#define NB1_RA_RNTI_MAX_FDD 0xA

#define NB1_RA_RNTI_MIN_TDD 0x1
#define NB1_RA_RNTI_MAX_TDD 0x3C

#define NB1_COM_RNTI_POOL_MIN_FDD 0x1
#define NB1_COM_RNTI_POOL_MAX_FDD 0xFFF3

#define NB1_COM_RNTI_POOL_MIN_TDD 0x1
#define NB1_COM_RNTI_POOL_MAX_TDD 0xFFF3

#define NB1_RNTI_RESERVED_MIN 0xFFF4
#define NB1_RNTI_RESERVED_MAX 0xFFFD

#define NB1_MAX_RNTI 0xFFFF

/* @} */ /* end group rntis */

/* NB1 capabilities begin */
/* NB1_UE_CATEGORY moved to nb1.h */
#define NB1_SON_RACH_RPT_R9_SUPPORTED                        TRUE

#define NB1_TX_ANTENNA_SELECTION_SUPPORTED_TDD               TRUE
#define NB1_TX_ANTENNA_SELECTION_SUPPORTED_FDD               FALSE
#define NB1_ROHC_PROFILE_0x0001_SUPPORTED                    TRUE
#define NB1_ROHC_PROFILE_0x0002_SUPPORTED                    TRUE
#define NB1_ROHC_PROFILE_0x0003_SUPPORTED                    FALSE
#define NB1_ROHC_PROFILE_0x0004_SUPPORTED                    FALSE
#define NB1_ROHC_PROFILE_0x0006_SUPPORTED                    FALSE
#define NB1_ROHC_PROFILE_0x0101_SUPPORTED                    FALSE
#define NB1_ROHC_PROFILE_0x0102_SUPPORTED                    FALSE
#define NB1_ROHC_PROFILE_0x0103_SUPPORTED                    FALSE
#define NB1_ROHC_PROFILE_0x0104_SUPPORTED                    FALSE
/* valid values as defined in 36.331 section 6.3.6
    cs2, cs4, cs8, cs12, cs16, cs24, cs32, cs48, cs64, cs128, cs256, cs512,
    cs1024, cs16384
    cs2 corresponds with 2 (context sessions), cs4 corresponds with 4 and so on
, setting it to the default 16 */
#define NB1_MAX_NUMBER_ROHC_CONTEXT_SESSIONS                 12
#define NB1_SUPPORTED_NUMBER_ROHC_CONTEXT_SESSIONS           2
#define NB1_DEFAULT_NUMBER_ROHC_CONTEXT_SESSIONS             2
#define NB1_SPECIFIC_REF_SIGS_SUPPORTED                      FALSE
#define NB1_HALF_DUPLEX                                      FALSE
#define NB1_INTER_FREQ_MEAS_GAPS_NEEDED                      TRUE
#define NB1_INTER_FREQ_MEAS_GAPS_NOT_NEEDED                  FALSE
#define NB1_IRAT_MEAS_GAPS_NEEDED                            TRUE
#define NB1_FEICIC_DCCH_SIB1_SUPPORTED                                 TRUE
#define NB1_FEICIC_CRS_SUPPORTED                                 TRUE
#define NB1_TDD_SPECIAL_SUBFRAME_SUPPORTED                   TRUE

/* Rel8 feature group indicators */
/* bit 1 */
#define NB1_INTRA_SUBFRAME_FREQ_HOPPING                      TRUE
#define NB1_DCI_FORMAT_3A                                    TRUE
/* #define NB1_MULTI_USER_MIMO                                  FALSE */
/* Renamed per Sept 2010 36.331 */
#define NB1_PDSCH_TM_5                                       FALSE
#define NB1_APERIODIC_CQI_REP_WITHOUT_PMI                    TRUE
#define NB1_APERIODIC_CQI_REP_WITH_MULT_PMI                  TRUE
/* bit 2 */
#define NB1_SIMUL_CQI_ACK_NACK                               TRUE
#define NB1_ABS_TPC_CMD                                      TRUE
#define NB1_RES_ALLOC_TYPE_1                                 TRUE
#define NB1_PERIODIC_CQI_REP_WITHOUT_PMI                     TRUE
#define NB1_PERIODIC_CQI_REP_WITH_SINGLE_PMI                 TRUE
/* bit 3 */
#define NB1_SEMI_PERSISTENT_SCHEDULING                       TRUE
#define NB1_TTI_BUNDLING                                     TRUE
#define NB1_5BIT_RLC_UM_SN                                   TRUE
#define NB1_7BIT_PDCP_SN                                     TRUE

/* bit 4 */
#define NB1_SHORT_DRX_CYCLE                                  TRUE
/* bit 5 */
#define NB1_LONG_DRX_CYCLE                                   TRUE
#define NB1_DRX_CMD_MAC_CTRL_ELEM                            TRUE

/* bit 6 */
#define NB1_PRIORITIZED_BIT_RATE                             TRUE
/* bit 7 */
#define NB1_RLC_UM                                           TRUE
/* bit 8 (see custnb1.h) */
#define NB1_EUTRA_CONN_TO_UTRA_CELL_DCH_PS_HO \
        NB1_EUTRA_CONN_TO_UTRA_CELL_DCH_PS_HO_DEFINED
/* bit 9 */
#define NB1_EUTRA_CONN_TO_GERAN_GSM_DED_HO                   TRUE

/* bit 10 */
#define NB1_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO \
        NB1_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO_DEFINED

#define NB1_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO_WITH_NACC \
        NB1_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO_WITH_NACC_DEFINED

/* bit 11 (see custnb1.h) */
#define NB1_EUTRA_CONN_TO_1XRTT_CS_ACTIVE_HO \
        NB1_EUTRA_CONN_TO_1XRTT_CS_ACTIVE_HO_DEFINED
/* bit 12 */
#define NB1_EUTRA_CONN_TO_HRPD_ACTIVE_HO \
        NB1_EUTRA_CONN_TO_HRPD_ACTIVE_HO_DEFINED
/* bit 13 */
#define NB1_INTER_FREQ_HO                                    TRUE
/* bit 14 */
#define NB1_MEAS_EVENT_A4                                    TRUE
#define NB1_MEAS_EVENT_A5                                    TRUE

/* bit 15 (see custnb1.h) */
#define NB1_MEAS_EVENT_B1               NB1_MEAS_EVENT_B1_DEFINED

/* bit 16 */
#define NB1_MEAS_PERIODIC_REP_NON_ANR                        TRUE
/* bit 17 - Can be set to TRUE only if bit 5 is set to TRUE */
#define NB1_MEAS_PERIODIC_REP_SON_ANR                        TRUE
#define NB1_MEAS_ANR_INTRA_FREQ_REP                          TRUE
/* bit 18 */
#define NB1_MEAS_ANR_INTER_FREQ_REP                          TRUE
/* bit 19 */
#define NB1_MEAS_ANR_INTER_RAT_REP                           TRUE

/* bit 20 */
#define NB1_5X_AM_DRB_3X_UM_DRB                              TRUE
#define NB1_8X_AM_DRB                                        TRUE
/* bit 21 */
#define NB1_PREDEFINED_INTRA_SUBFRAME_FREQ_HOPPING           TRUE
#define NB1_PREDEFINED_INTER_SUBFRAME_FREQ_HOPPING           TRUE

/* bit 22 (see custnb1.h) */
#define NB1_MEAS_UTRAN_REP_IN_EUTRA_CONN \
        NB1_MEAS_UTRAN_REP_IN_EUTRA_CONN_DEFINED

/* bit 23 (see custnb1.h) */
#define NB1_MEAS_GERAN_REP_IN_EUTRA_CONN \
        NB1_MEAS_GERAN_REP_IN_EUTRA_CONN_DEFINED
/* bit 24 */
#define NB1_MEAS_1X_REP_IN_EUTRA_CONN \
        NB1_MEAS_1X_REP_IN_EUTRA_CONN_DEFINED
/* bit 25 */
#define NB1_MEAS_INTER_FREQ_REP_IN_EUTRA_CONN                TRUE
/* bit 26 (see custnb1.h)  */
#define NB1_MEAS_HRPD_REP_IN_EUTRA_CONN \
        NB1_MEAS_HRPD_REP_IN_EUTRA_CONN_DEFINED
/* bit 27 */
#define NB1_EUTRA_CONN_TO_UTRA_CELL_DCH_CS_HO                TRUE

/* bit 31 */
#define NB1_MULTI_BAND_INFO_LIST                             TRUE

/* Rel9 feature group indicators */
/* bit 33 */
#define NB1_MEAS_UTRAN_ANR_REP                               TRUE
/* bit 34 */
#define NB1_MEAS_GERAN_ANR_REP                               TRUE
/* bit 35 */
#define NB1_MEAS_1X_ANR_REP                                  TRUE
/* bit 36 */
#define NB1_MEAS_HRPD_ANR_REP                                TRUE

/* Rel10 feature group indicators */
/* bit 102 aperiodic SRS*/
#define NB1_APERIODIC_SRS                                    TRUE

/* bit 103 */
#define NB1_PDSCH_TM9_MAX_4_ANT                                  FALSE

/* bit 104 */
#define NB1_PDSCH_TM9_MAX_8_ANT                                  FALSE

/* bit 105 */
#define NB1_PERIODIC_CQI_PMI_TM9_4_ANT                         FALSE

/* bit 106 */
#define NB1_PERIODIC_CQI_PMI_TM9_8_ANT                         FALSE

/* bit 107 */
#define NB1_APERIODIC_CQI_PMI_TM9_4_ANT                        FALSE

/* bit 108 */
#define NB1_APERIODIC_CQI_PMI_TM9_8_ANT                        FALSE

/* bit 109 */
#define NB1_PERIODIC_CQI_PMI_TM9_8_ANT_MODE_1_1                    FALSE

/* bit 110 */
#define NB1_PERIODIC_CQI_PMI_TM9_8_ANT_MODE_1_2                    FALSE

/* Rel10 feature group indicators */
/* bit 111 */
#define NB1_MEAS_EVENT_A6                                    TRUE

/* bit 112 */
#define NB1_X2L_PSHO_WITH_CA_FEATURE_ENABLE                  TRUE

/* bit 114 */
#define NB1_MEAS_REPORT_CPICH_RSCP_AND_ECNO                  TRUE

/* bit 115 */
#define NB1_EICIC_MEAS_SUBFRAME_RESTRICTION                  FALSE

/* bit 30 (see custnb1.h)*/
#define NB1_HANDOVER_BETWEEN_FDD_AND_TDD \
          NB1_HANDOVER_BETWEEN_FDD_AND_TDD_DEFINED
/* NB1 capabilities end */

/*UE supports enhanced dual layer (PDSCH transmission mode 8) for TDD */
#define NB1_PDSCH_TM_8_L_TDD                                 TRUE

/*UE supports enhanced dual layer (PDSCH transmission mode 8) for FDD */
#define NB1_PDSCH_TM_8_L_FDD                                 FALSE

#define NB1_PDCP_SN_EXTENSION_R11      FALSE

#define NB1_SUPPORT_ROHCCONTEXTCONTINUE_R11 TRUE

#define NB1_CRS_INTERFHANDL_R11 \
           NB1_FEICIC_CRS_SUPPORTED

#define NB1_SS_CCH_INTERFHANDL_R11 \
           NB1_FEICIC_DCCH_SIB1_SUPPORTED

#define NB1_TDD_SPECIAL_SUBFRAME_R11 \
            NB1_TDD_SPECIAL_SUBFRAME_SUPPORTED

#define NB1_DL_COMP_1CSIPROC_R11 FALSE

#define NB1_DL_COMP_MULTICSIPROC_R11 FALSE

#define NB1_EPDCCH_R11                 FALSE
#define NB1_MULTIACK_CSIREPORTING_R11  FALSE
#define NB1_TXDIV_PUCCH1B_CHSELECT_R11 FALSE
#define NB1_UL_COMP_R11                FALSE
#define NB1_MULTIPLETIMINGADVANCE_R11  FALSE
#define NB1_SIMULTANEOUSRX_TX_R11      FALSE
#define NB1_SUPPORTEDCSI_PROC_R11      FALSE
#define NB1_RSRQMEASWIDEBAND_R11       FALSE
#define NB1_UE_RX_TXTIMEDIFFMEASUREMENTSFORMDT_R11 FALSE
#define NB1_CDMA2000_NWSHARING_R11     FALSE
#define NB1_INDEVICECOEXIND_R11        FALSE
#define NB1_POWERPREFIND_R11           FALSE
#define NB1_STANDALONEGNSS_LOCATION_R10 TRUE
#define NB1_SIMULTANEOUSACKNACKANDCQI_FORMAT3_R11 FALSE
#define NB1_IDC_CONFIG_R11              FALSE
#define NB1_DRX_CONFIG_V1130            FALSE
#define NB1_OBTAINLOCATIONCONFIG_R11    FALSE
#define NB1_LOCATIONINFO_R10            FALSE

/* From 36.321 Section 7.1

  ----------------------------------------
  | RNTI TYPE   |         Value           |
  ----------------------------------------
  |             |   FDD     |    TDD      |
  ----------------------------------------
  |  RA-RNTI    |  0x0-0x9  |  0x0-0x3B   |
  ----------------------------------------
  |  C-RNTI,    |  0x000A-  |  0x003C-    |
  |  SPS-RNTI,  |  0xFFF2   |  0xFFF2     |
  |  Temp C-RNTI|           |             |
  ----------------------------------------
  |  Reserved   |    0xFFF3-0xFFFC        |
  ----------------------------------------
  |  P-RNTI     |       0xFFFE            |
  ----------------------------------------
  |  SI-RNTI    |       0xFFFF            |
  -----------------------------------------
*/
/*! @brief
  The type enum for RNTIs.
  Defined in TBD
*/
typedef uint16 nb1_rnti_val_t;


/*! @brief
  This is an enumeration of all RNTI types. Note this matches to the
  firmware rnti_enable_mask by corresponding bit shift.
  Defined in 36.321 Section 7.1
*/
/*! @brief
  This is an enumeration of all RNTI types. Note this matches to the
  firmware rnti_enable_mask by corresponding bit shift.
  Defined in 36.321 Section 7.1
*/
typedef enum
{
  /*! RNTI to identify System Information */
  NB1_SI_RNTI_TYPE = 0,
  /*! RNTI to decode dedicated UE data in connected mode */
  NB1_C_RNTI_TYPE,
  /*! RNTI used to identify paging information */
  NB1_P_RNTI_TYPE,
  /*! RNTI associated with PDCCH carrying RACH response */
  NB1_RA_RNTI_TYPE,
  /*! Temporary RNTI to decode MSG 4 of the RACH procedure */
  NB1_TEMP_RNTI_TYPE,
  /*! Max number of RNTI types */
  NB1_MAX_NUM_RNTI_TYPE,

} nb1_rnti_type_e;

/*! @brief
  This type capture the RNTI information
*/
typedef struct
{
  /*! The RNTI value */
  nb1_rnti_val_t   rnti_val;
  /*! The RNTI type */
  nb1_rnti_type_e  rnti_type;
} nb1_rnti_info_s;

/*! @brief
  The system/sub-frame number type
*/
typedef struct
{
  uint16 sub_fn:4;
  uint16 sys_fn:10;
  uint16 xxx:2;
} nb1_sfn_s;

/*! @brief MAC RACH contention result indication.
  MAC UL sends to L1,RRC and MAC DL
*/
typedef struct
{
  msgr_hdr_struct_type   hdr;     /*!< Message header */
  /* Message payload */
  boolean                status;  /*!< contention status */
  nb1_sfn_s              sfn;     /*!< system time when MSG4 is decoded */
  uint32                 carrier_idx; /*! Carrier idx needed to use after contention res*/
} nb1_mac_contention_result_ind_msg_s;

/*! @brief MAC send this indication to ML1 to notify ML1 that MAC
  does not have enough grant to include PHR.
*/
typedef struct
{
  msgr_hdr_struct_type   hdr;     /*!< Message header */
  /* Message payload */
} nb1_mac_send_phr_failed_ind_msg_s;

/*! @brief Data structure for Handover Completed indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  nb1_phy_cell_id_t phy_cell_id;  /*!< Physical cell id */
  nb1_earfcn_t dl_freq;  /*!< DL Frequency */
  nb1_earfcn_t ul_freq;  /*!< UL Frequency */
  nb1_bandwidth_e dl_bandwidth; /*!< DL Bandwidth */
  nb1_bandwidth_e ul_bandwidth; /*!< UL Bandwidth */
} nb1_rrc_handover_completed_ind_s;

/*! @brief Data structure for Config Completed indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

} nb1_rrc_config_completed_ind_s;

typedef struct
{
  nb1_earfcn_t carrier_freq;

  boolean carrier_freq_offset_present;

  nb1_freq_offset_e carrier_freq_offset;

} nb1_carrier_freq_s;



/*===========================================================================

  FUNCTION:  NB1_ML1_GET_CURR_SFN

===========================================================================*/
/*!
    @brief
    The function returns the current sfn in the format of nb1_sfn_s

    @details
    Get the sfn from the ML1 scheduler interface.

    @return
    The current sfn and subframe number of the current serving cell.
*/
/*=========================================================================*/
extern nb1_sfn_s nb1_ml1_get_curr_sfn( void );

/*==============================================================================

  FUNCTION:  nb1_mac_get_curr_celevel

==============================================================================*/
/*!
    @brief
    This API return MAC rach current CE level for CMAPI

    @detail
    None

    @return
    uint8 ce_level

*/
/*============================================================================*/
extern uint8 nb1_mac_get_curr_celevel(  void );
extern uint8 _nb1_mac_get_curr_celevel(  void );
#endif /* NB1_AS_H */

