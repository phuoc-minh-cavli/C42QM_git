/*!
  @file lte_rrc_meas_test_msg.h

  @brief
  Holds message buffers for encoded Meas messages needed for unit tests.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/meas/lte_rrc_meas_test_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------------------
08/24/11   pk      Added message definitions and function prototypes for IRAT GSM, WCDMA
02/26/10   mm      Added new SIB6, SIB7 and SIB8 messages for testing their handling in meas
08/12/09   sk      March 09 spec migration for SIB4,SIB4,SIB5 and conn meas
02/23/09   sk      Added support for inter-frequency measurements config
09/22/08   ask     Initial Revision

===========================================================================*/

#ifndef LTE_RRC_MEAS_TEST_MSG_H
#define LTE_RRC_MEAS_TEST_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <lte.h>
#include <geran_grr_msg.h>
#include <lte_rrc_irat_to_tds_mgr.h> 


/*===========================================================================

                           INTERNAL DEFINITIONS

===========================================================================*/
#define LTE_RRC_MEAS_TEST_MAX_BANDCLASS_VALUE 17
#define LTE_RRC_MEAS_TEST_MAX_PHY_CELL_IN_CHANNEL 16
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*
TypicalSIB3:value BCCH-DL-SCH-Message ::= 
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
                    q-Hyst dB1,
                    speedDependentReselection 
                    {
                      mobilityStateParameters 
                      {
                        t-Evalulation s60,
                        t-HystNormal s60,
                        n-CellChangeMedium 5,
                        n-CellChangeHigh 5
                      },
                      speedDependentScalingParametersHyst 
                      {
                        q-HystSF-Medium dB-4,
                        q-HystSF-High dB-4
                      }
                    },
                    sameRefSignalsInNeighbour FALSE
                  },
                  cellReselectionServingFreqInfo 
                  {
                    s-NonIntraSearch 5,
                    threshServingLow 6,
                    cellReselectionPriority 7
                  },
                  intraFreqCellReselectionInfo 
                  {
                    q-RxLevMin -70,
                    p-Max -30,
                    s-IntraSearch 8,
                    allowedMeasBandwidth mbw15,
                    neighbourCellConfiguration '00'B,
                    t-ReselectionEUTRAN 2,
                    speedDependentScalingParameters 
                    {
                      t-ReselectionEUTRAN-SF-Medium oDot5,
                      t-ReselectionEUTRAN-SF-High oDot5
                    }
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_LTE_RRC_MEAS_TEST_SIB3_MSG1 \
static const uint8 lte_rrc_meas_test_sib3_msg1[] = \
{ \
  0x00, 0x05, 0x12, 0x51,0x0A, 0x53, 0x7F, 0xBF, 0x84, 0x14, 0x94, \
}

/*
TypicalSIB4:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib4 : 
                {
                  intraFreqNeighbouringCellList 
                  {
                    {
                      physicalCellIdentity 1,
                      q-OffsetCell dB-22
                    },
                    {
                      physicalCellIdentity 2,
                      q-OffsetCell dB-20
                    }
                  },
                  intraFreqBlacklistedCellList 
                  {
                    {
                      physicalCellIdentityAndRange singlePCI : 10
                    },
                    {
                      physicalCellIdentityAndRange singlePCI : 11
                    }
                  },
                  csg-PCI-Range singlePCI : 4
                }
            }
          }
      }
}
*/
#define DECLARE_LTE_RRC_MEAS_TEST_SIB4_MSG1 \
static const uint8 lte_rrc_meas_test_sib4_msg1[] = \
{ \
  0x00, 0x09, 0xC4, 0x01, 0x08, 0x04, 0x21, 0x02, 0x80, 0xB0, 0x10, \
}

/*
sib5:value BCCH-DL-SCH-Message ::= 
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
                      dl-CarrierFreq 5180,
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

#define DECLARE_LTE_RRC_MEAS_TEST_SIB5_MSG1 \
static const uint8 lte_rrc_meas_test_sib5_msg1[] = \
{ \
  0x00, 0x0C, 0x5F, 0x8A, 0x1E, 0x5E, 0x07, 0xA5, 0x28, 0xF6, 0x92, 0x01, 0x08, \
  0x06, 0x30, 0x31, 0x48, 0x01, 0x01, 0x40, 0x85, 0x80, 0x20, 0x17, 0x81, 0xD4, \
  0xA3, 0xC2, 0x40,  \
}

/*
sib5:value BCCH-DL-SCH-Message ::= 
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
                      dl-CarrierFreq 5180,
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
                          physCellIdRange 
                          {
                            start 1
                          }
                        },
                        {
                          physCellIdRange 
                          {
                            start 5
                          }
                        },
                        {
                          physCellIdRange 
                          {
                            start 8
                          }
                        }
                      }
                    },
                    {
                      dl-CarrierFreq 1000,
                      q-RxLevMin -50,
                      t-ReselectionEUTRA 3,
                      threshX-High 3,
                      threshX-Low 5,
                      allowedMeasBandwidth mbw6,
                      presenceAntennaPort1 FALSE,
                      cellReselectionPriority 0,
                      neighCellConfig '00'B
                    }
                  }
                }
            }
          }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_SIB5_MSG2 \
static const uint8 lte_rrc_meas_test_sib5_msg2[] = \
{ \
  0x00, 0x0C, 0x5F, 0x8A, 0x1E, 0x5E, 0x07, 0xA5, 0x28, 0xF6, 0x92, \
  0x01, 0x08, 0x06, 0x30, 0x31, 0x48, 0x01, 0x01, 0x40, 0x81, 0x00, \
  0x7D, 0x0A, 0x31, 0x94, 0x00,  \
}

#define DECLARE_LTE_RRC_MEAS_TEST_SIB5_MSG3 \
static const uint8 lte_rrc_meas_test_sib5_msg3[] = \
{ \
  0x00, 0x0C, 0x1F, 0x8A, 0x1E, 0x5E, 0x07, 0xA5, 0x28, 0xF6, 0xF2, 0x01, 0x3C, \
  0x06, 0x30, 0x31, 0x48, 0x01, 0x01, 0x40, 0x80, \
}

/*
SIB5:value BCCH-DL-SCH-Message ::= 
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
                      dl-CarrierFreq 5180,
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
                          physCellId 2,
                          q-OffsetCell dB-14
                        },
                        {
                          physCellId 4,
                          q-OffsetCell dB-24
                        },
                        {
                          physCellId 4,
                          q-OffsetCell dB-24
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
                    }
                  }
                }
            }
          }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_SIB5_WITH_DUP_NEIGH_CELLS \
static const uint8 lte_rrc_meas_test_sib5_with_dup_neigh_cells[] = \
{ \
  0x00, 0x0C, 0x1F, 0x8A, 0x1E, 0x5E, 0x07, 0xA5, 0x28, 0xF6, 0x94, 0x01, 0x08, \
  0x06, 0x30, 0x11, 0x40, 0x80, 0x02, 0x00, 0x80, 0x10, 0x14, 0x08, \
}

/*
SIB5:value BCCH-DL-SCH-Message ::= 
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
                      dl-CarrierFreq 5180,
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
                      dl-CarrierFreq 5200,
                      q-RxLevMin -25,
                      t-ReselectionEUTRA 5,
                      threshX-High 10,
                      threshX-Low 5,
                      allowedMeasBandwidth mbw6,
                      presenceAntennaPort1 FALSE,
                      cellReselectionPriority 4,
                      neighCellConfig '00'B
                    },
                    {
                      dl-CarrierFreq 5180,
                      q-RxLevMin -27,
                      t-ReselectionEUTRA 5,
                      threshX-High 15,
                      threshX-Low 5,
                      allowedMeasBandwidth mbw6,
                      presenceAntennaPort1 FALSE,
                      cellReselectionPriority 4,
                      neighCellConfig '00'B
                    },
                    {
                      dl-CarrierFreq 5220,
                      q-RxLevMin -26,
                      t-ReselectionEUTRA 6,
                      threshX-High 15,
                      threshX-Low 7,
                      allowedMeasBandwidth mbw6,
                      presenceAntennaPort1 FALSE,
                      cellReselectionPriority 7,
                      neighCellConfig '00'B
                    },
                    {
                      dl-CarrierFreq 5220,
                      q-RxLevMin -28,
                      t-ReselectionEUTRA 3,
                      threshX-High 7,
                      threshX-Low 4,
                      allowedMeasBandwidth mbw6,
                      presenceAntennaPort1 FALSE,
                      cellReselectionPriority 6,
                      neighCellConfig '00'B
                    }
                  }
                }
            }
          }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_SIB5_WITH_DUP_FREQ \
static const uint8 lte_rrc_meas_test_sib5_with_dup_freq[] = \
{ \
  0x00, 0x0D, 0x1F, 0x8A, 0x1E, 0x5E, 0x07, 0xA5, 0x28, 0xF6, 0x90, 0x01, 0x08, \
  0x80, 0x10, 0x14, 0x08, 0x10, 0x28, 0xA1, 0x6D, 0x51, 0x42, 0x02, 0x05, 0x0F, \
  0x2B, 0xAF, 0x28, 0x40, 0x40, 0xA3, 0x25, 0x99, 0xE7, 0x0E, 0x08, 0x14, 0x64, \
  0xA9, 0x9C, 0x81, 0x80, \
}

/*
SIB5:value BCCH-DL-SCH-Message ::= 
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
                      dl-CarrierFreq 5180,
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
                      q-OffsetFreq dB0,
                      interFreqNeighCellList 
                      {
                        {
                          physCellId 2,
                          q-OffsetCell dB0
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
                      dl-CarrierFreq 36000,
                      q-RxLevMin -25,
                      t-ReselectionEUTRA 0,
                      threshX-High 20,
                      threshX-Low 10,
                      allowedMeasBandwidth mbw6,
                      presenceAntennaPort1 FALSE,
                      cellReselectionPriority 4,
                      neighCellConfig '00'B,
                      q-OffsetFreq dB-20
                    }
                  }
                }
            }
          }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_SIB5_FDD_AND_TDD_MSG \
static const uint8 lte_rrc_meas_test_sib5_fdd_and_tdd_msg[] = \
{ \
  0x00, 0x0C, 0x5F, 0x8A, 0x1E, 0x5E, 0x07, 0xA5, 0x28, 0xF6, 0xF2, 0x01, 0x3C, \
  0x06, 0x30, 0x31, 0x48, 0x01, 0x01, 0x40, 0x81, 0x91, 0x94, 0x16, 0x8A, 0x28, \
  0x20, 0x20, \
}

/*
SIB6:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib6 : 
                {
                  carrierFreqListUTRA-FDD 
                  {
                    {
                      carrierFreq 100,
                      cellReselectionPriority 5,
                      threshX-High 25,
                      threshX-Low 15,
                      q-RxLevMin -30,
                      p-MaxUTRA 5,
                      q-QualMin -10
                    },
                    {
                      carrierFreq 200,
                      cellReselectionPriority 3,
                      threshX-High 25,
                      threshX-Low 10,
                      q-RxLevMin -30,
                      p-MaxUTRA 1,
                      q-QualMin -5
                    },
                    {
                      carrierFreq 300,
                      cellReselectionPriority 3,
                      threshX-High 25,
                      threshX-Low 12,
                      q-RxLevMin -40,
                      p-MaxUTRA 5,
                      q-QualMin -10
                    }
                  },
                  t-ReselectionUTRA 3,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot5,
                    sf-High lDot0
                  }
                }
            }
          }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_SIB6_MSG1 \
static const uint8 lte_rrc_meas_test_sib6_msg1[] = \
{ \
  0x00, 0x11, 0x49, 0x01, 0x92, 0xE5, 0xEF, 0x37, 0x72, 0x06, 0x43, 0xCA, \
  0x9E, 0x67, 0x34, 0x12, 0xC7, 0x96, 0x28, 0xDD, 0xCD, 0xC0  \
}
/* sib6_dup:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib6 : 
                {
                  carrierFreqListUTRA-FDD 
                  {
                    {
                      carrierFreq 300,
                      cellReselectionPriority 5,
                      threshX-High 25,
                      threshX-Low 15,
                      q-RxLevMin -30,
                      p-MaxUTRA 5,
                      q-QualMin -10
                    },
                    {
                      carrierFreq 200,
                      cellReselectionPriority 3,
                      threshX-High 25,
                      threshX-Low 10,
                      q-RxLevMin -30,
                      p-MaxUTRA 1,
                      q-QualMin -5
                    },
                    {
                      carrierFreq 300,
                      cellReselectionPriority 3,
                      threshX-High 25,
                      threshX-Low 12,
                      q-RxLevMin -40,
                      p-MaxUTRA 5,
                      q-QualMin -10
                    }
                  },
                  t-ReselectionUTRA 3,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot5,
                    sf-High lDot0
                  }
                }
            }
          }
      }
}

*/
#define DECLARE_LTE_RRC_MEAS_TEST_SIB6_MSG2 \
static const uint8 lte_rrc_meas_test_sib6_msg2[] = \
{ \
  0x00, 0x11, 0x49, 0x04, 0xB2, 0xE5, 0xEF, 0x37, 0x72, 0x06, 0x43, 0xCA, \
  0x9E, 0x67, 0x34, 0x12, 0xC7, 0x96, 0x28, 0xDD, 0xCD, 0xC0  \
}
/*
sib6_tdd:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib6 : 
                {
                  carrierFreqListUTRA-TDD 
                  {
                    {
                      carrierFreq 11600,
                      cellReselectionPriority 5,
                      threshX-High 25,
                      threshX-Low 15,
                      q-RxLevMin -30,
                      p-MaxUTRA 5
                    },
                    {
                      carrierFreq 11650,
                      cellReselectionPriority 3,
                      threshX-High 25,
                      threshX-Low 10,
                      q-RxLevMin -30,
                      p-MaxUTRA 1
                    },
                    {
                      carrierFreq 11720,
                      cellReselectionPriority 3,
                      threshX-High 25,
                      threshX-Low 12,
                      q-RxLevMin -40,
                      p-MaxUTRA 5
                    }
                  },
                  t-ReselectionUTRA 3,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot5,
                    sf-High lDot0
                  }
                }
            }
          }
      }
}*/
#define DECLARE_LTE_RRC_MEAS_TEST_SIB6_TDD_MSG1 \
static const uint8 lte_rrc_meas_test_sib6_tdd_msg1[] = \
{\
  0x00, 0x10, 0xC9, 0xB5, 0x42, 0xE5, 0xEF, 0x37, 0x6D, 0x82, 0x79, 0x53, \
  0xCC, 0xDB, 0x72, 0x1E, 0x58, 0xA3, 0x76, 0xE0 \
}
/*
sib_tdd_dup:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib6 : 
                {
                  carrierFreqListUTRA-TDD 
                  {
                    {
                      carrierFreq 11650,
                      cellReselectionPriority 5,
                      threshX-High 25,
                      threshX-Low 15,
                      q-RxLevMin -30,
                      p-MaxUTRA 5
                    },
                    {
                      carrierFreq 11750,
                      cellReselectionPriority 3,
                      threshX-High 25,
                      threshX-Low 10,
                      q-RxLevMin -30,
                      p-MaxUTRA 1
                    },
                    {
                      carrierFreq 11650,
                      cellReselectionPriority 3,
                      threshX-High 25,
                      threshX-Low 12,
                      q-RxLevMin -40,
                      p-MaxUTRA 5
                    }
                  },
                  t-ReselectionUTRA 3,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot5,
                    sf-High lDot0
                  }
                }
            }
          }
      }
}*/
#define DECLARE_LTE_RRC_MEAS_TEST_SIB6_TDD_MSG2 \
static const uint8 lte_rrc_meas_test_sib6_tdd_msg2[] = \
{\
  0x00, 0x10, 0xC9, 0xB6, 0x0A, 0xE5, 0xEF, 0x37, 0x6D, 0xE6, 0x79, 0x53, \
  0xCC, 0xDB, 0x60, 0x9E, 0x58, 0xA3, 0x76, 0xE0, \
}
/*
sib6_tdd_fdd_msg:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib6 : 
                {
                  carrierFreqListUTRA-FDD 
                  {
                    {
                      carrierFreq 100,
                      cellReselectionPriority 5,
                      threshX-High 25,
                      threshX-Low 15,
                      q-RxLevMin -30,
                      p-MaxUTRA 5,
                      q-QualMin -10
                    },
                    {
                      carrierFreq 200,
                      cellReselectionPriority 3,
                      threshX-High 25,
                      threshX-Low 10,
                      q-RxLevMin -30,
                      p-MaxUTRA 1,
                      q-QualMin -5
                    },
                    {
                      carrierFreq 300,
                      cellReselectionPriority 3,
                      threshX-High 25,
                      threshX-Low 12,
                      q-RxLevMin -40,
                      p-MaxUTRA 5,
                      q-QualMin -10
                    }
                  },
                  carrierFreqListUTRA-TDD 
                  {
                    {
                      carrierFreq 11600,
                      cellReselectionPriority 5,
                      threshX-High 25,
                      threshX-Low 15,
                      q-RxLevMin -30,
                      p-MaxUTRA 5
                    },
                    {
                      carrierFreq 11650,
                      cellReselectionPriority 3,
                      threshX-High 25,
                      threshX-Low 10,
                      q-RxLevMin -30,
                      p-MaxUTRA 1
                    },
                    {
                      carrierFreq 11720,
                      cellReselectionPriority 3,
                      threshX-High 25,
                      threshX-Low 12,
                      q-RxLevMin -40,
                      p-MaxUTRA 5
                    }
                  },
                  t-ReselectionUTRA 3,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot5,
                    sf-High lDot0
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_LTE_RRC_MEAS_TEST_SIB6_FDD_TDD_MSG1 \
static const uint8 lte_rrc_meas_test_sib6_fdd_tdd_msg1[] = \
{\
  0x00, 0x11, 0xC9, 0x01, 0x92, 0xE5, 0xEF, 0x37, 0x72, 0x06, 0x43, 0xCA, \
  0x9E, 0x67, 0x34, 0x12, 0xC7, 0x96, 0x28, 0xDD, 0xC4, 0xDA, 0xA1, 0x72, \
  0xF7, 0x9B, 0xB6, 0xC1, 0x3C, 0xA9, 0xE6, 0x6D, 0xB9, 0x0F, 0x2C, 0x51, \
  0xBB, 0x70, \
}
/*
SIB7:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib7 : 
                {
                  t-ReselectionGERAN 4,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot5,
                    sf-High oDot75
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 50,
                        bandIndicator dcs1800,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            55,
                            60,
                            65,
                            70,
                            75,
                            80,
                            85
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000101'B,
                        q-RxLevMin 35,
                        p-MaxGERAN 20,
                        threshX-High 30,
                        threshX-Low 15
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 150,
                        bandIndicator pcs1900,
                        followingARFCNs equallySpacedARFCNs : 
                          {
                            arfcn-Spacing 5,
                            numberOfFollowingARFCNs 5
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000011'B,
                        q-RxLevMin 40,
                        p-MaxGERAN 28,
                        threshX-High 30,
                        threshX-Low 10
                      }
                    }
                  }
                }
            }
          }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_SIB7_MSG1 \
static const uint8 lte_rrc_meas_test_sib7_msg1[] = \
{ \
  0x00, 0x15, 0xC6, 0x10, 0x64, 0x0E, 0x1B, 0x87, 0x82, 0x08, 0x8C, 0x25, \
  0x8A, 0x02, 0xAF, 0x41, 0x63, 0x53, 0xCF, 0x12, 0xD6, 0x17, 0xA0, 0x74, \
  0x39, 0xE5, 0x00 \
}

/* 
sib7_dup_freq:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib7 : 
                {
                  t-ReselectionGERAN 4,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot5,
                    sf-High oDot75
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 50,
                        bandIndicator dcs1800,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            55,
                            60,
                            65,
                            65,
                            60,
                            55,
                            60
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000101'B,
                        q-RxLevMin 35,
                        p-MaxGERAN 20,
                        threshX-High 30,
                        threshX-Low 15
                      }
                    }
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_LTE_RRC_MEAS_TEST_SIB7_MSG2 \
static const uint8 lte_rrc_meas_test_sib7_msg2[] = \
{ \
  0x00, 0x15, 0xC6, 0x00, 0x64, 0x0E, 0x1B, 0x87, 0x82, 0x08, 0x82, \
  0x1E, 0x06, 0xE1, 0xE7, 0x41, 0x63, 0x53, 0xCF \
}

/*
SIB7:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib7 : 
                {
                  t-ReselectionGERAN 7,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 515,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            620,
                            621
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00100000'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 129,
                        bandIndicator dcs1800,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            124,
                            620
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00100000'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 129,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            515,
                            620,
                            126,
                            128
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00100000'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 31
                      }
                    }
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_LTE_RRC_MEAS_TEST_SIB7_MSG3 \
static const uint8 lte_rrc_meas_test_sib7_msg3[] = \
{ \
  0x00, 0x15, 0xF0, 0x24, 0x07, 0x05, 0x36, 0x4D, 0xBD, 0x20, 0x0C, 0x0F, 0x80, \
  0x40, 0x81, 0x0F, 0x93, 0x67, 0x48, 0x03, 0x03, 0xE0, 0x10, 0x30, 0x90, 0x1C, \
  0xD8, 0x3F, 0x10, 0x1D, 0x20, 0x0C, 0x0F, 0x80  \
}

/*
sib7:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib7 : 
                {
                  t-ReselectionGERAN 0,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 133,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            128,
                            130,
                            135,
                            140,
                            145,
                            512,
                            168,
                            514,
                            512,
                            516,
                            518,
                            520,
                            525,
                            530,
                            535,
                            540,
                            545,
                            560,
                            565,
                            570,
                            577,
                            580,
                            585,
                            590,
                            595,
                            600,
                            625,
                            635,
                            628,
                            639,
                            710
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 4,
                        ncc-Permitted '00100000'B,
                        q-RxLevMin 10,
                        p-MaxGERAN 0,
                        threshX-High 5,
                        threshX-Low 5
                      }
                    }
                  }
                }
            }
          }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_SIB7_MSG4 \
static const uint8 lte_rrc_meas_test_sib7_msg4[] = \
{ \
  0x00, 0x15, 0x80, 0x01, 0x0B, 0x3E, 0x40, 0x10, 0x44, 0x39, 0x18, 0x48, 0xC0, \
  0x05, 0x44, 0x05, 0x00, 0x40, 0x90, 0x34, 0x11, 0x06, 0xC2, 0x50, 0xBC, 0x39, \
  0x10, 0xC6, 0x11, 0xAC, 0x75, 0x20, 0xC8, 0x92, 0x4C, 0x9D, 0x29, 0xCB, 0x13, \
  0x8C, 0xF7, 0x3A, 0x4F, 0xF6, 0x37, 0x08, 0x0A, 0x00, 0xA5 \
}

/* 
SIB8:value BCCH-DL-SCH-Message ::= 
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
                    cdma-EUTRA-Synchronisation TRUE,
                    cdma-SystemTime synchronousSystemTime : '00000001 00000000 00000010 00000100 000 ...'B
                  },
                  searchWindowSize 10,
                  parametersHRPD 
                  {
                    preRegistrationInfoHRPD 
                    {
                      preRegistrationAllowed FALSE
                    },
                    cellReselectionParametersHRPD 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc0,
                          cellReselectionPriority 1,
                          threshX-High 60,
                          threshX-Low 10
                        },
                        {
                          bandClass bc3,
                          threshX-High 50,
                          threshX-Low 30
                        },
                        {
                          bandClass bc1,
                          cellReselectionPriority 2,
                          threshX-High 60,
                          threshX-Low 25
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc0,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 10,
                              physCellIdList 
                              {
                                10,
                                15
                              }
                            },
                            {
                              arfcn 20,
                              physCellIdList 
                              {
                                20,
                                25
                              }
                            }
                          }
                        },
                        {
                          bandClass bc1,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 30,
                              physCellIdList 
                              {
                                30,
                                35
                              }
                            },
                            {
                              arfcn 40,
                              physCellIdList 
                              {
                                40,
                                45
                              }
                            }
                          }
                        },
                        {
                          bandClass bc3,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 50,
                              physCellIdList 
                              {
                                50,
                                55
                              }
                            },
                            {
                              arfcn 60,
                              physCellIdList 
                              {
                                60
                              }
                            },
                            {
                              arfcn 70,
                              physCellIdList 
                              {
                                70,
                                72,
                                75
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 5,
                      t-ReselectionCDMA2000-SF 
                      {
                        sf-Medium oDot5,
                        sf-High oDot75
                      }
                    }
                  },
                  parameters1XRTT 
                  {
                    cellReselectionParameters1XRTT 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc4,
                          cellReselectionPriority 3,
                          threshX-High 63,
                          threshX-Low 0
                        },
                        {
                          bandClass bc16,
                          threshX-High 40,
                          threshX-Low 20
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc4,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 100,
                              physCellIdList 
                              {
                                100,
                                102,
                                104,
                                106,
                                108
                              }
                            },
                            {
                              arfcn 110,
                              physCellIdList 
                              {
                                110,
                                112,
                                115
                              }
                            }
                          }
                        },
                        {
                          bandClass bc16,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 120,
                              physCellIdList 
                              {
                                120,
                                125
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 0,
                      t-ReselectionCDMA2000-SF 
                      {
                        sf-Medium oDot25,
                        sf-High oDot25
                      }
                    }
                  }
                }
            }
          }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_SIB8_MSG1 \
static const uint8 lte_rrc_meas_test_sib8_msg1[] = \
{ \
  0x00, 0x19, 0xF0, 0x08, 0x00, 0x10, 0x20, 0x0A, 0x88, 0x90, 0x0F, 0x85, 0x01, 0xE4, \
  0xF2, 0x0A, 0xF1, 0x92, 0x00, 0x40, 0x50, 0x82, 0x81, 0xE0, 0x50, 0x42, 0x81, 0x90, \
  0x44, 0x0F, 0x08, 0x78, 0x46, 0x0A, 0x04, 0x50, 0x2D, 0x0C, 0x81, 0x90, 0x8C, 0x86, \
  0xE0, 0xF0, 0x07, 0x81, 0x18, 0x88, 0xC4, 0x82, 0x5D, 0x63, 0x0A, 0x23, 0xFC, 0x01, \
  0x0A, 0x14, 0x11, 0x04, 0x32, 0x21, 0x90, 0xCC, 0x68, 0x35, 0x1B, 0x03, 0x71, 0x1B, \
  0x8E, 0x07, 0x34, 0x00, 0x3C, 0x09, 0xE0, 0xFA, 0x00,   \
}

/*
SIB8:value BCCH-DL-SCH-Message ::= 
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
                    cdma-EUTRA-Synchronisation TRUE,
                    cdma-SystemTime synchronousSystemTime : '00000001 00000000 00000010 00000100 000 ...'B
                  },
                  searchWindowSize 10,
                  parametersHRPD 
                  {
                    preRegistrationInfoHRPD 
                    {
                      preRegistrationAllowed FALSE
                    },
                    cellReselectionParametersHRPD 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc0,
                          cellReselectionPriority 1,
                          threshX-High 60,
                          threshX-Low 10
                        },
                        {
                          bandClass bc3,
                          threshX-High 50,
                          threshX-Low 30
                        },
                        {
                          bandClass bc1,
                          cellReselectionPriority 2,
                          threshX-High 60,
                          threshX-Low 25
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc0,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 10,
                              physCellIdList 
                              {
                                10,
                                15
                              }
                            },
                            {
                              arfcn 20,
                              physCellIdList 
                              {
                                20,
                                25
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 5,
                      t-ReselectionCDMA2000-SF 
                      {
                        sf-Medium oDot5,
                        sf-High oDot75
                      }
                    }
                  },
                  parameters1XRTT 
                  {
                    cellReselectionParameters1XRTT 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc4,
                          cellReselectionPriority 3,
                          threshX-High 63,
                          threshX-Low 0
                        },
                        {
                          bandClass bc16,
                          threshX-High 40,
                          threshX-Low 20
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc4,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 100,
                              physCellIdList 
                              {
                                100,
                                102,
                                104,
                                106,
                                108
                              }
                            },
                            {
                              arfcn 110,
                              physCellIdList 
                              {
                                110,
                                112,
                                115
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 0,
                      t-ReselectionCDMA2000-SF 
                      {
                        sf-Medium oDot25,
                        sf-High oDot25
                      }
                    }
                  }
                }
            }
          }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_SIB8_MSG2 \
static const uint8 lte_rrc_meas_test_sib8_msg2[] = \
{ \
  0x00, 0x19, 0xF0, 0x08, 0x00, 0x10, 0x20, 0x0A, 0x88, 0x90, 0x0F, 0x85, 0x01, 0xE4, \
  0xF2, 0x0A, 0xF1, 0x90, 0x00, 0x40, 0x50, 0x82, 0x81, 0xE0, 0x50, 0x42, 0x81, 0x9A, \
  0xC6, 0x14, 0x47, 0xF8, 0x02, 0x14, 0x28, 0x02, 0x08, 0x64, 0x43, 0x21, 0x98, 0xD0, \
  0x6A, 0x36, 0x06, 0xE2, 0x37, 0x1C, 0x0E, 0x60, 0x00, \
}
/*
sib8_dup_1:value BCCH-DL-SCH-Message ::= 
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
                    cdma-EUTRA-Synchronisation TRUE,
                    cdma-SystemTime synchronousSystemTime : '00000001 00000000 00000010 00000100 000 ...'B
                  },
                  searchWindowSize 10,
                  parametersHRPD 
                  {
                    preRegistrationInfoHRPD 
                    {
                      preRegistrationAllowed FALSE
                    },
                    cellReselectionParametersHRPD 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc0,
                          cellReselectionPriority 1,
                          threshX-High 60,
                          threshX-Low 10
                        },
                        {
                          bandClass bc3,
                          threshX-High 50,
                          threshX-Low 30
                        },
                        {
                          bandClass bc1,
                          cellReselectionPriority 2,
                          threshX-High 60,
                          threshX-Low 25
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc0,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 10,
                              physCellIdList 
                              {
                                10,
                                20
                              }
                            },
                            {
                              arfcn 10,
                              physCellIdList 
                              {
                                20,
                                25
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 5,
                      t-ReselectionCDMA2000-SF 
                      {
                        sf-Medium oDot5,
                        sf-High oDot75
                      }
                    }
                  },
                  parameters1XRTT 
                  {
                    cellReselectionParameters1XRTT 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc4,
                          cellReselectionPriority 3,
                          threshX-High 63,
                          threshX-Low 0
                        },
                        {
                          bandClass bc16,
                          threshX-High 40,
                          threshX-Low 20
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc4,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 100,
                              physCellIdList 
                              {
                                100,
                                102,
                                104,
                                106,
                                108
                              }
                            },
                            {
                              arfcn 110,
                              physCellIdList 
                              {
                                110,
                                112,
                                115
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 0,
                      t-ReselectionCDMA2000-SF 
                      {
                        sf-Medium oDot25,
                        sf-High oDot25
                      }
                    }
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_LTE_RRC_MEAS_TEST_SIB8_MSG3 \
static const uint8 lte_rrc_meas_test_sib8_msg3[] = \
{ \
  0x00, 0x19, 0xF0, 0x08, 0x00, 0x10, 0x20, 0x0A, 0x88, \
  0x90, 0x0F, 0x85, 0x01, 0xE4, 0xF2, 0x0A, 0xF1, 0x90, \
  0x00, 0x40, 0x50, 0x82, 0x82, 0x80, 0x28, 0x42, 0x81, \
  0x9A, 0xC6, 0x14, 0x47, 0xF8, 0x02, 0x14, 0x28, 0x02, \
  0x00, 0x64, 0x43, 0x21, 0x98, 0xD0, 0x6A, 0x36, 0x00 \
}

/*
sib8_dup2:value BCCH-DL-SCH-Message ::= 
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
                    cdma-EUTRA-Synchronisation TRUE,
                    cdma-SystemTime synchronousSystemTime : '00000001 00000000 00000010 00000100 000 ...'B
                  },
                  searchWindowSize 10,
                  parametersHRPD 
                  {
                    preRegistrationInfoHRPD 
                    {
                      preRegistrationAllowed FALSE
                    },
                    cellReselectionParametersHRPD 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc0,
                          cellReselectionPriority 1,
                          threshX-High 60,
                          threshX-Low 10
                        },
                        {
                          bandClass bc3,
                          threshX-High 50,
                          threshX-Low 30
                        },
                        {
                          bandClass bc1,
                          cellReselectionPriority 2,
                          threshX-High 60,
                          threshX-Low 25
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc0,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 10,
                              physCellIdList 
                              {
                                10,
                                15
                              }
                            },
                            {
                              arfcn 20,
                              physCellIdList 
                              {
                                20,
                                25
                              }
                            }
                          }
                        },
                        {
                          bandClass bc1,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 60,
                              physCellIdList 
                              {
                                73,
                                74
                              }
                            },
                            {
                              arfcn 40,
                              physCellIdList 
                              {
                                40,
                                45
                              }
                            }
                          }
                        },
                        {
                          bandClass bc3,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 70,
                              physCellIdList 
                              {
                                73,
                                72
                              }
                            },
                            {
                              arfcn 60,
                              physCellIdList 
                              {
                                60
                              }
                            },
                            {
                              arfcn 70,
                              physCellIdList 
                              {
                                70,
                                72,
                                70
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 5,
                      t-ReselectionCDMA2000-SF 
                      {
                        sf-Medium oDot5,
                        sf-High oDot75
                      }
                    }
                  },
                  parameters1XRTT 
                  {
                    cellReselectionParameters1XRTT 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc4,
                          cellReselectionPriority 3,
                          threshX-High 63,
                          threshX-Low 0
                        },
                        {
                          bandClass bc16,
                          threshX-High 40,
                          threshX-Low 20
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc4,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 110,
                              physCellIdList 
                              {
                                100,
                                102,
                                115,
                                106,
                                108
                              }
                            },
                            {
                              arfcn 110,
                              physCellIdList 
                              {
                                110,
                                110,
                                115
                              }
                            }
                          }
                        },
                        {
                          bandClass bc16,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 120,
                              physCellIdList 
                              {
                                120,
                                125
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 0,
                      t-ReselectionCDMA2000-SF 
                      {
                        sf-Medium oDot25,
                        sf-High oDot25
                      }
                    }
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_LTE_RRC_MEAS_TEST_SIB8_MSG4 \
static const uint8 lte_rrc_meas_test_sib8_msg4[] = \
{ \
  0x00, 0x19, 0xF0, 0x08, 0x00, 0x10, 0x20, 0x0A, 0x88, 0x90, 0x0F, 0x85, 0x01, \
  0xE4, 0xF2, 0x0A, 0xF1, 0x92, 0x00, 0x40, 0x50, 0x82, 0x81, 0xE0, 0x50, 0x42, \
  0x81, 0x90, 0x44, 0x1E, 0x09, 0x24, 0x94, 0x0A, 0x04, 0x50, 0x2D, 0x0C, 0x82, \
  0x30, 0x92, 0x49, 0x00, 0xF0, 0x07, 0x81, 0x18, 0x88, 0xC4, 0x82, 0x35, 0x63, \
  0x0A, 0x23, 0xFC, 0x01, 0x0A, 0x14, 0x11, 0x04, 0x37, 0x21, 0x90, 0xCC, 0x73, \
  0x35, 0x1B, 0x03, 0x71, 0x1B, 0x8D, 0xC7, 0x34, 0x00, 0x3C, 0x09, 0xE0, 0xFA, \
  0x00, \
}

/*
SIB8_Extended_neigh_list:value BCCH-DL-SCH-Message ::= 
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
                    cdma-SystemTime asynchronousSystemTime : '00001111 00001111 00001110 00001010 000 ...'B
                  },
                  searchWindowSize 10,
                  parametersHRPD 
                  {
                    preRegistrationInfoHRPD 
                    {
                      preRegistrationAllowed FALSE
                    },
                    cellReselectionParametersHRPD 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc0,
                          cellReselectionPriority 4,
                          threshX-High 25,
                          threshX-Low 12
                        },
                        {
                          bandClass bc1,
                          cellReselectionPriority 3,
                          threshX-High 26,
                          threshX-Low 14
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc0,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 1,
                              physCellIdList 
                              {
                                10
                              }
                            },
                            {
                              arfcn 2,
                              physCellIdList 
                              {
                                20
                              }
                            }
                          }
                        },
                        {
                          bandClass bc1,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 3,
                              physCellIdList 
                              {
                                30
                              }
                            },
                            {
                              arfcn 4,
                              physCellIdList 
                              {
                                40
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 0
                    }
                  },
                  cellReselectionParametersHRPD-v920 
                  {
                    neighCellList-v920 
                    {
                      {
                        neighCellsPerFreqList-v920 
                        {
                          {
                            physCellIdList-v920 
                            {
                              110,
                              210
                            }
                          },
                          {
                            physCellIdList-v920 
                            {
                              120,
                              220
                            }
                          }
                        }
                      },
                      {
                        neighCellsPerFreqList-v920 
                        {
                          {
                            physCellIdList-v920 
                            {
                              130,
                              230
                            }
                          },
                          {
                            physCellIdList-v920 
                            {
                              140,
                              240
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

#define DECLARE_LTE_RRC_MEAS_TEST_SIB8_EXTENDED_NEIGH_CELLS_MSG \
static const uint8 lte_rrc_meas_test_sib8_extended_neigh_cells_msg[] = \
{ \
  0x00, 0x1B, 0xC8, 0x78, 0x78, 0x70, 0x50, 0x58, 0x62, 0xA0, 0x14, 0x08, 0xC9, \
  0x88, 0x2D, 0xA3, 0x84, 0x01, 0x00, 0x20, 0x0A, 0x00, 0x40, 0x14, 0x04, 0x40, \
  0x18, 0x07, 0x80, 0x20, 0x0A, 0x00, 0x14, 0x39, 0x02, 0x22, 0x37, 0x34, 0x84, \
  0x78, 0x6E, 0x08, 0x90, 0x4E, 0x61, 0x23, 0x1E, 0x00, \
}

/*
SIB8:value BCCH-DL-SCH-Message ::= 
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
                    cdma-EUTRA-Synchronisation TRUE,
                    cdma-SystemTime synchronousSystemTime : '00000001 00000000 00000000 00000001 000 ...'B
                  },
                  searchWindowSize 10,
                  parametersHRPD 
                  {
                    preRegistrationInfoHRPD 
                    {
                      preRegistrationAllowed FALSE
                    },
                    cellReselectionParametersHRPD 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc0,
                          threshX-High 25,
                          threshX-Low 12
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc0,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 1,
                              physCellIdList 
                              {
                                10
                              }
                            },
                            {
                              arfcn 2,
                              physCellIdList 
                              {
                                20
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 0
                    }
                  },
                  cellReselectionParametersHRPD-v920 
                  {
                    neighCellList-v920 
                    {
                      {
                        neighCellsPerFreqList-v920 
                        {
                          {
                            physCellIdList-v920 
                            {
                              110,
                              210
                            }
                          },
                          {
                            physCellIdList-v920 
                            {
                              120,
                              220
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

#define DECLARE_LTE_RRC_MEAS_TEST_SIB8_EXTENDED_NEIGH_CELLS_NO_RESEL_PRI_MSG \
static const uint8 lte_rrc_meas_test_sib8_extended_neigh_cells_no_resel_pri_msg[] = \
{ \
  0x00, 0x1B, 0xD0, 0x08, 0x00, 0x00, 0x08, 0x0A, 0x80, 0x00, 0x19, 0x30, 0x00, \
  0x10, 0x02, 0x00, 0xA0, 0x04, 0x01, 0x40, 0x05, 0x08, 0x40, 0x08, 0x8D, 0xCD, \
  0x21, 0x1E, 0x1B, 0x80, \
}

/*

*/

#define DECLARE_LTE_RRC_MEAS_TEST_SIB8_DUP_BANDCLASSES_MSG \
static const uint8 lte_rrc_meas_test_sib8_dup_bandclasses_msg[] = \
{ \
  0x00, 0x1B, 0xB1, 0x71, 0xD2, 0x20, 0x70, 0x9A, 0xA0, 0x00, 0x40, 0x00, 0x70, \
  0x00, 0x00, 0x20, 0x81, 0x40, 0x6C, 0xB2, 0x80, 0xD9, 0x64, 0x40, 0x05, 0x6C, \
  0x20, 0x50, 0x78, 0x00, 0xAD, 0x80, 0xD0, 0x40, 0xA2, 0x07, 0x00, 0x42, 0xF9, \
  0x90, 0xD7, 0x70, 0xBA, 0xDF, 0x50, 0x48, 0x70, 0x20, 0x00, 0x00, 0x1C, 0x00, \
  0x00, 0x00, 0x00, \
}

/*
SIB8:value BCCH-DL-SCH-Message ::= 
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
                    cdma-SystemTime asynchronousSystemTime : '00001111 00001111 00001110 00001010 000 ...'B
                  },
                  searchWindowSize 10,
                  parametersHRPD 
                  {
                    preRegistrationInfoHRPD 
                    {
                      preRegistrationAllowed FALSE
                    },
                    cellReselectionParametersHRPD 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc0,
                          cellReselectionPriority 4,
                          threshX-High 25,
                          threshX-Low 12
                        },
                        {
                          bandClass bc1,
                          cellReselectionPriority 3,
                          threshX-High 26,
                          threshX-Low 14
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc0,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 1,
                              physCellIdList 
                              {
                                10
                              }
                            },
                            {
                              arfcn 2,
                              physCellIdList 
                              {
                                10
                              }
                            }
                          }
                        },
                        {
                          bandClass bc1,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 1,
                              physCellIdList 
                              {
                                10
                              }
                            },
                            {
                              arfcn 2,
                              physCellIdList 
                              {
                                10
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 0
                    }
                  },
                  cellReselectionParametersHRPD-v920 
                  {
                    neighCellList-v920 
                    {
                      {
                        neighCellsPerFreqList-v920 
                        {
                          {
                            physCellIdList-v920 
                            {
                              110,
                              110
                            }
                          },
                          {
                            physCellIdList-v920 
                            {
                              110,
                              110
                            }
                          }
                        }
                      },
                      {
                        neighCellsPerFreqList-v920 
                        {
                          {
                            physCellIdList-v920 
                            {
                              110,
                              110
                            }
                          },
                          {
                            physCellIdList-v920 
                            {
                              110,
                              110
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

#define DECLARE_LTE_RRC_MEAS_TEST_SIB8_EXT_NEIGH_NO_DUP_MSG \
static const uint8 lte_rrc_meas_test_sib8_ext_neigh_no_dup_msg[] = \
{ \
  0x00, 0x1B, 0xC8, 0x78, 0x78, 0x70, 0x50, 0x58, 0x62, 0xA0, 0x14, 0x08, 0xC9, \
  0x88, 0x2D, 0xA3, 0x84, 0x01, 0x00, 0x20, 0x0A, 0x00, 0x40, 0x0A, 0x04, 0x40, \
  0x08, 0x02, 0x80, 0x10, 0x02, 0x80, 0x14, 0x39, 0x02, 0x22, 0x37, 0x1B, 0x84, \
  0x6E, 0x37, 0x08, 0x8D, 0xC6, 0xE1, 0x1B, 0x8D, 0xC0, \
}

/*
SIB8:value BCCH-DL-SCH-Message ::= 
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
                    cdma-EUTRA-Synchronisation TRUE,
                    cdma-SystemTime synchronousSystemTime : '00000001 00000000 00000010 00000100 000 ...'B
                  },
                  searchWindowSize 10,
                  parametersHRPD 
                  {
                    preRegistrationInfoHRPD 
                    {
                      preRegistrationAllowed FALSE
                    },
                    cellReselectionParametersHRPD 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc0,
                          cellReselectionPriority 4,
                          threshX-High 60,
                          threshX-Low 10
                        },
                        {
                          bandClass bc3,
                          cellReselectionPriority 4,
                          threshX-High 50,
                          threshX-Low 30
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc0,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 10,
                              physCellIdList 
                              {
                                10,
                                15,
                                20,
                                25,
                                30,
                                35,
                                40
                              }
                            },
                            {
                              arfcn 20,
                              physCellIdList 
                              {
                                20,
                                25,
                                30,
                                35,
                                40,
                                45,
                                50,
                                55
                              }
                            }
                          }
                        },
                        {
                          bandClass bc3,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 30,
                              physCellIdList 
                              {
                                30,
                                35,
                                40,
                                45,
                                50
                              }
                            },
                            {
                              arfcn 40,
                              physCellIdList 
                              {
                                40,
                                45,
                                50,
                                55,
                                60
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 5,
                      t-ReselectionCDMA2000-SF 
                      {
                        sf-Medium oDot5,
                        sf-High oDot75
                      }
                    }
                  },
                  parameters1XRTT 
                  {
                    cellReselectionParameters1XRTT 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc0,
                          cellReselectionPriority 3,
                          threshX-High 63,
                          threshX-Low 0
                        },
                        {
                          bandClass bc16,
                          threshX-High 40,
                          threshX-Low 20
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc0,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 100,
                              physCellIdList 
                              {
                                100,
                                102,
                                104,
                                106,
                                108
                              }
                            },
                            {
                              arfcn 110,
                              physCellIdList 
                              {
                                110,
                                112,
                                115
                              }
                            }
                          }
                        },
                        {
                          bandClass bc16,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 120,
                              physCellIdList 
                              {
                                120,
                                125
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 0,
                      t-ReselectionCDMA2000-SF 
                      {
                        sf-Medium oDot25,
                        sf-High oDot25
                      }
                    }
                  },
                  cellReselectionParameters1XRTT-v920 
                  {
                    neighCellList-v920 
                    {
                      {
                        neighCellsPerFreqList-v920 
                        {
                          {
                            physCellIdList-v920 
                            {
                              150,
                              155,
                              160,
                              155
                            }
                          },
                          {
                            physCellIdList-v920 
                            {
                              170,
                              175,
                              180,
                              185,
                              190
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

#define DECLARE_LTE_RRC_MEAS_TEST_SIB8_EXT_NEIGH_1X_MSG \
static const uint8 lte_rrc_meas_test_sib8_ext_neigh_1x_msg[] = \
{ \
  0x00, 0x1B, 0xF0, 0x08, 0x00, 0x10, 0x20, 0x0A, 0x88, 0x50, 0x27, 0x85, 0x21, \
  0xCC, 0x9E, 0x10, 0x04, 0x05, 0x30, 0x28, 0x1E, 0x14, 0x0C, 0x87, 0x84, 0x62, \
  0x80, 0x28, 0xE1, 0x40, 0xC8, 0x78, 0x46, 0x28, 0x16, 0x8C, 0x86, 0xE1, 0x88, \
  0x1E, 0x40, 0xF0, 0x8C, 0x50, 0x2D, 0x19, 0x02, 0x84, 0x14, 0x0B, 0x46, 0x43, \
  0x71, 0xE5, 0x63, 0x0A, 0x03, 0xFC, 0x01, 0x0A, 0x14, 0x10, 0x04, 0x32, 0x21, \
  0x90, 0xCC, 0x68, 0x35, 0x1B, 0x03, 0x71, 0x1B, 0x8E, 0x07, 0x34, 0x00, 0x3C, \
  0x09, 0xE0, 0xFA, 0x00, 0x0A, 0x1A, 0x40, 0x12, 0x25, 0x93, 0x6A, 0x04, 0xD9, \
  0x55, 0x4A, 0xF5, 0xA2, 0xE5, 0x7C \
}

/*
TypicalMeasConfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
              }
            }
      }
}
*/
#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_MSG1 \
static const uint8 lte_rrc_meas_test_meas_config_msg1[] = \
{ \
  0x22, 0x10, 0x00, 0x00 \
}

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_MSG_GAPS \
static const uint8 lte_rrc_meas_test_meas_config_msg_gaps[] = \
{ \
  0x20, 0x10, 0x15, 0x80, 0x80, 0x00, 0x04, \
  0x33, 0x30, 0x10, 0x00, 0x5A, 0x79, 0x81, \
  0x00, 0x12, 0x8E, 0x0C, 0x00, 0x00, 0x96, \
  0x11, 0xC0, 0x04, 0x40, 0x00, 0x02, 0x10, \
  0x08, 0x40, 0x46, 0x00, 0x00 \
}

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_MSG_GAPS_HO \
static const uint8 lte_rrc_meas_test_meas_config_msg_gaps_HO[] = \
{ \
  0x20, 0x0b, 0xb4, 0x06, 0x94, 0x70, 0x1e, \
  0x00, 0x1e, 0x7b, 0x4c, 0x58, 0xff, 0x50, \
  0xc1, 0x24, 0x80, 0x78, 0x80, 0x80, 0x00, \
  0x41, 0x30, 0x00, 0x29, 0xaa, 0x10, 0x02, \
  0x5d, 0xf8, 0x01, 0x60, 0x6a, 0x28, 0x7c, \
  0x14, 0x88, 0xb0, 0x00\
}


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
                measSubframePatternPCell-r10 release : NULL
              }
            }
      }
}

