/*!
  @file lte_rrc_cre_test_common.cpp

  @brief
  Common utilities for cre test

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/cre/lte_rrc_cre_test_common.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/08   mm     Initial Revision 
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "lte_rrc_sib.h"
#include "lte_rrc_csp.h"
#include "lte_rrc_config.h"
#include "lte_rrc_controller.h"
#include "lte.h"
#include "lte_rrc_int_msg.h"
#include "__lte_rrc_cre.h"
#include "lte_rrc_utils.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_cre.h"
#include "lte_rrc_crei.h"
#include "lte_rrc_sec.h"
#include "lte_mac_msg.h"
#include "lte_mac_ind.h"
#include "lte_mac_rrc.h"
}

#include <tf_stub.h>
#include <TestFramework.h>
#include "lte_rrc_utf.h"
#include "lte_rrc_cre_test_common.h"

extern "C"
{
  TF_PROTO_STUB( stm_state_t, lte_rrc_controller_get_state, void);
  TF_PROTO_STUB(void,lte_rrc_llc_get_ho_source_cell_info,lte_rrc_cell_info_s *);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s*);
  TF_PROTO_STUB(void*, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t,lte_rrc_sib_db_key_t);
  TF_PROTO_STUB(boolean, lte_rrc_config_ho_in_progress, void);
  TF_PROTO_STUB(lte_rnti_val_t, lte_rrc_llc_get_crnti, void);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_llc_update_crnti_in_cfgdb, lte_rnti_val_t);
  TF_PROTO_STUB(uint16, lte_rrc_sec_compute_short_maci, lte_phy_cell_id_t, lte_rnti_val_t);
  TF_PROTO_STUB( boolean, lte_rrc_sec_security_is_configured, void);
  TF_PROTO_STUB( stm_state_t, lte_rrc_config_get_state, void);
  TF_PROTO_STUB(boolean, lte_rrc_config_rb_reconfig_in_progress, void);
  TF_PROTO_STUB(boolean, lte_rrc_config_meas_reconfig_in_progress, void);
  TF_PROTO_STUB(boolean, lte_rrc_llc_get_dedicated_rlf_timers, lte_rrc_rlf_tmr_s *);
  TF_PROTO_STUB(void, lte_rrc_mh_get_last_encoded_message_rcvd, lte_rrc_pdu_buf_s *);
}

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_msg_ptr = NULL; 

/*===========================================================================
                     Basic SIB 2 message 
===========================================================================*/

/* Basic SIB2 message:
sib2:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib2 : 
                {
                  radioResourceConfigCommon 
                  {
                    rach-Config 
                    {
                      preambleInfo 
                      {
                        numberOfRA-Preambles n12,
                        preamblesGroupAConfig 
                        {
                          sizeOfRA-PreamblesGroupA n12,
                          messageSizeGroupA b56,
                          messagePowerOffsetGroupB minusinfinity
                        }
                      },
                      powerRampingParameters 
                      {
                        powerRampingStep dB2,
                        preambleInitialReceivedTargetPower dBm-118
                      },
                      ra-SupervisionInfo 
                      {
                        preambleTransMax n3,
                        ra-ResponseWindowSize sf5,
                        mac-ContentionResolutionTimer sf40
                      },
                      maxHARQ-Msg3Tx 3
                    },
                    bcch-Config 
                    {
                      modificationPeriodCoeff n4
                    },
                    pcch-Config 
                    {
                      defaultPagingCycle rf32,
                      nB fourT
                    },
                    prach-Config 
                    {
                      rootSequenceIndex 3,
                      prach-ConfigInfo 
                      {
                        prach-ConfigIndex 5,
                        highSpeedFlag FALSE,
                        zeroCorrelationZoneConfig 0,
                        prach-FreqOffset 94
                      }
                    },
                    pdsch-Config 
                    {
                      referenceSignalPower 0,
                      p-b 2
                    },
                    pusch-Config 
                    {
                      pusch-ConfigBasic 
                      {
                        n-SB 3,
                        hoppingMode intraAndInterSubFrame,
                        pusch-HoppingOffset 52,
                        enable64QAM FALSE
                      },
                      ul-ReferenceSignalsPUSCH 
                      {
                        groupHoppingEnabled TRUE,
                        groupAssignmentPUSCH 2,
                        sequenceHoppingEnabled TRUE,
                        cyclicShift 6
                      }
                    },
                    pucch-Config 
                    {
                      deltaPUCCH-Shift ds3,
                      nRB-CQI 98,
                      nCS-AN 6,
                      n1PUCCH-AN 1023
                    },
                    soundingRS-UL-Config setup : 
                      {
                        srs-BandwidthConfig bw2,
                        srs-SubframeConfig sc3,
                        ackNackSRS-SimultaneousTransmission FALSE,
                        srs-MaxUpPts true
                      },
                    uplinkPowerControl 
                    {
                      p0-NominalPUSCH 5,
                      alpha al05,
                      p0-NominalPUCCH -96,
                      deltaFList-PUCCH 
                      {
                        deltaF-PUCCH-Format1 deltaF-2,
                        deltaF-PUCCH-Format1b deltaF3,
                        deltaF-PUCCH-Format2 deltaF1,
                        deltaF-PUCCH-Format2a deltaF0,
                        deltaF-PUCCH-Format2b deltaF2
                      },
                      deltaPreambleMsg3 -1
                    },
                    ul-CyclicPrefixLength len1
                  },
                  ue-TimersAndConstants 
                  {
                    t300 ms100,
                    t301 ms100,
                    t310 ms200,
                    n310 n3,
                    t311 ms1000,
                    n311 n4
                  },
                  freqInfo 
                  {
                    ul-CarrierFreq 36199,
                    ul-Bandwidth n6,
                    additionalSpectrumEmission 5
                  },
                  timeAlignmentTimerCommon sf10240
                }
            }
          }
      }
}
*/

