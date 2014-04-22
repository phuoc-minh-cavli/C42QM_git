#ifndef QSEE_SEC_UI_H_
#define QSEE_SEC_UI_H_

/** @file SecureUI.h
 * @brief
 * This file contains the definitions of the constants, data structures
 * and interfaces to the QSEE Secure User Interface service
 */

/*===========================================================================
 Copyright (c) 2013-2016, 2018,2019 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/02/18   sw      (Tech Pubs) Edited Doxygen markup for rev. N.
09/08/16   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
02/02/16   sn      Added app buffer validation function.
12/24/15   gs      Remove buffer cache state validation function
12/10/15   kl      (Tech Pubs) Comment pdates for 80-NJ546-1 Rev. J.
07/29/15   ng      Add buffer cache state validation function
07/08/14   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
04/29/13   sz      Initial Version.

===========================================================================*/
#include <stdbool.h>
#include "secure_ui_defs.h"

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* This enum is taken from /jb_mr1/system/core/include/system/graphics.h.
   It includes all the supported pixel formats.
*/

/** @addtogroup qsee_core_layer
@{ */

/** @anonenum{Pixel Formats}  @xreflabel{hdr:pixelFormats}
  Pixel formats.

  @note1hang Currently, only RGBA_8888 is supported. Support for other formats
  might be added in future releases.
 */
enum {
    HAL_PIXEL_FORMAT_RGBA_8888              = 1,
    HAL_PIXEL_FORMAT_RGBX_8888              = 2,
    HAL_PIXEL_FORMAT_RGB_888                = 3,
    HAL_PIXEL_FORMAT_RGB_565                = 4,
    HAL_PIXEL_FORMAT_BGRA_8888              = 5,
    HAL_PIXEL_FORMAT_RGBA_5551              = 6,
    HAL_PIXEL_FORMAT_RGBA_4444              = 7,
    HAL_PIXEL_FORMAT_YV12                   = 0x32315659,
    HAL_PIXEL_FORMAT_RAW_SENSOR             = 0x20,
    HAL_PIXEL_FORMAT_BLOB                   = 0x21,
    HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED = 0x22,
    HAL_PIXEL_FORMAT_YCbCr_422_SP           = 0x10,
    HAL_PIXEL_FORMAT_YCrCb_420_SP           = 0x11,
    HAL_PIXEL_FORMAT_YCbCr_422_I            = 0x14,  /**< @newpage */
};

/** @cond */
 /* Type of input to copy to secure the display buffer.

    Deprecated -- removed from the documentation per shayn.
 */
 typedef enum {
   SEC_UI_FD_FS      = 0,  /* File descriptor of a file in the regular file system. */
   SEC_UI_FD_SFS     = 1,  /* File descriptor of a file in the secure file system. */
   SEC_UI_BUFFER     = 2,  /* Pointer to a RAM buffer. */
 } input_type;

 /* Defines a data input to copy to the secure display buffer.

    Deprecated -- removed from the documentation per shayn.
 */
 struct input_to_copy {
	 input_type type;  /* Input source; see #input_type. */
     /* Union between a uint32_t u32FD, used in the case of FD_FS or FD_SFS,
         and uint8_t *pu8Src used in the case of a buffer input. */
	 union {
		 uint32_t u32Fd;   /* Used in the case of FD_FS or FS_SFS. */
		 uint8_t *pu8Src;  /* Used in the case of a buffer input. */
	 } input;
 };

 /** @endcond */


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  Returns the display properties.

  @param[out]  u32Height  Pointer to the display height in pixels.
  @param[out]  u32Width   Pointer to the display width in pixels.

  @return
  #SEC_UI_SUCCESS \n
  #SEC_UI_LISTENER_ERROR \n
  #SEC_UI_ABORTED
  @par
  @note1hang This function can return #SEC_UI_SUCCESS even if the secure
             UI was aborted in instances other than the first call.

  @dependencies
  None. @newpage
*/
sec_ui_err_t sec_ui_get_disp_properties(uint32_t *u32Height, uint32_t *u32Width);


