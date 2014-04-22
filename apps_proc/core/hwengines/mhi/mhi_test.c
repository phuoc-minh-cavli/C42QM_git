/**
  @file mhi_test.c
  @brief
  This file contains the implementation of the MHI loopback test

  For ICMP echo response see:
  //components/dev/data.mpss/2.0/rpius.IPA_1_2/driver/ipa/src/ipa_test_loopback.c#4


*/
/*
===============================================================================

                             Edit History

 $Header:

Change revision history maintained in version system
===============================================================================
                   Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include <comdef.h>
#include <stdlib.h>

#include "rex.h"

#include "mhi_i.h"
#include "mhi_osal.h" 
#include "mhi_core.h"
#include "sio.h" 
#include "mhi_mmio.h"
#include "mhi_pm.h"
#include "mhi_link.h"
#include "mhi_sm.h"

#include "pcie.h"


#define IPA_LOOPBACK_IP_HDR_LENGTH 20
#define IPA_LOOPBACK_IP_VER_FIELD_V4 4
#define IPA_LOOPBACK_IP_VER_FIELD_V6 6
#define IPA_LOOPBACK_IP_PROT_FIELD_ICMP 1
#define IPA_LOOPBACK_IP_PROT_FIELD_DHCP 6
#define IPA_LOOPBACK_IP_PROT_FIELD_UDP 17
#define IPA_LOOPBACK_ICMP_TYPE_FIELD_ECHO_REQ 8
#define IPA_LOOPBACK_ICMP_TYPE_FIELD_ECHO_RES 0
#define IPA_LOOPBACK_ICMP_CODE_FIELD_ECHO_RES 0
#define IPA_LOOPBACK_ICMP_CHECKSUM_OFFSET 2 

#define MHI_TEST_DB_OFFSET 0x7F8
#define MHI_TEST_DB_CLEAR_REG_OFFSET 0x07C
#define MHI_TEST_DB_STATUS_REG_OFFSET 0x034

/*! IPv4 header format */
typedef struct 
{
  uint8   bVer;
  uint8   bServField;
  uint16  hwTotalLen;
  uint16  hwID;
  uint16  hwFragOff;
  uint8   bTTL;
  uint8   bProt;
  uint16  hwHeaderChkSum;
  uint32  wSource;
  uint32  wDest;
} ipa_ip_format;

/*! ICMP header format */
typedef struct  
{
  uint8   bType;
  uint8   bCode;
  uint16  hwCheckSum;
  uint16  wIdent;
  uint16  wSeqNum;
} ipa_icmp_format;

typedef struct
{
  uint64 num_ipv6_pkts;
  uint64 num_ipv4_pkts;
  uint64 num_icmpv4_ping_pkts;
  uint64 num_icmpv4_non_ping_pkts;
  uint64 num_udp_pkts;
  uint64 num_dhcp_pkts;
  uint64 num_dropped_pkts;
} ipa_test_loopback_s;

#ifdef MHI_PM_STATE_TEST
typedef enum _mhi_pm_test_ctrl_t
{
  PM_TEST_LINK_INACTIVITY    = 0,
  PM_TEST_ENTER_M0           = 1,  
  PM_TEST_ENTER_M2           = 2,
  PM_TEST_ENTER_M3           = 3,
  PM_TEST_DEVICE_WAKE_HIGH   = 4,
  PM_TEST_DEVICE_WAKE_LOW    = 5,
  PM_TEST_CORE_WAKEUP        = 6,
  PM_TEST_HW_ACC_WAKEUP      = 7,
  PM_TEST_ENTER_D0           = 8,
  PM_TEST_ENTER_D3hot        = 9,
  PM_TEST_ENTER_D3cold       = 10,
  PM_TEST_HOST_WAKEUP_PERST  = 11,
  PM_TEST_PCIE_LINKUP        = 12,
  PM_TEST_MAX
} mhi_pm_test_ctrl_t;

/* Notification for pm testing */
mhi_osal_event_handle hNotify_pmt;  
#endif

ipa_test_loopback_s mhi_loopback = {0,0,0,0,0,0};

mhi_timer_t soft_link_inactivity_timer;

