#ifndef FTM_HWTC_IQCAPTURE_H
#define FTM_HWTC_IQCAPTURE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM HWTC IQ CAPTURE

GENERAL DESCRIPTION
  This is the header file for the embedded FTM HWTC SBI commands comming from the diag service 

Copyright (c) 1994, 1995, 1996 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001, 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2021 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rfa.mpss/3.10/rf/common/hwtc/inc/ftm_hwtc_iqcapture.h#2 $
  $DateTime: 2021/04/14 23:20:15 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/23/21   sr      Remove support for iq capture hwtc command
12/07/11   sb      Support for GSM IQ capture in Online and FTM mode 
11/29/11   sb      Added functionality for GSM IQ CAPTURE
08/03/11   dw      Remove legacy UMTS IQ capture functions
05/24/11   aro     Re-structured IQ Capture to have generic "Get Config"
12/20/10   aro     Added enums for third WB Path
09/25/09   aro     Fixed "PACKED" usage to be compatible with QDSP6
07/02/08   adk     Add support for 8-bit IQ capture in 1x mode.
03/28/08   ad      Changes to support M6800B, M7500 targets.
11/20/07   jfc     Added function headers, declared some new prototypes.
12/07/06   adk     Added enum_iq_antenna_type and prototype ftm_hwtc_iq_capture_dispatch().
10-06-06   rt      Add function to support cleaner functionality of capture algorithm
01-23-03   lcl     Added SBI data structure
11-11-02   ra      Created 

===========================================================================*/

#include "rfa_variation.h"
#include "customer.h"
#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_FTM_HWTC

#include "diagpkt.h"
#include "ftm_hwtc_dispatch.h"

typedef enum
{
  FTM_HWTC_IQ_FAILURE,
  FTM_HWTC_IQ_SUCCESS
} iq_capture_result_type;


/******************************* IQ DATA STRUCTURE **************************/

#define FTM_GSM_MAX_BUFFER     1024
// Maximum size of GSM IQ buffer in samples 

typedef enum
{
  FTM_IQ_CARRIER_PATH_0,
  FTM_IQ_CARRIER_PATH_1,
  FTM_IQ_CARRIER_PATH_2
} ftm_iq_carrier_path_type;

typedef enum
{
  FTM_IQ_ANTENNA_0,
  FTM_IQ_ANTENNA_1
} ftm_iq_antenna_type;

typedef enum
{
  FTM_HWTC_IQ_4_BITS,
  FTM_HWTC_IQ_8_BITS,
  FTM_HWTC_IQ_16_BITS
} enum_iq_sample_bit_format_type;


typedef enum
{
    FTM_HWTC_SAMPSRV_PATH_0,
    FTM_HWTC_SAMPSRV_PATH_1
} ftm_hwtc_sampsrv_path_type;

typedef enum
{
    FTM_HWTC_SAMPSRV_CAPTURE_MODE_CONTINUOUS,
    FTM_HWTC_SAMPSRV_CAPTURE_MODE_WINDOW
} ftm_hwtc_sampsrv_capture_mode_type;

typedef enum
{
    FTM_HWTC_SAMPSRV_CAPTURE_TYPE_ADDR,
    FTM_HWTC_SAMPSRV_CAPTURE_TYPE_IMMEDIATE
} ftm_hwtc_sampsrv_capture_type;

typedef enum
{
    FTM_HWTC_SAMPSRV_CAPTURE_SEL_IQ_DATA,
    FTM_HWTC_SAMPSRV_CAPTURE_SEL_Y1Y2_DATA
} ftm_hwtc_sampsrv_capture_data_type;

typedef struct
{
    boolean                              capture_in_progress;
    ftm_hwtc_sampsrv_capture_mode_type   capture_mode;
    ftm_hwtc_sampsrv_capture_type        capture_type;
    ftm_hwtc_sampsrv_capture_data_type   capture_data_source;
} ftm_hwtc_sampsrv_status;

typedef PACK(struct)
{
  word data[FTM_IQ_RECORD_BLOCK_SIZE];
  word recordIndex;
} ftm_hwtc_iq_data_type;

typedef PACK(struct)
{
  word maxRecordSize;
  word sampleBitFormat;
  dword samplingFrequency;
} ftm_hwtc_iq_config_type;      

#endif /* FEATURE_FTM_HWTC */
#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_HWTC_IQCAPTURE_H */
