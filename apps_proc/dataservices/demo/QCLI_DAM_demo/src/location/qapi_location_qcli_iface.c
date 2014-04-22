/*****************************************************************************
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/

#include "txm_module.h"
#include "qapi_location.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qcli.h"
#include "qcli_api.h"

/*==========================================================================
Signals used for waiting in test app for callbacks
===========================================================================*/
static TX_EVENT_FLAGS_GROUP gSignalGeofenceResponse;
static TX_EVENT_FLAGS_GROUP gSignalResponse;
static TX_EVENT_FLAGS_GROUP gSignalLocation;

/*==========================================================================
Local state variables
===========================================================================*/
static uint32_t sTrackingSessionId = 0;
static uint32_t sBatchingSessionId = 0;
static UINT gGpsVerifyTestInProgress = 0;

#define MAX_SUPPORTED_CLIENTS 4
static uint32_t sActiveClientIdx = 0xFFFFFFFF;
static uint32_t clientIdArr[MAX_SUPPORTED_CLIENTS];
static uint8_t clientInited[MAX_SUPPORTED_CLIENTS];

static uint8_t qcliVerboseLoggingEnabled = 1;

// Just for printing
#define MAX_FRAC_DIGITS 10000000

// Maintain geofence data and options
#define GF_ARRAY_SIZE 5
static uint32_t gfIdArray[GF_ARRAY_SIZE];
static qapi_Geofence_Info_t gfDataArray[GF_ARRAY_SIZE];
static qapi_Geofence_Option_t gfOptionsArray[GF_ARRAY_SIZE];
static uint32_t gfCount = 0;
static bool gfBufferForAddGF;
static bool gfBufferForModifyGF;

#define USER_BUFFER_SIZE 4096
static uint8_t userBuffer[USER_BUFFER_SIZE];

// Handle for LocTest Command Group
static QCLI_Group_Handle_t qcli_loc_handle;

// LOGGING MACROS
#define LOC_LOG_QCLI(fmt, ...) QCLI_Printf(qcli_loc_handle, fmt "\n", ##__VA_ARGS__)
#define LOC_LOG_QCLI_V(fmt, ...) if (qcliVerboseLoggingEnabled) QCLI_Printf(qcli_loc_handle, fmt "\n", ##__VA_ARGS__)

#define MAX_PARAM_LEN 20

typedef struct  {
    uint32_t            paramCount;
    QCLI_Parameter_t*   paramList;
    uint32_t            accessIndex;
} LocQCLIParamBuffer;

/*---------------------------------------------
  location_hal_test APIs to be invoked
 ---------------------------------------------*/
static int location_init(uint32_t clientIdx, uint32_t cbMask);
static int location_deinit(uint32_t clientIdx);

static uint32_t* location_add_geofences(uint32_t clientIdx, size_t count,
                                 qapi_Geofence_Option_t* options,
                                 qapi_Geofence_Info_t* datas);
static int location_modify_geofences(uint32_t clientIdx, size_t count, uint32_t* ids, qapi_Geofence_Option_t* options);
static int location_remove_geofences(uint32_t clientIdx, size_t count, uint32_t* ids);
static int location_pause_geofences(uint32_t clientIdx, size_t count, uint32_t* ids);
static int location_resume_geofences(uint32_t clientIdx, size_t count, uint32_t* ids);

static uint32_t location_start_batching(uint32_t clientIdx, uint32_t minInterval, uint32_t minDistance);
static int location_get_batched_locations(uint32_t clientIdx, uint32_t id, size_t count);
static int location_stop_batching(uint32_t clientIdx, uint32_t id);
static int location_update_batching_options(uint32_t clientIdx, uint32_t id, uint32_t minInterval, uint32_t minDistance);

static uint32_t location_start_tracking(uint32_t clientIdx, uint32_t minInterval, uint32_t minDistance);
static int location_stop_tracking(uint32_t clientIdx, uint32_t id);
static int location_update_tracking_options(uint32_t clientIdx, uint32_t id, uint32_t minInterval, uint32_t minDistance);

static void gps_verify_tracking_test_singleshot(uint32_t clientIdx, uint32_t minInterval, uint32_t minDistance);
static void gps_verify_batching_test_singleshot(uint32_t clientIdx, uint32_t minInterval, uint32_t minDistance);

static uint32_t location_get_single_shot(uint32_t clientIdx, uint32_t powerLevel);
static int location_cancel_single_shot(uint32_t clientIdx, uint32_t id);

static uint32_t location_start_get_gnss_data(uint32_t clientIdx);
static int location_stop_get_gnss_data(uint32_t clientIdx, uint32_t id);

/* Utility APIs */
static double qapi_loc_util_string_to_double(char* str);

static LQPBinit(LocQCLIParamBuffer* pBuff,
                QCLI_Parameter_t *parameterList, 
                uint32_t parameterCount)
{
    pBuff->paramList = parameterList;
    pBuff->paramCount = parameterCount;
    pBuff->accessIndex = 0;
}

// any more parameters ?
bool LQPBparamsLeftToFetch(LocQCLIParamBuffer* pBuff)
{
    return (pBuff->accessIndex < pBuff->paramCount);
}

// Fetch a param as string
char* LQPBgetNextParamAsStr(LocQCLIParamBuffer* pBuff)
{
    if (pBuff->accessIndex >= pBuff->paramCount)
    {
        return NULL;
    }
    return (char*)pBuff->paramList[pBuff->accessIndex++].String_Value;
}

// Fetch a param as integer
int32_t LQPBgetNextParamAsInt(LocQCLIParamBuffer* pBuff)
{
    if (pBuff->accessIndex >= pBuff->paramCount) 
    {
        return -1;
    }
    if (pBuff->paramList[pBuff->accessIndex].Integer_Is_Valid == 0) {
        return -1;
    }
    return pBuff->paramList[pBuff->accessIndex++].Integer_Value;
}

// Parameter index, -1 if not present
int LQPBgetParamIndex(LocQCLIParamBuffer* pBuff, char* param)
{
    int i;

    for (i = 0; i < pBuff->paramCount; i++) 
    {
        if (strncmp(param, (char*)pBuff->paramList[i].String_Value,
            MAX_PARAM_LEN) == 0) 
        {
            return i;
        }
    }
    return -1;
}

// Get parameter at specified index
char* LQPBgetParamAtIndex(LocQCLIParamBuffer* pBuff, int idx) 
{
    if (idx < 0 || idx >= pBuff->paramCount) 
    {
        return NULL;
    }
    return (char*)pBuff->paramList[idx].String_Value;
}

// Fetch param next to the specified one
// can be used to fetch values:
// e.g. lat 98.33445 long 43.77688
// fetch param next to "lat" or "long" for its value
char* LQPBgetParamNextTo(LocQCLIParamBuffer* pBuff, char* param)
{
    int idx = LQPBgetParamIndex(pBuff, param);
    if (idx >= 0) 
    {
        return LQPBgetParamAtIndex(pBuff, idx + 1);
    }
    return NULL;
}

