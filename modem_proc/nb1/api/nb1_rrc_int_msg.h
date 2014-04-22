/*!
  @file
  nb1_rrc_int_msg.h

  @brief
  NB1 RRC internal interfaces

  @detail

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

$Header: //components/rel/nb1.mpss/2.2/api/nb1_rrc_int_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef NB1_RRC_INT_MSG_H
#define NB1_RRC_INT_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <msgr.h>
#include <msgr_nb1.h>
#include <nb1_as.h>
#include <nb1_cphy_msg.h>
#include <nb1_ml1_ext_api.h>
#include <nb1_rrc_ext_msg.h>
#include <nb1.h>
#include <nb1_security.h>
#include <dsm_pool.h>
#include <nb1_tlb.h>
#include <cm_msgr_msg.h>
#include <nb1_rrc_osys_asn1.h>
#include <nb1_mac_rrc.h>
#include <nb1_rlc_rrc.h>
#include <nb1_pdcp_msg.h>


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define NB1_RRC_DISPATCHER_EXTERNAL_MB_DEPTH          10
#define NB1_RRC_DISPATCHER_EXTERNALOTA_MB_DEPTH    10
#define NB1_RRC_DISPATCHER_INTERNAL_MB_DEPTH           20
#define NB1_RRC_DISPATCHER_PENDING_MB_DEPTH            10

#define NB1_RRC_DISPATCHER_EXTERNALOTA_MB_PRIO        1
#define NB1_RRC_DISPATCHER_EXTERNAL_MB_PRIO              2
#define NB1_RRC_DISPATCHER_PENDING_MB_PRIO                3
#define NB1_RRC_DISPATCHER_INTERNAL_MB_PRIO               4

/*! @brief NB1_RRC_MAX_CFG_INFO - Indicates the max number of req in one NB1_CFG_REQI
*/
#define NB1_RRC_MAX_CFG_INFO 2

/*! Max EUTRA Cells that we can blacklist per frequency */
#define NB1_RRC_MAX_BLIST_CELLS_PER_FREQ     (16)

#define NB1_RRC_MAX_INTFREQ_NEIGHBORS     (32)

/*! @brief Macros for remaining free memory in DSM small item pool
*/
#define NB1_RRC_REM_SMALL_ITEM_POOL_SIZE_IN_BYTES (DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL) \
                                                   * DSM_POOL_ITEM_SIZE(DSM_DS_SMALL_ITEM_POOL))

/*! @brief DSM Pool to use
*/
#define NB1_RRC_DSM_POOL(size) ((size <= NB1_RRC_REM_SMALL_ITEM_POOL_SIZE_IN_BYTES) ? \
                                                              (DSM_DS_SMALL_ITEM_POOL) : (DSM_DS_LARGE_ITEM_POOL))

/*! @brief NULL SFN, i.e. indicates invalid or unused value
*/
#define NB1_RRC_NULL_SFN NB1_CPHY_NULL_SFN


/*! @brief NULL SUB FN, i.e. indicates invalid or unused value
*/

#define NB1_RRC_NULL_SUBFN NB1_CHY_NULL_SUBFN

/*! @brief NULL SFN, i.e. indicates invalid or unused value
*/
#define NB1_RRC_NULL_HSFN NB1_CPHY_NULL_HYPER_SFN

/*! @brief Bitmask for MIB
*/
#define NB1_RRC_MIB_BITMASK   ((nb1_rrc_sib_bitmask_t)NB1_CPHY_SIB_MASK_BIT_MIB)

/*! @brief Bitmask for SIB1
*/
#define NB1_RRC_SIB1_BITMASK  ((nb1_rrc_sib_bitmask_t)NB1_CPHY_SIB_MASK_BIT_SIB1)

/*! @brief Bitmask for SIB2
*/
#define NB1_RRC_SIB2_BITMASK  ((nb1_rrc_sib_bitmask_t)NB1_CPHY_SIB_MASK_BIT_SIB2)

/*! @brief Bitmask for SIB3
*/
#define NB1_RRC_SIB3_BITMASK  ((nb1_rrc_sib_bitmask_t)NB1_CPHY_SIB_MASK_BIT_SIB3)

/*! @brief Bitmask for SIB4
*/
#define NB1_RRC_SIB4_BITMASK  ((nb1_rrc_sib_bitmask_t)NB1_CPHY_SIB_MASK_BIT_SIB4)

/*! @brief Bitmask for SIB5
*/
#define NB1_RRC_SIB5_BITMASK  ((nb1_rrc_sib_bitmask_t)NB1_CPHY_SIB_MASK_BIT_SIB5)

/*! @brief Bitmask for SIB9
*/
#define NB1_RRC_SIB9_BITMASK  ((nb1_rrc_sib_bitmask_t)NB1_CPHY_SIB_MASK_BIT_SIB9)

/*! @brief Bitmask for SIB10
*/
#define NB1_RRC_SIB10_BITMASK  ((nb1_rrc_sib_bitmask_t)NB1_CPHY_SIB_MASK_BIT_SIB10)

/*! @brief Bitmask for SIB11
*/
#define NB1_RRC_SIB11_BITMASK  ((nb1_rrc_sib_bitmask_t)NB1_CPHY_SIB_MASK_BIT_SIB11)

/*! @brief Bitmask for SIB12
*/
#define NB1_RRC_SIB12_BITMASK  ((nb1_rrc_sib_bitmask_t)NB1_CPHY_SIB_MASK_BIT_SIB12)

/*! @brief Bitmask for SIB13
*/
#define NB1_RRC_SIB13_BITMASK  ((nb1_rrc_sib_bitmask_t)NB1_CPHY_SIB_MASK_BIT_SIB13)

/*! @brief Bitmask for SIB14
*/
#define NB1_RRC_SIB14_BITMASK  ((nb1_rrc_sib_bitmask_t)NB1_CPHY_SIB_MASK_BIT_SIB14)

/*! @brief Bitmask for SIB15
*/
#define NB1_RRC_SIB15_BITMASK  ((nb1_rrc_sib_bitmask_t)NB1_CPHY_SIB_MASK_BIT_SIB15)

/*! @brief Bitmask for SIB16
*/
#define NB1_RRC_SIB16_BITMASK  ((nb1_rrc_sib_bitmask_t)NB1_CPHY_SIB_MASK_BIT_SIB16)

/*! @brief Bitmask for SIB22
*/
#define NB1_RRC_SIB22_BITMASK  ((nb1_rrc_sib_bitmask_t)NB1_CPHY_SIB_MASK_BIT_SIB22)

/*! @brief Maximum length of an encoded SI,
Max TBS for SI messages is 680 bits(85 bytes), and make it to be 16 bytes aligned.
*/
#define NB1_RRC_MAX_SI_LENGTH 96

/*! @brief Maximum number of SIs (Maximum supported SIB - 1)
e.g.: SIB16 is the max, then 15. Since SIB1 doesnt come a SI
*/
#define NB1_RRC_SIB_MAX_SIS 8
/*! @brief Maximum number of cells in SIB DB
*/
#define NB1_RRC_SIB_DB_MAX_CELLS 5




/*! @brief Maximum length of an Unsolicited SI

The physical layer imposes a limit to the maximum size a SIB can take. When
DCI format 1C is used the maximum allowed by the physical layer is 1736 bits (
217 bytes) while for format 1A the limit is 2216 bits (277 bytes), see TS 36.
212 [22] and TS 36.213 [23]

*/
#define NB1_RRC_MAX_UNSOLICITED_SI_LENGTH 300

/*! @brief Default value of T310 timer
*/
#define NB1_RRC_DEFAULT_T310_VALUE 1000

/*! @brief macro for SRB0
*/
#define NB1_RRC_CCCH_SRB0 0

/*! @brief macro for SRB1
*/
#define NB1_RRC_DCCH_SRB1 1

/*! @brief macro for SRB1_BIS
*/
#define NB1_RRC_DCCH_SRB1_BIS 3

/*! @brief macro for max number of messages that can be pended in RRC
*/
#define NB1_RRC_PENDQ_MAX_QUEUE_LEN     5

/*! @brief Initialized value for LLC Trans id, to be set by other RRC modules
  before sending CFG_REQI to LLC
*/
#define NB1_RRC_INVALID_TRANS_ID 0xFF

/*! @brief Initialized value for index corresponding to band in multiBandInfoList
*/
#define NB1_RRC_INVALID_MFBI_INDEX 0xFF

/*! @brief Initialized value for SI specific value tag
*/
#define NB1_RRC_INVALID_SI_VALUE_TAG 0xFF

/*<! Max number of deprioritized NB1 freqs, set to same as Max num of EUTRA freqs
  that can be sent/received for IRAT */
#define NB1_RRC_MAX_NUM_DEPRI_FREQS NB1_IRAT_MAX_EUTRAN_FREQ

/*! @brief Macro for SEC_TO_MS
*/
#define NB1_RRC_SEC_TO_MS 1000

/*! @brief Macro for MIN_TO_SEC
*/
#define NB1_RRC_MIN_TO_SEC 60

#define NB1_RRC_MIN_DELAY_TOLERANT_TIME 0
#define NB1_RRC_MAX_DELAY_TOLERANT_TIME 1800

/*! @brief Maximum number of entries in SIB1 PLMN list
*/
#define NB1_RRC_MAX_SIB1_PLMN_LIST_SIZE 6

#define NB1_RRC_MDT_AREA_CFG_MAX_NUM_CGI 32
#define NB1_RRC_MDT_AREA_CFG_MAX_NUM_TAC 8
#define NB1_RRC_MDT_TRACE_SESSION_REF_NUM_BITS 2
#define NB1_RRC_MDT_TRACE_ID_NUM_BITS 3

#define NB1_RRC_MAX_CA_BANDS_IN_BAND_COMBO 5

/*! @brief Number of SIBs (excluding MIB)
*/
#define NB1_RRC_SIB_NUM_OF_SIBS 9

/*! @brief Highest SIB number
*/
#define NB1_RRC_MAX_SIB_NUM 22

/*! EFS file permissions */
#define NB1_RRC_CSP_ACQ_DB_EFS_FILE_PERMISSIONS 0777

/*! EFS file path for acq db */
#define NB1_RRC_CSP_ACQ_DB_EFS_FILENAME "/nv/reg_files/modem/nb1/rrc/csp/acq_db"

/*! EFS file path for Inter Freq Band */
#define NB1_RRC_CSP_INTER_FREQ_BAND_EFS_FILENAME "/nv/reg_files/modem/nb1/rrc/csp/inter_freq_band"

/*! EFS file path for PSM acq db */
#define NB1_RRC_CSP_PSM_ACQ_DB_EFS_FILENAME "/nv/item_files/modem/nb1/rrc/csp/psm_acq_db"


/*! Maximum number of entries in SIB1 PLMN list */
#define NB1_RRC_CSP_MAX_SIB1_PLMN_LIST_SIZE 6

#define NB1_RRC_CSP_MAX_ACQ_DB_ENTRIES    10

#define NB1_RRC_CSP_MAX_ACQ_DB_EXT_ENTRIES    64

#define NB1_RRC_CSP_MAX_ACQ_DB_EXT_SYSTEMS    NB1_RRC_CSP_MAX_ACQ_DB_EXT_ENTRIES

#define NB1_RRC_CSP_MAX_ACQ_CANDIDATE_LIST_ENTRIES  (NB1_CPHY_BANDSCAN_MAX_SUPPORTED_BANDS*NB1_CPHY_BANDSCAN_NUM_CANDIDATES)

#define NB1_RRC_CSP_MAX_PREF_FREQ_LIST_ENTRIES   32

#define NB1_RRC_PSM_POI_ENCODED_PAGE_SIZE 70

#define NB1_RRC_PSM_SIB1_DECODE_LATENCY 5  //5 seconds

#define NB1_RRC_PSM_SIB1_DECODE_PSM_DURATION_MIN_DURATION 1200 //20 mins

/*! @brief Enum to represent various scan types in CSP
*/
typedef enum
{
  /*! acq db version 1 */
  NB1_RRC_CSP_ACQ_DB_VERSION_1 = 1,

  /*! acq db version 2 */
  NB1_RRC_CSP_ACQ_DB_VERSION_2,

  /*! acq db version 3  */
  NB1_RRC_CSP_ACQ_DB_VERSION_3,

  /*! acq db version max  */
  NB1_RRC_CSP_ACQ_DB_VERSION_MAX,

}nb1_rrc_csp_acq_db_version_e;

/*! @brief Enum to represent various scan types in CSP
*/
typedef enum
{
    /*! Cell Selection scan triggered by NAS request */
  NB1_RRC_CSP_SCAN_CELL_SEL_NAS,

    /*! Cell Selection scan due to Connected to Idle transition */
  NB1_RRC_CSP_SCAN_CELL_SEL_CONN_TO_IDLE,

  /*! Cell Selection scan due to Reselection */
  NB1_RRC_CSP_SCAN_CELL_SEL_RESELECTION,

  /*! Cell Selection scan due to intraNB1 Redirection */
  NB1_RRC_CSP_SCAN_CELL_SEL_INTRA_NB1_REDIR,

  /*! Cell Selection scan due to RLF */
  NB1_RRC_CSP_SCAN_CELL_SEL_RLF,

  /*! Cell Selection scan due to Connection release during RLF */
  NB1_RRC_CSP_SCAN_CELL_SEL_CONN_TO_IDLE_CRE_FAILURE,

  /*! Cell Selection scan triggered internally by CSP */
  NB1_RRC_CSP_SCAN_CELL_SEL_CSP,

}nb1_rrc_csp_scan_type_e;

typedef enum
{
  /*! ACQ list contains none of the below */
  NB1_RRC_CSP_ACQ_CANDIDATE_LIST_NONE = 0,

  /*! ACQ list contains systems scan entries */
  NB1_RRC_CSP_ACQ_CANDIDATE_LIST_SYSTEM_SCAN,

  /*! ACQ list contains band scan results */
  NB1_RRC_CSP_ACQ_CANDIDATE_LIST_BAND_SCAN,

} nb1_rrc_csp_acq_candidate_list_type_e;

/*! @brief Enumeration for timer type
*/
typedef enum
{
  NB1_RRC_SIB_TWENTY_FOUR_HOUR_TICK_TMR,     /*!< 1 - SIB 24hrs Timer */
  NB1_RRC_PLMN_SEARCH_ACQ_DB_TMR,            /*!< 2 - PLMN Srch Acq DB Timer */ 
  NB1_RRC_PLMN_SEARCH_MSEARCH_BSACN_SEP_TMR, /*!< 3 - Manual Search Band Scan Seperator Timer */    
  NB1_RRC_TMR_MAX                            /*!< 4 */
} nb1_rrc_tmr_type_e;



/*! @brief This structure defines the contents of each entry
           in the Acq DB as stored in EFS
*/
typedef struct
{
  /*! Numper of PLMNs from SIB1 */
  uint8 num_plmns;

  /*! PLMN List from SIB1 */
  sys_plmn_id_s_type plmn[NB1_RRC_CSP_MAX_SIB1_PLMN_LIST_SIZE];

  /*! DL Center frequency */
  nb1_efs_earfcn_t dl_earfcn;

  /*! Band */
  sys_sband_lte_e_type band;

  /*! Physical cell id of last camped cell */
  nb1_phy_cell_id_t phy_cell_id;

  /*! Index of PLMN selected for camping in the SIB1 list. For limited service,
      it is 0 */
  uint32 selected_plmn_index;
}nb1_rrc_csp_efs_acq_entry_s;

/*! @brief CSP PSM EFS Acq DB structure
*/
typedef struct
{
  /*! DB is valid or nor */
  boolean db_valid;
  
  /*! Acquisition Database */
  nb1_rrc_csp_efs_acq_entry_s efs_db;
}nb1_rrc_csp_psm_acq_db_efs;

/*! @brief CSP EFS Acq DB structure
*/
typedef struct
{
  /*! Acq DB Format Version */
  uint8 version;

  /*! Indicates the number of items stored in acq list order. If value is
   *  NB1_RRC_CSP_MAX_ACQ_DB_ENTRIES, it indicates that the Acq List is
   *  full and an old entry needs to be cleared to make room for the new one
   */
  uint8 acq_db_count;

    /*! Virtual list to access the Acq DB entries. Indices are sorted by age */
  uint8 acq_db_index[NB1_RRC_CSP_MAX_ACQ_DB_ENTRIES];

  /*! Is the corresponding array element of db occupied? */
  boolean entry_is_occupied[NB1_RRC_CSP_MAX_ACQ_DB_ENTRIES];

  /*! Acquisition Database */
  nb1_rrc_csp_efs_acq_entry_s efs_db[NB1_RRC_CSP_MAX_ACQ_DB_ENTRIES];

}nb1_rrc_csp_efs_acq_db_s;

/*! @brief This structure defines the contents of each entry 
           in the Acq DB V2 as stored in V2 EFS
*/
typedef struct
{
  /*! PLMN List from SIB1 */
  sys_plmn_id_s_type plmn[NB1_RRC_CSP_MAX_SIB1_PLMN_LIST_SIZE];

  /*! DL Center frequency */
  nb1_earfcn_t dl_earfcn;

  /*! Index of PLMN selected for camping in the SIB1 list. For limited service,
      it is 0 */
  uint32 selected_plmn_index;

  /*! Band */
  nb1_efs_extnd_band_enum_t band;

  /*! Physical cell id of last camped cell */
  nb1_phy_cell_id_t phy_cell_id;

  /*! Numper of PLMNs from SIB1 */
  uint8 num_plmns;
}nb1_rrc_csp_efs_acq_entry_v2_s;

/*! @brief CSP EFS Acq DB V2 structure
*/
typedef struct
{
  /*! Acq DB Format Version */
  uint8 version;

  /*! Indicates the number of items stored in acq list order. If value is 
   *  NB1_RRC_CSP_MAX_SIB1_PLMN_LIST_SIZE, it indicates that the Acq List is 
   *  full and an old entry needs to be cleared to make room for the new one 
   */
  uint8 acq_db_count;

  /*! Virtual list to access the Acq DB entries. Indices are sorted by age */
  uint8 acq_db_index[NB1_RRC_CSP_MAX_ACQ_DB_ENTRIES];

  /*! Is the corresponding array element of db occupied? */
  boolean entry_is_occupied[NB1_RRC_CSP_MAX_ACQ_DB_ENTRIES];

  /*! Acquisition Database */
  nb1_rrc_csp_efs_acq_entry_v2_s efs_db[NB1_RRC_CSP_MAX_ACQ_DB_ENTRIES];
  
}nb1_rrc_csp_efs_acq_db_v2_s;

/*! @brief CSP EFS Acq DB Extension structure
*/
typedef struct
{
  /*! Acq DB Format Version */
  uint8 version;

  /*! Indicates the number of items stored in acq list order. If value is
   *  NB1_RRC_CSP_MAX_ACQ_DB_ENTRIES, it indicates that the Acq List is
   *  full and an old entry needs to be cleared to make room for the new one
   */
  uint8 acq_db_count;

  /*! Acquisition Database */
  nb1_rrc_csp_efs_acq_entry_v2_s efs_db[NB1_RRC_CSP_MAX_ACQ_DB_EXT_ENTRIES];

    /*! Virtual list to access the Acq DB entries. Indices are sorted by age */
  uint8 acq_db_index[NB1_RRC_CSP_MAX_ACQ_DB_EXT_ENTRIES];

  /*! Is the corresponding array element of db occupied? */
  boolean entry_is_occupied[NB1_RRC_CSP_MAX_ACQ_DB_EXT_ENTRIES];
}nb1_rrc_csp_efs_acq_db_v3_s;

