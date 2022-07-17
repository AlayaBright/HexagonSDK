:: /* ==================================================================================== */
:: /*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
:: /*                           All Rights Reserved.                                       */
:: /*                  QUALCOMM Confidential and Proprietary                               */
:: /* ==================================================================================== */

set EXAMPLES_ROOT=%HALIDE_ROOT%\Examples
set INCLUDES=-I %HALIDE_ROOT%\include -I %EXAMPLES_ROOT%\include -I %HALIDE_ROOT%\support -I includes
set HL_LIBS=%HALIDE_ROOT%\lib\Halide.lib

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
echo === Build Android ARM-64/Hexagon executable for device ===

pushd .
cd small_lut
del /q bin\arm-64-android
del /q bin
mkdir bin
mkdir bin\arm-64-android

cl.exe %INCLUDES% pipeline.cpp %HALIDE_ROOT%\tools\GenGen.cpp /EHsc /nologo %HL_LIBS% -Febin\pipeline_generator.exe

bin\pipeline_generator.exe -g small_lut -f pipeline -o ./bin/arm-64-android target=arm-64-android-hvx_128

call %ANDROID_ARM64_TOOLCHAIN%\bin\aarch64-linux-android21-clang++ -std=c++11 %INCLUDES% -I ./bin/arm-64-android -Wall -O3 process.cpp bin/arm-64-android/pipeline.a -o bin/arm-64-android/small_lut-hvx128 -llog -fPIE -pie

%NORUN%  adb shell cp `ls /vendor/lib/rfsa/adsp/testsig*so` /data/
%NORUN%  adb push %HALIDE_ROOT%\lib\libhalide_hexagon_remote_skel.so /data/
%NORUN%  adb push %HALIDE_ROOT%\lib\arm-64-android\libhalide_hexagon_host.so /data/

%NORUN%  adb shell rm -f /data/small_lut-hvx128
%NORUN%  adb push bin/arm-64-android/small_lut-hvx128 /data/
%NORUN%  adb shell chmod +x /data/small_lut-hvx128
%NORUN%  adb shell "/data/small_lut-hvx128 10; echo status=$?" 2>&1 | tee adb.out
%NORUN%  findstr "status=0" adb.out

set result=%errorlevel%
popd
exit /B %result%
