/*!
  @file
  ssl_dam_demo.c

  @brief
  SSL test functionality using QCLI
*/
/*===========================================================================
  Copyright (c) 2017 - 2021  by Qualcomm Technologies, Inc.  All Rights Reserved.

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
 ======================================================================*/

/* Copyright (c) 2011-2021.  Qualcomm Atheros, Inc.
All rights reserved.

Qualcomm Atheros Confidential and Proprietary.

*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "qapi_ssl.h"
#include "qapi_status.h"
#include "bench.h"
#include "qapi_netservices.h"

#ifdef INCLUDE_SSLDEMO

extern QCLI_Group_Handle_t qcli_net_handle; /* Handle for Net Command Group. */

extern QCLI_Command_t net_cmd_list[];

extern bool thrd_running[NO_OF_APPS];

/* TODO Create default certificate/CA list to be used for testing */
uint8_t const *ssl_default_cert;
const uint16_t ssl_default_cert_len;
uint8_t const *ssl_default_calist;
const uint16_t ssl_default_calist_len;
uint8_t const *ssl_default_psktable;
const uint16_t ssl_default_psktable_len;

#define SSL_PRINTF(...) QCLI_Printf(qcli_net_handle, __VA_ARGS__)

#define INVALID_UINT32 0xFFFFFFFF
#define INVALID_UINT8 0xFF

QCLI_Command_Status_t ssl_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = SSL_COMMAND_HANDLER_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
#ifndef MINI_QCLI
  cmd_parser_temp->Command_Index = 12;
#else
  cmd_parser_temp->Command_Index = 9;
#endif

  result = multi_app_cmd_fill_func(cmd_parser_temp);

  return result;
}


