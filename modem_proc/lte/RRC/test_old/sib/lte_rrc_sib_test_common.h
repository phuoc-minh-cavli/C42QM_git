/*!
  @file
  lte_rrc_sib_test_common.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sib/lte_rrc_sib_test_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef LTE_RRC_SIB_TEST_COMMON_H
#define LTE_RRC_SIB_TEST_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "lte_rrc_osys_asn1util.h"

#define PHY_CELL_ID_1 21
#define        FREQ_1 2000
#define    FREQ_TDD_1 36000
#define PHY_CELL_ID_2 35
#define        FREQ_2 2001
#define    FREQ_TDD_2 36001


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

extern int lte_rrc_sib_test_sib_db_key;

//static boolean lte_rrc_sib_test_camped;
/*
SIB1_mcc_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/
#define DECLARE_SIB1_MCC_8_5 \
static unsigned char sib1_mcc_8_5[15] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x01, 0x01, 0x10}

/*
SIB1_2345_mcc_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType4,
              sibType5
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/
#define DECLARE_SIB1_2345_MCC_8_5 \
static unsigned char sib1_2345_mcc_8_5[18] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x09, 0x08, 0x00, 0x82, 0x22, 0x20}

#define DECLARE_SIB1_2345_MCC_8_5_V1 \
static unsigned char sib1_2345_mcc_8_5_v1[19] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x11, 0x08, 0x00, 0x42, 0x82, 0x22, 0x20}

#define DECLARE_SIB1_2310_45_11_MCC_8_5 \
static unsigned char sib1_2310_45_11_mcc_8_5[20] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x10, 0x10, 0x0E, 0xC4, 0x11, 0x00, 0xA0, 0x88,}

#define DECLARE_SIB1_2310_45_11_MCC_8_5_V1 \
static unsigned char sib1_2310_45_11_mcc_8_5_v1[20] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x10, 0x10, 0x0E, 0xC4, 0x11, 0x00, 0xA0, 0x8C,}

/*
DECLARE_SIB1_2345101112_MCC_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType4
            }
          },
          {
            si-Periodicity rf32,
            sib-MappingInfo 
            {
              sibType5
            }
          },
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType10
            }
          },
          {
            si-Periodicity rf32,
            sib-MappingInfo 
            {
              sibType11,
              sibType12-v920
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/
#define DECLARE_SIB1_2345101112_MCC_8_5 \
static unsigned char sib1_2345101112_mcc_8_5[23] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x21, 0x08, 0x00, 0x42, 0x82, 0x22, 0x13, 0xA1, 0x21, 0x24, 0x40}

#define DECLARE_SIB1_234510_MCC_8_5 \
static unsigned char sib1_234510_mcc_8_5[19] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x09, 0x10, 0x0E, 0x04, 0x11, 0x11, 0x00}

#define DECLARE_SIB2_SIB3_SIB10_8_5 \
static unsigned char sib2_sib3_sib10_8_5[36] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE1, 0xB0, 0x00, 0x80, 0x00, 0x00, 0x00, 0x10,\
 0x00, 0x01, 0x40, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define DECLARE_SIB1_234513_MCC_8_5 \
static unsigned char sib1_234513_mcc_8_5[20] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x11, 0x08, 0x00, 0x82, 0x24, 0x15, 0x11, 0x00}

#define DECLARE_SIB1_23451315_MCC_8_5 \
static unsigned char sib1_23451315_mcc_8_5[20] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x11, 0x10, 0x18, 0x04, 0x11, 0x20, 0xA8, 0x88}

#define DECLARE_SIB1_2345_MCC_8_5_pad_bytes \
static unsigned char sib1_2345_mcc_8_5_pad_bytes[22] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x09, 0x08, 0x00, 0x82, 0x22, 0x20, 0x01, 0x02, 0xFF, 0xAC}

/* Periodicity of first SI changed to 32 in sib1_2345_mcc_8_5 */
#define DECLARE_SIB1_2345_MCC_8_5_SIB2_PER_32 \
static unsigned char sib1_2345_mcc_8_5_sib2_per_32[18] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x0A, 0x08, 0x00, 0x82, 0x22, 0x20}

/* FreqBandIndicator set to 33 in sib1_2345_mcc_8_5 */
#define DECLARE_SIB1_2345_TDD_INVALID_MCC_8_5 \
static unsigned char sib1_2345_tdd_invalid_mcc_8_5[] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0xA0, 0x09, 0x08, 0x00, 0x82, 0x22, 0x20}

/*
SIB1_2345_mcc_8_5_value_tag_change:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType4,
              sibType5
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 3
      }
}
*/
#define DECLARE_SIB1_2345_MCC_8_5_VALUE_TAG_CHANGE \
static unsigned char sib1_2345_mcc_8_5_value_tag_change[] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x09, 0x08, 0x00, 0x82, 0x22, 0x30}

#define DECLARE_SIB1_2345_MCC_8_5_VALUE_TAG_CHANGE_PAD_BYTES \
static unsigned char sib1_2345_mcc_8_5_value_tag_change_pad_bytes[] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x09, 0x08, 0x00, 0x82, 0x22, 0x30, 0x00,0x01, 0x02}

/*
SIB1_2345_mcc_8_5_value_tag_match_sib1_differ:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType4,
              sibType5
            }
          }
        },
        si-WindowLength ms10,
        systemInfoValueTag 2
      }
}
*/
#define DECLARE_SIB1_2345_MCC_8_5_VALUE_TAG_MATCH_SIB1_DIFFER \
static unsigned char sib1_2345_mcc_8_5_value_tag_match_sib1_differ[] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x09, 0x08, 0x00, 0x82, 0x26, 0x20}

/*
SIB1_valueTagChange_mcc_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType3
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 4
      }
}
*/
#define DECLARE_SIB1_VALUETAGCHANGE_MCC_8_5 \
static unsigned char sib1_valueTagChange_mcc_8_5[16] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x01, 0x08, 0x09, 0x00}

/*
sib1_mem_test_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  0,
                  0,
                  1
                },
                mnc 
                {
                  0,
                  1
                }
              },
              cellReservedForOperatorUse notReserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000000 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication FALSE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -66,
          q-RxLevMinOffset 1
        },
        freqBandIndicator 13,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf32,
            sib-MappingInfo 
            {
              sibType4
            }
          }
        },
        si-WindowLength ms20,
        systemInfoValueTag 0
      }
}
*/
#define DECLARE_SIB1_MEM_TEST_8_5 \
static unsigned char sib1_mem_test_8_5[] = {0x40, 0x40, 0x04, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x10, 0x18, 0x12, 0x10, 0x20, 0x86, 0x80}

/*
SIB2_8_5:value BCCH-DL-SCH-Message ::= 
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
                        numberOfRA-Preambles n4
                      },
                      powerRampingParameters 
                      {
                        powerRampingStep dB0,
                        preambleInitialReceivedTargetPower dBm-120
                      },
                      ra-SupervisionInfo 
                      {
                        preambleTransMax n3,
                        ra-ResponseWindowSize sf2,
                        mac-ContentionResolutionTimer sf8
                      },
                      maxHARQ-Msg3Tx 2
                    },
                    bcch-Config 
                    {
                      modificationPeriodCoeff n2
                    },
                    pcch-Config 
                    {
                      defaultPagingCycle rf32,
                      nB fourT
                    },
                    prach-Config 
                    {
                      rootSequenceIndex 0,
                      prach-ConfigInfo 
                      {
                        prach-ConfigIndex 0,
                        highSpeedFlag FALSE,
                        zeroCorrelationZoneConfig 0,
                        prach-FreqOffset 0
                      }
                    },
                    pdsch-Config 
                    {
                      referenceSignalPower 0,
                      p-b 0
                    },
                    pusch-Config 
                    {
                      pusch-ConfigBasic 
                      {
                        n-SB 1,
                        hoppingMode interSubFrame,
                        pusch-HoppingOffset 0,
                        enable64QAM FALSE
                      },
                      ul-ReferenceSignalsPUSCH 
                      {
                        groupHoppingEnabled FALSE,
                        groupAssignmentPUSCH 0,
                        sequenceHoppingEnabled FALSE,
                        cyclicShift 0
                      }
                    },
                    pucch-Config 
                    {
                      deltaPUCCH-Shift ds1,
                      nRB-CQI 0,
                      nCS-AN 0,
                      n1PUCCH-AN 0
                    },
                    soundingRS-UL-Config release : NULL,
                    uplinkPowerControl 
                    {
                      p0-NominalPUSCH 0,
                      alpha al0,
                      p0-NominalPUCCH -100,
                      deltaFList-PUCCH 
                      {
                        deltaF-PUCCH-Format1 deltaF-2,
                        deltaF-PUCCH-Format1b deltaF1,
                        deltaF-PUCCH-Format2 deltaF-2,
                        deltaF-PUCCH-Format2a deltaF-2,
                        deltaF-PUCCH-Format2b deltaF-2
                      },
                      deltaPreambleMsg3 0
                    },
                    ul-CyclicPrefixLength len1
                  },
                  ue-TimersAndConstants 
                  {
                    t300 ms100,
                    t301 ms100,
                    t310 ms0,
                    n310 n1,
                    t311 ms1000,
                    n311 n1
                  },
                  freqInfo 
                  {
                    additionalSpectrumEmission 1
                  },
                  timeAlignmentTimerCommon sf500
                }
            }
          }
      }
}
*/
#define DECLARE_SIB2_8_5 \
static unsigned char sib2_8_5[24] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE1, 0xB0, 0x00, 0x80, 0x00, 0x00, 0x00}

/*
SIB2_SIB3_8_5:value BCCH-DL-SCH-Message ::= 
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
                        numberOfRA-Preambles n4
                      },
                      powerRampingParameters 
                      {
                        powerRampingStep dB0,
                        preambleInitialReceivedTargetPower dBm-120
                      },
                      ra-SupervisionInfo 
                      {
                        preambleTransMax n3,
                        ra-ResponseWindowSize sf2,
                        mac-ContentionResolutionTimer sf8
                      },
                      maxHARQ-Msg3Tx 2
                    },
                    bcch-Config 
                    {
                      modificationPeriodCoeff n2
                    },
                    pcch-Config 
                    {
                      defaultPagingCycle rf32,
                      nB fourT
                    },
                    prach-Config 
                    {
                      rootSequenceIndex 0,
                      prach-ConfigInfo 
                      {
                        prach-ConfigIndex 0,
                        highSpeedFlag FALSE,
                        zeroCorrelationZoneConfig 0,
                        prach-FreqOffset 0
                      }
                    },
                    pdsch-Config 
                    {
                      referenceSignalPower 0,
                      p-b 0
                    },
                    pusch-Config 
                    {
                      pusch-ConfigBasic 
                      {
                        n-SB 1,
                        hoppingMode interSubFrame,
                        pusch-HoppingOffset 0,
                        enable64QAM FALSE
                      },
                      ul-ReferenceSignalsPUSCH 
                      {
                        groupHoppingEnabled FALSE,
                        groupAssignmentPUSCH 0,
                        sequenceHoppingEnabled FALSE,
                        cyclicShift 0
                      }
                    },
                    pucch-Config 
                    {
                      deltaPUCCH-Shift ds1,
                      nRB-CQI 0,
                      nCS-AN 0,
                      n1PUCCH-AN 0
                    },
                    soundingRS-UL-Config release : NULL,
                    uplinkPowerControl 
                    {
                      p0-NominalPUSCH 0,
                      alpha al0,
                      p0-NominalPUCCH -100,
                      deltaFList-PUCCH 
                      {
                        deltaF-PUCCH-Format1 deltaF-2,
                        deltaF-PUCCH-Format1b deltaF1,
                        deltaF-PUCCH-Format2 deltaF-2,
                        deltaF-PUCCH-Format2a deltaF-2,
                        deltaF-PUCCH-Format2b deltaF-2
                      },
                      deltaPreambleMsg3 0
                    },
                    ul-CyclicPrefixLength len1
                  },
                  ue-TimersAndConstants 
                  {
                    t300 ms100,
                    t301 ms100,
                    t310 ms0,
                    n310 n1,
                    t311 ms1000,
                    n311 n1
                  },
                  freqInfo 
                  {
                    additionalSpectrumEmission 1
                  },
                  timeAlignmentTimerCommon sf500
                },
              sib3 : 
                {
                  cellReselectionInfoCommon 
                  {
                    q-Hyst dB0
                  },
                  cellReselectionServingFreqInfo 
                  {
                    threshServingLow 0,
                    cellReselectionPriority 0
                  },
                  intraFreqCellReselectionInfo 
                  {
                    q-RxLevMin -30,
                    presenceAntennaPort1 FALSE,
                    neighCellConfig '00'B,
                    t-ReselectionEUTRA 0
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_SIB2_SIB3_8_5 \
static unsigned char sib2_sib3_8_5[] = {0x00, 0x80, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE1, 0xB0, 0x00, 0x80, 0x00, 0x00, 0x00, 0x10, 0x00, 0x01, 0x40, 0x00}

/*
#define DECLARE_SIB2_SIB3_SIB10_8_5 \
static unsigned char sib2_sib3_sib10_8_5[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe1, 0xb0, 0x00, 0x80, 0x00, 0x00, 0x00, 0x10, \
0x00, 0x01, 0x40, 0x08, 0x02, 0xc0, 0x00, 0x40, 0x00, 0x00, 0x00}
*/