byte cre_basic_sib2_2[29] = {0x00, 0x00, 0x12, 0x10, 0x11, 0x07, 0x12, 0x00, 
  0x0C, 0x50, 0x5E, 0x79, 0x56, 0x88, 0xBA, 0xC5, 0x9F, 0xFE, 0x8D, 0x06, 0xBE, 
  0x33, 0x00, 0x06, 0x83, 0xE3, 0x59, 0xC1, 0x30};

/*===========================================================================

                            Reestablishment Message

============================================================================*/
/*
Reest:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionReestablishment : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReestablishment-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config explicitValue : am : 
                        {
                          ul-AM-RLC 
                          {
                            t-PollRetransmit ms15,
                            pollPDU p32,
                            pollByte kB100,
                            maxRetxThreshold t3
                          },
                          dl-AM-RLC 
                          {
                            t-Reordering ms0,
                            t-StatusProhibit ms0
                          }
                        },
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                mac-MainConfig explicitValue : 
                  {
                    timeAlignmentTimerDedicated sf1920
                  },
                sps-Config 
                {
                  sps-ConfigDL setup : 
                    {
                      semiPersistSchedIntervalDL sf20,
                      numberOfConfSPS-Processes 1,
                      n1-PUCCH-AN-PersistentList 
                      {
                        2
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw2,
                      srs-HoppingBandwidth hbw3,
                      freqDomainPosition 0,
                      duration FALSE,
                      srs-ConfigIndex 0,
                      transmissionComb 1,
                      cyclicShift cs4
                    },
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm1,
                      ue-TransmitAntennaSelection setup : openLoop
                    }
                }
              },
              nextHopChainingCount 2
            }
      }
}
*/

byte cre_reestablishment_msg[17] = {0x08, 0x13, 0x98, 0x02, 0x66, 0x80, 0x04, 0x0D, 0x42, 
  0x00, 0x04, 0x01, 0xB6, 0x00, 0x01, 0x80, 0xD0};

lte_rrc_pdu_buf_s lte_rrc_cre_test_pdu_buf;

/*===========================================================================

                         INTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_get_controller_state

===========================================================================*/
/*!
    @brief
    This function returns the controller state for test purposes

    @return
    E_SUCCESS if success, else other codes

 */
/*=========================================================================*/
stm_state_t lte_rrc_cre_test_get_controller_state
(
  void
)
{
  return LTE_RRC_CONTROLLER_SM__CONNECTED;
}
/*===========================================================================

  FUNCTION:  lte_test_stub_get_current_time_since_power_up_secs

===========================================================================*/
/*!
    @brief
    This function returns a time since power up in secs

    @return
    uint32

 */
/*=========================================================================*/
uint32 lte_test_stub_get_current_time_since_power_up_secs
(
   void
)
{
  return 100;
}
uint32 lte_test_stub_get_current_time_since_power_up_secs_2
(
   void
)
{
  return 120;
}
uint32 lte_test_stub_get_current_time_since_power_up_secs_3
(
   void
)
{
  return 130;
}
uint32 lte_test_stub_get_current_time_since_power_up_secs_4
(
   void
)
{
  return 185;
}
uint32 lte_test_stub_get_current_time_since_power_up_secs_5
(
   void
)
{
  return 190;
}
uint32 lte_test_stub_get_current_time_since_power_up_secs_6
(
   void
)
{
  return 1100;
}
/*===========================================================================

  FUNCTION:  lte_rrc_test_stub_get_rlf_window_size

===========================================================================*/
/*!
    @brief
    This function returns a the window size for repeated rlf's

    @return
    uint8

 */
/*=========================================================================*/
uint16 lte_rrc_test_stub_get_rlf_window_size
(
   void
)
{
  return 60;
}
/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_csp_get_serving_cell_params

===========================================================================*/
/*!
    @brief
    This function returns current cell ID and frequency

    @return
    E_SUCCESS if success, else other codes

 */
