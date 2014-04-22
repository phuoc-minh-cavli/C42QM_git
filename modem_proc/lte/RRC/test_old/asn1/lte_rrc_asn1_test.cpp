/*!
  @file
  lte_rrc_asn1_test.cpp

  @brief 
  Unit Test routines to test working of ASN.1 encode and decode functions 
   
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

$Header: 

when       who      what, where, why
--------   ---      ----------------------------------------------------------
06/02/10   aramani  Tweaked to fit conventions of lte_rrc_osys_asn1util.c 
05/18/08   da       Initial Revision
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_osys_asn1.h"
#include "lte_rrc_osys_asn1MemFree.h"
#include "lte_rrc_osys_asn1Dec.h"
#include "lte_rrc_osys_asn1Enc.h"

//#include "lte_rrc_mem_utili.h"
#include "lte_static_rrc_mem_util.h"
//#include "lte_static_rrc_mem_utili.h"

#include "rtxError.h"
#ifdef FEATURE_Q6SIM
#error code not present
#endif 
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_utfi.h"
#ifdef T_WINNT
#error code not present
#else
#define GetTickCount()  0
#endif // T_WINNT

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define LTE_RRC_ASN1_MAX_LOOP_COUNT 5

/*! @brief Typedef for variables internal to module lte_rrc_asn1_test.cpp
*/
typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */
} lte_rrc_asn1_test_s;

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/


/*extern "C"
{
extern lte_rrc_mem_util_s lte_rrc_mem_util;
}*/


/*! @brief Variables internal to module lte_rrc_asn1_test.cpp
*/
static lte_rrc_asn1_test_s lte_rrc_asn1_test_data;


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/


/*===========================================================================

                                UNIT TEST

===========================================================================*/
/*===========================================================================
          DL-DCCH RRC Connection Reconfiguration Message 
===========================================================================*/
/*
ReconfigMessage:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 1,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
              measurementConfiguration 
              {
                measIdToAddModifyList 
                {
                  {
                    measId 1,
                    measObjectId 1,
                    reportConfigId 1
                  }
                }
              },
              nas-DedicatedInformationList 
              {
                '010203'H,
                '040506'H,
                '070809'H
              },
              radioResourceConfiguration 
              {
                srb-ToAddModifyList 
                {
                  {
                    srb-Identity 2,
                    rlc-Configuration defaultValue : NULL,
                    logicalChannelConfig defaultValue : NULL
                  }
                },
                drb-ToAddModifyList 
                {
                  {
                    eps-BearerIdentity 3,
                    drb-Identity 3,
                    pdcp-Configuration 
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
                    rlc-Configuration um-Bi-Directional : 
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
                        prioritizedBitRate kBps16,
                        bucketSizeDuration ms50,
                        logicalChannelGroup 1
                      }
                    }
                  }
                },
                mac-MainConfig explicitValue : 
                  {
                    dl-SCH-Configuration 
                    {
                    },
                    ul-SCH-Configuration 
                    {
                      maxHARQ-Tx n1,
                      periodicBSR-Timer sf5,
                      retxBSR-Timer sf320,
                      ttiBundling TRUE
                    },
                    drx-Configuration enable : 
                      {
                        onDurationTimer psf1,
                        drx-InactivityTimer psf1,
                        drx-RetransmissionTimer sf2,
                        longDRX-CycleStartOffset sf10 : 0,
                        shortDRX enable : 
                          {
                            shortDRX-Cycle sf2,
                            drxShortCycleTimer 1
                          }
                      },
                    timeAlignmentTimerDedicated sf500,
                    phr-Configuration enable : 
                      {
                        periodicPHR-Timer sf10,
                        prohibitPHR-Timer sf0,
                        dl-PathlossChange dB1
                      }
                  },
                physicalConfigDedicated 
                {
                  pdsch-Configuration 
                  {
                    p-a dB-6
                  },
                  pucch-Configuration 
                  {
                    ackNackRepetition enable : 
                      {
                        repetitionFactor n2
                      },
                    tddAckNackFeedbackMode bundling
                  },
                  pusch-Configuration 
                  {
                    deltaOffset-ACK-Index 0,
                    deltaOffset-RI-Index 0,
                    deltaOffset-CQI-Index 0
                  },
                  uplinkPowerControl 
                  {
                    p0-UePUSCH 0,
                    deltaMCS-Enabled en0,
                    accumulationEnabled TRUE,
                    p0-uePUCCH 5,
                    pSRS-Offset 2
                  },
                  tpc-PDCCH-ConfigPUCCH enable : 
                    {
                      tpc-RNTI '00000000 00000000'B,
                      tpc-Index indexOfFormat3 : 2
                    },
                  tpc-PDCCH-ConfigPUSCH disable : NULL,
                  cqi-Reporting 
                  {
                    cqi-ReportingModeAperiodic rm12,
                    nomPDSCH-RS-EPRE-Offset 0,
                    cqi-ReportingPeriodic enable : 
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
                  soundingRsUl-Config enable : 
                    {
                      srsBandwidth bw0,
                      srsHoppingBandwidth hbw1,
                      frequencyDomainPosition 0,
                      duration FALSE,
                      srs-ConfigurationIndex 0,
                      transmissionComb 0,
                      cyclicShift cs0
                    },
                  antennaInformation defaultValue : NULL,
                  schedulingRequestConfig enable : 
                    {
                      sr-PUCCH-ResourceIndex 0,
                      sr-ConfigurationIndex 0,
                      dsr-TransMax n4
                    }
                }
              }
            }
      }
}
*/

byte dl_dcch_reconfig_msg[] =   {0x22, 0x16, 0x00, 0x80, 0x00, 0x00, 0x04, 
  0x06, 0x02, 0x04, 0x06, 0x06, 0x08, 0x0A, 0x0C, 0x06, 0x0E, 0x10, 0x12, 0xD4, 
  0xF8, 0x3E, 0x62, 0x50, 0xA0, 0x00, 0x00, 0x08, 0x2F, 0x44, 0x82, 0x7E, 0x00, 
  0x38, 0x00, 0x80, 0x40, 0x04, 0x01, 0xFF, 0x88, 0x00, 0x08, 0x74, 0xA0, 0x00, 
  0x01, 0x41, 0x80, 0x60, 0x4A, 0x00, 0x62, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00};


byte sib345_8_5_long_msg[] = {
  0x01, 0x05, 0x00, 0x00, 0x42, 0x00, 0x0F, 0x29, 0xE0, 0x00, 0x00, 0x4F, 0xE0, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
  0x9F, 0x80, 0x00, 0x14, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x05, 0x3C, 0x00, 0x00, 0x00,
  0x3F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF8, 0x00, 
  0x01, 0x4F, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00};


void lte_rrc_asn1_test_print_heap_status(void)
{


 /* TF_MSG("*** RRC heap total blocks %d total_bytes %d used bytes %d",
         lte_rrc_mem_util.dd_ptr->lte_rrc_heap.total_blocks,
         lte_rrc_mem_util.dd_ptr->lte_rrc_heap.total_bytes,
         lte_rrc_mem_util.dd_ptr->lte_rrc_heap.used_bytes);

  TF_MSG("*** RRC heap max used %d max requested %d ",
         lte_rrc_mem_util.dd_ptr->lte_rrc_heap.max_used,
         lte_rrc_mem_util.dd_ptr->lte_rrc_heap.max_request);


  TF_MSG("*** RRC small heap total blocks %d total_bytes %d used bytes %d",
         lte_rrc_mem_util.dd_ptr->lte_rrc_small_heap.total_blocks,
         lte_rrc_mem_util.dd_ptr->lte_rrc_small_heap.total_bytes,
         lte_rrc_mem_util.dd_ptr->lte_rrc_small_heap.used_bytes);

  TF_MSG("*** RRC small heap max used %d max requested %d ",
         lte_rrc_mem_util.dd_ptr->lte_rrc_small_heap.max_used,
         lte_rrc_mem_util.dd_ptr->lte_rrc_small_heap.max_request);*/

}

/*===========================================================================
  LteRrcAsn1OsysBasicDlDcchDecodeTest
===========================================================================*/
/*!
  @brief
  Tests Objective Systems ASN.1 basic DL DCCH Decoding ...

*/
/*=========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcAsn1OsysBasicDlDcchDecodeTest);

void LteRrcAsn1OsysBasicDlDcchDecodeTest::Setup()
{
  lte_rrc_mem_init_heap();
  lte_rrc_osys_init();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();
#endif

}

void LteRrcAsn1OsysBasicDlDcchDecodeTest::Test()
{
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg;
#ifdef T_WINNT
  #error code not present
#endif

#ifdef FEATURE_Q6SIM
  #error code not present
#endif 

  int status = 0;

  TF_MSG("Objective Systems ASN.1 Test :: Basic DL DCCH decode begin.");

#ifdef T_WINNT
  #error code not present
#endif


#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();

  lte_rrc_asn1_test_print_heap_status();
#endif

#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  dl_dcch_msg = (lte_rrc_osys_DL_DCCH_Message *)
  lte_rrc_osys_asn1_decode_pdu(&dl_dcch_reconfig_msg, sizeof(dl_dcch_reconfig_msg), DL_DCCH_Message, &status);

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 

#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();
#endif

#ifdef T_WINNT
  #error code not present
#endif

  if (0 != status) 
  {
    LTE_ERROR("Decoding failure",0,0,0);
    rtxErrPrint(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt));
  }

  TF_ASSERT(status == 0);

#ifdef T_WINNT
#error code not present
#endif

#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  /* Now free decoded message */
  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg, lte_rrc_osys_DL_DCCH_Message_PDU);

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif
 
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();
#endif

  TF_MSG("Objective Systems ASN.1 Test :: Basic DL DCCH decode end. \n");
}

