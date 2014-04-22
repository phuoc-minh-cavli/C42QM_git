#ifndef DS_DEFS_H
#define DS_DEFS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        DS _ D E F S . H

DESCRIPTION
  Header containing  names that need to be used in other contexts

Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/api/public/ds_defs.h#4 $ : //source/qcom/qct/modem/api/datacommon/main/latest/DS_defs.h#41 $ $DateTime: 2022/06/20 04:09:21 $ $Author: pwbldsvc $

when        who        what, where, why
--------    ---    ---------------------------------------------------------- 
01/30/18    svaka    Created main header file for DS stack.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "queue.h"
#include "ds_errno.h"
#include "ps_in.h"
#include "ds_sys.h"
#include "ds_ipfltr_defs.h"
//#include "ds_flow.h"


/*===========================================================================

                              TYPE DEFINITIONS

===========================================================================*/


/*---------------------------------------------------------------------*/
//temp: for compilation
#define pdn_ptr_type void*
#define bearer_ptr_type void*
//#define ds_flow_type void*
/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  DS_MAX_DOMAIN_NAME_SIZE - Maximum Domain name size.
---------------------------------------------------------------------------*/
#define DS_MAX_DOMAIN_NAME_SIZE    256

/*---------------------------------------------------------------------------
  DS_MAX_SIP_SERVER_DOMAIN_NAMES - Max supported Sip domain names.
---------------------------------------------------------------------------*/
#define DS_MAX_SIP_SERVER_DOMAIN_NAMES    5

/*---------------------------------------------------------------------------
DS_MAX_SIP_SERVER_ADDRESSES - Max supported Sip addresses.
---------------------------------------------------------------------------*/
#define DS_MAX_SIP_SERVER_ADDRESSES       6

/*---------------------------------------------------------------------------
DS_NUM_DNS_ADDRS -
Currently DS support a maximum of 2 DNS addresses per pdn -
Primary and Secondary.
This define shall be irrelevant and its usage altered when the handset
supports more DNS addresses.
---------------------------------------------------------------------------*/
#define DS_NUM_DNS_ADDRS               2

/* Max lenght of App specific information in Operator reserved PCO */
#define MAX_OP_RES_PCO_APP_SPECIFIC_LEN 255

#define MAX_SYSTEM_MTU 9216      /* MAX system MTU supported by Modem      */

/*---------------------------------------------------------------------------
  Maximum number of filters allowed in each QOS request. UMTS can fit atmost
  8 filters in one message. By restricting this to 8, UMTS need not worry
  about whether network rejected previous message.
---------------------------------------------------------------------------*/
#define MAX_FLTR_PER_REQ  16
#define MAX_ADDITIONAL_FLOWS_PER_REQ 6            /* 8 total: min/max/other
                                                     8 for rx and 8 for tx */


/*==========================================================================
DS Framework client type: ENUM to specify DS Framework clients, client needs to pass its client ID as part of 
PDN operations with ds framework.
===========================================================================*/
typedef enum 
{
  DS_FWK_CLIENT_INVALID =-1,
  DS_FWK_CLIENT_RMSM,
  DS_FWK_CLIENT_SOCKETS,
  DS_FWK_CLIENT_RMNET,
  DS_FWK_CLIENT_ATCOP,
  DS_FWK_CLIENT_TLB,
  DS_FWK_CLIENT_PROTO_V6,
  DS_FWK_CLIENT_MAX
}ds_fwk_client_type;

/*---------------------------------------------------------------------------
TYPEDEF DS_PDN_EVENT_ENUM_TYPE

DESCRIPTION
  List of all the possible events on an interface that modules can register
  callbacks for.
---------------------------------------------------------------------------*/
typedef enum
{
  PDN_MIN_EV,
  PDN_UP_EV,
  PDN_DOWN_EV,
  BEARER_UP_EV,
  BEARER_FLOW_ENABLED_EV,
  BEARER_FLOW_DISABLED_EV,
  PDN_ADDR_CHANGED_EV,
  PDN_PREFIX_UPDATE_EV, /* IPv6 specific event */
  PDN_VALID_RA_EV, /* IPv6 specific event */
  PDN_FLOW_ADDED_EV,
  PDN_FLOW_MODIFIED_EV,
  PDN_FLOW_DELETED_EV,
  PDN_IPV6_PRIV_ADDR_GENERATED_EV,
  PDN_IPV6_PRIV_ADDR_DEPRECATED_EV,
  PDN_IPV6_PRIV_ADDR_EXPIRED_EV,
  PDN_IPV6_PRIV_ADDR_DELETED_EV,
  PDN_IPV6_EXTERNAL_ADDR_DELETED_EV,
  /**< Event to convey external IPv6 address is deleted. */
  PDN_EXTENDED_IP_CONFIG_EX_EV,
  /**< Posted by mode handler when modem reserved pco(protocol configuration 
       options) information is saved on the pdn. PCO has APN, PCO container 
       ID and PCO message string and used by the network for authentication
       purpose.
  */
  /* QOS Events generated to Clients based on QOS status */
  PDN_NET_INITIATED_QOS_AVAILABLE_EV ,
  PDN_NET_AVAILABLE_QOS_MODIFIED_EV,
  PDN_NET_INITIATED_QOS_UNAVAILABLE_EV,
  PDN_WWAN_TECH_SUSPENSION_EV,
  PDN_EVENT_MAX                                     
#if 0
  /*-------------------------------------------------------------------------
    To make sure that libraries are not messed up when features are undefined,
    assigning an explicit value to FLOW_MIN_EV. Otherwise library will have
    one value for flow event and regular code will have another value
  -------------------------------------------------------------------------*/
  FLOW_MIN_EV                     = PDN_EVENT_MAX,
  FLOW_NULL_EV                    = FLOW_MIN_EV,     /*DSS_IFACE_IOCTL_QOS_UNAVAILABLE_EV*/
  FLOW_ACTIVATING_EV              = FLOW_MIN_EV + 1,
  FLOW_ACTIVATED_EV               = FLOW_MIN_EV + 2, /*DSS_IFACE_IOCTL_QOS_AVAILABLE_MODIFIED_EV*/
  FLOW_SUSPENDING_EV              = FLOW_MIN_EV + 3,
  FLOW_SUSPENDED_EV               = FLOW_MIN_EV + 4, /*DSS_IFACE_IOCTL_QOS_AVAILABLE_DEACTIVATED_EV*/
  FLOW_RESUMING_EV                = FLOW_MIN_EV + 5,
  FLOW_GOING_NULL_EV              = FLOW_MIN_EV + 6,
  FLOW_CONFIGURING_EV             = FLOW_MIN_EV + 7,
  FLOW_TX_ENABLED_EV              = FLOW_MIN_EV + 8,
  FLOW_TX_DISABLED_EV             = FLOW_MIN_EV + 9,
  FLOW_RX_FLTR_UPDATED_EV         = FLOW_MIN_EV + 10,
  FLOW_MODIFY_ACCEPTED_EV         = FLOW_MIN_EV + 11, /*DSS_IFACE_IOCTL_QOS_MODIFY_ACCEPTED_EV*/
  FLOW_MODIFY_REJECTED_EV         = FLOW_MIN_EV + 12,
  FLOW_MAX_EV
#endif

} ds_pdn_event_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF DS_NET_DOWN_REASON_TYPE

DESCRIPTION
  Enum type to determine the network going down reason.
  The 26 onwards values are taken from the 3GPP document and mapped
  corresponding to the values defined in the doc.
  3GPP TS 24.008 version 3.5.0 Release 1999
  The values from 500 onwards do NOT map to the 3GPP spec.
  The net down reasons have the following format:

  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |          Type                 |             Reason            |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  The upper two bytes represent the type of error.
  The type is 0 for all legacy net down reason such as CDMA, GSM, HDR, EAP
  and IPSEC
  The type is 1 for all MIP error codes.
  The type is 6 for all 3GPP error codes.
  The type is 7 for all PPP error codes.
  The type is 8 for all eHRPD error codes.
  The type is 9 for all IPv6 error codes
  The lower two bytes represent the actual net down reason.
