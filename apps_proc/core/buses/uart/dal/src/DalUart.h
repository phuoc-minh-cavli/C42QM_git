#ifndef __DALUART_H__
#define __DALUART_H__
/*==================================================================================================

FILE: DalUart.h

DESCRIPTION: UART DAL (OS-independent layer)

                     Copyright (c) 2009-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     Qualcomm Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.tx/6.0/buses/uart/dal/src/DalUart.h#1 $

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#include "DalUartTypes.h"

/*==================================================================================================
                                            ENUMERATIONS
==================================================================================================*/

enum
{
   UART_SUCCESS = 0,
   UART_ERROR,
};

typedef enum
{
   UART_DM = 0,
   UART_BAM,
} UartType;

typedef enum
{
   EVENT_NONE        = 0,
   EVENT_BREAK       = (1<<0),   // continuous Space value w/o framing detected on input
   EVENT_CTS         = (1<<1),   // CTS changed state
   EVENT_DSR         = (1<<3),   // DSR changed state
   EVENT_ERROR       = (1<<4),   // Line-status error (PARITY, FRAMING, OVERRUN) occurred
   EVENT_RX_WAKEUP   = (1<<5),   // a falling edge (wakeup byte) was detected while sleeping
   EVENT_RING        = (1<<7),
   EVENT_RLSD        = (1<<8),   // RLSD changed state
   EVENT_RX_CHAR     = (1<<9),   // one or more RX chars have been received
   EVENT_RX_HUNTCHAR = (1<<10),  // the hunt char has been received
   EVENT_TXEMPTY     = (1<<11),  // last char was moved from TXFIFO to unpacking buffer
   EVENT_BREAK_END   = (1<<12),  // End of break condition
} UartLineEventType;

typedef enum
{
   UART_NO_PARITY      = (1<<0),
   UART_ODD_PARITY     = (1<<1),
   UART_EVEN_PARITY    = (1<<2),
   UART_SPACE_PARITY   = (1<<3),
   UART_INVALID_PARITY = 0x7FFFFFFF
} UartParityModeType;

typedef enum
{
   UART_5_BITS_PER_CHAR       = (1<<0),
   UART_6_BITS_PER_CHAR       = (1<<1),
   UART_7_BITS_PER_CHAR       = (1<<2),
   UART_8_BITS_PER_CHAR       = (1<<3),
   UART_INVALID_BITS_PER_CHAR = 0x7FFFFFFF
} UartBitsPerCharType;

typedef enum
{
   UART_0_5_STOP_BITS     = (1<<0),
   UART_1_0_STOP_BITS     = (1<<1),
   UART_1_5_STOP_BITS     = (1<<2),
   UART_2_0_STOP_BITS     = (1<<3),
   UART_INVALID_STOP_BITS = 0x7FFFFFFF
} UartNumStopBitsType;

typedef enum
{
   RTS_DEASSERT,  // manually deassert RTS
   RTS_ASSERT,    // manually assert RTS
   RTS_AUTO,      // RTS automatically controlled by HW (based on level of RX FIFO)
} UartRtsControlType;

typedef enum
{
   CTS_DISABLE,  // UART transmitter will ignore CTS
   CTS_ENABLE,   // UART transmitter will automatically halt when CTS is deasserted
} UartCtsControlType;

typedef enum
{
   STATUS_RX_PARITY_ERROR       = (1<<0),
   STATUS_RX_FRAME_ERROR        = (1<<1),
   STATUS_RX_OVERRUN_ERROR      = (1<<2),
   STATUS_RX_BREAK              = (1<<3),
   STATUS_RX_HUNT_CHAR_DETECTED = (1<<4),
   STATUS_RX_FULL               = (1<<5),
   STATUS_RX_READY              = (1<<6),
   STATUS_TX_EMPTY              = (1<<7),
   STATUS_TX_READY              = (1<<8),
   STATUS_RING_ASSERTED         = (1<<16),
   STATUS_RLSD_ASSERTED         = (1<<17),
   STATUS_DSR_ASSERTED          = (1<<18),
   STATUS_CTS_ASSERTED          = (1<<19),
   STATUS_RX_BUFFER_EMPTY       = (1<<20),
} UartStatusType;

