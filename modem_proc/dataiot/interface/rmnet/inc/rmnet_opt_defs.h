#ifndef RMNET_OPT_DEFS_H
#define RMNET_OPT_DEFS_H
/*===========================================================================

                     RMNET_OPT_DEFS . H
DESCRIPTION
  Header file describing all Optimized data path definitions used by
    both UL/DL.

Copyright (c) 2020 - 2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/dataiot.mpss/2.0/interface/rmnet/inc/rmnet_opt_defs.h#4 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/27/13    wc     Support dynamic port configuration
01/29/13    vb     Fix to handle the GRE packet processing in
                   NAT optimized path.
09/14/12    tw     IPv6 prefix missmatch errors fixed
05/24/2012  pgm    RNDIS defns.
05/07/2012  mp     Fix to send TCP Reset for exhausted NAT entry.
08/15/2011  AM     created module.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

//#include "ps_ifacei.h"
#include  "ps_rm_taski.h"


#define RMNET_UL_OPT_PATH_WORD_SIZE          4
#define RMNET_UL_OPT_PATH_WORD_SHIFT         2
#define RMNET_UL_OPT_FIRST_BYTE              1

/*-----------------------------------------------------------------------
    ETHERNET HDR
 -----------------------------------------------------------------------*/
#define RMNET_OPT_ETH_HEADER_OFFSET  14
#define RMNET_OPT_ETHERTYPE_OFFSET   12

/*-----------------------------------------------------------------------
    IP HDR
 -----------------------------------------------------------------------*/
#define RMNET_OPT_IP_VERSION_MASK    0xF0
#define RMNET_OPT_IP_HDR_LEN_MASK    0x0F

#define RMNET_OPT_QOS_HEADER_OFFSET  6
#define RMNET_OPT_QOS2_HEADER_OFFSET 8

/*-----------------------------------------------------------------------
    IPV4
 -----------------------------------------------------------------------*/
#define RMNET_OPT_V4_VERSION           0x40
#define RMNET_OPT_V4_HDR_LEN           20
#define RMNET_OPT_MAX_V4_HDR_LEN       60
#define RMNET_OPT_V4_TOTAL_LEN_OFFSET  2
#define RMNET_OPT_V4_FRAG_OFFSET       6
#define RMNET_OPT_V4_PROT_FIELD_OFFSET 9
#define RMNET_OPT_V4_SRC_ADDR_OFFSET   12
#define RMNET_OPT_V4_DEST_ADDR_OFFSET  16
#define RMNET_OPT_V4_FRAG_MASK         0x3FFF
#define RMNET_OPT_V4_ADDRESS_LEN       4
#define RMNET_OPT_V4_SRC_PORT_OFFSET   20
#define RMNET_OPT_V4_DST_PORT_OFFSET   22
#define RMNET_OPT_V4_TCP_FLAGS_OFFSET  33
#define RMNET_OPT_V4_TCP_CKSUM_OFFSET  36
#define RMNET_OPT_V4_TCP_SEQNO_OFFSET  24
#define RMNET_OPT_V4_UDP_CKSUM_OFFSET  26
#define RMNET_OPT_V4_GRE_CALLID_OFFSET 26
#define RMNET_OPT_V4_ID_OFFSET         32
#define RMNET_OPT_V4_ID_LEN            16
#define RMNET_OPT_V4_FRAG_OFFSET_MASK  0xFF1F
#define RMNET_OPT_ICMP_TYPE_OFFSET     0 
#define RMNET_OPT_ICMP_CODE_OFFSET     1
#define RMNET_OPT_ICMP_HDR_LEN_EXTRACT 2

/*-----------------------------------------------------------------------
    IPV6
 -----------------------------------------------------------------------*/
#define RMNET_OPT_V6_VERSION           0x60
#define RMNET_OPT_V6_HDR_LEN           40
#define RMNET_OPT_V6_TOTAL_LEN_OFFSET  4
#define RMNET_OPT_V6_NXT_HDR_OFFSET    6
#define RMNET_OPT_V6_DEST_ADDR_OFFSET  24
#define RMNET_OPT_V6_SRC_ADDR_OFFSET   8
#define RMNET_OPT_V6_MCAST_MASK        0xFF
#define RMNET_OPT_V6_FRAG_OFFSET_MASK 0xF8FF
#define RMNET_OPT_ICMP6_TYPE_OFFSET    0
#define RMNET_OPT_ICMP6_CODE_OFFSET    1
#define RMNET_OPT_FRAG_NEXT_HDR_OFFSET 0
#define RMNET_OPT_FRAG_FRAG_OFFSET     2
#define RMNET_OPT_ICMP6_HDR_LEN_EXTRACT  4
#define RMNET_OPT_V6_OPT_HDR_LEN       8

