#ifndef WCD_INTF_TYPES_
#define WCD_INTF_TYPES_

/**
@file wcd_types.h
@brief WCD Types.
*/
/*-------------------------------------------------------------------------------
    Copyright (c) 2013 Qualcomm Technologies Incorporated.
    All rights reserved.
    Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                        Edit History
$Id: //components/rel/audio.tx/2.0/audio/api/adie/wcd_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/25/14   AG      Conga support
05/28/14   AG      Support for PMIC based interrupts
04/01/14   AG      Header update
01/27/14   KRP     Added new owner and payload in event call back for MBHC power handling
12/13/13   AG      Added new speaker PA element type
07/23/13   KNM     Added buck voltage to represent V5V6 mode. Basically mode to 
                   support dynamic buck voltage switching between 1.8V to 2.15V
                   or vice versa.
03/07/13   SSP     Driver Unification Update
03/01/13   KNM     Refactored bus manager i/f and added buck voltage selection
                   in wcd init.
12/15/10   SK      Initial revision
===============================================================================*/

#include "wcd_result.h"
#include "wcd_os_specific.h"

/*****************************************************************************
 * WCD 93xx common types.
*****************************************************************************/
typedef uint8 wcd_bool;
typedef uint8 BOOLEAN;
#define TRUE                1
#define FALSE               0
#define WCD_TRUE            1
#define WCD_FALSE           0
typedef void*               WCD_HANDLE;

#ifndef TEXT
#define TEXT( string ) string
#endif

typedef int32 (* wcd_osal_thread )( void* parameter );
typedef int32 (* wcd_osal_kill_thread_fcn )( void* parameter );
typedef void (* wcd_osal_timer_callback_fcn )( void* parameter );

typedef enum
{
  E_priority_low,
  E_priority_normal,
  E_priority_high,
  E_priority_veryhigh
}wcd_osal_priority_t;

#define WCD_MAX_ADIE_CHANNELS        8
#define WCD_SPKR_PROT_CHANNELS       2

/**
 * \brief The maximum number of WCD HALs Element Types.
 */
#define WCD_MAX_ELEMENT_TYPES 18

/**
 * \brief Sample rate masks.
 */
#define WCD_SAMPLE_RATE_8_KHZ     0x1
#define WCD_SAMPLE_RATE_16_KHZ    0x2
#define WCD_SAMPLE_RATE_32_KHZ    0x4
#define WCD_SAMPLE_RATE_48_KHZ    0x8
#define WCD_SAMPLE_RATE_96_KHZ    0x10
#define WCD_SAMPLE_RATE_192_KHZ   0x20

/**
 * \brief Bit width masks.
 */
#define WCD_BIT_WIDTH_12_BIT      0x1
#define WCD_BIT_WIDTH_16_BIT      0x2
#define WCD_BIT_WIDTH_20_BIT      0x4
#define WCD_BIT_WIDTH_24_BIT      0x8


/*****************************************************************************
 * ADIE profile ACDB Header
 ****************************************************************************/

/**
 * \brief Enumeration that represent the direction of the ADIE profile.
 */
typedef enum 
{
  ADIE_CODEC_RX = 0,  /**< ADIE_CODEC_RX :- Path Type is Rx*/
  ADIE_CODEC_TX,      /**< ADIE_CODEC_TX :- Path Type is Tx*/
  ADIE_CODEC_LB,      /**< ADIE_CODEC_LB :- Path Type is Loopback*/
  ADIE_CODEC_DIRECTION_32B_ANCHOR = 0x7FFFFFFF
} adie_codec_direction;

/**
 * \brief The different sources available on the WCD driver.
 */
typedef enum
{
  E_WCD_SOURCE_AUDIO_INPUT_1 = 0x10001,
  E_WCD_SOURCE_AUDIO_INPUT_2 = 0x10002,
  E_WCD_SOURCE_AUDIO_INPUT_3 = 0x10003,
  E_WCD_SOURCE_AUDIO_INPUT_4 = 0x10004,
  E_WCD_SOURCE_AUDIO_INPUT_5 = 0x10005,
  E_WCD_SOURCE_AUDIO_INPUT_6 = 0x10006,
  E_WCD_SOURCE_DMIC_INPUT_1 = 0x20001,
  E_WCD_SOURCE_DMIC_INPUT_2 = 0x20002,
  E_WCD_SOURCE_DMIC_INPUT_3 = 0x20003,
  E_WCD_SOURCE_DMIC_INPUT_4 = 0x20004,
  E_WCD_SOURCE_DMIC_INPUT_5 = 0x20005,
  E_WCD_SOURCE_DMIC_INPUT_6 = 0x20006,
  E_WCD_SOURCE_SLIMBUS_RX_1 = 0x40001,
  E_WCD_SOURCE_SLIMBUS_RX_2 = 0x40002,
  E_WCD_SOURCE_SLIMBUS_RX_3 = 0x40003,
  E_WCD_SOURCE_SLIMBUS_RX_4 = 0x40004,
  E_WCD_SOURCE_SLIMBUS_RX_5 = 0x40005,
  E_WCD_SOURCE_SLIMBUS_RX_6 = 0x40006,
  E_WCD_SOURCE_SLIMBUS_RX_7 = 0x40007,
  E_WCD_SOURCE_SLIMBUS_RX_8 = 0x40008,
  E_WCD_SOURCE_SLIMBUS_RX = 0x40000,
  E_WCD_SOURCE_I2S_RX_LEFT_PRIMARY    = 0x80001,
  E_WCD_SOURCE_I2S_RX_RIGHT_PRIMARY   = 0x80002,
  E_WCD_SOURCE_I2S_RX_LEFT_SECONDARY  = 0x80003,
  E_WCD_SOURCE_I2S_RX_RIGHT_SECONDARY = 0x80004,
  E_WCD_SOURCE_I2S_RX = 0x80000,
  E_WCD_SOURCE_REUSE_LAST_BUS_PORT = 0x100000,
  E_WCD_SOURCE_ANC_CHANNEL_1 = 0x200001,
  E_WCD_SOURCE_ANC_CHANNEL_2 = 0x200002,
  E_WCD_SOURCE_32B_ANCHOR = 0x7FFFFFFF
} wcd_source;

/**
 * \brief The different sinks available on the WCD driver.
 */
