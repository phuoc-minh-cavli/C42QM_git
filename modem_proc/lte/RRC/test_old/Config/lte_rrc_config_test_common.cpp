/*!
  @file
  lte_rrc_config_test_common.cpp

  @brief
  Common Test file used for testing the Config module

  @detail
  This file contains the definition of typical OTA messages received by the 
  Config module in encoded form. It is used by the unit tests for sending
  simulated OTA messages to Config module. The encoded messages were generated
  using the Config Editor tool.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/Config/lte_rrc_config_test_common.cpp#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/09/10   aramani Added stub for lte_rrc_mh_get_last_encoded_message_rcvd
02/09/09   mm      Initial Version
===========================================================================*/



/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte.h"
#include "lte_rrc_config_test_common.h"

#include <tf_stub.h>
#include <TestFramework.h>
#include "lte_rrc_utf.h"
#include "lte_rrc_controller.h"
#include "lte_rrc_csp.h"
#include "lte_rrc_sec.h"
#include "lte_rrc_cre.h"
#include "lte_rrc_meas.h"
#include "lte_rrc_controller.h"
#include "lte_rrc_mh.h"
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_cep.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern "C"
{
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, 
                lte_rrc_cell_info_s*);
  TF_PROTO_STUB(void, lte_rrc_mh_get_last_encoded_message_rcvd, 
                lte_rrc_pdu_buf_s *);
  TF_PROTO_STUB( stm_state_t, lte_rrc_controller_get_state, void);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s *);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_camped_cell_bw, lte_bandwidth_e *);
  TF_PROTO_STUB(boolean, lte_rrc_sec_security_is_configured, void);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_sec_validate_config, lte_rrc_osys_RRCConnectionReconfiguration *);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_meas_validate_connected_config, lte_rrc_osys_RRCConnectionReconfiguration *, boolean);
  TF_PROTO_STUB(stm_state_t, lte_rrc_cre_get_state, void);
  //TF_PROTO_STUB(lte_errno_e, lte_rrc_irat_validate_mobility_from_eutra_msg, lte_rrc_osys_DL_DCCH_Message *);
  TF_PROTO_STUB(uint8, lte_rrc_cap_get_fgi_bit, uint8);
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
  //TF_PROTO_STUB(boolean, lte_rrc_csp_cap_wcdma_is_supported, void);
 // //TF_PROTO_STUB(boolean, lte_rrc_csp_cap_tdscdma_is_supported, void);
  //TF_PROTO_STUB(boolean, lte_rrc_csp_gsm_is_supported, void);
  //TF_PROTO_STUB(boolean, lte_rrc_csp_cdma_1x_is_supported, void);
  TF_PROTO_STUB(lte_rrc_est_cause_e, lte_rrc_cep_get_est_cause, void);
}

/*===========================================================================

                    OTA Message Definitions in encoded form

===========================================================================*/

/*===========================================================================
                     Global config test encoded message
===========================================================================*/

/* This variable will be initialised by individual test cases so as to return 
   the current encoded message by stubbed out function 
   lte_rrc_config_test_mh_get_last_encoded_message_rcvd
*/
lte_rrc_pdu_buf_s lte_rrc_config_test_encoded_pdu;

/*===========================================================================
                     Invalid Message Class Extension Test
===========================================================================*/

/* Reconfig message containing invalid message class extension.
  Encoded version of the following message is assigned to a byte array

Reconfig:value DL-DCCH-Message ::= 
{
  message messageClassExtension : 
    {
    }
}
*/
byte invalid_msg_class_ext[1] = {0x80};

/*===========================================================================
                     Non-Reconfig Message Test
===========================================================================*/

/* DL DCCH message that is not a Reconfiguration message.
  Encoded version of the following message is assigned to a byte array

Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : dlInformationTransfer : 
      {
        rrc-TransactionIdentifier 0,
        criticalExtensions c1 : dlInformationTransfer-r8 : 
            {
              informationType nas3GPP : ''H
            }
      }
}
*/
byte non_reconfig_msg[3] = {0x08, 0x00, 0x00};

/*===========================================================================
                     Invalid Critical Extensions Test
===========================================================================*/

/* Reconfig message containing invalid critical extensions.
  Encoded version of the following message is assigned to a byte array

Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 0,
        criticalExtensions criticalExtensionsFuture : 
          {
          }
      }
}
*/
byte invalid_crit_ext_msg[1] = {0x21};

/*===========================================================================
                     Unsupported Release Test
===========================================================================*/

/* Reconfig message containing unsupported release IEs.
  Encoded version of the following message is assigned to a byte array

Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 0,
        criticalExtensions c1 : spare7 : NULL
      }
}
*/
byte invalid_release_msg[2] = {0x20, 0x20};

/*===========================================================================
                     Invalid Reconfig Security without Mobility Test
===========================================================================*/

/* Reconfig message containing Security Configuration but no Mobility Info 
  present.
  Encoded version of the following message is assigned to a byte array

Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                mac-MainConfig explicitValue : 
                  {
                    timeAlignmentTimerDedicated sf500
                  }
              },
              securityConfigHO 
              {
                handoverType intraLTE : 
                  {
                    keyChangeIndicator FALSE,
                    nextHopChainingCount 0
                  }
              }
            }
      }
}
*/
byte invalid_reconfig_sec_no_mobility_msg[5] = {0x22, 0x03, 0x04, 0x00, 0x00};

/*===========================================================================
                Invalid Reconfig NAS Info without DRB change Test
===========================================================================*/

/* Reconfig message containing NAS dedicated info but no Radio Resource 
   Configuration present.
  Encoded version of the following message is assigned to a byte array

Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              dedicatedInfoNASList 
              {
                '010203'H
              },
              radioResourceConfigDedicated 
              {
                mac-MainConfig explicitValue : 
                  {
                    timeAlignmentTimerDedicated sf500
                  }
              }
            }
      }
}
*/

byte invalid_reconfig_nas_wo_drb_msg[9] = {0x22, 0x06, 0x00, 0x18, 0x08, 0x10, 
  0x18, 0x40, 0x00};

/*===========================================================================
          Config Request Test I - Radio Resource Configuration alone
===========================================================================*/

  /* Reconfig message containing Radio Resource Configuration alone with default
  Transport Channel Configuration.
  Encoded version of the following message is assigned to a byte array

 Prox_msg_with_RRc:value DL-DCCH-Message ::= 
 {
   message c1 : rrcConnectionReconfiguration : 
       {
         rrc-TransactionIdentifier 1,
         criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
             {
               radioResourceConfigDedicated 
               {
                 mac-MainConfig explicitValue : 
                   {
                     timeAlignmentTimerDedicated sf500
                   }
               },
               nonCriticalExtension 
               {
                 nonCriticalExtension 
                 {
                   otherConfig-r9 
                   {
                     reportProximityConfig-r9 
                     {
                       proximityIndicationEUTRA-r9 enabled,
                       proximityIndicationUTRA-r9 enabled
                     }
                   }
                 }
               }
             }
       }
 }
 */ 

byte reconfig_rrc_msg[6] =  {0x22, 0x02, 0x84, 0x00, 0x63, 0x80};

/*===========================================================================
          Config Request Test III - Measurement Configuration alone
===========================================================================*/

  /* Reconfig message containing Measurement Configuration alone.
  Encoded version of the following message assigned to a byte array
 
Meas_cfg_msg:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measIdToAddModList 
                {
                  {
                    measId 1,
                    measObjectId 1,
                    reportConfigId 1
                  }
                },
                preRegistrationInfoHRPD 
                {
                  preRegistrationAllowed FALSE
                }
              },
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  otherConfig-r9 
                  {
                    reportProximityConfig-r9 
                    {
                      proximityIndicationEUTRA-r9 enabled,
                      proximityIndicationUTRA-r9 enabled
                    }
                  }
                }
              }
            }
      }
}
*/

byte reconfig_meas_info_msg[8] =  {0x22, 0x10, 0x81, 0x10, 0x00, 0x00, 0x06, 0x38};

/*
Config_with_prox_only:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  otherConfig-r9 
                  {
                    reportProximityConfig-r9 
                    {
                      proximityIndicationEUTRA-r9 enabled,
                      proximityIndicationUTRA-r9 enabled
                    }
                  }
                }
              }
            }
      }
}

*/

byte reconfig_prox_indi_only_msg[4] =  {0x22, 0x00, 0xB1, 0xC0};


/*
Meas_cfg_msg:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measIdToAddModList 
                {
                  {
                    measId 1,
                    measObjectId 1,
                    reportConfigId 1
                  }
                },
                preRegistrationInfoHRPD 
                {
                  preRegistrationAllowed FALSE
                }
              },
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  otherConfig-r9 
                  {
                    reportProximityConfig-r9 
                    {
                    }
                  }
                }
              }
            }
      }
}
*/

byte reconfig_meas_info_with_prox_cfg_msg[8] = {0x22, 0x10, 0x81, 0x10, 0x00, 0x00, 0x06, 0x20};

/*
Meas_cfg_msg:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measIdToAddModList 
                {
                  {
                    measId 1,
                    measObjectId 1,
                    reportConfigId 1
                  }
                },
                preRegistrationInfoHRPD 
                {
                  preRegistrationAllowed FALSE
                }
              },
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  otherConfig-r9 
                  {
                    reportProximityConfig-r9 
                    {
                      proximityIndicationEUTRA-r9 enabled
                    }
                  }
                }
              }
            }
      }
}
*/

byte reconfig_meas_info_with_prox_cfg_msg1[8] = {0x22, 0x10, 0x81, 0x10, 0x00, 0x00, 0x06, 0x30};

/*
Meas_cfg_msg:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measIdToAddModList 
                {
                  {
                    measId 1,
                    measObjectId 1,
                    reportConfigId 1
                  }
                },
                preRegistrationInfoHRPD 
                {
                  preRegistrationAllowed FALSE
                }
              },
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  otherConfig-r9 
                  {
                    reportProximityConfig-r9 
                    {
                      proximityIndicationUTRA-r9 enabled
                    }
                  }
                }
              }
            }
      }
}
*/

byte reconfig_meas_info_with_prox_cfg_msg2[8] = {0x22, 0x10, 0x81, 0x10, 0x00, 0x00, 0x06, 0x28};

/*===========================================================================
            Radio resource config, Meas & NAS Dedicated Info
===========================================================================*/
/*
Prox_config_msg:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measIdToAddModList 
                {
                  {
                    measId 1,
                    measObjectId 1,
                    reportConfigId 1
                  }
                }
              },
              dedicatedInfoNASList 
              {
                '010203'H,
                '040506'H,
                '070809'H
              },
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
                            t-PollRetransmit ms5,
                            pollPDU p4,
                            pollByte kB25,
                            maxRetxThreshold t1
                          },
                          dl-AM-RLC 
                          {
                            t-Reordering ms0,
                            t-StatusProhibit ms0
                          }
                        }
                  },
                  {
                    srb-Identity 2,
                    logicalChannelConfig explicitValue : 
                      {
                        ul-SpecificParameters 
                        {
                          priority 1,
                          prioritisedBitRate infinity,
                          bucketSizeDuration ms50
                        }
                      }
                  }
                },
                drb-ToAddModList 
                {
                  {
                    drb-Identity 3,
                    rlc-Config am : 
                      {
                        ul-AM-RLC 
                        {
                          t-PollRetransmit ms25,
                          pollPDU p32,
                          pollByte kB100,
                          maxRetxThreshold t1
                        },
                        dl-AM-RLC 
                        {
                          t-Reordering ms0,
                          t-StatusProhibit ms0
                        }
                      }
                  }
                }
              },
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  otherConfig-r9 
                  {
                    reportProximityConfig-r9 
                    {
                      proximityIndicationEUTRA-r9 enabled,
                      proximityIndicationUTRA-r9 enabled
                    }
                  }
                }
              }
            }
      }
}
*/

byte reconfig_meas_nas_info_msg[34] = {0x22, 0x16, 0x81, 0x00, 0x00, 0x00, 0x10, 
  0x18, 0x08, 0x10, 0x18, 0x18, 0x20, 0x28, 0x30, 0x18, 0x38, 0x40, 0x4B, 0x0A, 
  0x00, 0x00, 0x00, 0x00, 0x32, 0x07, 0x00, 0x20, 0x80, 0x8C, 0xC0, 0x00, 0x63, 0x80};

/*===========================================================================
       Radio resource config, NAS Dedicated Info with empty first element
===========================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              dedicatedInfoNASList 
              {
                ''H,
                '040506'H,
                '070809'H
              },
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
                            t-PollRetransmit ms5,
                            pollPDU p4,
                            pollByte kB25,
                            maxRetxThreshold t1
                          },
                          dl-AM-RLC 
                          {
                            t-Reordering ms0,
                            t-StatusProhibit ms0
                          }
                        }
                  },
                  {
                    srb-Identity 2,
                    logicalChannelConfig explicitValue : 
                      {
                        ul-SpecificParameters 
                        {
                          priority 1,
                          prioritisedBitRate infinity,
                          bucketSizeDuration ms50
                        }
                      }
                  }
                },
                drb-ToAddModList 
                {
                  {
                    drb-Identity 3,
                    rlc-Config am : 
                      {
                        ul-AM-RLC 
                        {
                          t-PollRetransmit ms25,
                          pollPDU p32,
                          pollByte kB100,
                          maxRetxThreshold t1
                        },
                        dl-AM-RLC 
                        {
                          t-Reordering ms0,
                          t-StatusProhibit ms0
                        }
                      }
                  }
                }
              }
            }
      }
}
*/
byte reconfig_meas_nas_info_msg_zero_length[24]= {
  0x22, 0x06, 0x10, 0x00, 0x18, 0x08, 0x00, 0x00, 0x18, 0x10, 0x00, 0x01, 0x80, 
  0x7D, 0x06, 0xA1, 0x40, 0x00, 0x00, 0x10, 0x5D, 0x09, 0x00, 0x10, 
};

/*===========================================================================
                    Reconfig Message - DRB3 Release 

============================================================================*/
/*
Prox_cfg_msg:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                drb-ToReleaseList 
                {
                  3
                }
              },
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  otherConfig-r9 
                  {
                    reportProximityConfig-r9 
                    {
                      proximityIndicationEUTRA-r9 enabled,
                      proximityIndicationUTRA-r9 enabled
                    }
                  }
                }
              }
            }
      }
}
*/

byte reconfig_with_drb_rel_msg[6] = {0x22, 0x02, 0x88, 0x01, 0x31, 0xC0};