/*=========================================================================*/
boolean lte_rrc_test_stub_get_camped_cell_info
(
  lte_rrc_plmn_s *selected_plmn_ptr,
   /*!< Ptr to selected PLMN to be filled*/
  lte_earfcn_t *dl_earfcn_ptr, /*!< Ptr to DL Center frequency of cell */
  lte_phy_cell_id_t *phy_cell_id_ptr,/*!< Ptr to Physical cell id of cell */
  lte_rrc_global_cell_id_t *cell_identity_ptr, 
  /*!< Ptr to Global cell id (from SIB1) of cell */
  uint16 *tac_ptr /*!< Ptr to Tracking area code of cell */
)
{
  /* Just hard code to something for now*/
  *cell_identity_ptr = 1;
  *dl_earfcn_ptr = 10689;
  *phy_cell_id_ptr= 501;
  *tac_ptr = 1;

  return true;
}

boolean lte_rrc_test_stub_get_camped_cell_info_2
(
  lte_rrc_plmn_s *selected_plmn_ptr,
   /*!< Ptr to selected PLMN to be filled*/
  lte_earfcn_t *dl_earfcn_ptr, /*!< Ptr to DL Center frequency of cell */
  lte_phy_cell_id_t *phy_cell_id_ptr,/*!< Ptr to Physical cell id of cell */
  lte_rrc_global_cell_id_t *cell_identity_ptr, 
  /*!< Ptr to Global cell id (from SIB1) of cell */
  uint16 *tac_ptr /*!< Ptr to Tracking area code of cell */
)
{
  /* Just hard code to something for now*/
  *cell_identity_ptr = 2;
  *dl_earfcn_ptr = 10689;
  *phy_cell_id_ptr= 501;
  *tac_ptr = 1;

  return true;
}

boolean lte_rrc_test_stub_get_camped_cell_info_3
(
  lte_rrc_plmn_s *selected_plmn_ptr,
   /*!< Ptr to selected PLMN to be filled*/
  lte_earfcn_t *dl_earfcn_ptr, /*!< Ptr to DL Center frequency of cell */
  lte_phy_cell_id_t *phy_cell_id_ptr,/*!< Ptr to Physical cell id of cell */
  lte_rrc_global_cell_id_t *cell_identity_ptr, 
  /*!< Ptr to Global cell id (from SIB1) of cell */
  uint16 *tac_ptr /*!< Ptr to Tracking area code of cell */
)
{
  /* Just hard code to something for now*/
  *cell_identity_ptr = 3;
  *dl_earfcn_ptr = 10689;
  *phy_cell_id_ptr= 501;
  *tac_ptr = 1;

  return true;
}

boolean lte_rrc_test_stub_get_camped_cell_info_4
(
  lte_rrc_plmn_s *selected_plmn_ptr,
   /*!< Ptr to selected PLMN to be filled*/
  lte_earfcn_t *dl_earfcn_ptr, /*!< Ptr to DL Center frequency of cell */
  lte_phy_cell_id_t *phy_cell_id_ptr,/*!< Ptr to Physical cell id of cell */
  lte_rrc_global_cell_id_t *cell_identity_ptr, 
  /*!< Ptr to Global cell id (from SIB1) of cell */
  uint16 *tac_ptr /*!< Ptr to Tracking area code of cell */
)
{
  /* Just hard code to something for now*/
  *cell_identity_ptr = 4;
  *dl_earfcn_ptr = 10689;
  *phy_cell_id_ptr= 501;
  *tac_ptr = 1;

  return true;
}

boolean lte_rrc_test_stub_get_camped_cell_info_5
(
  lte_rrc_plmn_s *selected_plmn_ptr,
   /*!< Ptr to selected PLMN to be filled*/
  lte_earfcn_t *dl_earfcn_ptr, /*!< Ptr to DL Center frequency of cell */
  lte_phy_cell_id_t *phy_cell_id_ptr,/*!< Ptr to Physical cell id of cell */
  lte_rrc_global_cell_id_t *cell_identity_ptr, 
  /*!< Ptr to Global cell id (from SIB1) of cell */
  uint16 *tac_ptr /*!< Ptr to Tracking area code of cell */
)
{
  /* Just hard code to something for now*/
  *cell_identity_ptr = 5;
  *dl_earfcn_ptr = 10689;
  *phy_cell_id_ptr= 501;
  *tac_ptr = 1;

  return true;
}
/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_csp_get_serving_cell_params

===========================================================================*/
/*!
    @brief
    This function returns current cell ID and frequency

    @return
    E_SUCCESS if success, else other codes

 */
/*=========================================================================*/
lte_errno_e lte_rrc_cre_test_csp_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr /*!< Pointer to cell info */
)
{
  /* Just hard code to something for now*/
  cell_info_ptr->cell_id.freq = 10689;
  cell_info_ptr->cell_id.phy_cell_id = 501;
  cell_info_ptr->sib_db_key = 3;

  return E_SUCCESS;
}

/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_llc_get_source_cell_info

===========================================================================*/
/*!
    @brief
    This function returns source cell ID and frequency

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_cre_test_llc_get_source_cell_info
(
  lte_rrc_cell_info_s *cell_info_ptr /*!< Pointer to cell info */
)
{
  /* Just hard code to something for now*/
  cell_info_ptr->cell_id.freq = 10689;
  cell_info_ptr->cell_id.phy_cell_id = 500;
  cell_info_ptr->sib_db_key = 2;
}


