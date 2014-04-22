#ifndef _CSD_DEV_OEM_PLUGIN_INTF_H_
#define _CSD_DEV_OEM_PLUGIN_INTF_H_
/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ----------------------------------------------------------
06/04/13   unni     Added is codec present oem query
05/17/10   suns		  Initial draft
============================================================================*/

/**-------------------------------------------------------------------------*/
/*! \brief  payload for IOCTL command */
/**-------------------------------------------------------------------------*/

#include "mmdefs.h"

/*! \def */
#define CSD_DEV_OEM_STATUS_ACTED		  0
#define CSD_DEV_OEM_STATUS_NOT_ACTED	-1
#define CSD_DEV_OEM_ERR_BASE			    -1000

#define CSD_DEV_OEM_MBHC_GPIO_NONE  0xFFFFFFFF

/*! \fn int32_t csd_dev_oem_init(void)
    \brief This function inits the oem plugin
    \return int32
*/
int32_t csd_dev_oem_init(void);

/*! \fn int32_t csd_dev_oem_dinit(void)
    \brief This function deinits the oem plugin
    \return int32
*/
int32_t csd_dev_oem_dinit(void);

/*! \fn uint32_t csd_dev_oem_get_dev_count(void)
    \brief This function returns the total number of supported devices.
    \return uint32
*/
uint32_t csd_dev_oem_get_dev_count(void);


/*! \fn int32_t csd_dev_oem_msg (uint32_t cmd, void* params, uint32_t size)
    \brief This function sends msg to oem plugin library.
*/
int32_t csd_dev_oem_msg(uint32_t cmd, void* params, uint32_t size);

/**-------------------------------------------------------------------------*/
/*! \brief  OEM MSG Type */
/**-------------------------------------------------------------------------*/
/*! \def    CSD_DEV_OEM_MSG_INIT_CODEC
    \brief  Initilaize external codec driver by OEM. This command has no paylaod.

    \def    CSD_DEV_OEM_MSG_DINIT_CODEC
    \brief  De-initialize external codec driver controlled by OEM. This command has
            no payload.

    \def    CSD_DEV_OEM_MSG_INIT_PMIC
    \brief  Initialize codec related power rails before power up codec. This
		        command has no payload.

    \def    CSD_DEV_OEM_MSG_DINIT_PMIC
    \brief  De-initialize codec related power rails after power down codec. This
		        command has no payload.

	  \def    CSD_DEV_OEM_MSG_ENABLE_CLK
    \brief  Enable codec clock controlled by OEM based on device ID. This command has
            payload "struct csd_oem_dev_enable_entry"

    \def    CSD_DEV_OEM_MSG_DISABLE_CLK
    \brief  Disable codec clock controlled by OEM based on device ID. This command has
            payload "struct csd_oem_dev_disable_entry"

	  \def    CSD_DEV_OEM_MSG_ENABLE_CODEC
    \brief  Disable codec controlled by OEM based on device ID. This command has
            payload "struct csd_oem_dev_enable_entry"

	  \def    CSD_DEV_OEM_MSG_DISABLE_CODEC
    \brief  Disable codec controlled by OEM based on device ID. This command has
            payload "struct csd_oem_dev_disable_entry"

    \def    CSD_DEV_OEM_MSG_ENABLE_PMIC_ANALOG
    \brief  Enable connected analog power rails and associated pmic GPIOs for analog
            output/input audio pathes. This command is only used when OEM using
            Qualcomm recommended codecs and PMIC controllers. This command has payload
            "struct csd_oem_dev_pmic_ctrl_entry"

    \def    CSD_DEV_OEM_MSG_DISABLE_PMIC_ANALOG
    \brief  Disable connected analog power rails and associated pmic GPIOs for analog
            output/input audio pathes. This command is only used when OEM using
            Qualcomm recommended codecs and PMIC controllers. This command has payload
            "struct csd_oem_dev_pmic_ctrl_entry"

	  \def    CSD_DEV_OEM_MSG_ENABLE_TLMM
    \brief  Enable MSM TLMM GPIOs controlled by OEM based on device ID. This command has
            payload "struct csd_oem_dev_enable_entry"

    \def    CSD_DEV_OEM_MSG_DISABLE_TLMM
    \brief  Disable MSM TLMM GPIOs controlled by OEM based on device ID. This command has
            payload "struct csd_oem_dev_disable_entry"

	  \def    CSD_DEV_OEM_MSG_INIT_CODEC_PATH
    \brief  Initialize codec path controlled by OEM based on device ID. This command has
            payload "struct csd_oem_dev_enable_entry"

    \def    CSD_DEV_OEM_MSG_DEINIT_CODEC_PATH
    \brief  Initialize codec path controlled by OEM based on device ID. This command has
            payload "struct csd_oem_dev_disable_entry"

	  \def    CSD_DEV_OEM_MSG_CREATE_CODEC_PATH
    \brief  Create codec path controlled by OEM based on device ID. This command has
            payload "struct csd_oem_dev_enable_entry"

    \def    CSD_DEV_OEM_MSG_RELEASE_CODEC_PATH
    \brief  Release codec path controlled by OEM based on device ID. This command has
            payload "struct csd_oem_dev_disable_entry"
            
    \def    CSD_DEV_OEM_MSG_QUERY_CODEC_BUS_TYPE
    \brief  Query the codec connection interface type on the target. This command has
            payload "uint32_t bus_type". Supported values are: csd_oem_codec_bus_type_e.

    \def    CSD_DEV_OEM_MSG_GET_MBHC_PARAM
    \brief  Retrieve MBHC default settings and parameters by OEM based on param ID. 
            This command has payload "struct csd_oem_dev_mbhc_param"

    \def    CSD_DEV_OEM_MSG_QUERY_IS_WCD_CODEC_PRESENT
    \brief  Query the whether wcd codec is present on the target. This command has
            payload "bool_t enable_codec". 

*/

