#ifndef __SPI_DEFS_H__
#define __SPI_DEFS_H__
/**
   @file spi_defs.h
   @brief
   This file contains definitions common betwee DAL1 and DAL0.
*/

/*=========================================================================

FILE: spi_defs.h

DESCRIPTION:
   This file contains DAL interface declarations for the SPI device driver.

===========================================================================

        Edit History
		

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
09/20/16 dpk    Ported for ThreadX
06/08/16 ska    Ported for TN
06/06/12 dk     Added minor review comments.
03/27/12 ag     Added batch transfer updates.
06/05/11 ag     Doxygen Markups.

===========================================================================
        Copyright (c) 2010-2016 Qualcomm Technologies Incorporated
                            All Rights Reserved
                         Qualcomm Proprietary/GTDR
===========================================================================*/

#include "SpiDeviceTypes.h"

/**
@defgroup spi_api SPI APIs
This group groups all the APIs in the Shapes API module.

@defgroup spi_dal_api SPI DAL APIs for clients
@ingroup spi_api
This section defines the various data structures used in the SPI DAL API.

@defgroup spi_dal_data_structures Data Structures.
@ingroup spi_api
This section defines the various data structures used in the SPI DAL API.

@defgroup spi_dal_enum Enumeration Constants
@ingroup spi_api
This section defines the various enums used in the SPI DAL API.
*/

/**
@addtogroup spi_dal_enum
@{ */
/**
   SPI Clock is always on or turned off after every transaction, this enum is deprecated.
   The driver ignores the value passed.
*/
typedef enum
{
   SPI_CLK_NORMAL,     /**< Turns off SPI CLK during idle state. */
   SPI_CLK_ALWAYS_ON,  /**< Runs SPI CLK during idle state. */
   SPI_CLK_PAD = 0x7FFFFFFF,
} SpiClockModeType;
 /**
@}
*/

/**
@addtogroup spi_dal_enum
@{ */
/**
   In the idle state whether the SPI clk is high or low.
*/
typedef enum
{
   SPI_CLK_IDLE_LOW,   /**< CLK signal is low when idle. */
   SPI_CLK_IDLE_HIGH,  /**< CLK signal is high when idle. */
   SPI_CLK_IDLE_PAD = 0x7FFFFFFF,
} SpiClockPolarityType;
 /**
@}
*/

/**
@addtogroup spi_dal_enum
@{ */
/**
   Shift mode, detemines which signal (input or output) is sampled first.
*/
typedef enum
{
   SPI_INPUT_FIRST_MODE,   /**< In both Master and slave input Bit is shifted in first. */
   SPI_OUTPUT_FIRST_MODE,  /**< In both Master and slave output Bit is shifted in first. */
   SPI_SHIFT_PAD = 0x7FFFFFFF,
} SpiShiftModeType;
 /**
@}
*/

/**
@addtogroup spi_dal_enum
@{ */
/**
   In idle state whether the Chip Select is high or low.
*/
typedef enum
{
   SPI_CS_ACTIVE_LOW,   /**< During idle state CS line is held low. */
   SPI_CS_ACTIVE_HIGH,  /**< During idle state CS line is held high. */
   SPI_CS_PAD = 0x7FFFFFFF,
} SpiCSPolarityType;
 /**
@}
*/

/**
@addtogroup spi_dal_enum
@{ */
/**
   List of values that inidicates, whether the Chip select is always active or if it's brought
   back to its idle state after N clock cycles.
*/
typedef enum
{
   SPI_CS_DEASSERT,       /**< CS is deasserted after transferring data for N clock cycles. */
   SPI_CS_KEEP_ASSERTED,  /**< CS is asserted as long as the core is in Run state. */
   SPI_CS_MODE_PAD = 0x7FFFFFFF,
} SpiCSModeType;
 /**
@}
*/

