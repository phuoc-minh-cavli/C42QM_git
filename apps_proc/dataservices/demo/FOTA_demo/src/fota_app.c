/*==============================================================================

                             FOTA_APP.C

GENERAL DESCRIPTION
  This file deals with the handling FOTA download through LwM2M

Copyright (c) 2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    -------------------------------------------------------------

==============================================================================*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "fs_public.h"
#include "qapi_lwm2m.h"
#include "qapi_coap.h"
#include "qapi_device_info.h"
#include "timer.h"
#include "fota_app.h"
#include "cJSON.h"
#include "qapi_ssl.h"
#include "fota_util.h"

extern size_t memscpy(void *dst, size_t dst_size, const void *src, size_t src_size);

#define FOTA_APP_FILE_PATH                "/datatx/fota_qapi_file"
#define FOTA_CONFIG_FILE_PATH             "/datatx/fota_config.ini"
#define COAP_SERVERS_SSL_MAX_FRAG_LEN     2048
#define FOTA_INSTANCE_ID_0                0


// FOTA app handler
static qapi_Net_LWM2M_App_Handler_t gHandler_fota = NULL;

static bool push_allowed = false;
static bool pull_allowed = false;


// variable to check the download state
static bool gdownloaded;
// variable to check the update state
static bool gupdate;

/* Global variables */
static int16_t last_block;
static timer_type notify_Timer;
timer_group_type FotaTimerGroup;
qapi_Coap_Session_Hdl_t coap_handle = NULL;
char server_uri[20];
char pkg_name[256];
char pkg_version[256];
uint8_t fota_payload_data[1024];
uint16_t curr_data;
static int fota_file_fd  = -1;

/* Fota config file related parameters */
int lwm2m_memory_avail;
int lwm2m_memory_avail_after_fota;
bool lwm2m_integrity_check;
bool lwm2m_execute_status;
int lwm2m_delivery_method;
int lwm2m_protocol_support;
int lwm2m_fota_blocksize;

/**
 * @fn write_to_efs()
 *
 * @brief This function is used to FOTA image to EFS partition.
 *        "efs_write" would be called only when the image size exceeds 1024 bytes
 *         or the fota image download is completed.
 *
 * @param data       - Fota image data.
 * @param length     - Fota image data length.
 * @param completed  - Fota image download completed.
 *
 * @return qapi_Status_t
 */
qapi_Status_t write_to_efs(uint8_t *data, size_t length, bool completed)
{
  qapi_Status_t ret = QAPI_ERROR;
  int32_t rc = -1;
  bool file_write1 = FALSE;
  bool file_write2 = FALSE;

  if(data == NULL || length == 0)
    return ret;

  /* If the fota image size exceeds the 1024 bytes then write to efs, else copy 
     data into the local variable. When the image download is complete then update
     the efs. */
  if(curr_data + length >= 1024)
  {
    file_write1 = TRUE;
  }
  else
  {
    memscpy(fota_payload_data + curr_data, length, data, length);
    curr_data += length;
    ret = QAPI_OK;
    if(completed)
      file_write2 = TRUE;
  }

  if(file_write1 == TRUE || file_write2 == TRUE)
  {
    if(fota_file_fd == -1)
    {
      fota_file_fd = efs_open(FOTA_APP_FILE_PATH, (O_CREAT | O_RDWR));
    }
    if(fota_file_fd < 0)
    {
      LOG_ERROR("FOTA_LOG: File opened failed");
      fota_file_fd = -1;
      ret           = QAPI_ERROR;
      goto end;
    }
    rc = efs_lseek(fota_file_fd, 0, SEEK_END);
    if(rc == -1)
    {
      ret           = QAPI_ERROR;
      efs_close(fota_file_fd);
      goto end;
    }
    else
    {
      rc = efs_write(fota_file_fd, 
                     fota_payload_data, 
                     curr_data);
    }
    if(rc < curr_data)
    {
      ret           = QAPI_ERROR;
      efs_close(fota_file_fd);
      goto end;
    }
    else
    {
      curr_data = 0;
      memset(fota_payload_data, 0x00, sizeof(fota_payload_data));
      LOG_INFO("Write to efs successful.");
      ret = QAPI_OK;
    }

    if(file_write1)
    {
      rc = efs_lseek(fota_file_fd, 0, SEEK_END);
      if(rc == -1)
      {
        ret           = QAPI_ERROR;
        efs_close(fota_file_fd);
        goto end;
      }
      else
      {
        rc = efs_write(fota_file_fd, 
                       data, 
                       length);
      }
      if(rc < length)
      {
        ret           = QAPI_ERROR;
        efs_close(fota_file_fd);
        goto end;
      }
      else
      {
        ret = QAPI_OK;
        LOG_INFO("Write to efs successful.");
      }
    }
  }
  efs_close(fota_file_fd);
  return ret;
end:
  return ret;
}

/**
 * @fn notify_signal_cb()
 *
 * @brief This callback function is called when the FOTA application is waiting 
 *        for the next packet during the FOTA transfer and the timer expired.
 *
 * @param time_ms      - Timer value.
 * @param userData     - User data.
 *
 * @return void
 */
void notify_signal_cb(int32 time_ms, timer_cb_data_type userData)
{
  // This scenario is used for handling the connection lost with server 
  lwm2m_firmware_set_state(QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E);
  lwm2m_firmware_set_result(QAPI_NET_LWM2M_FOTA_RESULT_CONNECTION_LOST_E);
  return;
}

/**
 * @fn lwm2m_out_band_fota_cb()
 *
 * @brief This function is called when the FOTA application is downloading FOTA
 *        image from Out of band mechanism and this function will notify whether
 *        download succeded or not.
 *
 * @param success      - Download successful or not.
 *
 * @return void
 */
void lwm2m_out_band_fota_cb(bool success)
{
  // Set the LwM2M FOTA state and result based on state machine.
  if(success == true)
  {
    lwm2m_firmware_set_state(QAPI_NET_LWM2M_FIRWARE_STATE_DOWNLOADED_E);
    lwm2m_firmware_set_result(QAPI_NET_LWM2M_FOTA_RESULT_INITIAL_E);
  }
  else
  {
    lwm2m_firmware_set_state(QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E);
    lwm2m_firmware_set_result(QAPI_NET_LWM2M_FOTA_RESULT_UNSUPPORTED_PROTOCOL_E);
  }
  return;
}

/**
 * @fn lwm2m_fota_update_cb()
 *
 * @brief This function is called when the FOTA application downloaded the image
 *        and applied the new FOTA image on the device.
 *
 * @param success      - Exceute successful or not.
 *
 * @return void
 * 
 * @Note OEM responsibilty to handle the applying the image and do associated 
 *       operations.
 */
void lwm2m_fota_update_cb(bool success)
{
  // Set the LwM2M FOTA state and result based on state machine.
  if(success == true)
  {
    lwm2m_firmware_set_state(QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E);
    lwm2m_firmware_set_result(QAPI_NET_LWM2M_FOTA_RESULT_UPDATE_SUCCESS_E);
  }
  else
  {
    lwm2m_firmware_set_state(QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E);
    lwm2m_firmware_set_result(QAPI_NET_LWM2M_FOTA_RESULT_UPDATE_FAILED_E);
  }
  gdownloaded = false;
  gupdate = false;
  return;
}

/**
 * @fn m2m_cert_read_from_EFS_file()
 *
 * @brief Certificate reading from efs partition.
 */
static int m2m_cert_read_from_EFS_file(
    const char * name,
    void ** buffer_ptr,
    size_t * buffer_size)
{
  uint32 bytes_read = 0;
  int efs_fd = QAPI_ERROR;
  struct fs_stat stat = {0};
  uint8_t *file_buf = NULL;

  if((!name) || (!buffer_ptr) || (!buffer_size)) 
  {
    return QAPI_ERROR;
  }

  memset(&stat, 0, sizeof(struct fs_stat ));
  if((efs_fd = efs_open(name, O_RDONLY)) < 0)
  {
    LOG_ERROR("FOTA_LOG: Opening EFS file %s failed", name);
    return QAPI_ERROR;
  }

  if(efs_stat(name, &stat) < 0)
  {
    LOG_ERROR("FOTA_LOG: Reading EFS file %s failed", name);
    return QAPI_ERROR;
  }

  file_buf = malloc((stat.st_size + 4)); /*Added 4 bytes extra for padding*/

  if(file_buf == NULL)
  {
    LOG_ERROR("FOTA_LOG: malloc failed");
    return QAPI_ERROR;
  }

  bytes_read = efs_read(efs_fd, file_buf, stat.st_size);
  if((bytes_read != stat.st_size) && (bytes_read == 0))
  {
    free(file_buf);
    LOG_ERROR("FOTA_LOG:Reading EFS file error");
    return QAPI_ERROR;
  }

  *buffer_ptr = file_buf;
  *buffer_size = stat.st_size;
  efs_close(efs_fd);
  return QAPI_OK;
}

/**
 * @fn coap_configure_ssl()
 *
 * @brief Configure the SSL related stuff if the COAP connection is using PSK mode.
 */
