/*!
  @file lte_rrc_llc_ota_msg.cpp

  @brief
  This file contains all the OTA messages for LLC used in the unit test cases.

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

$Header: //source/qcom/qct/modem/lte/RRC/main/latest/test/llc/lte_rrc_llc_ota_msg.cpp

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/09/09   mm     Initial Revision 
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "lte.h"
}
/*===========================================================================

                    OTA Message Definitions in encoded form

===========================================================================*/

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
                    rach-ConfigCommon 
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
                        highSpeedFlag TRUE,
                        zeroCorrelationZoneConfig 0,
                        prach-FreqOffset 10
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
                    pucch-ConfigCommon 
                    {
                      deltaPUCCH-Shift ds3,
                      nRB-CQI 98,
                      nCS-AN 6,
                      n1PUCCH-AN 1023
                    },
                    soundingRS-UL-ConfigCommon setup : 
                      {
                        srs-BandwidthConfig bw2,
                        srs-SubframeConfig sc3,
                        ackNackSRS-SimultaneousTransmission FALSE
                      },
                    uplinkPowerControlCommon 
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
byte llc_basic_sib2_2[27] = {0x00, 0x00, 0x12, 0x10, 0x11, 0x07, 0x12, 0x00, 0x0C, 0x58, 0x0A, 0x79, 0x56, 0x88, 0xBA, 0xC5, 0x9F, 0xFC, 0x8D, 0x06, 0xBE, 0x33, 0x00, 0x06, 0x00, 0x00, 0x00};

/*
Invalid_sib2:value BCCH-DL-SCH-Message ::= 
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
                    rach-ConfigCommon 
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
                        prach-FreqOffset 10
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
                    pucch-ConfigCommon 
                    {
                      deltaPUCCH-Shift ds3,
                      nRB-CQI 98,
                      nCS-AN 6,
                      n1PUCCH-AN 1023
                    },
                    soundingRS-UL-ConfigCommon setup : 
                      {
                        srs-BandwidthConfig bw2,
                        srs-SubframeConfig sc3,
                        ackNackSRS-SimultaneousTransmission FALSE
                      },
                    uplinkPowerControlCommon 
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
                    n310 n1,
                    t311 ms1000,
                    n311 n1
                  },
                  freqInfo 
                  {
                    ul-CarrierFreq 18501,
                    additionalSpectrumEmission 1
                  },
                  timeAlignmentTimerCommon sf500
                }
            }
          }
      }
}
*/
byte llc_invalid_sib2_2[29] = {0x00, 0x00, 0x12, 0x10, 0x11, 0x07, 0x12, 0x00, 0x0C, 0x50, 0x0A, 0x79, 0x56, 0x88, 0xBA, 0xC5, 0x9F, 0xFC, 0x8D, 0x06, 0xBE, 0x33, 0x00, 0x06, 0x00, 0x92, 0x11, 0x40, 0x00};

/*
invalid_sib5:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib5 : 
                {
                  interFreqCarrierFreqList 
                  {
                    {
                      dl-CarrierFreq 11376,
                      q-RxLevMin -23,
                      p-Max -30,
                      t-ReselectionEUTRA 7,
                      t-ReselectionEUTRA-SF 
                      {
                        sf-Medium oDot75,
                        sf-High oDot75
                      },
                      threshX-High 10,
                      threshX-Low 10,
                      allowedMeasBandwidth mbw15,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 6,
                      neighCellConfig '11'B,
                      q-OffsetFreq dB-6,
                      interFreqNeighCellList 
                      {
                        {
                          physCellId 2,
                          q-OffsetCell dB-20
                        },
                        {
                          physCellId 3,
                          q-OffsetCell dB-18
                        },
                        {
                          physCellId 6,
                          q-OffsetCell dB-14
                        }
                      },
                      interFreqBlackCellList 
                      {
                        {
                          start 1
                        },
                        {
                          start 5
                        },
                        {
                          start 8
                        }
                      }
                    },
                    {
                      dl-CarrierFreq 256,
                      q-RxLevMin -23,
                      p-Max -30,
                      t-ReselectionEUTRA 7,
                      threshX-High 10,
                      threshX-Low 10,
                      allowedMeasBandwidth mbw15,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 6,
                      neighCellConfig '00'B,
                      q-OffsetFreq dB-6
                    }
                  }
                }
            }
          }
      }
}
*/

byte llc_invalid_sib5[29] = {0x00, 0x0C, 0x5F, 0x96, 0x38, 0x5E, 0x07, 0xA5, 0x28, 0xF6, 0x92, 0x01, 0x08, 0x06, 
   0x30, 0x31, 0x48, 0x01, 0x01, 0x40, 0x85, 0x80, 0x20, 0x17, 0x81, 0xD4, 0xA3, 0xC2, 0x40};



/*===========================================================================
              SIB 2 message with MBSFN Config and SRS Config absent
===========================================================================*/
/*
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
                        numberOfRA-Preambles n4,
                        preamblesGroupAConfig 
                        {
                          sizeOfRA-PreamblesGroupA n4,
                          messageSizeGroupA b56,
                          messagePowerOffsetGroupB minusinfinity
                        }
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
                    soundingRS-UL-Config setup : 
                      {
                        srs-BandwidthConfig bw0,
                        srs-SubframeConfig sc0,
                        ackNackSRS-SimultaneousTransmission FALSE,
                        srs-MaxUpPts true
                      },
                    uplinkPowerControl 
                    {
                      p0-NominalPUSCH 1,
                      alpha al0,
                      p0-NominalPUCCH -96,
                      deltaFList-PUCCH 
                      {
                        deltaF-PUCCH-Format1 deltaF-2,
                        deltaF-PUCCH-Format1b deltaF1,
                        deltaF-PUCCH-Format2 deltaF-2,
                        deltaF-PUCCH-Format2a deltaF-2,
                        deltaF-PUCCH-Format2b deltaF-2
                      },
                      deltaPreambleMsg3 -1
                    },
                    ul-CyclicPrefixLength len1
                  },
                  ue-TimersAndConstants 
                  {
                    t300 ms100,
                    t301 ms100,
                    t310 ms0,
                    n310 n3,
                    t311 ms1000,
                    n311 n4
                  },
                  freqInfo 
                  {
                    additionalSpectrumEmission 5
                  },
                  mbsfn-SubframeConfigList 
                  {
                    {
                      radioframeAllocationPeriod n8,
                      radioframeAllocationOffset 1,
                      subframeAllocation oneFrame : '110000'B
                    }
                  },
                  timeAlignmentTimerCommon sf500
                }
            }
          }
      }
}
*/

byte llc_sib2_with_mbsfn[29] = {0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x12, 0x00, 
  0x0C, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0xFE, 0x3E, 
  0x00, 0x00, 0x00, 0x83, 0x08, 0x19, 0x18, 0x00};

/*===========================================================================
                     Invalid SIB2 with spare delta shift
===========================================================================*/

/*sib2:value BCCH-DL-SCH-Message ::= 
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
                      nCS-AN 5,
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
                    ul-Bandwidth n100,
                    additionalSpectrumEmission 5
                  },
                  timeAlignmentTimerCommon sf10240
                }
            }
          }
      }
}
*/

byte llc_invalid_sib2_spare_delta_field[29] = {0x00, 0x00, 0x12, 0x10, 0x11, 0x07, 0x12, 0x00, 0x0C, 0x50, 0x5E, 0x79, 0x56, 0x88, 0xBA, 0xC5, 0x5F, 0xFE, 0x8D, 0x06, 0xBE, 0x33, 0x00, 0x06, 0x83, \
0xE3, 0x59, 0xE9, 0x30,};


/*===========================================================================
                     Basic SIB 1 
===========================================================================*/
/*
Sib1:value BCCH-DL-SCH-Message ::= 
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
                  4,
                  4
                }
              },
              cellReservedForOperatorUse notReserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000001 00000001 00000001 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication FALSE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60,
          q-RxLevMinOffset 7
        },
        p-Max -12,
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
        systemInfoValueTag 2
      }
}
*/

byte llc_basic_sib1_1[15] = {0x60, 0x28, 0x89, 0x00, 0x00, 0x01, 0x01, 0x01, 
  0x09, 0x2B, 0x24, 0x18, 0x08, 0x40, 0x44};

/*
sib1:value BCCH-DL-SCH-Message ::= 
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
                  4,
                  4
                }
              },
              cellReservedForOperatorUse notReserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000001 00000001 00000001 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication FALSE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60,
          q-RxLevMinOffset 7
        },
        p-Max -12,
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
        tdd-Config 
        {
          subframeAssignment sa6,
          specialSubframePatterns ssp5
        },
        si-WindowLength ms2,
        systemInfoValueTag 2,
        nonCriticalExtension 
        {
          nonCriticalExtension 
          {
            nonCriticalExtension 
            {
              tdd-Config-v1130 
              {
                specialSubframePatterns-v1130 ssp9
              }
            }
          }
        }
      }
}

*/

byte llc_basic_sib1_ssf9[17] = {0x78, 0x28, 0x89, 0x00, 0x00, 0x01, 0x01, 0x01, 
   0x09, 0x2B, 0x24, 0x18, 0x08, 0x41, 0x94, 0x89, 0x32};

/*
sib1:value BCCH-DL-SCH-Message ::= 
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
                  4,
                  4
                }
              },
              cellReservedForOperatorUse notReserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000001 00000001 00000001 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication FALSE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60,
          q-RxLevMinOffset 7
        },
        p-Max -12,
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
        tdd-Config 
        {
          subframeAssignment sa6,
          specialSubframePatterns ssp5
        },
        si-WindowLength ms2,
        systemInfoValueTag 2,
        nonCriticalExtension 
        {
          nonCriticalExtension 
          {
            nonCriticalExtension 
            {
            }
          }
        }
      }
}

*/

 byte llc_basic_sib1_ssf9_1[17] = {0x78, 0x28, 0x89, 0x00, 0x00, 0x01, 0x01, 0x01, 0x09, 
    0x2B, 0x24, 0x18, 0x08, 0x41, 0x94, 0x89, 0x20
};
/*===========================================================================
                     SIB 1 for TDD
===========================================================================*/
/*
Sib1:value BCCH-DL-SCH-Message ::= 
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
                  4,
                  4
                }
              },
              cellReservedForOperatorUse notReserved
            }
          },
          trackingAreaCode '00000000 00000000'B,
          cellIdentity '00000001 00000001 00000001 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication FALSE
        },
        cellSelectionInfo 
        {
          q-RxLevMin -60,
          q-RxLevMinOffset 7
        },
        p-Max -12,
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
        tdd-Config 
        {
          subframeAssignment sa3,
          specialSubframePatterns ssp4
        },
        si-WindowLength ms2,
        systemInfoValueTag 2
      }
}
*/

byte llc_sib1_tdd[16] = {0x70, 0x28, 0x89, 0x00, 0x00, 0x01, 0x01, 0x01, 
  0x09, 0x2B, 0x24, 0x18, 0x08, 0x40, 0xD0, 0x88};


/*===========================================================================
                            Connection Setup Message
===========================================================================*/
/*
ConnSetup:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
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
                            t-PollRetransmit ms5,
                            pollPDU p4,
                            pollByte kB25,
                            maxRetxThreshold t1
                          },
                          dl-AM-RLC 
                          {
                            t-Reordering ms5,
                            t-StatusProhibit ms5
                          }
                        },
                    logicalChannelConfig explicitValue : 
                      {
                        ul-SpecificParameters 
                        {
                          priority 5,
                          prioritisedBitRate infinity,
                          bucketSizeDuration ms50,
                          logicalChannelGroup 1
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
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition release : NULL,
                    tdd-AckNackFeedbackMode bundling
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en0,
                    accumulationEnabled FALSE,
                    p0-UE-PUCCH 0,
                    pSRS-Offset 0
                  },
                  tpc-PDCCH-ConfigPUCCH release : NULL,
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  cqi-ReportConfig 
                  {
                    nomPDSCH-RS-EPRE-Offset 2,
                    cqi-ReportPeriodic setup : 
                      {
                        cqi-PUCCH-ResourceIndex 1154,
                        cqi-pmi-ConfigIndex 525,
                        cqi-FormatIndicatorPeriodic subbandCQI : 
                          {
                            k 3
                          },
                        ri-ConfigIndex 537,
                        simultaneousAckNackAndCQI TRUE
                      }
                  },
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw0,
                      srs-HoppingBandwidth hbw0,
                      freqDomainPosition 0,
                      duration FALSE,
                      srs-ConfigIndex 0,
                      transmissionComb 0,
                      cyclicShift cs0
                    },
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm1,
                      codebookSubsetRestriction n2TxAntenna-tm3 : '00'B,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    }
                }
              }
            }
      }
}
*/

byte llc_conn_setup_msg[26] = {0x68, 0x12, 0x98, 0x00, 0x00, 0x02, 0x09, 0xA3, 
  0x86, 0xDF, 0xC4, 0x41, 0x00, 0x2F, 0x90, 0x50, 0x6E, 0x86, 0x70, 0x00, 0x00, 
  0x04, 0x02, 0x80, 0x00, 0x00};

/*===========================================================================
                   Connection Setup Message - With Default IEs
===========================================================================*/
/*
ConnSetup:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                mac-MainConfig defaultValue : NULL,
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL release : NULL,
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf20,
                      implicitReleaseAfter e2,
                      p0-Persistent 
                      {
                        p0-NominalPUSCH-Persistent 5,
                        p0-UE-PUSCH-Persistent 5
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB2
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition release : NULL
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en1,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 2,
                    pSRS-Offset 3
                  },
                  tpc-PDCCH-ConfigPUCCH release : NULL,
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm30,
                    nomPDSCH-RS-EPRE-Offset 0
                  },
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw0,
                      srs-HoppingBandwidth hbw0,
                      freqDomainPosition 0,
                      duration TRUE,
                      srs-ConfigIndex 0,
                      transmissionComb 1,
                      cyclicShift cs0
                    },
                  antennaInfo defaultValue : NULL,
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    }
                }
              }
            }
      }
}
*/

byte llc_conn_setup_default_ies[20] = {0x68, 0x13, 0x9B, 0xF0, 0x10, 0x05, 0x09, 
  0x07, 0xAD, 0xFF, 0x08, 0xE8, 0xC9, 0x98, 0x02, 0x00, 0x46, 0x00, 0x00, 0x00};
/*
conn_setup1:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                mac-MainConfig defaultValue : NULL,
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL release : NULL,
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf20,
                      implicitReleaseAfter e2,
                      p0-Persistent 
                      {
                        p0-NominalPUSCH-Persistent 5,
                        p0-UE-PUSCH-Persistent 5
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB2
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition release : NULL
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en1,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 2,
                    pSRS-Offset 3
                  },
                  tpc-PDCCH-ConfigPUCCH release : NULL,
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw0,
                      srs-HoppingBandwidth hbw0,
                      freqDomainPosition 0,
                      duration TRUE,
                      srs-ConfigIndex 0,
                      transmissionComb 1,
                      cyclicShift cs0
                    },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    },
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm1,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-pmi-ConfigIndex 0,
                        cqi-FormatIndicatorPeriodic-r10 subbandCQI-r10 : 
                          {
                            k 4,
                            periodicityFactor-r10 n2
                          },
                        simultaneousAckNackAndCQI FALSE
                      }
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an1,
                        resourceConfig-r10 0,
                        subframeConfig-r10 0,
                        p-C-r10 0
                      }
                  }
                }
              }
            }
      }
}
*/
byte llc_conn_setup_with_tm1_csirs_enabled_antport_1_cqi_subband[31] = {
0x68, 0x13, 0x9B, 0xF0, 0x10, 0x05, 0x09, 0x07, 0xBD, 0xD7, 0x08, 0xE8, 0xC8, 0x02, 
   0x00, 0x44, 0x00, 0x00, 0x00, 0x48, 0x2A, 0x60, 0x01, 0x21, 0x80, 0x00, 0x00, 0x39, 
   0x40, 0x00, 0x20
};

/*
conn_setup1:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                mac-MainConfig defaultValue : NULL,
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL release : NULL,
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf20,
                      implicitReleaseAfter e2,
                      p0-Persistent 
                      {
                        p0-NominalPUSCH-Persistent 5,
                        p0-UE-PUSCH-Persistent 5
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB2
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition release : NULL
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en1,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 2,
                    pSRS-Offset 3
                  },
                  tpc-PDCCH-ConfigPUCCH release : NULL,
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    },
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm1,
                      ue-TransmitAntennaSelection setup : closedLoop
                    }
                }
              }
            }
      }
}

*/
byte llc_conn_setup_with_r10_antenna_selection[21] = 
{
0x68, 0x13, 0x9B, 0xF0, 0x10, 0x05, 0x09, 0x07, 0xBD, 0xC7, 0x08, 
0xE8, 0xC8, 0x00, 0x00, 0x00, 0x90, 0x1C, 0x00, 0x02, 0x00
};

/*
conn_setup1:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                mac-MainConfig defaultValue : NULL,
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL release : NULL,
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf20,
                      implicitReleaseAfter e2,
                      p0-Persistent 
                      {
                        p0-NominalPUSCH-Persistent 5,
                        p0-UE-PUSCH-Persistent 5
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB2
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition release : NULL
                  },
                  tpc-PDCCH-ConfigPUCCH release : NULL,
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  soundingRS-UL-ConfigDedicatedAperiodic-r10 setup : 
                    {
                      srs-ConfigIndexAp-r10 13,
                      srs-ActivateAp-r10 setup : 
                        {
                          srs-ConfigApDCI-Format0-r10 
                          {
                            srs-AntennaPortAp-r10 an1,
                            srs-BandwidthAp-r10 bw0,
                            freqDomainPositionAp-r10 0,
                            transmissionCombAp-r10 0,
                            cyclicShiftAp-r10 cs0
                          },
                          srs-ConfigApDCI-Format1a2b2c-r10 
                          {
                            srs-AntennaPortAp-r10 an1,
                            srs-BandwidthAp-r10 bw0,
                            freqDomainPositionAp-r10 0,
                            transmissionCombAp-r10 0,
                            cyclicShiftAp-r10 cs0
                          }
                        }
                    },
                  uplinkPowerControlDedicated-v1020 
                  {
                    pSRS-OffsetAp-r10 0
                  }
                }
              }
            }
      }
}

*/

byte llc_conn_setup_with_ap_srs[20] = {0x68, 0x13, 0x9B, 0xF0, 0x10, 0x05, 0x09, 0x07, 0xBC, 0xC3, 0x00, 0x24, 0x0E, 0x00, 0xEB, 0x60, 0x00, 0x00, 0x01, 0x00
};

/*
test:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                mac-MainConfig defaultValue : NULL,
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL release : NULL,
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf20,
                      implicitReleaseAfter e2,
                      p0-Persistent 
                      {
                        p0-NominalPUSCH-Persistent 5,
                        p0-UE-PUSCH-Persistent 5
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB2
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition release : NULL
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en1,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 2,
                    pSRS-Offset 3
                  },
                  tpc-PDCCH-ConfigPUCCH release : NULL,
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm30,
                    nomPDSCH-RS-EPRE-Offset 0
                  },
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw0,
                      srs-HoppingBandwidth hbw0,
                      freqDomainPosition 0,
                      duration TRUE,
                      srs-ConfigIndex 0,
                      transmissionComb 1,
                      cyclicShift cs0
                    },
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm1,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    }
                }
              }
            }
      }
}

*/
byte llc_conn_setup_with_r8_antenna_selection[21] = 
{0x68, 0x13, 0x9B, 0xF0, 0x10, 0x05, 0x09, 0x07, 0xAD, 0xFF, 
   0x08, 0xE8, 0xC9, 0x98, 0x02, 0x00, 0x40, 0x28, 0x00, 0x00, 0x00};
/*
conn_setup1:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                mac-MainConfig defaultValue : NULL,
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL release : NULL,
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf20,
                      implicitReleaseAfter e2,
                      p0-Persistent 
                      {
                        p0-NominalPUSCH-Persistent 5,
                        p0-UE-PUSCH-Persistent 5
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB2
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition release : NULL
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en1,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 2,
                    pSRS-Offset 3
                  },
                  tpc-PDCCH-ConfigPUCCH release : NULL,
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw0,
                      srs-HoppingBandwidth hbw0,
                      freqDomainPosition 0,
                      duration TRUE,
                      srs-ConfigIndex 0,
                      transmissionComb 1,
                      cyclicShift cs0
                    },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    },
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm1,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm30
                      },
                    nomPDSCH-RS-EPRE-Offset 0
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an1,
                        resourceConfig-r10 0,
                        subframeConfig-r10 0,
                        p-C-r10 0
                      }
                  }
                }
              }
            }
      }
}

*/
byte llc_conn_setup_with_tm1_cqi_aperiodic_m30_antport_1[28] = { 0x68, 0x13, 0x9B, 0xF0, 0x10, 
   0x05, 0x09, 0x07, 0xBD, 0xD7, 0x08, 0xE8, 0xC8, 0x02, 0x00, 0x44, 0x00, 0x00, 0x00, 0x48, 
   0x1E, 0x60, 0x01, 0x44, 0xCD, 0x00, 0x00, 0x80};