---------------------------------------------------------------------------*/
#define TYPE_REASON_VAL(type, reason) ((0xFFFF0000 & (type << 16)) | (0x0000FFFF & reason))
typedef enum
{
  DS_NET_DOWN_REASON_NOT_SPECIFIED               = 0,
  /**< The reason is posted by the UE when a more appropriate reason is not 
       known.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry */
  DS_NET_DOWN_REASON_OPERATOR_DETERMINED_BARRING = TYPE_REASON_VAL(6,8),
  /**< The reason code is posted by the MME to indicate operator has barred 
       the UE. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Temporary if PDN throttling applies. Permanent
       otherwise. 
       Recovery Mechanism:
       If PDN throttling is enabled, DSS clients can obtain information about
       PDN throttling via invoking a DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO.
       QMI clients can use QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is
       throttled, the client shall wait for the corresponding time period
       before retrying the call. If the PDN is not throttled, the client
       may retry immediately.
       If PDN throttling does not apply, contact network provider */
  DS_NET_DOWN_REASON_NAS_SIGNALLING_ERROR = TYPE_REASON_VAL(6, 14),
  /**< The reason code is posted by the NAS to inform higher layers 
       that deactivation is triggered because of NW.
       To differentiate this external cause with internal 
       cause DS_NET_DOWN_REASON_PPD_UNKNOWN_REASON code added _EXT_ to it
       Failure Type: Temporary
       Recovery Mechanism: Client may retry   */  
	   
  DS_NET_DOWN_REASON_LLC_SNDCP_FAILURE           = TYPE_REASON_VAL(6,25),
  /**< This reason is posted to indicate that the network cannot provide the 
       requested service and PDP context is deactivated because of LLC or
       SNDCP failure
       Failure Type: Temporary
       Recovery Mechanism: Client may retry */
  DS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES   = TYPE_REASON_VAL(6,26),
  /**< This reason is posted to indicate that the network cannot provide the 
       requested service due to insufficient resources.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry */
  DS_NET_DOWN_REASON_UNKNOWN_APN              = TYPE_REASON_VAL(6,27),
  /**< This reason is posted to specify that the APN was required and not 
       specified or APN could not be resolved. In LTE mode of operation,
       this is a PDN throttling cause code, meaning the UE may endup
       throttling further requests to the same APN.
       Failure Type: Permanent.
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If PDN throttling does not apply, client may retry with a different APN
       name or profile. On further failure with the same cause code, check with
       network provider. */
  DS_NET_DOWN_REASON_UNKNOWN_PDP              = TYPE_REASON_VAL(6,28),
  /**< The reason is posted by the network to indicate that the PDN type 
       was not recognized.
       Failure Type: Permanent
       Recovery Mechanism: Client may retry with a different IP type */
  DS_NET_DOWN_REASON_AUTH_FAILED              = TYPE_REASON_VAL(6,29),
  /**< The reason is posted when authentication fails. In LTE mode of operation, 
       this is a PDN throttling cause code, meaning the UE may endup throttling
       further requests to the same APN.
       Failure Type: Permanent
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately. Client shall retry
       with a different set of authentication parameters/algorithm */
  DS_NET_DOWN_REASON_GGSN_REJECT              = TYPE_REASON_VAL(6,30),
  /**< The reason is posted when the request was rejected by Serving GW or 
       PDN GW. In LTE mode of operation, this is a PDN throttling cause code, 
       meaning the UE may endup throttling further requests to the same APN.
       Failure Type: Temporary 
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.*/
  DS_NET_DOWN_REASON_ACTIVATION_REJECT        = TYPE_REASON_VAL(6,31),
  /**< The reason is posted when the request is rejected by the network 
       due to unspecified reasons. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately. */
  DS_NET_DOWN_REASON_OPTION_NOT_SUPPORTED     = TYPE_REASON_VAL(6,32),
  /**< The reason is posted when UE requested a service not supported by the 
       PLMN. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Temporary. 
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       Client may also retry after change in PLMN */
  DS_NET_DOWN_REASON_OPTION_UNSUBSCRIBED      = TYPE_REASON_VAL(6,33),
  /**< The reason is posted when UE requested a service option for which it 
       has no subscription. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN. 
       Failure Type: Temporary if PDN throttling applies. Permananet otherwise. 
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If PDN throttling does not apply, check device configuration or check with network
       provider */
  DS_NET_DOWN_REASON_OPTION_TEMP_OOO          = TYPE_REASON_VAL(6,34),
  /**< This reason is posted when the network is temporarily out of resources 
       to service the request. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.*/
  DS_NET_DOWN_REASON_NSAPI_ALREADY_USED       = TYPE_REASON_VAL(6,35),
  /**< The reason is posted to indicate that PTI (Procedure Transaction Identifier) 
       used in the request is already active via another UE requested procedure.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry after some time*/
  DS_NET_DOWN_REASON_REGULAR_DEACTIVATION     = TYPE_REASON_VAL(6,36),
  /**< This reason is posted by the network to indiate a regualr release of 
       bearer resources.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry */
  DS_NET_DOWN_REASON_QOS_NOT_ACCEPTED         = TYPE_REASON_VAL(6,37),
  /**< This reason is posted by the network to indicate that the QoS 
       requested by the UE could not be accepted.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry with different QoS */
  DS_NET_DOWN_REASON_NETWORK_FAILURE          = TYPE_REASON_VAL(6,38),
  /**< This reason is posted when an error occurs in the network. 
       In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.*/
  DS_NET_DOWN_REASON_UMTS_REATTACH_REQ        = TYPE_REASON_VAL(6,39),
  /**< This reason is posted by the network to request for bearer 
       reactivation. This code may be posted during network congestion.
       Failure Type: Temporary
       Recovery Mechanism: Client must retry after some time */
  DS_NET_DOWN_REASON_FEATURE_NOT_SUPPORTED    = TYPE_REASON_VAL(6,40),
  /**< This reason is posted to indicate that the Feature is not 
       supported by the network
       Failure Type: Permanent 
       Recovery Mechanism: Client shall retry after making sure the feature is
       supported by network*/
  DS_NET_DOWN_REASON_TFT_SEMANTIC_ERROR       = TYPE_REASON_VAL(6,41),
  /**< The reason is posted by the network to indicate semantic error(s) 
       in specifying TFT operation included in the request.
       Failure Type: Permanent
       Recovery Mechanism: Client may retry with different TFT */
  DS_NET_DOWN_REASON_TFT_SYNTAX_ERROR         = TYPE_REASON_VAL(6,42),
  /**< The reason is posted by the network to indicate syntactic error(s) 
       in specifying TFT operation included in the request.
       Failure Type: Permanent 
       Recovery Mechanism: Client may retry with different TFT */
  DS_NET_DOWN_REASON_UNKNOWN_PDP_CONTEXT      = TYPE_REASON_VAL(6,43),
  /**< The reason is posted when the bearer identity (or linked bearer
       identity) in the request is invalid (or inactive).
       Failure Type: Permanent
       Recovery Mechanism: Client may retry. */
  DS_NET_DOWN_REASON_FILTER_SEMANTIC_ERROR    = TYPE_REASON_VAL(6,44),
  /**< The reason is posted by the network to indicate semantic error(s) 
       in specifying packet filter(s) associated with a TFT.
       Failure Type: Permanent
       Recovery Mechanism: Client may retry with different TFT */
  DS_NET_DOWN_REASON_FILTER_SYNTAX_ERROR      = TYPE_REASON_VAL(6,45),
  /**< The reason is posted by the network to indicate syntactic error(s) 
       in specifying packet filter(s) associated with a TFT.
       Failure Type: Permanent
       Recovery Mechanism: Client may retry with different TFT */
  DS_NET_DOWN_REASON_PDP_WITHOUT_ACTIVE_TFT   = TYPE_REASON_VAL(6,46),
  /**< The reason is posted by the network when UW requested more than 
       one PDP connection without TFT.
       Failure Type: Permanent
       Recovery Mechanism: Client may retry with a TFT */
  DS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED       = TYPE_REASON_VAL(6,50),
  /**< The reason is posted when the network supports IPV4 PDP type only. 
       IPV6 is not allowed. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Permanent
       Recovery Mechanism: 
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately. PDP type during
       retry must be set to IPV4. */
  DS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED       = TYPE_REASON_VAL(6,51),
  /**< The reason is posted when the network supports IPV6 PDP type only. 
       IPV4 is not allowed. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Permanent
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately. PDP type during
       retry must be set to IPV6 */
  DS_NET_DOWN_REASON_SINGLE_ADDR_BEARER_ONLY  = TYPE_REASON_VAL(6,52),
  /**< The reason is posted when the network supports single address 
       bearers only, meaning dual IP bearers are not supported.      
       Failure Type: Permanent
       Recovery Mechanism: Client may retry the request specifying
       single IP bearer. */
  DS_NET_DOWN_REASON_ESM_INFO_NOT_RECEIVED    = TYPE_REASON_VAL(6,53),
  /**< The reason is posted by the network to indicate that the PDN 
       connection request was rejected because ESM information was
       not received.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry */
  DS_NET_DOWN_REASON_PDN_CONN_DOES_NOT_EXIST  = TYPE_REASON_VAL(6,54),
  /**< The reason is posted by the network during handover from a non-3GPP 
       network to indicate that the MME does not have any information
       regarding the requested PDN connection.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry */
  DS_NET_DOWN_REASON_MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED = TYPE_REASON_VAL(6,55),
  /**< The reason is posted by the network to indicate that the UE is already
       connected to the requested APN via another PDN/PDN connection. 
       Failure Type: Permanent
       Recovery Mechanism: NV item 67248 allows UE to be configured to send
       multiple PDN connection requests to the same APN. Check the NV to ensure
       that it is in sync with the network capabilities. */
  DS_NET_DOWN_REASON_COLLISION_WITH_NW_INIT_REQ = TYPE_REASON_VAL(6,56),
  /**< The reason is posted by the network to indicate that the network has 
       already initiated the activation, modification or deactivation of bearer
       resources which was requested by the UE. 
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.  */
  DS_NET_DOWN_REASON_IP_V4V6_ONLY_ALLOWED       = TYPE_REASON_VAL(6,57),
  /**< The reason is posted when the network supports IP4V6 PDP type only. 
         NON-IP type is not allowed. In LTE mode of operation, this is a PDN
         throttling cause code, meaning the UE may endup throttling further
         requests to the same APN.
         Failure Type: Permanent
         Recovery Mechanism:
         DSS clients can obtain information about PDN throttling via invoking a
         DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
         QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
         wait for the corresponding time period before retrying the call. If the
         PDN is not throttled, the client may retry immediately. PDP type during
         retry must be set to IPV4V6 or V4 or V6 */
  DS_NET_DOWN_REASON_NON_IP_ONLY_ALLOWED        = TYPE_REASON_VAL(6,58),
  /**< The reason is posted when the network supports NON-IP PDP type only. 
           IPV4, IPV6 and IPV4V6 is not allowed. In LTE mode of operation, this
           is a PDN throttling cause code, meaning the UE may endup throttling
           further requests to the same APN.
           Failure Type: Permanent
           Recovery Mechanism:
           DSS clients can obtain information about PDN throttling via invoking a
           DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
           QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
           wait for the corresponding time period before retrying the call. If the
           PDN is not throttled, the client may retry immediately. PDP type during
           retry must be set to NON-IP */
  DS_NET_DOWN_REASON_UNSUPPORTED_QCI_VALUE = TYPE_REASON_VAL(6,59),
  /**< The reason is posted by the network if the QCI indicated in the UE 
       request cannot be supported.
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately with a different
       QCI value */ 
  DS_NET_DOWN_REASON_BEARER_HANDLING_NOT_SUPPORTED  = TYPE_REASON_VAL(6,60),
  /**< The reason is posted by the network to indicate that the procedure 
       requested by the UE was rejected because the bearer handling is not
       supported. 
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.  */ 
  DS_NET_DOWN_REASON_MAX_ACTIVE_PDP_CONTEXT_REACHED = TYPE_REASON_VAL(6,65),
  /**< The reason is posted by the network to indicate that maximum number 
       of active PDP contexts per UE has been reached. 
       Failure Type: Temporary
       Recovery Mechanism: Client may retry after it brings down one of the active PDNs */
  DS_NET_DOWN_REASON_UNSUPPORTED_APN_IN_CURRENT_PLMN = TYPE_REASON_VAL(6,66),
  /**< The reason is posted by the network is to indicate that the APN is 
       not supported in the current RAT and PLMN.
       Failure Type: Permanent
       Recovery Mechanism: client may retry after a change in either PLMN or RAT. */
  DS_NET_DOWN_REASON_INVALID_TRANSACTION_ID       = TYPE_REASON_VAL(6,81),
  /**< The reason is posted by the network to indicate that the PTI used in 
       the request is unassigned or reserved.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry */
  DS_NET_DOWN_REASON_MESSAGE_INCORRECT_SEMANTIC   = TYPE_REASON_VAL(6,95),
  /**< The reason is posted by the network to indicate receipt of an 
       invalid message. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Permanent.
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If throttling does not apply, client may not retry. Check with eureka
       for resolution. */
  DS_NET_DOWN_REASON_INVALID_MANDATORY_INFO       = TYPE_REASON_VAL(6,96),
  /**< The reason is posted by the network to indicate receipt of a message 
       with semantic error in a mandatory information element. In LTE mode
       of operation, this is a PDN throttling cause code, meaning the UE may
       endup throttling further requests to the same APN.
       Failure Type: Temporary if PDN throttling applies. Permanent otherwise.
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If PDN throttling does not apply, client may not retry. Check with eureka
       for resolution */
  DS_NET_DOWN_REASON_MESSAGE_TYPE_UNSUPPORTED     = TYPE_REASON_VAL(6,97),
  /**< The reason is posted by the network to indicate receipt of a message 
       that is either undefined or defined but not implemented by the
       equipment sending this ESM cause. In LTE mode
       of operation, this is a PDN throttling cause code, meaning the UE may
       endup throttling further requests to the same APN.
       Failure Type: Temporary/Permanent
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If throttling does not apply, client may not retry. Check with network
       provider. */
  DS_NET_DOWN_REASON_MSG_TYPE_NONCOMPATIBLE_STATE = TYPE_REASON_VAL(6,98),
  /**< The reason is posted by the network to indicate receipt of a message 
       type that cannot be handled in the current network protocol state.
       In LTE mode of operation, this is a PDN throttling cause code, meaning
       the UE may endup throttling further requests to the same APN.
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.*/
  DS_NET_DOWN_REASON_UNKNOWN_INFO_ELEMENT         = TYPE_REASON_VAL(6,99),
  /**< The reason is posted by the network to indicate receipt of a message 
       that includes and information element that is either not defined or
       defined but not implemented by the equipment sending the ESM cause.
       In LTE mode of operation, this is a PDN throttling cause code, meaning
       the UE may endup throttling further requests to the same APN.
       Failure Type: Temporary/Permanent
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If PDN throttling does not apply, client may retry after a change in PLMN.  */
  DS_NET_DOWN_REASON_CONDITIONAL_IE_ERROR         = TYPE_REASON_VAL(6,100),
  /**< The reason is posted by the netowrk to indicate receipt of a message 
       that includes a syntactically incorrect information element. This
       message is ignored by the network.
       In LTE mode of operation, this is a PDN throttling cause code, meaning
       the UE may endup throttling further requests to the same APN.
       Failure Type: Temporary/Permanent
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If PDN throttling does not apply, client may not retry. Contact eureka for
       resolution */
  DS_NET_DOWN_REASON_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE = TYPE_REASON_VAL(6,101),
  /**< The reason is posted by the network to indicate receipt of a message 
       that cannot be handled in the current network protocol state.
       In LTE mode of operation, this is a PDN throttling cause code, meaning
       the UE may endup throttling further requests to the same APN.
       Failure Type: Temporary
       Recovery Mechanism:
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.*/
  DS_NET_DOWN_REASON_PROTOCOL_ERROR           = TYPE_REASON_VAL(6,111),
  /**< The reason is posted by the network to indicate a protocol error 
       when no other error applies. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Temporary if PDN throttling applies. Permanent otherwise.
       Recovery Mechanism:                                               .
       DSS clients can obtain information about PDN throttling via invoking a
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If PDN throttling does not apply, client may not retry. Check with network
       provider */
  DS_NET_DOWN_REASON_APN_TYPE_CONFLICT       = TYPE_REASON_VAL(6,112),
  /**< The reason is posted by the network to indicate that the EPS bearer 
       has an APN restriction value that cannot be used in conjunction with
       existing EPS bearers. In LTE mode of operation, this is a PDN
       throttling cause code, meaning the UE may endup throttling further
       requests to the same APN.
       Failure Type: Temporary if PDN throttling applies. Permanent otherwise.
       Recovery Mechanism:
       DS SYS IOCTL, DS_SYS_IOCTL_PDN_THROTTLE_INFO. QMI clients can use
       QMI_WDS_GET_PDN_THROTTLE_INFO. If the PDN is throttled, the client shall
       wait for the corresponding time period before retrying the call. If the
       PDN is not throttled, the client may retry immediately.
       If PDN throttling does not apply, client may not retry. Check device
       configuration */
  DS_NET_DOWN_REASON_INVALID_PCSCF_ADDR      = TYPE_REASON_VAL(6,113),
  /**< The reason is posted by the UE when it tears down a PDN for not 
       receiving a P-CSCF address that was mandatory.
       Failure Type: Permanent
       Recovery Mechanism: Client may retry with a different profile */

   DS_NET_DOWN_REASON_INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN = TYPE_REASON_VAL(6,114),
  /**< 
      This reason is posted by the UE when the data call is brought down
      due to insufficient resources to bring up another prioritized data call. 
      In LTE mode of operation this reason will be posted while bringing down an
      ongoing PDN connection, when the device runs out of bearer resources to bring
      up a priortized PDN connection
            
      Failure Type       : Permanent Failure
      Recovery Mechanism : Client may retry after the higher priority 
                           data call is brought down
  */
  DS_NET_DOWN_REASON_EMM_ACCESS_BARRED       = TYPE_REASON_VAL(6,115),
  /**< The reason is returned when there is RRC failure of NAS signalling 
       which is results in connection establishment rejected by the network
       Failure Type: Temporary 
       Recovery Mechanism: Client may retry after RRC has been established */
  
  DS_NET_DOWN_REASON_EMERGENCY_PDN_ONLY       = TYPE_REASON_VAL(6,116),
  /**< This reason is posted to indicate that pdn can support Emergency
       pdn Only
       Failure Type: Temporary
       Recovery Mechanism: UE can re-try after some time */
  
  DS_NET_DOWN_REASON_PDN_MISMATCH             = TYPE_REASON_VAL(6,117),
  /**< This reason is posted to indicate pdn mismatch between requested
       pdn and received pdn
       Failure Type:Temporary 
       Recovery Mechanism: UE can use re-try using correct pdn */
  
  DS_NET_DOWN_REASON_COMPANION_PDN_IN_USE     = TYPE_REASON_VAL(6,118),
  /**< This reason is posted to indicate that pdn can support Emergency
       pdn Only
       Failure Type: Temporary
       Recovery Mechanism: UE can use another profile which matches IP type */
  
  DS_NET_DOWN_REASON_IP_ADDRESS_MISMATCH         = TYPE_REASON_VAL(6,119),
  /**< This reason is posted to indicate IP address mismatch between 
       Profile and PDN Context
       Failure Type:  Temporary
       Recovery Mechanism: UE can retry after some time */

  DS_NET_DOWN_REASON_PDN_AND_POL_FAMILY_MISMATCH = TYPE_REASON_VAL(6,120),
  /**< This reason is posted to indicate that IFace and Policy IP type
       do not match
       Failure Type:  Temporary
       Recovery Mechanism: UE can retry after some time with another profile */
  DS_NET_DOWN_REASON_EMM_ACCESS_BARRED_INFINITE_RETRY       = TYPE_REASON_VAL(6,121),
  /**< The reason is returned when there is RRC failure of NAS signalling 
       which is results in connection establishment rejected by the network with
       an access barring timer value set to 20 ms
       Failure Type: Temporary 
       Recovery Mechanism: Client may retry after RRC has been established */

  DS_NET_DOWN_REASON_AUTH_FAILURE_ON_EMERGENCY_CALL  = TYPE_REASON_VAL(6,122),
  /**< This reason is returned when active data calls are brought down
       because authentication failure occured on emergency call.
       Data calls can be brought up after Emergency call has ended.
       Failure Type       : Temporary 
       Recovery Mechanism : UE can retry after Emergency call has ended*/
  
DS_NET_DOWN_REASON_INVALID_DNS_ADDR      = TYPE_REASON_VAL(6,123),
  /**< The reason is posted by the UE when it tears down a PDN for not 
       receiving a DNS address that was mandatory.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry with a different profile */

  DS_NET_DOWN_REASON_INVALID_PCSCF_DNS_ADDR      = TYPE_REASON_VAL(6,124),
  /**< The reason is posted by the UE when it tears down a PDN for not 
       receiving either a PCSCF or a DNS address one of them being mandatory.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry with the same profile */

  DS_NET_DOWN_REASON_TEST_LOOPBACK_MODE_A_OR_B_ENABLED = TYPE_REASON_VAL(6,125),
  /**< The reason is posted by the UE when when network indicates UE is loopback 
       test mode A or B.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry with the same profile once network
       deactivates loopback mode*/
  DS_NET_DOWN_REASON_EMM_ACCESS_BARRED_EAB       = TYPE_REASON_VAL(6,126),
  /**< The reason is returned when there is RRC failure of NAS signalling 
       which is results in connection establishment rejected by the network
       Failure Type: Temporary 
       Recovery Mechanism: Client may retry after RRC has been established */
  /* RESERVED  = TYPE_REASON_VAL(2,200) */

  DS_NET_DOWN_REASON_INTERNAL_MIN                 = TYPE_REASON_VAL(2,200),
  /**< Reserved for bound checks. Unused by clients */
  DS_NET_DOWN_REASON_INTERNAL_ERROR               = TYPE_REASON_VAL(2,201),
    /**< 
        This error code is returned when data call is brought down due  
        to some unspecified internal error
        Failure Type       : Permanent Failure
        Recovery Mechanism : Unknown
    */
  DS_NET_DOWN_REASON_INTERNAL_CALL_ENDED          = TYPE_REASON_VAL(2,202),
    /**< 
        This error code is returned when call was ended by internal clients  
        Failure Type       : Temporary Failure
        Recovery Mechanism : Client may retry after some time
    */
  DS_NET_DOWN_REASON_INTERNAL_UNKNOWN_CAUSE_CODE  = TYPE_REASON_VAL(2,203),
  /**< 
        This error code is returned when reason for the call end is unknown.  
        Failure Type       : Permanent Failure
        Recovery Mechanism : Unknown
    */
  DS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE           = TYPE_REASON_VAL(2,204),
    /**< 
        This error code is returned when data call is brought down due  
	to some unspecified unknown error
        Failure Type       : Permanent Failure
        Recovery Mechanism : Unknown
    */
  DS_NET_DOWN_REASON_CLOSE_IN_PROGRESS            = TYPE_REASON_VAL(2,205),
  /**< 
        This error code is returned when a data call teardown is in progress
        Failure Type       : Temporary Failure
        Recovery Mechanism : Client may retry once call end notification is
        returned when tear down is complete.
    */
  DS_NET_DOWN_REASON_NW_INITIATED_TERMINATION     = TYPE_REASON_VAL(2,206),
  /**< 
        This error code is returned when data call is brought down by 
		    the network
        Failure Type       : Permanent Failure
        Recovery Mechanism : Unknown
    */
  DS_NET_DOWN_REASON_APP_PREEMPTED                = TYPE_REASON_VAL(2,207),
  /**< 
        This error code is returned when another application preempts
        the data call.
        Failure Type       : Temporary Failure
        Recovery Mechanism : Client can retry when other application
        stops using the data call.
    */
  DS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_DISALLOWED = TYPE_REASON_VAL(2,208),
    /**< 
        This error code is returned when V4 PDN is in throttled state 
        due to network providing only V6 address e.g during the previous  
        VSNCP bringup (subs_limited_to_v6). The time for which the IPv4  
        PDN is throttled is determined by the IPv4 throttling timers  
        maintained in the profile
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after the V6 
                             interface is brought down and the V4 
                             throttling timer expires
    */
  DS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_THROTTLED  = TYPE_REASON_VAL(2,209),
    /**< 
        This error code is returned when V4 PDN is in throttled state 
        due to previous VSNCP bringup failure(s). The time for which the  
        IPv4 PDN is throttled is determined by the IPv4 throttling 
        timers maintained in the profile
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after V4 throttling 
                             timer expires
    */
  DS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_DISALLOWED = TYPE_REASON_VAL(2,210),
    /**< 
        This error code is returned when V6 PDN is in throttled state 
        due to network providing only V4 address during the previous  
        VSNCP bringup (subs_limited_to_v4). The time for which the IPv6  
        PDN is throttled is determined by the IPv6 throttling timers  
        maintained in the profile
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after the V4 
                             interface is brought down and the V6 
                             throttling timer expires
    */
  DS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_THROTTLED  = TYPE_REASON_VAL(2,211),
    /**< 
        This error code is returned when V6 PDN is in throttled state 
        due to previous VSNCP bringup failure(s). The time for which the  
        IPv6 PDN is throttled is determined by the IPv6 throttling  
        timers maintained in the profile
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after V6 throttling 
                             timer expires 
    */
  DS_NET_DOWN_REASON_MODEM_RESTART                = TYPE_REASON_VAL(2,212),
  /**< 
        This error code is returned when data call is brought 
		down due to modem restart
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after modem restart 
						     is complete.
    */
  DS_NET_DOWN_REASON_PDP_PPP_NOT_SUPPORTED        = TYPE_REASON_VAL(2,213),
    /**< 
        This error code is returned when PDP PPP calls are not supported        
        Failure Type       : Temporary Failure
        Recovery Mechanism : Clients may need to change Profile PDP type and retry
    */
  DS_NET_DOWN_REASON_UNPREFERRED_RAT              = TYPE_REASON_VAL(2,214),
    /**< 
        This error code is returned when data call is brought down since  
        the RAT on which the data call is attempted/connected is no  
        longer the preferred RAT
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination on the new preferred  
                             RAT reported by Data System Determination 
                             module
    */
  DS_NET_DOWN_REASON_PHYS_LINK_CLOSE_IN_PROGRESS  = TYPE_REASON_VAL(2,215),
    /**< 
        This error code is returned when data call bring up is rejected  
        because physical link is in the process of cleanup
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_APN_PENDING_HANDOVER  = TYPE_REASON_VAL(2,216),
     /**<
        This error code is returned when interface bring up is attempted for an APN 
        that is yet to be handed over to target RAT
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
     */      
  DS_NET_DOWN_REASON_PROFILE_BEARER_INCOMPATIBLE  = TYPE_REASON_VAL(2,217),
     /**< 
        This error code is returned when APN bearer type in the profile does not match Preferred
        network mode
        Failure Type       : : Temporary Failure
        Recovery Mechanism : Clients may need to change APN bearer type and retry
     */
     /* Error codes for calls ended by mode handler/EPC */
  DS_NET_DOWN_REASON_INTERNAL_MMGSDI_CARD_EVT         = TYPE_REASON_VAL(2,218),
    /**< 
        This error code is returned when data call is brought down  
        because card got refreshed/removed
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time. In 
                             case of card removal, expectation is that 
                             card has been reinserted into device
    */
  DS_NET_DOWN_REASON_INTERNAL_LPM_OR_PWR_DOWN         = TYPE_REASON_VAL(2,219),
    /**< 
        This error code is returned when data call is brought down  
        because device is going into lower power mode or powering down
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after device comes  
                             out of lower power mode or powers up
    */
  DS_NET_DOWN_REASON_INTERNAL_APN_DISABLED            = TYPE_REASON_VAL(2,220),
  /**< Alias to DS_NET_DOWN_REASON_APN_DISABLED. Do not publish in DSS/QMI */
  DS_NET_DOWN_REASON_APN_DISABLED                     
                                    = DS_NET_DOWN_REASON_INTERNAL_APN_DISABLED,
    /**< 
        This error code is returned when APN is disabled in card / 
        profile
        Failure Type       : Permanent Failure 
        Recovery Mechanism : Retry call origination after enabling APN  
                             in card/profile
    */
  DS_NET_DOWN_REASON_INTERNAL_MPIT_EXPIRED            = TYPE_REASON_VAL(2,221),
    /**< 
        This error code is returned when data call is brought down  
        because maximum PPP inactivity timer expired
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_INTERNAL_IPV6_ADDR_TRANSFER_FAILED  = TYPE_REASON_VAL(2,222),
    /**<
        This error code is returned when the transfer of secondary
        IPv6 address from srat to trat is failed during handoff.
        Failure Type       : Temporary failure
        Recovery Mechanism : Retry call origination immediately on
	    					the current system.
    */
  DS_NET_DOWN_REASON_INTERNAL_TRAT_SWAP_FAILED        = TYPE_REASON_VAL(2,223),
    /**<
        This error code is returned when the swap rat (trat <-> srat)
        fails after successful bringup of trat during handoff.
        Failure Type       :  Temporary Failure
        Recovery Mechanism :  Retry call origination immediately on
	    					  the current system.
    */

  DS_NET_DOWN_REASON_INTERNAL_EHRPD_TO_HRPD_FALLBACK  = TYPE_REASON_VAL(2,224),
    /**< 
        This error code is returned when data call is brought down  
        because device falls back from eHRPD to HRPD (not because of  
        OOS on eHRPD but due to operator/spec driven eHRPD to HRPD  
        fallback requirements)
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after Data System  
                             Determination module reportes preferred 
                             system as HRPD
    */
  DS_NET_DOWN_REASON_MANDATORY_APN_DISABLED           = TYPE_REASON_VAL(2,225),
    /**< 
        This error code is returned when any mandatory APN is disabled, 
        and MinApnList Disallow call config item is set to TRUE in 
        device
        Failure Type       : Permanent Failure 
        Recovery Mechanism : Retry call origination after enabling 
                             Mandatory APN(s)
    */
  DS_NET_DOWN_REASON_MIP_CONFIG_FAILURE               = TYPE_REASON_VAL(2,226),
    /**< 
        This error code is returned when UE is in MIP Only config (QCMIP 
        =2) but MIP config fails on call bring up due to incorrect  
        provisioning
        Failure Type       : Permanent Failure 
        Recovery Mechanism : Retry call origination after correctly 
                             provisioning the device with MIP 
                             information
    */
  DS_NET_DOWN_REASON_INTERNAL_PDN_INACTIVITY_TIMER_EXPIRED  = TYPE_REASON_VAL(2,227),
     /**<
        This error code is returned when pdn inactivity timer expired due to no data transmission 
        in a configurable duration of time. 

        Failure Type : Temporary Failure

        Recovery Mechanism : Retry call origination immediately since it is brought 
        down due to pdn inactivity timer expiration.

     */ 

  DS_NET_DOWN_REASON_MAX_V4_CONNECTIONS               = TYPE_REASON_VAL(2,228),
    /**< 
        This error code is returned when an IPv4 data call bring up is rejected
        since UE already maintains the allotted maximum number of IPv4 data
        connections
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after disconnecting existing
                             IPv4 call(s)
    */

  DS_NET_DOWN_REASON_MAX_V6_CONNECTIONS             = TYPE_REASON_VAL(2,229),
     /**<
      This error code is returned when an IPv6 data call bring up is rejected
      since UE already maintains the allotted maximum number of IPv6 data
      connections
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination after disconnecting existing
                           IPv6 call(s)
  */

  DS_NET_DOWN_REASON_APN_MISMATCH                   = TYPE_REASON_VAL(2,230),
  /**< 
      This error code is returned when a new PDN bring up is rejected
      during interface selection since UE already allotted the available
      interfaces for other PDNs
        Failure Type : Temporary Failure
      Recovery Mechanism : Retry call origination after disconnecting existing
                           PDN(s)
  */

  DS_NET_DOWN_REASON_IP_VERSION_MISMATCH            = TYPE_REASON_VAL(2,231),
  /**< 
      This error code is returned when a new call bring up is rejected
      since the existing data call IP type doesn't match the requested IP
      type
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination after disconnecting existing
                           call
  */

  DS_NET_DOWN_REASON_DUN_CALL_DISALLOWED            = TYPE_REASON_VAL(2,232),
  /**< 
      This error code is returned when a dial up networking (DUN) call bring
      up is rejected since UE is in eHRPD RAT
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry DUN call origination after UE transitions to
                           1X/HRPD
  */

  DS_NET_DOWN_REASON_INVALID_PROFILE                = TYPE_REASON_VAL(2,233),
  /**< 
      This error code is returned when a call bring up is requested with an
      invalid profile
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination using correct profile
  */

  DS_NET_DOWN_REASON_INTERNAL_EPC_NONEPC_TRANSITION = TYPE_REASON_VAL(2,234),
  /**< 
      This error code is returned when a data call is rejected/brought down
      since UE is transition between EPC and NONEPC RAT
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination after UE settles down on the
                           new RAT
  */

  DS_NET_DOWN_REASON_INVALID_PROFILE_ID             = TYPE_REASON_VAL(2,235),
  /**< 
      This error code is returned when a call bring up is requested with an
      invalid profile id
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination using correct profile id
     */ 

  DS_NET_DOWN_REASON_INTERNAL_CALL_ALREADY_PRESENT  = TYPE_REASON_VAL(2,236),
  /**<
      This error code is returned when a data call with the same policy is already
      connected or in the process of bringup on another RmNet instance. */

  DS_NET_DOWN_REASON_PDN_IN_USE                   = TYPE_REASON_VAL(2,237),
  /**< 
      This error code is returned the current Iface is being in use 
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination after disconnecting existing
                           call
  */

  DS_NET_DOWN_REASON_IP_PDP_MISMATCH                = TYPE_REASON_VAL(2,238),
  /**< 
      This error code is returned when PPP call is attempted on PDP profile 
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination using correct profile id
  */

DS_NET_DOWN_REASON_APN_DISALLOWED_ON_ROAMING      = TYPE_REASON_VAL(2,239),
  /**< 
      This reason is returned if PDN connection to the APN is disallowed on
      roaming network.
   
      DS will generate DS_SYS_EVENT_3GPP_ROAMING_DISALLOWED_INFO
      to inform clients of the APNs that need to deregister
      and disconnect (if the PDN is UP) while UE is on Roaming network.
   
      Clients need to deregister/disconnect if APN name corresponding
      to the client is listed in this SYS event.
   
      Clients can register again if APN name corresponding to the
      client is not listed in this SYS event.   

      Failure Type       : Temporary Failure
      Recovery Mechanism : Retry using an APN name that is
                           not blocked or
                           retry when UE moves back to home network.
                                                                       */
  DS_NET_DOWN_REASON_APN_PRM_CHG                    = TYPE_REASON_VAL(2,240),
  /**< 
      When APN-related parameters are changed, the PDN associated with
      the parameters has to be brought down and re-established with the
      changed parameters. This error code is returned on a failure to
      re establish the PDN with the changed parameters
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Application can retry immediately to establish
                           a connection.
 
  */

  DS_NET_DOWN_REASON_PDN_IN_USE_CFG_MATCH         = TYPE_REASON_VAL(2,241),
  /**< 
      This error code is returned when the pdn is already in use with a 
      matching configuration. 
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination after disconnecting existing
                           call
  */                                                 

  DS_NET_DOWN_REASON_NULL_APN_DISALLOWED          = TYPE_REASON_VAL(2,242),
  /**< 
      The error code is returned when a PDN is attempted to be brought up
      with NULL APN and when NULL APN is not supported
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Application can retry immediately with a non NULL apn
                           to establish a connection.
  */

   DS_NET_DOWN_REASON_THERMAL_MITIGATION          = TYPE_REASON_VAL(2,243),
   /**< 
       The error code is returned when thermal level increases and causes
       calls to be torn down since normal mode of operation is not allowed.
       Failure Type       : Temporary Failure 
       Recovery Mechanism : Application can retry after thermal level resumes
                            to normal
   */

  DS_NET_DOWN_REASON_SUBS_ID_MISMATCH            = TYPE_REASON_VAL(2,244),
  /**< 
      This error code is returned when a new call bring up is rejected
      since the subs_id in the profile doesn't match the subs_id in
      ACL policy info
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination using correct subs_id
  */

  DS_NET_DOWN_REASON_DATA_SETTINGS_DISABLED            = TYPE_REASON_VAL(2,245),
  /**< 
      The error code is returned when PDN Connection to a given APN is
      disallowed because Data is disabled from the Device
      User Interface(UI) Settings.
   
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Application can retry with the same APN
                           if
                           a) Data Connection is re-enabled from the UI
                           AND
                           b) Data Roaming Connection is enabled from the UI
                              OR UE is on Home network
  */

  DS_NET_DOWN_REASON_DATA_ROAMING_SETTINGS_DISABLED    = TYPE_REASON_VAL(2,246),
  /**< 
      The error code is returned when PDN Connection to a given APN is
      disallowed because Data Roaming is disabled from the Device
      User Interface(UI) Settings and the UE is on Roaming Network.
   
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Application can retry with the same APN
                           if
                           a) Data Connection is enabled on the UI
                           AND
                           b) Data Roaming Connection is re-enabled from the UI
                              OR UE moves back to Home network
  */

    DS_NET_DOWN_REASON_APN_FORMAT_INVALID    = TYPE_REASON_VAL(2,247),
  /**< 
      The error code is returned when APN specified in policy does not follow
      the format specificated in 3gpp spec
   
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Application can retry with the another APN
                          
  */

  DS_NET_DOWN_REASON_VALIDATION_FAILURE      = TYPE_REASON_VAL(2,248),
  /**< 
         This error code is returned when data call is brought down due to some
         internal validation failure
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period 
  */

  DS_NET_DOWN_REASON_PROFILES_NOT_COMPATIBLE             = TYPE_REASON_VAL(2,251),
   /**< 
        This error code is returned when handoff failed 
        because 3gpp and 3gpp2 profile is not compatible.
        Failure Type       : Permanent Failure
        Recovery Mechanism : Client can retry a call using compatible 3GPP and 
                             3GPP2 profiles with the same APN and compatible IP types.
    */
  DS_NET_DOWN_REASON_NULL_RESOLVED_APN_NO_MATCH           = TYPE_REASON_VAL(2,252),
    /**< 
        This error code is returned when handoff failed because
        of NULL APN handoff, TRAT resolved APN is not same as SRAT APN
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with the network provider for NULL APN resolution mechanism.
    */

  DS_NET_DOWN_REASON_INVALID_APN_NAME      = TYPE_REASON_VAL(2,253),
    /**< 
         The reason is posted by the UE when it tears down a PDN being brought 
         up with an APN that is part of forbidden APN Name list.
         Failure Type: Temporary
         Recovery Mechanism: Client may retry with the different APN/Profile */

  DS_NET_DOWN_REASON_DDS_SWITCH_IN_PROGRESS      = TYPE_REASON_VAL(2,254),
  /**< The reason is posted by the UE when it tears down a PDN due to 
       dds switch is in progress.
       Failure Type: Temporary
       Recovery Mechanism: Client may retry once the dds switch is complete */

  DS_NET_DOWN_REASON_CALL_DISALLOWED_IN_ROAMING = TYPE_REASON_VAL(2,255),
  /**<  This reason is returned when roaming is disallowed during call bring up 
        Failure Type: Temporary
        Recovery Mechanism: Client may retry once it is not in roaming
     */

  DS_NET_DOWN_REASON_MO_EXCEPTIONAL_NOT_SUPPORTED      = TYPE_REASON_VAL(2,256),
    /**< The reason is posted by the UE when UE is unable to bring up call
          with MO exceptional data because device is not properly configured to support MO Exception.
         Failure Type: Permanent Failure
         Recovery Mechanism: Client will only be able to recover from this by configuring MO Exception
         support in SIM Card/EFNAS */


  DS_NET_DOWN_REASON_NON_IP_NOT_SUPPORTED      = TYPE_REASON_VAL(2,257),
    /**<  
         The reason is posted by the UE when UE is unable to bring up a Non-IP
         Data call because device is not camped on NB1 cell.
         Failure Type: Permanent Failure
         Recovery Mechanism: Client will only be able to recover from when UE camps on NB1 cell.*/

  DS_NET_DOWN_REASON_ERR_PDN_NON_IP_CALL_THROTTLED  = TYPE_REASON_VAL(2,258),
    /**< 
        This error code is returned when NON IP PDN is in throttled state due to 
        previous VSNCP bringup failure(s). Also the time for which NON IP PDN is 
        throttled can be obtained by querying QMI_WDS_GET_PDN_THROTTLE_INFO.
        NON IP PDN is throttled is determined by the NON IP throttling 
        timers maintained in the profile
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after Non-IP throttling  timer expires
    */

  DS_NET_DOWN_REASON_ERR_PDN_NON_IP_CALL_DISALLOWED = TYPE_REASON_VAL(2,259),
    /**< 
        This error code is returned when NON IP PDN is in disallowed state 
        due to network providing only IP address e.g during the previous  
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after the V6 
                             interface is brought down and the V4 
                             throttling timer expires
    */

  DS_NET_DOWN_REASON_NON_IP_TYPE_MISMATCH            = TYPE_REASON_VAL(2,261),
   /**< 
        This error code is returned .when a new call bring up is rejected while
        1.  Trying to bring up NoN_IP call on IPv4v6 profile.
        2.  Trying to bring up v4/v6 call on NON_IP profile.
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination with proper calltype and profile type.
    */

  DS_NET_DOWN_REASON_MAX_NB_PDN_REACHED              = TYPE_REASON_VAL(2,262),
   /**< 
        This error code is returned when a new call bring up is rejected as 
        max PDN supported over CAT NB1 are already active.
      Failure Type       : Temporary Failure 
      Recovery Mechanism : Retry call origination after existing call is down.
    */

  DS_NET_DOWN_REASON_INTERNAL_MAX,

/* To map CDMA specific call-end reasons from CM */
  DS_NET_DOWN_REASON_CDMA_LOCK                 = TYPE_REASON_VAL(3,500),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM due to  
        device in CDMA locked state
        Failure Type       : Permanent Failure
        Recovery Mechanism : Power cycle the device and retry call  
                             origination
    */
  DS_NET_DOWN_REASON_INTERCEPT                 = TYPE_REASON_VAL(3,501),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since it  
        received an intercept order from the base station
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_REORDER                   = TYPE_REASON_VAL(3,502),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM due to  
        receiving a reorder from base station
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_REL_SO_REJ                = TYPE_REASON_VAL(3,503),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM due to  
        receiving a release from base station with reason: SO Reject
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  DS_NET_DOWN_REASON_INCOM_CALL                = TYPE_REASON_VAL(3,504),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since it  
        received an incoming call from base station
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_ALERT_STOP                = TYPE_REASON_VAL(3,505),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM due to RL/FL  
        fade (or) receiving call release from base stations
        code Failure Type  : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_ACTIVATION                = TYPE_REASON_VAL(3,506),
    /**< 
        This error code is returned when data call is brought down 
        because traffic channel got rejected/released by CM due to 
        channel acquisition failures. This indicates that device has 
        failed acquiring all the channels in the PRL
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_MAX_ACCESS_PROBE          = TYPE_REASON_VAL(3,507),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM due maximum  
        access probes transmitted
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_CCS_NOT_SUPPORTED_BY_BS   = TYPE_REASON_VAL(3,508),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since  
        concurrent service is not supported by base station
        Failure Type       : Permanent Failure 
        Recovery Mechanism : Check with network provider
    */
  DS_NET_DOWN_REASON_NO_RESPONSE_FROM_BS       = TYPE_REASON_VAL(3,509),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since there  
        is no response received from base station
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_REJECTED_BY_BS            = TYPE_REASON_VAL(3,510),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM due to base  
        station rejecting the call
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  DS_NET_DOWN_REASON_INCOMPATIBLE              = TYPE_REASON_VAL(3,511),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since  
        concurrent services requested were not compatible
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider
    */
  DS_NET_DOWN_REASON_ALREADY_IN_TC             = TYPE_REASON_VAL(3,512),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since traffic  
        channel is already up for voice calls
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_GPS = TYPE_REASON_VAL(3,513),
    /**< 
        This error code is not used. Do not publish in DSS/QMI
        Failure Type       : N/A
        Recovery Mechanism : N/A
    */
  DS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_SMS = TYPE_REASON_VAL(3,514),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected since SMS is  
        ongoing
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after SMS is  
                             complete
    */
  DS_NET_DOWN_REASON_NO_CDMA_SRV               = TYPE_REASON_VAL(3,515),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since device  
        does not have CDMA service
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_MC_ABORT                  = TYPE_REASON_VAL(3,516),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since MC  
        aborted the origination/conversation
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_PSIST_NG                  = TYPE_REASON_VAL(3,517),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM due to  
        persistence test failure
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_UIM_NOT_PRESENT           = TYPE_REASON_VAL(3,518),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM due to RUIM  
        not present
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after inserting UIM  
                             card
    */
  DS_NET_DOWN_REASON_RETRY_ORDER               = TYPE_REASON_VAL(3,519),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM due to  
        receiving a retry order from base station
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_ACCESS_BLOCK              = TYPE_REASON_VAL(3,520),
    /**<
        This error code is returned when data call is brought down because 
        traffic channel rejected/released due to Access blocked by 
        base station
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_ACCESS_BLOCK_ALL          = TYPE_REASON_VAL(3,521),
    /**<
        This error code is returned when data call is brought down because
        traffic channel rejected due to Access blocked by the 
        base station for all mobile devicess
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_IS707B_MAX_ACC            = TYPE_REASON_VAL(3,522),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM due maximum  
        access probes for IS-707B call
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_THERMAL_EMERGENCY         = TYPE_REASON_VAL(3,523),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM to put  
        device in thermal emergency
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after device cools  
                             down
    */
  DS_NET_DOWN_REASON_CALL_ORIG_THROTTLED       = TYPE_REASON_VAL(3,524),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since call  
        origination is throttled by DCTM module
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after CM unthrottles 
                             call origination
    */
  DS_NET_DOWN_REASON_USER_CALL_ORIG_DURING_VOICE_CALL = TYPE_REASON_VAL(3,525),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got released by CM in favor of voice  
        call or SMS when concurrent voice and data are not supported
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after SMS/Voice call 
                             is complete
    */

/* To map GSM/WCDMA specific call-end reasons from CM */
  DS_NET_DOWN_REASON_CONF_FAILED               = TYPE_REASON_VAL(3,1000),
  /**<   This reason is returned when data call is brought down 
         because data call origination request failed by CM
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after some time  */
  DS_NET_DOWN_REASON_INCOM_REJ                 = TYPE_REASON_VAL(3,1001),
  /**<   This reason is returned when data call is brought down because 
         the other clients reject the incoming call
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after some time */
  DS_NET_DOWN_REASON_NO_GW_SRV                 = TYPE_REASON_VAL(3,1002),
  /**<   This reason is returned when the phone has no service 
         avaliable on G/W
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call after service becomes available */
  DS_NET_DOWN_REASON_NO_GPRS_CONTEXT           = TYPE_REASON_VAL(3,1003),
  /**<   This reason is returned when GPRS context is not available
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after some time   */
  DS_NET_DOWN_REASON_ILLEGAL_MS                = TYPE_REASON_VAL(3,1004),
  /**<   This reason is returned when the network refuses service to 
         the MS either because an identity of the MS is not acceptable to
         the network or because the MS does not pass the authentication check
         Failure Type       : Permanent Failure
         Recovery Mechanism : None  */
  DS_NET_DOWN_REASON_ILLEGAL_ME                = TYPE_REASON_VAL(3,1005),
  /**<   This reason is returned because ME could not authenticated 
         and the ME used is not acceptable to the network 
         Failure Type       : Permanent Failure
         Recovery Mechanism : None */
  DS_NET_DOWN_REASON_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED 
                                                     = TYPE_REASON_VAL(3,1006),
  /**<   This reason is returned to the MS when it is not allowed 
         to operate either GPRS or non-GPRS services
         Failure Type       : Permanent Failure
         Recovery Mechanism : None  */
  DS_NET_DOWN_REASON_GPRS_SERVICES_NOT_ALLOWED                       = TYPE_REASON_VAL(3,1007),
  /**<   This reason is returned to the MS when it is not allowed to 
         operate GPRS services
         Failure Type       : Permanent Failure
         Recovery Mechanism : None  */
  DS_NET_DOWN_REASON_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK
                                                     = TYPE_REASON_VAL(3,1008),
  /**<   This reason is returned because no matching identity/context could 
         be found in the network
         Failure Type       : Temporary failure
         Recovery Mechanism : Retry after a back-off time period  */
  DS_NET_DOWN_REASON_IMPLICITLY_DETACHED                             = TYPE_REASON_VAL(3,1009),
  /**<   This reason is returned to the MS either after the Mobile
         reachable timer has expired, or if the GMM context data related to
         the subscription dose not exist in the SGSN 
         Failure Type       : Temporary failure
         Recovery Mechanism : Retry after a back-off time period  */
  DS_NET_DOWN_REASON_PLMN_NOT_ALLOWED                                = TYPE_REASON_VAL(3,1010),
  /**<   This reason is returned when UE requests GPRS service, 
         or if the network initiates a detach request in a PLMN which does
         not offer roaming for GPRS services to that MS
         Failure Type       : Permanent Failure
         Recovery Mechanism : None  */
  DS_NET_DOWN_REASON_LA_NOT_ALLOWED                                  = TYPE_REASON_VAL(3,1011),
  /**<   This reason is returned because to the MS if it requests service, 
         or if the network initiates a detach request, in a location area
         where the HPLMN determines that the MS, by subscription, is not
         allowed to operate
         Failure Type       : Permanent Failure
         Recovery Mechanism : None */
  DS_NET_DOWN_REASON_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN          
                                                     = TYPE_REASON_VAL(3,1012),
  /**<   This reason is returned when UE request GPRS service or network 
         initiates a detach request in a PLMN which does not offer roaming
         for GPRS services
         Failure Type       : Permanent Failure
         Recovery Mechanism : None  */
  DS_NET_DOWN_REASON_PDP_DUPLICATE                                   = TYPE_REASON_VAL(3,1013),
  /**<   This reason is returned because PDP context already exists , 
         PDP context is rejected
         Failure Type       : Temporary Failure 
         Recovery Mechanism : Retry call origination with different APN  */
  DS_NET_DOWN_REASON_UE_RAT_CHANGE                                   = TYPE_REASON_VAL(3,1014),
  /**<   This reason is returned because there is RAT change on UE 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  DS_NET_DOWN_REASON_CONGESTION                                      = TYPE_REASON_VAL(3,1015),
  /**<   This reason is returned because network cannot serve a 
         request from the MS because of congestion 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_NO_PDP_CONTEXT_ACTIVATED                        = TYPE_REASON_VAL(3,1016),
  /**<   This reason is returned when MS requests an establishment of the 
         radio access bearers for all active PDP contexts by sending a
         SERVICE REQUEST message indicating "data" to the network, but
         the SGSN does not have any active PDP context
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time   */
  DS_NET_DOWN_REASON_ACCESS_CLASS_DSAC_REJECTION                     = TYPE_REASON_VAL(3,1017),
  /**<   This reason is returned when there is Access Class blocking restrictions 
         for the current camped cell
         Failure Type       : Temporary Failure
         Recovery Mechanism :  UE can try after some time */
  DS_NET_DOWN_REASON_PDP_ACTIVATE_MAX_RETRY_FAILED                   = TYPE_REASON_VAL(3,1018),
  /**<   This reason is returned when SM attempts PDP activation 
         for MAX times(4 attempts)
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_RAB_FAILURE                                     = TYPE_REASON_VAL(3,1019),
  /**<   This reason is returned when RAB failure occurs
         Failure Type       : Temporary Failure
         Recovery Mechanism :  UE can try after some time  */
  DS_NET_DOWN_REASON_EPS_SERVICES_NOT_ALLOWED                        = TYPE_REASON_VAL(3,1020),
  /**    Not Used */
  DS_NET_DOWN_REASON_TRACKING_AREA_NOT_ALLOWED                       = TYPE_REASON_VAL(3,1021),
  /**    Not Used */
  DS_NET_DOWN_REASON_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA       
                                                     = TYPE_REASON_VAL(3,1022),
  /**    Not Used */
  DS_NET_DOWN_REASON_NO_SUITABLE_CELLS_IN_TRACKING_AREA
                                                     = TYPE_REASON_VAL(3,1023),
  /**    Not Used */
  DS_NET_DOWN_REASON_NOT_AUTHORIZED_FOR_THIS_CSG                     = TYPE_REASON_VAL(3,1024),
  /**    Not Used */
  DS_NET_DOWN_REASON_ESM_UNKNOWN_EPS_BEARER_CONTEXT                  = TYPE_REASON_VAL(3,1025),
  /**<   This reason is returned when Invalid EPS bearer identity 
         in the request
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  DS_NET_DOWN_REASON_DRB_RELEASED_AT_RRC                             = TYPE_REASON_VAL(3,1026),
 /**<    This reason is returned when DRBs are released for many reason. 
         a) The DRB is released by RRC for internal reasons
         b) There is a mismatch scenario where UE has more DRBs than the N/W.
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_NAS_SIG_CONN_RELEASED                           = TYPE_REASON_VAL(3,1027),
  /**<   This reason is returned to indicate connection released
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_EMM_DETACHED                                    = TYPE_REASON_VAL(3,1028),
  /**<   This reason is returned when UE is detached
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_EMM_ATTACH_FAILED                               = TYPE_REASON_VAL(3,1029),
  /**<   This reason is returned when attach procedure is 
         rejected by the network
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_EMM_ATTACH_STARTED                              = TYPE_REASON_VAL(3,1030),
  /**<   This reason is returned for LTE E911 when attach procedure 
         is started for EMC purposes
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LTE_NAS_SERVICE_REQ_FAILED                      = TYPE_REASON_VAL(3,1031),
  /**<   This reason is returned indicates service request procedure failure
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_ESM_ACTIVE_DEDICATED_BEARER_REACTIVATED_BY_NW      
                                                     = TYPE_REASON_VAL(3,1032),
  /**<   This reason is returned when ACT DEDICATION BEARER is requested using 
         same DEFAULT BEARER ID
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_ESM_LOWER_LAYER_FAILURE                         = TYPE_REASON_VAL(3,1033),
  /**<   This reason is returned for collision scenarios for UE and network 
         initiated procedures.
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_ESM_SYNC_UP_WITH_NW                             = TYPE_REASON_VAL(3,1034),
  /**<   This reason is returned when bearer needs to be deactivated 
         to get in sync with network
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER 
                                                     = TYPE_REASON_VAL(3,1035),
  /**<   This reason is returned when ACT DEDICATION BEARER is requested 
         for an existing DEFAULT BEARER 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time   */
  DS_NET_DOWN_REASON_ESM_BAD_OTA_MESSAGE                               = TYPE_REASON_VAL(3,1036),
  /**<   This reason is returned to abort ongoing procedure in DS if 
         bad OTA message is received from network
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time   */
  DS_NET_DOWN_REASON_ESM_DS_REJECTED_THE_CALL                          = TYPE_REASON_VAL(3,1037),
  /**<   This reason is returned when DS rejects the call
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_ESM_CONTEXT_TRANSFERED_DUE_TO_IRAT
                                                     = TYPE_REASON_VAL(3,1038),
  /**<   This reason is returned when PDN is disconnected by DS due to IRAT 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_DS_EXPLICIT_DEACT                                 = TYPE_REASON_VAL(3,1039),
  /**<   This reason is returned when the dedicated bearer 
         will be deactivated regardless NW response
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_ESM_LOCAL_CAUSE_NONE                              = TYPE_REASON_VAL(3,1040),
  /**<   This reason is returned when no specific local cause is mentioned. 
         Usually come with a valid OTA cause
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE            
                                                     = TYPE_REASON_VAL(3,1041),
  /**<   This reason is returned to indicate that throttling is not 
         needed for this service request failure
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  DS_NET_DOWN_REASON_ACL_FAILURE                                       = TYPE_REASON_VAL(3,1042),
  /**<   This reason is returned when ACL check failure at lower layer
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LTE_NAS_SERVICE_REQ_FAILED_DS_DISALLOW            
                                                     = TYPE_REASON_VAL(3,1043),
  /**<   This reason is returned when service not allowed on the requested
         PLMN.
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  DS_NET_DOWN_REASON_EMM_T3417_EXPIRED                                 = TYPE_REASON_VAL(3,1044),
  /**<   This reason is returned when T3417 expires for service 
         request procedure 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  DS_NET_DOWN_REASON_EMM_T3417_EXT_EXPIRED                             = TYPE_REASON_VAL(3,1045),
  /**<   This reason is returned when ESR fails as T3417 EXT timer expires 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  DS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_TXN                      = TYPE_REASON_VAL(3,1046),
  /**<   This reason is returned when Transmission failure of uplink data 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_HO                       = TYPE_REASON_VAL(3,1047),
  /**<   This reason this is returned when uplink data failed to be delivered 
         due to Handover
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_CONN_REL
                                                     = TYPE_REASON_VAL(3,1048),
  /**<   This reason is returned when uplink data failed to be delivered 
         due to connection release
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time   */
  DS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_RLF                      = TYPE_REASON_VAL(3,1049),
  /**<   This reason is returned when uplink data failed to be 
         delivered due to Radio Link Failure
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN           
                                                     = TYPE_REASON_VAL(3,1050),
  /**<   This reason is returned where RRC is not in connected but NAS 
         sends uplink data request
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE                             = TYPE_REASON_VAL(3,1051),
  /**<   This reason is returned when connection failure at access stratum
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time   */
  DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_ABORTED                     = TYPE_REASON_VAL(3,1052),
  /**<   This reason is returned when connection establishment is aborted 
         due to other procedure
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_ACCESS_BARRED          //     
                                                     = TYPE_REASON_VAL(3,1053),
  /**<   This reason is returned when connection establishment failed 
         due to Lower layer RRC connection Failure
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time   */
  DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CELL_RESEL                  
                                                     = TYPE_REASON_VAL(3,1054),
  /**<   This reason is returned when connection establishment failed 
         due to cell reselection at access stratum
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time   */
  DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE              
                                                     = TYPE_REASON_VAL(3,1055),
  /**<   This reason is returned when connection establishment 
         failed due to configuration failure at RRC
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED               
                                                     = TYPE_REASON_VAL(3,1056),
  /**<   This reason is returned when connection could not be 
         establishment in the time limit 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_LINK_FAILURE
                                                     = TYPE_REASON_VAL(3,1057),
  /**<   This reason is returned when connection establishment 
         failed due to link failure at RRC 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_NOT_CAMPED
                                                     = TYPE_REASON_VAL(3,1058),
  /**<   This reason is returned when connection establishment failed 
         as RRC is not camped on any cell
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_SI_FAILURE
                                                     = TYPE_REASON_VAL(3,1059),
  /**<   This reason is returned when connection establishment 
         failed due to SI failure at RRC
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CONN_REJECT
                                                     = TYPE_REASON_VAL(3,1060),
  /**<   This reason is returned when connection establishment failed 
         due to NW rejecting UE connection request
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LRRC_CONN_REL_NORMAL                              = TYPE_REASON_VAL(3,1061),
  /**<   This reason is returned for normal connection release
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LRRC_CONN_REL_RLF                                 = TYPE_REASON_VAL(3,1062),
  /**<   This reason is returned when connection release failed due 
         to RLF conditions       
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  DS_NET_DOWN_REASON_LRRC_CONN_REL_CRE_FAILURE                         = TYPE_REASON_VAL(3,1063),
  /**<   This reason is returned when connection re-establishment failure 
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LRRC_CONN_REL_OOS_DURING_CRE                      = TYPE_REASON_VAL(3,1064),
  /**<   This reason is returned when UE going OOS during CR, so the 
         connection is getting released
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LRRC_CONN_REL_ABORTED                             = TYPE_REASON_VAL(3,1065),
  /**<   This reason is returned when connection released by RRC due 
         to the abort request
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_LRRC_CONN_REL_SIB_READ_ERROR                      = TYPE_REASON_VAL(3,1066),
  /**<   This reason is returned when connection release due to SIB read 
         error
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  DS_NET_DOWN_REASON_DETACH_WITH_REATTACH_LTE_NW_DETACH               
                                                     = TYPE_REASON_VAL(3,1067),
  /**<   This reason is returned when NW initiated detach with reattach
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH            
                                                     = TYPE_REASON_VAL(3,1068),
  /**<   This reason is returned when NW initiated detach without reattach
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */
  DS_NET_DOWN_REASON_ESM_PROC_TIME_OUT                                 = TYPE_REASON_VAL(3,1069),
  /**<   This reason is returned when ESM procedure max attempt 
         timeout failure
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time  */

  DS_NET_DOWN_REASON_INVALID_CONNECTION_ID                              = TYPE_REASON_VAL(3,1070),
    /**< 
        This error code is returned when No PDP exists with the 
        given Connection Id while doing Modification/Deactivation 
        OR Activation for already Active PDP
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_INVALID_NSAPI                                     = TYPE_REASON_VAL(3,1071),
    /**< 
        This error code is returned when NSAPIs exceeds max while 
        PDP activation. Invalid Modify/Deactivation Request by CM 
        for ConId. While activating Secondary when PDP already active 
        with same connection Id, Reject the ACTIVATE req
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_INVALID_PRI_NSAPI                                 = TYPE_REASON_VAL(3,1072),
    /**< 
        This error code is returned when Primary context for NSAPI 
        Doesn�t exist, Reject the SEC ACTIVATE req con_id
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_INVALID_FIELD                                     = TYPE_REASON_VAL(3,1073),
    /**< 
        This error code is returned when Unable to Encode 
        OTA message for MT PDP or Deactivate PDP
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_RAB_SETUP_FAILURE                                 = TYPE_REASON_VAL(3,1074),
    /**< 
        This error code is returned when When RAB is not established
        by lower layers during Activation/Modification/Deactivation
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_PDP_ESTABLISH_MAX_TIMEOUT                         = TYPE_REASON_VAL(3,1075),
    /**< 
        This error code is returned when 
        On Expiry of PDP establish timer with MAX (5) retries
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_PDP_MODIFY_MAX_TIMEOUT                            = TYPE_REASON_VAL(3,1076),
    /**< 
        This error code is returned On Expiry 
        of PDP Modify timer with MAX (4) retries
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_PDP_INACTIVE_MAX_TIMEOUT                          = TYPE_REASON_VAL(3,1077),
    /**< 
        This error code is returned when on Expiry of PDP Deactivate 
        timer with MAX (4) reties
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_PDP_LOWERLAYER_ERROR                              = TYPE_REASON_VAL(3,1078),
    /**< 
        This error code is returned PDP activation failed 
        because of RRC_ABORT or Forbidden PLMN
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_PPD_UNKNOWN_REASON                                = TYPE_REASON_VAL(3,1079),
    /**< 
        This error code is returned when Local Deactivation 
        SM_NATIONAL_ROAMING_NOT_ALLOWED: Roaming not allowed
        SM_NO_SUITABLE_CELLS_IN_LA: No Suitable Cells In Location Area
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_PDP_MODIFY_COLLISION                              = TYPE_REASON_VAL(3,1080),
    /**< 
        This error code is returned when MO PDP Modify collision when 
        MT PDP is already in progress
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_PDP_MBMS_REQUEST_COLLISION                        = TYPE_REASON_VAL(3,1081),
    /**< 
        This error code is returned when PDP_MBMS_REQUEST received when 
        PDP activation is already PDP_ACTIVE_PENDING on same Conn Id
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_MBMS_DUPLICATE                                    = TYPE_REASON_VAL(3,1082),
    /**< 
        This error code is returned when MBMS activation is already pending 
        and PDP_MBMS_REQUEST is triggered
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_SM_PS_DETACHED                                    = TYPE_REASON_VAL(3,1083),
    /**< 
        This error code is returned when 
        Internal cause for call end due to PS DETACH
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_SM_NO_RADIO_AVAILABLE                             = TYPE_REASON_VAL(3,1084),
    /**< 
        This error code is returned when Internal cause to indicate that 
        Radio resource is not available
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_SM_ABORT_SERVICE_NOT_AVAILABLE                    = TYPE_REASON_VAL(3,1085),
    /**< 
        This error code is returned when Internal cause to indicate abort 
        because service is not available
        Failure Type       : Temporary Failure 
        Recovery Mechanism : Retry call origination after some time
    */

  DS_NET_DOWN_REASON_MESSAGE_EXCEED_MAX_L2_LIMIT                       = TYPE_REASON_VAL(3,1086),
  /**<   This reason is returned when MAX size of L3 message is exceeded
         Failure Type       : Temporary Failure
         Recovery Mechanism : UE can try after some time */
  DS_NET_DOWN_REASON_SM_NAS_SRV_REQ_FAILURE                            = TYPE_REASON_VAL(3,1087),
  /**<   
         This reason is returned when NAS/Lower layers service request 
         is rejected by the network
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry call origination after some time 
  */

  DS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_REQ_ERROR                    = TYPE_REASON_VAL(3,1088),
  /**<   This reason is returned when there is an RRC Connection 
         Establishment Failure due to error in Request message.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_TAI_CHANGE                   = TYPE_REASON_VAL(3,1089),
  /**<   This reason is returned when there is an RRC Connection 
         Establishment Failure due to change in Tracking Area ID.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE               = TYPE_REASON_VAL(3,1090),
  /**<   This reason is returned when there is an RRC Connection 
         Establishment Failure because RF was unavailable.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_RRC_CONN_REL_ABORTED_IRAT_SUCCESS                 = TYPE_REASON_VAL(3,1091),
  /**<   This reason is returned after connection is aborted 
         before deactivating LTE stack due to a successful
         L->X IRAT (e.g. after IRAT handovers).
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period or
                              after WCDMA/GSM/TDSCDMA System Status
                              Indication is received */

  DS_NET_DOWN_REASON_RRC_CONN_REL_RLF_SEC_NOT_ACTIVE                   = TYPE_REASON_VAL(3,1092),
  /**<   If UE hits an LTE Radio Link Failure(RLF) 
         before security is established, 
         the connection needs to be released and UE needs to move
         back to idle. 
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_RRC_CONN_REL_IRAT_TO_LTE_ABORTED                  = TYPE_REASON_VAL(3,1093),
  /**<   Connection aborted by NAS right after a IRAT to LTE
         IRAT handover.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_CCO_SUCCESS       = TYPE_REASON_VAL(3,1094),
  /**<   This reason is returned after connection is aborted before
         deactivating LTE stack after a successful L->G IRAT Cell
         Change Order (CCO) procedure.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period or
                              after GSM/EDGE System Status Indication
                              is received by the client */

  DS_NET_DOWN_REASON_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_CCO_ABORTED       = TYPE_REASON_VAL(3,1095),
  /**<   This reason is returned after connection is aborted in 
         the middle of a L->G IRAT Cell Change Order (CCO)
         procedure.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period or
                              after GSM/EDGE System Status Indication
                              is received by the client */

  DS_NET_DOWN_REASON_IMSI_UNKNOWN_IN_HSS                               = TYPE_REASON_VAL(3,1096),
  /**<   This reason is returned when the IMSI present in UE 
         is unknown in HSS.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_IMEI_NOT_ACCEPTED                                 = TYPE_REASON_VAL(3,1097),
  /**<   This reason is returned when the IMEI of the UE is 
         not accepted by the network.
         Failure Type       : Permanent Failure
         Recovery Mechanism : None */

  DS_NET_DOWN_REASON_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED     = TYPE_REASON_VAL(3,1098),
  /**<   This reason is returned when Evolved Packet System(EPS) 
         and non-Evolved Packet System(non-EPS) Services are
         not allowed by the network.
         Failure Type       : Permanent Failure
         Recovery Mechanism : None */

  DS_NET_DOWN_REASON_EPS_SERVICES_NOT_ALLOWED_IN_PLMN                  = TYPE_REASON_VAL(3,1099),
  /**<   This reason is returned when Evolved Packet System(EPS) 
         Services are not allowed in the PLMN.
         Failure Type       : Permanent Failure
         Recovery Mechanism : None */

  DS_NET_DOWN_REASON_MSC_TEMPORARILY_NOT_REACHABLE                     = TYPE_REASON_VAL(3,1100),
  /**<   This reason is returned when the Mobile Switching Center 
         (MSC) is temporarily not reachable.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_CS_DOMAIN_NOT_AVAILABLE                           = TYPE_REASON_VAL(3,1101),
  /**<   This reason is returned if CS Domain is not available.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_ESM_FAILURE                                       = TYPE_REASON_VAL(3,1102),
  /**<   This reason is returned if there is an ESM level failure.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_MAC_FAILURE                                       = TYPE_REASON_VAL(3,1103),
  /**<   This reason is returned if there is a MAC level failure.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_SYNCH_FAILURE                                     = TYPE_REASON_VAL(3,1104),
  /**<   This reason is returned if there is a Synchronization failure.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_UE_SECURITY_CAPABILITIES_MISMATCH                 = TYPE_REASON_VAL(3,1105),
  /**<   This reason is returned if there is a UE Security 
         capabilities mismatch.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_SECURITY_MODE_REJ_UNSPECIFIED                     = TYPE_REASON_VAL(3,1106),
  /**<   This reason is returned if there is an unspecified 
         security mode reject.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_NON_EPS_AUTH_UNACCEPTABLE                         = TYPE_REASON_VAL(3,1107),
  /**<   This reason is returned if there is unacceptable
         non-EPS authentication. 
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_CS_FALLBACK_CALL_EST_NOT_ALLOWED                  = TYPE_REASON_VAL(3,1108),
  /**<   This reason is returned if CS Fallback call establishment 
         is not allowed.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_NO_EPS_BEARER_CONTEXT_ACTIVATED                   = TYPE_REASON_VAL(3,1109),
  /**<   This reason is returned if no Evolved packet System (EPS) 
         Bearer Context was activated.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_EMM_INVALID_STATE                                 = TYPE_REASON_VAL(3,1110),
  /**<   This reason is returned if call ended due to Invalid 
         EMM state.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

   DS_NET_DOWN_REASON_NAS_LAYER_FAILURE                                 = TYPE_REASON_VAL(3,1111),
  /**<   This reason is returned if there was a 
         Non-Access Spectrum(NAS) layer failure.
         Failure Type       : Temporary Failure
         Recovery Mechanism : Retry after a back-off time period */

  DS_NET_DOWN_REASON_DETACH_EPS_SERVICES_NOT_ALLOWED = TYPE_REASON_VAL(3,1118),
  /**< 
         This error code is returned when Network initiates a detach on LTE with 
         cause code 7 saying data plan has been replinished or has expired

         Failure Type       : Temporary Failure
         Recovery Mechanism : Client may retry on a new APN
  */

/* To map technology-agnostic call-end reasons from CM */
  DS_NET_DOWN_REASON_CLIENT_END               = TYPE_REASON_VAL(3,2000),
    /**<
        This error code is returned when client ends the data call
        Failure Type       : Not Applicable
        Recovery Mechanism : Not Applicable
    */  
  DS_NET_DOWN_REASON_NO_SRV                   = TYPE_REASON_VAL(3,2001),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since device  
        has no service
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_FADE                     = TYPE_REASON_VAL(3,2002),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since device  
        lost the system due to fade
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_REL_NORMAL               = TYPE_REASON_VAL(3,2003),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM due to  
        receiving a release from base station with no reason
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_ACC_IN_PROG              = TYPE_REASON_VAL(3,2004),
    /**<
      This error code is returned when data call is brought down because 
      traffic channel got rejected/released by CM due to Access 
      attempt already in progress
      Failure Type       : Temporary Failure
      Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_ACC_FAIL                 = TYPE_REASON_VAL(3,2005),
    /**<
      This error code is returned when data call is brought down because 
      traffic channel got rejected/released by CM due to Access Failure
      Failure Type       : Temporary Failure
      Recovery Mechanism : Retry call origination after some time
    */

  DS_NET_DOWN_REASON_REDIR_OR_HANDOFF         = TYPE_REASON_VAL(3,2006),
    /**< 
        This error code is returned when data call is brought down  
        because device is in the process of redirecting/handing off to  
        a different target system
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after device settles 
                             down on the new system
    */

/* To map some call cmd err reasons from CM */
  DS_NET_DOWN_REASON_OFFLINE                  = TYPE_REASON_VAL(3,2500),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since device  
        went offline
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after device goes  
                             online
    */
  DS_NET_DOWN_REASON_EMERGENCY_MODE           = TYPE_REASON_VAL(3,2501),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since device  
        is operating in Emergency mode
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after device comes  
                             out of emergency mode
    */
  DS_NET_DOWN_REASON_PHONE_IN_USE             = TYPE_REASON_VAL(3,2502),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since device  
        is in use (e.g voice call)
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_INVALID_MODE             = TYPE_REASON_VAL(3,2503),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since the  
        device's operational mode is different from the mode requested  
        in the traffic channel bringup
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after correcting the  
        device's mode preference
    */
  DS_NET_DOWN_REASON_INVALID_SIM_STATE        = TYPE_REASON_VAL(3,2504),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since the  
        SIM was marked by network as invalid for circuit and/or packet  
        service domain
        Failure Type       : Permanent Failure
        Recovery Mechanism : Check with network provider/check SIM  
                             validity
    */
  DS_NET_DOWN_REASON_NO_COLLOC_HDR            = TYPE_REASON_VAL(3,2505),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel got rejected/released by CM since there  
        is no colocated HDR
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_CALL_CONTROL_REJECTED    = TYPE_REASON_VAL(3,2506),
    /**< 
        This error code is returned when data call is brought down  
        because traffic channel request got rejected by CM since Call  
        control module rejected the request
        Failure Type       : Permanent Failure 
        Recovery Mechanism : Check UIM configuration
    */ 
  DS_NET_DOWN_REASON_EMM_DETACHED_PSM    = TYPE_REASON_VAL(3,2507),
    /**< 
        This error code is returned when data call is brought down  
        because the UE is entering power save mode.
        Failure Type       : Permanent Failure 
        Recovery Mechanism : Retry call origination after Psm is restored
    */ 

  DS_NET_DOWN_REASON_INVALID_CLASS3_APN  = TYPE_REASON_VAL(3,2508),
     /**<
        This error code is returned when data call is brought down  
        because UE is trying to bring up class3 APN name which is
        different from the APN chosen by the network
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */

    /*PPP error codes */
  DS_NET_DOWN_REASON_PPP_ERR_TIMEOUT                            = TYPE_REASON_VAL(7,1),
    /**< 
        This error code is returned when the data call bringup fails in  
        PPP setup due to timeout (For e.g: LCP Conf Ack not received  
        from network)
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after some time
    */
  DS_NET_DOWN_REASON_PPP_ERR_AUTH_FAILURE                       = TYPE_REASON_VAL(7,2),
    /**< 
        This error code is returned when the data call bringup fails in  
        PPP setup due to authentication failure
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after correctly  
                             provisioning the device with authentication 
                             credentials
    */
  DS_NET_DOWN_REASON_PPP_ERR_OPTION_MISMATCH                    = TYPE_REASON_VAL(7,3),
    /**< 
        This error code is returned when the data call bringup fails in  
        PPP setup due option mismatch (e.g: Auth is required, but not  
        negotiated with network during LCP phase)
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after comparing the  
                             PPP configurations in device and network
    */
  DS_NET_DOWN_REASON_PPP_ERR_PAP_FAILURE                        = TYPE_REASON_VAL(7,31),
    /**< 
        This error code is returned when the data call bringup fails in  
        PPP setup due to PAP failure
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after correctly  
                             provisioning the device with PAP 
                             credentials
    */
  DS_NET_DOWN_REASON_PPP_ERR_CHAP_FAILURE                       = TYPE_REASON_VAL(7,32),
    /**< 
        This error code is returned when the data call bringup fails in  
        PPP setup due to CHAP failure
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry call origination after correctly  
                             provisioning the device with CHAP 
                             credentials
    */
  DS_NET_DOWN_REASON_PPP_ERR_CLOSE_IN_PROGRESS                  = TYPE_REASON_VAL(7,33),
    /**< 
        This error code is returned when the data call bringup fails in  
        PPP setup since PPP is in the process of cleaning the previous 
        PPP session
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after sometime
    */
   DS_NET_DOWN_REASON_NV_REFRESH_IN_PROGRESS                  =   TYPE_REASON_VAL(7,34),
    /**< 
        This error code is returned when the data call bringup fails in  
        bring up phase while nv refresh is in progress.
        
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after sometime
    */

  DS_NET_DOWN_REASON_PPP_ERR_REASON_UNKNOWN                     = TYPE_REASON_VAL(7,65535),
    /**< 
        This error code is unused. Do not publish in DSS/QMI
        Failure Type       : Not Applicable
        Recovery Mechanism : Not Applicable
    */

  /* IPv6 error codes */
  DS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE                = TYPE_REASON_VAL(9,1),
    /**< 
        This error code is returned when V6 data call is brought down  
        because device failed to get the prefix from network
        Failure Type       : Temporary Failure
        Recovery Mechanism : Retry call origination after the V6  
                             throttling timer expires. Throttling timer 
                             is maintained in the profile
    */
  DS_NET_DOWN_REASON_IPV6_ERR_HRPD_IPV6_DISABLED                = TYPE_REASON_VAL(9,2),
    /**< 
        This error code is returned when V6 data call bring up is  
        rejected because IPV6 is disabled in 1X/HRPD mode
        Failure Type       : Permanent Failure
        Recovery Mechanism : Retry V6 call origination after enabling  
                             V6 on HRPD configuration (NV# 65677)
    */
  DS_NET_DOWN_REASON_IPV6_DISABLED                              = TYPE_REASON_VAL(9,3),
   /**< 
   This error code is returned when IPv6 data call bring up is rejected
   because NV1896 (IPV6 enable) is disabled
   Failure Type : Permanent Failure
   Recovery Mechanism : Retry call origination after enabling NV1896
   */ 
  DS_NET_DOWN_REASON_IPV6_ERR_MAX,

  DS_NET_DOWN_REASON_MAX             /* DO NOT USE. Used for bounds check */
} ds_net_down_reason_type;