/*===========================================================================

  FUNCTION:  lte_rrc_cre_basic_sib2_update_test_get_serv_cell_sib

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API that returns the serving cell SIBs.

    @return
    Pointer to SIB2

 */
/*=========================================================================*/

void *lte_rrc_cre_basic_sib2_update_test_get_serv_cell_sib
(
  lte_rrc_sib_bitmask_t sib, /*!< Sib requested; 1=SIB1, 2=SIB2, ...*/
  lte_rrc_sib_db_key_t key /*!< sib db index */
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  /* Initializing(using) the parameters to handle compiler warnings */
  sib = 0;
  key = 0;
  int decode_status = 0;
  if (bcch_dl_sch_msg_ptr == NULL) 
  {
    encoded_pdu.length = 29;
    encoded_pdu.value = cre_basic_sib2_2;
    bcch_dl_sch_msg_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)    
    lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                                 encoded_pdu.length,
                                 BCCH_DL_SCH_Message,
                                 &decode_status);
  } 

  TF_ASSERT(bcch_dl_sch_msg_ptr != NULL);
  /* return sib 2 ptr */
  return &bcch_dl_sch_msg_ptr->message.u.c1->u.systemInformation->
    criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib2;
} 



/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_free_sib_ptr

===========================================================================*/
/*!
    @brief
    This function frees SIB2 ptr

    @return
    void

 */