/*
SIB2_SIB3_diff_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib2 : 
                {
                  ac-BarringInfo 
                  {
                    ac-BarringForEmergency FALSE
                  },
                  radioResourceConfigCommon 
                  {
                    rach-Config 
                    {
                      preambleInfo 
                      {
                        numberOfRA-Preambles n4
                      },
                      powerRampingParameters 
                      {
                        powerRampingStep dB0,
                        preambleInitialReceivedTargetPower dBm-120
                      },
                      ra-SupervisionInfo 
                      {
                        preambleTransMax n3,
                        ra-ResponseWindowSize sf2,
                        mac-ContentionResolutionTimer sf8
                      },
                      maxHARQ-Msg3Tx 2
                    },
                    bcch-Config 
                    {
                      modificationPeriodCoeff n2
                    },
                    pcch-Config 
                    {
                      defaultPagingCycle rf32,
                      nB fourT
                    },
                    prach-Config 
                    {
                      rootSequenceIndex 0,
                      prach-ConfigInfo 
                      {
                        prach-ConfigIndex 0,
                        highSpeedFlag FALSE,
                        zeroCorrelationZoneConfig 0,
                        prach-FreqOffset 0
                      }
                    },
                    pdsch-Config 
                    {
                      referenceSignalPower 0,
                      p-b 0
                    },
                    pusch-Config 
                    {
                      pusch-ConfigBasic 
                      {
                        n-SB 1,
                        hoppingMode interSubFrame,
                        pusch-HoppingOffset 0,
                        enable64QAM FALSE
                      },
                      ul-ReferenceSignalsPUSCH 
                      {
                        groupHoppingEnabled FALSE,
                        groupAssignmentPUSCH 0,
                        sequenceHoppingEnabled FALSE,
                        cyclicShift 0
                      }
                    },
                    pucch-Config 
                    {
                      deltaPUCCH-Shift ds1,
                      nRB-CQI 0,
                      nCS-AN 0,
                      n1PUCCH-AN 0
                    },
                    soundingRS-UL-Config release : NULL,
                    uplinkPowerControl 
                    {
                      p0-NominalPUSCH 0,
                      alpha al0,
                      p0-NominalPUCCH -100,
                      deltaFList-PUCCH 
                      {
                        deltaF-PUCCH-Format1 deltaF-2,
                        deltaF-PUCCH-Format1b deltaF1,
                        deltaF-PUCCH-Format2 deltaF-2,
                        deltaF-PUCCH-Format2a deltaF-2,
                        deltaF-PUCCH-Format2b deltaF-2
                      },
                      deltaPreambleMsg3 0
                    },
                    ul-CyclicPrefixLength len1
                  },
                  ue-TimersAndConstants 
                  {
                    t300 ms100,
                    t301 ms100,
                    t310 ms0,
                    n310 n1,
                    t311 ms1000,
                    n311 n1
                  },
                  freqInfo 
                  {
                    additionalSpectrumEmission 1
                  },
                  timeAlignmentTimerCommon sf500
                },
              sib3 : 
                {
                  cellReselectionInfoCommon 
                  {
                    q-Hyst dB0
                  },
                  cellReselectionServingFreqInfo 
                  {
                    threshServingLow 0,
                    cellReselectionPriority 0
                  },
                  intraFreqCellReselectionInfo 
                  {
                    q-RxLevMin -30,
                    presenceAntennaPort1 FALSE,
                    neighCellConfig '00'B,
                    t-ReselectionEUTRA 0
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_SIB2_SIB3_DIFF_8_5 \
static unsigned char sib2_sib3_diff_8_5[] = {0x00, 0x81, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x36, 0x00, 0x10, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x28, 0x00}

/* Paging changed to 64 in sib2_sib3_8_5 */
#define DECLARE_SIB2_SIB3_8_5_MOD_PRD_128 \
static unsigned char sib2_sib3_8_5_mod_prd_128[] = {0x00, 0x80, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE1, 0xB0, 0x00, 0x80, 0x00, 0x00, 0x00, 0x10, 0x00, 0x01, 0x40, 0x00}

/*
SIB4_SIB5_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib4 : 
                {
                },
              sib5 : 
                {
                  interFreqCarrierFreqList 
                  {
                    {
                      dl-CarrierFreq 0,
                      q-RxLevMin -30,
                      t-ReselectionEUTRA 0,
                      threshX-High 0,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw6,
                      presenceAntennaPort1 FALSE,
                      neighCellConfig '00'B
                    }
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_SIB4_SIB5_8_5 \
static unsigned char sib4_sib5_8_5[] = {0x00, 0x88, 0x06, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00}

#define DECLARE_SIB5_8_5 \
static unsigned char sib5_8_5[]={0x00, 0x0C, 0x02, 0x00, 0x00, 0x50, 0x00, 0x00, 0x1E}
/*
SIB1_2345_mcc_8_5_cell2:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  4,
                  5,
                  6
                },
                mnc 
                {
                  6,
                  7,
                  8
                }
              },
              cellReservedForOperatorUse reserved
            },
            {
              plmn-Identity 
              {
                mnc 
                {
                  1,
                  0,
                  1
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000001 00000000'B,
          cellIdentity '00010001 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType4,
              sibType5
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 3
      }
}
*/
#define DECLARE_SIB1_2345_MCC_8_5_CELL2 \
static unsigned char sib1_2345_mcc_8_5_cell2[] = {0x40, 0xD1, 0x5A, 0xCF, 0x04, 0x40, 0x40, 0x20, 0x02, 0x20, 0x00, 0x01, 0x45, 0x06, 0x12, 0x10, 0x01, 0x04, 0x44, 0x60}

/* Same as sib1_2345_mcc_8_5_cell2 with si_windowLength 5 */
#define DECLARE_SIB1_2345_MCC_8_5_CELL2_VALUE_TAG_MATCH_SIB1_DIFFER \
static unsigned char sib1_2345_mcc_8_5_cell2_value_tag_match_sib1_differ[] = {0x40, 0xD1, 0x5A, 0xCF, 0x04, 0x40, 0x40, 0x20, 0x02, 0x20, 0x00, 0x01, 0x45, 0x06, 0x12, 0x10, 0x01, 0x04, 0x48, 0x60}

/* Same as sib1_2345_mcc_8_5_cell2 with value tag 4 */
#define DECLARE_SIB1_2345_MCC_8_5_CELL2_VALUE_TAG_CHANGES \
static unsigned char sib1_2345_mcc_8_5_cell2_value_tag_changes[] = {0x40, 0xD1, 0x5A, 0xCF, 0x04, 0x40, 0x40, 0x20, 0x02, 0x20, 0x00, 0x01, 0x45, 0x06, 0x12, 0x10, 0x01, 0x04, 0x44, 0x80}

/*
SIB1_2_3_4_mcc_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType4
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/
#define DECLARE_SIB1_2_3_4_MCC_8_5 \
static unsigned char sib1_2_3_4_mcc_8_5[] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x10, 0x00, 0x08, 0x00, 0x42, 0x44}

/*
SIB3_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib3 : 
                {
                  cellReselectionInfoCommon 
                  {
                    q-Hyst dB0
                  },
                  cellReselectionServingFreqInfo 
                  {
                    threshServingLow 0,
                    cellReselectionPriority 0
                  },
                  intraFreqCellReselectionInfo 
                  {
                    q-RxLevMin -30,
                    presenceAntennaPort1 FALSE,
                    neighCellConfig '00'B,
                    t-ReselectionEUTRA 0
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_SIB3_8_5 \
static unsigned char sib3_8_5[] = {0x00, 0x04, 0x00, 0x00, 0x50, 0x00}

/*
SIB4_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib4 : 
                {
                }
            }
          }
      }
}
*/
#define DECLARE_SIB4_8_5 \
static unsigned char sib4_8_5[] = {0x00, 0x08, 0x00}

/*
SIB2_SIB3_mod_prd_change_8_5:value BCCH-DL-SCH-Message ::= 
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
                        numberOfRA-Preambles n4
                      },
                      powerRampingParameters 
                      {
                        powerRampingStep dB0,
                        preambleInitialReceivedTargetPower dBm-120
                      },
                      ra-SupervisionInfo 
                      {
                        preambleTransMax n3,
                        ra-ResponseWindowSize sf2,
                        mac-ContentionResolutionTimer sf8
                      },
                      maxHARQ-Msg3Tx 2
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
                      rootSequenceIndex 0,
                      prach-ConfigInfo 
                      {
                        prach-ConfigIndex 0,
                        highSpeedFlag FALSE,
                        zeroCorrelationZoneConfig 0,
                        prach-FreqOffset 0
                      }
                    },
                    pdsch-Config 
                    {
                      referenceSignalPower 0,
                      p-b 0
                    },
                    pusch-Config 
                    {
                      pusch-ConfigBasic 
                      {
                        n-SB 1,
                        hoppingMode interSubFrame,
                        pusch-HoppingOffset 0,
                        enable64QAM FALSE
                      },
                      ul-ReferenceSignalsPUSCH 
                      {
                        groupHoppingEnabled FALSE,
                        groupAssignmentPUSCH 0,
                        sequenceHoppingEnabled FALSE,
                        cyclicShift 0
                      }
                    },
                    pucch-Config 
                    {
                      deltaPUCCH-Shift ds1,
                      nRB-CQI 0,
                      nCS-AN 0,
                      n1PUCCH-AN 0
                    },
                    soundingRS-UL-Config release : NULL,
                    uplinkPowerControl 
                    {
                      p0-NominalPUSCH 0,
                      alpha al0,
                      p0-NominalPUCCH -100,
                      deltaFList-PUCCH 
                      {
                        deltaF-PUCCH-Format1 deltaF-2,
                        deltaF-PUCCH-Format1b deltaF1,
                        deltaF-PUCCH-Format2 deltaF-2,
                        deltaF-PUCCH-Format2a deltaF-2,
                        deltaF-PUCCH-Format2b deltaF-2
                      },
                      deltaPreambleMsg3 0
                    },
                    ul-CyclicPrefixLength len1
                  },
                  ue-TimersAndConstants 
                  {
                    t300 ms100,
                    t301 ms100,
                    t310 ms0,
                    n310 n1,
                    t311 ms1000,
                    n311 n1
                  },
                  freqInfo 
                  {
                    additionalSpectrumEmission 1
                  },
                  timeAlignmentTimerCommon sf500
                },
              sib3 : 
                {
                  cellReselectionInfoCommon 
                  {
                    q-Hyst dB0
                  },
                  cellReselectionServingFreqInfo 
                  {
                    threshServingLow 0,
                    cellReselectionPriority 0
                  },
                  intraFreqCellReselectionInfo 
                  {
                    q-RxLevMin -30,
                    presenceAntennaPort1 FALSE,
                    neighCellConfig '00'B,
                    t-ReselectionEUTRA 0
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_SIB2_SIB3_MOD_PRD_CHANGE_8_5 \
static unsigned char sib2_sib3_mod_prd_change_8_5[] = {0x00, 0x80, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE1, 0xB0, 0x00, 0x80, 0x00, 0x00, 0x00, 0x10, 0x00, 0x01, 0x40, 0x00}

/*
SIB2_SIB3_8_5_mod_prd_1024:value BCCH-DL-SCH-Message ::= 
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
                        numberOfRA-Preambles n4
                      },
                      powerRampingParameters 
                      {
                        powerRampingStep dB0,
                        preambleInitialReceivedTargetPower dBm-120
                      },
                      ra-SupervisionInfo 
                      {
                        preambleTransMax n3,
                        ra-ResponseWindowSize sf2,
                        mac-ContentionResolutionTimer sf8
                      },
                      maxHARQ-Msg3Tx 2
                    },
                    bcch-Config 
                    {
                      modificationPeriodCoeff n8
                    },
                    pcch-Config 
                    {
                      defaultPagingCycle rf128,
                      nB fourT
                    },
                    prach-Config 
                    {
                      rootSequenceIndex 0,
                      prach-ConfigInfo 
                      {
                        prach-ConfigIndex 0,
                        highSpeedFlag FALSE,
                        zeroCorrelationZoneConfig 0,
                        prach-FreqOffset 0
                      }
                    },
                    pdsch-Config 
                    {
                      referenceSignalPower 0,
                      p-b 0
                    },
                    pusch-Config 
                    {
                      pusch-ConfigBasic 
                      {
                        n-SB 1,
                        hoppingMode interSubFrame,
                        pusch-HoppingOffset 0,
                        enable64QAM FALSE
                      },
                      ul-ReferenceSignalsPUSCH 
                      {
                        groupHoppingEnabled FALSE,
                        groupAssignmentPUSCH 0,
                        sequenceHoppingEnabled FALSE,
                        cyclicShift 0
                      }
                    },
                    pucch-Config 
                    {
                      deltaPUCCH-Shift ds1,
                      nRB-CQI 0,
                      nCS-AN 0,
                      n1PUCCH-AN 0
                    },
                    soundingRS-UL-Config release : NULL,
                    uplinkPowerControl 
                    {
                      p0-NominalPUSCH 0,
                      alpha al0,
                      p0-NominalPUCCH -100,
                      deltaFList-PUCCH 
                      {
                        deltaF-PUCCH-Format1 deltaF-2,
                        deltaF-PUCCH-Format1b deltaF1,
                        deltaF-PUCCH-Format2 deltaF-2,
                        deltaF-PUCCH-Format2a deltaF-2,
                        deltaF-PUCCH-Format2b deltaF-2
                      },
                      deltaPreambleMsg3 0
                    },
                    ul-CyclicPrefixLength len1
                  },
                  ue-TimersAndConstants 
                  {
                    t300 ms100,
                    t301 ms100,
                    t310 ms0,
                    n310 n1,
                    t311 ms1000,
                    n311 n1
                  },
                  freqInfo 
                  {
                    additionalSpectrumEmission 1
                  },
                  timeAlignmentTimerCommon sf500
                },
              sib3 : 
                {
                  cellReselectionInfoCommon 
                  {
                    q-Hyst dB0
                  },
                  cellReselectionServingFreqInfo 
                  {
                    threshServingLow 0,
                    cellReselectionPriority 0
                  },
                  intraFreqCellReselectionInfo 
                  {
                    q-RxLevMin -30,
                    presenceAntennaPort1 FALSE,
                    neighCellConfig '00'B,
                    t-ReselectionEUTRA 0
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_SIB2_SIB3_8_5_MOD_PRD_1024 \
static unsigned char sib2_sib3_8_5_mod_prd_1024[] = {0x00, 0x80, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE1, 0xB0, 0x00, 0x80, 0x00, 0x00, 0x00, 0x10, 0x00, 0x01, 0x40, 0x00}

/* Paging changed to 64 in sib2_sib3_8_5_mod_prd_1024 */
#define DECLARE_SIB2_SIB3_8_5_MOD_PRD_512 \
static unsigned char sib2_sib3_8_5_mod_prd_512[] = {0x00, 0x80, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE1, 0xB0, 0x00, 0x80, 0x00, 0x00, 0x00, 0x10, 0x00, 0x01, 0x40, 0x00}