typedef enum
{
   POWER_INVALID = 0,
   POWER_FULL    = (1<<0),
   POWER_075     = (1<<1),
   POWER_050     = (1<<2),
   POWER_025     = (1<<3),
   POWER_OFF     = (1<<4),
} UartPowerType;

/*==================================================================================================
                                             STRUCTURES
==================================================================================================*/

// The IRQs are opaque data types that describe interrupt sources.  Each IRQ is cast to
// (void *) while passing through the UART DAL layer and recast to the appropriate data type
// inside the OS-specific interrupt controller.  The OS-specific data types are as follows:
//
// WM6 - SYSINTR mapped from a HAL IRQ
// WM7 - HAL IRQ (HAL_PIC_xxx_IRQ)
// AMSS - the irq enumerated in the tramp table
// Symbian - same as the value used by Symbian kernel class Interrupt,
//           as defined in the enum TMSMHwInterruptId
// WOA - pointer to struct that encapsulates vector, IRQL, CPU affinity, etc.

typedef struct
{
   void *   device_id;                                       // handle to MDD device private data
   UartType uart_type;                                       // type of UART device
   void *   uart_irq;                                        // IRQ for UART generated events
   void *   rx_irq;                                          // IRQ for the raw RX signal
   uint32   enable_dma;                                      // request DMA mode if available
   void *   Ctxt;                                            // context passed to interrupt handlers
   void     (*LineEventNotif)(void *Ctxt,                    // interrupt handler for line events
                              UartLineEventType LineEvents);
   void     (*TransmitReadyNotif)(void *Ctxt);               // interrupt handler for tx events
   void     (*ReceiveReadyNotif)(void *Ctxt);                // interrupt handler for rx events
   void     (*CxmMisalignNotif)(void *Ctxt, uint32 status);  // interrupt handler for misalign event
   void     (*CxmMsgNotif)(void *Ctxt, uint32 status,        // interrupt handler for wci2 msg event
                           void *wci2_msgs, uint32 length);
   void     (*CxmRatNotif)(void *Ctxt, uint32 status);    // interrupt handler for rat event
} UartInitConfig;

typedef struct
{
   UartParityModeType  ParityMode;
   UartBitsPerCharType BitsPerChar;
   UartNumStopBitsType NumStopBits;
} UartCharFormat;

typedef struct
{
   uint32             baud_rate;    // baud rate to use when opening the port
   UartCharFormat     char_format;  // character format to use when opening the port
   UartRtsControlType rts_control;  // initial RTS state
   UartCtsControlType cts_control;  // whether to obey CTS during transmit operations
} UartOpenConfig;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*!
 * \brief Causes the transmitter signal to go high
 *
 * If executed while channel is breaking, this command causes the transmitter
 * signal to go high. The signal remains high for at least one bit time before
 * sending out a new character.
 *
 * \param in h Handle to UART returned by attach
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
uint32 DalUart_ClearBreak(void *h);

/*!
 * \brief Close UART device
 *
 * This function is called to close the port for a given UART device.  It will
 * perform a graceful shutdown of the UART hardware and release any resources
 * allocated during DalUart_Open().
 *
 * \param in h   Opaque handle returned during init
 */
void DalUart_Close(void *h);

/*!
 * \brief Writes a byte to direct TX register of CXM UART for immediate transfer
 * \param in  h handle to UART returned by attach
 * \param in  character       Data byte to be sent immediately
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
uint32 DalUart_CxmTxDirectChar(void *h, uint8 character);

/*!
 * \brief Deinitialize UART device
 *
 * This function is called once (generally during system shutdown) for each UART
 * device that is present in the system.  It will deallocate anything that was
 * allocated during DalUart_Init().
 *
 * \param in h   Opaque handle returned during init
 */
void DalUart_DeInit(void *h);

