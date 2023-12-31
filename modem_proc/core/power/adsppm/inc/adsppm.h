/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm.h
@brief: ADSPPM internal data structure and API
        This is the internal data and API for ADSPPM.
        This is not distributed to any clients that uses ADSPPM.

$Header: //components/rel/core.mpss/3.10/power/adsppm/inc/adsppm.h#2 $
*/

#ifndef ADSPPM_H_
#define ADSPPM_H_

#include "adsppm_types.h"
#include "adsppm_defs.h"
#include "coreUtils.h"
#include "stringl.h"


#define ADSPPM_REVISION 001

#define MAX_NUM_OF_ADSPPM_CLIENTS 192
//#define MAX_NUM_OF_ADSPPM_HANDLE_PRE_ALLOCATE MAX_NUM_OF_ADSPPM_CLIENTS
#define MAX_NUM_OF_ADSPPM_CLOCK_ARRAY 8
#define MAX_NUM_OF_ADSPPM_BW_ARRAY    8

#define ADSPPM_REG_PROC_CLOCK_BW_PERCENTAGE 1

#define MAX_ADSPPM_CLIENT_NAME    64
#define MAX_ADSPPM_REQ_Q_ELEMENTS 16


/******************************************************************************
 *                     General definitions
 *****************************************************************************/
/**
 * @enum AdsppmApiType - ADSPPM API TYPE
 */
typedef enum
{
     Adsppm_Api_Type_None,              //!< None
     Adsppm_Api_Type_Sync,              //!< SYNC API call
     Adsppm_Api_Type_Async,             //!< ASYNC API call
     Adsppm_Ape_Type_Enum_Max,          //!< enum max
     Adsppm_Api_Type_Force8bits = 0x7F  //!< forcing enum to be 32 bit
} AdsppmApiType;

/**
 * @enum ADSPPMStatusType - Values used for ADSPPM return status
 */
typedef enum
{
    Adsppm_Status_Success,                  //!< no error
    Adsppm_Status_Failed,                   //!< general failure
    Adsppm_Status_NoMemory,                 //!< insufficient Memory
    Adsppm_Status_VersionNotSupport,        //!< version not supported
    Adsppm_Status_BadClass,                 //!< NULL class object
    Adsppm_Status_BadState,                 //!< invalid state
    Adsppm_Status_BadParm,                  //!< invalid parameter
    Adsppm_Status_InvalidFormat,            //!< invalid format
    Adsppm_Status_UnSupported,              //!< API is not supported
    Adsppm_Status_ResourceNotFound,         //!< Resource not found
    Adsppm_Status_BadMemPtr,                //!< bad memory pointer
    Adsppm_Status_BadHandle,                //!< invalid handle
    Adsppm_Status_ResourceInUse,            //!< a resource is in use
    Adsppm_Status_NoBandwidth,              //!< insufficient bandwidth
    Adsppm_Status_NullPointer,              //!< null pointer
    Adsppm_Status_NotInitialized,           //!< not initialized
    Adsppm_Status_ResourceNotRequested,     //!< resource not requested
    Adsppm_Status_CoreResourceNotAvailable, //!< Core Resource not available
    Adsppm_Status_Max,                      //!< Maximum count
    Adsppm_Status_Force32Bits = 0x7FFFFFFF  //!< forcing enum to be 32 bit
} AdsppmStatusType;

typedef AdsppmStatusType Adsppm_Status; //!< ADSPPM status type

/**
 * @enum AdsppmCoreIdType - ADSPPM CORE ID
 */
typedef enum
{
    Adsppm_Core_Id_None        = 0,    //!< None
    Adsppm_Core_Id_ADSP        = 1,    //!< Q6SS HW inside of LPASS
    Adsppm_Core_Id_LPASS_Core  = 2,    //!< Generic ID for entire LPASS core
    Adsppm_Core_Id_LPM         = 3,    //!< Low Power Memory inside of LPASS
    Adsppm_Core_Id_DML         = 4,    //!< DML (Data Mover Lite) HW inside of LPASS
    Adsppm_Core_Id_AIF         = 5,    //!< AIF(Audio Interface) HW inside of LPASS
    Adsppm_Core_Id_SlimBus     = 6,    //!< SLIMBus HW inside of LPASS
    Adsppm_Core_Id_Midi        = 7,    //!< MIDI HW inside of LPASS
    Adsppm_Core_Id_AVsync      = 8,    //!< AVT/ProntoSync HW inside of LPASS
    Adsppm_Core_Id_HWRSMP      = 9,    //!< HW Resampler HW inside of LPASS
    Adsppm_Core_Id_SRam        = 10,   //!< Sensors RAM
    Adsppm_Core_Id_DCodec      = 11,   //!< Digital codec
    Adsppm_Core_Id_Spdif       = 12,   //!< SPDIF
    Adsppm_Core_Id_Hdmirx      = 13,   //!< HDMI RX
    Adsppm_Core_Id_Hdmitx      = 14,   //!< HDMI TX
    Adsppm_Core_Id_Sif         = 15,   //!< analog audio
    Adsppm_Core_Id_BSTC        = 16,   //!< BSTC
    Adsppm_Core_Id_HVX         = 17,   //!< HVX
    Adsppm_Core_Id_Max,                //!< Maximum count
    Adsppm_Core_Id_Force8Bits  = 0x7F  //!< forcing enum to be 8 bit
} AdsppmCoreIdType;

