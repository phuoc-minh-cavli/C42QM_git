/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TLE test asn1 implementation
GENERAL DESCRIPTION
   This file contains constants, classes, structures and function implementations
   for TLE ASN1 Tests.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2015-2021 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
==============================================================================*/


/*==============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

Version Control

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/tle/2_0/src/test/src/tle_test_asn1.cpp#2 $
  $DateTime: 2021/09/01 02:25:50 $
  $Author: pwbldsvc $

when        who     what, where, why
--------    ---   ----------------------------------------------------------
9/09/2015  hleung Initial version

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifdef FEATURE_CGPS_PLAYBACK

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include <iostream>
#include "tle_test_asn1.h"

#include "tle_cell_db.h"

/*=============================================================================

       Macros

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/

/*=============================================================================

       Structures and enumerated types

=============================================================================*/
struct CellDbRecordDynamicDataType
{
  TDPv3CellDB_Record  z_CellDbRecord;
  TDPv3LTECellId z_LteCellId;
  TDPv3LocationInformation_locationSource_uploadLocationSource z_UploadLocationSource;
  TDPv3LocationInformation_locationSource_uploadLocationSource_sourceAllocation z_SourceAllocation;
  TDPv3TdpUploadData_tdpDataObservations_lteTdpUploadData z_LteTdpUploadDataSet;
  TDPv3LteTdpUploadData z_LteTdpUploadData[ASN1V_TDPv3maxNumTdpObservationSets];
  TDPv3LteTdpUploadData_lteTdpMeasAndCellData_element z_LteTdpMeasAndCellData[ASN1V_TDPv3maxNumTdpMeasurement];
};

/*==============================================================================

       Variables

==============================================================================*/
static CellDbRecordDynamicDataType z_TestCellDbRecord;

/*==============================================================================
  FUNCTION      tle_Asn1EncoderTesterClass::Run
  
  DESCRIPTION 	
    This function runs all the tests for ASN.1.
  
  DEPENDENCIES
    None
  
  PARAMETERS
    pzAsn1Decoder - Points to the decoder under test.
    (Input)

    pzAsn1Encoder - Points to the encoder under test.
    (Input)
  
  RETURN VALUE  
    None
  
  SIDE EFFECTS  
    None

==============================================================================*/
void tle_Asn1EncoderTesterClass::Run()
{
  int q_NumOfTestsPassed = 0;

  std::cout << std::endl;
  std::cout << "=======================================" << std::endl;
  std::cout << "           ASN.1 Tests Started         " << std::endl;
  std::cout << "=======================================" << std::endl;

  PrintTestResult("TestCellArrayEncoding", TestCellArrayEncoding());
  PrintTestResult("TestCellDbRecordToAsn1Struct", TestCellDbRecordToAsn1Struct());

  std::cout << "=======================================" << std::endl;
  std::cout << "           ASN.1 Tests Finished        " << std::endl;
  std::cout << "=======================================" << std::endl;
}

