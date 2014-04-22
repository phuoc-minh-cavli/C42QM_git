/*========================================================================*/
/**
@file adsp_media_fmt.h

This file contains the structure definitions for the media format
blocks used in client-ASM communications.
*/

/*===========================================================================
NOTE: The description above does not appear in the PDF. 

      The audio_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and Latex. To edit or update 
      any of the file/group text in the PDF, edit the audio_mainpage.dox file 
      contact Tech Pubs.
===========================================================================*/

/*===========================================================================
Copyright (c) 2012 Qualcomm Technologies, Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
======================================================================== */

/*===========================================================================
Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights reserved.

License to copy and use this software is granted provided that it is identified
as the "RSA Data Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing this software or this function.

License is also granted to make and use derivative works provided that such
works are identified as "derived from the RSA Data Security, Inc. MD5
Message-Digest Algorithm" in all material mentioning or referencing the
derived work.

RSA Data Security, Inc. makes no representations concerning either the
merchantability of this software or the suitability of this software for any 
particular purpose. It is provided "as is" without express or implied warranty
of any kind.

These notices must be retained in any copies of any part of this documentation
and/or software.
======================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/audio.tx/2.0/audio/api/avsaudio/inc/adsp_media_fmt.h#1 $

when       who     what, where, why
--------   ---      -------------------------------------------------------
05/30/11   sw      (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/12/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
04/15/10   rkc      Created file.

========================================================================== */

#ifndef _ADSP_MEDIA_FMT_H_
#define _ADSP_MEDIA_FMT_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @ingroup asm_strm_media_format_eg
    @xreflabel{hdr:AsmMediaFmtEg}
    Example media format for clients to try out a dummy encoder or decoder.
    This is for illustration purposes only.

  @parspace Example format block
    No format block is required.

  @parspace Example encode configuration block
    No format block is required.
  @newpage
*/
#define ASM_MEDIA_FMT_EXAMPLE                   0x00010C4D

/** @addtogroup asm_strm_media_format_pcm_ch_defs
@{ */

/** @xreflabel{hdr:PcmChDefL}  Front left channel. */
#define PCM_CHANNEL_L    1

/** @xreflabel{hdr:PcmChDefR}  Front right channel. */
#define PCM_CHANNEL_R    2

/** @xreflabel{hdr:PcmChDefC}  Front center channel. */
#define PCM_CHANNEL_C    3

/** @xreflabel{hdr:PcmChDefLS}  Left surround channel.

  This speaker position enumeration is maintained for backward compatibility.
  Currently, there are no devices that support this type. The client is
  responsible for ensuring that valid and existing channel mapping
  configurations corresponding to real speaker positions are used to ensure
  proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_LS   4

/** @xreflabel{hdr:PcmChDefRS}  Right surround channel.
  This speaker position enumeration is maintained for backward compatibility.
  Currently, there are no devices that support this type. The client is
  responsible for ensuring that valid and existing channel mapping
  configurations corresponding to real speaker positions are used to ensure
  proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_RS   5

/** @xreflabel{hdr:PcmChDefLFE}  Low frequency effect channel. */
#define PCM_CHANNEL_LFE  6

/** @xreflabel{hdr:PcmChDefCS}  Center surround channel; Rear center channel. */
#define PCM_CHANNEL_CS   7

/** @xreflabel{hdr:PcmChDefCb}  Center Back.
    @newpage */
#define PCM_CHANNEL_CB  PCM_CHANNEL_CS

/** @xreflabel{hdr:PcmChDefLB}  Left back channel; rear left channel. */
#define PCM_CHANNEL_LB   8

/** @xreflabel{hdr:PcmChDefRB}  Right back channel; Rear right channel. */
#define PCM_CHANNEL_RB   9 

/** @xreflabel{hdr:PcmChDefTS}  Top surround channel.

  This speaker position enumeration is maintained for backward compatibility.
  Currently, there are no devices that support this type. The client is
  responsible for ensuring that valid and existing channel mapping
  configurations corresponding to real speaker positions are used to ensure
  proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_TS   10

/** @xreflabel{hdr:PcmChDefCVH}  Center vertical height channel.

  This speaker position enumeration is maintained for backward compatibility.
  Currently, there are no devices that support this type. The client is
  responsible for ensuring that valid and existing channel mapping
  configurations corresponding to real speaker positions are used to ensure
  proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_CVH  11

/** @xreflabel{hdr:PcmChDefTfc}  Top Front Center (TFC)

    Currently, there are no devices that support this type. The client is
    responsible for ensuring that valid and existing channel mapping
    configurations corresponding to real speaker positions are used to ensure
    proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_TFC  PCM_CHANNEL_CVH

/** @xreflabel{hdr:PcmChDefMS}  Mono surround channel.

  This speaker position enumeration is maintained for backward compatibility.
  Currently, there are no devices that support this type. The client is
  responsible for ensuring that valid and existing channel mapping
  configurations corresponding to real speaker positions are used to ensure
  proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_MS   12

/** @xreflabel{hdr:PcmChDefFLC}  Front left of center. */
#define PCM_CHANNEL_FLC  13

/** @xreflabel{hdr:PcmChDefFRC}  Front right of center. */
#define PCM_CHANNEL_FRC  14

/** @xreflabel{hdr:PcmChDefRLC}  Rear left of center. */
#define PCM_CHANNEL_RLC  15

/** @xreflabel{hdr:PcmChDefRRC}  Rear right of center.
    @newpage */
#define PCM_CHANNEL_RRC  16

/** @xreflabel{hdr:PcmChDefLfe2}  Secondary LFE.
    @newpage */
#define PCM_CHANNEL_LFE2  17

/** @xreflabel{hdr:PcmChDefSl}  Side-Left (SL).
    @newpage */
#define PCM_CHANNEL_SL  18

/** @xreflabel{hdr:PcmChDefSr}  Side-Right.
    @newpage */
#define PCM_CHANNEL_SR  19

/** @xreflabel{hdr:PcmChDefTfl}  Top Front Left.
    @newpage */
#define PCM_CHANNEL_TFL  20

/** @xreflabel{hdr:PcmChDefLvh}  Left Vertical Height.
    @newpage */
#define PCM_CHANNEL_LVH  PCM_CHANNEL_TFL

/** @xreflabel{hdr:PcmChDefTfr}  Top Front Right.
    @newpage */
#define PCM_CHANNEL_TFR 21

/** @xreflabel{hdr:PcmChDefRvh}  Right Vertical Height.
    @newpage */
#define PCM_CHANNEL_RVH PCM_CHANNEL_TFR

/** @xreflabel{hdr:PcmChDefTc}  Top Center
    @newpage */
#define PCM_CHANNEL_TC  22

/** @xreflabel{hdr:PcmChDefTbl}  Top Back Left
    @newpage */
#define PCM_CHANNEL_TBL  23

/** @xreflabel{hdr:PcmChDefTbr}  Top Back Right.
    @newpage */
#define PCM_CHANNEL_TBR  24

/** @xreflabel{hdr:PcmChDefTsl}  Top Side Left.
    @newpage */
#define PCM_CHANNEL_TSL  25

/** @xreflabel{hdr:PcmChDefTsr}  Top Side Right.
    @newpage */
#define PCM_CHANNEL_TSR  26