/*
conn_setup1:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                mac-MainConfig defaultValue : NULL,
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL release : NULL,
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf20,
                      implicitReleaseAfter e2,
                      p0-Persistent 
                      {
                        p0-NominalPUSCH-Persistent 5,
                        p0-UE-PUSCH-Persistent 5
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB2
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition release : NULL
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en1,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 2,
                    pSRS-Offset 3
                  },
                  tpc-PDCCH-ConfigPUCCH release : NULL,
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw0,
                      srs-HoppingBandwidth hbw0,
                      freqDomainPosition 0,
                      duration TRUE,
                      srs-ConfigIndex 0,
                      transmissionComb 1,
                      cyclicShift cs0
                    },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    },
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm4,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm22
                      },
                    nomPDSCH-RS-EPRE-Offset 0
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an4,
                        resourceConfig-r10 0,
                        subframeConfig-r10 0,
                        p-C-r10 0
                      }
                  }
                }
              }
            }
      }
}

*/
byte llc_conn_setup_with_tm4_cqi_aperiodic_m22_antport_4[28] = {0x68, 0x13, 0x9B, 0xF0, 0x10, 
   0x05, 0x09, 0x07, 0xBD, 0xD7, 0x08, 0xE8, 0xC8, 0x02, 0x00, 0x44, 0x00, 0x00, 0x00, 0x48, 
   0x1E, 0x60, 0x07, 0x44, 0x8D, 0x80, 0x00, 0x80};

/*
Conn_setup_with_csi:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                mac-MainConfig defaultValue : NULL,
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL release : NULL,
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf20,
                      implicitReleaseAfter e2,
                      p0-Persistent 
                      {
                        p0-NominalPUSCH-Persistent 5,
                        p0-UE-PUSCH-Persistent 5
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB2
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition release : NULL
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en1,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 2,
                    pSRS-Offset 3
                  },
                  tpc-PDCCH-ConfigPUCCH release : NULL,
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm30,
                    nomPDSCH-RS-EPRE-Offset 0
                  },
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw0,
                      srs-HoppingBandwidth hbw0,
                      freqDomainPosition 0,
                      duration TRUE,
                      srs-ConfigIndex 0,
                      transmissionComb 1,
                      cyclicShift cs0
                    },
                  antennaInfo defaultValue : NULL,
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an4,
                        resourceConfig-r10 2,
                        subframeConfig-r10 3,
                        p-C-r10 0
                      }
                  }
                }
              }
            }
      }
}
*/

byte llc_conn_setup_with_csi_ies[27] = {0x68, 0x13, 0x9B, 0xF0, 0x10, 0x05, 0x09, 
  0x07, 0xBD, 0xFF, 0x08, 0xE8, 0xC9, 0x98, 0x02, 0x00, 0x46, 0x00, 0x00, 0x00, 
  0x24, 0x0A, 0x10, 0x2C, 0x20, 0x34, 0x00,};

/*
llc_rcfg_with_pcc_ap_cqi:value DL-DCCH-Message ::= 
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
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    timeAlignmentTimerDedicated sf500
                  },
                sps-Config 
                {
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
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : openLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm30,
                        aperiodicCSI-Trigger-r10 
                        {
                          trigger1-r10 '01100100'B,
                          trigger2-r10 '01100101'B
                        }
                      },
                    nomPDSCH-RS-EPRE-Offset 0
                  }
                }
              }
            }
      }
}
*/

byte llc_rcfg_with_pcc_ap_cqi[21] = {0x22, 0x02, 0x07, 0x20, 0x21, 0xC0, 0x40, 0x09, 0x40, 
  0x41, 0xEC, 0x00, 0x04, 0x81, 0xA4, 0x01, 0x1C, 0x6D, 0x91, 0x94, 0x80};

/*
mac_default:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                mac-MainConfig defaultValue : NULL,
                sps-Config 
                {
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
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : openLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm30,
                        aperiodicCSI-Trigger-r10 
                        {
                          trigger1-r10 '01100100'B,
                          trigger2-r10 '01100101'B
                        }
                      },
                    nomPDSCH-RS-EPRE-Offset 0
                  }
                }
              }
            }
      }
}
*/

byte llc_rcfg_with_default_mac_cfg[19] = {0x22, 0x02, 0x07, 0xB8, 0x08, 0x01, 0x28, 0x08, 0x3D, 0x80, 0x00, 0x90, 0x34, 0x80, 0x23, 0x8D, 0xB2, 0x32, 0x90};


/*
Valid_Scc_W_CQI:value DL-DCCH-Message ::= 
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
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    timeAlignmentTimerDedicated sf500
                  },
                sps-Config 
                {
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
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : openLoop
                    },
                  pucch-ConfigDedicated-v1020 
                  {
                    pucch-Format-r10 channelSelection-r10 : 
                      {
                        n1PUCCH-AN-CS-r10 setup : 
                          {
                            n1PUCCH-AN-CS-List-r10 
                            {
                              {
                                1
                              }
                            }
                          }
                      }
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
                          dl-CarrierFreq-r10 100
                        },
                        radioResourceConfigCommonSCell-r10 
                        {
                          nonUL-Configuration-r10 
                          {
                            dl-Bandwidth-r10 n100,
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
                          }
                        },
                        radioResourceConfigDedicatedSCell-r10 
                        {
                          physicalConfigDedicatedSCell-r10 
                          {
                            nonUL-Configuration-r10 
                            {
                            },
                            ul-Configuration-r10 
                            {
                              cqi-ReportConfigSCell-r10 
                              {
                                cqi-ReportModeAperiodic-r10 rm30,
                                nomPDSCH-RS-EPRE-Offset-r10 0
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

byte llc_rcfg_with_scc_ap_cqi[31] = {0x22, 0x02, 0x87, 0x20, 0x21, 0xC0, 0x40, 0x09, 0x40, 0x41, 0xEC, 0x00, 0x04, 0x81, 
  0x61, 0x01, 0x1C, 0x70, 0x00, 0x52, 0x87, 0x00, 0x00, 0x06, 0x40, 0xAA, 0x9C, 0xAC, 0x04, 0x46, 0x40,};

/*
llc_rcfg_with_scc_ap_cqi:value DL-DCCH-Message ::= 
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
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    timeAlignmentTimerDedicated sf500
                  },
                sps-Config 
                {
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
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : openLoop
                    },
                  pucch-ConfigDedicated-v1020 
                  {
                    pucch-Format-r10 channelSelection-r10 : 
                      {
                        n1PUCCH-AN-CS-r10 setup : 
                          {
                            n1PUCCH-AN-CS-List-r10 
                            {
                              {
                                1
                              }
                            }
                          }
                      }
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
                          dl-CarrierFreq-r10 100
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
                          }
                        },
                        radioResourceConfigDedicatedSCell-r10 
                        {
                          physicalConfigDedicatedSCell-r10 
                          {
                            nonUL-Configuration-r10 
                            {
                            },
                            ul-Configuration-r10 
                            {
                              cqi-ReportConfigSCell-r10 
                              {
                                cqi-ReportModeAperiodic-r10 rm30,
                                nomPDSCH-RS-EPRE-Offset-r10 0
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

byte llc_rcfg_with_invalid_bw_scc_ap_cqi[31] = {0x22, 0x02, 0x87, 0x20, 0x21, 0xC0, 0x40, 0x09, 0x40, 0x41, 0xEC, 0x00, 0x04, 0x81, 
  0x61, 0x01, 0x1C, 0x70, 0x00, 0x52, 0x87, 0x00, 0x00, 0x06, 0x40, 0x0A, 0x9C, 0xAC, 0x04, 0x46, 0x40,};



/*
SCC_with_CSI:value DL-DCCH-Message ::= 
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
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    timeAlignmentTimerDedicated sf500
                  },
                sps-Config 
                {
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
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : openLoop
                    },
                  pucch-ConfigDedicated-v1020 
                  {
                    pucch-Format-r10 channelSelection-r10 : 
                      {
                        n1PUCCH-AN-CS-r10 setup : 
                          {
                            n1PUCCH-AN-CS-List-r10 
                            {
                              {
                                1
                              }
                            }
                          }
                      }
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
                          dl-CarrierFreq-r10 100
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
                          }
                        },
                        radioResourceConfigDedicatedSCell-r10 
                        {
                          physicalConfigDedicatedSCell-r10 
                          {
                            nonUL-Configuration-r10 
                            {
                            },
                            ul-Configuration-r10 
                            {
                              cqi-ReportConfigSCell-r10 
                              {
                                cqi-ReportModeAperiodic-r10 rm30,
                                nomPDSCH-RS-EPRE-Offset-r10 0,
                                cqi-ReportPeriodicSCell-r10 setup : 
                                  {
                                    cqi-PUCCH-ResourceIndex-r10 0,
                                    cqi-pmi-ConfigIndex 0,
                                    cqi-FormatIndicatorPeriodic-r10 widebandCQI-r10 : 
                                      {
                                      },
                                    simultaneousAckNackAndCQI FALSE,
                                    csi-ConfigIndex-r10 setup : 
                                      {
                                        cqi-pmi-ConfigIndex2-r10 3
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
      }
}
*/

byte llc_rcfg_with_scc_csi[36] = {0x22, 0x02, 0x87, 0x20, 0x21, 0xC0, 0x40, 0x09, 0x40, 0x41, 0xEC, 0x00, 0x04, 0x81, 0x61, 0x01, 
                                                 0x1C, 0x70, 0x00, 0x52, 0x87, 0x00, 0x00, 0x06, 0x40, 0x0A, 0x9C, 0xAC, 0x04, 0x66, 0x62, 0x00, 
                                                 0x00, 0x01, 0x00, 0x60,};

/*
llc_rcfg_with_pcc_prd_cqi:value DL-DCCH-Message ::= 
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
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    timeAlignmentTimerDedicated sf500
                  },
                sps-Config 
                {
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
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : openLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-pmi-ConfigIndex 35,
                        cqi-FormatIndicatorPeriodic-r10 widebandCQI-r10 : 
                          {
                          },
                        ri-ConfigIndex 164,
                        simultaneousAckNackAndCQI TRUE
                      }
                  }
                }
              }
            }
      }
}
*/

byte llc_rcfg_with_pcc_prd_cqi[24] = {0x22, 0x02, 0x07, 0x20, 0x21, 0xC0, 0x40, 0x09, 0x40, 0x41, 0xEC, 0x00, 
0x04, 0x82, 0x64, 0x01, 0x1A, 0x1A, 0x00, 0x00, 0x8C, 0x29, 0x20, 0x00};

/*
s_cell_rel:value DL-DCCH-Message ::= 
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
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    timeAlignmentTimerDedicated sf500
                  },
                sps-Config 
                {
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
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pucch-ConfigDedicated-v1020 
                  {
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
                    }
                  }
                }
              }
            }
      }
}
*/

byte llc_rcfg_with_scc_rel[18] = {0x22, 0x02, 0x87, 0x20, 0x21, 0xC0, 0x40, 0x09, 0x40, 0x41, 0xEC, 0x00, 0x04, 0x80, 0x81, 0x00, 0x13, 0x00,};


/*
Valid_scell_w_cqi1:value DL-DCCH-Message ::= 
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
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    timeAlignmentTimerDedicated sf500
                  },
                sps-Config 
                {
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
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pucch-ConfigDedicated-v1020 
                  {
                    pucch-Format-r10 channelSelection-r10 : 
                      {
                        n1PUCCH-AN-CS-r10 setup : 
                          {
                            n1PUCCH-AN-CS-List-r10 
                            {
                              {
                                6
                              }
                            }
                          }
                      }
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
                          dl-CarrierFreq-r10 100
                        },
                        radioResourceConfigCommonSCell-r10 
                        {
                          nonUL-Configuration-r10 
                          {
                            dl-Bandwidth-r10 n100,
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
                          }
                        },
                        radioResourceConfigDedicatedSCell-r10 
                        {
                          physicalConfigDedicatedSCell-r10 
                          {
                            nonUL-Configuration-r10 
                            {
                            },
                            ul-Configuration-r10 
                            {
                              cqi-ReportConfigSCell-r10 
                              {
                                cqi-ReportModeAperiodic-r10 rm30,
                                nomPDSCH-RS-EPRE-Offset-r10 0
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

byte llc_rcfg_with_scc_added_back[30] = {0x22, 0x02, 0x87, 0x20, 0x21, 0xC0, 0x40, 0x09, 0x40, 0x41, 0xEC, 0x00, 0x04, 0x81, 0x01, 
  0x04, 0x70, 0x01, 0x92, 0x87, 0x00, 0x00, 0x06, 0x40, 0xAA, 0x9C, 0xAC, 0x04, 0x46, 0x40,};

/*
srs_ap:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                mac-MainConfig defaultValue : NULL,
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL release : NULL,
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf20,
                      implicitReleaseAfter e2,
                      p0-Persistent 
                      {
                        p0-NominalPUSCH-Persistent 5,
                        p0-UE-PUSCH-Persistent 5
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB2
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition release : NULL
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en1,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 2,
                    pSRS-Offset 3
                  },
                  tpc-PDCCH-ConfigPUCCH release : NULL,
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm30,
                    nomPDSCH-RS-EPRE-Offset 0
                  },
                  antennaInfo defaultValue : NULL,
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    },
                  soundingRS-UL-ConfigDedicated-v1020 
                  {
                    srs-AntennaPort-r10 an1
                  },
                  uplinkPowerControlDedicated-v1020 
                  {
                    pSRS-OffsetAp-r10 1
                  }
                }
              }
            }
      }
}
*/

byte llc_conn_setup_with_srs_ap[22] = {0x68, 0x13, 0x9B, 0xF0, 0x10, 0x05, 0x09, 0x07, 0xBD, 0xEF, 0x08, 
  0xE8, 0xC9, 0x9C, 0x00, 0x00, 0x00, 0x48, 0x0C, 0x02, 0x88, 0x80};

/*
TM9_COnn:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                mac-MainConfig defaultValue : NULL,
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL release : NULL,
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf20,
                      implicitReleaseAfter e2,
                      p0-Persistent 
                      {
                        p0-NominalPUSCH-Persistent 5,
                        p0-UE-PUSCH-Persistent 5
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB2
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition release : NULL
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en1,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 2,
                    pSRS-Offset 3
                  },
                  tpc-PDCCH-ConfigPUCCH release : NULL,
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw0,
                      srs-HoppingBandwidth hbw0,
                      freqDomainPosition 0,
                      duration TRUE,
                      srs-ConfigIndex 0,
                      transmissionComb 1,
                      cyclicShift cs0
                    },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    },
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      codebookSubsetRestriction-r10 '000000'B,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm22
                      },
                    nomPDSCH-RS-EPRE-Offset 1,
                    pmi-RI-Report-r9 setup
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an2,
                        resourceConfig-r10 2,
                        subframeConfig-r10 3,
                        p-C-r10 0
                      }
                  }
                }
              }
            }
      }
}
*/

byte llc_conn_setup_with_tm9_ies[30] = {0x68, 0x13, 0x9B, 0xF0, 0x10, 0x05, 0x09, 0x07, 0xBD, 0xD7, 0x08, 0xE8, 0xC8, 
    0x02, 0x00, 0x44, 0x00, 0x00, 0x00, 0x48, 0x26, 0x60, 0x30, 0x0C, 0x05, 0x52, 0x55, 0x10, 0x1A, 0x00,};
                                                            


/*
connection_setup_with_pmi:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                mac-MainConfig defaultValue : NULL,
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL release : NULL,
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf20,
                      implicitReleaseAfter e2,
                      p0-Persistent 
                      {
                        p0-NominalPUSCH-Persistent 5,
                        p0-UE-PUSCH-Persistent 5
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB2
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition release : NULL
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en1,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 2,
                    pSRS-Offset 3
                  },
                  tpc-PDCCH-ConfigPUCCH release : NULL,
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  cqi-ReportConfig 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic setup : 
                      {
                        cqi-PUCCH-ResourceIndex 0,
                        cqi-pmi-ConfigIndex 38,
                        cqi-FormatIndicatorPeriodic subbandCQI : 
                          {
                            k 4
                          },
                        ri-ConfigIndex 171,
                        simultaneousAckNackAndCQI TRUE
                      }
                  },
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw0,
                      srs-HoppingBandwidth hbw0,
                      freqDomainPosition 0,
                      duration TRUE,
                      srs-ConfigIndex 0,
                      transmissionComb 1,
                      cyclicShift cs0
                    },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    },
                  cqi-ReportConfig-v920 
                  {
                    cqi-Mask-r9 setup,
                    pmi-RI-Report-r9 setup
                  },
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection release : NULL
                    },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an4,
                        resourceConfig-r10 2,
                        subframeConfig-r10 3,
                        p-C-r10 0
                      }
                  }
                }
              }
            }
      }
}
*/

byte llc_conn_setup_with_csi_pmi_ies[34] = {0x68, 0x13, 0x9B, 0xF0, 0x10, 0x05, 0x09, 0x07, 0xBD, 0xF7, 
0x08, 0xE8, 0xC4, 0xE0, 0x00, 0x26, 0xE5, 0x5E, 0x00, 0x80, 0x11, 0x00, 0x00, 0x00, 0x16, 0x01, 0xB0, 
0x06, 0x88, 0x04, 0x2C, 0x20, 0x34, 0x00};


/*
AckNakWithPucchFmt:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                mac-MainConfig defaultValue : NULL,
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL release : NULL,
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf20,
                      implicitReleaseAfter e2,
                      p0-Persistent 
                      {
                        p0-NominalPUSCH-Persistent 5,
                        p0-UE-PUSCH-Persistent 5
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB2
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition release : NULL,
                    tdd-AckNackFeedbackMode bundling
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en1,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 2,
                    pSRS-Offset 3
                  },
                  tpc-PDCCH-ConfigPUCCH release : NULL,
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm30,
                    nomPDSCH-RS-EPRE-Offset 0
                  },
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw0,
                      srs-HoppingBandwidth hbw0,
                      freqDomainPosition 0,
                      duration TRUE,
                      srs-ConfigIndex 0,
                      transmissionComb 1,
                      cyclicShift cs0
                    },
                  antennaInfo defaultValue : NULL,
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    },
                  pucch-ConfigDedicated-v1020 
                  {
                    pucch-Format-r10 format3-r10 : 
                      {
                        n3PUCCH-AN-List-r10 
                        {
                          0
                        }
                      }
                  }
                }
              }
            }
      }
}
*/

byte llc_conn_setup_with_ack_nak_and_pucch_fmt[26] = {0x68, 0x13, 0x9B, 0xF0, 0x10, 0x05, 0x09, 
  0x07, 0xBD, 0xFF, 0x44, 0x74, 0x64, 0xCC, 0x01, 0x00, 0x23, 0x00, 0x00, 0x00, 0x12, 0x04, 0x04, 
  0x10, 0x80, 0x00};

/*
AckNakWithPucchFmt:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                mac-MainConfig defaultValue : NULL,
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL release : NULL,
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf20,
                      implicitReleaseAfter e2,
                      p0-Persistent 
                      {
                        p0-NominalPUSCH-Persistent 5,
                        p0-UE-PUSCH-Persistent 5
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB2
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en1,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 2,
                    pSRS-Offset 3
                  },
                  tpc-PDCCH-ConfigPUCCH release : NULL,
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm30,
                    nomPDSCH-RS-EPRE-Offset 0
                  },
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw0,
                      srs-HoppingBandwidth hbw0,
                      freqDomainPosition 0,
                      duration TRUE,
                      srs-ConfigIndex 0,
                      transmissionComb 1,
                      cyclicShift cs0
                    },
                  antennaInfo defaultValue : NULL,
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    }
                }
              }
            }
      }
}

*/

byte llc_conn_setup_with_no_pucch[20] = {0x68, 0x13, 0x9B, 0xF0, 0x10, 0x05, 0x09, 
  0x07, 0xA9, 0xFF, 0x23, 0xA3, 0x26, 0x60, 0x08, 0x01, 0x18, 0x00, 0x00, 0x00};



/*===========================================================================
                   Connection Setup Message - Invalid RB Mode
===========================================================================*/
/*
ConnSetup:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config explicitValue : um-Bi-Directional : 
                        {
                          ul-UM-RLC 
                          {
                            sn-FieldLength size5
                          },
                          dl-UM-RLC 
                          {
                            sn-FieldLength size5,
                            t-Reordering ms0
                          }
                        },
                    logicalChannelConfig defaultValue : NULL
                  }
                }
              }
            }
      }
}
*/

