/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               Iq Packet Parser

GENERAL DESCRIPTION
  IqPacketParser.c contains the main routine that implements the state machine
  for decoding IQ packet (0x0000) from a QXDM/Snapper .dlf file.
  Much of the code is leveraged from MMGPSDMSummary, for generating
  text files based on data in IQ log packets.



EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved. 
  Copyright (c) 2013 - 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss_tools/standalone_rfv/QXDM4/iq_test/iq_pkt_parser/IqPacketParser.c#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $


when       who     what, where, why
--------   ---     ----------------------------------------------------------

06/01/09    sp     Created

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* To disable "deprecation" warnings for library functions, use same function name succeeded by "_s".
 * Eg. Warning:- strcat': warning C4996: 'strcat': This function or variable may be unsafe.
 * Consider using strcat_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS.
 */

/* NOTE: The version of this file that is checked in is compliant with the banned API
   check, but will not compile with MSFT Visual Studio.  To recompile, search this file for
   BANNED_API_CHECK and enable the commented code */

#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "IqTypedef.h"
#include  <sys/stat.h>

#define LOG_CONVERGED_GPS_IQ_REPORT_C    0x138A

/* The DM log packet transfer 512 bytes at a time */
#define C_BYTES_PER_PKT       512

#define C_WORDS_PER_PKT       128

#define IQ_CONFIG_FLAG_HANNING_ENABLED        0x1
#define IQ_CONFIG_FLAG_NBIQ_CONTINUOUS_MODE   0x2
#define IQ_CONFIG_FLAG_IQ_IN_PAYLOAD            0
#define IQ_CONFIG_FLAG_FFT_IN_PAYLOAD         0x4

/** Config flags are defined in navrx_api.h. Copy of flags in 
navrx_api.h should be present here */
#define C_NAVRX_ADC_IQ_SWAP    (1 << 0)
#define C_NAVRX_ADC_6_TO_8_BIT (1 << 1)
#define C_NAVRX_BP_2_BIT_IQ_FORMAT (1 << 2)
#define C_NAVRX_GEN9_NBIQ (1 << 3)

enum
{
  IQ_MSG_ENUM_NOT_USED,
  IQ_MSG_IQ_SAMPLES,
  IQ_MSG_FFT,
  IQ_MSG_CONFIG,
  IQ_MSG_INTEGRATION_END,
  IQ_MSG_TEST_END,
  IQ_MSG_MAX
};

typedef enum
{
  IQ_FORMAT_WBIQ,
  IQ_FORMAT_NBIQ,
  IQ_FORMAT_MAX
} e_IqFormatType;

typedef enum
{
  COLLECT_MODE_WBIQ_BP1_BP2_2MHZ,
  COLLECT_MODE_WBIQ_BP1_2MHZ,
  COLLECT_MODE_WBIQ_BP2_2MHZ,
  COLLECT_MODE_WBIQ_BP3_1MHZ,
  COLLECT_MODE_FBIQ_BP4_20MHZ,
  COLLECT_MODE_NBIQ_BP1_AUTO_1KHZ,
  COLLECT_MODE_NBIQ_BP2_AUTO_1KHZ,
  COLLECT_MODE_NBIQ_BP3_AUTO_1KHZ,
  COLLECT_MODE_NBIQ_BP1_MANUAL_1KHZ,
  COLLECT_MODE_NBIQ_BP2_MANUAL_1KHZ,
  COLLECT_MODE_NBIQ_BP3_MANUAL_1KHZ,
  COLLECT_MODE_NBIQ_BP4_MANUAL_1KHZ,
  COLLECT_MODE_ADC1_16_POINT_8_MHZ,
  COLLECT_MODE_ADC2_16_POINT_8_MHZ,
  COLLECT_MODE_ADC1_ADC2_21_MHZ,
  COLLECT_MODE_FBIQ_BP3_10MHZ,
  COLLECT_MODE_ADC_IQ_81MHZ,
  COLLECT_MODE_WBIQ_BP2_4MHZ,
  COLLECT_MODE_WBIQ_BP1_4MHZ,
  COLLECT_MODE_MAX
} e_CollectModeType;

typedef struct
{
  boolean b_FirstIqConfigPktParsed;
  boolean b_FirstFftConfigPktParsed;
  boolean b_AllSamplesAccountedFor;
  boolean b_IqFilePtrCreated;
  boolean b_FftFilePtrCreated;
  boolean b_EndPktReceived;
  enum e_IqFormatType e_IqFormat;
  uint8 u_FileName;
  uint8 u_TotalExpectedIntegs;
  uint32 q_NumIqPktsExpected;
  uint32 q_NumFftPktsExpected;
  uint8 *p_IqData;
  uint8 *p_FftData;
  uint8 *p_TrackPktNum;
  uint8 u_CurrIntegNum;
  uint16 w_SampleCnt1k;
  FILE *p_DlfFile;
  FILE *p_IqBinFileWrPtr;
  FILE *p_FftBinFileWrPtr;
} iq_PpState;

#pragma pack(push, 1)

typedef struct        // This begins all MSG_FORMAT_DMSS
{
  uint16  length;
  uint16  log_code;
  uint64  h_TimeStamp;
  uint8  data[1];
} LOG_RECORD_HEADER;

#define LOG_RECORD_HEADER_LENGTH (sizeof(LOG_RECORD_HEADER)-1)

typedef struct
{
  uint8  u_I;
  uint8  u_Q;
} mc_DMCplx;

typedef struct
{
  uint8  u_Version;
  uint8  u_RespType;
  uint32 q_Total_Pkts;
  uint32 q_Pkt_Num;
} mc_IqTestDMCommonStruct;

typedef struct
{
  uint8  u_StatusFlag;
  uint32 q_PeakPower;
  int32  l_FftCenterFreqHz;
  uint8  u_CollectMode;
  int8   b_GloR1Freq;
  uint8  u_GloHWChan;
  uint16 w_SampleCnt1k;
  uint8  u_TotalIntegCnt;
  uint8  u_CurrIntegNum;
  int32  l_NBCenterFreqHz;
  int32  l_BpOffsetFreqHz;
  uint32 q_TotalRelativeGain;
  uint8  u_RfMode;
  uint32 q_AdcSampleFreqHz;
  uint32 q_SampleMemSize;
  int32  l_RefOscOffset;
  int32  l_IfDownConvFreq;
  int32  l_PGAGain;
  uint32 q_PGASetPointP1Mv;
  uint32 q_AdcVrefMv;
  uint8  u_AdcNumBits;
  uint8  u_ConfigFlags;
  uint32 q_Reserved;
} mc_IqTestDMConfigStructV54;

/* The mc_IqTestDMConfigStruct gets stuffed into an array of int32, 
   this specifies the size of the array */
#define C_IQ_TEST_DM_CONFIG_STRUCT_V54_SIZE_BYTES (23*4)

typedef struct
{
  mc_IqTestDMCommonStruct z_IqDM_Common;
  mc_IqTestDMConfigStructV54 z_IqDM_Config;
} mc_IqTestDMConfigPktV54;

typedef struct
{
  uint8  u_StatusFlag;
  uint32 q_PeakPower;
  int32  l_FftCenterFreqHz;
  uint8  u_CollectMode;
  int8   b_GloR1Freq;
  uint8  u_GloHWChan;
  uint16 w_SampleCnt1k;
  uint8  u_TotalIntegCnt;
  uint8  u_CurrIntegNum;
  int32  l_NBCenterFreqHz;
  int32  l_BpOffsetFreqHz;
  uint32 q_TotalRelativeGain;
  uint8  u_RfMode;
  uint32 q_AdcSampleFreqHz;
  uint32 q_SampleMemSize;
  int32  l_RefOscOffset;
  int32  l_IfDownConvFreq;
  int32  l_PGAGain;
  uint32 q_Reserved;
} mc_IqTestDMConfigStructV53;

/* The mc_IqTestDMConfigStruct gets stuffed into an array of int32, 
   this specifies the size of the array */
#define C_IQ_TEST_DM_CONFIG_STRUCT_V53_SIZE_BYTES (19*4)

typedef struct
{
  mc_IqTestDMCommonStruct z_IqDM_Common;
  mc_IqTestDMConfigStructV53 z_IqDM_Config;
} mc_IqTestDMConfigPktV53;

typedef struct
{
  uint8  u_StatusFlag;
  uint32 q_PeakPower;
  int32  l_FftCenterFreqHz;
  uint8  u_CollectMode;
  int8   b_GloR1Freq;
  uint8  u_GloHWChan;
  uint16 w_SampleCnt1k;
  uint8  u_TotalIntegCnt;
  uint8  u_CurrIntegNum;
  int32  l_NBCenterFreqHz;
  int32  l_BpOffsetFreqHz;
  uint32 q_TotalRelativeGain;
  uint8  u_RfMode;
  uint32 q_Reserved;
} mc_IqTestDMConfigStructV52;

/* The mc_IqTestDMConfigStruct gets stuffed into an array of int32, 
   this specifies the size of the array */
#define C_IQ_TEST_DM_CONFIG_STRUCT_V52_SIZE_BYTES (14*4)

typedef struct
{
  mc_IqTestDMCommonStruct z_IqDM_Common;
  mc_IqTestDMConfigStructV52 z_IqDM_Config;
} mc_IqTestDMConfigPktV52;

typedef struct
{
  mc_IqTestDMCommonStruct z_IqDM_Common;
  uint32     q_Samples[C_WORDS_PER_PKT];
} mc_IqTestDMWbIqPkt;

