::###############################################################
:: Copyright (c) 2020 QUALCOMM INCORPORATED.
:: All Rights Reserved.
::################################################################
::
:: Make.cmd to build the example
:: Please note that this example requires Microsoft Visual Studio 14
:: or higher. Search for "Microsoft compiler" within this makefile to 
:: modify the variables pointing to your installation of Visual Studio.  
::

@echo off
@set HEXAGON_TOOLS=
@set INCLUDE=
@set LIB=
@set COSIM_CC=
::@set CURRENT_DIR=
@set Q6VERSION=v68
@set PORTNUM1=10101
IF "%MULTISIM%"=="" (set MULTISIM=0)

::################################################
:: Make sure that `where` command only picks up ##
:: first path it finds, in case there are more  ##
:: than one version tools path in PATH variable ##
::################################################
@for /f "delims=" %%a in ('where hexagon-sim') do (
    @set HEXAGON_TOOLS=%%a
    @goto :done
)
:done
::################################################
:: Truncate hexagon-sim.exe from string         ##
::################################################

@for %%F in (%HEXAGON_TOOLS%) do @set dirname=%%~dpF
@set HEXAGON_TOOLS=%dirname%
@set INCLUDE_DIR=%HEXAGON_TOOLS%\..\include\iss

for /f "delims=" %%a in ('cd') do @set CURRENT_DIR=%%a
@set SRC_TOP=%CURRENT_DIR%\..

@set CC=hexagon-clang
@set SIM=hexagon-sim
@set Q6VERSION=v68
@set COSIM_NAME=TxRxCosim
@set TESTFILE_NAME=TxRxTest
@set CFLAGS=-m%Q6VERSION%

@set BIN_DIR=%SRC_TOP%\bin
@set TEST_DIR=%SRC_TOP%\hexagon_src
@set SIMFLAGS= --cosim_file bdCosimCfg --m%Q6VERSION% %BIN_DIR%\%TESTFILE_NAME% -- BD

:: Microsoft compiler to use to build cosim
@set VCTOOLS="c:\Program Files (x86)\Microsoft Visual Studio 14.0\VC"
@set COSIM_INIT=%VCTOOLS%\vcvarsall.bat x86_amd64
@set COSIM_CC=%VCTOOLS%\bin\amd64\cl.exe
@set COSIM_CFLAGS=/MT /O2 /nologo /Zm1000 /EHsc  -DVCPP -DLITTLE_ENDIAN /TP /I. /DCYGPC /I%INCLUDE_DIR% /I%CURRENT_DIR%\..\src
@set COSIM_LINK=/nologo /link /dll libwrapper.lib /MACHINE:X64 /libpath:%HEXAGON_TOOLS%\..\lib\iss

@IF "%1"=="clean" GOTO Clean
@IF "%1"=="build" GOTO Build
@IF "%1"=="sim" GOTO Sim
@IF "%1"=="mini_all" GOTO Build

::
:: Clean up
::
:Clean
@echo.
@echo Cleaning files...
@IF EXIST pmu_stats* @DEL pmu_stats*
@IF EXIST stats.txt @DEL stats.txt
@IF EXIST bdCosimCfg @DEL bdCosimCfg
@IF EXIST rxCosimCfg @DEL rxCosimCfg
@IF EXIST txCosimCfg @DEL txCosimCfg
@IF EXIST %BIN_DIR% @RMDIR /S /Q %BIN_DIR%
@IF "%1"=="clean" GOTO End

::
:: Build the cosim(s)
::
:Build
@echo.
@echo Compiling cosim ...
@MKDIR %BIN_DIR%
@set PATH_TEMP=%PATH%
call %COSIM_INIT%
@echo %COSIM_CC% %COSIM_CFLAGS%  -c %COSIM_NAME%.cpp /Fo%BIN_DIR%\%COSIM_NAME%.obj
%COSIM_CC% %COSIM_CFLAGS%  -c %COSIM_NAME%.cpp /Fo%BIN_DIR%\%COSIM_NAME%.obj
@echo %COSIM_CC% %BIN_DIR%\%COSIM_NAME%.obj %COSIM_LINK% /out:%BIN_DIR%\%COSIM_NAME%.dll
%COSIM_CC% %BIN_DIR%\%COSIM_NAME%.obj %COSIM_LINK% /out:%BIN_DIR%\%COSIM_NAME%.dll
@set PATH=%PATH_TEMP%

::
:: Copy over ..\test\cosimCfg and change .so to .dll
::
::@echo off

@setlocal
(SET var=)
@setlocal enabledelayedexpansion
FOR /f "delims=" %%x IN (%TEST_DIR%\bdCosimCfg) DO (
	CALL SET var=%%var%% %%x
	CALL SET var=!var:%COSIM_NAME%.so=%BIN_DIR%\%COSIM_NAME%.dll!
)
CALL SET var=!var:20202 RX =20202 RX ##!
FOR /f "tokens=1,2 delims=##" %%a IN ("%var%") DO ( 
    echo %%a >> bdCosimCfg
    echo %%b >> bdCosimCfg
)

(SET var1=)
FOR /f "delims=" %%y IN (%TEST_DIR%\txCosimCfg) DO (
	CALL SET var1=%%var1%% %%y
)
@echo !var1:%COSIM_NAME%.so=%BIN_DIR%\%COSIM_NAME%.dll! >> txCosimCfg

(SET var2=)
FOR /f "delims=" %%z IN (%TEST_DIR%\rxCosimCfg) DO (
	CALL SET var2=%%var2%% %%z
)
@echo !var2:%COSIM_NAME%.so=%BIN_DIR%\%COSIM_NAME%.dll! >> rxCosimCfg

::
:: Build test case
::
@echo.
@echo Compiling test ...
::cd ..\test
@echo %CC% %CFLAGS% -o %BIN_DIR%\%TESTFILE_NAME% -DMULTISIM=%MULTISIM% %TEST_DIR%\%TESTFILE_NAME%.c
%CC% %CFLAGS% -o %BIN_DIR%\%TESTFILE_NAME% -DMULTISIM=%MULTISIM% %TEST_DIR%\%TESTFILE_NAME%.c

@IF "%1"=="build" GOTO End
@IF "%1"=="mini_all" GOTO End
::
:: Simulate the example
::
:Sim
@echo.
@echo Simulating example ...
@echo %SIM% %SIMFLAGS%
%SIM% %SIMFLAGS%
::
:End

::
