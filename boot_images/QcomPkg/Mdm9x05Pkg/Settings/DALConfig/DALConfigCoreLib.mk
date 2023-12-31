# @file DALConfigCoreLib.mk
#
#  DAL configuration Tools library Custom Makefile
#
#  Copyright (c) 2015-2018,2019 Qualcomm Technologies, Inc.
#  All rights reserved.
#  Qualcomm Confidential and Proprietary
#
#

#==============================================================================
#                              EDIT HISTORY
#
#  $Header: $
#  $DateTime: $
#  $Author:  $
#
# when       who     what, where, why
# --------   ---     ----------------------------------------------------------
# 01/30/18   vg      Added spi/i2c config xml 
# 01/30/17   ska     Added spi config xml 
# 03/28/16   ss      Updating BAM xml config support
# 02/23/16   ap      Added dependency fix for C Config files
# 02/12/16   ts      Added pcie xml config support
# 12/15/15   bh      Settings relocation
# 03/12/15   ap      Added DALConfigCoreLib.mk for DALConfigCoreLib.inf
#
#==============================================================================



#=======================================README===========================================
# Device Config user adding new xml or updating existing xml need to update
# modify the following variables:
# 1]  INC_DEPS
# 2]  XML_DEPS
#
# XML_DEPS need to be modified if a user adds a config xml in DALConfigLib
# folder of any of its subdirectories. The proper path of the xml file needs
# to be appended to the variable XML_DEPS
#
# INC_DEPS need to be modified if the added config xml file includes (#include directive)
# any header file(s). Proper paths of the header files need to be appended to
# the variable INC_DEPS
# Nothing else should be modified in the file.
#
# Any xxxx.dec Package entries meant to be added in DALConfigCoreLib.inf have to be
# now added to the DALGenLibCore.inf
#========================================================================================


MACRO_NAME              =  core

