#ifndef WCDCODECUUID_H
#define WCDCODECUUID_H
/**
@file wcd_codec_uids.h
@brief Has all the device UUIDs related to the database
*/
/*-------------------------------------------------------------------------------
    Copyright (c) 2013 Qualcomm Technologies Incorporated.
    All rights reserved.
    Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                        Edit History
$Id: //components/rel/audio.tx/2.0/audio/api/adie/wcd_codec_uids.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/01/14   AG      Header update
06/01/11   OA	   Initial revision
===============================================================================*/

/*
  These profiles are names asigned to each one of the devices,
  the profilenames are used to determine the physical configurations
  such as pmic gpios to be enabled, please note the profile is used
  to assing the analog configuration regardless of the order of the 
  channels, say if for one device LO1 is used for left and LO2 is used
  for right channel the profile id used will be the same as if LO1 is
  used for Right and LO2 for left, channel order is assigned on the 
  profile header and depends on the COPP configuration selected by the 
  client.
  
  New profile Ids can be added to represent configurations not shown later
  on this file. Important note most of these profile names may not be used 
  but they are kept for future reference and usage.
  
  
  each profile ID is represented by the sum of the following masks
  
  0x000D0000  D represents direction:
               1 = RX 
               2 = TX
  0x0000T000  I represents type identifier
              for Rx devices
                0 = Handset
                1 = Headset
                2 = Line out (speaker)
                3 = Differential Line out (speaker)
                4 = Single Slimbus Port
                5 = Integrated Class D Speaker
              for Tx devices
                0 = ADC
                1 = DMIC
                2 = Aux PGA
  0x00000C00  C represents number of channels  
  0x000000XX  XX represents a unique (serial) identifier to diferentiate
                  among different variants
*/

// RX Profile IDs
#define ADIE_CODEC_HANDSET_CAPL_AB_RX                 0x00010100
#define ADIE_CODEC_HANDSET_LEG_AB_RX                  0x00010101
#define ADIE_CODEC_HANDSET_H_RX                       0x00010102

#define ADIE_CODEC_HEADSET_CAPL_AB_MONO_LEFT_RX       0x00011100
#define ADIE_CODEC_HEADSET_LEG_AB_MONO_LEFT_RX        0x00011101
#define ADIE_CODEC_HEADSET_G_MONO_LEFT_RX             0x00011102
#define ADIE_CODEC_HEADSET_CAPL_AB_MONO_RIGHT_RX      0x00011103
#define ADIE_CODEC_HEADSET_LEG_AB_MONO_RIGHT_RX       0x00011104
#define ADIE_CODEC_HEADSET_G_MONO_RIGHT_RX            0x00011105
#define ADIE_CODEC_HEADSET_H_MONO_RX                  0x00011106
#define ADIE_CODEC_HEADSET_H_MONO_RIGHT_RX            0x00011107

#define ADIE_CODEC_HEADSET_CAPL_AB_STEREO_RX          0x00011200
#define ADIE_CODEC_HEADSET_LEG_AB_STEREO_RX           0x00011201
#define ADIE_CODEC_HEADSET_G_STEREO_RX                0x00011202
#define ADIE_CODEC_HEADSET_H_STEREO_RX                0x00011203

#define ADIE_CODEC_SPEAKER_MONO_LO1_RX                0x00012100
#define ADIE_CODEC_SPEAKER_MONO_LO2_RX                0x00012101
#define ADIE_CODEC_SPEAKER_MONO_LO3_RX                0x00012102
#define ADIE_CODEC_SPEAKER_MONO_LO4_RX                0x00012103
#define ADIE_CODEC_SPEAKER_MONO_LO5_RX                0x00012104

#define ADIE_CODEC_SPEAKER_STEREO_LO1_LO2_RX          0x00012200
#define ADIE_CODEC_SPEAKER_STEREO_LO3_LO4_RX          0x00012201

#define ADIE_CODEC_SPEAKER_5_1_RX                     0x00012600

#define ADIE_CODEC_SPEAKER_DIFF_MONO_LO1_LO3_RX       0x00013100
#define ADIE_CODEC_SPEAKER_DIFF_MONO_LO2_LO4_RX       0x00013101

#define ADIE_CODEC_SPEAKER_DIFF_STEREO_L13_L24_RX     0x00013200

#define ADIE_CODEC_HANDSET_G_HEADSET_G_MONO_RIGHT_RX_SINGLEPORT     0x00014200
#define ADIE_CODEC_HANDSET_H_CLASS_D_SPEAKER_RX_SINGLEPORT          0x00014201

#define ADIE_CODEC_INTEGRATED_CLASS_D_SPEAKER_RX      0x00015100

// TX Profile IDs
#define ADIE_CODEC_ADC_1_MONO_TX                      0x00020100
#define ADIE_CODEC_ADC_2_MONO_TX                      0x00020101
#define ADIE_CODEC_ADC_3_MONO_TX                      0x00020102
#define ADIE_CODEC_ADC_4_MONO_TX                      0x00020103
#define ADIE_CODEC_ADC_5_MONO_TX                      0x00020104
#define ADIE_CODEC_ADC_6_MONO_TX                      0x00020105

#define ADIE_CODEC_ADC_13_STEREO_TX                   0x00020200
#define ADIE_CODEC_ADC_45_STEREO_TX                   0x00020201
#define ADIE_CODEC_ADC_14_STEREO_TX                   0x00020202

#define ADIE_CODEC_ADC_1234_QUAD_TX                   0x00020400


#define ADIE_CODEC_DMIC_1_MONO_TX                     0x00021100
#define ADIE_CODEC_DMIC_2_MONO_TX                     0x00021101
#define ADIE_CODEC_DMIC_3_MONO_TX                     0x00021102
#define ADIE_CODEC_DMIC_4_MONO_TX                     0x00021103
#define ADIE_CODEC_DMIC_5_MONO_TX                     0x00021104
#define ADIE_CODEC_DMIC_6_MONO_TX                     0x00021105

#define ADIE_CODEC_DMIC_12_STEREO_TX                  0x00021200
#define ADIE_CODEC_DMIC_13_STEREO_TX                  0x00021201
#define ADIE_CODEC_DMIC_14_STEREO_TX                  0x00021202
#define ADIE_CODEC_DMIC_24_STEREO_TX                  0x00021203
#define ADIE_CODEC_DMIC_12_STEREO_AUDIO_TX            0x00021204

#define ADIE_CODEC_DMIC_1234_QUAD_TX                  0x00021400


#define ADIE_CODEC_AUXPGA_L_TX                        0x00022100
#define ADIE_CODEC_AUXPGA_R_TX                        0x00022101

#endif /* WCDCODECUUID_H */


