/*============================================================================
  @file therm_npa_sensor.c

  Define NPA nodes representing sensors.  Provide polling functionality where
  needed.

  Copyright (c) 2011-2019 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.mpss/3.10/power/thermal/src/target/therm_npa_sensor.c#4 $
$DateTime: 2019/08/16 04:06:39 $
$Author: pwbldsvc $
========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "npa_resource.h"
#include "AdcInputs.h"
#include "DDIAdc.h"
#include "DDITsens.h"
#include "timer.h"
#include "CoreVerify.h"
#include "CoreMutex.h"
#include "therm_diag.h"
#include "therm_log.h"
#include "stdarg.h"
#include <stringl/stringl.h>
#include "log.h"
#include "rex.h"
#include "therm_npa_sensor.h"
#include "therm_v.h"
#include "DDITimetick.h"
#include "CoreTime.h"
#ifdef QFE_PA
#include "rfm.h"
#include "ftm.h"
#include "mmtask.h"
#endif

/*=======================================================================

                  STATIC MEMBER / FUNCTION DECLARATIONS / DEFINITIONS

========================================================================*/

/* Use 300 K as default temp */
#define THERM_NPA_DEFAULT_TEMP 300
#define THERM_NPA_MAX_RES_TEMP 528

#define THERM_NPA_SENSOR_MAX 8
#define THERM_NPA_STR_MAX    64

#define MAX_QFE_THERMISTOR_COUNT 8
#define QFE_BIT_MASK             0x0004
#define TSENS_BIT_MASK           0x0002
#define ADC_BIT_MASK             0x0001

#define THERM_K_TO_C(x) ((int32)(((int32)(x)) - 273))
#define THERM_C_TO_K(x) ((int32)(((int32)(x)) + 273))

#define THERM_NPA_BCL_WARNING             "therm_bcl_warning"
#define THERM_NPA_BCL_DEFAULT_WARNING     0
#define THERM_TSENS                       "TSENS"
#define THERM_QFE                         "QFE"
#define THERM_DEGREES_K_STR               "Degrees K"
#define THERM_WARNING_LVL_STR             "Warning Level"

/* Max age of valid sensor reading 15 seconds */
#define THERM_MAX_QFE_SENSOR_AGE 15000
/* Min age of prior QFE query before making another 1ms */
#define THERM_MIN_QFE_QUERY_AGE 1

#define NPA_RESOURCE_AT_INDEX( node_ptr, index ) (&((node_ptr)->resources[(index)]))
#define TEST_BIT_OF_MASK( mask, value ) (mask & value)

#define MCPM_RF_THERM_NPA_RESRC_NAME        "/mcpm/rf_thermistor"

/* Struct for allowing sync reads using ADC DAL interface. */
typedef struct
{
  AdcResultType *pAdcResult;
  DALSYSEventHandle hCallbackEvent;
  DALSYS_EVENT_OBJECT(callbackEventObject);
  DALSYSEventHandle hSignalEvent;
  DALSYS_EVENT_OBJECT(signalEventObject);
  DALSYSSyncHandle hSync;
  DALSYS_SYNC_OBJECT(syncObject);
} therm_npa_adc_sync_read_obj_type;

/* Internal struct for managing ADC DAL sensor init of read object */
typedef struct
{
  AdcResultType                    readResult;
  therm_npa_adc_sync_read_obj_type readObject;
  AdcInputPropertiesType           properties;
  boolean                          itemInit;
  boolean                          sensorPresent;
} therm_npa_adc_sync_read_type;

/* Struct for the polling grouping */
typedef struct
{
  timer_type                       groupSamplingTimer;
  npa_resource_state               groupSamplingTime;
  boolean                          groupInUse;
  uint8                            groupSensorCount;
} therm_group_polling_info_type;

/* Struct for the sensorClientHandles and group containing */
typedef struct
{
  npa_client_handle                sensorClientHandle;
  uint8                            groupId;
} sensor_clients;

/* ADC DAL device handle */
static DalDeviceHandle *phADCDalDevHandle = NULL;

/* Internal struct for managing TSENS DAL sensor init of read object */
typedef struct
{
  boolean                          itemInit;
  boolean                          sensorPresent;
} therm_npa_tsens_sync_read_type;

#ifdef QFE_PA
/* Internal struct for managing QFE senosr init of read object */
typedef struct
{
  /* Can have up to MAX_QFE_THERMISTOR_COUNT QFE devices */
  npa_query_handle                 pa_npa_clnt;
  npa_query_handle                 pa1_npa_clnt;
  rfm_thermistor_type              rf_therm_list[MAX_QFE_THERMISTOR_COUNT];
  uint64                           rf_query_time;
  uint8                            rf_therm_cnt;
  boolean                          rf_query_valid;
  npa_query_handle                 mcpm_query_clnt;
} therm_npa_rf_read_type;
#endif

/* TSENS DAL devince handle */
static DalDeviceHandle *phTSENSDALDevHandle = NULL;

static uint32 numTSensors;

#ifdef QFE_PA
static unsigned int mask_init = 0x0000;
static unsigned int mask_temp_available = 0x0000;

static DalDeviceHandle *phTimetick = NULL;;
#endif

static therm_npa_adc_sync_read_type pa0_adc_read_data;

static therm_sensor_info pa0_info =
{
  .subsystem_id = PA_THERM_SS,
  .sensor_id = 0,
  .units_of_measure = THERM_DEGREES_K_STR,
  .unit_per_whole = 1,
  .sensor_input_str = ADC_INPUT_PA_THERM,
  .npa_node_str = "pa",
  .sensor_type = THERM_NPA_HKADC_SENSOR,
  .sensor_data = (void*)&pa0_adc_read_data,
  .export = TRUE,
};

static therm_npa_adc_sync_read_type pa1_adc_read_data;

static therm_sensor_info pa1_info =
{
  .subsystem_id = PA_THERM_SS,
  .sensor_id = 1,
  .units_of_measure = THERM_DEGREES_K_STR,
  .unit_per_whole = 1,
  .sensor_input_str = ADC_INPUT_PA_THERM1,
  .npa_node_str = "pa_1",
  .sensor_type = THERM_NPA_HKADC_SENSOR,
  .sensor_data = (void*)&pa1_adc_read_data,
  .export = TRUE,
};

