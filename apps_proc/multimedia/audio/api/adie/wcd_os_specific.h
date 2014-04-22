#ifndef WCD_OS_SPECIFIC_MSG
#define WCD_OS_SPECIFIC_MSG

#include "msg.h"
#include "DALSys.h"

#define WCD_LOGGING

//Infinite definition for waiting for event/mutex etc.
#define WCD_INFINITE 0xFFFFFFFF

#define HANDLE_UNUSED_PARAMETER(parameter) 

#define WCD_PRIO_LOW      (MSG_MASK_0)
#define WCD_PRIO_MEDIUM   (MSG_MASK_1)
#define WCD_PRIO_HIGH     (MSG_MASK_2)
#define WCD_PRIO_ERROR    (MSG_MASK_3)
#define WCD_PRIO_FATAL    (MSG_MASK_4)
#define WCD_PRIO_DEBUG    (MSG_MASK_5)


#ifdef WCD_LOGGING

#ifdef MSG_SSID_ADIE
  #define WCD_DEBUG_INFO_0(msg)    MSG ( MSG_SSID_ADIE, WCD_PRIO_MEDIUM, msg );
  #define WCD_DEBUG_INFO_1(msg,param_one)   MSG_1( MSG_SSID_ADIE, WCD_PRIO_MEDIUM, msg, param_one );
  #define WCD_DEBUG_INFO_2(msg,param_one,param_two)   MSG_2( MSG_SSID_ADIE, WCD_PRIO_MEDIUM, msg, param_one, param_two );
  #define WCD_DEBUG_INFO_3(msg,param_one,param_two,param_three)   MSG_3( MSG_SSID_ADIE, WCD_PRIO_MEDIUM, msg, param_one, param_two, param_three );
  #define WCD_DEBUG_WARN_0(msg)    MSG ( MSG_SSID_ADIE, WCD_PRIO_MEDIUM, msg );
  #define WCD_DEBUG_WARN_1(msg,param_one)   MSG_1( MSG_SSID_ADIE, WCD_PRIO_MEDIUM, msg, param_one );
  #define WCD_DEBUG_WARN_2(msg,param_one,param_two)   MSG_2( MSG_SSID_ADIE, WCD_PRIO_MEDIUM, msg, param_one, param_two );
  #define WCD_DEBUG_WARN_3(msg,param_one,param_two,param_three)   MSG_3( MSG_SSID_ADIE, WCD_PRIO_MEDIUM, msg, param_one, param_two, param_three );
  #define WCD_DEBUG_ERROR_0(msg)    MSG ( MSG_SSID_ADIE, WCD_PRIO_ERROR, msg );
  #define WCD_DEBUG_ERROR_1(msg,param_one)   MSG_1( MSG_SSID_ADIE, WCD_PRIO_ERROR, msg, param_one );
  #define WCD_DEBUG_ERROR_2(msg,param_one,param_two)   MSG_2( MSG_SSID_ADIE, WCD_PRIO_ERROR, msg, param_one, param_two );
  #define WCD_DEBUG_ERROR_3(msg,param_one,param_two,param_three)   MSG_3( MSG_SSID_ADIE, WCD_PRIO_ERROR, msg, param_one, param_two, param_three );
#else //MSG_SSID_ADIE not defined 
  #define WCD_DEBUG_INFO_0(msg)    MSG ( MSG_SSID_APPS_CAD_GENERAL, WCD_PRIO_MEDIUM, msg );
  #define WCD_DEBUG_INFO_1(msg,param_one)   MSG_1( MSG_SSID_APPS_CAD_GENERAL, WCD_PRIO_MEDIUM, msg, param_one );
  #define WCD_DEBUG_INFO_2(msg,param_one,param_two)   MSG_2( MSG_SSID_APPS_CAD_GENERAL, WCD_PRIO_MEDIUM, msg, param_one, param_two );
  #define WCD_DEBUG_INFO_3(msg,param_one,param_two,param_three)   MSG_3( MSG_SSID_APPS_CAD_GENERAL, WCD_PRIO_MEDIUM, msg, param_one, param_two, param_three );
  #define WCD_DEBUG_WARN_0(msg)    MSG ( MSG_SSID_APPS_CAD_GENERAL, WCD_PRIO_MEDIUM, msg );
  #define WCD_DEBUG_WARN_1(msg,param_one)   MSG_1( MSG_SSID_APPS_CAD_GENERAL, WCD_PRIO_MEDIUM, msg, param_one );
  #define WCD_DEBUG_WARN_2(msg,param_one,param_two)   MSG_2( MSG_SSID_APPS_CAD_GENERAL, WCD_PRIO_MEDIUM, msg, param_one, param_two );
  #define WCD_DEBUG_WARN_3(msg,param_one,param_two,param_three)   MSG_3( MSG_SSID_APPS_CAD_GENERAL, WCD_PRIO_MEDIUM, msg, param_one, param_two, param_three );
  #define WCD_DEBUG_ERROR_0(msg)    MSG ( MSG_SSID_APPS_CAD_GENERAL, WCD_PRIO_ERROR, msg );
  #define WCD_DEBUG_ERROR_1(msg,param_one)   MSG_1( MSG_SSID_APPS_CAD_GENERAL, WCD_PRIO_ERROR, msg, param_one );
  #define WCD_DEBUG_ERROR_2(msg,param_one,param_two)   MSG_2( MSG_SSID_APPS_CAD_GENERAL, WCD_PRIO_ERROR, msg, param_one, param_two );
  #define WCD_DEBUG_ERROR_3(msg,param_one,param_two,param_three)   MSG_3( MSG_SSID_APPS_CAD_GENERAL, WCD_PRIO_ERROR, msg, param_one, param_two, param_three );