byte llc_invalid_conn_setup_mode[5] = {0x68, 0x10, 0x18, 0x40, 0x40};

/*===========================================================================
                   Connection Setup Message - Invalid RB ID
===========================================================================*/
/*
ConnectionSetup:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfiguration 
              {
                srb-ToAddModifyList 
                {
                  {
                    srb-Identity 2,
                    rlc-Configuration defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                }
              }
            }
      }
}
*/

byte llc_invalid_conn_setup_id[3] = {0x68, 0x10, 0x1F};

/*===========================================================================
                   Connection Setup Message - With Both SRB1 & SRB2
===========================================================================*/
/*
ConnectionSetup:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfiguration 
              {
                srb-ToAddModifyList 
                {
                  {
                    srb-Identity 1,
                    rlc-Configuration defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  },
                  {
                    srb-Identity 2,
                    rlc-Configuration defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                }
              }
            }
      }
}
*/

byte llc_invalid_conn_setup_both_srbs[4] = {0x68, 0x10, 0x5B, 0x7C};

/*===========================================================================
                   Connection Setup Message - Non Rel. 8 Contents
===========================================================================*/
/*
ConnectionSetup:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : spare7 : NULL
      }
}
*/

byte llc_invalid_conn_setup_non_rel8[2] = {0x68, 0x80};

/*===========================================================================
                   Connection Setup Message - With DRB Setup
===========================================================================*/
/*
ConnectionSetup:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfiguration 
              {
                srb-ToAddModifyList 
                {
                  {
                    srb-Identity 1,
                    rlc-Configuration defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                drb-ToAddModifyList 
                {
                  {
                    eps-BearerIdentity 1,
                    drb-Identity 1,
                    pdcp-Configuration 
                    {
                      headerCompression notUsed : NULL
                    },
                    rlc-Configuration um-Bi-Directional : 
                      {
                        ul-UM-RLC 
                        {
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms0
                        }
                      },
                    logicalChannelIdentity 4,
                    logicalChannelConfig 
                    {
                    }
                  }
                }
              }
            }
      }
}
*/

byte llc_invalid_conn_setup_with_drb[8] = {0x68, 0x18, 0x1B, 0x07, 0xC4, 0x00, 
  0x20, 0x08};

/*===========================================================================
                   Connection Setup Message - With DRB Release
===========================================================================*/
/*
ConnSetup:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
                  {
                    srb-Identity 1,
                    rlc-Config defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                drb-ToReleaseList 
                {
                  1
                },
                mac-MainConfig defaultValue : NULL,
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB2
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition release : NULL
                  },
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH -7,
                    deltaMCS-Enabled en1,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH -4,
                    pSRS-Offset 6,
                    filterCoefficient fc2
                  },
                  tpc-PDCCH-ConfigPUCCH release : NULL,
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm30,
                    nomPDSCH-RS-EPRE-Offset -1,
                    cqi-ReportPeriodic release : NULL
                  },
                  soundingRS-UL-ConfigDedicated release : NULL,
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm1,
                      codebookSubsetRestriction n2TxAntenna-tm3 : '00'B,
                      ue-TransmitAntennaSelection release : NULL
                    },
                  schedulingRequestConfig release : NULL
                }
              }
            }
      }
}
*/

byte llc_conn_setup_with_drb_rel[16] = {0x68, 0x16, 0x9B, 0x00, 0x5B, 0xFE, 0x23, 
  0xA3, 0x08, 0xD8, 0x10, 0x00};

/*===========================================================================
       Connection Setup Message - With No Physical Dedicated Config Info.
===========================================================================*/
/*
ConnectionSetup:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionSetup-r8 : 
            {
              radioResourceConfiguration 
              {
                srb-ToAddModifyList 
                {
                  {
                    srb-Identity 1,
                    rlc-Configuration defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                }
              }
            }
      }
}
*/

byte llc_conn_setup_with_no_phy_ded[3] = {0x68, 0x10, 0x1B};

/*===========================================================================
       Connection Setup Message - With invalid Critical Extension
===========================================================================*/

/*
ConnectionSetup:value DL-CCCH-Message ::= 
{
  message c1 : rrcConnectionSetup : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions criticalExtensionsFuture : 
          {
          }
      }
}
*/

byte llc_invalid_conn_setup_crit_ext[1] = {0x6C};

/*===========================================================================
                            Reconfiguration Message
===========================================================================*/
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
              },
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
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
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
                      },
                      headerCompression rohc : 
                        {
                          maxCID 1,
                          profiles 
                          {
                            profile0x0001 TRUE,
                            profile0x0002 TRUE,
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 2,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 1
                      }
                    }
                  }
                },
                mac-MainConfig explicitValue : 
                  {
                    ul-SCH-Config 
                    {
                      maxHARQ-Tx n1,
                      periodicBSR-Timer sf5,
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    drx-Config setup : 
                      {
                        onDurationTimer psf1,
                        drx-InactivityTimer psf1,
                        drx-RetransmissionTimer psf2,
                        longDRX-CycleStartOffset sf10 : 0,
                        shortDRX 
                        {
                          shortDRX-Cycle sf2,
                          drxShortCycleTimer 1
                        }
                      },
                    timeAlignmentTimerDedicated sf500,
                    phr-Config setup : 
                      {
                        periodicPHR-Timer sf10,
                        prohibitPHR-Timer sf0,
                        dl-PathlossChange dB1
                      }
                  },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB-6
                  },
                  pucch-ConfigDedicated 
                  {
                    ackNackRepetition setup : 
                      {
                        repetitionFactor n2,
                        n1PUCCH-AN-Rep 100
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
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en0,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 5,
                    pSRS-Offset 2,
                    filterCoefficient fc4
                  },
                  tpc-PDCCH-ConfigPUCCH setup : 
                    {
                      tpc-RNTI '00000001 00000000'B,
                      tpc-Index indexOfFormat3 : 2
                    },
                  tpc-PDCCH-ConfigPUSCH release : NULL,
                  cqi-ReportConfig 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic setup : 
                      {
                        cqi-PUCCH-ResourceIndex 3,
                        cqi-pmi-ConfigIndex 4,
                        cqi-FormatIndicatorPeriodic subbandCQI : 
                          {
                            k 2
                          },
                        simultaneousAckNackAndCQI TRUE
                      }
                  },
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw0,
                      srs-HoppingBandwidth hbw1,
                      freqDomainPosition 0,
                      duration FALSE,
                      srs-ConfigIndex 0,
                      transmissionComb 0,
                      cyclicShift cs0
                    },
                  antennaInfo defaultValue : NULL,
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    }
                }
              }
            }
      }
}
*/

byte llc_reconfig_msg[44] = {0x22, 0x12, 0x00, 0x03, 0x53, 0xE0, 0xF9, 0x89, 
  0x42, 0x80, 0x01, 0x80, 0x20, 0xBB, 0x12, 0x09, 0xF0, 0x01, 0xC0, 0x04, 0x00, 
  0x00, 0x40, 0x1F, 0xF8, 0xC0, 0xC8, 0x00, 0x0C, 0x3A, 0x44, 0x80, 0x80, 0x04, 
  0x98, 0x01, 0x80, 0x97, 0x10, 0x00, 0x00, 0xC0, 0x00, 0x00};


/*
FeICIC_CRS_Assistance_Info:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                neighCellsCRS-Info-r11 setup : 
                  {
                    {
                      physCellId-r11 10,
                      antennaPortsCount-r11 an2,
                      mbsfn-SubframeConfigList-r11 
                      {
                        {
                          radioframeAllocationPeriod n2,
                          radioframeAllocationOffset 2,
                          subframeAllocation oneFrame : '011001'B
                        }
                      }
                    },
                    {
                      physCellId-r11 20,
                      antennaPortsCount-r11 an4,
                      mbsfn-SubframeConfigList-r11 
                      {
                        {
                          radioframeAllocationPeriod n16,
                          radioframeAllocationOffset 5,
                          subframeAllocation oneFrame : '001101'B
                        }
                      }
                    },
                    {
                      physCellId-r11 14,
                      antennaPortsCount-r11 an4,
                      mbsfn-SubframeConfigList-r11 
                      {
                        {
                          radioframeAllocationPeriod n8,
                          radioframeAllocationOffset 6,
                          subframeAllocation oneFrame : '001000'B
                        }
                      }
                    },
                    {
                      physCellId-r11 16,
                      antennaPortsCount-r11 an1,
                      mbsfn-SubframeConfigList-r11 
                      {
                        {
                          radioframeAllocationPeriod n16,
                          radioframeAllocationOffset 3,
                          subframeAllocation oneFrame : '000001'B
                        }
                      }
                    },
                    {
                      physCellId-r11 18,
                      antennaPortsCount-r11 an4,
                      mbsfn-SubframeConfigList-r11 
                      {
                        {
                          radioframeAllocationPeriod n32,
                          radioframeAllocationOffset 2,
                          subframeAllocation fourFrames : '00000110 00000000 10010000'B
                        }
                      }
                    },
                    {
                      physCellId-r11 25,
                      antennaPortsCount-r11 an2,
                      mbsfn-SubframeConfigList-r11 
                      {
                        {
                          radioframeAllocationPeriod n8,
                          radioframeAllocationOffset 3,
                          subframeAllocation oneFrame : '000001'B
                        }
                      }
                    },
                    {
                      physCellId-r11 56,
                      antennaPortsCount-r11 an4,
                      mbsfn-SubframeConfigList-r11 
                      {
                        {
                          radioframeAllocationPeriod n8,
                          radioframeAllocationOffset 3,
                          subframeAllocation oneFrame : '000000'B
                        }
                      }
                    },
                    {
                      physCellId-r11 19,
                      antennaPortsCount-r11 an4,
                      mbsfn-SubframeConfigList-r11 
                      {
                        {
                          radioframeAllocationPeriod n1,
                          radioframeAllocationOffset 2,
                          subframeAllocation oneFrame : '000001'B
                        }
                      }
                    }
                  }
              }
            }
      }
}
*/

byte llc_feicic_crs_info_msg[37] = {0x22, 0x02, 0x40, 0x04, 0x47, 0xFE, 0x05, 0x20, 0xA3, 0x20, 0xA4, 0x25, 0x1A, 0x07, 0x41, 0xE1, 0x00, 
    0x80, 0x23, 0x02, 0x09, 0x42, 0xA8, 0x30, 0x04, 0x80, 0x32, 0x86, 0xC0, 0x87, 0x10, 0x6C, 0x00, 0x27, 0x00, 0x80, 0x80};

/*
Feicic_CRS_info:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                neighCellsCRS-Info-r11 release : NULL
              }
            }
      }
}
*/

byte llc_feicic_crs_info_rel_msg[7] = {0x22, 0x02, 0x40, 0x04, 0x40, 0x60, 0x00};



/*
eICIC_pcell:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measObjectToAddModList 
                {
                  {
                    measObjectId 1,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 100,
                        allowedMeasBandwidth mbw6,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '00'B,
                        measSubframePatternConfigNeigh-r10 setup : 
                          {
                            measSubframePatternNeigh-r10 subframePatternFDD-r10 : '01000001 01000010 01000011 01000100 010 ...'B,
                            measSubframeCellList-r10 
                            {
                              {
                                start 1,
                                range n12
                              },
                              {
                                start 4
                              }
                            }
                          }
                      }
                  }
                }
              },
              radioResourceConfigDedicated 
              {
                measSubframePatternPCell-r10 release : NULL
              }
            }
      }
}

*/

byte llc_eicic_release_msg[25] = {0x22, 0x12, 0x10, 0x00, 0x00, 0x20, 0x00, 0x32, 0x00, 0x02, 0x14, 0xE2, 0x0A, 0x12, 0x1A, 0x22, 0x28, 0x60, 0x12, 0x01, 0x01, 0x00, 0x0A, 0x03, 0x00};

/*
CSI_RS_Fields:value DL-DCCH-Message ::= 
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
                  nonCriticalExtension 
                  {
                    sCellToAddModList-r10 
                    {
                      {
                        sCellIndex-r10 1,
                        radioResourceConfigDedicatedSCell-r10 
                        {
                          physicalConfigDedicatedSCell-r10 
                          {
                            nonUL-Configuration-r10 
                            {
                              csi-RS-Config-r10 
                              {
                                csi-RS-r10 setup : 
                                  {
                                    antennaPortsCount-r10 an1,
                                    resourceConfig-r10 1,
                                    subframeConfig-r10 1,
                                    p-C-r10 1
                                  },
                                zeroTxPowerCSI-RS-r10 setup : 
                                  {
                                    zeroTxPowerResourceConfigList-r10 '00000001 00000001'B,
                                    zeroTxPowerSubframeConfig-r10 1
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
}
*/

byte llc_csi_rs_msg[12] = {0x22, 0x00, 0xA5, 0x02, 0x14, 0x5C, 0x08, 0x0A, 0x60, 0x20, 0x20, 0x20};


/*
eICIC_pcell:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    csi-SubframePatternConfig-r10 release : NULL
                  }
                }
              }
            }
      }
}

*/

byte llc_eicic_set_release_msg[10] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x18, 0x80, 0x12, 0x00};

/*
eICIC_pcell:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    csi-SubframePatternConfig-r10 setup : 
                      {
                        csi-MeasSubframeSet1-r10 subframePatternFDD-r10 : '10011001 10011000 10010111 10010110 100 ...'B,
                        csi-MeasSubframeSet2-r10 subframePatternFDD-r10 : '10011001 10011000 10010111 10010110 100 ...'B
                      }
                  }
                }
              }
            }
      }
}

*/

byte llc_eicic_set_FDD_msg[20] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x68, 0x80, 0x13, 0x26, 0x66, 0x25, 0xE5, 0xA4, 0x09, 0x99, 0x89, 0x79, 0x69, 0x00};


/*
eICIC_pcell:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    csi-SubframePatternConfig-r10 setup : 
                      {
                        csi-MeasSubframeSet1-r10 subframePatternTDD-r10 : subframeConfig1-5-r10 : '10010010 10010001 1001'B,
                        csi-MeasSubframeSet2-r10 subframePatternTDD-r10 : subframeConfig0-r10 : '01000000 01000001 01000010 01000011 010 ...'B
                      }
                  }
                }
              }
            }
      }
}

*/

byte llc_eicic_set_TDD_msg[22] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x78, 0x80, 0x13, 0x44, 0x94, 0x8C, 0xA5, 0x01, 0x05, 0x09, 0x0D, 0x11, 0x15, 0x19, 0x1D, 0x00};

/*
zero_subfm:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    csi-SubframePatternConfig-r10 setup : 
                      {
                        csi-MeasSubframeSet1-r10 subframePatternTDD-r10 : subframeConfig1-5-r10 : '00000000 00000000 0000'B,
                        csi-MeasSubframeSet2-r10 subframePatternTDD-r10 : subframeConfig1-5-r10 : '00000000 00000000 0000'B
                      }
                  }
                }
              }
            }
      }
}
*/

byte llc_eicic_zero_subframe_msg[16] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x48, 0x80, 0x13, 0x40, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,};


/*
eICIC_TDD_Subframe:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    csi-SubframePatternConfig-r10 setup : 
                      {
                        csi-MeasSubframeSet1-r10 subframePatternTDD-r10 : subframeConfig1-5-r10 : '00001010 00000000 0000'B,
                        csi-MeasSubframeSet2-r10 subframePatternTDD-r10 : subframeConfig1-5-r10 : '00000001 00000000 0000'B
                      }
                  }
                }
              }
            }
      }
}
*/

byte llc_eicic_set_TDD_msg1[16] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x48, 0x80, 0x13, 0x40, 0x50, 0x00, 0x20, 0x04, 0x00, 0x00,};

/*
eICIC_TDD_Invalid:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    csi-SubframePatternConfig-r10 setup : 
                      {
                        csi-MeasSubframeSet1-r10 subframePatternTDD-r10 : subframeConfig0-r10 : '00001010 00000000 00000000 00000000 000 ...'B,
                        csi-MeasSubframeSet2-r10 subframePatternTDD-r10 : subframeConfig0-r10 : '00000001 00000000 00000000 00000000 000 ...'B
                      }
                  }
                }
              }
            }
      }
}
*/

byte llc_eicic_TDD_invalid[29] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0xB0, 0x80, 0x13, 0x48, 0x50, 0x00, 0x00, 0x00, 
                                                 0x00, 0x00, 0x00, 0x05, 0x69, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,};

/*
csi_rs_wo_csi_subframe:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-pmi-ConfigIndex 0,
                        cqi-FormatIndicatorPeriodic-r10 widebandCQI-r10 : 
                          {
                          },
                        simultaneousAckNackAndCQI FALSE,
                        csi-ConfigIndex-r10 setup : 
                          {
                            cqi-pmi-ConfigIndex2-r10 15,
                            ri-ConfigIndex2-r10 8
                          }
                      }
                  }
                }
              }
            }
      }
}
*/

byte llc_eicic_csi_rs_cfg_invalid[16] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x48, 0x80, 0x43, 0x10, 0x00, 0x00, 0x0C, 0x0F, 0x02, 0x00,};

/*
CSI_rel:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm20,
                        aperiodicCSI-Trigger-r10 
                        {
                          trigger1-r10 '00000001'B,
                          trigger2-r10 '00001011'B
                        }
                      },
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-pmi-ConfigIndex 0,
                        cqi-FormatIndicatorPeriodic-r10 widebandCQI-r10 : 
                          {
                          },
                        simultaneousAckNackAndCQI FALSE,
                        csi-ConfigIndex-r10 setup : 
                          {
                            cqi-pmi-ConfigIndex2-r10 15,
                            ri-ConfigIndex2-r10 8
                          }
                      },
                    csi-SubframePatternConfig-r10 setup : 
                      {
                        csi-MeasSubframeSet1-r10 subframePatternTDD-r10 : subframeConfig1-5-r10 : '00000001 00000000 0000'B,
                        csi-MeasSubframeSet2-r10 subframePatternTDD-r10 : subframeConfig1-5-r10 : '00001010 00000000 0000'B
                      }
                  }
                }
              }
            }
      }
}
*/
byte llc_eicic_csi_setup[25] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x90, 0x80, 0xDC, 0x80, 0x85, 0x98, 0x80, 
                                             0x00, 0x00, 0x60, 0x78, 0x11, 0x40, 0x08, 0x00, 0x20, 0x28, 0x00, 0x00,};


/*
CSI_Setup:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 release : NULL,
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 release : NULL,
                    csi-SubframePatternConfig-r10 release : NULL
                  }
                }
              }
            }
      }
}
*/
byte llc_eicic_csi_rel[10] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x18, 0x80, 0xD1, 0x00};


/*
eICIC_pcell:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measObjectToAddModList 
                {
                  {
                    measObjectId 1,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 100,
                        allowedMeasBandwidth mbw6,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '00'B,
                        measSubframePatternConfigNeigh-r10 release : NULL
                      }
                  }
                }
              },
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-pmi-ConfigIndex 0,
                        cqi-FormatIndicatorPeriodic-r10 widebandCQI-r10 : 
                          {
                          },
                        simultaneousAckNackAndCQI FALSE,
                        csi-ConfigIndex-r10 release : NULL
                      }
                  }
                }
              }
            }
      }
}

*/

byte llc_eicic_csi_rel_msg[23] = {0x22, 0x12, 0x10, 0x00, 0x00, 0x20, 0x00, 0x32, 0x00, 0x02, 0x02, 0x80, 0x06, 0x00, 0x02, 0x40, 0xC2, 0x01, 0x0C, 0x40, 0x00, 0x00, 0x00};


/*
eICIC_pcell:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measObjectToAddModList 
                {
                  {
                    measObjectId 1,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 100,
                        allowedMeasBandwidth mbw6,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '00'B,
                        measSubframePatternConfigNeigh-r10 release : NULL
                      }
                  }
                }
              },
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-pmi-ConfigIndex 0,
                        cqi-FormatIndicatorPeriodic-r10 widebandCQI-r10 : 
                          {
                          },
                        simultaneousAckNackAndCQI FALSE,
                        csi-ConfigIndex-r10 setup : 
                          {
                            cqi-pmi-ConfigIndex2-r10 15,
                            ri-ConfigIndex2-r10 10
                          }
                      }
                  }
                }
              }
            }
      }
}

*/

byte llc_eicic_csi_msg[26] = {0x22, 0x12, 0x10, 0x00, 0x00, 0x20, 0x00, 0x32, 0x00, 0x02, 0x02, 0x80, 0x06, 0x00, 0x02, 0x41, 0x22, 0x01, 0x0C, 0x40, 0x00, 0x00, 0x30, 0x3C, 0x0A, 0x00};