/*! @brief This structure to hold the Acq list during cell selection process
*/
typedef struct
{
  /*! 0..65535 (allowing for expansion) */
  nb1_earfcn_t  dl_earfcn;

  /*! E-UTRA Band (optional; if 0 L1 calculates band from EARFCN) Range:
      0, 1..14, 33..40 */
  sys_sband_lte_e_type band;

  /*! Indicates the number of cells in the blacklist Range: 0..4 */
  uint8 num_black_cells;

  /*! Indicates the number of cells tried for a particular frequency, need
      not be the same as num_black_cells because barred cells are not added
      to blacklist */
  uint8 num_tried_cells;

  /*! If PCI is provided, camp on that cell only */
  boolean is_pci_valid;

  /* Did acquisition succeed on this entry? */
  boolean acq_success;

  /*! Energy, if known */
  int16 energy;

  /*! Cell blacklist; L1 must try to acquire a cell not in this list; used for
      e.g. barred cells, can be empty */
  nb1_phy_cell_id_t blacklist[NB1_CPHY_MAX_ACQ_BLACKLISTED_CELLS];

  /*! PCI */
  nb1_phy_cell_id_t pci;  

  /*! Custom duplex spaced cell */
  boolean custom_duplex_spacing;
}nb1_rrc_csp_acq_candidate_list_entry_s;

/*! @brief This structure holds the Acq list.
*/
typedef struct
{
  /*! Number of entries in the list. 0 indicates Acq list is empty */
  int32 num_entries;

  /*! Index that was last tried. -1 indicates that this is a fresh list
        and none of the entries have been tried. If this index is equal to
            "num_entries-1" then all entries have been tried */
  int32 last_tried_index;

  /*! Acquisition list prepared wither from System scan results or Band scan
        results */
  nb1_rrc_csp_acq_candidate_list_entry_s
              acq_list[NB1_RRC_CSP_MAX_ACQ_CANDIDATE_LIST_ENTRIES];
}nb1_rrc_csp_acq_candidate_list_s;

/*! @brief This structure holds the candidate' cell's data
*/
typedef struct
{
  /*! Cell identity */
  lte_rrc_cell_id_s cell_id;

  /*! Energy, if known */
  int16 energy;

  /*! Band to which cell belongs */
  sys_sband_lte_e_type band;

  /*! Is the cell barred */
  boolean cell_barred;

}nb1_rrc_csp_candidate_cell_s;



/*! @brief Structure to hold band scan results
*/
typedef struct
{
    /*! Number of entries in the list. 0 indicates band list in empty */
    int32 num_entries;

    /*! Index that was last tried. -1 indicates that this is a fresh list
        and none of the entries have been tried. If this index is equal to
            "num_entries-1" then all entries have been tried
      There is a change in meaning with the feature of sending multiple bands
      in a band scan request. Now, this variable is the index of the last
      band in the group of bands that has been tried. In other words, if it is
      4, then indices tried were 2,3,4 if 3 bands were sent in the band scan
      request. num_sent_bands below would then be 3. The bands sent would be
      (last_tried_index - (num_sent_bands - 1)) to last_tried_index.
      The condition for all entries having been tried is still index equal to
      num_entries-1.
      */
  int32 last_tried_index;

  /*! Number of bands sent in last band scan request */
  int32 num_sent_bands;

  /*! Band information */
  sys_sband_lte_e_type bands[SYS_SBAND_LTE_EUTRAN_BAND_MAX];

}nb1_rrc_csp_band_scan_list_s;

/*! @brief This structure holds the data that is needed during a particular
    scan.
*/
typedef struct
{
  /*! Holds the current scan type. This field identifies what type of scan CSP
       is currently performing */
  nb1_rrc_csp_scan_type_e scan_type;

  /*! Band scan list */
  nb1_rrc_csp_band_scan_list_s band_scan_list;

  /*! Acq candidate list type */
  nb1_rrc_csp_acq_candidate_list_type_e acq_candidate_list_type;

  /*! Acq candidate list. This will be build either from System Scan results
      or band scan results */
  nb1_rrc_csp_acq_candidate_list_s acq_candidate_list;

  /*! Candidate cell */
  nb1_rrc_csp_candidate_cell_s candidate_cell;

  /*! List of acceptable PLMNs found so far */
  lte_rrc_found_plmn_list_s found_plmn_list;

  /*! attempt acquisition on a better cell if indicated by ML1*/
  boolean acq_on_better_cell  : 1;

  /*! priority scan in progress. Perform non priority scan at the end of priority scan*/
  boolean priority_scan : 1;

  boolean reserved : 6;
  /*! Last sent CPHY trans id */
  uint8 last_cphy_trans_id;

  /*! ML1 timing info to be included only for acq on conn rel*/
  uint64 ref_time;

  /*! flag to record full band interleave scan when oos timer expire.*/
  boolean full_band_priority_scan;
} nb1_rrc_csp_current_scan_info_s;

/*! @brief This structure defines the contents of each entry in the Acq DB
*/
typedef struct
{
  /*! PLMN List from SIB1 */
  lte_rrc_plmn_list_s plmn_list;

  /*! DL Center frequency */
  nb1_earfcn_t dl_earfcn;

  /*! Band */
  sys_sband_lte_e_type band;

  /*! Physical cell id of last camped cell */
  nb1_phy_cell_id_t phy_cell_id;

  /*! Global cell id (from SIB1) of last camped cell */
  lte_rrc_global_cell_id_t cell_identity;

  /*! Tracking area code of last camped cell */
  uint16 tac;

  /*! SIB db index of last camped cell */
  uint8 sib_db_key;

  /*! PLMN selected for camping. For limited service, it is the first PLMN in
      the SIB1 list */
  lte_rrc_plmn_s selected_plmn;

  boolean attachwithoutpdn_connectivity;

  /*! Index of PLMN selected for camping in the SIB1 list. For limited service,
      it is 0 */
  uint32 selected_plmn_index;

  /* q_rxlevmin received in SIB1 */
  int16 q_rxlevmin;
}nb1_rrc_csp_acq_entry_s;

/*! @brief CSP Acq DB structure
*/
typedef struct
{
   /*! Acquisition Database */
  nb1_rrc_csp_acq_entry_s db[NB1_RRC_CSP_MAX_ACQ_DB_EXT_ENTRIES];

  /*! Virtual list to access the Acq DB entries. Indices are sorted by age */
  uint32 acq_db_index[NB1_RRC_CSP_MAX_ACQ_DB_EXT_ENTRIES];

  /*! Indicates the number of items stored in acq list order. If value is
  *   NB1_RRC_CSP_MAX_ACQ_DB_ENTRIES, it indicates that the Acq List is
  *  full and an old entry needs to be cleared to make room for the new one
  */
  uint32 acq_db_count;

  /*! Is the corresponding array element of db occupied? */
  boolean entry_is_occupied[NB1_RRC_CSP_MAX_ACQ_DB_EXT_ENTRIES];
}nb1_rrc_csp_acq_db_s;

typedef struct
{
  /*! 0..65535 (allowing for expansion) */
  nb1_earfcn_t  dl_earfcn;

  /*! PCI */
  nb1_phy_cell_id_t pci;

}nb1_rrc_csp_pci_lock_s;

typedef struct
{
  /*! 0..65535 (allowing for expansion) */
  nb1_earfcn_t  dl_earfcn1;

  /*! 0..65535 (allowing for expansion) */
  nb1_earfcn_t  dl_earfcn2;

}nb1_rrc_csp_earfcn_lock_s;

/*! @brief The structure for cell lock
*/
typedef struct
{
  uint8 num_cell_id;

  lte_rrc_cell_id_s cell_id_list[LTE_RRC_MAX_NUM_CELL_LOCK];

} nb1_rrc_csp_cell_lock_s;

/*! @brief This structure holds the NAS request contents
*/
typedef struct
{
  /*! Transaction id from NAS */
  uint32 trans_id;

  /*! network select mode */
  sys_network_selection_mode_e_type network_select_mode;

  /*! request type*/
  lte_rrc_service_req_type_e req_type;

  /* Service Request triggered cause (NAS/SHIM) */
  service_req_cause_e req_cause;

  /*! scan type*/
  sys_scan_scope_e_type scan_scope;

  /*! Requested PLMN */
  lte_rrc_plmn_s req_plmn;

  /*! Is requested PLMN valid */
  boolean req_plmn_info_is_valid;

  /*! Is registered PLMN info available?  */
  boolean rplmn_info_is_valid;

  /*! Registered PLMN */
  lte_rrc_plmn_s rplmn;

  /*! Is HPLMN info available? */
  boolean hplmn_info_is_valid;

  /*! HPLMN */
  lte_rrc_plmn_s hplmn;

  /*! EHPLMN list */
  lte_rrc_ehplmn_list_s ehplmn_list;

  /*! Forbidden TA list */
  lte_rrc_tracking_area_list_s forbidden_ta_list;

  /*!< Forbidden TA list in Manual mode */
  lte_rrc_tracking_area_list_s forbidden_manual_ta_list;

  sys_rat_pri_list_info_s_type rat_pri_list; /*!< RAT priority list */

  /*!< EPLMN list - list of PLMNS equivalent to RPLMN if rplmn_info_is_valid
       is TRUE */
  lte_rrc_plmn_list_s eplmn_list;

  /*! service request timeout duration */
  uint32 t_timeout;

  /*!Boolean to give priority to EHPLMN/HPLMN to RPLMN*/
  boolean ehplmn_camping_allowed;

  /*!< Is service request new or not */
  boolean new_scan;  

  /*!< True = use timer (Interlace scan), False = don't use timer  (Non- Interlace scan) */ 
  boolean use_search_timer; 

  /*!< Timer (in ms) for which LTE band scan runs before puncturing ACQ_DB scans*/
  uint32 lte_search_timer ;  

  /*!< boolean to indicator whether snr level 2 system scan timer expired or not */
  boolean snr2_acq_db_timer_expired;

  /*!< boolean to indicator whether snr level 2 band scan timer expired or not */
  boolean snr2_band_scan_timer_expired;

  /*!< boolean to indicator whether it is power on case or not */
  boolean ue_is_poweron;
  
  /*!< boolean indicates enhanced coverage is restricted or not */
  boolean ce_restricted;

  #ifdef  FEATURE_FMD_SPEED_INFO
  /*!< boolean to relay additional info for pseudo online mode */
  boolean pseudo_online_mode;
  #endif
}nb1_rrc_csp_nas_req_info_s;

/* Type for SIB bitmask */
typedef uint32 nb1_rrc_sib_bitmask_t;

/* Type for Timer Struct*/
typedef struct nb1_rrc_timer_struct
{
  timer_type timer_type_inst;

}nb1_rrc_timer_type;

/* Type for Timer CB*/
typedef unsigned long    nb1_rrc_timer_cb_data_type;

/*! @brief Data structure for RRC timer
*/
typedef struct
{
  nb1_rrc_timer_type timer; /*!< MCS timer */
  msgr_umid_type umid; /*!< Message ID to be posted in callback */
} nb1_rrc_tmr_s;

/*! @brief Enumeration of Cap IEs that need EFS support
*/
typedef enum
{
  /* Disable IEs Bitmask */
  NB1_RRC_CAP_TM8_TDD = 1,
  NB1_RRC_CAP_INTRE_FREQ_GAPLESS_MEAS_SUPPORT_FDD = 2,
  NB1_RRC_CAP_DISABLE_IE_OTDOA_RSTD_TDD = 3,
  NB1_RRC_CAP_DISABLE_IE_OTDOA_RSTD_FDD = 4,
  NB1_RRC_CAP_DISABLE_IE_MAX = 64,
  /* Enable IEs Bitmask, Should start with 65 */
  NB1_RRC_CAP_NON_CA_BAND_SUPPORT = 65,
  NB1_RRC_CAP_INTRE_FREQ_GAPLESS_MEAS_SUPPORT_TDD = 66,
  NB1_RRC_CAP_ENABLE_IE_UTRA_PROXIMITY_IND = 67,
  NB1_RRC_CAP_ENABLE_IE_EUTRA_PROXIMITY_IND = 68,
  NB1_RRC_CAP_ENABLE_IE_SSF9 = 69,
  NB1_RRC_CAP_ENABLE_IE_3DL_TDD_CA = 70,
  NB1_RRC_CAP_ENABLE_IE_MAX = 128
} nb1_rrc_cap_ies_e;


/*! @brief NB1_RRC_PROC_E - Specifies the RRC Procedures
*/
typedef enum
{
  NB1_RRC_PROC_CSP, /*!< 0 Cell Selection Procedure */
  NB1_RRC_PROC_SIB, /*!< 1 RRC SIB Procedure */
  NB1_RRC_PROC_LLC, /*!< 2 RRC Lower Layer Controller Procedure */
  NB1_RRC_PROC_CEP, /*!< 3 RRC Connection Establishement Procedure */
  NB1_RRC_PROC_CONFIG, /*!< 4 RRC Connection Reconfiguration Procedure */
  NB1_RRC_PROC_PG, /*!< 5 RRC Paging Procedure */
  NB1_RRC_PROC_CRP, /*!< 6 RRC Connection Release Procedure */
  NB1_RRC_PROC_MH, /*!< 7 RRC Message Handler Procedure */
  NB1_RRC_PROC_MEAS, /*!< 8 RRC Measurement Procedure */
  NB1_RRC_PROC_SEC, /*!< 9 RRC Security Procedure */
  NB1_RRC_PROC_CRE, /*!< 10 RRC Connection Re-establishment Procedure */
  NB1_RRC_PROC_CONTROLLER, /*!< 11 RRC Controller Procedure */
  NB1_RRC_PROC_IRAT, /*!< 12 IRAT Procedures */
  NB1_RRC_PROC_UEINFO, /*!< 15 UEInfo procedure */
  NB1_RRC_PROC_PLMN, /*!< 16 PLMN procedure */
  NB1_RRC_PROC_MAX /*!< MAX - for initialization and range check purpose */
} nb1_rrc_proc_e;

/*! @brief Enumeration for cell type
*/
typedef enum
{
  NB1_RRC_CELL_TYPE_NONE = 0, /*!< 0 Cell Type NONE */
  NB1_RRC_CELL_TYPE_PCELL,     /*!< 0 Cell Type NONE */
  NB1_RRC_CELL_TYPE_SCELL     /*!< 0 Cell Type NONE */
} nb1_rrc_cell_type_e;


/*! @brief Enumeration for mode field in Mode Change Request
*/
typedef enum
{
  NB1_RRC_MODE_CHANGE_ACTIVATE, /*!< 0 */
  NB1_RRC_MODE_CHANGE_DEACTIVATE, /*!< 1 */
  NB1_RRC_MODE_CHANGE_SUSPEND, /*!< 2 */
  NB1_RRC_MODE_CHANGE_RESUME, /*!< 3 */
  NB1_RRC_MODE_CHANGE_MAX /*!< 4 */
} nb1_rrc_mode_change_e;

/*! @brief Enumeration for destination scenario field in Mode Change Request for
    activate and suspend mode change options
*/
typedef enum
{
  NB1_RRC_ACTIVATED_DUE_TO_NAS_REQUEST, /*!< 0 */
  NB1_RRC_ACTIVATED_DUE_TO_IRAT_TO_NB1, /*!< 1 */
  NB1_RRC_SUSPENDED_DUE_TO_RLF, /*!< 2 */

  NB1_RRC_SUSPENDED_DUE_TO_CAMPED_PLMN_SRCH_FROM_NB1, /*!< 3 */
  NB1_RRC_SUSPENDED_DUE_TO_IDLE_NOT_CAMPED_PLMN_SRCH_FROM_NB1, /*!< 4 */
  NB1_RRC_SUSPENDED_DUE_TO_TX_UNAVAILABILITY, /*!< 5 */
} nb1_rrc_dest_scenario_e;

/*! @brief NB1_RRC_GET_SIBS_CAUSE_E - Indicates the cause/trigger of get_sibs_req
*/
typedef enum
{
  NB1_RRC_GET_SIBS_CAUSE_CELL_SEL,  /*!< 0 Cell Selection */
  NB1_RRC_GET_SIBS_CAUSE_CELL_RESEL,  /*!< 1 Cell Reselection */
  NB1_RRC_GET_SIBS_CAUSE_SIB_UPDATE,  /*!< 2 Paged for SIB update */
  NB1_RRC_GET_SIBS_CAUSE_24HR_TMR, /*!< 3 3hr timer expiry */
  NB1_RRC_GET_SIBS_CAUSE_HO_COMPLETE, /*!< 4 Handover Complete */
  NB1_RRC_GET_SIBS_CAUSE_ETWS, /*!< 5 ETWS */
  NB1_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL, /*!< 6 Going from Connected to Idle on the same cell */
  NB1_RRC_GET_SIBS_CAUSE_NEXT_MOD_PRD_VALUE_TAG_CHECK, /*!< 7 All SIBs read, but SIB1 to be read in next mod prd to compare value tag */
  NB1_RRC_GET_SIBS_CAUSE_RESEL_FAIL, /*!< 8 Reselection failed, going back to old serving cell */
  NB1_RRC_GET_SIBS_CAUSE_SIB8_TIMING_INFO, /*!< 9 Read SIB8 to get timing info */
  NB1_RRC_GET_SIBS_CAUSE_CMAS, /*!< 10 CMAS */
  NB1_RRC_GET_SIBS_CAUSE_SIB16_TIMING_INFO, /*!< 12 Read SIB16 to get UTC timing info */
  NB1_RRC_GET_SIBS_CAUSE_RF_RECOVERY, /*!< 13 After RF_recovery re-reading SIB1 to confirm value-tag */
  NB1_RRC_GET_SIBS_CAUSE_DCCH_SIB1, /*!< 14 DCCH  SIB1 signalling*/
  NB1_RRC_GET_SIBS_CAUSE_EAB, /*!< Get SIB14 for access barring check */
  NB1_RRC_GET_SIBS_CAUSE_EDRX_SIB_UPDATE,  /*!< eDRX sibs update*/
  NB1_RRC_GET_SIBS_CAUSE_EDRX_REFRESH, /*!< eDRX Sibs referesh*/
  NB1_RRC_GET_SIBS_CAUSE_CONN_EST_FAIL,
  NB1_RRC_GET_SIBS_CAUSE_MAX  /*!< MAX Invalid */
} nb1_rrc_get_sibs_cause_e;

/*! @brief NB1_RRC_CFG_TYPE_E - Indicates the configuration type
*/
typedef enum
{
  NB1_RRC_CFG_DED,             /*!< 0 CONFIG due to dec OTA msg - CONFIG and CEP */
  NB1_RRC_CFG_SRV_COMMON,      /*!< 1 Used for updating phy layer with common info of serving cell  */
  NB1_RRC_CFG_CONN_REL,        /*!< 2 Configuration due to RRC Conn Release - No data to fill in union */
  NB1_RRC_CFG_ABORT_ML1,       /*!< 3 Used for sending NB1_ML1_ABORT_REQ - No date to fill in union */
  NB1_RRC_CFG_SEC,             /*!< 4 Used for performing security config of PDCP
                                    (via PDCPUL_CFG_REQ and PDCPDL_CFG_REQ*/
  NB1_RRC_CFG_RLF,             /*!< 5 Used for aborting ML1 and suspending all RBs except SRB0 */
  NB1_RRC_CFG_DEACTIVATE_FS,   /*!< 6 Used for releasing all L1 and L2 resources */
  NB1_RRC_CFG_IRAT_HO,         /*!< 7 Used for doing the preparatory steps for IRAT HO */
  NB1_RRC_CFG_BAND_SCAN,       /*!< 9 Used for CSP to send band scan request to ML1*/
  NB1_RRC_CFG_SYSTEM_SCAN,     /*!< 10 Used for CSP to send system scan request to ML1*/
  NB1_RRC_CFG_ACQ,         /*!< 11 Used for CSP to send acquisition request to ML1*/
  NB1_RRC_CFG_MAX              /*!< MAX invalid - for initialization purpose */
} nb1_rrc_cfg_type_e;

/*! @brief NB1_RRC_CFG_FAILURE_TYPE_E - Indicates the configuration Failure type
*/
typedef enum
{
  NB1_RRC_CFG_FAILURE_NONE = 0,             /*!< 0 CONFIG FAILURE - NONE*/
  NB1_RRC_CFG_FAILURE_NORMAL,              /*!< 1 CONFIG FAILURE - NORMAL*/
  NB1_RRC_CFG_FAILURE_PHY_ABORT,         /*!< 2 CONFIG FAILURE - PHY ABORT*/
  NB1_RRC_CFG_FAILURE_CONN_REL,           /*!< 3 CONFIG FAILURE - CONN REL*/
  NB1_RRC_CFG_FAILURE_CPHY_STOP          /*!< 4 CONFIG FAILURE - CPHY STOP*/
} nb1_rrc_cfg_failure_type_e;