#endif //MSG_SSID_ADIE

#else

#define WCD_DEBUG_INFO_0(msg)
#define WCD_DEBUG_INFO_1(msg,param_one)
#define WCD_DEBUG_INFO_2(msg,param_one,param_two)
#define WCD_DEBUG_INFO_3(msg,param_one,param_two,param_three)

#define WCD_DEBUG_WARN_0(msg)    
#define WCD_DEBUG_WARN_1(msg,param_one) 
#define WCD_DEBUG_WARN_2(msg,param_one,param_two) 
#define WCD_DEBUG_WARN_3(msg,param_one,param_two,param_three)

#define WCD_DEBUG_ERROR_0(msg)    
#define WCD_DEBUG_ERROR_1(msg,param_one)
#define WCD_DEBUG_ERROR_2(msg,param_one,param_two) 
#define WCD_DEBUG_ERROR_3(msg,param_one,param_two,param_three)

#endif

#ifdef WCD_CORE_PAGED_CODE_SUPPORT // Macro to allow HLOS code to selectively enable/disable WCD-CORE paged code feature

    //  Macros to tag different code and data sections to reside in paged memory

    //  Macro to make functions pageable
    #define WCD_CORE_PAGED_CODE_FUNCTION(fn)                  

    //  Macros to make certain sections of code & data pageable
    #define WCD_CORE_PAGED_CODE_SEGMENT_START                 
    #define WCD_CORE_PAGED_CODE_SEGMENT_STOP                  

    //  Macros to make constant global variables pageable
    //  Example: static const int foo[x] = {z};
    #define WCD_CORE_PAGED_CODE_CONST_START                   
    #define WCD_CORE_PAGED_CODE_CONST_STOP                    

    //  Macros to make non-constant initialized global variables pageable
    //  Example: int foo[x] = {z};
    #define WCD_CORE_PAGED_CODE_DATA_START                    
    #define WCD_CORE_PAGED_CODE_DATA_STOP                     

    //  Macros to make non-constant uninitialized global variables pageable (BSS: block started by symbol)
    //  Example: int foo[x];
    #define WCD_CORE_PAGED_CODE_BSS_START                     
    #define WCD_CORE_PAGED_CODE_BSS_STOP                      

    //  Macros to make functions pageable that are used only during initialization/bootup, discardable memory segment
    #define WCD_CORE_PAGED_CODE_INIT_MEMORY_FUNCTION(fn)      

    #define WCD_CORE_PAGED_CODE

#else	

    //  Macros to tag different code and data sections to reside in paged memory

    //  Macro to make functions pageable
    #define WCD_CORE_PAGED_CODE_FUNCTION(fn)                  

    //  Macros to make certain sections of code & data pageable
    #define WCD_CORE_PAGED_CODE_SEGMENT_START                 
    #define WCD_CORE_PAGED_CODE_SEGMENT_STOP                  

    //  Macros to make constant global variables pageable
    //  Example: static const int foo[x] = {z};
    #define WCD_CORE_PAGED_CODE_CONST_START                   
    #define WCD_CORE_PAGED_CODE_CONST_STOP                    

    //  Macros to make non-constant initialized global variables pageable
    //  Example: int foo[x] = {z};
    #define WCD_CORE_PAGED_CODE_DATA_START                    
    #define WCD_CORE_PAGED_CODE_DATA_STOP                     

    //  Macros to make non-constant uninitialized global variables pageable (BSS: block started by symbol)
    //  Example: int foo[x];
    #define WCD_CORE_PAGED_CODE_BSS_START                     
    #define WCD_CORE_PAGED_CODE_BSS_STOP                      

    //  Macros to make functions pageable that are used only during initialization/bootup, discardable memory segment
    #define WCD_CORE_PAGED_CODE_INIT_MEMORY_FUNCTION(fn)      

    #define WCD_CORE_PAGED_CODE

#endif


#endif //WCD_OS_SPECIFIC_MSG


