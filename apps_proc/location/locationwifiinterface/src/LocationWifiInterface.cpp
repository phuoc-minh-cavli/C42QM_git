/*=========================================================================
  Copyright (c) 2013-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#include <platform_lib_log_util.h>
#include "LocationWifiInterface.h"
#include <LocOSIface.h>
#include <IzatPcid.h>
#include "stdlib.h"
#include "stdint.h"
#include "string.h"

// APIs implemented in C
#ifdef __cplusplus
extern "C" {
#endif

#include "timer.h"
#ifdef __cplusplus
}
#endif

using namespace std;

namespace izat_location_wifi_interface {

#define WIFI_DRIVER_ON_REQ_TIMER_NAME "Wifi Driver ON Request Timer"
#define WIFI_DRIVER_KEEP_ON_TIMER_NAME "Wifi Driver Keep ON Timer"
#define WIFI_SCAN_REQ_TIMER_NAME "Wifi Scan Request Timer"
#define MAX_WIFI_DRIVER_ON_RETRY 3

static void mWifiDriverOnReqTimerCb(void* wifiInterfacePtr) 
{
  LOC_LOGH0("mWifiDriverOnReqTimerCb()");
    static_cast<LocationWifiInterface*>(wifiInterfacePtr)->wifiDriverOnReqTimerExpired();
}

static void mWifiDriverKeepOnTimerCb(void* wifiInterfacePtr) 
{
  LOC_LOGH0("mWifiDriverOnReqTimerCb()");
    static_cast<LocationWifiInterface*>(wifiInterfacePtr)->wifiDriverKeepOnTimerExpired();
}

static void mWifiScanReqTimerCb(void* wifiInterfacePtr) 
{
  LOC_LOGH0("mWifiScanReqTimerCb()");
    static_cast<LocationWifiInterface*>(wifiInterfacePtr)->wifiScanReqTimerExpired();
}


void QCA4004_PowerStateCb(uint32_t requestId,
	                 QCA4004_Error_Core_t errCode,
	                 QCA4004_Power_State_t state,
                  unsigned char macAddWIfiChip[6])
{
  location_wifi_interface_op_status opStatus;
  location_wifi_interface_driver_state driverStatus;
  unsigned char MacIdWifiChip[6] = {0};
LOC_LOGL3("QCA4004_PowerStateCb for reqid %d, error code : %d, State: %d", 
   	          requestId, errCode, state);

  switch(errCode)
  {
    case QCA4004_ERROR_SUCCESS:
      opStatus = OP_STATUS_SUCCESS;
      break;
    case QCA4004_ERR_GPIO:
      opStatus = OP_STATUS_GPIO_FAILURE;
      break;
    default:
      opStatus = OP_STATUS_GENERAL_FAILURE;
      break;
  }

  switch(state)
  {
    case QCA4004_POWER_ON:
      driverStatus = DRIVER_STATE_POWER_ON;
      break;
    case QCA4004_POWER_OFF:
      driverStatus = DRIVER_STATE_POWER_OFF;
      break;
    default:
      driverStatus = DRIVER_STATE_POWER_OFF;
      break;
  }

  LocationWifiInterface::getInstance()->PowerStateCb(requestId, 
                                                   opStatus,
                                                   driverStatus,
                                                   macAddWIfiChip);
                                                
}

void QCA4004_ScanStateCb(uint32_t requestId,
	                 QCA4004_Error_Core_t errCode,
	                 unsigned char macIdWifiChip[6],
                         uint8_t numAps,
	                 QCA4004_Bss_Scan_Info* pOut)
{
  location_wifi_interface_op_status opStatus;
    LOC_LOGL3("QCA4004_ScanStateCb for reqid %d, error code : %d, numAps = %d", 
             requestId, 
             errCode,
             numAps);

  switch(errCode)
  {
    case QCA4004_ERROR_SUCCESS:
      opStatus = OP_STATUS_SUCCESS;
      break;
    case QCA4004_ERR_GPIO:
      opStatus = OP_STATUS_GPIO_FAILURE;
      break;
    default:
      opStatus = OP_STATUS_GENERAL_FAILURE;
      break;
  }
  LocationWifiInterface::getInstance()->ScanStateCb(requestId, opStatus, macIdWifiChip, numAps, pOut);
}


qca4004_Callback_Table_t zCallBackTable = { 
  QCA4004_PowerStateCb,
  QCA4004_ScanStateCb
};


void LocationWifiInterface::PowerStateCb(uint32_t requestId,
	                                      location_wifi_interface_op_status errCode,
	                                      location_wifi_interface_driver_state driverState,
                                       unsigned char macAddWIfiChip[6])
{
   Location_Wifi_Interface_Operation_Info scanInfo = {0};
   memset(&scanInfo, 0, sizeof(Location_Wifi_Interface_Operation_Info));
   LOC_LOGL3("Power callback for reqid %d, error code : %d, driverState: %d", 
   	          requestId, errCode, driverState);
   mDriverState = driverState;
   /* Stop the driver On timer */
   mWifiDriverOnReqTimer.stop();
   if((OP_STATUS_SUCCESS !=  errCode))     
   {
     if(DRIVER_STATE_POWER_ON != driverState)
     {
       LOC_LOGL1("Wifi driver not in ON state  %d", driverState);
       mDriverState = driverState;
       
       if(NULL != mClientScancb)
       {
         scanInfo.opStatus = OP_STATUS_GENERAL_FAILURE;
         scanInfo.opType = OP_TYPE_UNKNOWN;
         mClientScancb(&scanInfo);
         return;
       }
     }
   }// end if
  else
  {
    if(TRUE == mMacAddWifiChipRequested)
    {
      LOC_LOGL1("Wifi chip mac add for reqid1 %d", requestId);
      mMacAddWifiChipAvail = TRUE;
      scanInfo.requestId = requestId;
      scanInfo.numAps = NUM_SCAN_APS_INVALID;
      scanInfo.opStatus = OP_STATUS_SUCCESS;
      scanInfo.opType = OP_TYPE_DRIVER_ADDRESS;
      memcpy(scanInfo.macAddWifiChip, macAddWIfiChip, sizeof(scanInfo.macAddWifiChip));
      mMacAddWifiChipRequested = FALSE;
      LOC_LOGSM6("Mac addr of wifi chip is 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", 
                scanInfo.macAddWifiChip[0],
                scanInfo.macAddWifiChip[1],
                scanInfo.macAddWifiChip[2],
                scanInfo.macAddWifiChip[3],
                scanInfo.macAddWifiChip[4],
                scanInfo.macAddWifiChip[5]);
      mClientScancb(&scanInfo);
    }
    if(DRIVER_STATE_POWER_ON == driverState)
    {
      /* Start the wifi driver keep on timer */
      mWifiDriverKeepOnTimer.stop();
      mWifiDriverKeepOnTimer.start();
#ifdef FEATURE_QCA4004
      if(TRUE == mScanRequested)
      {
        mDriverState = driverState;

        QCA4004_Status_t status = qca4004_get_AP_list(requestId, WIFI_SCAN_TIMEOUT_S);
		LOC_LOGL2("Driver turned ON req scan from for reqid %d status %d", requestId, status);
        /* Stop and restart the wifi scan request timer */
        mWifiScanReqTimer.stop();
        mWifiScanReqTimer.start();
      }
#endif
    }// end if(QCA4004_POWER_ON == state)
  }
}