#
# Following macro i.e. INC_DEPS lists all the header files that
# are included in the xml files. Any new header file added in the
# config xml files need to be appended to the following list
#
INC_DEPS = $(WORKSPACE)/QcomPkg/Include/api/dal/*.h \
           $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Settings/DALConfig/*.h \
           $(WORKSPACE)/QcomPkg/Library/PmicLib/framework/inc/pm_dal_prop_ids.h \

#
# Following macro i.e. XML_DEPS lists all the xml config files that
# It also enlists any xml files which are #included by other xml files.
# Any new xml file added will need to be appended to the following list.
#
XML_DEPS = $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Settings/DALSystem/$(MACRO_NAME)/dalsystem_$(MACRO_NAME).xml \
           $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Settings/ChipInfo/DalChipInfo.xml \
           $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Settings/Clock/ClockChipset.xml \
           $(WORKSPACE)/QcomPkg/Settings/Clock/$(MACRO_NAME)/DalClock.xml \
           $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Settings/HWIO/$(MACRO_NAME)/DalHWIO.xml \
           $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Settings/I2C/$(MACRO_NAME)/i2c_loader_9205.xml \
           $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Settings/SPI/$(MACRO_NAME)/spi_uefi_405.xml \
           $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Settings/PMIC/$(MACRO_NAME)/pm_config_target_$(MACRO_NAME).xml \
           $(WORKSPACE)/QcomPkg/Settings/PMIC/pm_config_common.xml \
           $(WORKSPACE)/QcomPkg/Settings/PlatformInfo/PlatformInfo.xml \
           $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Settings/TSENS/tsens_props.xml \
           $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Settings/ADC/internal/core/adc_props.xml \
           $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Settings/ADC/internal/core/vadc_props.xml \
           $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Settings/BAM/bam_config.xml \
           #$(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Settings/Qusb/qusb.xml \


#=====================USERS NEED NOT EDIT ANYTHING BEYOND THIS LINE========================

#
# Build Macro
#
OBJECT_FILES =  \
    $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).obj



INC = @$(MODULE_BUILD_DIR)/../DALGenLibCore/OUTPUT/inc_list.inc

OBJECT_FILES_LIST = $(OUTPUT_DIR)/object_files.lst


DALCONFIG_C        =  $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).c
DALCONFIG_d        =  $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).d
DALCONFIG_XML      =  $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).xml
DALCONFIG_XML_PRE  =  $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME)_pre.xml
DALCONFIG_XML_OUT  =  $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME)_out.xml

#=======================================================================
#Include the dependency file for DALConfig_<MACRO>.c file
#If any files #included by the DALConfig C file are changed,
#Then DALConfig_<MACRO>.obj/lib and the dependency file are rebuilt.

#The first 3 lines below are targeted for NMAKE. GNUMake ignores them
#thinking that they are comments. We exploit the following fact for this:
#"NMAKE doesn't recognize line continuation character in comments whereas Make does".
#In essence, to enforce GNU Make to bypass a line, use a trailing "# \" on the very
#preceeding line.

#The next 3 lines are intended for GNUMake. We enforce NMAKE to bypass it by
#using the "!IF 0" preprocessor directive. Here again we again use "# \" to bypass
#syntax meant for NMAKE i.e "!IF 0" and "!ENDIF".

# \
!IF EXIST($(DALCONFIG_d)) # \
!include $(DALCONFIG_d)   # \
!ENDIF # \

# \
!IF 0
-include $(DALCONFIG_d) # \
!ENDIF

#======================================================================
#
# Individual Object Build Targets
#
$(OUTPUT_DIR)/DALConfigLib.lib : $(OBJECT_FILES)
	"$(SLINK)" -cr -D  $(OUTPUT_DIR)/DALConfigLib.lib $(SLINK_FLAGS) $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).obj

$(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).obj : $(MAKE_FILE)
$(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).obj : $(COMMON_DEPS)
$(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).obj : $(DALCONFIG_C) $(DALCONFIG_d)
	"$(CC)" $(CC_FLAGS) -o $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).obj $(INC) $(DALCONFIG_C)

$(DALCONFIG_d) : $(DALCONFIG_C)
	( "$(CC)" $(CC_FLAGS) $(INC) -MM -MT "$(OBJECT_FILES) $(DALCONFIG_d)" $(DALCONFIG_C) -o $(DALCONFIG_d) )

$(DALCONFIG_C) : $(DALCONFIG_XML_OUT)
	( python $(WORKSPACE)/QcomPkg/Tools/propgen.py $(DALCONFIG_XML_OUT) $(OUTPUT_DIR) DALConfig_$(MACRO_NAME).c )

$(DALCONFIG_XML_OUT) :  $(DALCONFIG_XML_PRE)
	( python $(WORKSPACE)/QcomPkg/Tools/cleanup_xml.py -p $(DALCONFIG_XML_PRE) -o $(DALCONFIG_XML_OUT) -c $(DALCONFIG_C) )

$(DALCONFIG_XML_PRE) :  $(DALCONFIG_XML)
	( "$(CC)" -x assembler-with-cpp -E -o $(DALCONFIG_XML_PRE) -I$(WORKSPACE)/QcomPkg/Include/api/dal -I$(MODULE_DIR) $(INC)  $(DALCONFIG_XML) )

$(DALCONFIG_XML) :  $(INC_DEPS) $(XML_DEPS)
	( python $(WORKSPACE)/QcomPkg/Tools/cleanup_xml.py -d $(MODULE_DIR)/../DALSystem/$(MACRO_NAME) -i $(DALCONFIG_XML) -s dalsystem_$(MACRO_NAME).xml -m $(MACRO_NAME) $(XML_DEPS))


all: $(OUTPUT_DIR)/DALConfigLib.lib

#
# Build Flash Device Image
#
gen_fds:
	@"$(MAKE)" $(MAKE_FLAGS) -f $(BUILD_DIR)\Makefile fds
	@cd $(MODULE_BUILD_DIR)

#
# Target to update the FD
#
fds: mbuild gen_fds


#
# clean all intermediate files
#
clean:
	if exist $(OUTPUT_DIR) $(RD) $(OUTPUT_DIR)


#
# clean all generated files
#
cleanall:
	if exist $(DEBUG_DIR) $(RD) $(DEBUG_DIR)
	if exist $(OUTPUT_DIR) $(RD) $(OUTPUT_DIR)
	$(RM) *.pdb *.idb > NUL 2>&1
	$(RM) $(BIN_DIR)\$(MODULE_NAME).efi

#
# clean all dependent libraries built
#
cleanlib:
	@cd $(MODULE_BUILD_DIR)
