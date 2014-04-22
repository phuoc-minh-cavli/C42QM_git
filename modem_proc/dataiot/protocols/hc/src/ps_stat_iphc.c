/*===========================================================================

                          P S _ S T A T _ I P H C . C

GENERAL DESCRIPTION
  This program specifically deals with statistics at the IPHC layer of 
  data services stack

EXTERNALIZED FUNCTIONS
  ps_stat_get_iphc
    This function supplies the required IPHC stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2006-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/protocols/hc/src/ps_stat_iphc.c#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/14   vl      Optimization of F3 messages
03/23/11   op      Data SU API cleanup
12/27/10   rp      RAM optimization changes.
12/23/10   ss      Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
03/25/09   sp      MOB - Fixed compilation warnings and High Lint Errors.
10/24/08   pp      Fixed compiler warnings.
04/11/08   scb     Klocwork fixes
02/27/07   scb     Added changes to fix RCVT compilation warning
08/18/06   scb     Added API to get device handle from PPP control block.
07/11/06   scb     Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "IxErrno.h"
#include "err.h"

#include "ps_stat_iphc.h"
#include "ps_stat_common.h"
#include "ps_stat_logging.h"

#include "ps_iphc_internal.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_defs.h"

#include <stddef.h>
#include <string.h>
#include "ds_Utils_DebugMsg.h"


/*===========================================================================

                             INTERNAL DECLARATIONS

===========================================================================*/

/*===========================================================================
MACRO IPHC_STAT_OFFSET_SIZE(INDEX,FIELD,SIZE)

DESCRIPTION
  This macro fills the offset and size fields of the ps_stat_iphc_i_table.
  INDEX is used only for readability, not for usage

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define IPHC_STAT_OFFSET_SIZE(INDEX,FIELD,SIZE)                             \
{                                                                           \
    FPOS_STAT(iphc_instance_s_type,FIELD),                                  \
    sizeof(SIZE)                                                            \
}

/*---------------------------------------------------------------------------
  IPHC stats access table
---------------------------------------------------------------------------*/
static const ps_stat_access_table_s_type ps_stat_iphc_i_table[PS_STAT_IPHC_ALL + 1] = 
{
  /*-------------------------------------------------------------------------
   Compression statistics
	-------------------------------------------------------------------------*/
  IPHC_STAT_OFFSET_SIZE(0,iphc_i_stats.comp_hdr_bytes,			   uint32),
  IPHC_STAT_OFFSET_SIZE(1,iphc_i_stats.comp_hdr_bytes_comp, 	   uint32),
  IPHC_STAT_OFFSET_SIZE(2,iphc_i_stats.comp_ipv4_frags, 		   uint32),
  IPHC_STAT_OFFSET_SIZE(3,iphc_i_stats.comp_ipv4_full_hdr_tos_67,  uint32),
  IPHC_STAT_OFFSET_SIZE(4,iphc_i_stats.comp_ipv6_full_hdr_tos_67,  uint32),
  IPHC_STAT_OFFSET_SIZE(5,iphc_i_stats.comp_tcp_full_hdr_rexmit,   uint32),

  /*-------------------------------------------------------------------------
	Decompression statistics
  -------------------------------------------------------------------------*/
  IPHC_STAT_OFFSET_SIZE(6,iphc_i_stats.decomp_hdr_bytes,		   uint32),
  IPHC_STAT_OFFSET_SIZE(7,iphc_i_stats.decomp_hdr_bytes_decomp,    uint32),
  IPHC_STAT_OFFSET_SIZE(8,iphc_i_stats.decomp_tcp_twice_alg,	   uint32),
  IPHC_STAT_OFFSET_SIZE(9,iphc_i_stats.decomp_ctx_state_pkt_tx,    uint32),
  IPHC_STAT_OFFSET_SIZE(10,iphc_i_stats.decomp_ctx_state_bytes_tx, uint32),
  IPHC_STAT_OFFSET_SIZE(11,iphc_i_stats.decomp_ctx_state_pkt_rx,   uint32),
  IPHC_STAT_OFFSET_SIZE(12,iphc_i_stats.decomp_ctx_state_bytes_rx, uint32),

  /*-------------------------------------------------------------------------
	Compression error statistics
  -------------------------------------------------------------------------*/
  IPHC_STAT_OFFSET_SIZE(13,iphc_i_stats.comp_gen_errors,		   uint32),
  IPHC_STAT_OFFSET_SIZE(14,iphc_i_stats.comp_dsm_errors,		   uint32),
  IPHC_STAT_OFFSET_SIZE(15,iphc_i_stats.comp_full_hdr_fail, 	   uint32),
  IPHC_STAT_OFFSET_SIZE(16,iphc_i_stats.comp_ipv4_fail, 		   uint32),
  IPHC_STAT_OFFSET_SIZE(17,iphc_i_stats.comp_ipv4_malformed_pkt,   uint32),
  IPHC_STAT_OFFSET_SIZE(18,iphc_i_stats.comp_ipv6_fail, 		   uint32),
  IPHC_STAT_OFFSET_SIZE(19,iphc_i_stats.comp_ipv6_malformed_pkt,   uint32),
  IPHC_STAT_OFFSET_SIZE(20,iphc_i_stats.comp_tcp_fail,			   uint32),
  IPHC_STAT_OFFSET_SIZE(21,iphc_i_stats.comp_non_tcp_fail,		   uint32),

  /*-------------------------------------------------------------------------
	Decompression error statistics
  -------------------------------------------------------------------------*/
  IPHC_STAT_OFFSET_SIZE(22,iphc_i_stats.decomp_invalid_ip,		   uint32),
  IPHC_STAT_OFFSET_SIZE(23,iphc_i_stats.decomp_gen_errors,		   uint32),
  IPHC_STAT_OFFSET_SIZE(24,iphc_i_stats.decomp_dsm_errors,		   uint32),
  IPHC_STAT_OFFSET_SIZE(25,iphc_i_stats.decomp_full_hdr_fail,	   uint32),
  IPHC_STAT_OFFSET_SIZE(26,iphc_i_stats.decomp_ipv4_fail,		   uint32),
  IPHC_STAT_OFFSET_SIZE(27,iphc_i_stats.decomp_ipv4_malformed_pkt, uint32),
  IPHC_STAT_OFFSET_SIZE(28,iphc_i_stats.decomp_ipv6_fail,		   uint32),
  IPHC_STAT_OFFSET_SIZE(29,iphc_i_stats.decomp_ipv6_malformed_pkt, uint32),
  IPHC_STAT_OFFSET_SIZE(30,iphc_i_stats.decomp_tcp_fail,		   uint32),
  IPHC_STAT_OFFSET_SIZE(31,iphc_i_stats.decomp_tcp_mis_cksum,	   uint32),
  IPHC_STAT_OFFSET_SIZE(32,iphc_i_stats.decomp_tcp_nodelta_fail,   uint32),
  IPHC_STAT_OFFSET_SIZE(33,iphc_i_stats.decomp_non_tcp_fail,	   uint32),

  /*-------------------------------------------------------------------------
  Indices 34(PS_STAT_IPHC_COMP_IPV4) to 45(PS_STAT_IPHC_DECOMP_NON_TCP) 
  are unused. Fill with zeros
  - Refer ps_stat_iphc_enum_type
  -------------------------------------------------------------------------*/
  STAT_FILL_NULL(34),
  STAT_FILL_NULL(35),
  STAT_FILL_NULL(36),
  STAT_FILL_NULL(37),
  STAT_FILL_NULL(38),
  STAT_FILL_NULL(39),
  STAT_FILL_NULL(40),
  STAT_FILL_NULL(41),
  STAT_FILL_NULL(42),
  STAT_FILL_NULL(43),
  STAT_FILL_NULL(44),
  STAT_FILL_NULL(45),
  /*---------------------------------------------------------------------------
  For IPHC_ALL will be handled separately
  ---------------------------------------------------------------------------*/
  IPHC_STAT_OFFSET_SIZE(46,iphc_i_stats.comp_hdr_bytes,ps_stat_iphc_i_s_type)
};