/*!
 * \brief Prints the contents of the UART hardware registers to the UART log
 * for debugging
 * \param in  h handle to UART returned by attach
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
uint32 DalUart_DumpRegs(void *h);

/*!
 * \brief Gets current state of CTS
 * \param in h Handle to UART returned by attach
 * \param in cts_state is pointer to current CTS state
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
uint32 DalUart_GetCTS(void *h, uint32 *cts_state);

/*!
 * \brief Reads and optionally clears the 802.11 TX sticky flag
 * \param in  h handle to UART returned by attach
 * \param out tx_sticky       Returns the value of the TX sticky flag
 * \param in  clear           TRUE to clear the TX sticky flag
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
uint32 DalUart_GetCxmTxSticky(void *h, uint32 *tx_sticky, uint32 clear);

/*!
 * \brief Gets the status of the UART device
 *
 * Gets the status of the UART as an ORed combination of applicable values
 *
 * \param in h Handle to UART returned by attach
 * \param in status pointer to a uint32 to be filled with the status value
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
uint32 DalUart_GetStatus(void *h, UartStatusType *status);

/*!
 * \brief Initialize UART device
 *
 * This function is called once (generally at boot time) for each UART device that
 * is present in the system.  It will allocate an object to represent the device
 * and perform one-time initialization.
 *
 * \param in init_cfg    Pointer to configuration settings
 * \return               Opaque handle to DalUart device private data (NULL for failure)
 */
void *DalUart_Init(UartInitConfig *init_cfg);

/*!
 * \brief Requests a change in the loopback state
 * \param in  h handle to UART returned by attach
 * \param in  enabled         TRUE to enable internal loopback, FALSE to disable
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
uint32 DalUart_LoopbackSet(void *h, uint32 enabled);

/*!
 * \brief Open UART device
 *
 * This function is called to open the port for a given UART device.  It will
 * initialize the UART hardware and allocate necessary resources.
 *
 * \param in h          Opaque handle returned during init
 * \param in open_cfg   Pointer to configuration settings
 * \return              DAL_SUCCESS|DAL_ERROR
 */
uint32 DalUart_Open(void *h, UartOpenConfig *open_cfg);

/*!
 * \brief Gets a list of supported power states
 * \param in  h handle to UART returned by attach
 * \param out power_mask      pointer to a mask of supported power states
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
uint32 DalUart_PowerCapabilities(void *h, uint32 *power_mask);

/*!
 * \brief Gets the current power state
 * \param in  h handle to UART returned by attach
 * \param out power_state     pointer to location to write the current power state
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
uint32 DalUart_PowerGet(void *h, UartPowerType *power_state);

/*!
 * \brief Requests a change in the power state
 * \param in  h handle to UART returned by attach
 * \param in  power_state     requested power state
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
uint32 DalUart_PowerSet(void *h, UartPowerType power_state);

/*!
 * \brief Purges device's receiver
 *
 * Purges the device's receive FIFO and unpacking buffer
 *
 * \param in h Handle to UART returned by attach
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
uint32 DalUart_PurgeRx(void *h);

/*!
 * \brief Purges device's transmitter
 *
 * Purges the device's transmit FIFO and packing buffer
 *
 * \param in h Handle to UART returned by attach
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
uint32 DalUart_PurgeTx(void *h);

/*!
 * \brief Retrieves data available in receive buffer.
 *
 * Should be called from client in response to call of client's registered
 * receive-ready notification callback.
 *
 * \param in h Handle to UART returned by attach
 * \param in overrun_events Buffer to be filled in with number of receive overrun events
 * \param in read_buffer Buffer to be filled with received data
 * \param in buffer_len Length of buffer pointed to by pReadBuffer
 * \param in bytes_read Buffer to be filled with length of data actually read
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
uint32 DalUart_Read(void *h, uint8 *read_buffer, uint32 buffer_len,
                    uint32 *bytes_read, uint32 *overrun_events);

/*!
 * \brief Gets a boolean indicating if there is an active RX transfer
 * \param in h handle to UART returned by attach
 * \param in rx_active       pointer to a uint32 to be filled with the boolean value
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
uint32 DalUart_RxActive(void *h, uint32 *rx_active);

/*!
 * \brief Forces the transmitter signal low
 *
 * The transmitter must be enabled. If the transmitter is busy, the break is
 * started when all characters in the transmit FIFO and the transmit shift
 * register have been completely sent
 *
 * \param in h Handle to UART returned by attach
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
uint32 DalUart_SetBreak(void *h);

/*!
 * \brief Sets the character format parameters for device
 *
 * Sets the character format parameters (UartBitsPerCharType, UartNumStopBitsType,
 * UartParityModeType) to one of supported values
 *
 * \param in h Handle to UART returned by attach
 * \param in char_format pointer to a UartCharFormat type to be filled in
 * \param in dwCharFormatLen size of UartCharFormat
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
uint32 DalUart_SetCharFormat(void *h, UartCharFormat *char_format);

/*!
 * \brief Sets the current type of CTS control (tx flow control)
 * \param in h Handle to UART returned by attach
 * \param in cts_control the type of CTS control
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
uint32 DalUart_SetCtsControl(void *h, UartCtsControlType cts_control);

/*!
 * \brief Enables or disables two-wire coexistence
 * \param in  h handle to UART returned by attach
 * \param in  enabled         TRUE to enable two-wire coexistence, FALSE to disable
 * \param in  sam             The station address included in certain CXM messages
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
uint32 DalUart_SetCxm(void *h, uint32 enabled, uint32 sam);

/*!
 * \brief Sets the interrupt mask for the wci2 messages
 * \param in  h handle to UART returned by attach
 * \param in  intr_mask       The mask for the wci2 messages interrupt 
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
uint32 DalUart_SetCxmMsgIntrMask(void *h, uint32 intr_mask);

/*!
 * \brief Enables or disables coexistence type 2 mode
 * \param in  h handle to UART returned by attach
 * \param in  enabled         TRUE to enable TYPE2 mode in CXM core. 
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
uint32 DalUart_SetCxmType2Mode(void *h, uint32 enabled);

/*!
 * \brief Enables or disables coexistence type 2 mode
 * \param in  h handle to UART returned by attach
 * \param in  enabled         TRUE to enable TYPE2 mode in CXM core. 
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 */
uint32 DalUart_SetCxmRatIdMode(void *h, uint32 rat_msg_type, 
                                        boolean enable_rat_msg_to_fifo,
                                        boolean enable_tcm_resend,
                                        uint32 sam, boolean enabled);

