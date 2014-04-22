/*!
  @file
  transport_selection.c

  @brief
  Module to perform the transport binding selection. 
*/

/*===========================================================================

  Copyright (c) 2008-2021 Qualcomm Technologies, Inc. All Rights Reserved

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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/12/18  vpulimi  Added support for transport mode selection 
===========================================================================*/


#include <stdio.h>
#include <stdlib.h>

#include "lwm2mclient.h"
#include "internals.h"
#include "liblwm2m.h"
#include "object_server.h"
#include "iotapp_qmi_internal.h"
#include "object_apn_connection.h"
#include "ds_log.h"



/* Initial look-up table 
 *
 * By default, we assume UDP transport is always present even though the provider configures "Binding"
 * resource to transports other than UDP. Forcefully, perform the bit-wise or operation to ensure that 
 * we have the UDP transport binding set by default always. 
 *
 * If we are not able to find the appropriate transport based on the input parameters we would always 
 * default to UDP transport. 
 *
 */ 
#define TRANSPORT_LOOKUP_TABLE_1_ATTRS   0x03 
/* Bindings */ 
#define TRANSPORT_LOOKUP_TABLE_1_ATTR_0  0x00
/* Preferred Transport */
#define TRANSPORT_LOOKUP_TABLE_1_ATTR_1  0x01
/* Transport mode(selected) */ 
#define TRANSPORT_LOOKUP_TABLE_1_ATTR_2  0x02

static uint32_t transport_lookup_table1[ ][TRANSPORT_LOOKUP_TABLE_1_ATTRS] = 
{
  {UDP_NONIP_TRANSPORT,  INVALID_TRANSPORT,   UDP_TRANSPORT}, 
  {UDP_NONIP_TRANSPORT,  UDP_TRANSPORT,       UDP_TRANSPORT},
  {UDP_NONIP_TRANSPORT,  NONIP_TRANSPORT,     NONIP_TRANSPORT},  
  {NONIP_TRANSPORT,      INVALID_TRANSPORT,   NONIP_TRANSPORT},  
  {NONIP_TRANSPORT,      NONIP_TRANSPORT,     NONIP_TRANSPORT},  
  {NONIP_TRANSPORT,      UDP_TRANSPORT,       UDP_TRANSPORT},    
  {UDP_TRANSPORT,        INVALID_TRANSPORT,   UDP_TRANSPORT},    
  {UDP_TRANSPORT,        UDP_TRANSPORT,       UDP_TRANSPORT},
  {UDP_TRANSPORT,        NONIP_TRANSPORT,     UDP_TRANSPORT}
};


/* Secondary look-up table (Registration Update Trigger) 
 *
 * Registration update trigger without parameters use only look-up table1. In case, there are parameters 
 * use the following table below. If we are not able to find appropriate transport from secondary table 
 * rely on the primary table to retrieve the transport. Preferred transport is not used in finding the 
 * transport, as the register update parameter dictates the tranport binding to choose.
 *
 * All the conditions indicated in inital look-up table apply here as well.
 *
 */ 
#define TRANSPORT_LOOKUP_TABLE_2_ATTRS   0x03 
/* Register Update Trigger Param */ 
#define TRANSPORT_LOOKUP_TABLE_2_ATTR_0  0x00
/* Bindings */ 
#define TRANSPORT_LOOKUP_TABLE_2_ATTR_1  0x01
/* Transport mode(selected) */ 
#define TRANSPORT_LOOKUP_TABLE_2_ATTR_2  0x02

static uint32_t transport_lookup_table2[ ][TRANSPORT_LOOKUP_TABLE_2_ATTRS] = 
{
  /* Preferred transport is not considered as part of the selection (dummy so ignoring it) */ 
  {UDP_TRANSPORT,    UDP_NONIP_TRANSPORT,  UDP_TRANSPORT},
  {UDP_TRANSPORT,    NONIP_TRANSPORT,      UDP_TRANSPORT},     
  {UDP_TRANSPORT,    UDP_TRANSPORT,        UDP_TRANSPORT},                           
  {NONIP_TRANSPORT,  UDP_NONIP_TRANSPORT,  NONIP_TRANSPORT},
  {NONIP_TRANSPORT,  NONIP_TRANSPORT,      NONIP_TRANSPORT},   
  {NONIP_TRANSPORT,  NONIP_TRANSPORT,      NONIP_TRANSPORT},
};


/* Teritiary look-up table (PDN Type) 
 *
 * Trasnsport mode selected from look-up table1 and table2 is used as input to this table. PDN type 
 * dictates the transport that we would be selecting to bring-up the data call.  
 *
 * If the PDN type is not valid then we need not use this look-up table and the transport obtained 
 * from the prior look-ups can be used. 
 *
 * Also if the PDN supports both IP and NON-IP then the transport obtained from the prior look-ups 
 * can be used and this look-up is skipped. 
 *
 */ 
#define TRANSPORT_LOOKUP_TABLE_3_ATTRS   0x04 
/* Bindings */ 
#define TRANSPORT_LOOKUP_TABLE_3_ATTR_0  0x00
/* Transport mode (selected from either lookup tables 1 or 2) */
#define TRANSPORT_LOOKUP_TABLE_3_ATTR_1  0x01
/* PDN Type */ 
#define TRANSPORT_LOOKUP_TABLE_3_ATTR_2  0x02
/* Transport mode(selected) */ 
#define TRANSPORT_LOOKUP_TABLE_3_ATTR_3  0x03

