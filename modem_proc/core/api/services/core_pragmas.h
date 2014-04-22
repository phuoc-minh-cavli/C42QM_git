#ifndef _CORE_PRAGMAS_H
#define _CORE_PRAGMAS_H
/*=============================================================================
                core_pragmas.h

GENERAL DESCRIPTION
  Common set of Pre-processor directives / pragmas used across Core
  Platform software.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2018  by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================*/

/* Below line may be un-commented to globally disable the generation of
   deprecated-declarations warning. */
//#define CORE_DISABLE_WARN_DEPRECATED

/**
INTERNAL Compiler Pragma Helpers
*/
#define  CORE_DISABLE_PRAGMA_I /**< PL Specialization Tuneable */
#undef   CORE_DISABLE_PRAGMA_W /**< PL Specialization Tuneable */
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
  #define CORE_GCC_HELPER_PRAGMA(x)           _Pragma( #x )
  #if !defined(CORE_DISABLE_PRAGMA_I)
    #define CORE_GCC_HELPER_PRAGMA_I(x)       CORE_GCC_HELPER_PRAGMA(GCC diagnostic ignored #x)
  #else
    #define CORE_GCC_HELPER_PRAGMA_I(x)       /*NULL*/
  #endif
  #if !defined(CORE_DISABLE_PRAGMA_W)
    #define CORE_GCC_HELPER_PRAGMA_W(x)       CORE_GCC_HELPER_PRAGMA(GCC diagnostic warning #x)
  #else
    #define CORE_GCC_HELPER_PRAGMA_W(x)       /*NULL*/
  #endif

  #define CORE_GCC_HELPER_PRAGMA_IW(x)        CORE_GCC_HELPER_PRAGMA_I(x) CORE_GCC_HELPER_PRAGMA_W(x)
  #define CORE_GCC_HELPER_WPUSH               CORE_GCC_HELPER_PRAGMA(GCC diagnostic push)
  #define CORE_GCC_HELPER_WPOP                CORE_GCC_HELPER_PRAGMA(GCC diagnostic pop)
#endif


/**
Compiler to Warn on Use of Deprecated Declarations

CORE_WARN_DEPRECATED [...]
int function_signature(void*, void*, int);

*/
#define CORE_WARN_DEPRECATED_ATTR             __attribute__((deprecated))
#if !defined(CORE_DISABLE_WARN_DEPRECATED)
  #if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
    CORE_GCC_HELPER_PRAGMA_IW(-Wdeprecated-declarations)
    #define CORE_WARN_DEPRECATED              CORE_WARN_DEPRECATED_ATTR
  #elif defined(__ARMCC_VERSION)
    #pragma diag_warning 1361
    #define CORE_WARN_DEPRECATED              CORE_WARN_DEPRECATED_ATTR
  #else
    #define CORE_WARN_DEPRECATED              /*NULL*/
  #endif
#else
  #define CORE_WARN_DEPRECATED                /*NULL*/
#endif

/**
Compiler to Warn on Use of Deprecated Declarations with messages

CORE_WARN_DEPRECATED_MSG(x,y) [...]
int function_signature(void*, void*, int);

*/
#define CORE_WARN_DEPRECATED_MSG_ATTR(x,y)    __attribute__((deprecated(x,y)))
#if !defined(CORE_DISABLE_WARN_DEPRECATED)
  #if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
    CORE_GCC_HELPER_PRAGMA_IW(-Wdeprecated-declarations)
    #define CORE_WARN_DEPRECATED_MSG(x,y)     CORE_WARN_DEPRECATED_MSG_ATTR(x,y)
  #elif defined(__ARMCC_VERSION)
    #pragma diag_warning 1361
    #define CORE_WARN_DEPRECATED_MSG(x,y)     CORE_WARN_DEPRECATED_MSG_ATTR(x,y)
  #else
    #define CORE_WARN_DEPRECATED_MSG(x,y)     /*NULL*/
  #endif
#else
  #define CORE_WARN_DEPRECATED_MSG(x,y)       /*NULL*/
#endif

#endif /* _CORE_PRAGMAS_H */