typedef enum
{
  E_WCD_SINK_EAROUT = 0x10001,
  E_WCD_SINK_ULTRASOUND_EAROUT = 0x10011,
  E_WCD_SINK_SPEAKER_OUT       = 0x10002,
  E_WCD_SINK_HEADPHONE_LEFT    = 0x10003,
  E_WCD_SINK_HEADPHONE_RIGHT   = 0x10004,
  E_WCD_SINK_SPEAKER_2_OUT     = 0x10005,
  E_WCD_SINK_LINE_1 = 0x20001,
  E_WCD_SINK_LINE_2 = 0x20002,
  E_WCD_SINK_LINE_3 = 0x20003,
  E_WCD_SINK_LINE_4 = 0x20004,
  E_WCD_SINK_SLIMBUS_TX_1 = 0x40001,
  E_WCD_SINK_SLIMBUS_TX_2 = 0x40002,
  E_WCD_SINK_SLIMBUS_TX_3 = 0x40003,
  E_WCD_SINK_SLIMBUS_TX_4 = 0x40004,
  E_WCD_SINK_SLIMBUS_TX_5 = 0x40005,
  E_WCD_SINK_SLIMBUS_TX_6 = 0x40006,
  E_WCD_SINK_SLIMBUS_TX_7 = 0x40007,
  E_WCD_SINK_SLIMBUS_TX_8 = 0x40008,
  E_WCD_SINK_SLIMBUS_TX_9 = 0x40009,
  E_WCD_SINK_SLIMBUS_TX_10 = 0x4000A,
  E_WCD_SINK_SLIMBUS_TX_11 = 0x4000B,
  E_WCD_SINK_SLIMBUS_TX_12 = 0x4000C,
  E_WCD_SINK_SLIMBUS_TX_13 = 0x4000D,
  E_WCD_SINK_SLIMBUS_TX = 0x40000,
  E_WCD_SINK_I2S_TX_LEFT_PRIMARY    = 0x80001,
  E_WCD_SINK_I2S_TX_RIGHT_PRIMARY   = 0x80002,
  E_WCD_SINK_I2S_TX_LEFT_SECONDARY  = 0x80003,
  E_WCD_SINK_I2S_TX_RIGHT_SECONDARY = 0x80004,
  E_WCD_SINK_I2S_TX = 0x80000,
  E_WCD_SINK_32B_ANCHOR = 0x7FFFFFFF
} wcd_sink;

/**
 * \brief The different connection types available on the WCD driver.
 */
typedef enum
{
  E_WCD_CONNECTION_DEFAULT,
  E_WCD_CONNECTION_DIFFERENTIAL,
  E_WCD_CONNECTION_32B_ANCHOR = 0x7FFFFFFF
} wcd_connection_type;


/**
 * ADIE Header for ADIE profiles
 */
typedef struct 
{
  uint32                            size;
  uint32                            version;
  uint32                            profile_id;
  uint32                            number_of_paths;
  adie_codec_direction              profile_direction;
  uint32                            num_actions; 
  wcd_source                        sources[WCD_MAX_ADIE_CHANNELS]; 
  wcd_sink                          sinks[WCD_MAX_ADIE_CHANNELS];
  wcd_connection_type               connection_type[WCD_MAX_ADIE_CHANNELS]; 
} adie_codec_acdb_header;


/*****************************************************************************
 * WCD 93XX INTF types.
*****************************************************************************/


/**
 * \brief The different bus types supported by the WCD driver.
 */
typedef enum
{
  E_WCD_SLIMBUS,
  E_WCD_I2C,
  E_WCD_AHB_IF,
  E_WCD_SPMI
} wcd_bus_type;

/**
 * \brief Different MCLK speed supported by the WCD driver.
 */
typedef enum
{
  E_WCD_9_6_MHZ,
  E_WCD_12_288_MHZ
} wcd_mclk_speed;

/**
 * \brief Different buck voltages supported by the WCD driver.
 */
typedef enum
{
  E_WCD_VDD_BUCK_1P8V,
  E_WCD_VDD_BUCK_2P15V,
  E_WCD_VDD_BUCK_1P8V_2P15V // Mode to support dynamic buck voltage switching
} wcd_vdd_buck_voltage;

/**
  \brief wcd_init_param structures are used to pass initialization requirements to WCD driver.
  
  The idea of the init param structure is to be able to pass different initialization options depending
  on the project requirements, the major and minor version fields must be populated accordingly to 
  adapt to the required configurations:
    - Version 0.0 will configure only the bustype, interrupt handler will not be configured since 
      IHGpio is not provided on wcd_init_param v0.0. this is specially useful on projects where the 
      WCD codec is used only as reference.
    - Version 0.1 will configure the bus type and will enable interrupt handler, registering it to
      the specified GPIO.
*/
  
/* version 0.0  
  Default configuration:
    -Interrupt handler is disabled
    -Mclk is set to 9.6MHz
*/
typedef struct
{
  uint16          MajorVersion;  // (MUST be 0)
  uint16          MinorVersion;  // (MUST be 0)
  wcd_bus_type    bustype; // E_WCD_SLIMBUS or E_WCD_I2C    
} wcd_init_param_v0_0;

/* version 0.1  
  Default configuration:
    -Interrupt handler is disabled
*/
typedef struct
{
  uint16          MajorVersion;  // (MUST be 0)
  uint16          MinorVersion;  // (MUST be 0)
  wcd_bus_type    bustype; // E_WCD_SLIMBUS or E_WCD_I2C
  wcd_mclk_speed  mclk_speed; //E_WCD_9_6_MHZ or E_WCD_12_288MHZ
} wcd_init_param_v0_1;

  
/* version 1.0
  Default configuration
  -Interrupt handler is enabled and the IRQ is registered for the specified GPIO
  -Mclk is set to 9.6MHz
*/
typedef struct
{
  uint16          MajorVersion;  //(MUST be 1)
  uint16          MinorVersion;  //(MUST be 0)
  wcd_bus_type    bustype; // E_WCD_SLIMBUS or E_WCD_I2C    
  uint32          IHGpio; // GPIO to be used for interrupt handler
} wcd_init_param_v1_0;

/* version 1.1
  Default configuration
  -Interrupt handler is enabled and the IRQ is registered for the specified GPIO
*/
typedef struct
{
  uint16          MajorVersion;  //(MUST be 1)
  uint16          MinorVersion;  //(MUST be 0)
  wcd_bus_type    bustype; // E_WCD_SLIMBUS or E_WCD_I2C    
  uint32          IHGpio; // GPIO to be used for interrupt handler
  wcd_mclk_speed  mclk_speed; //E_WCD_9_6_MHZ or E_WCD_12_288MHZ
} wcd_init_param_v1_1;