/*
sib2_8_5_mod_prd_1024:value BCCH-DL-SCH-Message ::= 
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
                        numberOfRA-Preambles n4
                      },
                      powerRampingParameters 
                      {
                        powerRampingStep dB0,
                        preambleInitialReceivedTargetPower dBm-120
                      },
                      ra-SupervisionInfo 
                      {
                        preambleTransMax n3,
                        ra-ResponseWindowSize sf2,
                        mac-ContentionResolutionTimer sf8
                      },
                      maxHARQ-Msg3Tx 2
                    },
                    bcch-Config 
                    {
                      modificationPeriodCoeff n8
                    },
                    pcch-Config 
                    {
                      defaultPagingCycle rf128,
                      nB fourT
                    },
                    prach-Config 
                    {
                      rootSequenceIndex 0,
                      prach-ConfigInfo 
                      {
                        prach-ConfigIndex 0,
                        highSpeedFlag FALSE,
                        zeroCorrelationZoneConfig 0,
                        prach-FreqOffset 0
                      }
                    },
                    pdsch-Config 
                    {
                      referenceSignalPower 0,
                      p-b 0
                    },
                    pusch-Config 
                    {
                      pusch-ConfigBasic 
                      {
                        n-SB 1,
                        hoppingMode interSubFrame,
                        pusch-HoppingOffset 0,
                        enable64QAM FALSE
                      },
                      ul-ReferenceSignalsPUSCH 
                      {
                        groupHoppingEnabled FALSE,
                        groupAssignmentPUSCH 0,
                        sequenceHoppingEnabled FALSE,
                        cyclicShift 0
                      }
                    },
                    pucch-Config 
                    {
                      deltaPUCCH-Shift ds1,
                      nRB-CQI 0,
                      nCS-AN 0,
                      n1PUCCH-AN 0
                    },
                    soundingRS-UL-Config release : NULL,
                    uplinkPowerControl 
                    {
                      p0-NominalPUSCH 0,
                      alpha al0,
                      p0-NominalPUCCH -100,
                      deltaFList-PUCCH 
                      {
                        deltaF-PUCCH-Format1 deltaF-2,
                        deltaF-PUCCH-Format1b deltaF1,
                        deltaF-PUCCH-Format2 deltaF-2,
                        deltaF-PUCCH-Format2a deltaF-2,
                        deltaF-PUCCH-Format2b deltaF-2
                      },
                      deltaPreambleMsg3 0
                    },
                    ul-CyclicPrefixLength len1
                  },
                  ue-TimersAndConstants 
                  {
                    t300 ms100,
                    t301 ms100,
                    t310 ms0,
                    n310 n1,
                    t311 ms1000,
                    n311 n1
                  },
                  freqInfo 
                  {
                    additionalSpectrumEmission 1
                  },
                  timeAlignmentTimerCommon sf500
                }
            }
          }
      }
}
*/
#define DECLARE_SIB2_8_5_MOD_PRD_1024 \
static unsigned char sib2_8_5_mod_prd_1024[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE1, 0xB0, 0x00, 0x80, 0x00, 0x00, 0x00}

/* Paging changed to 64 in sib2_8_5_mod_prd_1024 */
#define DECLARE_SIB2_8_5_MOD_PRD_512 \
static unsigned char sib2_8_5_mod_prd_512[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE1, 0xB0, 0x00, 0x80, 0x00, 0x00, 0x00}

/* Paging changed to 256 in sib2_8_5_mod_prd_1024 */
#define DECLARE_SIB2_8_5_MOD_PRD_2048 \
static unsigned char sib2_8_5_mod_prd_2048[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE1, 0xB0, 0x00, 0x80, 0x00, 0x00, 0x00}

#define DECLARE_SI_DECODE_FAIL \
static unsigned char si_decode_fail[] = {0x01, 0x02, 0x03, 0x04}

/*
bcch_dl_sch_ver_not_supported:value BCCH-DL-SCH-Message ::= 
{
  message messageClassExtension : 
    {
    }
}
*/
#define DECLARE_BCCH_DL_SCH_VER_NOT_SUPPORTED \
static unsigned char bcch_dl_sch_ver_not_supported[] = {0x80}

/*
si_ver_not_supported:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions criticalExtensionsFuture : 
    {
    }
}
}
*/
#define DECLARE_SI_VER_NOT_SUPPORTED \
static unsigned char si_ver_not_supported[] = {0x20}

/*
sib1_no_mcc_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mnc 
                {
                  4,
                  5,
                  6
                }
              },
              cellReservedForOperatorUse notReserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000001 00000000 00000000 0000'B,
          cellBarred barred,
          intraFreqReselection allowed,
          csg-Indication FALSE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType3
            }
          }
        },
        si-WindowLength ms1,
        systemInfoValueTag 0
      }
}
*/
#define DECLARE_SIB1_NO_MCC_8_5 \
static unsigned char sib1_no_mcc_8_5[] = {0x40, 0x28, 0xAD, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x28, 0x30, 0x00, 0x80, 0x00}

/*

sib1_sample:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType16-v11x0
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/
#define DECLARE_SIB1_8_16_sched \
static unsigned char sib1_8_16_sched[] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x01, 0x0B, 0x48, 0x80}

/*

sib1_cell2_withsib16:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000101 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType16-v11x0
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/

#define DECLARE_SIB1_id2_16_sched \
static unsigned char sib1_id2_16_sched[] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x50, 0x00, 0x00, 0xA2, 0x83, 0x01, 0x0B, 0x48, 0x80}

/*
sib1_invalid_mapping_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  5,
                  6
                }
              },
              cellReservedForOperatorUse notReserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000001 00000000 00000000 0000'B,
          cellBarred barred,
          intraFreqReselection allowed,
          csg-Indication FALSE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType3,
              sibType3
            }
          }
        },
        si-WindowLength ms1,
        systemInfoValueTag 0
      }
}
*/
#define DECLARE_SIB1_INVALID_MAPPING_8_5 \
static unsigned char sib1_invalid_mapping_8_5[] = {0x40, 0x44, 0x8E, 0x8A, 0xD0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x02, 0x83, 0x00, 0x10, 0x00, 0x00}

/*
sib1_invalid_empty_si_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  5,
                  6
                }
              },
              cellReservedForOperatorUse notReserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000001 00000000 00000000 0000'B,
          cellBarred barred,
          intraFreqReselection allowed,
          csg-Indication FALSE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
            }
          }
        },
        si-WindowLength ms1,
        systemInfoValueTag 0
      }
}
*/
#define DECLARE_SIB1_INVALID_EMPTY_SI_8_5 \
static unsigned char sib1_invalid_empty_si_8_5[] = {0x40, 0x44, 0x8E, 0x8A, 0xD0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x02, 0x83, 0x08, 0x08, 0x00, 0x00, 0x00}

/*
sib1_mapping_spare_value_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  5,
                  6
                }
              },
              cellReservedForOperatorUse notReserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000001 00000000 00000000 0000'B,
          cellBarred barred,
          intraFreqReselection allowed,
          csg-Indication FALSE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              spare7
            }
          }
        },
        si-WindowLength ms1,
        systemInfoValueTag 0
      }
}
*/
#define DECLARE_SIB1_MAPPING_SPARE_VALUE_8_5 \
static unsigned char sib1_mapping_spare_value_8_5[] = {0x40, 0x44, 0x8E, 0x8A, 0xD0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x02, 0x83, 0x00, 0x0A, 0x40, 0x00}

/*
sib1_infeasible_schedule_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  5,
                  6
                }
              },
              cellReservedForOperatorUse notReserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000001 00000000 00000000 0000'B,
          cellBarred barred,
          intraFreqReselection allowed,
          csg-Indication FALSE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType4
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType5
            }
          }
        },
        si-WindowLength ms40,
        systemInfoValueTag 0
      }
}
*/
#define DECLARE_SIB1_INFEASIBLE_SCHEDULE_8_5 \
static unsigned char sib1_infeasible_schedule_8_5[] = {0x40, 0x44, 0x8E, 0x8A, 0xD0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x02, 0x83, 0x10, 0x08, 0x00, 0x42, 0x02, 0x2C, 0x00}

/*
sib1_cell1_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  5,
                  6
                }
              },
              cellReservedForOperatorUse notReserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000001 00000000 00000000 0000'B,
          cellBarred barred,
          intraFreqReselection allowed,
          csg-Indication FALSE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType3
            }
          }
        },
        si-WindowLength ms1,
        systemInfoValueTag 0
      }
}
*/
#define DECLARE_SIB1_CELLS_8_5 \
static unsigned char sib1_cells_8_5[11][16] = {{0x40, 0x44, 0x8E, 0x8A, 0xD0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x02, 0x83, 0x00, 0x08, 0x00, 0x00}, \
                                           {0x40, 0x44, 0x8E, 0x8A, 0xD0, 0x00, 0x00, 0x20, 0x00, 0x00, 0x02, 0x83, 0x00, 0x08, 0x00, 0x00}, \
                                           {0x40, 0x44, 0x8E, 0x8A, 0xD0, 0x00, 0x00, 0x30, 0x00, 0x00, 0x02, 0x83, 0x00, 0x08, 0x00, 0x00}, \
                                           {0x40, 0x44, 0x8E, 0x8A, 0xD0, 0x00, 0x00, 0x40, 0x00, 0x00, 0x02, 0x83, 0x00, 0x08, 0x00, 0x00}, \
                                           {0x40, 0x44, 0x8E, 0x8A, 0xD0, 0x00, 0x00, 0x50, 0x00, 0x00, 0x02, 0x83, 0x00, 0x08, 0x00, 0x00}, \
                                           {0x40, 0x44, 0x8E, 0x8A, 0xD0, 0x00, 0x00, 0x60, 0x00, 0x00, 0x02, 0x83, 0x00, 0x08, 0x00, 0x00}, \
                                           {0x40, 0x44, 0x8E, 0x8A, 0xD0, 0x00, 0x00, 0x70, 0x00, 0x00, 0x02, 0x83, 0x00, 0x08, 0x00, 0x00}, \
                                           {0x40, 0x44, 0x8E, 0x8A, 0xD0, 0x00, 0x00, 0x80, 0x00, 0x00, 0x02, 0x83, 0x00, 0x08, 0x00, 0x00}, \
                                           {0x40, 0x44, 0x8E, 0x8A, 0xD0, 0x00, 0x00, 0x90, 0x00, 0x00, 0x02, 0x83, 0x00, 0x08, 0x00, 0x00}, \
                                           {0x40, 0x44, 0x8E, 0x8A, 0xD0, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x02, 0x83, 0x00, 0x08, 0x00, 0x00}, \
                                           {0x40, 0x44, 0x8E, 0x8A, 0xD0, 0x00, 0x00, 0xB0, 0x00, 0x00, 0x02, 0x83, 0x00, 0x08, 0x00, 0x00}}

/*
SIB1_2345_mcc_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType3,
              sibType4,
              sibType5
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/
#define DECLARE_SIB1_2_345_MCC_8_5 \
static unsigned char sib1_2_345_mcc_8_5[] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x09, 0x00, 0x18, 0x02, 0x22, 0x20}

#define DECLARE_SIB345_8_5_LONG \
static unsigned char sib345_8_5_long[] = {0x01, 0x05, 0x00, 0x00, 0x42, 0x00, 0x0F, 0x29, 0xE0, 0x00, 0x00, 0x4F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,0x9F, 0x80, 0x00, 0x14, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x05, 0x3C, 0x00, 0x00, 0x00,0x3F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x01, 0x4F, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

/*
SIB1_2345_8_mcc_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType4,
              sibType5
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType8
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/
#define DECLARE_SIB1_2345_8_MCC_8_5 \
static unsigned char sib1_2345_8_mcc_8_5[] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x11, 0x08, 0x00, 0x82, 0x20, 0x12, 0x91, 0x00}

/* Same as sib1_2345_8_mcc_8_5 - value tag changed to 3 */
#define DECLARE_SIB1_2345_8_MCC_8_5_VALUE_TAG_CHANGE \
static unsigned char sib1_2345_8_mcc_8_5_value_tag_change[] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x11, 0x08, 0x00, 0x82, 0x20, 0x12, 0x91, 0x80}

/*
SIB1_2345_8_mcc_8_5_cell2:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  4,
                  5,
                  6
                },
                mnc 
                {
                  6,
                  7,
                  8
                }
              },
              cellReservedForOperatorUse reserved
            },
            {
              plmn-Identity 
              {
                mnc 
                {
                  1,
                  0,
                  1
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000001 00000000'B,
          cellIdentity '00010001 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType4,
              sibType5
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType8
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 3
      }
}
*/
#define DECLARE_SIB1_2345_8_MCC_8_5_CELL2 \
static unsigned char sib1_2345_8_mcc_8_5_cell2[] = {0x40, 0xD1, 0x5A, 0xCF, 0x04, 0x40, 0x40, 0x20, 0x02, 0x20, 0x00, 0x01, 0x45, 0x06, 0x22, 0x10, 0x01, 0x04, 0x40, 0x25, 0x23}

/*
SIB1_234567_mcc_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType4,
              sibType5
            }
          },
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType6
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType7
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/
#define DECLARE_SIB1_234567_MCC_8_5 \
static unsigned char sib1_234567_mcc_8_5[] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x19, 0x08, 0x00, 0x82, 0x22, 0x11, 0x80, 0x90, 0x88}

#define DECLARE_SIB1_234567_MCC_8_5_V1 \
static unsigned char sib1_234567_mcc_8_5_v1[] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x19, 0x08, 0x10, 0x82, 0x22, 0x11, 0x80, 0x90, 0x84}


/*
SIB1_2345_10_11_mcc_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType4,
              sibType5
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType10
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType11
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/
#define DECLARE_SIB1_2345_10_11_MCC_8_5 \
static unsigned char sib1_2345_10_11_mcc_8_5[] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x19, 0x08, 0x00, 0x82, 0x20, 0x13, 0x80, 0xA0, 0x88}

/*
SIB1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType4,
              sibType5
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType12-v920
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/

#define DECLARE_SIB1_2345_12_MCC_8_5 \
static unsigned char sib1_2345_12_mcc_8_5[] = { \
  0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, \
  0x11, 0x08, 0x00, 0x82, 0x20, 0x14, 0x91, 0x00}


/*
SIB1_2345_tdd_mcc_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 33,
        schedulingInfoList 
        {
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType4,
              sibType5
            }
          }
        },
        tdd-Config 
        {
          subframeAssignment sa2,
          specialSubframePatterns ssp2
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/
#define DECLARE_SIB1_2345_TDD_MCC_8_5 \
static unsigned char sib1_2345_tdd_mcc_8_5[] = {0x50, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0xA0, 0x09, 0x08, 0x00, 0x82, 0x24, 0x44, 0x40}

/* Same as sib1_2345_tdd_mcc_8_5 with new tdd config (enum values 3 instead of 2) */
#define DECLARE_SIB1_2345_NEW_TDD_MCC_8_5 \
static unsigned char sib1_2345_new_tdd_mcc_8_5[] = {0x50, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0xA0, 0x09, 0x08, 0x00, 0x82, 0x26, 0x64, 0x40}

/*
sib6_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib6 : 
                {
                  t-ReselectionUTRA 0
                }
            }
          }
      }
}*/
#define DECLARE_SIB6_8_5 \
static unsigned char sib6_8_5[] = {0x00, 0x10, 0x00}

/*
sib7_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib7 : 
                {
                  t-ReselectionGERAN 0
                }
            }
          }
      }
}*/
#define DECLARE_SIB7_8_5 \
static unsigned char sib7_8_5[] = {0x00, 0x14, 0x00}

/*
sib8_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib8 : 
                {
                }
            }
          }
      }
}
*/
#define DECLARE_SIB8_8_5 \
static unsigned char sib8_8_5[] = {0x00, 0x18, 0x00}

/*
sib8_sync_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib8 : 
                {
                  systemTimeInfo 
                  {
                    cdma-EUTRA-Synchronisation FALSE,
                    cdma-SystemTime synchronousSystemTime : '00000000 00101111 11100110 00000000 000 ...'B
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_SIB8_SYNC_8_5 \
static unsigned char sib8_sync_8_5[] = {0x00, 0x19, 0x10, 0x01, 0x7F, 0x30, 0x00, 0x00}

/*
sib8_async_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib8 : 
                {
                  systemTimeInfo 
                  {
                    cdma-EUTRA-Synchronisation FALSE,
                    cdma-SystemTime asynchronousSystemTime : '00000000 01111000 11101010 00000000 000 ...'B
                  }
                }
            }
          }
      }
}
*/ 
#define DECLARE_SIB8_ASYNC_8_5 \
static unsigned char sib8_async_8_5[] = {0x00, 0x19, 0x08, 0x03, 0xC7, 0x50, 0x00, 0x00, 0x00}