void LocationWifiInterface::ScanStateCb(	uint32_t requestId,
	                                      location_wifi_interface_op_status errCode,
                                       unsigned char macIdWifiChip[6],
                                       uint8_t numAps,
	                                      void* pOut)
{
  Location_Wifi_Interface_Operation_Info scanInfo = {0};
  memset(&scanInfo, 0, sizeof(Location_Wifi_Interface_Operation_Info));
  QCA4004_Bss_Scan_Info* pScanOut = (QCA4004_Bss_Scan_Info*)malloc(numAps* sizeof(QCA4004_Bss_Scan_Info));
  memcpy(pScanOut, pOut, numAps*sizeof(QCA4004_Bss_Scan_Info));
  //QCA4004_Bss_Scan_Info* pScanOut = (QCA4004_Bss_Scan_Info*)pOut;
  LOC_LOGL3("Scan callback for reqid %d, error code : %d, numAps = %d", 
             requestId, 
             errCode,
             numAps);
  scanInfo.requestId = requestId;
  mScanRequested = FALSE;
 /* Stop the wifi scan request timer */
 mWifiScanReqTimer.stop();

  if(OP_STATUS_SUCCESS !=  errCode)
  {
    LOC_LOGL1("Error in scan for reqid  %d", requestId);
    if(NULL != mClientScancb)
    {
      scanInfo.opStatus = OP_STATUS_GENERAL_FAILURE;
      scanInfo.opType = OP_TYPE_WIFI_SCAN;
      mClientScancb(&scanInfo);
      return;
    }
  }// end if
  else
  {
    /* Copy the scan information */
    scanInfo.numAps = numAps;
    scanInfo.opType = OP_TYPE_WIFI_SCAN;
    memcpy(scanInfo.macAddWifiChip, macIdWifiChip, sizeof(macIdWifiChip));
    for(uint32_t i = 0; i < numAps; i++)
    {
      Location_Wifi_Interface_Ap_Info zApInfo = {0};
      if(NULL != pScanOut)
      {
        zApInfo.channel = pScanOut[i].channel;
        zApInfo.rssi = pScanOut[i].rssi;
        zApInfo.ssid_len = pScanOut[i].ssid_len;
        memcpy(zApInfo.ssid, pScanOut[i].ssid, sizeof(pScanOut[i].ssid));
        memcpy(zApInfo.bssid, pScanOut[i].bssid, sizeof(pScanOut[i].bssid));

      }
      LOC_LOGSM1("AP --> %d", i) ;
      LOC_LOGSM6("Mac Addrs =0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", 
             zApInfo.bssid[0],zApInfo.bssid[1],zApInfo.bssid[2],zApInfo.bssid[3],zApInfo.bssid[4],zApInfo.bssid[5]);
      LOC_LOGSM3(" Channel %d , RSSI: %d, SSID: %s ", zApInfo.channel, zApInfo.rssi, zApInfo.ssid);
      scanInfo.pApInfo.push_back(zApInfo);
    }
    if(NULL != mClientScancb)
    {
		    scanInfo.opStatus = OP_STATUS_SUCCESS;
      mClientScancb(&scanInfo);
    }
  }// end else
  if (pScanOut != NULL)
  {
  	free(pScanOut);
  }
}

