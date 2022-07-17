::
:: Make.cmd to build the QFE library for Windows
:: libnative.lib is 64 bit and requires the visual studio 64 bit compiler/linker
::
:: Get Hexagon tools path
@echo .
@echo off

IF NOT DEFINED SDK_SETUP_ENV (
    @echo Error! SDK Environment not set up, please run setup_sdk_env script from SDK root directory
    @GOTO End
    )

@set HEXAGON_TOOLS_DIR=%HEXAGON_SDK_ROOT%\tools\HEXAGON_Tools\8.4.05\Tools
@echo on
@set TESTFILE_NAME=qfe_test
@set MYLIB=%HEXAGON_TOOLS_DIR%\libnative\lib\libnative.lib
@set MYINCLUDE=%HEXAGON_TOOLS_DIR%\libnative\include
@set QHMATH_HVX_DIR=%HEXAGON_SDK_ROOT%\libs\qhl_hvx\src\qhmath_hvx\


@set VCTOOLS="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\"
@set MSVC_INIT=%VCTOOLS%\Auxiliary\Build\vcvarsall.bat x86_amd64
@set MSVC_CC=%VCTOOLS%\Tools\MSVC\14.16.27023\bin\Hostx64\x64\cl.exe

@set FLAGS=-D__HVX_ARCH__=68 -D__HVXDBL__=1

::
:: Important to add the path to the libnative.dll to the PATH environment var
::
@set PATH=%MYLIB%\..;%PATH%

@echo Libnative environment is now ready to execute make.cm

:Bypass_setup

@IF "%1"=="clean" GOTO Clean
@IF "%1"=="build" GOTO Build
@IF "%1"=="sim" GOTO Sim
@IF  "%1"=="list" GOTO List

:Clean
@echo.
@echo Cleaning files...
@IF EXIST bin\qfe_test.exe @DEL bin\qfe_test.exe
@IF EXIST bin\qfe_test.obj @DEL bin\qfe_test.obj
@IF EXIST bin\qfe.obj @DEL bin\qfe.obj

@IF EXIST bin\qhmath_*.obj @DEL bin\qhmath_*.obj

@IF EXIST libnative-calls* @DEL libnative-calls*
@IF "%1"=="clean" GOTO End

:Build
:: build the QFE Library
@echo.
@echo Compiling the QFE example ...
@set PATH_TEMP=%PATH%
call %MSVC_INIT%

%MSVC_CC% /MD  /c /I %MYINCLUDE% /I .\inc\ %QHMATH_HVX_DIR%\qhmath_hvx_*.c  /Fo.\bin\ %FLAGS% /link %MYLIB%

%MSVC_CC% /MD  /c /I %MYINCLUDE% /I .\inc\ src\qfe.c /Fo.\bin\ %FLAGS% /link  %MYLIB%
%MSVC_CC% /MD  /c /I %MYINCLUDE% /I .\inc\ src\qfe_test.c /Fo.\bin\ %FLAGS% /link  %MYLIB%

%MSVC_CC% /MD  /I %MYINCLUDE% /I .\inc\ .\bin\qfe_test.obj %FLAGS% /link .\bin\qfe.obj /link .\bin\qhmath_hvx_*.obj  /link %MYLIB% /out:%TESTFILE_NAME%.exe


@set PATH=%PATH_TEMP%

@IF "%1"=="build" GOTO End

:Sim
:: run the example
@echo.

@echo Running qfe_test example...
%TESTFILE_NAME%.exe
@GOTO End

:List
@echo Outputting libnative-calls.txt
@call %VCTOOLS%\bin\dumpbin.exe /ALL /OUT:libnative-calls.txt %HEXAGON_TOOLS%\libnative\lib\libnative.lib

:End
