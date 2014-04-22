
/*
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc. 

 * All Rights Reserved. 

 * Qualcomm Technologies, Inc. Confidential and Proprietary. 

  $Header: //components/rel/audio.tx/2.0/apr/api/apr_pack_end.h#1 $
  $Author: pwbldsvc $
*/

#if defined( __GNUC__ )
  __attribute__((packed))
#elif defined( __arm__ ) || defined( __arm )
#elif defined( _MSC_VER )
  #pragma pack( pop )
#else
  #error Unsupported compiler.
#endif /* __GNUC__ */