/**
 * @enum AdsppmInstanceIdType - ADSPPM Instance ID
 */
typedef enum
{
    Adsppm_Instance_Id_None        = 0,    //!< None
    Adsppm_Instance_Id_0           = 1,
    Adsppm_Instance_Id_1           = 2,
    Adsppm_Instance_Id_2           = 3,
    Adsppm_Instance_Id_Max,
    Adsppm_Instance_Id_Force8Bits  = 0x7F  //!< forcing enum to be 8 bit
} AdsppmInstanceIdType;

/**
 * @enum AdsppmRscIdType - ADSPPM resource ID
 */
typedef enum
{
    Adsppm_Rsc_Id_None            = 0,    //!< None
    Adsppm_Rsc_Id_Power           = 1,
    Adsppm_Rsc_Id_Core_Clk        = 2,
    Adsppm_Rsc_Id_Sleep_Latency   = 3,
    Adsppm_Rsc_Id_Mips            = 4,
    Adsppm_Rsc_Id_BW              = 5,
    Adsppm_Rsc_Id_Thermal         = 6,
    Adsppm_Rsc_Id_MemPower        = 7,
    Adsppm_Rsc_Id_Core_Clk_Domain = 8,
    Adsppm_Rsc_Id_Max,                    //!< Maximum count
    Adsppm_Rsc_Id_Force8Bits      = 0x7F  //!< forcing enum to be 8 bit
} AdsppmRscIdType;

/**
 * @enum AdsppmInitStateType - Initialization state of HAL and Manager layers
 */
typedef enum
{
    Adsppm_State_ACMInit = 0x1,
    Adsppm_State_HalIntrInit = 0x2,
    Adsppm_State_HalHwIoInit = 0x4,
    Adsppm_State_HalClkRgmInit = 0x8,
    Adsppm_State_HalBusInit = 0x10,
    Adsppm_State_HalSlpInit = 0x20,
    Adsppm_State_HalCAInit = 0x40,
    Adsppm_State_CoreCtxLockInit = 0x80,
    Adsppm_State_CoreRMInit = 0x100,
    Adsppm_State_CoreAMAsyncInit = 0x200,
    Adsppm_State_CoreMIPSInit = 0x400,
    Adsppm_State_CoreBUSInit = 0x800,
    Adsppm_State_CoreAHBMInit = 0x1000,
    Adsppm_State_CorePWRInit = 0x2000,
    Adsppm_State_CoreCLKInit = 0x4000,
    Adsppm_State_CoreSLEEPInit = 0x8000,
    Adsppm_State_CoreTHERMALInit = 0x10000,
    Adsppm_State_CoreMEMPWRInit = 0x20000,
    Adsppm_State_CoreCMInit = 0x40000,
    Adsppm_State_CoreCPMInit = 0x80000,
    Adsppm_State_CoreDCVSInit = 0x100000,
    Adsppm_State_CoreEXTBWInit = 0x200000,
    Adsppm_State_CoreADSPCLKInit = 0x400000,
    Adsppm_State_CoreQCMInit = 0x800000,
    Adsppm_State_CoreBMRegisterEvent = 0x1000000,
    Adsppm_State_Force32bit = 0x7fffffff
} AdsppmInitStateType;

/******************************************************************************
 *                     Callback definitions
 *****************************************************************************/
/**
 * @enum AdsppmCallbackEventIdType - Callback events
 */
typedef enum
{
    Adsppm_Callback_Event_Id_None,                     //!< None
    Adsppm_Callback_Event_Id_Thermal = 0x0002,         //!< Callback event: thermal event
    Adsppm_Callback_Event_Id_Async_Complete = 0x0004,  //!< Callback event: async request complete event
    Adsppm_Callback_Event_Id_Idle,                     //!< Callback event: except for MDP, MMSS is idle
    Adsppm_Callback_Event_Id_Busy,                     //!< Callback event: except for MDP, MMSS is busy
    Adsppm_Callback_Event_Id_Max,                      //!< Maximum count
    Adsppm_Callback_Event_Id_Force32Bits = 0x7FFFFFFF  //!< forcing enum to be 32 bit
} AdsppmCallbackEventIdType;