/*==============================================================================
  FUNCTION      GenerateLteTdpMeasAndCellData
  
  DESCRIPTION 	
    This function generates a LteTdpMeasAndCellData ASN.1 structure.
  
  DEPENDENCIES
    None
  
  PARAMETERS
    None
  
  RETURN VALUE  
    The pointer to the initialized LteTdpMeasAndCellData ASN.1 structure.
  
  SIDE EFFECTS  
    None

==============================================================================*/
static TDPv3LteTdpUploadData_lteTdpMeasAndCellData_element *GenerateLteTdpMeasAndCellData(void)
{
  TDPv3LteTdpUploadData_lteTdpMeasAndCellData_element *pz_LteTdpMeasAndCellData =
    z_TestCellDbRecord.z_LteTdpMeasAndCellData;

  for (int i=0; i<ASN1V_TDPv3maxNumTdpMeasurement; i++)
  {
    pz_LteTdpMeasAndCellData[i].m.neighborCellIDPresent = 1;
    pz_LteTdpMeasAndCellData[i].neighborCellID.m.globalIdentifierPresent = 1;
    pz_LteTdpMeasAndCellData[i].neighborCellID.globalIdentifier.m.tacUploadPresent = 1;
    pz_LteTdpMeasAndCellData[i].neighborCellID.globalIdentifier.mcc = 999;
    pz_LteTdpMeasAndCellData[i].neighborCellID.globalIdentifier.mnc = 999;
    pz_LteTdpMeasAndCellData[i].neighborCellID.globalIdentifier.tacUpload = 65535;
    pz_LteTdpMeasAndCellData[i].neighborCellID.globalIdentifier.globalCellId = 268435455;        
    pz_LteTdpMeasAndCellData[i].neighborCellID.localIdentifier.plid = 503;
    pz_LteTdpMeasAndCellData[i].neighborCellID.localIdentifier.earfcn = 262143;
    pz_LteTdpMeasAndCellData[i].measurementBlock.m.signalPowerPresent = 1;
    pz_LteTdpMeasAndCellData[i].measurementBlock.m.signalQualityPresent = 1;
    pz_LteTdpMeasAndCellData[i].measurementBlock.m.rxTxTimeDiffPresent = 1;
    pz_LteTdpMeasAndCellData[i].measurementBlock.signalPower = 97;
    pz_LteTdpMeasAndCellData[i].measurementBlock.signalQuality = 34;
    pz_LteTdpMeasAndCellData[i].measurementBlock.rxTxTimeDiff = 4095;
    pz_LteTdpMeasAndCellData[i].measurementBlock.rangingMeasurements.m.tToTMsPresent = 1;
    pz_LteTdpMeasAndCellData[i].measurementBlock.rangingMeasurements.rangingMeasurementType = TDPv3MeasurementBlock_rangingMeasurements_rangingMeasurementType_opCRS;
    pz_LteTdpMeasAndCellData[i].measurementBlock.rangingMeasurements.rangingMeasurementBW = TDPv3LteSystemBandwidth_bw_20;
    pz_LteTdpMeasAndCellData[i].measurementBlock.rangingMeasurements.fineClockSyncFlag = TRUE;
    pz_LteTdpMeasAndCellData[i].measurementBlock.rangingMeasurements.tToTMs = 10239;
    pz_LteTdpMeasAndCellData[i].measurementBlock.rangingMeasurements.rToaMs = 32767;
    pz_LteTdpMeasAndCellData[i].measurementBlock.rangingMeasurements.rToaNs = 500000;
    pz_LteTdpMeasAndCellData[i].measurementBlock.rangingMeasurements.rToaMeasUnc = 63;
    pz_LteTdpMeasAndCellData[i].measurementBlock.rangingMeasurements.rToaClockUnc = 63;
  }

  return pz_LteTdpMeasAndCellData;
}