/**
  Starts the secure display in Full Screen mode.

  @param[in]  colorFormat    Pixel format. for supported formats, see Section
                             @xref{hdr:pixelFormats}.
  @param[in]  initScreen     Pointer to the buffer containing the initial
                             screen. If NULL, a default black screen is used.
  @param[in]  initScreenLen  Length of initScreenBuf.
  @param[out] buffer         Double pointer to the next buffer to be displayed.
  @param[out] u32Stride      Pointer to the stride of the associated window.
  @param[out] u32PixelWidth  Pointer to the ixel width of the associated window.
  @param[out] u32Rotation    Pointer to the current rotation of the Android
                             screen from the natural orientation.

  @detdesc
  This function triggers the allocation and locking of secure buffers and
  activates the secure display. When the function returns, a (secure) black
  frame is guaranteed to be displayed on the screen.
  @par
  This function returns a pointer to the secure buffer into which the
  application can draw its content. The buffer corresponds to a full screen
  window now active on the HLOS. To operate on this buffer, the associated
  stride is also returned.
  @par
  Stride is the hardware buffer width, and it can be larger than the screen
  width. If the stride is larger than the width, the pixels beyond the screen
  width are displayed. Thus, you should know both screen width and buffer
  stride, know to break the pixel lines in the screen width, and continue
  filling the next line where the stride ends.

  @return
  #SEC_UI_SUCCESS \n
  #SEC_UI_LISTENER_ERROR \n
  #SEC_UI_ALREADY_RUNNING \n
  #SEC_UI_ABORTED \n
  #SEC_UI_NON_CONTIGUOUS_BUFFER

  @dependencies
  None. @newpage
*/
sec_ui_err_t sec_ui_start_disp_fullscreen(
  uint32_t colorFormat,
  uint8_t  *initScreen,
  uint32_t  initScreenLen,
  uint8_t **buffer,
  uint32_t *u32Stride,
  uint32_t *u32PixelWidth,
  uint32_t *u32Rotation);

/**
  Stops a secure display.

  This function triggers tear down of the secure display service. Tear down
  includes freeing buffers, unlocking, enabling overlays, and so on.

  @note1hang This function never returns SEC_UI_ABORTED because the function
  closes the display session.

  @return
  #SEC_UI_SUCCESS \n
  #SEC_UI_LISTENER_ERROR \n
  #SEC_UI_IS_NOT_RUNNING

  @dependencies
  None.
*/
sec_ui_err_t sec_ui_stop_disp(void);

/**
  Authenticates the secure display on the screen.

  This external function authenticates the current frame being displayed on
  the screen. If there is a failure (MISR match is not found), the secure UI
  session is aborted.

  The authentication can be disabled within the library by setting the
  secdisp_config_LM_idx property to 0 in the Secure Display devcfg
  configuration file, secdisp_oem_config.xml 
  (see Section @xref{sec:FrameAuthEnablemt}).

  @return
  #SEC_UI_SUCCESS \n
  #SEC_UI_FRAME_AUTHENTICATION_FAILED \n

  @dependencies
  None. @newpage
*/
sec_ui_err_t sec_ui_authenticate_frame(void);


/**
  Gets the TA state based on the expired time and TIMEOUT time set by the
  SecureUI libary.

  @return
  #SEC_UI_SUCCESS if the TA was scheduled back again within the TIMEOUT time. \n
  #SEC_UI_ABORTED if the expired time exceeds the TIMEOUT time. \n

  @dependencies
  None.
*/
sec_ui_err_t sec_ui_get_ta_state(void);


/**
  Sets the timeout value for the application.

  Application execution must come back to the TA within this timeout period,
  failing which the session will be aborted.

  @param[in] timeout_val   Time in milliseconds: -1 for no timeout; the default
                           value is 48 ms.

  @return
  #SEC_UI_SUCCESS \n

  @dependencies
  None. @newpage
*/
sec_ui_err_t sec_ui_set_ta_timeout(long long timeout_val);


/**
  Shows a secure buffer.

  @param[in]  bDuplicate  Duplicate the current active buffer being submitted
                          to the screen onto the next active buffer, which
                          will be returned.
  @param[out] buffer      Double pointer to the next buffer.

  @detdesc
  This function sends the current secure buffer to be displayed. Optionally,
  the previous active buffer can be duplicated on the new active buffer before
  this is returned to the calling application.
  @par
  An error code is returned if the secure display is not enabled or the buffer
  was unlocked.

  @return
  #SEC_UI_SUCCESS -- Success. \n
  #SEC_UI_NON_SECURE_BUFFER \n
  #SEC_UI_IS_NOT_RUNNING \n
  #SEC_UI_LISTENER_ERROR \n
  #SEC_UI_NON_CONTIGUOUS_BUFFER \n
  #SEC_UI_QSEE_SECURE_BUFFER_ERROR \n
  #SEC_UI_ABORTED

  @dependencies
  None. @newpage
*/
sec_ui_err_t sec_ui_show_buf(bool bDuplicate, uint8_t **buffer);


