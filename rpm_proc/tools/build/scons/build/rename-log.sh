#===============================================================================
#
# CBSP Buils system
#
# General Description
#    build rename log file.
#
# Copyright (c) 2009-2009 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/rpm.bf/2.1.3/tools/build/scons/build/rename-log.sh#1 $
# $DateTime: 2018/12/26 21:36:05 $
# $Author: pwbldsvc $
# $Change: 17934472 $
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