/*---------------------------------------------------------------------------
TYPEDEF DS_ADDR_FAMILY_TYPE

DESCRIPTION
  The type that is used to define the IP type that an interface supports.
---------------------------------------------------------------------------*/
typedef ip_addr_enum_type ds_addr_family_type;

/*---------------------------------------------------------------------------
  APN type enum
---------------------------------------------------------------------------*/
typedef enum
{
  DS_APN_TYPE_UNSPECIFIED  = 0,
  DS_APN_TYPE_IMS          = 1
}ds_apn_enum_type;


/*  @brief IP Configuration change mask values.
         These values are set and propagated along with the following event:
         PDN_EXTENDED_IP_CONFIG_EX_EV
         The various mask values indicate which IP Config parmeter has been 
         updated.
*/
/**< No change in extended ip config */
#define DS_EXT_IP_CFG_MASK_NONE             (0x00)

/**< This mask values is set when DNS server IP address changes as compared 
     to previously configured value */
#define DS_EXT_IP_CFG_MASK_DNS_ADDR         (0x10)

/**< This mask value is set when Gateway address changes compared to 
     previously configured value */
#define DS_EXT_IP_CFG_MASK_GATEWAY          (0x0200)

/**< This mask value is set when PCSCF server's IP address changes compared
     to previously configured value */