/** @xreflabel{hdr:PcmChDefTbc}  Top Back Center.
    @newpage */
#define PCM_CHANNEL_TBC  27

/** @xreflabel{hdr:PcmChDefBfc}  Bottom Front Center.
    @newpage */
#define PCM_CHANNEL_BFC  28

/** @xreflabel{hdr:PcmChDefBfl}  Bottom Front Left.
    @newpage */
#define PCM_CHANNEL_BFL  29

/** @xreflabel{hdr:PcmChDefBfr}  Bottom Front Right.
    @newpage */
#define PCM_CHANNEL_BFR  30

/** @xreflabel{hdr:PcmChDefLw}  Left Wide.
    @newpage */
#define PCM_CHANNEL_LW  31

/** @xreflabel{hdr:PcmChDefRw}  Right Wide.
    @newpage */
#define PCM_CHANNEL_RW  32

/** @xreflabel{hdr:PcmChDefLsd}  Left Side Direct.
    @newpage */
#define PCM_CHANNEL_LSD  33

/** @xreflabel{hdr:PcmChDefRsd} Right Side Direct.
    @newpage */
#define PCM_CHANNEL_RSD  34

/** @} */ /* end_addtogroup asm_strm_media_format_pcm_ch_defs */


/** @addtogroup asm_strm_media_format_multich_pcm
@{ */
/** @xreflabel{hdr:AsmMediaFmtMultiChanPcm}
    Media format ID for multiple channel linear PCM.

  @parspace Multiple-channel PCM format block
    @structure{asm_multi_channel_pcm_fmt_blk_v2_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_WRITE_Multichannel_PCM_format_block.tex}

  @parspace PCM Multiple-channel encode configuration block
    @structure{asm_multi_channel_pcm_enc_cfg_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_READ_PCM_multichannel_encode_config.tex}
  @newpage

  API_developer_note: ASM_MEDIA_FMT_MULTI_CHANNEL_PCM & ASM_MEDIA_FMT_LINEAR_PCM
     were A family APIs. They are replaced by below in B family.
     Difference: is_interleaved flag is removed
     & data will always be treated as interleaved.
*/
#define ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2                0x00010DA5

/* Linear multiple channel PCM decoder format block structure. */
typedef struct asm_multi_channel_pcm_fmt_blk_v2_t asm_multi_channel_pcm_fmt_blk_v2_t;

#include "adsp_begin_pack.h"

/** @brief Multiple-channel PCM decoder format block structure used in the
    #ASM_STREAM_CMD_OPEN_WRITE_V2 command.\n
    \n
    The data must be in little-endian format. Also, it should always be interleaved.
*/
struct asm_multi_channel_pcm_fmt_blk_v2_t
{
    uint16_t  num_channels;
    /**< Number of channels.

         Supported values: 1 to 8 */

    uint16_t  bits_per_sample;
    /**< Number of bits per sample per channel.

         Supported values: 16, 24
    
         When used for playback, the client must send 24-bit samples packed in
         32-bit words. The 24-bit samples must be placed in the most
         significant 24 bits of the 32-bit word.

         When used for recording, the aDSP sends 24-bit samples packed in
         32-bit words. The 24-bit samples are placed in the most significant
         24 bits of the 32-bit word. */

    uint32_t  sample_rate;
    /**< Number of samples per second (in Hertz).

         Supported values: 2000 to 48000 */

    uint16_t  is_signed;
    /**< Flag that indicates the samples are signed (1). */

    uint16_t  reserved;
    /**< reserved field for 32 bit alignment. must be set to zero. */

    uint8_t   channel_mapping[8];
    /**< Channel array of size 8.

         Supported values:
         - #PCM_CHANNEL_L
         - #PCM_CHANNEL_R
         - #PCM_CHANNEL_C
         - #PCM_CHANNEL_LS
         - #PCM_CHANNEL_RS
         - #PCM_CHANNEL_LFE
         - #PCM_CHANNEL_CS
         - #PCM_CHANNEL_LB
         - #PCM_CHANNEL_RB
         - #PCM_CHANNEL_TS
         - #PCM_CHANNEL_CVH
         - #PCM_CHANNEL_MS
         - #PCM_CHANNEL_FLC
         - #PCM_CHANNEL_FRC
         - #PCM_CHANNEL_RLC
         - #PCM_CHANNEL_RRC

         Channel[i] mapping describes channel I. Each element i of the array
         describes channel I inside the buffer where 0 @le I < num_channels.
         An unused channel is set to zero. 
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Multichannel PCM encoder configuration structure. */
typedef struct asm_multi_channel_pcm_enc_cfg_v2_t asm_multi_channel_pcm_enc_cfg_v2_t;

#include "adsp_begin_pack.h"

/** @brief Multichannel PCM encoder configuration structure used in the
    #ASM_STREAM_CMD_OPEN_READ_V2 command.
*/
struct asm_multi_channel_pcm_enc_cfg_v2_t
{
   uint16_t  num_channels;
    /**< Number of PCM channels.

         Supported values:
         - 0 -- Native mode
         - 1 -- 8
         
         Native mode indicates that encoding must be performed with the number
         of channels at the input. */

    uint16_t  bits_per_sample;
    /**< Number of bits per sample per channel.

         Supported values: 16, 24 */

    uint32_t  sample_rate;
    /**< Number of samples per second (in Hertz).

         Supported values: 0, 8000 to 48000 
         
         A value of 0 indicates the native sampling rate. Encoding is
         performed at the input sampling rate. */

    uint16_t  is_signed;
    /**< Specifies whether the samples are signed (1). Currently, only signed
         samples are supported. */

    uint16_t  reserved;
    /**< reserved field for 32 bit alignment. must be set to zero. */

    uint8_t   channel_mapping[8];
    /**< Channel mapping expected at the encoder output. If native
         mode is set for the channels, this field is ignored.

         Supported values:
         - #PCM_CHANNEL_L
         - #PCM_CHANNEL_R
         - #PCM_CHANNEL_C
         - #PCM_CHANNEL_LS
         - #PCM_CHANNEL_RS
         - #PCM_CHANNEL_LFE
         - #PCM_CHANNEL_CS
         - #PCM_CHANNEL_LB
         - #PCM_CHANNEL_RB
         - #PCM_CHANNEL_TS
         - #PCM_CHANNEL_CVH
         - #PCM_CHANNEL_MS
         - #PCM_CHANNEL_FLC
         - #PCM_CHANNEL_FRC
         - #PCM_CHANNEL_RLC
         - #PCM_CHANNEL_RRC

         Channel[i] mapping describes channel I inside the buffer, where
         0 @le I < num_channels.
    
         @newpage */
}
#include "adsp_end_pack.h"
;

/** @} */ /* end_addtogroup asm_strm_media_format_multich_pcm */

/** @addtogroup asm_strm_media_format_adpcm
@{ */

/** @xreflabel{hdr:AsmMediaFmtAdpcm}
    Media format ID for adaptive differential PCM.

  @parspace ADPCM format block
    @structure{asm_adpcm_fmt_blk_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_WRITE_ADPCM_format_block.tex}

  @parspace ADPCM encode configuration block
    The encoder configuration block is the same as the ADPCM decoder
    format block.
      @negparspace
    @structure{asm_adpcm_fmt_blk_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_READ_ADPCM_encode_config.tex}
  @newpage
*/
#define ASM_MEDIA_FMT_ADPCM                     0x00010BE7

/* ADPCM decoder format block structure. */
typedef struct asm_adpcm_fmt_blk_t asm_adpcm_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @brief ADPCM encoder/decoder format block structure used in the
    #ASM_STREAM_CMD_OPEN_WRITE_V2 and #ASM_STREAM_CMD_OPEN_READ_V2 commands.
*/
struct asm_adpcm_fmt_blk_t
{
    uint16_t          num_channels;
    /**< Number of PCM channels.

         Supported values:
         - 0 -- Native mode (encoder only)
         - 1 -- Mono (decoder and encoder)
         - 2 -- Stereo (decoder and encoder) */