/*===========================================================================
                    Reconfig Message - HO Only 

============================================================================*/
/*
Prox_msg_with_Ho_only:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              mobilityControlInfo 
              {
                targetPhysCellId 10,
                carrierFreq 
                {
                  dl-CarrierFreq 36000,
                  ul-CarrierFreq 36199
                },
                carrierBandwidth 
                {
                  dl-Bandwidth n25,
                  ul-Bandwidth n25
                },
                additionalSpectrumEmission 10,
                t304 ms150,
                newUE-Identity '00000001 00000000'B,
                radioResourceConfigCommon 
                {
                  rach-ConfigCommon 
                  {
                    preambleInfo 
                    {
                      numberOfRA-Preambles n36,
                      preamblesGroupAConfig 
                      {
                        sizeOfRA-PreamblesGroupA n24,
                        messageSizeGroupA b56,
                        messagePowerOffsetGroupB minusinfinity
                      }
                    },
                    powerRampingParameters 
                    {
                      powerRampingStep dB2,
                      preambleInitialReceivedTargetPower dBm-120
                    },
                    ra-SupervisionInfo 
                    {
                      preambleTransMax n5,
                      ra-ResponseWindowSize sf4,
                      mac-ContentionResolutionTimer sf32
                    },
                    maxHARQ-Msg3Tx 3
                  },
                  prach-Config 
                  {
                    rootSequenceIndex 5,
                    prach-ConfigInfo 
                    {
                      prach-ConfigIndex 1,
                      highSpeedFlag TRUE,
                      zeroCorrelationZoneConfig 1,
                      prach-FreqOffset 0
                    }
                  },
                  pdsch-ConfigCommon 
                  {
                    referenceSignalPower 0,
                    p-b 2
                  },
                  pusch-ConfigCommon 
                  {
                    pusch-ConfigBasic 
                    {
                      n-SB 1,
                      hoppingMode intraAndInterSubFrame,
                      pusch-HoppingOffset 0,
                      enable64QAM FALSE
                    },
                    ul-ReferenceSignalsPUSCH 
                    {
                      groupHoppingEnabled TRUE,
                      groupAssignmentPUSCH 2,
                      sequenceHoppingEnabled TRUE,
                      cyclicShift 0
                    }
                  },
                  phich-Config 
                  {
                    phich-Duration extended,
                    phich-Resource one
                  },
                  pucch-ConfigCommon 
                  {
                    deltaPUCCH-Shift ds3,
                    nRB-CQI 2,
                    nCS-AN 6,
                    n1PUCCH-AN 2
                  },
                  soundingRS-UL-ConfigCommon setup : 
                    {
                      srs-BandwidthConfig bw2,
                      srs-SubframeConfig sc2,
                      ackNackSRS-SimultaneousTransmission TRUE
                    },
                  uplinkPowerControlCommon 
                  {
                    p0-NominalPUSCH 5,
                    alpha al05,
                    p0-NominalPUCCH -100,
                    deltaFList-PUCCH 
                    {
                      deltaF-PUCCH-Format1 deltaF-2,
                      deltaF-PUCCH-Format1b deltaF1,
                      deltaF-PUCCH-Format2 deltaF-2,
                      deltaF-PUCCH-Format2a deltaF-2,
                      deltaF-PUCCH-Format2b deltaF-2
                    },
                    deltaPreambleMsg3 3
                  },
                  antennaInfoCommon 
                  {
                    antennaPortsCount an1
                  },
                  p-Max 10,
                  tdd-Config 
                  {
                    subframeAssignment sa2,
                    specialSubframePatterns ssp0
                  },
                  ul-CyclicPrefixLength len1
                },
                rach-ConfigDedicated 
                {
                  ra-PreambleIndex 1,
                  ra-PRACH-MaskIndex 1
                }
              },
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL
                  }
                }
              },
              securityConfigHO 
              {
                handoverType intraLTE : 
                  {
                    securityAlgorithmConfig 
                    {
                      cipheringAlgorithm eea0,
                      integrityProtAlgorithm eia1
                    },
                    keyChangeIndicator FALSE,
                    nextHopChainingCount 0
                  }
              },
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  otherConfig-r9 
                  {
                    reportProximityConfig-r9 
                    {
                      proximityIndicationEUTRA-r9 enabled,
                      proximityIndicationUTRA-r9 enabled
                    }
                  }
                }
              }
            }
      }
}
*/

byte reconfig_with_ho_only_msg[42] = {0x22, 0x0B, 0xBC, 0x15, 0x8C, 0xA0, 0x8D, 0x67, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xEC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14,
0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x10, 0x01, 0x14, 0x04, 0x90, 0x10, 0x63, 0x80,};

/*===========================================================================
                Invalid Reconfig Message - HO with NAS Dedicated Info

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              mobilityControlInfo 
              {
                targetPhysCellId 10,
                carrierFreq 
                {
                  dl-CarrierFreq 36000,
                  ul-CarrierFreq 36199
                },
                carrierBandwidth 
                {
                  dl-Bandwidth n25,
                  ul-Bandwidth n6
                },
                additionalSpectrumEmission 10,
                t304 ms150,
                newUE-Identity '00000000 00000000'B,
                radioResourceConfigCommon 
                {
                  rach-Config 
                  {
                    preambleInfo 
                    {
                      numberOfRA-Preambles n36,
                      preamblesGroupAConfig 
                      {
                        sizeOfRA-PreamblesGroupA n24,
                        messageSizeGroupA b56,
                        messagePowerOffsetGroupB minusinfinity
                      }
                    },
                    powerRampingParameters 
                    {
                      powerRampingStep dB2,
                      preambleInitialReceivedTargetPower dBm-120
                    },
                    ra-SupervisionInfo 
                    {
                      preambleTransMax n5,
                      ra-ResponseWindowSize sf4,
                      mac-ContentionResolutionTimer sf32
                    },
                    maxHARQ-Msg3Tx 3
                  },
                  prach-Config 
                  {
                    rootSequenceIndex 5,
                    prach-ConfigInfo 
                    {
                      prach-ConfigIndex 1,
                      highSpeedFlag TRUE,
                      zeroCorrelationZoneConfig 1,
                      prach-FreqOffset 0
                    }
                  },
                  pdsch-ConfigCommon 
                  {
                    referenceSignalPower 0,
                    p-b 2
                  },
                  pusch-ConfigCommon 
                  {
                    pusch-ConfigBasic 
                    {
                      n-SB 1,
                      hoppingMode intraAndInterSubFrame,
                      pusch-HoppingOffset 0,
                      enable64QAM FALSE
                    },
                    ul-ReferenceSignalsPUSCH 
                    {
                      groupHoppingEnabled TRUE,
                      groupAssignmentPUSCH 2,
                      sequenceHoppingEnabled TRUE,
                      cyclicShift 0
                    }
                  },
                  phich-Config 
                  {
                    phich-Duration extended,
                    phich-Resource one
                  },
                  pucch-ConfigCommon 
                  {
                    deltaPUCCH-Shift ds3,
                    nRB-CQI 2,
                    nCS-AN 6,
                    n1PUCCH-AN 2
                  },
                  soundingRS-UL-ConfigCommon setup : 
                    {
                      srs-BandwidthConfig bw2,
                      srs-SubframeConfig sc2,
                      ackNackSRS-SimultaneousTransmission TRUE
                    },
                  uplinkPowerControlCommon 
                  {
                    p0-NominalPUSCH 5,
                    alpha al05,
                    p0-NominalPUCCH -100,
                    deltaFList-PUCCH 
                    {
                      deltaF-PUCCH-Format1 deltaF-2,
                      deltaF-PUCCH-Format1b deltaF1,
                      deltaF-PUCCH-Format2 deltaF-2,
                      deltaF-PUCCH-Format2a deltaF-2,
                      deltaF-PUCCH-Format2b deltaF-2
                    },
                    deltaPreambleMsg3 3
                  },
                  antennaInfoCommon 
                  {
                    antennaPortsCount an1
                  },
                  p-Max 10,
                  tdd-Config 
                  {
                    subframeAssignment sa2,
                    specialSubframePatterns ssp0
                  },
                  ul-CyclicPrefixLength len1
                },
                rach-ConfigDedicated 
                {
                  ra-PreambleIndex 1,
                  ra-PRACH-MaskIndex 1
                }
              },
              dedicatedInfoNASList 
              {
                '01'H
              },
              securityConfigHO 
              {
                handoverType intraLTE : 
                  {
                    securityAlgorithmConfig 
                    {
                      cipheringAlgorithm eea0,
                      integrityProtAlgorithm eia1
                    },
                    keyChangeIndicator FALSE,
                    nextHopChainingCount 0
                  }
              }
            }
      }
}
*/

byte invalid_mobility_info_with_nas_ded_msg[41] = {0x22, 0x0D, 0x3C, 0x15, 0x8C, 0xA0, 0x8D, 0x67, 0x91, 0x25, 0x00, 0x00, 0x3F, 0xEC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14,
0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x10, 0x01, 0x10, 0x01, 0x01, 0x20, 0x20,};

/*===========================================================================
                    Reconfig Message - HO with Full Config 

============================================================================*/

/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              mobilityControlInfo 
              {
                targetPhysCellId 10,
                carrierFreq 
                {
                  dl-CarrierFreq 36000,
                  ul-CarrierFreq 36199
                },
                carrierBandwidth 
                {
                  dl-Bandwidth n25,
                  ul-Bandwidth n6
                },
                additionalSpectrumEmission 10,
                t304 ms150,
                newUE-Identity '00000001 00000000'B,
                radioResourceConfigCommon 
                {
                  rach-Config 
                  {
                    preambleInfo 
                    {
                      numberOfRA-Preambles n36,
                      preamblesGroupAConfig 
                      {
                        sizeOfRA-PreamblesGroupA n24,
                        messageSizeGroupA b56,
                        messagePowerOffsetGroupB minusinfinity
                      }
                    },
                    powerRampingParameters 
                    {
                      powerRampingStep dB2,
                      preambleInitialReceivedTargetPower dBm-120
                    },
                    ra-SupervisionInfo 
                    {
                      preambleTransMax n5,
                      ra-ResponseWindowSize sf4,
                      mac-ContentionResolutionTimer sf32
                    },
                    maxHARQ-Msg3Tx 3
                  },
                  prach-Config 
                  {
                    rootSequenceIndex 5,
                    prach-ConfigInfo 
                    {
                      prach-ConfigIndex 1,
                      highSpeedFlag TRUE,
                      zeroCorrelationZoneConfig 1,
                      prach-FreqOffset 0
                    }
                  },
                  pdsch-ConfigCommon 
                  {
                    referenceSignalPower 0,
                    p-b 2
                  },
                  pusch-ConfigCommon 
                  {
                    pusch-ConfigBasic 
                    {
                      n-SB 1,
                      hoppingMode intraAndInterSubFrame,
                      pusch-HoppingOffset 0,
                      enable64QAM FALSE
                    },
                    ul-ReferenceSignalsPUSCH 
                    {
                      groupHoppingEnabled TRUE,
                      groupAssignmentPUSCH 2,
                      sequenceHoppingEnabled TRUE,
                      cyclicShift 0
                    }
                  },
                  phich-Config 
                  {
                    phich-Duration extended,
                    phich-Resource one
                  },
                  pucch-ConfigCommon 
                  {
                    deltaPUCCH-Shift ds3,
                    nRB-CQI 2,
                    nCS-AN 6,
                    n1PUCCH-AN 2
                  },
                  soundingRS-UL-ConfigCommon setup : 
                    {
                      srs-BandwidthConfig bw2,
                      srs-SubframeConfig sc2,
                      ackNackSRS-SimultaneousTransmission TRUE
                    },
                  uplinkPowerControlCommon 
                  {
                    p0-NominalPUSCH 5,
                    alpha al05,
                    p0-NominalPUCCH -100,
                    deltaFList-PUCCH 
                    {
                      deltaF-PUCCH-Format1 deltaF-2,
                      deltaF-PUCCH-Format1b deltaF1,
                      deltaF-PUCCH-Format2 deltaF-2,
                      deltaF-PUCCH-Format2a deltaF-2,
                      deltaF-PUCCH-Format2b deltaF-2
                    },
                    deltaPreambleMsg3 3
                  },
                  antennaInfoCommon 
                  {
                    antennaPortsCount an1
                  },
                  p-Max 10,
                  tdd-Config 
                  {
                    subframeAssignment sa2,
                    specialSubframePatterns ssp0
                  },
                  ul-CyclicPrefixLength len1
                },
                rach-ConfigDedicated 
                {
                  ra-PreambleIndex 1,
                  ra-PRACH-MaskIndex 1
                }
              },
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 5,
                    drb-Identity 1,
                    pdcp-Config 
                    {
                      discardTimer ms100,
                      rlc-UM 
                      {
                        pdcp-SN-Size len12bits
                      },
                      headerCompression rohc : 
                        {
                          maxCID 1,
                          profiles 
                          {
                            profile0x0001 FALSE,
                            profile0x0002 FALSE,
                            profile0x0003 FALSE,
                            profile0x0004 FALSE,
                            profile0x0006 FALSE,
                            profile0x0101 FALSE,
                            profile0x0102 FALSE,
                            profile0x0103 FALSE,
                            profile0x0104 FALSE
                          }
                        }
                    },
                    rlc-Config um-Bi-Directional : 
                      {
                        ul-UM-RLC 
                        {
                          sn-FieldLength size10
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size10,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 3,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 3,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                },
                drb-ToReleaseList 
                {
                  3
                },
                mac-MainConfig explicitValue : 
                  {
                    ul-SCH-Config 
                    {
                      maxHARQ-Tx n5,
                      periodicBSR-Timer sf16,
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    drx-Config release : NULL,
                    timeAlignmentTimerDedicated sf5120,
                    phr-Config setup : 
                      {
                        periodicPHR-Timer sf10,
                        prohibitPHR-Timer sf0,
                        dl-PathlossChange dB1
                      }
                  },
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL setup : 
                    {
                      semiPersistSchedIntervalDL sf10,
                      numberOfConfSPS-Processes 2,
                      n1-PUCCH-AN-PersistentList 
                      {
                        4
                      }
                    },
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf10,
                      implicitReleaseAfter e2,
                      p0-Persistent 
                      {
                        p0-NominalPUSCH-Persistent 5,
                        p0-UE-PUSCH-Persistent 5
                      },
                      twoIntervalsConfig true
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB-3
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition setup : 
                      {
                        repetitionFactor n2,
                        n1PUCCH-AN-Rep 0
                      },
                    tdd-AckNackFeedbackMode bundling
                  },
                  pusch-ConfigDedicated 
                  {
                    betaOffset-ACK-Index 0,
                    betaOffset-RI-Index 0,
                    betaOffset-CQI-Index 0
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 5,
                    deltaMCS-Enabled en1,
                    accumulationEnabled FALSE,
                    p0-UE-PUCCH 0,
                    pSRS-Offset 0
                  },
                  tpc-PDCCH-ConfigPUCCH setup : 
                    {
                      tpc-RNTI '00000001 00000000'B,
                      tpc-Index indexOfFormat3 : 1
                    },
                  tpc-PDCCH-ConfigPUSCH setup : 
                    {
                      tpc-RNTI '00000001 00000000'B,
                      tpc-Index indexOfFormat3 : 1
                    },
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm20,
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic setup : 
                      {
                        cqi-PUCCH-ResourceIndex 0,
                        cqi-pmi-ConfigIndex 0,
                        cqi-FormatIndicatorPeriodic subbandCQI : 
                          {
                            k 1
                          },
                        ri-ConfigIndex 0,
                        simultaneousAckNackAndCQI FALSE
                      }
                  },
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw2,
                      srs-HoppingBandwidth hbw1,
                      freqDomainPosition 0,
                      duration FALSE,
                      srs-ConfigIndex 0,
                      transmissionComb 0,
                      cyclicShift cs3
                    },
                  antennaInfo defaultValue : NULL,
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    }
                }
              },
              securityConfigHO 
              {
                handoverType intraLTE : 
                  {
                    securityAlgorithmConfig 
                    {
                      cipheringAlgorithm eea0,
                      integrityProtAlgorithm eia1
                    },
                    keyChangeIndicator FALSE,
                    nextHopChainingCount 0
                  }
              },
              nonCriticalExtension 
              {
              }
            }
      }
}
*/