static int coap_configure_ssl(int mode, qapi_Net_SSL_Config_t *ssl_cfg, qapi_Coap_Sec_Info_t *cfg)
{
  qapi_Status_t ret = QAPI_OK;
  char ssl_cert[QAPI_NET_SSL_MAX_CERT_NAME_LEN] = {0};     //efs filename .PSK or .PEM
  char ssl_key[QAPI_NET_SSL_MAX_CERT_NAME_LEN]  = {0};    //efs filename key.pem 
  qapi_Net_SSL_Cert_Info_t cert_info;
 
  char psk_fname_raw[QAPI_NET_SSL_MAX_CERT_NAME_LEN] = {0};
  char cert_fname_bin[QAPI_NET_SSL_MAX_CERT_NAME_LEN];
  void *ssl_cert_buf  = NULL;
  size_t ssl_cert_buf_size = 0;
  
  if(cfg == NULL || ssl_cfg == NULL)
    return QAPI_ERROR;

  memset(cfg , 0, sizeof( qapi_Coap_Sec_Info_t));
  memset(ssl_cfg, 0, sizeof(qapi_Net_SSL_Config_t));
  memset(&cert_info, 0, sizeof(qapi_Net_SSL_Cert_Info_t));
  memset(ssl_cert, 0, QAPI_NET_SSL_MAX_CERT_NAME_LEN);
  memset(ssl_key, 0, QAPI_NET_SSL_MAX_CERT_NAME_LEN);

  ssl_cfg->verify.domain =  0;
  ssl_cfg->verify.match_Name[0] = '\0';
  ssl_cfg->verify.time_Validity = 1;
  ssl_cfg->verify.send_Alert = 1;
  ssl_cfg->max_Frag_Len = COAP_SERVERS_SSL_MAX_FRAG_LEN;

  if(mode == QAPI_COAP_MODE_PSK)
  {
    LOG_INFO("FOTA_LOG: sec_mode == QAPI_COAP_MODE_PSK ");

    memset(cert_fname_bin, 0, QAPI_NET_SSL_MAX_CERT_NAME_LEN);
    memset(psk_fname_raw, 0, QAPI_NET_SSL_MAX_CERT_NAME_LEN);

    ssl_cfg->protocol = QAPI_NET_SSL_PROTOCOL_DTLS_1_2;
    ssl_cfg->cipher[0] = QAPI_NET_TLS_PSK_WITH_AES_128_CCM_8;
    ssl_cfg->cipher[1] = QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA;
    ssl_cfg->cipher[2]= QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA;
    ssl_cfg->cipher[3] = QAPI_NET_TLS_PSK_WITH_AES_128_GCM_SHA256;
    ssl_cfg->cipher[4] = QAPI_NET_TLS_PSK_WITH_AES_256_GCM_SHA384;
    ssl_cfg->cipher[5] = QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA256;
    ssl_cfg->cipher[6] = QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA384;

    cfg->psk = malloc(QAPI_NET_SSL_MAX_CERT_NAME_LEN);

    if(cfg->psk == NULL)
      return QAPI_ERROR;

    strlcpy((char *)cfg->psk ,"coap_psk.bin" ,QAPI_NET_SSL_MAX_CERT_NAME_LEN);

    memset(cert_fname_bin, 0, QAPI_NET_SSL_MAX_CERT_NAME_LEN);

    strlcpy((char *)cert_fname_bin ,"coap_psk.bin" ,QAPI_NET_SSL_MAX_CERT_NAME_LEN);

    memset(psk_fname_raw, 0, QAPI_NET_SSL_MAX_CERT_NAME_LEN);

    strlcpy(psk_fname_raw , "/datatx/ssl/coap_server.psk" , QAPI_NET_SSL_MAX_CERT_NAME_LEN);

    /*read from default location and encrypt and store in secure location*/
    if(m2m_cert_read_from_EFS_file(psk_fname_raw, &ssl_cert_buf, &ssl_cert_buf_size ) != QAPI_OK)
    {
      if(cfg->psk != NULL) 
      {
        free((void *)cfg->psk);
        cfg->psk = NULL;
      }
      return QAPI_ERROR;
    }
    else
      ret = QAPI_OK;
    
    cert_info.cert_Type = QAPI_NET_SSL_PSK_TABLE_E;
    cert_info.info.psk_Tbl.psk_Size = ssl_cert_buf_size;
    cert_info.info.psk_Tbl.psk_Buf = ssl_cert_buf;

    ret = qapi_Net_SSL_Cert_Convert_And_Store(&cert_info, (uint8_t*)cert_fname_bin);

    if(ret != QAPI_OK)
    {
      LOG_INFO("FOTA_LOG: returned error for mode %d " , mode);
      goto Error;
    }
    else
    {
      LOG_INFO("FOTA_LOG: Client PSK stored successfully");
      return QAPI_OK;
    }
  } 
  else
  {
    ret = QAPI_ERROR;
  }
 if(ret == QAPI_OK) return QAPI_OK;

Error:
  if(cfg->psk != NULL) 
  {
    free((void *)cfg->psk);
    cfg->psk = NULL;
  }
  if(cert_info.info.psk_Tbl.psk_Buf != NULL)
  {
    free(cert_info.info.psk_Tbl.psk_Buf);
    cert_info.info.psk_Tbl.psk_Buf = NULL;
    cert_info.info.psk_Tbl.psk_Size = 0;
  }
   return ret;
}

/**
 * @fn fota_get_json_from_file()
 *
 * @brief This function is used to parse the intended file and store the file 
 *        content in the JSON format.
 *
 * @param file_name      - JSON file to be read.
 *
 * @return cJSON *
 */
static cJSON *fota_get_json_from_file(const char *file_name)
{
  cJSON *object     = NULL;
  int fd            = -1;
  int32 ret         = -1;
  int32 json_size   = 0;
  int32 read_bytes = 0;
  char *json_buf    = NULL;
  struct fs_stat  file_stat = {0};
  
  if(file_name == NULL || *file_name == 0)
    return NULL;

  fd = efs_open(file_name, (O_RDWR| O_CREAT));
  
  if(fd < 0)
  {
    LOG_ERROR("FOTA_LOG: Error opening %s file", file_name);
    return NULL;
  }
  
  memset(&file_stat, 0, sizeof(struct fs_stat));

  ret = efs_stat(file_name, &file_stat);

  if(ret < 0 )
  {
    LOG_ERROR("FOTA_LOG: Error stat file");
    return NULL;
  }
  
  json_size = file_stat.st_size;
  LOG_ERROR("FOTA_LOG:json size %d", json_size);
  json_buf = malloc(json_size + 1);
  
  if(json_buf == NULL) 
  {
    LOG_ERROR("FOTA_LOG: Alloc failed");
    efs_close(fd);
    return NULL;
  }
  
  read_bytes = efs_read(fd, (uint8_t *)json_buf, json_size);
  
  efs_close(fd);
  
  if(read_bytes < 0)
  {
    LOG_ERROR("FOTA_LOG:Error reading file");
    free(json_buf);
    return NULL;
  }

  json_buf[json_size] = '\0';
  
  object = cJSON_Parse(json_buf);
  
  free(json_buf);
  
  return object;
}

/**
 * @fn fota_config_load()
 *
 * @brief This function is used to read the FOTA json file and parse the JSON 
 *        content to the appropriate fields.
 *
 * @param void
 *
 * @return void
 */
void fota_config_load()
{ 
  cJSON *object                           = NULL;
  cJSON *memory_avail                     = NULL;
  cJSON *memory_avail_after_fota          = NULL;
  cJSON *integrity_check                  = NULL;
  cJSON *execute_status                   = NULL;
  cJSON *delivery_method                  = NULL;
  cJSON *protocol_support                  = NULL;
  cJSON *fota_blocksize                    = NULL;

  object = fota_get_json_from_file(FOTA_CONFIG_FILE_PATH);
  
  if(object == NULL)
  {
    LOG_INFO("FOTA_LOG: fota_config_load : object == NULL");
  }
  else
  {
    memory_avail            = cJSON_GetObjectItem(object, "memory_avail");
    memory_avail_after_fota       = cJSON_GetObjectItem(object, "memory_avail_after_fota");
    integrity_check     = cJSON_GetObjectItem(object, "integrity_check");
    execute_status   = cJSON_GetObjectItem(object, "execute_status");
    delivery_method  = cJSON_GetObjectItem(object, "delivery_method");
    protocol_support = cJSON_GetObjectItem(object, "protocol_support");
    fota_blocksize   = cJSON_GetObjectItem(object, "fota_blocksize");

    if(memory_avail != NULL) 
    {
      lwm2m_memory_avail = memory_avail->valueint;
    }
    if(memory_avail_after_fota != NULL) 
    {
      lwm2m_memory_avail_after_fota = memory_avail_after_fota->valueint;
    }
    if(integrity_check != NULL) 
    {
      lwm2m_integrity_check = integrity_check->valueint;
    }
    if(execute_status != NULL) 
    {
      lwm2m_execute_status = execute_status->valueint;
    }
    if(delivery_method != NULL)
    {
      lwm2m_delivery_method = delivery_method->valueint;
      if(delivery_method->valueint == 0)
        pull_allowed = true;
      else if(delivery_method->valueint == 1)
        push_allowed = true;
      else if(delivery_method->valueint == 2)
      {
        pull_allowed = true;
        push_allowed = true;
      }
    }
    if(protocol_support != NULL)
    {
      lwm2m_protocol_support = protocol_support->valueint;
    }
    if(fota_blocksize != NULL)
    {
      lwm2m_fota_blocksize = fota_blocksize->valueint;
    }
    cJSON_Delete(object);
  }
  
  LOG_INFO("FOTA_LOG: : Memory_avail : %d", lwm2m_memory_avail);
  LOG_INFO("FOTA_LOG: : Block size    : %d", lwm2m_fota_blocksize);
  LOG_INFO("FOTA_LOG: : Protocol support %d", lwm2m_protocol_support);
  LOG_INFO("FOTA_LOG: : Delivery method : %d", lwm2m_delivery_method);
  LOG_INFO("FOTA_LOG: : Integrity check  %d", lwm2m_integrity_check);
  LOG_INFO("FOTA_LOG: : Execute status  %d", lwm2m_execute_status);
}