/*
sib10_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib10 : 
                {
                  messageIdentifier '00010001 00000000'B,
                  serialNumber '00000000 00000000'B,
                  warningType '0000'H
                }
            }
          }
      }
}
*/
#define DECLARE_SIB10_8_5 \
static unsigned char sib10_8_5[] = {0x00, 0x20, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00}

/*
sib11_seg1of2_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib11 : 
                {
                  messageIdentifier '00010001 00000000'B,
                  serialNumber '00000000 00000000'B,
                  warningMessageSegmentType notLastSegment,
                  warningMessageSegmentNumber 0,
                  warningMessageSegment '1111'H
                  dataCodingScheme '01'H
                }
            }
          }
      }
}
*/
#define DECLARE_SIB11_SEG1OF2_8_5 \
static unsigned char sib11_seg1of2_8_5[] = {0x00, 0x25, 0x11, 0x00, 0x00, 0x00, 0x00, 0x04, 0x22, 0x22, 0x02}

/*
SIB11:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib11 : 
                {
                  messageIdentifier '00010001 00010001'B,
                  serialNumber '00000000 00000000'B,
                  warningMessageSegmentType notLastSegment,
                  warningMessageSegmentNumber 0,
                  warningMessageSegment '1111'H,
                  dataCodingScheme '01'H
                }
            }
          }
      }
}
*/
/* Same as sib11_seg1of2_8_5 except msgId changed from 0x1100 to 0x1111 */
#define DECLARE_SIB11_SEG1OF2_NEW_MSG_ID_8_5 \
static unsigned char sib11_seg1of2_new_msg_id_8_5[] = {0x00, 0x25, 0x11, 0x11, 0x00, 0x00, 0x00, 0x04, 0x22, 0x22, 0x02}

/*
sib11_seg2of2_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib11 : 
                {
                  messageIdentifier '00010001 00000000'B,
                  serialNumber '00000000 00000000'B,
                  warningMessageSegmentType lastSegment,
                  warningMessageSegmentNumber 1,
                  warningMessageSegment '222222'H
                }
            }
          }
      }
}
*/
#define DECLARE_SIB11_SEG2OF2_8_5 \
static unsigned char sib11_seg2of2_8_5[] = {0x00, 0x24, 0x11, 0x00, 0x00, 0x00, 0x82, 0x06, 0x44, 0x44, 0x44}

/* Same as sib11_seg2of2_8_5 except msgId changed from 0x1100 to 0x1111 */
#define DECLARE_SIB11_SEG2OF2_NEW_MSG_ID_8_5 \
static unsigned char sib11_seg2of2_new_msg_id_8_5[] = {0x00, 0x24, 0x11, 0x11, 0x00, 0x00, 0x82, 0x06, 0x44, 0x44, 0x44}

/*
sib11_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib11 : 
                {
                  messageIdentifier '00010001 00000000'B,
                  serialNumber '00000000 00000000'B,
                  warningMessageSegmentType lastSegment,
                  warningMessageSegmentNumber 0,
                  warningMessageSegment '222222'H
                  dataCodingScheme '01'H
                }
            }
          }
      }
}
*/
#define DECLARE_SIB11_8_5 \
static unsigned char sib11_8_5[] = {0x00, 0x25, 0x11, 0x00, 0x00, 0x00, 0x80, 0x06, 0x44, 0x44, 0x44, 0x02}

/*
sib_11_null_msg_seg:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib11 : 
                {
                  messageIdentifier '00010001 00000000'B,
                  serialNumber '00000000 00000000'B,
                  warningMessageSegmentType lastSegment,
                  warningMessageSegmentNumber 0,
                  warningMessageSegment ''H,
                  dataCodingScheme '01'H
                }
            }
          }
      }
}
*/
#define DECLARE_SIB11_8_NULL_MSGSEG \
static unsigned char sib11_8_NULL_MSGSEG[] = {0x00, 0x25, 0x11, 0x00, 0x00, 0x00, 0x80, 0x00, 0x02}

/*
sib11_seg3extra_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib11 : 
                {
                  messageIdentifier '00010001 00000000'B,
                  serialNumber '00000000 00000000'B,
                  warningMessageSegmentType notLastSegment,
                  warningMessageSegmentNumber 2,
                  warningMessageSegment '33333333'H
                }
            }
          }
      }
}
*/
#define DECLARE_SIB11_SEG3EXTRA_8_5 \
static unsigned char sib11_seg3extra_8_5[] = {0x00, 0x24, 0x11, 0x00, 0x00, 0x00, 0x04, 0x08, 0x66, 0x66, 0x66, 0x66}

/*
SIB10_SIB3_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib10 : 
                {
                  messageIdentifier '00010001 00000000'B,
                  serialNumber '00000000 00000000'B,
                  warningType '0000'H
                },
              sib3 : 
                {
                  cellReselectionInfoCommon 
                  {
                    q-Hyst dB0
                  },
                  cellReselectionServingFreqInfo 
                  {
                    threshServingLow 0,
                    cellReselectionPriority 0
                  },
                  intraFreqCellReselectionInfo 
                  {
                    q-RxLevMin -30,
                    presenceAntennaPort1 FALSE,
                    neighCellConfig '00'B,
                    t-ReselectionEUTRA 0
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_SIB10_SIB3_8_5 \
static unsigned char sib10_sib3_8_5[] = {0x00, 0xA0, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0xA0, 0x00}

/*
SIB2_SIB11_seg1of2_8_5:value BCCH-DL-SCH-Message ::= 
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
                        numberOfRA-Preambles n4
                      },
                      powerRampingParameters 
                      {
                        powerRampingStep dB0,
                        preambleInitialReceivedTargetPower dBm-120
                      },
                      ra-SupervisionInfo 
                      {
                        preambleTransMax n3,
                        ra-ResponseWindowSize sf2,
                        mac-ContentionResolutionTimer sf8
                      },
                      maxHARQ-Msg3Tx 2
                    },
                    bcch-Config 
                    {
                      modificationPeriodCoeff n2
                    },
                    pcch-Config 
                    {
                      defaultPagingCycle rf32,
                      nB fourT
                    },
                    prach-Config 
                    {
                      rootSequenceIndex 0,
                      prach-ConfigInfo 
                      {
                        prach-ConfigIndex 0,
                        highSpeedFlag FALSE,
                        zeroCorrelationZoneConfig 0,
                        prach-FreqOffset 0
                      }
                    },
                    pdsch-Config 
                    {
                      referenceSignalPower 0,
                      p-b 0
                    },
                    pusch-Config 
                    {
                      pusch-ConfigBasic 
                      {
                        n-SB 1,
                        hoppingMode interSubFrame,
                        pusch-HoppingOffset 0,
                        enable64QAM FALSE
                      },
                      ul-ReferenceSignalsPUSCH 
                      {
                        groupHoppingEnabled FALSE,
                        groupAssignmentPUSCH 0,
                        sequenceHoppingEnabled FALSE,
                        cyclicShift 0
                      }
                    },
                    pucch-Config 
                    {
                      deltaPUCCH-Shift ds1,
                      nRB-CQI 0,
                      nCS-AN 0,
                      n1PUCCH-AN 0
                    },
                    soundingRS-UL-Config release : NULL,
                    uplinkPowerControl 
                    {
                      p0-NominalPUSCH 0,
                      alpha al0,
                      p0-NominalPUCCH -100,
                      deltaFList-PUCCH 
                      {
                        deltaF-PUCCH-Format1 deltaF-2,
                        deltaF-PUCCH-Format1b deltaF1,
                        deltaF-PUCCH-Format2 deltaF-2,
                        deltaF-PUCCH-Format2a deltaF-2,
                        deltaF-PUCCH-Format2b deltaF-2
                      },
                      deltaPreambleMsg3 0
                    },
                    ul-CyclicPrefixLength len1
                  },
                  ue-TimersAndConstants 
                  {
                    t300 ms100,
                    t301 ms100,
                    t310 ms0,
                    n310 n1,
                    t311 ms1000,
                    n311 n1
                  },
                  freqInfo 
                  {
                    additionalSpectrumEmission 1
                  },
                  timeAlignmentTimerCommon sf500
                },
              sib11 : 
                {
                  messageIdentifier '00010001 00000000'B,
                  serialNumber '00000000 00000000'B,
                  warningMessageSegmentType notLastSegment,
                  warningMessageSegmentNumber 0,
                  warningMessageSegment '1111'H
                  dataCodingScheme '01'H
                }
            }
          }
      }
}
*/
#define DECLARE_SIB2_SIB11_SEG1OF2_8_5 \
static unsigned char sib2_sib11_seg1of2_8_5[] = {0x00, 0x80, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE1, 0xB0, 0x00, 0x80, 0x00, 0x00, 0x00, 0x94, 0x44, 0x00, 0x00, 0x00, 0x00, 0x10, 0x88, 0x88, 0x08}

/*
SIB2_SIB11_seg2of2_8_5:value BCCH-DL-SCH-Message ::= 
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
                        numberOfRA-Preambles n4
                      },
                      powerRampingParameters 
                      {
                        powerRampingStep dB0,
                        preambleInitialReceivedTargetPower dBm-120
                      },
                      ra-SupervisionInfo 
                      {
                        preambleTransMax n3,
                        ra-ResponseWindowSize sf2,
                        mac-ContentionResolutionTimer sf8
                      },
                      maxHARQ-Msg3Tx 2
                    },
                    bcch-Config 
                    {
                      modificationPeriodCoeff n2
                    },
                    pcch-Config 
                    {
                      defaultPagingCycle rf32,
                      nB fourT
                    },
                    prach-Config 
                    {
                      rootSequenceIndex 0,
                      prach-ConfigInfo 
                      {
                        prach-ConfigIndex 0,
                        highSpeedFlag FALSE,
                        zeroCorrelationZoneConfig 0,
                        prach-FreqOffset 0
                      }
                    },
                    pdsch-Config 
                    {
                      referenceSignalPower 0,
                      p-b 0
                    },
                    pusch-Config 
                    {
                      pusch-ConfigBasic 
                      {
                        n-SB 1,
                        hoppingMode interSubFrame,
                        pusch-HoppingOffset 0,
                        enable64QAM FALSE
                      },
                      ul-ReferenceSignalsPUSCH 
                      {
                        groupHoppingEnabled FALSE,
                        groupAssignmentPUSCH 0,
                        sequenceHoppingEnabled FALSE,
                        cyclicShift 0
                      }
                    },
                    pucch-Config 
                    {
                      deltaPUCCH-Shift ds1,
                      nRB-CQI 0,
                      nCS-AN 0,
                      n1PUCCH-AN 0
                    },
                    soundingRS-UL-Config release : NULL,
                    uplinkPowerControl 
                    {
                      p0-NominalPUSCH 0,
                      alpha al0,
                      p0-NominalPUCCH -100,
                      deltaFList-PUCCH 
                      {
                        deltaF-PUCCH-Format1 deltaF-2,
                        deltaF-PUCCH-Format1b deltaF1,
                        deltaF-PUCCH-Format2 deltaF-2,
                        deltaF-PUCCH-Format2a deltaF-2,
                        deltaF-PUCCH-Format2b deltaF-2
                      },
                      deltaPreambleMsg3 0
                    },
                    ul-CyclicPrefixLength len1
                  },
                  ue-TimersAndConstants 
                  {
                    t300 ms100,
                    t301 ms100,
                    t310 ms0,
                    n310 n1,
                    t311 ms1000,
                    n311 n1
                  },
                  freqInfo 
                  {
                    additionalSpectrumEmission 1
                  },
                  timeAlignmentTimerCommon sf500
                },
              sib11 : 
                {
                  messageIdentifier '00010001 00000000'B,
                  serialNumber '00000000 00000000'B,
                  warningMessageSegmentType lastSegment,
                  warningMessageSegmentNumber 1,
                  warningMessageSegment '222222'H
                }
            }
          }
      }
}
*/
#define DECLARE_SIB2_SIB11_SEG2OF2_8_5 \
static unsigned char sib2_sib11_seg2of2_8_5[] = {0x00, 0x80, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE1, 0xB0, 0x00, 0x80, 0x00, 0x00, 0x00, 0x90, 0x44, 0x00, 0x00, 0x02, 0x08, 0x19, 0x11, 0x11, 0x10}