    uint16_t          bits_per_sample;
    /**< Number of bits per sample per channel.

         Supported values:
         - 4 -- Decoder
         - 16 -- Encoder */

    uint32_t          sample_rate;
    /**< Number of samples per second (in Hertz).
    
         Supported values for the decoder: 2000 to 48000 \n
         Supported values for the encoder:
         - 0 -- Native mode
         - 1 -- 2000 to 48000
         
         Native mode indicates that encoding must be performed with the
         sampling rate at the input. */

    uint32_t          blk_size;
    /**< Block size.
    
         For the decoder:
         - Value that is retrieved from the bitstream by the parser (upper
           layer).
         - The parser must parse it and send it to the aDSP.
    
         For the encoder:
         - Size of the block, which is a single unit or a frame of encoded
           samples.
         - Each block is completely independent of the other blocks in the
           stream, and each block contains the information necessary to decode
           a fixed number of PCM samples.
         - Supported values: 20 to 4096 bytes.
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Encoder configuration block is the same as the ADPCM decoder
    format block. */
typedef asm_adpcm_fmt_blk_t asm_adpcm_enc_cfg_t;

/** @} */ /* end_addtogroup asm_strm_media_format_adpcm */

/** @ingroup asm_strm_media_format_mp3
    @xreflabel{hdr:AsmMediaFmtMp3}
    Media format ID for MP3. This format block is empty; all information is
    contained in the bitstream.
*/
#define ASM_MEDIA_FMT_MP3                                       0x00010BE9

/** @addtogroup asm_strm_media_format_aac
@{ */
/** @xreflabel{hdr:AsmMediaFmtAac}
    Media format ID for the AAC format block, which supports both MPEG2 and
    MPEG4 AAC formats.

    @note1hang AOT=17 is the ER AAC-LC format, which currently is not
               supported.

  @parspace AAC format block
    @structure{asm_aac_fmt_blk_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_WRITE_AAC_format_block.tex}
    @newpage

  @parspace AAC encode configuration block
    @structure{asm_aac_enc_cfg_v2_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_READ_AAC_encode_config.tex}

    @tablespace
    @inputtable{Encoder_AAC_allowed_bitrates.tex}
    @ast f_s = Input sampling frequency in Hertz.
    @tablespace

    @inputtable{Encoder_AAC_supported_sampling_rates.tex}
*/
#define ASM_MEDIA_FMT_AAC_V2                                       0x00010DA6

/** @xreflabel{hdr:AsmMediaFmtDolbyAac}
    Media format ID for the Dolby AAC decoder. This format ID is be used if
    the client wants to use the Dolby AAC decoder to decode MPEG2 and MPEG4
    AAC contents.
*/
#define ASM_MEDIA_FMT_DOLBY_AAC                                 0x00010D86

/** Enumeration for the audio data transport stream AAC format. */
#define ASM_MEDIA_FMT_AAC_FORMAT_FLAG_ADTS                      0

/** Enumeration for low overhead audio stream AAC format. */
#define ASM_MEDIA_FMT_AAC_FORMAT_FLAG_LOAS                      1

/** Enumeration for the audio data interchange format AAC format. */
#define ASM_MEDIA_FMT_AAC_FORMAT_FLAG_ADIF                      2

/** Enumeration for the raw AAC format. */
#define ASM_MEDIA_FMT_AAC_FORMAT_FLAG_RAW                       3

/** Enumeration for the AAC low complexity audio object type:

    - When used for aac_fmt_flag in asm_aac_fmt_blk_t, this parameter
      specifies that the AOT in the AAC stream is LC.
    - When used for enc_mode in asm_aac_enc_cfg_v2_t, this parameter specifies
      that the Encoding mode to be used is LC.
*/
#define ASM_MEDIA_FMT_AAC_AOT_LC                                2

/** Enumeration for the AAC spectral band replication AOT:

    - When used for aac_fmt_flag in asm_aac_fmt_blk_t, this parameter
      specifies that the AOT in the AAC stream is SBR.
    - When used for enc_mode in asm_aac_enc_cfg_v2_t, this parameter specifies
      that the Encoding mode to be used is AAC+.
*/
#define ASM_MEDIA_FMT_AAC_AOT_SBR                               5

/** Enumeration for the AAC parametric stereo AOT:

    - When used for aac_fmt_flag in asm_aac_fmt_blk_t, this parameter
      specifies that the AOT in the AAC stream is PS.
    - When used for enc_mode in asm_aac_enc_cfg_v2_t, this parameter specifies
      that the Encoding mode to be used is eAAC+.
*/
#define ASM_MEDIA_FMT_AAC_AOT_PS                                29

/** Enumeration for the bit-sliced arithmetic coding AOT:

    - When used for aac_fmt_flag in asm_aac_fmt_blk_t, this parameter
      specifies that the AOT in the AAC stream is BSAC.
    - This parameter must not be used for enc_mode in
      asm_aac_enc_cfg_v2_t. BSAC encoding is not supported.
    @newpage
*/
#define ASM_MEDIA_FMT_AAC_AOT_BSAC                              22

/* AAC decoder format block structure. */
typedef struct asm_aac_fmt_blk_v2_t asm_aac_fmt_blk_v2_t;

#include "adsp_begin_pack.h"

/** @brief AAC decoder format block structure used in the
    #ASM_STREAM_CMD_OPEN_WRITE_V2 command.

    if total size of PCE in bits > 0, then PCE info follows after the structure.
    Client should pad additional bits for 32 bit alignment when needed.
*/
struct asm_aac_fmt_blk_v2_t
{
    uint16_t          aac_fmt_flag;
    /**< Bitstream format option.

         Supported values:
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_ADTS
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_LOAS
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_ADIF
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_RAW */

    uint16_t          audio_obj_type;
    /**< Audio Object Type (AOT) present in the AAC stream.

         Supported values:
         - #ASM_MEDIA_FMT_AAC_AOT_LC
         - #ASM_MEDIA_FMT_AAC_AOT_SBR
         - #ASM_MEDIA_FMT_AAC_AOT_BSAC
         - #ASM_MEDIA_FMT_AAC_AOT_PS
         - Otherwise -- Not supported */

    uint16_t          channel_config;
    /**< Number of channels present in the AAC stream.

         Supported values:
         - 1 -- Mono
         - 2 -- Stereo
         - 6 -- 5.1 content */