#define DS_EXT_IP_CFG_MASK_PCSCF_ADDR       (0x0800)

/**< This mask value is set when PCSCF server's URL changes compared to 
     previously configured value */
#define DS_EXT_IP_CFG_MASK_PCSCF_NAME       (0x1000)

/**< This mask value is set when IP MTU value changes compared to 
     previously configured value */
#define DS_EXT_IP_CFG_MASK_MTU              (0x2000)

/**< This mask value is set when DNS server URL changes compared to 
     previously configured value */
#define DS_EXT_IP_CFG_MASK_DNS_URL_LIST     (0x4000)

/**< This mask value is set when Operator reserved PCO flag changes compared
     to previously configured value */
#define DS_EXT_IP_CFG_MASK_OP_RES_PCO       (0x40000)

/**< This mask value is set when MSISDN PCO changes compared to 
     previously configured value */
#define DS_EXT_IP_CFG_MASK_MSISDN_PCO       (0x80000)


/**
  @brief Extended IP config EX event data : PDN_EXTENDED_IP_CONFIG_EX_EV
         Mask value indicating what all config items have changed.
         This is an extension to the extended ip config event which only 
         denotes whether or not config is fetched.
*/
typedef uint32 ds_ioctl_extended_ip_config_ex_type;


/*---------------------------------------------------------------------------
TYPEDEF PS_PDN_PREFIX_UPDATE_ENUM_TYPE

DESCRIPTION
  Contains the enums describing the action for the accompanying prefix.
---------------------------------------------------------------------------*/