static uint32_t transport_lookup_table3[ ][TRANSPORT_LOOKUP_TABLE_3_ATTRS] = 
{
  {UDP_NONIP_TRANSPORT,  UDP_TRANSPORT,    IP_PDN_TYPE,     UDP_TRANSPORT},
  {UDP_NONIP_TRANSPORT,  UDP_TRANSPORT,    NONIP_PDN_TYPE,  NONIP_TRANSPORT},
  {UDP_NONIP_TRANSPORT,  NONIP_TRANSPORT,  IP_PDN_TYPE,     UDP_TRANSPORT},
  {UDP_NONIP_TRANSPORT,  NONIP_TRANSPORT,  NONIP_PDN_TYPE,  NONIP_TRANSPORT},    
  {NONIP_TRANSPORT,      NONIP_TRANSPORT,  IP_PDN_TYPE,     UDP_TRANSPORT},    
  {NONIP_TRANSPORT,      NONIP_TRANSPORT,  NONIP_PDN_TYPE,  NONIP_TRANSPORT},  
  {NONIP_TRANSPORT,      UDP_TRANSPORT,    IP_PDN_TYPE,     UDP_TRANSPORT},    
  {NONIP_TRANSPORT,      UDP_TRANSPORT,    NONIP_PDN_TYPE,  NONIP_TRANSPORT},  
  {UDP_TRANSPORT,        UDP_TRANSPORT,    IP_PDN_TYPE,     UDP_TRANSPORT},  
  {UDP_TRANSPORT,        UDP_TRANSPORT,    NONIP_PDN_TYPE,  UDP_TRANSPORT},    /* Error case - data call would never come up */ 
};

                                           
bool data_call_conn_profile_change(uint16_t short_server_id)
{
  uint8_t pdn_type = INVALID_TRANSPORT; 
  uint8_t pref_transport = INVALID_TRANSPORT;  
  uint8_t bindings = INVALID_TRANSPORT; 
  uint8_t curr_transport_binding = INVALID_TRANSPORT;
  uint8_t prev_transport_binding = INVALID_TRANSPORT; 
  server_instance_t *server_inst = NULL;
  lwm2m_object_t *server_list = NULL; 
  carrier_apn_cfg_t *apn_info = NULL;  
  apn_conn_input_params_t conn_params;  
  lwm2m_server_t *server = NULL; 
  uint8_t reg_trigger_param = INVALID_TRANSPORT; 
  lwm2m_context_t * contextP = get_lwm2m_context();

  /* Skip the bootstrap server, as there is no APN profile associated with it */
  if (short_server_id == bs_ssid)
  {
	ds_iot_ulog_high("BS doesn't have any APN profile - no profile change");
	return false;
  }

  /* Server that existed is no longer present */ 
  server = (lwm2m_server_t *)lwm2m_list_find((lwm2m_list_t *)contextP->serverList, short_server_id);
  if (NULL == server)
  {
	ds_iot_ulog_high1("SSID[%d] that existed before with is no longer present - profile changed", short_server_id);
    return true;
  }

  /* APN profile exists only for LwM2Mv1.1 clients or higher - Skip profile change validation for LwM2Mv1.0 clients */ 
  if ((float)LWM2M_V_1_0 == server->version)
  {
    ds_iot_ulog_high("LwM2Mv1.0 client doesn't support APN profile - no profile change");
    return false;
  }    

  /* Retrieve the server instance related inforamtion */
  server_list = (lwm2m_object_t *)LWM2M_LIST_FIND(data.lwm2mH->objectList, LWM2M_SERVER_OBJECT_ID);
  if (!server_list)
  {
	ds_iot_ulog_high("Server instance list that existed not present - profile changed");
    return true;
  }
  
  /* Traverse the server instance list and populate the APN information required */
  server_inst = (server_instance_t *)server_list->instanceList;
  while (server_inst)
  {
    if (short_server_id == server_inst->shortServerId)
      break;

    server_inst = server_inst->next; 
  }

  /* Server instance not found */
  if (NULL == server_inst)
  {
	ds_iot_ulog_high("Server instance list that existed not present - profile changed");
	return true;
  }

  /* Retrieve APN information associated with the server short ID (carrier_apn config) */
  apn_info = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(data.lwm2mH->apn_cfg, short_server_id);
  if (NULL == apn_info)
  {
	ds_iot_ulog_high("APN info that existed in APN config not present - profile changed");
    return true;
  } 

  /* APN link changed */ 
  if (apn_info->apn_link != server_inst->apnLink)
  {
    ds_iot_ulog_high2("APN links (0x%x) -> (0x%x) differ - profile changed", apn_info->apn_link, server_inst->apnLink);
    return true;
  } 

  /* Service type changed */
  if (apn_info->srvc_type.lte_mode != srvc_type.lte_mode)
  {
    ds_iot_ulog_high2("Service types (0x%x) -> (0x%x) differ - profile changed", apn_info->srvc_type.lte_mode, srvc_type.lte_mode);
    return true; 
  }

  /* Initialize APN connection information */
  memset(&conn_params, 0x00, sizeof(apn_conn_input_params_t));      
  conn_params.pdn_type  = 0xFF;   
                                   
  /* Retrieve the APN connection object information associated with the APN link */        
  get_apn_conn_params(server_inst->apnLink, &conn_params);
  
  /* Release the resources associated with the re-connection schedule, as it is not used for decision-making 
   * at this point. When there is a transport change, the new re-connection schedule parameters take effect.  
   */
  if ((conn_params.params_set & RECONNECT_SET) &&
      (conn_params.reconn_count))
    free(conn_params.reconn_schedule);
  
  /* APN info
   *    1. APN information is configured in the factory provisioned file only 
   *    2. APN information is configured in the APN Connection Object-11 (overrides above)
   *    3. APN information is dynamically updated in the APN Connection Object-11
   *    4. APN information can be identical or differ in the factory provisioned config and APN Connection Object-11
   */
  if ((conn_params.params_set & APN_NAME_SET) &&
      (!apn_info->apn_name_obj_11) &&
      (strncmp(apn_info->apn_name, conn_params.apn_name, CARRIER_APN_LEN)))
  {
    /* APN name configured in factory provisioned config was used to bring-up the data call. Currently, the server has updated
     * the APN name in Object-11. Compare the cached APN name with the one configured in Object-11 and if they differ indicate 
     * that the profile has changed.
     */
    ds_iot_log_strf_high("APN name (%s) in Object-11 differs from factory config (%s) - profile changed", conn_params.apn_name, apn_info->apn_name);  
    return true; 
  }
//  else if ((!(conn_params.params_set & APN_NAME_SET)) &&
//           (apn_info->apn_name_obj_11))
//  {
    /* APN name configured in Object-11 was used to bring-up the data call. Currently, the server has cleared the APN name in 
     * Object-11. Continue using the configured APN name until reboot, reg trigger, re-bootstrapping, factory reset or OOS. 
     */

//  }
  else if ((conn_params.params_set & APN_NAME_SET) &&
           (apn_info->apn_name_obj_11) &&
           (strncmp(apn_info->apn_name, conn_params.apn_name, CARRIER_APN_LEN)))
  {
    /* APN name configured in Object-11 was used to bring-up the data call. Currently, APN name in object-11 is still configured. 
     * Compare the cached APN name with the one configured in Object-11 and if they differ indicate that the profile has changed. 
     */
    ds_iot_log_strf_high("APN names (%s) -> (%s) differ - profile changed", apn_info->apn_name, conn_params.apn_name);
    return true;
  }

  /* Authentication info 
   *    1. AUTH information is configured in the APN Connection Object-11 
   *    2. AUTH information is dynamically updated in the APN Connection Object-11
   */
  if ((conn_params.params_set & AUTH_TYPE_SET) && 
      (!apn_info->auth_type_obj_11))
  {
    /* AUTH info was not configured in Object-11 when the data call was brought-up. Currently, the server has configured the AUTH 
     * info in Object-11. Indicate that the profile has changed and use the updated AUTH information to bring-up the data call.  
     */
    ds_iot_ulog_high1("Auth type (0x%x) is configured in Object-11 - profile changed", conn_params.auth_type);                 
    return true;
  }
//  else if ((!(conn_params.params_set & AUTH_TYPE_SET)) && 
//           (apn_info->auth_type_obj_11))
//  {
    /* AUTH info configured in Object-11 was used to bring-up the data call. Currently, the server has cleared the AUTH info
     * in Object-11. Continue using the configured AUTH info until reboot, reg trigger, re-bootstrapping, factory reset or OOS. 
     */

//  }
  else if ((conn_params.params_set & AUTH_TYPE_SET) && 
           (apn_info->auth_type_obj_11) && 
           (apn_info->auth_type != conn_params.auth_type))
  {  
    /* AUTH info configured in Object-11 was used to bring-up the data call. Currently, the server has still configured the AUTH 
     * info in object-11. Compare the cached AUTH info with the one configured in Object-11 and if they differ indicate that the 
     * profile has changed. 
     */
    ds_iot_ulog_high2("Auth types (0x%x) -> (0x%x) differ - profile changed", apn_info->auth_type, conn_params.auth_type);                 
    return true;
  }

  /* Username info 
   *    1. USER information is configured in the APN Connection Object-11 
   *    2. USER information is dynamically updated in the APN Connection Object-11
   */
  if ((conn_params.params_set & USER_NAME_SET) && 
      (!apn_info->user_name_obj_11))
  {
    /* USER info was not configured in Object-11 when the data call was brought-up. Currently, the server has configured the USER 
     * info in Object-11. Indicate that the profile has changed and use the updated USER information to bring-up the data call.  
     */
    ds_iot_log_strf_high("Username (%s) is configured in Object-11 - profile changed", conn_params.username);                 
    return true;
  }
//  else if ((!(conn_params.params_set & USER_NAME_SET)) && 
//           (apn_info->user_name_obj_11))
//  {
    /* USER info configured in Object-11 was used to bring-up the data call. Currently, the server has cleared the USER info
     * in Object-11. Continue using the configured USER info until reboot, reg trigger, re-bootstrapping, factory reset or OOS. 
     */

//  }
  else if ((conn_params.params_set & USER_NAME_SET) && 
           (apn_info->user_name_obj_11) && 
           (strncmp(apn_info->user_name, conn_params.username, USER_NAME_MAX_LEN)))
  {  
    /* USER info configured in Object-11 was used to bring-up the data call. Currently, the server has still configured the USER
     * info in object-11. Compare the cached USER info with the one configured in Object-11 and if they differ indicate that the 
     * profile has changed. 
     */
    ds_iot_log_strf_high("Usernames (%s) -> (%s) differ - profile changed", apn_info->user_name, conn_params.username);                 
    return true;
  }

  /* Password info 
   *    1. PASSWORD information is configured in the APN Connection Object-11 
   *    2. PASSWORD information is dynamically updated in the APN Connection Object-11
   */
  if ((conn_params.params_set & SECRET_SET) && 
      (!apn_info->password_obj_11))
  {
    /* PASSWORD info was not configured in Object-11 when the data call was brought-up. Currently, the server has configured the  
     * PASSWORD info in Object-11. Indicate that profile has changed and use updated PASSWORD information to bring-up data call.  
     */
    ds_iot_log_strf_high("Password (%s) is configured in Object-11 - profile changed", conn_params.secret);                 
    return true;
  }
//  else if ((!(conn_params.params_set & SECRET_SET)) && 
//           (apn_info->password_obj_11))
//  {
    /* PASSWORD info configured in Object-11 was used to bring-up the data call. Currently, the server has cleared the PASSWORD 
     * info in Object-11. Continue using configured USER info until reboot, reg trigger, re-bootstrapping, factory reset or OOS. 
     */

//  }
  else if ((conn_params.params_set & SECRET_SET) && 
           (apn_info->password_obj_11) && 
           (strncmp(apn_info->password, conn_params.secret, PASSWORD_MAX_LEN)))
  {  
    /* PASSWORD info configured in Object-11 was used to bring-up the data call. Currently, the server has still configured the 
     * PASSWORD info in object-11. Compare the cached PASSWORD info with the one configured in Object-11 and if they differ then
     * indicate that the profile has changed. 
     */
    ds_iot_log_strf_high("Usernames (%s) -> (%s) differ - profile changed", apn_info->user_name, conn_params.username);                 
    return true;
  }

  /* PDN info
   *    1. PDN information is configured in the factory provisioned file only 
   *    2. PDN information is configured in the APN Connection Object-11 (overrides above)
   *    3. PDN information is dynamically updated in the APN Connection Object-11
   *    4. PDN information can be identical or differ in the factory provisioned config and APN Connection Object-11
   */
  if ((conn_params.params_set & PDN_TYPE_SET) &&
      (!apn_info->pdn_type_obj_11) &&
      (apn_info->pdn_type != conn_params.pdn_type))
  {
    /* PDN info configured in factory provisioned config was used to bring-up the data call. Currently, the server has updated
     * the PDN info in Object-11. Compare the cached PDN info with the one configured in Object-11 and if they differ indicate 
     * that the profile has changed.
     * 
     * PDN type in object-11 doesn't support both NON_IP and IP PDN types. However, factory provisioned config allows support 
     * for both PDN types simultaneously. In APN connection config creation, we mark the PDN type as 0xFF for such a scenario
     * and take care of it here in the profile change detection. For design simplicity, indicate profile change and update with
     * the current factory config with the Object-11 config.   
     */
    ds_iot_ulog_high2("PDN type (0x%x) in Object-11 differs from factory config (0x%x) - profile changed", conn_params.pdn_type, apn_info->pdn_type);
    return true; 
  }
  else if (((!(conn_params.params_set & PDN_TYPE_SET)) &&
            (apn_info->pdn_type_obj_11)) ||
           (0xFFFFFFFF == apn_info->apn_link))
  {
    /* PDN info configured in Object-11 was used to bring-up the data call. Currently, the server has cleared the PDN info in 
     * Object-11. Continue using the configured PDN info until reboot, reg update trigger, re-bootstrapping, factory reset or OOS.
     * 
     * Use the PDN type configured for transport selection algorithm
     */
    if (0xFF == apn_info->pdn_type)
      pdn_type = NONIP_TRANSPORT | UDP_TRANSPORT;
    else if (AF_NONIP == apn_info->pdn_type)
      pdn_type = NONIP_TRANSPORT;
    else  
      pdn_type = UDP_TRANSPORT;
  }
  else if ((conn_params.params_set & PDN_TYPE_SET) &&
           (apn_info->pdn_type_obj_11) &&
           (apn_info->pdn_type != conn_params.pdn_type))
  {
    /* PDN info configured in Object-11 was used to bring-up the data call. Currently, PDN info in object-11 is still configured. 
     * Compare the cached PDN info with the one configured in Object-11 and if they differ indicate that the profile has changed. 
     */
    ds_iot_ulog_high2("PDN types (%d) -> (%d) differ - profile changed", apn_info->pdn_type, conn_params.pdn_type);
    return true;
  }
  else if (conn_params.params_set & PDN_TYPE_SET)
  {
    /* Use the PDN type configured in the Object-11 for transport selection algorithm */ 
    pdn_type = (AF_NONIP == conn_params.pdn_type) ? NONIP_TRANSPORT : UDP_TRANSPORT;  
  }
  
  /* Bindings */ 
  bindings = utils_stringToBinding((uint8_t *)server_inst->binding, strlen(server_inst->binding));
  /* Skip transports other than U and N */
  bindings &= (UDP_NONIP_TRANSPORT);
  if (!bindings)
    bindings = UDP_TRANSPORT; 
   
  /* Preferred mode of transport (optional) */ 
  if (strlen(server_inst->prefTransport)) 
  {
    pref_transport = utils_stringToBinding((uint8_t *)server_inst->prefTransport, strlen(server_inst->prefTransport));
    /* Skip transports other than U and N */
    pref_transport &= (UDP_NONIP_TRANSPORT);
    if (!pref_transport)
      pref_transport = INVALID_TRANSPORT; 
  }  
      
  /* Register update trigger param */ 
  if ((server_inst->regTrigger) && (strlen(server_inst->regTriggerParam)))
  {
    reg_trigger_param = utils_stringToBinding((uint8_t *)server_inst->regTriggerParam, strlen(server_inst->regTriggerParam)); 
    if (reg_trigger_param)
    {
      /* Skip transports other than U and N. If the register trigger transport param is invalid, ignore the parameter.
       * Invalid transport param in register update trigger would result in using look-up tables 1 and 3 (not table 2).
       */
      reg_trigger_param &= (UDP_NONIP_TRANSPORT);
      if (!reg_trigger_param)
        reg_trigger_param = INVALID_TRANSPORT; 
    }
  }  

  ds_iot_ulog_high4("Bindings (0x%x), Pref transport (0x%x), PDN type (0x%x), Reg param (0x%x)", bindings, pref_transport, pdn_type, reg_trigger_param);

  /* Transport binding changed */ 
  curr_transport_binding = lwm2m_transport_selection(bindings, pref_transport, pdn_type, server_inst->regTrigger, reg_trigger_param);
  prev_transport_binding = (AF_NONIP == apn_info->transport_binding) ? NONIP_TRANSPORT : UDP_TRANSPORT; 
  if (curr_transport_binding != prev_transport_binding)
  {
    ds_iot_ulog_high2("Transport binding (0x%x) -> (0x%x) - profile changed", prev_transport_binding, curr_transport_binding);
    return true;
  }
  else 
  {
    ds_iot_ulog_high2("No change in transport bindings (0x%x) -> (0x%x)", prev_transport_binding, curr_transport_binding);    
  } 

  return false;
}


