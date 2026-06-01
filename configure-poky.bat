@echo off
setlocal EnableDelayedExpansion

if not defined BRAVO_SDK (
  echo Bravo SDK not found
  exit /b 1
)

set CONFIGURE_OPTIONS=%*

set BRAVO_TOOLSET_ROOT=!BRAVO_SDK!\sysroots\x86_64-w64-mingw32
set BRAVO_SYSROOT=!BRAVO_SDK!\sysroots\armv7at2hf-neon-poky-linux-gnueabi
set "PATH=!BRAVO_TOOLSET_ROOT!\usr\bin;%PATH%"

set CONFIGURE_OPTIONS=!CONFIGURE_OPTIONS! -sTARGET_OS=linux -sTARGET_OSPLAT=arm ^
-sTOOLSET_ROOT="!BRAVO_TOOLSET_ROOT!\usr\bin\arm-poky-linux-gnueabi" ^
-sTOOLSET_PATH="!BRAVO_TOOLSET_ROOT!\usr\bin" ^
-sTOOLSET_PREFIX="arm-poky-linux-gnueabi-" ^
-sCCFLAGS="-mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard --sysroot \"!BRAVO_SYSROOT!\"" ^
-sC++FLAGS="-std=c++17 -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard --sysroot \"!BRAVO_SYSROOT!\"" ^
-sLINKFLAGS="--sysroot \"!BRAVO_SYSROOT!\" -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard" ^
-sC++LINKFLAGS="--sysroot \"!BRAVO_SYSROOT!\" -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard" --with-blend2d

set "PKG_CONFIG_PATH=!BRAVO_SYSROOT!\usr\lib\pkgconfig"

echo pkg-config --cflags freetype2:
pkg-config --cflags freetype2
REM pkg-config --libs zlib

call jam.bat configure -sCONFIG=%CONFIG% !CONFIGURE_OPTIONS!
