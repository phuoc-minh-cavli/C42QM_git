/*
   DESCRIPTION
   This file contains definitions for processing packets in voice adapter
   DSM pools.

   Copyright (C) 2015, 2017-2018 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/utils/src/voice_amr.c#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

/* CORE APIs. */
#include "comdef.h"
#include "msg.h"
#include "err.h"

/* APR APIs. */
#include "apr_errcodes.h"

/* VSD APIs. */
#include "voice_amr_if.h"
#include "voice_dsm_if.h"

/****************************************************************************
  VOICE AMR INTERNAL DEFINATION
****************************************************************************/

/**
 * Buffer allocation of AMR core speech data
 */
typedef struct voice_amr_speech_buffer_t {

  uint8 data_a[ AMRSUP_CLASS_A_BYTES ];
  uint8 data_b[ AMRSUP_CLASS_B_BYTES ];
  uint8 data_c[ AMRSUP_CLASS_C_BYTES ];

} voice_amr_speech_buffer_t;


/**
 * Voice AMR frame info.
 */
typedef struct voice_amr_frame_info_t {

  amrsup_frame_type frame_type;
  amrsup_mode_type mode;

} voice_amr_frame_info_t;


/****************************************************************************
 * VOICE DSM GLOBALs                                                        *
 ****************************************************************************/

