
Patches are generated automatically(in this directory) while building apps_proc.
These patches are to demonstrate/incorporate Qualcomm specific customizations on original threadx codebase.

Patches are  text files which can be loaded into any comparison tool. (see loading_patch_using_GUI.jpg)

Applying patch automatically:
Unix "patch" command can also be used for merging/extracting all changes from a patch file.
command : "patch < patch-file-name"   ==> this command should be executed from each source directory containing at least one file listed in patch. 



This directory also has 2 header files required for applying patch and to make it able to compile and build.
Rename these files as "fx_api.h" and "tx_internal.h" and copy them to include header directory.