/*!
  This is an enumeration of all RNTIs used by RRC.
  */
typedef enum
{
  NB1_RRC_C_RNTI, /*!< 0 RNTI to decode dedicated UE data in connected mode */
  NB1_RRC_SI_RNTI_TYPE, /*!< 1 RNTI to identify System Information  */
  NB1_RRC_P_RNTI_TYPE,  /*!< 2 RNTI used to identify paging information */
  NB1_RRC_RNTI_MAX /*!< 3 MAX - for initialization and range check purpose */
} nb1_rrc_rnti_type_e;


/*! @brief NB1_CHAN_CFG_ACTION_E - Specifies action for a channel
*/
typedef enum
{
  NB1_RRC_CFG_ACTION_SETUP_ENABLE, /*!< 0 setup CHAN or enable RNTI */
  NB1_RRC_CFG_ACTION_TEARDOWN_DISABLE, /*!< 1 teardown CHAN or disable RNTI */
  NB1_RRC_CFG_ACTION_MAX /*!< 2 MAX - for initialization and range check purpose */
} nb1_rrc_cfg_action_e;

/*! @brief NB1_RRC_UL_MSG_STATUS_E - Status of a message sent on uplink
*/
typedef enum
{
  NB1_RRC_UL_MSG_ACK,  /*!< 0 Ack */
  NB1_RRC_UL_MSG_NACK,  /*!< 1 Nack */
  NB1_RRC_UL_MSG_MAYBE,  /*!< 2 May be */
  NB1_RRC_UL_MSG_STATUS_MAX  /*!< 3 Invalid */
} nb1_rrc_ul_msg_status_e;

/*! @brief NB1_RRC_GET_SIBS_STATUS_E - Status sent with get_sibs_cnf
*/
typedef enum
{
  NB1_RRC_GET_SIBS_SUCCESS,  /*!< 0 Success */
  NB1_RRC_GET_SIBS_FAILURE,  /*!< 1 Failure */
  NB1_RRC_GET_SIBS_BANDCHG_SUCCESS, /*!< 2 Sib retrieved new dl overlap frequency */
  NB1_RRC_GET_SIBS_STATUS_MAX  /*!< 3 Invalid */
} nb1_rrc_get_sibs_status_e;

/*! @brief Enum to represent camping status
*/
typedef enum
{
  /*! Not camped */
  NB1_RRC_STATUS_NOT_CAMPED,
  /*! Camped on suitable cell, all calls are allowed */
  NB1_RRC_STATUS_CAMPED_SUITABLE,
  /*! Camped on Acceptable cell, only emergency calls are allowed */
  NB1_RRC_STATUS_CAMPED_ACCEPTABLE,
  /*! MAX value */
  NB1_RRC_STATUS_MAX
} nb1_rrc_camped_status_e;

/*! @brief Enum to represent UL msg failure (nack/maybe) cause
*/
typedef enum
{
  NB1_RRC_UL_MSG_FAIL_CAUSE_MAX_TX,  /*!< 0 Maximum number of transmissions */
  NB1_RRC_UL_MSG_FAIL_CAUSE_HO,  /*!< 1 In a HO */
  NB1_RRC_UL_MSG_FAIL_CAUSE_RLF,  /*!< 2 In a RLF */
  NB1_RRC_UL_MSG_FAIL_CAUSE_CONN_RELEASE,  /*!< 3 Connection being released */
  NB1_RRC_UL_MSG_FAIL_CAUSE_MAX  /*!< 4 Invalid */
} nb1_rrc_ul_msg_fail_cause_e;

/*! @brief Enum to represent camped reason in Camped indication
*/
typedef enum
{
  NB1_RRC_CAMPED_CELL_SELECTION,               /*!< 0 */
  NB1_RRC_CAMPED_CONN_TO_IDLE,                 /*!< 1 */
  NB1_RRC_CAMPED_CONN_EST_FAILED,              /*!< 2 */
  NB1_RRC_CAMPED_CELL_RESELECTION,             /*!< 3 */
  NB1_RRC_CAMPED_CONN_TO_IDLE_INTRA_NB1_REDIR, /*!< 4 */
  NB1_RRC_CAMPED_CELL_SELECTION_RLF,           /*!< 5 */
} nb1_rrc_camped_reason_e;

/*! @brief Enumeration for sib read error cause
*/
typedef enum
{
  NB1_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT,      /*!< 0 */
  NB1_RRC_SIB_READ_ERROR_CAUSE_INVALID_SIBS,     /*!< 1 */
  NB1_RRC_SIB_READ_ERROR_CAUSE_MIB_CHANGED       /*!< 2 */
} nb1_rrc_sib_read_error_cause_e;

/*! @brief Enumeration describing state of ML1, either its running normally,
    or if its stopped.
*/
typedef enum
{

  /*!< ML1 successfully resumed/started */
  NB1_RRC_ML1_STATUS_NORMAL                          = 0,

  /*!< ML1 not resumed */
  NB1_RRC_ML1_STATUS_NOT_RESUMED                     = 1,

}nb1_rrc_ml1_status_type_e;


/*! @brief Enumeration of different causes for CRE failure cause
*/
typedef enum
{

  /*! Reestablishment not triggered since security is inactive */
  NB1_RRC_CRE_FAILURE_CAUSE_SEC_NOT_ACTIVE = 0,

  /*! Reestablishment not triggered because of flow control shutdown from RF */
  NB1_RRC_CRE_FAILURE_CAUSE_FC_SHUTDOWN,

  /*! Reestablishment not triggered because of PDCPUL Conn release */
  NB1_RRC_CRE_FAILURE_CAUSE_PDCPUL_CONN_REL,

  /*! Reestablishment not triggered because of SIB read error */
  NB1_RRC_CRE_FAILURE_CAUSE_SIB_READ_ERROR,

  /*! Reestablishment failed because of T311 expiry */
  NB1_RRC_CRE_FAILURE_CAUSE_T311_EXPIRY,

  /*! Reestablishment failed because of T301 expiry */
  NB1_RRC_CRE_FAILURE_CAUSE_T301_EXPIRY,

  /*! Reestablishment failed because of ReestablishmentReject */
  NB1_RRC_CRE_FAILURE_CAUSE_REEST_REJECT,

  /*! Reestablishment failed because of ReestablishmentReject */
  NB1_RRC_CRE_FAILURE_CAUSE_REEST_INVALID_CRNTI,

  /*! Reestablishment failed because of Reestablishment validation failure */
  NB1_RRC_CRE_FAILURE_CAUSE_REEST_VALIDATION_FAILURE,

  /*! Reestablishment failed because of SRB1 resume failure */
  NB1_RRC_CRE_FAILURE_CAUSE_SRB1_RESUME_FAILURE,

  /*! Reestablishment failed because of CPHY resets */
  NB1_RRC_CRE_FAILURE_CAUSE_CPHY_RESET,

  /*! Reestablishment failed because of RF unavailability*/
  NB1_RRC_CRE_FAILURE_CAUSE_CPHY_RF_UNAVAILABLE,

  /*! Max value */
  NB1_RRC_CRE_FAILURE_CAUSE_MAX

} nb1_rrc_cre_failure_cause_e;

/*! @brief Enumeration for SRB1 configuration source
*/
typedef enum
{
  NB1_RRC_CEP_USE_RRC_SETUP_FOR_SRB1,     /*!< 0 - ConnectionSetup OTA Config */
  NB1_RRC_CEP_USE_RRC_RESUME_FOR_SRB1,    /*!< 1 - ConnectionResume OTA Config */
  NB1_RRC_CEP_USE_SAVED_CONTEXT_FOR_SRB1, /*!< 2 - Configure SRB1 before sending ConnectionResumeRequest OTA */
} nb1_rrc_cep_SRB1_config_data_source_e ;

typedef uint8 nb1_rrc_sib_db_key_t;

/*! @brief Structure for cell info
*/
typedef struct
{
  lte_rrc_cell_id_s cell_id;
  nb1_rrc_sib_db_key_t sib_db_key;
} nb1_rrc_cell_info_s;

/*! @brief Structure for Non-anchor DL Freq Info
*/
typedef struct
{
  /*!< can be of RANGE 0 to NB1_CPHY_MAX_NON_ANCHOR_CARRIER
       If 0, NO non-anchor carriers and all DL are in anchor carrier */
  uint8 num_non_anchor;

  /*! Non-anchor DL Frequency */
  nb1_earfcn_t freq[NB1_CPHY_MAX_NON_ANCHOR_CARRIER];
} nb1_rrc_non_anchor_dl_freq_info_s;

/*! @brief Structure for dedicated RLF timers maintained by RRC
*/
typedef struct
{
  uint32 t301;  /*!< Timer T301 value */
  uint32 t311;  /*!< Timer T311 value */

} nb1_rrc_rlf_tmr_s;

/*! @brief Structure for dedicated RLF timers
*/
typedef struct
{
  /*! If dedicated RLF timers and constants are enabled */
  boolean enable;

  /*! If default values are used for RLF timers and constants */
  boolean default_values_used;

  /*! Dedicated RLF timers and constant params that are passed to ML1 */
  nb1_cphy_rlf_tmr_and_const_dedicated_param_s cphy_tmr_and_const;

  /*! Dedicated RLF timers that are maintained by RRC */
  nb1_rrc_rlf_tmr_s rrc_tmr_params;

} nb1_rrc_dedicated_rlf_timers_s;

/*! @brief Enumeration for msg type
*/
typedef enum
{
  NB1_RRC_UL_MSG_TYPE_NONE = 0,        /*!< 0 - Used by all modules */
} nb1_rrc_ul_msg_type_e;

/*! @brief Data structure for Send UL Msg Request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  void *ul_msg_ptr; /*!< Message to be sent */
  nb1_rb_id_t rb_id; /*!< Radio bearer id */
  boolean ack_needed; /*!< Ack needed or not*/
  uint8 *mu_id; /*!< Message unit identifier - for book keeping */
  uint8 *encode_status; /*!< Set by MH - 0 means SUCCESS */
  nb1_rrc_ul_msg_type_e msg_type; /*!< UL msg type */
  boolean esm_data_transport; /*!< TRUE if data is for ESM (Extended Session Mgmt) from NAS */   
} nb1_rrc_send_ul_msg_reqi_s;

/*! @brief Data structure for Mode Change Request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /* mandatory field */
  nb1_rrc_mode_change_e mode; /*!< Requested mode */
  /* fields below are only looked at in specific cases and are don't care for
     the rest */
  nb1_rrc_dest_scenario_e dest_scenario; /*!< Requested destination scenario if
          mode = NB1_RRC_MODE_CHANGE_ACTIVATE or NB1_RRC_MODE_CHANGE_SUSPEND */

  sys_lte_band_mask_e_type  nb1_band_pref; //Populated only if Mode change is for activate
} nb1_rrc_mode_change_reqi_s;

/*! @brief Data structure for Get SIBs request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_cell_id_s cell_id; /*!< Cell identity */
  nb1_rrc_proc_e req_proc; /*!< Requesting procedure */
  nb1_rrc_get_sibs_cause_e cause; /*!< Cause/trigger for the request */
  boolean cnf_needed; /*!< CNF needed or not */
  nb1_sfn_s sfn; /*!< (To be used by Paging only) SFN in which paging message
                         was received */
  /*! Hyper SFN value when indication is sent */
  nb1_l1_hyper_sfn_t   hyper_sfn;
  /*! SIB1 BR sch info if available; 0 if not available */
  uint8 sib1_br_sch_info;

  boolean presence_of_mib; //Set to TRUE if MIB is part of cell selection
  
  /** ASN.1-decoded MIB */
  nb1_l1_nb_mib_info_param_s mib;

  /** SFN on which MIB was received and decoded on PBCH */
  nb1_l1_sfn_t               mib_sfn;

} nb1_rrc_get_sibs_reqi_s;

/*! @brief Data structure for SIBs abort request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_cell_id_s cell_id; /*!< Cell identity */
} nb1_rrc_sib_abort_reqi_s;

/*! @brief Data structure for dedicated configuration info
*/
typedef struct
{
  nb1_rrc_cep_SRB1_config_data_source_e srb1_config_src;
  void* msg_ptr; /*!< ASN.1 decoded msg pointer to CONFIG or CEP  */
} nb1_rrc_llc_ded_cfg_s;

/*! @brief Enumeration for cell selection cause
*/
typedef enum
{
  NB1_RRC_INITIATE_CELL_SEL_RL_FAILURE,     /*!< 0 */
  NB1_RRC_INITIATE_CELL_SEL_RLF_RF_UNAVAIL, /*!< 1 */
  NB1_RRC_INITIATE_CELL_SEL_DMM_SUSPEND,    /*!< 2 */
  NB1_RRC_INITIATE_CELL_SEL_DMM_RESUME,  /*!< 3 */
  NB1_RRC_INITIATE_CELL_SEL_RL_FAILURE_NS4,
  NB1_RRC_INITIATE_CELL_SEL_MAX             /*!< 4 */
} nb1_rrc_initiate_cell_sel_cause_e;

/*! @brief Enmueration for barring indication
*/
typedef enum
{
  NB1_RRC_NO_BARRING, /*!< 0*/
  NB1_RRC_SHORT_DURATION_BARRING, /*! 1*/
  NB1_RRC_LONG_DURATION_BARRING, /*! 2*/
  NB1_RRC_NS4_BARRING
}nb1_rrc_barring_type_e;

/*! @brief Data structure for Initiate Cell Selection request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  nb1_rrc_initiate_cell_sel_cause_e cause; /*!< Reason for initiating cell selection */
  nb1_rrc_barring_type_e bar_type;/*! Decide whether or not to bar cell for a certian duration*/
} nb1_rrc_initiate_cell_sel_reqi_s;

/*! @brief Data structure for Cell Selection Abort request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_csp_cell_select_abort_reqi_s;

/*! @brief Data structure for Start Internal Cell Selection request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_start_internal_cell_selection_reqi_s;

/*! @brief Data structure for PLMN Search Abort request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_plmn_search_abort_reqi_s;

/*! @brief Enumeration of nB values (needed to identity paging group)
*/
typedef enum
{
  NB1_RRC_NB_4T,    /*!< 0 */
  NB1_RRC_NB_2T,    /*!< 1 */
  NB1_RRC_NB_1T,    /*!< 2 */
  NB1_RRC_NB_T2,    /*!< 3 */
  NB1_RRC_NB_T4,    /*!< 4 */
  NB1_RRC_NB_T8,    /*!< 5 */
  NB1_RRC_NB_T16,   /*!< 6 */
  NB1_RRC_NB_T32,   /*!< 7 */
  NB1_RRC_NB_T64,   /*!< 8 */
  NB1_RRC_NB_T128,   /*!< 9 */
  NB1_RRC_NB_T256,   /*!< 10 */
  NB1_RRC_NB_T512,   /*!< 11 */
  NB1_RRC_NB_T1024,   /*!< 12 */
  NB1_RRC_NB_MAX
} nb1_rrc_nb_e;

/*! @brief Enumeration for Config failure reasons
*/
typedef enum
{
  NB1_RRC_CONN_MODE_FAILURE_CAUSE_HO,           /*!< 0 - Handover failure due to RACH*/
  NB1_RRC_CONN_MODE_FAILURE_CAUSE_INVALID_CFG,      /*!< 1 - Invalid configuration */
  NB1_RRC_CONN_MODE_FAILURE_CAUSE_SIB_READ_FAILURE, /*!< 2 - SIB read failure */
  NB1_RRC_CONN_MODE_FAILURE_CAUSE_IP_CHECK_FAILURE,  /*!< 3 - IP check failure */
  NB1_RRC_CONN_MODE_FAILURE_CAUSE_SMC_FAILURE,   /*!< 4 - SMC Failure*/
  NB1_RRC_CONN_MODE_FAILURE_CAUSE_FULL_BUFFER,  /*!< 5 - MH UL MSG buffer is full */
  NB1_RRC_CONN_MODE_FAILURE_CAUSE_RESUME_FAILURE, /*!< 5 - MH UL MSG buffer is full */
  NB1_RRC_CONN_MODE_FAILURE_CAUSE_MIB_CHG, /*!< 6 - mib info changed */
  NB1_RRC_CONN_MODE_FAILURE_CAUSE_SUSPEND_FAILURE, /*!< 7 - Suspend Failures */
  NB1_RRC_CONN_MODE_FAILURE_CAUSE_OTHER,        /*!< 8 - Other Failures */
  NB1_RRC_CONN_MODE_FAILURE_CAUSE_DMM_SUSPEND,
  NB1_RRC_CONN_MODE_FAILURE_CAUSE_INVALID_CFG_NS4, 
  NB1_RRC_CONN_MODE_FAILURE_CAUSE_MAX          /*!< 9 Invalid */
} nb1_rrc_conn_mode_failure_cause_e;

/*! @brief enumeration to identify the cause of Connection Reestablishment
    ****************************************************************
    * Always append new ENUM cause and add this new ENUM cause in  *
    * lte_rrc_cre_cause_e ENUM definition as well, To log correct  *        
    * RLF cause in EVENT_LTE_RRC_IOT_RADIO_LINK_FAILURE_STAT event *
    ****************************************************************
*/
typedef enum
{
  NB1_RRC_CRE_CAUSE_CFG_FAILURE,      /*!< 0 - Reconfig Failure from Config module */
  NB1_RRC_CRE_CAUSE_HO_FAILURE,       /*!< 1 - HO Failure from Config module.Added here only to correct the enum parsing */
  NB1_RRC_CRE_CAUSE_RLF,              /*!< 2 - T310 expiry from CPHY */
  NB1_RRC_CRE_CAUSE_RACH_PROBLEM,     /*!< 3 - RACH problem indication from MAC */
  NB1_RRC_CRE_CAUSE_MAX_RETRX,        /*!< 4 - Maximum Retransmissions indication from RLC */
  NB1_RRC_CRE_CAUSE_IP_CHECK_FAILURE, /*!< 5 - IP Check Failure from MH module */
  NB1_RRC_CRE_CAUSE_SIB_READ_FAILURE, /*!< 6 - SIB Read Failure from CSP module */
  NB1_RRC_CRE_CAUSE_OTHER_FAILURE,    /*!< 7 - Any other Failure causing RLF */
  NB1_RRC_CRE_CAUSE_MAX,              /*!< 8 - Invalid */
  NB1_RRC_CRE_CAUSE_MIB_CHG,          /*!< 9 - mib info changed in Connected Mode */
  NB1_RRC_CRE_CAUSE_RLF_RF_UNAVAILABLE, /*!< 10 - DSDS RL failure due to RF unavailability */
  NB1_RRC_CRE_CAUSE_PDCPUL_RL_FAILURE,  /*!< 11 - PDCPUL RL Fail Ind from L2 */
  NB1_RRC_CRE_CAUSE_L2_RL_FAILURE,      /*!< 12 -RL Fail Ind from L2 */
  NB1_RRC_CRE_CAUSE_ELS,                /*!< 13 -RLF due to ELS*/
  NB1_RRC_CRE_CAUSE_RESUME_ON_LTE_FAILURE, /*!< 14 - Resume on lte failure */
  NB1_RRC_CRE_CAUSE_SUSPEND_LTE_FAILURE,   /*!< 15 - Suspend LTE failure */
  NB1_RRC_CRE_CAUSE_DMM_SUSPEND,           /*!< 16 - DMM Suspend LTE failure */
  NB1_RRC_CRE_CAUSE_CFG_FAILURE_NS4,       /*!< 17 - Failure due to Security is not activated */
  NB1_RRC_CRE_CAUSE_LOCAL_RELEASE,         /*!< 18 - Local Release by MAC */  
} nb1_rrc_cre_cause_e;

/*! @brief Enumeration for HO failure reasons
*/
typedef enum
{
  NB1_RRC_HO_FAILURE_CAUSE_NONE,    /*!< 0 - Cause not applicable */
  NB1_RRC_HO_FAILURE_CAUSE_INVALID_CFG,    /*!< 1 - Invalid configuration  */
  NB1_RRC_HO_FAILURE_CAUSE_CPHY,         /*!< 2 - Handover failure due to ML1 */
  NB1_RRC_HO_FAILURE_CAUSE_RACH,    /*!< 3 - Handover failure due to RACH */
  NB1_RRC_HO_FAILURE_CAUSE_RACH_MEAS,  /*!< 4 - Handover with Meas, failure due to RACH */
  NB1_RRC_HO_FAILURE_CAUSE_IRAT,      /*!< 5 - Handover with Meas, failure due to RACH */
  NB1_RRC_HO_FAILURE_CAUSE_MAX          /*!< 6 Invalid */
} nb1_rrc_ho_failure_cause_e;