/*===========================================================================

                             INTERNAL FORWARD DECLARATIONS

===========================================================================*/

static errno_enum_type ps_stat_get_iphc_instance_all
(
  ps_stat_iphc_enum_type    stat,
  iphc_instance_s_type      *iphc,
  void                      *return_value,
  uint16                    ret_len
);

static errno_enum_type ps_stat_get_iphc_comp_ip
(
  ps_stat_iphc_enum_type   stat,
  iphc_packet_type_e_type  iphc_pkt_e_type, 
  iphc_instance_s_type     *iphc,
  void                     *return_value,
  uint16                   ret_len
);

static errno_enum_type ps_stat_get_iphc_comp_pkt
(
  ps_stat_iphc_enum_type   stat,
  iphc_packet_type_e_type  iphc_pkt_e_type,
  iphc_instance_s_type     *iphc,
  void                     *return_value,
  uint16                   ret_len
);

static errno_enum_type ps_stat_get_iphc_decomp_pkt
(
  ps_stat_iphc_enum_type   stat,
  iphc_packet_type_e_type  iphc_pkt_e_type,
  iphc_instance_s_type     *iphc,
  void                     *return_value,
  uint16                   ret_len
);

static void ps_stat_get_iphc_desc_log_pkt
(
  int32          handle
);

static void ps_stat_get_iphc_stat_log_pkt
(
  int32          handle
);