byte reconfig_with_ho_full_config_msg[88] = {0x22, 0x0B, 0xBC, 0x15, 0x8C, 0xA0, 0x8D, 0x67, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xEC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14,
0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x10, 0x01, 0x17, 0xE4, 0x83, 0xEA, 0x05, 0x3A, 0x00, 0x00, 0x00, 0xE2, 0x08, 0x88, 0x00, 0x8F,
0xA1, 0x0A, 0xC0, 0x38, 0x08, 0x04, 0x04, 0x00, 0x96, 0x04, 0x1E, 0xBF, 0xF5, 0x80, 0x00, 0x00, 0x0D, 0xA0, 0x20, 0x20, 0x00, 0x80, 0x80, 0x03, 0x27,
0x00, 0x00, 0x04, 0x00, 0x19, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x02, 0x02, 0x00,};


/*===========================================================================
                    Reconfig Message - HO with Meas Config 

============================================================================*/

/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measIdToAddModList 
                {
                  {
                    measId 1,
                    measObjectId 1,
                    reportConfigId 1
                  }
                },
                preRegistrationInfoHRPD 
                {
                  preRegistrationAllowed TRUE
                }
              },
              mobilityControlInfo 
              {
                targetPhysCellId 10,
                carrierFreq 
                {
                  dl-CarrierFreq 36000,
                  ul-CarrierFreq 36199
                },
                carrierBandwidth 
                {
                  dl-Bandwidth n25,
                  ul-Bandwidth n6
                },
                additionalSpectrumEmission 10,
                t304 ms150,
                newUE-Identity '00000001 00000000'B,
                radioResourceConfigCommon 
                {
                  rach-ConfigCommon 
                  {
                    preambleInfo 
                    {
                      numberOfRA-Preambles n36,
                      preamblesGroupAConfig 
                      {
                        sizeOfRA-PreamblesGroupA n24,
                        messageSizeGroupA b56,
                        messagePowerOffsetGroupB minusinfinity
                      }
                    },
                    powerRampingParameters 
                    {
                      powerRampingStep dB2,
                      preambleInitialReceivedTargetPower dBm-120
                    },
                    ra-SupervisionInfo 
                    {
                      preambleTransMax n5,
                      ra-ResponseWindowSize sf4,
                      mac-ContentionResolutionTimer sf32
                    },
                    maxHARQ-Msg3Tx 3
                  },
                  prach-Config 
                  {
                    rootSequenceIndex 5,
                    prach-ConfigInfo 
                    {
                      prach-ConfigIndex 1,
                      highSpeedFlag TRUE,
                      zeroCorrelationZoneConfig 1,
                      prach-FreqOffset 0
                    }
                  },
                  pdsch-ConfigCommon 
                  {
                    referenceSignalPower 0,
                    p-b 2
                  },
                  pusch-ConfigCommon 
                  {
                    pusch-ConfigBasic 
                    {
                      n-SB 1,
                      hoppingMode intraAndInterSubFrame,
                      pusch-HoppingOffset 0,
                      enable64QAM FALSE
                    },
                    ul-ReferenceSignalsPUSCH 
                    {
                      groupHoppingEnabled TRUE,
                      groupAssignmentPUSCH 2,
                      sequenceHoppingEnabled TRUE,
                      cyclicShift 0
                    }
                  },
                  phich-Config 
                  {
                    phich-Duration extended,
                    phich-Resource one
                  },
                  pucch-ConfigCommon 
                  {
                    deltaPUCCH-Shift ds3,
                    nRB-CQI 2,
                    nCS-AN 6,
                    n1PUCCH-AN 2
                  },
                  soundingRS-UL-ConfigCommon setup : 
                    {
                      srs-BandwidthConfig bw2,
                      srs-SubframeConfig sc2,
                      ackNackSRS-SimultaneousTransmission TRUE
                    },
                  uplinkPowerControlCommon 
                  {
                    p0-NominalPUSCH 5,
                    alpha al05,
                    p0-NominalPUCCH -100,
                    deltaFList-PUCCH 
                    {
                      deltaF-PUCCH-Format1 deltaF-2,
                      deltaF-PUCCH-Format1b deltaF1,
                      deltaF-PUCCH-Format2 deltaF-2,
                      deltaF-PUCCH-Format2a deltaF-2,
                      deltaF-PUCCH-Format2b deltaF-2
                    },
                    deltaPreambleMsg3 3
                  },
                  antennaInfoCommon 
                  {
                    antennaPortsCount an1
                  },
                  p-Max 10,
                  tdd-Config 
                  {
                    subframeAssignment sa2,
                    specialSubframePatterns ssp0
                  },
                  ul-CyclicPrefixLength len1
                },
                rach-ConfigDedicated 
                {
                  ra-PreambleIndex 1,
                  ra-PRACH-MaskIndex 1
                }
              },
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL
                  }
                }
              },
              securityConfigHO 
              {
                handoverType intraLTE : 
                  {
                    securityAlgorithmConfig 
                    {
                      cipheringAlgorithm eea0,
                      integrityProtAlgorithm eia1
                    },
                    keyChangeIndicator FALSE,
                    nextHopChainingCount 0
                  }
              }
            }
      }
}
*/


byte reconfig_with_ho_with_meas_msg[45] = {0x22, 0x1B, 0x01, 0x10, 0x00, 0x00, 0x17, 0x82, 0xB1, 0x94, 0x11, 0xAC, 0xF2, 0x24, 0xA0, 0x10, 0x07, 0xFD, 0x82, 0x81, 0x02, 0x4D, 0x40, 0x50, 0x62,
0x01, 0xE4, 0x40, 0x22, 0x8D, 0x02, 0xC0, 0x0A, 0x45, 0x83, 0x5B, 0x00, 0x21, 0x42, 0x00, 0x22, 0x80, 0x92, 0x02, 0x00,};

/*===========================================================================
                    Reconfig Message - HO with Delta Config 

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              mobilityControlInfo 
              {
                targetPhysCellId 10,
                t304 ms150,
                newUE-Identity '00000001 00000000'B,
                radioResourceConfigCommon 
                {
                  prach-Config 
                  {
                    rootSequenceIndex 5,
                    prach-ConfigInfo 
                    {
                      prach-ConfigIndex 1,
                      highSpeedFlag TRUE,
                      zeroCorrelationZoneConfig 1,
                      prach-FreqOffset 0
                    }
                  },
                  pusch-ConfigCommon 
                  {
                    pusch-ConfigBasic 
                    {
                      n-SB 1,
                      hoppingMode intraAndInterSubFrame,
                      pusch-HoppingOffset 0,
                      enable64QAM FALSE
                    },
                    ul-ReferenceSignalsPUSCH 
                    {
                      groupHoppingEnabled TRUE,
                      groupAssignmentPUSCH 2,
                      sequenceHoppingEnabled TRUE,
                      cyclicShift 0
                    }
                  },
                  ul-CyclicPrefixLength len1
                }
              },
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL
                  }
                },
                drb-ToReleaseList 
                {
                  3
                }
              },
              securityConfigHO 
              {
                handoverType intraLTE : 
                  {
                    keyChangeIndicator FALSE,
                    nextHopChainingCount 0
                  }
              }
            }
      }
}
*/

byte reconfig_with_ho_delta_config_msg[18] = {0x22, 0x0B, 0x00, 0x14, 0x80, 0x40, 
  0x00, 0x08, 0x0A, 0x0C, 0x40, 0x10, 0x08, 0xA0, 0xA0, 0x90, 0x10, 0x00};

/*===========================================================================
                    Reconfig Message - Invalid T304 value 

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              mobilityControlInfo 
              {
                targetPhysCellId 10,
                carrierFreq 
                {
                  dl-CarrierFreq 36000,
                  ul-CarrierFreq 36199
                },
                carrierBandwidth 
                {
                  dl-Bandwidth n25,
                  ul-Bandwidth n6
                },
                additionalSpectrumEmission 10,
                t304 spare1,
                newUE-Identity '00000000 00000000'B,
                radioResourceConfigCommon 
                {
                  rach-Config 
                  {
                    preambleInfo 
                    {
                      numberOfRA-Preambles n36,
                      preamblesGroupAConfig 
                      {
                        sizeOfRA-PreamblesGroupA n24,
                        messageSizeGroupA b56,
                        messagePowerOffsetGroupB minusinfinity
                      }
                    },
                    powerRampingParameters 
                    {
                      powerRampingStep dB2,
                      preambleInitialReceivedTargetPower dBm-120
                    },
                    ra-SupervisionInfo 
                    {
                      preambleTransMax n5,
                      ra-ResponseWindowSize sf4,
                      mac-ContentionResolutionTimer sf32
                    },
                    maxHARQ-Msg3Tx 3
                  },
                  prach-Config 
                  {
                    rootSequenceIndex 5,
                    prach-ConfigInfo 
                    {
                      prach-ConfigIndex 1,
                      highSpeedFlag TRUE,
                      zeroCorrelationZoneConfig 1,
                      prach-FreqOffset 0
                    }
                  },
                  pdsch-ConfigCommon 
                  {
                    referenceSignalPower 0,
                    p-b 2
                  },
                  pusch-ConfigCommon 
                  {
                    pusch-ConfigBasic 
                    {
                      n-SB 1,
                      hoppingMode intraAndInterSubFrame,
                      pusch-HoppingOffset 0,
                      enable64QAM FALSE
                    },
                    ul-ReferenceSignalsPUSCH 
                    {
                      groupHoppingEnabled TRUE,
                      groupAssignmentPUSCH 2,
                      sequenceHoppingEnabled TRUE,
                      cyclicShift 0
                    }
                  },
                  phich-Config 
                  {
                    phich-Duration extended,
                    phich-Resource one
                  },
                  pucch-ConfigCommon 
                  {
                    deltaPUCCH-Shift ds3,
                    nRB-CQI 2,
                    nCS-AN 6,
                    n1PUCCH-AN 2
                  },
                  soundingRS-UL-ConfigCommon setup : 
                    {
                      srs-BandwidthConfig bw2,
                      srs-SubframeConfig sc2,
                      ackNackSRS-SimultaneousTransmission TRUE
                    },
                  uplinkPowerControlCommon 
                  {
                    p0-NominalPUSCH 5,
                    alpha al05,
                    p0-NominalPUCCH -100,
                    deltaFList-PUCCH 
                    {
                      deltaF-PUCCH-Format1 deltaF-2,
                      deltaF-PUCCH-Format1b deltaF1,
                      deltaF-PUCCH-Format2 deltaF-2,
                      deltaF-PUCCH-Format2a deltaF-2,
                      deltaF-PUCCH-Format2b deltaF-2
                    },
                    deltaPreambleMsg3 3
                  },
                  antennaInfoCommon 
                  {
                    antennaPortsCount an1
                  },
                  p-Max 10,
                  tdd-Config 
                  {
                    subframeAssignment sa2,
                    specialSubframePatterns ssp0
                  },
                  ul-CyclicPrefixLength len1
                },
                rach-ConfigDedicated 
                {
                  ra-PreambleIndex 1,
                  ra-PRACH-MaskIndex 1
                }
              },
              securityConfigHO 
              {
                handoverType intraLTE : 
                  {
                    securityAlgorithmConfig 
                    {
                      cipheringAlgorithm eea0,
                      integrityProtAlgorithm eia1
                    },
                    keyChangeIndicator FALSE,
                    nextHopChainingCount 0
                  }
              }
            }
      }
}
*/

byte reconfig_msg_invalid_t304[39] = {0x22, 0x09, 0x3C, 0x15, 0x8C, 0xA0, 0x8D, 0x67, 0x91, 0x27, 0x80, 0x00, 0x3F, 0xEC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14,
0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x10, 0x01, 0x12, 0x02, 0x00,};

