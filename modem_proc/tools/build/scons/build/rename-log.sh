#===============================================================================
#
# CBSP Buils system
#
# General Description
#    build rename log file.
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //commercial/MPSS.JO.1.2.r3.1/Main/modem_proc/tools/build/scons/build/rename-log.sh#1 $
# $DateTime: 2016/02/11 01:26:26 $
# $Author: mplcsds1 $
# $Change: 9883334 $
#                      EDIT HISTORY FOR FILE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
#
#===============================================================================

log_file_name="build-log"
log_file_ext=".txt"
log_file="$log_file_name$log_file_ext"

if [ -f $log_file ]; then
   for i in {001..1000}; do
      i=`printf '%.03d' $i`
      old_log_file="$log_file_name-$i$log_file_ext"
      
      if [ ! -f $old_log_file ]; then
         break
      fi
   done

   if [ $i != "1000" ]; then
      mv $log_file $old_log_file
   fi
fi 