enum csd_dev_oem_msg_type {
  CSD_DEV_OEM_MSG_INIT_CODEC = 0,
  CSD_DEV_OEM_MSG_DINIT_CODEC,
  CSD_DEV_OEM_MSG_INIT_PMIC,
  CSD_DEV_OEM_MSG_DINIT_PMIC,
  CSD_DEV_OEM_MSG_ENABLE_CLK,
  CSD_DEV_OEM_MSG_DISABLE_CLK,
  CSD_DEV_OEM_MSG_ENABLE_CODEC,
  CSD_DEV_OEM_MSG_DISABLE_CODEC,
  CSD_DEV_OEM_MSG_ENABLE_PMIC_ANALOG,
  CSD_DEV_OEM_MSG_DISABLE_PMIC_ANALOG,
  CSD_DEV_OEM_MSG_ENABLE_TLMM,
  CSD_DEV_OEM_MSG_DISABLE_TLMM,
  CSD_DEV_OEM_MSG_INIT_CODEC_PATH,
  CSD_DEV_OEM_MSG_DEINIT_CODEC_PATH,
  CSD_DEV_OEM_MSG_CREATE_CODEC_PATH,
  CSD_DEV_OEM_MSG_RELEASE_CODEC_PATH,
  CSD_DEV_OEM_MSG_RESET_PMIC,
  CSD_DEV_OEM_MSG_QUERY_CODEC_BUS_TYPE,
  CSD_DEV_OEM_MSG_GET_MBHC_PARAM,
  CSD_DEV_OEM_MSG_QUERY_IS_WCD_CODEC_PRESENT,
  CSD_DEV_OEM_MSG_MAX_COUNT
};

/**-------------------------------------------------------------------------*/
/*! \brief  payload for OEM MSG command */
/**-------------------------------------------------------------------------*/

/* CSD_DEV_OEM_MSG_ENABLE_CLK */
/* CSD_DEV_OEM_MSG_ENABLE_CODEC */
struct csd_oem_dev_enable_entry {
  uint32_t dev_id;         /**< requested device ID */
	uint32_t freq;				   /**< requested device sample rate */
	uint32_t bps;				     /**< requested device bits per sample */
};

/* CSD_DEV_OEM_MSG_DISABLE_CLK */
/* CSD_DEV_OEM_MSG_DISABLE_CODEC */
struct csd_oem_dev_disable_entry {
  uint32_t dev_id;         /**< requested device ID */
};

