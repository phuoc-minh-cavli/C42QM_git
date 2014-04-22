#ifndef WCD_INTF_
#define WCD_INTF_

/**
@file wcd_intf.h
@brief External WCD interface.
*/
/*-------------------------------------------------------------------------------
    Copyright (c) 2013 Qualcomm Technologies Incorporated.
    All rights reserved.
    Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                        Edit History
$Id: //components/rel/audio.tx/2.0/audio/api/adie/wcd_intf.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/01/14   AG      Header update
03/07/13   SSP     Driver Unification Update
12/15/10   SK      Initial revision
===============================================================================*/

#include "wcd_result.h"
#include "wcd_types.h"

/**
 * \brief The codec family supported on this WCD driver.
 */
#define WCD93XX_B

/**
 * \brief The Module ID for the IIR Configuration to be used with ACDB.
 */
#define WCD_IIR_CONFIG_MID    0x12A07

/**
 * \brief The Parameter ID for the IIR Configuration to be used with ACDB.
 */
#define WCD_IIR_CONFIG_PID    0x12A08

/*****************************************************************************
 * wcd_init options
*****************************************************************************/

/** 
 * \brief This function initializes the driver.
 *
 */
wcd_result wcd_init( void* options     /**< options for initializing the driver - see above */
                    );

                                      
/** 
 * \brief This function opens the driver for excersizing a use case. This function must be
 * called prior to calling any IOCTLs using a handle.
 *
 * @note wcd_init must be called prior to calling wcd_open
 *
 */
wcd_result wcd_open ( void **p_handle             /**< returns a handle to the use case */
                    );


/**
 * \brief This is the workhorse function for the codec driver. It 
 * follows the linux IOCTL style.
 *
 */
wcd_result wcd_ioctl  ( void *p_handle,             /**< Pointer to the handle returned in wcd_open */
                        uint32 ioctl_cmd,           /**< IOCTL command ID */
                        void *p_command             /**< IOCTL commmand structure */
                      );

/**
 * \brief This function closes a particular usecase by removing it from the global list
 * The handle is deallocated as well. The usecase must be disabled prior to attempting a close.
 *
 */
wcd_result wcd_close  ( void *p_handle          /**< Pointer to the handle returned in wcd_open */
                      );
                      
/** 
 * \brief This function deinitializes the driver.
 *
 */
wcd_result wcd_deinit ( void );

/*****************************************************************************
 * IOCTL Commands
*****************************************************************************/

/**
 * \brief This will create a connection between a codec source and codec sink. 
 * The codec elements between the source and sink will be reserved to ensure another 
 * connection will not be disturbed. It will also create a secondary connection for 
 * differential RX connections.
 */
#define WCD_IOCTL_CONNECT_SOURCE_TO_SINK          1

/**
 * \brief This will create multiple connections using the CONNECT_SOURCE_TO_SINK IOCTL
 */
#define WCD_IOCTL_CONNECT_SOURCES_TO_SINKS        2

/**
 * \brief Disconnects a particular connection from the usecase. The usecase needs to be 
 * disabled prior to disconnecting connections.
 */
#define WCD_IOCTL_DISCONNECT_CONNECTION           3

/**
 * \brief This will disconnect all the connections in the usecase. The usecase will have 
 * to be disabled prior to disconnecting the connections. There are no parameters 
 * associated with this IOCTL.
 */
#define WCD_IOCTL_DISCONNECT_CONNECTIONS          4

/**
 * \brief This will copy the ADIE profile to the usecase. The ACDB profile is used during 
 * enable and disable. It contains the events required to enable and disable the 
 * particular device.
 */
#define WCD_IOCTL_SET_PROFILE                     5

/**
 * \brief This will enable the particular usecase/device using the ADIE profile supplied 
 * during SET_PROFILE.
 */
#define WCD_IOCTL_ENABLE_USECASE                  6

/**
 * \brief This will disable the particular usecase/device using the remaining commands
 * in the ADIE profiel supplied during SET_PROFILE. There are no parameters associated with
 * this IOCTL.
 */
#define WCD_IOCTL_DISABLE_USECASE                 7

/**
 * \brief This will return the logical address of the PGD and INTF SLIMbus devices. Only applicable
 * for SLIMbus.
 */
#define WCD_IOCTL_GET_SLIMBUS_LOGICAL_ADDRESS     8

/**
 * \brief Writes a register to the codec register bank.
 */
#define WCD_IOCTL_REGISTER_WRITE                  9

/**
 * \brief Reads a register from the codec register bank.
 */
#define WCD_IOCTL_REGISTER_READ                   10

/**
 * \brief Dumps the writable registers to a log.
 */
#define WCD_IOCTL_REGISTER_DUMP_TO_LOG            11

/**
 * \brief Enables the slimbus interface and shared channel on the codec
 */
