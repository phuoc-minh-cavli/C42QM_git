#!/bin/bash
cd `dirname $0`
if [[ $1 = pack ]]
then
	python build.py BUILD_ID=ACFNAABZ,ACFNAAGZ,ACFNAAMZ,ACFNAANZ,ACFNAAFZ,ACFNAAIZ,ACENAAKZ USES_FLAGS=USES_NO_STRIP_NO_ODM && ./build_llvm.sh llvm_dam BUILD_ID=ACFNAADZ USES_FLAGS=USES_NO_STRIP_NO_ODM
elif [[ $1 = internal ]]
then 
	./build.sh all BUILD_ID=ACTNAAAH && ./build_llvm.sh llvm_dam BUILD_ID=ACTNAAAD
elif [[ $1 = packclean ]]
then 
	python build.py BUILD_ID=ACINAAAZ,ACINAATZ USES_FLAGS=USES_NO_STRIP_NO_ODM -c 
elif [[ $1 = noaudio ]]
then
	./build.sh all BUILD_ID=ACINAAAM && ./build_llvm.sh llvm_dam BUILD_ID=ACINAAAM
elif [[ $1 = damapp ]]
then 
    python build.py BUILD_ID=ACINAAAA,ACINAAAT && ./build_llvm.sh BUILD_ID=ACINAAAD
elif [[ $1 = SDK ]]
then 
	./build_llvm_sdk.sh llvm_dam BUILD_ID=ACTNAAAD  && "../../data_iot_sdk/CMCC_SDK/CMCC_build/cmcc_build.sh" && "../../data_iot_sdk/AZURE_SDK/build/azure_build.sh" && "../../data_iot_sdk/sample_apps/build/build_sample_apps.sh" && "../../data_iot_sdk/AliBaba/build/AliBaba_build.sh" && "../../data_iot_sdk/AZURE_C_EMBEDDED_SDK/build/azure_c_embedded_build.sh"
elif [[ $1 = SDK_TWIZY ]]
then 
	./build_llvm_sdk.sh llvm_dam BUILD_ID=ACTNAAAD  && "../../data_iot_sdk/CMCC_SDK/CMCC_build/cmcc_build.sh" && "../../data_iot_sdk/AZURE_SDK/build/azure_build.sh" && "../../data_iot_sdk/sample_apps/build/build_sample_apps.sh" && "../../data_iot_sdk/AliBaba/build/AliBaba_build.sh" && "../../data_iot_sdk/Tracker_ref/build/build_tracker_app.sh" && "../../data_iot_sdk/AZURE_C_EMBEDDED_SDK/build/azure_c_embedded_build.sh"
elif [[ $1 = SDK_AZURE ]]
then 
	./build_llvm_sdk.sh llvm_dam BUILD_ID=ACTNAAAD  && "../../data_iot_sdk/AZURE_SDK/build/azure_build.sh" && "../../data_iot_sdk/AZURE_C_EMBEDDED_SDK/build/azure_c_embedded_build.sh"
elif [[ $1 = '' ]]
then 
    echo "Please provide argument"
elif [[ $1 = --help ]]
then 
	echo [-packclean] for external clean build
	echo [-pack] for external build
	echo [-internalclean] for internal clean build
	echo [-internal] for internal build
	echo [-help] for HELP
fi