/**
 * @struct AdsppmCallbackParamType - Callback parameter structure
 */
typedef struct
{
    AdsppmCallbackEventIdType eventId;
    uint32                    clientId;
    uint32                    callbackDataSize; //!< size of callbackData
    void                      *callbackData;    //!< data structure of callbackData to be defined
                                                //!< when async calls are supported
} AdsppmCallbackParamType;


/******************************************************************************
 *                     Bus/Clock definitions
 *****************************************************************************/
/**
 * @enum AdsppmClkIdType - defines clock IDs as used by ADSPPM internally
 */
typedef enum
{
    AdsppmClk_None = 0,
    AdsppmClk_Adsp_Core,
//!< Core Bus Clocks Start
    AdsppmClk_Ahb_Root,
    AdsppmClk_AhbI_Hclk,
    AdsppmClk_AhbX_Hclk, 
    AdsppmClk_Aif_Hclk,
    AdsppmClk_Aif_Csr_Hclk,
    AdsppmClk_AvSync_Hclk,
    AdsppmClk_Lpm_Hclk,
    AdsppmClk_Lpm_cbc,
    AdsppmClk_Csr_Hclk,
    AdsppmClk_Bstc_Hclk,
    AdsppmClk_Smmu_Lpass_Hclk,
	AdsppmClk_Adsp_Hclk,
	AdsppmClk_Smmu_Sysnoc_Hclk,
    AdsppmClk_Sram_Hclk,
//!< Core Bus Clocks End
//!< AON Bus Clocks Start
    AdsppmClk_AhbE_Hclk,
//!< AON Bus Clocks End
//!< Core Clocks Start
    AdsppmClk_HwRsp_Core,
    AdsppmClk_Midi_Core,
    AdsppmClk_AvSync_Xo,
    AdsppmClk_AvSync_Bt,
    AdsppmClk_AvSync_Fm,
    AdsppmClk_Slimbus_Core,
    AdsppmClk_Slimbus2_Core,
    AdsppmClk_Avtimer_core,
    AdsppmClk_Atime_core,
    AdsppmClk_Atime2_core,
	AdsppmClk_BSTC_core,
//!< Core Clocks End
    AdsppmClk_EnumMax,                     //!< Max number of the items in the enum
    AdsppmClk_Slimbus_Hclk,
    AdsppmClk_Slimbus2_Hclk,
	// AdsppmClk_HwRsp_Hclk,
    // AdsppmClk_Dml_Hclk,
    // AdsppmClk_Slimbus_cbc, 
    // AdsppmClk_Slimbus2_cbc,
    // AdsppmClk_Midi_Hclk,
	// AdsppmClk_Atimer_Hclk,
	// AdsppmClk_Dcodec_Hclk,
    // AdsppmClk_Spdif_Hmclk,
    // AdsppmClk_Spdif_Hsclk,
    // AdsppmClk_Hdmirx_Hclk,
    // AdsppmClk_Hdmitx_Hclk,
    // AdsppmClk_Sif_Hclk,
	// AdsppmClk_Smmu_Adsp_Hclk,
	// AdsppmClk_Sysnoc_Hclk,
    // AdsppmClk_Sysnoc_cbc,
    // AdsppmClk_Bus_Timeout_Hclk,
    // AdsppmClk_Tlb_Preload_Hclk,
    // AdsppmClk_Qos_Hclk,
    // AdsppmClk_Qdsp_Sway_Hclk,
    // AdsppmClk_Adsp_Hmclk,
    // AdsppmClk_Adsp_Hsclk,
    // AdsppmClk_Lcc_Hclk,
    // AdsppmClk_Security_Hclk,
    // AdsppmClk_Wrapper_Security_Hclk,
    // AdsppmClk_Wrapper_Br_Hclk,
    // AdsppmClk_Audio_Core_AON,
    // AdsppmClk_Audio_Wrapper_AON,
    AdsppmClk_EnumForce32Bit = 0x7fffffff  //!< AdsppmClk_EnumForce32Bit
} AdsppmClkIdType;

/******************************************************************************
 *                     Bus definitions
 *****************************************************************************/
/**
 * @enum AdsppmBusPortIdType - defines Bus ports to be used by ADSPPM internally
 */