// Functions registered with QCLI as callbacks
static QCLI_Command_Status_t LocTest_enableVerboseLogging(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List) {
    qcliVerboseLoggingEnabled = 1;
    return QCLI_STATUS_SUCCESS_E;
}
static QCLI_Command_Status_t LocTest_disableVerboseLogging(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List) {
    qcliVerboseLoggingEnabled = 0;
    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_init(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    char* clientIdxStr = NULL;
    uint32_t clientIdx = 0;
    char* cbMaskStr = NULL;
    uint32_t cbMask = 0;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    // Client Index to initialize
    clientIdxStr = LQPBgetParamNextTo(&buffer, "client");
    if(clientIdxStr == NULL){
        LOC_LOG_QCLI("client arg missing");
        return QCLI_STATUS_USAGE_E;
    }
    // Now parse the values
    clientIdx = (uint32_t)qapi_loc_util_string_to_double(clientIdxStr);

    // Client Index to initialize
    cbMaskStr = LQPBgetParamNextTo(&buffer, "cbmask");
    if(cbMaskStr == NULL){
        LOC_LOG_QCLI("cbmask arg missing");
        return QCLI_STATUS_USAGE_E;
    }
    // Now parse the values
    cbMask = (uint32_t)qapi_loc_util_string_to_double(cbMaskStr);

    if(location_init(clientIdx, cbMask) == 0)
        LOC_LOG_QCLI("Location Init Success");
    else
        LOC_LOG_QCLI("Location Init Failure !");

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_deinit(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    char* clientIdxStr;
    uint32_t clientIdx;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    // Client index to de-init
    clientIdxStr = LQPBgetParamNextTo(&buffer, "client");
    if(clientIdxStr == NULL){
        LOC_LOG_QCLI("client arg missing");
        return QCLI_STATUS_USAGE_E;
    }

    // Now parse the values
    clientIdx = (uint32_t)qapi_loc_util_string_to_double(clientIdxStr);

    if(location_deinit(clientIdx) == 0)
        LOC_LOG_QCLI("Location Deinit Success");
    else
        LOC_LOG_QCLI("Location Deinit Failure !");

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_set_active_client_idx(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    char* clientIdxStr;
    uint32_t clientIdx;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    // Parse client index which to set as active
    clientIdxStr = LQPBgetParamNextTo(&buffer, "client");
    if(clientIdxStr == NULL){
        LOC_LOG_QCLI("client arg missing");
        return QCLI_STATUS_USAGE_E;
    }

    // Now parse the values
    clientIdx = (uint32_t)qapi_loc_util_string_to_double(clientIdxStr);

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return QCLI_STATUS_SUCCESS_E;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited, first invoke Loc Init",
                clientIdx);
        return QCLI_STATUS_SUCCESS_E;
    }

    sActiveClientIdx = clientIdx;
    LOC_LOG_QCLI("Active client index has been set to %d", sActiveClientIdx);

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_add_to_gf_buf(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;

    // Let's create Geofence Data and Options
    qapi_Geofence_Info_t gfData;
    qapi_Geofence_Option_t gfOptions;
    char* latStr;
    char* longStr;
    char* radStr;
    char* maskStr;
    char* resStr;
    char* dwellStr;

    if(gfCount >= GF_ARRAY_SIZE){
        LOC_LOG_QCLI("No space to store more GF, Add now");
        return QCLI_STATUS_SUCCESS_E;
    }

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    LOC_LOG_QCLI("Parsing arguments..");

    // Latitude
    latStr = LQPBgetParamNextTo(&buffer, "lat");
    if(latStr == NULL){
        LOC_LOG_QCLI("lat arg missing !");
        return QCLI_STATUS_USAGE_E;
    }

    // Longitude
    longStr = LQPBgetParamNextTo(&buffer, "long");
    if(longStr == NULL){
        LOC_LOG_QCLI("long arg missing !");
        return QCLI_STATUS_USAGE_E;
    }

    // Radius
    radStr = LQPBgetParamNextTo(&buffer, "rad");
    if(radStr == NULL){
        LOC_LOG_QCLI("rad arg missing !");
        return QCLI_STATUS_USAGE_E;
    }

    // Breach Type Mask
    maskStr = LQPBgetParamNextTo(&buffer, "mask");
    if(maskStr == NULL){
        LOC_LOG_QCLI("mask arg missing !");
        return QCLI_STATUS_USAGE_E;
    }

    // Responsiveness
    resStr = LQPBgetParamNextTo(&buffer, "res");
    if(resStr == NULL){
        LOC_LOG_QCLI("res arg missing !");
        return QCLI_STATUS_USAGE_E;
    }

    // Dwell Time
    dwellStr = LQPBgetParamNextTo(&buffer, "dwell");
    if(dwellStr == NULL){
        LOC_LOG_QCLI("dwell arg missing !");
        return QCLI_STATUS_USAGE_E;
    }

    // Now print
    LOC_LOG_QCLI("Geofence Data:\n"
            "Latitude: %s, Longitude: %s Radius %s", latStr, longStr, radStr);
    LOC_LOG_QCLI("Geofence Options:\n"
            "Mask %s, Responsiveness %s, Dwell Time %s", maskStr, resStr, dwellStr);

	memset(&gfData, 0, sizeof(qapi_Geofence_Info_t));
	memset(&gfOptions, 0, sizeof(qapi_Geofence_Option_t));
	
    // Now parse and store in geofence data and options
    gfData.latitude = qapi_loc_util_string_to_double(latStr);
    gfData.longitude = qapi_loc_util_string_to_double(longStr);
    gfData.radius = qapi_loc_util_string_to_double(radStr);
    gfOptions.breachTypeMask = (uint8_t) qapi_loc_util_string_to_double(maskStr);
    gfOptions.responsiveness = (uint32_t) qapi_loc_util_string_to_double(resStr);
    gfOptions.dwellTime = (uint32_t) qapi_loc_util_string_to_double(dwellStr);

    LOC_LOG_QCLI("Parsed arguments:\n"
            "Latitude: %d.%d Longitude: %d.%d Radius: %d.%d metres\n"
            "BreachTypeMask: 0x%x Responsiveness: %d ms Dwell Time %d sec",
            (int)gfData.latitude, (abs((int)(gfData.latitude*MAX_FRAC_DIGITS)))%MAX_FRAC_DIGITS,
            (int)gfData.longitude, (abs((int)(gfData.longitude*MAX_FRAC_DIGITS)))%MAX_FRAC_DIGITS,
            (int)gfData.radius, (abs((int)(gfData.radius*MAX_FRAC_DIGITS)))%MAX_FRAC_DIGITS,
            gfOptions.breachTypeMask, gfOptions.responsiveness, gfOptions.dwellTime);

    memcpy(&gfDataArray[gfCount], &gfData, sizeof(gfData));
    memcpy(&gfOptionsArray[gfCount], &gfOptions, sizeof(gfOptions));
    gfCount++;

    LOC_LOG_QCLI("Added Geofence to Buffer, use 'locaddgf' to pass buffer to Loc API");
    gfBufferForAddGF = true;
    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_add_to_gf_modify_buf(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    // Let's create Geofence Options
    uint32_t gfId = 0;
    qapi_Geofence_Option_t gfOptions = { 0 };
    char* idStr;
    char* maskStr;
    char* resStr;
    char* dwellStr;

    if(gfCount >= GF_ARRAY_SIZE){
        LOC_LOG_QCLI("No space to store more GF options");
        return QCLI_STATUS_SUCCESS_E;
    }

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    LOC_LOG_QCLI("Parsing arguments..");

    // GF ID
    idStr = LQPBgetParamNextTo(&buffer, "id");
    if(idStr == NULL){
        LOC_LOG_QCLI("id arg missing !");
        return QCLI_STATUS_USAGE_E;
    }

    // Breach Type Mask
    maskStr = LQPBgetParamNextTo(&buffer, "mask");
    if(maskStr == NULL){
        LOC_LOG_QCLI("mask arg missing !");
        return QCLI_STATUS_USAGE_E;
    }

    // Responsiveness
    resStr = LQPBgetParamNextTo(&buffer, "res");
    if(resStr == NULL){
        LOC_LOG_QCLI("res arg missing !");
        return QCLI_STATUS_USAGE_E;
    }

    // Dwell Time
    dwellStr = LQPBgetParamNextTo(&buffer, "dwell");
    if(dwellStr == NULL){
        LOC_LOG_QCLI("dwell arg missing !");
        return QCLI_STATUS_USAGE_E;
    }

    // Now print
    LOC_LOG_QCLI("Geofence Options:\n"
            "ID %s, Mask %s, Responsiveness %s, Dwell Time %s",
            idStr, maskStr, resStr, dwellStr);

    // Now parse and store id and geofence options
    gfId = (uint32_t)qapi_loc_util_string_to_double(idStr);
    gfOptions.breachTypeMask = (uint8_t)qapi_loc_util_string_to_double(maskStr);
    gfOptions.responsiveness = (uint32_t)qapi_loc_util_string_to_double(resStr);
    gfOptions.dwellTime = (uint32_t)qapi_loc_util_string_to_double(dwellStr);

    LOC_LOG_QCLI("Parsed Arguments:\n"
            "ID %d, BreachTypeMask: 0x%x Responsiveness: %d ms Dwell Time %d sec",
            gfId, gfOptions.breachTypeMask, gfOptions.responsiveness, gfOptions.dwellTime);

    gfIdArray[gfCount] = gfId;
    memcpy(&gfOptionsArray[gfCount], &gfOptions, sizeof(gfOptions));
    gfCount++;

    LOC_LOG_QCLI("Added Geofence ID and Options to buf, use 'locmodifygf' to modify GF with new options");
    gfBufferForModifyGF = true;
    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_reset_gf_buf(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    LOC_LOG_QCLI("Clearing all GF Buffers !");

    memset(gfIdArray, 0, sizeof(gfIdArray));
    memset(gfOptionsArray, 0, sizeof(gfOptionsArray));
    memset(gfDataArray, 0, sizeof(gfDataArray));
    gfCount = 0;
    gfBufferForAddGF = false;
    gfBufferForModifyGF = false;

    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t LocTest_print_gf_buf(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    int i;

    if(gfCount == 0){
        LOC_LOG_QCLI("No GF in buffer !");
        return QCLI_STATUS_SUCCESS_E;
    }

    LOC_LOG_QCLI("Below geofences will be used for locaddgf cmd");

    for(i=0; i<gfCount; i++){

        LOC_LOG_QCLI("GF[%d]: Latitude: %d.%d Longitude: %d.%d Radius: %d.%d metres "
                "BreachTypeMask: 0x%x Responsiveness: %d ms Dwell Time %d sec \n", i,
                (int)gfDataArray[i].latitude, (abs((int)(gfDataArray[i].latitude*MAX_FRAC_DIGITS)))%MAX_FRAC_DIGITS,
                (int)gfDataArray[i].longitude, (abs((int)(gfDataArray[i].longitude*MAX_FRAC_DIGITS)))%MAX_FRAC_DIGITS,
                (int)gfDataArray[i].radius, (abs((int)(gfDataArray[i].radius*MAX_FRAC_DIGITS)))%MAX_FRAC_DIGITS,
                gfOptionsArray[i].breachTypeMask, gfOptionsArray[i].responsiveness, gfOptionsArray[i].dwellTime);
        LOC_LOG_QCLI("---");
    }

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_print_gf_modify_buf(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    int i;

    if(gfCount == 0){
        LOC_LOG_QCLI("No GF in buffer !");
        return QCLI_STATUS_SUCCESS_E;
    }

    LOC_LOG_QCLI("Below geofence options will be used for locmodifygf cmd");

    for(i=0; i<gfCount; i++){

        LOC_LOG_QCLI("GF[%d]: ID %d, "
                "BreachTypeMask: 0x%x Responsiveness: %d ms Dwell Time %d sec ", i, gfIdArray[i],
                gfOptionsArray[i].breachTypeMask, gfOptionsArray[i].responsiveness, gfOptionsArray[i].dwellTime);
        LOC_LOG_QCLI("---");
    }

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_add_geofences(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    uint32_t* idArray;

    if(gfCount == 0 || !gfBufferForAddGF){
        LOC_LOG_QCLI("No Geofence added to Add GF buffer");
        return QCLI_STATUS_SUCCESS_E;
    }

    LOC_LOG_QCLI("There are %d GF in buffer, invoking Loc API "
            "(will block until modem responds)..", gfCount);

    idArray = location_add_geofences(sActiveClientIdx, gfCount, gfOptionsArray, gfDataArray);
    if(idArray == NULL){
        LOC_LOG_QCLI("NULL ID Array returned, Failure !");
        return QCLI_STATUS_SUCCESS_E;
    }

    LOC_LOG_QCLI("Added Geofences successfully.");

    LOC_LOG_QCLI("Clearing all GF Buffers !");
    memset(gfIdArray, 0, sizeof(gfIdArray));
    memset(gfOptionsArray, 0, sizeof(gfOptionsArray));
    memset(gfDataArray, 0, sizeof(gfDataArray));
    gfCount = 0;
    gfBufferForAddGF = false;
    gfBufferForModifyGF = false;

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_modify_geofences(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    int res;
    LocQCLIParamBuffer buffer;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    if(gfCount == 0 || !gfBufferForModifyGF){
        LOC_LOG_QCLI("No Geofence added to Modify GF buffer");
        return QCLI_STATUS_SUCCESS_E;
    }

    LOC_LOG_QCLI("There are %d GF in buffer, invoking Loc API "
            "(will block until modem responds)..", gfCount);

    res = location_modify_geofences(sActiveClientIdx, gfCount, gfIdArray, gfOptionsArray);
    if(res != 0){
        LOC_LOG_QCLI("Failed to modify geofences !");
        return QCLI_STATUS_ERROR_E;
    }

    LOC_LOG_QCLI("Modify Geofences successful.");

    LOC_LOG_QCLI("Now clearing all GF Buffers !");
    memset(gfIdArray, 0, sizeof(gfIdArray));
    memset(gfOptionsArray, 0, sizeof(gfOptionsArray));
    memset(gfDataArray, 0, sizeof(gfDataArray));
    gfCount = 0;
    gfBufferForAddGF = false;
    gfBufferForModifyGF = false;

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_remove_geofences(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    uint32_t ids[GF_ARRAY_SIZE];
    uint32_t idx = 0;
    int res;

    if(Parameter_Count == 0){
        LOC_LOG_QCLI("Enter at least one ID to remove.");
        return QCLI_STATUS_USAGE_E;
    }
    if(Parameter_Count >= GF_ARRAY_SIZE){
        LOC_LOG_QCLI("Only up to %d IDs supported.", GF_ARRAY_SIZE);
        return QCLI_STATUS_SUCCESS_E;
    }

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    while(LQPBparamsLeftToFetch(&buffer)){
        res = LQPBgetNextParamAsInt(&buffer);
        if (res < 0) {
            LOC_LOG_QCLI("Invalid args.");
            return QCLI_STATUS_USAGE_E;
        }
        ids[idx] = res;
        idx++;
    }

    res = location_remove_geofences(sActiveClientIdx, Parameter_Count, ids);
    if(res == 0){
        LOC_LOG_QCLI("Invoked location_remove_geofences() with specified IDs");
    }
    else{
        LOC_LOG_QCLI("location_remove_geofences() returned failure");
    }

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_pause_geofences(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    uint32_t ids[GF_ARRAY_SIZE];
    uint32_t idx = 0;
    int res;

    if(Parameter_Count == 0){
        LOC_LOG_QCLI("Enter at least one ID to pause.");
        return QCLI_STATUS_USAGE_E;
    }
    if(Parameter_Count >= GF_ARRAY_SIZE){
        LOC_LOG_QCLI("Only up to %d IDs supported.", GF_ARRAY_SIZE);
        return QCLI_STATUS_SUCCESS_E;
    }

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    while(LQPBparamsLeftToFetch(&buffer)){
        res = LQPBgetNextParamAsInt(&buffer);
        if (res < 0) {
            LOC_LOG_QCLI("Invalid args.");
            return QCLI_STATUS_USAGE_E;
        }
        ids[idx] = res;
        idx++;
    }

    res = location_pause_geofences(sActiveClientIdx, Parameter_Count, ids);
    if(res == 0){
        LOC_LOG_QCLI("Invoked location_pause_geofences() with specified IDs");
    }
    else{
        LOC_LOG_QCLI("location_pause_geofences() returned failure");
    }

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_resume_geofences(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    uint32_t ids[GF_ARRAY_SIZE];
    uint32_t idx = 0;
    int res;

    if(Parameter_Count == 0){
        LOC_LOG_QCLI("Enter at least one ID to resume.");
        return QCLI_STATUS_USAGE_E;
    }
    if(Parameter_Count >= GF_ARRAY_SIZE){
        LOC_LOG_QCLI("Only up to %d IDs supported.", GF_ARRAY_SIZE);
        return QCLI_STATUS_SUCCESS_E;
    }

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    while(LQPBparamsLeftToFetch(&buffer)){
        res = LQPBgetNextParamAsInt(&buffer);
        if (res < 0) {
            LOC_LOG_QCLI("Invalid args.");
            return QCLI_STATUS_USAGE_E;
        }
        ids[idx] = res;
        idx++;
    }

    res = location_resume_geofences(sActiveClientIdx, Parameter_Count, ids);
    if(res == 0){
        LOC_LOG_QCLI("Invoked location_resume_geofences() with specified IDs");
    }
    else{
        LOC_LOG_QCLI("location_resume_geofences() returned failure");
    }

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_start_batching(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    char* tbfStr;
    char* dbtStr;
    uint32_t minInterval;
    uint32_t minDistance;
    uint32_t id;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    // Time between fixes : minInterval
    tbfStr = LQPBgetParamNextTo(&buffer, "tbf");
    if(tbfStr == NULL){
        LOC_LOG_QCLI("tbf arg missing, specify tbf 0 if you want only distance based tracking");
        return QCLI_STATUS_USAGE_E;
    }

    // Distance based tracking
    dbtStr = LQPBgetParamNextTo(&buffer, "dbt");
    if(dbtStr == NULL){
        LOC_LOG_QCLI("dbt arg missing, specify dbt 0 if you want time based tracking");
        return QCLI_STATUS_USAGE_E;
    }

    // Now parse the values
    minInterval = (uint32_t)qapi_loc_util_string_to_double(tbfStr);
    minDistance = (uint32_t)qapi_loc_util_string_to_double(dbtStr);

    // Invoke loc API
    id = location_start_batching(sActiveClientIdx, minInterval, minDistance);
    if(id > 1000){
        LOC_LOG_QCLI("location_start_batching() returned suspicious ID (%u), "
                "check logs for failure !", id);
    }
    else{
        LOC_LOG_QCLI("Batching started, ID = %u", id);
    }
    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_stop_batching(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    char* idStr;
    uint32_t id;
    int res;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    // Time between fixes : minInterval
    idStr = LQPBgetParamNextTo(&buffer, "id");
    if(idStr == NULL){
        LOC_LOG_QCLI("id arg missing");
        return QCLI_STATUS_USAGE_E;
    }

    // Now parse the values
    id = (uint32_t)qapi_loc_util_string_to_double(idStr);

    // Invoke loc API
    res = location_stop_batching(sActiveClientIdx, id);
    if(res == 0){
        LOC_LOG_QCLI("location_stop_batching() returned success");
    }
    else{
        LOC_LOG_QCLI("location_stop_batching() returned failure !");
    }

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_get_batched_locations(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    char* idStr;
    char* countStr;
    uint32_t id; 
    uint32_t count;
    int res;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    // ID for the batching session
    idStr = LQPBgetParamNextTo(&buffer, "id");
    if(idStr == NULL){
        LOC_LOG_QCLI("id arg missing");
        return QCLI_STATUS_USAGE_E;
    }

    // Distance based tracking
    countStr = LQPBgetParamNextTo(&buffer, "count");
    if(countStr == NULL){
        LOC_LOG_QCLI("count arg missing, number of batched loc to fetch ?");
        return QCLI_STATUS_USAGE_E;
    }

    // Now parse the values
    id = (uint32_t)qapi_loc_util_string_to_double(idStr);
    count = (uint32_t)qapi_loc_util_string_to_double(countStr);

    // Invoke loc API
    res = location_get_batched_locations(sActiveClientIdx, id, count);
    if(res != 0){
        LOC_LOG_QCLI("Get batched location failure !");
    }
    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_update_batching_options(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    char* idStr;
    char* tbfStr;
    char* dbtStr;
    uint32_t id;
    uint32_t minInterval;
    uint32_t minDistance;
    int res;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    // ID for the batching session
    idStr = LQPBgetParamNextTo(&buffer, "id");
    if(idStr == NULL){
        LOC_LOG_QCLI("id arg missing");
        return QCLI_STATUS_USAGE_E;
    }

    // Time between fixes : minInterval
    tbfStr = LQPBgetParamNextTo(&buffer, "tbf");
    if(tbfStr == NULL){
        LOC_LOG_QCLI("tbf arg missing, specify tbf 0 if you want only distance based tracking");
        return QCLI_STATUS_USAGE_E;
    }

    // Distance based tracking
    dbtStr = LQPBgetParamNextTo(&buffer, "dbt");
    if(dbtStr == NULL){
        LOC_LOG_QCLI("dbt arg missing, specify dbt 0 if you want time based tracking");
        return QCLI_STATUS_USAGE_E;
    }

    // Now parse the values
    id = (uint32_t)qapi_loc_util_string_to_double(idStr);
    minInterval = (uint32_t)qapi_loc_util_string_to_double(tbfStr);
    minDistance = (uint32_t)qapi_loc_util_string_to_double(dbtStr);

    // Invoke loc API
    res = location_update_batching_options(sActiveClientIdx, id, minInterval, minDistance);
    if(res == 0){
        LOC_LOG_QCLI("location_update_batching_options() returned success");
    }
    else{
        LOC_LOG_QCLI("location_update_batching_options() returned failure");
    }

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_start_tracking(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    char* tbfStr;
    char* dbtStr;
    uint32_t minInterval;
    uint32_t minDistance;
    uint32_t id;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    // Time between fixes : minInterval
    tbfStr = LQPBgetParamNextTo(&buffer, "tbf");
    if(tbfStr == NULL){
        LOC_LOG_QCLI("tbf arg missing, specify tbf 0 if you want only distance based tracking");
        return QCLI_STATUS_USAGE_E;
    }

    // Distance based tracking
    dbtStr = LQPBgetParamNextTo(&buffer, "dbt");
    if(dbtStr == NULL){
        LOC_LOG_QCLI("dbt arg missing, specify dbt 0 if you want time based tracking");
        return QCLI_STATUS_USAGE_E;
    }

    // Now parse the values
    minInterval = (uint32_t)qapi_loc_util_string_to_double(tbfStr);
    minDistance = (uint32_t)qapi_loc_util_string_to_double(dbtStr);

    // Invoke loc API
    id = location_start_tracking(sActiveClientIdx, minInterval, minDistance);
    if(id > 1000){
        LOC_LOG_QCLI("location_start_tracking() returned suspicious ID (%u), "
                "check logs for failure !", id);
    }
    else{
        LOC_LOG_QCLI("Tracking started, ID = %u", id);
    }
    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_stop_tracking(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    char* idStr;
    uint32_t id;
    int res;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    // Time between fixes : minInterval
    idStr = LQPBgetParamNextTo(&buffer, "id");
    if(idStr == NULL){
        LOC_LOG_QCLI("id arg missing");
        return QCLI_STATUS_USAGE_E;
    }

    // Now parse the values
    id = (uint32_t)qapi_loc_util_string_to_double(idStr);

    // Invoke loc API
    res = location_stop_tracking(sActiveClientIdx, id);
    if(res == 0){
        LOC_LOG_QCLI("location_stop_tracking() returned success");
    }
    else{
        LOC_LOG_QCLI("location_stop_tracking() returned failure !");
    }

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_update_tracking_options(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    char* idStr;
    char* tbfStr;
    char* dbtStr;
    uint32_t id;
    uint32_t minInterval;
    uint32_t minDistance;
    int res;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    // ID for the batching session
    idStr = LQPBgetParamNextTo(&buffer, "id");
    if(idStr == NULL){
        LOC_LOG_QCLI("id arg missing");
        return QCLI_STATUS_USAGE_E;
    }

    // Time between fixes : minInterval
    tbfStr = LQPBgetParamNextTo(&buffer, "tbf");
    if(tbfStr == NULL){
        LOC_LOG_QCLI("tbf arg missing, specify tbf 0 if you want only distance based tracking");
        return QCLI_STATUS_USAGE_E;
    }

    // Distance based tracking
    dbtStr = LQPBgetParamNextTo(&buffer, "dbt");
    if(dbtStr == NULL){
        LOC_LOG_QCLI("dbt arg missing, specify dbt 0 if you want time based tracking");
        return QCLI_STATUS_USAGE_E;
    }

    // Now parse the values
    id = (uint32_t)qapi_loc_util_string_to_double(idStr);
    minInterval = (uint32_t)qapi_loc_util_string_to_double(tbfStr);
    minDistance = (uint32_t)qapi_loc_util_string_to_double(dbtStr);

    // Invoke loc API
    res = location_update_tracking_options(sActiveClientIdx, id, minInterval, minDistance);
    if(res == 0){
        LOC_LOG_QCLI("location_update_tracking_options() returned success");
    }
    else{
        LOC_LOG_QCLI("location_update_tracking_options() returned failure");
    }

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_batching_ss(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    char* tbfStr;
    char* dbtStr;
    uint32_t minInterval;
    uint32_t minDistance;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    // Time between fixes : minInterval
    tbfStr = LQPBgetParamNextTo(&buffer, "tbf");
    if(tbfStr == NULL){
        LOC_LOG_QCLI("tbf arg missing, specify tbf 0 if you want only distance based tracking");
        return QCLI_STATUS_USAGE_E;
    }

    // Distance based tracking
    dbtStr = LQPBgetParamNextTo(&buffer, "dbt");
    if(dbtStr == NULL){
        LOC_LOG_QCLI("dbt arg missing, specify dbt 0 if you want time based tracking");
        return QCLI_STATUS_USAGE_E;
    }

    // Now parse the values
    minInterval = (uint32_t)qapi_loc_util_string_to_double(tbfStr);
    minDistance = (uint32_t)qapi_loc_util_string_to_double(dbtStr);

    // Invoke loc API
    gps_verify_batching_test_singleshot(sActiveClientIdx, minInterval, minDistance);
    LOC_LOG_QCLI("gps_verify_batching_test_singleshot() invoked");

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_tracking_ss(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    char* tbfStr;
    char* dbtStr;
    uint32_t minInterval;
    uint32_t minDistance;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    // Time between fixes : minInterval
    tbfStr = LQPBgetParamNextTo(&buffer, "tbf");
    if(tbfStr == NULL){
        LOC_LOG_QCLI("tbf arg missing, specify tbf 0 if you want only distance based tracking");
        return QCLI_STATUS_USAGE_E;
    }

    // Distance based tracking
    dbtStr = LQPBgetParamNextTo(&buffer, "dbt");
    if(dbtStr == NULL){
        LOC_LOG_QCLI("dbt arg missing, specify dbt 0 if you want time based tracking");
        return QCLI_STATUS_USAGE_E;
    }

    // Now parse the values
    minInterval = (uint32_t)qapi_loc_util_string_to_double(tbfStr);
    minDistance = (uint32_t)qapi_loc_util_string_to_double(dbtStr);

    // Invoke loc API
    gps_verify_tracking_test_singleshot(sActiveClientIdx, minInterval, minDistance);
    LOC_LOG_QCLI("gps_verify_tracking_test_singleshot() invoked");

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_get_single_shot(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    char* lplStr;
    uint32_t powerLevel;
    uint32_t id;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    // Location Power Level
    lplStr = LQPBgetParamNextTo(&buffer, "lpl");
    if (lplStr == NULL) {
        LOC_LOG_QCLI("lpl arg missing");
        return QCLI_STATUS_USAGE_E;
    }

    // Now parse the values
    powerLevel = (uint32_t)qapi_loc_util_string_to_double(lplStr);

    // Invoke loc API
    id = location_get_single_shot(sActiveClientIdx, powerLevel);
    if (id > 1000) {
        LOC_LOG_QCLI("location_get_single_shot() returned suspicious ID (%u), "
            "check logs for failure !", id);
    }
    else {
        LOC_LOG_QCLI("Single shot session started, ID = %u", id);
    }

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_cancel_single_shot(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    char* idStr;
    uint32_t id;
    int res;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    // Id
    idStr = LQPBgetParamNextTo(&buffer, "id");
    if (idStr == NULL) {
        LOC_LOG_QCLI("id arg missing");
        return QCLI_STATUS_USAGE_E;
    }

    // Now parse the values
    id = (uint32_t)qapi_loc_util_string_to_double(idStr);

    // Invoke loc API
    res = location_cancel_single_shot(sActiveClientIdx, id);
    if (res == 0) {
        LOC_LOG_QCLI("location_cancel_single_shot() returned success");
    }
    else {
        LOC_LOG_QCLI("location_cancel_single_shot() returned failure !");
    }

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_start_get_gnss_data(
    uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    uint32_t id;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    // Invoke loc API
    id = location_start_get_gnss_data(sActiveClientIdx);
    if (id > 1000) {
        LOC_LOG_QCLI("location_start_get_gnss_data() returned suspicious ID (%u), "
            "check logs for failure !", id);
    }
    else {
        LOC_LOG_QCLI("Get GNSS Data started, ID = %u", id);
    }
    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_location_stop_get_gnss_data(
    uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    LocQCLIParamBuffer buffer;
    char* idStr;
    uint32_t id;
    int res;

    LQPBinit(&buffer, Parameter_List, Parameter_Count);

    idStr = LQPBgetParamNextTo(&buffer, "id");
    if (idStr == NULL) {
        LOC_LOG_QCLI("id arg missing");
        return QCLI_STATUS_USAGE_E;
    }

    // Now parse the values
    id = (uint32_t)qapi_loc_util_string_to_double(idStr);

    // Invoke loc API
    res = location_stop_get_gnss_data(sActiveClientIdx, id);
    if (res == 0) {
        LOC_LOG_QCLI("location_stop_get_gnss_data() returned success");
    }
    else {
        LOC_LOG_QCLI("location_stop_get_gnss_data() returned failure !");
    }

    return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t LocTest_print_cmd_info(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

// List of commands registered with QCLI
const QCLI_Command_t loc_cmd_list[] =
{
    /* Command_Function, Start_Thread, Command_String, Usage_String, Description, Abort_function */
    {LocTest_enableVerboseLogging, false, "enableverboselogging", "enableverboselogging", "Enable QCLI Verbose Logging", NULL},
    {LocTest_disableVerboseLogging, false, "disableverboselogging", "disableverboselogging", "Disable QCLI Verbose Logging", NULL},

    {LocTest_location_init, false, "locinit", "client <client_number: in [0,3]> cbmask <5 bit mask value in [1,31] 1:TRK 2:BAT 4:GF 8:SS 16:GD>",
            "Location API Init for specified client", NULL},
    {LocTest_location_deinit, false, "locdeinit", "client <client_number: in [0,3]>", "Location API Deinit for specified client", NULL},

    {LocTest_set_active_client_idx, false, "setactiveclientidx", "client <client_number: in [0,3]>", "Set Active Client Index", NULL},

    {LocTest_add_to_gf_buf, false, "addtogfbuf",
            "lat <latitude> long <longitude> rad <radius_metres> "
            "mask <breachMask> res <responsiveness_ms> dwell <dwellTime_sec>",
            "Save Geofence to buffer, used in locaddgf", NULL},

    {LocTest_add_to_gf_modify_buf, false, "addtogfmodifybuf",
            "id <GF id to modify> mask <breachMask> res <responsiveness_ms> dwell <dwellTime_sec>",
            "Save Geofence Options to modify buffer, used in locmodifygf", NULL},

    {LocTest_reset_gf_buf, false, "resetgfbuf", "resetgfbuf",
            "Remove all values added to Geofence buffers via addtogfbuf or addtogfmodifybuf", NULL},

    {LocTest_print_gf_buf, false, "printgfbuf", "printgfbuf",
            "Print all values added to Geofence buffer to be used for addtogfbuf", NULL},

    {LocTest_print_gf_modify_buf, false, "printgfmodifybuf", "printgfmodifybuf",
            "Print all values added to Geofence buffer to be used for addtogfmodifybuf", NULL},

    {LocTest_location_add_geofences, false, "locaddgf", "locaddgf",
            "Invoke Loc API to add GF in buffer", NULL},

    {LocTest_location_modify_geofences, false, "locmodifygf", "locaddgf",
            "Invoke Loc API to modify GF with GF Options in buffer", NULL},

    {LocTest_location_remove_geofences, false, "locremovegf", "<id1> <id2> <id3> ...",
            "Invoke Loc API to remove Geofences with specified IDs", NULL},

    {LocTest_location_pause_geofences, false, "locpausegf", "<id1> <id2> <id3> ...",
            "Invoke Loc API to pause Geofences with specified IDs", NULL},

    {LocTest_location_resume_geofences, false, "locresumegf", "<id1> <id2> <id3> ...",
            "Invoke Loc API to resume Geofences with specified IDs", NULL},

    {LocTest_location_start_batching, false, "locstartbatching", "tbf <minInterval_ms> dbt <minDistance_metres>",
            "Invoke Loc API to start batching", NULL},

    {LocTest_location_stop_batching, false, "locstopbatching", "id <id>",
            "Invoke Loc API to stop batching for specified ID", NULL},

    {LocTest_location_get_batched_locations, false, "locgetbatched", "id <id> count <number of batched loc to get>",
            "Invoke Loc API to get specified number of batched locations", NULL},

    {LocTest_location_update_batching_options, false, "locupdatebatching", "id <id> tbf <minInterval_ms> dbt <minDistance_metres>",
            "Invoke Loc API to update batching options", NULL},

    {LocTest_location_start_tracking, false, "locstarttracking", "tbf <minInterval_ms> dbt <minDistance_metres>",
            "Invoke Loc API to start tracking", NULL},

    {LocTest_location_stop_tracking, false, "locstoptracking", "id <id>",
            "Invoke Loc API to stop tracking for specified ID", NULL},

    {LocTest_location_update_tracking_options, false, "locupdatetracking", "id <id> tbf <minInterval_ms> dbt <minDistance_metres>",
            "Invoke Loc API to update tracking options", NULL},

    {LocTest_location_batching_ss, false, "locbatchingss", "tbf <minInterval_ms> dbt <minDistance_metres>",
            "Simulate Single Shot Batching session", NULL},

    {LocTest_location_tracking_ss, false, "loctrackingss", "tbf <minInterval_ms> dbt <minDistance_metres>",
            "Simulate Single Shot Tracking session", NULL},

    {LocTest_location_get_single_shot, false, "locgetsingleshot", "lpl <powerLevel> HIGH=0, LOW=1",
            "Invoke Loc API to get single shot", NULL},

    {LocTest_location_cancel_single_shot, false, "loccancelsingleshot", "id <id>",
            "Invoke Loc API to cancel single shot", NULL},

    {LocTest_location_start_get_gnss_data, false, "locstartgetgnssdata",
            "Invoke Loc API to start get GNSS Data", NULL},

    {LocTest_location_stop_get_gnss_data, false, "locstopgetgnssdata", "id <id>",
            "Invoke Loc API to stop get GNSS Data for specified ID", NULL},

    {LocTest_print_cmd_info, false, "lochelp", "lochelp",
            "Print info about all location commands", NULL},
};

static QCLI_Command_Status_t LocTest_print_cmd_info(
        uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    int i;
    uint32_t locCmdListSize;
    QCLI_Command_t* cmdPtr;

    // Browse through loc_cmd_list[]
    locCmdListSize = sizeof(loc_cmd_list)/sizeof(loc_cmd_list[0]);
    LOC_LOG_QCLI("Loc Cmd List size: %u\n", locCmdListSize);

    cmdPtr = NULL;
    for(i=0; i<locCmdListSize; i++){

        cmdPtr = (QCLI_Command_t*)&loc_cmd_list[i];
        LOC_LOG_QCLI("Command: %s", cmdPtr->Command_String);
        LOC_LOG_QCLI("Usage: %s", cmdPtr->Usage_String);
        LOC_LOG_QCLI("Description: %s\n", cmdPtr->Description);
    }

    return QCLI_STATUS_SUCCESS_E;
}

// Parent group for all Location Commands
const QCLI_Command_Group_t loc_cmd_group =
{
    "LocTest",              /* Group_String: will display cmd prompt as "LocTest> " */
    sizeof(loc_cmd_list)/sizeof(loc_cmd_list[0]),   /* Command_Count */
    loc_cmd_list        /* Command_List */
};

// Init function, register with QCLI module
void loc_qcli_iface_init()
{
    /* Attempt to reqister the Command Groups with the qcli framework.*/
    qcli_loc_handle = QCLI_Register_Command_Group(NULL, &loc_cmd_group);
    if (qcli_loc_handle)
    {
        LOC_LOG_QCLI("LocTest Registered !");
    }
}

/*==========================================================================
LOCATION API REGISTERED CALLBACKS
===========================================================================*/
/* CAPABILITIES CALLBACKs */
static void location_capabilities_callback(
        qapi_Location_Capabilities_Mask_t capabilitiesMask)
{
    LOC_LOG_QCLI("LOC API CALLBACK : CAPABILITIES (mask 0x%x)", capabilitiesMask);
}
static void location_capabilities_callback_clnt_0(
        qapi_Location_Capabilities_Mask_t capabilitiesMask) {
    LOC_LOG_QCLI("\nlocation_capabilities_callback_clnt_0");
    location_capabilities_callback(capabilitiesMask);
}
static void location_capabilities_callback_clnt_1(
        qapi_Location_Capabilities_Mask_t capabilitiesMask) {
    LOC_LOG_QCLI("\nlocation_capabilities_callback_clnt_1");
    location_capabilities_callback(capabilitiesMask);
}
static void location_capabilities_callback_clnt_2(
        qapi_Location_Capabilities_Mask_t capabilitiesMask) {
    LOC_LOG_QCLI("\nlocation_capabilities_callback_clnt_2");
    location_capabilities_callback(capabilitiesMask);
}
static void location_capabilities_callback_clnt_3(
        qapi_Location_Capabilities_Mask_t capabilitiesMask) {
    LOC_LOG_QCLI("\nlocation_capabilities_callback_clnt_3");
    location_capabilities_callback(capabilitiesMask);
}

/* RESPONSE CALLBACKs */
static void location_response_callback(
        qapi_Location_Error_t err, uint32_t id) {
    LOC_LOG_QCLI("LOC API CALLBACK : RESPONSE (err=%u id=%u)", err, id);
    tx_event_flags_set(&gSignalResponse, 1, TX_OR);
}
static void location_response_callback_clnt_0(
        qapi_Location_Error_t err, uint32_t id) {
    LOC_LOG_QCLI("\nlocation_response_callback_clnt_0");
    location_response_callback(err, id);
}
static void location_response_callback_clnt_1(
        qapi_Location_Error_t err, uint32_t id) {
    LOC_LOG_QCLI("\nlocation_response_callback_clnt_1");
    location_response_callback(err, id);
}
static void location_response_callback_clnt_2(
        qapi_Location_Error_t err, uint32_t id) {
    LOC_LOG_QCLI("\nlocation_response_callback_clnt_2");
    location_response_callback(err, id);
}
static void location_response_callback_clnt_3(
        qapi_Location_Error_t err, uint32_t id) {
    LOC_LOG_QCLI("\nlocation_response_callback_clnt_3");
    location_response_callback(err, id);
}

/* COLLECTIVE RESPONSE CALLBACKs */
static void location_collective_response_callback(size_t count,
    qapi_Location_Error_t* err,
    uint32_t* ids)
{
    uint32_t i = 0;
    LOC_LOG_QCLI("LOC API CALLBACK : COLLECTIVE RESPONSE (count %d)", count);

    for (i = 0; i < count; ++i) {
        LOC_LOG_QCLI("ID[%d] = %d, err[%d] = %d", i, ids[i], i, err[i]);
    }

    tx_event_flags_set(&gSignalGeofenceResponse, 1, TX_OR);
}
static void location_collective_response_callback_clnt_0(
        size_t count, qapi_Location_Error_t* err, uint32_t* ids) {
    LOC_LOG_QCLI("\nlocation_collective_response_callback_clnt_0");
    location_collective_response_callback(count, err, ids);
}
static void location_collective_response_callback_clnt_1(
        size_t count, qapi_Location_Error_t* err, uint32_t* ids) {
    LOC_LOG_QCLI("\nlocation_collective_response_callback_clnt_1");
    location_collective_response_callback(count, err, ids);
}
static void location_collective_response_callback_clnt_2(
        size_t count, qapi_Location_Error_t* err, uint32_t* ids) {
    LOC_LOG_QCLI("\nlocation_collective_response_callback_clnt_2");
    location_collective_response_callback(count, err, ids);
}
static void location_collective_response_callback_clnt_3(
        size_t count, qapi_Location_Error_t* err, uint32_t* ids) {
    LOC_LOG_QCLI("\nlocation_collective_response_callback_clnt_3");
    location_collective_response_callback(count, err, ids);
}

/* TRACKING CALLBACKs */
static void location_tracking_callback(qapi_Location_t location)
{
    uint64_t ts_sec = location.timestamp/1000;

    LOC_LOG_QCLI_V("LOC API CALLBACK : TRACKING");
    LOC_LOG_QCLI_V("Time(HH:MM:SS): %02d:%02d:%02d",
            (int)((ts_sec/3600)%24), (int)((ts_sec/60)%60), (int)(ts_sec%60));

    LOC_LOG_QCLI_V("LAT: %d.%d LON: %d.%d ACC: %d.%d",
        (int)location.latitude, (abs((int)(location.latitude * 100000))) % 100000,
        (int)location.longitude, (abs((int)(location.longitude * 100000))) % 100000,
        (int)location.accuracy, (abs((int)(location.accuracy * 100))) % 100);

    LOC_LOG_QCLI_V("ALT: %d.%d ALT_MSL: %d.%d",
        (int)location.altitude, (abs((int)(location.altitude * 100))) % 100,
        (int)location.altitudeMeanSeaLevel, (abs((int)(location.altitudeMeanSeaLevel * 100))) % 100);

    LOC_LOG_QCLI_V("SPEED: %d.%d BEAR: %d.%d TIME: 0x%x%x FLAGS: %u",
        (int)location.speed, (abs((int)(location.speed * 100))) % 100,
        (int)location.bearing, (abs((int)(location.bearing * 100))) % 100,
        (int)(location.timestamp >> 32), (int)location.timestamp, location.flags);

    if (gGpsVerifyTestInProgress) {
        tx_event_flags_set(&gSignalLocation, 1, TX_OR);
    }
}
static void location_tracking_callback_clnt_0(qapi_Location_t location) {
    LOC_LOG_QCLI_V("\nlocation_tracking_callback_clnt_0");
    location_tracking_callback(location);
}
static void location_tracking_callback_clnt_1(qapi_Location_t location) {
    LOC_LOG_QCLI_V("\nlocation_tracking_callback_clnt_1");
    location_tracking_callback(location);
}
static void location_tracking_callback_clnt_2(qapi_Location_t location) {
    LOC_LOG_QCLI_V("\nlocation_tracking_callback_clnt_2");
    location_tracking_callback(location);
}
static void location_tracking_callback_clnt_3(qapi_Location_t location) {
    LOC_LOG_QCLI_V("\nlocation_tracking_callback_clnt_3");
    location_tracking_callback(location);
}

/* BATCHING CALLBACKs */
static void location_batching_callback(
        size_t count, qapi_Location_t* location) {
    uint32_t i = 0;
    uint64_t ts_sec = 0;

    LOC_LOG_QCLI_V("LOC API CALLBACK : BATCHING (count %d)", count);

    for (i = 0; i < count; ++i) {
        ts_sec = location[i].timestamp/1000;
        LOC_LOG_QCLI_V("Time(HH:MM:SS): %02d:%02d:%02d",
                (int)((ts_sec/3600)%24), (int)((ts_sec/60)%60), (int)(ts_sec%60));

        LOC_LOG_QCLI_V("location[%d]:", i);

        LOC_LOG_QCLI_V("LAT: %d.%d LON: %d.%d ACC: %d.%d",
            (int)location[i].latitude, (abs((int)(location[i].latitude * 100000))) % 100000,
            (int)location[i].longitude, (abs((int)(location[i].longitude * 100000))) % 100000,
            (int)location[i].accuracy, (abs((int)(location[i].accuracy * 100))) % 100);

        LOC_LOG_QCLI_V("ALT: %d.%d ALT_MSL: %d.%d",
            (int)location[i].altitude, (abs((int)(location[i].altitude * 100))) % 100,
            (int)location[i].altitudeMeanSeaLevel, (abs((int)(location[i].altitudeMeanSeaLevel * 100))) % 100);

        LOC_LOG_QCLI_V("SPEED: %d.%d BEAR: %d.%d TIME: 0x%x%x FLAGS: %u",
            (int)location[i].speed, (abs((int)(location[i].speed * 100))) % 100,
            (int)location[i].bearing, (abs((int)(location[i].bearing * 100))) % 100,
            (int)(location[i].timestamp >> 32), (int)location[i].timestamp, location[i].flags);
    }

    if (gGpsVerifyTestInProgress) {
        tx_event_flags_set(&gSignalLocation, 1, TX_OR);
    }
}
static void location_batching_callback_clnt_0(
        size_t count, qapi_Location_t* location) {
    LOC_LOG_QCLI_V("\nlocation_batching_callback_clnt_0");
    location_batching_callback(count, location);
}
static void location_batching_callback_clnt_1(
        size_t count, qapi_Location_t* location) {
    LOC_LOG_QCLI_V("\nlocation_batching_callback_clnt_1");
    location_batching_callback(count, location);
}
static void location_batching_callback_clnt_2(
        size_t count, qapi_Location_t* location) {
    LOC_LOG_QCLI_V("\nlocation_batching_callback_clnt_2");
    location_batching_callback(count, location);
}
static void location_batching_callback_clnt_3(
        size_t count, qapi_Location_t* location) {
    LOC_LOG_QCLI_V("\nlocation_batching_callback_clnt_3");
    location_batching_callback(count, location);
}

/* GEOFENCE BREACH CALLBACKs */
static void location_geofence_breach_callback(
        qapi_Geofence_Breach_Notification_t notification)
{
    uint32_t i = 0;
    uint64_t ts_sec = 0;
    LOC_LOG_QCLI_V("LOC API CALLBACK : GEOFENCE BREACH (count %d)", notification.count);

    // Breach type
    if (notification.type == QAPI_GEOFENCE_BREACH_ENTER) {
        LOC_LOG_QCLI_V("Breach Type: ENTER");
    } else if (notification.type == QAPI_GEOFENCE_BREACH_EXIT) {
        LOC_LOG_QCLI_V("Breach Type: EXIT");
    } else if (notification.type == QAPI_GEOFENCE_BREACH_DWELL_IN) {
        LOC_LOG_QCLI_V("Breach Type: DWELL IN");
    } else if (notification.type == QAPI_GEOFENCE_BREACH_DWELL_OUT) {
        LOC_LOG_QCLI_V("Breach Type: DWELL OUT");
    } else {
        LOC_LOG_QCLI_V("Breach type invalid: %d", notification.type);
    }

    // Timestamp and List of IDs received
    ts_sec = notification.location.timestamp/1000;
    LOC_LOG_QCLI_V("Time(HH:MM:SS): %02d:%02d:%02d : "
            "List of IDs recvd (count %d):",
            (int)((ts_sec/3600)%24), (int)((ts_sec/60)%60), (int)(ts_sec%60),
            notification.count);
    for(i = 0; i < notification.count; i++) {
        LOC_LOG_QCLI_V("ID: %d", notification.ids[i]);
    }

    LOC_LOG_QCLI_V("LAT: %d.%d LON: %d.%d ACC: %d.%d",
        (int)notification.location.latitude,
        (abs((int)(notification.location.latitude * 100000))) % 100000,
        (int)notification.location.longitude,
        (abs((int)(notification.location.longitude * 100000))) % 100000,
        (int)notification.location.accuracy,
        (abs((int)(notification.location.accuracy * 100))) % 100);

    LOC_LOG_QCLI_V("ALT: %d.%d ALT_MSL: %d.%d",
        (int)notification.location.altitude,
        (abs((int)(notification.location.altitude * 100))) % 100,
        (int)notification.location.altitudeMeanSeaLevel,
        (abs((int)(notification.location.altitudeMeanSeaLevel * 100))) % 100);

    LOC_LOG_QCLI_V("SPEED: %d.%d BEAR: %d.%d TIME: 0x%x%x FLAGS: %u",
        (int)notification.location.speed,
        (abs((int)(notification.location.speed * 100))) % 100,
        (int)notification.location.bearing,
        (abs((int)(notification.location.bearing * 100))) % 100,
        (int)(notification.location.timestamp >> 32),
        (int)notification.location.timestamp, notification.location.flags);
}
static void location_geofence_breach_callback_clnt_0(
        qapi_Geofence_Breach_Notification_t notification) {
    LOC_LOG_QCLI_V("\nlocation_geofence_breach_callback_clnt_0");
    location_geofence_breach_callback(notification);
}
static void location_geofence_breach_callback_clnt_1(
        qapi_Geofence_Breach_Notification_t notification) {
    LOC_LOG_QCLI_V("\nlocation_geofence_breach_callback_clnt_1");
    location_geofence_breach_callback(notification);
}
static void location_geofence_breach_callback_clnt_2(
        qapi_Geofence_Breach_Notification_t notification) {
    LOC_LOG_QCLI_V("\nlocation_geofence_breach_callback_clnt_2");
    location_geofence_breach_callback(notification);
}
static void location_geofence_breach_callback_clnt_3(
        qapi_Geofence_Breach_Notification_t notification) {
    LOC_LOG_QCLI_V("\nlocation_geofence_breach_callback_clnt_3");
    location_geofence_breach_callback(notification);
}

/* SINGLE SHOT CALLBACKs */
static void location_single_shot_callback(qapi_Location_t location)
{
    uint64_t ts_sec = location.timestamp / 1000;

    LOC_LOG_QCLI_V("LOC API CALLBACK : SINGLE SHOT");
    LOC_LOG_QCLI_V("Time(HH:MM:SS): %02d:%02d:%02d",
        (int)((ts_sec / 3600) % 24), (int)((ts_sec / 60) % 60), (int)(ts_sec % 60));

    LOC_LOG_QCLI_V("LAT: %d.%d LON: %d.%d ACC: %d.%d",
        (int)location.latitude, ((int)(location.latitude * 100000)) % 100000,
        (int)location.longitude, ((int)(location.longitude * 100000)) % 100000,
        (int)location.accuracy, ((int)(location.accuracy * 100)) % 100);

    LOC_LOG_QCLI_V("ALT: %d.%d ALT_MSL: %d.%d",
        (int)location.altitude, ((int)(location.altitude * 100)) % 100,
        (int)location.altitudeMeanSeaLevel, ((int)(location.altitudeMeanSeaLevel * 100)) % 100);

    LOC_LOG_QCLI_V("SPEED: %d.%d BEAR: %d.%d TIME: 0x%x%x FLAGS: %u",
        (int)location.speed, ((int)(location.speed * 100)) % 100,
        (int)location.bearing, ((int)(location.bearing * 100)) % 100,
        (int)(location.timestamp >> 32), (int)location.timestamp, location.flags);

    if (gGpsVerifyTestInProgress) {
        tx_event_flags_set(&gSignalLocation, 1, TX_OR);
    }
}
static void location_single_shot_callback_clnt_0(
    qapi_Location_t location,
    qapi_Location_Error_t err)
{
    LOC_LOG_QCLI_V("\nlocation_single_shot_callback_clnt_0");
    if (QAPI_LOCATION_ERROR_TIMEOUT == err) {
        LOC_LOG_QCLI_V("Timeout occurred, location is invalid");
    }
    else {
        location_single_shot_callback(location);
    }
}
static void location_single_shot_callback_clnt_1(
    qapi_Location_t location,
    qapi_Location_Error_t err)
{
    LOC_LOG_QCLI_V("\nlocation_single_shot_callback_clnt_1");
    if (QAPI_LOCATION_ERROR_TIMEOUT == err) {
        LOC_LOG_QCLI_V("Timeout occurred, location is invalid");
    }
    else {
        location_single_shot_callback(location);
    }
}
static void location_single_shot_callback_clnt_2(
    qapi_Location_t location,
    qapi_Location_Error_t err)
{
    LOC_LOG_QCLI_V("\nlocation_single_shot_callback_clnt_2");
    if (QAPI_LOCATION_ERROR_TIMEOUT == err) {
        LOC_LOG_QCLI_V("Timeout occurred, location is invalid");
    }
    else {
        location_single_shot_callback(location);
    }
}
static void location_single_shot_callback_clnt_3(
    qapi_Location_t location,
    qapi_Location_Error_t err)
{
    LOC_LOG_QCLI_V("\nlocation_single_shot_callback_clnt_3");
    if (QAPI_LOCATION_ERROR_TIMEOUT == err) {
        LOC_LOG_QCLI_V("Timeout occurred, location is invalid");
    }
    else {
        location_single_shot_callback(location);
    }
}

/* GNSS DATA CALLBACKs */
static void location_gnss_data_callback(qapi_Gnss_Data_t gnssData)
{
    LOC_LOG_QCLI_V("LOC API CALLBACK : GNSS DATA");
    LOC_LOG_QCLI_V("JAMMER_GPS: %u JAMMER_GLONASS: %u",
        gnssData.jammerInd[QAPI_GNSS_SV_TYPE_GPS],
        gnssData.jammerInd[QAPI_GNSS_SV_TYPE_GLONASS]);

    LOC_LOG_QCLI_V("JAMMER_BEIDOU: %u JAMMER_GALILEO: %u",
        gnssData.jammerInd[QAPI_GNSS_SV_TYPE_BEIDOU],
        gnssData.jammerInd[QAPI_GNSS_SV_TYPE_GALILEO]);

    if (gGpsVerifyTestInProgress) {
        tx_event_flags_set(&gSignalLocation, 1, TX_OR);
    }
}
static void location_gnss_data_callback_clnt_0(qapi_Gnss_Data_t gnssData)
{
    LOC_LOG_QCLI_V("\nlocation_gnss_data_callback_clnt_0");
    location_gnss_data_callback(gnssData);
}
static void location_gnss_data_callback_clnt_1(qapi_Gnss_Data_t gnssData)
{
    LOC_LOG_QCLI_V("\nlocation_gnss_data_callback_clnt_1");
    location_gnss_data_callback(gnssData);
}
static void location_gnss_data_callback_clnt_2(qapi_Gnss_Data_t gnssData)
{
    LOC_LOG_QCLI_V("\nlocation_gnss_data_callback_clnt_2");
    location_gnss_data_callback(gnssData);
}
static void location_gnss_data_callback_clnt_3(qapi_Gnss_Data_t gnssData)
{
    LOC_LOG_QCLI_V("\nlocation_gnss_data_callback_clnt_3");
    location_gnss_data_callback(gnssData);
}

static qapi_Location_Callbacks_t sLocationCallbacks;

/*==========================================================================
LOCATION INIT / DEINIT APIs
===========================================================================*/
static int location_init(uint32_t clientIdx, uint32_t cbMask)
{
    qapi_Location_Error_t ret;

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return -1;
    }
    if (clientInited[clientIdx]) {
        LOC_LOG_QCLI("Client at idx %d already inited", clientIdx);
        return -1;
    }
    if (cbMask == 0) {
        LOC_LOG_QCLI("cbmask can't be 0");
        return -1;
    }

    tx_event_flags_create(&gSignalGeofenceResponse, "gSignalGeofenceResponse");
    tx_event_flags_create(&gSignalResponse, "gSignalResponse");
    tx_event_flags_create(&gSignalLocation, "gSignalLocation");

    memset(&sLocationCallbacks, 0, sizeof(qapi_Location_Callbacks_t));
    sLocationCallbacks.size = sizeof(qapi_Location_Callbacks_t);

    if (clientIdx == 0) {
        sLocationCallbacks.capabilitiesCb = location_capabilities_callback_clnt_0;
        sLocationCallbacks.responseCb = location_response_callback_clnt_0;
        sLocationCallbacks.collectiveResponseCb = location_collective_response_callback_clnt_0;
        sLocationCallbacks.trackingCb = (cbMask & 0x1)? location_tracking_callback_clnt_0 : NULL;
        sLocationCallbacks.batchingCb = (cbMask & 0x2)? location_batching_callback_clnt_0 : NULL;
        sLocationCallbacks.geofenceBreachCb = (cbMask & 0x4)? location_geofence_breach_callback_clnt_0 : NULL;
        sLocationCallbacks.singleShotCb = (cbMask & 0x8)? location_single_shot_callback_clnt_0 : NULL;
        sLocationCallbacks.gnssDataCb = (cbMask & 0x10) ? location_gnss_data_callback_clnt_0 : NULL;
    } else if (clientIdx == 1) {
        sLocationCallbacks.capabilitiesCb = location_capabilities_callback_clnt_1;
        sLocationCallbacks.responseCb = location_response_callback_clnt_1;
        sLocationCallbacks.collectiveResponseCb = location_collective_response_callback_clnt_1;
        sLocationCallbacks.trackingCb = (cbMask & 0x1)? location_tracking_callback_clnt_1 : NULL;
        sLocationCallbacks.batchingCb = (cbMask & 0x2)? location_batching_callback_clnt_1 : NULL;
        sLocationCallbacks.geofenceBreachCb = (cbMask & 0x4)? location_geofence_breach_callback_clnt_1 : NULL;
        sLocationCallbacks.singleShotCb = (cbMask & 0x8)? location_single_shot_callback_clnt_1 : NULL;
        sLocationCallbacks.gnssDataCb = (cbMask & 0x10) ? location_gnss_data_callback_clnt_1 : NULL;
    } else if (clientIdx == 2) {
        sLocationCallbacks.capabilitiesCb = location_capabilities_callback_clnt_2;
        sLocationCallbacks.responseCb = location_response_callback_clnt_2;
        sLocationCallbacks.collectiveResponseCb = location_collective_response_callback_clnt_2;
        sLocationCallbacks.trackingCb = (cbMask & 0x1)? location_tracking_callback_clnt_2 : NULL;
        sLocationCallbacks.batchingCb = (cbMask & 0x2)? location_batching_callback_clnt_2 : NULL;
        sLocationCallbacks.geofenceBreachCb = (cbMask & 0x4)? location_geofence_breach_callback_clnt_2 : NULL;
        sLocationCallbacks.singleShotCb = (cbMask & 0x8)? location_single_shot_callback_clnt_2 : NULL;
        sLocationCallbacks.gnssDataCb = (cbMask & 0x10) ? location_gnss_data_callback_clnt_2 : NULL;
    } else if (clientIdx == 3) {
        sLocationCallbacks.capabilitiesCb = location_capabilities_callback_clnt_3;
        sLocationCallbacks.responseCb = location_response_callback_clnt_3;
        sLocationCallbacks.collectiveResponseCb = location_collective_response_callback_clnt_3;
        sLocationCallbacks.trackingCb = (cbMask & 0x1)? location_tracking_callback_clnt_3 : NULL;
        sLocationCallbacks.batchingCb = (cbMask & 0x2)? location_batching_callback_clnt_3 : NULL;
        sLocationCallbacks.geofenceBreachCb = (cbMask & 0x4)? location_geofence_breach_callback_clnt_3 : NULL;
        sLocationCallbacks.singleShotCb = (cbMask & 0x8)? location_single_shot_callback_clnt_3 : NULL;
        sLocationCallbacks.gnssDataCb = (cbMask & 0x10) ? location_gnss_data_callback_clnt_3 : NULL;
    }

    LOC_LOG_QCLI("Invoking qapi_Loc_Init()");

    ret = (qapi_Location_Error_t)qapi_Loc_Init(
            (UINT)&clientIdArr[clientIdx],
            (UINT)&sLocationCallbacks);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("LOC INIT returned failure ! (ret %d)", ret);
        return -1;
    }

    ret = (qapi_Location_Error_t)qapi_Loc_Set_User_Buffer(
            (qapi_loc_client_id)clientIdArr[clientIdx],
            (uint8_t*)userBuffer,
            (size_t)USER_BUFFER_SIZE);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Set user buffer failed ! (ret %d)", ret);
        return -1;
    }

    LOC_LOG_QCLI("Returned success ! (client ID %d)", clientIdArr[clientIdx]);
    clientInited[clientIdx] = 1;

    return 0;
}

static int location_deinit(uint32_t clientIdx)
{
    qapi_Location_Error_t ret;

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return -1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d already de-inited", clientIdx);
        return -1;
    }

    LOC_LOG_QCLI("Invoking qapi_Loc_Deinit(), clientIdArr[%d] = %d",
            clientIdx, clientIdArr[clientIdx]);
    ret = (qapi_Location_Error_t)qapi_Loc_Deinit(clientIdArr[clientIdx]);
    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QCLI("Returned success");
    clientInited[clientIdx] = 0;

    tx_event_flags_delete(&gSignalGeofenceResponse);
    tx_event_flags_delete(&gSignalResponse);
    tx_event_flags_delete(&gSignalLocation);

    return 0;
}

/*==========================================================================
TEST CASES
===========================================================================*/
// This is singleshot test
// Once callback is received, it invokes stop tracking API

static void gps_verify_tracking_test_singleshot(
        uint32_t clientIdx, uint32_t minInterval, uint32_t minDistance)
{
    ULONG set_signal = 0;

    LOC_LOG_QCLI("GPS TRACKING TEST : SINGLESHOT : START");

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return;
    }

    gGpsVerifyTestInProgress = 1;

    sTrackingSessionId = location_start_tracking(
            clientIdx, minInterval, minDistance);

    LOC_LOG_QCLI("Waiting for Location..");
    tx_event_flags_get(&gSignalLocation,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever
    LOC_LOG_QCLI("Got Location");

    gGpsVerifyTestInProgress = 0;

    location_stop_tracking(clientIdx, sTrackingSessionId);

    LOC_LOG_QCLI("GPS TRACKING TEST : SINGLESHOT : END");
}

static void gps_verify_batching_test_singleshot(
        uint32_t clientIdx, uint32_t minInterval, uint32_t minDistance)
{
    ULONG set_signal = 0;

    LOC_LOG_QCLI("GPS BATCHING TEST : SINGLESHOT : START");

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return;
    }

    gGpsVerifyTestInProgress = 1;

    sBatchingSessionId = location_start_batching(
            clientIdx, minInterval, minDistance);

    LOC_LOG_QCLI("Waiting for Location Batch..");
    tx_event_flags_get(&gSignalLocation,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever
    LOC_LOG_QCLI("Got Location Batch");

    gGpsVerifyTestInProgress = 0;

    location_stop_batching(clientIdx, sBatchingSessionId);

    LOC_LOG_QCLI("GPS BATCHING TEST : SINGLESHOT : END");
}

/*==========================================================================
LOCATION API USAGE HELPER APIs : BEGIN
===========================================================================*/

static int location_resume_geofences(uint32_t clientIdx, size_t count, uint32_t* ids)
{
    ULONG set_signal = 0;
    uint32_t i = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION RESUME GEOFENCES");

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return -1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return -1;
    }

    // print count and ID list
    LOC_LOG_QCLI("Count %d, ID list: ", count);
    for(i = 0; i < count; i++) {
        LOC_LOG_QCLI("ID: %d", ids[i]);
    }

    LOC_LOG_QCLI("Invoking qapi_Loc_Resume_Geofences()");
    ret = (qapi_Location_Error_t)qapi_Loc_Resume_Geofences(clientIdArr[clientIdx], count, ids);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QCLI("Returned success");

    LOC_LOG_QCLI("Waiting on Callback..");
    tx_event_flags_get(&gSignalGeofenceResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return 0;
}

static int location_pause_geofences(uint32_t clientIdx, size_t count, uint32_t* ids)
{
    ULONG set_signal = 0;
    uint32_t i = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION PAUSE GEOFENCES");

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return -1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return -1;
    }

    // print count and ID list
    LOC_LOG_QCLI("Count %d, ID list: ", count);
    for(i = 0; i < count; i++) {
        LOC_LOG_QCLI("ID: %d", ids[i]);
    }

    LOC_LOG_QCLI("Invoking qapi_Loc_Pause_Geofences()");
    ret = (qapi_Location_Error_t)qapi_Loc_Pause_Geofences(clientIdArr[clientIdx], count, ids);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QCLI("Returned success");

    LOC_LOG_QCLI("Waiting on Callback...");
    tx_event_flags_get(&gSignalGeofenceResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return 0;
}

static int location_modify_geofences(
        uint32_t clientIdx, size_t count, uint32_t* ids, qapi_Geofence_Option_t* options)
{
    uint32_t i;
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION MODIFY GEOFENCES");

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return -1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return -1;
    }

    // print count and ID list
    LOC_LOG_QCLI("Count %d, ID/Options list: ", count);
    for (i = 0; i < count; ++i) {
        LOC_LOG_QCLI("id=%u breach=%u respon=%u dwell=%u", ids[i],
            options[i].breachTypeMask, options[i].responsiveness,
            options[i].dwellTime);
    }

    LOC_LOG_QCLI("Invoking qapi_Loc_Modify_Geofences()");
    ret = (qapi_Location_Error_t)qapi_Loc_Modify_Geofences(clientIdArr[clientIdx], count, ids, options);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QCLI("Returned success");

    LOC_LOG_QCLI("Waiting on Callback...");
    tx_event_flags_get(&gSignalGeofenceResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return 0;
}

static int location_remove_geofences(uint32_t clientIdx, size_t count, uint32_t* ids)
{
    ULONG set_signal = 0;
    uint32_t i;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION REMOVE GEOFENCES");

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return -1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return -1;
    }

    // print count and ID list
    LOC_LOG_QCLI("Count %d, ID list: ", count);
    for(i = 0; i < count; i++) {
        LOC_LOG_QCLI("ID: %d", ids[i]);
    }

    LOC_LOG_QCLI("Invoking qapi_Loc_Remove_Geofences()");
    ret = (qapi_Location_Error_t)qapi_Loc_Remove_Geofences(clientIdArr[clientIdx], count, ids);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QCLI("Returned success");

    LOC_LOG_QCLI("Waiting on Callback...");
    tx_event_flags_get(&gSignalGeofenceResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return 0;
}

static uint32_t* location_add_geofences(
        uint32_t clientIdx, size_t count, qapi_Geofence_Option_t* options,
    qapi_Geofence_Info_t* datas)
{
    uint32_t* ids = NULL;
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION ADD GEOFENCES");

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return NULL;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return NULL;
    }

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return NULL;
    }

    LOC_LOG_QCLI("Invoking qapi_Loc_Add_Geofences()");
    ret = (qapi_Location_Error_t)qapi_Loc_Add_Geofences(
            clientIdArr[clientIdx], count, options, datas, &ids);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return NULL;
    }
    LOC_LOG_QCLI("Returned success");

    LOC_LOG_QCLI("Waiting on Callback...");
    tx_event_flags_get(&gSignalGeofenceResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return ids;
}

static int location_get_batched_locations(uint32_t clientIdx, uint32_t id, size_t count)
{
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION GET BATCHED LOCATIONS");
    LOC_LOG_QCLI("ID %d, Count %d", id, count);

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return -1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return -1;
    }

    LOC_LOG_QCLI("Invoking qapi_Loc_Get_Batched_Locations()");
    ret = (qapi_Location_Error_t)qapi_Loc_Get_Batched_Locations(clientIdArr[clientIdx], id, count);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QCLI("Returned success");

    LOC_LOG_QCLI("Waiting on Callback...");
    tx_event_flags_get(&gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER);  // Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return 0;
}

static uint32_t location_start_batching(
        uint32_t clientIdx, uint32_t minInterval, uint32_t minDistance)
{
    uint32_t id = (uint32_t)-1;
    qapi_Location_Options_t locationOptions;
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION START BATCHING");
    LOC_LOG_QCLI("Interval %u, Distance %u", minInterval, minDistance);

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return (uint32_t)-1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return (uint32_t)-1;
    }

    locationOptions.size = sizeof(qapi_Location_Options_t);
    locationOptions.minInterval = minInterval;
    locationOptions.minDistance = minDistance;

    LOC_LOG_QCLI("Invoking qapi_Loc_Start_Batching");
    ret = (qapi_Location_Error_t)qapi_Loc_Start_Batching(clientIdArr[clientIdx], &locationOptions, &id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return (uint32_t)-1;
    }
    LOC_LOG_QCLI("Returned success, id %d", id);

    LOC_LOG_QCLI("Waiting on Callback...");
    tx_event_flags_get(&gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return id;
}

static int location_stop_batching(uint32_t clientIdx, uint32_t id)
{
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION STOP BATCHING");

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return -1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return -1;
    }

    LOC_LOG_QCLI("Invoking qapi_Loc_Stop_Batching() : id %d", id);
    ret = (qapi_Location_Error_t)qapi_Loc_Stop_Batching(clientIdArr[clientIdx], id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QCLI("Returned success");

    LOC_LOG_QCLI("Waiting on Callback...");
    tx_event_flags_get(&gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return 0;
}

static int location_update_batching_options(uint32_t clientIdx, uint32_t id, uint32_t minInterval, uint32_t minDistance)
{
    qapi_Location_Options_t locationOptions;
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION UPDATE BATCHING OPTIONS");
    LOC_LOG_QCLI("id %u, Interval %u, Distance %u", id,
            minInterval, minDistance);

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return -1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return -1;
    }

    locationOptions.size = sizeof(qapi_Location_Options_t);
    locationOptions.minInterval = minInterval;
    locationOptions.minDistance = minDistance;

    LOC_LOG_QCLI("Invoking qapi_Loc_Update_Batching_Options()");
    ret = (qapi_Location_Error_t)qapi_Loc_Update_Batching_Options(clientIdArr[clientIdx], id, &locationOptions);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QCLI("Returned success");

    LOC_LOG_QCLI("Waiting on Callback...");
    tx_event_flags_get(&gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return 0;
}

static uint32_t location_start_tracking(uint32_t clientIdx, uint32_t minInterval, uint32_t minDistance)
{
    uint32_t id = (uint32_t)-1;
    qapi_Location_Options_t locationOptions = {0};
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION START TRACKING");
    LOC_LOG_QCLI("Interval %u, Distance %u", minInterval, minDistance);

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return (uint32_t)-1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return (uint32_t)-1;
    }

    locationOptions.size = sizeof(qapi_Location_Options_t);
    locationOptions.minInterval = minInterval;
    locationOptions.minDistance = minDistance;

    LOC_LOG_QCLI("Invoking qapi_Loc_Start_Tracking()");
    ret = (qapi_Location_Error_t)qapi_Loc_Start_Tracking(
            (UINT)clientIdArr[clientIdx], (UINT)&locationOptions, (UINT)&id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return (uint32_t)-1;
    }
    LOC_LOG_QCLI("Returned success, id %d", id);

    LOC_LOG_QCLI("Waiting on Callback...");
    tx_event_flags_get(&gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return id;
}

static int location_stop_tracking(uint32_t clientIdx, uint32_t id)
{
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION STOP TRACKING");

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return -1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return -1;
    }

    LOC_LOG_QCLI("Invoking qapi_Loc_Stop_Tracking() : id %d", id);
    ret = (qapi_Location_Error_t)qapi_Loc_Stop_Tracking(clientIdArr[clientIdx], id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QCLI("Returned success");

    LOC_LOG_QCLI("Waiting on Callback...");
    tx_event_flags_get(&gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return 0;
}

static int location_update_tracking_options(uint32_t clientIdx, uint32_t id, uint32_t minInterval, uint32_t minDistance)
{
    qapi_Location_Options_t locationOptions;
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION UPDATE TRACKING OPTIONS");
    LOC_LOG_QCLI("id %u, Interval %u, Distance %u", id,
            minInterval, minDistance);

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
                clientIdx, MAX_SUPPORTED_CLIENTS-1);
        return -1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return -1;
    }

    locationOptions.size = sizeof(qapi_Location_Options_t);
    locationOptions.minInterval = minInterval;
    locationOptions.minDistance = minDistance;

    LOC_LOG_QCLI("Invoking qapi_Loc_Update_Tracking_Options()");
    ret = (qapi_Location_Error_t)qapi_Loc_Update_Tracking_Options(
            clientIdArr[clientIdx], id, &locationOptions);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QCLI("Returned success");

    LOC_LOG_QCLI("Waiting on Callback...");
    tx_event_flags_get(&gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return 0;
}

static uint32_t location_get_single_shot(
    uint32_t clientIdx, uint32_t powerLevel)
{
    uint32_t id = (uint32_t)-1;
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION GET SINGLE SHOT");
    LOC_LOG_QCLI("Location Power Level %u", powerLevel);

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
            clientIdx, MAX_SUPPORTED_CLIENTS - 1);
        return (uint32_t)-1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return (uint32_t)-1;
    }

    LOC_LOG_QCLI("Invoking qapi_Loc_Get_Single_Shot()");
    ret = (qapi_Location_Error_t)qapi_Loc_Get_Single_Shot(
        (UINT)clientIdArr[clientIdx], (UINT)powerLevel, (UINT)&id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return (uint32_t)-1;
    }
    LOC_LOG_QCLI("Returned success, id %d", id);

    LOC_LOG_QCLI("Waiting on Callback...");
    tx_event_flags_get(&gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return id;
}

static int location_cancel_single_shot(uint32_t clientIdx, uint32_t id)
{
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION CANCEL SINGLE SHOT");

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
            clientIdx, MAX_SUPPORTED_CLIENTS - 1);
        return -1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return -1;
    }

    LOC_LOG_QCLI("Invoking qapi_Loc_Cancel_Single_Shot() : id %d", id);
    ret = (qapi_Location_Error_t)qapi_Loc_Cancel_Single_Shot(clientIdArr[clientIdx], id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QCLI("Returned success");

    LOC_LOG_QCLI("Waiting on Callback...");
    tx_event_flags_get(&gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return 0;
}

static uint32_t location_start_get_gnss_data(uint32_t clientIdx)
{
    uint32_t id = (uint32_t)-1;
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION START GET GNSS DATA");

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
            clientIdx, MAX_SUPPORTED_CLIENTS - 1);
        return (uint32_t)-1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return (uint32_t)-1;
    }

    LOC_LOG_QCLI("Invoking qapi_Loc_Start_Get_Gnss_Data()");
    ret = (qapi_Location_Error_t)qapi_Loc_Start_Get_Gnss_Data(
        (UINT)clientIdArr[clientIdx], (UINT)&id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return (uint32_t)-1;
    }
    LOC_LOG_QCLI("Returned success, id %d", id);

    LOC_LOG_QCLI("Waiting on Callback...");
    tx_event_flags_get(&gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return id;
}

static int location_stop_get_gnss_data(uint32_t clientIdx, uint32_t id)
{
    ULONG set_signal = 0;
    qapi_Location_Error_t ret;

    LOC_LOG_QCLI("LOCATION STOP GET GNSS DATA");

    if (clientIdx >= MAX_SUPPORTED_CLIENTS) {
        LOC_LOG_QCLI("Invalid Client Index %d, should be between 0 and %d",
            clientIdx, MAX_SUPPORTED_CLIENTS - 1);
        return -1;
    }
    if (clientInited[clientIdx] == 0) {
        LOC_LOG_QCLI("Client at idx %d is not inited", clientIdx);
        return -1;
    }

    LOC_LOG_QCLI("Invoking qapi_Loc_Stop_Get_Gnss_Data() : id %d", id);
    ret = (qapi_Location_Error_t)qapi_Loc_Stop_Get_Gnss_Data(clientIdArr[clientIdx], id);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
        LOC_LOG_QCLI("Returned failure ! (ret %d)", ret);
        return -1;
    }
    LOC_LOG_QCLI("Returned success");

    LOC_LOG_QCLI("Waiting on Callback...");
    tx_event_flags_get(&gSignalResponse,
        1, TX_OR_CLEAR,
        &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

    LOC_LOG_QCLI("REQUEST SUCCESS");
    return 0;
}

/*==========================================================================
LOCATION API USAGE HELPER APIs : END
===========================================================================*/

/* Utility APIs */
double qapi_loc_util_string_to_double(char* str){

    uint8_t digitValue = 0;
    double value = 0.0;
    char digit = '\0';
    int i = 0, negative = 0, dotSeen = 0;
    double fracMultiplier = 1;
    int strLen = strlen(str);

    if(strLen <= 0){
        return 0.0;
    }

    // traverse all characters in string
    if(str[0] == '-'){
        i++;
        negative = 1;
        if(strLen == 1){
            return 0.0;
        }
    }
    for(; i < strLen; i++){
        digit = str[i];
        if(dotSeen == 0 && digit == '.'){
            dotSeen = 1;
            continue;
        }
        if(dotSeen == 1){
            fracMultiplier *= 0.1;
        }
        // ascii 48 = '0', 57 = '9'
        if(digit < 48 || digit > 57){
            return 0.0;
        }
        digitValue = digit - 48;
        value = value*10 + digitValue;
    }
    value = value * fracMultiplier;
    if(negative == 1)
        value = value * -1;

    return value;
}
