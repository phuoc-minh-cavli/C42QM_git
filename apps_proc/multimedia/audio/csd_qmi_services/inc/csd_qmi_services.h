#ifndef __AUDIO_CSD_QMI_SERVICES_H__
#define __AUDIO_CSD_QMI_SERVICES_H__
/*===========================================================================

      A U D I O _ C S D _ Q M I _ S E R V I C E   H E A D E R    F I L E

DESCRIPTION


Copyright(c) 2010 - 2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd_qmi_services/inc/csd_qmi_services.h#1 $
$DateTime: 2018/12/25 22:39:07 $ 
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
30/06/14    rp     Added support for Calibration Feature ID  
02/13/13    jk     Added PAUSE feature support  
02/13/13    rp     Added AANC feature support  
04/19/12    rp     Added support for voice standby mode
09/06/11    akj    Added Data declaration.
07/06/11    jp     Initial version.

===========================================================================*/
/****************************************************************************
  INCLUDE HEADER FILES
****************************************************************************/

#include <stddef.h>
#include <string.h>
#include "qmi_idl_lib.h"
#include "qmi_client.h"
#include "qmi_sap.h"
#include "qmi_csi.h"

#include "core_sound_driver_v01.h"
#include "csd.h"
#include "csd_dev_ioctl.h"
#include "csd_vc_ioctl.h"
#include "csd_vm_ioctl.h"
#include "csd_vs_ioctl.h"
#include "csd_status.h"
#include "csd_vcommon.h"
#include "csd_debug_msg.h"



/****************************************************************************
   DEFINES
****************************************************************************/

/* This struct holds the arguments that need to be passed to the service reader thread */
typedef struct {
  service_context_type *service_cookie;
  qmi_csi_os_params *os_params;
}reader_args_type;

/* ENUM to specify what CSD Handle type*/
enum csd_ioctl_type_opcode
{
    QMI_CSD_NOT_USED                    = 0,	
    QMI_CSD_AUDIO_STREAM                = 1,	
    QMI_CSD_VOICE_STREAM                = 2,	
    QMI_CSD_AUDIO_CONTEXT               = 3,	
    QMI_CSD_VOICE_CONTEXT               = 4,	
    QMI_CSD_DEVICE_CONTROL              = 5,	
    QMI_CSD_DEBUG_DEVICE_CONTROL        = 6,	
    QMI_CSD_DEBUG_AUDIO_STREAM_CONTROL  = 7,	
    QMI_CSD_DEBUG_AUDIO_CONTEXT_CONTROL = 8,	
    QMI_CSD_DEBUG_VOICE_CONTROL	        = 9,	
    QMI_CSD_VOICE_MANAGER               = 10,	
    QMI_CSD_MAX_NUM							
      /**< Maximum number. */
};