/* The following functions have been exposed for test purposes only */
extern void mhi_link_notifications(pcie_event_notification_type link_notif, void *data);
extern void mhi_trigger_device_wake_intr(void);
extern void mhi_mmio_set_mhictrl_mhistate(mhi_state_t state);
extern uint32 mhi_mmio_read_reg(uint32 base, uint32 offset);
extern void mhi_mmio_write_reg(uint32 base, uint32 offset, uint32 val);
#ifdef MHI_PM_STATE_TEST
#ifdef MHIIPA
extern void ipa_test_mhi_load_uc_and_enable(void);
extern void ipa_mhi_mgr_sched_event (uint32 mhi_event, uint32 event_data);
typedef enum {
   MHI_MGR_PENDING_EVENT_WAKEUP = 0x0002
}mhi_mgr_pending_event_types;
#endif
#endif


/*===========================================================================

FUNCTION ipa_test_loopback_checksumadjust()

DESCRIPTION
  This function perform checksum adjustment.

DEPENDENCIES
  - chksum points to the chksum in the packet
  - optr points to the old data in the packet
  - nptr points to the new data in the packet

RETURN VALUE
  TRUE - Success
  FALSE - Fail

SIDE EFFECTS
  None
===========================================================================*/
boolean mhi_test_loopback_checksumadjust
(
  /* chksum points to the chksum in the packet */
  uint8 *chksum,
  /* pointer to old data in the packet */
  const uint8 *optr,
  /* length of old data in the packet*/
  int olen, 
  /* pointer to new data in the packet */
  const uint8 *nptr,
  /* length of new data in the packet which is replacing old data */
  int nlen
)
{
  uint32 temp;
  int32 x, old, new;

  if( (chksum == NULL) || (optr == NULL) || (nptr == NULL) )
  {
    return FALSE;
  }

  temp =(chksum[0] * 256) +chksum[1];
  x=(int32) (~temp);
  x &= 0xffff;
  while (olen) 
  {
    if (olen==1) 
    {
      old=optr[0]*256+optr[1];
      x-=old & 0xff00;
      if (x<=0) 
      { 
        x--;
        x&=0xffff;
      }
      break;
    }
    else 
    {
      old=(optr[0] << 8) +optr[1]; optr+=2;
      x-=old & 0xffff;
      if (x<=0) 
      { 
        x--;
        x&=0xffff; 
      }
      olen-=2;
    }
  }
  while (nlen) 
  {
    if (nlen==1) 
    {
      new=nptr[0]*256+nptr[1];
      x+=new & 0xff00;
      if (x & 0x10000) 
      {
        x++; 
        x&=0xffff; 
      }
      break;
    }
    else 
    {
      new=(nptr[0]  << 8) +nptr[1]; nptr+=2;
      x+=(new & 0xffff);
      if (x & 0x10000) 
      {
        x++;
        x&=0xffff;
      }
      nlen-=2;
    }
  }
  temp = (int32) x;
  temp=~temp;
  chksum[0]=(uint8) (temp >> 8); chksum[1]=(uint8) (temp & 0xff);
  return TRUE;
} /* ipa_test_loopback_checksumadjust */