static therm_sensor_info modem_bcl_warn_info =
{
  .subsystem_id = LOGICAL_THERM_SS,
  .sensor_id = 0,
  .units_of_measure = THERM_WARNING_LVL_STR,
  .unit_per_whole = 1,
  .sensor_input_str = THERM_NPA_BCL_WARNING,
  .npa_node_str = "modem_bcl_warning",
  .sensor_type = THERM_NPA_LOGICAL_SENSOR,
  .export = TRUE,
};

static therm_npa_tsens_sync_read_type tsens_sync_read_data;
static therm_sensor_info modem_tsens_info =
{
  .subsystem_id = MSM_THERM_SS,
  .sensor_id = TSENS_OFFLINE_MODEM,
  .units_of_measure = THERM_DEGREES_K_STR,
  .unit_per_whole = 1,
  .sensor_input_str = THERM_TSENS,
  .npa_node_str = "modem_tsens",
  .sensor_type = THERM_NPA_TSENS_SENSOR,
  .sensor_data = (void*)&tsens_sync_read_data,
  .export = FALSE,
};

#ifdef QFE_PA
/* Statically allocate memory for the resulting qfe query */
static therm_npa_rf_read_type qfe_pa0_read_data;
static therm_sensor_info qfe_pa0_info =
{
  .subsystem_id = PA_THERM_SS,
  .sensor_id = 2,
  .units_of_measure = THERM_DEGREES_K_STR,
  .unit_per_whole = 1,
  .sensor_input_str = THERM_QFE,
  .npa_node_str = "qfe_pa0",
  .sensor_type = THERM_NPA_QFE_SENSOR,
  .sensor_data = (void*)&qfe_pa0_read_data,
  .export = TRUE,
};
#endif

/* Structure to maintain the internal state of the DCVS NPA members */
typedef struct
{
  npa_resource_state sampling_time;
} therm_npa_type;

static npa_resource_state therm_sensor_update_fcn( npa_resource *resource,
                                                   npa_client_handle client );

static npa_resource_state therm_logical_max_update_fcn( npa_resource *resource,
                                                        npa_client_handle client );

static npa_resource_state therm_node_driver_fcn( npa_resource *resource,
                                                 npa_client *client,
                                                 npa_resource_state state );

static npa_resource_state therm_sampling_node_driver_fcn( npa_resource *resource,
                                                          npa_client *client,
                                                          npa_resource_state state );

static void therm_serial_timer_cb(timer_cb_data_type group_id);

/*=======================================================================

                  GLOBAL DEFINITIONS

========================================================================*/

/* "/therm/sensor/resource/sampling" node and resource
  Warning:
  therm_sampling_resource size should be the same as
  therm_sensor_resource.
*/
static npa_resource_definition therm_sampling_resource[] =
{
  {
    "/therm/sensor/pa/sampling",        /* name of resource */
    "Milliseconds",                     /* Units of the resource */
    0xFFFFFFFF,                         /* Maximum value of resource */
    &npa_min_plugin,                    /* Plugin - only handles impulses */
    NPA_RESOURCE_DEFAULT,
    NULL,
    NULL
  },
  {
    "/therm/sensor/pa_1/sampling",      /* name of resource */
    "Milliseconds",                     /* Units of the resource */
    0xFFFFFFFF,                         /* Maximum value of resource */
    &npa_min_plugin,                    /* Plugin - only handles impulses */
    NPA_RESOURCE_DEFAULT,
    NULL,
    NULL
  },
  {
    "/therm/sensor/modem_bcl_warning/sampling", /* name of resource */
    "Milliseconds",                             /* Units of the resource */
    0xFFFFFFFF,                                 /* Maximum value of resource */
    &npa_min_plugin,                            /* Plugin - only handles impulses */
    NPA_RESOURCE_DEFAULT,
    NULL,
    NULL
  },
  {
    "/therm/sensor/modem_tsens/sampling", /* name of resource */
    "Milliseconds",                       /* Units of the resource */
    0xFFFFFFFF,                           /* Maximum value of resource */
    &npa_min_plugin,                      /* Plugin - only handles impulses */
    NPA_RESOURCE_DEFAULT,
    NULL,
    NULL
  },
  #ifdef QFE_PA
  {
    "/therm/sensor/qfe_pa0/sampling",   /* name of resource */
    "Milliseconds",                     /* Units of the resource */
    0xFFFFFFFF,                         /* Maximum value of resource */
    &npa_min_plugin,                    /* Plugin - only handles impulses */
    NPA_RESOURCE_DEFAULT,
    NULL,
    NULL
  },
  #endif
};

/* Define the dependencies of our sampling resource.  Must edit if additional
   sensors are added */
static npa_node_dependency therm_sampling_deps[] =
{
  { "/therm/sensor/pa",                NPA_NO_CLIENT },
  { "/therm/sensor/pa_1",              NPA_NO_CLIENT },
  { "/therm/sensor/modem_bcl_warning", NPA_NO_CLIENT },
  { "/therm/sensor/modem_tsens",         NPA_NO_CLIENT },
#ifdef QFE_PA
  { "/therm/sensor/qfe_pa0",           NPA_NO_CLIENT },
#endif
};


static npa_node_definition therm_sampling_node =
{
  "/node/therm/sampling",            /* Node name - info only */
  therm_sampling_node_driver_fcn,    /* Driver function for temperature */
  NPA_NODE_DEFAULT,                  /* No attributes */
  NULL,                              /* No User Data */
  NPA_ARRAY(therm_sampling_deps),    /* Dependencies */
  NPA_ARRAY(therm_sampling_resource) /* Resources */
};

static npa_resource_state initial_sampling_state[] = {
  10000,
  10000,
  10000,
  10000,
  #ifdef QFE_PA
  10000,
  #endif
  };

/* The plugin definition for /therm/<sensor> resource */
const npa_resource_plugin therm_sensor_plugin =
{
  therm_sensor_update_fcn,
  NPA_CLIENT_IMPULSE,                            /* Supported client types */
  NULL,                                          /* Create client function */
  NULL                                           /* Destroy client function */
};

/* The plugin definition for logical resource - similar to npa max
 * plugin with added logging */
const npa_resource_plugin therm_sensor_max_plugin =
{
  therm_logical_max_update_fcn,
  NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE |
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE,     /* Supported Client types */
  NULL,                                          /* Create client function */
  NULL                                           /* Destroy client function */
};

/* "therm" node and resource
   Must edit if additional sensors are added.

   Place all actual physical resources together at the beginning and
   logical/virtual resources at the end.

   Warning:
   therm_sampling_resource size should be the same as
   therm_sensor_resource.
 */