    uint16_t          total_size_of_PCE_bits;
    /**< greater or equal to zero.
     * -In case of RAW formats and channel config = 0 (PCE), client can send the bit stream containing PCE immediately following this structure (in-band).
       -This number doesn’t include bits included for 32 bit alignment.
       -If zero, then the PCE info is assumed to be available in the audio bit stream & not in-band.
     * */
    uint32_t          sample_rate;
    /**< Number of samples per second (in Hertz).

         Supported values: 8000, 11025, 12000, 16000, 22050, 24000, 32000,
                           44100, 48000

         This field must be equal to the sample rate of the AAC-LC decoder's
         output.
         - For MP4 or 3GP containers, this is indicated by the
           samplingFrequencyIndex field in the AudioSpecificConfig element.
         - For ADTS format, this is indicated by the samplingFrequencyIndex
           in the ADTS fixed header.
         - For ADIF format, this is indicated by the samplingFrequencyIndex
           in the program_config_element present in the ADIF header.
         @newpage */
}
#include "adsp_end_pack.h"
;

/* AAC encoder configuration structure. */
typedef struct asm_aac_enc_cfg_v2_t asm_aac_enc_cfg_v2_t;

#include "adsp_begin_pack.h"

/** @brief AAC encoder configuration structure used in the
    #ASM_STREAM_CMD_OPEN_READ_V2 command.
*/
struct asm_aac_enc_cfg_v2_t
{
    uint32_t          bit_rate;
    /**< Encoding rate in bits per second. */

    uint32_t          enc_mode;
    /**< Encoding mode.

         Supported values:
         - #ASM_MEDIA_FMT_AAC_AOT_LC
         - #ASM_MEDIA_FMT_AAC_AOT_SBR
         - #ASM_MEDIA_FMT_AAC_AOT_PS */

    uint16_t          aac_fmt_flag;
    /**< AAC format flag.

         Supported values:    
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_ADTS
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_RAW
         @newpage*/

    uint16_t          channel_cfg;
    /**< Number of channels to encode.

         Supported values:
         - 0 -- Native mode
         - 1 -- Mono
         - 2 -- Stereo
         - Other values are not supported.
         
         @note1hang The eAAC+ encoder mode supports only stereo.

         Native mode indicates that encoding must be performed with the number
         of channels at the input.

         The number of channels must not change during encoding. */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         Supported values:
         - 0 -- Native mode
         - For other values, see the
           @xhyperref{hdr:AACencoderSupportedSamplingRates,AAC encoder
           supported sampling rates} table
         
         Native mode indicates that encoding must be performed with the
         sampling rate at the input.

         The sampling rate must not change during encoding.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_aac */

/** @addtogroup asm_strm_media_format_amrnb
@{ */

/** @xreflabel{hdr:AsmMediaFmtAmrNb}
    Media format ID for adaptive multirate narrowband.

  @parspace AMR-NB format block
    No format block is needed;
    all information is contained in the bitstream.

  @parspace AMR-NB encode configuration block
    @structure{asm_amrnb_enc_cfg_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_READ_AMRNB_encode_config.tex}
*/
#define ASM_MEDIA_FMT_AMRNB_FS                  0x00010BEB

/** Enumeration for 4.75 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MR475                0

/** Enumeration for 5.15 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MR515                1

/** Enumeration for 5.90 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR59                2

/** Enumeration for 6.70 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR67                3

/** Enumeration for 7.40 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR74                4

/** Enumeration for 7.95 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR795               5

/** Enumeration for 10.20 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR102               6

/** Enumeration for 12.20 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR122               7

/** Enumeration for AMR-NB Discontinuous Transmission mode off. */
#define ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_OFF                     0

/** Enumeration for AMR-NB DTX mode VAD1. */
#define ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_VAD1                    1

/** Enumeration for AMR-NB DTX mode VAD2. */
#define ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_VAD2                    2

/** Enumeration for AMR-NB DTX mode auto.
    @newpage */
#define ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_AUTO                    3

/* AMR-NB encoder configuration structure. */
typedef struct asm_amrnb_enc_cfg_t asm_amrnb_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @brief AMR-NB encoder configuration structure used in the
    #ASM_STREAM_CMD_OPEN_READ_V2 command.
*/
struct asm_amrnb_enc_cfg_t
{
    uint16_t          enc_mode;
    /**< AMR-NB encoding rate.
    
         Supported values: Use the ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_*
                           macros */

    uint16_t          dtx_mode;
    /**< Specifies whether DTX mode is disabled or enabled.
    
         Supported values:
         - #ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_OFF
         - #ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_VAD1
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_amrnb */

/** @addtogroup asm_strm_media_format_amrwb
@{ */

/** @xreflabel{hdr:AsmMediaFmtAmrWb}
    Media format ID for adaptive multirate wideband.

  @parspace AMR-WB format block
    No format block is needed;
    all information is contained in the bitstream.

  @parspace AMR-WB encode configuration
    @structure{asm_amrwb_enc_cfg_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_READ_AMRWB_encode_config.tex}
*/
#define ASM_MEDIA_FMT_AMRWB_FS                  0x00010BEC

/** Enumeration for 6.6 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR66                 0

/** Enumeration for 8.85 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR885                1

/** Enumeration for 12.65 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1265               2

/** Enumeration for 14.25 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1425               3

/** Enumeration for 15.85 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1585               4

/** Enumeration for 18.25 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1825               5

/** Enumeration for 19.85 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1985               6

/** Enumeration for 23.05 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR2305               7

/** Enumeration for 23.85 kbps AMR-WB Encoding mode.
    @newpage */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR2385               8

/* AMR-WB encoder configuration structure. */
typedef struct asm_amrwb_enc_cfg_t asm_amrwb_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @brief AMR-WB encoder configuration structure used in the
    #ASM_STREAM_CMD_OPEN_READ_V2 command.
*/
struct asm_amrwb_enc_cfg_t
{
    uint16_t          enc_mode;
    /**< AMR-WB encoding rate.
    
         Suupported values: Use the ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_*
                            macros */

    uint16_t          dtx_mode;
    /**< Specifies whether DTX mode is disabled or enabled.

         Supported values:
         - #ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_OFF
         - #ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_VAD1
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_amrwb */

/** @addtogroup asm_strm_media_format_v13k
@{ */
/** @xreflabel{hdr:AsmMediaFmtV13k}
    Media format ID for V13K Encoding mode.

  @parspace V13K format block
    No format block is needed; all
    information is contained in the bitstream.

  @parspace V13K encode configuration
    @structure{asm_v13k_enc_cfg_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_READ_V13K_encode_config.tex}
*/
#define ASM_MEDIA_FMT_V13K_FS                      0x00010BED

/** Enumeration for 14.4 kbps V13K Encoding mode. */
#define ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1440                0

/** Enumeration for 12.2 kbps V13K Encoding mode. */
#define ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1220                1

/** Enumeration for 11.2 kbps V13K Encoding mode. */
#define ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1120                2

/** Enumeration for 9.0 kbps V13K Encoding mode. */
#define ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR90                  3

/** Enumeration for 7.2 kbps V13K eEncoding mode. */
#define ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR720                 4

/** Enumeration for 1/8 vocoder rate.*/
#define ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE          1

/** Enumeration for 1/4 vocoder rate. */
#define ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE       2

/** Enumeration for 1/2 vocoder rate. */
#define ASM_MEDIA_FMT_VOC_HALF_RATE             3

/** Enumeration for full vocoder rate.
    @newpage */
#define ASM_MEDIA_FMT_VOC_FULL_RATE             4

/* V13K encoder configuration structure. */
typedef struct asm_v13k_enc_cfg_t asm_v13k_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @brief V13K encoder configuration structure used in the
    #ASM_STREAM_CMD_OPEN_READ_V2 command.
*/
struct asm_v13k_enc_cfg_t
{
    uint16_t          max_rate;
    /**< Maximum allowed encoder frame rate.

         Supported values:
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE */

    uint16_t          min_rate;
    /**< Minimum allowed encoder frame rate.

         Supported values:
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE */

    uint16_t          reduced_rate_cmd;
    /**< Reduced rate command, used to change the average bitrate of the V13K
         vocoder.

         Supported values:
         - #ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1440 (Default)
         - #ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1220
         - #ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1120
         - #ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR90
         - #ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR720 */

    uint16_t          rate_mod_cmd;
    /**< Rate modulation command. Default = 0.

         - If bit 0=1, rate control is enabled.
         - If bit 1=1, the maximum number of consecutive full rate frames is 
           limited with numbers supplied in bits 2 to 10.
         - If bit 1=0, the minimum number of non-full rate frames in between
           two full rate frames is forced to the number supplied in bits 2 to
           10. In both cases, if necessary, half rate is used to substitute
           full rate.
         - Bits 15 to 10 are reserved and must all be set to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_v13k */

/** @addtogroup asm_strm_media_format_evrc
@{ */
/** @xreflabel{hdr:AsmMediaFmtEvrc}
    Media format ID for enhanced variable rate coder.

  @parspace EVRC format block
    No format block is needed;
    all information is contained in the bitstream.

  @parspace EVRC encode configuration
    @structure{asm_evrc_enc_cfg_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_READ_EVRC_encode_config.tex}
    @newpage
*/
#define ASM_MEDIA_FMT_EVRC_FS                   0x00010BEE

/* EVRC encoder configuration structure. */
typedef struct asm_evrc_enc_cfg_t asm_evrc_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @brief EVRC encoder configuration structure used in the
    #ASM_STREAM_CMD_OPEN_READ_V2 command.
*/
struct asm_evrc_enc_cfg_t
{
    uint16_t          max_rate;
    /**< Maximum allowed encoder frame rate.

         Supported values:
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE */

    uint16_t          min_rate;
    /**< Minimum allowed encoder frame rate.

         Supported values:
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE */

    uint16_t          rate_mod_cmd;
    /**< Rate modulation command. Default: 0.

         - If bit 0=1, rate control is enabled.
         - If bit 1=1, the maximum number of consecutive full rate frames is
           limited with numbers supplied in bits 2 to 10.
         - If bit 1=0, the minimum number of non-full rate frames in between
           two full rate frames is forced to the number supplied in bits 2 to
           10. In both cases, if necessary, half rate is used to substitute
           full rate.
         - Bits 15 to 10 are reserved and must all be set to zero. */

    uint16_t          reserved;
    /**< Reserved. Clients must set this field to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_evrc */

/** @addtogroup asm_strm_media_format_evcrb
@{ */
/** @xreflabel{hdr:AsmMediaFmtEvrcB}
    Media format ID for enhanced variable rate codec B.

  @parspace EVRC-B format block
    No format block is
    needed; all information is contained in the bitstream.

  @parspace EVRC-B encode configuration
    @structure{asm_evrcb_enc_cfg_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_READ_EVRCB_encode_config.tex}
*/
#define ASM_MEDIA_FMT_EVRCB_FS                  0x00010BEF

/** Enumeration for 9.3 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR930        0

/** Enumeration for 8.5 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR850        1

/** Enumeration for 7.5 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR750        2

/** Enumeration for 7.0 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR700        3

/** Enumeration for 6.6 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR660        4

/** Enumeration for 6.2 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR620        5

/** Enumeration for 5.8 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR580        6

/** Enumeration for 4.8 kbps EVRC-B Encoding mode.
    @newpage */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR480        7

/* EVCR-B encoder configuration structure. */
typedef struct asm_evrcb_enc_cfg_t asm_evrcb_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @brief EVCR-B encoder configuration structure used in the
    #ASM_STREAM_CMD_OPEN_READ_V2 command.
*/
struct asm_evrcb_enc_cfg_t
{
    uint16_t          max_rate;
    /**< Maximum allowed encoder frame rate.

         Supported values:
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE */

    uint16_t          min_rate;
    /**< Minimum allowed encoder frame rate.

         Supported values:
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE */

    uint16_t          dtx_enable;
    /**< Specifies whether DTX mode is disabled (0) or enabled (1).
         @newpage */

    uint16_t          reduced_rate_level;
    /**< Reduced rate level for the average encoding rate. Use the
         ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_* macros to set this field.

         Supported values:
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR930 (Default)
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR850
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR750
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR700
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR660
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR620
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR580
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR480
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_evcrb */

/** @addtogroup asm_strm_media_format_evrcwb
@{ */
/** @xreflabel{hdr:AsmMediaFmtEvrcWb}
    Media format ID for enhanced variable rate codec wideband.

  @parspace EVRC-WB format block
    @xreflabel{hdr:}
    No format block
    is needed; all information is contained in the bitstream.

  @parspace EVRC-WB encode configuration
    @xreflabel{hdr:}
    @structure{asm_evrcwb_enc_cfg_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_READ_EVRCWB_encode_config.tex}
*/
#define ASM_MEDIA_FMT_EVRCWB_FS                 0x00010BF0

/** Enumeration for 8.5 kbps EVRC-WB Encoding mode.  */
#define ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR850        0

/** Enumeration for 9.3 kbps EVRC-WB Encoding mode. */
#define ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR930        4

/** Enumeration for 4.8 kbps EVRC-WB Encoding mode.
    @newpage */
#define ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR480        7

/* EVRC-WB encoder configuration structure. */
typedef struct asm_evrcwb_enc_cfg_t asm_evrcwb_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @brief EVRC-WB encoder configuration structure used in the
    #ASM_STREAM_CMD_OPEN_READ_V2 command.
*/
struct asm_evrcwb_enc_cfg_t
{
    uint16_t          max_rate;
    /**< Maximum allowed encoder frame rate.

         Supported values:
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE */

    uint16_t          min_rate;
    /**< Minimum allowed encoder frame rate.

         Supported values:
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE */

    uint16_t          dtx_enable;
    /**< Specifies whether DTX mode is disabled (0) or enabled (1).
         @newpage */