*/
#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_EICIC_REL_MSG1 \
static const uint8 lte_rrc_meas_test_meas_eicic_rel_msg1[] = \
{ \
  0x22, 0x12, 0x10, 0x00, 0x00, 0x20, 0x00, 0x32, 0x00, 0x02, 0x02, 0x81, 0x00, 0x0A, 0x03, 0x00 \
}


/*
eICIC_msg:value DL-DCCH-Message ::= 
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
              }
            }
      }
}

*/
#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_EICIC_MSG1 \
static const uint8 lte_rrc_meas_test_meas_eicic_msg1[] = \
{ \
  0x22, 0x10, 0x10, 0x00, 0x00, 0x20, 0x00, 0x32, 0x00, 0x02, 0x14, 0xE2, 0x0A, 0x12, 0x1A, 0x22, 0x28, 0x60, 0x12, 0x01, 0x00 \
}

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
                            measSubframePatternNeigh-r10 subframePatternTDD-r10 : subframeConfig1-5-r10 : '10011001 10011001 1001'B,
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
#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_EICIC_TDD_MSG1 \
static const uint8 lte_rrc_meas_test_meas_eicic_tdd_msg1[] = \
{ \
  0x22, 0x12, 0x10, 0x00, 0x00, 0x20, 0x00, 0x32, 0x00, 0x02, 0x10, 0xE8, 0x99, 0x99, 0x90, 0xC0, 0x24, 0x02, 0x01, 0x00, 0x0A, 0x03, 0x00 \
}

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
                targetPhysCellId 12,
                carrierFreq 
                {
                  dl-CarrierFreq 512
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

#define DECLARE_LTE_RRC_MEAS_TEST_NO_MEAS_CFG_DURING_HO_MSG \
static const uint8 lte_rrc_meas_test_no_meas_config_during_ho_msg[] = \
{ \
  0x22, 0x09, 0x3C, 0x18, 0x02, 0x00, 0x90, 0x25, 0x00, 0x80, 0x00, 0x10, 0x14, \
  0x18, 0x80, 0x20, 0x11, 0x40, 0x11, 0x20, 0x20 \
}

/*
Reconfig:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 0,
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
                        carrierFreq 2130,
                        allowedMeasBandwidth mbw50,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '01'B
                      }
                  },
                  {
                    measObjectId 2,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 2300,
                        allowedMeasBandwidth mbw50,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '01'B
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 2,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType event : 
                          {
                            eventId eventA2 : 
                              {
                                a2-Threshold threshold-RSRP : 50
                              },
                            hysteresis 2,
                            timeToTrigger ms128
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 5,
                        reportInterval ms480,
                        reportAmount r4
                      }
                  },
                  {
                    reportConfigId 1,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType event : 
                          {
                            eventId eventA4 : 
                              {
                                a4-Threshold threshold-RSRP : 35
                              },
                            hysteresis 5,
                            timeToTrigger ms40
                          },
                        triggerQuantity rsrp,
                        reportQuantity sameAsTriggerQuantity,
                        maxReportCells 2,
                        reportInterval ms120,
                        reportAmount r4
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 1,
                    measObjectId 1,
                    reportConfigId 2
                  },
                  {
                    measId 2,
                    measObjectId 2,
                    reportConfigId 1
                  }
                },
                measGapConfig setup : 
                  {
                    gapOffset gp0 : 36
                  }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_IFREQ_MSG1 \
static const uint8 lte_rrc_meas_test_meas_config_ifreq_msg1[] = \
{ \
  0x20, 0x10, 0x15, 0x40, 0x40, 0x00, 0x04, 0x29, 0x32, 0x10, 0x00, 0x23, 0xF1, \
  0x90, 0x84, 0x09, 0x90, 0x95, 0x84, 0x80, 0x0C, 0x8C, 0xA2, 0x10, 0x41, 0x00, \
  0x02, 0x10, 0x82, 0x48, \
}


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
                    measObjectId 5,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 100,
                        offsetFreq -10,
                        cellsToRemoveList 
                        {
                          1
                        },
                        cellsToAddModList cellsToAddModListUTRA-FDD : 
                          {
                            {
                              cellIndex 1,
                              physCellId 100
                            },
                            {
                              cellIndex 2,
                              physCellId 200
                            },
                            {
                              cellIndex 3,
                              physCellId 300
                            },
                            {
                              cellIndex 4,
                              physCellId 400
                            },
                            {
                              cellIndex 5,
                              physCellId 500
                            }
                          },
                        cellForWhichToReportCGI utra-FDD : 250
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRQ : 20,
                                b2-Threshold2 b2-Threshold2UTRA : utra-EcN0 : 20
                              },
                            hysteresis 15,
                            timeToTrigger ms80
                          },
                        maxReportCells 4,
                        reportInterval min1,
                        reportAmount r16
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigUTRA 
                  {
                    measQuantityUTRA-FDD cpich-EcN0,
                    measQuantityUTRA-TDD pccpch-RSCP,
                    filterCoefficient fc3
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_WCDMA_MSG \
static const uint8 lte_rrc_meas_test_meas_config_wcdma_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x5E, 0x03, 0x21, 0x40, 0x01, 0x00, 0x64, 0x0B, \
  0x20, 0x52, 0xC1, 0xE4, 0x09, 0xF4, 0x3E, 0x80, 0x48, 0xD4, 0x2A, 0x3C, 0xDC, \
  0x40, 0x10, 0x84, 0x26, 0x30  \
}
/*
interRAT:value DL-DCCH-Message ::= 
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
                    measObjectId 5,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 100,
                        offsetFreq -10,
                        cellsToRemoveList 
                        {
                          1
                        },
                        cellsToAddModList cellsToAddModListUTRA-FDD : 
                          {
                            {
                              cellIndex 1,
                              physCellId 100
                            },
                            {
                              cellIndex 2,
                              physCellId 200
                            },
                            {
                              cellIndex 3,
                              physCellId 300
                            },
                            {
                              cellIndex 4,
                              physCellId 400
                            },
                            {
                              cellIndex 5,
                              physCellId 500
                            }
                          },
                        cellForWhichToReportCGI utra-FDD : 250
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRQ : 20,
                                b2-Threshold2 b2-Threshold2UTRA : utra-EcN0 : 20
                              },
                            hysteresis 15,
                            timeToTrigger ms80
                          },
                        maxReportCells 4,
                        reportInterval min1,
                        reportAmount r16,
                        includeLocationInfo-r11 TRUE
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigUTRA 
                  {
                    measQuantityUTRA-FDD cpich-EcN0,
                    measQuantityUTRA-TDD pccpch-RSCP,
                    filterCoefficient fc3
                  }
                }
              }
            }
      }
}
*/
#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_WCDMA_LOC_INFO_MSG \
static const uint8 lte_rrc_meas_test_meas_config_wcdma_loc_info_msg[] = \
{ \
0x22, 0x10, 0x15, 0x80, 0x08, 0x5E, 0x03, 0x21, 0x40, 0x01, 0x00, 0x64, 0x0B, \
0x20, 0x52, 0xC1, 0xE4, 0x09, 0xF4, 0x3E, 0x80, 0x4C, 0xD4, 0x2A, 0x3C, 0xDC, \
0x40, 0x44, 0x07, 0x00, 0x04, 0x21, 0x09, 0x8C \
}

/*
UTRA_with_no_mode_msg:value DL-DCCH-Message ::= 
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
                    measObjectId 5,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 100
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportStrongestCellsForSON
                          },
                        maxReportCells 4,
                        reportInterval min1,
                        reportAmount r16
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigUTRA 
                  {
                    measQuantityUTRA-FDD cpich-EcN0,
                    measQuantityUTRA-TDD pccpch-RSCP,
                    filterCoefficient fc3
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_UTRA_WITH_NO_MODE_MSG \
static const uint8 lte_rrc_meas_test_meas_config_utra_with_no_mode_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x40, 0x03, 0x20, 0x09, 0x57, 0x10, 0x04, 0x21, \
  0x09, 0x8C \
}

/*
WCDMA_SRC:value DL-DCCH-Message ::= 
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
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 100,
                        offsetFreq -10,
                        cellsToRemoveList 
                        {
                          1
                        },
                        cellsToAddModList cellsToAddModListUTRA-FDD : 
                          {
                            {
                              cellIndex 1,
                              physCellId 100
                            },
                            {
                              cellIndex 2,
                              physCellId 200
                            },
                            {
                              cellIndex 3,
                              physCellId 300
                            },
                            {
                              cellIndex 4,
                              physCellId 400
                            },
                            {
                              cellIndex 5,
                              physCellId 500
                            }
                          },
                        cellForWhichToReportCGI utra-FDD : 250
                      }
                  },
                  {
                    measObjectId 2,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 200,
                        offsetFreq -10,
                        cellsToRemoveList 
                        {
                          1
                        },
                        cellsToAddModList cellsToAddModListUTRA-FDD : 
                          {
                            {
                              cellIndex 1,
                              physCellId 100
                            }
                          },
                        cellForWhichToReportCGI utra-FDD : 250
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRQ : 20,
                                b2-Threshold2 b2-Threshold2UTRA : utra-EcN0 : 20
                              },
                            hysteresis 15,
                            timeToTrigger ms80
                          },
                        maxReportCells 4,
                        reportInterval min1,
                        reportAmount r16
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigUTRA 
                  {
                    measQuantityUTRA-FDD cpich-EcN0,
                    measQuantityUTRA-TDD pccpch-RSCP,
                    filterCoefficient fc3
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_WCDMA_SRC_MSG \
static const uint8 lte_rrc_meas_test_meas_config_wcdma_src_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x40, 0x5E, 0x03, 0x21, 0x40, 0x01, 0x00, 0x64, 0x0B, \
  0x20, 0x52, 0xC1, 0xE4, 0x09, 0xF4, 0x3E, 0x82, 0x5E, 0x06, 0x41, 0x40, 0x00, \
  0x00, 0x64, 0x3E, 0x80, 0x48, 0xD4, 0x2A, 0x3C, 0xDC, 0x40, 0x10, 0x84, 0x26, \
  0x30  \
}

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
                    measObjectId 5,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 100,
                        offsetFreq -10,
                        cellsToRemoveList 
                        {
                          1
                        },
                        cellsToAddModList cellsToAddModListUTRA-FDD : 
                          {
                            {
                              cellIndex 1,
                              physCellId 100
                            },
                            {
                              cellIndex 2,
                              physCellId 200
                            },
                            {
                              cellIndex 3,
                              physCellId 300
                            },
                            {
                              cellIndex 4,
                              physCellId 400
                            },
                            {
                              cellIndex 5,
                              physCellId 500
                            }
                          },
                        cellForWhichToReportCGI utra-FDD : 250
                      }
                  },
                  {
                    measObjectId 6,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 500,
                        offsetFreq 15,
                        cellsToAddModList cellsToAddModListUTRA-TDD : 
                          {
                            {
                              cellIndex 2,
                              physCellId 124
                            },
                            {
                              cellIndex 4,
                              physCellId 125
                            },
                            {
                              cellIndex 6,
                              physCellId 126
                            },
                            {
                              cellIndex 8,
                              physCellId 127
                            }
                          },
                        cellForWhichToReportCGI utra-TDD : 120
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRQ : 20,
                                b2-Threshold2 b2-Threshold2UTRA : utra-EcN0 : 20
                              },
                            hysteresis 15,
                            timeToTrigger ms80
                          },
                        maxReportCells 4,
                        reportInterval min1,
                        reportAmount r16
                      }
                  },
                  {
                    reportConfigId 6,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB1 : 
                              {
                                b1-Threshold b1-ThresholdUTRA : utra-RSCP : 91
                              },
                            hysteresis 20,
                            timeToTrigger ms2560
                          },
                        maxReportCells 5,
                        reportInterval min6,
                        reportAmount r8
                      }
                  },
                  {
                    reportConfigId 7,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportStrongestCellsForSON
                          },
                        maxReportCells 7,
                        reportInterval ms10240,
                        reportAmount r32
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 1,
                    measObjectId 5,
                    reportConfigId 5
                  },
                  {
                    measId 2,
                    measObjectId 5,
                    reportConfigId 6
                  },
                  {
                    measId 3,
                    measObjectId 5,
                    reportConfigId 7
                  },
                  {
                    measId 4,
                    measObjectId 6,
                    reportConfigId 5
                  },
                  {
                    measId 5,
                    measObjectId 6,
                    reportConfigId 7
                  },
                  {
                    measId 6,
                    measObjectId 6,
                    reportConfigId 6
                  }
                },
                quantityConfig 
                {
                  quantityConfigUTRA 
                  {
                    measQuantityUTRA-FDD cpich-EcN0,
                    measQuantityUTRA-TDD pccpch-RSCP,
                    filterCoefficient spare1
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_WCDMA_MSG2 \
static const uint8 lte_rrc_meas_test_meas_config_wcdma_msg2[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x48, 0x5E, 0x03, 0x21, 0x40, 0x01, 0x00, 0x64, 0x0B, \
  0x20, 0x52, 0xC1, 0xE4, 0x09, 0xF4, 0x3E, 0x8A, 0x56, 0x0F, 0xA7, 0xA3, 0x0F, \
  0xC1, 0xFD, 0x2F, 0xE3, 0xFF, 0xF8, 0x11, 0x23, 0x50, 0xA8, 0xF3, 0x71, 0x0B, \
  0x01, 0x82, 0x9D, 0x25, 0x9A, 0xB9, 0xE9, 0x40, 0x42, 0x04, 0x85, 0x11, 0x0C, \
  0x32, 0x90, 0x85, 0x31, 0x4A, 0x52, 0x6F \
}

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
                    measObjectId 5,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 100,
                        offsetFreq -10
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportStrongestCellsForSON
                          },
                        maxReportCells 4,
                        reportInterval min1,
                        reportAmount r16
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigUTRA 
                  {
                    measQuantityUTRA-FDD cpich-EcN0,
                    measQuantityUTRA-TDD pccpch-RSCP,
                    filterCoefficient fc3
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_WCDMA_MSG3 \
static const uint8 lte_rrc_meas_test_meas_config_wcdma_msg3[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x50, 0x03, 0x21, 0x40, 0x4A, 0xB8, 0x80, 0x21, \
  0x08, 0x4C, 0x60  \
}

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
                    measObjectId 5,
                    measObject measObjectGERAN : 
                      {
                        carrierFreqs 
                        {
                          startingARFCN 100,
                          bandIndicator dcs1800,
                          followingARFCNs equallySpacedARFCNs : 
                            {
                              arfcn-Spacing 5,
                              numberOfFollowingARFCNs 5
                            }
                        },
                        offsetFreq -5,
                        ncc-Permitted '00000110'B,
                        cellForWhichToReportCGI 
                        {
                          networkColourCode '011'B,
                          baseStationColourCode '101'B
                        }
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRQ : 20,
                                b2-Threshold2 b2-Threshold2GERAN : 35
                              },
                            hysteresis 15,
                            timeToTrigger ms80
                          },
                        maxReportCells 4,
                        reportInterval min1,
                        reportAmount r16
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigGERAN 
                  {
                    measQuantityGERAN rssi,
                    filterCoefficient fc5
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_GSM_MSG \
static const uint8 lte_rrc_meas_test_meas_config_gsm_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x9C, 0x64, 0x30, 0xAA, 0x06, 0x74, 0x04, 0x8D, \
  0x46, 0x37, 0x9B, 0x88, 0x02, 0x10, 0x82, 0x94, \
}

/*
GSM_Meas_Obj:value DL-DCCH-Message ::= 
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
                    measObject measObjectGERAN : 
                      {
                        carrierFreqs 
                        {
                          startingARFCN 100,
                          bandIndicator dcs1800,
                          followingARFCNs equallySpacedARFCNs : 
                            {
                              arfcn-Spacing 5,
                              numberOfFollowingARFCNs 5
                            }
                        },
                        offsetFreq -5,
                        ncc-Permitted '00000110'B,
                        cellForWhichToReportCGI 
                        {
                          networkColourCode '011'B,
                          baseStationColourCode '101'B
                        }
                      }
                  },
                  {
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 100,
                        allowedMeasBandwidth mbw6,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '00'B
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 1,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRQ : 20,
                                b2-Threshold2 b2-Threshold2GERAN : 35
                              },
                            hysteresis 15,
                            timeToTrigger ms80
                          },
                        maxReportCells 4,
                        reportInterval min1,
                        reportAmount r16
                      }
                  },
                  {
                    reportConfigId 5,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType event : 
                          {
                            eventId eventA2 : 
                              {
                                a2-Threshold threshold-RSRP : 0
                              },
                            hysteresis 0,
                            timeToTrigger ms0
                          },
                        triggerQuantity rsrp,
                        reportQuantity sameAsTriggerQuantity,
                        maxReportCells 1,
                        reportInterval ms120,
                        reportAmount r1
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  },
                  {
                    measId 1,
                    measObjectId 1,
                    reportConfigId 1
                  }
                },
                quantityConfig 
                {
                  quantityConfigGERAN 
                  {
                    measQuantityGERAN rssi,
                    filterCoefficient fc5
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_GSM_WITH_ADD_MOD_MSG \
static const uint8 lte_rrc_meas_test_meas_config_gsm_msg_with_add_mod[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x40, 0x9C, 0x64, 0x30, 0xAA, 0x06, 0x74, 0x80, 0x00, \
  0x03, 0x20, 0x01, 0x04, 0x6A, 0x31, 0xBC, 0xDC, 0x42, 0x01, 0x00, 0x00, 0x00, \
  0x00, 0x48, 0x42, 0x00, 0x00, 0x14, 0xA0, \
}


/*
GSM_Meas_Obj:value DL-DCCH-Message ::= 
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
                    measObject measObjectGERAN : 
                      {
                        carrierFreqs 
                        {
                          startingARFCN 100,
                          bandIndicator dcs1800,
                          followingARFCNs equallySpacedARFCNs : 
                            {
                              arfcn-Spacing 5,
                              numberOfFollowingARFCNs 5
                            }
                        },
                        offsetFreq -5,
                        ncc-Permitted '00000110'B,
                        cellForWhichToReportCGI 
                        {
                          networkColourCode '011'B,
                          baseStationColourCode '101'B
                        }
                      }
                  },
                  {
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 100,
                        allowedMeasBandwidth mbw6,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '00'B
                      }
                  },
                  {
                    measObjectId 2,
                    measObject measObjectGERAN : 
                      {
                        carrierFreqs 
                        {
                          startingARFCN 101,
                          bandIndicator dcs1800,
                          followingARFCNs equallySpacedARFCNs : 
                            {
                              arfcn-Spacing 6,
                              numberOfFollowingARFCNs 4
                            }
                        },
                        offsetFreq -5,
                        ncc-Permitted '00000110'B
                      }
                  },
                  {
                    measObjectId 4,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 100
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 1,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRQ : 20,
                                b2-Threshold2 b2-Threshold2GERAN : 35
                              },
                            hysteresis 15,
                            timeToTrigger ms80
                          },
                        maxReportCells 4,
                        reportInterval min1,
                        reportAmount r16
                      }
                  },
                  {
                    reportConfigId 5,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType event : 
                          {
                            eventId eventA2 : 
                              {
                                a2-Threshold threshold-RSRP : 0
                              },
                            hysteresis 0,
                            timeToTrigger ms0
                          },
                        triggerQuantity rsrp,
                        reportQuantity sameAsTriggerQuantity,
                        maxReportCells 1,
                        reportInterval ms120,
                        reportAmount r1
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  },
                  {
                    measId 1,
                    measObjectId 1,
                    reportConfigId 1
                  },
                  {
                    measId 2,
                    measObjectId 2,
                    reportConfigId 1
                  },
                  {
                    measId 4,
                    measObjectId 4,
                    reportConfigId 1
                  }
                },
                quantityConfig 
                {
                  quantityConfigGERAN 
                  {
                    measQuantityGERAN rssi,
                    filterCoefficient fc5
                  }
                }
              }
            }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_GSM_WITH_ADD_REM_MOD_MSG \
static const uint8 lte_rrc_meas_test_meas_config_gsm_msg_with_add_rem_mod[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0xC0, 0x9C, 0x64, 0x30, 0xAA, 0x06, 0x74, 0x80, 0x00, \
  0x03, 0x20, 0x01, 0x4C, 0x32, 0x9A, 0x45, 0x03, 0x0C, 0x80, 0x06, 0x40, 0x82, \
  0x35, 0x18, 0xDE, 0x6E, 0x21, 0x00, 0x80, 0x00, 0x00, 0x00, 0x64, 0x21, 0x00, \
  0x00, 0x04, 0x20, 0x18, 0xC0, 0x29, 0x40, \
}

/*
Meas_msg:value DL-DCCH-Message ::= 
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
                        carrierFreq 211,
                        allowedMeasBandwidth mbw6,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '00'B
                      }
                  },
                  {
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 100,
                        allowedMeasBandwidth mbw6,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '00'B
                      }
                  },
                  {
                    measObjectId 2,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 401,
                        allowedMeasBandwidth mbw6,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '00'B
                      }
                  },
                  {
                    measObjectId 4,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 100
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 1,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRQ : 20,
                                b2-Threshold2 b2-Threshold2GERAN : 35
                              },
                            hysteresis 15,
                            timeToTrigger ms80
                          },
                        maxReportCells 4,
                        reportInterval min1,
                        reportAmount r16
                      }
                  },
                  {
                    reportConfigId 5,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType event : 
                          {
                            eventId eventA2 : 
                              {
                                a2-Threshold threshold-RSRP : 0
                              },
                            hysteresis 0,
                            timeToTrigger ms0
                          },
                        triggerQuantity rsrp,
                        reportQuantity sameAsTriggerQuantity,
                        maxReportCells 1,
                        reportInterval ms120,
                        reportAmount r1
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  },
                  {
                    measId 1,
                    measObjectId 1,
                    reportConfigId 5
                  },
                  {
                    measId 2,
                    measObjectId 2,
                    reportConfigId 5
                  },
                  {
                    measId 4,
                    measObjectId 4,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigGERAN 
                  {
                    measQuantityGERAN rssi,
                    filterCoefficient fc5
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_INCORRECT_CONN_MEAS_LYR_CNT \
static const uint8 lte_rrc_meas_test_meas_incorrect_conn_meas_lyr_cnt[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0xC0, 0x00, 0x00, 0x69, 0x80, 0x40, 0x00, 0x01, 0x90, 0x00, 0x80, 0x00, 0x32, 0x20, 0x0C, 0x80, \
  0x06, 0x40, 0x82, 0x35, 0x18, 0xDE, 0x6E, 0x21, 0x00, 0x80, 0x00, 0x00, 0x00, 0x64, 0x21, 0x00, 0x02, 0x04, 0x24, 0x18, \
  0xC8, 0x29, 0x40, \
}



/*
Irat_gsm_meas_objs:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measObjectToRemoveList 
                {
                  1,
                  5
                },
                reportConfigToRemoveList 
                {
                  1,
                  5
                },
                measIdToRemoveList 
                {
                  1,
                  5
                },
                quantityConfig 
                {
                  quantityConfigGERAN 
                  {
                    measQuantityGERAN rssi,
                    filterCoefficient fc5
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_GSM_WITH_REM_MOD_MSG \
static const uint8 lte_rrc_meas_test_meas_config_gsm_msg_with_rem_mod[] = \
{ \
  0x22, 0x10, 0x2A, 0x80, 0x40, 0x40, 0x80, 0x81, 0x01, 0x05, 0x28,\
}

/*
GSM_Meas_Obj:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measObjectToRemoveList 
                {
                  2,
                  4
                },
                measIdToRemoveList 
                {
                  2,
                  4
                },
                quantityConfig 
                {
                  quantityConfigGERAN 
                  {
                    measQuantityGERAN rssi,
                    filterCoefficient fc5
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_GSM_WITH_REM_ADD_MOD_MSG \
static const uint8 lte_rrc_meas_test_meas_config_gsm_msg_with_rem_and_add_mod[] = \
{ \
  0x22, 0x10, 0x22, 0x80, 0x42, 0x30, 0x84, 0x62, 0x94,\
}

/*
GSM_Meas_Obj:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measObjectToRemoveList 
                {
                  1
                },
                measObjectToAddModList 
                {
                  {
                    measObjectId 2,
                    measObject measObjectGERAN : 
                      {
                        carrierFreqs 
                        {
                          startingARFCN 100,
                          bandIndicator dcs1800,
                          followingARFCNs equallySpacedARFCNs : 
                            {
                              arfcn-Spacing 5,
                              numberOfFollowingARFCNs 5
                            }
                        },
                        offsetFreq -5,
                        ncc-Permitted '00000110'B,
                        cellForWhichToReportCGI 
                        {
                          networkColourCode '011'B,
                          baseStationColourCode '101'B
                        }
                      }
                  }
                },
                measIdToRemoveList 
                {
                  1
                },
                measIdToAddModList 
                {
                  {
                    measId 2,
                    measObjectId 2,
                    reportConfigId 1
                  }
                },
                quantityConfig 
                {
                  quantityConfigGERAN 
                  {
                    measQuantityGERAN rssi,
                    filterCoefficient fc5
                  }
                }
              }
            }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_GSM_WITH_REM_ADD_MOD_GSM_MSG \
static const uint8 lte_rrc_meas_test_meas_config_gsm_msg_with_rem_and_add_mod_g_msg[] = \
{ \
  0x22, 0x10, 0x33, 0x80, 0x00, 0x00, 0xA7, 0x19, 0x0C, 0x2A, 0x81, 0x9D, 0x00, 0x00, 0x10, 0x80, 0x52, 0x80,\
}


/*
GSM_Meas_Obj:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measConfig 
              {
                measObjectToRemoveList 
                {
                  2
                },
                reportConfigToRemoveList 
                {
                  1
                },
                measIdToRemoveList 
                {
                  2
                },
                quantityConfig 
                {
                  quantityConfigGERAN 
                  {
                    measQuantityGERAN rssi,
                    filterCoefficient fc5
                  }
                }
              }
            }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_GSM_WITH_REM_ALL_G_MSG \
static const uint8 lte_rrc_meas_test_meas_config_gsm_msg_with_rem_all_g_msg[] = \
{ \
  0x22, 0x10, 0x2A, 0x80, 0x02, 0x00, 0x00, 0x22, 0x94,\
}

/*
GSM_src_msg:value DL-DCCH-Message ::= 
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
                    measObject measObjectGERAN : 
                      {
                        carrierFreqs 
                        {
                          startingARFCN 100,
                          bandIndicator dcs1800,
                          followingARFCNs equallySpacedARFCNs : 
                            {
                              arfcn-Spacing 5,
                              numberOfFollowingARFCNs 5
                            }
                        },
                        offsetFreq -5,
                        ncc-Permitted '00000110'B,
                        cellForWhichToReportCGI 
                        {
                          networkColourCode '011'B,
                          baseStationColourCode '101'B
                        }
                      }
                  },
                  {
                    measObjectId 2,
                    measObject measObjectGERAN : 
                      {
                        carrierFreqs 
                        {
                          startingARFCN 200,
                          bandIndicator dcs1800,
                          followingARFCNs equallySpacedARFCNs : 
                            {
                              arfcn-Spacing 5,
                              numberOfFollowingARFCNs 5
                            }
                        }
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRQ : 20,
                                b2-Threshold2 b2-Threshold2GERAN : 35
                              },
                            hysteresis 15,
                            timeToTrigger ms80
                          },
                        maxReportCells 4,
                        reportInterval min1,
                        reportAmount r16
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigGERAN 
                  {
                    measQuantityGERAN rssi,
                    filterCoefficient fc5
                  }
                }
              }
            }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_GSM_SRC_MSG \
static const uint8 lte_rrc_meas_test_meas_config_gsm_src_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x40, 0x9C, 0x64, 0x30, 0xAA, 0x06, 0x74, 0x28, 0x0C, \
  0x83, 0x0A, 0x02, 0x46, 0xA3, 0x1B, 0xCD, 0xC4, 0x01, 0x08, 0x41, 0x4A, \
}

/*
g_without_multiple_cgi:value DL-DCCH-Message ::= 
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
                    measObjectId 5,
                    measObject measObjectGERAN : 
                      {
                        carrierFreqs 
                        {
                          startingARFCN 490,
                          bandIndicator dcs1800,
                          followingARFCNs equallySpacedARFCNs : 
                            {
                              arfcn-Spacing 5,
                              numberOfFollowingARFCNs 10
                            }
                        },
                        offsetFreq -5,
                        ncc-Permitted '00000110'B,
                        cellForWhichToReportCGI 
                        {
                          networkColourCode '011'B,
                          baseStationColourCode '101'B
                        }
                      }
                  },
                  {
                    measObjectId 6,
                    measObject measObjectGERAN : 
                      {
                        carrierFreqs 
                        {
                          startingARFCN 540,
                          bandIndicator dcs1800,
                          followingARFCNs variableBitMapOfARFCNs : 'FFFF'H
                        },
                        offsetFreq 10,
                        ncc-Permitted '01110000'B
                      }
                  },
                  {
                    measObjectId 7,
                    measObject measObjectGERAN : 
                      {
                        carrierFreqs 
                        {
                          startingARFCN 507,
                          bandIndicator pcs1900,
                          followingARFCNs variableBitMapOfARFCNs : 'F100'H
                        },
                        offsetFreq 0
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRQ : 20,
                                b2-Threshold2 b2-Threshold2GERAN : 35
                              },
                            hysteresis 15,
                            timeToTrigger ms80
                          },
                        maxReportCells 4,
                        reportInterval min1,
                        reportAmount r16
                      }
                  },
                  {
                    reportConfigId 6,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB1 : 
                              {
                                b1-Threshold b1-ThresholdGERAN : 49
                              },
                            hysteresis 24,
                            timeToTrigger ms100
                          },
                        maxReportCells 7,
                        reportInterval ms2048,
                        reportAmount r1
                      }
                  },
                  {
                    reportConfigId 7,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        maxReportCells 4,
                        reportInterval ms1024,
                        reportAmount r4
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 1,
                    measObjectId 5,
                    reportConfigId 5
                  },
                  {
                    measId 2,
                    measObjectId 5,
                    reportConfigId 6
                  },
                  {
                    measId 3,
                    measObjectId 5,
                    reportConfigId 7
                  },
                  {
                    measId 4,
                    measObjectId 6,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigGERAN 
                  {
                    measQuantityGERAN rssi,
                    filterCoefficient fc8
                  }
                }
              }
            }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_GSM_MSG2 \
static const uint8 lte_rrc_meas_test_meas_config_gsm_msg2[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x88, 0x9D, 0xEA, 0x31, 0x4A, 0x06, 0x74, 0xA9, \
  0xA1, 0xC4, 0x3F, 0xFF, 0xF9, 0x70, 0x32, 0x47, 0xEF, 0x0F, 0x88, 0x03, \
  0xC4, 0x48, 0xD4, 0x63, 0x79, 0xB8, 0x85, 0x83, 0x8E, 0x13, 0x28, 0x35, \
  0x9A, 0x21, 0x80, 0x84, 0x09, 0x0A, 0x22, 0x18, 0x65, 0x20, 0xA8, \
}


/*
G_WO_Meas_id_5:value DL-DCCH-Message ::= 
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
                    measObjectId 5,
                    measObject measObjectGERAN : 
                      {
                        carrierFreqs 
                        {
                          startingARFCN 100,
                          bandIndicator dcs1800,
                          followingARFCNs equallySpacedARFCNs : 
                            {
                              arfcn-Spacing 5,
                              numberOfFollowingARFCNs 30
                            }
                        },
                        offsetFreq -5,
                        ncc-Permitted '00000110'B,
                        cellForWhichToReportCGI 
                        {
                          networkColourCode '011'B,
                          baseStationColourCode '101'B
                        }
                      }
                  },
                  {
                    measObjectId 6,
                    measObject measObjectGERAN : 
                      {
                        carrierFreqs 
                        {
                          startingARFCN 200,
                          bandIndicator dcs1800,
                          followingARFCNs variableBitMapOfARFCNs : '0F0F'H
                        },
                        offsetFreq 10,
                        ncc-Permitted '01110000'B
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRQ : 20,
                                b2-Threshold2 b2-Threshold2GERAN : 35
                              },
                            hysteresis 15,
                            timeToTrigger ms80
                          },
                        maxReportCells 4,
                        reportInterval min1,
                        reportAmount r16
                      }
                  },
                  {
                    reportConfigId 6,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB1 : 
                              {
                                b1-Threshold b1-ThresholdGERAN : 49
                              },
                            hysteresis 24,
                            timeToTrigger ms100
                          },
                        maxReportCells 7,
                        reportInterval ms2048,
                        reportAmount r1
                      }
                  },
                  {
                    reportConfigId 7,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        maxReportCells 4,
                        reportInterval ms1024,
                        reportAmount r4
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 1,
                    measObjectId 5,
                    reportConfigId 5
                  },
                  {
                    measId 2,
                    measObjectId 5,
                    reportConfigId 6
                  },
                  {
                    measId 3,
                    measObjectId 5,
                    reportConfigId 7
                  },
                  {
                    measId 4,
                    measObjectId 6,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigGERAN 
                  {
                    measQuantityGERAN rssi,
                    filterCoefficient fc8
                  }
                }
              }
            }
      }
}

*/


#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_GSM_EXCESS_ARFCN_MSG \
static const uint8 lte_rrc_meas_test_meas_config_gsm_excess_arfcn_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x48, 0x9C, 0x64, 0x33, 0xCA, 0x06, 0x74, 0xA9, 0x8C, \
  0x84, 0x21, 0xE1, 0xF9, 0x70, 0x11, 0x23, 0x51, 0x8D, 0xE6, 0xE2, 0x16, 0x0E, \
  0x38, 0x4C, 0xA0, 0xD6, 0x68, 0x86, 0x02, 0x10, 0x24, 0x28, 0x88, 0x61, 0x94, \
  0x82, 0xA0, \
}


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
                    measObjectId 5,
                    measObject measObjectCDMA2000 : 
                      {
                        cdma2000-Type typeHRPD,
                        carrierFreq 
                        {
                          bandClass bc4,
                          arfcn 300
                        },
                        searchWindowSize 8,
                        offsetFreq -3,
                        cellsToRemoveList 
                        {
                          2
                        },
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 15
                          },
                          {
                            cellIndex 2,
                            physCellId 25
                          },
                          {
                            cellIndex 3,
                            physCellId 35
                          },
                          {
                            cellIndex 4,
                            physCellId 45
                          },
                          {
                            cellIndex 5,
                            physCellId 55
                          }
                        },
                        cellForWhichToReportCGI 65
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRP : 30,
                                b2-Threshold2 b2-Threshold2CDMA2000 : 50
                              },
                            hysteresis 20,
                            timeToTrigger ms100
                          },
                        maxReportCells 2,
                        reportInterval min30,
                        reportAmount r32
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigCDMA2000 
                  {
                    measQuantityCDMA2000 pilotPnPhaseAndPilotStrength
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_CDMA_MSG \
static const uint8 lte_rrc_meas_test_meas_config_cdma_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0xDF, 0x88, 0x4B, 0x21, 0x80, 0x09, 0x00, 0x0F, \
  0x08, 0x64, 0x42, 0x31, 0x8B, 0x48, 0x37, 0x20, 0x80, 0x91, 0x1E, 0xB2, 0xA2, \
  0x1B, 0xA0, 0x21, 0x08, 0x18 \
}