/*
valid_eicic_csi:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measObjectToAddModList 
                {
                  {
                    measObjectId 1,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 100,
                        allowedMeasBandwidth mbw6,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '00'B,
                        measSubframePatternConfigNeigh-r10 release : NULL
                      }
                  }
                }
              },
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-pmi-ConfigIndex 0,
                        cqi-FormatIndicatorPeriodic-r10 widebandCQI-r10 : 
                          {
                          },
                        simultaneousAckNackAndCQI FALSE,
                        csi-ConfigIndex-r10 setup : 
                          {
                            cqi-pmi-ConfigIndex2-r10 15,
                            ri-ConfigIndex2-r10 8
                          }
                      }
                  }
                }
              }
            }
      }
}
*/

byte llc_valid_eicic_csi_msg[26] = {0x22, 0x12, 0x10, 0x00, 0x00, 0x20, 0x00, 0x32, 0x00, 0x02, 0x02, 0x80, 0x06, 
    0x00, 0x02, 0x41, 0x22, 0x01, 0x0C, 0x40, 0x00, 0x00, 0x30, 0x3C, 0x08, 0x00,};


/*
eICIC_pcell:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measObjectToAddModList 
                {
                  {
                    measObjectId 1,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 100,
                        allowedMeasBandwidth mbw6,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '00'B,
                        measSubframePatternConfigNeigh-r10 setup : 
                          {
                            measSubframePatternNeigh-r10 subframePatternFDD-r10 : '01000001 01000010 01000011 01000100 010 ...'B,
                            measSubframeCellList-r10 
                            {
                              {
                                start 1,
                                range n12
                              },
                              {
                                start 4
                              }
                            }
                          }
                      }
                  }
                }
              },
              radioResourceConfigDedicated 
              {
                measSubframePatternPCell-r10 setup : subframePatternFDD-r10 : '01000001 01000010 01000011 01000100 010 ...'B
              }
            }
      }
}
*/
byte llc_eicic_msg[30] = {0x22, 0x12, 0x10, 0x00, 0x00, 0x20, 0x00, 0x32, 0x00, 0x02, 0x14, 0xE2, 0x0A, 0x12, 0x1A, 0x22, 0x28, 0x60, 0x12, 0x01, 0x01, 0x00, 0x0A, 0x0D, 0x88, 0x28, 0x48, 0x68, 0x88, 0xA0};

/*
eICIC_pcell:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measObjectToAddModList 
                {
                  {
                    measObjectId 1,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 100,
                        allowedMeasBandwidth mbw6,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '00'B,
                        measSubframePatternConfigNeigh-r10 setup : 
                          {
                            measSubframePatternNeigh-r10 subframePatternFDD-r10 : '01000001 01000010 01000011 01000100 010 ...'B,
                            measSubframeCellList-r10 
                            {
                              {
                                start 1,
                                range n12
                              },
                              {
                                start 4
                              }
                            }
                          }
                      }
                  }
                }
              },
              radioResourceConfigDedicated 
              {
                measSubframePatternPCell-r10 setup : subframePatternTDD-r10 : subframeConfig1-5-r10 : '10011001 10011001 1001'B
              }
            }
      }
}


*/


byte llc_eicic_TDD_msg[28] = {0x22, 0x12, 0x10, 0x00, 0x00, 0x20, 0x00, 0x32, 0x00, 0x02, 0x14, 0xE2, 0x0A, 0x12, 0x1A, 0x22, 0x28, 0x60, 0x12, 0x01, 0x01, 0x00, 0x0A, 0x09, 0xA2, 0x66, 0x66, 0x40};


/*
eICIC_pcell:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measObjectToAddModList 
                {
                  {
                    measObjectId 1,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 100,
                        allowedMeasBandwidth mbw6,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '00'B,
                        measSubframePatternConfigNeigh-r10 setup : 
                          {
                            measSubframePatternNeigh-r10 subframePatternFDD-r10 : '01000001 01000010 01000011 01000100 010 ...'B,
                            measSubframeCellList-r10 
                            {
                              {
                                start 1,
                                range n12
                              },
                              {
                                start 4
                              }
                            }
                          }
                      }
                  }
                }
              },
              radioResourceConfigDedicated 
              {
                measSubframePatternPCell-r10 setup : subframePatternTDD-r10 : subframeConfig0-r10 : '01000001 01000010 0100'B
              }
            }
      }
}

*/

byte llc_eicic_TDD1_msg[34] = {0x22, 0x12, 0x10, 0x00, 0x00, 0x20, 0x00, 0x32, 0x00, 0x02, 0x14, 0xE2, 0x0A, 0x12, 0x1A, 0x22, 0x28, 0x60, 0x12, 0x01, 0x01, 0x00, 0x0A, 0x15, 0xA5, 0x05, 0x09, 0x0D, 0x11, 0x15, 0x19, 0x1D, 0x21, 0x20};


/*
eICIC_pcell:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measObjectToAddModList 
                {
                  {
                    measObjectId 1,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 100,
                        allowedMeasBandwidth mbw6,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '00'B,
                        measSubframePatternConfigNeigh-r10 setup : 
                          {
                            measSubframePatternNeigh-r10 subframePatternFDD-r10 : '01000001 01000010 01000011 01000100 010 ...'B,
                            measSubframeCellList-r10 
                            {
                              {
                                start 1,
                                range n12
                              },
                              {
                                start 4
                              }
                            }
                          }
                      }
                  }
                }
              },
              radioResourceConfigDedicated 
              {
                measSubframePatternPCell-r10 setup : subframePatternTDD-r10 : subframeConfig6-r10 : '01000001 01000010 01000011 01000100 010 ...'B
              }
            }
      }
}
*/

byte llc_eicic_TDD2_msg[33] = {0x22, 0x12, 0x10, 0x00, 0x00, 0x20, 0x00, 0x32, 0x00, 0x02, 0x14, 0xE2, 0x0A, 0x12, 0x1A, 0x22, 0x28, 0x60, 0x12, 0x01, 0x01, 0x00, 0x0A, 0x13, 0xA9, 0x05, 0x09, 0x0D, 0x11, 0x15, 0x19, 0x1D, 0x00};



/*===========================================================================
            Reconfig Message - reconfigure Physical Dedicated Config only
============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL setup : 
                    {
                      semiPersistSchedIntervalDL sf32,
                      numberOfConfSPS-Processes 2,
                      n1-PUCCH-AN-PersistentList 
                      {
                        2,
                        3,
                        4
                      }
                    },
                  sps-ConfigUL setup : 
                    {
                      semiPersistSchedIntervalUL sf32,
                      implicitReleaseAfter e8,
                      twoIntervalsConfig true
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB-6
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
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en0,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 5,
                    pSRS-Offset 2,
                    filterCoefficient fc4
                  },
                  tpc-PDCCH-ConfigPUCCH setup : 
                    {
                      tpc-RNTI '00000001 00000000'B,
                      tpc-Index indexOfFormat3 : 2
                    },
                  tpc-PDCCH-ConfigPUSCH setup : 
                    {
                      tpc-RNTI '00000001 00000000'B,
                      tpc-Index indexOfFormat3 : 2
                    },
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm12,
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic setup : 
                      {
                        cqi-PUCCH-ResourceIndex 3,
                        cqi-pmi-ConfigIndex 4,
                        cqi-FormatIndicatorPeriodic subbandCQI : 
                          {
                            k 2
                          },
                        ri-ConfigIndex 0,
                        simultaneousAckNackAndCQI TRUE
                      }
                  },
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw2,
                      srs-HoppingBandwidth hbw1,
                      freqDomainPosition 0,
                      duration TRUE,
                      srs-ConfigIndex 1,
                      transmissionComb 1,
                      cyclicShift cs3
                    },
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm4,
                      codebookSubsetRestriction n2TxAntenna-tm4 : '101010'B,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 0,
                      dsr-TransMax n4
                    }
                }
              }
            }
      }
}
*/

byte llc_reconfig_phyded_msg[39] = {0x22, 0x02, 0x03, 0xE0, 0x20, 0x11, 0x18, 
  0x01, 0x00, 0x30, 0x09, 0x25, 0xBF, 0xF1, 0x80, 0x00, 0x00, 0x18, 0x74, 0x89, 
  0x01, 0x00, 0x0C, 0x04, 0x00, 0x38, 0x38, 0x03, 0x01, 0x28, 0x01, 0xC8, 0x20, 
  0x0D, 0xAD, 0x55, 0x40, 0x00, 0x00};

/*===========================================================================
            Reconfig Message - PHY Layer
============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL setup : 
                    {
                      semiPersistSchedIntervalDL sf32,
                      numberOfConfSPS-Processes 2,
                      n1-PUCCH-AN-PersistentList 
                      {
                        2,
                        3,
                        4
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB-6
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
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en0,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 5,
                    pSRS-Offset 2,
                    filterCoefficient fc4
                  },
                  tpc-PDCCH-ConfigPUCCH setup : 
                    {
                      tpc-RNTI '00000001 00000000'B,
                      tpc-Index indexOfFormat3 : 2
                    },
                  tpc-PDCCH-ConfigPUSCH setup : 
                    {
                      tpc-RNTI '00000001 00000000'B,
                      tpc-Index indexOfFormat3 : 2
                    },
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm12,
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic setup : 
                      {
                        cqi-PUCCH-ResourceIndex 3,
                        cqi-pmi-ConfigIndex 4,
                        cqi-FormatIndicatorPeriodic subbandCQI : 
                          {
                            k 2
                          },
                        ri-ConfigIndex 0,
                        simultaneousAckNackAndCQI TRUE
                      }
                  },
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw2,
                      srs-HoppingBandwidth hbw1,
                      freqDomainPosition 0,
                      duration TRUE,
                      srs-ConfigIndex 1,
                      transmissionComb 1,
                      cyclicShift cs3
                    },
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm4,
                      codebookSubsetRestriction n2TxAntenna-tm4 : '101010'B,
                      ue-TransmitAntennaSelection setup : closedLoop
                    }
                }
              }
            }
      }
}
*/

byte llc_reconfig_phy_msg[35] = {0x22, 0x02, 0x03, 0xC0, 0x20, 0x11, 0x18, 0x01, 
  0x00, 0x30, 0x08, 0xFF, 0x86, 0x00, 0x00, 0x00, 0x61, 0xD2, 0x24, 0x04, 0x00, 
  0x30, 0x10, 0x00, 0xE0, 0xE0, 0x0C, 0x04, 0xA0, 0x07, 0x20, 0x80, 0x36, 0xB5, 
  0x54};

/*===========================================================================
            Reconfig Message - reconfigure Transport Channel only
============================================================================*/
/*
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
                    ul-SCH-Config 
                    {
                      maxHARQ-Tx n5,
                      periodicBSR-Timer sf16,
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    drx-Config setup : 
                      {
                        onDurationTimer psf2,
                        drx-InactivityTimer psf4,
                        drx-RetransmissionTimer psf6,
                        longDRX-CycleStartOffset sf20 : 0,
                        shortDRX 
                        {
                          shortDRX-Cycle sf2,
                          drxShortCycleTimer 1
                        }
                      },
                    timeAlignmentTimerDedicated sf2560,
                    phr-Config setup : 
                      {
                        periodicPHR-Timer sf50,
                        prohibitPHR-Timer sf100,
                        dl-PathlossChange dB3
                      }
                  }
              }
            }
      }
}
*/

byte llc_reconfig_trch_msg[11] = {0x22, 0x02, 0x04, 0x3E, 0x84, 0x38, 0x8D, 0x88, 
  0x00, 0x25, 0x44};

/*===========================================================================
            Reconfig Message - reconfigure MAC only
============================================================================*/
/*
ReconfigMessage:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfiguration 
              {
                mac-MainConfig explicitValue : 
                  {
                    timeAlignmentTimerDedicated sf2560
                  }
              }
            }
      }
}
*/

byte llc_reconfig_mac_msg[4] = {0x22, 0x02, 0x04, 0x04};


/*===========================================================================
                    Reconfig Message - AM DRB Setup

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
                            t-PollRetransmit ms20,
                            pollPDU p32,
                            pollByte kB75,
                            maxRetxThreshold t3
                          },
                          dl-AM-RLC 
                          {
                            t-Reordering ms5,
                            t-StatusProhibit ms5
                          }
                        },
                    logicalChannelConfig explicitValue : 
                      {
                        ul-SpecificParameters 
                        {
                          priority 2,
                          prioritisedBitRate infinity,
                          bucketSizeDuration ms50,
                          logicalChannelGroup 2
                        }
                      }
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-AM 
                      {
                        statusReportRequired TRUE
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
                    rlc-Config am : 
                      {
                        ul-AM-RLC 
                        {
                          t-PollRetransmit ms5,
                          pollPDU p4,
                          pollByte kB75,
                          maxRetxThreshold t1
                        },
                        dl-AM-RLC 
                        {
                          t-Reordering ms5,
                          t-StatusProhibit ms15
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                }
              },
              nonCriticalExtension 
              {
              }
            }
      }
}
*/

byte llc_reconfig_drb_am_msg[24] = {0x22, 0x12, 0x80, 0x03, 0x03, 0x80, 0x6C, 
  0x90, 0x41, 0x31, 0x71, 0x03, 0xE6, 0x26, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x08, 
  0x04, 0x3E, 0x80, 0x80};

/*===========================================================================
                    Reconfig Message - DL UM DRB Setup

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
                            t-PollRetransmit ms20,
                            pollPDU p32,
                            pollByte kB75,
                            maxRetxThreshold t3
                          },
                          dl-AM-RLC 
                          {
                            t-Reordering ms5,
                            t-StatusProhibit ms5
                          }
                        },
                    logicalChannelConfig explicitValue : 
                      {
                        ul-SpecificParameters 
                        {
                          priority 2,
                          prioritisedBitRate infinity,
                          bucketSizeDuration ms50,
                          logicalChannelGroup 2
                        }
                      }
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
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
                    rlc-Config um-Uni-Directional-DL : 
                      {
                        dl-UM-RLC 
                        {
                          sn-FieldLength size10,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                }
              }
            }
      }
}
*/

byte llc_reconfig_drb_dl_um_msg[21] = {0x22, 0x12, 0x00, 0x03, 0x03, 0x80, 0x6C, 
  0x90, 0x41, 0x31, 0x71, 0x03, 0xE6, 0x25, 0x0A, 0x00, 0x00, 0x01, 0xC5, 0xD0, 
  0x10};

/*===========================================================================
                    Reconfig Message - UL UM DRB Setup

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
                            t-PollRetransmit ms20,
                            pollPDU p32,
                            pollByte kB75,
                            maxRetxThreshold t3
                          },
                          dl-AM-RLC 
                          {
                            t-Reordering ms5,
                            t-StatusProhibit ms5
                          }
                        },
                    logicalChannelConfig explicitValue : 
                      {
                        ul-SpecificParameters 
                        {
                          priority 2,
                          prioritisedBitRate infinity,
                          bucketSizeDuration ms50,
                          logicalChannelGroup 2
                        }
                      }
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
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
                    rlc-Config um-Uni-Directional-UL : 
                      {
                        ul-UM-RLC 
                        {
                          sn-FieldLength size5
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                }
              },
              nonCriticalExtension 
              {
              }
            }
      }
}
*/

byte llc_reconfig_drb_ul_um_msg[21] = {0x22, 0x12, 0x80, 0x03, 0x03, 0x80, 0x6C, 
  0x90, 0x41, 0x31, 0x71, 0x03, 0xE6, 0x25, 0x0A, 0x00, 0x00, 0x01, 0x3A, 0x02, 
  0x00};

/*===========================================================================
                Reconfig Message - Duplicate DRB configuration 

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
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
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
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
                    rlc-Config um-Uni-Directional-DL : 
                      {
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms0
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 3,
                        prioritisedBitRate infinity,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 0
                      }
                    }
                  },
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    rlc-Config um-Uni-Directional-DL : 
                      {
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms25
                        }
                      },
                    logicalChannelIdentity 10
                  }
                }
              }
            }
      }
}
*/

byte llc_reconfig_duplicate_drb_msg[18] = {0x22, 0x02, 0x30, 0x3E, 0x2F, 0x98, 
  0x94, 0x28, 0x00, 0x00, 0x06, 0x07, 0x64, 0xE0, 0x58, 0xC4, 0xC5, 0xE0};

/*===========================================================================
                Reconfig Message - Setup DRB3 

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                }
              }
            }
      }
}
*/

byte llc_reconfig_drb3_setup_msg[13] = {0x22, 0x02, 0x10, 0x07, 0xCC, 0x4A, 
  0x14, 0x00, 0x00, 0x01, 0x05, 0xD0, 0x10};

/*===========================================================================
                Reconfig Message - Reconfigure SRB1, SRB2 and DRB3 

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
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

byte llc_reconfig_srb1_srb2_drb3_msg[16] = {0x22, 0x02, 0x30, 0xA0, 0x00, 0x00, 
  0x00, 0x03, 0x20, 0x70, 0x02, 0x08, 0x08, 0xCC, 0x00, 0x00};

/*
TTI_Msg:value DL-DCCH-Message ::= 
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
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    timeAlignmentTimerDedicated sf500
                  },
                sps-Config 
                {
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
                      }
                    }
                }
              },
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  fullConfig-r9 true,
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

byte llc_reconfig_TTI_msg[32] = {0x22, 0x02, 0x86, 0x20, 0x21, 0xC0, 0x40, 0x09, 0x40, 0x41, 
    0xEA, 0xD0, 0xE0, 0x00, 0x00, 0x00, 0x81, 0x53, 0x93, 0x00, 0x01, 0x21, 0x29, 0xB3, 0x08, 0x04, 0x40, 0x0B, 0x80, 0xD0, 0x22};

/*
TM9_with_SRB:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
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
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
                      },
                      headerCompression rohc : 
                        {
                          maxCID 1,
                          profiles 
                          {
                            profile0x0001 TRUE,
                            profile0x0002 TRUE,
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 2,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 1
                      }
                    }
                  }
                },
                mac-MainConfig explicitValue : 
                  {
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    timeAlignmentTimerDedicated sf500
                  },
                sps-Config 
                {
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
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      codebookSubsetRestriction-r10 '01101110 00000000 00000000 00000000 000 ...'B,
                      ue-TransmitAntennaSelection setup : openLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm12
                      },
                    nomPDSCH-RS-EPRE-Offset 0,
                    pmi-RI-Report-r9 setup
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an8,
                        resourceConfig-r10 2,
                        subframeConfig-r10 1,
                        p-C-r10 0
                    }
                }
                }
              }
            }
      }
}

*/

byte llc_reconfig_tm9_cbr_109_msg[48] = {0x22, 0x02, 0x37, 0x3E, 0x0F, 0x98, 0x94, 0x28, 0x00, 0x18, 0x02, 0x0B, 0xB1, 0x20, 0x90, 0x10, 0xE0, 0x20, 0x04, 
       0xA0, 0x20, 0xF6, 0x00, 0x02, 0x42, 0xD3, 0x01, 0x86, 0xD6, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x75, 0x03, 
       0x71, 0x00, 0xA0, 0x00};

/*
tm9_valid:value DL-DCCH-Message ::= 
              {
  message c1 : rrcConnectionReconfiguration : 
                {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
                  {
              radioResourceConfigDedicated 
                    {
                srb-ToAddModList 
                      {
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
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
                            },
                      headerCompression rohc : 
                            {
                          maxCID 1,
                          profiles 
                          {
                            profile0x0001 TRUE,
                            profile0x0002 TRUE,
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                            }
                          },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                          {
                      ul-SpecificParameters 
                            {
                        priority 2,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 1
                      }
                    }
                  }
                            },
                mac-MainConfig explicitValue : 
                            {
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                            },
                    timeAlignmentTimerDedicated sf500
                  },
                sps-Config 
                              {
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
                      }
                    }
                              },
                physicalConfigDedicated 
                              {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      codebookSubsetRestriction-r10 '01100100 00000000 00000000 00000000 000 ...'B,
                      ue-TransmitAntennaSelection setup : openLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm12
                      },
                    nomPDSCH-RS-EPRE-Offset 0,
                    pmi-RI-Report-r9 setup
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an4,
                        resourceConfig-r10 2,
                        subframeConfig-r10 1,
                        p-C-r10 0
                              }
                            }
                          }
              }
            }
      }
}

*/

byte llc_reconfig_tm9_cbr_64_msg[42] = {0x22, 0x02, 0x37, 0x3E, 0x0F, 0x98, 0x94, 0x28, 0x00, 0x18, 0x02, 0x0B, 0xB1, 0x20, 
                                                              0x90, 0x10, 0xE0, 0x20, 0x04, 0xA0, 0x20, 0xF6, 0x00, 0x02, 0x42, 0x13, 0x01, 0x84, 
                                                              0x06, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0xA0, 0x6C, 0x20, 0x14, 0x00,};