typedef enum
{
    AdsppmBusPort_None = 0,                    //!< AdsppmBusMaster_None
    AdsppmBusPort_Adsp_Master,                 //!< AdsppmBusMaster_Adsp
    AdsppmBusPort_Aif_Master,                  //!< AdsppmBusMaster_Aif
    AdsppmBusPort_Ext_Ahb_Master,              //!< SNOC Master port from AHB-X
    AdsppmBusPort_Aif_Slave,                   //!< AdsppmBusSlave_Aif
    AdsppmBusPort_AvSync_Slave,                //!< AdsppmBusSlave_AvSync
    AdsppmBusPort_Lpm_Slave,                   //!< AdsppmBusSlave_Lpm
    AdsppmBusPort_Ext_Ahb_Slave,               //!< SNOC Slave port from AHB-E
    AdsppmBusPort_Ddr_Slave,                   //!< AdsppmBusSlave_Ddr
    AdsppmBusPort_Bstc_Slave,                  //!< BSTC slave port
    AdsppmBusPort_Core,                        //!< Core current client is registered for
    AdsppmBusPort_Adsp_Slave,                  //!< ADSP core slave port for Reg Prog only. Not exposed to mmpm.h
    AdsppmBusPort_EnumMax,                     //!< AdsppmBusSlave_EnumMax
	AdsppmBusPort_Dml_Master,                  //!< AdsppmBusMaster_Dml
	AdsppmBusPort_Slimbus_Master,              //!< AdsppmBusMaster_Slimbus
    AdsppmBusPort_Slimbus2_Master,             //!< AdsppmBusMaster_Slimbus
    AdsppmBusPort_Midi_Master,                 //!< AdsppmBusMaster_Midi
    AdsppmBusPort_HwRsmp_Master,               //!< AdsppmBusMaster_HwRsmp
	AdsppmBusPort_Spdif_Master,                //!< SPDIF master port
    AdsppmBusPort_Hdmirx_Master,               //!< HDMI RX slave port
    AdsppmBusPort_Hdmitx_Master,               //!< HDMI TX slave port
    AdsppmBusPort_Sif_Master,                  //!< SIF slave port
    AdsppmBusPort_Dml_Slave,                   //!< AdsppmBusSlave_Dml
	AdsppmBusPort_Slimbus_Slave,               //!< AdsppmBusSlave_Slimbus
    AdsppmBusPort_Slimbus2_Slave,              //!< AdsppmBusSlave_Slimbus
    AdsppmBusPort_Midi_Slave,                  //!< AdsppmBusSlave_Midi
    AdsppmBusPort_HwRsmp_Slave,                //!< AdsppmBusSlave_HwRsmp
	AdsppmBusPort_Sram_Slave,                  //!< AdsppmBusSlave_Sram
    AdsppmBusPort_Ocmem_Slave,                 //!< AdsppmBusSlave_Ocmem
    AdsppmBusPort_PerifNoc_Slave,              //!< AdsppmBusSlave_PerifNoc
    AdsppmBusPort_Spdif_Slave,                 //!< SPDIF slave port
    AdsppmBusPort_Hdmirx_Slave,                //!< HDMI RX slave port
    AdsppmBusPort_Hdmitx_Slave,                //!< HDMI TX slave port
    AdsppmBusPort_Sif_Slave,                   //!< SIF slave port
	AdsppmBusPort_Dcodec_Slave,                //!< DCodec slave port
    AdsppmBusPort_EnumForce32Bit = 0x7fffffff  //!< AdsppmBusSlave_EnumForce32Bit
} AdsppmBusPortIdType;

/**
 * @struct AdsppmBusRouteType - Definition for the bus route type (master/slave pair)
 */
typedef struct
{
    AdsppmBusPortIdType masterPort;
    AdsppmBusPortIdType slavePort;
} AdsppmBusRouteType;

/**
 * @struct AdsppmBusBWDataIbAbType - Data type holding BW value for type 3 requests (Ab/Ib)
 */
typedef struct
{
    uint64 Ab;         //!< Average BW in bytes per second
    uint64 Ib;         //!< Instantaneous BW in bytes per second
    uint32 latencyNs;  //!< latency in nano sec
} AdsppmBusBWDataIbAbType;

/**
 * @enum AdsppmBwUsageLpassType - BW usage types for LPASS
 */
typedef enum
{
    Adsppm_BwUsage_None,                      //!< None
    Adsppm_BwUsage_DSP,                       //!< BW usage by ADSP
    Adsppm_BwUsage_DMA,                       //!< BW usage by DMA/BAM
    Adsppm_BwUsage_EXT_CPU,                   //!< BW usage by CPU outside of LPASS
    Adsppm_BwUsage_Enum_Max,                  //!< Maximum count */
    Adsppm_BwUsage_Force32Bbits = 0x7FFFFFFF  //!< forcing enum to be 32 bit
} AdsppmBwUsageLpassType;

