@ECHO OFF

if exist deploy\nt-vc10-x86-debug (
    echo removing nt-vc10-x86-debug
    call rmdir /s/q deploy\nt-vc10-x86-debug
)

call:Build -sCONFIG=nt-vc10-Win32-Debug --debug --with-openssl --with-mfc -sTOOLSET=vc10 -sTARGET_OSPLAT=x86

if %errorlevel% neq 0 (
    echo build failed
    goto :eof
) 

if exist deploy\nt-vc10-x86-release (
    echo removing nt-vc10-x86-release
    call rmdir /s/q deploy\nt-vc10-x86-release
)

call:Build -sCONFIG=nt-vc10-Win32-Release --debug --with-openssl --with-mfc --optimize -sTOOLSET=vc10 -sTARGET_OSPLAT=x86

if %errorlevel% neq 0 (
    echo build failed
    goto :eof
) 

if exist deploy\nt-vc10-x86_64-debug (
    echo removing nt-vc10-x86_64-debug
    call rmdir /s/q deploy\nt-vc10-x86_64-debug
)

call:Build -sCONFIG=nt-vc10-x64-Debug --debug --with-openssl --with-mfc -sTOOLSET=vc10 -sTARGET_OSPLAT=x86_64

if %errorlevel% neq 0 (
    echo build failed
    goto :eof
) 

if exist deploy\nt-vc10-x86_64-release (
    echo removing nt-vc10-x86_64-release
    call rmdir /s/q deploy\nt-vc10-x86_64-release
)

call:Build -sCONFIG=nt-vc10-x64-Release --debug --with-openssl --with-mfc --optimize -sTOOLSET=vc10 -sTARGET_OSPLAT=x86_64

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