/*
value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 0,
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
                        carrierFreq 40150,
                        allowedMeasBandwidth mbw50,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '01'B
                      }
                  },
                  {
                    measObjectId 2,
                    measObject measObjectCDMA2000 : 
                      {
                        cdma2000-Type typeHRPD,
                        carrierFreq 
                        {
                          bandClass bc0,
                          arfcn 500
                        },
                        offsetFreq 0
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 1,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportStrongestCells
                          },
                        triggerQuantity rsrp,
                        reportQuantity sameAsTriggerQuantity,
                        maxReportCells 1,
                        reportInterval ms5120,
                        reportAmount infinity
                      }
                  },
                  {
                    reportConfigId 3,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportStrongestCellsForSON
                          },
                        maxReportCells 1,
                        reportInterval ms120,
                        reportAmount r1
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 2,
                    measObjectId 2,
                    reportConfigId 3
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc4
                  },
                  quantityConfigCDMA2000 
                  {
                    measQuantityCDMA2000 pilotStrength
                  }
                },
                measGapConfig setup : 
                  {
                    gapOffset gp1 : 0
                  }
              },
              radioResourceConfigDedicated 
              {
                mac-MainConfig explicitValue : 
                  {
                    timeAlignmentTimerDedicated infinity
                  }
              }
            }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_CDMA_SON_MSG \
static const uint8 lte_rrc_meas_test_meas_config_cdma_son_msg[] = \
{ \
  0x20, 0x12, 0x15, 0xC0, 0x40, 0x00, 0x4E, 0x6B, 0x32, 0x16, 0x44, 0x03, 0xE8, \
  0xF0, 0x80, 0x80, 0xDC, 0x55, 0x00, 0x00, 0x10, 0x89, 0x31, 0x14, 0x00, 0x40, \
  0x70 \
}


/*
CDMA_src:value DL-DCCH-Message ::= 
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
                    measObject measObjectCDMA2000 : 
                      {
                        cdma2000-Type typeHRPD,
                        carrierFreq 
                        {
                          bandClass bc4,
                          arfcn 300
                        },
                        searchWindowSize 8,
                        offsetFreq -3,
                        cellsToRemoveList 
                        {
                          2
                        },
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 15
                          },
                          {
                            cellIndex 2,
                            physCellId 25
                          },
                          {
                            cellIndex 3,
                            physCellId 35
                          },
                          {
                            cellIndex 4,
                            physCellId 45
                          },
                          {
                            cellIndex 5,
                            physCellId 55
                          }
                        },
                        cellForWhichToReportCGI 65
                      }
                  },
                  {
                    measObjectId 2,
                    measObject measObjectCDMA2000 : 
                      {
                        cdma2000-Type typeHRPD,
                        carrierFreq 
                        {
                          bandClass bc3,
                          arfcn 400
                        },
                        searchWindowSize 8,
                        offsetFreq -3,
                        cellsToRemoveList 
                        {
                          2
                        },
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 15
                          }
                        },
                        cellForWhichToReportCGI 65
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRP : 30,
                                b2-Threshold2 b2-Threshold2CDMA2000 : 50
                              },
                            hysteresis 20,
                            timeToTrigger ms100
                          },
                        maxReportCells 2,
                        reportInterval min30,
                        reportAmount r32
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigCDMA2000 
                  {
                    measQuantityCDMA2000 pilotPnPhaseAndPilotStrength
                  }
                }
              }
            }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_CDMA_SRC_MSG \
static const uint8 lte_rrc_meas_test_meas_config_cdma_src_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x40, 0xDF, 0x88, 0x4B, 0x21, 0x80, 0x09, 0x00, 0x0F, \
  0x08, 0x64, 0x42, 0x31, 0x8B, 0x48, 0x37, 0x20, 0x85, 0xBF, 0x0C, 0xC8, 0x43, \
  0x00, 0x10, 0x00, 0x1E, 0x41, 0x01, 0x22, 0x3D, 0x65, 0x44, 0x37, 0x40, 0x42, \
  0x10, 0x30 \
}

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
                    measObjectId 5,
                    measObject measObjectCDMA2000 : 
                      {
                        cdma2000-Type typeHRPD,
                        carrierFreq 
                        {
                          bandClass bc4,
                          arfcn 300
                        },
                        searchWindowSize 8,
                        offsetFreq -3,
                        cellsToRemoveList 
                        {
                          2
                        },
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 15
                          },
                          {
                            cellIndex 2,
                            physCellId 25
                          },
                          {
                            cellIndex 3,
                            physCellId 35
                          },
                          {
                            cellIndex 4,
                            physCellId 45
                          },
                          {
                            cellIndex 5,
                            physCellId 55
                          }
                        },
                        cellForWhichToReportCGI 65
                      }
                  },
                  {
                    measObjectId 6,
                    measObject measObjectCDMA2000 : 
                      {
                        cdma2000-Type type1XRTT,
                        carrierFreq 
                        {
                          bandClass bc10,
                          arfcn 300
                        },
                        offsetFreq -12,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100
                          },
                          {
                            cellIndex 3,
                            physCellId 120
                          },
                          {
                            cellIndex 5,
                            physCellId 140
                          }
                        }
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRP : 30,
                                b2-Threshold2 b2-Threshold2CDMA2000 : 50
                              },
                            hysteresis 20,
                            timeToTrigger ms100
                          },
                        maxReportCells 2,
                        reportInterval min30,
                        reportAmount r32
                      }
                  },
                  {
                    reportConfigId 6,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB1 : 
                              {
                                b1-Threshold b1-ThresholdCDMA2000 : 55
                              },
                            hysteresis 22,
                            timeToTrigger ms5120
                          },
                        maxReportCells 4,
                        reportInterval ms1024,
                        reportAmount r8
                      }
                  },
                  {
                    reportConfigId 7,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportStrongestCells
                          },
                        maxReportCells 8,
                        reportInterval ms10240,
                        reportAmount r64
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  },
                  {
                    measId 1,
                    measObjectId 5,
                    reportConfigId 6
                  },
                  {
                    measId 2,
                    measObjectId 5,
                    reportConfigId 7
                  },
                  {
                    measId 3,
                    measObjectId 6,
                    reportConfigId 6
                  },
                  {
                    measId 4,
                    measObjectId 6,
                    reportConfigId 7
                  }
                },
                quantityConfig 
                {
                  quantityConfigCDMA2000 
                  {
                    measQuantityCDMA2000 pilotPnPhaseAndPilotStrength
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_CDMA_MSG2 \
static const uint8 lte_rrc_meas_test_meas_config_cdma_msg2[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x48, 0xDF, 0x88, 0x4B, 0x21, 0x80, 0x09, 0x00, 0x0F, \
  0x08, 0x64, 0x42, 0x31, 0x8B, 0x48, 0x37, 0x20, 0x95, 0x94, 0x28, 0x96, 0x0C, \
  0x40, 0x32, 0x08, 0xF0, 0x44, 0x60, 0x89, 0x11, 0xEB, 0x2A, 0x21, 0xBA, 0x58, \
  0x5B, 0xDB, 0xDA, 0x33, 0x53, 0xBE, 0x21, 0x08, 0x40, 0x10, 0xA1, 0x21, 0x84, \
  0x52, 0x8C, 0xA6, 0x0C,  \
}

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
                    measObjectId 6,
                    measObject measObjectCDMA2000 : 
                      {
                        cdma2000-Type type1XRTT,
                        carrierFreq 
                        {
                          bandClass bc4,
                          arfcn 300
                        },
                        searchWindowSize 8,
                        offsetFreq -3,
                        cellsToRemoveList 
                        {
                          2
                        },
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 15
                          },
                          {
                            cellIndex 2,
                            physCellId 25
                          },
                          {
                            cellIndex 3,
                            physCellId 35
                          },
                          {
                            cellIndex 4,
                            physCellId 45
                          },
                          {
                            cellIndex 5,
                            physCellId 55
                          }
                        },
                        cellForWhichToReportCGI 65
                      }
                  },
                  {
                    measObjectId 7,
                    measObject measObjectCDMA2000 : 
                      {
                        cdma2000-Type typeHRPD,
                        carrierFreq 
                        {
                          bandClass bc4,
                          arfcn 300
                        },
                        offsetFreq -12,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100
                          },
                          {
                            cellIndex 3,
                            physCellId 120
                          },
                          {
                            cellIndex 5,
                            physCellId 140
                          }
                        }
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRP : 30,
                                b2-Threshold2 b2-Threshold2CDMA2000 : 50
                              },
                            hysteresis 20,
                            timeToTrigger ms100
                          },
                        maxReportCells 2,
                        reportInterval min30,
                        reportAmount r32
                      }
                  },
                  {
                    reportConfigId 6,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB1 : 
                              {
                                b1-Threshold b1-ThresholdCDMA2000 : 55
                              },
                            hysteresis 22,
                            timeToTrigger ms5120
                          },
                        maxReportCells 4,
                        reportInterval ms1024,
                        reportAmount r8
                      }
                  },
                  {
                    reportConfigId 7,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportStrongestCells
                          },
                        maxReportCells 8,
                        reportInterval ms10240,
                        reportAmount r64
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 6,
                    reportConfigId 5
                  },
                  {
                    measId 1,
                    measObjectId 6,
                    reportConfigId 6
                  },
                  {
                    measId 2,
                    measObjectId 6,
                    reportConfigId 7
                  },
                  {
                    measId 3,
                    measObjectId 7,
                    reportConfigId 6
                  },
                  {
                    measId 4,
                    measObjectId 7,
                    reportConfigId 7
                  }
                },
                quantityConfig 
                {
                  quantityConfigCDMA2000 
                  {
                    measQuantityCDMA2000 pilotPnPhaseAndPilotStrength
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_MULTIPLE_MEAS_OBJ_FOR_SAME_BC_CHANNEL_MSG \
static const uint8 lte_rrc_meas_test_multiple_meas_obj_for_same_bc_channel_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x4A, 0xDF, 0x08, 0x4B, 0x21, 0x80, 0x09, 0x00, 0x0F, \
  0x08, 0x64, 0x42, 0x31, 0x8B, 0x48, 0x37, 0x20, 0x99, 0x95, 0x10, 0x96, 0x0C, \
  0x40, 0x32, 0x08, 0xF0, 0x44, 0x60, 0x89, 0x11, 0xEB, 0x2A, 0x21, 0xBA, 0x58, \
  0x5B, 0xDB, 0xDA, 0x33, 0x53, 0xBE, 0x21, 0x0A, 0x40, 0x14, 0xA1, 0x29, 0x84, \
  0x62, 0x8C, 0xC6, 0x0C,  \
}

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
                    measObjectId 5,
                    measObject measObjectGERAN : 
                      {
                        carrierFreqs 
                        {
                          startingARFCN 100,
                          bandIndicator dcs1800,
                          followingARFCNs equallySpacedARFCNs : 
                            {
                              arfcn-Spacing 5,
                              numberOfFollowingARFCNs 5
                            }
                        },
                        offsetFreq -5,
                        ncc-Permitted '00000110'B,
                        cellForWhichToReportCGI 
                        {
                          networkColourCode '011'B,
                          baseStationColourCode '101'B
                        }
                      }
                  },
                  {
                    measObjectId 6,
                    measObject measObjectGERAN : 
                      {
                        carrierFreqs 
                        {
                          startingARFCN 200,
                          bandIndicator dcs1800,
                          followingARFCNs variableBitMapOfARFCNs : '0F0F'H
                        },
                        offsetFreq 10,
                        ncc-Permitted '01110000'B
                      }
                  },
                  {
                    measObjectId 1,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 256,
                        allowedMeasBandwidth mbw50,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        offsetFreq dB24,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100,
                            cellIndividualOffset dB24
                          },
                          {
                            cellIndex 2,
                            physCellId 150,
                            cellIndividualOffset dB-16
                          },
                          {
                            cellIndex 3,
                            physCellId 200,
                            cellIndividualOffset dB-24
                          }
                        },
                        blackCellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellIdRange 
                            {
                              start 250,
                              range n12
                            }
                          }
                        },
                        cellForWhichToReportCGI 300
                      }
                  },
                  {
                    measObjectId 2,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 5730,
                        allowedMeasBandwidth mbw100,
                        presenceAntennaPort1 FALSE,
                        neighCellConfig '00'B,
                        offsetFreq dB-20,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 350,
                            cellIndividualOffset dB-8
                          },
                          {
                            cellIndex 2,
                            physCellId 400,
                            cellIndividualOffset dB-2
                          }
                        },
                        blackCellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellIdRange 
                            {
                              start 320
                            }
                          }
                        }
                      }
                  },
                  {
                    measObjectId 3,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 7000,
                        offsetFreq 8,
                        cellsToAddModList cellsToAddModListUTRA-FDD : 
                          {
                            {
                              cellIndex 1,
                              physCellId 100
                            },
                            {
                              cellIndex 2,
                              physCellId 150
                            }
                          },
                        cellForWhichToReportCGI utra-FDD : 120
                      }
                  },
                  {
                    measObjectId 4,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 200,
                        offsetFreq 3,
                        cellsToAddModList cellsToAddModListUTRA-TDD : 
                          {
                            {
                              cellIndex 2,
                              physCellId 70
                            },
                            {
                              cellIndex 3,
                              physCellId 75
                            }
                          },
                        cellForWhichToReportCGI utra-TDD : 80
                      }
                  },
                  {
                    measObjectId 7,
                    measObject measObjectCDMA2000 : 
                      {
                        cdma2000-Type type1XRTT,
                        carrierFreq 
                        {
                          bandClass bc5,
                          arfcn 2000
                        },
                        searchWindowSize 11,
                        offsetFreq -4,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 300
                          },
                          {
                            cellIndex 3,
                            physCellId 320
                          },
                          {
                            cellIndex 4,
                            physCellId 340
                          }
                        },
                        cellForWhichToReportCGI 350
                      }
                  },
                  {
                    measObjectId 8,
                    measObject measObjectCDMA2000 : 
                      {
                        cdma2000-Type typeHRPD,
                        carrierFreq 
                        {
                          bandClass bc9,
                          arfcn 400
                        },
                        searchWindowSize 7,
                        offsetFreq -7,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 4,
                            physCellId 400
                          },
                          {
                            cellIndex 5,
                            physCellId 450
                          }
                        },
                        cellForWhichToReportCGI 475
                      }
                  },
                  {
                    measObjectId 9,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 500,
                        offsetFreq 5,
                        cellsToAddModList cellsToAddModListUTRA-FDD : 
                          {
                            {
                              cellIndex 1,
                              physCellId 45
                            }
                          }
                      }
                  },
                  {
                    measObjectId 10,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 600,
                        offsetFreq 6,
                        cellsToAddModList cellsToAddModListUTRA-FDD : 
                          {
                            {
                              cellIndex 1,
                              physCellId 52
                            }
                          }
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRQ : 20,
                                b2-Threshold2 b2-Threshold2GERAN : 35
                              },
                            hysteresis 15,
                            timeToTrigger ms80
                          },
                        maxReportCells 4,
                        reportInterval min1,
                        reportAmount r16
                      }
                  },
                  {
                    reportConfigId 6,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB1 : 
                              {
                                b1-Threshold b1-ThresholdGERAN : 49
                              },
                            hysteresis 24,
                            timeToTrigger ms100
                          },
                        maxReportCells 7,
                        reportInterval ms2048,
                        reportAmount r1
                      }
                  },
                  {
                    reportConfigId 7,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        maxReportCells 4,
                        reportInterval ms1024,
                        reportAmount r4
                      }
                  },
                  {
                    reportConfigId 1,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType event : 
                          {
                            eventId eventA1 : 
                              {
                                a1-Threshold threshold-RSRP : 90
                              },
                            hysteresis 0,
                            timeToTrigger ms64
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 6,
                        reportInterval ms120,
                        reportAmount infinity
                      }
                  },
                  {
                    reportConfigId 2,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType event : 
                          {
                            eventId eventA3 : 
                              {
                                a3-Offset -20,
                                reportOnLeave TRUE
                              },
                            hysteresis 10,
                            timeToTrigger ms1024
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 4,
                        reportInterval ms480,
                        reportAmount infinity
                      }
                  },
                  {
                    reportConfigId 3,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB1 : 
                              {
                                b1-Threshold b1-ThresholdUTRA : utra-RSCP : 91
                              },
                            hysteresis 10,
                            timeToTrigger ms5120
                          },
                        maxReportCells 5,
                        reportInterval min1,
                        reportAmount r8
                      }
                  },
                  {
                    reportConfigId 4,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB2 : 
                              {
                                b2-Threshold1 threshold-RSRQ : 34,
                                b2-Threshold2 b2-Threshold2CDMA2000 : 63
                              },
                            hysteresis 18,
                            timeToTrigger ms2560
                          },
                        maxReportCells 2,
                        reportInterval min1,
                        reportAmount r4
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 1,
                    measObjectId 1,
                    reportConfigId 1
                  },
                  {
                    measId 2,
                    measObjectId 1,
                    reportConfigId 2
                  },
                  {
                    measId 3,
                    measObjectId 3,
                    reportConfigId 3
                  },
                  {
                    measId 4,
                    measObjectId 4,
                    reportConfigId 3
                  },
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  },
                  {
                    measId 6,
                    measObjectId 6,
                    reportConfigId 6
                  },
                  {
                    measId 7,
                    measObjectId 7,
                    reportConfigId 4
                  },
                  {
                    measId 8,
                    measObjectId 8,
                    reportConfigId 7
                  },
                  {
                    measId 9,
                    measObjectId 9,
                    reportConfigId 3
                  },
                  {
                    measId 10,
                    measObjectId 10,
                    reportConfigId 3
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc19,
                    filterCoefficientRSRQ spare1
                  },
                  quantityConfigUTRA 
                  {
                    measQuantityUTRA-FDD cpich-RSCP,
                    measQuantityUTRA-TDD pccpch-RSCP
                  },
                  quantityConfigGERAN 
                  {
                    measQuantityGERAN rssi,
                    filterCoefficient fc8
                  },
                  quantityConfigCDMA2000 
                  {
                    measQuantityCDMA2000 pilotStrength
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_ALL_RAT_MSG \
static const uint8 lte_rrc_meas_test_meas_config_all_rat_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x82, 0x48, 0x9C, 0x64, 0x30, 0xAA, 0x06, 0x74, 0xA9, 0x8C, \
  0x84, 0x21, 0xE1, 0xF9, 0x70, 0x00, 0x56, 0x02, 0x00, 0xE7, 0x84, 0x03, 0x27, \
  0x82, 0x96, 0x20, 0x99, 0x00, 0x00, 0x2F, 0xA2, 0x96, 0x04, 0x2A, 0x16, 0x62, \
  0xA0, 0x41, 0x05, 0x79, 0x01, 0xC8, 0x34, 0x00, 0x50, 0x04, 0x56, 0xDA, 0xC5, \
  0xC1, 0x01, 0x90, 0x29, 0x61, 0xE0, 0x65, 0x60, 0x64, 0x4A, 0x10, 0xC6, 0x14, \
  0xBD, 0x03, 0x36, 0xC2, 0xFD, 0x0B, 0x58, 0x81, 0x2C, 0x15, 0x00, 0x75, 0x4A, \
  0xF1, 0xDB, 0x72, 0x4C, 0x83, 0xA0, 0x23, 0xC8, 0x13, 0x85, 0xDB, 0x41, 0x50, \
  0x3E, 0x94, 0x00, 0x02, 0xD4, 0x95, 0x04, 0xB1, 0x50, 0x00, 0x34, 0x31, 0x23, \
  0x51, 0x8D, 0xE6, 0xE2, 0x16, 0x0E, 0x38, 0x4C, 0xA0, 0xD6, 0x68, 0x80, 0x01, \
  0x68, 0x04, 0xD0, 0xE1, 0x04, 0x55, 0x58, 0xB2, 0xE2, 0x80, 0xC0, 0xAF, 0x90, \
  0xC7, 0x1C, 0x57, 0xF2, 0xE3, 0x09, 0x20, 0x00, 0x02, 0x00, 0x88, 0x42, 0x18, \
  0xC4, 0x42, 0x10, 0xA5, 0x29, 0x8C, 0x33, 0x9C, 0xC8, 0x40, 0x92, 0x91, 0x3F, \
  0x73, 0xCA, 0x00, \
}

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
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 100,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        offsetFreq dB-20,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100,
                            cellIndividualOffset dB-20
                          },
                          {
                            cellIndex 2,
                            physCellId 200,
                            cellIndividualOffset dB-24
                          }
                        },
                        cellForWhichToReportCGI 150
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 1,
                        reportInterval ms480,
                        reportAmount r1
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc2,
                    filterCoefficientRSRQ fc2
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_MSG \
static const uint8 lte_rrc_meas_test_eutra_cgi_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x14, 0x80, 0x80, 0x28, 0x20, 0x80, 0xC8, 0x20, \
  0xB2, 0x00, 0x96, 0x01, 0x0D, 0x04, 0x00, 0x42, 0x11, 0x18, 0x84, \
}

/*
cgi:value DL-DCCH-Message ::= 
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
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 256,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        offsetFreq dB-20,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100,
                            cellIndividualOffset dB-20
                          },
                          {
                            cellIndex 2,
                            physCellId 200,
                            cellIndividualOffset dB-24
                          }
                        },
                        cellForWhichToReportCGI 150
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 1,
                        reportInterval ms480,
                        reportAmount r1,
                        includeLocationInfo-r10 true
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc2,
                    filterCoefficientRSRQ fc2
                  }
                }
              },
              nonCriticalExtension 
              {
                nonCriticalExtension 
                {
                  otherConfig-r9 
                  {
                    obtainLocationConfig-r11 
                    {
                      obtainLocation-r11 setup
                    }
                  }
                }
              }
            }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_LOC_INFO_MSG \
static const uint8 lte_rrc_meas_test_eutra_cgi_loc_info_msg[] = \
{ \
0x22, 0x10, 0x95, 0x80, 0x08, 0x14, 0x80, 0x80, 0x28, 0x20, 0x80, 0xC8, 0x20, \
0xB2, 0x00, 0x96, 0x01, 0x1D, 0x04, 0x00, 0xA0, 0x30, 0x00, 0x21, 0x08, 0x8C, \
0x42, 0x64, 0x02, 0x02, 0x60 \
}

/*
CGI_ON_B29:value DL-DCCH-Message ::= 
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
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 9710,
                        allowedMeasBandwidth mbw50,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        offsetFreq dB-20,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100,
                            cellIndividualOffset dB-20
                          },
                          {
                            cellIndex 2,
                            physCellId 200,
                            cellIndividualOffset dB-24
                          }
                        },
                        cellForWhichToReportCGI 150
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 1,
                        reportInterval ms480,
                        reportAmount r1
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc2,
                    filterCoefficientRSRQ fc2
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_MSG_ON_B29 \
static const uint8 lte_rrc_meas_test_eutra_cgi_msg_on_b29[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x14, 0x92, 0xF7, 0x38, 0x20, 0x80, 0xC8, 0x20, 0xB2, 0x00, 0x96, 0x01, 0x0D, 0x04, 0x00, 0x42, 0x11, 0x18, 0x84 \
}


/*
CGI_msg:value DL-DCCH-Message ::= 
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
                        carrierFreq 256,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B
                      }
                  },
                  {
                    measObjectId 2,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 280,
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
                            eventId eventA3 : 
                              {
                                a3-Offset 2,
                                reportOnLeave FALSE
                              },
                            hysteresis 2,
                            timeToTrigger ms128
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 2,
                        reportInterval ms1024,
                        reportAmount r2
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 1,
                    measObjectId 1,
                    reportConfigId 1
                  },
                  {
                    measId 2,
                    measObjectId 2,
                    reportConfigId 1
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc2,
                    filterCoefficientRSRQ fc2
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_RPT_CFG_UNMODIFIED \
static const uint8 lte_rrc_meas_test_eutra_rpt_cfg_unmodified[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x40, 0x00, 0x00, 0x80, 0x28, 0x10, 0x00, 0x04, 0x61, \
    0x40, 0x00, 0x14, 0x01, 0x2A, 0x50, 0x84, 0x00, 0x00, 0x42, 0x04, 0x62, 0x10, \
}


/*
CGI_msg:value DL-DCCH-Message ::= 
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
                        carrierFreq 256,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        cellForWhichToReportCGI 116
                      }
                  },
                  {
                    measObjectId 2,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 280,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        cellForWhichToReportCGI 116
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 1,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 2,
                        reportInterval ms1024,
                        reportAmount r2,
                        si-RequestForHO-r9 setup
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 1,
                    measObjectId 2,
                    reportConfigId 1
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc2,
                    filterCoefficientRSRQ fc2
                  }
                }
              }
            }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_RPT_CFG_MODIFIED_TO_CGI \
static const uint8 lte_rrc_meas_test_eutra_rpt_cfg_modified_to_cgi[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x40, 0x00, 0x80, 0x80, 0x28, 0x74, 0x08, 0x02, 0x02, \
    0x30, 0xA1, 0xD0, 0x00, 0x74, 0xA1, 0x03, 0x00, 0xC0, 0x00, 0x01, 0x02, 0x31, 0x08, \
}

/*
CGI_WO_cells_to_rpt:value DL-DCCH-Message ::= 
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
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 256,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        offsetFreq dB-20,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100,
                            cellIndividualOffset dB-20
                          },
                          {
                            cellIndex 2,
                            physCellId 200,
                            cellIndividualOffset dB-24
                          }
                        }
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 1,
                        reportInterval ms480,
                        reportAmount r1
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc2,
                    filterCoefficientRSRQ fc2
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_MSG_WO_CGI_CELL \
static const uint8 lte_rrc_meas_test_eutra_cgi_msg_without_cgi_cell[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x14, 0x00, 0x80, 0x28, 0x20, 0x80, 0xC8, 0x20, \
  0xB2, 0x00, 0x02, 0x1A, 0x08, 0x00, 0x84, 0x22, 0x31, 0x08, \
}


/*
Empty_reconfig_msg:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
            }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_EMPTY_RECFG_MSG \
static const uint8 lte_rrc_meas_test_empty_reconfig_msg[] = \
{ \
  0x22, 0x00, 0x00\
}


/*
cgi_report:value DL-DCCH-Message ::= 
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
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 256,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        offsetFreq dB-20,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100,
                            cellIndividualOffset dB-20
                          },
                          {
                            cellIndex 2,
                            physCellId 200,
                            cellIndividualOffset dB-24
                          }
                        },
                        cellForWhichToReportCGI 150
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 1,
                        reportInterval ms480,
                        reportAmount r1,
                        si-RequestForHO-r9 setup
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc2,
                    filterCoefficientRSRQ fc2
                  }
                }
              }
            }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_WITH_SIHOREQ_MSG \
static const uint8 lte_rrc_meas_test_eutra_cgi_with_sihoreq_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x14, 0x80, 0x80, 0x28, 0x20, 0x80, 0xC8, 0x20, \
  0xB2, 0x00, 0x96, 0x01, 0x1D, 0x04, 0x00, 0xC0, 0x30, 0x00, 0x21, 0x08, 0x8C, 0x42, \
}

/*
eutra_meas_config:value DL-DCCH-Message ::= 
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
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 256,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        offsetFreq dB-20,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100,
                            cellIndividualOffset dB-20
                          },
                          {
                            cellIndex 2,
                            physCellId 200,
                            cellIndividualOffset dB-24
                          }
                        },
                        cellForWhichToReportCGI 150
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType event : 
                          {
                            eventId eventA3 : 
                              {
                                a3-Offset 10,
                                reportOnLeave FALSE
                              },
                            hysteresis 0,
                            timeToTrigger ms0
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 1,
                        reportInterval ms480,
                        reportAmount r1
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc2,
                    filterCoefficientRSRQ fc2
                  }
                }
              }
            }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_RECONFIG_MSG1 \
static const uint8 lte_rrc_meas_test_eutra_reconfig_msg1[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x14, 0x80, 0x80, 0x28, 0x20, 0x80, 0xC8, 0x20, \
  0xB2, 0x00, 0x96, 0x01, 0x01, 0x50, 0x00, 0x20, 0x80, 0x08, 0x42, 0x23, 0x10, 0x80 \
}


/*
eutra_reconfig_msg2:value DL-DCCH-Message ::= 
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
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 256,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        offsetFreq dB-20,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100,
                            cellIndividualOffset dB-20
                          },
                          {
                            cellIndex 2,
                            physCellId 200,
                            cellIndividualOffset dB-24
                          }
                        },
                        cellForWhichToReportCGI 150
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 1,
                        reportInterval ms480,
                        reportAmount r1
                      }
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc2,
                    filterCoefficientRSRQ fc2
                  }
                }
              }
            }
      }
}


*/

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_RECONFIG_MSG2 \
static const uint8 lte_rrc_meas_test_eutra_reconfig_msg2[] = \
{ \
  0x22, 0x10, 0x14, 0x80, 0x08, 0x14, 0x80, 0x80, 0x28, 0x20, 0x80, 0xC8, 0x20, \
  0xB2, 0x00, 0x96, 0x01, 0x0D, 0x04, 0x11, 0x88, 0x40 \
}


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
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 256,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        offsetFreq dB-20,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100,
                            cellIndividualOffset dB-20
                          },
                          {
                            cellIndex 2,
                            physCellId 200,
                            cellIndividualOffset dB-24
                          }
                        },
                        cellForWhichToReportCGI 16
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 1,
                        reportInterval ms480,
                        reportAmount r1
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc2,
                    filterCoefficientRSRQ fc2
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_SERVING_CGI_MSG \
static const uint8 lte_rrc_meas_test_eutra_serving_cgi_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x14, 0x80, 0x80, 0x28, 0x20, 0x80, 0xC8, 0x20, \
  0xB2, 0x00, 0x10, 0x01, 0x0D, 0x04, 0x00, 0x42, 0x11, 0x18, 0x84, \
}

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
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 256,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        offsetFreq dB-20,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100,
                            cellIndividualOffset dB-20
                          },
                          {
                            cellIndex 2,
                            physCellId 200,
                            cellIndividualOffset dB-24
                          }
                        }
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportStrongestCells
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 1,
                        reportInterval ms480,
                        reportAmount r1
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc2,
                    filterCoefficientRSRQ fc2
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_PERIODICAL_MEAS_MSG \
static const uint8 lte_rrc_meas_test_eutra_periodical_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x14, 0x00, 0x80, 0x28, 0x20, 0x80, 0xC8, \
  0x20, 0xB2, 0x00, 0x02, 0x12, 0x08, 0x00, 0x84, 0x22, 0x31, 0x08, \
}

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
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 256,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        offsetFreq dB-20,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100,
                            cellIndividualOffset dB-20
                          },
                          {
                            cellIndex 2,
                            physCellId 200,
                            cellIndividualOffset dB-24
                          }
                        }
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportStrongestCells
                          },
                        maxReportCells 1,
                        reportInterval ms240,
                        reportAmount r4
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc2,
                    filterCoefficientRSRQ fc2
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_IRAT_PERIODICAL_MEAS_MSG \
static const uint8 lte_rrc_meas_test_irat_periodical_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x14, 0x00, 0x80, 0x28, 0x20, 0x80, 0xC8, \
  0x20, 0xB2, 0x00, 0x02, 0x50, 0x0A, 0x01, 0x08, 0x44, 0x62, 0x10, \
}

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
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 256,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        offsetFreq dB-20,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100,
                            cellIndividualOffset dB-20
                          },
                          {
                            cellIndex 2,
                            physCellId 200,
                            cellIndividualOffset dB-24
                          }
                        }
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType event : 
                          {
                            eventId eventB1 : 
                              {
                                b1-Threshold b1-ThresholdUTRA : utra-RSCP : 5
                              },
                            hysteresis 0,
                            timeToTrigger ms0
                          },
                        maxReportCells 1,
                        reportInterval ms240,
                        reportAmount r4
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc2,
                    filterCoefficientRSRQ fc2
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_IRAT_EVENT_B1_MEAS_MSG \
static const uint8 lte_rrc_meas_test_irat_event_b1_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x14, 0x00, 0x80, 0x28, 0x20, 0x80, 0xC8, \
  0x20, 0xB2, 0x00, 0x02, 0x40, 0x0A, 0x00, 0x01, 0x40, 0x21, 0x08, 0x8C, \
  0x42, \
}

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
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 256,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        offsetFreq dB-20,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100,
                            cellIndividualOffset dB-20
                          },
                          {
                            cellIndex 2,
                            physCellId 200,
                            cellIndividualOffset dB-24
                          }
                        }
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType event : 
                          {
                            eventId eventA4 : 
                              {
                                a4-Threshold threshold-RSRP : 1
                              },
                            hysteresis 0,
                            timeToTrigger ms0
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 1,
                        reportInterval ms480,
                        reportAmount r1
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc2,
                    filterCoefficientRSRQ fc2
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_EVENT_A4_MEAS_MSG \
static const uint8 lte_rrc_meas_test_eutra_event_a4_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x14, 0x00, 0x80, 0x28, 0x20, 0x80, 0xC8, \
  0x20, 0xB2, 0x00, 0x02, 0x03, 0x01, 0x00, 0x20, 0x80, 0x08, 0x42, 0x23, \
  0x10, 0x80, \
}

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
                    measObjectId 5,
                    measObject measObjectEUTRA : 
                      {
                        carrierFreq 256,
                        allowedMeasBandwidth mbw25,
                        presenceAntennaPort1 TRUE,
                        neighCellConfig '00'B,
                        offsetFreq dB-20,
                        cellsToAddModList 
                        {
                          {
                            cellIndex 1,
                            physCellId 100,
                            cellIndividualOffset dB-20
                          },
                          {
                            cellIndex 2,
                            physCellId 200,
                            cellIndividualOffset dB-24
                          }
                        }
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigEUTRA : 
                      {
                        triggerType event : 
                          {
                            eventId eventA5 : 
                              {
                                a5-Threshold1 threshold-RSRP : 1,
                                a5-Threshold2 threshold-RSRP : 0
                              },
                            hysteresis 0,
                            timeToTrigger ms0
                          },
                        triggerQuantity rsrp,
                        reportQuantity both,
                        maxReportCells 1,
                        reportInterval ms480,
                        reportAmount r1
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigEUTRA 
                  {
                    filterCoefficientRSRP fc2,
                    filterCoefficientRSRQ fc2
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_EVENT_A5_MEAS_MSG \
static const uint8 lte_rrc_meas_test_eutra_event_a5_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x14, 0x00, 0x80, 0x28, 0x20, 0x80, 0xC8, \
  0x20, 0xB2, 0x00, 0x02, 0x04, 0x01, 0x00, 0x00, 0x20, 0x80, 0x08, 0x42, \
  0x23, 0x10, 0x80, \
}

/*
Sib1_with_CSG:value BCCH-DL-SCH-Message ::= 
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
                  3,
                  5,
                  7
                },
                mnc 
                {
                  4,
                  5,
                  6
                }
              },
              cellReservedForOperatorUse notReserved
            },
            {
              plmn-Identity 
              {
                mnc 
                {
                  1,
                  2,
                  3
                }
              },
              cellReservedForOperatorUse reserved
            },
            {
              plmn-Identity 
              {
                mcc 
                {
                  0,
                  9,
                  1
                },
                mnc 
                {
                  3,
                  2,
                  1
                }
              },
              cellReservedForOperatorUse notReserved
            },
            {
              plmn-Identity 
              {
                mnc 
                {
                  5,
                  1,
                  0
                }
              },
              cellReservedForOperatorUse notReserved
            }
          },
          trackingAreaCode '00001111 00001111'B,
          cellIdentity '00001010 00000101 00000100 0000'B,
          cellBarred notBarred,
          intraFreqReselection allowed,
          csg-Indication TRUE,
          csg-Identity '00001010 00001011 00001100 000'B
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

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_SIB1_MSG \
static const uint8 lte_rrc_meas_test_eutra_cgi_sib1_msg[] = \
{ \
  0x75, 0xCD, 0x5E, 0x8A, 0xD4, 0x48, 0xD0, 0x91, 0x99, 0x0D, 0x51, 0x08, 0x78, \
  0x78, 0x50, 0x28, 0x20, 0x50, 0xA0, 0xB0, 0xC1, 0x2B, 0x24, 0x18, 0x08, 0x40, \
  0xD0, 0x88 \
}

/*
BCCH-DL-SCH-Message ::= {
    message: c1: systemInformationBlockType1: SystemInformationBlockType1 ::= {
        cellAccessRelatedInfo: cellAccessRelatedInfo ::= {
            plmn-IdentityList: PLMN-IdentityList ::= {
                PLMN-IdentityInfo ::= {
                    plmn-Identity: PLMN-Identity ::= {
                        mcc: MCC ::= {
                            3
                            5
                            7
                        }
                        mnc: MNC ::= {
                            4
                            5
                            6
                        }
                    }
                    cellReservedForOperatorUse: 1 (notReserved)
                }
                PLMN-IdentityInfo ::= {
                    plmn-Identity: PLMN-Identity ::= {
                        mnc: MNC ::= {
                            1
                            2
                            3
                        }
                    }
                    cellReservedForOperatorUse: 0 (reserved)
                }
                PLMN-IdentityInfo ::= {
                    plmn-Identity: PLMN-Identity ::= {
                        mcc: MCC ::= {
                            0
                            9
                            1
                        }
                        mnc: MNC ::= {
                            3
                            2
                            1
                        }
                    }
                    cellReservedForOperatorUse: 1 (notReserved)
                }
                PLMN-IdentityInfo ::= {
                    plmn-Identity: PLMN-Identity ::= {
                        mnc: MNC ::= {
                            5
                            1
                            0
                        }
                    }
                    cellReservedForOperatorUse: 1 (notReserved)
                }
            }
            trackingAreaCode: 0F 0F
            cellIdentity: 0A 05 04 00
            cellBarred: 0 (barred)
            intraFreqReselection: 0 (allowed)
            csg-Indication: TRUE
            csg-Identity: 0A 0B 0C 00
        }
        cellSelectionInfo: cellSelectionInfo ::= {
            q-RxLevMin: -60
            q-RxLevMinOffset: 7
        }
        p-Max: -12
        freqBandIndicator: 4
        schedulingInfoList: SchedulingInfoList ::= {
            SchedulingInfo ::= {
                si-Periodicity: 1 (rf16)
                sib-MappingInfo: SIB-MappingInfo ::= {
                    0 (sibType3)
                }
            }
        }
        tdd-Config: TDD-Config ::= {
            subframeAssignment: 3 (sa3)
            specialSubframePatterns: 4 (ssp4)
        }
        si-WindowLength: 1 (ms2)
        systemInfoValueTag: 2
    }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_SIB1_BARRED_MSG \
   static const uint8 lte_rrc_meas_test_eutra_cgi_sib1_barred_msg[] = \
   { \
     0x75, 0xCD, 0x5E, 0x8A, 0xD4, 0x48, 0xD0, 0x91, 0x99, 0x0D, 0x51, 0x08, 0x78, \
     0x78, 0x50, 0x28, 0x20, 0x10, 0xA0, 0xB0, 0xC1, 0x2B, 0x24, 0x18, 0x08, 0x40, \
     0xD0, 0x88 \
   }

/*
BCCH-DL-SCH-Message ::= {
    message: c1: systemInformationBlockType1: SystemInformationBlockType1 ::= {
        cellAccessRelatedInfo: cellAccessRelatedInfo ::= {
            plmn-IdentityList: PLMN-IdentityList ::= {
                PLMN-IdentityInfo ::= {
                    plmn-Identity: PLMN-Identity ::= {
                        mcc: MCC ::= {
                            3
                            5
                            7
                        }
                        mnc: MNC ::= {
                            4
                            5
                            6
                        }
                    }
                    cellReservedForOperatorUse: 0 (reserved)
                }
                PLMN-IdentityInfo ::= {
                    plmn-Identity: PLMN-Identity ::= {
                        mnc: MNC ::= {
                            1
                            2
                            3
                        }
                    }
                    cellReservedForOperatorUse: 0 (reserved)
                }
                PLMN-IdentityInfo ::= {
                    plmn-Identity: PLMN-Identity ::= {
                        mcc: MCC ::= {
                            0
                            9
                            1
                        }
                        mnc: MNC ::= {
                            3
                            2
                            1
                        }
                    }
                    cellReservedForOperatorUse: 0 (reserved)
                }
                PLMN-IdentityInfo ::= {
                    plmn-Identity: PLMN-Identity ::= {
                        mnc: MNC ::= {
                            5
                            1
                            0
                        }
                    }
                    cellReservedForOperatorUse: 0 (reserved)
                }
            }
            trackingAreaCode: 0F 0F
            cellIdentity: 0A 05 04 00
            cellBarred: 0 (barred)
            intraFreqReselection: 0 (allowed)
            csg-Indication: TRUE
            csg-Identity: 0A 0B 0C 00
        }
        cellSelectionInfo: cellSelectionInfo ::= {
            q-RxLevMin: -60
            q-RxLevMinOffset: 7
        }
        p-Max: -12
        freqBandIndicator: 4
        schedulingInfoList: SchedulingInfoList ::= {
            SchedulingInfo ::= {
                si-Periodicity: 1 (rf16)
                sib-MappingInfo: SIB-MappingInfo ::= {
                    0 (sibType3)
                }
            }
        }
        tdd-Config: TDD-Config ::= {
            subframeAssignment: 3 (sa3)
            specialSubframePatterns: 4 (ssp4)
        }
        si-WindowLength: 1 (ms2)
        systemInfoValueTag: 2
    }
}
   

*/
#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_SIB1_RESERVED_PLMN_MSG \
static const uint8 lte_rrc_meas_test_eutra_cgi_sib1_reserved_plmn_msg[] = \
{ \
  0x75, 0xCD, 0x5E, 0x8A, 0xC4, 0x48, 0xD0, 0x91, 0x99, 0x09, 0x51, 0x00, 0x78,  \
  0x78, 0x50, 0x28, 0x20, 0x10, 0xA0, 0xB0, 0xC1, 0x2B, 0x24, 0x18, 0x08, 0x40,  \
  0xD0, 0x88 \
}

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
                mcc 
                {
                  3,
                  5,
                  7
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
          trackingAreaCode '00001111 00001111'B,
          cellIdentity '00001010 00000101 00000100 0000'B,
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

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_SIB1_MSG2 \
static const uint8 lte_rrc_meas_test_eutra_cgi_sib1_one_plmn_only_msg[] = \
{ \
  0x70, 0x4D, 0x5E, 0x8A, 0xD0, 0xF0, 0xF0, 0xA0, 0x50, 0x40, 0x92, 0xB2, 0x41, \
  0x80, 0x84, 0x0D, 0x08, 0x80  \
}

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
                mcc 
                {
                  3,
                  5,
                  7
                },
                mnc 
                {
                  4,
                  5,
                  6
                }
              },
              cellReservedForOperatorUse notReserved
            },
            {
              plmn-Identity 
              {
                mnc 
                {
                  1,
                  2,
                  3
                }
              },
              cellReservedForOperatorUse reserved
            },
            {
              plmn-Identity 
              {
                mnc 
                {
                  3,
                  2,
                  1
                }
              },
              cellReservedForOperatorUse notReserved
            },
            {
              plmn-Identity 
              {
                mnc 
                {
                  5,
                  1,
                  0
                }
              },
              cellReservedForOperatorUse notReserved
            },
            {
              plmn-Identity 
              {
                mnc 
                {
                  7,
                  1,
                  4
                }
              },
              cellReservedForOperatorUse reserved
            },
            {
              plmn-Identity 
              {
                mnc 
                {
                  8,
                  5,
                  8
                }
              },
              cellReservedForOperatorUse notReserved
            }
          },
          trackingAreaCode '00001111 00001111'B,
          cellIdentity '00001010 00000101 00000100 0000'B,
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

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_CGI_SIB1_MSG3 \
static const uint8 lte_rrc_meas_test_eutra_cgi_sib1_max_plmn_id_msg[] = \
{ \
  0x72, 0xCD, 0x5E, 0x8A, 0xD4, 0x48, 0xC9, 0x90, 0xD5, 0x10, 0xAE, 0x28, 0x61, \
  0x62, 0x1E, 0x1E, 0x14, 0x0A, 0x08, 0x12, 0x56, 0x48, 0x30, 0x10, 0x81, 0xA1, \
  0x10  \
}

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
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
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