void LteRrcAsn1OsysBasicDlDcchDecodeTest::Teardown()
{
  lte_rrc_osys_free_context();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();
  TF_ASSERT(lte_rrc_mem_util_get_rrc_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_small_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_sib_heap_used_bytes() == 0);
#endif

}


/*===========================================================================
  LteRrcAsn1OsysBasicDlDcchDecodeLoopTest
===========================================================================*/
/*!
  @brief
  Tests Objective Systems ASN.1 basic DL DCCH Decoding ...

*/
/*=========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcAsn1OsysBasicDlDcchDecodeLoopTest);

void LteRrcAsn1OsysBasicDlDcchDecodeLoopTest::Setup()
{
  lte_rrc_mem_init_heap();
  lte_rrc_osys_init();
}

void LteRrcAsn1OsysBasicDlDcchDecodeLoopTest::Test()
{
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg;

#ifdef FEATURE_Q6SIM
  #error code not present
#endif 

  int status = 0;
  int i = 0;


  TF_MSG("Objective Systems ASN.1 Loop Test :: Basic DL DCCH decode begin.");


  for (i = 0; i < LTE_RRC_ASN1_MAX_LOOP_COUNT; i++ ) 
  {
#ifdef FEATURE_Q6SIM
    #error code not present
#endif

    dl_dcch_msg = (lte_rrc_osys_DL_DCCH_Message *)
    lte_rrc_osys_asn1_decode_pdu(&dl_dcch_reconfig_msg, sizeof(dl_dcch_reconfig_msg), DL_DCCH_Message, &status);
    
    /* Now free decoded message */
    lte_rrc_osys_asn1_free_pdu(dl_dcch_msg, lte_rrc_osys_DL_DCCH_Message_PDU);
    
#ifdef FEATURE_Q6SIM  
    #error code not present
#endif 
  }

#ifdef FEATURE_Q6SIM
#error code not present
#endif

  TF_MSG("Objective Systems ASN.1 Loop Test :: Basic DL DCCH decode end. \n");
}

void LteRrcAsn1OsysBasicDlDcchDecodeLoopTest::Teardown()
{
  lte_rrc_osys_free_context();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  TF_ASSERT(lte_rrc_mem_util_get_rrc_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_small_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_sib_heap_used_bytes() == 0);
#endif

}


/*===========================================================================
  LteRrcAsn1OsysBasicDlDcchDecodeLoopTest
===========================================================================*/
/*!
  @brief
  Tests Objective Systems ASN.1 basic DL DCCH Decoding ...

*/
/*=========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcAsn1OsysBasicDlDcchDecodeLoopTest2);

void LteRrcAsn1OsysBasicDlDcchDecodeLoopTest2::Setup()
{
  lte_rrc_mem_init_heap();
  lte_rrc_osys_init();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();
#endif
}


void LteRrcAsn1OsysBasicDlDcchDecodeLoopTest2::Test()
{
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_1;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_2;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_3;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_4;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_5;


#ifdef FEATURE_Q6SIM
  #error code not present
#endif 

  int status = 0;
  int i = 0;


  TF_MSG("Objective Systems ASN.1 Loop Test :: Basic DL DCCH decode begin.");


  for (i = 0; i < LTE_RRC_ASN1_MAX_LOOP_COUNT; i++ ) 
  {

    TF_MSG("Objective Systems ASN.1 Loop Test :: i %d 0.", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif

    dl_dcch_msg_1 = (lte_rrc_osys_DL_DCCH_Message *)
    lte_rrc_osys_asn1_decode_pdu(&dl_dcch_reconfig_msg, sizeof(dl_dcch_reconfig_msg), DL_DCCH_Message, &status);
    
    TF_MSG("Objective Systems ASN.1 Loop Test :: i %d 1.", i);

#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif

    dl_dcch_msg_2 = (lte_rrc_osys_DL_DCCH_Message *)
    lte_rrc_osys_asn1_decode_pdu(&dl_dcch_reconfig_msg, sizeof(dl_dcch_reconfig_msg), DL_DCCH_Message, &status);

    TF_MSG("Objective Systems ASN.1 Loop Test :: i %d 2.", i);

#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif

    dl_dcch_msg_3 = (lte_rrc_osys_DL_DCCH_Message *)
    lte_rrc_osys_asn1_decode_pdu(&dl_dcch_reconfig_msg, sizeof(dl_dcch_reconfig_msg), DL_DCCH_Message, &status);

    TF_MSG("Objective Systems ASN.1 Loop Test :: i %d 3.", i);

#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif

    dl_dcch_msg_4 = (lte_rrc_osys_DL_DCCH_Message *)
    lte_rrc_osys_asn1_decode_pdu(&dl_dcch_reconfig_msg, sizeof(dl_dcch_reconfig_msg), DL_DCCH_Message, &status);

    TF_MSG("Objective Systems ASN.1 Loop Test :: i %d 4.", i);

#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif
    dl_dcch_msg_5 = (lte_rrc_osys_DL_DCCH_Message *)
    lte_rrc_osys_asn1_decode_pdu(&dl_dcch_reconfig_msg, sizeof(dl_dcch_reconfig_msg), DL_DCCH_Message, &status);

    TF_MSG("Objective Systems ASN.1 Loop Test :: i %d 5.", i);

#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif

    /* Now free decoded message */
    lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_1, lte_rrc_osys_DL_DCCH_Message_PDU);
    TF_MSG("Objective Systems ASN.1 Loop Test :: i %d free 1.", i);

#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif

    lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_2, lte_rrc_osys_DL_DCCH_Message_PDU);
    
    TF_MSG("Objective Systems ASN.1 Loop Test :: i %d free 2.", i);

#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif

    lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_3, lte_rrc_osys_DL_DCCH_Message_PDU);


    TF_MSG("Objective Systems ASN.1 Loop Test :: i %d free 3.", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif
    lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_4, lte_rrc_osys_DL_DCCH_Message_PDU);

    TF_MSG("Objective Systems ASN.1 Loop Test :: i %d free 4.", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif

    lte_rrc_osys_asn1_free_pdu(dl_dcch_msg_5, lte_rrc_osys_DL_DCCH_Message_PDU);

    TF_MSG("Objective Systems ASN.1 Loop Test :: i %d free 5.", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif

  }


  TF_MSG("Objective Systems ASN.1 Loop Test :: Basic DL DCCH decode end. \n");
}

void LteRrcAsn1OsysBasicDlDcchDecodeLoopTest2::Teardown()
{
  lte_rrc_osys_free_context();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();
  TF_ASSERT(lte_rrc_mem_util_get_rrc_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_small_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_sib_heap_used_bytes() == 0);
#endif

}


/*===========================================================================
  LteRrcAsn1OsysLongBcchDlSchDecodeTest
===========================================================================*/
/*!
  @brief
  Tests Objective Systems ASN.1 log BCCH DL SCH Decoding ...

*/
/*=========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcAsn1OsysLongBcchDlSchDecodeTest);

void LteRrcAsn1OsysLongBcchDlSchDecodeTest::Setup()
{
  lte_rrc_mem_init_heap();
  lte_rrc_osys_init();
}

void LteRrcAsn1OsysLongBcchDlSchDecodeTest::Test()
{
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_msg;
#ifdef T_WINNT
  #error code not present
#endif

#ifdef FEATURE_Q6SIM
  #error code not present
#endif 

  int status = 0;

  TF_MSG("Objective Systems ASN.1 Test :: Long BCCH DL SCH decode begin.");

#ifdef T_WINNT
  #error code not present
#endif

#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  bcch_dl_sch_msg = (lte_rrc_osys_BCCH_DL_SCH_Message *)
  lte_rrc_osys_asn1_decode_pdu(&sib345_8_5_long_msg, sizeof(sib345_8_5_long_msg), lte_rrc_osys_BCCH_DL_SCH_Message_PDU, &status);

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 


#ifdef T_WINNT
  #error code not present
#endif

  if (0 != status) 
  {
    LTE_ERROR("Decoding failure",0,0,0);
    rtxErrPrint(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt));
  }

  TF_ASSERT(status == 0);

#ifdef T_WINNT
#error code not present
#endif

#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  /* Now free decoded message */
  lte_rrc_osys_asn1_free_pdu(bcch_dl_sch_msg, lte_rrc_osys_BCCH_DL_SCH_Message_PDU);


#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 

  TF_MSG("Objective Systems ASN.1 Test :: Long BCCH DL SCH decode end. \n");
}

void LteRrcAsn1OsysLongBcchDlSchDecodeTest::Teardown()
{
  lte_rrc_osys_free_context();

#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  TF_ASSERT(lte_rrc_mem_util_get_rrc_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_small_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_sib_heap_used_bytes() == 0);
#endif
}