static uint8 voice_amr_mask_array[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

/* Frame type mapping for AMR-NB vocoder over the downlink path,
 * from AMRSUP to VS.
 */
static vs_vocamr_frame_type_t amrsup_to_vs_frame_type[ AMRSUP_FRAME_TYPE_MAX ] = 
{
  VS_VOCAMR_FRAME_TYPE_SPEECH_GOOD,     /* AMRSUP_SPEECH_GOOD */
  VS_VOCAMR_FRAME_TYPE_SPEECH_DEGRADED, /* AMRSUP_SPEECH_DEGRADED */
  VS_VOCAMR_FRAME_TYPE_ONSET,           /* AMRSUP_ONSET */
  VS_VOCAMR_FRAME_TYPE_SPEECH_BAD,      /* AMRSUP_SPEECH_BAD */
  VS_VOCAMR_FRAME_TYPE_SID_FIRST,       /* AMRSUP_SID_FIRST */
  VS_VOCAMR_FRAME_TYPE_SID_UPDATE,      /* AMRSUP_SID_UPDATE */
  VS_VOCAMR_FRAME_TYPE_SID_BAD,         /* AMRSUP_SID_BAD */
  VS_VOCAMR_FRAME_TYPE_NO_DATA,         /* AMRSUP_NO_DATA */
  VS_VOCAMR_FRAME_TYPE_NO_DATA          /* AMRSUP_SPEECH_LOST */ 
};


/* Frame type mapping for AMR-NB vocoder over the uplink path,
 * from VS to AMRSUP.
 */
static amrsup_frame_type vs_to_amrsup_frame_type[ VS_VOCAMR_FRAME_TYPE_NO_DATA + 1 
] =
{
  AMRSUP_SPEECH_GOOD,     /* VS_VOCAMR_FRAME_TYPE_SPEECH_GOOD */
  AMRSUP_SPEECH_DEGRADED, /* VS_VOCAMR_FRAME_TYPE_SPEECH_DEGRADED */
  AMRSUP_ONSET,           /* VS_VOCAMR_FRAME_TYPE_ONSET */
  AMRSUP_SPEECH_BAD,      /* VS_VOCAMR_FRAME_TYPE_SPEECH_BAD */
  AMRSUP_SID_FIRST,       /* VS_VOCAMR_FRAME_TYPE_SID_FIRST */
  AMRSUP_SID_UPDATE,      /* VS_VOCAMR_FRAME_TYPE_SID_UPDATE */
  AMRSUP_SID_BAD,         /* VS_VOCAMR_FRAME_TYPE_SID_BAD */
  AMRSUP_NO_DATA          /* VS_VOCAMR_FRAME_TYPE_NO_DATA */
};

/* Frame type mapping for AMR-WB vocoder over the downlink path,
 * from MVS to VS.
 */
static vs_vocamrwb_frame_type_t amrsup_to_vs_armwb_frame_type[ AMRSUP_FRAME_TYPE_MAX ] =
{
  VS_VOCAMRWB_FRAME_TYPE_SPEECH_GOOD,              /* AMRSUP_SPEECH_GOOD */
  VS_VOCAMRWB_FRAME_TYPE_SPEECH_PROBABLY_DEGRADED, /* AMRSUP_SPEECH_DEGRADED */
  VS_VOCAMRWB_FRAME_TYPE_NO_DATA,                  /* AMRSUP_ONSET */
  VS_VOCAMRWB_FRAME_TYPE_SPEECH_BAD,               /* AMRSUP_SPEECH_BAD */
  VS_VOCAMRWB_FRAME_TYPE_SID_FIRST,                /* AMRSUP_SID_FIRST */
  VS_VOCAMRWB_FRAME_TYPE_SID_UPDATE,               /* AMRSUP_SID_UPDATE */
  VS_VOCAMRWB_FRAME_TYPE_SID_BAD,                  /* AMRSUP_SID_BAD */
  VS_VOCAMRWB_FRAME_TYPE_NO_DATA,                  /* AMRSUP_NO_DATA */
  VS_VOCAMRWB_FRAME_TYPE_SPEECH_LOST               /* AMRSUP_SPEECH_LOST */
};

/* Frame type mapping for AMR-WB vocoder over the uplink path,
 * from VS to MVS.
 */
static amrsup_frame_type vs_to_amrsup_amrwb_frame_type[ VS_VOCAMRWB_FRAME_TYPE_NO_DATA + 1 ] = 
{
  AMRSUP_SPEECH_GOOD,     /* VS_VOCAMRWB_FRAME_TYPE_SPEECH_GOOD */
  AMRSUP_SPEECH_DEGRADED, /* VS_VOCAMRWB_FRAME_TYPE_SPEECH_PROBABLY_DEGRADED */
  AMRSUP_SPEECH_LOST,     /* VS_VOCAMRWB_FRAME_TYPE_SPEECH_LOST */
  AMRSUP_SPEECH_BAD,      /* VS_VOCAMRWB_FRAME_TYPE_SPEECH_BAD */
  AMRSUP_SID_FIRST,       /* VS_VOCAMRWB_FRAME_TYPE_SID_FIRST */
  AMRSUP_SID_UPDATE,      /* VS_VOCAMRWB_FRAME_TYPE_SID_UPDATE */
  AMRSUP_SID_BAD,         /* VS_VOCAMRWB_FRAME_TYPE_SID_BAD */
  AMRSUP_NO_DATA          /* VS_VOCAMRWB_FRAME_TYPE_NO_DATA */
};


/****************************************************************************
 * VOICE AMR UTILITY INTERNAL ROUTINES                                      *
 ****************************************************************************/

static bool_t voice_amr_is_subset_chan_state (
  voice_amr_chan_state_t *chan_state1,
  voice_amr_chan_state_t *chan_state2
)
{
  if ((chan_state1==NULL) || (chan_state2==NULL))
  {
    return (chan_state1==chan_state2);
  }
 
  if ((chan_state1->has_chan_a == TRUE) && (chan_state2->has_chan_a == FALSE))
  {
    return FALSE;
  }
  
  if ((chan_state1->has_chan_b == TRUE) && (chan_state2->has_chan_b == FALSE))
  {
    return FALSE;
  }
  
  if ((chan_state1->has_chan_c == TRUE) && (chan_state2->has_chan_c == FALSE))
  {
    return FALSE;
  }
  
  return TRUE;
}


static amrsup_codec_type vs_to_amrsup_codec_type (
  uint32_t vs_media_id
)
{
  amrsup_codec_type amrsup_codec = AMRSUP_CODEC_MAX;

  switch ( vs_media_id )
  {
   case VS_VOCEAMR_MEDIA_ID:
   case VS_VOCAMR_MEDIA_ID:
     amrsup_codec = AMRSUP_CODEC_AMR_NB;
     break;

   case VS_VOCAMRWB_MEDIA_ID:
     amrsup_codec = AMRSUP_CODEC_AMR_WB;
     break;

   default:
     break;
  }

  return amrsup_codec;
}

static void voice_amr_init_core_speech (
  amrsup_core_speech_type*   amr_speech,
  voice_amr_speech_buffer_t* speech_buf
)
{
  if ((amr_speech==NULL) || (speech_buf==NULL))
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "voice_amr_init_core_speech(): Speech init failed." );
    return;
  }

  amr_speech->num_bits_a = 0;
  amr_speech->num_bits_b = 0;
  amr_speech->num_bits_c = 0;

  amr_speech->data_a = speech_buf->data_a;
  amr_speech->data_b = speech_buf->data_b;
  amr_speech->data_c = speech_buf->data_c;

  return;
}