static npa_resource_definition therm_sensor_resource[] =
{
  /* Physical Resources */
  {
    "/therm/sensor/pa",                        /* name of resource */
    "Degrees K",                               /* Units of the resource */
    THERM_NPA_MAX_RES_TEMP,                    /* Maximum value of resource */
    &therm_sensor_plugin,
    NPA_RESOURCE_DEFAULT,
    (npa_user_data)&pa0_info,
    NULL
  },
  {
    "/therm/sensor/pa_1",                      /* name of resource */
    "Degrees K",                               /* Units of the resource */
    THERM_NPA_MAX_RES_TEMP,                    /* Maximum value of resource */
    &therm_sensor_plugin,
    NPA_RESOURCE_DEFAULT,
    (npa_user_data)&pa1_info,
    NULL
  },
  /* Logical Resources */
  {
    "/therm/sensor/modem_bcl_warning",         /* name of resource */
    "Warning Level",                           /* Unit of the resource */
    100,                                       /* Maximum value of resource */
    &therm_sensor_max_plugin,                  /* Plugin - Max Aggregation */
    NPA_RESOURCE_DEFAULT,                      /* Resource attribute */
    (npa_user_data)&modem_bcl_warn_info,       /* Resource specific data */
    NULL                                       /* Default query function */
  },
  /* Tsens Resources */
  {
    "/therm/sensor/modem_tsens",               /* name of resource */
    "Degrees K",                               /* Units of the resource */
    THERM_NPA_MAX_RES_TEMP,                    /* Maximum value of resource */
    &therm_sensor_plugin,
    NPA_RESOURCE_DEFAULT,
    (npa_user_data)&modem_tsens_info,
    NULL
  },
#ifdef QFE_PA
  /* QFE Resources */
  {
    "/therm/sensor/qfe_pa0",                   /* name of resource */
    "Degrees K",                               /* Units of the resource */
    THERM_NPA_MAX_RES_TEMP,                    /* Maximum value of resource */
    &therm_sensor_plugin,
    NPA_RESOURCE_DEFAULT,
    (npa_user_data)&qfe_pa0_info,
    NULL
  },
#endif
};

/* Must edit if additional sensors are added */
static npa_resource_state initial_sensor_state[] =
{
  /* Initial state for ADC resources */
  THERM_NPA_DEFAULT_TEMP,
  THERM_NPA_DEFAULT_TEMP,
  /* Initial state for logical resources */
  THERM_NPA_BCL_DEFAULT_WARNING,
  /* Initial state for TSENS resources */
  THERM_NPA_DEFAULT_TEMP,
#ifdef QFE_PA
  /* Initial state for QFE resources */
  THERM_NPA_DEFAULT_TEMP,
#endif
};


npa_node_definition therm_sensor_node =
{
  "/node/therm/sensor",          /* Node name - info only */
  therm_node_driver_fcn,         /* Driver function for temperature */
  NPA_NODE_DEFAULT,              /* No attributes */
  NULL,                          /* No User Data */
  NPA_EMPTY_ARRAY,               /* No Dependencies */
  NPA_ARRAY(therm_sensor_resource) /* Resources */
};

static uint32 therm_sensor_sampling_size = ARR_SIZE(therm_sensor_resource);

static therm_group_polling_info_type polling_groups[ARR_SIZE(therm_sensor_resource)];

static sensor_clients sensor_client_node[ARR_SIZE(therm_sensor_resource)];

extern rex_tcb_type thermal_tcb;

/* Mask is used for serializing the group timer calls using the thermal_tcb */
static uint32 thermal_sampling_group_mask = 0x0;

static CoreMutexType *group_sensor_mutex = NULL;

static boolean sensor_client_initialized = FALSE;

/*=======================================================================

                  LOCAL FUNCTION DEFINITIONS

========================================================================*/
/**
  @brief therm_npa_log_reading

  Log sensor reading to QXDM.

  @param  :  res_index: resource index.
             state: Thermal resource state to report over QXDM.

  @return : None
*/
static void therm_npa_log_reading( unsigned int res_index,
                                   int          state )
{
  PACK(void *)            log_ptr = NULL;
  diag_therm_packet       temp_data;
  const char              *debug_log_str = NULL;
  const char              *stats_log_str = NULL;
  npa_resource_definition *resource = NPA_RESOURCE_AT_INDEX( &therm_sensor_node, res_index );
  therm_sensor_info       *sensor_info = (therm_sensor_info *)resource->data;

  memset(&temp_data, '\0', sizeof(temp_data));

  temp_data.version = 0x1;
  temp_data.num_samples = 1;

  temp_data.samples[0].subsystem_id = sensor_info->subsystem_id;
  temp_data.samples[0].sensor_id    = sensor_info->sensor_id;
  temp_data.samples[0].temp         = state;

  /* Setting log string as per resource - Make sure to provide same number of
   * format specifiers (%d, %s, ..) */
  if( THERM_NPA_LOGICAL_SENSOR == sensor_info->sensor_type )
  {
    debug_log_str = "THERM_NPA read ss_id %d %s, sensor_id %d, State %d";
    stats_log_str = "State ( %s:%d )";
  }
  /* Add checks for other type of resources as required */
  else
  {
    debug_log_str = "THERM_NPA read ss_id %d %s, sensor_id %d, TEMP %d C";
    stats_log_str = "Temperature ( %s:%d )";
  }

  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 4,
                   debug_log_str,
                   temp_data.samples[0].subsystem_id,
                   sensor_info->npa_node_str,
                   temp_data.samples[0].sensor_id,
                   temp_data.samples[0].temp);

  therm_log_printf(THERM_LOG_LEVEL_STATS, 2,
                   stats_log_str,
                   sensor_info->npa_node_str,
                   temp_data.samples[0].temp);

  /* Only send out a log packet if we have samples to report on. */
  log_ptr = log_alloc(LOG_TEMPERATURE_MONITOR_C, sizeof(temp_data) + sizeof(log_hdr_type));

  if (log_ptr)
  {
    memscpy((uint8 *)log_ptr + sizeof(log_hdr_type), sizeof(temp_data),
            &temp_data, sizeof(temp_data));
    log_commit(log_ptr);
  }
}