    uint16_t          reduced_rate_level;
    /**< Reduced rate level for the average encoding rate. Use the
         ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_* macros to set this field.

         Supported values:
         - #ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR850 (Default)
         - 1 -- Reserved
         - 2 -- Reserved
         - 3 -- Reserved
         - #ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR930
         - 5 -- Reserved
         - 6 -- Reserved
         - #ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR480
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_evrcwb */

/** @addtogroup asm_strm_media_format_midi
@{ */
/** @xreflabel{hdr:AsmMediaFmtMidi}
    Media format ID for MIDI.

    @structure{asm_midi_fmt_blk_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_WRITE_MIDI_format_block.tex}
*/
#define ASM_MEDIA_FMT_MIDI                      0x00010BF1

/** Enumeration for MIDI mode GM. */
#define ASM_MIDI_MODE_GM                        0

/** Enumeration for MIDI mode MA3. */
#define ASM_MIDI_MODE_MA3                       1

/** Enumeration for MIDI mode MA5.
    @newpage */
#define ASM_MIDI_MODE_MA5                       2

/* MIDI format block structure. */
typedef struct asm_midi_fmt_blk_t asm_midi_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @brief MIDI format block structure used in the #ASM_STREAM_CMD_OPEN_WRITE_V2
    command.
*/
struct asm_midi_fmt_blk_t
{
    uint32_t          mode;
    /**< Operating mode that indicates which commercial MIDI device is to be
         emulated.
         
         Supported values:
         - #ASM_MIDI_MODE_GM
         - #ASM_MIDI_MODE_MA3
         - #ASM_MIDI_MODE_MA5
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_midi */

/** @addtogroup asm_strm_media_format_sbc
@{ */

/** @xreflabel{hdr:AsmMediaFmtSbc}
    Media format ID for SBC encode configuration.

    @structure{asm_sbc_enc_cfg_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_READ_SBC_encode_config.tex}
*/
#define ASM_MEDIA_FMT_SBC                       0x00010BF2

/** Enumeration for SBC channel Mono mode. */
#define ASM_MEDIA_FMT_SBC_CHANNEL_MODE_MONO                     1

/** Enumeration for SBC channel Stereo mode. */
#define ASM_MEDIA_FMT_SBC_CHANNEL_MODE_STEREO                   2

/** Enumeration for SBC channel Dual Mono mode. */
#define ASM_MEDIA_FMT_SBC_CHANNEL_MODE_DUAL_MONO                8

/** Enumeration for SBC channel Joint Stereo mode. */
#define ASM_MEDIA_FMT_SBC_CHANNEL_MODE_JOINT_STEREO             9

/** Enumeration for SBC bit allocation method = loudness. */
#define ASM_MEDIA_FMT_SBC_ALLOCATION_METHOD_LOUDNESS            0

/** Enumeration for SBC bit allocation method = SNR.
    @newpage */
#define ASM_MEDIA_FMT_SBC_ALLOCATION_METHOD_SNR                 1

/* SBC encoder configuration structure. */
typedef struct asm_sbc_enc_cfg_t asm_sbc_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @brief SBC encoder configuration structure used in the
    #ASM_STREAM_CMD_OPEN_READ_V2 command.
*/
struct asm_sbc_enc_cfg_t
{
    uint32_t          num_subbands;
    /**< Number of sub-bands.

         Supported values: 4, 8 */

    uint32_t          blk_len;
    /**<Size of the encoded block in samples.

         Supported values: 4, 8, 12, 16 */

    uint32_t          channel_mode;
    /**< Used to allocate bits between channels. Use the
         ASM_MEDIA_FMT_SBC_CHANNEL_MODE_* macros to set this field.
START HERE
         Supported values:
         - 0 -- Native mode
         - 1 -- Mono
         - 2 -- Stereo
         - 8 -- Dual
         - 9 -- Joint stereo
         
         Native mode indicates that encoding must be performed with the number
         of channels at the input.

         If postprocessing outputs one-channel data, Mono mode is used. If
         postprocessing outputs two-channel data, Stereo mode is used. 
         The number of channels must not change during encoding. */

    uint32_t          alloc_method;
    /**< Encoder bit allocation method. Use the
         ASM_MEDIA_FMT_SBC_ALLOCATION_METHOD_* macros to set this field.

         Supported values:
         - 0 -- Loudness
         - 1 -- SNR */

    uint32_t          bit_rate;
    /**< Number of encoded bits per second.

         Supported values:    
         - Maximum of 320 kbps for a mono channel
         - Maximum of 512 kbps for a stereo channel */

    uint32_t          sample_rate;
    /**< Number of samples per second (in Hertz).

         Supported values: 0 (Native mode), 16000, 32000, 44100, 48000
         
         Native mode indicates that encoding must be performed with the
         sampling rate at the input.
         
         The sampling rate must not change during encoding.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup SBC Media Formats and Macros */

/** @addtogroup asm_strm_media_format_wmapro10
@{ */
/** @xreflabel{hdr:AsmMediaFmtWma10Pro}
    Media format ID for the WMA v10
    Professional format block.

    @structure{asm_wmaprov10_fmt_blk_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_WRITE_WMAPro_10_format_block.tex}
    @newpage
*/
#define ASM_MEDIA_FMT_WMA_V10PRO_V2                0x00010DA7

/* WMA v10 Professional decoder format block structure. */
typedef struct asm_wmaprov10_fmt_blk_v2_t asm_wmaprov10_fmt_blk_v2_t;

#include "adsp_begin_pack.h"

/** @brief WMA Professional v10 decoder format block structure used in the
    #ASM_STREAM_CMD_OPEN_WRITE_V2 command.
*/
struct asm_wmaprov10_fmt_blk_v2_t
{
    uint16_t          fmt_tag;
    /**< WMA format type.

         Supported values:
         - 0x162 -- WMA 9 Pro
         - 0x163 -- WMA 9 Pro Lossless
         - 0x166 -- WMA 10 Pro
         - 0x167 -- WMA 10 Pro Lossless */

    uint16_t          num_channels;
    /**< Number of channels encoded in the input stream.

         Supported values: 1 to 8 */

    uint32_t          sample_rate;
    /**< Number of samples per second (in Hertz).

         Supported values: 11025, 16000, 22050, 32000, 44100, 48000, 88200,
                           96000 */

    uint32_t          avg_bytes_per_sec;
    /**< Bitrate expressed as the average bytes per second.
    
         Supported values: 2000 to 96000 */

    uint16_t          blk_align;
    /**< Size of the bitstream packet size in bytes. WMA Pro files have a
         payload of one block per bitstream packet.
    
         Supported values: @le 13376 */

    uint16_t          bits_per_sample;
    /**< Number of bits per sample in the encoded WMA stream.

         Supported values: 16, 24 */

    uint32_t          channel_mask;
    /**< Bit-packed double word (32-bits) that indicates the recommended
         speaker positions for each source channel.

         Supported values: See xx Popular Speaker Positions for WMAPro 10 table */

    uint16_t          enc_options;
    /**< Bit-packed word with values that indicate whether certain features of
         the bitstream are used.
         
         Supported values:
         - 0x0001 -- ENCOPT3_PURE_LOSSLESS
         - 0x0006 -- ENCOPT3_FRM_SIZE_MOD
         - 0x0038 -- ENCOPT3_SUBFRM_DIV
         - 0x0040 -- ENCOPT3_WRITE_FRAMESIZE_IN_HDR
         - 0x0080 -- ENCOPT3_GENERATE_DRC_PARAMS
         - 0x0100 -- ENCOPT3_RTMBITS */

    uint16_t          usAdvancedEncodeOpt;
    /**< Advanced encoding option.  */