static uint32_t voice_amr_frame_len (
  amrsup_frame_type frame_type,
  amrsup_mode_type amr_mode
)
{
  if ( ( frame_type >= AMRSUP_FRAME_TYPE_MAX ) ||
       ( amr_mode >= AMRSUP_MODE_MAX ) )
  {
    return 0;
  }

  return amrsup_frame_len( frame_type, amr_mode );

}

#if 0
static uint32_t voice_amr_frame_len_bits (
 amrsup_frame_type frame_type,
 amrsup_mode_type amr_mode
)
{
  if ( ( frame_type >= AMRSUP_FRAME_TYPE_MAX ) ||
       ( amr_mode >= AMRSUP_MODE_MAX ) )
  {
    return 0;
  }
 
  return amrsup_frame_len_bits( frame_type, amr_mode );
}
#endif

/**
 * Extract the amr frame info from VS buffers.
 */
static uint32_t voice_amr_extract_amr_info ( 
  voice_amr_frame_info_t* amr_info,
  uint8_t* frame,
  uint32_t vs_media_id
)
{
  uint32_t rc = APR_EOK;
  amrsup_mode_type mode;
  amrsup_frame_type frametype;
  vs_vocamr_frame_header_t* amrFrameHeader = NULL;
  vs_vocamrwb_frame_header_t* amrwbFrameHeader = NULL;

  switch ( vs_media_id )
  {
    case VS_VOCAMR_MEDIA_ID:
    case VS_VOCEAMR_MEDIA_ID:
    {
      amrFrameHeader = ( vs_vocamr_frame_header_t*) &frame[0];
      frametype = vs_to_amrsup_frame_type[amrFrameHeader->frame_type];
      mode = ( amrsup_mode_type ) amrFrameHeader->codec_mode;

      /* Validate mode and frame type populated by VS. */
      if ( ( mode < AMRSUP_MODE_0475 ) || ( mode > AMRSUP_MODE_1220 ) ||
           ( frametype < AMRSUP_SPEECH_GOOD ) || ( frametype > AMRSUP_SPEECH_LOST ) )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "voice_amr_extract_amr_info():Invalid (E)AMR frame info: "
               "mode (%d), frametype (%d)",  mode, frametype );
        return APR_EBADPARAM;
      }

      amr_info->frame_type = frametype;
      amr_info->mode = mode;
    }
    break;

    case VS_VOCAMRWB_MEDIA_ID:
    {
      amrwbFrameHeader = ( vs_vocamrwb_frame_header_t*) &frame[0];
      frametype = vs_to_amrsup_amrwb_frame_type[ amrwbFrameHeader->frame_type ];
      mode = ( amrsup_mode_type ) amrwbFrameHeader->codec_mode + AMRSUP_MODE_0660;

      /* Validate mode and frame type populated by VS. */
      if ( ( mode < AMRSUP_MODE_0660 ) || ( mode > AMRSUP_MODE_2385 ) ||
           ( frametype < AMRSUP_SPEECH_GOOD || frametype > AMRSUP_SPEECH_LOST ) )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "voice_amr_extract_amr_info():Invalid AMR-WB frame info: "
               "mode (%d), frametype (%d)",  mode, frametype );
        return APR_EBADPARAM;
      }

      amr_info->frame_type = frametype;
      amr_info->mode = mode;
    }
    break;

   default:
     rc = APR_EBADPARAM;
     break;
  }

  return rc;
}


/**
 * Update the amr frame info to VS buffers.
 */