/*
sib11_seg2of2_1246_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib11 : 
                {
                  messageIdentifier '00010001 00000000'B,
                  serialNumber '00000000 00000000'B,
                  warningMessageSegmentType lastSegment,
                  warningMessageSegmentNumber 1,
                  warningMessageSegment '000FF200780133006064D6003CF1F10078 ...'H
                }
            }
          }
      }
}
*/
#define DECLARE_SIB11_SEG2OF2_1246_8_5 \
static unsigned char sib11_seg2of2_1246_8_5[] = {0x00, 0x24, 0x11, 0x00, 0x00, 0x00, 0x83, 0x09, 0xB8, 0x00, 0x1F, 0xE4, 0x00, 0xF0, 0x02, 0x66, 0x00, 0xC0, 0xC9, \
 0xAC, 0x00, 0x79, 0xE3, 0xE2, 0x00, 0xF0, 0xCF, 0xAC, 0x00, 0x40, 0xCF, 0xAC, 0x00, 0x70, 0xD5, 0xAC, 0x00, 0x99, 0xE3, 0xE2, 0x01, 0x11, 0x83, 0xAC, 0x01, 0x21, \
 0x85, 0xAC, 0x01, 0xF1, 0x87, 0xAC, 0x00, 0xB9, 0xE3, 0xE2, 0x01, 0xF1, 0x89, 0xAC, 0x01, 0x51, 0x8B, 0xAC, 0x01, 0xD1, 0x91, 0xAC, 0x00, 0xD9, 0xE3, 0xE2, 0x01, \
 0x01, 0x99, 0xAC, 0x00, 0xD1, 0x9B, 0xAC, 0x01, 0x51, 0x9D, 0xAC, 0x00, 0xF9, 0xE3, 0xE2, 0x00, 0xD0, 0xE1, 0xAC, 0x01, 0x80, 0xEF, 0xAC, 0x01, 0x00, 0xFD, 0xAC, \
 0x01, 0x19, 0xE3, 0xE2, 0x00, 0x40, 0xE3, 0xAE, 0x01, 0x90, 0xE1, 0xAE, 0x00, 0x60, 0xEB, 0xAE, 0x01, 0x39, 0xE3, 0xE2, 0x01, 0x40, 0xEB, 0xAE, 0x00, 0x50, 0xF1, \
 0xAE, 0x00, 0x40, 0xF7, 0xAE, 0x01, 0x59, 0xE3, 0xE2, 0x01, 0xA0, 0xEF, 0xAE, 0x00, 0xD0, 0xFD, 0xAE, 0x00, 0x20, 0xFD, 0xAE, 0x01, 0x79, 0xE3, 0xE2, 0x00, 0xF1, \
 0xEB, 0xC8, 0x00, 0x41, 0xEB, 0xC8, 0x01, 0x41, 0xF1, 0xC8, 0x01, 0x99, 0xE3, 0xE2, 0x01, 0x31, 0xF5, 0xC8, 0x01, 0xE1, 0xF5, 0xC8, 0x00, 0xF1, 0xFD, 0xC8, 0x01, \
 0xB9, 0xE3, 0xE2, 0x01, 0x71, 0x05, 0xAC, 0x00, 0x11, 0x09, 0xAC, 0x01, 0xA1, 0xFD, 0xC8, 0x01, 0xD9, 0xE3, 0xE2, 0x00, 0xA1, 0x0D, 0xAC, 0x01, 0xA1, 0x11, 0xAC, \
 0x00, 0x21, 0x15, 0xAC, 0x01, 0xF9, 0xE3, 0xE2, 0x00, 0x71, 0x19, 0xAC, 0x01, 0xE1, 0x1D, 0xAC, 0x01, 0x31, 0x1D, 0xAC, 0x00, 0x19, 0xE5, 0xE2, 0x01, 0x51, 0x29, \
 0xAC, 0x00, 0xE1, 0x33, 0xAC, 0x00, 0xA1, 0x29, 0xAC, 0x00, 0x39, 0xE5, 0xE2, 0x01, 0xC1, 0x33, 0xAC, 0x01, 0x30, 0xE3, 0xB4, 0x00, 0x80, 0xE9, 0xB4, 0x00, 0x59, \
 0xE5, 0xE2, 0x00, 0x20, 0xF1, 0xB4, 0x01, 0x50, 0xF5, 0xB4, 0x01, 0x70, 0xEF, 0xB4, 0x00, 0x79, 0xE5, 0xE2, 0x00, 0x30, 0xF7, 0xB4, 0x00, 0x81, 0x01, 0xB4, 0x01, \
 0xD0, 0xFF, 0xB4, 0x00, 0x99, 0xE5, 0xE2, 0x01, 0x61, 0x01, 0xB4, 0x01, 0xE1, 0x09, 0xB4, 0x01, 0xF1, 0x21, 0xB6, 0x00, 0xB9, 0xE5, 0xE2, 0x00, 0xA1, 0x29, 0xB6, \
 0x00, 0x31, 0x25, 0xB6, 0x01, 0x41, 0x2B, 0xB6, 0x00, 0xD9, 0xE5, 0xE2, 0x01, 0x81, 0x33, 0xB6, 0x00, 0x21, 0x37, 0xB6, 0x00, 0xC1, 0x39, 0xB6, 0x00, 0xF9, 0xE5, \
 0xE2, 0x00, 0x41, 0x3D, 0xB6, 0x01, 0x81, 0x43, 0xB6, 0x00, 0xB1, 0x45, 0xB6, 0x01, 0x19, 0xE5, 0xE2, 0x01, 0xD1, 0x4D, 0xB6, 0x01, 0x90, 0xA5, 0xB8, 0x00, 0x10, \
 0xA9, 0xB8, 0x01, 0x39, 0xE5, 0xE2, 0x00, 0x60, 0xAD, 0xB8, 0x00, 0x30, 0xAF, 0xB8, 0x00, 0xB0, 0xB1, 0xB8, 0x01, 0x59, 0xE5, 0xE2, 0x01, 0xB0, 0xB5, 0xB8, 0x01, \
 0x80, 0xB7, 0xB8, 0x01, 0x20, 0xBB, 0xB8, 0x01, 0x79, 0xE5, 0xE2, 0x01, 0x81, 0x29, 0xBC, 0x01, 0x21, 0x03, 0xBA, 0x01, 0x50, 0xC9, 0xB8, 0x01, 0x99, 0xE5, 0xE2, \
 0x00, 0x11, 0x2F, 0xBC, 0x00, 0x71, 0x41, 0xBC, 0x01, 0x30, 0x65, 0x96, 0x01, 0xB9, 0xE5, 0xE2, 0x01, 0xC1, 0x3F, 0xBC, 0x01, 0xF0, 0x6D, 0x96, 0x00, 0x70, 0x71, \
 0x96, 0x01, 0xD9, 0xE5, 0xE2, 0x01, 0xE0, 0x77, 0x96, 0x01, 0x20, 0x7D, 0x96, 0x00, 0x70, 0x7D, 0x96, 0x01, 0xF9, 0xE5, 0xE2, 0x00, 0x50, 0x27, 0x96, 0x01, 0x90, \
 0x2B, 0x96, 0x00, 0xE0, 0x2B, 0x96, 0x00, 0x19, 0xE7, 0xE2, 0x01, 0x70, 0x8B, 0x96, 0x00, 0x30, 0x8F, 0x96, 0x01, 0x50, 0x93, 0x96, 0x00, 0x39, 0xE7, 0xE2, 0x01, \
 0x80, 0x8D, 0x96, 0x00, 0x30, 0x95, 0x96, 0x01, 0x70, 0x9D, 0x96, 0x00, 0x59, 0xE7, 0xE2, 0x01, 0x30, 0x97, 0x96, 0x00, 0x20, 0xA3, 0x96, 0x00, 0xD0, 0xA3, 0x96, \
 0x00, 0x79, 0xE7, 0xE2, 0x00, 0x70, 0xAF, 0x96, 0x01, 0xF0, 0xB9, 0x96, 0x01, 0x40, 0xB9, 0x96, 0x00, 0x99, 0xE7, 0xE2, 0x00, 0xA0, 0xC5, 0x96, 0x00, 0x30, 0xC7, \
 0x96, 0x01, 0xF0, 0xC3, 0x96, 0x00, 0xB9, 0xE7, 0xE2, 0x00, 0x70, 0xD5, 0x96, 0x00, 0x40, 0xDD, 0x96, 0x01, 0x90, 0xDB, 0x96, 0x00, 0xD9, 0xE7, 0xE2, 0x01, 0x20, \
 0xDD, 0x96, 0x00, 0xE0, 0xFD, 0x96, 0x01, 0x60, 0xFF, 0x96, 0x00, 0xF9, 0xE7, 0xE2, 0x01, 0xE1, 0x45, 0xAC, 0x00, 0x91, 0x47, 0xAC, 0x01, 0x61, 0x4F, 0xAC, 0x01, \
 0x19, 0xE7, 0xE2, 0x00, 0x11, 0xA1, 0xAC, 0x00, 0x21, 0xA7, 0xAC, 0x01, 0x71, 0xA5, 0xAC, 0x01, 0x39, 0xE7, 0xE2, 0x01, 0x01, 0xA7, 0xAC, 0x00, 0x31, 0xB5, 0xAC, \
 0x01, 0xC1, 0xB5, 0xAC, 0x01, 0x59, 0xE7, 0xE2, 0x00, 0x71, 0xBB, 0xAC, 0x00, 0xB1, 0xE1, 0xAC, 0x00, 0x41, 0xE3, 0xAC, 0x01, 0x79, 0xE7, 0xE2, 0x01, 0xA1, 0xE7, \
 0xAC, 0x00, 0xF1, 0xE7, 0xAC, 0x01, 0x21, 0xBB, 0xAC, 0x01, 0x99, 0xE7, 0xE2, 0x01, 0xB1, 0xF1, 0xAC, 0x00, 0x31, 0xF7, 0xAC, 0x00, 0x51, 0xED, 0xAC, 0x01, 0xB9, \
 0xE7, 0xE2, 0x01, 0x11, 0xF7, 0xAC, 0x00, 0x10, 0x21, 0xAE, 0x00, 0x90, 0x23, 0xAE, 0x01, 0xD9, 0xE7, 0xE2, 0x01, 0x80, 0x27, 0xAE, 0x00, 0x40, 0x2B, 0xAE, 0x00, \
 0xE0, 0x2D, 0xAE, 0x01, 0xF9, 0xE7, 0xE2, 0x01, 0xC0, 0x2D, 0xAE, 0x00, 0x60, 0x35, 0xAE, 0x01, 0xB0, 0x33, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x65, 0xAE, \
 0x01, 0xC0, 0x67, 0xAE, 0x01, 0x00, 0x37, 0xAE, 0x01, 0xE0, 0x39, 0xAE, 0x00, 0x1A, 0x07, 0x04, 0x00, 0x30, 0x01, 0xAE, 0x05, 0xF0, 0x1F, 0xE4, 0x00, 0xF0, 0x02, \
 0x66, 0x01, 0xE0, 0x6B, 0xAE, 0x01, 0x40, 0x6D, 0xAE, 0x00, 0x90, 0x6D, 0xAE, 0x00, 0x10, 0xAF, 0xAE, 0x00, 0xA0, 0xB1, 0xAE, 0x00, 0xC0, 0xAF, 0xAE, 0x00, 0x50, \
 0xBD, 0xAE, 0x01, 0x70, 0xAF, 0xAE, 0x01, 0x70, 0xA5, 0xAE, 0x01, 0xA0, 0xB3, 0xAE, 0x00, 0x30, 0xB7, 0xAE, 0x01, 0x80, 0xB5, 0xAE, 0x00, 0xA1, 0x0B, 0xAE, 0x00, \
 0x31, 0x0D, 0xAE, 0x01, 0x81, 0x0B, 0xAE, 0x00, 0x61, 0x1D, 0xAE, 0x00, 0x41, 0x1F, 0xAE, 0x01, 0x91, 0x1D, 0xAE, 0x00, 0x21, 0x07, 0xAE, 0x01, 0xC1, 0x07, 0xAE, \
 0x01, 0xF1, 0x11, 0xAE, 0x00, 0x11, 0x41, 0xAE, 0x01, 0x21, 0x19, 0xAE, 0x00, 0x71, 0x19, 0xAE, 0x00, 0xB1, 0x4F, 0xAE, 0x01, 0xD1, 0x19, 0xAE, 0x01, 0x41, 0x47, \
 0xAE, 0x00, 0x91, 0x47, 0xAE, 0x00, 0xA1, 0x49, 0xAE, 0x01, 0xF1, 0x47, 0xAE, 0x01, 0x51, 0x55, 0xAE, 0x00, 0xA1, 0x55, 0xAE, 0x00, 0xD1, 0x59, 0xAE, 0x00, 0x71, \
 0x8F, 0xAE, 0x01, 0x21, 0x9B, 0xAE, 0x00, 0xB1, 0x9D, 0xAE, 0x00, 0x01, 0x9D, 0xAE, 0x00, 0x51, 0x85, 0xAE, 0x01, 0x91, 0x87, 0xAE, 0x00, 0x71, 0x89, 0xAE, 0x01, \
 0xB1, 0x93, 0xAE, 0x01, 0xB1, 0x97, 0xAE, 0x00, 0x31, 0x9B, 0xAE, 0x01, 0xA1, 0xCB, 0xAE, 0x00, 0xF1, 0xCB, 0xAE, 0x01, 0x21, 0xDB, 0xAE, 0x00, 0x21, 0xDD, 0xAE, \
 0x00, 0x61, 0xC3, 0xAE, 0x00, 0xD1, 0xDD, 0xAE, 0x01, 0x91, 0xC9, 0xAE, 0x00, 0x01, 0xD1, 0xAE, 0x00, 0xB1, 0xD1, 0xAE, 0x00, 0x31, 0xD7, 0xAE, 0x00, 0x00, 0x0B, \
 0xB0, 0x00, 0xF0, 0x0F, 0xB0, 0x00, 0xD1, 0xD9, 0xAE, 0x00, 0xD0, 0x01, 0xB0, 0x01, 0x10, 0x03, 0xB0, 0x00, 0xD0, 0x05, 0xB0, 0x00, 0x20, 0x05, 0xB0, 0x00, 0x90, \
 0x13, 0xB0, 0x00, 0xF0, 0x15, 0xB0, 0x01, 0x70, 0x17, 0xB0, 0x01, 0x00, 0x19, 0xB0, 0x00, 0x50, 0x19, 0xB0, 0x01, 0xB0, 0x5B, 0xB0, 0x00, 0x60, 0x5D, 0xB0, 0x01, \
 0xF0, 0x41, 0xB0, 0x00, 0x80, 0x47, 0xB0, 0x01, 0x40, 0x41, 0xB0, 0x01, 0xE0, 0x47, 0xB0, 0x01, 0x30, 0x47, 0xB0, 0x00, 0x00, 0x51, 0xB0, 0x00, 0xB0, 0x55, 0xB0, \
 0x01, 0x50, 0x4F, 0xB0, 0x00, 0xA0, 0x8D, 0xB0, 0x01, 0xF0, 0x8B, 0xB0, 0x00, 0x90, 0x9F, 0xB0, 0x01, 0x40, 0x9F, 0xB0, 0x01, 0x30, 0x85, 0xB0, 0x00, 0x90, 0x87, \
 0xB0, 0x01, 0xE0, 0x85, 0xB0, 0x01, 0x50, 0x99, 0xB0, 0x01, 0x30, 0xD1, 0xB0, 0x00, 0x80, 0xDF, 0xB0, 0x00, 0xC0, 0xC1, 0xB0, 0x00, 0x10, 0xC1, 0xB0, 0x00, 0x80, \
 0xC9, 0xB0, 0x01, 0xD0, 0xC7, 0xB0, 0x01, 0xE0, 0xC9, 0xB0, 0x01, 0x30, 0xC9, 0xB0, 0x00, 0x20, 0xD7, 0xB0, 0x01, 0x70, 0xD5, 0xB0, 0x01, 0xB0, 0xDD, 0xB0, 0x00, \
 0x71, 0x11, 0xB0, 0x01, 0xC1, 0x0F, 0xB0, 0x01, 0xD1, 0x07, 0xB0, 0x01, 0x81, 0x09, 0xB0, 0x00, 0xD1, 0x09, 0xB0, 0x00, 0x71, 0x15, 0xB0, 0x00, 0xC1, 0x17, 0xB0, \
 0x00, 0x21, 0x19, 0xB0, 0x01, 0x71, 0x17, 0xB0, 0x00, 0xA1, 0x4D, 0xB0, 0x00, 0x11, 0x51, 0xB0, 0x01, 0x61, 0x4F, 0xB0, 0x00, 0xC1, 0x49, 0xB0, 0x01, 0x71, 0x49, \
 0xB0, 0x01, 0x71, 0x83, 0xB0, 0x00, 0x21, 0x85, 0xB0, 0x00, 0x61, 0x55, 0xB0, 0x01, 0x81, 0xC3, 0xB0, 0x01, 0x31, 0xC1, 0xB0, 0x00, 0xF1, 0xCB, 0xB0, 0x01, 0xA1, \
 0xCB, 0xB0, 0x01, 0x81, 0xDD, 0xB0, 0x00, 0x31, 0xDF, 0xB0, 0x01, 0xD1, 0x8F, 0xB0, 0x01, 0x11, 0x93, 0xB0, 0x00}

