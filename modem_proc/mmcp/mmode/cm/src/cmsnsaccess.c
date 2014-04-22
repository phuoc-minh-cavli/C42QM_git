
#include "Cmsnsaccess.h"
#include "comdef.h"
#include  "cmidbg.h"
#include  "cmtaski.h"

#ifdef FEATURE_REF_TRACKER 

#include "qmi_idl_lib.h"
#include  "sns_client_api_v01.h"
#include  "sns_std_type.pb.h"
#endif
#ifdef FEATURE_MMODE_TX_21
#include "cmsnsmgr.h"
#include "cmsnsutil.h"
#endif

/*=============================================================================
  Static Variable Definitions
  ===========================================================================*/

boolean cm_if_qmi_init = FALSE;
const uint32 cm_init_svr_discvery_handle = 0;
uint64_t sample_time_stamp = 0;

#ifdef FEATURE_REF_TRACKER 
#define CM_SNS_MAX_SERIAL_LEN 20

// supported on chiltepin this time
cm_sns_id_e_type current_sensor_type = CM_SNS_TYPE_ENV_SENSOR_MAX;
cm_sns_id_e_type write_registry_current_sensor_type = CM_SNS_TYPE_MAX;
cm_sns_id_e_type read_registry_current_sensor_type = CM_SNS_TYPE_MAX;
static uint8 config_write_counter;
static uint8 config_read_counter;
cm_sns_registry_rsp_s_type *reg_read = NULL;
boolean registry_write =FALSE ;
boolean registry_read = FALSE;

/* QMI Client handle created within cm_create_client_conn(), and used to send
 * a request to the Sensor service */
static qmi_client_type qmi_sensors_handle = NULL;
static qmi_client_type curr_qmi_sensors_handle = NULL;



/** new change ***/
static qmi_client_type non_rel_client_qmi_handle = NULL;
static qmi_client_type rel_client_qmi_handle = NULL;
/** change**/

/* The SUID of the SUID Lookup Sensor is a constant value, and published within
 * sns_suid.proto. */
static sns_std_suid suid_lookup = sns_suid_sensor_init_default;

/* SUID for the accel Sensor, as received from the SUID Lookup Sensor */
static sns_std_suid curr_sns_suid = { .suid_low = 0, .suid_high = 0 };
static sns_std_suid qmi_temp_suid[2]={0};

static qmi_cci_os_signal_type os_params = {0};
static qmi_idl_service_object_type service_obj ;
static qmi_client_type notifier_handle;

uint16 cm_sns_als_stream_type = 0;
boolean cm_sns_als_stream_disable = FALSE;
char cm_sns_temperature_sn[CM_SNS_MAX_SERIAL_LEN];
int32_t cm_sns_current_attr_id = 0;
int8 attr_len;
/** new change **/
typedef struct cm_sns_comm_data
{
  cm_sns_id_e_type sns_id;
  sns_std_suid    sns_suid ;

}cm_sns_basic_comm_data;

cm_sns_basic_comm_data sns_db_global[CM_SNS_TYPE_MAX] = {0};
/** change **/
/*=============================================================================
  Static Function Definitions
  ===========================================================================*/
/*   below def can be removed while merging to main line*/

extern void cm_sns_update_sample_buffer_per_sid(cm_sns_id_e_type sensor_id, float sample_arr[], int len, uint64 timestamp);
extern void cm_sns_update_motion_rsp(cm_sns_motion_event_e_type event);
extern void cm_sns_update_mcd_rsp(cm_sns_motion_event_e_type event, uint32 psm_time);
#ifndef FEATURE_MMODE_TX_21
extern void cm_sns_get_fmd_config(byte *state,uint16 *pres_thres,byte *accel_freq,byte *accel_sample_len);

extern boolean cm_sns_get_tilt_config(

  cm_sns_tilt_tracker_config_transportation_type *transportation_type,
  boolean *has_redo_gravity_init,
  boolean *redo_gravity_init, 
  boolean *has_iteration_delay,
  uint32 *iteration_delay,
  boolean *has_max_init_trial_num,
  uint8 *max_init_trial_num,
  boolean *has_max_update_trial_num,    
  uint8 *max_update_trial_num, 
  boolean *has_init_gravity_threshold, 
  float *init_gravity_threshold,
  boolean *has_update_gravity_threshold,
  float *update_gravity_threshold
);
extern boolean cm_sns_get_tilt_gravity_vector(boolean *init_success,  float *gravity_init_vector_x,float *gravity_init_vector_y,float *gravity_init_vector_z);
extern void cm_sns_set_tilt_gravity_vector(boolean init_success,  float gravity_init_vector_x,float gravity_init_vector_y,float gravity_init_vector_z);

#endif
extern void cmsns_snsmod_send_sns_qmi_cmd_rsp(void *user_handle,
                                      unsigned int msg_id,
                                      void *ind_buf,
                                      unsigned int ind_buf_len);


void cm_disable_sns_stream_by_sensor_id(cm_sns_id_e_type sns_id);

boolean cm_is_non_release_client(cm_sns_id_e_type sensor_id)
{
  boolean res = FALSE;
  switch(sensor_id)
  {
    case CM_SNS_TYPE_ALS:
	case CM_SNS_TYPE_FALL_DETECT:
	case CM_SNS_TYPE_MOTION_DETECT:
	  res = true;
	  break;
	default:
		break;
  }
  
  return res;
}

/*===========================================================================
FUNCTION cm_sns_find_sns_by_suid

DESCRIPTION
 it will return sensor id as per SUID

DEPENDENCIES
none

RETURN VALUE
cm_sns_id_e_type

SIDE EFFECTS
none

===========================================================================*/

void cm_sns_reset_suid_by_sensor_id(cm_sns_id_e_type sns_id)
{
	sns_db_global[sns_id].sns_id = 0;
	sns_db_global[sns_id].sns_suid.suid_low  = 0;
    sns_db_global[sns_id].sns_suid.suid_high = 0;

}


/*===========================================================================
FUNCTION cm_sns_find_sns_by_suid

DESCRIPTION
 it will return sensor id as per SUID

DEPENDENCIES
none

RETURN VALUE
cm_sns_id_e_type

SIDE EFFECTS
none

===========================================================================*/

cm_sns_id_e_type cm_sns_find_sns_by_suid(sns_std_suid    sns_suid)
{

  int idx = 0;
  int found = 0;
  cm_sns_id_e_type sns_id = CM_SNS_TYPE_NONE;
  for(idx = CM_SNS_TYPE_ALS;idx<CM_SNS_TYPE_MAX;idx++)
  {
    if(sns_db_global[idx].sns_suid.suid_low == sns_suid.suid_low && 
		sns_db_global[idx].sns_suid.suid_high == sns_suid.suid_high)
    {
      sns_id = sns_db_global[idx].sns_id;
	  CM_MSG_HIGH_3("cm_sns_find_sns_by_suid  sns_id low =0x%x sns_id hg = 0x %x",sns_id,sns_suid.suid_low,sns_suid.suid_high);
	  found = 1;
	  break;
    }
  }
  if(found == 0 )
  {
	 if(qmi_temp_suid[0].suid_low == sns_suid.suid_low && 
		qmi_temp_suid[0].suid_high == sns_suid.suid_high)
	{
	  sns_id = CM_SNS_TYPE_ALS;
	  CM_MSG_HIGH_0(" match with ALS suid 0");
	}
    if(qmi_temp_suid[1].suid_low == sns_suid.suid_low && 
	  qmi_temp_suid[1].suid_high == sns_suid.suid_high)
	{
	  CM_MSG_HIGH_0(" match with ALS suid 1");
	  sns_id = CM_SNS_TYPE_ALS;
	}
	  
  }
  
  
  return sns_id;
}

/*===========================================================================
FUNCTION cm_sns_update_suid

DESCRIPTION
    update global suid as per sensor id

DEPENDENCIES
none

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
void cm_sns_update_suid(sns_std_suid    sns_suid,cm_sns_id_e_type sns_id )
{

   if(sns_id > CM_SNS_TYPE_NONE && sns_id < CM_SNS_TYPE_MAX)
   {
     sns_db_global[sns_id].sns_suid.suid_low = sns_suid.suid_low;
	 sns_db_global[sns_id].sns_suid.suid_high = sns_suid.suid_high;
	 sns_db_global[sns_id].sns_id = sns_id;
	 CM_MSG_HIGH_1(" cm_sns_update_suid update for sns id = %d ",sns_db_global[sns_id].sns_id );
   }

}



static qmi_client_type cm_sns_get_current_qmi_handle(sns_std_suid sns_suid)
{
   return qmi_sensors_handle;
#if 0
  cm_sns_id_e_type sns_id =  cm_sns_find_sns_by_suid(sns_suid);

  if(cm_is_non_release_client(sns_id) == TRUE )
  {
    return non_rel_client_qmi_handle;
  }
  else
  {
    return rel_client_qmi_handle;
  }
#endif
}

static qmi_client_type cm_sns_get_current_qmi_handle_by_sensor(cm_sns_id_e_type sns_id)
{
   return qmi_sensors_handle;
#if 0
  if(cm_is_non_release_client(sns_id) == TRUE )
  {
    return non_rel_client_qmi_handle;
  }
  else
  {
    return rel_client_qmi_handle;
  }
#endif
}


/*===========================================================================
FUNCTION cm_get_msg_id_by_sensor_type

DESCRIPTION
 it will return message type as per sensor id
 for message id refer to proto file

DEPENDENCIES
none

RETURN VALUE
sns_std_sensor_msgid

SIDE EFFECTS
none

===========================================================================*/

sns_std_sensor_msgid cm_get_msg_id_by_sensor_type(cm_sns_id_e_type type)
{
  sns_std_sensor_msgid ret = 0;
  switch(type)
  {
 /* case CM_SNS_TYPE_NONE:
    case CM_SNS_TYPE_GYRO:*/
    case CM_SNS_TYPE_PRESSURE:  
    case CM_SNS_TYPE_ACCEL:	
      ret = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
      break;    
    case CM_SNS_TYPE_HUMIDITY:
    case CM_SNS_TYPE_SMD:
	case CM_SNS_TYPE_MOTION_DETECT:
    case CM_SNS_TYPE_TEMPERATURE:
	case CM_SNS_TYPE_FMD:
	case CM_SNS_TYPE_FALL_DETECT:
	case CM_SNS_TYPE_TILT:
 /*   case CM_SNS_TYPE_PROXIMITY:*/
      ret = SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG;
      break;    
	case CM_SNS_TYPE_ALS:
	  if (cm_sns_als_stream_type == SNS_STD_SENSOR_STREAM_TYPE_STREAMING )
	  {
		  ret = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
		  cm_sns_als_stream_disable = TRUE;
	  }
	  else 
	  {
		ret = SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG; 
	  }
	  break;
    default :
      break;
  }
  return ret;

}

/*===========================================================================
FUNCTION cm_get_sensor_name_by_sensor_type

DESCRIPTION
  it will return the name of sensor as per type
  for name refer to proto type

DEPENDENCIES
none

RETURN VALUE
pointer to type char

SIDE EFFECTS
none

===========================================================================*/

char * cm_get_sensor_name_by_sensor_type(cm_sns_id_e_type type)
{
  char *name = NULL;
  switch(type)
  {

/*    case CM_SNS_TYPE_ACCELEROMETER:
      name = "accel";
      break;
    case CM_SNS_TYPE_GYRO:
      name="gyro";
      break;*/
    case CM_SNS_TYPE_PRESSURE:
      name = "pressure";
      break;
    case CM_SNS_TYPE_TEMPERATURE:
      name = "ambient_temperature";
      break;
    case CM_SNS_TYPE_HUMIDITY:
      name ="humidity";
      break;
    case CM_SNS_TYPE_ALS:
      name = "ambient_light";
      break;
    case CM_SNS_TYPE_SMD:
      name = "sig_motion";
      break;
	case CM_SNS_TYPE_MOTION_DETECT:
	  name = "motion_detect";
	  break;
#ifdef FEATURE_MMODE_TX_21
	  /**  fill form proto file */
	case CM_SNS_TYPE_FALL_DETECT:
	  name = "freefall_detect";
	  break;
	case CM_SNS_TYPE_TILT:
	  name = "tilt_tracker"; /* change the value as per proto here only demo here*/
	  break;

      	case CM_SNS_TYPE_FMD:
	  name = "flight_mode";
          break;
	  case CM_SNS_TYPE_ACCEL:
	  name = "accel";
	  break;
        case CM_SNS_TYPE_MCD:
	  name = "mcd_tracker"; 
         break;
        case CM_SNS_TYPE_REGISTRY:
	  name = "registry";
         break;
#endif
/*    case CM_SNS_TYPE_PROXIMITY:
      name = "proximity";
      break; */
    
    default:
      break;
  }
  
  return name;
}