/*===========================================================================
  LteRrcAsn1OsysDlDcchDecodeAndPrintTest
===========================================================================*/
/*!
  @brief
  Tests Objective Systems ASN.1 basic DL DCCH Decoding and printing the 
  decoded DL DCCH message ...

*/
/*=========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcAsn1OsysDlDcchDecodeAndPrintTest);

void LteRrcAsn1OsysDlDcchDecodeAndPrintTest::Setup()
{
  lte_rrc_mem_init_heap();
  lte_rrc_osys_init();
}

void LteRrcAsn1OsysDlDcchDecodeAndPrintTest::Test()
{
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg;
  int status = 0;

#ifdef FEATURE_Q6SIM
  #error code not present
#endif 

  TF_MSG(" Objective Systems ASN.1 Test :: Basic DL DCCH decode and Print begin. \n");

  dl_dcch_msg = (lte_rrc_osys_DL_DCCH_Message *)
  lte_rrc_osys_asn1_decode_pdu(&dl_dcch_reconfig_msg, sizeof(dl_dcch_reconfig_msg), DL_DCCH_Message, &status);
  if (0 != status)
  {
    LTE_ERROR("Decoding failure",0,0,0);
    rtxErrPrint(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt));
  }

  TF_ASSERT(status == 0);
#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  /* Now print decoded msg */
  status = LTE_RRC_OSYS_ASN1_PRINT_PDU("DL_DCCH_Message", &dl_dcch_msg, 
									   DL_DCCH_Message);
  if (0 != status)
  {				  	  
	TF_ASSERT(0);
  }

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 


  /* Now free decoded message */
  lte_rrc_osys_asn1_free_pdu(dl_dcch_msg, lte_rrc_osys_DL_DCCH_Message_PDU);

  TF_MSG("Objective systems ASN.1 Test :: Basic DL DCCH decode and Print end. \n");
}

void LteRrcAsn1OsysDlDcchDecodeAndPrintTest::Teardown()
{
  lte_rrc_osys_free_context();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  TF_ASSERT(lte_rrc_mem_util_get_rrc_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_small_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_sib_heap_used_bytes() == 0);
#endif
}



/*===========================================================================
  LteRrcAsn1OsysDlDcchDecodeFailureTest
===========================================================================*/
/*!
  @brief
  Tests Objective Systems ASN.1 DL DCCH Decoding failure ...

*/
/*=========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcAsn1OsysDlDcchDecodeFailureTest);

void LteRrcAsn1OsysDlDcchDecodeFailureTest::Setup()
{

  lte_rrc_mem_init_heap();
  lte_rrc_osys_init();
}

void LteRrcAsn1OsysDlDcchDecodeFailureTest::Test()
{
  lte_rrc_osys_DL_DCCH_Message dl_dcch_msg;

  int status = 0;

#ifdef FEATURE_Q6SIM
  #error code not present
#endif 


  TF_MSG("Objective systems ASN.1 Test :: Basic DL DCCH decode failure begin.");

#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  /* Specify the encoded ptr and length of the PER encoded message to be decoded. Pass
     in wrong size so that decoding would fail.  */
  pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), (OSOCTET*) &dl_dcch_reconfig_msg, (sizeof(dl_dcch_reconfig_msg) - 12), FALSE);

  status = asn1PD_lte_rrc_osys_DL_DCCH_Message(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &dl_dcch_msg);

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 

  if (0 != status ) 
  {
    LTE_ERROR("Decoding failure",0,0,0);
    rtxErrPrint(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt));
  }

  TF_MSG("Objective Systems ASN.1 Test :: Basic DL DCCH decode failure end.\n");
}

void LteRrcAsn1OsysDlDcchDecodeFailureTest::Teardown()
{
  lte_rrc_osys_free_context();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  TF_ASSERT(lte_rrc_mem_util_get_rrc_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_small_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_sib_heap_used_bytes() == 0);
#endif
}

/*===========================================================================
  LteRrcAsn1OsysBasicULDCCHEncodeTest
===========================================================================*/
/*!
  @brief
  Tests Objective Systems ASN.1 basic UL DCCH Encoding ...

*/
/*=========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcAsn1OsysBasicUlDcchEncodeTest);

void LteRrcAsn1OsysBasicUlDcchEncodeTest::Setup()
{
  lte_rrc_mem_init_heap();
  lte_rrc_osys_init();
}

void LteRrcAsn1OsysBasicUlDcchEncodeTest::Test()
{
  lte_rrc_osys_UL_DCCH_Message ul_dcch_msg;
  lte_rrc_osys_MeasurementReport_r8_IEs *meas_rpt_ptr;
  lte_rrc_osys_MeasResultListEUTRA *eutra_list_ptr;
  OSOCTET *encoded_pdu_ptr = NULL;
  int encoded_pdu_length = 0;
  int status = 0;

#ifdef T_WINNT
  #error code not present
#endif

#ifdef FEATURE_Q6SIM
  #error code not present
#endif 


  TF_MSG("Objective Systems ASN.1 Test :: Basic UL DCCH encode begin. \n");

#ifdef T_WINNT
  #error code not present
#endif


#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();
#endif

#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  /* Construct mesurement report UL DCCH message */

  ul_dcch_msg.message.t = T_lte_rrc_osys_UL_DCCH_MessageType_c1;

  ul_dcch_msg.message.u.c1 = 
    (lte_rrc_osys_UL_DCCH_MessageType_c1 * )rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_UL_DCCH_MessageType_c1));
  ul_dcch_msg.message.u.c1->t = T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport;
  
  ul_dcch_msg.message.u.c1->u.measurementReport = 
    (lte_rrc_osys_MeasurementReport *)rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasurementReport));
  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.t = 
    T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1;

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1 = 
    (lte_rrc_osys_MeasurementReport_criticalExtensions_c1 *) 
  rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasurementReport_criticalExtensions_c1));

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->t = 
    T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8;

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.measurementReport_r8 = 
    (lte_rrc_osys_MeasurementReport_r8_IEs *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasurementReport_r8_IEs));

  meas_rpt_ptr = 
    ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.measurementReport_r8;


  meas_rpt_ptr->measResults.measId = 4;
  meas_rpt_ptr->measResults.measResultPCell.rsrpResult = 5;
  meas_rpt_ptr->measResults.measResultPCell.rsrqResult = 12;

  meas_rpt_ptr->measResults.extElem1.count = 0;
  meas_rpt_ptr->measResults.extElem1.head = NULL;

  meas_rpt_ptr->measResults.m.measResultNeighCellsPresent = 1;
  meas_rpt_ptr->measResults.measResultNeighCells.t = 
    T_lte_rrc_osys_MeasResults_measResultNeighCells_measResultListEUTRA;
  meas_rpt_ptr->measResults.measResultNeighCells.u.measResultListEUTRA = 
    (lte_rrc_osys_MeasResultListEUTRA *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasResultListEUTRA));

  eutra_list_ptr = 
    meas_rpt_ptr->measResults.measResultNeighCells.u.measResultListEUTRA;

  /* Set the number of elements in list */
  eutra_list_ptr->n = 1;
  eutra_list_ptr->elem = (lte_rrc_osys_MeasResultEUTRA *)  
  rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasResultEUTRA) * eutra_list_ptr->n);

  /* Populate info for neighbor 1 */
  eutra_list_ptr->elem[0].measResult.m.rsrpResultPresent = 1;
  eutra_list_ptr->elem[0].measResult.rsrpResult = 7;
  eutra_list_ptr->elem[0].physCellId = 502;

  /* Populate info for neighbor 2 */
  
  eutra_list_ptr->elem[0].m.cgi_InfoPresent = 1;
  eutra_list_ptr->elem[0].cgi_Info.m.plmn_IdentityListPresent = 0;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.numbits = 28;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[0] = 0xFF;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[1] = 0x00;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[2] = 0xFF;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[3] = 0x00;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.m.mccPresent = 1;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.n = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem =
    (lte_rrc_osys_MCC_MNC_Digit *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), 
                                             (sizeof(lte_rrc_osys_MCC_MNC_Digit) * eutra_list_ptr->elem[0].cgi_Info.cellGlobalId .plmn_Identity.mcc.n));
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[0] = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[1] = 2;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[2] = 1;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.n = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem =
    (lte_rrc_osys_MCC_MNC_Digit *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), 
                                             (sizeof(lte_rrc_osys_MCC_MNC_Digit) * eutra_list_ptr->elem[0].cgi_Info. cellGlobalId.plmn_Identity.mnc.n));  
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[0] = 1;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[1] = 2;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[2] = 3;

  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.numbits = 16;
  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.data[0] = 0xF0;
  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.data[1] = 0x0F;

  eutra_list_ptr->elem[0].measResult.m.rsrpResultPresent = 1;
  eutra_list_ptr->elem[0].measResult.m.rsrqResultPresent = 0;
  eutra_list_ptr->elem[0].measResult.rsrpResult = 10;
  eutra_list_ptr->elem[0].measResult.extElem1.count = 0;
  eutra_list_ptr->elem[0].measResult.extElem1.head = NULL;


  eutra_list_ptr->elem[0].physCellId = 503;

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 

#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();
#endif