static uint32_t  csd_req_handle_table[] = 
{   NULL,                                 //20 CSD_GET_VERSION
    NULL,                                 //21 CSD_INIT
    NULL,                                 //22 CSD_DINIT
    NULL,                                 //23 CSD_OPEN_VOICE_STREAM_PASSIVE
    NULL,                                 //24 CSD_OPEN_VOICE_STREAM_FULL
    NULL,                                 //25 CSD_OPEN_VOICE_CONTEXT_FULL
    NULL,                                 //26 CSD_OPEN_VOICE_MANAGER_FULL
    NULL,                                 //27 CSD_OPEN_DEVICE_CONTROL_FULL
    NULL,                                 //28
    CSD_DEV_CMD_ENABLE,                   //29
    CSD_DEV_CMD_DISABLE,                  //2A
    CSD_DEV_CMD_AFE_LOOPBACK,             //2B                                                       
    CSD_DEV_CMD_ANC_CONTROL,              //2C
    CSD_DEV_CMD_COMPANDING_CONTROL,       //2D
    CSD_DEV_CMD_GET_MAX_DEV_NUMS,         //2E
    CSD_DEV_CMD_GET_DEV_CAPS,             //2F
    CSD_DEV_CMD_DTMF_CONTROL,             //30
    CSD_DEV_CMD_SIDETONE_CONTROL,         //31
    CSD_DEV_CMD_CONFIGURE,                //32
    CSD_VC_IOCTL_SET_DEVICE_CONFIG,       //33
    CSD_VC_IOCTL_ENABLE,                  //34
    CSD_VC_IOCTL_DISABLE,                 //35
    CSD_VC_IOCTL_SET_RX_VOLUME_INDEX,     //36                                
    CSD_VC_IOCTL_SET_MUTE,                //37
    CSD_VC_IOCTL_SET_TX_DTMF_DETECTION,   //38
    NULL,                                 //39
    CSD_VC_IOCTL_SET_UI_PROPERTY,         //3A
    CSD_VC_IOCTL_GET_UI_PROPERTY,         //3B
    NULL,                                 //3C
    CSD_VS_IOCTL_SET_MEDIA_TYPE,          //3D
    CSD_VS_IOCTL_SET_MUTE,                //3E
    CSD_VS_IOCTL_SET_ENCODER_DTX_MODE,    //3F
    CSD_VS_IOCTL_SET_DEC_TIMEWARP,        //40
    CSD_VS_IOCTL_SET_ENC_MINMAX_RATE,     //41
    CSD_VS_IOCTL_SET_ENC_RATE_MODULATION, //42
    CSD_VS_IOCTL_VOC_QCELP13K_SET_RATE,   //43
    CSD_VS_IOCTL_VOC_4GVNB_SET_RATE,      //44
    CSD_VS_IOCTL_VOC_4GVWB_SET_RATE,      //45
    CSD_VS_IOCTL_VOC_AMR_SET_ENC_RATE,    //46
    CSD_VS_IOCTL_VOC_AMRWB_SET_ENC_RATE,  //47
    CSD_VS_IOCTL_SET_DTMF_GENERATION,     //48
    NULL,                                 //49
    CSD_VS_IOCTL_SET_RX_DTMF_DETECTION,   //4A
    NULL,                                 //4B
    CSD_VS_IOCTL_SET_UI_PROPERTY,         //4C
    CSD_VS_IOCTL_GET_UI_PROPERTY,         //4D
    CSD_VS_IOCTL_START_RECORD,            //4E
    CSD_VS_IOCTL_STOP_RECORD,             //4F                        
    NULL,                                 //50
    NULL,                                 //51
    NULL,                                 //52
    CSD_VM_IOCTL_ATTACH_STREAM,           //53
    CSD_VM_IOCTL_DETACH_STREAM,           //54
    CSD_VM_IOCTL_ATTACH_CONTEXT,          //55
    CSD_VM_IOCTL_DETACH_CONTEXT,          //56
    CSD_VM_IOCTL_START_VOICE,             //57
    CSD_VM_IOCTL_STOP_VOICE,              //58
    CSD_VM_IOCTL_SET_NETWORK,             //59
    CSD_VM_IOCTL_SET_VOICE_TIMING,        //5A
    CSD_VM_IOCTL_SET_TTY_MODE,            //5B
    CSD_VM_IOCTL_SET_WIDEVOICE,           //5C
        /* Space for audio messages */
    NULL,                                 //5D     
    NULL,                                 //5E    
    NULL,                                 //5F     							        
    NULL,                                 //60     							        
    NULL,                                 //61     							        
    NULL,                                 //62     							      
    NULL,                                 //63     							      
    NULL,                                 //64     								        
    NULL,                                 //65     						      
    NULL,                                 //66    							      
    NULL,                                 //67 
    NULL,                                 //68 
    NULL,                                 //69 
    NULL,                                 //6A 
    NULL,                                 //6B 
    NULL,                                 //6C 
    NULL,                                 //6D 
    NULL,                                 //6E 
    NULL,                                 //6F 
    NULL,                                 //70 
    NULL,                                 //71 
    NULL,                                 //72 
    NULL,                                 //73 
    NULL,                                 //74 
    NULL,                                 //75 
    NULL,                                 //76 
    NULL,                                 //77 
    NULL,                                 //78 
    NULL,                                 //79 
    NULL,                                 //7A 
    NULL,                                 //7B     										            
    NULL,                                 //7C 
    NULL,                                 //7D 
    NULL,                                 //7E 
    NULL,                                 //7F 
    NULL,                                 //80 
    NULL,                                 //81 
    NULL,                                 //82 
    NULL,                                 //83     							        
    NULL,                                 //84     							        
    NULL,                                 //85     						    
    NULL,                                 //86     					    
    NULL,                                 //87     							      
    NULL,                                 //88     								          
    NULL,                                 //89			      
    NULL,                                 //8A  
    NULL,                                 //8B	    
    NULL,                                 //8C	  
    NULL,                                 //8D
    NULL,                                 //8E
    NULL,                                 //8F			        
    NULL,                                 //90     								          
    NULL,                                 //91     								          
    NULL,                                 //92     							          
    NULL,                                 //93     									          
    NULL,                                 //94     									          
    NULL,                                 //95     				  
    NULL,                                 //96     				  
    NULL,                                 //97     		
    NULL,                                 //98    						      
    NULL,                                 //99     						    
    NULL,                                 //9A    			 				        
    NULL,                                 //9B     							        
    NULL,                                 //9C     							        
    NULL,                                 //9D     							            
    /* New messages added for CSD- QMI revision 1.2 */
    CSD_DEV_CMD_CONNECT_DEVICE,               //9E
    CSD_VC_IOCTL_SET_NUMBER_OF_VOLUME_STEPS,  //9F
    CSD_VC_IOCTL_SET_RX_VOLUME_STEP,          //A0
    CSD_VS_IOCTL_START_PLAYBACK,              //A1
    CSD_VS_IOCTL_STOP_PLAYBACK,               //A2
    CSD_VM_IOCTL_STANDBY_VOICE,               //A3
    CSD_DEV_CMD_AANC_CONTROL,                 //A4
    CSD_VM_IOCTL_PAUSE_VOICE,                 //A5
    CSD_DEV_CMD_RESTART,                      //A6
    CSD_VC_IOCTL_SET_CAL_FID,                 //A7
    CSD_VM_IOCTL_SET_HDVOICE_MODE             //A8
	};