#define DECLARE_LTE_RRC_MEAS_TEST_EUTRA_HO_WITH_CGI \
static const uint8 lte_rrc_meas_test_eutra_ho_with_cgi_msg[] = \
{ \
  0x22, 0x1B, 0x01, 0x00, 0x08, 0x42, 0x3C, 0x15, 0x8C, 0xA0, 0x8D, 0x67, 0x90, \
  0x25, 0x00, 0x80, 0x3F, 0xEC, 0x14, 0x08, 0x12, 0x6A, 0x02, 0x83, 0x10, 0x0F, \
  0x22, 0x01, 0x14, 0x68, 0x16, 0x00, 0x52, 0x2C, 0x1A, 0xD8, 0x01, 0x0A, 0x10, \
  0x01, 0x14, 0x04, 0x90, 0x10,  \
}

/*
SIB3:value BCCH-DL-SCH-Message ::= 
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
                    s-NonIntraSearch 22,
                    threshServingLow 0,
                    cellReselectionPriority 0
                  },
                  intraFreqCellReselectionInfo 
                  {
                    q-RxLevMin -25,
                    s-IntraSearch 20,
                    presenceAntennaPort1 FALSE,
                    neighCellConfig '11'B,
                    t-ReselectionEUTRA 0
                  },
                  s-IntraSearch-v920 
                  {
                    s-IntraSearchP-r9 30,
                    s-IntraSearchQ-r9 25
                  },
                  s-NonIntraSearch-v920 
                  {
                    s-NonIntraSearchP-r9 31,
                    s-NonIntraSearchQ-r9 4
                  },
                  q-QualMin-r9 -3,
                  threshServingLowQ-r9 7
                }
            }
          }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_REL9_RSRQ_FIELDS_PRESENT_SIB3_MSG \
static const uint8 lte_rrc_meas_test_rel9_rsrq_fields_present_sib3_msg[] = \
{ \
  0x00, 0x06, 0x0D, 0x80, 0x12, 0xDA, 0x30, 0x05, 0x05, 0xFF, 0x67, 0xE4, 0xF9, \
  0xC0  \
}

/*
SIB5:value BCCH-DL-SCH-Message ::= 
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
                      dl-CarrierFreq 100,
                      q-RxLevMin -25,
                      t-ReselectionEUTRA 3,
                      threshX-High 25,
                      threshX-Low 10,
                      allowedMeasBandwidth mbw15,
                      presenceAntennaPort1 FALSE,
                      cellReselectionPriority 4,
                      neighCellConfig '11'B,
                      q-OffsetFreq dB-18,
                      q-QualMin-r9 -22,
                      threshX-Q-r9 
                      {
                        threshX-HighQ-r9 29,
                        threshX-LowQ-r9 8
                      }
                    },
                    {
                      dl-CarrierFreq 200,
                      q-RxLevMin -24,
                      t-ReselectionEUTRA 4,
                      threshX-High 27,
                      threshX-Low 12,
                      allowedMeasBandwidth mbw6,
                      presenceAntennaPort1 FALSE,
                      cellReselectionPriority 3,
                      neighCellConfig '11'B
                    },
                    {
                      dl-CarrierFreq 300,
                      q-RxLevMin -28,
                      t-ReselectionEUTRA 3,
                      threshX-High 31,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw6,
                      presenceAntennaPort1 FALSE,
                      cellReselectionPriority 5,
                      neighCellConfig '11'B,
                      threshX-Q-r9 
                      {
                        threshX-HighQ-r9 31,
                        threshX-LowQ-r9 12
                      }
                    }
                  }
                }
            }
          }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_REL9_RSRQ_FIELDS_PRESENT_SIB5_MSG \
static const uint8 lte_rrc_meas_test_rel9_rsrq_fields_present_sib5_msg[] = \
{ \
  0x00, 0x0C, 0xA6, 0x00, 0x32, 0x5A, 0xFE, 0xA0, 0x98, 0xC0, 0x40, 0xF6, 0x75, \
  0x00, 0x24, 0x00, 0x64, 0x5D, 0x32, 0xE0, 0x58, 0x08, 0x0D, 0xD4, 0x80, 0x12, \
  0xCA, 0x98, 0x00, 0x17, 0x01, 0x02, 0x7E, 0xC0  \
}

/*
SIB6:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib6 : 
                {
                  carrierFreqListUTRA-FDD 
                  {
                    {
                      carrierFreq 100,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 21,
                      q-RxLevMin -23,
                      p-MaxUTRA 10,
                      q-QualMin -10,
                      threshX-Q-r9 
                      {
                        threshX-HighQ-r9 31,
                        threshX-LowQ-r9 14
                      }
                    },
                    {
                      carrierFreq 200,
                      cellReselectionPriority 4,
                      threshX-High 24,
                      threshX-Low 5,
                      q-RxLevMin -27,
                      p-MaxUTRA 0,
                      q-QualMin -4
                    }
                  },
                  t-ReselectionUTRA 0
                }
            }
          }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_REL9_RSRQ_FIELDS_PRESENT_SIB6_MSG \
static const uint8 lte_rrc_meas_test_rel9_rsrq_fields_present_sib6_msg[] = \
{ \
  0x00, 0x11, 0x07, 0x01, 0x93, 0x7E, 0xB2, 0xBC, 0x70, 0x08, 0x17, 0xEE, 0x02, \
  0x06, 0x43, 0xD3, 0x68, 0x6B, 0x30  \
}


/*
UTRA_CGI:value DL-DCCH-Message ::= 
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
                    measObjectId 5,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 100,
                        offsetFreq -10,
                        cellsToRemoveList 
                        {
                          1
                        },
                        cellsToAddModList cellsToAddModListUTRA-FDD : 
                          {
                            {
                              cellIndex 1,
                              physCellId 100
                            },
                            {
                              cellIndex 2,
                              physCellId 200
                            },
                            {
                              cellIndex 3,
                              physCellId 300
                            },
                            {
                              cellIndex 4,
                              physCellId 400
                            },
                            {
                              cellIndex 5,
                              physCellId 500
                            }
                          },
                        cellForWhichToReportCGI utra-FDD : 250
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        maxReportCells 1,
                        reportInterval min1,
                        reportAmount r1,
                        reportQuantityUTRA-FDD-r10 both
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigUTRA 
                  {
                    measQuantityUTRA-FDD cpich-EcN0,
                    measQuantityUTRA-TDD pccpch-RSCP,
                    filterCoefficient fc3
                  },
                  quantityConfigUTRA-v1020 
                  {
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_IRAT_TEST_CGI_TO_W_MSG \
static uint8 lte_rrc_irat_test_cgi_to_w_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x5E, 0x03, 0x21, 0x40, 0x01, 0x00, 0x64, 0x0B, \
  0x20, 0x52, 0xC1, 0xE4, 0x09, 0xF4, 0x3E, 0x80, 0x4F, 0x08, 0x00, 0x50, 0x18, \
  0x00, 0x10, 0x84, 0xA6, 0x30, 0x10, 0x18, 0x00, \
}

/*
Onex_Msg:value DL-DCCH-Message ::= 
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
                    measObjectId 5,
                    measObject measObjectCDMA2000 : 
                      {
                        cdma2000-Type type1XRTT,
                        carrierFreq 
                        {
                          bandClass bc0,
                          arfcn 1000
                        },
                        cellForWhichToReportCGI 500
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        maxReportCells 1,
                        reportInterval min1,
                        reportAmount r1
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigCDMA2000 
                  {
                    measQuantityCDMA2000 pilotStrength
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_IRAT_TEST_CGI_TO_1X_MSG \
static uint8 lte_rrc_irat_test_cgi_to_1x_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0xC1, 0x00, 0xFA, 0x3E, 0x80, 0x25, 0x84, 0x00, \
  0x10, 0x84, 0x08 \
}

/*
w_cgi_report:value DL-DCCH-Message ::= 
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
                    measObjectId 5,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 100,
                        offsetFreq -10,
                        cellsToRemoveList 
                        {
                          1
                        },
                        cellsToAddModList cellsToAddModListUTRA-FDD : 
                          {
                            {
                              cellIndex 1,
                              physCellId 100
                            },
                            {
                              cellIndex 2,
                              physCellId 200
                            },
                            {
                              cellIndex 3,
                              physCellId 300
                            },
                            {
                              cellIndex 4,
                              physCellId 400
                            },
                            {
                              cellIndex 5,
                              physCellId 500
                            }
                          },
                        cellForWhichToReportCGI utra-FDD : 250
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        maxReportCells 1,
                        reportInterval min1,
                        reportAmount r1,
                        si-RequestForHO-r9 setup
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigUTRA 
                  {
                    measQuantityUTRA-FDD cpich-EcN0,
                    measQuantityUTRA-TDD pccpch-RSCP,
                    filterCoefficient fc3
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_IRAT_TEST_CGI_WITH_SIHOREQ_TO_W_MSG \
static uint8 lte_rrc_irat_test_cgi_with_sihoreq_to_w_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x5E, 0x03, 0x21, 0x40, 0x01, 0x00, 0x64, 0x0B, \
  0x20, 0x52, 0xC1, 0xE4, 0x09, 0xF4, 0x3E, 0x80, 0x4F, 0x08, 0x00, 0x60, 0x18, \
  0x00, 0x10, 0x84, 0x26, 0x30,  \
}

/*
w_cgi_report:value DL-DCCH-Message ::= 
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
                    measObjectId 5,
                    measObject measObjectUTRA : 
                      {
                        carrierFreq 100,
                        offsetFreq -10,
                        cellsToRemoveList 
                        {
                          1
                        },
                        cellsToAddModList cellsToAddModListUTRA-TDD : 
                          {
                            {
                              cellIndex 1,
                              physCellId 100
                            },
                            {
                              cellIndex 2,
                              physCellId 101
                            },
                            {
                              cellIndex 3,
                              physCellId 103
                            },
                            {
                              cellIndex 4,
                              physCellId 104
                            },
                            {
                              cellIndex 5,
                              physCellId 105
                            }
                          },
                        cellForWhichToReportCGI utra-TDD : 102
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        maxReportCells 1,
                        reportInterval min1,
                        reportAmount r1,
                        si-RequestForHO-r9 setup
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigUTRA 
                  {
                    measQuantityUTRA-FDD cpich-EcN0,
                    measQuantityUTRA-TDD pccpch-RSCP,
                    filterCoefficient fc3
                  }
                }
              }
            }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_IRAT_TEST_CGI_WITH_SIHOREQ_TO_W_TDD_MSG \
static uint8 lte_rrc_irat_test_cgi_with_sihoreq_to_w_tdd_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x5E, 0x03, 0x21, 0x40, 0x09, 0x01, 0x90, 0x39, \
  0x45, 0x9C, 0x7A, 0x09, 0xA7, 0x98, 0x04, 0xF0, 0x80, 0x06, 0x01, 0x80, 0x01, \
  0x08, 0x42, 0x63,  \
}

/*
G_CGI:value DL-DCCH-Message ::= 
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
                    measObjectId 5,
                    measObject measObjectGERAN : 
                      {
                        carrierFreqs 
                        {
                          startingARFCN 100,
                          bandIndicator dcs1800,
                          followingARFCNs equallySpacedARFCNs : 
                            {
                              arfcn-Spacing 5,
                              numberOfFollowingARFCNs 1
                            }
                        },
                        offsetFreq 1,
                        ncc-Permitted '00001111'B,
                        cellForWhichToReportCGI 
                        {
                          networkColourCode '111'B,
                          baseStationColourCode '111'B
                        }
                      }
                  }
                },
                reportConfigToAddModList 
                {
                  {
                    reportConfigId 5,
                    reportConfig reportConfigInterRAT : 
                      {
                        triggerType periodical : 
                          {
                            purpose reportCGI
                          },
                        maxReportCells 1,
                        reportInterval min1,
                        reportAmount r1
                      }
                  }
                },
                measIdToAddModList 
                {
                  {
                    measId 5,
                    measObjectId 5,
                    reportConfigId 5
                  }
                },
                quantityConfig 
                {
                  quantityConfigGERAN 
                  {
                    measQuantityGERAN rssi,
                    filterCoefficient fc2
                  }
                }
              }
            }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_IRAT_TEST_CGI_TO_G_MSG \
static uint8 lte_rrc_irat_test_cgi_to_g_msg[] = \
{ \
  0x22, 0x10, 0x15, 0x80, 0x08, 0x9C, 0x64, 0x30, 0x30, 0x0F, 0xFC, 0x04, 0xB0, \
  0x80, 0x02, 0x10, 0x82, 0x88, \
}


/*
SIBs:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib4 : 
                {
                  intraFreqNeighCellList 
                  {
                    {
                      physCellId 36,
                      q-OffsetCell dB3
                    },
                    {
                      physCellId 38,
                      q-OffsetCell dB3
                    }
                  }
                },
              sib5 : 
                {
                  interFreqCarrierFreqList 
                  {
                    {
                      dl-CarrierFreq 5760,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 5,
                      neighCellConfig '01'B
                    }
                  }
                },
              sib6 : 
                {
                  carrierFreqListUTRA-FDD 
                  {
                    {
                      carrierFreq 4415,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24,
                      q-QualMin -16
                    },
                    {
                      carrierFreq 612,
                      cellReselectionPriority 3,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24,
                      q-QualMin -16
                    }
                  },
                  carrierFreqListUTRA-TDD 
                  {
                    {
                      carrierFreq 10080,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11670,
                      cellReselectionPriority 2,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    }
                  },
                  t-ReselectionUTRA 2,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  }
                },
              sib7 : 
                {
                  t-ReselectionGERAN 2,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 222,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            232,
                            242
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 2,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 555,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 4,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 140,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 2,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 175,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 2,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 666,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 4,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 777,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 4,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    }
                  }
                }
            }
          }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_IDLE_LTE_WCDMA_GSM_MSG \
static const uint8 lte_rrc_meas_test_idle_lte_wcdma_gsm_msg[] = \
{ \
  0x01, 0x89, 0x04, 0x24, 0x90, 0x4D, 0x21, 0x82, 0x81, 0x68, 0x02, 0x1E, 0x4A, \
  0x02, 0xD4, 0x8E, 0x2A, 0x27, 0xFB, 0xE7, 0x1E, 0x52, 0x10, 0x99, 0x1F, 0xCE, \
  0x3C, 0xA4, 0x0B, 0x3B, 0x05, 0xF9, 0xC7, 0x94, 0xDB, 0x2C, 0xBE, 0x71, 0xE5, \
  0x20, 0x2B, 0x40, 0xA3, 0x7A, 0x08, 0xE8, 0x3C, 0xB5, 0xFE, 0x18, 0x1F, 0x02, \
  0x2B, 0x80, 0xE7, 0xF8, 0x00, 0x00, 0x02, 0x32, 0x03, 0x5F, 0xE1, 0x81, 0xF0, \
  0x0A, 0xF8, 0x0D, 0x7F, 0x86, 0x07, 0xC0, 0xA6, 0xA0, 0x39, 0xFE, 0x18, 0x1F, \
  0x03, 0x09, 0x80, 0xE7, 0xF8, 0x60, 0x7C, 0x00,  \
}

/*
Cmcc_msg1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib4 : 
                {
                  intraFreqNeighCellList 
                  {
                    {
                      physCellId 36,
                      q-OffsetCell dB3
                    },
                    {
                      physCellId 38,
                      q-OffsetCell dB3
                    }
                  }
                },
              sib5 : 
                {
                  interFreqCarrierFreqList 
                  {
                    {
                      dl-CarrierFreq 5760,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5780,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5800,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5820,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5840,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5860,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    }
                  }
                },
              sib6 : 
                {
                  carrierFreqListUTRA-TDD 
                  {
                    {
                      carrierFreq 10080,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11670,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11690,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11710,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11730,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11750,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11770,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11790,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11810,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11830,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11850,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11870,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11890,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11910,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11930,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11950,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    }
                  },
                  t-ReselectionUTRA 2,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  }
                },
              sib7 : 
                {
                  t-ReselectionGERAN 2,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 222,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            232,
                            242
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 555,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 140,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 175,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 666,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 777,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 790,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 800,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 810,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 850,
                        bandIndicator dcs1800,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 900,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 950,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 968,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 987,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 683,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 521,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    }
                  }
                }
            }
          }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG1 \
static const uint8 lte_rrc_meas_test_idle_cmcc_msg1[] = \
{ \
  0x01, 0x89, 0x04, 0x24, 0x90, 0x4D, 0x21, 0xAA, 0x81, 0x68, 0x02, 0x1E, 0x4A, \
  0x02, 0xF5, 0x40, 0xB4, 0xA1, 0x0F, 0x25, 0x01, 0x7A, 0xA0, 0x5A, 0xA0, 0x87, \
  0x92, 0x80, 0xBD, 0x50, 0x2D, 0x78, 0x43, 0xC9, 0x40, 0x5E, 0xA8, 0x16, 0xD0, \
  0x21, 0xE4, 0xA0, 0x2F, 0x54, 0x0B, 0x72, 0x10, 0xF2, 0x50, 0x17, 0xA4, 0x3F, \
  0x67, 0x60, 0xBF, 0x38, 0xF2, 0x9B, 0x65, 0xAF, 0xCE, 0x3C, 0xA6, 0xDA, 0xAB, \
  0xF3, 0x8F, 0x29, 0xB6, 0xFA, 0xFC, 0xE3, 0xCA, 0x6D, 0xD2, 0xBF, 0x38, 0xF2, \
  0x9B, 0x79, 0xAF, 0xCE, 0x3C, 0xA6, 0xDF, 0xAB, 0xF3, 0x8F, 0x29, 0xB8, 0x3A, \
  0xFC, 0xE3, 0xCA, 0x6E, 0x22, 0xBF, 0x38, 0xF2, 0x9B, 0x8D, 0xAF, 0xCE, 0x3C, \
  0xA6, 0xE4, 0xAB, 0xF3, 0x8F, 0x29, 0xB9, 0x7A, 0xFC, 0xE3, 0xCA, 0x6E, 0x72, \
  0xBF, 0x38, 0xF2, 0x9B, 0xA1, 0xAF, 0xCE, 0x3C, 0xA6, 0xE9, 0xAB, 0xF3, 0x8F, \
  0x29, 0xBA, 0xBA, 0xFC, 0xE3, 0xCA, 0x40, 0x56, 0x83, 0xC6, 0xF4, 0x11, 0xD0, \
  0x79, 0x7B, 0xFC, 0x30, 0x3E, 0x04, 0x57, 0x01, 0xEF, 0xF0, 0x00, 0x00, 0x04, \
  0x64, 0x07, 0xBF, 0xC3, 0x03, 0xE0, 0x15, 0xF0, 0x1E, 0xFF, 0x0C, 0x0F, 0x81, \
  0x4D, 0x40, 0x7B, 0xFC, 0x30, 0x3E, 0x06, 0x13, 0x01, 0xEF, 0xF0, 0xC0, 0xF8, \
  0x18, 0xB4, 0x07, 0xBF, 0xC3, 0x03, 0xE0, 0x64, 0x10, 0x1E, 0xFF, 0x0C, 0x0F, \
  0x81, 0x95, 0x40, 0x7B, 0xFC, 0x00, 0x00, 0x06, 0xA4, 0x01, 0xEF, 0xF0, 0x00, \
  0x00, 0x1C, 0x24, 0x07, 0x80, 0x00, 0x00, 0x00, 0x76, 0xD0, 0x16, 0x00, 0x00, \
  0x00, 0x79, 0x10, 0x16, 0x00, 0x00, 0x00, 0x7B, 0x70, 0x16, 0x00, 0x00, 0x00, \
  0x55, 0x70, 0x16, 0x00, 0x00, 0x00, 0x41, 0x30, 0x16, 0x00, 0x00, 0x00, \
}

/*
CMCC_SIB_MSG:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib4 : 
                {
                  intraFreqNeighCellList 
                  {
                    {
                      physCellId 36,
                      q-OffsetCell dB3
                    },
                    {
                      physCellId 38,
                      q-OffsetCell dB3
                    }
                  }
                },
              sib5 : 
                {
                  interFreqCarrierFreqList 
                  {
                    {
                      dl-CarrierFreq 5760,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5780,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5800,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5820,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5840,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5860,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    }
                  }
                },
              sib6 : 
                {
                  carrierFreqListUTRA-TDD 
                  {
                    {
                      carrierFreq 10080,
                      cellReselectionPriority 7,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11670,
                      cellReselectionPriority 7,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11690,
                      cellReselectionPriority 7,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11710,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11730,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11750,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11770,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11790,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11810,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11830,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11850,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11870,
                      cellReselectionPriority 7,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11890,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11910,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11930,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11950,
                      cellReselectionPriority 7,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    }
                  },
                  t-ReselectionUTRA 2,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  }
                },
              sib7 : 
                {
                  t-ReselectionGERAN 2,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 222,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            232,
                            242
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 555,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 140,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 175,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 666,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 777,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 790,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 800,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 810,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 850,
                        bandIndicator dcs1800,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 900,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 950,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 968,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 987,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 683,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 521,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    }
                  }
                }
            }
          }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG_WITH_DIFF_TDS_PRIO \
static const uint8 lte_rrc_meas_test_idle_cmcc_msg_with_different_tds_prio[] = \
{ \
  0x01, 0x89, 0x04, 0x24, 0x90, 0x4D, 0x21, 0xAA, 0x81, 0x68, 0x02, 0x1E, 0x4A, \
  0x02, 0xF5, 0x40, 0xB4, 0xA1, 0x0F, 0x25, 0x01, 0x7A, 0xA0, 0x5A, 0xA0, 0x87, \
  0x92, 0x80, 0xBD, 0x50, 0x2D, 0x78, 0x43, 0xC9, 0x40, 0x5E, 0xA8, 0x16, 0xD0, \
  0x21, 0xE4, 0xA0, 0x2F, 0x54, 0x0B, 0x72, 0x10, 0xF2, 0x50, 0x17, 0xA4, 0x3F, \
  0x67, 0x60, 0xFF, 0x38, 0xF2, 0x9B, 0x65, 0xBF, 0xCE, 0x3C, 0xA6, 0xDA, 0xAF, \
  0xF3, 0x8F, 0x29, 0xB6, 0xFB, 0x7C, 0xE3, 0xCA, 0x6D, 0xD2, 0xDF, 0x38, 0xF2, \
  0x9B, 0x79, 0xB7, 0xCE, 0x3C, 0xA6, 0xDF, 0xAB, 0xF3, 0x8F, 0x29, 0xB8, 0x3A, \
  0xFC, 0xE3, 0xCA, 0x6E, 0x22, 0xBF, 0x38, 0xF2, 0x9B, 0x8D, 0xA7, 0xCE, 0x3C, \
  0xA6, 0xE4, 0xAD, 0xF3, 0x8F, 0x29, 0xB9, 0x7B, 0xFC, 0xE3, 0xCA, 0x6E, 0x72, \
  0x9F, 0x38, 0xF2, 0x9B, 0xA1, 0xAF, 0xCE, 0x3C, 0xA6, 0xE9, 0xA9, 0xF3, 0x8F, \
  0x29, 0xBA, 0xBB, 0xFC, 0xE3, 0xCA, 0x40, 0x56, 0x83, 0xC6, 0xF4, 0x11, 0xD0, \
  0x79, 0x7B, 0xFC, 0x30, 0x3E, 0x04, 0x57, 0x01, 0xEF, 0xF0, 0x00, 0x00, 0x04, \
  0x64, 0x07, 0xBF, 0xC3, 0x03, 0xE0, 0x15, 0xF0, 0x1E, 0xFF, 0x0C, 0x0F, 0x81, \
  0x4D, 0x40, 0x7B, 0xFC, 0x30, 0x3E, 0x06, 0x13, 0x01, 0xEF, 0xF0, 0xC0, 0xF8, \
  0x18, 0xB4, 0x07, 0xBF, 0xC3, 0x03, 0xE0, 0x64, 0x10, 0x1E, 0xFF, 0x0C, 0x0F, \
  0x81, 0x95, 0x40, 0x7B, 0xFC, 0x00, 0x00, 0x06, 0xA4, 0x01, 0xEF, 0xF0, 0x00, \
  0x00, 0x1C, 0x24, 0x07, 0x80, 0x00, 0x00, 0x00, 0x76, 0xD0, 0x16, 0x00, 0x00, \
  0x00, 0x79, 0x10, 0x16, 0x00, 0x00, 0x00, 0x7B, 0x70, 0x16, 0x00, 0x00, 0x00, \
  0x55, 0x70, 0x16, 0x00, 0x00, 0x00, 0x41, 0x30, 0x16, 0x00, 0x00, 0x00, \
}

/*
cmcc_wo_lte:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib4 : 
                {
                  intraFreqNeighCellList 
                  {
                    {
                      physCellId 36,
                      q-OffsetCell dB3
                    },
                    {
                      physCellId 38,
                      q-OffsetCell dB3
                    }
                  }
                },
              sib8 : 
                {
                },
              sib6 : 
                {
                  carrierFreqListUTRA-TDD 
                  {
                    {
                      carrierFreq 10080,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11670,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11690,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11710,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11730,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11750,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11770,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11790,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11810,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11830,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11850,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11870,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11890,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11910,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11930,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11950,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    }
                  },
                  t-ReselectionUTRA 2,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  }
                },
              sib7 : 
                {
                  t-ReselectionGERAN 2,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 222,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            232,
                            242
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 555,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 140,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 175,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 666,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 777,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 790,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 800,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 810,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 850,
                        bandIndicator dcs1800,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 900,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 950,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 968,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 987,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 683,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 521,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 6,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    }
                  }
                }
            }
          }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG_WO_LTE \
static const uint8 lte_rrc_meas_test_idle_cmcc_msg_wo_lte[] = \
{ \
  0x01, 0x89, 0x04, 0x24, 0x90, 0x4D, 0x23, 0x00, 0x87, 0xEC, 0xEC, 0x17, 0xE7, 0x1E, \
  0x53, 0x6C, 0xB5, 0xF9, 0xC7, 0x94, 0xDB, 0x55, 0x7E, 0x71, 0xE5, 0x36, 0xDF, 0x5F, \
  0x9C, 0x79, 0x4D, 0xBA, 0x57, 0xE7, 0x1E, 0x53, 0x6F, 0x35, 0xF9, 0xC7, 0x94, 0xDB, \
  0xF5, 0x7E, 0x71, 0xE5, 0x37, 0x07, 0x5F, 0x9C, 0x79, 0x4D, 0xC4, 0x57, 0xE7, 0x1E, \
  0x53, 0x71, 0xB5, 0xF9, 0xC7, 0x94, 0xDC, 0x95, 0x7E, 0x71, 0xE5, 0x37, 0x2F, 0x5F, \
  0x9C, 0x79, 0x4D, 0xCE, 0x57, 0xE7, 0x1E, 0x53, 0x74, 0x35, 0xF9, 0xC7, 0x94, 0xDD, \
  0x35, 0x7E, 0x71, 0xE5, 0x37, 0x57, 0x5F, 0x9C, 0x79, 0x48, 0x0A, 0xD0, 0x78, 0xDE, \
  0x82, 0x3A, 0x0F, 0x2F, 0x7F, 0x86, 0x07, 0xC0, 0x8A, 0xE0, 0x3D, 0xFE, 0x00, 0x00, \
  0x00, 0x8C, 0x80, 0xF7, 0xF8, 0x60, 0x7C, 0x02, 0xBE, 0x03, 0xDF, 0xE1, 0x81, 0xF0, \
  0x29, 0xA8, 0x0F, 0x7F, 0x86, 0x07, 0xC0, 0xC2, 0x60, 0x3D, 0xFE, 0x18, 0x1F, 0x03, \
  0x16, 0x80, 0xF7, 0xF8, 0x60, 0x7C, 0x0C, 0x82, 0x03, 0xDF, 0xE1, 0x81, 0xF0, 0x32, \
  0xA8, 0x0F, 0x7F, 0x80, 0x00, 0x00, 0xD4, 0x80, 0x3D, 0xFE, 0x00, 0x00, 0x03, 0x84, \
  0x80, 0xF0, 0x00, 0x00, 0x00, 0x0E, 0xDA, 0x02, 0xC0, 0x00, 0x00, 0x0F, 0x22, 0x02, \
  0xC0, 0x00, 0x00, 0x0F, 0x6E, 0x02, 0xC0, 0x00, 0x00, 0x0A, 0xAE, 0x02, 0xC0, 0x00, \
  0x00, 0x08, 0x26, 0x02, 0xC0, 0x00, 0x00, 0x00,\
}

/*
Cmcc_msg1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib4 : 
                {
                  intraFreqNeighCellList 
                  {
                    {
                      physCellId 36,
                      q-OffsetCell dB3
                    },
                    {
                      physCellId 38,
                      q-OffsetCell dB3
                    }
                  }
                },
              sib5 : 
                {
                  interFreqCarrierFreqList 
                  {
                    {
                      dl-CarrierFreq 5760,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5780,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5800,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5820,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5840,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5860,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    }
                  }
                },
              sib6 : 
                {
                  carrierFreqListUTRA-TDD 
                  {
                    {
                      carrierFreq 10080,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11670,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11690,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11710,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11730,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11750,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11770,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11790,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11810,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11830,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11850,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11870,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11890,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11910,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11930,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11950,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    }
                  },
                  t-ReselectionUTRA 2,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  }
                },
              sib7 : 
                {
                  t-ReselectionGERAN 2,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 222,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            232,
                            242
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 555,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 140,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 175,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 666,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 777,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 790,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 800,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 810,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 850,
                        bandIndicator dcs1800,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 900,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 950,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 968,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 987,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 683,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 521,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    }
                  }
                }
            }
          }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG2 \
static const uint8 lte_rrc_meas_test_idle_cmcc_msg2[] = \
{ \
  0x01, 0x89, 0x04, 0x24, 0x90, 0x4D, 0x21, 0xAA, 0x81, 0x68, 0x02, 0x1E, 0x4A, \
  0x02, 0xF5, 0x40, 0xB4, 0xA1, 0x0F, 0x25, 0x01, 0x7A, 0xA0, 0x5A, 0xA0, 0x87, \
  0x92, 0x80, 0xBD, 0x50, 0x2D, 0x78, 0x43, 0xC9, 0x40, 0x5E, 0xA8, 0x16, 0xD0, \
  0x21, 0xE4, 0xA0, 0x2F, 0x54, 0x0B, 0x72, 0x10, 0xF2, 0x50, 0x17, 0xA4, 0x3F, \
  0x67, 0x60, 0xDF, 0x38, 0xF2, 0x9B, 0x65, 0xB7, 0xCE, 0x3C, 0xA6, 0xDA, 0xAD, \
  0xF3, 0x8F, 0x29, 0xB6, 0xFB, 0x7C, 0xE3, 0xCA, 0x6D, 0xD2, 0xDF, 0x38, 0xF2, \
  0x9B, 0x79, 0xB7, 0xCE, 0x3C, 0xA6, 0xDF, 0xAD, 0xF3, 0x8F, 0x29, 0xB8, 0x3B, \
  0x7C, 0xE3, 0xCA, 0x6E, 0x22, 0xDF, 0x38, 0xF2, 0x9B, 0x8D, 0xB7, 0xCE, 0x3C, \
  0xA6, 0xE4, 0xAD, 0xF3, 0x8F, 0x29, 0xB9, 0x7B, 0x7C, 0xE3, 0xCA, 0x6E, 0x72, \
  0xDF, 0x38, 0xF2, 0x9B, 0xA1, 0xB7, 0xCE, 0x3C, 0xA6, 0xE9, 0xAD, 0xF3, 0x8F, \
  0x29, 0xBA, 0xBB, 0x7C, 0xE3, 0xCA, 0x40, 0x56, 0x83, 0xC6, 0xF4, 0x11, 0xD0, \
  0x79, 0x77, 0xFC, 0x30, 0x3E, 0x04, 0x57, 0x01, 0xDF, 0xF0, 0x00, 0x00, 0x04, \
  0x64, 0x07, 0x7F, 0xC3, 0x03, 0xE0, 0x15, 0xF0, 0x1D, 0xFF, 0x0C, 0x0F, 0x81, \
  0x4D, 0x40, 0x77, 0xFC, 0x30, 0x3E, 0x06, 0x13, 0x01, 0xDF, 0xF0, 0xC0, 0xF8, \
  0x18, 0xB4, 0x07, 0x7F, 0xC3, 0x03, 0xE0, 0x64, 0x10, 0x1D, 0xFF, 0x0C, 0x0F, \
  0x81, 0x95, 0x40, 0x77, 0xFC, 0x00, 0x00, 0x06, 0xA4, 0x01, 0xDF, 0xF0, 0x00, \
  0x00, 0x1C, 0x24, 0x07, 0x40, 0x00, 0x00, 0x00, 0x76, 0xD0, 0x15, 0x00, 0x00, \
  0x00, 0x79, 0x10, 0x15, 0x00, 0x00, 0x00, 0x7B, 0x70, 0x15, 0x00, 0x00, 0x00, \
  0x55, 0x70, 0x15, 0x00, 0x00, 0x00, 0x41, 0x30, 0x15, 0x00, 0x00, 0x00, \
}


/*
Cmcc_msg1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib4 : 
                {
                  intraFreqNeighCellList 
                  {
                    {
                      physCellId 36,
                      q-OffsetCell dB3
                    },
                    {
                      physCellId 38,
                      q-OffsetCell dB3
                    }
                  }
                },
              sib5 : 
                {
                  interFreqCarrierFreqList 
                  {
                    {
                      dl-CarrierFreq 5760,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 6,
                      neighCellConfig '01'B
                    }
                  }
                },
              sib6 : 
                {
                  carrierFreqListUTRA-TDD 
                  {
                    {
                      carrierFreq 10080,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11670,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11690,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11710,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11730,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11750,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11770,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11790,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11810,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11830,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11850,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11870,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11890,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11910,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11930,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11950,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    }
                  },
                  t-ReselectionUTRA 2,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  }
                },
              sib7 : 
                {
                  t-ReselectionGERAN 2,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 222,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            232,
                            242
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 555,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 140,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 175,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 666,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 777,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 790,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 800,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 810,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 850,
                        bandIndicator dcs1800,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 900,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 950,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 968,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 987,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 683,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 521,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    }
                  }
                },
              sib8 : 
                {
                  systemTimeInfo 
                  {
                    cdma-EUTRA-Synchronisation TRUE,
                    cdma-SystemTime synchronousSystemTime : '00011100 00011101 00011110 00011111 001 ...'B
                  },
                  searchWindowSize 10,
                  parametersHRPD 
                  {
                    preRegistrationInfoHRPD 
                    {
                      preRegistrationAllowed FALSE
                    },
                    cellReselectionParametersHRPD 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc0,
                          cellReselectionPriority 7,
                          threshX-High 60,
                          threshX-Low 10
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc0,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 10,
                              physCellIdList 
                              {
                                15
                              }
                            },
                            {
                              arfcn 20,
                              physCellIdList 
                              {
                                25
                              }
                            },
                            {
                              arfcn 30,
                              physCellIdList 
                              {
                                35
                              }
                            },
                            {
                              arfcn 40,
                              physCellIdList 
                              {
                                45
                              }
                            },
                            {
                              arfcn 50,
                              physCellIdList 
                              {
                                55
                              }
                            },
                            {
                              arfcn 60,
                              physCellIdList 
                              {
                                65
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 5,
                      t-ReselectionCDMA2000-SF 
                      {
                        sf-Medium oDot5,
                        sf-High oDot75
                      }
                    }
                  }
                }
            }
          }
      }
}
*/

