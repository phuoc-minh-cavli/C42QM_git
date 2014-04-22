/*============================================================================
  FILE:         TsensDiag.c

  OVERVIEW:     Diag interface to TSENS.

  DEPENDENCIES: Diag, Timetick.

                Copyright (c) 2019 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.10/hwengines/tsens/diag/src/TsensDiag.c#1 $$DateTime: 2019/03/06 01:39:50 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2019-03-04  ay  Initial revision(saipan as reference)

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "TsensDiag.h"
#include "DDITsens.h"
#include "DDITimetick.h"
#include "DalDevice.h"
#include "DALSys.h"
#include "DALDeviceId.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct
{
   DalDeviceHandle *phTimetick;
   DalDeviceHandle *phTsens;
   uint32 uNumSensors;
} TsensDiagCtxt;

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/
static void* TsensDiagHandleGetVersion(void *pReqPkt, uint16 uPktLen);
static void* TsensDiagHandleGetNumSensors(void *pReqPkt, uint16 uPktLen);
static void* TsensDiagHandleGetTemp(void *pReqPkt, uint16 uPktLen);

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
boolean gbTsensDiagInitialized = FALSE;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static TsensDiagCtxt tsensDiagCtxt;

static const diagpkt_user_table_entry_type gTsensDiagDispatchTable[] =
{
   {
      /* cmd_code_lo */ TSENS_DIAG_CMD_GET_VERSION,
      /* cmd_code_hi */ TSENS_DIAG_CMD_GET_VERSION,
      /* func_ptr    */ TsensDiagHandleGetVersion
   },

   {
      /* cmd_code_lo */ TSENS_DIAG_CMD_GET_NUM_SENSORS,
      /* cmd_code_hi */ TSENS_DIAG_CMD_GET_NUM_SENSORS,
      /* func_ptr    */ TsensDiagHandleGetNumSensors
   },

   {
      /* cmd_code_lo */ TSENS_DIAG_CMD_GET_TEMP,
      /* cmd_code_hi */ TSENS_DIAG_CMD_GET_TEMP,
      /* func_ptr    */ TsensDiagHandleGetTemp
   },
};

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
/*======================================================================

  FUNCTION       TsensDiagGetTimetick

  DESCRIPTION    Gets the current timetick.

  DEPENDENCIES   None

  PARAMETERS     None

  RETURN VALUE   Current 64-bit timetick.

  SIDE EFFECTS   None

======================================================================*/
static uint64 TsensDiagGetTimetick(void)
{
   DALResult result;
   DalTimetickTime64Type u64Tick;

   result = DalTimetick_GetTimetick64(tsensDiagCtxt.phTimetick, &u64Tick);
   if (result != DAL_SUCCESS)
   {
      return 0;
   }

   return (uint64)u64Tick;
}

/*======================================================================

  FUNCTION       TsensDiagHandleGetVersion

  DESCRIPTION    Generates the response for the GetVersion request.

  DEPENDENCIES   None

  PARAMETERS
     pReqPkt [in]: request packet
     uPktLen [in]: request packet length

  RETURN VALUE   Pointer to the response packet.

  SIDE EFFECTS   None

======================================================================*/
static void* TsensDiagHandleGetVersion(void *pReqPkt, uint16 uPktLen)
{
   TsensDiagCmdGetVersionRsp *pRespPkt;

   if (uPktLen != sizeof(TsensDiagCmdGetVersionReq))
   {
      DALSYS_LogEvent(DALDEVICEID_TSENS,
                      DALSYS_LOGEVENT_ERROR,
                      "TsensDiag: GetVersionReq invalid length");
      return NULL;
   }

   pRespPkt = (TsensDiagCmdGetVersionRsp *)diagpkt_subsys_alloc(TSENS_DIAG_SYBSYS,
                                                                TSENS_DIAG_CMD_GET_VERSION,
                                                                sizeof(*pRespPkt));
   if (NULL == pRespPkt)
   {
      DALSYS_LogEvent(DALDEVICEID_TSENS,
                      DALSYS_LOGEVENT_ERROR,
                      "TsensDiag: failed to allocate GetVersionRsp packet");
      return pRespPkt;
   }

   pRespPkt->uResult = TSENS_DIAG_SUCCESS;
   pRespPkt->ucMajorVersion = TSENS_DIAG_MAJOR_VERSION;
   pRespPkt->ucMinorVersion = TSENS_DIAG_MINOR_VERSION;

   return pRespPkt;
}