/* CSD_DEV_OEM_MSG_ENABLE_PMIC_ANALOG  */
/* CSD_DEV_OEM_MSG_DISABLE_PMIC_ANALOG */
/* CSD_DEV_OEM_MSG_ENABLE_PMIC_DIGITAL */
/* CSD_DEV_OEM_MSG_DISABLE_PMIC_DIGITAL */
struct csd_oem_dev_pmic_ctrl_entry {
  uint32_t codec_path_id;		/**< requested codec path ID for a given device ID*/
};

/* CSD_DEV_OEM_MSG_QUERY_CODEC_BUS_TYPE */
typedef enum {
  CSD_OEM_CODEC_INTF_TYPE_INVALID = 0,
	CSD_OEM_CODEC_INTF_TYPE_I2S     = 1,       /**< Codec is connected through I2S interface on the target platform. */
  CSD_OEM_CODEC_INTF_TYPE_SLIMBUS = 2,       /**< Codec is connected through slimbus interface on the target platform. */
}csd_oem_codec_bus_type_e;

/* CSD_DEV_OEM_MSG_GET_MBHC_GPIO_PARAM */
enum csd_oem_dev_mbhc_gpio_owner_type
{
  CSD_OEM_DEV_MBHC_GPIO_OWNER_TYPE_PMIC = 0, /**< PMIC module will be the owner that provides the GPIO support */
  CSD_OEM_DEV_MBHC_GPIO_OWNER_TYPE_MSM,      /**< MSM module will be the owner that provides the GPIO support */
  CSD_OEM_DEV_MBHC_GPIO_OWNER_TYPE_MAX
};

