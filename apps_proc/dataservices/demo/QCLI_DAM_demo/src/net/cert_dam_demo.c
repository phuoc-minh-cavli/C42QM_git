/*!
   @file
   cert_dam_demo.c
 
   @brief
   Security protocol-certificate related functionality.
*/
 
/*===========================================================================
 
   Copyright (c) 2017 Qualcomm Technologies, Inc. All Rights Reserved
 
   Qualcomm Technologies Proprietary
 
   Export of this technology or software is regulated by the U.S. Government.
   Diversion contrary to U.S. law prohibited.
 
   All ideas, data and information contained in or disclosed by
   this document are confidential and proprietary information of
   Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
   By accepting this material the recipient agrees that this material
   and the information contained therein are held in confidence and in
   trust and will not be used, copied, reproduced in whole or in part,
   nor its contents revealed in any manner to others without the express
   written permission of Qualcomm Technologies, Inc.
 
===========================================================================*/

/* Copyright (c) 2011-2015.  Qualcomm Atheros, Inc.
All rights reserved.

Qualcomm Atheros Confidential and Proprietary.

*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "bench.h"
#include "qapi_status.h"
#include "qapi_netservices.h"
#include "qapi_dnsc.h"
#include "qapi_ns_gen_v6.h"
#include "qapi_types.h"
#include "txm_module.h"
#include "qapi_fs.h"
#include "fs_public.h"


/* 
 * This file contains the command handlers for certificate management operations
 * on non-volatile memory like store, delete, list, etc.  
 * 
 */
#ifdef INCLUDE_SSLDEMO
#define  CERT_DEMO_DNS_MAX_HOSTNAME_LEN      128
extern TX_BYTE_POOL * byte_pool_qcli;
extern QCLI_Group_Handle_t qcli_net_handle; /* Handle for Net Command Group. */

extern QCLI_Command_t net_cmd_list[];

extern uint8_t v6_enable_flag[NO_OF_APPS];

extern int ssl_store_read_from_EFS_file(
    const char * name,
    void ** buffer_ptr,
    size_t * buffer_size
    );

const char * file_type_certificate = "cert";
const char * file_type_di_certificate = "di_cert";
const char * file_type_ca_list = "ca_list";
const char * file_type_psk_table = "psk_table";
const char * file_type_rpk_certificate = "rpk_cert";

uint8_t *cert_data_buf[NO_OF_APPS];
uint16_t cert_data_buf_len[NO_OF_APPS];

extern bool dam_thrd_running[NO_OF_APPS];

#define CERT_PRINTF(...) QCLI_Printf(qcli_net_handle, __VA_ARGS__)


static int cert_read_from_EFS_file(
    const char * name,
    void ** buffer_ptr,
    size_t * buffer_size
    )
{
    int bytes_read =0;
    int efs_fd = -1;
    struct qapi_FS_Stat_Type_s stat;
	uint8 *file_buf = NULL;
    stat.st_size = 0;

    if ( (!name) ||(!buffer_ptr) ||(!buffer_size) )
    {
        CERT_PRINTF("Reading SSL from EFS file failed!\n");
        return QAPI_ERROR;
    }

    if (qapi_FS_Open(name, QAPI_FS_O_RDONLY_E, &efs_fd) < 0)
    {
        CERT_PRINTF("Opening EFS EFS file %s failed\n", name);
        return QAPI_ERROR;
    }

    if (qapi_FS_Stat_With_Handle(efs_fd, &stat) < 0) {
        CERT_PRINTF("Reading EFS file %s failed\n", name);
        return QAPI_ERROR;
    }

    CERT_PRINTF("Reading EFS file size %d \n", stat.st_size);
    
    tx_byte_allocate(byte_pool_qcli, (VOID **) &file_buf, stat.st_size, TX_NO_WAIT);
    if (file_buf  == NULL) {
        CERT_PRINTF("SSL_cert_store: QAPI_ERR_NO_MEMORY \n");
        return QAPI_ERR_NO_MEMORY;
    }

    qapi_FS_Read(efs_fd, file_buf, stat.st_size, &bytes_read);
    if ((bytes_read < 0) || (bytes_read != stat.st_size)) {
        tx_byte_release(file_buf);
        CERT_PRINTF("SSL_cert_store: Reading EFS file error\n");
        return QAPI_ERROR;
    }

    *buffer_ptr = file_buf;
    *buffer_size = stat.st_size;

    qapi_FS_Close(efs_fd);

    return QAPI_OK;
}