/*==============================================================================
  FUNCTION      GenerateLteTdpUploadData
  
  DESCRIPTION 	
    This function generates a LteTdpUploadData ASN.1 structure.
  
  DEPENDENCIES
    None
  
  PARAMETERS
    NumOfNeighborInGrid - Number of neighbor cell in each of the scanlist.
    (Input)
  
  RETURN VALUE  
    The pointer to the initialized LteTdpUploadData ASN.1 structure.
  
  SIDE EFFECTS  
    None

==============================================================================*/
static TDPv3LteTdpUploadData *GenerateLteTdpUploadData(int NumOfNeighborInGrid)
{
  TDPv3LteTdpUploadData *pz_LteTdpUploadData = z_TestCellDbRecord.z_LteTdpUploadData;
  TDPv3LteTdpUploadData_lteTdpMeasAndCellData_element *pz_LteTdpMeasAndCellData = 
    GenerateLteTdpMeasAndCellData();

  if (NumOfNeighborInGrid > ASN1V_TDPv3maxNumTdpMeasurement)
  {
    NumOfNeighborInGrid = ASN1V_TDPv3maxNumTdpMeasurement;
  }

  for (int i=0; i<ASN1V_TDPv3maxNumTdpObservationSets; i++)
  {
    pz_LteTdpUploadData[i].locationInformation.m.positionTimestampPresent = 1;
    pz_LteTdpUploadData[i].locationInformation.m.velocityPresent = 1;
    pz_LteTdpUploadData[i].locationInformation.locationSource.t = T_TDPv3LocationInformation_locationSource_uploadLocationSource;
    pz_LteTdpUploadData[i].locationInformation.locationSource.u.uploadLocationSource = &z_TestCellDbRecord.z_UploadLocationSource;
    pz_LteTdpUploadData[i].locationInformation.locationData.m.horUncertConfidencePresent = 1;
    pz_LteTdpUploadData[i].locationInformation.locationData.m.horPosReliabilityPresent = 1;
    pz_LteTdpUploadData[i].locationInformation.locationData.m.altitudePresent = 1;
    pz_LteTdpUploadData[i].locationInformation.locationData.m.vertUncertainty_1_sigmaPresent = 1;
    pz_LteTdpUploadData[i].locationInformation.locationData.m.vertPosReliabilityPresent = 1;
    pz_LteTdpUploadData[i].locationInformation.locationData.latitude = 2147483647;
    pz_LteTdpUploadData[i].locationInformation.locationData.longitude = 2147483647;
    pz_LteTdpUploadData[i].locationInformation.locationData.horUncertainty = 127;
    pz_LteTdpUploadData[i].locationInformation.locationData.horUncertConfidence = 100;
    pz_LteTdpUploadData[i].locationInformation.locationData.horPosReliability = TDPv3PosReliability_very_high;
    pz_LteTdpUploadData[i].locationInformation.locationData.altitude = 15883;
    pz_LteTdpUploadData[i].locationInformation.locationData.vertUncertainty_1_sigma = 127;
    pz_LteTdpUploadData[i].locationInformation.locationData.vertPosReliability = TDPv3PosReliability_very_high;
    pz_LteTdpUploadData[i].locationInformation.positionTimestamp.t = T_TDPv3TimeStampGeneric_timeTickMsec;
    pz_LteTdpUploadData[i].locationInformation.positionTimestamp.u.timeTickMsec = 18446744073709551615;
    pz_LteTdpUploadData[i].locationInformation.velocity.horizontalSpeed = 1030;
    pz_LteTdpUploadData[i].locationInformation.velocity.horSpeedUncertainty = 640;
    pz_LteTdpUploadData[i].locationInformation.velocity.heading = 1024;
    pz_LteTdpUploadData[i].locationInformation.velocity.headingUncertainty = 64;
    pz_LteTdpUploadData[i].connectionMode = TDPv3LteTdpUploadData_connectionMode_connected;
    pz_LteTdpUploadData[i].lteTdpMeasAndCellData.n = NumOfNeighborInGrid;
    pz_LteTdpUploadData[i].lteTdpMeasAndCellData.elem = pz_LteTdpMeasAndCellData;
  }

  return pz_LteTdpUploadData;
}

