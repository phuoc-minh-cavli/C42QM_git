***Read Me***
______________________________________________________________________________________________________________________
Contents: ( JUMP quickly by searching for the key word in brackets )

1. Pre-requisites. ( VS_Description )

2. Sequence of commands for Loop back:
    1. Sequence of commands to start loop back. ( VS_START_SEQ )
    2. Sequence of commands to change vocoder while running. ( VS_VOCODER_SWITCH )
    3. Sequence of commands to Stop and De-initialize the Loop back utility. ( VS_STOP_SEQ )

3. HLOS Sequence of commands ( HLOS_COMMANDS )

4. Voice diag command format (TESTCLIENT)

5. Brief description of commands exposed in loop back client ( VS_COMMANDS )
         - Payload supported for each Vocoders are explained here ( SET_VOCODER_PROP )

6. Supported media ID list ( MEDIA_ID )

7. Brief description of payloads in SEND_DATA command ( COMMAND_ID_DEC )

______________________________________________________________________________________________________________________

VS_Description:

Pre-requisites.
#) To verify loop back of all Vocoders HLOS, Modem and ADSP should be up.
#) Device need not Camp to the network.

For information on different vocoder properties refer to: "DCN: 80-NF774-31 Core Voice Driver API"

______________________________________________________________________________________________________________________

VS_START_SEQ:

Sequence to start loop back. 
1. Follow all the sequence in HLOS_COMMANDS section listed below to issue Start voice from HLOS.

2. Initialize VS loop back client 
 send_data 75 54 00 00 00 

3. Set Vocoder as AMR_NB
   AMR_NB - send_data 75 54 00 00 04 01 00 00 00 00
      ( Payload: Refer SET_VOCODER_PROP for setting up other vocoders and their properties )

4. Issue VS Start voice to start loop back.
 send_data 75 54 00 00 02
 
______________________________________________________________________________________________________________________

VS_VOCODER_SWITCH:

Sequence to change Vocoders while Running.
1. HLOS need not be stopped.

2. Issue Stop voice in VS by 
 send_data 75 54 00 00 03 
    
3. Set new vocoder properties as AMR_WB
   AMR_WB - send_data 75 54 00 00 04 01 01 00 00 00
      ( Payload: Refer SET_VOCODER_PROP for setting up other vocoders and their properties )
      
4. Issue Start voice again to start loop back with new vocoder
 send_data 75 54 00 00 02 
    
______________________________________________________________________________________________________________________

VS_STOP_SEQ:

Sequence to Stop and De-initialize
1. Issue destroy sequence from HLOS by CTRL + C in cmd prompt where HLOS was initialized.

2. Issue Stop voice in VS by 
 send_data 75 54 00 00 03 
 
3. Issue VS De-initialize sequence 
 send_data 75 54 00 00 01

______________________________________________________________________________________________________________________

HLOS_COMMANDS

Following are the HLOS voice sequence for 8250:
1. Open adb shell command
2. Following to ensure audio is enabled
  cat /proc/asound/cards
3. Following commands to push tinyvoice app
  adb root && adb wait-for-device remount && adb wait-for-device
  adb push \\good\mmaudio02\VS_LOOPBACK\voice\8250\tinyvoice /system/bin
  adb shell chmod 777 /system/bin/tinyvoice
4a. In the same adb shell, issue following to setup headset
    tinymix "RX_MACRO RX0 MUX" "AIF1_PB"
    tinymix "RX_MACRO RX1 MUX" "AIF1_PB"
    tinymix "RX_CDC_DMA_RX_0 Channels" "Two"
    tinymix "RX INT0_1 MIX1 INP0" "RX0"
    tinymix "RX INT1_1 MIX1 INP0" "RX1"
    tinymix "RX INT0 DEM MUX" "CLSH_DSM_OUT"
    tinymix "RX INT1 DEM MUX" "CLSH_DSM_OUT"
    tinymix "RX_COMP1 Switch" 1
    tinymix "RX_COMP2 Switch" 1
    tinymix "HPHL_RDAC Switch" 1
    tinymix "HPHR_RDAC Switch" 1
    tinymix "HPHL_COMP Switch" 1
    tinymix "HPHR_COMP Switch" 1
    tinymix "RX_HPH_PWR_MODE" "LOHIFI"
    tinymix "RX HPH Mode" "CLS_H_LOHIFI"
    tinymix "RX_RX0 Digital Volume" 84
    tinymix "RX_RX1 Digital Volume" 84

    tinymix 'TX DEC0 MUX' 'SWR_MIC'
    tinymix 'TX SMIC MUX0' 'ADC1'
    tinymix 'TX_CDC_DMA_TX_3 Channels' 'One'
    tinymix 'TX_AIF1_CAP Mixer DEC0' 1
    tinymix 'ADC2 Volume' 62
    tinymix 'ADC2_MIXER Switch' 1
    tinymix 'ADC2 MUX' 'INP2'

    tinymix 'RX_CDC_DMA_RX_0_Voice Mixer VoiceMMode1' 1
    tinymix 'VoiceMMode1_Tx Mixer TX_CDC_DMA_TX_3_MMode1' 1
    tinyvoice