/*! @brief nb1 catm1 cell type
*/
typedef enum
{
  NB1_RRC_CELL_TYPE_NB1,    /*!< 0 - NB1 cell */
  NB1_RRC_CELL_TYPE_CATM1,    /*!< 1 - CATM1 cell */
  NB1_RRC_CELL_TYPE_LTE         /*!< 2 - LTE cell */
} nb1_rrc_cell_type_event_e;

/*! @brief Enumeration for RRC OOS Indi reasons
*/
typedef enum
{
  NB1_RRC_OOS_IND_CAUSE_IM3_BACKOFF,    /*!< 0 - OOS due to IM3 backoff */
  NB1_RRC_OOS_IND_CAUSE_MIB_CHG,    /*!< 1 - OOS due to MIB change in idle mode  */
  NB1_RRC_OOS_IND_CAUSE_CONN_CONTROL,    /*!< 2 - OOS due to Conn Control criteria successful */
  NB1_RRC_OOS_IND_CAUSE_INVALID_NS4_CONFIG, /*!< 3 - OOS due to NS4 invalid config */
  NB1_RRC_OOS_IND_CAUSE_MAX         /*!< 3 Invalid */
} nb1_rrc_oos_ind_cause_e;

/*! @brief Data structure for dedicated configuration info
*/
typedef struct
{
  boolean sib1_update; /*!< If set to true, need to update L1 with S-criteria */
  boolean sib2_update;  /*!< If set to true, need to update L1 with common Chan config */
  boolean sib22_update;  /*!< If set to true, need to update L1 with common Chan config */
  boolean sib22_removal_update;  /*!< If set to true sib22 is removed, need to update L1 with common Chan config */
  boolean default_cfg;  /*!< If set to true, need to update L1 with dedicated default cfg */
  boolean forced_common_cfg; /*!< If set to true, forcefully update L1 with common Chan config */
} nb1_rrc_llc_common_cfg_s;


/*! @brief Data structure for Security configuration info
*/
typedef struct
{
  uint8 key_index_rrc_int;  /*!< Key index for the RRC integrity Key*/
  uint8 *key_rrc_int_ptr;/*!< RRC integrity Key ptr*/
  uint8 key_index_rrc_enc; /*!< Key index for the RRC ciphering key*/
  uint8 *key_rrc_enc_ptr; /*!< RRC ciphering key*/
  uint8 key_index_up_enc;/*!< Key index for User plane ciphering key*/
  uint8 *key_up_enc_ptr;/*!< User plane ciphering key ptr*/
  nb1_cipher_algo_e cipher_algo;/*!< Ciphering Algo*/
  nb1_integrity_algo_e integrity_algo;/*!< Integrity Algo*/
}nb1_rrc_sec_cfg_s;

/*! @brief Data structure for suspension configuration info
*/
typedef struct
{
  boolean revert_l2_config; /*!< if L2 needs to be reveted to old config */
} nb1_rrc_llc_susp_cfg_s;

/*! @brief Data structure for release configuration info
*/
typedef struct
{
  boolean save_cfg_context;
  nb1_rrc_ml1_status_type_e ml1_status; /*!< ml1 state */
} nb1_rrc_llc_rel_cfg_s;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct
{
  nb1_rrc_cfg_type_e  cfg_type;  /*!< This will indicate the configuration type */
  union {
    nb1_rrc_llc_ded_cfg_s  ded_cfg; /*!< Can be used for recfg & CEP, includes msg_ptr */
    nb1_rrc_llc_common_cfg_s  common_cfg; /*!< Can be used to update common info sib1/sib2/drx params*/
    nb1_rrc_llc_susp_cfg_s susp_cfg; /*!< Can be used to indicate the cause for RLF */
    nb1_rrc_llc_rel_cfg_s  rel_cfg; /*!< Can be used to indicate if ML1 status is resumed or not */
    nb1_cphy_band_scan_req_s band_scan_cfg; /*!< can be used for sending band scan request to ML1*/
    nb1_cphy_system_scan_req_s system_scan_cfg; /*!< can be used for sending system scan request to ML1*/
    nb1_cphy_acq_req_s acq_req_cfg; /*!< can be used for sending acquisitoon request to ML1*/
  }cfg_u;
} nb1_rrc_cfg_info_s;


/*! @brief Data structure for redirection info
*/
typedef struct
{
  lte_rrc_redir_rat_type_e rat_type; /*!< RAT Type */
  sys_band_class_e_type band; /*!< Band Indicator */
  nb1_carrier_freq_s dl_arfcn; /*!< DL Frequency */
} nb1_rrc_redir_info_s;

/*! @brief Data structure for Config request
*/
typedef struct
{
  msgr_hdr_s msg_hdr;       /*!< Message router header */
  nb1_rrc_proc_e proc;      /*!< Procedure ID of the initiating module */
  uint32 *trans_id_ptr;     /*!< transaction ID to pair the CNF_CNFI with CNF_REQI. */
                            /*!< IMPORTANT NOTE: This has to be set to 0xFF by
                              all RRC modules before sending the request to LLC.
                              Else, the CFG_REQI will not be handled correctly */
  boolean cnfi_required;    /*!< Field to indicate if CFG_CNFI is needed by Proc */
  uint32 num_cfg;           /*!< Number of cfg info */
  nb1_rrc_cfg_info_s cfg_info[NB1_RRC_MAX_CFG_INFO]; /*!< Payload required for configurations */
} nb1_rrc_cfg_reqi_s;

/*! @brief Data structure for pend Config request
*/
typedef struct
{
  msgr_hdr_s msg_hdr;           /*!< Message router header */
  nb1_rrc_cfg_reqi_s cfg_reqi;  /*!< cfg_reqi payload when pended */
  uint32 trans_id;              /*!< transaction ID to pair the CNF_CNFI with CNF_REQI. */
} nb1_rrc_pended_cfg_reqi_s;


/*! @brief Enumeration of different causes for meas config
*/
typedef enum
{
  /*! Normal connection reconfiguration scenario */
  NB1_RRC_MEAS_CFG_REQI_CAUSE_NORMAL = 0,

  /*! Start of a handover */
  NB1_RRC_MEAS_CFG_REQI_CAUSE_HANDOVER,

  /*! Revert meas config to old, e.g. in case of handover failure */
  NB1_RRC_MEAS_CFG_REQI_CAUSE_REVERT_CONFIG,

  /*! Reapply current meas config, e.g. in case of RLF */
  NB1_RRC_MEAS_CFG_REQI_CAUSE_REAPPLY_CONFIG,

  /*! Max value */
  NB1_RRC_MEAS_CFG_REQI_CAUSE_MAX,

} nb1_rrc_meas_cfg_reqi_cause_e;


/*! @brief Enumeration for ML1 Stop status
*/
typedef enum
{
  NB1_CPHY_STOPPED          = 0,
  NB1_CPHY_ACTIVE           = 1,
  NB1_CPHY_PENDING_STOP_CNF = 2,
}nb1_rrc_ctlr_cphy_stop_status;

/*! @brief Data structure for Measurement Config request
*/
typedef struct
{
  msgr_hdr_s msg_hdr;                   /*!< Message router header */
  void *dl_dcch_ptr;                    /*!< Ptr to the reconfig dlm message */
  lte_rrc_cell_id_s cell_id;            /*!< Cell info */
  nb1_rrc_meas_cfg_reqi_cause_e cause;  /*!< Meas config cause */

} nb1_rrc_meas_cfg_reqi_s;

/*!
    @brief

    Internal RRC definition of common NAS related info needed for
    IRAT operations like redir to NB1 needed for camping.

    Has the internal RRC definition of PLMN.

    Also see nb1_rrc_irat_nas_info_s
*/
typedef struct
{
  /*! RAT priority list */
  sys_rat_pri_list_info_s_type        rat_pri_list;

  /*! TRUE if req_plmn field is present, FALSE otherwise */
  boolean                             req_plmn_info_is_valid;

  /*! Requested plmn (Internal RRC Type) */
  lte_rrc_plmn_s                      req_plmn;

  /*! Network select mode */
  sys_network_selection_mode_e_type   network_select_mode;

  /*! NAS Requested CSG Id */
  sys_csg_id_type                     csg_id;

} nb1_rrc_nas_info_s;

/*! @brief Data structure for controller to revoke ongoing suspend
*/
typedef struct
{
  msgr_hdr_s msg_hdr;
  nb1_rrc_proc_e req_proc; /*!< Requesting procedure */
}nb1_rrc_revoke_suspend_reqi_s;

/*! @brief Data structure for controller to revoke ongoing suspend

    If a connected mode procedure is in progress and gets aborted and
    ML1 was suspended, status is used to inform if ML1 succeeded in
    resuming or not
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  nb1_rrc_ml1_status_type_e status;
} nb1_rrc_revoke_suspend_cnfi_s;

/*! @brief Enumeration of REQI id nums
*/
typedef enum _nb1_rrc_reqi_type_e
{
  /*Message Handler*/
  NB_RRC_REQI_SEND_UL_MSG = 0x00,
  /*RRC Controller*/
  NB_RRC_REQI_MODE_CHANGE = 0x01,
  /*SIB*/
  NB_RRC_REQI_GET_SIBS = 0x02,
  NB_RRC_REQI_SIB_ABORT = 0x03,
  /*CEP*/
  /*CRP*/
  /*CSP*/
  NB_RRC_REQI_INITIATE_CELL_SEL = 0x04,
  NB_RRC_REQI_CSP_CELL_SELECT_ABORT =0x05,
  /*Config*/
  /*LLC*/
  NB_RRC_REQI_CFG = 0x06,
  /*Measurement*/
  NB_RRC_REQI_MEAS_CFG = 0x07,

  /* PLMN_Search */
  NB_RRC_REQI_PLMN_SEARCH_ABORT        =0x08,

  NB_RRC_REQI_ESMGR_ACT_TMGI                      = 0x09,
  NB_RRC_REQI_ESMGR_DEACT_TMGI                    = 0x0A,
  NB_RRC_REQI_ESMGR_AVAIL_TMGI_LIST               = 0x0B,
  /* LLC */
  NB_RRC_REQI_PENDED_CFG                          = 0x0C,
  NB_RRC_REQI_START_INTERNAL_CELL_SELECTION       = 0x0D,

  /* eMBMS - contd */
  NB_RRC_REQI_ESMGR_ACT_DEACT_TMGI                   = 0x0E,

  /* Misc */
  NB_RRC_REQI_SIB_EXTND_TMR                           = 0x0F,

  /* GNSS coex */
  NB_RRC_REQI_REVOKE_SUSPEND                          = 0x10,

  NB_RRC_REQI_MAX     /*!< Max value */
}nb1_rrc_reqi_type_e;

/*! @brief Internal requests that various RRC procedures receive
*/
enum
{
  /*Internal Requests*/

  NB1_RRC_REQI_FIRST = MSGR_UMID_BASE(MSGR_NB1_RRC, MSGR_TYPE_REQI),

  /*Message Handler*/

  MSGR_DEFINE_UMID(NB1, RRC, REQI, SEND_UL_MSG,
                   NB_RRC_REQI_SEND_UL_MSG,
                   nb1_rrc_send_ul_msg_reqi_s),

  /*RRC Controller*/
  MSGR_DEFINE_UMID(NB1, RRC, REQI, MODE_CHANGE,
                   NB_RRC_REQI_MODE_CHANGE,
                   nb1_rrc_mode_change_reqi_s),

  /*SIB*/

  MSGR_DEFINE_UMID(NB1, RRC, REQI, GET_SIBS,
                   NB_RRC_REQI_GET_SIBS,
                   nb1_rrc_get_sibs_reqi_s),
  MSGR_DEFINE_UMID(NB1, RRC, REQI, SIB_ABORT,
                   NB_RRC_REQI_SIB_ABORT,
                   nb1_rrc_sib_abort_reqi_s),

  /*CEP*/

  /*CRP*/

  /*CSP*/
   MSGR_DEFINE_UMID(NB1, RRC, REQI, INITIATE_CELL_SEL,
                    NB_RRC_REQI_INITIATE_CELL_SEL,
                  nb1_rrc_initiate_cell_sel_reqi_s),

   MSGR_DEFINE_UMID(NB1, RRC, REQI, CSP_CELL_SELECT_ABORT,
                    NB_RRC_REQI_CSP_CELL_SELECT_ABORT,
                    nb1_rrc_csp_cell_select_abort_reqi_s),

   MSGR_DEFINE_UMID(NB1, RRC, REQI, START_INTERNAL_CELL_SELECTION,
                    NB_RRC_REQI_START_INTERNAL_CELL_SELECTION,
                    nb1_rrc_start_internal_cell_selection_reqi_s),

  /*Config*/

  /*LLC*/
  MSGR_DEFINE_UMID(NB1, RRC, REQI, CFG, NB_RRC_REQI_CFG, nb1_rrc_cfg_reqi_s),
  MSGR_DEFINE_UMID(NB1, RRC, REQI, PENDED_CFG,
                   NB_RRC_REQI_PENDED_CFG, nb1_rrc_pended_cfg_reqi_s),

  /*Measurement*/
  MSGR_DEFINE_UMID(NB1, RRC, REQI, MEAS_CFG, NB_RRC_REQI_MEAS_CFG,
                   nb1_rrc_meas_cfg_reqi_s),

  /* PLMN_Search */
   MSGR_DEFINE_UMID(NB1, RRC, REQI, PLMN_SEARCH_ABORT, NB_RRC_REQI_PLMN_SEARCH_ABORT,
                    lte_rrc_plmn_search_abort_reqi_s),

  MSGR_DEFINE_UMID(NB1, RRC, REQI, SIB_EXTND_TMR, NB_RRC_REQI_SIB_EXTND_TMR,
                   nb1_rrc_sib_extnd_tmr_reqi_s),

  MSGR_DEFINE_UMID(NB1, RRC, REQI, REVOKE_SUSPEND, NB_RRC_REQI_REVOKE_SUSPEND,
                   nb1_rrc_revoke_suspend_reqi_s),

  NB1_RRC_REQI_MAX,
  NB1_RRC_REQI_LAST = NB1_RRC_REQI_MAX - 1

};

/*! @brief Data structure for UL msg Confirmation
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  uint8 mu_id; /*!< Message unit identifier */
  nb1_rrc_ul_msg_status_e status; /*!< Response (ack/nack/maybe) */
  nb1_rrc_ul_msg_fail_cause_e fail_cause; /*!< Failure cause (used by DT in
                                               case of nack/maybe) */
} nb1_rrc_ul_msg_cnfi_s;

/*! @brief Data structure for Mode Change Confirmation
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  boolean status; /*!< TRUE = success, FALSE = failure */
  nb1_rrc_mode_change_e mode; /*!< Mode in the request */
} nb1_rrc_mode_change_cnfi_s;

/*! @brief Data structure for storing overlapping frequencies populated from sib1
*/
typedef struct
{
  uint8 num_freqs;/*!< number of overlapping frequencies*/
  nb1_earfcn_t new_dl_freq[NB1_CPHY_MAX_OVERLAP_BANDS]; /*!< list of converted EARFCN of overlapping bands*/
} nb1_rrc_earfcn_list;

/*! @brief Data structure for Get SIBS Confirmation
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  nb1_rrc_get_sibs_status_e status; /*!< Status */
  nb1_rrc_cell_info_s cell_info; /*!< Cell info */
  nb1_rrc_earfcn_list new_dl_freq_list; /*!< List of converted EARFCNs of overlapping bands*/
} nb1_rrc_get_sibs_cnfi_s;

/*! @brief Data structure for SIB1 Avail Indi
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router Header */
  lte_rrc_cell_id_s cell_id; /*!< Cell Id */
  uint8 mfbi_index; /*!< Index corresponding to band chosen in multiBandInfoList */
  boolean band_change_success; /*!< Denotes band change success after receiving sib1*/
  nb1_rrc_osys_SystemInformationBlockType1_NB *sib1_ptr; /*!< Sib1 decoded ptr for populating parameters in service Indi */
  nb1_rrc_earfcn_list new_dl_freq_list; /*!< List of converted EARFCNs of overlapping bands*/
}nb1_rrc_sib1_avail_indi_s;


/*! @brief Data structure for Config Confirmation
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  nb1_rrc_proc_e proc; /*!< Procedure ID for which this REQI is meant for */
	nb1_rrc_cfg_type_e cfg_type;  /*!< This will indicate the configuration type */
  uint32 trans_id;  /*!< transaction ID to pair the CNF_CNFI with CNF_REQI. */
  IxErrnoType status; /*!< status field to indicate the status of cfg req */
  nb1_rrc_cfg_failure_type_e failure_type; /*!< Failure type applicable when status != E_SUCCESS */
} nb1_rrc_cfg_cnfi_s;

/*! @brief Data structure for Measurement Config Confirmation
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  nb1_errno_e status; /*!< status field to indicate status of meas cfg req */

} nb1_rrc_meas_cfg_cnfi_s;

/*! @brief Data structure for Cell Selection Abort Confirmation
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

} nb1_rrc_csp_cell_select_abort_cnfi_s;

/*! @brief Message for PLMN Search Confirmation
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_plmn_search_status_e status; /*!< Status of search */

  nb1_detected_macro_cell_list_type *detected_cell_list_ptr;  /*!< Detected Macro cells for fingerprinting */

} nb1_rrc_plmn_search_cnfi_s;

/*! @brief Enumeration of CNFI id nums
*/
typedef enum _nb1_rrc_cnfi_type_e
{
  /*Message Handler, please don't change the value */
  NB_RRC_CNFI_RRC_RECONFIGURATION_COMPLETE = 0x0,
  NB_RRC_CNFI_RRC_REESTABLISHMENT_COMPLETE = 0x01,
  NB_RRC_CNFI_RRC_CONNECTION_SETUP_COMPLETE = 0x02,
  NB_RRC_CNFI_SECURITY_MODE_COMPLETE = 0x03,
  NB_RRC_CNFI_SECURITY_MODE_FAILURE = 0x04,
  NB_RRC_CNFI_UE_CAPABILITY_INFORMATION = 0x05,
  NB_RRC_CNFI_UL_INFORMATION_TRANSFER = 0x06,
  NB_RRC_CNFI_RRC_CONNECTION_RESUME_COMPLETE = 0x07,
  /*RRC Controller*/
  NB_RRC_CNFI_MODE_CHANGE = 0x08,
  /*SIB*/
  NB_RRC_CNFI_GET_SIBS = 0x09,
  /*CEP*/
  /*Paging*/
  /*CRP*/
  /*CSP*/
  NB_RRC_CNFI_CSP_CELL_SELECT_ABORT = 0x0a,
  /*Config*/
  /*LLC*/
  NB_RRC_CNFI_CFG = 0x0b,
  /*Measurement*/
  NB_RRC_CNFI_MEAS_CFG = 0x0c,
  NB_RRC_CNFI_ESMGR_ACT_TMGI                 = 0x0d,
  NB_RRC_CNFI_ESMGR_DEACT_TMGI               = 0x0e,
  NB_RRC_CNFI_ESMGR_ACT_DEACT_TMGI               = 0x0f,

  NB_RRC_CNFI_PLMN_SEARCH        = 0x10,

  NB_RRC_CNFI_REVOKE_SUSPEND        = 0x11,

  NB_RRC_CNFI_MAX     /*!< Max value */

} nb1_rrc_cnfi_type_e;



/*! @brief Internal confirmations that various RRC procedures send
*/
enum
{
  /*Internal Confirmations */

  NB1_RRC_CNFI_FIRST = MSGR_UMID_BASE(MSGR_NB1_RRC, MSGR_TYPE_CNFI),