/*===========================================================================
                    Reconfig Message - HO without security 

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              mobilityControlInfo 
              {
                targetPhysCellId 10,
                carrierFreq 
                {
                  dl-CarrierFreq 36000,
                  ul-CarrierFreq 36199
                },
                carrierBandwidth 
                {
                  dl-Bandwidth n25,
                  ul-Bandwidth n6
                },
                additionalSpectrumEmission 10,
                t304 ms150,
                newUE-Identity '00000000 00000000'B,
                radioResourceConfigCommon 
                {
                  rach-Config 
                  {
                    preambleInfo 
                    {
                      numberOfRA-Preambles n36,
                      preamblesGroupAConfig 
                      {
                        sizeOfRA-PreamblesGroupA n24,
                        messageSizeGroupA b56,
                        messagePowerOffsetGroupB minusinfinity
                      }
                    },
                    powerRampingParameters 
                    {
                      powerRampingStep dB2,
                      preambleInitialReceivedTargetPower dBm-120
                    },
                    ra-SupervisionInfo 
                    {
                      preambleTransMax n5,
                      ra-ResponseWindowSize sf4,
                      mac-ContentionResolutionTimer sf32
                    },
                    maxHARQ-Msg3Tx 3
                  },
                  prach-Config 
                  {
                    rootSequenceIndex 5,
                    prach-ConfigInfo 
                    {
                      prach-ConfigIndex 1,
                      highSpeedFlag TRUE,
                      zeroCorrelationZoneConfig 1,
                      prach-FreqOffset 0
                    }
                  },
                  pdsch-ConfigCommon 
                  {
                    referenceSignalPower 0,
                    p-b 2
                  },
                  pusch-ConfigCommon 
                  {
                    pusch-ConfigBasic 
                    {
                      n-SB 1,
                      hoppingMode intraAndInterSubFrame,
                      pusch-HoppingOffset 0,
                      enable64QAM FALSE
                    },
                    ul-ReferenceSignalsPUSCH 
                    {
                      groupHoppingEnabled TRUE,
                      groupAssignmentPUSCH 2,
                      sequenceHoppingEnabled TRUE,
                      cyclicShift 0
                    }
                  },
                  phich-Config 
                  {
                    phich-Duration extended,
                    phich-Resource one
                  },
                  pucch-ConfigCommon 
                  {
                    deltaPUCCH-Shift ds3,
                    nRB-CQI 2,
                    nCS-AN 6,
                    n1PUCCH-AN 2
                  },
                  soundingRS-UL-ConfigCommon setup : 
                    {
                      srs-BandwidthConfig bw2,
                      srs-SubframeConfig sc2,
                      ackNackSRS-SimultaneousTransmission TRUE
                    },
                  uplinkPowerControlCommon 
                  {
                    p0-NominalPUSCH 5,
                    alpha al05,
                    p0-NominalPUCCH -100,
                    deltaFList-PUCCH 
                    {
                      deltaF-PUCCH-Format1 deltaF-2,
                      deltaF-PUCCH-Format1b deltaF1,
                      deltaF-PUCCH-Format2 deltaF-2,
                      deltaF-PUCCH-Format2a deltaF-2,
                      deltaF-PUCCH-Format2b deltaF-2
                    },
                    deltaPreambleMsg3 3
                  },
                  antennaInfoCommon 
                  {
                    antennaPortsCount an1
                  },
                  p-Max 10,
                  tdd-Config 
                  {
                    subframeAssignment sa2,
                    specialSubframePatterns ssp0
                  },
                  ul-CyclicPrefixLength len1
                },
                rach-ConfigDedicated 
                {
                  ra-PreambleIndex 1,
                  ra-PRACH-MaskIndex 1
                }
              }
            }
      }
}
*/

byte reconfig_msg_ho_without_security[37] = {0x22, 0x08, 0x3C, 0x15, 0x8C, 0xA0, 0x8D, 0x67, 0x91, 0x25, 0x00, 0x00, 0x3F, 0xEC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14,
0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x10, 0x01, 0x10,};

/*===========================================================================
                    Reconfig Message with Empty IEs

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 0,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
            }
      }
}
*/

byte reconfig_with_empty_ies[3] = {0x20, 0x00, 0x00};

/*===========================================================================
                    Mobility from EUTRA - CCO

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : mobilityFromEUTRACommand : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : mobilityFromEUTRACommand-r8 : 
            {
              cs-FallbackIndicator FALSE,
              purpose cellChangeOrder : 
                {
                  t304 ms500,
                  targetRAT-Type geran : 
                    {
                      physCellId 
                      {
                        networkColourCode '000'B,
                        baseStationColourCode '000'B
                      },
                      carrierFreq 
                      {
                        arfcn 100,
                        bandIndicator dcs1800
                      }
                    }
                }
            }
      }
}
*/

byte mobility_from_eutra_cco[5] = {0x1A, 0x0A, 0x00, 0x0C, 0x80};

/*===========================================================================
                    Mobility from EUTRA to W

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : mobilityFromEUTRACommand : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : mobilityFromEUTRACommand-r8 : 
            {
              cs-FallbackIndicator FALSE,
              purpose handover : 
                {
                  targetRAT-Type utra,
                  targetRAT-MessageContainer 'FF'H,
                  nas-SecurityParamFromEUTRA 'FF'H
                }
            }
      }
}
*/

byte mobility_from_eutra_to_w[6] = {0x1A, 0x04, 0x00, 0x3F, 0xFF, 0xE0};

/*===========================================================================
                    Mobility from EUTRA to 1x

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : mobilityFromEUTRACommand : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : mobilityFromEUTRACommand-r8 : 
            {
              cs-FallbackIndicator FALSE,
              purpose handover : 
                {
                  targetRAT-Type cdma2000-1XRTT,
                  targetRAT-MessageContainer 'FF'H
                }
            }
      }
}
*/

byte mobility_from_eutra_to_1x[5] = {0x1A, 0x00, 0x40, 0x3F, 0xE0};

/*===========================================================================
                    Mobility from EUTRA for eCSFB

============================================================================*/
/*
MobilityFromEUTRA:value DL-DCCH-Message ::= 
{
  message c1 : mobilityFromEUTRACommand : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : mobilityFromEUTRACommand-r9 : 
            {
              cs-FallbackIndicator FALSE,
              purpose e-CSFB-r9 : 
                {
                  messageContCDMA2000-1XRTT-r9 'FF'H,
                  mobilityCDMA2000-HRPD-r9 handover,
                  messageContCDMA2000-HRPD-r9 'FF'H
                }
            }
      }
}
*/

byte mobility_from_eutra_for_ecsfb[7] = {0x1A, 0x45, 0xC0, 0x3F, 0xE0, 0x1F, 0xF0};

/*===========================================================================
                    CCO from EUTRA to G for Rel 9

============================================================================*/
/*
MobilityFromEUTRA:value DL-DCCH-Message ::= 
{
  message c1 : mobilityFromEUTRACommand : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : mobilityFromEUTRACommand-r9 : 
            {
              cs-FallbackIndicator FALSE,
              purpose cellChangeOrder : 
                {
                  t304 ms1000,
                  targetRAT-Type geran : 
                    {
                      physCellId 
                      {
                        networkColourCode '111'B,
                        baseStationColourCode '000'B
                      },
                      carrierFreq 
                      {
                        arfcn 100,
                        bandIndicator dcs1800
                      },
                      networkControlOrder '00'B
                    }
                }
            }
      }
}
*/

byte mobility_from_eutra_cco_rel9[5] = {0x1A, 0x42, 0xD7, 0x03, 0x20};

/*===========================================================================
                    Mobility from EUTRA to 1x for Rel 9

============================================================================*/
/*
MobilityFromEUTRA:value DL-DCCH-Message ::= 
{
  message c1 : mobilityFromEUTRACommand : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : mobilityFromEUTRACommand-r9 : 
            {
              cs-FallbackIndicator FALSE,
              purpose handover : 
                {
                  targetRAT-Type cdma2000-1XRTT,
                  targetRAT-MessageContainer 'FF'H
                }
            }
      }
}
*/

byte mobility_from_eutra_to_1x_rel9[5] = {0x1A, 0x40, 0x10, 0x0F, 0xF8};

/*===========================================================================
                    Mobility from EUTRA to W for Rel 9

============================================================================*/
/*
MobilityFromEUTRA:value DL-DCCH-Message ::= 
{
  message c1 : mobilityFromEUTRACommand : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : mobilityFromEUTRACommand-r9 : 
            {
              cs-FallbackIndicator FALSE,
              purpose handover : 
                {
                  targetRAT-Type utra,
                  targetRAT-MessageContainer 'FF'H,
                  nas-SecurityParamFromEUTRA '00'H
                }
            }
      }
}
*/

byte mobility_from_eutra_to_w_rel9[6] = {0x1A, 0x41, 0x00, 0x0F, 0xF8, 0x00};

/*
unsupported_mobilityFromEUTRA:value DL-DCCH-Message ::= 
{
  message c1 : mobilityFromEUTRACommand : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : mobilityFromEUTRACommand-r8 : 
            {
              cs-FallbackIndicator FALSE,
              purpose handover : 
                {
                  targetRAT-Type spare4,
                  targetRAT-MessageContainer 'FF'H
                }
            }
      }
}
*/
byte mobility_from_eutra_to_spare4[5] = {0x1A ,0x00 ,0x80 ,0x3F ,0xE0};

/*
invalid_ho_msg:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measIdToAddModList 
                {
                  {
                    measId 1,
                    measObjectId 1,
                    reportConfigId 1
                  }
                },
                preRegistrationInfoHRPD 
                {
                  preRegistrationAllowed TRUE
                }
              },
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  },
                  {
                    srb-Identity 2,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      rlc-AM 
                      {
                        statusReportRequired TRUE
                      },
                      headerCompression notUsed : NULL
                    },
                    rlc-Config am : 
                      {
                        ul-AM-RLC 
                        {
                          t-PollRetransmit ms5,
                          pollPDU p4,
                          pollByte kB25,
                          maxRetxThreshold t1
                        },
                        dl-AM-RLC 
                        {
                          t-Reordering ms0,
                          t-StatusProhibit ms0
                        }
                      },
                    logicalChannelIdentity 3,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 2,
                        prioritisedBitRate kBps32,
                        bucketSizeDuration ms300
                      }
                    }
                  }
                },
                mac-MainConfig defaultValue : NULL,
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB-6
                  },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    }
                }
              },
              securityConfigHO 
              {
                handoverType intraLTE : 
                  {
                    securityAlgorithmConfig 
                    {
                      cipheringAlgorithm eea0,
                      integrityProtAlgorithm eia1
                    },
                    keyChangeIndicator FALSE,
                    nextHopChainingCount 0
                  }
              }
            }
      }
}

*/