typedef struct
{
  mc_IqTestDMCommonStruct z_IqDM_Common;
  uint8     u_Samples[C_BYTES_PER_PKT];
} mc_IqTestDMNbIqPkt;

typedef struct
{
  mc_IqTestDMCommonStruct z_IqDM_Common;
  uint16         w_FftPower[256];
} mc_IqTestDMFftPkt;

#pragma pack(pop, 1)

/* For Gen8A Sample memory Size is 148KB; Gen8A lite Sample Mem Size = 60KB 
   This variable is needed for v52 of the 0x138A log packet only */
uint8 iqu_HalfSampleMemSize = 74;

/* Gen9 GLo WBIQ, BDS WBIQ, GAL WBIQ and NBIQ need special processing. */
boolean iqv_Gen9TwoBitWBIQ = FALSE;
boolean iqv_Gen9NBIQ = FALSE;

iq_PpState iqz_PpState;
unsigned char uInBuf[4096];
char c_ConfigBuf[1024*2];
FILE *hInFile;
char szInFileName[1024];
char szOutIqFileName[1024];
char szOutFftFileName[1024];
char c_RespTypes[6][16] = {"NOT_USED", "IQ_SAMPLES", "FFT", "CONFIG", "INTEGRATION_END", "TEST_END"};
uint32 iqq_BinFileBytesPerCollect = 0;

/*===========================================================================

FUNCTION
  CheckDroppedPkts

DESCRIPTION
  This function looks at the array containing TRUE/FALSE information for the
  receipt of each packet. FALSE value triggers a printf. Recovery of dropped
  pkts is part of iq_driver. This function is just a notification that a pkt
  was dropped at the first attempt.

PARAMETERS
  None

===========================================================================*/
void CheckDroppedPkts( void )
{
uint16 w_PktNum;

  for( w_PktNum = 1; w_PktNum <= iqz_PpState.q_NumIqPktsExpected; w_PktNum++ )
  {
    if( iqz_PpState.p_TrackPktNum[w_PktNum] == TRUE )
      continue;
    else
    {
       printf( "Warning: Pkt num %u at integ num %u was lost in transit at the first attempt\n",
                w_PktNum, iqz_PpState.u_CurrIntegNum );
    }
  }
}

/*===========================================================================

FUNCTION
  UnpackIqLog

DESCRIPTION
  This function processes GPS_IQ log records when there's IQ config data. Each IQ
  is a pair of real and img int8 and there are 128 IQ pairs in each log packet.
  An Iq txt file is created only when there's at least one IQ packet.

PARAMETERS
  p_IqDM_Pkt - pointer to IQ data in log packet.

===========================================================================*/
void WriteIqBinFile()
{
uint16 w_PktNum;
uint32 q_Temp;

  for( w_PktNum = 1; w_PktNum <= iqz_PpState.q_NumIqPktsExpected; w_PktNum++ )
  {
    if( iqz_PpState.p_TrackPktNum[w_PktNum] == TRUE )
      continue;
    else
    {
       printf( "ERROR: Pkt num %u at integ num %u not found in isf\n", w_PktNum,
                iqz_PpState.u_CurrIntegNum );
    }
  }
  q_Temp = fwrite(iqz_PpState.p_IqData, 1, iqq_BinFileBytesPerCollect,
                  iqz_PpState.p_IqBinFileWrPtr);


}


/*===========================================================================

FUNCTION
  UnpackIqLog

DESCRIPTION
  This function processes GPS_IQ log records when there's IQ config data. Each IQ
  is a pair of real and img int8 and there are 128 IQ pairs in each log packet.
  An Iq txt file is created only when there's at least one IQ packet.

PARAMETERS
  p_IqDM_Pkt - pointer to IQ data in log packet.

===========================================================================*/
void WriteFftBinFile( void )
{

  fwrite(iqz_PpState.p_FftData, 1, iqq_BinFileBytesPerCollect,
          iqz_PpState.p_FftBinFileWrPtr );
}

/*===========================================================================

FUNCTION
  UnpackFftPkt

DESCRIPTION
  This function calls the helper function UnpackIqLog to process GPS_IQ log
  records when there's IQ config data that has arrived in order. If a
  particular GPS_IQ log record arrives out of order (by checking its Packet
  Number), the log record is buffered until it is ready to be processed in
  it's rightful order.

PARAMETERS
  p_Data - pointer to IQ data in log packet.

===========================================================================*/
void UnpackFftPkt( uint8 *p_Data )
{
mc_IqTestDMFftPkt *p_FftPkt = (mc_IqTestDMFftPkt*) p_Data;
uint32 q_CurrPktNum = p_FftPkt->z_IqDM_Common.q_Pkt_Num;
uint16* p_Buf = (uint16 *) iqz_PpState.p_FftData;
uint16 w_IqIndex, w_FftPower;

  if( q_CurrPktNum > iqz_PpState.q_NumFftPktsExpected )
  {
    printf("\nPacket number greater than expected: %u", q_CurrPktNum );
    exit(1);
  }

  p_Buf += (C_BYTES_PER_PKT/sizeof(uint16)) * ( q_CurrPktNum - 1 ); // Pktnum is one based
  for( w_IqIndex = 0; w_IqIndex < ( C_BYTES_PER_PKT/sizeof(uint16) ); w_IqIndex ++ )
  {
    w_FftPower = p_FftPkt->w_FftPower[w_IqIndex];

    *p_Buf++ = w_FftPower;
  }
}

/*===========================================================================

FUNCTION
  UnpackIqPkt

DESCRIPTION
  This function unpacks the I and Q data from the packet. NB and WB are the 
  two possible formats. The data is written to a central buffer reserved for
  dumping the IQ samples. The struct tracking packet nums is updated with the
  info that this packet was received. This is useful at the end of every 
  integration to see if all packets were received.

PARAMETERS
  p_Data - pointer to IQ data in log packet.

===========================================================================*/
void UnpackIqPkt( uint8 *p_Data )
{
  mc_IqTestDMWbIqPkt *p_IqPkt = (mc_IqTestDMWbIqPkt *)p_Data;
  uint32 q_CurrPktNum = p_IqPkt->z_IqDM_Common.q_Pkt_Num;
  int8 *p_Buf = (int8 *)iqz_PpState.p_IqData;
  uint16 w_IqIndex;
  int8 b_I, b_Q;

  if ( q_CurrPktNum > iqz_PpState.q_NumIqPktsExpected )
  {
    printf("\nPacket number greater than expected: %u", q_CurrPktNum);
    exit(1);
  }

  iqz_PpState.p_TrackPktNum[q_CurrPktNum] = TRUE;

//  printf("pkt num = %u\n", q_CurrPktNum);

  if ( iqz_PpState.e_IqFormat == IQ_FORMAT_WBIQ )
  {
    /* Handle Gen9 Glo, BDS, GAL WBIQ differently as I/Q is 2 bit */
    if ( iqv_Gen9TwoBitWBIQ )
    {
      p_Buf += C_BYTES_PER_PKT * 4 * (q_CurrPktNum - 1); // Pktnum is one based
      for (w_IqIndex = 0; w_IqIndex < C_WORDS_PER_PKT; w_IqIndex++)
      {
        uint32 q_I, q_Sample, q_EightSamples;
        q_EightSamples = p_IqPkt->q_Samples[w_IqIndex];

        for (q_I = 0; q_I < 8; q_I++)
        {
          q_Sample = q_EightSamples >> (28 - q_I * 4);

          b_Q = (int8)(q_Sample & 0x3);
          if ( b_Q >= 2 )
          {
            b_Q = b_Q - 4;
          }

          q_Sample = q_Sample >> 2;
          b_I = (int8)(q_Sample & 0x3);
          if ( b_I >= 2 )
          {
            b_I = b_I - 4;
          }

          *p_Buf++ = b_I;
          *p_Buf++ = b_Q;
        }
      }
    }
    else
    {
      p_Buf += C_BYTES_PER_PKT * 2 * (q_CurrPktNum - 1); // Pktnum is one based
      for (w_IqIndex = 0; w_IqIndex < C_WORDS_PER_PKT; w_IqIndex++)
      {
        uint32 q_I, q_Sample, q_FourSamples;
        q_FourSamples = p_IqPkt->q_Samples[w_IqIndex];

        for (q_I = 0; q_I < 4; q_I++)
        {
          q_Sample = q_FourSamples >> (24 - q_I * 8);

          b_Q = (int8)(q_Sample & 0xf);
          b_Q = (b_Q << 4);
          b_Q = (b_Q >> 4);

          q_Sample = q_Sample >> 4;
          b_I = (int8)(q_Sample & 0xf);
          b_I = (b_I << 4);
          b_I = (b_I >> 4);

          *p_Buf++ = b_I;
          *p_Buf++ = b_Q;
        }
      }
    }
  }
  else if ( iqz_PpState.e_IqFormat == IQ_FORMAT_NBIQ )
  {
    mc_IqTestDMNbIqPkt *p_IqPkt = (mc_IqTestDMNbIqPkt *)p_Data;
    if ( iqv_Gen9NBIQ )
    {
      /*In gen9 we do not have 16 bit (1 I and Q) but 2 samples 
      (2 I and Q) per ms. Parser is ignoring duplicate IQ sample below. */
      p_Buf += (C_BYTES_PER_PKT/2) * (q_CurrPktNum - 1); // Pktnum is one based
      for (w_IqIndex = 0; w_IqIndex < (C_BYTES_PER_PKT - 1); w_IqIndex += 4)
      {
        b_I = (int8)p_IqPkt->u_Samples[w_IqIndex];
        b_Q = (int8)p_IqPkt->u_Samples[w_IqIndex + 1];

        *p_Buf++ = b_I;
        *p_Buf++ = b_Q;
      }
    }
    else
    {
      p_Buf += 512 * (q_CurrPktNum - 1); // Pktnum is one based
      for (w_IqIndex = 0; w_IqIndex < (C_BYTES_PER_PKT - 1); w_IqIndex += 2)
      {
        b_I = (int8)p_IqPkt->u_Samples[w_IqIndex];
        b_Q = (int8)p_IqPkt->u_Samples[w_IqIndex + 1];

        *p_Buf++ = b_I;
        *p_Buf++ = b_Q;
      }
    }
  }
}

