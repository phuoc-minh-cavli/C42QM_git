/*==============================================================================
 *
 *                           EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the module.
 *   Notice that changes are listed in reverse chronological order.
 *
 * When         Who     Version  What
 * ----------   ---     -------  -----------------------------------------------
 * 01-23-2019   QC      1.12.2   Updating sensor threshold values
 *
 * 03-07-2019   QC      1.12.1   Updated sensor name to be the sensor model number
 *                               Fixed the driver version to comply with
 *                               major[31:16].minor[15:8].revision[7:0]
 * 01-30-2019   QC      1.12.0   Updated registry defaults for new HW
 * 08-13-2018   QC      1.11.0   Restricted handling of flush request such that
 *                               the only driver activity sends a flush complete event
 * 07-25-2018   SEN     1.10.0   Various bug fixes
 * 03-05-2018   SEN     1.9.0    Update to OpenSSC 5.0.8 and various bug fixes
 * 08-30-2017   SEN     1.8.0    Update to OpenSSC 5.0.5
 * 08-03-2017   SEN     1.7.0    Threshold values support in registry (for both
 *                               temperature and humidity)
 * 07-27-2017   QC      1.6.0    Update temp and humidity diff to 0.5
 * 07-21-2017   SEN     1.5.0    Bug fixes and full registry support
 * 07-12-2017   SEN     1.4.0    Island mode, periodic timer, registry support
 * 06-26-2017   SEN     1.3.1    Add revisioning file
 * 06-19-2017   SEN     1.3.0    Support communication test
 * 06-16-2017   QC      -        Update SHTW2 driver to OpenSSC 5.0.4
 * 05-05-2017   SEN     1.2.1    Update physical sensor config envent
 * 03-05-2017   SEN     1.2.0    Change threshold values
 * 03-05-2017   SEN     -        FIX humidity signal in SHTW2 driver
 * 19-04-2017   QC      -        Change driver from streaming to on-change mode
 * 02-15-2017   SEN     1.0.0    Initial driver implementation for OpenSSC 5.0.0
 *
 *==============================================================================
 */
#define SHTW2_DRIVER_VERSION 0x00010C02    // major[31:16].minor[15:8].revision[7:0]