/**
  @brief therm_npa_adc_sync_read_complete_cb

  Read complete CB func of type DALSYSCallbackFunc.

  @param  :  pObj: Internal therm NPA adc read object data.
             dwParam: Unused
             pPayload: AdcResultType data.
             nPayloadSize: Unused

  @return : None
*/
static void therm_npa_adc_sync_read_complete_cb(
                                         therm_npa_adc_sync_read_obj_type *pObj,
                                         uint32 dwParam,
                                         void *pPayload,
                                         uint32 nPayloadSize)
{
  AdcResultType *pAdcResult = pPayload;

  DALSYS_memcpy(pObj->pAdcResult, pAdcResult, sizeof(AdcResultType));
  DALSYS_EventCtrl(pObj->hSignalEvent, DALSYS_EVENT_CTRL_TRIGGER);
}

/**
  @brief therm_npa_adc_sync_read_perform

  Peforms sync/blocking read of sensor.

  @param  :  pObj: Internal therm NPA adc read object data.
             pAdcInputProps: Input properties struct of sensor to read.
             pAdcResult: Pointer to address to place read result.

  @return : DALResult
*/
static DALResult therm_npa_adc_sync_read_perform(
                                        therm_npa_adc_sync_read_obj_type *pObj,
                                        AdcInputPropertiesType *pAdcInputProps,
                                        AdcResultType *pAdcResult)
{
  AdcRequestParametersType adcParams;
  DALResult result = DAL_ERROR;

  DALSYS_SyncEnter(pObj->hSync);
  pObj->pAdcResult = pAdcResult;
  adcParams.hEvent = pObj->hCallbackEvent;
  adcParams.nDeviceIdx = pAdcInputProps->nDeviceIdx;
  adcParams.nChannelIdx = pAdcInputProps->nChannelIdx;
  DALSYS_EventCtrl(pObj->hSignalEvent, DALSYS_EVENT_CTRL_RESET);
  result = DalAdc_RequestConversion(phADCDalDevHandle, &adcParams, NULL);
  (void)DALSYS_EventWait(pObj->hSignalEvent);
  DALSYS_SyncLeave(pObj->hSync);

  return result;
}

/**
  @brief therm_npa_adc_sync_read_obj_init

  Init DAL ADC read obj.

  @param  : phDev: DAL handle for ADC device driver.
            pObj: Internal therm NPA adc read object data.

  @return : DALResult
*/
static DALResult therm_npa_adc_sync_read_obj_init(DalDeviceHandle *phDev,
                                          therm_npa_adc_sync_read_obj_type *pObj)
{
  DALResult result;

  DALSYS_memset(pObj, 0, sizeof(therm_npa_adc_sync_read_obj_type));

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT,
                              &pObj->hCallbackEvent,
                              &pObj->callbackEventObject);

  if(result != DAL_SUCCESS)
  {
     return result;
  }

  result = DALSYS_SetupCallbackEvent(
     pObj->hCallbackEvent,
     (DALSYSCallbackFunc)therm_npa_adc_sync_read_complete_cb,
     pObj);

  if(result != DAL_SUCCESS)
  {
     return result;
  }

  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT,
     &pObj->hSignalEvent,
     &pObj->signalEventObject);

  if(result != DAL_SUCCESS)
  {
     return result;
  }

  result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                             &pObj->hSync,
                             &pObj->syncObject);
  if(result != DAL_SUCCESS)
  {
     return result;
  }

  return DAL_SUCCESS;
}

/**
  @brief therm_npa_adc_sync_read_init

  Init ADC DAL read objects.

  @param  : res_index: resource index.

  @return : TRUE or FALSE
*/
static boolean therm_npa_adc_sync_read_init(unsigned int res_index)
{
  DALResult result;
  DALBOOL ret_val    = FALSE;
  char   *input_name = NULL;
  uint32  input_name_size = 0;
  npa_resource_definition *resource = NPA_RESOURCE_AT_INDEX( &therm_sensor_node, res_index );
  therm_sensor_info       *sensor_info = (therm_sensor_info *)resource->data;
  therm_npa_adc_sync_read_type *adc_sync_read_data = (therm_npa_adc_sync_read_type *)sensor_info->sensor_data;

  do
  {
    input_name = (char*)sensor_info->sensor_input_str;
    input_name_size = strlen(sensor_info->sensor_input_str);

    result = DalAdc_GetAdcInputProperties(phADCDalDevHandle, input_name,
                                          input_name_size,
                                          &adc_sync_read_data->properties);

    if(result != DAL_SUCCESS)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                       "DalAdc_GetAdcInputProperties FAIL %d", result);
      break;
    }

    result = therm_npa_adc_sync_read_obj_init(phADCDalDevHandle,
                                              &adc_sync_read_data->readObject);
    if(result != DAL_SUCCESS)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "Init read obj FAIL %d", result);
      break;
    }
    ret_val = TRUE;
  } while (0);

  return ret_val;
}

/**
  @brief therm_npa_adc_sync_read

  This function is used to perform sensor reads.

  @param  : res_index : resource index.

  @return : Sensor reading.
*/
static npa_resource_state therm_npa_adc_sync_read(unsigned int res_index)
{
  static DALBOOL adc_init = FALSE;
  npa_resource_state therm_state = THERM_NPA_DEFAULT_TEMP;
  DALResult result;
  int32 nPhysical = 0;
  npa_resource_definition *resource = NPA_RESOURCE_AT_INDEX( &therm_sensor_node, res_index );
  therm_sensor_info       *sensor_info = (therm_sensor_info *)resource->data;
  therm_npa_adc_sync_read_type *adc_sync_read_data = (therm_npa_adc_sync_read_type *)sensor_info->sensor_data;

  if (adc_init == FALSE)
  {
    /* One time init. */
    DALResult result;
    unsigned int idx;

    DALSYS_InitMod(NULL);

    result = DAL_AdcDeviceAttach((DALDEVICEID)DALDEVICEID_ADC,
                                 &phADCDalDevHandle);
    if (result != DAL_SUCCESS)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "DAL_AdcDeviceAttach FAIL %d",
                       result);

      /* Mark all sensors as init'd and not present. */
      for (idx = 0;
           (idx < ARR_SIZE(therm_sampling_resource)) && (idx < THERM_NPA_SENSOR_MAX);
           idx++)
      {
        npa_resource_definition *res = NPA_RESOURCE_AT_INDEX( &therm_sensor_node, idx );
        therm_sensor_info       *sensor = (therm_sensor_info *)res->data;

        if (THERM_NPA_HKADC_SENSOR == sensor->sensor_type)
        {
          therm_npa_adc_sync_read_type *adc_data =
            (therm_npa_adc_sync_read_type *)sensor->sensor_data;
          adc_data->itemInit = TRUE;
          adc_data->sensorPresent = FALSE;;
        }
      }
    }

    adc_init = TRUE;
  }

  if (adc_sync_read_data->itemInit == FALSE)
  {
    /* Init ADC sync read object data for each signal. */
    adc_sync_read_data->sensorPresent =
      therm_npa_adc_sync_read_init(res_index);
    adc_sync_read_data->itemInit = TRUE;
  }

  do
  {
    if(adc_sync_read_data->sensorPresent == FALSE)
    {
      break;
    }

    result = therm_npa_adc_sync_read_perform(&adc_sync_read_data->readObject,
                                             &adc_sync_read_data->properties,
                                             &adc_sync_read_data->readResult);

    if(result != DAL_SUCCESS)
    {
      break;
    }

    /* Convert to whole units */
    nPhysical = adc_sync_read_data->readResult.nPhysical /
                ((int)sensor_info->unit_per_whole);

    /* Convert units to K. */
    therm_state =  (npa_resource_state)THERM_C_TO_K(nPhysical);

  } while (0);
  return therm_state;
}