byte ho_invalid_msg[25] = {0x22, 0x13, 0x01, 0x10, 0x00, 0x00, 0x16, 0xB6, 0xDF, 0x07, 0xCC, 0x45, 0x00, 0x00, 0x00, 0x00, 0x10, 0x9B, 0xA0, 0x11, 0x00, 0x00, 0x00, 0x80, 0x80,};

 /*
 Reconfig:value DL-DCCH-Message ::= 
 {
   message c1 : rrcConnectionReconfiguration : 
	   {
		 rrc-TransactionIdentifier 1,
		 criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
			 {
			   measConfig 
			   {
				 measIdToAddModList 
				 {
				   {
					 measId 1,
					 measObjectId 1,
					 reportConfigId 1
				   }
				 },
				 preRegistrationInfoHRPD 
				 {
				   preRegistrationAllowed TRUE
				 }
			   },
			   mobilityControlInfo 
			   {
				 targetPhysCellId 10,
				 carrierFreq 
				 {
				   dl-CarrierFreq 36000,
				   ul-CarrierFreq 36199
				 },
				 carrierBandwidth 
				 {
				   dl-Bandwidth n25,
				   ul-Bandwidth n6
				 },
				 additionalSpectrumEmission 10,
				 t304 ms150,
				 newUE-Identity '00000001 00000000'B,
				 radioResourceConfigCommon 
				 {
				   rach-ConfigCommon 
				   {
					 preambleInfo 
					 {
					   numberOfRA-Preambles n36,
					   preamblesGroupAConfig 
					   {
						 sizeOfRA-PreamblesGroupA n24,
						 messageSizeGroupA b56,
						 messagePowerOffsetGroupB minusinfinity
					   }
					 },
					 powerRampingParameters 
					 {
					   powerRampingStep dB2,
					   preambleInitialReceivedTargetPower dBm-120
					 },
					 ra-SupervisionInfo 
					 {
					   preambleTransMax n5,
					   ra-ResponseWindowSize sf4,
					   mac-ContentionResolutionTimer sf32
					 },
					 maxHARQ-Msg3Tx 3
				   },
				   prach-Config 
				   {
					 rootSequenceIndex 5,
					 prach-ConfigInfo 
					 {
					   prach-ConfigIndex 1,
					   highSpeedFlag TRUE,
					   zeroCorrelationZoneConfig 1,
					   prach-FreqOffset 0
					 }
				   },
				   pdsch-ConfigCommon 
				   {
					 referenceSignalPower 0,
					 p-b 2
				   },
				   pusch-ConfigCommon 
				   {
					 pusch-ConfigBasic 
					 {
					   n-SB 1,
					   hoppingMode intraAndInterSubFrame,
					   pusch-HoppingOffset 0,
					   enable64QAM FALSE
					 },
					 ul-ReferenceSignalsPUSCH 
					 {
					   groupHoppingEnabled TRUE,
					   groupAssignmentPUSCH 2,
					   sequenceHoppingEnabled TRUE,
					   cyclicShift 0
					 }
				   },
				   phich-Config 
				   {
					 phich-Duration extended,
					 phich-Resource one
				   },
				   pucch-ConfigCommon 
				   {
					 deltaPUCCH-Shift ds3,
					 nRB-CQI 2,
					 nCS-AN 6,
					 n1PUCCH-AN 2
				   },
				   soundingRS-UL-ConfigCommon setup : 
					 {
					   srs-BandwidthConfig bw2,
					   srs-SubframeConfig sc2,
					   ackNackSRS-SimultaneousTransmission TRUE
					 },
				   uplinkPowerControlCommon 
				   {
					 p0-NominalPUSCH 5,
					 alpha al05,
					 p0-NominalPUCCH -100,
					 deltaFList-PUCCH 
					 {
					   deltaF-PUCCH-Format1 deltaF-2,
					   deltaF-PUCCH-Format1b deltaF1,
					   deltaF-PUCCH-Format2 deltaF-2,
					   deltaF-PUCCH-Format2a deltaF-2,
					   deltaF-PUCCH-Format2b deltaF-2
					 },
					 deltaPreambleMsg3 3
				   },
				   antennaInfoCommon 
				   {
					 antennaPortsCount an1
				   },
				   p-Max 10,
				   tdd-Config 
				   {
					 subframeAssignment sa2,
					 specialSubframePatterns ssp0
				   },
				   ul-CyclicPrefixLength len1
				 },
				 rach-ConfigDedicated 
				 {
				   ra-PreambleIndex 1,
				   ra-PRACH-MaskIndex 1
				 }
			   },
			   radioResourceConfigDedicated 
			   {
				 srb-ToAddModList 
				 {
				   {
					 srb-Identity 1,
					 rlc-Config defaultValue : NULL,
					 logicalChannelConfig defaultValue : NULL
				   },
				   {
					 srb-Identity 2,
					 rlc-Config defaultValue : NULL,
					 logicalChannelConfig defaultValue : NULL
				   }
				 },
				 drb-ToAddModList 
				 {
				   {
					 eps-BearerIdentity 3,
					 drb-Identity 3,
					 pdcp-Config 
					 {
					   rlc-AM 
					   {
						 statusReportRequired TRUE
					   },
					   headerCompression notUsed : NULL
					 },
					 rlc-Config am : 
					   {
						 ul-AM-RLC 
						 {
						   t-PollRetransmit ms5,
						   pollPDU p4,
						   pollByte kB25,
						   maxRetxThreshold t1
						 },
						 dl-AM-RLC 
						 {
						   t-Reordering ms0,
						   t-StatusProhibit ms0
						 }
					   },
					 logicalChannelIdentity 3,
					 logicalChannelConfig 
					 {
					   ul-SpecificParameters 
					   {
						 priority 2,
						 prioritisedBitRate kBps32,
						 bucketSizeDuration ms300
					   }
					 }
				   }
				 },
				 mac-MainConfig defaultValue : NULL,
				 physicalConfigDedicated 
				 {
				   pdsch-ConfigDedicated 
				   {
					 p-a dB-6
				   },
				   schedulingRequestConfig setup : 
					 {
					   sr-PUCCH-ResourceIndex 0,
					   sr-ConfigIndex 0,
					   dsr-TransMax n4
					 }
				 }
			   },
			   securityConfigHO 
			   {
				 handoverType intraLTE : 
				   {
					 securityAlgorithmConfig 
					 {
					   cipheringAlgorithm eea0,
					   integrityProtAlgorithm eia1
					 },
					 keyChangeIndicator FALSE,
					 nextHopChainingCount 0
				   }
			   }
			 }
	   }
 }
 */
 
 byte ho_to_eutra_msg[59] = {0x22, 0x1B, 0x01, 0x10, 0x00, 0x00, 0x17, 0x82, 0xB1, 0x94, 0x11, 0xAC, 0xF2, 0x24, 0xA0, 0x10, 0x07, 0xFD, 0x82, 0x81, 0x02, 0x4D, 0x40, 0x50, 0x62,
 0x01, 0xE4, 0x40, 0x22, 0x8D, 0x02, 0xC0, 0x0A, 0x45, 0x83, 0x5B, 0x00, 0x21, 0x42, 0x00, 0x22, 0xD6, 0xDB, 0xE0, 0xF9, 0x88, 0xA0, 0x00, 0x00, 0x00,
 0x02, 0x13, 0x74, 0x02, 0x20, 0x00, 0x00, 0x10, 0x10,};


 /*===========================================================================
                    Reconfig Message for CA Scell Addition (Success)

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::=  
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 3,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              dedicatedInfoNASList 
              {
                '27F1701D9A02713B473656D28635DE358C ...'H
              },
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 2,
                    rlc-Config explicitValue : am : 
                        {
                          ul-AM-RLC 
                          {
                            t-PollRetransmit ms45,
                            pollPDU pInfinity,
                            pollByte kBinfinity,
                            maxRetxThreshold t32
                          },
                          dl-AM-RLC 
                          {
                            t-Reordering ms35,
                            t-StatusProhibit ms0
                          }
                        },
                    logicalChannelConfig explicitValue : 
                      {
                        ul-SpecificParameters 
                        {
                          priority 1,
                          prioritisedBitRate infinity,
                          bucketSizeDuration ms50,
                          logicalChannelGroup 0
                        }
                      }
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 5,
                    drb-Identity 1,
                    pdcp-Config 
                    {
                      discardTimer infinity,
                      rlc-AM 
                      {
                        statusReportRequired TRUE
                      },
                      headerCompression notUsed : NULL
                    },
                    rlc-Config am : 
                      {
                        ul-AM-RLC 
                        {
                          t-PollRetransmit ms50,
                          pollPDU p16,
                          pollByte kB50,
                          maxRetxThreshold t32
                        },
                        dl-AM-RLC 
                        {
                          t-Reordering ms40,
                          t-StatusProhibit ms50
                        }
                      },
                    logicalChannelIdentity 3,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 9,
                        prioritisedBitRate kBps8,
                        bucketSizeDuration ms300,
                        logicalChannelGroup 3
                      }
                    }
                  }
                },
                mac-MainConfig explicitValue : 
                  {
                    ul-SCH-Config 
                    {
                      maxHARQ-Tx n5,
                      periodicBSR-Timer sf40,
                      retxBSR-Timer sf320,
                      ttiBundling FALSE
                    },
                    drx-Config setup : 
                      {
                        onDurationTimer psf10,
                        drx-InactivityTimer psf100,
                        drx-RetransmissionTimer psf16,
                        longDRX-CycleStartOffset sf320 : 319
                      },
                    timeAlignmentTimerDedicated infinity
                  },
                physicalConfigDedicated 
                {
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm30,
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic setup : 
                      {
                        cqi-PUCCH-ResourceIndex 0,
                        cqi-pmi-ConfigIndex 26,
                        cqi-FormatIndicatorPeriodic widebandCQI : NULL,
                        ri-ConfigIndex 161,
                        simultaneousAckNackAndCQI TRUE
                      }
                  },
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm3,
                      codebookSubsetRestriction n2TxAntenna-tm3 : '11'B,
                      ue-TransmitAntennaSelection release : NULL
                    },
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm3,
                      codebookSubsetRestriction-r10 ''B,
                      ue-TransmitAntennaSelection release : NULL
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm30
                      },
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-PUCCH-ResourceIndexP1-r10 0,
                        cqi-pmi-ConfigIndex 26,
                        cqi-FormatIndicatorPeriodic-r10 widebandCQI-r10 : 
                          {
                            csi-ReportMode-r10 submode1
                          },
                        ri-ConfigIndex 161,
                        simultaneousAckNackAndCQI TRUE,
                        cqi-Mask-r9 setup,
                        csi-ConfigIndex-r10 release : NULL
                      },
                    pmi-RI-Report-r9 setup,
                    csi-SubframePatternConfig-r10 release : NULL
                  }
                }
              },
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  nonCriticalExtension 
                  {
                    sCellToAddModList-r10 
                    {
                      {
                        sCellIndex-r10 1,
                        cellIdentification-r10 
                        {
                          physCellId-r10 0,
                          dl-CarrierFreq-r10 0
                        },
                        radioResourceConfigCommonSCell-r10 
                        {
                          nonUL-Configuration-r10 
                          {
                            dl-Bandwidth-r10 n6,
                            antennaInfoCommon-r10 
                            {
                              antennaPortsCount an2
                            },
                            phich-Config-r10 
                            {
                              phich-Duration normal,
                              phich-Resource one
                            },
                            pdsch-ConfigCommon-r10 
                            {
                              referenceSignalPower 18,
                              p-b 1
                            }
                          },
                          ul-Configuration-r10 
                          {
                            ul-FreqInfo-r10 
                            {
                              ul-CarrierFreq-r10 1,
                              ul-Bandwidth-r10 n15,
                              additionalSpectrumEmissionSCell-r10 2
                            },
                            uplinkPowerControlCommonSCell-r10 
                            {
                              p0-NominalPUSCH-r10 -85,
                              alpha-r10 al08
                            },
                            soundingRS-UL-ConfigCommon-r10 setup : 
                              {
                                srs-BandwidthConfig bw3,
                                srs-SubframeConfig sc0,
                                ackNackSRS-SimultaneousTransmission TRUE
                              },
                            ul-CyclicPrefixLength-r10 len1,
                            pusch-ConfigCommon-r10 
                            {
                              pusch-ConfigBasic 
                              {
                                n-SB 1,
                                hoppingMode interSubFrame,
                                pusch-HoppingOffset 4,
                                enable64QAM FALSE
                              },
                              ul-ReferenceSignalsPUSCH 
                              {
                                groupHoppingEnabled TRUE,
                                groupAssignmentPUSCH 0,
                                sequenceHoppingEnabled FALSE,
                                cyclicShift 0
                              }
                            }
                          }
                        },
                        radioResourceConfigDedicatedSCell-r10 
                        {
                          physicalConfigDedicatedSCell-r10 
                          {
                            nonUL-Configuration-r10 
                            {
                              antennaInfo-r10 
                              {
                                transmissionMode-r10 tm2,
                                ue-TransmitAntennaSelection setup : closedLoop
                              }
                            },
                            ul-Configuration-r10 
                            {
                              antennaInfoUL-r10 
                              {
                                transmissionModeUL-r10 tm2
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
      }
}
*/

byte reconfig_ca_scell_add[187] = {0x26 , 0x06 , 0x83 , 0xA9 , 0x3F , 0x8B , 0x80 , 0xEC , 0xD0 , 0x13 , 0x89 , 0xDA , 
    0x39 , 0xB2 , 0xB6 , 0x94 , 0x31 , 0xAE , 0xF1 , 0xAC , 0x66 , 0x13 , 0xAA , 0x26 , 0xD9 , 0x74 , 0x49 , 0xA2 , 
    0xEE , 0xA6 , 0x90 , 0x22 , 0xBC , 0x5D , 0x3F , 0x66 , 0x22 , 0x99 , 0x43 , 0x25 , 0x36 , 0x68 , 0xDF , 0x42 , 
    0xF8 , 0x0A , 0x33 , 0x1F , 0xBF , 0xCA , 0xD9 , 0xEE , 0xAC , 0x71 , 0x7A , 0xF9 , 0xBC , 0xDB , 0x16 , 0xF2 , 
    0xA2 , 0x53 , 0xEC , 0x8C , 0x20 , 0x3A , 0x3E , 0xBE , 0xF0 , 0x0A , 0x32 , 0x6D , 0x98 , 0x16 , 0x12 , 0x00 , 
    0x62 , 0x7E , 0x1A , 0xCE , 0x61 , 0x05 , 0x33 , 0x28 , 0xBF , 0x49 , 0x4B , 0x66 , 0xB3 , 0x3F , 0x5B , 0x61 , 
    0x60 , 0xCB , 0x4B , 0x60 , 0x13 , 0x82 , 0x12 , 0x6E , 0x0A , 0xA6 , 0xD2 , 0xD8 , 0x88 , 0x88 , 0xD7 , 0x9A , 
    0x5D , 0x4F , 0xC2 , 0xBB , 0x97 , 0x17 , 0x07 , 0xF4 , 0x8C , 0xF4 , 0x23 , 0x9B , 0x63 , 0x53 , 0x81 , 0x1F , 
    0xB9 , 0xC0 , 0x30 , 0x70 , 0x03 , 0xEA , 0x06 , 0xF0 , 0x25 , 0x0F , 0x41 , 0x41 , 0xC0 , 0xBC , 0xDA , 0x28 , 
    0x4E , 0xEB , 0x33 , 0xFF , 0x02 , 0xB6 , 0x70 , 0x00 , 0x0D , 0x0A , 0x1A , 0x86 , 0x04 , 0x83 , 0x24 , 0x02 , 
    0x40 , 0x0F , 0x99 , 0xF8 , 0x00 , 0x00 , 0x00 , 0xD2 , 0x28 , 0x60 , 0x12 , 0x87 , 0x00 , 0x00 , 0x00 , 0x04 , 
    0x0A , 0x9C , 0x98 , 0x00 , 0x09 , 0x09 , 0x4D , 0x98 , 0x40 , 0x22 , 0x00 , 0x5C , 0x06 , 0x81 , 0x10};


/*===========================================================================
                    Reconfig Message for CA Scell Modify 

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 3,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              dedicatedInfoNASList 
              {
                '27F1701D9A02713B473656D28635DE358C ...'H
              },
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 2,
                    rlc-Config explicitValue : am : 
                        {
                          ul-AM-RLC 
                          {
                            t-PollRetransmit ms45,
                            pollPDU pInfinity,
                            pollByte kBinfinity,
                            maxRetxThreshold t32
                          },
                          dl-AM-RLC 
                          {
                            t-Reordering ms35,
                            t-StatusProhibit ms0
                          }
                        },
                    logicalChannelConfig explicitValue : 
                      {
                        ul-SpecificParameters 
                        {
                          priority 1,
                          prioritisedBitRate infinity,
                          bucketSizeDuration ms50,
                          logicalChannelGroup 0
                        }
                      }
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 5,
                    drb-Identity 1,
                    pdcp-Config 
                    {
                      discardTimer infinity,
                      rlc-AM 
                      {
                        statusReportRequired TRUE
                      },
                      headerCompression notUsed : NULL
                    },
                    rlc-Config am : 
                      {
                        ul-AM-RLC 
                        {
                          t-PollRetransmit ms50,
                          pollPDU p16,
                          pollByte kB50,
                          maxRetxThreshold t32
                        },
                        dl-AM-RLC 
                        {
                          t-Reordering ms40,
                          t-StatusProhibit ms50
                        }
                      },
                    logicalChannelIdentity 3,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 9,
                        prioritisedBitRate kBps8,
                        bucketSizeDuration ms300,
                        logicalChannelGroup 3
                      }
                    }
                  }
                },
                mac-MainConfig explicitValue : 
                  {
                    ul-SCH-Config 
                    {
                      maxHARQ-Tx n5,
                      periodicBSR-Timer sf40,
                      retxBSR-Timer sf320,
                      ttiBundling FALSE
                    },
                    drx-Config setup : 
                      {
                        onDurationTimer psf10,
                        drx-InactivityTimer psf100,
                        drx-RetransmissionTimer psf16,
                        longDRX-CycleStartOffset sf320 : 319
                      },
                    timeAlignmentTimerDedicated infinity
                  },
                physicalConfigDedicated 
                {
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm30,
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic setup : 
                      {
                        cqi-PUCCH-ResourceIndex 0,
                        cqi-pmi-ConfigIndex 26,
                        cqi-FormatIndicatorPeriodic widebandCQI : NULL,
                        ri-ConfigIndex 161,
                        simultaneousAckNackAndCQI TRUE
                      }
                  },
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm3,
                      codebookSubsetRestriction n2TxAntenna-tm3 : '11'B,
                      ue-TransmitAntennaSelection release : NULL
                    },
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm3,
                      codebookSubsetRestriction-r10 ''B,
                      ue-TransmitAntennaSelection release : NULL
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm30
                      },
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-PUCCH-ResourceIndexP1-r10 0,
                        cqi-pmi-ConfigIndex 26,
                        cqi-FormatIndicatorPeriodic-r10 widebandCQI-r10 : 
                          {
                            csi-ReportMode-r10 submode1
                          },
                        ri-ConfigIndex 161,
                        simultaneousAckNackAndCQI TRUE,
                        cqi-Mask-r9 setup,
                        csi-ConfigIndex-r10 release : NULL
                      },
                    pmi-RI-Report-r9 setup,
                    csi-SubframePatternConfig-r10 release : NULL
                  }
                }
              },
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  nonCriticalExtension 
                  {
                    sCellToAddModList-r10 
                    {
                      {
                        sCellIndex-r10 1,
                        cellIdentification-r10 
                        {
                          physCellId-r10 0,
                          dl-CarrierFreq-r10 0
                        },
                        radioResourceConfigDedicatedSCell-r10 
                        {
                          physicalConfigDedicatedSCell-r10 
                          {
                            nonUL-Configuration-r10 
                            {
                              antennaInfo-r10 
                              {
                                transmissionMode-r10 tm2,
                                ue-TransmitAntennaSelection setup : closedLoop
                              }
                            },
                            ul-Configuration-r10 
                            {
                              antennaInfoUL-r10 
                              {
                                transmissionModeUL-r10 tm2
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
      }
}
*/ 
byte reconfig_ca_scell_modify[175]  = {0x26 , 0x06 , 0x83 , 0xA9 , 0x3F , 0x8B , 0x80 , 0xEC , 0xD0 , 0x13 , 0x89 , 0xDA ,
    0x39 , 0xB2 , 0xB6 , 0x94 , 0x31 , 0xAE , 0xF1 , 0xAC , 0x66 , 0x13 , 0xAA , 0x26 , 0xD9 , 0x74 , 0x49 , 0xA2 , 
    0xEE , 0xA6 , 0x90 , 0x22 , 0xBC , 0x5D , 0x3F , 0x66 , 0x22 , 0x99 , 0x43 , 0x25 , 0x36 , 0x68 , 0xDF , 0x42 , 
    0xF8 , 0x0A , 0x33 , 0x1F , 0xBF , 0xCA , 0xD9 , 0xEE , 0xAC , 0x71 , 0x7A , 0xF9 , 0xBC , 0xDB , 0x16 , 0xF2 , 
    0xA2 , 0x53 , 0xEC , 0x8C , 0x20 , 0x3A , 0x3E , 0xBE , 0xF0 , 0x0A , 0x32 , 0x6D , 0x98 , 0x16 , 0x12 , 0x00 , 
    0x62 , 0x7E , 0x1A , 0xCE , 0x61 , 0x05 , 0x33 , 0x28 , 0xBF , 0x49 , 0x4B , 0x66 , 0xB3 , 0x3F , 0x5B , 0x61 , 
    0x60 , 0xCB , 0x4B , 0x60 , 0x13 , 0x82 , 0x12 , 0x6E , 0x0A , 0xA6 , 0xD2 , 0xD8 , 0x88 , 0x88 , 0xD7 , 0x9A , 
    0x5D , 0x4F , 0xC2 , 0xBB , 0x97 , 0x17 , 0x07 , 0xF4 , 0x8C , 0xF4 , 0x23 , 0x9B , 0x63 , 0x53 , 0x81 , 0x1F , 
    0xB9 , 0xC0 , 0x30 , 0x70 , 0x03 , 0xEA , 0x06 , 0xF0 , 0x25 , 0x0F , 0x41 , 0x41 , 0xC0 , 0xBC , 0xDA , 0x28 , 
    0x4E , 0xEB , 0x33 , 0xFF , 0x02 , 0xB6 , 0x70 , 0x00 , 0x0D , 0x0A , 0x1A , 0x86 , 0x04 , 0x83 , 0x24 , 0x02 , 
    0x40 , 0x0F , 0x99 , 0xF8 , 0x00 , 0x00 , 0x00 , 0xD2 , 0x28 , 0x60 , 0x12 , 0x85 , 0x00 , 0x00 , 0x00 , 0x05 , 
    0xC0 , 0x68 , 0x11};

