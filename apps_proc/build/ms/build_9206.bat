@ECHO OFF
IF /I [%1] == [] goto NOARGUMENT
IF /I %1 == packclean goto PACKCLEANBUILD
IF /I %1 == pack goto PACKBUILD
IF /I %1 == damapp goto DAMAPP
IF /I %1 == damappclean goto DAMAPPCLEAN
IF /I %1 == internalclean goto INTERNALCLEANBUILD
IF /I %1 == internal goto INTERNALBUILD
IF /I %1 == --help goto HELP
goto NOARGUMENT
:PACKCLEANBUILD
python build.py BUILD_ID=ACINAAAZ,ACINAATZ USES_FLAGS=USES_NO_STRIP_NO_ODM -c
goto END
:PACKBUILD
python build.py BUILD_ID=ACINAAAZ,ACINAATZ USES_FLAGS=USES_NO_STRIP_NO_ODM && build_llvm.cmd llvm_dam BUILD_ID=ACINAADZ USES_FLAGS=USES_NO_STRIP_NO_ODM
goto END
:INTERNALCLEANBUILD
build.cmd all BUILD_ID=ACINAAAA -c && build.cmd all BUILD_ID=ACINAAAL -c
goto END
:INTERNALBUILD
python build.py BUILD_ID=ACINAAAA,ACINAAAT && build_llvm.cmd llvm_dam BUILD_ID=ACINAAAD
goto END
:DAMAPP
python build.py BUILD_ID=ACINAAAA,ACINAAAT && build_llvm.cmd BUILD_ID=ACINAAAD
goto END
:DAMAPPCLEAN
build.cmd all BUILD_ID=ACINAAAA -c && build.cmd all BUILD_ID=ACINAAAT -c && build_llvm.cmd llvm_dam BUILD_ID=ACINAAAD -c
:NOARGUMENT
ECHO please provide argument
ECHO eg: build_9206.bat packclean
goto END
:HELP
ECHO [-packclean] for external clean build
ECHO [-pack] for external build
ECHO [-internalclean] for internal clean build
ECHO [-internal] for internal build
ECHO [-help] for HELP
:END