#ifdef FEATURE_Q6SIM
  #error code not present
#endif


  /* Specify the encoded ptr and length of the PER encoded message to be decoded.  Specifing
     NULL ptr and length of 0 makes it allocate memory dynamically.  Setting the last
     param to FALSE will make it use unaligned PER  */
  pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), NULL, 0, FALSE);

  status = asn1PE_lte_rrc_osys_UL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);

#ifdef T_WINNT
  #error code not present
#endif 

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 

#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();
#endif

  if (0 != status)
  {
    LTE_ERROR("encoding failure",0,0,0);
    rtxErrPrint(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt));
  }

  TF_ASSERT(status == 0);

#ifdef T_WINNT
  #error code not present
#endif

  /* Get the pointer and length of the encoded msg */
  encoded_pdu_ptr = pe_GetMsgPtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &encoded_pdu_length);

  TF_ASSERT(encoded_pdu_ptr != NULL);
  TF_ASSERT(encoded_pdu_ptr != 0);

  /* Free encoded pdu ptr */
  rtxMemFreePtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), encoded_pdu_ptr);


  /* Now print decoded UL msg */
  status = LTE_RRC_OSYS_ASN1_PRINT_PDU("UL_DCCH_Message", &ul_dcch_msg,
									   UL_DCCH_Message);
  if (0 != status)  
  {
    TF_ASSERT(0);
  }

  /* Free embeded pointers for UL DCCH message */
  asn1Free_lte_rrc_osys_UL_DCCH_Message(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);
  
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();
#endif

  TF_MSG("\n Objective Systems ASN.1 Test :: Basic UL DCCH encode end. \n");
}

void LteRrcAsn1OsysBasicUlDcchEncodeTest::Teardown()
{
  lte_rrc_osys_free_context();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();

  TF_ASSERT(lte_rrc_mem_util_get_rrc_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_small_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_sib_heap_used_bytes() == 0);
#endif
}



/*===========================================================================
  LteRrcAsn1OsysMallocProfileTest
===========================================================================*/
/*!
  @brief
  Tests Objective Systems ASN.1 basic UL DCCH Encoding ...

*/
/*=========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcAsn1OsysMallocProfileTest);

void LteRrcAsn1OsysMallocProfileTest::Setup()
{
  lte_rrc_mem_init_heap();
  lte_rrc_osys_init();
}

void LteRrcAsn1OsysMallocProfileTest::Test()
{
  lte_rrc_osys_UL_DCCH_Message ul_dcch_msg;
  lte_rrc_osys_MeasurementReport_r8_IEs *meas_rpt_ptr;
  lte_rrc_osys_MeasResultListEUTRA *eutra_list_ptr;
  OSOCTET *encoded_pdu_ptr = NULL;
  int encoded_pdu_length = 0;
  int status = 0;

#ifdef T_WINNT
  #error code not present
#endif

#ifdef FEATURE_Q6SIM
  #error code not present
#endif 


  TF_MSG("Objective Systems ASN.1 Test :: Basic UL DCCH encode begin. \n");

#ifdef T_WINNT
  #error code not present
#endif


#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  /* Construct mesurement report UL DCCH message */

  ul_dcch_msg.message.t = T_lte_rrc_osys_UL_DCCH_MessageType_c1;

  ul_dcch_msg.message.u.c1 = 
    (lte_rrc_osys_UL_DCCH_MessageType_c1 * )rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_UL_DCCH_MessageType_c1));

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 
  ul_dcch_msg.message.u.c1->t = T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport;

#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  ul_dcch_msg.message.u.c1->u.measurementReport = 
    (lte_rrc_osys_MeasurementReport *)rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasurementReport));
#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.t = 
    T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1;

#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1 = 
    (lte_rrc_osys_MeasurementReport_criticalExtensions_c1 *) 
  rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasurementReport_criticalExtensions_c1));

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->t = 
    T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8;

#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.measurementReport_r8 = 
    (lte_rrc_osys_MeasurementReport_r8_IEs *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasurementReport_r8_IEs));

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif

  meas_rpt_ptr = 
    ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.measurementReport_r8;


  meas_rpt_ptr->measResults.measId = 4;
  meas_rpt_ptr->measResults.measResultPCell.rsrpResult = 5;
  meas_rpt_ptr->measResults.measResultPCell.rsrqResult = 12;

  meas_rpt_ptr->measResults.extElem1.count = 0;
  meas_rpt_ptr->measResults.extElem1.head = NULL;

  meas_rpt_ptr->measResults.m.measResultNeighCellsPresent = 1;
  meas_rpt_ptr->measResults.measResultNeighCells.t = 
    T_lte_rrc_osys_MeasResults_measResultNeighCells_measResultListEUTRA;
#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  meas_rpt_ptr->measResults.measResultNeighCells.u.measResultListEUTRA = 
    (lte_rrc_osys_MeasResultListEUTRA *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasResultListEUTRA));

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif

  eutra_list_ptr = 
    meas_rpt_ptr->measResults.measResultNeighCells.u.measResultListEUTRA;

  /* Set the number of elements in list */
  eutra_list_ptr->n = 1;

#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  eutra_list_ptr->elem = (lte_rrc_osys_MeasResultEUTRA *)  
  rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasResultEUTRA) * eutra_list_ptr->n);

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif

  /* Populate info for neighbor 1 */
  eutra_list_ptr->elem[0].measResult.m.rsrpResultPresent = 1;
  eutra_list_ptr->elem[0].measResult.rsrpResult = 7;
  eutra_list_ptr->elem[0].physCellId = 502;

  /* Populate info for neighbor 2 */
  
  eutra_list_ptr->elem[0].m.cgi_InfoPresent = 1;
  eutra_list_ptr->elem[0].cgi_Info.m.plmn_IdentityListPresent = 0;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.numbits = 28;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[0] = 0xFF;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[1] = 0x00;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[2] = 0xFF;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[3] = 0x00;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.m.mccPresent = 1;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.n = 3;
#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem =
    (lte_rrc_osys_MCC_MNC_Digit *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), 
                                             (sizeof(lte_rrc_osys_MCC_MNC_Digit) * eutra_list_ptr->elem[0].cgi_Info.cellGlobalId .plmn_Identity.mcc.n));

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[0] = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[1] = 2;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[2] = 1;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.n = 3;

#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem =
    (lte_rrc_osys_MCC_MNC_Digit *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), 
                                             (sizeof(lte_rrc_osys_MCC_MNC_Digit) * eutra_list_ptr->elem[0].cgi_Info. cellGlobalId.plmn_Identity.mnc.n));  

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[0] = 1;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[1] = 2;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[2] = 3;

  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.numbits = 16;
  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.data[0] = 0xF0;
  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.data[1] = 0x0F;

  eutra_list_ptr->elem[0].measResult.m.rsrpResultPresent = 1;
  eutra_list_ptr->elem[0].measResult.m.rsrqResultPresent = 0;
  eutra_list_ptr->elem[0].measResult.rsrpResult = 10;
  eutra_list_ptr->elem[0].measResult.extElem1.count = 0;
  eutra_list_ptr->elem[0].measResult.extElem1.head = NULL;


  eutra_list_ptr->elem[0].physCellId = 503;



#ifdef FEATURE_Q6SIM
  #error code not present
#endif


  /* Specify the encoded ptr and length of the PER encoded message to be decoded.  Specifing
     NULL ptr and length of 0 makes it allocate memory dynamically.  Setting the last
     param to FALSE will make it use unaligned PER  */
  pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), NULL, 0, FALSE);

  status = asn1PE_lte_rrc_osys_UL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);

#ifdef T_WINNT
  #error code not present
#endif 

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 


  if (0 != status)
  {
    LTE_ERROR("encoding failure",0,0,0);
    rtxErrPrint(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt));
  }

  TF_ASSERT(status == 0);

#ifdef T_WINNT
  #error code not present
#endif

  /* Get the pointer and length of the encoded msg */
  encoded_pdu_ptr = pe_GetMsgPtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &encoded_pdu_length);

  TF_ASSERT(encoded_pdu_ptr != NULL);
  TF_ASSERT(encoded_pdu_ptr != 0);

  /* Free encoded pdu ptr */
  rtxMemFreePtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), encoded_pdu_ptr);


  /* Now print decoded UL msg */
  status = LTE_RRC_OSYS_ASN1_PRINT_PDU("UL_DCCH_Message", &ul_dcch_msg,
									   UL_DCCH_Message);
  if (0 != status) 
  {
    TF_ASSERT(0);
  }

  /* Free embeded pointers for UL DCCH message */
  asn1Free_lte_rrc_osys_UL_DCCH_Message(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);
  

  TF_MSG("\n Objective Systems ASN.1 Test :: Basic UL DCCH encode end. \n");
}

void LteRrcAsn1OsysMallocProfileTest::Teardown()
{
  lte_rrc_osys_free_context();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  TF_ASSERT(lte_rrc_mem_util_get_rrc_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_small_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_sib_heap_used_bytes() == 0);
#endif
}