/*======================================================================

  FUNCTION       TsensDiagHandleGetNumSensors

  DESCRIPTION    Generates the response for the GetNumSensors request.

  DEPENDENCIES   None

  PARAMETERS
     pReqPkt [in]: request packet
     uPktLen [in]: request packet length

  RETURN VALUE   Pointer to the response packet.

  SIDE EFFECTS   None

======================================================================*/
static void* TsensDiagHandleGetNumSensors(void *pReqPkt, uint16 uPktLen)
{
   TsensDiagCmdGetNumSensorsRsp *pRespPkt;

   if (uPktLen != sizeof(TsensDiagCmdGetNumSensorsReq))
   {
      DALSYS_LogEvent(DALDEVICEID_TSENS,
                      DALSYS_LOGEVENT_ERROR,
                      "TsensDiag: GetNumSensorsReq invalid length");
      return NULL;
   }

   pRespPkt = (TsensDiagCmdGetNumSensorsRsp *)diagpkt_subsys_alloc(TSENS_DIAG_SYBSYS,
                                                                   TSENS_DIAG_CMD_GET_NUM_SENSORS,
                                                                   sizeof(*pRespPkt));
   if (NULL == pRespPkt)
   {
      DALSYS_LogEvent(DALDEVICEID_TSENS,
                      DALSYS_LOGEVENT_ERROR,
                      "TsensDiag: failed to allocate GetNumSensorsRsp packet");
      return pRespPkt;
   }

   pRespPkt->uResult = TSENS_DIAG_SUCCESS;
   pRespPkt->uNumSensors = (uint16)tsensDiagCtxt.uNumSensors;

   return pRespPkt;
}