#define DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG3 \
static const uint8 lte_rrc_meas_test_idle_cmcc_msg3[] = \
{ \
  0x02, 0x09, 0x04, 0x24, 0x90, 0x4D, 0x21, 0x82, 0x81, 0x68, 0x02, 0x1E, 0x4A, \
  0x02, 0xE4, 0x87, 0xEC, 0xEC, 0x13, 0xE7, 0x1E, 0x53, 0x6C, 0xB4, 0xF9, 0xC7, \
  0x94, 0xDB, 0x55, 0x3E, 0x71, 0xE5, 0x36, 0xDF, 0x4F, 0x9C, 0x79, 0x4D, 0xBA, \
  0x53, 0xE7, 0x1E, 0x53, 0x6F, 0x34, 0xF9, 0xC7, 0x94, 0xDB, 0xF5, 0x3E, 0x71, \
  0xE5, 0x37, 0x07, 0x4F, 0x9C, 0x79, 0x4D, 0xC4, 0x53, 0xE7, 0x1E, 0x53, 0x71, \
  0xB4, 0xF9, 0xC7, 0x94, 0xDC, 0x95, 0x3E, 0x71, 0xE5, 0x37, 0x2F, 0x4F, 0x9C, \
  0x79, 0x4D, 0xCE, 0x53, 0xE7, 0x1E, 0x53, 0x74, 0x34, 0xF9, 0xC7, 0x94, 0xDD, \
  0x35, 0x3E, 0x71, 0xE5, 0x37, 0x57, 0x4F, 0x9C, 0x79, 0x48, 0x0A, 0xD0, 0x78, \
  0xDE, 0x82, 0x3A, 0x0F, 0x2E, 0xFF, 0x86, 0x07, 0xC0, 0x8A, 0xE0, 0x3B, 0xFE, \
  0x00, 0x00, 0x00, 0x8C, 0x80, 0xEF, 0xF8, 0x60, 0x7C, 0x02, 0xBE, 0x03, 0xBF, \
  0xE1, 0x81, 0xF0, 0x29, 0xA8, 0x0E, 0xFF, 0x86, 0x07, 0xC0, 0xC2, 0x60, 0x3B, \
  0xFE, 0x18, 0x1F, 0x03, 0x16, 0x80, 0xEF, 0xF8, 0x60, 0x7C, 0x0C, 0x82, 0x03, \
  0xBF, 0xE1, 0x81, 0xF0, 0x32, 0xA8, 0x0E, 0xFF, 0x80, 0x00, 0x00, 0xD4, 0x80, \
  0x3B, 0xFE, 0x00, 0x00, 0x03, 0x84, 0x80, 0xE8, 0x00, 0x00, 0x00, 0x0E, 0xDA, \
  0x02, 0xA0, 0x00, 0x00, 0x0F, 0x22, 0x02, 0xA0, 0x00, 0x00, 0x0F, 0x6E, 0x02, \
  0xA0, 0x00, 0x00, 0x0A, 0xAE, 0x02, 0xA0, 0x00, 0x00, 0x08, 0x26, 0x02, 0xA0, \
  0x00, 0x00, 0x06, 0x74, 0x38, 0x3A, 0x3C, 0x3E, 0x42, 0xA2, 0x04, 0x0F, 0xE1, \
  0x40, 0x02, 0x80, 0xA0, 0x07, 0x81, 0x40, 0x0C, 0x81, 0xE0, 0x11, 0x82, 0x80, \
  0x16, 0x83, 0x20, 0x1B, 0x83, 0xC0, 0x20, 0xD6, \
}


