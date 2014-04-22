How to Enable eAMR & Sample Rate Notification for all GSM, WCDMA, TDSCDMA and VoLTE. 

By default, eAMR and e-AMR SRN is disabled.

First create a folder named voice in EFS Explorer under /nv/item_files/

1. Enable eAMR and e-AMR SRN for GSM, WCDMA, TDSCDMA and VoLTE.
Drag and drop the voicecfg.bin from modem build
  Modem_build_root\modem_proc\avs\vsd\voicecfg\sample_vcfg\eAMR_with_SRN
to target EFS Explorer /nv/item_files/voice/
Reboot target.

2. Enable only e-AMR without SRN for GSM, WCDMA, TDSCDMA and VoLTE.
Drag and drop the voicecfg.bin from modem build
  Modem_build_root\modem_proc\avs\vsd\voicecfg\sample_vcfg\eAMR_without_SRN
to target EFS Explorer /nv/item_files/voice/
Reboot target.