/* version 2.1
  Default configuration
  -Interrupt handler is enabled and the IRQ is registered for the specified GPIO
*/
typedef struct
{
  uint16          MajorVersion;  //(MUST be 1)
  uint16          MinorVersion;  //(MUST be 0)
  wcd_bus_type    bustype; // E_WCD_SLIMBUS or E_WCD_I2C    
  uint32          IHGpio; // GPIO to be used for interrupt handler
  wcd_mclk_speed  mclk_speed; //E_WCD_9_6_MHZ or E_WCD_12_288MHZ
  wcd_vdd_buck_voltage vdd_buck; // 1.8V or 2.15V
} wcd_init_param_v2_1;

/**
 * \brief The different codecs supported by the WCD driver.
 */
typedef enum
{
  E_WCD_WCD9320,
  E_WCD_WCD9302,
  E_WCD_WCD9306,
  E_WCD_HELICON,
  E_WCD_TOMBAK,
  E_WCD_WCD9330,
  E_WCD_CONGA,
  E_WCD_CODEC_32B_ANCHOR = 0x7FFFFFFF
} wcd_codec;

/**
 * \brief The different codec versions supported by the WCD driver.
 */
typedef enum
{
  E_WCD_VER_1P0,
  E_WCD_VER_2P0,
  E_WCD_CODEC_VERSION_32B_ANCHOR = 0x7FFFFFFF
} wcd_codec_version;

/**
 * \brief Special get codec info flag supported by the WCD driver.
 */
typedef enum
{
  E_WCD_CODEC_INFO_REGULAR = 0,
  E_WCD_CODEC_INFO_FORCE_DETECT,
  E_WCD_CODEC_INFO_MAX = 0x7FFFFFFF
} wcd_codec_info_force_detect;

/**
 * \brief Either RX or TX slimbus direction.
 */
typedef enum
{
  E_WCD_SLIMBUS_RX = 0,
  E_WCD_SLIMBUS_TX,
  E_WCD_SLIMBUS_DIRECTION_32B_ANCHOR = 0x7FFFFFFF
} wcd_slimbus_direction;

/*****************************************************************************
 * WCD 93XX ANC types.
*****************************************************************************/

/**
 * \brief ANC Input devices
 */
typedef enum
{
  E_ANC_DEV_ADC1 = 0x1,
  E_ANC_DEV_ADC2,
  E_ANC_DEV_ADC3,
  E_ANC_DEV_ADC4,
  E_ANC_DEV_ADC5,
  E_ANC_DEV_ADC6,
  E_ANC_DEV_ADCMB, // ADC7
  E_ANC_DEV_DMIC1 = 0x9,
  E_ANC_DEV_DMIC2,
  E_ANC_DEV_DMIC3,
  E_ANC_DEV_DMIC4,
  E_ANC_DEV_DMIC5,
  E_ANC_DEV_DMIC6,
  E_ANC_DEV_MAX = 0x7FFFFFFF
} wcd_adie_codec_db_anc_input_device;

/**
 * \brief ADIE DB ANC Config for a channel
 *
 * ACDB will store 2 of these structs back to back. 
 * The first struct represents channel 1, the second channel 2.
 *
 */
typedef struct
{
  wcd_bool                              default_enable;                   //check if the anc channel is enabled or not 
  wcd_bool                              anc_feedback_enable;              //CDC_CLK_ANC_CLK_EN_CTL
  wcd_bool                              anc_lr_mix_enable;                //CDC_ANC_B1_CTL  -- used with anc_lr_mix_output_channel, if true and 1, set it for CDC_ANC1_CTL, if true and 2, set it for CDC_ANC2_CTL, if false, set to false
  wcd_bool                              smlpf_enable;                     //CDC_ANC_B1_CTL
  wcd_bool                              ff_in_enable;                     //CDC_ANC_B1_CTL
  wcd_bool                              hybrid_enable;                    //CDC_ANC_B1_CTL
  wcd_bool                              ff_out_enable;                    //CDC_ANC_B1_CTL
  wcd_bool                              dcflt_enable;                     //CDC_ANC_B1_CTL
  wcd_bool                              adaptive;                         //check for adaptive ANC or not
  wcd_bool                              adaptive_gain_enable;             //CDC_ANC_B1_CTL
  wcd_bool                              flex_enable;                      //CDC_ANC_B2_CTL
  wcd_bool                              dmic_x2_enable;                   //CDC_ANC_B2_CTL
  uint8                                 anc_lr_mix_output_channel;        //only used if anc_lr_mix_enable = true  1 = channel 1, 2 = channel 2
  int8                                  anc_ff_shift;                     //CDC_ANC_SHIFT // -4 -> 6
  int8                                  anc_fb_shift;                     //CDC_ANC_SHIFT // -4 -> 6
  uint8                                 padding;
  int32                                 anc_ff_coeff[15];                 //CDC_ANC_IIR_B3_CTL // IIR coeffs: A[2 - 8] B[1-8] - Q3.13 (signed)
  int32                                 anc_gain;                         //iir coefficient calculation
  int32                                 anc_ff_lpf_shift[2];              //CDC_ANC_LPF_B1_CTL // 0 -> 12
  int32                                 anc_fb_coeff[13];                 //CDC_ANC_IIR_B3_CTL // IIR coeffs: A[2 - 7] B[1-7] - Q3.13 (signed)
  int32                                 anc_gain_default;                 //iir coefficient calculation
  int32                                 anc_fb_lpf_shift[2];              //CDC_ANC_LPF_B1_CTL // 0 -> 12
  wcd_adie_codec_db_anc_input_device    input_device;                     //CDC_ANC_CTL_ADC_DMIC_SEL & CDC_CONN_ANC_B1_CTL
  uint32                                smlpf_shift;                      //CDC_ANC_SMLPF_CTL
  uint32                                dcflt_shift;                      //CDC_ANC_DCFLT_CTL
  int32                                 adaptive_gain;                    //CDC_ANC_GAIN_CTL
} wcd_adie_codec_db_anc_config;

/**
 * \brief The number of Stages in the IIR on the codec.
 */
#define WCD_IIR_STAGE_COUNT           5

/**
 * \brief The number of coefficients in each stage of the the IIR on the codec.
 */
#define WCD_IIR_COEFFICIENT_COUNT     5

/**
 * \brief ADIE DB IIR Config for sidetone
 *
 * ACDB will store one of these per TX/RX device pair. 
 * This is only used for ENABLE/DISABLE sidetone.
 */