/*!
 * \brief Sets the current baud rate for device
 *
 * Sets the current baud rate, changing the device's clock source and divisor setting as needed
 *
 * \param in h Handle to UART returned by attach
 * \param in BaudRate one of supported UartBaudRateType
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
uint32 DalUart_SetRate(void *h, uint32 baud_rate);

/*!
 * \brief Sets the current type of RTS control (rx flow control)
 * \param in h Handle to UART returned by attach
 * \param in rts_control the type of RTS control
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
uint32 DalUart_SetRtsControl(void *h, UartRtsControlType rts_control);

/*!
 * \brief Gets a boolean indicating if transmission is complete
 *
 * A value of true indicates the stop bit at the end of the last character from
 * the transmit FIFO has been sent. It's reset to false when a character is written
 * to the TX fifo
 *
 * \param in h Handle to UART returned by attach
 * \param in tx_complete pointer to a uint32 to be filled with the boolean value
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
uint32 DalUart_TxComplete(void *h, uint32 *tx_complete);

/*!
 * \brief Sends a single character out the transmit port
 *
 * Usually used for XON/XOFF software flow control characters
 *
 * \param in h Handle to UART returned by attach
 * \param in dwChar the character to send
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning This function will not send the character immediately but only after
 *   all data currently enqueued
 */
uint32 DalUart_TxSingleChar(void *h, uint8 character);

/*!
 * \brief Sends data from supplied buffer.
 *
 * Should be called from client for initial transmission of a quantity of data
 * and in response to call of client's registered transmit-ready notification
 * callback for any follow-on transmissions
 *
 * \param in h Handle to UART returned by attach
 * \param in write_buffer Buffer containing data to be sent
 * \param in buffer_len Quantity of data pointed to by write_buffer
 * \return DAL_SUCCESS|DAL_ERROR, See DALStdErr.h
 * \note
 * \warning
 */
uint32 DalUart_Write(void *h, const uint8 *write_buffer, uint32 buffer_len, uint32 *bytes_written);

#endif  // __DALUART_H__
