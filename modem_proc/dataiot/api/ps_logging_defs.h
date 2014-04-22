#ifndef PS_LOGGING_DEFS_H
#define PS_LOGGING_DEFS_H
/*===========================================================================

                        P S _ L O G G I N G _ D E F S . H

GENERAL DESCRIPTION
  This file defines the data sturctures, constants used by PS to
  frame diagnostic responses to the requests from the tool (DM).

Copyright (c) 2004-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

 $Header: //components/rel/dataiot.mpss/2.0/api/ps_logging_defs.h#1 $
 $Author: pwbldsvc $ $DateTime: 2020/03/17 08:03:16 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/08/18     mks     Created the file for the logging.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"


/*===========================================================================

                            CONSTANTS

===========================================================================*/
/*---------------------------------------------------------------------------
  Maximum length of desc description
---------------------------------------------------------------------------*/
#define DPL_IFNAME_DESC_S_LEN  32

/*---------------------------------------------------------------------------
  Snaplen value special case to indicate that whole packet needs to be
  logged
---------------------------------------------------------------------------*/
#ifndef UINT32_MAX
#define UINT32_MAX           4294967295u
#endif

#define SNAPLEN_ALL  UINT32_MAX

#define DPL_LINK_DESC_S_LEN    DPL_IFNAME_DESC_S_LEN
#define DPL_FLOW_DESC_S_LEN    DPL_IFNAME_DESC_S_LEN

#define DPL_PACKET_PARTIAL_LENGTH 100

/*---------------------------------------------------------------------------
  Snaplen value special case to indicate the maximum partial logging length,
  that is supporte
---------------------------------------------------------------------------*/
#define SNAPLEN_MAX  65535

/*---------------------------------------------------------------------------
  DPL flag masks and access macros.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------

Interface Identifier (IID) fields:

  + - - - - - - - - + - - - - - - - - + - - - - - - - - + - - - - - - - - +
  | Flags           | Ifname          | Protocol        | Link instance   |
  + - - - - - - - - + - - - - - - - - + - - - - - - - - + - - - - - - - - +
  |                  \
  |                   \
  + - - - - - - - - -- +
  |L | D| F|  Reserved |
  + - - - - - - - - -  +

---------------------------------------------------------------------------*/

#define DPL_IID_RESERVED_MASK (0x1F)

#define IS_DPL_IID_RESERVED_NONZERO(iid)         \
  ((iid).dpl_flags & (DPL_IID_RESERVED_MASK))

/*---------------------------------------------------------------------------
  Flow bit (Which of flow and network logging is enabled?)

typedef enum
{
  DPL_IID_FLOW_BIT_MIN      = 0,
  DPL_IID_FLOW_BIT_NONFLOW  = 0,
  DPL_IID_FLOW_BIT_FLOW     = 1,
  DPL_IID_FLOW_BIT_WIDTH    = 1        Ensures that enum occupies 1 bit
} dpl_iid_flow_bit_enum_type;
---------------------------------------------------------------------------*/
#define DPL_IID_FLOW_MASK (0x20)

#define SET_DPL_IID_F_BIT_FLOW(iid)              \
  ((iid).dpl_flags |= (DPL_IID_FLOW_MASK))

#define SET_DPL_IID_F_BIT_NONFLOW(iid)           \
  ((iid).dpl_flags &= ~(DPL_IID_FLOW_MASK))

#define IS_DPL_IID_F_BIT_FLOW(iid)               \
  ((iid).dpl_flags & (DPL_IID_FLOW_MASK))

#define IS_DPL_IID_F_BIT_NONFLOW(iid)            \
  (!IS_DPL_IID_F_BIT_FLOW(iid))

/*---------------------------------------------------------------------------
  D bit (Direction)

typedef enum
{
  DPL_IID_DIR_MIN   = 0,
  DPL_IID_DIR_RX    = 0,
  DPL_IID_DIR_TX    = 1,
  DPL_IID_DIR_WIDTH = 1                Ensures that enum occupies 1 bit
} dpl_iid_d_bit_enum_type;
---------------------------------------------------------------------------*/
#define DPL_IID_DIRECTION_MASK (0x40)

#define SET_DPL_IID_DIR_TX(iid)                  \
  ((iid).dpl_flags |= (DPL_IID_DIRECTION_MASK))

#define SET_DPL_IID_DIR_RX(iid)                  \
  ((iid).dpl_flags &= ~(DPL_IID_DIRECTION_MASK))