/**
 * Copy an already encoded payload into a message field.  For the purpose of
 * this function, an encoded submessage is identical to a character string.
 */

 /*===========================================================================
 FUNCTION cm_encode_payload

 DESCRIPTION
  it will encode the payload

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_encode_payload(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
  pb_buffer_arg *info = (pb_buffer_arg*)*arg;

  if(!pb_encode_tag_for_field(stream, field))
  {
    return false;
  }
  if(!pb_encode_string(stream, info->buf, info->buf_len))
  {
    return false;
  }

  return true;
}

/**
 * Decode a string of bytes from a field. This function is intended to be
 * used as a callback function during the decode process.
 *
 * @see pb_callback_s::decode
 */

 /*===========================================================================
 FUNCTION cm_decode_payload

 DESCRIPTION
   it will help to decode the payload

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_decode_payload(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  pb_buffer_arg *data = (pb_buffer_arg*)*arg;

  data->buf_len = stream->bytes_left;
  data->buf = stream->state;
  return pb_read(stream, (pb_byte_t*)NULL, stream->bytes_left);
}

/**
 * Decode an array of float values, such as the array within the sensor
 * sample data.
 */

 /*===========================================================================
 FUNCTION cm_decode_float_arr_cb

 DESCRIPTION
   this decodes the sensors sample rsponse

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

bool cm_decode_float_arr_cb(pb_istream_t *stream, const pb_field_t *field,void **arg)
{
  UNUSED_VAR(field);
  pb_float_arr_arg const *float_arr = (pb_float_arr_arg const*)*arg;

  if(!pb_decode_fixed32(stream, &(float_arr->arr[*float_arr->arr_index])))
      return false;

  (*float_arr->arr_index)++;

  return true;
}

/*===========================================================================
FUNCTION cm_sns_qmi_resp_msg_cb

DESCRIPTION
  this is response call back call by sensors services

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/

static void
cm_sns_qmi_resp_msg_cb(qmi_client_type user_handle, unsigned int msg_id,
    void *resp_c_struct, unsigned int resp_c_struct_len,
    void *resp_cb_data, qmi_client_error_type transp_err)
{
  UNUSED_VAR(user_handle);
  UNUSED_VAR(resp_cb_data);

  CM_MSG_HIGH_3("=cm= Received response; error %i, msg_id %i, size %i",
      transp_err, msg_id, resp_c_struct_len);

  if(QMI_NO_ERR == transp_err) 
  {
    sns_client_resp_msg_v01 *resp = (sns_client_resp_msg_v01*)resp_c_struct;

    CM_MSG_HIGH_2(" =CM= Received client ID %d,result %i",resp->client_id, resp->result);
    cm_mem_free(resp);
  }

}


/**
 * Send a QMI request message.
 *
 * @param[i] qmi_handle QMI connecton created in cm_create_client_conn
 * @param[i] req_msg Completed request message to be sent
 *
 * @return true on success, false otherwise
 */

 /*===========================================================================
 FUNCTION cm_send_qmi_req

 DESCRIPTION
   it send qmi request

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_send_qmi_req(qmi_client_type qmi_handle, sns_client_req_msg_v01 const *req_msg)
{
  bool rv = false;
  size_t resp_len = sizeof(sns_client_resp_msg_v01);
  void *resp = cm_mem_malloc(resp_len);

  CM_MSG_HIGH_1(" =CM= Sending request payload len %i", req_msg->payload_len);

  if(NULL == resp)
  {
    CM_MSG_HIGH_0("=CM= Unable to allocate sns_client_resp_msg_v01");
  }
  else
  {
    qmi_txn_handle txn_handle;

    qmi_client_error_type qmi_err = qmi_client_send_msg_async(
        qmi_handle, SNS_CLIENT_REQ_V01, (void*)req_msg, sizeof(*req_msg),
        resp, resp_len, cm_sns_qmi_resp_msg_cb, resp, &txn_handle);

    if(QMI_NO_ERR != qmi_err)
	{
      CM_MSG_HIGH_1(" =CM= qmi_client_send_msg_async error %i", qmi_err);
      cm_mem_free(resp);
    } 
	else
	{
      rv = true;
    }
  }

  return rv;
}

/**
 * Send a request to the specified service.  Do not specify any batching
 * options.
 *
 * @param[i] payload Encoded Sensor-specific request message
 * @param[i] suid Destination SUID
 * @param[i] msg_id Sensor-specific message ID
 */


 /*===========================================================================
 FUNCTION cm_send_basic_req

 DESCRIPTION
  it sends basic request to sensor

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_send_basic_req(qmi_client_type qmi_handle, pb_buffer_arg const *payload, sns_std_suid suid, uint32_t msg_id)
{
  sns_client_request_msg pb_req_msg = sns_client_request_msg_init_default;
  size_t req_len = sizeof(sns_client_req_msg_v01);
    sns_client_req_msg_v01 *req_msg = (sns_client_req_msg_v01 *)cm_mem_malloc(req_len);
  pb_ostream_t stream = pb_ostream_from_buffer(req_msg->payload, SNS_CLIENT_REQ_LEN_MAX_V01);

  pb_req_msg.suid = suid;
  pb_req_msg.msg_id = msg_id;
  pb_req_msg.susp_config.client_proc_type = SNS_STD_CLIENT_PROCESSOR_MDSP;
  pb_req_msg.susp_config.delivery_type = SNS_CLIENT_DELIVERY_WAKEUP;
  pb_req_msg.request.has_batching = false;
  pb_req_msg.request.payload.funcs.encode = &cm_encode_payload;
  pb_req_msg.request.payload.arg = (void*)payload;

  if(!pb_encode(&stream, sns_client_request_msg_fields, &pb_req_msg))
  {
    CM_MSG_HIGH_1(" =CM= Error Encoding request: %s", PB_GET_ERROR(&stream));
	cm_mem_free(req_msg);
    return false;
  }
  else
  {
    req_msg->payload_len = stream.bytes_written;
    cm_send_qmi_req(qmi_handle, req_msg);
    cm_mem_free(req_msg);
	CM_MSG_HIGH_3("cm_send_basic_req qmi_handle = 0x %x  suidlow= 0x %x  suid high= 0x %x ",qmi_handle,suid.suid_low,suid.suid_high);
#if 0
	CM_MSG_HIGH_2(" cm_send_basic_req   has_threshold_config = %d   has_resampler_config = %d  ",pb_req_msg.has_threshold_config,pb_req_msg.has_resampler_config);
#endif
    return true;
  }
}


/**
 * Create an encoded Attribute request message.
 *
 * @param[o] encoded_req Generated encoded request
 *
 * @return Length of encoded_req
 */


 /*===========================================================================
 FUNCTION cm_get_encoded_attr_req

 DESCRIPTION
   it encode the sensor attribute request

 DEPENDENCIES
 none

 RETURN VALUE
 sizet

 SIDE EFFECTS
 none

 ===========================================================================*/