typedef enum
{
  DS_PREFIX_ADDED      = 0, /* Added a prefix          */
  DS_PREFIX_REMOVED    = 1, /* Removed a prefix        */
  DS_PREFIX_DEPRECATED = 2, /* State of prefix changed */
  DS_PREFIX_UPDATED    = 3  /* Lifetimes of prefix have been updated */
} ds_prefix_update_enum_type;


/*---------------------------------------------------------------------------
TYPEDEF DS_IPV6_ADDR_STATE_ENUM_TYPE

DESCRIPTION
  State of the IPv6 address.
---------------------------------------------------------------------------*/
typedef enum
{
  // No address has been created. An invalid entry.
  DS_IPV6_ADDR_STATE_INVALID    = 0,
  // This address is pending DAD verification
  DS_IPV6_ADDR_STATE_TENTATIVE  = 1,
  // This address has been verified (DAD/local) and is ready to be allocated
  DS_IPV6_ADDR_STATE_UNASSIGNED = 2,
  // This address has been assigned.
  DS_IPV6_ADDR_STATE_VALID      = 3,
  // This address has been deprecated
  DS_IPV6_ADDR_STATE_DEPRECATED = 4
} ds_ipv6_addr_state_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF DS_IPV6_ADDR_TYPE_ENUM_TYPE