typedef struct
{
  //0 = false, 1 = enable
  uint16    enable;   
  //Integer format in dB (Max = 40dB, min = -84dB. 0db = 0; 5dB = 5; etc.)
  int16     pregain;  
  //Q2.28 (including sign bit). Order each each stage: B0, B1, B2, A1, A2
  uint32    coefficients[WCD_IIR_STAGE_COUNT * WCD_IIR_COEFFICIENT_COUNT];
} wcd_adie_codec_db_sidetone_iir_config;

/**
 * \brief ACDB IIR Configuration storage format for QACT RTC (ACPH).
 *
 * This is only used for GET/SET SIDETONE IIR Configuration
 */
typedef struct
{
  uint32    tx_usecase_handle;  // The TX usecase handle.
  uint32    module_id;          // Module ID of the sidetone IIR Data 0x12A07
  uint32    parameter_id;       // Parmeter ID of the sidetone IIR Data 0x12A08
  uint16    length;             // Size of the iir configuration below
  uint16    reserved;           // padding
  wcd_adie_codec_db_sidetone_iir_config   iir_config;   // IIR Configuration data
} wcd_acdb_iir_config;

/*****************************************************************************
 * IOCTL Command Structures
*****************************************************************************/

/**
 * \brief Command structure associated with WCD_IOCTL_CONNECT_SOURCE_TO_SINK
 */
typedef struct
{
  wcd_source  source;     /**<> The source of the connection. If dynamic porting is used, this will be changed to the port that is used. Refer to WCD Source  */

  wcd_sink    sink;       /**<> The sink of the connection. If dynamic porting is used, this will be changed to the port that is used. Refer to WCD Sink  */

  wcd_connection_type   connection_type;      /**< The type of the connection. Refer to Connection Type  */

  uint32      connection_id;      /**> The connection ID of the connection just created.  */

} wcd_ioctl_cmd_connect_source_to_sink;


/**
 * \brief Command structure associated with WCD_IOCTL_CONNECT_SOURCES_TO_SINKS
 */
typedef struct
{
  wcd_source  sources[WCD_MAX_ADIE_CHANNELS];     /**< The sources of the connections. Refer to WCD Source  */

  wcd_sink    sinks[WCD_MAX_ADIE_CHANNELS];       /**< The sinks of the connections. Refer to WCD Sink  */

  wcd_connection_type   connection_types[WCD_MAX_ADIE_CHANNELS];      /**< The types of the connections. Refer to Connection Type  */

  uint32      number_of_connections;      /**< The number of connections to create.  */

} wcd_ioctl_cmd_connect_sources_to_sinks;


/**
 * \brief Command structure associated with WCD_IOCTL_DISCONNECT_CONNECTION
 */
typedef struct
{
  uint32      connection_id;      /**> The connection ID returned in CONNECT_SOURCE_TO_SINK.  */
} wcd_ioctl_cmd_disconnect_connection;

/**
 * \brief Command structure associated with WCD_IOCTL_ENABLE_USECASE
 */
typedef struct
{
  uint32      sample_rate;      /**> The sample rate (in integer) to enable the usecase with.  */
  uint32      bit_width;        /**> The bit width (in integer) to enable the usecase with.  */
} wcd_ioctl_cmd_enable_usecase;

/**
 * \brief Each ACDB command consisting of 2 32bit integers
 */
typedef struct
{
  uint32 parameter_one;
  uint32 parameter_two;
} wcd_acdb_command;

/**
 * \brief Command structure associated with WCD_IOCTL_SET_PROFILE
 */
typedef struct
{
  uint32      command_count;      /**<  The number of ACDB commands in the profile. */
  wcd_acdb_command* acdb_commands;  /**<  The profile. */
  uint32      version;            /**< The version of the profile. */
} wcd_ioctl_cmd_set_profile;

/**
 * \brief Command structure associated with WCD_IOCTL_GET_SLIMBUS_LOGICAL_ADDRESS
 */
typedef struct
{
  uint8   pgd_logical_addr;       /**> The SLIMbus logical address of the PGD device. */
  uint8   intf_logical_addr;      /**> The SLIMbus logical address of the Interface device. */
} wcd_ioctl_cmd_slimbus_logical_address;

/**
 * \brief The slimbus slave interfaces on the codec.
 */
typedef enum
{
  E_WCD_CODEC_MAIN = 0, //I2C or SB PGD depending on which bus is used
  E_WCD_INTF,
  E_WCD_AHB,
  E_WCD_INTERRUPT,
  E_WCD_CTRL_BUS_INTERFACE_8B_ANCHOR = 0X7F
} wcd_ctrl_bus_interface;

/**
 * \brief Command structure associated with WCD_IOCTL_REGISTER_WRITE and WCD_IOCTL_REGISTER_READ
 */
typedef struct
{
  wcd_ctrl_bus_interface ctrl_bus_intf;    /**< The interface to access the register from. For I2C & SB PGD this should be E_WCD_CODEC_MAIN. */
  uint16  reg;                    /**< The register to access. */
  uint8   mask;                   /**< The mask to access the register with. */
  uint8   shift;                  /**< The shift to access the register with. */
  uint8   value;                  /**<> The value read or written to the register. */
} wcd_ioctl_cmd_register_access;

/**
 * \brief Structure associated with WCD_IOCTL_ENABLE_SLIMBUS_INTF
 * 
 * Structure describing the slimbus configuration required per usecase
 * this structure is only required for ADIE_CODEC_INTERFACE_SLIMBUS_V2 interface 
 * where slimbus slave interface device has to be configured by WCD driver.
 *
 *
 * @param direction       : Indicates id the usecase is a Rx or TX device
 * @param num_of_channels : number of channels used on the provided arrays
 * @param shared_channel_num[WCD_MAX_ADIE_CHANNELS]: Contains the shared channel
                  numbers associated with each slave port id.
 */
typedef struct
{
  uint16 num_of_channels;
  wcd_slimbus_direction direction;
  uint8  shared_channel_num[WCD_MAX_ADIE_CHANNELS];
} wcd_ioctl_cmd_slimbus_intf_config;

/**
 * \brief Structure associated with WCD_IOCTL_ENABLE_ANC
 */
typedef struct
{
  wcd_adie_codec_db_anc_config*       anc_configuration;  /**<  The ANC configuration to apply. */
  uint32                              command_count;      /**<  The number of ACDB commands in the profile. */
  wcd_acdb_command*                   acdb_commands;      /**<  The profile. */
  uint32                              version;            /**<  The version of the profile. */
} wcd_ioctl_cmd_enable_anc;