/**
 * @struct AdsppmBusBWDataUsageType - Data type holding BW value for type 1 requests (BW, usage)
 */
typedef struct
{
    uint64 bwBytePerSec;              //!< BW value in bytes per second
    uint32 usagePercentage;           //!< utilization percentage
    AdsppmBwUsageLpassType usageType; //!< BW usage type
} AdsppmBusBWDataUsageType;

typedef union
{
    AdsppmBusBWDataIbAbType  busBwAbIb;
    AdsppmBusBWDataUsageType busBwValue;
} AdsppmBusBWDataType;

/**
 * @enum AdsppmBusBWOperationType - Nature of BW operation
 */
typedef enum
{
    AdsppmBusBWOperation_BW = 1,    //!< Normal BW request
    AdsppmBusBWOperation_RegAccess  //!< BW for register access
}AdsppmBusBWOperationType;

/**
 * @struct AdsppmBusBWRequestValueType - Data type specifying BW request parameters
 */
typedef struct
{
    AdsppmBusRouteType       busRoute;    //!< master/slave pair the BW is requested for
    AdsppmBusBWDataType      bwValue;     //!< BW value
    AdsppmBusBWOperationType bwOperation; //!< BW request reason
} AdsppmBusBWRequestValueType;

typedef enum{
    Adsppm_BwReqClass_Generic,
    Adsppm_BwReqClass_Compensated,
    Adsppm_BwReqClass_Force32Bbits = 0x7FFFFFFF  //!< forcing enum to be 32 bit
} AdsppmBwRequestClass;

/**
 * @struct AdsppmBwReqType - adsppm BW value array that contains BW settings for each corresponding mem ID
 */
typedef struct
{
    uint32                      numOfBw; //!< number of BW settings in the array, max is 16 per client
    AdsppmBusBWRequestValueType pBwArray[MAX_NUM_OF_ADSPPM_BW_ARRAY]; //!< the array of BW settings
    AdsppmBwRequestClass        requestClass;
} AdsppmBwReqType;

/**
 * @struct AdsppmBwSingleReqType - adsppm BW value array that contains BW settings for each corresponding mem ID
 */
typedef struct
{
    AdsppmBusBWRequestValueType adsppmBwReqDataVal; //!< BW settings
    AdsppmBwRequestClass        requestClass;
} AdsppmBwSingleReqType;

/******************************************************************************
 *                     Definitions for private info
 *****************************************************************************/

/**
 * @struct AdsppmInfoAggregatedBwType
 * @brief Structure to store aggregated BW ib/ab values
 */
typedef struct
{
    uint64 adspDdrBwAb;
    uint64 adspDdrBwIb;
    uint64 extAhbDdrBwAb;
    uint64 extAhbDdrBwIb;
    // Internal BW: ADSP<->LPASS
    uint64 intAhbBwAb;
    uint64 intAhbBwIb;
} AdsppmInfoAggregatedBwType;

/**
 * @struct AdsppmInfoAhbType 
 * @brief AHB information
 */
typedef struct
{
    uint32 ahbeFreqHz;                      //!< AHB-E bus frequency in Hz
} AdsppmInfoAhbType;

/**
 * @struct AdsppmInfoDcvsAdspDdrBwType 
 * @brief DCVS ADSP to DDR BW aggregation information
 */
typedef struct
{
    AdsppmBusBWDataIbAbType dcvsVote;
    AdsppmBusBWDataIbAbType clientsFloorVote;
    AdsppmBusBWDataIbAbType clientsFinalVote;
    AdsppmBusBWDataIbAbType finalVoteToNpa;
} AdsppmInfoDcvsAdspDdrBwType;

/**
 * @struct AdsppmInfoDcvsAdspClockType 
 * @brief DCVS ADSP clock aggregation information
 */
typedef struct
{
    uint32 dcvsVote;
    uint32 clientsFloorVote;
    uint32 clientsFinalVote;
    uint32 finalVoteToNpa;
} AdsppmInfoDcvsAdspClockType;

// Private information type
typedef enum
{
    ADSPPM_PRIVATE_INFO_AGGREGATED_BW = 0,
    ADSPPM_PRIVATE_INFO_AHB           = 1,
    ADSPPM_PRIVATE_INFO_ADSPDDR_BW    = 2,
    ADSPPM_PRIVATE_INFO_ADSPCLOCK     = 3,
} AdsppmInfoPrivateTypeType;