/**
@addtogroup spi_dal_data_structures
@{ */
/**
   Data structure that contains information regarding the SPI device settings.
*/
typedef struct
{
   SpiClockModeType eClockMode;          /**< Clock mode type to be used for the SPI core.
                                              This parameter determines if the Clock could
                                              be running even if there is no ongoing transfer. */
   SpiClockPolarityType eClockPolarity;  /**< Clock polarity type to be used for the SPI core. */
   SpiShiftModeType eShiftMode;          /**< Shift mode type to be used for SPI core.  This
                                              parameter specifies whether the SPI core operates
                                              in OUTPUT or INPUT FIRST Mode. This specifies whether
                                              the shift register latches the DATA at the input pin
                                              on the rising or falling edge. */
   uint32 u32DeassertionTime;            /**< The minimum time delay in nano second, between two
                                              word(N-bits) transfers.  This sets the minimum time
                                              in nanoseconds that the chip select is deasserted
                                              between SPI transfers (in master mode).  The
                                              deassertion clock ticks will be configured so that
                                              the deassertion time will be as close as possible to,
                                              but not less than, the time specified by this
                                              parameter. */
   uint32 u32MinSlaveFrequencyHz;        /**< The Minimum frequency above which the spi core
                                              should be run.  Normally this is set to zero. */
   uint32 u32MaxSlaveFrequencyHz;        /**< This is desired frequency of operation.  If the core
                                              cannot run at this frequency, a value below this but
                                              greater than u32MinSlaveFrequencyHz will be chosen
                                              depending on the frequencies available in the
                                              clkregim SPI clock steps configuration. */
   SpiCSPolarityType eCSPolarity;        /**< CS polarity type to be used for the SPI core.  This
                                              field tells whethere the Chip select is active high
                                              or active low. */
   SpiCSModeType eCSMode;                /**< CS Mode to be used for the SPI core.  This specifies
                                              whether the Chip select is asserted through the
                                              entire Transaction or whether it is deasserted in
                                              between SPI transfers of each transaction. */
   boolean bHSMode;                      /**< Flag to specify whether to enable high speed mode. */
} SpiDeviceParametersType;
 /**
@}
*/

/**
@addtogroup spi_dal_enum
@{ */
/**
   List of MODES in which the SPI core can be used.
*/
typedef enum
{
   SPI_CORE_MODE_SLAVE,   /**< SPI core is set in slave mode. */
   SPI_CORE_MODE_MASTER,  /**< SPI core is set in master mode. */
   SPI_CORE_PAD = 0x7FFFFFFF,
} SpiCoreModeType;
 /**
@}
*/

/**
@addtogroup spi_dal_data_structures
@{ */
/**
   Data structure that holds that board information in which the SPI device is present.
*/
typedef struct
{
   uint32 nSlaveNumber;        /**< This specifies the slave number, which identifies what
                                    CS the slave uses. */
   SpiCoreModeType eCoreMode;  /**< This specifies wether the core acts as master or a slave. */
} SpiDeviceBoardInfoType;
 /**
@}
*/

/**
@addtogroup spi_dal_enum
@{ */
/**
   List of different SPI transfer Modes.
*/
typedef enum
{
   SPI_TRANSFER_MODE_DEFAULT,     /**< Let the SPI DAL driver choose the transfer mode.
                                       If the transfer length is greater than one block size,
                                       DATA MOVER mode is used, otherwise BLOCK mode is used. */
   SPI_TRANSFER_MODE_FIFO,        /**< FIFO mode is not supported. */
   SPI_TRANSFER_MODE_BLOCK,       /**< BLOCK mode is used for the data transfer. */
   SPI_TRANSFER_MODE_DATA_MOVER,  /**< DATA MOVER mode is used for the data transfer. */
   SPI_TRANSFER_PAD = 0x7FFFFFFF,
} SpiTransferModeType;
 /**
@}
*/