/**
  Copies data from memory or from an open file to the secure display buffer.
  If the specified length exceeds the row length, the function continues
  copying to the following rows, starting from offset 0.

  @param[in] input   Source buffer for copying the FS/SFS file descriptor to
                     the display buffer.
  @param[in] u32X    X-position (horizontal axis) in the display buffer to
                     which to copy the data t
  @param[in] u32Y    Y-position (vertical axis) in the display buffer to which
                     to copy the data.
  @param[in] u32Len  Number of bytes to copy.

  @return
  #SEC_UI_SUCCESS \n
  #SEC_UI_IS_NOT_RUNNING \n
  #SEC_UI_NON_SECURE_BUFFER \n
  #SEC_UI_INVALID_INPUT \n
  #SEC_UI_QSEE_SECURE_BUFFER_ERROR \n
  #SEC_UI_ABORTED  \n
  #SEC_UI_COPY_TO_BUFFER_FAILED

  @dependencies
  None.
*/
sec_ui_err_t sec_ui_copy_to_buffer(struct input_to_copy input, uint32_t u32X, uint32_t u32Y, uint32_t u32Len);

/**
  Releases both Secure Touch and Secure UI on the HLOS side.

  This function is called in an emergency application shutdown; for example,
  the corresponding application in the HLOS was killed or has crashed.
  The function provides a way out of the Secure Application if its unsecure
  counterpart has died. <i>This function must not be used in any other case</i>.

  This function does not, on its own, free any resource in the QSEE. It
  merely notifies the Secure UI Listener that resources have been freed
  in the QSEE and that they can be taken over again by the HLOS.

  @return
  #SEC_UI_SUCCESS \n
  #SEC_UI_LISTENER_ERROR

  @dependencies
  None. @newpage
*/
sec_ui_err_t sec_ui_hlos_release(void);

/**
   Checks if the buffers or sessions have changed while in HLOS.

   This function is called internally every time a touch event is received.
   It intercepts possible changes done in the HLOS that can compromise the
   security of the data used for secure display.

   @return
   TRUE if the session is still valid. \n
   FALSE otherwise.

   @dependencies
   None.
*/
bool session_is_unchanged(void);

/**
   Checks if the application buffer is still properly tagged (if the buffer is
   allocated).

   @return
   TRUE if the application buffer is still tagged. \n
   FALSE otherwise.

   @dependencies
   None. @newpage
*/
bool is_app_buffer_unchanged(void);

/**
   Gets a secure buffer that is tagged as an application buffer. The buffer is
   allocated in the HLOS.

   @param[in]  len  Length of the requested application buffer.
   @param[out] buf  Double pointer to the application buffer.

   @return
   #SEC_UI_SUCCESS \n
   #SEC_UI_INVALID_INPUT \n
   #SEC_UI_LISTENER_ERROR \n
   #SEC_UI_GENERAL_ERROR

   @dependencies
   None.
*/
sec_ui_err_t sec_ui_alloc_app_buffer(
        uint32_t    len,
        uint8_t   **buf);

/**
   requests the HLOS to free the ION memory that was allocated by
   sec_ui_alloc_app_buffer().

   @return
   #SEC_UI_SUCCESS \n
   #SEC_UI_LISTENER_ERROR \n
   #SEC_UI_GENERAL_ERROR

   @dependencies
   None.
*/
sec_ui_err_t sec_ui_free_app_buffer(void);
/** @} */ /* end_addtogroup qsee_core_layer */

/**
  Gets the current state of SecureUI Session.

  @return
  #SEC_UI_STATE_NOT_ACTIVE = 0,
  #SEC_UI_STATE_ACTIVE = 1 ,
  #SEC_UI_STATE_ABORTED = 2,

  @dependencies
  None.
*/
sec_ui_state_type sec_ui_get_lib_state(void);


#endif /* QSEE_SEC_UI_H_ */
