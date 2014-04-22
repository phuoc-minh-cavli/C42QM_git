
/*
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc. 

 * All Rights Reserved. 

 * Qualcomm Technologies, Inc. Confidential and Proprietary. 

  $Header: //components/rel/audio.tx/2.0/apr/api/apr_pack_begin.h#1 $
  $Author: pwbldsvc $
*/

#if defined( __GNUC__ )
#elif defined( __arm__ ) || defined( __arm )
  __packed
#elif defined( _MSC_VER )
  #pragma warning( disable:4103 )  /* Another header changing "pack". */
  #pragma pack( push, 1 )
#else
  #error Unsupported compiler.
#endif /* __GNUC__ */