uint32_t lwm2m_transport_selection
(                                                                  
  uint32_t bindings, 
  uint32_t preferred_mode, 
  uint32_t pdn_type, 
  bool     reg_trigger, 
  uint32_t reg_trigger_param
)
{
  uint32_t cntr = 0x00;
  uint32_t entries = 0x00; 
  uint32_t transport_mode = INVALID_TRANSPORT;
  
  ds_iot_ulog_high5("Transport selection algo - Bindings (0x%x), Pref transport (0x%x), PDN type (0x%x), Reg trigger (0x%x), Reg param (0x%x)", 
                                                                                                                      bindings, 
                                                                                                                      preferred_mode, 
                                                                                                                      pdn_type, 
                                                                                                                      reg_trigger, 
                                                                                                                      reg_trigger_param);
  
  
  /* Check the mode of operation - M1 or NB1 
   * NB-1 - UDP transport and Non-IP transport are supported
   *  M-1 - UDP transport is only supported   
   */
  if (IOTAPP_CIOT_MODE_NB1 != srvc_type.ciot_mode)
  {
	ds_iot_ulog_high("Service detected is M-1, so selected transport mode is UDP");	
    return UDP_TRANSPORT;
  }  

  /* Registration trigger is set and has associated param - use the look-up table-2 */
  if ((reg_trigger) && 
      (reg_trigger_param))  
  {
    entries = sizeof(transport_lookup_table2)/(sizeof(uint32_t) * TRANSPORT_LOOKUP_TABLE_2_ATTRS); 

    /* Look-up table 2 */ 
    for (cntr = 0; cntr < entries; cntr++)
    {
      /* Retrieve the matching entry based on the register update trigger param and bindings */ 
      if ((reg_trigger_param == transport_lookup_table2[cntr][TRANSPORT_LOOKUP_TABLE_2_ATTR_0]) &&
          (bindings == transport_lookup_table2[cntr][TRANSPORT_LOOKUP_TABLE_2_ATTR_1]))
      {
        transport_mode = transport_lookup_table2[cntr][TRANSPORT_LOOKUP_TABLE_2_ATTR_2];
        
        ds_iot_ulog_high1("Transport mode selected from lookup table - 2[0x%x]", transport_mode);   
        break;
      }
    }
  }
  
  /* Transport is invalid indicates that the register trigger param wasn't set - use the look-up table-1 */
  if (INVALID_TRANSPORT == transport_mode)
  {
    entries = sizeof(transport_lookup_table1)/(sizeof(uint32_t) * TRANSPORT_LOOKUP_TABLE_1_ATTRS); 

    /* Look-up table 1 */ 
    for (cntr = 0; cntr < entries; cntr++)
    {
      /* Retrieve the matching entry based on the bindings and preferred transport */ 
      if ((bindings == transport_lookup_table1[cntr][TRANSPORT_LOOKUP_TABLE_1_ATTR_0]) &&
          (preferred_mode == transport_lookup_table1[cntr][TRANSPORT_LOOKUP_TABLE_1_ATTR_1]))
      {
        transport_mode = transport_lookup_table1[cntr][TRANSPORT_LOOKUP_TABLE_2_ATTR_2];
        
        ds_iot_ulog_high1("Transport mode selected from lookup table - 1[0x%x]", transport_mode);   
        break;
      } 
    }
  }

  /* PDN type is set to both IP and NON IP then no need to perform further look-up in table 3. We can just
   * return the transport selected from either table-1 or table-2. 
   */
  if (UDP_NONIP_TRANSPORT == pdn_type)
  {
	ds_iot_ulog_high1("PDN type supports both IP and NON_IP (no need any further look-up).Selected transport mode[0x%x]", transport_mode); 
    return transport_mode; 
  }

  /* Make the final decision on the transport selection based on the PDN type - use look-up table-3 */ 
  entries = sizeof(transport_lookup_table3)/(sizeof(uint32_t) * TRANSPORT_LOOKUP_TABLE_3_ATTRS); 
  for (cntr = 0; cntr < entries; cntr++)
  {
    /* Retrieve the matching entry based on the bindings, selected transport and PDN type */ 
    if ((bindings == transport_lookup_table3[cntr][TRANSPORT_LOOKUP_TABLE_3_ATTR_0]) &&
        (transport_mode == transport_lookup_table3[cntr][TRANSPORT_LOOKUP_TABLE_3_ATTR_1]) &&
        (pdn_type == transport_lookup_table3[cntr][TRANSPORT_LOOKUP_TABLE_3_ATTR_2]))
    {
      transport_mode = transport_lookup_table3[cntr][TRANSPORT_LOOKUP_TABLE_3_ATTR_3];
  
      ds_iot_ulog_high1("Transport mode selected from lookup table - 3[0x%x]", transport_mode);
      break;
    }   
  }

  return transport_mode;
}