/*
Scell_with_invalid_cqi:value DL-DCCH-Message ::= 
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
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    timeAlignmentTimerDedicated sf500
                  },
                sps-Config 
                {
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
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  cqi-ReportConfig 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic release : NULL
                  },
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : openLoop
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
                          dl-CarrierFreq-r10 100
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
                                transmissionMode-r10 tm4,
                                codebookSubsetRestriction-r10 '000000'B,
                                ue-TransmitAntennaSelection setup : closedLoop
                              }
                            },
                            ul-Configuration-r10 
                            {
                              cqi-ReportConfigSCell-r10 
                              {
                                cqi-ReportModeAperiodic-r10 rm12,
                                nomPDSCH-RS-EPRE-Offset-r10 0
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

byte llc_scell_cqi_bw_test[32] = {0x22, 0x02, 0x87, 0x20, 0x21, 0xC0, 0x40, 0x09, 0x40, 0x41, 0xEC, 0x08, 0x48, 0x12, 
  0x03, 0x80, 0x04, 0x60, 0x4A, 0x1C, 0x00, 0x00, 0x19, 0x00, 0x2A, 0x72, 0xB8, 0x98, 0x30, 0x10, 0x88, 0x08};

/*
Scell_with_invalid_cqi:value DL-DCCH-Message ::= 
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
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    timeAlignmentTimerDedicated sf500
                  },
                sps-Config 
                {
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
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  cqi-ReportConfig 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic release : NULL
                  },
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : openLoop
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
                          dl-CarrierFreq-r10 100
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
                                transmissionMode-r10 tm7,
                                codebookSubsetRestriction-r10 '000000'B,
                                ue-TransmitAntennaSelection setup : closedLoop
                              }
                            },
                            ul-Configuration-r10 
                            {
                              cqi-ReportConfigSCell-r10 
                              {
                                cqi-ReportModeAperiodic-r10 rm12,
                                nomPDSCH-RS-EPRE-Offset-r10 0
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

byte llc_scell_tm7_test[32] = {0x22, 0x02, 0x87, 0x20, 0x21, 0xC0, 0x40, 0x09, 0x40, 0x41, 0xEC, 0x08, 0x48, 0x12, 0x03, 0x80, 0x04, 
       0x60, 0x4A, 0x1C, 0x00, 0x00, 0x19, 0x00, 0x2A, 0x72, 0xB8, 0xB0, 0x30, 0x10, 0x88, 0x08};

/*
tm9_6:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                srb-ToAddModList 
                {
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
                      discardTimer ms50,
                      rlc-UM 
                              {
                        pdcp-SN-Size len7bits
                      },
                      headerCompression rohc : 
                        {
                          maxCID 1,
                          profiles 
                          {
                            profile0x0001 TRUE,
                            profile0x0002 TRUE,
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 2,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 1
                              }
                            }
                          }
                },
                mac-MainConfig explicitValue : 
                  {
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    timeAlignmentTimerDedicated sf500
                  },
                sps-Config 
                {
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
                        }
                      }
                },
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      codebookSubsetRestriction-r10 '000010'B,
                      ue-TransmitAntennaSelection setup : openLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm12
                      },
                    nomPDSCH-RS-EPRE-Offset 0,
                    pmi-RI-Report-r9 setup
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an2,
                        resourceConfig-r10 2,
                        subframeConfig-r10 1,
                        p-C-r10 0
                    }
                  }
                }
              }
            }
      }
}
*/

byte llc_reconfig_tm9_cbr_6_msg[35] = {0x22, 0x02, 0x37, 0x3E, 0x0F, 0x98, 0x94, 0x28, 0x00, 0x18, 0x02, 0x0B, 0xB1, 
                                                            0x20, 0x90, 0x10, 0xE0, 0x20, 0x04, 0xA0, 0x20, 0xF6, 0x00, 0x02, 0x41, 0x33, 
                                                            0x01, 0x80, 0x60, 0xBA, 0x81, 0xA8, 0x80, 0x50, 0x00,}; 
                                                        

/*
TM9_wo_csi:value DL-DCCH-Message ::= 
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
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    timeAlignmentTimerDedicated sf500
                  },
                sps-Config 
                {
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
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : openLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-pmi-ConfigIndex 38,
                        cqi-FormatIndicatorPeriodic-r10 subbandCQI-r10 : 
                          {
                            k 4,
                            periodicityFactor-r10 n2
                          },
                        ri-ConfigIndex 171,
                        simultaneousAckNackAndCQI TRUE
                      },
                    pmi-RI-Report-r9 setup
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 release : NULL
                  }
                }
              }
            }
      }
}
*/

byte llc_reconfig_tm9_wo_csi_msg[24] = {0x22, 0x02, 0x07, 0x20, 0x21, 0xC0, 0x40, 
    0x09, 0x40, 0x41, 0xEC, 0x00, 0x04, 0x82, 0x66, 0x01, 0x1B, 0x1A, 0x00, 0x00, 
    0x9B, 0x8A, 0xBC, 0x00,};

/*
Reconfig_wo_csi:value DL-DCCH-Message ::= 
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
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    timeAlignmentTimerDedicated sf500
                  },
                sps-Config 
                {
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
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 release : NULL
                  }
                }
              }
            }
      }
}
*/

byte llc_reconfig_tm9_wo_csi_pmi_msg[17] = {0x22, 0x02, 0x07, 0x20, 0x21, 0xC0, 0x40, 0x09, 0x40, 0x41, 0xEC, 0x00, 0x04, 0x80, 0x82, 0x04, 0x00};


/*TTI_Msg:value DL-DCCH-Message ::= 
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
                    ul-SCH-Config 
                    {
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    timeAlignmentTimerDedicated sf500
                  }
              },
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  fullConfig-r9 true,
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

byte llc_reconfig_PHR_msg[25] = {0x22, 0x02, 0x84, 0x20, 0x21, 0x68, 0x70, 0x00, 0x00, 0x00, 
    0x40, 0xA9, 0xC9, 0x80, 0x00, 0x90, 0x94, 0xD9, 0x84, 0x02, 0x20, 0x05, 0xC0, 0x68, 0x11};


/*
Reconfig:value DL-DCCH-Message ::= 
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
                  fullConfig-r9 true,
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

byte llc_reconfig_ca_scell_add_msg[23] = { 0x22, 0x00, 0xAD, 0x0E, 0x00, 0x00, 0x00, 0x08, 0x15, 0x39, 
    0x30, 0x00, 0x12, 0x12, 0x9B, 0x30, 0x80, 0x44, 0x00, 0xB8, 0x0D, 0x02, 0x20 };

/*
scell:value DL-DCCH-Message ::= 
{
scell:value DL-DCCH-Message ::= 
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
byte llc_reconfig_ca_scell_add_with_apsrs_msg[12] = {0x22, 0x00, 0xA5, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x15, 0x39, 0x54, 0x06
};
/*===========================================================================
               Reconfig Message - Reconfigure PHY, MAC and RLC DRB3

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    drb-Identity 3,
                    rlc-Config um-Uni-Directional-DL : 
                      {
                        dl-UM-RLC 
                        {
                          sn-FieldLength size10,
                          t-Reordering ms25
                        }
                      }
                  }
                },
                mac-MainConfig explicitValue : 
                  {
                    timeAlignmentTimerDedicated sf5120
                  },
                physicalConfigDedicated 
                {
                  uplinkPowerControlDedicated 
                  {
                    p0-UE-PUSCH 2,
                    deltaMCS-Enabled en1,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 3,
                    pSRS-Offset 1
                  }
                }
              }
            }
      }
}
*/

byte llc_reconfig_phy_mac_drb3_rlc_msg[11] = {0x22, 0x02, 0x15, 0x01, 0x04, 
  0xE5, 0x05, 0x08, 0x0A, 0xEC, 0x40};

/*===========================================================================
                    Reconfig Message - DRB3 Release 

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
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
              }
            }
      }
}
*/
byte llc_reconfig_with_drb_rel_msg[5] = {0x22, 0x02, 0x08, 0x01, 0x00};

/*===========================================================================
            Reconfig Message - SRB1 recfg and SRB2 Setup 

============================================================================*/

/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
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
                }
              }
            }
      }
}
*/

byte llc_config_srb1_recfg_srb2_setup[5] = {0x22, 0x02, 0x20, 0xB6, 0xF8};



/*===========================================================================
            Reconfig Message - DRB3 release and DRB4 setup

============================================================================*/

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
                '0001'H
              },
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 4,
                    drb-Identity 4,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 2,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                },
                drb-ToReleaseList 
                {
                  3
                }
              }
            }
      }
}
*/

byte llc_reconfig_drb3_release_drb4_setup_msg[18] = {0x22, 0x06, 0x00, 0x10, 
  0x00, 0x09, 0x80, 0x7D, 0x06, 0xA1, 0x40, 0x00, 0x00, 0x10, 0x5D, 0x09, 0x00, 
  0x10};

/*===========================================================================
            Reconfig Message - DRB3 Reconfig, DRB4 Setup
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
                '0001'H
              },
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    drb-Identity 3,
                    rlc-Config um-Bi-Directional : 
                      {
                        ul-UM-RLC 
                        {
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 1
                      }
                    }
                  },
                  {
                    eps-BearerIdentity 4,
                    drb-Identity 4,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
                      },
                      headerCompression rohc : 
                        {
                          maxCID 5,
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
                          t-Reordering ms20
                        }
                      },
                    logicalChannelIdentity 8,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 3,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 1
                      }
                    }
                  }
                }
              }
            }
      }
}
*/

byte llc_reconfig_drb3_drb4_setup_msg[22] = {0x22, 0x06, 0x00, 0x10, 0x00, 0x09, 
  0x01, 0x1C, 0x44, 0x17, 0x60, 0x41, 0x7D, 0x06, 0xA1, 0x40, 0x04, 0x00, 0x1C, 
  0x95, 0x91, 0x04};

/*===========================================================================
            Reconfig Message - DRB3 Reconfig, DRB4 & DRB5 Setup
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
                '0001'H
              },
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    rlc-Config um-Bi-Directional : 
                      {
                        ul-UM-RLC 
                        {
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 1
                      }
                    }
                  },
                  {
                    eps-BearerIdentity 4,
                    drb-Identity 4,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
                      },
                      headerCompression rohc : 
                        {
                          maxCID 5,
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
                          t-Reordering ms20
                        }
                      },
                    logicalChannelIdentity 8,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 3,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 1
                      }
                    }
                  },
                  {
                    eps-BearerIdentity 5,
                    drb-Identity 5,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
                      },
                      headerCompression rohc : 
                        {
                          maxCID 5,
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 6,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 5,
                        prioritisedBitRate kBps8,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 2
                      }
                    }
                  }
                }
              }
            }
      }
}
*/

byte llc_reconfig_drb3_drb4_drb5_setup_msg[32] = {0x22, 0x06, 0x00, 0x10, 0x00, 
  0x09, 0x02, 0x5C, 0xC4, 0x41, 0x76, 0x04, 0x17, 0xD0, 0x6A, 0x14, 0x00, 0x40, 
  0x01, 0xC9, 0x59, 0x10, 0x5F, 0x52, 0x28, 0x50, 0x01, 0x00, 0x04, 0x13, 0x68, 
  0x22};

/*===========================================================================
            Reconfig Message - DRB3 PDCP Reconfig, DRB4 & DRB5 Setup
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
                '0001'H
              },
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      headerCompression rohc : 
                        {
                          maxCID 5,
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 1
                      }
                    }
                  },
                  {
                    eps-BearerIdentity 4,
                    drb-Identity 4,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
                      },
                      headerCompression rohc : 
                        {
                          maxCID 5,
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
                          t-Reordering ms20
                        }
                      },
                    logicalChannelIdentity 8,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 3,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 1
                      }
                    }
                  },
                  {
                    eps-BearerIdentity 5,
                    drb-Identity 5,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
                      },
                      headerCompression rohc : 
                        {
                          maxCID 5,
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 6,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 5,
                        prioritisedBitRate kBps8,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 2
                      }
                    }
                  }
                }
              }
            }
      }
}
*/

byte llc_reconfig_drb3_pdcp_drb4_drb5_setup_msg[36] = {0x22, 0x06, 0x00, 0x10, 
  0x00, 0x09, 0x02, 0x7C, 0xC4, 0x14, 0x00, 0x40, 0x01, 0x05, 0xD8, 0x10, 0x5F, 
  0x41, 0xA8, 0x50, 0x01, 0x00, 0x07, 0x25, 0x64, 0x41, 0x7D, 0x48, 0xA1, 0x40, 
  0x04, 0x00, 0x10, 0x4D, 0xA0, 0x88};

/*===========================================================================
            Reconfig Message - DRB Release for DRB3, DRB4 and DRB5
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
                '0001'H
              },
              radioResourceConfigDedicated 
              {
                drb-ToReleaseList 
                {
                  3,
                  4,
                  5
                }
              }
            }
      }
}
*/
byte llc_reconfig_drb3_drb4_drb5_rel_msg[9] = {0x22, 0x06, 0x00, 0x10, 0x00, 
  0x08, 0x82, 0x10, 0xC8};

/*===========================================================================
            Reconfig Message - Same DRB DL Released and UL Setup
===========================================================================*/
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
              },
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
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
                    rlc-Config um-Uni-Directional-UL : 
                      {
                        ul-UM-RLC 
                        {
                          sn-FieldLength size5
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                },
                drb-ToReleaseList 
                {
                  3
                }
              },
              nonCriticalExtension 
              {
              }
            }
      }
}
*/

byte llc_reconfig_same_drb_dl_released_ul_setup_msg[15] = {0x22, 0x12, 0x00, 
  0x01, 0x80, 0x7C, 0xC4, 0xA1, 0x40, 0x00, 0x00, 0x27, 0x40, 0x40, 0x04};

/*===========================================================================
            Reconfig Message - Same DRB Released and Setup
===========================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                },
                drb-ToReleaseList 
                {
                  3
                }
              }
            }
      }
}
*/

byte llc_reconfig_same_drb_released_setup_msg[15] = {0x22, 0x02, 0x18, 0x07, 
  0xCC, 0x4A, 0x14, 0x00, 0x00, 0x01, 0x05, 0xD0, 0x10, 0x01, 0x00};

/*===========================================================================
            Invalid Reconfig Message - DRB Setup without RLC Config
===========================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 1,
                    drb-Identity 1,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
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
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                }
              }
            }
      }
}
*/

byte llc_invalid_drb_no_rlc_msg[12] = {0x22, 0x02, 0x10, 0x06, 0xC4, 0x0A, 0x14, 
  0x00, 0x00, 0x07, 0x40, 0x40};

/*===========================================================================
            Invalid Reconfig Message - Multiple DRBs on a single LC
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
                '0001'H
              },
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 5,
                    drb-Identity 5,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
                      },
                      headerCompression notUsed : NULL
                    },
                    rlc-Config um-Bi-Directional : 
                      {
                        ul-UM-RLC 
                        {
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 8,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 1
                      }
                    }
                  },
                  {
                    eps-BearerIdentity 4,
                    drb-Identity 4,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
                      },
                      headerCompression rohc : 
                        {
                          maxCID 5,
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
                          t-Reordering ms20
                        }
                      },
                    logicalChannelIdentity 8,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 3,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 1
                      }
                    }
                  }
                }
              }
            }
      }
}
*/

byte llc_invalid_multiple_rb_single_lc[24] = {0x22, 0x06, 0x00, 0x10, 0x00, 
  0x09, 0x01, 0x7D, 0x48, 0xA0, 0x20, 0xAB, 0x02, 0x0B, 0xE8, 0x35, 0x0A, 0x00, 
  0x20, 0x00, 0xE4, 0xAC, 0x88, 0x20}; 

/*===========================================================================
            Invalid Reconfig Message - RB Mode change
===========================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    drb-Identity 3,
                    rlc-Config um-Bi-Directional : 
                      {
                        ul-UM-RLC 
                        {
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      }
                  }
                }
              }
            }
      }
}
*/

byte llc_invalid_rb_mode_change[7] = {0x22, 0x02, 0x10, 0x01, 0x04, 0x41, 0x00}; 

/*===========================================================================
            Invalid Reconfig Message - DRB Setup without PDCP Config
===========================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 1,
                    drb-Identity 1,
                    rlc-Config um-Bi-Directional : 
                      {
                        ul-UM-RLC 
                        {
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                }
              }
            }
      }
}
*/

byte llc_invalid_drb_no_pdcp_msg[9] = {0x22, 0x02, 0x10, 0x05, 0xC4, 0x04, 0x17, 
  0x40, 0x40};

/*===========================================================================
      Invalid Reconfig Message - DRB Reconfig with PDCP Configuration
===========================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                }
              }
            }
      }
}
*/

byte llc_reconfig_unwanted_pdcp_msg[13] = {0x22, 0x02, 0x10, 0x07, 0xCC, 0x4A, 
  0x14, 0x00, 0x00, 0x01, 0x05, 0xD0, 0x10};

/*===========================================================================
   Invalid Reconfig Message - Bi-directional DRB Setup without discard timer
===========================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 1,
                    drb-Identity 1,
                    pdcp-Config 
                    {
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                }
              }
            }
      }
}
*/

byte llc_invalid_drb_no_discard_timer_msg[13] = {0x22, 0x02, 0x10, 0x07, 0xC4, 
  0x02, 0xA0, 0x00, 0x00, 0x08, 0x2E, 0x80, 0x80}; 

/*===========================================================================
      Invalid Reconfig Message - DRB Setup without Logical Channel Config
===========================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 1,
                    drb-Identity 1,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10
                  }
                }
              }
            }
      }
}
*/

byte llc_invalid_drb_no_logchan_msg[12] = {0x22, 0x02, 0x10, 0x07, 0x84, 0x0A, 
  0x14, 0x00, 0x00, 0x01, 0x05, 0xC0};

/*===========================================================================
         Invalid Reconfig Message - DRB Setup without RB Mapping
===========================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 1,
                    drb-Identity 1,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                }
              }
            }
      }
}
*/

byte llc_invalid_drb_no_rbmap_msg[13] = {0x22, 0x02, 0x10, 0x07, 0x44, 0x0A, 
  0x14, 0x00, 0x00, 0x01, 0x04, 0x80, 0x80};

/*===========================================================================
          Reconfig Message with Mobility Related Information alone
===========================================================================*/
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
                  dl-CarrierFreq 500,
                  ul-CarrierFreq 18500
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

byte llc_reconfig_intra_freq_mobility_info_only_msg[40] = {0x22, 0x0B, 0x3C, 0x15, 0x01, 0xF4, 0x48, 0x44, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xEC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14,
0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x10, 0x01, 0x10, 0x04, 0x04, 0x00,};