/*===========================================================================
  LteRrcAsn1OsysBasicUlDcchEncodeLoopTest
===========================================================================*/
/*!
  @brief
  Tests Objective Systems ASN.1 basic UL DCCH Encoding ...

*/
/*=========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcAsn1OsysBasicUlDcchEncodeLoopTest);

void LteRrcAsn1OsysBasicUlDcchEncodeLoopTest::Setup()
{
  lte_rrc_mem_init_heap();
  lte_rrc_osys_init();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();
#endif

}

void LteRrcAsn1OsysBasicUlDcchEncodeLoopTest::Test()
{
  lte_rrc_osys_UL_DCCH_Message ul_dcch_msg;
  lte_rrc_osys_MeasurementReport_r8_IEs *meas_rpt_ptr;
  lte_rrc_osys_MeasResultListEUTRA *eutra_list_ptr;
  OSOCTET *encoded_pdu_ptr = NULL;
  int encoded_pdu_length = 0;
  int status = 0;
  int i = 0;
  
#ifdef FEATURE_Q6SIM
  #error code not present
#endif 


  TF_MSG("Objective Systems ASN.1 Test :: Basic UL DCCH encode begin. \n");

#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  /* Construct mesurement report UL DCCH message */

  ul_dcch_msg.message.t = T_lte_rrc_osys_UL_DCCH_MessageType_c1;

  ul_dcch_msg.message.u.c1 = 
    (lte_rrc_osys_UL_DCCH_MessageType_c1 * )rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_UL_DCCH_MessageType_c1));
  ul_dcch_msg.message.u.c1->t = T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport;
  
  ul_dcch_msg.message.u.c1->u.measurementReport = 
    (lte_rrc_osys_MeasurementReport *)rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasurementReport));
  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.t = 
    T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1;

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1 = 
    (lte_rrc_osys_MeasurementReport_criticalExtensions_c1 *) 
  rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasurementReport_criticalExtensions_c1));

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->t = 
    T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8;

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.measurementReport_r8 = 
    (lte_rrc_osys_MeasurementReport_r8_IEs *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasurementReport_r8_IEs));

  meas_rpt_ptr = 
    ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.measurementReport_r8;


  meas_rpt_ptr->measResults.measId = 4;
  meas_rpt_ptr->measResults.measResultPCell.rsrpResult = 5;
  meas_rpt_ptr->measResults.measResultPCell.rsrqResult = 12;

  meas_rpt_ptr->measResults.extElem1.count = 0;
  meas_rpt_ptr->measResults.extElem1.head = NULL;

  meas_rpt_ptr->measResults.m.measResultNeighCellsPresent = 1;
  meas_rpt_ptr->measResults.measResultNeighCells.t = 
    T_lte_rrc_osys_MeasResults_measResultNeighCells_measResultListEUTRA;
  meas_rpt_ptr->measResults.measResultNeighCells.u.measResultListEUTRA = 
    (lte_rrc_osys_MeasResultListEUTRA *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasResultListEUTRA));

  eutra_list_ptr = 
    meas_rpt_ptr->measResults.measResultNeighCells.u.measResultListEUTRA;

  /* Set the number of elements in list */
  eutra_list_ptr->n = 1;
  eutra_list_ptr->elem = (lte_rrc_osys_MeasResultEUTRA *)  
  rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasResultEUTRA) * eutra_list_ptr->n);

  /* Populate info for neighbor 1 */
  eutra_list_ptr->elem[0].measResult.m.rsrpResultPresent = 1;
  eutra_list_ptr->elem[0].measResult.rsrpResult = 7;
  eutra_list_ptr->elem[0].physCellId = 502;

  /* Populate info for neighbor 2 */
  
  eutra_list_ptr->elem[0].m.cgi_InfoPresent = 1;
  eutra_list_ptr->elem[0].cgi_Info.m.plmn_IdentityListPresent = 0;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.numbits = 28;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[0] = 0xFF;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[1] = 0x00;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[2] = 0xFF;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[3] = 0x00;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.m.mccPresent = 1;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.n = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem =
    (lte_rrc_osys_MCC_MNC_Digit *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), 
                                             (sizeof(lte_rrc_osys_MCC_MNC_Digit) * eutra_list_ptr->elem[0].cgi_Info.cellGlobalId .plmn_Identity.mcc.n));
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[0] = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[1] = 2;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[2] = 1;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.n = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem =
    (lte_rrc_osys_MCC_MNC_Digit *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), 
                                             (sizeof(lte_rrc_osys_MCC_MNC_Digit) * eutra_list_ptr->elem[0].cgi_Info. cellGlobalId.plmn_Identity.mnc.n));  
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[0] = 1;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[1] = 2;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[2] = 3;

  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.numbits = 16;
  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.data[0] = 0xF0;
  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.data[1] = 0x0F;

  eutra_list_ptr->elem[0].measResult.m.rsrpResultPresent = 1;
  eutra_list_ptr->elem[0].measResult.m.rsrqResultPresent = 0;
  eutra_list_ptr->elem[0].measResult.rsrpResult = 10;
  eutra_list_ptr->elem[0].measResult.extElem1.count = 0;
  eutra_list_ptr->elem[0].measResult.extElem1.head = NULL;


  eutra_list_ptr->elem[0].physCellId = 503;

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 

#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();
#endif
  TF_MSG("Print prior to encode");

  for (i = 0 ; i < LTE_RRC_ASN1_MAX_LOOP_COUNT; i++) 
  {
  
#ifdef FEATURE_Q6SIM
    #error code not present
#endif


    /* Specify the encoded ptr and length of the PER encoded message to be decoded.  Specifing
       NULL ptr and length of 0 makes it allocate memory dynamically.  Setting the last
       param to FALSE will make it use unaligned PER  */
    pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), NULL, 0, FALSE);

    status = asn1PE_lte_rrc_osys_UL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);


    /* Get the pointer and length of the encoded msg */
    encoded_pdu_ptr = pe_GetMsgPtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &encoded_pdu_length);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif
    TF_MSG("Print after encode %d", i);

    /* Free encoded pdu ptr */
    rtxMemFreePtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), encoded_pdu_ptr);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP    
    lte_rrc_asn1_test_print_heap_status();
#endif
    TF_MSG("Print after free %d", i);


#ifdef FEATURE_Q6SIM  
    #error code not present
#endif 

  }

#ifdef FEATURE_Q6SIM 
#error code not present
#endif

  /* Free embeded pointers for UL DCCH message */
  asn1Free_lte_rrc_osys_UL_DCCH_Message(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);
  
  TF_MSG("Objective Systems ASN.1 loop Test :: Basic UL DCCH encode end. \n");
}

void LteRrcAsn1OsysBasicUlDcchEncodeLoopTest::Teardown()
{
  lte_rrc_osys_free_context();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  TF_ASSERT(lte_rrc_mem_util_get_rrc_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_small_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_sib_heap_used_bytes() == 0);
#endif
}



