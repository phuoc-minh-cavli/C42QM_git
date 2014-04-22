#ifndef DS_FWK_PATH_DEFS_H
#define DS_FWK_PATH_DEFS_H
/*===========================================================================

                     DS_FWK_OPT_DEFS . H
DESCRIPTION
  Header file describing all Optimized data path definitions used by
    both UL/DL.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/dataiot.mpss/2.0/interface/framework/inc/ds_fwk_opt_defs.h#1 $

when        who        what, where, why
--------    ---    ----------------------------------------------------------
03/05/18  svaka      created module

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_headers.h"
#include "ds_defsi.h"
#include "ps_tx_meta_info.h" 

#define DS_FWK_UL_OPT_PATH_WORD_SIZE          4
#define DS_FWK_UL_OPT_PATH_WORD_SHIFT         2

/*-----------------------------------------------------------------------
    IP HDR
 -----------------------------------------------------------------------*/
#define DS_FWK_OPT_IP_VERSION_MASK    0xF0
#define DS_FWK_OPT_IP_HDR_LEN_MASK    0x0F

#define DS_FWK_OPT_QOS_HEADER_OFFSET  6
#define DS_FWK_OPT_QOS2_HEADER_OFFSET 8

/*-----------------------------------------------------------------------
    IPV4
 -----------------------------------------------------------------------*/
#define DS_FWK_OPT_V4_VERSION           0x40
#define DS_FWK_OPT_V4_HDR_LEN           20
#define DS_FWK_OPT_MAX_V4_HDR_LEN       60
#define DS_FWK_OPT_V4_TOTAL_LEN_OFFSET  2
#define DS_FWK_OPT_V4_FRAG_OFFSET       6
#define DS_FWK_OPT_V4_PROT_FIELD_OFFSET 9
#define DS_FWK_OPT_V4_SRC_ADDR_OFFSET   12
#define DS_FWK_OPT_V4_DEST_ADDR_OFFSET  16
#define DS_FWK_OPT_V4_FRAG_MASK         0x3FFF
#define DS_FWK_OPT_V4_ADDRESS_LEN       4
#define DS_FWK_OPT_V4_SRC_PORT_OFFSET   20
#define DS_FWK_OPT_V4_DST_PORT_OFFSET   22
#define DS_FWK_OPT_V4_TCP_FLAGS_OFFSET  33
#define DS_FWK_OPT_V4_TCP_CKSUM_OFFSET  36
#define DS_FWK_OPT_V4_TCP_SEQNO_OFFSET  24
#define DS_FWK_OPT_V4_UDP_CKSUM_OFFSET  26
#define DS_FWK_OPT_V4_GRE_CALLID_OFFSET 26
#define DS_FWK_OPT_V4_ID_OFFSET         32
#define DS_FWK_OPT_V4_ID_LEN            16
#define DS_FWK_OPT_V4_FRAG_OFFSET_MASK  0xFF1F
#define DS_FWK_OPT_ICMP_TYPE_OFFSET     0 
#define DS_FWK_OPT_ICMP_CODE_OFFSET     1
#define DS_FWK_OPT_ICMP_HDR_LEN_EXTRACT 2

/*-----------------------------------------------------------------------
    IPV6
 -----------------------------------------------------------------------*/
#define DS_FWK_OPT_V6_VERSION           0x60
#define DS_FWK_OPT_V6_HDR_LEN           40
#define DS_FWK_OPT_V6_TOTAL_LEN_OFFSET  4
#define DS_FWK_OPT_V6_NXT_HDR_OFFSET    6
#define DS_FWK_OPT_V6_DEST_ADDR_OFFSET  24
#define DS_FWK_OPT_V6_SRC_ADDR_OFFSET   8
#define DS_FWK_OPT_V6_MCAST_MASK        0xFF
#define DS_FWK_OPT_V6_FRAG_OFFSET_MASK 0xF8FF
#define DS_FWK_OPT_ICMP6_TYPE_OFFSET    0
#define DS_FWK_OPT_ICMP6_CODE_OFFSET    1
#define DS_FWK_OPT_FRAG_NEXT_HDR_OFFSET 0
#define DS_FWK_OPT_FRAG_FRAG_OFFSET     2
#define DS_FWK_OPT_ICMP6_HDR_LEN_EXTRACT  4
#define DS_FWK_OPT_V6_OPT_HDR_LEN       8

#define DS_FWK_OPT_PORT_INFO_LEN            4
#define DS_FWK_OPT_EXP_HEADER_LEN_DL                                     \
    ( DS_FWK_OPT_MAX_V4_HDR_LEN + DS_FWK_OPT_PORT_INFO_LEN )

/*-----------------------------------------------------------------------
    TCP
 -----------------------------------------------------------------------*/
#define DS_FWK_OPT_TCP_HDR_LEN_OFFSET   12
#define DS_FWK_OPT_TCP_HDR_LEN_EXTRACT  13
#define DS_FWK_OPT_TCP_HDR_LEN_MASK     0xF0
#define DS_FWK_OPT_TCP_RST_FIN_SET      0x05
#define DS_FWK_OPT_TXPRT_PORT_LEN       2

