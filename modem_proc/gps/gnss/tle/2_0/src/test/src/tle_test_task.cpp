/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TLE test task implementation
GENERAL DESCRIPTION
   This file contains constants, classes, structures and function implementations
   for TLE (e.g Terrestrial Downlink Positioning) task.


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

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/tle/2_0/src/test/src/tle_test_task.cpp#2 $
  $DateTime: 2021/09/01 02:25:50 $
  $Author: pwbldsvc $

when        who     what, where, why
--------    ---   ----------------------------------------------------------
6/01/2015  ahmadh Initial version

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#ifdef FEATURE_CGPS_PLAYBACK
#include <iostream>

#ifdef PE_PLAYBACK_AP_CLIENT
#include <fstream>
#include <string>
#endif /* PE_PLAYBACK_AP_CLIENT */

#include "tle_test_task.h"
#include "tle_task.h"
#include "pe_playback.h"

/*==============================================================================

       static Variables

==============================================================================*/

tle_PtmMgrClass  * tle_PbTestCellClass::m_pzPtm = NULL;
tle_HalCellClass * tle_PbTestCellClass::m_pzCellMgr = NULL;


const tle_WcdmaTestCellPosType cz_WcdmaTestCells[2] =
{
  { 
    { 1, 0xFFFF, 0, { 0.652305, -2.12891, 120.0 }, 50.0F, 100.0F, PDSM_POSITION_RELIABILITY_MEDIUM, PDSM_POSITION_RELIABILITY_LOW,
      { 0, 0, 0xFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0, 0, 0.0F, 0.0F, 0.0F, 0.0F, 17 },
    310, 410, 0, 5838187, 15, 20, 1, 0
  },

  {
    { 1, 0xFFFF, 0, { 0.652323, -2.1289, 120.0 } , 50.00F, 100.0F, PDSM_POSITION_RELIABILITY_MEDIUM, PDSM_POSITION_RELIABILITY_LOW,
    { 0, 0, 0xFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } , 0, 0, 0.0F, 0.0F, 0.0F, 0.0F, 17 },
    310, 410, 0, 5838188, 15, 20, 1, 0
  }
};

tle_PbTestWcdmaCellClass::tle_PbTestWcdmaCellClass(const tle_WcdmaTestCellPosType & cz_WcdmaCell):
tle_PbTestCellClass(cz_WcdmaCell.z_Pos)
{
  m_zCell.b_CellIdValid = cz_WcdmaCell.b_CellIdValid;
  m_zCell.u_SubscrId    = cz_WcdmaCell.u_SubscrId;
  m_zCell.z_Cell.e_ActiveRat = CELLDB_ACTIVERAT_WCDMA;
  m_zCell.z_Cell.z_Cell.z_Wcdma.z_Id.w_Mcc  = cz_WcdmaCell.w_L1;
  m_zCell.z_Cell.z_Cell.z_Wcdma.z_Id.w_Mnc  = cz_WcdmaCell.w_L2;
  m_zCell.z_Cell.z_Cell.z_Wcdma.z_Id.w_Lac  = cz_WcdmaCell.w_L3;
  m_zCell.z_Cell.z_Cell.z_Wcdma.z_Id.q_UcId = cz_WcdmaCell.q_L4;
  m_zCell.z_Cell.z_Cell.z_Wcdma.w_Psc = cz_WcdmaCell.psc;
  m_zCell.z_Cell.z_Cell.z_Wcdma.z_FreqList.Arr_wFreq[0] = cz_WcdmaCell.freq;
  m_zCell.z_Cell.z_Cell.z_Wcdma.z_FreqList.u_NumOfValidFreq = 1;
  m_zCell.z_WcdmaCell.freq = cz_WcdmaCell.freq;
  m_zCell.z_WcdmaCell.psc = cz_WcdmaCell.psc;
}


/*==============================================================================

       static Functions

==============================================================================*/

/*=============================================================================

       Class and method implementations

=============================================================================*/

/*=============================================================================

  FUNCTION      PbAllocateMaxSpace()

  DESCRIPTION
    This method attempts to allocate maximum amount of memory (500 K) to
    the virtual memory manager for PE playback 
  

  DEPENDENCIES  

  PARAMETERS
    Size - The allocated size is reported in this variable
    (output)
 
  RETURN VALUE

  SIDE EFFECTS  

=============================================================================*/
void tle_TaskTesterClass::PbAllocateMaxSpace(size_t & Size)
{
  m_pzTleTask->m_DbmMgr.AllocateMaxSpace(Size);
}

#ifdef PE_PLAYBACK_AP_CLIENT

/*=============================================================================

  FUNCTION      tle_TaskTesterClass::TestSelfLearned()

  DESCRIPTION
    This method injects an array of predefined cells with positions into TLE.
    The ininjection, includes a cell change to the cell and position injection
    for the cell. After injecting all cells a GTP upload session is initiated.
  

  DEPENDENCIES  

  PARAMETERS
 
  RETURN VALUE

  SIDE EFFECTS  

=============================================================================*/
void tle_TaskTesterClass::TestSelfLearned()
{
  for(uint32 q_i=0;q_i<2;++q_i)
  {
    tle_PbTestWcdmaCellClass z_TestWcdmaCell = tle_PbTestWcdmaCellClass(cz_WcdmaTestCells[q_i]);
    z_TestWcdmaCell.InjectSelfLearnedPos();

    m_pzTleTask->m_PtmMgr.DeletePosition();
  }

  PbTestUpload(C_TDP_UPLD_SRC_PERIODIC_FORCED);
}

/*=============================================================================

  FUNCTION      tle_TaskTesterClass::PbTestDownload()

  DESCRIPTION
    This method attempts to start a GTP download session. 
  

  DEPENDENCIES  

  PARAMETERS
    e_Src - the source for initiating the session
    (input)
 
   t_CurrentTimeMs - timetick msec at session start
   (input)
 
  RETURN VALUE
    true  - If session is initiated
    false - Otherwise

  SIDE EFFECTS  

=============================================================================*/
bool tle_TaskTesterClass::PbTestDownload(e_TleSessionSrcType e_Src, uint64 t_CurrentTimeMs)
{
  if (m_pzTleTask->m_DownloadMgr.InitiateTdpDownload(e_Src, t_CurrentTimeMs, true))
  {
    if(m_pzTleTask->m_zApClient.ProcessNextMsg(e_ApClient_QmiLocGdtDownloadBeginStatusReq))
    {
      do
      {
        ApClientMsgStatusType e_Result = m_pzTleTask->m_zApClient.RcvFromAp(e_ApClient_QmiLocGdtDownloadReadyStatusReq);
        switch(e_Result)
        {
        case e_RequestedDataIsReady:
          if(m_pzTleTask->m_zApClient.ProcessNextMsg(e_ApClient_QmiLocGdtDownloadReadyStatusReq))
          {
            bool b_FileClosed = m_pzTleTask->m_zApClient.VerifyForClose();
            if(m_pzTleTask->m_zApClient.VerifyForEnd())
            {
              std::cout << "GDT session ended" << std::endl;
              return true;
            }
            else 
            {
              if(e_GDT_SUCCESS == m_pzTleTask->m_zApClient.RcvGdtDownloadOpenReponse())
              {
                int count = 1;
                b_FileClosed = false;
                while (e_GDT_SUCCESS == m_pzTleTask->m_zApClient.RcvGdtDownloadRcvDataAck())
                {
                  ++count;
                  b_FileClosed = m_pzTleTask->m_zApClient.VerifyForClose();

                  if(m_pzTleTask->m_zApClient.VerifyForEnd())
                  {
                    std::cout << "GDT session ended" << std::endl;
                    return true;
                  }

                  if(b_FileClosed)
                  {
                    break; 
                  }
                }
              }
              /* Download is completed */
              continue;
            }
          }
          break;

        case e_GdtCloseIsPending:
          m_pzTleTask->m_zApClient.VerifyForClose();
          if(m_pzTleTask->m_zApClient.VerifyForEnd())
          {
            std::cout << "GDT session ended" << std::endl;
            return true;
          }
          break;

        case e_GdtEndIsPending:
          if(m_pzTleTask->m_zApClient.VerifyForEnd())
          {
            std::cout << "GDT session ended" << std::endl;
            return true;
          }
          break;

        default:
          std::cout << "Invalid result:" << e_Result << std::endl;
        }
      } while (1);
    }
    else
    {
      std::cout << "Did not receive begin response" << std::endl;
    }
  }
  else
  {
    std::cout << "TDP Session request failed" << std::endl;
  }

  return false;
}

/*=============================================================================

  FUNCTION      tle_TaskTesterClass::PbTestApClient()

  DESCRIPTION
    This method is used for generating multiple cell chnages and GTP sessions
    for testing in Playback environment.
  

  DEPENDENCIES  

  PARAMETERS
 
  RETURN VALUE

  SIDE EFFECTS  

=============================================================================*/
void tle_TaskTesterClass::PbTestApClient()
{
  uint64 t_CurrentTimeMs = 0;

  t_CurrentTimeMs = cgps_TimeTickGetMsec() + 650000;
  pb_SetTimeTickMsec(t_CurrentTimeMs);
  InitiateWcdmaCellChange();

  t_CurrentTimeMs = cgps_TimeTickGetMsec() + 650000;
  pb_SetTimeTickMsec(t_CurrentTimeMs);
  InitiateCdmaCellChange();

  t_CurrentTimeMs = cgps_TimeTickGetMsec() + 650000;
  pb_SetTimeTickMsec(t_CurrentTimeMs);
  InitiateGsmCellChange(); 

  t_CurrentTimeMs = cgps_TimeTickGetMsec() + 650000;
  pb_SetTimeTickMsec(t_CurrentTimeMs);
  InitiateLteCellChange();

  tle_PbTestCellClass::SetManagers(&m_pzTleTask->m_PtmMgr,
                                   &m_pzTleTask->m_HalCellMgr);
  TestSelfLearned();
}

/*=============================================================================

  FUNCTION      tle_TaskTesterClass::PbTestUpload()

  DESCRIPTION
    This method is used for initiating an upload session in test mode.
  

  DEPENDENCIES  

  PARAMETERS
    e_Src - The source for intiating the session
    (input)
 
  RETURN VALUE
    true  - If session is initiated
    false - Otherwise

  SIDE EFFECTS  

=============================================================================*/
bool tle_TaskTesterClass::PbTestUpload(e_TleSessionSrcType e_Src)
{
  if (m_pzTleTask->m_UploadMgr.InitiateTdpUpload(e_Src))
  {
    if (m_pzTleTask->m_zApClient.ProcessNextMsg(e_Apclient_QmiLocGdtUplBeginStatusReqMsg))
    {
      if( e_GDT_SUCCESS == m_pzTleTask->m_zApClient.UpldProcOpenResponse())
      {
        std::cout << "Wrote client info" << std::endl;
        do
        {
          m_pzTleTask->m_zApClient.UpldProcSendAck();
        } while(!m_pzTleTask->m_zApClient.UploadFileClosed());

        if(m_pzTleTask->m_zApClient.ProcessNextMsg(e_ApClient_QmiLocGdtUpldEndReqMsg))
        {
          return true;
        }
      }
    }
  }
  return false;
}

/*=============================================================================

FUNCTION      tle_TaskTesterClass::PbProcessServingCellList()

DESCRIPTION
This method reads all the cell entries in the internal list and intiate a cell
change based on the entries. If the XTA for the cell does not exist a GTP session 
will be initiated.

DEPENDENCIES

PARAMETERS

RETURN VALUE


SIDE EFFECTS
Memory will be allocated for storing this information

=============================================================================*/
void tle_TaskTesterClass::PbProcessServingCellList()
{
  while(m_zDownldCellList.getNumOfElements() > 0)
  {
    std::string *pzThisStr = m_zDownldCellList.front();
    if(NULL != pzThisStr)
    {
      gnss_StlNameSpace::gnss_VectorClass<FLT> z_Vector;
      if (ParseDelimitedString(*pzThisStr, ',', z_Vector))
      {
        uint64 t_CurrentTimeMs = 0;

        /* take current time forward to prevent session throttling */
        t_CurrentTimeMs = cgps_TimeTickGetMsec() + 650000;
        pb_SetTimeTickMsec(t_CurrentTimeMs);

        uint8 u_Rat = static_cast<uint8>(z_Vector[0]);

        /* Determin Rat */
        switch(u_Rat)
        {
        case C_GTP_RAT_CDMA:
          {
            tle_CdmaServingCellType z_Cell;

            memset(&z_Cell, 0x00, sizeof(z_Cell));
            z_Cell.z_BsInfo.b_StaleInfo = FALSE;
            z_Cell.z_BsInfo.u_BandClass = 1; /* Dummy setup for test */
            z_Cell.z_BsInfo.w_BaseId = static_cast<uint16>(z_Vector[4]);
            z_Cell.z_BsInfo.w_Sid = static_cast<uint16>(z_Vector[2]);
            z_Cell.z_BsInfo.w_Nid = static_cast<uint16>(z_Vector[3]);;
            z_Cell.z_BsInfo.w_PilotPn = 1; /* Dummy setup for test */

            z_Cell.z_SSInfo.base_id = z_Cell.z_BsInfo.w_BaseId;
            z_Cell.z_SSInfo.changed_fields = z_Cell.z_BsInfo.w_Channel;
            z_Cell.z_SSInfo.sid = z_Cell.z_BsInfo.w_Sid;
            z_Cell.z_SSInfo.nid = z_Cell.z_BsInfo.w_Nid;
            z_Cell.z_SSInfo.srv_status = SYS_SRV_STATUS_SRV;
            z_Cell.z_SSInfo.ss_data_type = CGPS_SSINFO_SRV_SYS;
            z_Cell.z_SSInfo.sys_mode = SYS_SYS_MODE_CDMA;
            z_Cell.z_SSInfo.z_TimeZoneInfo.time_zone_avail = FALSE;
            z_Cell.u_SubscrId = 0;

            m_pzTleTask->m_HalCellMgr.PbCdmaSsInfoUpdate(z_Cell);
          }
          break;

        case C_GTP_RAT_GSM:
          {
            tle_GsmCellInfoType z_Cell;
            memset(&z_Cell, 0x00, sizeof(z_Cell));

            z_Cell.b_CellIdValid = TRUE;
            z_Cell.u_SubscrId = 0;

            z_Cell.z_Cell.e_ActiveRat = CELLDB_ACTIVERAT_GSM;
            z_Cell.z_Cell.z_Cell.z_Gsm.z_Id.w_Mcc = static_cast<uint16>(z_Vector[1]);
            z_Cell.z_Cell.z_Cell.z_Gsm.z_Id.w_Mnc = static_cast<uint16>(z_Vector[2]);
            z_Cell.z_Cell.z_Cell.z_Gsm.z_Id.w_Lac = static_cast<uint16>(z_Vector[3]);
            z_Cell.z_Cell.z_Cell.z_Gsm.z_Id.w_CellId = static_cast<uint16>(z_Vector[4]);

            z_Cell.z_Cell.z_Cell.z_Gsm.u_Band = 1; /* Dummy for test */
            z_Cell.z_Cell.z_Cell.z_Gsm.z_FreqList.u_NumOfValidFreq = 1;
            z_Cell.z_Cell.z_Cell.z_Gsm.z_FreqList.Arr_wArfcn[0] = 180; /* Dummy for test */
            z_Cell.z_Cell.z_Cell.z_Gsm.u_Bsic = 15; /* Dummy for test */

            m_pzTleTask->m_HalCellMgr.GsmCellChange(z_Cell);
          }
          break;

        case C_GTP_RAT_WCDMA:
          {
            tle_WcdmaCellInfoType z_Cell;
            memset(&z_Cell, 0x00, sizeof(z_Cell));

            z_Cell.b_CellIdValid = TRUE;
            z_Cell.u_SubscrId = 0;
            z_Cell.z_Cell.e_ActiveRat = CELLDB_ACTIVERAT_WCDMA;

            z_Cell.z_Cell.z_Cell.z_Wcdma.z_Id.w_Mcc = static_cast<uint16>(z_Vector[1]);
            z_Cell.z_Cell.z_Cell.z_Wcdma.z_Id.w_Mnc = static_cast<uint16>(z_Vector[2]);
            z_Cell.z_Cell.z_Cell.z_Wcdma.z_Id.w_Lac = static_cast<uint16>(z_Vector[3]);
            z_Cell.z_Cell.z_Cell.z_Wcdma.z_Id.q_UcId = static_cast<uint32>(z_Vector[4]);

            z_Cell.z_Cell.z_Cell.z_Wcdma.w_Psc = 307; /* Dummy for test */
            z_Cell.z_Cell.z_Cell.z_Wcdma.z_FreqList.u_NumOfValidFreq = 1;
            z_Cell.z_Cell.z_Cell.z_Wcdma.z_FreqList.Arr_wFreq[0] = 4360; /* Dummy for test */

            m_pzTleTask->m_HalCellMgr.WcdmaCellChange(z_Cell);
          }
          break;

        case C_GTP_RAT_LTE:
          {
            tle_LteCellInfoType z_Cell;
            memset(&z_Cell, 0x00, sizeof(z_Cell));

            z_Cell.b_CellIdValid = TRUE;
            z_Cell.b_PreferCdma = TRUE;
            z_Cell.u_SubscrId = 0;
            z_Cell.z_Cell.e_ActiveRat = CELLDB_ACTIVERAT_LTE;

            z_Cell.z_Cell.z_Cell.z_Lte.z_Id.w_Mcc = static_cast<uint16>(z_Vector[1]);
            z_Cell.z_Cell.z_Cell.z_Lte.z_Id.w_Mnc = static_cast<uint16>(z_Vector[2]);
            z_Cell.z_Cell.z_Cell.z_Lte.z_Id.w_Tac = static_cast<uint16>(z_Vector[3]);
            z_Cell.z_Cell.z_Cell.z_Lte.z_Id.q_CI = static_cast<uint32>(z_Vector[4]);

            z_Cell.z_Cell.z_Cell.z_Lte.w_PhysCellId = 271;              /* Dummy for test */
            z_Cell.z_Cell.z_Cell.z_Lte.z_FreqList.u_NumOfValidFreq = 1; /* Dummy for test */
            z_Cell.z_Cell.z_Cell.z_Lte.z_FreqList.Arr_qFreq[0] = 5780;  /* Dummy for test */
            z_Cell.z_Cell.z_Cell.z_Lte.z_FreqList.w_DlFreq = 5780;      /* Dummy for test */

            m_pzTleTask->m_HalCellMgr.LteCellChange(z_Cell);
          }
          break;

        default:
          TLE_MSG_HIGH_1("Invalid rat:%d", u_Rat);
        }
      }
      z_Vector.clear();
    }
    gnss_HalClass::MemFree(CGPS_TLE_MODULE_TEST, reinterpret_cast<void **>(&pzThisStr), sizeof(*pzThisStr));
    m_zDownldCellList.pop_front();
  }
}

/*=============================================================================

FUNCTION      tle_TaskTesterClass::PbStoreServingCell()

DESCRIPTION
This method stores a requested serving cell into a list of cells, to be used 
for GTP download when a AP ready message is received from AP.

DEPENDENCIES

PARAMETERS

pc_CellStr - The servong cell in comma delimited format
(input)      (rat, L1, L2, L3, L4)

q_Len - The length of the input string
(output)

RETURN VALUE
true  - If the cell is stored in the internal list
false - otherwise

SIDE EFFECTS
Memory will be allocated for storing this information

=============================================================================*/
bool tle_TaskTesterClass::PbStoreServingCell(const char *pc_CellStr, uint32 q_Len)
{
  if( (NULL != pc_CellStr) &&
      (q_Len > 0) )
  {
    std::string *pz_thisStr = NULL;
    
    GNSS_NEW_ARGS(CGPS_TLE_MODULE_TEST, std::string, pz_thisStr, pc_CellStr);
    if(NULL != pz_thisStr)
    {
      m_zDownldCellList.push_back(*pz_thisStr);
      return true;
    }
  }
  
  return false;
}

/*=============================================================================

FUNCTION      tle_TaskTesterClass::PbProcessServingCellFile()

DESCRIPTION
This method reads a .txt file which contains cell ID's in a ',' delimited 
format. (rat, mcc, mnc/sid, lac/tac/nid, cellId) and stores the cells 
in a list to be used for serving cell information for GTP downloads. 

DEPENDENCIES

PARAMETERS

pc_CellFileStr - The file to be read
(input)      

q_Len - The length of the filename
(output)

RETURN VALUE
true  - If the file is read successfuly
false - otherwise

SIDE EFFECTS

=============================================================================*/
bool tle_TaskTesterClass::PbProcessServingCellFile(const char *pc_CellFileStr, 
                                                   uint32 q_Len)
{
  if ((NULL != pc_CellFileStr) &&
    (q_Len > 0))
  {
    std::string CellStr;
    std::ifstream thisFile(pc_CellFileStr);
    if (thisFile.is_open())
    {
      while (getline(thisFile, CellStr))
      {
        if (PbStoreServingCell(CellStr.c_str(), CellStr.size()))
        {
          std::cout << "Stored cell: " << CellStr << std::endl;
        }
      }
      thisFile.close();
      return true;
    }
  }

  return false;
}

/*=============================================================================

FUNCTION      tle_TaskTesterClass::ConvertListtoVector()

DESCRIPTION
This method converts a numeric list to an equivalent vector. If the conversion
is successful, the input list will be released and vector will contain the 
converted data


DEPENDENCIES
  rz_NumList - The input list which contains numeric values
  (input/output)

  rz_Vector - The output vector which contains numeric values
  (output)

PARAMETERS

RETURN VALUE
  true  - The conversion is successful
  false - otherwise

SIDE EFFECTS
  Memory will be allocated for generated vector, and the memory used by the
  list will be released

=============================================================================*/
bool tle_TaskTesterClass::ConvertListtoVector(gnss_StlNameSpace::gnss_ListClass<FLT> &rz_NumList,
                                              gnss_StlNameSpace::gnss_VectorClass<FLT> &rz_Vector)
{
  if(rz_Vector.reserve(rz_NumList.getNumOfElements()))
  {
    gnss_StlNameSpace::gnss_ListClass<FLT>::iterator num_pos;
    gnss_StlNameSpace::gnss_ListClass<FLT>::iterator num_end_pos;
    uint32 q_Count = 0;

    num_end_pos = rz_NumList.end();
    for (num_pos = rz_NumList.begin(); num_pos != num_end_pos; ++num_pos)
    {
      FLT *pz_thisNum = *num_pos;

      if(NULL != pz_thisNum)
      {
        rz_Vector[q_Count] = *pz_thisNum;
        ++q_Count;
        gnss_HalClass::MemFree(CGPS_TLE_MODULE_TEST, (void **) &pz_thisNum, sizeof(*pz_thisNum));
      }
    }
    rz_NumList.clear();
    return true;
  }
  return false;
}

/*=============================================================================

FUNCTION      tle_TaskTesterClass::ConvertList()

DESCRIPTION
This method converts a list of strings into a list of floating point numbers.

DEPENDENCIES

PARAMETERS
rz_StrList - The input string with numbers in string format
(input/output)


rz_NumList - The converted list with floating point entries
(output)

RETURN VALUE
true  - The operation is successful
false - otherwise

SIDE EFFECTS
Memory will be allocated for the output list, which needs to be releasd
by the caller. If the conversion is successful, the memorya allocated for
the input list will be released.

=============================================================================*/
bool tle_TaskTesterClass::ConvertList(gnss_StlNameSpace::gnss_ListClass<std::string> &rz_StrList,
                                      gnss_StlNameSpace::gnss_ListClass<FLT> &rz_NumList)
{
  bool b_Result = true;
  gnss_StlNameSpace::gnss_ListClass<std::string>::iterator str_pos;
  gnss_StlNameSpace::gnss_ListClass<std::string>::iterator str_end_pos;

  str_end_pos = rz_StrList.end();
  for (str_pos = rz_StrList.begin(); str_pos != str_end_pos; ++str_pos)
  {
    std::string *pz_thisStr = *str_pos;

    if(NULL != pz_thisStr)
    {
      FLT *p_ThisNum = static_cast<FLT *>(gnss_HalClass::MemAlloc(CGPS_TLE_MODULE_TEST, sizeof(*p_ThisNum)));
      
      if(NULL != p_ThisNum)
      {
        *p_ThisNum = std::stof(*pz_thisStr, NULL);
        if(!rz_NumList.push_back(*p_ThisNum))
        {
          b_Result = false;
          break;
        }
      }
      else
      {
        b_Result = false;
        break;
      }
    }
  }

  if(b_Result)
  {
    /* Release the memory used by input list */
    for (str_pos = rz_StrList.begin(); str_pos != str_end_pos; ++str_pos)
    {
      std::string *pz_thisStr = *str_pos;

      if(NULL != pz_thisStr)
      {
        gnss_HalClass::MemFree(CGPS_TLE_MODULE_TEST, (void **) &pz_thisStr, sizeof(*pz_thisStr));
      }
    }
    rz_StrList.clear();
  }
  else
  {
    /* since there is an error, release the memory used by input list */
    gnss_StlNameSpace::gnss_ListClass<FLT>::iterator num_pos;
    gnss_StlNameSpace::gnss_ListClass<FLT>::iterator num_end_pos;

    num_end_pos = rz_NumList.end();
    for (num_pos = rz_NumList.begin(); num_pos != num_end_pos; ++num_pos)
    {
      FLT *pz_thisNum = *num_pos;

      if(NULL != pz_thisNum)
      {
        gnss_HalClass::MemFree(CGPS_TLE_MODULE_TEST, (void **) &pz_thisNum, sizeof(*pz_thisNum));
      }
    }
    rz_NumList.clear();
  }

  return b_Result;
}

/*=============================================================================

FUNCTION      tle_TaskTesterClass::ParseDelimitedString()

DESCRIPTION
This method parses a character delimited input string of numbers to a vector of
floating point numbers.

DEPENDENCIES

PARAMETERS
cr_String - The input string to be parsed
(input)

c_Delim - The delimiter character
(input)

rz_Vector - Contains the parsed numeric vector
(output)

RETURN VALUE
true  - The operation is successful
false - otherwise

SIDE EFFECTS
Memory will be allocated for the numeric vector, which needs to be releasd
by the caller.

=============================================================================*/
bool tle_TaskTesterClass::ParseDelimitedString(const std::string &cr_String, char c_Delim,
                                               gnss_StlNameSpace::gnss_VectorClass<FLT> &rz_Vector)
{
  gnss_StlNameSpace::gnss_ListClass<FLT> z_NumList;

  if(ParseDelimitedString(cr_String, c_Delim, z_NumList))
  {
    return ConvertListtoVector(z_NumList, rz_Vector);
  }

  return false;
}

/*=============================================================================

FUNCTION      tle_TaskTesterClass::ParseDelimitedString()

DESCRIPTION
This method parses a character delimited input string of numbers to a list of 
floating point numbers.

DEPENDENCIES

PARAMETERS
cr_String - The input string to be parsed
(input)

c_Delim - The delimiter character
(input)

rz_NumList - Contains the parsed numeric values
(output)

RETURN VALUE
true  - The operation is successful
false - otherwise

SIDE EFFECTS
Memory will be allocated for the numeric list, which needs to be releasd 
by the caller.

=============================================================================*/
bool tle_TaskTesterClass::ParseDelimitedString(const std::string &cr_String, char c_Delim,
                                               gnss_StlNameSpace::gnss_ListClass<FLT> &rz_NumList)
{
  gnss_StlNameSpace::gnss_ListClass<std::string> z_StrList;

  if(ParseDelimitedString(cr_String, c_Delim, z_StrList))
  {
    return ConvertList(z_StrList, rz_NumList);
  }
  return false;
}

/*=============================================================================

FUNCTION      tle_TaskTesterClass::ParseDelimitedString()

DESCRIPTION
This method parses a character delimited input string to a list of delimited 
substrings.

DEPENDENCIES

PARAMETERS
cr_String - The input string to be parsed
(input)

c_Delim - The delimiter character
(input)

rz_TheList - Contains the parsed strings list
(output)


RETURN VALUE
true  - The operation is successful
false - otherwise

SIDE EFFECTS
Memory will be allocated for the parsed list, which needs to be releasd
by the caller.

=============================================================================*/
bool tle_TaskTesterClass::ParseDelimitedString(const std::string &rc_String, char c_Delim,
                                               gnss_StlNameSpace::gnss_ListClass<std::string> &rz_TheList)
{
  std::string::size_type StartIdx = 0;

  do
  {
    std::string ThisString(rc_String, StartIdx, rc_String.size());
    std::string::size_type idx = ThisString.find(c_Delim);
    std::string *pz_thisStr = NULL;

    if (idx != std::string::npos)
    {
      GNSS_NEW_ARGS(CGPS_TLE_MODULE_TEST, std::string, pz_thisStr, ThisString, 0, idx);
      if (NULL != pz_thisStr)
      {
        rz_TheList.push_back(*pz_thisStr);
        StartIdx += idx + 1;
      }
      else
      {
        return false;
      }
    }
    else
    {
      GNSS_NEW_ARGS(CGPS_TLE_MODULE_TEST, std::string, pz_thisStr, ThisString, 0, ThisString.size());
      if (NULL != pz_thisStr)
      {
        rz_TheList.push_back(*pz_thisStr);
      }
      else
      {
        return false;
      }
      break;
    }
  } while (true);

  return true;
}
#endif /* PE_PLAYBACK_AP_CLIENT */

tle_TaskTesterClass::tle_TaskTesterClass()
{

}

/*=============================================================================

  FUNCTION      tle_TaskTesterClass::InitiateGsmCellChange()

  DESCRIPTION
    This method is used for generating GSM cell change.
  

  DEPENDENCIES  

  PARAMETERS
 
  RETURN VALUE

  SIDE EFFECTS  

=============================================================================*/
void tle_TaskTesterClass::InitiateGsmCellChange()
{
  tle_GsmCellInfoType m_zGsmCell;

  m_zGsmCell.b_CellIdValid = TRUE;
  m_zGsmCell.u_SubscrId = 0;

  m_zGsmCell.z_Cell.e_ActiveRat = CELLDB_ACTIVERAT_GSM;
  m_zGsmCell.z_Cell.z_Cell.z_Gsm.z_Id.w_Mcc = 310;
  m_zGsmCell.z_Cell.z_Cell.z_Gsm.z_Id.w_Mnc = 410;
  m_zGsmCell.z_Cell.z_Cell.z_Gsm.z_Id.w_Lac = 6037;
  m_zGsmCell.z_Cell.z_Cell.z_Gsm.z_Id.w_CellId = 7844;

  m_zGsmCell.z_Cell.z_Cell.z_Gsm.u_Band = 4;
  m_zGsmCell.z_Cell.z_Cell.z_Gsm.z_FreqList.u_NumOfValidFreq = 1;
  m_zGsmCell.z_Cell.z_Cell.z_Gsm.z_FreqList.Arr_wArfcn[0] = 180;
  m_zGsmCell.z_Cell.z_Cell.z_Gsm.u_Bsic = 15;

  m_pzTleTask->m_HalCellMgr.GsmCellChange(m_zGsmCell);
}


/*=============================================================================

  FUNCTION      tle_TaskTesterClass::InitiateWcdmaCellChange()

  DESCRIPTION
    This method is used for generating WCDMA cell change.
  

  DEPENDENCIES  

  PARAMETERS
 
  RETURN VALUE

  SIDE EFFECTS  

=============================================================================*/
void tle_TaskTesterClass::InitiateWcdmaCellChange()
{
  tle_WcdmaCellInfoType z_Cell;

  z_Cell.b_CellIdValid = TRUE;
  z_Cell.u_SubscrId = 0;

  z_Cell.z_Cell.e_ActiveRat = CELLDB_ACTIVERAT_WCDMA;
  z_Cell.z_Cell.z_Cell.z_Wcdma.z_Id.w_Mcc = 310;
  z_Cell.z_Cell.z_Cell.z_Wcdma.z_Id.w_Mnc = 410;
  z_Cell.z_Cell.z_Cell.z_Wcdma.z_Id.w_Lac = 56952;
  z_Cell.z_Cell.z_Cell.z_Wcdma.z_Id.q_UcId = 5539498;

  z_Cell.z_Cell.z_Cell.z_Wcdma.w_Psc = 307;
  z_Cell.z_Cell.z_Cell.z_Wcdma.z_FreqList.u_NumOfValidFreq = 1;
  z_Cell.z_Cell.z_Cell.z_Wcdma.z_FreqList.Arr_wFreq[0] = 4360;

  m_pzTleTask->m_HalCellMgr.WcdmaCellChange(z_Cell);
}

/*=============================================================================

  FUNCTION      tle_TaskTesterClass::InitiateCdmaCellChange()

  DESCRIPTION
    This method is used for generating CDMA cell change.
  

  DEPENDENCIES  

  PARAMETERS
 
  RETURN VALUE

  SIDE EFFECTS  

=============================================================================*/
void tle_TaskTesterClass::InitiateCdmaCellChange()
{
  tle_CdmaServingCellType z_Cell;

  z_Cell.z_BsInfo.b_StaleInfo = FALSE;
  z_Cell.z_BsInfo.u_BandClass = 1;
  z_Cell.z_BsInfo.w_BaseId = 36385;
  z_Cell.z_BsInfo.w_Channel = 0;
  z_Cell.z_BsInfo.w_Sid = 4183; 
  z_Cell.z_BsInfo.w_Nid = 233;
  z_Cell.z_BsInfo.w_PilotPn = 24;

  z_Cell.z_Mask.BsLocValid = 0;
  z_Cell.z_Mask.DayLightSaveTimePresent = 0;
  z_Cell.z_Mask.OtaRadiusValid = 0;
  z_Cell.z_Mask.TzValid = 0;

  z_Cell.z_SSInfo.base_id = z_Cell.z_BsInfo.w_BaseId;
  z_Cell.z_SSInfo.changed_fields = z_Cell.z_BsInfo.w_Channel;
  z_Cell.z_SSInfo.sid = z_Cell.z_BsInfo.w_Sid;
  z_Cell.z_SSInfo.nid = z_Cell.z_BsInfo.w_Nid;
  z_Cell.z_SSInfo.srv_status = SYS_SRV_STATUS_SRV;
  z_Cell.z_SSInfo.ss_data_type = CGPS_SSINFO_SRV_SYS;
  z_Cell.z_SSInfo.sys_mode = SYS_SYS_MODE_CDMA;
  z_Cell.z_SSInfo.z_TimeZoneInfo.time_zone_avail = FALSE;
  z_Cell.u_SubscrId = 0;

  m_pzTleTask->m_HalCellMgr.PbCdmaSsInfoUpdate(z_Cell);
}

/*=============================================================================

  FUNCTION      tle_TaskTesterClass::InitiateLteCellChange()

  DESCRIPTION
    This method is used for generating an LTE cell change.
  

  DEPENDENCIES  

  PARAMETERS
 
  RETURN VALUE

  SIDE EFFECTS  

=============================================================================*/
void tle_TaskTesterClass::InitiateLteCellChange()
{
  tle_LteCellInfoType m_zLteCell;

  m_zLteCell.b_CellIdValid = TRUE;
  m_zLteCell.b_PreferCdma = TRUE;
  m_zLteCell.u_SubscrId = 0;
  m_zLteCell.z_Cell.e_ActiveRat = CELLDB_ACTIVERAT_LTE;
  m_zLteCell.z_Cell.z_Cell.z_Lte.z_Id.w_Mcc = 310;
  m_zLteCell.z_Cell.z_Cell.z_Lte.z_Id.w_Mnc = 410;
  m_zLteCell.z_Cell.z_Cell.z_Lte.z_Id.w_Tac = 35653;
  m_zLteCell.z_Cell.z_Cell.z_Lte.z_Id.q_CI = 169100304;

  m_zLteCell.z_Cell.z_Cell.z_Lte.w_PhysCellId = 271;
  m_zLteCell.z_Cell.z_Cell.z_Lte.z_FreqList.u_NumOfValidFreq = 1;
  m_zLteCell.z_Cell.z_Cell.z_Lte.z_FreqList.Arr_qFreq[0] = 5780;
  m_zLteCell.z_Cell.z_Cell.z_Lte.z_FreqList.w_DlFreq = 5780;

  m_pzTleTask->m_HalCellMgr.LteCellChange(m_zLteCell);
}


/*=============================================================================

  FUNCTION      PbTestPosConversion()

  DESCRIPTION
    This method is used for testing position conversion  
  

  DEPENDENCIES  

  PARAMETERS
 
  RETURN VALUE

  SIDE EFFECTS  

=============================================================================*/
void tle_TaskTesterClass::PbTestPosConversion()
{
  gtp_InputPosStructType z_Pos;
  tle_CellDbNameSpace::InputPosStructType z_DbPos;

  z_Pos.b_PropagatedPosition = true;
  z_Pos.b_SpeedAndHeadingValid = true;
  z_Pos.d_PosLLa[0] = 37.12 * C_PI / 180.0;
  z_Pos.d_PosLLa[1] = -121.3568 * C_PI / 180.0;
  z_Pos.d_PosLLa[2] = 25.8;
  z_Pos.e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_MEDIUM;
  z_Pos.e_VertRelIndicator = PDSM_POSITION_RELIABILITY_VERY_LOW;
  z_Pos.f_AltUnc68 = 658;
  z_Pos.f_HeadingRad = (FLT) 0.123;
  z_Pos.f_HeadingUnc99Rad = (FLT) .01;
  z_Pos.f_HorSpeedMps = (FLT) 5.4;
  z_Pos.f_HorSpeedUnc99Mps = (FLT) 1.2;
  z_Pos.f_PosUnc99 = 6500;
  z_Pos.q_GpsMsec = 15236;
  z_Pos.u_PosInfoValid = TRUE;
  z_Pos.w_GpsWeek = 1825;
  z_Pos.u_DeltaLeapSec = 17;

  for(uint16 w_i = 0; w_i < TECHNOLOGIES_MAX;++w_i) 
  {
    switch(w_i)
    {
/*    case TECHNOLOGIES_UNSPECIFIED_INTERNAL:
      break;

    case TECHNOLOGIES_UNSPECIFIED_EXTERNAL:
      break; */

    case TECHNOLOGIES_GPS:
      z_Pos.w_TechContributions[w_i] = 41019;
      break;

    case TECHNOLOGIES_GLONASS:
      z_Pos.w_TechContributions[w_i] = 11173;
      break;

    case TECHNOLOGIES_BEIDOU:
      z_Pos.w_TechContributions[w_i] = 1887;
      break;

    case TECHNOLOGIES_GALILEO:
      z_Pos.w_TechContributions[w_i] = 248;
      break;

    case TECHNOLOGIES_SENSORS_ON_DEVICE:
      z_Pos.w_TechContributions[w_i] = 1272;
      break;

    case TECHNOLOGIES_SENSORS_ON_VEHICLE:
      z_Pos.w_TechContributions[w_i] = 1048;
      break;

    case TECHNOLOGIES_WIFI_COVERAGE:
      z_Pos.w_TechContributions[w_i] = 1703;
      break;

    case TECHNOLOGIES_WWAN_COVERAGE:
      z_Pos.w_TechContributions[w_i] = 5242;
      break;

    case TECHNOLOGIES_WIFI_RANGING:
      z_Pos.w_TechContributions[w_i] = 305;
      break;

    case TECHNOLOGIES_WWAN_RANGING:
      z_Pos.w_TechContributions[w_i] = 1638;
      break;

    default:
      z_Pos.w_TechContributions[w_i] = 0;
    }
  }

  tle_CellDbNameSpace::ConvertPosInfo(z_Pos, z_DbPos);
}
#endif /* FEATURE_CGPS_PLAYBACK */