static size_t cm_get_encoded_attr_req(void **encoded_req)
{
  size_t encoded_req_size = 0;
  sns_std_attr_req attr_req = sns_std_attr_req_init_default;
  attr_req.has_register_updates = FALSE;
  attr_req.register_updates = FALSE;

  *encoded_req = NULL;

  if(!pb_get_encoded_size(&encoded_req_size, sns_std_attr_req_fields, &attr_req))
  {
      CM_MSG_HIGH_0(" =CM= pb_get_encoded_size error");
  } 
  else
  {
    CM_MSG_HIGH_1(" =CM= Encode ATTR req = %d ",encoded_req_size);
    void *encoded_buffer = cm_mem_malloc(encoded_req_size);
    pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

    if(!pb_encode(&stream, sns_std_attr_req_fields, &attr_req)) {
       CM_MSG_HIGH_1(" =CM= Error Encoding attribute request: %s", PB_GET_ERROR(&stream));
       cm_mem_free(encoded_buffer);
    } else {
      *encoded_req = encoded_buffer;
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}

/**
 * Send an attribute request for the specified SUID.
 */

 /*===========================================================================
 FUNCTION cm_send_attr_req

 DESCRIPTION
   it send attribute request

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_send_attr_req(qmi_client_type qmi_handle, sns_std_suid *suid)
{
  bool rv = false;
  pb_buffer_arg payload = (pb_buffer_arg)
	    { .buf = NULL, .buf_len = 0 };
		
  rv = cm_send_basic_req(qmi_handle, &payload, *suid, SNS_STD_MSGID_SNS_STD_ATTR_REQ);
  return rv;
}

#if 0
static bool cm_send_attr_req(qmi_client_type qmi_handle, sns_std_suid *suid)
{

   bool rv = false;
   void *encoded_req = NULL;
   size_t encoded_req_len;

   encoded_req_len = cm_get_encoded_attr_req(&encoded_req);
   if (NULL != encoded_req) {
      pb_buffer_arg payload = (pb_buffer_arg)
        {.buf = encoded_req, .buf_len = encoded_req_len };
      rv = cm_send_basic_req(qmi_handle, &payload, *suid, SNS_STD_MSGID_SNS_STD_ATTR_REQ);
      cm_mem_free(encoded_req);
   }

   return rv;
}
#endif
/**
 * Create a SUID lookup request for the specified data type.
 *
 * @param[i] data_type Sensor type to search for
 * @param[o] encoded_req Generated encoded request
 *
 * @return Length of encoded_req
 */

 /*===========================================================================
 FUNCTION cm_get_encoded_suid_req

 DESCRIPTION
   it encodes the sensors id request

 DEPENDENCIES
 none

 RETURN VALUE
 size

 SIDE EFFECTS
 none

 ===========================================================================*/

static size_t cm_get_encoded_suid_req(char const *data_type, void **encoded_req)
{
  size_t encoded_req_size;
  sns_suid_req suid_req = sns_suid_req_init_default;

  *encoded_req = NULL;

  suid_req.data_type.funcs.encode = &cm_encode_payload;
  suid_req.data_type.arg = &((pb_buffer_arg)
    { .buf = data_type, .buf_len = strlen(data_type) });
  suid_req.has_register_updates = true;
  suid_req.register_updates = true;
  suid_req.has_default_only = TRUE;
  suid_req.default_only = FALSE;

  if(!pb_get_encoded_size(&encoded_req_size, sns_suid_req_fields, &suid_req)) 
  {
      CM_MSG_HIGH_0("pb_get_encoded_size error");
  } 
  else 
  {
      void *encoded_buffer = cm_mem_malloc(encoded_req_size);
      pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

      if(!pb_encode(&stream, sns_suid_req_fields, &suid_req))
      {
         CM_MSG_HIGH_1("Error Encoding attribute request: %s", PB_GET_ERROR(&stream));
         cm_mem_free(encoded_buffer);
      } 
      else 
      {
        *encoded_req = encoded_buffer;
      }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}

/**
 * Send a request to the SUID Lookup Sensor for the complete list of SUIDs
 *
 * @param[i] qmi_handle QMI connecton created in cm_create_client_conn
 * @param[i] data_type Sensor type to search for
 *
 * @return true if request was sent successfully, false otherwise
 */

 /*===========================================================================
 FUNCTION cm_send_suid_req

 DESCRIPTION
  send basic sensor request for suid 

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_send_suid_req(qmi_client_type qmi_handle, char const *data_type)
{
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len;

  encoded_req_len = cm_get_encoded_suid_req(data_type, &encoded_req);
  if(NULL != encoded_req)
  {
    pb_buffer_arg payload = (pb_buffer_arg) { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = cm_send_basic_req(qmi_handle, &payload, suid_lookup, SNS_SUID_MSGID_SNS_SUID_REQ);
    cm_mem_free(encoded_req);
  }

  return rv;
}

/**
 * Create an encoded Accel enable request message.
 *
 * @param[o] encoded_req Generated encoded request
 *
 * @return Length of encoded_req
 */

 /*===========================================================================
 FUNCTION cm_get_encoded_enable_req

 DESCRIPTION
  it enable the environmnet sesnors

 DEPENDENCIES
 none

 RETURN VALUE
 size

 SIDE EFFECTS
 none

 ===========================================================================*/

static size_t
cm_get_encoded_enable_req(void **encoded_req)
{
  size_t encoded_req_size;
  sns_std_sensor_config config_req = sns_std_sensor_config_init_default;
  config_req.sample_rate = 10;  // Stream at 10Hz
  #ifdef FEATURE_MMODE_TX_21
  cm_sns_phy_test_config_s_type *config = cm_sns_get_phy_test_config();

  if(config->sample_rate != 0 && config->cm_sns_phy_test_get_data_req == TRUE)
  {
    config_req.sample_rate = config->sample_rate;
  }
  #endif
  *encoded_req = NULL;

  if(!pb_get_encoded_size(&encoded_req_size, sns_std_sensor_config_fields, &config_req)) 
    {
      CM_MSG_HIGH_0("=CM= pb_get_encoded_size error");
    } 
   else
   {
    void *encoded_buffer = cm_mem_malloc(encoded_req_size);
    pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

    if(!pb_encode(&stream, sns_std_sensor_config_fields, &config_req)) {
        CM_MSG_HIGH_1("=CM= Error Encoding attribute request: %s", PB_GET_ERROR(&stream));
    } else {
      *encoded_req = encoded_buffer;
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}

#ifdef FEATURE_MMODE_TX_21
#ifndef FEATURE_MMODE_TX_21
/*===========================================================================
 FUNCTION cm_sns_get_encoded_fmd_req

 DESCRIPTION
  enabling sensosr request

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/


size_t
cm_sns_get_encoded_fmd_req(void **encoded_req)
{
  size_t encoded_req_size;
  void *encoded_buffer;
  sns_set_fmd_config_req fmd_config_req = sns_set_fmd_config_req_init_default;
  byte state,accel_rate,accel_sample;
  uint16 press_thres;
  
  cm_sns_get_fmd_config(&state,&press_thres,&accel_rate,&accel_sample);

  if(state == CM_SNS_FMD_STATUS_NONE)
  {
     fmd_config_req.prev_state = 1;
  }
  else
  {
     fmd_config_req.prev_state = state;
  } 
  fmd_config_req.has_threshold_pressure = true;
  fmd_config_req.threshold_pressure = press_thres;
  fmd_config_req.has_threshold_accel = true;
  fmd_config_req.threshold_accel = accel_rate;
  fmd_config_req.has_accel_buffer_count = true;
  fmd_config_req.accel_buffer_count = accel_sample;

  *encoded_req = NULL;

  if(!pb_get_encoded_size(&encoded_req_size, sns_set_fmd_config_req_fields,
      &fmd_config_req))
  {
    CM_MSG_HIGH_0("pb_get_encoded_size error");
  }
  else
  {
    encoded_buffer = cm_mem_malloc(encoded_req_size);
    if(NULL != encoded_buffer)
    {
      pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

      if(!pb_encode(&stream, sns_set_fmd_config_req_fields, &fmd_config_req))
      {
        CM_MSG_HIGH_0("Error Encoding sns_flight_mode_config_req request");
		
      }
      else
      {
        *encoded_req = encoded_buffer;
      }
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;

}
#endif

/*===========================================================================
 FUNCTION cm_send_fmd_config_request

 DESCRIPTION
  enabling sensosr request

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

bool cm_send_fmd_enable_request(qmi_client_type qmi_handle, sns_std_suid *suid)
{
  bool rv = false;

 	 pb_buffer_arg payload = (pb_buffer_arg)
		   { .buf = NULL, .buf_len = 0 };
	 rv = cm_send_basic_req(qmi_handle, &payload, *suid, SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG);

   CM_MSG_HIGH_0("The  _fmd_config_request sent");
   return rv;
}

#ifndef FEATURE_MMODE_TX_21
/*===========================================================================
 FUNCTION cm_get_encoded_tilt_tracker_config_gravity_init_req

 DESCRIPTION
  enabling sensosr request

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

size_t cm_get_encoded_tilt_tracker_config_gravity_init_req(void **encoded_req)
{
  size_t encoded_req_size;
  sns_tilt_tracker_config_gravity_init config_gravity_init = sns_tilt_tracker_config_gravity_init_init_default;

  *encoded_req = NULL;
#if 0
  config_gravity_init.gravity_init_success = true;
  config_gravity_init.gravity_init_vector_x = read from registry;
  config_gravity_init.gravity_init_vector_y = read from registry;
  config_gravity_init.gravity_init_vector_z = read from registry;
#endif
  if( cm_sns_get_tilt_gravity_vector((boolean *)&(config_gravity_init.gravity_init_success),
                                  &(config_gravity_init.gravity_init_vector_x),
                                  &(config_gravity_init.gravity_init_vector_y),
                                  &(config_gravity_init.gravity_init_vector_z)) == FALSE)
  {
      return 0;
  }
  if (!pb_get_encoded_size(&encoded_req_size, sns_tilt_tracker_config_gravity_init_fields, &config_gravity_init))
  {
      CM_MSG_HIGH_0("pb_get_encoded_size error");
  }
  else
  {
      void *encoded_buffer = cm_mem_malloc(encoded_req_size);
      pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

      if (!pb_encode(&stream, sns_tilt_tracker_config_gravity_init_fields, &config_gravity_init))
      {
         CM_MSG_HIGH_1("Error Encoding config_gravity_init request: %s", PB_GET_ERROR(&stream));
         cm_mem_free(encoded_buffer);
      }
      else
      {
        *encoded_req = encoded_buffer;
      }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}

 /*===========================================================================
  FUNCTION cm_send_tilt_tracker_config_gravity_init_req
 
  DESCRIPTION
   enabling sensosr request
 
  DEPENDENCIES
  none
 
  RETURN VALUE
  boolean
 
  SIDE EFFECTS
  none
 
  ===========================================================================*/

 bool cm_send_tilt_tracker_config_gravity_init_req(qmi_client_type qmi_handle,sns_std_suid *suid)
{
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len;

  encoded_req_len = cm_get_encoded_tilt_tracker_config_gravity_init_req( &encoded_req);
  if (NULL != encoded_req)
  {
    pb_buffer_arg payload = (pb_buffer_arg) { .buf = encoded_req, .buf_len = encoded_req_len };

    rv = cm_send_basic_req(qmi_handle, &payload, *suid, SNS_TILT_TRACKER_CONFIG_MSGID_SNS_TILT_TRACKER_CONFIG_GRAVITY_INIT);

    cm_mem_free(encoded_req);
  }

  return rv;
}

/*===========================================================================
 FUNCTION cm_get_encoded_tilt_tracker_config_req

 DESCRIPTION
  enabling sensosr request

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/



size_t cm_get_encoded_tilt_tracker_config_req( void **encoded_req)
{
  size_t encoded_req_size;
  sns_tilt_tracker_config_req tilt_tracker_config_req = sns_tilt_tracker_config_req_init_default;

  *encoded_req = NULL;
  if (cm_sns_get_tilt_config(
  
	&tilt_tracker_config_req.transportation_type ,
	(boolean *)&tilt_tracker_config_req.has_redo_gravity_init,
	(boolean *)&tilt_tracker_config_req.redo_gravity_init, 
	(boolean *)&tilt_tracker_config_req.has_iteration_delay ,
	&tilt_tracker_config_req.iteration_delay,
	(boolean *)&tilt_tracker_config_req.has_max_init_trial_num,
	(uint8 *)&tilt_tracker_config_req.max_init_trial_num,
	(boolean *)&tilt_tracker_config_req.has_max_update_trial_num,	  
	(uint8 *)&tilt_tracker_config_req.max_update_trial_num , 
	(boolean *)&tilt_tracker_config_req.has_init_gravity_threshold , 
	&tilt_tracker_config_req.init_gravity_threshold,
	(boolean *)&tilt_tracker_config_req.has_update_gravity_threshold,
	&tilt_tracker_config_req.update_gravity_threshold 
    ) == FALSE)
  	{
      return 0;
  	}

  	                      
  
#if 0
  /* this is required parameter, sns_mgr should get from customer level config */
  tilt_tracker_config_req.transportation_type = SNS_TILT_TRACKER_TRANSPORTATION_TYPE_TRAIN;
  /* this is optional parameter, sns_mgr should get from customer level config */
  tilt_tracker_config_req.has_iteration_delay = true;
  tilt_tracker_config_req.iteration_delay = 10000000; // 100 s (in unit of us)

  /* these are optional parameters, sns_mgr should get from Admin level config */
  tilt_tracker_config_req.has_max_init_trial_num = true;
  tilt_tracker_config_req.max_init_trial_num = 5;
  tilt_tracker_config_req.has_max_update_trial_num = true;
  tilt_tracker_config_req.max_update_trial_num = 5;

  tilt_tracker_config_req.has_tilt_report_threshold = true;
  tilt_tracker_config_req.tilt_report_threshold = 40.0;

  /* these are optional parameters, sns_mgr should get from Admin level config */
  /* if not these from Admin, then default values will be used, see above default Macros */
  tilt_tracker_config_req.has_init_gravity_threshold = true;
  tilt_tracker_config_req.init_gravity_threshold = xxx;
  tilt_tracker_config_req.has_update_gravity_threshold = true;
  tilt_tracker_config_req.update_gravity_threshold = yyy;

  /* this is optional parameter, sns_mgr should get from Admin level config */
  /*
   * there is scenario that client may want to do gravity init again (like package moved from one train to trunk, its position will be changed)
   * so new initialization gravity should be performed. but sensor and sns_mgr not aware of postion changed. so need client to tell sns_mgr,
   * and then sns_mgr tells tilt sensor.
  */
  tilt_tracker_config_req.has_redo_gravity_init = true;
  tilt_tracker_config_req.redo_gravity_init = true;
#endif
  if (!pb_get_encoded_size(&encoded_req_size, sns_tilt_tracker_config_req_fields, &tilt_tracker_config_req))
  {
      CM_MSG_HIGH_0("pb_get_encoded_size error");
  }
  else
  {
      void *encoded_buffer = cm_mem_malloc(encoded_req_size);
      pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

      if (!pb_encode(&stream, sns_tilt_tracker_config_req_fields, &tilt_tracker_config_req))
      {
         CM_MSG_HIGH_1("Error Encoding tilt_tracker_config_req request: %s", PB_GET_ERROR(&stream));
         cm_mem_free(encoded_buffer);
      }
      else
      {
        *encoded_req = encoded_buffer;
      }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}
#endif
 /*===========================================================================
  FUNCTION cm_send_tilt_tracker_config_req
 
  DESCRIPTION
   enabling sensosr request
 
  DEPENDENCIES
  none
 
  RETURN VALUE
  boolean
 
  SIDE EFFECTS
  none
 
  ===========================================================================*/

 bool cm_send_tilt_tracker_enable_req(qmi_client_type qmi_handle,sns_std_suid *suid )
{
  bool rv = false;

  pb_buffer_arg payload = (pb_buffer_arg) { .buf = NULL, .buf_len = 0 };
  rv = cm_send_basic_req(qmi_handle, &payload, *suid, SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG);
    
  return rv;
}



#endif


 /*===========================================================================
 FUNCTION cm_send_sns_disable_req

 DESCRIPTION
  enabling sensor request

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_send_sns_disable_req(qmi_client_type qmi_handle, sns_std_suid *suid)
{
  bool rv = false;
  
  pb_buffer_arg payload = (pb_buffer_arg)
     {.buf = NULL, .buf_len = 0};

  rv = cm_send_basic_req(qmi_handle, &payload, *suid,
       SNS_CLIENT_MSGID_SNS_CLIENT_DISABLE_REQ);	 
  return rv;
}

/*===========================================================================
 FUNCTION cm_send_sns_enable_req

 DESCRIPTION
  enabling sensosr request

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_send_sns_enable_req(qmi_client_type qmi_handle, sns_std_suid *suid)
{
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len =0;

  sns_std_sensor_msgid msg_id = cm_get_msg_id_by_sensor_type(current_sensor_type);
  CM_MSG_HIGH_3("current sensor msgid = %d suid.high = 0x %x and suid.low = 0x %x",msg_id,suid->suid_high,suid->suid_low);

  if(current_sensor_type == CM_SNS_TYPE_FMD)
  {
    #ifdef FEATURE_MMODE_TX_21
    cm_send_fmd_enable_request(qmi_handle, suid);
	#endif
    cm_send_sns_disable_req(qmi_handle, suid);
    CM_MSG_HIGH_0(" send FMD disable request ");	
  }
  else if(current_sensor_type == CM_SNS_TYPE_TILT)
  {
    #ifdef FEATURE_MMODE_TX_21
    #ifndef FEATURE_MMODE_TX_21
    cm_send_tilt_tracker_config_gravity_init_req(qmi_handle,suid);
    #endif
    cm_send_tilt_tracker_enable_req(qmi_handle,suid);   
    #endif
  }
  #ifdef FEATURE_MMODE_TX_21
   else if(current_sensor_type == CM_SNS_TYPE_MCD)
  {
      pb_buffer_arg payload = (pb_buffer_arg)
     {.buf = NULL, .buf_len = 0};
     cm_send_basic_req(qmi_handle, &payload, *suid,
       SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG);
  }
 #endif
  if(msg_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG )
  {
    encoded_req_len = cm_get_encoded_enable_req(&encoded_req);
    pb_buffer_arg payload = (pb_buffer_arg)
      { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = cm_send_basic_req(qmi_handle, &payload, *suid, msg_id);
    if(NULL != encoded_req)
    cm_mem_free(encoded_req);
  }
  CM_MSG_HIGH_1("The enable request sent sns =%d", current_sensor_type);
  return rv;
}

#ifdef FEATURE_MMODE_TX_21
boolean cm_send_sns_enable_req_wrapper()
{
  sns_std_suid uid;
  cm_sns_phy_test_config_s_type* config = cm_sns_get_phy_test_config();
  cm_sns_id_e_type sns_id = config->sensor_type; ;
  uid.suid_low =  sns_db_global[sns_id].sns_suid.suid_low;
  uid.suid_high = sns_db_global[sns_id].sns_suid.suid_high;

  return cm_send_sns_enable_req(qmi_sensors_handle, &uid);
}
#endif

/*===========================================================================
 FUNCTION cm_sns_qmi_error_cb

 DESCRIPTION
   qmi error call back

 DEPENDENCIES
 none

 RETURN VALUE
 void

 SIDE EFFECTS
 none

 ===========================================================================*/

static void cm_sns_qmi_error_cb(qmi_client_type user_handle,
    qmi_client_error_type error, void *err_cb_data)
{
  UNUSED_VAR(user_handle);
  UNUSED_VAR(err_cb_data);
  CM_MSG_HIGH_1("QMI error callback %d", error);
}


/*===========================================================================
 FUNCTION cm_decode_attr_value

 DESCRIPTION
  it decodes the sensors attribute value

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_decode_attr_value(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  //UNUSED_VAR(arg);
  int *val = (int*)*arg;
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  pb_buffer_arg str_data = (pb_buffer_arg){ .buf = NULL, .buf_len = 0 };

  value.str.funcs.decode = &cm_decode_payload;
  value.str.arg = &str_data;

  value.subtype.values.funcs.decode = &cm_decode_attr_value;

  if(!pb_decode(stream, sns_std_attr_value_data_fields, &value)) 
  {
     CM_MSG_HIGH_1(" =CM= Error decoding attribute: %s", PB_GET_ERROR(stream));
     return false;
  }

  if(value.has_flt)
    CM_MSG_HIGH_1("=CM= Attribute float: %d", value.flt);
  else if(value.has_sint)
  {
	 *val = value.sint;
	 CM_MSG_HIGH_1(" =CM= Attribute int: %d",value.sint);
  }
    
  else if(value.has_boolean)
    CM_MSG_HIGH_1(" =CM= Attribute boolean: %i", value.boolean);
  else if(NULL != str_data.buf)
  {
    CM_MSG_HIGH_1(" =CM= Attribute string: %s", (char*)str_data.buf);
	#ifdef FEATURE_MMODE_TX_21 	
	if(cm_sns_current_attr_id == SNS_STD_SENSOR_ATTRID_SERIAL_NUM)
    {
      attr_len = strlen(str_data.buf);
	  if(attr_len > CM_SNS_MAX_SERIAL_LEN)
	  {
        CM_MSG_HIGH_1(" =CM= Attr len is greater than CM_SNS_MAX_SERIAL_LEN: %d", attr_len);
        return false;
      }
	  memset(cm_sns_temperature_sn, 0, sizeof(cm_sns_temperature_sn));
	  memscpy(cm_sns_temperature_sn,CM_SNS_MAX_SERIAL_LEN,str_data.buf,attr_len);
  
      CM_MSG_HIGH_6("=CM= Sensor serial number[1-6] %c%c%c%c%c%c", cm_sns_temperature_sn[0], cm_sns_temperature_sn[1], cm_sns_temperature_sn[2],
          cm_sns_temperature_sn[3], cm_sns_temperature_sn[4], cm_sns_temperature_sn[5]);
      CM_MSG_HIGH_4("=CM= Sensor serial number[7-10] %c%c%c%c", cm_sns_temperature_sn[6], cm_sns_temperature_sn[7], cm_sns_temperature_sn[8],
          cm_sns_temperature_sn[9]);
    }
	#endif
  }
  else if(value.has_subtype)
    CM_MSG_HIGH_0(" = CM= Attribute nested");
  else
    CM_MSG_HIGH_0(" =CM= Unknown attribute type");

  return true;
}


 /*===========================================================================
 FUNCTION cm_decode_attr

 DESCRIPTION
  it decodes the sensors attribute
 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_decode_attr(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);

  sns_std_attr attribute = sns_std_attr_init_default;
  int attr_value = 0;  
  pb_istream_t stream_cpy = *stream;
  
  attribute.value.values.funcs.decode = NULL;
  attribute.value.values.arg = NULL;
 
  if(!pb_decode(stream, sns_std_attr_fields, &attribute)) 
   {
     CM_MSG_HIGH_1(" =CM= Error decoding attribute: %s", PB_GET_ERROR(stream));
     return false;
   }

  cm_sns_current_attr_id = attribute.attr_id;
  attribute.value.values.funcs.decode = &cm_decode_attr_value;
  attribute.value.values.arg = &attr_value;
  if(!pb_decode(&stream_cpy, sns_std_attr_fields, &attribute))
  {
  	CM_MSG_HIGH_1(" =CM= Error decoding attribute: %s", PB_GET_ERROR(stream));
  	return false;
  }
  CM_MSG_HIGH_3("=CM= Attribute ID %d current_sensor_type = %d attr_value = %d	", attribute.attr_id,current_sensor_type,attr_value);


  if(SNS_STD_SENSOR_ATTRID_STREAM_TYPE == attribute.attr_id)
  {
	  if (current_sensor_type == CM_SNS_TYPE_ALS && attr_value == SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE)
	  {
		cm_sns_als_stream_type =  attr_value;
		CM_MSG_HIGH_1(" Stream type for ALS sns is  %d", cm_sns_als_stream_type);
		cm_send_sns_enable_req(qmi_sensors_handle, &curr_sns_suid);
			  /***new change **/
        cm_sns_update_suid(curr_sns_suid,CM_SNS_TYPE_ALS);
       /**** change ***/
	    qmi_temp_suid[0].suid_low = 0;
		qmi_temp_suid[0].suid_high = 0;
	    qmi_temp_suid[1].suid_low = 0;
		qmi_temp_suid[1].suid_high = 0;
	  }   

  }

  return true;
}


 /*===========================================================================
 FUNCTION cm_decode_attr_event

 DESCRIPTION
   it decodes the attribute event

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_decode_attr_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  cm_sns_qmi_attribute_rsp_s_type cm_attribute_ind;
  errno_enum_type msgr_error = E_SUCCESS;;
  
  sns_std_attr_event attr_event = sns_std_attr_event_init_default;

  attr_event.attributes.funcs.decode = &cm_decode_attr;
  CM_MSG_HIGH_1("=CM= Received Attribute Event length %d", stream->bytes_left);

  if(!pb_decode(stream, sns_std_attr_event_fields, &attr_event)) 
  {
    CM_MSG_HIGH_1(" =CM= Error decoding Attr Event: %s", PB_GET_ERROR(stream));
    return false;
  }
  if(current_sensor_type == CM_SNS_TYPE_TEMPERATURE)
  {
      cm_sns_reset_suid_by_sensor_id(current_sensor_type);
      
      memset((void*)&cm_attribute_ind,0,sizeof(cm_sns_qmi_attribute_rsp_s_type)); 
      cm_attribute_ind.msg_hdr.id = MM_CM_SNS_MGR_ATTRIBUTE_IND;
      cm_attribute_ind.sn_len = attr_len;
      memscpy(&(cm_attribute_ind.cm_sns_temperature),CM_SNS_MAX_SERIAL_LEN,cm_sns_temperature_sn,attr_len);
      
      msgr_error = cm_msgr_send(MM_CM_SNS_MGR_ATTRIBUTE_IND,MSGR_MM_CM_SNS_MGR,
    	       (msgr_hdr_s*)&cm_attribute_ind,sizeof( cm_sns_qmi_attribute_rsp_s_type)); 
      
      if(msgr_error != E_SUCCESS)
      {
    		  return  FALSE;
      }
  }
  return true;
}

  bool cm_pb_decode_string_cb(pb_istream_t *stream, const pb_field_t *field,
	 void **arg)
 {
   UNUSED_VAR(field);
   pb_buffer_arg *data = (pb_buffer_arg*)*arg;
 
   data->buf_len = stream->bytes_left;
   data->buf = stream->state;
   return pb_read(stream, (pb_byte_t*)NULL, stream->bytes_left);
 }

 /*===========================================================================
  FUNCTION cm_decode_physical_sensor_test_event
 
  DESCRIPTION
	it decodes the test event
 
  DEPENDENCIES
  none
 
  RETURN VALUE
  boolean
 
  SIDE EFFECTS
  none
 
  ===========================================================================*/


 bool cm_decode_physical_sensor_test_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
 {
   UNUSED_VAR(field);
   UNUSED_VAR(arg);
 
   sns_physical_sensor_test_event test_event;
   errno_enum_type msgr_error = E_SUCCESS;
   cm_sns_qmi_sns_phy_test_rsp_s_type res;
   pb_buffer_arg payload_args = {0,0};
 
   test_event.test_data.funcs.decode = &cm_pb_decode_string_cb;
   test_event.test_data.arg = &payload_args;
   if (!pb_decode(stream, sns_physical_sensor_test_event_fields, &test_event))
   {
	 CM_MSG_HIGH_1("=CM= Error decoding SUID: %s", PB_GET_ERROR(stream));
	 return false;
   }
 
   /*
	* test type 0: SW
	* test type 1: HW
	* test type 2: factory
	* test type 3: com
	*/
   memset((void*)&res,0,sizeof(cm_sns_qmi_sns_phy_test_rsp_s_type)); 
 
   if(test_event.test_passed)
   {
	 CM_MSG_HIGH_1("Test type: %d: PASS", test_event.test_type);
	 res.test_passed = TRUE;
   }
   else
   {
	 CM_MSG_HIGH_1("Test type: %d: FAIL", test_event.test_type);
	 res.test_passed = FALSE; 
   }
   res.msg_hdr.id = MM_CM_SNS_MGR_SNS_TEST_PHY_IND;
   res.physical_sns_test = (cm_sns_phy_test_e_type) test_event.test_type;
   
   msgr_error = cm_msgr_send(MM_CM_SNS_MGR_SNS_TEST_PHY_IND,MSGR_MM_CM_SNS_MGR,
					 (msgr_hdr_s*)&res,sizeof(cm_sns_qmi_sns_phy_test_rsp_s_type));
	 
   if(msgr_error != E_SUCCESS)
   {
		 return  FALSE;
   }
 
   return TRUE;
 }


 /*===========================================================================
 FUNCTION cm_decode_physical_config_event

 DESCRIPTION
   it decode the physical sensor event

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_decode_physical_config_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_sensor_physical_config_event event = sns_std_sensor_physical_config_event_init_default;

  if(!pb_decode(stream, sns_std_sensor_physical_config_event_fields, &event)) {
    CM_MSG_HIGH_1(" =CM= cm_decode_physical_config_event: Error decoding event: %s", PB_GET_ERROR(stream));
    return false;
  }

  CM_MSG_HIGH_1(" =CM= Received config event.  Sample rate: %d", event.sample_rate);

  return true;
}

/**
 * Decode the sensor event (aka a sensor sample).
 */

 /*===========================================================================
 FUNCTION cm_decode_sensor_event

 DESCRIPTION
  it decode the sensor event
 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_decode_sensor_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_sensor_event event = sns_std_sensor_event_init_default;
  float sensor_data[MAX_SENSOR_DATA]= {0.0};
  uint8_t arr_index = 0;
  pb_float_arr_arg float_arg = (pb_float_arr_arg)
    { .arr = sensor_data, .arr_len = MAX_SENSOR_DATA, .arr_index = &arr_index };

  event.data.funcs.decode = &cm_decode_float_arr_cb;
  event.data.arg = &float_arg;

  if(!pb_decode(stream, sns_std_sensor_event_fields, &event))
  {
     CM_MSG_HIGH_1("=CM= cm_decode_sensor_event: Error decoding Attr Event: %s", PB_GET_ERROR(stream));
     return false;
  }
#ifdef FEATURE_MMODE_TX_21
 if(event.status != SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE)
#endif 
{
  /* Following can be replaces with actual functional where we can update the sensor buffer   */
  cm_sns_update_sample_buffer_per_sid(current_sensor_type,sensor_data,arr_index,(uint64) time_get_secs());
} 
  CM_MSG_HIGH_5(" [debug]Received sensor sample %d %d %d for sensor type = %d  time = %d ",
        (int)sensor_data[0], (int)sensor_data[1], (int)sensor_data[2],current_sensor_type,time_get_secs());

  return true;
}