/**
 * \brief Structure associated with WCD_IOCTL_GET_CODEC_INFO
 *
 * @param codec                   : The codec family that the drivers reads
 * @param version                 : The version of the codec the driver reads
 * @param max_rx_ports            : The maximum number of RX ports (different for I2S or SB)
 * @param max_tx_ports            : The maximum number of TX ports (different for I2S or SB)
 * @param supported_sample_rates  : A mask containing which sample rates are supported on the codec
 * @param supported_bit_widths    : A mask containing which bit widths are supported on the codec
 * @force_detect                  : Special flag that can be used to verify codec bus interface and retrieve codec id & version details
 */
typedef struct
{
  wcd_codec                                 codec;
  wcd_codec_version                         version;
  uint32                                    max_rx_ports;
  uint32                                    max_tx_ports;
  uint32                                    supported_sample_rates;
  uint32                                    supported_bit_widths;
  wcd_codec_info_force_detect               force_detect;
} wcd_ioctl_cmd_codec_info;

/**
 * \brief Structure associated with WCD_IOCTL_ENABLE_SIDETONE
 *
 * @param tx_usecase              : The TX usecase to use in the sidetone connection.
 * @param iir_configuration       : The configuration of the IIR filter in the sidetone connection on the codec.
 */
typedef struct
{
  void*                                     tx_usecase;
  wcd_adie_codec_db_sidetone_iir_config*    iir_configuration;
} wcd_ioctl_cmd_enable_sidetone;

/**
* \brief Speaker protection path enums.
*/
typedef enum
{
  E_WCD_SPKR_PROT_NONE = 0,
  E_WCD_SPKR_PROT_1,
  E_WCD_SPKR_PROT_2,
  E_WCD_SPKR_PROT_1_2,
  E_WCD_SPKR_PROT_MAX
} wcd_spkr_prot_path_no;

/**
 * \brief Structure associated with WCD_IOCTL_ENABLE_FB_SPKR_PROT
 *
 * @param shared_channel_num_v    : The shared channel number of the voltage coming from the speaker
 * @param shared_channel_num_i    : The shared channel number of the current coming from the speaker
 * @param speaker_path_no         : The speaker path number that indicates if Spkr1 or Spkr2 needs to be configured
 */
typedef struct
{
  uint8 shared_channel_num_v;
  uint8 shared_channel_num_i;
  uint8 shared_channel_num_v_2;
  uint8 shared_channel_num_i_2;
  wcd_spkr_prot_path_no speaker_path_no;
} wcd_ioctl_cmd_enable_fb_spkr_prot;

/**
 * \brief Structure associated with WCD_IOCTL_GET_SIDETONE_IIR_CONFIG and WCD_IOCTL_SET_SIDETONE_IIR_CONFIG
 *
 * @param acdb_configuration       : The configuration from ACDB/ACPH QACT.
 */
typedef struct
{
  wcd_acdb_iir_config    acdb_configuration;
} wcd_ioctl_cmd_sidetone_iir_config;




typedef enum
{
  E_WCD_MAD_MBIAS_FIRST  = 0,
  E_WCD_MAD_NO_MBIAS    = 0,
  E_WCD_MAD_MBIAS1      = 1,
  E_WCD_MAD_MBIAS2      = 2,
  E_WCD_MAD_MBIAS3      = 3,
  E_WCD_MAD_MBIAS4      = 4,
  E_WCD_MAD_MBIAS_LAST  = 4,
  E_WCD_MAD_MBIAS_8B_ANCHOR = 0x7f
}wcd_mad_mbias_sel;

typedef enum
{
  E_WCD_MAD_ADC_FIRST     = 0x00,
  E_WCD_MAD_ADC_MB        = 0x00,
  E_WCD_MAD_ADC1          = 0x01,
  E_WCD_MAD_ADC2          = 0x02,
  E_WCD_MAD_ADC3          = 0x03,
  E_WCD_MAD_ADC4          = 0x04,
  E_WCD_MAD_ADC5          = 0x05,
  E_WCD_MAD_ADC6          = 0x06,
  E_WCD_MAD_ADC_LAST      = 0x06,
  E_WCD_MAD_DMIC_FIRST    = 0x08,
  E_WCD_MAD_DMIC1         = 0x08,
  E_WCD_MAD_DMIC2         = 0x09,
  E_WCD_MAD_DMIC3         = 0x0A,
  E_WCD_MAD_DMIC4         = 0x0B,
  E_WCD_MAD_DMIC5         = 0x0C,
  E_WCD_MAD_DMIC6         = 0x0D,
  E_WCD_MAD_DMIC_LAST     = 0x0D,
  E_WCD_MAD_MIC_8B_ANCHOR = 0x7f
}wcd_mad_mic_sel;
  
#define WCD_MAD_STAGE_COUNT         4
#define WCD_MAD_COEFFICIENT_COUNT   9

typedef struct
{
  uint8               microphone;                   // Use enum wcd_mad_mic_sel
  uint8               cycle_time;
  uint8               mic_settle_time;
  uint8               mic_type;                     // 0:ECM; 1:MEMS
}wcd_mad_mic_config;

typedef struct
{
  uint8               mbias;                      // Use enum wcd_mad_mbias_sel
  uint8               k_factor;                   // 4 -> 44
  uint8               external_bypass_capacitor;  // 0/1
  uint8               internal_resistor_biasing;  // 0/1
  uint8               cfilter;                    // 1 -> 3
  uint8               high_power_force_mbias_on;
  uint8               padding[2];
}wcd_mad_mbias_config;

typedef struct
{
  uint8   rms_omit_samples;          // 0 -> 255
  uint8   rms_comp_time;             // 0 -> 7
  uint8   detection_mechanism;       // 0 -> 3
  uint8   rms_diff_threshold;        // 0 -> 63
  uint8   rms_threshold_lsb;         // 0 -> 255
  uint8   rms_threshold_msb;         // 0 -> 255
  uint8   padding[2];
  uint8   iir_coefficients[36];      // 0 -> 255
}wcd_audio_beacon_config;

typedef struct
{
  uint8   rms_comp_time;             // 0 -> 7
  uint8   detection_mechanism;       // 0 -> 3
  uint8   rms_diff_threshold;        // 0 -> 63
  uint8   rms_threshold_lsb;         // 0 -> 255
  uint8   rms_threshold_msb;         // 0 -> 255
  uint8   padding[3];
  uint8   iir_coefficients[36];      // 0 -> 255
}wcd_ultrasound_config;
 