QCLI_Command_Status_t
proc_ssl_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    int i;
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

    if(Parameter_Count < 1)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    if (0 == strcmp((char*)Parameter_List[0].String_Value, "start")) {
        result = ssl_start(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp((char*)Parameter_List[0].String_Value, "stop")) {
        result = ssl_stop(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp((char*)Parameter_List[0].String_Value, "config")) {
        result = ssl_config(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp((char*)Parameter_List[0].String_Value, "cert")) {
        result = ssl_add_cert(Parameter_Count - 1, &Parameter_List[1], app_id);
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
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

QCLI_Command_Status_t ssl_start(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    SSL_INST *ssl = 0;
    qapi_Net_SSL_Role_t role = QAPI_NET_SSL_CLIENT_E;
    static uint32_t init = 0;
    int i;
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

    if(Parameter_Count < 1)
    {
usage:
        SSL_PRINTF("\nUsage: ssl [-app <app_id>] start <server|client>\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    if (init == 0)
    {
        init = 1;
        memset(&ssl_inst[app_id][SSL_SERVER_INST], 0, sizeof(SSL_INST));
        memset(&ssl_inst[app_id][SSL_CLIENT_INST], 0, sizeof(SSL_INST));
    }

	if (0 == strcmp((char*)Parameter_List[0].String_Value, "server"))
	 {
		 ssl = &ssl_inst[app_id][SSL_SERVER_INST];
		 role = QAPI_NET_SSL_SERVER_E;
	 }
	 else if (0 == strcmp((char*)Parameter_List[0].String_Value, "client"))
	 {
		 ssl = &ssl_inst[app_id][SSL_CLIENT_INST];
		 role = QAPI_NET_SSL_CLIENT_E;
	 }
	 else
	 {
		goto usage;
	 }


    if (ssl->sslCtx != QAPI_NET_SSL_INVALID_HANDLE)
    {
        SSL_PRINTF("ERROR: one SSL context only!\r\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    SSL_PRINTF("\nUsage: qapi_Net_SSL_Obj_New\n");
    // Create SSL context
    memset(ssl, 0, sizeof(SSL_INST));
    ssl->role = role;
    ssl->sslCtx = qapi_Net_SSL_Obj_New(role);

    if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
    {
        SSL_PRINTF("ERROR: Unable to create SSL context\r\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    // Reset config struct
    memset(&ssl->config, 0, sizeof(qapi_Net_SSL_Config_t));

    // Done
    SSL_PRINTF("SSL %s started\r\n", Parameter_List[0].String_Value);

end:
  if (Parameter_List != NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

QCLI_Command_Status_t ssl_stop(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    SSL_INST *ssl;
    qapi_Net_SSL_Role_t role;
    int i;
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

    if(Parameter_Count < 1)
    {
usage:
        SSL_PRINTF("\nUsage: ssl [-app <app_id>] stop <client>\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    if (0 == strcmp((char*)Parameter_List[0].String_Value, "server"))
    {
        ssl = &ssl_inst[app_id][SSL_SERVER_INST];
        role = QAPI_NET_SSL_SERVER_E;
    }
    else if (0 == strcmp((char*)Parameter_List[0].String_Value, "client"))
    {
        ssl = &ssl_inst[app_id][SSL_CLIENT_INST];
        role = QAPI_NET_SSL_CLIENT_E;
    }
    else
    {
        goto usage;
    }

    if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE || role != ssl->role)
    {
        SSL_PRINTF("ERROR: SSL %s not started\r\n", Parameter_List[0].String_Value);
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    if (ssl->ssl)
    {
        qapi_Net_SSL_Shutdown(ssl->ssl);
        ssl->ssl = QAPI_NET_SSL_INVALID_HANDLE;
    }

    if (ssl->sslCtx)
    {
        qapi_Net_SSL_Obj_Free(ssl->sslCtx);
        ssl->sslCtx = QAPI_NET_SSL_INVALID_HANDLE;
    }

    if(ssl->config.sni_Name  != NULL) {
        free(ssl->config.sni_Name );
    }

    SSL_PRINTF("SSL %s stopped\r\n", Parameter_List[0].String_Value);

end:
  if (Parameter_List != NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

qapi_Net_SSL_Protocol_t findproto(uint16_t protocol)
{
    qapi_Net_SSL_Protocol_t proto;

    switch(protocol)
    {
        case QAPI_NET_SSL_PROTOCOL_TLS_1_0:
        case QAPI_NET_SSL_PROTOCOL_TLS_1_1:
        case QAPI_NET_SSL_PROTOCOL_TLS_1_2:
        case QAPI_NET_SSL_PROTOCOL_TLS_1_3:
            proto = QAPI_NET_SSL_TLS_E;
            break;
        case QAPI_NET_SSL_PROTOCOL_DTLS_1_0:
        case QAPI_NET_SSL_PROTOCOL_DTLS_1_2:
            proto = QAPI_NET_SSL_DTLS_E;
            break;
        default:
            proto = (qapi_Net_SSL_Protocol_t)QAPI_NET_SSL_PROTOCOL_UNKNOWN;
    }

    return proto;
}

/*used by http, without extcfg-sessionname configuration;
  in http, http API client provide the SSL config and then HTTP control the SSL connection, no QAPI-HTTP can configure the session name now*/
void utility_command_line_help(const char *str, const char* extcfg)
{
    SSL_PRINTF("\n%s [protocol <protocol>]"
               " [time 0|1]"
               " [domain 0|<name>]"
               " [alert 0|1]"
               " [cipher <cipher>]"
               " [max_frag_len <bytes>]"
               " [neg_disable 0|1]"
               " [sni <name>] %s\n\n", str, extcfg);
}

/*used by http, without extcfg-sessionname configuration;
  in http, http API client provide the SSL config and then HTTP control the SSL connection, no QAPI-HTTP can configure the session name now*/
void utility_parameter_help()
{
    SSL_PRINTF("\n   where:\n");
	SSL_PRINTF("   protocol <protocol> = select protocol: TLS1.0, TLS1.1, TLS1.2, TLS1.3, DTLS1.0, DTLS1.2\n\n");
    SSL_PRINTF("   time 0|1            = disable|enable certificate time validation (optional)\n\n");
    SSL_PRINTF("   domain 0|<name>     = disable|enable validation of peer's domain name against <name>\n\n");
    SSL_PRINTF("   alert 0|1           = disable|enable sending of SSL alert if certificate validation fails.\n\n");
    SSL_PRINTF("   cipher <cipher>     = select <cipher> (enum name) suite to use, can be repeated 8 times (optional)\n\n");
    SSL_PRINTF("   max_frag_len <bytes>= max fragment length in bytes\n\n");
    SSL_PRINTF("   neg_disable 0|1     = disable maximum fragment length negotiation\n\n");
    SSL_PRINTF("   sni sni_Name   = configure name for server name indication (SNI)\n\n");
}

/*used by http/mqtt, without extcfg-sessionname configuration;
  http/mqtt level API support SSL config passthrough but not support extcfg setting*/
void sslconfig_help(const char *str)
{
	utility_command_line_help(str, "");
	utility_parameter_help();
}

/*used by SSL,  with extcfg-sessionname configuration,
  for those application that can control the SSL connection*/
void sslconfig_help_extcfg(const char *str)
{
	utility_command_line_help(str, "[extcfgsession <name>] [extcfgsesslifetime <lifetime>] [extcfgconntimer <timeout>] ...other ext_cfg as list below...");
	utility_parameter_help();
    SSL_PRINTF("   extcfgsession      session_Name     = configure session name\n\n");
    SSL_PRINTF("   extcfgsesslifetime session_lifetime = configure session lifetime\n\n");
    SSL_PRINTF("   extcfgconntimer    connect_timeout  = configure connection timer\n\n");

    SSL_PRINTF("   extcfgdisableclosenotify     disable_close_notify      = disable close notify\n\n");
    SSL_PRINTF("   extcfgmintlsprotocol         min_tls_prot              = select protocol: TLS1.0, TLS1.1, TLS1.2, TLS1.3\n\n");
    SSL_PRINTF("   extcfgkeyexchangegroup       <key_ex_group>            = select <key_ex_group> (enum name) gourp name to use, can be repeated 5 times (optional)\n\n");
    SSL_PRINTF("   extcfgkeyexchangenegotiate   key_ex_negotiate          = key exchange negotiate\n\n");
    SSL_PRINTF("   extcfgallowserverrpk         allow_server_rpk          = whether allow server to pick raw public key certificate\n\n");
	SSL_PRINTF("   extcfgalertwithepoch0        handle_alert_with_epoch0  = After handshake done, DTLS whether handle alert if epoch is 0\n\n");
	SSL_PRINTF("   extcfgenforceextmaster       enforce_ext_master_secret = whether enforce extended master secret to be used for key calculation\n\n");
	SSL_PRINTF("   extcfgenforcesecurereneg     enforce_secure_reneg      = whether enforce server to must support secure renegotiation\n\n");
	SSL_PRINTF("   extcfgenablerenegextension   enable_reneg_extension    = whether enable using of renegotiation_info extension in stead of SCSV\n\n");
    SSL_PRINTF("   extcfgenablenonsectls12      enable_nonsectls12        = enable nonsec tls12 features\n\n");

}

QCLI_Command_Status_t ssl_parse_config_parameters_extcfg(
        uint32_t Parameter_Count,
        QCLI_Parameter_t *Parameter_List,
        qapi_Net_SSL_Config_t *cfg,
        qbool_t server, char* session_name, uint32_t* session_lifetime, uint32_t* conn_timeout, uint32_t* disable_close_notify, uint8_t* min_tls_prot, uint8_t* key_ex_group,
        uint32_t * group_count, uint8_t* key_ex_negotiate, uint32_t* allow_server_rpk, uint8_t * alert_invalid_epoch, uint8_t * enforce_ext_master_secret, uint8_t * enforce_secure_reneg, uint8_t * enable_reneg_extension, uint8_t * enable_nonsectls12)
{
    uint32_t argn;
    int cipher_count = 0;
    int i;
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

    memset(cfg, 0, sizeof(*cfg));
    if(group_count != NULL)
    {
      *group_count = 0;
    }
    for (argn = 0; argn < Parameter_Count; ++argn)
    {
        if (argn == Parameter_Count-1)
        {
            SSL_PRINTF("What is value of %s?\n", Parameter_List[argn].String_Value);
            result = QCLI_STATUS_ERROR_E;
            goto end;
        }

        /* 'protocol TLS1.2' */
        if (0 == strncmp("protocol", (const char *)Parameter_List[argn].String_Value, 8))
        {
            argn++;

            // Setting of protocol version is supported for SSL client only,
            // but SSL server uses this to set the protocol type (DTLS/TLS).
            if (server)
            {
                SSL_PRINTF("Warning: Server uses only protocol type (DTLS/TLS) and ignores version (1.1, 1.2, ...).\n");
            }

            if (0 == strcmp("TLS1.0", (const char *)Parameter_List[argn].String_Value))
            {
                cfg->protocol = QAPI_NET_SSL_PROTOCOL_TLS_1_0;
            }
            else if (0 == strcmp("TLS1.1", (const char *)Parameter_List[argn].String_Value))
            {
                cfg->protocol = QAPI_NET_SSL_PROTOCOL_TLS_1_1;
            }
            else if (0 == strcmp("TLS1.2", (const char *)Parameter_List[argn].String_Value))
            {
                cfg->protocol = QAPI_NET_SSL_PROTOCOL_TLS_1_2;
            }
            else if (0 == strcmp("TLS1.3", (const char *)Parameter_List[argn].String_Value))
            {
                cfg->protocol = QAPI_NET_SSL_PROTOCOL_TLS_1_3;
            }
            else if (0 == strcmp("DTLS1.0", (const char *)Parameter_List[argn].String_Value))
            {
                cfg->protocol = QAPI_NET_SSL_PROTOCOL_DTLS_1_0;
            }
            else if (0 == strcmp("DTLS1.2", (const char *)Parameter_List[argn].String_Value))
            {
                cfg->protocol = QAPI_NET_SSL_PROTOCOL_DTLS_1_2;
            }
            else
            {
                SSL_PRINTF("ERROR: Invalid protocol: %s\n", Parameter_List[argn].String_Value);
                result = QCLI_STATUS_ERROR_E;
                goto end;
            }
        }

        /* 'time 1' */
        else if (0 == strncmp("time", (const char *)Parameter_List[argn].String_Value, 4))
        {
            argn++;

            if (0 == strcmp("1", (const char *)Parameter_List[argn].String_Value))
            {
                cfg->verify.time_Validity = 1;
            }
            else if (0 == strcmp("0", (const char *)Parameter_List[argn].String_Value))
            {
                cfg->verify.time_Validity = 0;
            }
            else
            {
                SSL_PRINTF("ERROR: Invalid option: %s\n", Parameter_List[argn].String_Value);
                result = QCLI_STATUS_ERROR_E;
                goto end;
            }
        }

        /* 'alert 0 ' */
        else if (0 == strncmp("alert", (const char *)Parameter_List[argn].String_Value, 5))
        {
            argn++;

            if (0 == strcmp("1", (const char *)Parameter_List[argn].String_Value))
            {
                cfg->verify.send_Alert = 1;
            }
            else if (0 == strcmp("0", (const char *)Parameter_List[argn].String_Value))
            {
                cfg->verify.send_Alert = 0;
            }
            else
            {
                SSL_PRINTF("ERROR: Invalid option: %s\n", Parameter_List[argn].String_Value);
                result = QCLI_STATUS_ERROR_E;
                goto end;
            }
        }

        /* 'domain 0' */
        else if (0 == strncmp("domain", (const char *)Parameter_List[argn].String_Value, 6))
        {
            argn++;

            if (0 == strcmp("0", (const char *)Parameter_List[argn].String_Value))
            {
                cfg->verify.domain = 0;
                cfg->verify.match_Name[0] = '\0';
            }
            else
            {
                cfg->verify.domain = 1;
                if (strlen((const char *)Parameter_List[argn].String_Value) >= sizeof(cfg->verify.match_Name))
                {
                    SSL_PRINTF("ERROR: %s too long (max %d chars)\n", Parameter_List[argn].String_Value, sizeof(cfg->verify.match_Name));
                    result = QCLI_STATUS_ERROR_E;
                    goto end;
                }

                memcpy(cfg->verify.match_Name, (const char *)Parameter_List[argn].String_Value, strlen((const char *)Parameter_List[argn].String_Value) + 1);
            }
        }

        /* 'cipher QAPI_NET_TLS_RSA_WITH_AES_256_GCM_SHA384' */
        else if (0 == strncmp("cipher", (const char *)Parameter_List[argn].String_Value, 6))
        {
            qbool_t is_valid_cipher = true;

            argn++;

            if (cipher_count == QAPI_NET_SSL_CIPHERSUITE_LIST_DEPTH)
            {
                SSL_PRINTF("ERROR: Too many cipher options %d (max %d)\r\n",(char*)Parameter_List[argn].String_Value, QAPI_NET_SSL_CIPHERSUITE_LIST_DEPTH);
                result = QCLI_STATUS_ERROR_E;
                goto end;
            }

            if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_RC4_128_SHA",(char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_PSK_WITH_RC4_128_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_3DES_EDE_CBC_SHA",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_PSK_WITH_3DES_EDE_CBC_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_AES_128_GCM_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_PSK_WITH_AES_128_GCM_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_AES_256_GCM_SHA384",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_PSK_WITH_AES_256_GCM_SHA384;
            }
            else if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA384",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA384;
            }
            else if (0 == strcmp("QAPI_NET_TLS_PSK_WITH_AES_128_CCM_8",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_PSK_WITH_AES_128_CCM_8;
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_256_GCM_SHA384",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_RSA_WITH_AES_256_GCM_SHA384;
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_256_CBC_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_RSA_WITH_AES_256_CBC_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_256_CBC_SHA",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_RSA_WITH_AES_256_CBC_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CBC_SHA",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CBC_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_128_GCM_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_RSA_WITH_AES_128_GCM_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_128_CBC_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_RSA_WITH_AES_128_CBC_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_128_CBC_SHA",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_RSA_WITH_AES_128_CBC_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CBC_SHA",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CBC_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256",(char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384;
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_128_CCM",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_RSA_WITH_AES_128_CCM;
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_256_CCM",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_RSA_WITH_AES_256_CCM;
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CCM",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CCM;
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CCM",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CCM;
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_128_CCM_8",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_RSA_WITH_AES_128_CCM_8;
            }
            else if (0 == strcmp("QAPI_NET_TLS_RSA_WITH_AES_256_CCM_8",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_RSA_WITH_AES_256_CCM_8;
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CCM_8",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CCM_8;
            }
            else if (0 == strcmp("QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CCM_8",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CCM_8;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA;
            }
            else if (0 == strcmp("QAPI_NET_TLS13_AES_128_GCM_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS13_AES_128_GCM_SHA256;
            }
            else if (0 == strcmp("QAPI_NET_TLS13_AES_256_GCM_SHA384",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS13_AES_256_GCM_SHA384;
            }
            else if (0 == strcmp("QAPI_NET_TLS13_CHACHA20_POLY1305_SHA256",(const char*)Parameter_List[argn].String_Value))
            {
                cfg->cipher[cipher_count] = QAPI_NET_TLS13_CHACHA20_POLY1305_SHA256;
            }
            else
            {
                SSL_PRINTF("Warning: Ignoring invalid cipher: %s\n", Parameter_List[argn].String_Value);
                is_valid_cipher = false;
            }

            if (is_valid_cipher)
                cipher_count++;
        }

        /* 'max_frag_len 1000' */
        else if (0 == strncmp("max_frag_len", (const char *)Parameter_List[argn].String_Value, 12))
        {
            argn++;
            cfg->max_Frag_Len = atoi((const char *)Parameter_List[argn].String_Value);
        }

        /* 'neg_disable 1' */
        else if (0 == strncmp("neg_disable", (const char *)Parameter_List[argn].String_Value, 11))
        {
            argn++;
            cfg->max_Frag_Len_Neg_Disable = atoi((const char *)Parameter_List[argn].String_Value);
        }
        /* 'sni name' */
        else if(0 == strncmp("sni", (const char *) Parameter_List[argn].String_Value, 3)) {
            argn++;
            if(server == true) {
                SSL_PRINTF("Warning: ignoring SNI option, this only valid for clients\n");
            } else {
                cfg->sni_Name_Size = strlen((const char *)Parameter_List[argn].String_Value);
                cfg->sni_Name  = (char *)malloc(cfg->sni_Name_Size);
                if(cfg->sni_Name  == NULL) {
                    SSL_PRINTF("Error: unable to allocate memory for password\n");
                    result = QCLI_STATUS_ERROR_E;
                    goto end;
                }
                memcpy(cfg->sni_Name , Parameter_List[argn].String_Value, cfg->sni_Name_Size);
            }
        }
		else if(0 == strncmp("extcfgsession", (const char *) Parameter_List[argn].String_Value, strlen("extcfgsession")))
		{
          argn++;

		  if (server == true)
          {
            SSL_PRINTF("Warning: ignoring extcfgsession option, this only valid for clients\n");
          }
          else if(session_name != NULL)
          strlcpy(session_name, (char*)Parameter_List[argn].String_Value, 255);
        }
        else if(0 == strncmp("extcfgsesslifetime", (const char *) Parameter_List[argn].String_Value, strlen("extcfgsesslifetime")))
        {
          argn++;

          if (server == true)
          {
            SSL_PRINTF("Warning: ignoring extcfgsession option, this only valid for clients\n");
          }
		  else if(session_lifetime)
           *session_lifetime = atoi((char *)Parameter_List[argn].String_Value);
        }
        else if(0 == strncmp("extcfgconntimer", (const char *) Parameter_List[argn].String_Value, strlen("extcfgconntimer")))
        {
          argn++;

          if (server == true)
          {
            SSL_PRINTF("Warning: ignoring extcfgsession option, this only valid for clients\n");
          }
		  else if(conn_timeout)
           *conn_timeout = atoi((char *)Parameter_List[argn].String_Value);
        }
        else if(0 == strncmp("extcfgdisableclosenotify", (const char *) Parameter_List[argn].String_Value, strlen("extcfgdisableclosenotify")))
        {
          argn++;
          if(disable_close_notify)
           *disable_close_notify = atoi((char *)Parameter_List[argn].String_Value);
        }
        else if(0 == strncmp("extcfgmintlsprotocol", (const char *) Parameter_List[argn].String_Value, strlen("extcfgmintlsprotocol")))
        {
          argn++;
          if(min_tls_prot)
          {
            if (0 == strcmp("TLS1.0", (const char *)Parameter_List[argn].String_Value))
            {
                *min_tls_prot = QAPI_NET_SSL_PROTOCOL_TLS_1_0;
            }
            else if (0 == strcmp("TLS1.1", (const char *)Parameter_List[argn].String_Value))
            {
                *min_tls_prot = QAPI_NET_SSL_PROTOCOL_TLS_1_1;
            }
            else if (0 == strcmp("TLS1.2", (const char *)Parameter_List[argn].String_Value))
            {
                *min_tls_prot = QAPI_NET_SSL_PROTOCOL_TLS_1_2;
            }
            else if (0 == strcmp("TLS1.3", (const char *)Parameter_List[argn].String_Value))
            {
                *min_tls_prot = QAPI_NET_SSL_PROTOCOL_TLS_1_3;
            }
            else
            {
                SSL_PRINTF("ERROR: Invalid protocol: %s\n", Parameter_List[argn].String_Value);
                result = QCLI_STATUS_ERROR_E;
                goto end;
            }
          }
        }
        else if(0 == strncmp("extcfgkeyexchangegroup", (const char *) Parameter_List[argn].String_Value, strlen("extcfgkeyexchangegroup")))
        {
            qbool_t is_valid_cipher = true;
            argn++;
            if (key_ex_group == NULL || group_count == NULL || *group_count == 4) //Max support 4 group to be configured
            {
                SSL_PRINTF("ERROR: Too many group options %d (max %d)\r\n",(char*)Parameter_List[argn].String_Value, 4);
                result = QCLI_STATUS_ERROR_E;
                goto end;
            }
            if (0 == strcmp("GROUP_SECP256R1",(char*)Parameter_List[argn].String_Value))
            {
                key_ex_group[*group_count] = QAPI_NET_TLS13_KEY_EXCHANGE_GROUP_SECP256R1;
            }
            else if (0 == strcmp("GROUP_SECP384R1",(const char*)Parameter_List[argn].String_Value))
            {
                key_ex_group[*group_count] = QAPI_NET_TLS13_KEY_EXCHANGE_GROUP_SECP384R1;
            }
            else if (0 == strcmp("GROUP_SECP521R1",(const char*)Parameter_List[argn].String_Value))
            {
                key_ex_group[*group_count] = QAPI_NET_TLS13_KEY_EXCHANGE_GROUP_SECP521R1;
            }
            else if (0 == strcmp("GROUP_X25519",(const char*)Parameter_List[argn].String_Value))
            {
                key_ex_group[*group_count] = QAPI_NET_TLS13_KEY_EXCHANGE_GROUP_X25519;
            }
            else
            {
                SSL_PRINTF("Warning: Ignoring invalid group: %s\n", Parameter_List[argn].String_Value);
                is_valid_cipher = false;
            }

            if (is_valid_cipher)
                (*group_count)++;
        }
        else if(0 == strncmp("extcfgkeyexchangenegotiate", (const char *) Parameter_List[argn].String_Value, strlen("extcfgkeyexchangenegotiate")))
        {
          argn++;
          if(key_ex_negotiate)
          {
            if (0 == strcmp("1", (const char *)Parameter_List[argn].String_Value))
            {
                *key_ex_negotiate = 1;
            }
            else if (0 == strcmp("0", (const char *)Parameter_List[argn].String_Value))
            {
                *key_ex_negotiate = 0;
            }
            else
            {
                SSL_PRINTF("ERROR: Invalid input: %s\n", Parameter_List[argn].String_Value);
                result = QCLI_STATUS_ERROR_E;
                goto end;
            }
          }
        }
        else if(0 == strncmp("extcfgallowserverrpk", (const char *) Parameter_List[argn].String_Value, strlen("extcfgallowserverrpk")))
        {
          argn++;
          if(allow_server_rpk)
           *allow_server_rpk = atoi((char *)Parameter_List[argn].String_Value);
        }
        else if(0 == strncmp("extcfgalertwithepoch0", (const char *) Parameter_List[argn].String_Value, strlen("extcfgalertwithepoch0")))
        {
          argn++;
          if(alert_invalid_epoch)
           *alert_invalid_epoch = atoi((char *)Parameter_List[argn].String_Value);
        }
        else if(0 == strncmp("extcfgenforceextmaster", (const char *) Parameter_List[argn].String_Value, strlen("extcfgenforceextmaster")))
        {
          argn++;
          if(enforce_ext_master_secret)
           *enforce_ext_master_secret = atoi((char *)Parameter_List[argn].String_Value);
        }
        else if(0 == strncmp("extcfgenforcesecurereneg", (const char *) Parameter_List[argn].String_Value, strlen("extcfgenforcesecurereneg")))
        {
          argn++;
          if(enforce_secure_reneg)
           *enforce_secure_reneg = atoi((char *)Parameter_List[argn].String_Value);
        }
        else if(0 == strncmp("extcfgenablerenegextension", (const char *) Parameter_List[argn].String_Value, strlen("extcfgenablerenegextension")))
        {
          argn++;
          if(enable_reneg_extension)
           *enable_reneg_extension = atoi((char *)Parameter_List[argn].String_Value);
        }
        else if(0 == strncmp("extcfgenablenonsectls12", (const char *) Parameter_List[argn].String_Value, strlen("extcfgenablenonsectls12")))
        {
          argn++;
          if(enable_nonsectls12)
           *enable_nonsectls12 = atoi((char *)Parameter_List[argn].String_Value);
        }
    } /*for*/

end:
  if (Parameter_List != NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

QCLI_Command_Status_t ssl_parse_config_parameters(
        uint32_t Parameter_Count,
        QCLI_Parameter_t *Parameter_List,
        qapi_Net_SSL_Config_t *cfg,
        qbool_t server)
{

  return ssl_parse_config_parameters_extcfg(Parameter_Count, Parameter_List, cfg, server, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,NULL);
}

QCLI_Command_Status_t ssl_config(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    SSL_INST *ssl;
    int32_t res;
    qapi_Net_SSL_Protocol_t old_protocol;
    qapi_Net_SSL_Protocol_t new_protocol;
    qbool_t server = false;
    QCLI_Command_Status_t status;
    int i;
	char session_name[255];
	uint32_t session_lifetime=INVALID_UINT32;
	uint32_t connection_timeout =INVALID_UINT32;
    uint32_t disable_close_notify = INVALID_UINT32;
    uint8_t min_tls_prot = QAPI_NET_SSL_PROTOCOL_UNKNOWN;
    uint8_t key_ex_group[5] = {0}; //support config 4 group, and end with NULL
    uint8_t key_ex_negotiate = INVALID_UINT8;
    uint32_t allow_server_rpk = INVALID_UINT32;
    uint32_t group_count = 0;
    qapi_Net_SSL_Config_t tmp_cfg;
	uint8_t alert_invalid_epoch = INVALID_UINT8;
    uint8_t enforce_ext_master_secret = INVALID_UINT8;
	uint8_t enforce_secure_reneg = INVALID_UINT8;
	uint8_t enable_reneg_extension = INVALID_UINT8;
    uint8_t enable_nonsectls12 = INVALID_UINT8;

    memset(session_name, 0, 255);

    if(Parameter_Count < 1)
    {
        goto usage;
    }

    if (0 == strncmp((const char *)Parameter_List[0].String_Value, "server", 3))
    {
        ssl = &ssl_inst[app_id][SSL_SERVER_INST];
        server = true;
    }
    else if (0 == strncmp((const char *)Parameter_List[0].String_Value, "client", 3))
    {
        ssl = &ssl_inst[app_id][SSL_CLIENT_INST];
    }
    else
    {
        goto usage;
    }


    if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
    {
        SSL_PRINTF("ERROR: SSL %s not started\n", Parameter_List[0].String_Value);
        status = QCLI_STATUS_ERROR_E;
        goto end;
    }

    if (ssl->ssl == QAPI_NET_SSL_INVALID_HANDLE)
    {
        ssl->config_set = 0;
    }
    /* Parse SSL config parameters from command line */
    old_protocol = findproto(ssl->config.protocol);

    status = ssl_parse_config_parameters_extcfg(Parameter_Count-1, &Parameter_List[1], &tmp_cfg, server, session_name, &session_lifetime, &connection_timeout, &disable_close_notify, &min_tls_prot, (uint8_t *)key_ex_group, &group_count, &key_ex_negotiate, &allow_server_rpk, &alert_invalid_epoch, &enforce_ext_master_secret, &enforce_secure_reneg, &enable_reneg_extension,&enable_nonsectls12);
    if (status == QCLI_STATUS_USAGE_E)
    {
        goto usage;
    }
    else if (status != QCLI_STATUS_SUCCESS_E)
    {
        goto end;
    }

    /* Create SSL connection object */
    new_protocol = findproto(tmp_cfg.protocol);
    if (ssl->ssl == QAPI_NET_SSL_INVALID_HANDLE)
    {
        ssl->ssl = qapi_Net_SSL_Con_New(ssl->sslCtx, new_protocol);
        if (ssl->ssl == QAPI_NET_SSL_INVALID_HANDLE)
        {
            SSL_PRINTF("ERROR: SSL configure failed (Unable to create SSL context)\r\n");
            status = QCLI_STATUS_ERROR_E;
            goto end;
        }
    }
    else
    {
        if (new_protocol != old_protocol && tmp_cfg.protocol != QAPI_NET_SSL_PROTOCOL_UNKNOWN)
        {
            SSL_PRINTF("ERROR: Trying to switch between TLS to DTLS or vice versa!\r\n");
            SSL_PRINTF("ERROR: Stop current SSL session and start new session with new Protocol.\r\n");
            ssl->config.protocol = old_protocol;

            goto usage;
        }
    }

    /* configure the SSL connection */
    if(tmp_cfg.protocol != QAPI_NET_SSL_PROTOCOL_UNKNOWN)
    {
        memcpy(&ssl->config, &tmp_cfg, sizeof(qapi_Net_SSL_Config_t));
        ssl->config_set = 1;
        res = qapi_Net_SSL_Configure(ssl->ssl, &ssl->config);
        if (res < 0)
        {
            SSL_PRINTF("ERROR: SSL configure failed (%d)\n", res);
            status = QCLI_STATUS_ERROR_E;
            goto end;
        }
    }
    if(strlen(session_name) != 0)
    {
      res = qapi_Net_SSL_Set_Extended_Config_Option(ssl->ssl, QAPI_NET_SSL_EXTENDED_CONFIG_SESSION_NAME, session_name, strlen(session_name));
      if (res < 0)
      {
        SSL_PRINTF("ERROR: SSL ExtendConfig-SessionName configure failed (%d)\n", res);
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }

    }
    if(session_lifetime != INVALID_UINT32)
    {
      res = qapi_Net_SSL_Set_Extended_Config_Option(ssl->ssl, QAPI_NET_SSL_EXTENDED_CONFIG_SESSION_LIFETIME, &session_lifetime, 4);
      if (res < 0)
      {
        SSL_PRINTF("ERROR: SSL ExtendConfig-lifetime configure failed (%d)\n", res);
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }

    }
    if(connection_timeout != INVALID_UINT32)
    {
      res = qapi_Net_SSL_Set_Extended_Config_Option(ssl->ssl, QAPI_NET_SSL_EXTENDED_CONFIG_CONNECTION_TIMEOUT, &connection_timeout, 4);
      if (res < 0)
      {
        SSL_PRINTF("ERROR: SSL ExtendConfig-Connection Timeout configure failed (%d)\n", res);
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
    }
    if(disable_close_notify != INVALID_UINT32)
    {
      res = qapi_Net_SSL_Set_Extended_Config_Option(ssl->ssl, QAPI_NET_SSL_EXTENDED_CONFIG_DISABLE_CLOSE_NOTIFY, &disable_close_notify, 4);
      if (res < 0)
      {
        SSL_PRINTF("ERROR: SSL ExtendConfig-Connection disable close notify failed (%d)\n", res);
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
    }
    if(min_tls_prot != QAPI_NET_SSL_PROTOCOL_UNKNOWN)
    {
      res = qapi_Net_SSL_Set_Extended_Config_Option(ssl->ssl, QAPI_NET_SSL_EXTENDED_CONFIG_MIN_TLS_PROTOCOL, &min_tls_prot, 1);
      if (res < 0)
      {
        SSL_PRINTF("ERROR: SSL ExtendConfig-min TLS protocol failed (%d)\n", res);
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
    }
    if(group_count != 0)
    {
      res = qapi_Net_SSL_Set_Extended_Config_Option(ssl->ssl, QAPI_NET_SSL_EXTENDED_CONFIG_TLS13_KEY_EXCHANGE_GROUP, (void *)key_ex_group, group_count);
      if (res < 0)
      {
        SSL_PRINTF("ERROR: SSL ExtendConfig-TLS13 key exchange group failed (%d)\n", res);
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
    }
    if(key_ex_negotiate != INVALID_UINT8)
    {
      res = qapi_Net_SSL_Set_Extended_Config_Option(ssl->ssl, QAPI_NET_SSL_EXTENDED_CONFIG_TLS13_KEY_EXCHANGE_NEGOTIATE, &key_ex_negotiate, 1);
      if (res < 0)
      {
        SSL_PRINTF("ERROR: SSL ExtendConfig-TLS13 key exchange negotiate failed (%d)\n", res);
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
    }
    if(allow_server_rpk != INVALID_UINT32)
    {
      res = qapi_Net_SSL_Set_Extended_Config_Option(ssl->ssl, QAPI_NET_SSL_EXTENDED_CONFIG_ALLOW_SERVER_RPK, &allow_server_rpk, 4);
      if (res < 0)
      {
        SSL_PRINTF("ERROR: SSL ExtendConfig-Connection allow server rpk failed (%d)\n", res);
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
    }
    if(alert_invalid_epoch != INVALID_UINT8)
    {
      res = qapi_Net_SSL_Set_Extended_Config_Option(ssl->ssl, QAPI_NET_SSL_EXTENDED_CONFIG_DTLS_HANDLE_ALERT_WITH_EPOCH_0, &alert_invalid_epoch, 1);
      if (res < 0)
      {
        SSL_PRINTF("ERROR: SSL ExtendConfig- Config DTLS to handle alert with invalid epoch failed (%d)\n", res);
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
    }
    if(enforce_ext_master_secret != INVALID_UINT8)
    {
      res = qapi_Net_SSL_Set_Extended_Config_Option(ssl->ssl, QAPI_NET_SSL_EXTENDED_CONFIG_RFC_COMPATIBLE_EXT_MASTER_SECRET_FEATURE, &enforce_ext_master_secret, 1);
      if (res < 0)
      {
        SSL_PRINTF("ERROR: SSL ExtendConfig- enforce extended master secret failed (%d)\n", res);
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
    }

    if(enforce_secure_reneg != INVALID_UINT8)
    {
      res = qapi_Net_SSL_Set_Extended_Config_Option(ssl->ssl, QAPI_NET_SSL_EXTENDED_CONFIG_RFC_COMPATIBLE_SECURE_RENEGOTIATION_FEATURE, &enforce_secure_reneg, 1);
      if (res < 0)
      {
        SSL_PRINTF("ERROR: SSL ExtendConfig- Enforce server secure renegotiation failed (%d)\n", res);
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
    }
    if(enable_reneg_extension != INVALID_UINT8)
    {
      res = qapi_Net_SSL_Set_Extended_Config_Option(ssl->ssl, QAPI_NET_SSL_EXTENDED_CONFIG_ENABLE_RENEG_EXTENSION, &enable_reneg_extension, 1);
      if (res < 0)
      {
        SSL_PRINTF("ERROR: SSL ExtendConfig- Enable Renegotiation_Info extension failed (%d)\n", res);
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
    }
    if(enable_nonsectls12 != INVALID_UINT8)
	{
	  res = qapi_Net_SSL_Set_Extended_Config_Option(ssl->ssl, QAPI_NET_SSL_EXTENDED_CONFIG_ENABLE_NON_SECURE_TLS12_OPTIONS, &enable_nonsectls12, 1);
	  if (res < 0)
	  {
		 SSL_PRINTF("ERROR: SSL ExtendConfig- enable non sec tls12 (%d)\n", res);
		 status = QCLI_STATUS_ERROR_E;
		 goto end;
	  }
	}
    SSL_PRINTF("SSL %s configuration changed\n", Parameter_List[0].String_Value);

    status = QCLI_STATUS_SUCCESS_E;
    goto end;

usage:
    sslconfig_help_extcfg("\nssl config <client>");
    status = QCLI_STATUS_ERROR_E;
    goto end;

end:
  if (Parameter_List != NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return status;
}

QCLI_Command_Status_t ssl_add_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    SSL_INST *ssl;
    qapi_Net_SSL_Cert_Type_t type;
    char *name = NULL;
    int i;
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

    if(Parameter_Count < 2)
    {
usage:
          SSL_PRINTF("\nUsage: ssl cert <client> <cert|rpk_cert|ca_list|psk_table|di_cert> <name> \r\n"
                    " This adds a certificate or CA list to either SSL server or client. \r\n"
                    "      where <name> = name of file to load from FLASH. A default certificate or CA list will be added if <name> is omitted.\r\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    if (0 == strcmp((char*)Parameter_List[0].String_Value, "server"))
    {
        ssl = &ssl_inst[app_id][SSL_SERVER_INST];
        if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
        {
            SSL_PRINTF("ERROR: SSL server not started\r\n");
            result = QCLI_STATUS_ERROR_E;
            goto end;
        }
    }
    else if (0 == strcmp((char*)Parameter_List[0].String_Value, "client"))
    {
        ssl = &ssl_inst[app_id][SSL_CLIENT_INST];
        if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
        {
            SSL_PRINTF("ERROR: SSL client not started\r\n");
            result = QCLI_STATUS_ERROR_E;
            goto end;
        }
    }
    else
    {
        goto usage;
    }


    if (0 == strcmp("cert", (char *)Parameter_List[1].String_Value))
    {
        type = QAPI_NET_SSL_CERTIFICATE_E;
    }
    else if (0 == strcmp("rpk_cert", (char *)Parameter_List[1].String_Value))
    {
        type = QAPI_NET_SSL_RPK_CERT_E;
    }
    else if (0 == strcmp("ca_list", (char *)Parameter_List[1].String_Value))
    {
        type = QAPI_NET_SSL_CA_LIST_E;
    }
    else if (0 == strcmp("psk_table", (char *)Parameter_List[1].String_Value))
    {
        type = QAPI_NET_SSL_PSK_TABLE_E;
    }
    else if (0 == strcmp("di_cert", (char *)Parameter_List[1].String_Value))
    {
        type = QAPI_NET_SSL_DI_CERT_E;
    }
    else
    {
        goto usage;
    }

    if (Parameter_Count > 2)
    {
        name = (char *)Parameter_List[2].String_Value;
    }

    // Load/add certificate
    if (name != NULL)
    {
        if (type == QAPI_NET_SSL_CERTIFICATE_E || type == QAPI_NET_SSL_RPK_CERT_E || type == QAPI_NET_SSL_CA_LIST_E || type == QAPI_NET_SSL_PSK_TABLE_E)
        {
          if (qapi_Net_SSL_Cert_Load(ssl->sslCtx, type, name) < 0)
          {
              SSL_PRINTF("ERROR: Unable to load %s from FLASH\r\n" , name);
              result = QCLI_STATUS_ERROR_E;
              goto end;
          }
        }
        else if (type == QAPI_NET_SSL_DI_CERT_E )
        {
          qapi_Net_SSL_Identifier_t ref_Identifier;
          if (qapi_Net_SSL_Cert_Load_Get_Identifier(ssl->sslCtx, &ref_Identifier, name) < 0)
          {
              SSL_PRINTF("ERROR: Unable to load %s from FLASH\r\n" , name);
              result = QCLI_STATUS_ERROR_E;
              goto end;
          }
          SSL_PRINTF("DI Cert: Identifier Type:%d and Name:%s\r\n" , ref_Identifier.identifier_Type, ref_Identifier.identifier_Name);
        }
        SSL_PRINTF("%s loaded from FLASH\r\n", name);
    }
    else
    {
        goto usage;
    }

end:
  if (Parameter_List != NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}


#endif // INCLUDE_SSLDEMO