static uint32_t voice_amr_update_amr_info( 
  voice_amr_frame_info_t* amr_info,
  vs_pktexg_buffer_t* vs_buffer 
)
{
  uint32_t rc = APR_EOK;
  amrsup_codec_type amr_codec;
  amrsup_mode_type mode;
  amrsup_frame_type frametype;

  uint8_t* frame = NULL;
  vs_vocamr_frame_header_t* amrFrameHeader = NULL;
  vs_vocamrwb_frame_header_t* amrwbFrameHeader = NULL;

  amr_codec = vs_to_amrsup_codec_type ( vs_buffer->media_id );
  if ( amr_codec ==  AMRSUP_CODEC_MAX ) return APR_EBADPARAM;

  frame = ( (uint8_t*)vs_buffer ) + sizeof( vs_pktexg_buffer_t );
  switch ( amr_codec )
  {
   case AMRSUP_CODEC_AMR_NB:
    {
      frametype = amr_info->frame_type;
      mode = amr_info->mode;
      /* Validate mode and frame type populated by VS. */
      if ( ( mode < AMRSUP_MODE_0475 ) || ( mode > AMRSUP_MODE_1220 ) ||
           ( frametype < AMRSUP_SPEECH_GOOD ) || ( frametype > AMRSUP_SPEECH_LOST ) )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "voice_amr_update_amr_info():Invalid AMR-NB frame info: "
               "mode (%d), frametype (%d)",  mode, frametype );
        return APR_EBADPARAM;
      }

      /* voice_amr_frame_len() requires frametype and codec mode in amrsup format */
      vs_buffer->frame_size = voice_amr_frame_len ( frametype, mode ) + 1;
      amrFrameHeader = ( vs_vocamr_frame_header_t*) frame;
      amrFrameHeader->frame_type = amrsup_to_vs_frame_type[ frametype ];
      amrFrameHeader->codec_mode = ( vs_vocamr_codec_mode_t ) mode;
    }
    break;

   case AMRSUP_CODEC_AMR_WB:
    {
      frametype = amr_info->frame_type;
      mode = amr_info->mode;
      /* Validate mode and frame type populated by VS. */
      if ( ( mode < AMRSUP_MODE_0660 ) || ( mode > AMRSUP_MODE_2385 ) ||
           ( frametype < AMRSUP_SPEECH_GOOD || frametype > AMRSUP_SPEECH_LOST ) )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "voice_amr_update_amr_info():Invalid AMR-WB frame info: "
               "mode (%d), frametype (%d)",  mode, frametype );
        return APR_EBADPARAM;
      }

      /* voice_amr_frame_len() requires frametype and codec mode in amrsup format */
      vs_buffer->frame_size = voice_amr_frame_len ( frametype, mode ) + 1;
      mode -= AMRSUP_MODE_0660; //offsetting the codec mode for amrwb to 0
      amrwbFrameHeader = ( vs_vocamrwb_frame_header_t*) frame;
      amrwbFrameHeader->frame_type = amrsup_to_vs_armwb_frame_type[frametype];
      amrwbFrameHeader->codec_mode = ( vs_vocamrwb_codec_mode_t ) mode;
    }
    break;

   default:
     rc = APR_EBADPARAM;
     break;
  }

  return rc;
}

