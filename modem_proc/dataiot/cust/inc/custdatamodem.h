#ifndef CUSTDATAMODEM_H
#define CUSTDATAMODEM_H
/*===========================================================================
 
DESCRIPTION
  ** DATA SERVICES CUST FILE (DATA MODEM) **

  Copyright (c) 2018 - 2022  by Qualcomm Technologies Incorporated.
  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/19/22   rj      FR 78240: Memory optimizations- Disable DHCPv6, PPP, HDLC and SSL

===========================================================================*/

/*===========================================================================
** DATA SERVICES CUST FILE (DATA MODEM)
** - Hosts all features for all Technologies: 
** - Hosts all common protocol, interface features

** ::Following Grouping/Ordering is Maintained::
   ---------------------------------------------
** -==> Data High Level Features
** -==> Data Interface
** -==> Data Protocols
** -==> Data 3GPP
** -==> Data Debug/Test Feature Set
===========================================================================*/

/*---------------------------------------------------------------------------
  Data High Level Features (Features common to all packages)
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Data Interface - Start
---------------------------------------------------------------------------*/
#define FEATURE_DATA_PS_SYSTEM_HEAP
#define FEATURE_DATA_PS_ADDR_MGMT
#define FEATURE_DATA_PS_QOS
#define FEATURE_DATA_QMI_QOS
#define FEATURE_DATA_QMI_AT
#define FEATURE_DATA_ATCOP

//Enable A2 features only if IPA is not defined!
#define FEATURE_DATA_A2
//#define FEATURE_DSAT_EXT_CLIENT_SUPPORT

#define FEATURE_DSAT_ATCOP_VEND

#define FEATURE_DATA_QMI_RESP_FILTERING
#define FEATURE_DATA_QMI_SVC_AVAILABILITY
#define FEATURE_DSAT_HIGH_TIER
//#define FEATURE_DATA_QCRMCALL
#define PS_LOGGING_IOE_DPL

/*---------------------------------------------------------------------------
  Data Interface - End
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Data Protocols - Start
---------------------------------------------------------------------------*/
#ifndef FEATURE_AWARE_CT21
  #define FEATURE_DATA_PS_PPP
  #define FEATURE_DATA_PS_PPP_LOGGING
  #define FEATURE_DATA_PS_HDLC
#endif

/* Enable PPP for DUN call on NTN only flavor*/
#ifdef FEATURE_NBIOT_NTN
  #define FEATURE_DATA_PS_PPP
  #define FEATURE_DATA_PS_PPP_LOGGING
  #define FEATURE_DATA_PS_HDLC
#endif

#define FEATURE_DATA_PS_ROHC
#define FEATURE_SEC_IPSEC
#define FEATURE_DATA_PS_IPV6
#define FEATURE_RUIM_FUNC_INTERFACE
#undef FEATURE_DATA_PS_IPHC

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*---------------------------------------------------------------------------
  Data Protocols - End
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Data 3GPP - Start
---------------------------------------------------------------------------*/
#define FEATURE_DATA_IMS

#define FEATURE_MODE_TLB

#define FEATURE_DATA_CC_SP_THROTTLING
/*Control CARD PUP/DOWN as part of +CFUN command*/
#define FEATURE_DSAT_CFUN_CARD_POWER_CTL

#define FEATURE_DSAT_ETSI_RLP2

#define FEATURE_DSAT_KEYPAD_EMULATION
#define FEATURE_DATA_ETSI_PIN
#define FEATURE_DATA_RPM

#define FEATURE_DSAT_DEV_CMDS

#define FEATURE_DSAT_ETSI_DATA
#define FEATURE_DATA_TEST_LOOPBACK
#define FEATURE_DATA_3GPP

/* LTE related ATCoP feature wrapping */
#define FEATURE_MMGSDI_ATCSIM
#define FEATURE_DSAT_LTE
#define FEATURE_DSAT_ETSI_MODE
#define FEATURE_SUPPORT_TFT_ON_DEFAULT_BEARER

/* Enables SMS for ATCOP
*/
#define FEATURE_ETSI_SMS_TEMPLATE
#define FEATURE_ETSI_ATTACH
#define FEATURE_ETSI_SMS
#define FEATURE_ETSI_SMS_CB
#define FEATURE_ETSI_SMS_PS

#define FEATURE_DATA_REL10
#define FEATURE_DATA_REL11
#define FEATURE_DSAT_EDRX
/*---------------------------------------------------------------------------
  Data 3GPP - End
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Data Debug/Test Features
---------------------------------------------------------------------------*/
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/* Support PDCP SIM for loop back in off target environment */
#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST
#define FEATURE_DATA_TEST_LOOPBACK_PDCPSIM
#endif

#endif /* CUSTDATAMODEM_H */
