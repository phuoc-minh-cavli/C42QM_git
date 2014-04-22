/*===========================================================================

           S Y S _ M_SMP2P. C

DESCRIPTION

Copyright (c) 2015-16 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/debugtools/sys_m/src/lib/9x65/apps/sys_m_smp2p.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/24/15   rks     Created

===========================================================================*/

#include "sys_m_smp2p.h"
#include "sys_m.h"
#include "rcinit.h"
#include "err.h"
#include "smp2p.h"
#include "DALSys.h"
#include "npa_resource.h"
#include "task.h"

smp2p_context_type *sys_m_smp2p_tx_context[SMEM_NUM_HOSTS];
smp2p_context_type *sys_m_smp2p_rx_context[SMEM_NUM_HOSTS];

uint32 sys_m_modem_shutdown_ack;
static uint8 sys_m_modem_shutdown_ack_try_count =0;
volatile uint8 sys_m_modem_shutdown_ack_err_fatal =0;
uint32 sys_m_adsp_shutdown_ack;


uint32 sys_m_rx_state = 0;
uint32 sys_m_tx_state = 0;

/* Polls for modem shutdown ack */
uint32 sys_m_smp2p_get_modem_shutdwn_ack(void)
{
   uint32 rx_state_received;
   int32 return_val;

   if (sys_m_modem_shutdown_ack == SYS_M_SUBSYS2AP_SMP2P_SHUT_DWN_ACK)
   {  
     return sys_m_modem_shutdown_ack;
   }
   
   /* read the data here */
   return_val = smp2p_read(sys_m_smp2p_rx_context[SMEM_MODEM], &rx_state_received);
   if(SMP2P_STATUS_SUCCESS != return_val)
   {
      /* It's an error scenario and the function is only invoked from the err handler
      So avoiding a recursive ERR_FATAL call and updating a flag */
      sys_m_modem_shutdown_ack_err_fatal=1;
   }

   if (rx_state_received & SYS_M_SUBSYS2AP_SMP2P_SHUT_DWN_ACK)
   {  
     sys_m_modem_shutdown_ack = SYS_M_SUBSYS2AP_SMP2P_SHUT_DWN_ACK;
   }
   
   /* Signifies the number of iterations for shutdown ack */
   sys_m_modem_shutdown_ack_try_count++;

   return sys_m_modem_shutdown_ack;
}


/*returns adsp shutdown ack status bit*/
uint32 sys_m_smp2p_get_adsp_shutdwn_ack(void)
{
   return sys_m_adsp_shutdown_ack;
}


/**
  Sets bit in sys_m point to point array to apps

  @return
  Error codes

  @dependencies
  None.
*/
int32 sys_m_smp2p_set(uint32 bits, smem_host_type host)
{
   if ((host <SMEM_NUM_HOSTS) && (sys_m_smp2p_tx_context[host] == 0))
      return SMP2P_STATUS_NOT_INIT;

    return smp2p_write(sys_m_smp2p_tx_context[host], bits);

}


/**
 *
 * @brief sys_m_smp2p_init
 *
 * System monitor SMP2P initialization function
 *
*/
void sys_m_smp2p_init(void)
{
  
}

/**
 *
 * @brief sys_m_smp2p_task
 *
 * System Monitor SMP2P handling task
 *
 * @param[in] hEvent: Task init paramter
 * @param[in] tid: task id
 *
*/
DALResult sys_m_smp2p_task(DALSYSEventHandle hEvent, void* tid)

