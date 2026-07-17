@echo off
setlocal EnableDelayedExpansion

::set POKY_SDK=C:\Program Files\Bravo SDKs\2.0.3

if not defined POKY_SDK (
  echo Poky SDK not found. Please set POKY_SDK.
  exit /b 1
)

set CONFIGURE_OPTIONS=%*

set POKY_TOOLSET_ROOT=!POKY_SDK!\sysroots\x86_64-w64-mingw32
set POKY_SYSROOT=!POKY_SDK!\sysroots\armv7at2hf-neon-poky-linux-gnueabi

set "PATH=!POKY_TOOLSET_ROOT!\usr\bin;%PATH%"
set PKG_CONFIG_LIBDIR=!POKY_SYSROOT!\usr\lib\pkgconfig;!POKY_SYSROOT!\usr\share\pkgconfig

set CONFIGURE_OPTIONS=!CONFIGURE_OPTIONS! -sTARGET_OS=linux -sTARGET_OSPLAT=arm ^
-sTOOLSET_ROOT="!POKY_TOOLSET_ROOT!\usr\bin\arm-poky-linux-gnueabi" ^
-sTOOLSET_PATH="!POKY_TOOLSET_ROOT!\usr\bin" ^
-sTOOLSET_PREFIX="arm-poky-linux-gnueabi-" ^
-sCCFLAGS="-mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard --sysroot \"!POKY_SYSROOT!\"" ^
-sC++FLAGS="-std=c++20 -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard --sysroot \"!POKY_SYSROOT!\"" ^
-sLINKFLAGS="--sysroot \"!POKY_SYSROOT!\" -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard" ^
-sC++LINKFLAGS="--sysroot \"!POKY_SYSROOT!\" -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard" --with-blend2d

call jam.bat configure !CONFIGURE_OPTIONS!