/*
Cmcc_msg1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib4 : 
                {
                  intraFreqNeighCellList 
                  {
                    {
                      physCellId 36,
                      q-OffsetCell dB3
                    },
                    {
                      physCellId 38,
                      q-OffsetCell dB3
                    }
                  }
                },
              sib5 : 
                {
                  interFreqCarrierFreqList 
                  {
                    {
                      dl-CarrierFreq 5760,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 6,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5780,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 6,
                      neighCellConfig '01'B
                    }
                  }
                },
              sib6 : 
                {
                  carrierFreqListUTRA-TDD 
                  {
                    {
                      carrierFreq 10080,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11670,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11690,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11710,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11730,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11750,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11770,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11790,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11810,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11830,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11850,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11870,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11890,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11910,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11930,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11950,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    }
                  },
                  t-ReselectionUTRA 2,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  }
                },
              sib7 : 
                {
                  t-ReselectionGERAN 2,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 222,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            232,
                            242
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 555,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    }
                  }
                }
            }
          }
      }
}

*/

#define DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG4 \
static const uint8 lte_rrc_meas_test_idle_cmcc_msg4[] = \
{ \
  0x01, 0x89, 0x04, 0x24, 0x90, 0x4D, 0x21, 0x8A, 0x81, 0x68, 0x02, 0x1E, 0x4A, \
  0x02, 0xE5, 0x40, 0xB4, 0xA1, 0x0F, 0x25, 0x01, 0x72, 0x43, 0xF6, 0x76, 0x09, \
  0xF3, 0x8F, 0x29, 0xB6, 0x5A, 0x7C, 0xE3, 0xCA, 0x6D, 0xAA, 0x9F, 0x38, 0xF2, \
  0x9B, 0x6F, 0xA7, 0xCE, 0x3C, 0xA6, 0xDD, 0x29, 0xF3, 0x8F, 0x29, 0xB7, 0x9A, \
  0x7C, 0xE3, 0xCA, 0x6D, 0xFA, 0x9F, 0x38, 0xF2, 0x9B, 0x83, 0xA7, 0xCE, 0x3C, \
  0xA6, 0xE2, 0x29, 0xF3, 0x8F, 0x29, 0xB8, 0xDA, 0x7C, 0xE3, 0xCA, 0x6E, 0x4A, \
  0x9F, 0x38, 0xF2, 0x9B, 0x97, 0xA7, 0xCE, 0x3C, 0xA6, 0xE7, 0x29, 0xF3, 0x8F, \
  0x29, 0xBA, 0x1A, 0x7C, 0xE3, 0xCA, 0x6E, 0x9A, 0x9F, 0x38, 0xF2, 0x9B, 0xAB, \
  0xA7, 0xCE, 0x3C, 0xA4, 0x05, 0x68, 0x04, 0x6F, 0x41, 0x1D, 0x07, 0x97, 0x7F, \
  0xC3, 0x03, 0xE0, 0x45, 0x70, 0x1D, 0xFF, 0x00, 0x00, 0x00, \
}