#ifdef FEATURE_MMODE_TX_21
/*===========================================================================
FUNCTION cm_sns_decode_fmd_event

DESCRIPTION
 it decode the sensor event
DEPENDENCIES
none

RETURN VALUE
boolean

SIDE EFFECTS
none

===========================================================================*/


bool
cm_sns_decode_fmd_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  cm_sns_motion_event_e_type fmd_evt = CM_SNS_MOTION_EVENT_TYPE_UNKNOWN;
  sns_fmd_event fmd_event;


  if (!pb_decode(stream, sns_fmd_event_fields, &fmd_event))
  {
     CM_MSG_HIGH_0("error decoding Flight Mode Detection Event");
    return false;
  }

  CM_MSG_HIGH_1("Flight mode state: %d ",fmd_event.state);
  if(fmd_event.state == SNS_FMD_NOT_IN_FLIGHT )
  {
    fmd_evt = CM_SNS_MOTION_EVENT_TYPE_DEVICE_NOT_IN_FLIGHT;
  }
  else if(fmd_event.state == SNS_FMD_IN_FLIGHT )
  {
     fmd_evt = CM_SNS_MOTION_EVENT_TYPE_DEVICE_IN_FLIGHT;
  }

   cm_sns_update_motion_rsp(fmd_evt);

  return true;
}

/*===========================================================================
FUNCTION cm_sns_decode_free_fall_event

DESCRIPTION
 it decode the sensor event
DEPENDENCIES
none

RETURN VALUE
boolean

SIDE EFFECTS
none

===========================================================================*/



bool
cm_sns_decode_free_fall_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  cm_sns_motion_event_e_type fall_evt = CM_SNS_MOTION_EVENT_TYPE_UNKNOWN;
  sns_freefall_detect_event fall_event;
 

  if (!pb_decode(stream, sns_freefall_detect_event_fields, &fall_event))
  {
    CM_MSG_HIGH_0("error decoding free fall  Detection Event");
    return false;
  }

  CM_MSG_HIGH_1("Free fall detect state: %d ",fall_event.state);
  if(fall_event.state == SNS_FREEFALL_DETECT_EVENT_TYPE_NO_FALL )
  {
    fall_evt = CM_SNS_MOTION_EVENT_TYPR_DEVICE_FALL_NOT_DETECT;
  }
  else if(fall_event.state == SNS_FREEFALL_DETECT_EVENT_TYPE_FALL )
  {
     fall_evt = CM_SNS_MOTION_EVENT_TYPE_DEVICE_FALL_DETECT;
  }

   cm_sns_update_motion_rsp(fall_evt);

  return true;
}
#ifndef FEATURE_MMODE_TX_21
/*===========================================================================
FUNCTION cm_sns_decode_tilt_gravity_event

DESCRIPTION
 it decode the tilt sensor gravity event
DEPENDENCIES
none

RETURN VALUE
boolean

SIDE EFFECTS
none

===========================================================================*/


bool
cm_sns_decode_tilt_gravity_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_tilt_tracker_gravity_init_event gravity_event = sns_tilt_tracker_gravity_init_event_init_default;
 

  if (!pb_decode(stream, sns_tilt_tracker_gravity_init_event_fields, &gravity_event))
  {
    CM_MSG_HIGH_0("error decoding free fall  Detection Event");
    return false;
  }

   CM_MSG_HIGH_4("cm_sns tilt_gravity_vector sucs = %d gx = %d gy = %d gz = %d ",gravity_event.gravity_init_success,
   	gravity_event.gravity_init_vector_x,gravity_event.gravity_init_vector_y,gravity_event.gravity_init_vector_z);
   cm_sns_set_tilt_gravity_vector(gravity_event.gravity_init_success,
   	gravity_event.gravity_init_vector_x,gravity_event.gravity_init_vector_y,gravity_event.gravity_init_vector_z);

  return true;
}
#endif
/*===========================================================================
FUNCTION cm_sns_decode_tilt_angle_event

DESCRIPTION
 it decode the tilt sensor angle event
DEPENDENCIES
none

RETURN VALUE
boolean

SIDE EFFECTS
none

===========================================================================*/


bool
cm_sns_decode_tilt_angle_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  float sample_arr[1];
  sns_tilt_tracker_event tilt_anlge_event = sns_tilt_tracker_event_init_default;
 

  if (!pb_decode(stream, sns_tilt_tracker_event_fields, &tilt_anlge_event))
  {
    CM_MSG_HIGH_0("error decoding free fall  Detection Event");
    return false;
  }

  CM_MSG_HIGH_1("Tilt Angle received %d ",tilt_anlge_event.tilt_angle);
  sample_arr[0] = tilt_anlge_event.tilt_angle;
  cm_sns_update_sample_buffer_per_sid(current_sensor_type,sample_arr,1,(uint64) time_get_secs());

  return true;
}

/*===========================================================================
FUNCTION cm_sns_decode_motion_change_detector_event

DESCRIPTION
 it decode the sensor event
DEPENDENCIES
none

RETURN VALUE
boolean

SIDE EFFECTS
none

===========================================================================*/



bool
cm_sns_decode_motion_change_detector_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  cm_sns_mcd_event_state_s_type mcd_evt ;
  sns_mcd_tracker_event mcd_event =sns_mcd_tracker_event_init_default ;

  if (!pb_decode(stream, sns_mcd_tracker_event_fields, &mcd_event))
  {
    CM_MSG_HIGH_0("error decoding MCD Event");
    return false;
  }

  CM_MSG_HIGH_2("SEE MCD prev state: %d and current state : %d",mcd_event.prev_state,mcd_event.curr_state);
  CM_MSG_HIGH_1("SEE MCD next run =  %d ",mcd_event.next_run_duration);
  mcd_evt.psm_time = mcd_event.next_run_duration;
  if(mcd_event.next_run_duration== 0)
  {
  if(mcd_event.prev_state != mcd_event.curr_state)
  {
     mcd_evt.event= (cm_sns_motion_event_e_type)mcd_event.curr_state;
  }
  else 
  {
     mcd_evt.event=CM_SNS_MOTION_EVENT_TYPE_UNKNOWN;
  }
  }
  else 
  {
    mcd_evt.event=CM_SNS_MOTION_EVENT_TYPE_UNKNOWN;
  }
  CM_MSG_HIGH_1("MCD event =  %d ",mcd_evt.event);
  cm_sns_update_mcd_rsp(mcd_evt.event,mcd_evt.psm_time);
  return true;
}


#endif
/*===========================================================================
FUNCTION cm_process_md_event

DESCRIPTION
it process the amd sensor event
DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/

void cm_process_md_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{

  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  
   sns_motion_detect_event md_event = sns_motion_detect_event_init_default;
  
  if (!pb_decode(stream, sns_motion_detect_event_fields, &md_event))
  {
    CM_MSG_HIGH_0("=CM= Error in decoding MD data event");
  }
  else
  {      
     CM_MSG_HIGH_1("  Received MD  event = %d, ignore here",md_event.motion_detect_event_type); 
	 if(md_event.motion_detect_event_type == SNS_MOTION_DETECT_EVENT_TYPE_ENABLED)
	 {
       cm_sns_update_motion_rsp(CM_SNS_MOTION_EVENT_TYPE_MD_INTR_ENABLE);
	 }
  }

}


/*===========================================================================
FUNCTION cm_process_motion_sns_event

DESCRIPTION
  it send the motion sensor event to CM task
DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/

void cm_process_motion_sns_event(void)
{
  cm_sns_update_motion_rsp(CM_SNS_MOTION_EVENT_TYPE_MOTION);
}

/**
 * Decode an event message received from the accel Sensor.
 */

#ifdef FEATURE_MMODE_TX_21
 /*===========================================================================
FUNCTION cm_sns_decode_registry_event

DESCRIPTION
 it decode the registry write response
DEPENDENCIES
none

RETURN VALUE
boolean

SIDE EFFECTS
none

===========================================================================*/


bool
cm_sns_decode_registry_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  cm_sns_registry_write_rsp_s_type registry;
  errno_enum_type msgr_error = E_SUCCESS;

  msgr_error = cm_msgr_send(MM_CM_SNS_MGR_REGISTRY_IND,MSGR_MM_CM_SNS_MGR,
                      (msgr_hdr_s*)&registry,sizeof(cm_sns_registry_write_rsp_s_type) );

  return true;
}
bool sns_smd_tracker_decode_float_arr(
   pb_istream_t stream, pb_buffer_arg group_name,
   uint8_t arr_sz, float* data)
{
  bool rv;
  uint8_t arr_index = 0;
  sns_registry_read_event read_event = sns_registry_read_event_init_default;
  pb_float_arr_arg arr_arg = {
 .arr = data,
 .arr_index = &arr_index,
 .arr_len = arr_sz
};

  sns_registry_decode_arg arg = {
 .item_group_name = &group_name,
 .parse_info_len = 1,
 .parse_info[0] = {
        .parse_func = sns_registry_parse_float_arr,
        .parsed_buffer = &arr_arg
  }
};

  read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
  read_event.data.items.arg = &arg;

  rv =pb_decode(&stream, sns_registry_read_event_fields, &read_event);
  return rv;
}

static bool fmd_registry_read_cb(
pb_istream_t *stream,
const pb_field_t *field,
void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_registry_data_item reg_item;
  pb_buffer_arg item_name = {0,0};
  reg_item.name.arg = &item_name;
  reg_item.name.funcs.decode = pb_decode_string_cb;
  bool rv = pb_decode(stream, sns_registry_data_item_fields, &reg_item);
  if(reg_item.has_sint)
  {
  CM_MSG_HIGH_2("name %s sint %lld", (char*)item_name.buf, reg_item.sint);
  }
  else if(reg_item.has_flt)
  {
  CM_MSG_HIGH_2("name %s flt %f", (char*)item_name.buf, reg_item.flt);
  }

  reg_read->sensor_id = CM_SNS_TYPE_FMD;
  if(0 == strcmp(item_name.buf, "threshold_accel") && reg_item.has_flt)
  {
   reg_read->registry_fmd_config_data.accel_threshold = reg_item.flt;
  }
  else if(0 == strcmp(item_name.buf, "accel_buffer_count") && reg_item.has_sint)
  {
    reg_read->registry_fmd_config_data.accel_buffer_count= reg_item.sint;
  }
  else if(0 == strcmp(item_name.buf, "run_period") && reg_item.has_sint)
  {
     reg_read->registry_fmd_config_data.run_duration= reg_item.has_sint;
  }
  return rv;
}

static bool mcd_registry_read_cb(
pb_istream_t *stream,
const pb_field_t *field,
void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_registry_data_item reg_item;
  pb_buffer_arg item_name = {0,0};
  reg_item.name.arg = &item_name;
  reg_item.name.funcs.decode = pb_decode_string_cb;
  bool rv = pb_decode(stream, sns_registry_data_item_fields, &reg_item);
  if(reg_item.has_sint)
  {
    CM_MSG_HIGH_2("name %s sint %lld", (char*)item_name.buf, reg_item.sint);
  }
  else if(reg_item.has_flt)
  {
    CM_MSG_HIGH_2("name %s flt %f", (char*)item_name.buf, reg_item.flt);
  }

  reg_read->sensor_id = CM_SNS_TYPE_MCD;
  if(0 == strcmp(item_name.buf, "tgl_dly_thld") && reg_item.has_sint)
  {
    CM_MSG_HIGH_1("tgl_dly_thld = %d",reg_item.has_sint);
    reg_read->registry_mcd_config_data.mcd_motion_toggle_delay_threshold = reg_item.sint;
  }
  else if(0 == strcmp(item_name.buf, "tgl_cnt_thld") && reg_item.has_sint)
  {
    CM_MSG_HIGH_1("tgl_cnt_thld = %d",reg_item.has_sint);
    reg_read->registry_mcd_config_data.mcd_motion_toggle_counter_threshold= reg_item.sint;
  }
  else if(0 == strcmp(item_name.buf, "psm_timer_cfg") && reg_item.has_sint)
  {
    CM_MSG_HIGH_1("psm_timer_cfg = %d",reg_item.has_sint);
    reg_read->registry_mcd_config_data.mcd_psm_timer_config= 0;
  }
  return rv;
}

static bool tilt_registry_read_cb(
pb_istream_t *stream,
const pb_field_t *field,
void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_registry_data_item reg_item;
  pb_buffer_arg item_name = {0,0};
  reg_item.name.arg = &item_name;
  reg_item.name.funcs.decode = pb_decode_string_cb;
  bool rv = pb_decode(stream, sns_registry_data_item_fields, &reg_item);
  if(reg_item.has_sint)
  {
    CM_MSG_HIGH_2("name %s sint %lld", (char*)item_name.buf, reg_item.sint);
  }
  else if(reg_item.has_flt)
  {
    CM_MSG_HIGH_2("name %s flt %f", (char*)item_name.buf, reg_item.flt);
  }

   reg_read->sensor_id = CM_SNS_TYPE_TILT;

   if(0 == strcmp(item_name.buf, "gravity_init_success") && reg_item.has_sint)
  {
    reg_read->registry_tilt_config_data.redo_gravity_init = reg_item.sint;
  }
  else if(0 == strcmp(item_name.buf, "transportation_type") && reg_item.has_sint)
  {
    reg_read->registry_tilt_config_data.transportation_type= reg_item.sint;
  }
  else if(0 == strcmp(item_name.buf, "max_init_trial_num") && reg_item.has_sint)
  {
    reg_read->registry_tilt_config_data.max_init_trial_num = reg_item.sint;
  }
  else if(0 == strcmp(item_name.buf, "max_update_trial_num") && reg_item.has_sint)
  {
    reg_read->registry_tilt_config_data.max_update_trial_num= reg_item.sint;
  }
  else if(0 == strcmp(item_name.buf, "init_gravity_threshold") && reg_item.has_flt)
  {
    reg_read->registry_tilt_config_data.init_gravity_threshold = reg_item.flt;
  }
  else if(0 == strcmp(item_name.buf, "update_gravity_threshold") && reg_item.has_flt)
  {
    reg_read->registry_tilt_config_data.update_gravity_threshold = reg_item.flt;
  }

 else if(0 == strcmp(item_name.buf, "iteration_delay") && reg_item.has_sint)
  {
    reg_read->registry_tilt_config_data.iteration_delay = reg_item.sint;
  } 
return rv;
}