void lwm2m_free_apn_list(carrier_apn_cfg_t *apn_list)
{
  carrier_apn_cfg_t *apn_cfg = apn_list;
  if(NULL == apn_cfg)
  {
    ds_iot_ulog_high("APN list is NULL, no need to free");
    return;
  }

  while(apn_cfg)
  {
    if(apn_cfg->retry_timeout_lst)
    {
      free(apn_cfg->retry_timeout_lst);
      apn_cfg->retry_timeout_lst = NULL;
    }

    apn_cfg = apn_cfg->next;
  }
  LWM2M_LIST_FREE(apn_list);
  return;
}

carrier_apn_cfg_t *update_apn_conn_cfg
(
  lwm2m_context_t *lwm2mH,
  uint16_t *short_serverID
)
{
  server_instance_t    *server_inst = NULL;
  lwm2m_object_t       *server_list = NULL;
  carrier_apn_cfg_t *server_apn_lst = NULL;
  carrier_apn_cfg_t     *s_apn_info = NULL;
  void        *bs_match_v4_nip_hndl = NULL;
  void            *bs_match_v6_hndl = NULL;

  if (NULL == lwm2mH)
  {
    ds_iot_ulog_err("Invalid input parameters - lwm2mH");
    return NULL;
  }

  /* Also when the device is in bootstrap phase don't parse the server instances, as they might get overwritten. Wait 
   * till the registration phase starts. Just return the bootstrap APN config (only one bootstrap server is supported). 
   */
  if ((STATE_BOOTSTRAP_REQUIRED == lwm2m_get_client_state()) ||
      (STATE_BOOTSTRAPPING == lwm2m_get_client_state()))
  {
    bool default_apn_populated = false;
    carrier_apn_cfg_t *apn_cfg = lwm2mH->c_apn_cfg;

    /* Associated APN information pertaining to the short server ID is found. In that case, go ahead
     * and re-use the existing carrier_apn config. 
     *
     * If not present, that means the server configuration  was not present at the time of boot. Later,
     * when the bootstrapping was performed the configuration was written.      
     */
    if (NULL == apn_cfg)
    {
      /* Generate default APN information on-the-fly for the bootstrap server */
      if (NULL == (apn_cfg = populate_default_apn_info(bs_ssid, true)))
        return NULL;

      default_apn_populated = true;
    } 
  
    while (apn_cfg)
    {
      /* Bootstrap APN */
      if (apn_cfg->bootstrap_apn)
      {
        carrier_apn_cfg_t *s_apn_info = NULL;

        /* Allocate memory for storing the server APN information */
        s_apn_info = (carrier_apn_cfg_t *)malloc(sizeof(carrier_apn_cfg_t));
        if (NULL == s_apn_info)
          return NULL;

        memset(s_apn_info, 0x00, sizeof(carrier_apn_cfg_t));

        memcpy(s_apn_info, apn_cfg, sizeof(carrier_apn_cfg_t));
        s_apn_info->next = NULL;

        s_apn_info->retry_timeout_lst_len = apn_cfg->retry_timeout_lst_len;
        if (s_apn_info->retry_timeout_lst_len)
        {
          s_apn_info->retry_timeout_lst = malloc(sizeof(uint32_t) * apn_cfg->retry_timeout_lst_len);
          if (NULL == s_apn_info->retry_timeout_lst)
          { 
            free(s_apn_info);
            s_apn_info = NULL;
            return NULL;
          }

          memcpy(s_apn_info->retry_timeout_lst, apn_cfg->retry_timeout_lst, (sizeof(uint32_t) * apn_cfg->retry_timeout_lst_len));
        }

        /* Check the mode of operation - M1 or NB1
         * NB-1 - UDP transport and Non-IP transport are supported
         *  M-1 - UDP transport is only supported
         */
        if (IOTAPP_CIOT_MODE_NB1 == srvc_type.ciot_mode)
        {
          /* IP is given preference over Non-IP (gets overridden if IP support is enabled) */
          if (s_apn_info->non_ip_support)
            s_apn_info->transport_binding = AF_NONIP; 

          if (s_apn_info->ip_support)
            s_apn_info->transport_binding = s_apn_info->ip_family;
        }
        else
        { 
          s_apn_info->transport_binding = s_apn_info->ip_family;
        }

        ds_iot_ulog_high2("Transport mode for BS[0x%x] with curr service type[0x%x]", s_apn_info->transport_binding, srvc_type.ciot_mode);
 
        /* Release any resources allocated for populating the default APN configuration */
        if ((default_apn_populated) && (apn_cfg))
        {
          lwm2m_free(apn_cfg);
          apn_cfg = NULL;
        }

        if(lwm2mH->apn_cfg != NULL)
          lwm2m_free_apn_list(lwm2mH->apn_cfg);

        lwm2mH->apn_cfg = s_apn_info;
        return lwm2mH->apn_cfg;
      }

      apn_cfg = apn_cfg->next;
    }

    return NULL;
  }
  
  /* Clean-up the server instance and rebuild based on the updated server configuration */
  if ((NULL != lwm2mH->apn_cfg) &&
      (NULL == short_serverID))
  {
    carrier_apn_cfg_t *apn_lst = lwm2mH->apn_cfg;

    /* Clean-up the allocated resources */ 
    while (apn_lst)
    {
      /* Bootstrap and registration server(s) have matching APNs */
      if ((true == apn_lst->bootstrap_apn) &&
          (0x00 != lwm2mH->bs_apn_match_reg_id))
      {
        /* Copy the data call instance handles to restore them later */
        bs_match_v4_nip_hndl = apn_lst->callHdl_v4_nip;
        bs_match_v6_hndl = apn_lst->callHdl_v6;
      }
      apn_lst = apn_lst->next;
    }
    if(lwm2mH->apn_cfg != NULL)
    {
      lwm2m_free_apn_list(lwm2mH->apn_cfg);
      lwm2mH->apn_cfg = NULL;
    }
  }

  /* Retrieve the server instance related inforamtion */
  server_list = (lwm2m_object_t *)LWM2M_LIST_FIND(lwm2mH->objectList, LWM2M_SERVER_OBJECT_ID);
  if (!server_list)
  {
    ds_iot_ulog_err("Server list not found, Data call bring up failure");
    return NULL;
  }

  /* Traverse the server instance list and populate the APN information required */
  server_inst = (server_instance_t *)server_list->instanceList;
  while (server_inst)
  {
    uint8_t                  pdn_type = INVALID_TRANSPORT;
    uint8_t            pref_transport = INVALID_TRANSPORT;
    bool                  reg_trigger = false;
    uint8_t         reg_trigger_param = INVALID_TRANSPORT;
    uint8_t                  bindings = INVALID_TRANSPORT;
    uint8_t         transport_binding = INVALID_TRANSPORT;
    carrier_apn_cfg_t     *c_apn_info = NULL;
    lwm2m_server_t       *server_list = NULL;
    apn_conn_input_params_t conn_params;

    s_apn_info = NULL;
    memset(&conn_params, 0x00, sizeof(apn_conn_input_params_t));

    /* Find the APN information associated with the short server ID */
    if ((short_serverID) && (*short_serverID != server_inst->shortServerId))
      goto skip_apn;

    /* Retrieve APN information associated with the server short ID (carrier_apn config) */
    c_apn_info = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(lwm2mH->c_apn_cfg, server_inst->shortServerId);

    /* Retrieve the server list to get the version associated with the server */
    server_list = (lwm2m_server_t *)lwm2m_list_find((lwm2m_list_t *)lwm2mH->serverList, server_inst->shortServerId);
    if (NULL == server_list)
      goto skip_apn;

    /* Allocate memory for storing the server APN information */
    s_apn_info = (carrier_apn_cfg_t *)malloc(sizeof(carrier_apn_cfg_t));
    if (NULL == s_apn_info)
      goto update_apn_err;

    memset(s_apn_info, 0x00, sizeof(carrier_apn_cfg_t));

    /* Bootstrap and registration server(s) have matching APNs */
    if ((0x00 != lwm2mH->bs_apn_match_reg_id) && 
        (server_inst->shortServerId == lwm2mH->bs_apn_match_reg_id))
    {
      /* Restore the saved bootstrap server data call instance handles to the matching registration server. We would only
       * be assinging the handles to just one of the matching registration server (eventhough there are multiple matching
       * servers). For the other severs, we would be bringing up the data calls separately and generating the handles.
       */
      s_apn_info->bs_match_callHdl_v4_nip = bs_match_v4_nip_hndl;
      s_apn_info->bs_match_callHdl_v6 = bs_match_v6_hndl;

      /* Reset the registration ID matching the bootstrap server APN */
      lwm2mH->bs_apn_match_reg_id = 0x00; 
    }

    /* For both these cases:
     *     LwM2Mv1.0 there is no associated APN link resource with the server instance
     *     NULL APN link configured for the server instance
     *
     * Just use the APN config parsed from "carrier apn" config file for both these cases
     */
    if ((0xFFFFFFFF == server_inst->apnLink) ||
        ((float)LWM2M_V_1_0 == server_list->version))
    {
      bool default_apn_populated = false;

      /* Associated APN information pertaining to the short server ID is found. In that case, go ahead
       * and re-use the existing carrier_apn config.
       *
       * If not present, that means the server configuration  was not present at the time of boot. Later,
       * when the bootstrapping was performed the configuration was written.
       */
      if (NULL == c_apn_info)
      {
        /* Generate default APN information on-the-fly for the registration server */
        if (NULL == (c_apn_info = populate_default_apn_info(server_inst->shortServerId, false)))
          goto skip_apn;

        default_apn_populated = true;
      }

      /* Copy the "carrier_apn" config to the "server apn" config */ 
      s_apn_info->short_server_id          = c_apn_info->short_server_id;
      s_apn_info->registration_apn         = true;
      strlcpy(s_apn_info->apn_name, c_apn_info->apn_name, CARRIER_APN_LEN);
      s_apn_info->apn_class                = c_apn_info->apn_class;
      s_apn_info->bs_if_reg_fails          = c_apn_info->bs_if_reg_fails;
      s_apn_info->ip_family                = c_apn_info->ip_family;
      s_apn_info->ip_support               = c_apn_info->ip_support;
      s_apn_info->transport_binding        = c_apn_info->ip_family;
      s_apn_info->srvc_type                = srvc_type;
      s_apn_info->apn_link                 = 0xFFFFFFFF;

      if ((float)LWM2M_V_1_0 != server_list->version)
      {
        s_apn_info->non_ip_support         = c_apn_info->non_ip_support;

        /* PDN type supported in the factory provisioned config */
        if ((s_apn_info->ip_support) && (s_apn_info->non_ip_support))
        {
          /* PDN type in object-11 doesn't support both NON_IP and IP PDN types. In profile change detection, we need to
           * take care of this case where factory provisioned config allows support for both PDN types. For now, mark the
           * PDN type as 0xFF and take care of it in the profile change detection.
           */
          s_apn_info->pdn_type = 0xFF;
          pdn_type = NONIP_PDN_TYPE | IP_PDN_TYPE;
        }
        else if (s_apn_info->ip_support)
        {
          s_apn_info->pdn_type = s_apn_info->ip_family;
          pdn_type = IP_PDN_TYPE;
        }
        else if (s_apn_info->non_ip_support)
        {
          s_apn_info->pdn_type = AF_NONIP;
          pdn_type = NONIP_PDN_TYPE;
        }
        
        /* Check the mode of operation - M1 or NB1
         * NB-1 - UDP transport and Non-IP transport are supported
         *  M-1 - UDP transport is only supported
         */
        if (IOTAPP_CIOT_MODE_NB1 == srvc_type.ciot_mode)
        {
          /* Retrieve the bindings information */ 
          bindings |= utils_stringToBinding((uint8_t *)server_inst->binding, strlen(server_inst->binding));
          /* Skip transports other than U and N */
          bindings &= (UDP_NONIP_TRANSPORT);
          if (!bindings)
            bindings = UDP_TRANSPORT;
          /* Preferred mode of transport (optional) */
          if (strlen(server_inst->prefTransport))
          {
            pref_transport = utils_stringToBinding((uint8_t *)server_inst->prefTransport, strlen(server_inst->prefTransport));
            /* Skip transports other than U and N */
            pref_transport &= (UDP_NONIP_TRANSPORT);
            if (!pref_transport)
              pref_transport = INVALID_TRANSPORT;
          }

          /* Register update trigger param (optional) */
          if (server_inst->regTrigger)
          {
            reg_trigger = server_inst->regTrigger;

            if (strlen(server_inst->regTriggerParam))
              reg_trigger_param = utils_stringToBinding((uint8_t *)server_inst->regTriggerParam, strlen(server_inst->regTriggerParam));

            if (reg_trigger_param)
            {
              /* Skip transports other than U and N. If the register trigger transport param is invalid, ignore the parameter.
               * Invalid transport param in register update trigger would result in using look-up tables 1 and 3 (not table 2).
               */
              reg_trigger_param &= (UDP_NONIP_TRANSPORT);
              if (!reg_trigger_param)
                reg_trigger_param = INVALID_TRANSPORT;
            }

            /* Reset the registration trigger and registration trigger parameter */
            server_inst->regTrigger = false;
            memset(server_inst->regTriggerParam, 0x00, MAX_REG_TRIGGER_PARAM_SIZE);
          }

          /* Transport binding selection */
          transport_binding = lwm2m_transport_selection(bindings, pref_transport, pdn_type, reg_trigger, reg_trigger_param);
          s_apn_info->transport_binding = (UDP_TRANSPORT == transport_binding) ? s_apn_info->ip_family : AF_NONIP;
          ds_iot_ulog_high2("Transport binding (0x%x) is selected for SSID[%d]", s_apn_info->transport_binding, s_apn_info->short_server_id);
        }
        else
        {
          s_apn_info->transport_binding = s_apn_info->ip_family;
        }
      }

      s_apn_info->retry_timeout_lst_len    = c_apn_info->retry_timeout_lst_len;
      if (s_apn_info->retry_timeout_lst_len)
      {       
        s_apn_info->retry_timeout_lst = malloc(sizeof(uint32_t) * c_apn_info->retry_timeout_lst_len);
        if (NULL == s_apn_info->retry_timeout_lst)
          goto update_apn_err;

        memcpy(s_apn_info->retry_timeout_lst, c_apn_info->retry_timeout_lst, (sizeof(uint32_t) * c_apn_info->retry_timeout_lst_len));
      }

      /* Release any resources allocated for populating the default APN configuration */
      if ((default_apn_populated) && (c_apn_info))
      {
        lwm2m_free(c_apn_info);
        c_apn_info = NULL;
      }

      /* Add the APN information parsed from the "carrier_apn" config to the "server apn" config */
      server_apn_lst = (carrier_apn_cfg_t *)LWM2M_LIST_ADD(server_apn_lst, s_apn_info);
      server_inst = server_inst->next;

      continue;
    }

    /* LwM2Mv1.1 or higher
     *
     * Copy the mandatory items from "carrier_apn" config (will be overwritten later from the server APN link).
     * If the carrier APN configuration is not present, it means that the server information was not present at
     * the time of boot. Later, during bootstrapping it was populated. 
     *
     */
    if (NULL != c_apn_info)
    {
      /* Trigger re-bootstrapping on registration failure */ 
      s_apn_info->bs_if_reg_fails   = c_apn_info->bs_if_reg_fails;

      /* APN class */
      s_apn_info->apn_class         = c_apn_info->apn_class;

      /* APN family */
      s_apn_info->ip_family         = c_apn_info->ip_family;
      s_apn_info->ip_support        = c_apn_info->ip_support;
      s_apn_info->non_ip_support    = c_apn_info->non_ip_support;

      /* PDN type supported in the factory provisioned config */
      if ((s_apn_info->ip_support) && (s_apn_info->non_ip_support))
        /* PDN type in object-11 doesn't support both NON_IP and IP PDN types. In profile change detection, we need to
         * take care of this case where factory provisioned config allows support for both PDN types. For now, mark the
         * PDN type as 0xFF and take care of it in the profile change detection. 
         */
        s_apn_info->pdn_type = 0xFF;
      else if (s_apn_info->ip_support)
        s_apn_info->pdn_type = s_apn_info->ip_family;
      else if (s_apn_info->non_ip_support)
        s_apn_info->pdn_type = AF_NONIP;
      
      /* APN Name */ 
      strlcpy(s_apn_info->apn_name, c_apn_info->apn_name, CARRIER_APN_LEN);
    }

    /* Retrieve the APN connection object information associated with the APN link */
    get_apn_conn_params(server_inst->apnLink, &conn_params);

    s_apn_info->srvc_type          = srvc_type;
    s_apn_info->short_server_id    = server_inst->shortServerId;
    s_apn_info->registration_apn   = true;
    s_apn_info->apn_link           = server_inst->apnLink;

    /* Retry (mandatory) - if not present config from the carrier_apn config will be used */
    if (conn_params.params_set & RECONNECT_SET)
    {
      /* Reconnection schedule - APN connection instance */ 
      s_apn_info->retry_timeout_lst      = conn_params.reconn_schedule;
      s_apn_info->retry_timeout_lst_len  = conn_params.reconn_count;
    }
    else
    {
      uint32_t *retry_timeout_lst = NULL;
      uint32_t  retry_timeout_lst_len = 0x00;

      /* Retry (mandatory) - if not present APN from the carrier_apn config will be used */
      retry_timeout_lst_len = (c_apn_info) ? c_apn_info->retry_timeout_lst_len : data.cfg_info.retry_timeout_lst_len;
      retry_timeout_lst =  (c_apn_info) ? c_apn_info->retry_timeout_lst : data.cfg_info.retry_timeout_lst;
      if (retry_timeout_lst_len)
      {
        s_apn_info->retry_timeout_lst = malloc(sizeof(uint32_t) * retry_timeout_lst_len);
        if (NULL == s_apn_info->retry_timeout_lst)
          goto update_apn_err;
        memcpy(s_apn_info->retry_timeout_lst, retry_timeout_lst, (sizeof(uint32_t) * retry_timeout_lst_len));
        s_apn_info->retry_timeout_lst_len = retry_timeout_lst_len;
      }
    }

    /* APN (mandatory) - if not present APN from the carrier_apn config will be used */
    if (conn_params.params_set & APN_NAME_SET)
    {
      /* Overriding the factory provisioned APN configuration. Compare the APN configured in the Object-11 with the
       * one in factory provisioned config. If they differ, update the APN class to other than CLASS-2.
       */
      if (strncmp(s_apn_info->apn_name, conn_params.apn_name, CARRIER_APN_LEN))
        s_apn_info->apn_class = LWM2M_APN_CLASS3;

      s_apn_info->apn_name_obj_11 = true; 
      strlcpy(s_apn_info->apn_name, conn_params.apn_name, CARRIER_APN_LEN);
    }
 
    /* Authentication type */
    if (conn_params.params_set & AUTH_TYPE_SET)
    {
      s_apn_info->auth_type_obj_11= true;
      s_apn_info->auth_type = conn_params.auth_type;
    }

    /* User name */
    if (conn_params.params_set & USER_NAME_SET)
    {
      s_apn_info->user_name_obj_11 = true;
      strlcpy(s_apn_info->user_name, conn_params.username, USERNAME_LEN);
    }

    /* Password */
    if (conn_params.params_set & SECRET_SET)
    {
      s_apn_info->password_obj_11 = true;
      strlcpy(s_apn_info->password, conn_params.secret, PASSWORD_LEN);
    }

    /* PDN type */
    if (conn_params.params_set & PDN_TYPE_SET)
    {
      s_apn_info->pdn_type_obj_11 = true;
      s_apn_info->pdn_type = conn_params.pdn_type;

      /* Non-IP configuration */
      if (AF_NONIP == conn_params.pdn_type)
      {
        s_apn_info->ip_support = false;
        s_apn_info->non_ip_support = true;
        pdn_type = NONIP_PDN_TYPE;
      }
      else
      {
        /* IP configuration */
        s_apn_info->ip_family = conn_params.pdn_type;

        s_apn_info->non_ip_support = false;
        s_apn_info->ip_support = true;
        pdn_type = IP_PDN_TYPE;
      }
    }
    else
    {
      /* PDN type is not configured in object 11 for the server instance. Assumption is NON-IP is always supported 
       * if the PDN type is not specified (same is the case with APN config file as well).
       */
      pdn_type = NONIP_PDN_TYPE;
      s_apn_info->non_ip_support = true;

      /* IP family is picked from the APN config if it is supported. Compare the APN configured in the Object-11 with the
       * one in factory provisioned config. If they match, pick the IP family information that was configured in factory
       * provisioned configuration. If not, default to IPv4 family.  
       */
      if (strncmp(s_apn_info->apn_name, conn_params.apn_name, CARRIER_APN_LEN))
      {
        s_apn_info->ip_family  = AF_INET;
        s_apn_info->ip_support = true;
      }

      pdn_type |= (s_apn_info->ip_support) ? IP_PDN_TYPE : pdn_type;

      /* PDN type in object-11 doesn't support both NON_IP and IP PDN types. In profile change detection, we need to
       * take care of this case where factory provisioned config allows support for both PDN types. For now, mark the
       * PDN type as 0xFF and take care of it in the profile change detection.
       *
       * For just NON_IP, set the PDN type to AF_NONIP the value in Object-11.
       */
      s_apn_info->pdn_type = ((NONIP_PDN_TYPE | IP_PDN_TYPE) == pdn_type) ? 0xFF : AF_NONIP;
    }

    /* Retrieve the bindings information */
    bindings |= utils_stringToBinding((uint8_t *)server_inst->binding, strlen(server_inst->binding));
    /* Skip transports other than U and N */
    bindings &= (UDP_NONIP_TRANSPORT);
    if (!bindings)
      bindings = UDP_TRANSPORT;

    /* Preferred mode of transport (optional) */
    if (strlen(server_inst->prefTransport))
    {
      pref_transport = utils_stringToBinding((uint8_t *)server_inst->prefTransport, strlen(server_inst->prefTransport));
      /* Skip transports other than U and N */
      pref_transport &= (UDP_NONIP_TRANSPORT);
      if (!pref_transport)
        pref_transport = INVALID_TRANSPORT;
    }
    
    /* Register update trigger param (optional) */
    if (server_inst->regTrigger)
    {
      reg_trigger = server_inst->regTrigger;

      if (strlen(server_inst->regTriggerParam))
        reg_trigger_param = utils_stringToBinding((uint8_t *)server_inst->regTriggerParam, strlen(server_inst->regTriggerParam));

      if (reg_trigger_param)
      {
        /* Skip transports other than U and N. If the register trigger transport param is invalid, ignore the parameter.
         * Invalid transport param in register update trigger would result in using look-up tables 1 and 3 (not table 2).
         */
        reg_trigger_param &= (UDP_NONIP_TRANSPORT);
        if (!reg_trigger_param)
          reg_trigger_param = INVALID_TRANSPORT;
      }  

      /* Reset the registration trigger and registration trigger parameter */
      server_inst->regTrigger = false;
      memset(server_inst->regTriggerParam, 0x00, MAX_REG_TRIGGER_PARAM_SIZE);
    }

    /* Transport binding selection */
    transport_binding = lwm2m_transport_selection(bindings, pref_transport, pdn_type, reg_trigger, reg_trigger_param);
    s_apn_info->transport_binding = (UDP_TRANSPORT == transport_binding) ? s_apn_info->ip_family : AF_NONIP;
    ds_iot_ulog_high2("Transport binding (0x%x) for SSID[%d]", s_apn_info->transport_binding, s_apn_info->short_server_id); 

    /* Non-IP family - duplicates are not allowed */
    if ((s_apn_info->non_ip_support) &&
        (NULL != find_dup_apn_entry(server_apn_lst, s_apn_info)))
    {
      ds_iot_ulog_err("LWM2M_ERROR : Invalid configuration - duplicates for Non-IP APN found (skipping)");
      goto skip_apn;
    }

    /* Mandatory config items are configured and not missing */
    if ((!s_apn_info->retry_timeout_lst_len) ||
        (AF_UNSPEC == s_apn_info->ip_family) ||
        (INVALID_TRANSPORT == s_apn_info->transport_binding))
    {
      ds_iot_ulog_err("LWM2M_ERROR : Invalid configuration - Mandatory config items missing (skipping)");
      goto skip_apn;
    }
 
    /* Add the APN information parsed from the "carrier_apn" config to the "server apn" config */
    server_apn_lst = (carrier_apn_cfg_t *)LWM2M_LIST_ADD(server_apn_lst, s_apn_info); 
    server_inst = server_inst->next;
    continue;

skip_apn:
    if(s_apn_info != NULL)
    {
      lwm2m_free_apn_list(s_apn_info);
      s_apn_info = NULL;
    }

    server_inst = server_inst->next;
  }

  /* Update the internal APN configuration with the new server configuration (particular server) */
  if (NULL != short_serverID)
  {
    bool reconnect = false;
    bool send_update = false;
    carrier_apn_cfg_t *apn_info = NULL;

    /* Retrieve APN information associated with the server short ID (carrier_apn config) */
    apn_info = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(lwm2mH->apn_cfg, *short_serverID);
    if (apn_info)
    {
      /* Release the resources associated with the previous APN information */
      lwm2mH->apn_cfg = (void *) LWM2M_LIST_RM(lwm2mH->apn_cfg, *short_serverID, &apn_info);

      /* Additional check added to address KW error */
      if (apn_info)
      {
        /* Retrieve the reconnect and register update flag status */
        reconnect = apn_info->reconnect;
        send_update = apn_info->send_update;
        if(apn_info->retry_timeout_lst)
        {
          lwm2m_free(apn_info->retry_timeout_lst);
          apn_info->retry_timeout_lst = NULL;
        }
        lwm2m_free(apn_info);
        apn_info = NULL;
      }
    }

    /* Add just the updated APN information internally (KW errors - added extra checking for NULL) */
    if (server_apn_lst)
    {
      /* Update the reconnect and register update flag status */
      server_apn_lst->reconnect = reconnect;
      server_apn_lst->send_update = send_update;

      if (lwm2mH->apn_cfg)
      {
        lwm2mH->apn_cfg = (carrier_apn_cfg_t *)LWM2M_LIST_ADD(lwm2mH->apn_cfg, server_apn_lst);
      }
      else 
      {
        lwm2mH->apn_cfg = server_apn_lst;
      }
    }
  }
  else
  {
    /* Update the internal APN configuration with the newly populated server list */
    if(lwm2mH->apn_cfg != NULL)
        lwm2m_free_apn_list(lwm2mH->apn_cfg);

    lwm2mH->apn_cfg = server_apn_lst;
  }

  return server_apn_lst;

update_apn_err:
  {
    if(s_apn_info != NULL)
    {
      lwm2m_free_apn_list(s_apn_info);
      s_apn_info = NULL;
    }

    if(server_apn_lst != NULL)
    {
      lwm2m_free_apn_list(server_apn_lst);
      server_apn_lst = NULL;
    }
  }

  return NULL;
}