/**
  @brief therm_npa_tsens_sync_read

  This function is used to perform sensor reads.

  @param  : res_index : resource index.

  @return : Sensor reading.
*/
static npa_resource_state therm_npa_tsens_sync_read(unsigned int res_index)
{
  static boolean init = FALSE;
  npa_resource_definition *resource = NPA_RESOURCE_AT_INDEX( &therm_sensor_node, res_index );
  therm_sensor_info       *sensor_info = (therm_sensor_info *)resource->data;
  therm_npa_tsens_sync_read_type *tsens_sync_read_data =
    (therm_npa_tsens_sync_read_type *)sensor_info->sensor_data;
  TsensTempType tsensTemp = {0};
  npa_resource_state therm_state = THERM_NPA_DEFAULT_TEMP;

  if (!init)
  {
    DALSYS_InitMod(NULL);

    if (DAL_SUCCESS !=
          DAL_TsensDeviceAttach(DALDEVICEID_TSENS, &phTSENSDALDevHandle))
    {
      uint32 idx;
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "DAL_TsensDeviceAttach FAIL");

      /* Mark all TSENS sensors as init'd and not present. */
      for (idx = 0;
           (idx < ARR_SIZE(therm_sampling_resource)) && (idx < THERM_NPA_SENSOR_MAX);
           idx++)
      {
        npa_resource_definition *res = NPA_RESOURCE_AT_INDEX( &therm_sensor_node, idx );
        therm_sensor_info       *sensor = (therm_sensor_info *)res->data;

        if (THERM_NPA_TSENS_SENSOR == sensor->sensor_type)
        {
          therm_npa_tsens_sync_read_type *tsens_data =
            (therm_npa_tsens_sync_read_type *)sensor->sensor_data;
          tsens_data->itemInit = TRUE;
          tsens_data->sensorPresent = FALSE;;
        }
      }
    }
    else
    {
      if (DAL_SUCCESS != DalTsens_GetNumSensors(phTSENSDALDevHandle,
                                                &numTSensors))
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                         "FAILED to get number of TSENS sensors");
      }
    }
    init = TRUE;
  }

  if (tsens_sync_read_data->itemInit == FALSE)
  {
    /* Init TSENS */
    if (sensor_info->sensor_id < numTSensors)
    {
      tsens_sync_read_data->sensorPresent = TRUE;
      tsens_sync_read_data->itemInit = TRUE;
    }
  }

  do
  {
    if (tsens_sync_read_data->sensorPresent == FALSE || phTSENSDALDevHandle == NULL)
    {
      break;
    }

    if (DalTsens_GetTemp(phTSENSDALDevHandle, sensor_info->sensor_id, &tsensTemp) !=
        DAL_SUCCESS)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Get Temp failed",
                       __func__);
      break;
    }

    /* Convert units to K. */
    therm_state =  (npa_resource_state)THERM_C_TO_K(tsensTemp.nDegC);

  } while (0);
  return therm_state;
}

#ifdef QFE_PA
/**
  @brief mcpm_rf_therm_avail_cb

  Notify once MCPM node for rf reads is available.

  @return : void
*/
static void mcpm_rf_therm_avail_cb(void         *context,
                                   unsigned int  event_type,
                                   void         *data,
                                   unsigned int  data_size )
{
  qfe_pa0_read_data.mcpm_query_clnt =
    npa_create_query_handle(MCPM_RF_THERM_NPA_RESRC_NAME);

  if (qfe_pa0_read_data.mcpm_query_clnt == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Fatal Error - Cannot create query clnt",
                   __func__);
    CORE_VERIFY(NULL);
  }
}

/**
  @brief qfe_query_thermistor_read_init

  This function sets up total thermistor count and enumeration of the thermistor list

  @param  : qfe_read_data: Contains all QFE sensor data

  @return : True if init succeeded or false otherwise
*/
static boolean qfe_query_thermistor_read_init(therm_npa_rf_read_type *qfe_read_data)
{
  boolean ret_val = FALSE;
  uint8   cnt;

  do
  {
    if (qfe_read_data->mcpm_query_clnt == NULL)
    {
      therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "%s: MCPM node not available",
                       __func__);
      break;
    }

    /* Get available thermistor count */
    ret_val = rfm_get_thermistor_count(&(qfe_read_data->rf_therm_cnt));

    if (!ret_val)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Getting thermistors count failed",
                       __func__);
      break;
    }

    if (qfe_read_data->rf_therm_cnt > MAX_QFE_THERMISTOR_COUNT)
    {
      /* The device has more thermistors than we have allocated space for.
         Must reduce the total number of qfe thermistors to enumerate through */
      qfe_read_data->rf_therm_cnt = MAX_QFE_THERMISTOR_COUNT;
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 2,
                       "%s: Device has more thermistors than allocated space. "
                       "total qfe therms = %d", __func__,
                       MAX_QFE_THERMISTOR_COUNT);
    }

    /* Don't want to override total available cnt with active cnt of
       thermistors returned. */
    cnt = qfe_read_data->rf_therm_cnt;
    ret_val = rfm_enumerate_thermistors(&cnt, qfe_read_data->rf_therm_list);
    if (!ret_val)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                       "%s: Enumerating thermistors failed", __func__);
      break;
    }

    therm_log_printf(THERM_LOG_LEVEL_REF, 1, "QFE cnt:%d",
                     qfe_read_data->rf_therm_cnt);

    mask_init |= QFE_BIT_MASK;
    ret_val = TRUE;

  } while(0);

  return ret_val;
}