/*============================================================================= 
 
           FUNCTION IPA_LOOPBACK_CONVERT_PACKET
 
=============================================================================*/
/*!
  @brief
  This function converts ICMP ping request packets to ping responces, and
  swaps addresses in UDP iperf traffic
  
  @param
   data_ptr - pointer to the packet
 
  @return
  @retval TRUE, if it was either an IPv4 ICMP ping request or an IPv4 UDP
          packet and conversion was successful.
  @retval FALSE, if packet was not IPv4 ICMP ping request or an IPv4 UDP
          packet.
*/ 
/*===========================================================================*/
boolean mhi_test_loopback_convert_packet
(
  uint8 *data_ptr
)
{
  uint32  wSource;
  uint32  wDest;
  uint8   protocol;
  uint8   icmp_type; 
  uint8  retval = 0;
  uint8   *checksum = NULL;
  uint8   *optr = NULL;
  uint8   new_header[2];
  /*-------------------------------------------------------------------------
    Check if packet is IPv4
  -------------------------------------------------------------------------*/
  if (((*data_ptr) >> 4) == IPA_LOOPBACK_IP_VER_FIELD_V4)
  {
    mhi_loopback.num_ipv4_pkts++;
    protocol = ((ipa_ip_format*)data_ptr)->bProt;

    switch (protocol) 
    {
      case IPA_LOOPBACK_IP_PROT_FIELD_ICMP:
        icmp_type = ((ipa_icmp_format*)
                     (data_ptr + sizeof(ipa_ip_format)))->bType;
        if (icmp_type == IPA_LOOPBACK_ICMP_TYPE_FIELD_ECHO_REQ)
        {
          /*-----------------------------------------------------------------
            This is a Ping Request pkt, change it to Ping Response.
            Also, replace the checksum field
          -----------------------------------------------------------------*/
          checksum = data_ptr + 
                     IPA_LOOPBACK_IP_HDR_LENGTH + 
                     IPA_LOOPBACK_ICMP_CHECKSUM_OFFSET;
          optr = data_ptr + IPA_LOOPBACK_IP_HDR_LENGTH;
          new_header[0] = IPA_LOOPBACK_ICMP_TYPE_FIELD_ECHO_RES;
          new_header[1] = IPA_LOOPBACK_ICMP_CODE_FIELD_ECHO_RES;
          (void)mhi_test_loopback_checksumadjust (
                                   checksum, 
                                   optr, 
                                   2, 
                                   new_header, 
                                   2 );

          ((ipa_icmp_format*)(data_ptr + sizeof(ipa_ip_format)))->bType =
                                  new_header[0];
          ((ipa_icmp_format*)(data_ptr + sizeof(ipa_ip_format)))->bCode = 
                                  new_header[1];
          /*-----------------------------------------------------------------
            Swap source and Dest addresses
          -----------------------------------------------------------------*/
          wSource = ((ipa_ip_format*)data_ptr)->wSource;
          wDest =   ((ipa_ip_format*)data_ptr)->wDest;
          ((ipa_ip_format*)data_ptr)->wSource = wDest;
          ((ipa_ip_format*)data_ptr)->wDest = wSource;
          mhi_loopback.num_icmpv4_ping_pkts++;
          retval = 1;
        }
        else
        {
          /*-----------------------------------------------------------------
            This is NOT a Ping Request pkt, Do Nothing
          -----------------------------------------------------------------*/
          mhi_loopback.num_icmpv4_non_ping_pkts++;
          retval = 0;
        }
        break;

      case IPA_LOOPBACK_IP_PROT_FIELD_UDP:
         mhi_loopback.num_udp_pkts++;
        /*-------------------------------------------------------------------
          Swap source and Dest addresses
        -------------------------------------------------------------------*/
        wSource = ((ipa_ip_format*)data_ptr)->wSource;
        wDest = ((ipa_ip_format*)data_ptr)->wDest;
        ((ipa_ip_format*)data_ptr)->wSource = wDest;
        ((ipa_ip_format*)data_ptr)->wDest = wSource;
        mhi_loopback.num_udp_pkts++;
        retval = 1;
        break;

       case IPA_LOOPBACK_IP_PROT_FIELD_DHCP:
         mhi_loopback.num_dhcp_pkts++;
         mhi_loopback.num_dropped_pkts++;
         retval = 2;
        break;

      default:
         mhi_loopback.num_dropped_pkts++;
         retval = 2;
        break;
    }
  }
  /* Do nothing for non IPv4 packets */
  else if (((*data_ptr) >> 4) == IPA_LOOPBACK_IP_VER_FIELD_V6)
  {
    mhi_loopback.num_ipv6_pkts++;
    mhi_loopback.num_dropped_pkts++;
    retval = 2;
  }
  return retval;
} /* ipa_test_loopback_convert_packet */

uint64 mhi_loopCount; 
uint64 mhi_loop_dropped_bytes; 
uint64 mhi_loop_drop_count; 

/* Local constants for mhi_sio_test */
typedef struct _mhi_sio_test_ctx
{
sio_open_type      open_ptr; 
dsm_watermark_type tx_wm_q; 
q_type             tx_q; 
sio_stream_id_type stream; 
mhi_osal_sync_type sync;
boolean            open; 
}mhi_sio_test_ctx_t; 

mhi_osal_thread_t threads[5]; 
mhi_sio_test_ctx_t mhi_sio_test_ctx[5]; 

/******************** * Functions *****************************/

/* Dump packet head */
void mhi_sio_test_dump_packet_head(dsm_item_type* dsm_item)
{
   mhi_osal_debug(MSG_LOW, "====Dumping packet====: %x%x%x%x%x%x%x%x, size: %d", dsm_item->data_ptr[0], dsm_item->data_ptr[1], dsm_item->data_ptr[2], dsm_item->data_ptr[3], 
                                                                                  dsm_item->data_ptr[4], dsm_item->data_ptr[5], dsm_item->data_ptr[6], dsm_item->data_ptr[7], dsm_item->size);
}