#ifndef WINSIM
static int32_t voice_amr_extract_channels_from_if1 (
  uint8 *AMRIF1Packet,
  amrsup_core_speech_type *speech_data
)
{
  int nResult = APR_EOK;
  int bitsReadForClass ;
  uint8* pbyDest  ;
  int32 CurrbitRead ;
  int32 CurrBitWrite ;
  int32 CurrByteRead ;
  int32 CurrByteWrite ;
  uint8* pbySrc ;
  uint8 pbyDesC ;
  uint8 CurrByte;
  uint8 CurrBit;
  int writeBitPos;

  if ( AMRIF1Packet == NULL )
  {
    return APR_EBADPARAM;
  }

  //uint32 ClassABitsOffset = AmrFrameInfo.AMRCoreDataOffset ;
  memset((void*)(speech_data->data_a), 0, AMRSUP_CLASS_A_BYTES);
  memset((void*)(speech_data->data_b), 0, AMRSUP_CLASS_B_BYTES);
  memset((void*)(speech_data->data_c), 0, AMRSUP_CLASS_C_BYTES);

  bitsReadForClass = 0 ;
  pbyDest = speech_data->data_a ;
  CurrbitRead = 0 ;
  CurrBitWrite = 0 ;
  CurrByteRead = 0 ;
  CurrByteWrite = 0 ;
  pbySrc = NULL;
  pbySrc = AMRIF1Packet ;
  pbyDesC = 0x0;

  while ( bitsReadForClass < speech_data->num_bits_a )
  {
    CurrbitRead = 0 ;
    CurrBitWrite = 0 ;

    CurrByte = *(pbySrc + CurrByteRead) ;
    //munch 8 bits or if u have reached your quota for this Class of bits
    //change
    while ( (CurrbitRead < 8) && (bitsReadForClass < speech_data->num_bits_a) )
    {
      CurrBit = voice_amr_mask_array[7-CurrbitRead] & CurrByte ; // got a bit here
      pbyDesC = pbyDesC | CurrBit ;                             // put it together
      CurrbitRead++ ;
      CurrBitWrite++ ;
      bitsReadForClass++ ;
      if ( CurrBitWrite == 8 )
      {
        *(pbyDest + CurrByteWrite) = pbyDesC ;
        CurrBitWrite = 0 ;
        CurrByteWrite++ ;
        pbyDesC  = 0x0 ;
      }

    }

    if ( CurrBitWrite == 8 )
    {
      *(pbyDest + CurrByteWrite) = pbyDesC ;
      CurrBitWrite = 0 ;
      CurrByteWrite++ ;
      pbyDesC = 0x0;
    }
    if ( CurrbitRead==8 )
    {
      CurrByteRead++ ;
      CurrbitRead = 0 ;
    }


  }

  bitsReadForClass = 0 ;
  *(pbyDest + CurrByteWrite) = pbyDesC ;
  pbyDesC = 0x0 ;
  CurrByteWrite = 0 ; // destination is changed
  CurrBitWrite = 0 ;
  pbyDest = speech_data->data_b ;
  //uint32 pRemain = CurrbitRead ;
  //CurrByteRead is not moved ahead

  while ( bitsReadForClass < speech_data->num_bits_b )
  {

    CurrByte = *(pbySrc + CurrByteRead) ;
    //munch 8 bits or if u have reached your quota for this Class of bits
    while ( (CurrbitRead < 8) && (bitsReadForClass < speech_data->num_bits_b) )
    {
      CurrBit = voice_amr_mask_array[7-CurrbitRead] & CurrByte ;
      writeBitPos = CurrbitRead-CurrBitWrite ;
      if ( writeBitPos>0 )
      {
        CurrBit = CurrBit << (CurrbitRead-CurrBitWrite) ;
      }
      else
      {
        CurrBit = CurrBit >> (CurrBitWrite-CurrbitRead) ;
      }

      pbyDesC = pbyDesC | CurrBit ;
      CurrbitRead++ ;
      CurrBitWrite++ ;
      bitsReadForClass++ ;
      if ( CurrBitWrite == 8 )
      {
        *(pbyDest + CurrByteWrite) = pbyDesC ;
        CurrBitWrite = 0 ;
        CurrByteWrite++ ;
        pbyDesC  = 0x0 ;
      }

    }

    if ( CurrbitRead == 8 )
    {
      CurrbitRead = 0 ;
      CurrByteRead++ ;
    }

    if ( CurrBitWrite == 8 )
    {
      *(pbyDest + CurrByteWrite) = pbyDesC ;
      CurrBitWrite = 0 ;
      CurrByteWrite++ ;
      pbyDesC  = 0x0 ;
    }


  }
  bitsReadForClass = 0 ;
  *(pbyDest + CurrByteWrite) = pbyDesC ;
  pbyDesC = 0x0 ;
  CurrByteWrite = 0 ; // destination is changed
  CurrBitWrite = 0 ;
  pbyDest = speech_data->data_c ;
  //CurrByteRead is not moved ahead

  while ( bitsReadForClass < speech_data->num_bits_c )
  {
    CurrByte = *(pbySrc + CurrByteRead) ;
    //munch 8 bits or if u have reached your quota for this Class of bits
    while ( (CurrbitRead < 8) && (bitsReadForClass < speech_data->num_bits_c) )
    {
      CurrBit = voice_amr_mask_array[7-CurrbitRead] & CurrByte ;
      writeBitPos = CurrbitRead-CurrBitWrite ;
      if ( writeBitPos>0 )
      {
        CurrBit = CurrBit << (CurrbitRead-CurrBitWrite) ;
      }
      else
      {
        CurrBit = CurrBit >> (CurrBitWrite-CurrbitRead) ;
      }

      pbyDesC = pbyDesC | CurrBit ;
      CurrbitRead++ ;
      CurrBitWrite++ ;
      bitsReadForClass++ ;
      if ( CurrBitWrite == 8 )
      {
        *(pbyDest + CurrByteWrite) = pbyDesC ;
        CurrBitWrite = 0 ;
        CurrByteWrite++ ;
        pbyDesC  = 0x0 ;
      }
    }

    if ( CurrbitRead == 8 )
    {
      CurrbitRead = 0 ;
      CurrByteRead++ ;
    }

    if ( CurrBitWrite == 8 )
    {
      *(pbyDest + CurrByteWrite) = pbyDesC ;
      CurrBitWrite = 0 ;
      CurrByteWrite++ ;
      pbyDesC  = 0x0 ;
    }

  }

  *(pbyDest + CurrByteWrite) = pbyDesC ;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "voice_amr_extract_channels_from_if1(): complete (%d)", nResult );

  return nResult;
}