/*=========================================================================*/
void lte_rrc_cre_test_free_sib_ptr
(
  void
)
{
  if (bcch_dl_sch_msg_ptr != NULL) 
  {
    lte_rrc_osys_asn1_free_pdu(bcch_dl_sch_msg_ptr, lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
    bcch_dl_sch_msg_ptr = NULL; 
  }
}
/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_get_crnti

===========================================================================*/
/*!
    @brief
    This function returns the crnti

    @return
    lte_rnti_val_t

 */
/*=========================================================================*/
lte_rnti_val_t lte_rrc_cre_test_get_crnti
(
  void
)
{
  /* Just hardcode a value */
  lte_rnti_val_t crnti = 5;
  return crnti;
}

/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_update_crnti_in_cfgdb

===========================================================================*/
/*!
    @brief
    This function simulates updating crnti in LLC Config DB.

    @return
    lte_errno_e

 */
/*=========================================================================*/
lte_errno_e lte_rrc_cre_test_update_crnti_in_cfgdb
(
  lte_rnti_val_t crnti_val
)
{
  crnti_val = 0;
  return E_SUCCESS;
}

/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_get_shortmaci

===========================================================================*/
/*!
    @brief
    This function returns the shortmacI for test purposes.

    @return
    uint16

 */
/*=========================================================================*/
uint16 lte_rrc_cre_test_get_shortmaci
(
  lte_phy_cell_id_t phy_cell_id,
  lte_rnti_val_t crnti
)
{
  /* Just hardcode a value */
  uint16 shortmaci_integer_val = 10;
  return shortmaci_integer_val;
}

/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_is_sec_activated

===========================================================================*/
/*!
    @brief
    This function returns simulates the security module API that returns
    if security is activated.

    @return
    TRUE, to imply security has been activated

 */
/*=========================================================================*/
boolean lte_rrc_cre_test_is_sec_activated
(
  void
)
{
  return TRUE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_get_release_version

===========================================================================*/
/*!
    @brief
    This function determines the release version of LTE RRC for test purposes.

    @return
    lte_rrc_release_e, denoting the LTE RRC release version.

*/
/*=========================================================================*/
lte_rrc_release_e lte_rrc_cre_test_get_release_version
(
  void
)
{
  return LTE_RRC_REL_1_5;
} 

/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_get_release_version_for_2_0

===========================================================================*/
/*!
    @brief
    This function determines the release version of LTE RRC for test purposes.

    @return
    lte_rrc_release_e, denoting the LTE RRC release version.

*/
/*=========================================================================*/
lte_rrc_release_e lte_rrc_cre_test_get_release_version_for_2_0
(
  void
)
{
  return LTE_RRC_REL_2_0;
} 

/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_ho_not_in_progress

===========================================================================*/
/*!
    @brief
    This function simulates the LLC API that returns if HO is in progress.

    @return
    boolean, indicating if HO in progress.

*/
/*=========================================================================*/
boolean lte_rrc_cre_test_ho_not_in_progress
(
  void
)
{
  return FALSE;
} 
/*===========================================================================

  FUNCTION:  lte_rrc_config_test_ho_to_eutra_in_progress

===========================================================================*/
/*!
    @brief
    This function simulates the LLC API that returns if HO to eutra is in progress.

    @return
    boolean, indicating if HO to Eutra in progress.

*/
/*=========================================================================*/
boolean lte_rrc_config_test_ho_to_eutra_in_progress
(
   void
)
{
  return TRUE;
}
/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_ho_in_progress

===========================================================================*/
/*!
    @brief
    This function simulates the LLC API that returns if HO is in progress.

    @return
    boolean, indicating if HO in progress.

*/
/*=========================================================================*/
boolean lte_rrc_cre_test_ho_in_progress
(
  void
)
{
  return TRUE;
} 

/*===========================================================================

  FUNCTION:  lte_rrc_cre_config_get_state_for_meas_reapply

===========================================================================*/
/*!
    @brief
    This function simulates Config API that returns the state of CONFIG SM.

    @return
    boolean

 */
/*=========================================================================*/
stm_state_t lte_rrc_cre_config_get_state_for_meas_reapply
(
  void
)
{
  return LTE_RRC_CONFIG_SM__WAIT_FOR_CFG_CNFI;

} /* End of lte_rrc_cre_config_get_state_for_meas_reapply() */

/*===========================================================================

  FUNCTION:  lte_rrc_cre_config_get_state_for_meas_revert

===========================================================================*/
/*!
    @brief
    This function simulates Config API that returns the state of CONFIG SM.

    @return
    boolean

 */
/*=========================================================================*/
stm_state_t lte_rrc_cre_config_get_state_for_meas_revert
(
  void
)
{
  return LTE_RRC_CONFIG_SM__WAIT_FOR_MEAS_CFG_CNFI;

} /* End of lte_rrc_cre_config_get_state_for_meas_revert() */

/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_config_rb_reconfig_in_progress

===========================================================================*/
/*!
    @brief
    This function simulates Config API that returns if RB reconfig processing is 
    active.

    @return
    boolean

 */
/*=========================================================================*/
boolean lte_rrc_cre_test_config_rb_reconfig_in_progress
(
  void
)
{
  return TRUE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_config_rb_reconfig_not_in_progress

===========================================================================*/
/*!
    @brief
    This function simulates Config API that returns if RB reconfig processing is 
    active.

    @return
    boolean

 */
/*=========================================================================*/
boolean lte_rrc_cre_test_config_rb_reconfig_not_in_progress
(
  void
)
{
  return FALSE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_config_meas_reconfig_in_progress

===========================================================================*/
/*!
    @brief
    This function simulates Config API that returns if meas reconfig processing is 
    active.

    @return
    boolean

 */
/*=========================================================================*/
boolean lte_rrc_cre_test_config_meas_reconfig_in_progress
(
  void
)
{
  return TRUE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_config_meas_reconfig_not_in_progress

===========================================================================*/
/*!
    @brief
    This function simulates Config API that returns if meas reconfig processing is 
    active.

    @return
    boolean

 */
/*=========================================================================*/
boolean lte_rrc_cre_test_config_meas_reconfig_not_in_progress
(
  void
)
{
  return FALSE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_llc_dedicated_rlf_timers_configured

===========================================================================*/
/*!
    @brief
    This function simulates LLC API that populates the dedicated RLF timers
    if configured.

    @return
    boolean

 */
/*=========================================================================*/
boolean lte_rrc_cre_test_llc_dedicated_rlf_timers_configured
(
  lte_rrc_rlf_tmr_s *rlf_tmr_ptr
)
{
  rlf_tmr_ptr->t301 = 200;
  rlf_tmr_ptr->t311 = 2000;

  return TRUE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_llc_dedicated_rlf_timers_not_configured

===========================================================================*/
/*!
    @brief
    This function simulates LLC API that populates the dedicated RLF timers
    if configured.

    @return
    boolean

 */
/*=========================================================================*/
boolean lte_rrc_cre_test_llc_dedicated_rlf_timers_not_configured
(
  lte_rrc_rlf_tmr_s *rlf_tmr_ptr
)
{
  return FALSE;
}

void lte_rrc_cre_test_mh_get_last_encoded_message_rcvd
(
  lte_rrc_pdu_buf_s *pdu_buf_ptr /*! PDU buffer pointer */
)
{
  pdu_buf_ptr->value  = lte_rrc_cre_test_pdu_buf.value;
  pdu_buf_ptr->length = lte_rrc_cre_test_pdu_buf.length;
}

/*===========================================================================

  FUNCTION:  lte_rrc_cre_test_reestablishment

===========================================================================*/
/*!
    @brief
    Reestablishment process after RLF

    @return
    Void

 */
/*=========================================================================*/

void lte_rrc_cre_test_reestablishment
(
  void
)

{
  lte_cphy_rl_failure_ind_s rl_fail_ind;
  lte_rrc_cre_started_indi_s *cre_started_indi_ptr = NULL;
  lte_rrc_sib_abort_reqi_s* sib_abort_reqi_ptr = NULL;
  lte_rrc_cfg_reqi_s* cfg_req_msg_ptr = NULL;
  lte_rrc_cfg_cnfi_s cfg_cnfi_msg;
  lte_rrc_initiate_cell_sel_reqi_s *cel_sel_reqi_ptr = NULL;
  lte_rrc_tmri_s *t311_tmri_ptr = NULL;
  lte_rrc_initiate_conn_rel_indi_s *initiate_conn_rel_indi_ptr = NULL;
  uint32 buf_len = 0; int decode_status = 0;
   lte_rrc_conn_mode_failure_indi_s config_failure_ind;
  lte_rrc_camped_indi_s camped_indi_msg;
  lte_mac_access_req_msg_s *mac_access_req_ptr = NULL;
  lte_mac_access_cnf_msg_s mac_access_cnf;
  lte_rrc_osys_UL_CCCH_Message *ul_ccch_msg_ptr = NULL;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_mh_dlm_s mh_dlm;
  lte_rrc_send_ul_msg_reqi_s *ul_mh_msg_ptr = NULL;
  lte_rrc_cre_completed_indi_s *cre_completed_ind_ptr = NULL;
  lte_rrc_meas_cfg_reqi_s *meas_cfg_req_ptr = NULL;
  lte_rrc_dlm_processed_indi_s *dlm_processed_ind_ptr = NULL;
  uint64 crnti_integer_val = 0;

  /* Catch the Config request sent by CRE to LLC */
  lte_rrc_utf_get_next_int_msg((byte **)&cfg_req_msg_ptr,&buf_len);
  /* Make sure the Config request is valid */
  TF_ASSERT(sizeof(lte_rrc_cfg_reqi_s) == buf_len);
  TF_ASSERT(cfg_req_msg_ptr != NULL);
  TF_ASSERT(cfg_req_msg_ptr->num_cfg == 1);
  TF_ASSERT(cfg_req_msg_ptr->cfg_info[0].cfg_type == LTE_RRC_CFG_RLF);
  TF_ASSERT(cfg_req_msg_ptr->cfg_info[0].cfg_u.susp_cfg.revert_l2_config == FALSE);

  /* Catch the CRE started internal indication thrown by CRE */
  lte_rrc_utf_get_next_int_msg((byte **)&cre_started_indi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cre_started_indi_s) == buf_len);

  /* Assert that CRE is in WAIT_FOR_CFG_CNFI state */
  TF_ASSERT(lte_rrc_cre_get_state() == LTE_RRC_CRE_SM__WAIT_FOR_CFG_CNFI);
  TF_MSG("\nCRE cfg request sent to LLC and CRE Module in WAIT_FOR_CFG_CNFI state\n");

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_cnfi_msg.msg_hdr, LTE_RRC_CFG_CNFI);
  /* Setting the fields in the CFG_CNFI sent by LLC */
  cfg_cnfi_msg.proc = LTE_RRC_PROC_CRE;
  cfg_cnfi_msg.status = E_SUCCESS;
  cfg_cnfi_msg.trans_id = 0;
  /* Send the CFG_CNFI to CRE module there by mimicing the action of LLC */
  lte_rrc_utf_send_msg((byte*)&cfg_cnfi_msg, sizeof(cfg_cnfi_msg));

  /* Catch the Initiate Cell selection message sent to CSP */
  lte_rrc_utf_get_next_int_msg((byte **)&cel_sel_reqi_ptr,&buf_len);
  /* Make sure the message is valid */
  TF_ASSERT(sizeof(lte_rrc_initiate_cell_sel_reqi_s) == buf_len);
  TF_ASSERT(cel_sel_reqi_ptr != NULL);
  
  /* Assert that CRE is in WAIT_FOR_UE_TO_CAMP state */
  TF_ASSERT(lte_rrc_cre_get_state() == LTE_RRC_CRE_SM__WAIT_FOR_UE_TO_CAMP);
  TF_MSG("\nInititate Cell select request sent to CSP and "
         "CRE Module in WAIT_FOR_UE_TO_CAMP state\n");

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&camped_indi_msg.msg_hdr, LTE_RRC_CAMPED_INDI);
  camped_indi_msg.reason = LTE_RRC_CAMPED_CELL_SELECTION_RLF;
  camped_indi_msg.camped_cell.freq = 10689;
  camped_indi_msg.camped_cell.phy_cell_id = 25;

  /* Send the CAMPED_INDI to CRE module there by mimicing the action of CSP */
  lte_rrc_utf_send_msg((byte*)&camped_indi_msg, sizeof(camped_indi_msg));

  TF_STUB(lte_rrc_csp_get_serving_cell_params) = lte_rrc_cre_test_csp_get_serving_cell_params;

  /* Catch the MAC_ACCESS_REQ message sent by CRE to MAC */
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_access_req_ptr,&buf_len);
  /* Make sure the message is valid */
  TF_ASSERT(sizeof(lte_mac_access_req_msg_s) == buf_len);
  TF_ASSERT(mac_access_req_ptr != NULL);
  TF_ASSERT(mac_access_req_ptr->access_info.
            access_reason == LTE_MAC_ACCESS_REASON_RADIO_FAILURE);
  TF_ASSERT(mac_access_req_ptr->access_info.raid == LTE_INVALID_RA_ID);
  TF_ASSERT(mac_access_req_ptr->access_info.raid_mask == LTE_INVALID_RA_ID_MASK);

  /* Decode the sent ReestablishmentRequest within the MAC_ACCESS_REQ */
  ul_ccch_msg_ptr = (lte_rrc_osys_UL_CCCH_Message *)
  lte_rrc_osys_asn1_decode_pdu(mac_access_req_ptr->access_info.msg_ptr, 
                               mac_access_req_ptr->access_info.msg_length,
                               UL_CCCH_Message,
                               &decode_status);
  TF_ASSERT(ul_ccch_msg_ptr != NULL);
  /* Make sure the Connection Reestablishment Request sent is valid */
  TF_ASSERT(ul_ccch_msg_ptr->message.t == T_lte_rrc_osys_UL_CCCH_MessageType_c1);
  TF_ASSERT(ul_ccch_msg_ptr->message.u.c1->t == T_lte_rrc_osys_UL_CCCH_MessageType_c1_rrcConnectionReestablishmentRequest);
  TF_ASSERT(ul_ccch_msg_ptr->message.u.c1->u.rrcConnectionReestablishmentRequest->
    criticalExtensions.t == T_lte_rrc_osys_RRCConnectionReestablishmentRequest_criticalExtensions_rrcConnectionReestablishmentRequest_r8);
  TF_ASSERT(ul_ccch_msg_ptr->message.u.c1->u.rrcConnectionReestablishmentRequest->
            criticalExtensions.u.rrcConnectionReestablishmentRequest_r8->ue_Identity.
            physCellId == 501);
  crnti_integer_val = lte_rrc_osys_asn1_bitstring_to_int(sizeof(uint64),&ul_ccch_msg_ptr->message.
                                                    u.c1->u.rrcConnectionReestablishmentRequest->
                                                    criticalExtensions.u.rrcConnectionReestablishmentRequest_r8->
                                                    ue_Identity.c_RNTI);
  TF_ASSERT(crnti_integer_val == 5);
  TF_ASSERT(ul_ccch_msg_ptr->message.u.c1->u.rrcConnectionReestablishmentRequest->
      criticalExtensions.u.rrcConnectionReestablishmentRequest_r8->
      reestablishmentCause == lte_rrc_osys_ReestablishmentCause_otherFailure);

  /* Assert that CRE is in WAIT_FOR_CONN_RE state */
  TF_ASSERT(lte_rrc_cre_get_state() == LTE_RRC_CRE_SM__WAIT_FOR_CONN_RE);
  TF_MSG("\nReestablishmentRequest sent and CRE Module in WAIT_FOR_CONN_RE state\n");

  /* Send ACCESS_CNF to indicate RACH succeeded */
  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&mac_access_cnf.hdr, LTE_MAC_ACCESS_CNF);
  mac_access_cnf.crnti_included = TRUE;
  mac_access_cnf.crnti = 10;
  /* Send the ACCESS_CNF to CRE module there by mimicing the action of MAC */
  lte_rrc_utf_send_msg((byte*)&mac_access_cnf, sizeof(mac_access_cnf));

  encoded_pdu.length = sizeof(cre_reestablishment_msg);
  encoded_pdu.value = cre_reestablishment_msg;

  lte_rrc_cre_test_pdu_buf.length = encoded_pdu.length;
  lte_rrc_cre_test_pdu_buf.value = encoded_pdu.value;
  
  /* Construct asn1 pdu to be sent to the CRE module after decoding the 
     encoded pdu*/
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr,
                           LTE_RRC_RRC_CONNECTION_REESTABLISHMENT_DLM);
   
  mh_dlm.dl_msg_ptr = 
    lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, DL_CCCH_Message, &decode_status);
  mh_dlm.pdu_num = lte_rrc_osys_DL_CCCH_Message_PDU;
  /* Send the MH DLM */
  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  /* Catch the Config request sent by CRE to LLC */
  lte_rrc_utf_get_next_int_msg((byte **)&cfg_req_msg_ptr,&buf_len);
  /* Make sure the Config request is valid */
  TF_ASSERT(sizeof(lte_rrc_cfg_reqi_s) == buf_len);
  TF_ASSERT(cfg_req_msg_ptr != NULL);
  TF_ASSERT(cfg_req_msg_ptr->cfg_info[0].cfg_type == LTE_RRC_CFG_DED);

  /* Assert that CRE is in WAIT_FOR_CFG_CNFI state */
  TF_ASSERT(lte_rrc_cre_get_state() == LTE_RRC_CRE_SM__WAIT_FOR_CFG_CNFI);
  TF_MSG("\nCRE cfg request sent to LLC and CRE Module in WAIT_FOR_CFG_CNFI state\n");

  /* Setting the header for internal RRC messages */
  lte_rrc_init_default_hdr(&cfg_cnfi_msg.msg_hdr, LTE_RRC_CFG_CNFI);
  /* Setting the fields in the CFG_CNFI sent by LLC */
  cfg_cnfi_msg.proc = LTE_RRC_PROC_CRE;
  cfg_cnfi_msg.status = E_SUCCESS;
  cfg_cnfi_msg.trans_id = 0;
  /* Send the CFG_CNFI to CRE module there by mimicing the action of LLC */
  lte_rrc_utf_send_msg((byte*)&cfg_cnfi_msg, sizeof(cfg_cnfi_msg));

  /* Catch the ReestablishmentComplete sent by CRE to MH */
  lte_rrc_utf_get_next_int_msg((byte **)&ul_mh_msg_ptr,&buf_len);
  /* Make sure the UL MSG is valid */
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  TF_ASSERT(ul_mh_msg_ptr != NULL);
  TF_ASSERT(ul_mh_msg_ptr->ack_needed == FALSE);
  TF_ASSERT(ul_mh_msg_ptr->rb_id == LTE_RRC_DCCH_SRB1);

  /* Verify the contents of ReestablishmentComplete */
  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ul_mh_msg_ptr->ul_msg_ptr;
  TF_ASSERT(ul_dcch_msg_ptr != NULL);
  TF_ASSERT(ul_dcch_msg_ptr->message.t == T_lte_rrc_osys_UL_DCCH_MessageType_c1);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->t == 
                      T_lte_rrc_osys_UL_DCCH_MessageType_c1_rrcConnectionReestablishmentComplete);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.rrcConnectionReestablishmentComplete->
  rrc_TransactionIdentifier == 1);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.rrcConnectionReestablishmentComplete->
    criticalExtensions.t == T_lte_rrc_osys_RRCConnectionReestablishmentComplete_criticalExtensions_rrcConnectionReestablishmentComplete_r8);
  TF_ASSERT(ul_dcch_msg_ptr->message.u.c1->u.rrcConnectionReestablishmentComplete->
    criticalExtensions.u.rrcConnectionReestablishmentComplete_r8->m.nonCriticalExtensionPresent == 0);

  /* Catch the MEAS_CFG_REQI sent by CRE to Meas module */
  lte_rrc_utf_get_next_int_msg((byte **)&meas_cfg_req_ptr,&buf_len);
  /* Make sure the MEAS_CFG_REQI is valid */
  TF_ASSERT(sizeof(lte_rrc_meas_cfg_reqi_s) == buf_len);
  TF_ASSERT(meas_cfg_req_ptr != NULL);
  TF_ASSERT(meas_cfg_req_ptr->cell_id.phy_cell_id == 501);
  TF_ASSERT(meas_cfg_req_ptr->cause == LTE_RRC_MEAS_CFG_REQI_CAUSE_REAPPLY_CONFIG);

  /* Catch the CRE_COMPLETED_INDI sent by CRE to RRC modules */
  lte_rrc_utf_get_next_int_msg((byte **)&cre_completed_ind_ptr,&buf_len);
  /* Make sure the CRE_COMPLETED_INDI is valid */
  TF_ASSERT(sizeof(lte_rrc_cre_completed_indi_s) == buf_len);

  /* Assert that CRE is in INITIAL state */
  TF_ASSERT(lte_rrc_cre_get_state() == LTE_RRC_CRE_SM__INITIAL);

  lte_rrc_osys_asn1_free_pdu(ul_ccch_msg_ptr, lte_rrc_osys_UL_CCCH_Message_PDU);
  lte_rrc_osys_asn1_free_pdu(ul_dcch_msg_ptr, lte_rrc_osys_UL_DCCH_Message_PDU);
  
  lte_rrc_cre_test_free_sib_ptr();


}