enum csd_oem_dev_mbhc_param_type
{
  CSD_OEM_DEV_MBHC_PARAM_UNKNOWN = 0,
  /* Output GPIO parameter for audio video select */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_VID_SEL_OWNER,                    /**<  Owner of of GPIO, see csd_oem_dev_mbhc_gpio_hw_owner_type */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_VID_SEL_PIN_NUMBER,               /**<  GPIO number, set to CSD_DEV_OEM_MBHC_GPIO_NONE if GPIO is not used */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_VID_SEL_DEFAULT_VALUE,            /**<  Initial GPIO value, set to 0 for low and 1 for high */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_VID_SEL_OUT_BUFFER_DRIVER_STRENGTH, /**<  Output buffer driver strength */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_VID_SEL_OUT_BUFFER_CONFIG,        /**<  Output buffer configuration (for PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_VID_SEL_VOLTAGE_SOURCE,           /**<  Voltage source (for PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_VID_SEL_SOURCE_CONFIG,            /**<  Source configuration (PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_VID_SEL_CURRENT_SOURCE_PULLS,     /**<  Current source pulls */
  /* Output GPIO parameter for us euro select */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_US_EURO_SEL_OWNER,                /**<  Owner of of GPIO, see csd_oem_dev_mbhc_gpio_hw_owner_type */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_US_EURO_SEL_PIN_NUMBER,           /**<  GPIO number, set to CSD_DEV_OEM_MBHC_GPIO_NONE if GPIO is not used */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_US_EURO_SEL_DEFAULT_VALUE,        /**<  Initial GPIO value, set to 0 for low and 1 for high */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_US_EURO_SEL_OUT_BUFFER_DRIVER_STRENGTH, /**<  Output buffer driver strength */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_US_EURO_SEL_OUT_BUFFER_CONFIG,    /**<  Output buffer configuration (for PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_US_EURO_SEL_VOLTAGE_SOURCE,       /**<  Voltage source (for PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_US_EURO_SEL_SOURCE_CONFIG,        /**<  Source configuration (PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_AUDIO_US_EURO_SEL_CURRENT_SOURCE_PULLS, /**<  Current source pulls */
  /* Input GPIO parameter for plug detection */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PLUG_DET_OWNER,                         /**<  Owner of of GPIO, see csd_oem_dev_mbhc_gpio_hw_owner_type */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PLUG_DET_PIN_NUMBER,                    /**<  GPIO number, set to CSD_DEV_OEM_MBHC_GPIO_NONE if GPIO is not used */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PLUG_DET_DEFAULT_VALUE,                 /**<  Initial GPIO value, set to 0 for low and 1 for high */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PLUG_DET_OUT_BUFFER_DRIVER_STRENGTH,    /**<  Output buffer driver strength */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PLUG_DET_VOLTAGE_SOURCE,                /**<  Voltage source (for PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PLUG_DET_SOURCE_CONFIG,                 /**<  Source configuration (PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PLUG_DET_CURRENT_SOURCE_PULLS,          /**<  Current source pulls */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PLUG_DET_INTR_BLOCK_ID,			            /**<  Block ID for register GPIO interrupt (PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PLUG_DET_INTR_BIT_IDX,                  /**<  Block Index for register GPIO interrupt (PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PLUG_DET_INTR_TRIGGER,                  /**<  Trigger type for GPIO interrupt */
  /* Input GPIO parameter for PTT detection */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PTT_DET_OWNER,                          /**<  Owner of of GPIO, see csd_oem_dev_mbhc_gpio_hw_owner_type */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PTT_DET_PIN_NUMBER,                     /**<  GPIO number, set to CSD_DEV_OEM_MBHC_GPIO_NONE if GPIO is not used */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PTT_DET_DEFAULT_VALUE,                  /**<  Initial GPIO value, set to 0 for low and 1 for high */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PTT_DET_OUT_BUFFER_DRIVER_STRENGTH,     /**<  Output buffer driver strength */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PTT_DET_VOLTAGE_SOURCE,                 /**<  Voltage source (for PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PTT_DET_SOURCE_CONFIG,                  /**<  Source configuration (PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PTT_DET_CURRENT_SOURCE_PULLS,           /**<  Current source pulls */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PTT_DET_INTR_BLOCK_ID,                  /**<  Block ID for register GPIO interrupt (PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PTT_DET_INTR_BIT_IDX,                   /**<  Block Index for register GPIO interrupt (PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_PTT_DET_INTR_TRIGGER,                   /**<  Trigger type for GPIO interrupt */
  /* Input GPIO parameter for passive button detection */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_BUTTON_OWNER,                           /**<  Owner of of GPIO, see csd_oem_dev_mbhc_gpio_hw_owner_type */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_BUTTON_PIN_NUMBER,                      /**<  GPIO number, set to CSD_DEV_OEM_MBHC_GPIO_NONE if GPIO is not used */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_BUTTON_DEFAULT_VALUE,                   /**<  Initial GPIO value, set to 0 for low and 1 for high */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_BUTTON_OUT_BUFFER_DRIVER_STRENGTH,      /**<  Output buffer driver strength */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_BUTTON_VOLTAGE_SOURCE,                  /**<  Voltage source (for PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_BUTTON_SOURCE_CONFIG,                   /**<  Source configuration (PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_BUTTON_CURRENT_SOURCE_PULLS,            /**<  Current source pulls */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_BUTTON_INTR_BLOCK_ID,                   /**<  Block ID for register GPIO interrupt (PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_BUTTON_INTR_BIT_IDX,                    /**<  Block Index for register GPIO interrupt (PMIC only) */
  CSD_OEM_DEV_MBHC_GPIO_PARAM_BUTTON_INTR_TRIGGER,                    /**<  Trigger type for GPIO interrupt */
  CSD_OEM_DEV_MBHC_ADVANCED_PARAM_NORMALLY_OPEN_CLOSED_CIRCUIT,       /**<  Normally open or closed circuit */
  CSD_OEM_DEV_MBHC_ADVANCED_PARAM_EXTERNAL_BYPASS_CAP_MODE,           /**<  External bypass cap mode */
  CSD_OEM_DEV_MBHC_ADVANCED_PARAM_ANC_HEADSET_DETECTION_MODE,         /**<  ANC headset detection mode */
  CSD_OEM_DEV_MBHC_ADVANCED_PARAM_SUPPORT_MIC_AFTER_PLUG_REMOVAL,     /**<  Support MIC afer plug removal */
  CSD_OEM_DEV_MBHC_PARAM_MAX
};

typedef enum csd_oem_dev_mbhc_param_type csd_oem_dev_mbhc_param_type_e;

typedef struct csd_oem_dev_mbhc_param {
  uint32_t* data_ptr;   /**< output data buffer pointer */
  uint32_t data_size;   /**< output data buffer size */
} csd_oem_dev_mbhc_param_t ;

#endif /* _CSD_DEV_OEM_PLUGIN_INTF_H_ */