DESCRIPTION
  The type of the IPv6 address. Public/Private, etc.
---------------------------------------------------------------------------*/
typedef enum
{
  // This is either a PUBLIC address or unused
  DS_IPV6_ADDR_TYPE_INVALID          = 0,
  // This address is a public address
  DS_IPV6_ADDR_TYPE_PUBLIC           = 1,
  // This address is an in use private shareable address
  DS_IPV6_ADDR_TYPE_PRIV_SHARED      = 2,
  // This address is an in use private unique address
  DS_IPV6_ADDR_TYPE_PRIV_UNIQUE      = 3,
  // This address is in use by an external device
  DS_IPV6_ADDR_TYPE_EXTERNAL         = 4
} ds_ipv6_addr_type_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_PDN_PREFIX_UPDATE_TYPE

DESCRIPTION
  This contains all of the prefix information related to the update.

MEMBERS:
  prefix: the prefix that changed.
  n_prefixes: the number of prefixes
  kind: added, changed, state change (i.e. preferred -> deprecated)
---------------------------------------------------------------------------*/
typedef struct
{
  struct ps_in6_addr prefix;
  ds_prefix_update_enum_type kind;
  uint8 prefix_len;
} ds_prefix_update_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_PDN_PREFIX_INFO_TYPE

DESCRIPTION
  This contains all of the prefix information.