typedef struct
{
    // 'type' is an input from the caller and determines the type of private
    // query
    AdsppmInfoPrivateTypeType type;
    union
    {
        // Used when 'type' is ADSPPM_PRIVATE_INFO_AGGREGATED_BW
        AdsppmInfoAggregatedBwType aggregatedBw;
        // Used when 'type' is ADSPPM_PRIVATE_INFO_AHB
        AdsppmInfoAhbType ahb;
        // Used when 'type' is ADSPPM_PRIVATE_INFO_ADSPDDR_BW
        AdsppmInfoDcvsAdspDdrBwType adspDdrBw;
        // Used when 'type' is ADSPPM_PRIVATE_INFO_ADSPCLOCK
        AdsppmInfoDcvsAdspClockType adspClock;
    };
} AdsppmInfoPrivateType;

/******************************************************************************
 *                     MIPS definitions
 *****************************************************************************/
typedef enum
{
    MipsRequestOperation_None = 0,             //!< MipsRequestOperation_None
    MipsRequestOperation_MIPS = 1,             //!< MipsRequestOperation_MIPS
    MipsRequestOperation_BWandMIPS = 2,        //!< MipsRequestOperation_BWandMIPS
    MipsRequestOperation_MAX = 3,              //!< MipsRequestOperation_MAX
    MipsRequestOperation_EnumForce8Bit = 0x7f  //!< MipsRequestOperation_EnumForce8Bit
} AdsppmMipsOperationType;

typedef enum
{
    Adsppm_Q6ClockRequestUsageType_Mips,
    Adsppm_Q6ClockRequestUsageType_Mpps,
    Adsppm_Q6ClockRequestUsageType_Force32Bbits = 0x7FFFFFFF  //!< forcing enum to be 32 bit
} AdsppmQ6ClockRequestUsageType;

/**
 * @struct AdsppmMipsRequestType - MIPS request structure
 */
typedef struct
{
    uint32                  mipsTotal;      //!< Total MIPS value
    uint32                  mipsPerThread;  //!< MIPS value per HW thread
    AdsppmBusPortIdType     codeLocation;   //!< Location where code is executed from
    AdsppmMipsOperationType reqOperation;   //!< Which operations the MIPS request should include
} AdsppmMipsRequestType;

/**
 * @struct AdsppmMppsRequestType - MPPS request structure
 */
typedef struct
{
    uint32 mppsTotal;       //!< Total MPPS value
    uint32 adspFloorClock;  //!< ADSP Floor clock value in MHz
} AdsppmMppsRequestType;

/**
 * @struct AdsppmQ6ClockRequestInfoType - Q6 Clock Request Info structure
 */
typedef struct
{
    // Union to select either MIPS or MPPS request
    union
    {
        AdsppmMipsRequestType mipsRequestData;  //!< MIPS request Data
        AdsppmMppsRequestType mppsRequestData;  //!< MPPS request Data
    } AdsppmQ6ClockRequestType;
    AdsppmQ6ClockRequestUsageType  usageType;   //!< Usage Type - MIPS/MPPS

} AdsppmQ6ClockRequestInfoType;

/**
 * @struct AdsppmMipsDataType - structure for MIPS aggregate data
 */
typedef struct
{
    uint32 mipsTotal;      //!< Total MIPS value
    uint32 mipsPerThread;  //!< MIPS value per HW thread
} AdsppmMipsDataType;

/**
 * @struct AdsppmMIPSToBWAggregateType - structure for mipstoBW aggregate data
 */
typedef struct
{
    AdsppmMipsDataType          mipsData;  //!< MIPS value (total and per thread)
    AdsppmBusBWRequestValueType mipsToBW;  //!< [OUT] returns BW and bus route
} AdsppmMIPSToBWAggregateType;

/**
 * @struct AdsppmMIPSToClockAggregateType - data structure for MIPS to clock aggregate data
 */
typedef struct
{
    AdsppmMipsDataType mipsData;    //!< MIPS value (total and per thread)
    uint32             qDSP6Clock;  //!< [OUT]Q6 Clock in kHz
} AdsppmMIPSToClockAggregateType;

/******************************************************************************
 *                     Register Programming definitions
 *****************************************************************************/
/**
 * @enum AdsppmRegProgMatchType - Register programming match
 */
typedef enum
{
    Adsppm_RegProg_None,                        //!< None
    Adsppm_RegProg_Norm,                        //!< Normal
    Adsppm_RegProg_Fast,                        //!< Fast
    Adsppm_RegProg_Enum_Max,                    //!< Maximum count
    Adsppm_RegProg_EnumForce32Bit = 0x7fffffff  //!< forcing enum to be 8 bit
} AdsppmRegProgMatchType;

/******************************************************************************
 *                     Core Clock definitions
 *****************************************************************************/
/**
 * @enum AdsppmFreqMatchType - Frequency clock match
 */