QCLI_Command_Status_t cert_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = CERT_COMMAND_HANDLER_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  cmd_parser_temp->Command_Index = 11;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t 
proc_cert_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    int i;
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

    if(Parameter_Count < 1)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    if (0 == strcmp((char*)Parameter_List[0].String_Value, "store")) {
        result = store_cert(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp((char*)Parameter_List[0].String_Value, "delete")) {
        result = delete_cert(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp((char*)Parameter_List[0].String_Value, "list")) {
        result = list_cert(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp((char*)Parameter_List[0].String_Value, "get")) {
        result = get_cert(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp((char*)Parameter_List[0].String_Value, "convert")) {
        result = covert_store_cert(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

end:
  if (Parameter_List != NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        tx_byte_release(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

QCLI_Command_Status_t covert_store_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    int32_t cntr = 0;
    int32_t result = QCLI_STATUS_ERROR_E;
    uint8_t *out_file = NULL; 
    uint8_t *cert_Buf = NULL; 
    uint32_t cert_Size = 0;
    uint8_t *key_Buf = NULL;
    uint32_t key_Size = 0;
    qapi_NET_SSL_CA_Info_t calist_info[QAPI_NET_SSL_MAX_CA_LIST];
    uint32_t ca_list_cnt = 0; 
    uint8_t *psk_Buf = NULL; 
    uint32_t psk_Size = 0;
    uint8_t *dicert_Buf = NULL; 
    uint32_t dicert_Size = 0;
    qapi_Net_SSL_Cert_Info_t cert_info;
    int i;

    if (Parameter_Count < 2 || Parameter_List == NULL)
    {
        result = QCLI_STATUS_ERROR_E;
        goto usage;
    }

    memset(&cert_info, 0x00, sizeof(qapi_Net_SSL_Cert_Info_t));
    
    /* Convert and store the certificate */
    if (0 == strcmp("cert", (char *)Parameter_List[0].String_Value))
    {
        int32_t argc = 0;
        uint8_t *cert_name = NULL;
        uint8_t *key_name = NULL; 
        uint8_t *pass_Key = NULL; 

        cert_info.cert_Type = QAPI_NET_SSL_CERTIFICATE_E;     

        for (argc = 0; argc + 1 < Parameter_Count; ++argc)
        {
            if (0 == strcmp("cert", (const char *)Parameter_List[argc].String_Value))
                cert_name = (uint8_t *)Parameter_List[++argc].String_Value;
            else if (0 == strcmp("key", (const char *)Parameter_List[argc].String_Value))
                key_name = (uint8_t *)Parameter_List[++argc].String_Value;
            else if (0 == strcmp("pass", (const char *)Parameter_List[argc].String_Value))
                pass_Key = (uint8_t *)Parameter_List[++argc].String_Value;
            else if (0 == strcmp("out", (const char *)Parameter_List[argc].String_Value))
                out_file = (uint8_t *)Parameter_List[++argc].String_Value;
        }
           
        if (!cert_name || !key_name || !out_file)
            goto usage;
        
        /* Read the client certificate information */
        result = cert_read_from_EFS_file((char *)cert_name, (void **)&cert_Buf, (size_t *)&cert_Size);
        if (QAPI_OK != result) 
        {
             CERT_PRINTF("ERROR: Reading client certificate from EFS failed!! \r\n");
             result = QCLI_STATUS_ERROR_E;

             goto cert_err;
        }

        /* Read the key information */
        result = cert_read_from_EFS_file((char *)key_name, (void **)&key_Buf, (size_t *)&key_Size);
        if (QAPI_OK != result) 
        {
             CERT_PRINTF("ERROR: Reading key information from EFS failed!! \r\n");
             result = QCLI_STATUS_ERROR_E;

             goto cert_err;
        }
        
        /* Update the client certificate information */
        cert_info.info.cert.cert_Buf = cert_Buf;
        cert_info.info.cert.cert_Size = cert_Size;
        cert_info.info.cert.key_Buf = key_Buf;
        cert_info.info.cert.key_Size = key_Size;
        cert_info.info.cert.pass_Key = pass_Key; 
    }
    /* Convert and store the RPK certificate */
    else if (0 == strcmp("rpk_cert", (char *)Parameter_List[0].String_Value))
    {
        int32_t argc = 0;
        uint8_t *cert_name = NULL;
        uint8_t *key_name = NULL; 
        uint8_t *pass_Key = NULL; 

        cert_info.cert_Type = QAPI_NET_SSL_RPK_CERT_E;     

        for (argc = 0; argc + 1 < Parameter_Count; ++argc)
        {
            if (0 == strcmp("rpk_cert", (const char *)Parameter_List[argc].String_Value))
                cert_name = (uint8_t *)Parameter_List[++argc].String_Value;
            else if (0 == strcmp("key", (const char *)Parameter_List[argc].String_Value))
                key_name = (uint8_t *)Parameter_List[++argc].String_Value;
            else if (0 == strcmp("pass", (const char *)Parameter_List[argc].String_Value))
                pass_Key = (uint8_t *)Parameter_List[++argc].String_Value;
            else if (0 == strcmp("out", (const char *)Parameter_List[argc].String_Value))
                out_file = (uint8_t *)Parameter_List[++argc].String_Value;
        }
           
        if (!cert_name || !key_name || !out_file)
            goto usage;
        
        /* Read the client certificate information */
        result = cert_read_from_EFS_file((char *)cert_name, (void **)&cert_Buf, (size_t *)&cert_Size);
        if (QAPI_OK != result) 
        {
             CERT_PRINTF("ERROR: Reading client certificate from EFS failed!! \r\n");
             result = QCLI_STATUS_ERROR_E;

             goto cert_err;
        }

        /* Read the key information */
        result = cert_read_from_EFS_file((char *)key_name, (void **)&key_Buf, (size_t *)&key_Size);
        if (QAPI_OK != result) 
        {
             CERT_PRINTF("ERROR: Reading key information from EFS failed!! \r\n");
             result = QCLI_STATUS_ERROR_E;

             goto cert_err;
        }
        
        /* Update the client certificate information */
        cert_info.info.rpk_cert.pubkey_Buf = cert_Buf;
        cert_info.info.rpk_cert.pubkey_Size = cert_Size;
        cert_info.info.rpk_cert.privkey_Buf = key_Buf;
        cert_info.info.rpk_cert.privkey_Size = key_Size;
        cert_info.info.rpk_cert.pass_Key = pass_Key; 
    }
    /* Convert and store the certificate authority list */
    else if (0 == strcmp("ca_list", (char *)Parameter_List[0].String_Value))
    {
        int32_t argc = 0; 

        memset(&calist_info, 0x00, sizeof(calist_info));

        cert_info.cert_Type = QAPI_NET_SSL_CA_LIST_E;
               
        for (argc = 0; argc + 1 < Parameter_Count; ++argc)
        {
            if (0 == strcmp("ca_list", (const char *)Parameter_List[argc].String_Value))
            {
                for (argc += 1 ; argc < Parameter_Count; ++argc)
                {
                    if (0 == strcmp("out", (const char *)Parameter_List[argc].String_Value))
                    {
                        argc--;
                        break;                      
                    }
                    
                    /* Read the CA list information */
                    result = cert_read_from_EFS_file((char *)Parameter_List[argc].String_Value, (void **)&calist_info[ca_list_cnt].ca_Buf, (size_t *)&calist_info[ca_list_cnt].ca_Size);
                    if (QAPI_OK != result) 
                    {
                         CERT_PRINTF("ERROR: Reading CA list information from EFS failed!! \r\n");
                         result = QCLI_STATUS_ERROR_E;

                         goto cert_err;
                    }

                    cert_info.info.ca_List.ca_Info[ca_list_cnt] = &calist_info[ca_list_cnt];
                    cert_info.info.ca_List.ca_Cnt = ++ca_list_cnt;
                }

            }
            else if (0 == strcmp("out", (const char *)Parameter_List[argc].String_Value))
                out_file = (uint8_t *)Parameter_List[++argc].String_Value;
        }
           
        if (!cert_info.info.ca_List.ca_Cnt || !out_file)
            goto usage; 
 
    }
    /* Convert and store the PSK table */
    else if (0 == strcmp("psk_table", (char *)Parameter_List[0].String_Value))
    {
        int32_t argc = 0;        
        uint8_t *psk_Tbl = NULL;

        cert_info.cert_Type = QAPI_NET_SSL_PSK_TABLE_E; 

        for (argc = 0; argc + 1 < Parameter_Count; ++argc)
        {
            if (0 == strcmp("psk_table", (const char *)Parameter_List[argc].String_Value))
                psk_Tbl = (uint8_t *)Parameter_List[++argc].String_Value;
            else if (0 == strcmp("out", (const char *)Parameter_List[argc].String_Value))
                out_file = (uint8_t *)Parameter_List[++argc].String_Value;
        }
           
        if (!psk_Tbl || !out_file)
            goto usage;   

        /* Read the client certificate information */
        result = cert_read_from_EFS_file((char *)psk_Tbl, (void **)&psk_Buf, (size_t *)&psk_Size);
        if (QAPI_OK != result) 
        {
            CERT_PRINTF("ERROR: Reading PSK table information from EFS failed!! \r\n");
            result = QCLI_STATUS_ERROR_E;

            goto cert_err;
        }

        /* Update the PSK table information */
        cert_info.info.psk_Tbl.psk_Buf = psk_Buf;
        cert_info.info.psk_Tbl.psk_Size = psk_Size;
    }
    /* Convert and store the PSK table */
    else if (0 == strcmp("di_cert", (char *)Parameter_List[0].String_Value))
    {
      int32_t argc = 0;        
      uint8_t *di_cert = NULL;

      cert_info.cert_Type = QAPI_NET_SSL_DI_CERT_E; 

      for (argc = 0; argc + 1 < Parameter_Count; ++argc)
      {
          if (0 == strcmp("di_cert", (const char *)Parameter_List[argc].String_Value))
              di_cert = (uint8_t *)Parameter_List[++argc].String_Value;
          else if (0 == strcmp("out", (const char *)Parameter_List[argc].String_Value))
              out_file = (uint8_t *)Parameter_List[++argc].String_Value;
      }

      if (!di_cert || !out_file)
          goto usage;   

      /* Read the client certificate information */
      result = cert_read_from_EFS_file((char *)di_cert, (void **)&dicert_Buf, (size_t *)&dicert_Size);
      if (QAPI_OK != result) 
      {
          CERT_PRINTF("ERROR: Reading PSK table information from EFS failed!! \r\n");
          result = QCLI_STATUS_ERROR_E;

          goto cert_err;
      }

      /* Update the PSK table information */
      cert_info.info.di_cert.di_Cert_Buf   = dicert_Buf;
      cert_info.info.di_cert.di_Cert_Size  = dicert_Size;

    }
    else
    {
        goto usage;
    }

    /* Convert and store the certificate */ 
    if (qapi_Net_SSL_Cert_Convert_And_Store(&cert_info, out_file) != QAPI_OK)
    {
        CERT_PRINTF("ERROR: Unable to convert and store the certificate from FLASH!\r\n");
        result = QCLI_STATUS_ERROR_E;

        goto cert_err;
    }

    CERT_PRINTF("Certificate converted and stored in FLASH successfully!\r\n");
    result = QCLI_STATUS_SUCCESS_E;  

cert_err:
    /* Free the allocated resources */
    if (cert_Buf)
        tx_byte_release(cert_Buf);

    if (key_Buf)
        tx_byte_release(key_Buf);

    if (psk_Buf)
        tx_byte_release(psk_Buf);

    if (dicert_Buf)
        tx_byte_release(dicert_Buf);

    for (cntr = 0; cntr < ca_list_cnt; cntr++)
    {
        if (calist_info[cntr].ca_Buf)
            tx_byte_release(calist_info[cntr].ca_Buf);
    }

    if (Parameter_List != NULL)
    {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        tx_byte_release(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
    }
    return (QCLI_Command_Status_t)result;

usage:
    CERT_PRINTF("\nUsage: cert convert <cert|rpk_cert|ca_list|psk_table>\r\n"
                " Convert certificate, CA Lists from  .PEM, .DER, .P7B and PSK files to binary format and stores securely\r\n"
                "                <cert> [cert name] <key> <key name> <pass> [pass key (optional)] <out> [out file]\r\n"
                "                <rpk_cert> [cert name] <key> <key name> <pass> [pass key (optional)] <out> [out file]\r\n"
                "                <ca_list> [cert file1] [cert file2 ...] <out> [out file]\r\n"
                "                <psk_table> [psk file] <out> [out file]\r\n"
               );

    if (Parameter_List != NULL)
    {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        tx_byte_release(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
    }
    return (QCLI_Command_Status_t)result;
}

QCLI_Command_Status_t store_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    char * name;
    char * option_file_type;
    char * file_type;
    char * option_srcEFS_type;
    char * EFS_loc;
    char file_path[FS_PATH_MAX];
    int  file_name_len;
    int  file_path_len;
    int  result;
    int i;
    QCLI_Command_Status_t status =  QCLI_STATUS_SUCCESS_E;

    if ( Parameter_Count < 3 ) {
        CERT_PRINTF("ERROR: Invalid number of parameters\r\n");
        goto store_cert_on_error; 
    }

    name = (char*)Parameter_List[0].String_Value;
    option_file_type = (char*)Parameter_List[1].String_Value;
    file_type = (char*)Parameter_List[2].String_Value;

    if ((!option_file_type) || (!file_type) || (!( strcmp("-t", option_file_type) == 0 )) ||
        (!((strcmp(file_type_certificate, file_type) == 0) || (strcmp(file_type_ca_list, file_type) == 0) || (strcmp(file_type_rpk_certificate, file_type) == 0)
           || (strcmp(file_type_psk_table, file_type) == 0)|| (strcmp(file_type_di_certificate, file_type) == 0) )))
    {
        CERT_PRINTF("ERROR: Invalid filetype.  Must be either cert or ca_list\r\n");
        goto store_cert_on_error;
    }

    if ( Parameter_Count > 3 ) {
        
        option_srcEFS_type = (char*)Parameter_List[3].String_Value;
        EFS_loc = (char*)Parameter_List[4].String_Value;
        if ((!option_srcEFS_type) || (!EFS_loc ) || (!( strcmp("-d", option_srcEFS_type) == 0 )))
        {
            CERT_PRINTF("ERROR: Invalid filetype.  Must be either cert or ca_list\r\n");
            goto store_cert_on_error;
        }

        file_name_len = strlen(name);
        file_path_len = strlen(EFS_loc);
        if ((file_name_len >= QAPI_NET_SSL_MAX_CERT_NAME_LEN) || (file_name_len + file_path_len) >= FS_PATH_MAX) {
            CERT_PRINTF("ERROR: Invalid file length\r\n");
            status = QCLI_STATUS_ERROR_E;
            goto end;
        }
        strlcpy(file_path, EFS_loc, file_path_len + 1);
        strlcpy(&file_path[file_path_len], name, file_name_len + 1);
        CERT_PRINTF("EFS Source location:%s\n",file_path);
        
        result = cert_read_from_EFS_file(file_path, (void **)&cert_data_buf[app_id], (size_t *)&cert_data_buf_len[app_id]);

        if ( QAPI_OK != result ) {
            CERT_PRINTF("ERROR: Reading source EFS file failed!! \r\n");
            status = QCLI_STATUS_ERROR_E;
            goto end;
         }

    }
    
    if (cert_data_buf_len[app_id] == 0)
    {
        CERT_PRINTF("ERROR: no certificate data.\nHint: Use the \"cert get\" command to read a certificate from a certificate server.\n");
        goto store_cert_on_error;
    }

    if ( strcmp(file_type_certificate, file_type) == 0 ) {
        result = qapi_Net_SSL_Cert_Store(name, QAPI_NET_SSL_CERTIFICATE_E, cert_data_buf[app_id], cert_data_buf_len[app_id]);
    }
    else if ( strcmp(file_type_rpk_certificate, file_type) == 0 ) {
        result = qapi_Net_SSL_Cert_Store(name, QAPI_NET_SSL_RPK_CERT_E, cert_data_buf[app_id], cert_data_buf_len[app_id]);
    }
    else if ( strcmp(file_type_di_certificate, file_type) == 0 ) {
        result = qapi_Net_SSL_Cert_Store(name, QAPI_NET_SSL_DI_CERT_E, cert_data_buf[app_id], cert_data_buf_len[app_id]);
    }
    else if ( strcmp(file_type_ca_list, file_type) == 0 ) {
        result = qapi_Net_SSL_Cert_Store(name, QAPI_NET_SSL_CA_LIST_E, cert_data_buf[app_id], cert_data_buf_len[app_id]);
    }
    else if ( strcmp(file_type_psk_table, file_type) == 0 ) {
        result = qapi_Net_SSL_Cert_Store(name, QAPI_NET_SSL_PSK_TABLE_E, cert_data_buf[app_id], cert_data_buf_len[app_id]);
    }
    else
    {
      goto store_cert_on_error;
    }

    if (QAPI_OK == result)
    {
        CERT_PRINTF("%s is stored in NV memory\n", name);
    }
    else
    {
        CERT_PRINTF("ERROR: failed to store %s\n", name);
        goto store_cert_on_error;
    }

    if(cert_data_buf[app_id]){
      tx_byte_release(cert_data_buf[app_id]);
      cert_data_buf[app_id] = NULL;
    }

    status = QCLI_STATUS_SUCCESS_E;
    goto end;

store_cert_on_error:
    CERT_PRINTF("\nUsage: cert store <name> -t <filetype> [-d <efs source location>]\n");
    CERT_PRINTF("This encrypt and stores a certificate or CA list or PSK table in EFS memory\n");
    CERT_PRINTF("<name>  = Name of the certificate or CA list or PSK file file to store\r\n");
    CERT_PRINTF("<filetype>  = File type could be \"cert\" or \"rpk_cert\" or \"ca_list\" or \"psk_table\"\r\n");
    CERT_PRINTF("<efs source location>  = EFS location for SSL certificated need to be encrypted(/datatx/ssl by default)\r\n");

    if(cert_data_buf[app_id]){
      tx_byte_release(cert_data_buf[app_id]);
      cert_data_buf[app_id] = NULL;
    }

    status = QCLI_STATUS_ERROR_E;
    goto end;

end:
  if (Parameter_List != NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        tx_byte_release(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return status;
}

QCLI_Command_Status_t delete_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    char *name;
    char * option_file_type;
    char * file_type;
    int status;
    int i;
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

    if(Parameter_Count < 1)
    {
        if (cert_data_buf[app_id]) {
            cert_data_buf_len[app_id] = 0;
            tx_byte_release(cert_data_buf[app_id]);
            cert_data_buf[app_id] = NULL;
            CERT_PRINTF("Deleted the certificate data stored in RAM.\n");
            result = QCLI_STATUS_SUCCESS_E;
            goto end;
        }
        else {
            goto delete_cert_on_error;
        }
    }

    if (Parameter_Count < 3) {
        goto delete_cert_on_error;
    }

    name = (char*)Parameter_List[0].String_Value;
    option_file_type = (char*)Parameter_List[1].String_Value;
    file_type = (char*)Parameter_List[2].String_Value;

    if ( 0 ||
        !option_file_type ||
        !file_type ||
        !( strcmp("-t", option_file_type) == 0 ) ||
        !( (strcmp(file_type_certificate, file_type) == 0) || 
           (strcmp(file_type_rpk_certificate, file_type) == 0)  || 
           (strcmp(file_type_di_certificate, file_type) == 0)  || 
           (strcmp(file_type_ca_list, file_type) == 0) ||
           (strcmp(file_type_psk_table, file_type) == 0) )
    )
    {
        CERT_PRINTF("ERROR: Invalid filetype.  Must be either cert or ca_list\r\n");
        goto delete_cert_on_error;
    }

    if ( strcmp(file_type_certificate, file_type) == 0 ) {
        status = qapi_Net_SSL_Cert_delete(name, QAPI_NET_SSL_CERTIFICATE_E);
    }
    else if ( strcmp(file_type_rpk_certificate, file_type) == 0 ) {
        status = qapi_Net_SSL_Cert_delete(name, QAPI_NET_SSL_RPK_CERT_E);
    }
    else if ( strcmp(file_type_di_certificate, file_type) == 0 ) {
        status = qapi_Net_SSL_Cert_delete(name, QAPI_NET_SSL_DI_CERT_E);
    }
    else if ( strcmp(file_type_ca_list, file_type) == 0 ) {
        status = qapi_Net_SSL_Cert_delete(name, QAPI_NET_SSL_CA_LIST_E);
        }
    else if ( strcmp(file_type_psk_table, file_type) == 0 ) {
        status = qapi_Net_SSL_Cert_delete(name, QAPI_NET_SSL_PSK_TABLE_E);
    }
    else
    {
      goto delete_cert_on_error;
    }

    if (QAPI_OK == status)
    {
        CERT_PRINTF("Deleted %s from NV memory\n", name);
    }
    else
    {
        CERT_PRINTF("ERROR: failed to delete %s\n", name);
        goto delete_cert_on_error;
    }

    result = QCLI_STATUS_SUCCESS_E;
    goto end;

delete_cert_on_error:
    CERT_PRINTF("Usage: cert delete <name> -t <filetype>\n"
            "This deletes the certificate or CA list with <name> from non-volatile memory. File type could be \"cert\" or \"rpk_cert\" or \"ca_list\" or \"psk_table\" \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;

end:
  if (Parameter_List != NULL)
  {
        for (i = 0; i<Parameter_Count; i++)
        {
          if (Parameter_List[i].String_Value != NULL)
          {
            tx_byte_release(Parameter_List[i].String_Value);
            Parameter_List[i].String_Value = NULL;
          }
        }
  }
  return result;
}

QCLI_Command_Status_t list_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    qapi_Net_SSL_Cert_List_t list;
    int32_t i, numFiles;
    QCLI_Command_Status_t status =  QCLI_STATUS_SUCCESS_E;

    if ( Parameter_Count < 2 ) {
        goto list_cert_on_error;
    }

    if ( 0 != strcmp("-t", (char *)Parameter_List[0].String_Value) ) {
        goto list_cert_on_error;
    }

    if ( 0 == strcmp(file_type_certificate, (char*)Parameter_List[1].String_Value) ) {
        numFiles = qapi_Net_SSL_Cert_List(QAPI_NET_SSL_CERTIFICATE_E, &list);
    }
    else if ( 0 == strcmp(file_type_rpk_certificate, (char*)Parameter_List[1].String_Value) ) {
        numFiles = qapi_Net_SSL_Cert_List(QAPI_NET_SSL_RPK_CERT_E, &list);
    }
    else if ( 0 == strcmp(file_type_di_certificate, (char*)Parameter_List[1].String_Value) ) {
        numFiles = qapi_Net_SSL_Cert_List(QAPI_NET_SSL_DI_CERT_E, &list);
    }
    else if ( 0 == strcmp(file_type_ca_list, (char*)Parameter_List[1].String_Value) ) {
        numFiles = qapi_Net_SSL_Cert_List(QAPI_NET_SSL_CA_LIST_E, &list);
    }
    else if ( 0 == strcmp(file_type_psk_table, (char*)Parameter_List[1].String_Value) ) {
        numFiles = qapi_Net_SSL_Cert_List(QAPI_NET_SSL_PSK_TABLE_E, &list);
    }
    else {
        goto list_cert_on_error;
    }

    if (numFiles < 0)
    {
        CERT_PRINTF("ERROR: failed to list files (%d)\r\n", numFiles);
        status = QCLI_STATUS_ERROR_E;
        goto end;
    }

    CERT_PRINTF("%d %s stored in NV memory\r\n", numFiles, numFiles == 1 ? "file" : "files");
    for (i=0; i<numFiles; i++)
    {
        CERT_PRINTF("%s\r\n", list.name[i]);
    }

    status = QCLI_STATUS_SUCCESS_E;
    goto end;

list_cert_on_error:
        CERT_PRINTF("Usage: cert list -t <filetype>\r\n"
                "Lists the certificates / CA lists stored on the device. File type could be \"cert\" or \"rpk_cert\" or \"ca_list\" or \"psk_table\" \r\n");
        status = QCLI_STATUS_ERROR_E;
        goto end;

 end:
  if (Parameter_List != NULL)
  {
        for (i = 0; i<Parameter_Count; i++)
        {
          if (Parameter_List[i].String_Value != NULL)
          {
            tx_byte_release(Parameter_List[i].String_Value);
            Parameter_List[i].String_Value = NULL;
          }
        }
  }
  return status;
}

/************************************************************************
 ************************************************************************/
QCLI_Command_Status_t get_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    int32_t res = -1;
    struct sockaddr_in sockaddr;
    struct sockaddr_in6 sockaddr6;
    struct sockaddr *to;
    uint32_t tolen;
    uint32_t socketHandle = 0;
    int reqLen;
    CERT_HEADER_T *req = NULL;
    CERT_HEADER_T *header;
    uint8_t *buf = NULL;
    int certNameLen, numRead = 0, index, port = 1443;
    char *host, *certName, *flashName = NULL;
    char * file_type = NULL;
    struct ip46addr ipaddr;
    int32_t scope_id = 0;
    int retval ;
    int i;
    QCLI_Command_Status_t status =  QCLI_STATUS_SUCCESS_E;

    // Free certificate buffer if allocated
    if (cert_data_buf[app_id])
    {
        tx_byte_release(cert_data_buf[app_id]);
        cert_data_buf[app_id] = NULL;
    }
    cert_data_buf_len[app_id] = 0;

    // Parse the arguments
    if(Parameter_Count < 3)
    {
        CERT_PRINTF("\nUsage: cert get <name> <host> [-p <port>] [-s <filename>] [-t <filetype>]\n");
        CERT_PRINTF("  <name>  = Name of the certificate or CA list file to retrieve\n");
        CERT_PRINTF("  <host>  = Host name or IP address of certificate server\n");
        CERT_PRINTF("  <port>  = Optional TCP port number\n");
        CERT_PRINTF("  <filename> = File name to store certificate/ca_list in non-volatile memory\n");
        CERT_PRINTF("  <filetype> = File type could be \"cert\" or \"ca_list\" or \"psk_table\" \n");

        status = QCLI_STATUS_ERROR_E;
        goto ret;
    }

    certName    = (char*)Parameter_List[0].String_Value;
    host        = (char*)Parameter_List[1].String_Value;
    for(index = 2; index < Parameter_Count ; index++)
    {
        if(Parameter_List[index].String_Value[0] == '-')
        {
            switch(Parameter_List[index].String_Value[1])
            {
                case 'p':
                    index++;
                    port = atoi((char*)Parameter_List[index].String_Value);
                    break;
                case 's':
                    index++;
                    if (index < Parameter_Count)
                        flashName = (char*)Parameter_List[index].String_Value;
                    break;
                case 't':
                    index++;
                    file_type = (char*)Parameter_List[index].String_Value;
                    break;
                default:
                    QCLI_Printf(qcli_net_handle, "Unknown option: %s\n", Parameter_List[index].String_Value);
                    status = QCLI_STATUS_ERROR_E;
                    goto ret;
            }
        }
    }

    memset(&ipaddr, 0, sizeof(ipaddr));
    /* Get IP address of Peer */
    do
    {
        if (inet_pton(AF_INET, host, &ipaddr.a) == 0)
        {
            ipaddr.type = AF_INET;
            break;
        }

        if (inet_pton(AF_INET6, host, &ipaddr.a) == 0)
        {
            if ( QAPI_IS_IPV6_LINK_LOCAL(&ipaddr.a) )
            {
                // if this is a link local address, then the interface must be specified after %
                int host_argument_length = strlen(host);
                char * interface_name_with_percent_char = strchr(host, '%');
                char * interface_name = NULL;
                if ( interface_name_with_percent_char ) {
                    interface_name = interface_name_with_percent_char + 1;
                }
                else {
                    QCLI_Printf(qcli_net_handle, "Link local IPv6 address is used, must append %%interface_name to the address\n");
                    QCLI_Printf(qcli_net_handle, "For example: fe80::a2ce:c8ff:fe03:68f4%%wlan1 if you intend to talk over link local address on interface wlan1\n");
                    status =  QCLI_STATUS_ERROR_E;
                    goto ret;
                }

                retval = qapi_Net_IPv6_Get_Scope_ID(interface_name, &scope_id);
                if ( retval != 0 ) {
                    QCLI_Printf(qcli_net_handle, "Failed to get scope id for the interface %s\n", interface_name);
                    status = QCLI_STATUS_ERROR_E;
                    goto ret;
                }
            }
            ipaddr.type = AF_INET6;
            break;
        }

        if (strlen(host) >= CERT_DEMO_DNS_MAX_HOSTNAME_LEN)
        {
            QCLI_Printf(qcli_net_handle, "hostname cannot be more than %d bytes\n", CERT_DEMO_DNS_MAX_HOSTNAME_LEN);
            status = QCLI_STATUS_ERROR_E;
            goto ret;
        }

        ipaddr.type = AF_INET;
        /*
        if (qapi_Net_DNSc_Reshost(host, &ipaddr) == 0)
        {
            break;
        }

        ipaddr.type = AF_INET6;
        if (qapi_Net_DNSc_Reshost(host, &ipaddr) == 0)
        {
            break;
        }
        */
        QCLI_Printf(qcli_net_handle, "Incorrect server name or IP address %s\n", host);
        status = QCLI_STATUS_ERROR_E;
        goto ret;

    } while (0);

    /* Create a TCP socket */
    if ((int)(socketHandle = qapi_socket(ipaddr.type, SOCK_STREAM, 0))== -1 )
    {
        QCLI_Printf(qcli_net_handle, "ERROR: Unable to create socket\n");
        status = QCLI_STATUS_ERROR_E;
        goto ret;
    }

    if (ipaddr.type == AF_INET)
    {
        memset(&sockaddr, 0, sizeof(sockaddr));
        sockaddr.sin_addr.s_addr = ipaddr.a.addr4;
        sockaddr.sin_port = htons(port);
        sockaddr.sin_family = AF_INET;

        to      = (struct sockaddr *)&sockaddr;
        tolen   = sizeof(sockaddr);
    }
    else
    {
        memset(&sockaddr6, 0, sizeof(sockaddr6));
        memcpy(&sockaddr6.sin_addr, &ipaddr.a, sizeof(ip6_addr));
        sockaddr6.sin_port     = htons(port);
        sockaddr6.sin_family   = AF_INET6;
        sockaddr6.sin_scope_id = scope_id;

        to      = (struct sockaddr *)&sockaddr6;
        tolen   = sizeof(sockaddr6);
    }

    /* Connect to the server.*/
    if (((int)qapi_connect(socketHandle, to, tolen)) == -1)
    {
        QCLI_Printf(qcli_net_handle, "Connection failed.\n");
        res = -1;
        goto end;
    }

    // Build and send request
    certNameLen = strlen(certName);
    reqLen = CERT_HEADER_LEN + certNameLen;
    tx_byte_allocate(byte_pool_qcli, (VOID **) &req, reqLen, TX_NO_WAIT);

    if (req == NULL)
    {
        QCLI_Printf(qcli_net_handle, "ERROR: Out of memory.\n");
        res = -1;
        goto end;
    }
    req->id[0] = 'C';
    req->id[1] = 'R';
    req->id[2] = 'T';
    req->id[3] = '0';
    req->length = htonl(certNameLen);
    memcpy(&req->data[0], certName, certNameLen);
    res = qapi_send(socketHandle, (char *)req, reqLen, 0);

    /*Free the buffer only if NON_BLOCKING_TX is not enabled*/
    if(req)
        tx_byte_release(req);

    if (res < 0 )
    {
        QCLI_Printf(qcli_net_handle, "ERROR: send error = %d\n", res);
        goto end;
    }
    //else if(res == A_SOCK_INVALID )
    //{
    //    /*Socket has been closed by target due to some error, gracefully exit*/
    //    QCLI_Printf(qcli_net_handle, "ERROR: Socket closed unexpectedly\n");
    //    return -1;
    //}

    tx_byte_allocate(byte_pool_qcli, (VOID **) &buf, SSL_CERT_BUF_LEN, TX_NO_WAIT);
    if (buf  == NULL) {
        QCLI_Printf(qcli_net_handle, "\nmem alloc failed\n");
        res = -1;
        goto end;
    }

    // Read the response
    do
    {
        fd_set rset;
        qapi_fd_zero(&rset);
        qapi_fd_set(socketHandle, &rset);

        res = qapi_select(&rset, NULL, NULL, CLIENT_WAIT_TIME);
        if (res > 0)
        {
            res = qapi_recv(socketHandle, (char *)buf, SSL_CERT_BUF_LEN, 0);
            if (res > 0)
            {
                if (cert_data_buf_len[app_id] == 0)
                {
                    if (buf[0] != 'C' || buf[1] != 'R' || buf[2] != 'T')
                    {
                        QCLI_Printf(qcli_net_handle, "ERROR: Bad MAGIC received in header\n");
                        break;
                    }
                    header = (CERT_HEADER_T*)buf;
                    header->length =  ntohl(header->length);
                    if (header->length == 0)
                    {
                        break;
                    }

                    tx_byte_allocate(byte_pool_qcli, (VOID **) &cert_data_buf[app_id], header->length, TX_NO_WAIT);
                    if (cert_data_buf[app_id] == NULL)
                    {
                        QCLI_Printf(qcli_net_handle, "ERROR: Out of memory error\n");
                        res = -1;
                        break;
                    }
                    cert_data_buf_len[app_id] = header->length;
                    res -= OFFSETOF(CERT_HEADER_T, data);
                    memcpy(cert_data_buf[app_id], header->data, res);
                    numRead = res;
                }
                else
                {
                    if (res + numRead <= cert_data_buf_len[app_id])
                    {
                        memcpy(&cert_data_buf[app_id][numRead], buf, res);
                        numRead += res;
                        res = cert_data_buf_len[app_id];
                    }
                    else
                    {
                        QCLI_Printf(qcli_net_handle, "ERROR: read failed\n");
                        res = -1;
                        break;
                    }
                }
            }
            else
            {
                QCLI_Printf(qcli_net_handle, "ERROR: no response\n");
                res = -1;
                break;
            }
        }
        else
        {
            if (res < 0)
            {
                QCLI_Printf(qcli_net_handle, "ERROR: no response\n");
                res = -1;
                break;
            }
        }
    } while (numRead < cert_data_buf_len[app_id]);

    if (res == cert_data_buf_len[app_id])
    {
        QCLI_Printf(qcli_net_handle, "Received %d bytes from %s:%d\n", cert_data_buf_len[app_id], host, port);

        if (flashName == NULL)
        {
            flashName = certName;
        }

        /* store certificate in NV memory */
        if ( 0 ||
            !file_type ||
            !( (strcmp(file_type_ca_list, file_type) == 0) || 
               (strcmp(file_type_certificate, file_type) == 0) || 
               (strcmp(file_type_di_certificate, file_type) == 0) || 
               (strcmp(file_type_psk_table, file_type) == 0))
            )
        {
            CERT_PRINTF("ERROR: the filetype must be either cert or ca_list\n");
            res = -1;
            goto end;
        }

        if ( strcmp(file_type_certificate, file_type) == 0 ) {
            res = qapi_Net_SSL_Cert_Store(flashName, QAPI_NET_SSL_CERTIFICATE_E, cert_data_buf[app_id], cert_data_buf_len[app_id]);
        }
        else if ( strcmp(file_type_di_certificate, file_type) == 0) {
            res = qapi_Net_SSL_Cert_Store(flashName, QAPI_NET_SSL_DI_CERT_E, cert_data_buf[app_id], cert_data_buf_len[app_id]);
        }
        else if ( strcmp(file_type_ca_list, file_type) == 0) {
            res = qapi_Net_SSL_Cert_Store(flashName, QAPI_NET_SSL_CA_LIST_E, cert_data_buf[app_id], cert_data_buf_len[app_id]);
        }
        else if ( strcmp(file_type_psk_table, file_type) == 0) {
            res = qapi_Net_SSL_Cert_Store(flashName, QAPI_NET_SSL_PSK_TABLE_E, cert_data_buf[app_id], cert_data_buf_len[app_id]);
        }


        if (res == QAPI_OK)
        {
            QCLI_Printf(qcli_net_handle, "'%s' is stored in NV memory\n", flashName);
        }
        else
        {
            QCLI_Printf(qcli_net_handle, "ERROR: failed to store %s in NV memory\n", flashName);
            res = -1;
        }
    }
    else {
        QCLI_Printf(qcli_net_handle, "\nError in receiving cert\n");
    }

end:
    if (buf)
    {
        tx_byte_release(buf);
    }

    if (socketHandle)
    {
        qapi_socketclose(socketHandle);
    }

    if (res < 0) {
        status = QCLI_STATUS_ERROR_E;
        goto ret;
    }
    else {
        status = QCLI_STATUS_SUCCESS_E;
        goto ret;
    }

ret:
  if (Parameter_List != NULL)
  {
        for (i = 0; i<Parameter_Count; i++)
        {
          if (Parameter_List[i].String_Value != NULL)
          {
            tx_byte_release(Parameter_List[i].String_Value);
            Parameter_List[i].String_Value = NULL;
          }
        }
    }
  return status;
}
#endif