/* RX function which will receive data form the port */
void mhi_sio_test_rx_cb( mhi_sio_test_ctx_t *ctx,  dsm_item_type **dsm_item)
{

   /* This mutex grab is done only once to solve race condition where we get data before we are able to succefully return 
    * from sio_open and get the streamid. Most clients just queue the data to a queue and signal a seperate thread so it should not be a problem
    * for existing clients */ 
   if(ctx->open == 0) 
   {
      mhi_osal_syncenter(&ctx->sync);
      while(ctx->open !=1);
      mhi_osal_syncleave(&ctx->sync);
   }

   mhi_osal_debug(MSG_HIGH, "Received data:%x",  dsm_length_packet(*dsm_item)); 
   mhi_loopCount++; 
#ifdef MHI_CONVERT_PACKET
/* Send back only if packet is good  */ 
    if( 1 == mhi_test_loopback_convert_packet((uint8*)(*dsm_item)->data_ptr) )
#endif
    { 
       /* Loop back the data */
       mhi_sio_test_dump_packet_head(*dsm_item);
       /* This loop back test does not use app_ptr and app_field */
       (*dsm_item)->app_ptr=NULL;
       (*dsm_item)->app_field=NULL;
       sio_transmit(ctx->stream , *dsm_item);
    }
#ifdef MHI_CONVERT_PACKET
    else
    {
       dsm_free_buffer(*dsm_item); 
       mhi_loop_dropped_bytes+=(*dsm_item)->used; 
       mhi_loop_drop_count++;
    }
#endif
}




void mhi_sio_test_rx_cb1(dsm_item_type **dsm_item)
{
   mhi_sio_test_rx_cb(&mhi_sio_test_ctx[0], dsm_item); 
}
void mhi_sio_test_rx_cb2(dsm_item_type **dsm_item)
{
   mhi_sio_test_rx_cb(&mhi_sio_test_ctx[1], dsm_item); 
}
void mhi_sio_test_rx_cb3(dsm_item_type **dsm_item)
{
   mhi_sio_test_rx_cb(&mhi_sio_test_ctx[2], dsm_item); 
}
void mhi_sio_test_rx_cb4(dsm_item_type **dsm_item)
{
   mhi_sio_test_rx_cb(&mhi_sio_test_ctx[3], dsm_item); 
}
void mhi_sio_test_rx_cb5(dsm_item_type **dsm_item)
{
   mhi_sio_test_rx_cb(&mhi_sio_test_ctx[4], dsm_item); 
}
    
void mhi_sio_loopback_thread(void *ctx)
{
   mhi_sio_test_ctx_t *test_ctx = (mhi_sio_test_ctx_t*)ctx; 
   
   /* Open the port */
   /* Grav a mutex so we can finish opening port before rx_Cb tries to access the open ptr  */
   
   mhi_osal_syncenter(&test_ctx->sync);
   test_ctx->stream=sio_open(&test_ctx->open_ptr); 
   if (test_ctx->stream == SIO_NO_STREAM_ID)
   {
      mhi_osal_debug(MSG_ERR, "Could not open sio port"); 
      return; 
   }
   test_ctx->open=1; 
   mhi_osal_syncleave(&test_ctx->sync);
   mhi_osal_debug(MSG_HIGH, "Succesfully opened sio port, returning");
}

void mhi_sio_test_init_ctx(mhi_sio_test_ctx_t *ctx, uint32 port)
{
   sio_rx_func_ptr_type func; 

   ctx->open_ptr.port_id = (sio_port_id_type)port;
   ctx->open_ptr.stream_mode=SIO_GENERIC_MODE; 
   
   switch(port)
   {
   case DEVICE_ID(SIO_MAJOR_MHI, 20):
      func=mhi_sio_test_rx_cb1;
      break; 
   case DEVICE_ID(SIO_MAJOR_MHI, 21):
      func=mhi_sio_test_rx_cb2;
      break; 
   case DEVICE_ID(SIO_MAJOR_MHI, 22):
      func=mhi_sio_test_rx_cb3;
      break; 
   case DEVICE_ID(SIO_MAJOR_MHI, 23):
      func=mhi_sio_test_rx_cb4;
      break; 
   case DEVICE_ID(SIO_MAJOR_MHI, 24):
      func=mhi_sio_test_rx_cb5;
      break; 
   default:
      return;
   }

   ctx->open_ptr.rx_func_ptr=func;
   dsm_queue_init(&ctx->tx_wm_q, 0xA00000, &ctx->tx_q);
   ctx->open_ptr.tx_queue=&ctx->tx_wm_q;

   mhi_osal_syncinit(&ctx->sync);
   ctx->open = 0;
}