static bool md_registry_read_cb(
pb_istream_t *stream,
const pb_field_t *field,
void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_registry_data_item reg_item;
  pb_buffer_arg item_name = {0,0};
  reg_item.name.arg = &item_name;
  reg_item.name.funcs.decode = pb_decode_string_cb;
  bool rv = pb_decode(stream, sns_registry_data_item_fields, &reg_item);
  if(reg_item.has_sint)
  {
    CM_MSG_HIGH_2("name %s sint %lld", (char*)item_name.buf, reg_item.sint);
  }
  else if(reg_item.has_flt)
  {
   CM_MSG_HIGH_2("name %s flt %f", (char*)item_name.buf, reg_item.flt);
  }

  reg_read->sensor_id = CM_SNS_TYPE_MOTION_DETECT;
  if(0 == strcmp(item_name.buf, "thresh") && reg_item.has_flt)
  {
    reg_read->registry_md_config_data.md_tuning_threshold = reg_item.flt;
  }

  return rv;
}

static bool smd_threshold_registry_read_cb(
pb_istream_t *stream,
const pb_field_t *field,
void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_registry_data_item reg_item;
  pb_buffer_arg item_name = {0,0};
  reg_item.name.arg = &item_name;
  reg_item.name.funcs.decode = pb_decode_string_cb;
  bool rv = pb_decode(stream, sns_registry_data_item_fields, &reg_item);
  if(reg_item.has_sint)
  {
    CM_MSG_HIGH_2("name %s sint %lld", (char*)item_name.buf, reg_item.sint);
  }
  else if(reg_item.has_flt)
  {
   CM_MSG_HIGH_2("name %s flt %f", (char*)item_name.buf, reg_item.flt);
  }
  reg_read->sensor_id = CM_SNS_TYPE_SMD;
  reg_read->registry_smd_config_data.smd_sum_score_threshold = reg_item.sint;

  return rv;
}