4b Alternatively, issue following to setup speakerphone
    tinymix 'WSA_CDC_DMA_RX_0 Channels' 'One'
    tinymix 'WSA RX0 MUX' 'AIF1_PB'
    tinymix 'WSA_RX0 INP0' 'RX0'
    tinymix 'WSA_COMP1 Switch' 1
    tinymix 'SpkrLeft COMP Switch' 1
    tinymix 'SpkrLeft BOOST Switch' 1
    tinymix 'SpkrLeft VISENSE Switch' 1
    tinymix 'SpkrLeft SWR DAC_Port Switch' 1
    tinymix 'TX DEC0 MUX' 'MSM_DMIC'
    tinymix 'TX DMIC MUX0' 'DMIC0'
    tinymix 'TX_CDC_DMA_TX_3 Channels' 'One'
    tinymix 'TX_AIF1_CAP Mixer DEC0' 1
    tinymix 'TX_DEC0 Volume' 84
    tinymix 'WSA_CDC_DMA_RX_0_Voice Mixer VoiceMMode1' 1
    tinymix 'VoiceMMode1_Tx Mixer TX_CDC_DMA_TX_3_MMode1' 1
    tinyvoice
  
5. Following to send stop voice sequence
    ctrl + c

Following are the HLOS voice sequence for 8996 and 8998:
1. Open adb shell command
2. Following to ensure audio is enabled
  cat /proc/asound/cards
3. Following commands to push tinyvoice app
  adb root && adb wait-for-device remount && adb wait-for-device
  adb push \\good\mmaudio02\VS_LOOPBACK\voice\tinyvoice /system/bin
  adb shell chmod 777 /system/bin/tinyvoice
4. In the same adb shell, issue following to setup headphones/headset-mic
  tinymix 'SLIM RX0 MUX' 'AIF_MIX1_PB'
  tinymix 'SLIM RX1 MUX' 'AIF_MIX1_PB'
  tinymix 'SLIM_0_RX Channels' 'Two'
  tinymix 'RX INT1_1 MIX1 INP0' 'RX0'
  tinymix 'RX INT2_1 MIX1 INP0' 'RX1'
  tinymix 'RX INT1 DEM MUX' 'CLSH_DSM_OUT'
  tinymix 'RX INT2 DEM MUX' 'CLSH_DSM_OUT'
  tinymix 'COMP1 Switch' 1
  tinymix 'COMP2 Switch' 1

  tinymix 'SLIM_0_TX Channels' 'One'
  tinymix 'AIF1_CAP Mixer SLIM TX6' 1
  tinymix 'SLIM TX6 MUX' 'DEC6'
  tinymix 'ADC MUX6' 'AMIC'
  tinymix 'AMIC MUX6' 'ADC2'
  tinymix 'DEC6 Volume' 84
  //the following command is not requried for 8996
  echo "0623 50" > /d/regmap/tasha-slim-pgd/registers
  
  tinymix "SLIM_0_RX_Voice Mixer VoiceMMode1" 1
  tinymix "VoiceMMode1_Tx Mixer SLIM_0_TX_MMode1" 1
  tinyvoice
5. Following to send stop voice sequence
  ctrl + c

  
Following are the HLOS voice sequence for 670/710:

1. Open adb shell command
2. Following to ensure audio is enabled
  cat /proc/asound/cards