/*===========================================================================
  LteRrcAsn1OsysBasicUlDcchEncodeLoopTest
===========================================================================*/
/*!
  @brief
  Tests Objective Systems ASN.1 basic UL DCCH Encoding ...

*/
/*=========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcAsn1OsysBasicUlDcchEncodeLoopTest2);

void LteRrcAsn1OsysBasicUlDcchEncodeLoopTest2::Setup()
{
  lte_rrc_mem_init_heap();
  lte_rrc_osys_init();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();
#endif

}

void LteRrcAsn1OsysBasicUlDcchEncodeLoopTest2::Test()
{
  lte_rrc_osys_UL_DCCH_Message ul_dcch_msg;
  lte_rrc_osys_MeasurementReport_r8_IEs *meas_rpt_ptr;
  lte_rrc_osys_MeasResultListEUTRA *eutra_list_ptr;
  OSOCTET *encoded_pdu_ptr_1 = NULL;
  OSOCTET *encoded_pdu_ptr_2 = NULL;
  OSOCTET *encoded_pdu_ptr_3 = NULL;
  OSOCTET *encoded_pdu_ptr_4 = NULL;
  OSOCTET *encoded_pdu_ptr_5 = NULL;
  OSOCTET *encoded_pdu_ptr_6 = NULL;
  OSOCTET *encoded_pdu_ptr_7 = NULL;
  OSOCTET *encoded_pdu_ptr_8 = NULL;
  OSOCTET *encoded_pdu_ptr_9 = NULL;
  OSOCTET *encoded_pdu_ptr_10 = NULL;
  int encoded_pdu_length = 0;
  int status = 0;
  int i = 0;
  

  TF_MSG("Objective Systems ASN.1 Test :: Basic UL DCCH encode begin. \n");


  /* Construct mesurement report UL DCCH message */

  ul_dcch_msg.message.t = T_lte_rrc_osys_UL_DCCH_MessageType_c1;

  ul_dcch_msg.message.u.c1 = 
    (lte_rrc_osys_UL_DCCH_MessageType_c1 * )rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_UL_DCCH_MessageType_c1));
  ul_dcch_msg.message.u.c1->t = T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport;
  
  ul_dcch_msg.message.u.c1->u.measurementReport = 
    (lte_rrc_osys_MeasurementReport *)rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasurementReport));
  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.t = 
    T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1;

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1 = 
    (lte_rrc_osys_MeasurementReport_criticalExtensions_c1 *) 
  rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasurementReport_criticalExtensions_c1));

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->t = 
    T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8;

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.measurementReport_r8 = 
    (lte_rrc_osys_MeasurementReport_r8_IEs *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasurementReport_r8_IEs));

  meas_rpt_ptr = 
    ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.measurementReport_r8;


  meas_rpt_ptr->measResults.measId = 4;
  meas_rpt_ptr->measResults.measResultPCell.rsrpResult = 5;
  meas_rpt_ptr->measResults.measResultPCell.rsrqResult = 12;

  meas_rpt_ptr->measResults.extElem1.count = 0;
  meas_rpt_ptr->measResults.extElem1.head = NULL;

  meas_rpt_ptr->measResults.m.measResultNeighCellsPresent = 1;
  meas_rpt_ptr->measResults.measResultNeighCells.t = 
    T_lte_rrc_osys_MeasResults_measResultNeighCells_measResultListEUTRA;
  meas_rpt_ptr->measResults.measResultNeighCells.u.measResultListEUTRA = 
    (lte_rrc_osys_MeasResultListEUTRA *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasResultListEUTRA));

  eutra_list_ptr = 
    meas_rpt_ptr->measResults.measResultNeighCells.u.measResultListEUTRA;

  /* Set the number of elements in list */
  eutra_list_ptr->n = 1;
  eutra_list_ptr->elem = (lte_rrc_osys_MeasResultEUTRA *)  
  rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasResultEUTRA) * eutra_list_ptr->n);

  /* Populate info for neighbor 1 */
  eutra_list_ptr->elem[0].measResult.m.rsrpResultPresent = 1;
  eutra_list_ptr->elem[0].measResult.rsrpResult = 7;
  eutra_list_ptr->elem[0].physCellId = 502;

  /* Populate info for neighbor 2 */
  
  eutra_list_ptr->elem[0].m.cgi_InfoPresent = 1;
  eutra_list_ptr->elem[0].cgi_Info.m.plmn_IdentityListPresent = 0;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.numbits = 28;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[0] = 0xFF;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[1] = 0x00;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[2] = 0xFF;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[3] = 0x00;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.m.mccPresent = 1;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.n = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem =
    (lte_rrc_osys_MCC_MNC_Digit *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), 
                                             (sizeof(lte_rrc_osys_MCC_MNC_Digit) * eutra_list_ptr->elem[0].cgi_Info.cellGlobalId .plmn_Identity.mcc.n));
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[0] = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[1] = 2;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[2] = 1;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.n = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem =
    (lte_rrc_osys_MCC_MNC_Digit *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), 
                                             (sizeof(lte_rrc_osys_MCC_MNC_Digit) * eutra_list_ptr->elem[0].cgi_Info. cellGlobalId.plmn_Identity.mnc.n));  
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[0] = 1;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[1] = 2;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[2] = 3;

  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.numbits = 16;
  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.data[0] = 0xF0;
  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.data[1] = 0x0F;

  eutra_list_ptr->elem[0].measResult.m.rsrpResultPresent = 1;
  eutra_list_ptr->elem[0].measResult.m.rsrqResultPresent = 0;
  eutra_list_ptr->elem[0].measResult.rsrpResult = 10;
  eutra_list_ptr->elem[0].measResult.extElem1.count = 0;
  eutra_list_ptr->elem[0].measResult.extElem1.head = NULL;


  eutra_list_ptr->elem[0].physCellId = 503;


  TF_MSG("Print before encoding");
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();
#endif


  for (i = 0 ; i < LTE_RRC_ASN1_MAX_LOOP_COUNT; i++) 
  {
  

    /* Specify the encoded ptr and length of the PER encoded message to be decoded.  Specifing
       NULL ptr and length of 0 makes it allocate memory dynamically.  Setting the last
       param to FALSE will make it use unaligned PER  */
    pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), NULL, 0, FALSE);

    status = asn1PE_lte_rrc_osys_UL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);

    /* Get the pointer and length of the encoded msg */
    encoded_pdu_ptr_1 = pe_GetMsgPtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &encoded_pdu_length);

    TF_MSG("Print after encode 1 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif


    /* Specify the encoded ptr and length of the PER encoded message to be decoded.  Specifing
       NULL ptr and length of 0 makes it allocate memory dynamically.  Setting the last
       param to FALSE will make it use unaligned PER  */
    pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), NULL, 0, FALSE);

    status = asn1PE_lte_rrc_osys_UL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);

    /* Get the pointer and length of the encoded msg */
    encoded_pdu_ptr_2 = pe_GetMsgPtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &encoded_pdu_length);


    TF_MSG("Print after encode 2 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif


    /* Specify the encoded ptr and length of the PER encoded message to be decoded.  Specifing
       NULL ptr and length of 0 makes it allocate memory dynamically.  Setting the last
       param to FALSE will make it use unaligned PER  */
    pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), NULL, 0, FALSE);

    status = asn1PE_lte_rrc_osys_UL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);

    /* Get the pointer and length of the encoded msg */
    encoded_pdu_ptr_3 = pe_GetMsgPtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &encoded_pdu_length);

    TF_MSG("Print after encode 3 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif

    /* Specify the encoded ptr and length of the PER encoded message to be decoded.  Specifing
       NULL ptr and length of 0 makes it allocate memory dynamically.  Setting the last
       param to FALSE will make it use unaligned PER  */
    pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), NULL, 0, FALSE);

    status = asn1PE_lte_rrc_osys_UL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);

    /* Get the pointer and length of the encoded msg */
    encoded_pdu_ptr_4 = pe_GetMsgPtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &encoded_pdu_length);

    TF_MSG("Print after encode 4 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif


    /* Specify the encoded ptr and length of the PER encoded message to be decoded.  Specifing
       NULL ptr and length of 0 makes it allocate memory dynamically.  Setting the last
       param to FALSE will make it use unaligned PER  */
    pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), NULL, 0, FALSE);

    status = asn1PE_lte_rrc_osys_UL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);


    /* Get the pointer and length of the encoded msg */
    encoded_pdu_ptr_5 = pe_GetMsgPtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &encoded_pdu_length);

    TF_MSG("Print after encode 5 i %d", i);

#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif


    /* Specify the encoded ptr and length of the PER encoded message to be decoded.  Specifing
       NULL ptr and length of 0 makes it allocate memory dynamically.  Setting the last
       param to FALSE will make it use unaligned PER  */
    pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), NULL, 0, FALSE);

    status = asn1PE_lte_rrc_osys_UL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);

    /* Get the pointer and length of the encoded msg */
    encoded_pdu_ptr_6 = pe_GetMsgPtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &encoded_pdu_length);

    TF_MSG("Print after encode 6 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif


    /* Specify the encoded ptr and length of the PER encoded message to be decoded.  Specifing
       NULL ptr and length of 0 makes it allocate memory dynamically.  Setting the last
       param to FALSE will make it use unaligned PER  */
    pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), NULL, 0, FALSE);

    status = asn1PE_lte_rrc_osys_UL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);


    /* Get the pointer and length of the encoded msg */
    encoded_pdu_ptr_7 = pe_GetMsgPtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &encoded_pdu_length);

    TF_MSG("Print after encode 7 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif

    /* Specify the encoded ptr and length of the PER encoded message to be decoded.  Specifing
       NULL ptr and length of 0 makes it allocate memory dynamically.  Setting the last
       param to FALSE will make it use unaligned PER  */
    pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), NULL, 0, FALSE);

    status = asn1PE_lte_rrc_osys_UL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);

    /* Get the pointer and length of the encoded msg */
    encoded_pdu_ptr_8 = pe_GetMsgPtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &encoded_pdu_length);

    TF_MSG("Print after encode 8 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif
    /* Specify the encoded ptr and length of the PER encoded message to be decoded.  Specifing
       NULL ptr and length of 0 makes it allocate memory dynamically.  Setting the last
       param to FALSE will make it use unaligned PER  */
    pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), NULL, 0, FALSE);

    status = asn1PE_lte_rrc_osys_UL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);

    /* Get the pointer and length of the encoded msg */
    encoded_pdu_ptr_9 = pe_GetMsgPtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &encoded_pdu_length);

    TF_MSG("Print after encode 9 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif

    /* Specify the encoded ptr and length of the PER encoded message to be decoded.  Specifing
       NULL ptr and length of 0 makes it allocate memory dynamically.  Setting the last
       param to FALSE will make it use unaligned PER  */
    pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), NULL, 0, FALSE);

    status = asn1PE_lte_rrc_osys_UL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);

    /* Get the pointer and length of the encoded msg */
    encoded_pdu_ptr_10 = pe_GetMsgPtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &encoded_pdu_length);

    TF_MSG("Print after encode 10 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif


    /* Free encoded pdu ptr */
    rtxMemFreePtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), encoded_pdu_ptr_1);
    TF_MSG("Print after free 1 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif
    /* Free encoded pdu ptr */
    rtxMemFreePtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), encoded_pdu_ptr_2);
    TF_MSG("Print after free 2 i %d", i);