void mhi_sio_test(void *ctx)
{
    
   /* Initialize the test contests */ 
   mhi_sio_test_init_ctx(&mhi_sio_test_ctx[0], DEVICE_ID(SIO_MAJOR_MHI,20)); 
   mhi_sio_test_init_ctx(&mhi_sio_test_ctx[1], DEVICE_ID(SIO_MAJOR_MHI,21)); 
   mhi_sio_test_init_ctx(&mhi_sio_test_ctx[2], DEVICE_ID(SIO_MAJOR_MHI,22)); 
   mhi_sio_test_init_ctx(&mhi_sio_test_ctx[3], DEVICE_ID(SIO_MAJOR_MHI,23)); 
   mhi_sio_test_init_ctx(&mhi_sio_test_ctx[4], DEVICE_ID(SIO_MAJOR_MHI,24)); 

   /* Spawn additional threads per port */
   mhi_osal_spawn_thread(&threads[0], "MHI_SIO_LP1", mhi_sio_loopback_thread, &mhi_sio_test_ctx[0], 100);
#ifdef MHI_MULTI_LOOBPACK
   mhi_osal_spawn_thread(&threads[1], "MHI_SIO_LP2", mhi_sio_loopback_thread, &mhi_sio_test_ctx[1], 100);
   mhi_osal_spawn_thread(&threads[2], "MHI_SIO_LP3", mhi_sio_loopback_thread, &mhi_sio_test_ctx[2], 100);
   mhi_osal_spawn_thread(&threads[3], "MHI_SIO_LP4", mhi_sio_loopback_thread, &mhi_sio_test_ctx[3], 100);
   mhi_osal_spawn_thread(&threads[4], "MHI_SIO_LP5", mhi_sio_loopback_thread, &mhi_sio_test_ctx[4], 100);
#endif 

   return; 
}
void mhi_loopback_test(void)
{
  mhi_status_t status;  
  void* data; 
  uint32 bytesrw, ret; 
 
  (void)ret;  
  status = mhi_channel_open(0, NULL);
  if(status != MHI_SUCCESS)
  {
     mhi_osal_debug(MSG_HIGH, "mhi_test: Failed to open writechannel\n\r");
  } 

  status = mhi_channel_open(1, NULL);
  if(status != MHI_SUCCESS)
  {
     mhi_osal_debug(MSG_HIGH, "mhi_test: Failed to open writechannel\n\r");
  } 

  /* 2k packet for loopback */
  data=mhi_osal_malloc(0x800);
 

  mhi_loopCount=1; 
  mhi_loop_dropped_bytes=0; 
  mhi_loop_drop_count=0;

  /* Loopback testing */
  for(;;)
  {
    start:
    if(MHI_SUCCESS != mhi_channel_read(0,data,0x800, &bytesrw,NULL))
    {
     mhi_osal_debug(MSG_HIGH, "mhi_test: Failed to read from loopback channel\n\r");
     goto start; 
    }
    
    mhi_osal_debug(MSG_HIGH, "mhi_test: Read succesfful, bytes: %x \n\r, packet #%d", bytesrw, mhi_loopCount);

#ifdef MHI_CONVERT_PACKET
    /* Send back only if packet is good  */ 
    if( 1 == (ret = mhi_test_loopback_convert_packet((uint8*)data)) )
    {
#endif
       if(MHI_SUCCESS != mhi_channel_write(1,data,bytesrw, &bytesrw,NULL))
       {
          mhi_osal_debug(MSG_HIGH, "mhi_test: Failed to write to loopback channel\n\r");
          goto start;
       }
       mhi_osal_debug(MSG_HIGH, "mhi_test: Write succesfful, bytes: %x \n\r", bytesrw);

#ifdef MHI_CONVERT_PACKET
    }
    else
    {
       mhi_osal_debug(MSG_MEDIUM, "mhi_test: Failed to convert loopback packet, reason: %d, drop count: %d", ret, mhi_loop_drop_count); 
       mhi_loop_dropped_bytes+=bytesrw; 
       mhi_loop_drop_count++; 

    }
#endif 
    mhi_loopCount++; 

  }

}