/*===========================================================================
FUNCTION PS_STAT_GET_IPHC_INSTANCE_ALL()

DESCRIPTION
  This function supplies all the IPHC statistics for the instance handle
  supplied. This is needed because of the need to extract and format 
  data available in the IPHC Instance block

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  iphc_ptr        - IN - pointer that points to the IPHC instance block
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
static errno_enum_type ps_stat_get_iphc_instance_all
(
  ps_stat_iphc_enum_type    stat,
  iphc_instance_s_type      *iphc,
  void                      *return_value,
  uint16                    ret_len
)
{
   ps_stat_iphc_i_s_type *iphc_i_stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    validate 'stat'; only PS_STAT_IPHC_ALL should be passed to this 
    function
  -------------------------------------------------------------------------*/
  if (stat != PS_STAT_IPHC_ALL)
  {
    LOG_MSG_INFO1_0("Invalid stat value passed to "
                    "ps_stat_get_iphc_instance_all");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    validate iphc : should not be NULL
  -------------------------------------------------------------------------*/
  if (iphc == NULL)
  {
    LOG_MSG_INFO1_0("ps_stat_get_iphc_instance_all:Null value of iphc passed");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    return_value validation : should not be null
  -------------------------------------------------------------------------*/

  if (return_value == NULL)
  {
    LOG_MSG_ERROR_0("return_value should not be NULL ");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    check the sizeof the stat requested. If the size is zero, the stat 
    is invalid in the current context. return appropriate error
  -------------------------------------------------------------------------*/

  if (ret_len < sizeof(ps_stat_iphc_i_s_type))
  {
    LOG_MSG_ERROR_0(" Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }

  iphc_i_stats = (ps_stat_iphc_i_s_type *) return_value ;

  /*-------------------------------------------------------------------------
    Compression statistics
  -------------------------------------------------------------------------*/

  iphc_i_stats->comp_hdr_bytes              = 
                                       iphc->iphc_i_stats.comp_hdr_bytes;
  iphc_i_stats->comp_hdr_bytes_comp         = 
                                  iphc->iphc_i_stats.comp_hdr_bytes_comp;
  iphc_i_stats->comp_ipv4_frags             = 
                                     iphc->iphc_i_stats.comp_ipv4_frags ;
  iphc_i_stats->comp_ipv4_full_hdr_tos_67   = 
                           iphc->iphc_i_stats.comp_ipv4_full_hdr_tos_67 ;
  iphc_i_stats->comp_ipv6_full_hdr_tos_67   = 
                           iphc->iphc_i_stats.comp_ipv6_full_hdr_tos_67 ;
  iphc_i_stats->comp_tcp_full_hdr_rexmit    = 
                            iphc->iphc_i_stats.comp_tcp_full_hdr_rexmit ;

  /*-------------------------------------------------------------------------
    Decompression statistics
  -------------------------------------------------------------------------*/

  iphc_i_stats->decomp_tcp_twice_alg        = 
                                iphc->iphc_i_stats.decomp_tcp_twice_alg ;
  iphc_i_stats->decomp_hdr_bytes            = 
                                     iphc->iphc_i_stats.decomp_hdr_bytes;
  iphc_i_stats->decomp_hdr_bytes_decomp     = 
                              iphc->iphc_i_stats.decomp_hdr_bytes_decomp;
  iphc_i_stats->decomp_ctx_state_pkt_tx     = 
                              iphc->iphc_i_stats.decomp_ctx_state_pkt_tx;
  iphc_i_stats->decomp_ctx_state_bytes_tx          = 
                            iphc->iphc_i_stats.decomp_ctx_state_bytes_tx;
  iphc_i_stats->decomp_ctx_state_pkt_rx     = 
                              iphc->iphc_i_stats.decomp_ctx_state_pkt_rx;
  iphc_i_stats->decomp_ctx_state_bytes_rx          = 
                            iphc->iphc_i_stats.decomp_ctx_state_bytes_rx;

  /*-------------------------------------------------------------------------
    Compression error statistics
  -------------------------------------------------------------------------*/

  iphc_i_stats->comp_gen_errors             = 
                                      iphc->iphc_i_stats.comp_gen_errors;
  iphc_i_stats->comp_dsm_errors             = 
                                     iphc->iphc_i_stats.comp_dsm_errors ; 
  iphc_i_stats->comp_full_hdr_fail          = 
                                iphc->iphc_i_stats.comp_full_hdr_fail   ;
  iphc_i_stats->comp_ipv4_fail              = 
                                      iphc->iphc_i_stats.comp_ipv4_fail ;
  iphc_i_stats->comp_ipv4_malformed_pkt     = 
                             iphc->iphc_i_stats.comp_ipv4_malformed_pkt ;
  iphc_i_stats->comp_ipv6_malformed_pkt     = 
                             iphc->iphc_i_stats.comp_ipv6_malformed_pkt ;
  iphc_i_stats->comp_ipv6_fail              = 
                                      iphc->iphc_i_stats.comp_ipv6_fail ;
  iphc_i_stats->comp_tcp_fail               = 
                                       iphc->iphc_i_stats.comp_tcp_fail ;
  iphc_i_stats->comp_non_tcp_fail           = 
                                   iphc->iphc_i_stats.comp_non_tcp_fail ;

  /*-------------------------------------------------------------------------
    Decompression error statistics
  -------------------------------------------------------------------------*/

  iphc_i_stats->decomp_invalid_ip           = 
                                    iphc->iphc_i_stats.decomp_invalid_ip; 
  iphc_i_stats->decomp_gen_errors           = 
                                    iphc->iphc_i_stats.decomp_gen_errors;
  iphc_i_stats->decomp_dsm_errors           = 
                                  iphc->iphc_i_stats.decomp_dsm_errors  ;
  iphc_i_stats->decomp_full_hdr_fail        = 
                                iphc->iphc_i_stats.decomp_full_hdr_fail ;
  iphc_i_stats->decomp_ipv4_fail            = 
                                    iphc->iphc_i_stats.decomp_ipv4_fail ;
  iphc_i_stats->decomp_ipv4_malformed_pkt   = 
                            iphc->iphc_i_stats.decomp_ipv4_malformed_pkt;
  iphc_i_stats->decomp_ipv6_fail            = 
                                    iphc->iphc_i_stats.decomp_ipv6_fail ;
  iphc_i_stats->decomp_ipv6_malformed_pkt   = 
                            iphc->iphc_i_stats.decomp_ipv6_malformed_pkt;
  iphc_i_stats->decomp_tcp_fail             = 
                                     iphc->iphc_i_stats.decomp_tcp_fail ;
  iphc_i_stats->decomp_tcp_mis_cksum        = 
                                iphc->iphc_i_stats.decomp_tcp_mis_cksum ;
  iphc_i_stats->decomp_tcp_nodelta_fail     = 
                             iphc->iphc_i_stats.decomp_tcp_nodelta_fail ;
  iphc_i_stats->decomp_non_tcp_fail         = 
                                 iphc->iphc_i_stats.decomp_non_tcp_fail ;
  
/*---------------------------------------------------------------------------
  Compression statistics
---------------------------------------------------------------------------*/
 
  iphc_i_stats->comp_pkt_ipv4                     =
               iphc->iphc_i_stats.comp_ip[IPHC_PACKET_TYPE_NORMAL_IPV4] ;
  iphc_i_stats->comp_pkt_ipv6                     = 
               iphc->iphc_i_stats.comp_ip[IPHC_PACKET_TYPE_NORMAL_IPV6] ;
  iphc_i_stats->comp_pkt_full_header              = 
             iphc->iphc_i_stats.comp_pkt[IPHC_PACKET_TYPE_FULL_HEADER]  ;
  iphc_i_stats->comp_pkt_compressed_tcp           = 
           iphc->iphc_i_stats.comp_pkt[IPHC_PACKET_TYPE_COMPRESSED_TCP] ;
  iphc_i_stats->comp_pkt_compressed_tcp_nodelta   = 
   iphc->iphc_i_stats.comp_pkt[IPHC_PACKET_TYPE_COMPRESSED_TCP_NODELTA] ;
  iphc_i_stats->comp_pkt_compressed_non_tcp       = 
   iphc->iphc_i_stats.comp_pkt[IPHC_PACKET_TYPE_COMPRESSED_NON_TCP]     ;

/*---------------------------------------------------------------------------
  Decompression statistics
---------------------------------------------------------------------------*/
  
  iphc_i_stats->decomp_pkt_ipv4                     =
            iphc->iphc_i_stats.decomp_pkt[IPHC_PACKET_TYPE_NORMAL_IPV4] ;
  iphc_i_stats->decomp_pkt_ipv6                     = 
            iphc->iphc_i_stats.decomp_pkt[IPHC_PACKET_TYPE_NORMAL_IPV6] ;
  iphc_i_stats->decomp_pkt_full_header              = 
           iphc->iphc_i_stats.decomp_pkt[IPHC_PACKET_TYPE_FULL_HEADER]  ;
  iphc_i_stats->decomp_pkt_compressed_tcp           = 
         iphc->iphc_i_stats.decomp_pkt[IPHC_PACKET_TYPE_COMPRESSED_TCP] ;
  iphc_i_stats->decomp_pkt_compressed_tcp_nodelta   = 
  iphc->iphc_i_stats.decomp_pkt[IPHC_PACKET_TYPE_COMPRESSED_TCP_NODELTA];
  iphc_i_stats->decomp_pkt_compressed_non_tcp       = 
   iphc->iphc_i_stats.decomp_pkt[IPHC_PACKET_TYPE_COMPRESSED_NON_TCP]   ;

  iphc->iphc_i_stats.queried = PS_STAT_QUERIED;

  return E_SUCCESS ;
}


/*===========================================================================
FUNCTION PS_STAT_GET_IPHC_COMP_IP()

DESCRIPTION
  This function supplies all the compressed IPHC statistics for the 
  instance handle supplied for IPv4 and IPv6. This is needed in order to 
  extract and format data available in the IPHC Instance block.

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat                  - IN - type of stat
  iphc_pkt_e_type       - IN - IP version type 
  iphc_instance_s_type  - IN - pointer that points to the IPHC instance block
  return_value          - IN - pointer for return value
                        - OUT- return value (statistics)
  ret_len               - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
static errno_enum_type ps_stat_get_iphc_comp_ip
(
  ps_stat_iphc_enum_type   stat,
  iphc_packet_type_e_type  iphc_pkt_e_type, 
  iphc_instance_s_type     *iphc,
  void                     *return_value,
  uint16                   ret_len
)
{
   uint32   *comp_ip_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    validate 'stat'; only PS_STAT_IPHC_COMP_IPV4 or PS_STAT_IPHC_COMP_IPV6
    should be passed to this function.
  -------------------------------------------------------------------------*/
  if (stat != PS_STAT_IPHC_COMP_IPV4 && stat != PS_STAT_IPHC_COMP_IPV6)
  {
    LOG_MSG_INFO1_0("Invalid stat value passed to ps_stat_get_iphc_comp_ip");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    validate iphc : should not be NULL
  -------------------------------------------------------------------------*/
  if (iphc == NULL)
  {
    LOG_MSG_INFO1_0("ps_stat_get_iphc_comp_ip:Null value of iphc passed");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    validate iphc_pkt_e_type : should be betwen 0 and 
    IPHC_PACKET_TYPE_NORMAL_IP_MAX
  -------------------------------------------------------------------------*/
  if ( iphc_pkt_e_type >= IPHC_PACKET_TYPE_NORMAL_IP_MAX  )
  {
    LOG_MSG_INFO1_0("ps_stat_get_iphc_comp_ip:Invalid iphc_pkt_e_type passed");
    return E_NOT_ALLOWED ;
  }
  /*-------------------------------------------------------------------------
    return_value validation : should not be null
  -------------------------------------------------------------------------*/

  if (return_value == NULL)
  {
    LOG_MSG_ERROR_0("return_value should not be NULL ");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    check the sizeof the stat requested. If the size is zero, the stat is
    invalid in the current context. return appropriate error
  -------------------------------------------------------------------------*/

  if (ret_len < sizeof(uint32))
  {
    LOG_MSG_ERROR_0(" Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }

  comp_ip_val = (uint32 *) return_value;

  *comp_ip_val = iphc->iphc_i_stats.comp_ip[iphc_pkt_e_type];

  return E_SUCCESS ;

}


/*===========================================================================
FUNCTION PS_STAT_GET_IPHC_COMP_PKT()

DESCRIPTION
  This function supplies all the compressed IPHC statistics for the 
  instance handle supplied for TCP/TCP_NODELTA and NON_TCP. This is 
  needed in order to extract and format data available in the IPHC 
  instance block.

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat                    - IN - type of stat
  iphc_packet_type_e_type - IN - IPHC packet type 
  iphc_instance_s_type    - IN - pointer to the IPHC instance block
  return_value            - IN - pointer for return value
                          - OUT- return value (statistics)
  ret_len                 - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
static errno_enum_type ps_stat_get_iphc_comp_pkt
(
  ps_stat_iphc_enum_type   stat,
  iphc_packet_type_e_type  iphc_pkt_e_type,
  iphc_instance_s_type     *iphc,
  void                     *return_value,
  uint16                   ret_len
)
{
   uint32   *comp_ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    validate 'stat'; only PS_STAT_IPHC_COMP_TCP or 
                          PS_STAT_IPHC_COMP_TCP_NODELTA or 
                          PS_STAT_IPHC_COMP_NON_TCP or
                          PS_STAT_IPHC_COMP_FULL_HDR
    should be passed to this function.
  -------------------------------------------------------------------------*/
  if (stat != PS_STAT_IPHC_COMP_TCP && 
      stat != PS_STAT_IPHC_COMP_TCP_NODELTA &&
      stat != PS_STAT_IPHC_COMP_NON_TCP &&
      stat != PS_STAT_IPHC_COMP_FULL_HDR)
  {
    LOG_MSG_INFO1_0("Invalid stat value passed to ps_stat_get_iphc_comp_pkt");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    validate iphc : should not be NULL
  -------------------------------------------------------------------------*/
  if (iphc == NULL)
  {
    LOG_MSG_INFO1_0("ps_stat_get_iphc_comp_pkt:Null value of iphc passed");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    return_value validation : should not be null
  -------------------------------------------------------------------------*/

  if (return_value == NULL)
  {
    LOG_MSG_ERROR_0("return_value should not be NULL ");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    check the sizeof the stat requested. If the size is zero, the stat is
    invalid in the current context. return appropriate error
  -------------------------------------------------------------------------*/

  if (ret_len < sizeof(uint32))
  {
    LOG_MSG_ERROR_0(" Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }
  
  comp_ret_val = (uint32 *) return_value;

  *comp_ret_val = iphc->iphc_i_stats.comp_pkt[iphc_pkt_e_type];

  return E_SUCCESS ;

}


/*===========================================================================
FUNCTION PS_STAT_GET_IPHC_DECOMP_PKT()

DESCRIPTION
  This function supplies the decompressed IPHC statistics for the instance
  handle supplied for TCP, TCP_NODELTA and NON_TCP. This is needed in 
  order to extract and format data available in the IPHC Instance block.

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat                    - IN - type of stat
  iphc_packet_type_e_type - IN - IPHC packet type 
  iphc_instance_s_type    - IN - pointer that points to the IPHC instance block
  return_value            - IN - pointer for return value
                          - OUT- return value (statistics)
  ret_len                 - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
static errno_enum_type ps_stat_get_iphc_decomp_pkt
(
  ps_stat_iphc_enum_type   stat,
  iphc_packet_type_e_type  iphc_pkt_e_type,
  iphc_instance_s_type     *iphc,
  void                     *return_value,
  uint16                   ret_len
)
{
   uint32   *decomp_ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
  validate 'stat'; only PS_STAT_IPHC_DECOMP_IPV4 or 
                        PS_STAT_IPHC_DECOMP_IPV6 or 
                        PS_STAT_IPHC_DECOMP_TCP or 
                        PS_STAT_IPHC_DECOMP_TCP_NODELTA or
                        PS_STAT_IPHC_DECOMP_NON_TCP or
                        PS_STAT_IPHC_DECOMP_FULL_HDR 
  should be passed to this function.
  -------------------------------------------------------------------------*/
  if (stat != PS_STAT_IPHC_DECOMP_IPV4 && 
      stat != PS_STAT_IPHC_DECOMP_IPV6 && 
      stat != PS_STAT_IPHC_DECOMP_TCP && 
      stat != PS_STAT_IPHC_DECOMP_TCP_NODELTA &&
      stat != PS_STAT_IPHC_DECOMP_NON_TCP &&
      stat != PS_STAT_IPHC_DECOMP_FULL_HDR)
  {
    LOG_MSG_INFO1_0("Invalid stat value passed to ps_stat_get_iphc_decomp_pkt");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    validate iphc : should not be NULL
  -------------------------------------------------------------------------*/
  if (iphc == NULL)
  {
    LOG_MSG_INFO1_0("ps_stat_get_iphc_decomp_pkt: Null value of iphc passed");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    return_value validation : should not be null
  -------------------------------------------------------------------------*/

  if (return_value == NULL)
  {
    LOG_MSG_ERROR_0("return_value should not be NULL ");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    check the sizeof the stat requested. If the size is zero, the stat is
    invalid in the current context. return appropriate error
  -------------------------------------------------------------------------*/

  if (ret_len < sizeof(uint32))
  {
    LOG_MSG_ERROR_0(" Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }
  
  decomp_ret_val = (uint32 *) return_value;

  *decomp_ret_val = iphc->iphc_i_stats.decomp_pkt[iphc_pkt_e_type];

  return E_SUCCESS ;

}


/*===========================================================================
FUNCTION PS_STAT_GET_IPHC_CONTROL_BLOCK_FOR_PPP_DEV()

DESCRIPTION
  This function supplies the address of the IPHC control block 
  corresponding to the PPP control block for the specified device.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  device         - IN -PPP device
  iphccb_address - OUT - address of the iphc control blaock from 
                         the ppp control block corresponding to the
                         device. This is a void ** so that a void* 
                         value can be returned back and can be used to
                         call ps_stat_get_ppp

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_iphc_control_block_for_ppp_dev
(
  ppp_dev_enum_type     device ,
  void                **iphccb_address
)
{
#ifdef FEATURE_DATA_PS_PPP_IPHC
  ppp_type *ppp_cb_array = NULL;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (device >= PPP_MAX_DEV)
  {
    LOG_MSG_ERROR_0(" Invalid device number passed");
    return E_INVALID_ARG ;
  }

  if (iphccb_address == NULL)
  {
    return E_INVALID_ARG;
  }

  /*-------------------------------------------------------------------------
    Copy the control block address
  -------------------------------------------------------------------------*/
  
#ifdef FEATURE_DATA_PS_PPP_IPHC
  ppp_cb_array = fsm_get_ppp_cb_array();
  *iphccb_address = (&ppp_cb_array[device])->iphcp ;
#endif

  return E_SUCCESS ;
}


/*===========================================================================
FUNCTION PS_STAT_GET_IPHC_DESC_LOG_PKT()

DESCRIPTION
  This function fills the description of the specified instance handle into
  a log packet.  If the handle matches the handle for all descriptions, it
  copies all of the active instance descriptions into the log packet.  If the
  handle does not specify all instances but specifies a valid active instance,
  the description associated with that instance is copied into the log packet.
  If the handle is invalid, nothing is done.

PARAMETERS
  handle          : Specifies which instance to retrieve the description from

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ps_stat_get_iphc_desc_log_pkt
(
  int32          handle
)
{
  int32          idx                = 0;
  int32          desc_size          = 0;
  uint8          inst_count         = 0;
  int32          log_pkt_avail_size = 0;
  uint8        * log_pkt_ptr        = NULL;
  uint8        * log_pkt_fill_ptr   = NULL;

  ps_stat_desc_all_inst_hdr_log_pkt_type     * desc_all_inst_pkt_ptr = NULL;
  ps_stat_desc_delta_inst_hdr_log_pkt_type   * delta_pkt_ptr         = NULL;
  ps_stat_inst_desc_iphc_type                * iphc_desc_ptr         = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_STAT_INST_DESC_ALL == handle)
  {
    /*-----------------------------------------------------------------------
      Iterate through each IPHC cb array entry.  Necessary since order is
      not maintained in the table (item 1 could be deallocated before item
      2, but we do not push item 2 into item 1's place)
    -----------------------------------------------------------------------*/
    for (idx = 0; idx < IPHC_MAX_INSTANCE_COUNT; ++idx)
    {
      /*---------------------------------------------------------------------
        Only show if the instance has been initialized
      ---------------------------------------------------------------------*/
      if (NULL != iphc_get_ctrl_blk(idx))
      {
        desc_size = sizeof(ps_stat_inst_desc_iphc_type);

        /*-------------------------------------------------------------------
          Insufficient amount of space to fit this description in the log
          packet, so commit the existing log
        -------------------------------------------------------------------*/
        if (log_pkt_avail_size < desc_size)
        {
          if (NULL != log_pkt_ptr)
          {
            desc_all_inst_pkt_ptr = 
              (ps_stat_desc_all_inst_hdr_log_pkt_type *) log_pkt_ptr;

            desc_all_inst_pkt_ptr->count = inst_count;
            memset(desc_all_inst_pkt_ptr->reserved, 0, 3);

            ps_stat_logging_commit_log_pkt
            (
              log_pkt_ptr,
              (PS_STAT_MAX_LOG_PKT_SIZE - log_pkt_avail_size)
            );
          }

          inst_count         = 0;
          log_pkt_avail_size = PS_STAT_MAX_LOG_PKT_SIZE;
          log_pkt_ptr        = NULL;
          log_pkt_fill_ptr   = NULL;

          /*------------------------------------------------------------------
            Get the pointer to the log packet
          ------------------------------------------------------------------*/
          ps_stat_logging_get_log_pkt(log_pkt_avail_size,
                                      LOG_PS_STAT_DESC_ALL_IPHC_INST_C,
                                      &log_pkt_ptr);

          /*-----------------------------------------------------------------
            If we fail to get a log packet, just return
          -----------------------------------------------------------------*/
          if (NULL == log_pkt_ptr)
          {
            LOG_MSG_ERROR_0("Unable to allocate log packet");
            return;
          }

          /*-----------------------------------------------------------------
            Leave space for the describe all instance header
          -----------------------------------------------------------------*/
          log_pkt_fill_ptr =
            (log_pkt_ptr + 
              (uint8) sizeof(ps_stat_desc_all_inst_hdr_log_pkt_type));

          log_pkt_avail_size -= (int32)sizeof(ps_stat_desc_all_inst_hdr_log_pkt_type);
        }

        /* By this time, log_pkt_fill_ptr should be valid! */
        if (NULL != log_pkt_fill_ptr)
        {
          iphc_desc_ptr = (ps_stat_inst_desc_iphc_type *)log_pkt_fill_ptr ;

          iphc_desc_ptr->handle       = (uint8)handle;
          memset(iphc_desc_ptr->reserved, 0, 3);

          /*-------------------------------------------------------------------
            Increment the fill pointer by the size, and decrement the count
            by the same value
          -------------------------------------------------------------------*/
          log_pkt_fill_ptr += (uint8) desc_size;
          log_pkt_avail_size -= desc_size;
          inst_count++;
        } /* (NULL != log_pkt_fill_ptr) */
      }
    } /* for all iphc instances*/

    /*-----------------------------------------------------------------------
      Dispatch the last packet
    -----------------------------------------------------------------------*/
    if (NULL != log_pkt_ptr)
    {
      desc_all_inst_pkt_ptr = 
        (ps_stat_desc_all_inst_hdr_log_pkt_type *) log_pkt_ptr;

      desc_all_inst_pkt_ptr->count = inst_count;
      memset(desc_all_inst_pkt_ptr->reserved, 0, 3);

      ps_stat_logging_commit_log_pkt
      (
        log_pkt_ptr,
        (PS_STAT_MAX_LOG_PKT_SIZE - log_pkt_avail_size)
      );
    }
  } /* if fill all iphc instances */
  else
  {
    desc_size = sizeof(ps_stat_inst_desc_iphc_type);

    /*-----------------------------------------------------------------------
      We need to allocate enough memory for the description and the delta
      header
    -----------------------------------------------------------------------*/
    log_pkt_avail_size = 
              (desc_size + (int32)sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type));

    ps_stat_logging_get_log_pkt(log_pkt_avail_size,
                                LOG_PS_STAT_DESC_DELTA_INST_C,
                                &log_pkt_ptr);

    /*-----------------------------------------------------------------------
      If we fail to get a log packet, just return
    -----------------------------------------------------------------------*/
    if (NULL == log_pkt_ptr)
    {
      LOG_MSG_ERROR_0("Unable to allocate log packet");
      return;
    }

    /*-----------------------------------------------------------------------
      Fill in the delta-log-packet specific fields
    -----------------------------------------------------------------------*/
    delta_pkt_ptr = (ps_stat_desc_delta_inst_hdr_log_pkt_type *) log_pkt_ptr;

    delta_pkt_ptr->type    = (uint8)PS_STAT_MODULE_INST_IPHC;
    delta_pkt_ptr->deleted = FALSE;
    memset(delta_pkt_ptr->reserved, 0, 2);

    log_pkt_fill_ptr =
      (log_pkt_ptr + (uint8) sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type));

    iphc_desc_ptr = (ps_stat_inst_desc_iphc_type *)log_pkt_fill_ptr ;

    iphc_desc_ptr->handle       = (uint8)handle;
    memset(iphc_desc_ptr->reserved, 0, 3);

    /*-----------------------------------------------------------------------
      Dispatch the last packet
    -----------------------------------------------------------------------*/
    ps_stat_logging_commit_log_pkt
    (
      log_pkt_ptr,
      (desc_size + (int32)sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type))
    );

  } /* else fill one iphc instance */
} /* ps_stat_get_iphc_desc_log_pkt */


/*===========================================================================
FUNCTION PS_STAT_GET_IPHC_STAT_LOG_PKT()

DESCRIPTION
  This function fills the description of the specified instance handle into
  a log packet.  If the handle matches the handle for all statistics, it
  copies all of the active instance statistics into the log packet.  If the
  handle does not specify all instances but specifies a valid active instance,
  the statistics associated with that instance is copied into the log packet.
  If the handle is invalid, nothing is done.

PARAMETERS
  handle          : Specifies which instance to retrieve the stats from

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ps_stat_get_iphc_stat_log_pkt
(
  int32          handle
)
{
  int32                               idx                   = 0;
  uint8                             * log_pkt_ptr           = NULL;
  void                              * iphc_ptr              = NULL;
  ps_stat_inst_iphc_log_pkt_type    * inst_iphc_log_pkt_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_STAT_INST_DESC_ALL == handle)
  {
    /*-----------------------------------------------------------------------
      Iterate through each IPHC cb array entry.  Necessary since order is
      not maintained in the table (item 1 could be deallocated before item
      2, but we do not push item 2 into item 1's place)
    -----------------------------------------------------------------------*/
    for (idx = 0; idx < IPHC_MAX_INSTANCE_COUNT; ++idx)
    {
      /*---------------------------------------------------------------------
        Only show if the instance has been initialized
      ---------------------------------------------------------------------*/
      iphc_ptr = iphc_get_ctrl_blk(idx);

      if (NULL == iphc_ptr)
      {
        continue;
      }

      ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_iphc_log_pkt_type),
                                  LOG_PS_STAT_IPHC_INST_C,
                                  &log_pkt_ptr);

      if (NULL == log_pkt_ptr)
      {
        LOG_MSG_ERROR_0("Log packet allocated for IPHC get stats failed");
        return;
      }

      inst_iphc_log_pkt_ptr = (ps_stat_inst_iphc_log_pkt_type *) ((void *)log_pkt_ptr);

      inst_iphc_log_pkt_ptr->handle = idx;

      (void) ps_stat_get_iphc
        (
          PS_STAT_IPHC_ALL,
          iphc_ptr,
          &(inst_iphc_log_pkt_ptr->inst_iphc_stats),
          sizeof(ps_stat_iphc_i_s_type)
        );

      ps_stat_logging_commit_log_pkt
      (
        log_pkt_ptr,
        sizeof(ps_stat_inst_iphc_log_pkt_type)
      );

    } /* for all iphc instances*/
  } /* if fill all iphc instances */
  else
  {
    /*---------------------------------------------------------------------
      Only show if the instance has been initialized
    ---------------------------------------------------------------------*/
    iphc_ptr = iphc_get_ctrl_blk(handle);

    if (NULL == iphc_ptr)
    {
      LOG_MSG_ERROR_1("Invalid handle %d", handle);
      return;
    }

    ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_iphc_log_pkt_type),
                                LOG_PS_STAT_IPHC_INST_C,
                                &log_pkt_ptr);

    if (NULL == log_pkt_ptr)
    {
      LOG_MSG_ERROR_0("Log packet allocated for IPHC get stats failed");
      return;
    }

    inst_iphc_log_pkt_ptr = (ps_stat_inst_iphc_log_pkt_type *) ((void *)log_pkt_ptr);

    inst_iphc_log_pkt_ptr->handle = handle;

    (void) ps_stat_get_iphc
      (
        PS_STAT_IPHC_ALL,
        iphc_ptr,
        &(inst_iphc_log_pkt_ptr->inst_iphc_stats),
        sizeof(ps_stat_iphc_i_s_type)
      );

    ps_stat_logging_commit_log_pkt
    (
      log_pkt_ptr,
      sizeof(ps_stat_inst_iphc_log_pkt_type)
    );

  } /* else fill one iphc instance */
} /* ps_stat_get_iphc_stat_log_pkt */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_STAT_RESET_IPHC()

DESCRIPTION
  This function resets the instance statistics associated with the specified
  instance.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle : Handle to the instance

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_iphc
(
  int32                    handle
)
{
  iphc_instance_s_type   * iphc_cb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  iphc_cb_ptr = iphc_get_ctrl_blk(handle);

  if (NULL == iphc_cb_ptr)
  {
    LOG_MSG_ERROR_1("Unable to get IPHC control block for reset.  Handle: %d",
                     handle);
    return;
  }

  if (PS_STAT_QUERIED == iphc_cb_ptr->iphc_i_stats.queried)
  {
    ps_stat_get_iphc_stat_log_pkt(handle);
  }

  memset(&(iphc_cb_ptr->iphc_i_stats), 0, sizeof(iphc_cb_ptr->iphc_i_stats));

  return;
}


/*===========================================================================
FUNCTION PS_STAT_INIT_IPHC()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_iphc
(
  void
)
{
  ps_stat_logging_inst_reg_fill_desc_cback(PS_STAT_MODULE_INST_IPHC,
                                           ps_stat_get_iphc_desc_log_pkt);

  ps_stat_logging_inst_reg_get_stat_cback(PS_STAT_MODULE_INST_IPHC,
                                          ps_stat_get_iphc_stat_log_pkt);

  ps_stat_logging_inst_reg_reset_stat_cback(PS_STAT_MODULE_INST_IPHC,
                                            ps_stat_reset_iphc);
} /* ps_stat_init_iphc */

/*===========================================================================
FUNCTION PS_STAT_GET_IPHC()

DESCRIPTION
  This function supplies the required iphc layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to the address of the
  iphc control block. The third argument points to the memory location
  where the results will be copied.The result (statistic) will be copied  
  to this memory address. The fourth argument passes the length of 
  memory allocated for the return value and should be greater than or 
  equal to the memory size of the statistic requested.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  instance_ptr    - IN - Handle to instance
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_iphc
(
  ps_stat_iphc_enum_type    stat,
  void                      *instance_ptr,
  void                      *return_value,
  uint16                    ret_len
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
      Check Instance pointer; Instance pointer should not be null
  -------------------------------------------------------------------------*/

  if (instance_ptr == NULL)
  {
    LOG_MSG_ERROR_0(" Instance pointer should NOT be NULL for IPHC stats");
    return E_INVALID_ARG ;
  }

  switch(stat)
  {

    case  PS_STAT_IPHC_COMP_IPV4:

      return ps_stat_get_iphc_comp_ip(stat,
                                      IPHC_PACKET_TYPE_NORMAL_IPV4,
                                      instance_ptr,
                                      return_value,
                                      ret_len
                                     );

    case  PS_STAT_IPHC_COMP_IPV6:

      return ps_stat_get_iphc_comp_ip(stat,
                                      IPHC_PACKET_TYPE_NORMAL_IPV6,
                                      instance_ptr,
                                      return_value,
                                      ret_len
                                     );

    case  PS_STAT_IPHC_COMP_FULL_HDR:

      return ps_stat_get_iphc_comp_pkt(
                                        stat,
                                        IPHC_PACKET_TYPE_FULL_HEADER,
                                        instance_ptr,
                                        return_value,
                                        ret_len
                                      );

    case  PS_STAT_IPHC_COMP_TCP:

      return ps_stat_get_iphc_comp_pkt(
                                        stat,
                                        IPHC_PACKET_TYPE_COMPRESSED_TCP,
                                        instance_ptr,
                                        return_value,
                                        ret_len
                                       );
      
    case  PS_STAT_IPHC_COMP_TCP_NODELTA:

      return ps_stat_get_iphc_comp_pkt(
                                      stat,
                                      IPHC_PACKET_TYPE_COMPRESSED_TCP_NODELTA,
                                      instance_ptr,
                                      return_value,
                                      ret_len
                                      );

     case  PS_STAT_IPHC_COMP_NON_TCP:

      return ps_stat_get_iphc_comp_pkt(
                                       stat,
                                       IPHC_PACKET_TYPE_COMPRESSED_NON_TCP,
                                       instance_ptr,
                                       return_value,
                                       ret_len
                                      );

    case  PS_STAT_IPHC_DECOMP_IPV4:

      return ps_stat_get_iphc_decomp_pkt(stat,
                                         IPHC_PACKET_TYPE_NORMAL_IPV4,
                                         instance_ptr,
                                         return_value,
                                         ret_len
                                        );

    case  PS_STAT_IPHC_DECOMP_IPV6:

      return ps_stat_get_iphc_decomp_pkt(stat,
                                         IPHC_PACKET_TYPE_NORMAL_IPV6,
                                         instance_ptr,
                                         return_value,
                                         ret_len
                                        );

    case  PS_STAT_IPHC_DECOMP_FULL_HDR:

    return ps_stat_get_iphc_decomp_pkt(
                                       stat,
                                       IPHC_PACKET_TYPE_FULL_HEADER,
                                       instance_ptr,
                                       return_value,
                                       ret_len
                                      );

    case  PS_STAT_IPHC_DECOMP_TCP:

      return ps_stat_get_iphc_decomp_pkt(
                                          stat,
                                          IPHC_PACKET_TYPE_COMPRESSED_TCP,
                                          instance_ptr,
                                          return_value,
                                          ret_len
                                        );
      
    case  PS_STAT_IPHC_DECOMP_TCP_NODELTA:

    return ps_stat_get_iphc_decomp_pkt(
                                      stat,
                                      IPHC_PACKET_TYPE_COMPRESSED_TCP_NODELTA,
                                      instance_ptr,
                                      return_value,
                                      ret_len
                                      );

    case  PS_STAT_IPHC_DECOMP_NON_TCP:

    return ps_stat_get_iphc_decomp_pkt(
                                       stat,
                                       IPHC_PACKET_TYPE_COMPRESSED_NON_TCP,
                                       instance_ptr,
                                       return_value,
                                       ret_len
                                      );
    case PS_STAT_IPHC_ALL:

    return ps_stat_get_iphc_instance_all(
                                       stat,
                                       instance_ptr,
                                       return_value,
                                       ret_len
                                      );

    default:
    LOG_MSG_INFO2_0("Retrieving information from the common block.");
    break;


  }

  return ps_stat_get_common(
                             (uint32)stat,
                             ps_stat_iphc_i_table,
                             (uint16)PS_STAT_IPHC_ALL,
                             instance_ptr,
                             return_value,
                             ret_len
                           );
} /* ps_stat_get_iphc */