#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif
    /* Free encoded pdu ptr */
    rtxMemFreePtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), encoded_pdu_ptr_3);
    TF_MSG("Print after free 3 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif
    /* Free encoded pdu ptr */
    rtxMemFreePtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), encoded_pdu_ptr_4);
    TF_MSG("Print after free 4 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif
    /* Free encoded pdu ptr */
    rtxMemFreePtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), encoded_pdu_ptr_5);
    TF_MSG("Print after free 5 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif
    /* Free encoded pdu ptr */
    rtxMemFreePtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), encoded_pdu_ptr_6);
    TF_MSG("Print after free 6 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif
    /* Free encoded pdu ptr */
    rtxMemFreePtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), encoded_pdu_ptr_7);
    TF_MSG("Print after free 7 i %", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif
    /* Free encoded pdu ptr */
    rtxMemFreePtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), encoded_pdu_ptr_8);
    TF_MSG("Print after free 8 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif
    /* Free encoded pdu ptr */
    rtxMemFreePtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), encoded_pdu_ptr_9);
    TF_MSG("Print after free 9 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif
    /* Free encoded pdu ptr */
    rtxMemFreePtr(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), encoded_pdu_ptr_10);
    TF_MSG("Print after free 10 i %d", i);
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
    lte_rrc_asn1_test_print_heap_status();
#endif
  }


  /* Free embeded pointers for UL DCCH message */
  asn1Free_lte_rrc_osys_UL_DCCH_Message(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);

  TF_MSG("Print free of decoded msg");
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  lte_rrc_asn1_test_print_heap_status();
#endif
  TF_MSG("Objective Systems ASN.1 loop Test :: Basic UL DCCH encode end. \n");
}

void LteRrcAsn1OsysBasicUlDcchEncodeLoopTest2::Teardown()
{
  lte_rrc_osys_free_context();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  TF_ASSERT(lte_rrc_mem_util_get_rrc_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_small_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_sib_heap_used_bytes() == 0);
#endif
}

/*===========================================================================
  LteRrcAsn1OsysULDCCHEncodeUsingStaticBufTest
===========================================================================*/
/*!
  @brief
  Tests Objective Systems ASN.1 basic UL DCCH Encoding using static buffer 

*/
/*=========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcAsn1OsysULDCCHEncodeUsingStaticBufTest);

void LteRrcAsn1OsysULDCCHEncodeUsingStaticBufTest::Setup()
{
  lte_rrc_mem_init_heap();
  lte_rrc_osys_init();
}

void LteRrcAsn1OsysULDCCHEncodeUsingStaticBufTest::Test()
{
  lte_rrc_osys_UL_DCCH_Message ul_dcch_msg;
  lte_rrc_osys_MeasurementReport_r8_IEs *meas_rpt_ptr;
  lte_rrc_osys_MeasResultListEUTRA *eutra_list_ptr;
  OSOCTET encoded_pdu_buf[100];
  int encoded_pdu_length = 0;
  int status = 0;


#ifdef FEATURE_Q6SIM
  #error code not present
#endif



  TF_MSG("Objective Systems ASN.1 Test :: UL DCCH encode using static buf begin. ");


  /* Construct mesurement report UL DCCH message */

  ul_dcch_msg.message.t = T_lte_rrc_osys_UL_DCCH_MessageType_c1;

  ul_dcch_msg.message.u.c1 = 
    (lte_rrc_osys_UL_DCCH_MessageType_c1 * )rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_UL_DCCH_MessageType_c1));
  ul_dcch_msg.message.u.c1->t = T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport;
  
  ul_dcch_msg.message.u.c1->u.measurementReport = 
    (lte_rrc_osys_MeasurementReport *)rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasurementReport));
  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.t = 
    T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1;

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1 = 
    (lte_rrc_osys_MeasurementReport_criticalExtensions_c1 *) 
  rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasurementReport_criticalExtensions_c1));

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->t = 
    T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8;

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.measurementReport_r8 = 
    (lte_rrc_osys_MeasurementReport_r8_IEs *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasurementReport_r8_IEs));

  meas_rpt_ptr = 
    ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.measurementReport_r8;


  meas_rpt_ptr->measResults.measId = 4;
  meas_rpt_ptr->measResults.measResultPCell.rsrpResult = 5;
  meas_rpt_ptr->measResults.measResultPCell.rsrqResult = 12;

  meas_rpt_ptr->measResults.extElem1.count = 0;
  meas_rpt_ptr->measResults.extElem1.head = NULL;

  meas_rpt_ptr->measResults.m.measResultNeighCellsPresent = 1;
  meas_rpt_ptr->measResults.measResultNeighCells.t = 
    T_lte_rrc_osys_MeasResults_measResultNeighCells_measResultListEUTRA;
  meas_rpt_ptr->measResults.measResultNeighCells.u.measResultListEUTRA = 
    (lte_rrc_osys_MeasResultListEUTRA *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasResultListEUTRA));

  eutra_list_ptr = 
    meas_rpt_ptr->measResults.measResultNeighCells.u.measResultListEUTRA;

  /* Set the number of elements in list */
  eutra_list_ptr->n = 1;
  eutra_list_ptr->elem = (lte_rrc_osys_MeasResultEUTRA *)  
  rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), sizeof(lte_rrc_osys_MeasResultEUTRA) * eutra_list_ptr->n);

  /* Populate info for neighbor 1 */
  eutra_list_ptr->elem[0].measResult.m.rsrpResultPresent = 1;
  eutra_list_ptr->elem[0].measResult.rsrpResult = 7;
  eutra_list_ptr->elem[0].physCellId = 502;

  /* Populate info for neighbor 2 */
  
  eutra_list_ptr->elem[0].m.cgi_InfoPresent = 1;
  eutra_list_ptr->elem[0].cgi_Info.m.plmn_IdentityListPresent = 0;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.numbits = 28;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[0] = 0xFF;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[1] = 0x00;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[2] = 0xFF;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[3] = 0x00;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.m.mccPresent = 1;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.n = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem =
    (lte_rrc_osys_MCC_MNC_Digit *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), 
                                             (sizeof(lte_rrc_osys_MCC_MNC_Digit) * eutra_list_ptr->elem[0].cgi_Info.cellGlobalId .plmn_Identity.mcc.n));
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[0] = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[1] = 2;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[2] = 1;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.n = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem =
    (lte_rrc_osys_MCC_MNC_Digit *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), 
                                             (sizeof(lte_rrc_osys_MCC_MNC_Digit) * eutra_list_ptr->elem[0].cgi_Info. cellGlobalId.plmn_Identity.mnc.n));  
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[0] = 1;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[1] = 2;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[2] = 3;

  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.numbits = 16;
  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.data[0] = 0xF0;
  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.data[1] = 0x0F;

  eutra_list_ptr->elem[0].measResult.m.rsrpResultPresent = 1;
  eutra_list_ptr->elem[0].measResult.m.rsrqResultPresent = 0;
  eutra_list_ptr->elem[0].measResult.rsrpResult = 10;
  eutra_list_ptr->elem[0].measResult.extElem1.count = 0;
  eutra_list_ptr->elem[0].measResult.extElem1.head = NULL;


  eutra_list_ptr->elem[0].physCellId = 503;

  /* Specify the encoded ptr and length of the PER encoded message to be decoded.  Specifing
     NULL ptr and length of 0 makes it allocate memory dynamically.  Setting the last
     param to FALSE will make it use unaligned PER  */

#ifdef FEATURE_Q6SIM
  #error code not present
#endif
  pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), encoded_pdu_buf, 100, FALSE);

  status = asn1PE_lte_rrc_osys_UL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 
  if (0 != status)
  {
    LTE_ERROR("encoding failure",0,0,0);
    rtxErrPrint(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt));
  }

  TF_ASSERT(status == 0);

  /* Get the pointer and length of the encoded msg */
  encoded_pdu_length = pe_GetMsgLen(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt));

  TF_ASSERT(encoded_pdu_length > 0);


  /* Free embeded pointers for UL DCCH message */
  asn1Free_lte_rrc_osys_UL_DCCH_Message(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);
  

  TF_MSG("Objective Systems ASN.1 Test :: UL DCCH encode using static buf end. \n");
}

void LteRrcAsn1OsysULDCCHEncodeUsingStaticBufTest::Teardown()
{
  lte_rrc_osys_free_context();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  TF_ASSERT(lte_rrc_mem_util_get_rrc_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_small_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_sib_heap_used_bytes() == 0);
#endif
}

