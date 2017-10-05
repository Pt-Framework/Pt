@ECHO OFF

if exist deploy\linux-gcc-arm-Debug (
    echo removing linux-gcc-arm-Debug
    call rmdir /s/q deploy\linux-gcc-arm-Debug
)

call:Build -sCONFIG=linux-gcc-arm-Debug --debug -sTARGET_OS=linux -sTARGET_OSPLAT=arm ^
-sTOOLSET=gcc -sTOOLSET_ROOT="%LINARO_ARM_GNUABIHF_2013_11_R1%\bin\arm-linux-gnueabihf-" ^
-sLINKFLAGS="-Wl,-rpath-link,\"%LINARO_ARM_GNUABIHF_2013_11_R1%\arm-linux-gnueabihf\lib\" -Wl,-rpath-link,\"%LINARO_ARM_GNUABIHF_2013_11_R1%\arm-linux-gnueabihf\libarm-linux-gnueabihf\"" ^
--with-openssl 

if %errorlevel% neq 0 (
    echo build failed
    goto :eof
) 

if exist deploy\linux-gcc-arm-Release (
    echo removing linux-gcc-arm-Release
    call rmdir /s/q deploy\linux-gcc-arm-Release
)

call:Build -sCONFIG=linux-gcc-arm-Release --debug --optimize -sTARGET_OS=linux -sTARGET_OSPLAT=arm ^
-sTOOLSET=gcc -sTOOLSET_ROOT="%LINARO_ARM_GNUABIHF_2013_11_R1%\bin\arm-linux-gnueabihf-" ^
-sLINKFLAGS="-Wl,-rpath-link,\"%LINARO_ARM_GNUABIHF_2013_11_R1%\arm-linux-gnueabihf\lib\" -Wl,-rpath-link,\"%LINARO_ARM_GNUABIHF_2013_11_R1%\arm-linux-gnueabihf\libarm-linux-gnueabihf\"" ^
--with-openssl 

if %errorlevel% neq 0 (
    echo build failed
    goto :eof
) 

goto :eof

REM ---------------------------------------------------------------------------
REM Build Function
REM ---------------------------------------------------------------------------

:Build

call:Clean

echo jam configure %*
call jam configure %*

call jam -q clean

if %errorlevel% neq 0 (
    echo configure failed
    goto :eof
) 

call jam -q -j4

if %errorlevel% neq 0 (

    echo PARALLEL BUILD FAILED, retry one CPU.
    set ERRORLEVEL=0
    call jam -q

    if %errorlevel% neq 0 (
        echo ERROR: build failed, giving up.
        goto :eof
    ) 
) 

call jam -q install

if %errorlevel% neq 0 (
    echo build failed
    goto :eof
) 

call:Clean

goto :eof

REM ---------------------------------------------------------------------------
REM Clean Function
REM ---------------------------------------------------------------------------

:Clean

call jam -q clean

if exist build (
    echo removing build dir
    call rmdir /s/q build
)

if exist tmp (
    echo removing tmp dir
    call rmdir /s/q tmp
)

if exist Jamrules* (
    echo removing Jamrules*
    call del /f/q Jamrules*
)

goto :eof