static int32_t voice_amr_make_if1_from_channels (
  uint8 *AMRIF1Packet,
  amrsup_core_speech_type *speech_data,
  int nIF1PacketLength,
  int *nIF1PacketLengthFormed
)
{
  int32_t nResult = APR_EOK;
  int32 CurrByteWrite  = 0 ;
  int32 CurrBitWrite = 0 ;
  uint8 *pbySrc = NULL;
  uint32 bitsReadForClass = 0 ;
  int32 CurrbitRead = 0 ;
  int32 CurrByteRead = 0 ;
  uint8 pbyDesC =0x0;
  uint8* pbyDest = NULL;
  uint8 CurrByte;
  uint8 CurrBit;
  int writeBitPos;

  if ( AMRIF1Packet == NULL )
    return APR_EBADPARAM;

  //Initialize vocoder packet to zero
  memset((void*)(AMRIF1Packet), 0, nIF1PacketLength);

  //Check for NO_DATA frame
  if ( speech_data->num_bits_a == 0 &&
       speech_data->num_bits_b == 0 &&
       speech_data->num_bits_c == 0 )
  {
    if ( nIF1PacketLengthFormed != NULL ) *nIF1PacketLengthFormed = 0;
    return APR_EOK;
  }

  CurrByteWrite  = 0 ;
  CurrBitWrite = 0 ;
  pbySrc = NULL;
  bitsReadForClass = 0 ;
  CurrbitRead = 0 ;
  CurrByteRead = 0 ;
  pbyDesC =0x0;
  pbyDest = NULL;

  if ( speech_data->num_bits_a )
  {
    pbySrc = speech_data->data_a ;

    /* pbyDest = (AMRIF1Packet+3)+CurrByteWrite ; */
    pbyDest = (AMRIF1Packet)+CurrByteWrite ;

    while ( bitsReadForClass < speech_data->num_bits_a )
    {
      CurrByte = *(pbySrc + CurrByteRead) ;
      while ( (CurrbitRead<8)&&(bitsReadForClass < speech_data->num_bits_a) )
      {
        CurrBit = voice_amr_mask_array[7-CurrbitRead] & CurrByte ;

#if 0 /* TODO: Need to re-enable? */
        int writeBitPos = CurrbitRead-CurrBitWrite ;
        if (writeBitPos>0)
        {
          CurrBit = CurrBit >> (CurrbitRead-CurrBitWrite) ;
        }
        else
        {
          CurrBit = CurrBit << (CurrBitWrite-CurrbitRead) ;
        }
#endif /* 0 */

        pbyDesC = pbyDesC | CurrBit ;
        CurrbitRead++ ;
        CurrBitWrite++ ;
        bitsReadForClass++ ;
        if ( CurrBitWrite == 8 )
        {
          *(pbyDest + CurrByteWrite) = pbyDesC ;
          CurrBitWrite = 0 ;
          CurrByteWrite++ ;
          pbyDesC  = 0x0 ;
        }
      }//process 8 bits

      if ( CurrbitRead == 8 )
      {
        CurrbitRead = 0 ;
        CurrByteRead++ ;
      }

      if ( CurrBitWrite == 8 )
      {
        *(pbyDest + CurrByteWrite) = pbyDesC ;
        CurrBitWrite = 0 ;
        CurrByteWrite++ ;
        pbyDesC  = 0x0 ;
      }
    }//for all A bits

  }//processed all A channel bits
  bitsReadForClass = 0 ;

  //    *(pbyDest + CurrByteWrite) = pbyDesC ;
  CurrbitRead = 0 ;
  CurrByteRead = 0 ;
  if ( speech_data->num_bits_b )
  {
    pbySrc = speech_data->data_b ;
    if( speech_data->num_bits_a == 0 )
    {
      pbyDest = AMRIF1Packet;
    }
    //uint8* pbyDest = (AMRIF1Packet+4)+CurrByteWrite ;
    while ( bitsReadForClass < speech_data->num_bits_b )
    {
      CurrByte = *(pbySrc + CurrByteRead) ;
      while ( (CurrbitRead<8)&&(bitsReadForClass < speech_data->num_bits_b) )
      {
        CurrBit = voice_amr_mask_array[7-CurrbitRead] & CurrByte ;
        writeBitPos = CurrBitWrite - CurrbitRead ;
        if ( writeBitPos>0 )
        {
          CurrBit = CurrBit >> (CurrBitWrite-CurrbitRead) ;
        }
        else
        {
          CurrBit = CurrBit << (CurrbitRead - CurrBitWrite ) ;
        }

        //CurrBit = CurrBit << CurrBitWrite ;

        pbyDesC = pbyDesC | CurrBit ;
        CurrbitRead++ ;
        CurrBitWrite++ ;
        bitsReadForClass++ ;
        if ( CurrBitWrite == 8 )
        {
          *(pbyDest + CurrByteWrite) = pbyDesC ;
          CurrBitWrite = 0 ;
          CurrByteWrite++ ;
          pbyDesC  = 0x0 ;
        }
      }//process 8 bits

      if ( CurrbitRead == 8 )
      {
        CurrbitRead = 0 ;
        CurrByteRead++ ;
      }

      if ( CurrBitWrite == 8 )
      {
        *(pbyDest + CurrByteWrite) = pbyDesC ;
        CurrBitWrite = 0 ;
        CurrByteWrite++ ;
        pbyDesC  = 0x0 ;
      }
    }//for all B bits

  }//processed all B channel bits
  bitsReadForClass = 0 ;
  CurrbitRead = 0 ;
  CurrByteRead = 0 ;
  if ( speech_data->num_bits_c )
  {
    if( ( speech_data->num_bits_a == 0 ) && ( speech_data->num_bits_b == 0 ) )
    {
      pbyDest = AMRIF1Packet;
    }
    pbySrc = speech_data->data_c ;
    //uint8* pbyDest = (AMRIF1Packet+4)+CurrByteWrite ;
    while ( bitsReadForClass < speech_data->num_bits_c )
    {
      CurrByte = *(pbySrc + CurrByteRead) ;
      while ( (CurrbitRead<8)&&(bitsReadForClass < speech_data->num_bits_c) )
      {
        CurrBit = voice_amr_mask_array[7-CurrbitRead] & CurrByte ;
        writeBitPos = CurrBitWrite - CurrbitRead ;
        if ( writeBitPos>0 )
        {
          CurrBit = CurrBit >> (CurrBitWrite - CurrbitRead) ;
        }
        else
        {
          CurrBit = CurrBit << (CurrbitRead - CurrBitWrite ) ;
        }

        //CurrBit = CurrBit << CurrBitWrite ;

        pbyDesC = pbyDesC | CurrBit ;
        CurrbitRead++ ;
        CurrBitWrite++ ;
        bitsReadForClass++ ;
        if ( CurrBitWrite == 8 )
        {
          *(pbyDest + CurrByteWrite) = pbyDesC ;
          CurrBitWrite = 0 ;
          CurrByteWrite++ ;
          pbyDesC  = 0x0 ;
        }
      }//process 8 bits

      if ( CurrbitRead == 8 )
      {
        CurrbitRead = 0 ;
        CurrByteRead++ ;
      }

      if ( CurrBitWrite == 8 )
      {
        *(pbyDest + CurrByteWrite) = pbyDesC ;
        CurrBitWrite = 0 ;
        CurrByteWrite++ ;
        pbyDesC  = 0x0 ;
      }
    }//for all c bits

  }//processed all c channel bits

  *(pbyDest + CurrByteWrite) = pbyDesC ;
  if ( nIF1PacketLengthFormed != NULL ) *nIF1PacketLengthFormed = CurrByteWrite + 1;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "voice_amr_make_if1_from_channels(): complete (%d)",
        nResult );

  return nResult ;
}