  /*Message Handler*/
  MSGR_DEFINE_UMID(NB1, RRC, CNFI, RRC_CONNECTION_RECONFIGURATION_COMPLETE,
                   NB_RRC_CNFI_RRC_RECONFIGURATION_COMPLETE,
                   nb1_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(NB1, RRC, CNFI, RRC_CONNECTION_REESTABLISHMENT_COMPLETE,
                   NB_RRC_CNFI_RRC_REESTABLISHMENT_COMPLETE,
                   nb1_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(NB1, RRC, CNFI, RRC_CONNECTION_SETUP_COMPLETE,
                   NB_RRC_CNFI_RRC_CONNECTION_SETUP_COMPLETE,
                   nb1_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(NB1, RRC, CNFI, SECURITY_MODE_COMPLETE,
                   NB_RRC_CNFI_SECURITY_MODE_COMPLETE,
                   nb1_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(NB1, RRC, CNFI, SECURITY_MODE_FAILURE,
                   NB_RRC_CNFI_SECURITY_MODE_FAILURE,
                   nb1_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(NB1, RRC, CNFI, UE_CAPABILITY_INFORMATION,
                   NB_RRC_CNFI_UE_CAPABILITY_INFORMATION,
                   nb1_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(NB1, RRC, CNFI, UL_INFORMATION_TRANSFER,
                   NB_RRC_CNFI_UL_INFORMATION_TRANSFER,
                   nb1_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(NB1, RRC, CNFI, RRC_CONNECTION_RESUME_COMPLETE,
                   NB_RRC_CNFI_RRC_CONNECTION_RESUME_COMPLETE,
                   nb1_rrc_ul_msg_cnfi_s),

  /*RRCController*/
  MSGR_DEFINE_UMID(NB1, RRC, CNFI, MODE_CHANGE,
                   NB_RRC_CNFI_MODE_CHANGE,
                   nb1_rrc_mode_change_cnfi_s),
  /*SIB*/
  MSGR_DEFINE_UMID(NB1, RRC, CNFI, GET_SIBS,
                   NB_RRC_CNFI_GET_SIBS,
                   nb1_rrc_get_sibs_cnfi_s),
  /*CEP*/
  /*Paging*/
  /*CRP*/
  /*CSP*/
  MSGR_DEFINE_UMID(NB1, RRC, CNFI, CSP_CELL_SELECT_ABORT,
                   NB_RRC_CNFI_CSP_CELL_SELECT_ABORT,
                   nb1_rrc_csp_cell_select_abort_cnfi_s),

  /*Config*/

  /*LLC*/
  MSGR_DEFINE_UMID(NB1, RRC, CNFI, CFG,
                   NB_RRC_CNFI_CFG, nb1_rrc_cfg_cnfi_s),

  /*Measurement*/
  MSGR_DEFINE_UMID(NB1, RRC, CNFI, MEAS_CFG,
                   NB_RRC_CNFI_MEAS_CFG, nb1_rrc_meas_cfg_cnfi_s),

  MSGR_DEFINE_UMID(NB1, RRC, CNFI, PLMN_SEARCH, NB_RRC_CNFI_PLMN_SEARCH,
                   nb1_rrc_plmn_search_cnfi_s),

  MSGR_DEFINE_UMID(NB1, RRC, CNFI, REVOKE_SUSPEND, NB_RRC_CNFI_REVOKE_SUSPEND,
                   nb1_rrc_revoke_suspend_cnfi_s),

  NB1_RRC_CNFI_MAX ,
  NB1_RRC_CNFI_LAST = NB1_RRC_CNFI_MAX - 1

};


/*! @brief Data structure for SMC failure indication;
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}nb1_rrc_smc_failure_indi_s;

/*! @brief Data structure for SMC failure indication;
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  boolean config_smc_from_saved_db; /*! TRUE to configure SMC from saved dB, FALSE to reset SMC */
}nb1_rrc_smc_config_indi_s;

/*! @brief Data structure for Stopped indication;
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_stopped_indi_s;

/*! @brief Data structure for unsolicited SIB indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_cell_id_s cell_id; /*!< Cell id */
  uint8 encoded_si[NB1_RRC_MAX_UNSOLICITED_SI_LENGTH]; /*!< ASN1 Encoded SI */
  uint32 encoded_si_size; /*!< Size of ASN1 encoded SI */
} nb1_rrc_unsolicited_sib_indi_s;

/*! @brief Data structure for SIB Updated indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  nb1_rrc_sib_bitmask_t sibs_changed_bitmask; /*!< Bitmask indicating sibs changed */
  nb1_rrc_sib_bitmask_t sibs_scheduled_bitmask; /*!< Bitmask indicating sibs scheduled; used by meas */
  lte_rrc_cell_id_s cell_id;
} nb1_rrc_sib_updated_indi_s;

/*! @brief Data structure for SIB Updated indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  nb1_rrc_sib_bitmask_t sibs_not_read_bitmask; /*!< Bitmask indicating sibs required but not read */
  nb1_rrc_sib_bitmask_t sibs_scheduled_bitmask; /*!< Bitmask indicating sibs scheduled*/
  lte_rrc_cell_id_s cell_id;
} nb1_rrc_sib_not_rcvd_indi_s;

/*! @brief Data structure for SIB read error indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_cell_id_s cell_id;
  nb1_rrc_sib_read_error_cause_e cause; /*!< Cause of the error */
  nb1_rrc_sib_bitmask_t sibs_rcvd_curr_bitmask; /*!< SIBs received in curr mod prd */
  nb1_rrc_sib_bitmask_t sibs_not_rcvd_curr_bitmask; /*!< SIBs not received in curr mod prd,
                                                      to be looked only for
                                                      NB1_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT cause */
  boolean intra_freq_resel_allowed; /*!< intraFreqReselection to be used for cell/freq barring */
} nb1_rrc_sib_read_error_indi_s;

/*! @brief Data structure for Connection Establishment Started indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_conn_establishment_started_indi_s;

/*! @brief Data structure for Connection Establishment Failure indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  boolean is_conn_rel_involved; /*!< Indicates if connection is explicitly released by CEP*/
} nb1_rrc_conn_establishment_failure_indi_s;

/*! @brief Data structure for Stop Cell Reselection indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_stop_cell_resel_indi_s;
 /*! @brief Data structure for Stop Cell Reselection indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  boolean status;
} nb1_rrc_proceed_with_resel_indi_s;

/*! @brief Data structure for Connection Release Started indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_conn_rel_started_indi_s;

/*! @brief Enumeration of Source Tech. RRC internal DS
*/
typedef enum
{
  NB1_RRC_TRM_PRIORITY_REQ_FROM_NAS,               /*!< 0 */
  NB1_RRC_TRM_PRIORITY_REQ_FROM_DS,                /*!< 1 */
  NB1_RRC_TRM_PRIORITY_REQ_FROM_CEP,               /*!< 2 */
  NB1_RRC_TRM_PRIORITY_REQ_FROM_CRP,               /*!< 3 */
  NB1_RRC_TRM_PRIORITY_REQ_FROM_CSP,               /*!< 4 */
  NB1_RRC_TRM_PRIORITY_REQ_FROM_CONFIG,            /*!< 5 */
  NB1_RRC_TRM_PRIORITY_REQ_FROM_IRAT_FROM_G_MGR,   /*!< 6 */
  NB1_RRC_TRM_PRIORITY_REQ_FROM_IRAT_FROM_W_MGR,   /*!< 7  */
  NB1_RRC_TRM_PRIORITY_REQ_FROM_IRAT_FROM_TDS_MGR, /*!< 8 */
  NB1_RRC_TRM_PRIORITY_REQ_FROM_IRAT_FROM_1X_MGR,  /*!< 9 */
  NB1_RRC_TRM_PRIORITY_REQ_FROM_CM,                /*!< 10 */
  NB1_RRC_TRM_PRIORITY_REQ_MAX                     /*!< 11 */
} nb1_rrc_trm_client_id_e;

/*! @brief Data structure for Stop Connection Released indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_conn_rel_reason_e rel_reason; /*!< Release reason */
  boolean cell_barring_is_reqd; /*!< Is cell barring required by CSP */
  nb1_phy_cell_id_t phy_cell_id;  /*!< Physical cell id to be barred */
  nb1_earfcn_t earfcn;  /*!< Frequency of cell to be barred */
  boolean redir_info_is_present; /*!< Is redirection info present? */
  nb1_rrc_redir_info_s redir_info; /*!< Redirection info
                                        - only valid if rel_reason is
                                        NB1_RRC_CONN_REL_REDIRECTED*/
  nb1_rrc_ml1_status_type_e ml1_status; /*!< ml1 state */
  boolean is_acq_needed;
  nb1_cphy_conn_release_acq_cell_info_s acq_cell_info;
  boolean save_security_config;
  /* Sending Commong Config to lower layers required */
  boolean forced_common_cfg;
} nb1_rrc_conn_released_indi_s;

/*! @brief Data structure the detailed info of t322 and Qoffset DB, which will affect the reselction when T322 is running.
detailed can refer 36.304.
*/
typedef struct
{
  /*! E-UTRA DL carrrier frequency dedicated */
  nb1_carrier_freq_s nb1_dl_carrier_freq;
  uint8 t_322_min;/*!<value of the T322 unit is minutes. */
  nb1_redir_carrier_offset_ded_e offset_ded;/*Parameter QoffsetDedicatedfrequency*/
}nb1_rrc_redir_carrier_info_s;

/*! @brief Data structure the detailed info of t322 and Qoffset DB. and the PLMN info to determine the PLMN selection.
*/
typedef struct
{
  /*!<EQPLMN list to determin the NAS PLMN selection performed or not>*/
  lte_rrc_plmn_list_s  redir_plmn_list;

  /* !< Detailed of the dedicated Qoffset info, and when will be used (defined by t322) */
  nb1_rrc_redir_carrier_info_s redir_carrier_info;

  /* !< T322 running or not, TRUE is running , FALSE is not running.*/
  boolean t322_running;

}nb1_rrc_meas_redir_carrier_info_s;

/*! @brief Data structure for RedirectedCarrierInfo indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  
  nb1_rrc_redir_carrier_info_s redir_carrier_info;
} nb1_rrc_redir_carrier_info_indi_s;

/*! @brief Data structure for Not Camped indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_not_camped_indi_s;


/*! @brief Data structure for Not Camped indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_cell_id_s camped_cell;  /*!< Cell Id of the new serving cell */
  nb1_rrc_camped_reason_e reason; /*!< Camped reason */
} nb1_rrc_camped_indi_s;

/*! @brief Data structure for each frequency entry
*/
typedef struct
{
  /*! DL Center frequency */
  nb1_earfcn_t dl_earfcn;
	/*! System Bandwidth */
	nb1_bandwidth_e bandwidth;
} nb1_earfcn_bw;

/*! @brief Data structure for connmode interfreq neighbors indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  uint8 num_frequencies;
  nb1_earfcn_bw interFreqNeighborList[NB1_RRC_MAX_INTFREQ_NEIGHBORS];
} nb1_rrc_interfreq_list_update_indi_s;


/*! @brief Data structure for Connected indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_connected_indi_s;


/*! @brief Data structure for Cell Reselction Started indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_cell_id_s camped_cell;  /*!< Cell Id of the current serving cell */
  nb1_cphy_resel_target_cell_info_s target_cell_info;  /*!< Target cell info */
  nb1_source_cell_info_type serving_cell_info; /*!< Serving cell info for ASF fingerprinting */
} nb1_rrc_cell_resel_started_indi_s;

/*! @brief Data structure for TX unavailable indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_tx_unavailable_indi_s;

/*! @brief Data structure for TX unavailable indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_tx_available_indi_s;


typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_proceed_with_rach_opt_indi_s;

/*! @brief Data structure for Cell Reselction Canceled indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_cell_resel_canceled_indi_s;

/*! @brief Data structure for Connected mode failure indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  nb1_rrc_conn_mode_failure_cause_e conn_mode_failure_cause; /*!< Conn Mode Failure cause */
  nb1_rrc_ho_failure_cause_e ho_failure_cause; /*!< HO Failure cause, applicable only
                                                    when conn_mode_failure_cause is HO */
  nb1_rrc_cell_info_s ho_source_cell_info;   /*! HO Source cell Info */
} nb1_rrc_conn_mode_failure_indi_s;

/*! @brief Data structure for CRE Started indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_cre_started_indi_s;

/*! @brief Data structure for CRE Completed indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_cre_completed_indi_s;

/*! @brief Data structure for Initiate Connection Release indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_conn_rel_reason_e rel_reason; /*!< Reason */
  boolean rlf_due_to_ns4;	/*!< If RLF happened due to NS4 validation */

 /* LRRC will set it to TRUE for all the RLF causes that we want to track, 
    such that NAS will trigger TAU if required as UE workaround fix for frequent RACH failures */
  boolean tau_req_due_to_rach_fail_rlf;   
} nb1_rrc_initiate_conn_rel_indi_s;

/*! @brief Data structure for serving cell changed indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_cell_id_s serving_cell_id;  /*!< serving cell id */

  /*! Bandwidth of the target cell */
  nb1_bandwidth_e cell_bandwidth;

  /*! Number of Tx antennas */
  uint8 num_tx_antennas;

} nb1_rrc_serving_cell_changed_indi_s;

/*! @brief Data structure for serving cell changed indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  boolean mbsfn_cfg_valid;  /*!< MBSFN cfg status */

} nb1_rrc_llc_mbsfn_cfg_chng_indi_s;


/*! @brief Data structure for dlm processed indication passed to MH
*/
typedef struct
{
  msgr_hdr_s msg_hdr;       /*!< Message router header */
}nb1_rrc_dlm_processed_indi_s;

//CEP opt
/*! @brief Data structure for sending RRC OOS Indi due to CEP opt/MIB change */
typedef struct
{
  msgr_hdr_s msg_hdr;      /*!< Message router header */
  nb1_rrc_oos_ind_cause_e oos_ind_cause;      /*!< Message router header */
}nb1_rrc_oos_indi_s;

/*! @brief Data structure for sending tracking area info after camping on new cell */
typedef struct
{
  msgr_hdr_s msg_hdr;      /*!< Message router header */
  nb1_rrc_cell_info_s cell_info; /*!< Cell info */
  uint16 tac; /*!< Tracking area code of the cell */
  lte_rrc_plmn_s plmn; /*!< PLMN area code of the cell */
  boolean attachwithoutpdn_connectivity;
}nb1_rrc_new_cell_info_indi_s;

/*! @brief Data structure for SRB2 resumed indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_rb_resumed_indi_s;

/*! @brief Data structure for sending Pre-Registration info for HRPD to IRAT Mgr */
typedef struct
{
  msgr_hdr_s msg_hdr;      /*!< Message router header */

  void *dl_msg_ptr; /*!< Pointer to decoded DL msg containing DL DCCH message for
                          RRC Reconfiguration */

  boolean prereg_with_handover; /*!< If PreRegInfo came with the handover msg */

} nb1_rrc_irat_hrpd_pre_reg_info_indi_s;

/**/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  nb1_irat_rat_type_e                       source_rat; /*!< RAT Type */

  /*! T304 value (in ms) contained in RRCConnectionReconfiguration */
  uint32                                    t_304_value_ms;

  /*! NAS related info needed for camping on NB1 */
  nb1_rrc_nas_info_s                        nas_info;

 /*!< Serving cell info for ASF fingerprinting */
  nb1_source_cell_info_type                     serving_cell_info;

 /*! UTRA cell info for too early handover */
  nb1_utra_serving_cell_info_type                 utra_cell_info;

} nb1_rrc_irat_to_nb1_psho_started_indi_s;

/*! @brief Data structure for key generation for IRAT to NB1 PSHO indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_irat_to_nb1_psho_key_gen_indi_s;

/*! @brief Data Structure for MBSFN SNR Ind.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /*! MBSFN SNR info */
  lte_rrc_mbsfn_snr_info_s mbsfn_snr_info;

} nb1_rrc_mbsfn_snr_indi_s;

/* Type for Global Cell id */
typedef uint16 nb1_rrc_tac_t;

/*! @brief Enum to represent timer extension request reason
*/
typedef enum
{
  NB1_RRC_SIB_EXTND_TMR_AUTO_GAP,              /*!< 0 */
  NB1_RRC_SIB_EXTND_TMR_MAX = 0xFF,            /*!< 255 */
} nb1_rrc_sib_extnd_tmr_reason_e;

/*! @brief Data structure for SIB extnd timer reqi
*/
typedef struct
{
  /*!< Message router header */
  msgr_hdr_s msg_hdr;

  /*!< SIB timer extension reason */
  nb1_rrc_sib_extnd_tmr_reason_e reason;

  /*!< Duration to extend the SIB timers */
  uint64 duration;

} nb1_rrc_sib_extnd_tmr_reqi_s;

/*! @brief CSP Interfreq neighbors list
*/
typedef struct
{
  nb1_earfcn_bw intfreq_list[NB1_RRC_CSP_MAX_ACQ_DB_ENTRIES];
  uint8 num_frequencies;
  uint8 next_index;/*track next index location to update neighbors*/
  boolean  interfreq_scan;
}nb1_rrc_interfreq_list;

/*! @brief This structure defines nb1 csp common params */
typedef struct
{
  /*! List of acceptable PLMNs found so far */
  lte_rrc_found_plmn_list_s found_plmn_list;

  /*! Candidate cell */
  nb1_rrc_csp_candidate_cell_s candidate_cell;

  /*! Holds the current scan type. This field identifies what type of scan CSP
      is currently performing */
  nb1_rrc_csp_scan_type_e scan_type;

  /*! Acq candidate list type */
  nb1_rrc_csp_acq_candidate_list_type_e acq_candidate_list_type;

  /*! flag to record full band interleave scan when oos timer expire */
  boolean full_band_priority_scan;

  /*! attempt acquisition on a better cell if indicated by ML1 */
  boolean acq_on_better_cell  : 1;

  /*! priority scan in progress. Perform non priority scan at the end of priority scan*/
  boolean priority_scan : 1;

  boolean reserved : 6;
} nb1_rrc_csp_shim_common_param_s;

/*! @brief This structure defines nb1 band scan params */
typedef struct
{
  /* To save channel search stop index in ML1 for the last band 
     during band scan */
  uint16 search_stop_index;

  /*! To indicated whether the band scan for current band is complete or not */
  boolean ffs_incomplete;

  /*! To store the band scan retry counter */
  uint8 band_scan_retry_counter;
} nb1_rrc_csp_band_scan_param_s;

/*! @brief This structure defines nb1 interleave param */
typedef struct
{
  /*! To save Band scan list */
  nb1_rrc_csp_band_scan_list_s band_scan_list;

  /*! To save Unusable systems */
  nb1_cphy_band_scan_req_unusable_systems_s unusable_systems;
  
  /*! nb1 band scan params */
  nb1_rrc_csp_band_scan_param_s band_scan_params;
  
} nb1_rrc_csp_shim_band_scan_param_s;

/*! @brief Data structure to store nb1 system scan param info
*/
typedef struct
{
  /*! System scan construction list for operator  - a temporary structure */
  nb1_cphy_system_s acq_db_ext_systems[NB1_RRC_CSP_MAX_ACQ_DB_EXT_SYSTEMS];

  /*! System scan construction list for operator */
  nb1_cphy_system_s pref_freq_list_systems[NB1_RRC_CSP_MAX_PREF_FREQ_LIST_ENTRIES];

  /*! System scan construction list - a temporary structure */
  nb1_cphy_system_s systems[NB1_CPHY_SYSTEMSCAN_MAX_SYSTEMS_TO_SCAN];
      
  /*! To save frequency search stop index in ML1 for the last frequency 
     during system scan */
  uint16 search_stop_index;

  /*! To perform operator scan */
  boolean pref_freq_list_scan;

  /*! Index to split pref freq list system scan for batch when system number 
      is largr than MAX number of system scan number */
  uint8 pref_freq_list_split_index;

  /*! To store total number of systems for system scan */
  uint8 pref_freq_list_num_systems;

  /*! To store the system scan retry time in different SNR level */
  uint8 system_scan_retry_counter;

  /*! addtional system scan retry is ongoing */
  boolean additional_sys_scan_retry;

  /* To perform RLF acq Db scan only */
  boolean rlf_last_acq_db_scan;

  /*! To store total number of systems for system scan */
  uint8 num_systems_in_sys_scan;

  /*! Indicated current frequency list scan(subset) is complete or not 
      boolean: TRUE, incomplete,otherwise, complete */
  boolean lfs_incomplete; 

  /*Index to split common acq db list system scan for batch 
    when system number is largr than MAX number of system scan number */
  uint8 acq_db_ext_num_systems;
  uint8 acq_db_ext_split_index;
} nb1_rrc_csp_shim_system_scan_param_s;

