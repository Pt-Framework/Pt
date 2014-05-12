@ECHO OFF

REM if exist deploy (
REM     echo removing deploy dir
REM     call rmdir /s/q deploy
REM )

REM call:Build -sCONFIG=nt-vc10-x86-debug --debug --with-openssl --with-mfc --with-qt5 -sTOOLSET=vc10 -sTARGET_OSPLAT=x86

if %errorlevel% neq 0 (
    echo build failed
    goto :eof
) 

REM call:Build -sCONFIG=nt-vc10-x86-release --debug --with-openssl --with-mfc --with-qt5 --optimize -sTOOLSET=vc10 -sTARGET_OSPLAT=x86

if %errorlevel% neq 0 (
    echo build failed
    goto :eof
) 

call:Build -sCONFIG=nt-vc10-x86_64-debug --debug --with-openssl --with-mfc -sTOOLSET=vc10 -sTARGET_OSPLAT=x86_64

if %errorlevel% neq 0 (
    echo build failed
    goto :eof
) 

call:Build -sCONFIG=nt-vc10-x86_64-release --debug --with-openssl --with-mfc --optimize -sTOOLSET=vc10 -sTARGET_OSPLAT=x86_64

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