/*===========================================================================
                    Reconfig Message for CA Scell Release

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 0,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  nonCriticalExtension 
                  {
                    sCellToReleaseList-r10 
                    {
                      2
                    }
                  }
                }
              }
            }
      }
}

*/ 
byte reconfig_ca_scell_release[4] = {0x20 , 0x00 , 0xA6 , 0x04 };


/*===========================================================================
                    Reconfig Message for HO from Pcell to Pcell+Scell

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              mobilityControlInfo 
              {
                targetPhysCellId 10,
                carrierFreq 
                {
                  dl-CarrierFreq 36000,
                  ul-CarrierFreq 36199
                },
                carrierBandwidth 
                {
                  dl-Bandwidth n25,
                  ul-Bandwidth n6
                },
                additionalSpectrumEmission 10,
                t304 ms150,
                newUE-Identity '00000001 00000000'B,
                radioResourceConfigCommon 
                {
                  rach-ConfigCommon 
                  {
                    preambleInfo 
                    {
                      numberOfRA-Preambles n36,
                      preamblesGroupAConfig 
                      {
                        sizeOfRA-PreamblesGroupA n24,
                        messageSizeGroupA b56,
                        messagePowerOffsetGroupB minusinfinity
                      }
                    },
                    powerRampingParameters 
                    {
                      powerRampingStep dB2,
                      preambleInitialReceivedTargetPower dBm-120
                    },
                    ra-SupervisionInfo 
                    {
                      preambleTransMax n5,
                      ra-ResponseWindowSize sf4,
                      mac-ContentionResolutionTimer sf32
                    },
                    maxHARQ-Msg3Tx 3
                  },
                  prach-Config 
                  {
                    rootSequenceIndex 5,
                    prach-ConfigInfo 
                    {
                      prach-ConfigIndex 1,
                      highSpeedFlag TRUE,
                      zeroCorrelationZoneConfig 1,
                      prach-FreqOffset 0
                    }
                  },
                  pdsch-ConfigCommon 
                  {
                    referenceSignalPower 0,
                    p-b 2
                  },
                  pusch-ConfigCommon 
                  {
                    pusch-ConfigBasic 
                    {
                      n-SB 1,
                      hoppingMode intraAndInterSubFrame,
                      pusch-HoppingOffset 0,
                      enable64QAM FALSE
                    },
                    ul-ReferenceSignalsPUSCH 
                    {
                      groupHoppingEnabled TRUE,
                      groupAssignmentPUSCH 2,
                      sequenceHoppingEnabled TRUE,
                      cyclicShift 0
                    }
                  },
                  phich-Config 
                  {
                    phich-Duration extended,
                    phich-Resource one
                  },
                  pucch-ConfigCommon 
                  {
                    deltaPUCCH-Shift ds3,
                    nRB-CQI 2,
                    nCS-AN 6,
                    n1PUCCH-AN 2
                  },
                  soundingRS-UL-ConfigCommon setup : 
                    {
                      srs-BandwidthConfig bw2,
                      srs-SubframeConfig sc2,
                      ackNackSRS-SimultaneousTransmission TRUE
                    },
                  uplinkPowerControlCommon 
                  {
                    p0-NominalPUSCH 5,
                    alpha al05,
                    p0-NominalPUCCH -100,
                    deltaFList-PUCCH 
                    {
                      deltaF-PUCCH-Format1 deltaF-2,
                      deltaF-PUCCH-Format1b deltaF1,
                      deltaF-PUCCH-Format2 deltaF-2,
                      deltaF-PUCCH-Format2a deltaF-2,
                      deltaF-PUCCH-Format2b deltaF-2
                    },
                    deltaPreambleMsg3 3
                  },
                  antennaInfoCommon 
                  {
                    antennaPortsCount an1
                  },
                  p-Max 10,
                  tdd-Config 
                  {
                    subframeAssignment sa2,
                    specialSubframePatterns ssp0
                  },
                  ul-CyclicPrefixLength len1
                },
                rach-ConfigDedicated 
                {
                  ra-PreambleIndex 1,
                  ra-PRACH-MaskIndex 1
                }
              },
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 5,
                    drb-Identity 1,
                    pdcp-Config 
                    {
                      discardTimer ms100,
                      rlc-UM 
                      {
                        pdcp-SN-Size len12bits
                      },
                      headerCompression rohc : 
                        {
                          maxCID 1,
                          profiles 
                          {
                            profile0x0001 FALSE,
                            profile0x0002 FALSE,
                            profile0x0003 FALSE,
                            profile0x0004 FALSE,
                            profile0x0006 FALSE,
                            profile0x0101 FALSE,
                            profile0x0102 FALSE,
                            profile0x0103 FALSE,
                            profile0x0104 FALSE
                          }
                        }
                    },
                    rlc-Config um-Bi-Directional : 
                      {
                        ul-UM-RLC 
                        {
                          sn-FieldLength size10
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size10,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 3,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 3,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                },
                drb-ToReleaseList 
                {
                  3
                },
                mac-MainConfig explicitValue : 
                  {
                    ul-SCH-Config 
                    {
                      maxHARQ-Tx n5,
                      periodicBSR-Timer sf16,
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    drx-Config release : NULL,
                    timeAlignmentTimerDedicated sf5120,
                    phr-Config setup : 
                      {
                        periodicPHR-Timer sf10,
                        prohibitPHR-Timer sf0,
                        dl-PathlossChange dB1
                      }
                  },
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL setup : 
                    {
                      semiPersistSchedIntervalDL sf10,
                      numberOfConfSPS-Processes 2,
                      n1PUCCH-AN-PersistentList 
                      {
                        4
                      }
                    },
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf10,
                      implicitReleaseAfter e2,
                      p0-Persistent 
                      {
                        p0-NominalPUSCH-Persistent 5,
                        p0-UE-PUSCH-Persistent 5
                      },
                      twoIntervalsConfig true
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB-3
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition setup : 
                      {
                        repetitionFactor n2,
                        n1PUCCH-AN-Rep 0
                      },
                    tdd-AckNackFeedbackMode bundling
                  },
                  pusch-ConfigDedicated 
                  {
                    betaOffset-ACK-Index 0,
                    betaOffset-RI-Index 0,
                    betaOffset-CQI-Index 0
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 5,
                    deltaMCS-Enabled en1,
                    accumulationEnabled FALSE,
                    p0-UE-PUCCH 0,
                    pSRS-Offset 0
                  },
                  tpc-PDCCH-ConfigPUCCH setup : 
                    {
                      tpc-RNTI '00000001 00000000'B,
                      tpc-Index indexOfFormat3 : 1
                    },
                  tpc-PDCCH-ConfigPUSCH setup : 
                    {
                      tpc-RNTI '00000001 00000000'B,
                      tpc-Index indexOfFormat3 : 1
                    },
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm20,
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic setup : 
                      {
                        cqi-PUCCH-ResourceIndex 0,
                        cqi-pmi-ConfigIndex 0,
                        cqi-FormatIndicatorPeriodic subbandCQI : 
                          {
                            k 1
                          },
                        ri-ConfigIndex 0,
                        simultaneousAckNackAndCQI FALSE
                      }
                  },
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw2,
                      srs-HoppingBandwidth hbw1,
                      freqDomainPosition 0,
                      duration FALSE,
                      srs-ConfigIndex 0,
                      transmissionComb 0,
                      cyclicShift cs3
                    },
                  antennaInfo defaultValue : NULL,
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    }
                }
              },
              securityConfigHO 
              {
                handoverType intraLTE : 
                  {
                    securityAlgorithmConfig 
                    {
                      cipheringAlgorithm eea0,
                      integrityProtAlgorithm eia1
                    },
                    keyChangeIndicator FALSE,
                    nextHopChainingCount 0
                  }
              },
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  nonCriticalExtension 
                  {
                    sCellToAddModList-r10 
                    {
                      {
                        sCellIndex-r10 1,
                        cellIdentification-r10 
                        {
                          physCellId-r10 0,
                          dl-CarrierFreq-r10 0
                        },
                        radioResourceConfigCommonSCell-r10 
                        {
                          nonUL-Configuration-r10 
                          {
                            dl-Bandwidth-r10 n6,
                            antennaInfoCommon-r10 
                            {
                              antennaPortsCount an2
                            },
                            phich-Config-r10 
                            {
                              phich-Duration normal,
                              phich-Resource one
                            },
                            pdsch-ConfigCommon-r10 
                            {
                              referenceSignalPower 18,
                              p-b 1
                            }
                          },
                          ul-Configuration-r10 
                          {
                            ul-FreqInfo-r10 
                            {
                              ul-CarrierFreq-r10 1,
                              ul-Bandwidth-r10 n15,
                              additionalSpectrumEmissionSCell-r10 2
                            },
                            uplinkPowerControlCommonSCell-r10 
                            {
                              p0-NominalPUSCH-r10 -85,
                              alpha-r10 al08
                            },
                            soundingRS-UL-ConfigCommon-r10 setup : 
                              {
                                srs-BandwidthConfig bw3,
                                srs-SubframeConfig sc0,
                                ackNackSRS-SimultaneousTransmission TRUE
                              },
                            ul-CyclicPrefixLength-r10 len1,
                            pusch-ConfigCommon-r10 
                            {
                              pusch-ConfigBasic 
                              {
                                n-SB 1,
                                hoppingMode interSubFrame,
                                pusch-HoppingOffset 4,
                                enable64QAM FALSE
                              },
                              ul-ReferenceSignalsPUSCH 
                              {
                                groupHoppingEnabled TRUE,
                                groupAssignmentPUSCH 0,
                                sequenceHoppingEnabled FALSE,
                                cyclicShift 0
                              }
                            }
                          }
                        },
                        radioResourceConfigDedicatedSCell-r10 
                        {
                          physicalConfigDedicatedSCell-r10 
                          {
                            nonUL-Configuration-r10 
                            {
                              antennaInfo-r10 
                              {
                                transmissionMode-r10 tm2,
                                ue-TransmitAntennaSelection setup : closedLoop
                              }
                            },
                            ul-Configuration-r10 
                            {
                              antennaInfoUL-r10 
                              {
                                transmissionModeUL-r10 tm2
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
      }
}
*/ 
byte reconfig_with_ho_pcell_to_pcell_scell[108] = {0x22 ,0x0B ,0xBC ,0x15 ,0x8C ,0xA0 ,0x8D ,0x67 ,0x90
     ,0x25 ,0x00 ,0x80 ,0x3F ,0xEC ,0x14 ,0x08 ,0x12 ,0x6A ,0x02 ,0x83 ,0x10 ,0x0F ,0x22 ,0x01 ,0x14 ,0x68
     ,0x16 ,0x00 ,0x52 ,0x2C ,0x1A ,0xD8 ,0x01 ,0x0A ,0x10 ,0x01 ,0x17 ,0xE4 ,0x83 ,0xEA ,0x05 ,0x3A ,0x00
     ,0x00 ,0x00 ,0xE2 ,0x08 ,0x88 ,0x00 ,0x8F ,0xA1 ,0x0A ,0xC0 ,0x38 ,0x08 ,0x04 ,0x04 ,0x00 ,0x96 ,0x04
     ,0x1E ,0xBF ,0xF5 ,0x80 ,0x00 ,0x00 ,0x0D ,0xA0 ,0x20 ,0x20 ,0x00 ,0x80 ,0x80 ,0x03 ,0x27 ,0x00 ,0x00
     ,0x04 ,0x00 ,0x19 ,0x00 ,0x00 ,0x3C ,0x00 ,0x00 ,0x02 ,0x02 ,0x09 ,0x43 ,0x80 ,0x00 ,0x00 ,0x02 ,0x05
     ,0x4E ,0x4C ,0x00 ,0x04 ,0x84 ,0xA6 ,0xCC ,0x20 ,0x11 ,0x00 ,0x2E ,0x03 ,0x40 ,0x88 };