/**
 * @fn parse_coap_uri()
 *
 * @brief This function parses the COAP URI and check whether the format is 
 *         coap://host:port/uri_path
 *         coaps://host:port/uri_path
 *         If the format is either of two then it is success, else error is thrown.
 *
 * @return 0 - success, 1 - failure
 */
uint8_t parse_coap_uri(char *uri, char **host, char **port, qapi_Sec_Mode *sec_mode)
{
  char *portP = NULL;
  char *hostP = NULL;
  char *s_uri = NULL;
  uint8_t result = 0x01;

  if ((uri == NULL) || (strlen(uri) < 7)) /* strlen("coap://") */
    goto parse_err;

  /* Parse URI information which is in the following formats 
   *     coap://[host]:port/uri_path
   *    coaps://[host]:port/uri_path
   */
  /* Host information */ 
  if (!strncmp(uri, "coap://", 7))
  {
    hostP = uri + 7;
    *sec_mode = QAPI_COAP_MODE_NONE;
  }
  else if (!strncmp(uri, "coaps://", 8))
  {
    hostP = uri + 8;
    *sec_mode = QAPI_COAP_MODE_PSK;
  }
  else 
    goto parse_err;

  /* Port information */
  portP = strrchr(hostP, ':');
  if (portP == NULL) 
    goto parse_err;

  /* Remove any optional brackets */
  if (hostP[0] == '[')
  {
    hostP++;

    if (*(portP - 1) == ']')
      *(portP - 1) = 0;
    else 
      goto parse_err;
  }

  /* Remove the ':' and split the host and port information */
 *portP = '\0';
  portP++;

  /* Uri Path information*/
  s_uri = strrchr(portP, '/');
  if (s_uri == NULL) 
    goto parse_err;

  /* Remove the '/' and split the port and uri path information */
 *s_uri = '\0';
  s_uri++;

  /* Allocate resources and copy the host and port information */
  *host = malloc(strlen(hostP) + 1);
  if (NULL == *host)
    goto parse_err; 

  /* Copy the host information */
  *port = malloc(strlen(portP) + 1);
  if (NULL == *port)
    goto parse_err; 


  strlcpy(*host, hostP, strlen(hostP) + 1);
  strlcpy(*port, portP, strlen(portP) + 1);
    /* Copy the uri path information */
   strlcpy(server_uri, s_uri, strlen(s_uri) + 1);
  result = 0x00; 
 
parse_err: 
  if(result)
  {
    if(*port)
     free(*port);

    if(*host)
     free(*host);

   *port = NULL;
   *host = NULL; 
  }

  return result;
}

/**
 * @fn fota_get_fw_name_version()
 *
 * @brief This function is to fetch the OS and Firmware version from modem.
 *
 * @return QAPI_*
 */
int fota_get_fw_name_version()
{
  qapi_Device_Info_t info;
  qapi_Status_t ret = QAPI_ERROR;
  qapi_Device_Info_Hndl_t hndl;

  memset(&info, 0x00, sizeof(qapi_Device_Info_t));
  ret = qapi_Device_Info_Init_v2(&hndl);
  if(ret == QAPI_OK)
  {
    ret = qapi_Device_Info_Get_v2(hndl, QAPI_DEVICE_INFO_OS_VERSION_E, &info);
    if(ret == QAPI_OK &&
       info.info_type == QAPI_DEVICE_INFO_TYPE_BUFFER_E
        && info.u.valuebuf.len > 0)
    {
      strlcpy(pkg_name, info.u.valuebuf.buf, info.u.valuebuf.len + 1);
    }
    ret = qapi_Device_Info_Get_v2(hndl, QAPI_DEVICE_INFO_FIRMWARE_VERSION_E, &info);
    if(ret == QAPI_OK &&
       info.info_type == QAPI_DEVICE_INFO_TYPE_BUFFER_E
        && info.u.valuebuf.len > 0)
    {
      strlcpy(pkg_version, info.u.valuebuf.buf, info.u.valuebuf.len + 1);
    }
    qapi_Device_Info_Release_v2(hndl);
  }
  return ret;
}

/**
 * @fn fota_get_package()
 *
 * @brief This function will create the COAP connection with the server, secure
 *        or non-secure based on COAP URI path. For secure connection currently
 *        PSK mode is supported.
 *
 * @param uri      - COAP URI path
 *
 * @return qapi_Status_t
 */
qapi_Status_t fota_get_package(char *uri)
{
  qapi_Status_t ret = QAPI_ERROR;
  qapi_Coap_Session_Info_t session_params;
  qapi_Coap_Connection_Cfg_t connection_params;
  char *host = NULL;
  char *port = NULL;
  qapi_Sec_Mode sec_mode =QAPI_COAP_MODE_NONE;
  qapi_Net_SSL_Config_t ssl_cfg = {0};
  qapi_Coap_Sec_Info_t cfg = {0};

  if(uri == NULL)
    return ret;

  memset(&session_params, 0x00, sizeof(qapi_Coap_Session_Info_t));
  memset(&connection_params, 0x00, sizeof(qapi_Coap_Connection_Cfg_t));
  session_params.coap_transaction_timeout = QAPI_COAP_RESPONSE_TIMEOUT; 
  session_params.coap_max_retransmits = QAPI_COAP_MAX_RETRANSMIT;
  session_params.coap_default_maxage = QAPI_COAP_DEFAULT_MAX_AGE;
  session_params.coap_ack_random_factor = QAPI_COAP_ACK_RANDOM_FACTOR;
  session_params.coap_max_latency = QAPI_COAP_MAX_LATENCY;
  session_params.clientData = NULL;

  if(parse_coap_uri(uri, &host, &port, &sec_mode))
  {
    LOG_ERROR("FOTA_LOG:Parsing the URI information failed");
    goto connect_err;
  }

  /* Create CoAP Session */
  if(QAPI_OK != qapi_Coap_Create_Session(&coap_handle,&session_params))
  {
    LOG_INFO("FOTA_LOG: qapi_Coap_Create_Session failed");
    goto connect_err;
  }

  /* Assign Config Parameters */
  connection_params.sec_Mode = sec_mode;
  connection_params.proto    = QAPI_COAP_PROTOCOL_UDP;
  connection_params.resumption_enabled = TRUE;
  connection_params.session_resumption_timeout = 0;
  connection_params.src_host = NULL;
  connection_params.src_port = 0;
  connection_params.dst_host = host;
  connection_params.dst_port = atoi(port);
  connection_params.family_type = AF_INET;
  connection_params.callHndl = NULL;
  if(connection_params.sec_Mode == QAPI_COAP_MODE_PSK)
  {
    if(coap_configure_ssl(connection_params.sec_Mode, &ssl_cfg, &cfg) != QAPI_OK)
    {
      goto connect_err;
    }
    else
    {
      if(&cfg && &ssl_cfg)
      {
        connection_params.sec_info = &cfg;
        connection_params.ssl_cfg = &ssl_cfg;
      }
      else
      {
        goto connect_err;
      }
    }
  }
  else
  {
    connection_params.sec_info = NULL;
    connection_params.ssl_cfg = NULL;
  }

  /* Connect to CoAP server */
  ret = qapi_Coap_Create_Connection(coap_handle,&connection_params);
  if(QAPI_OK != ret)
  {
    LOG_INFO("FOTA_LOG: Connect to CoAP server failure");
    goto connect_err;
  }

connect_err:
  {
    if(coap_handle != NULL && ret != QAPI_OK)
    {
     /* Deregister with CoAP client */
      qapi_Coap_Destroy_Session(coap_handle);
      coap_handle = NULL;
    }
    /* Release the associated resources */ 
    if (cfg.psk != NULL)
    {
      free((void*)cfg.psk);
      cfg.psk = NULL;
    }
   
    if (host)
     free(host);
 
    if (port)
     free(port);
  }
  return ret;
}

/**
 * @fn fota_handle_coap_pkt()
 *
 * @brief This function handles the COAP packet received. Handle the Out of band
 *        FOTA download with COAP server using blockwise transfer (Block 2).
 *
 * @return qapi_Status_t
 */