bool utility_compare_APN_cfg(carrier_apn_cfg_t *cfg_dm, carrier_apn_cfg_t *cfg_bs, bool checkApnName)
{  
   /* PDN type is not configured in Object-11. We pick the PDN type information from the factory configuration file for that server. */

   if(cfg_dm == NULL || cfg_bs == NULL)
   	 return false;
   
   /* APN info match */
   if ((checkApnName == true) && strncmp(cfg_bs->apn_name, cfg_dm->apn_name, CARRIER_APN_LEN))
   {
      //LOG_INFO("Factory provisioned APN [%s] for SSID [%d] differs from bootstrap APN name [%s]\n", cfg_dm->apn_name, cfg_dm->short_server_id, cfg_bs->apn_name); 
      ds_iot_log_strf_high("Factory APN [%s] for SSID [%d] differs from BS APN name [%s]", cfg_dm->apn_name, cfg_dm->short_server_id, cfg_bs->apn_name); 
      return false;
   }

   /* Support for IP and NON_IP configuration */
   if ((cfg_dm->non_ip_support != cfg_bs->non_ip_support) || 
       (cfg_dm->ip_support     != cfg_bs->ip_support))
   {
     //LOG_INFO("Bootstrap APN [%s] PDN types doesn't match the factory config for SSID [%d] ", cfg_bs->apn_name, cfg_dm->short_server_id);
     ds_iot_log_strf_high("BS APN [%s] PDN types doesn't match the factory config for SSID [%d] ", cfg_bs->apn_name, cfg_dm->short_server_id);
     return false;
   }
   /* IP configuration */ 
   else if ((cfg_dm->ip_support == cfg_bs->ip_support) &&
            (cfg_dm->ip_family  != cfg_bs->ip_family))
   {
     //LOG_INFO("Bootstrap APN [%s] IP family doesn't match the factory config for SSID [%d]", cfg_bs->apn_name, cfg_dm->short_server_id);
     ds_iot_log_strf_high("BS APN [%s] IP family doesn't match the factory config for SSID [%d]", cfg_bs->apn_name, cfg_dm->short_server_id);
     return false;
   } 

   return true;
}

