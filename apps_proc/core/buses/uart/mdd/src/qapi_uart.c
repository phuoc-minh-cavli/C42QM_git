/**
 * @file qapi_uart.h
 */

/*==================================================================================================
Copyright (c) 2017-2019,2020 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   qapi_UART_Close
   qapi_UART_Open
   qapi_UART_Receive
   qapi_UART_Transmit
   qapi_UART_Power_On
   qapi_UART_Power_Off
   qapi_UART_Ioctl
==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.tx/6.0/buses/uart/mdd/src/qapi_uart.c#7 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/16/2020   ss     Fixed KW Issue
03/12/2019   ss     Updated datatype of cb_base to uint32_t.
10/06/2017   vg     Added UART DAM changes
12/14/2016   rc     Initial version

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "qapi_uart.h"
#include "DalUart.h"
#include "qurt_mutex.h"
#include "DALDeviceId.h"
#include "malloc.h"
#include "uart_device_props.h"
#include "qurt_txm_qapi_fwk.h"
#include <stringl/stringl.h>
#include"qurt_memory.h"
#include "qapi_uart_handler.h"
#include "qurt_memory.h"
#include "qurt_error.h"



/*==================================================================================================
                                              MACROS
==================================================================================================*/
#define UART_TRACE_BUFFER_FILE_ID     11
#define MAX_BUFFERS 15
#define CB_BASE_AVAILABLE 0
#define CB_BASE_ALLOCATED 1
#define NUM_UART_CBS      8
#define TXM_QAPI_UART_MAX_CB_BASE (TXM_QAPI_BUSES_UART_CB_BASE + NUM_UART_CBS)

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/


typedef struct uart_ports_t
{
  qurt_mutex_t         *pPort_status_lock;
  boolean is_uart_open[QAPI_UART_MAX_PORTS_E];
} uart_ports_t;


static uart_ports_t uart_ports;
/*==================================================================================================
                                         TYPEDEFS
==================================================================================================*/

typedef void (*DALNotifyFunc)( void* );
typedef void (*qurt_cb_config_func)(UINT, UINT);

uart_tx_dispatcher_cb pGlobUartTxCb;
uart_tx_dispatcher_cb pGlobUartRxCb;

typedef struct uart_device_properties
{
  boolean                  is_enable_dma;
  DALDEVICEID              dal_device_id;
} uart_device_properties;

typedef struct cb_info
{
  char                    *pBuf;
  uint32_t                size;
  uint32_t                bytes_pending;
  void                    *pCbData;
  struct cb_info          *pNext;
} cb_info;

typedef struct tx_data
{
  qurt_mutex_t            *pLock;
  cb_info                 *pTxHead;
  cb_info                 *pTxCurPtr;
  qurt_cb_config_func     tx_cb_func; 
  uint32_t                 tx_cb_base;
} tx_data;

typedef struct rx_data
{
  qurt_mutex_t            *pLock;
  cb_info                 *pRxHead;
  qurt_cb_config_func     rx_cb_func;  
  uint32_t                  rx_cb_base;
} rx_data;

typedef struct uart_handle
{
  void                    *pDeviceHandle;
  qapi_UART_Port_Id_e     port_id;
  qapi_UART_Open_Config_t *pUartConfig;
  boolean                 is_tx_in_progress;
  qurt_mutex_t            *port_lock_handle;
  tx_data                 *pTxp; 
  rx_data                 *pRxp;
  boolean                  is_port_open;
  boolean                  is_clock_on;
} uart_handle;

static boolean cb_base_array[8];




/** @} */ /* end_addtogroup qapi_uart */


/** @addtogroup qapi_uart
@{ */

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static qurt_mutex_t *create_lock(void);
static void destroy_lock(qurt_mutex_t *mutex_lock);
static void	tx_cb_isr( uart_handle *uart_h );
static void	rx_cb_isr( uart_handle *uart_h );
static boolean PopHeadNode(cb_info **pRoot, cb_info *PHead);
static uint32 PushTailNode( cb_info **pRoot, char *pBuf, uint32_t size, void *pCbData );
static boolean uart_get_device_props(qapi_UART_Port_Id_e id, uart_device_properties *device_info );
static boolean uart_transfer( uart_handle *uart_h);
/*==================================================================================================
                                    LOCAL FUNCTION DEFINITIONS
==================================================================================================*/

static qurt_mutex_t *create_lock()
{
  qurt_mutex_t *mutex_lock;

  mutex_lock = NULL;
  mutex_lock = malloc(sizeof(qurt_mutex_t));
  
  if(mutex_lock != NULL)
  {
    qurt_mutex_init(mutex_lock);
  }

  return mutex_lock;
}

static void destroy_lock(qurt_mutex_t *mutex_lock)
{
  if(mutex_lock != NULL)
  {
    qurt_mutex_destroy(mutex_lock);
	free(mutex_lock);
	mutex_lock = NULL;
  }
}
static uint32 PushTailNode( cb_info **PHead, char *pBuf, uint32_t size, void *pCbData )
{
  cb_info *pNode,*pNode1;
  
  if(PHead == NULL)
  {
    return FALSE;
  }
  pNode = malloc(sizeof(cb_info));
	
  if(pNode == NULL)
  {
    return FALSE;
  }
  
  pNode->pBuf      = pBuf;
  pNode->size      = size;
  pNode->pCbData   = pCbData;
  pNode->pNext     = NULL;
   
  if (*PHead == NULL)
  {
    *PHead = pNode; 
  }
  else
  {
    pNode1 = *PHead;
  
    while(pNode1->pNext != NULL)
    {
      pNode1->pNext = pNode1->pNext->pNext;
    }
    pNode1->pNext = pNode;
  }
  return TRUE;
}

