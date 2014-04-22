#ifndef __ECM_LOG_H__
#define __ECM_LOG_H__
#include "DALStdDef.h"
typedef enum 
{
    ECM_LOG_DEBUG = 0,
    ECM_LOG_INFO  = 1,
    ECM_LOG_ERROR = 2,
    ECM_LOG_CRITICAL = 3,
}ecm_log_lvl;

extern ecm_log_lvl master_log_lvl;


#define ecm_log(lvl,fmt) \
  do{                                                      \
   if (lvl > master_log_lvl )                              \
   {                                                       \
      switch (lvl)                                         \
      {                                                      \
        case 3:                                               \
        case 2:                                               \
			MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, fmt);     \
        break;                                                \
        case 1:                                               \
            MSG(MSG_SSID_DFLT, MSG_LEGACY_MED, fmt);     \
       break;                                                \
        case 0:                                               \
            MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, fmt);     \
       break;                                                \
     default:                                               \
	   break;                                                 \
     }                                                        \
   }                                                          \
   }while(0)                                    \


#define ecm_log_1(lvl,fmt,arg1) \
		  do{													   \
		   if (lvl > master_log_lvl )							   \
		   {													   \
			  switch (lvl)										   \
			  { 													 \
				case 3: 											  \
				case 2: 											  \
					MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, fmt,arg1);	   \
				break;												  \
				case 1: 											  \
					MSG_1(MSG_SSID_DFLT, MSG_LEGACY_MED, fmt,arg1);	 \
			   break;												 \
				case 0: 											  \
					MSG_1(MSG_SSID_DFLT, MSG_LEGACY_LOW, fmt,arg1);	 \
			   break;												 \
			 default:												\
			   break;												  \
			 }														  \
		   }														  \
		   }while(0)									\
		   

#endif