/*===========================================================================

   FUNCTION:  lte_rrc_cre_test_stub_init

===========================================================================*/
/*!
    @brief
    Initializes the stub required for CRE unit test 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_cre_test_stub_init
( 
  void 
)
{
  TF_STUB(lte_rrc_controller_get_state) = lte_rrc_cre_test_get_controller_state;
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_cre_basic_sib2_update_test_get_serv_cell_sib;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) = lte_rrc_cre_test_csp_get_serving_cell_params;
  TF_STUB(lte_rrc_llc_get_ho_source_cell_info) = lte_rrc_cre_test_llc_get_source_cell_info;
  TF_STUB(lte_rrc_config_ho_in_progress) = lte_rrc_cre_test_ho_not_in_progress;
  TF_STUB(lte_rrc_llc_get_crnti) = lte_rrc_cre_test_get_crnti;
  TF_STUB(lte_rrc_llc_update_crnti_in_cfgdb) = lte_rrc_cre_test_update_crnti_in_cfgdb;
  TF_STUB(lte_rrc_sec_compute_short_maci) = lte_rrc_cre_test_get_shortmaci;
  TF_STUB(lte_rrc_sec_security_is_configured) = lte_rrc_cre_test_is_sec_activated;
  TF_STUB(lte_rrc_config_get_state) = lte_rrc_cre_config_get_state_for_meas_reapply;
  TF_STUB(lte_rrc_config_rb_reconfig_in_progress) = lte_rrc_cre_test_config_rb_reconfig_not_in_progress;
  TF_STUB(lte_rrc_config_meas_reconfig_in_progress) = lte_rrc_cre_test_config_meas_reconfig_not_in_progress;
  TF_STUB(lte_rrc_llc_get_dedicated_rlf_timers) = lte_rrc_cre_test_llc_dedicated_rlf_timers_not_configured;
  TF_STUB(lte_rrc_mh_get_last_encoded_message_rcvd) = lte_rrc_cre_test_mh_get_last_encoded_message_rcvd;
}/*lte_rrc_cre_test_stub_init*/



