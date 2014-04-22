#!/bin/bash
cd `dirname $0`
if [[ $1 = pack ]]
then
	python build.py BUILD_ID=ACINAAAZ,ACINAATZ USES_FLAGS=USES_NO_STRIP_NO_ODM && ./build_llvm.sh llvm_dam BUILD_ID=ACINAADZ USES_FLAGS=USES_NO_STRIP_NO_ODM
elif [[ $1 = internal ]]
then 
	python build.py BUILD_ID=ACINAAAA,ACINAAAT && ./build_llvm.sh llvm_dam BUILD_ID=ACINAAAD
elif [[ $1 = packclean ]]
then 
	python build.py BUILD_ID=ACINAAAZ,ACINAATZ USES_FLAGS=USES_NO_STRIP_NO_ODM -c 
elif [[ $1 = internalclean ]]
then
	./build.sh all BUILD_ID=ACINAAAA -c && ./build.sh all BUILD_ID=ACINAAAT -c
elif [[ $1 = damapp ]]
then 
    python build.py BUILD_ID=ACINAAAA,ACINAAAT && ./build_llvm.sh BUILD_ID=ACINAAAD
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