/*===========================================================================
                    Reconfig Message - HO with Full Config 

============================================================================*/
  /*
  CQIREL10:value DL-DCCH-Message ::= 
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
                    dl-CarrierFreq 500,
                    ul-CarrierFreq 18500
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
                        },
                        logicalChannelSR-Mask-r9 setup
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
                        },
                      sr-ProhibitTimer-r9 4
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
                        }
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
                    antennaInfo explicitValue : 
                      {
                        transmissionMode tm8-v920,
                        ue-TransmitAntennaSelection setup : closedLoop
                      },
                    schedulingRequestConfig setup : 
                      {
                        sr-PUCCH-ResourceIndex 0,
                        sr-ConfigIndex 157,
                        dsr-TransMax n4
                      },
                    cqi-ReportConfig-v920 
                    {
                    },
                    antennaInfo-v920 
                    {
                      codebookSubsetRestriction-v920 n4TxAntenna-tm8-r9 : '00001111 00001111 00000000 00000111'B
                    },
                    cqi-ReportConfig-r10 
                    {
                      nomPDSCH-RS-EPRE-Offset 0,
                      cqi-ReportPeriodic-r10 setup : 
                        {
                          cqi-PUCCH-ResourceIndex-r10 0,
                          cqi-pmi-ConfigIndex 150,
                          cqi-FormatIndicatorPeriodic-r10 widebandCQI-r10 : 
                            {
                            },
                          simultaneousAckNackAndCQI FALSE
                        }
                    }
                  },
                  rlf-TimersAndConstants-r9 setup : 
                    {
                      t301-r9 ms600,
                      t310-r9 ms500,
                      n310-r9 n4,
                      t311-r9 ms15000,
                      n311-r9 n4
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

 byte llc_reconfig_ho_msg[113] = {0x22, 0x0B, 0x3C, 0x15, 0x01, 0xF4, 0x48, 0x44, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xCC, 
   0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14, 0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x00, 
   0x8F, 0xF2, 0x41, 0xF5, 0x02, 0x9D, 0x00, 0x00, 0x00, 0x71, 0x0C, 0x44, 0x00, 0x40, 0x60, 0x00, 0x4F, 0xD0, 0x85, 0x60, 0x00, 
   0x60, 0x1C, 0x0E, 0x02, 0x01, 0x01, 0x00, 0x25, 0x01, 0x07, 0xBF, 0xFD, 0x60, 0x00, 0x00, 0x03, 0x68, 0x08, 0x08, 0x00, 0x20, 
   0x20, 0x00, 0xC9, 0xC0, 0x00, 0x01, 0x00, 0x06, 0x40, 0x00, 0x0C, 0xF4, 0x00, 0x4E, 0x80, 0x58, 0x17, 0x30, 0xF0, 0xF0, 0x00, 
   0x70, 0x18, 0x40, 0x21, 0x80, 0x00, 0x25, 0x80, 0x0C, 0x07, 0xA4, 0x71, 0x80, 0x40, 0x40,};
  
  

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
                  dl-CarrierFreq 500,
                  ul-CarrierFreq 18500
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
                      },
                      logicalChannelSR-Mask-r9 setup
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
                      },
                    sr-ProhibitTimer-r9 4
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
                      }
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
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm8-v920,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 157,
                      dsr-TransMax n4
                    },
                  cqi-ReportConfig-v920 
                  {
                  },
                  antennaInfo-v920 
                  {
                    codebookSubsetRestriction-v920 n4TxAntenna-tm8-r9 : '00001111 00001111 00000000 00000111'B
                  }
                },
                rlf-TimersAndConstants-r9 setup : 
                  {
                    t301-r9 ms600,
                    t310-r9 ms500,
                    n310-r9 n4,
                    t311-r9 ms15000,
                    n311-r9 n4
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

byte llc_reconfig_ho_msg2[106] = {0x22, 0x0B, 0x3C, 0x15, 0x01, 0xF4, 0x48, 0x44, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xCC, 
  0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14, 0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x00, 
  0x8F, 0xF2, 0x41, 0xF5, 0x02, 0x9D, 0x00, 0x00, 0x00, 0x71, 0x0C, 0x44, 0x00, 0x40, 0x60, 0x00, 0x4F, 0xD0, 0x85, 0x60, 0x00, 
  0x20, 0x38, 0x1C, 0x04, 0x02, 0x02, 0x00, 0x4A, 0x02, 0x0F, 0x7F, 0xFA, 0xC0, 0x00, 0x00, 0x06, 0xD0, 0x10, 0x10, 0x00, 0x40, 
  0x40, 0x01, 0x93, 0x80, 0x00, 0x02, 0x00, 0x0C, 0x80, 0x00, 0x19, 0xE8, 0x00, 0x9D, 0x00, 0x20, 0xB9, 0x87, 0x87, 0x80, 0x03, 
  0x80, 0x20, 0x7A, 0x47, 0x18, 0x04, 0x04, 0x00,};

byte llc_reconfig_ho_msg21[106] = {0x22, 0x0B, 0x3C, 0x15, 0x01, 0xF4, 0x48, 0x44, 0x91, 0x25, 0x00,
  0x80, 0x3F, 0xCC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14, 0x68, 0x16, 0x00, 0x52,
  0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x00, 0x8F, 0xF2, 0x41, 0xF5, 0x02, 0x9D, 0x00, 0x00, 0x00, 0x71, 0x0C, 0x44,
  0x00, 0x40, 0x60, 0x00, 0x4F, 0xD0, 0x85, 0x60, 0x00, 0xA0, 0x0E, 0x07, 0x01, 0x00, 0x80, 0x80, 0x12, 0x80,
  0x83, 0xDF, 0xFA, 0xB0, 0x00, 0x00, 0x01, 0xB4, 0x04, 0x04, 0x00, 0x10, 0x10, 0x00, 0x64, 0xE0, 0x00, 0x00,
  0x80, 0x03, 0x20, 0x00, 0x07, 0x00, 0x13, 0xA0, 0x1C, 0x02, 0xE6, 0x1E, 0x1E, 0x00, 0x0E, 0x02, 0x80, 0x7A,
  0x47, 0x18, 0x04, 0x04, 0x00,};
/*
Full_config_msg3:value DL-DCCH-Message ::= 
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
                  dl-CarrierFreq 500,
                  ul-CarrierFreq 18500
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
                      },
                      logicalChannelSR-Mask-r9 setup
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
                      },
                    sr-ProhibitTimer-r9 4
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
                      }
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
                      }
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
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm8-v920,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 157,
                      dsr-TransMax n4
                    },
                  cqi-ReportConfig-v920 
                  {
                  },
                  antennaInfo-v920 
                  {
                    codebookSubsetRestriction-v920 n4TxAntenna-tm8-r9 : '00001111 00001111 00000000 00000111'B
                  },
                  pucch-ConfigDedicated-v1020 
                  {
                    pucch-Format-r10 format3-r10 : 
                      {
                        n3PUCCH-AN-List-r10 
                        {
                          0
                        }
                      }
                  }
                },
                rlf-TimersAndConstants-r9 setup : 
                  {
                    t301-r9 ms600,
                    t310-r9 ms500,
                    n310-r9 n4,
                    t311-r9 ms15000,
                    n311-r9 n4
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

byte llc_reconfig_ho_msg3[111] = {0x22, 0x0B, 0x3C, 0x15, 0x01, 0xF4, 0x48, 0x44, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xCC, 
  0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14, 0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x00, 
  0x8F, 0xF2, 0x41, 0xF5, 0x02, 0x9D, 0x00, 0x00, 0x00, 0x71, 0x0C, 0x44, 0x00, 0x40, 0x60, 0x00, 0x4F, 0xD0, 0x85, 0x60, 0x00, 
  0x60, 0x1C, 0x0E, 0x02, 0x01, 0x01, 0x00, 0x25, 0x01, 0x07, 0xBF, 0xFD, 0x20, 0x00, 0x00, 0x06, 0xD0, 0x10, 0x10, 0x00, 0x40, 
  0x40, 0x01, 0x93, 0x80, 0x00, 0x02, 0x00, 0x0C, 0x80, 0x00, 0x19, 0xE8, 0x00, 0x9D, 0x00, 0xB0, 0x2E, 0x61, 0xE1, 0xE0, 0x00, 
  0xE0, 0x20, 0x20, 0x84, 0x00, 0x00, 0x18, 0x0F, 0x48, 0xE3, 0x00, 0x80, 0x80,};

/*
Full_config_msg3:value DL-DCCH-Message ::= 
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
                  dl-CarrierFreq 500,
                  ul-CarrierFreq 18500
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
                      },
                      logicalChannelSR-Mask-r9 setup
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
                      },
                    sr-ProhibitTimer-r9 4
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
                      }
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
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm8-v920,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 157,
                      dsr-TransMax n4
                    },
                  cqi-ReportConfig-v920 
                  {
                  },
                  antennaInfo-v920 
                  {
                    codebookSubsetRestriction-v920 n4TxAntenna-tm8-r9 : '00001111 00001111 00000000 00000111'B
                  },
                  pucch-ConfigDedicated-v1020 
                  {
                    pucch-Format-r10 format3-r10 : 
                      {
                        n3PUCCH-AN-List-r10 
                        {
                          0
                        }
                      }
                  }
                },
                rlf-TimersAndConstants-r9 setup : 
                  {
                    t301-r9 ms600,
                    t310-r9 ms500,
                    n310-r9 n4,
                    t311-r9 ms15000,
                    n311-r9 n4
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

byte llc_reconfig_ho_msg4[111] = {0x22, 0x0B, 0x3C, 0x15, 0x01, 0xF4, 0x48, 0x44, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xCC, 
  0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14, 0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x00, 
  0x8F, 0xF2, 0x41, 0xF5, 0x02, 0x9D, 0x00, 0x00, 0x00, 0x71, 0x0C, 0x44, 0x00, 0x40, 0x60, 0x00, 0x4F, 0xD0, 0x85, 0x60, 0x00, 
  0x60, 0x1C, 0x0E, 0x02, 0x01, 0x01, 0x00, 0x25, 0x01, 0x07, 0xBF, 0xFD, 0x60, 0x00, 0x00, 0x03, 0x68, 0x08, 0x08, 0x00, 0x20, 
  0x20, 0x00, 0xC9, 0xC0, 0x00, 0x01, 0x00, 0x06, 0x40, 0x00, 0x0C, 0xF4, 0x00, 0x4E, 0x80, 0x58, 0x17, 0x30, 0xF0, 0xF0, 0x00, 
  0x70, 0x10, 0x10, 0x42, 0x00, 0x00, 0x0C, 0x07, 0xA4, 0x71, 0x80, 0x40, 0x40,};


/*
HO_MSG:value DL-DCCH-Message ::= 
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
                  dl-CarrierFreq 500,
                  ul-CarrierFreq 18500
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
                      },
                      logicalChannelSR-Mask-r9 setup
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
                      },
                    sr-ProhibitTimer-r9 4
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
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB-3
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
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm8-v920,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 157,
                      dsr-TransMax n4
                    },
                  cqi-ReportConfig-v920 
                  {
                  },
                  antennaInfo-v920 
                  {
                    codebookSubsetRestriction-v920 n4TxAntenna-tm8-r9 : '00001111 00001111 00000000 00000111'B
                  },
                  pucch-ConfigDedicated-v1020 
                  {
                    pucch-Format-r10 channelSelection-r10 : 
                      {
                        n1PUCCH-AN-CS-r10 release : NULL
                      }
                  }
                },
                rlf-TimersAndConstants-r9 setup : 
                  {
                    t301-r9 ms600,
                    t310-r9 ms500,
                    n310-r9 n4,
                    t311-r9 ms15000,
                    n311-r9 n4
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

byte llc_reconfig_ho_msg41[108] = {0x22, 0x0B, 0x3C, 0x15, 0x01, 0xF4, 0x48, 0x44, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xCC, 
  0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14, 0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x00, 
  0x8F, 0xF2, 0x41, 0xF5, 0x02, 0x9D, 0x00, 0x00, 0x00, 0x71, 0x0C, 0x44, 0x00, 0x40, 0x60, 0x00, 0x4F, 0xD0, 0x85, 0x60, 0x00, 
  0x60, 0x1C, 0x0E, 0x02, 0x01, 0x01, 0x00, 0x25, 0x01, 0x07, 0xBB, 0xFD, 0x00, 0x03, 0x68, 0x08, 0x08, 0x00, 0x20, 0x20, 0x00, 
  0xC9, 0xC0, 0x00, 0x01, 0x00, 0x06, 0x40, 0x00, 0x0C, 0xF4, 0x00, 0x4E, 0x80, 0x58, 0x17, 0x30, 0xF0, 0xF0, 0x00, 0x70, 0x0C, 
  0x10, 0x46, 0x00, 0x0C, 0x07, 0xA4, 0x71, 0x80, 0x40, 0x40,};

/*
HO_FULL_Config_Mas:value DL-DCCH-Message ::= 
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
                  dl-CarrierFreq 500,
                  ul-CarrierFreq 18500
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
                      },
                      logicalChannelSR-Mask-r9 setup
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
                      ttiBundling FALSE
                    },
                    drx-Config release : NULL,
                    timeAlignmentTimerDedicated sf5120,
                    phr-Config setup : 
                      {
                        periodicPHR-Timer sf10,
                        prohibitPHR-Timer sf0,
                        dl-PathlossChange dB1
                      },
                    sr-ProhibitTimer-r9 4
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
                      }
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
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm8-v920,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 157,
                      dsr-TransMax n4
                    },
                  cqi-ReportConfig-v920 
                  {
                  },
                  antennaInfo-v920 
                  {
                    codebookSubsetRestriction-v920 n4TxAntenna-tm8-r9 : '00001111 00001111 

00000000 00000111'B
                  }
                },
                rlf-TimersAndConstants-r9 setup : 
                  {
                    t301-r9 ms600,
                    t310-r9 ms500,
                    n310-r9 n4,
                    t311-r9 ms15000,
                    n311-r9 n4
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

byte llc_reconfig_ho_msg1[106] = {0x22, 0x0B, 0x3C, 0x15, 0x01, 0xF4, 0x48, 0x44, 
    0x91, 0x25, 0x00, 0x80, 0x3F, 0xCC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 
    0x01, 0x14, 0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x00, 0x8F, 0xF2, 0x41, 
    0xF5, 0x02, 0x9D, 0x00, 0x00, 0x00, 0x71, 0x0C, 0x44, 0x00, 0x40, 0x60, 0x00, 0x4F, 0xD0, 
    0x81, 0x60, 0x00, 0x60, 0x1C, 0x0E, 0x02, 0x01, 0x01, 0x00, 0x25, 0x01, 0x07, 0xBF, 0xFD, 
    0x60, 0x00, 0x00, 0x03, 0x68, 0x08, 0x08, 0x00, 0x20, 0x20, 0x00, 0xC9, 0xC0, 0x00, 0x01, 
    0x00, 0x06, 0x40, 0x00, 0x0C, 0xF4, 0x00, 0x4E, 0x80, 0x50, 0x17, 0x30, 0xF0, 0xF0, 0x00, 
    0x70, 0x0C, 0x07, 0xA4, 0x71, 0x80, 0x40, 0x40};

byte llc_reconfig_ho_msg_tdd[106] = {
  0x22, 0x0B, 0x3C, 0x15, 0x9D, 0x80, 0x9D, 0x80, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xCC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14,
  0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x00, 
  0x8F, 0xF2, 0x41, 0xF5, 0x02, 0x9D, 0x00, 0x00, 0x00, 0x71, 0x0C, 0x44, 0x00, 0x40, 0x60, 0x00, 0x4F, 0xD0, 0x81, 0x60, 0x00, 
  0x60, 0x1C, 0x0E, 0x02, 0x01, 0x01, 0x00, 0x25, 0x01, 0x07, 0xBF, 0xFD, 0x60, 0x00, 0x00, 0x03, 0x68, 0x08,
  0x08, 0x00, 0x20, 0x20, 0x00, 0xC9, 0xC0, 0x00, 0x01, 0x00, 0x06, 0x40, 0x00, 0x0C, 0xF4, 0x00, 0x4E, 0x80, 0x50, 0x17, 0x30, 0xF0, 0xF0, 0x00, 
  0x70, 0x0C, 0x07, 0xA4, 0x71, 0x80, 0x40, 0x40};


byte llc_reconfig_csi_rs_config_msg [45] = {0x22, 0x02, 0x81, 0x80, 0x00, 0x90, 0x96, 
	0xF0, 0x02, 0x86, 0x64, 0x3F, 0xC0, 0xC0, 0x00, 0x80, 0x2B, 0x41, 0x00, 0x22, 
	0x10, 0x00, 0xC0, 0x00, 0x02, 0x50, 0xE0, 0x00, 0x2C, 0xC4, 0x81, 0x53, 0x93, 
	0x00, 0x01, 0x21, 0x29, 0xB3, 0x08, 0x04, 0x40, 0x0B, 0x80, 0xD0, 0x22};


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

byte llc_reconfig_ho_delta_config_msg[17] = {0x22, 0x0B, 0x00, 0x14, 0x80, 0x40, 
  0x00, 0x08, 0x0A, 0x0C, 0x40, 0x10, 0x08, 0xA0, 0x20, 0x04, 0x00};

/*===========================================================================
              Reconfig Message - HO with DRB Add, Modify and Release 

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
                measObjectToAddModList 
                {
                  {
                    measObjectId 1,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 500,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B
                      }
                  }
                },
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
                                a1-Threshold threshold-RSRP : 5
                              },
                            hysteresis 4,
                            timeToTrigger ms40
                          },
                        triggerQuantity rsrp,
                        reportQuantity sameAsTriggerQuantity,
                        maxReportCells 2,
                        reportInterval ms120,
                        reportAmount r1
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 1,
                    measObjectId 1,
                    reportConfigId 1
                  }
                }
              },
              mobilityControlInfo 
              {
                targetPhysCellId 10,
                carrierFreq 
                {
                  dl-CarrierFreq 500,
                  ul-CarrierFreq 18500
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
              dedicatedInfoNASList 
              {
                '010203'H
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
                    eps-BearerIdentity 1,
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
                  },
                  {
                    drb-Identity 4,
                    rlc-Config um-Bi-Directional : 
                      {
                        ul-UM-RLC 
                        {
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
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

byte llc_reconfig_ho_with_drb_add_mod_rel_msg[110] = {0x22, 0x1F, 0x95, 0x00, 0x00, 0x00, 0x46, 0x50, 0x28, 0x00, 0x00, 0x05, 0x20, 0x84, 0x00, 0x00, 0x00, 0x3C, 0x15, 0x01, 0xF4, 0x48, 0x44, 0x91, 0x25,
0x00, 0x80, 0x3F, 0xEC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14, 0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x10,
0x01, 0x10, 0x03, 0x01, 0x02, 0x03, 0x7E, 0x48, 0xBE, 0x20, 0x53, 0xA0, 0x00, 0x00, 0x0E, 0x20, 0x88, 0x80, 0x83, 0x20, 0x80, 0x47, 0xD0, 0x85, 0x60,
0x1C, 0x04, 0x02, 0x02, 0x00, 0x4B, 0x02, 0x0F, 0x5F, 0xFA, 0xC0, 0x00, 0x00, 0x06, 0xD0, 0x10, 0x10, 0x00, 0x40, 0x40, 0x01, 0x93, 0x80, 0x00, 0x02,
0x00, 0x0C, 0x80, 0x00, 0x1E, 0x00, 0x00, 0x01, 0x01, 0x00,};

/*===========================================================================
                    Reconfig Message - HO with PDCP Modification

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
                  dl-CarrierFreq 500
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
                drb-ToAddModList 
                {
                  {
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      rlc-AM 
                      {
                        statusReportRequired FALSE
                      },
                      headerCompression notUsed : NULL
                    }
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

byte llc_reconfig_ho_pdcp_modify_msg[40] = {0x22, 0x0B, 0x3C, 0x14, 0x01, 0xF4, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xEC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14, 0x68, 0x16,
0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x10, 0x01, 0x12, 0x00, 0x40, 0x88, 0x20, 0x20,};

/*===========================================================================
                    Reconfig Message - HO with RLC, PDCP Modification

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
                  dl-CarrierFreq 500,
                  ul-CarrierFreq 18500
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
                drb-ToAddModList 
                {
                  {
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      rlc-AM 
                      {
                        statusReportRequired FALSE
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
                          t-Reordering ms10,
                          t-StatusProhibit ms10
                        }
                      }
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

byte llc_reconfig_ho_rlc_pdcp_modify_msg[46] = {0x22, 0x0B, 0x3C, 0x15, 0x01, 0xF4, 0x48, 0x44, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xEC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14,
0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x10, 0x01, 0x12, 0x00, 0x60, 0x88, 0x00, 0x00, 0x02, 0x08, 0x80, 0x80,};

/*===========================================================================
                                HO to SIMO 
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
                  antennaInfoCommon 
                  {
                    antennaPortsCount an1
                  },
                  ul-CyclicPrefixLength len1
                }
              },
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  antennaInfo defaultValue : NULL
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

byte llc_reconfig_ho_to_simo_msg[18] = {0x22, 0x0B, 0x00, 0x14, 0x80, 0x40, 0x00, 
  0x48, 0x0A, 0x0C, 0x40, 0x10, 0x08, 0xA0, 0x01, 0x00, 0x50, 0x00};

/*===========================================================================
                                HO to MIMO 
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
                  antennaInfoCommon 
                  {
                    antennaPortsCount an2
                  },
                  ul-CyclicPrefixLength len1
                }
              },
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  antennaInfo defaultValue : NULL
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

byte llc_reconfig_ho_to_mimo_msg[18] = {0x22, 0x0B, 0x00, 0x14, 0x80, 0x40, 0x00, 
  0x48, 0x0A, 0x0C, 0x40, 0x10, 0x08, 0xA1, 0x01, 0x00, 0x50, 0x00};

/*===========================================================================
                    Invalid Reconfig Message - Inter Freq HO 

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
                  dl-CarrierFreq 12000
                },
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

byte llc_invalid_ho_inter_freq_msg[17] = {0x22, 0x09, 0x20, 0x14, 0x2E, 0xE0, 
  0x40, 0x20, 0x00, 0x04, 0x05, 0x06, 0x20, 0x08, 0x04, 0x50, 0x00};

/*
Rohc_reset:value DL-DCCH-Message ::= 
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
                  dl-CarrierFreq 525
                },
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
                },
                drb-ContinueROHC-r11 true
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

byte llc_rohc_reset_ho_inter_freq_msg[20] = {0x22, 0x09, 0x60, 0x14, 0x02, 0x0D, 0x40, 0x20, 0x00, 
 0x04, 0x05, 0x06, 0x20, 0x08, 0x04, 0x50, 0x02, 0x80, 0xC0, 0x00};

/*
Rohc_reset:value DL-DCCH-Message ::= 
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
                  dl-CarrierFreq 525
                },
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
                },
                drb-ContinueROHC-r11 true
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

byte llc_wo_rohc_reset_ho_inter_freq_msg[17] = {0x22, 0x09, 0x20, 0x14, 0x02, 0x0D, 0x40, 0x20, 0x00, 
       0x04, 0x05, 0x06, 0x20, 0x08, 0x04, 0x50, 0x00};


/*===========================================================================
                    Reconfig Message - Invalid Antenna Info 

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
                newUE-Identity '00000000 00000000'B,
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
                  antennaInfoCommon 
                  {
                    antennaPortsCount spare1
                  },
                  ul-CyclicPrefixLength len1
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

byte llc_invalid_ho_antenna_info_msg[15] = {0x22, 0x09, 0x00, 0x14, 0x80, 0x00,
  0x00, 0x48, 0x0A, 0x0C, 0x40, 0x10, 0x08, 0xA3, 0x00};


/*===========================================================================
                    Reconfig Message - Invalid CRNTI 

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
                newUE-Identity '00000000 00000000'B,
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

byte llc_invalid_crnti_msg[15] = {0x22, 0x09, 0x00, 0x14, 0x80, 0x00, 0x00, 0x08, 
  0x0A, 0x0C, 0x40, 0x10, 0x08, 0xA0, 0x00};

/*===========================================================================
               Reconfig Message - HO with Invalid PDCP Modification 

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
                  dl-CarrierFreq 500,
                  ul-CarrierFreq 18500
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
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      rlc-UM 
                      {
                        pdcp-SN-Size len12bits
                      },
                      headerCompression notUsed : NULL
                    }
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

byte llc_invalid_ho_with_pdcp_modify_msg[43] = {0x22, 0x0B, 0x3C, 0x15, 0x01, 0xF4, 0x48, 0x44, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xEC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14,
0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x10, 0x01, 0x16, 0x04, 0x81, 0x02, 0x18, 0x80, 0x80,};

/*===========================================================================

               Reconfig Message - Invalid duplicate DRB Configuration 

============================================================================*/
/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  },
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      discardTimer ms50,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
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
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms10
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                }
              }
            }
      }
}
*/