bool cm_decode_registry_read_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);

  pb_istream_t stream_cpy = *stream;
  sns_rc rv = SNS_RC_NOT_AVAILABLE;
  sns_registry_read_event read_event = sns_registry_read_event_init_default;
  pb_buffer_arg group_name = {0,0};

  read_event.name.arg = &group_name;
  read_event.name.funcs.decode = cm_pb_decode_string_cb;
  errno_enum_type msgr_error = E_SUCCESS;

  if(reg_read == NULL)
  {
    reg_read = (cm_sns_registry_rsp_s_type*)cm_mem_malloc(sizeof(cm_sns_registry_rsp_s_type)) ;
  }

  if(!pb_decode(stream, sns_registry_read_event_fields, &read_event))
  {
   CM_MSG_HIGH_0("Error decoding registry event");
  }
  else
  {

  /* Compare group name firstly, then call different read_cb to store data to each structure */
  if(0 == strcmp("sns_smd_tracker.threshold", group_name.buf))
  {
   read_event.data.items.arg = NULL;
   read_event.data.items.funcs.decode = smd_threshold_registry_read_cb;
    if(!pb_decode(&stream_cpy, sns_registry_read_event_fields, &read_event))
   {
      CM_MSG_HIGH_0("Error decoding registry event");
   }
  }
   else if(0 == strcmp("sns_smd_tracker.percep_weights", group_name.buf))
  {
    reg_read->sensor_id = CM_SNS_TYPE_SMD;
    sns_smd_tracker_decode_float_arr(stream_cpy,group_name,20,reg_read->registry_smd_config_data.smd_weight );
  }
  else if(0 == strcmp("sns_smd_tracker.percep_min_features", group_name.buf))
  {
    reg_read->sensor_id = CM_SNS_TYPE_SMD;
    sns_smd_tracker_decode_float_arr(stream_cpy,group_name,4,
    reg_read->registry_smd_config_data.smd_perceptron_min_weight );
  }
  else if(0 == strcmp("sns_smd_tracker.percep_max_features", group_name.buf))
  {
   reg_read->sensor_id = CM_SNS_TYPE_SMD;
   sns_smd_tracker_decode_float_arr(stream_cpy,group_name,4,
   reg_read->registry_smd_config_data.smd_perceptron_max_weight );
  }
  else if(0 == strcmp("sns_flight_mode.threshold", group_name.buf))
  {
    read_event.data.items.arg = NULL;
    read_event.data.items.funcs.decode = fmd_registry_read_cb;
    if(!pb_decode(&stream_cpy, sns_registry_read_event_fields, &read_event))
    {
      CM_MSG_HIGH_0("Error decoding registry event");
    }
  }
  else if(0 == strcmp("sns_tilt_tracker", group_name.buf))
  {
    read_event.data.items.arg = NULL;
    read_event.data.items.funcs.decode = tilt_registry_read_cb;
    if(!pb_decode(&stream_cpy, sns_registry_read_event_fields, &read_event))
    {
      CM_MSG_HIGH_0("Error decoding registry event");
    }
  }

  else if(0 == strcmp("lsm6dso_0_platform.md.config", group_name.buf))
  {
    read_event.data.items.arg = NULL;
    read_event.data.items.funcs.decode = md_registry_read_cb;
    if(!pb_decode(&stream_cpy, sns_registry_read_event_fields, &read_event))
    {
      CM_MSG_HIGH_0("Error decoding registry event");
    }
  }

  else if(0 == strcmp("sns_mcd_tracker.threshold", group_name.buf))
  {
    read_event.data.items.arg = NULL;
    read_event.data.items.funcs.decode = mcd_registry_read_cb;
    if(!pb_decode(&stream_cpy, sns_registry_read_event_fields, &read_event))
    {
      CM_MSG_HIGH_0("Error decoding registry event");
    }
  }

}

  if(config_read_counter == 0)
 {
   msgr_error = cm_msgr_send(MM_CM_SNS_MGR_READ_CONFIG_RSP,MSGR_MM_CM_SNS_MGR,
     (msgr_hdr_s*)reg_read,sizeof(cm_sns_registry_rsp_s_type));
  if(reg_read != NULL)
 {
  cm_mem_free(reg_read);
  reg_read = NULL;
 }
 } 

 return TRUE;
}
#endif
 /*===========================================================================
 FUNCTION cm_handle_sensor_event

 DESCRIPTION
  it handle sensors related event

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_handle_sensor_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  uint32_t *msg_id = (uint32_t*)*arg;
  pb_byte_t *temp = NULL;
  CM_MSG_HIGH_1("=CM= Begin decoding Sensor event msg_id = %d ",*msg_id);
  
  if (SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG == *msg_id) {
    temp = cm_mem_malloc(stream->bytes_left);
    pb_read(stream, temp, stream->bytes_left);
    cm_mem_free(temp);
  }
  else if (SNS_SIG_MOTION_MSGID_SNS_SIG_MOTION_EVENT == *msg_id) {
    cm_process_motion_sns_event();
  }
#ifdef FEATURE_MMODE_TX_21
  else if(*msg_id == (sns_flight_mode_msgid)SNS_FLIGHT_MODE_MSGID_SNS_FMD_EVENT && (current_sensor_type == CM_SNS_TYPE_FMD) )
  {
     cm_sns_decode_fmd_event(stream, field, arg);
  } 
  else if(*msg_id == (sns_freefall_detect_msgid)SNS_FREEFALL_DETECT_MSGID_SNS_FREEFALL_DETECT_EVENT && (current_sensor_type == CM_SNS_TYPE_FALL_DETECT ))
  {
     cm_sns_decode_free_fall_event(stream, field, arg);
  } 
 #ifndef FEATURE_MMODE_TX_21
  else if(*msg_id == SNS_TILT_TRACKER_MSGID_SNS_TILT_TRACKER_GRAVITY_INIT_EVENT && (current_sensor_type == CM_SNS_TYPE_TILT ))
  {

     cm_sns_decode_tilt_gravity_event(stream,field, arg);
  }
 #endif
  else if(*msg_id == SNS_TILT_TRACKER_MSGID_SNS_TILT_TRACKER_EVENT && (current_sensor_type ==  CM_SNS_TYPE_TILT ))
  {
     cm_sns_decode_tilt_angle_event(stream,field,arg);
  }
  else if(*msg_id == SNS_MCD_TRACKER_MSGID_SNS_MCD_TRACKER_EVENT && (current_sensor_type ==  CM_SNS_TYPE_MCD ))
  {
     cm_sns_decode_motion_change_detector_event(stream,field,arg);
  }
  else if(*msg_id == SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_EVENT  && (current_sensor_type ==  CM_SNS_TYPE_REGISTRY ))
  {
     CM_MSG_HIGH_0("Registry write event");
     config_write_counter--;
    if(config_write_counter ==0)
   {
    cm_sns_decode_registry_event(stream,field,arg);
    cm_sns_reset_suid_by_sensor_id(CM_SNS_TYPE_REGISTRY);
    current_sensor_type = CM_SNS_TYPE_NONE;
   }
  }
  else if(*msg_id == SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT  && (current_sensor_type ==  CM_SNS_TYPE_REGISTRY ))
  {
    CM_MSG_HIGH_0("Registry read event");
    config_read_counter--;
    cm_decode_registry_read_event(stream,field,arg);
    if(config_read_counter ==0)
    {
      cm_sns_reset_suid_by_sensor_id(CM_SNS_TYPE_REGISTRY);
      current_sensor_type = CM_SNS_TYPE_NONE;
    }
  }
#endif
 else if( SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT == *msg_id){
    cm_process_md_event(stream, field, arg);
  }
  else if (SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT == *msg_id) {
  return cm_decode_physical_config_event(stream, field, arg);
  } else if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT == *msg_id) {
  return cm_decode_sensor_event(stream, field, arg);
  } else if(SNS_STD_MSGID_SNS_STD_ATTR_EVENT == *msg_id) {
  return cm_decode_attr_event(stream, field, arg);
  } else if(SNS_STD_MSGID_SNS_STD_ERROR_EVENT == *msg_id) {
  CM_MSG_HIGH_0(" =CM= Received Error event");
  return false;
  } 
  else if(SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT == *msg_id){
  return cm_decode_physical_sensor_test_event(stream, field, arg);
  }
  else {
  CM_MSG_HIGH_1("=CM= Received unknown event %i", *msg_id);
  return false;
  }
  CM_MSG_HIGH_0("=CM= Done decoding Sensors event");
  
  return true;
}

#ifdef FEATURE_MMODE_TX_21
    size_t cm_get_encoded_registry_write_req(void **encoded_req,
     char* group_name, bool (*encode_cb)(struct pb_ostream_s *, struct pb_field_s const *, void *const *arg))
    {
    size_t encoded_req_size;
    sns_registry_write_req write_req = sns_registry_write_req_init_default;
    pb_buffer_arg name_data = (pb_buffer_arg) {
     .buf = group_name, .buf_len = (strlen(group_name) + 1)
    };
    write_req.name.arg = &name_data;
    write_req.name.funcs.encode = &cm_encode_payload;
    write_req.data.items.arg = NULL;
    write_req.data.items.funcs.encode = encode_cb;

    *encoded_req = NULL;

    if(!pb_get_encoded_size(&encoded_req_size, sns_registry_write_req_fields,
       &write_req))
    {
     CM_MSG_HIGH_0("pb_get_encoded_size error");
    }
    else
    {
     void *encoded_buffer = cm_mem_malloc(encoded_req_size);
     if(NULL != encoded_buffer)
     {
       pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

       if(!pb_encode(&stream, sns_registry_write_req_fields, &write_req))
       {
         CM_MSG_HIGH_0("Error Encoding sns_registry_write_req request");
       }
       else
       {
         *encoded_req = encoded_buffer;
       }
     }
    }

    return NULL == *encoded_req ? 0 : encoded_req_size;
    }

  bool cm_smd_tracker_sum_score_thresh_registry_write_cb(
    struct pb_ostream_s *stream,
    struct pb_field_s const *field,
    void *const *arg)
    {
      UNUSED_VAR(arg);
      cm_sns_reg_config_s_type *config_ptr = get_reg_config();

      pb_buffer_arg name_data = (pb_buffer_arg)
      { .buf = "sum_score_thresh", .buf_len = strlen("sum_score_thresh") + 1 };

      sns_registry_data_item pb_item = sns_registry_data_item_init_default;
      pb_item.name.funcs.encode = &pb_encode_string_cb;
      pb_item.name.arg = &name_data;

      pb_item.has_sint = true;
      pb_item.sint = config_ptr->smd_config_reg.smd_sum_score_threshold;

    if (!pb_encode_tag_for_field(stream, field))
    {
       CM_MSG_HIGH_0("smd_tracker: registry write Error tag item sum_score_thresh");
       return false;
    }

    if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
    {
       CM_MSG_HIGH_0("smd_tracker: registry write Error item sum_score_thresh");
       return false;
    }
      return true;
    }

  bool cm_smd_tracker_percep_weights_registry_write_cb(
    struct pb_ostream_s *stream,
    struct pb_field_s const *field,
    void *const *arg)
      {
        UNUSED_VAR(arg);
        char name[3];
        cm_sns_reg_config_s_type *config_ptr = get_reg_config();

        for(int i = 0; i < ARR_SIZE(config_ptr->smd_config_reg.smd_perceptron_weight); i++)
        {
         memset(name, 0, sizeof(name));
         snprintf(name, sizeof(name), "%d", i);
         pb_buffer_arg name_data = (pb_buffer_arg)
           { .buf = name, .buf_len = strlen(name) + 1 };

         sns_registry_data_item pb_item = sns_registry_data_item_init_default;
         pb_item.name.funcs.encode = &pb_encode_string_cb;
         pb_item.name.arg = &name_data;
         
         pb_item.has_flt = true;
         pb_item.flt = config_ptr->smd_config_reg.smd_perceptron_weight[i];

         if (!pb_encode_tag_for_field(stream, field))
         {
           CM_MSG_HIGH_0("smd_tracker: registry write Error tag item percep_weights");
           return false;
         }

         if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
         {
           CM_MSG_HIGH_0("smd_tracker: registry write Error item percep_weights");
           return false;
         }
        }
        return true;
      }

  bool cm_smd_tracker_percep_min_features_registry_write_cb(
    struct pb_ostream_s *stream,
    struct pb_field_s const *field,
    void *const *arg)
	    {
        UNUSED_VAR(arg);
        char name[3];
        cm_sns_reg_config_s_type *config_ptr = get_reg_config();

        for(int i = 0; i < ARR_SIZE(config_ptr->smd_config_reg.smd_perceptron_min_weight); i++)
        {
        memset(name, 0, sizeof(name));
        snprintf(name, sizeof(name), "%d", i);
        pb_buffer_arg name_data = (pb_buffer_arg)
          { .buf = name, .buf_len = strlen(name) + 1 };

        sns_registry_data_item pb_item = sns_registry_data_item_init_default;
        pb_item.name.funcs.encode = &pb_encode_string_cb;
        pb_item.name.arg = &name_data;

        pb_item.has_flt = true;
        pb_item.flt = config_ptr->smd_config_reg.smd_perceptron_min_weight[i];

        if (!pb_encode_tag_for_field(stream, field))
        {
          CM_MSG_HIGH_0("smd_tracker: registry write Error tag item percep_min_features");
          return false;
        }

        if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
        {
          CM_MSG_HIGH_0("smd_tracker: registry write Error item percep_min_features");
          return false;
        }
        }
        return true;
        }

    bool cm_smd_tracker_percep_max_features_registry_write_cb(
    struct pb_ostream_s *stream,
    struct pb_field_s const *field,
    void *const *arg)
    {
    UNUSED_VAR(arg);
    char name[3];
    cm_sns_reg_config_s_type *config_ptr = get_reg_config();

    for(int i = 0; i < ARR_SIZE(config_ptr->smd_config_reg.smd_perceptron_max_weight); i++)
    {
    memset(name, 0, sizeof(name));
    snprintf(name, sizeof(name), "%d", i);
    pb_buffer_arg name_data = (pb_buffer_arg)
      { .buf = name, .buf_len = strlen(name) + 1 };

    sns_registry_data_item pb_item = sns_registry_data_item_init_default;
    pb_item.name.funcs.encode = &pb_encode_string_cb;
    pb_item.name.arg = &name_data;

    pb_item.has_flt = true;
    pb_item.flt = config_ptr->smd_config_reg.smd_perceptron_max_weight[i];

    if (!pb_encode_tag_for_field(stream, field))
    {
      CM_MSG_HIGH_0("smd_tracker: registry write Error tag item percep_max_features");
      return false;
    }

    if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
    {
      CM_MSG_HIGH_0("smd_tracker: registry write Error item percep_max_features");
      return false;
    }
    }
    return true;
    }

    bool cm_tilt_tracker_registry_write_cb(
    struct pb_ostream_s *stream,
    struct pb_field_s const *field,
    void *const *arg)
    {
    UNUSED_VAR(arg);


    char const *names[] = {    "gravity_init_success",
                   "transportation_type",
                           "max_init_trial_num",
                   "max_update_trial_num",
                   "init_gravity_threshold",
                   "update_gravity_threshold",
                    "iteration_delay"};
    cm_sns_reg_config_s_type *config_ptr = get_reg_config();

    for(int i = 0; i < ARR_SIZE(names); i++)
    {


     if (0 == i && config_ptr->tilt_tracker_config_reg.gravity_init_success ==0 )
     {
       continue;
     }
      if(1 == i && config_ptr->tilt_tracker_config_reg.transportation_type ==0)
     {
       continue;
     }

     if(2 == i && config_ptr->tilt_tracker_config_reg.max_init_trial_num ==0xFF)
     {
       continue;
     }
      if(3 == i  && config_ptr->tilt_tracker_config_reg.max_update_trial_num ==0xFF)
     {
      continue;
     }
      if(4 == i && config_ptr->tilt_tracker_config_reg.init_gravity_threshold ==0xFFFFFFFF)
     {
       continue;
     }
      if(5 == i && config_ptr->tilt_tracker_config_reg.update_gravity_threshold ==0xFF)
     {
       continue;
     }
      if(6 == i&& config_ptr->tilt_tracker_config_reg.iteration_delay ==0xFFFFFFFF)
     {
       continue;
     }


     pb_buffer_arg name_data = (pb_buffer_arg)
       { .buf = names[i], .buf_len = strlen(names[i]) + 1 };

     sns_registry_data_item pb_item = sns_registry_data_item_init_default;
     pb_item.name.funcs.encode = &pb_encode_string_cb;
     pb_item.name.arg = &name_data;



    if (0 == i)
     {
       pb_item.has_sint = true;
       pb_item.sint = 0;
     }
     else if(1 == i)
     {
       pb_item.has_sint = true;
       pb_item.sint = config_ptr->tilt_tracker_config_reg.transportation_type;
     }

    else if(2 == i)
     {
       pb_item.has_sint = true;
       pb_item.sint = config_ptr->tilt_tracker_config_reg.max_init_trial_num;
     }
     else if(3 == i)
     {
       pb_item.has_sint = true;
       pb_item.sint = config_ptr->tilt_tracker_config_reg.max_update_trial_num;
     }
     else if(4 == i)
     {
       pb_item.has_flt = true;
       pb_item.flt = config_ptr->tilt_tracker_config_reg.init_gravity_threshold;
     }
     else if(5 == i)
     {
       pb_item.has_flt = true;
       pb_item.flt = config_ptr->tilt_tracker_config_reg.update_gravity_threshold;
     }
     else if(6 == i)
     {
       pb_item.has_sint = true;
       pb_item.sint = config_ptr->tilt_tracker_config_reg.iteration_delay;
     }

     if (!pb_encode_tag_for_field(stream, field))
     {
       CM_MSG_HIGH_0("tilt_tracker: registry write Error tag item: ");
       return false;
     }

     if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
     {
       CM_MSG_HIGH_0("tilt_tracker: registry write Error item: ");
       return false;
     }
    }
    return true;
    }

    bool cm_fmd_registry_write_cb(
    struct pb_ostream_s *stream,
    struct pb_field_s const *field,
    void *const *arg)
    {
    UNUSED_VAR(arg);
    char const *names[] = {
                          "threshold_accel",
                          "accel_buffer_count",
                          "next_run_duration"};

      cm_sns_reg_config_s_type *config_ptr = get_reg_config();
    for(int i = 0; i < ARR_SIZE(names); i++)
    {


      if(0 == i && config_ptr->fmd_config_reg.threshold_accel ==0xFFFFFFFF )
     {
      continue;
     }
     if(1 == i && config_ptr->fmd_config_reg.accel_buffer_count ==0xFFFFFFFF)
     {
       continue;
     }
      if(2 == i && config_ptr->fmd_config_reg.run_period ==0xFFFFFFFF)
     {
       continue;
     }
     
     pb_buffer_arg name_data = (pb_buffer_arg)
       { .buf = names[i], .buf_len = strlen(names[i]) + 1 };

     sns_registry_data_item pb_item = sns_registry_data_item_init_default;
     pb_item.name.funcs.encode = &cm_encode_payload;
     pb_item.name.arg = &name_data;


     if(0 == i  )
     {
       pb_item.has_flt = true;
       pb_item.flt = config_ptr->fmd_config_reg.threshold_accel;
     }
     else if(1 == i )
     {
       pb_item.has_sint = true;
       pb_item.sint = config_ptr->fmd_config_reg.accel_buffer_count;
     }
     else if(2 == i )
     {
       pb_item.has_sint = true;
       pb_item.sint = config_ptr->fmd_config_reg.run_period;
     }

     if (!pb_encode_tag_for_field(stream, field))
     {
       return false;
     }

     if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
     {
       CM_MSG_HIGH_0("tilt_tracker: registry write Error item: ");
       return false;
     }
    }
    return true;
    }

    bool cm_mcd_registry_write_cb(
      struct pb_ostream_s *stream,
      struct pb_field_s const *field,
      void *const *arg)
    {
      UNUSED_VAR(arg);
      char const *names[] = {"tgl_dly_thld",
                             "tgl_cnt_thld",
                             "psm_timer_cfg"};

      cm_sns_reg_config_s_type *config_ptr = get_reg_config();
      for(int i = 0; i < ARR_SIZE(names); i++)
      {
         if(0 == i && config_ptr->mcd_config_reg.mcd_motion_toggle_delay_threshold == 0xFFFF)
        {
          continue;
        }
        else if(1 == i && config_ptr->mcd_config_reg.mcd_motion_toggle_counter_threshold == 0xFF)
        {
          continue;
        }
        else if(2 == i && config_ptr->mcd_config_reg.mcd_psm_timer_config == 0xFF)
        {
          continue;
        }
        pb_buffer_arg name_data = (pb_buffer_arg)
          { .buf = names[i], .buf_len = strlen(names[i]) + 1 };

        sns_registry_data_item pb_item = sns_registry_data_item_init_default;
        pb_item.name.funcs.encode = &pb_encode_string_cb;
        pb_item.name.arg = &name_data;
       
         
        CM_MSG_HIGH_1("mcd_motion_toggle_delay_threshold: %d",config_ptr->mcd_config_reg.mcd_motion_toggle_delay_threshold  );    
        if(0 == i )
        {
          pb_item.has_sint = true;
          pb_item.sint = config_ptr->mcd_config_reg.mcd_motion_toggle_delay_threshold;
        }
        else if(1 == i )
        {
          pb_item.has_sint = true;
          pb_item.sint = config_ptr->mcd_config_reg.mcd_motion_toggle_counter_threshold;
        }
        else if(2 == i )
        {
          pb_item.has_sint = true;
          pb_item.sint = config_ptr->mcd_config_reg.mcd_psm_timer_config;
        }

        if (!pb_encode_tag_for_field(stream, field))
        {
          //SNS_LOGE("mcd: registry write Error tag item: %d", i);
          return false;
        }

        if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
        {
          //SNS_LOGE("mcd: registry write Error item: %d", i);
          return false;
        }
      }
      return true;
    }

      bool cm_md_registry_write_cb(
      struct pb_ostream_s *stream,
      struct pb_field_s const *field,
      void *const *arg)
    {
      UNUSED_VAR(arg);
      char const *names[] = {"thresh",
                             "disable",
                             "win"};
      cm_sns_reg_config_s_type *config_ptr = get_reg_config();

      for(int i = 0; i < ARR_SIZE(names); i++)
      {

        if(0 == i  && config_ptr->md_config_reg.thresh == 0xFFFFFFFF)
        {
          continue;
        }
        
        pb_buffer_arg name_data = (pb_buffer_arg)
          { .buf = names[i], .buf_len = strlen(names[i]) + 1 };

        sns_registry_data_item pb_item = sns_registry_data_item_init_default;
        pb_item.name.funcs.encode = &pb_encode_string_cb;
        pb_item.name.arg = &name_data;

        if(0 == i  )
        {
          pb_item.has_flt = true;
          pb_item.flt = config_ptr->md_config_reg.thresh;
        }
        else if(1 == i )
        {
          pb_item.has_sint = true;
          pb_item.sint = 0;
        }
        else if(2 == i)
        {
          pb_item.has_flt = true;
          pb_item.flt = 0;
        }

        if (!pb_encode_tag_for_field(stream, field))
        {
         // SNS_LOGE("tilt_tracker: registry write Error tag item: %d", i);
          return false;
        }

        if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
        {
          //SNS_LOGE("tilt_tracker: registry write Error item: %d", i);
          return false;
        }
      }
      return true;
    }


    bool cm_send_registry_write_req(qmi_client_type qmi_sensors_handle, sns_std_suid uid, cm_sns_id_e_type current_sensor_type)
    {

    void *encoded_req = NULL;
    size_t encoded_req_len;
    bool rv = FALSE;
    cm_sns_reg_config_s_type *config_ptr = get_reg_config();

    switch( current_sensor_type)
    {

           case CM_SNS_TYPE_FMD:
        
       /************ fmd *************/
       encoded_req_len = cm_get_encoded_registry_write_req(&encoded_req, "sns_flight_mode.threshold", &cm_fmd_registry_write_cb);
       if(NULL != encoded_req)
       {
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ);

             cm_mem_free(encoded_req);
              config_write_counter++;
         CM_MSG_HIGH_0("  send registry write FMD ");
       }
       
       break;

       case CM_SNS_TYPE_TILT:
       /************ tilt_tracker *************/
       encoded_req_len = cm_get_encoded_registry_write_req(&encoded_req, "sns_tilt_tracker", &cm_tilt_tracker_registry_write_cb);
       if(NULL != encoded_req)
       {
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ);

             cm_mem_free(encoded_req);
             config_write_counter++;
          CM_MSG_HIGH_0("  send registry write tilt");
       }
       break;
       
     case CM_SNS_TYPE_SMD: 

       /************ smd_tracker *************/
     if(config_ptr->smd_config_reg.smd_sum_score_threshold!= 0xFF)
     {
       encoded_req_len = cm_get_encoded_registry_write_req(&encoded_req, "sns_smd_tracker.threshold", &cm_smd_tracker_sum_score_thresh_registry_write_cb);
       if(NULL != encoded_req)
       {
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ);

             cm_mem_free(encoded_req);
             config_write_counter++;
             CM_MSG_HIGH_0("  send registry write SMD ");
       }
     }

       if(config_ptr->smd_config_reg.smd_perceptron_weight[0]!= 0xFFFFFFFF)
     {
       encoded_req_len = cm_get_encoded_registry_write_req(&encoded_req, "sns_smd_tracker.percep_weights", &cm_smd_tracker_percep_weights_registry_write_cb);
       if(NULL != encoded_req)
       {
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ);

             cm_mem_free(encoded_req);
             config_write_counter++;
         CM_MSG_HIGH_0("  send registry write SMD ");
       }
     }
       if(config_ptr->smd_config_reg.smd_perceptron_min_weight[0]!= 0xFFFFFFFF)
     {
       encoded_req_len = cm_get_encoded_registry_write_req(&encoded_req, "sns_smd_tracker.percep_min_features", &cm_smd_tracker_percep_min_features_registry_write_cb);
       if(NULL != encoded_req)
       {
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ);

             cm_mem_free(encoded_req);
             config_write_counter++;
       CM_MSG_HIGH_0("  send registry write SMD ");
       }
      }
       if(config_ptr->smd_config_reg.smd_perceptron_max_weight[0]!= 0xFFFFFFFF)
     {
       encoded_req_len = cm_get_encoded_registry_write_req(&encoded_req, "sns_smd_tracker.percep_max_features", &cm_smd_tracker_percep_max_features_registry_write_cb);
       if(NULL != encoded_req)
       {
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ);

             cm_mem_free(encoded_req);
           config_write_counter++;
      CM_MSG_HIGH_0("  send registry write SMD ");
       }
     }
       break;



       break;
       case CM_SNS_TYPE_MOTION_DETECT:

       /************ motion_detect *************/
       encoded_req_len = cm_get_encoded_registry_write_req(&encoded_req, "lsm6dso_0_platform.md.config", &cm_md_registry_write_cb);
       if(NULL != encoded_req)
       {
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ);

             cm_mem_free(encoded_req);
             config_write_counter++;
             CM_MSG_HIGH_0("  send registry write MD ");
       }
       break;

      case CM_SNS_TYPE_MCD:
        /************ MCD  *************/
           
       encoded_req_len = cm_get_encoded_registry_write_req(&encoded_req, "sns_mcd_tracker.threshold", &cm_mcd_registry_write_cb);
       if(NULL != encoded_req)
       {
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ);

             cm_mem_free(encoded_req);
             config_write_counter++;
             CM_MSG_HIGH_0("  send registry write MCD ");
       }    
      
       break;

       default: 
        CM_MSG_HIGH_1("Invalid sns: %d ", current_sensor_type);

    }
        return TRUE;
        }

        
    size_t cm_get_encoded_registry_read_req(
    void **encoded_req,
    char* group_name,
    uint8_t group_name_len)
    {
    size_t encoded_req_size;
    sns_registry_read_req read_req = sns_registry_read_req_init_default;
    pb_buffer_arg name_data = (pb_buffer_arg) {
    .buf = group_name, .buf_len = (group_name_len + 1)
    };

    read_req.name.arg = &name_data;
    read_req.name.funcs.encode = pb_encode_string_cb;

    *encoded_req = NULL;

    if(!pb_get_encoded_size(&encoded_req_size, sns_registry_read_req_fields,
      &read_req))
    {
    CM_MSG_HIGH_0("pb_get_encoded_size error");
    }
    else
    {
    void *encoded_buffer = cm_mem_malloc(encoded_req_size);
    if(NULL != encoded_buffer)
    {
      pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

      if(!pb_encode(&stream, sns_registry_read_req_fields, &read_req))
      {
        CM_MSG_HIGH_1("Error Encoding sns_registry_read_req request: %s", PB_GET_ERROR(&stream));
      }
      else
      {
        *encoded_req = encoded_buffer;
      }
    }
    }

    return NULL == *encoded_req ? 0 : encoded_req_size;
    }


    bool cm_send_registry_read_req(qmi_client_type qmi_sensors_handle, sns_std_suid uid, cm_sns_id_e_type current_sensor_type)
    {

    void *encoded_req = NULL;
    size_t encoded_req_len;
    bool rv = FALSE;

    switch( current_sensor_type)
    {
      case CM_SNS_TYPE_FMD:
      CM_MSG_HIGH_0("FMD read registry req");
       /************ fmd *************/
      config_read_counter++;
       encoded_req_len = cm_get_encoded_registry_read_req(&encoded_req, "sns_flight_mode.threshold", strlen("sns_flight_mode.threshold"));
       if(NULL != encoded_req)
       {
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ);

             cm_mem_free(encoded_req);
       }
       
       break;

       case CM_SNS_TYPE_TILT:
       /************ tilt_tracker *************/
      config_read_counter++;
       encoded_req_len = cm_get_encoded_registry_read_req(&encoded_req, "sns_tilt_tracker", strlen("sns_tilt_tracker"));
       if(NULL != encoded_req)
       {
         CM_MSG_HIGH_0("tilt read registry req");
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ);

             cm_mem_free(encoded_req);

       }
       break;
       
     case CM_SNS_TYPE_SMD: 

      config_read_counter++;
       /************ smd_tracker *************/
     CM_MSG_HIGH_0("SMD read registry req");
       encoded_req_len = cm_get_encoded_registry_read_req(&encoded_req, "sns_smd_tracker.threshold", strlen("sns_smd_tracker.threshold"));
       if(NULL != encoded_req)
       {
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ);

             cm_mem_free(encoded_req);
       }
       config_read_counter++;
       encoded_req_len = cm_get_encoded_registry_read_req(&encoded_req, "sns_smd_tracker.percep_weights", strlen("sns_smd_tracker.percep_weights"));
       if(NULL != encoded_req)
       {
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ);

             cm_mem_free(encoded_req);

       }
       config_read_counter++;
       encoded_req_len = cm_get_encoded_registry_read_req(&encoded_req, "sns_smd_tracker.percep_min_features", strlen("sns_smd_tracker.percep_min_features"));
       if(NULL != encoded_req)
       {
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ);

             cm_mem_free(encoded_req);

       }
       config_read_counter++;
       encoded_req_len = cm_get_encoded_registry_read_req(&encoded_req, "sns_smd_tracker.percep_max_features", strlen("sns_smd_tracker.percep_max_features"));
       if(NULL != encoded_req)
       {
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ);

             cm_mem_free(encoded_req);

       }
       break;

       case CM_SNS_TYPE_MOTION_DETECT:

       config_read_counter++;
       /************ motion_detect *************/
       encoded_req_len = cm_get_encoded_registry_read_req(&encoded_req, "lsm6dso_0_platform.md.config", strlen("lsm6dso_0_platform.md.config"));
       if(NULL != encoded_req)
       {
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ);

             cm_mem_free(encoded_req);
          CM_MSG_HIGH_0("MD read registry req");
       }
       break;

      case CM_SNS_TYPE_MCD:
        config_read_counter++;
        /************ MCD  *************/  
       encoded_req_len = cm_get_encoded_registry_read_req(&encoded_req, "sns_mcd_tracker.threshold", strlen("sns_mcd_tracker.threshold"));
       if(NULL != encoded_req)
       {
         pb_buffer_arg payload = (pb_buffer_arg)
             { .buf = encoded_req, .buf_len = encoded_req_len };
         rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid, SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ);

             cm_mem_free(encoded_req);

              CM_MSG_HIGH_0("MCD read");
       }    
        break;
      
       default: CM_MSG_HIGH_1("Invalid sns: %d ", current_sensor_type);

    }

    return TRUE;
    }