/*===========================================================================
                    Reconfig Message for CA Scell Change

============================================================================*/
/* 
Reconfig:value DL-DCCH-Message ::=  
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 3,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              dedicatedInfoNASList 
              {
                '27F1701D9A02713B473656D28635DE358C ...'H
              },
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 2,
                    rlc-Config explicitValue : am : 
                        {
                          ul-AM-RLC 
                          {
                            t-PollRetransmit ms45,
                            pollPDU pInfinity,
                            pollByte kBinfinity,
                            maxRetxThreshold t32
                          },
                          dl-AM-RLC 
                          {
                            t-Reordering ms35,
                            t-StatusProhibit ms0
                          }
                        },
                    logicalChannelConfig explicitValue : 
                      {
                        ul-SpecificParameters 
                        {
                          priority 1,
                          prioritisedBitRate infinity,
                          bucketSizeDuration ms50,
                          logicalChannelGroup 0
                        }
                      }
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 5,
                    drb-Identity 1,
                    pdcp-Config 
                    {
                      discardTimer infinity,
                      rlc-AM 
                      {
                        statusReportRequired TRUE
                      },
                      headerCompression notUsed : NULL
                    },
                    rlc-Config am : 
                      {
                        ul-AM-RLC 
                        {
                          t-PollRetransmit ms50,
                          pollPDU p16,
                          pollByte kB50,
                          maxRetxThreshold t32
                        },
                        dl-AM-RLC 
                        {
                          t-Reordering ms40,
                          t-StatusProhibit ms50
                        }
                      },
                    logicalChannelIdentity 3,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 9,
                        prioritisedBitRate kBps8,
                        bucketSizeDuration ms300,
                        logicalChannelGroup 3
                      }
                    }
                  }
                },
                mac-MainConfig explicitValue : 
                  {
                    ul-SCH-Config 
                    {
                      maxHARQ-Tx n5,
                      periodicBSR-Timer sf40,
                      retxBSR-Timer sf320,
                      ttiBundling FALSE
                    },
                    drx-Config setup : 
                      {
                        onDurationTimer psf10,
                        drx-InactivityTimer psf100,
                        drx-RetransmissionTimer psf16,
                        longDRX-CycleStartOffset sf320 : 319
                      },
                    timeAlignmentTimerDedicated infinity
                  },
                physicalConfigDedicated 
                {
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm30,
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic setup : 
                      {
                        cqi-PUCCH-ResourceIndex 0,
                        cqi-pmi-ConfigIndex 26,
                        cqi-FormatIndicatorPeriodic widebandCQI : NULL,
                        ri-ConfigIndex 161,
                        simultaneousAckNackAndCQI TRUE
                      }
                  },
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm3,
                      codebookSubsetRestriction n2TxAntenna-tm3 : '11'B,
                      ue-TransmitAntennaSelection release : NULL
                    },
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm3,
                      codebookSubsetRestriction-r10 ''B,
                      ue-TransmitAntennaSelection release : NULL
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm30
                      },
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-PUCCH-ResourceIndexP1-r10 0,
                        cqi-pmi-ConfigIndex 26,
                        cqi-FormatIndicatorPeriodic-r10 widebandCQI-r10 : 
                          {
                            csi-ReportMode-r10 submode1
                          },
                        ri-ConfigIndex 161,
                        simultaneousAckNackAndCQI TRUE,
                        cqi-Mask-r9 setup,
                        csi-ConfigIndex-r10 release : NULL
                      },
                    pmi-RI-Report-r9 setup,
                    csi-SubframePatternConfig-r10 release : NULL
                  }
                }
              },
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  nonCriticalExtension 
                  {
                    sCellToReleaseList-r10 
                    {
                      1
                    },
                    sCellToAddModList-r10 
                    {
                      {
                        sCellIndex-r10 1,
                        cellIdentification-r10 
                        {
                          physCellId-r10 0,
                          dl-CarrierFreq-r10 0
                        },
                        radioResourceConfigCommonSCell-r10 
                        {
                          nonUL-Configuration-r10 
                          {
                            dl-Bandwidth-r10 n6,
                            antennaInfoCommon-r10 
                            {
                              antennaPortsCount an2
                            },
                            phich-Config-r10 
                            {
                              phich-Duration normal,
                              phich-Resource one
                            },
                            pdsch-ConfigCommon-r10 
                            {
                              referenceSignalPower 18,
                              p-b 1
                            }
                          },
                          ul-Configuration-r10 
                          {
                            ul-FreqInfo-r10 
                            {
                              ul-CarrierFreq-r10 1,
                              ul-Bandwidth-r10 n15,
                              additionalSpectrumEmissionSCell-r10 2
                            },
                            uplinkPowerControlCommonSCell-r10 
                            {
                              p0-NominalPUSCH-r10 -85,
                              alpha-r10 al08
                            },
                            soundingRS-UL-ConfigCommon-r10 setup : 
                              {
                                srs-BandwidthConfig bw3,
                                srs-SubframeConfig sc0,
                                ackNackSRS-SimultaneousTransmission TRUE
                              },
                            ul-CyclicPrefixLength-r10 len1,
                            pusch-ConfigCommon-r10 
                            {
                              pusch-ConfigBasic 
                              {
                                n-SB 1,
                                hoppingMode interSubFrame,
                                pusch-HoppingOffset 4,
                                enable64QAM FALSE
                              },
                              ul-ReferenceSignalsPUSCH 
                              {
                                groupHoppingEnabled TRUE,
                                groupAssignmentPUSCH 0,
                                sequenceHoppingEnabled FALSE,
                                cyclicShift 0
                              }
                            }
                          }
                        },
                        radioResourceConfigDedicatedSCell-r10 
                        {
                          physicalConfigDedicatedSCell-r10 
                          {
                            nonUL-Configuration-r10 
                            {
                              antennaInfo-r10 
                              {
                                transmissionMode-r10 tm2,
                                ue-TransmitAntennaSelection setup : closedLoop
                              }
                            },
                            ul-Configuration-r10 
                            {
                              antennaInfoUL-r10 
                              {
                                transmissionModeUL-r10 tm2
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
      }
}
*/ 
byte reconfig_ca_scell_change[188] = {0x26 ,0x06 ,0x83 ,0xA9 ,0x3F ,0x8B ,0x80 ,0xEC ,0xD0 ,0x13 ,0x89
     ,0xDA ,0x39 ,0xB2 ,0xB6 ,0x94 ,0x31 ,0xAE ,0xF1 ,0xAC ,0x66 ,0x13 ,0xAA ,0x26 ,0xD9 ,0x74 ,0x49
     ,0xA2 ,0xEE ,0xA6 ,0x90 ,0x22 ,0xBC ,0x5D ,0x3F ,0x66 ,0x22 ,0x99 ,0x43 ,0x25 ,0x36 ,0x68 ,0xDF
     ,0x42 ,0xF8 ,0x0A ,0x33 ,0x1F ,0xBF ,0xCA ,0xD9 ,0xEE ,0xAC ,0x71 ,0x7A ,0xF9 ,0xBC ,0xDB ,0x16
     ,0xF2 ,0xA2 ,0x53 ,0xEC ,0x8C ,0x20 ,0x3A ,0x3E ,0xBE ,0xF0 ,0x0A ,0x32 ,0x6D ,0x98 ,0x16 ,0x12
     ,0x00 ,0x62 ,0x7E ,0x1A ,0xCE ,0x61 ,0x05 ,0x33 ,0x28 ,0xBF ,0x49 ,0x4B ,0x66 ,0xB3 ,0x3F ,0x5B
     ,0x61 ,0x60 ,0xCB ,0x4B ,0x60 ,0x13 ,0x82 ,0x12 ,0x6E ,0x0A ,0xA6 ,0xD2 ,0xD8 ,0x88 ,0x88 ,0xD7
     ,0x9A ,0x5D ,0x4F ,0xC2 ,0xBB ,0x97 ,0x17 ,0x07 ,0xF4 ,0x8C ,0xF4 ,0x23 ,0x9B ,0x63 ,0x53 ,0x81
     ,0x1F ,0xB9 ,0xC0 ,0x30 ,0x70 ,0x03 ,0xEA ,0x06 ,0xF0 ,0x25 ,0x0F ,0x41 ,0x41 ,0xC0 ,0xBC ,0xDA
     ,0x28 ,0x4E ,0xEB ,0x33 ,0xFF ,0x02 ,0xB6 ,0x70 ,0x00 ,0x0D ,0x0A ,0x1A ,0x86 ,0x04 ,0x83 ,0x24
     ,0x02 ,0x40 ,0x0F ,0x99 ,0xF8 ,0x00 ,0x00 ,0x00 ,0xD2 ,0x28 ,0x60 ,0x13 ,0x80 ,0x38 ,0x00 ,0x00
     ,0x00 ,0x20 ,0x54 ,0xE4 ,0xC0 ,0x00 ,0x48 ,0x4A ,0x6C ,0xC2 ,0x01 ,0x10 ,0x02 ,0xE0 ,0x34 ,0x08
     ,0x80 };


/*===========================================================================
                    Reconfig Message for CA Scell Add with Event A1

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::=  
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 3,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 1,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType event : 
                          {
                            eventId eventA1 : 
                              {
                                a1-Threshold threshold-RSRP : 0
                              },
                            hysteresis 0,
                            timeToTrigger ms0
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 6,
                        reportInterval ms5120,
                        reportAmount r16
                      }
                  }
                }
              },
              dedicatedInfoNASList 
              {
                '27F1701D9A02713B473656D28635DE358C ...'H
              },
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 2,
                    rlc-Config explicitValue : am : 
                        {
                          ul-AM-RLC 
                          {
                            t-PollRetransmit ms45,
                            pollPDU pInfinity,
                            pollByte kBinfinity,
                            maxRetxThreshold t32
                          },
                          dl-AM-RLC 
                          {
                            t-Reordering ms35,
                            t-StatusProhibit ms0
                          }
                        },
                    logicalChannelConfig explicitValue : 
                      {
                        ul-SpecificParameters 
                        {
                          priority 1,
                          prioritisedBitRate infinity,
                          bucketSizeDuration ms50,
                          logicalChannelGroup 0
                        }
                      }
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 5,
                    drb-Identity 1,
                    pdcp-Config 
                    {
                      discardTimer infinity,
                      rlc-AM 
                      {
                        statusReportRequired TRUE
                      },
                      headerCompression notUsed : NULL
                    },
                    rlc-Config am : 
                      {
                        ul-AM-RLC 
                        {
                          t-PollRetransmit ms50,
                          pollPDU p16,
                          pollByte kB50,
                          maxRetxThreshold t32
                        },
                        dl-AM-RLC 
                        {
                          t-Reordering ms40,
                          t-StatusProhibit ms50
                        }
                      },
                    logicalChannelIdentity 3,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 9,
                        prioritisedBitRate kBps8,
                        bucketSizeDuration ms300,
                        logicalChannelGroup 3
                      }
                    }
                  }
                },
                mac-MainConfig explicitValue : 
                  {
                    ul-SCH-Config 
                    {
                      maxHARQ-Tx n5,
                      periodicBSR-Timer sf40,
                      retxBSR-Timer sf320,
                      ttiBundling FALSE
                    },
                    drx-Config setup : 
                      {
                        onDurationTimer psf10,
                        drx-InactivityTimer psf100,
                        drx-RetransmissionTimer psf16,
                        longDRX-CycleStartOffset sf320 : 319
                      },
                    timeAlignmentTimerDedicated infinity
                  },
                physicalConfigDedicated 
                {
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm30,
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic setup : 
                      {
                        cqi-PUCCH-ResourceIndex 0,
                        cqi-pmi-ConfigIndex 26,
                        cqi-FormatIndicatorPeriodic widebandCQI : NULL,
                        ri-ConfigIndex 161,
                        simultaneousAckNackAndCQI TRUE
                      }
                  },
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm3,
                      codebookSubsetRestriction n2TxAntenna-tm3 : '11'B,
                      ue-TransmitAntennaSelection release : NULL
                    },
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm3,
                      codebookSubsetRestriction-r10 ''B,
                      ue-TransmitAntennaSelection release : NULL
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm30
                      },
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-PUCCH-ResourceIndexP1-r10 0,
                        cqi-pmi-ConfigIndex 26,
                        cqi-FormatIndicatorPeriodic-r10 widebandCQI-r10 : 
                          {
                            csi-ReportMode-r10 submode1
                          },
                        ri-ConfigIndex 161,
                        simultaneousAckNackAndCQI TRUE,
                        cqi-Mask-r9 setup,
                        csi-ConfigIndex-r10 release : NULL
                      },
                    pmi-RI-Report-r9 setup,
                    csi-SubframePatternConfig-r10 release : NULL
                  }
                }
              },
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  nonCriticalExtension 
                  {
                    sCellToAddModList-r10 
                    {
                      {
                        sCellIndex-r10 1,
                        cellIdentification-r10 
                        {
                          physCellId-r10 0,
                          dl-CarrierFreq-r10 0
                        },
                        radioResourceConfigCommonSCell-r10 
                        {
                          nonUL-Configuration-r10 
                          {
                            dl-Bandwidth-r10 n6,
                            antennaInfoCommon-r10 
                            {
                              antennaPortsCount an2
                            },
                            phich-Config-r10 
                            {
                              phich-Duration normal,
                              phich-Resource one
                            },
                            pdsch-ConfigCommon-r10 
                            {
                              referenceSignalPower 18,
                              p-b 1
                            }
                          },
                          ul-Configuration-r10 
                          {
                            ul-FreqInfo-r10 
                            {
                              ul-CarrierFreq-r10 1,
                              ul-Bandwidth-r10 n15,
                              additionalSpectrumEmissionSCell-r10 2
                            },
                            uplinkPowerControlCommonSCell-r10 
                            {
                              p0-NominalPUSCH-r10 -85,
                              alpha-r10 al08
                            },
                            soundingRS-UL-ConfigCommon-r10 setup : 
                              {
                                srs-BandwidthConfig bw3,
                                srs-SubframeConfig sc0,
                                ackNackSRS-SimultaneousTransmission TRUE
                              },
                            ul-CyclicPrefixLength-r10 len1,
                            pusch-ConfigCommon-r10 
                            {
                              pusch-ConfigBasic 
                              {
                                n-SB 1,
                                hoppingMode interSubFrame,
                                pusch-HoppingOffset 4,
                                enable64QAM FALSE
                              },
                              ul-ReferenceSignalsPUSCH 
                              {
                                groupHoppingEnabled TRUE,
                                groupAssignmentPUSCH 0,
                                sequenceHoppingEnabled FALSE,
                                cyclicShift 0
                              }
                            }
                          }
                        },
                        radioResourceConfigDedicatedSCell-r10 
                        {
                          physicalConfigDedicatedSCell-r10 
                          {
                            nonUL-Configuration-r10 
                            {
                              antennaInfo-r10 
                              {
                                transmissionMode-r10 tm2,
                                ue-TransmitAntennaSelection setup : closedLoop
                              }
                            },
                            ul-Configuration-r10 
                            {
                              antennaInfoUL-r10 
                              {
                                transmissionModeUL-r10 tm2
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
      }
}

 */