/*===========================================================================

FUNCTION
  WriteConfigTextBufV54

DESCRIPTION
  This function simply writes summary configuration info to a write buffer.
  The write buffer normally would be routed to a text file.

PARAMETERS
  p_WrBuf - Pointer to write buffer
  p_Config - Contains the info to be printed

===========================================================================*/
void WriteConfigTextBufV54( char *p_WrBuf, mc_IqTestDMConfigStructV54* p_Config )
{
  int ret = 0, len = sizeof(c_ConfigBuf);

  ret = snprintf( p_WrBuf, len, "%-15u %% Status Flag\n", p_Config->u_StatusFlag );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15u %% Collect mode (0~4=>WBIQ (BP1&BP2, BP1, BP2, BP3, BP4)\n \
               %%              (5~7=>NBIQ Autocenter (BP1, BP2, BP3)\n \
               %%              (8-11=>NBIQ Manual (BP1, BP2, BP3, BP4)\n \
               %%              (12-14)=>ADC1 Only, ADC2 Only, ADC1 & ADC2 simultaneous\n",
                               p_Config->u_CollectMode );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15d %% GLO R1 Freq (k value)\n", p_Config->b_GloR1Freq );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15u %% GLO HW Chan (1 of 12)\n", p_Config->u_GloHWChan );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15u %% Number of 1k (1024) Samples \n", p_Config->w_SampleCnt1k );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15u %% Number of integrations\n\n", p_Config->u_TotalIntegCnt );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15ld %% NB center frequency\n", p_Config->l_NBCenterFreqHz );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15.2f %% Peak FFT Power [db]\n", p_Config->q_PeakPower * 0.002 );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15ld %% FFT Center Freq [Hz]\n", p_Config->l_FftCenterFreqHz );
  p_WrBuf += ret;
  len -= ret;

 //BANNED_API_CHECK
  //p_WrBuf += sprintf( p_WrBuf, "%-15u %% Status Flag\n", p_Config->u_StatusFlag );
  //p_WrBuf += sprintf( p_WrBuf, "%-15u %% Collect mode (0~4=>WBIQ (BP1&BP2, BP1, BP2, BP3, BP4)\n \
  //             %%              (5~7=>NBIQ Autocenter (BP1, BP2, BP3)\n \
  //             %%              (8-11=>NBIQ Manual (BP1, BP2, BP3, BP4)\n \
  //             %%              (12-14)=>ADC1 Only, ADC2 Only, ADC1 & ADC2 simultaneous\n",
  //                             p_Config->u_CollectMode );
  //p_WrBuf += sprintf( p_WrBuf, "%-15d %% GLO R1 Freq (k value)\n", p_Config->b_GloR1Freq );
  //p_WrBuf += sprintf( p_WrBuf, "%-15u %% GLO HW Chan (1 of 12)\n", p_Config->u_GloHWChan );
  //p_WrBuf += sprintf( p_WrBuf, "%-15u %% Number of 1k (1024) Samples \n", p_Config->w_SampleCnt1k );
  //p_WrBuf += sprintf( p_WrBuf, "%-15u %% Number of integrations\n\n", p_Config->u_TotalIntegCnt );
  //p_WrBuf += sprintf( p_WrBuf, "%-15ld %% NB center frequency\n", p_Config->l_NBCenterFreqHz );
  //p_WrBuf += sprintf( p_WrBuf, "%-15.2f %% Peak FFT Power [db]\n", p_Config->q_PeakPower * 0.002 );
  //p_WrBuf += sprintf( p_WrBuf, "%-15ld %% FFT Center Freq [Hz]\n", p_Config->l_FftCenterFreqHz );

}

/*===========================================================================

FUNCTION
  WriteConfigTextBufV53

DESCRIPTION
  This function simply writes summary configuration info to a write buffer.
  The write buffer normally would be routed to a text file.

PARAMETERS
  p_WrBuf - Pointer to write buffer
  p_Config - Contains the info to be printed

===========================================================================*/
void WriteConfigTextBufV53( char *p_WrBuf, mc_IqTestDMConfigStructV53* p_Config )
{
  int ret = 0, len = sizeof(c_ConfigBuf);

#if 0
  ret = snprintf( p_WrBuf, len, "%-15u %% Status Flag\n", p_Config->u_StatusFlag );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15u %% Collect mode (0~4=>WBIQ (BP1&BP2, BP1, BP2, BP3, BP4)\n \
               %%              (5~7=>NBIQ Autocenter (BP1, BP2, BP3)\n \
               %%              (8-11=>NBIQ Manual (BP1, BP2, BP3, BP4)\n \
               %%              (12-14)=>ADC1 Only, ADC2 Only, ADC1 & ADC2 simultaneous\n",
                               p_Config->u_CollectMode );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15d %% GLO R1 Freq (k value)\n", p_Config->b_GloR1Freq );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15u %% GLO HW Chan (1 of 12)\n", p_Config->u_GloHWChan );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15u %% Number of 1k (1024) Samples \n", p_Config->w_SampleCnt1k );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15u %% Number of integrations\n\n", p_Config->u_TotalIntegCnt );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15ld %% NB center frequency\n", p_Config->l_NBCenterFreqHz );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15.2f %% Peak FFT Power [db]\n", p_Config->q_PeakPower * 0.002 );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15ld %% FFT Center Freq [Hz]\n", p_Config->l_FftCenterFreqHz );
  p_WrBuf += ret;
  len -= ret;
#endif

 //BANNED_API_CHECK
  //p_WrBuf += sprintf( p_WrBuf, "%-15u %% Status Flag\n", p_Config->u_StatusFlag );
  //p_WrBuf += sprintf( p_WrBuf, "%-15u %% Collect mode (0~4=>WBIQ (BP1&BP2, BP1, BP2, BP3, BP4)\n \
  //             %%              (5~7=>NBIQ Autocenter (BP1, BP2, BP3)\n \
  //             %%              (8-11=>NBIQ Manual (BP1, BP2, BP3, BP4)\n \
  //             %%              (12-14)=>ADC1 Only, ADC2 Only, ADC1 & ADC2 simultaneous\n",
  //                             p_Config->u_CollectMode );
  //p_WrBuf += sprintf( p_WrBuf, "%-15d %% GLO R1 Freq (k value)\n", p_Config->b_GloR1Freq );
  //p_WrBuf += sprintf( p_WrBuf, "%-15u %% GLO HW Chan (1 of 12)\n", p_Config->u_GloHWChan );
  //p_WrBuf += sprintf( p_WrBuf, "%-15u %% Number of 1k (1024) Samples \n", p_Config->w_SampleCnt1k );
  //p_WrBuf += sprintf( p_WrBuf, "%-15u %% Number of integrations\n\n", p_Config->u_TotalIntegCnt );
  //p_WrBuf += sprintf( p_WrBuf, "%-15ld %% NB center frequency\n", p_Config->l_NBCenterFreqHz );
  //p_WrBuf += sprintf( p_WrBuf, "%-15.2f %% Peak FFT Power [db]\n", p_Config->q_PeakPower * 0.002 );
  //p_WrBuf += sprintf( p_WrBuf, "%-15ld %% FFT Center Freq [Hz]\n", p_Config->l_FftCenterFreqHz );

}

/*===========================================================================

FUNCTION
  WriteConfigTextBufV52

DESCRIPTION
  This function simply writes summary configuration info to a write buffer.
  The write buffer normally would be routed to a text file.

PARAMETERS
  p_WrBuf - Pointer to write buffer
  p_Config - Contains the info to be printed

===========================================================================*/
void WriteConfigTextBufV52( char *p_WrBuf, mc_IqTestDMConfigStructV52* p_Config )
{
  int ret = 0, len = sizeof(c_ConfigBuf);
#if 0
  ret = snprintf( p_WrBuf, len, "%-15u %% Status Flag\n", p_Config->u_StatusFlag );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15u %% Collect mode (0~4=>WBIQ (BP1&BP2, BP1, BP2, BP3, BP4)\n \
               %%              (5~7=>NBIQ Autocenter (BP1, BP2, BP3)\n \
               %%              (8-11=>NBIQ Manual (BP1, BP2, BP3, BP4)\n \
               %%              (12-14)=>ADC1 Only, ADC2 Only, ADC1 & ADC2 simultaneous\n",
                               p_Config->u_CollectMode );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15d %% GLO R1 Freq (k value)\n", p_Config->b_GloR1Freq );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15u %% GLO HW Chan (1 of 12)\n", p_Config->u_GloHWChan );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15u %% Number of 1k (1024) Samples \n", p_Config->w_SampleCnt1k );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15u %% Number of integrations\n\n", p_Config->u_TotalIntegCnt );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15ld %% NB center frequency\n", p_Config->l_NBCenterFreqHz );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15.2f %% Peak FFT Power [db]\n", p_Config->q_PeakPower * 0.002 );
  p_WrBuf += ret;
  len -= ret;
  ret = snprintf( p_WrBuf, len, "%-15ld %% FFT Center Freq [Hz]\n", p_Config->l_FftCenterFreqHz );
  p_WrBuf += ret;
  len -= ret;