typedef enum
{
    Adsppm_Freq_At_Least,          //!< At least
    Adsppm_Freq_At_Most,           //!< At most
    Adsppm_Freq_Closest,           //!< Closest
    Adsppm_Freq_Exact,             //!< Exact
    Adsppm_Freq_Max,               //!< Maximum count
    Adsppm_Freq_Force8Bits = 0x7F  //!< forcing enum to be 8 bit
} AdsppmFreqMatchType;

/**
 * @enum AdsppmInfoType - Adsppm info type definition
 */
typedef enum
{
    Adsppm_Info_Type_None,               //!< None
    Adsppm_Info_Type_Max_Value,          //!< get MAX value
    Adsppm_Info_Type_Min_Value,          //!< get Min Value
    Adsppm_Info_Type_Current_Value,      //!< Get current Value
    Adsppm_Info_Type_Max,                //!< Maximum count
    Adsppm_Info_Type_Force8Bits = 0x7F   //!< forcing enum to be 32 bit
} AdsppmInfoType;

/**
 * @struct AdsppmClkValType - Clock setting for one clock ID
 */
typedef struct
{
    AdsppmClkIdType      clkId;     //!< Core clock ID(s) defined by enums of AdsppmClkIdXyzType, Xyz is the core name
    uint32               clkFreqHz; //!< Unit of clock Freq is Hz
    AdsppmFreqMatchType  freqMatch; //!< Frequency match: at least, at most, closest, and exact
} AdsppmClkValType;

/**
 * @struct AdsppmInfoClkFreqType - get clock freq in Hz
 */
typedef struct
{
    uint32 clkId;        //!< Input: Core clock ID(s) defined by AdsppmClkIdType
    uint32 clkFreqHz;    //!< Output: Clock frequency in (Hz)
    uint32 forceMeasure; //!< Control to force the clock to be measured,
                         //!< clock freq may be read from a cached table if this is not set to 1
} AdsppmInfoClkFreqType;

/**
 * @struct AdsppmClkRequestType - Clock value array that contains clock frequencies for each corresponding clock ID
 */
typedef struct
{
    uint32            numOfClk; //!< number of clock settings in the array
    AdsppmClkValType  pClkArray[MAX_NUM_OF_ADSPPM_CLOCK_ARRAY]; //!< the array of clock settings
} AdsppmClkRequestType;

/******************************************************************************
 *                     Domain Clock definitions
 *****************************************************************************/
/**
 * @enum AdsppmClkDomainSrcIdLpassType - LPASS clock domain source ID
 */
typedef enum
{
    Adsppm_Clk_Domain_Src_Id_Lpass_None,                     //!< None
    Adsppm_Clk_Domain_Src_Id_Primary_PLL,
    Adsppm_Clk_Domain_Src_Id_Secondary_PLL,
    Adsppm_Clk_Domain_Src_Id_Ternery_PLL,
    Adsppm_Clk_Domain_Src_Id_Lpass_Max,                      //!< Maximum count
    Adsppm_Clk_Domain_Src_Id_Lpass_Force32bits = 0x7FFFFFFF  //!< Forces the enumeration to 32 bits
} AdsppmClkDomainSrcIdLpassType;

typedef union
{
    AdsppmClkDomainSrcIdLpassType clkDomainSrcIdLpass; //!< Clock domain source IDs of LPASS
} AdsppmClkDomainSrcIdType;

/**
 * @struct AdsppmClkDomainType - Clock domain setting for one clock ID
 */
typedef struct
{
    AdsppmClkIdType          clkId;        //!< Core clock ID
    uint32                   clkFreqHz;    //!< Unit of clock Freq is Hz
    AdsppmClkDomainSrcIdType clkDomainSrc; //!< Clock domain source ID
} AdsppmClkDomainType;

/**
 * @struct AdsppmClkDomainReqType - Clock domain array that contains clock domain settings for each corresponding clock ID
 */
typedef struct
{
    uint32              numOfClk; //!< Number of clock settings in the array
    AdsppmClkDomainType pClkDomainArray[MAX_NUM_OF_ADSPPM_CLOCK_ARRAY]; //!< Array of clock domain settings
} AdsppmClkDomainReqType;

/* //sn: TODO MCA Structure used here */
// AdsppmMIPSToBWAggregateType
/**
* structure for MIPS aggregation data
*/

typedef struct{
       uint32              numOfClients;
       uint32              mipsTotal;         /**< Total MIPS value*/
       uint32              mipsPerThread;     /**< MIPS value per HW thread*/
       uint32              cppFactor;         /**< CPP factor represented as total CPP*1000*/
       uint32              numOfPeriodicClients;               /**< Number of periodic clients*/
}AdsppmMIPSAggregateType;