/*==============================================================================
  FUNCTION      GenerateCellDbRecord
  
  DESCRIPTION 	
    This function generates a CellDbRecord ASN.1 structure.
  
  DEPENDENCIES
    None
  
  PARAMETERS
    NumOfTdpGrid - Number of TDP grid in each of the cell.
    (Input)

    NumOfNeighborInGrid - Number of neighbor cell in each of the scanlist.
    (Input)
  
  RETURN VALUE  
    The pointer to the initialized CellDbRecord ASN.1 structure.
  
  SIDE EFFECTS  
    None

==============================================================================*/
static const TDPv3CellDB_Record *GenerateCellDbRecord(
  int NumOfTdpGrid, int NumOfNeighborInGrid)
{
  TDPv3CellDB_Record *p_CellDbRecord = &z_TestCellDbRecord.z_CellDbRecord;

  p_CellDbRecord->m.recordUpdateTimePresent = 1;
  p_CellDbRecord->m.tdpUploadDataPresent = 1;
  p_CellDbRecord->recordUpdateTime.t = T_TDPv3TimeStampGeneric_timeTickMsec;
  p_CellDbRecord->recordUpdateTime.u.timeTickMsec = 18446744073709551615;
  p_CellDbRecord->cellDBCellIdentification.t = T_TDPv3CellDB_Record_cellDBCellIdentification_lteCellId;
  p_CellDbRecord->cellDBCellIdentification.u.lteCellId = &z_TestCellDbRecord.z_LteCellId;
  p_CellDbRecord->cellDBCellIdentification.u.lteCellId->m.earfcnPresent = 1;
  p_CellDbRecord->cellDBCellIdentification.u.lteCellId->m.plidPresent = 1;
  p_CellDbRecord->cellDBCellIdentification.u.lteCellId->mcc = 999;
  p_CellDbRecord->cellDBCellIdentification.u.lteCellId->mnc = 999;
  p_CellDbRecord->cellDBCellIdentification.u.lteCellId->earfcn = 262143;
  p_CellDbRecord->cellDBCellIdentification.u.lteCellId->plid = 503;
  p_CellDbRecord->cellDBCellIdentification.u.lteCellId->globalCellId = 268435455;
  p_CellDbRecord->cellDBCellIdentification.u.lteCellId->tacUpload = 65535;
  p_CellDbRecord->positionEstimateCellDB.m.positionTimestampPresent = 1;
  p_CellDbRecord->positionEstimateCellDB.m.velocityPresent = 1;
  p_CellDbRecord->positionEstimateCellDB.locationSource.t = T_TDPv3LocationInformation_locationSource_uploadLocationSource;
  p_CellDbRecord->positionEstimateCellDB.locationSource.u.uploadLocationSource = &z_TestCellDbRecord.z_UploadLocationSource;
  p_CellDbRecord->positionEstimateCellDB.locationSource.u.uploadLocationSource->t = T_TDPv3LocationInformation_locationSource_uploadLocationSource_sourceAllocation;
  p_CellDbRecord->positionEstimateCellDB.locationSource.u.uploadLocationSource->u.sourceAllocation = &z_TestCellDbRecord.z_SourceAllocation;
  p_CellDbRecord->positionEstimateCellDB.locationSource.u.uploadLocationSource->u.sourceAllocation->m.unspecifiedInternalPresent = 1;
  p_CellDbRecord->positionEstimateCellDB.locationSource.u.uploadLocationSource->u.sourceAllocation->m.unspecifiedExternalPresent = 1;
  p_CellDbRecord->positionEstimateCellDB.locationSource.u.uploadLocationSource->u.sourceAllocation->m.gnssPresent = 1;
  p_CellDbRecord->positionEstimateCellDB.locationSource.u.uploadLocationSource->u.sourceAllocation->m.sensorsPresent = 1;
  p_CellDbRecord->positionEstimateCellDB.locationSource.u.uploadLocationSource->u.sourceAllocation->m.wifiPresent = 1;
  p_CellDbRecord->positionEstimateCellDB.locationSource.u.uploadLocationSource->u.sourceAllocation->m.wwanPresent = 1;
  p_CellDbRecord->positionEstimateCellDB.locationSource.u.uploadLocationSource->u.sourceAllocation->unspecifiedInternal = 100;
  p_CellDbRecord->positionEstimateCellDB.locationSource.u.uploadLocationSource->u.sourceAllocation->unspecifiedExternal = 100;
  p_CellDbRecord->positionEstimateCellDB.locationSource.u.uploadLocationSource->u.sourceAllocation->gnss = 100;
  p_CellDbRecord->positionEstimateCellDB.locationSource.u.uploadLocationSource->u.sourceAllocation->sensors = 100;
  p_CellDbRecord->positionEstimateCellDB.locationSource.u.uploadLocationSource->u.sourceAllocation->wifi = 100;
  p_CellDbRecord->positionEstimateCellDB.locationSource.u.uploadLocationSource->u.sourceAllocation->wwan = 100;
  p_CellDbRecord->positionEstimateCellDB.locationData.m.horUncertConfidencePresent = 1;
  p_CellDbRecord->positionEstimateCellDB.locationData.m.horPosReliabilityPresent = 1;
  p_CellDbRecord->positionEstimateCellDB.locationData.m.altitudePresent = 1;
  p_CellDbRecord->positionEstimateCellDB.locationData.m.vertUncertainty_1_sigmaPresent = 1;
  p_CellDbRecord->positionEstimateCellDB.locationData.m.vertPosReliabilityPresent = 1;
  p_CellDbRecord->positionEstimateCellDB.locationData.latitude = 2147483647;
  p_CellDbRecord->positionEstimateCellDB.locationData.longitude = 2147483647;
  p_CellDbRecord->positionEstimateCellDB.locationData.horUncertainty = 127;
  p_CellDbRecord->positionEstimateCellDB.locationData.horUncertConfidence = 100;
  p_CellDbRecord->positionEstimateCellDB.locationData.horPosReliability = TDPv3PosReliability_very_high;
  p_CellDbRecord->positionEstimateCellDB.locationData.altitude = 15883;
  p_CellDbRecord->positionEstimateCellDB.locationData.vertUncertainty_1_sigma = 127;
  p_CellDbRecord->positionEstimateCellDB.locationData.vertPosReliability = TDPv3PosReliability_very_high;
  p_CellDbRecord->positionEstimateCellDB.positionTimestamp.t = T_TDPv3TimeStampGeneric_timeTickMsec;
  p_CellDbRecord->positionEstimateCellDB.positionTimestamp.u.timeTickMsec = 18446744073709551615;
  p_CellDbRecord->positionEstimateCellDB.velocity.horizontalSpeed = 1030;
  p_CellDbRecord->positionEstimateCellDB.velocity.horSpeedUncertainty = 640;
  p_CellDbRecord->positionEstimateCellDB.velocity.heading = 1024;
  p_CellDbRecord->positionEstimateCellDB.velocity.headingUncertainty = 64;
  p_CellDbRecord->tdpUploadData.servingCellConfiguration.m.systemTimePresent = 1;
  p_CellDbRecord->tdpUploadData.servingCellConfiguration.cellSystemBW = TDPv3LteSystemBandwidth_bw_20;
  p_CellDbRecord->tdpUploadData.servingCellConfiguration.cyclicPrefix = TRUE;
  p_CellDbRecord->tdpUploadData.servingCellConfiguration.antennaPorts = TRUE;
  p_CellDbRecord->tdpUploadData.servingCellConfiguration.systemTime.m.synchronousSystemTimePresent = 1;
  p_CellDbRecord->tdpUploadData.servingCellConfiguration.systemTime.m.timeInfoUTCPresent = 1;
  p_CellDbRecord->tdpUploadData.servingCellConfiguration.systemTime.asynchronousSystemTimePresent = TRUE;
  p_CellDbRecord->tdpUploadData.servingCellConfiguration.systemTime.synchronousSystemTime = 549755813887;
  p_CellDbRecord->tdpUploadData.servingCellConfiguration.systemTime.timeInfoUTC = 549755813887;
  p_CellDbRecord->tdpUploadData.scanSelectionMethod = TDPv3TdpUploadData_scanSelectionMethod_randomSelection;
  p_CellDbRecord->tdpUploadData.tdpDataObservations.t = T_TDPv3TdpUploadData_tdpDataObservations_lteTdpUploadData;
  p_CellDbRecord->tdpUploadData.tdpDataObservations.u.lteTdpUploadData = &z_TestCellDbRecord.z_LteTdpUploadDataSet;

  if (NumOfTdpGrid > ASN1V_TDPv3maxNumTdpObservationSets)
  {
    NumOfTdpGrid = ASN1V_TDPv3maxNumTdpObservationSets;
  }

  p_CellDbRecord->tdpUploadData.tdpDataObservations.u.lteTdpUploadData->n = NumOfTdpGrid;
  p_CellDbRecord->tdpUploadData.tdpDataObservations.u.lteTdpUploadData->elem =
    GenerateLteTdpUploadData(NumOfNeighborInGrid);

  return p_CellDbRecord;
}