#define RMNET_OPT_PORT_INFO_LEN            4
#define RMNET_OPT_EXP_HEADER_LEN_DL                                     \
    ( RMNET_OPT_MAX_V4_HDR_LEN + RMNET_OPT_PORT_INFO_LEN )

/*-----------------------------------------------------------------------
    TCP
 -----------------------------------------------------------------------*/
#define RMNET_OPT_TCP_HDR_LEN_OFFSET   12
#define RMNET_OPT_TCP_HDR_LEN_EXTRACT  13
#define RMNET_OPT_TCP_HDR_LEN_MASK     0xF0
#define RMNET_OPT_TCP_RST_FIN_SET      0x05
#define RMNET_OPT_TXPRT_PORT_LEN       2

#define RMNET_OPT_FTP_CONTROL_PORT       21



#define RMNET_OPT_UDP_HDR_LEN          8

/*-----------------------------------------------------------------------
   HEADER LEN FOR ALL MODES
   PS_OPT_PATH_IP_NO_QOS_HEADER_LEN: IP(40) + TCP Header len(13)
   PS_OPT_PATH_IP_QOS_HEADER_LEN: QOS(6) + IP(40) + TCP Header len(13)
   PS_OPT_PATH_ETH_NO_QOS_HEADER_LEN:IP(40)+ETH(14) +
                                           TCP Header len(13)
   PS_OPT_PATH_ETH_QOS_HEADER_LEN:QOS(6)+IP(40)+ETH(14) +
                                           TCP Header len(13)
 -----------------------------------------------------------------------*/
#define PS_OPT_PATH_IP_NO_QOS_HEADER_LEN         \
   (RMNET_OPT_V6_HDR_LEN +                     \
    RMNET_OPT_TCP_HDR_LEN_EXTRACT)

#define PS_OPT_PATH_IP_QOS_HEADER_LEN            \
   (RMNET_OPT_V6_HDR_LEN +                     \
    RMNET_OPT_QOS_HEADER_OFFSET +              \
    RMNET_OPT_TCP_HDR_LEN_EXTRACT)

#define PS_OPT_PATH_IP_QOS2_HEADER_LEN          \
   (RMNET_OPT_V6_HDR_LEN +                     \
    RMNET_OPT_QOS2_HEADER_OFFSET +             \
    RMNET_OPT_TCP_HDR_LEN_EXTRACT)

#define PS_OPT_PATH_ETH_NO_QOS_HEADER_LEN        \
  (RMNET_OPT_V6_HDR_LEN +                      \
   RMNET_OPT_ETH_HEADER_OFFSET +               \
   RMNET_OPT_TCP_HDR_LEN_EXTRACT)

#define PS_OPT_PATH_ETH_QOS_HEADER_LEN            \
  (RMNET_OPT_V6_HDR_LEN        +                \
   RMNET_OPT_QOS_HEADER_OFFSET +                \
   RMNET_OPT_ETH_HEADER_OFFSET +                \
   RMNET_OPT_TCP_HDR_LEN_EXTRACT)

/*-----------------------------------------------------------------------
    HEADER LEN CHECKS FOR DSM_EXTRACT
 -----------------------------------------------------------------------*/
#define PS_RMNET_OPT_IP_NO_QOS_HEADER_LEN_CHECK    \
   RMNET_OPT_V6_HDR_LEN

#define PS_RMNET_OPT_IP_QOS_HEADER_LEN_CHECK       \
  (RMNET_OPT_V6_HDR_LEN +                       \
   RMNET_OPT_QOS_HEADER_OFFSET)

#define PS_RMNET_OPT_IP_QOS2_HEADER_LEN_CHECK      \
  (RMNET_OPT_V6_HDR_LEN +                       \
   RMNET_OPT_QOS2_HEADER_OFFSET)