/* Callback for software simulated link inactivity timer */
void mhi_pm_test_link_inact_timer_cb(int32 time_ms, mhi_timer_user_data data)
{
#ifdef MHI_SW_LINK_INACTIVITY
    if (!mhiCtx.mhi_sm_info.device_wake)
    {
       /* Issue link inactivity timer event trigger to the MHI SM */
       mhi_osal_debug(MSG_HIGH,"mhi_pm_test_link_inact_timer_cb: PM_TEST_LINK_INACTIVITY");
       mhi_link_notifications(PCIE_PM_L1_INACT_TIMEOUT, NULL); 
    }

    /* Restart the randomized link inactivity timer */
    mhi_osal_timer_start(&soft_link_inactivity_timer,mhi_pm_test_link_inact_timer_cb,NULL,(rand()+TIME_US_TO_MS(MAX_INACTIVITY))%100,NULL);
#endif /* MHI_SW_LINK_INACTIVITY */
}

/* Initialize and start the software simulated link inactivity timer */
void mhi_pm_test_init_sw_link_inact_timer(void)
{
#ifdef MHI_SW_LINK_INACTIVITY
   /* Initialize software link inactivity timer */ 
   mhi_osal_timer_init(&soft_link_inactivity_timer);
   
   /* Start the randomized link inactivity timer */
   mhi_osal_timer_start(&soft_link_inactivity_timer,mhi_pm_test_link_inact_timer_cb,NULL,(rand()+TIME_US_TO_MS(MAX_INACTIVITY))%100,NULL);
#endif /* MHI_SW_LINK_INACTIVITY */
}

#ifdef MHI_PM_STATE_TEST

/* Artificially trigger events to MHI */
void mhi_send_pm_event(mhi_pm_test_ctrl_t event, mhi_pm_info_t *pm_info)
{
     mhi_pm_info_t *mhi_pm_info = pm_info;
  
     switch (event)
    {
      case PM_TEST_LINK_INACTIVITY:
        mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: PM_TEST_LINK_INACTIVITY");
        mhi_link_notifications(PCIE_PM_L1_INACT_TIMEOUT, NULL);
        break;

      case PM_TEST_ENTER_M0:
        if (mhi_sm_get_mhistate() != MHI_STATE_READY && mhi_sm_get_mhistate() != MHI_STATE_M2)
        {
          /* Host should not write M0 to device until it receives the M3 completion event.
           * Completion events for READY and M2 do not exist.
           */ 
          while (mhi_sm_get_mhistate() != MHI_STATE_M3);
        }
        mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: PM_TEST_ENTER_M0");
        mhi_mmio_set_mhictrl_mhistate(MHI_STATE_M0);        
        break; 
        
      case PM_TEST_ENTER_M2:
        /* Host should not write M2 to device until it receives the M1 completion event */
        while (mhi_sm_get_mhistate() != MHI_STATE_M1);
        mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: PM_TEST_ENTER_M2");
        mhi_mmio_set_mhictrl_mhistate(MHI_STATE_M2);
        break;

      case PM_TEST_ENTER_M3:
        /* Host should not write M3 to device until it receives either the M0 or M1 completion event */
        while (mhi_sm_get_mhistate() != MHI_STATE_M0 && mhi_sm_get_mhistate() != MHI_STATE_M1);
        mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: PM_TEST_ENTER_M3");
        mhi_mmio_set_mhictrl_mhistate(MHI_STATE_M3);
        break;
        
      case PM_TEST_DEVICE_WAKE_HIGH:
        mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: PM_TEST_DEVICE_WAKE_HIGH");
        mhi_pm_info->device_wake_gpio_state = HIGH;
        //mhi_pm_device_wake_isr(NULL);
        mhi_trigger_device_wake_intr();
        break;

      case PM_TEST_DEVICE_WAKE_LOW:
        mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: PM_TEST_DEVICE_WAKE_LOW");
        mhi_pm_info->device_wake_gpio_state = LOW;
        //mhi_pm_device_wake_isr(NULL);
        mhi_trigger_device_wake_intr();
        break;

      case PM_TEST_CORE_WAKEUP:
        mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: PM_TEST_CORE_WAKEUP");
        mhi_pm_core_wakeup();
        mhi_core_signal_worker_thread();
        break;

      case PM_TEST_HW_ACC_WAKEUP:
        mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: PM_TEST_HW_ACC_WAKEUP");
#ifdef MHIIPA
        /* For DL activity emulation send wake-up for pipe 0 (channel 0) */
        ipa_mhi_mgr_sched_event(MHI_MGR_PENDING_EVENT_WAKEUP, 1);
#else
        mhi_pm_hw_acc_wakeup();
#endif
        break; 
        
      case PM_TEST_ENTER_D0:
        mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: PM_TEST_ENTER_D0");
        pcie_change_device_state(0); //This doesn't seem to generate an interrupt.
        //But it still needs to be called, or D3 interrupt won't work either.
        mhi_link_notifications(PCIE_PM_DEVICE_STATE_IN_D0, NULL);
        break;

      case PM_TEST_ENTER_D3hot:
        /* Host should not issue D3hot to device until it receives the M3 completion event */
        while (mhi_sm_get_mhistate() != MHI_STATE_M3);
        mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: PM_TEST_ENTER_D3hot");
        //mhi_link_notifications(PCIE_PM_DEVICE_STATE_IN_D3_HOT, NULL);
        pcie_change_device_state(3);
        break;
        
      case PM_TEST_ENTER_D3cold:
        mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: PM_TEST_ENTER_D3cold");
        //mhi_link_notifications(PCIE_PM_DEVICE_STATE_IN_D3_COLD, NULL);
        pcie_trigger_perst_int(); //PERST ASSERTED
        break;

      case PM_TEST_HOST_WAKEUP_PERST:
        mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: PM_TEST_HOST_WAKEUP_PERST");
        //mhi_link_notifications(PCIE_PM_PERST_DEASSERTED, NULL);
        pcie_trigger_perst_int(); //PERST DEASSERTED
        break;

      case PM_TEST_PCIE_LINKUP:
        mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: PM_TEST_PCIE_LINKUP");
        mhi_link_notifications(PCIE_LINK_UP, NULL);
        /* When link up notification is received, device must be in D0 */
        pcie_change_device_state(0); //This is a hack needed so that pcie_change_device_state(3) will trigger an interrupt.
        break;

      case PM_TEST_MAX:
        mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: PM_TEST_MAX");
        break;
        
      default:   
        mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: Invalid command");
        break;
    }

    //Wait 1ms after issuing each command.
    mhi_osal_wait(1); 

}

