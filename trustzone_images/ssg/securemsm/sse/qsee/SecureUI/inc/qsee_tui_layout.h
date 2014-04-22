/** @file qsee_tui_layout.h
*/
/********************************************************************
---------------------------------------------------------------------
 Copyright (c) 2015, 2019 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
----------------------------------------------------------------------
*********************************************************************/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/19   sw      (Tech Pubs) Edited Doxygen markup for Rev. N.
12/10/15    kl     (Tech Pubs) Comment updates for 80-NJ546-1 Rev. J.
===========================================================================*/


#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#include "qsee_tui_dialogs.h"

/*
   Do not use, for internal usage only.
*/
LayoutPage_t* create_layout(LayoutFlavor_t layout_flavor);


/** @ingroup customization_tool
   Receives a layout by name.

   @note1hang  If the generated layout file was compiled together with the
               calling application, this function returns any layout that was
               created with the Customization tool.

   @param[in]  layout_name  Layout name; must be unique per application.

   @return
   A pointer to the layout structure.

   @dependencies
   None.
*/
LayoutPage_t* get_layout_by_name(const char * layout_name);


#endif