#define PS_RMNET_OPT_ETH_NO_QOS_HEADER_LEN_CHECK   \
  (RMNET_OPT_V6_HDR_LEN +                       \
   RMNET_OPT_ETH_HEADER_OFFSET)

#define PS_RMNET_OPT_ETH_QOS_HEADER_LEN_CHECK      \
  (RMNET_OPT_V6_HDR_LEN        +                \
   RMNET_OPT_QOS_HEADER_OFFSET +                \
   RMNET_OPT_ETH_HEADER_OFFSET)

#define PS_RMNET_UL_OPT_V4_IP_ICMP_LEN_CHECK       \
  ( RMNET_OPT_V4_HDR_LEN +                      \
    RMNET_OPT_ICMP_HDR_LEN_EXTRACT )

#define PS_RMNET_UL_OPT_V6_IP_ICMP6_LEN_CHECK       \
  ( RMNET_OPT_V6_HDR_LEN +                      \
    RMNET_OPT_ICMP6_HDR_LEN_EXTRACT )

/*-----------------------------------------------------------------------
    MBIM HEADER
 -----------------------------------------------------------------------*/

/*NTH COMMON HEADER*/
#define PS_OPT_PATH_NTHC_LEN      8
#define PS_OPT_PATH_NTHS_LEN      4

/*NDP HEADER*/
#define PS_OPT_PATH_NDP_HDR_LEN           8
#define PS_OPT_PATH_NDP_DG_OFFSET         4
#define PS_OPT_PATH_NDP_MAX_DG            10
#define PS_OPT_PATH_NDP_HDR_FIELD_OFFSET  4
#define PS_OPT_PATH_NDP_HDR_FIELD_LEN     2
#define PS_OPT_PATH_NDP_LEN_CHECK         16
#define PS_OPT_PATH_NDP_IP_OFF_LEN_PAIR_LEN          \
        (PS_OPT_PATH_NDP_MAX_DG * PS_OPT_PATH_NDP_DG_OFFSET)
#define PS_OPT_PATH_MAX_ST_NDP_LEN                   \
        (PS_OPT_PATH_NDP_IP_OFF_LEN_PAIR_LEN +       \
         PS_OPT_PATH_NDP_HDR_LEN)

#define RMNET_UL_OPT_IP_VERSION_MASK  0xF0000000
#define RMNET_UL_OPT_IP_PKT_LEN_MASK  0x0000FFFF
#define PRMNET_UL_OPT_IPV4_VERSION     0x40000000
#define RMNET_UL_OPT_IPV6_VERSION     0x60000000

/*---------------------------------------------------------------------------
  ps optimized hdlr path types
  PS_DPM_UL_OPT_NO_PATH: Error cases, packet drops, Flow_controlled
  PS_DPM_UL_OPT_OPT_PATH: Um tx_cmd
  PS_DPM_UL_OPT_QOS_PATH: When Filters installed-ps_iface_tx_cmd()
  PS_DPM_UL_OPT_LEGACY_PATH: fallback_rx_sig_hdlr()
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_UL_OPT_NO_PATH            = 0,
  RMNET_UL_OPT_OPT_PATH           = 1,
  RMNET_UL_OPT_QOS_PATH           = 2,
  RMNET_UL_OPT_LEGACY_PATH        = 3,
  RMNET_UL_OPT_FLOW_CONTROL_PATH  = 4
} rmnet_ul_opt_path_type;

extern uint8 wwan_tech_suspend_cntr;

/*===========================================================================
FUNCTION RMNET_UL_OPT_INIT

DESCRIPTION


PARAMETERS


RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void rmnet_ul_opt_init
(
  void
);


/*===========================================================================
FUNCTION RMNET_UL_OPT_IP_NO_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for IP NO QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean rmnet_ul_opt_ip_no_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                  *user_data
);

#ifdef FEATURE_DATA_QMI_QOS
boolean rmnet_ul_opt_ip_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,  
  void                  *user_data
);
#endif

#ifdef FEATURE_DATA_PS_464XLAT
/*===========================================================================
FUNCTION RMNET_UL_OPT_CLAT_IP_NO_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for IP NO QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean rmnet_ul_opt_clat_ip_no_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                  *user_data
);
#endif

#endif /*RMNET_OPT_DEFS_H*/
