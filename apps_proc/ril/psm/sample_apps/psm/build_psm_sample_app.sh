#!/bin/sh

DAM_RO_BASE=0x42000000

export LLVM_ROOT_PATH=/pkg/qct/software/llvm/release/arm/4.0.3
export TOOLCHAIN_PATH=$LLVM_ROOT_PATH/bin
export LLVMLINK_PATH=$LLVM_ROOT_PATH/tools/bin
export LLVMLIB=$LLVM_ROOT_PATH/clang/4.0.3/lib
export TOOLCHAIN_PATH_STANDARDS=$LLVM_ROOT_PATH/armv7m-none-eabi/libc/include


export DAM_OUTPUT_PATH="./bin"
export DAM_INC_BASE="../../common/include"
export DAM_LIB_PATH="../../common/libs"
export DAM_SRC_PATH="./src"

export DEMO_APP_SRC_PATH="./src/demo"
export DEMO_APP_OUTPUT_PATH="./src/build"
export DAM_DEMO_LD_PATH="./build"




export DAM_LIBNAME="txm_lib.lib"
export TIMER_LIBNAME="timer_dam_lib.lib"


export DAM_ELF_NAME="psm_dam_demo.elf"
export DAM_TARGET_BIN="psm_dam_demo.bin"
export DAM_MAP_NAME="psm_dam_demo.map"
rm -rf  "$DEMO_APP_OUTPUT_PATH"

if [ ! -d "$DAM_OUTPUT_PATH" ]
  then
  mkdir -p "$DAM_OUTPUT_PATH"
	 fi



if [ ! -d "$DEMO_APP_OUTPUT_PATH" ]
then
  mkdir -p "$DEMO_APP_OUTPUT_PATH"
fi

echo "Application RO base selected = $DAM_RO_BASE"

export DAM_CPPFLAGS="-DQAPI_TXM_MODULE -DTXM_MODULE -DTX_DAM_QC_CUSTOMIZATIONS -DTX_ENABLE_PROFILING -DTX_ENABLE_EVENT_TRACE -DTX_DISABLE_NOTIFY_CALLBACKS  -DFX_FILEX_PRESENT -DTX_ENABLE_IRQ_NESTING  -DTX3_CHANGES"

export DAM_CFLAGS="-marm -target armv7m-none-musleabi -mfloat-abi=softfp -mfpu=none -mcpu=cortex-a7 -mno-unaligned-access  -fms-extensions -Osize -fshort-enums -Wbuiltin-macro-redefined"

export DAM_INCPATHS="-I $DAM_INC_BASE -I $DAM_INC_BASE/threadx_api -I $DAM_INC_BASE/qapi -I $DAM_INC_BASE/qmi -I $LLVMLIB -I $TOOLCHAIN_PATH_STANDARDS"

export DAM_APP_INCPATHS=" -I $DEMO_APP_SRC_PATH $DAM_INCPATHS"


#Turn on verbose mode by default
set -x;

echo "Compiling Demo application"

$TOOLCHAIN_PATH/clang -E  $DAM_CPPFLAGS $DAM_CFLAGS $DAM_SRC_PATH/txm_module_preamble_llvm.S > txm_module_preamble_llvm_pp.S

$TOOLCHAIN_PATH/clang -c $DAM_CPPFLAGS $DAM_CFLAGS txm_module_preamble_llvm_pp.S -o $DEMO_APP_OUTPUT_PATH/txm_module_preamble_llvm.o

if [ $? -eq 0 ]; 
then
  echo "txm_module_preamble_llvm compiled sucessfully"
rm txm_module_preamble_llvm_pp.S
$TOOLCHAIN_PATH/clang -c $DAM_CPPFLAGS $DAM_CFLAGS  $DAM_APP_INCPATHS $DEMO_APP_SRC_PATH/*.c 

    if [ $? -eq 0 ]; 
	then
	  echo "DAM APP compilation succeed"
mv *.o $DEMO_APP_OUTPUT_PATH
echo "Linking Demo application"
$TOOLCHAIN_PATH/clang++ -d -o $DAM_OUTPUT_PATH/$DAM_ELF_NAME -target armv7m-none-musleabi -fuse-ld=qcld -lc++ -Wl,-mno-unaligned-access -fuse-baremetal-sysroot -fno-use-baremetal-crt -Wl,-entry=$DAM_RO_BASE $DEMO_APP_OUTPUT_PATH/txm_module_preamble_llvm.o -Wl,-T$DAM_DEMO_LD_PATH/psm_dam_demo.ld -Wl,-Map,$DAM_OUTPUT_PATH/$DAM_MAP_NAME,-gc-sections -Wl,-gc-sections $DEMO_APP_OUTPUT_PATH/*.o $DAM_LIB_PATH/*.lib
$LLVMLINK_PATH/llvm-elf-to-hex.py  --bin $DAM_OUTPUT_PATH/$DAM_ELF_NAME --output $DAM_OUTPUT_PATH/$DAM_TARGET_BIN
echo "Demo application is built at" $DAM_OUTPUT_PATH/$DAM_TARGET_BIN
        else
          echo "Compilation failed with errors"
    fi

fi
