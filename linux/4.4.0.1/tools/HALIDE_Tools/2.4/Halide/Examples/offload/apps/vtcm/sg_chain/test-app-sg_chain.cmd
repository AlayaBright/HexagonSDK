:: /* ==================================================================================== */
:: /*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
:: /*                           All Rights Reserved.                                       */
:: /*                  QUALCOMM Confidential and Proprietary                               */
:: /* ==================================================================================== */

set EXAMPLES_ROOT=%HALIDE_ROOT%\Examples
set INCLUDES=-I %HALIDE_ROOT%\include -I %EXAMPLES_ROOT%\include -I %HALIDE_ROOT%\support -I includes
set HL_LIBS=%HALIDE_ROOT%\lib\Halide.lib

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
echo === Build Host/Hexagon executable for simulation ===

pushd .
cd sg_chain
del /q bin\host
del /q bin
mkdir bin
mkdir bin\host

cl.exe %INCLUDES% pipeline.cpp %HALIDE_ROOT%\tools\GenGen.cpp /EHsc /nologo %HL_LIBS% -Febin\pipeline_generator.exe

bin\pipeline_generator -g sg_chain -f pipeline -o ./bin/host target=host-hvx_128

cl.exe %INCLUDES% -I bin\host process.cpp /EHsc /nologo bin\host\pipeline.lib -Febin\host\test

bin\host\test 1

set result=%errorlevel%
popd
exit /B %result%
