Step to upload csd.conf from local PC to Target 

eg: For APT voice testing, we need to enable wcd codec

Source:(Local PC)
  <APPS CRM>\apps_proc\multimedia\audio\csd\config\<config Type>\csd\csd.conf
Destination:(Target)
  /nv/item_files/csd/csd.conf

Instructions:

1. Open QPST
2. Ensure that 'MDM9xxx' is populated.
3. Choose 'Start Clients' -> 'EFS explorer'
4. A prompt for 'Phone Selection' will appear, Select 'MDM9xxx' 
5. in EFS browser, open'/nv/item_files/'
6. In APPS Meta Build, open '\apps_proc\multimedia\audio\csd\config\'
7. Open appropirate configuration, eg: 'config_01'
6. Drag and drop folder named 'csd' from PC to '/nv/item_files/'
7. Ensure that '/nv/item_files/csd/csd.conf' is present and its size is 64bytes
8. Close 'EFS explorer'
9. Reset the target

Note: Persistence rules for QCN applies to these files as well. That is, if you 
      erase the target, you wil have to load this again.
Note: To safely replace a config file in efs, first delete the previous copy 
      
Types of Configuration:

================================================================================
|   Config Type   |   QC Voice    |  WCD Codec |            Usecases           |
|(Folder Location)|   Processing  |            |                               |
================================================================================
| ..\config_00\.. |   Yes         |  Enable    |-TN build testing/bringup      |
|or no config file|               |            | MDM Demonstration             |
|                 |               |            | Proto-typing Solution         |
| ..\config_01\.. |   None        |  Enable    |-MN build testing/bringup      |
| ..\config_02\.. |   None        |  Disable   |-customer designs with 3rd     |
|                 |               |            | party voice processing and    |
|                 |               |            | 3rd party codec               |
| ..\config_03\.. |   Yes         |  Disable   |-customer with 3rd party codec |
|                 |               |            | with QC voice porcessing      |
================================================================================