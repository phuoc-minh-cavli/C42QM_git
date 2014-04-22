#pragma once
/**
 * sns_com_port_types.h
 *
 * Common definitions used by synch_com_port (SCP) and
 * asynch_com_port (ACP).
 *
 * Copyright (c) 2016 - 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <inttypes.h>
#include <stdbool.h>

/**
 * Types of register addresses.
 */
typedef enum
{
  SNS_REG_ADDR_8_BIT,
  SNS_REG_ADDR_16_BIT,
  SNS_REG_ADDR_32_BIT,
  /* Additional register types will be added here. */
} sns_reg_addr_type;

/**
 * Types of communication ports.
 */
typedef enum
{
  SNS_BUS_MIN            = 0,
  SNS_BUS_I2C            = SNS_BUS_MIN,
  SNS_BUS_SPI            = 1,
  SNS_BUS_UART           = 2, // DEPRECATED. Please use the async_uart sensor
  SNS_BUS_I3C_SDR        = 3, // I3C Standard Data Rate
  SNS_BUS_I3C_HDR_DDR    = 4, // I3C Double Data Rate
  SNS_BUS_I3C_I2C_LEGACY = 5, // for I2C devices on an I3C bus
  SNS_BUS_MAX            = SNS_BUS_I3C_I2C_LEGACY,
} sns_bus_type;

typedef struct
{
  sns_bus_type       bus_type;           /* Bus type from sns_bus_type.*/
  uint32_t           slave_control;      /* Slave Address for I2C.
                                            Dynamic slave address I3C.
                                            Chip Select for SPI.*/
  sns_reg_addr_type  reg_addr_type;      /* Register address type for the slave.*/
  uint32_t           min_bus_speed_KHz;  /* Minimum bus clock supported by slave in kHz.*/
  uint32_t           max_bus_speed_KHz;  /* Maximum bus clock supported by slave in kHz.*/
  uint8_t            bus_instance;       /* Platform bus instance number (BLSP number).*/

} sns_com_port_config;

/**
 * Read or Write transaction on a port
 */
typedef struct
{
  bool             is_write;     /* true for a write operation. false for read.*/
  uint32_t         reg_addr;     /* Register address for tranfer.*/
  uint32_t         bytes;        /* Number of bytes to read or write */
  uint8_t          *buffer;      /* Buffer to read or write */
} sns_port_vector;

/**
 * Common Command Codes
 *
 * See Mipi Alliance Specificiation for I3C for complete information
 */

/* Masks for the GETSTATUS result byte */
/** 0 if no interrupts are pending. If more than one interrupt is set, contains
 * the highest priority interrupt */
#define SNS_CCC_STATUS_IRQ_PENDING_MASK    0x0F
/** Reserved for future use */
#define SNS_CCC_STATUS_RESERVED_MASK       0x10
/** Set to 1 if slave detects protocol error since the last status read */
#define SNS_CCC_STATUS_PROTOCOL_ERROR_MASK 0x20
/** Slave's current activity mode */
#define SNS_CCC_STATUS_ACTIVITY_MODE_MASK  0xC0


/** Values for max sustained read/write speeds returned by GETMXDS */
#define SNS_CCC_MXDS_SPEED_MAX  0 /* f_SCL max -- default value */
#define SNS_CCC_MXDS_SPEED_8MHZ 1 /* 8MHz */
#define SNS_CCC_MXDS_SPEED_6MHZ 2 /* 6MHz */
#define SNS_CCC_MXDS_SPEED_4MHZ 3 /* 4MHz */
#define SNS_CCC_MXDS_SPEED_2MHZ 4 /* 2MHz */
#define SNS_CCC_MXDS_SPEED_MASK 0x07

/** Values for max Clock to Data turnaround time returned by GETMXDS */
#define SNS_CCC_MXDS_TURNAROUND_LT_8NS  (0<<3) /* T_SCO less than 8ns */
#define SNS_CCC_MXDS_TURNAROUND_LT_9NS  (1<<3) /* T_SCO less than 9ns */
#define SNS_CCC_MXDS_TURNAROUND_LT_10NS (2<<3) /* T_SCO less than 10ns */
#define SNS_CCC_MXDS_TURNAROUND_LT_11NS (3<<3) /* T_SCO less than 11ns */
#define SNS_CCC_MXDS_TURNAROUND_LT_12NS (4<<3) /* T_SCO less than 12ns */
#define SNS_CCC_MXDS_TURNAROUND_MASK    (0x7<<3)

/** Defining bytes for SETXTIME command */
#define SNS_CCC_SETXTIME_ST            0x7F /* Sync Tick -- 0 additional bytes */
#define SNS_CCC_SETXTIME_DT            0xBF /* Delay Tick -- 1 additional byte */
#define SNS_CCC_SETXTIME_ENTER_ASYNC_0 0xDF /* Enter async mode. See spec */
#define SNS_CCC_SETXTIME_ENTER_ASYNC_1 0xEF /* No additional bytes */
#define SNS_CCC_SETXTIME_ENTER_ASYNC_2 0xF7
#define SNS_CCC_SETXTIME_ENTER_ASYNC_3 0xFB
#define SNS_CCC_SETXTIME_ASYNC_TRIGGER 0xFD
#define SNS_CCC_SETXTIME_TPH           0x3F /* Repitition period -- 1 or more bytes */
#define SNS_CCC_SETXTIME_TU            0x9F /* Time unit -- one byte */
#define SNS_CCC_SETXTIME_ODR           0x8F /* ODR -- one byte */