void mhi_pm_test_M2_wake_timer_cb(int32 time_ms, mhi_timer_user_data data)
{
  if (MHI_STATE_M2 == mhiCtx.mhi_sm_info.mhiState)
  {
    mhi_osal_debug(MSG_HIGH,"mhi_pm_test_M2_wake_timer_cb: MHI SM in M2, trigger wake up");
    mhi_send_pm_event(PM_TEST_DEVICE_WAKE_HIGH,(mhi_pm_info_t *)data);
  }
}

/* Check for manual test doorbell (db for ch#127) */
void mhi_pm_test_intr_check(void)
{
   /* mmio_base should be initialized by the time the isr is called */
   if(mhi_mmio_read_reg(mmio_base,MHI_TEST_DB_STATUS_REG_OFFSET) & 0x80000000)
   {
      mhi_osal_eventsignal(hNotify_pmt);
      mhi_mmio_write_reg(mmio_base,MHI_TEST_DB_CLEAR_REG_OFFSET,0x80000000);
   }
}

/* Power management and state machine test harness */
void mhi_pm_state_loop_test(mhi_pm_info_t *pm_info)
{
  mhi_timer_t M2_wake_timer;

  mhi_pm_test_ctrl_t command = PM_TEST_MAX;

  mhi_osal_debug(MSG_LOW,"mhi_pm_state_loop_test: Enter");

  /* Create events we will listen on */
  if(MHI_SUCCESS != mhi_osal_eventcreate(&hNotify_pmt, NULL) )
  {
     mhi_osal_debug(MSG_FATAL,"mhi_pm_state_loop_test: Worker thread couldn't create event. Fatal error");
     return; 
  }

#if defined(MHIIPA) && defined(VIRTIO)
  rex_sleep(1);
  ipa_test_mhi_load_uc_and_enable();
#endif

#ifdef PM_TEST_MANUAL
  /* Initialize command to PM_TEST_MAX */
  mhi_mmio_write_reg(mmio_base,MHI_TEST_DB_OFFSET,(uint32)command);
  /* Initialize software link inactivity timer */
  mhi_osal_timer_init(&soft_link_inactivity_timer);

  mhi_osal_timer_init(&M2_wake_timer);
  mhi_osal_timer_start(&M2_wake_timer,mhi_pm_test_M2_wake_timer_cb,(mhi_timer_user_data)pm_info,1000,1000);

  while(1)
  {
#if 0
    /* This allows manipulation via JTAG */
    while(PM_TEST_MAX == *mhi_pm_test_ctrl)
    {
      rex_sleep(1);
    }        
#endif
    /* Wait for event */
    mhi_osal_eventwait(hNotify_pmt);

    mhi_osal_syncenter(&mhiCtx.mhiSync);

    if (MHI_STATE_M2 == mhiCtx.mhi_sm_info.mhiState ||
        MHI_STATE_M3 == mhiCtx.mhi_sm_info.mhiState)
    {
        mhi_osal_debug(MSG_ERR,"mhi_pm_state_loop_test: Event signaled when M state %d",mhiCtx.mhi_sm_info.mhiState);
        mhi_osal_syncleave(&mhiCtx.mhiSync); 
        continue;
    }

    command = (mhi_pm_test_ctrl_t)mhi_mmio_read_reg(mmio_base,MHI_TEST_DB_OFFSET);

    mhi_send_pm_event(command,pm_info);

    if(PM_TEST_DEVICE_WAKE_LOW == command)
    {
      /* Start the random link inactivity timer */
      mhi_osal_timer_start(&soft_link_inactivity_timer,mhi_pm_test_link_inact_timer_cb,NULL,rand()%5,NULL);
    }

    mhi_osal_syncleave(&mhiCtx.mhiSync);
  }
#else  
  {
     /* Automated power managemenet test suite */

     /* Testing valid cases only. Using brute force for now. Also, only numbers,
      * not enum names, are used to save typing.
      * Need to come up with an algorithm for robust testing of the state machine.
      * All sequences below originate and terminate in the M0 state with 
      * DEVICE_WAKE being HIGH.
      *
      * PM_TEST_LINK_INACTIVITY    = 0,
      * PM_TEST_ENTER_M0           = 1,  
      * PM_TEST_ENTER_M2           = 2,
      * PM_TEST_ENTER_M3           = 3,
      * PM_TEST_DEVICE_WAKE_HIGH   = 4,
      * PM_TEST_DEVICE_WAKE_LOW    = 5,
      * PM_TEST_CORE_WAKEUP        = 6,
      * PM_TEST_HW_ACC_WAKEUP      = 7,
      * PM_TEST_ENTER_D0           = 8,
      * PM_TEST_ENTER_D3hot        = 9,
      * PM_TEST_ENTER_D3cold       = 10,
      * PM_TEST_HOST_WAKEUP_PERST  = 11,
      * PM_TEST_PCIE_LINKUP        = 12,
      * 
      * 55 terminates a sequence.
      * 255 terminates the entire test.
      */
     uint32 valid_test_cases[] = {
      /* Seq 1 */ 5, 0, 2, 0, 4, 55,
       
   // 0, 5, 2, 0, 4, 55, Not testing very similar paths for now.
   
      /* Seq 2 */ 5, 0, 2, 0, 6, 4, 55,
      
      /* Seq 3 */ 5, 0, 2, 0, 7, 4, 55,
      
      /* Seq 4 */ 5, 0, 3, 1, 4, 55,//M0->M1->M3->M0
      
      /* Seq 5 */ 5, 0, 3, 9, 8, 1, 4, 55,
      
      /* Seq 6 */ 5, 0, 3, 9, 10, 11, 12, 1, 4, 55,
      
      /* Seq 7 */ 5, 0, 3, 9, 10, 7, 11, 12, 1, 4, 55,
      
      /* Seq 8 */ 3, 9, 8, 1, 55,
      
      /* Seq 9 */ 3, 9, 10, 11, 12, 1, 55,
      
      255
      };

     uint32 i=0;
     uint32 seq=1;

     /* First put the SM into M0 */
     mhi_send_pm_event(PM_TEST_ENTER_M0,pm_info);
      
     /* Walk through the events in the valid_test_cases array */ 
     while (valid_test_cases[i] != 255)
     {
       if (valid_test_cases[i] == 55)
       { 
         mhi_osal_debug(MSG_MEDIUM,"mhi_pm_state_loop_test: Seq %d done",seq++);
         i++;
         continue;
       }

       mhi_send_pm_event((mhi_pm_test_ctrl_t)valid_test_cases[i++],pm_info);
     } 
  }
  NOT_USED(soft_link_inactivity_timer);
  NOT_USED(command);
#endif   
}
#endif /* MHI_PM_STATE_TEST */