#endif

/*===========================================================================
FUNCTION cm_get_encoded_physical_sensor_test_config

DESCRIPTION
 send req to SEE
  
DEPENDENCIES
none

RETURN VALUE
boolean

SIDE EFFECTS
none

===========================================================================*/

size_t cm_get_encoded_physical_sensor_test_config(void **encoded_req, uint8_t test_type)
{
  size_t encoded_req_size;
  sns_physical_sensor_test_config physical_test = sns_physical_sensor_test_config_init_default;
  physical_test.test_type = test_type;

  *encoded_req = NULL;

  if(!pb_get_encoded_size(&encoded_req_size, sns_physical_sensor_test_config_fields, &physical_test))
  {
    CM_MSG_HIGH_0("pb_get_encoded_size error");
  }
  else
  {
    void *encoded_buffer = cm_mem_malloc(encoded_req_size);
    if(NULL != encoded_buffer)
    {
      pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

      if(!pb_encode(&stream, sns_physical_sensor_test_config_fields, &physical_test))
      {
        CM_MSG_HIGH_1("Error Encoding physical sensor test request: %s", PB_GET_ERROR(&stream));
      }
      else
      {
        *encoded_req = encoded_buffer;
      }
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}


/*===========================================================================
FUNCTION cm_send_sns_phy_test_req

DESCRIPTION
 send req to SEE
 
DEPENDENCIES
none

RETURN VALUE
boolean

SIDE EFFECTS
none

===========================================================================*/

bool cm_send_sns_phy_test_req(qmi_client_type qmi_sensors_handle, sns_std_suid uid, uint8 test_type) 
{
  /*
   * Suppport factory test sensor data_type:
   * 1. accel
   * 2. gyro
   * 3. ambient_light
   * 4. ambient_temperature
   * 5. humidity
   */
    bool rv = false;
    void *encoded_req = NULL;
    size_t encoded_req_len;

    encoded_req_len = cm_get_encoded_physical_sensor_test_config(&encoded_req, test_type);
    if(NULL != encoded_req)
    {
      pb_buffer_arg payload = (pb_buffer_arg)
               { .buf = encoded_req, .buf_len = encoded_req_len };
      rv = cm_send_basic_req(qmi_sensors_handle, &payload, uid,
                          SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG);
      cm_mem_free(encoded_req);
    }
	return rv;
}

/**
 * Each SUID event contains an array of SUIDs.  This function will be called
 * once per each SUID in the array.
 *
 * At this time we will send an attributes request to the SUID.
 */

 /*===========================================================================
 FUNCTION cm_decode_suid

 DESCRIPTION
  it helps to decode the suid

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_decode_suid(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  int *len = (int*)(*arg);
  sns_std_suid uid;

  if(!pb_decode(stream, sns_std_suid_fields, &uid)) 
  {
    CM_MSG_HIGH_1("=CM= Error decoding SUID: %s", PB_GET_ERROR(stream));
    return false;
  }

  CM_MSG_HIGH_4("=CM= Received SUID Event with SUID 0x %x , 0x %x len = %d, current_sensor = %d",
      uid.suid_low, uid.suid_high,(*len), current_sensor_type);
  if( ((*len) < 2) && ((*len) >= 0))
  {
    qmi_temp_suid[*len].suid_low = uid.suid_low;
	qmi_temp_suid[*len].suid_high = uid.suid_high;
  }
   (*len)++;
  if (current_sensor_type == CM_SNS_TYPE_ALS 
 #ifdef FEATURE_MMODE_TX_21 	
  	&& cm_sns_get_phy_test_config()->cm_sns_phy_test_req == FALSE
 #endif
   )
  {
	CM_MSG_HIGH_0(" sensor type ALS so sending attribute request");
	cm_send_attr_req(qmi_sensors_handle, &uid);  
  }
  #ifdef FEATURE_MMODE_TX_21 	  
  else if(current_sensor_type == CM_SNS_TYPE_TEMPERATURE && cm_sns_get_phy_test_config()->serial_number_req == TRUE)
  {
	  CM_MSG_HIGH_0(" sensor type ambient_temperature so sending attribute request");
	  cm_sns_get_phy_test_config()->serial_number_req = FALSE;
	  cm_send_attr_req(qmi_sensors_handle, &uid);
	  cm_sns_update_suid(uid,current_sensor_type);
  }
  #endif
  else
  {
 #ifdef FEATURE_MMODE_TX_21

   if(registry_write == TRUE)
    {
      CM_MSG_HIGH_0(" Decode SUID registry write completed ");
      registry_write = FALSE;  
      cm_send_registry_write_req(qmi_sensors_handle, uid,write_registry_current_sensor_type);        
    }
   else if(registry_read == TRUE)
    {
      CM_MSG_HIGH_0(" Decode SUID registry read completed ");
      registry_read = FALSE;
     cm_send_registry_read_req(qmi_sensors_handle, uid,read_registry_current_sensor_type);
    }
    /*If this is set, this means we have suid for sns_phy_test, in that case we need not enable the sensor*/
    if(cm_sns_get_phy_test_config()->cm_sns_phy_test_req == FALSE &&
         current_sensor_type != CM_SNS_TYPE_NONE && current_sensor_type != CM_SNS_TYPE_REGISTRY )
 #endif		
	{
	  cm_send_sns_enable_req(qmi_sensors_handle,&uid);  
	  /***new change **/
  	}
    cm_sns_update_suid(uid,current_sensor_type);
 
#ifdef FEATURE_MMODE_TX_21
	if(cm_sns_get_phy_test_config()->cm_sns_phy_test_req == TRUE)
    {
	  /**** change ***/
      cm_sns_get_phy_test_config()->cm_sns_phy_test_req = FALSE;
	  
      cm_send_sns_phy_test_req(qmi_sensors_handle, uid, cm_sns_get_phy_test_config()->cm_sns_test_type);        
	}
	if(cm_sns_get_phy_test_config()->cm_sns_phy_test_get_data_req == TRUE)
	{
      cm_sns_get_phy_test_config()->next_report_time = time_get_secs() + cm_sns_get_phy_test_config()->report_rate;
	}
#endif	
  }

  return true;
}


/**
 * Decode the payload of the event message, i.e. the SUID Event
 *
 * @param[i] arg Message ID
 */
 /*===========================================================================
 FUNCTION cm_decode_suid_event

 DESCRIPTION
  it decode the suid event

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_decode_suid_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  int len = 0;
  sns_suid_event suid_event;
  pb_buffer_arg data;
  char *data_type = NULL; 
  suid_event.suid.funcs.decode = &cm_decode_suid;
  suid_event.suid.arg = &len;
  suid_event.data_type.funcs.decode = &cm_decode_payload;
  suid_event.data_type.arg = &data;

  if(!pb_decode(stream, sns_suid_event_fields, &suid_event))
  {
    CM_MSG_HIGH_1(" =CM= Error decoding SUID Event: %s", PB_GET_ERROR(stream));
    return false;
  }
  

  {
	CM_MSG_HIGH_1(" =CM= Received SUID Event with suid len = %d ", len);
    CM_MSG_HIGH_1(" =CM= Received SUID Event with data buf len = %d ", data.buf_len);
   
  }

  return true;
}



 /*===========================================================================
 FUNCTION cm_get_sns_msg_id

 DESCRIPTION
  it will return config msg id

 DEPENDENCIES
 none

 RETURN VALUE
 uint32

 SIDE EFFECTS
 none

 ===========================================================================*/

static uint32_t cm_get_sns_msg_id(pb_istream_t *stream)
{
  sns_client_event_msg_sns_client_event event =
    sns_client_event_msg_sns_client_event_init_default;

  if(!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event))
  {
    CM_MSG_HIGH_1(" =CM= event: %s", PB_GET_ERROR(stream));
  }
  else
  {
    CM_MSG_HIGH_2("=CM=Decoding event with message ID %d, timestamp 0x %x",
        event.msg_id, event.timestamp);
    sample_time_stamp = event.timestamp;
    return event.msg_id;
  }

  return 0;
}

/*===========================================================================
 FUNCTION cm_decode_events

 DESCRIPTION
   it will decode the sensors event
 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static bool cm_decode_events(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  bool rv = true;
  sns_client_event_msg_sns_client_event event =
    sns_client_event_msg_sns_client_event_init_default;
  pb_istream_t stream_cpy = *stream;


  CM_MSG_HIGH_0("=CM= Begin decoding event");
  uint32_t msg_id = cm_get_sns_msg_id(&stream_cpy);

  event.payload.funcs.decode = *arg;
  event.payload.arg = &msg_id;

  if(!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event))
  {
    CM_MSG_HIGH_1("=CM= Error decoding Event: %s", PB_GET_ERROR(stream));
    rv = false;
  }

  CM_MSG_HIGH_0(" =CM= Event decoding complete");
  return rv;
}

/**
 * Handle a received event message from the Sensor Service.
 *
 * This example code only ever sends requests to the SUID Lookup Sensor
 * and an Accel Sensor.  Therefore we only need to check against those two
 * SUIDs below.  If this client were to use more Sensors, either:
 *  - The client would open separate QMI connections for each Sensor
 *    (and then the SUID would be known implicitly because of the indication
 *    callback function the event was received from).
 *  - Save the SUIDs in some sort of global state, so that they can be used
 *    for comparison here.
 *
 * @param[i] suid Source Sensor of this event
 * @param[i] event_msg PB-encoded message of type sns_client_event_msg
 * @param[i] event_msg_len Length (in bytes) of the encoded event_msg
 */

 /*===========================================================================
 FUNCTION cm_handle_event_msg

 DESCRIPTION
   This function handle the sensors event message

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/

static void cm_handle_event_msg(sns_std_suid const *suid, void const *event_msg, size_t event_msg_len)
{
  pb_istream_t stream;
  sns_client_event_msg event = sns_client_event_msg_init_default;
  CM_MSG_HIGH_2(" =CM= Processing events from SUID low = 0x %x high = 0x %x",
      suid->suid_low, suid->suid_high);
  CM_MSG_HIGH_2("Look up low = 0x %x and high = 0x %x",suid_lookup.suid_low,suid_lookup.suid_high);

  if(suid->suid_high == suid_lookup.suid_high && suid->suid_low == suid_lookup.suid_low) {
    event.events.funcs.decode = &cm_decode_events;
    event.events.arg = &cm_decode_suid_event;
  } else {
    curr_sns_suid = *suid;
	/*****change *******/
     current_sensor_type = cm_sns_find_sns_by_suid(curr_sns_suid);
	 qmi_sensors_handle = cm_sns_get_current_qmi_handle(curr_sns_suid);
	/******change *****/
    event.events.funcs.decode = &cm_decode_events;
    event.events.arg = &cm_handle_sensor_event;
  }

  stream = pb_istream_from_buffer(event_msg, event_msg_len);
  if(!pb_decode(&stream, sns_client_event_msg_fields, &event)) 
  {
     CM_MSG_HIGH_1("=CM= Error decoding event list: %s", PB_GET_ERROR(&stream));
  }
}
/* QMI indication callback.  See qmi_client_ind_cb. */

/*===========================================================================
FUNCTION cm_qmi_ind_msg_cb

DESCRIPTION

QMI sensor call this call back, if there is any response

DEPENDENCIES
none

RETURN VALUE
boolean

SIDE EFFECTS
none

===========================================================================*/
static void cm_qmi_ind_msg_cb(qmi_client_type user_handle, unsigned int msg_id,
                          void *ind_buf, unsigned int ind_buf_len, void *ind_cb_data)
{

  CM_MSG_HIGH_4("cm_qmi_ind_msg_cb uhad = 0x %x msgid = %d indbuf = 0x %x indbuflen = %d ",user_handle,
  	                                msg_id,ind_buf,ind_buf_len);
  cmsns_snsmod_send_sns_qmi_cmd_rsp(user_handle,
                         msg_id,
                         ind_buf,
                         ind_buf_len);


}