/*==============================================================================
  FUNCTION      tle_Asn1EncoderTesterClass::TestCellArrayEncoding
  
  DESCRIPTION 	
    Test case for Cell Array encoding
  
  DEPENDENCIES
    None
  
  PARAMETERS
    None
  
  RETURN VALUE  
    true  - Test passed
    false - Test failed
  
  SIDE EFFECTS  
    None

==============================================================================*/
bool tle_Asn1EncoderTesterClass::TestCellArrayEncoding()
{
  bool b_Result = false;

  // Encode 128 cells X 50 grids X 24 neighbor
  const int l_NumOfCells = 128;
  const int l_NumOfGridsPerCell = 50;
  const int l_NumOfNeighborsPerCell = 24;
  const TDPv3CellDB_Record *pzCellDbRecord =
    GenerateCellDbRecord(l_NumOfGridsPerCell, l_NumOfNeighborsPerCell);
  TDPv3Cell_Record_Array z_CellRecordArray;
  uint32 q_BytesWritten = 0;

  z_CellRecordArray.n = l_NumOfCells;
  z_CellRecordArray.elem = static_cast<TDPv3CellDB_Record *>(gnss_HalClass::MemAlloc(CGPS_TLE_MODULE_TEST,
                                                                                    sizeof(TDPv3CellDB_Record)*l_NumOfCells));

  if (z_CellRecordArray.elem)
  {
    const uint32 q_BufLen = 6 * 1024 * 1024; /* 6MB */
    char *pBuf = static_cast<char *>(gnss_HalClass::MemAlloc(CGPS_TLE_MODULE_TEST, q_BufLen));

    if (NULL != pBuf)
    {
      // Populate the same CellDbRecord to all elements
      for (int i=0; i<l_NumOfCells; i++)
      {
        z_CellRecordArray.elem[i] = *pzCellDbRecord;
      }

      b_Result = m_pzAsn1Encoder->EncodeData(&z_CellRecordArray, q_BufLen, pBuf, q_BytesWritten);

      gnss_HalClass::MemFree(CGPS_TLE_MODULE_TEST,
                            (void **)&z_CellRecordArray.elem,
                            sizeof(*z_CellRecordArray.elem) * l_NumOfCells);

      gnss_HalClass::MemFree(CGPS_TLE_MODULE_TEST,
                            (void **)&pBuf, 
                            sizeof(*pBuf) * q_BufLen);
    }
  }

  return b_Result;
}

/*==============================================================================
  FUNCTION      tle_Asn1EncoderTesterClass::TestCellDbRecordToAsn1Struct
  
  DESCRIPTION 	
    Test case for converting CellDbRecord to ASN.1 Structure
  
  DEPENDENCIES
    None
  
  PARAMETERS
    None
  
  RETURN VALUE  
    true  - Test passed
    false - Test failed
  
  SIDE EFFECTS  
    None

==============================================================================*/
bool tle_Asn1EncoderTesterClass::TestCellDbRecordToAsn1Struct()
{
  tle_CellDbNameSpace::CellDbRecordClass z_CellDbRecord;

  /* TODO: run full test */

  return true;
}

#endif /* FEATURE_CGPS_PLAYBACK */