/**
*@struct MIPS data structure for Aggregate
*/
typedef struct
{
	AdsppmMIPSAggregateType mipsAggregate;
    AdsppmMIPSToBWAggregateType  mipsToBWAggregateDataDdr;
}AdsppmMipsAggregateDataType;

// /******************************************************************************
 // *                     Memory Power definitions
 // *****************************************************************************/
// /**
 // * @enum AdsppmMemPowerStateType - Memory power states
 // */
// typedef enum
// {
    // Adsppm_Mem_Power_None,                     //!< None
    // Adsppm_Mem_Power_Off,                      //!< Memory off
    // Adsppm_Mem_Power_Retention,                //!< Memory in retention
    // Adsppm_Mem_Power_Active,                   //!< Memory in active state
    // Adsppm_Mem_Power_Max,                      //!< Maximum count
    // Adsppm_Mem_Power_Force32Bits = 0x7FFFFFFF  //!< forcing enum to be 32 bit
// } AdsppmMemPowerStateType;

/**
 * @enum AdsppmMemIdType - Memory Id definition for Memory Power state requests
 */
typedef enum
{
    Adsppm_Mem_None,                     //!< None
    Adsppm_Mem_Ocmem,                    //!< OCMEM
    Adsppm_Mem_Lpass_LPM,                //!< LPM
    Adsppm_Mem_Sram,                     //!< Sensors RAM
    Adsppm_Mem_Max,                      //!< Maximum count
    Adsppm_MEM_FORCE32BITS = 0x7FFFFFFF  //!< forcing enum to be 32 bit
} AdsppmMemIdType;

// /**
 // * @struct AdsppmMemPowerReqParamType - Memory power state request parameter structure
 // */
// typedef struct
// {
    // AdsppmMemIdType         memory;
    // AdsppmMemPowerStateType powerState;
// } AdsppmMemPowerReqParamType;

/******************************************************************************
 *                     Set Parameter API definitions
 *****************************************************************************/
/**
 * @enum AdsppmParameterIdType - defines parameter IDs for Adsppm_SetParameter()
 */
typedef enum
{
    Adsppm_Param_Id_None = 0,                //!< Invalid parameter
    Adsppm_Param_Id_Resource_Limit,          //!< Set resource limits (min and max). Works globally
    Adsppm_Param_Id_Client_Ocmem_Map,        //!< Set client's OCMEM allocation map
    Adsppm_Param_Id_Memory_Map,              //!< Set memory address map
    Adsppm_Param_Id_Enum_Max,                //!< Enumeration max
    Adsppm_Param_Id_Force32bit = 0x7fffffff
} AdsppmParameterIdType;

/**
 * @struct AdsppmParameterConfigType - data structure for parameter configuration API
 * Based on paramId pParamConfig will take different structure pointers:
 * paramId:
 *      Adsppm_Param_Id_Client_Ocmem_Map - pParamConfig points to AdsppmOcmemMapType structure
 */
typedef struct
{
    AdsppmParameterIdType paramId;      //!< Parameter ID
    void                 *pParamConfig; //!< Pointer to Parameter-specific structure
} AdsppmParameterConfigType;


/******************************************************************************
 *                     Utilities
 *****************************************************************************/
#define ADSPPM_ASSERT(x)  assert(x)

#ifndef MAX
#define MAX(a, b)  (((a)>(b))?(a):(b))
#endif

#ifndef MIN
#define MIN(a, b)  (((a)<(b))?(a):(b))
#endif

/******************************************************************************
 *                     Functions
 *****************************************************************************/
Adsppm_Status ADSPPM_Init(void);

/**
 * @fn ADSPPM_IsInitialized - Check if ADSPPM initialized successfully
 * @return - 1 ADSPPM init was successful
 *           0 ADSPPM init was unsuccessful
 */
uint32 ADSPPM_IsInitialized(void);

/**
 * @fn ADSPPM_EnterSleep
 * @brief This function is called by Sleep driver at the very late stage of entering ADSP PC.
 *
 * The function will ensure that AHB clock is set as low as possible while ADSP is in power collapse.
 * Limitation: the function should only be called from Sleep context after entering STM.
 */
void ADSPPM_EnterSleep(void);

/**
 * @fn ADSPPM_ExitSleep
 * @brief This function is called by SLeep driver early during exit from ADSP power collapse.
 *
 * The function will restore AHB clock to the frequency, which was set before entering power collapse.
 * Limitation: the function should only be called from Sleep context after entering STM.
 */
void ADSPPM_ExitSleep(void);


#endif /* ADSPPM_H_ */