#endif
 //BANNED_API_CHECK
  //p_WrBuf += sprintf( p_WrBuf, "%-15u %% Status Flag\n", p_Config->u_StatusFlag );
  //p_WrBuf += sprintf( p_WrBuf, "%-15u %% Collect mode (0~4=>WBIQ (BP1&BP2, BP1, BP2, BP3, BP4)\n \
  //             %%              (5~7=>NBIQ Autocenter (BP1, BP2, BP3)\n \
  //             %%              (8-11=>NBIQ Manual (BP1, BP2, BP3, BP4)\n \
  //             %%              (12-14)=>ADC1 Only, ADC2 Only, ADC1 & ADC2 simultaneous\n",
  //                             p_Config->u_CollectMode );
  //p_WrBuf += sprintf( p_WrBuf, "%-15d %% GLO R1 Freq (k value)\n", p_Config->b_GloR1Freq );
  //p_WrBuf += sprintf( p_WrBuf, "%-15u %% GLO HW Chan (1 of 12)\n", p_Config->u_GloHWChan );
  //p_WrBuf += sprintf( p_WrBuf, "%-15u %% Number of 1k (1024) Samples \n", p_Config->w_SampleCnt1k );
  //p_WrBuf += sprintf( p_WrBuf, "%-15u %% Number of integrations\n\n", p_Config->u_TotalIntegCnt );
  //p_WrBuf += sprintf( p_WrBuf, "%-15ld %% NB center frequency\n", p_Config->l_NBCenterFreqHz );
  //p_WrBuf += sprintf( p_WrBuf, "%-15.2f %% Peak FFT Power [db]\n", p_Config->q_PeakPower * 0.002 );
  //p_WrBuf += sprintf( p_WrBuf, "%-15ld %% FFT Center Freq [Hz]\n", p_Config->l_FftCenterFreqHz );

}

/*===========================================================================

FUNCTION
  WriteConfigBinV54

DESCRIPTION
  This function simply writes configuration parameters into a buffer that is 
  eventually destined to the binary file. This function was created only to
  make the code modular. Its not intended to be reused often.

PARAMETERS
  l_ConfigBuf - Pointer to write buffer
  p_Config - Source of info

===========================================================================*/
void WriteConfigBinV54( int32 *l_ConfigBuf, mc_IqTestDMConfigStructV54 *p_Config, mc_IqTestDMCommonStruct *p_Common )
{
  *l_ConfigBuf++ = (int32) p_Common->u_Version;
  *l_ConfigBuf++ = (int32) p_Config->u_StatusFlag;
  *l_ConfigBuf++ = (int32) p_Config->q_PeakPower;
  *l_ConfigBuf++ = p_Config->l_FftCenterFreqHz;
  *l_ConfigBuf++ = (int32) p_Config->u_CollectMode;
  *l_ConfigBuf++ = (int32) p_Config->b_GloR1Freq;
  *l_ConfigBuf++ = (int32) p_Config->u_GloHWChan;
  *l_ConfigBuf++ = (int32) p_Config->w_SampleCnt1k;
  *l_ConfigBuf++ = (int32) p_Config->u_TotalIntegCnt;
  *l_ConfigBuf++ = p_Config->l_NBCenterFreqHz;
  *l_ConfigBuf++ = p_Config->l_BpOffsetFreqHz;
  *l_ConfigBuf++ = (int32) p_Config->q_TotalRelativeGain;
  *l_ConfigBuf++ = (int32) p_Config->u_RfMode;
  *l_ConfigBuf++ = (int32) p_Config->q_AdcSampleFreqHz; 
  *l_ConfigBuf++ = (int32) p_Config->q_SampleMemSize; 
  *l_ConfigBuf++ = (int32) p_Config->l_RefOscOffset; 
  *l_ConfigBuf++ = (int32) p_Config->l_IfDownConvFreq; 
  *l_ConfigBuf++ = (int32) p_Config->l_PGAGain;  
  *l_ConfigBuf++ = (int32) p_Config->q_PGASetPointP1Mv; 
  *l_ConfigBuf++ = (int32) p_Config->q_AdcVrefMv; 
  *l_ConfigBuf++ = (int32) p_Config->u_AdcNumBits; 
  *l_ConfigBuf++ = (int32) p_Config->u_ConfigFlags; 
  *l_ConfigBuf++ = (int32) p_Config->q_Reserved;


}

/*===========================================================================

FUNCTION
  WriteConfigBinV53

DESCRIPTION
  This function simply writes configuration parameters into a buffer that is 
  eventually destined to the binary file. This function was created only to
  make the code modular. Its not intended to be reused often.

PARAMETERS
  l_ConfigBuf - Pointer to write buffer
  p_Config - Source of info

===========================================================================*/
void WriteConfigBinV53( int32 *l_ConfigBuf, mc_IqTestDMConfigStructV53 *p_Config, mc_IqTestDMCommonStruct *p_Common )
{
  *l_ConfigBuf++ = (int32) p_Common->u_Version;
  *l_ConfigBuf++ = (int32) p_Config->u_StatusFlag;
  *l_ConfigBuf++ = (int32) p_Config->q_PeakPower;
  *l_ConfigBuf++ = p_Config->l_FftCenterFreqHz;
  *l_ConfigBuf++ = (int32) p_Config->u_CollectMode;
  *l_ConfigBuf++ = (int32) p_Config->b_GloR1Freq;
  *l_ConfigBuf++ = (int32) p_Config->u_GloHWChan;
  *l_ConfigBuf++ = (int32) p_Config->w_SampleCnt1k;
  *l_ConfigBuf++ = (int32) p_Config->u_TotalIntegCnt;
  *l_ConfigBuf++ = p_Config->l_NBCenterFreqHz;
  *l_ConfigBuf++ = p_Config->l_BpOffsetFreqHz;
  *l_ConfigBuf++ = (int32) p_Config->q_TotalRelativeGain;
  *l_ConfigBuf++ = (int32) p_Config->u_RfMode;
  *l_ConfigBuf++ = (int32) p_Config->q_AdcSampleFreqHz; 
  *l_ConfigBuf++ = (int32) p_Config->q_SampleMemSize; 
  *l_ConfigBuf++ = (int32) p_Config->l_RefOscOffset; 
  *l_ConfigBuf++ = (int32) p_Config->l_IfDownConvFreq; 
  *l_ConfigBuf++ = (int32) p_Config->l_PGAGain;  
  *l_ConfigBuf++ = (int32) p_Config->q_Reserved;
}

/*===========================================================================

FUNCTION
  WriteConfigBinV52

DESCRIPTION
  This function simply writes configuration parameters into a buffer that is 
  eventually destined to the binary file. This function was created only to
  make the code modular. Its not intended to be reused often.

PARAMETERS
  l_ConfigBuf - Pointer to write buffer
  p_Config - Source of info

===========================================================================*/
void WriteConfigBinV52( int32 *l_ConfigBuf, mc_IqTestDMConfigStructV52 *p_Config, mc_IqTestDMCommonStruct *p_Common )
{
  *l_ConfigBuf++ = (int32) p_Common->u_Version;
  *l_ConfigBuf++ = (int32) p_Config->u_StatusFlag;
  *l_ConfigBuf++ = (int32) p_Config->q_PeakPower;
  *l_ConfigBuf++ = p_Config->l_FftCenterFreqHz;
  *l_ConfigBuf++ = (int32) p_Config->u_CollectMode;
  *l_ConfigBuf++ = (int32) p_Config->b_GloR1Freq;
  *l_ConfigBuf++ = (int32) p_Config->u_GloHWChan;
  *l_ConfigBuf++ = (int32) p_Config->w_SampleCnt1k;
  *l_ConfigBuf++ = (int32) p_Config->u_TotalIntegCnt;
  *l_ConfigBuf++ = p_Config->l_NBCenterFreqHz;
  *l_ConfigBuf++ = p_Config->l_BpOffsetFreqHz;
  *l_ConfigBuf++ = (int32) p_Config->q_TotalRelativeGain;
  *l_ConfigBuf++ = (int32) p_Config->u_RfMode;
  *l_ConfigBuf++ = (int32) p_Config->q_Reserved;
}

