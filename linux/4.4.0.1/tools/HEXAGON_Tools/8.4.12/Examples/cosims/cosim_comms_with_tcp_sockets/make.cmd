::###############################################################
:: Copyright (c) 2020 QUALCOMM INCORPORATED.
:: All Rights Reserved.
::################################################################
::
:: Make.cmd to build the example
:: Please note that this example requires Microsoft Visual Studio 14
:: or higher. Search for "Microsoft compiler" within this makefile to 
:: modify the variables pointing to your installation of Visual Studio.  

@echo off
@set HEXAGON_TOOLS=
@set INCLUDE=
@set LIB=
@set SYSSIM_CC=
@set Q6VERSION=v68
@set MULTISIM=1
@set SYSSIM_NAME=SysSim

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
@set SRC_TOP=%CURRENT_DIR%

@set Q6VERSION=v68
@set COSIM_TX=txCosimCfg
@set COSIM_RX=rxCosimCfg
@set TEST_NAME=TxRxTest
@set CFLAGS=-m%Q6VERSION%

@set BIN_DIR=%SRC_TOP%\bin
@set COSIM_DIR=%SRC_TOP%\cosim_src

:: Microsoft compiler to use to build cosim
@set VCTOOLS="c:\Program Files (x86)\Microsoft Visual Studio 14.0\VC"
@set SYSSIM_INIT=%VCTOOLS%\vcvarsall.bat x86_amd64
@set SYSSIM_CC=%VCTOOLS%\bin\amd64\cl.exe
@set SYSSIM_CFLAGS=/MT /O2 /nologo /Zm1000 /EHsc  -DVCPP -DLITTLE_ENDIAN /TP /I. /DCYGPC /I%INCLUDE_DIR% /I%CURRENT_DIR%\..\src
@set SYSSIM_LINK=/nologo /link libwrapper.lib /MACHINE:X64 /libpath:%HEXAGON_TOOLS%\..\lib\iss

@IF "%1"=="clean" GOTO Clean
@IF "%1"=="build" GOTO Build
@IF "%1"=="sim" GOTO Sim
@If "%1"=="cosim" GOTO Cosim

::
:: Clean up
::
:Clean
@echo.
@echo Cleaning files...
@IF EXIST pmu_stats* @DEL pmu_stats*
@IF EXIST stats.txt @DEL stats.txt
@IF EXIST %COSIM_DIR%\bdCosimCfg @DEL %COSIM_DIR%\bdCosimCfg
@IF EXIST %COSIM_DIR%\txCosimCfg @DEL %COSIM_DIR%\txCosimCfg
@IF EXIST %COSIM_DIR%\rxCosimCfg @DEL %COSIM_DIR%\rxCosimCfg
@IF EXIST %BIN_DIR% @RMDIR /S /Q %BIN_DIR%
@IF "%1"=="clean" GOTO End

::
:: Build the system simulation file
::
:Build
@echo.
@echo Compiling SysSimp.cpp...
@MKDIR %BIN_DIR%
@set PATH_TEMP=%PATH%
call %SYSSIM_INIT%
@echo %SYSSIM_CC% %SYSSIM_CFLAGS%  -c %SYSSIM_NAME%.cpp /Fo%BIN_DIR%\%SYSSIM_NAME%.obj
%SYSSIM_CC% %SYSSIM_CFLAGS%  -c %SYSSIM_NAME%.cpp /Fo%BIN_DIR%\%SYSSIM_NAME%.obj
@echo %SYSSIM_CC% %BIN_DIR%\%SYSSIM_NAME%.obj %SYSSIM_LINK% /out:%BIN_DIR%\%SYSSIM_NAME%.exe
%SYSSIM_CC% %BIN_DIR%\%SYSSIM_NAME%.obj %SYSSIM_LINK% /out:%BIN_DIR%\%SYSSIM_NAME%.exe
@set PATH=%PATH_TEMP%

::
:: Build cosim(s) and test case
::
:Cosim
@echo.
@echo Compiling cosim...
cd %COSIM_DIR%
echo %COSIM_DIR%\cosim.cmd mini_all MULTISIM=%MULTISIM%
call %COSIM_DIR%\cosim.cmd mini_all MULTISIM=%MULTISIM%
cd ..
@IF "%1"=="cosim" GOTO End
@IF "%1"=="build" GOTO End

::
:: Simulate the example
::
:Sim
@echo.
@echo Simulating example ...
@echo %BIN_DIR%\%SYSSIM_NAME%.exe %BIN_DIR%\%TEST_NAME% %COSIM_DIR%\%COSIM_TX% %COSIM_DIR%\%COSIM_RX%
%BIN_DIR%\%SYSSIM_NAME%.exe %BIN_DIR%\%TEST_NAME% %COSIM_DIR%\%COSIM_TX% %COSIM_DIR%\%COSIM_RX%

:End

::