/* Same as sib11_seg2of2_1246_8_5 with segment longer by 1 byte */
#define DECLARE_SIB11_SEG2OF2_1247_8_5 \
static unsigned char sib11_seg2of2_1247_8_5[] = {0x00, 0x24, 0x11, 0x00, 0x00, 0x00, 0x83, 0x09, 0xBA, 0x01, 0x55, 0xEC, 0x00, 0xF0, 0x02, 0x66, 0x00, 0xC0, 0xC9, \
 0xAC, 0x00, 0x79, 0xE3, 0xE2, 0x00, 0xF0, 0xCF, 0xAC, 0x00, 0x40, 0xCF, 0xAC, 0x00, 0x70, 0xD5, 0xAC, 0x00, 0x99, 0xE3, 0xE2, 0x01, 0x11, 0x83, 0xAC, 0x01, 0x21, \
 0x85, 0xAC, 0x01, 0xF1, 0x87, 0xAC, 0x00, 0xB9, 0xE3, 0xE2, 0x01, 0xF1, 0x89, 0xAC, 0x01, 0x51, 0x8B, 0xAC, 0x01, 0xD1, 0x91, 0xAC, 0x00, 0xD9, 0xE3, 0xE2, 0x01, \
 0x01, 0x99, 0xAC, 0x00, 0xD1, 0x9B, 0xAC, 0x01, 0x51, 0x9D, 0xAC, 0x00, 0xF9, 0xE3, 0xE2, 0x00, 0xD0, 0xE1, 0xAC, 0x01, 0x80, 0xEF, 0xAC, 0x01, 0x00, 0xFD, 0xAC, \
 0x01, 0x19, 0xE3, 0xE2, 0x00, 0x40, 0xE3, 0xAE, 0x01, 0x90, 0xE1, 0xAE, 0x00, 0x60, 0xEB, 0xAE, 0x01, 0x39, 0xE3, 0xE2, 0x01, 0x40, 0xEB, 0xAE, 0x00, 0x50, 0xF1, \
 0xAE, 0x00, 0x40, 0xF7, 0xAE, 0x01, 0x59, 0xE3, 0xE2, 0x01, 0xA0, 0xEF, 0xAE, 0x00, 0xD0, 0xFD, 0xAE, 0x00, 0x20, 0xFD, 0xAE, 0x01, 0x79, 0xE3, 0xE2, 0x00, 0xF1, \
 0xEB, 0xC8, 0x00, 0x41, 0xEB, 0xC8, 0x01, 0x41, 0xF1, 0xC8, 0x01, 0x99, 0xE3, 0xE2, 0x01, 0x31, 0xF5, 0xC8, 0x01, 0xE1, 0xF5, 0xC8, 0x00, 0xF1, 0xFD, 0xC8, 0x01, \
 0xB9, 0xE3, 0xE2, 0x01, 0x71, 0x05, 0xAC, 0x00, 0x11, 0x09, 0xAC, 0x01, 0xA1, 0xFD, 0xC8, 0x01, 0xD9, 0xE3, 0xE2, 0x00, 0xA1, 0x0D, 0xAC, 0x01, 0xA1, 0x11, 0xAC, \
 0x00, 0x21, 0x15, 0xAC, 0x01, 0xF9, 0xE3, 0xE2, 0x00, 0x71, 0x19, 0xAC, 0x01, 0xE1, 0x1D, 0xAC, 0x01, 0x31, 0x1D, 0xAC, 0x00, 0x19, 0xE5, 0xE2, 0x01, 0x51, 0x29, \
 0xAC, 0x00, 0xE1, 0x33, 0xAC, 0x00, 0xA1, 0x29, 0xAC, 0x00, 0x39, 0xE5, 0xE2, 0x01, 0xC1, 0x33, 0xAC, 0x01, 0x30, 0xE3, 0xB4, 0x00, 0x80, 0xE9, 0xB4, 0x00, 0x59, \
 0xE5, 0xE2, 0x00, 0x20, 0xF1, 0xB4, 0x01, 0x50, 0xF5, 0xB4, 0x01, 0x70, 0xEF, 0xB4, 0x00, 0x79, 0xE5, 0xE2, 0x00, 0x30, 0xF7, 0xB4, 0x00, 0x81, 0x01, 0xB4, 0x01, \
 0xD0, 0xFF, 0xB4, 0x00, 0x99, 0xE5, 0xE2, 0x01, 0x61, 0x01, 0xB4, 0x01, 0xE1, 0x09, 0xB4, 0x01, 0xF1, 0x21, 0xB6, 0x00, 0xB9, 0xE5, 0xE2, 0x00, 0xA1, 0x29, 0xB6, \
 0x00, 0x31, 0x25, 0xB6, 0x01, 0x41, 0x2B, 0xB6, 0x00, 0xD9, 0xE5, 0xE2, 0x01, 0x81, 0x33, 0xB6, 0x00, 0x21, 0x37, 0xB6, 0x00, 0xC1, 0x39, 0xB6, 0x00, 0xF9, 0xE5, \
 0xE2, 0x00, 0x41, 0x3D, 0xB6, 0x01, 0x81, 0x43, 0xB6, 0x00, 0xB1, 0x45, 0xB6, 0x01, 0x19, 0xE5, 0xE2, 0x01, 0xD1, 0x4D, 0xB6, 0x01, 0x90, 0xA5, 0xB8, 0x00, 0x10, \
 0xA9, 0xB8, 0x01, 0x39, 0xE5, 0xE2, 0x00, 0x60, 0xAD, 0xB8, 0x00, 0x30, 0xAF, 0xB8, 0x00, 0xB0, 0xB1, 0xB8, 0x01, 0x59, 0xE5, 0xE2, 0x01, 0xB0, 0xB5, 0xB8, 0x01, \
 0x80, 0xB7, 0xB8, 0x01, 0x20, 0xBB, 0xB8, 0x01, 0x79, 0xE5, 0xE2, 0x01, 0x81, 0x29, 0xBC, 0x01, 0x21, 0x03, 0xBA, 0x01, 0x50, 0xC9, 0xB8, 0x01, 0x99, 0xE5, 0xE2, \
 0x00, 0x11, 0x2F, 0xBC, 0x00, 0x71, 0x41, 0xBC, 0x01, 0x30, 0x65, 0x96, 0x01, 0xB9, 0xE5, 0xE2, 0x01, 0xC1, 0x3F, 0xBC, 0x01, 0xF0, 0x6D, 0x96, 0x00, 0x70, 0x71, \
 0x96, 0x01, 0xD9, 0xE5, 0xE2, 0x01, 0xE0, 0x77, 0x96, 0x01, 0x20, 0x7D, 0x96, 0x00, 0x70, 0x7D, 0x96, 0x01, 0xF9, 0xE5, 0xE2, 0x00, 0x50, 0x27, 0x96, 0x01, 0x90, \
 0x2B, 0x96, 0x00, 0xE0, 0x2B, 0x96, 0x00, 0x19, 0xE7, 0xE2, 0x01, 0x70, 0x8B, 0x96, 0x00, 0x30, 0x8F, 0x96, 0x01, 0x50, 0x93, 0x96, 0x00, 0x39, 0xE7, 0xE2, 0x01, \
 0x80, 0x8D, 0x96, 0x00, 0x30, 0x95, 0x96, 0x01, 0x70, 0x9D, 0x96, 0x00, 0x59, 0xE7, 0xE2, 0x01, 0x30, 0x97, 0x96, 0x00, 0x20, 0xA3, 0x96, 0x00, 0xD0, 0xA3, 0x96, \
 0x00, 0x79, 0xE7, 0xE2, 0x00, 0x70, 0xAF, 0x96, 0x01, 0xF0, 0xB9, 0x96, 0x01, 0x40, 0xB9, 0x96, 0x00, 0x99, 0xE7, 0xE2, 0x00, 0xA0, 0xC5, 0x96, 0x00, 0x30, 0xC7, \
 0x96, 0x01, 0xF0, 0xC3, 0x96, 0x00, 0xB9, 0xE7, 0xE2, 0x00, 0x70, 0xD5, 0x96, 0x00, 0x40, 0xDD, 0x96, 0x01, 0x90, 0xDB, 0x96, 0x00, 0xD9, 0xE7, 0xE2, 0x01, 0x20, \
 0xDD, 0x96, 0x00, 0xE0, 0xFD, 0x96, 0x01, 0x60, 0xFF, 0x96, 0x00, 0xF9, 0xE7, 0xE2, 0x01, 0xE1, 0x45, 0xAC, 0x00, 0x91, 0x47, 0xAC, 0x01, 0x61, 0x4F, 0xAC, 0x01, \
 0x19, 0xE7, 0xE2, 0x00, 0x11, 0xA1, 0xAC, 0x00, 0x21, 0xA7, 0xAC, 0x01, 0x71, 0xA5, 0xAC, 0x01, 0x39, 0xE7, 0xE2, 0x01, 0x01, 0xA7, 0xAC, 0x00, 0x31, 0xB5, 0xAC, \
 0x01, 0xC1, 0xB5, 0xAC, 0x01, 0x59, 0xE7, 0xE2, 0x00, 0x71, 0xBB, 0xAC, 0x00, 0xB1, 0xE1, 0xAC, 0x00, 0x41, 0xE3, 0xAC, 0x01, 0x79, 0xE7, 0xE2, 0x01, 0xA1, 0xE7, \
 0xAC, 0x00, 0xF1, 0xE7, 0xAC, 0x01, 0x21, 0xBB, 0xAC, 0x01, 0x99, 0xE7, 0xE2, 0x01, 0xB1, 0xF1, 0xAC, 0x00, 0x31, 0xF7, 0xAC, 0x00, 0x51, 0xED, 0xAC, 0x01, 0xB9, \
 0xE7, 0xE2, 0x01, 0x11, 0xF7, 0xAC, 0x00, 0x10, 0x21, 0xAE, 0x00, 0x90, 0x23, 0xAE, 0x01, 0xD9, 0xE7, 0xE2, 0x01, 0x80, 0x27, 0xAE, 0x00, 0x40, 0x2B, 0xAE, 0x00, \
 0xE0, 0x2D, 0xAE, 0x01, 0xF9, 0xE7, 0xE2, 0x01, 0xC0, 0x2D, 0xAE, 0x00, 0x60, 0x35, 0xAE, 0x01, 0xB0, 0x33, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x65, 0xAE, \
 0x01, 0xC0, 0x67, 0xAE, 0x01, 0x00, 0x37, 0xAE, 0x01, 0xE0, 0x39, 0xAE, 0x00, 0x1A, 0x07, 0x04, 0x00, 0x30, 0x01, 0xAE, 0x05, 0xF0, 0x1F, 0xE4, 0x00, 0xF0, 0x02, \
 0x66, 0x01, 0xE0, 0x6B, 0xAE, 0x01, 0x40, 0x6D, 0xAE, 0x00, 0x90, 0x6D, 0xAE, 0x00, 0x10, 0xAF, 0xAE, 0x00, 0xA0, 0xB1, 0xAE, 0x00, 0xC0, 0xAF, 0xAE, 0x00, 0x50, \
 0xBD, 0xAE, 0x01, 0x70, 0xAF, 0xAE, 0x01, 0x70, 0xA5, 0xAE, 0x01, 0xA0, 0xB3, 0xAE, 0x00, 0x30, 0xB7, 0xAE, 0x01, 0x80, 0xB5, 0xAE, 0x00, 0xA1, 0x0B, 0xAE, 0x00, \
 0x31, 0x0D, 0xAE, 0x01, 0x81, 0x0B, 0xAE, 0x00, 0x61, 0x1D, 0xAE, 0x00, 0x41, 0x1F, 0xAE, 0x01, 0x91, 0x1D, 0xAE, 0x00, 0x21, 0x07, 0xAE, 0x01, 0xC1, 0x07, 0xAE, \
 0x01, 0xF1, 0x11, 0xAE, 0x00, 0x11, 0x41, 0xAE, 0x01, 0x21, 0x19, 0xAE, 0x00, 0x71, 0x19, 0xAE, 0x00, 0xB1, 0x4F, 0xAE, 0x01, 0xD1, 0x19, 0xAE, 0x01, 0x41, 0x47, \
 0xAE, 0x00, 0x91, 0x47, 0xAE, 0x00, 0xA1, 0x49, 0xAE, 0x01, 0xF1, 0x47, 0xAE, 0x01, 0x51, 0x55, 0xAE, 0x00, 0xA1, 0x55, 0xAE, 0x00, 0xD1, 0x59, 0xAE, 0x00, 0x71, \
 0x8F, 0xAE, 0x01, 0x21, 0x9B, 0xAE, 0x00, 0xB1, 0x9D, 0xAE, 0x00, 0x01, 0x9D, 0xAE, 0x00, 0x51, 0x85, 0xAE, 0x01, 0x91, 0x87, 0xAE, 0x00, 0x71, 0x89, 0xAE, 0x01, \
 0xB1, 0x93, 0xAE, 0x01, 0xB1, 0x97, 0xAE, 0x00, 0x31, 0x9B, 0xAE, 0x01, 0xA1, 0xCB, 0xAE, 0x00, 0xF1, 0xCB, 0xAE, 0x01, 0x21, 0xDB, 0xAE, 0x00, 0x21, 0xDD, 0xAE, \
 0x00, 0x61, 0xC3, 0xAE, 0x00, 0xD1, 0xDD, 0xAE, 0x01, 0x91, 0xC9, 0xAE, 0x00, 0x01, 0xD1, 0xAE, 0x00, 0xB1, 0xD1, 0xAE, 0x00, 0x31, 0xD7, 0xAE, 0x00, 0x00, 0x0B, \
 0xB0, 0x00, 0xF0, 0x0F, 0xB0, 0x00, 0xD1, 0xD9, 0xAE, 0x00, 0xD0, 0x01, 0xB0, 0x01, 0x10, 0x03, 0xB0, 0x00, 0xD0, 0x05, 0xB0, 0x00, 0x20, 0x05, 0xB0, 0x00, 0x90, \
 0x13, 0xB0, 0x00, 0xF0, 0x15, 0xB0, 0x01, 0x70, 0x17, 0xB0, 0x01, 0x00, 0x19, 0xB0, 0x00, 0x50, 0x19, 0xB0, 0x01, 0xB0, 0x5B, 0xB0, 0x00, 0x60, 0x5D, 0xB0, 0x01, \
 0xF0, 0x41, 0xB0, 0x00, 0x80, 0x47, 0xB0, 0x01, 0x40, 0x41, 0xB0, 0x01, 0xE0, 0x47, 0xB0, 0x01, 0x30, 0x47, 0xB0, 0x00, 0x00, 0x51, 0xB0, 0x00, 0xB0, 0x55, 0xB0, \
 0x01, 0x50, 0x4F, 0xB0, 0x00, 0xA0, 0x8D, 0xB0, 0x01, 0xF0, 0x8B, 0xB0, 0x00, 0x90, 0x9F, 0xB0, 0x01, 0x40, 0x9F, 0xB0, 0x01, 0x30, 0x85, 0xB0, 0x00, 0x90, 0x87, \
 0xB0, 0x01, 0xE0, 0x85, 0xB0, 0x01, 0x50, 0x99, 0xB0, 0x01, 0x30, 0xD1, 0xB0, 0x00, 0x80, 0xDF, 0xB0, 0x00, 0xC0, 0xC1, 0xB0, 0x00, 0x10, 0xC1, 0xB0, 0x00, 0x80, \
 0xC9, 0xB0, 0x01, 0xD0, 0xC7, 0xB0, 0x01, 0xE0, 0xC9, 0xB0, 0x01, 0x30, 0xC9, 0xB0, 0x00, 0x20, 0xD7, 0xB0, 0x01, 0x70, 0xD5, 0xB0, 0x01, 0xB0, 0xDD, 0xB0, 0x00, \
 0x71, 0x11, 0xB0, 0x01, 0xC1, 0x0F, 0xB0, 0x01, 0xD1, 0x07, 0xB0, 0x01, 0x81, 0x09, 0xB0, 0x00, 0xD1, 0x09, 0xB0, 0x00, 0x71, 0x15, 0xB0, 0x00, 0xC1, 0x17, 0xB0, \
 0x00, 0x21, 0x19, 0xB0, 0x01, 0x71, 0x17, 0xB0, 0x00, 0xA1, 0x4D, 0xB0, 0x00, 0x11, 0x51, 0xB0, 0x01, 0x61, 0x4F, 0xB0, 0x00, 0xC1, 0x49, 0xB0, 0x01, 0x71, 0x49, \
 0xB0, 0x01, 0x71, 0x83, 0xB0, 0x00, 0x21, 0x85, 0xB0, 0x00, 0x61, 0x55, 0xB0, 0x01, 0x81, 0xC3, 0xB0, 0x01, 0x31, 0xC1, 0xB0, 0x00, 0xF1, 0xCB, 0xB0, 0x01, 0xA1, \
 0xCB, 0xB0, 0x01, 0x81, 0xDD, 0xB0, 0x00, 0x31, 0xDF, 0xB0, 0x01, 0xD1, 0x8F, 0xB0, 0x01, 0x11, 0x93, 0xB0, 0x00, 0x70}