LocationWifiInterface* LocationWifiInterface::sWifiInterfaceInstance = 0;

LocationWifiInterface* LocationWifiInterface::getInstance(void){
    if(sWifiInterfaceInstance == 0){
        sWifiInterfaceInstance = new LocationWifiInterface();
    }
    return sWifiInterfaceInstance;
}

void LocationWifiInterface::destroyInstance(){
    delete sWifiInterfaceInstance;
    sWifiInterfaceInstance = 0;
    return;
}


LocationWifiInterface::LocationWifiInterface(void):mWifiDriverOnReqTimer(WIFI_DRIVER_ON_REQ_TIMER_NAME),
                                               mWifiScanReqTimer(WIFI_SCAN_REQ_TIMER_NAME),
                                               mWifiDriverKeepOnTimer(WIFI_DRIVER_KEEP_ON_TIMER_NAME)
{
  mClientScancb = NULL;
  mWifiDriverOnReqTimer.set(WIFI_DRIVER_ON_REQ_TIMEOUT_S, mWifiDriverOnReqTimerCb, this);
  mWifiDriverKeepOnTimer.set(WIFI_DRIVER_KEEP_ON_TIMEOUT_S, mWifiDriverKeepOnTimerCb, this);
  mWifiScanReqTimer.set(WIFI_SCAN_TIMEOUT_S+1, mWifiScanReqTimerCb, this);
  mDriverOnRetryCnt = 0;
  mScanRequested = FALSE;
}

bool LocationWifiInterface::initLocationWifiInterface(OpResultCb ScanCbFn){
#ifdef FEATURE_QCA4004
    qca4004_init(&zCallBackTable);
    mClientScancb = ScanCbFn;
    return true;
#else
	LOC_LOGSM1("%s wifi driver not supported",__FUNCTION__);
	return false;
#endif
}

LocationWifiInterface::~LocationWifiInterface()
{
    LOC_LOGSM1("%s",__FUNCTION__);
}


void LocationWifiInterface::requestWifiScan(uint32_t clientReqId)
{
#ifdef FEATURE_QCA4004
  if(TRUE == mScanRequested)
  {
    LOC_LOGL1("Wifi scan already requested by another reqid %d", mScanRequestId);
    return;
  }
  mScanRequestId = clientReqId;
  /* If the wifi driver is already turned ON directly request the scan */
  if(DRIVER_STATE_POWER_ON == mDriverState)
  {
    LOC_LOGL1("Wifi driver already ON req scan from wifi driver for reqid %d", mScanRequestId);
    mScanRequested = TRUE;
    /* Stop and restart the wifi scan req timer */
    mWifiScanReqTimer.stop();
    mWifiScanReqTimer.start();
    QCA4004_Status_t status = qca4004_get_AP_list(mScanRequestId, WIFI_SCAN_TIMEOUT_S);
	LOC_LOGL1("Driver turned ON  status %d", status);
  }
  else
  {
    mScanRequested = TRUE;
    mDriverOnRequestId = clientReqId;
    if(0 == mDriverOnRetryCnt)
    {
      mDriverOnRetryCnt = 1;
    }
    else
    {
      mDriverOnRetryCnt++;
    }
    /* Stop and restart the driver on timer */
    mWifiDriverOnReqTimer.stop();
    mWifiDriverOnReqTimer.start();

    /* First turn on the wifi driver . When the driver is turned on , 
     then request for scan */
    qca4004_power_state_change(mDriverOnRequestId, QCA4004_POWER_ON);
  }
 #else
 	LOC_LOGSM1("%s wifi driver not supported",__FUNCTION__);
 #endif
}