qapi_Status_t fota_handle_coap_pkt(qapi_Coap_Session_Hdl_t hdl ,qapi_Coap_Transaction_t *transaction,
                                            qapi_Coap_Packet_t *message)
{
  qapi_Status_t ret = QAPI_ERROR;
  if(hdl == NULL || transaction == NULL)
  {
    return ret;
  }

  timer_clr(&notify_Timer, T_SEC);
  if(message != NULL)
  {
    if(hdl != coap_handle || message->type == QAPI_COAP_TYPE_RST || message->code != QAPI_CONTENT_2_05
       || message->payload == NULL || message->payload_len == 0)
    {
      goto end;
    }
    else
    {
      /* Initial block */
      if(message->block2_num == 0)
      {
        /* Not enough memory for new package. TODO :- OEM can have their own logic.*/
        if(message->size > lwm2m_memory_avail)
        {
          goto end;
        }
        else
        {
          /* Clean up any old firmware image */
          efs_unlink(FOTA_APP_FILE_PATH);
          ret           = QAPI_OK;
        }
      }

      /* Handle the subsequent packets */
      if(message->block2_num + 1 == last_block)
      {
        ret = write_to_efs(message->payload, 
                           message->payload_len,
                           !message->block2_more);
        if(ret != QAPI_OK)
        {
          goto end;
        }
        else
        {
          LOG_INFO("FOTA_LOG: Block %d written in efs successfully.",message->block2_num);
          /* Fire a timer for the next packet  To handle the lost connection 
             scenario, will fire a timer and if the packet is not received in the
             duration, application will assume lost connection and will abort the
             FOTA transfer.*/ 
          timer_set(&notify_Timer, 64, FALSE, T_SEC);
        }
      }
      else if(message->block2_num == last_block)
      {
        ret           = QAPI_OK;
        /* Ignore the packet as it might be retrasnmission */
        /* Fire a timer for the next packet  To handle the lost connection 
           scenario, will fire a timer and if the packet is not received in the
           duration, application will assume lost connection and will abort the
           FOTA transfer.*/ 
        timer_set(&notify_Timer, 64, FALSE, T_SEC);
      }
      else
      {
        /* Blocks came out of order */
        /* Clean up firmware image present in EFS.*/
        efs_unlink(FOTA_APP_FILE_PATH);
        ret           = QAPI_ERROR;
        goto end;
      }

      /*Last block */
      if(message->block2_more == 0)
      {
        ret           = QAPI_OK;
        gdownloaded = true;
        last_block = -1;
        /* TODO :- OEM can have their own logic for handling the integrity of
                    downloaded fota image.*/
        if(lwm2m_integrity_check)
        {
          lwm2m_out_band_fota_cb(true);
        }
        else
        {
          lwm2m_out_band_fota_cb(false);
        
        }
        timer_clr(&notify_Timer, T_SEC);
      }
      else
      {
        /* Get the next blockwise packet */
        ret = fota_send_coap_pkt(message->block2_size, message->block2_num + 1);
      }
    }
  }
  else
  {
    // Empty packet stop everything
    goto end;
  }
end:
  if(ret != QAPI_OK)
  {
    efs_unlink(FOTA_APP_FILE_PATH);
    lwm2m_out_band_fota_cb(false);
  }
  return ret;
}

/**
 * @fn coap_handle_transaction_cb()
 *
 * @brief This function will switch the context from CoAP RX thread to FOTA 
 *        application thread.
 *
 * @return void
 */
void coap_handle_transaction_cb(qapi_Coap_Session_Hdl_t hdl ,qapi_Coap_Transaction_t * transaction,
                                 qapi_Coap_Packet_t * message)
{
  fota_obj_cmd_t *cmd_ptr = NULL;
  fota_app_data_t *data = NULL;

  data = (fota_app_data_t *)malloc(sizeof(fota_app_data_t));
  if(data == NULL)
  {
    return;
  }
  memset(data, 0x00, sizeof(fota_app_data_t));

  data->app_data.coap_op.hdl = hdl;
  if(coap_copy_client_transaction(hdl, transaction, &data->app_data.coap_op.transaction) != QAPI_OK)
  {
    goto error;
  }
  if(coap_copy_pkt(hdl,message, &data->app_data.coap_op.message) != QAPI_OK)
  {
    goto error;
  }

  LOG_INFO("FOTA_LOG: Switching to application callback context");

  if((cmd_ptr = fota_obj_get_cmd_buffer()) == NULL)
  {
    LOG_ERROR("FOTA_LOG:Cannot obtain command buffer");
    goto error;
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id               = FOTA_OBJ_APP_ORIGINATED_EVT;
    cmd_ptr->cmd_data.data                = (void *)data;

    LOG_INFO("FOTA_LOG: Enqueue the command into the command queue.");
    fota_obj_enqueue_cmd(cmd_ptr);
  }

  fota_obj_set_signal();

  if(transaction->message)
  {
    qapi_Coap_Free_Message(hdl,transaction->message);
    transaction->message = NULL;
  }

  if(message)
    qapi_Coap_Free_Message(hdl, message);
  return;
error:
  if(transaction->message)
  {
    qapi_Coap_Free_Message(hdl,transaction->message);
    transaction->message = NULL;
  }
  if(message)
    qapi_Coap_Free_Message(hdl, message);
  if(data->app_data.coap_op.transaction)
  {
    free_coap_client_transaction(coap_handle, &data->app_data.coap_op.transaction);
  }
  if(data->app_data.coap_op.message)
  {
    qapi_Coap_Free_Message(coap_handle, data->app_data.coap_op.message);
  }
  free_memory((void **)&data);
  return;
}

/**
 * @fn fota_send_coap_pkt()
 *
 * @brief This function is used to send out the next COAP packet request for blockwise
 *        transfer.
 *
 * @param blocksize      - Block 2 packet size.
 * @param pkt_num        - Block 2 packet number.
 *
 * @return qapi_Status_t
 */
qapi_Status_t fota_send_coap_pkt(int blocksize, int pkt_num)
{
  qapi_Coap_Message_Type_t   msg_type       = QAPI_COAP_TYPE_CON;
  qapi_Coap_Message_Params_t message_param  = {0};
  qapi_Coap_Method_t         method_type    = QAPI_COAP_GET;
  qapi_Coap_Packet_t         *messageP      = {0};
  qapi_Coap_Header_type      header_type    = QAPI_COAP_URI_PATH;
  qapi_Status_t              ret            = QAPI_ERROR;
  char                       *uri_path;
  int                        uri_length;
  qapi_Coap_Block_Params_t params;

  int length = strlen(server_uri);

  /* Setting uri_path and uri_length */
  uri_path = (char*)malloc((length+1)*sizeof(char));

  if (uri_path == NULL)
  {
    LOG_ERROR("FOTA_LOG: Memory allocation failed to uri_path.");
    return ret;
  }

  memset(uri_path,0x00,length + 1);
  strlcpy(uri_path, server_uri, length+1);
  uri_length = strlen(uri_path)+1;
  uri_path[uri_length]='\0';

  method_type = QAPI_COAP_GET;
  ret = qapi_Coap_Init_Message(coap_handle, &messageP, msg_type, method_type);
  if(QAPI_OK != ret)
  {
    LOG_INFO("FOTA_LOG: qapi_Coap_Init_Message failed");
    goto end;
  }

  header_type = QAPI_COAP_URI_PATH;
  ret = qapi_Coap_Set_Header(coap_handle, messageP , header_type , (void * )uri_path, uri_length);

  if(QAPI_OK != ret)
  {
    LOG_INFO("FOTA_LOG: qapi_Coap_Set_Header failure");
    goto end;
  }

  /* Set block2 header parameters */
  header_type = QAPI_COAP_BLOCK2;
  params.num = pkt_num;
  params.more = 0;
  params.size = blocksize; /* Set blocksize you want in downlink */

  ret = qapi_Coap_Set_Header(coap_handle, messageP , header_type , &params,sizeof(qapi_Coap_Block_Params_t));
  if(QAPI_OK != ret)
  {
    LOG_INFO("FOTA_LOG: qapi_Coap_Set_Header failure");
    goto end;
  }
  message_param.msg_cb = coap_handle_transaction_cb;

  ret= qapi_Coap_Send_Message(coap_handle ,messageP, &message_param);
  if(QAPI_OK != ret)
  {
    LOG_INFO("FOTA_LOG: qapi_Coap_Send_Message failure");
    goto end;
  }
  else
  {
    last_block = pkt_num + 1;
    LOG_INFO("FOTA_LOG: Packet %d sent successfully ",pkt_num);
    return QAPI_OK;
  }

end:
  qapi_Coap_Free_Message(coap_handle,messageP);
  return ret;
}

/**
 * @fn lwm2m_firmware_set_state()
 *
 * @brief This function is to set the LwM2M firmware state (/5/0/3) resource.
 *
 * @param state      - LwM2M Firmware state
 *
 * @return void
 */