/**
  @brief therm_qfe_calc_time_delta_ms

  This function is used to calculate delta between
  to tick points and convert to ms.

  @param  :  end : End meas qtimer tick.
             start : Start meas qtimer tick.

  @return : Delta in ms.
*/
static uint64 therm_qfe_calc_time_delta_ms(uint64 start, uint64 end)
{
  DalTimetickTime64Type delta = 0;

  do
  {
    if (start > end)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "QFE Calc time invalid start",
                       __func__);
      break;
    }

    if (DAL_SUCCESS != DalTimetick_CvtFromTimetick64(phTimetick,
                              (DalTimetickTime64Type)(end - start),
                                                     T_MSEC,
                                                     &delta))
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Cvt Time Fail");
      break;
    }
  } while (FALSE);
  return (uint64)delta;
}

/**
  @brief therm_qfe_read_time_valid

  This function is used to determine if a reading is recent
  enough.

  @param  :  now : Current qtimer tick.
             read :  Reading qtimer tick.

  @return : Sensor reading in DegC.
*/
static boolean therm_qfe_read_time_valid(uint64 now, uint64 read)
{
  boolean bret_val = FALSE;
  uint64 delta = 0;

  do
  {
    if (read == RFM_INVALID_THERM_READ_TIME_STAMP)
    {
      therm_log_printf(THERM_LOG_LEVEL_DEBUG, 0, "TS never set");
      break;
    }

    if (read > now)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid time",
                       __func__);
      break;
    }

    delta = therm_qfe_calc_time_delta_ms(read, now);

    if (delta <= THERM_MAX_QFE_SENSOR_AGE)
    {
      bret_val = TRUE;
      break;
    }
    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "TS delta too large %d(ms)",
                     (uint32)delta);
  } while (FALSE);
  return bret_val;
}

/**
  @brief query_rf_temp

  This function is used to query all available RF sensors.

  @param  : qfe_read_data : qfe resource information.

  @return : TRUE if any available sensor read succeeded.
*/
static boolean query_rf_temp(therm_npa_rf_read_type *qfe_read_data)
{
  uint8 idx = 0;
  uint8 cnt = qfe_read_data->rf_therm_cnt;
  npa_query_type result;

  boolean ret_val = FALSE;

  for (idx = 0; idx < cnt; idx++)
  {
    memset(&result, 0x0, sizeof(npa_query_type));
    result.data.reference = &(qfe_read_data->rf_therm_list[idx]);
    if (npa_query(qfe_read_data->mcpm_query_clnt, NPA_QUERY_CURRENT_STATE,
                  &result) == NPA_QUERY_SUCCESS)
    {
      ret_val = TRUE;
    }
    else
    {
      therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2,
                       "%s %d",__func__, idx);
    }
  }

  return ret_val;
}

/**
  @brief get_rf_max_valid_temp

  This function is used to extract the max temp value of a
  certain type and is valid within a certain window of time.

  @param  : qfe_read_data : qfe resource information.
            type :  WTR or NON_WTR (PA)

  @return : Sensor reading in DegC.
*/
static int get_rf_max_valid_temp(therm_npa_rf_read_type *qfe_read_data,
                                 rfm_therm_device_type_enum_type type)
{
  uint8 idx = 0;
  int32 max_temp = THERM_K_TO_C(0);
  uint8 cnt = qfe_read_data->rf_therm_cnt;
  uint64 qtime = 0;

  qtime = CoreTimetick_Get64();

  /* Attempt to optimize re-queries */
  if ((qfe_read_data->rf_query_valid == FALSE)  ||
      (therm_qfe_calc_time_delta_ms(qfe_read_data->rf_query_time, qtime)
       >= THERM_MIN_QFE_QUERY_AGE))
  {
    qfe_read_data->rf_query_valid = query_rf_temp(qfe_read_data);
    qtime = qfe_read_data->rf_query_time = CoreTimetick_Get64();
  }
  else
  {
    qtime = CoreTimetick_Get64();
  }

  if (qfe_read_data->rf_query_valid)
  {
    for (idx = 0; idx < cnt; idx++)
    {
      rfm_thermistor_type *therm = &qfe_read_data->rf_therm_list[idx];

      if (therm->therm_device_type != type)
      {
        continue;
      }

      if (therm->therm_result.therm_read_deg_C ==
          (int16)RFM_INVALID_TEMPERATURE)
      {
        therm_log_printf(THERM_LOG_LEVEL_DEBUG, 3,
                         "Invalid TEMP QFE idx %d type %d TEMP %d",
                         idx, therm->therm_device_type,
                         therm->therm_result.therm_read_deg_C);
        continue;
      }

      if (!(therm_qfe_read_time_valid(qtime,
                                    therm->therm_result.therm_read_time_stamp)))
      {
        therm_log_printf(THERM_LOG_LEVEL_DEBUG, 3,
                         "Invalid TS QFE idx %d type %d TEMP %d",
                         idx, therm->therm_device_type,
                         therm->therm_result.therm_read_deg_C);
        continue;
      }

      /* Queried thermistor successfully */
      therm_log_printf(THERM_LOG_LEVEL_DEBUG, 3,
                       "QFE idx %d type %d TEMP %d",
                       idx, therm->therm_device_type,
                       therm->therm_result.therm_read_deg_C);
      max_temp = MAX( therm->therm_result.therm_read_deg_C, max_temp);
    }
  }
  else
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Query Failed", __func__);
  }

  return max_temp;
}

/**
  @brief qfe_query_thermistor_read

  This function is used to check the qfe thermistors temperature readings.
  If the thermistors are unreadable, then the temperature of the Tsens is used
  for the output.

  @param  : qfe_read_data : qfe resource information.
            type :  WTR or NON_WTR (PA)

  @return : Sensor reading.
*/
static npa_resource_state qfe_query_thermistor_read(therm_npa_rf_read_type *qfe_read_data,
                                                    rfm_therm_device_type_enum_type type)
{
  static boolean init = FALSE;
  npa_resource_state therm_state = 0;  /* 0 Kelvin is -273 C */
  int32 max_temp = THERM_K_TO_C(0);
  //uint64 qtime = 0;

  if (!init)
  {
    init = qfe_query_thermistor_read_init(qfe_read_data);
  }

  if(TEST_BIT_OF_MASK( QFE_BIT_MASK, mask_init) != 0)
  {
    /* Read QFE sensor if possible */
    mask_temp_available |= QFE_BIT_MASK;
    max_temp = get_rf_max_valid_temp(qfe_read_data, type);
  }
  therm_state = (npa_resource_state)THERM_C_TO_K((int32)max_temp);
  return therm_state;
}
#endif