void LocationWifiInterface::requestWifiChipMac(uint32_t clientReqId)
{
#ifdef FEATURE_QCA4004
  if(TRUE == mMacAddWifiChipRequested)
  {
    LOC_LOGL1("Wifi chip MAC is already requested by another reqid %d", mDriverOnRequestId);
    return;
  }
  if(TRUE == mMacAddWifiChipAvail)
  {
    Location_Wifi_Interface_Operation_Info scanInfo = {0};
    LOC_LOGL1("Wifi chip mac already available for %d", clientReqId);
    scanInfo.requestId = clientReqId;
    scanInfo.numAps = 0;
    scanInfo.opStatus = OP_STATUS_SUCCESS;
    scanInfo.opType = OP_TYPE_DRIVER_ADDRESS;
    memcpy(scanInfo.macAddWifiChip, mMacAddWifiChip, sizeof(scanInfo.macAddWifiChip));
    if(NULL != mClientScancb)
    {
      mClientScancb(&scanInfo);
    }
    return;
  }
  else
  {
    mMacAddWifiChipRequested = TRUE;
    mDriverOnRequestId = clientReqId;
    if(0 == mDriverOnRetryCnt)
    {
      mDriverOnRetryCnt = 1;
    }
    else
    {
      mDriverOnRetryCnt++;
    }
    /* Stop and restart the wifi driver on timer */
    mWifiDriverOnReqTimer.stop();
    mWifiDriverOnReqTimer.start();

    /* First turn on the wifi driver . When the driver is turned on , 
     the wifi chip mac is returned  */
    qca4004_power_state_change(clientReqId, QCA4004_POWER_ON);
  }
#else
	LOC_LOGSM1("%s wifi driver not supported",__FUNCTION__);
#endif

}


void LocationWifiInterface::wifiDriverOnReqTimerExpired(void) 
{
#ifdef FEATURE_QCA4004
  LOC_LOGL1("Wifi driver ON timer expired: scan req?:%d ", mScanRequested);

  /* Retry attempt to turn on Wifi Driver again */
  if(mDriverOnRetryCnt <= MAX_WIFI_DRIVER_ON_RETRY)
  {
    LOC_LOGL1("Wifi driver ON retry attempt %d", mDriverOnRetryCnt);
    /* Stop and restart the wifi driver on timer */
    mWifiDriverOnReqTimer.stop();
    mWifiDriverOnReqTimer.start();

    /* First turn on the wifi driver . When the driver is turned on , 
     the wifi chip mac is returned  */
    qca4004_power_state_change(mDriverOnRequestId, QCA4004_POWER_ON);
  }
  else
  {
    LOC_LOGL1("Max retry attempt for driver ON for request %d ", mDriverOnRequestId);
    mDriverOnRetryCnt = 0;
  }
 #else
 	LOC_LOGSM1("%s wifi driver not supported",__FUNCTION__);
 #endif
}



void LocationWifiInterface::wifiScanReqTimerExpired(void) 
{
  LOC_LOGL1("Wifi scan req timer expired: driver state : %d", mDriverState);
  Location_Wifi_Interface_Operation_Info scanInfo = {0};
  scanInfo.requestId = mScanRequestId;
  scanInfo.numAps = 0;
  scanInfo.opStatus = OP_STATUS_TIMEOUT;
  scanInfo.opType = OP_TYPE_WIFI_SCAN;
  if(NULL != mClientScancb)
  {
    mClientScancb(&scanInfo);
  }
}


void LocationWifiInterface::wifiDriverKeepOnTimerExpired(void) 
{
#ifdef FEATURE_QCA4004
  uint32 q_ReqId = rand()%100;
  LOC_LOGL1("Wifi keep ON expired: driver state : %d", mDriverState);
  /* If an active scan request is going on wait for some more time to turn OFF the driver */
  if(TRUE == mScanRequested)
  {
    mWifiDriverKeepOnTimer.start();
    return;
  }
  /* Turn off the wifi driver */
  if(mDriverState != DRIVER_STATE_POWER_OFF)
  {
    LOC_LOGL1("Turn OFF driver for reqid %d", q_ReqId);
    qca4004_power_state_change(q_ReqId, QCA4004_POWER_OFF);
  }
 #else
 	LOC_LOGSM1("%s wifi driver not supported",__FUNCTION__);
 #endif

}

} // end of namespace