/*===========================================================================

FUNCTION
  WriteConfigTextFileV54

DESCRIPTION
  This function processes GPS_IQ log records when there's IQ config data. Config
  data is included at the top of each text file so that users can have a
  single file to know configuration of the collect.

PARAMETERS
  p_Data - pointer to IQ config in log packet.

===========================================================================*/
void WriteConfigTextFileV54( uint8 *p_Data )
{
  uint8 u_HanningUsed = 0;
  float f_BytesPerSample;
  int32 l_ConfigBuf[40];
  mc_IqTestDMConfigPktV54 *p_ConfigPkt = (mc_IqTestDMConfigPktV54 *)p_Data;
  mc_IqTestDMConfigStructV54 *p_ConfigStruct = &p_ConfigPkt->z_IqDM_Config;
  mc_IqTestDMCommonStruct *p_CommonStruct = &p_ConfigPkt->z_IqDM_Common;
  char *p_Buf = &c_ConfigBuf[0];
  FILE *p_ConfigFile;

  /* Glo, BDS and GAL WBIQ in Gen9 needs 2 bit IQ processing */
  if ( p_ConfigStruct->u_ConfigFlags & C_NAVRX_BP_2_BIT_IQ_FORMAT )
    {
      iqv_Gen9TwoBitWBIQ = TRUE;
    }
  /* NBIQ in Gen9 requires special processing - parser should dump duplicate samples */
  if ( p_ConfigStruct->u_ConfigFlags & C_NAVRX_GEN9_NBIQ )
  {
    iqv_Gen9NBIQ = TRUE;
  }

  if ( (p_ConfigStruct->u_StatusFlag & IQ_CONFIG_FLAG_FFT_IN_PAYLOAD) == IQ_CONFIG_FLAG_FFT_IN_PAYLOAD )
  {
    if ( (p_ConfigFile = fopen("fftsummary.txt", "w+")) == NULL )
    {
      printf("\n*****Error: Could not open fftsummary.txt");
      exit(1);
    }
    p_Buf += snprintf(p_Buf, sizeof(c_ConfigBuf), "%% File: fftsummary.txt\n");
    // BANNED_API_CHECK
    //p_Buf += sprintf( p_Buf, "%% File: fftsummary.txt\n" );
    WriteConfigTextBufV54(p_Buf, p_ConfigStruct);
    fprintf(p_ConfigFile, "%s", c_ConfigBuf);
    fclose(p_ConfigFile);

    if ( (iqz_PpState.p_FftBinFileWrPtr = fopen(szOutFftFileName, "wb")) == NULL )
    {
      printf("\n*****Error: Could not open %s\n", szOutFftFileName);
      exit(1);
    }
    iqz_PpState.b_FftFilePtrCreated = TRUE;

    WriteConfigBinV54(l_ConfigBuf, p_ConfigStruct, p_CommonStruct);
    fwrite(&l_ConfigBuf[0], 1, C_IQ_TEST_DM_CONFIG_STRUCT_V54_SIZE_BYTES,  iqz_PpState.p_FftBinFileWrPtr);

    /** Irrespective of collect mode, on board FFT is possible
     *  only if 1K samples is less than or equal to 32K. This
     *  check is redundant here as modem code already ensures
     *  that on board FFT is done for max 32Ksamples */
    if(p_ConfigStruct->w_SampleCnt1k <= 32)
    {
    iqz_PpState.w_SampleCnt1k = p_ConfigStruct->w_SampleCnt1k;
    iqz_PpState.q_NumFftPktsExpected = ((iqz_PpState.w_SampleCnt1k * 2 * 1024) / 512);
    /* 2 bytes per power sample */
      iqq_BinFileBytesPerCollect = iqz_PpState.w_SampleCnt1k * 2 * 1024;

      iqz_PpState.p_FftData = (uint8 *)malloc(iqq_BinFileBytesPerCollect);
    if ( iqz_PpState.p_FftData == NULL )
    {
      printf("\n*****Error: Malloc failed. No memory to collect data!\n");
      exit(1);
    }

    }
    else
    {
      printf("\n*****Error: SampleCnt1k %d is greater than 32K!\n", p_ConfigStruct->w_SampleCnt1k);
    }
    return;
  }

  if ( (p_ConfigStruct->u_StatusFlag & IQ_CONFIG_FLAG_IQ_IN_PAYLOAD) == IQ_CONFIG_FLAG_IQ_IN_PAYLOAD )
  {
    if ( iqz_PpState.b_FirstIqConfigPktParsed == FALSE )
    {
      c_ConfigBuf[0] = '\0';

      if ( (p_ConfigFile = fopen("iqsummary.txt", "w+")) == NULL )
      {
        printf("\n*****Error: Could not open iqsummary.txt");
        exit(1);
      }

      p_Buf += snprintf(p_Buf, sizeof(c_ConfigBuf), "%% File: iqsummary.txt\n");
      // BANNED_API_CHECK
      //p_Buf += sprintf(p_Buf, "%% File: iqsummary.txt\n");

      WriteConfigTextBufV54(p_Buf, p_ConfigStruct);
      fprintf(p_ConfigFile, "%s", c_ConfigBuf);
      fclose(p_ConfigFile);

      if ( (iqz_PpState.p_IqBinFileWrPtr = fopen(szOutIqFileName, "wb")) == NULL )
      {
        printf("\n*****Error: Could not open %s\n", szOutIqFileName);
        exit(1);
      }

      iqz_PpState.b_IqFilePtrCreated = TRUE;
      WriteConfigBinV54(l_ConfigBuf, p_ConfigStruct, p_CommonStruct);
      fwrite(&l_ConfigBuf[0], 1, C_IQ_TEST_DM_CONFIG_STRUCT_V54_SIZE_BYTES,  iqz_PpState.p_IqBinFileWrPtr);
      iqz_PpState.b_FirstIqConfigPktParsed = TRUE;

      iqz_PpState.u_TotalExpectedIntegs = p_ConfigStruct->u_TotalIntegCnt;

      switch (p_ConfigStruct->u_CollectMode)
      {
      case COLLECT_MODE_WBIQ_BP1_BP2_2MHZ:
      case COLLECT_MODE_WBIQ_BP1_2MHZ:
      case COLLECT_MODE_WBIQ_BP2_2MHZ:
      case COLLECT_MODE_WBIQ_BP3_1MHZ:
      case COLLECT_MODE_FBIQ_BP4_20MHZ:
      case COLLECT_MODE_WBIQ_BP2_4MHZ:
      case COLLECT_MODE_WBIQ_BP1_4MHZ:
        {
          /** 1 sample is pair of I and Q. f_BytesPerSample indicates
           *  how many bytes are required to represent one I and Q
           *  data. If SM holds 2 bit I and 2 bit Q data then it means
           *  that we require half byte to represent 1 sample. If
           *  sample memory holds 4 bit I and 4 bit Q data then it means
           *  that we will need 1 byte to represent 1 IQ pair.
           *  */
          if ( iqv_Gen9TwoBitWBIQ == TRUE )
          {
            f_BytesPerSample = 0.5;
          }
          else
          {
            f_BytesPerSample = 1.0;
          }
          iqz_PpState.e_IqFormat = IQ_FORMAT_WBIQ;
          break;
        }
      case COLLECT_MODE_NBIQ_BP1_AUTO_1KHZ:
      case COLLECT_MODE_NBIQ_BP2_AUTO_1KHZ:
      case COLLECT_MODE_NBIQ_BP3_AUTO_1KHZ:
      case COLLECT_MODE_NBIQ_BP1_MANUAL_1KHZ:
      case COLLECT_MODE_NBIQ_BP2_MANUAL_1KHZ:
      case COLLECT_MODE_NBIQ_BP3_MANUAL_1KHZ:
      case COLLECT_MODE_NBIQ_BP4_MANUAL_1KHZ:
      case COLLECT_MODE_FBIQ_BP3_10MHZ:
      case COLLECT_MODE_ADC_IQ_81MHZ:
        {
          /** Gen9 NBIQ does not have 1 byte I and 1 byte Q but 2I and 2Q
           *  per ms. Duplicate I and Q will be dropped when we unpackt IQ
           *  packet. 
           */
          if ( iqv_Gen9NBIQ == TRUE )
          {
            f_BytesPerSample = 4.0;
          }
          else
          {
            f_BytesPerSample = 2.0;
          }
          iqz_PpState.e_IqFormat = IQ_FORMAT_NBIQ;
          break;
        }

      default:
        {
          printf("*****Error: Unknown num of \"bytes per sample\" for specified collect mode %u\n", p_ConfigStruct->u_CollectMode);
          exit(1);
        }
      }
      if ( p_ConfigStruct->u_CollectMode == COLLECT_MODE_ADC_IQ_81MHZ )
      {
        /** w_SampleCnt1k for ADC IQ is not used in Matlab script.
         *  This variable will have truncation issues if size
         *  of sample memory is *odd* KB. So pass on the value we
         *  obtain from ConfigStruct as is */
        iqz_PpState.w_SampleCnt1k = p_ConfigStruct->w_SampleCnt1k;
        iqz_PpState.q_NumIqPktsExpected = (p_ConfigStruct->q_SampleMemSize / 512);
        iqq_BinFileBytesPerCollect = p_ConfigStruct->q_SampleMemSize;
      }
      else
      {
        iqz_PpState.w_SampleCnt1k = p_ConfigStruct->w_SampleCnt1k;
        iqz_PpState.q_NumIqPktsExpected = ((iqz_PpState.w_SampleCnt1k * f_BytesPerSample * 1024) / 512);
        iqq_BinFileBytesPerCollect = iqz_PpState.w_SampleCnt1k * 1024 * 2;
      }
      printf( "\nNumber of Pkts Expected %d\n", iqz_PpState.q_NumIqPktsExpected);
      iqz_PpState.p_IqData = (uint8 *)malloc(iqq_BinFileBytesPerCollect);

      if ( iqz_PpState.p_IqData == NULL )
      {
        printf("\n*****Error: Malloc failed. No memory to collect data!\n");
        exit(1);
      }

      //    iqz_PpState.p_IqData = &iqu_Buf[0];

      iqz_PpState.p_TrackPktNum = (uint8 *)malloc(iqz_PpState.q_NumIqPktsExpected + 1);
      if ( iqz_PpState.p_TrackPktNum == NULL )
      {
        printf("\n*****Error: Malloc failed. No memory to track dropped packets!\n");
        exit(1);
      }
    }
    else
    {
      WriteIqBinFile();
    }
  }

  memset(iqz_PpState.p_TrackPktNum, 0, (iqz_PpState.q_NumIqPktsExpected + 1));
  memset( iqz_PpState.p_IqData, 0, iqq_BinFileBytesPerCollect);

  iqz_PpState.u_CurrIntegNum = p_ConfigStruct->u_CurrIntegNum;
}