typedef struct
{
  int16                                        rsrp;        
  int16                                        rsrq;   
  int16                                        rssi; 
}nb1_rrc_cell_scan_signal_info_s_type;

/*! @brief Data type for PLMN_SEARCH SM
*/
typedef struct
{
    nb1_phy_cell_id_t         phy_cell_id;           /*!< Physical cell id */
    nb1_earfcn_t              freq;                  /*!< Frequency */
    lte_rrc_plmn_list_s       plmn_list;             /*!< PLMN list from SIB1 */
    boolean                   plmn_match_has_failed : 1; /*!< No PLMNs Matched request */
    boolean                   scrit_has_failed : 1;      /*!< S-criteria check */
    boolean                   plmns_are_reserved : 1;           /*!< Reserved PLMNs in list */
    boolean                   reserved_plmn_cell_is_barred : 1; /*!< Cell barred due to reserved PLMNs  */
    boolean                   reserved : 4;
    uint16                      tac;                   /*!< Tracking area code */
    uint32                      plmn_index;            /*!< Matching PLMn index */
    sys_signal_quality_e_type signal_quality;        /*!< Signal Quality */
    sys_signal_strength_type  signal_strength;       /*!< RSRP */
    lte_rrc_global_cell_id_t  cell_identity;         /*!< Global cell id (from SIB1) */
    boolean                   cell_barred;           /*!< SIB1 Cell barring */
    nb1_rrc_cell_scan_signal_info_s_type pci_sig_info;    /*!< RSRP/RSRQ */
} nb1_rrc_plmn_search_record_s;

typedef struct
{
    nb1_rrc_plmn_search_record_s found_plmn[SYS_PLMN_LIST_MAX_LENGTH]; /*!< Found NB1 PLMNs List */
    uint32                       next_index;  /*!< Next free index in the list */
    uint32                       hplmn_index; /*!< HPLMN index in the list */
} nb1_rrc_plmn_search_table_s;

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
typedef struct
{
  /*!< control params sent by WWAN for scanning */
  sys_loc_wwan_scan_control_s_type control_param;

  /*!< Assistance parameters sent by WWAN for scanning*/
  sys_local_wwan_scan_assitance_data_nb1_s_type assistance_param;
  
}nb1_rrc_plmn_search_wwan_scan_params_s_type;
#endif

/*! @brief Data structure to store nb1 plmn search info
*/
typedef struct
{
  nb1_rrc_plmn_search_table_s found_plmn_table; /*!< Table to keep track of NB1 PLMN scan results */
} nb1_rrc_plmn_search_shim_param_s;

/*! @brief Enum to represent various MARB Unload State types in CSP
*/
typedef enum
{
  /*! MARB Unload State Type is none of the below */
  NB1_RRC_CSP_MARB_UNLOAD_STATE_NONE = 0,
  
  /*! MARB Unload triggered during Band/System Scan */
  NB1_RRC_CSP_MARB_UNLOAD_STATE_BAND_OR_SYSTEM_SCAN,

  /*! MARB Unload triggered during Resection in Progress */
  NB1_RRC_CSP_MARB_UNLOAD_STATE_RESEL_IN_PROG,

  /*! MARB Unload triggered during Camped State */
  NB1_RRC_CSP_MARB_UNLOAD_STATE_CAMPED,

}nb1_rrc_csp_marb_unload_state_type_e;

/*! @brief This structure defines the contents of each entry
           in the MARB Unload Acq DB stored in SHIM Layer
*/
typedef struct
{
  /*! DL Center frequency */
  nb1_efs_earfcn_t dl_earfcn;

  /*! Physical cell id of last camped cell */
  nb1_phy_cell_id_t phy_cell_id;

}nb1_rrc_csp_marb_unload_acq_entry_s;

/*! @brief NB1 CSP MARB UNLOAD Acq DB structure
*/
typedef struct
{  
  /*! Reselection Target cell Acq Entry */
  nb1_rrc_csp_marb_unload_acq_entry_s resel_target_cell;

  /*! Camped cell Acq Entry */
  nb1_rrc_csp_marb_unload_acq_entry_s camped_cell;

  /*! Holds NB1 CSP status when MARB UNLOAD triggered */
  nb1_rrc_csp_marb_unload_state_type_e state_type;

  /* Reselection Target cell Acq Entry Valid */
  boolean resel_target_cell_info_valid;

  /* Camped cell Acq Entry Valid */
  boolean camped_cell_info_valid;
  
}nb1_rrc_csp_marb_unload_acq_info_s;

/*! @brief Enum to represent various NB1 RRC SHIM Layer Param Update Type
*/
typedef enum
{
  /*! NB1 RRC SHIM Layer Param Update Type is none of the below */
  NB1_RRC_SHIM_LAYER_UPDATE_TYPE_NONE = 0,

  /*! NB1 RRC SHIM Layer Common Param Update Type */
  NB1_RRC_SHIM_LAYER_UPDATE_TYPE_CSP_COMMON_PARAM,
  
  /*! NB1 RRC SHIM Layer Band Scan Param Update Type */
  NB1_RRC_SHIM_LAYER_UPDATE_TYPE_BAND_SCAN_PARAM,

  /*! NB1 RRC SHIM Layer System Scan Param Update Type */
  NB1_RRC_SHIM_LAYER_UPDATE_TYPE_SYSTEM_SCAN_PARAM,

  /*! NB1 RRC SHIM Layer PPLMN Search Param Update Type */
  NB1_RRC_SHIM_LAYER_UPDATE_TYPE_PLMN_SEARCH_PARAM,

  /*! NB1 RRC SHIM Layer MARB UNLOAD Acq Info Param Update Type */
  NB1_RRC_SHIM_LAYER_UPDATE_TYPE_MARB_UNLOAD_ACQ_INFO_PARAM,
} nb1_rrc_shim_layer_param_update_type_e;

/*! @brief Union of different types of NB1 RRC SHIM Layer Param Updates */
typedef union
{
  /*! NB1 RRC SHIM Layer Common Param Structure */
  nb1_rrc_csp_shim_common_param_s csp_common_db;
  
  /*! NB1 RRC SHIM Layer Band Scan Param Structure */
  nb1_rrc_csp_shim_band_scan_param_s band_scan_db;

  /*! NB1 RRC SHIM Layer System Scan Param Structure */
  nb1_rrc_csp_shim_system_scan_param_s system_scan_db;
  
  /*! NB1 RRC SHIM Layer PLMN Seach Param Structure */
  nb1_rrc_plmn_search_shim_param_s   plmn_search_db;

  /*! NB1 RRC SHIM Layer MARB Unload Acq Info Structure */
  nb1_rrc_csp_marb_unload_acq_info_s marb_unload_acq_info;

} nb1_rrc_shim_layer_update_info_u;

/*! @brief NB1 RRC SHIM Layer Param Updates
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /*! NB1 RRC SHIM Layer Param Update Type */
  nb1_rrc_shim_layer_param_update_type_e param_update_type;

  /*! Union of different types of NB1 RRC SHIM Layer Param Updates */
  nb1_rrc_shim_layer_update_info_u param_info;

} nb1_rrc_shim_layer_param_udpate_s;

/*! @brief Enumeration of INDI id nums
*/
typedef enum _nb1_rrc_indi_type_e
{
  /*RRC Controller*/
  NB_RRC_INDI_STOPPED = 0x0,
  /*SIB*/
  NB_RRC_INDI_UNSOLICITED_SIB = 0x01,
  NB_RRC_INDI_SIB_UPDATED = 0x02,
  NB_RRC_INDI_SIB_READ_ERROR = 0x03,
  /*CEP*/
  NB_RRC_INDI_CONN_ESTABLISHMENT_STARTED = 0x04,
  NB_RRC_INDI_CONNECTED = 0x05,
  NB_RRC_INDI_CONN_ESTABLISHMENT_FAILURE = 0x06,
  NB_RRC_INDI_STOP_CELL_RESEL = 0x07,
  NB_RRC_INDI_PROCEED_WITH_RESEL = 0x08,
  /*Paging*/
  /*CRP*/
  NB_RRC_INDI_CONN_REL_STARTED = 0x09,
  NB_RRC_INDI_CONN_RELEASED = 0x0a,
  /*CSP*/
  NB_RRC_INDI_NOT_CAMPED = 0x0b,
  NB_RRC_INDI_CAMPED = 0x0c,
  NB_RRC_INDI_CELL_RESEL_STARTED = 0x0d,
  NB_RRC_INDI_CELL_RESEL_CANCELED = 0x0e,
  /*Config*/
  NB_RRC_INDI_CONN_MODE_FAILURE = 0x0f,
  /*Dispatcher*/
  NB_RRC_INDI_PENDING_MSG = 0x10,
  /*CRE*/
  NB_RRC_INDI_CRE_STARTED = 0x12,
  NB_RRC_INDI_CRE_COMPLETED = 0x13,
  NB_RRC_INDI_INITIATE_CONN_REL = 0x14,
  /*Measurement*/
  NB_RRC_INDI_MEAS_PENDING_SIB_UPDATED = 0x15,
  /*Security*/
  NB_RRC_INDI_SMC_FAILURE = 0x16,

  NB_RRC_INDI_SMC_CONFIGURE = 0x17,

  /*Message Handler*/
  NB_RRC_INDI_DLM_PROCESSED = 0x1F,
  /*LLC*/
  NB_RRC_INDI_SERVING_CELL_CHANGED = 0x20,
  /*CEP*/
  NB_RRC_INDI_OOS = 0x21,
  /* CEP - new cell info containing tac\plmn */
  NB_RRC_INDI_NEW_CELL_INFO = 0x23,
  NB_RRC_INDI_RB_RESUMED = 0x25,

  NB_RRC_INDI_SHIM_LAYER_PARAM_UPDATE  = 0x26,

  /* eMBMS */
  NB_RRC_INDI_EMP_DEACT_TMGI = 0x2B,

  /* eMBMS */
  NB_RRC_INDI_EMP_OOS_WARN = 0x2D,
  NB_RRC_INDI_EMP_PENDED_SERVICE = 0x2E,
  NB_RRC_INDI_ESMGR_AVAIL_TMGI_LIST = 0x2F,
  NB_RRC_INDI_INTERFREQ_LIST_UPDATE = 0x37,
  NB_RRC_INDI_LLC_MBSFN_CFG_CHANGED = 0x38,
  NB_RRC_INDI_ESMGR_RESEL_PRIO_CHANGE = 0x3B,
  NB_RRC_INDI_PROX_CFG = 0x3F,
  NB_RRC_INDI_SIB_NOT_RCVD = 0x40,
  NB_RRC_INDI_MBSFN_SNR = 0x43,
  NB_RRC_INDI_NV_REFRESH = 0x44,
  NB_RRC_INDI_SIB1_AVAIL = 0x46,
  NB_RRC_INDI_TX_UNAVAILABLE = 0x47,
  NB_RRC_INDI_TX_AVAILABLE = 0x48,
  NB_RRC_INDI_REDIR_CARRIER_INFO = 0x49,
  NB_RRC_INDI_PENDED_SIB_UPDATED = 0x4A,
  NB_RRC_INDI_PROCEED_WITH_RACH_OPT = 0x4B,
  NB_RRC_INDI_MAX     /*!< Max value */
}nb1_rrc_indi_type_e;

/*! @brief Data Structure for RRC NV Refresh Ind.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  boolean reference_valid;

  /*! Reference id */
  uint32 reference_id;

  /*! Subscription id to refresh NVs */
  int8 sub_id;

} nb1_rrc_nv_refresh_indi_s;

/*! @brief Static data type for CSP
*/
typedef struct
{
  /*!< Make sure to declare the fields in Descending order of memory size to minimize 
       automatic structure padding and to save the memory */

  /*! CSP Acquisition DB */
  nb1_rrc_csp_acq_db_s acq_db;

  /*! Info related to current scan */
  nb1_rrc_csp_current_scan_info_s current_scan_info;

  /*! CSP Acquisition EFS DB to be read/written to EFS */
  nb1_rrc_csp_efs_acq_db_v3_s efs_acq_db_v3;

  /*! Holds NAS req contents */
  nb1_rrc_csp_nas_req_info_s nas_req_info;

  /*! Temporary print buffer - static because it is used to print system
      update contents */
  char str_buf[1024];

  /*! CSP Acquisition EFS DB to be read/written to EFS */
  nb1_rrc_csp_efs_acq_db_s efs_acq_db;

  /*! CSP Acquisition EFS DB to be read/written to EFS */
  nb1_rrc_csp_efs_acq_db_v2_s efs_acq_db_v2;

  /*! Prioritized band list */
  sys_sband_lte_e_type band_priority_list[SYS_SBAND_LTE_EUTRAN_BAND_MAX];

  /*! Systems unsuitable for camping found during band scan */
  nb1_cphy_band_scan_req_unusable_systems_s unusable_systems;

  /*struct to the cell list for cell lock functionality */
  nb1_rrc_csp_cell_lock_s cell_lock_list;

  /*! Pref earfcns list */
  nb1_earfcn_t   pref_earfcn_lists[NB1_RRC_CSP_MAX_PREF_FREQ_LIST_ENTRIES];

  /*struct to store interfrequency neighbors received in SIB5*/
  nb1_rrc_interfreq_list interfreq_list_update;

  /*! RLF System Scan Timer */
  nb1_rrc_tmr_s rlf_system_scan_tmr;

  /*! RLF System Scan Timer */
  nb1_rrc_tmr_s rlf_sys_scan_interleave_tmr;


  /*! NB1 BST band preference - need to check for EOOS scans across deactivation */
  sys_lte_band_mask_e_type nb1_bst_band_preference;

  /*! Prioritized band mask to perform two pass scan */
  sys_lte_band_mask_e_type priority_band_mask;

  /*! BAND/EARFCN/PhyCellId for PCI LOCK feature and only valid if pci_lock_enabled is TRUE*/
  nb1_rrc_csp_pci_lock_s pci_lock_efs_info;

  /*! Two EARFCNs for EARFCN_LOCK feature and only valid if earfcn_lock_enabled is TRUE*/
  nb1_rrc_csp_earfcn_lock_s earfcn_lock_efs_info;

  /*! store band scan params */
  nb1_rrc_csp_band_scan_param_s  band_scan_params;

  /*! Enable/Disable Cell barring through EFS */
  uint32 cell_barring_is_enabled;

  /*! Short cell barring time configured through EFS */
  uint32 short_cell_barring_time;

  /*! Short cell barring time for RLF configured through EFS */
  uint32 rlf_short_cell_barring_time;

  /*! Reduced Barring time configured through EFS for causes such as SIB read errors*/
  uint32 reduced_barring_time;

  /*! barring time configured through EFS for internal errors such as IM3 backoff */
  uint32 backoff_barring_time;

  /*! Enable/Disable Intra-NB1 Redir retries */
  uint32 intra_nb1_redir_test_enabled;

  /*! Num prioritized bands */
  uint32 num_prioritized_bands;

  /*!RLF recovery system scan duration*/
  uint32 rlf_system_scan_duration;

  /*!Interval for interleaved sys scan for RLF recovery*/
  uint32 rlf_sys_scan_interleave_period;

  uint32  psm_threshold;

  /*! Num prefer earfcn */
  uint32 num_pref_earfcns;

  /*! Current mode */
  lte_rrc_div_duplex_e curr_mode;

  /*! Previous mode */
  lte_rrc_div_duplex_e prev_mode;

  /*! Frequency list type */
  lte_rrc_freq_list_type_e  freq_list_type;

  /*! Holds NB1 CSP status when MARB UNLOAD triggered */
  nb1_rrc_csp_marb_unload_state_type_e marb_unload_state_type;

    /*! Enable/Disable pci lock feature through EFS*/
  uint8 pci_lock_enabled;

  /*! Enable/Disable earfcn lock feature through EFS*/
  uint8 earfcn_lock_enabled;

  /*! Maximum number of acquisition retries on same frequency */
  uint8 max_num_acq_retries;

  /*! Maximum number of bands in a band scan request */
  uint8 max_bands_bs_req;

    /*! Variable to activate memory report */
  uint8 mem_report_valid;

  /*! Enable/Disable SNR Level2 band scan as default */
  uint8 snr2_band_scan_enable;

  /*! SNR Levels scope for band scan, default is full SNR level */
  uint8 snr_level_scan_scope;

  /*! if it's TRUE, UE will always try lfs only before snr2_band_scan_tmr expire */
  uint8 lfs_only_before_snr2_expired;

   /*! Systems scan disabled through EFS */
  boolean lfs_is_disabled;

  /*! ACQ DB will be considered to configure inter freq neighbours through EFS */ 
  boolean cfg_acq_db_neigh_enabled;

  /*! BSR Qrxlev min enabled through EFS */
  boolean bsr_qrx_is_enabled;

  /*! EFS to use all three MNC digits for matching */
  boolean use_all_mnc_digits_enabled;

  boolean save_acq_db_enabled;

  /*Flag to indicate PSM feature enabled or not */
  boolean psm_enabled;

  /* ! Indicator whether interfreq list was modify or not */
  boolean                interfreq_list_modified; 

  /*! boolean indicates interleave timer expired during band scan */
  boolean interleave_timer_expired_during_band_scan;

  /*! boolean indicates if current scan is interleave band scan */
  boolean full_band_interleave_scan;  

  /* Indicates if the optimization for current freq cell selection completion
     on EOOS timer expiry is turned on */
  boolean eoos_complete_current_freq : 1;

  /*! OOS System Scan Timer */
  boolean use_oos_system_scan_tmr : 1;

  /* Acq_db modified, needs writing to EFS */
  boolean acq_db_modified : 1;

  boolean band_pri_list_modified :1;

  boolean pci_lock_info_modified :1;

  boolean earfcn_lock_info_modified : 1;

  /*! Is IM3 backoff applied ?*/
  boolean im3_backoff_is_applied :1;

  /*! Iteratively get more candidates on the same band */
  boolean iterative_band_scan_enabled :1;

  /*Flag to indicate POI SIB1 decode feature enabled or not */
  boolean poi_sib1_decode_enabled : 1;
} nb1_rrc_csp_static_data_s;

/*PSM context store at CM*/

/*! @brief This structure holds all encoded SIs of a cell
*/
typedef struct
{
  /*!< Bitmask indicating what SIBs are present */
  nb1_rrc_sib_bitmask_t sibs_bitmask;
  /*!< SIBs contained in SI */
  nb1_rrc_sib_bitmask_t sibs_in_si_bitmask[NB1_RRC_SIB_MAX_SIS];
  /*!< Array of encoded SIs */
  uint32 encoded_si_size[NB1_RRC_SIB_MAX_SIS];
  /*!< Indicates whether decoded SIs are present or not. */
} nb1_rrc_sib_psm_all_encoded_si_s;

/*! @brief This structure holds cell identity
*/
typedef struct
{
  /*!< PLMN of the cell */
  lte_rrc_plmn_s plmn;
  /*!< Cell identity (received in SIB1) */
  lte_rrc_global_cell_id_t global_cell_id;
  /*!< Holds phy_cell_id and freq of the cell*/
  lte_rrc_cell_id_s cell_id;
} nb1_rrc_sib_cell_id_s;

/*! @brief This structure holds SIB DB EFS
*/
typedef struct
{
  /*!< Cell Identity */
  nb1_rrc_sib_cell_id_s sib_cell_id;

  /*!< Value tag */
  uint32 value_tag;

  /*!< MIB */
  nb1_l1_nb_mib_info_param_s mib;

  /*!< Index corresponding to band chosen in multiBandInfoList */
  uint8 mfbi_index;

  //eDRX page read valid
  boolean edrx_page_read_valid;
  
  //Current time before PSM
  uint64 curr_time_before_psm;
  
  //Time remaining for eDRX acq boundary
  uint64 acq_bnd_remaining_time;

  nb1_l1_sfn_t psm_sfn;
  
  nb1_l1_hyper_sfn_t psm_hyper_sfn;

  //SI value tag list.
  uint8 si_value_tag[NB1_CPHY_MAX_SI_MSGS];
  
  /*!< Encoded SIB1 */
  //lte_rrc_sib_psm_encoded_si_info_s encoded_sib1;
  /*!< Size of encoded SI in bytes */
  uint32 encoded_sib1_size;

  /*!< Encoded & decoded SIs */
  nb1_rrc_sib_psm_all_encoded_si_s	 all_encoded_si;

  /*SIB1 + SIz pointer */
  uint8 encoded_si_ptr[0];  //variable length structure
  
}nb1_rrc_sib_db_psm_efs;