MEMBERS:
  prefix:       the prefix that changed
  prefix_state: the state of the prefix
  prefix_len:   the length of the prefix
---------------------------------------------------------------------------*/
typedef struct
{
  struct ps_in6_addr           prefix;
  ds_ipv6_addr_state_enum_type prefix_state;
  uint8                        prefix_len;
} ds_prefix_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_PDN_PRIV_IPV6_ADDR_INFO_TYPE

DESCRIPTION
  This contains the private IPv6 address and type.

MEMBERS:
  ip_addr: the IPv6 privacy address.
  is_unique: whether the address is private shared or private unique
---------------------------------------------------------------------------*/
typedef struct
{
  ps_ip_addr_type   ip_addr;
  boolean           is_unique;
} ds_priv_ipv6_addr_info_type;

/**
  @brief Handle to network initiated qos request.
*/
typedef uint32 ds_net_initiated_qos_req_handle_type;

typedef uint32 ds_qos_handle_type;

/**
  @brief Event info for PDN_NET_INITIATED_QOS_AVAILABLE_EV
*/
typedef struct
{
   /**< Flow, whose filters satisfy the net initiated qos request */
  ds_qos_handle_type                     qos_handle;
  /**< Handle to QOS instance */
   ds_net_initiated_qos_req_handle_type  req_handle;
} ds_net_initiated_qos_event_info_type;

