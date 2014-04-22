/**
@file wcd_gen_events.c
@brief 
Generated event functions for WCD codec elements.
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
   All rights reserved.
   Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/wcd_gen_events.c#5 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/23/13   KNM    Added ANC FB Tune1 & Tune2 routing in Tx path
05/31/13   KNM    Adopted common code logging message related changes.
03/01/13   KNM    Initial revision.
===============================================================================*/

#include "wcd_gen_events.h"
#include "wcd_gen_id_defines.h"
#include "wcd_custom_functions.h"
#include "bus_manager.h"
#include "wcd_intf.h"

#ifdef SPL_LOG_SUPPORT
	#include "wcd_gen_events.tmh"
#endif

wcd_result wcd_SBRX_1_SET_BIT_WIDTH_12BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_1 SET_BIT_WIDTH_12BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_1 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0x3, 0, 0x3); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_1 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_1_SET_BIT_WIDTH_16BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_1 SET_BIT_WIDTH_16BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_1 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0x3, 0, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_1 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_1_SET_BIT_WIDTH_20BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_1 SET_BIT_WIDTH_20BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_1 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0x3, 0, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_1 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_1_SET_BIT_WIDTH_24BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_1 SET_BIT_WIDTH_24BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_1 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0x3, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_1 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_2_SET_BIT_WIDTH_12BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_2 SET_BIT_WIDTH_12BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_2 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0xC, 2, 0x3); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_2 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_2_SET_BIT_WIDTH_16BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_2 SET_BIT_WIDTH_16BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_2 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0xC, 2, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_2 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_2_SET_BIT_WIDTH_20BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_2 SET_BIT_WIDTH_20BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_2 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0xC, 2, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_2 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_2_SET_BIT_WIDTH_24BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_2 SET_BIT_WIDTH_24BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_2 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0xC, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_2 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_3_SET_BIT_WIDTH_12BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_3 SET_BIT_WIDTH_12BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_3 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0x30, 0x4, 0x3); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_3 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_3_SET_BIT_WIDTH_16BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_3 SET_BIT_WIDTH_16BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_3 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0x30, 4, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_3 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_3_SET_BIT_WIDTH_20BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_3 SET_BIT_WIDTH_20BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_3 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0x30, 4, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_3 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_3_SET_BIT_WIDTH_24BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_3 SET_BIT_WIDTH_24BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_3 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0x30, 4, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_3 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_4_SET_BIT_WIDTH_12BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_4 SET_BIT_WIDTH_12BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_4 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0xC0, 6, 0x3); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_4 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_4_SET_BIT_WIDTH_16BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_4 SET_BIT_WIDTH_16BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_4 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0xC0, 6, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_4 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_4_SET_BIT_WIDTH_20BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_4 SET_BIT_WIDTH_20BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_4 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0xC0, 6, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_4 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_4_SET_BIT_WIDTH_24BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_4 SET_BIT_WIDTH_24BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_4 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AE, 0xC0, 6, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_4 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_5_SET_BIT_WIDTH_12BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_5 SET_BIT_WIDTH_12BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_5 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AF, 0x3, 0, 0x3); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_5 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_5_SET_BIT_WIDTH_16BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_5 SET_BIT_WIDTH_16BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_5 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AF, 0x3, 0, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_5 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_5_SET_BIT_WIDTH_20BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_5 SET_BIT_WIDTH_20BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_5 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AF, 0x3, 0, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_5 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBRX_5_SET_BIT_WIDTH_24BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBRX_5 SET_BIT_WIDTH_24BIT.> \n"); 
	 if (p_element->id != WCD_SBRX_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_5 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3AF, 0x3, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBRX_5 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RDAC_1_MUX_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RDAC_1_MUX CONNECT.> \n"); 
	 if (p_element->id != WCD_RDAC_1_MUX_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_1_MUX CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RDAC_1_MUX CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_BIAS_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x381, 0x10, 4, 0x0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_BIAS_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x381, 0x10, 4, 0x1); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_1_MUX CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RDAC_1_MUX_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RDAC_1_MUX DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RDAC_1_MUX_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_1_MUX DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x381, 0x10, 4, 0x0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_1_MUX DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_EAROUT_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event EAROUT ENABLE.> \n"); 
	 if (p_element->id != WCD_EAROUT_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_EAROUT_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_EAROUT_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event EAROUT DISABLE.> \n"); 
	 if (p_element->id != WCD_EAROUT_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_EAROUT_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_EAROUT_SET_GAIN_0DB(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event EAROUT SET_GAIN_0DB.> \n"); 
	 if (p_element->id != WCD_EAROUT_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT SET_GAIN_0DB. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x1BD, 0xE0, 5, 0x4); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT SET_GAIN_0DB. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_EAROUT_SET_GAIN_1P5DB(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event EAROUT SET_GAIN_1P5DB.> \n"); 
	 if (p_element->id != WCD_EAROUT_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT SET_GAIN_1P5DB. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x1BD, 0xE0, 5, 0x3); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT SET_GAIN_1P5DB. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_EAROUT_SET_GAIN_3DB(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event EAROUT SET_GAIN_3DB.> \n"); 
	 if (p_element->id != WCD_EAROUT_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT SET_GAIN_3DB. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x1BD, 0xE0, 5, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT SET_GAIN_3DB. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_EAROUT_SET_GAIN_4P5DB(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event EAROUT SET_GAIN_4P5DB.> \n"); 
	 if (p_element->id != WCD_EAROUT_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT SET_GAIN_4P5DB. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x1BD, 0xE0, 5, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT SET_GAIN_4P5DB. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_EAROUT_SET_GAIN_6DB(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event EAROUT SET_GAIN_6DB.> \n"); 
	 if (p_element->id != WCD_EAROUT_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT SET_GAIN_6DB. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x1BD, 0xE0, 5, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event EAROUT SET_GAIN_6DB. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_MAIN_MUX_1_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_MAIN_MUX_1 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_1_MAIN_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_1_MAIN_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x380, 0xF, 0, 5); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x380, 0xF, 0, 6); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x380, 0xF, 0, 7); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x380, 0xF, 0, 8); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_5_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x380, 0xF, 0, 9); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MUX_1 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_MAIN_MUX_1_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_MAIN_MUX_1 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_1_MAIN_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x380, 0xF, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_MAIN_MUX_2_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_MAIN_MUX_2 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_1_MAIN_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_1_MAIN_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x380, 0xF0, 4, 5); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x380, 0xF0, 4, 6); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x380, 0xF0, 4, 7); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x380, 0xF0, 4, 8); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_5_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x380, 0xF0, 4, 9); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MUX_2 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_MAIN_MUX_2_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_MAIN_MUX_2 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_1_MAIN_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x380, 0xF0, 4, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_MAIN_MUX_3_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_MAIN_MUX_3 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_1_MAIN_MUX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MUX_3 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_1_MAIN_MUX_3 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x381, 0xF, 0, 5); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x381, 0xF, 0, 6); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x381, 0xF, 0, 7); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x381, 0xF, 0, 8); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_5_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x381, 0xF, 0, 9); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MUX_3 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_MAIN_MUX_3_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_MAIN_MUX_3 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_1_MAIN_MUX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MUX_3 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x381, 0xF, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MUX_3 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_MAIN_MIXER_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_MAIN_MIXER CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_1_MAIN_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_1_MAIN_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_1_MAIN_MUX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_1_MAIN_MUX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_1_MAIN_MUX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MIXER CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_MAIN_MIXER_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_MAIN_MIXER DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_1_MAIN_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MIXER DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_MAIN_MIXER DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_ST_MIXER_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_ST_MIXER CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_1_ST_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_ST_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_1_ST_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_1_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_1_ST_MUX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_1_ST_MUX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_ST_MIXER CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_ST_MIXER_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_ST_MIXER DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_1_ST_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_ST_MIXER DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_ST_MIXER DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_ST_MUX_1_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_ST_MUX_1 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_1_ST_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_ST_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_1_ST_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_ST_MUX_1 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_ST_MUX_1_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_ST_MUX_1 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_1_ST_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_ST_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_ST_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_ST_MUX_2_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_ST_MUX_2 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_1_ST_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_ST_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_1_ST_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_ST_MUX_2 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_ST_MUX_2_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_ST_MUX_2 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_1_ST_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_ST_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_ST_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_HPF_BYPASS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_HPF BYPASS.> \n"); 
	 if (p_element->id != WCD_RX_1_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_HPF BYPASS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2B4, 0x4, 2, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_HPF BYPASS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_HPF_SET_CUTOFF_FREQ_4HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_HPF SET_CUTOFF_FREQ_4HZ.> \n"); 
	 if (p_element->id != WCD_RX_1_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_HPF SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2B4, 0x4, 2, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2B3, 0x3, 0, 0x0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_HPF SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_HPF_SET_CUTOFF_FREQ_75HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_HPF SET_CUTOFF_FREQ_75HZ.> \n"); 
	 if (p_element->id != WCD_RX_1_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_HPF SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2B4, 0x4, 2, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2B3, 0x3, 0, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_HPF SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_HPF_SET_CUTOFF_FREQ_150HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_HPF SET_CUTOFF_FREQ_150HZ.> \n"); 
	 if (p_element->id != WCD_RX_1_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_HPF SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2B4, 0x4, 2, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2B3, 0x3, 0, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_HPF SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_PATH_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_PATH ENABLE.> \n"); 
	 if (p_element->id != WCD_RX_1_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_PATH_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_PATH DISABLE.> \n"); 
	 if (p_element->id != WCD_RX_1_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_PATH_MUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_PATH MUTE.> \n"); 
	 if (p_element->id != WCD_RX_1_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH MUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2B5, 0x1, 0, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH MUTE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_PATH_UNMUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_PATH UNMUTE.> \n"); 
	 if (p_element->id != WCD_RX_1_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH UNMUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2B5, 1, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH UNMUTE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_PATH_RESET(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_PATH RESET.> \n"); 
	 if (p_element->id != WCD_RX_1_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_RESET(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH RESET. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_PATH_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_PATH SET_GAIN.> \n"); 
	 if (p_element->id != WCD_RX_1_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_SET_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_PATH SET_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_COMPANDER_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_COMPANDER ENABLE.> \n"); 
	 if (p_element->id != WCD_RX_1_COMPANDER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_COMPANDER ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_COMPANDER_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_COMPANDER ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_COMPANDER ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_1_COMPANDER_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_1_COMPANDER DISABLE.> \n"); 
	 if (p_element->id != WCD_RX_1_COMPANDER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_COMPANDER DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_COMPANDER_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_COMPANDER DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_1_COMPANDER DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_CLASS_CONTROLLER_1_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event CLASS_CONTROLLER_1 ENABLE.> \n"); 
	 if (p_element->id != WCD_CLASS_CONTROLLER_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_1 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_CLASS_CONTROLLER_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_1 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_1 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_CLASS_CONTROLLER_1_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event CLASS_CONTROLLER_1 DISABLE.> \n"); 
	 if (p_element->id != WCD_CLASS_CONTROLLER_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_1 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_CLASS_CONTROLLER_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_1 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_1 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_BIAS_1_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_BIAS_1 ENABLE.> \n"); 
	 if (p_element->id != WCD_RX_BIAS_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_1 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_BIAS_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_1 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_1 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_BIAS_1_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_BIAS_1 DISABLE.> \n"); 
	 if (p_element->id != WCD_RX_BIAS_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_1 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_BIAS_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_1 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_1 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_HPHL_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event HPHL SET_GAIN.> \n"); 
	 if (p_element->id != WCD_HPHL_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHL SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_HPH_SET_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHL SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHL SET_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_HPHL_PRECHARGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event HPHL PRECHARGE.> \n"); 
	 if (p_element->id != WCD_HPHL_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHL PRECHARGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_HPH_PRECHARGE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHL PRECHARGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHL PRECHARGE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_HPHL_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event HPHL ENABLE.> \n"); 
	 if (p_element->id != WCD_HPHL_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHL ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_HPH_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHL ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHL ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_HPHL_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event HPHL DISABLE.> \n"); 
	 if (p_element->id != WCD_HPHL_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHL DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_HPH_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHL DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHL DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_MAIN_MUX_1_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_MAIN_MUX_1 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_2_MAIN_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_2_MAIN_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x383, 0xF, 0, 5); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x383, 0xF, 0, 6); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x383, 0xF, 0, 7); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x383, 0xF, 0, 8); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_5_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x383, 0xF, 0, 9); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MUX_1 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_MAIN_MUX_1_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_MAIN_MUX_1 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_2_MAIN_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x383, 0xF, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_MAIN_MUX_2_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_MAIN_MUX_2 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_2_MAIN_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_2_MAIN_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x383, 0xF0, 4, 5); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x383, 0xF0, 4, 6); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x383, 0xF0, 4, 7); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x383, 0xF0, 4, 8); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_5_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x383, 0xF0, 4, 9); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MUX_2 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_MAIN_MUX_2_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_MAIN_MUX_2 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_2_MAIN_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x383, 0xF0, 4, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_MAIN_MUX_3_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_MAIN_MUX_3 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_2_MAIN_MUX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MUX_3 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_2_MAIN_MUX_3 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x384, 0xF, 0, 5); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x384, 0xF, 0, 6); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x384, 0xF, 0, 7); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x384, 0xF, 0, 8); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_5_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x384, 0xF, 0, 9); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MUX_3 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_MAIN_MUX_3_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_MAIN_MUX_3 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_2_MAIN_MUX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MUX_3 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x384, 0xF, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MUX_3 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_MAIN_MIXER_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_MAIN_MIXER CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_2_MAIN_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_2_MAIN_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_2_MAIN_MUX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_2_MAIN_MUX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_2_MAIN_MUX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MIXER CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_MAIN_MIXER_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_MAIN_MIXER DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_2_MAIN_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MIXER DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_MAIN_MIXER DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_ST_MUX_1_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_ST_MUX_1 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_2_ST_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_ST_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_2_ST_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_ST_MUX_1 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_ST_MUX_1_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_ST_MUX_1 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_2_ST_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_ST_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_ST_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_ST_MUX_2_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_ST_MUX_2 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_2_ST_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_ST_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_2_ST_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_ST_MUX_2 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_ST_MUX_2_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_ST_MUX_2 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_2_ST_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_ST_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_ST_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_ST_MIXER_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_ST_MIXER CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_2_ST_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_ST_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_2_ST_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_2_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_2_ST_MUX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_2_ST_MUX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_ST_MIXER CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_ST_MIXER_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_ST_MIXER DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_2_ST_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_ST_MIXER DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_ST_MIXER DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_HPF_BYPASS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_HPF BYPASS.> \n"); 
	 if (p_element->id != WCD_RX_2_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_HPF BYPASS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2BC, 0x4, 2, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_HPF BYPASS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_HPF_SET_CUTOFF_FREQ_4HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_HPF SET_CUTOFF_FREQ_4HZ.> \n"); 
	 if (p_element->id != WCD_RX_2_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_HPF SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2BC, 0x4, 2, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2BB, 0x3, 0, 0x0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_HPF SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_HPF_SET_CUTOFF_FREQ_75HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_HPF SET_CUTOFF_FREQ_75HZ.> \n"); 
	 if (p_element->id != WCD_RX_2_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_HPF SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2BC, 0x4, 2, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2BB, 0x3, 0, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_HPF SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_HPF_SET_CUTOFF_FREQ_150HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_HPF SET_CUTOFF_FREQ_150HZ.> \n"); 
	 if (p_element->id != WCD_RX_2_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_HPF SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2BC, 0x4, 2, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2BB, 0x3, 0, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_HPF SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_PATH_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_PATH ENABLE.> \n"); 
	 if (p_element->id != WCD_RX_2_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_PATH_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_PATH DISABLE.> \n"); 
	 if (p_element->id != WCD_RX_2_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_PATH_MUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_PATH MUTE.> \n"); 
	 if (p_element->id != WCD_RX_2_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH MUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2BD, 0x1, 0, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH MUTE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_PATH_UNMUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_PATH UNMUTE.> \n"); 
	 if (p_element->id != WCD_RX_2_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH UNMUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2BD, 1, 0, 0);
	
	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH UNMUTE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_PATH_RESET(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_PATH RESET.> \n"); 
	 if (p_element->id != WCD_RX_2_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_RESET(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH RESET. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_PATH_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_PATH SET_GAIN.> \n"); 
	 if (p_element->id != WCD_RX_2_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_SET_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_PATH SET_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_COMPANDER_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_COMPANDER ENABLE.> \n"); 
	 if (p_element->id != WCD_RX_2_COMPANDER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_COMPANDER ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_COMPANDER_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_COMPANDER ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_COMPANDER ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_2_COMPANDER_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_2_COMPANDER DISABLE.> \n"); 
	 if (p_element->id != WCD_RX_2_COMPANDER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_COMPANDER DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_COMPANDER_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_COMPANDER DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_2_COMPANDER DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_CLASS_CONTROLLER_2_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event CLASS_CONTROLLER_2 ENABLE.> \n"); 
	 if (p_element->id != WCD_CLASS_CONTROLLER_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_2 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_CLASS_CONTROLLER_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_2 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_2 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_CLASS_CONTROLLER_2_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event CLASS_CONTROLLER_2 DISABLE.> \n"); 
	 if (p_element->id != WCD_CLASS_CONTROLLER_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_2 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_CLASS_CONTROLLER_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_2 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_2 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_BIAS_2_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_BIAS_2 ENABLE.> \n"); 
	 if (p_element->id != WCD_RX_BIAS_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_2 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_BIAS_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_2 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_2 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_BIAS_2_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_BIAS_2 DISABLE.> \n"); 
	 if (p_element->id != WCD_RX_BIAS_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_2 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_BIAS_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_2 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_2 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RDAC_3_MUX_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RDAC_3_MUX CONNECT.> \n"); 
	 if (p_element->id != WCD_RDAC_3_MUX_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_3_MUX CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RDAC_3_MUX CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_BIAS_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x384, 0x10, 4, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_BIAS_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x384, 0x10, 4, 1); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_3_MUX CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RDAC_3_MUX_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RDAC_3_MUX DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RDAC_3_MUX_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_3_MUX DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x384, 0x10, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_3_MUX DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_HPHR_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event HPHR SET_GAIN.> \n"); 
	 if (p_element->id != WCD_HPHR_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHR SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_HPH_SET_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHR SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHR SET_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_HPHR_PRECHARGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event HPHR PRECHARGE.> \n"); 
	 if (p_element->id != WCD_HPHR_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHR PRECHARGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_HPH_PRECHARGE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHR PRECHARGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHR PRECHARGE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_HPHR_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event HPHR ENABLE.> \n"); 
	 if (p_element->id != WCD_HPHR_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHR ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_HPH_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHR ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHR ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_HPHR_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event HPHR DISABLE.> \n"); 
	 if (p_element->id != WCD_HPHR_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHR DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_HPH_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHR DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event HPHR DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_MAIN_MUX_1_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_MAIN_MUX_1 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_3_MAIN_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_3_MAIN_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x386, 0xF, 0, 5); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x386, 0xF, 0, 6); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x386, 0xF, 0, 7); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x386, 0xF, 0, 8); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_5_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x386, 0xF, 0, 9); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MUX_1 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_MAIN_MUX_1_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_MAIN_MUX_1 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_3_MAIN_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x386, 0xF, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_MAIN_MUX_2_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_MAIN_MUX_2 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_3_MAIN_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_3_MAIN_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x386, 0xF0, 4, 5); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x386, 0xF0, 4, 6); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x386, 0xF0, 4, 7); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x386, 0xF0, 4, 8); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_5_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x386, 0xF0, 4, 9); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MUX_2 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_MAIN_MUX_2_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_MAIN_MUX_2 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_3_MAIN_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x386, 0xF0, 4, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_MAIN_MUX_3_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_MAIN_MUX_3 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_3_MAIN_MUX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MUX_3 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_3_MAIN_MUX_3 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x387, 0xF, 0, 5); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x387, 0xF, 0, 6); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x387, 0xF, 0, 7); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x387, 0xF, 0, 8); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_5_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x387, 0xF, 0, 9); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MUX_3 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_MAIN_MUX_3_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_MAIN_MUX_3 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_3_MAIN_MUX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MUX_3 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x387, 0xF, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MUX_3 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_MAIN_MIXER_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_MAIN_MIXER CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_3_MAIN_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_3_MAIN_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_3_MAIN_MUX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_3_MAIN_MUX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_3_MAIN_MUX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MIXER CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_MAIN_MIXER_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_MAIN_MIXER DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_3_MAIN_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MIXER DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_MAIN_MIXER DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_HPF_BYPASS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_HPF BYPASS.> \n"); 
	 if (p_element->id != WCD_RX_3_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_HPF BYPASS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2C4, 0x4, 2, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_HPF BYPASS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_HPF_SET_CUTOFF_FREQ_4HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_HPF SET_CUTOFF_FREQ_4HZ.> \n"); 
	 if (p_element->id != WCD_RX_3_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_HPF SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2C4, 0x4, 2, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2C3, 0x3, 0, 0x0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_HPF SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_HPF_SET_CUTOFF_FREQ_75HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_HPF SET_CUTOFF_FREQ_75HZ.> \n"); 
	 if (p_element->id != WCD_RX_3_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_HPF SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2C4, 0x4, 2, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2C3, 0x3, 0, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_HPF SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_HPF_SET_CUTOFF_FREQ_150HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_HPF SET_CUTOFF_FREQ_150HZ.> \n"); 
	 if (p_element->id != WCD_RX_3_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_HPF SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2C4, 0x4, 2, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2C3, 0x3, 0, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_HPF SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_PATH_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_PATH ENABLE.> \n"); 
	 if (p_element->id != WCD_RX_3_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_PATH_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_PATH DISABLE.> \n"); 
	 if (p_element->id != WCD_RX_3_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_PATH_MUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_PATH MUTE.> \n"); 
	 if (p_element->id != WCD_RX_3_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH MUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2C5, 0x1, 0, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH MUTE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_PATH_UNMUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_PATH UNMUTE.> \n"); 
	 if (p_element->id != WCD_RX_3_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH UNMUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2C5, 1, 0, 0); 
    

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH UNMUTE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_PATH_RESET(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_PATH RESET.> \n"); 
	 if (p_element->id != WCD_RX_3_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_RESET(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH RESET. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_PATH_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_PATH SET_GAIN.> \n"); 
	 if (p_element->id != WCD_RX_3_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_SET_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_PATH SET_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_MAIN_MUX_1_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_MAIN_MUX_1 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_MAIN_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_4_MAIN_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x388, 0xF, 0, 5); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x388, 0xF, 0, 6); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x388, 0xF, 0, 7); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x388, 0xF, 0, 8); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_5_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x388, 0xF, 0, 9); 
	 } 

	
     wcd_register_write(E_WCD_CODEC_MAIN, 0x388, 0x05, 0, 0x05);


	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MUX_1 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_MAIN_MUX_1_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_MAIN_MUX_1 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_MAIN_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x388, 0xF, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_MAIN_MUX_2_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_MAIN_MUX_2 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_MAIN_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_4_MAIN_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x388, 0xF0, 4, 5); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x388, 0xF0, 4, 6); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x388, 0xF0, 4, 7); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x388, 0xF0, 4, 8); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_5_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x388, 0xF0, 4, 9); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MUX_2 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_MAIN_MUX_2_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_MAIN_MUX_2 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_MAIN_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x388, 0xF0, 4, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_MAIN_MUX_3_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_MAIN_MUX_3 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_MAIN_MUX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MUX_3 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_4_MAIN_MUX_3 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x389, 0xF, 0, 5); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x389, 0xF, 0, 6); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x389, 0xF, 0, 7); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x389, 0xF, 0, 8); 
	 } 
	 if (p_element->connected_element->id == WCD_SBRX_5_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x389, 0xF, 0, 9); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MUX_3 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_MAIN_MUX_3_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_MAIN_MUX_3 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_MAIN_MUX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MUX_3 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x389, 0xF, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MUX_3 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_MAIN_MIXER_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_MAIN_MIXER CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_MAIN_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_4_MAIN_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_4_MAIN_MUX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_4_MAIN_MUX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_4_MAIN_MUX_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MIXER CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_MAIN_MIXER_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_MAIN_MIXER DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_MAIN_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MIXER DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_MAIN_MIXER DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_ST_MUX_1_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_ST_MUX_1 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_ST_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_ST_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_4_ST_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_ST_MUX_1 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_ST_MUX_1_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_ST_MUX_1 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_ST_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_ST_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_ST_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_ST_MUX_2_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_ST_MUX_2 CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_ST_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_ST_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_4_ST_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_ST_MUX_2 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_ST_MUX_2_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_ST_MUX_2 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_ST_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_ST_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_ST_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_ST_MIXER_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_ST_MIXER CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_ST_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_ST_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_4_ST_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_4_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_4_ST_MUX_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_4_ST_MUX_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_ST_MIXER CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_ST_MIXER_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_ST_MIXER DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_ST_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_ST_MIXER DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_ST_MIXER DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_HPF_BYPASS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_HPF BYPASS.> \n"); 
	 if (p_element->id != WCD_RX_4_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_HPF BYPASS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2CC, 0x4, 2, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_HPF BYPASS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_HPF_SET_CUTOFF_FREQ_4HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_HPF SET_CUTOFF_FREQ_4HZ.> \n"); 
	 if (p_element->id != WCD_RX_4_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_HPF SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2CC, 0x4, 2, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2CB, 0x3, 0, 0x0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_HPF SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_HPF_SET_CUTOFF_FREQ_75HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_HPF SET_CUTOFF_FREQ_75HZ.> \n"); 
	 if (p_element->id != WCD_RX_4_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_HPF SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2CC, 0x4, 2, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2CB, 0x3, 0, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_HPF SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_HPF_SET_CUTOFF_FREQ_150HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_HPF SET_CUTOFF_FREQ_150HZ.> \n"); 
	 if (p_element->id != WCD_RX_4_HPF_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_HPF SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2CC, 0x4, 2, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2CB, 0x3, 0, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_HPF SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_PATH_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_PATH ENABLE.> \n"); 
	 if (p_element->id != WCD_RX_4_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_PATH_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_PATH DISABLE.> \n"); 
	 if (p_element->id != WCD_RX_4_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_PATH_MUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_PATH MUTE.> \n"); 
	 if (p_element->id != WCD_RX_4_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH MUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2CD, 0x1, 0, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH MUTE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_PATH_UNMUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_PATH UNMUTE.> \n"); 
	 if (p_element->id != WCD_RX_4_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH UNMUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x2CD, 1, 0, 0); 
	  WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH UNMUTE. RC: %d.> \n", return_code); 

	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
     

	 //WCD_SLEEP(1);
	 //taking dumps after unmute to compare
  	 //wcd_ioctl(NULL, WCD_IOCTL_REGISTER_DUMP_TO_LOG, NULL);
     // WCD_SLEEP(1);


	 return return_code; 
}
wcd_result wcd_RX_4_PATH_RESET(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_PATH RESET.> \n"); 
	 if (p_element->id != WCD_RX_4_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_RESET(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH RESET. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_PATH_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_PATH SET_GAIN.> \n"); 
	 if (p_element->id != WCD_RX_4_PATH_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_PATH_SET_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_PATH SET_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_1_SELECT_MICBIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_1 SELECT_MICBIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_MICBIAS(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 SELECT_MICBIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_1_SELECT_CFILTER(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_1 SELECT_CFILTER.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_CFILTER(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 SELECT_CFILTER. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_1_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_1 ENABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_1_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_1 DISABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_1_APPLY_DESIRED_VOLTAGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_1 APPLY_DESIRED_VOLTAGE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_DESIRED_VOLTAGE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_1_APPLY_EXTERNAL_BYPASS_CAPACITOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_1 APPLY_EXTERNAL_BYPASS_CAPACITOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_EXTERNAL_BYPASS_CAPACITOR(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_1_SET_INTERNAL_BIAS_RESISTOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_1 SET_INTERNAL_BIAS_RESISTOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SET_IB_RES(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_1_ENABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_1 ENABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_1_DISABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_1 DISABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_1 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_DMIC_1_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event DMIC_1 ENABLE.> \n"); 
	 if (p_element->id != WCD_DMIC_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_1 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_DMIC_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_1 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_1 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_DMIC_1_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event DMIC_1 DISABLE.> \n"); 
	 if (p_element->id != WCD_DMIC_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_1 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_DMIC_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_1 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_1 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_DMIC_1_SET_CLOCK_MODE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event DMIC_1 SET_CLOCK_MODE.> \n"); 
	 if (p_element->id != WCD_DMIC_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_1 SET_CLOCK_MODE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_DMIC_SET_CLOCK_MODE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_1 SET_CLOCK_MODE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_1 SET_CLOCK_MODE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_2_SELECT_MICBIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_2 SELECT_MICBIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_MICBIAS(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 SELECT_MICBIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_2_SELECT_CFILTER(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_2 SELECT_CFILTER.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_CFILTER(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 SELECT_CFILTER. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_2_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_2 ENABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_2_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_2 DISABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_2_APPLY_DESIRED_VOLTAGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_2 APPLY_DESIRED_VOLTAGE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_DESIRED_VOLTAGE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_2_APPLY_EXTERNAL_BYPASS_CAPACITOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_2 APPLY_EXTERNAL_BYPASS_CAPACITOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_EXTERNAL_BYPASS_CAPACITOR(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_2_SET_INTERNAL_BIAS_RESISTOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_2 SET_INTERNAL_BIAS_RESISTOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SET_IB_RES(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_2_ENABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_2 ENABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_2_DISABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_2 DISABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_2 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_DMIC_2_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event DMIC_2 ENABLE.> \n"); 
	 if (p_element->id != WCD_DMIC_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_2 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_DMIC_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_2 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_2 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_DMIC_2_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event DMIC_2 DISABLE.> \n"); 
	 if (p_element->id != WCD_DMIC_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_2 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_DMIC_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_2 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_2 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_DMIC_2_SET_CLOCK_MODE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event DMIC_2 SET_CLOCK_MODE.> \n"); 
	 if (p_element->id != WCD_DMIC_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_2 SET_CLOCK_MODE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_DMIC_SET_CLOCK_MODE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_2 SET_CLOCK_MODE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_2 SET_CLOCK_MODE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_3_SELECT_MICBIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_3 SELECT_MICBIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_MICBIAS(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 SELECT_MICBIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_3_SELECT_CFILTER(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_3 SELECT_CFILTER.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_CFILTER(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 SELECT_CFILTER. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_3_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_3 ENABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_3_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_3 DISABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_3_APPLY_DESIRED_VOLTAGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_3 APPLY_DESIRED_VOLTAGE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_DESIRED_VOLTAGE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_3_APPLY_EXTERNAL_BYPASS_CAPACITOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_3 APPLY_EXTERNAL_BYPASS_CAPACITOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_EXTERNAL_BYPASS_CAPACITOR(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_3_SET_INTERNAL_BIAS_RESISTOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_3 SET_INTERNAL_BIAS_RESISTOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SET_IB_RES(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_3_ENABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_3 ENABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_3_DISABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_3 DISABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_3 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_DMIC_3_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event DMIC_3 ENABLE.> \n"); 
	 if (p_element->id != WCD_DMIC_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_3 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_DMIC_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_3 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_3 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_DMIC_3_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event DMIC_3 DISABLE.> \n"); 
	 if (p_element->id != WCD_DMIC_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_3 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_DMIC_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_3 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_3 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_DMIC_3_SET_CLOCK_MODE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event DMIC_3 SET_CLOCK_MODE.> \n"); 
	 if (p_element->id != WCD_DMIC_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_3 SET_CLOCK_MODE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_DMIC_SET_CLOCK_MODE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_3 SET_CLOCK_MODE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_3 SET_CLOCK_MODE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_4_SELECT_MICBIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_4 SELECT_MICBIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_MICBIAS(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 SELECT_MICBIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_4_SELECT_CFILTER(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_4 SELECT_CFILTER.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_CFILTER(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 SELECT_CFILTER. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_4_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_4 ENABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_4_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_4 DISABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_4_APPLY_DESIRED_VOLTAGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_4 APPLY_DESIRED_VOLTAGE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_DESIRED_VOLTAGE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_4_APPLY_EXTERNAL_BYPASS_CAPACITOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_4 APPLY_EXTERNAL_BYPASS_CAPACITOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_EXTERNAL_BYPASS_CAPACITOR(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_4_SET_INTERNAL_BIAS_RESISTOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_4 SET_INTERNAL_BIAS_RESISTOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SET_IB_RES(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_4_ENABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_4 ENABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_4_DISABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_4 DISABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_4 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_DMIC_4_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event DMIC_4 ENABLE.> \n"); 
	 if (p_element->id != WCD_DMIC_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_4 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_DMIC_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_4 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_4 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_DMIC_4_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event DMIC_4 DISABLE.> \n"); 
	 if (p_element->id != WCD_DMIC_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_4 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_DMIC_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_4 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_4 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_DMIC_4_SET_CLOCK_MODE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event DMIC_4 SET_CLOCK_MODE.> \n"); 
	 if (p_element->id != WCD_DMIC_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_4 SET_CLOCK_MODE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_DMIC_SET_CLOCK_MODE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_4 SET_CLOCK_MODE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event DMIC_4 SET_CLOCK_MODE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_5_SELECT_MICBIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_5 SELECT_MICBIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_MICBIAS(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 SELECT_MICBIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_5_SELECT_CFILTER(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_5 SELECT_CFILTER.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_CFILTER(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 SELECT_CFILTER. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_5_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_5 ENABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_5_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_5 DISABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_5_APPLY_DESIRED_VOLTAGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_5 APPLY_DESIRED_VOLTAGE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_DESIRED_VOLTAGE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_5_APPLY_EXTERNAL_BYPASS_CAPACITOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_5 APPLY_EXTERNAL_BYPASS_CAPACITOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_EXTERNAL_BYPASS_CAPACITOR(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_5_SET_INTERNAL_BIAS_RESISTOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_5 SET_INTERNAL_BIAS_RESISTOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SET_IB_RES(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_5_ENABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_5 ENABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_5_DISABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_5 DISABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_5 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_ADC_1_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event ADC_1 ENABLE.> \n"); 
	 if (p_element->id != WCD_ADC_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_1 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_ADC_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_1 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_1 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_ADC_1_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event ADC_1 DISABLE.> \n"); 
	 if (p_element->id != WCD_ADC_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_1 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_ADC_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_1 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_1 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_ADC_1_SET_ANALOG_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event ADC_1 SET_ANALOG_GAIN.> \n"); 
	 if (p_element->id != WCD_ADC_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_1 SET_ANALOG_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_ADC_SET_ANALOG_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_1 SET_ANALOG_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_1 SET_ANALOG_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_6_SELECT_MICBIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_6 SELECT_MICBIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_6_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_MICBIAS(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 SELECT_MICBIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_6_SELECT_CFILTER(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_6 SELECT_CFILTER.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_6_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_CFILTER(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 SELECT_CFILTER. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_6_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_6 ENABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_6_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_6_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_6 DISABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_6_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_6_APPLY_DESIRED_VOLTAGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_6 APPLY_DESIRED_VOLTAGE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_6_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_DESIRED_VOLTAGE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_6_APPLY_EXTERNAL_BYPASS_CAPACITOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_6 APPLY_EXTERNAL_BYPASS_CAPACITOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_6_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_EXTERNAL_BYPASS_CAPACITOR(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_6_SET_INTERNAL_BIAS_RESISTOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_6 SET_INTERNAL_BIAS_RESISTOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_6_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SET_IB_RES(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_6_ENABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_6 ENABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_6_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_6_DISABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_6 DISABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_6_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_6 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_7_SELECT_MICBIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_7 SELECT_MICBIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_7_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_MICBIAS(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 SELECT_MICBIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_7_SELECT_CFILTER(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_7 SELECT_CFILTER.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_7_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_CFILTER(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 SELECT_CFILTER. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_7_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_7 ENABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_7_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_7_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_7 DISABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_7_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_7_APPLY_DESIRED_VOLTAGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_7 APPLY_DESIRED_VOLTAGE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_7_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_DESIRED_VOLTAGE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_7_APPLY_EXTERNAL_BYPASS_CAPACITOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_7 APPLY_EXTERNAL_BYPASS_CAPACITOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_7_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_EXTERNAL_BYPASS_CAPACITOR(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_7_SET_INTERNAL_BIAS_RESISTOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_7 SET_INTERNAL_BIAS_RESISTOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_7_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SET_IB_RES(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_7_ENABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_7 ENABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_7_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_7_DISABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_7 DISABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_7_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_7 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_8_SELECT_MICBIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_8 SELECT_MICBIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_8_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_MICBIAS(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 SELECT_MICBIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_8_SELECT_CFILTER(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_8 SELECT_CFILTER.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_8_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_CFILTER(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 SELECT_CFILTER. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_8_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_8 ENABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_8_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_8_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_8 DISABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_8_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_8_APPLY_DESIRED_VOLTAGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_8 APPLY_DESIRED_VOLTAGE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_8_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_DESIRED_VOLTAGE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_8_APPLY_EXTERNAL_BYPASS_CAPACITOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_8 APPLY_EXTERNAL_BYPASS_CAPACITOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_8_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_EXTERNAL_BYPASS_CAPACITOR(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_8_SET_INTERNAL_BIAS_RESISTOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_8 SET_INTERNAL_BIAS_RESISTOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_8_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SET_IB_RES(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_8_ENABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_8 ENABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_8_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_8_DISABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_8 DISABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_8_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_8 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_ADC_2_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event ADC_2 ENABLE.> \n"); 
	 if (p_element->id != WCD_ADC_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_2 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_ADC_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_2 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_2 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_ADC_2_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event ADC_2 DISABLE.> \n"); 
	 if (p_element->id != WCD_ADC_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_2 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_ADC_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_2 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_2 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_ADC_2_SET_ANALOG_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event ADC_2 SET_ANALOG_GAIN.> \n"); 
	 if (p_element->id != WCD_ADC_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_2 SET_ANALOG_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_ADC_SET_ANALOG_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_2 SET_ANALOG_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_2 SET_ANALOG_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_MUX_1_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_MUX_1 CONNECT.> \n"); 
	 if (p_element->id != WCD_TX_PATH_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. TX_PATH_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF, 0x0, 0x1); 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF, 0x0, 0x2); 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF, 0x0, 0x3); 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF, 0x0, 0x4);
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF, 0x0, 0x6);
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x223, 0x1, 0x0, 0x1); 
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF, 0x0, 0x7);
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x223, 0x1, 0x0, 0x1); 
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF, 0x0, 0x8); 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x223, 0x1, 0x0, 0x1); 
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF, 0x0, 0x9);
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x223, 0x1, 0x0, 0x1); 
	 } 
      
     
	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_1 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_MUX_1_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_MUX_1 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_TX_PATH_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF, 0, 0);
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x223, 0x1, 0x0, 0x0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_1_BYPASS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_1 BYPASS.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_1 BYPASS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x223, 0x8, 3, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_1 BYPASS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_1_SET_CUTOFF_FREQ_4HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_1 SET_CUTOFF_FREQ_4HZ.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_1 SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x223, 0x8, 3, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x223, 0x30, 4, 0x0); 
     
		 
     



	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_1 SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_1_SET_CUTOFF_FREQ_75HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_1 SET_CUTOFF_FREQ_75HZ.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_1 SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x223, 0x8, 3, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x223, 0x30, 4, 0x1); 

	  WCD_DEBUG_ERROR_0("=WCD_93XX: <@Ram : dumping loopback registers.>\n");
 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_1 SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_1_SET_CUTOFF_FREQ_150HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_1 SET_CUTOFF_FREQ_150HZ.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_1 SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x223, 0x8, 3, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x223, 0x30, 4, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_1 SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_1_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_1 ENABLE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_1_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_1 DISABLE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_1_RESET(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_1 RESET.> \n"); 
	 if (p_element->id != WCD_TX_PATH_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_RESET(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 RESET. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_1_MUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_1 MUTE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 MUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x222, 1, 0, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 MUTE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_1_UNMUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_1 UNMUTE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 UNMUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x222, 1, 0, 0); 
	// wcd_register_write(E_WCD_CODEC_MAIN, 0xC0, 0x18, 0, 0x18); 
	 //wcd_register_write(E_WCD_CODEC_MAIN, 0xC1, 0x18, 0, 0x18); 
	 //wcd_register_write(E_WCD_CODEC_MAIN, 0xC2, 0x18, 0, 0x18); 
	// wcd_register_write(E_WCD_CODEC_MAIN, 0xC3, 0x18, 0, 0x18); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 UNMUTE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_1_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_1 SET_GAIN.> \n"); 
	 if (p_element->id != WCD_TX_PATH_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_SET_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_1 SET_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_MUX_2_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_MUX_2 CONNECT.> \n"); 
	 if (p_element->id != WCD_TX_PATH_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. TX_PATH_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF0, 0x4, 0x1); 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF0, 0x4, 0x2); 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF0, 0x4, 0x3); 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF0, 0x4, 0x4); 
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF0, 0x4, 0x6);
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x22B, 0x1, 0x0, 0x1); 
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF0, 0x4, 0x7);
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x22B, 0x1, 0x0, 0x1);
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF0, 0x4, 0x8);
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x22B, 0x1, 0x0, 0x1);
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF0, 0x4, 0x9);
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x22B, 0x1, 0x0, 0x1);
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_2 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_MUX_2_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_MUX_2 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_TX_PATH_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x393, 0xF0, 0x0, 0);
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x22B, 0x1, 0x0, 0x0);

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_MUX_3_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_MUX_3 CONNECT.> \n"); 
	 if (p_element->id != WCD_TX_PATH_MUX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_3 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. TX_PATH_MUX_3 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF, 0x0, 0x1); 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF, 0x0, 0x2); 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF, 0x0, 0x3); 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF, 0x0, 0x4); 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_5_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF, 0x0, 0x5); 
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF, 0x0, 0x7);
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x233, 0x1, 0x0, 0x1);
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF, 0x0, 0x8);
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x233, 0x1, 0x0, 0x1);
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF, 0x0, 0x9); 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x233, 0x1, 0x0, 0x1);
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF, 0x0, 0xA);
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x233, 0x1, 0x0, 0x1);
	 }
	 if (p_element->connected_element->id == WCD_ANC_CHANNEL_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF, 0, 0xB);
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_3 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_MUX_3_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_MUX_3 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_TX_PATH_MUX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_3 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF, 0x0, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x233, 0x1, 0x0, 0x0);

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_3 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_MUX_4_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_MUX_4 CONNECT.> \n"); 
	 if (p_element->id != WCD_TX_PATH_MUX_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_4 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. TX_PATH_MUX_4 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF0, 0x4, 0x1); 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF0, 0x4, 0x2); 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF0, 0x4, 0x3); 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF0, 0x4, 0x4); 
	 } 
	 if (p_element->connected_element->id == WCD_ADC_5_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF0, 0x4, 0x5); 
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF0, 0x4, 0x7);
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x23B, 0x1, 0x0, 0x1);
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF0, 0x4, 0x8);
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x23B, 0x1, 0x0, 0x1);
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF0, 0x4, 0x9); 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x23B, 0x1, 0x0, 0x1);
	 } 
	 if (p_element->connected_element->id == WCD_DMIC_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF0, 0x4, 0xA);
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x23B, 0x1, 0x0, 0x1);
	 }
	 if (p_element->connected_element->id == WCD_ANC_CHANNEL_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF0, 0x4, 0xB);
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_4 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_MUX_4_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_MUX_4 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_TX_PATH_MUX_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_4 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x394, 0xF0, 0x4, 0x0);
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x23B, 0x1, 0x0, 0x0);

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_MUX_4 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_2_BYPASS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_2 BYPASS.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_2 BYPASS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x22B, 0x8, 3, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_2 BYPASS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_2_SET_CUTOFF_FREQ_4HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_2 SET_CUTOFF_FREQ_4HZ.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_2 SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x22B, 0x8, 3, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x22B, 0x30, 4, 0x0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_2 SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_2_SET_CUTOFF_FREQ_75HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_2 SET_CUTOFF_FREQ_75HZ.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_2 SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x22B, 0x8, 3, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x22B, 0x30, 4, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_2 SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_2_SET_CUTOFF_FREQ_150HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_2 SET_CUTOFF_FREQ_150HZ.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_2 SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x22B, 0x8, 3, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x22B, 0x30, 4, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_2 SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_2_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_2 ENABLE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_2_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_2 DISABLE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_2_RESET(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_2 RESET.> \n"); 
	 if (p_element->id != WCD_TX_PATH_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_RESET(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 RESET. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_2_MUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_2 MUTE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 MUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x22A, 1, 0, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 MUTE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_2_UNMUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_2 UNMUTE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 UNMUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x22A, 1, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 UNMUTE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_2_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_2 SET_GAIN.> \n"); 
	 if (p_element->id != WCD_TX_PATH_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_SET_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_2 SET_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_3_BYPASS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_3 BYPASS.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_3 BYPASS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x233, 0x8, 3, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_3 BYPASS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_3_SET_CUTOFF_FREQ_4HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_3 SET_CUTOFF_FREQ_4HZ.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_3 SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x233, 0x8, 3, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x233, 0x30, 4, 0x0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_3 SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_3_SET_CUTOFF_FREQ_75HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_3 SET_CUTOFF_FREQ_75HZ.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_3 SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x233, 0x8, 3, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x233, 0x30, 4, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_3 SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_3_SET_CUTOFF_FREQ_150HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_3 SET_CUTOFF_FREQ_150HZ.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_3 SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x233, 0x8, 3, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x233, 0x30, 4, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_3 SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_4_BYPASS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_4 BYPASS.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_4 BYPASS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x23B, 0x8, 3, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_4 BYPASS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_4_SET_CUTOFF_FREQ_4HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_4 SET_CUTOFF_FREQ_4HZ.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_4 SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x23B, 0x8, 3, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x23B, 0x30, 4, 0x0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_4 SET_CUTOFF_FREQ_4HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_4_SET_CUTOFF_FREQ_75HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_4 SET_CUTOFF_FREQ_75HZ.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_4 SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x23B, 0x8, 3, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x23B, 0x30, 4, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_4 SET_CUTOFF_FREQ_75HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_HPF_4_SET_CUTOFF_FREQ_150HZ(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_HPF_4 SET_CUTOFF_FREQ_150HZ.> \n"); 
	 if (p_element->id != WCD_TX_PATH_HPF_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_4 SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x23B, 0x8, 3, 0x0); 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x23B, 0x30, 4, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_HPF_4 SET_CUTOFF_FREQ_150HZ. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_3_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_3 ENABLE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_3_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_3 DISABLE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_3_RESET(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_3 RESET.> \n"); 
	 if (p_element->id != WCD_TX_PATH_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_RESET(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 RESET. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_3_MUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_3 MUTE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 MUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x232, 1, 0, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 MUTE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_3_UNMUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_3 UNMUTE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 UNMUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x232, 1, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 UNMUTE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_3_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_3 SET_GAIN.> \n"); 
	 if (p_element->id != WCD_TX_PATH_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_SET_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_3 SET_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_4_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_4 ENABLE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_4_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_4 DISABLE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_4_RESET(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_4 RESET.> \n"); 
	 if (p_element->id != WCD_TX_PATH_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_RESET(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 RESET. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 RESET. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_4_MUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_4 MUTE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 MUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x23A, 1, 0, 1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 MUTE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_4_UNMUTE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_4 UNMUTE.> \n"); 
	 if (p_element->id != WCD_TX_PATH_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 UNMUTE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x23A, 1, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 UNMUTE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_TX_PATH_4_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event TX_PATH_4 SET_GAIN.> \n"); 
	 if (p_element->id != WCD_TX_PATH_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_TX_PATH_SET_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event TX_PATH_4 SET_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_MUX_1_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_MUX_1 CONNECT.> \n"); 
	 if (p_element->id != WCD_SBTX_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. SBTX_MUX_1 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_1_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A3, 0xF, 0, 0x1); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_2_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A3, 0xF, 0, 0x2); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_3_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A3, 0xF, 0, 0x3); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_4_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A3, 0xF, 0, 0x4); 
	 } 
	 if (p_element->connected_element->id == WCD_TX_PATH_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A3, 0xF, 0, 0x8); 
	 } 
	 if (p_element->connected_element->id == WCD_TX_PATH_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A3, 0xF, 0, 0x9); 
	 } 
	 if (p_element->connected_element->id == WCD_TX_PATH_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A3, 0xF, 0, 0xA); 
	 } 
	 if (p_element->connected_element->id == WCD_TX_PATH_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A3, 0xF, 0, 0xB); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_1 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_MUX_1_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_MUX_1 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_SBTX_MUX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A3, 0xF, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_1 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_1_SET_BIT_WIDTH_12BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_1 SET_BIT_WIDTH_12BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_1 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A3, 0x30, 4, 0x3); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_1 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_1_SET_BIT_WIDTH_16BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_1 SET_BIT_WIDTH_16BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_1 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A3, 0x30, 4, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_1 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_1_SET_BIT_WIDTH_20BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_1 SET_BIT_WIDTH_20BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_1 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A3, 0x30, 4, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_1 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_1_SET_BIT_WIDTH_24BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_1 SET_BIT_WIDTH_24BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_1_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_1 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A3, 0x30, 4, 0x0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_1 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_MUX_2_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_MUX_2 CONNECT.> \n"); 
	 if (p_element->id != WCD_SBTX_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. SBTX_MUX_2 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_1_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A4, 0xF, 0, 0x1); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_2_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A4, 0xF, 0, 0x2); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_3_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A4, 0xF, 0, 0x3); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_4_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A4, 0xF, 0, 0x4); 
	 } 
	 if (p_element->connected_element->id == WCD_TX_PATH_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A4, 0xF, 0, 0x8); 
	 } 
	 if (p_element->connected_element->id == WCD_TX_PATH_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A4, 0xF, 0, 0x9); 
	 } 
	 if (p_element->connected_element->id == WCD_TX_PATH_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A4, 0xF, 0, 0xA); 
	 } 
	 if (p_element->connected_element->id == WCD_TX_PATH_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A4, 0xF, 0, 0xB); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_2 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_MUX_2_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_MUX_2 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_SBTX_MUX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A4, 0xF, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_2 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_2_SET_BIT_WIDTH_12BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_2 SET_BIT_WIDTH_12BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_2 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A4, 0x30, 4, 0x3); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_2 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_2_SET_BIT_WIDTH_16BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_2 SET_BIT_WIDTH_16BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_2 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A4, 0x30, 4, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_2 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_2_SET_BIT_WIDTH_20BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_2 SET_BIT_WIDTH_20BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_2 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A4, 0x30, 4, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_2 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_2_SET_BIT_WIDTH_24BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_2 SET_BIT_WIDTH_24BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_2_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_2 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A4, 0x30, 4, 0x0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_2 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_MUX_3_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_MUX_3 CONNECT.> \n"); 
	 if (p_element->id != WCD_SBTX_MUX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_3 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. SBTX_MUX_3 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_1_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A5, 0xF, 0, 0x1); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_2_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A5, 0xF, 0, 0x2); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_3_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A5, 0xF, 0, 0x3); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_4_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A5, 0xF, 0, 0x4); 
	 } 
	 if (p_element->connected_element->id == WCD_TX_PATH_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A5, 0xF, 0, 0xA); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_3 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_MUX_3_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_MUX_3 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_SBTX_MUX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_3 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A5, 0xF, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_3 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_MUX_4_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_MUX_4 CONNECT.> \n"); 
	 if (p_element->id != WCD_SBTX_MUX_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_4 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. SBTX_MUX_4 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_1_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A6, 0xF, 0, 0x1); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_2_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A6, 0xF, 0, 0x2); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_3_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A6, 0xF, 0, 0x3); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_4_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A6, 0xF, 0, 0x4); 
	 } 
	 if (p_element->connected_element->id == WCD_TX_PATH_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A6, 0xF, 0, 0xB); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_4 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_MUX_4_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_MUX_4 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_SBTX_MUX_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_4 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A6, 0xF, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_4 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_MUX_5_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_MUX_5 CONNECT.> \n"); 
	 if (p_element->id != WCD_SBTX_MUX_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_5 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. SBTX_MUX_5 CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_1_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A7, 0xF, 0, 0x1); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_2_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A7, 0xF, 0, 0x2); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_3_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A7, 0xF, 0, 0x3); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_4_MAIN_MIXER_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A7, 0xF, 0, 0x4); 
	 } 
	 if (p_element->connected_element->id == WCD_TX_PATH_1_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A7, 0xF, 0, 0x8); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_5 CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_MUX_5_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_MUX_5 DISCONNECT.> \n"); 
	 if (p_element->id != WCD_SBTX_MUX_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_5 DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A7, 0xF, 0, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_MUX_5 DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_3_SET_BIT_WIDTH_12BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_3 SET_BIT_WIDTH_12BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_3 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A5, 0x30, 4, 0x3); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_3 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_3_SET_BIT_WIDTH_16BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_3 SET_BIT_WIDTH_16BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_3 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A5, 0x30, 4, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_3 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_3_SET_BIT_WIDTH_20BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_3 SET_BIT_WIDTH_20BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_3 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A5, 0x30, 4, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_3 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_3_SET_BIT_WIDTH_24BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_3 SET_BIT_WIDTH_24BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_3 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A5, 0x30, 4, 0x0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_3 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_4_SET_BIT_WIDTH_12BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_4 SET_BIT_WIDTH_12BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_4 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A6, 0x30, 4, 0x3); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_4 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_4_SET_BIT_WIDTH_16BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_4 SET_BIT_WIDTH_16BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_4 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A6, 0x30, 4, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_4 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_4_SET_BIT_WIDTH_20BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_4 SET_BIT_WIDTH_20BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_4 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A6, 0x30, 4, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_4 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_4_SET_BIT_WIDTH_24BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_4 SET_BIT_WIDTH_24BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_4 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A6, 0x30, 4, 0x0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_4 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_5_SET_BIT_WIDTH_12BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_5 SET_BIT_WIDTH_12BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_5 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A7, 0x30, 4, 0x3); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_5 SET_BIT_WIDTH_12BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_5_SET_BIT_WIDTH_16BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_5 SET_BIT_WIDTH_16BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_5 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A7, 0x30, 4, 0x2); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_5 SET_BIT_WIDTH_16BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_5_SET_BIT_WIDTH_20BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_5 SET_BIT_WIDTH_20BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_5 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A7, 0x30, 4, 0x1); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_5 SET_BIT_WIDTH_20BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SBTX_5_SET_BIT_WIDTH_24BIT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SBTX_5 SET_BIT_WIDTH_24BIT.> \n"); 
	 if (p_element->id != WCD_SBTX_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_5 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3A7, 0x30, 4, 0x0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SBTX_5 SET_BIT_WIDTH_24BIT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_INTERPOLATOR_MIXER_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_INTERPOLATOR_MIXER CONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_INTERPOLATOR_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_INTERPOLATOR_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RX_4_INTERPOLATOR_MIXER CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_4_PATH_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_USONIC_MUX_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0, 0, 0, 0); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_INTERPOLATOR_MIXER CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_INTERPOLATOR_MIXER_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_INTERPOLATOR_MIXER DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RX_4_INTERPOLATOR_MIXER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_INTERPOLATOR_MIXER DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_INTERPOLATOR_MIXER DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_COMPANDER_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_COMPANDER ENABLE.> \n"); 
	 if (p_element->id != WCD_RX_3_COMPANDER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_COMPANDER ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_COMPANDER_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_COMPANDER ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_COMPANDER ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_3_COMPANDER_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_3_COMPANDER DISABLE.> \n"); 
	 if (p_element->id != WCD_RX_3_COMPANDER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_COMPANDER DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_COMPANDER_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_COMPANDER DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_3_COMPANDER DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_BIAS_3_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_BIAS_3 ENABLE.> \n"); 
	 if (p_element->id != WCD_RX_BIAS_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_3 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_BIAS_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_3 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_3 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_BIAS_3_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_BIAS_3 DISABLE.> \n"); 
	 if (p_element->id != WCD_RX_BIAS_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_3 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_BIAS_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_3 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_3 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RDAC_4_MUX_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RDAC_4_MUX CONNECT.> \n"); 
	 if (p_element->id != WCD_RDAC_4_MUX_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_4_MUX CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RDAC_4_MUX CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_BIAS_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3B1, 0x2, 1, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RX_BIAS_2_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3B1, 0x2, 1, 1); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_4_MUX CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RDAC_4_MUX_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RDAC_4_MUX DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RDAC_4_MUX_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_4_MUX DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3B1, 0x2, 1, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_4_MUX DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_LO1_L_P_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event LO1_L_P SET_GAIN.> \n"); 
	 if (p_element->id != WCD_LO1_L_P_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO1_L_P SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_LO_SET_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO1_L_P SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO1_L_P SET_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_LO1_L_P_PRECHARGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event LO1_L_P PRECHARGE.> \n"); 
	 if (p_element->id != WCD_LO1_L_P_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO1_L_P PRECHARGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_LO_PRECHARGE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO1_L_P PRECHARGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO1_L_P PRECHARGE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_LO1_L_P_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event LO1_L_P ENABLE.> \n"); 
	 if (p_element->id != WCD_LO1_L_P_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO1_L_P ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_LO_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO1_L_P ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO1_L_P ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_LO1_L_P_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event LO1_L_P DISABLE.> \n"); 
	 if (p_element->id != WCD_LO1_L_P_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO1_L_P DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_LO_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO1_L_P DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO1_L_P DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_COMPANDER_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_COMPANDER ENABLE.> \n"); 
	 if (p_element->id != WCD_RX_4_COMPANDER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_COMPANDER ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_COMPANDER_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_COMPANDER ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_COMPANDER ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_4_COMPANDER_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_4_COMPANDER DISABLE.> \n"); 
	 if (p_element->id != WCD_RX_4_COMPANDER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_COMPANDER DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_COMPANDER_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_COMPANDER DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_4_COMPANDER DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_CLASS_CONTROLLER_4_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event CLASS_CONTROLLER_4 ENABLE.> \n"); 
	 if (p_element->id != WCD_CLASS_CONTROLLER_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_4 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_CLASS_CONTROLLER_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_4 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_4 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_CLASS_CONTROLLER_4_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event CLASS_CONTROLLER_4 DISABLE.> \n"); 
	 if (p_element->id != WCD_CLASS_CONTROLLER_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_4 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_CLASS_CONTROLLER_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_4 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event CLASS_CONTROLLER_4 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_BIAS_4_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_BIAS_4 ENABLE.> \n"); 
	 if (p_element->id != WCD_RX_BIAS_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_4 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_BIAS_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_4 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_4 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_RX_BIAS_4_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RX_BIAS_4 DISABLE.> \n"); 
	 if (p_element->id != WCD_RX_BIAS_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_4 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_RX_BIAS_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_4 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RX_BIAS_4 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SPEAKER_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SPEAKER SET_GAIN.> \n"); 
	 if (p_element->id != WCD_SPEAKER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SPEAKER SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_SPEAKER_SET_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SPEAKER SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SPEAKER SET_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SPEAKER_PRECHARGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SPEAKER PRECHARGE.> \n"); 
	 if (p_element->id != WCD_SPEAKER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SPEAKER PRECHARGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_SPEAKER_PRECHARGE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SPEAKER PRECHARGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SPEAKER PRECHARGE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SPEAKER_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SPEAKER ENABLE.> \n"); 
	 if (p_element->id != WCD_SPEAKER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SPEAKER ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_SPEAKER_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SPEAKER ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SPEAKER ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_SPEAKER_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event SPEAKER DISABLE.> \n"); 
	 if (p_element->id != WCD_SPEAKER_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SPEAKER DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_SPEAKER_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SPEAKER DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event SPEAKER DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd9306_RDAC_5_DIFF_MUX_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RDAC_5_DIFF_MUX CONNECT.> \n"); 
	 if (p_element->id != WCD_RDAC_5_DIFF_MUX_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_5_DIFF_MUX CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RDAC_5_DIFF_MUX CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_BIAS_4_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3B1, 0x4, 0x2, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RDAC_4_MUX_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3B1, 0x4, 0x2, 1); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_5_DIFF_MUX CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd9306_RDAC_5_DIFF_MUX_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RDAC_5_DIFF_MUX DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RDAC_5_DIFF_MUX_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_5_DIFF_MUX DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3B1, 0x4, 0x2, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_5_DIFF_MUX DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd9302_RDAC_5_DIFF_MUX_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RDAC_5_DIFF_MUX CONNECT.> \n"); 
	 if (p_element->id != WCD_RDAC_5_DIFF_MUX_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_5_DIFF_MUX CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. RDAC_5_DIFF_MUX CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_BIAS_3_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3B1, 0x4, 0x2, 0); 
	 } 
	 if (p_element->connected_element->id == WCD_RDAC_4_MUX_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3B1, 0x4, 0x2, 1); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_5_DIFF_MUX CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd9302_RDAC_5_DIFF_MUX_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event RDAC_5_DIFF_MUX DISCONNECT.> \n"); 
	 if (p_element->id != WCD_RDAC_5_DIFF_MUX_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_5_DIFF_MUX DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3B1, 0x4, 0x2, 0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event RDAC_5_DIFF_MUX DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_LO2_R_M_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event LO2_R_M SET_GAIN.> \n"); 
	 if (p_element->id != WCD_LO2_R_M_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO2_R_M SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_LO_SET_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO2_R_M SET_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO2_R_M SET_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_LO2_R_M_PRECHARGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event LO2_R_M PRECHARGE.> \n"); 
	 if (p_element->id != WCD_LO2_R_M_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO2_R_M PRECHARGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_LO_PRECHARGE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO2_R_M PRECHARGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO2_R_M PRECHARGE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_LO2_R_M_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event LO2_R_M ENABLE.> \n"); 
	 if (p_element->id != WCD_LO2_R_M_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO2_R_M ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_LO_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO2_R_M ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO2_R_M ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_LO2_R_M_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event LO2_R_M DISABLE.> \n"); 
	 if (p_element->id != WCD_LO2_R_M_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO2_R_M DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_LO_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO2_R_M DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event LO2_R_M DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_USONIC_MUX_CONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event USONIC_MUX CONNECT.> \n"); 
	 if (p_element->id != WCD_USONIC_MUX_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event USONIC_MUX CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element == NULL) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ConnectedElement is NULL. USONIC_MUX CONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 if (p_element->connected_element->id == WCD_RX_3_PATH_ID) { 
		 wcd_register_write(E_WCD_CODEC_MAIN, 0x3B1, 0x8, 0x3, 0x1); 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event USONIC_MUX CONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_USONIC_MUX_DISCONNECT(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event USONIC_MUX DISCONNECT.> \n"); 
	 if (p_element->id != WCD_USONIC_MUX_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event USONIC_MUX DISCONNECT. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 wcd_register_write(E_WCD_CODEC_MAIN, 0x3B1, 0x8, 0x3, 0x0); 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event USONIC_MUX DISCONNECT. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_ADC_3_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event ADC_3 ENABLE.> \n"); 
	 if (p_element->id != WCD_ADC_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_3 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_ADC_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_3 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_3 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_ADC_3_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event ADC_3 DISABLE.> \n"); 
	 if (p_element->id != WCD_ADC_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_3 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_ADC_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_3 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_3 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_ADC_3_SET_ANALOG_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event ADC_3 SET_ANALOG_GAIN.> \n"); 
	 if (p_element->id != WCD_ADC_3_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_3 SET_ANALOG_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_ADC_SET_ANALOG_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_3 SET_ANALOG_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_3 SET_ANALOG_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_ADC_4_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event ADC_4 ENABLE.> \n"); 
	 if (p_element->id != WCD_ADC_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_4 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_ADC_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_4 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_4 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_ADC_4_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event ADC_4 DISABLE.> \n"); 
	 if (p_element->id != WCD_ADC_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_4 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_ADC_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_4 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_4 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_ADC_4_SET_ANALOG_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event ADC_4 SET_ANALOG_GAIN.> \n"); 
	 if (p_element->id != WCD_ADC_4_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_4 SET_ANALOG_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_ADC_SET_ANALOG_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_4 SET_ANALOG_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_4 SET_ANALOG_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_9_SELECT_MICBIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_9 SELECT_MICBIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_9_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_MICBIAS(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 SELECT_MICBIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 SELECT_MICBIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_9_SELECT_CFILTER(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_9 SELECT_CFILTER.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_9_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SELECT_CFILTER(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 SELECT_CFILTER. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 SELECT_CFILTER. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_9_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_9 ENABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_9_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_9_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_9 DISABLE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_9_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_9_APPLY_DESIRED_VOLTAGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_9 APPLY_DESIRED_VOLTAGE.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_9_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_DESIRED_VOLTAGE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 APPLY_DESIRED_VOLTAGE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_9_APPLY_EXTERNAL_BYPASS_CAPACITOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_9 APPLY_EXTERNAL_BYPASS_CAPACITOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_9_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_APPLY_EXTERNAL_BYPASS_CAPACITOR(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 APPLY_EXTERNAL_BYPASS_CAPACITOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_9_SET_INTERNAL_BIAS_RESISTOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_9 SET_INTERNAL_BIAS_RESISTOR.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_9_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_SET_IB_RES(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 SET_INTERNAL_BIAS_RESISTOR. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_9_ENABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_9 ENABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_9_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_ENABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 ENABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_MBIAS_MGR_9_DISABLE_INTERNAL_BIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event MBIAS_MGR_9 DISABLE_INTERNAL_BIAS.> \n"); 
	 if (p_element->id != WCD_MBIAS_MGR_9_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_MBIAS_MGR_DISABLE_IB(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event MBIAS_MGR_9 DISABLE_INTERNAL_BIAS. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_ADC_5_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event ADC_5 ENABLE.> \n"); 
	 if (p_element->id != WCD_ADC_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_5 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_ADC_ENABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_5 ENABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_5 ENABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_ADC_5_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event ADC_5 DISABLE.> \n"); 
	 if (p_element->id != WCD_ADC_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_5 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_ADC_DISABLE(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_5 DISABLE. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_5 DISABLE. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}
wcd_result wcd_ADC_5_SET_ANALOG_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr) { 
	 wcd_result return_code = E_WCD_SUCCESS; 
	 WCD_DEBUG_INFO_0("+WCD_93XX: <Generated Event ADC_5 SET_ANALOG_GAIN.> \n"); 
	 if (p_element->id != WCD_ADC_5_ID) { 
		 return_code = E_WCD_INVALID_HANDLE; 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_5 SET_ANALOG_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 
	 return_code = WCDH_ADC_SET_ANALOG_GAIN(p_element, parameter, p_element_list, context_ptr); 
	 if (return_code != E_WCD_SUCCESS) { 
		 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_5 SET_ANALOG_GAIN. RC: %d.> \n", return_code); 
		 return return_code; 
	 } 

	 WCD_DEBUG_INFO_1("-WCD_93XX: <Generated Event ADC_5 SET_ANALOG_GAIN. RC: %d.> \n", return_code); 
	 HANDLE_UNUSED_PARAMETER(p_element); 
	 HANDLE_UNUSED_PARAMETER(parameter); 
	 HANDLE_UNUSED_PARAMETER(p_element_list); 
	 HANDLE_UNUSED_PARAMETER(context_ptr); 
	 return return_code; 
}