/**
 * \brief Structure associated with WCD_IOCTL_CONFIGURE_MAD
 *
 */
typedef struct
{
  uint32 version;
  wcd_mad_mic_config      microphone_info;
  wcd_mad_mbias_config    micbias;
  wcd_audio_beacon_config audio;
  wcd_audio_beacon_config beacon;
  wcd_ultrasound_config   ultrasound;
//  uint8                   shared_channel_num_mad;
}wcd_configure_mad;

typedef wcd_configure_mad wcd_ioctl_cmd_configure_mad;



/*****************************************************************************
 * WCD 93XX internal types.
*****************************************************************************/

//forward decleration begin
typedef struct _wcd_route wcd_route;
typedef struct _wcd_element wcd_element;
//forward decleration end

/**
 * \brief Structure describing each element in the WCD List
 *
 */
typedef struct _wcd_list_element
{
  void *data;
  struct _wcd_list_element *next;
  struct _wcd_list_element *previous;
} wcd_list_element;

/**
 * \brief Structure describing the WCD List
 *
 */
typedef struct _wcd_list
{
  wcd_list_element *head;
  wcd_list_element *tail;

  uint32 list_size;
} wcd_list;

/**
 * \brief Custom event context pointer.
 *
 */
typedef struct
{
  uint32 sample_rate;           //Sample rate of the usecase
  uint32 bit_width;             //Bit width of the ucse
  wcd_list* busport_elements;   //Busport elements of the usecase
} wcd_event_context_pointer;

/**
 * \brief HAL Elements' Event function pointer typedef.
 */
typedef wcd_result (*p_wcd_event_function)(wcd_element*, uint32, wcd_list*, wcd_event_context_pointer*);


/**
 * \brief This is the structure that is contained in each element's event map for the different
 * events supported by each element
 */
typedef struct
{
  uint32 id;
  p_wcd_event_function event_function;
} wcd_element_event;

/**
 * \brief WCD HAL Element - Structure describing different elements in the codec
 *
 */
struct _wcd_element
{
  uint32 id;
  uint32 type;

  uint32 connectable_elements_count;
  wcd_element **connectable_elements;

  wcd_element *connected_element; // This is only useful for muxes

  uint32  events_count;
  wcd_element_event* events_map;
  
  uint32 bringup_count;
  uint32 reference_count;
};

/**
 * \brief Use case enable level
 *
 */
typedef enum
{
  E_WCD_USECASE_DISABLED,
  E_WCD_USECASE_ENABLED
} wcd_usecase_enable_level;

/**
 * \brief Element table for fast access during enable/disable.
 */
typedef struct
{
  wcd_list*      elements_list[WCD_MAX_ELEMENT_TYPES];
} wcd_element_table;

/** 
 * \brief Structure describing each route in a connection
 *
 */
struct _wcd_route
{
  uint32 id;

  wcd_list *elements;
  wcd_list *differential_elements;    //list of elements for differential connections
  wcd_list *sidetone_elements;        //list of elements for sidetone connections

  //route table for fast access during enable/disable
  wcd_element_table* route_table;          
  wcd_element_table* differential_table;
  wcd_element_table* sidetone_table;
};

/**
 * \brief Connection States
 *
 */
typedef enum
{
  E_WCD_CONNECTION_START,                    //default
  E_WCD_CONNECTION_CONNECTED,
  E_WCD_CONNECTION_ENABLED,
  E_WCD_CONNECTION_ERROR,
  E_WCD_CONNECTION_DISABLED = E_WCD_CONNECTION_CONNECTED
} wcd_connection_state;

/**
 * \brief Structure for connections in a usecase
 *
 */
typedef struct
{
  uint32 id;
  wcd_connection_state state;
  wcd_route *route;
  adie_codec_direction connection_direction;
} wcd_connection_info;

/**
 * \brief Structure for usecase
 *
 */
typedef struct
{
  uint32 id;
  wcd_usecase_enable_level enable_level;    // do not set properties if the enable_level is 0 (disabled)

  wcd_list *connection_list;
  uint32   connect_blocked;     // prevents any more CONNECT_SOURCE_TO_SINK from going through

  uint32 current_command;
  uint32 command_list_size;
  wcd_acdb_command* command_list;

  uint32  anc_current_command;
  uint32  anc_command_list_size;
  wcd_acdb_command* anc_command_list;
  wcd_list* primary_anc_elements;
  wcd_list* secondary_anc_elements;
  wcd_bool  adaptive_anc;

  wcd_event_context_pointer* context_pointer;

  void*   slimbus_shared_channel_handle[WCD_MAX_ADIE_CHANNELS];
  void*   slimbus_shared_channel_group_handle;

  wcd_bool spkr_prot_enabled; 
  wcd_bool spkr_2_prot_enabled;
  
} wcd_usecase;

/**
 * \brief Codec info. Returned from wcd_get_codec_info in target_defs.h
 *
 */
typedef struct
{
  wcd_codec codec;
  wcd_codec_version version;
} wcd_codec_info;

typedef struct _wcd_reset_codec_payload_struct
{
  wcd_vdd_buck_voltage  buck_voltage;
} wcd_reset_codec_payload_struct;

/** 
  * \brief Enum describing the events supported by the ioctl
  *        manager. Ioctl manager uses these events to interact
  *        with the rest of the WCD internal modules, such as
  *        MBHC.
  *
  */
typedef enum
{
  E_WCD_INVALID_IOCTL_MGR_EVENT = -1,
  E_WCD_CMD_MBHC_MIN,
  E_WCD_CMD_MBHC_SET_START,
  E_WCD_CMD_MBHC_SET_STOP,
  E_WCD_CMD_GET_GPIO_DET_CB,
  E_WCD_CMD_GET_IH_CB,
  E_WCD_CMD_SET_GPIO_INTF_CB,
  E_WCD_CMD_SET_EVENT_CB,
  E_WCD_CMD_MBHC_SET_FTM_START,
  E_WCD_CMD_MBHC_SET_FTM_CMD,
  E_WCD_CMD_MBHC_INVOKE_GPIO_INTR_CB,
  E_WCD_CMD_IH_INVOKE_INTR_CB,
  E_WCD_CMD_MBHC_SET_TRIGGER_CMD,
  E_WCD_CMD_MBHC_MAX,
  E_WCD_MAX_IOCTL_MGR_EVENT
} wcd_ioctl_manager_events;