bool bootstrap_server_apn_info_match
(
  lwm2m_context_t *lwm2mH,
  carrier_apn_cfg_t *bs_apn_cfg
)
{
  bool match = false;
  server_instance_t *server_inst = NULL;
  lwm2m_object_t *server_list = NULL;

  /* Validate the input parameters */
  if ((NULL == lwm2mH) ||
      (NULL == bs_apn_cfg))
  {
    ds_iot_ulog_err2("Invalid input params - lwm2mH [0x%x], bs_apn_cfg [0x%x]", lwm2mH, bs_apn_cfg);
    return false;
  }

  /* Intialize the APN match registration server ID */
  lwm2mH->bs_apn_match_reg_id = 0x00; 

  /* Retrieve the server instance related inforamtion */
  server_list = (lwm2m_object_t *)LWM2M_LIST_FIND(lwm2mH->objectList, LWM2M_SERVER_OBJECT_ID);
  if (!server_list)
  {
    ds_iot_ulog_err("Server list not found, Data call bring up failure");
    return false;
  }

  /* Traverse the server instance list and populate the APN information required */
  server_inst = (server_instance_t *)server_list->instanceList;
  while (server_inst)
  {
    lwm2m_server_t *server_list = NULL;
    carrier_apn_cfg_t *c_apn_info = NULL;
    bool c_apn_info_to_free = false; 

    /* Retrieve the server list to get the version associated with the server */
    server_list = (lwm2m_server_t *)lwm2m_list_find((lwm2m_list_t *)lwm2mH->serverList, server_inst->shortServerId);
    if (NULL == server_list)
      goto next_instance;

    /* Retrieve APN information associated with the server short ID (carrier_apn config) */
    c_apn_info = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(lwm2mH->c_apn_cfg, server_inst->shortServerId);
    if (NULL == c_apn_info)
    {
        /* Generate default APN information on-the-fly for the registration server */   
        if (NULL == (c_apn_info = populate_default_apn_info(server_inst->shortServerId, false)))
          goto next_instance;
        c_apn_info_to_free = true;
    }

    /* Pick the PDN information from the factory provisioned config files */
    if ((0xFFFFFFFF == server_inst->apnLink) ||
        ((float)LWM2M_V_1_0 == server_list->version))
    {

      bool result = utility_compare_APN_cfg(c_apn_info, bs_apn_cfg, true);
      if(result == false)
      {
        goto next_instance;
      }

      /* Registration server APN that matches the bootstrap server APN is found */
      lwm2mH->bs_apn_match_reg_id = server_inst->shortServerId;
      match = true;

    ds_iot_log_strf_high("Factory APN [%s] for SSID [%d] matches BS APN name [%s]", c_apn_info->apn_name, server_inst->shortServerId, bs_apn_cfg->apn_name);
    if(c_apn_info_to_free)
    {
      c_apn_info_to_free = false;
      lwm2m_free(c_apn_info);
      c_apn_info = NULL;
    }
      break;
    }
    else
    {
      apn_conn_input_params_t conn_params;  

      /* Initialize APN connection information */
      memset(&conn_params, 0x00, sizeof(apn_conn_input_params_t));
      conn_params.pdn_type = 0xFF;

      /* Retrieve the APN connection object information associated with the APN link */
      get_apn_conn_params(server_inst->apnLink, &conn_params);
      
      /* Release the resources associated with the re-connection schedule, as it is not used for decision-making */
      if ((conn_params.params_set & RECONNECT_SET) &&
          (conn_params.reconn_count))
        free(conn_params.reconn_schedule);

      /* Any credentials associated with the APN in object-11 */
      if (((conn_params.params_set & AUTH_TYPE_SET) && 
           (conn_params.auth_type != QAPI_DSS_AUTH_PREF_PAP_CHAP_NOT_ALLOWED_E)) ||
          (conn_params.params_set & USER_NAME_SET) ||
          (conn_params.params_set & SECRET_SET))
      {
        ds_iot_log_strf_high("Skipping object-11 APN [%s] for SSID [%d] - Credentials are configured", conn_params.apn_name, bs_apn_cfg->apn_name);
        goto next_instance;
      }

      /* APN info match */
      if ((conn_params.params_set & APN_NAME_SET) &&
          (strncmp(bs_apn_cfg->apn_name, conn_params.apn_name, CARRIER_APN_LEN)))
      {
        ds_iot_log_strf_high("Obj-11 APN [%s] for SSID [%d] differs from BS APN name [%s]", conn_params.apn_name, server_inst->shortServerId, bs_apn_cfg->apn_name);
        goto next_instance;
      }

      /* PDN type match */
      if (conn_params.params_set & PDN_TYPE_SET) 
      {
        bool bs_non_ip_support = bs_apn_cfg->non_ip_support;
        bool bs_ip_support = bs_apn_cfg->ip_support;
        uint8_t bs_ip_family = bs_apn_cfg->ip_family;

        /* Bootstrap server supports both IP and NON_IP PDN types which not possible using PDN type resource in Object-11 */
        if ((bs_non_ip_support) &&
            (bs_ip_support))
        {
          ds_iot_log_strf_high("BS APN [%s] supports IP & NON_IP, not possible using Object-11 for SSID [%d]", bs_apn_cfg->apn_name, server_inst->shortServerId);
          goto next_instance;
        }
        /* Non-IP configuration */ 
        else if ((AF_NONIP == conn_params.pdn_type) &&
                 (false == bs_non_ip_support))
        {
          ds_iot_log_strf_high("BS APN [%s] doesn't support NON_IP, supported by Object-11 for SSID [%d]", bs_apn_cfg->apn_name, server_inst->shortServerId);
          goto next_instance;
        }
        /* IP configuration */
        else if (bs_ip_family != conn_params.pdn_type) 
        {
          ds_iot_log_strf_high("BS APN [%s] IP family no match with Obj-11 for SSID [%d]", bs_apn_cfg->apn_name, server_inst->shortServerId);
          goto next_instance;
        }
      }
      else
      {
        /* PDN type is not configured in Object-11. We pick the PDN type information from the factory configuration file for that server. */
        bool result = utility_compare_APN_cfg(c_apn_info, bs_apn_cfg, false);
        if(result == false)
        {
            goto next_instance;
        }
      }

      /* Registration server APN that matches the bootstrap server APN is found */
      lwm2mH->bs_apn_match_reg_id = server_inst->shortServerId;
      match = true;

      ds_iot_log_strf_high("Obj-11 APN [%s] for SSID [%d] matches BS APN name [%s]", conn_params.apn_name, server_inst->shortServerId, bs_apn_cfg->apn_name);
      if(c_apn_info_to_free)
      {
        c_apn_info_to_free = false;
        lwm2m_free(c_apn_info);
        c_apn_info = NULL;
      }
      break;
    }

next_instance:
    if(c_apn_info_to_free)
    {
      c_apn_info_to_free = false;
      if(c_apn_info != NULL)
      {
        lwm2m_free(c_apn_info);
        c_apn_info = NULL;
      }
    }
    server_inst = server_inst->next;
  }

  return match;
}