/*
  Event info associated with PDN_OUT_OF_USE_EV/PDN_DOWN_EV.
*/
  
typedef struct
{
  ds_net_down_reason_type   netdown_reason;
  ip_addr_enum_type         addr_family;
} ds_net_down_event_info_type;

/**
  @brief Argument to be passed with 
         PS_PDN_IOCTL_REVERSE_IP_TRANSPORT_GET_IPFLTRS.
*/
typedef struct
{
  uint8                   list_ptr_size;  /* Size in num filters for memory allocated for list_ptr */       
  ip_filter_spec_type     filters;
} ds_ipfltr_list_type;

typedef struct
{
  char apn_name[DS_SYS_MAX_APN_LEN];
  /**< Container to store APN name*/

  uint32 pdp_type;
  /**< Container to store PDN's IP Type */

} ds_apn_param_value_type; 

/**
  @brief Argument to be passed with PDN_APN_PARAM_CHANGED_EV and 
*/
typedef struct
{
  uint32 apn_mask;
  /**< Apn Mask to specify which parameter changed*/
  ds_apn_param_value_type old_apn_param_values;
   /**< Old Apn Parameter Information*/
  ds_apn_param_value_type new_apn_param_values;
  /**< New Apn Parameter Information */
} ds_apn_param_change_info_type;

/*---------------------------------------------------------------------------
  Domain name type. Used for SIP/DNS server domain names.
---------------------------------------------------------------------------*/
typedef struct
{
  char domain_name[DS_MAX_DOMAIN_NAME_SIZE];
} ds_domain_name_type;

typedef struct
{
  q_link_type link;
    /**< Link, must be the first member        */
  uint64  prefix;                                 
    /**< IPv6 prefix.           */
  uint64  iid;                                    
    /**< IPv6 IID.              */
  uint64  gateway_iid;                            
    /**< Gateway from which the prefix is acquired.       */
  int64 pref_lifetime;    
    /**< Preferred lifetime.    */
  int64 valid_lifetime;    
    /**< Valid lifetime.        */
  int64  unused_addr_time; 
    /**< Timer for the duration of time a privacy address can remain unused. If 
         no application binds before the timer expires, the address is freed. */
  ds_ipv6_addr_type_enum_type   addr_type;
    /**< IPv6 address type.     */
} ds_v6_addr_type;

/*---------------------------------------------------------------------------
TYPEDEF DS_PDN_EVENT_INFO_U_TYPE

DESCRIPTION
  Data structure that is used to pass event specific information into the
  callback functions that are registered for the events.
---------------------------------------------------------------------------*/
typedef union
{

  ds_priv_ipv6_addr_info_type               priv_ipv6_addr;
  ds_prefix_update_type                     prefix_info;
  ps_ip_addr_type                           ip_addr;
  ds_ioctl_extended_ip_config_ex_type       extended_ip_config_change_mask;
  ds_net_down_event_info_type               pdn_down_info;  
  ds_net_initiated_qos_event_info_type      net_initiated_qos_info;
  uint16                                    ra_lifetime;
  ds_apn_param_change_info_type             apn_param_change_info_type;
  boolean                                   tech_suspension_status;
  uint32                                    bearer_flow_mask;
  /**< Event info associated with PDN_APN_PARAM_CHANGED_EV*/
} ds_pdn_event_info_u_type;

#endif /* DS_DEFS_H */