// Following structure and enum are used for invoking a WCD CB
// Used for WCD_IOCTL "WCD_IOCTL_CMD_INVOKE_CB"
typedef enum _wcd_invoke_cb_event
{
  E_WCD_INVOKE_CB_EVENT_NONE = 0,
  E_WCD_INVOKE_CB_IH_EVENT,
  E_WCD_INVOKE_CB_MBHC_PMIC_GPIO_DETECTION_EVENT,
  E_WCD_INVOKE_CB_EVENT_MAX
} wcd_invoke_cb_event;

typedef enum {
  E_WCD_INTR_SWITCH_INT,
  E_WCD_INTR_MIC_ELEC_INS_REM_DET,
  E_WCD_INTR_BUTTON_PRESS,
  E_WCD_INTR_BUTTON_RELEASE,
  E_WCD_INTR_MIC_ELEC_INS_REM_DET1,
  E_WCD_INTR_MAX
}wcd_intr_type;

typedef struct _wcd_invoke_cb_event_struct
{
  wcd_invoke_cb_event      wcd_cb_event;
  wcd_intr_type            wcd_intr_num;
} wcd_invoke_cb_event_struct, *p_wcd_invoke_cb_event_struct;

/*****************************************************************************
 * WCD Callback Related Definitions - Common
*****************************************************************************/

// wcd_owner_module enumerates different WCD sub-modules such as
// MBHC, MAD, VBAT etc.
// Everything else is considered as E_WCD_CORE.
typedef enum _wcd_owner_module
{
  E_WCD_INVALID = -1,
  E_WCD_CORE,
  E_WCD_MBHC,
  E_WCD_SVA,
  E_WCD_CORE_POWER,
  E_WCD_MBHC_POWER,
  E_WCD_SVA_POWER,
  E_WCD_LATENCY,
  E_WCD_OWNER_MAX
} wcd_owner_module;

// WCD detection event ids that will be sent to the client as part of the callback function
typedef enum _wcd_detection_event
{
  E_WCD_EVENT_INVALID = -1,
  E_WCD_EVENT_NONE,
  E_WCD_MBHC_EVENT_PLUG_INSERT,
  E_WCD_MBHC_EVENT_PLUG_REMOVE,
  E_WCD_MBHC_EVENT_BUTTON_PRESS,
  E_WCD_MBHC_EVENT_BUTTON_RELEASE,
  E_WCD_MBHC_EVENT_BUTTON_PRESS_RELEASE,
  E_WCD_MBHC_EVENT_FTM_INTR_LINE,
  E_WCD_MBHC_EVENT_FTM_MICB_MIC_CONNECTION,
  E_WCD_MBHC_EVENT_FTM_JACK_US,
  E_WCD_MBHC_EVENT_FTM_JACK_EU,
  E_WCD_MBHC_EVENT_INCORRECT_PLUG_JACK_COMBINATION,
  E_WCD_MBHC_EVENT_CORRECT_PLUG_JACK_COMBINATION,
  E_WCD_MBHC_EVENT_FTM_SIMULATE_INS_DET_INTR_RESPONSE,
  E_WCD_MBHC_EVENT_PLUG_DETECTION_IN_PROGRESS,
  E_WCD_MBHC_EVENT_RESET,
  E_WCD_EVENT_MAX
} wcd_detection_event;

/*****************************************************************************
 * WCD Callback Related Definitions - MBHC
*****************************************************************************/

/* Enum for identifying the type of accessory plugged in for MBHC_PLUG_INSERT &  MBHC_PLUG_REMOVE*/

typedef enum _mbhc_accessory
{
  E_WCD_MBHC_ACCESSORY_INVALID                                        = 0,
  E_WCD_MBHC_ACCESSORY_NO_PLUG                                        = 1,
  E_WCD_MBHC_ACCESSORY_NA_STEREO_HEADSET_WITH_MICROPHONE_HHGM         = 2,
  E_WCD_MBHC_ACCESSORY_NA_MONO_HEADSET_WITH_MICROPHONE_HXGM           = 3,
  E_WCD_MBHC_ACCESSORY_SPECIAL_HEADSET_HHGM                           = 4,
  E_WCD_MBHC_ACCESSORY_EU_STEREO_HEADSET_WITH_MICROPHONE_HHMG         = 5,
  E_WCD_MBHC_ACCESSORY_EU_MONO_HEADSET_WITH_MICROPHONE_HXMG           = 6,
  E_WCD_MBHC_ACCESSORY_NA_ANC_STEREO_HEADSET_WITH_MICROPHONE_HHGM     = 7,
  E_WCD_MBHC_ACCESSORY_NA_ANC_MONO_HEADSET_WITH_MICROPHONE_HXGM       = 8,
  E_WCD_MBHC_ACCESSORY_STEREO_HEADPHONE_WITHOUT_MICROPHONE_HHGG       = 9,
  E_WCD_MBHC_ACCESSORY_MONO_HEADPHONE_WITHOUT_MICROPHONE_HXGG         = 10,
  E_WCD_MBHC_ACCESSORY_AUDIO_LINEOUT_FFGG                             = 11,
  E_WCD_MBHC_ACCESSORY_PLUG_GGGG                                      = 12,
  E_WCD_MBHC_ACCESSORY_4POLE_EXTENSION_CABLE_FFFF                     = 13,
  E_WCD_MBHC_ACCESSORY_ANC_STEREO_HEADPHONE_WITHOUT_MICROPHONE_HHGG   = 14,
  E_WCD_MBHC_ACCESSORY_ANC_MONO_HEADPHONE_WITHOUT_MICROPHONE_HXGG     = 15,
  E_WCD_MBHC_ACCESSORY_MAX = 100
} mbhc_accessory;

typedef enum _mbhc_plug_jack_combo
{
	E_WCD_MBHC_INCORRECT_PLUG_JACK_COMBO = 0,
	E_WCD_MBHC_CORRECT_PLUG_JACK_COMBO
} mbhc_plug_jack_combo;

typedef struct _mbhc_button_details
{
    uint32			button_id;		      
    uint32			button_press_duration;
} mbhc_button_details;

typedef enum _mbhc_pin_impedance_range
{
	E_WCD_MBHC_PINOUT_GND = 0,
	E_WCD_MBHC_PINOUT_MIC,
	E_WCD_MBHC_PINOUT_HPH,
	E_WCD_MBHC_PINOUT_FLOATING,
	E_WCD_MBHC_PINOUT_UNKNOWN
} mbhc_pin_impedance_range;