3. Following commands to push tinyvoice app
  adb root && adb wait-for-device remount && adb wait-for-device
  adb push \\smaliyap-linux\workspace\dropbox\8998_bu\voice\tinyvoice /system/bin
  adb shell chmod 777 /system/bin/tinyvoice
4. In the adb shell, issue following to setup handset
   tinymix 'SLIM RX0 MUX' 'AIF1_PB'
   tinymix 'CDC_IF RX0 MUX' 'SLIM RX0'
   tinymix 'SLIM_0_RX Channels' 'One'
   tinymix 'RX INT7_1 MIX1 INP0' 'RX0'
   tinymix 'COMP7 Switch' '1'
   tinymix 'SpkrLeft COMP Switch' '1'
   tinymix 'SpkrLeft BOOST Switch' '1'
   tinymix 'SpkrLeft SWR DAC_Port Switch' '1'
   tinymix 'EAR SPKR PA Gain' 'G_6_DB'


   tinymix 'AIF1_CAP Mixer SLIM TX7' 1
   tinymix 'CDC_IF TX7 MUX' 'DEC7'
   tinymix 'SLIM_0_TX Channels' 'One'
   tinymix 'ADC MUX7' 'DMIC'
   tinymix 'DMIC MUX7' 'DMIC2'
   tinymix 'IIR0 INP0 MUX' 'DEC7'

   tinymix "SLIM_0_RX_Voice Mixer VoiceMMode1" 1
   tinymix "VoiceMMode1_Tx Mixer SLIM_0_TX_MMode1" 1
   tinyvoice   
5. Following to send stop voice sequence
  ctrl + c

  
Following are the HLOS voice sequence for 855:

1. Open adb shell command
2. Following to ensure audio is enabled
   cat /proc/asound/cards
3. Following commands to push tinyvoice app
   adb root
   adb remount
   adb push \\smaliyap-linux\workspace\dropbox\8998_bu\voice\tinyvoice /system/bin
   adb shell chmod 777 /system/bin/tinyvoice
4. In the adb shell, issue following:
   tinymix 'AIF1_CAP Mixer SLIM TX7' 1
   tinymix 'CDC_IF TX7 MUX' 'DEC7'
   tinymix 'SLIM_0_TX Channels' 'One'
   tinymix 'ADC MUX7' 'DMIC'
   tinymix 'DMIC MUX7' 'DMIC2'
   tinymix 'DEC7 Volume' 84
   tinymix 'SLIM RX0 MUX' 'AIF1_PB'
   tinymix 'CDC_IF RX0 MUX' 'SLIM RX0'
   tinymix 'SLIM_0_RX Channels' 'One'
   tinymix 'RX INT7_1 MIX1 INP0' 'RX0'
   tinymix 'COMP7 Switch' 1
   tinymix 'SpkrLeft COMP Switch' 1
   tinymix 'SpkrLeft BOOST Switch' 1
   tinymix 'SpkrLeft SWR DAC_Port Switch' 1
   tinymix 'SLIM_0_RX_Voice Mixer VoiceMMode1' 1
   tinymix 'VoiceMMode1_Tx Mixer SLIM_0_TX_MMode1' 1
   tinyvoice 
5. Following to send stop voice sequence
  ctrl + c
_____________________________________________________________________________________________________________________

TESTCLIENT

VOICE DIAG command format (every field is a uint8_t value):
            send_data <diagID> <subsystemID> <cmdCodeLow> <cmdCodeHigh> <cmdID> <cmdSpecificPayload>
Example:
            send_data <diagID> <subsystemID> <cmdCodeLow> <cmdCodeHigh> <vsTestInitCmd>
            send_data <diagID> <subsystemID> <cmdCodeLow> <cmdCodeHigh> <setParamCmd> <paramID> <paramPayload1> <paramPayload2> <paramPayload3> <paramPayload4>            
            send_data <diagID> <subsystemID> <cmdCodeLow> <cmdCodeHigh> <vsTestStartLoopbackCmd>
            send_data <diagID> <subsystemID> <cmdCodeLow> <cmdCodeHigh> <vsTestStopLoopbackCmd>
            send_data <diagID> <subsystemID> <cmdCodeLow> <cmdCodeHigh> <vsTestDeinitCmd> 