byte llc_invalid_duplicate_drb_msg[23] = {0x22, 0x02, 0x10, 0x17, 0xCC, 0x4A, 
  0x14, 0x00, 0x00, 0x01, 0x05, 0xD0, 0x10, 0x7C, 0xC4, 0xA1, 0x40, 0x00, 0x00, 
  0x10, 0x5D, 0x01, 0x00};

/*
*===========================================================================

               Reconfig Message - Invalid SRS AP config

============================================================================*/
/*:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  soundingRS-UL-ConfigDedicatedAperiodic-r10 setup : 
                    {
                      srs-ConfigIndexAp-r10 31
                    }
                }
              }
            }
      }
}*/
byte llc_invalid_ap_srs_msg[10] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x18, 0x02, 0x9F, 0x00};

/*
srs:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  soundingRS-UL-ConfigDedicatedAperiodic-r10 setup : 
                    {
                      srs-ConfigIndexAp-r10 13
                    },
                  uplinkPowerControlDedicated-v1020 
                  {
                    pSRS-OffsetAp-r10 0
                  }
                }
              }
            }
      }
}

*/

byte llc_valid_srs_msg[11] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x20, 0x03, 0x8D, 0x40, 0x00};

/*
srs:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm1,
                      ue-TransmitAntennaSelection release : NULL
                    },
                  soundingRS-UL-ConfigDedicatedAperiodic-r10 setup : 
                    {
                      srs-ConfigIndexAp-r10 13
                    },
                  uplinkPowerControlDedicated-v1020 
                  {
                    pSRS-OffsetAp-r10 0
                  }
                }
              }
            }
      }
}

*/

byte llc_valid_apsrs_antenna_selection_rel_msg[12] = { 0x22, 0x02, 0x01, 0x80, 0x40, 0x02, 
   0x40, 0x80, 0x0E, 0x35, 0x00, 0x00 };

/*:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  soundingRS-UL-ConfigDedicatedAperiodic-r10 setup : 
                    {
                      srs-ConfigIndexAp-r10 0
                    }
                }
              }
            }
      }
}
*/
byte llc_invalid_ap_srs_0_msg[10] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x18, 0x02, 0x80, 0x00}; 

/*
Aperiodic_SRS_cfg:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  soundingRS-UL-ConfigDedicatedAperiodic-r10 setup : 
                    {
                      srs-ConfigIndexAp-r10 3
                    },
                  uplinkPowerControlDedicated-v1020 
                  {
                  }
                }
              }
            }
      }
}
*/
byte llc_invalid_srs_aperiodic_msg[10] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x18, 0x03, 0x83, 0x00}; 

/*
Aperiodic_SRS_cfg:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  soundingRS-UL-ConfigDedicatedAperiodic-r10 setup : 
                    {
                      srs-ConfigIndexAp-r10 3
                    }
                }
              }
            }
      }
}
*/
byte llc_valid_srs_aperiodic_msg[10] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x18, 0x02, 0x83, 0x00}; 



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

byte llc_cre_msg[17] = {0x08, 0x13, 0x98, 0x02, 0x66, 0x80, 0x04, 0x0D, 0x42, 
  0x00, 0x04, 0x01, 0xB6, 0x00, 0x01, 0x80, 0xD0};

byte llc_cre_msg_without_srs_dedicated[14] = {0x08, 0x13, 0x98, 0x02, 0x66, 0x80, 0x04, 0x0D, 0x42, 
  0x00, 0x04, 0x00, 0x81, 0xA0};
/*
cqi_mode:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm1,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-pmi-ConfigIndex 0,
                        cqi-FormatIndicatorPeriodic-r10 subbandCQI-r10 : 
                          {
                            k 2,
                            periodicityFactor-r10 n4
                          },
                        simultaneousAckNackAndCQI FALSE
                      }
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an1,
                        resourceConfig-r10 0,
                        subframeConfig-r10 0,
                        p-C-r10 0
                      }
                  }
                }
              }
            }
      }
}
*/
byte llc_valid_cqi_fgi_105[17] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x54, 0xC0, 0x02, 0x43, 0x00, 0x00, 0x00, 0x5A, 0x80, 0x00, 0x40};

/*
cqi_mode:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    pmi-RI-Report-r9 setup
                  }
                }
              }
            }
      }
}
*/

byte llc_reconfig_tm9_pmi_ri_enabled[11] = { 0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x24, 0x80, 0x22, 0x22, 0x00 };

/*
tm9_fgi_110:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-pmi-ConfigIndex 0,
                        cqi-FormatIndicatorPeriodic-r10 widebandCQI-r10 : 
                          {
                            csi-ReportMode-r10 submode2
                          },
                        simultaneousAckNackAndCQI FALSE
                      },
                    pmi-RI-Report-r9 setup
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an8,
                        resourceConfig-r10 0,
                        subframeConfig-r10 0,
                        p-C-r10 0
                      }
                  }
                }
              }
            }
      }
}

*/

byte llc_reconfig_tm9_fgi_110_config[17] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x54, 0xC0, 0x22, 
   0x63, 0x00, 0x00, 0x00, 0x35, 0xC0, 0x00, 0x80};

/*
tm9_fgi_110:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-pmi-ConfigIndex 0,
                        cqi-FormatIndicatorPeriodic-r10 widebandCQI-r10 : 
                          {
                            csi-ReportMode-r10 submode1
                          },
                        simultaneousAckNackAndCQI FALSE
                      },
                    pmi-RI-Report-r9 setup
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an8,
                        resourceConfig-r10 0,
                        subframeConfig-r10 0,
                        p-C-r10 0
                      }
                  }
                }
              }
            }
      }
}
*/

byte llc_reconfig_tm9_fgi_109_config[17] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x54, 0xC0, 0x22, 
   0x63, 0x00, 0x00, 0x00, 0x25, 0xC0, 0x00, 0x80};

/*
invalid_cqi_for_tm9:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-pmi-ConfigIndex 0,
                        cqi-FormatIndicatorPeriodic-r10 widebandCQI-r10 : 
                          {
                          },
                        simultaneousAckNackAndCQI FALSE
                      },
                    pmi-RI-Report-r9 setup
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an8,
                        resourceConfig-r10 0,
                        subframeConfig-r10 0,
                        p-C-r10 0
                      }
                  }
                }
              }
            }
      }
}
*/

byte llc_invalid_tm9_csi_mode[17] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x54, 0xC0, 0x22, 0x63, 0x00, 0x00, 0x00, 0x0B, 0x80, 0x01, 0x00};

/*
cqi_mode:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm20
                      },
                    nomPDSCH-RS-EPRE-Offset 0
                  }
                }
              }
            }
      }
}

*/
byte llc_reconfig_tm9_aperiodic_rm20[11] = { 0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x24, 0x80, 
   0x22, 0x88, 0x90};

/*
cqi_mode:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm22
                      },
                    nomPDSCH-RS-EPRE-Offset 0,
                    pmi-RI-Report-r9 setup
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an4,
                        resourceConfig-r10 0,
                        subframeConfig-r10 0,
                        p-C-r10 0
                      }
                  }
                }
              }
            }
      }
}

*/
byte llc_reconfig_tm9_aperiodic_rm22_pmiri_enabled_anport_4[14] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x3C, 0xC0, 
   0x22, 0xA9, 0x1B, 0x00, 0x01, 0x00 };

/*
cqi_mode:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm22
                      },
                    nomPDSCH-RS-EPRE-Offset 0,
                    pmi-RI-Report-r9 setup
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an8,
                        resourceConfig-r10 0,
                        subframeConfig-r10 0,
                        p-C-r10 0
                      }
                  }
                }
              }
            }
      }
}

*/
byte llc_reconfig_tm9_aperiodic_rm22_pmiri_enabled_anport_8[14] = {0x22, 0x02, 0x01, 0x80, 
	0x00, 0x90, 0x3C, 0xC0, 0x22, 0xA9, 0x1B, 0x80, 0x01, 0x00};

/*
cqi_mode:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    cqi-ReportAperiodic-r10 setup : 
                      {
                        cqi-ReportModeAperiodic-r10 rm30
                      },
                    nomPDSCH-RS-EPRE-Offset 0,
                    pmi-RI-Report-r9 setup
                  }
                }
              }
            }
      }
}

*/

byte llc_reconfig_tm9_pmiri_enable_aperiodic_rm30[11] = { 0x22, 0x02, 0x01, 0x80, 0x00, 
   0x90, 0x24, 0x80, 0x22, 0xA9, 0x90};

/*
cqi_mode:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    pmi-RI-Report-r9 setup
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an2,
                        resourceConfig-r10 0,
                        subframeConfig-r10 0,
                        p-C-r10 0
                      }
                  }
                }
              }
            }
      }
}

*/
byte llc_reconfig_tm9_pmi_ri_enabled_ports_8[14] = { 0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x3C, 
   0xC0, 0x22, 0x23, 0x70, 0x00, 0x20, 0x00 };

/*
cqi_mode:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    pmi-RI-Report-r9 setup
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an8,
                        resourceConfig-r10 0,
                        subframeConfig-r10 0,
                        p-C-r10 0
                      }
                  }
                }
              }
            }
      }
}

*/
byte llc_reconfig_tm9_pmi_ri_enabled_ports_2[14] = { 0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 0x3C, 
   0xC0, 0x22, 0x23, 0x50, 0x00, 0x20, 0x00 };

/*
cqi_mode:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                physicalConfigDedicated 
                {
                  antennaInfo-r10 explicitValue-r10 : 
                    {
                      transmissionMode-r10 tm9-v1020,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-r10 
                  {
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic-r10 setup : 
                      {
                        cqi-PUCCH-ResourceIndex-r10 0,
                        cqi-pmi-ConfigIndex 0,
                        cqi-FormatIndicatorPeriodic-r10 subbandCQI-r10 : 
                          {
                            k 2,
                            periodicityFactor-r10 n4
                          },
                        simultaneousAckNackAndCQI FALSE
                      }
                  },
                  csi-RS-Config-r10 
                  {
                    csi-RS-r10 setup : 
                      {
                        antennaPortsCount-r10 an1,
                        resourceConfig-r10 0,
                        subframeConfig-r10 0,
                        p-C-r10 0
                      }
                  }
                }
              }
            }
      }
}

*/

byte llc_reconfig_tm9_pmi_ri_disabled_ports_1[17] = {0x22, 0x02, 0x01, 0x80, 0x00, 0x90, 
   0x54, 0xC0, 0x22, 0x43, 0x00, 0x00, 0x00, 0x5A, 0x80, 0x00, 0x40};
/*===========================================================================

                   Rel 9 Reconfig message with PHY parameters only

============================================================================*/

/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL setup : 
                    {
                      semiPersistSchedIntervalDL sf32,
                      numberOfConfSPS-Processes 2,
                      n1-PUCCH-AN-PersistentList 
                      {
                        2,
                        3,
                        4
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB-6
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
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en0,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 5,
                    pSRS-Offset 2,
                    filterCoefficient fc4
                  },
                  tpc-PDCCH-ConfigPUCCH setup : 
                    {
                      tpc-RNTI '00000001 00000000'B,
                      tpc-Index indexOfFormat3 : 2
                    },
                  tpc-PDCCH-ConfigPUSCH setup : 
                    {
                      tpc-RNTI '00000001 00000000'B,
                      tpc-Index indexOfFormat3 : 2
                    },
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm12,
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic setup : 
                      {
                        cqi-PUCCH-ResourceIndex 3,
                        cqi-pmi-ConfigIndex 4,
                        cqi-FormatIndicatorPeriodic subbandCQI : 
                          {
                            k 2
                          },
                        ri-ConfigIndex 0,
                        simultaneousAckNackAndCQI TRUE
                      }
                  },
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw2,
                      srs-HoppingBandwidth hbw1,
                      freqDomainPosition 0,
                      duration TRUE,
                      srs-ConfigIndex 1,
                      transmissionComb 1,
                      cyclicShift cs3
                    },
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm8-v920,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-v920 
                  {
                    cqi-Mask-r9 setup,
                    pmi-RI-Report-r9 setup
                  },
                  antennaInfo-v920 
                  {
                    codebookSubsetRestriction-v920 n2TxAntenna-tm8-r9 : '000001'B
                  }
                },
                rlf-TimersAndConstants-r9 setup : 
                  {
                    t301-r9 ms200,
                    t310-r9 ms100,
                    n310-r9 n4,
                    t311-r9 ms5000,
                    n311-r9 n5
                  }
              }
            }
      }
}
*/

byte llc_reconfig_rel9_phy_msg[43] = {0x22, 0x02, 0x43, 0xC0, 0x20, 0x11, 0x18, 
  0x01, 0x00, 0x30, 0x09, 0xFF, 0x86, 0x00, 0x00, 0x00, 0x61, 0xD2, 0x24, 0x04, 
  0x00, 0x30, 0x10, 0x00, 0xE0, 0xE0, 0x0C, 0x04, 0xA0, 0x07, 0x20, 0x80, 0x36, 
  0x78, 0x04, 0x0B, 0xE0, 0x40, 0x04, 0x0F, 0x14, 0xD4, 0x00};

/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL setup : 
                    {
                      semiPersistSchedIntervalDL sf32,
                      numberOfConfSPS-Processes 2,
                      n1-PUCCH-AN-PersistentList 
                      {
                        2,
                        3,
                        4
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB-6
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
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en0,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 5,
                    pSRS-Offset 2,
                    filterCoefficient fc4
                  },
                  tpc-PDCCH-ConfigPUCCH setup : 
                    {
                      tpc-RNTI '00000001 00000000'B,
                      tpc-Index indexOfFormat3 : 2
                    },
                  tpc-PDCCH-ConfigPUSCH setup : 
                    {
                      tpc-RNTI '00000001 00000000'B,
                      tpc-Index indexOfFormat3 : 2
                    },
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm30,
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic setup : 
                      {
                        cqi-PUCCH-ResourceIndex 3,
                        cqi-pmi-ConfigIndex 4,
                        cqi-FormatIndicatorPeriodic subbandCQI : 
                          {
                            k 2
                          },
                        ri-ConfigIndex 0,
                        simultaneousAckNackAndCQI TRUE
                      }
                  },
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw2,
                      srs-HoppingBandwidth hbw1,
                      freqDomainPosition 0,
                      duration TRUE,
                      srs-ConfigIndex 1,
                      transmissionComb 1,
                      cyclicShift cs3
                    },
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm8-v920,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  cqi-ReportConfig-v920 
                  {
                  },
                  antennaInfo-v920 
                  {
                  }
                },
                rlf-TimersAndConstants-r9 release : NULL
              }
            }
      }
}
*/

byte llc_reconfig_rel9_cqi_pmi_explicit_rel_msg[40] = {0x22, 0x02, 0x43, 0xC0, 
  0x20, 0x11, 0x18, 0x01, 0x00, 0x30, 0x09, 0xFF, 0x86, 0x00, 0x00, 0x00, 0x61, 
  0xD2, 0x24, 0x04, 0x00, 0x30, 0x10, 0x00, 0xEC, 0xE0, 0x0C, 0x04, 0xA0, 0x07, 
  0x20, 0x80, 0x36, 0x78, 0x04, 0x07, 0x00, 0x04, 0x06, 0x00};

/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              radioResourceConfigDedicated 
              {
                sps-Config 
                {
                  semiPersistSchedC-RNTI '00000001 00000000'B,
                  sps-ConfigDL setup : 
                    {
                      semiPersistSchedIntervalDL sf32,
                      numberOfConfSPS-Processes 2,
                      n1-PUCCH-AN-PersistentList 
                      {
                        2,
                        3,
                        4
                      }
                    }
                },
                physicalConfigDedicated 
                {
                  pdsch-ConfigDedicated 
                  {
                    p-a dB-6
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
                    p0-UE-PUSCH 0,
                    deltaMCS-Enabled en0,
                    accumulationEnabled TRUE,
                    p0-UE-PUCCH 5,
                    pSRS-Offset 2,
                    filterCoefficient fc4
                  },
                  tpc-PDCCH-ConfigPUCCH setup : 
                    {
                      tpc-RNTI '00000001 00000000'B,
                      tpc-Index indexOfFormat3 : 2
                    },
                  tpc-PDCCH-ConfigPUSCH setup : 
                    {
                      tpc-RNTI '00000001 00000000'B,
                      tpc-Index indexOfFormat3 : 2
                    },
                  cqi-ReportConfig 
                  {
                    cqi-ReportModeAperiodic rm20,
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportPeriodic release : NULL
                  },
                  soundingRS-UL-ConfigDedicated setup : 
                    {
                      srs-Bandwidth bw2,
                      srs-HoppingBandwidth hbw1,
                      freqDomainPosition 0,
                      duration TRUE,
                      srs-ConfigIndex 1,
                      transmissionComb 1,
                      cyclicShift cs3
                    },
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm2,
                      ue-TransmitAntennaSelection setup : closedLoop
                    }
                },
                rlf-TimersAndConstants-r9 release : NULL
              }
            }
      }
}
*/