/*===========================================================================
 Function Prototype

===========================================================================*/
uint32_t qmi_csd_client_info_list_init(void);
uint32_t qmi_csd_client_info_list_dinit(void);
  
extern qmi_csi_cb_error csd_open_device_control_req_handler 
( client_info_type *clnt_info, qmi_req_handle req_handle, int32_t msg_id,
  void *req_c_struct, int32_t req_c_struct_len, void *service_cookie);

extern qmi_csi_cb_error qmi_csd_ioctl_dev_cmd_req_handler
( uint32_t handle, uint32_t cmd_msg_id, client_info_type *clnt_info,
  qmi_req_handle req_handle, int32_t msg_id, void *req_c_struct,
  int32_t req_c_struct_len, void *service_cookie );

extern qmi_csi_cb_error csd_open_voice_context_req_handler 
( client_info_type *clnt_info, qmi_req_handle req_handle, int32_t msg_id,
  void *req_c_struct, int32_t req_c_struct_len, void *service_cookie);							   

extern qmi_csi_cb_error qmi_csd_ioctl_vc_cmd_req_handler
( uint32_t handle, uint32_t cmd_msg_id, client_info_type *clnt_info,
  qmi_req_handle req_handle, int32_t msg_id, void *req_c_struct,
  int32_t req_c_struct_len, void *service_cookie );

extern qmi_csi_cb_error csd_open_voice_manager_req_handler 
( client_info_type *clnt_info, qmi_req_handle req_handle,
  int32_t msg_id, void *req_c_struct, int32_t req_c_struct_len,
  void *service_cookie);

extern qmi_csi_cb_error qmi_csd_ioctl_vm_cmd_req_handler
( uint32_t handle, uint32_t cmd_msg_id, client_info_type *clnt_info,
  qmi_req_handle req_handle, int32_t msg_id, void *req_c_struct,
  int32_t req_c_struct_len, void *service_cookie );

extern qmi_csi_cb_error csd_open_passive_control_voice_stream_req_handler 
( client_info_type *clnt_info, qmi_req_handle req_handle,
  int32_t msg_id, void *req_c_struct, int32_t req_c_struct_len,
  void *service_cookie);

extern qmi_csi_cb_error csd_open_full_control_voice_stream_req_handler 
( client_info_type *clnt_info, qmi_req_handle req_handle,
  int32_t msg_id, void *req_c_struct, int32_t req_c_struct_len,
  void *service_cookie);

extern qmi_csi_cb_error qmi_csd_ioctl_vs_cmd_req_handler
( uint32_t handle, uint32_t cmd_msg_id, client_info_type *clnt_info,
  qmi_req_handle req_handle, int32_t msg_id, void *req_c_struct,
  int32_t req_c_struct_len, void *service_cookie );  						   

extern qmi_csi_cb_error qmi_csdlite_handle_req
( client_info_type *clnt_info, qmi_req_handle req_handle,
  int32_t msg_id, void *req_c_struct, int32_t req_c_struct_len,
  void *service_cookie);
  
#endif /* __AUDIO_CSD_QMI_SERVICES_H__*/