#define IS_DPL_IID_DIR_TX(iid)                   \
  ((iid).dpl_flags & (DPL_IID_DIRECTION_MASK))

#define IS_DPL_IID_DIR_RX(iid)                   \
  (!IS_DPL_IID_DIR_TX(iid))


/*---------------------------------------------------------------------------
  L bit (Is link level logging enabled?)

typedef enum
{
  DPL_IID_L_BIT_MIN      = 0,
  DPL_IID_L_BIT_NETWORK  = 0,
  DPL_IID_L_BIT_LINK     = 1,
  DPL_IID_L_BIT_WIDTH    = 1           Ensures that enum occupies 1 bit
} dpl_iid_l_bit_enum_type;
---------------------------------------------------------------------------*/
#define DPL_IID_LINK_MASK (0x80)

#define SET_DPL_IID_L_BIT_LINK(iid)              \
  ((iid).dpl_flags |= (DPL_IID_LINK_MASK))

#define SET_DPL_IID_L_BIT_NETWORK(iid)           \
  ((iid).dpl_flags &= ~(DPL_IID_LINK_MASK))

#define IS_DPL_IID_L_BIT_LINK(iid)               \
  ((iid).dpl_flags & (DPL_IID_LINK_MASK))

#define IS_DPL_IID_L_BIT_NETWORK(iid)            \
  (!IS_DPL_IID_L_BIT_LINK(iid))
  
/*===========================================================================

                                   VARIABLES

===========================================================================*/
/*---------------------------------------------------------------------------
  Enum used for IID Interface names
---------------------------------------------------------------------------*/
typedef enum
{
  /* Value 0 is reserved and must not be defined or used
  DPL_IID_IFNAME_RESERVED                   = 0                         */
  DPL_IID_IFNAME_MIN                        = 1,
  DPL_IID_IFNAME_SIO_IFACE_UMTS_1           = 20,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_1          = 28,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_2          = 29,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_3          = 30,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_4          = 31,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_5          = 32,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_6          = 33,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_7          = 34,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_8          = 35,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_9          = 36,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_10         = 37,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_11         = 38,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_12         = 39,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_13         = 40,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_14         = 41,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_15         = 42,
  DPL_IID_IFNAME_SIO_IFACE_RMNET_16         = 43,
  DPL_IID_IFNAME_3GPP_CONTEXT_0             = 56,
  DPL_IID_IFNAME_3GPP_CONTEXT_1             = 57,
  DPL_IID_IFNAME_3GPP_CONTEXT_2             = 58,
  DPL_IID_IFNAME_3GPP_CONTEXT_3             = 59,
  DPL_IID_IFNAME_3GPP_CONTEXT_4             = 60,
  DPL_IID_IFNAME_3GPP_CONTEXT_5             = 61,
  DPL_IID_IFNAME_3GPP_CONTEXT_6             = 62,
  DPL_IID_IFNAME_3GPP_CONTEXT_7             = 63,
  DPL_IID_IFNAME_3GPP_CONTEXT_8             = 64,
  DPL_IID_IFNAME_3GPP_CONTEXT_9             = 65,
  DPL_IID_IFNAME_3GPP_CONTEXT_10            = 66,
  DPL_IID_IFNAME_3GPP_CONTEXT_11            = 67,
  DPL_IID_IFNAME_3GPP_CONTEXT_12            = 68,
  DPL_IID_IFNAME_3GPP_CONTEXT_13            = 69,
  DPL_IID_IFNAME_3GPP_CONTEXT_14            = 70,
  DPL_IID_IFNAME_3GPP_CONTEXT_15            = 71,
  DPL_IID_IFNAME_SIO_IFACE_UMTS_2           = 99,
  DPL_IID_IFNAME_SIO_IFACE_UMTS_3           = 108,
  DPL_IID_IFNAME_SIO_IFACE_UMTS_4           = 109,

  DPL_IID_IFNAME_MAX,
  DPL_IID_IFNAME_WIDTH                  = 0xFF /* Ensures that enum
                                                  occupies 8 bits          */
} dpl_iid_ifname_enum_type;