    uint32_t          advanced_enc_options2;
    /**< Advanced encoding option 2. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_wmapro10 */

/** @addtogroup asm_strm_media_format_wma9
@{ */

/** @xreflabel{hdr:AsmMediaFmtWma9}
    Media format ID for the Windows Media<sup>&reg;</sup> Audio (WMA) v9 format block.

    @structure{asm_wmastdv9_fmt_blk_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_WRITE_WMA_9_format_block.tex}
    @newpage
*/
#define ASM_MEDIA_FMT_WMA_V9_V2                    0x00010DA8

/* WMA v9 decoder format block structure. */
typedef struct asm_wmastdv9_fmt_blk_v2_t asm_wmastdv9_fmt_blk_v2_t;

#include "adsp_begin_pack.h"

/** @brief WMA v9 decoder format block structure used in the
    #ASM_STREAM_CMD_OPEN_WRITE_V2 command.
*/
struct asm_wmastdv9_fmt_blk_v2_t
{
    uint16_t          fmt_tag;
    /**< WMA format tag.

         Supported values: 0x161 (WMA 9 standard) */

    uint16_t          num_channels;
    /**< Number of channels in the stream.

         Supported values: 1, 2 */

    uint32_t          sample_rate;
    /**< Number of samples per second (in Hertz).

         Supported values: 48000 */

    uint32_t          avg_bytes_per_sec;
    /**< Bitrate expressed as the average bytes per second. */

    uint16_t          blk_align;
    /**< Block align. All WMA files with a maximum packet size of 13376
         are supported. */

    uint16_t          bits_per_sample;
    /**< Number of bits per sample in the output.

         Supported values: 16 */

    uint32_t          channel_mask;
    /**< Channel mask.

         Supported values:
         - 3 -- Stereo (front left/front right)
         - 4 -- Mono (center) */

    uint16_t          enc_options;
    /**< Options used during encoding. */

