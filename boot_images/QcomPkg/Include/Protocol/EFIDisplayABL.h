/** 
  @file  EFIDisplayABL.h
  @brief UEFI display apps boot loader definitions.
*/
/*=============================================================================
    Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __EFIDISPLAYABL_H__
#define __EFIDISPLAYABL_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_displayABL_constants  
@{ */
/**
  Protocol version.
*/
#define DISPLAY_ABL_REVISION 0x0000000000010000

/** @} */ /* end_addtogroup efi_displayABL_constants */

/*  Protocol GUID definition */
/** @ingroup efi_displayABL_protocol */

// {c0dd69ac-76ba-11e6-ab24-1fc7f5575f19}
#define EFI_DISPLAYABL_PROTOCOL_GUID \
    {0xc0dd69ac, 0x76ba, 0x11e6, {0xab, 0x24, 0x1f, 0xc7, 0xf5, 0x57, 0x5f, 0x19}}


/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the Display ABL Protocol GUID.
*/
extern EFI_GUID gQcomDisplayABLProtocolGuid;


/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/**
  Protocol declaration.
*/
typedef struct _EFI_QCOM_DISPLAY_ABL_PROTOCOL  EFI_QCOM_DISPLAY_ABL_PROTOCOL;
/** @endcond */

/** @addtogroup efi_displayABL_data_types 
@{ */
/** 
  Display ABL property types
*/
typedef enum
{
  EFI_DISPLAY_ABL_DEVICE_TREE_ADDR = 0x0, /**< Device tree base address: (Type: VOID*) */
  EFI_DISPLAY_ABL_PANEL_OVERRIDE,         /**< Panel override string (Type: CHAR8*) */
  EFI_DISPLAY_ABL_SUPPORTED_PANELS,       /**< Newline separated list of supported panels (Type: CHAR16*) */
} EFI_DISPLAY_ABL_PROPERTY_TYPE;


/** 
  Parameter structure for EFI_DISPLAY_ABL_PANEL_LIST
*/
typedef struct
{
  UINT32   PanelCount;                  /**< Number of panels in PanelList */
  UINT32   Length;                      /**< String length of PanelList */
  CHAR8   *PanelList;                   /**< Newline separated list of supported panels */
} EFI_DISPLAY_ABL_PANEL_LIST_PARAM;


/** @} */ /* end_addtogroup efi_displayABL_data_types */


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_DISPLAY_ABL_SET_PROPERTY */ 
/** @ingroup efi_displayABL_control
  @par Summary
  Set a property on Display UEFI

  @param[in] Type                Property type
  @param[in] Data                Pointer to data structure
  @param[in] DataSize            Size of data in bytes
  
  @return
  EFI_SUCCESS              : Function returned successfully.
  EFI_INVALID_PARAMETER    : Parameter passed is incorrect.  
  EFI_UNSUPPORTED          : Parameter passed is not supported. 
  EFI_DEVICE_ERROR         : Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_DISPLAY_ABL_SET_PROPERTY)(
  IN EFI_DISPLAY_ABL_PROPERTY_TYPE   Type,
  IN VOID                           *Data,
  IN UINT32                          DataSize
);

/* EFI_DISPLAY_ABL_GET_PROPERTY */ 
/** @ingroup efi_displayABL_control
  @par Summary
  Gets the value of a property from Display UEFI.
  If Data is NULL or DataSize is smaller then function 
  returns EFI_BUFFER_TOO_SMALL with DataSize set to 
  the required buffer size

  @param[in] Type                Property type
  @param[out] Data               Pointer to data structure
  @param[in/out] DataSize        [in] Max size of data 
                                 [out] Actual size of data 
  
  @return
  EFI_SUCCESS              : Function returned successfully.
  EFI_INVALID_PARAMETER    : Parameter passed is incorrect.  
  EFI_UNSUPPORTED          : Parameter passed is not supported. 
  EFI_DEVICE_ERROR         : Physical device reported an error.
  EFI_BUFFER_TOO_SMALL     : Buffer to small to copy data into
*/
typedef
EFI_STATUS (EFIAPI *EFI_DISPLAY_ABL_GET_PROPERTY)(
  IN     EFI_DISPLAY_ABL_PROPERTY_TYPE   Type,
  OUT    VOID                           *Data,
  IN OUT UINT32                         *DataSize
);


/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_displayABL_protocol
  @par Summary
  Qualcomm Display ABL Protocol interface.

  @par Parameters
  @inputprotoparams{} 
*/
struct _EFI_QCOM_DISPLAY_ABL_PROTOCOL
{
  UINT64                                           Revision;
  EFI_DISPLAY_ABL_SET_PROPERTY                     DisplayABLSetProperty; 
  EFI_DISPLAY_ABL_GET_PROPERTY                     DisplayABLGetProperty; 
};

#endif  /* __EFIDISPLAYABL_H__ */