/*
sib11_seg_len_1246_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib11 : 
                {
                  messageIdentifier '00010001 00000000'B,
                  serialNumber '00000000 00000000'B,
                  warningMessageSegmentType lastSegment,
                  warningMessageSegmentNumber 0,
                  warningMessageSegment '00E8F60078013300000000000000000000 ...'H
                  dataCodingScheme '01'H
                }
            }
          }
      }
}
*/
#define DECLARE_SIB11_SEG_LEN_1246_8_5 \
static unsigned char sib11_seg_len_1246_8_5[] = {0x00, 0x25, 0x11, 0x00, 0x00, 0x00, 0x81, 0x09, 0xBC, 0x01, 0xD1, 0xEC, 0x00, 0xF0, 0x02, 0x66, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x19, 0x68, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x39, \
 0x68, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x59, 0x68, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x01, 0x79, 0x68, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x99, 0x68, 0x66, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xB9, 0x68, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xD9, 0x68, \
 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF9, 0x68, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x19, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x59, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0x6A, 0x66, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0xB9, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD9, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF9, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x19, 0x6A, 0x66, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x39, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x01, 0x59, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x79, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x99, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xB9, 0x6A, 0x66, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xD9, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, \
 0xF9, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x59, 0x6C, 0x66, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, \
 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB9, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0xD9, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF9, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x19, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x39, 0x6C, \
 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x59, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x01, 0x79, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x99, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xB9, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xD9, 0x6C, 0x66, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF9, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x19, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x59, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0x6E, 0x66, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0xB9, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD9, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF9, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x19, 0x6E, 0x66, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x39, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, \
 0x59, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x79, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x01, 0x99, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xB9, 0x6E, 0x66, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xD9, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF9, \
 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x70, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x39, 0x70, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x59, 0x70, 0x66, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0x70, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, 0x70, \
 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB9, 0x70, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xC2, 0x01, 0x04, 0x00, \
 0x00, 0x00, 0x00, 0x01, 0xF0, 0xE9, 0xD6, 0x00, 0xF0, 0x02, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}

/* Same as sib11_seg_len_1246_8_5 with segment longer by 1 byte */
#define DECLARE_SIB11_SEG_LEN_1247_8_5 \
static unsigned char sib11_seg_len_1247_8_5[] = {0x00, 0x24, 0x11, 0x00, 0x00, 0x00, 0x81, 0x09, 0xBE, 0x01, 0x7F, 0x4C, 0x02, 0xF0, 0x02, 0x66, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x19, 0x68, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x39, \
 0x68, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x59, 0x68, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x01, 0x79, 0x68, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x99, 0x68, 0x66, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xB9, 0x68, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xD9, 0x68, \
 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF9, 0x68, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x19, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x59, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0x6A, 0x66, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0xB9, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD9, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF9, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x19, 0x6A, 0x66, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x39, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x01, 0x59, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x79, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x99, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xB9, 0x6A, 0x66, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xD9, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, \
 0xF9, 0x6A, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x59, 0x6C, 0x66, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, \
 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB9, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0xD9, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF9, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x19, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x39, 0x6C, \
 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x59, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x01, 0x79, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x99, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xB9, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xD9, 0x6C, 0x66, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF9, 0x6C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x19, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x59, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0x6E, 0x66, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0xB9, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD9, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF9, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x19, 0x6E, 0x66, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x39, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, \
 0x59, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x79, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x01, 0x99, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xB9, 0x6E, 0x66, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xD9, 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF9, \
 0x6E, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x70, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x39, 0x70, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x59, 0x70, 0x66, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0x70, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, 0x70, \
 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB9, 0x70, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xC2, 0x01, 0x04, 0x00, \
 0x00, 0x00, 0x00, 0x01, 0xF0, 0xE9, 0xD6, 0x00, 0xF0, 0x02, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xCE}

/*
sib11_multiple_seg_8_5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib11 : 
                {
                  messageIdentifier '00010001 00000000'B,
                  serialNumber '00000000 00000000'B,
                  warningMessageSegmentType lastSegment,
                  warningMessageSegmentNumber 1,
                  warningMessageSegment '222222'H
                },
              sib11 : 
                {
                  messageIdentifier '00010001 00000000'B,
                  serialNumber '00000000 00000000'B,
                  warningMessageSegmentType notLastSegment,
                  warningMessageSegmentNumber 0,
                  warningMessageSegment '1111'H
                  dataCodingScheme '01'H
                }
            }
          }
      }
}
*/
#define DECLARE_SIB11_MULTIPLE_SEG_8_5 \
static unsigned char sib11_multiple_seg_8_5[] = {0x00, 0xA4, 0x11, 0x00, 0x00, 0x00, 0x82, 0x06, 0x44, 0x44, 0x44, 0x94, 0x44, 0x00, 0x00, 0x00, 0x00, 0x10, 0x88, 0x88, 0x08}

/*
SIB12_1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib12-v920 : 
                {
                  messageIdentifier-r9 '00001001 00001001'B,
                  serialNumber-r9 '00000001 00000000'B,
                  warningMessageSegmentType-r9 notLastSegment,
                  warningMessageSegmentNumber-r9 0,
                  warningMessageSegment-r9 '1000'H,
                  dataCodingScheme-r9 '01'H
                }
            }
          }
      }
}
*/

#define DECLARE_SIB12_SEG1OF2_8_5 \
static unsigned char sib12_seg1of2_8_5[] = {  \
  0x00, 0x40, 0x05, 0x20, 0x90, 0x90, 0x10, 0x00, 0x00, 0x42, 0x00, 0x00, 0x20, 0x00}

/*
SIB12_1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib12-v920 : 
                {
                  messageIdentifier-r9 '00001001 00001001'B,
                  serialNumber-r9 '00000001 00000000'B,
                  warningMessageSegmentType-r9 lastSegment,
                  warningMessageSegmentNumber-r9 1,
                  warningMessageSegment-r9 '0F00'H
                }
            }
          }
      }
}
*/

#define DECLARE_SIB12_SEG2OF2_8_5 \
static unsigned char sib12_seg2of2_8_5[] = {  \
  0x00, 0x40, 0x04, 0x80, 0x90, 0x90, 0x10, 0x08, 0x20, 0x41, 0xE0, 0x00, 0x00}

/*
SIB1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
            }
          },
          {
            si-Periodicity rf64,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf128,
            sib-MappingInfo 
            {
              sibType6
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/
#define DECLARE_SIB1_236_MCC_8_5 \
static unsigned char sib1_236_mcc_8_5[] = { \
0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x10, 0x03, 0x08, 0x20, 0x46, 0x44}

/*
SIB1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType4,
              sibType5
            }
          },
          {
            si-Periodicity rf32,
            sib-MappingInfo 
            {
              sibType10
            }
          },
          {
            si-Periodicity rf128,
            sib-MappingInfo 
            {
              sibType11
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/
#define DECLARE_SIB1_2345_10_11w128_MCC_8_5 \
static unsigned char sib1_2345_10_11w128_mcc_8_5[] = { \
0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x18, 0x08, 0x08, 0x82, 0x24, 0x13, 0xC0, 0xA0, 0x88}

#define DECLARE_SIB1_2345_10_11w64_MCC_8_5 \
static unsigned char sib1_2345_10_11w64_mcc_8_5[] = { \
0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x18, 0x08, 0x08, 0x82, 0x24, 0x13, 0xB0, 0xA0, 0x88}

/*
SIB1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType3
            }
          },
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType8
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/
#define DECLARE_SIB1_23_8_MCC_8_5 \
static unsigned char sib1_23_8_mcc_8_5[] = { \
0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x08, 0x08, 0x08, 0x4A, 0x44}


#define DECLARE_SIB1_23_16_MCC_8_5 \
static unsigned char sib1_23_16_mcc_8_5[] = { \
0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x08, 0x08, 0x08, 0x5A, 0x44}

/*
SIB12:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib12-v920 : 
                {
                  messageIdentifier-r9 '00000111 00000111'B,
                  serialNumber-r9 '00000100 00000111'B,
                  warningMessageSegmentType-r9 lastSegment,
                  warningMessageSegmentNumber-r9 0,
                  warningMessageSegment-r9 ''H
                }
            }
          }
      }
}
*/
#define DECLARE_SIB12_BLANK_8_5 \
static unsigned char sib12_blank_8_5[] = { \
0x00, 0x40, 0x03, 0x80, 0x70, 0x70, 0x40, 0x78, 0x00, 0x00, 0x00}

/*
SIB1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  5,
                  6
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
            }
          },
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType13-v920
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 5
      }
}
*/
#define DECLARE_SIB1_WITH_SIB13_8_5 \
static unsigned char sib1_with_sib13_8_5[] = {0x40, 0x44, 0x8E, 0x8A, 0xC0, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, \
0x83, 0x08, 0x01, 0x0A, 0x89, 0x40}

/*Value tag as 2*/
#define DECLARE_SIB1_WITH_SIB13_8_VT2 \
static unsigned char sib1_with_sib13_8_VT2[] = {0x40, 0x44, 0x8E, 0x8A, 0xC0, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, \
0x83, 0x08, 0x01, 0x0A, 0x88, 0x80}

/*

sib1_with13_15:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  5,
                  6
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
            }
          },
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType13-v920
            }
          },
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
              sibType15-v11x0
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 5
      }
}
*/
#define DECLARE_SIB1_WITH_SIB13_and_15 \
static unsigned char sib1_with_sib13_and_15[] = {0x40, 0x44, 0x8E, 0x8A, 0xC0, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, \
0x83, 0x10, 0x01, 0x0A, 0x80, 0x58, 0x4A}


/*
SIB13:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib13-v920 : 
                {
                  mbsfn-AreaInfoList-r9 
                  {
                    {
                      mbsfn-AreaId-r9 100,
                      non-MBSFNregionLength s2,
                      notificationIndicator-r9 1,
                      mcch-Config-r9 
                      {
                        mcch-RepetitionPeriod-r9 rf128,
                        mcch-Offset-r9 4,
                        mcch-ModificationPeriod-r9 rf1024,
                        sf-AllocInfo-r9 '111111'B,
                        signallingMCS-r9 n7
                      }
                    },
                    {
                      mbsfn-AreaId-r9 101,
                      non-MBSFNregionLength s1,
                      notificationIndicator-r9 7,
                      mcch-Config-r9 
                      {
                        mcch-RepetitionPeriod-r9 rf256,
                        mcch-Offset-r9 7,
                        mcch-ModificationPeriod-r9 rf512,
                        sf-AllocInfo-r9 '111100'B,
                        signallingMCS-r9 n19
                      }
                    }
                  },
                  notificationConfig-r9 
                  {
                    notificationRepetitionCoeff-r9 n4,
                    notificationOffset-r9 5,
                    notificationSF-Index-r9 5
                  }
                }
            }
          }
      }
} 
*/
#define DECLARE_SIB13_8_5 \
static unsigned char sib13_8_5[] = {0x00, 0x40, 0x84, 0x84, 0xC9, 0x32, 0x7F, 0x4C, 0xAF, 0xBB, 0xCE, 0xB0, 0x00}