typedef struct _mbhc_pin_details
{
	mbhc_pin_impedance_range        imp_range;	
	uint32                          imp_value;        // In Ohms
} mbhc_pin_details;

typedef struct _mbhc_plug_impedance_details
{
	mbhc_pin_details                tip_impedance;		
	mbhc_pin_details                ring1_impedance;	
	mbhc_pin_details                ring2_impedance;	
	mbhc_pin_details                sleeve_impedance;	
	uint32                          no_of_channels;			
	uint32                          R_channel_shorted_to_L;   // 0 = R is not shorted to L
                                                            // 1 = R is shorted to L
} mbhc_plug_impedance_details;

typedef struct _mbhc_payload
{
	wcd_owner_module                owner_module;
	wcd_detection_event             evt_id;
	mbhc_accessory                  accessory_type;	
	mbhc_plug_jack_combo            plug_jack_combination;  
	mbhc_button_details             button_details;
	mbhc_plug_impedance_details     plug_impedance_details;			
	wcd_result                      cmd_response;		
} mbhc_payload;

typedef enum _wcd_power_state_type
{
  E_WCD_POWER_INVALID = -1,
  E_WCD_POWER_IDLE,
  E_WCD_POWER_ACTIVE,
  E_WCD_POWER_DEFAULT
} wcd_power_state_type;

// The following payload is used by a WCD module
// to call the callback originally provided by the client as part of 
// IOCTL "WCD_IOCTL_CMD_SET_EVENT_CB"
// 
// This payload is used by WCD to provide dynamic changes in the power state of the calling module
typedef struct _wcd_power_state_payload
{
  wcd_owner_module          power_state_owner;                  
  wcd_power_state_type	    power_state;   // enum = wcd_power_state_type
} wcd_power_state_payload;

/*****************************************************************************
 * WCD Callback Related Definitions - Event CB
*****************************************************************************/

// typedef struct mbhc_payload;

// The following payload is used by a WCD module
// to call the callback originally provided by the client as part of 
// IOCTL "WCD_IOCTL_CMD_SET_EVENT_CB"
// 
// This payload is used by WCD to provide event callbacks to the client
typedef struct _wcd_event_cb_payload
{
  wcd_owner_module                owner_module;                  

  union wcd_module_payload
  {
    mbhc_payload                  mbhc_cb_payload;        // This payload is defined in "wcd_mbhc_api.h"
                                                          // and is used by MBHC module to provide event callbacks to the client              
    wcd_power_state_payload       wcd_power_cb_payload;   // This payload is defined in "wcd_types.h"
                                                          // and is used by different WCD modules to provide event callbacks to the client for dynamic changes in power requirements
//  vbat_payload                  vbat_cb_payload;        // This payload is used by VBAT module to provide event callbacks to the client
//  mad_payload                   mad_cb_payload;         // This payload is used by MAD module to provide event callbacks to the client
  } wcd_module_payload;

} wcd_event_cb_payload;

/*****************************************************************************
 * WCD Callback Definition
*****************************************************************************/

// Client provides its own callback function to WCD
// This callback function can be used by the client to
//    1. Provide EVENT CB function of the client to WCD modules, so that WCD modules can send an event back to the client,
//       such as Headset Insertion/Removal event
//    2. Provide GPIO INTERFACE CB function of the client to WCD modules, so that WCD modules can use this callback 
//       to READ/WRITE GPIO status

// Callback function proto type for the ioctls
//    1. "WCD_IOCTL_CMD_SET_GPIO_INTF_CB"
//    2. "WCD_IOCTL_CMD_SET_EVENT_CB"
//
// "payload" in this callback function is filled by WCD and can either be of:
//    1. Type "wcd_event_cb_payload", if it is a EVENT CB function pointer, used to provide WCD events to the client
//    OR
//    2. Type "wcd_gpio_intf_payload", if it is a GPIO INTERFACE CB function pointer, used by WCD to call the client callback to read/write GPIOs
typedef wcd_result (*wcd_client_intf_cb_fn)(void* payload, uint32 payload_size, void* client_data);

struct wcd_client_intf_cb
{
  wcd_client_intf_cb_fn	    cb;
  void*                     cb_data;
};

/*****************************************************************************
 * WCD Callback Related Definitions - GPIO Interface CB
*****************************************************************************/

typedef enum _mbhc_module_gpio_id
{
  E_WCD_MBHC_GPIO_AUDIO_VID_SEL = 0,
  E_WCD_MBHC_GPIO_US_EURO_SEL,
  E_WCD_MBHC_GPIO_ANC_HS_DET,
  E_WCD_MBHC_MBHC_GPIO_MAX
} mbhc_module_gpio_id;

typedef enum _wcd_module_gpio_id
{
  E_WCD_GPIO_RESET = E_WCD_MBHC_MBHC_GPIO_MAX+1,
  E_WCD_GPIO_MAX
} wcd_module_gpio_id;

typedef enum _wcd_gpio_type
{
  E_WCD_GPIO_INVALID = -1,
  E_WCD_GPIO_INPUT,
  E_WCD_GPIO_OUTPUT
} wcd_gpio_type;

typedef enum _wcd_gpio_value
{
  E_WCD_GPIO_FLIP = -1,
  E_WCD_GPIO_LOW,
  E_WCD_GPIO_HIGH,
  E_WCD_GPIO_DEFAULT,
  E_WCD_GPIO_UNKNOWN
} wcd_gpio_value;

// The following payload is used by a WCD module
// to call the callback originally provided by the client as part of 
// IOCTL "WCD_IOCTL_CMD_SET_GPIO_INTF_CB"
// 
// This payload is used by WCD to pass GPIO configuration to the client
typedef struct _wcd_gpio_intf_payload
{
  wcd_owner_module          gpio_owner;                  

  union gpio_id
  {
    wcd_module_gpio_id      wcd_gpio_id;        // enum = wcd_module_gpio_id
    mbhc_module_gpio_id	    mbhc_gpio_id;       // enum = mbhc_module_gpio_id
  } gpio_id;

  wcd_gpio_type	            type;               // enum = wcd_gpio_type
  int32                     *value;             // Value of the GPIO, Enum = wcd_gpio_value
                                                // This value will be set by MBHC/WCD and read by the client in case of O/P GPIO
                                                // This value will be filled by the client and read by MBHC/WCD in case of I/P GPIO
                                                // value = -1 = Flip
                                                // value =  0 = Off / Low
                                                // value =  1 = On  / High
                                                // value =  2 = Default
} wcd_gpio_intf_payload;

#endif