    uint16_t          reserved;
    /**< reserved field client must set to zero */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_wma9 */

/** @addtogroup asm_strm_media_format_wma8
@{ */

/** @xreflabel{hdr:AsmMediaFmtWma8}
    Media format ID for the Windows Media<sup>&reg;</sup> Audio (WMA) v8 format block.

    @structure{asm_wmastdv8_enc_cfg_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_READ_WMA_8_format_block.tex}
    @newpage
*/
#define ASM_MEDIA_FMT_WMA_V8                    0x00010D91

/* WMA v8 encoder configuration structure. */
typedef struct asm_wmastdv8_enc_cfg_t asm_wmastdv8_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @brief WMA v8 encoder configuration structure used in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_wmastdv8_enc_cfg_t
{
    uint32_t          bit_rate;
    /**< Encoding rate in bits per second. */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         Supported values:
         - 0 -- Native mode
         - Other Supported values are 22050, 32000, 44100, and 48000. 
         
         Native mode indicates that encoding must be performed with the
         sampling rate at the input.

         The sampling rate must not change during encoding. */
		 
    uint16_t          channel_cfg;
    /**< Number of channels to encode.

         Supported values:
         - 0 -- Native mode
         - 1 -- Mono
         - 2 -- Stereo
         - Other values are not supported.
         
         Native mode indicates that encoding must be performed with the number
         of channels at the input.

         The number of channels must not change during encoding. */
		 
    uint16_t          reserved;
    /**< Reserved. Clients must set this field to zero.
         @newpage */
	
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_wma8 */
/** @addtogroup asm_strm_media_format_amrwbplus
@{ */
/** @xreflabel{hdr:AsmMediaFmtAmrWbPlus}
    Media format ID for the AMR-WB Plus format block.

    @structure{asm_amrwbplus_fmt_blk_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_WRITE_AMR_WB_PLUS_format_block.tex}
    @newpage
*/
#define ASM_MEDIA_FMT_AMR_WB_PLUS_V2               0x00010DA9

/* AMR-WB Plus decoder format block structure. */
typedef struct asm_amrwbplus_fmt_blk_v2_t asm_amrwbplus_fmt_blk_v2_t;

#include "adsp_begin_pack.h"

/** @brief AMR-WB Plus decoder format block structure used in the
    #ASM_STREAM_CMD_OPEN_WRITE_V2 command.
*/
struct asm_amrwbplus_fmt_blk_v2_t
{
    uint32_t          amr_frame_fmt;
    /**< AMR frame format.

         Supported values:
         - 6 -- Transport Interface Format (TIF)
         - Any other value -- File storage format (FSF)
         
         TIF stream contains 2-byte header for each frame within the superframe.
         FSF stream contains one 2-byte header per superframe. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_amrwbplus */

/** @addtogroup asm_strm_media_format_eac3
@{ */
/** @xreflabel{hdr:AsmMediaFmtAc3Dec}
    Media format ID for an AC3 decoder. No format block is required.
*/
#define ASM_MEDIA_FMT_AC3_DEC                   0x00010BF6

/** @xreflabel{hdr:AsmMediaFmtEac3Dec}
    Media format ID for an E-AC3 decoder.
    @newpage
*/
#define ASM_MEDIA_FMT_EAC3_DEC                   0x00010C3C

/** @} */ /* end_addtogroup asm_strm_media_format_eac3 */

/** @addtogroup asm_strm_media_format_g711_alaw
@{ */
/** @xreflabel{hdr:AsmMediaFmtG711Alaw}
    Media format ID for the G711 A-law algorithm.

  @parspace G711 A-law format block
    @structure{asm_g711_alaw_fmt_blk_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_WRITE_G711_ALAW_format_block.tex}

  @parspace G711 ALAW encode configuration
    @structure{asm_g711_alaw_enc_cfg_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_READ_G711_ALAW_encode_config.tex}
  @newpage
*/
#define ASM_MEDIA_FMT_G711_ALAW_FS                   0x00010BF7

/* G711 A-law encoder configuration structure. */
typedef struct asm_g711_alaw_enc_cfg_t asm_g711_alaw_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @brief G711 A-law encoder configuration structure used in the
    #ASM_STREAM_CMD_OPEN_READ_V2 command.
*/
struct asm_g711_alaw_enc_cfg_t
{
    uint32_t            sample_rate;
    /**< Number of samples per second (in Hertz).

         Supported values: 8000, 16000
         @newpage */
}
#include "adsp_end_pack.h"
;

/* G711 A-law decoder format block structure. */
typedef struct asm_g711_alaw_fmt_blk_t asm_g711_alaw_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @brief G711 A-law decoder format block structure used in the
    #ASM_STREAM_CMD_OPEN_WRITE_V2 command.
 */
struct asm_g711_alaw_fmt_blk_t
{
    uint32_t            sample_rate;
    /**< Number of samples per second (in Hertz).

         Supported values: 8000, 16000
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_g711_alaw */

/** @addtogroup asm_strm_media_format_g711_mlaw
@{ */
/** @xreflabel{hdr:AsmMediaFmtG711Mlaw}
    Media format ID for the G711 m-law algorithm.

  @parspace G711 M-law format block
    @structure{asm_g711_mlaw_fmt_blk_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_WRITE_G711_MLAW_format_block.tex}

  @parspace G711 MLAW encode configuration
    @structure{asm_g711_mlaw_enc_cfg_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_READ_G711_MLAW_encode_config.tex}
  @newpage
*/
#define ASM_MEDIA_FMT_G711_MLAW_FS                   0x00010C2E

/* G711 m-law encoder configuration structure. */
typedef struct asm_g711_mlaw_enc_cfg_t asm_g711_mlaw_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @brief G711 m-law encoder configuration structure used in the
    #ASM_STREAM_CMD_OPEN_READ_V2 command.
*/
struct asm_g711_mlaw_enc_cfg_t
{
    uint32_t            sample_rate;
    /**< Number of samples per second (in Hertz).

         Supported values: 8000, 16000
         @newpage */
}
#include "adsp_end_pack.h"
;

/* G711 m-law decoder format block structure. */
typedef struct asm_g711_mlaw_fmt_blk_t asm_g711_mlaw_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @brief G711 m-law decoder format block structure used in the
    #ASM_STREAM_CMD_OPEN_WRITE_V2 command.
*/
struct asm_g711_mlaw_fmt_blk_t
{
    uint32_t            sample_rate;
    /**< Number of samples per second (in Hertz).

         Supported values: 8000, 16000
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_g711_mlaw */

/** @ingroup asm_strm_media_format_g729a
    @xreflabel{hdr:AsmMediaFmtG729a}
    Media format ID for G729A.

  @parspace G729A format block
    No format block is required.

  @parspace G729A encode configuration block
    No encoder configuration block is required. This is currently unused.
*/
#define ASM_MEDIA_FMT_G729A_FS                 0x00010C2D

/** @ingroup asm_strm_media_format_dtmf
    @xreflabel{hdr:AsmMediaFmtDtmf}
    Media format ID for DTMF.
*/
#define ASM_MEDIA_FMT_DTMF                     0x00010C2F

/** @ingroup asm_strm_media_format_gsmfr
    @xreflabel{hdr:AsmMediaFmtGsmFr}
    Media format ID for GSM-FR.
    @newpage
*/
#define ASM_MEDIA_FMT_FR_FS                     0x00010D6B

/** @addtogroup asm_strm_media_format_vorbis
@{ */
/** @xreflabel{hdr:AsmMediaFmtVorbis}
    Media format ID for Vorbis.

    @structure{asm_vorbis_fmt_blk_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_WRITE_Vorbis_format_block.tex}
    @newpage
*/
#define ASM_MEDIA_FMT_VORBIS                    0x00010C15

/* Vorbis decoder format block structure. */
typedef struct asm_vorbis_fmt_blk_t asm_vorbis_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @brief Vorbis decoder format block structure used in the
    #ASM_STREAM_CMD_OPEN_WRITE_V2 command.
*/
struct asm_vorbis_fmt_blk_t
{
    uint32_t          bit_stream_fmt;
    /**< Transcoded bitstream containing the size of the frame as the first
         word in each frame.

         Supported values:
         - 0 -- Raw bitstream
         - 1 -- Transcoded bitstream
         
         Currently, the Vorbis decoder library supports only a transcoded
         bitstream.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_vorbis */

/** @addtogroup asm_strm_media_format_flac
@{ */
/** @xreflabel{hdr:AsmMediaFmtFlac}
    Media format ID for FLAC.

    @structure{asm_flac_fmt_blk_t}
    @tablespace
    @inputtable{ASM_STREAM_CMD_OPEN_WRITE_FLAC_format_block.tex}
    @newpage
*/
#define ASM_MEDIA_FMT_FLAC                      0x00010C16

/* FLAC decoder format block structure. */
typedef struct asm_flac_fmt_blk_t asm_flac_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @brief FLAC decoder format block structure used in the
    #ASM_STREAM_CMD_OPEN_WRITE_V2 command.
*/
struct asm_flac_fmt_blk_t
{
    uint16_t          is_stream_info_present;
    /**< Specifies whether stream information is present in the FLAC format
         block.

         Supported values:
         - 0 -- Stream information is not present in this message
         - 1 -- Stream information is present in this message

         When set to 1, the FLAC bitstream was successfully parsed by the
         client, and other fields in the FLAC format block can be read by the
         decoder to get metadata stream information. */

    uint16_t          num_channels;
    /**< Number of channels for multichannel decoding.

         Supported values: 1 to 8 */

    uint16_t          min_blk_size;
    /**< Minimum block size (in samples) used in the stream. It must be less
         than or equal to max_blk_size. */

    uint16_t          max_blk_size;
    /**< Maximum block size (in samples) used in the stream. If the minimum
         block size equals the maximum block size, a fixed block size stream
         is implied. */

    uint16_t          md5_sum[8];
    /**< MD5 signature array of the unencoded audio data. This allows the
         decoder to determine if an error exists in the audio data, even when
         the error does not result in an invalid bitstream. */

    uint32_t          sample_rate;
    /**< Number of samples per second (in Hertz).

         Supported values:  8000 to 48000 */

    uint32_t          min_frame_size;
    /**< Minimum frame size (in bytes) used in the stream. If 0, the value is
         unknown. */

    uint32_t          max_frame_size;
    /**< Maximum frame size (in bytes) used in the stream. If 0, the value is
         unknown. */

    uint16_t          sample_size;
    /**< Bits per sample.

         Supported values: 8, 16 */

    uint16_t          reserved;
    /**< Reserved. Clients must set this field to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_flac */

/** @ingroup asm_strm_media_format_dts
    @xreflabel{hdr:AsmMediaFmtDts}
    Media format ID for Digital Theater Systems. This ID is used by the
    #ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED command only.
    @newpage
*/
#define ASM_MEDIA_FMT_DTS                    0x00010D88

/* DTS encoder configuration structure. */
typedef struct asm_dts_enc_cfg_t asm_dts_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the DTS encoder configuration parameters in the
    #ASM_MEDIA_FMT_DTS media format.
*/
struct asm_dts_enc_cfg_t
{
    uint32_t          sample_rate;
    /**< Samples at which input is to be encoded.

         Supported values:
         - 44100 -- encode at 44.1 Khz
         - 48000 -- encode at 48 Khz */

    uint32_t          num_channels;
    /**< Number of channels for multi-channel encoding.

	Supported values: 1 to 6 */

    uint8_t   channel_mapping[16];
    /**< Channel array of size 16. Channel[i] mapping describes channel I. Each
         element i of the array describes channel I inside the buffer where 0
         @le I < num_channels. An unused channel is set to zero. Only first
         num_channels elements are valid

         Supported values:
         - #PCM_CHANNEL_L
         - #PCM_CHANNEL_R
         - #PCM_CHANNEL_C
         - #PCM_CHANNEL_LS
         - #PCM_CHANNEL_RS
         - #PCM_CHANNEL_LFE
         @newpage */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_media_format_dts */

/** @ingroup asm_strm_media_format_atrac
    @xreflabel{hdr:AsmMediaFmtAtrac}
    Media format ID for adaptive transform acoustic coding. This ID is used by
    the #ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED command only.
*/
#define ASM_MEDIA_FMT_ATRAC                  0x00010D89

/** @ingroup asm_strm_media_format_mat
    @xreflabel{hdr:AsmMediaFmtMat}
    Media format ID for metadata-enhanced audio transmission. This ID is used
    by the #ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED command only.
    @newpage
*/
#define ASM_MEDIA_FMT_MAT                    0x00010D8A
/** @ingroup asm_strm_media_format_dts
    @xreflabel{hdr:AsmMediaFmtDts}
    Media format ID for Digital Theater Systems. This ID is used by the
    #ASM_STREAM_CMD_OPEN_WRITE/#ASM_STREAM_CMD_OPEN_READ commands only.
    @newpage
*/
#define ASM_MEDIA_FMT_DTS_LBR                0x00010DBB

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_ADSP_MEDIA_FMT_H_*/
