#------------------------------------------------------------------------------
# This script is used to satisfy CRM team's requirement of a recompile command
# For the case of TN which has adopted a new strip/pack process, the only 
# purpose of this file will be to delete the existing BuildProducts.txt and 
# regenerate it.
# Generate BuildProducts.txt
#------------------------------------------------------------------------------
rm -f BuildProducts.txt
echo HY11_CompileTest/apps_proc/build/ms/bin/ACIMAAAA/acdb.mbn > BuildProducts.txt
echo HY11_CompileTest/apps_proc/build/ms/bin/ACIMAAAA/amss_mbn.mbn >> BuildProducts.txt
echo HY11_CompileTest/apps_proc/build/ms/bin/ACIMAAAA/apps.mbn >> BuildProducts.txt