/*
Cmcc_msg1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib4 : 
                {
                  intraFreqNeighCellList 
                  {
                    {
                      physCellId 36,
                      q-OffsetCell dB3
                    },
                    {
                      physCellId 38,
                      q-OffsetCell dB3
                    }
                  }
                },
              sib5 : 
                {
                  interFreqCarrierFreqList 
                  {
                    {
                      dl-CarrierFreq 5760,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 6,
                      neighCellConfig '01'B
                    }
                  }
                },
              sib6 : 
                {
                  carrierFreqListUTRA-TDD 
                  {
                    {
                      carrierFreq 10080,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11670,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11690,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11710,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11730,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11750,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11770,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11790,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11810,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11830,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11850,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11870,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11890,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11910,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11930,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11950,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    }
                  },
                  t-ReselectionUTRA 2,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  }
                },
              sib7 : 
                {
                  t-ReselectionGERAN 2,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 222,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            232,
                            242
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 555,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    }
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG5 \
static const uint8 lte_rrc_meas_test_idle_cmcc_msg5[] = \
{ \
  0x01, 0x89, 0x04, 0x24, 0x90, 0x4D, 0x21, 0x82, 0x81, 0x68, 0x02, 0x1E, 0x4A, \
  0x02, 0xE4, 0x87, 0xEC, 0xEC, 0x13, 0xE7, 0x1E, 0x53, 0x6C, 0xB4, 0xF9, 0xC7, \
  0x94, 0xDB, 0x55, 0x3E, 0x71, 0xE5, 0x36, 0xDF, 0x4F, 0x9C, 0x79, 0x4D, 0xBA, \
  0x53, 0xE7, 0x1E, 0x53, 0x6F, 0x34, 0xF9, 0xC7, 0x94, 0xDB, 0xF5, 0x3E, 0x71, \
  0xE5, 0x37, 0x07, 0x4F, 0x9C, 0x79, 0x4D, 0xC4, 0x53, 0xE7, 0x1E, 0x53, 0x71, \
  0xB4, 0xF9, 0xC7, 0x94, 0xDC, 0x95, 0x3E, 0x71, 0xE5, 0x37, 0x2F, 0x4F, 0x9C, \
  0x79, 0x4D, 0xCE, 0x53, 0xE7, 0x1E, 0x53, 0x74, 0x34, 0xF9, 0xC7, 0x94, 0xDD, \
  0x35, 0x3E, 0x71, 0xE5, 0x37, 0x57, 0x4F, 0x9C, 0x79, 0x48, 0x0A, 0xD0, 0x08, \
  0xDE, 0x82, 0x3A, 0x0F, 0x2E, 0xFF, 0x86, 0x07, 0xC0, 0x8A, 0xE0, 0x3B, 0xFE, \
  0x00, 0x00, 0x00,  \
}

/*
Cmcc_msg1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib4 : 
                {
                  intraFreqNeighCellList 
                  {
                    {
                      physCellId 36,
                      q-OffsetCell dB3
                    },
                    {
                      physCellId 38,
                      q-OffsetCell dB3
                    }
                  }
                },
              sib5 : 
                {
                  interFreqCarrierFreqList 
                  {
                    {
                      dl-CarrierFreq 5760,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 6,
                      neighCellConfig '01'B
                    }
                  }
                },
              sib6 : 
                {
                  carrierFreqListUTRA-TDD 
                  {
                    {
                      carrierFreq 10080,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11670,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    }
                  },
                  t-ReselectionUTRA 2,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  }
                },
              sib7 : 
                {
                  t-ReselectionGERAN 2,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 222,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            232,
                            242
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 555,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    }
                  }
                }
            }
          }
      }
}

*/
#define DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG6 \
static const uint8 lte_rrc_meas_test_idle_cmcc_msg6[] = \
{ \
  0x01, 0x89, 0x04, 0x24, 0x90, 0x4D, 0x21, 0x82, 0x81, 0x68, 0x02, 0x1E, 0x4A, \
  0x02, 0xE4, 0x86, 0x2C, 0xEC, 0x13, 0xE7, 0x1E, 0x53, 0x6C, 0xB4, 0xF9, 0xC7, \
  0x94, 0x80, 0xAD, 0x00, 0x8D, 0xE8, 0x23, 0xA0, 0xF2, 0xEF, 0xF8, 0x60, 0x7C, \
  0x08, 0xAE, 0x03, 0xBF, 0xE0, 0x00, 0x00, 0x00, \
}