#endif /* !WINSIM */


uint32_t voice_amr_ul_processing (
  uint32_t vs_media_id,
  uint8_t* frame,
  voice_amr_chan_state_t* ul_chan_state,
  voice_amr_dsm_queue_t* amr_dsm_q
)
{
  uint32_t rc = APR_EOK;
  amrsup_core_speech_type amr_speech;
  voice_amr_speech_buffer_t speech_data;
  voice_amr_frame_info_t amr_info;
  voice_amr_chan_state_t chan_state;

  if ( ( frame == NULL ) || ( amr_dsm_q == NULL ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "voice_amr_ul_processing(): Invalid parameters" );
    return APR_EBADPARAM;
  }

#ifndef WINSIM
  /* Extract amr_info from VS buffer. */
  rc = voice_amr_extract_amr_info( &amr_info, frame, vs_media_id );
  if ( rc ) return rc;

  voice_amr_init_core_speech( &amr_speech, &speech_data );

  /* Determine number of bits in each class. */
  amr_speech.num_bits_a = amrsup_frame_class_bits( amr_info.frame_type,
                                                   amr_info.mode, AMRSUP_CLASS_A );
  amr_speech.num_bits_b = amrsup_frame_class_bits( amr_info.frame_type,
                                                   amr_info.mode, AMRSUP_CLASS_B );
  amr_speech.num_bits_c = amrsup_frame_class_bits( amr_info.frame_type,
                                                   amr_info.mode, AMRSUP_CLASS_C );

  chan_state.has_chan_a = ( amr_speech.num_bits_a > 0 ) ? TRUE : FALSE;
  chan_state.has_chan_b = ( amr_speech.num_bits_b > 0 ) ? TRUE : FALSE;
  chan_state.has_chan_c = ( amr_speech.num_bits_c > 0 ) ? TRUE : FALSE;


  if ( voice_amr_is_subset_chan_state( &chan_state, ul_chan_state ) != TRUE )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "voice_amr_ul_processing(): inconsistent channel setup for AMR mode" );
    return APR_EFAILED;
  }

  /* Extract individual channel data from vocoder packet in order to put in 
     separate DSM queues. */
  rc = voice_amr_extract_channels_from_if1( &frame[1], &amr_speech );
  if ( rc )
  {
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "voice_amr_ul_processing(): FAILED to extract individual channel "
            "data from vocoder packet (%d)", rc );
     return APR_EBADPARAM;
  }

   /* Transport individual AMR channels to modem via DSM queues. */
   voice_dsm_queue_ul_amr_speech( &amr_speech, &chan_state, amr_dsm_q );

