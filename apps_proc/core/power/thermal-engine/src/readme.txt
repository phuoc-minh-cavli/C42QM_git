/*===========================================================================

Copyright (c) 2013 QUALCOMM Incorporated.  All Rights Reserved.
QUALCOMM Proprietary and Confidential.

===========================================================================*/

thermal-engine
~~~~~~~~~~~~~~

This daemon monitors thermal/temperature sensor data and performs
actions based on a configuration file (default is no config file).

A sample configuration file is provided (thermal-engine.conf_sample).


Configuration File Details
~~~~~~~~~~~~~~~~~~~~~~~~~~
File location:
"/nv/item_files/thermal-engine/thermal-engine.conf"
Configuration file must be newline terminated and padded to exactly 2048 bytes.


Configuration file format:
~~~~~~~~~~~~~~~~~~~~~~~~~~
sampling <default sampling rate in ms>

[<Algorithm instance label>] LABEL MUST BE UNIQUE MAX LEN 31 CHARACTERS
algo_type      <algorithm type for instance>  MUST BE FIRST FIELD IN USER
                                              DEFINED SECTION.
disable        <OPTIONAL field used to disable default target algorithm
                instances.
....

[<Algorithm instance label>]
algo_type      monitor
sensor         <sensor name>
sampling       <OPTIONAL sampling rate in ms>
descending     <OPTIONAL threshold trigger direction is falling below value
                as opposed to default behavior rising above.>
thresholds     <threshold values in degree mC or mA> ...
thresholds_clr <temperature value to clear thresholds> ...
actions        <action to perform at threshold;
                multiple actions separated by '+'> ...
action_info    <additional info for action;
                multiple action_info separated by '+'> ...

* 'thresholds'/'thresholds_clr'/'actions'/'action_info' accepts a list of
  space separated values for up to 8 thresholds.

'actions' field
---------------
* 'none'
- ACTION - Do nothing
- ACTION_INFO - ignored

* 'modem'  (NOT CURRENTLY SUPPORTED)
- ACTION - Request throttling of modem functionality
- ACTION_INFO - 0-3 throttling level for modem mitigation

* 'vdd_min_disable'
- ACTION - Request disabling of less than nominal voltage levels across
           digital domains.
- ACTION_INFO - 0 Voltage restrictions disabled, 1 Voltage restrictions enabled

Example 1:
----------
sampling         1000

[PA_MONITOR]
algo_type        monitor
descending       1
sensor           tsens_tz_sensor0
thresholds       45000  50000  65000
thresholds_clr   43000  48000  63000
actions          modem  modem  modem
action_info      1      2      3

Description:
1) Default sampling rate of 1 second if sensor doesn't support interrupt
   triggers.
2) Threshold level 1 triggered at 45 deg C; clears if temperature drops
   to/below 43 deg C.  When triggered, set modem mitigation level to 1.
3) Threshold level 2 triggered at 50 deg C; clears if temperature drops
   to/below 48 deg C.  When triggered, set modem mitigation level to 2.
3) Threshold level 3 triggered at 65 deg C; clears if temperature drops
   to/below 63 deg C.  When triggered, set modem mitigation level to 3.