{
  uint32 rx_state_received;
  uint32 rx_state_change;
  int32 return_val;
  uint32 host_idx;
  smp2p_context_type * rx_ctx_arr[SMEM_NUM_HOSTS];
  uint32 rx_map[SMEM_NUM_HOSTS];
  uint32 rx_cnt;
  uint32 remote_id;

  rcinit_handshake_startup();
 

  for (host_idx=0, rx_cnt=0; host_idx<SMEM_NUM_HOSTS; host_idx++)
  {
    /* ignore unused endpoints */
    if (SMEM_MODEM == host_idx) /* there is no ADSP on 9x45 */
    {
       return_val = smp2p_create(&sys_m_smp2p_tx_context[host_idx], (smem_host_type)host_idx, SYS_M_SMP2P_PORT_OUT);
       if (return_val != SMP2P_STATUS_SUCCESS && return_val != SMP2P_STATUS_NOT_INIT)
           ERR_FATAL("Sys_m cannot create outbound SMP2P", return_val, 0, 0);
        
       //wait for outbound readiness 
       if (return_val == SMP2P_STATUS_NOT_INIT)
       {
           return_val = smp2p_wait(sys_m_smp2p_tx_context[host_idx]);
           if (return_val == SMP2P_STATUS_FAILURE)
               ERR_FATAL("Sys_m cannot wait outbound SMP2P", return_val, 0, 0);
       }
       //register to listen on inbound 
       return_val = smp2p_register(&sys_m_smp2p_rx_context[host_idx], (smem_host_type)host_idx, SYS_M_SMP2P_PORT_IN);
       if (return_val != SMP2P_STATUS_SUCCESS && return_val != SMP2P_STATUS_NOT_FOUND)
           ERR_FATAL("Sys_m cannot listen on SMP2P", return_val, 0, 0);

      /* save the incoming event handle */
      rx_ctx_arr[rx_cnt] = sys_m_smp2p_rx_context[host_idx];

      /* save the destination id, so that when rx_ctx_arr[x] has an event,
       * we can map that back to an destination */
      rx_map[rx_cnt] = host_idx;

      /* increment count */
      rx_cnt++;
    }
  }
  npa_define_marker("/ssr/smp2p/inited");

  /* main receiving loop */
  while (1)
  {

    /* wait for incoming events
     * this is a blocking call until we receive data on an incoming entry.
     * when this returns, the context at rx_ctx_arr[i] will have data
     * available to read */
    return_val = smp2p_wait_multiple(rx_ctx_arr, rx_cnt, &host_idx);
    if((SMP2P_STATUS_SUCCESS != return_val)||(host_idx > rx_cnt))
    {
       ERR_FATAL("smp2p_wait_multiple return_val =%d ,host_idx = %d", return_val, host_idx, 0);
    }

    /* map the remote id */
    remote_id = rx_map[host_idx];
    if (remote_id > SMEM_NUM_HOSTS){
       ERR_FATAL("remote_id is invalid  = %d",remote_id,0,0);
    }

    /* read the data there */
    return_val = smp2p_read(sys_m_smp2p_rx_context[remote_id], &rx_state_received);
    if(SMP2P_STATUS_SUCCESS != return_val)
    {
       ERR_FATAL("smp2p_read() return_val  = %d ",return_val,0,0);
    }

    //if state has changed, act appropriately
    rx_state_change = rx_state_received ^ sys_m_rx_state;

    //update global state if it has changed
    if (rx_state_change != 0){
        sys_m_rx_state = rx_state_received;
    }
     MSG_3(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"smp2p received remote_id =%d , rx_state_received =%d rx_state_change =%d", remote_id, rx_state_received ,rx_state_change);

    //error fatal
    if (rx_state_change & SYS_M_SUBSYS2AP_SMP2P_ERRFATAL)
    {
        if(remote_id == SMEM_MODEM){
          sys_m_modem_shutdown_ack = SYS_M_SUBSYS2AP_SMP2P_SHUT_DWN_ACK; /*set the shutdown_ack once error fatal notification received
                                                                            from the proc to avoid unwanted delay for waiting shutdonw ack in error handling path
                                                                            since the peripheral only initiated fatal error notification */
          sys_m_mpss_errfatal_cb();
        }
        else if(remote_id== SMEM_ADSP){
          sys_m_adsp_shutdown_ack = SYS_M_SUBSYS2AP_SMP2P_SHUT_DWN_ACK; /*set the shutdown_ack once error fatal notification received
                                                                            from the proc to avoid unwanted delay for waiting shutdonw ack in error handling path
                                                                            since the peripheral only initiated fatal error notification */
          sys_m_adsp_errfatal_cb();
        }
    }
    if (rx_state_change & SYS_M_SUBSYS2AP_SMP2P_SHUT_DWN_ACK)
    {
       if(remote_id == SMEM_MODEM){
          sys_m_modem_shutdown_ack = SYS_M_SUBSYS2AP_SMP2P_SHUT_DWN_ACK;
       }
       else if(remote_id== SMEM_ADSP){
          sys_m_adsp_shutdown_ack = SYS_M_SUBSYS2AP_SMP2P_SHUT_DWN_ACK;
      }
    }
    if ((rx_state_change & SYS_M_SUBSYS2AP_SMP2P_SHUT_DWN)
       &&(remote_id ==  SMEM_MODEM))
    {
      /*modem has requested a shutdown, enter graceful shutdown sequence*/
      MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"SYS_M_SUBSYS2AP_SMP2P_SHUT_DWN received");
      sys_m_reset();
    }
    if ((rx_state_change & SYS_M_SUBSYS2AP_SMP2P_PWR_OFF)
       &&(remote_id ==  SMEM_MODEM))
    {
      /*modem has requested a poweroff, enter graceful poweroff sequence*/
      MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"SYS_M_SUBSYS2AP_SMP2P_PWR_OFF received");
      sys_m_shutdown();
    }
    if ((rx_state_change & SYS_M_SUBSYS2AP_QMI_REQ_ACK)
       &&(remote_id ==  SMEM_MODEM))
    {
      /*modem ACKed for QMI SSCTL Shutdown/Restart request complete*/
      MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,PROC_ID"SYS_M_SUBSYS2AP_QMI_REQ_ACK received");
      sys_m_complete_reset();
    }
  }
}