/*

SIB15_earfcn400:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib15-v11x0 : 
                {
                  sai-IntraFreq-r11 
                  {
                    0,
                    1,
                    2
                  },
                  sai-InterFreqList-r11 
                  {
                    {
                      dl-CarrierFreq 300,
                      sai-List-r11 
                      {
                        1,
                        2,
                        3
                      }
                    },
                    {
                      dl-CarrierFreq 350,
                      sai-List-r11 
                      {
                        2,
                        3,
                        4
                      }
                    }
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_SIB15_8_EFCN400 \
static unsigned char sib15_8_efcn400[] = {0x00, 0x41, 0x8D, 0x30, 0x40, 0x00, 0x00, 0x00, 0x20, 0x00, 0x44, \
                                          0x02, 0x58, 0x10, 0x00, 0x08, 0x00, 0x10, 0x00, 0x18, 0x05, 0x78, \
                                          0x20, 0x00, 0x20, 0x00, 0x30, 0x00, 0x40, 0x00}

#define DECLARE_SIB15_8_5 \
static unsigned char sib15_8_5[] = {0x00, 0x41, 0x88, 0xB0, 0x40, 0x00, 0x00, 0x00, 0x20, 0x00, 0x40, 0x01, 0x2C, \
                                    0x08, 0x00, 0x08, 0x00, 0x0C, 0x00, 0x10, 0x00}

/*
sib1_with_sib2:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  4,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/
#define DECLARE_SIB1_2_MCC_8_5 \
static unsigned char sib1_2_mcc_8_5[] = {0x40, 0x44, 0x8E, 0x88, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0xA2, 0x83, 0x00, 0x01, 0x10}

/*
SIB2:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              <unknown>,
              sib2 : 
                {
                  radioResourceConfigCommon 
                  {
                    rach-Config 
                    {
                      preambleInfo 
                      {
                        numberOfRA-Preambles n64
                     },
                      powerRampingParameters 
                      {
                        powerRampingStep dB2,
                        preambleInitialReceivedTargetPower dBm-90
                      },
                      ra-SupervisionInfo 
                      {
                        preambleTransMax n5,
                        ra-ResponseWindowSize sf10,
                        mac-ContentionResolutionTimer sf64
                      },
                      maxHARQ-Msg3Tx 4
                    },
                    bcch-Config 
                    {
                      modificationPeriodCoeff n4
                    },
                    pcch-Config 
                    {
                      defaultPagingCycle rf32,
                      nB oneT
                    },
                    prach-Config 
                    {
                      rootSequenceIndex 0,
                      prach-ConfigInfo 
                      {
                        prach-ConfigIndex 12,
                        highSpeedFlag FALSE,
                        zeroCorrelationZoneConfig 10,
                        prach-FreqOffset 2
                      }
                    },
                    pdsch-Config 
                    {
                      referenceSignalPower -43,
                      p-b 1
                    },
                    pusch-Config 
                    {
                      pusch-ConfigBasic 
                      {
                        n-SB 1,
                        hoppingMode interSubFrame,
                        pusch-HoppingOffset 0,
                        enable64QAM FALSE
                      },
                      ul-ReferenceSignalsPUSCH 
                      {
                        groupHoppingEnabled TRUE,
                        groupAssignmentPUSCH 0,
                        sequenceHoppingEnabled FALSE,
                        cyclicShift 0
                      }
                    },
                    pucch-Config 
                    {
                      deltaPUCCH-Shift ds1,
                      nRB-CQI 2,
                      nCS-AN 0,
                      n1PUCCH-AN 36
                    },
                    soundingRS-UL-Config release : NULL,
                    uplinkPowerControl 
                    {
                      p0-NominalPUSCH -108,
                      alpha al1,
                      p0-NominalPUCCH -96,
                      deltaFList-PUCCH 
                      {
                        deltaF-PUCCH-Format1 deltaF2,
                        deltaF-PUCCH-Format1b deltaF5,
                        deltaF-PUCCH-Format2 deltaF2,
                        deltaF-PUCCH-Format2a deltaF2,
                        deltaF-PUCCH-Format2b deltaF2
                      },
                      deltaPreambleMsg3 3
                    },
                    ul-CyclicPrefixLength len1
                  },
                  ue-TimersAndConstants 
                  {
                    t300 ms2000,
                    t301 ms600,
                    t310 ms2000,
                    n310 n20,
                    t311 ms1000,
                    n311 n4
                  },
                  freqInfo 
                  {
                    ul-CarrierFreq 23230,
                    ul-Bandwidth n50,
                    additionalSpectrumEmission 1
                  },
                  mbsfn-SubframeConfigList 
                  {
                    {
                      radioframeAllocationPeriod n1,
                     radioframeAllocationOffset 0,
                      subframeAllocation oneFrame : '111111'B
                    }
                  },
                  timeAlignmentTimerCommon infinity
                }
            }
          }
      }
}
*/
#define DECLARE_SI_NONCOMPREHENDED_8_5 \
static unsigned char si_noncomprehended_8_5[] = {0x00, 0xc0, 0x83, 0x00, 0x01, 0x00, 0x20, 0xa0, 0x00, 0x00, \
0x8f, 0x7c, 0xbf, 0x68, 0x80, 0x03, 0x14, 0x08, 0x8a, 0x00, 0x20, 0x00, 0x10, 0x04, 0x81, 0x2f, 0xfa, 0xea, \
0x1e, 0x6e, 0x1e, 0xb5, 0x7c, 0xc0, 0x00, 0x7f, 0xc0}

/*
value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              <unknown>
            }
          }
      }
}
*/

#define DECLARE_SIB_NOT_SUPPORTED_8_5 \
static unsigned char sib_not_supported_8_5[] = {0x00, 0x5E, 0x1D, 0xB1, 0x7C, 0xF2, 0xA6, 0x21, 0xB4, 0x58, 0x43, \
0x20, 0x5C, 0x41, 0x9E, 0xB9, 0x96, 0x59, 0x94, 0x11, 0x64, 0x6A, 0xCB, 0xD5, 0x7A, 0xA1, 0xF6, 0xC7, 0xDC, \
0x5A, 0x5A, 0xCD, 0x26, 0xED, 0x52, 0xCA, 0xDA, 0xC6, 0x8E, 0x58, 0x10, 0x77, 0x40, 0xE9, 0x61, 0x03, 0xE6, \
0x80, 0x21, 0x93, 0xC4, 0x37, 0x86, 0x32, 0x57, 0xC5, 0x56, 0x03, 0x5F, 0x0A, 0xB8, 0x38, 0x3F, 0x95, 0x0E, \
0x2F, 0xA0, 0x3F, 0x52, 0x11, 0x7A, 0xB3, 0x1E, 0xFE, 0x19, 0x44, 0x21, 0x92, 0x01, 0x26, 0xD7, 0xF2, 0x2A, \
0x94, 0x5B, 0x5C, 0xA5, 0x97, 0x25, 0xED, 0x99, 0x01, 0xC9, 0xBC, 0x96, 0x81, 0xC7, 0x29, 0x14, 0xB3, 0x91, \
0xD9, 0x70, 0x37, 0xB8, 0x55, 0x4B, 0xB7, 0xF4, 0xC4, 0x32, 0x91, 0x93, 0xE9, 0x60, 0xE3, 0xB4, 0x9D, 0xEE, \
0x2B, 0xAF, 0xA2, 0x15, 0xD9, 0x34, 0x37, 0x87, 0x2F, 0x7B, 0xCB, 0xEE, 0x61, 0xA8, 0xFC, 0x1C, 0x8B, 0x32, \
0x61, 0xD4, 0x17, 0xD6, 0x1A, 0x5B, 0xDA, 0xA5, 0xAB, 0xDB, 0x6A, 0x90, 0x39, 0x7E, 0xF7, 0x7A, 0x72, 0xBE, \
0x75, 0x4A, 0x57, 0x59, 0x38, 0xDC, 0x17, 0xE9, 0xA0, 0x1D, 0x03, 0xA4, 0xAA, 0x0B, 0x1C, 0x04, 0xCA, 0x09, \
0x99, 0x03, 0x51, 0x7C, 0xEA, 0x83, 0xCE, 0xBF, 0x4F, 0x94, 0x82, 0x32, 0x2B, 0x35, 0x61, 0x3C, 0x14, 0xC6, \
0x29, 0x69, 0xDF, 0xBD, 0xB6, 0x51, 0xC8, 0xDE, 0x75, 0x3E, 0x3E, 0xE2, 0xA1, 0x15, 0x6D, 0x35, 0x55, 0xDD, \
0xAD, 0xFB, 0x93, 0xE1, 0x71, 0x2F, 0xF8, 0x40, 0xEE, 0x88, 0x19, 0x6D, 0xBC, 0x23, 0xD7, 0x02, 0x2D, 0x55, \
0xF2, 0x5F, 0x54, 0x67, 0x7F, 0x01, 0xF5, 0x99, 0x51, 0x4E, 0x67, 0x62, 0xE5, 0xE4, 0x0E}

/*

sib1_with9:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformationBlockType1 : 
      {
        cellAccessRelatedInfo 
        {
          plmn-IdentityList 
          {
            {
              plmn-Identity 
              {
                mcc 
                {
                  1,
                  2,
                  3
                },
                mnc 
                {
                  4,
                  5,
                  6
                }
              },
              cellReservedForOperatorUse reserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000100 00000000 00000000 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60
        },
        freqBandIndicator 4,
        schedulingInfoList 
        {
          {
            si-Periodicity rf8,
            sib-MappingInfo 
            {
            }
          },
          {
            si-Periodicity rf16,
            sib-MappingInfo 
            {
              sibType9
            }
          }
        },
        si-WindowLength ms2,
        systemInfoValueTag 5
      }
}
*/
#define DECLARE_SIB1_WITH_SIB9 \
static unsigned char sib1_with_sib9[] = {0x40, 0x44, 0x8E, 0x8A, 0xC0, 0x00, 0x00, 0x40, 0x00, \
                                         0x00, 0xA2, 0x83, 0x08, 0x01, 0x09, 0x89, 0x40}
/*

sib9_sample1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib9 : 
                {
                }
            }
          }
      }
}
*/
#define DECLARE_SIB9_NO_INFO \
static unsigned char sib9_no_info[] = {0x00, 0x1C}


/*

sib_16_sample:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib16-v11x0 : 
                {
                  timeInfo timeInfoUTC : '00000001 00000010 00000000 00000000 000 ...'B,
                  dayLightSavingTime '00'B,
                  leapSeconds 2,
                  localTimeOffset 3
                }
            }
          }
      }
}
*/
#define DECLARE_SIB16_SAMPLE_1 \
static unsigned char sib16_sample_1[] = { 0x00, 0x42, 0x03, 0xA3, 0x4D, 0x40, 0x62, 0x78, 0x09, 0xE8, 0x00, 0x00, 0x00 }

//static unsigned char sib16_sample_1[] = {0x00, 0x42, 0x04, 0x7C, 0x04, 0x08, 0x00, 0x00, 0x01, 0x03, 0x08, 0x00, }
/*
sib16_null_data_2:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib16-v11x0 : 
                {
                  timeInfo timeInfoUTC : '00000000 00000000 00000000 00000000 000 ...'B
                }
            }
          }
      }
}

*/

#define DECLARE_SIB16_NULL_INFO \
static unsigned char sib16_null_info[] = {0x00, 0x42, 0x03, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern void lte_rrc_sib_test_stub_init();

extern void lte_rrc_sib_test_send_stopped_indi();

extern void lte_rrc_sib_test_send_deact_req ();

extern void lte_rrc_sib_test_send_abort_reqi(lte_phy_cell_id_t phy_cell_id,
                                             lte_earfcn_t freq);

extern void lte_rrc_sib_test_send_camped_indi(lte_phy_cell_id_t phy_cell_id,
                                              lte_earfcn_t freq);

extern void lte_rrc_sib_test_send_connected_indi();

extern void lte_rrc_sib_test_send_mib(uint8 num_tx_antennas,
                                      lte_bandwidth_e dl_bandwidth,
                                      lte_l1_phich_duration_e phich_duration,
                                      lte_l1_phich_resource_e phich_resource,
                                      lte_phy_cell_id_t phy_cell_id,
                                      lte_earfcn_t freq,
                                      uint16 sfn);

extern void lte_rrc_sib_test_send_bcch_dl_sch_msg(unsigned char *msg,
                                                  uint16 msg_size,
                                                  lte_phy_cell_id_t phy_cell_id,
                                                  lte_earfcn_t freq,
                                                  uint16 sfn);

extern void lte_rrc_sib_test_send_get_sibs_reqi(lte_phy_cell_id_t phy_cell_id,
                                                lte_earfcn_t freq,
                                                lte_rrc_proc_e req_proc,
                                                lte_rrc_get_sibs_cause_e cause,
                                                boolean cnf_needed,
                                                lte_l1_sfn_t sfn);

extern void lte_rrc_sib_test_send_ho_started_indi();

extern void lte_rrc_sib_test_send_tdd_cfg_cnf(lte_phy_cell_id_t phy_cell_id,
                                              lte_earfcn_t freq,
                                              errno_enum_type status);

extern void lte_rrc_sib_test_send_utc_time_update_cnf(errno_enum_type status);

extern void lte_rrc_sib_test_send_service_ind();

extern lte_cphy_sib_sched_req_s* lte_rrc_sib_test_rcv_sib_sched_req(lte_phy_cell_id_t phy_cell_id,
                                                                    lte_earfcn_t freq,
                                                                    lte_rrc_sib_bitmask_t curr_mod_prd_sib_mask,
                                                                    lte_l1_sfn_t mod_period_boundary_sfn,
                                                                    lte_rrc_sib_bitmask_t next_mod_prd_sib_mask,
                                                                    lte_l1_sfn_t last_sfn_in_sib_rsp,
                                                                    lte_cphy_si_window_length_e si_window_length,
                                                                    uint8 num_si_msgs);

extern void lte_rrc_sib_test_rcv_get_sibs_cnfi(lte_phy_cell_id_t phy_cell_id,
                                               lte_earfcn_t freq,
                                               lte_rrc_sib_db_key_t sib_db_key,
                                               lte_rrc_get_sibs_status_e status);

extern void lte_rrc_sib_test_rcv_sib_updated_indi(lte_phy_cell_id_t phy_cell_id,
                                                  lte_earfcn_t freq,
                                                  lte_rrc_sib_bitmask_t sibs_changed_bitmask,
                                                  lte_rrc_sib_bitmask_t sibs_scheduled_bitmask);

extern void lte_rrc_sib_test_rcv_sib_read_error_indi(lte_phy_cell_id_t phy_cell_id,
                                                     lte_earfcn_t freq,
                                                     lte_rrc_sib_read_error_cause_e cause,
                                                     lte_rrc_sib_bitmask_t sibs_rcvd_curr_bitmask,
                                                     boolean csg_indication,
                                                     boolean intra_freq_resel_allowed);

extern void lte_rrc_sib_test_rcv_revoke_reqi();

extern void lte_rrc_sib_test_rcv_get_sibs_reqi();

extern void lte_rrc_sib_test_camp_phase1();

extern void lte_rrc_sib_test_camp_phase2();

extern void lte_rrc_sib_test_camp_phase3();

extern void lte_rrc_sib_test_camp_phase4();

extern void lte_rrc_sib_test_camp_phase5();

extern void lte_rrc_sib_test_camp_phase6();

extern void lte_rrc_sib_test_camp_phase1_asserts();

extern void lte_rrc_sib_test_camp_phase2_asserts();

extern void lte_rrc_sib_test_camp_phase3_asserts();

extern void lte_rrc_sib_test_camp_phase4_asserts();

extern void lte_rrc_sib_test_camp_phase5_asserts();

extern void lte_rrc_sib_test_camp_phase6_asserts();

extern void lte_rrc_sib_test_stub_send_sib8_unavailable_to_1x();

extern uint16 lte_rrc_sib_test_paging_get_ue_specific_paging_cycle(void);

extern void lte_rrc_sib_test_send_rf_unavail_ind();
extern void lte_rrc_sib_test_send_rf_avail_ind();

extern void lte_rrc_sib_test_send_extnd_tmr_reqi
(
  uint32 time_duration
);

#endif /* LTE_RRC_SIB_TEST_COMMON_H */