/*---------------------------------------------------------------------------
  When link bit is DPL_IID_L_BIT_NETWORK, the protocol field uses this enum
---------------------------------------------------------------------------*/
typedef enum
{
  /* Value 0 is reserved and must not be used
  DPL_IID_NETPROT_RESERVED      = 0                                         */
  DPL_IID_NETPROT_MIN           = 1,
  DPL_IID_NETPROT_IP            = 1,
  DPL_IID_NETPROT_HDLC_UNFRAMED = 2,
  DPL_IID_NETPROT_MAX,
  DPL_IID_NETPROT_WIDTH         = 0xFF /* Ensures that enum occupies 8 bits */
} dpl_iid_netprot_enum_type;

/*---------------------------------------------------------------------------
  When link bit is DPL_IID_L_BIT_LINK, the protocol field uses this enum
---------------------------------------------------------------------------*/
typedef enum
{
  DPL_IID_LINKPROT_MIN          = 0,
  DPL_IID_LINKPROT_WILDCARD     = 0,
  DPL_IID_LINKPROT_ETHERNET     = 1,
  DPL_IID_LINKPROT_PPP_IN_HDLC  = 2,
  DPL_IID_LINKPROT_ROHC_COMP_IP = 3,
  DPL_IID_LINKPROT_IPHC_COMP_IP = 4,
  DPL_IID_LINKPROT_MAX,
  DPL_IID_LINKPROT_WIDTH        = 0xFF /* Ensures that enum occupies 8 bits */
} dpl_iid_linkprot_enum_type;

/*--------------------------------------------------------
  Data Protocol Logging Status
  ---------------------------------------------------------*/
  typedef enum
  {
    DPL_DISABLED         = 0,
    DPL_ENABLED_PARTIAL  = 1,
    DPL_ENABLED_FULL     = 2,
    DPL_ENABLED_IP_HEADER
  } dpl_state_enum_type;

/*-----------------------------------------------------------
  IID data structure.
-----------------------------------------------------------*/
typedef struct
{
  unsigned int                   dpl_flags:8;
  unsigned int                   ifname:8;
  unsigned int                   protocol:8;
  unsigned int                   link_instance:8;
} dpl_iid_type;

  /*---------------------------------------------------------

  ROHC and IPHC Headers for DPL logging
------------------------------------------------ ---------*/
typedef struct
{
  byte   ppp_flag;
  byte   ppp_prot[2];
} dpl_comp_header_type;

/*-----------------------------------------------------------
  Link logging control block for Tx and Rx directions

    - partial_bytes_logged : records the number of bytes in a packet logged
                             so far
    - snaplen              : partial logging length for link protocols
    - mask                 : A set bit implies that logging is enabled for
                             corresponding protocol
    - seq_num              : Sequence number sent in last log packet
    - dpl_id               : DPL IID of this Physlink. Protocol field must
                             always be 0
    - is_logged            : Indicates if logging is enables or not

  There are two iid's in the link control block so resets 
  can be done easily and are consistent over time.

  This is not featurized so ROHC and IPHC libraries will 
  build if logging is turned off

-----------------------------------------------------------*/


typedef struct
{
  uint32   partial_bytes_logged;
  uint32   snaplen;
  uint16   seq_num[DPL_IID_LINKPROT_MAX];
  uint16   seg_num[DPL_IID_LINKPROT_MAX];
  boolean  is_logged;
} dpl_link_dir_cb_type;

typedef struct
{
  dpl_link_dir_cb_type  tx_cb;
  dpl_link_dir_cb_type  recv_cb;
  dpl_iid_type          tx_dpl_id;
  dpl_iid_type          recv_dpl_id;
  char                  desc[DPL_LINK_DESC_S_LEN];
} dpl_link_cb_type;

typedef struct
{
  dpl_iid_ifname_enum_type           dpl_id;
  boolean                            in_use;
}dpl_cp_iid_info_type;

typedef struct
{
  dpl_iid_type                    dpl_id;             //Same for both the RX and TX side
  uint16                          tx_seq_num[DPL_IID_NETPROT_MAX]; 
  uint16                          rx_seq_num[DPL_IID_NETPROT_MAX]; 
}dpl_net_cb;

/*----------------------------------------------------------
  Network logging control block for Tx and Rx directions
    - snaplen                    : partial logging length for network
                                    protocols
    - mask                       : A set bit implies that logging is enabled
                                   for corresponding protocol (see netprot enum)
    - is_logging_enabled         : Indicates if logging is enabled or not
----------------------------------------------------------*/


typedef struct
{
  uint32        mask;
  uint32        snaplen[DPL_IID_NETPROT_MAX];
  boolean       is_logging_enabled;
} dpl_net_dir_cb_type;

#endif /* PS_LOGGING_DEFS_H */
