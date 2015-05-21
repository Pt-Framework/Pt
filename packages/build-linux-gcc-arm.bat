@ECHO OFF

if exist deploy\linux-gcc-arm-debug (
    echo removing linux-gcc-arm-debug
    call rmdir /s/q deploy\linux-gcc-arm-debug
)

call:Build -sCONFIG=linux-gcc-arm-debug --debug -sTARGET_OS=linux -sTARGET_OSPLAT=arm ^
-sTOOLSET=gcc -sTOOLSET_ROOT="%LINARO_ARM_GNUABIHF_2013_11_R1%\bin\arm-linux-gnueabihf-" ^
--with-openssl -sSYSLIBS=-lrt --with-qt5 -sQTDIR=C:\Projekte\Extern\Qt\5.2.1\linux-arm-gnuabihf

if %errorlevel% neq 0 (
    echo build failed
    goto :eof
) 

if exist deploy\linux-gcc-arm-release (
    echo removing linux-gcc-arm-release
    call rmdir /s/q deploy\linux-gcc-arm-release
)

call:Build -sCONFIG=linux-gcc-arm-release --debug --optimize -sTARGET_OS=linux -sTARGET_OSPLAT=arm ^
-sTOOLSET=gcc -sTOOLSET_ROOT="%LINARO_ARM_GNUABIHF_2013_11_R1%\bin\arm-linux-gnueabihf-" ^
--with-openssl -sSYSLIBS=-lrt --with-posix-rt --with-qt5 -sQTDIR=C:\Projekte\Extern\Qt\5.2.1\linux-arm-gnuabihf

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

    call jam -q

    if %errorlevel% neq 0 (
        echo PARALLEL BUILD FAILED, retry one CPU.
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