static boolean PopHeadNode(cb_info **pHead, cb_info *pNode)
{
  cb_info *pTnode=NULL,*pTnode1=NULL;

  if((NULL == pNode) || (NULL == pHead))
  {
    return FALSE;
  }
  pTnode = *pHead;

  if (NULL != pTnode)
  {
    if(pTnode->pBuf == NULL)
   	{
	 free(pTnode);                                 //this condition never appear but need to take care 
   	 return FALSE;
   	}
    pNode->pBuf      = pTnode->pBuf;
	pNode->size      = pTnode->size;
	pNode->pCbData   = pTnode->pCbData;
   	pNode->pNext     = NULL;
	
	if(pTnode->pNext !=  NULL)
	{
	  pTnode1 = pTnode->pNext;
	  *pHead = pTnode1;
	}
	else
	{
	 *pHead = NULL;
	}
	free(pTnode);
	pTnode = NULL;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

static void rx_cb_isr(uart_handle *uart_h)
{
  cb_info *pRxNode      = NULL;
  uint8   *pBuf         = NULL;
  DALResult result      = UART_ERROR;
  uint32   bytes_read   = 0;
  uint32   read_size    = 0;
  uint32 overrun_events = 0;
  uint32 cb_idx = 0;
  UartStatusType status = (UartStatusType)0;                       
  
  if(uart_h == NULL)
  {
	UART_LOG_0( ERROR, "rx_cb_isr:null handle" );
    return;
  }
  qurt_mutex_lock(uart_h->pRxp->pLock);

  pRxNode = malloc(sizeof(cb_info));
  if(pRxNode == NULL)
  {
    UART_LOG_0( TRACE, "[%d] pRxNode:memory alloc failed" );
	return;
  }
  memsset(pRxNode,0,sizeof(cb_info));

  
  while(TRUE == PopHeadNode(&uart_h->pRxp->pRxHead, pRxNode))
  {
    pBuf = (uint8 *)pRxNode->pBuf;
	read_size = pRxNode->size;
    result = DalUart_Read(uart_h->pDeviceHandle ,pBuf,
		                  read_size,&bytes_read, &overrun_events);
	if(result == UART_SUCCESS)
	{
	  UART_BUFFER_LOG_2( pBuf, bytes_read, "[%d] Recived %d bytes:", 0, bytes_read );
      pRxNode->size = bytes_read;
	  if(uart_h->pUartConfig->user_mode_client)
	  {
         uart_h->pRxp->rx_cb_func(pRxNode->size, (UINT)pRxNode->pCbData);
         cb_idx                    = uart_h->pRxp->rx_cb_base - TXM_QAPI_BUSES_UART_CB_BASE;
         cb_base_array[cb_idx]     = CB_BASE_AVAILABLE;
	  }
	  else
	  {
	      if(uart_h->pUartConfig->rx_CB_ISR != NULL)   	  
		  {   	    
		      uart_h->pUartConfig->rx_CB_ISR(pRxNode->size,pRxNode->pCbData);	  
		  }   
	  }
	  result = DalUart_GetStatus(uart_h->pDeviceHandle,&status);
	  if((result == UART_SUCCESS) && (status & STATUS_RX_BUFFER_EMPTY))
	  {
	    break;
	  }
	  bytes_read = 0;
    }
  }
	free(pRxNode);
	pRxNode = NULL;
    
  qurt_mutex_unlock(uart_h->pRxp->pLock);
}


static void
tx_cb_isr( uart_handle *uart_h )
{
  uint32_t        port_id = QAPI_UART_MAX_PORTS_E;
  uint32_t        cb_idx  = 0;
  
  
  if(uart_h == NULL)
  {
	UART_LOG_0( ERROR, "tx_cb_isr:null handle" );
    return;
  }
  
  port_id = uart_h->port_id;
  UART_LOG_1( TRACE, "[%d] +tx_cb_isr", port_id );

  qurt_mutex_lock( uart_h->pTxp->pLock);
  
  if(uart_h->pTxp->pTxCurPtr->bytes_pending == 0)
  {
	if(uart_h->pUartConfig->user_mode_client)
	{
       uart_h->pTxp->tx_cb_func(uart_h->pTxp->pTxCurPtr->size, (UINT)uart_h->pTxp->pTxCurPtr->pCbData);
       cb_idx = uart_h->pTxp->tx_cb_base - TXM_QAPI_BUSES_UART_CB_BASE;
       cb_base_array[cb_idx] = CB_BASE_AVAILABLE;
	}
	else
	{
	    if(uart_h->pUartConfig->tx_CB_ISR != NULL)   	 
		{   	    
           uart_h->pUartConfig->tx_CB_ISR(uart_h->pTxp->pTxCurPtr->size, uart_h->pTxp->pTxCurPtr->pCbData);	
		}
	}
	uart_h->pTxp->pTxCurPtr->pBuf      = NULL;
	uart_h->pTxp->pTxCurPtr->size    = 0;
	uart_h->pTxp->pTxCurPtr->pCbData = NULL; 
	
	if(!(uart_transfer( uart_h)))
	{
	  UART_LOG_1( TRACE, "[%d] uart transfer is failed", port_id );	
	}
  }
  else
  {
	if(!(uart_transfer( uart_h)))
	{
	  UART_LOG_1( TRACE, "[%d] uart transfer is failed", port_id );	
	}
  }
  qurt_mutex_unlock(uart_h->pTxp->pLock);

  UART_LOG_1( TRACE, "[%d] -tx_cb_isr", port_id );
}

static boolean uart_get_device_props(qapi_UART_Port_Id_e id, uart_device_properties *device_info )
{
   DALDEVICEID    dal_device_id = UART_INVALID_DALDEVICEID;
   uint32_t       driver_type = NO_DRIVER;
   boolean result = FALSE;
   DALSYS_PROPERTY_HANDLE_DECLARE( handle );
   
   UART_LOG_1( TRACE, "[%d] +uart_get_device_props", id );

   if(device_info == NULL)
   {
     UART_LOG_1( ERROR, "[%d] -uart_get_device_props: passed NULL pointer", id );
	 return result;
   }

   if(id >= QAPI_UART_MAX_PORTS_E)
   {  
     UART_LOG_1( ERROR, "[%d] -uart_get_device_props: unrecognized device ID", id );
	 return result;
   }

   do
   {
     if( DALSYS_GetDALPropertyHandleStr( "qapi_uart_props", handle ) != DAL_SUCCESS )
     {
       UART_LOG_0( ERROR, "-uart_get_device_props: couldn't get uart_build_props property handle" );
       break;
     }
 
	 switch(id)
     {
       case QAPI_UART_PORT_001_E:
	 	 get_dword_property( handle, "UART_FIRST_QAPI_ID", &dal_device_id,
                                UART_INVALID_DALDEVICEID );
         get_dword_property( handle, "UART_FIRST_QAPI_ID_TYPE", (uint32 *)&driver_type,
                                NO_DRIVER );
		 break;
       case QAPI_UART_PORT_002_E:
		   get_dword_property( handle, "UART_SECOND_QAPI_ID", &dal_device_id,
								  UART_INVALID_DALDEVICEID );
		   get_dword_property( handle, "UART_SECOND_QAPI_ID_TYPE", (uint32 *)&driver_type,
								  NO_DRIVER );
		   break;
       case QAPI_UART_PORT_003_E:
		   get_dword_property( handle, "UART_THIRD_QAPI_ID", &dal_device_id,
								  UART_INVALID_DALDEVICEID );
		   get_dword_property( handle, "UART_THIRD_QAPI_ID_TYPE", (uint32 *)&driver_type,
								  NO_DRIVER );
		   break;
       case QAPI_UART_PORT_004_E:
		   get_dword_property( handle, "UART_FOURTH_QAPI_ID", &dal_device_id,
								  UART_INVALID_DALDEVICEID );
		   get_dword_property( handle, "UART_FOURTH_QAPI_ID_TYPE", (uint32 *)&driver_type,
								  NO_DRIVER );
		   break;		   
	   case QAPI_UART_PORT_005_E:
		   get_dword_property( handle, "UART_FIFTH_QAPI_ID", &dal_device_id,
									  UART_INVALID_DALDEVICEID );
		   get_dword_property( handle, "UART_FIFTH_QAPI_ID_TYPE", (uint32 *)&driver_type,
									  NO_DRIVER );
		   break;
	   case QAPI_UART_PORT_006_E:
		   get_dword_property( handle, "UART_SIXTH_QAPI_ID", &dal_device_id,
										  UART_INVALID_DALDEVICEID );
		   get_dword_property( handle, "UART_SIXTH_QAPI_ID_TYPE", (uint32 *)&driver_type,
										  NO_DRIVER );
			   break;
	   defaut:
	   	   dal_device_id = UART_INVALID_DALDEVICEID;
		   driver_type = NO_DRIVER;
		   break;
	   	   
 	}

    if(( dal_device_id == UART_INVALID_DALDEVICEID ) || (driver_type == NO_DRIVER))
    {
	  UART_LOG_1( ERROR, "[%d] NO DRIVER/ invalid device ID", id );
      result = FALSE;
	  break;
    }
    else
    {
      device_info->dal_device_id = dal_device_id;
	  result = TRUE;
    }
   
    switch( driver_type )
    {
      case BAM_DRIVER:
	  case DM_DRIVER:
	    device_info->is_enable_dma = TRUE;
	    break;
	  default:
        device_info->is_enable_dma = FALSE;
	    break;
    }
  }while(0);
	 
   UART_LOG_1( TRACE, "[%d] -uart_get_device_props", id );
   return result;
}

static boolean uart_transfer( uart_handle *uart_h)
{
  uint32 bytes_written  = 0;
  uint32 num_bytes      = 0;
  const uint8 *pBuf     = NULL;
  uint8 *pBuf1          = NULL;
  cb_info *tx_cb_ptr    = NULL;
  DALResult dal_result  = UART_ERROR;
  boolean result        = FALSE;
  
  do
  {
    if(uart_h->pTxp->pTxCurPtr->bytes_pending != 0) 
    {
      pBuf        = (const uint8 *)uart_h->pTxp->pTxCurPtr->pBuf;
	  num_bytes  = uart_h->pTxp->pTxCurPtr->bytes_pending;
    }
    else
    {
      if((PopHeadNode(&uart_h->pTxp->pTxHead, uart_h->pTxp->pTxCurPtr)))
      {
	    pBuf        = (const uint8 *)uart_h->pTxp->pTxCurPtr->pBuf;
	    num_bytes  = uart_h->pTxp->pTxCurPtr->size;
      }
      else
	  {
	    result = TRUE;
	    uart_h->is_tx_in_progress = FALSE;
		free(uart_h->pTxp->pTxCurPtr);
        uart_h->pTxp->pTxCurPtr = NULL;		
		break;
	  }
    }

    dal_result = DalUart_Write(uart_h->pDeviceHandle, pBuf, 
					 num_bytes, &bytes_written);
	
    if(dal_result == UART_SUCCESS)
    {
      if(num_bytes != bytes_written)
      {
        uart_h->pTxp->pTxCurPtr->pBuf += bytes_written;
      }
      uart_h->pTxp->pTxCurPtr->bytes_pending = num_bytes - bytes_written;
	  uart_h->is_tx_in_progress = TRUE;
	  result = TRUE;
	  pBuf1 = (uint8 *)uart_h->pTxp->pTxCurPtr->pBuf; 
	  UART_BUFFER_LOG_2( pBuf1, num_bytes, "[%d] TX %d bytes:", 0, num_bytes );
    }
  }while(0);

  return result;
}
/*==================================================================================================
                                    PUBLIC FUNCTION DEFINITIONS
==================================================================================================*/
/**
 * Closes the UART port. Not to be called from ISR context.
 *
 * Releases clock, interrupt, and GPIO handles related to this UART and
 *          cancels any pending transfers.
 * 
 * @note1hang Do not call this API from ISR context.
 * 
 * @param[in] handle      UART handle provided by qapi_UART_Open().
 * 
 * @return
 * QAPI_OK      Port close successful. \n
 * QAPI_ERROR   Port close failed.
 * 
 */

qapi_Status_t qapi_UART_Close
(  
   qapi_UART_Handle_t handle
)
{
  uart_handle *uart_h;
  boolean     cb_idx;


  if(handle == NULL)
  {
    return QAPI_ERROR;
  }
  
  uart_h = (uart_handle *)handle;
  uart_h->is_port_open = FALSE;
  if(uart_h->pDeviceHandle != NULL)
  {
    DalUart_Close(uart_h->pDeviceHandle);
    DalUart_DeInit(uart_h->pDeviceHandle);
  }

  destroy_lock(uart_h->port_lock_handle);
  
  if(uart_h->pTxp != NULL)
  {
    destroy_lock(uart_h->pTxp->pLock);
	free(uart_h->pTxp);
	uart_h->pTxp = NULL;
  }
  
  if(uart_h->pRxp != NULL)
  {
    destroy_lock(uart_h->pRxp->pLock);
	free(uart_h->pRxp);
	uart_h->pRxp = NULL;
  }  
  
  qurt_mutex_lock(uart_ports.pPort_status_lock);
  uart_ports.is_uart_open[uart_h->port_id] = FALSE;
  qurt_mutex_unlock(uart_ports.pPort_status_lock);
  if(uart_h->pUartConfig != NULL)
  {
	  free(uart_h->pUartConfig);
	  uart_h->pUartConfig = NULL;
  }
	
  free(uart_h);
  uart_h = NULL;
  
  return QAPI_OK;
}
/**
 * Initializes the UART port. Not to be called from ISR context.
 *
 * Opens the UART port and configures the corresponding clocks, interrupts, and GPIO.
 * 
 * @note1hang Do not call this API from ISR context.
 * 
 * @param[in] handle UART handle. 
 * @param[in] id ID of the port to be opened.
 * @param[in] config Structure that holds all configuration data.
 *  
 * @return 
 * QAPI_OK      Port open successful. \n
 * QAPI_ERROR   Port open failed.
 *  
 *  
 */

qapi_Status_t qapi_UART_Open
(
   qapi_UART_Handle_t*        handle, 
   qapi_UART_Port_Id_e        id, 
   qapi_UART_Open_Config_t*   config
)
{
  UartInitConfig              init_Config;
  UartOpenConfig              open_config;
  uart_device_properties      device_info;
  uart_handle                 *uart_h            = NULL;  
  void                        *dal_device_handle = NULL;
  boolean                     port_locked        = FALSE;
  qapi_Status_t               result             = QAPI_ERROR;
  uint32                      cb_idx = 0;
  void                        *pTxCbFunc;
  void                        *pRxCbFunc;


  if((id >= QAPI_UART_MAX_PORTS_E) || (handle == NULL) || (config == NULL))
  {
    return result;
  }

  if(uart_ports.pPort_status_lock == NULL)
  {
    uart_ports.pPort_status_lock = create_lock(); //todo need to move it to rc_init function
	if(uart_ports.pPort_status_lock == NULL)
	{
	  return result;
    }
  }

  qurt_mutex_lock(uart_ports.pPort_status_lock);
  
  if(uart_ports.is_uart_open[id] == TRUE)
  {
    qurt_mutex_unlock(uart_ports.pPort_status_lock);
    return result;
  }
  else
  {
    uart_ports.is_uart_open[id] = TRUE;
	qurt_mutex_unlock(uart_ports.pPort_status_lock);
  }

  do
  {        
	uart_h = malloc(sizeof(uart_handle));
	if(uart_h == NULL)
	{
      UART_LOG_1( ERROR, "[%d] memory allocation failed uart_h: %d.", id);
      break;
	}
	
	memsset(uart_h,0,sizeof(uart_handle));

   	uart_h->port_lock_handle = create_lock();
	if(uart_h->port_lock_handle == NULL)
	{
      UART_LOG_1( ERROR, "[%d] mutex_lock for port initialization failed: %d.", id);
      break;
	}
	
	qurt_mutex_lock(uart_h->port_lock_handle);
	port_locked = TRUE;

	uart_h->pTxp = malloc(sizeof(tx_data));
	if(uart_h->pTxp == NULL)
	{
      UART_LOG_1( ERROR, "[%d] pTxp memory allocation failed : %d.", id);
      break;
	}
    memsset(uart_h->pTxp,0,sizeof(tx_data));
	
    uart_h->pTxp->pLock = create_lock();
	if(uart_h->pTxp->pLock == NULL)
	{
      UART_LOG_1( ERROR, "[%d] mutex_lock for tx initialization failed: %d.", id);
      break;
	}
     
	uart_h->pRxp = malloc(sizeof(rx_data));
	if(uart_h->pRxp == NULL)
	{
      UART_LOG_1( ERROR, "[%d] pRxp memory allocation failed: %d.", id);
      break;
	}
	memsset(uart_h->pRxp,0,sizeof(rx_data));

	uart_h->pRxp->pLock = create_lock();
	if(uart_h->pRxp->pLock == NULL)
	{
      UART_LOG_1( ERROR, "[%d] mutex_lock for rx initialization failed: %d.", id);
      break;
	}

	if(!(uart_get_device_props(id , &device_info)))
    {
      break;
    }

	
	init_Config.device_id          = (void*)device_info.dal_device_id;
    init_Config.Ctxt               = uart_h; 
    init_Config.enable_dma         = device_info.is_enable_dma;
    init_Config.uart_type          = UART_BAM;
    init_Config.uart_irq           = 0;  // Read from property file.  Don't override here.
    init_Config.rx_irq             = 0;  // Read from property file.  Don't override here.
    init_Config.LineEventNotif     =  NULL;//todo rajesh
    init_Config.TransmitReadyNotif =  (DALNotifyFunc)tx_cb_isr; 
    init_Config.ReceiveReadyNotif  =  (DALNotifyFunc)rx_cb_isr; 
    init_Config.CxmMisalignNotif   = NULL;
    init_Config.CxmMsgNotif        = NULL;
    init_Config.CxmRatNotif        = NULL;

    dal_device_handle = DalUart_Init(&init_Config);
    if(dal_device_handle == NULL)
    {
	  UART_LOG_1( ERROR, "[%d] DalUart_Init failed.", id );
	  break;
    }

    
    open_config.char_format.ParityMode  = (UartParityModeType)(1<<config->parity_Mode);
    open_config.char_format.BitsPerChar = (UartBitsPerCharType)(1<<config->bits_Per_Char);
    open_config.char_format.NumStopBits = (UartNumStopBitsType)(1<<config->num_Stop_Bits);
    open_config.baud_rate               = config->baud_Rate;
    open_config.rts_control             = RTS_AUTO; 
    open_config.cts_control             = CTS_ENABLE; 

 
    if((DalUart_Open(dal_device_handle, &open_config ) != UART_SUCCESS))
    {
      UART_LOG_1( ERROR, "[%d] DalUart_Open failed: %d.", id);
      break;
    }
	
	uart_h->pUartConfig = malloc(sizeof(qapi_UART_Open_Config_t));
	if(uart_h->pUartConfig == NULL)
	{
      UART_LOG_1( ERROR, "[%d] memory allocation failed: %d.", id);
      break;
	}
	memsset(uart_h->pUartConfig,0,sizeof(qapi_UART_Open_Config_t));


	uart_h->pDeviceHandle = dal_device_handle;
	uart_h->port_id = id;
	uart_h->pUartConfig->parity_Mode   = config->parity_Mode;
	uart_h->pUartConfig->bits_Per_Char = config->bits_Per_Char;
	uart_h->pUartConfig->num_Stop_Bits = config->num_Stop_Bits;
	uart_h->pUartConfig->tx_CB_ISR     = config->tx_CB_ISR;
	uart_h->pUartConfig->rx_CB_ISR     = config->rx_CB_ISR;
	uart_h->pUartConfig->user_mode_client = config->user_mode_client;
	uart_h->is_clock_on  = TRUE;
	uart_h->is_port_open = TRUE;
	*handle = (qapi_UART_Handle_t )uart_h;
	

	if(config->enable_Loopback)
	{
	  DalUart_LoopbackSet(dal_device_handle,config->enable_Loopback);
	}
	
    result = QAPI_OK;

    if(uart_h->pUartConfig->user_mode_client)
	{
       if(uart_h->pUartConfig->tx_CB_ISR != NULL)
   	   { 
           for(cb_idx=0 ; cb_idx< NUM_UART_CBS; cb_idx++)
	       {
	         if(cb_base_array[cb_idx] == CB_BASE_AVAILABLE)
	         {
	            uart_h->pTxp->tx_cb_base = TXM_QAPI_BUSES_UART_CB_BASE + cb_idx;
	     	    cb_base_array[cb_idx]   = CB_BASE_ALLOCATED;
    	        break;
             }
           }
	       if(!(qurt_qapi_callback_configure(uart_h->pTxp->tx_cb_base, // type
   	                                        (void *)0xDEADCAFE, // dummy obj_ptr
   	                                        0,	// id
   	                                       (void *)uart_h->pUartConfig->tx_CB_ISR, // test_api1 - a3: app_cb_dispatcher
   	                                       (void *)pGlobUartTxCb, // test_api1 - a2: app_cb
   	                                       (void**)&uart_h->pTxp->tx_cb_func,
   	                                        NULL)))
   	      {
   	        if((uart_h->pTxp->tx_cb_func) == NULL)
   	        {
              result = QAPI_ERROR;
		      goto exit;
            }
	      }
   	  
   	   }
	   else
	   {
           result = QAPI_ERROR;
	       goto exit;
	   }
	}
	else
	{
		if(uart_h->pUartConfig->tx_CB_ISR == NULL)
		{
		   result = QAPI_ERROR;
	       goto exit;
		}
	}
	
	if(uart_h->pUartConfig->user_mode_client)
	{
       if(uart_h->pUartConfig->rx_CB_ISR != NULL)
 	   {
           for(cb_idx=0 ; cb_idx< NUM_UART_CBS; cb_idx++)
	       {
	          if(cb_base_array[cb_idx] == CB_BASE_AVAILABLE)
	          {
	             uart_h->pRxp->rx_cb_base = TXM_QAPI_BUSES_UART_CB_BASE + cb_idx;
	        	 cb_base_array[cb_idx]   = CB_BASE_ALLOCATED;
       	         break;
              }
           }
   	      if(!(qurt_qapi_callback_configure(uart_h->pRxp->rx_cb_base, // type
   	                                       (void *)0xDEADCAFE, // dummy obj_ptr
   	                                        0,	// id
   	                                       (void *)uart_h->pUartConfig->rx_CB_ISR, // test_api1 - a3: app_cb_dispatcher
   	                                       (void *)pGlobUartRxCb, // test_api1 - a2: app_cb
   	                                       (void**)&uart_h->pRxp->rx_cb_func,
   	                                       NULL)))
   	       {
   	         if((uart_h->pRxp->rx_cb_func) == NULL)
   	         {
               result = QAPI_ERROR;
	   		   goto exit;
             }
	       }
	   }
	   else
	   {
	       result = QAPI_ERROR;
	       goto exit;
	   }
	}
    else
	{
		if(uart_h->pUartConfig->rx_CB_ISR == NULL)
		{
		   result = QAPI_ERROR;
	       goto exit;
		}
	}
	
    qurt_mutex_unlock(uart_h->port_lock_handle);
	port_locked = FALSE;
  }while(0);

 
exit: 
  if(result != QAPI_OK)
  {
    if(port_locked)
   	{
      qurt_mutex_unlock(uart_h->port_lock_handle);
    }
	qapi_UART_Close(uart_h);
  }  
  return result;

}

/**
 * Queues the buffer provided for receiving the data. Not to be called from ISR context.
 *
 * This is an asynchronous call. rx_cb_isr is called when the Rx transfer completes.
 * The buffer is owned by the UART driver until rx_cb_isr is called.
 * 
 * There must always be a pending Rx. The UART hardware has a limited buffer
 * (FIFO), and if there is no software buffer available, for HS-UART,
 * the flow control will de-assert the RFR line.
 * @newpage           
 * Call uart_receive immediately after uart_open to queue a buffer.
 * After every rx_cb_isr, from a different non-ISR thread, queue the next transfer.
 * 
 * There can be a maximum of 2 buffers queued at a time.
 * 
 * @note1hang Do not call this API from ISR context.
 * 
 * @param[in] handle       UART handle provided by qapi_UART_Open().
 * @param[in] buf          Buffer to be filled with data.                                         
 * @param[in] buf_Size     Buffer size. Must be @ge 4 and a multiple of 4. 
 * @param[in] cb_Data      Callback data to be passed when rx_cb_isr is called 
 *                         during Rx completion.
 *  
 * @return 
 * QAPI_OK      Queuing of the receive buffer was successful. \n
 * QAPI_ERROR   Queuing of the receive buffer failed.
 *  
 */

qapi_Status_t qapi_UART_Receive
(
   qapi_UART_Handle_t   handle,
   char*                buf, 
   uint32_t             buf_Size, 
   void*                cb_Data
)
{
  uart_handle *uart_h    = NULL;
  cb_info     *rx_cb_ptr = NULL;
  qapi_Status_t  result  = QAPI_ERROR;

  if((handle == NULL) || (buf == NULL) || (buf_Size == 0))
  {
    UART_LOG_0( ERROR, "Invalid parameter");
    return result;
  }

  uart_h = (uart_handle *)(handle);
  
  if((!(uart_h->is_port_open) || (!(uart_h->is_clock_on))))
  {
    return result;
  }
  
  if(uart_h->pUartConfig->user_mode_client)
  {
     if (TRUE != qurt_check_if_module_address((qurt_addr_t)buf, buf_Size))
     {
	    result = QAPI_ERROR;
	    UART_LOG_0( ERROR, "Address doesnot belong to current module!");
	    return result;
     }
  }
  
  UART_LOG_1( TRACE, "[%d]+qapi_UART_Receive",uart_h->port_id);
  
  qurt_mutex_lock(uart_h->port_lock_handle);
  qurt_mutex_lock(uart_h->pRxp->pLock);
  
  PushTailNode(&uart_h->pRxp->pRxHead, buf, buf_Size, cb_Data );
  result = QAPI_OK;
  
  qurt_mutex_unlock(uart_h->pRxp->pLock);
  qurt_mutex_unlock(uart_h->port_lock_handle);
  
  UART_LOG_1( TRACE, "[%d]-qapi_UART_Receive",uart_h->port_id);
  return result;	
}

/**
 * Transmits data from a specified buffer. Not to be called from ISR context.
 *
 * This is an asynchronous call. The buffer is queued for Tx, and when transmit is completed,
 * tx_cb_isr is called.
 * 
 * The buffer is owned by the UART driver until tx_cb_isr is called.
 * 
 * @note1hang Do not call this API from ISR context.
 *
 * @param[in] handle         UART handle provided by qapi_UART_Open().
 * @param[in] buf            Buffer with data for transmit. 
 * @param[in] bytes_To_Tx    Bytes of data to transmit
 * @param[in] cb_Data        Callback data to be passed when tx_cb_isr is called 
 *                           during Tx completion. 
 *  
 * @return 
 * QAPI_OK      Queuing of the transmit buffer was successful. \n
 * QAPI_ERROR   Queuing of the transmit buffer failed. 
 *  
 *  
 */

qapi_Status_t qapi_UART_Transmit
(
   qapi_UART_Handle_t handle, 
   char* buf, 
   uint32_t bytes_To_Tx, 
   void* cb_Data
)
{
  uart_handle *uart_h    = NULL;
  cb_info     *tx_cb_ptr = NULL;
  qapi_Status_t result   = QAPI_ERROR;
  

  if((handle == NULL) || (buf == NULL) || (bytes_To_Tx == 0))
  {
    UART_LOG_0( ERROR, "Invalid parameter");
    return result;
  }

  uart_h = (uart_handle *)(handle);

  if((!(uart_h->is_port_open) || (!(uart_h->is_clock_on))))
  {
    return result;
  }
  if(uart_h->pUartConfig->user_mode_client)
  {
      if (TRUE != qurt_check_if_module_address((qurt_addr_t)buf, bytes_To_Tx))
	  {
	    result = QAPI_ERROR;
	    UART_LOG_0( ERROR, "Address doesnot belong to current module!");
	    return result;
	  }
  }
  
  UART_LOG_1( TRACE, "[%d] +qapi_UART_Transmit",uart_h->port_id);

  qurt_mutex_lock(uart_h->port_lock_handle);
  qurt_mutex_lock(uart_h->pTxp->pLock);

  do
  {
    if(uart_h->is_tx_in_progress != TRUE)
    {
      if(uart_h->pTxp->pTxCurPtr == NULL)
      {
        uart_h->pTxp->pTxCurPtr = malloc(sizeof(cb_info));
		if(uart_h->pTxp->pTxCurPtr == NULL)
		{
	      result = QAPI_ERROR;
	      break;		  
        }
      }
	  
	  uart_h->pTxp->pTxCurPtr->pBuf          = buf;
	  uart_h->pTxp->pTxCurPtr->size          = bytes_To_Tx;
  	  uart_h->pTxp->pTxCurPtr->bytes_pending = bytes_To_Tx;
   	  uart_h->pTxp->pTxCurPtr->pCbData      = cb_Data;
	  uart_h->pTxp->pTxCurPtr->pNext         = NULL;
	  
	  if(!(uart_transfer( uart_h)))
	  {
	    result = QAPI_ERROR;
	    break;
	  }
  	}
    else
   	{
      PushTailNode(&uart_h->pTxp->pTxHead, buf,bytes_To_Tx, cb_Data );
   	}
    result = QAPI_OK;
  }while(0);

  qurt_mutex_unlock(uart_h->pTxp->pLock);
  qurt_mutex_unlock(uart_h->port_lock_handle);
  UART_LOG_1( TRACE, "[%d] -qapi_UART_Transmit",uart_h->port_id);
  
  return result;
}

/*==================================================================================
  FUNCTION      qapi_UART_Power_On
==================================================================================*/
/**
*  Enables the UART Hardware resources for an UART transaction.
*
*  This function enables all resources required for a successful UART transaction. This 
*  includes clocks, power resources and pin multiplex functions. This function should 
*  be called before a transfer or a batch of UART transfers.
* 
*  @param[in] UART_Handle  Driver handle returned by \c qapi_UART_Open.
*
*  @return
*  QAPI_OK      UART powered on successfully. \n
*  QAPI_ERROR   UART power on is failed. 
*/
qapi_Status_t
qapi_UART_Power_On
(
    qapi_UART_Handle_t handle
)
{
  uart_handle *uart_h;
  qapi_Status_t result = QAPI_ERROR;
  DALResult            dal_result;

  if(handle == NULL)
  {
    return result;
  }

  uart_h = (uart_handle *)handle;

  qurt_mutex_lock(uart_h->port_lock_handle);
  do
  {
    if((uart_h->is_port_open != TRUE) && (uart_h->is_clock_on == TRUE))
  	{
  	  break;
   	}

	dal_result = DalUart_PowerSet(uart_h->pDeviceHandle,POWER_FULL);
	
	if( UART_SUCCESS == dal_result )
    {
      result = QAPI_OK;
	  uart_h->is_clock_on = TRUE;
    }
    else
    {
      UART_LOG_2( TRACE, "[%d] Error, DalUart_PowerSet() = %d.", uart_h->port_id, dal_result );
      result = QAPI_ERROR;
    }
	
  }while(0);
  qurt_mutex_unlock(uart_h->port_lock_handle);
  return result;
  
}

/*==================================================================================
  FUNCTION      qapi_UART_Power_Off
==================================================================================*/
/**
*  Disables the UART Hardware resources for an UART transaction.
*
*  This function turns off all resources used by the UART master. This includes clocks, 
*  power resources and GPIOs. This function should be called to put the UART master
*  in its lowest possible power state.
* 
*  @param[in] UART_Handle  Driver handle returned by \c qapi_UART_Open.
*
*  @return
*  QAPI_OK      UART powered off successfully. \n
*  QAPI_ERROR   UART power off is failed.
*/
qapi_Status_t 
qapi_UART_Power_Off 
(
    qapi_UART_Handle_t handle
)
{
	uart_handle *uart_h;
	qapi_Status_t result = QAPI_ERROR;
	DALResult			 dal_result;
	
	uart_h = (uart_handle *)handle;
	
	qurt_mutex_lock(uart_h->port_lock_handle);
	do
	{
	  if((uart_h->is_port_open != TRUE) && (uart_h->is_clock_on != TRUE))
	  {
		break;
	  }
	  qurt_mutex_lock(uart_h->pTxp->pLock); //todo if tx_progress need to moved to tx_cb
	  
	  if(uart_h->is_tx_in_progress)
	  {
		qurt_mutex_unlock(uart_h->pTxp->pLock);	  
		break;
	  }
	  qurt_mutex_unlock(uart_h->pTxp->pLock); //todo   
	
	  dal_result = DalUart_PowerSet(uart_h->pDeviceHandle,POWER_OFF);
	  
	  if( UART_SUCCESS == dal_result )
	  {
		result = QAPI_OK;
		uart_h->is_clock_on = FALSE;
	  }
	  else
	  {
		UART_LOG_2( TRACE, "[%d] Error, DalUart_PowerSet() = %d.", uart_h->port_id, dal_result );
		result = QAPI_ERROR;
	  }
	  
	}while(0);
	qurt_mutex_unlock(uart_h->port_lock_handle);
	return result;

}
/*==================================================================================
  FUNCTION      qapi_UART_Ioctl
==================================================================================*/
/**
*  Controls the UART configurations for an UART transaction.
*
*  This function control the UART configurations apart from the IO operations which 
*  can not be achieved through standard API's.
* 
*  @param[in] UART_Handle  Driver handle returned by \c qapi_UART_Open.
*  @param[in] ioctl_Command Pass the commnds listed with the enum qapi_UART_Ioctl_Command_e.
*  @param[in] ioctl_Param   Pass the argument associated with the ioctl_Command.
*  @return
*  QAPI_OK      UART IOCTL configuration is successfull. \n
*  QAPI_ERROR   UART IOCTL configuration is failed.
*/
qapi_Status_t qapi_UART_Ioctl
(    
 qapi_UART_Handle_t handle,
 qapi_UART_Ioctl_Command_e ioctl_Command,
  void *ioctl_Param
)
{
  uart_handle *uart_h;
  uint32 baud_rate;
  QAPI_Flow_Control_Type flow_ctrl_type;
  QAPI_UART_Ioctl_Param *param;
  qapi_Status_t qapi_result = QAPI_ERROR;
  DALResult dalresult;
  qapi_Status_t result   = QAPI_ERROR;

 if (TRUE != qurt_check_if_module_address((qurt_addr_t)ioctl_Param, sizeof(ioctl_Param)))
	{
	  result = QAPI_ERROR;
	  UART_LOG_0( ERROR, "Address doesnot belong to current module!");
	  return result;
	}
  uart_h = (uart_handle *)handle;

  qurt_mutex_lock(uart_h->port_lock_handle);
  if((!(uart_h->is_port_open)) && (!(uart_h->is_clock_on)))
  {
    qurt_mutex_unlock(uart_h->port_lock_handle);
    return qapi_result;
  }
  qurt_mutex_unlock(uart_h->port_lock_handle);

  switch(ioctl_Command)
  {
    case QAPI_SET_FLOW_CTRL_E:
	  param = (QAPI_UART_Ioctl_Param *)ioctl_Param;
      qurt_mutex_lock(uart_h->pTxp->pLock);
      qurt_mutex_lock(uart_h->pRxp->pLock);
	  if(param->Flow_Control_Type == QAPI_CTSRFR_AUTO_FCTL_E)
      {
   	    DalUart_SetCtsControl( uart_h->pDeviceHandle, CTS_ENABLE );
        DalUart_SetRtsControl( uart_h->pDeviceHandle, RTS_AUTO );
		qapi_result = QAPI_OK;
	  }
	  else if(param->Flow_Control_Type == QAPI_FCTL_OFF_E)
	  {
        DalUart_SetCtsControl( uart_h->pDeviceHandle, CTS_DISABLE );   
        DalUart_SetRtsControl( uart_h->pDeviceHandle, RTS_DEASSERT );
       	qapi_result = QAPI_OK;
	  }
	  else
	  {
   		qapi_result = QAPI_ERROR;
	  }
	  qurt_mutex_unlock(uart_h->pRxp->pLock);
	  qurt_mutex_unlock(uart_h->pTxp->pLock);
	break;
	case QAPI_SET_BAUD_RATE_E:
	  param = (QAPI_UART_Ioctl_Param *)ioctl_Param;
      qurt_mutex_lock(uart_h->port_lock_handle);
      qurt_mutex_lock(uart_h->pTxp->pLock);
	  if(uart_h->is_tx_in_progress)
	  {
        qurt_mutex_unlock(uart_h->pTxp->pLock);	  
    	qurt_mutex_unlock(uart_h->port_lock_handle);
		qapi_result = QAPI_ERROR;
		break;
	  }
      qurt_mutex_unlock(uart_h->pTxp->pLock);
	  baud_rate = param->baud_Rate;
	  dalresult = DalUart_SetRate(uart_h->pDeviceHandle, baud_rate);
	  if(dalresult != UART_SUCCESS)
	  {
    	qapi_result = QAPI_ERROR;
	  }
	  else
      {
        uart_h->pUartConfig->baud_Rate = baud_rate;
	    qapi_result = QAPI_OK;
	  }
	  qurt_mutex_unlock(uart_h->port_lock_handle);
	  break;
	default:
	  break;
  }
return qapi_result;
}
/** @} */ /* end_addtogroup qapi_uart */