/*===========================================================================

FUNCTION
  WriteConfigTextFileV53

DESCRIPTION
  This function processes GPS_IQ log records when there's IQ config data. Config
  data is included at the top of each text file so that users can have a
  single file to know configuration of the collect.

PARAMETERS
  p_Data - pointer to IQ config in log packet.

===========================================================================*/
void WriteConfigTextFileV53( uint8 *p_Data )
{
uint8 u_HanningUsed = 0;
uint8 u_BytesPerSample;
int32 l_ConfigBuf[40];
mc_IqTestDMConfigPktV53 *p_ConfigPkt = (mc_IqTestDMConfigPktV53*) p_Data;
mc_IqTestDMConfigStructV53 *p_ConfigStruct = &p_ConfigPkt->z_IqDM_Config;
mc_IqTestDMCommonStruct *p_CommonStruct = &p_ConfigPkt->z_IqDM_Common;
char *p_Buf = &c_ConfigBuf[0];
FILE *p_ConfigFile;

  if( ( p_ConfigStruct->u_StatusFlag & IQ_CONFIG_FLAG_FFT_IN_PAYLOAD ) == IQ_CONFIG_FLAG_FFT_IN_PAYLOAD)
  {
    if( ( p_ConfigFile = fopen ( "fftsummary.txt", "w+" ) ) == NULL )
    {
      printf( "\n*****Error: Could not open fftsummary.txt" );
      exit(1);
    }
    p_Buf += snprintf( p_Buf, sizeof(c_ConfigBuf), "%% File: fftsummary.txt\n" );
    // BANNED_API_CHECK
    //p_Buf += sprintf( p_Buf, "%% File: fftsummary.txt\n" );
    WriteConfigTextBufV53( p_Buf, p_ConfigStruct );
    fprintf( p_ConfigFile, "%s", c_ConfigBuf );
    fclose( p_ConfigFile );

    if( ( iqz_PpState.p_FftBinFileWrPtr = fopen ( szOutFftFileName, "wb" ) ) == NULL )
    {
      printf( "\n*****Error: Could not open %s\n", szOutFftFileName );
      exit(1);
    }
    iqz_PpState.b_FftFilePtrCreated = TRUE;

    WriteConfigBinV53( l_ConfigBuf, p_ConfigStruct, p_CommonStruct );
    fwrite( &l_ConfigBuf[0], 1, C_IQ_TEST_DM_CONFIG_STRUCT_V53_SIZE_BYTES,  iqz_PpState.p_FftBinFileWrPtr );


    /** Irrespective of collect mode, on board FFT is possible
     *  only if 1K samples is less than or equal to 32K. This
     *  check is redundant here as modem code already ensures
     *  that on board FFT is done for max 32Ksamples */
    if ( p_ConfigStruct->w_SampleCnt1k <= 32 )
    {
    iqz_PpState.w_SampleCnt1k = p_ConfigStruct->w_SampleCnt1k;
    iqz_PpState.q_NumFftPktsExpected = ( ( iqz_PpState.w_SampleCnt1k * 2 * 1024 ) / 512 );
     /* 2 bytes per power sample */
      iqq_BinFileBytesPerCollect = iqz_PpState.w_SampleCnt1k * 2 * 1024;

      iqz_PpState.p_FftData = (uint8 *)malloc(iqq_BinFileBytesPerCollect);
    if( iqz_PpState.p_FftData == NULL )
    {
      printf( "\n*****Error: Malloc failed. No memory to collect data!\n" );
      exit(1);
    }

    }
    else
    {
      printf("\n*****Error: SampleCnt1k %d is greater than 32K!\n", p_ConfigStruct->w_SampleCnt1k);
    }
    return;
  }

  if( ( p_ConfigStruct->u_StatusFlag & IQ_CONFIG_FLAG_IQ_IN_PAYLOAD ) == IQ_CONFIG_FLAG_IQ_IN_PAYLOAD )
  {
    if( iqz_PpState.b_FirstIqConfigPktParsed == FALSE )
    {
      c_ConfigBuf[0] = '\0';

      if( ( p_ConfigFile = fopen ( "iqsummary.txt", "w+" ) ) == NULL )
      {
        printf( "\n*****Error: Could not open iqsummary.txt" );
        exit(1);
      }

    p_Buf += snprintf( p_Buf, sizeof(c_ConfigBuf), "%% File: iqsummary.txt\n" );
    // BANNED_API_CHECK
    //p_Buf += sprintf( p_Buf, "%% File: iqsummary.txt\n" );
      WriteConfigTextBufV53( p_Buf, p_ConfigStruct );
      fprintf( p_ConfigFile, "%s", c_ConfigBuf );
      fclose( p_ConfigFile );

      if( ( iqz_PpState.p_IqBinFileWrPtr = fopen ( szOutIqFileName, "wb" ) ) == NULL )
      {
        printf( "\n*****Error: Could not open %s\n", szOutIqFileName );
        exit(1);
      }

      iqz_PpState.b_IqFilePtrCreated = TRUE;
      WriteConfigBinV53( l_ConfigBuf, p_ConfigStruct, p_CommonStruct );
      fwrite( &l_ConfigBuf[0], 1, C_IQ_TEST_DM_CONFIG_STRUCT_V53_SIZE_BYTES,  iqz_PpState.p_IqBinFileWrPtr );
      iqz_PpState.b_FirstIqConfigPktParsed = TRUE;

      iqz_PpState.u_TotalExpectedIntegs = p_ConfigStruct->u_TotalIntegCnt;

      switch( p_ConfigStruct->u_CollectMode )
      {
        case COLLECT_MODE_WBIQ_BP1_BP2_2MHZ:
        case COLLECT_MODE_WBIQ_BP1_2MHZ:
        case COLLECT_MODE_WBIQ_BP2_2MHZ:
        case COLLECT_MODE_WBIQ_BP3_1MHZ:
        case COLLECT_MODE_FBIQ_BP4_20MHZ:
        case COLLECT_MODE_WBIQ_BP2_4MHZ:
        {
          u_BytesPerSample = 1;
          iqz_PpState.e_IqFormat = IQ_FORMAT_WBIQ;
          break;
        }
        case COLLECT_MODE_NBIQ_BP1_AUTO_1KHZ:
        case COLLECT_MODE_NBIQ_BP2_AUTO_1KHZ:
        case COLLECT_MODE_NBIQ_BP3_AUTO_1KHZ:
        case COLLECT_MODE_NBIQ_BP1_MANUAL_1KHZ:
        case COLLECT_MODE_NBIQ_BP2_MANUAL_1KHZ:
        case COLLECT_MODE_NBIQ_BP3_MANUAL_1KHZ:
        case COLLECT_MODE_NBIQ_BP4_MANUAL_1KHZ:
        case COLLECT_MODE_FBIQ_BP3_10MHZ:
        case COLLECT_MODE_ADC_IQ_81MHZ:		
        {
          u_BytesPerSample = 2;
          iqz_PpState.e_IqFormat = IQ_FORMAT_NBIQ;
          break;
        }
        default:
        {
          printf( "*****Error: Unknown num of \"bytes per sample\" for specified collect mode %u\n", p_ConfigStruct->u_CollectMode );
          exit(1);
        }
      }
      if ( p_ConfigStruct->u_CollectMode == COLLECT_MODE_ADC_IQ_81MHZ )
      {
        /** w_SampleCnt1k for ADC IQ is not used in Matlab script.
         *  This variable will have truncation issues if size
         *  of sample memory is *odd* KB. So pass on the value we
         *  obtain from ConfigStruct as is */
        iqz_PpState.w_SampleCnt1k = p_ConfigStruct->w_SampleCnt1k;
        iqz_PpState.q_NumIqPktsExpected = (p_ConfigStruct->q_SampleMemSize / 512);
        iqq_BinFileBytesPerCollect = p_ConfigStruct->q_SampleMemSize;
      }
      else
      {
        iqz_PpState.w_SampleCnt1k = p_ConfigStruct->w_SampleCnt1k;
        iqz_PpState.q_NumIqPktsExpected = ((iqz_PpState.w_SampleCnt1k * u_BytesPerSample * 1024) / 512);
        iqq_BinFileBytesPerCollect = iqz_PpState.w_SampleCnt1k * 1024 * 2;
      }
      iqz_PpState.p_IqData = (uint8 *)malloc(iqq_BinFileBytesPerCollect);
      if( iqz_PpState.p_IqData == NULL )
      {
        printf( "\n*****Error: Malloc failed. No memory to collect data!\n" );
        exit(1);
      }


      iqz_PpState.p_TrackPktNum = (uint8 *) malloc( iqz_PpState.q_NumIqPktsExpected + 1 );
      if( iqz_PpState.p_TrackPktNum == NULL )
      {
        printf( "\n*****Error: Malloc failed. No memory to track dropped packets!\n" );
        exit(1);
      }
    }
    else
    {
      WriteIqBinFile();
    }
  }

  memset( iqz_PpState.p_TrackPktNum, 0, ( iqz_PpState.q_NumIqPktsExpected + 1 ) );
  memset(iqz_PpState.p_IqData, 0, iqq_BinFileBytesPerCollect);

  iqz_PpState.u_CurrIntegNum = p_ConfigStruct->u_CurrIntegNum;
}
/*===========================================================================

FUNCTION
  WriteConfigTextFileV52

DESCRIPTION
  This function processes GPS_IQ log records when there's IQ config data. Config
  data is included at the top of each text file so that users can have a
  single file to know configuration of the collect.

PARAMETERS
  p_Data - pointer to IQ config in log packet.

===========================================================================*/
void WriteConfigTextFileV52( uint8 *p_Data )
{
uint8 u_HanningUsed = 0;
uint8 u_BytesPerSample;
int32 l_ConfigBuf[40];
mc_IqTestDMConfigPktV52 *p_ConfigPkt = (mc_IqTestDMConfigPktV52*) p_Data;
mc_IqTestDMConfigStructV52 *p_ConfigStruct = &p_ConfigPkt->z_IqDM_Config;
mc_IqTestDMCommonStruct *p_CommonStruct = &p_ConfigPkt->z_IqDM_Common;
char *p_Buf = &c_ConfigBuf[0];
FILE *p_ConfigFile;

  if( ( p_ConfigStruct->u_StatusFlag & IQ_CONFIG_FLAG_FFT_IN_PAYLOAD ) == IQ_CONFIG_FLAG_FFT_IN_PAYLOAD)
  {
    if( ( p_ConfigFile = fopen ( "fftsummary.txt", "w+" ) ) == NULL )
    {
      printf( "\n*****Error: Could not open fftsummary.txt" );
      exit(1);
    }
    p_Buf += snprintf( p_Buf, sizeof(c_ConfigBuf), "%% File: fftsummary.txt\n" );

    // BANNED_API_CHECK
    //p_Buf += sprintf( p_Buf, "%% File: fftsummary.txt\n" );

    WriteConfigTextBufV52( p_Buf, p_ConfigStruct );
    fprintf( p_ConfigFile, "%s", c_ConfigBuf );
    fclose( p_ConfigFile );

    if( ( iqz_PpState.p_FftBinFileWrPtr = fopen ( szOutFftFileName, "wb" ) ) == NULL )
    {
      printf( "\n*****Error: Could not open %s\n", szOutFftFileName );
      exit(1);
    }
    iqz_PpState.b_FftFilePtrCreated = TRUE;

    WriteConfigBinV52( l_ConfigBuf, p_ConfigStruct, p_CommonStruct );
    fwrite( &l_ConfigBuf[0], 1, C_IQ_TEST_DM_CONFIG_STRUCT_V52_SIZE_BYTES,  iqz_PpState.p_FftBinFileWrPtr );

    iqz_PpState.w_SampleCnt1k = p_ConfigStruct->w_SampleCnt1k;
    iqz_PpState.q_NumFftPktsExpected = ( ( iqz_PpState.w_SampleCnt1k * 2 * 1024 ) / 512 );
     /* 2 bytes per power sample */
    iqz_PpState.p_FftData = (uint8 *) malloc( iqz_PpState.w_SampleCnt1k * 1024 * 2 );
    if( iqz_PpState.p_FftData == NULL )
    {
      printf( "\n*****Error: Malloc failed. No memory to collect data!\n" );
      exit(1);
    }
    return;
  }

  if( ( p_ConfigStruct->u_StatusFlag & IQ_CONFIG_FLAG_IQ_IN_PAYLOAD ) == IQ_CONFIG_FLAG_IQ_IN_PAYLOAD )
  {
    if( iqz_PpState.b_FirstIqConfigPktParsed == FALSE )
    {
      c_ConfigBuf[0] = '\0';

      if( ( p_ConfigFile = fopen ( "iqsummary.txt", "w+" ) ) == NULL )
      {
        printf( "\n*****Error: Could not open iqsummary.txt" );
        exit(1);
      }


    p_Buf += snprintf( p_Buf, sizeof(c_ConfigBuf), "%% File: iqsummary.txt\n" );

    // BANNED_API_CHECK
    //p_Buf += sprintf( p_Buf, "%% File: iqsummary.txt\n" );

      WriteConfigTextBufV52( p_Buf, p_ConfigStruct );
      fprintf( p_ConfigFile, "%s", c_ConfigBuf );
      fclose( p_ConfigFile );

      if( ( iqz_PpState.p_IqBinFileWrPtr = fopen ( szOutIqFileName, "wb" ) ) == NULL )
      {
        printf( "\n*****Error: Could not open %s\n", szOutIqFileName );
        exit(1);
      }

      iqz_PpState.b_IqFilePtrCreated = TRUE;
      WriteConfigBinV52( l_ConfigBuf, p_ConfigStruct, p_CommonStruct );
      fwrite( &l_ConfigBuf[0], 1, C_IQ_TEST_DM_CONFIG_STRUCT_V52_SIZE_BYTES,  iqz_PpState.p_IqBinFileWrPtr );
      iqz_PpState.b_FirstIqConfigPktParsed = TRUE;

      iqz_PpState.u_TotalExpectedIntegs = p_ConfigStruct->u_TotalIntegCnt;
      switch( p_ConfigStruct->u_CollectMode )
      {
        case COLLECT_MODE_WBIQ_BP1_BP2_2MHZ:
        case COLLECT_MODE_WBIQ_BP1_2MHZ:
        case COLLECT_MODE_WBIQ_BP2_2MHZ:
        case COLLECT_MODE_WBIQ_BP3_1MHZ:
        case COLLECT_MODE_FBIQ_BP4_20MHZ:
        case COLLECT_MODE_WBIQ_BP2_4MHZ:
        {
          u_BytesPerSample = 1;
          iqz_PpState.e_IqFormat = IQ_FORMAT_WBIQ;
          break;
        }
        case COLLECT_MODE_NBIQ_BP1_AUTO_1KHZ:
        case COLLECT_MODE_NBIQ_BP2_AUTO_1KHZ:
        case COLLECT_MODE_NBIQ_BP3_AUTO_1KHZ:
        case COLLECT_MODE_NBIQ_BP1_MANUAL_1KHZ:
        case COLLECT_MODE_NBIQ_BP2_MANUAL_1KHZ:
        case COLLECT_MODE_NBIQ_BP3_MANUAL_1KHZ:
        case COLLECT_MODE_NBIQ_BP4_MANUAL_1KHZ:
        case COLLECT_MODE_FBIQ_BP3_10MHZ:
        case COLLECT_MODE_ADC_IQ_81MHZ:		
        {
          u_BytesPerSample = 2;
          iqz_PpState.e_IqFormat = IQ_FORMAT_NBIQ;
          break;
        }
        default:
        {
          printf( "*****Error: Unknown num of \"bytes per sample\" for specified collect mode %u\n", p_ConfigStruct->u_CollectMode );
          exit(1);
        }
      }
      if ( p_ConfigStruct->u_CollectMode == COLLECT_MODE_ADC_IQ_81MHZ )
      {
        iqz_PpState.w_SampleCnt1k = (uint16)(iqu_HalfSampleMemSize); 
      }
      else
      {
        iqz_PpState.w_SampleCnt1k = p_ConfigStruct->w_SampleCnt1k;
      }


      iqz_PpState.q_NumIqPktsExpected = ((iqz_PpState.w_SampleCnt1k * u_BytesPerSample * 1024) / 512);
      iqq_BinFileBytesPerCollect = iqz_PpState.w_SampleCnt1k * 1024 * 2;


      /* 2 bytes per sample (common bin file format) */
      iqz_PpState.p_IqData = (uint8 *)malloc(iqq_BinFileBytesPerCollect);
      if( iqz_PpState.p_IqData == NULL )
      {
        printf( "\n*****Error: Malloc failed. No memory to collect data!\n" );
        exit(1);
      }

      iqz_PpState.p_TrackPktNum = (uint8 *) malloc( iqz_PpState.q_NumIqPktsExpected + 1 );
      if( iqz_PpState.p_TrackPktNum == NULL )
      {
        printf( "\n*****Error: Malloc failed. No memory to track dropped packets!\n" );
        exit(1);
      }
    }
    else
    {
      WriteIqBinFile();
    }
  }

  memset( iqz_PpState.p_TrackPktNum, 0, ( iqz_PpState.q_NumIqPktsExpected + 1 ) );
  memset(iqz_PpState.p_IqData, 0, iqq_BinFileBytesPerCollect);

  iqz_PpState.u_CurrIntegNum = p_ConfigStruct->u_CurrIntegNum;
}