void lwm2m_firmware_set_state(qapi_Net_Firmware_State_t state)
{
  qapi_Net_LWM2M_Data_v2_t *lwm2m_data = NULL;
  qapi_Status_t result = QAPI_ERROR;

  if(state > QAPI_NET_LWM2M_FIRWARE_STATE_UPDATING_E)
  {
    LOG_ERROR("FOTA_LOG: Invalid FOTA state.");
    return;
  }

  lwm2m_data = (qapi_Net_LWM2M_Data_v2_t *)malloc(sizeof(qapi_Net_LWM2M_Data_v2_t));
  if(lwm2m_data == NULL)
  {
    LOG_INFO("FOTA_LOG:Memory allocation failure.");
    return;
  }
  memset(lwm2m_data, 0x00, sizeof(qapi_Net_LWM2M_Data_v2_t));
  
  lwm2m_data->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  lwm2m_data->no_instances++;
  
  lwm2m_data->instance_info = (qapi_Net_LWM2M_Instance_Info_v2_t *)malloc(sizeof(qapi_Net_LWM2M_Instance_Info_v2_t));
  if(lwm2m_data->instance_info == NULL)
  {
    LOG_INFO("FOTA_LOG: Memory allocation failure.");
    free_lwm2m_copy_data_(&lwm2m_data);
    return;
  }
  memset(lwm2m_data->instance_info, 0x00, sizeof(qapi_Net_LWM2M_Instance_Info_v2_t));
  
  lwm2m_data->instance_info->instance_ID = FOTA_INSTANCE_ID_0;
  lwm2m_data->instance_info->no_resources++;
  
  lwm2m_data->instance_info->resource_info = malloc(sizeof(qapi_Net_LWM2M_Resource_Info_t));
  if(lwm2m_data->instance_info->resource_info == NULL)
  {
    LOG_INFO("FOTA_LOG: Memory allocation failure.");
    free_lwm2m_copy_data_(&lwm2m_data);
    return;
  }
  memset(lwm2m_data->instance_info->resource_info, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data->instance_info->resource_info->resource_ID     = QAPI_NET_LWM2M_FOTA_RES_M_STATE_E;
  lwm2m_data->instance_info->resource_info->type            = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  lwm2m_data->instance_info->resource_info->value.asInteger = state;

  result = qapi_Net_LWM2M_Set_v2(gHandler_fota,lwm2m_data);
  if(result != QAPI_OK)
  {
    LOG_ERROR("FOTA_LOG:Setting firmware state failed with result %d",result);
  }

  free_lwm2m_copy_data_(&lwm2m_data);
}

/**
 * @fn lwm2m_firmware_set_result()
 *
 * @brief This function is to set the LwM2M firmware result (/5/0/5) resource.
 *
 * @param result      - LwM2M Firmware result
 *
 * @return void
 */
void lwm2m_firmware_set_result(qapi_Net_LWM2M_Fota_Result_t result)
{
  qapi_Net_LWM2M_Data_v2_t *lwm2m_data = NULL;
  qapi_Status_t res = QAPI_ERROR;

  if(result > QAPI_NET_LWM2M_FOTA_RESULT_UNSUPPORTED_PROTOCOL_E)
  {
    LOG_ERROR("FOTA_LOG: Invalid FOTA update result.");
    return;
  }

  lwm2m_data = (qapi_Net_LWM2M_Data_v2_t *)malloc(sizeof(qapi_Net_LWM2M_Data_v2_t));
  if(lwm2m_data == NULL)
  {
    LOG_INFO("FOTA_LOG: Memory allocation failure.");
    return;
  }
  memset(lwm2m_data, 0x00, sizeof(qapi_Net_LWM2M_Data_v2_t));
  
  lwm2m_data->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  lwm2m_data->no_instances++;
  
  lwm2m_data->instance_info = (qapi_Net_LWM2M_Instance_Info_v2_t *)malloc(sizeof(qapi_Net_LWM2M_Instance_Info_v2_t));
  if(lwm2m_data->instance_info == NULL)
  {
    LOG_INFO("FOTA_LOG: Memory allocation failure.");
    free_lwm2m_copy_data_(&lwm2m_data);
    return;
  }
  memset(lwm2m_data->instance_info, 0x00, sizeof(qapi_Net_LWM2M_Instance_Info_v2_t));
  
  lwm2m_data->instance_info->instance_ID = FOTA_INSTANCE_ID_0;
  lwm2m_data->instance_info->no_resources++;
  
  lwm2m_data->instance_info->resource_info = malloc(sizeof(qapi_Net_LWM2M_Resource_Info_t));
  if(lwm2m_data->instance_info->resource_info == NULL)
  {
    LOG_INFO("FOTA_LOG: Memory allocation failure.");
    free_lwm2m_copy_data_(&lwm2m_data);
    return;
  }
  memset(lwm2m_data->instance_info->resource_info, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data->instance_info->resource_info->resource_ID     = QAPI_NET_LWM2M_FOTA_RES_M_UPDATE_RESULT_E;
  lwm2m_data->instance_info->resource_info->type            = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  lwm2m_data->instance_info->resource_info->value.asInteger = result;

  res = qapi_Net_LWM2M_Set_v2(gHandler_fota,lwm2m_data);
  if(res != QAPI_OK)
  {
    LOG_ERROR("FOTA_LOG: Setting firmware update result failed with result %d",res);
  }

  free_lwm2m_copy_data_(&lwm2m_data);
}

/**
 * @fn fota_app_callback()
 *
 * @brief This function will switch the context from LWM2M RX thread to FOTA 
 *        application thread.
 *
 * @return void
 */
void fota_app_callback(qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Data_v2_t * lwm2m_data, qapi_Status_t *result)
{
  fota_obj_cmd_t *cmd_ptr = NULL;
  fota_app_data_t *data = NULL;

  data = (fota_app_data_t *)malloc(sizeof(fota_app_data_t));
  if(data == NULL)
  {
    *result = QAPI_ERR_NO_MEMORY;
    return;
  }
  memset(data, 0x00, sizeof(fota_app_data_t));

  data->app_data.dl_op.handler = handler;
  if(lwm2m_copy_data_(lwm2m_data, &data->app_data.dl_op.lwm2m_data) != QAPI_OK)
  {
    *result = QAPI_ERROR;
    goto error;
  }

  LOG_INFO("FOTA_LOG: Switching to application callback context");

  if((cmd_ptr = fota_obj_get_cmd_buffer()) == NULL)
  {
    LOG_ERROR("FOTA_LOG: Cannot obtain command buffer");
    *result = QAPI_ERR_NO_ENTRY;
    free_lwm2m_copy_data_(&data->app_data.dl_op.lwm2m_data);
    goto error;
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id               = FOTA_OBJ_CALL_BACK_EVT;
    cmd_ptr->cmd_data.data                = (void *)data;

    LOG_INFO("FOTA_LOG: Enqueue the command into the command queue.");
    fota_obj_enqueue_cmd(cmd_ptr);
  }

  fota_obj_set_signal();
  *result = QAPI_OK;
  return;
error:
  free_memory((void **)&data);
  return;
}

/**
 * @fn fota_object_callback()
 *
 * @brief This function will be needed to handle the different LwM2M events.
 *        Not implemented currently.
 *
 * @return qapi_Status_t
 */
qapi_Status_t fota_object_callback(qapi_Net_LWM2M_App_Handler_t handler, 
                                            qapi_Net_LWM2M_Server_Data_t *lwm2m_srv_data,
                                            void                         *user_data)
{
  return QAPI_OK;
}

/**
 * @fn fota_handle_data()
 *
 * @brief This fucntion is called when observe operation is performed 
 *        on firmware object resource (/5/0/0, /5/0/1, /5/0/2)
 * @param handler LWM2M application handler
 * @param lwm2m_data  Firmware object data
 * @return on success - QAPI_OK
 on failure - QAPI_ERROR
 */
qapi_Status_t fota_handle_data(qapi_Net_LWM2M_App_Handler_t handler,
                                      qapi_Net_LWM2M_Data_v2_t lwm2m_data)
                                      
{
  int32_t rc = 0;
  qapi_Status_t res = QAPI_ERROR;
  qapi_Net_Firmware_State_t state = QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E;
  qapi_Net_LWM2M_Fota_Result_t update_result = QAPI_NET_LWM2M_FOTA_RESULT_INITIAL_E;

  if(handler == NULL || handler != gHandler_fota ||
     lwm2m_data.instance_info == NULL ||
     lwm2m_data.instance_info->resource_info == NULL)
  {
    LOG_INFO ("FOTA_LOG: Passed arguments are NULL");
    return QAPI_ERROR;
  }

  LOG_INFO("FOTA_LOG: App callback is invoked DATA is --> ");
  LOG_INFO("FOTA_LOG: object_ID ----> %d",lwm2m_data.object_ID);
  LOG_INFO("FOTA_LOG: instance id ----> %d",
      lwm2m_data.instance_info->instance_ID);
  LOG_INFO("FOTA_LOG: resouces id -----> %d",
      lwm2m_data.instance_info->resource_info->resource_ID);

  switch(lwm2m_data.instance_info->resource_info->type)
  {
    case QAPI_NET_LWM2M_TYPE_STRING_E :
    case QAPI_NET_LWM2M_TYPE_OPAQUE_E :
      LOG_INFO("FOTA_LOG: resource value is length %d",
                lwm2m_data.instance_info->resource_info->value.asBuffer.length);
      if(lwm2m_data.instance_info->resource_info->value.asBuffer.length)
        LOG_INFO("FOTA_LOG: resource value is %s", 
                  lwm2m_data.instance_info->resource_info->value.asBuffer.buffer );
    break;
    case QAPI_NET_LWM2M_TYPE_INTEGER_E :
      LOG_INFO("FOTA_LOG: resource value is %ld",
                lwm2m_data.instance_info->resource_info->value.asInteger);
    break;
    case QAPI_NET_LWM2M_TYPE_FLOAT_E :
      LOG_INFO("FOTA_LOG: resource value is %ld",
               lwm2m_data.instance_info->resource_info->value.asFloat);
    break;
    case QAPI_NET_LWM2M_TYPE_BOOLEAN_E :
      LOG_INFO("FOTA_LOG:resource value is %d",
               lwm2m_data.instance_info->resource_info->value.asBoolean);
    break;
  }

  //check if the resource is Package that the application has done a observation on
  if(lwm2m_data.instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_FOTA_RES_M_PACKAGE_E)
  {
    timer_clr(&notify_Timer, T_SEC);
    if(lwm2m_data.instance_info->resource_info->value.asBuffer.length == 0)
    {
      LOG_INFO("FOTA_LOG: Null package recieved.");
      state         = QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E;
      update_result = QAPI_NET_LWM2M_FOTA_RESULT_INITIAL_E;
      res           = QAPI_OK;
      gdownloaded = false;
      gupdate     = false;
      goto end;
    }
    else
    {
      if(push_allowed != true)
      {
        LOG_INFO("FOTA_LOG: Push not allowed.");
        state         = QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E;
        update_result = QAPI_NET_LWM2M_FOTA_RESULT_UNSUPPORTED_PACKAGE_TYPE_E;
        res           = QAPI_OK;
        goto end;
      }

      if(lwm2m_data.instance_info->resource_info->value.asBuffer.length)
      {
        qapi_Net_LWM2M_Resource_Info_t *resource = lwm2m_data.instance_info->resource_info;

        /* Initial block */
        if(resource->value.asBuffer.block1_num == 0)
        {
          last_block = -1;
          /* Not enough memory for new package. TODO :- OEM can have their own logic.*/
          if(resource->value.asBuffer.size1 > lwm2m_memory_avail) 
          {
            state         = QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E;
            update_result = QAPI_NET_LWM2M_FOTA_RESULT_NOT_ENOUGH_STORAGE_E;
            res           = QAPI_ERROR;
            goto end;
          }
          else
          {
            /* Clean up any old firmware image */
            //efs_close(fota_file_fd);
            efs_unlink(FOTA_APP_FILE_PATH);
            state         = QAPI_NET_LWM2M_FIRWARE_STATE_DOWNLOADING_E;
            update_result = QAPI_NET_LWM2M_FOTA_RESULT_INITIAL_E;
            res           = QAPI_OK;
          }
        }

        if(resource->value.asBuffer.block1_num == last_block + 1)
        {
          res = write_to_efs(resource->value.asBuffer.buffer, 
                             resource->value.asBuffer.length,
                             !resource->value.asBuffer.block1_more);
          if(res != QAPI_OK)
          {
            state         = QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E;
            update_result = QAPI_NET_LWM2M_FOTA_RESULT_OUT_OF_MEMORY_E;
            res           = QAPI_ERROR;
            goto end;
          }
          else
          {
            last_block = resource->value.asBuffer.block1_num;
            LOG_INFO("FOTA_LOG: Block %d written in efs successfully.",resource->value.asBuffer.block1_num);

            /* Fire a timer for the next packet  To handle the lost connection 
               scenario, will fire a timer and if the packet is not received in the
               duration, application will assume lost connection and will abort the
               FOTA transfer.*/ 
            timer_set(&notify_Timer, 64, FALSE, T_SEC);
          }
        }
        else if(resource->value.asBuffer.block1_num == last_block)
        {
          /* Ignore the packet as it might be retrasnmission */
          /* Fire a timer for the next packet  To handle the lost connection 
             scenario, will fire a timer and if the packet is not received in the
             duration, application will assume lost connection and will abort the
             FOTA transfer.*/ 
          timer_set(&notify_Timer, 64, FALSE, T_SEC);
        }
        else
        {
          /* Blocks came out of order */
          /* Clean up firmware image present in EFS.*/
          //efs_close(fota_file_fd);
          efs_unlink(FOTA_APP_FILE_PATH);
          state         = QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E;
          update_result = QAPI_NET_LWM2M_FOTA_RESULT_CRC_CHECK_FAIL_E;
          res           = QAPI_ERROR;
          goto end;
        }

        /*Last block */
        if(resource->value.asBuffer.block1_more == 0)
        {
          state         = QAPI_NET_LWM2M_FIRWARE_STATE_DOWNLOADED_E;
          update_result = QAPI_NET_LWM2M_FOTA_RESULT_INITIAL_E;
          res           = QAPI_OK;
          gdownloaded = true;
          last_block = -1;
          timer_clr(&notify_Timer, T_SEC);
          /* TODO :- OEM can have their own logic for handling the integrity of
                    downloaded fota image.*/
          if(lwm2m_integrity_check != 1)
          {
            state         = QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E;
            update_result = QAPI_NET_LWM2M_FOTA_RESULT_CRC_CHECK_FAIL_E;
            res           = QAPI_ERROR;
            goto end;
          }
          //efs_close(fota_file_fd);
        }
      }
    }
  }
  //check if the resource is Package URI that the application has done a observation on
  else if (lwm2m_data.instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_FOTA_RES_M_PACKAGE_URI_E)
  {
    if(lwm2m_data.instance_info->resource_info->value.asBuffer.length == 0)
    {
      LOG_INFO("FOTA_LOG: Null package URI recieved.");
      state         = QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E;
      update_result = QAPI_NET_LWM2M_FOTA_RESULT_INITIAL_E;
      res           = QAPI_OK;
      gdownloaded = false;
      gupdate     = false;
      goto end;
    }
    else
    {
      if(pull_allowed != true)
      {
        LOG_INFO("FOTA_LOG: Pull not allowed.");
        state         = QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E;
        update_result = QAPI_NET_LWM2M_FOTA_RESULT_UNSUPPORTED_PACKAGE_TYPE_E;
        res           = QAPI_ERROR;
        goto end;
      }

      //if(gdownloaded == false)
      {
        if(lwm2m_data.instance_info->resource_info->value.asBuffer.length)
        {
          char pkg_uri[255] = {0};
          qapi_Sec_Mode sec_mode;
          char *host = NULL;
          char *port = NULL;

          strlcpy(pkg_uri, (const char *)lwm2m_data.instance_info->resource_info->value.asBuffer.buffer,
                  lwm2m_data.instance_info->resource_info->value.asBuffer.length + 1);

          //Check the uri validity
          rc = parse_coap_uri(pkg_uri, &host, &port, &sec_mode);
          if(rc != 0)
          {
            LOG_INFO("FOTA_LOG: URI is not in the valid format.");
            state         = QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E;
            update_result = QAPI_NET_LWM2M_FOTA_RESULT_INVAILD_URI_E;
            res           = QAPI_ERROR;
            goto end;
          }

          state         = QAPI_NET_LWM2M_FIRWARE_STATE_DOWNLOADING_E;
          update_result = QAPI_NET_LWM2M_FOTA_RESULT_INITIAL_E;
          res           = QAPI_OK;
          gdownloaded = true;

          strlcpy(pkg_uri, (const char *)lwm2m_data.instance_info->resource_info->value.asBuffer.buffer,
                  lwm2m_data.instance_info->resource_info->value.asBuffer.length + 1);

          rc = fota_get_package(pkg_uri);

          if(rc == QAPI_OK)
          {
            rc = fota_send_coap_pkt(lwm2m_fota_blocksize, 0);
          }
          if(rc != QAPI_OK)
          {
            state         = QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E;
            update_result = QAPI_NET_LWM2M_FOTA_RESULT_UNSUPPORTED_PROTOCOL_E;
            res           = QAPI_ERROR;
            gdownloaded = false;
            goto end;
          }
        }
      }
    }
  } 
  //check if the resource is Update execute that the application has done a observation on  
  else if(lwm2m_data.instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_FOTA_RES_M_UPDATE_E)
  {
    if((gupdate == false) && (gdownloaded == true))
    {
      gupdate = true;
      LOG_INFO("FOTA_LOG: Execute operation received.");
    }
    else
    {
      LOG_ERROR("FOTA_LOG: Not in the proper state.");
    }

    state         = QAPI_NET_LWM2M_FIRWARE_STATE_UPDATING_E;
    update_result = QAPI_NET_LWM2M_FOTA_RESULT_INITIAL_E;
    res           = QAPI_OK;

    /* Todo Need to apply the new image and reset the device.*/
    timer_sleep(2,T_SEC,TRUE);

    lwm2m_fota_update_cb(lwm2m_execute_status);
    return res;
  }
  else
  {
    res = QAPI_ERROR;
    return res;
  }
end:
  lwm2m_firmware_set_state(state);
  lwm2m_firmware_set_result(update_result);
  return res;
}

/**
 * @fn fota_observe()
 *
 * @brief This function is to set the observe on /5/0/0, /5/0/1 and /5/0/2 resource
 *        so that any activity on the above resources will trigger a callback of
 *        FOTA application and it will be handled according to state machine.
 */
qapi_Status_t fota_observe(qapi_Net_LWM2M_Object_Info_v2_t *observe_info)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Id_Info_v2_t *id_info1 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t *id_info2 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t *id_info3 = NULL;

  if(observe_info == NULL)
    return QAPI_ERR_INVALID_PARAM;

  memset(observe_info, 0x00, sizeof(qapi_Net_LWM2M_Object_Info_v2_t));
  observe_info->no_object_info = 3;

  id_info1 = (qapi_Net_LWM2M_Id_Info_v2_t *)malloc(sizeof(qapi_Net_LWM2M_Id_Info_v2_t));
  if(id_info1 == NULL)
  {
   return QAPI_ERR_NO_MEMORY;
  }
  memset(id_info1, 0x00, sizeof(qapi_Net_LWM2M_Id_Info_v2_t));

  observe_info->id_info = id_info1;
  id_info1->id_set |= (QAPI_NET_LWM2M_OBJECT_ID_E | QAPI_NET_LWM2M_INSTANCE_ID_E
                      | QAPI_NET_LWM2M_RESOURCE_ID_E);
  id_info1->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  id_info1->instance_ID = 0;
  id_info1->resource_ID = QAPI_NET_LWM2M_FOTA_RES_M_PACKAGE_E;
  id_info1->next =NULL;

  id_info2 = (qapi_Net_LWM2M_Id_Info_v2_t *)malloc(sizeof(qapi_Net_LWM2M_Id_Info_v2_t));
  if(id_info2 == NULL)
  {
   return QAPI_ERR_NO_MEMORY;
  }
  memset(id_info2, 0x00, sizeof(qapi_Net_LWM2M_Id_Info_v2_t));
  id_info1->next = id_info2;
  id_info2->id_set |= (QAPI_NET_LWM2M_OBJECT_ID_E | QAPI_NET_LWM2M_INSTANCE_ID_E
                      | QAPI_NET_LWM2M_RESOURCE_ID_E);
  id_info2->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  id_info2->instance_ID = 0;
  id_info2->resource_ID = QAPI_NET_LWM2M_FOTA_RES_M_PACKAGE_URI_E;
  id_info2->next =NULL;

  id_info3 = (qapi_Net_LWM2M_Id_Info_v2_t *)malloc(sizeof(qapi_Net_LWM2M_Id_Info_v2_t));
  if(id_info3 == NULL)
  {
   return QAPI_ERR_NO_MEMORY;
  }
  memset(id_info3, 0x00, sizeof(qapi_Net_LWM2M_Id_Info_v2_t));
  id_info2->next = id_info3;
  id_info3->id_set |= (QAPI_NET_LWM2M_OBJECT_ID_E | QAPI_NET_LWM2M_INSTANCE_ID_E
                      | QAPI_NET_LWM2M_RESOURCE_ID_E);
  id_info3->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  id_info3->instance_ID = 0;
  id_info3->resource_ID = QAPI_NET_LWM2M_FOTA_RES_M_UPDATE_E;
  id_info3->next =NULL;

  result = qapi_Net_LWM2M_Observe_v2(gHandler_fota, fota_app_callback, observe_info);

  return result;
}

/**
 * @fn fota_init()
 *
 * @brief This function loads the fota config values and update the Firmware object
 *        resource values.
 */
static qapi_Status_t fota_init()
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Data_v2_t lwm2m_data = {0};
  qapi_Net_LWM2M_Data_v2_t *lwm2m_dataP = NULL;
  qapi_Net_LWM2M_Object_Info_v2_t lwm2m_info_req = {0};
  qapi_Net_LWM2M_Object_Info_v2_t observe_info = {0};
  qapi_Net_LWM2M_Data_v2_t *lwm2m_set_data = NULL;

  // Retrieve the FOTA related config values.
  fota_config_load();

  // Retrieve the OS and FW version.
  fota_get_fw_name_version();

  memset(&lwm2m_data, 0x00,sizeof (qapi_Net_LWM2M_Data_v2_t));
  
  lwm2m_data.object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  lwm2m_data.no_instances = 1;
  lwm2m_data.instance_info = calloc(sizeof(qapi_Net_LWM2M_Instance_Info_v2_t), 1);

  if(lwm2m_data.instance_info == NULL)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  result = qapi_Net_LWM2M_Create_Object_Instance_v2(gHandler_fota,&lwm2m_data);

  if(result == QAPI_OK || result == QAPI_ERR_EXISTS)
  {
    LOG_INFO("FOTA_LOG: Firmware object instance creation successful ");
  }
  else
    LOG_ERROR("FOTA_LOG: Error in Firmware object instance creation %d",result);

  /* Set the resource values */
  lwm2m_set_data = (qapi_Net_LWM2M_Data_v2_t *)malloc(sizeof(qapi_Net_LWM2M_Data_v2_t));
  if(lwm2m_set_data == NULL)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  memset(lwm2m_set_data, 0x00, sizeof(qapi_Net_LWM2M_Data_v2_t));
  lwm2m_set_data->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  lwm2m_set_data->no_instances++;
  lwm2m_set_data->instance_info = (qapi_Net_LWM2M_Instance_Info_v2_t *)malloc(sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
  if(NULL == lwm2m_set_data->instance_info)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  memset(lwm2m_set_data->instance_info, 0, sizeof(qapi_Net_LWM2M_Instance_Info_v2_t));
  lwm2m_set_data->instance_info->instance_ID = FOTA_INSTANCE_ID_0;
  lwm2m_set_data->instance_info->no_resources++;
  lwm2m_set_data->instance_info->resource_info = (qapi_Net_LWM2M_Resource_Info_t *)malloc(sizeof(qapi_Net_LWM2M_Resource_Info_t));
  if(NULL == lwm2m_set_data->instance_info->resource_info)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  memset(lwm2m_set_data->instance_info->resource_info, 0, sizeof(qapi_Net_LWM2M_Resource_Info_t));
  lwm2m_set_data->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_FOTA_RES_O_UPDATE_PROTOCOL_SUPPORT_E;
  lwm2m_set_data->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  lwm2m_set_data->instance_info->resource_info->value.asInteger = 3; 

  result = qapi_Net_LWM2M_Set_v2(gHandler_fota, lwm2m_set_data);
  if(result == QAPI_OK)
  {
    LOG_INFO("FOTA_LOG: Set successful for protocol support.");
    if(lwm2m_set_data)
    {
      free_lwm2m_copy_data_(&lwm2m_set_data);
    }
  }
  else
  {
    LOG_ERROR("FOTA_LOG: Set failed for protocol support with error %d",result);
  }

  /* Set the resource values */
  lwm2m_set_data = (qapi_Net_LWM2M_Data_v2_t *)malloc(sizeof(qapi_Net_LWM2M_Data_v2_t));
  if(lwm2m_set_data == NULL)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  memset(lwm2m_set_data, 0x00, sizeof(qapi_Net_LWM2M_Data_v2_t));
  lwm2m_set_data->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  lwm2m_set_data->no_instances++;
  lwm2m_set_data->instance_info = (qapi_Net_LWM2M_Instance_Info_v2_t *)malloc(sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
  if(NULL == lwm2m_set_data->instance_info)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  memset(lwm2m_set_data->instance_info, 0, sizeof(qapi_Net_LWM2M_Instance_Info_v2_t));
  lwm2m_set_data->instance_info->instance_ID = FOTA_INSTANCE_ID_0;
  lwm2m_set_data->instance_info->no_resources++;
  lwm2m_set_data->instance_info->resource_info = (qapi_Net_LWM2M_Resource_Info_t *)malloc(sizeof(qapi_Net_LWM2M_Resource_Info_t));
  if(NULL == lwm2m_set_data->instance_info->resource_info)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  memset(lwm2m_set_data->instance_info->resource_info, 0, sizeof(qapi_Net_LWM2M_Resource_Info_t));
  lwm2m_set_data->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_FOTA_RES_M_UPDATE_DELIVERY_METHOD_E;
  lwm2m_set_data->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  lwm2m_set_data->instance_info->resource_info->value.asInteger = lwm2m_delivery_method; 

  result = qapi_Net_LWM2M_Set_v2(gHandler_fota, lwm2m_set_data);
  if(result == QAPI_OK)
  {
    LOG_INFO("FOTA_LOG: Set successful for delivery method.");
    if(lwm2m_set_data)
    {
      free_lwm2m_copy_data_(&lwm2m_set_data);
    }
  }
  else
  {
    LOG_ERROR("FOTA_LOG: Set failed for delivery method with error %d",result);
  }

  /* Set the resource values */
  lwm2m_set_data = (qapi_Net_LWM2M_Data_v2_t *)malloc(sizeof(qapi_Net_LWM2M_Data_v2_t));
  if(lwm2m_set_data == NULL)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  memset(lwm2m_set_data, 0x00, sizeof(qapi_Net_LWM2M_Data_v2_t));
  lwm2m_set_data->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  lwm2m_set_data->no_instances++;
  lwm2m_set_data->instance_info = (qapi_Net_LWM2M_Instance_Info_v2_t *)malloc(sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
  if(NULL == lwm2m_set_data->instance_info)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  memset(lwm2m_set_data->instance_info, 0, sizeof(qapi_Net_LWM2M_Instance_Info_v2_t));
  lwm2m_set_data->instance_info->instance_ID = FOTA_INSTANCE_ID_0;
  lwm2m_set_data->instance_info->no_resources++;
  lwm2m_set_data->instance_info->resource_info = (qapi_Net_LWM2M_Resource_Info_t *)malloc(sizeof(qapi_Net_LWM2M_Resource_Info_t));
  if(NULL == lwm2m_set_data->instance_info->resource_info)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  memset(lwm2m_set_data->instance_info->resource_info, 0, sizeof(qapi_Net_LWM2M_Resource_Info_t));
  lwm2m_set_data->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_FOTA_RES_O_PACKAGE_NAME_E;
  lwm2m_set_data->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_STRING_E;
  lwm2m_set_data->instance_info->resource_info->value.asBuffer.length = strlen(pkg_name);
  lwm2m_set_data->instance_info->resource_info->value.asBuffer.buffer = malloc 
   (lwm2m_set_data->instance_info->resource_info->value.asBuffer.length + 1);
  if(lwm2m_set_data->instance_info->resource_info->value.asBuffer.buffer == NULL)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  memset(lwm2m_set_data->instance_info->resource_info->value.asBuffer.buffer, 0x00,
         lwm2m_set_data->instance_info->resource_info->value.asBuffer.length + 1);
  strlcpy((char *)lwm2m_set_data->instance_info->resource_info->value.asBuffer.buffer, 
          pkg_name, lwm2m_set_data->instance_info->resource_info->value.asBuffer.length + 1);

  result = qapi_Net_LWM2M_Set_v2(gHandler_fota, lwm2m_set_data);
  if(result == QAPI_OK)
  {
    LOG_INFO("FOTA_LOG: Set successful for package name.");
    if(lwm2m_set_data)
    {
      free_lwm2m_copy_data_(&lwm2m_set_data);
    }
  }
  else
  {
    LOG_ERROR("FOTA_LOG: Set failed for package name with error %d",result);
  }

  /* Set the resource values */
  lwm2m_set_data = (qapi_Net_LWM2M_Data_v2_t *)malloc(sizeof(qapi_Net_LWM2M_Data_v2_t));
  if(lwm2m_set_data == NULL)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  memset(lwm2m_set_data, 0x00, sizeof(qapi_Net_LWM2M_Data_v2_t));
  lwm2m_set_data->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  lwm2m_set_data->no_instances++;
  lwm2m_set_data->instance_info = (qapi_Net_LWM2M_Instance_Info_v2_t *)malloc(sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
  if(NULL == lwm2m_set_data->instance_info)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  memset(lwm2m_set_data->instance_info, 0, sizeof(qapi_Net_LWM2M_Instance_Info_v2_t));
  lwm2m_set_data->instance_info->instance_ID = FOTA_INSTANCE_ID_0;
  lwm2m_set_data->instance_info->no_resources++;
  lwm2m_set_data->instance_info->resource_info = (qapi_Net_LWM2M_Resource_Info_t *)malloc(sizeof(qapi_Net_LWM2M_Resource_Info_t));
  if(NULL == lwm2m_set_data->instance_info->resource_info)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  memset(lwm2m_set_data->instance_info->resource_info, 0, sizeof(qapi_Net_LWM2M_Resource_Info_t));
  lwm2m_set_data->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_FOTA_RES_O_PACKAGE_VERSION_E;
  lwm2m_set_data->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_STRING_E;
  lwm2m_set_data->instance_info->resource_info->value.asBuffer.length = strlen(pkg_version);
  lwm2m_set_data->instance_info->resource_info->value.asBuffer.buffer = malloc 
   (lwm2m_set_data->instance_info->resource_info->value.asBuffer.length + 1);
  if(lwm2m_set_data->instance_info->resource_info->value.asBuffer.buffer == NULL)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  memset(lwm2m_set_data->instance_info->resource_info->value.asBuffer.buffer, 0x00,
         lwm2m_set_data->instance_info->resource_info->value.asBuffer.length + 1);
  strlcpy((char *)lwm2m_set_data->instance_info->resource_info->value.asBuffer.buffer, 
          pkg_version, lwm2m_set_data->instance_info->resource_info->value.asBuffer.length + 1);

  result = qapi_Net_LWM2M_Set_v2(gHandler_fota, lwm2m_set_data);
  if(result == QAPI_OK)
  {
    LOG_INFO("FOTA_LOG: Set successful for package version.");
    if(lwm2m_set_data)
    {
      free_lwm2m_copy_data_(&lwm2m_set_data);
    }
  }
  else
  {
    LOG_ERROR("FOTA_LOG: Set failed for package version with error %d",result);
  }

  /* Get the FOTA state status */
  memset(&lwm2m_info_req , 0 , sizeof (qapi_Net_LWM2M_Object_Info_v2_t));
  lwm2m_info_req.no_object_info++;
  lwm2m_info_req.id_info = (qapi_Net_LWM2M_Id_Info_v2_t *)malloc(sizeof(qapi_Net_LWM2M_Id_Info_v2_t));

  if(lwm2m_info_req.id_info == NULL)
  {
    result = QAPI_ERR_NO_MEMORY;
    goto error;
  }
  memset(lwm2m_info_req.id_info, 0x00, sizeof(qapi_Net_LWM2M_Id_Info_v2_t));

  lwm2m_info_req.id_info->id_set |= (QAPI_NET_LWM2M_OBJECT_ID_E | QAPI_NET_LWM2M_INSTANCE_ID_E
                                    | QAPI_NET_LWM2M_RESOURCE_ID_E);
  lwm2m_info_req.id_info->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  lwm2m_info_req.id_info->instance_ID = 0;
  lwm2m_info_req.id_info->resource_ID = QAPI_NET_LWM2M_FOTA_RES_M_STATE_E;
  lwm2m_info_req.id_info->next =NULL;

  result = qapi_Net_LWM2M_Get_v2(gHandler_fota, &lwm2m_info_req, &lwm2m_dataP);

  if(result == QAPI_OK)
  {
    if(lwm2m_dataP != NULL &&  lwm2m_dataP->instance_info != NULL && 
       lwm2m_dataP->instance_info->resource_info != NULL && 
       lwm2m_dataP->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_FOTA_RES_M_STATE_E)
    {
      int resource_state_value = lwm2m_dataP->instance_info->resource_info->value.asInteger;
      LOG_INFO("FOTA_LOG: FOTA_STATE->%d", resource_state_value);
      switch(resource_state_value)
      {
        case QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E:
          gdownloaded = false;
          gupdate     = false;
        break;
        case QAPI_NET_LWM2M_FIRWARE_STATE_DOWNLOADING_E:
        case QAPI_NET_LWM2M_FIRWARE_STATE_DOWNLOADED_E:
          gdownloaded = true;
          gupdate     = false;
        break;
        case QAPI_NET_LWM2M_FIRWARE_STATE_UPDATING_E:
          gdownloaded = true;
          gupdate    = true;
        break;
        default:
        LOG_ERROR("FOTA_LOG:Unexpected FOTA_STATE");
        break;
      }
    }
     LOG_INFO("FOTA_LOG:FOTA_STATE->gdownloaded[%d],gupdate[%d]", gdownloaded, gupdate);
  }
  else 
  {
    LOG_ERROR("FOTA_LOG: Fetching FOTA_STATE failed.");
  }

  result = fota_observe(&observe_info);
  if(result == QAPI_OK)
  {
    LOG_INFO("FOTA_LOG: Observation set successfully.");
  }
  else
  {
    LOG_ERROR("FOTA_LOG: Error %d in setting observation",result);
    goto error;
  }

  /* Define a timer to track connection */
  timer_def_osal(&notify_Timer, &FotaTimerGroup, TIMER_FUNC1_CB_TYPE, notify_signal_cb,NULL);

error:

  if(lwm2m_data.instance_info)
  {
    free(lwm2m_data.instance_info);
    lwm2m_data.instance_info = NULL;
  }
  if(lwm2m_info_req.id_info)
  {
    free(lwm2m_info_req.id_info);
    lwm2m_info_req.id_info = NULL;
  }
  if(lwm2m_dataP)
  {
    free(lwm2m_dataP);
    lwm2m_dataP = NULL;
  }
  if(observe_info.id_info)
  {
    app_free_id_info_(observe_info.id_info);
    observe_info.id_info = NULL;
  }
  if(lwm2m_set_data)
  {
    free_lwm2m_copy_data_(&lwm2m_set_data);
  }
  return result;
}


/**
 * @fn Initialize_Fota_App()
 *
 * @brief This function is used to initialise FOTA application.
 *
 * @param void
 * @return qapi_Status_t
 */
qapi_Status_t Initialize_Fota_App()
{
  qapi_Status_t result = QAPI_ERROR;
  uint8_t retry_cnt = 0;

  // Sleep for 20 secs so that LwM2M client can be initialized.
  timer_sleep(20, T_SEC, 1);
  do
  {
    result = qapi_Net_LWM2M_Register_App_Extended(&gHandler_fota, NULL,
                                                  fota_object_callback);
  
    if(result == QAPI_OK)
    {
      LOG_INFO("FOTA_LOG: Firmware object application registered successfully ");
      break;
    }
    else if(result == QAPI_ERR_NOT_INITIALIZED)
    {
      if(retry_cnt < 10)
      {
        timer_sleep(5, T_SEC, 1);    // Retry after 5 secs
        retry_cnt++;
      }
      else
      {
        LOG_ERROR("FOTA_LOG: Max attempt exhausted for registering. Exiting...");
        return result;
      }
    }
    else
    {
      LOG_ERROR("FOTA_LOG: Error %d in registration. Exiting...",result);
      return result;
    }
  }while(1);

  fota_init();
  return result;
}