/**
@addtogroup spi_dal_enum
@{ */
/**
   List of settings for LOOBBACK MODE.
*/
typedef enum
{
   SPI_LOOPBACK_DISABLED,  /**< The loopback mode is disabled. */
   SPI_LOOPBACK_ENABLED,   /**< The loopback mode is enabled. */
   SPI_LOOPBACK_PAD = 0x7FFFFFFF,
} SpiLoopbackModeType;
 /**
@}
*/

/**
@addtogroup spi_dal_enum
@{ */
/**
   List of different SPI input packing types.
*/
typedef enum
{
   SPI_INPUT_PACKING_DISABLED,  /**< Packing is disabled in the INPUT FIFO. */
   SPI_INPUT_PACKING_ENABLED,   /**< Packing is enabled in the INPUT FIFO. */
   SPI_INPUT_PACKING_PAD = 0x7FFFFFFF,
} SpiInputPackingType;
 /**
@}
*/

/**
@addtogroup spi_dal_enum
@{ */
/**
   List of different SPI output unpacking types.
*/
typedef enum
{
   SPI_OUTPUT_UNPACKING_DISABLED,  /**< Unpacking is disabled in the OUTPUT FIFO. */
   SPI_OUTPUT_UNPACKING_ENABLED,   /**< Unpacking is enabled in the OUTPUT FIFO. */
   SPI_OUTPUT_PACKING_PAD = 0x7FFFFFFF,
} SpiOutputUnpackingType;
 /**
@}
*/

/**
@addtogroup spi_dal_data_structures
@{ */
/**
   Data structure that holds different parameters that can be used for SPI transfer.
*/
typedef struct
{
   uint32 nNumBits;                          /**< Specifies the number of bits (N) in one logical
                                                  SPI transfer. */
   SpiTransferModeType eTransferMode;        /**< Specify which transfer mode to use. */
   SpiLoopbackModeType eLoopbackMode;        /**< Specifies whether loopback mode is enabled. */
   SpiInputPackingType eInputPacking;        /**< Specifies whether input packing is enabled. */
   SpiOutputUnpackingType eOutputUnpacking;  /**< Specifies whether output unpacking is enabled. */
   uint32 slaveTimeoutUs;                    /**< Slave timeout in micro-seconds. Ignored by the
                                                  master. */
} SpiTransferParametersType;
 /**
@}
*/

/**
@addtogroup spi_dal_data_structures
@{ */
/**
   Data structure that holds information regarding the SPI device.
*/
typedef struct
{
   SpiDeviceParametersType deviceParameters;      /**< Specifies the device parameters to be used
                                                       for the SPI core. */
   SpiDeviceBoardInfoType deviceBoardInfo;        /**< Specifies the board information. */
   SpiTransferParametersType transferParameters;  /**< Specifies the transfer parameters to be used
                                                       for a SPI transfer. */
} SpiDeviceInfoType;
 /**
@}
*/

/**
@addtogroup spi_dal_data_structures
@{ */
/**
   Data structure to hold the physical and virtual address of the data buffers,
   which are used for SPI transfers.
*/
typedef struct
{
  void *virtualAddr;   /**< The virtual address of the data buffer. */
  void *physicalAddr;  /**< The physical address of the data buffer. */
} SpiDataAddrType;
 /**
@}
*/

/**
@addtogroup spi_dal_data_structures
@{ */
/**
   Data structure to hold the HW information obtained from DALPD.
*/
typedef struct
{
   uint32 nNumInstances;           /**< Number of SPI cores in the system. */
   uint32 uInputFifoSize;          /**< Input FIFO size of HW in bytes. */
   uint32 uOutputFifoSize;         /**< Output FIFO size of HW in bytes. */
   uint32 uInputBlockSize;         /**< Input block size in bytes. */
   uint32 uOutputBlockSize;        /**< Output block size in bytes. */
   uint32 uMaxNumInputTransfers;   /**< Max number of input transfers per transaction. */
   uint32 uMaxNumOutputTransfers;  /**< Max number of output transfers per transaction. */
} SpiInfoType;
/**
@}
*/

#endif  /* __SPI_DEFS_H__ */