/*===========================================================================

FUNCTION
  ProcessGPSIqLog

DESCRIPTION
  This function processes GPS_IQ log records. Depending on the response type, the
  data in the log record is different. Hence, different functions are called to parse
  remainder of the packet.

PARAMETERS
  pLogRecordHeader - pointer to generic log record header

===========================================================================*/
void ProcessGPSIqLog( LOG_RECORD_HEADER *pLogRecordHeader )
{
  mc_IqTestDMCommonStruct *p_Log = (mc_IqTestDMCommonStruct *)&pLogRecordHeader->data;
  uint8 *p_Data = (uint8 *)&pLogRecordHeader->data;

  if ( iqz_PpState.b_EndPktReceived )
  {
    /* There have been occassional cases of an extra TEST_END pkt
    * showing up about 4secs after the correct TEST_END pkt. While dev
    * figures out why the phone is sending a duplicate, this 
    * check will allow users to continue using the tool. There is 
    * no impact to functionality.
    */
    printf("\nWarning: Unexpected IQ pkt received after TEST_END pkt. Resp Type = %s", c_RespTypes[p_Log->u_RespType]);
    return;
  }

  switch (p_Log->u_RespType)
  {
  case IQ_MSG_CONFIG:
    {
      switch (p_Log->u_Version)
      {
      case 53:
        {
          WriteConfigTextFileV53(p_Data);
          break;
        }
      case 54:
        {
          WriteConfigTextFileV54(p_Data);
          break;
        }
      default:
        {
          printf("\n*****Warning: I/Q log version %d: parser supports -full only, no -lite support\n", p_Log->u_Version);
          WriteConfigTextFileV52(p_Data);
          break;
        }
      }
      break;
    }

  case IQ_MSG_IQ_SAMPLES:
    {
      UnpackIqPkt(p_Data);
      break;
    }

  case IQ_MSG_TEST_END:
    {
      iqz_PpState.b_EndPktReceived = TRUE;
      if ( iqz_PpState.b_IqFilePtrCreated )
      {
        WriteIqBinFile();
        fclose(iqz_PpState.p_IqBinFileWrPtr);
        printf("Created IQ file:\n%s\n", szOutIqFileName);
      }
      else
      {
        printf("No IQ File Created since no IQ data was found\n");
      }

      if ( iqz_PpState.b_FftFilePtrCreated )
      {
        WriteFftBinFile();
        fclose(iqz_PpState.p_FftBinFileWrPtr);
        printf("Created FFT file:\n%s\n", szOutFftFileName);
      }
      else
      {
        printf("No FFT File created since no FFT data was found\n");
      }
      break;
    }
  case IQ_MSG_INTEGRATION_END:
    {
      CheckDroppedPkts();
      break;
    }
  case IQ_MSG_FFT:
    {
      UnpackFftPkt(p_Data);
      break;
    }

  default:
    break;
  }
}

