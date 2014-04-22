#ifndef SYS_M_SMP2P_H
#define SYS_M_SMP2P_H
/*===========================================================================

           S Y S _ M _ S M P 2 P M. H

DESCRIPTION

Copyright (c) 2015,2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/debugtools/api/sys_m_smp2p.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/24/15   rks     Created
09/19/16   nk      Sym_changes to support PIL on ThreadX

===========================================================================*/
#include "comdef.h"
#include "smem_type.h"


#define SYS_M_AP2SUBSYS_SMP2P_ERRFATAL       0x1

#define SYS_M_SUBSYS2AP_SMP2P_ERRFATAL      0x00000001
#define SYS_M_SUBSYS2AP_SMP2P_ERR_HDL_RDY   0x00000002
#define SYS_M_SUBSYS2AP_SMP2P_PWR_CLK_RDY   0x00000004
#define SYS_M_SUBSYS2AP_SMP2P_SHUT_DWN_ACK  0x00000008
#define SYS_M_SUBSYS2AP_SMP2P_SHUT_DWN      0x00000010
#define SYS_M_SUBSYS2AP_SMP2P_PWR_OFF       0x00000020
#define SYS_M_SUBSYS2AP_QMI_REQ_ACK         0x00000080

#define SYS_M_SMP2P_PORT_OUT        "master-kernel"
#define SYS_M_SMP2P_PORT_IN         "slave-kernel"


/* Callback type to be called when any smp2p bits set 
    that were passed as arguments */
typedef void (*sys_m_modem_smp2p_bit_notify_cb_type)(uint32 bits_set);

/**
  Sets bit in sys_m point to point array to apps
  If sys_m_smp2p_tx_context is not ready, then set sys_m_smp2p_cached_init_state flag

  @param[in] bit       Bit to set
  
  @return
   SYS_M_SMP2P_SUCCESS : successfully set or cached the bit

  @dependencies
  None.
*/

// 
int32 sys_m_smp2p_set(uint32 bits, smem_host_type host);

/*returns modem shutdown ack status bit*/
uint32 sys_m_smp2p_get_modem_shutdwn_ack(void);

/*returns adsp shutdown ack status bit*/
uint32 sys_m_smp2p_get_adsp_shutdwn_ack(void);


/**
 *
 * @brief sys_m_modem_smp2p_notify_register
 *
 * @param[in] bits: smp2p bits for which fp to be called
 * @param[in] fp  : function to be called when smp2p bits passed are set
 *
 * @return
 *  TRUE  : Registration successfull
 *  FALSE : Registration unsuccessfull
*/

boolean sys_m_modem_smp2p_notify_register(uint32 bits, sys_m_modem_smp2p_bit_notify_cb_type fp);

/**
 *
 * @brief sys_m_modem_smp2p_notify_deregister
 *
 * @param[in] fp : Function pointer passed during registration
 *
 * @return
 *  TRUE  : Deregistration successfull
 *  FALSE : Deregistration unsuccessfull
*/

boolean sys_m_modem_smp2p_notify_deregister(sys_m_modem_smp2p_bit_notify_cb_type fp);



#endif  /* SYS_M_SMP2P_H */
