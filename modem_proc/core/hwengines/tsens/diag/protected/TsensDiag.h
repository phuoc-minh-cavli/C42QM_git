#ifndef __TSENS_DIAG_H__
#define __TSENS_DIAG_H__
/*============================================================================
  @file TsensDiag.h

  Function and data structure declarations for the TSENS diag interface.


                Copyright (c) 2019 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.mpss/3.10/hwengines/tsens/diag/protected/TsensDiag.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "diagcmd.h"
#include "diagpkt.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define TSENS_DIAG_MAJOR_VERSION  1
#define TSENS_DIAG_MINOR_VERSION  0

#define TSENS_DIAG_CMD_CODE  (DIAG_SUBSYS_CMD_F)
#define TSENS_DIAG_SYBSYS    (DIAG_SUBSYS_COREBSP)

#define TSENS_DIAG_CMD_GET_VERSION      0x100
#define TSENS_DIAG_CMD_GET_NUM_SENSORS  0x101
#define TSENS_DIAG_CMD_GET_TEMP         0x102

#define TSENS_DIAG_SUCCESS                    ((uint16)0)
#define TSENS_DIAG_ERROR                      ((uint16)1)
#define TSENS_DIAG_ERROR_INVALID_CALIBRATION  ((uint16)2)
#define TSENS_DIAG_ERROR_TIMEOUT              ((uint16)3)
#define TSENS_DIAG_ERROR_INVALID_PARAMETER    ((uint16)4)
#define TSENS_DIAG_ERROR_UNSUPPORTED          ((uint16)5)
#define TSENS_DIAG_ERROR_DEAD_SENSOR          ((uint16)6)

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
   diagpkt_subsys_header_type hdr;
} TsensDiagCmdGetVersionReq;

typedef PACKED struct PACKED_POST
{
   diagpkt_subsys_header_type hdr;
   uint16 uResult;
   uint8 ucMajorVersion;
   uint8 ucMinorVersion;
} TsensDiagCmdGetVersionRsp;

typedef PACKED struct PACKED_POST
{
   diagpkt_subsys_header_type hdr;
} TsensDiagCmdGetNumSensorsReq;

typedef PACKED struct PACKED_POST
{
   diagpkt_subsys_header_type hdr;
   uint16 uResult;
   uint16 uNumSensors;
} TsensDiagCmdGetNumSensorsRsp;

typedef PACKED struct PACKED_POST
{
   diagpkt_subsys_header_type hdr;
   uint16 uNumSensors;
   uint16 auSensor[1];
} TsensDiagCmdGetTempReq;

typedef PACKED struct PACKED_POST
{
   uint16 uResult;
   uint16 uSensor;
   int32 nDeciDegc;
} TsensDiagReadResult;

typedef PACKED struct PACKED_POST
{
   diagpkt_subsys_header_type hdr;
   uint16 uResult;
   uint64 uStartTimestamp;
   uint64 uEndTimestamp;
   uint16 uNumResults;
   TsensDiagReadResult aResult[1];
} TsensDiagCmdGetTempRsp;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

#endif /* #ifndef __TSENS_DIAG_H__ */