/**
  @brief therm_sensor_update_fcn

  This function is invoked by the /therm/sensor/<sensor>
  resources to service requests.

  @param  : resource: A dynamic system element that work requests can be made against.
            client: The handle to the clients registered to the resource.

  @return : None.
*/
static npa_resource_state therm_sensor_update_fcn( npa_resource *resource,
                                                   npa_client_handle client )
{
  npa_resource_state temperature = THERM_NPA_DEFAULT_TEMP;
  npa_resource_definition *definition = resource->definition;
  therm_sensor_info       *sensor_info = (therm_sensor_info *)definition->data;

  /* Ensure that the client is not NULL */
  CORE_VERIFY(client);

  switch (client->type)
  {
    case NPA_CLIENT_IMPULSE:
      if (THERM_NPA_HKADC_SENSOR == sensor_info->sensor_type)
      {
        therm_npa_adc_sync_read_type *adc_sync_read_data =
          (therm_npa_adc_sync_read_type *)sensor_info->sensor_data;

        temperature = therm_npa_adc_sync_read(resource->index);
        if (adc_sync_read_data->sensorPresent)
        {
          therm_npa_log_reading(resource->index, THERM_K_TO_C(temperature));
        }
      }
      else if (THERM_NPA_TSENS_SENSOR == sensor_info->sensor_type)
      {
        therm_npa_tsens_sync_read_type *tsens_sync_read_data =
          (therm_npa_tsens_sync_read_type *)sensor_info->sensor_data;

        temperature = therm_npa_tsens_sync_read(resource->index);
        if (tsens_sync_read_data->sensorPresent)
        {
          therm_npa_log_reading(resource->index, THERM_K_TO_C(temperature));
        }
      }
      #ifdef QFE_PA
      else if (THERM_NPA_QFE_SENSOR == sensor_info->sensor_type)
      {
        if(mm_is_gsm_active_for_thermal_read())
        {
          therm_log_printf(THERM_LOG_LEVEL_DEBUG,0, "GSM is active that's why not quering RF thermistor");
        }

        else if (ftm_get_mode() != FTM_MODE && !(mm_is_gsm_active_for_thermal_read())) 
        {
          rfm_therm_device_type_enum_type type = RFM_THERM_DEVICE_TYPE_WTR;
          therm_npa_rf_read_type *qfe_read_data =
          (therm_npa_rf_read_type *)sensor_info->sensor_data;

          if (sensor_info->subsystem_id == PA_THERM_SS)
          {
            type = RFM_THERM_DEVICE_TYPE_NON_WTR;
          }

          temperature = qfe_query_thermistor_read(qfe_read_data, type);
          if (mask_temp_available)
          {
            therm_npa_log_reading(resource->index, THERM_K_TO_C(temperature));
          }
        }
      }
      #endif
      break;

    default:
      break;
  } /* End of switch */

  return temperature;
}

/**
  @brief therm_logical_max_update_fcn

  This function will be invoked when an NPA request is issued to
  /therm/sensor/modem_bcl_warning resource.

  @param : resource: NPA resource against which request was isssued.
           client: The handle to the client which issued this request.

  @return : State of the resource after request is applied.
 */
static npa_resource_state therm_logical_max_update_fcn( npa_resource *resource,
                                                        npa_client_handle client )
{
  npa_resource_state new_state = npa_max_plugin.update_fcn(resource, client);

  /* Log the state of the resource */
  therm_npa_log_reading( resource->index, new_state );

  return new_state;
}

/**
  @brief therm_node_driver_fcn

  Sensor node driver function.  Nothing to update.

  @param  : resource: A dynamic system element that work requests can be made against.
            client: The handle to the clients registered to the
            resource.
            state: Update function state.

  @return : Resource state.

*/
static npa_resource_state therm_node_driver_fcn ( npa_resource *resource,
                                                  npa_client *client,
                                                  npa_resource_state state )
{
  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    // The driver function will be called with this client *before* it is
    // made publicly available, so you can place any initialization you
    // need here.

    // The value of state passed here is provided by the
    // npa_define_node function.
    therm_log_printf(THERM_LOG_LEVEL_REF, 2, "%s:%&d",
                   resource->definition->name,
                   &resource->active_state);
  }
  else
  {
    // Set the resource based on the value state.
  }

  return state;
}

/**
  @brief remove_from_group_fcn

  Remove from group function.

  @param  : resource_id: Resource id of the resource that was just used in the
                                group organization function

  @return : void

*/
static void remove_from_group_fcn(uint8 resource_id)
{
  /* Check to see if we need to remove an unused group */
  uint8 previous_group = sensor_client_node[resource_id].groupId;
  const uint8 polling_group_count =
     (uint8)(sizeof(polling_groups) / sizeof(polling_groups[0]));

  if ((previous_group != therm_sensor_sampling_size) &&
      (previous_group < polling_group_count))    /* KW */
  {
    polling_groups[previous_group].groupSensorCount--;

    /* If group has no more sensors active on it, disable it */
    if (polling_groups[previous_group].groupSensorCount == 0)
    {
      polling_groups[previous_group].groupInUse = FALSE;
    }
    sensor_client_node[resource_id].groupId = therm_sensor_sampling_size;
  }
}