/*===========================================================================

FUNCTION
  FormatAndOutputRecord

DESCRIPTION
  This function processes GPS_IQ logs from the input file.
  Other log messages are ignored.

PARAMETERS
  None

===========================================================================*/
void FormatAndOutputRecord( void )
{
  LOG_RECORD_HEADER* pLogRecordHeader = (LOG_RECORD_HEADER*)uInBuf;

  switch ( pLogRecordHeader->log_code )
  {
   case LOG_CONVERGED_GPS_IQ_REPORT_C:
    ProcessGPSIqLog( pLogRecordHeader );
    break;

    default:
    break;
  }
}

/*===========================================================================

FUNCTION
  ReadLogRecord

DESCRIPTION
  This function reads the input file character by character to determine
  log message boundaries.  When a log message is found, it is passed to the
  format routine to handle each possible log message.


PARAMETERS
  None

OUTPUTS
  Returns TRUE when a log message has been decoded.
  Returns FALSE when the end of file is reached.

===========================================================================*/
boolean ReadLogRecord( void )
{
boolean  bSkipping = 0;
int i = 0;
int C;
int nRecordLength, nRemaining;
LOG_RECORD_HEADER* pLogRecordHeader = (LOG_RECORD_HEADER*)uInBuf;


  while ((C = fgetc(hInFile)) != EOF)
  {
   if (i >= sizeof(uInBuf))
   {
     i = 0;
     bSkipping = 1;
   }
   uInBuf[i++] = C;
   if (i == 12)
   {
     nRecordLength = pLogRecordHeader->length;
     nRemaining = nRecordLength - LOG_RECORD_HEADER_LENGTH;
   }
   else if (i < LOG_RECORD_HEADER_LENGTH) continue;
   if (nRemaining-- <= 0)
   {
     if (!bSkipping) return TRUE;
     bSkipping = 0;
     i = 0;
   }
  }
  return FALSE;
}


/*===========================================================================

FUNCTION
  DisplayHelp

DESCRIPTION
  This function displays the usage of this command.  It is displayed when
  the user invokes an inappropriate use of the command.

PARAMETERS
  None

===========================================================================*/
void DisplayHelp( void )
{
  printf ("\nUsage: IqPacketParser <InFile> <IqFile> <FftFile>\n\n");
  printf ("Outputs two text files based on IQ and FFT data from input dlf file.\n");
  printf (" InFile is a QXDM/Snapper Phone Log (.dlf) file.\n");
  printf (" IqFile is a text file containing IQ values.\n");
  printf (" FftFile is a text file containing FFT values.\n");
 // printf (" If the InFile name has no suffix, default .dlf is assumed.\n");
}


/*===========================================================================

FUNCTION
  GetCommandLineParameters

DESCRIPTION
  Interprets command line parameters and switches.

PARAMETERS
  argc - is the number of command line tokens, including the program name.
  argv - is an array of pointers to blank delimited command line tokens.

OUTPUTS
  Returns TRUE if command line parameters are correct.
  Returns FALSE if the command line syntax is incorrect.

===========================================================================*/
boolean GetCommandLineParameters (int argc, char *argv[])
{
FILE *hFile;
char *pFileName;

  // Display help if there are insufficient parameters
  // argc includes the program name
  if (--argc != 3)
  {
    DisplayHelp();
    return FALSE;
  }

  // Get first file name parameter
  strlcpy(szInFileName, *++argv, sizeof(szInFileName));
  // BANNED_API_CHECK
  //strcpy(szInFileName, *++argv);

  // Add default extension to file name if necessary
  if ( (pFileName = strrchr(szInFileName, '\\')) == 0) pFileName = szInFileName;
  if (!(strchr(pFileName, '.'))) strlcat(szInFileName,".dlf", sizeof(szInFileName));
  // BANNED_API_CHECK
  //if (!(strchr(pFileName, '.'))) strcat(szInFileName,".dlf");

  // Test open file to make sure it exists
  if ((hFile = fopen(szInFileName, "rb")) == NULL)
  {
    fprintf (stderr, "*****Error: Cannot open input file %s.\n", szInFileName);
    return FALSE;
  }
  fclose(hFile);

  // Get second file name parameter
  strlcpy(szOutIqFileName, *++argv, sizeof(szOutIqFileName));
  // BANNED_API_CHECK
  //strcpy(szOutIqFileName, *++argv);

  // Add default extension to file name if necessary
  if ( (pFileName = strrchr(szOutIqFileName, '\\')) == 0) pFileName = szOutIqFileName;
  if (!(strchr(pFileName, '.'))) strlcat(szOutIqFileName,".bin", sizeof(szOutIqFileName));
  // BANNED_API_CHECK
  //if (!(strchr(pFileName, '.'))) strcat(szOutIqFileName,".bin");

  // Get third file name parameter
  strlcpy(szOutFftFileName, *++argv, sizeof(szOutFftFileName));
  // BANNED_API_CHECK
  //strcpy(szOutFftFileName, *++argv);

  // Add default extension to file name if necessary
  if ( (pFileName = strrchr(szOutFftFileName, '\\')) == 0) pFileName = szOutFftFileName;
  if (!(strchr(pFileName, '.'))) strlcat(szOutFftFileName,".txt", sizeof(szOutFftFileName));
  // BANNED_API_CHECK
  //if (!(strchr(pFileName, '.'))) strcat(szOutFftFileName,".txt");

  return TRUE;
}

/*===========================================================================

FUNCTION
  main

DESCRIPTION
  Main program entry point

PARAMETERS
  argc - is the number of command line tokens, including the program name.
  argv - is an array of pointers to blank delimited command line tokens.

OUTPUTS
  Returns FALSE if there are no problems.
  Returns TRUE if problems encountered.

===========================================================================*/
int main(int argc, char* argv[])
{
uint16 w_I;
 
  // Display program name / version and library type / version
  printf("\nIqPacketParser Version 1.4.00 (11/19/2014)\n");

  // Get command line parameters - If there are errors exit with code 1
  if (!GetCommandLineParameters(argc, argv)) return 1;

  hInFile = fopen(szInFileName, "rb");
  if (hInFile == NULL ) return 1;

  iqz_PpState.b_FirstIqConfigPktParsed = FALSE;
  iqz_PpState.b_FirstFftConfigPktParsed = FALSE;
  iqz_PpState.b_IqFilePtrCreated = FALSE;
  iqz_PpState.b_FftFilePtrCreated = FALSE;
  iqz_PpState.b_AllSamplesAccountedFor = FALSE;
  iqz_PpState.b_EndPktReceived = FALSE;

  while (ReadLogRecord()) FormatAndOutputRecord();

  /* In some error conditions, logging just stops abruptly. Below for loop
   * will catch data integrity errors in such cases. For the normal case
   * when everything is OK, it's an extra overhead, but harmless.
   * Note that this standalone app just keeps looking for logs in the dlf
   * until there are no more logs to parse. So an integrity check at the end
   * is imperative.
   */
  for( w_I = 1; w_I <= iqz_PpState.q_NumIqPktsExpected; w_I++ )
  {
    if( iqz_PpState.p_TrackPktNum[w_I] == TRUE )
      continue;
    else
    {
       printf( "Error: Pkt num %u at integ #%u not found in ISF\n",
                w_I, iqz_PpState.u_CurrIntegNum );
    }
  }

  for( w_I = iqz_PpState.u_CurrIntegNum; w_I < iqz_PpState.u_TotalExpectedIntegs; w_I++ )
  {
    printf( "\nError: All pkts in integ #%u are missing in ISF\n", w_I+1 );
  }

  // Announce completion and exit
  printf("\nIqPacketParser Complete\n\n");
  return 0;
}