/*
Cmcc_msg1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib4 : 
                {
                  intraFreqNeighCellList 
                  {
                    {
                      physCellId 36,
                      q-OffsetCell dB3
                    },
                    {
                      physCellId 38,
                      q-OffsetCell dB3
                    }
                  }
                },
              sib5 : 
                {
                  interFreqCarrierFreqList 
                  {
                    {
                      dl-CarrierFreq 5760,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 6,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5780,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 6,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5800,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 6,
                      neighCellConfig '01'B
                    }
                  }
                },
              sib6 : 
                {
                  carrierFreqListUTRA-TDD 
                  {
                    {
                      carrierFreq 10080,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11670,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11690,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11710,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11730,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11750,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11770,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11790,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    }
                  },
                  t-ReselectionUTRA 2,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  }
                },
              sib7 : 
                {
                  t-ReselectionGERAN 2,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 222,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            232,
                            242
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 555,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    }
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG7 \
static const uint8 lte_rrc_meas_test_idle_cmcc_msg7[] = \
{ \
  0x01, 0x89, 0x04, 0x24, 0x90, 0x4D, 0x21, 0x92, 0x81, 0x68, 0x02, 0x1E, 0x4A, \
  0x02, 0xE5, 0x40, 0xB4, 0xA1, 0x0F, 0x25, 0x01, 0x72, 0xA0, 0x5A, 0xA0, 0x87, \
  0x92, 0x80, 0xB9, 0x21, 0xBB, 0x3B, 0x04, 0xF9, 0xC7, 0x94, 0xDB, 0x2D, 0x3E, \
  0x71, 0xE5, 0x36, 0xD5, 0x4F, 0x9C, 0x79, 0x4D, 0xB7, 0xD3, 0xE7, 0x1E, 0x53, \
  0x6E, 0x94, 0xF9, 0xC7, 0x94, 0xDB, 0xCD, 0x3E, 0x71, 0xE5, 0x36, 0xFD, 0x4F, \
  0x9C, 0x79, 0x4D, 0xC1, 0xD3, 0xE7, 0x1E, 0x52, 0x02, 0xB4, 0x02, 0x37, 0xA0, \
  0x8E, 0x83, 0xCB, 0xBF, 0xE1, 0x81, 0xF0, 0x22, 0xB8, 0x0E, 0xFF, 0x80, 0x00, \
  0x00, \
}

/*
Cmcc_msg1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib4 : 
                {
                  intraFreqNeighCellList 
                  {
                    {
                      physCellId 36,
                      q-OffsetCell dB3
                    },
                    {
                      physCellId 38,
                      q-OffsetCell dB3
                    }
                  }
                },
              sib5 : 
                {
                  interFreqCarrierFreqList 
                  {
                    {
                      dl-CarrierFreq 5760,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 2,
                      neighCellConfig '01'B
                    }
                  }
                },
              sib6 : 
                {
                  carrierFreqListUTRA-TDD 
                  {
                    {
                      carrierFreq 10080,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11670,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11690,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11710,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11730,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11750,
                      cellReselectionPriority 4,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    }
                  },
                  t-ReselectionUTRA 2,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  }
                },
              sib7 : 
                {
                  t-ReselectionGERAN 2,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 222,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            232,
                            242
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 555,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 140,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 175,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 666,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 777,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 790,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 800,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 810,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 850,
                        bandIndicator dcs1800,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 900,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 950,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 968,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 987,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 683,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 521,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000000'B,
                        q-RxLevMin 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    }
                  }
                }
            }
          }
      }
}

*/
#define DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG8 \
static const uint8 lte_rrc_meas_test_idle_cmcc_msg8[] = \
{ \
  0x01, 0x89, 0x04, 0x24, 0x90, 0x4D, 0x21, 0x82, 0x81, 0x68, 0x02, 0x1E, 0x4A, \
  0x02, 0xA4, 0x86, 0xAC, 0xEC, 0x13, 0xE7, 0x1E, 0x53, 0x6C, 0xB4, 0xF9, 0xC7, \
  0x94, 0xDB, 0x55, 0x3E, 0x71, 0xE5, 0x36, 0xDF, 0x4F, 0x9C, 0x79, 0x4D, 0xBA, \
  0x53, 0xE7, 0x1E, 0x53, 0x6F, 0x34, 0xF9, 0xC7, 0x94, 0x80, 0xAD, 0x07, 0x8D, \
  0xE8, 0x23, 0xA0, 0xF2, 0xEF, 0xF8, 0x60, 0x7C, 0x08, 0xAE, 0x03, 0xBF, 0xE0, \
  0x00, 0x00, 0x08, 0xC8, 0x0E, 0xFF, 0x86, 0x07, 0xC0, 0x2B, 0xE0, 0x3B, 0xFE, \
  0x18, 0x1F, 0x02, 0x9A, 0x80, 0xEF, 0xF8, 0x60, 0x7C, 0x0C, 0x26, 0x03, 0xBF, \
  0xE1, 0x81, 0xF0, 0x31, 0x68, 0x0E, 0xFF, 0x86, 0x07, 0xC0, 0xC8, 0x20, 0x3B, \
  0xFE, 0x18, 0x1F, 0x03, 0x2A, 0x80, 0xEF, 0xF8, 0x00, 0x00, 0x0D, 0x48, 0x03, \
  0xBF, 0xE0, 0x00, 0x00, 0x38, 0x48, 0x0E, 0x80, 0x00, 0x00, 0x00, 0xED, 0xA0, \
  0x2A, 0x00, 0x00, 0x00, 0xF2, 0x20, 0x2A, 0x00, 0x00, 0x00, 0xF6, 0xE0, 0x2A, \
  0x00, 0x00, 0x00, 0xAA, 0xE0, 0x2A, 0x00, 0x00, 0x00, 0x82, 0x60, 0x2A, 0x00, \
  0x00, 0x00, \
}

/*
Cmcc_msg1:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib4 : 
                {
                  intraFreqNeighCellList 
                  {
                    {
                      physCellId 36,
                      q-OffsetCell dB3
                    },
                    {
                      physCellId 38,
                      q-OffsetCell dB3
                    }
                  }
                },
              sib5 : 
                {
                  interFreqCarrierFreqList 
                  {
                    {
                      dl-CarrierFreq 5760,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 6,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5780,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 5,
                      neighCellConfig '01'B
                    },
                    {
                      dl-CarrierFreq 5800,
                      q-RxLevMin -62,
                      p-Max 0,
                      t-ReselectionEUTRA 2,
                      threshX-High 10,
                      threshX-Low 0,
                      allowedMeasBandwidth mbw25,
                      presenceAntennaPort1 TRUE,
                      cellReselectionPriority 7,
                      neighCellConfig '01'B
                    }
                  }
                },
              sib6 : 
                {
                  carrierFreqListUTRA-TDD 
                  {
                    {
                      carrierFreq 10080,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11670,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11690,
                      cellReselectionPriority 7,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11710,
                      cellReselectionPriority 7,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11730,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11750,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11770,
                      cellReselectionPriority 5,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11790,
                      cellReselectionPriority 6,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    },
                    {
                      carrierFreq 11810,
                      cellReselectionPriority 7,
                      threshX-High 31,
                      threshX-Low 7,
                      q-RxLevMin -53,
                      p-MaxUTRA 24
                    }
                  },
                  t-ReselectionUTRA 2,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  }
                },
              sib7 : 
                {
                  t-ReselectionGERAN 2,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot25,
                    sf-High oDot25
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 222,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            232,
                            242
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 3,
                        p-MaxGERAN 0,
                        threshX-High 31,
                        threshX-Low 0
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 555,
                        bandIndicator pcs1900,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '11111111'B,
                        q-RxLevMin 0,
                        p-MaxGERAN 0,
                        threshX-High 0,
                        threshX-Low 0
                      }
                    }
                  }
                }
            }
          }
      }
}

*/
#define DECLARE_LTE_RRC_MEAS_TEST_IDLE_CMCC_MSG9 \
static const uint8 lte_rrc_meas_test_idle_cmcc_msg9[] = \
{ \
  0x01, 0x89, 0x04, 0x24, 0x90, 0x4D, 0x21, 0x92, 0x81, 0x68, 0x02, 0x1E, 0x4A, \
  0x02, 0xE5, 0x40, 0xB4, 0xA1, 0x0F, 0x25, 0x01, 0x6A, 0xA0, 0x5A, 0xA0, 0x87, \
  0x92, 0x80, 0xBD, 0x21, 0xC3, 0x3B, 0x06, 0xF9, 0xC7, 0x94, 0xDB, 0x2D, 0x7E, \
  0x71, 0xE5, 0x36, 0xD5, 0x7F, 0x9C, 0x79, 0x4D, 0xB7, 0xDF, 0xE7, 0x1E, 0x53, \
  0x6E, 0x96, 0xF9, 0xC7, 0x94, 0xDB, 0xCD, 0x7E, 0x71, 0xE5, 0x36, 0xFD, 0x5F, \
  0x9C, 0x79, 0x4D, 0xC1, 0xDB, 0xE7, 0x1E, 0x53, 0x71, 0x17, 0xF9, 0xC7, 0x94, \
  0x80, 0xAD, 0x00, 0x8D, 0xE8, 0x23, 0xA0, 0xF2, 0xEF, 0xF8, 0x60, 0x7C, 0x08, \
  0xAE, 0x03, 0xBF, 0xE0, 0x00, 0x00, 0x00, \
}

/*
value BCCH-DL-SCH-Message ::= 
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
                    cdma-EUTRA-Synchronisation TRUE,
                    cdma-SystemTime synchronousSystemTime : '00101111 01001100 01001010 01110010 0101111'B
                  },
                  searchWindowSize 10,
                  parameters1XRTT 
                  {
                    csfb-RegistrationParam1XRTT 
                    {
                      sid '00000000 0000001'B,
                      nid '00000000 00000001'B,
                      multipleSID TRUE,
                      multipleNID TRUE,
                      homeReg TRUE,
                      foreignSIDReg TRUE,
                      foreignNIDReg TRUE,
                      parameterReg TRUE,
                      powerUpReg TRUE,
                      registrationPeriod '0000000'B,
                      registrationZone '00000000 0001'B,
                      totalZone '000'B,
                      zoneTimer '010'B
                    },
                    cellReselectionParameters1XRTT 
                    {
                      bandClassList 
                      {
                        {
                          bandClass bc0,
                          cellReselectionPriority 3,
                          threshX-High 25,
                          threshX-Low 25
                        }
                      },
                      neighCellList 
                      {
                        {
                          bandClass bc0,
                          neighCellsPerFreqList 
                          {
                            {
                              arfcn 40,
                              physCellIdList 
                              {
                                120,
                                104,
                                108,
                                112,
                                116,
                                124
                              }
                            }
                          }
                        }
                      },
                      t-ReselectionCDMA2000 2
                    }
                  },
                  cellReselectionParameters1XRTT-v920 
                  {
                    neighCellList-v920 
                    {
                      {
                        neighCellsPerFreqList-v920 
                        {
                          {
                            physCellIdList-v920 
                            {
                              8,
                              10,
                              20,
                              30
                            }
                          }
                        }
                      },
                      {
                        neighCellsPerFreqList-v920 
                        {
                          {
                            physCellIdList-v920 
                            {
                              10,
                              11,
                              12,
                              13,
                              14
                            }
                          }
                        }
                      }
                    }
                  },
                  csfb-RegistrationParam1XRTT-v920 
                  {
                    powerDownReg-r9 true
                  },
                  ac-BarringConfig1XRTT-r9 
                  {
                    ac-Barring0to9-r9 1,
                    ac-Barring10-r9 1,
                    ac-Barring11-r9 1,
                    ac-Barring12-r9 0,
                    ac-Barring13-r9 7,
                    ac-Barring14-r9 7,
                    ac-Barring15-r9 7,
                    ac-BarringMsg-r9 1,
                    ac-BarringReg-r9 0,
                    ac-BarringEmg-r9 0
                  }
                }
            }
          }
      }
}
*/
#define DECLARE_LTE_RRC_MEAS_TEST_SIB8_EXTENDED_NEIGH_CELLS_NO_BANDCLASS \
static const uint8 lte_rrc_meas_test_sib8_extended_neigh_cells_no_bandclass[] = \
{ \
  0x00, 0x1B, 0xB1, 0x7A, 0x62, 0x53, 0x92, 0xFA, 0xA0, 0x00, 0x40, 0x00, 0x7F, \
  0x80, 0x00, 0x10, 0x80, 0x40, 0x6C, 0xB2, 0x00, 0x00, 0x28, 0x53, 0xC1, 0xA0, \
  0xD8, 0x70, 0x3A, 0x1F, 0x10, 0x14, 0x48, 0xE2, 0x04, 0x04, 0x02, 0x82, 0x81, \
  0xE0, 0x28, 0x28, 0x16, 0x0C, 0x06, 0x83, 0x81, 0x24, 0x7F, 0xC8, 0x00, 0x00, \
  0x00, 0x00, 0x00, \
}

extern lte_rrc_osys_SystemInformationBlockType1 *lte_rrc_meas_test_sib1;
extern lte_rrc_osys_SystemInformationBlockType3 *lte_rrc_meas_test_sib3;
extern lte_rrc_osys_SystemInformationBlockType4 *lte_rrc_meas_test_sib4;
extern lte_rrc_osys_SystemInformationBlockType5 *lte_rrc_meas_test_sib5;
extern lte_rrc_osys_SystemInformationBlockType6 *lte_rrc_meas_test_sib6;
extern lte_rrc_osys_SystemInformationBlockType7 *lte_rrc_meas_test_sib7;
extern lte_rrc_osys_SystemInformationBlockType8 *lte_rrc_meas_test_sib8;
extern lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib1_ptr;
extern lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib3_ptr;
extern lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib4_ptr;
extern lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib5_ptr;
extern lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib6_ptr;
extern lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib7_ptr;
extern lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib8_ptr;
extern lte_rrc_osys_RRCConnectionReconfiguration  *lte_rrc_meas_test_meas_config;
extern lte_rrc_osys_UL_DCCH_Message         *ul_dcch_msg_ptr;
extern lte_rrc_camped_status_e lte_rrc_meas_test_camped_status;
extern stm_state_t lte_rrc_meas_test_ctrl_state;
extern stm_state_t lte_rrc_meas_test_cep_state;
extern boolean lte_rrc_meas_test_lte_band_is_supported;
extern boolean lte_rrc_meas_test_gsm_band_is_supported;
extern boolean lte_rrc_meas_test_cdma_1x_band_is_supported;
extern boolean lte_rrc_meas_test_cdma_do_band_is_supported;
extern lte_rrc_cell_id_s lte_rrc_meas_test_cell_id; 
extern boolean lte_rrc_meas_test_sib5_sched_and_rcvd;
/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES
===========================================================================*/

extern void lte_rrc_meas_test_init(void);
extern void lte_rrc_meas_test_get_meas_sm_to_idle_camped(void);
extern void lte_rrc_meas_test_free_sib_ptr(void);
extern void lte_rrc_meas_test_free_reconfig_msg_ptr(void);

extern lte_errno_e lte_rrc_meas_test_stub__csp_get_serving_cell_params_failure
(
  lte_rrc_cell_info_s *cell_info_ptr
);

extern lte_errno_e lte_rrc_meas_test_stub__csp_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr
);

extern lte_errno_e lte_rrc_meas_test_stub__csp_get_serving_cell_params1
(
  lte_rrc_cell_info_s *cell_info_ptr
);


extern lte_rrc_revoke_priority_e lte_rrc_meas_test_stub__get_revoke_priority_success
(
  lte_rrc_revoke_trigger_e trigger /*!< Revoke trigger */
);

extern lte_rrc_revoke_priority_e lte_rrc_meas_test_stub__get_revoke_priority_failure
(
  lte_rrc_revoke_trigger_e trigger /*!< Revoke trigger */
);

extern lte_errno_e lte_rrc_meas_test_stub__get_selected_plmn
(
  lte_rrc_plmn_s *selected_plmn_ptr, /*!< Pointer to selected PLMN to be filled
                                          in */
  uint32 *selected_plmn_index_ptr /*!< Pointer to selected PLMN index to be 
                                       filled in */
);

extern lte_errno_e lte_rrc_meas_test_stub__get_selected_plmn_for_depri
(
  lte_rrc_plmn_s *selected_plmn_ptr, /*!< Pointer to selected PLMN to be filled
                                          in */
  uint32 *selected_plmn_index_ptr /*!< Pointer to selected PLMN index to be 
                                       filled in */
);

extern lte_errno_e lte_rrc_meas_test_stub__get_selected_plmn_for_depri1
(
  lte_rrc_plmn_s *selected_plmn_ptr, /*!< Pointer to selected PLMN to be filled
                                          in */
  uint32 *selected_plmn_index_ptr /*!< Pointer to selected PLMN index to be 
                                       filled in */
);

extern lte_errno_e lte_rrc_meas_test_stub__get_selected_plmn_for_depri2
(
  lte_rrc_plmn_s *selected_plmn_ptr, /*!< Pointer to selected PLMN to be filled
                                          in */
  uint32 *selected_plmn_index_ptr /*!< Pointer to selected PLMN index to be 
                                       filled in */
);

extern lte_errno_e lte_rrc_meas_test_stub__get_selected_plmn_for_depri3
(
  lte_rrc_plmn_s *selected_plmn_ptr, /*!< Pointer to selected PLMN to be filled
                                          in */
  uint32 *selected_plmn_index_ptr /*!< Pointer to selected PLMN index to be 
                                       filled in */
);

extern sys_sband_e_type lte_rrc_meas_test_stub__get_wcdma_band_from_uarfcn
(
   lte_uarfcn_t dl_uarfcn, /*!< DL UARFCN */
   lte_rrc_plmn_s *camped_plmn_ptr /*!< Ptr to camped PLMN */
);

extern sys_sband_tds_e_type lte_rrc_meas_test_stub__get_tdscdma_band_from_uarfcn
(
   lte_uarfcn_t dl_uarfcn, /*!< DL UARFCN */
   lte_rrc_plmn_s *camped_plmn_ptr /*!< Ptr to camped PLMN */
);

extern boolean lte_rrc_meas_test_stub__b12_to_b17_conversion_is_needed
(
  void
);

extern boolean lte_rrc_meas_test_stub__b12_to_b17_conversion_is_not_needed
(
  void
);

extern lte_3gpp_release_ver_e lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel8
(
  void
);

extern lte_3gpp_release_ver_e lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel9
(
  void
);

extern lte_3gpp_release_ver_e lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel10
(
  void
);

extern lte_3gpp_release_ver_e lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel11
(
  void
);
extern void lte_rrc_meas_test_decode_sib1
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
);

extern void lte_rrc_meas_test_decode_sib3
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
);

extern void lte_rrc_meas_test_decode_sib4
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
);

extern void lte_rrc_meas_test_decode_sib5
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
);

extern void lte_rrc_meas_test_decode_sib6
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
);

extern void lte_rrc_meas_test_decode_sib7
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
);

extern void lte_rrc_meas_test_decode_sib8
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
);

extern void lte_rrc_meas_test_decode_si
(
  void *encoded_bytes,  /*!< Ptr to encoded bytes */
  uint32 encoded_length  /*!< Length of encoded string (in bytes) */
);

extern void lte_rrc_meas_test_decode_meas_config
(
  void *encoded_bytes,   /*!< Ptr to encoded bytes */
  uint32 encoded_length, /*!< Length of encoded string (in bytes) */
  lte_rrc_osys_MeasConfig **vmc_ptr
);

extern void lte_rrc_verify_serving_cell_info
(
  const lte_cphy_idle_meas_cfg_req_s *cfg_req_ptr
);

extern void lte_rrc_verify_common_resel_info
(
  const lte_cphy_idle_meas_cfg_req_s *cfg_req_ptr
);

extern void lte_rrc_verify_serving_freq_info
(
  const lte_cphy_idle_meas_cfg_req_s *cfg_req_ptr
);

extern void lte_rrc_verify_default_sib3_param
(
  const lte_cphy_idle_meas_cfg_req_s *cfg_req_ptr
);

extern void  lte_rrc_verify_intra_freq_info
(
  const lte_cphy_idle_meas_cfg_req_s *cfg_req_ptr
);

extern void lte_rrc_meas_test_free_unused_memory_in_sib8
(
  boolean sys_time_memory,
  boolean free_hrpd_memory,
  boolean free_1x_memory
);

#if 0
extern void lte_rrc_verify_cdma_bandclass_info
(
  const lte_cphy_meas_irat_cdma_info_s *cdma_info_ptr
);

extern void lte_rrc_verify_cdma_bandclass_invalid
(
  const lte_cphy_meas_irat_cdma_info_s *cdma_info_ptr
);

extern void lte_rrc_verify_cdma_neigh_cell_list
(
  const lte_cphy_meas_irat_cdma_info_s *cdma_info_ptr
);

extern void lte_rrc_verify_cdma_neigh_cell_list_for_dup_entries
(
  const lte_cphy_meas_irat_cdma_info_s *cdma_info_ptr
);

extern void lte_rrc_verify_cdma_neigh_cell_list_for_dup_entries_1
(
  const lte_cphy_meas_irat_cdma_info_s *cdma_info_ptr
);

extern void lte_rrc_verify_cdma_neigh_cell_list_invalid
(
  const lte_cphy_meas_irat_cdma_info_s *cdma_info_ptr
);

extern void lte_rrc_meas_test_populate_max_cdma_neigh_cells
(
  lte_rrc_osys_NeighCellListCDMA2000 *neighCellList,
  boolean test_invalid_neigh_cells
);

extern void lte_rrc_meas_test_verify_max_cdma_neigh_cells
(
  const lte_cphy_meas_irat_cdma_info_s *cdma_info_ptr,
  boolean max_cells_exceeded
);

extern void lte_rrc_meas_test_populate_max_wcdma_neigh_cells
(
  lte_rrc_osys_CarrierFreqListUTRA_FDD *neigh_list,
  boolean test_excess_neigh_cells
);

extern void lte_rrc_meas_test_verify_max_wcdma_neigh_cells
(
  const lte_cphy_meas_irat_wcdma_resel_info_s *wcdma_info_ptr
);

extern void lte_rrc_meas_test_populate_max_tdscdma_neigh_cells
(
  lte_rrc_osys_CarrierFreqListUTRA_TDD *neigh_list,
  boolean test_excess_neigh_cells
);

extern void lte_rrc_meas_test_verify_max_tdscdma_neigh_cells
(
  const lte_cphy_meas_irat_tdscdma_resel_info_s *tdscdma_info_ptr
);

extern void lte_rrc_meas_test_populate_max_gsm_neigh_freq_groups
(
  lte_rrc_osys_CarrierFreqsInfoListGERAN *neigh_freq_group_list
);

extern void lte_rrc_meas_test_verify_max_gsm_neigh_freq_groups
(
  const lte_cphy_meas_irat_gsm_info_s *gsm_info_ptr
);

extern void lte_rrc_meas_test_populate_max_gsm_neigh_freqs_in_bitmap
(
  lte_rrc_osys_CarrierFreqsInfoListGERAN *neigh_freq_group_list
);

extern void lte_rrc_meas_test_verify_max_gsm_neigh_freqs_in_bitmap
(
  const lte_cphy_meas_irat_gsm_info_s *gsm_info_ptr
);

extern void lte_rrc_meas_test_populate_max_gsm_neigh_freqs
(
  lte_rrc_osys_CarrierFreqsInfoListGERAN *neigh_freq_group_list
);

extern void lte_rrc_meas_test_verify_max_gsm_neigh_freqs
(
  const lte_cphy_meas_irat_gsm_info_s *gsm_info_ptr
);

extern void lte_rrc_meas_test_populate_max_gsm_freqs_in_measObj_using_bitmap
(
  lte_rrc_osys_CarrierFreqsGERAN *freq_list
);

extern void lte_rrc_meas_test_verify_max_gsm_freqs_in_measObj_using_bitmap
(
  const lte_l1_gsm_arfcn_t *gsm_freq_list
);
#endif

extern void lte_rrc_meas_test_get_meas_sm_to_connected
(
  void
);

extern void lte_rrc_meas_test_send_phy_cnf_and_verify_meas_cfg_cnfi
(
  void
);

extern uint8 lte_rrc_meas_test_stub__get_fgi_bit_not_supported
(
  uint8 bit_pos
);

extern uint8 lte_rrc_meas_test_stub__get_fgi_bit_not_supported_except_utran_bit
(
  uint8 bit_pos
);

extern uint8 lte_rrc_meas_test_stub__get_fgi_bit_cgi_not_supported
(
  uint8 bit_pos
);

extern uint8 lte_rrc_meas_test_stub__get_fgi_bit_son_not_supported
(
  uint8 bit_pos
);

extern void lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf
(
  void
);

extern void lte_rrc_meas_test_send_phy_serv_cell_meas_req_and_cnf
(
  void
);

extern lte_rrc_div_duplex_e lte_rrc_meas_test_stub__get_mode_tdd_for_last_camped_cell
(
  void
);


extern boolean lte_rrc_meas_test_csp_tdscdma_is_supported
(
  void
);

extern const lte_rrc_irat_to_tds_prev_acq_freq_s* lte_rrc_irat_to_tds_mgr_irat_acq_db_stub_1
(
  void
);

extern const lte_rrc_irat_to_tds_prev_acq_freq_s* lte_rrc_irat_to_tds_mgr_irat_acq_db_stub_2
(
  void
);


extern boolean lte_rrc_meas_test_csp_wcdma_is_supported
(
  void
);

#if 0
extern sys_sband_e_type lte_rrc_meas_test_stub__get_gsm_band_from_arfcn
(
  geran_grr_band_ind_e    band_ind,
  uint16                  arfcn,
  boolean                 *gsm_band_valid_ptr
);
#endif

extern uint8 lte_rrc_meas_test_stub__get_irat_fgi_bit_not_supported
(
  uint8 bit_pos
);

extern boolean lte_rrc_meas_test_stub__ctlr_irat_cgi_in_progress
(
    void
);
extern boolean lte_rrc_meas_test_stub__irat_proc_is_in_progress
(
    void
);
extern boolean lte_rrc_meas_test_stub__irat_proc_is_in_progress_false
(
    void
);

extern boolean lte_rrc_meas_test_stub__csg_in_whitelist
(
  const lte_rrc_plmn_s *plmn_ptr, /*!< PLMN ptr */
  sys_csg_id_type csg_id, /*!< CSG id */
  lte_earfcn_t freq /*!< Freq */
);

extern void lte_rrc_meas_test_cphy_rf_status_available
( 
  lte_rrc_rf_status_info_s *info_ptr 
);

extern lte_rrc_cell_type_e lte_rrc_meas_test_freq_match_with_pcell_scell
(
  lte_earfcn_t freq, 
  lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs *reconfig_ptr
);

extern lte_rrc_cell_type_e lte_rrc_meas_test_freq_match_with_pcell_scell_none
(
  lte_earfcn_t freq, 
  lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs *reconfig_ptr
);


extern boolean lte_rrc_meas_test_stub__csg_not_in_whitelist
(
  const lte_rrc_plmn_s *plmn_ptr, /*!< PLMN ptr */
  sys_csg_id_type csg_id, /*!< CSG id */
  lte_earfcn_t freq /*!< Freq */
);

extern void lte_rrc_meas_test_set_wcdma_rat_support
(
  boolean val
);

extern void lte_rrc_meas_test_set_tdscdma_rat_support
(
  boolean val
);

#endif /* LTE_RRC_MEAS_TEST_MSG_H */