Note: diagID = 75, subsystemID(MVS) = 54, cmdCodeLow = 0, cmdCodeHigh = 0. (Parameter here on are custom defined the diag client)
Please refer to interface definitions for each of remaining payload at end of this file.
______________________________________________________________________________________________________________________

VS_COMMANDS

Following Sections describes briefly about the commands supported by VS loop back client 

1. Initialize loop back client, Creates loop back threads and initialize sessions
 send_data 75 54 00 00 00 
    ( Payload: none )

2. De-initialize loop back client - Destroys the loop back sessions and the thread.
 send_data 75 54 00 00 01
 
3. Start voice - Sets cached vocoder properties and starts the vocoder session.
 send_data 75 54 00 00 02 
    ( Payload: none )
 
4. Stop voice - Stops the vocoder
 send_data 75 54 00 00 03 
    ( Payload: none )

6. SET_VOCODER_PROP
    - payload1 - sub command ID.
    1) Set DTX mode during loop back
       send_data 75 54 00 00 04 02 00 00 00 00 
       ( SUB Command ID: 02
         Payload: enable/disable )
       
    2) Set media ID alone 
       send_data 75 54 00 00 04 01 00 00 00 00
       ( SUB Command ID: 01
         Payload2: Media ID, Supported list is at MEDIA_ID below )
       
    3) Minimum and Maximum Rate ( media id has to be set before using this command )
        - Specific to EVRC Vocoders 
       send_data 75 54 00 00 04 04 00 00 00 00
       ( SUB Command ID: 04 
         Payload2: Min rate, 
         Payload3: Max rate { 0 to 7 based on the vocoder } )
       
    4) Set Encoder rate ( media id has to be set before using this command )
        - Specific to AMR, AMR_WB and EVRC Vocoders 
       send_data 75 54 00 00 04 03 00 00 00 00
       ( SUB Command ID: 03
         Payload2: encoder rate { 0 to 4 based on the vocoder } )
    
    5) Codec mode and bandwidth ( media id has to be set before using this command )
        - Specific to EVS Vocoder
       send_data 75 54 00 00 04 8 00 00 00 00
       ( SUB Command ID: 8
         Payload2: Codec mode, 
         Payload3: Bandwidth { 0 - 8000, 1 - 16000, 2 - 32000, 3 - 48000 } )
       
    6) Set Maximum Tx and Rx Sampling rate ( media id has to be set before using this command )
        - Specific to EVS Vocoder
       send_data 75 54 00 00 04 06 00 00 00 00
       ( SUB Command ID: 06
         Payload2: Tx Sampling rate, 
         Payload3: Rx Sampling rate { 0 - 8000, 1 - 16000, 2 - 32000, 3 - 48000 } )
       
    7) Channel Aware mode Enable/ Disable for EVS 
       send_data 75 54 00 00 04 07 00 00 00 00 
       ( SUB Command ID: 07
	     Payload2: Enable/Disable
         Payload3: FEC offset { 2, 3, 5, 7 } 
         Payload4: FER rate { 0, 1 } )
       
______________________________________________________________________________________________________________________

MEDIA_ID
Following shows the payload to media ID mapping in voice loop back utility.
Uncomment VS_PCM_VOCODER feature flag definition in vs.scons if PCM vocoder support is needed
____________________
Payload - Media ID
____________________
0 - AMR-NB
1 - AMR-WB
2 - EFR
3 - FR
4 - HR
5 - EVRC A
6 - EVRC B 
7 - EVRC WB
8 - EVRC NW
9 - EVRC NW2k 
10 - QCELP 13
11 - G711 a-law V2
12 - G711 mu-law V2
13 - EVS
14 - PCM 8 KHz
15 - PCM 16 KHz
16 - PCM 32 KHz
17 - PCM 48Khz
18 - EAMR
______________________________________________________________________________________________________________________

COMMAND_ID_DEC
Description for the payload used with send_data command in QXDM.
75    - Diag server ID.
54    - Voice sub-system ID.
00 00 - Start and end of the command. ( Voice loopback command ID - 00 00 )
00    - VS Command ID.
00    - payload1 ( Used as sub command ID for SET_VOCODER_PROP )
00    - payload2 ( Payloads for SET_VOCODER_PROP )
00    - payload3 
00    - payload4 
00    - payload5 
_____________________________________________________________________________________________________________________