byte reconfig_ca_scell_add_configure_a1[194] = {0x26 , 0x16 , 0x84 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 
    0xD6 , 0x80 , 0xEA , 0x4F , 0xE2 , 0xE0 , 0x3B , 0x34 , 0x04 , 0xE2 , 0x76 , 0x8E , 0x6C , 0xAD , 
    0xA5 , 0x0C , 0x6B , 0xBC , 0x6B , 0x19 , 0x84 , 0xEA , 0x89 , 0xB6 , 0x5D , 0x12 , 0x68 , 0xBB , 
    0xA9 , 0xA4 , 0x08 , 0xAF , 0x17 , 0x4F , 0xD9 , 0x88 , 0xA6 , 0x50 , 0xC9 , 0x4D , 0x9A , 0x37 , 
    0xD0 , 0xBE , 0x02 , 0x8C , 0xC7 , 0xEF , 0xF2 , 0xB6 , 0x7B , 0xAB , 0x1C , 0x5E , 0xBE , 0x6F , 
    0x36 , 0xC5 , 0xBC , 0xA8 , 0x94 , 0xFB , 0x23 , 0x08 , 0x0E , 0x8F , 0xAF , 0xBC , 0x02 , 0x8C , 
    0x9B , 0x66 , 0x05 , 0x84 , 0x80 , 0x18 , 0x9F , 0x86 , 0xB3 , 0x98 , 0x41 , 0x4C , 0xCA , 0x2F , 
    0xD2 , 0x52 , 0xD9 , 0xAC , 0xCF , 0xD6 , 0xD8 , 0x58 , 0x32 , 0xD2 , 0xD8 , 0x04 , 0xE0 , 0x84 , 
    0x9B , 0x82 , 0xA9 , 0xB4 , 0xB6 , 0x22 , 0x22 , 0x35 , 0xE6 , 0x97 , 0x53 , 0xF0 , 0xAE , 0xE5 , 
    0xC5 , 0xC1 , 0xFD , 0x23 , 0x3D , 0x08 , 0xE6 , 0xD8 , 0xD4 , 0xE0 , 0x47 , 0xEE , 0x70 , 0x0C , 
    0x1C , 0x00 , 0xFA , 0x81 , 0xBC , 0x09 , 0x43 , 0xD0 , 0x50 , 0x70 , 0x2F , 0x36 , 0x8A , 0x13 , 
    0xBA , 0xCC , 0xFF , 0xC0 , 0xAD , 0x9C , 0x00 , 0x03 , 0x42 , 0x86 , 0xA1 , 0x81 , 0x20 , 0xC9 , 
    0x00 , 0x90 , 0x03 , 0xE6 , 0x7E , 0x00 , 0x00 , 0x00 , 0x34 , 0x8A , 0x18 , 0x04 , 0xA1 , 0xC0 , 
    0x00 , 0x00 , 0x01 , 0x02 , 0xA7 , 0x26 , 0x00 , 0x02 , 0x42 , 0x53 , 0x66 , 0x10 , 0x08 , 0x80 , 
    0x17 , 0x01 , 0xA0 , 0x44};

/*===========================================================================

                                Functions

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_get_controller_state

===========================================================================*/
/*!
    @brief
    This function returns the controller state for test purposes

    @return
    E_SUCCESS if success, else other codes

 */
/*=========================================================================*/
stm_state_t lte_rrc_config_test_get_controller_state
(
  void
)
{
  return LTE_RRC_CONTROLLER_SM__CONNECTED;
}

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_csp_get_serving_cell_params

===========================================================================*/
/*!
    @brief
    This function returns current cell ID and frequency

    @return
    E_SUCCESS if success, else other codes

 */
/*=========================================================================*/
lte_errno_e lte_rrc_config_test_csp_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr /*!< Pointer to cell info */
)
{
  /* Just hard code to something for now*/
  cell_info_ptr->cell_id.freq = 10689;
  cell_info_ptr->cell_id.phy_cell_id = 511;
  cell_info_ptr->sib_db_key = 2;

  return E_SUCCESS;
}

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_csp_get_camped_cell_bw

===========================================================================*/
/*!
    @brief
    This function simulates the CSP API that returns the camped cell bandwidth. 
    The function returns the number of active EPS bearers.

    @return
    lte_errno_e
*/
/*=========================================================================*/
lte_errno_e lte_rrc_config_test_csp_get_camped_cell_bw
(
  lte_bandwidth_e *bw_ptr /*!< Pointer to BW to be filled in */
)
{
  lte_errno_e status = E_SUCCESS;
  *bw_ptr = LTE_BW_NRB_15;
  return status;
}

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_is_sec_activated

===========================================================================*/
/*!
    @brief
    This function simulates the security module API that returns a boolean 
    indicating if security is activated.

    @return
    TRUE, to imply security has been activated

 */
/*=========================================================================*/
boolean lte_rrc_config_test_is_sec_activated
(
  void
)
{
  return TRUE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_sec_validate_config

===========================================================================*/
/*!
    @brief
    This function simulates the security module API that validates the security
    configuration that comes during HO and returns a boolean indicating if 
    security config passed validation.

    @return
    TRUE, to imply security has been activated

 */
/*=========================================================================*/
lte_errno_e lte_rrc_config_test_sec_validate_config
(
  lte_rrc_osys_RRCConnectionReconfiguration *dl_dcch_ptr  /*! Pointer to DL DCCH msg */
)
{
  return E_SUCCESS;
}

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_mh_get_last_encoded_message_rcvd

===========================================================================*/
/*!
    @brief
    This function allows access to the last encoded message recieved on the
    downlink by the MH module

    @return
    void

 */
/*=========================================================================*/

void lte_rrc_config_test_mh_get_last_encoded_message_rcvd
(
  lte_rrc_pdu_buf_s *pdu_buf  /*! PDU buffer */
)
{
  *pdu_buf = lte_rrc_config_test_encoded_pdu;
}

/*===========================================================================

  FUNCTION:  lte_rrc_config_meas_cfg_test_validate_meas_param

===========================================================================*/
/*!
    @brief
    This function simulates the Measurement procedure API that validates
    the connected mode measurement parameters

    @return
    lte_errno_e
*/
/*=========================================================================*/
lte_errno_e lte_rrc_config_meas_cfg_test_validate_meas_param
(
  /*! Pointer to decoded ASN.1 DL DCCH Message */
  lte_rrc_osys_RRCConnectionReconfiguration *dl_dcch_msg_ptr,
  /*! TRUE if Reconfig DLM also includes MobilityControlInformation IE */
  boolean is_part_of_handover
)
{
  return E_SUCCESS;
} /* lte_rrc_config_meas_cfg_test_validate_meas_param() */

/*===========================================================================

  FUNCTION:  lte_rrc_config_meas_test_is_sec_activated

===========================================================================*/
/*!
    @brief
    This function returns simulates the security module API that returns
    if security is activated.

    @return
    FALSE, to imply security has not been activated

 */
/*=========================================================================*/
boolean lte_rrc_config_meas_test_is_sec_activated
(
  void
)
{
  return FALSE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_config_prox_cfg_allowed

===========================================================================*/
/*!
    @brief
    This function returns simulates the CSG module API that 
    returns TRUE if prox cfg is allowed

    @return
    TRUE

 */
/*=========================================================================*/
boolean lte_rrc_config_prox_cfg_allowed
(
  void
)
{
  return TRUE;
}

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_get_cre_state

===========================================================================*/
/*!
    @brief
    This function simulates the CRE API for returning the state of CRE SM.

    @return
    stm_state_t

 */
/*=========================================================================*/
stm_state_t lte_rrc_config_test_get_cre_state
(
  void
)
{
  return LTE_RRC_CRE_SM__INITIAL;
}

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_validate_mobility_from_eutra_msg

===========================================================================*/
/*!
    @brief
    This function simulates the IRAT utils API that validates Mobility from EUTRA 
    message.

    @return
    lte_errno_e

 */
/*=========================================================================*/
lte_errno_e lte_rrc_config_test_validate_mobility_from_eutra_msg
(
  lte_rrc_osys_DL_DCCH_Message  *msg_ptr
)
{
  return E_SUCCESS;
}

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_csp_wcdma_is_supported

===========================================================================*/
/*!
    @brief
    This function simulates the CSP API.

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_config_test_csp_wcdma_is_supported
(
  void
)
{
  return TRUE;

} /* lte_rrc_config_test_csp_wcdma_is_supported */
/*===========================================================================

  FUNCTION:  lte_rrc_config_test_csp_tdscdma_is_supported

===========================================================================*/
/*!
    @brief
    This function simulates the CSP API.

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_config_test_csp_tdscdma_is_supported
(
  void
)
{
  return FALSE;

} /* lte_rrc_config_test_csp_tdscdma_is_supported */
/*===========================================================================

  FUNCTION:  lte_rrc_config_test_csp_gsm_is_supported

===========================================================================*/
/*!
    @brief
    This function simulates the CSP API.

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_config_test_csp_gsm_is_supported
(
  void
)
{
  return TRUE;

} /* lte_rrc_config_test_csp_gsm_is_supported */

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_csp_cdma_1x_is_supported

===========================================================================*/
/*!
    @brief
    This function simulates the CSP API.

    @return
    boolean
*/
/*=========================================================================*/
boolean lte_rrc_config_test_csp_cdma_1x_is_supported
(
  void
)
{
  return TRUE;

} /* lte_rrc_config_test_csp_cdma_1x_is_supported */

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_cap_get_fgi_bit_1

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_config_test_cap_get_fgi_bit_1
(
  uint8 bit_pos
)
{
  return 1;

} /* lte_rrc_config_test_cap_get_fgi_bit_1() */

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_cap_get_fgi_bit_0

===========================================================================*/
/*!
    @brief
    This function simulates the capabilities API.

    @return
    boolean
*/
/*=========================================================================*/
uint8 lte_rrc_config_test_cap_get_fgi_bit_0
(
  uint8 bit_pos
)
{
  return 0;

} /* lte_rrc_config_test_cap_get_fgi_bit_0() */

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_get_3gpp_rel_version_for_rel8

===========================================================================*/
/*!
    @brief
    This function simulates the Utils API.

    @return
    boolean
*/
/*=========================================================================*/
lte_3gpp_release_ver_e lte_rrc_config_test_get_3gpp_rel_version_for_rel8
(
  void
)
{
  return LTE_3GPP_REL8_MAR09;

} /* lte_rrc_config_test_get_3gpp_rel_version_for_rel8 */

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_get_3gpp_rel_version_for_rel9

===========================================================================*/
/*!
    @brief
    This function simulates the Utils API.

    @return
    boolean
*/
/*=========================================================================*/
lte_3gpp_release_ver_e lte_rrc_config_test_get_3gpp_rel_version_for_rel9
(
  void
)
{
  return LTE_3GPP_REL9_JUN10;

} /* lte_rrc_config_test_get_3gpp_rel_version_for_rel9 */

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_get_est_cause_mos

===========================================================================*/
/*!
    @brief
    This function simulates the get est cause function of CEP.

    @return
    boolean
*/
/*=========================================================================*/
lte_rrc_est_cause_e lte_rrc_config_test_get_est_cause_mos
(
  void
)
{
  return LTE_RRC_EST_CAUSE_MO_SIGNALING;

} /* lte_rrc_config_test_get_est_cause_mos */

/*===========================================================================

  FUNCTION:  lte_rrc_config_test_get_est_cause_mod

===========================================================================*/
/*!
    @brief
    This function simulates the get est cause function of CEP.

    @return
    boolean
*/
/*=========================================================================*/
lte_rrc_est_cause_e lte_rrc_config_test_get_est_cause_mod
(
  void
)
{
  return LTE_RRC_EST_CAUSE_MO_DATA;

} /* lte_rrc_config_test_get_est_cause_mod */

/*===========================================================================

   FUNCTION:  lte_rrc_config_test_stub_init

===========================================================================*/
/*!
    @brief
    Initializes the stub required for llc unit test 

    @return
    None

*/
/*=========================================================================*/
void lte_rrc_config_test_stub_init
( 
  void 
)
{
  TF_STUB(lte_rrc_controller_get_state) = lte_rrc_config_test_get_controller_state;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) = lte_rrc_config_test_csp_get_serving_cell_params;
  TF_STUB(lte_rrc_csp_get_camped_cell_bw) = lte_rrc_config_test_csp_get_camped_cell_bw;
  TF_STUB(lte_rrc_sec_security_is_configured) = lte_rrc_config_test_is_sec_activated;
  TF_STUB(lte_rrc_meas_validate_connected_config) = lte_rrc_config_meas_cfg_test_validate_meas_param;
  TF_STUB(lte_rrc_cre_get_state) = lte_rrc_config_test_get_cre_state;
 // TF_STUB(lte_rrc_irat_validate_mobility_from_eutra_msg) = lte_rrc_config_test_validate_mobility_from_eutra_msg;
  TF_STUB(lte_rrc_mh_get_last_encoded_message_rcvd) = lte_rrc_config_test_mh_get_last_encoded_message_rcvd;
  TF_STUB(lte_rrc_sec_validate_config) = lte_rrc_config_test_sec_validate_config;
 // TF_STUB(lte_rrc_csp_cap_wcdma_is_supported) = lte_rrc_config_test_csp_wcdma_is_supported;
  //TF_STUB(lte_rrc_csp_cap_tdscdma_is_supported) = lte_rrc_config_test_csp_tdscdma_is_supported;
 // TF_STUB(lte_rrc_csp_gsm_is_supported) = lte_rrc_config_test_csp_gsm_is_supported;
  //TF_STUB(lte_rrc_csp_cdma_1x_is_supported) = lte_rrc_config_test_csp_cdma_1x_is_supported;
  TF_STUB(lte_rrc_cap_get_fgi_bit) = lte_rrc_config_test_cap_get_fgi_bit_1;
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_config_test_get_3gpp_rel_version_for_rel8;
  TF_STUB(lte_rrc_cep_get_est_cause) = lte_rrc_config_test_get_est_cause_mod;

}/*lte_rrc_config_test_stub_init*/