/*===========================================================================
FUNCTION cm_sns_process_qmi_sns_cmd_rsp

DESCRIPTION

QMI sensor call this call back, if there is any response
/* typedef void           *qmi_client_type; in qmi_client.h"

DEPENDENCIES
none

RETURN VALUE
boolean

SIDE EFFECTS
none

===========================================================================*/
void cm_sns_process_qmi_sns_cmd_rsp(void * user_handle, unsigned int msg_id,
                          void *ind_buf, unsigned int ind_buf_len)
{
  UNUSED_VAR(user_handle);
  int32_t qmi_err;
  size_t ind_len =0; //sizeof(sns_client_report_ind_msg_v01);
  sns_client_report_ind_msg_v01 *ind = NULL; //cm_mem_malloc(ind_len);
  sns_client_jumbo_report_ind_msg_v01 *ind_jumbo = NULL;
  
  CM_MSG_HIGH_2("user_handle = 0x %x and msg id = %d ",user_handle,msg_id);
  curr_qmi_sensors_handle = user_handle;

  if(SNS_CLIENT_REPORT_IND_V01 == msg_id)
  	{
      ind_len = sizeof(sns_client_report_ind_msg_v01); 	  
	  ind = (sns_client_report_ind_msg_v01 *)cm_mem_malloc(ind_len);
	  qmi_err = qmi_idl_message_decode(SNS_CLIENT_SVC_get_service_object_v01(),
      QMI_IDL_INDICATION, msg_id, ind_buf, ind_buf_len, ind, ind_len);

      if(QMI_IDL_LIB_NO_ERR != qmi_err)
      {
        CM_MSG_HIGH_1("=CM= QMI decode error %i", qmi_err);
      } 
      else
      {
        sns_client_event_msg event = sns_client_event_msg_init_default;
        CM_MSG_HIGH_1("=CM= Indication from client ID %d", ind->client_id);       
   
        pb_istream_t stream = pb_istream_from_buffer(ind->payload, ind->payload_len);
        if(pb_decode(&stream, sns_client_event_msg_fields, &event))
        {
          cm_handle_event_msg(&event.suid, ind->payload, ind->payload_len);
        } 
        else
        {
          CM_MSG_HIGH_1("=CM= Error decoding Event Message: %s", PB_GET_ERROR(&stream));
        }
      }
	    if(ind != NULL)
        cm_mem_free(ind);
  	}
    else if(SNS_CLIENT_JUMBO_REPORT_IND_V01 == msg_id)
    {
      CM_MSG_HIGH_0("Inside Jumbo");
      ind_len = sizeof(sns_client_jumbo_report_ind_msg_v01);
	  ind_jumbo = (sns_client_jumbo_report_ind_msg_v01 *)cm_mem_malloc(ind_len);
	  qmi_err = qmi_idl_message_decode(SNS_CLIENT_SVC_get_service_object_v01(),
      QMI_IDL_INDICATION, msg_id, ind_buf, ind_buf_len, ind_jumbo, ind_len);

        if(QMI_IDL_LIB_NO_ERR != qmi_err)
      {
        CM_MSG_HIGH_1("=CM= QMI decode error %i", qmi_err);
      } 
      else
      {
        sns_client_event_msg event = sns_client_event_msg_init_default;
        CM_MSG_HIGH_1("=CM= Indication from client ID %d", ind_jumbo->client_id);
        
        // Decode just the sns_client_event_msg in order to get the SUID
        pb_istream_t stream = pb_istream_from_buffer(ind_jumbo->payload, ind_jumbo->payload_len);
        if(pb_decode(&stream, sns_client_event_msg_fields, &event))
        {
          cm_handle_event_msg(&event.suid, ind_jumbo->payload, ind_jumbo->payload_len);
        } 
        else
        {
          CM_MSG_HIGH_1("=CM= Error decoding Event Message: %s", PB_GET_ERROR(&stream));
        }
      }
	  if(ind_jumbo != NULL)
      cm_mem_free(ind_jumbo);
    }
	cm_mem_free(ind_buf);
	CM_MSG_HIGH_2("Received Indication ind_buf_len %d local_buf_len = %d", ind_buf_len,ind_len);
   
    CM_MSG_HIGH_0(" =CM= Indication processing completed");
}

/**
 * Create a QMI connection to Sensors
 *
 * @param[i] qmi_handle QMI Handle created in cm_wait_for_service
 * @param[i] timeout Time-out in milliseconds. 0 = no timeout
 *
 * @return True on success; false otherwise
 */

 /*===========================================================================
 FUNCTION cm_create_client_conn

 DESCRIPTION
   it will create the client connection
 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/
static bool cm_create_client_conn(qmi_client_type *qmi_handle, uint32_t timeout)
{
  bool rv = false;
  int i =0;
  qmi_client_error_type qmi_err;
  qmi_service_info svc_info_array[5] = {0};
  qmi_service_info svc_info;
  unsigned int num_services = 0, num_entries = 5;

   for(i = 0 ; i < num_entries ; i++)  
   {
     memset(&svc_info_array[i], 0, sizeof(svc_info_array[i]));
   }

   qmi_err = qmi_client_get_service_list(service_obj, svc_info_array, 
   	                                  &num_entries, &num_services);
   if(QMI_NO_ERR != qmi_err)
   {
	 CM_MSG_HIGH_1("=CM= qmi_client_get_service_list error %d", qmi_err);
   }
   else if(num_entries == 0)
   {
     CM_MSG_HIGH_0("sensors service has no available instances");
   }
   else
   {
     /* As only one qmi service is expected for sensors, use the 1st instance */   
	 svc_info = svc_info_array[0];
	 qmi_err = qmi_client_init(&svc_info, service_obj, cm_qmi_ind_msg_cb,  
			                   (void*)&cm_init_svr_discvery_handle, &os_params, qmi_handle);
	 if(QMI_NO_ERR != qmi_err)
     {
	   CM_MSG_HIGH_1("=CM= qmi_client_init error %d", qmi_err);
     }
	 else
	 {
	   qmi_err = qmi_client_register_error_cb(*qmi_handle, cm_sns_qmi_error_cb, NULL);
	   
       if(QMI_NO_ERR != qmi_err)
       {
         CM_MSG_HIGH_1("=CM= qmi_client_register_error_cb error %d", qmi_err);
       }
       else
       {
         rv = true;
       }

	 }
   }
 
   if(!rv)
    {
      if(NULL != *qmi_handle)
      {
         qmi_client_release(*qmi_handle);
        *qmi_handle = NULL;
      }
    }
  else
  {
    CM_MSG_HIGH_1 (" client connection Successful QMI handle = 0x %x",*qmi_handle);
  }

  return rv;
}

/**
 * Check whether the Sensor service is available.
 *
 * @param[i] timeout Timeout period in ms; 0 infinite
 *
 * @return true if the service is available, false upon timeout
 */
 /*===========================================================================
 FUNCTION cm_wait_for_service

 DESCRIPTION
  wait until the services are available

 DEPENDENCIES
 none

 RETURN VALUE
 boolean

 SIDE EFFECTS
 none

 ===========================================================================*/
static bool cm_wait_for_service(uint32_t timeout)
{
  service_obj = SNS_CLIENT_SVC_get_service_object_v01();
  qmi_client_error_type qmi_err;
  bool rv = true;
  
  os_params.tcb = rex_self();
  os_params.sig = CM_OS_FLAG_QMI_CCI_EVENT;

  CM_MSG_HIGH_0("=CM= Waiting for service");
  CM_MSG_HIGH_1(" Value for service object is 0x %x ",service_obj);


  qmi_err = qmi_client_notifier_init(service_obj, &os_params, &notifier_handle);
  CM_MSG_HIGH_3(" OS param signal 0x %x and  notifier handle = 0x %x TIMER SIg = %d ",os_params.sig,notifier_handle,os_params.timer_sig);
  if(QMI_NO_ERR != qmi_err)
  {
    CM_MSG_HIGH_1("=CM= qmi_client_notifier_init error %i", qmi_err);
    rv = false;
  } 
  else
  {
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, timeout);
    if(os_params.timed_out)
    {
      CM_MSG_HIGH_1("=CM= Service is not available after %i timeout", timeout);
      rv = false;
    }
  }

  CM_MSG_HIGH_0("=CM= Waiting complete");
  return rv;
}


/*===========================================================================
FUNCTION cm_send_sns_req

DESCRIPTION
send the request to the sensor service

DEPENDENCIES
none

RETURN VALUE
boolean

SIDE EFFECTS
none
static qmi_client_type non_rel_client_qmi_handle = NULL;
static qmi_client_type rel_client_qmi_handle = NULL;

===========================================================================*/
	boolean cm_send_sns_req(cm_sns_id_e_type sensor_type)
	{
	
	   char *sensor_name = cm_get_sensor_name_by_sensor_type(sensor_type);
	   current_sensor_type = sensor_type;
	   if(sensor_name == NULL)
	   {
		 return 0;
	   }
	   
	   if(cm_if_qmi_init == FALSE)
	   {
		 if(!cm_wait_for_service(200))
		 {
	
		   if(NULL != notifier_handle)
		   {
			 qmi_client_release(notifier_handle);
			 notifier_handle = NULL;
		   }
		   return 0;
	
		 }
	   
		 if(!cm_create_client_conn(&qmi_sensors_handle, 200))
		 {
		   if(NULL != notifier_handle)
		   {
			 qmi_client_release(notifier_handle);
			 notifier_handle = NULL;
		   }
		   return 0;
		 }
		 
		 cm_if_qmi_init = TRUE;
		 
		 if(NULL != notifier_handle)
		 {
		   qmi_client_release(notifier_handle);
		   notifier_handle = NULL;
		 }
	   }
	   
		if(!cm_send_suid_req(qmi_sensors_handle, sensor_name))
		 return 0;
	 return TRUE;
}

#ifdef FEATURE_MMODE_TX_21



void cm_send_registry_read_req_wrapper(cm_sns_id_e_type sns_id)
{
  registry_read = TRUE;
  read_registry_current_sensor_type = sns_id;
  cm_send_sns_req(CM_SNS_TYPE_REGISTRY);
}


/*===========================================================================
FUNCTION cm_send_sns_registry_req

DESCRIPTION
send the request to the registry sensor service

DEPENDENCIES
none

RETURN VALUE
boolean

SIDE EFFECTS
none
static qmi_client_type non_rel_client_qmi_handle = NULL;
static qmi_client_type rel_client_qmi_handle = NULL;

===========================================================================*/

boolean cm_send_sns_registry_req(cm_sns_id_e_type sensor_type )
{
    registry_write = TRUE;
   switch(sensor_type)
   {

   case CM_SNS_TYPE_FMD:         
    write_registry_current_sensor_type = CM_SNS_TYPE_FMD;
    break;

   case CM_SNS_TYPE_TILT:
   write_registry_current_sensor_type = CM_SNS_TYPE_TILT;
   break;

   case CM_SNS_TYPE_SMD:
   write_registry_current_sensor_type = CM_SNS_TYPE_SMD;
   break;
     
   case CM_SNS_TYPE_MOTION_DETECT:
   write_registry_current_sensor_type = CM_SNS_TYPE_MOTION_DETECT;
   break;
   case CM_SNS_TYPE_MCD:
   write_registry_current_sensor_type = CM_SNS_TYPE_MCD;
    break;
       
    default:
    CM_MSG_HIGH_1("Registry not supported for Sns %d",sensor_type );
    return FALSE;
   }
   
   return TRUE;
}

#endif

#if 0
boolean cm_send_sns_req(cm_sns_id_e_type sensor_type)
{

   char *sensor_name = cm_get_sensor_name_by_sensor_type(sensor_type);
   current_sensor_type = sensor_type;
   qmi_sensors_handle = NULL;
   if(sensor_name == NULL)
   {
     return 0;
   }

  if(non_rel_client_qmi_handle == NULL || rel_client_qmi_handle == NULL )
  {
   //if(cm_if_qmi_init == FALSE)
   {
     if(!cm_wait_for_service(200))
     {

	   if(NULL != notifier_handle)
       {
         qmi_client_release(notifier_handle);
         notifier_handle = NULL;
       }
       return 0;

     }
	// cm_if_qmi_init = TRUE;

   	}
   
     if(!cm_create_client_conn(&qmi_sensors_handle, 200))
     {
       if(NULL != notifier_handle)
       {
         qmi_client_release(notifier_handle);
         notifier_handle = NULL;
       }
	   return 0;
     }
	 
   //  cm_if_qmi_init = TRUE;
	 
	
   //}

   if(cm_is_non_release_client(sensor_type) == TRUE )
     {
     non_rel_client_qmi_handle = qmi_sensors_handle;
   }
   else
   {
     rel_client_qmi_handle =   qmi_sensors_handle;              
   }
   
     }

  /**** new code **/
   if(cm_is_non_release_client(sensor_type) == TRUE )
   {
     qmi_sensors_handle = non_rel_client_qmi_handle;
     }
   else
   {
     qmi_sensors_handle =  rel_client_qmi_handle ;              
   }
   /** code end here  **/
   
    if(!cm_send_suid_req(qmi_sensors_handle, sensor_name))
     return 0;
 return TRUE;
}
#endif
/*===========================================================================
FUNCTION cm_release_qmi_client

DESCRIPTION
release the qmi client

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/

void cm_release_qmi_client()
{
  if(qmi_sensors_handle != NULL )
  {
    qmi_client_release(qmi_sensors_handle);
  }
  qmi_sensors_handle = NULL;
  cm_if_qmi_init = FALSE;
  service_obj = NULL ;
  notifier_handle = NULL;  
}


/*===========================================================================
FUNCTION cm_handle_md_interrupt

DESCRIPTION
enable the motion detect interrupt

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/

void cm_handle_md_interrupt(boolean is_enable)
{ 
  //static qmi_client_type qmi_sensors_handle_md = NULL;
  if(is_enable == TRUE)
  {
   cm_send_sns_req(CM_SNS_TYPE_MOTION_DETECT);  
   //qmi_sensors_handle_md = qmi_sensors_handle;
  }
  
}

boolean  cm_sns_if_sns_rel_allowed(cm_sns_id_e_type type)
{
   boolean rel = FALSE;
   #ifdef FEATURE_MMODE_TX_21
   cm_sns_phy_test_config_s_type* config = cm_sns_get_phy_test_config();
   #endif
   switch(type)
    {
     case CM_SNS_TYPE_PRESSURE:
	 case CM_SNS_TYPE_TEMPERATURE:
	 case CM_SNS_TYPE_HUMIDITY:
	 case CM_SNS_TYPE_SMD:	 	
	 case CM_SNS_TYPE_FMD:
	 case CM_SNS_TYPE_ALS:
	 	rel = TRUE;
		break;
	 case CM_SNS_TYPE_ACCEL:
	 #ifdef FEATURE_MMODE_TX_21
	 if(config->cm_sns_phy_test_get_data_req == TRUE)
	 {
            rel = TRUE;
	 }
	 #endif  
	 break;
	 default:
	 	rel = FALSE;
    }
   return rel;

}
/*===========================================================================
FUNCTION cm_disable_sns_stream

DESCRIPTION
disable the sensor stream 

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/

void cm_disable_sns_stream(cm_sns_id_e_type type)
{
	/*  currently only pressure sensor need to disable streaming
    later other sensor can be added if needed	*/


  if(cm_sns_if_sns_rel_allowed(type) == TRUE || cm_sns_als_stream_disable == TRUE )
  {
    cm_disable_sns_stream_by_sensor_id(type);
#if 0

    qmi_sensors_handle = rel_client_qmi_handle;
   if(qmi_sensors_handle != NULL )
   {
     qmi_client_release(qmi_sensors_handle);
   }
    qmi_sensors_handle = NULL;
	rel_client_qmi_handle = NULL;
	cm_sns_als_stream_disable = FALSE;
#endif

  }

	
}

/*===========================================================================
FUNCTION cm_enable_sns_motion_interrupt

DESCRIPTION
disable the SMD stream and enable the motion interrupt

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/

void cm_disable_sns_motion_interrupt(void)
{
  if(qmi_sensors_handle != NULL )
  {
    qmi_client_release(qmi_sensors_handle);
  }

}


/*===========================================================================
FUNCTION cm_disable_smd_stream

DESCRIPTION
disable the SMD stream

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/


void cm_disable_smd_stream()
{
	cm_disable_sns_stream(CM_SNS_TYPE_SMD);
}

/*===========================================================================
FUNCTION cm_disable_sns_stream_by_sensor_id

DESCRIPTION
disable the SMD stream

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/


void cm_disable_sns_stream_by_sensor_id(cm_sns_id_e_type sns_id)
{
  sns_std_suid    sns_suid;
  #ifdef FEATURE_MMODE_TX_21
  cm_sns_phy_test_config_s_type* config = cm_sns_get_phy_test_config();
  #endif
  qmi_client_type qmi_hd = cm_sns_get_current_qmi_handle_by_sensor(sns_id);
  sns_suid.suid_low =  sns_db_global[sns_id].sns_suid.suid_low;
  sns_suid.suid_high =  sns_db_global[sns_id].sns_suid.suid_high;
  CM_MSG_HIGH_1("Disable req sent for sns= %d",sns_id);
  cm_send_sns_disable_req(qmi_hd,&sns_suid);
  #ifdef FEATURE_MMODE_TX_21
  if(config->cm_sns_phy_test_get_data_req == FALSE)
  #endif
  {
    cm_sns_reset_suid_by_sensor_id(sns_id);
  }
}

#endif