#define WCD_IOCTL_ENABLE_SLIMBUS_INTF             12

/**
 * \brief Enables ANC on the existing RX usecase. 
 */
#define WCD_IOCTL_ENABLE_ANC                      13

/**
 * \brief Disables ANC on the RX usecase. There are no parameters associated with
 * this IOCTL.
 */
#define WCD_IOCTL_DISABLE_ANC                     14

/**
 * \brief Returns information regarding the codec.
 */
#define WCD_IOCTL_GET_CODEC_INFO                  15

/**
 * \brief Enables Sidetone on the existing RX usecase using a TX usecase as a reference.
 */
#define WCD_IOCTL_ENABLE_SIDETONE                 16

/**
 * \brief Disables Sidetone on the existing RX usecase
 */

#define WCD_IOCTL_DISABLE_SIDETONE                17

/**
 * \brief Enables internal codec feedback speaker protection on the RX usecase.
 */
#define WCD_IOCTL_ENABLE_FB_SPKR_PROT             18

/**
 * \brief Disables internal codec feedback speaker protection on the RX usecase.
 */
#define WCD_IOCTL_DISABLE_FB_SPKR_PROT            19

/**
 * \brief Configures MAD and MICBIAS, MAD modules are meant to be enabled by LPASS
 */
#define WCD_IOCLT_CONFIGURE_MAD                   20

/**
 * \brief Configures MAD and MICBIAS, MAD modules are meant to be enabled by LPASS
 */
#define WCD_IOCLT_DISABLE_MAD                     21

/**
 * \brief Gets the sidetone IIR configuration for the existing RX usecase
 */
#define WCD_IOCTL_GET_SIDETONE_IIR_CONFIG         22

/**
 * \brief Sets the sidetone IIR configuration on the existing RX usecase
 */
#define WCD_IOCTL_SET_SIDETONE_IIR_CONFIG         23


/**
 * \brief This MBHC IOCTL starts MBHC algorithm.
 *
 * @note command strucutre associated with this IOCTL command 
 * "wcd_client_state" in "wcd_types.h" 
 * "MBHC_ACDB_Param_Main" defined in "wcd_mbhc_api.h"
 */
#define WCD_IOCTL_CMD_MBHC_SET_START                   101

/**
 * \brief This MBHC IOCTL stops MBHC algorithm.
 *
 * @note command strucutre associated with this IOCTL command 
 * "wcd_client_state" in "wcd_types.h" 
 */
#define WCD_IOCTL_CMD_MBHC_SET_STOP                    102

/**
 * \brief This WCD IOCTL is used by its client to invoke 
 a WCD callback to notify WCD of either MSM or PMIC GPIO
 interrupts. PMIC GPIO interrupt includes plug detection 
 interrupt for MBHC and MSM GPIO interrupt includes 
 button-press-release interrupt for IH module 
 * 
 * @note No command strucutre associated with this IOCTL command
 */
#define WCD_IOCTL_CMD_INVOKE_CB                        103

/**
 * \brief This WCD IOCTL is used by its client to
 provide its callback function to WCD. WCD driver stores this 
 CB function pointer and uses it whenever it needs to read from 
 or write to a PMIC/MSM GPIO 
 *
 * @note No command strucutre associated with this IOCTL command
 */
#define WCD_IOCTL_CMD_SET_GPIO_INTF_CB                 104

/**
 * \brief This WCD IOCTL is used by its client to
 provide its callback function to WCD. WCD stores this
 CB function pointer and uses it to send events back to its 
 client 
 *
 * @note No command strucutre associated with this IOCTL command
 */
#define WCD_IOCTL_CMD_SET_EVENT_CB                     105

/**
 * \brief This MBHC IOCTL is used by its client to start the FTM
 *        mode
 *
 * @note No command strucutre associated with this IOCTL command
 */
#define WCD_IOCTL_CMD_MBHC_SET_FTM_START               106

/**
 * \brief This MBHC IOCTL is used by its client to send FTM
 *        commands
 *
 * @note No command strucutre associated with this IOCTL command
 */
#define WCD_IOCTL_CMD_MBHC_SET_FTM_CMD                 107

/**
 * \brief This MBHC IOCTL is used by its client to send preset 
 *        trigger commands to MBHC
 *
 * @note No command strucutre associated with this IOCTL command
 */
#define WCD_IOCTL_CMD_MBHC_SET_TRIGGER_CMD             108

/**
* \brief This WCD IOCTL is used by its client such as CPE SVC to
         send a request to ACRM to enable-disable required clocks
*
* @note command strucutre associated with this IOCTL command 
*       "rm_manage_clock_request_payload" in "audio_codec_resource_manager.h" 
*/
#define WCD_IOCTL_CMD_ACRM_MANAGE_CLOCK_REQUEST        109

#define WCD_IOCTL_CMD_MBHC_MAX                         120


#endif