/**
  @brief group_organization_fcn

  Group organization function. Figures out the polling group system.
  If a resource needs to be in a different group and removing unused groups.

  @param  : resource: A dynamic system element that work requests can be made against.
            state: State passed in from update function.
            group_id_return: Group id of the timing group where the sensor is located
            resource_id_return: Resource id of the resource that was just used in the
                                group organization function

  @return : boolean: True if new group was created
                     False if group is already available

*/
static boolean group_organization_fcn( npa_resource *resource,
                                    npa_resource_state state,
                                    uint8 *group_id_return,
                                    uint8 *resource_id_return)
{
  uint8 i;
  boolean group_available = FALSE;
  uint8 group_id = 0;
  uint8 resource_id = resource->index;
  uint8 first_group_available = therm_sensor_sampling_size;
  boolean new_group = FALSE;
  const uint8 polling_group_count =
     (uint8)(sizeof(polling_groups) / sizeof(polling_groups[0]));

  /* Remove from previous group if it is in a previous group */
  remove_from_group_fcn(resource_id);

  for (i = 0; i < therm_sensor_sampling_size; i++)
  {
    /* Check to see if there is a group available at this polling rate */
    if (polling_groups[i].groupInUse && polling_groups[i].groupSamplingTime == state)
    {
      group_available = TRUE;
      group_id = i;
    }

    if (!polling_groups[i].groupInUse)
    {
      /* Set the index of the first available group slot */
      first_group_available = i;
    }
  }

  /* Must create a new sampling group if we couldn't find an active one */
  if (!group_available)
  {
    /* Set this indexed group as the new polling group for the given state */
    if (first_group_available < polling_group_count)
    {
      group_id = first_group_available;
      polling_groups[group_id].groupInUse = TRUE;
      polling_groups[group_id].groupSamplingTime = state;
      new_group = TRUE;
    }
    else
    {
      /* KW - No group found */
      group_id = 0;
      CORE_VERIFY(0);
    }
  }

  /* Set the resource active for group id */
  sensor_client_node[resource_id].groupId = group_id;
  polling_groups[group_id].groupSensorCount++;
  *group_id_return = group_id;
  *resource_id_return = resource_id;

  return new_group;
}

/**
  @brief therm_sampling_node_driver_fcn

  Sampling node driver function.  Perform init, but no actual
  driver update.

  @param  : resource: A dynamic system element that work requests can be made against.
            client: The handle to the clients registered to the
            resource.
            state: State passed in from update function.

  @return : Resource state.

*/
static npa_resource_state therm_sampling_node_driver_fcn( npa_resource *resource,
                                                          npa_client *client,
                                                          npa_resource_state state )
{
  unsigned int i;
  uint8 group_id;
  uint8 resource_id;
  boolean new_group = FALSE;

  Core_MutexLock(group_sensor_mutex);

  new_group = group_organization_fcn(resource, state, &group_id, &resource_id);

  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    // The driver function will be called with this client *before* it is
    // made publicly available, so you can place any initialization you
    // need here.

    if (!sensor_client_initialized)
    {
      for (i = 0; i < ARR_SIZE(sensor_client_node); i++)
      {
        /* Create impulse client for sampling node to trigger a sensor read */
        sensor_client_node[i].sensorClientHandle =  npa_create_sync_client( therm_sensor_resource[i].name,
                                                         "Sampling Sensor Handle",
                                                         NPA_CLIENT_IMPULSE);
        CORE_VERIFY(sensor_client_node[i].sensorClientHandle);

        /* Define all the timers */
        timer_def( &(polling_groups[i].groupSamplingTimer), NULL, NULL,
               NULL, therm_serial_timer_cb, (timer_cb_data_type)i);
      }
      sensor_client_initialized = TRUE;
    }
  }
  else
  {
    // Set the resource based on the value state.
  }

  if (new_group)
  {
      /* Start the sensor sampling polling timer. */
      timer_set( &(polling_groups[group_id].groupSamplingTimer), polling_groups[group_id].groupSamplingTime,
        0, T_MSEC);
      therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2, "New group %d created with sampling time %d",
        group_id,
        polling_groups[group_id].groupSamplingTime);
  }

  Core_MutexUnlock(group_sensor_mutex);

  return state;
}

/*=======================================================================

                 PUBLIC FUNCTION DEFINITIONS

========================================================================*/
/**
  @brief therm_serial_timer_cb

  Used to signal the thermal_tcb to serialize timers

  @param  : group_id: sensor timer group id

  @return : None
*/
void therm_serial_timer_cb(timer_cb_data_type group_id)
{
    thermal_sampling_group_mask |= (0x1 << group_id);
    rex_set_sigs(&thermal_tcb, THERM_SAMPLING_LOOP_TIMER_SIG);
}

/**
  @brief therm_sampling_timer_cb

  Used to trigger sensor read update.

  @param  : void

  @return : None.
*/
void therm_sampling_timer_cb(void)
{
  unsigned int i, j;
  unsigned int impulses_given = 0;
  uint8 group_id;

  for (i = 0; i < therm_sensor_sampling_size; i++)
  {
    impulses_given = 0;
    if ((0x1 << i) & thermal_sampling_group_mask)
    {
      group_id = i;
      for ( j = 0; j < therm_sensor_sampling_size; j++)
      {
        if (sensor_client_node[j].groupId == group_id)
        {
          impulses_given++;
          npa_issue_impulse_request(sensor_client_node[j].sensorClientHandle);
        }
      }

      if (impulses_given > 0)
      {
        /* Restart sampling timer. */
        timer_set( &(polling_groups[group_id].groupSamplingTimer), polling_groups[group_id].groupSamplingTime,
          0, T_MSEC);
      }
      thermal_sampling_group_mask &= ~(0x1 << group_id);
    }
  }
}

/**
  @brief therm_npa_sensor_init

  Thermal NPA sensor init.

 */
void therm_npa_sensor_init(void)
{
  uint8 i;

  for (i = 0; i < therm_sensor_sampling_size; i++)
  {
    polling_groups[i].groupInUse = FALSE;
    polling_groups[i].groupSensorCount = 0;
    sensor_client_node[i].groupId = therm_sensor_sampling_size;
  }

  CORE_VERIFY_PTR(group_sensor_mutex = Core_MutexCreate(CORE_MUTEXATTR_DEFAULT));

  CORE_VERIFY(ARR_SIZE(therm_sensor_resource) == ARR_SIZE(therm_sampling_resource));

  CORE_VERIFY(ARR_SIZE(therm_sampling_resource) == ARR_SIZE(therm_sampling_deps));

  /* Define nodes */
  npa_define_node( &therm_sensor_node, initial_sensor_state, NULL );

  npa_define_node( &therm_sampling_node, initial_sampling_state, NULL );
#ifdef QFE_PA
  DalTimetick_Attach("SystemTimer", &phTimetick);
  CORE_VERIFY_PTR(phTimetick);

  npa_resource_available_cb(MCPM_RF_THERM_NPA_RESRC_NAME,
                            mcpm_rf_therm_avail_cb, NULL);
#endif
}