byte llc_reconfig_rel9_cqi_pmi_implicit_rel_msg[33] = {0x22, 0x02, 0x43, 0xC0, 
  0x20, 0x11, 0x18, 0x01, 0x00, 0x30, 0x08, 0xFF, 0x86, 0x00, 0x00, 0x00, 0x61, 
  0xD2, 0x24, 0x04, 0x00, 0x30, 0x10, 0x00, 0xE4, 0xB2, 0x08, 0x03, 0x61, 0x80,
  0x40, 0x60, 0x00};

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
                  dl-CarrierFreq 500,
                  ul-CarrierFreq 18500
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
                  },
                  {
                    srb-Identity 2,
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
                        },
                    logicalChannelConfig explicitValue : 
                      {
                        ul-SpecificParameters 
                        {
                          priority 4,
                          prioritisedBitRate infinity,
                          bucketSizeDuration ms500
                        }
                      }
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 1,
                    pdcp-Config 
                    {
                      discardTimer ms300,
                      rlc-AM 
                      {
                        statusReportRequired TRUE
                      },
                      headerCompression rohc : 
                        {
                          maxCID 2,
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
                    rlc-Config am : 
                      {
                        ul-AM-RLC 
                        {
                          t-PollRetransmit ms20,
                          pollPDU p32,
                          pollByte kB250,
                          maxRetxThreshold t6
                        },
                        dl-AM-RLC 
                        {
                          t-Reordering ms20,
                          t-StatusProhibit ms15
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
                  },
                  {
                    eps-BearerIdentity 5,
                    drb-Identity 2,
                    pdcp-Config 
                    {
                      discardTimer ms150,
                      rlc-UM 
                      {
                        pdcp-SN-Size len12bits
                      },
                      headerCompression notUsed : NULL
                    },
                    rlc-Config um-Uni-Directional-UL : 
                      {
                        ul-UM-RLC 
                        {
                          sn-FieldLength size10
                        }
                      },
                    logicalChannelIdentity 4,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 5,
                        prioritisedBitRate kBps8,
                        bucketSizeDuration ms100
                      }
                    }
                  },
                  {
                    eps-BearerIdentity 6,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      discardTimer ms100,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
                      },
                      headerCompression notUsed : NULL
                    },
                    rlc-Config um-Bi-Directional : 
                      {
                        ul-UM-RLC 
                        {
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms15
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 5,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms300
                      }
                    }
                  }
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
                      },
                    sr-ProhibitTimer-r9 4
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
                      }
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
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm8-v920,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 157,
                      dsr-TransMax n4
                    },
                  cqi-ReportConfig-v920 
                  {
                  },
                  antennaInfo-v920 
                  {
                    codebookSubsetRestriction-v920 n4TxAntenna-tm8-r9 : '00001111 00001111 00000000 00000111'B
                  }
                },
                rlf-TimersAndConstants-r9 setup : 
                  {
                    t301-r9 ms600,
                    t310-r9 ms500,
                    n310-r9 n4,
                    t311-r9 ms15000,
                    n311-r9 n4
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
                  fullConfig-r9 true
                }
              }
            }
      }
}
*/

byte llc_reconfig_rel9_ho_full_config_msg[123] = {
0x22, 0x0B, 0xBC, 0x15, 0x01, 0xF4, 0x48, 0x44, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xCC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14,
0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x00, 0x8F, 0x7A, 0x5C, 0x00, 0x00, 0x00, 0x01, 0x1B, 0xC2, 0x7C, 0xC0, 0xCF, 0x40, 0x01, 0x00,
0x00, 0xDA, 0xC2, 0x06, 0x11, 0x10, 0x7D, 0x42, 0xAA, 0x52, 0x90, 0x4B, 0xEC, 0x25, 0x21, 0x07, 0xD2, 0x13, 0x7E, 0x84, 0x2B, 0x00, 0x01, 0x01, 0xC0,
0xE0, 0x20, 0x10, 0x10, 0x02, 0x50, 0x10, 0x7B, 0xFF, 0xD6, 0x00, 0x00, 0x00, 0x36, 0x80, 0x80, 0x80, 0x02, 0x02, 0x00, 0x0C, 0x9C, 0x00, 0x00, 0x10,
0x00, 0x64, 0x00, 0x00, 0xCF, 0x40, 0x04, 0xE8, 0x01, 0x05, 0xCC, 0x3C, 0x3C, 0x00, 0x1C, 0x01, 0x03, 0xD2, 0x38, 0xC0, 0x20, 0x20, 0xA0,};

byte llc_reconfig_rel9_ho_full_config_msg_tdd[123] = {
0x22, 0x0B, 0xBC, 0x15, 0x9D, 0x80, 0x9D, 0x80, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xCC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14,
0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x00, 0x8F, 0x7A, 0x5C, 0x00, 0x00, 0x00, 0x01, 0x1B, 0xC2, 0x7C, 0xC0, 0xCF, 0x40, 0x01,
0x00, 
0x00, 0xDA, 0xC2, 0x06, 0x11, 0x10, 0x7D, 0x42, 0xAA, 0x52, 0x90, 0x4B, 0xEC, 0x25, 0x21, 0x07, 0xD2, 0x13, 0x7E, 0x84, 0x0B, 0x00, 0x03, 0x00,
0xE0, 0x70, 0x10, 0x08, 0x08, 0x01, 0x28, 0x08, 0x3D, 0xFF, 0xEB, 0x00, 0x00, 0x00, 0x1B, 0x40, 0x40, 0x40, 0x01, 0x01, 0x00, 0x06, 0x4E, 0x00, 0x00,
0x08, 0x00, 0x32, 0x00, 0x00, 0x67, 0xA0, 0x02, 0x74, 0x02, 0x80, 0xB9, 0x87, 0x87, 0x80, 0x03, 0x80, 0x60, 0x3D, 0x23, 0x8C, 0x02, 0x02, 0x0A};

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
                  dl-CarrierFreq 500,
                  ul-CarrierFreq 18500
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
                  },
                  {
                    srb-Identity 2,
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
                        },
                    logicalChannelConfig explicitValue : 
                      {
                        ul-SpecificParameters 
                        {
                          priority 4,
                          prioritisedBitRate infinity,
                          bucketSizeDuration ms500
                        }
                      }
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      discardTimer ms300,
                      rlc-AM 
                      {
                        statusReportRequired TRUE
                      },
                      headerCompression rohc : 
                        {
                          maxCID 2,
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
                    rlc-Config am : 
                      {
                        ul-AM-RLC 
                        {
                          t-PollRetransmit ms20,
                          pollPDU p32,
                          pollByte kB250,
                          maxRetxThreshold t6
                        },
                        dl-AM-RLC 
                        {
                          t-Reordering ms20,
                          t-StatusProhibit ms15
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps32,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                },
                mac-MainConfig defaultValue : NULL
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
                  fullConfig-r9 true
                }
              }
            }
      }
}
*/

byte llc_rel9_ho_full_config_mode_change_msg[59] = {0x22, 0x0B, 0xBC, 0x15, 0x01, 0xF4, 0x48, 0x44, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xCC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, 0x22, 0x01, 0x14,
0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x00, 0x8B, 0x4A, 0x5C, 0x00, 0x00, 0x00, 0x01, 0x1B, 0xC0, 0x7C, 0xC4, 0xCF, 0x40, 0x01, 0x00,
0x00, 0xDA, 0xC2, 0x07, 0xD0, 0x18, 0x90, 0x10, 0x50,};

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
                  dl-CarrierFreq 500,
                  ul-CarrierFreq 18500
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
                  },
                  {
                    srb-Identity 2,
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
                        },
                    logicalChannelConfig explicitValue : 
                      {
                        ul-SpecificParameters 
                        {
                          priority 4,
                          prioritisedBitRate infinity,
                          bucketSizeDuration ms500
                        }
                      }
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      discardTimer ms300,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
                      },
                      headerCompression rohc : 
                        {
                          maxCID 2,
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
                    rlc-Config um-Uni-Directional-UL : 
                      {
                        ul-UM-RLC 
                        {
                          sn-FieldLength size5
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps32,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                },
                mac-MainConfig defaultValue : NULL
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
                  fullConfig-r9 true
                }
              }
            }
      }
}
*/

byte llc_ho_full_config_mode_change_ul_only_msg[56] = {0x22, 0x0B, 0xBC, 0x15, 0x01, 0xF4, 
  0x48, 0x44, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xCC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 
  0x0F, 0x22, 0x01, 0x14, 0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x00, 0x8B, 
  0x4A, 0x5C, 0x00, 0x00, 0x00, 0x01, 0x1B, 0xC0, 0x7C, 0xC4, 0xAD, 0x40, 0x01, 0x00, 0x27, 
  0x40, 0x62, 0x40, 0x41, 0x40};

/*
ultoam:value DL-DCCH-Message ::= 
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
                  dl-CarrierFreq 500,
                  ul-CarrierFreq 18500
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
                  },
                  {
                    srb-Identity 2,
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
                        },
                    logicalChannelConfig explicitValue : 
                      {
                        ul-SpecificParameters 
                        {
                          priority 4,
                          prioritisedBitRate infinity,
                          bucketSizeDuration ms500
                        }
                      }
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      discardTimer ms300,
                      rlc-AM 
                      {
                        statusReportRequired FALSE
                      },
                      headerCompression rohc : 
                        {
                          maxCID 2,
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
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps32,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                },
                mac-MainConfig defaultValue : NULL
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
                  fullConfig-r9 true
                }
              }
            }
      }
}

*/


byte llc_ho_full_config_mode_change_am [59] = {0x22, 0x0B, 0xBC, 0x15, 0x01, 0xF4, 
  0x48, 0x44, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xCC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 
  0x0F, 0x22, 0x01, 0x14, 0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x00, 0x8B, 
  0x4A, 0x5C, 0x00, 0x00, 0x00, 0x01, 0x1B, 0xC0, 0x7C, 0xC4, 0xCD, 0x40, 0x01, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xD0, 0x18, 0x90, 0x10, 0x50};

/*
llc_ho_full_config_mode_change_dl_only_msg:value DL-DCCH-Message ::= 
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
                  dl-CarrierFreq 500,
                  ul-CarrierFreq 18500
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
                  },
                  {
                    srb-Identity 2,
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
                        },
                    logicalChannelConfig explicitValue : 
                      {
                        ul-SpecificParameters 
                        {
                          priority 4,
                          prioritisedBitRate infinity,
                          bucketSizeDuration ms500
                        }
                      }
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      discardTimer ms300,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
                      },
                      headerCompression rohc : 
                        {
                          maxCID 2,
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
                    rlc-Config um-Uni-Directional-DL : 
                      {
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms0
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 1,
                        prioritisedBitRate kBps32,
                        bucketSizeDuration ms50
                      }
                    }
                  }
                },
                mac-MainConfig defaultValue : NULL
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
                  fullConfig-r9 true
                }
              }
            }
      }
}


*/

byte llc_ho_full_config_mode_change_dl_only_msg[56] = {0x22, 0x0B, 0xBC, 0x15, 0x01, 0xF4, 
  0x48, 0x44, 0x91, 0x25, 0x00, 0x80, 0x3F, 0xCC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 
  0x0F, 0x22, 0x01, 0x14, 0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x00, 0x8B, 
  0x4A, 0x5C, 0x00, 0x00, 0x00, 0x01, 0x1B, 0xC0, 0x7C, 0xC4, 0xAD, 0x40, 0x01, 0x00, 0x30, 
  0x3A, 0x03, 0x12, 0x02, 0x0A};

/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
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
                    srb-Identity 2
                  }
                },
                drb-ToAddModList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 1,
                    pdcp-Config 
                    {
                      discardTimer ms300,
                      rlc-AM 
                      {
                        statusReportRequired TRUE
                      },
                      headerCompression rohc : 
                        {
                          maxCID 2,
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
                    rlc-Config am : 
                      {
                        ul-AM-RLC 
                        {
                          t-PollRetransmit ms20,
                          pollPDU p32,
                          pollByte kB250,
                          maxRetxThreshold t6
                        },
                        dl-AM-RLC 
                        {
                          t-Reordering ms20,
                          t-StatusProhibit ms15
                        }
                      },
                    logicalChannelIdentity 6,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 3,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms50
                      }
                    }
                  },
                  {
                    eps-BearerIdentity 5,
                    drb-Identity 2,
                    pdcp-Config 
                    {
                      discardTimer ms150,
                      rlc-UM 
                      {
                        pdcp-SN-Size len12bits
                      },
                      headerCompression notUsed : NULL
                    },
                    rlc-Config um-Uni-Directional-DL : 
                      {
                        dl-UM-RLC 
                        {
                          sn-FieldLength size10,
                          t-Reordering ms20
                        }
                      },
                    logicalChannelIdentity 8,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 5,
                        prioritisedBitRate kBps8,
                        bucketSizeDuration ms100
                      }
                    }
                  },
                  {
                    eps-BearerIdentity 6,
                    drb-Identity 3,
                    pdcp-Config 
                    {
                      discardTimer ms100,
                      rlc-UM 
                      {
                        pdcp-SN-Size len7bits
                      },
                      headerCompression notUsed : NULL
                    },
                    rlc-Config um-Bi-Directional : 
                      {
                        ul-UM-RLC 
                        {
                          sn-FieldLength size5
                        },
                        dl-UM-RLC 
                        {
                          sn-FieldLength size5,
                          t-Reordering ms15
                        }
                      },
                    logicalChannelIdentity 10,
                    logicalChannelConfig 
                    {
                      ul-SpecificParameters 
                      {
                        priority 5,
                        prioritisedBitRate kBps16,
                        bucketSizeDuration ms300
                      }
                    }
                  }
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
                      },
                    sr-ProhibitTimer-r9 4
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
                      }
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
                  antennaInfo explicitValue : 
                    {
                      transmissionMode tm8-v920,
                      ue-TransmitAntennaSelection setup : closedLoop
                    },
                  schedulingRequestConfig setup : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigIndex 157,
                      dsr-TransMax n4
                    },
                  cqi-ReportConfig-v920 
                  {
                  },
                  antennaInfo-v920 
                  {
                    codebookSubsetRestriction-v920 n4TxAntenna-tm8-r9 : '00001111 00001111 00000000 00000111'B
                  }
                },
                rlf-TimersAndConstants-r9 setup : 
                  {
                    t301-r9 ms600,
                    t310-r9 ms500,
                    n310-r9 n4,
                    t311-r9 ms15000,
                    n311-r9 n4
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
                  fullConfig-r9 true
                }
              }
            }
      }
}
*/

byte llc_reconfig_after_cre_rel9_full_config_msg[81] = {0x22, 0x03, 0xF7, 0xB6, 
  0x24, 0xF9, 0x81, 0x9E, 0x80, 0x02, 0x00, 0x01, 0xB5, 0x84, 0x0D, 0xA2, 0x20, 
  0xFA, 0x85, 0x54, 0xE4, 0xA9, 0x04, 0xBE, 0xC2, 0x52, 0x10, 0x7D, 0x21, 0x37, 
  0xE8, 0x42, 0xB0, 0x00, 0x10, 0x1C, 0x0E, 0x02, 0x01, 0x01, 0x00, 0x25, 0x01, 
  0x07, 0xBF, 0xED, 0x60, 0x00, 0x00, 0x03, 0x68, 0x08, 0x08, 0x00, 0x20, 0x20, 
  0x00, 0xC9, 0xC0, 0x00, 0x01, 0x00, 0x01, 0xE8, 0x00, 0x9D, 0x00, 0x20, 0xB9, 
  0x87, 0x87, 0x80, 0x03, 0x80, 0x20, 0x7A, 0x47, 0x18, 0x04, 0x04, 0x14};

/*===========================================================================
                      eMBMS - SIB 13 message
===========================================================================*/
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

byte llc_sib13_msg[13] = {0x00, 0x40, 0x84, 0x84, 0xC9, 0x32, 0x7F, 0x4C, 0xAF, 
  0xBB, 0xCE, 0xB0, 0x00};

/*===========================================================================
                      eMBMS - MCCH message
===========================================================================*/
/*
MCCH_MSG:value MCCH-Message ::= 
{
  message c1 : mbsfnAreaConfiguration-r9 : 
      {
        commonSF-Alloc-r9 
        {
          {
            radioframeAllocationPeriod n4,
            radioframeAllocationOffset 5,
            subframeAllocation fourFrames : '11111111 11110000 11111111'B
          },
          {
            radioframeAllocationPeriod n16,
            radioframeAllocationOffset 7,
            subframeAllocation oneFrame : '000011'B
          }
        },
        commonSF-AllocPeriod-r9 rf32,
        pmch-InfoList-r9 
        {
          {
            pmch-Config-r9 
            {
              sf-AllocEnd-r9 500,
              dataMCS-r9 28,
              mch-SchedulingPeriod-r9 rf64
            },
            mbms-SessionInfoList-r9 
            {
              {
                tmgi-r9 
                {
                  plmn-Id-r9 explicitValue-r9 : 
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
                  serviceId-r9 'FFF00F'H
                },
                logicalChannelIdentity-r9 28
              },
              {
                tmgi-r9 
                {
                  plmn-Id-r9 plmn-Index-r9 : 1,
                  serviceId-r9 '0F0F0F'H
                },
                sessionId-r9 'F0'H,
                logicalChannelIdentity-r9 0
              }
            }
          },
          {
            pmch-Config-r9 
            {
              sf-AllocEnd-r9 1000,
              dataMCS-r9 25,
              mch-SchedulingPeriod-r9 rf128
            },
            mbms-SessionInfoList-r9 
            {
              {
                tmgi-r9 
                {
                  plmn-Id-r9 plmn-Index-r9 : 6,
                  serviceId-r9 'F0F0F0'H
                },
                logicalChannelIdentity-r9 0
              },
              {
                tmgi-r9 
                {
                  plmn-Id-r9 plmn-Index-r9 : 5,
                  serviceId-r9 'FFFFFF'H
                },
                logicalChannelIdentity-r9 10
              }
            }
          },
          {
            pmch-Config-r9 
            {
              sf-AllocEnd-r9 1500,
              dataMCS-r9 15,
              mch-SchedulingPeriod-r9 rf16
            },
            mbms-SessionInfoList-r9 
            {
              {
                tmgi-r9 
                {
                  plmn-Id-r9 plmn-Index-r9 : 6,
                  serviceId-r9 '010101'H
                },
                logicalChannelIdentity-r9 0
              }
            }
          }
        }
      }
}

*/


byte llc_mcch_msg[43] = {0x0A, 0xBF, 0xFF, 0x0F, 0xF9, 0xC1, 0xB3, 0x0F, 0xA7, 
0x18, 0x8C, 0x48, 0xE8, 0xAD, 0xFF, 0xE0, 0x1F, 0xC4, 0x03, 0xC3, 0xC3, 0xFC, 
0x00, 0x3E, 0x8C, 0xC1, 0x0B, 0xE1, 0xE1, 0xE0, 0x01, 0x3F, 0xFF, 0xFF, 0xD4, 
0x5D, 0xC7, 0x90, 0x8A, 0x02, 0x02, 0x02, 0x00};


/*
MCCH:value MCCH-Message ::= 
{
  message c1 : mbsfnAreaConfiguration-r9 : 
      {
        commonSF-Alloc-r9 
        {
          {
            radioframeAllocationPeriod n4,
            radioframeAllocationOffset 5,
            subframeAllocation fourFrames : '11111111 11110000 11111111'B
          },
          {
            radioframeAllocationPeriod n16,
            radioframeAllocationOffset 7,
            subframeAllocation oneFrame : '000011'B
          }
        },
        commonSF-AllocPeriod-r9 rf32,
        pmch-InfoList-r9 
        {
          {
            pmch-Config-r9 
            {
              sf-AllocEnd-r9 500,
              dataMCS-r9 28,
              mch-SchedulingPeriod-r9 rf1024
            },
            mbms-SessionInfoList-r9 
            {
              {
                tmgi-r9 
                {
                  plmn-Id-r9 explicitValue-r9 : 
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
                  serviceId-r9 'FFF00F'H
                },
                logicalChannelIdentity-r9 28
              },
              {
                tmgi-r9 
                {
                  plmn-Id-r9 plmn-Index-r9 : 1,
                  serviceId-r9 '0F0F0F'H
                },
                sessionId-r9 'F0'H,
                logicalChannelIdentity-r9 0
              }
            }
          },
          {
            pmch-Config-r9 
            {
              sf-AllocEnd-r9 1000,
              dataMCS-r9 25,
              mch-SchedulingPeriod-r9 rf128
            },
            mbms-SessionInfoList-r9 
            {
              {
                tmgi-r9 
                {
                  plmn-Id-r9 plmn-Index-r9 : 6,
                  serviceId-r9 'F0F0F0'H
                },
                logicalChannelIdentity-r9 0
              },
              {
                tmgi-r9 
                {
                  plmn-Id-r9 plmn-Index-r9 : 5,
                  serviceId-r9 'FFFFFF'H
                },
                logicalChannelIdentity-r9 10
              }
            }
          },
          {
            pmch-Config-r9 
            {
              sf-AllocEnd-r9 1500,
              dataMCS-r9 15,
              mch-SchedulingPeriod-r9 rf16
            },
            mbms-SessionInfoList-r9 
            {
              {
                tmgi-r9 
                {
                  plmn-Id-r9 plmn-Index-r9 : 6,
                  serviceId-r9 '010101'H
                },
                logicalChannelIdentity-r9 0
              }
            }
          }
        }
      }
}
*/

/*invalid message for simulating MCCH repetition period lesser than 1st PMCH scheduling period*/


byte llc_mcch_invalid_msg[43] = {0x0A, 0xBF, 0xFF, 0x0F, 0xF9, 0xC1, 0xB3, 0x0F, 0xA7, 
0x38, 0x8C, 0x48, 0xE8, 0xAD, 0xFF, 0xE0, 0x1F, 0xC4, 0x03, 0xC3, 0xC3, 0xFC, 
0x00, 0x3E, 0x8C, 0xC1, 0x0B, 0xE1, 0xE1, 0xE0, 0x01, 0x3F, 0xFF, 0xFF, 0xD4, 
0x5D, 0xC7, 0x90, 0x8A, 0x02, 0x02, 0x02, 0x00};