#define DS_FWK_OPT_FTP_CONTROL_PORT       21



#define DS_FWK_OPT_UDP_HDR_LEN          8

/*-----------------------------------------------------------------------
   HEADER LEN FOR ALL MODES
   DS_FWK_PATH_IP_NO_QOS_HEADER_LEN: IP(40) + TCP Header len(13)
   DS_FWK_PATH_IP_QOS_HEADER_LEN: QOS(6) + IP(40) + TCP Header len(13)
   DS_FWK_PATH_ETH_NO_QOS_HEADER_LEN:IP(40)+ETH(14) +
                                           TCP Header len(13)
   DS_FWK_PATH_ETH_QOS_HEADER_LEN:QOS(6)+IP(40)+ETH(14) +
                                           TCP Header len(13)
 -----------------------------------------------------------------------*/
#define DS_FWK_PATH_IP_NO_QOS_HEADER_LEN         \
   (DS_FWK_OPT_V6_HDR_LEN +                     \
    DS_FWK_OPT_TCP_HDR_LEN_EXTRACT)

#define DS_FWK_PATH_IP_QOS_HEADER_LEN            \
   (DS_FWK_OPT_V6_HDR_LEN +                     \
    DS_FWK_OPT_QOS_HEADER_OFFSET +              \
    DS_FWK_OPT_TCP_HDR_LEN_EXTRACT)

#define DS_FWK_PATH_IP_QOS2_HEADER_LEN          \
   (DS_FWK_OPT_V6_HDR_LEN +                     \
    DS_FWK_OPT_QOS2_HEADER_OFFSET +             \
    DS_FWK_OPT_TCP_HDR_LEN_EXTRACT)

#define DS_FWK_PATH_ETH_NO_QOS_HEADER_LEN        \
  (DS_FWK_OPT_V6_HDR_LEN +                      \
   DS_FWK_OPT_ETH_HEADER_OFFSET +               \
   DS_FWK_OPT_TCP_HDR_LEN_EXTRACT)

#define DS_FWK_PATH_ETH_QOS_HEADER_LEN            \
  (DS_FWK_OPT_V6_HDR_LEN        +                \
   DS_FWK_OPT_QOS_HEADER_OFFSET +                \
   DS_FWK_OPT_ETH_HEADER_OFFSET +                \
   DS_FWK_OPT_TCP_HDR_LEN_EXTRACT)

/*-----------------------------------------------------------------------
    HEADER LEN CHECKS FOR DSM_EXTRACT
 -----------------------------------------------------------------------*/
#define DS_FWK_OPT_IP_NO_QOS_HEADER_LEN_CHECK    \
   DS_FWK_OPT_V6_HDR_LEN

#define DS_FWK_OPT_IP_QOS_HEADER_LEN_CHECK       \
  (DS_FWK_OPT_V6_HDR_LEN +                       \
   DS_FWK_OPT_QOS_HEADER_OFFSET)

#define DS_FWK_OPT_IP_QOS2_HEADER_LEN_CHECK      \
  (DS_FWK_OPT_V6_HDR_LEN +                       \
   DS_FWK_OPT_QOS2_HEADER_OFFSET)

#define DS_FWK_UL_OPT_V4_IP_ICMP_LEN_CHECK       \
  ( DS_FWK_OPT_V4_HDR_LEN +                      \
    DS_FWK_OPT_ICMP_HDR_LEN_EXTRACT )

#define DS_FWK_UL_OPT_V6_IP_ICMP6_LEN_CHECK       \
  ( DS_FWK_OPT_V6_HDR_LEN +                      \
    DS_FWK_OPT_ICMP6_HDR_LEN_EXTRACT )

/*-----------------------------------------------------------------------
    MBIM HEADER
 -----------------------------------------------------------------------*/

/*NTH COMMON HEADER*/
#define DS_FWK_PATH_NTHC_LEN      8
#define DS_FWK_PATH_NTHS_LEN      4

/*NDP HEADER*/
#define DS_FWK_PATH_NDP_HDR_LEN           8
#define DS_FWK_PATH_NDP_DG_OFFSET         4
#define DS_FWK_PATH_NDP_MAX_DG            10
#define DS_FWK_PATH_NDP_HDR_FIELD_OFFSET  4
#define DS_FWK_PATH_NDP_HDR_FIELD_LEN     2
#define DS_FWK_PATH_NDP_LEN_CHECK         16
#define DS_FWK_PATH_NDP_IP_OFF_LEN_PAIR_LEN          \
        (DS_FWK_PATH_NDP_MAX_DG * DS_FWK_PATH_NDP_DG_OFFSET)
#define DS_FWK_PATH_MAX_ST_NDP_LEN                   \
        (DS_FWK_PATH_NDP_IP_OFF_LEN_PAIR_LEN +       \
         DS_FWK_PATH_NDP_HDR_LEN)

#endif

