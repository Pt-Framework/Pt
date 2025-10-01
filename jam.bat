@ECHO OFF

set JAMDIR=jam

if not exist %JAMDIR%\Jambase (
    echo Jambase not found, please set JAMDIR.
    goto :eof
)

if not exist %JAMDIR%\jam.exe (
    echo "jam.exe not found"

    cl.exe
    if %errorlevel% neq 0 (
        echo no cl.exe
    ) 

    goto :eof
)

call %JAMDIR%\jam.exe %*

if ERRORLEVEL 1 (
  exit /b !ERRORLEVEL!
)