/*======================================================================

  FUNCTION       TsensDiagHandleGetTemp

  DESCRIPTION    Generates the response for the GetTemp request.

  DEPENDENCIES   None

  PARAMETERS
     pReqPkt [in]: request packet
     uPktLen [in]: request packet length

  RETURN VALUE   Pointer to the response packet.

  SIDE EFFECTS   None

======================================================================*/
static void* TsensDiagHandleGetTemp(void *pReqPkt, uint16 uPktLen)
{
   const TsensDiagCmdGetTempReq *pGetTempReq = (TsensDiagCmdGetTempReq *)pReqPkt;
   TsensDiagCmdGetTempRsp *pRespPkt;
   TsensDiagReadResult *pResult;
   uint32 uNumSensors;
   size_t reqPacketSize;
   size_t rspPacketSize;
   uint32 uIndex;
   DALResult result;
   TsensTempType tsensResult;

   if (uPktLen < sizeof(TsensDiagCmdGetTempReq))
   {
      DALSYS_LogEvent(DALDEVICEID_TSENS,
                      DALSYS_LOGEVENT_ERROR,
                      "TsensDiag: GetTempReq invalid packet");
      return NULL;
   }

   uNumSensors = pGetTempReq->uNumSensors;
   if (uNumSensors == 0)
   {
      DALSYS_LogEvent(DALDEVICEID_TSENS,
                      DALSYS_LOGEVENT_ERROR,
                      "TsensDiag: GetTempReq invalid num channels");
      return NULL;
   }

   reqPacketSize = sizeof(*pGetTempReq) + ((uNumSensors - 1) * sizeof(pGetTempReq->auSensor[0]));
   if (reqPacketSize != (size_t)uPktLen)
   {
      DALSYS_LogEvent(DALDEVICEID_TSENS,
                      DALSYS_LOGEVENT_ERROR,
                      "TsensDiag: GetTempReq invalid length");
      return NULL;
   }

   rspPacketSize = sizeof(*pRespPkt) + ((uNumSensors - 1) * sizeof(pRespPkt->aResult[0]));

   pRespPkt = (TsensDiagCmdGetTempRsp *)diagpkt_subsys_alloc(TSENS_DIAG_SYBSYS,
                                                             TSENS_DIAG_CMD_GET_TEMP,
                                                             rspPacketSize);
   if (NULL == pRespPkt)
   {
      DALSYS_LogEvent(DALDEVICEID_TSENS,
                      DALSYS_LOGEVENT_ERROR,
                      "TsensDiag: failed to allocate GetTempRsp packet");
      return pRespPkt;
   }

   pRespPkt->uResult = TSENS_DIAG_ERROR;
   pRespPkt->uNumResults = (uint16)uNumSensors;
   pRespPkt->uStartTimestamp = TsensDiagGetTimetick();

   for (uIndex = 0; uIndex < uNumSensors; uIndex++)
   {
      pResult = &pRespPkt->aResult[uIndex];
      pResult->uSensor = pGetTempReq->auSensor[uIndex];

      if (pResult->uSensor >= tsensDiagCtxt.uNumSensors)
      {
         pResult->uResult = TSENS_DIAG_ERROR_UNSUPPORTED;
         continue;
      }

      result = DalTsens_GetTemp(tsensDiagCtxt.phTsens, pResult->uSensor, &tsensResult);
      if (result == DAL_SUCCESS)
      {
         pResult->nDeciDegc = (tsensResult.nDegC)*10;
         pResult->uResult = TSENS_DIAG_SUCCESS;
      }
      else
      {
         switch (result)
         {
            case TSENS_ERROR_INVALID_CALIBRATION:
               pResult->uResult = TSENS_DIAG_ERROR_INVALID_CALIBRATION;
            case TSENS_ERROR_TIMEOUT:
               pResult->uResult = TSENS_DIAG_ERROR_TIMEOUT;
            case TSENS_ERROR_INVALID_PARAMETER:
               pResult->uResult = TSENS_DIAG_ERROR_INVALID_PARAMETER;
            case TSENS_ERROR_UNSUPPORTED:
               pResult->uResult = TSENS_DIAG_ERROR_UNSUPPORTED;
            default:
               pResult->uResult = TSENS_DIAG_ERROR;
         }
      }
   }

   pRespPkt->uEndTimestamp = TsensDiagGetTimetick();
   pRespPkt->uResult = TSENS_DIAG_SUCCESS;

   return pRespPkt;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*======================================================================

  FUNCTION       TsensDiagInit

  DESCRIPTION    Attaches to TSENS and initializes the TSENS diag
                 interface.

                 This is called from RCINIT.

  DEPENDENCIES   None

  PARAMETERS     None

  RETURN VALUE   None.

  SIDE EFFECTS   None

======================================================================*/
void TsensDiagInit(void)
{
   DALResult result;

   if (gbTsensDiagInitialized == TRUE)
   {
      return;
   }

   DALSYS_InitMod(NULL);

   result = DalTimetick_Attach("SystemTimer", &tsensDiagCtxt.phTimetick);
   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(DALSYS_LOGEVENT_ERROR,
                      DALSYS_LOGEVENT_FATAL_ERROR,
                      "TsensDiag: failed to attach to Timetick");
      return;
   }

   result = DAL_TsensDeviceAttach(DALDEVICEID_TSENS, &tsensDiagCtxt.phTsens);
   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(DALDEVICEID_TSENS,
                      DALSYS_LOGEVENT_ERROR,
                      "TsensDiag: failed to attach to TSENS");
      return;
   }

   result = DalDevice_Open(tsensDiagCtxt.phTsens, DAL_OPEN_SHARED);
   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(DALDEVICEID_TSENS,
                      DALSYS_LOGEVENT_ERROR,
                      "TsensDiag: failed to open");
      return;
   }

   result = DalTsens_GetNumSensors(tsensDiagCtxt.phTsens, &tsensDiagCtxt.uNumSensors);
   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(DALDEVICEID_TSENS,
                      DALSYS_LOGEVENT_ERROR,
                      "TsensDiag: failed to get number of sensors");
      return;
   }

   DIAGPKT_DISPATCH_TABLE_REGISTER(TSENS_DIAG_SYBSYS, gTsensDiagDispatchTable);

   gbTsensDiagInitialized = TRUE;

   return;
}