/** Supported modes reported in GETXTIME */
#define SNS_CCC_GETXTIME_SUPPORT_SYNC     0x01
#define SNS_CCC_GETXTIME_SUPPORT_ASYNC_0  0x02
#define SNS_CCC_GETXTIME_SUPPORT_ASYNC_1  0x04
#define SNS_CCC_GETXTIME_SUPPORT_ASYNC_2  0x08
#define SNS_CCC_GETXTIME_SUPPORT_ASYNC_3  0x10

/** State reported in GETXTIME */
#define SNS_CCC_GETXTIME_STATE_SYNC     0x01
#define SNS_CCC_GETXTIME_STATE_ASYNC_0  0x02
#define SNS_CCC_GETXTIME_STATE_ASYNC_1  0x04
#define SNS_CCC_GETXTIME_STATE_ASYNC_2  0x08
#define SNS_CCC_GETXTIME_STATE_ASYNC_3  0x10
#define SNS_CCC_GETXTIME_STATE_OVERFLOW 0x80




typedef enum sns_sync_com_port_ccc
{
  SNS_SYNC_COM_PORT_CCC_ENEC,
  /**< Enable slave event driven interrupts. Write. One byte.
   * Set to 1 to enable interrupts. See spec for other values */

  SNS_SYNC_COM_PORT_CCC_DISEC,
  /**< Disable slave event driven interrupts. Write. One byte.
   * Set to 1 to disable interrupts. See spec for other values */

  SNS_SYNC_COM_PORT_CCC_SETDASA,
  /**< Assigns a dynamic address to a slave with a known static address. Write. One byte.
   * The dynamic address (left shifted one bit to allow for r/w bit).
   * NOTE: this must be sent on a port opened with the slave's I2C static address.
   *       The port must be closed & re-opened if the address changes */

  SNS_SYNC_COM_PORT_CCC_RSTDAA,
  /**< Resets dynamic address assignment. Zero bytes.
   * NOTE: This must be sent on a port opened with the slave's current I3C dynamic
   *       address.
   *       The port must be closed & re-opened to communicate with the slave if the
   *       I2C static address is not the same as the previously assigned I3C address */

  SNS_SYNC_COM_PORT_CCC_SETMWL,
  /**< Set maximum write length. Write. Two bytes.
   * Max length in bytes (MSB first) */

  SNS_SYNC_COM_PORT_CCC_SETMRL,
  /**< Set maximum read length in a single command. Write. Two or Three bytes.
   * Max read length: 2 bytes (MSB first). Max IBI read length (if IBI data supported) */

  SNS_SYNC_COM_PORT_CCC_GETMWL,
  /**< Get maximum write length. Read. Two bytes.
   * Max length in bytes (MSB first) */

  SNS_SYNC_COM_PORT_CCC_GETMRL,
  /**< Get maximum read length. Read. Two or Three bytes
   * Max read length: 2 bytes (MSB first). Max IBI read length (if IBI data supported) */

  SNS_SYNC_COM_PORT_CCC_GETPID,
  /**< Get a slave's provisional id (PID). Read. Six bytes */

  SNS_SYNC_COM_PORT_CCC_GETBCR,
  /**< Get a device's bus characteristic (BCR). Read. One byte */

  SNS_SYNC_COM_PORT_CCC_GETDCR,
  /**< Get a device's device characteristics (DCR). Read. One byte */

  SNS_SYNC_COM_PORT_CCC_GETSTATUS,
  /**< Get a device's operating status. Read. Two bytes 
   * MSB first: vendor reserved byte
   * LSB second: see SNS_CCC_STATUS_* masks */

  SNS_SYNC_COM_PORT_CCC_GETMXDS,
  /**< Get sdr mode max read and write data speeds (& optionally max read turnaround time).
   * Read. Two or Five bytes
   * byte[0]: Max write clock
   * byte[1]: Max Read clock | clock to data turnaround time:
   * optional bytes[2..4]: Max read turnaround time in uSec, LSB first.
   * */

  SNS_SYNC_COM_PORT_CCC_SETXTIME,
  /**< Framework for exchanging event timing information. Write. Various number of bytes
   * byte[0]: SETXTIME defining byte
   * optional bytes: See SNS_CCC_SETXTIME_* for number of optional bytes */

  SNS_SYNC_COM_PORT_CCC_GETXTIME,
  /**< Framework for exchanging event timing information. Read. Four bytes.
   * byte[0]: Supported modes. See SNS_CCC_GETXTIME_SUPPORT_*
   * byte[1]: State. See SNS_CCC_GETXTIME_STATE_
   * byte[2]: Internal oscilator frequency, in increments of 0.5 MHz (0-->~32kHz)
   * byte[3]: Inaccuracy byte. Max variation of slave's oscillator, in 0.1% increments */
} sns_sync_com_port_ccc;