/*! @brief AS Security Context
*/

typedef struct
{
  /*pre computed keys gen at sim_update time*/
  uint8 k_rrc_int_snow3g_algo[NB1_CIPHER_INTEGRITY_KEY_LEN]; /*!K-eNB*/
  uint8 k_rrc_enc_snow3g_algo[NB1_CIPHER_INTEGRITY_KEY_LEN]; /*!K-RRC-enc*/
  uint8 k_up_enc_snow3g_algo[NB1_CIPHER_INTEGRITY_KEY_LEN];/*!K-UP-enc*/

  uint8 k_rrc_int_aes_algo[NB1_CIPHER_INTEGRITY_KEY_LEN]; /*!K-eNB*/
  uint8 k_rrc_enc_aes_algo[NB1_CIPHER_INTEGRITY_KEY_LEN]; /*!K-RRC-enc*/
  uint8 k_up_enc_aes_algo[NB1_CIPHER_INTEGRITY_KEY_LEN];/*!K-UP-enc*/

  uint8 k_rrc_int_zuc_algo[NB1_CIPHER_INTEGRITY_KEY_LEN]; /*!K-eNB*/
  uint8 k_rrc_enc_zuc_algo[NB1_CIPHER_INTEGRITY_KEY_LEN]; /*!K-RRC-enc*/
  uint8 k_up_enc_zuc_algo[NB1_CIPHER_INTEGRITY_KEY_LEN];/*!K-UP-enc*/
}nb1_rrc_sec_pre_comp_as_security_keys_s;

typedef struct
{
  nb1_integrity_algo_e integrity_algo;/*! Integrity Algo*/
  nb1_cipher_algo_e cipher_algo;/*! Cipher Algo*/
  uint8 ncc;  /*!Next Hop Chaining Count (NCC) */
  uint8 k_enb[NB1_GENERATOR_KEY_LEN];
  uint8 k_rrc_int[NB1_CIPHER_INTEGRITY_KEY_LEN]; /*!K-eNB*/
  uint8 k_rrc_enc[NB1_CIPHER_INTEGRITY_KEY_LEN]; /*!K-RRC-enc*/
  uint8 k_up_enc[NB1_CIPHER_INTEGRITY_KEY_LEN];/*!K-UP-enc*/
  boolean is_initial_k_enb; /*!Is this K_eNB derived from a new NAS context ?*/
  uint8 nh[NB1_GENERATOR_KEY_LEN]; /*! NextHop (NH) Key*/

  nb1_rrc_sec_pre_comp_as_security_keys_s pre_comp_keys;

  boolean freq_pci_is_valid;/*! Is the DL-EARFCN and PCI info valid ?
  This is set to FALSE from the time of SMC to the first handover. It is set to TRUE
  from then onwards*/
  nb1_phy_cell_id_t phy_cell_id;  /*!Phy cell ID of the target cell*/
  nb1_earfcn_t freq ;   /*!Downlink EARFCN of the target cell*/
}nb1_rrc_sec_as_security_context_s;

/*! @brief NAS Security Context
*/

typedef struct
{
  lte_rrc_security_info_s sec_info_from_nas;/*! Security Info from NAS*/
  uint8 k_enb_initial[NB1_GENERATOR_KEY_LEN];/*!Initial K eNB derived from K-ASME*/
}nb1_rrc_sec_nas_security_context_s;

typedef struct
{
  boolean has_valid_saved_security_context;
  
  /* saved security config*/
  nb1_rrc_sec_as_security_context_s saved_as_security_context;

          /*!< AS security Contexts*/
  uint8 saved_current_as_security_context_index; /*!< Current AS Security Context index*/
  
  /*!< Current NAS Security Context index */
  uint8 saved_current_nas_security_context_index;

  uint8 saved_a2_key_index_n; /*!<Key indices are 3*n, 3*n+1 and 3*n+2 */
  
  nb1_rrc_sec_nas_security_context_s saved_nas_security_context;
}nb1_rrc_psm_saved_sec_db;

/*! This structure defines all NB1 related dedicated lower layer configuration
    information [PDCP, RLC, MAC, L1]
*/
typedef struct
{
  nb1_rrc_cell_info_s cell_info;

  /* ====================================== */
  /* Dedicated Physical layer Configuration                   */
  /* ====================================== */
  /*! Parameters required to configure PUSCH physical channel */
  nb1_cphy_npusch_cfg_dedicated_param_s pusch_cfg_dedicated;
  /*! Parameters required to configure Carrier Config DED information */
  nb1_cphy_carrier_cfg_dedicated_param_s carrier_cfg_dedicated;
  /*! Parameters required to configure NPDCCH Config DED information */
  nb1_cphy_npdcch_cfg_dedicated_param_s npdcch_cfg_dedicated;
  /*! Parameters required to configure UL Power Control information */
  nb1_cphy_ul_power_ctrl_dedicated_param_s ul_power_ctrl_dedicated;
  /*!Indicates if the cqi_cfg I.E received is rel-8 or Rel-10 */
  lte_3gpp_release_ver_e cqi_cfg_rel_version;
  /*!Indicates if the antenna_cfg I.E received is rel-8 or Rel-10 */
  lte_3gpp_release_ver_e antenna_cfg_rel_version;
  /*! Parameters required to configure connected mode DRX configuration. */
  nb1_cphy_drx_cfg_dedicated_param_s connected_drx_cfg;
  boolean drx_cycle_r14_cfg_present;
  /*! Parameters required to configure UE related configuration */
  nb1_cphy_ue_related_cfg_s ue_related_cfg;
  /*! Dedicated RLF timers and constant params: used for RLF detection/recovery */
  nb1_rrc_dedicated_rlf_timers_s tmr_and_const_dedicated;
  /*! DRX optimization for UM mode enabled in ML1 */
  boolean drx_opt_enabled;
  /*! Time domain resource restriction pattern for the serving cell measurements
     (RSRP, RSRQ and the radio link monitoring),*/
  nb1_l1_subfm_pattern_cfg_param_s meas_subfm_pattern_pcell;
  /*! Time domain resource restriction pattern applicable to intra-freq neighbour cell
      RSRP and RSRQ measurements */

  /* =========================== */
  /* MAC layer config parameters */
  /* =========================== */

  nb1_mac_dl_cfg_s  mac_dl_cfg; /*!< Configuration Information for MAC DL */
  nb1_mac_ul_cfg_s  mac_ul_cfg; /*!< Configuration Information for MAC UL */

  uint8 mac_num_active_lc;
  nb1_lc_id_t mac_active_lc[NB1_MAX_ACTIVE_LC];
  nb1_mac_lcqos_cfg_s mac_lc_list[NB1_MAX_LC];
  boolean mac_tdd_subframe_assign_enabled;  /*!< If TDD subframe cfg is enabled in MAC */

  /* =========================== */
  /* RLC layer config parameters */
  /* =========================== */
  uint8 rlcdl_num_active_rb; /*!< Number of active RBs in RLC DL */
  nb1_rb_cfg_idx_t rlcdl_active_rb_cfg_idx[NB1_MAX_ACTIVE_RB]; /*!< Array of indices of active RBs into base cfg info*/
  nb1_rlcdl_rb_info_s rlcdl_rb_info[NB1_MAX_RB_CFG_IDX]; /*!< RLC DL base cfg info */

  uint8 rlcul_num_active_rb; /*!< Number of active RBs in RLC UL */
  nb1_rb_cfg_idx_t rlcul_active_rb_cfg_idx[NB1_MAX_ACTIVE_RB]; /*!< Array of indices of active RBs into base cfg info */
  nb1_rlcul_rb_info_s rlcul_rb_info[NB1_MAX_RB_CFG_IDX]; /*!< RLC UL base cfg info */

  /* ============================= */
  /* PDCP layer config parameters  */
  /* ============================= */
  uint8 pdcpdl_num_active_rb; /*!< Number of active RBs in PDCP DL */
  nb1_rb_cfg_idx_t pdcpdl_active_rb_cfg_idx[NB1_MAX_ACTIVE_RB]; /*!< Array of indices of active RBs into base cfg info*/
  nb1_pdcpdl_info_s pdcpdl_info[NB1_MAX_RB_CFG_IDX]; /*!< PDCP DL base cfg info */
  uint8 pdcpul_num_active_rb; /*!< Number of active RBs in PDCP UL */
  nb1_rb_cfg_idx_t pdcpul_active_rb_cfg_idx[NB1_MAX_ACTIVE_RB]; /*!< Array of indices of active RBs into base cfg info*/
  nb1_pdcpul_info_s pdcpul_info[NB1_MAX_RB_CFG_IDX]; /*!< PDCP UL base cfg info */ 

} nb1_rrc_saved_cfg_db_type;

typedef struct
{
  boolean has_valid_saved_context;
  nb1_rrc_saved_cfg_db_type saved_cfg_db;
  nb1_rrc_osys_ResumeIdentity_r13 resume_id; /*!<  redume identity from RRC Connection release DLM*/
}nb1_rrc_psm_saved_cfg_db;

typedef struct
{
 /*Paging DLM size and pdu */
 uint32 encoded_page_size;
 uint8  encoded_page_msg[NB1_RRC_PSM_POI_ENCODED_PAGE_SIZE];
 nb1_sfn_s sfn; /*!< SFN on which the msg was received */
}nb1_rrc_psm_poi_page_pdu_data_s;

typedef struct
{
 /*! LTE specific cell info for reselection candidate*/
 nb1_cphy_resel_poi_params_s  target_cell_params;
 /*Paging DLM size and pdu */
 nb1_rrc_psm_poi_page_pdu_data_s page_pdu;
 /*RTC error in units of ms for each sec of PSM sleep */
 int32                              rtc_timing_err;
}nb1_rrc_psm_poi_data_u;

typedef struct
{
  /* MSGR data */
  nb1_ml1_poi_psm_img_switch_type_e img_switch_reason;
  nb1_rrc_psm_poi_data_u   msgr_data;
} nb1_rrc_psm_img_switch_data_s;

/*! @brief PSM context save structure
*/
typedef struct
{
  uint8 llc_context_present;
  nb1_rrc_psm_img_switch_data_s img_switch_data;
  lte_rrc_ue_id_efs_s    ue_id;   /*Used by POI */
  nb1_rrc_sib_db_psm_efs psm_sib_db;
}nb1_rrc_psm_context_save_s;

typedef struct
{
  uint8 llc_context_present;
  nb1_rrc_psm_img_switch_data_s img_switch_data;
  lte_rrc_ue_id_efs_s    ue_id;   /*Used by POI */
  nb1_rrc_psm_saved_cfg_db psm_cfg_db;
  nb1_rrc_psm_saved_sec_db psm_sec_db;
  nb1_rrc_sib_db_psm_efs psm_sib_db;
  /*psm_sib_db should be the last structure. Any new data has to be add before that */
}nb1_rrc_edrx_psm_context_save_s;

/*! @brief Internal indications that various RRC procedures throw
*/
enum
{
  /*Internal Indications */

  NB1_RRC_INDI_FIRST = MSGR_UMID_BASE(MSGR_NB1_RRC, MSGR_TYPE_INDI),

  /*RRC Controller*/
  MSGR_DEFINE_UMID(NB1, RRC, INDI, STOPPED,
                   NB_RRC_INDI_STOPPED,
                   nb1_rrc_stopped_indi_s),

  /*SIB*/
  MSGR_DEFINE_UMID(NB1, RRC, INDI, UNSOLICITED_SIB,
                   NB_RRC_INDI_UNSOLICITED_SIB,
                   nb1_rrc_unsolicited_sib_indi_s),

  MSGR_DEFINE_UMID(NB1, RRC, INDI, SIB_UPDATED,
                   NB_RRC_INDI_SIB_UPDATED,
                   nb1_rrc_sib_updated_indi_s),

  MSGR_DEFINE_UMID(NB1, RRC, INDI, PENDED_SIB_UPDATED, 
                   NB_RRC_INDI_PENDED_SIB_UPDATED,
                   nb1_rrc_sib_updated_indi_s),
                   
  MSGR_DEFINE_UMID(NB1, RRC, INDI, SIB_READ_ERROR,
                   NB_RRC_INDI_SIB_READ_ERROR,
                   nb1_rrc_sib_read_error_indi_s),

  /*CEP*/

  MSGR_DEFINE_UMID(NB1, RRC, INDI, CONN_ESTABLISHMENT_STARTED,
                   NB_RRC_INDI_CONN_ESTABLISHMENT_STARTED,
                   nb1_rrc_conn_establishment_started_indi_s),
  MSGR_DEFINE_UMID(NB1, RRC, INDI, CONNECTED,
                   NB_RRC_INDI_CONNECTED,
                   nb1_rrc_connected_indi_s),
  MSGR_DEFINE_UMID(NB1, RRC, INDI, CONN_ESTABLISHMENT_FAILURE,
                   NB_RRC_INDI_CONN_ESTABLISHMENT_FAILURE,
                   nb1_rrc_conn_establishment_failure_indi_s),
  MSGR_DEFINE_UMID(NB1, RRC, INDI, STOP_CELL_RESEL,
                   NB_RRC_INDI_STOP_CELL_RESEL,
                   nb1_rrc_stop_cell_resel_indi_s),
  MSGR_DEFINE_UMID(NB1, RRC, INDI, PROCEED_WITH_RESEL,
                   NB_RRC_INDI_PROCEED_WITH_RESEL,
                   nb1_rrc_proceed_with_resel_indi_s),
  MSGR_DEFINE_UMID(NB1, RRC, INDI, OOS,
                   NB_RRC_INDI_OOS,
                   nb1_rrc_oos_indi_s),
  MSGR_DEFINE_UMID(NB1, RRC, INDI, TX_UNAVAILABLE,
                   NB_RRC_INDI_TX_UNAVAILABLE,
                   nb1_rrc_tx_unavailable_indi_s),
  MSGR_DEFINE_UMID(NB1, RRC, INDI, TX_AVAILABLE,
                   NB_RRC_INDI_TX_AVAILABLE,
                   nb1_rrc_tx_available_indi_s),

  MSGR_DEFINE_UMID(NB1, RRC, INDI, PROCEED_WITH_RACH_OPT,
                   NB_RRC_INDI_PROCEED_WITH_RACH_OPT,
                   nb1_rrc_proceed_with_rach_opt_indi_s),
  /*Paging*/
  /*CRP*/

  MSGR_DEFINE_UMID(NB1, RRC, INDI, CONN_REL_STARTED,
                   NB_RRC_INDI_CONN_REL_STARTED,
                   nb1_rrc_conn_rel_started_indi_s),

  MSGR_DEFINE_UMID(NB1, RRC, INDI, CONN_RELEASED,
                   NB_RRC_INDI_CONN_RELEASED,
                   nb1_rrc_conn_released_indi_s),

  MSGR_DEFINE_UMID(NB1, RRC, INDI, REDIR_CARRIER_INFO,
                   NB_RRC_INDI_REDIR_CARRIER_INFO,
                   nb1_rrc_redir_carrier_info_indi_s),                 
 /*CSP*/
  MSGR_DEFINE_UMID(NB1, RRC, INDI, NOT_CAMPED,
                   NB_RRC_INDI_NOT_CAMPED,
                   nb1_rrc_not_camped_indi_s),

  MSGR_DEFINE_UMID(NB1, RRC, INDI, CAMPED,
                   NB_RRC_INDI_CAMPED,
                   nb1_rrc_camped_indi_s),

  MSGR_DEFINE_UMID(NB1, RRC, INDI, CELL_RESEL_STARTED,
                   NB_RRC_INDI_CELL_RESEL_STARTED,
                   nb1_rrc_cell_resel_started_indi_s),

  MSGR_DEFINE_UMID(NB1, RRC, INDI, CELL_RESEL_CANCELED,
                   NB_RRC_INDI_CELL_RESEL_CANCELED,
                   nb1_rrc_cell_resel_canceled_indi_s),

  MSGR_DEFINE_UMID(NB1, RRC, INDI, CONN_MODE_FAILURE,
                   NB_RRC_INDI_CONN_MODE_FAILURE,
                   nb1_rrc_conn_mode_failure_indi_s),

  MSGR_DEFINE_UMID(NB1, RRC, INDI, RB_RESUMED,
                   NB_RRC_INDI_RB_RESUMED,
                   nb1_rrc_rb_resumed_indi_s),

  MSGR_DEFINE_UMID(NB1, RRC, INDI, SHIM_LAYER_PARAM_UPDATE,
                   NB_RRC_INDI_SHIM_LAYER_PARAM_UPDATE,
                   nb1_rrc_shim_layer_param_udpate_s),
                   
  /* CRE */
  MSGR_DEFINE_UMID(NB1, RRC, INDI, CRE_STARTED,
                   NB_RRC_INDI_CRE_STARTED,
                   nb1_rrc_cre_started_indi_s),

  MSGR_DEFINE_UMID(NB1, RRC, INDI, CRE_COMPLETED,
                   NB_RRC_INDI_CRE_COMPLETED,
                   nb1_rrc_cre_completed_indi_s),

  MSGR_DEFINE_UMID(NB1, RRC, INDI, INITIATE_CONN_REL,
                   NB_RRC_INDI_INITIATE_CONN_REL,
                   nb1_rrc_initiate_conn_rel_indi_s),
  /*LLC*/
  MSGR_DEFINE_UMID(NB1, RRC, INDI, SERVING_CELL_CHANGED,
                   NB_RRC_INDI_SERVING_CELL_CHANGED,
                   nb1_rrc_serving_cell_changed_indi_s),

  MSGR_DEFINE_UMID(NB1, RRC, INDI, LLC_MBSFN_CFG_CHANGED,
                   NB_RRC_INDI_LLC_MBSFN_CFG_CHANGED,
                   nb1_rrc_llc_mbsfn_cfg_chng_indi_s),

  /*Dispatcher*/
  MSGR_DEFINE_UMID(NB1, RRC, INDI, PENDING_MSG,
                   NB_RRC_INDI_PENDING_MSG,
                   none),

  /*Measurement*/
  MSGR_DEFINE_UMID(NB1, RRC, INDI, MEAS_PENDING_SIB_UPDATED,
                   NB_RRC_INDI_MEAS_PENDING_SIB_UPDATED,
                   nb1_rrc_sib_updated_indi_s),

  /*Security*/
  MSGR_DEFINE_UMID(NB1, RRC, INDI, SMC_FAILURE,
                   NB_RRC_INDI_SMC_FAILURE,
                   nb1_rrc_smc_failure_indi_s),

  MSGR_DEFINE_UMID(NB1, RRC, INDI, CONFIG_SAVED_SEC,
                   NB_RRC_INDI_SMC_CONFIGURE,
                   nb1_rrc_smc_config_indi_s),

  /*Message Handler*/
  MSGR_DEFINE_UMID(NB1, RRC, INDI, DLM_PROCESSED,
                   NB_RRC_INDI_DLM_PROCESSED,
                   nb1_rrc_dlm_processed_indi_s),

  /* Contains information of new cell */
  MSGR_DEFINE_UMID(NB1, RRC, INDI, NEW_CELL_INFO,
                   NB_RRC_INDI_NEW_CELL_INFO,
                   nb1_rrc_new_cell_info_indi_s),

  /* Connectedmode interfrequency neighbor list indication*/
  MSGR_DEFINE_UMID(NB1, RRC, INDI, INTERFREQ_LIST_UPDATE,
                   NB_RRC_INDI_INTERFREQ_LIST_UPDATE,
                   nb1_rrc_interfreq_list_update_indi_s),