/*===========================================================================
  LteRrcAsn1OsysUlDcchEncodeFailureTest
===========================================================================*/
/*!
  @brief
  Tests Objective Systems ASN.1 UL DCCH Encoding failure...

*/
/*=========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcAsn1OsysUlDcchEncodeFailureTest);

void LteRrcAsn1OsysUlDcchEncodeFailureTest::Setup()
{
  lte_rrc_mem_init_heap();
  lte_rrc_osys_init();
}

void LteRrcAsn1OsysUlDcchEncodeFailureTest::Test()
{
  lte_rrc_osys_UL_DCCH_Message ul_dcch_msg;
  lte_rrc_osys_MeasurementReport_r8_IEs *meas_rpt_ptr;
  lte_rrc_osys_MeasResultListEUTRA *eutra_list_ptr;
  int status = 0;

  TF_MSG("Objective systems ASN.1 Test :: Basic UL DCCH encode failure.");

  /* Construct mesurement report UL DCCH message */

  ul_dcch_msg.message.t = T_lte_rrc_osys_UL_DCCH_MessageType_c1;

  ul_dcch_msg.message.u.c1 = 
    (lte_rrc_osys_UL_DCCH_MessageType_c1 * )rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt),sizeof(lte_rrc_osys_UL_DCCH_MessageType_c1));
  ul_dcch_msg.message.u.c1->t = T_lte_rrc_osys_UL_DCCH_MessageType_c1_measurementReport;
  
  ul_dcch_msg.message.u.c1->u.measurementReport = 
    (lte_rrc_osys_MeasurementReport *)rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt),sizeof(lte_rrc_osys_MeasurementReport));
  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.t = 
    T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1;

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1 = 
    (lte_rrc_osys_MeasurementReport_criticalExtensions_c1 *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt),
                                                                       sizeof(lte_rrc_osys_MeasurementReport_criticalExtensions_c1));

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->t = 
    T_lte_rrc_osys_MeasurementReport_criticalExtensions_c1_measurementReport_r8;

  ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.measurementReport_r8 = 
    (lte_rrc_osys_MeasurementReport_r8_IEs *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt),sizeof(lte_rrc_osys_MeasurementReport_r8_IEs));

  meas_rpt_ptr = 
    ul_dcch_msg.message.u.c1->u.measurementReport->criticalExtensions.u.c1->u.measurementReport_r8;

  meas_rpt_ptr->measResults.measId = 4;
  meas_rpt_ptr->measResults.measResultPCell.rsrpResult = 5;
  meas_rpt_ptr->measResults.measResultPCell.rsrqResult = 12;

  meas_rpt_ptr->measResults.extElem1.count = 0;
  meas_rpt_ptr->measResults.extElem1.head = NULL;
  
  meas_rpt_ptr->measResults.m.measResultNeighCellsPresent = 1;
  meas_rpt_ptr->measResults.measResultNeighCells.t = 
    T_lte_rrc_osys_MeasResults_measResultNeighCells_measResultListEUTRA;
  
  meas_rpt_ptr->measResults.measResultNeighCells.u.measResultListEUTRA = 
    (lte_rrc_osys_MeasResultListEUTRA *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt),sizeof(lte_rrc_osys_MeasResultListEUTRA));

  eutra_list_ptr = 
    meas_rpt_ptr->measResults.measResultNeighCells.u.measResultListEUTRA;

  /* Set the number of elements in list */
  eutra_list_ptr->n = 1;
  eutra_list_ptr->elem = (lte_rrc_osys_MeasResultEUTRA *)  
  rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt),sizeof(lte_rrc_osys_MeasResultEUTRA) * eutra_list_ptr->n);

  /* Populate info for neighbor 1 */
  eutra_list_ptr->elem[0].measResult.m.rsrpResultPresent = 1;
  eutra_list_ptr->elem[0].measResult.rsrpResult = 7;

  eutra_list_ptr->elem[0].physCellId = 502;

  /* Populate info for neighbor 2 */
  
  eutra_list_ptr->elem[0].m.cgi_InfoPresent = 1;

  eutra_list_ptr->elem[0].cgi_Info.m.plmn_IdentityListPresent = 0;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.numbits = 28;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[0] = 0xFF;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[1] = 0x00;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[2] = 0xFF;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.cellIdentity.data[3] = 0x00;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.m.mccPresent = 1;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.n = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem =
    (lte_rrc_osys_MCC_MNC_Digit *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt),
                                             sizeof(lte_rrc_osys_MCC_MNC_Digit) * eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.n);
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[0] = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[1] = 2;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mcc.elem[2] = 1;

  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.n = 3;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem =
    (lte_rrc_osys_MCC_MNC_Digit *) rtxMemAlloc(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt),
                                             sizeof(lte_rrc_osys_MCC_MNC_Digit) * eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.n);  
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[0] = 1;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[1] = 2;
  eutra_list_ptr->elem[0].cgi_Info.cellGlobalId.plmn_Identity.mnc.elem[2] = 3;

  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.numbits = 16;
  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.data[0] = 0xF0;
  eutra_list_ptr->elem[0].cgi_Info.trackingAreaCode.data[1] = 0x0F;

  eutra_list_ptr->elem[0].measResult.m.rsrpResultPresent = 1;
  eutra_list_ptr->elem[0].measResult.rsrpResult = 10;
  eutra_list_ptr->elem[0].measResult.extElem1.count = 0;
  eutra_list_ptr->elem[0].measResult.extElem1.head = NULL;

  /*** Try to encode wrong and out of range Phy cell id so encoding fails ****/
  eutra_list_ptr->elem[0].physCellId = 511;

  /* Specify the encoded ptr and length of the PER encoded message to be decoded.  Specifing
     NULL ptr and length of 0 makes it allocate memory dynamically.  Setting the last
     param to FALSE will make it use unaligned PER  */
  pu_setBuffer (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), NULL, 0, FALSE);

  if (0 != (status = asn1PE_lte_rrc_osys_UL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg)))
  {
    LTE_ERROR("encoding failure",0,0,0);
    rtxErrPrint(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt));
    (void)rtxErrReset(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt));
  }

  /* Free embeded pointers for UL DCCH message */
  asn1Free_lte_rrc_osys_UL_DCCH_Message(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &ul_dcch_msg);
  

  TF_MSG("Objective systems ASN.1 Test :: Basic UL DCCH encode failure. \n");
}

void LteRrcAsn1OsysUlDcchEncodeFailureTest::Teardown()
{
  lte_rrc_osys_free_context();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  TF_ASSERT(lte_rrc_mem_util_get_rrc_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_small_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_sib_heap_used_bytes() == 0);
#endif
}


/*===========================================================================
  LteRrcAsn1OsysDcchDecodeCopyAndCompareTest
===========================================================================*/
/*!
  @brief
  Tests Objective Systems ASN.1 basic DL DCCH Decoding, copying and comparing

  Steps:
    1 - Decode encoded message
    2 - Copy same message
    3 - Call compare function to compare original and copy
    4 - Change some values in original then compare with the copy.

*/
/*=========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcAsn1OsysDcchDecodeCopyAndCompareTest);

void LteRrcAsn1OsysDcchDecodeCopyAndCompareTest::Setup()
{
  lte_rrc_mem_init_heap();
  lte_rrc_osys_init();
}

void LteRrcAsn1OsysDcchDecodeCopyAndCompareTest::Test()
{
  lte_rrc_osys_DL_DCCH_Message *src_dl_dcch_msg;
  lte_rrc_osys_DL_DCCH_Message trg_dl_dcch_msg;
  char errBuff[100];

  int status = 0;

#ifdef FEATURE_Q6SIM
  #error code not present
#endif




  TF_MSG("Objective systems ASN.1 Test :: Basic DL DCCH decode copy and compare begin. \n");

#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  src_dl_dcch_msg = (lte_rrc_osys_DL_DCCH_Message *)
  lte_rrc_osys_asn1_decode_pdu(&dl_dcch_reconfig_msg, sizeof(dl_dcch_reconfig_msg), DL_DCCH_Message, &status);

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 

  if (0 != status) 
  {
    LTE_ERROR("Decoding failure",0,0,0);
    rtxErrPrint(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt));
  }

  TF_ASSERT(status == 0);


#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  /* Copy from source to target */
  //asn1Copy_lte_rrc_osys_DL_DCCH_Message (&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &src_dl_dcch_msg, &trg_dl_dcch_msg);

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 

#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  //status = asn1Compare_lte_rrc_osys_DL_DCCH_Message ("DL_DCCH_Message", &src_dl_dcch_msg, &trg_dl_dcch_msg, errBuff, 100);

#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 

  /* Compare source and target... It is expected that compare returns TRUE */
  //TF_ASSERT(status != 0);

  /* Change something in the original message */

  src_dl_dcch_msg->message.u.c1->u.rrcConnectionReconfiguration->rrc_TransactionIdentifier = 2;

#ifdef FEATURE_Q6SIM
  #error code not present
#endif

  //status = asn1Compare_lte_rrc_osys_DL_DCCH_Message ("DL_DCCH_Message", &src_dl_dcch_msg, &trg_dl_dcch_msg, errBuff, 100);


#ifdef FEATURE_Q6SIM  
  #error code not present
#endif 

  /* Compare source and target... It is expected that compare returns FALSE */
  //TF_ASSERT(status == 0);

  /* Now free decoded messages */
  lte_rrc_osys_asn1_free_pdu(src_dl_dcch_msg, lte_rrc_osys_DL_DCCH_Message_PDU);
  asn1Free_lte_rrc_osys_DL_DCCH_Message(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), src_dl_dcch_msg);
  asn1Free_lte_rrc_osys_DL_DCCH_Message(&(osys_priv_ptr->dd_ptr->lte_rrc_asn1_ctxt), &trg_dl_dcch_msg);

  TF_MSG("Objective systems ASN.1 Test :: Basic DL DCCH decode copy and compare end. \n");
}

void LteRrcAsn1OsysDcchDecodeCopyAndCompareTest::Teardown()
{
  lte_rrc_osys_free_context();
#ifdef FEATURE_LTE_RRC_USE_RRC_HEAP
  TF_ASSERT(lte_rrc_mem_util_get_rrc_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_small_heap_used_bytes() == 0);
  TF_ASSERT(lte_rrc_mem_util_get_rrc_sib_heap_used_bytes() == 0);
#endif
}