#endif /* !WINSIM */

   return rc;
}


uint32_t voice_amr_dl_processing (
  vs_pktexg_buffer_t* vs_buffer,
  voice_amr_dsm_queue_t* amr_dsm_q
)
{
  int32_t rc = APR_EOK;
  amrsup_core_speech_type amr_speech;
  voice_amr_frame_info_t amr_info;
  voice_amr_speech_buffer_t speech_data;
  uint8_t *frame = NULL;

  if ( vs_buffer == NULL || amr_dsm_q == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "voice_amr_dl_processing(): Invalid parameters" );
    return APR_EBADPARAM;
  }

#ifndef WINSIM
  voice_amr_init_core_speech( &amr_speech, &speech_data );

  /* Get per channel speech data from modem via the DSM queues. */
  voice_dsm_dequeue_dl_amr_apeech( &amr_speech, amr_dsm_q );

  /* Determine frame type and mode. */
  amr_info.frame_type = amrsup_get_frame_type( &amr_speech );
  amr_info.mode = amrsup_get_frame_mode( &amr_speech, amr_info.frame_type );

  /* Create vocoder packet from data in individual channels. */
  /* Initialize vocoder packet to zero. */
  frame = ((uint8_t*)vs_buffer) + sizeof( vs_pktexg_buffer_t );
  memset( ( (void*)frame ), 0, VS_VOCAMRWB_MAX_PACKET_DATA_LENGTH );
  rc = voice_amr_make_if1_from_channels( frame+1, &amr_speech,
                                         VS_VOCAMRWB_MAX_PACKET_DATA_LENGTH, NULL );
  if ( rc ) return rc;

  /* update amr_info from VS buffer. */
  rc = voice_amr_update_amr_info( &amr_info, vs_buffer );
  if ( rc ) return rc;

#endif /* !WINSIM */

  return APR_EOK;
}