  /*Sibs NOT rcvd Indication*/
  MSGR_DEFINE_UMID(NB1, RRC, INDI, SIB_NOT_RCVD,
                   NB_RRC_INDI_SIB_NOT_RCVD,
                   nb1_rrc_sib_not_rcvd_indi_s),

  /* NB1 RRC MBSFN SNR indication*/
  MSGR_DEFINE_UMID(NB1, RRC, INDI, MBSFN_SNR,
                   NB_RRC_INDI_MBSFN_SNR,
                   nb1_rrc_mbsfn_snr_indi_s),

  /* NB1 RRC NV Refresh indication by RRC MISC */
  MSGR_DEFINE_UMID(NB1, RRC, INDI, NV_REFRESH,
                   NB_RRC_INDI_NV_REFRESH,
                   nb1_rrc_nv_refresh_indi_s),

   /*NB1 RRC Sib1 avail indi sent by nb1_rrc_sib.c*/
  MSGR_DEFINE_UMID(NB1, RRC, INDI, SIB1_AVAIL,
                   NB_RRC_INDI_SIB1_AVAIL,
                   nb1_rrc_sib1_avail_indi_s),

  NB1_RRC_INDI_MAX,
  NB1_RRC_INDI_LAST = NB1_RRC_INDI_MAX - 1
};

/*! @brief Data structure for DL message indications
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  void *dl_msg_ptr; /*!< Pointer to decoded DL msg */
  lte_rrc_cell_id_s cell_id; /*!< Cell identity */
  nb1_sfn_s sfn; /*!< SFN on which the msg was received */
  int pdu_num; /*!< PDU number of the message; to be used by dispatcher to free
                    the decoded msg*/
  /*Security related data... specifically for SMC*/
  uint8 pdcp_hdr;/*!< PDCP PDU hdr*/
  uint32 count; /*!< PDCP DL count*/
  uint8 mac_i[NB1_MACI_LEN]; /*!< MAC_I */
  boolean is_img_switch; /* TRUE: If this message forwarded by CSP after image switch..FALSE: Legacy behaviour */
} nb1_rrc_mh_dlm_s;

/*! @brief Enumeration of DLM id nums
*/
typedef enum
{
  /*Message Handler*/
  NB_RRC_DLM_RRC_CONNECTION_REESTABLISHMENT = 0x0,
  NB_RRC_DLM_RRC_CONNECTION_REESTABLISHMENT_REJECT = 0x01,
  NB_RRC_DLM_RRC_CONNECTION_REJECT = 0x02,
  NB_RRC_DLM_RRC_CONNECTION_SETUP = 0x03,
  NB_RRC_DLM_DL_INFORMATION_TRANSFER = 0x04,
  NB_RRC_DLM_RRC_CONNECTION_RECONFIGURATION = 0x05,
  NB_RRC_DLM_RRC_CONNECTION_RELEASE = 0x06,
  NB_RRC_DLM_SECURITY_MODE_COMMAND = 0x07,
  NB_RRC_DLM_UE_CAPABILITY_ENQUIRY = 0x08,
  NB_RRC_DLM_RRC_CONNECTION_RESUME = 0x09,
  NB_RRC_DLM_PAGING = 0x0a,
  /*RRC Controller*/
  /*SIB*/
  /*CEP*/
  /*Paging*/
  /*CRP*/
  /*CSP*/
  /*Config*/
  /*LLC*/
  /*Measurement*/
  NB_RRC_DLM_MAX     /*!< Max value */
}nb1_rrc_dlm_type_e;

 /*! @brief Downlink Message indications from Message Handler
*/
enum
{
  /*Downlink Message Indications */

  NB1_RRC_DLM_FIRST = MSGR_UMID_BASE(MSGR_NB1_RRC, MSGR_TYPE_DLM),

  /*Message Handler*/

  MSGR_DEFINE_UMID(NB1, RRC, DLM, RRC_CONNECTION_REESTABLISHMENT,
                   NB_RRC_DLM_RRC_CONNECTION_REESTABLISHMENT,
                   nb1_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(NB1, RRC, DLM, RRC_CONNECTION_REESTABLISHMENT_REJECT,
                   NB_RRC_DLM_RRC_CONNECTION_REESTABLISHMENT_REJECT,
                   nb1_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(NB1, RRC, DLM, RRC_CONNECTION_REJECT,
                   NB_RRC_DLM_RRC_CONNECTION_REJECT,
                   nb1_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(NB1, RRC, DLM, RRC_CONNECTION_SETUP,
                   NB_RRC_DLM_RRC_CONNECTION_SETUP,
                   nb1_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(NB1, RRC, DLM, DL_INFORMATION_TRANSFER,
                   NB_RRC_DLM_DL_INFORMATION_TRANSFER,
                   nb1_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(NB1, RRC, DLM, RRC_CONNECTION_RECONFIGURATION,
                   NB_RRC_DLM_RRC_CONNECTION_RECONFIGURATION,
                   nb1_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(NB1, RRC, DLM, RRC_CONNECTION_RELEASE,
                   NB_RRC_DLM_RRC_CONNECTION_RELEASE,
                   nb1_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(NB1, RRC, DLM, SECURITY_MODE_COMMAND,
                   NB_RRC_DLM_SECURITY_MODE_COMMAND,
                   nb1_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(NB1, RRC, DLM, UE_CAPABILITY_ENQUIRY,
                   NB_RRC_DLM_UE_CAPABILITY_ENQUIRY,
                   nb1_rrc_mh_dlm_s),
  MSGR_DEFINE_UMID(NB1, RRC, DLM, RRC_CONNECTION_RESUME,
                   NB_RRC_DLM_RRC_CONNECTION_RESUME,
                   nb1_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(NB1, RRC, DLM, PAGING,
                   NB_RRC_DLM_PAGING,
                   nb1_rrc_mh_dlm_s),

  NB1_RRC_DLM_MAX,
  NB1_RRC_DLM_LAST = NB1_RRC_DLM_MAX - 1

};

/*! @brief Data structure for Timer expiry indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} nb1_rrc_tmri_s;

/*! @brief Enumeration of TMRI id nums
*/
/*********************************************************************
   DO NOT CHANGE THE ORDER OF THE ENUM BELOW - USED FOR EVENT LOGGING.
   ALWAYS ADD YOUR NEW TIMER AT THE BOTTOM
 *********************************************************************/
typedef enum
{
  NB_RRC_TMRI_SIB_EVENT1_WT = 0x0, /* do not change */
  NB_RRC_TMRI_SIB_EVENT2_WT = 0x01, /* do not change */
  NB_RRC_TMRI_SIB_24HR_CLOCK_TICK = 0x02, /* do not change */
  NB_RRC_TMRI_T300 = 0x03, /* do not change */
  NB_RRC_TMRI_T301 = 0x04, /* do not change */
  NB_RRC_TMRI_T302 = 0x05, /* do not change */
  NB_RRC_TMRI_T303 = 0x06, /* do not change */
  NB_RRC_TMRI_T305 = 0x07, /* do not change */
  NB_RRC_TMRI_OOS = 0x09, /* do not change */
  NB_RRC_TMRI_CELL_RESEL_STARTED = 0x0a, /* do not change */
  NB_RRC_TMRI_T304 = 0x0b, /* do not change */
  NB_RRC_TMRI_CONN_REL = 0x0c, /* do not change */
  NB_RRC_TMRI_DEADLOCK = 0x0d, /* do not change */
  NB_RRC_TMRI_CEP_RESEL = 0x0e, /* do not change */
  /*CRE*/
  NB_RRC_TMRI_T311 = 0x0f, /* do not change */
  /*SEC*/
  NB_RRC_TMRI_SMC_FAIL = 0x10, /*do not change*/
  NB_RRC_TMRI_T320 = 0x11, /* do not change */
  NB_RRC_TMRI_PLMN_SEARCH_GUARD = 0x12, /* do not change */
  NB_RRC_TMRI_PERIODIC_SIB8    = 0x17, /* do not change */
  NB_RRC_TMRI_T321 = 0x18, /* do not change */
  NB_RRC_TMRI_PLMN_SEARCH_ACQ_DB = 0x19, /* do not change */
  NB_RRC_TMRI_OOS_SYSTEM_SCAN = 0x1a, /* do not change */
  NB_RRC_TMRI_PLMN_SEARCH_MSEARCH_BSCAN_SEP = 0x1e, /* do not change */
  NB_RRC_TMRI_CTLR_DEADLOCK = 0x2a, /* do not change */
  NB_RRC_TMRI_RECONFIG_WAIT = 0x2b, /* do not change */
  NB_RRC_TMRI_CTLR_CONNECTED_DURATION = 0x2c, /* do not change */
  NB_RRC_TMRI_CTLR_TX_UNAVAILABLE = 0x2d, /* do not change */
  /*RLF*/
  NB_RRC_TMRI_RLF_SYSTEM_SCAN = 0x33, /* do not change */
  NB_RRC_TMRI_SYSTEM_SCAN_PRIOR_T311_EXPIRY = 0x34, /* do not change */
  NB_RRC_TMRI_PERIODIC_SIB16    = 0x35, /* do not change */
  NB_RRC_TMRI_ESMGR_CLEAR_CFL_WAIT = 0x39, /* do not change */
  NB_RRC_TMRI_PLMN_SEARCH_FIRSTP = 0x3a, /* do not change */
  NB_RRC_TMRI_PLMN_SEARCH_MOREP = 0x3b, /* do not change */
  NB_RRC_TMRI_CEP_DEADLOCK = 0x3e,  /* do not change */
  NB_RRC_TMRI_CEP_ACB_DELAY_RACH = 0x3F, /* do not change */
  NB_RRC_TMRI_CEP_TX_UNAVAILABLE = 0x40, /* do not change */
  NB_RRC_TMRI_T322 = 0x41,  /* do not change */  
  NB_RRC_TMRI_MAX     /*!< Max value */
}nb1_rrc_tmri_type_e;

/*! @brief Timers maintained by the RRC procedures
*/
enum
{
  /* Internal Timers */

  NB1_RRC_TMRI_FIRST = MSGR_UMID_BASE(MSGR_NB1_RRC, MSGR_TYPE_TMRI),

  /*Message Handler*/

  /*RRC Controller*/
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, CTLR_DEADLOCK,
                   NB_RRC_TMRI_CTLR_DEADLOCK, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, CTLR_CONNECTED_DURATION,
                   NB_RRC_TMRI_CTLR_CONNECTED_DURATION, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, CTLR_TX_UNAVAILABLE,
                     NB_RRC_TMRI_CTLR_TX_UNAVAILABLE, nb1_rrc_tmri_s),

  /*SIB*/
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, SIB_EVENT1_WT,
                   NB_RRC_TMRI_SIB_EVENT1_WT, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, SIB_EVENT2_WT,
                   NB_RRC_TMRI_SIB_EVENT2_WT, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, SIB_24HR_CLOCK_TICK,
                   NB_RRC_TMRI_SIB_24HR_CLOCK_TICK, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, PERIODIC_SIB8,
                   NB_RRC_TMRI_PERIODIC_SIB8, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, PERIODIC_SIB16,
                   NB_RRC_TMRI_PERIODIC_SIB16, nb1_rrc_tmri_s),

  /*CEP*/
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, T300,
                   NB_RRC_TMRI_T300, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, T301,
                   NB_RRC_TMRI_T301, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, T302,
                   NB_RRC_TMRI_T302, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, T303,
                   NB_RRC_TMRI_T303, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, T305,
                   NB_RRC_TMRI_T305, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, CEP_RESEL,
                   NB_RRC_TMRI_CEP_RESEL, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, CEP_DEADLOCK,
                   NB_RRC_TMRI_CEP_DEADLOCK, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, CEP_ACB_DELAY_RACH,
                   NB_RRC_TMRI_CEP_ACB_DELAY_RACH, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, CEP_TX_UNAVAILABLE,
                     NB_RRC_TMRI_CEP_TX_UNAVAILABLE, nb1_rrc_tmri_s),
  /*Paging*/

  /*CSP*/

  MSGR_DEFINE_UMID(NB1, RRC, TMRI, OOS,
                   NB_RRC_TMRI_OOS, nb1_rrc_tmri_s),

  MSGR_DEFINE_UMID(NB1, RRC, TMRI, OOS_SYSTEM_SCAN,
                   NB_RRC_TMRI_OOS_SYSTEM_SCAN, nb1_rrc_tmri_s),

  MSGR_DEFINE_UMID(NB1, RRC, TMRI, RLF_SYSTEM_SCAN,
                   NB_RRC_TMRI_RLF_SYSTEM_SCAN, nb1_rrc_tmri_s),

  MSGR_DEFINE_UMID(NB1, RRC, TMRI, SYSTEM_SCAN_PRIOR_T311_EXPIRY,
                   NB_RRC_TMRI_SYSTEM_SCAN_PRIOR_T311_EXPIRY, nb1_rrc_tmri_s),

  /*Config*/
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, T304,
                   NB_RRC_TMRI_T304, nb1_rrc_tmri_s),
  /*CRP*/
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, CONN_REL,
                   NB_RRC_TMRI_CONN_REL, nb1_rrc_tmri_s),
  /*LLC*/
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, DEADLOCK,
                   NB_RRC_TMRI_DEADLOCK, nb1_rrc_tmri_s),

  /* CRE */
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, T311,
                   NB_RRC_TMRI_T311, nb1_rrc_tmri_s),

  /*SEC*/
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, SMC_FAIL,
                   NB_RRC_TMRI_SMC_FAIL, nb1_rrc_tmri_s),

  /*Measurement*/
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, T320,
                   NB_RRC_TMRI_T320, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, T321,
                   NB_RRC_TMRI_T321, nb1_rrc_tmri_s),

  /*PLMN Search*/
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, PLMN_SEARCH_GUARD,
                   NB_RRC_TMRI_PLMN_SEARCH_GUARD, nb1_rrc_tmri_s),

  MSGR_DEFINE_UMID(NB1, RRC, TMRI, PLMN_SEARCH_ACQ_DB,
                   NB_RRC_TMRI_PLMN_SEARCH_ACQ_DB, nb1_rrc_tmri_s),

  MSGR_DEFINE_UMID(NB1, RRC, TMRI, PLMN_SEARCH_MSEARCH_BSCAN_SEP,
                   NB_RRC_TMRI_PLMN_SEARCH_MSEARCH_BSCAN_SEP, nb1_rrc_tmri_s),

  MSGR_DEFINE_UMID(NB1, RRC, TMRI, PLMN_SEARCH_FIRSTP,
                   NB_RRC_TMRI_PLMN_SEARCH_FIRSTP, nb1_rrc_tmri_s),

  MSGR_DEFINE_UMID(NB1, RRC, TMRI, PLMN_SEARCH_MOREP,
                   NB_RRC_TMRI_PLMN_SEARCH_MOREP, nb1_rrc_tmri_s),

  /* RRC Reconfig Wait timer */
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, RECONFIG_WAIT,
                   NB_RRC_TMRI_RECONFIG_WAIT, nb1_rrc_tmri_s),
  MSGR_DEFINE_UMID(NB1, RRC, TMRI, T322,
                   NB_RRC_TMRI_T322, nb1_rrc_tmri_s),

  NB1_RRC_TMRI_MAX,
  NB1_RRC_TMRI_LAST = NB1_RRC_TMRI_MAX - 1
};

/*! @brief This structure stores an encoded SI
*/
typedef struct
{
  /*!< Encoded SI */
  uint8 encoded_si[NB1_RRC_MAX_SI_LENGTH];

  /*!< Size of encoded SI in bytes */
  uint32 encoded_si_size;

  /*!< Encoded SI pointer - used when encoded msg is longer than
       NB1_RRC_MAX_SI_LENGTH */
  uint8 *encoded_si_ptr;
} nb1_rrc_sib_encoded_si_info_s;

/*! @brief This structure holds all encoded & decoded SIs of a cell
*/
typedef struct
{
  /*!< Array of encoded SIs */
  nb1_rrc_sib_encoded_si_info_s encoded_si_info[NB1_RRC_SIB_MAX_SIS];

  /*!< Array of decoded SIs */
  nb1_rrc_osys_BCCH_DL_SCH_Message_NB *decoded_si_ptr[NB1_RRC_SIB_MAX_SIS];

  /*!< SIBs contained in SI */
  nb1_rrc_sib_bitmask_t sibs_in_si_bitmask[NB1_RRC_SIB_MAX_SIS];

  /*!< Bitmask indicating what SIBs are present */
  nb1_rrc_sib_bitmask_t sibs_bitmask;

  /*!< Indicates whether decoded SIs are present or not. */
  boolean si_decoded;
} nb1_rrc_sib_all_si_s;

/*! @brief This structure stores pointers to decoded SIBs
*/
typedef struct
{
  /*!< Pointer to SIB2 */
  nb1_rrc_osys_SystemInformationBlockType2_NB_r13 *sib2_ptr;
  /*!< Pointer to SIB3 */
  nb1_rrc_osys_SystemInformationBlockType3_NB_r13 *sib3_ptr;
  /*!< Pointer to SIB4 */
  nb1_rrc_osys_SystemInformationBlockType4_NB_r13 *sib4_ptr;
  /*!< Pointer to SIB5 */
  nb1_rrc_osys_SystemInformationBlockType5_NB_r13 *sib5_ptr;
  /*!< Pointer to SIB14 */
  nb1_rrc_osys_SystemInformationBlockType14_NB_r13 *sib14_ptr;
  /*!< Pointer to SIB16 */
  nb1_rrc_osys_SystemInformationBlockType16_NB_r13 *sib16_ptr;
  /*!< Pointer to SIB16 */
  nb1_rrc_osys_SystemInformationBlockType22_NB_r14 *sib22_ptr;

} nb1_rrc_sib_all_decoded_sibs_s;

/*! @brief This structure holds a cell in the SIB db
*/
typedef struct
{
  /*!< Indicates if the info contained here is valid */
  boolean index_valid;

  /*!< To maintain age or rank of a cell in db; used for LRU algo used to remove
       cells from db when new ones are added (0 indicates youngest or most
       recently used cell) */
  uint8 index_rank;

  /*!< 10 min counter; used for 3hr timer */
  uint8 index_counter;

  /*!< Index corresponding to band chosen in multiBandInfoList */
  uint8 mfbi_index;

  /*!< Encoded & decoded SIs */
  nb1_rrc_sib_all_si_s all_si;

  /*!< Encoded SIB1 */
  nb1_rrc_sib_encoded_si_info_s encoded_sib1;

  /*!< Decoded SIBs (SIB2 - SIB12) */
  nb1_rrc_sib_all_decoded_sibs_s all_decoded_sibs;

  /*!< Cell Identity */
  nb1_rrc_sib_cell_id_s sib_cell_id;

  /*!< MIB */
  nb1_l1_nb_mib_info_param_s mib;

  /*! Timestamp of last read sib in DB */
  uint64 last_read_sib_timestamp;

  /*!< SI value tag */
  uint8 si_value_tag[NB1_CPHY_MAX_SI_MSGS];

  /*!< Decoded SIB1 */
  nb1_rrc_osys_BCCH_DL_SCH_Message_NB *sib1_ptr;

  /*!< Value tag */
  uint32 value_tag;

  /*! SFN of the last read SIB */
  nb1_l1_sfn_t last_read_sib_sfn;
} nb1_rrc_sib_cell_s;

/*! @brief This structure holds the SIB db
*/
typedef struct
{
  /*!< Cells in the db */
  nb1_rrc_sib_cell_s cell[NB1_RRC_SIB_DB_MAX_CELLS];
} nb1_rrc_sib_db_s;



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/* Returns a pointer to acq db */
extern nb1_rrc_csp_efs_acq_db_v3_s* _nb1_rrc_csp_get_efs_acq_db(void);

extern nb1_rrc_csp_static_data_s* _nb1_rrc_csp_get_csp_static_data(void);

/* Returns a pointer to lte_rrc_sib_db_s*/
extern nb1_rrc_sib_db_s* _nb1_rrc_sib_get_sib_db(void);

extern void _nb1_rrc_sib_copy_sib_db_from_shim_layer
(
  uint8 db_index,  /*!< Index of cell in sib_db */
  nb1_rrc_sib_cell_s *cell_ptr
);

extern uint8 _nb1_rrc_sib_get_cell_info_age_efs(void);

#endif /* NB1_RRC_INT_MSG_H */
